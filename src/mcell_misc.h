/******************************************************************************
 *
 * Copyright (C) 2006-2015 by
 * The Salk Institute for Biological Studies and
 * Pittsburgh Supercomputing Center, Carnegie Mellon University
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
******************************************************************************/

#ifndef MCELL_MISC_H
#define MCELL_MISC_H

#include "config.h"
#include "mcell_init.h"
#include "mcell_structs.h"

void mcell_print_version();

void mcell_print_usage(const char *executable_name);

void mcell_print_stats();

int mcell_argparse(int argc, char **argv, MCELL_STATE *state);

struct num_expr_list *mcell_copysort_numeric_list(struct num_expr_list *head);

void mcell_sort_numeric_list(struct num_expr_list *head);

void mcell_free_numeric_list(struct num_expr_list *nel);

MCELL_STATUS mcell_generate_range(struct num_expr_list_head *list, double start,
                                  double end, double step);

int advance_range(struct num_expr_list_head *list, double tmp_dbl);

int mcell_generate_range_singleton(struct num_expr_list_head *lh, double value);

// XXX this is a temporary hack to be able to print in mcell.c
// since mcell disables regular printf
void mcell_print(const char *message);

#endif
