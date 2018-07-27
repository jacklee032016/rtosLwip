/*
 *
 */

#include "lwip/apps/muxHttpOpts.h"
#include "lwip/def.h"
#include "lwip/apps/muxHttpFs.h"
#include <string.h>


#if MHTTPD_USE_CUSTOM_FSDATA
#include "mfsdata_custom.c"
#else
#include	"muxHttpFsData.c"
#endif


#if MHTTPD_CUSTOM_FILES
	int mfsOpenCustom(struct mfs_file *file, const char *name);
	void mfsCloseCustom(struct mfs_file *file);
	#if MHTTPD_FS_ASYNC_READ
		u8_t mfsCanReadCustom(struct mfs_file *file);
		u8_t mfsWaitReadCustom(struct mfs_file *file, fs_wait_cb callback_fn, void *callback_arg);
		int mfsReadAsyncCustom(struct mfs_file *file, char *buffer, int count, mfs_wait_cb callback_fn, void *callback_arg);
	#else
		int mfsReadCustom(struct mfs_file *file, char *buffer, int count);
	#endif
#endif


err_t mfsOpen(struct mfs_file *file, const char *name)
{
	const struct mfsdata_file *f;

	if ((file == NULL) || (name == NULL))
	{
		return ERR_ARG;
	}

#if	MHTTPD_CUSTOM_FILES
	if (mfsOpenCustom(file, name))
	{
		file->is_custom_file = 1;
		return ERR_OK;
	}
	file->is_custom_file = 0;
#endif

	for (f = MFS_ROOT; f != NULL; f = f->next)
	{
		if (!strcmp(name, (const char *)f->name))
		{
			file->data = (const char *)f->data;
			file->len = f->len;
			file->index = f->len;
			file->pextension = NULL;
			file->flags = f->flags;
#if MHTTPD_PRECALCULATED_CHECKSUM
			file->chksum_count = f->chksum_count;
			file->chksum = f->chksum;
#endif

#if MHTTPD_FILE_STATE
			file->state = mfsStateInit(file, name);
#endif
			return ERR_OK;
		}
	}

	/* file not found */
	return ERR_VAL;
}


void mfsClose(struct mfs_file *file)
{
#if MHTTPD_CUSTOM_FILES
	if (file->is_custom_file)
	{
		mfsCloseCustom(file);
	}
	
#endif

#if MHTTPD_FILE_STATE
	mfsStateFree(file, file->state);
#endif
	LWIP_UNUSED_ARG(file);
}


#if MHTTPD_DYNAMIC_FILE_READ
#if MHTTPD_FS_ASYNC_READ
int mfsReadAsync(struct mfs_file *file, char *buffer, int count, mfs_wait_cb callback_fn, void *callback_arg)
#else
int mfsRead(struct mfs_file *file, char *buffer, int count)
#endif
{
	int read;
	if(file->index == file->len)
	{
		return FS_READ_EOF;
	}
	
#if	MHTTPD_FS_ASYNC_READ
	LWIP_UNUSED_ARG(callback_fn);
	LWIP_UNUSED_ARG(callback_arg);
#endif

#if 	MHTTPD_CUSTOM_FILES
	if (file->is_custom_file)
	{
#if 	MHTTPD_FS_ASYNC_READ
		return mfsReadAsyncCustom(file, buffer, count, callback_fn, callback_arg);
#else
		return mfsReadCustom(file, buffer, count);
#endif
	}
#endif

	read = file->len - file->index;
	if(read > count)
	{
		read = count;
	}

	MEMCPY(buffer, (file->data + file->index), read);
	file->index += read;

	return(read);
}
#endif


#if MHTTPD_FS_ASYNC_READ
int mfsIsFileReady(struct mfs_file *file, mfs_wait_cb callback_fn, void *callback_arg)
{
	if (file != NULL)
	{
#if	MHTTPD_FS_ASYNC_READ
#if	MHTTPD_CUSTOM_FILES
		if (!mfsCanReadCustom(file))
		{
			if (mfsWaitReadCustom(file, callback_fn, callback_arg))
			{
				return 0;
			}
		}
#else
		LWIP_UNUSED_ARG(callback_fn);
		LWIP_UNUSED_ARG(callback_arg);
#endif
#endif
	}
	
	return 1;
}
#endif


int mfsBytesLeft(struct mfs_file *file)
{
	return file->len - file->index;
}

