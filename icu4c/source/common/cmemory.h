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

#جے_بنا_تعریف CMEMORY_H
#تعریف_کرو CMEMORY_H

#ضمن "unicode/utypes.h"

#ضمن <limits.h>
#ضمن <stddef.h>
#ضمن <string.h>
#ضمن "unicode/localpointer.h"
#ضمن "uassert.h"

#إذا U_DEBUG && defined(UPRV_MALLOC_گنتی)
#ضمن <stdio.h>
#جے_انت

// uprv_memcpy and uprv_memmove
#إذا defined(__clang__)
#تعریف_کرو uprv_memcpy(dst, src, ناپ_2) UPRV_بلاک_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != عدم); \
    U_ASSERT(src != عدم); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE حافظہ_نقل(dst, src, ناپ_2); \
} UPRV_بلاک_MACRO_سرا
#تعریف_کرو uprv_memmove(dst, src, ناپ_2) UPRV_بلاک_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != عدم); \
    U_ASSERT(src != عدم); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, ناپ_2); \
} UPRV_بلاک_MACRO_سرا
#وإلاإذا defined(__GNUC__)
#تعریف_کرو uprv_memcpy(dst, src, ناپ_2) UPRV_بلاک_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != عدم); \
    U_ASSERT(src != عدم); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE حافظہ_نقل(dst, src, ناپ_2); \
} UPRV_بلاک_MACRO_سرا
#تعریف_کرو uprv_memmove(dst, src, ناپ_2) UPRV_بلاک_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != عدم); \
    U_ASSERT(src != عدم); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, ناپ_2); \
} UPRV_بلاک_MACRO_سرا
#وإلا
#تعریف_کرو uprv_memcpy(dst, src, ناپ_2) UPRV_بلاک_MACRO_BEGIN { \
    U_ASSERT(dst != عدم); \
    U_ASSERT(src != عدم); \
    U_STANDARD_CPP_NAMESPACE حافظہ_نقل(dst, src, ناپ_2); \
} UPRV_بلاک_MACRO_سرا
#تعریف_کرو uprv_memmove(dst, src, ناپ_2) UPRV_بلاک_MACRO_BEGIN { \
    U_ASSERT(dst != عدم); \
    U_ASSERT(src != عدم); \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, ناپ_2); \
} UPRV_بلاک_MACRO_سرا
#جے_انت

/**
 * \def UPRV_LENGTHOF
 * Convenience macro to determine the length of a fixed array at compile-time.
 * @param array A fixed length array
 * @return The length of the array, in elements
 * @internal
 */
#تعریف_کرو UPRV_LENGTHOF(array) (int32_t)(حجم(array)/حجم((array)[0]))
#تعریف_کرو uprv_memset(بفر, mark, ناپ_2) U_STANDARD_CPP_NAMESPACE memset(بفر, mark, ناپ_2)
#تعریف_کرو uprv_memcmp(buffer1, buffer2, ناپ_2) U_STANDARD_CPP_NAMESPACE memcmp(buffer1, buffer2,ناپ_2)
#تعریف_کرو uprv_memchr(ptr, قدر_2, عدد) U_STANDARD_CPP_NAMESPACE memchr(ptr, قدر_2, عدد)

U_CAPI فراغ * U_EXPORT2
uprv_malloc(size_t s) U_MALLOC_ATTR U_ALLOC_ناپ_ATTR(1);

U_CAPI فراغ * U_EXPORT2
uprv_realloc(فراغ *mem, size_t ناپ_2) U_ALLOC_ناپ_ATTR(2);

U_CAPI فراغ U_EXPORT2
uprv_free(فراغ *mem);

U_CAPI فراغ * U_EXPORT2
uprv_calloc(size_t عدد, size_t ناپ_2) U_MALLOC_ATTR U_ALLOC_ناپ_ATTR2(1,2);

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
    alignas(type) ساكن حرف storage[حجم(type)]; \
    أرجع new(storage) type();} ()

/**
  *  Heap clean up function, called from u_cleanup()
  *    Clears any user heap functions from u_setMemoryFunctions()
  *    Does NOT deallocate any remaining allocated memory.
  */
U_CFUNC UBool 
cmemory_cleanup(فراغ);

/**
 * A function called by <TT>uhash_remove</TT>,
 * <TT>uhash_close</TT>, or <TT>uhash_put</TT> to delete
 * an existing key or value.
 * @param obj A key or value stored in a hashtable
 * @see uprv_deleteUObject
 */
تعريفنوع فراغ U_CALLCONV UObjectDeleter(فراغ* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI فراغ U_EXPORT2
uprv_deleteUObject(فراغ *obj);

#جے_تعریف_شدہ __cplusplus

#ضمن <memory>
#ضمن <utility>
#ضمن <type_traits>
#ضمن "unicode/uobject.h"

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
سانچا<typename T>
class Local_حافظہ : عام Local_اشارہ_گر_ادھار<T> {
عام:
    using Local_اشارہ_گر_ادھار<T>::عامل*;
    using Local_اشارہ_گر_ادھار<T>::عامل->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an array of T items that is adopted
     */
    explicit Local_حافظہ(T *p=nullptr) : Local_اشارہ_گر_ادھار<T>(p) {}
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     */
    Local_حافظہ(Local_حافظہ<T> &&src) noexcept : Local_اشارہ_گر_ادھار<T>(src.ptr) {
        src.ptr=nullptr;
    }
    /**
     * Destructor deletes the memory it owns.
     */
    ~Local_حافظہ() {
        uprv_free(Local_اشارہ_گر_ادھار<T>::ptr);
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     */
    Local_حافظہ<T>& عامل=(Local_حافظہ<T>&& src) noexcept U_LIFETIME_BOUND {
        uprv_free(Local_اشارہ_گر_ادھار<T>::ptr);
        Local_اشارہ_گر_ادھار<T>::ptr=src.ptr;
        src.ptr=nullptr;
        أرجع *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    فراغ وٹاندرا(Local_حافظہ<T> &other) noexcept {
        T *عارضی=Local_اشارہ_گر_ادھار<T>::ptr;
        Local_اشارہ_گر_ادھار<T>::ptr=other.ptr;
        other.ptr=عارضی;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    یار مضمن فراغ وٹاندرا(Local_حافظہ<T> &p1, Local_حافظہ<T> &p2) noexcept {
        p1.وٹاندرا(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    فراغ adoptInstead(T *p) {
        uprv_free(Local_اشارہ_گر_ادھار<T>::ptr);
        Local_اشارہ_گر_ادھار<T>::ptr=p;
    }
    /**
     * Deletes the array it owns, allocates a new one and reset its bytes to 0.
     * Returns the new array pointer.
     * If the allocation fails, then the current array is unchanged and
     * this method returns nullptr.
     * @param newCapacity must be >0
     * @return the allocated array pointer, or nullptr if the allocation failed
     */
    مضمن T* allocateInsteadAndReset(int32_t new_سمرتھا = 1) U_LIFETIME_BOUND;
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
    مضمن T* allocateInsteadAndCopy(int32_t new_سمرتھا = 1, int32_t لمائی = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& عامل[](ptrdiff_t i) ثابت U_LIFETIME_BOUND { أرجع Local_اشارہ_گر_ادھار<T>::ptr[i]; }
};

سانچا <typename T>
مضمن T* Local_حافظہ<T>::allocateInsteadAndReset(int32_t new_سمرتھا) U_LIFETIME_BOUND {
    إذا(new_سمرتھا>0) {
        T *p=(T *)uprv_malloc(new_سمرتھا*حجم(T));
        إذا(p!=nullptr) {
            uprv_memset(p, 0, new_سمرتھا*حجم(T));
            uprv_free(Local_اشارہ_گر_ادھار<T>::ptr);
            Local_اشارہ_گر_ادھار<T>::ptr=p;
        }
        أرجع p;
    } وإلا {
        أرجع nullptr;
    }
}

سانچا <typename T>
مضمن T* Local_حافظہ<T>::allocateInsteadAndCopy(int32_t new_سمرتھا,
                                                 int32_t لمائی) U_LIFETIME_BOUND {
    إذا(new_سمرتھا>0) {
        T *p=(T *)uprv_malloc(new_سمرتھا*حجم(T));
        إذا(p!=nullptr) {
            إذا(لمائی>0) {
                إذا(لمائی>new_سمرتھا) {
                    لمائی=new_سمرتھا;
                }
                uprv_memcpy(p, Local_اشارہ_گر_ادھار<T>::ptr, (size_t)لمائی*حجم(T));
            }
            uprv_free(Local_اشارہ_گر_ادھار<T>::ptr);
            Local_اشارہ_گر_ادھار<T>::ptr=p;
        }
        أرجع p;
    } وإلا {
        أرجع nullptr;
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
سانچا<typename T, int32_t ڈھیر_سمرتھا>
class Maybe_ڈھیر_Array {
عام:
    // No heap allocation. Use only on the stack.
    ساكن فراغ* U_EXPORT2 عامل new(size_t) noexcept = delete;
    ساكن فراغ* U_EXPORT2 عامل new[](size_t) noexcept = delete;
    ساكن فراغ* U_EXPORT2 عامل new(size_t, فراغ*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_ڈھیر_Array() : ptr(ڈھیر_Array), سمرتھا(ڈھیر_سمرتھا), need_منزل_Release(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_ڈھیر_Array(int32_t new_سمرتھا, UError_کوڈ حالت) : Maybe_ڈھیر_Array() {
        إذا (U_FAILURE(حالت)) {
            أرجع;
        }
        إذا (سمرتھا < new_سمرتھا) {
            إذا (resize(new_سمرتھا) == nullptr) {
                حالت = U_حافظہ_ونڈ_ERROR;
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
    Maybe_ڈھیر_Array(Maybe_ڈھیر_Array<T, ڈھیر_سمرتھا> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_ڈھیر_Array<T, ڈھیر_سمرتھا>&
    عامل=(Maybe_ڈھیر_Array<T, ڈھیر_سمرتھا>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_سمرتھا() ثابت { أرجع سمرتھا; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() ثابت U_LIFETIME_BOUND { أرجع ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* get_Array_حد_بندی() ثابت U_LIFETIME_BOUND { أرجع getAlias() + سمرتھا; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ثابت T& عامل[](ptrdiff_t i) ثابت U_LIFETIME_BOUND { أرجع ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& عامل[](ptrdiff_t i) U_LIFETIME_BOUND { أرجع ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    فراغ aliasInstead(T *otherArray, int32_t other_سمرتھا) {
        إذا(otherArray!=nullptr && other_سمرتھا>0) {
            releaseArray();
            ptr=otherArray;
            سمرتھا=other_سمرتھا;
            need_منزل_Release=false;
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
    مضمن T* resize(int32_t new_سمرتھا, int32_t لمائی = 0) U_LIFETIME_BOUND;
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
    مضمن T *orphanOrClone(int32_t لمائی, int32_t &نتیجہ_سمرتھا);

محمي:
    // Resizes the array to the size of src, then copies the contents of src.
    فراغ copyFrom(ثابت Maybe_ڈھیر_Array &src, UError_کوڈ &حالت) {
        إذا (U_FAILURE(حالت)) {
            أرجع;
        }
        إذا (this->resize(src.سمرتھا, 0) == nullptr) {
            حالت = U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
        uprv_memcpy(this->ptr, src.ptr, (size_t)سمرتھا * حجم(T));
    }

خاص:
    T *ptr;
    int32_t سمرتھا;
    UBool need_منزل_Release;
    T ڈھیر_Array[ڈھیر_سمرتھا];
    فراغ releaseArray() {
        إذا(need_منزل_Release) {
            uprv_free(ptr);
        }
    }
    فراغ reset_منزل_ڈھیر_Array() {
        ptr=ڈھیر_Array;
        سمرتھا=ڈھیر_سمرتھا;
        need_منزل_Release=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool عامل==(ثابت Maybe_ڈھیر_Array & /*other*/) = delete;
    bool عامل!=(ثابت Maybe_ڈھیر_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_ڈھیر_Array(ثابت Maybe_ڈھیر_Array & /*other*/) = delete;
    فراغ عامل=(ثابت Maybe_ڈھیر_Array & /*other*/) = delete;
};

سانچا<typename T, int32_t ڈھیر_سمرتھا>
icu::Maybe_ڈھیر_Array<T, ڈھیر_سمرتھا>::Maybe_ڈھیر_Array(
        Maybe_ڈھیر_Array <T, ڈھیر_سمرتھا>&& src) noexcept
        : ptr(src.ptr), سمرتھا(src.سمرتھا), need_منزل_Release(src.need_منزل_Release) {
    إذا (src.ptr == src.ڈھیر_Array) {
        ptr = ڈھیر_Array;
        uprv_memcpy(ڈھیر_Array, src.ڈھیر_Array, حجم(T) * src.سمرتھا);
    } وإلا {
        src.reset_منزل_ڈھیر_Array();  // take ownership away from src
    }
}

سانچا <typename T, int32_t ڈھیر_سمرتھا>
مضمن Maybe_ڈھیر_Array<T, ڈھیر_سمرتھا>& Maybe_ڈھیر_Array<T, ڈھیر_سمرتھا>::عامل=(
    Maybe_ڈھیر_Array<T, ڈھیر_سمرتھا>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    سمرتھا = src.سمرتھا;
    need_منزل_Release = src.need_منزل_Release;
    إذا (src.ptr == src.ڈھیر_Array) {
        ptr = ڈھیر_Array;
        uprv_memcpy(ڈھیر_Array, src.ڈھیر_Array, حجم(T) * src.سمرتھا);
    } وإلا {
        ptr = src.ptr;
        src.reset_منزل_ڈھیر_Array();  // take ownership away from src
    }
    أرجع *this;
}

سانچا <typename T, int32_t ڈھیر_سمرتھا>
مضمن T* Maybe_ڈھیر_Array<T, ڈھیر_سمرتھا>::resize(int32_t new_سمرتھا,
                                                    int32_t لمائی) U_LIFETIME_BOUND {
    إذا(new_سمرتھا>0) {
#إذا U_DEBUG && defined(UPRV_MALLOC_گنتی)
        ::fprintf(::stderr, "MaybeStackArray (resize) alloc %d * %lu\n", new_سمرتھا, حجم(T));
#جے_انت
        T *p=(T *)uprv_malloc(new_سمرتھا*حجم(T));
        إذا(p!=nullptr) {
            إذا(لمائی>0) {
                إذا(لمائی>سمرتھا) {
                    لمائی=سمرتھا;
                }
                إذا(لمائی>new_سمرتھا) {
                    لمائی=new_سمرتھا;
                }
                uprv_memcpy(p, ptr, (size_t)لمائی*حجم(T));
            }
            releaseArray();
            ptr=p;
            سمرتھا=new_سمرتھا;
            need_منزل_Release=true;
        }
        أرجع p;
    } وإلا {
        أرجع nullptr;
    }
}

سانچا<typename T, int32_t ڈھیر_سمرتھا>
مضمن T *Maybe_ڈھیر_Array<T, ڈھیر_سمرتھا>::orphanOrClone(int32_t لمائی, int32_t &نتیجہ_سمرتھا) {
    T *p;
    إذا(need_منزل_Release) {
        p=ptr;
    } وإلا إذا(لمائی<=0) {
        أرجع nullptr;
    } وإلا {
        إذا(لمائی>سمرتھا) {
            لمائی=سمرتھا;
        }
        p=(T *)uprv_malloc(لمائی*حجم(T));
#إذا U_DEBUG && defined(UPRV_MALLOC_گنتی)
      ::fprintf(::stderr,"MaybeStacArray (orphan) alloc %d * %lu\n", لمائی,حجم(T));
#جے_انت
        إذا(p==nullptr) {
            أرجع nullptr;
        }
        uprv_memcpy(p, ptr, (size_t)لمائی*حجم(T));
    }
    نتیجہ_سمرتھا=لمائی;
    reset_منزل_ڈھیر_Array();
    أرجع p;
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
سانچا<typename H, typename T, int32_t ڈھیر_سمرتھا>
class Maybe_ڈھیر_سرخی_And_Array {
عام:
    // No heap allocation. Use only on the stack.
    ساكن فراغ* U_EXPORT2 عامل new(size_t) noexcept = delete;
    ساكن فراغ* U_EXPORT2 عامل new[](size_t) noexcept = delete;
    ساكن فراغ* U_EXPORT2 عامل new(size_t, فراغ*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_ڈھیر_سرخی_And_Array() : ptr(&ڈھیر_سرخی), سمرتھا(ڈھیر_سمرتھا), need_منزل_Release(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_ڈھیر_سرخی_And_Array() { release_حافظہ(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_سمرتھا() ثابت { أرجع سمرتھا; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() ثابت U_LIFETIME_BOUND { أرجع ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* get_Array_شروع() ثابت U_LIFETIME_BOUND { أرجع reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* get_Array_حد_بندی() ثابت U_LIFETIME_BOUND { أرجع get_Array_شروع() + سمرتھا; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    عامل H*() ثابت U_LIFETIME_BOUND { أرجع ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& عامل[](ptrdiff_t i) U_LIFETIME_BOUND { أرجع get_Array_شروع()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    فراغ aliasInstead(H *other_حافظہ, int32_t other_سمرتھا) {
        إذا(other_حافظہ!=nullptr && other_سمرتھا>0) {
            release_حافظہ();
            ptr=other_حافظہ;
            سمرتھا=other_سمرتھا;
            need_منزل_Release=false;
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
    مضمن H* resize(int32_t new_سمرتھا, int32_t لمائی = 0) U_LIFETIME_BOUND;
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
    مضمن H *orphanOrClone(int32_t لمائی, int32_t &نتیجہ_سمرتھا);
خاص:
    H *ptr;
    int32_t سمرتھا;
    UBool need_منزل_Release;
    // stackHeader must precede stackArray immediately.
    H ڈھیر_سرخی;
    T ڈھیر_Array[ڈھیر_سمرتھا];
    فراغ release_حافظہ() {
        إذا(need_منزل_Release) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool عامل==(ثابت Maybe_ڈھیر_سرخی_And_Array & /*other*/) {أرجع false;}
    bool عامل!=(ثابت Maybe_ڈھیر_سرخی_And_Array & /*other*/) {أرجع true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_ڈھیر_سرخی_And_Array(ثابت Maybe_ڈھیر_سرخی_And_Array & /*other*/) {}
    فراغ عامل=(ثابت Maybe_ڈھیر_سرخی_And_Array & /*other*/) {}
};

سانچا <typename H, typename T, int32_t ڈھیر_سمرتھا>
مضمن H* Maybe_ڈھیر_سرخی_And_Array<H, T, ڈھیر_سمرتھا>::resize(int32_t new_سمرتھا,
                                                                int32_t لمائی) U_LIFETIME_BOUND {
    إذا(new_سمرتھا>=0) {
#إذا U_DEBUG && defined(UPRV_MALLOC_گنتی)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", حجم(H),new_سمرتھا,حجم(T));
#جے_انت
        H *p=(H *)uprv_malloc(حجم(H)+new_سمرتھا*حجم(T));
        إذا(p!=nullptr) {
            إذا(لمائی<0) {
                لمائی=0;
            } وإلا إذا(لمائی>0) {
                إذا(لمائی>سمرتھا) {
                    لمائی=سمرتھا;
                }
                إذا(لمائی>new_سمرتھا) {
                    لمائی=new_سمرتھا;
                }
            }
            uprv_memcpy(p, ptr, حجم(H)+(size_t)لمائی*حجم(T));
            release_حافظہ();
            ptr=p;
            سمرتھا=new_سمرتھا;
            need_منزل_Release=true;
        }
        أرجع p;
    } وإلا {
        أرجع nullptr;
    }
}

سانچا<typename H, typename T, int32_t ڈھیر_سمرتھا>
مضمن H *Maybe_ڈھیر_سرخی_And_Array<H, T, ڈھیر_سمرتھا>::orphanOrClone(int32_t لمائی,
                                                                       int32_t &نتیجہ_سمرتھا) {
    H *p;
    إذا(need_منزل_Release) {
        p=ptr;
    } وإلا {
        إذا(لمائی<0) {
            لمائی=0;
        } وإلا إذا(لمائی>سمرتھا) {
            لمائی=سمرتھا;
        }
#إذا U_DEBUG && defined(UPRV_MALLOC_گنتی)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", حجم(H),لمائی,حجم(T));
#جے_انت
        p=(H *)uprv_malloc(حجم(H)+لمائی*حجم(T));
        إذا(p==nullptr) {
            أرجع nullptr;
        }
        uprv_memcpy(p, ptr, حجم(H)+(size_t)لمائی*حجم(T));
    }
    نتیجہ_سمرتھا=لمائی;
    ptr=&ڈھیر_سرخی;
    سمرتھا=ڈھیر_سمرتھا;
    need_منزل_Release=false;
    أرجع p;
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
سانچا<typename T, int32_t ڈھیر_سمرتھا = 8>
class حافظہ_Pool : عام UMemory {
عام:
    حافظہ_Pool() : f_گنتی(0), fPool() {}

    ~حافظہ_Pool() {
        لكل (int32_t i = 0; i < f_گنتی; ++i) {
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
        std::وٹاندرا(f_گنتی, other.f_گنتی);
        std::وٹاندرا(fPool, other.fPool);
        أرجع *this;
    }

    /**
     * Creates a new object of typename T, by forwarding any and all arguments
     * to the typename T constructor.
     *
     * @param args Arguments to be forwarded to the typename T constructor.
     * @return A pointer to the newly created object, or nullptr on error.
     */
    سانچا <typename... دلیلاں>
    T* create(دلیلاں&&... دلیلاں_2) U_LIFETIME_BOUND {
        int32_t سمرتھا = fPool.get_سمرتھا();
        إذا (f_گنتی == سمرتھا &&
            fPool.resize(سمرتھا == ڈھیر_سمرتھا ? 4 * سمرتھا : 2 * سمرتھا,
                         سمرتھا) == nullptr) {
            أرجع nullptr;
        }
        أرجع fPool[f_گنتی++] = new T(std::forward<دلیلاں>(دلیلاں_2)...);
    }

    سانچا <typename... دلیلاں>
    T* create_And_Check_Error_کوڈ(UError_کوڈ& حالت, دلیلاں&&... دلیلاں_2) U_LIFETIME_BOUND {
        إذا (U_FAILURE(حالت)) {
            أرجع nullptr;
        }
        T *اشارہ_گر = this->create(دلیلاں_2...);
        إذا (U_SUCCESS(حالت) && اشارہ_گر == nullptr) {
            حالت = U_حافظہ_ونڈ_ERROR;
        }
        أرجع اشارہ_گر;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t گنتی() ثابت {
        أرجع f_گنتی;
    }

محمي:
    int32_t f_گنتی;
    Maybe_ڈھیر_Array<T*, ڈھیر_سمرتھا> fPool;
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
سانچا<typename T, int32_t ڈھیر_سمرتھا = 8>
class Maybe_ڈھیر_سمتیہ : محمي حافظہ_Pool<T, ڈھیر_سمرتھا> {
عام:
    سانچا <typename... دلیلاں>
    T* emplace_پٹھ(دلیلاں&&... دلیلاں_2) U_LIFETIME_BOUND {
        أرجع this->create(دلیلاں_2...);
    }

    سانچا <typename... دلیلاں>
    T* emplace_پٹھ_And_Check_Error_کوڈ(UError_کوڈ& حالت, دلیلاں&&... دلیلاں_2) U_LIFETIME_BOUND {
        أرجع this->create_And_Check_Error_کوڈ(حالت, دلیلاں_2...);
    }

    int32_t لمائی() ثابت {
        أرجع this->f_گنتی;
    }

    T** getAlias() U_LIFETIME_BOUND {
        أرجع this->fPool.getAlias();
    }

    ثابت T* ثابت* getAlias() ثابت U_LIFETIME_BOUND {
        أرجع this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ثابت T* عامل[](ptrdiff_t i) ثابت U_LIFETIME_BOUND {
        أرجع this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* عامل[](ptrdiff_t i) U_LIFETIME_BOUND {
        أرجع this->fPool[i];
    }
};

namespace prv {

#إذا U_OVERRIDE_CXX_ونڈ

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
سانچا <typename T, فراغ deallocate(فراغ *), typename = فراغ> class Deleter {
  عام:
    Deleter() = افتراضي;
    فراغ عامل()(T *ثابت ptr) ثابت {
        إذا constexpr (!std::is_trivially_destructible_v<T>) {
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
سانچا <typename T, فراغ deallocate(فراغ *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  عام:
    Deleter() = افتراضي;
    فراغ عامل()(T *ثابت ptr) ثابت { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
سانچا <typename T, فراغ deallocate(فراغ *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  عام:
    Deleter(ثابت std::size_t ناپ_2) : ناپ(ناپ_2) {}
    فراغ عامل()(T *ثابت ptr) ثابت {
        لكل (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(ناپ) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  خاص:
    ثابت std::size_t ناپ;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
سانچا <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#وإلا

using std::unique_ptr;

#جے_انت

/**
 * Helper for make_unique<T, Args...>; forwards all but the last of args to the constructor of
 * T, uses the last of args as the error code for allocation errors.
 *
 * As usual, does nothing if the incoming UErrorCode already is U_FAILURE.
 *
 * @internal
 */
سانچا <typename T, typename... دلیلاں, std::size_t... i>
unique_ptr<T> make_unique_impl(std::tuple<دلیلاں...> دلیلاں_2,
                               std::اشاریہ_sequence<i...> constructor_arg_indices) {
    static_assert(std::is_same_v<decltype(constructor_arg_indices),
                                 std::make_اشاریہ_sequence<حجم...(دلیلاں) - 1>>);
    خودکار &&error = std::get<حجم...(دلیلاں) - 1>(دلیلاں_2);
    static_assert(std::is_same_v<decltype(error), UError_کوڈ &>);
    إذا (U_FAILURE(error)) {
        أرجع nullptr;
    }
#إذا U_OVERRIDE_CXX_ونڈ
    فراغ *ثابت بفر = uprv_malloc(حجم(T));
    إذا (بفر == nullptr) {
        error = U_حافظہ_ونڈ_ERROR;
        أرجع nullptr;
    }
    أرجع unique_ptr<T>(new (بفر) T(std::get<i>(دلیلاں_2)...));
#وإلا
    أرجع std::make_unique<T>(std::get<i>(دلیلاں_2)...);
#جے_انت
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
سانچا <typename T, typename... دلیلاں, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique(دلیلاں &&...دلیلاں_2) {
    أرجع make_unique_impl<T>(std::forward_as_tuple(دلیلاں_2...),
                               std::make_اشاریہ_sequence<حجم...(دلیلاں) - 1>());
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
سانچا <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique(ثابت std::size_t n, UError_کوڈ &error) {
    إذا (U_FAILURE(error)) {
        أرجع nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_ناپ = SIZE_MAX / حجم(std::remove_extent_t<T>);
    إذا (n > max_ناپ) {
        error = U_ILLEGAL_دلیل_ERROR;
        أرجع nullptr;
    }
#إذا U_OVERRIDE_CXX_ونڈ
    فراغ *ثابت بفر = uprv_malloc(حجم(std::remove_extent_t<T>) * n);
    إذا (بفر == nullptr) {
        error = U_حافظہ_ونڈ_ERROR;
        إذا constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            أرجع nullptr;
        } وإلا {
            أرجع unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    إذا constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        أرجع unique_ptr<T>(new (بفر) std::remove_extent_t<T>[n]());
    } وإلا {
        أرجع unique_ptr<T>(new (بفر) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#وإلا
    أرجع std::make_unique<T>(n);
#جے_انت
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
سانچا <typename T, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique_for_overwrite(UError_کوڈ &error) {
    إذا (U_FAILURE(error)) {
        أرجع nullptr;
    }
#إذا U_OVERRIDE_CXX_ونڈ
    فراغ *ثابت بفر = uprv_malloc(حجم(T));
    إذا (بفر == nullptr) {
        error = U_حافظہ_ونڈ_ERROR;
        أرجع nullptr;
    }
    أرجع unique_ptr<T>(new (بفر) T);
#وإلاإذا U_CPLUSPLUS_نسخہ >= 20
    أرجع std::make_unique_for_overwrite<T>();
#وإلا
    أرجع unique_ptr<T>(new T);
#جے_انت
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
سانچا <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique_for_overwrite(ثابت std::size_t n, UError_کوڈ &error) {
    إذا (U_FAILURE(error)) {
        أرجع nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_ناپ = SIZE_MAX / حجم(std::remove_extent_t<T>);
    إذا (n > max_ناپ) {
        error = U_ILLEGAL_دلیل_ERROR;
        أرجع nullptr;
    }
#إذا U_OVERRIDE_CXX_ونڈ
    فراغ *ثابت بفر = uprv_malloc(حجم(std::remove_extent_t<T>) * n);
    إذا (بفر == nullptr) {
        error = U_حافظہ_ونڈ_ERROR;
        إذا constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            أرجع nullptr;
        } وإلا {
            أرجع unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    إذا constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        أرجع unique_ptr<T>(new (بفر) std::remove_extent_t<T>[n]);
    } وإلا {
        أرجع unique_ptr<T>(new (بفر) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#وإلاإذا U_CPLUSPLUS_نسخہ >= 20
    أرجع std::make_unique_for_overwrite<T>(n);
#وإلا
    أرجع unique_ptr<T>(new std::remove_extent_t<T>[n]);
#جے_انت
}

}  // namespace prv

U_NAMESPACE_سرا

#جے_انت  /* __cplusplus */
#جے_انت  /* CMEMORY_H */
