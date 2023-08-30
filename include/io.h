/*
 * io.h:
 *
 * Copyright (c) 2006 James McKenzie <james@fishsoup.dhs.org>,
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*
 * $Id: io.h,v 1.3 2007/04/26 00:25:10 james Exp $
 */

/*
 * $Log: io.h,v $
 * Revision 1.3  2007/04/26 00:25:10  james
 * *** empty log message ***
 *
 * Revision 1.2  2007/04/19 23:41:17  james
 * *** empty log message ***
 *
 * Revision 1.1  2007/04/19 23:05:37  james
 * *** empty log message ***
 *
 * Revision 1.2  2007/02/28 20:05:27  james
 * *** empty log message ***
 *
 * Revision 1.1  2006/05/02 19:22:56  james
 * *** empty log message ***
 *
 */

#ifndef __IO_H__
#define __IO_H__

#include "atvint.h"

static inline void
outb (uint8_t value, uint16_t port)
{
    __asm__ __volatile__ ("out" "b" " %" "b" "0,%" "w" "1"::"a" (value),
    "Nd" (port));
}

static inline void
outw (uint16_t value, uint16_t port)
{
    __asm__ __volatile__ ("out" "w" " %" "w" "0,%" "w" "1"::"a" (value),
    "Nd" (port));
}

static inline void
outl (uint32_t value, uint16_t port)
{
    __asm__ __volatile__ ("out" "l" " %" "0,%" "w" "1"::"a" (value),
    "Nd" (port));
}

static inline uint8_t
inb (uint16_t port)
{
    uint8_t _v;
    __asm__ __volatile__ ("in" "b" " %" "w" "1,%" "" "0":"=a" (_v):"Nd" (port));
    return _v;
}

static inline uint16_t
inw (uint16_t port)
{
    uint16_t _v;
    __asm__ __volatile__ ("in" "w" " %" "w" "1,%" "" "0":"=a" (_v):"Nd" (port));
    return _v;
}

static inline uint32_t
inl (uint16_t port)
{
    uint32_t _v;
    __asm__ __volatile__ ("in" "l" " %" "w" "1,%" "" "0":"=a" (_v):"Nd" (port));
    return _v;
}



static inline void
in_string_16 (uint16_t port, void *addr, uint32_t count)
{
    __asm__ __volatile__ ("cld ; rep ; ins" "w":"=D" (addr),
    "=c" (count):"d" (port), "0" (addr), "1" (count));

}


#endif /* __IO_H__ */
