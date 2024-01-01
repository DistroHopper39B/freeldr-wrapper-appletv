/* https://github.com/loop333/atv-bootloader/blob/master/utils.c **************/

#include <fldrwrapper.h>

/*
 * time.c:
 * sleep.c:
 *
 * Copyright (c) 2007 Mythic-Beasts LTD ( http://www.mythic-beasts.com ),
 * All rights reserved. Written by James McKenzie <macmini@madingley.org>.
 *
 */

/**********************************************************************/
void
msleep(int s)
{
	s += mseconds();

	while ( mseconds() < s);
}
/**********************************************************************/
void
sleep(int s)
{
	msleep(s * 1000);
}

/**********************************************************************/
/**********************************************************************/
static uint32_t		l0, h0;
static int			set = 0;

int
mseconds(void)
{
	uint32_t	l, q;
	uint32_t	h;
	int			r;

	if (!set) {
		__asm__ __volatile__ ("rdtsc\n":"=a" (l0), "=d" (h0):);
		set++;
	}

	__asm__ __volatile__ ("rdtsc\n":"=a" (l), "=d" (h):);


	r = (h - h0) << 12;

	q = l;
	q >>= 20;
	r += q;

	q = l0;
	q >>= 20;
	r -= q;

	return r;
}
/**********************************************************************/
int
seconds(void)
{
	return(mseconds() / 1000);
}

/**********************************************************************/
/**********************************************************************/
// these come from linux/arch/i386/lib/string.c
//  Copyright (C) 1991, 1992  Linus Torvalds
/**********************************************************************/
char* strcpy(char *dest, const char *src)
{
	char *tmp = dest;

	while ((*dest++ = *src++) != '\0')
		/* nothing */;
		
	return tmp;
}
/*
char * strcpy(char * dest, const char *src)
{
	int d0, d1, d2;
	__asm__ __volatile__(
       		"1:\tlodsb\n\t"
        	"stosb\n\t"
       		"testb %%al,%%al\n\t"
       		"jne 1b"
    		: "=&S" (d0), "=&D" (d1), "=&a" (d2)
     		:"0" (src),"1" (dest) : "memory");
	return dest;
}
*/
/**********************************************************************/
char* strncpy(char *dest, const char *src, size_t count)
{
	char *tmp = dest;

	while (count-- && (*dest++ = *src++) != '\0')
		/* nothing */;

	return tmp;
}
 /*
char *strncpy(char * dest, const char *src, size_t count)
{
	int d0, d1, d2, d3;
	asm volatile( "1:\tdecl %2\n\t"
		"js 2f\n\t"
		"lodsb\n\t"
		"stosb\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b\n\t"
		"rep\n\t"
		"stosb\n"
		"2:"
		: "=&S" (d0), "=&D" (d1), "=&c" (d2), "=&a" (d3)
		:"0" (src),"1" (dest),"2" (count) : "memory");
	return dest;
}
*/
/**********************************************************************/
int strncmp(const char *cs, const char *ct, size_t count)
{
	register signed char __res = 0;

	while (count) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
		count--;
	}

	return __res;
}
/*
int strncmp(const char * cs, const char * ct, size_t count)
{
	int res;
	int d0, d1, d2;
	asm volatile( "1:\tdecl %3\n\t"
		"js 2f\n\t"
		"lodsb\n\t"
		"scasb\n\t"
		"jne 3f\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b\n"
		"2:\txorl %%eax,%%eax\n\t"
		"jmp 4f\n"
		"3:\tsbbl %%eax,%%eax\n\t"
		"orb $1,%%al\n"
		"4:"
		:"=a" (res), "=&S" (d0), "=&D" (d1), "=&c" (d2)
		:"1" (cs),"2" (ct),"3" (count)
		:"memory");
	return res;
}
*/
/**********************************************************************/
char * strcat(char * dest, const char * src)
{
	char *tmp = dest;

	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0')
		;

	return tmp;
}
/**********************************************************************/
char * strstr(const char * s1,const char * s2)
{
	int l1, l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *) s1;
	l1 = strlen(s1);
	while (l1 >= l2) {
		l1--;
		if (!memcmp(s1,s2,l2))
			return (char *) s1;
		s1++;
	}
	return NULL;
}
/**********************************************************************/
size_t strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
		
	return sc - s;
}
/*
size_t strlen(const char *s)
{
	int d0;
	register int __res;
	__asm__ __volatile__(
        	"repne\n\t"
	       	"scasb\n\t"
        	"notl %0\n\t"
       		"decl %0"
		:"=c" (__res), "=&D" (d0) :"1" (s),"a" (0), "0" (0xffffffffu));
	return __res;
}
*/
/**********************************************************************/
void* memcpy(void *dest, const void *src, size_t count)
{
	char *tmp = (char *) dest, *s = (char *) src;

	while (count--)
		*tmp++ = *s++;

	return dest;
}
/*
void * memcpy(void * to, const void * from, size_t n)
{
	int d0, d1, d2;
	__asm__ __volatile__(
       		"rep ; movsl\n\t"
       		"testb $2,%b4\n\t"
      		"je 1f\n\t"
      	 	"movsw\n"
      		"1:\ttestb $1,%b4\n\t"
     		"je 2f\n\t"
     		"movsb\n"
     	  	"2:"
     		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		:"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from)
	       	: "memory");
	return (to);
}
*/
/**********************************************************************/
void* memset(void *s, int c, size_t count)
{
	char *xs = (char *) s;

	while (count--)
		*xs++ = c;
		
	return s;
}
/*
void * memset(void *s, int c,  size_t count)
{
  	int d0, d1;
	__asm__ __volatile__(
	        "rep\n\t"
	        "stosb"
	        : "=&c" (d0), "=&D" (d1)
	        :"a" (c),"1" (s),"0" (count)
	        :"memory");
	return s;
}
*/
/**********************************************************************/
int memcmp(const void *cs,const void *ct, size_t count)
{
	const unsigned char *su1, *su2;
	int					res = 0;

	for( su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
		if ((res = *su1 - *su2) != 0) break;
	return res;
}

/* https://github.com/loop333/atv-bootloader/blob/master/darwin_code.c ********/

const struct section *getsectbynamefromheader(struct mach_header *mhp, const char *segname, const char *sectname)
{
    struct segment_command *sgp;
    struct section         *sp;
    long                   i, j;

    //debug_printf("getsectbynamefromheader\n");
    sgp = (struct segment_command *) ((char *) mhp + sizeof(struct mach_header));
    for (i = 0; i < mhp->ncmds; i++) {
        if (sgp->cmd == LC_SEGMENT) {
            //debug_printf("sgp->segname = %s, matching to %s.\n", sgp->segname, segname);

            //if (strncmp(sgp->segname, segname, sizeof(sgp->segname)) == 0 || mhp->filetype == MH_OBJECT) {
            if (strncmp(sgp->segname, segname, strlen(sgp->segname)) == 0 || mhp->filetype == MH_OBJECT) {
                sp = (struct section *) ((char *) sgp + sizeof(struct segment_command));
                for (j = 0; j < sgp->nsects; j++) {
                    // if (strncmp(sp->sectname, sectname, sizeof(sp->sectname)) == 0 &&
                    //  strncmp(sp->segname,  segname,  sizeof(sp->segname)) == 0) {
                    if (strncmp(sp->sectname, sectname, strlen(sp->sectname)) == 0 &&
                        strncmp(sp->segname, segname, strlen(sp->segname)) == 0) {
                        debug_printf("Found %s,%s ", segname, sectname);
                        return (sp);
                    }
                    //
                    sp = (struct section *)((char *) sp + sizeof(struct section));
                }
            }
        }
        sgp = (struct segment_command *)((char *)sgp + sgp->cmdsize);
    }
    ChangeColors(0xFFFF0000, 0x00000000);
    printf("FATAL: Could not find FreeLoader in executable!\n", segname, sectname);
    fail();
    //
    return ((struct section *) 0);
}
/**********************************************************************/
// This routine returns the a pointer to the data for the named section in the
// named segment if it exist in the mach header passed to it.  Also it returns
// the size of the section data indirectly through the pointer size.  Otherwise
//  it returns zero for the pointer and the size.
char *getsectdatafromheader(struct mach_header *mhp, const char *segname, const char *sectname, unsigned long *size)
{
    const struct section	*sp;

    // debug_printf("getsectdatafromheader\n");
    sp = getsectbynamefromheader(mhp, segname, sectname);
    if (sp == (struct section *) 0) {
        *size = 0;
        return ((char*) 0);
    }
    *size = sp->size;
    //
    debug_printf("@ 0x%08X size %i\n", sp->addr, sp->size);
    return ((char *) (sp->addr));
}
