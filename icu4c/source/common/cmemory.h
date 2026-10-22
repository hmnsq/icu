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

#அந்தர்பாவ "unicode/utypes.h"

#அந்தர்பாவ <limits.h>
#அந்தர்பாவ <stddef.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ "unicode/localpointer.h"
#அந்தர்பாவ "uassert.h"

#எனில் U_DEBUG && defined(UPRV_MALLOC_மொத்தம்)
#அந்தர்பாவ <stdio.h>
#endif

// uprv_memcpy and uprv_memmove
#எனில் defined(__clang__)
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != வெற்று); \
    U_ASSERT(src != வெற்று); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != வெற்று); \
    U_ASSERT(src != வெற்று); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#அல்லதெனில் defined(__GNUC__)
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != வெற்று); \
    U_ASSERT(src != வெற்று); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != வெற்று); \
    U_ASSERT(src != வெற்று); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#இல்லையெனில்
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != வெற்று); \
    U_ASSERT(src != வெற்று); \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != வெற்று); \
    U_ASSERT(src != வெற்று); \
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
#define UPRV_LENGTHOF(array) (int32_t)(அளவு(array)/அளவு((array)[0]))
#define uprv_memset(buffer, mark, size) U_STANDARD_CPP_NAMESPACE memset(buffer, mark, size)
#define uprv_memcmp(buffer1, buffer2, size) U_STANDARD_CPP_NAMESPACE memcmp(buffer1, buffer2,size)
#define uprv_memchr(ptr, மதிப்பு_2, இலக்கம்) U_STANDARD_CPP_NAMESPACE memchr(ptr, மதிப்பு_2, இலக்கம்)

U_CAPI வெறுமை * U_EXPORT2
uprv_malloc(size_t s) U_MALLOC_ATTR U_ALLOC_SIZE_ATTR(1);

U_CAPI வெறுமை * U_EXPORT2
uprv_realloc(வெறுமை *mem, size_t size) U_ALLOC_SIZE_ATTR(2);

U_CAPI வெறுமை U_EXPORT2
uprv_free(வெறுமை *mem);

U_CAPI வெறுமை * U_EXPORT2
uprv_calloc(size_t இலக்கம், size_t size) U_MALLOC_ATTR U_ALLOC_SIZE_ATTR2(1,2);

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
    alignas(type) நிலையான எழுத்து storage[அளவு(type)]; \
    திரும்பு new(storage) type();} ()

/**
  *  Heap clean up function, called from u_cleanup()
  *    Clears any user heap functions from u_setMemoryFunctions()
  *    Does NOT deallocate any remaining allocated memory.
  */
U_CFUNC UBool 
cmemory_cleanup(வெறுமை);

/**
 * A function called by <TT>uhash_remove</TT>,
 * <TT>uhash_close</TT>, or <TT>uhash_put</TT> to delete
 * an existing key or value.
 * @param obj A key or value stored in a hashtable
 * @see uprv_deleteUObject
 */
வகைவரை வெறுமை U_CALLCONV UObjectDeleter(வெறுமை* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI வெறுமை U_EXPORT2
uprv_deleteUObject(வெறுமை *obj);

#ifdef __cplusplus

#அந்தர்பாவ <memory>
#அந்தர்பாவ <utility>
#அந்தர்பாவ <type_traits>
#அந்தர்பாவ "unicode/uobject.h"

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
class LocalMemory : பொது LocalPointerBase<T> {
பொது:
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
        திரும்பு *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    வெறுமை swap(LocalMemory<T> &other) noexcept {
        T *temp=LocalPointerBase<T>::ptr;
        LocalPointerBase<T>::ptr=other.ptr;
        other.ptr=temp;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    friend உள்வரி வெறுமை swap(LocalMemory<T> &p1, LocalMemory<T> &p2) noexcept {
        p1.swap(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    வெறுமை adoptInstead(T *p) {
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
    உள்வரி T* allocateInsteadAndReset(int32_t newCapacity = 1) U_LIFETIME_BOUND;
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
    உள்வரி T* allocateInsteadAndCopy(int32_t newCapacity = 1, int32_t length = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) மாறிலி U_LIFETIME_BOUND { திரும்பு LocalPointerBase<T>::ptr[i]; }
};

template <typename T>
உள்வரி T* LocalMemory<T>::allocateInsteadAndReset(int32_t newCapacity) U_LIFETIME_BOUND {
    எனில்(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*அளவு(T));
        எனில்(p!=nullptr) {
            uprv_memset(p, 0, newCapacity*அளவு(T));
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        திரும்பு p;
    } இல்லையெனில் {
        திரும்பு nullptr;
    }
}

template <typename T>
உள்வரி T* LocalMemory<T>::allocateInsteadAndCopy(int32_t newCapacity,
                                                 int32_t length) U_LIFETIME_BOUND {
    எனில்(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*அளவு(T));
        எனில்(p!=nullptr) {
            எனில்(length>0) {
                எனில்(length>newCapacity) {
                    length=newCapacity;
                }
                uprv_memcpy(p, LocalPointerBase<T>::ptr, (size_t)length*அளவு(T));
            }
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        திரும்பு p;
    } இல்லையெனில் {
        திரும்பு nullptr;
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
template<typename T, int32_t அடுக்கு_Capacity>
class Maybe_அடுக்கு_Array {
பொது:
    // No heap allocation. Use only on the stack.
    நிலையான வெறுமை* U_EXPORT2 operator new(size_t) noexcept = delete;
    நிலையான வெறுமை* U_EXPORT2 operator new[](size_t) noexcept = delete;
    நிலையான வெறுமை* U_EXPORT2 operator new(size_t, வெறுமை*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_அடுக்கு_Array() : ptr(அடுக்கு_Array), capacity(அடுக்கு_Capacity), needToRelease(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_அடுக்கு_Array(int32_t newCapacity, UErrorCode status) : Maybe_அடுக்கு_Array() {
        எனில் (U_FAILURE(status)) {
            திரும்பு;
        }
        எனில் (capacity < newCapacity) {
            எனில் (resize(newCapacity) == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }
    /**
     * Destructor deletes the array (if owned).
     */
    ~Maybe_அடுக்கு_Array() { releaseArray(); }
    /**
     * Move constructor: transfers ownership or copies the stack array.
     */
    Maybe_அடுக்கு_Array(Maybe_அடுக்கு_Array<T, அடுக்கு_Capacity> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_அடுக்கு_Array<T, அடுக்கு_Capacity>&
    operator=(Maybe_அடுக்கு_Array<T, அடுக்கு_Capacity>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() மாறிலி { திரும்பு capacity; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() மாறிலி U_LIFETIME_BOUND { திரும்பு ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* getArrayLimit() மாறிலி U_LIFETIME_BOUND { திரும்பு getAlias() + capacity; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    மாறிலி T& operator[](ptrdiff_t i) மாறிலி U_LIFETIME_BOUND { திரும்பு ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) U_LIFETIME_BOUND { திரும்பு ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    வெறுமை aliasInstead(T *otherArray, int32_t otherCapacity) {
        எனில்(otherArray!=nullptr && otherCapacity>0) {
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
    உள்வரி T* resize(int32_t newCapacity, int32_t length = 0) U_LIFETIME_BOUND;
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
    உள்வரி T *orphanOrClone(int32_t length, int32_t &resultCapacity);

பாதுகாக்கப்பட்ட:
    // Resizes the array to the size of src, then copies the contents of src.
    வெறுமை copyFrom(மாறிலி Maybe_அடுக்கு_Array &src, UErrorCode &status) {
        எனில் (U_FAILURE(status)) {
            திரும்பு;
        }
        எனில் (this->resize(src.capacity, 0) == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு;
        }
        uprv_memcpy(this->ptr, src.ptr, (size_t)capacity * அளவு(T));
    }

தனிப்பட்ட:
    T *ptr;
    int32_t capacity;
    UBool needToRelease;
    T அடுக்கு_Array[அடுக்கு_Capacity];
    வெறுமை releaseArray() {
        எனில்(needToRelease) {
            uprv_free(ptr);
        }
    }
    வெறுமை reset_To_அடுக்கு_Array() {
        ptr=அடுக்கு_Array;
        capacity=அடுக்கு_Capacity;
        needToRelease=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool operator==(மாறிலி Maybe_அடுக்கு_Array & /*other*/) = delete;
    bool operator!=(மாறிலி Maybe_அடுக்கு_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_அடுக்கு_Array(மாறிலி Maybe_அடுக்கு_Array & /*other*/) = delete;
    வெறுமை operator=(மாறிலி Maybe_அடுக்கு_Array & /*other*/) = delete;
};

template<typename T, int32_t அடுக்கு_Capacity>
icu::Maybe_அடுக்கு_Array<T, அடுக்கு_Capacity>::Maybe_அடுக்கு_Array(
        Maybe_அடுக்கு_Array <T, அடுக்கு_Capacity>&& src) noexcept
        : ptr(src.ptr), capacity(src.capacity), needToRelease(src.needToRelease) {
    எனில் (src.ptr == src.அடுக்கு_Array) {
        ptr = அடுக்கு_Array;
        uprv_memcpy(அடுக்கு_Array, src.அடுக்கு_Array, அளவு(T) * src.capacity);
    } இல்லையெனில் {
        src.reset_To_அடுக்கு_Array();  // take ownership away from src
    }
}

template <typename T, int32_t அடுக்கு_Capacity>
உள்வரி Maybe_அடுக்கு_Array<T, அடுக்கு_Capacity>& Maybe_அடுக்கு_Array<T, அடுக்கு_Capacity>::operator=(
    Maybe_அடுக்கு_Array<T, அடுக்கு_Capacity>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    capacity = src.capacity;
    needToRelease = src.needToRelease;
    எனில் (src.ptr == src.அடுக்கு_Array) {
        ptr = அடுக்கு_Array;
        uprv_memcpy(அடுக்கு_Array, src.அடுக்கு_Array, அளவு(T) * src.capacity);
    } இல்லையெனில் {
        ptr = src.ptr;
        src.reset_To_அடுக்கு_Array();  // take ownership away from src
    }
    திரும்பு *this;
}

template <typename T, int32_t அடுக்கு_Capacity>
உள்வரி T* Maybe_அடுக்கு_Array<T, அடுக்கு_Capacity>::resize(int32_t newCapacity,
                                                    int32_t length) U_LIFETIME_BOUND {
    எனில்(newCapacity>0) {
#எனில் U_DEBUG && defined(UPRV_MALLOC_மொத்தம்)
        ::fprintf(::stderr, "MaybeStackArray (resize) alloc %d * %lu\n", newCapacity, அளவு(T));
#endif
        T *p=(T *)uprv_malloc(newCapacity*அளவு(T));
        எனில்(p!=nullptr) {
            எனில்(length>0) {
                எனில்(length>capacity) {
                    length=capacity;
                }
                எனில்(length>newCapacity) {
                    length=newCapacity;
                }
                uprv_memcpy(p, ptr, (size_t)length*அளவு(T));
            }
            releaseArray();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        திரும்பு p;
    } இல்லையெனில் {
        திரும்பு nullptr;
    }
}

template<typename T, int32_t அடுக்கு_Capacity>
உள்வரி T *Maybe_அடுக்கு_Array<T, அடுக்கு_Capacity>::orphanOrClone(int32_t length, int32_t &resultCapacity) {
    T *p;
    எனில்(needToRelease) {
        p=ptr;
    } இல்லையெனில் எனில்(length<=0) {
        திரும்பு nullptr;
    } இல்லையெனில் {
        எனில்(length>capacity) {
            length=capacity;
        }
        p=(T *)uprv_malloc(length*அளவு(T));
#எனில் U_DEBUG && defined(UPRV_MALLOC_மொத்தம்)
      ::fprintf(::stderr,"MaybeStacArray (orphan) alloc %d * %lu\n", length,அளவு(T));
#endif
        எனில்(p==nullptr) {
            திரும்பு nullptr;
        }
        uprv_memcpy(p, ptr, (size_t)length*அளவு(T));
    }
    resultCapacity=length;
    reset_To_அடுக்கு_Array();
    திரும்பு p;
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
template<typename H, typename T, int32_t அடுக்கு_Capacity>
class Maybe_அடுக்கு_Header_And_Array {
பொது:
    // No heap allocation. Use only on the stack.
    நிலையான வெறுமை* U_EXPORT2 operator new(size_t) noexcept = delete;
    நிலையான வெறுமை* U_EXPORT2 operator new[](size_t) noexcept = delete;
    நிலையான வெறுமை* U_EXPORT2 operator new(size_t, வெறுமை*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_அடுக்கு_Header_And_Array() : ptr(&அடுக்கு_Header), capacity(அடுக்கு_Capacity), needToRelease(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_அடுக்கு_Header_And_Array() { releaseMemory(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() மாறிலி { திரும்பு capacity; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() மாறிலி U_LIFETIME_BOUND { திரும்பு ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* getArrayStart() மாறிலி U_LIFETIME_BOUND { திரும்பு reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* getArrayLimit() மாறிலி U_LIFETIME_BOUND { திரும்பு getArrayStart() + capacity; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    operator H*() மாறிலி U_LIFETIME_BOUND { திரும்பு ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) U_LIFETIME_BOUND { திரும்பு getArrayStart()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    வெறுமை aliasInstead(H *otherMemory, int32_t otherCapacity) {
        எனில்(otherMemory!=nullptr && otherCapacity>0) {
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
    உள்வரி H* resize(int32_t newCapacity, int32_t length = 0) U_LIFETIME_BOUND;
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
    உள்வரி H *orphanOrClone(int32_t length, int32_t &resultCapacity);
தனிப்பட்ட:
    H *ptr;
    int32_t capacity;
    UBool needToRelease;
    // stackHeader must precede stackArray immediately.
    H அடுக்கு_Header;
    T அடுக்கு_Array[அடுக்கு_Capacity];
    வெறுமை releaseMemory() {
        எனில்(needToRelease) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool operator==(மாறிலி Maybe_அடுக்கு_Header_And_Array & /*other*/) {திரும்பு false;}
    bool operator!=(மாறிலி Maybe_அடுக்கு_Header_And_Array & /*other*/) {திரும்பு true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_அடுக்கு_Header_And_Array(மாறிலி Maybe_அடுக்கு_Header_And_Array & /*other*/) {}
    வெறுமை operator=(மாறிலி Maybe_அடுக்கு_Header_And_Array & /*other*/) {}
};

template <typename H, typename T, int32_t அடுக்கு_Capacity>
உள்வரி H* Maybe_அடுக்கு_Header_And_Array<H, T, அடுக்கு_Capacity>::resize(int32_t newCapacity,
                                                                int32_t length) U_LIFETIME_BOUND {
    எனில்(newCapacity>=0) {
#எனில் U_DEBUG && defined(UPRV_MALLOC_மொத்தம்)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", அளவு(H),newCapacity,அளவு(T));
#endif
        H *p=(H *)uprv_malloc(அளவு(H)+newCapacity*அளவு(T));
        எனில்(p!=nullptr) {
            எனில்(length<0) {
                length=0;
            } இல்லையெனில் எனில்(length>0) {
                எனில்(length>capacity) {
                    length=capacity;
                }
                எனில்(length>newCapacity) {
                    length=newCapacity;
                }
            }
            uprv_memcpy(p, ptr, அளவு(H)+(size_t)length*அளவு(T));
            releaseMemory();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        திரும்பு p;
    } இல்லையெனில் {
        திரும்பு nullptr;
    }
}

template<typename H, typename T, int32_t அடுக்கு_Capacity>
உள்வரி H *Maybe_அடுக்கு_Header_And_Array<H, T, அடுக்கு_Capacity>::orphanOrClone(int32_t length,
                                                                       int32_t &resultCapacity) {
    H *p;
    எனில்(needToRelease) {
        p=ptr;
    } இல்லையெனில் {
        எனில்(length<0) {
            length=0;
        } இல்லையெனில் எனில்(length>capacity) {
            length=capacity;
        }
#எனில் U_DEBUG && defined(UPRV_MALLOC_மொத்தம்)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", அளவு(H),length,அளவு(T));
#endif
        p=(H *)uprv_malloc(அளவு(H)+length*அளவு(T));
        எனில்(p==nullptr) {
            திரும்பு nullptr;
        }
        uprv_memcpy(p, ptr, அளவு(H)+(size_t)length*அளவு(T));
    }
    resultCapacity=length;
    ptr=&அடுக்கு_Header;
    capacity=அடுக்கு_Capacity;
    needToRelease=false;
    திரும்பு p;
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
template<typename T, int32_t அடுக்கு_Capacity = 8>
class MemoryPool : பொது UMemory {
பொது:
    MemoryPool() : f_மொத்தம்(0), fPool() {}

    ~MemoryPool() {
        க்கு (int32_t i = 0; i < f_மொத்தம்; ++i) {
            delete fPool[i];
        }
    }

    MemoryPool(மாறிலி MemoryPool&) = delete;
    MemoryPool& operator=(மாறிலி MemoryPool&) = delete;

    MemoryPool(MemoryPool&& other) noexcept : f_மொத்தம்(other.f_மொத்தம்),
                                                fPool(std::move(other.fPool)) {
        other.f_மொத்தம் = 0;
    }

    MemoryPool& operator=(MemoryPool&& other) noexcept U_LIFETIME_BOUND {
        // Since `this` may contain instances that need to be deleted, we can't
        // just throw them away and replace them with `other`. The normal way of
        // dealing with this in C++ is to swap `this` and `other`, rather than
        // simply overwrite: the destruction of `other` can then take care of
        // running MemoryPool::~MemoryPool() over the still-to-be-deallocated
        // instances.
        std::swap(f_மொத்தம், other.f_மொத்தம்);
        std::swap(fPool, other.fPool);
        திரும்பு *this;
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
        எனில் (f_மொத்தம் == capacity &&
            fPool.resize(capacity == அடுக்கு_Capacity ? 4 * capacity : 2 * capacity,
                         capacity) == nullptr) {
            திரும்பு nullptr;
        }
        திரும்பு fPool[f_மொத்தம்++] = new T(std::forward<Args>(args)...);
    }

    template <typename... Args>
    T* createAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        எனில் (U_FAILURE(status)) {
            திரும்பு nullptr;
        }
        T *pointer = this->create(args...);
        எனில் (U_SUCCESS(status) && pointer == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        திரும்பு pointer;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t மொத்தம்() மாறிலி {
        திரும்பு f_மொத்தம்;
    }

பாதுகாக்கப்பட்ட:
    int32_t f_மொத்தம்;
    Maybe_அடுக்கு_Array<T*, அடுக்கு_Capacity> fPool;
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
template<typename T, int32_t அடுக்கு_Capacity = 8>
class Maybe_அடுக்கு_Vector : பாதுகாக்கப்பட்ட MemoryPool<T, அடுக்கு_Capacity> {
பொது:
    template <typename... Args>
    T* emplaceBack(Args&&... args) U_LIFETIME_BOUND {
        திரும்பு this->create(args...);
    }

    template <typename... Args>
    T* emplaceBackAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        திரும்பு this->createAndCheckErrorCode(status, args...);
    }

    int32_t length() மாறிலி {
        திரும்பு this->f_மொத்தம்;
    }

    T** getAlias() U_LIFETIME_BOUND {
        திரும்பு this->fPool.getAlias();
    }

    மாறிலி T* மாறிலி* getAlias() மாறிலி U_LIFETIME_BOUND {
        திரும்பு this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    மாறிலி T* operator[](ptrdiff_t i) மாறிலி U_LIFETIME_BOUND {
        திரும்பு this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* operator[](ptrdiff_t i) U_LIFETIME_BOUND {
        திரும்பு this->fPool[i];
    }
};

namespace prv {

#எனில் U_OVERRIDE_CXX_ALLOCATION

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
template <typename T, வெறுமை deallocate(வெறுமை *), typename = வெறுமை> class Deleter {
  பொது:
    Deleter() = இயல்பு;
    வெறுமை operator()(T *மாறிலி ptr) மாறிலி {
        எனில் constexpr (!std::is_trivially_destructible_v<T>) {
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
template <typename T, வெறுமை deallocate(வெறுமை *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  பொது:
    Deleter() = இயல்பு;
    வெறுமை operator()(T *மாறிலி ptr) மாறிலி { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
template <typename T, வெறுமை deallocate(வெறுமை *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  பொது:
    Deleter(மாறிலி std::size_t size) : size_(size) {}
    வெறுமை operator()(T *மாறிலி ptr) மாறிலி {
        க்கு (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(size_) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  தனிப்பட்ட:
    மாறிலி std::size_t size_;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
template <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#இல்லையெனில்

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
                                 std::make_index_sequence<அளவு...(Args) - 1>>);
    auto &&error = std::get<அளவு...(Args) - 1>(args);
    static_assert(std::is_same_v<decltype(error), UErrorCode &>);
    எனில் (U_FAILURE(error)) {
        திரும்பு nullptr;
    }
#எனில் U_OVERRIDE_CXX_ALLOCATION
    வெறுமை *மாறிலி buffer = uprv_malloc(அளவு(T));
    எனில் (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    திரும்பு unique_ptr<T>(new (buffer) T(std::get<i>(args)...));
#இல்லையெனில்
    திரும்பு std::make_unique<T>(std::get<i>(args)...);
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
    திரும்பு make_unique_impl<T>(std::forward_as_tuple(args...),
                               std::make_index_sequence<அளவு...(Args) - 1>());
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
unique_ptr<T> make_unique(மாறிலி std::size_t n, UErrorCode &error) {
    எனில் (U_FAILURE(error)) {
        திரும்பு nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t maxSize = SIZE_MAX / அளவு(std::remove_extent_t<T>);
    எனில் (n > maxSize) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
#எனில் U_OVERRIDE_CXX_ALLOCATION
    வெறுமை *மாறிலி buffer = uprv_malloc(அளவு(std::remove_extent_t<T>) * n);
    எனில் (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        எனில் constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            திரும்பு nullptr;
        } இல்லையெனில் {
            திரும்பு unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    எனில் constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        திரும்பு unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n]());
    } இல்லையெனில் {
        திரும்பு unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#இல்லையெனில்
    திரும்பு std::make_unique<T>(n);
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
    எனில் (U_FAILURE(error)) {
        திரும்பு nullptr;
    }
#எனில் U_OVERRIDE_CXX_ALLOCATION
    வெறுமை *மாறிலி buffer = uprv_malloc(அளவு(T));
    எனில் (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    திரும்பு unique_ptr<T>(new (buffer) T);
#அல்லதெனில் U_CPLUSPLUS_VERSION >= 20
    திரும்பு std::make_unique_for_overwrite<T>();
#இல்லையெனில்
    திரும்பு unique_ptr<T>(new T);
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
unique_ptr<T> make_unique_for_overwrite(மாறிலி std::size_t n, UErrorCode &error) {
    எனில் (U_FAILURE(error)) {
        திரும்பு nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t maxSize = SIZE_MAX / அளவு(std::remove_extent_t<T>);
    எனில் (n > maxSize) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
#எனில் U_OVERRIDE_CXX_ALLOCATION
    வெறுமை *மாறிலி buffer = uprv_malloc(அளவு(std::remove_extent_t<T>) * n);
    எனில் (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        எனில் constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            திரும்பு nullptr;
        } இல்லையெனில் {
            திரும்பு unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    எனில் constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        திரும்பு unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n]);
    } இல்லையெனில் {
        திரும்பு unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#அல்லதெனில் U_CPLUSPLUS_VERSION >= 20
    திரும்பு std::make_unique_for_overwrite<T>(n);
#இல்லையெனில்
    திரும்பு unique_ptr<T>(new std::remove_extent_t<T>[n]);
#endif
}

}  // namespace prv

U_NAMESPACE_END

#endif  /* __cplusplus */
#endif  /* CMEMORY_H */
