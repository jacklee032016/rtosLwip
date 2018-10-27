
#include "lwipExt.h"

#include "http.h"

#if 0
typedef struct
{
	const char	*name;
	u8_t			shtml;
} default_filename;

static const default_filename _psDefaultFilenames[] =
{
	{"/index.shtml", 1 },
	{"/index.ssi",   1 },
	{"/index.shtm",  1 },
	{"/index.html",  0 },
	{"/index.htm",   0 }
};

#define NUM_DEFAULT_FILENAMES		\
				(sizeof(_psDefaultFilenames) /sizeof(default_filename) )
#endif


#if	MHTTPD_CGI || MHTTPD_CGI_SSI
/**
 * Extract URI parameters from the parameter-part of an URI in the form
 * "test.cgi?x=y" @todo: better explanation!
 * Pointers to the parameters are stored in mhc->param_vals.
 *
 * @param mhc http connection state
 * @param params pointer to the NULL-terminated parameter string from the URI
 * @return number of parameters extracted
 */
static int _extractUriParameters(ExtHttpConn *mhc, char *params)
{
	char *pair;
	char *equals;
	int loop;

	LWIP_UNUSED_ARG(mhc);

	/* If we have no parameters at all, return immediately. */
	if(!params || (params[0] == '\0'))
	{
		return(0);
	}

	/* Get a pointer to our first parameter */
	pair = params;

	/* Parse up to LWIP_HTTPD_MAX_CGI_PARAMETERS from the passed string and ignore the remainder (if any) */
	for(loop = 0; (loop < MHTTPD_MAX_CGI_PARAMETERS) && pair; loop++)
	{
		/* Save the name of the parameter */
		http_cgi_params[loop] = pair;

		/* Remember the start of this name=value pair */
		equals = pair;

		/* Find the start of the next name=value pair and replace the delimiter
		* with a 0 to terminate the previous pair string. */
		pair = strchr(pair, '&');
		if(pair)
		{
			*pair = '\0';
			pair++;
		}
		else
		{
			/* We didn't find a new parameter so find the end of the URI and replace the space with a '\0' */
			pair = strchr(equals, ' ');
			if(pair)
			{
				*pair = '\0';
			}

			/* Revert to NULL so that we exit the loop as expected. */
			pair = NULL;
		}

		/* Now find the '=' in the previous pair, replace it with '\0' and save the parameter value string. */
		equals = strchr(equals, '=');
		if(equals)
		{
			*equals = '\0';
			http_cgi_param_vals[loop] = equals + 1;
		}
		else
		{
			http_cgi_param_vals[loop] = NULL;
		}
	}

	return loop;
}
#endif

/**
 * Get the file struct for a 404 error page.
 * Tries some file names and returns NULL if none found.
 *
 * @param uri pointer that receives the actual file name URI
 * @return file struct for the error page or NULL no matching file was found
 */
static struct mfs_file *_mhttpFileGet404(ExtHttpConn *mhc)
{
	err_t err;
	const char *filename = "/404.html";

	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Opening 404 as '%s'", mhc->uri));
	err = mfsOpen(&mhc->file_handle, filename);
	if (err != ERR_OK)
	{/* 404.html doesn't exist. Try 404.htm instead. */
		filename = "/404.htm";
		err = mfsOpen(&mhc->file_handle, filename );
		if (err != ERR_OK)
		{/* 404.htm doesn't exist either. Try 404.shtml instead. */
			filename =  "/404.shtml";
			err = mfsOpen(&mhc->file_handle, filename);
			if (err != ERR_OK)
			{/* 404.htm doesn't exist either. Indicate to the caller that it should send back a default 404 page.
			*/
				return NULL;
			}
		}
	}

	snprintf(mhc->uri, sizeof(mhc->uri), filename);
	return &mhc->file_handle;
}



/** Initialize a http connection with a file to send (if found).
 * Called by mhttpFileFind and mhttpFindErrorFile.
 *
 * @param mhc http connection state
 * @param file file structure to send (or NULL if not found)
 * @param is_09 1 if the request is HTTP/0.9 (no HTTP headers in response)
 * @param uri the HTTP header URI
 * @param tag_check enable SSI tag checking
 * @param params != NULL if URI has parameters (separated by '?')
 * @return ERR_OK if file was found and mhc has been initialized correctly
 *         another err_t otherwise
 */
static err_t _mhttpFileInit(ExtHttpConn *mhc, struct mfs_file *file, u8_t tag_check, char* params)
{
	if (file != NULL)
	{
		/* file opened, initialise struct http_state */
#if	MHTTPD_SSI
		if (tag_check)
		{
			struct mhttp_ssi_state *ssi = http_ssi_state_alloc();
			if (ssi != NULL)
			{
				ssi->tag_index = 0;
				ssi->tag_state = TAG_NONE;
				ssi->parsed = file->data;
				ssi->parse_left = file->len;
				ssi->tag_end = file->data;
				mhc->ssi = ssi;
			}
		}
#else
		LWIP_UNUSED_ARG(tag_check);
#endif

		mhc->handle = file;
		mhc->file = file->data;
		EXT_ASSERT(("File length must be positive!"), (file->len >= 0));
#if	MHTTPD_CUSTOM_FILES
		if (file->is_custom_file && (file->data == NULL))
		{
			/* custom file, need to read data first (via fs_read_custom) */
			mhc->left = 0;
		}
		else
#endif

		{
			mhc->left = file->len;
		}

		mhc->retries = 0;
#if	MHTTPD_TIMING
		mhc->time_started = sys_now();
#endif

		EXT_ASSERT(("HTTP headers not included in file system"), (mhc->handle->flags & MFS_FILE_FLAGS_HEADER_INCLUDED) != 0);

#if	MHTTPD_SUPPORT_V09
		if ( mhc->isV09 && ((mhc->handle->flags & MFS_FILE_FLAGS_HEADER_INCLUDED) != 0))
		{/* HTTP/0.9 responses are sent without HTTP header,	search for the end of the header. */
			char *file_start = lwip_strnstr(mhc->file, MHTTP_CRLF MHTTP_CRLF, mhc->left);
			if (file_start != NULL)
			{
				size_t diff = file_start + 4 - mhc->file;
				mhc->file += diff;
				mhc->left -= (u32_t)diff;
			}
		}
#endif

#if	MHTTPD_CGI_SSI
		if (params != NULL)
		{/* URI contains parameters, call generic CGI handler */
			int count;
#if	MHTTPD_CGI
			if (http_cgi_paramcount >= 0)
			{
				count = http_cgi_paramcount;
			}
			else
#endif
			{
				count = _extractUriParameters(mhc, params);
			}

			httpd_cgi_handler(mhc->uri, count, http_cgi_params, http_cgi_param_vals
#if defined(MHTTPD_FILE_STATE) && MHTTPD_FILE_STATE
				, mhc->handle->state
#endif
			);
		}
#else /* MHTTPD_CGI_SSI */
		LWIP_UNUSED_ARG(params);
#endif /* MHTTPD_CGI_SSI */
	}
	else
	{
		mhc->handle = NULL;
		mhc->file = NULL;
		mhc->left = 0;
		mhc->retries = 0;
	}
	
	/* Determine the HTTP headers to send based on the file extension of the requested URI. */
	if ((mhc->handle == NULL) || ((mhc->handle->flags & MFS_FILE_FLAGS_HEADER_INCLUDED) == 0))
	{
//		mhttpAddHeaders4Uri(mhc, uri);
	}
	

#if	MHTTPD_SUPPORT_11_KEEPALIVE
	if (mhc->keepalive)
	{
#if	MHTTPD_SSI
		if (mhc->ssi != NULL)
		{
			mhc->keepalive = 0;
		}
		else
#endif

		{
			if ((mhc->handle != NULL) && ((mhc->handle->flags & (MFS_FILE_FLAGS_HEADER_INCLUDED|MFS_FILE_FLAGS_HEADER_PERSISTENT)) == MFS_FILE_FLAGS_HEADER_INCLUDED))
			{
				mhc->keepalive = 0;
			}
		}
	}
#endif /* MHTTPD_SUPPORT_11_KEEPALIVE */
	return ERR_OK;
}


/** Initialize a http connection with a file to send for an error message
 *
 * @param mhc http connection state
 * @param error_nr HTTP error number
 * @return ERR_OK if file was found and mhc has been initialized correctly
 *         another err_t otherwise
 */
err_t mhttpFindErrorFile(ExtHttpConn *mhc, u16_t error_nr)
{
	const char *uri1, *uri2, *uri3;
	err_t err;

	if (error_nr == 501)
	{
		uri1 = "/501.html";
		uri2 = "/501.htm";
		uri3 = "/501.shtml";
	}
	else
	{
		/* 400 (bad request is the default) */
		uri1 = "/404.html";
		uri2 = "/400.htm";
		uri3 = "/400.shtml";
	}
	
	err = mfsOpen(&mhc->file_handle, uri1);
	if (err != ERR_OK)
	{
		err = mfsOpen(&mhc->file_handle, uri2);
		if (err != ERR_OK)
		{
			err = mfsOpen(&mhc->file_handle, uri3);
			if (err != ERR_OK)
			{
				EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Error page for error %"U16_F" not found", error_nr));
				return ERR_ARG;
			}
		}
	}
	
	return _mhttpFileInit(mhc, &mhc->file_handle, 0, NULL);
}



/** Try to find the file specified by uri and, if found, initialize mhc accordingly.
 *
 * @param mhc the connection state
 * @param uri the HTTP header URI
 * @param is_09 1 if the request is HTTP/0.9 (no HTTP headers in response)
 * @return ERR_OK if file was found and mhc has been initialized correctly
 *         another err_t otherwise
 */
err_t extHttpFileFind(ExtHttpConn *mhc )
{
//	size_t loop;
	struct mfs_file *file = NULL;
	char *params = NULL;
	err_t err;
#if	MHTTPD_CGI
	int i;
#endif

#if 0//!MHTTPD_SSI
	const
#endif /* !M_HTTPD_SSI */
		/* By default, assume we will not be processing server-side-includes tags */
	u8_t tag_check = 0;

	/* Have we been asked for the default file (in root or a directory) ? */
	size_t uri_len = strlen(mhc->uri);

	/* case of '/' */
	if ((uri_len > 0) && (mhc->uri[uri_len-1] == '/') && ((mhc->uri != mhttpUriBuf) || (uri_len == 1)))
	{
		size_t copy_len = LWIP_MIN(sizeof(mhttpUriBuf) - 1, uri_len - 1);
		if (copy_len > 0)
		{
			MEMCPY(mhttpUriBuf, mhc->uri, copy_len);
			mhttpUriBuf[copy_len] = 0;
		}

#if 0
		/* Try each of the configured default filenames until we find one that exists. */
		for (loop = 0; loop < NUM_DEFAULT_FILENAMES; loop++)
		{
			const char* file_name;

			if (copy_len > 0)
			{
				size_t len_left = sizeof(mhttpUriBuf) - copy_len - 1;
				if (len_left > 0)
				{
					size_t name_len = strlen(_psDefaultFilenames[loop].name);
					size_t name_copy_len = LWIP_MIN(len_left, name_len);
					MEMCPY(&mhttpUriBuf[copy_len], _psDefaultFilenames[loop].name, name_copy_len);
				}

				file_name = mhttpUriBuf;
			}
			else
			{
				file_name = _psDefaultFilenames[loop].name;
			}
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Looking for '%s' for root...", file_name));

			err = mfsOpen(&mhc->file_handle, file_name);
			if(err == ERR_OK)
			{
				snprintf(mhc->uri, sizeof(mhc->uri), "%s", file_name);
				file = &mhc->file_handle;
				EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Opened '%s'!", file_name ));
#if	MHTTPD_SSI
				tag_check = _psDefaultFilenames[loop].shtml;
#endif /* LWIP_HTTPD_SSI */
				break;
			}
		}
#else
		extHttpWebPageRootHander(mhc, NULL);
#endif
	}

		
	if (file == NULL)
	{
		/* No - we've been asked for a specific file. */
		/* First, isolate the base URI (without any parameters) */
		params = (char *)strchr(mhc->uri, '?');
		if (params != NULL)
		{/* URI contains parameters. NULL-terminate the base URI */
			*params = '\0';
			params++;
		}

#if	MHTTPD_CGI
		http_cgi_paramcount = -1;
		/* Does the base URI we have isolated correspond to a CGI handler? */
		if (g_iNumCGIs && g_pCGIs)
		{
			for (i = 0; i < g_iNumCGIs; i++)
			{
				if (strcmp(mhc->uri, g_pCGIs[i].pcCGIName) == 0)
				{
					/* We found a CGI that handles this URI so extract the
					* parameters and call the handler.*/
					http_cgi_paramcount = _extractUriParameters(mhc, params);
						
					mhc->uri = g_pCGIs[i].pfnCGIHandler(i, http_cgi_paramcount, mhc->params, mhc->param_vals);
					break;
				}
			}
		}
#endif

		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Opening '%s'", mhc->uri));

		err = mfsOpen(&mhc->file_handle, mhc->uri);
		if (err == ERR_OK)
		{
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Opening '%s' OK", mhc->uri));
			file = &mhc->file_handle;
		}
		else
		{
			file = _mhttpFileGet404(mhc);
		}
#if	MHTTPD_SSI
		if (file != NULL)
		{
			/* See if we have been asked for an shtml file and, if so,	enable tag checking. */
			const char* ext = NULL, *sub;
			char* param = (char*)strstr(mhc->uri, "?");
			if (param != NULL)
			{
				/* separate uri from parameters for now, set back later */
				*param = 0;
			}
				
			sub = mhc->uri;
			ext = mhc->uri;
			for (sub = strstr(sub, "."); sub != NULL; sub = strstr(sub, "."))
			{
				ext = sub;
				sub++;
			}
				
			tag_check = 0;
			for (loop = 0; loop < NUM_MSHTML_EXTENSIONS; loop++)
			{
				if (!lwip_stricmp(ext, mSSIExtensions[loop]))
				{
					tag_check = 1;
					break;
				}
			}

			if (param != NULL)
			{
				*param = '?';
			}
		}
#endif /* MHTTPD_SSI */
	}
		
	if (file == NULL)
	{/* None of the default filenames exist so send back a 404 page */
		file = _mhttpFileGet404(mhc);
	}
		
	return _mhttpFileInit(mhc, file, tag_check, params);
}

