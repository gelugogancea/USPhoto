/* include readline */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = read(fd, &c, 1)) == 1) {
			*ptr++ = c;
			printf("Address : %x\n",ptr);
			if (rc == 0) {
			    if (n == 1)
				return(0);	/* EOF, no data read */
			    else
				break;		/* EOF, some data was read */
			}
		} else
			return(-1);	/* error */
	}

	*ptr = 0;
	return(n);
}

ssize_t
Readline(int fd, void *ptr, size_t maxlen)
{
	int n;

	if ( (n = readline(fd, ptr, maxlen)) == -1)
		printf("readline error");
	return(n);
}
