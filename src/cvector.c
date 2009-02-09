/*
 *  CVector.c
 *  CVector
 *
 * Modified by Friedrich Weber to match my naming conventions ;-)
 *
 *  Created by Herbert J. Bernstein on 11/28/08.
 *  Copyright 2008 Herbert J. Bernstein. All rights reserved.
 *
 */


/**********************************************************************
 *                                                                    *
 * YOU MAY REDISTRIBUTE THE CVector API UNDER THE TERMS OF THE LGPL   *
 *                                                                    *
 **********************************************************************/

/************************* LGPL NOTICES *******************************
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU Lesser General Public         *
 * License as published by the Free Software Foundation; either       *
 * version 2.1 of the License, or (at your option) any later version. *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 * Lesser General Public License for more details.                    *
 *                                                                    *
 * You should have received a copy of the GNU Lesser General Public   *
 * License along with this library; if not, write to the Free         *
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,    *
 * MA  02110-1301  USA                                                *
 *                                                                    *
 **********************************************************************/

#include "mem.h"

#include "cvector.h"

/* cvector_create -- create a generic vector */

int cvector_create(CVectorHandle FAR * vectorhandle, const size_t elementsize, const size_t capacity) {
    
    size_t cap = capacity;
    
    if ((vectorhandle==NULL)) { return CVECTOR_BAD_ARGUMENT; }
    
    *vectorhandle = (CVectorHandle)MALLOC(sizeof(CVector));
    
    if ((*vectorhandle==NULL)) {
        return CVECTOR_MALLOC_FAILED;
    }
    
    (*vectorhandle)->size = 0;
    (*vectorhandle)->flags = 0;
    (*vectorhandle)->capacity = 0;
    (*vectorhandle)->elementsize = elementsize;
    if (!cap) { cap = 10; }
    (*vectorhandle)->array = (void FAR *)MALLOC(cap*elementsize);
    if ((*vectorhandle)->array) {
        (*vectorhandle)->capacity = cap;
        return 0;
    }
    FREE(*vectorhandle);
    *vectorhandle = NULL;
    return CVECTOR_MALLOC_FAILED;
}


/*  cvector_add_element -- add an element to a generic vector 
 equivalent to vector::push_back  */


int cvector_add_element(const CVectorHandle vectorhandle, const void FAR * element) {
    
    size_t newcap;
    
    int errorcode;
    
    if ((vectorhandle==NULL)) { return CVECTOR_BAD_ARGUMENT; }
    
    if ( (vectorhandle->flags&CVECTOR_FLAGS_NO_RESIZE) ) { return CVECTOR_NO_RESIZE; }
    
    if (vectorhandle->size >= vectorhandle->capacity) {
        
        newcap = vectorhandle->capacity*2;
        
        if (newcap < 1) { newcap = 1; }
        
        errorcode = cvector_set_capacity (vectorhandle, newcap);
        
        if (errorcode != 0) {
            
            newcap = vectorhandle->capacity+(size_t)(vectorhandle->capacity>>2);
            
            if (newcap < 1) { newcap = 1; }
            
            errorcode = cvector_set_capacity (vectorhandle, newcap);
            
            if (errorcode != 0) { return errorcode; }
        }
        
    }
    
    MEMMOVE(((char FAR *)(vectorhandle->array))+vectorhandle->size*vectorhandle->elementsize,
            (const char FAR *)element, vectorhandle->elementsize);
    vectorhandle->size ++;
    return 0;
    
}

/* cvector_get_element -- get a copy of an element from a generic vector */

int cvector_get_element(const CVectorHandle vectorhandle, void FAR * element, const size_t index) {
    
    if ((vectorhandle==NULL)) { return CVECTOR_BAD_ARGUMENT; }
    
    if (index >= 0 && index < vectorhandle->size) {
        
        MEMMOVE((char *)element,((char *)(vectorhandle->array))+index*vectorhandle->elementsize,
                vectorhandle->elementsize);
        
        return 0;
        
    } else {
        
        return CVECTOR_NOT_FOUND;
    }
    
}


/* cvector_get_elementptr -- get a pointer to an element from a generic vector */

int cvector_get_elementptr(const CVectorHandle vectorhandle, void FAR ** elementptr, const size_t index) {
    
    if ((vectorhandle==NULL)) { return CVECTOR_BAD_ARGUMENT; }
    
    if (index >= 0 && index < vectorhandle->size) {
        
        *elementptr = (void FAR*)(((char *)(vectorhandle->array))+index*vectorhandle->elementsize);
        
        vectorhandle->flags |= CVECTOR_FLAGS_NO_RELOCATION;
        
        return 0;
        
    } else {
        
        return CVECTOR_NOT_FOUND;
    }
    
}


/* cvector_set_element -- set a copy of an element into a generic vector */

int cvector_set_element(const CVectorHandle vectorhandle, const void FAR * element, const size_t index) {
    
    size_t newcap;
    
    int errorcode;
    
    if ((vectorhandle==NULL) ) { return CVECTOR_BAD_ARGUMENT; }
    
    if (index >= vectorhandle->capacity) {
        
        newcap = index+vectorhandle->capacity+1;
        
        errorcode = cvector_set_capacity(vectorhandle, newcap);
        
        if (errorcode != 0 ) {
            
            newcap = index*1.2;
            if (newcap < index+128) { newcap = index+128; }
            
            errorcode = cvector_set_capacity(vectorhandle, newcap);
            
            if (errorcode != 0) { return errorcode; }
            
        }
    }
    
    
    if (index >= 0 && index < vectorhandle->capacity) {
        
        MEMMOVE(((char *)(vectorhandle->array))+index*vectorhandle->elementsize,(char *)element,
                vectorhandle->elementsize);
        
        if (index >= vectorhandle->size) { vectorhandle->size = index+1; }
        return 0;
        
    } else {
        
        return CVECTOR_NOT_FOUND;
    }
    
}

/* cvector_remove_element -- remove an element from a generic vector */

/* keeps elements 0 -- index-1, discards element index
 moves elements index+1 through vectorhandle->size-1
 into element index through vectorhandle->size-2
 
 i.e. moves characters (index+1)*(vectorhandle->elementsize)
 through (vectorhandle->size)*(vectorhandle->elementsize)-1
 to index*(vectorhandle->elementsize)
 */

int cvector_remove_element(const CVectorHandle vectorhandle, const size_t index) {
    
    if ((vectorhandle==NULL)) { return CVECTOR_BAD_ARGUMENT; }
    
    if ((vectorhandle->flags&CVECTOR_FLAGS_NO_RELOCATION)) { return CVECTOR_NO_RELOCATION; }
    
    if (index >= vectorhandle->size || index < 0 ) { return CVECTOR_NOT_FOUND; }
    
    if (index == vectorhandle->size-1) {
        vectorhandle->size--;
        return 0;
    }
    
    MEMMOVE((char *)vectorhandle->array+index*(vectorhandle->elementsize),
            (char *)vectorhandle->array+(index+1)*(vectorhandle->elementsize),
            (vectorhandle->size-1-index)*(vectorhandle->elementsize));
    
    vectorhandle->size--;
    return 0;
}

/* cvector_clear -- clear a generic vector */

int cvector_clear(const CVectorHandle vectorhandle) {
    
    if ((vectorhandle==NULL)) { return CVECTOR_BAD_ARGUMENT; }
    
    if (vectorhandle->flags & CVECTOR_FLAGS_NO_RESIZE) { return CVECTOR_NO_RESIZE; }
    
    vectorhandle->size = 0;
    
    return 0;
    
}

/* cvector_free -- remove a generic vector */

int cvector_free(CVectorHandle FAR * vectorhandle) {
    
    if ((vectorhandle==NULL)) { return CVECTOR_BAD_ARGUMENT; }
    
    if (*vectorhandle) {
        
        if ((*vectorhandle)->flags & CVECTOR_FLAGS_NO_RESIZE) { return CVECTOR_NO_RESIZE; }
        
        if ((*vectorhandle)->flags & CVECTOR_FLAGS_NO_RELOCATION) { return CVECTOR_NO_RELOCATION; }
        
        if ((*vectorhandle)->array) {
            
            FREE((*vectorhandle)->array);
            
        }
        
        FREE(*vectorhandle);
        
    }
    
    *vectorhandle = 0;
    
    return 0;
    
}

/* cvector_get_capacity - function to return the CVector capacity */

int cvector_get_capacity(const CVectorHandle vectorhandle, size_t FAR * capacity) {
    
    if ((vectorhandle==NULL)||!(capacity)) { return CVECTOR_BAD_ARGUMENT; }
    
    *capacity = vectorhandle->capacity;
    
    return 0;
}

/* cvector_get_size - function to return the CVector size */

int cvector_get_size(const CVectorHandle vectorhandle, size_t FAR * size) {
    
    if ((vectorhandle==NULL)||!(size)) { return CVECTOR_BAD_ARGUMENT; }
    
    *size = vectorhandle->size;
    
    return 0;
}

/* cvector_get_flags - function to return the CVector flags */

int cvector_get_flags(const CVectorHandle vectorhandle, unsigned int FAR * flags) {
    
    if ((vectorhandle==NULL)||!(flags)) { return CVECTOR_BAD_ARGUMENT; }
    
    *flags = vectorhandle->flags;
    
    return 0;
}

/* cvector_set_capacity - function to set the CVector capacity */

int cvector_set_capacity(const CVectorHandle vectorhandle, const size_t capacity) {
    
    void FAR * temparray;
    
    if ((vectorhandle==NULL) || capacity < vectorhandle->size) { return CVECTOR_BAD_ARGUMENT; }
    
    if (capacity == vectorhandle->capacity) { return 0; }
    
    if (vectorhandle->flags&CVECTOR_FLAGS_NO_RELOCATION) { return CVECTOR_NO_RELOCATION; }
    
    if (capacity) {
        
        temparray = MALLOC(capacity*vectorhandle->elementsize);
        if (!temparray)   { return CVECTOR_MALLOC_FAILED; }
        
        if (vectorhandle->size) {   
          MEMMOVE((char *)temparray, (char *)vectorhandle->array, vectorhandle->size*vectorhandle->elementsize); 
        }
        FREE(vectorhandle->array);
        
    } else {
        temparray = NULL;
    }
    
    vectorhandle->array = temparray;
    vectorhandle->capacity = capacity;
    
    return 0;
}

/* cvector_set_size - function to set the CVector size */

int cvector_set_size(const CVectorHandle vectorhandle, const size_t size) {
    
    int errorcode;
    
    if ((vectorhandle==NULL) ) { return CVECTOR_BAD_ARGUMENT; }
    
    if (size == vectorhandle->size) { return 0; }
    
    if ((vectorhandle->flags & CVECTOR_FLAGS_NO_RESIZE)) { return CVECTOR_NO_RESIZE; }
    
    if ( size > vectorhandle->capacity ) {
        
        if ((vectorhandle->flags & CVECTOR_FLAGS_NO_RELOCATION) ) { return CVECTOR_NO_RELOCATION; }
        
        errorcode = cvector_set_capacity(vectorhandle,size);
        
        if (errorcode != 0) { return errorcode; }
        
    }
    
    if ( size <= vectorhandle->capacity ) {
        
        if (size > vectorhandle->size) {
            
            MEMSET(((char *)vectorhandle->array)+(vectorhandle->size)*(vectorhandle->elementsize),
                   0, (vectorhandle->size-size)*(vectorhandle->elementsize));
        }
        
        vectorhandle->size = size;
        
    }
    
    
    return 0;
}


/* cvector_set_fags - function to set the CVector flags */

int cvector_set_flags(const CVectorHandle vectorhandle, const unsigned int flags) {
    
    if ((vectorhandle==NULL) ) { return CVECTOR_BAD_ARGUMENT; }
    
    vectorhandle->flags = flags;
    
    return 0;
    
}

void *cvector_top(const CVectorHandle vectorhandle) {
    void *ptr;
    cvector_get_elementptr(vectorhandle, &ptr, cvector_size(vectorhandle) - 1); // TODO: error checking
    return ptr;
}
