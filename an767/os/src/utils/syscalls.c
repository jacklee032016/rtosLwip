/**
 *
 * \brief Syscalls for SAM (GCC).
 *
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>


#undef errno
extern int errno;
extern int _end;
extern int __ram_end__;

extern caddr_t _sbrk(int incr);
extern int link(char *old, char *new);
extern int _close(int file);
extern int _fstat(int file, struct stat *st);
extern int _isatty(int file);
extern int _lseek(int file, int ptr, int dir);
extern void _exit(int status);
extern void _kill(int pid, int sig);
extern int _getpid(void);

extern caddr_t _sbrk(int incr)
{
	static unsigned char *heap = NULL;
	unsigned char *prev_heap;
	int ramend = (int)&__ram_end__;

	if (heap == NULL) {
		heap = (unsigned char *)&_end;
	}
	prev_heap = heap;

	if (((int)prev_heap + incr) > ramend) {
		return (caddr_t) -1;	
	}

	heap += incr;

	return (caddr_t) prev_heap;
}

extern int link(char *old, char *new)
{
	( void ) old;
	( void ) new;
	return -1;
}

extern int _close(int file)
{
	( void ) file;
	return -1;
}

extern int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	( void ) file;

	return 0;
}

extern int _isatty(int file)
{
	( void ) file;
	return 1;
}

extern int _lseek(int file, int ptr, int dir)
{
	( void ) file;
	( void ) ptr;
	( void ) dir;
	return 0;
}

extern void _exit(int status)
{
	printf("Exiting with status %d.\n", status);

	for (;;);
}

extern void _kill(int pid, int sig)
{
	( void ) pid;
	( void ) sig;
	return;
}

extern int _getpid(void)
{
	return -1;
}


