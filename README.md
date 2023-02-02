# Smart pointers for C
This library implements simple smart pointers for C using macros.

# Getting started
## Installing the library
Download it from GitHub, build the library and install it
```bash
git clone [git url]
cd spalloc
make
sudo make install
```

## Making a minimal program
```c
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define SPALLOC_DEBUG
#include <spalloc.h>

int main(void)
{
    sp_t(uint32_t) array = spcalloc(uint32_t, 32);
    sp_auto_t(uint32_t) array_cpy = spcopy(array);

    for(int i = 0; i < 32; i++)
        spptr(array)[i] = i;

    spdel(array);

    for(int i = 0; i < 32; i++)
        fprintf(stderr, "%i\n", spptr(array_cpy)[i]);
    
    //array_cpy is automatically deleted when going out of scope
}
```

# Usage
## Allocating a pointer
Memory allocation can be done using `spmalloc(<type>, <number of elements>)` or `spcalloc(<type>, <number of elements>)`. Those function-like macros return a smart pointer of the corresponding type pointing to a block of memory of the requested size.

## Freeing memory
The memory allocated shouldn't be explicitly freed. It's handled by the reference counter mechanism. You can manually delete smart pointers, or use an automatic cleanup when the pointer goes out of scope.

### Manual deletion
A pointer can be deleted using `spdel(<smart pointer>)` once it not needed anymore. `<sp_t(T)>` pointers *have* to be manually deleted, failure to do so leads to memory leaks. `<sp_auto_t(T)>` pointers can be manually deleted but will automatically get deleted when they go out of scope.

### Automatic deletion
Pointers with the type `<sp_auto_t(T)>` are automatically deleted when they go out of scope. Deleting an `<sp_auto_t(T)>` pointer is valid, but a warning message will be logged when debugging is enabled. Depending on the compiler, automatic smart pointer may not be available.

## Accessing the raw pointer
The raw pointer can be accessed via `spptr(<smart pointer>)`, and is returned as a constant pointer.

## Copying a pointer
To copy a smart pointer, use `spcopy(<smart pointer>)`. It increments the reference counter and return a copy of the smart pointer with the same type.

***NOTE :*** *Smart pointers should **never** be copied using the assigment operator. This would duplicate the pointer without increasing its reference counter, leading to unexpected behaviour.*

## Moving a pointer
Using `spmove(<smart pointer>)` will return a copy of the original pointer and delete the original pointer. This can be usefull for returning a non-automatic smart pointer. 

## Resizing a pointer
Resizing a memory block can get complicated when multiple pointers reference the same area. The `spresize(<smart pointer>, <count>)` will resize the memory block pointed by the smart pointer. Resizing a smart pointer with more than one reference will lead to unexpected beahviour, and is logged as an error if debug is enabled.

## Automatic smart pointers
The library uses a feature of GCC that allows objects to be automatically collected when they go out of scope. This feature may not be available in some compiler. If the compiler allows for automatic smart pointers, the macro `SPALLOC_HAS_AUTO` is defined by `spalloc.h`. Automatic cleanup can be forcefully disabled by defining `SPALLOC_NO_AUTO_CLEANUP`.

If a pointer should be automatically deleted, declare it with the type `sp_auto_t(<T>)`. It is guaranteed to be deleted when going out of scope. `sp_auto_t(<T>)` and `sp_t(<T>)` of the same derived type are compatible.

## Smart pointer for custom types
C doesn't support C++ templates, and thus some tricks need to be used to allow for pseudo-templates. Smart pointer for the corresponding type must be declared before being used by using the `SP_DEF(<type>)` macro. The following smart pointer types are already defined : 
`char`, `void`, `bool`, `uint8_t`, `int8_t`, `uint16_t`, `int16_t`, `uint32_t`, `int32_t`, `uint64_t`, `int64_t`, `float`, `double` plus `char_ptr` which is a typedef of `char*` *(see Limitations)*.
## Thread-safe smart pointers
Smart pointers can be made thread-safe by defining `SPALLOC_THREAD_SAFE` befor including the header. 

***NOTE :*** *If thread-safe smart pointers are enabled in one file, they must be enabled in every file who may share a pointer with it.*

## Debug
Defining `SPALLOC_DEBUG` before including `spalloc.h` will enable log messages to `stderr`.

# Limitations

Only types that are a single word can be used because of the way macro handles parameters. A simple workaround is to use a typedef. Example:

```c
typedef int *int_ptr;
SP_DEF(int_ptr);
sp_t(int_ptr) smart_pointer_of_pointer_of_int;
```
