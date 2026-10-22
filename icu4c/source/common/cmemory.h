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

#অন্তর্ভাব "unicode/utypes.h"

#অন্তর্ভাব <limits.h>
#অন্তর্ভাব <stddef.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব "unicode/localpointer.h"
#অন্তর্ভাব "uassert.h"

#যদি U_DEBUG && defined(UPRV_MALLOC_গুনতি)
#অন্তর্ভাব <stdio.h>
#endif

// uprv_memcpy and uprv_memmove
#যদি defined(__clang__)
#define uprv_memcpy(dst, src, আকার_3) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ফাঁকা); \
    U_ASSERT(src != ফাঁকা); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, আকার_3); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, আকার_3) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ফাঁকা); \
    U_ASSERT(src != ফাঁকা); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, আকার_3); \
} UPRV_BLOCK_MACRO_END
#নতুবা_যদি defined(__GNUC__)
#define uprv_memcpy(dst, src, আকার_3) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ফাঁকা); \
    U_ASSERT(src != ফাঁকা); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, আকার_3); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, আকার_3) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ফাঁকা); \
    U_ASSERT(src != ফাঁকা); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, আকার_3); \
} UPRV_BLOCK_MACRO_END
#অন্যথা
#define uprv_memcpy(dst, src, আকার_3) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != ফাঁকা); \
    U_ASSERT(src != ফাঁকা); \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, আকার_3); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, আকার_3) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != ফাঁকা); \
    U_ASSERT(src != ফাঁকা); \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, আকার_3); \
} UPRV_BLOCK_MACRO_END
#endif

/**
 * \def UPRV_LENGTHOF
 * Convenience macro to determine the length of a fixed array at compile-time.
 * @param array A fixed length array
 * @return The length of the array, in elements
 * @internal
 */
#define UPRV_LENGTHOF(array) (int32_t)(আকার(array)/আকার((array)[0]))
#define uprv_memset(বাফার, mark, আকার_3) U_STANDARD_CPP_NAMESPACE memset(বাফার, mark, আকার_3)
#define uprv_memcmp(buffer1, buffer2, আকার_3) U_STANDARD_CPP_NAMESPACE memcmp(buffer1, buffer2,আকার_3)
#define uprv_memchr(ptr, মান_2, অঙ্ক) U_STANDARD_CPP_NAMESPACE memchr(ptr, মান_2, অঙ্ক)

U_CAPI শূন্য * U_EXPORT2
uprv_malloc(size_t s) U_MALLOC_ATTR U_ALLOC_আকার_ATTR(1);

U_CAPI শূন্য * U_EXPORT2
uprv_realloc(শূন্য *mem, size_t আকার_3) U_ALLOC_আকার_ATTR(2);

U_CAPI শূন্য U_EXPORT2
uprv_free(শূন্য *mem);

U_CAPI শূন্য * U_EXPORT2
uprv_calloc(size_t অঙ্ক, size_t আকার_3) U_MALLOC_ATTR U_ALLOC_আকার_ATTR2(1,2);

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
    alignas(type) স্থির অক্ষর storage[আকার(type)]; \
    ফেরত new(storage) type();} ()

/**
  *  Heap clean up function, called from u_cleanup()
  *    Clears any user heap functions from u_setMemoryFunctions()
  *    Does NOT deallocate any remaining allocated memory.
  */
U_CFUNC UBool 
cmemory_cleanup(শূন্য);

/**
 * A function called by <TT>uhash_remove</TT>,
 * <TT>uhash_close</TT>, or <TT>uhash_put</TT> to delete
 * an existing key or value.
 * @param obj A key or value stored in a hashtable
 * @see uprv_deleteUObject
 */
প্রকারলেখ শূন্য U_CALLCONV UObjectDeleter(শূন্য* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI শূন্য U_EXPORT2
uprv_deleteUObject(শূন্য *obj);

#ifdef __cplusplus

#অন্তর্ভাব <memory>
#অন্তর্ভাব <utility>
#অন্তর্ভাব <type_traits>
#অন্তর্ভাব "unicode/uobject.h"

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
class LocalMemory : সর্বজনীন LocalPointerBase<T> {
সর্বজনীন:
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
        ফেরত *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    শূন্য swap(LocalMemory<T> &other) noexcept {
        T *অস্থায়ী=LocalPointerBase<T>::ptr;
        LocalPointerBase<T>::ptr=other.ptr;
        other.ptr=অস্থায়ী;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    friend অন্তঃরেখ শূন্য swap(LocalMemory<T> &p1, LocalMemory<T> &p2) noexcept {
        p1.swap(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    শূন্য adoptInstead(T *p) {
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
    অন্তঃরেখ T* allocateInsteadAndReset(int32_t newCapacity = 1) U_LIFETIME_BOUND;
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
    অন্তঃরেখ T* allocateInsteadAndCopy(int32_t newCapacity = 1, int32_t দৈর্ঘ্য = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) ধ্রুবক U_LIFETIME_BOUND { ফেরত LocalPointerBase<T>::ptr[i]; }
};

template <typename T>
অন্তঃরেখ T* LocalMemory<T>::allocateInsteadAndReset(int32_t newCapacity) U_LIFETIME_BOUND {
    যদি(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*আকার(T));
        যদি(p!=nullptr) {
            uprv_memset(p, 0, newCapacity*আকার(T));
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        ফেরত p;
    } অন্যথা {
        ফেরত nullptr;
    }
}

template <typename T>
অন্তঃরেখ T* LocalMemory<T>::allocateInsteadAndCopy(int32_t newCapacity,
                                                 int32_t দৈর্ঘ্য) U_LIFETIME_BOUND {
    যদি(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*আকার(T));
        যদি(p!=nullptr) {
            যদি(দৈর্ঘ্য>0) {
                যদি(দৈর্ঘ্য>newCapacity) {
                    দৈর্ঘ্য=newCapacity;
                }
                uprv_memcpy(p, LocalPointerBase<T>::ptr, (size_t)দৈর্ঘ্য*আকার(T));
            }
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        ফেরত p;
    } অন্যথা {
        ফেরত nullptr;
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
template<typename T, int32_t স্তূপ_Capacity>
class Maybe_স্তূপ_Array {
সর্বজনীন:
    // No heap allocation. Use only on the stack.
    স্থির শূন্য* U_EXPORT2 operator new(size_t) noexcept = delete;
    স্থির শূন্য* U_EXPORT2 operator new[](size_t) noexcept = delete;
    স্থির শূন্য* U_EXPORT2 operator new(size_t, শূন্য*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_স্তূপ_Array() : ptr(স্তূপ_Array), capacity(স্তূপ_Capacity), needToRelease(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_স্তূপ_Array(int32_t newCapacity, UErrorCode status) : Maybe_স্তূপ_Array() {
        যদি (U_FAILURE(status)) {
            ফেরত;
        }
        যদি (capacity < newCapacity) {
            যদি (resize(newCapacity) == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }
    /**
     * Destructor deletes the array (if owned).
     */
    ~Maybe_স্তূপ_Array() { releaseArray(); }
    /**
     * Move constructor: transfers ownership or copies the stack array.
     */
    Maybe_স্তূপ_Array(Maybe_স্তূপ_Array<T, স্তূপ_Capacity> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_স্তূপ_Array<T, স্তূপ_Capacity>&
    operator=(Maybe_স্তূপ_Array<T, স্তূপ_Capacity>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() ধ্রুবক { ফেরত capacity; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() ধ্রুবক U_LIFETIME_BOUND { ফেরত ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* getArrayLimit() ধ্রুবক U_LIFETIME_BOUND { ফেরত getAlias() + capacity; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ধ্রুবক T& operator[](ptrdiff_t i) ধ্রুবক U_LIFETIME_BOUND { ফেরত ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) U_LIFETIME_BOUND { ফেরত ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    শূন্য aliasInstead(T *otherArray, int32_t otherCapacity) {
        যদি(otherArray!=nullptr && otherCapacity>0) {
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
    অন্তঃরেখ T* resize(int32_t newCapacity, int32_t দৈর্ঘ্য = 0) U_LIFETIME_BOUND;
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
    অন্তঃরেখ T *orphanOrClone(int32_t দৈর্ঘ্য, int32_t &ফলাফল_Capacity);

সুরক্ষিত:
    // Resizes the array to the size of src, then copies the contents of src.
    শূন্য copyFrom(ধ্রুবক Maybe_স্তূপ_Array &src, UErrorCode &status) {
        যদি (U_FAILURE(status)) {
            ফেরত;
        }
        যদি (this->resize(src.capacity, 0) == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
        uprv_memcpy(this->ptr, src.ptr, (size_t)capacity * আকার(T));
    }

ব্যক্তিগত:
    T *ptr;
    int32_t capacity;
    UBool needToRelease;
    T স্তূপ_Array[স্তূপ_Capacity];
    শূন্য releaseArray() {
        যদি(needToRelease) {
            uprv_free(ptr);
        }
    }
    শূন্য reset_To_স্তূপ_Array() {
        ptr=স্তূপ_Array;
        capacity=স্তূপ_Capacity;
        needToRelease=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool operator==(ধ্রুবক Maybe_স্তূপ_Array & /*other*/) = delete;
    bool operator!=(ধ্রুবক Maybe_স্তূপ_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_স্তূপ_Array(ধ্রুবক Maybe_স্তূপ_Array & /*other*/) = delete;
    শূন্য operator=(ধ্রুবক Maybe_স্তূপ_Array & /*other*/) = delete;
};

template<typename T, int32_t স্তূপ_Capacity>
icu::Maybe_স্তূপ_Array<T, স্তূপ_Capacity>::Maybe_স্তূপ_Array(
        Maybe_স্তূপ_Array <T, স্তূপ_Capacity>&& src) noexcept
        : ptr(src.ptr), capacity(src.capacity), needToRelease(src.needToRelease) {
    যদি (src.ptr == src.স্তূপ_Array) {
        ptr = স্তূপ_Array;
        uprv_memcpy(স্তূপ_Array, src.স্তূপ_Array, আকার(T) * src.capacity);
    } অন্যথা {
        src.reset_To_স্তূপ_Array();  // take ownership away from src
    }
}

template <typename T, int32_t স্তূপ_Capacity>
অন্তঃরেখ Maybe_স্তূপ_Array<T, স্তূপ_Capacity>& Maybe_স্তূপ_Array<T, স্তূপ_Capacity>::operator=(
    Maybe_স্তূপ_Array<T, স্তূপ_Capacity>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    capacity = src.capacity;
    needToRelease = src.needToRelease;
    যদি (src.ptr == src.স্তূপ_Array) {
        ptr = স্তূপ_Array;
        uprv_memcpy(স্তূপ_Array, src.স্তূপ_Array, আকার(T) * src.capacity);
    } অন্যথা {
        ptr = src.ptr;
        src.reset_To_স্তূপ_Array();  // take ownership away from src
    }
    ফেরত *this;
}

template <typename T, int32_t স্তূপ_Capacity>
অন্তঃরেখ T* Maybe_স্তূপ_Array<T, স্তূপ_Capacity>::resize(int32_t newCapacity,
                                                    int32_t দৈর্ঘ্য) U_LIFETIME_BOUND {
    যদি(newCapacity>0) {
#যদি U_DEBUG && defined(UPRV_MALLOC_গুনতি)
        ::fprintf(::stderr, "MaybeStackArray (resize) alloc %d * %lu\n", newCapacity, আকার(T));
#endif
        T *p=(T *)uprv_malloc(newCapacity*আকার(T));
        যদি(p!=nullptr) {
            যদি(দৈর্ঘ্য>0) {
                যদি(দৈর্ঘ্য>capacity) {
                    দৈর্ঘ্য=capacity;
                }
                যদি(দৈর্ঘ্য>newCapacity) {
                    দৈর্ঘ্য=newCapacity;
                }
                uprv_memcpy(p, ptr, (size_t)দৈর্ঘ্য*আকার(T));
            }
            releaseArray();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        ফেরত p;
    } অন্যথা {
        ফেরত nullptr;
    }
}

template<typename T, int32_t স্তূপ_Capacity>
অন্তঃরেখ T *Maybe_স্তূপ_Array<T, স্তূপ_Capacity>::orphanOrClone(int32_t দৈর্ঘ্য, int32_t &ফলাফল_Capacity) {
    T *p;
    যদি(needToRelease) {
        p=ptr;
    } অন্যথা যদি(দৈর্ঘ্য<=0) {
        ফেরত nullptr;
    } অন্যথা {
        যদি(দৈর্ঘ্য>capacity) {
            দৈর্ঘ্য=capacity;
        }
        p=(T *)uprv_malloc(দৈর্ঘ্য*আকার(T));
#যদি U_DEBUG && defined(UPRV_MALLOC_গুনতি)
      ::fprintf(::stderr,"MaybeStacArray (orphan) alloc %d * %lu\n", দৈর্ঘ্য,আকার(T));
#endif
        যদি(p==nullptr) {
            ফেরত nullptr;
        }
        uprv_memcpy(p, ptr, (size_t)দৈর্ঘ্য*আকার(T));
    }
    ফলাফল_Capacity=দৈর্ঘ্য;
    reset_To_স্তূপ_Array();
    ফেরত p;
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
template<typename H, typename T, int32_t স্তূপ_Capacity>
class Maybe_স্তূপ_Header_And_Array {
সর্বজনীন:
    // No heap allocation. Use only on the stack.
    স্থির শূন্য* U_EXPORT2 operator new(size_t) noexcept = delete;
    স্থির শূন্য* U_EXPORT2 operator new[](size_t) noexcept = delete;
    স্থির শূন্য* U_EXPORT2 operator new(size_t, শূন্য*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_স্তূপ_Header_And_Array() : ptr(&স্তূপ_Header), capacity(স্তূপ_Capacity), needToRelease(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_স্তূপ_Header_And_Array() { releaseMemory(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() ধ্রুবক { ফেরত capacity; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() ধ্রুবক U_LIFETIME_BOUND { ফেরত ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* getArrayStart() ধ্রুবক U_LIFETIME_BOUND { ফেরত reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* getArrayLimit() ধ্রুবক U_LIFETIME_BOUND { ফেরত getArrayStart() + capacity; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    operator H*() ধ্রুবক U_LIFETIME_BOUND { ফেরত ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) U_LIFETIME_BOUND { ফেরত getArrayStart()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    শূন্য aliasInstead(H *otherMemory, int32_t otherCapacity) {
        যদি(otherMemory!=nullptr && otherCapacity>0) {
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
    অন্তঃরেখ H* resize(int32_t newCapacity, int32_t দৈর্ঘ্য = 0) U_LIFETIME_BOUND;
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
    অন্তঃরেখ H *orphanOrClone(int32_t দৈর্ঘ্য, int32_t &ফলাফল_Capacity);
ব্যক্তিগত:
    H *ptr;
    int32_t capacity;
    UBool needToRelease;
    // stackHeader must precede stackArray immediately.
    H স্তূপ_Header;
    T স্তূপ_Array[স্তূপ_Capacity];
    শূন্য releaseMemory() {
        যদি(needToRelease) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool operator==(ধ্রুবক Maybe_স্তূপ_Header_And_Array & /*other*/) {ফেরত false;}
    bool operator!=(ধ্রুবক Maybe_স্তূপ_Header_And_Array & /*other*/) {ফেরত true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_স্তূপ_Header_And_Array(ধ্রুবক Maybe_স্তূপ_Header_And_Array & /*other*/) {}
    শূন্য operator=(ধ্রুবক Maybe_স্তূপ_Header_And_Array & /*other*/) {}
};

template <typename H, typename T, int32_t স্তূপ_Capacity>
অন্তঃরেখ H* Maybe_স্তূপ_Header_And_Array<H, T, স্তূপ_Capacity>::resize(int32_t newCapacity,
                                                                int32_t দৈর্ঘ্য) U_LIFETIME_BOUND {
    যদি(newCapacity>=0) {
#যদি U_DEBUG && defined(UPRV_MALLOC_গুনতি)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", আকার(H),newCapacity,আকার(T));
#endif
        H *p=(H *)uprv_malloc(আকার(H)+newCapacity*আকার(T));
        যদি(p!=nullptr) {
            যদি(দৈর্ঘ্য<0) {
                দৈর্ঘ্য=0;
            } অন্যথা যদি(দৈর্ঘ্য>0) {
                যদি(দৈর্ঘ্য>capacity) {
                    দৈর্ঘ্য=capacity;
                }
                যদি(দৈর্ঘ্য>newCapacity) {
                    দৈর্ঘ্য=newCapacity;
                }
            }
            uprv_memcpy(p, ptr, আকার(H)+(size_t)দৈর্ঘ্য*আকার(T));
            releaseMemory();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        ফেরত p;
    } অন্যথা {
        ফেরত nullptr;
    }
}

template<typename H, typename T, int32_t স্তূপ_Capacity>
অন্তঃরেখ H *Maybe_স্তূপ_Header_And_Array<H, T, স্তূপ_Capacity>::orphanOrClone(int32_t দৈর্ঘ্য,
                                                                       int32_t &ফলাফল_Capacity) {
    H *p;
    যদি(needToRelease) {
        p=ptr;
    } অন্যথা {
        যদি(দৈর্ঘ্য<0) {
            দৈর্ঘ্য=0;
        } অন্যথা যদি(দৈর্ঘ্য>capacity) {
            দৈর্ঘ্য=capacity;
        }
#যদি U_DEBUG && defined(UPRV_MALLOC_গুনতি)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", আকার(H),দৈর্ঘ্য,আকার(T));
#endif
        p=(H *)uprv_malloc(আকার(H)+দৈর্ঘ্য*আকার(T));
        যদি(p==nullptr) {
            ফেরত nullptr;
        }
        uprv_memcpy(p, ptr, আকার(H)+(size_t)দৈর্ঘ্য*আকার(T));
    }
    ফলাফল_Capacity=দৈর্ঘ্য;
    ptr=&স্তূপ_Header;
    capacity=স্তূপ_Capacity;
    needToRelease=false;
    ফেরত p;
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
template<typename T, int32_t স্তূপ_Capacity = 8>
class MemoryPool : সর্বজনীন UMemory {
সর্বজনীন:
    MemoryPool() : f_গুনতি(0), fPool() {}

    ~MemoryPool() {
        জন্য (int32_t i = 0; i < f_গুনতি; ++i) {
            delete fPool[i];
        }
    }

    MemoryPool(ধ্রুবক MemoryPool&) = delete;
    MemoryPool& operator=(ধ্রুবক MemoryPool&) = delete;

    MemoryPool(MemoryPool&& other) noexcept : f_গুনতি(other.f_গুনতি),
                                                fPool(std::move(other.fPool)) {
        other.f_গুনতি = 0;
    }

    MemoryPool& operator=(MemoryPool&& other) noexcept U_LIFETIME_BOUND {
        // Since `this` may contain instances that need to be deleted, we can't
        // just throw them away and replace them with `other`. The normal way of
        // dealing with this in C++ is to swap `this` and `other`, rather than
        // simply overwrite: the destruction of `other` can then take care of
        // running MemoryPool::~MemoryPool() over the still-to-be-deallocated
        // instances.
        std::swap(f_গুনতি, other.f_গুনতি);
        std::swap(fPool, other.fPool);
        ফেরত *this;
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
        যদি (f_গুনতি == capacity &&
            fPool.resize(capacity == স্তূপ_Capacity ? 4 * capacity : 2 * capacity,
                         capacity) == nullptr) {
            ফেরত nullptr;
        }
        ফেরত fPool[f_গুনতি++] = new T(std::forward<Args>(args)...);
    }

    template <typename... Args>
    T* createAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        যদি (U_FAILURE(status)) {
            ফেরত nullptr;
        }
        T *pointer = this->create(args...);
        যদি (U_SUCCESS(status) && pointer == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ফেরত pointer;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t গুনতি() ধ্রুবক {
        ফেরত f_গুনতি;
    }

সুরক্ষিত:
    int32_t f_গুনতি;
    Maybe_স্তূপ_Array<T*, স্তূপ_Capacity> fPool;
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
template<typename T, int32_t স্তূপ_Capacity = 8>
class Maybe_স্তূপ_Vector : সুরক্ষিত MemoryPool<T, স্তূপ_Capacity> {
সর্বজনীন:
    template <typename... Args>
    T* emplaceBack(Args&&... args) U_LIFETIME_BOUND {
        ফেরত this->create(args...);
    }

    template <typename... Args>
    T* emplaceBackAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        ফেরত this->createAndCheckErrorCode(status, args...);
    }

    int32_t দৈর্ঘ্য() ধ্রুবক {
        ফেরত this->f_গুনতি;
    }

    T** getAlias() U_LIFETIME_BOUND {
        ফেরত this->fPool.getAlias();
    }

    ধ্রুবক T* ধ্রুবক* getAlias() ধ্রুবক U_LIFETIME_BOUND {
        ফেরত this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ধ্রুবক T* operator[](ptrdiff_t i) ধ্রুবক U_LIFETIME_BOUND {
        ফেরত this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* operator[](ptrdiff_t i) U_LIFETIME_BOUND {
        ফেরত this->fPool[i];
    }
};

namespace prv {

#যদি U_OVERRIDE_CXX_ALLOCATION

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
template <typename T, শূন্য deallocate(শূন্য *), typename = শূন্য> class Deleter {
  সর্বজনীন:
    Deleter() = পূর্বনির্ধারিত;
    শূন্য operator()(T *ধ্রুবক ptr) ধ্রুবক {
        যদি constexpr (!std::is_trivially_destructible_v<T>) {
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
template <typename T, শূন্য deallocate(শূন্য *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  সর্বজনীন:
    Deleter() = পূর্বনির্ধারিত;
    শূন্য operator()(T *ধ্রুবক ptr) ধ্রুবক { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
template <typename T, শূন্য deallocate(শূন্য *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  সর্বজনীন:
    Deleter(ধ্রুবক std::size_t আকার_3) : আকার_2(আকার_3) {}
    শূন্য operator()(T *ধ্রুবক ptr) ধ্রুবক {
        জন্য (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(আকার_2) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  ব্যক্তিগত:
    ধ্রুবক std::size_t আকার_2;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
template <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#অন্যথা

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
                               std::সূচক_sequence<i...> constructor_arg_indices) {
    static_assert(std::is_same_v<decltype(constructor_arg_indices),
                                 std::make_সূচক_sequence<আকার...(Args) - 1>>);
    auto &&error = std::get<আকার...(Args) - 1>(args);
    static_assert(std::is_same_v<decltype(error), UErrorCode &>);
    যদি (U_FAILURE(error)) {
        ফেরত nullptr;
    }
#যদি U_OVERRIDE_CXX_ALLOCATION
    শূন্য *ধ্রুবক বাফার = uprv_malloc(আকার(T));
    যদি (বাফার == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    ফেরত unique_ptr<T>(new (বাফার) T(std::get<i>(args)...));
#অন্যথা
    ফেরত std::make_unique<T>(std::get<i>(args)...);
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
    ফেরত make_unique_impl<T>(std::forward_as_tuple(args...),
                               std::make_সূচক_sequence<আকার...(Args) - 1>());
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
unique_ptr<T> make_unique(ধ্রুবক std::size_t n, UErrorCode &error) {
    যদি (U_FAILURE(error)) {
        ফেরত nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_আকার = SIZE_MAX / আকার(std::remove_extent_t<T>);
    যদি (n > max_আকার) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
#যদি U_OVERRIDE_CXX_ALLOCATION
    শূন্য *ধ্রুবক বাফার = uprv_malloc(আকার(std::remove_extent_t<T>) * n);
    যদি (বাফার == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        যদি constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            ফেরত nullptr;
        } অন্যথা {
            ফেরত unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    যদি constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        ফেরত unique_ptr<T>(new (বাফার) std::remove_extent_t<T>[n]());
    } অন্যথা {
        ফেরত unique_ptr<T>(new (বাফার) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#অন্যথা
    ফেরত std::make_unique<T>(n);
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
    যদি (U_FAILURE(error)) {
        ফেরত nullptr;
    }
#যদি U_OVERRIDE_CXX_ALLOCATION
    শূন্য *ধ্রুবক বাফার = uprv_malloc(আকার(T));
    যদি (বাফার == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    ফেরত unique_ptr<T>(new (বাফার) T);
#নতুবা_যদি U_CPLUSPLUS_VERSION >= 20
    ফেরত std::make_unique_for_overwrite<T>();
#অন্যথা
    ফেরত unique_ptr<T>(new T);
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
unique_ptr<T> make_unique_for_overwrite(ধ্রুবক std::size_t n, UErrorCode &error) {
    যদি (U_FAILURE(error)) {
        ফেরত nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_আকার = SIZE_MAX / আকার(std::remove_extent_t<T>);
    যদি (n > max_আকার) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
#যদি U_OVERRIDE_CXX_ALLOCATION
    শূন্য *ধ্রুবক বাফার = uprv_malloc(আকার(std::remove_extent_t<T>) * n);
    যদি (বাফার == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        যদি constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            ফেরত nullptr;
        } অন্যথা {
            ফেরত unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    যদি constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        ফেরত unique_ptr<T>(new (বাফার) std::remove_extent_t<T>[n]);
    } অন্যথা {
        ফেরত unique_ptr<T>(new (বাফার) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#নতুবা_যদি U_CPLUSPLUS_VERSION >= 20
    ফেরত std::make_unique_for_overwrite<T>(n);
#অন্যথা
    ফেরত unique_ptr<T>(new std::remove_extent_t<T>[n]);
#endif
}

}  // namespace prv

U_NAMESPACE_END

#endif  /* __cplusplus */
#endif  /* CMEMORY_H */
