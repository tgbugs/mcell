/***********************************************************************************
 *                                                                                 *
 * Copyright (C) 2006-2013 by                                                      *
 * The Salk Institute for Biological Studies and                                   *
 * Pittsburgh Supercomputing Center, Carnegie Mellon University                    *
 *                                                                                 *
 * This program is free software; you can redistribute it and/or                   *
 * modify it under the terms of the GNU General Public License                     *
 * as published by the Free Software Foundation; either version 2                  *
 * of the License, or (at your option) any later version.                          *
 *                                                                                 *
 * This program is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   *
 * GNU General Public License for more details.                                    *
 *                                                                                 *
 * You should have received a copy of the GNU General Public License               *
 * along with this program; if not, write to the Free Software                     *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. *
 *                                                                                 *
 ***********************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "create_reactions.h"
#include "react_util.h"
#include "strfunc.h"



/* static functions */
static char* create_prod_signature(struct product **product_head);
static int sort_product_list_compare(struct product *list_item, 
    struct product *new_item);
static struct product* sort_product_list(struct product *product_head);


/*************************************************************************
 *
 * extract_reactants extracts the reactant info into a pathway structure
 *
 *************************************************************************/
MCELL_STATUS 
extract_reactants(struct pathway *path, struct species_opt_orient *reactants,
  int *num_reactants, int *num_vol_mols, int *num_grid_mols, int *all_3d) 
{
  for (struct species_opt_orient *current_reactant = reactants; 
       current_reactant != NULL; current_reactant = current_reactant->next)
  {
    short orient = current_reactant->orient_set ? current_reactant->orient : 0;
    struct species *reactant_species = (struct species *)current_reactant->mol_type->value;
  
    if ((reactant_species->flags & NOT_FREE) == 0)
    {
      (*num_vol_mols)++;
    } 
    else 
    {
      *all_3d = 0;
    }

    if (reactant_species->flags & ON_GRID)
    {
      (*num_grid_mols)++;
    }

    switch (*num_reactants)
    {
      case 0:
        path->reactant1 = reactant_species;
        path->orientation1 = orient;
        break;

      case 1:
        path->reactant2 = reactant_species;
        path->orientation2 = orient;
        break;

      case 2:
        path->reactant3 = reactant_species;
        path->orientation3 = orient;
        break;

      /* too many reactants */
      default: return MCELL_FAIL;
    }

    (*num_reactants)++;
  }

  return MCELL_SUCCESS;
}


/*************************************************************************
 *
 * extract_catalytic_arrow extracts the info for a catalytic arrow
 * into a pathway structure
 *
 *************************************************************************/
MCELL_STATUS 
extract_catalytic_arrow(struct pathway *path, 
    struct reaction_arrow *react_arrow, int *num_reactants, 
    int *num_vol_mols, int *num_grid_mols, int *all_3d) 
{
  if (*num_reactants >= 3) 
  {
    return MCELL_FAIL;
  }

  struct species *catalyst_species = (struct species *)react_arrow->catalyst.mol_type->value;
  short orient = react_arrow->catalyst.orient_set ? react_arrow->catalyst.orient : 0;

  /* XXX: Should surface class be allowed inside a catalytic arrow? */
  if (catalyst_species->flags & IS_SURFACE)
  {
    //mdlerror(parse_state, "A surface classes may not appear inside a catalytic arrow");
    return MCELL_FAIL;
  }

  /* Count the type of this reactant */
  if ((catalyst_species->flags & NOT_FREE) == 0)
  {
    (*num_vol_mols)++;
  }
  else 
  {
    *all_3d = 0;
  }

  if (catalyst_species->flags & ON_GRID) 
  {
    (*num_grid_mols)++;
  }

  /* Copy in catalytic reactant */
  switch (*num_reactants)
  {
    case 1:
      path->reactant2 = (struct species*)react_arrow->catalyst.mol_type->value;
      path->orientation2 = orient;
      break;

    case 2:
      path->reactant3 = (struct species*)react_arrow->catalyst.mol_type->value;
      path->orientation3 = orient;
      break;

    case 0:
    default:
      //mcell_internal_error("Catalytic reagent ended up in an invalid slot (%d).", reactant_idx);
      return MCELL_FAIL;
  }
  (*num_reactants)++;

  return MCELL_SUCCESS;
}



/*************************************************************************
 *
 * extract_surface extracts the info for a surface included in the
 * reaction specification
 *
 *************************************************************************/
MCELL_STATUS 
extract_surface(struct pathway *path, struct species_opt_orient *surf_class,
    int *num_reactants, int *num_surfaces, int *oriented_count) 
{
  short orient = surf_class->orient_set ? surf_class->orient : 0;
  if (surf_class->orient_set) {
    oriented_count++;
  }

  /* Copy reactant into next available slot */
  switch (*num_reactants)
  {
    case 0:
      //mdlerror(parse_state, "Before defining reaction surface class at least one reactant should be defined.");
      return MCELL_FAIL;

    case 1:
      path->reactant2 = (struct species*)surf_class->mol_type->value;
      path->orientation2 = orient;
      break;

    case 2:
      path->reactant3 = (struct species*)surf_class->mol_type->value;
      path->orientation3 = orient;
      break;

    default:
      //mdlerror(parse_state, "Too many reactants--maximum number is two plus reaction surface class.");
      return MCELL_FAIL;
  }

  num_reactants++;
  num_surfaces++;

  return MCELL_SUCCESS;
}



/*************************************************************************
 *
 * add_catalytic_species_to_products adds all species that are part of a
 * catalytic reaction to the list of products.
 *
 *************************************************************************/
MCELL_STATUS
add_catalytic_species_to_products(struct pathway *path, int catalytic,
    int bidirectional, int all_3d)
{
  struct species *catalyst;
  short catalyst_orient;
  switch (catalytic)
  {
    case 0: 
      catalyst = path->reactant1; 
      catalyst_orient = path->orientation1; 
      break;
    case 1: 
      catalyst = path->reactant2; 
      catalyst_orient = path->orientation2; 
      break;
    case 2: 
      catalyst = path->reactant3; 
      catalyst_orient = path->orientation3; 
      break;
    default:
      //mcell_internal_error("Catalytic reagent index is invalid.");
      return MCELL_FAIL;
  }

  if (bidirectional || !(catalyst->flags & IS_SURFACE))
  {
    struct product *prodp = (struct product*)CHECKED_MALLOC_STRUCT(
        struct product, "reaction product");
    if (prodp == NULL)
    {
      return MCELL_FAIL;
    }

    prodp->is_complex = 0;
    prodp->prod = catalyst;
    if (all_3d) 
    {
      prodp->orientation = 0;
    }
    else 
    {
      prodp->orientation = catalyst_orient;
    }
    prodp->next = path->product_head;
    path->product_head = prodp;
  }

  return MCELL_SUCCESS;
}



/*************************************************************************
 *
 * extract_products extracts the product info into a pathway structure
 *
 *************************************************************************/
MCELL_STATUS 
extract_products(struct pathway *path, struct species_opt_orient *products,
  int *num_surf_products, int *bidirectional, int *all_3d) 
{
  struct species_opt_orient *current_product;
  for (current_product = products;
       current_product != NULL;
       current_product = current_product->next)
  {
    /* Nothing to do for NO_SPECIES */
    if (current_product->mol_type == NULL)
    {
      continue;
    }

    /* Create new product */
    struct product *prodp = (struct product*)CHECKED_MALLOC_STRUCT(
       struct product, "reaction product"); 
    if (prodp == NULL)
    {
      return MCELL_FAIL;
    }

    /* Set product species and orientation */
    prodp->prod = (struct species *)current_product->mol_type->value;
    if (*all_3d) 
    {
      prodp->orientation = 0;
    }
    else 
    {
      prodp->orientation = current_product->orient;
    }

    /* Disallow surface as product unless reaction is bidirectional */
    if (!*bidirectional && (prodp->prod->flags & IS_SURFACE))
    {
      return MCELL_FAIL;
    }

    /* Append product to list */
    prodp->next = path->product_head;
    path->product_head = prodp;

    if (prodp->prod->flags & ON_GRID)
    {
      num_surf_products++;
    }

    /* Add product if it isn't a surface */
    if (!(prodp->prod->flags&IS_SURFACE))
    {
      if (all_3d == 0 && (!current_product->orient_set))
      {
        return MCELL_FAIL;  // product orientation not specified
      }
      else
      {
        if ((prodp->prod->flags&NOT_FREE)!=0)
        {
          return MCELL_FAIL; // trying to create surface product in presence 
                             // of only volume reactants
        }
        if (current_product->orient_set)
        {
            return MCELL_FAIL; // orientation specified for only volume reactants
        }
      }
    }
  }

  return MCELL_SUCCESS;
}


/*************************************************************************
 *
 * extract_pathname extracts the pathname (if one was given into 
 * a pathway structure
 *
 *************************************************************************/
MCELL_STATUS 
extract_pathname(struct pathway *path, struct rxn *rxnp, 
    struct sym_table *pathname)
{
  struct rxn_pathname *rxpnp = (struct rxn_pathname *)pathname->value;
  rxpnp->rx = rxnp;
  path->pathname = rxpnp;

  return MCELL_FAIL;
}



/*************************************************************************
 *
 * extract_rath extracts the forward rate of the reaction 
 *
 *************************************************************************/
MCELL_STATUS
extract_forward_rate(struct pathway *path, struct reaction_rates* rate,
    const char *rate_filename)
{
  switch (rate->forward_rate.rate_type)
  {
    case RATE_UNSET:
      return MCELL_FAIL;  // no rate set

    case RATE_CONSTANT:
      path->km = rate->forward_rate.v.rate_constant;
      path->km_filename = NULL;
      path->km_complex = NULL;
      break;

    case RATE_FILE:
      path->km = 0.0;
      path->km_filename = (char*)rate_filename;
      free(rate->forward_rate.v.rate_file);
      path->km_complex = NULL;
      break;

    case RATE_COMPLEX:
      path->km = 0.0;
      path->km_filename = NULL;
      path->km_complex = rate->forward_rate.v.rate_complex;
      break;

    default: 
      //UNHANDLED_CASE(rate->forward_rate.rate_type);
      return MCELL_FAIL;
  }

  return MCELL_SUCCESS;
}



/*************************************************************************
 *
 * create_product_signature for the pathway 
 *
 *************************************************************************/
MCELL_STATUS
create_product_signature(struct pathway *path) 
{
  if (path->product_head != NULL)
  {
    path->prod_signature = create_prod_signature(&path->product_head);
    if (path->prod_signature == NULL)
    {
      return MCELL_FAIL;  // creation of field failed
    }
  }
  else
  {
    path->prod_signature = NULL;
  }

  return MCELL_SUCCESS;
}



/*************************************************************************
 *
 * grid_space_available_for_surface_products checks for enough available
 * grid space for surface products. 
 * If the vacancy search distance is zero and this reaction produces more
 * grid molecules than it comsumes, it can never succeed, except if it is a
 * volume molecule hitting the surface and producing a single grid molecule.
 * Fail with an error message.
 *
 *************************************************************************/
MCELL_STATUS
grid_space_available_for_surface_products(double vacancy_search_dist2,
    int num_grid_mols, int num_vol_mols, int num_surf_products) 
{
  if ((vacancy_search_dist2 == 0) && (num_surf_products > num_grid_mols))
  {
    /* The case with one volume molecule reacting with the surface and
     * producing one grid molecule is okay.
     */
    if (num_grid_mols == 0 && num_vol_mols == 1 && num_surf_products == 1)
    {
      /* do nothing */
    }
    else
    {
      return MCELL_FAIL;  // number of surface products exceeds number of 
                          // surface reactants but VACANCY_SEARCH_DISTANCE
                          // is not specified
    }
  }
  return MCELL_SUCCESS;
}





/*************************************************************************
 create_rx_name:
    Assemble reactants alphabetically into a reaction name string.

 In:  p: reaction pathway whose reaction name we are to create
 Out: a string to be used as a symbol name for the reaction
*************************************************************************/
char*
create_rx_name( struct pathway *p)
{

  struct species *reagents[3];
  int n_reagents = 0;
  int is_complex = 0;

  /* Store reagents in an array. */
  reagents[0] = p->reactant1;
  reagents[1] = p->reactant2;
  reagents[2] = p->reactant3;

  /* Count non-null reagents. */
  for (n_reagents = 0; n_reagents < 3; ++ n_reagents)
    if (reagents[n_reagents] == NULL)
      break;
    else if (p->is_complex[n_reagents])
      is_complex = 1;

  /* Sort reagents. */
  for (int i = 0; i<n_reagents; ++i)
  {
    for (int j = i+1; j<n_reagents; ++ j)
    {
      /* If 'i' is a subunit, 'i' wins. */
      if (p->is_complex[i])
        break;

      /* If 'j' is a subunit, 'j' wins. */
      else if (p->is_complex[j])
      {
        struct species *tmp = reagents[j];
        reagents[j] = reagents[i];
        reagents[i] = tmp;
      }

      /* If 'j' precedes 'i', 'j' wins. */
      else if (strcmp(reagents[j]->sym->name, reagents[i]->sym->name) < 0)
      {
        struct species *tmp = reagents[j];
        reagents[j] = reagents[i];
        reagents[i] = tmp;
      }
    }
  }

  /* Now, produce a name! */
  if (is_complex)
  {
    switch (n_reagents)
    {
      case 1: return alloc_sprintf("(%s)", reagents[0]->sym->name);
      case 2: return alloc_sprintf("(%s)+%s", reagents[0]->sym->name, reagents[1]->sym->name);
      case 3: return alloc_sprintf("(%s)+%s+%s", reagents[0]->sym->name, reagents[1]->sym->name, reagents[2]->sym->name);
      default:
        //mcell_internal_error("Invalid number of reagents in reaction pathway (%d).", n_reagents);
        return NULL;
    }
  }
  else
  {
    switch (n_reagents)
    {
      case 1: return alloc_sprintf("%s", reagents[0]->sym->name);
      case 2: return alloc_sprintf("%s+%s", reagents[0]->sym->name, reagents[1]->sym->name);
      case 3: return alloc_sprintf("%s+%s+%s", reagents[0]->sym->name, reagents[1]->sym->name, reagents[2]->sym->name);
      default:
        //mcell_internal_error("Invalid number of reagents in reaction pathway (%d).", n_reagents);
        return NULL;
    }
  }
}



/************************************************************************
 * static helper functions
 ************************************************************************/

/*************************************************************************
 sort_product_list_compare:
    Comparison function for products to be sorted when generating the product
    signature.

 In:  list_item: first item to compare
      new_item:  second item to compare
 Out: -1 if list_item < new_item, 1 if list_item > new_item, 0 if they are
      equal

  XXX Currently this function also appears in mdlparse_util.c. It should
      eventually be removed from there and only appear in this file.
*************************************************************************/
static int 
sort_product_list_compare(struct product *list_item, struct product *new_item)
{
  int cmp = list_item->is_complex - new_item->is_complex;
  if (cmp != 0)
    return cmp;

  cmp = strcmp(list_item->prod->sym->name, new_item->prod->sym->name);
  if (cmp == 0)
  {
    if (list_item->orientation > new_item->orientation)
      cmp = -1;
    else if (list_item->orientation < new_item->orientation)
      cmp = 1;
    else
      cmp = 0;
  }
  return cmp;
}

/*************************************************************************
 sort_product_list:
    Sorts product_head in alphabetical order, and descending orientation order.
    Current algorithm uses insertion sort.

 In:  product_head: list to sort
 Out: the new list head

  XXX Currently this function also appears in mdlparse_util.c. It should
      eventually be removed from there and only appear in this file.
*************************************************************************/
static struct product*
sort_product_list(struct product *product_head)
{
  struct product *next;             /* Saved next item (next field in product is overwritten) */
  struct product *iter;             /* List iterator */
  struct product *result = NULL;    /* Sorted list */
  int cmp;

  /* Use insertion sort to sort the list of products */
  for (struct product *current = product_head;
       current != NULL;
       current = next)
  {
    next = current->next;

    /* First item added always goes at the head */
    if (result == NULL)
    {
      current->next = result;
      result = current;
      continue;
    }

    /* Check if the item belongs at the head */
    cmp = sort_product_list_compare(result, current);
    if (cmp >= 0)
    {
      current->next = result;
      result = current;
      continue;
    }

    /* Otherwise, if it goes after the current entry, scan forward to find the insert point */
    else
    {
      /* locate the node before the point of insertion */
      iter = result;
      while (iter->next != NULL  &&  sort_product_list_compare(iter, current) < 0)
        iter = iter->next;

      current->next = iter->next;
      iter->next = current;
    }
  }

  return result;
}


/*************************************************************************
 create_prod_signature:
    Returns a string containing all products in the product_head list,
    separated by '+', and sorted in alphabetical order by name and descending
    orientation order.

 In:  product_head: list of products
 Out: product signature as a string.  *product_head list is sorted in
      alphabetical order by name, and descending order by orientation.  Returns
      NULL on failure.

  XXX Currently this function also appears in mdlparse_util.c. It should
      eventually be removed from there and only appear in this file.
*************************************************************************/
static char*
create_prod_signature( struct product **product_head)
{
  /* points to the head of the sorted alphabetically list of products */
  char *prod_signature = NULL;

  *product_head = sort_product_list(*product_head);

  /* create prod_signature string */
  struct product *current = *product_head;
  prod_signature = CHECKED_STRDUP(current->prod->sym->name, "product name");

  /* Concatenate to create product signature */
  char *temp_str = NULL;
  while (current->next != NULL)
  {
    temp_str = prod_signature;
    prod_signature = CHECKED_SPRINTF("%s+%s",
                                     prod_signature,
                                     current->next->prod->sym->name);

    if (prod_signature == NULL)
    {
      if (temp_str != NULL) free(temp_str);
      return NULL;
    }
    if (temp_str != NULL) free(temp_str);

    current = current->next;
  }

  return prod_signature;
}


static void alphabetize_pathway(struct pathway *path, struct rxn *reaction);
static void check_duplicate_special_reactions(struct pathway *path);
static struct rxn *split_reaction(struct rxn *rx);
static struct rxn *create_sibling_reaction(struct rxn *rx);
static int equivalent_geometry(struct pathway *p1, struct pathway *p2, int n);
static int equivalent_geometry_for_two_reactants(int o1a, int o1b, int o2a, int o2b);
static void check_reaction_for_duplicate_pathways(struct pathway **head);
static int load_rate_file(MCELL_STATE *state, struct rxn *rx, char *fname, int path);
static int set_product_geometries(struct rxn *rx, struct product *prod);

MCELL_STATUS finalize_reaction(MCELL_STATE *state, struct rxn *reaction)
{
  struct pathway *path;
  struct product *prod;
  struct rxn *rx;
  struct t_func *tp;
  //double D_tot, t_step;
  short geom;
  int k, kk;
  /* flags that tell whether reactant_1 is also on the product list,
     same for reactant_2 and reactant_3 */
  int recycled1, recycled2, recycled3;
  int num_rx, num_players;
  struct species *temp_sp;
  int n_prob_t_rxns; /* # of pathways with time-varying rates */
  //struct rxn *reaction;

  num_rx = 0;

  //parse_state->vol->tv_rxn_mem = create_mem(sizeof(struct t_func) , 100);
  //if (parse_state->vol->tv_rxn_mem == NULL) return 1;
  //reaction = (struct rxn*)sym->value;
  //reaction->next = NULL;

  for (path=reaction->pathway_head ; path != NULL ; path = path->next)
  {
    check_duplicate_special_reactions(path);

    /* if one of the reactants is a surface, move it to the last reactant.
     * Also arrange reactant1 and reactant2 in alphabetical order */
    if (reaction->n_reactants>1)
    {
      /* Put surface last */
      if ((path->reactant1->flags & IS_SURFACE) != 0)
      {
        temp_sp = path->reactant1;
        path->reactant1 = path->reactant2;
        path->reactant2 = temp_sp;
        geom = path->orientation1;
        path->orientation1 = path->orientation2;
        path->orientation2 = geom;
      }
      if (reaction->n_reactants>2)
      {
        if ((path->reactant2->flags & IS_SURFACE) != 0)
        {
          temp_sp = path->reactant3;
          path->reactant3 = path->reactant2;
          path->reactant2 = temp_sp;
          geom = path->orientation3;
          path->orientation3 = path->orientation2;
          path->orientation2 = geom;
        }
      }
      alphabetize_pathway(path, reaction);
    } /* end if (n_reactants > 1) */

  }  /* end for (path = reaction->pathway_head; ...) */


  /* if reaction contains equivalent pathways, split this reaction into a
   * linked list of reactions each containing only equivalent pathways.
   */

  rx = split_reaction(reaction);

  /* set the symbol value to the head of the linked list of reactions */
  //sym->value = (void *)rx;

  while (rx != NULL)
  {
    double pb_factor = 0.0;
    /* Check whether reaction contains pathways with equivalent product
     * lists.  Also sort pathways in alphabetical order according to the
     * "prod_signature" field.
     */
    check_reaction_for_duplicate_pathways(&rx->pathway_head);

    num_rx++;

    /* At this point we have reactions of the same geometry and can collapse them
     * and count how many non-reactant products are in each pathway. */

    /* Search for reactants that appear as products */
    /* Any reactants that don't appear are set to be destroyed. */
    rx->product_idx = CHECKED_MALLOC_ARRAY(u_int, rx->n_pathways+1, "reaction product index array");
    rx->cum_probs = CHECKED_MALLOC_ARRAY(double, rx->n_pathways, "reaction cumulative probabilities array");

    /* Note, that the last member of the array "rx->product_idx"
     * contains size of the array "rx->players" */

    if (rx->product_idx == NULL  || rx->cum_probs == NULL)
      return 1;
#if 0
    if (reaction_has_complex_rates(rx))
    {
      int pathway_idx;
      rx->rates = CHECKED_MALLOC_ARRAY(struct complex_rate *, rx->n_pathways, "reaction complex rates array");
      if (rx->rates == NULL)
        return 1;
      for (pathway_idx = 0; pathway_idx < rx->n_pathways; ++ pathway_idx)
        rx->rates[pathway_idx] = NULL;
    }
#endif
    n_prob_t_rxns = 0;
    path = rx->pathway_head;

    for (int n_pathway=0; path!=NULL ; n_pathway++ , path = path->next)
    {
      rx->product_idx[n_pathway] = 0;
      if (rx->rates)
        rx->rates[n_pathway] = path->km_complex;

      /* Look for concentration clamp */
      if (path->reactant2!=NULL && (path->reactant2->flags&IS_SURFACE)!=0 &&
          path->km >= 0.0 && path->product_head==NULL && ((path->flags & PATHW_CLAMP_CONC) != 0))
      {
        struct ccn_clamp_data *ccd;

        //if (n_pathway!=0 || path->next!=NULL)
        //  mcell_warn("Mixing surface modes with other surface reactions.  Please don't.");

        if (path->km>0)
        {
          ccd = CHECKED_MALLOC_STRUCT(struct ccn_clamp_data, "concentration clamp data");
          if (ccd==NULL)
            return 1;

          ccd->surf_class = path->reactant2;
          ccd->mol = path->reactant1;
          ccd->concentration = path->km;
          if (path->orientation1*path->orientation2==0)
          {
            ccd->orient = 0;
          }
          else
          {
            ccd->orient = (path->orientation1==path->orientation2) ? 1 : -1;
          }
          ccd->sides = NULL;
          ccd->next_mol = NULL;
          ccd->next_obj = NULL;
          ccd->objp = NULL;
          ccd->n_sides = 0;
          ccd->side_idx = NULL;
          ccd->cum_area = NULL;
          ccd->scaling_factor = 0.0;
          ccd->next = state->clamp_list;
          state->clamp_list = ccd;
        }
        path->km = GIGANTIC;
      }
      else if ((path->flags & PATHW_TRANSP) != 0)
      {
        rx->n_pathways = RX_TRANSP;
        if (path->reactant2!=NULL && (path->reactant2->flags&IS_SURFACE) &&
           (path->reactant1->flags & ON_GRID))
        {
                path->reactant1->flags |= CAN_REGION_BORDER;
        }
      }
      else if ((path->flags & PATHW_REFLEC) != 0)
      {
        rx->n_pathways = RX_REFLEC;
        if (path->reactant2!=NULL && (path->reactant2->flags&IS_SURFACE) &&
           (path->reactant1->flags & ON_GRID))
        {
                path->reactant1->flags |= CAN_REGION_BORDER;
        }
      }
      else if (path->reactant2!=NULL && (path->reactant2->flags&IS_SURFACE) && (path->reactant1->flags & ON_GRID) && (path->product_head==NULL) && (path->flags & PATHW_ABSORP))
      {
         rx->n_pathways = RX_ABSORB_REGION_BORDER;
         path->reactant1->flags |= CAN_REGION_BORDER;
      }
      else if ((strcmp(path->reactant1->sym->name, "ALL_SURFACE_MOLECULES") == 0))
      {
        if (path->reactant2!=NULL && (path->reactant2->flags&IS_SURFACE)  && (path->product_head==NULL) && (path->flags & PATHW_ABSORP))
        {
          rx->n_pathways = RX_ABSORB_REGION_BORDER;
          path->reactant1->flags |= CAN_REGION_BORDER;
        }
      }
      if (path->km_filename == NULL) rx->cum_probs[n_pathway] = path->km;
      else
      {
        rx->cum_probs[n_pathway]=0;
        n_prob_t_rxns++;
      }

      recycled1 = 0;
      recycled2 = 0;
      recycled3 = 0;

      for (prod=path->product_head ; prod != NULL ; prod = prod->next)
      {
        if (recycled1 == 0 && prod->prod == path->reactant1) recycled1 = 1;
        else if (recycled2 == 0 && prod->prod == path->reactant2) recycled2 = 1;
        else if (recycled3 == 0 && prod->prod == path->reactant3) recycled3 = 1;
        else rx->product_idx[n_pathway]++;
      }
    } /* end for (n_pathway=0,path=rx->pathway_head; ...) */

    /* Now that we know how many products there really are, set the index array */
    /* and malloc space for the products and geometries. */
    num_players = rx->n_reactants;
    kk = rx->n_pathways;
    if (kk<=RX_SPECIAL) kk = 1;
    for (int n_pathway=0;n_pathway<kk;n_pathway++)
    {
      k = rx->product_idx[n_pathway] + rx->n_reactants;
      rx->product_idx[n_pathway] = num_players;
      num_players += k;
    }
    rx->product_idx[kk] = num_players;

    rx->players = CHECKED_MALLOC_ARRAY(struct species*, num_players, "reaction players array");
    rx->geometries = CHECKED_MALLOC_ARRAY(short, num_players, "reaction geometries array");
    if (rx->pathway_head->is_complex[0] ||
        rx->pathway_head->is_complex[1] ||
        rx->pathway_head->is_complex[2])
    {
      rx->is_complex = CHECKED_MALLOC_ARRAY(unsigned char, num_players, "reaction 'is complex' flag");
      if (rx->is_complex == NULL)
        return 1;
      memset(rx->is_complex, 0, sizeof(unsigned char) * num_players);
    }
    else
      rx->is_complex = NULL;

    if (rx->players==NULL || rx->geometries==NULL)
      return 1;

    /* Load all the time-varying rates from disk (if any), merge them into */
    /* a single sorted list, and pull off any updates for time zero. */
    if (n_prob_t_rxns > 0)
    {
      path = rx->pathway_head;
      for (int n_pathway = 0; path!=NULL ; n_pathway++, path=path->next)
      {
        if (path->km_filename != NULL)
        {
          if (load_rate_file(state, rx, path->km_filename, n_pathway))
            //mcell_error("Failed to load rates from file '%s'.", path->km_filename);
            return MCELL_FAIL;
        }
      }
      rx->prob_t = (struct t_func*) ae_list_sort((struct abstract_element*)rx->prob_t);

      while (rx->prob_t != NULL && rx->prob_t->time <= 0.0)
      {
        rx->cum_probs[ rx->prob_t->path ] = rx->prob_t->value;
        rx->prob_t = rx->prob_t->next;
      }
    } /* end if (n_prob_t_rxns > 0) */


    /* Set the geometry of the reactants.  These are used for triggering.                 */
    /* Since we use flags to control orientation changes, just tell everyone to stay put. */
    path = rx->pathway_head;
    rx->players[0] = path->reactant1;
    rx->geometries[0] = path->orientation1;
    if (rx->is_complex) rx->is_complex[0] = path->is_complex[0];
    if (rx->n_reactants > 1)
    {
      rx->players[1] = path->reactant2;
      rx->geometries[1] = path->orientation2;
      if (rx->is_complex) rx->is_complex[1] = path->is_complex[1];
      if (rx->n_reactants > 2)
      {
        rx->players[2] = path->reactant3;
        rx->geometries[2] = path->orientation3;
        if (rx->is_complex) rx->is_complex[2] = path->is_complex[2];
      }
    }

    /* maximum number of surface products */
    path = rx->pathway_head;
    int max_num_surf_products = set_product_geometries(rx, prod);

    pb_factor = compute_pb_factor(state, rx, max_num_surf_products);
    rx->pb_factor = pb_factor;
    path = rx->pathway_head;

    if (scale_probabilities(path, rx, parse_state, pb_factor))
      return 1;

    if (n_prob_t_rxns > 0)
    {
      for (tp = rx->prob_t ; tp != NULL ; tp = tp->next)
        tp->value *= pb_factor;
    }

    /* Move counts from list into array */
    if (rx->n_pathways > 0)
    {
      rx->info = CHECKED_MALLOC_ARRAY(struct pathway_info, rx->n_pathways, "reaction pathway info");
      if (rx->info == NULL)
        return 1;

      path = rx->pathway_head;
      for (int n_pathway=0; path!=NULL ; n_pathway++,path=path->next)
      {
        rx->info[n_pathway].count = 0;
        rx->info[n_pathway].pathname = path->pathname;    /* Keep track of named rxns */
        if (path->pathname!=NULL)
        {
          rx->info[n_pathway].pathname->path_num = n_pathway;
          rx->info[n_pathway].pathname->rx = rx;
        }
      }
    }
    else /* Special reaction, only one exit pathway */
    {
      rx->info = CHECKED_MALLOC_STRUCT(struct pathway_info,
                                           "reaction pathway info");
      if (rx->info == NULL)
        return 1;
      rx->info[0].count = 0;
      rx->info[0].pathname = rx->pathway_head->pathname;
      if (rx->pathway_head->pathname!=NULL)
      {
        rx->info[0].pathname->path_num = 0;
        rx->info[0].pathname->rx = rx;
      }
    }

    /* Sort pathways so all fixed pathways precede all varying pathways */
    if (rx->rates  &&  rx->n_pathways > 0)
      reorder_varying_pathways(rx);

    /* Compute cumulative properties */
    for (int n_pathway=1; n_pathway<rx->n_pathways; ++n_pathway)
      rx->cum_probs[n_pathway] += rx->cum_probs[n_pathway-1];
    if (rx->n_pathways > 0)
      rx->min_noreaction_p = rx->max_fixed_p = rx->cum_probs[rx->n_pathways - 1];
    else
      rx->min_noreaction_p = rx->max_fixed_p = 1.0;
    if (rx->rates)
      for (int n_pathway=0; n_pathway<rx->n_pathways; ++n_pathway)
        if (rx->rates[n_pathway])
          rx->min_noreaction_p += macro_max_rate(rx->rates[n_pathway], pb_factor);

    rx = rx->next;
  }

  if (parse_state->vol->grid_grid_reaction_flag  || parse_state->vol->grid_grid_grid_reaction_flag)
  {
    if (parse_state->vol->notify->reaction_probabilities==NOTIFY_FULL)
      mcell_log("For reaction between two (or three) surface molecules the upper probability limit is given. The effective reaction probability will be recalculated dynamically during simulation.");
  }

  if (build_reaction_hash_table(parse_state, num_rx))
    return 1;

  parse_state->vol->rx_radius_3d *= parse_state->vol->r_length_unit; /* Convert into length units */

  for (int n_rxn_bin=0;n_rxn_bin<parse_state->vol->rx_hashsize;n_rxn_bin++)
  {
    for (struct rxn *this_rx = parse_state->vol->reaction_hash[n_rxn_bin];
         this_rx != NULL;
         this_rx = this_rx->next)
    {
      /* Here we deallocate some memory used for creating pathways.
         Other pathways related memory will be freed in
         'mdlparse.y'.  */
      for (path = this_rx->pathway_head; path != NULL; path = path->next)
      {
        if (path->prod_signature != NULL) free(path->prod_signature);
      }

      set_reaction_player_flags(this_rx);
      this_rx->pathway_head = NULL;
    }
  }

  add_surface_reaction_flags(parse_state);

  if (parse_state->vol->notify->reaction_probabilities==NOTIFY_FULL)
    mcell_log_raw("\n");

  return 0;
}


void alphabetize_pathway(struct pathway *path, struct rxn *reaction)
{
  unsigned char temp_is_complex;
  short geom, geom2;
  struct species *temp_sp, *temp_sp2;

  /* Alphabetize if we have two molecules */
  if ((path->reactant2->flags&IS_SURFACE)==0)
  {
    if (strcmp(path->reactant1->sym->name, path->reactant2->sym->name) > 0)
    {
      temp_sp = path->reactant1;
      path->reactant1 = path->reactant2;
      path->reactant2 = temp_sp;
      geom = path->orientation1;
      path->orientation1 = path->orientation2;
      path->orientation2 = geom;
      temp_is_complex = path->is_complex[0];
      path->is_complex[0] = path->is_complex[1];
      path->is_complex[1] = temp_is_complex;
    }
    else if (strcmp(path->reactant1->sym->name, path->reactant2->sym->name) == 0)
    {
      if (path->orientation1 < path->orientation2)
      {
        geom = path->orientation1;
        path->orientation1 = path->orientation2;
        path->orientation2 = geom;
        temp_is_complex = path->is_complex[0];
        path->is_complex[0] = path->is_complex[1];
        path->is_complex[1] = temp_is_complex;
      }
    }
  }

  /* Alphabetize if we have three molecules */
  if (reaction->n_reactants == 3)
  {
    if ((path->reactant3->flags&IS_SURFACE)==0)
    {
      if (strcmp(path->reactant1->sym->name, path->reactant3->sym->name) > 0)
      {
         /* Put reactant3 at the beginning */
         temp_sp = path->reactant1;
         geom = path->orientation1;
         path->reactant1 = path->reactant3;
         path->orientation1 = path->orientation3;

         /* Put former reactant1 in place of reactant2 */
         temp_sp2 = path->reactant2;
         geom2 = path->orientation2;
         path->reactant2 = temp_sp;
         path->orientation2 = geom;

         /* Put former reactant2 in place of reactant3 */
         path->reactant3 = temp_sp2;
         path->orientation3 = geom2;
         /* XXX: Update to deal with macromolecules? */

      }
      else if (strcmp(path->reactant2->sym->name, path->reactant3->sym->name) > 0)
      {

         /* Put reactant3 after reactant1 */
         temp_sp = path->reactant2;
         path->reactant2 = path->reactant3;
         path->reactant3 = temp_sp;
         geom = path->orientation2;
         path->orientation2 = path->orientation3;
         path->orientation3 = geom;

      }
    } /*end */
  }
}


/*************************************************************************
 check_duplicate_special_reactions:
   Check for duplicate special reaction pathways (e.g. TRANSPARENT = molecule).

 In: path: Parse-time structure for reaction pathways
 Out: Nothing. 
 Note: I'm not sure if this code is ever actually called.
*************************************************************************/
void check_duplicate_special_reactions(struct pathway *path)
{
  /* if it is a special reaction - check for the duplicates pathways */
  if (path->next != NULL)
  {
    if ((path->flags & PATHW_TRANSP) && (path->next->flags & PATHW_TRANSP))
    {
      if ((path->orientation2 == path->next->orientation2) ||
         (path->orientation2 == 0) || (path->next->orientation2 == 0))
      {
         mcell_error("Exact duplicates of special reaction TRANSPARENT = %s are not allowed.  Please verify the contents of DEFINE_SURFACE_CLASS statement.", path->reactant2->sym->name);
      }
    }

    if ((path->flags & PATHW_REFLEC) && (path->next->flags & PATHW_REFLEC))
    {
      if ((path->orientation2 == path->next->orientation2) ||
         (path->orientation2 == 0) || (path->next->orientation2 == 0))
      {
         mcell_error("Exact duplicates of special reaction REFLECTIVE = %s are not allowed.  Please verify the contents of DEFINE_SURFACE_CLASS statement.", path->reactant2->sym->name);
      }
    }
    if ((path->flags & PATHW_ABSORP) && (path->next->flags & PATHW_ABSORP))
    {
      if ((path->orientation2 == path->next->orientation2) ||
         (path->orientation2 == 0) || (path->next->orientation2 == 0))
      {
        mcell_error("Exact duplicates of special reaction ABSORPTIVE = %s are not allowed.  Please verify the contents of DEFINE_SURFACE_CLASS statement.", path->reactant2->sym->name);
      }
    }
  }
}


/*************************************************************************
 split_reaction:
 In:  parse_state: parser state
      rx: reaction to split
 Out: Returns head of the linked list of reactions where each reaction
      contains only geometrically equivalent pathways
*************************************************************************/
struct rxn *split_reaction(struct rxn *rx)
{
  struct rxn  *curr_rxn_ptr = NULL,  *head = NULL, *end = NULL;
  struct rxn *reaction;
  struct pathway *to_place, *temp;

  /* keep reference to the head of the future linked_list */
  head = end = rx;
  to_place = head->pathway_head->next;
  head->pathway_head->next = NULL;
  head->n_pathways = 1;
  while (to_place != NULL)
  {
    if (to_place->flags & (PATHW_TRANSP | PATHW_REFLEC | PATHW_ABSORP | PATHW_CLAMP_CONC))
    {
      reaction = create_sibling_reaction(rx);
      if (reaction == NULL)
        return NULL;

      reaction->pathway_head = to_place;
      to_place = to_place->next;
      reaction->pathway_head->next = NULL;
      ++ reaction->n_pathways;

      end->next = reaction;
      end = reaction;
    }
    else
    {
      for (curr_rxn_ptr = head; curr_rxn_ptr != NULL; curr_rxn_ptr = curr_rxn_ptr->next)
      {
        if (curr_rxn_ptr->pathway_head->flags & (PATHW_TRANSP | PATHW_REFLEC | PATHW_ABSORP))
          continue;
        if (equivalent_geometry(to_place, curr_rxn_ptr->pathway_head, curr_rxn_ptr->n_reactants))
          break;
      }

      if (! curr_rxn_ptr)
      {
        reaction = create_sibling_reaction(rx);
        if (reaction == NULL)
          return NULL;

        end->next = reaction;
        end = reaction;

        curr_rxn_ptr = end;
      }

      temp = to_place;
      to_place = to_place->next;

      temp->next = curr_rxn_ptr->pathway_head;
      curr_rxn_ptr->pathway_head = temp;
      ++ curr_rxn_ptr->n_pathways;
    }
  }

  return head;
}


/*************************************************************************
 create_sibling_reaction:
    Create a sibling reaction to the given reaction -- a reaction into which
    some of the pathways may be split by split_reaction.

 In:  rx:   reaction for whom to create sibling
 Out: sibling reaction, or NULL on error
*************************************************************************/
struct rxn *create_sibling_reaction(struct rxn *rx)
{

  struct rxn *reaction = CHECKED_MALLOC_STRUCT(struct rxn, "reaction");
  if (reaction == NULL)
    return NULL;
  reaction->next = NULL;
  reaction->sym = rx->sym;
  reaction->n_reactants = rx->n_reactants;
  reaction->n_pathways = 0;
  reaction->cum_probs = NULL;
  reaction->product_idx = NULL;
  reaction->rates = NULL;
  reaction->max_fixed_p = 0.0;
  reaction->min_noreaction_p = 0.0;
  reaction->pb_factor = 0.0;
  reaction->players = NULL;
  reaction->geometries = NULL;
  reaction->is_complex = NULL;
  reaction->n_occurred = 0;
  reaction->n_skipped = 0.0;
  reaction->prob_t = NULL;
  reaction->pathway_head = NULL;
  reaction->info = NULL;
  return reaction;
}


/*************************************************************************
 equivalent_geometry:

 In: p1, p2: pathways to compare
     n: The number of reactants for the pathways
 Out: Returns 1 if the two pathways are the same (i.e. have equivalent
      geometry), 0 otherwise.
*************************************************************************/
int equivalent_geometry(struct pathway *p1, struct pathway *p2, int n)
{

  short o11,o12,o13,o21,o22,o23; /* orientations of individual reactants */
  /* flags for 3-reactant reactions signaling whether molecules orientations
   * are parallel one another and molecule and surface orientaions are parallel
   * one another
   */
  int mols_parallel_1 = SHRT_MIN + 1; /* for first pathway */
  int mols_parallel_2 = SHRT_MIN + 2; /* for second pathway */
  int mol_surf_parallel_1 = SHRT_MIN + 3; /* for first pathway */
  int mol_surf_parallel_2 = SHRT_MIN + 4; /* for second pathway */

  if (memcmp(p1->is_complex, p2->is_complex, 3))
    return 0;

  if (n < 2)
  {
     /* one reactant case */
     /* RULE: all one_reactant pathway geometries are equivalent */

      return 1;

  }
  else if (n < 3)
  {
    /* two reactants case */

    /* RULE - Two pathways have equivalent geometry when:
       1) Both pathways have exactly the same number of reactants;
       2) There exists an identity mapping between reactants from Pathway 1 and
          Pathway 2 such that for each pair of reactants, r1a and r1b from Pathway
          1, and r2a, and r2b from Pathway 2:
         - r1a is the same species as r2a (likewise for r1b and r2b);
         - r1a and r1b have the same orientation in the same orientation class
           if and only if r2a and r2b do;
         - r1a and r1b have the opposite orientation in the same orientation
           class if and only if r2a and r2b do;
         - r1a and r1b are not in the same orientation class, either because
           they have different orientation classes or both are in the zero
           orientation class, if and only if r2a and r2b are not in the same
           orientation class or both are in the zero orientation class
     */

    o11 = p1->orientation1;
    o12 = p1->orientation2;
    o21 = p2->orientation1;
    o22 = p2->orientation2;

    o13 = o23 = 0;


    return equivalent_geometry_for_two_reactants(o11, o12, o21, o22);

  }
  else if (n < 4)
  {
     /* three reactants case */

    o11 = p1->orientation1;
    o12 = p1->orientation2;
    o13 = p1->orientation3;
    o21 = p2->orientation1;
    o22 = p2->orientation2;
    o23 = p2->orientation3;

    /* special case: two identical reactants */
      if ((p1->reactant1 == p1->reactant2)
          && (p2->reactant1 == p2->reactant2))
      {

       /* Case 1: two molecules and surface are in the same orientation class */
        if ((abs(o11) == abs(o12)) && (abs(o11) == abs(o13)))
        {
          if (o11 == o12) mols_parallel_1 = 1;
          else mols_parallel_1 = 0;

          if (mols_parallel_1)
          {
            if ((o11 == -o13) || (o12 == -o13))
            {
               mol_surf_parallel_1 = 0;
            }
            else 
            {
               mol_surf_parallel_1 = 1;
            }
          }
          else 
          {
               mol_surf_parallel_1 = 0;
          }

          if ((abs(o21) == abs(o22)) && (abs(o21) == abs(o23)))
          {
             if (o21 == o22) mols_parallel_2 = 1;
             else mols_parallel_2 = 0;

             if (mols_parallel_2)
             {
               if ((o21 == -o23) || (o22 == -o23))
               {
                  mol_surf_parallel_2 = 0;
               }
               else 
               {
                  mol_surf_parallel_2 = 1;
               }
             }
             else 
             {
                  mol_surf_parallel_2 = 0;
             }

          }

          if ((mols_parallel_1 == mols_parallel_2) &&
              (mol_surf_parallel_1 == mol_surf_parallel_2))
              {
                 return 1;
          }

       } /* end case 1 */

       /* Case 2: one molecule and surface are in the same orientation class */
       else if ((abs(o11) == abs(o13)) || (abs(o12) == abs(o13)))
       {
          if ((o11 == o13) || (o12 == o13)) mol_surf_parallel_1 = 1;
          else mol_surf_parallel_1 = 0;

          /* check that pathway2 is also in the case2 */

          if ((abs(o21) != abs(o23)) || (abs(o22) != abs(o23)))
          {
             if ((abs(o21) == abs(o23)) || (abs(o22) == abs(o23)))
             {
                if ((o21 == o23) || (o22 == o23)) mol_surf_parallel_2 = 1;
                else mol_surf_parallel_2 = 0;

             }
          }
          if (mol_surf_parallel_1 == mol_surf_parallel_2)
          {
             return 1;
          }

       } /* end case 2 */

       /* Case 3: two molecules but not surface are in the same
                  orientation class */
       else if ((abs(o11) == abs(o12)) && (abs(o11) != abs(o13)))
       {
          if (o11 == o12) mols_parallel_1 = 1;
          else mols_parallel_1 = 0;

          if ((abs(o21) == abs(o22)) && (abs(o21) != abs(o23)))
          {
             if (o21 == o22) mols_parallel_2 = 1;
             else mols_parallel_2 = 0;
          }
          if (mols_parallel_1 == mols_parallel_2)
          {
                 return 1;
          }

       }
       /* Case 4: all molecules and surface are in different orientation classes */
       else if ((abs(o11) != abs(o13)) && (abs(o12) != abs(o13)) &&
                 (abs(o11) != abs(o12)))
                 {

          if ((abs(o21) != abs(o23)) && (abs(o22) != abs(o23)) &&
                 (abs(o21) != abs(o22)))
                 {
               return 1;
          }
       } /* end all cases */

    }
    else { /* no identical reactants */

       if ((equivalent_geometry_for_two_reactants(o11, o12, o21, o22))
           && (equivalent_geometry_for_two_reactants(o12, o13, o22, o23))
           && (equivalent_geometry_for_two_reactants(o11, o13, o21, o23)))
           {
                return 1;
       }

    }

  } // end if (n < 4)


  return 0;
}


/*************************************************************************
 equivalent_geometry_for_two_reactants:

 In: o1a: orientation of the first reactant from first reaction
     o1b: orientation of the second reactant from first reaction
     o2a: orientation of the first reactant from second reaction
     o2b: orientation of the second reactant from second reaction
 Out: Returns 1 if the two pathways (defined by pairs o1a-o1b and o2a-o2b)
      have equivalent geometry, 0 otherwise.
*************************************************************************/
int equivalent_geometry_for_two_reactants(int o1a, int o1b, int o2a, int o2b)
{

    /* both reactants for each pathway are in the same
       orientation class and parallel one another */
    if ((o1a == o1b) && (o2a == o2b))
    {
       return 1;
    /* both reactants for each pathway are in the same
       orientation class and opposite one another */
    }
    else if ((o1a == -o1b) && (o2a == -o2b))
    {
       return 1;
    }
    /* reactants are not in the same orientation class */
    if (abs(o1a) != abs(o1b))
    {
       if ((abs(o2a) != abs(o2b)) || ((o2a == 0) && (o2b == 0)))
       {
          return 1;
       }
    }
    if (abs(o2a) != abs(o2b))
    {
       if ((abs(o1a) != abs(o1b)) || ((o1a == 0) && (o1b == 0)))
       {
          return 1;
       }
    }

    return 0;
}


/*************************************************************************
 check_reaction_for_duplicate_pathways:
 In:  head: head of linked list of pathways
 Out: Sorts linked list of pathways in alphabetical order according to the
      "prod_signature" field.  Checks for the duplicate pathways.  Prints error
      message and exits simulation if duplicates found.
 Note: This function is called after 'split_reaction()' function so all
       pathways have equivalent geometry from the reactant side.  Here we check
       whether relative orientation of all players (both reactants and
       products) is the same for the two seemingly identical pathways.
 RULE: Two reactions pathways are duplicates if and only if
        (a) they both have the same number and species of reactants;
        (b) they both have the same number and species of products;
        (c) there exists a bijective mapping between the reactants and products
            of the two pathways such that reactants map to reactants, products
            map to products, and the two pathways have equivalent geometry
            under mapping.
            Two pathways R1 and R2 have an equivalent geometry under a mapping
            M if and only if for every pair of players "i" and "j" in R1, the
            corresponding players M(i) and M(j) in R2 have the same orientation
            relation as do "i" and "j" in R1.
            Two players "i" and "j" in a reaction pathway have the following
            orientation:
              parallel - if both "i" and "j" are in the same nonzero orientation
              class with the same sign;
              antiparallel (opposite) - if they are both in the same nonzero
              orientation class but have opposite sign;
              independent - if they are in different orientation classes or both
              in the zero orientation class.

 PostNote: In this function we check only the validity of Rule (c) since
           conditions of Rule (a) and (b) are already satisfied when the
           function is called.
*************************************************************************/
void check_reaction_for_duplicate_pathways(struct pathway **head)
{

  struct pathway *result = NULL; /* build the sorted list here */
  struct pathway *null_result = NULL; /* put pathways with NULL
                                        prod_signature field here */
  struct pathway *current, *next, **pprev;
  struct product *iter1, *iter2;
  int pathways_equivalent;  /* flag */
  int i, j;
  int num_reactants; /* number of reactants in the pathway */
  int num_products; /* number of products in the pathway */
  int num_players; /* total number of reactants and products in the pathway */
  int *orient_players_1, *orient_players_2; /* array of orientations of players */
  int o1a, o1b, o2a,o2b;

  /* extract  pathways with "prod_signature" field equal to NULL
   into "null_result" list */
  current = *head;
  pprev = head;
  while (current != NULL)
  {
   if (current->prod_signature == NULL)
   {
     *pprev = current->next;
     current->next = null_result;
     null_result = current;
     current = *pprev;
   }
   else
   {
     pprev = &current->next;
     current = current->next;
   }
  }

  /* check for duplicate pathways in null_result */
  current = null_result;
  if ((current != NULL) && (current->next != NULL))
  {
   /* From the previously called function "split_reaction()"
      we know that reactant-reactant pairs in two pathways
      are equivalent. Because there are no products the pathways
      are duplicates.
      RULE: There may be no more than one pathway with zero (--->NULL)
            products in the reaction->pathway_head
            after calling the function "split_reaction()"
   */
   if (current->reactant2 == NULL)
     mcell_error("Exact duplicates of reaction %s  ----> NULL are not allowed.  Please verify that orientations of reactants are not equivalent.",
                 current->reactant1->sym->name);
   else if (current->reactant3 == NULL)
     mcell_error("Exact duplicates of reaction %s + %s  ----> NULL are not allowed.  Please verify that orientations of reactants are not equivalent.",
                 current->reactant1->sym->name,
                 current->reactant2->sym->name);
   else
     mcell_error("Exact duplicates of reaction %s + %s + %s  ----> NULL are not allowed.  Please verify that orientations of reactants are not equivalent.",
                 current->reactant1->sym->name,
                 current->reactant2->sym->name,
                 current->reactant3->sym->name);
  }

  /* now sort the remaining pathway list by "prod_signature" field
     and check for the duplicates */
   current = *head;

  while(current != NULL)
  {
     next = current->next;

     /* insert in sorted order into the "result" */
     if (result == NULL || (strcmp(result->prod_signature, current->prod_signature) >= 0))
     {
        current->next = result;
        result = current;
     }
     else 
     {
        struct pathway *iter = result;
        while(iter->next != NULL && (strcmp(iter->next->prod_signature, current->prod_signature) < 0))
        {
             iter = iter->next;
        }
        current->next = iter->next;
        iter->next = current;
     }

     /* move along the original list */
     current = next;
  }

   /* Now check for the duplicate pathways */
   /* Since the list is sorted we can proceed down the list
      and compare the adjacent nodes */

   current = result;

   if (current != NULL)
   {
     while(current->next != NULL) 
     {
       if (strcmp(current->prod_signature, current->next->prod_signature) == 0)
       {

         pathways_equivalent  = 1;
         /* find total number of players in the pathways */
         num_reactants = 0;
         num_products = 0;
         if (current->reactant1 != NULL) num_reactants++;
         if (current->reactant2 != NULL) num_reactants++;
         if (current->reactant3 != NULL) num_reactants++;

         iter1 = current->product_head;
         while(iter1 != NULL)
         {
           num_products++;
           iter1 = iter1->next;
         }

         num_players = num_reactants + num_products;

         /* create arrays of players orientations */
         orient_players_1 = CHECKED_MALLOC_ARRAY(int, num_players, "reaction player orientations");
         if (orient_players_1 == NULL)
           mcell_die();
         orient_players_2 = CHECKED_MALLOC_ARRAY(int, num_players, "reaction player orientations");
         if (orient_players_2 == NULL)
           mcell_die();

         if (current->reactant1!=NULL) orient_players_1[0]=current->orientation1;
         if (current->reactant2!=NULL) orient_players_1[1]=current->orientation2;
         if (current->reactant3!=NULL) orient_players_1[2]=current->orientation3;
         if (current->next->reactant1!=NULL) orient_players_2[0]=current->next->orientation1;
         if (current->next->reactant2!=NULL) orient_players_2[1]=current->next->orientation2;
         if (current->next->reactant3!=NULL) orient_players_2[2]=current->next->orientation3;


         iter1 = current->product_head;
         iter2 = current->next->product_head;

         for (i = num_reactants; i < num_players; i++)
         {
           orient_players_1[i] = iter1->orientation;
           orient_players_2[i] = iter2->orientation;
           iter1 = iter1->next;
           iter2 = iter2->next;
         }


         /* below we will compare only reactant-product
            and product-product combinations
            because reactant-reactant combinations
            were compared previously in the function
            "equivalent_geometry()"
            */

         /* Initial assumption - pathways are equivalent.
            We check whether this assumption is
            valid by  comparing pairs as described
            above */

         i = 0;
         while((i < num_players) && (pathways_equivalent))
         {
           if (i < num_reactants)
           {
             j = num_reactants;
           }
           else 
           {
             j = i + 1;
           }
           for (; j < num_players; j++)
           {
             o1a = orient_players_1[i];
             o1b = orient_players_1[j];
             o2a = orient_players_2[i];
             o2b = orient_players_2[j];
             if (!equivalent_geometry_for_two_reactants(o1a, o1b, o2a, o2b))
             {
               pathways_equivalent = 0;
               break;
             }
           }
           i++;
         }

         if (pathways_equivalent)
         {
           if (current->reactant2 == NULL)
             mcell_error("Exact duplicates of reaction %s  ----> %s are not allowed.  Please verify that orientations of reactants are not equivalent.",
                         current->reactant1->sym->name,
                         current->prod_signature);
           else if (current->reactant3 == NULL)
             mcell_error("Exact duplicates of reaction %s + %s  ----> %s are not allowed.  Please verify that orientations of reactants are not equivalent.",
                         current->reactant1->sym->name,
                         current->reactant2->sym->name,
                         current->prod_signature);
           else
             mcell_error("Exact duplicates of reaction %s + %s + %s  ----> %s are not allowed.  Please verify that orientations of reactants are not equivalent.",
                         current->reactant1->sym->name,
                         current->reactant2->sym->name,
                         current->reactant3->sym->name,
                         current->prod_signature);
         }
       }

       current = current->next;
     }
   }

    if (null_result == NULL)
    {
       *head = result;
    }
    else if (result == NULL)
    {
       *head = null_result;
    }
    else
    {
       current = result;
       while(current->next != NULL)
       {
          current = current->next;
       }
       current->next = null_result;
       null_result->next = NULL;

       *head = result;
    }

}

/*************************************************************************
 load_rate_file:
    Read in a time-varying reaction rates file.

 In:  parse_state:  parser state
      rx:    Reaction structure that we'll load the rates into.
      fname: Filename to read the rates from.
      path:  Index of the pathway that these rates apply to.
 Out: Returns 1 on error, 0 on success.
      Rates are added to the prob_t linked list.  If there is a rate given for
      time <= 0, then this rate is stuck into cum_probs and the (time <= 0)
      entries are not added to the list.  If no initial rate is given in the
      file, it is assumed to be zero.
 Note: The file format is assumed to be two columns of numbers; the first
      column is time (in seconds) and the other is rate (in appropriate
      units) that starts at that time.  Lines that are not numbers are
      ignored.
*************************************************************************/
#define RATE_SEPARATORS "\f\n\r\t\v ,;"
#define FIRST_DIGIT "+-0123456789"
int
load_rate_file(MCELL_STATE *state, struct rxn *rx, char *fname, int path)
{
  int i;
  FILE *f = fopen(fname,"r");

  if (!f) return 1;
  else
  {
    struct t_func *tp,*tp2;
    double t,rate;
    char buf[2048];
    char *cp;
    int linecount = 0;
#ifdef DEBUG
    int valid_linecount = 0;
#endif

    tp2 = NULL;
    while (fgets(buf,2048,f))
    {
      linecount++;
      for (i=0;i<2048;i++) { if (!strchr(RATE_SEPARATORS,buf[i])) break; }

      if (i<2048 && strchr(FIRST_DIGIT,buf[i]))
      {
        t = strtod((buf+i) , &cp);
        if (cp == (buf+i)) continue;  /* Conversion error. */

        for (i=cp-buf ; i<2048 ; i++) { if (!strchr(RATE_SEPARATORS,buf[i])) break; }
        rate = strtod((buf+i) , &cp);
        if (cp == (buf+i)) continue;  /* Conversion error */

        /* at this point we need to handle negative reaction rates */
        if (rate < 0.0)
        {
          if (state->notify->neg_reaction==WARN_ERROR)
          {
            //mdlerror(parse_state, "Error: reaction rates should be zero or positive.");
            return 1;
          }
          else if (state->notify->neg_reaction == WARN_WARN) {
            //mcell_warn("Warning: negative reaction rate %f; setting to zero and continuing.", rate);
            rate = 0.0;
          }
        }


        tp = CHECKED_MEM_GET(state->tv_rxn_mem, "time-varying reaction rate");
        if (tp == NULL)
          return 1;
        tp->next = NULL;
        tp->path = path;
        tp->time = t / state->time_unit;
        tp->value = rate;
#ifdef DEBUG
        valid_linecount++;
#endif

        if (rx->prob_t == NULL)
        {
          rx->prob_t = tp;
          tp2 = tp;
        }
        else
        {
          if (tp2==NULL)
          {
            tp2 = tp;
            tp->next = rx->prob_t;
            rx->prob_t = tp;
          }
          else
          {
            if (tp->time < tp2->time)
              //mcell_warn("In rate file '%s', line %d is out of sequence.  Resorting.", fname, linecount);
            tp->next = tp2->next;
            tp2->next = tp;
            tp2 = tp;
          }
        }
      }
    }

#ifdef DEBUG
    mcell_log("Read %d rates from file %s.", valid_linecount, fname);
#endif

    fclose(f);
  }
  return 0;
}
#undef FIRST_DIGIT
#undef RATE_SEPARATORS



/*************************************************************************
 set_product_geometries:
 
  Walk through the list, setting the geometries of each of the products. We do
  this by looking for an earlier geometric match and pointing there or we just
  point to 0 if there is no match.

 In: path: Parse-time structure for reaction pathways
     rx: Pathways leading away from a given intermediate
     prod: Parse-time structure for products of reaction pathways
 Out: max_num_surf_products: Maximum number of surface products 
*************************************************************************/
int 
set_product_geometries(struct rxn *rx, struct product *prod)
{
  int recycled1, recycled2, recycled3;
  int k, kk, k2;
  short geom;
  struct product *prod2;
  int max_num_surf_products;         /* maximum number of surface products */
  int num_surf_products_per_pathway;

  max_num_surf_products = 0;
  for (int n_pathway=0; path!=NULL ; n_pathway++ , path = path->next)
  {
    recycled1 = 0;
    recycled2 = 0;
    recycled3 = 0;
    k = rx->product_idx[n_pathway] + rx->n_reactants;
    num_surf_products_per_pathway = 0;
    for (prod=path->product_head ; prod != NULL ; prod = prod->next)
    {
      if (recycled1==0 && prod->prod == path->reactant1)
      {
        recycled1 = 1;
        kk = rx->product_idx[n_pathway] + 0;
      }
      else if (recycled2==0 && prod->prod == path->reactant2)
      {
        recycled2 = 1;
        kk = rx->product_idx[n_pathway] + 1;
      }
      else if (recycled3==0 && prod->prod == path->reactant3)
      {
        recycled3 = 1;
        kk = rx->product_idx[n_pathway] + 2;
      }
      else
      {
        kk = k;
        k++;
      }

      if (prod->prod->flags & ON_GRID) num_surf_products_per_pathway++;

      rx->players[kk] = prod->prod;
      if (rx->is_complex) rx->is_complex[kk] = prod->is_complex;

      if ((prod->orientation+path->orientation1)*(prod->orientation-path->orientation1)==0 && prod->orientation*path->orientation1!=0)
      {
        if (prod->orientation == path->orientation1) rx->geometries[kk] = 1;
        else rx->geometries[kk] = -1;
      }
      else if (rx->n_reactants > 1 &&
                (prod->orientation+path->orientation2)*(prod->orientation-path->orientation2)==0 && prod->orientation*path->orientation2!=0
             )
      {
        if (prod->orientation == path->orientation2) rx->geometries[kk] = 2;
        else rx->geometries[kk] = -2;
      }
      else if (rx->n_reactants > 2 &&
                (prod->orientation+path->orientation3)*(prod->orientation-path->orientation3)==0 && prod->orientation*path->orientation3!=0
             )
      {
        if (prod->orientation == path->orientation3) rx->geometries[kk] = 3;
        else rx->geometries[kk] = -3;
      }
      else
      {
        k2 = 2*rx->n_reactants + 1;  /* Geometry index of first non-reactant product, counting from 1. */
        geom = 0;
        for (prod2=path->product_head ; prod2!=prod && prod2!=NULL && geom==0 ; prod2 = prod2->next)
        {
          if ((prod2->orientation+prod->orientation)*(prod2->orientation-prod->orientation)==0 && prod->orientation*prod2->orientation!=0)
          {
            if (prod2->orientation == prod->orientation) geom = 1;
            else geom = -1;
          }
          else geom = 0;

          if (recycled1 == 1)
          {
            if (prod2->prod == path->reactant1)
            {
              recycled1 = 2;
              geom *= rx->n_reactants+1;
            }
          }
          else if (recycled2==1)
          {
            if (prod2->prod == path->reactant2)
            {
              recycled2 = 2;
              geom *= rx->n_reactants+2;
            }
          }
          else if (recycled3==1)
          {
            if (prod2->prod == path->reactant3)
            {
              recycled3 = 2;
              geom *= rx->n_reactants+3;
            }
          }
          else
          {
            geom *= k2;
            k2++;
          }
        }
        rx->geometries[kk] = geom;
      }
      if (num_surf_products_per_pathway > max_num_surf_products) max_num_surf_products = num_surf_products_per_pathway;
    }

    k = rx->product_idx[n_pathway];
    if (recycled1==0) rx->players[k] = NULL;
    if (recycled2==0 && rx->n_reactants>1) rx->players[k+1] = NULL;
    if (recycled3==0 && rx->n_reactants>2) rx->players[k+2] = NULL;
  } /* end for (n_pathway = 0, ...) */
  return max_num_surf_products;
}
