#include <stdio.h>
#include <stdlib.h>

#include <libconcord.h>

#include "concord-common.h"
#include "debug.h"


#define M_OUT stderr
#define M_FMT_PREFIX "[%s:%d] %s()\n\t"
#define M_FMT_ARGS file, line, func


void*
__safe_calloc(size_t nmemb, size_t size, const char file[], const int line, const char func[])
{
    void *ptr = calloc(nmemb, size);
    ASSERT_S(NULL != ptr, "Out of memory(%ld bytes)");

#if MEMDEBUG_MODE == 1
    fprintf(M_OUT, M_FMT_PREFIX "Alloc:\t%p(%ld bytes)\n", M_FMT_ARGS, ptr, size);
#else
    (void)file;
    (void)line;
    (void)func;
#endif

    return ptr;
}

void*
__safe_malloc(size_t size, const char file[], const int line, const char func[])
{
    void *ptr = malloc(size);
    ASSERT_S(NULL != ptr, "Out of memory(%ld bytes)");

#if MEMDEBUG_MODE == 1
    fprintf(M_OUT, M_FMT_PREFIX "Alloc:\t%p(%ld bytes)\n", M_FMT_ARGS, ptr, size);
#else
      (void)file;
      (void)line;
      (void)func;
#endif

    return ptr;
}

void*
__safe_realloc(void *ptr, size_t size, const char file[], const int line, const char func[])
{
    void *tmp = realloc(ptr, size);
    ASSERT_S(NULL != ptr, "Out of memory(%ld bytes)");

#if MEMDEBUG_MODE == 1
    fprintf(M_OUT, M_FMT_PREFIX "Alloc:\t%p(%ld bytes)\n", M_FMT_ARGS, ptr, size);
#else
    (void)file;
    (void)line;
    (void)func;
#endif

    return tmp;
}

void
__safe_free(void **p_ptr, const char file[], const int line, const char func[])
{
    if(*p_ptr)
    {
        free(*p_ptr);
#if MEMDEBUG_MODE == 1
        fprintf(M_OUT, M_FMT_PREFIX "Free:\t%p\n", M_FMT_ARGS, *p_ptr);
#else
        (void)file;
        (void)line;
        (void)func;
#endif

        *p_ptr = NULL;
    } 
}

