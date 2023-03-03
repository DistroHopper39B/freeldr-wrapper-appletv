#ifndef _PLAYGROUND_H_
#define _PLAYGROUND_H_

extern int sprintf(char *buf, const char *fmt, ...);
extern int printk(const char *szFormat, ...);

extern volatile uint32_t VIDEO_CURSOR_POSY;
extern volatile uint32_t VIDEO_CURSOR_POSX;
extern volatile uint32_t VIDEO_ATTR;
extern volatile CURRENT_VIDEO_MODE_DETAILS vmode;

#endif