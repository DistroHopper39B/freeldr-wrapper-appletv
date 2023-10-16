#ifndef _ATVUTILS_H_
#define _ATVUTILS_H_

#include <atvint.h>

extern void	msleep(int s);
extern void	sleep(int s);
extern int		mseconds(void);
extern int		seconds(void);
//
extern char*	strcpy(char *dest, const char *src);
extern char*	strncpy(char *dest, const char *src, size_t count);
extern int		strncmp(const char *cs, const char *ct, size_t count);
extern char*	strcat(char * dest, const char * src);
extern char*	strstr(const char * s1,const char * s2);
extern size_t	strlen(const char *s);
extern void*	memcpy(void * to, const void *from, size_t n);
extern void*	memset(void *s, int c,  size_t count);
extern int		memcmp(const void *cs, const void *ct, size_t count);

#endif
