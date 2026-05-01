#ifndef LIBHASHT_H
#define LIBHASHT_H_

#define EKNOTFOUND	-1	/* Provided key was not found in hash table */
#define SUCCESSFUL	0	/* Successful operation */
#define EPTRINVALD	1	/* A null pointer was passed to a function that expected a value */
#define ENELINVALD	2	/* Value of zero passed for number of elements */
#define EMEMFAILED	3	/* Memory allocation failed */
#define EHASHFFAIL	4	/* The hash function failed */
#define ETRUNCATED	5	/* The result of a lookup was truncated to fit in the provided buffer */

/* Hash table type that is returned from the init function */
/* This is passed into subsequent hash table functions */
typedef struct Hashtable{
	struct Ht_node **nodes;
	size_t nnodes;
	size_t entries;
} Hashtable;

/* On success these functions return 0 */
/* On error they will return a positive error code */
/* If a delete or lookup completed successfully but the key was not in the table it will return -1*/

/* Initialization function for the hash table that */
/* creates the table with nelements elements and */
/* returns a pointer to it */
/* On failure it reutrns a null pointer and if err is not null, updates it with the error code */
Hashtable *ht_init(size_t nelements, int *err);

/* Get the string form of an error code returned by a hash table function */
char *ht_strerror(int err);

/* Free the table and all key value pairs from memory */
void ht_free(Hashtable *ht);

/* Insert the string key with length keylen paired with value with length vallen */
/* into the table */
int ht_insert(Hashtable *ht, const char *key, size_t keylen, const char *val, size_t vallen);

/* Delete the specified key from the table */
/* Returns EKNOTFOUND if the key is not in the table */
int ht_delete(Hashtable *ht, const char *key, size_t keylen);

/* Lookup the specified key in the table */
/* Returns EKNOTFOUND if the key is not found in the table */
/* Places the value asociated with key in dst and updates dstlen with */
/* the amount written */
/* If dstlen is not big enough to store the value then the value is  */
/* truncated and ETRUNCATED is returned, dstlen wont be changed if ETRUNCATED is returned */
int ht_lookup(Hashtable *ht, const char *key, size_t keylen, char *dst, size_t *dstlen);

#endif
