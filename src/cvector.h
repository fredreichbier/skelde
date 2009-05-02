/*
 *  CVector.h
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

#ifndef CVECTOR_H_INCLUDED
#define CVECTOR_H_INCLUDED

    /* CVector flags */
    
#define CVECTOR_FLAGS_NO_RELOCATION  1
#define CVECTOR_FLAGS_NO_RESIZE      2
    
    
    /* CVector error return values */
    
#define CVECTOR_MALLOC_FAILED -1
#define CVECTOR_BAD_ARGUMENT  -2
#define CVECTOR_NOT_FOUND     -4
#define CVECTOR_NO_RELOCATION -8
#define CVECTOR_NO_RESIZE     -16
    
#include <stdlib.h>
#include <string.h>

#define FAR
#define MALLOC sk_malloc
#define FREE sk_free
#define MEMSET memset
#define MEMMOVE memmove

typedef struct {
    size_t size;             /* size of the vector      */
    size_t capacity;         /* capacity of the vector  */
    size_t elementsize;      /* size of an element      */
    void FAR * array;        /* the array of elements   */
    unsigned int flags;      /* flags                   */
} CVector;

typedef CVector FAR * CVectorHandle;


/*  cvector_add_element -- add an element to a CVector */

int cvector_add_element(const CVectorHandle vectorhandle, const void FAR * element);

/* cvector_capacity -- macro to return the CVector capacity */

#define cvector_capacity(vectorhandle)  (vectorhandle)->capacity

/* cvector_clear -- clear a generic vector */

int cvector_clear(const CVectorHandle vectorhandle);

/* cvector_create -- create a CVector */

int cvector_create(CVectorHandle FAR * vectorhandle, const size_t elementsize, const size_t capacity);
    
/* cvector_free -- remove a CVector */

int cvector_free(CVectorHandle FAR * vectorhandle);

/* cvector_get_capacity - function to return the CVector capacity */

int cvector_get_capacity(const CVectorHandle vectorhandle, size_t FAR * capacity);

/* cvector_get_element -- get a copy of an element from a CVector */

int cvector_get_element(const CVectorHandle vectorhandle, void FAR * element, const size_t index);

/* cvector_get_elementptr -- get a pointer to an element from a CVector */

int cvector_get_elementptr(const CVectorHandle vectorhandle, void FAR ** elementptr, const size_t index);

/* CVectorGetFlags - function to return the CVector flags */

int CVectorGetFlags(const CVectorHandle vectorhandle, unsigned int FAR * flags);

/* cvector_get_size - function to return the CVector size */

int cvector_get_size(const CVectorHandle vectorhandle, size_t FAR * size);
    
/* cvector_remove_element -- remove an element from a generic vector */

int cvector_remove_element(const CVectorHandle vectorhandle, const size_t index);

/* cvector_set_capacity - function to set the CVector capacity */

int cvector_set_capacity(const CVectorHandle vectorhandle, const size_t capacity);       
    
/* cvector_set_element -- set a copy of an element into a CVector */

int cvector_set_element(const CVectorHandle vectorhandle, const void FAR * element, const size_t index);

/* cvector_set_flags - function to set the CVector flags */

int cvector_set_flags(const CVectorHandle vectorhandle, const unsigned int flags);

/* cvector_set_size - function to set the CVector size */

int cvector_set_size(const CVectorHandle vectorhandle, const size_t size);

/* cvector_size -- macro to return the CVector size */

#define cvector_size(vectorhandle)  (vectorhandle)->size

#define cvector_push(VEC, ITEM) \
        cvector_add_element(VEC, ITEM)
#define cvector_pop(VEC) \
        cvector_remove_element(VEC, cvector_size(VEC) - 1)

#endif
