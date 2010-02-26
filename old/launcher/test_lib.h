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

#ifndef TEST_LIB_H
#define TEST_LIB_H

#include <stdbool.h>
#include <stdint.h>

void tests_init(int total);
bool tests_summary(void);
void test_success(bool r);
void test_failure(bool r);
void test_cmp_int(uint32_t a, uint32_t b);
void test_cmp_str(const char *a, const char *b);

#endif

