/*
 * Copyright (c) 2005 Martin Decky
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

/** @addtogroup lc Libc
 * @brief HelenOS C library
 * @{
 * @}
 */

/** @addtogroup libc generic
 * @ingroup lc
 * @{
 */

/** @file
 */

#include <errno.h>
#include <libc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <tls.h>
#include <fibril.h>
#include <task.h>
#include <loader/pcb.h>
#include <vfs/vfs.h>
#include <vfs/inbox.h>
#include "private/libc.h"
#include "private/async.h"
#include "private/malloc.h"
#include "private/io.h"
#include "private/fibril.h"

#ifdef CONFIG_RTLD
#include <rtld/rtld.h>
#endif

static bool env_setup = false;

/**
 * Used for C++ constructors/destructors
 * and the GCC constructor/destructor extension.
 */
typedef void (*init_array_entry_t)();
extern init_array_entry_t __init_array_start[];
extern init_array_entry_t __init_array_end[];
typedef void (*fini_array_entry_t)();
extern fini_array_entry_t __fini_array_start[];
extern fini_array_entry_t __fini_array_end[];

void __libc_main(void *pcb_ptr)
{
	/* Initialize user task run-time environment */
	__malloc_init();

	/* Save the PCB pointer */
	__pcb = (pcb_t *) pcb_ptr;

#ifdef CONFIG_RTLD
	if (__pcb != NULL && __pcb->rtld_runtime != NULL) {
		runtime_env = (rtld_t *) __pcb->rtld_runtime;
	} else {
		if (rtld_init_static() != EOK)
			abort();
	}
#endif

	fibril_t *fibril = fibril_setup();
	if (fibril == NULL)
		abort();

	__tcb_set(fibril->tcb);

	__async_server_init();
	__async_client_init();
	__async_ports_init();

	/* The basic run-time environment is setup */
	env_setup = true;

	int argc;
	char **argv;

	/*
	 * Get command line arguments and initialize
	 * standard input and output
	 */
	if (__pcb == NULL) {
		argc = 0;
		argv = NULL;
		__stdio_init();
	} else {
		argc = __pcb->argc;
		argv = __pcb->argv;
		__inbox_init(__pcb->inbox, __pcb->inbox_entries);
		__stdio_init();
		vfs_root_set(inbox_get("root"));
		(void) vfs_cwd_set(__pcb->cwd);
	}

	/*
	 * C++ Static constructor calls.
	 */
	ptrdiff_t init_array_entries = (__init_array_end - __init_array_start);

	for (int i = init_array_entries - 1; i > 0; --i)
		__init_array_start[i]();

	/*
	 * Run main() and set task return value
	 * according the result
	 */
	int retval = main(argc, argv);
	exit(retval);
}

void __libc_exit(int status)
{
	/*
	 * GCC extension __attribute__((destructor)),
	 * C++ destructors are added to __cxa_finalize call
	 * when the respective constructor is called.
	 */
	ptrdiff_t fini_array_entries = (__fini_array_end - __fini_array_start);

	for (int i = 0; i < fini_array_entries; ++i)
		__fini_array_start[i]();

	if (env_setup) {
		__stdio_done();
		task_retval(status);
		fibril_teardown(__tcb_get()->fibril_data, false);
	}

	__SYSCALL1(SYS_TASK_EXIT, false);

	/* Unreachable */
	while (true)
		;
}

void __libc_abort(void)
{
	__SYSCALL1(SYS_TASK_EXIT, true);

	/* Unreachable */
	while (true)
		;
}

/** @}
 */
