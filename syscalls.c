/* In XDC Tools v3_25_03_72, these were in
 * gnu.targets.arm.rtsv7M/syscalls.c, as expected, but in v3_25_05_94
 * they are no longer there. Adding them here for now.
 */

#include <errno.h>
#undef errno
extern int errno;

/*
 *  ======== _kill ========
 */
int _kill(int pid, int sig) 
{
    errno = EINVAL;
    return (-1);
}

/*
 *  ======== _getpid ========
 */
int _getpid(void) 
{
    return (1);
}
