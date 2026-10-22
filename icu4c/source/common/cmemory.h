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

#اگر_غیر_تعریف_شدہ CMEMORY_H
#تعریف_کرو CMEMORY_H

#شامل "unicode/utypes.h"

#شامل <limits.h>
#شامل <stddef.h>
#شامل <string.h>
#شامل "unicode/localpointer.h"
#شامل "uassert.h"

#اگر U_DEBUG && defined(UPRV_MALLOC_گنتی)
#شامل <stdio.h>
#اگر_ختم

// uprv_memcpy and uprv_memmove
#اگر defined(__clang__)
#تعریف_کرو uprv_memcpy(dst, src, سائز_2) UPRV_بلاک_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != تہی); \
    U_ASSERT(src != تہی); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE حافظہ_نقل(dst, src, سائز_2); \
} UPRV_بلاک_MACRO_انتہا
#تعریف_کرو uprv_memmove(dst, src, سائز_2) UPRV_بلاک_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != تہی); \
    U_ASSERT(src != تہی); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, سائز_2); \
} UPRV_بلاک_MACRO_انتہا
#وگرنہ_اگر defined(__GNUC__)
#تعریف_کرو uprv_memcpy(dst, src, سائز_2) UPRV_بلاک_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != تہی); \
    U_ASSERT(src != تہی); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE حافظہ_نقل(dst, src, سائز_2); \
} UPRV_بلاک_MACRO_انتہا
#تعریف_کرو uprv_memmove(dst, src, سائز_2) UPRV_بلاک_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != تہی); \
    U_ASSERT(src != تہی); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, سائز_2); \
} UPRV_بلاک_MACRO_انتہا
#ورنہ
#تعریف_کرو uprv_memcpy(dst, src, سائز_2) UPRV_بلاک_MACRO_BEGIN { \
    U_ASSERT(dst != تہی); \
    U_ASSERT(src != تہی); \
    U_STANDARD_CPP_NAMESPACE حافظہ_نقل(dst, src, سائز_2); \
} UPRV_بلاک_MACRO_انتہا
#تعریف_کرو uprv_memmove(dst, src, سائز_2) UPRV_بلاک_MACRO_BEGIN { \
    U_ASSERT(dst != تہی); \
    U_ASSERT(src != تہی); \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, سائز_2); \
} UPRV_بلاک_MACRO_انتہا
#اگر_ختم

/**
 * \def UPRV_LENGTHOF
 * Convenience macro to determine the length of a fixed array at compile-time.
 * @param array A fixed length array
 * @return The length of the array, in elements
 * @internal
 */
#تعریف_کرو UPRV_LENGTHOF(array) (int32_t)(حجم(array)/حجم((array)[0]))
#تعریف_کرو uprv_memset(بفر, mark, سائز_2) U_STANDARD_CPP_NAMESPACE memset(بفر, mark, سائز_2)
#تعریف_کرو uprv_memcmp(buffer1, buffer2, سائز_2) U_STANDARD_CPP_NAMESPACE memcmp(buffer1, buffer2,سائز_2)
#تعریف_کرو uprv_memchr(ptr, قدر_2, عدد) U_STANDARD_CPP_NAMESPACE memchr(ptr, قدر_2, عدد)

U_CAPI خالی * U_EXPORT2
uprv_malloc(size_t s) U_MALLOC_ATTR U_ALLOC_سائز_ATTR(1);

U_CAPI خالی * U_EXPORT2
uprv_realloc(خالی *mem, size_t سائز_2) U_ALLOC_سائز_ATTR(2);

U_CAPI خالی U_EXPORT2
uprv_free(خالی *mem);

U_CAPI خالی * U_EXPORT2
uprv_calloc(size_t عدد, size_t سائز_2) U_MALLOC_ATTR U_ALLOC_سائز_ATTR2(1,2);

/**
 * Get the least significant bits of a pointer (a memory address).
 * For example, with a mask of 3, the macro gets the 2 least significant bits,
 * which will be 0 if the pointer is 32-bit (4-byte) aligned.
 *
 * uintptr_t is the most appropriate integer type to cast to.
 */
#تعریف_کرو U_اشارہ_گر_نقاب_LSB(ptr, نقاب) ((uintptr_t)(ptr) & (نقاب))

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
#تعریف_کرو STATIC_NEW(type) [] () { \
    alignas(type) ساکن حرف storage[حجم(type)]; \
    واپس new(storage) type();} ()

/**
  *  Heap clean up function, called from u_cleanup()
  *    Clears any user heap functions from u_setMemoryFunctions()
  *    Does NOT deallocate any remaining allocated memory.
  */
U_CFUNC UBool 
cmemory_cleanup(خالی);

/**
 * A function called by <TT>uhash_remove</TT>,
 * <TT>uhash_close</TT>, or <TT>uhash_put</TT> to delete
 * an existing key or value.
 * @param obj A key or value stored in a hashtable
 * @see uprv_deleteUObject
 */
قسمتعریف خالی U_CALLCONV UObjectDeleter(خالی* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI خالی U_EXPORT2
uprv_deleteUObject(خالی *obj);

#اگر_تعریف_شدہ __cplusplus

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
سانچہ<typename T>
class Local_حافظہ : عوامی Local_اشارہ_گر_بنیاد<T> {
عوامی:
    using Local_اشارہ_گر_بنیاد<T>::عامل*;
    using Local_اشارہ_گر_بنیاد<T>::عامل->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an array of T items that is adopted
     */
    explicit Local_حافظہ(T *p=nullptr) : Local_اشارہ_گر_بنیاد<T>(p) {}
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     */
    Local_حافظہ(Local_حافظہ<T> &&src) noexcept : Local_اشارہ_گر_بنیاد<T>(src.ptr) {
        src.ptr=nullptr;
    }
    /**
     * Destructor deletes the memory it owns.
     */
    ~Local_حافظہ() {
        uprv_free(Local_اشارہ_گر_بنیاد<T>::ptr);
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     */
    Local_حافظہ<T>& عامل=(Local_حافظہ<T>&& src) noexcept U_LIFETIME_BOUND {
        uprv_free(Local_اشارہ_گر_بنیاد<T>::ptr);
        Local_اشارہ_گر_بنیاد<T>::ptr=src.ptr;
        src.ptr=nullptr;
        واپس *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    خالی تبادلہ(Local_حافظہ<T> &other) noexcept {
        T *عارضی=Local_اشارہ_گر_بنیاد<T>::ptr;
        Local_اشارہ_گر_بنیاد<T>::ptr=other.ptr;
        other.ptr=عارضی;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    دوست اندرونی خالی تبادلہ(Local_حافظہ<T> &p1, Local_حافظہ<T> &p2) noexcept {
        p1.تبادلہ(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    خالی adoptInstead(T *p) {
        uprv_free(Local_اشارہ_گر_بنیاد<T>::ptr);
        Local_اشارہ_گر_بنیاد<T>::ptr=p;
    }
    /**
     * Deletes the array it owns, allocates a new one and reset its bytes to 0.
     * Returns the new array pointer.
     * If the allocation fails, then the current array is unchanged and
     * this method returns nullptr.
     * @param newCapacity must be >0
     * @return the allocated array pointer, or nullptr if the allocation failed
     */
    اندرونی T* allocateInsteadAndReset(int32_t new_استعداد = 1) U_LIFETIME_BOUND;
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
    اندرونی T* allocateInsteadAndCopy(int32_t new_استعداد = 1, int32_t لمبائی = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& عامل[](ptrdiff_t i) ثابت U_LIFETIME_BOUND { واپس Local_اشارہ_گر_بنیاد<T>::ptr[i]; }
};

سانچہ <typename T>
اندرونی T* Local_حافظہ<T>::allocateInsteadAndReset(int32_t new_استعداد) U_LIFETIME_BOUND {
    اگر(new_استعداد>0) {
        T *p=(T *)uprv_malloc(new_استعداد*حجم(T));
        اگر(p!=nullptr) {
            uprv_memset(p, 0, new_استعداد*حجم(T));
            uprv_free(Local_اشارہ_گر_بنیاد<T>::ptr);
            Local_اشارہ_گر_بنیاد<T>::ptr=p;
        }
        واپس p;
    } ورنہ {
        واپس nullptr;
    }
}

سانچہ <typename T>
اندرونی T* Local_حافظہ<T>::allocateInsteadAndCopy(int32_t new_استعداد,
                                                 int32_t لمبائی) U_LIFETIME_BOUND {
    اگر(new_استعداد>0) {
        T *p=(T *)uprv_malloc(new_استعداد*حجم(T));
        اگر(p!=nullptr) {
            اگر(لمبائی>0) {
                اگر(لمبائی>new_استعداد) {
                    لمبائی=new_استعداد;
                }
                uprv_memcpy(p, Local_اشارہ_گر_بنیاد<T>::ptr, (size_t)لمبائی*حجم(T));
            }
            uprv_free(Local_اشارہ_گر_بنیاد<T>::ptr);
            Local_اشارہ_گر_بنیاد<T>::ptr=p;
        }
        واپس p;
    } ورنہ {
        واپس nullptr;
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
سانچہ<typename T, int32_t ڈھیر_استعداد>
class Maybe_ڈھیر_Array {
عوامی:
    // No heap allocation. Use only on the stack.
    ساکن خالی* U_EXPORT2 عامل new(size_t) noexcept = delete;
    ساکن خالی* U_EXPORT2 عامل new[](size_t) noexcept = delete;
    ساکن خالی* U_EXPORT2 عامل new(size_t, خالی*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_ڈھیر_Array() : ptr(ڈھیر_Array), استعداد(ڈھیر_استعداد), need_مقصد_Release(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_ڈھیر_Array(int32_t new_استعداد, UError_رمز حالت_2) : Maybe_ڈھیر_Array() {
        اگر (U_FAILURE(حالت_2)) {
            واپس;
        }
        اگر (استعداد < new_استعداد) {
            اگر (resize(new_استعداد) == nullptr) {
                حالت_2 = U_حافظہ_تخصیص_ERROR;
            }
        }
    }
    /**
     * Destructor deletes the array (if owned).
     */
    ~Maybe_ڈھیر_Array() { releaseArray(); }
    /**
     * Move constructor: transfers ownership or copies the stack array.
     */
    Maybe_ڈھیر_Array(Maybe_ڈھیر_Array<T, ڈھیر_استعداد> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_ڈھیر_Array<T, ڈھیر_استعداد>&
    عامل=(Maybe_ڈھیر_Array<T, ڈھیر_استعداد>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_استعداد() ثابت { واپس استعداد; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() ثابت U_LIFETIME_BOUND { واپس ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* get_Array_حد_بندی() ثابت U_LIFETIME_BOUND { واپس getAlias() + استعداد; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ثابت T& عامل[](ptrdiff_t i) ثابت U_LIFETIME_BOUND { واپس ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& عامل[](ptrdiff_t i) U_LIFETIME_BOUND { واپس ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    خالی aliasInstead(T *otherArray, int32_t other_استعداد) {
        اگر(otherArray!=nullptr && other_استعداد>0) {
            releaseArray();
            ptr=otherArray;
            استعداد=other_استعداد;
            need_مقصد_Release=false;
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
    اندرونی T* resize(int32_t new_استعداد, int32_t لمبائی = 0) U_LIFETIME_BOUND;
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
    اندرونی T *orphanOrClone(int32_t لمبائی, int32_t &نتیجہ_استعداد);

محفوظ:
    // Resizes the array to the size of src, then copies the contents of src.
    خالی copyFrom(ثابت Maybe_ڈھیر_Array &src, UError_رمز &حالت_2) {
        اگر (U_FAILURE(حالت_2)) {
            واپس;
        }
        اگر (this->resize(src.استعداد, 0) == nullptr) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس;
        }
        uprv_memcpy(this->ptr, src.ptr, (size_t)استعداد * حجم(T));
    }

نجی:
    T *ptr;
    int32_t استعداد;
    UBool need_مقصد_Release;
    T ڈھیر_Array[ڈھیر_استعداد];
    خالی releaseArray() {
        اگر(need_مقصد_Release) {
            uprv_free(ptr);
        }
    }
    خالی reset_مقصد_ڈھیر_Array() {
        ptr=ڈھیر_Array;
        استعداد=ڈھیر_استعداد;
        need_مقصد_Release=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool عامل==(ثابت Maybe_ڈھیر_Array & /*other*/) = delete;
    bool عامل!=(ثابت Maybe_ڈھیر_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_ڈھیر_Array(ثابت Maybe_ڈھیر_Array & /*other*/) = delete;
    خالی عامل=(ثابت Maybe_ڈھیر_Array & /*other*/) = delete;
};

سانچہ<typename T, int32_t ڈھیر_استعداد>
icu::Maybe_ڈھیر_Array<T, ڈھیر_استعداد>::Maybe_ڈھیر_Array(
        Maybe_ڈھیر_Array <T, ڈھیر_استعداد>&& src) noexcept
        : ptr(src.ptr), استعداد(src.استعداد), need_مقصد_Release(src.need_مقصد_Release) {
    اگر (src.ptr == src.ڈھیر_Array) {
        ptr = ڈھیر_Array;
        uprv_memcpy(ڈھیر_Array, src.ڈھیر_Array, حجم(T) * src.استعداد);
    } ورنہ {
        src.reset_مقصد_ڈھیر_Array();  // take ownership away from src
    }
}

سانچہ <typename T, int32_t ڈھیر_استعداد>
اندرونی Maybe_ڈھیر_Array<T, ڈھیر_استعداد>& Maybe_ڈھیر_Array<T, ڈھیر_استعداد>::عامل=(
    Maybe_ڈھیر_Array<T, ڈھیر_استعداد>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    استعداد = src.استعداد;
    need_مقصد_Release = src.need_مقصد_Release;
    اگر (src.ptr == src.ڈھیر_Array) {
        ptr = ڈھیر_Array;
        uprv_memcpy(ڈھیر_Array, src.ڈھیر_Array, حجم(T) * src.استعداد);
    } ورنہ {
        ptr = src.ptr;
        src.reset_مقصد_ڈھیر_Array();  // take ownership away from src
    }
    واپس *this;
}

سانچہ <typename T, int32_t ڈھیر_استعداد>
اندرونی T* Maybe_ڈھیر_Array<T, ڈھیر_استعداد>::resize(int32_t new_استعداد,
                                                    int32_t لمبائی) U_LIFETIME_BOUND {
    اگر(new_استعداد>0) {
#اگر U_DEBUG && defined(UPRV_MALLOC_گنتی)
        ::fprintf(::stderr, "MaybeStackArray (resize) alloc %d * %lu\n", new_استعداد, حجم(T));
#اگر_ختم
        T *p=(T *)uprv_malloc(new_استعداد*حجم(T));
        اگر(p!=nullptr) {
            اگر(لمبائی>0) {
                اگر(لمبائی>استعداد) {
                    لمبائی=استعداد;
                }
                اگر(لمبائی>new_استعداد) {
                    لمبائی=new_استعداد;
                }
                uprv_memcpy(p, ptr, (size_t)لمبائی*حجم(T));
            }
            releaseArray();
            ptr=p;
            استعداد=new_استعداد;
            need_مقصد_Release=true;
        }
        واپس p;
    } ورنہ {
        واپس nullptr;
    }
}

سانچہ<typename T, int32_t ڈھیر_استعداد>
اندرونی T *Maybe_ڈھیر_Array<T, ڈھیر_استعداد>::orphanOrClone(int32_t لمبائی, int32_t &نتیجہ_استعداد) {
    T *p;
    اگر(need_مقصد_Release) {
        p=ptr;
    } ورنہ اگر(لمبائی<=0) {
        واپس nullptr;
    } ورنہ {
        اگر(لمبائی>استعداد) {
            لمبائی=استعداد;
        }
        p=(T *)uprv_malloc(لمبائی*حجم(T));
#اگر U_DEBUG && defined(UPRV_MALLOC_گنتی)
      ::fprintf(::stderr,"MaybeStacArray (orphan) alloc %d * %lu\n", لمبائی,حجم(T));
#اگر_ختم
        اگر(p==nullptr) {
            واپس nullptr;
        }
        uprv_memcpy(p, ptr, (size_t)لمبائی*حجم(T));
    }
    نتیجہ_استعداد=لمبائی;
    reset_مقصد_ڈھیر_Array();
    واپس p;
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
سانچہ<typename H, typename T, int32_t ڈھیر_استعداد>
class Maybe_ڈھیر_سرخی_And_Array {
عوامی:
    // No heap allocation. Use only on the stack.
    ساکن خالی* U_EXPORT2 عامل new(size_t) noexcept = delete;
    ساکن خالی* U_EXPORT2 عامل new[](size_t) noexcept = delete;
    ساکن خالی* U_EXPORT2 عامل new(size_t, خالی*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_ڈھیر_سرخی_And_Array() : ptr(&ڈھیر_سرخی), استعداد(ڈھیر_استعداد), need_مقصد_Release(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_ڈھیر_سرخی_And_Array() { release_حافظہ(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_استعداد() ثابت { واپس استعداد; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() ثابت U_LIFETIME_BOUND { واپس ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* get_Array_آغاز() ثابت U_LIFETIME_BOUND { واپس reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* get_Array_حد_بندی() ثابت U_LIFETIME_BOUND { واپس get_Array_آغاز() + استعداد; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    عامل H*() ثابت U_LIFETIME_BOUND { واپس ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& عامل[](ptrdiff_t i) U_LIFETIME_BOUND { واپس get_Array_آغاز()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    خالی aliasInstead(H *other_حافظہ, int32_t other_استعداد) {
        اگر(other_حافظہ!=nullptr && other_استعداد>0) {
            release_حافظہ();
            ptr=other_حافظہ;
            استعداد=other_استعداد;
            need_مقصد_Release=false;
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
    اندرونی H* resize(int32_t new_استعداد, int32_t لمبائی = 0) U_LIFETIME_BOUND;
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
    اندرونی H *orphanOrClone(int32_t لمبائی, int32_t &نتیجہ_استعداد);
نجی:
    H *ptr;
    int32_t استعداد;
    UBool need_مقصد_Release;
    // stackHeader must precede stackArray immediately.
    H ڈھیر_سرخی;
    T ڈھیر_Array[ڈھیر_استعداد];
    خالی release_حافظہ() {
        اگر(need_مقصد_Release) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool عامل==(ثابت Maybe_ڈھیر_سرخی_And_Array & /*other*/) {واپس false;}
    bool عامل!=(ثابت Maybe_ڈھیر_سرخی_And_Array & /*other*/) {واپس true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_ڈھیر_سرخی_And_Array(ثابت Maybe_ڈھیر_سرخی_And_Array & /*other*/) {}
    خالی عامل=(ثابت Maybe_ڈھیر_سرخی_And_Array & /*other*/) {}
};

سانچہ <typename H, typename T, int32_t ڈھیر_استعداد>
اندرونی H* Maybe_ڈھیر_سرخی_And_Array<H, T, ڈھیر_استعداد>::resize(int32_t new_استعداد,
                                                                int32_t لمبائی) U_LIFETIME_BOUND {
    اگر(new_استعداد>=0) {
#اگر U_DEBUG && defined(UPRV_MALLOC_گنتی)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", حجم(H),new_استعداد,حجم(T));
#اگر_ختم
        H *p=(H *)uprv_malloc(حجم(H)+new_استعداد*حجم(T));
        اگر(p!=nullptr) {
            اگر(لمبائی<0) {
                لمبائی=0;
            } ورنہ اگر(لمبائی>0) {
                اگر(لمبائی>استعداد) {
                    لمبائی=استعداد;
                }
                اگر(لمبائی>new_استعداد) {
                    لمبائی=new_استعداد;
                }
            }
            uprv_memcpy(p, ptr, حجم(H)+(size_t)لمبائی*حجم(T));
            release_حافظہ();
            ptr=p;
            استعداد=new_استعداد;
            need_مقصد_Release=true;
        }
        واپس p;
    } ورنہ {
        واپس nullptr;
    }
}

سانچہ<typename H, typename T, int32_t ڈھیر_استعداد>
اندرونی H *Maybe_ڈھیر_سرخی_And_Array<H, T, ڈھیر_استعداد>::orphanOrClone(int32_t لمبائی,
                                                                       int32_t &نتیجہ_استعداد) {
    H *p;
    اگر(need_مقصد_Release) {
        p=ptr;
    } ورنہ {
        اگر(لمبائی<0) {
            لمبائی=0;
        } ورنہ اگر(لمبائی>استعداد) {
            لمبائی=استعداد;
        }
#اگر U_DEBUG && defined(UPRV_MALLOC_گنتی)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", حجم(H),لمبائی,حجم(T));
#اگر_ختم
        p=(H *)uprv_malloc(حجم(H)+لمبائی*حجم(T));
        اگر(p==nullptr) {
            واپس nullptr;
        }
        uprv_memcpy(p, ptr, حجم(H)+(size_t)لمبائی*حجم(T));
    }
    نتیجہ_استعداد=لمبائی;
    ptr=&ڈھیر_سرخی;
    استعداد=ڈھیر_استعداد;
    need_مقصد_Release=false;
    واپس p;
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
سانچہ<typename T, int32_t ڈھیر_استعداد = 8>
class حافظہ_Pool : عوامی UMemory {
عوامی:
    حافظہ_Pool() : f_گنتی(0), fPool() {}

    ~حافظہ_Pool() {
        برائے (int32_t i = 0; i < f_گنتی; ++i) {
            delete fPool[i];
        }
    }

    حافظہ_Pool(ثابت حافظہ_Pool&) = delete;
    حافظہ_Pool& عامل=(ثابت حافظہ_Pool&) = delete;

    حافظہ_Pool(حافظہ_Pool&& other) noexcept : f_گنتی(other.f_گنتی),
                                                fPool(std::move(other.fPool)) {
        other.f_گنتی = 0;
    }

    حافظہ_Pool& عامل=(حافظہ_Pool&& other) noexcept U_LIFETIME_BOUND {
        // Since `this` may contain instances that need to be deleted, we can't
        // just throw them away and replace them with `other`. The normal way of
        // dealing with this in C++ is to swap `this` and `other`, rather than
        // simply overwrite: the destruction of `other` can then take care of
        // running MemoryPool::~MemoryPool() over the still-to-be-deallocated
        // instances.
        std::تبادلہ(f_گنتی, other.f_گنتی);
        std::تبادلہ(fPool, other.fPool);
        واپس *this;
    }

    /**
     * Creates a new object of typename T, by forwarding any and all arguments
     * to the typename T constructor.
     *
     * @param args Arguments to be forwarded to the typename T constructor.
     * @return A pointer to the newly created object, or nullptr on error.
     */
    سانچہ <typename... دلائل>
    T* create(دلائل&&... دلائل_2) U_LIFETIME_BOUND {
        int32_t استعداد = fPool.get_استعداد();
        اگر (f_گنتی == استعداد &&
            fPool.resize(استعداد == ڈھیر_استعداد ? 4 * استعداد : 2 * استعداد,
                         استعداد) == nullptr) {
            واپس nullptr;
        }
        واپس fPool[f_گنتی++] = new T(std::forward<دلائل>(دلائل_2)...);
    }

    سانچہ <typename... دلائل>
    T* create_And_Check_Error_رمز(UError_رمز& حالت_2, دلائل&&... دلائل_2) U_LIFETIME_BOUND {
        اگر (U_FAILURE(حالت_2)) {
            واپس nullptr;
        }
        T *اشارہ_گر = this->create(دلائل_2...);
        اگر (U_SUCCESS(حالت_2) && اشارہ_گر == nullptr) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
        }
        واپس اشارہ_گر;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t گنتی() ثابت {
        واپس f_گنتی;
    }

محفوظ:
    int32_t f_گنتی;
    Maybe_ڈھیر_Array<T*, ڈھیر_استعداد> fPool;
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
سانچہ<typename T, int32_t ڈھیر_استعداد = 8>
class Maybe_ڈھیر_سمتیہ : محفوظ حافظہ_Pool<T, ڈھیر_استعداد> {
عوامی:
    سانچہ <typename... دلائل>
    T* emplace_عقب(دلائل&&... دلائل_2) U_LIFETIME_BOUND {
        واپس this->create(دلائل_2...);
    }

    سانچہ <typename... دلائل>
    T* emplace_عقب_And_Check_Error_رمز(UError_رمز& حالت_2, دلائل&&... دلائل_2) U_LIFETIME_BOUND {
        واپس this->create_And_Check_Error_رمز(حالت_2, دلائل_2...);
    }

    int32_t لمبائی() ثابت {
        واپس this->f_گنتی;
    }

    T** getAlias() U_LIFETIME_BOUND {
        واپس this->fPool.getAlias();
    }

    ثابت T* ثابت* getAlias() ثابت U_LIFETIME_BOUND {
        واپس this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ثابت T* عامل[](ptrdiff_t i) ثابت U_LIFETIME_BOUND {
        واپس this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* عامل[](ptrdiff_t i) U_LIFETIME_BOUND {
        واپس this->fPool[i];
    }
};

namespace prv {

#اگر U_OVERRIDE_CXX_تخصیص

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
سانچہ <typename T, خالی deallocate(خالی *), typename = خالی> class Deleter {
  عوامی:
    Deleter() = طےشدہ;
    خالی عامل()(T *ثابت ptr) ثابت {
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
سانچہ <typename T, خالی deallocate(خالی *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  عوامی:
    Deleter() = طےشدہ;
    خالی عامل()(T *ثابت ptr) ثابت { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
سانچہ <typename T, خالی deallocate(خالی *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  عوامی:
    Deleter(ثابت std::size_t سائز_2) : سائز(سائز_2) {}
    خالی عامل()(T *ثابت ptr) ثابت {
        برائے (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(سائز) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  نجی:
    ثابت std::size_t سائز;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
سانچہ <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#ورنہ

using std::unique_ptr;

#اگر_ختم

/**
 * Helper for make_unique<T, Args...>; forwards all but the last of args to the constructor of
 * T, uses the last of args as the error code for allocation errors.
 *
 * As usual, does nothing if the incoming UErrorCode already is U_FAILURE.
 *
 * @internal
 */
سانچہ <typename T, typename... دلائل, std::size_t... i>
unique_ptr<T> make_unique_impl(std::tuple<دلائل...> دلائل_2,
                               std::اشاریہ_sequence<i...> constructor_arg_indices) {
    static_assert(std::is_same_v<decltype(constructor_arg_indices),
                                 std::make_اشاریہ_sequence<حجم...(دلائل) - 1>>);
    خودکار &&error = std::get<حجم...(دلائل) - 1>(دلائل_2);
    static_assert(std::is_same_v<decltype(error), UError_رمز &>);
    اگر (U_FAILURE(error)) {
        واپس nullptr;
    }
#اگر U_OVERRIDE_CXX_تخصیص
    خالی *ثابت بفر = uprv_malloc(حجم(T));
    اگر (بفر == nullptr) {
        error = U_حافظہ_تخصیص_ERROR;
        واپس nullptr;
    }
    واپس unique_ptr<T>(new (بفر) T(std::get<i>(دلائل_2)...));
#ورنہ
    واپس std::make_unique<T>(std::get<i>(دلائل_2)...);
#اگر_ختم
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
سانچہ <typename T, typename... دلائل, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique(دلائل &&...دلائل_2) {
    واپس make_unique_impl<T>(std::forward_as_tuple(دلائل_2...),
                               std::make_اشاریہ_sequence<حجم...(دلائل) - 1>());
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
سانچہ <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique(ثابت std::size_t n, UError_رمز &error) {
    اگر (U_FAILURE(error)) {
        واپس nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_سائز = SIZE_MAX / حجم(std::remove_extent_t<T>);
    اگر (n > max_سائز) {
        error = U_ILLEGAL_دلیل_ERROR;
        واپس nullptr;
    }
#اگر U_OVERRIDE_CXX_تخصیص
    خالی *ثابت بفر = uprv_malloc(حجم(std::remove_extent_t<T>) * n);
    اگر (بفر == nullptr) {
        error = U_حافظہ_تخصیص_ERROR;
        اگر constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            واپس nullptr;
        } ورنہ {
            واپس unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    اگر constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        واپس unique_ptr<T>(new (بفر) std::remove_extent_t<T>[n]());
    } ورنہ {
        واپس unique_ptr<T>(new (بفر) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#ورنہ
    واپس std::make_unique<T>(n);
#اگر_ختم
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
سانچہ <typename T, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique_for_overwrite(UError_رمز &error) {
    اگر (U_FAILURE(error)) {
        واپس nullptr;
    }
#اگر U_OVERRIDE_CXX_تخصیص
    خالی *ثابت بفر = uprv_malloc(حجم(T));
    اگر (بفر == nullptr) {
        error = U_حافظہ_تخصیص_ERROR;
        واپس nullptr;
    }
    واپس unique_ptr<T>(new (بفر) T);
#وگرنہ_اگر U_CPLUSPLUS_نسخہ >= 20
    واپس std::make_unique_for_overwrite<T>();
#ورنہ
    واپس unique_ptr<T>(new T);
#اگر_ختم
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
سانچہ <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique_for_overwrite(ثابت std::size_t n, UError_رمز &error) {
    اگر (U_FAILURE(error)) {
        واپس nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_سائز = SIZE_MAX / حجم(std::remove_extent_t<T>);
    اگر (n > max_سائز) {
        error = U_ILLEGAL_دلیل_ERROR;
        واپس nullptr;
    }
#اگر U_OVERRIDE_CXX_تخصیص
    خالی *ثابت بفر = uprv_malloc(حجم(std::remove_extent_t<T>) * n);
    اگر (بفر == nullptr) {
        error = U_حافظہ_تخصیص_ERROR;
        اگر constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            واپس nullptr;
        } ورنہ {
            واپس unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    اگر constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        واپس unique_ptr<T>(new (بفر) std::remove_extent_t<T>[n]);
    } ورنہ {
        واپس unique_ptr<T>(new (بفر) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#وگرنہ_اگر U_CPLUSPLUS_نسخہ >= 20
    واپس std::make_unique_for_overwrite<T>(n);
#ورنہ
    واپس unique_ptr<T>(new std::remove_extent_t<T>[n]);
#اگر_ختم
}

}  // namespace prv

U_NAMESPACE_انتہا

#اگر_ختم  /* __cplusplus */
#اگر_ختم  /* CMEMORY_H */
