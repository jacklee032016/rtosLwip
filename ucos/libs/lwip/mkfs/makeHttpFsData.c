/**
 * @todo:
 * - take TCP_MSS, LWIP_TCP_TIMESTAMPS and
 *   PAYLOAD_ALIGN_TYPE/PAYLOAD_ALIGNMENT as arguments
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#else
#include <dir.h>
#endif
//#include <dos.h>
#include <direct.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

/** Makefsdata can generate *all* files deflate-compressed (where file size shrinks).
 * Since nearly all browsers support this, this is a good way to reduce ROM size.
 * To compress the files, "miniz.c" must be downloaded seperately.
 */
#ifndef MMAKEFS_SUPPORT_DEFLATE
#define	MMAKEFS_SUPPORT_DEFLATE		0
#endif

#define	MCOPY_BUFSIZE		(1024*1024) /* 1 MByte */

#if MMAKEFS_SUPPORT_DEFLATE
#include "../miniz.c"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

#define my_max(a,b)		(((a) > (b)) ? (a) : (b))
#define my_min(a,b)		(((a) < (b)) ? (a) : (b))

/* COMP_OUT_BUF_SIZE is the size of the output buffer used during compression.
   COMP_OUT_BUF_SIZE must be >= 1 and <= OUT_BUF_SIZE */
#define	COMP_OUT_BUF_SIZE		MCOPY_BUFSIZE

/* OUT_BUF_SIZE is the size of the output buffer used during decompression.
   OUT_BUF_SIZE must be a power of 2 >= TINFL_LZ_DICT_SIZE (because the low-level decompressor not only writes, but reads from the output buffer as it decompresses) */
#define	OUT_BUF_SIZE				MCOPY_BUFSIZE

static uint8	s_outbuf[OUT_BUF_SIZE];
static uint8	s_checkbuf[OUT_BUF_SIZE];

/* tdefl_compressor contains all the state needed by the low-level compressor so it's a pretty big struct (~300k).
   This example makes it a global vs. putting it on the stack, of course in real-world usage you'll probably malloc() or new it. */
tdefl_compressor		g_deflator;
tinfl_decompressor		g_inflator;

int	deflate_level = 10; /* default compression level, can be changed via command line */
#define		USAGE_ARG_DEFLATE " [-defl<:compr_level>]"
#else
#define		USAGE_ARG_DEFLATE		""
#endif /* MMAKEFS_SUPPORT_DEFLATE */

/* Compatibility defines Win32 vs. DOS */
#ifdef WIN32

#define FIND_T                        WIN32_FIND_DATAA
#define FIND_T_FILENAME(fInfo)        (fInfo.cFileName)
#define FIND_T_IS_DIR(fInfo)          ((fInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
#define FIND_T_IS_FILE(fInfo)         ((fInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
#define FIND_RET_T                    HANDLE
#define FINDFIRST_FILE(path, result)  FindFirstFileA(path, result)
#define FINDFIRST_DIR(path, result)   FindFirstFileA(path, result)
#define FINDNEXT(ff_res, result)      FindNextFileA(ff_res, result)
#define FINDFIRST_SUCCEEDED(ret)      (ret != INVALID_HANDLE_VALUE)
#define FINDNEXT_SUCCEEDED(ret)       (ret == TRUE)

#define GETCWD(path, len)             GetCurrentDirectoryA(len, path)
#define CHDIR(path)                   SetCurrentDirectoryA(path)
#define CHDIR_SUCCEEDED(ret)          (ret == TRUE)

#else

#define FIND_T                        struct ffblk
#define FIND_T_FILENAME(fInfo)        (fInfo.ff_name)
#define FIND_T_IS_DIR(fInfo)          ((fInfo.ff_attrib & FA_DIREC) == FA_DIREC)
#define FIND_T_IS_FILE(fInfo)         (1)
#define FIND_RET_T                    int
#define FINDFIRST_FILE(path, result)  findfirst(path, result, FA_ARCH)
#define FINDFIRST_DIR(path, result)   findfirst(path, result, FA_DIREC)
#define FINDNEXT(ff_res, result)      FindNextFileA(ff_res, result)
#define FINDFIRST_SUCCEEDED(ret)      (ret == 0)
#define FINDNEXT_SUCCEEDED(ret)       (ret == 0)

#define GETCWD(path, len)             getcwd(path, len)
#define CHDIR(path)                   	chdir(path)
#define CHDIR_SUCCEEDED(ret)          (ret == 0)

#endif


/* define this to get the header variables we use to build HTTP headers */
#define	MHTTPD_DYNAMIC_HEADERS		1	/* define as 1 to refer variable when creating file */

#define MHTTPD_SSI             1

#include "lwip/init.h"
#include "lwip/debug.h"
#include "src/core/inet_chksum.c"
#include "src/core/def.c"

#include "lwip/apps/httpApp.h"
#include "lwip/apps/httpHeadData.c"


/** (Your server name here) */
const char *serverID = "Server: "MHTTPD_SERVER_AGENT"\r\n";

char		serverIDBuffer[1024];

/* change this to suit your MEM_ALIGNMENT */
#define	PAYLOAD_ALIGNMENT		4
/* set this to 0 to prevent aligning payload */
#define	ALIGN_PAYLOAD				1
/* define this to a type that has the required alignment */
#define PAYLOAD_ALIGN_TYPE			"unsigned int"

static int		payload_alingment_dummy_counter = 0;

#define	HEX_BYTES_PER_LINE			16

#define	MAX_PATH_LEN					256

#define	MK_OUT_FILE					"httpFsData.c"

struct mfile_entry
{
	struct mfile_entry		*next;
	const char			*filename_c;
};


/* 5 bytes per char + 3 bytes per line */
static char file_buffer_c[MCOPY_BUFSIZE * 5 + ((MCOPY_BUFSIZE / HEX_BYTES_PER_LINE) * 3)];

char curSubdir[MAX_PATH_LEN];
char lastFileVar[MAX_PATH_LEN];
char hdr_buf[4096];

unsigned char processSubs = 1;
unsigned char includeHttpHeader = 1;
unsigned char useHttp11 = 0;
unsigned char supportSsi = 1;
unsigned char precalcChksum = 0;
unsigned char includeLastModified = 0;
#if	MMAKEFS_SUPPORT_DEFLATE
unsigned char deflateNonSsiFiles = 0;
size_t deflatedBytesReduced = 0;
size_t overallDataBytes = 0;
#endif

struct mfile_entry	*first_file = NULL;
struct mfile_entry	*last_file = NULL;

static void mkfsUsage(void)
{
	printf(" Usage: htmlgen [targetdir] [-s] [-e] [-i] [-11] [-nossi] [-c] [-f:<filename>] [-m] [-svr:<name>]" USAGE_ARG_DEFLATE HTTP_NEWLINE HTTP_NEWLINE);
	printf("   targetdir: relative or absolute path to files to convert" HTTP_NEWLINE);
	printf("   switch -s: toggle processing of subdirectories (default is on)" HTTP_NEWLINE);
	printf("   switch -e: exclude HTTP header from file (header is created at runtime, default is off)" HTTP_NEWLINE);
	printf("   switch -11: include HTTP 1.1 header (1.0 is default)" HTTP_NEWLINE);
	printf("   switch -nossi: no support for SSI (cannot calculate Content-Length for SSI)" HTTP_NEWLINE);
	printf("   switch -c: precalculate checksums for all pages (default is off)" HTTP_NEWLINE);
	printf("   switch -f: target filename (default is \"fsdata.c\")" HTTP_NEWLINE);
	printf("   switch -m: include \"Last-Modified\" header based on file time" HTTP_NEWLINE);
	printf("   switch -svr: server identifier sent in HTTP response header ('Server' field)" HTTP_NEWLINE);
#if MMAKEFS_SUPPORT_DEFLATE
	printf("   switch -defl: deflate-compress all non-SSI files (with opt. compr.-level, default=10)" HTTP_NEWLINE);
	printf("                 ATTENTION: browser has to support \"Content-Encoding: deflate\"!" HTTP_NEWLINE);
#endif
	printf("   if targetdir not specified, htmlgen will attempt to" HTTP_NEWLINE);
	printf("   process files in subdirectory 'fs'" HTTP_NEWLINE);
}

int checkPath(char* path, size_t size)
{
	size_t slen;

	if (path[0] == 0)
	{/* empty */
		return 0;
	}

	slen = strlen(path);
	if (slen >= size)
	{/* not NULL-terminated */
		return 0;
	}

	while ((slen > 0) && ((path[slen] == '\\') || (path[slen] == '/')))
	{/* path should not end with trailing backslash */
		path[slen] = 0;
		slen--;
	}
	
	if (slen == 0)
	{
		return 0;
	}
	return 1;
}

static void copyFile(const char *filename_in, FILE *fout)
{
	FILE *fin;
	size_t len;
	void* buf;
	
	fin = fopen(filename_in, "rb");
	if (fin == NULL)
	{
		printf("Failed to open file \"%s\"\n", filename_in);
		exit(-1);
	}
	
	buf = malloc(MCOPY_BUFSIZE);
	while ((len = fread(buf, 1, MCOPY_BUFSIZE, fin)) > 0)
	{
		fwrite(buf, 1, len, fout);
	}
	
	free(buf);
	fclose(fin);
}

void concatFiles(const char *file1, const char *file2, const char *targetfile)
{
	FILE *fout;
	fout = fopen(targetfile, "wb");

	if (fout == NULL)
	{
		printf("Failed to open file \"%s\"\n", targetfile);
		exit(-1);
	}
	
	copyFile(file1, fout);
	copyFile(file2, fout);
	fclose(fout);
}

int filePutAscii(FILE *file, const char* ascii_string, int len, int *i)
{
	int x;
	for (x = 0; x < len; x++)
	{
		unsigned char cur = ascii_string[x];
		fprintf(file, "0x%02.2x,", cur);
		if ((++(*i) % HEX_BYTES_PER_LINE) == 0)
		{
			fprintf(file, HTTP_NEWLINE);
		}
	}
	
	return len;
}

u8_t* getFileData(const char* filename, int* file_size, int can_be_compressed, int* is_compressed)
{
	FILE *inFile;
	size_t fsize = 0;
	u8_t* buf;
	size_t r;
	int rs;
	inFile = fopen(filename, "rb");
	if (inFile == NULL)
	{
		printf("Failed to open file \"%s\"\n", filename);
		exit(-1);
	}
	
	fseek(inFile, 0, SEEK_END);
	rs = ftell(inFile);
	if (rs < 0)
	{
		printf("ftell failed with %d\n", errno);
		exit(-1);
	}
	
	fsize = (size_t)rs;
	fseek(inFile, 0, SEEK_SET);
	buf = (u8_t*)malloc(fsize);
	LWIP_ASSERT(("buf != NULL"), buf != NULL);
	
	r = fread(buf, 1, fsize, inFile);
	*file_size = fsize;
	*is_compressed = 0;
#if MAKEFS_SUPPORT_DEFLATE
	overallDataBytes += fsize;
	if (deflateNonSsiFiles)
	{
		if (can_be_compressed)
		{
			if (fsize < OUT_BUF_SIZE)
			{
				u8_t* ret_buf;
				tdefl_status status;

				size_t in_bytes = fsize;
				size_t out_bytes = OUT_BUF_SIZE;
				const void *next_in = buf;

				void *next_out = s_outbuf;
				/* create tdefl() compatible flags (we have to compose the low-level flags ourselves, or use tdefl_create_comp_flags_from_zip_params() but that means MINIZ_NO_ZLIB_APIS can't be defined). */

				mz_uint comp_flags = s_tdefl_num_probes[MZ_MIN(10, deflate_level)] | ((deflate_level <= 3) ? TDEFL_GREEDY_PARSING_FLAG : 0);
				if (!deflate_level)
				{
					comp_flags |= TDEFL_FORCE_ALL_RAW_BLOCKS;
				}

				status = tdefl_init(&g_deflator, NULL, NULL, comp_flags);
				if (status != TDEFL_STATUS_OKAY)
				{
					printf("tdefl_init() failed!\n");
					exit(-1);
				}

				memset(s_outbuf, 0, sizeof(s_outbuf));
				status = tdefl_compress(&g_deflator, next_in, &in_bytes, next_out, &out_bytes, TDEFL_FINISH);
				if (status != TDEFL_STATUS_DONE)
				{
					printf("deflate failed: %d\n", status);
					exit(-1);
				}

				LWIP_ASSERT(("out_bytes <= COPY_BUFSIZE"), out_bytes <= OUT_BUF_SIZE);

				if (out_bytes < fsize)
				{
					ret_buf = (u8_t*)malloc(out_bytes);
					LWIP_ASSERT(("ret_buf != NULL"), ret_buf != NULL);
					memcpy(ret_buf, s_outbuf, out_bytes);
					{
						/* sanity-check compression be inflating and comparing to the original */
						tinfl_status dec_status;
						tinfl_decompressor inflator;
						size_t dec_in_bytes = out_bytes;
						size_t dec_out_bytes = OUT_BUF_SIZE;
						next_out = s_checkbuf;

						tinfl_init(&inflator);
						memset(s_checkbuf, 0, sizeof(s_checkbuf));

						dec_status = tinfl_decompress(&inflator, (const mz_uint8 *)ret_buf, &dec_in_bytes, s_checkbuf, (mz_uint8 *)next_out, &dec_out_bytes, 0);
						LWIP_ASSERT(("tinfl_decompress failed"), dec_status == TINFL_STATUS_DONE);
						LWIP_ASSERT(("tinfl_decompress size mismatch"), fsize == dec_out_bytes);
						LWIP_ASSERT(("decompressed memcmp failed"), !memcmp(s_checkbuf, buf, fsize));
					}

					/* free original buffer, use compressed data + size */
					free(buf);
					buf = ret_buf;
					*file_size = out_bytes;

					printf(" - deflate: %d bytes -> %d bytes (%.02f%%)" HTTP_NEWLINE, (int)fsize, (int)out_bytes, (float)((out_bytes*100.0)/fsize));
					deflatedBytesReduced += (size_t)(fsize - out_bytes);
					*is_compressed = 1;
				}
				else
				{
					printf(" - uncompressed: (would be %d bytes larger using deflate)" HTTP_NEWLINE, (int)(out_bytes - fsize));
				}
			}
			else
			{
				printf(" - uncompressed: (file is larger than deflate bufer)" HTTP_NEWLINE);
			}
		}
		else
		{
			printf(" - SSI file, cannot be compressed" HTTP_NEWLINE);
		}
	}
#else
	LWIP_UNUSED_ARG(can_be_compressed);
#endif
	fclose(inFile);
	return buf;
}

void processFileData(FILE* data_file, u8_t* file_data, size_t file_size)
{
	size_t written, i, src_off=0;

	size_t off = 0;
	for (i = 0; i < file_size; i++)
	{
		LWIP_ASSERT(("file_buffer_c overflow"), off < sizeof(file_buffer_c) - 5);
		sprintf(&file_buffer_c[off], "0x%02.2x,", file_data[i]);
		off += 5;
		if ((++src_off % HEX_BYTES_PER_LINE) == 0)
		{
			LWIP_ASSERT(("file_buffer_c overflow"), off < sizeof(file_buffer_c) - HTTP_NEWLINE_LEN);
			memcpy(&file_buffer_c[off], HTTP_NEWLINE, HTTP_NEWLINE_LEN);
			off += HTTP_NEWLINE_LEN;
		}

		if (off + 20 >= sizeof(file_buffer_c))
		{
			written = fwrite(file_buffer_c, 1, off, data_file);
			LWIP_ASSERT(("written == off"), written == off);
			off = 0;
		}
	}
	
	written = fwrite(file_buffer_c, 1, off, data_file);
//	LWIP_ASSERT( written == off);
}

int writeChecksums(FILE *struct_file, const char *varname,
                    u16_t hdr_len, u16_t hdr_chksum, const u8_t* file_data, size_t file_size)
{
	int chunk_size = TCP_MSS;
	int offset, src_offset;
	size_t len;
	int i = 0;
#if LWIP_TCP_TIMESTAMPS
	/* when timestamps are used, usable space is 12 bytes less per segment */
	chunk_size -= 12;
#endif

	fprintf(struct_file, "#if HTTPD_PRECALCULATED_CHECKSUM" HTTP_NEWLINE);
	fprintf(struct_file, "const struct fsdata_chksum chksums_%s[] = {" HTTP_NEWLINE, varname);

	if (hdr_len > 0)
	{
		/* add checksum for HTTP header */
		fprintf(struct_file, "{%d, 0x%04x, %d}," HTTP_NEWLINE, 0, hdr_chksum, hdr_len);
		i++;
	}
	
	src_offset = 0;
	for (offset = hdr_len; ; offset += len)
	{
		unsigned short chksum;
		void* data = (void*)&file_data[src_offset];
		len = LWIP_MIN(chunk_size, (int)file_size - src_offset);
		if (len == 0)
		{
			break;
		}

		chksum = ~inet_chksum(data, (u16_t)len);
		/* add checksum for data */
		fprintf(struct_file, "{%d, 0x%04x, %d}," HTTP_NEWLINE, offset, chksum, len);
		i++;
	}
	
	fprintf(struct_file, "};" HTTP_NEWLINE);
	fprintf(struct_file, "#endif /* HTTPD_PRECALCULATED_CHECKSUM */" HTTP_NEWLINE);
	return i;
}


static int isValidChar4cVar(char x)
{
	if (((x >= 'A') && (x <= 'Z')) ||
		((x >= 'a') && (x <= 'z')) ||
		((x >= '0') && (x <= '9')) ||
		(x == '_'))
	{
		return 1;
	}
	return 0;
}

static void fixFilename4c(char* qualifiedName, size_t max_len)
{
	struct mfile_entry* f;
	size_t len = strlen(qualifiedName);
	char *new_name = (char*)malloc(len + 2);
	int filename_ok;
	int cnt = 0;
	size_t i;

	if (len + 3 == max_len)
	{
		printf("File name too long: \"%s\"\n", qualifiedName);
		exit(-1);
	}
	strcpy(new_name, qualifiedName);
	for (i = 0; i < len; i++)
	{
		if (!isValidChar4cVar(new_name[i]))
		{
			new_name[i] = '_';
		}
	}
	
	do
	{
		filename_ok = 1;
		for (f = first_file; f != NULL; f = f->next)
		{
			if (!strcmp(f->filename_c, new_name))
			{
				filename_ok = 0;
				cnt++;
				/* try next unique file name */
				sprintf(&new_name[len], "%d", cnt);
				break;
			}
		}
	} while (!filename_ok && (cnt < 999));
	
	if (!filename_ok)
	{
		printf("Failed to get unique file name: \"%s\"\n", qualifiedName);
		exit(-1);
	}
	strcpy(qualifiedName, new_name);
	free(new_name);
}

static void registerFilename(const char* qualifiedName)
{
	struct mfile_entry* fe = (struct mfile_entry*)malloc(sizeof(struct mfile_entry));
	fe->filename_c = strdup(qualifiedName);
	fe->next = NULL;
	if (first_file == NULL)
	{
		first_file = last_file = fe;
	}
	else
	{
		last_file->next = fe;
		last_file = fe;
	}
}

int isSsiFile(const char* filename)
{
	size_t loop;
	for (loop = 0; loop < NUM_MSHTML_EXTENSIONS; loop++)
	{
		if (strstr(filename, mSSIExtensions[loop]))
		{
			return 1;
		}
	}
	return 0;
}


int fileWriteHttpHeader(FILE *data_file, const char *filename, int file_size, u16_t *http_hdr_len,
                           u16_t *http_hdr_chksum, u8_t provide_content_len, int is_compressed)
{
	int i = 0;
	int response_type = MHTTP_HDR_OK;
	const char* file_type;
	const char *cur_string;
	size_t cur_len;
	int written = 0;
	size_t hdr_len = 0;
	u16_t acc;
	const char *file_ext;
	int j;
	u8_t provide_last_modified = includeLastModified;

	memset(hdr_buf, 0, sizeof(hdr_buf));

	if (useHttp11)
	{
		response_type = MHTTP_HDR_OK_11;
	}

	fprintf(data_file, HTTP_NEWLINE "/* HTTP header */");
	if (strstr(filename, "404") == filename)
	{
		response_type = MHTTP_HDR_NOT_FOUND;
		if (useHttp11)
		{
			response_type = MHTTP_HDR_NOT_FOUND_11;
		}
	}
	else if (strstr(filename, "400") == filename)
	{
		response_type = MHTTP_HDR_BAD_REQUEST;
		if (useHttp11)
		{
			response_type = MHTTP_HDR_BAD_REQUEST_11;
		}
	}
	else if (strstr(filename, "501") == filename)
	{
		response_type = MHTTP_HDR_NOT_IMPL;
		if (useHttp11)
		{
			response_type = MHTTP_HDR_NOT_IMPL_11;
		}
	}
	cur_string = mHTTPHeaderStrings[response_type];
	cur_len = strlen(cur_string);
	fprintf(data_file, HTTP_NEWLINE "/* \"%s\" (%d bytes) */" HTTP_NEWLINE, cur_string, cur_len);
	written += filePutAscii(data_file, cur_string, cur_len, &i);
	i = 0;

	if (precalcChksum)
	{
		memcpy(&hdr_buf[hdr_len], cur_string, cur_len);
		hdr_len += cur_len;
	}

	cur_string = serverID;
	cur_len = strlen(cur_string);
	fprintf(data_file, HTTP_NEWLINE "/* \"%s\" (%d bytes) */" HTTP_NEWLINE, cur_string, cur_len);
	written += filePutAscii(data_file, cur_string, cur_len, &i);
	i = 0;
	if (precalcChksum)
	{
		memcpy(&hdr_buf[hdr_len], cur_string, cur_len);
		hdr_len += cur_len;
	}

	file_ext = filename;
	if (file_ext != NULL)
	{
		while(strstr(file_ext, ".") != NULL)
		{
			file_ext = strstr(file_ext, ".");
			file_ext++;
		}
	}
	
	if ((file_ext == NULL) || (*file_ext == 0))
	{
		printf("failed to get extension for file \"%s\", using default.\n", filename);
		file_type = MHTTP_HDR_DEFAULT_TYPE;
	}
	else
	{
		file_type = NULL;
		for (j = 0; j < NUM_MHTTP_HEADERS; j++)
		{
			if (!strcmp(file_ext, mHTTPHeaders[j].extension))
			{
				file_type = mHTTPHeaders[j].contentType;
				break;
			}
		}

		if (file_type == NULL)
		{
			printf("failed to get file type for extension \"%s\", using default.\n", file_ext);
			file_type = MHTTP_HDR_DEFAULT_TYPE;
		}
	}

	/* Content-Length is used for persistent connections in HTTP/1.1 but also for
	download progress in older versions
	@todo: just use a big-enough buffer and let the HTTPD send spaces? */
	if (provide_content_len)
	{
		char intbuf[MAX_PATH_LEN];
		int content_len = file_size;
		memset(intbuf, 0, sizeof(intbuf));
		cur_string = mHTTPHeaderStrings[MHTTP_HDR_CONTENT_LENGTH];
		cur_len = strlen(cur_string);
		fprintf(data_file, HTTP_NEWLINE "/* \"%s%d\r\n\" (%d+ bytes) */" HTTP_NEWLINE, cur_string, content_len, cur_len+2);
		written += filePutAscii(data_file, cur_string, cur_len, &i);
		if (precalcChksum)
		{
			memcpy(&hdr_buf[hdr_len], cur_string, cur_len);
			hdr_len += cur_len;
		}

		_itoa(content_len, intbuf, 10);
		strcat(intbuf, "\r\n");
		cur_len = strlen(intbuf);
		written += filePutAscii(data_file, intbuf, cur_len, &i);
		i = 0;

		if (precalcChksum)
		{
			memcpy(&hdr_buf[hdr_len], intbuf, cur_len);
			hdr_len += cur_len;
		}
	}
	
	if (provide_last_modified)
	{
		char modbuf[256];
		struct stat stat_data;
		struct tm* t;
		memset(modbuf, 0, sizeof(modbuf));
		memset(&stat_data, 0, sizeof(stat_data));
		cur_string = modbuf;
		strcpy(modbuf, "Last-Modified: ");
		if (stat(filename, &stat_data) != 0)
		{
			printf("stat(%s) failed with error %d\n", filename, errno);
			exit(-1);
		}

		t = gmtime(&stat_data.st_mtime);
		if (t == NULL)
		{
			printf("gmtime() failed with error %d\n", errno);
			exit(-1);
		}

		strftime(&modbuf[15], sizeof(modbuf)-15, "%a, %d %b %Y %H:%M:%S GMT", t);
		cur_len = strlen(cur_string);
		fprintf(data_file, HTTP_NEWLINE "/* \"%s\"\r\n\" (%d+ bytes) */" HTTP_NEWLINE, cur_string, cur_len+2);
		written += filePutAscii(data_file, cur_string, cur_len, &i);
		if (precalcChksum)
		{
			memcpy(&hdr_buf[hdr_len], cur_string, cur_len);
			hdr_len += cur_len;
		}


		modbuf[0] = 0;
		strcat(modbuf, "\r\n");
		cur_len = strlen(modbuf);
		written += filePutAscii(data_file, modbuf, cur_len, &i);

		i = 0;
		if (precalcChksum)
		{
			memcpy(&hdr_buf[hdr_len], modbuf, cur_len);
			hdr_len += cur_len;
		}
	}

	/* HTTP/1.1 implements persistent connections */
	if (useHttp11)
	{
		if (provide_content_len)
		{
			cur_string = mHTTPHeaderStrings[MHTTP_HDR_CONN_KEEPALIVE];
		}
		else
		{
			/* no Content-Length available, so a persistent connection is no possible
			because the client does not know the data length */
			cur_string = mHTTPHeaderStrings[MHTTP_HDR_CONN_CLOSE];
		}

		cur_len = strlen(cur_string);
		fprintf(data_file, HTTP_NEWLINE "/* \"%s\" (%d bytes) */" HTTP_NEWLINE, cur_string, cur_len);
		written += filePutAscii(data_file, cur_string, cur_len, &i);
		i = 0;

		if (precalcChksum)
		{
			memcpy(&hdr_buf[hdr_len], cur_string, cur_len);
			hdr_len += cur_len;
		}
	}

#if MMAKEFS_SUPPORT_DEFLATE
	if (is_compressed)
	{/* tell the client about the deflate encoding */
		LWIP_ASSERT(("error"), deflateNonSsiFiles);
		cur_string = "Content-Encoding: deflate\r\n";
		cur_len = strlen(cur_string);
		fprintf(data_file, HTTP_NEWLINE "/* \"%s\" (%d bytes) */" HTTP_NEWLINE, cur_string, cur_len);
		written += filePutAscii(data_file, cur_string, cur_len, &i);
		i = 0;
	}
#else
	LWIP_UNUSED_ARG(is_compressed);
#endif

	/* write content-type, ATTENTION: this includes the double-CRLF! */
	cur_string = file_type;
	cur_len = strlen(cur_string);
	fprintf(data_file, HTTP_NEWLINE "/* \"%s\" (%d bytes) */" HTTP_NEWLINE, cur_string, cur_len);
	written += filePutAscii(data_file, cur_string, cur_len, &i);
	i = 0;

	/* ATTENTION: headers are done now (double-CRLF has been written!) */

	if (precalcChksum)
	{
		memcpy(&hdr_buf[hdr_len], cur_string, cur_len);
		hdr_len += cur_len;

		LWIP_ASSERT(("hdr_len <= 0xffff"), hdr_len <= 0xffff);
		LWIP_ASSERT(("strlen(hdr_buf) == hdr_len"), strlen(hdr_buf) == hdr_len);
		acc = ~inet_chksum(hdr_buf, (u16_t)hdr_len);
		*http_hdr_len = (u16_t)hdr_len;
		*http_hdr_chksum = acc;
	}

	return written;
}

int processFile(FILE *data_file, FILE *struct_file, const char *filename)
{
	char varname[MAX_PATH_LEN];
	int i = 0;
	char qualifiedName[MAX_PATH_LEN];
	int file_size;
	u16_t http_hdr_chksum = 0;
	u16_t http_hdr_len = 0;
	int chksum_count = 0;
	u8_t flags = 0;
	const char* flags_str;
	u8_t has_content_len;
	u8_t* file_data;
	int is_compressed = 0;

	/* create qualified name (@todo: prepend slash or not?) */
	sprintf(qualifiedName,"%s/%s", curSubdir, filename);
	/* create C variable name */
	strcpy(varname, qualifiedName);
	
	/* convert slashes & dots to underscores */
	fixFilename4c(varname, MAX_PATH_LEN);
	registerFilename(varname);
	
#if ALIGN_PAYLOAD
	/* to force even alignment of array, type 1 */
	fprintf(data_file, "#if MFSDATA_FILE_ALIGNMENT==1" HTTP_NEWLINE);
	fprintf(data_file, "static const " PAYLOAD_ALIGN_TYPE " dummy_align_%s = %d;" HTTP_NEWLINE, varname, payload_alingment_dummy_counter++);
	fprintf(data_file, "#endif" HTTP_NEWLINE);
#endif /* ALIGN_PAYLOAD */
	fprintf(data_file, "static const unsigned char MFSDATA_ALIGN_PRE data_%s[] MFSDATA_ALIGN_POST = {" HTTP_NEWLINE, varname);
	/* encode source file name (used by file system, not returned to browser) */
	fprintf(data_file, "/* %s (%d chars) */" HTTP_NEWLINE, qualifiedName, strlen(qualifiedName)+1);
	
	filePutAscii(data_file, qualifiedName, strlen(qualifiedName)+1, &i);
#if ALIGN_PAYLOAD
	/* pad to even number of bytes to assure payload is on aligned boundary */
	while(i % PAYLOAD_ALIGNMENT != 0)
	{
		fprintf(data_file, "0x%02.2x,", 0);
		i++;
	}
#endif /* ALIGN_PAYLOAD */
	fprintf(data_file, HTTP_NEWLINE);

	has_content_len = !isSsiFile(filename);
	file_data = getFileData(filename, &file_size, includeHttpHeader && has_content_len, &is_compressed);
	
	if (includeHttpHeader)
	{
		fileWriteHttpHeader(data_file, filename, file_size, &http_hdr_len, &http_hdr_chksum, has_content_len, is_compressed);
		flags = MFS_FILE_FLAGS_HEADER_INCLUDED;
		if (has_content_len)
		{
			flags |= MFS_FILE_FLAGS_HEADER_PERSISTENT;
		}
	}
	
	if (precalcChksum)
	{
		chksum_count = writeChecksums(struct_file, varname, http_hdr_len, http_hdr_chksum, file_data, file_size);
	}

	/* build declaration of struct fsdata_file in temp file */
	fprintf(struct_file, "const struct mfsdata_file file_%s[] = { {" HTTP_NEWLINE, varname);
	fprintf(struct_file, "file_%s," HTTP_NEWLINE, lastFileVar);
	fprintf(struct_file, "data_%s," HTTP_NEWLINE, varname);
	fprintf(struct_file, "data_%s + %d," HTTP_NEWLINE, varname, i);
	fprintf(struct_file, "sizeof(data_%s) - %d," HTTP_NEWLINE, varname, i);
	
	switch(flags)
	{
		case(MFS_FILE_FLAGS_HEADER_INCLUDED):
			flags_str = "MFS_FILE_FLAGS_HEADER_INCLUDED";
			break;
		case(MFS_FILE_FLAGS_HEADER_PERSISTENT):
			flags_str = "MFS_FILE_FLAGS_HEADER_PERSISTENT";
			break;
		case(MFS_FILE_FLAGS_HEADER_INCLUDED | MFS_FILE_FLAGS_HEADER_PERSISTENT):
			flags_str = "MFS_FILE_FLAGS_HEADER_INCLUDED | MFS_FILE_FLAGS_HEADER_PERSISTENT";
			break;
		default:
			flags_str = "0";
			break;
	}
	
	fprintf(struct_file, "%s," HTTP_NEWLINE, flags_str);
	if (precalcChksum)
	{
		fprintf(struct_file, "#if MHTTPD_PRECALCULATED_CHECKSUM" HTTP_NEWLINE);
		fprintf(struct_file, "%d, chksums_%s," HTTP_NEWLINE, chksum_count, varname);
		fprintf(struct_file, "#endif /* MHTTPD_PRECALCULATED_CHECKSUM */" HTTP_NEWLINE);
	}
	
	fprintf(struct_file, "}};" HTTP_NEWLINE HTTP_NEWLINE);
	strcpy(lastFileVar, varname);

	/* write actual file contents */
	i = 0;
	fprintf(data_file, HTTP_NEWLINE "/* raw file data (%d bytes) */" HTTP_NEWLINE, file_size);
	
	processFileData(data_file, file_data, file_size);
	fprintf(data_file, "};" HTTP_NEWLINE HTTP_NEWLINE);
	free(file_data);
	return 0;
}



int processSub(FILE *data_file, FILE *struct_file)
{
	FIND_T fInfo;
	FIND_RET_T fret;
	int filesProcessed = 0;

	if (processSubs)
	{
			/* process subs recursively */
		size_t sublen = strlen(curSubdir);
		size_t freelen = sizeof(curSubdir) - sublen - 1;
		LWIP_ASSERT(("sublen < sizeof(curSubdir)"), sublen < sizeof(curSubdir));
		fret = FINDFIRST_DIR("*", &fInfo);

		if (FINDFIRST_SUCCEEDED(fret))
		{
			do
			{
				const char *curName = FIND_T_FILENAME(fInfo);
				if ((curName[0] == '.') || (strcmp(curName, "CVS") == 0))
				{
					continue;
				}

				if (!FIND_T_IS_DIR(fInfo))
				{
					continue;
				}

				if (freelen > 0)
				{
					CHDIR(curName);
					strncat(curSubdir, "/", freelen);
					strncat(curSubdir, curName, freelen - 1);
					curSubdir[sizeof(curSubdir) - 1] = 0;
					printf("processing subdirectory %s/..." HTTP_NEWLINE, curSubdir);
					
					filesProcessed += processSub(data_file, struct_file);
					CHDIR("..");
					curSubdir[sublen] = 0;
				}
				else
				{
					printf("WARNING: cannot process sub due to path length restrictions: \"%s/%s\"\n", curSubdir, curName);
				}
			} while (FINDNEXT_SUCCEEDED(FINDNEXT(fret, &fInfo)));
		}
	}

	fret = FINDFIRST_FILE("*.*", &fInfo);
	if (FINDFIRST_SUCCEEDED(fret))
	{
		/* at least one file in directory */
		do
		{
			if (FIND_T_IS_FILE(fInfo))
			{
				const char *curName = FIND_T_FILENAME(fInfo);
				printf("processing %s/%s..." HTTP_NEWLINE, curSubdir, curName);
				if (processFile(data_file, struct_file, curName) < 0)
				{
					printf(HTTP_NEWLINE "Error... aborting" HTTP_NEWLINE);
					return -1;
				}
				filesProcessed++;
			}
		} while (FINDNEXT_SUCCEEDED(FINDNEXT(fret, &fInfo)));
	}
	
	return filesProcessed;
}

int sPutAscii(char *buf, const char *ascii_string, int len, int *i)
{
	int x;
	int idx = 0;

	for (x = 0; x < len; x++)
	{
		unsigned char cur = ascii_string[x];
		sprintf(&buf[idx], "0x%02.2x,", cur);
		idx += 5;
		if ((++(*i) % HEX_BYTES_PER_LINE) == 0)
		{
			sprintf(&buf[idx], HTTP_NEWLINE);
			idx += HTTP_NEWLINE_LEN;
		}
	}
	
	return len;
}


int main(int argc, char *argv[])
{
	char path[MAX_PATH_LEN];
	char appPath[MAX_PATH_LEN];
	FILE *data_file;
	FILE *struct_file;
	int filesProcessed;
	int i;
	
	char targetfile[MAX_PATH_LEN];
	strcpy(targetfile, MK_OUT_FILE);

	memset(path, 0, sizeof(path));
	memset(appPath, 0, sizeof(appPath));

	printf(HTTP_NEWLINE " makefsdata - HTML to C source converter" HTTP_NEWLINE);
//	printf("     by Jim Pettinato               - circa 2003 " HTTP_NEWLINE);
//	printf("     extended by Simon Goldschmidt  - 2009 " HTTP_NEWLINE HTTP_NEWLINE);

	strcpy(path, "fs");
	for (i = 1; i < argc; i++)
	{
		if (argv[i] == NULL)
		{
			continue;
		}
		
		if (argv[i][0] == '-')
		{
			if (strstr(argv[i], "-svr:") == argv[i])
			{
				snprintf(serverIDBuffer, sizeof(serverIDBuffer), "Server: %s\r\n", &argv[i][5]);
				serverID = serverIDBuffer;
				printf("Using Server-ID: \"%s\"\n", serverID);
			}
			else if (strstr(argv[i], "-s") == argv[i])
			{
				processSubs = 0;
			}
			else if (strstr(argv[i], "-e") == argv[i])
			{
				includeHttpHeader = 0;
			}
			else if (strstr(argv[i], "-11") == argv[i])
			{
				useHttp11 = 1;
			}
			else if (strstr(argv[i], "-nossi") == argv[i])
			{
				supportSsi = 0;
			}
			else if (strstr(argv[i], "-c") == argv[i])
			{
				precalcChksum = 1;
			}
			else if (strstr(argv[i], "-f:") == argv[i])
			{
				strncpy(targetfile, &argv[i][3], sizeof(targetfile) - 1);
				targetfile[sizeof(targetfile) - 1] = 0;
				printf("Writing to file \"%s\"\n", targetfile);
			}
			else if (strstr(argv[i], "-m") == argv[i])
			{
				includeLastModified = 1;
			}
			else if (strstr(argv[i], "-defl") == argv[i])
			{
#if MMAKEFS_SUPPORT_DEFLATE
				char* colon = strstr(argv[i], ":");
				if (colon)
				{
					if (colon[1] != 0)
					{
						int defl_level = atoi(&colon[1]);
						if ((defl_level >= 0) && (defl_level <= 10))
						{
							deflate_level = defl_level;
						}
						else
						{
							printf("ERROR: deflate level must be [0..10]" HTTP_NEWLINE);
							exit(0);
						}
					}
				}

				deflateNonSsiFiles = 1;
				printf("Deflating all non-SSI files with level %d (but only if size is reduced)" HTTP_NEWLINE, deflate_level);
#else
				printf("WARNING: Deflate support is disabled\n");
#endif
			}
			else if ((strstr(argv[i], "-?")) || (strstr(argv[i], "-h")))
			{
				mkfsUsage();
				exit(0);
			}
		}
		else if ((argv[i][0] == '/') && (argv[i][1] == '?') && (argv[i][2] == 0))
		{
			mkfsUsage();
			exit(0);
		}
		else
		{
			strncpy(path, argv[i], sizeof(path)-1);
			path[sizeof(path)-1] = 0;
		}
	}

	if (!checkPath(path, sizeof(path)))
	{
		printf("Invalid path: \"%s\"." HTTP_NEWLINE, path);
		exit(-1);
	}

	GETCWD(appPath, MAX_PATH_LEN);
	/* if command line param or subdir named 'fs' not found spout usage verbiage */
	if (!CHDIR_SUCCEEDED(CHDIR(path)))
	{/* if no subdir named 'fs' (or the one which was given) exists, spout usage verbiage */
		printf(" Failed to open directory \"%s\"." HTTP_NEWLINE HTTP_NEWLINE, path);
		mkfsUsage();
		exit(-1);
	}
	CHDIR(appPath);

	printf("HTTP %sheader will %s statically included." HTTP_NEWLINE, 	(includeHttpHeader ? (useHttp11 ? "1.1 " : "1.0 ") : ""), (includeHttpHeader ? "be" : "not be"));

	sprintf(curSubdir, "");  /* start off in web page's root directory - relative paths */
	printf("  Processing all files in directory %s", path);
	if (processSubs)
	{
		printf(" and subdirectories..." HTTP_NEWLINE HTTP_NEWLINE);
	}
	else
	{
		printf("..." HTTP_NEWLINE HTTP_NEWLINE);
	}

	data_file = fopen("fsdata.tmp", "wb");
	if (data_file == NULL)
	{
		printf("Failed to create file \"fsdata.tmp\"\n");
		exit(-1);
	}
	
	struct_file = fopen("fshdr.tmp", "wb");
	if (struct_file == NULL)
	{
		printf("Failed to create file \"fshdr.tmp\"\n");
		fclose(data_file);
		exit(-1);
	}

	CHDIR(path);

	fprintf(data_file, "#include \"lwip/apps/httpApp.h\"" HTTP_NEWLINE);
	fprintf(data_file, "#include \"lwip/def.h\"" HTTP_NEWLINE);
//	fprintf(data_file, "#include \"lwip/apps/fsdata.h\"" HTTP_NEWLINE HTTP_NEWLINE HTTP_NEWLINE);

	fprintf(data_file, "#define file_NULL (struct mfsdata_file *) NULL" HTTP_NEWLINE HTTP_NEWLINE HTTP_NEWLINE);
	/* define FS_FILE_FLAGS_HEADER_INCLUDED to 1 if not defined (compatibility with older httpd/fs) */
	fprintf(data_file, "#ifndef FS_FILE_FLAGS_HEADER_INCLUDED" HTTP_NEWLINE "#define FS_FILE_FLAGS_HEADER_INCLUDED 1" HTTP_NEWLINE "#endif" HTTP_NEWLINE);
	/* define FS_FILE_FLAGS_HEADER_PERSISTENT to 0 if not defined (compatibility with older httpd/fs: wasn't supported back then) */
	fprintf(data_file, "#ifndef FS_FILE_FLAGS_HEADER_PERSISTENT" HTTP_NEWLINE "#define FS_FILE_FLAGS_HEADER_PERSISTENT 0" HTTP_NEWLINE "#endif" HTTP_NEWLINE);

	/* define alignment defines */
#if ALIGN_PAYLOAD
	fprintf(data_file, "/* MFSDATA_FILE_ALIGNMENT: 0=off, 1=by variable, 2=by include */" HTTP_NEWLINE "#ifndef MFSDATA_FILE_ALIGNMENT" HTTP_NEWLINE "#define MFSDATA_FILE_ALIGNMENT 0" HTTP_NEWLINE "#endif" HTTP_NEWLINE);
#endif
	fprintf(data_file, "#ifndef MFSDATA_ALIGN_PRE"  HTTP_NEWLINE "#define MFSDATA_ALIGN_PRE"  HTTP_NEWLINE "#endif" HTTP_NEWLINE);
	fprintf(data_file, "#ifndef MFSDATA_ALIGN_POST" HTTP_NEWLINE "#define MFSDATA_ALIGN_POST" HTTP_NEWLINE "#endif" HTTP_NEWLINE);
#if ALIGN_PAYLOAD
	fprintf(data_file, "#if MFSDATA_FILE_ALIGNMENT==2" HTTP_NEWLINE "#include \"mfsdata_alignment.h\"" HTTP_NEWLINE "#endif" HTTP_NEWLINE);
#endif

	sprintf(lastFileVar, "NULL");

	filesProcessed = processSub(data_file, struct_file);

	/* data_file now contains all of the raw data.. now append linked list of
	* file header structs to allow embedded app to search for a file name */
	fprintf(data_file, HTTP_NEWLINE HTTP_NEWLINE);
	fprintf(struct_file, "#define MFS_ROOT \t\tfile_%s" HTTP_NEWLINE, lastFileVar);
	fprintf(struct_file, "#define MFS_NUMFILES \t\t%d" HTTP_NEWLINE HTTP_NEWLINE, filesProcessed);

	fclose(data_file);
	fclose(struct_file);

	CHDIR(appPath);
	/* append struct_file to data_file */
	printf(HTTP_NEWLINE "Creating target file..." HTTP_NEWLINE HTTP_NEWLINE);
	concatFiles("fsdata.tmp", "fshdr.tmp", targetfile);

	/* if succeeded, delete the temporary files */
	if (remove("fsdata.tmp") != 0)
	{
		printf("Warning: failed to delete fsdata.tmp\n");
	}
	
	if (remove("fshdr.tmp") != 0)
	{
		printf("Warning: failed to delete fshdr.tmp\n");
	}

	printf(HTTP_NEWLINE "Processed %d files - done." HTTP_NEWLINE, filesProcessed);
#if MMAKEFS_SUPPORT_DEFLATE
	if (deflateNonSsiFiles)
	{
		printf("(Deflated total byte reduction: %d bytes -> %d bytes (%.02f%%)" HTTP_NEWLINE, (int)overallDataBytes, (int)deflatedBytesReduced, (float)((deflatedBytesReduced*100.0)/overallDataBytes));
	}
#endif
	printf(HTTP_NEWLINE);

	while (first_file != NULL)
	{
		struct mfile_entry* fe = first_file;
		first_file = fe->next;
		free(fe);
	}

	return 0;
}

