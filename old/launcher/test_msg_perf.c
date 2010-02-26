/*
 * Copyright © 2008 Nokia Corporation
 *
 * Author: Guillem Jover <guillem.jover@nokia.com>
 *
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <unistd.h>
#include <time.h>

#include "comm_msg.h"
#include "report.h"

int
main()
{
	comm_msg_t *msg;
	uint32_t w = 0x87654321;
	const int i_max = 100000, j_max = 100;
	int i, j;
	clock_t t_ini, t_end;
	float secs;

	/* Serializing. */

	msg = comm_msg_new(j_max * 2 * sizeof(uint32_t), 0);

	t_ini = clock();

	for (i = 0; i < i_max; i++)
	{
		for (j = 0; j < j_max; j++)
			comm_msg_put_int(msg, w);

		comm_msg_reset(msg);
	}

	t_end = clock();

	comm_msg_destroy(msg);

	secs = (float)(t_end - t_ini) / (float)CLOCKS_PER_SEC;

	info("time needed to pack %d words: %f\n", i_max * j_max, secs);

	return 0;
}

