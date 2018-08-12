
#ifndef __HTTP_FILESYSTEM_H__
#define __HTTP_FILESYSTEM_H__

#include "lwip/apps/httpOpts.h"
#include "lwip/err.h"

#if MHTTPD_PRECALCULATED_CHECKSUM
struct 	mhttpdata_chksum
{
	u32_t	offset;
	u16_t	chksum;
	u16_t	len;
};
#endif

struct mfsdata_file
{
	const struct mfsdata_file		*next;
	const unsigned char			*name;
	const unsigned char			*data;
	int							len;
	u8_t							flags;
#if MHTTPD_PRECALCULATED_CHECKSUM
	u16_t						chksum_count;
	const struct mhttpdata_chksum	*chksum;
#endif
};


#define	MFS_READ_EOF							-1
#define	MFS_READ_DELAYED						-2



#define	MFS_FILE_FLAGS_HEADER_INCLUDED		0x01
#define	MFS_FILE_FLAGS_HEADER_PERSISTENT	0x02

struct mfs_file
{
	const char					*data;
	int							len;
	int							index;
	
	void							*pextension;
#if MHTTPD_PRECALCULATED_CHECKSUM
	const struct mhttpdata_chksum	*chksum;
	u16_t						chksum_count;
#endif

	u8_t							flags;
#if	MHTTPD_CUSTOM_FILES
	u8_t							is_custom_file;
#endif

#if	MHTTPD_FILE_STATE
	void			*state;
#endif
};

#if	MHTTPD_FS_ASYNC_READ
typedef void (*mfs_wait_cb)(void *arg);
#endif

err_t	mfsOpen(struct mfs_file *file, const char *name);
void		mfsClose(struct mfs_file *file);
int mfsBytesLeft(struct mfs_file *file);

#if 	MHTTPD_DYNAMIC_FILE_READ
#if	MHTTPD_FS_ASYNC_READ
int mfsReadAsync(struct mfs_file *file, char *buffer, int count, mfs_wait_cb callback_fn, void *callback_arg);
#else
int mfsRead(struct mfs_file *file, char *buffer, int count);
#endif
#endif

#if	MHTTPD_FS_ASYNC_READ
int mfsIsFileReady(struct mfs_file *file, mfs_wait_cb callback_fn, void *callback_arg);
#endif

#if 	MHTTPD_FILE_STATE
/** This user-defined function is called when a file is opened. */
void *mfsStateInit(struct mfs_file *file, const char *name);
/** This user-defined function is called when a file is closed. */
void mfsStateFree(struct mfs_file *file, void *state);
#endif


#endif

