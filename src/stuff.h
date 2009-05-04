#ifndef STUFF_H_INCLUDED
#define STUFF_H_INCLUDED

#include <stdarg.h>
#include <stdio.h>

#include <pthread.h>

/* hm, that shouldn't be here. TODO */
#define TRUE 1
#define FALSE 0

typedef unsigned char ArgCount;

int sk_printf(const char *format, ...);

#endif
