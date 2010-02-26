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

#include <string.h>

#include "report.h"
#include "test_lib.h"

static int tests_ok, tests_total;

void
tests_init(int total)
{
	tests_ok = 0;
	tests_total = total;
}

bool
tests_summary(void)
{
	bool pass = tests_total == tests_ok;
	const char *outcome = pass ? "ok" : "fail";

	info("Tests pass (%d/%d): %s\n", tests_ok, tests_total, outcome);

	return pass;
}

void
test_success(bool r)
{
	if (r)
		++tests_ok;
	else
		error("got %d expected %d\n", r, true);
}

void
test_failure(bool r)
{
	if (!r)
		++tests_ok;
	else
		error("got %d expected %d\n", r, false);
}

void
test_cmp_int(uint32_t a, uint32_t b)
{
	if (a == b)
		++tests_ok;
	else
		error("got %0#8x expected %0#8x\n", b, a);
}

void
test_cmp_str(const char *a, const char *b)
{
	if (strcmp(a, b) == 0)
		++tests_ok;
	else
		error("got %s expected %s\n", b, a);
}

