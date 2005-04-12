#ifndef MCELL_UTIL
#define MCELL_UTIL

/**********************************************************************
* Definitions for the infinite array whose size may grow. *
*
***********************************************************************/

#define BLOCK_SIZE 10

/** struct infinite_double_array
    Used to hold information for an infinite array of doubles.
*/
struct infinite_double_array{
	/* the data  for this block */
	double data[BLOCK_SIZE];

	/* pointer to the next array. */
	struct infinite_double_array *next;
};

/** struct infinite_double_array
    Used to hold information for an infinite array of doubles.
*/
struct infinite_int_array{
	/* the data  for this block */
	int data[BLOCK_SIZE];

	/* pointer to the next array. */
	struct infinite_int_array *next;
};

/** struct infinite_string_array
    Used to hold information for an infinite array of strings.
*/
struct infinite_string_array{
	/* the data  for this block */
	char *data[BLOCK_SIZE];

	/* pointer to the next array. */
	struct infinite_string_array *next;
};

/* Initializes the infinite array */
#define ia_init(array_ptr)	{(array_ptr)->next = NULL;}

double ia_double_get(struct infinite_double_array *array_ptr, int index);
void ia_double_store(struct infinite_double_array *array_ptr, int index, double data_to_store);
int ia_int_get(struct infinite_int_array *array_ptr, int index);
void ia_int_store(struct infinite_int_array *array_ptr, int index, int data_to_store);
char* ia_string_get(struct infinite_string_array *array_ptr, int index);
void ia_string_store(struct infinite_string_array *array_ptr, int index, char *data_to_store);


struct bit_array
{
  int nbits;
  int nints;
  /* Bit array data runs off the end of this struct */
};

struct bit_array* new_bit_array(int bits);
struct bit_array* duplicate_bit_array(struct bit_array *old);
int get_bit(struct bit_array* ba, int idx);
void set_bit(struct bit_array *ba, int idx, int value);
void set_bit_range(struct bit_array *ba,int idx1,int idx2,int value);
void set_all_bits(struct bit_array *ba,int value);
void bit_operation(struct bit_array *ba,struct bit_array *bb,char op);
void free_bit_array(struct bit_array *ba);


int bisect(double *list,int n,double val);
int bisect_near(double *list,int n,double val);
int bin(double *list,int n,double val);


int distinguishable(double a,double b,double eps);


#endif
