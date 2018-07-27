/**
 * System-specific implementation of the _read function used by the standard library.
 */

#include "compact.h"

/**
 * \defgroup group_common_utils_stdio Standard I/O (stdio)
 *
 * Common standard I/O driver that implements the stdio
 * read and write functions on AVR and SAM devices.
 *
 * \{
 */

void (*ptr_get)(void volatile*, char*);

volatile void *volatile stdio_base;
int (*ptr_put)(void volatile*, char);



// IAR common implementation
#if ( defined(__ICCAVR32__) || defined(__ICCAVR__) || defined(__ICCARM__) )

#include <yfuns.h>

_STD_BEGIN

#pragma module_name = "?__read"

/*! \brief Reads a number of bytes, at most \a size, into the memory area
 *         pointed to by \a buffer.
 *
 * \param handle File handle to read from.
 * \param buffer Pointer to buffer to write read bytes to.
 * \param size Number of bytes to read.
 *
 * \return The number of bytes read, \c 0 at the end of the file, or
 *         \c _LLIO_ERROR on failure.
 */
size_t __read(int handle, unsigned char *buffer, size_t size)
{
	int nChars = 0;
	// This implementation only reads from stdin.
	// For all other file handles, it returns failure.
	if (handle != _LLIO_STDIN) {
		return _LLIO_ERROR;
	}
	for (; size > 0; --size) {
		ptr_get(stdio_base, (char*)buffer);
		buffer++;
		nChars++;
	}
	return nChars;
}

/*! \brief This routine is required by IAR DLIB library since EWAVR V6.10
 * the implementation is empty to be compatible with old IAR version.
 */
int __close(int handle)
{
	UNUSED(handle);
	return 0;
}

/*! \brief This routine is required by IAR DLIB library since EWAVR V6.10
 * the implementation is empty to be compatible with old IAR version.
 */
int remove(const char* val)
{
	UNUSED(val);
	return 0;
}

/*! \brief This routine is required by IAR DLIB library since EWAVR V6.10
 * the implementation is empty to be compatible with old IAR version.
 */
long __lseek(int handle, long val, int val2)
{
	UNUSED(handle);
	UNUSED(val2);
	return val;
}

_STD_END

// GCC AVR32 and SAM implementation
#elif (defined(__GNUC__) && !XMEGA && !MEGA) 

int __attribute__((weak))
_read (int file, char * ptr, int len); // Remove GCC compiler warning

int __attribute__((weak))
_read (int file, char * ptr, int len)
{
	int nChars = 0;

	if (file != 0) {
		return -1;
	}

	for (; len > 0; --len) {
		ptr_get(stdio_base, ptr);
		ptr++;
		nChars++;
	}
	return nChars;
}

// GCC AVR implementation
#elif (defined(__GNUC__) && (XMEGA || MEGA) )

int _read (int *f); // Remove GCC compiler warning

int _read (int *f)
{
	char c;
	ptr_get(stdio_base,&c);
	return c;
}
#endif


#if ( defined(__ICCAVR32__) || defined(__ICCAVR__) || defined(__ICCARM__))

#include <yfuns.h>

_STD_BEGIN

#pragma module_name = "?__write"

/*! \brief Writes a number of bytes, at most \a size, from the memory area
 *         pointed to by \a buffer.
 *
 * If \a buffer is zero then \ref __write performs flushing of internal buffers,
 * if any. In this case, \a handle can be \c -1 to indicate that all handles
 * should be flushed.
 *
 * \param handle File handle to write to.
 * \param buffer Pointer to buffer to read bytes to write from.
 * \param size Number of bytes to write.
 *
 * \return The number of bytes written, or \c _LLIO_ERROR on failure.
 */
size_t __write(int handle, const unsigned char *buffer, size_t size)
{
	size_t nChars = 0;

	if (buffer == 0) {
		// This means that we should flush internal buffers.
		return 0;
	}

	// This implementation only writes to stdout and stderr.
	// For all other file handles, it returns failure.
	if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
		return _LLIO_ERROR;
	}

	for (; size != 0; --size) {
		if (ptr_put(stdio_base, *buffer++) < 0) {
			return _LLIO_ERROR;
		}
		++nChars;
	}
	return nChars;
}

_STD_END


#elif (defined(__GNUC__) && !XMEGA && !MEGA)

int __attribute__((weak))
_write (int file, const char *ptr, int len);

int __attribute__((weak))
_write (int file, const char *ptr, int len)
{
	int nChars = 0;

	if ((file != 1) && (file != 2) && (file!=3)) {
		return -1;
	}

	for (; len != 0; --len) {
		if (ptr_put(stdio_base, *ptr++) < 0) {
			return -1;
		}
		++nChars;
	}
	return nChars;
}

#elif (defined(__GNUC__) && (XMEGA || MEGA))

int _write (char c, int *f);

int _write (char c, int *f)
{
	if (ptr_put(stdio_base, c) < 0) {
		return -1;
	}
	return 1;
}
#endif

/**
 * \}
 */



