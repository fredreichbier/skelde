/*
 * ... from http://elliottback.com/wp/hashmap-implementation-in-c/
 * modified to use the GC and some minor things
 * thanks!
 */

#include <stdlib.h>
#include "mem.h"
#include "hashmap.h"

#define INITIAL_SIZE 1024
/*
 * Return an empty hashmap, or NULL on failure.
 */
Hashmap *hashmap_new() {
	Hashmap* m = (Hashmap*) sk_malloc(sizeof(Hashmap));
	if(!m) goto err;

	m->data = (HashmapElement*) sk_calloc(INITIAL_SIZE, sizeof(HashmapElement));
	if(!m->data) goto err;

	m->table_size = INITIAL_SIZE;
	m->size = 0;

	return m;
	err:
		if (m)
			hashmap_free(m);
		return NULL;
}

/* This function is originally called SuperFastHash.
 * from http://www.azillionmonkeys.com/qed/hash.html
 * Thanks ;-)
 */
uint32_t hashmap_hash_string(const char * data, int len) {
    uint32_t hash = len, tmp;
    int rem;
    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= data[sizeof (uint16_t)] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

uint32_t hashmap_hash_bstring(bstring bs) {
    return hashmap_hash_string(bstr2cstr(bs, ':'), blength(bs)); // TODO: really convert all \0 to ':'?
}

/*
 * Hashing function for an integer
 */
unsigned int hashmap_hash_int(Hashmap * m, unsigned int key){
	/* Robert Jenkins' 32 bit Mix Function */
	key += (key << 12);
	key ^= (key >> 22);
	key += (key << 4);
	key ^= (key >> 9);
	key += (key << 10);
	key ^= (key >> 2);
	key += (key << 7);
	key ^= (key >> 12);

	/* Knuth's Multiplicative Method */
	key = (key >> 3) * 2654435761UL;

	return key % m->table_size;
}

/*
 * Return the integer of the location in data
 * to store the point to the item, or MAP_FULL.
 */
int hashmap_hash(Hashmap *in, int key){
	int curr;
	int i;

	/* Cast the hashmap */
	Hashmap* m = (Hashmap *) in;

	/* If full, return immediately */
	if(m->size == m->table_size) return MAP_FULL;

	/* Find the best index */
	curr = hashmap_hash_int(m, key);

	/* Linear probling */
	for(i = 0; i< m->table_size; i++){
		if(m->data[curr].in_use == 0)
			return curr;

		if(m->data[curr].key == key && m->data[curr].in_use == 1)
			return curr;

		curr = (curr + 1) % m->table_size;
	}

	return MAP_FULL;
}

/*
 * Doubles the size of the hashmap, and rehashes all the elements
 */
int hashmap_rehash(Hashmap *in){
	int i;
	int old_size;
	HashmapElement* curr;

	/* Setup the new elements */
	Hashmap *m = (Hashmap *) in;
	HashmapElement* temp = (HashmapElement *)
		sk_calloc(2 * m->table_size, sizeof(HashmapElement));
	if(!temp) return MAP_OMEM;

	/* Update the array */
	curr = m->data;
	m->data = temp;

	/* Update the size */
	old_size = m->table_size;
	m->table_size = 2 * m->table_size;
	m->size = 0;

	/* Rehash the elements */
	for(i = 0; i < old_size; i++){
		int status = hashmap_put(m, curr[i].key, curr[i].data);
		if (status != MAP_OK)
			return status;
	}

//	free(curr);

	return MAP_OK;
}

/*
 * Add a pointer to the hashmap with some key
 */
int hashmap_put(Hashmap *in, int key, Any value){
	int index;
	Hashmap* m;

	/* Cast the hashmap */
	m = (Hashmap *) in;

	/* Find a place to put our value */
	index = hashmap_hash(in, key);
	while(index == MAP_FULL){
		if (hashmap_rehash(in) == MAP_OMEM) {
			return MAP_OMEM;
		}
		index = hashmap_hash(in, key);
	}

	/* Set the data */
	m->data[index].data = value;
	m->data[index].key = key;
	m->data[index].in_use = 1;
	m->size++; 

	return MAP_OK;
}

/*
 * Get your pointer out of the hashmap with a key
 */
int hashmap_get(Hashmap *in, int key, Any *arg){
	int curr;
	int i;
	Hashmap* m;

	/* Cast the hashmap */
	m = (Hashmap *) in;

	/* Find data location */
	curr = hashmap_hash_int(m, key);

	/* Linear probing, if necessary */
	for(i = 0; i< m->table_size; i++){

		if(m->data[curr].key == key && m->data[curr].in_use == 1){
			*arg = (m->data[curr].data);
			return MAP_OK;
		}

		curr = (curr + 1) % m->table_size;
	}

	*arg = NULL;

	/* Not found */
	return MAP_MISSING;
}

/*
 * Get a random element from the hashmap
 */
int hashmap_get_one(Hashmap *in, Any *arg, int remove){
	int i;
	Hashmap* m;

	/* Cast the hashmap */
	m = (Hashmap *) in;

	/* On empty hashmap return immediately */
	if (hashmap_length(m) <= 0)
		return MAP_MISSING;

	/* Linear probing */
	for(i = 0; i< m->table_size; i++)
		if(m->data[i].in_use != 0){
			*arg = (Any) (m->data[i].data);
			if (remove) {
				m->data[i].in_use = 0;
				m->size--;
			}
			return MAP_OK;
		}

	return MAP_OK;
}

/*
 * Iterate the function parameter over each element in the hashmap.  The
 * additional Any argument is passed to the function as its first
 * argument and the hashmap element is the second.
 */
int hashmap_iterate(Hashmap *in, PFany f, Any item) {
	int i;

	/* Cast the hashmap */
	Hashmap* m = (Hashmap*) in;

	/* On empty hashmap, return immediately */
	if (hashmap_length(m) <= 0)
		return MAP_MISSING;	

	/* Linear probing */
	for(i = 0; i< m->table_size; i++)
		if(m->data[i].in_use != 0) {
			Any data = (Any) (m->data[i].data);
			int status = f(item, data);
			if (status != MAP_OK) {
				return status;
			}
		}

        return MAP_OK;
}

/*
 * Remove an element with that key from the map
 */
int hashmap_remove(Hashmap *in, int key){
	int i;
	int curr;
	Hashmap* m;

	/* Cast the hashmap */
	m = (Hashmap *) in;

	/* Find key */
	curr = hashmap_hash_int(m, key);

	/* Linear probing, if necessary */
	for(i = 0; i< m->table_size; i++){
		if(m->data[curr].key == key && m->data[curr].in_use == 1){
			/* Blank out the fields */
			m->data[curr].in_use = 0;
			m->data[curr].data = NULL;
			m->data[curr].key = 0;

			/* Reduce the size */
			m->size--;
			return MAP_OK;
		}
		curr = (curr + 1) % m->table_size;
	}

	/* Data not found */
	return MAP_MISSING;
}

/* Deallocate the hashmap */
void hashmap_free(Hashmap *in){
	/*Hashmap* m = (Hashmap*) in;
	free(m->data);
	free(m);*/
    // noop, because we use a GC
}

/* Return the length of the hashmap */
int hashmap_length(Hashmap *in){
	Hashmap* m = (Hashmap *) in;
	if(m != NULL) return m->size;
	else return 0;
}
