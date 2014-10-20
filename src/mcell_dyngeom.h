/***********************************************************************************
 *                                                                                 *
 * Copyright (C) 2006-2014 by *
 * The Salk Institute for Biological Studies and *
 * Pittsburgh Supercomputing Center, Carnegie Mellon University *
 *                                                                                 *
 * This program is free software; you can redistribute it and/or *
 * modify it under the terms of the GNU General Public License *
 * as published by the Free Software Foundation; either version 2 *
 * of the License, or (at your option) any later version. *
 *                                                                                 *
 * This program is distributed in the hope that it will be useful, *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the *
 * GNU General Public License for more details. *
 *                                                                                 *
 * You should have received a copy of the GNU General Public License *
 * along with this program; if not, write to the Free Software *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 *USA. *
 *                                                                                 *
 ***********************************************************************************/

#ifndef MCELL_DYNGEOM_H
#define MCELL_DYNGEOM_H

int mcell_add_dynamic_geometry(
    char const *dynamic_geometry_filepath, char const *curr_mdl_filepath,
    double time_unit, struct mem_helper *dynamic_geometry_mem, 
    struct dynamic_geometry **dynamic_geometry_head);

struct molecule_info ** save_all_molecules(
    struct volume *state, struct storage_list *storage_head);

void save_common_molecule_properties(struct molecule_info *mol_info,
                                     struct abstract_molecule *am_ptr,
                                     struct string_buffer *reg_names,
                                     char *mesh_name);

void save_volume_molecule(struct volume *state, struct molecule_info *mol_info,
                          struct abstract_molecule *am_ptr, char **mesh_name);

int save_surface_molecule(struct molecule_info *mol_info,
                          struct abstract_molecule *am_ptr,
                          struct string_buffer **reg_names,
                          char **mesh_name);

int place_all_molecules(struct volume *world);

#endif
