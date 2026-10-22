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

#شامل "unicode/utypes.h"

#شامل <limits.h>
#شامل <stddef.h>
#شامل <string.h>
#شامل "unicode/localpointer.h"
#شامل "uassert.h"

#اگر U_DEBUG && defined(UPRV_MALLOC_شمار)
#شامل <stdio.h>
#endif

// uprv_memcpy and uprv_memmove
#اگر defined(__clang__)
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != پوچ); \
    U_ASSERT(src != پوچ); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != پوچ); \
    U_ASSERT(src != پوچ); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#وگرنهاگر defined(__GNUC__)
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != پوچ); \
    U_ASSERT(src != پوچ); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != پوچ); \
    U_ASSERT(src != پوچ); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#وگرنه
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != پوچ); \
    U_ASSERT(src != پوچ); \
    U_STANDARD_CPP_NAMESPACE memcpy(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != پوچ); \
    U_ASSERT(src != پوچ); \
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
#define UPRV_LENGTHOF(array) (int32_t)(اندازه(array)/اندازه((array)[0]))
#define uprv_memset(buffer, mark, size) U_STANDARD_CPP_NAMESPACE memset(buffer, mark, size)
#define uprv_memcmp(buffer1, buffer2, size) U_STANDARD_CPP_NAMESPACE memcmp(buffer1, buffer2,size)
#define uprv_memchr(ptr, مقدار_2, رقم) U_STANDARD_CPP_NAMESPACE memchr(ptr, مقدار_2, رقم)

U_CAPI تهی * U_EXPORT2
uprv_malloc(size_t s) U_MALLOC_ATTR U_ALLOC_SIZE_ATTR(1);

U_CAPI تهی * U_EXPORT2
uprv_realloc(تهی *mem, size_t size) U_ALLOC_SIZE_ATTR(2);

U_CAPI تهی U_EXPORT2
uprv_free(تهی *mem);

U_CAPI تهی * U_EXPORT2
uprv_calloc(size_t رقم, size_t size) U_MALLOC_ATTR U_ALLOC_SIZE_ATTR2(1,2);

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
    alignas(type) ایستا نویسه storage[اندازه(type)]; \
    بازگشت new(storage) type();} ()

/**
  *  Heap clean up function, called from u_cleanup()
  *    Clears any user heap functions from u_setMemoryFunctions()
  *    Does NOT deallocate any remaining allocated memory.
  */
U_CFUNC UBool 
cmemory_cleanup(تهی);

/**
 * A function called by <TT>uhash_remove</TT>,
 * <TT>uhash_close</TT>, or <TT>uhash_put</TT> to delete
 * an existing key or value.
 * @param obj A key or value stored in a hashtable
 * @see uprv_deleteUObject
 */
تعریفنوع تهی U_CALLCONV UObjectDeleter(تهی* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI تهی U_EXPORT2
uprv_deleteUObject(تهی *obj);

#ifdef __cplusplus

#شامل <memory>
#شامل <utility>
#شامل <type_traits>
#شامل "unicode/uobject.h"

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
class LocalMemory : عمومی LocalPointerBase<T> {
عمومی:
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
        بازگشت *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    تهی swap(LocalMemory<T> &other) noexcept {
        T *temp=LocalPointerBase<T>::ptr;
        LocalPointerBase<T>::ptr=other.ptr;
        other.ptr=temp;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    friend درونخطی تهی swap(LocalMemory<T> &p1, LocalMemory<T> &p2) noexcept {
        p1.swap(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    تهی adoptInstead(T *p) {
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
    درونخطی T* allocateInsteadAndReset(int32_t newCapacity = 1) U_LIFETIME_BOUND;
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
    درونخطی T* allocateInsteadAndCopy(int32_t newCapacity = 1, int32_t length = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) ثابت U_LIFETIME_BOUND { بازگشت LocalPointerBase<T>::ptr[i]; }
};

template <typename T>
درونخطی T* LocalMemory<T>::allocateInsteadAndReset(int32_t newCapacity) U_LIFETIME_BOUND {
    اگر(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*اندازه(T));
        اگر(p!=nullptr) {
            uprv_memset(p, 0, newCapacity*اندازه(T));
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        بازگشت p;
    } وگرنه {
        بازگشت nullptr;
    }
}

template <typename T>
درونخطی T* LocalMemory<T>::allocateInsteadAndCopy(int32_t newCapacity,
                                                 int32_t length) U_LIFETIME_BOUND {
    اگر(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*اندازه(T));
        اگر(p!=nullptr) {
            اگر(length>0) {
                اگر(length>newCapacity) {
                    length=newCapacity;
                }
                uprv_memcpy(p, LocalPointerBase<T>::ptr, (size_t)length*اندازه(T));
            }
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        بازگشت p;
    } وگرنه {
        بازگشت nullptr;
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
template<typename T, int32_t پشته_Capacity>
class Maybe_پشته_Array {
عمومی:
    // No heap allocation. Use only on the stack.
    ایستا تهی* U_EXPORT2 operator new(size_t) noexcept = delete;
    ایستا تهی* U_EXPORT2 operator new[](size_t) noexcept = delete;
    ایستا تهی* U_EXPORT2 operator new(size_t, تهی*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_پشته_Array() : ptr(پشته_Array), capacity(پشته_Capacity), needToRelease(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_پشته_Array(int32_t newCapacity, UErrorCode status) : Maybe_پشته_Array() {
        اگر (U_FAILURE(status)) {
            بازگشت;
        }
        اگر (capacity < newCapacity) {
            اگر (resize(newCapacity) == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }
    /**
     * Destructor deletes the array (if owned).
     */
    ~Maybe_پشته_Array() { releaseArray(); }
    /**
     * Move constructor: transfers ownership or copies the stack array.
     */
    Maybe_پشته_Array(Maybe_پشته_Array<T, پشته_Capacity> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_پشته_Array<T, پشته_Capacity>&
    operator=(Maybe_پشته_Array<T, پشته_Capacity>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() ثابت { بازگشت capacity; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() ثابت U_LIFETIME_BOUND { بازگشت ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* getArrayLimit() ثابت U_LIFETIME_BOUND { بازگشت getAlias() + capacity; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ثابت T& operator[](ptrdiff_t i) ثابت U_LIFETIME_BOUND { بازگشت ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) U_LIFETIME_BOUND { بازگشت ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    تهی aliasInstead(T *otherArray, int32_t otherCapacity) {
        اگر(otherArray!=nullptr && otherCapacity>0) {
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
    درونخطی T* resize(int32_t newCapacity, int32_t length = 0) U_LIFETIME_BOUND;
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
    درونخطی T *orphanOrClone(int32_t length, int32_t &resultCapacity);

محافظتشده:
    // Resizes the array to the size of src, then copies the contents of src.
    تهی copyFrom(ثابت Maybe_پشته_Array &src, UErrorCode &status) {
        اگر (U_FAILURE(status)) {
            بازگشت;
        }
        اگر (this->resize(src.capacity, 0) == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
        }
        uprv_memcpy(this->ptr, src.ptr, (size_t)capacity * اندازه(T));
    }

خصوصی:
    T *ptr;
    int32_t capacity;
    UBool needToRelease;
    T پشته_Array[پشته_Capacity];
    تهی releaseArray() {
        اگر(needToRelease) {
            uprv_free(ptr);
        }
    }
    تهی reset_To_پشته_Array() {
        ptr=پشته_Array;
        capacity=پشته_Capacity;
        needToRelease=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool operator==(ثابت Maybe_پشته_Array & /*other*/) = delete;
    bool operator!=(ثابت Maybe_پشته_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_پشته_Array(ثابت Maybe_پشته_Array & /*other*/) = delete;
    تهی operator=(ثابت Maybe_پشته_Array & /*other*/) = delete;
};

template<typename T, int32_t پشته_Capacity>
icu::Maybe_پشته_Array<T, پشته_Capacity>::Maybe_پشته_Array(
        Maybe_پشته_Array <T, پشته_Capacity>&& src) noexcept
        : ptr(src.ptr), capacity(src.capacity), needToRelease(src.needToRelease) {
    اگر (src.ptr == src.پشته_Array) {
        ptr = پشته_Array;
        uprv_memcpy(پشته_Array, src.پشته_Array, اندازه(T) * src.capacity);
    } وگرنه {
        src.reset_To_پشته_Array();  // take ownership away from src
    }
}

template <typename T, int32_t پشته_Capacity>
درونخطی Maybe_پشته_Array<T, پشته_Capacity>& Maybe_پشته_Array<T, پشته_Capacity>::operator=(
    Maybe_پشته_Array<T, پشته_Capacity>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    capacity = src.capacity;
    needToRelease = src.needToRelease;
    اگر (src.ptr == src.پشته_Array) {
        ptr = پشته_Array;
        uprv_memcpy(پشته_Array, src.پشته_Array, اندازه(T) * src.capacity);
    } وگرنه {
        ptr = src.ptr;
        src.reset_To_پشته_Array();  // take ownership away from src
    }
    بازگشت *this;
}

template <typename T, int32_t پشته_Capacity>
درونخطی T* Maybe_پشته_Array<T, پشته_Capacity>::resize(int32_t newCapacity,
                                                    int32_t length) U_LIFETIME_BOUND {
    اگر(newCapacity>0) {
#اگر U_DEBUG && defined(UPRV_MALLOC_شمار)
        ::fprintf(::stderr, "MaybeStackArray (resize) alloc %d * %lu\n", newCapacity, اندازه(T));
#endif
        T *p=(T *)uprv_malloc(newCapacity*اندازه(T));
        اگر(p!=nullptr) {
            اگر(length>0) {
                اگر(length>capacity) {
                    length=capacity;
                }
                اگر(length>newCapacity) {
                    length=newCapacity;
                }
                uprv_memcpy(p, ptr, (size_t)length*اندازه(T));
            }
            releaseArray();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        بازگشت p;
    } وگرنه {
        بازگشت nullptr;
    }
}

template<typename T, int32_t پشته_Capacity>
درونخطی T *Maybe_پشته_Array<T, پشته_Capacity>::orphanOrClone(int32_t length, int32_t &resultCapacity) {
    T *p;
    اگر(needToRelease) {
        p=ptr;
    } وگرنه اگر(length<=0) {
        بازگشت nullptr;
    } وگرنه {
        اگر(length>capacity) {
            length=capacity;
        }
        p=(T *)uprv_malloc(length*اندازه(T));
#اگر U_DEBUG && defined(UPRV_MALLOC_شمار)
      ::fprintf(::stderr,"MaybeStacArray (orphan) alloc %d * %lu\n", length,اندازه(T));
#endif
        اگر(p==nullptr) {
            بازگشت nullptr;
        }
        uprv_memcpy(p, ptr, (size_t)length*اندازه(T));
    }
    resultCapacity=length;
    reset_To_پشته_Array();
    بازگشت p;
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
template<typename H, typename T, int32_t پشته_Capacity>
class Maybe_پشته_Header_And_Array {
عمومی:
    // No heap allocation. Use only on the stack.
    ایستا تهی* U_EXPORT2 operator new(size_t) noexcept = delete;
    ایستا تهی* U_EXPORT2 operator new[](size_t) noexcept = delete;
    ایستا تهی* U_EXPORT2 operator new(size_t, تهی*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_پشته_Header_And_Array() : ptr(&پشته_Header), capacity(پشته_Capacity), needToRelease(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_پشته_Header_And_Array() { releaseMemory(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() ثابت { بازگشت capacity; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() ثابت U_LIFETIME_BOUND { بازگشت ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* getArrayStart() ثابت U_LIFETIME_BOUND { بازگشت reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* getArrayLimit() ثابت U_LIFETIME_BOUND { بازگشت getArrayStart() + capacity; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    operator H*() ثابت U_LIFETIME_BOUND { بازگشت ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& operator[](ptrdiff_t i) U_LIFETIME_BOUND { بازگشت getArrayStart()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    تهی aliasInstead(H *otherMemory, int32_t otherCapacity) {
        اگر(otherMemory!=nullptr && otherCapacity>0) {
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
    درونخطی H* resize(int32_t newCapacity, int32_t length = 0) U_LIFETIME_BOUND;
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
    درونخطی H *orphanOrClone(int32_t length, int32_t &resultCapacity);
خصوصی:
    H *ptr;
    int32_t capacity;
    UBool needToRelease;
    // stackHeader must precede stackArray immediately.
    H پشته_Header;
    T پشته_Array[پشته_Capacity];
    تهی releaseMemory() {
        اگر(needToRelease) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool operator==(ثابت Maybe_پشته_Header_And_Array & /*other*/) {بازگشت false;}
    bool operator!=(ثابت Maybe_پشته_Header_And_Array & /*other*/) {بازگشت true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_پشته_Header_And_Array(ثابت Maybe_پشته_Header_And_Array & /*other*/) {}
    تهی operator=(ثابت Maybe_پشته_Header_And_Array & /*other*/) {}
};

template <typename H, typename T, int32_t پشته_Capacity>
درونخطی H* Maybe_پشته_Header_And_Array<H, T, پشته_Capacity>::resize(int32_t newCapacity,
                                                                int32_t length) U_LIFETIME_BOUND {
    اگر(newCapacity>=0) {
#اگر U_DEBUG && defined(UPRV_MALLOC_شمار)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", اندازه(H),newCapacity,اندازه(T));
#endif
        H *p=(H *)uprv_malloc(اندازه(H)+newCapacity*اندازه(T));
        اگر(p!=nullptr) {
            اگر(length<0) {
                length=0;
            } وگرنه اگر(length>0) {
                اگر(length>capacity) {
                    length=capacity;
                }
                اگر(length>newCapacity) {
                    length=newCapacity;
                }
            }
            uprv_memcpy(p, ptr, اندازه(H)+(size_t)length*اندازه(T));
            releaseMemory();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        بازگشت p;
    } وگرنه {
        بازگشت nullptr;
    }
}

template<typename H, typename T, int32_t پشته_Capacity>
درونخطی H *Maybe_پشته_Header_And_Array<H, T, پشته_Capacity>::orphanOrClone(int32_t length,
                                                                       int32_t &resultCapacity) {
    H *p;
    اگر(needToRelease) {
        p=ptr;
    } وگرنه {
        اگر(length<0) {
            length=0;
        } وگرنه اگر(length>capacity) {
            length=capacity;
        }
#اگر U_DEBUG && defined(UPRV_MALLOC_شمار)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", اندازه(H),length,اندازه(T));
#endif
        p=(H *)uprv_malloc(اندازه(H)+length*اندازه(T));
        اگر(p==nullptr) {
            بازگشت nullptr;
        }
        uprv_memcpy(p, ptr, اندازه(H)+(size_t)length*اندازه(T));
    }
    resultCapacity=length;
    ptr=&پشته_Header;
    capacity=پشته_Capacity;
    needToRelease=false;
    بازگشت p;
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
template<typename T, int32_t پشته_Capacity = 8>
class MemoryPool : عمومی UMemory {
عمومی:
    MemoryPool() : f_شمار(0), fPool() {}

    ~MemoryPool() {
        برای (int32_t i = 0; i < f_شمار; ++i) {
            delete fPool[i];
        }
    }

    MemoryPool(ثابت MemoryPool&) = delete;
    MemoryPool& operator=(ثابت MemoryPool&) = delete;

    MemoryPool(MemoryPool&& other) noexcept : f_شمار(other.f_شمار),
                                                fPool(std::move(other.fPool)) {
        other.f_شمار = 0;
    }

    MemoryPool& operator=(MemoryPool&& other) noexcept U_LIFETIME_BOUND {
        // Since `this` may contain instances that need to be deleted, we can't
        // just throw them away and replace them with `other`. The normal way of
        // dealing with this in C++ is to swap `this` and `other`, rather than
        // simply overwrite: the destruction of `other` can then take care of
        // running MemoryPool::~MemoryPool() over the still-to-be-deallocated
        // instances.
        std::swap(f_شمار, other.f_شمار);
        std::swap(fPool, other.fPool);
        بازگشت *this;
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
        اگر (f_شمار == capacity &&
            fPool.resize(capacity == پشته_Capacity ? 4 * capacity : 2 * capacity,
                         capacity) == nullptr) {
            بازگشت nullptr;
        }
        بازگشت fPool[f_شمار++] = new T(std::forward<Args>(args)...);
    }

    template <typename... Args>
    T* createAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        اگر (U_FAILURE(status)) {
            بازگشت nullptr;
        }
        T *pointer = this->create(args...);
        اگر (U_SUCCESS(status) && pointer == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        بازگشت pointer;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t شمار() ثابت {
        بازگشت f_شمار;
    }

محافظتشده:
    int32_t f_شمار;
    Maybe_پشته_Array<T*, پشته_Capacity> fPool;
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
template<typename T, int32_t پشته_Capacity = 8>
class Maybe_پشته_Vector : محافظتشده MemoryPool<T, پشته_Capacity> {
عمومی:
    template <typename... Args>
    T* emplaceBack(Args&&... args) U_LIFETIME_BOUND {
        بازگشت this->create(args...);
    }

    template <typename... Args>
    T* emplaceBackAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        بازگشت this->createAndCheckErrorCode(status, args...);
    }

    int32_t length() ثابت {
        بازگشت this->f_شمار;
    }

    T** getAlias() U_LIFETIME_BOUND {
        بازگشت this->fPool.getAlias();
    }

    ثابت T* ثابت* getAlias() ثابت U_LIFETIME_BOUND {
        بازگشت this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ثابت T* operator[](ptrdiff_t i) ثابت U_LIFETIME_BOUND {
        بازگشت this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* operator[](ptrdiff_t i) U_LIFETIME_BOUND {
        بازگشت this->fPool[i];
    }
};

namespace prv {

#اگر U_OVERRIDE_CXX_ALLOCATION

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
template <typename T, تهی deallocate(تهی *), typename = تهی> class Deleter {
  عمومی:
    Deleter() = پیشفرض;
    تهی operator()(T *ثابت ptr) ثابت {
        اگر constexpr (!std::is_trivially_destructible_v<T>) {
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
template <typename T, تهی deallocate(تهی *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  عمومی:
    Deleter() = پیشفرض;
    تهی operator()(T *ثابت ptr) ثابت { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
template <typename T, تهی deallocate(تهی *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  عمومی:
    Deleter(ثابت std::size_t size) : size_(size) {}
    تهی operator()(T *ثابت ptr) ثابت {
        برای (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(size_) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  خصوصی:
    ثابت std::size_t size_;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
template <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#وگرنه

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
                                 std::make_index_sequence<اندازه...(Args) - 1>>);
    auto &&error = std::get<اندازه...(Args) - 1>(args);
    static_assert(std::is_same_v<decltype(error), UErrorCode &>);
    اگر (U_FAILURE(error)) {
        بازگشت nullptr;
    }
#اگر U_OVERRIDE_CXX_ALLOCATION
    تهی *ثابت buffer = uprv_malloc(اندازه(T));
    اگر (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    بازگشت unique_ptr<T>(new (buffer) T(std::get<i>(args)...));
#وگرنه
    بازگشت std::make_unique<T>(std::get<i>(args)...);
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
    بازگشت make_unique_impl<T>(std::forward_as_tuple(args...),
                               std::make_index_sequence<اندازه...(Args) - 1>());
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
unique_ptr<T> make_unique(ثابت std::size_t n, UErrorCode &error) {
    اگر (U_FAILURE(error)) {
        بازگشت nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t maxSize = SIZE_MAX / اندازه(std::remove_extent_t<T>);
    اگر (n > maxSize) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
#اگر U_OVERRIDE_CXX_ALLOCATION
    تهی *ثابت buffer = uprv_malloc(اندازه(std::remove_extent_t<T>) * n);
    اگر (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        اگر constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            بازگشت nullptr;
        } وگرنه {
            بازگشت unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    اگر constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        بازگشت unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n]());
    } وگرنه {
        بازگشت unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#وگرنه
    بازگشت std::make_unique<T>(n);
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
    اگر (U_FAILURE(error)) {
        بازگشت nullptr;
    }
#اگر U_OVERRIDE_CXX_ALLOCATION
    تهی *ثابت buffer = uprv_malloc(اندازه(T));
    اگر (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    بازگشت unique_ptr<T>(new (buffer) T);
#وگرنهاگر U_CPLUSPLUS_VERSION >= 20
    بازگشت std::make_unique_for_overwrite<T>();
#وگرنه
    بازگشت unique_ptr<T>(new T);
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
unique_ptr<T> make_unique_for_overwrite(ثابت std::size_t n, UErrorCode &error) {
    اگر (U_FAILURE(error)) {
        بازگشت nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t maxSize = SIZE_MAX / اندازه(std::remove_extent_t<T>);
    اگر (n > maxSize) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
#اگر U_OVERRIDE_CXX_ALLOCATION
    تهی *ثابت buffer = uprv_malloc(اندازه(std::remove_extent_t<T>) * n);
    اگر (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        اگر constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            بازگشت nullptr;
        } وگرنه {
            بازگشت unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    اگر constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        بازگشت unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n]);
    } وگرنه {
        بازگشت unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#وگرنهاگر U_CPLUSPLUS_VERSION >= 20
    بازگشت std::make_unique_for_overwrite<T>(n);
#وگرنه
    بازگشت unique_ptr<T>(new std::remove_extent_t<T>[n]);
#endif
}

}  // namespace prv

U_NAMESPACE_END

#endif  /* __cplusplus */
#endif  /* CMEMORY_H */
