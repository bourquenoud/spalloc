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

#include <stdlib.h>
#include <stdio.h>

#include "spalloc.h"

void __sp_cleanup(void *ptr_to_sp)
{
    sp_t(void) cleanup_sptr = *((sp_t(void) *)(ptr_to_sp));

    if (cleanup_sptr.refcount == NULL)
        return;

    if (--(*cleanup_sptr.refcount) == 0)
    {
        free(cleanup_sptr.ptr);
        free((void *)cleanup_sptr.refcount);
        ((sp_t(void) *)(ptr_to_sp))->refcount = NULL;
        ((sp_t(void) *)(ptr_to_sp))->ptr = NULL;
    }
}

void __sp_cleanup_debug(void *ptr_to_sp)
{
    sp_t(void) cleanup_sptr = *((sp_t(void) *)(ptr_to_sp));

    if (cleanup_sptr.refcount == NULL)
    {
        fprintf(stderr, "WARNING : Deleted smart pointer detected during automatic cleanup."
                        " Make sure no <sp_auto_t> pointers are manually deleted.\n");
        return;
    }

    fprintf(stderr,
            "Automatic reference counter decrement of smart pointer %p, "
            "there are now %u refs.\n",
            cleanup_sptr.ptr,
            *cleanup_sptr.refcount - 1);

    if (--(*cleanup_sptr.refcount) == 0)
    {
        fprintf(stderr,
                "Automatic cleanup destroyed smart pointer %p.\n",
                cleanup_sptr.ptr);
        free(cleanup_sptr.ptr);
        free((void *)cleanup_sptr.refcount); // Cast to "void*" to avoid a "discared qualifier" warning
        ((sp_t(void) *)(ptr_to_sp))->refcount = NULL;
        ((sp_t(void) *)(ptr_to_sp))->ptr = NULL;
    }
}