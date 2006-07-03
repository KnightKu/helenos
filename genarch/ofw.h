/*
 * Copyright (C) 2005 Martin Decky
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __OFW_H__
#define __OFW_H__

#include <types.h>
#include <stdarg.h>

#define BUF_SIZE		1024

#define MEMMAP_MAX_RECORDS 	32

typedef struct {
	void *start;
	unsigned int size;
} memzone_t;

typedef struct {
	unsigned int total;
	unsigned int count;
	memzone_t zones[MEMMAP_MAX_RECORDS];
} memmap_t;

typedef struct {
	void *addr;
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	unsigned int scanline;
} screen_t;

typedef struct {
	void *addr;
	unsigned int size;
} keyboard_t;

typedef struct {
	unsigned int info;
	unsigned int addr_hi;
	unsigned int addr_lo;
} pci_addr_t;

typedef struct {
	pci_addr_t addr;
	unsigned int size_hi;
	unsigned int size_lo;
} pci_reg_t;

typedef unsigned long ofw_arg_t;
typedef unsigned int ihandle;
typedef unsigned int phandle;

extern phandle ofw_aliases;

extern void init(void);
extern void ofw_write(const char *str, const int len);

extern int ofw_get_property(const phandle device, const char *name, const void *buf, const int buflen);
extern phandle ofw_find_device(const char *name);

extern void *ofw_translate(const void *virt);
extern int ofw_map(const void *phys, const void *virt, const int size, const int mode);
extern int ofw_memmap(memmap_t *map);
extern int ofw_screen(screen_t *screen);
extern int ofw_keyboard(keyboard_t *keyboard);

#endif
