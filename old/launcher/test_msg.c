/*
 * Copyright © 2008 Nokia Corporation
 *
 * Authors: Guillem Jover <guillem.jover@nokia.com>
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
#include <sys/types.h>
#include <fcntl.h>

#include "comm_msg.h"
#include "report.h"
#include "test_lib.h"

const char *test_file = "test_msg.out";

int
main()
{
	int fd;
	uint32_t magic = 0xffff5555;
	uint32_t w1 = 0xcafef00d, w2 = 0xdeadbeef, w3 = 0xd00d, w4 = 0xbabe;
	const char *s1 = "0123456789abcdef, end here.";
	const char *s2 = "some other more reasonable string";
	uint32_t w;
	const char *s;
	comm_msg_t *msg;

	/* Serializing. */

	msg = comm_msg_new(20, 0);

	comm_msg_put_magic(msg, magic);

	comm_msg_put_int(msg, w1);
	comm_msg_put_int(msg, w2);
	comm_msg_put_str(msg, s1);
	comm_msg_put_str(msg, s2);
	comm_msg_put_int(msg, w3);
	comm_msg_put_int(msg, w4);

	fd = open(test_file, O_WRONLY | O_CREAT, 0644);
	if (fd < 0)
		die(1, "cannot create file");

	comm_msg_send(fd, msg);
	comm_msg_destroy(msg);

	close(fd);

	/* Deserializing. */

	msg = comm_msg_new(5, 0);

	fd = open(test_file, O_RDONLY, 0644);
	if (fd < 0)
		die(2, "cannot open file");

	comm_msg_recv(fd, msg);

	tests_init(8);

	if (comm_msg_get_magic(msg, &w))
		test_cmp_int(magic, w);

	if (comm_msg_get_int(msg, &w))
		test_cmp_int(w1, w);
	if (comm_msg_get_int(msg, &w))
		test_cmp_int(w2, w);
	if (comm_msg_get_str(msg, &s))
		test_cmp_str(s1, s);
	if (comm_msg_get_str(msg, &s))
		test_cmp_str(s2, s);
	if (comm_msg_get_int(msg, &w))
		test_cmp_int(w3, w);
	if (comm_msg_get_int(msg, &w))
		test_cmp_int(w4, w);

	comm_msg_destroy(msg);

	close(fd);

	unlink(test_file);

	/* Capped buffer. */

	msg = comm_msg_new(4, 8);

	comm_msg_put_int(msg, w1);
	comm_msg_put_int(msg, w2);
	test_failure(comm_msg_put_int(msg, w3));

	comm_msg_destroy(msg);

	return tests_summary() ? 0 : 1;
}

