/*
 * Copyright (c) 2023 Jiri Svoboda
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

/** @addtogroup display
 * @{
 */
/**
 * @file Display server input device configuration
 */

#include <adt/list.h>
#include <errno.h>
#include <io/log.h>
#include <loc.h>
#include <stdlib.h>
#include "display.h"
#include "idevcfg.h"

/** Create input device configuration entry.
 *
 * @param display Parent display
 * @param svc_id Device service ID
 * @param seat Seat to which device is assigned
 * @param ridevcfg Place to store pointer to new input device configuration
 *                 entry
 * @return EOK on success, ENOMEM if out of memory
 */
errno_t ds_idevcfg_create(ds_display_t *display, service_id_t svc_id,
    ds_seat_t *seat, ds_idevcfg_t **ridevcfg)
{
	ds_idevcfg_t *idevcfg;

	idevcfg = calloc(1, sizeof(ds_idevcfg_t));
	if (idevcfg == NULL)
		return ENOMEM;

	idevcfg->svc_id = svc_id;
	idevcfg->seat = seat;

	ds_display_add_idevcfg(display, idevcfg);
	*ridevcfg = idevcfg;
	return EOK;
}

/** Destroy input device configuration entry.
 *
 * @param ddev Display device
 */
void ds_idevcfg_destroy(ds_idevcfg_t *idevcfg)
{
	ds_display_remove_idevcfg(idevcfg);
	free(idevcfg);
}

/** @}
 */
