#ifndef MCELL_STRUCTS
#define MCELL_STRUCTS

#include <machine/limits.h>
#include <sys/types.h>
#include <stdio.h>

#include "vector.h"
#include "mem_util.h"
#include "sched_util.h"


/*****************************************************/
/**  Brand new constants created for use in MCell3  **/
/*****************************************************/


/* Species flags */
   /* Walls have IS_SURFACE set, molecules do not. */
   /* Surface and grid molecules have ON_SURFACE set */
   /* Grid molecules have ON_GRID set */
   /* IS_ACTIVE is set if this molecule can do anything on its own */
#define ON_SURFACE  0x01
#define ON_GRID     0x02
#define IS_SURFACE  0x04
#define IS_ACTIVE   0x08


/* Reaction flags */
#define RX_DESTROY   0x001
#define RX_FLIP      0x002
#define RX_PROD      0x004
#define RX_REFL      0x008
#define RX_2DESTROY  0x010
#define RX_2FLIP     0x020
#define RX_2PROD     0x040


/* Flags for BSP trees to determine whether something is a node or a branch */
/* Will either have BRANCH_XN through _ZP, or _L, _R, _X, _Y, _Z. */
#define BRANCH_XN  0x01
#define BRANCH_XP  0x02
#define BRANCH_YN  0x04
#define BRANCH_YP  0x08
#define BRANCH_ZN  0x10
#define BRANCH_ZP  0x20

#define BRANCH_L  0x01
#define BRANCH_R  0x02
#define BRANCH_X  0x04
#define BRANCH_Y  0x08
#define BRANCH_Z  0x10


/* Constants equating integers with coordinates */
#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

#define X_NEG 0
#define X_POS 1
#define Y_NEG 2
#define Y_POS 3
#define Z_NEG 4
#define Z_POS 5

#define X_NEG_BIT 0x01
#define X_POS_BIT 0x02
#define Y_NEG_BIT 0x04
#define Y_POS_BIT 0x08
#define Z_NEG_BIT 0x10
#define Z_POS_BIT 0x20


/* Collision types for rays striking surfaces */
#define COLLIDE_MISS    0
#define COLLIDE_FRONT   1
#define COLLIDE_BACK    2
#define COLLIDE_REDO   -1
#define COLLIDE_HIT     3


/* Types for things we can hit */
#define VOL_COLLISION    1
#define WALL_COLLISION   2   
#define MOL_COLLISION    3


/* Flags for edges. */
#define EDGE_BARE     0
#define EDGE_SHARED   1
#define EDGE_ROTONLY  2
#define EDGE_TRANSROT 3


/* ID for default species */
#define GENERIC_MOLECULE 1
#define GENERIC_SURFACE 2


/* Size constants */
#define EPS_C 1e-12
#define GIGANTIC 1e140

/*********************************************************/
/**  Constants used in MCell3 brought over from MCell2  **/
/*********************************************************/


/* Generic numerical constants */
#define EPSILON 1e-14
                                                                                
#define R_UINT_MAX 2.3283064365386963e-10
#define MY_PI 3.14159265358979323846
#define N_AV 6.02205e23
#define ROUND_UP 0.5
                                                                                
                                                                                                                                                                
/* Polygon list types */
#define BOX_POLY 0  
#define ORDERED_POLY 1  
#define UNORDERED_POLY 2  
   /* BOX_POLY is a right parallelepiped polyhedron */
   /* ORDERED_POLY is a SGI Inventor-style polyhedron with shared vertices */
   /* UNORDERED_POLY is a free-form collection of distinct polygons */


/* Wall element shapes */
#define RECT_POLY 0
#define TRI_POLY 1
#define GEN_POLY 2
   /* RECT_POLY is rectangular */
   /* TRI_POLY is triangular */
   /* GEN_POLY is non-rectangular with >3 vertices */


/* Surface grid shapes */
#define RECTANGULAR 0
#define TRIANGULAR 1


/* Orientations */
/* Relative to a surface */
#define OUTWRD 1
#define INWRD -1

/* Relative to a molecule */
#define POS_POLE 1
#define NEG_POLE -1
#define POLE POS_POLE
                                                                                

/* Grid molecule site placement types */
#define EFFDENS 0
#define EFFNUM 1
   /* Place either a certain density or an exact number of effectors */


/*******************************************************/
/**  Old constants copied from MCell2, may be broken  **/
/*******************************************************/

/* Parser parameters.  Probably need to be revisited. */
#define HASHSIZE 128
#define HASHMASK 0x7f
#define COUNTER_HASH 16 
#define COUNTER_HASHMASK 0xf
   /*LWW 6/13/03 Hashtable size for region counters in each OBJECT*/

#define PATHWAYSIZE 64
#define RXSIZE 2048
#define ARGSIZE 255
#define NUM_ADD_EFFECTORS 1024
#define MAX_INCLUDE_DEPTH 16
#define COUNTBUFFERSIZE 10000
                                                                                

/* Data types to be stored in sym_table, probably broken! */
#define RX 1
#define MOL 2
#define PNT 3
#define CMP 4
#define POLY 5
#define RSITE 6
#define OBJ 7
#define RPAT 8
#define REG 9
#define INT 10
#define DBL 11
#define STR 12
#define ARRAY 13
#define FSTRM 14
#define EXPR 15
#define TMP 16

                                                                                
/* Object types, probably okay. */
#define META_OBJ 0
#define BOX_OBJ 1
#define POLY_OBJ 2
#define REL_SITE_OBJ 3
                                                                                

/* Box sides.  This is a weird way to do it.  Why not bitmasks? */
#define TP 0
#define BOT 1
#define FRNT 2
#define BCK 3
#define LFT 4
#define RT 5
#define ALL_SIDES INT_MAX
                   
                                                             
/* Viz state values */
#define EXCLUDE_OBJ INT_MIN


/* Count list specifications.  INIT stuff is probably broken. */
#define OVER_E 0
#define EACH_E 1
#define SPEC_E 2
                                                                                
#define OVER_L 0
#define EACH_L 1
#define SPEC_L 2
                                                                                
#define SUM 0
#define DT 1
#define CUM 2
                                                                                
#define A_EVENTS 0
#define INIT_EVENTS 1
#define INTER_EVENTS 2


/* Count list value types.  Probably okay. */
#define UNKNOWN 0
#define TIME_STAMP_VAL 1
#define INDEX_VAL 2


/* Reaction data output type */
#define FRAME_DATA 0
#define FREQ_DATA 1
   /* Use either frame data or frequency to control when to output */


/* Output list type */
#define FRAME_NUMBER 0
#define REAL_TIME 1


/* Region counter type.  INIT probably broken. */
#define RX_STATE 0
#define INIT_TRANS 1
#define TRANSITIONS 2
#define MOL_TRANS_EACH 3
#define MOL_TRANS_ALL 4


/* Visualization stuff. */
/* Visualization modes. */
#define NO_VIZ_MODE 0
#define DX_MODE 1
#define IRIT_MODE 2
#define RADIANCE_MODE 3
#define RAYSHADE_MODE 4
#define RENDERMAN_MODE 5
#define POVRAY_MODE 6
#define MCELL_MODE 7

/* Visualization frame data types. */
#define ALL_FRAME_DATA 0
#define EFF_POS 1
#define EFF_STATES 2
#define MOL_POS 3
#define MOL_STATES 4
#define SURF_POS 5
#define SURF_STATES 6


/* event types for release event queue */
#define TRAIN_HIGH_EVENT 0
#define TRAIN_LOW_EVENT 1
#define RELEASE_EVENT 2
                                                                                
/* release number methods */
#define CONSTNUM 0
#define GAUSSNUM 1
#define VOLNUM 2


/* Stimulus motion types.  What is this? */
#define FXD 0



/**********************************************/
/**  New/reworked structures used in MCell3  **/
/**********************************************/

typedef unsigned char byte;

#ifdef NOINCLUDE_SYS_TYPES
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
#endif


/* Properties of one type of molecule or surface */
struct species
{
  u_int species_id;             /* Unique ID for this species */
  u_int hashval;                /* Hash value (may be nonunique) */
  struct sym_table *sym;        /* Symbol table entry (name) */
  
  u_int population;             /* How many of this species exist? */
  
  double D;                     /* Diffusion constant */
  double D_ref;                 /* Reference diffusion constant */
  double radius;                /* Molecular radius */
  double space_step;            /* Characteristic step length */
/*double time_step;*/           /* Minimum (maximum?) sensible timestep */
  short charge;                 /* Electric charge. */
  u_short flags;                /* Free?  Membrane bound?  Membrane? */
  
  int viz_state;                /* Visualization state for output */
  byte checked;                 /* Bread crumb for graph traversal */
};


/* All pathways leading away from a given intermediate */
struct rxn
{
  struct rxn *next;          /* Next reaction with these reactants */
  
  u_int n_reactants;         /* How many reactants? (At least 1.) */
  u_int n_pathways;          /* How many pathways lead away? */
  u_int *product_idx;        /* Index of 1st player for products of pathway */
  double *cum_rates;         /* Cumulative rates for (entering) all pathways */
  double *cat_rates;         /* Rate of leaving all pathways (<=0.0 is instant) */
  
  struct species **players;  /* Identities of reactants/products */
  short *geometries;         /* Geometries of reactants/products */

  byte *fates;               /* What happens to reactants in each pathway? */
  
  int n_rate_t_rxns;         /* How many pathways have varying rates? */
  int *rate_t_rxn_map;       /* Indices of pathways with varying rates */
  struct t_func *rate_t;     /* Rate over time for each varying pathway */
  struct t_func *jump_t;     /* Summary of transition times */
   
  u_int last_update;         /* When did we last update rates/counts? */
  
  u_int *rxn_count_dt;       /* How many times this timestep? */
  u_int *rxn_count_cum;      /* How many times ever? */
};


/* Sets of reactions grouped by...? */
struct rxn_group
{
/* Someone else gets to fill this in. */
};


/* Piecewise constant function for time-varying reaction rates */
struct t_func
{
  int index;        /* Which constant part are we in? */
  int n;            /* How many pieces do we have? */
  u_int *time;      /* When are we done (units=timesteps)? */
  double *value;    /* What is the value now? */
};


/* Abstract structure that starts all molecule structures */
struct abstract_molecule
{
  struct abstract_molecule *next;  /* Next molecule in scheduling queue */
  double t;                        /* Scheduling time. */
  double t_inert;                  /* Dead time for catalysts. */
  struct species *properties;      /* What type of molecule are we? */
};


/* Freely diffusing or fixed molecules in solution */
struct molecule
{
  struct abstract_molecule *next;
  double t;
  double t_inert;
  struct species *properties;
  
  double t2;                      /* Time of move or -time of unimol. rxn */
  
  struct vector3 pos;             /* Position in space */
  struct subvolume *subvol;       /* Partition we are in */
  
  struct cmprt_data *curr_cmprt;  /* Compartment we are in (for counting) */
  
  struct molecule *next_v;        /* Next molecule in this subvolume */
};


/* Freely diffusing or fixed molecules on a surface */
struct surface_molecule
{
  struct abstract_molecule *next;
  double t;
  double t_inert;
  struct species *properties;
  
  double t2;                        /* Time of move or -time of unimol. rxn */
  
  struct vector3 pos;              /* Position in the world */
  struct vector2 s_pos;            /* Position in surface coordinates */
  struct wall *curr_wall;           /* The surface element we are on */
  struct subvolume *subvol;         /* Partition we're in */
  short orient;                     /* Facing up or down? */
  
  struct region_data *curr_region;  /* Region we are in (for counting) */
  
  struct surface_molecule *next_s;  /* Next molecule on this surface */
  struct surface_molecule *next_v;  /* Next molecule in this volume */
};


/* Fixed molecule on a grid on a surface */
struct grid_molecule
{
  struct abstract_molecule *next;
  double t;
  double t_inert;
  struct species *properties;
  
  int grid_index;              /* Which gridpoint do we occupy? */
  short orient;                /* Which way do we point? */
  struct surface_grid *grid;   /* Our grid (which tells us our surface) */
};


struct edge
{
  struct wall *forward;     /* For which wall is this a forwards transform? */
  struct wall *backward;    /* For which wall is this a reverse transform? */
  
  struct vector2 translate;  /* Translation vector */
  double cos_theta;          /* Cosine of angle between bases */
  double sin_theta;          /* Sine of angle between bases */
  
  double length;             /* Length of the edge */
  double length_1;           /* Reciprocal of length of edge */
  
  int flags;
};


struct wall
{
  struct wall *next;              /* Next wall in the universe */
  
  struct species *wall_type;      /* Parameters for this type of wall */

  struct vector3 *vert[3];        /* Array of pointers to vertices */
  struct vector3 *vert_normal[3]; /* Array of pointers to vertex normals */
  
  double uv_vert1_u;              /* Surface u-coord of 2nd corner (v=0) */
  struct vector2 uv_vert2;        /* Surface coords of third corner */

  struct edge *edges[3];          /* Array of pointers to each edge. */
  struct wall *nb_walls[3];       /* Array of pointers to neighboring walls */

  double area;                    /* Area of this element */
  
  struct vector3 normal;          /* Normal vector for this wall */
  struct vector3 unit_u;          /* U basis vector for this wall */
  struct vector3 unit_v;          /* V basis vector for this wall */
  double d;                       /* Distance to origin (point normal form) */
  
  struct surface_molecule *mol;   /* Head of list of surface molecules */
  int mol_count;                  /* How many surface molecules? */
  
  struct surface_grid *effectors; /* Grid of effectors for this wall */
  
  int viz_state;                  /* For display purposes */

  struct object *parent_object;   /* The object we are a part of */

  struct cmprt_data *parent;      /* Compartment data of parent */
  struct cmprt_data *neighbor;    /* Compartment data of neighbor */
  
  struct region_list *regions;    /* Regions that contain this wall */
};


/* Linked list of walls (for subvolumes) */
struct wall_list
{
  struct wall_list *next;   /* The next entry in the list */
  
  struct wall *this_wall;        /* The wall in this entry */
};


/* 3D vector of short integers */
struct short3D
{
  short x;
  short y;
  short z;
};


/* Point in space that will tell us which compartments we're in */

struct waypoint
{
  struct vector3 loc;
  struct cmprt_data **owners;
  int n_owners;
};


/* Contains space for molcules, walls, wall_lists, etc. */
struct storage
{
  struct mem_helper *list;
  struct mem_helper *mol;
  struct mem_helper *smol;
  struct mem_helper *gmol;
  struct mem_helper *wall;
  
  struct schedule_helper *timer;
  double current_time;
};

/* Walls and molecules in a spatial subvolume */
struct subvolume
{
  struct wall_list *wall_head; /* Head of linked list of intersecting walls */
  struct wall_list *wall_tail; /* Tail of list of walls */
  int wall_count;              /* How many walls intersect? */
  
  struct molecule *mol_head;   /* Head of linked list of molecules */
  int mol_count;               /* How many molecules are here? */
  
  int index;                   /* Index of subvolume (parallelization?) */
  
  struct short3D llf;          /* Indices of left lower front corner */
  struct short3D urb;          /* Indices of upper right back corner */
  
  short is_bsp;                /* Flags saying what the void pointers are */

  void *neighbor[6];           /* Subvolume or bsp_tree across each face */
  
  struct storage *mem;         /* Local storage */
};


/* Binary space partitioning tree for subvolume connections */
struct bsp_tree
{
  void *left;        /* The tree below the partition */
  void *right;       /* The tree above the partition */
  short partition;   /* The index of the partition */
  short flags;       /* Coordinate that is split, plus terminal node flags */
};


struct counter
{
  struct counter *next;
  int wall_id;
  int mol_id;
  int crossings;
  int impacts;
};


/* All data about the world */
struct volume
{
/*  struct vector3 corner[8];*/  /* Corners of the world */
  struct vector3 llf;           /* left lower front corner of world */
  struct vector3 urb;           /* upper right back corner of world */
  
  int n_axis_partitions;        /* Number of coarse partition boundaries */
  double *x_partitions;         /* Coarse X partition boundaries */
  double *y_partitions;         /* Coarse Y partition boundaries */
  double *z_partitions;         /* Coarse Z partition boundaries */
  
  int n_fine_partitions;        /* Number of fine partition boundaries (multiple of n_axis_partitions) */
  double *x_fineparts;           /* Fine X partition boundaries */
  double *y_fineparts;           /* Fine Y partition boundaries */
  double *z_fineparts;           /* Fine Z partition boundaries */
  
  int n_waypoints;              /* How many of these = (n_axis_p-3)^3 */
  struct waypoint *waypoints;   /* Contains compartment information */
  
  int n_subvol;                 /* How many coarse subvolumes? */
  struct subvolume *subvol;     /* Array containing all subvolumes */
   
  int binning;                  /* How many real partitions per one in lookup? */
  struct subvolume **lookup;     /* 3D lookup array pointing at subvolumes */
  
  int n_surfaces;               /* Count of how many walls we have */
  struct wall *walls;           /* Head of a linked list of all walls */
  
  int hashsize;                 /* How many entries in our hash table? */
  int n_reactions;              /* How many reactions are there, total? */
  struct rxn **reaction_hash;   /* A hash table of all reactions. */
  
  int counter_hashmask;         /* Mask for looking up collision hash table */
  struct counter **collide_hash;/* Collision hash table */
  
  int n_species;                /* How many different species? */
  struct species **species_list; /* Array of all species. */
  
  struct stack_helper *collision_stack;
  struct counter_helper *collision_counter;
  
  struct counter **counter_hash;
  
  u_int rng_idx;

  /* Simulation initialization parameters  */
  struct sym_table **main_sym_table;
  struct object *root_object;
  struct object *root_instance;
  struct release_pattern *default_release_pattern;
  struct release_event_queue *release_event_queue_head;
  struct count_list *count_list;
  struct count_list *count_zero;
  struct output_list *output_list;
  struct viz_obj *viz_obj_head;
  struct frame_data_list *frame_data_head;
  double time_unit;
  double length_unit;
  double effector_grid_density;
  double *r_step;
  double *d_step;
  double *factorial_r;
  double r_num_directions;
  double sim_elapsed_time;
  double chkpt_elapsed_time;
  double chkpt_elapsed_time_start;
  double current_time;
  double current_start_time;
  double max_diffusion_step;
  double random_number_use;
  double ray_voxel_tests;
  double ray_polygon_tests;
  double ray_polygon_colls;
  double diffusion_steps;
  double sim_elapse_time;
  struct vector3 bb_min;
  struct vector3 bb_max;
  u_int tot_mols;
  u_int seed;
  u_int init_seed;
  u_int it_time;
  u_int start_time;
  u_int n_release_events;
  u_int radial_directions;
  u_int radial_subdivisions;
  u_int num_directions;
  int fully_random;
  int procnum;
  int viz_mode;
  byte voxel_image_mode;
  byte voxel_volume_mode;
  char *molecule_prefix_name;

  /* MCell startup command line arguments */
  byte info_opt;
  u_int seed_seq;
  u_int iterations;
  char *log_file_name;
  FILE *log_file;
  u_int log_freq;
  u_int chkpt_init;
  u_int chkpt_flag;
  u_int chkpt_iterations;
  u_int chkpt_seq_num;
  char *chkpt_infile;
  char *chkpt_outfile;
  FILE *chkpt_infs;
  FILE *chkpt_outfs;
  FILE *chkpt_signal_file_tmp;
  char *mdl_infile_name;
  char *curr_file;

};


/* Grid over a surface containing grid_molecules */
struct surface_grid
{
  int n;                   /* Number of slots along each axis */

  double inv_strip_wid;    /* Reciprocal of the width of one strip */
  double vert2_slope;      /* Slope from vertex 0 to vertex 2 */
  double fullslope;        /* Slope of full width of triangle */
  
  double binding_factor;   /* Binding probability correction factor for surface area */
  
  u_int n_tiles;           /* Number of tiles in effector grid (triangle: grid_size^2, rectangle: 2*grid_size^2) */
  u_int n_occupied;        /* Number of tiles occupied by grid_molecules */
  struct grid_molecule **mol;  /* Array of pointers to grid_molecule for each tile */
  
  int set;                 /* segl object set number */
  int index;               /* Unique index into effector_table */
  
  struct subvolume *subvol;/* Best match for which subvolume we're in */
  struct wall *surface;    /* The wall that we are in */
};


/* Temporary data structure to store information about collisions. */
struct collision
{
  void *target;
  struct rxn *intermediate;
  double t;
  struct vector3 loc;
  int what;
};


/******************************************************************/
/**  Everything below this line has been copied from MCell 2.69  **/
/******************************************************************/


/**
 * Node data structure. 
 * \todo why only left and right walls?
 */
struct node_dat {
	struct vector3 corner[8];
	struct wall *right_wall;
	struct wall *left_wall;
	byte left_subvol;
	byte right_subvol;
	int left_node;
	int right_node;
};

/**
 * Linked list for all counter data being output.
 * Modified by Lin-Wei 5/21/02
 */
struct output_list {
	byte out_type;
	unsigned int id;		/**<unique id number for each REACTION_OUTPUT_DATA command*/
	int counter;
	int freq;
	int n_output;
	int index;
	struct counter_info *counter_info;
	struct reaction_list *reaction_list;
	struct output_list *next;   /**< next item in output list*/
};

struct counter_info {
	char outfile_name[1024];              /**< name of variable being tracked*/
	struct count_list *count_list;
	struct counter_info *next;  
};

/**
 * Reaction output data list
 *
 */
struct reaction_list {
  byte list_type;
  int n_reac_iterations;
  int reac_iteration;
  int *array;
  struct num_expr_list *iteration_list;
  struct num_expr_list *curr_reac_iteration;
  struct reaction_list *next;
};

/**
 * Linked list of counters.
 */
struct count_list {
	int n_output;			/**< total output time steps.*/
	int freq;
	unsigned int frame_index;
	byte reset_flag;            /**< reset temp_data to 0 on each iteration?*/
	byte update_flag;           /**< counter update necessary?*/
	byte data_type;             /**< type of data to track:
	                              MOL RX CMP EXPR INT DBL*/
	byte index_type;            /**< flag indicating final_data is to be
	                              indexed by either
	                              TIME_STAMP_VAL or INDEX_VAL*/
	int n_data;
	void *temp_data;            /**< ptr to data specified by type*/
	void *final_data;           /**< ptr to data specified by type*/
	struct count_list *operand1;
	struct count_list *operand2;
	char oper;
	struct count_list *next;    /**< next item in count list*/
};

struct lig_count_list {
        struct count_list *count_list;
	struct lig_count_list *next;
};

struct lig_count_ref {
	unsigned short type;
        char *full_name;
        struct count_list *count_list;
	struct lig_count_ref *next;
};

struct viz_state_ref {
	int viz_state;
        char *full_name;
	struct viz_state_ref *next;
};

/**
 * Compartment. [\todo need more info]
 */
struct cmprt_data {
	struct sym_table *sym;
        char *full_name;
        byte cmprt_type;            /**< type of compartment:
                                       BOX_POLY, ORDERED_POLY, UNORDERED_POLY */
        byte fully_closed;
	int instance;
	int *lig_count;
	double *conc;
	double volume;
	double vm;
        int n_corners;
        int n_walls;
	struct vector3 *corner;
	struct vector3 *vertex_normal;
	struct vector3 *normal;
	struct wall_list *wall_list;
	struct wall **wall;
	struct cmprt_data **neighbor;
	struct cmprt_data *next;
};

struct cmprt_data_list {
	struct cmprt_data *cmprt_data;
	struct cmprt_data_list *next;
};

/**
 * A Polygon object, part of a surface.
 */
struct polygon_object {
	struct lig_count_ref *lig_count_ref;
					/**< ptr to list of lig_count_ref
	                                   structures: one for each time polygon
	                                   object is referenced in count stmt */
	struct viz_state_ref *viz_state_ref;
					/**< ptr to list of viz_state_ref
	                                   structures: one for each time polygon
	                                   object is referenced in
					   STATE_VALUES block */
	byte list_type;			/**< type of polygon list:
					   BOX_POLY, ORDERED_POLY,
                                           or UNORDERED_POLY */
        void *polygon_data;             /**< pointer to appropriate data structure
                                           holding polygon vertices etc...
                                           either:
                                           struct box_poly
                                           -or-
                                           struct ordered_poly
                                           -or-
                                           struct unordered_poly
                                           as specified by list_type. */
	struct polygon_list *polygon_list;  /**< For UNORDERED_LISTs of polygons.
				 	       pointer to list of polygon_list
					       data structures.  The list will
					       store the vertices for each
					       polygon in the object.
					       The box object is a special case
					       in that we only need to know the
					       LLF and URB vertices to construct
				               all 6 faces of the box.  These
					       vertices will be stored as the
					       first two vertices in the first
					       polygon_list structure. */
	int n_polys;			/**< Number of polygons in polygon
					   object*/
	byte fully_closed;		/**< flag indicating closure of object */
        unsigned short *side_stat;	/**< array of side status values:
	                                   one for each polygon in object.
					   0 indicates removed
					   1 indicates exists. */
        byte **lig_prop;		/**< array of ligand/wall property arrays:
					   one array for each polygon in object.
					   each array contains n_ligand_types
					   entries indicating ligand
					   permeability. */
        unsigned int *cmprt_side_map;   /**< Array of index mappings from
                                           polygon sides to cmprt_data sides
                                           necessary because polygon sides
                                           might be outside the parallel
                                           partition and therefore not
                                           created as a cmprt side */
/*        struct eff_dat **eff_prop;*/	/**< array of ptrs to eff_dat data
					   structures, one for each polygon. */
};

/**
 * A polygon that is a box.
 */
struct box_poly {
        struct vector3 *llf;             /**< LLF corner of box polyhedron */
        struct vector3 *urb;             /**< URB corner of box polyhedron */
};

/**
 * An ordered polygon.
 * That is, the vertices of the polygon are ordered according to
 * the right hand rule.
 */
struct ordered_poly {
	struct vector3 **vertex;         /**< Array of polygon vertices. */
	struct vector3 **normal;         /**< Array of polygon normals. */
	struct element_data *element_data; /**< Array element_data
                                              data structures */
	int n_verts;                    /**< Number of vertices in polyhedron. */
};

/**
 * Polygon data structure used to build polygon.
 * This data structure is used to store the data from the MDL file
 * and to contruct a polygon object from it.
 */
struct element_data {
        int *vertex_index;              /**< Array of vertex indices forming a
                                           polygon. */
	int n_verts;                    /**< Number of vertices in polygon. */
};

/**
 * Polygon object where the vertices of the polygon are not ordered
 * according to the right hand rule.
 */
struct unordered_poly {
        struct vertex_list *vertex_list;  /**< pointer to linked list of
                                             vertex_list data structures */
	struct unordered_poly *next;      /**< pointer to next unordered_poly */
};

/**
 * A linked list used to store the coordinates of a vertex and the
 * corresponding normal vector at that point.
 */
struct vertex_list {
        struct vector3 *vertex;           /**< pointer to one polygon vertex */
        struct vector3 *normal;           /**< pointer to one polygon normal */
        struct vertex_list *next;         /**< pointer to next vertex_list */
};
 
/**
 * A compartment.
 */
struct cmprt {
	struct sym_table *sym;
        unsigned short type;
	int inst_count;
	struct lig_count_list **lig_count_list;  /**< array of ptrs to lig_count_list
	                                        structures: one for each
	                                        ligand type */
        byte a_zone_lig;
        unsigned short side_stat[6];
        struct vector3 *vert1;
        struct vector3 *vert2;
        struct vector3 *a_zone_loc;
        byte *lig_prop[6];
        struct eff_dat *eff_prop[6];
	int *count_freq;
        int color[6];
};

/**
 * Linked list of effector type data.
 */
struct eff_dat {
        struct rx *rx;
        byte quantity_type;  /* type is either EFFDENS or EFFNUM */
        double quantity; /* amount of effectors to place, density or number */
	signed char orient;
        struct eff_dat *next;
};

/**
 * Linked list of elements.
 * [\todo what is this?]
 */
struct element_list {
        unsigned int begin;
        unsigned int end;   
        struct element_list *next;
};

/**
 * A region.
 * [\todo what is this?]
 */
struct region {
	struct sym_table *sym;
	int hashval;
        char *region_last_name;
	struct object *parent;
	struct element_list *element_list;
	struct reg_counter_ref_list *reg_counter_ref_list;
	struct eff_dat *eff_dat;
};

/**
 * Linked list of regions.
 * [\todo what is this?]
 */
struct region_list {
	struct region *reg;
	struct region_list *next;
};

/*
 *region counter reference, store all the info for counting reaction on regions
 */
struct reg_counter_ref {
	unsigned int counter;
	byte count_type; 	/*Three possible types:RX_STATE, TRANSITION, INIT_TRANS, MOL_TRANS*/
	byte count_method;	/* Three types:DT, SUM, CUM */
	struct region *parent;
	struct rx *state;
	struct rx *next_state;
	struct lig_transition_count **transition_count_each; /**< array of pointers to transition counter structures on region. One array element per rx mechanism in simulation. Indexed by parent_rx.rx_index */
	struct reg_counter_ref *next;  
};

struct reg_counter_ref_list {
	struct reg_counter_ref *reg_counter_ref;
	struct reg_counter_ref_list *next;
};
/*
 *counter hash table for counting on regions in each object
 */
struct counter_hash_table {
	char *name;
	void *value;
	struct counter_hash_table *next;
};

/**
 * Container data structure for all physical objects.
 */
struct object {
        struct sym_table *sym;
        char *last_name;
        byte object_type;
        void *contents;			/**< ptr to actual physical object */
        struct object *parent;		/**< ptr to parent meta object */
        struct object *next;		/**< ptr to next child object */
        struct object *first_child;	/**< ptr to first child object */
        struct object *last_child;	/**< ptr to last child object */
	struct lig_count_ref *lig_count_ref;
					/**< ptr to list of lig_count_ref
	                                   structures: one for each time meta-
	                                   object is referenced in count stmt */
        unsigned int num_regions;	/**< number of regions defined */
	struct region_list *region_list; /**< ptr to list of regions for 
					      this object */
	struct counter_hash_table **counter_hash_table;	/**<hash table for region counter in object*/
/*        struct eff_dat **eff_prop;*/	/**<  if this object is a
					   BOX_OBJ or POLY_OBJ this will be an
					   array of ptrs to eff_dat data
					   structures, one for each polygon. */
	struct cmprt_data *cmprt_data;	/**< if this object is a 
					   BOX_OBJ or POLY_OBJ this will
					   point to the cmprt_data struct
					   containing the instantiated object */
        struct viz_obj *viz_obj;
        int *viz_state;			/**< array of viz state values.
					   One for each element of object. */
        double t_matrix[4][4];		/**< transformation matrix for object */
};

/**
 * Doubly linked list of names.
 */
struct name_list {
        char *name;
        struct name_list *prev;
        struct name_list *next;
};

/**
 * Visualization objects.
 */
struct viz_obj {
	char *name;
        char *full_name;
	struct object *obj;
	struct cmprt_data_list *cmprt_data_list;
	struct viz_obj *next;
};

/**
 * Linked list of instances.
 * [\todo what is this?]
 */
struct instance {
        struct object *obj;
        struct vector3 translate;
        struct vector3 axis;
	double angle;
	struct dspl_flag *dspl_flag;
	struct conn_list *conn_list;
        struct instance *next_instance;
};

/**
 * Geometric transformation data for a physical object.
 * Used to instantiate an object.
 */
struct transformation {
        struct vector3 translate;
        struct vector3 scale;
        struct vector3 rot_axis;
        double rot_angle;
};
 
/**
 * Data pertaining to a ligand release site.
 */
struct release_site_obj {
	struct vector3 *location;	/**< location of release site */
	struct species *mol_type;	/**< species to be released */
	byte release_number_method;
	int release_number;
	int mean_number;
	double mean_diameter;
	double concentration;
        double standard_deviation;
	double diameter;	/**< diameter of release site [\todo can release sites
						  only be spherical? */
	double release_prob;
	struct release_pattern *release_pattern;
};

/**
 * Molecule release pattern data.
 */
struct release_pattern {
        struct sym_table *sym;
	double delay;
	double release_interval;
	double train_interval;
	double train_duration;
	int number_of_trains;
};

/**
 * Linked list of timed ligand release events.
 */
struct release_event_queue {
	struct release_site_obj *release_site_obj;
	struct vector3 location;
	byte event_type;
	double event_time;
	int event_counter;
	double train_high_time;
        int index;                   /**< unique index of this release_event */
	struct release_event_queue *next;
};

/**
 * Linked list of data to be output.
 * [\todo better description.]
 */
struct frame_data_list {
  byte list_type;
	int type;
	int viz_iteration;
	int n_viz_iterations;
	struct num_expr_list *iteration_list;
	struct num_expr_list *curr_viz_iteration;
	struct frame_data_list *next;
};


/**
 * A pointer to filehandle and it's real name.
 * Used for user defined file IO operations.
 */
struct file_stream {
	char *name;
	FILE *stream;
};

/**
 * Linked list of symbols.
 * Used to parse and store user defined symbols from the MDL input file.
 */
struct sym_table {
        unsigned short sym_type; /**< type of symbol stored -
                                   OBJ, RX, MOL, DBL, PNT ...*/
        char *name;              /**< name of symbol*/
        void *value;             /**< ptr to stored value*/
        struct sym_table *next;  /**< next symbol in symbol table*/
#ifdef KELP
		byte keep_alive;	/**< flag to indicate continued use of
							  this symbol table entry during computation */
		byte ref_count;		/**< number of times referenced in MDL file */
#endif
};

/**
 * Linked list of numerical expressions.
 * Used for parsing MDL input file arithmetic expressions.
 */
struct num_expr_list {
  double value;
  struct num_expr_list *next;
};

#ifdef DEBUG
#define no_printf printf
#else
void no_printf(const char *,...);
#endif


#endif
