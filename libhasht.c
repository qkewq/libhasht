#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "libhasht.h"

/* Nodes that will store the key value pairs in linked list */
typedef struct Ht_node{
	struct Ht_node *next;
	char *key;
	size_t keylen;
	char *val;
	size_t vallen;
} Ht_node;

Hashtable *ht_init(size_t nelements, int *err){
	if(!nelements){
		if(err){
			*err = ENELINVALD;
		}
		return NULL;
	}
	Hashtable *ht = calloc(1, sizeof(Hashtable));
	Ht_node **nodes = calloc(nelements, sizeof(Ht_node *));
	if(!ht || !nodes){
		free(ht);
		free(nodes);
		if(err){
			*err = EMEMFAILED;
		}
		return NULL;
	}
	ht->nnodes = nelements;
	ht->nodes = nodes;

	return ht;
}

char *ht_strerror(int err){
	switch(err){
		case EKNOTFOUND:
			return "EKNOTFOUND: The key was not found in the table";
		case SUCCESSFUL:
			return "Operation completed successfully";
		case EPTRINVALD:
			return "EPTRINVALD: Invalid pointer";
		case ENELINVALD:
			return "ENELINVALD: Invalid number of elements";
		case EMEMFAILED:
			return "EMEMINVALD: Memory allocation failed";
		case EHASHFFAIL:
			return "EHASHFFAIL: Failed to hash key";
		case ETRUNCATED:
			return "ETRUNCATED: Provided buffer was too small";
		default:
			return "Invalid error code";
	}
}

void ht_free(Hashtable *ht){
	if(!ht){
		return;
	}

	Ht_node *current = NULL;
	Ht_node *next = NULL;
	for(int i = 0; i < ht->nnodes; i++){
		current = ht->nodes[i];
		while(current){
			next = current->next;
			free(current->key);
			free(current->val);
			free(current);
			current = next;
		}
	}
	free(ht->nodes);
	free(ht);
}

int ht_hash(char *key, size_t keylen){
	int ret = 1543; // magic number
	for(int i = 0; i < keylen; i++){
		ret += ret * (key[i] * (i + 1)); /* +1 to avoid multiplying by 0 */
	}

	return ret;
}

int ht_get_index(char *key, size_t keylen, size_t nnodes){
	int index = ht_hash(key, keylen);
	index = index * (int)nnodes;

	return index % (int)nnodes;
}

int ht_insert(Hashtable *ht, const char *key, size_t keylen, const char *val, size_t vallen){
	if(!ht || !key){
		return EPTRINVALD;
	}

	int index = ht_get_index(key, keylen, ht->nnodes);

	Ht_node *new = calloc(1, sizeof(Ht_node));
	char *stored_key = calloc(1, keylen);
	char *stored_val = calloc(1, vallen);
	if(!new || !stored_key || !stored_val){
		free(new);
		free(stored_key);
		free(stored_val);
		return EMEMFAILED;
	}
	memcpy(stored_key, key, keylen);
	memcpy(stored_val, val, vallen);
	new->key = stored_key;
	new->keylen = keylen;
	new->val = stored_val;
	new->vallen = vallen;

	new->next = ht->nodes[index];
	ht->nodes[index] = new;
	ht->entries++;

	return SUCCESSFUL;
}

int ht_delete(Hashtable *ht, const char *key, size_t keylen){
	if(!ht || !key){
		return EPTRINVALD;
	}

	int index = ht_get_index(key, keylen, ht->nnodes);

	Ht_node *previous = ht->nodes[index];
	Ht_node *current = previous;
	while(current){
		if(current->keylen != keylen){
			previous = current;
			current = current->next;
			continue;
		}
		if(memcmp(current->key, key, keylen) == 0){
			if(current == previous){
				ht->nodes[index] = current->next;
			}
			else{
				previous->next = current->next;
			}
			free(current->key);
			free(current->val);
			free(current);
			ht->entries--;
			return SUCCESSFUL;
		}
		else{
			previous = current;
			current = current->next;
			continue;
		}
	}

	return EKNOTFOUND;
}

int ht_lookup(Hashtable *ht, const char *key, size_t keylen, char *dst, size_t *dstlen){
	if(!ht || !key || !dst || !dstlen){
		return EPTRINVALD;
	}

	int index = ht_get_index(key, keylen, ht->nnodes);

	Ht_node *current = ht->nodes[index];
	while(current){
		if(current->keylen != keylen){
			current = current->next;
			continue;
		}
		if(memcmp(current->key, key, keylen) == 0){
			if(*dstlen < current->vallen){
				memcpy(dst, current->val, *dstlen);
				return ETRUNCATED;
			}
			else{
				memcpy(dst, current->val, current->vallen);
				*dstlen = current->vallen;
				return SUCCESSFUL;
			}
		}
		else{
			current = current->next;
			continue;
		}
	}

	return EKNOTFOUND;
}

int ht_resize(Hashtable *ht, size_t nelements){
	if(!ht){
		return EPTRINVALD;
	}
	if(!nelements){
		return ENELINVALD;
	}

	Ht_node **new_nodes = calloc(nelements, sizeof(Ht_node *));
	if(!new_nodes){
		return EMEMFAILED;
	}

	Ht_node *current = NULL;
	Ht_node *next = NULL;
	for(int i = 0; i < ht->nnodes; i++){
		current = ht->nodes[i];
		while(current){
			next = current->next;
			int index = ht_get_index(current->key, current->keylen, nelements);
			current->next = new_nodes[index];
			new_nodes[index] = current;
			current = next;
		}
	}

	free(ht->nodes);
	ht->nnodes = nelements;
	ht->nodes = new_nodes;

	return SUCCESSFUL;
}
