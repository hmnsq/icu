// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1997-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File CMEMORY.H
*
*  Contains stdlib.h/string.h memory functions
*
* @author       Bertrand A. Damiba
*
* Modification History:
*
*   Date        Name        Description
*   6/20/98     Bertrand    Created.
*  05/03/99     stephen     Changed from functions to macros.
*
******************************************************************************
*/

#ifndef CMEMORY_H
#define CMEMORY_H

#അന്തര്ഭാവ "unicode/utypes.h"

#അന്തര്ഭാവ <limits.h>
#അന്തര്ഭാവ <stddef.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ "unicode/localpointer.h"
#അന്തര്ഭാവ "uassert.h"

#എങ്കിൽ U_DEBUG && defined(UPRV_MALLOC_എണ്ണം)
#അന്തര്ഭാവ <stdio.h>
#endif

// uprv_memcpy and uprv_memmove
#എങ്കിൽ defined(__clang__)
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ശൂന്യ); \
    U_ASSERT(src != ശൂന്യ); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ശൂന്യ); \
    U_ASSERT(src != ശൂന്യ); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#അല്ലെങ്കിലെങ്കിൽ defined(__GNUC__)
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ശൂന്യ); \
    U_ASSERT(src != ശൂന്യ); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ശൂന്യ); \
    U_ASSERT(src != ശൂന്യ); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#അല്ലെങ്കിൽ
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != ശൂന്യ); \
    U_ASSERT(src != ശൂന്യ); \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != ശൂന്യ); \
    U_ASSERT(src != ശൂന്യ); \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#endif

/**
 * \def UPRV_LENGTHOF
 * Convenience macro to determine the length of a fixed array at compile-time.
 * @param array A fixed length array
 * @return The length of the array, in elements
 * @internal
 */
#define UPRV_LENGTHOF(array) (int32_t)(വലുപ്പം(array)/വലുപ്പം((array)[0]))
#define uprv_memset(buffer, mark, size) U_STANDARD_CPP_NAMESPACE memset(buffer, mark, size)
#define uprv_memcmp(buffer1, buffer2, size) U_STANDARD_CPP_NAMESPACE memcmp(buffer1, buffer2,size)
#define uprv_memchr(ptr, മൂല്യം_2, അക്കം) U_STANDARD_CPP_NAMESPACE memchr(ptr, മൂല്യം_2, അക്കം)

U_CAPI ശൂന്യം * U_EXPORT2
uprv_malloc(size_t s) U_MALLOC_ATTR U_ALLOC_SIZE_ATTR(1);

U_CAPI ശൂന്യം * U_EXPORT2
uprv_realloc(ശൂന്യം *mem, size_t size) U_ALLOC_SIZE_ATTR(2);

U_CAPI ശൂന്യം U_EXPORT2
uprv_free(ശൂന്യം *mem);

U_CAPI ശൂന്യം * U_EXPORT2
uprv_calloc(size_t അക്കം, size_t size) U_MALLOC_ATTR U_ALLOC_SIZE_ATTR2(1,2);

/**
 * Get the least significant bits of a pointer (a memory address).
 * For example, with a mask of 3, the macro gets the 2 least significant bits,
 * which will be 0 if the pointer is 32-bit (4-byte) aligned.
 *
 * uintptr_t is the most appropriate integer type to cast to.
 */
#define U_POINTER_MASK_LSB(ptr, mask) ((uintptr_t)(ptr) & (mask))

/**
 * Create & return an instance of "type" in statically allocated storage.
 * e.g.
 *    static std::mutex *myMutex = STATIC_NEW(std::mutex);
 * To destroy an object created in this way, invoke the destructor explicitly, e.g.
 *    myMutex->~mutex();
 * DO NOT use delete.
 * DO NOT use with class UMutex, which has specific support for static instances.
 *
 * STATIC_NEW is intended for use when
 *   - We want a static (or global) object.
 *   - We don't want it to ever be destructed, or to explicitly control destruction,
 *     to avoid use-after-destruction problems.
 *   - We want to avoid an ordinary heap allocated object,
 *     to avoid the possibility of memory allocation failures, and
 *     to avoid memory leak reports, from valgrind, for example.
 * This is defined as a macro rather than a template function because each invocation
 * must define distinct static storage for the object being returned.
 */
#define STATIC_NEW(type) [] () { \
    alignas(type) സ്ഥിതം അക്ഷരം storage[വലുപ്പം(type)]; \
    തിരികെ new(storage) type();} ()

/**
  *  Heap clean up function, called from u_cleanup()
  *    Clears any user heap functions from u_setMemoryFunctions()
  *    Does NOT deallocate any remaining allocated memory.
  */
U_CFUNC UBool 
cmemory_cleanup(ശൂന്യം);

/**
 * A function called by <TT>uhash_remove</TT>,
 * <TT>uhash_close</TT>, or <TT>uhash_put</TT> to delete
 * an existing key or value.
 * @param obj A key or value stored in a hashtable
 * @see uprv_deleteUObject
 */
തരനിർവചനം ശൂന്യം U_CALLCONV UObjectDeleter(ശൂന്യം* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI ശൂന്യം U_EXPORT2
uprv_deleteUObject(ശൂന്യം *obj);

#ifdef __cplusplus

#അന്തര്ഭാവ <memory>
#അന്തര്ഭാവ <utility>
#അന്തര്ഭാവ <type_traits>
#അന്തര്ഭാവ "unicode/uobject.h"

U_NAMESPACE_BEGIN

/**
 * "Smart pointer" class, deletes memory via uprv_free().
 * ICU 79+: Use prv::make_unique & prv::make_unique_for_overwrite instead.
 *
 * For most methods see the LocalPointerBase base class.
 * Adds operator[] for array item access.
 *
 * @see LocalPointerBase
 * @see prv::make_unique
 * @deprecated use prv::make_unique & prv::make_unique_for_overwrite
 */
template<typename T>
class LocalMemory : പൊതു LocalPointerBase<T> {
പൊതു:
    using LocalPointerBase<T>::operator*;
    using LocalPointerBase<T>::operator->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an array of T items that is adopted
     */
    explicit LocalMemory(T *p=nullptr) : LocalPointerBase<T>(p) {}
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     */
    LocalMemory(LocalMemory<T> &&src) noexcept : LocalPointerBase<T>(src.ptr) {
        src.ptr=nullptr;
    }
    /**
     * Destructor deletes the memory it owns.
     */
    ~LocalMemory() {
        uprv_free(LocalPointerBase<T>::ptr);
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     */
    LocalMemory<T>& operator=(LocalMemory<T>&& src) noexcept U_LIFETIME_BOUND {
        uprv_free(LocalPointerBase<T>::ptr);
        LocalPointerBase<T>::ptr=src.ptr;
        src.ptr=nullptr;
        തിരികെ *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    ശൂന്യം swap(LocalMemory<T> &other) noexcept {
        T *temp=LocalPointerBase<T>::ptr;
        LocalPointerBase<T>::ptr=other.ptr;
        other.ptr=temp;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    friend വരിയിൽ ശൂന്യം swap(LocalMemory<T> &p1, LocalMemory<T> &p2) noexcept {
        p1.swap(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    ശൂന്യം adoptInstead(T *p) {
        uprv_free(LocalPointerBase<T>::ptr);
        LocalPointerBase<T>::ptr=p;
    }
    /**
     * Deletes the array it owns, allocates a new one and reset its bytes to 0.
     * Returns the new array pointer.
     * If the allocation fails, then the current array is unchanged and
     * this method returns nullptr.
     * @param newCapacity must be >0
     * @return the allocated array pointer, or nullptr if the allocation failed
     */
    വരിയിൽ T* allocateInsteadAndReset(int32_t newCapacity = 1) U_LIFETIME_BOUND;
    /**
     * Deletes the array it owns and allocates a new one, copying length T items.
     * Returns the new array pointer.
     * If the allocation fails, then the current array is unchanged and
     * this method returns nullptr.
     * @param newCapacity must be >0
     * @param length number of T items to be copied from the old array to the new one;
     *               must be no more than the capacity of the old array,
     *               which the caller must track because the LocalMemory does not track it
     * @return the allocated array pointer, or nullptr if the allocation failed
     */
    വരിയിൽ T* allocateInsteadAndCopy(int32_t newCapacity = 1, int32_t length = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) സ്ഥിരം U_LIFETIME_BOUND { തിരികെ LocalPointerBase<T>::ptr[i]; }
};

template <typename T>
വരിയിൽ T* LocalMemory<T>::allocateInsteadAndReset(int32_t newCapacity) U_LIFETIME_BOUND {
    എങ്കിൽ(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*വലുപ്പം(T));
        എങ്കിൽ(p!=nullptr) {
            uprv_memset(p, 0, newCapacity*വലുപ്പം(T));
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        തിരികെ p;
    } അല്ലെങ്കിൽ {
        തിരികെ nullptr;
    }
}

template <typename T>
വരിയിൽ T* LocalMemory<T>::allocateInsteadAndCopy(int32_t newCapacity,
                                                 int32_t length) U_LIFETIME_BOUND {
    എങ്കിൽ(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*വലുപ്പം(T));
        എങ്കിൽ(p!=nullptr) {
            എങ്കിൽ(length>0) {
                എങ്കിൽ(length>newCapacity) {
                    length=newCapacity;
                }
                uprv_memcpy(p, LocalPointerBase<T>::ptr, (size_t)length*വലുപ്പം(T));
            }
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        തിരികെ p;
    } അല്ലെങ്കിൽ {
        തിരികെ nullptr;
    }
}

/**
 * Simple array/buffer management class using uprv_malloc() and uprv_free().
 * Provides an internal array with fixed capacity. Can alias another array
 * or allocate one.
 *
 * The array address is properly aligned for type T. It might not be properly
 * aligned for types larger than T (or larger than the largest subtype of T).
 *
 * Unlike LocalMemory and LocalArray, this class never adopts
 * (takes ownership of) another array.
 *
 * WARNING: MaybeStackArray only works with primitive (plain-old data) types.
 * It does NOT know how to call a destructor! If you work with classes with
 * destructors, consider:
 *
 * - LocalArray in localpointer.h if you know the length ahead of time
 * - MaybeStackVector if you know the length at runtime
 */
template<typename T, int32_t അട്ടി_Capacity>
class Maybe_അട്ടി_Array {
പൊതു:
    // No heap allocation. Use only on the stack.
    സ്ഥിതം ശൂന്യം* U_EXPORT2 operator new(size_t) noexcept = delete;
    സ്ഥിതം ശൂന്യം* U_EXPORT2 operator new[](size_t) noexcept = delete;
    സ്ഥിതം ശൂന്യം* U_EXPORT2 operator new(size_t, ശൂന്യം*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_അട്ടി_Array() : ptr(അട്ടി_Array), capacity(അട്ടി_Capacity), needToRelease(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_അട്ടി_Array(int32_t newCapacity, UErrorCode status) : Maybe_അട്ടി_Array() {
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ;
        }
        എങ്കിൽ (capacity < newCapacity) {
            എങ്കിൽ (resize(newCapacity) == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }
    /**
     * Destructor deletes the array (if owned).
     */
    ~Maybe_അട്ടി_Array() { releaseArray(); }
    /**
     * Move constructor: transfers ownership or copies the stack array.
     */
    Maybe_അട്ടി_Array(Maybe_അട്ടി_Array<T, അട്ടി_Capacity> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_അട്ടി_Array<T, അട്ടി_Capacity>&
    operator=(Maybe_അട്ടി_Array<T, അട്ടി_Capacity>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() സ്ഥിരം { തിരികെ capacity; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* getArrayLimit() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ getAlias() + capacity; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    സ്ഥിരം T& operator[](ptrdiff_t i) സ്ഥിരം U_LIFETIME_BOUND { തിരികെ ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) U_LIFETIME_BOUND { തിരികെ ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    ശൂന്യം aliasInstead(T *otherArray, int32_t otherCapacity) {
        എങ്കിൽ(otherArray!=nullptr && otherCapacity>0) {
            releaseArray();
            ptr=otherArray;
            capacity=otherCapacity;
            needToRelease=false;
        }
    }
    /**
     * Deletes the array (if owned) and allocates a new one, copying length T items.
     * Returns the new array pointer.
     * If the allocation fails, then the current array is unchanged and
     * this method returns nullptr.
     * @param newCapacity can be less than or greater than the current capacity;
     *                    must be >0
     * @param length number of T items to be copied from the old array to the new one
     * @return the allocated array pointer, or nullptr if the allocation failed
     */
    വരിയിൽ T* resize(int32_t newCapacity, int32_t length = 0) U_LIFETIME_BOUND;
    /**
     * Gives up ownership of the array if owned, or else clones it,
     * copying length T items; resets itself to the internal stack array.
     * Returns nullptr if the allocation failed.
     * @param length number of T items to copy when cloning,
     *        and capacity of the clone when cloning
     * @param resultCapacity will be set to the returned array's capacity (output-only)
     * @return the array pointer;
     *         caller becomes responsible for deleting the array
     */
    വരിയിൽ T *orphanOrClone(int32_t length, int32_t &resultCapacity);

സംരക്ഷിത:
    // Resizes the array to the size of src, then copies the contents of src.
    ശൂന്യം copyFrom(സ്ഥിരം Maybe_അട്ടി_Array &src, UErrorCode &status) {
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ;
        }
        എങ്കിൽ (this->resize(src.capacity, 0) == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
        uprv_memcpy(this->ptr, src.ptr, (size_t)capacity * വലുപ്പം(T));
    }

സ്വകാര്യ:
    T *ptr;
    int32_t capacity;
    UBool needToRelease;
    T അട്ടി_Array[അട്ടി_Capacity];
    ശൂന്യം releaseArray() {
        എങ്കിൽ(needToRelease) {
            uprv_free(ptr);
        }
    }
    ശൂന്യം reset_To_അട്ടി_Array() {
        ptr=അട്ടി_Array;
        capacity=അട്ടി_Capacity;
        needToRelease=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool operator==(സ്ഥിരം Maybe_അട്ടി_Array & /*other*/) = delete;
    bool operator!=(സ്ഥിരം Maybe_അട്ടി_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_അട്ടി_Array(സ്ഥിരം Maybe_അട്ടി_Array & /*other*/) = delete;
    ശൂന്യം operator=(സ്ഥിരം Maybe_അട്ടി_Array & /*other*/) = delete;
};

template<typename T, int32_t അട്ടി_Capacity>
icu::Maybe_അട്ടി_Array<T, അട്ടി_Capacity>::Maybe_അട്ടി_Array(
        Maybe_അട്ടി_Array <T, അട്ടി_Capacity>&& src) noexcept
        : ptr(src.ptr), capacity(src.capacity), needToRelease(src.needToRelease) {
    എങ്കിൽ (src.ptr == src.അട്ടി_Array) {
        ptr = അട്ടി_Array;
        uprv_memcpy(അട്ടി_Array, src.അട്ടി_Array, വലുപ്പം(T) * src.capacity);
    } അല്ലെങ്കിൽ {
        src.reset_To_അട്ടി_Array();  // take ownership away from src
    }
}

template <typename T, int32_t അട്ടി_Capacity>
വരിയിൽ Maybe_അട്ടി_Array<T, അട്ടി_Capacity>& Maybe_അട്ടി_Array<T, അട്ടി_Capacity>::operator=(
    Maybe_അട്ടി_Array<T, അട്ടി_Capacity>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    capacity = src.capacity;
    needToRelease = src.needToRelease;
    എങ്കിൽ (src.ptr == src.അട്ടി_Array) {
        ptr = അട്ടി_Array;
        uprv_memcpy(അട്ടി_Array, src.അട്ടി_Array, വലുപ്പം(T) * src.capacity);
    } അല്ലെങ്കിൽ {
        ptr = src.ptr;
        src.reset_To_അട്ടി_Array();  // take ownership away from src
    }
    തിരികെ *this;
}

template <typename T, int32_t അട്ടി_Capacity>
വരിയിൽ T* Maybe_അട്ടി_Array<T, അട്ടി_Capacity>::resize(int32_t newCapacity,
                                                    int32_t length) U_LIFETIME_BOUND {
    എങ്കിൽ(newCapacity>0) {
#എങ്കിൽ U_DEBUG && defined(UPRV_MALLOC_എണ്ണം)
        ::fprintf(::stderr, "MaybeStackArray (resize) alloc %d * %lu\n", newCapacity, വലുപ്പം(T));
#endif
        T *p=(T *)uprv_malloc(newCapacity*വലുപ്പം(T));
        എങ്കിൽ(p!=nullptr) {
            എങ്കിൽ(length>0) {
                എങ്കിൽ(length>capacity) {
                    length=capacity;
                }
                എങ്കിൽ(length>newCapacity) {
                    length=newCapacity;
                }
                uprv_memcpy(p, ptr, (size_t)length*വലുപ്പം(T));
            }
            releaseArray();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        തിരികെ p;
    } അല്ലെങ്കിൽ {
        തിരികെ nullptr;
    }
}

template<typename T, int32_t അട്ടി_Capacity>
വരിയിൽ T *Maybe_അട്ടി_Array<T, അട്ടി_Capacity>::orphanOrClone(int32_t length, int32_t &resultCapacity) {
    T *p;
    എങ്കിൽ(needToRelease) {
        p=ptr;
    } അല്ലെങ്കിൽ എങ്കിൽ(length<=0) {
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(length>capacity) {
            length=capacity;
        }
        p=(T *)uprv_malloc(length*വലുപ്പം(T));
#എങ്കിൽ U_DEBUG && defined(UPRV_MALLOC_എണ്ണം)
      ::fprintf(::stderr,"MaybeStacArray (orphan) alloc %d * %lu\n", length,വലുപ്പം(T));
#endif
        എങ്കിൽ(p==nullptr) {
            തിരികെ nullptr;
        }
        uprv_memcpy(p, ptr, (size_t)length*വലുപ്പം(T));
    }
    resultCapacity=length;
    reset_To_അട്ടി_Array();
    തിരികെ p;
}

/**
 * Variant of MaybeStackArray that allocates a header struct and an array
 * in one contiguous memory block, using uprv_malloc() and uprv_free().
 * Provides internal memory with fixed array capacity. Can alias another memory
 * block or allocate one.
 * The stackCapacity is the number of T items in the internal memory,
 * not counting the H header.
 * Unlike LocalMemory and LocalArray, this class never adopts
 * (takes ownership of) another memory block.
 */
template<typename H, typename T, int32_t അട്ടി_Capacity>
class Maybe_അട്ടി_Header_And_Array {
പൊതു:
    // No heap allocation. Use only on the stack.
    സ്ഥിതം ശൂന്യം* U_EXPORT2 operator new(size_t) noexcept = delete;
    സ്ഥിതം ശൂന്യം* U_EXPORT2 operator new[](size_t) noexcept = delete;
    സ്ഥിതം ശൂന്യം* U_EXPORT2 operator new(size_t, ശൂന്യം*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_അട്ടി_Header_And_Array() : ptr(&അട്ടി_Header), capacity(അട്ടി_Capacity), needToRelease(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_അട്ടി_Header_And_Array() { releaseMemory(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() സ്ഥിരം { തിരികെ capacity; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* getArrayStart() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* getArrayLimit() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ getArrayStart() + capacity; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    operator H*() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) U_LIFETIME_BOUND { തിരികെ getArrayStart()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    ശൂന്യം aliasInstead(H *otherMemory, int32_t otherCapacity) {
        എങ്കിൽ(otherMemory!=nullptr && otherCapacity>0) {
            releaseMemory();
            ptr=otherMemory;
            capacity=otherCapacity;
            needToRelease=false;
        }
    }
    /**
     * Deletes the memory block (if owned) and allocates a new one,
     * copying the header and length T array items.
     * Returns the new header pointer.
     * If the allocation fails, then the current memory is unchanged and
     * this method returns nullptr.
     * @param newCapacity can be less than or greater than the current capacity;
     *                    must be >0
     * @param length number of T items to be copied from the old array to the new one
     * @return the allocated pointer, or nullptr if the allocation failed
     */
    വരിയിൽ H* resize(int32_t newCapacity, int32_t length = 0) U_LIFETIME_BOUND;
    /**
     * Gives up ownership of the memory if owned, or else clones it,
     * copying the header and length T array items; resets itself to the internal memory.
     * Returns nullptr if the allocation failed.
     * @param length number of T items to copy when cloning,
     *        and array capacity of the clone when cloning
     * @param resultCapacity will be set to the returned array's capacity (output-only)
     * @return the header pointer;
     *         caller becomes responsible for deleting the array
     */
    വരിയിൽ H *orphanOrClone(int32_t length, int32_t &resultCapacity);
സ്വകാര്യ:
    H *ptr;
    int32_t capacity;
    UBool needToRelease;
    // stackHeader must precede stackArray immediately.
    H അട്ടി_Header;
    T അട്ടി_Array[അട്ടി_Capacity];
    ശൂന്യം releaseMemory() {
        എങ്കിൽ(needToRelease) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool operator==(സ്ഥിരം Maybe_അട്ടി_Header_And_Array & /*other*/) {തിരികെ false;}
    bool operator!=(സ്ഥിരം Maybe_അട്ടി_Header_And_Array & /*other*/) {തിരികെ true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_അട്ടി_Header_And_Array(സ്ഥിരം Maybe_അട്ടി_Header_And_Array & /*other*/) {}
    ശൂന്യം operator=(സ്ഥിരം Maybe_അട്ടി_Header_And_Array & /*other*/) {}
};

template <typename H, typename T, int32_t അട്ടി_Capacity>
വരിയിൽ H* Maybe_അട്ടി_Header_And_Array<H, T, അട്ടി_Capacity>::resize(int32_t newCapacity,
                                                                int32_t length) U_LIFETIME_BOUND {
    എങ്കിൽ(newCapacity>=0) {
#എങ്കിൽ U_DEBUG && defined(UPRV_MALLOC_എണ്ണം)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", വലുപ്പം(H),newCapacity,വലുപ്പം(T));
#endif
        H *p=(H *)uprv_malloc(വലുപ്പം(H)+newCapacity*വലുപ്പം(T));
        എങ്കിൽ(p!=nullptr) {
            എങ്കിൽ(length<0) {
                length=0;
            } അല്ലെങ്കിൽ എങ്കിൽ(length>0) {
                എങ്കിൽ(length>capacity) {
                    length=capacity;
                }
                എങ്കിൽ(length>newCapacity) {
                    length=newCapacity;
                }
            }
            uprv_memcpy(p, ptr, വലുപ്പം(H)+(size_t)length*വലുപ്പം(T));
            releaseMemory();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        തിരികെ p;
    } അല്ലെങ്കിൽ {
        തിരികെ nullptr;
    }
}

template<typename H, typename T, int32_t അട്ടി_Capacity>
വരിയിൽ H *Maybe_അട്ടി_Header_And_Array<H, T, അട്ടി_Capacity>::orphanOrClone(int32_t length,
                                                                       int32_t &resultCapacity) {
    H *p;
    എങ്കിൽ(needToRelease) {
        p=ptr;
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(length<0) {
            length=0;
        } അല്ലെങ്കിൽ എങ്കിൽ(length>capacity) {
            length=capacity;
        }
#എങ്കിൽ U_DEBUG && defined(UPRV_MALLOC_എണ്ണം)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", വലുപ്പം(H),length,വലുപ്പം(T));
#endif
        p=(H *)uprv_malloc(വലുപ്പം(H)+length*വലുപ്പം(T));
        എങ്കിൽ(p==nullptr) {
            തിരികെ nullptr;
        }
        uprv_memcpy(p, ptr, വലുപ്പം(H)+(size_t)length*വലുപ്പം(T));
    }
    resultCapacity=length;
    ptr=&അട്ടി_Header;
    capacity=അട്ടി_Capacity;
    needToRelease=false;
    തിരികെ p;
}

/**
 * A simple memory management class that creates new heap allocated objects (of
 * any class that has a public constructor), keeps track of them and eventually
 * deletes them all in its own destructor.
 *
 * A typical use-case would be code like this:
 *
 *     MemoryPool<MyType> pool;
 *
 *     MyType* o1 = pool.create();
 *     if (o1 != nullptr) {
 *         foo(o1);
 *     }
 *
 *     MyType* o2 = pool.create(1, 2, 3);
 *     if (o2 != nullptr) {
 *         bar(o2);
 *     }
 *
 *     // MemoryPool will take care of deleting the MyType objects.
 *
 * It doesn't do anything more than that, and is intentionally kept minimalist.
 */
template<typename T, int32_t അട്ടി_Capacity = 8>
class MemoryPool : പൊതു UMemory {
പൊതു:
    MemoryPool() : f_എണ്ണം(0), fPool() {}

    ~MemoryPool() {
        വേണ്ടി (int32_t i = 0; i < f_എണ്ണം; ++i) {
            delete fPool[i];
        }
    }

    MemoryPool(സ്ഥിരം MemoryPool&) = delete;
    MemoryPool& operator=(സ്ഥിരം MemoryPool&) = delete;

    MemoryPool(MemoryPool&& other) noexcept : f_എണ്ണം(other.f_എണ്ണം),
                                                fPool(std::move(other.fPool)) {
        other.f_എണ്ണം = 0;
    }

    MemoryPool& operator=(MemoryPool&& other) noexcept U_LIFETIME_BOUND {
        // Since `this` may contain instances that need to be deleted, we can't
        // just throw them away and replace them with `other`. The normal way of
        // dealing with this in C++ is to swap `this` and `other`, rather than
        // simply overwrite: the destruction of `other` can then take care of
        // running MemoryPool::~MemoryPool() over the still-to-be-deallocated
        // instances.
        std::swap(f_എണ്ണം, other.f_എണ്ണം);
        std::swap(fPool, other.fPool);
        തിരികെ *this;
    }

    /**
     * Creates a new object of typename T, by forwarding any and all arguments
     * to the typename T constructor.
     *
     * @param args Arguments to be forwarded to the typename T constructor.
     * @return A pointer to the newly created object, or nullptr on error.
     */
    template <typename... Args>
    T* create(Args&&... args) U_LIFETIME_BOUND {
        int32_t capacity = fPool.getCapacity();
        എങ്കിൽ (f_എണ്ണം == capacity &&
            fPool.resize(capacity == അട്ടി_Capacity ? 4 * capacity : 2 * capacity,
                         capacity) == nullptr) {
            തിരികെ nullptr;
        }
        തിരികെ fPool[f_എണ്ണം++] = new T(std::forward<Args>(args)...);
    }

    template <typename... Args>
    T* createAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ nullptr;
        }
        T *pointer = this->create(args...);
        എങ്കിൽ (U_SUCCESS(status) && pointer == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        തിരികെ pointer;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t എണ്ണം() സ്ഥിരം {
        തിരികെ f_എണ്ണം;
    }

സംരക്ഷിത:
    int32_t f_എണ്ണം;
    Maybe_അട്ടി_Array<T*, അട്ടി_Capacity> fPool;
};

/**
 * An internal Vector-like implementation based on MemoryPool.
 *
 * Heap-allocates each element and stores pointers.
 *
 * To append an item to the vector, use emplaceBack.
 *
 *     MaybeStackVector<MyType> vector;
 *     MyType* element = vector.emplaceBack();
 *     if (!element) {
 *         status = U_MEMORY_ALLOCATION_ERROR;
 *     }
 *     // do stuff with element
 *
 * To loop over the vector, use a for loop with indices:
 *
 *     for (int32_t i = 0; i < vector.length(); i++) {
 *         MyType* element = vector[i];
 *     }
 */
template<typename T, int32_t അട്ടി_Capacity = 8>
class Maybe_അട്ടി_Vector : സംരക്ഷിത MemoryPool<T, അട്ടി_Capacity> {
പൊതു:
    template <typename... Args>
    T* emplaceBack(Args&&... args) U_LIFETIME_BOUND {
        തിരികെ this->create(args...);
    }

    template <typename... Args>
    T* emplaceBackAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        തിരികെ this->createAndCheckErrorCode(status, args...);
    }

    int32_t length() സ്ഥിരം {
        തിരികെ this->f_എണ്ണം;
    }

    T** getAlias() U_LIFETIME_BOUND {
        തിരികെ this->fPool.getAlias();
    }

    സ്ഥിരം T* സ്ഥിരം* getAlias() സ്ഥിരം U_LIFETIME_BOUND {
        തിരികെ this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    സ്ഥിരം T* operator[](ptrdiff_t i) സ്ഥിരം U_LIFETIME_BOUND {
        തിരികെ this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* operator[](ptrdiff_t i) U_LIFETIME_BOUND {
        തിരികെ this->fPool[i];
    }
};

namespace prv {

#എങ്കിൽ U_OVERRIDE_CXX_ALLOCATION

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
template <typename T, ശൂന്യം deallocate(ശൂന്യം *), typename = ശൂന്യം> class Deleter {
  പൊതു:
    Deleter() = സ്ഥിരസ്ഥിതി;
    ശൂന്യം operator()(T *സ്ഥിരം ptr) സ്ഥിരം {
        എങ്കിൽ constexpr (!std::is_trivially_destructible_v<T>) {
            ptr->~T();
        }
        deallocate(ptr);
    }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of trivially destructible objects, e.g., int32_t[].
 * @internal
 */
template <typename T, ശൂന്യം deallocate(ശൂന്യം *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  പൊതു:
    Deleter() = സ്ഥിരസ്ഥിതി;
    ശൂന്യം operator()(T *സ്ഥിരം ptr) സ്ഥിരം { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
template <typename T, ശൂന്യം deallocate(ശൂന്യം *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  പൊതു:
    Deleter(സ്ഥിരം std::size_t size) : size_(size) {}
    ശൂന്യം operator()(T *സ്ഥിരം ptr) സ്ഥിരം {
        വേണ്ടി (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(size_) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  സ്വകാര്യ:
    സ്ഥിരം std::size_t size_;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
template <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#അല്ലെങ്കിൽ

using std::unique_ptr;

#endif

/**
 * Helper for make_unique<T, Args...>; forwards all but the last of args to the constructor of
 * T, uses the last of args as the error code for allocation errors.
 *
 * As usual, does nothing if the incoming UErrorCode already is U_FAILURE.
 *
 * @internal
 */
template <typename T, typename... Args, std::size_t... i>
unique_ptr<T> make_unique_impl(std::tuple<Args...> args,
                               std::index_sequence<i...> constructor_arg_indices) {
    static_assert(std::is_same_v<decltype(constructor_arg_indices),
                                 std::make_index_sequence<വലുപ്പം...(Args) - 1>>);
    auto &&error = std::get<വലുപ്പം...(Args) - 1>(args);
    static_assert(std::is_same_v<decltype(error), UErrorCode &>);
    എങ്കിൽ (U_FAILURE(error)) {
        തിരികെ nullptr;
    }
#എങ്കിൽ U_OVERRIDE_CXX_ALLOCATION
    ശൂന്യം *സ്ഥിരം buffer = uprv_malloc(വലുപ്പം(T));
    എങ്കിൽ (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    തിരികെ unique_ptr<T>(new (buffer) T(std::get<i>(args)...));
#അല്ലെങ്കിൽ
    തിരികെ std::make_unique<T>(std::get<i>(args)...);
#endif
}

/**
 * A version of std::make_unique<T> that is compatible with ICU allocation error handling.
 * If U_OVERRIDE_CXX_ALLOCATION, allocates using uprv_malloc, and sets a UErrorCode on failure.
 * Otherwise, equivalent to std::make_unique<T>.
 *   prv::make_unique<T>(Args... args, UErrorCode& status)
 * corresponds to
 *   std::make_unique<T>(Args... args)
 * and
 *   new T(Args... args).
 *
 * As usual, does nothing if the incoming UErrorCode already is U_FAILURE.
 *
 * With empty args, this corresponds to new T(), and the allocated object is value-initialized; in
 * particular, scalar types are zero-initialized; cf. make_unique_for_overwrite which
 * corresponds to new T.
 *
 * @internal
 */
template <typename T, typename... Args, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique(Args &&...args) {
    തിരികെ make_unique_impl<T>(std::forward_as_tuple(args...),
                               std::make_index_sequence<വലുപ്പം...(Args) - 1>());
}

/**
 * A version of std::make_unique<T[]> that is compatible with ICU allocation error handling.
 * If U_OVERRIDE_CXX_ALLOCATION, allocates using uprv_malloc, and sets a UErrorCode on failure.
 * Otherwise, equivalent to std::make_unique<T[]>.
 *   prv::make_unique<T[]>(std::size_t n, UErrorCode& status)
 * corresponds to
 *   std::make_unique<T[]>(std::size_t n)
 * and
 *   new T[std::size_t n]().
 *
 * As usual, does nothing if the incoming UErrorCode already is U_FAILURE.
 *
 * The allocated objects are value-initialized; in particular, scalar types are zero-initialized;
 * cf. make_unique_for_overwrite which corresponds to new T[n].
 *
 * @internal
 */
template <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique(സ്ഥിരം std::size_t n, UErrorCode &error) {
    എങ്കിൽ (U_FAILURE(error)) {
        തിരികെ nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t maxSize = SIZE_MAX / വലുപ്പം(std::remove_extent_t<T>);
    എങ്കിൽ (n > maxSize) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
#എങ്കിൽ U_OVERRIDE_CXX_ALLOCATION
    ശൂന്യം *സ്ഥിരം buffer = uprv_malloc(വലുപ്പം(std::remove_extent_t<T>) * n);
    എങ്കിൽ (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        എങ്കിൽ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            തിരികെ nullptr;
        } അല്ലെങ്കിൽ {
            തിരികെ unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    എങ്കിൽ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        തിരികെ unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n]());
    } അല്ലെങ്കിൽ {
        തിരികെ unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#അല്ലെങ്കിൽ
    തിരികെ std::make_unique<T>(n);
#endif
}

/**
 * A version of std::make_unique_for_overwrite<T> that is compatible with ICU allocation error
 * handling.
 * If U_OVERRIDE_CXX_ALLOCATION, allocates using uprv_malloc, and sets a UErrorCode on failure.
 * Otherwise, equivalent to std::make_unique_for_overwrite<T>.
 *   prv::make_unique_for_overwrite<T>(UErrorCode& status)
 * corresponds to
 *   std::make_unique_for_overwrite<T>()
 * and
 *   new T.
 *
 * As usual, does nothing if the incoming UErrorCode already is U_FAILURE.
 *
 * The allocated object is default-initialized.  For some types, e.g., scalar types, this means it
 * has an indeterminate value, or an erroneous value since C++26 (in plain English, the memory is
 * uninitialized).
 *
 * @internal
 */
template <typename T, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique_for_overwrite(UErrorCode &error) {
    എങ്കിൽ (U_FAILURE(error)) {
        തിരികെ nullptr;
    }
#എങ്കിൽ U_OVERRIDE_CXX_ALLOCATION
    ശൂന്യം *സ്ഥിരം buffer = uprv_malloc(വലുപ്പം(T));
    എങ്കിൽ (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    തിരികെ unique_ptr<T>(new (buffer) T);
#അല്ലെങ്കിലെങ്കിൽ U_CPLUSPLUS_VERSION >= 20
    തിരികെ std::make_unique_for_overwrite<T>();
#അല്ലെങ്കിൽ
    തിരികെ unique_ptr<T>(new T);
#endif
}

/**
 * A version of std::make_unique_for_overwrite<T[]> that is compatible with ICU allocation error
 * handling.
 * If U_OVERRIDE_CXX_ALLOCATION, allocates using uprv_malloc, and sets a UErrorCode on failure.
 * Otherwise, equivalent to std::make_unique_for_overwrite<T[]>.
 *   prv::make_unique_for_overwrite<T[]>(std::size_t n, UErrorCode& status)
 * corresponds to
 *   std::make_unique_for_overwrite<T[]>(std::size_t n)
 * and
 *   new T[std::size_t n].
 *
 * As usual, does nothing if the incoming UErrorCode already is U_FAILURE.
 *
 * The allocated objects are default-initialized.  For some types, e.g., scalar types, this means
 * they have indeterminate values, or erroneous values since C++26 (in plain English, the memory is
 * uninitialized).
 *
 * @internal
 */
template <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique_for_overwrite(സ്ഥിരം std::size_t n, UErrorCode &error) {
    എങ്കിൽ (U_FAILURE(error)) {
        തിരികെ nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t maxSize = SIZE_MAX / വലുപ്പം(std::remove_extent_t<T>);
    എങ്കിൽ (n > maxSize) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
#എങ്കിൽ U_OVERRIDE_CXX_ALLOCATION
    ശൂന്യം *സ്ഥിരം buffer = uprv_malloc(വലുപ്പം(std::remove_extent_t<T>) * n);
    എങ്കിൽ (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        എങ്കിൽ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            തിരികെ nullptr;
        } അല്ലെങ്കിൽ {
            തിരികെ unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    എങ്കിൽ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        തിരികെ unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n]);
    } അല്ലെങ്കിൽ {
        തിരികെ unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#അല്ലെങ്കിലെങ്കിൽ U_CPLUSPLUS_VERSION >= 20
    തിരികെ std::make_unique_for_overwrite<T>(n);
#അല്ലെങ്കിൽ
    തിരികെ unique_ptr<T>(new std::remove_extent_t<T>[n]);
#endif
}

}  // namespace prv

U_NAMESPACE_END

#endif  /* __cplusplus */
#endif  /* CMEMORY_H */
