# libhasht
libhasht is a small C module for creating and using hash tables

Hash tables store key value pairs, since it indexes using the hash value of the key it offers nearly constant time lookups for values.  It is not completely O(1) for lookups because when hash collisions occur the key value pairs are chained together in a linked list.  When searching for a value the lookup function must run through the list until it finds the value.

# The API

### NAME

ht_init, ht_strerror, ht_free, ht_insert, ht_delete, ht_lookup - Create and manage hash table

### SYNOPSIS

int ht_init(Hashtable ***res*, size_t *nelements*);
char *ht_strerror(int *err*);
void ht_free(Hashtable **ht*);
int ht_insert(Hashtable **ht*, char **key*, size_t *keylen*, char **val*, size_t *vallen*);
int ht_delete(Hashtable **ht*, char **key*, size_t *keylen*);
int ht_lookup(Hashtable **ht*, char **key*, size_t *keylen*, char **dst*, size_t **dstlen*);

### DESCRIPTION

The `ht_init` function creates a `Hashtable` structure with `nelements` number of elements and places a pointer to it in `res`.  Note, `nelements` is not the maximum number of keys that can be stored but the number of indexes used to store data.  The struct `Hashtable` used by the library contains the following feilds:
```C
typdef struct Hashtable{
	struct Ht_nodes  **nodes;
	size_t             nodes;
	size_t             entries;
} Hashtable;
```
The `ht_strerror` function converts an error code returned by a hash table function into a long format string.  It simply returns a pointer to a string literal.

The `ht_free` function takes in a pointer to the `Hastable` struct and frees all memory associated with the table.  After calling `ht_free` the pointer that was passed in should be treated as freed and not referenced by any hash table functions.

The `ht_insert` function is used to add key value pairs to the map.  The function adds the `key` and `value` with size `keylen` and `vallen` to the table pointed to by `ht`.

The `ht_delete` function searches for and removes the `key` with size `keylen` from the table pointed to by `ht`.  If the key is not found in the table then the function does nothing and returns `EKNOTFOUND`.

The `ht_lookup` function searches the table for `key` with size `keylen` and places the value in the buffer pointed to by `dst` with size `dstlen`.  If the key is not found in the table then the function returns `EKNOTFOUND`.  When the function successfully places the value in `dst` it updates `dstlen` with the actual size of the value that was returned.  If `dstlen` is smaller than the value, it will be truncated to the size `dstlen` and the function will return `ETRUNCATED`.

### RETURN VALUE

On success, all functions with return values return 0.  On failure, the functions will return a nonzero error code:

`EKNOTFOUND`: Returned by `ht_delete` or `ht_lookup` if the provided key was not found in the table.  This does not indicate an error in the function.

`EPTRINVALD`: Returned if a null pointer was provided to one of the functions.

`ENELINVALD`: An invalid value for `nelements` was provided to `ht_init`.

`EMEMFAILED`: A memory allocation operation failed.

`EHASHFFAIL`: There was an error when trying to hash the key.

`ETRUNCATED`: Returned by `ht_lookup` if the buffer provided by the caller is smaller than the value and the value was truncated.

### EXAMPLE

This example demonstrates most of the basic features of the module.

```C
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libhasht.h>

#define BUFFSZ	64
#define TABLESZ	4943

char *keys[] = {
	"foo",
	"bar",
	"fizz",
};

char *values[] = {
	"fizz",
	"buzz",
	"bazz",
};

int main(int argc, char *argv[]){
	if(argc < 3){
		printf("Usage: %s [lookup] [delete]\n"
		"lookup\tLookup in key in table\n"
		"delete\tDelete key from table\n", argv[0]);
		return 0;
	}

	Hashtable *ht;
	char buff[BUFFSZ] = {0};
	size_t bufflen = sizeof(buff);
	int err = 0;
	err = ht_init(&ht, TABLESZ);
	if(err){
		printf("Init error %s\n", ht_strerror(err));
		return 1;
	}

	for(int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++){
		printf("BALLS");
		err = ht_insert(ht, keys[i], strlen(keys[i]), values[i], strlen(values[i]));
		if(err){
			printf("Insert error %s\n", ht_strerror(err));
			ht_free(ht);
			return 1;
		}
	}

	err = ht_lookup(ht, argv[1], strlen(argv[1]), buff, &bufflen);
	if(!err){
		printf("KEY: %s | VALUE: %s\n", argv[1], buff);
	}
	else if(err == ETRUNCATED){
		printf("Value for key %s did not fit in buffer\n", argv[1]);
	}
	else if(err == EKNOTFOUND){
		printf("KEY: %s was not found in table\n", argv[1]);
	}
	else{
		printf("Lookup error %s\n", ht_strerror(err));
		ht_free(ht);
		return 1;
	}

	err = ht_delete(ht, argv[2], strlen(argv[2]));
	if(!err){
		printf("KEY: %s | Deleted from table\n", argv[2]);
	}
	else if(err == EKNOTFOUND){
		printf("KEY: %s was not found in table\n", argv[2]);
	}
	else{
		printf("Delete error %s\n", ht_strerror(err));
		ht_free(ht);
		return 1;
	}

	ht_free(ht);
	return 0;
}
```
