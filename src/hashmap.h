/*
 * Generic hashmap manipulation functions
 */
#ifndef HASHMAP_H_INCLUDED
#define HASHMAP_H_INCLUDED

#include <stdint.h>

#include "bstrlib.h"

#define MAP_MISSING -3  /* No such element */
#define MAP_FULL -2 	/* Hashmap is full */
#define MAP_OMEM -1 	/* Out of Memory */
#define MAP_OK 0 	/* OK */

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

/*
 * Any is a pointer.  This allows you to put arbitrary structures in
 * the hashmap.
 */
typedef void *Any;

// We need to keep keys and values
typedef struct _hashmap_element{
	int key;
	int in_use;
	Any data;
} HashmapElement;

// A hashmap has some maximum size and current size,
// as well as the data to hold.
typedef struct _hashmap_map{
	int table_size;
	int size;
	HashmapElement *data;
} Hashmap;

uint32_t hashmap_hash_string(const char * data, int len);
uint32_t hashmap_hash_bstring(const_bstring bs);

/*
 * PFany is a pointer to a function that can take two Any arguments
 * and return an integer. Returns status code..
 */
typedef int (*PFany)(Any, Any);

/*
 * Return an empty hashmap. Returns NULL if empty.
*/
extern Hashmap *hashmap_new();

/*
 * Iteratively call f with argument (item, data) for
 * each element data in the hashmap. The function must
 * return a map status code. If it returns anything other
 * than MAP_OK the traversal is terminated. f must
 * not reenter any hashmap functions, or deadlock may arise.
 */
extern int hashmap_iterate(Hashmap *in, PFany f, Any item);

/*
 * Add an element to the hashmap. Return MAP_OK or MAP_OMEM.
 */
extern int hashmap_put(Hashmap *in, int key, Any value);

/*
 * Get an element from the hashmap. Return MAP_OK or MAP_MISSING.
 */
extern int hashmap_get(Hashmap *in, int key, Any *arg);

/*
 * Remove an element from the hashmap. Return MAP_OK or MAP_MISSING.
 */
extern int hashmap_remove(Hashmap *in, int key);

/*
 * Get any element. Return MAP_OK or MAP_MISSING.
 * remove - should the element be removed from the hashmap
 */
extern int hashmap_get_one(Hashmap *in, Any *arg, int remove);

/*
 * Free the hashmap
 */
extern void hashmap_free(Hashmap *in);

/*
 * Get the current size of a hashmap
 */
extern int hashmap_length(Hashmap *in);

#endif
