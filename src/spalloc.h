/* Copyright 2022 Mathieu Bourquenoud
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SPALLOC_H
#define SPALLOC_H

#include <stddef.h>
#include <stdint.h>

#ifdef SPALLOC_DEBUG
#include <stdio.h>
#define _spalloc_print_log(msg, ...) fprintf(stderr, (msg), __VA_ARGS__)
#else
#define _spalloc_print_log(msg, ...)
#endif

#define sp_t(type) sp_##type##_t

// Enable auto smart pointer only if the compiler can use the cleanup attribute
#if defined __has_attribute
#if __has_attribute(cleanup) && !defined(SPALLOC_NO_AUTO_CLEANUP)
#ifdef SPALLOC_DEBUG
// Cleanup logs messages
#define sp_auto_t(type) sp_##type##_t __attribute__((cleanup(__sp_cleanup_debug)))
#else
// Use the non-debug cleanup
#define sp_auto_t(type) sp_##type##_t __attribute__((cleanup(__sp_cleanup)))
#endif
#endif
#endif

#if __STDC_VERSION__ >= 201100L && !defined(__STDC_NO_ATOMICS__) && defined(SPALLOC_THREAD_SAFE)
#define SP_DEF(type)                         \
    typedef struct                           \
    {                                        \
        type *ptr;                           \
        _Atomic volatile uint32_t *refcount; \
    } sp_##type##_t;
#else
#define SP_DEF(type)        \
    typedef struct          \
    {                       \
        type *ptr;          \
        uint32_t *refcount; \
    } sp_##type##_t;
#endif

// Decrement the reference count and free the memory if the number of
// references reached zero
#define spdel(sp) \
    ({                                                                   \
        _spalloc_print_log("Line %i: Decrementing smart pointer %p,"     \
                           "there are now %u refs.\n",                   \
                           __LINE__,                                     \
                           (sp).ptr,                                     \
                           *(sp).refcount - 1);                          \
        if (--(*(sp).refcount) == 0)                                     \
        {                                                                \
            _spalloc_print_log("Line %i: Destroying smart pointer %p.\n",\
                               __LINE__,                                 \
                               (sp).ptr);                                \
                                                                         \
            free((sp).ptr);                                              \
                                                                         \
            /* cast to "void*" to avoid a */                             \
            /* "discard qualifier" warning */                            \
            free((void*)(sp).refcount);                                  \
        }                                                                \
        (sp).refcount = NULL;                                            \
        while(0); })

// Return a copy of the smart pointer and increment the reference count
#define spcopy(src) ({                                                 \
    (*(src).refcount)++;                                               \
    _spalloc_print_log("Line %i: Creating a copy of smart pointer %p," \
                       "there are now %u refs.\n",                     \
                       __LINE__,                                       \
                       (src).ptr,                                      \
                       *(src).refcount - 1);                           \
    (typeof(src)){.ptr = (src).ptr, .refcount = (src).refcount};       \
})

// Return the a copy of the smart pointer and delete the original smart
// pointer
#define spmove(src) ({                                                \
    _spalloc_print_log("Line %i: Moving smart pointer %p,\n" __LINE__,\
                       (sp).ptr);                                     \
    typeof(src) new_ptr = (src);                                      \
    (src) = typeof(src){NULL, NULL};                                  \
    new_ptr;                                                          \
})

// Return the value pointed by the smart pointer and delete the smart pointer
#define spextract(src) ({                \
    typeof(*(src).ptr) res = *(src).ptr; \
    spdel(src);                          \
    res;                                 \
})

#define spmalloc(type, count) ({                        \
    sp_##type##_t new_sp;                               \
    new_sp.ptr = malloc(count * sizeof(type));          \
    new_sp.refcount = malloc(sizeof(new_sp.refcount));  \
    *(new_sp.refcount) = 1;                             \
    new_sp; })

#define spcalloc(type, count) ({                        \
    sp_##type##_t new_sp;                               \
    new_sp.ptr = calloc(count * sizeof(type));          \
    new_sp.refcount = malloc(sizeof(new_sp.refcount));  \
    *(new_sp.refcount) = 1;                             \
    new_sp; })

#define spresize(sp, count) ({                                  \
    _spalloc_print_log("%s",                                    \
    (*(sp).refcount) > 1 ?                                      \
        "WARNING : resizing a smart pointer with more than"     \
        " one reference, this can lead to data corruption.\n"   \
        : "");                                                  \
    (sp).ptr = realloc((sp).ptr, count * sizeof(typeof(src)));  \
    while(0); })

// Define smart pointers for basic types.
// Note that it's missing long doubles and complex types (and decimal floats
// for C23)
SP_DEF(char);
SP_DEF(void);
SP_DEF(uint8_t);
SP_DEF(int8_t);
SP_DEF(uint16_t);
SP_DEF(int16_t);
SP_DEF(uint32_t);
SP_DEF(int32_t);
SP_DEF(uint64_t);
SP_DEF(int64_t);
SP_DEF(float);
SP_DEF(double);

// String defintion
typedef char *char_ptr;
SP_DEF(char_ptr);

// Cleanup function for auto smart pointers
void __sp_cleanup(void *);
void __sp_cleanup_debug(void *ptr_to_sp);

#endif //