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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CMEMORY_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMEMORY_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stddef.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localpointer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

#ਜੇ U_DEBUG && defined(UPRV_MALLOC_ਗਿਣਤੀ)
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਜੇ_ਅੰਤ

// uprv_memcpy and uprv_memmove
#ਜੇ defined(__clang__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_memcpy(dst, src, ਮਾਪ_2) UPRV_ਖੰਡ_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ਨਿਰਾ); \
    U_ASSERT(src != ਨਿਰਾ); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE ਸਮ੍ਰਿਤੀ_ਨਕਲ(dst, src, ਮਾਪ_2); \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_memmove(dst, src, ਮਾਪ_2) UPRV_ਖੰਡ_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ਨਿਰਾ); \
    U_ASSERT(src != ਨਿਰਾ); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, ਮਾਪ_2); \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਜਾਂਜੇ defined(__GNUC__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_memcpy(dst, src, ਮਾਪ_2) UPRV_ਖੰਡ_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ਨਿਰਾ); \
    U_ASSERT(src != ਨਿਰਾ); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE ਸਮ੍ਰਿਤੀ_ਨਕਲ(dst, src, ਮਾਪ_2); \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_memmove(dst, src, ਮਾਪ_2) UPRV_ਖੰਡ_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ਨਿਰਾ); \
    U_ASSERT(src != ਨਿਰਾ); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, ਮਾਪ_2); \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_memcpy(dst, src, ਮਾਪ_2) UPRV_ਖੰਡ_MACRO_BEGIN { \
    U_ASSERT(dst != ਨਿਰਾ); \
    U_ASSERT(src != ਨਿਰਾ); \
    U_STANDARD_CPP_NAMESPACE ਸਮ੍ਰਿਤੀ_ਨਕਲ(dst, src, ਮਾਪ_2); \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_memmove(dst, src, ਮਾਪ_2) UPRV_ਖੰਡ_MACRO_BEGIN { \
    U_ASSERT(dst != ਨਿਰਾ); \
    U_ASSERT(src != ਨਿਰਾ); \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, ਮਾਪ_2); \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਜੇ_ਅੰਤ

/**
 * \def UPRV_LENGTHOF
 * Convenience macro to determine the length of a fixed array at compile-time.
 * @param array A fixed length array
 * @return The length of the array, in elements
 * @internal
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPRV_LENGTHOF(array) (int32_t)(ਆਕਾਰ(array)/ਆਕਾਰ((array)[0]))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_memset(ਬਫਰ, mark, ਮਾਪ_2) U_STANDARD_CPP_NAMESPACE memset(ਬਫਰ, mark, ਮਾਪ_2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_memcmp(buffer1, buffer2, ਮਾਪ_2) U_STANDARD_CPP_NAMESPACE memcmp(buffer1, buffer2,ਮਾਪ_2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ uprv_memchr(ptr, ਮੁੱਲ_2, ਅੰਕ) U_STANDARD_CPP_NAMESPACE memchr(ptr, ਮੁੱਲ_2, ਅੰਕ)

U_CAPI ਖਾਲੀ * U_EXPORT2
uprv_malloc(size_t s) U_MALLOC_ATTR U_ALLOC_ਮਾਪ_ATTR(1);

U_CAPI ਖਾਲੀ * U_EXPORT2
uprv_realloc(ਖਾਲੀ *mem, size_t ਮਾਪ_2) U_ALLOC_ਮਾਪ_ATTR(2);

U_CAPI ਖਾਲੀ U_EXPORT2
uprv_free(ਖਾਲੀ *mem);

U_CAPI ਖਾਲੀ * U_EXPORT2
uprv_calloc(size_t ਅੰਕ, size_t ਮਾਪ_2) U_MALLOC_ATTR U_ALLOC_ਮਾਪ_ATTR2(1,2);

/**
 * Get the least significant bits of a pointer (a memory address).
 * For example, with a mask of 3, the macro gets the 2 least significant bits,
 * which will be 0 if the pointer is 32-bit (4-byte) aligned.
 *
 * uintptr_t is the most appropriate integer type to cast to.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_ਸੂਚਕ_ਨਕਾਬ_LSB(ptr, ਨਕਾਬ) ((uintptr_t)(ptr) & (ਨਕਾਬ))

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STATIC_NEW(type) [] () { \
    alignas(type) ਸਥਿਰ ਅੱਖਰ storage[ਆਕਾਰ(type)]; \
    ਵਾਪਸ new(storage) type();} ()

/**
  *  Heap clean up function, called from u_cleanup()
  *    Clears any user heap functions from u_setMemoryFunctions()
  *    Does NOT deallocate any remaining allocated memory.
  */
U_CFUNC UBool 
cmemory_cleanup(ਖਾਲੀ);

/**
 * A function called by <TT>uhash_remove</TT>,
 * <TT>uhash_close</TT>, or <TT>uhash_put</TT> to delete
 * an existing key or value.
 * @param obj A key or value stored in a hashtable
 * @see uprv_deleteUObject
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ U_CALLCONV UObjectDeleter(ਖਾਲੀ* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI ਖਾਲੀ U_EXPORT2
uprv_deleteUObject(ਖਾਲੀ *obj);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus

#ਅਨ੍ਤਰ੍ਭਾਵ <memory>
#ਅਨ੍ਤਰ੍ਭਾਵ <utility>
#ਅਨ੍ਤਰ੍ਭਾਵ <type_traits>
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

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
ਸਾਂਚਾ<typename T>
class Local_ਸਮ੍ਰਿਤੀ : ਜਨਤਕ Local_ਸੂਚਕ_ਅਧਾਰ<T> {
ਜਨਤਕ:
    using Local_ਸੂਚਕ_ਅਧਾਰ<T>::ਸੰਚਾਲਕ*;
    using Local_ਸੂਚਕ_ਅਧਾਰ<T>::ਸੰਚਾਲਕ->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an array of T items that is adopted
     */
    explicit Local_ਸਮ੍ਰਿਤੀ(T *p=nullptr) : Local_ਸੂਚਕ_ਅਧਾਰ<T>(p) {}
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     */
    Local_ਸਮ੍ਰਿਤੀ(Local_ਸਮ੍ਰਿਤੀ<T> &&src) noexcept : Local_ਸੂਚਕ_ਅਧਾਰ<T>(src.ptr) {
        src.ptr=nullptr;
    }
    /**
     * Destructor deletes the memory it owns.
     */
    ~Local_ਸਮ੍ਰਿਤੀ() {
        uprv_free(Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr);
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     */
    Local_ਸਮ੍ਰਿਤੀ<T>& ਸੰਚਾਲਕ=(Local_ਸਮ੍ਰਿਤੀ<T>&& src) noexcept U_LIFETIME_BOUND {
        uprv_free(Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr);
        Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr=src.ptr;
        src.ptr=nullptr;
        ਵਾਪਸ *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ(Local_ਸਮ੍ਰਿਤੀ<T> &other) noexcept {
        T *ਆਰਜ਼ੀ=Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr;
        Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr=other.ptr;
        other.ptr=ਆਰਜ਼ੀ;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    ਮਿੱਤਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ(Local_ਸਮ੍ਰਿਤੀ<T> &p1, Local_ਸਮ੍ਰਿਤੀ<T> &p2) noexcept {
        p1.ਅਦਲਾ_ਬਦਲੀ(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    ਖਾਲੀ adoptInstead(T *p) {
        uprv_free(Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr);
        Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr=p;
    }
    /**
     * Deletes the array it owns, allocates a new one and reset its bytes to 0.
     * Returns the new array pointer.
     * If the allocation fails, then the current array is unchanged and
     * this method returns nullptr.
     * @param newCapacity must be >0
     * @return the allocated array pointer, or nullptr if the allocation failed
     */
    ਸਤਰਅੰਦਰ T* allocateInsteadAndReset(int32_t new_ਸਮਰੱਥਾ = 1) U_LIFETIME_BOUND;
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
    ਸਤਰਅੰਦਰ T* allocateInsteadAndCopy(int32_t new_ਸਮਰੱਥਾ = 1, int32_t ਲੰਮਾਈ = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& ਸੰਚਾਲਕ[](ptrdiff_t i) ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr[i]; }
};

ਸਾਂਚਾ <typename T>
ਸਤਰਅੰਦਰ T* Local_ਸਮ੍ਰਿਤੀ<T>::allocateInsteadAndReset(int32_t new_ਸਮਰੱਥਾ) U_LIFETIME_BOUND {
    ਜੇ(new_ਸਮਰੱਥਾ>0) {
        T *p=(T *)uprv_malloc(new_ਸਮਰੱਥਾ*ਆਕਾਰ(T));
        ਜੇ(p!=nullptr) {
            uprv_memset(p, 0, new_ਸਮਰੱਥਾ*ਆਕਾਰ(T));
            uprv_free(Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr);
            Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr=p;
        }
        ਵਾਪਸ p;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ nullptr;
    }
}

ਸਾਂਚਾ <typename T>
ਸਤਰਅੰਦਰ T* Local_ਸਮ੍ਰਿਤੀ<T>::allocateInsteadAndCopy(int32_t new_ਸਮਰੱਥਾ,
                                                 int32_t ਲੰਮਾਈ) U_LIFETIME_BOUND {
    ਜੇ(new_ਸਮਰੱਥਾ>0) {
        T *p=(T *)uprv_malloc(new_ਸਮਰੱਥਾ*ਆਕਾਰ(T));
        ਜੇ(p!=nullptr) {
            ਜੇ(ਲੰਮਾਈ>0) {
                ਜੇ(ਲੰਮਾਈ>new_ਸਮਰੱਥਾ) {
                    ਲੰਮਾਈ=new_ਸਮਰੱਥਾ;
                }
                uprv_memcpy(p, Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr, (size_t)ਲੰਮਾਈ*ਆਕਾਰ(T));
            }
            uprv_free(Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr);
            Local_ਸੂਚਕ_ਅਧਾਰ<T>::ptr=p;
        }
        ਵਾਪਸ p;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ nullptr;
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
ਸਾਂਚਾ<typename T, int32_t ਢੇਰ_ਸਮਰੱਥਾ>
class Maybe_ਢੇਰ_Array {
ਜਨਤਕ:
    // No heap allocation. Use only on the stack.
    ਸਥਿਰ ਖਾਲੀ* U_EXPORT2 ਸੰਚਾਲਕ new(size_t) noexcept = delete;
    ਸਥਿਰ ਖਾਲੀ* U_EXPORT2 ਸੰਚਾਲਕ new[](size_t) noexcept = delete;
    ਸਥਿਰ ਖਾਲੀ* U_EXPORT2 ਸੰਚਾਲਕ new(size_t, ਖਾਲੀ*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_ਢੇਰ_Array() : ptr(ਢੇਰ_Array), ਸਮਰੱਥਾ(ਢੇਰ_ਸਮਰੱਥਾ), need_ਮੰਜ਼ਿਲ_Release(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_ਢੇਰ_Array(int32_t new_ਸਮਰੱਥਾ, UError_ਕੋਡ ਸਥਿਤੀ) : Maybe_ਢੇਰ_Array() {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ;
        }
        ਜੇ (ਸਮਰੱਥਾ < new_ਸਮਰੱਥਾ) {
            ਜੇ (resize(new_ਸਮਰੱਥਾ) == nullptr) {
                ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            }
        }
    }
    /**
     * Destructor deletes the array (if owned).
     */
    ~Maybe_ਢੇਰ_Array() { releaseArray(); }
    /**
     * Move constructor: transfers ownership or copies the stack array.
     */
    Maybe_ਢੇਰ_Array(Maybe_ਢੇਰ_Array<T, ਢੇਰ_ਸਮਰੱਥਾ> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_ਢੇਰ_Array<T, ਢੇਰ_ਸਮਰੱਥਾ>&
    ਸੰਚਾਲਕ=(Maybe_ਢੇਰ_Array<T, ਢੇਰ_ਸਮਰੱਥਾ>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_ਸਮਰੱਥਾ() ਅਚਲ { ਵਾਪਸ ਸਮਰੱਥਾ; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* get_Array_ਹੱਦਬੰਦੀ() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ getAlias() + ਸਮਰੱਥਾ; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ਅਚਲ T& ਸੰਚਾਲਕ[](ptrdiff_t i) ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& ਸੰਚਾਲਕ[](ptrdiff_t i) U_LIFETIME_BOUND { ਵਾਪਸ ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    ਖਾਲੀ aliasInstead(T *otherArray, int32_t other_ਸਮਰੱਥਾ) {
        ਜੇ(otherArray!=nullptr && other_ਸਮਰੱਥਾ>0) {
            releaseArray();
            ptr=otherArray;
            ਸਮਰੱਥਾ=other_ਸਮਰੱਥਾ;
            need_ਮੰਜ਼ਿਲ_Release=false;
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
    ਸਤਰਅੰਦਰ T* resize(int32_t new_ਸਮਰੱਥਾ, int32_t ਲੰਮਾਈ = 0) U_LIFETIME_BOUND;
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
    ਸਤਰਅੰਦਰ T *orphanOrClone(int32_t ਲੰਮਾਈ, int32_t &ਨਤੀਜਾ_ਸਮਰੱਥਾ);

ਸੁਰੱਖਿਅਤ:
    // Resizes the array to the size of src, then copies the contents of src.
    ਖਾਲੀ copyFrom(ਅਚਲ Maybe_ਢੇਰ_Array &src, UError_ਕੋਡ &ਸਥਿਤੀ) {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ;
        }
        ਜੇ (this->resize(src.ਸਮਰੱਥਾ, 0) == nullptr) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
        uprv_memcpy(this->ptr, src.ptr, (size_t)ਸਮਰੱਥਾ * ਆਕਾਰ(T));
    }

ਨਿਜੀ:
    T *ptr;
    int32_t ਸਮਰੱਥਾ;
    UBool need_ਮੰਜ਼ਿਲ_Release;
    T ਢੇਰ_Array[ਢੇਰ_ਸਮਰੱਥਾ];
    ਖਾਲੀ releaseArray() {
        ਜੇ(need_ਮੰਜ਼ਿਲ_Release) {
            uprv_free(ptr);
        }
    }
    ਖਾਲੀ reset_ਮੰਜ਼ਿਲ_ਢੇਰ_Array() {
        ptr=ਢੇਰ_Array;
        ਸਮਰੱਥਾ=ਢੇਰ_ਸਮਰੱਥਾ;
        need_ਮੰਜ਼ਿਲ_Release=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool ਸੰਚਾਲਕ==(ਅਚਲ Maybe_ਢੇਰ_Array & /*other*/) = delete;
    bool ਸੰਚਾਲਕ!=(ਅਚਲ Maybe_ਢੇਰ_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_ਢੇਰ_Array(ਅਚਲ Maybe_ਢੇਰ_Array & /*other*/) = delete;
    ਖਾਲੀ ਸੰਚਾਲਕ=(ਅਚਲ Maybe_ਢੇਰ_Array & /*other*/) = delete;
};

ਸਾਂਚਾ<typename T, int32_t ਢੇਰ_ਸਮਰੱਥਾ>
icu::Maybe_ਢੇਰ_Array<T, ਢੇਰ_ਸਮਰੱਥਾ>::Maybe_ਢੇਰ_Array(
        Maybe_ਢੇਰ_Array <T, ਢੇਰ_ਸਮਰੱਥਾ>&& src) noexcept
        : ptr(src.ptr), ਸਮਰੱਥਾ(src.ਸਮਰੱਥਾ), need_ਮੰਜ਼ਿਲ_Release(src.need_ਮੰਜ਼ਿਲ_Release) {
    ਜੇ (src.ptr == src.ਢੇਰ_Array) {
        ptr = ਢੇਰ_Array;
        uprv_memcpy(ਢੇਰ_Array, src.ਢੇਰ_Array, ਆਕਾਰ(T) * src.ਸਮਰੱਥਾ);
    } ਨਹੀਂਤਾਂ {
        src.reset_ਮੰਜ਼ਿਲ_ਢੇਰ_Array();  // take ownership away from src
    }
}

ਸਾਂਚਾ <typename T, int32_t ਢੇਰ_ਸਮਰੱਥਾ>
ਸਤਰਅੰਦਰ Maybe_ਢੇਰ_Array<T, ਢੇਰ_ਸਮਰੱਥਾ>& Maybe_ਢੇਰ_Array<T, ਢੇਰ_ਸਮਰੱਥਾ>::ਸੰਚਾਲਕ=(
    Maybe_ਢੇਰ_Array<T, ਢੇਰ_ਸਮਰੱਥਾ>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    ਸਮਰੱਥਾ = src.ਸਮਰੱਥਾ;
    need_ਮੰਜ਼ਿਲ_Release = src.need_ਮੰਜ਼ਿਲ_Release;
    ਜੇ (src.ptr == src.ਢੇਰ_Array) {
        ptr = ਢੇਰ_Array;
        uprv_memcpy(ਢੇਰ_Array, src.ਢੇਰ_Array, ਆਕਾਰ(T) * src.ਸਮਰੱਥਾ);
    } ਨਹੀਂਤਾਂ {
        ptr = src.ptr;
        src.reset_ਮੰਜ਼ਿਲ_ਢੇਰ_Array();  // take ownership away from src
    }
    ਵਾਪਸ *this;
}

ਸਾਂਚਾ <typename T, int32_t ਢੇਰ_ਸਮਰੱਥਾ>
ਸਤਰਅੰਦਰ T* Maybe_ਢੇਰ_Array<T, ਢੇਰ_ਸਮਰੱਥਾ>::resize(int32_t new_ਸਮਰੱਥਾ,
                                                    int32_t ਲੰਮਾਈ) U_LIFETIME_BOUND {
    ਜੇ(new_ਸਮਰੱਥਾ>0) {
#ਜੇ U_DEBUG && defined(UPRV_MALLOC_ਗਿਣਤੀ)
        ::fprintf(::stderr, "MaybeStackArray (resize) alloc %d * %lu\n", new_ਸਮਰੱਥਾ, ਆਕਾਰ(T));
#ਜੇ_ਅੰਤ
        T *p=(T *)uprv_malloc(new_ਸਮਰੱਥਾ*ਆਕਾਰ(T));
        ਜੇ(p!=nullptr) {
            ਜੇ(ਲੰਮਾਈ>0) {
                ਜੇ(ਲੰਮਾਈ>ਸਮਰੱਥਾ) {
                    ਲੰਮਾਈ=ਸਮਰੱਥਾ;
                }
                ਜੇ(ਲੰਮਾਈ>new_ਸਮਰੱਥਾ) {
                    ਲੰਮਾਈ=new_ਸਮਰੱਥਾ;
                }
                uprv_memcpy(p, ptr, (size_t)ਲੰਮਾਈ*ਆਕਾਰ(T));
            }
            releaseArray();
            ptr=p;
            ਸਮਰੱਥਾ=new_ਸਮਰੱਥਾ;
            need_ਮੰਜ਼ਿਲ_Release=true;
        }
        ਵਾਪਸ p;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ nullptr;
    }
}

ਸਾਂਚਾ<typename T, int32_t ਢੇਰ_ਸਮਰੱਥਾ>
ਸਤਰਅੰਦਰ T *Maybe_ਢੇਰ_Array<T, ਢੇਰ_ਸਮਰੱਥਾ>::orphanOrClone(int32_t ਲੰਮਾਈ, int32_t &ਨਤੀਜਾ_ਸਮਰੱਥਾ) {
    T *p;
    ਜੇ(need_ਮੰਜ਼ਿਲ_Release) {
        p=ptr;
    } ਨਹੀਂਤਾਂ ਜੇ(ਲੰਮਾਈ<=0) {
        ਵਾਪਸ nullptr;
    } ਨਹੀਂਤਾਂ {
        ਜੇ(ਲੰਮਾਈ>ਸਮਰੱਥਾ) {
            ਲੰਮਾਈ=ਸਮਰੱਥਾ;
        }
        p=(T *)uprv_malloc(ਲੰਮਾਈ*ਆਕਾਰ(T));
#ਜੇ U_DEBUG && defined(UPRV_MALLOC_ਗਿਣਤੀ)
      ::fprintf(::stderr,"MaybeStacArray (orphan) alloc %d * %lu\n", ਲੰਮਾਈ,ਆਕਾਰ(T));
#ਜੇ_ਅੰਤ
        ਜੇ(p==nullptr) {
            ਵਾਪਸ nullptr;
        }
        uprv_memcpy(p, ptr, (size_t)ਲੰਮਾਈ*ਆਕਾਰ(T));
    }
    ਨਤੀਜਾ_ਸਮਰੱਥਾ=ਲੰਮਾਈ;
    reset_ਮੰਜ਼ਿਲ_ਢੇਰ_Array();
    ਵਾਪਸ p;
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
ਸਾਂਚਾ<typename H, typename T, int32_t ਢੇਰ_ਸਮਰੱਥਾ>
class Maybe_ਢੇਰ_ਸਿਰਲੇਖ_And_Array {
ਜਨਤਕ:
    // No heap allocation. Use only on the stack.
    ਸਥਿਰ ਖਾਲੀ* U_EXPORT2 ਸੰਚਾਲਕ new(size_t) noexcept = delete;
    ਸਥਿਰ ਖਾਲੀ* U_EXPORT2 ਸੰਚਾਲਕ new[](size_t) noexcept = delete;
    ਸਥਿਰ ਖਾਲੀ* U_EXPORT2 ਸੰਚਾਲਕ new(size_t, ਖਾਲੀ*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_ਢੇਰ_ਸਿਰਲੇਖ_And_Array() : ptr(&ਢੇਰ_ਸਿਰਲੇਖ), ਸਮਰੱਥਾ(ਢੇਰ_ਸਮਰੱਥਾ), need_ਮੰਜ਼ਿਲ_Release(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_ਢੇਰ_ਸਿਰਲੇਖ_And_Array() { release_ਸਮ੍ਰਿਤੀ(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_ਸਮਰੱਥਾ() ਅਚਲ { ਵਾਪਸ ਸਮਰੱਥਾ; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* get_Array_ਸ਼ੁਰੂ() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* get_Array_ਹੱਦਬੰਦੀ() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ get_Array_ਸ਼ੁਰੂ() + ਸਮਰੱਥਾ; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    ਸੰਚਾਲਕ H*() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& ਸੰਚਾਲਕ[](ptrdiff_t i) U_LIFETIME_BOUND { ਵਾਪਸ get_Array_ਸ਼ੁਰੂ()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    ਖਾਲੀ aliasInstead(H *other_ਸਮ੍ਰਿਤੀ, int32_t other_ਸਮਰੱਥਾ) {
        ਜੇ(other_ਸਮ੍ਰਿਤੀ!=nullptr && other_ਸਮਰੱਥਾ>0) {
            release_ਸਮ੍ਰਿਤੀ();
            ptr=other_ਸਮ੍ਰਿਤੀ;
            ਸਮਰੱਥਾ=other_ਸਮਰੱਥਾ;
            need_ਮੰਜ਼ਿਲ_Release=false;
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
    ਸਤਰਅੰਦਰ H* resize(int32_t new_ਸਮਰੱਥਾ, int32_t ਲੰਮਾਈ = 0) U_LIFETIME_BOUND;
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
    ਸਤਰਅੰਦਰ H *orphanOrClone(int32_t ਲੰਮਾਈ, int32_t &ਨਤੀਜਾ_ਸਮਰੱਥਾ);
ਨਿਜੀ:
    H *ptr;
    int32_t ਸਮਰੱਥਾ;
    UBool need_ਮੰਜ਼ਿਲ_Release;
    // stackHeader must precede stackArray immediately.
    H ਢੇਰ_ਸਿਰਲੇਖ;
    T ਢੇਰ_Array[ਢੇਰ_ਸਮਰੱਥਾ];
    ਖਾਲੀ release_ਸਮ੍ਰਿਤੀ() {
        ਜੇ(need_ਮੰਜ਼ਿਲ_Release) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool ਸੰਚਾਲਕ==(ਅਚਲ Maybe_ਢੇਰ_ਸਿਰਲੇਖ_And_Array & /*other*/) {ਵਾਪਸ false;}
    bool ਸੰਚਾਲਕ!=(ਅਚਲ Maybe_ਢੇਰ_ਸਿਰਲੇਖ_And_Array & /*other*/) {ਵਾਪਸ true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_ਢੇਰ_ਸਿਰਲੇਖ_And_Array(ਅਚਲ Maybe_ਢੇਰ_ਸਿਰਲੇਖ_And_Array & /*other*/) {}
    ਖਾਲੀ ਸੰਚਾਲਕ=(ਅਚਲ Maybe_ਢੇਰ_ਸਿਰਲੇਖ_And_Array & /*other*/) {}
};

ਸਾਂਚਾ <typename H, typename T, int32_t ਢੇਰ_ਸਮਰੱਥਾ>
ਸਤਰਅੰਦਰ H* Maybe_ਢੇਰ_ਸਿਰਲੇਖ_And_Array<H, T, ਢੇਰ_ਸਮਰੱਥਾ>::resize(int32_t new_ਸਮਰੱਥਾ,
                                                                int32_t ਲੰਮਾਈ) U_LIFETIME_BOUND {
    ਜੇ(new_ਸਮਰੱਥਾ>=0) {
#ਜੇ U_DEBUG && defined(UPRV_MALLOC_ਗਿਣਤੀ)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", ਆਕਾਰ(H),new_ਸਮਰੱਥਾ,ਆਕਾਰ(T));
#ਜੇ_ਅੰਤ
        H *p=(H *)uprv_malloc(ਆਕਾਰ(H)+new_ਸਮਰੱਥਾ*ਆਕਾਰ(T));
        ਜੇ(p!=nullptr) {
            ਜੇ(ਲੰਮਾਈ<0) {
                ਲੰਮਾਈ=0;
            } ਨਹੀਂਤਾਂ ਜੇ(ਲੰਮਾਈ>0) {
                ਜੇ(ਲੰਮਾਈ>ਸਮਰੱਥਾ) {
                    ਲੰਮਾਈ=ਸਮਰੱਥਾ;
                }
                ਜੇ(ਲੰਮਾਈ>new_ਸਮਰੱਥਾ) {
                    ਲੰਮਾਈ=new_ਸਮਰੱਥਾ;
                }
            }
            uprv_memcpy(p, ptr, ਆਕਾਰ(H)+(size_t)ਲੰਮਾਈ*ਆਕਾਰ(T));
            release_ਸਮ੍ਰਿਤੀ();
            ptr=p;
            ਸਮਰੱਥਾ=new_ਸਮਰੱਥਾ;
            need_ਮੰਜ਼ਿਲ_Release=true;
        }
        ਵਾਪਸ p;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ nullptr;
    }
}

ਸਾਂਚਾ<typename H, typename T, int32_t ਢੇਰ_ਸਮਰੱਥਾ>
ਸਤਰਅੰਦਰ H *Maybe_ਢੇਰ_ਸਿਰਲੇਖ_And_Array<H, T, ਢੇਰ_ਸਮਰੱਥਾ>::orphanOrClone(int32_t ਲੰਮਾਈ,
                                                                       int32_t &ਨਤੀਜਾ_ਸਮਰੱਥਾ) {
    H *p;
    ਜੇ(need_ਮੰਜ਼ਿਲ_Release) {
        p=ptr;
    } ਨਹੀਂਤਾਂ {
        ਜੇ(ਲੰਮਾਈ<0) {
            ਲੰਮਾਈ=0;
        } ਨਹੀਂਤਾਂ ਜੇ(ਲੰਮਾਈ>ਸਮਰੱਥਾ) {
            ਲੰਮਾਈ=ਸਮਰੱਥਾ;
        }
#ਜੇ U_DEBUG && defined(UPRV_MALLOC_ਗਿਣਤੀ)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", ਆਕਾਰ(H),ਲੰਮਾਈ,ਆਕਾਰ(T));
#ਜੇ_ਅੰਤ
        p=(H *)uprv_malloc(ਆਕਾਰ(H)+ਲੰਮਾਈ*ਆਕਾਰ(T));
        ਜੇ(p==nullptr) {
            ਵਾਪਸ nullptr;
        }
        uprv_memcpy(p, ptr, ਆਕਾਰ(H)+(size_t)ਲੰਮਾਈ*ਆਕਾਰ(T));
    }
    ਨਤੀਜਾ_ਸਮਰੱਥਾ=ਲੰਮਾਈ;
    ptr=&ਢੇਰ_ਸਿਰਲੇਖ;
    ਸਮਰੱਥਾ=ਢੇਰ_ਸਮਰੱਥਾ;
    need_ਮੰਜ਼ਿਲ_Release=false;
    ਵਾਪਸ p;
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
ਸਾਂਚਾ<typename T, int32_t ਢੇਰ_ਸਮਰੱਥਾ = 8>
class ਸਮ੍ਰਿਤੀ_Pool : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    ਸਮ੍ਰਿਤੀ_Pool() : f_ਗਿਣਤੀ(0), fPool() {}

    ~ਸਮ੍ਰਿਤੀ_Pool() {
        ਲਈ (int32_t i = 0; i < f_ਗਿਣਤੀ; ++i) {
            delete fPool[i];
        }
    }

    ਸਮ੍ਰਿਤੀ_Pool(ਅਚਲ ਸਮ੍ਰਿਤੀ_Pool&) = delete;
    ਸਮ੍ਰਿਤੀ_Pool& ਸੰਚਾਲਕ=(ਅਚਲ ਸਮ੍ਰਿਤੀ_Pool&) = delete;

    ਸਮ੍ਰਿਤੀ_Pool(ਸਮ੍ਰਿਤੀ_Pool&& other) noexcept : f_ਗਿਣਤੀ(other.f_ਗਿਣਤੀ),
                                                fPool(std::move(other.fPool)) {
        other.f_ਗਿਣਤੀ = 0;
    }

    ਸਮ੍ਰਿਤੀ_Pool& ਸੰਚਾਲਕ=(ਸਮ੍ਰਿਤੀ_Pool&& other) noexcept U_LIFETIME_BOUND {
        // Since `this` may contain instances that need to be deleted, we can't
        // just throw them away and replace them with `other`. The normal way of
        // dealing with this in C++ is to swap `this` and `other`, rather than
        // simply overwrite: the destruction of `other` can then take care of
        // running MemoryPool::~MemoryPool() over the still-to-be-deallocated
        // instances.
        std::ਅਦਲਾ_ਬਦਲੀ(f_ਗਿਣਤੀ, other.f_ਗਿਣਤੀ);
        std::ਅਦਲਾ_ਬਦਲੀ(fPool, other.fPool);
        ਵਾਪਸ *this;
    }

    /**
     * Creates a new object of typename T, by forwarding any and all arguments
     * to the typename T constructor.
     *
     * @param args Arguments to be forwarded to the typename T constructor.
     * @return A pointer to the newly created object, or nullptr on error.
     */
    ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ>
    T* create(ਦਲੀਲਾਂ&&... ਦਲੀਲਾਂ_2) U_LIFETIME_BOUND {
        int32_t ਸਮਰੱਥਾ = fPool.get_ਸਮਰੱਥਾ();
        ਜੇ (f_ਗਿਣਤੀ == ਸਮਰੱਥਾ &&
            fPool.resize(ਸਮਰੱਥਾ == ਢੇਰ_ਸਮਰੱਥਾ ? 4 * ਸਮਰੱਥਾ : 2 * ਸਮਰੱਥਾ,
                         ਸਮਰੱਥਾ) == nullptr) {
            ਵਾਪਸ nullptr;
        }
        ਵਾਪਸ fPool[f_ਗਿਣਤੀ++] = new T(std::forward<ਦਲੀਲਾਂ>(ਦਲੀਲਾਂ_2)...);
    }

    ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ>
    T* create_And_Check_Error_ਕੋਡ(UError_ਕੋਡ& ਸਥਿਤੀ, ਦਲੀਲਾਂ&&... ਦਲੀਲਾਂ_2) U_LIFETIME_BOUND {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ nullptr;
        }
        T *ਸੂਚਕ = this->create(ਦਲੀਲਾਂ_2...);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && ਸੂਚਕ == nullptr) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
        ਵਾਪਸ ਸੂਚਕ;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t ਗਿਣਤੀ_2() ਅਚਲ {
        ਵਾਪਸ f_ਗਿਣਤੀ;
    }

ਸੁਰੱਖਿਅਤ:
    int32_t f_ਗਿਣਤੀ;
    Maybe_ਢੇਰ_Array<T*, ਢੇਰ_ਸਮਰੱਥਾ> fPool;
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
ਸਾਂਚਾ<typename T, int32_t ਢੇਰ_ਸਮਰੱਥਾ = 8>
class Maybe_ਢੇਰ_ਸਦਿਸ਼ : ਸੁਰੱਖਿਅਤ ਸਮ੍ਰਿਤੀ_Pool<T, ਢੇਰ_ਸਮਰੱਥਾ> {
ਜਨਤਕ:
    ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ>
    T* emplace_ਪਿੱਠ(ਦਲੀਲਾਂ&&... ਦਲੀਲਾਂ_2) U_LIFETIME_BOUND {
        ਵਾਪਸ this->create(ਦਲੀਲਾਂ_2...);
    }

    ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ>
    T* emplace_ਪਿੱਠ_And_Check_Error_ਕੋਡ(UError_ਕੋਡ& ਸਥਿਤੀ, ਦਲੀਲਾਂ&&... ਦਲੀਲਾਂ_2) U_LIFETIME_BOUND {
        ਵਾਪਸ this->create_And_Check_Error_ਕੋਡ(ਸਥਿਤੀ, ਦਲੀਲਾਂ_2...);
    }

    int32_t ਲੰਮਾਈ() ਅਚਲ {
        ਵਾਪਸ this->f_ਗਿਣਤੀ;
    }

    T** getAlias() U_LIFETIME_BOUND {
        ਵਾਪਸ this->fPool.getAlias();
    }

    ਅਚਲ T* ਅਚਲ* getAlias() ਅਚਲ U_LIFETIME_BOUND {
        ਵਾਪਸ this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ਅਚਲ T* ਸੰਚਾਲਕ[](ptrdiff_t i) ਅਚਲ U_LIFETIME_BOUND {
        ਵਾਪਸ this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* ਸੰਚਾਲਕ[](ptrdiff_t i) U_LIFETIME_BOUND {
        ਵਾਪਸ this->fPool[i];
    }
};

namespace prv {

#ਜੇ U_OVERRIDE_CXX_ਵੰਡ

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
ਸਾਂਚਾ <typename T, ਖਾਲੀ deallocate(ਖਾਲੀ *), typename = ਖਾਲੀ> class Deleter {
  ਜਨਤਕ:
    Deleter() = ਮੂਲ;
    ਖਾਲੀ ਸੰਚਾਲਕ()(T *ਅਚਲ ptr) ਅਚਲ {
        ਜੇ constexpr (!std::is_trivially_destructible_v<T>) {
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
ਸਾਂਚਾ <typename T, ਖਾਲੀ deallocate(ਖਾਲੀ *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  ਜਨਤਕ:
    Deleter() = ਮੂਲ;
    ਖਾਲੀ ਸੰਚਾਲਕ()(T *ਅਚਲ ptr) ਅਚਲ { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
ਸਾਂਚਾ <typename T, ਖਾਲੀ deallocate(ਖਾਲੀ *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  ਜਨਤਕ:
    Deleter(ਅਚਲ std::size_t ਮਾਪ_2) : ਮਾਪ(ਮਾਪ_2) {}
    ਖਾਲੀ ਸੰਚਾਲਕ()(T *ਅਚਲ ptr) ਅਚਲ {
        ਲਈ (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(ਮਾਪ) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  ਨਿਜੀ:
    ਅਚਲ std::size_t ਮਾਪ;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
ਸਾਂਚਾ <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#ਨਹੀਂਤਾਂ

using std::unique_ptr;

#ਜੇ_ਅੰਤ

/**
 * Helper for make_unique<T, Args...>; forwards all but the last of args to the constructor of
 * T, uses the last of args as the error code for allocation errors.
 *
 * As usual, does nothing if the incoming UErrorCode already is U_FAILURE.
 *
 * @internal
 */
ਸਾਂਚਾ <typename T, typename... ਦਲੀਲਾਂ, std::size_t... i>
unique_ptr<T> make_unique_impl(std::tuple<ਦਲੀਲਾਂ...> ਦਲੀਲਾਂ_2,
                               std::ਸੂਚਕਾਂਕ_sequence<i...> constructor_arg_indices) {
    static_assert(std::is_same_v<decltype(constructor_arg_indices),
                                 std::make_ਸੂਚਕਾਂਕ_sequence<ਆਕਾਰ...(ਦਲੀਲਾਂ) - 1>>);
    ਸਵੈਚਾਲਿਤ &&error = std::get<ਆਕਾਰ...(ਦਲੀਲਾਂ) - 1>(ਦਲੀਲਾਂ_2);
    static_assert(std::is_same_v<decltype(error), UError_ਕੋਡ &>);
    ਜੇ (U_FAILURE(error)) {
        ਵਾਪਸ nullptr;
    }
#ਜੇ U_OVERRIDE_CXX_ਵੰਡ
    ਖਾਲੀ *ਅਚਲ ਬਫਰ = uprv_malloc(ਆਕਾਰ(T));
    ਜੇ (ਬਫਰ == nullptr) {
        error = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ nullptr;
    }
    ਵਾਪਸ unique_ptr<T>(new (ਬਫਰ) T(std::get<i>(ਦਲੀਲਾਂ_2)...));
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ std::make_unique<T>(std::get<i>(ਦਲੀਲਾਂ_2)...);
#ਜੇ_ਅੰਤ
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
ਸਾਂਚਾ <typename T, typename... ਦਲੀਲਾਂ, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique(ਦਲੀਲਾਂ &&...ਦਲੀਲਾਂ_2) {
    ਵਾਪਸ make_unique_impl<T>(std::forward_as_tuple(ਦਲੀਲਾਂ_2...),
                               std::make_ਸੂਚਕਾਂਕ_sequence<ਆਕਾਰ...(ਦਲੀਲਾਂ) - 1>());
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
ਸਾਂਚਾ <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique(ਅਚਲ std::size_t n, UError_ਕੋਡ &error) {
    ਜੇ (U_FAILURE(error)) {
        ਵਾਪਸ nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_ਮਾਪ = SIZE_MAX / ਆਕਾਰ(std::remove_extent_t<T>);
    ਜੇ (n > max_ਮਾਪ) {
        error = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ nullptr;
    }
#ਜੇ U_OVERRIDE_CXX_ਵੰਡ
    ਖਾਲੀ *ਅਚਲ ਬਫਰ = uprv_malloc(ਆਕਾਰ(std::remove_extent_t<T>) * n);
    ਜੇ (ਬਫਰ == nullptr) {
        error = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਜੇ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            ਵਾਪਸ nullptr;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    ਜੇ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        ਵਾਪਸ unique_ptr<T>(new (ਬਫਰ) std::remove_extent_t<T>[n]());
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ unique_ptr<T>(new (ਬਫਰ) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ std::make_unique<T>(n);
#ਜੇ_ਅੰਤ
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
ਸਾਂਚਾ <typename T, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique_for_overwrite(UError_ਕੋਡ &error) {
    ਜੇ (U_FAILURE(error)) {
        ਵਾਪਸ nullptr;
    }
#ਜੇ U_OVERRIDE_CXX_ਵੰਡ
    ਖਾਲੀ *ਅਚਲ ਬਫਰ = uprv_malloc(ਆਕਾਰ(T));
    ਜੇ (ਬਫਰ == nullptr) {
        error = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ nullptr;
    }
    ਵਾਪਸ unique_ptr<T>(new (ਬਫਰ) T);
#ਜਾਂਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ >= 20
    ਵਾਪਸ std::make_unique_for_overwrite<T>();
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ unique_ptr<T>(new T);
#ਜੇ_ਅੰਤ
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
ਸਾਂਚਾ <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique_for_overwrite(ਅਚਲ std::size_t n, UError_ਕੋਡ &error) {
    ਜੇ (U_FAILURE(error)) {
        ਵਾਪਸ nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_ਮਾਪ = SIZE_MAX / ਆਕਾਰ(std::remove_extent_t<T>);
    ਜੇ (n > max_ਮਾਪ) {
        error = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ nullptr;
    }
#ਜੇ U_OVERRIDE_CXX_ਵੰਡ
    ਖਾਲੀ *ਅਚਲ ਬਫਰ = uprv_malloc(ਆਕਾਰ(std::remove_extent_t<T>) * n);
    ਜੇ (ਬਫਰ == nullptr) {
        error = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਜੇ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            ਵਾਪਸ nullptr;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    ਜੇ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        ਵਾਪਸ unique_ptr<T>(new (ਬਫਰ) std::remove_extent_t<T>[n]);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ unique_ptr<T>(new (ਬਫਰ) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#ਜਾਂਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ >= 20
    ਵਾਪਸ std::make_unique_for_overwrite<T>(n);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ unique_ptr<T>(new std::remove_extent_t<T>[n]);
#ਜੇ_ਅੰਤ
}

}  // namespace prv

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  /* __cplusplus */
#ਜੇ_ਅੰਤ  /* CMEMORY_H */
