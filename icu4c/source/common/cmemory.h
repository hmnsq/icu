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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CMEMORY_H
#ವ್ಯಾಖ್ಯಾನಿಸು CMEMORY_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <stddef.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#ಆದರೆ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಆದರೆ_ಅಂತ್ಯ

// uprv_memcpy and uprv_memmove
#ಆದರೆ defined(__clang__)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_memcpy(dst, src, ಗಾತ್ರ_3) UPRV_ಖಂಡ_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ಖಾಲಿ); \
    U_ASSERT(src != ಖಾಲಿ); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE ಸ್ಮರಣೆ_ನಕಲು(dst, src, ಗಾತ್ರ_3); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_memmove(dst, src, ಗಾತ್ರ_3) UPRV_ಖಂಡ_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ಖಾಲಿ); \
    U_ASSERT(src != ಖಾಲಿ); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, ಗಾತ್ರ_3); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ಅಥವಾಆದರೆ defined(__GNUC__)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_memcpy(dst, src, ಗಾತ್ರ_3) UPRV_ಖಂಡ_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ಖಾಲಿ); \
    U_ASSERT(src != ಖಾಲಿ); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE ಸ್ಮರಣೆ_ನಕಲು(dst, src, ಗಾತ್ರ_3); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_memmove(dst, src, ಗಾತ್ರ_3) UPRV_ಖಂಡ_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ಖಾಲಿ); \
    U_ASSERT(src != ಖಾಲಿ); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, ಗಾತ್ರ_3); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_memcpy(dst, src, ಗಾತ್ರ_3) UPRV_ಖಂಡ_MACRO_BEGIN { \
    U_ASSERT(dst != ಖಾಲಿ); \
    U_ASSERT(src != ಖಾಲಿ); \
    U_STANDARD_CPP_NAMESPACE ಸ್ಮರಣೆ_ನಕಲು(dst, src, ಗಾತ್ರ_3); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_memmove(dst, src, ಗಾತ್ರ_3) UPRV_ಖಂಡ_MACRO_BEGIN { \
    U_ASSERT(dst != ಖಾಲಿ); \
    U_ASSERT(src != ಖಾಲಿ); \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, ಗಾತ್ರ_3); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ಆದರೆ_ಅಂತ್ಯ

/**
 * \def UPRV_LENGTHOF
 * Convenience macro to determine the length of a fixed array at compile-time.
 * @param array A fixed length array
 * @return The length of the array, in elements
 * @internal
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UPRV_LENGTHOF(array) (int32_t)(ಗಾತ್ರ(array)/ಗಾತ್ರ((array)[0]))
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_memset(ಬಫರ್, mark, ಗಾತ್ರ_3) U_STANDARD_CPP_NAMESPACE memset(ಬಫರ್, mark, ಗಾತ್ರ_3)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_memcmp(buffer1, buffer2, ಗಾತ್ರ_3) U_STANDARD_CPP_NAMESPACE memcmp(buffer1, buffer2,ಗಾತ್ರ_3)
#ವ್ಯಾಖ್ಯಾನಿಸು uprv_memchr(ptr, ಮೌಲ್ಯ_2, ಅಂಕೆ) U_STANDARD_CPP_NAMESPACE memchr(ptr, ಮೌಲ್ಯ_2, ಅಂಕೆ)

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_malloc(size_t s) U_MALLOC_ATTR U_ALLOC_ಗಾತ್ರ_ATTR(1);

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_realloc(ಶೂನ್ಯ *mem, size_t ಗಾತ್ರ_3) U_ALLOC_ಗಾತ್ರ_ATTR(2);

U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_free(ಶೂನ್ಯ *mem);

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_calloc(size_t ಅಂಕೆ, size_t ಗಾತ್ರ_3) U_MALLOC_ATTR U_ALLOC_ಗಾತ್ರ_ATTR2(1,2);

/**
 * Get the least significant bits of a pointer (a memory address).
 * For example, with a mask of 3, the macro gets the 2 least significant bits,
 * which will be 0 if the pointer is 32-bit (4-byte) aligned.
 *
 * uintptr_t is the most appropriate integer type to cast to.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು U_ಸೂಚಕ_ಮುಖವಾಡ_LSB(ptr, ಮುಖವಾಡ) ((uintptr_t)(ptr) & (ಮುಖವಾಡ))

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
#ವ್ಯಾಖ್ಯಾನಿಸು STATIC_NEW(type) [] () { \
    alignas(type) ಸ್ಥಿರ ಅಕ್ಷರ storage[ಗಾತ್ರ(type)]; \
    ಹಿಂತಿರುಗಿ new(storage) type();} ()

/**
  *  Heap clean up function, called from u_cleanup()
  *    Clears any user heap functions from u_setMemoryFunctions()
  *    Does NOT deallocate any remaining allocated memory.
  */
U_CFUNC UBool 
cmemory_cleanup(ಶೂನ್ಯ);

/**
 * A function called by <TT>uhash_remove</TT>,
 * <TT>uhash_close</TT>, or <TT>uhash_put</TT> to delete
 * an existing key or value.
 * @param obj A key or value stored in a hashtable
 * @see uprv_deleteUObject
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ U_CALLCONV UObjectDeleter(ಶೂನ್ಯ* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_deleteUObject(ಶೂನ್ಯ *obj);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus

#ಅನ್ತರ್ಭಾವ <memory>
#ಅನ್ತರ್ಭಾವ <utility>
#ಅನ್ತರ್ಭಾವ <type_traits>
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

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
ಮಾದರಿ<typename T>
class Local_ಸ್ಮರಣೆ : ಸಾರ್ವಜನಿಕ Local_ಸೂಚಕ_ತಳ<T> {
ಸಾರ್ವಜನಿಕ:
    using Local_ಸೂಚಕ_ತಳ<T>::ಕ್ರಿಯಾಕಾರಕ*;
    using Local_ಸೂಚಕ_ತಳ<T>::ಕ್ರಿಯಾಕಾರಕ->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an array of T items that is adopted
     */
    explicit Local_ಸ್ಮರಣೆ(T *p=nullptr) : Local_ಸೂಚಕ_ತಳ<T>(p) {}
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     */
    Local_ಸ್ಮರಣೆ(Local_ಸ್ಮರಣೆ<T> &&src) noexcept : Local_ಸೂಚಕ_ತಳ<T>(src.ptr) {
        src.ptr=nullptr;
    }
    /**
     * Destructor deletes the memory it owns.
     */
    ~Local_ಸ್ಮರಣೆ() {
        uprv_free(Local_ಸೂಚಕ_ತಳ<T>::ptr);
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     */
    Local_ಸ್ಮರಣೆ<T>& ಕ್ರಿಯಾಕಾರಕ=(Local_ಸ್ಮರಣೆ<T>&& src) noexcept U_LIFETIME_BOUND {
        uprv_free(Local_ಸೂಚಕ_ತಳ<T>::ptr);
        Local_ಸೂಚಕ_ತಳ<T>::ptr=src.ptr;
        src.ptr=nullptr;
        ಹಿಂತಿರುಗಿ *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    ಶೂನ್ಯ ಅದಲುಬದಲು(Local_ಸ್ಮರಣೆ<T> &other) noexcept {
        T *ತಾತ್ಕಾಲಿಕ=Local_ಸೂಚಕ_ತಳ<T>::ptr;
        Local_ಸೂಚಕ_ತಳ<T>::ptr=other.ptr;
        other.ptr=ತಾತ್ಕಾಲಿಕ;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    ಮಿತ್ರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಅದಲುಬದಲು(Local_ಸ್ಮರಣೆ<T> &p1, Local_ಸ್ಮರಣೆ<T> &p2) noexcept {
        p1.ಅದಲುಬದಲು(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    ಶೂನ್ಯ adoptInstead(T *p) {
        uprv_free(Local_ಸೂಚಕ_ತಳ<T>::ptr);
        Local_ಸೂಚಕ_ತಳ<T>::ptr=p;
    }
    /**
     * Deletes the array it owns, allocates a new one and reset its bytes to 0.
     * Returns the new array pointer.
     * If the allocation fails, then the current array is unchanged and
     * this method returns nullptr.
     * @param newCapacity must be >0
     * @return the allocated array pointer, or nullptr if the allocation failed
     */
    ಸಾಲೊಳಗೆ T* allocateInsteadAndReset(int32_t new_ಸಾಮರ್ಥ್ಯ = 1) U_LIFETIME_BOUND;
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
    ಸಾಲೊಳಗೆ T* allocateInsteadAndCopy(int32_t new_ಸಾಮರ್ಥ್ಯ = 1, int32_t ಉದ್ದಳತೆ = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& ಕ್ರಿಯಾಕಾರಕ[](ptrdiff_t i) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ Local_ಸೂಚಕ_ತಳ<T>::ptr[i]; }
};

ಮಾದರಿ <typename T>
ಸಾಲೊಳಗೆ T* Local_ಸ್ಮರಣೆ<T>::allocateInsteadAndReset(int32_t new_ಸಾಮರ್ಥ್ಯ) U_LIFETIME_BOUND {
    ಆದರೆ(new_ಸಾಮರ್ಥ್ಯ>0) {
        T *p=(T *)uprv_malloc(new_ಸಾಮರ್ಥ್ಯ*ಗಾತ್ರ(T));
        ಆದರೆ(p!=nullptr) {
            uprv_memset(p, 0, new_ಸಾಮರ್ಥ್ಯ*ಗಾತ್ರ(T));
            uprv_free(Local_ಸೂಚಕ_ತಳ<T>::ptr);
            Local_ಸೂಚಕ_ತಳ<T>::ptr=p;
        }
        ಹಿಂತಿರುಗಿ p;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ nullptr;
    }
}

ಮಾದರಿ <typename T>
ಸಾಲೊಳಗೆ T* Local_ಸ್ಮರಣೆ<T>::allocateInsteadAndCopy(int32_t new_ಸಾಮರ್ಥ್ಯ,
                                                 int32_t ಉದ್ದಳತೆ) U_LIFETIME_BOUND {
    ಆದರೆ(new_ಸಾಮರ್ಥ್ಯ>0) {
        T *p=(T *)uprv_malloc(new_ಸಾಮರ್ಥ್ಯ*ಗಾತ್ರ(T));
        ಆದರೆ(p!=nullptr) {
            ಆದರೆ(ಉದ್ದಳತೆ>0) {
                ಆದರೆ(ಉದ್ದಳತೆ>new_ಸಾಮರ್ಥ್ಯ) {
                    ಉದ್ದಳತೆ=new_ಸಾಮರ್ಥ್ಯ;
                }
                uprv_memcpy(p, Local_ಸೂಚಕ_ತಳ<T>::ptr, (size_t)ಉದ್ದಳತೆ*ಗಾತ್ರ(T));
            }
            uprv_free(Local_ಸೂಚಕ_ತಳ<T>::ptr);
            Local_ಸೂಚಕ_ತಳ<T>::ptr=p;
        }
        ಹಿಂತಿರುಗಿ p;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ nullptr;
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
ಮಾದರಿ<typename T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>
class Maybe_ರಾಶಿ_Array {
ಸಾರ್ವಜನಿಕ:
    // No heap allocation. Use only on the stack.
    ಸ್ಥಿರ ಶೂನ್ಯ* U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new(size_t) noexcept = delete;
    ಸ್ಥಿರ ಶೂನ್ಯ* U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new[](size_t) noexcept = delete;
    ಸ್ಥಿರ ಶೂನ್ಯ* U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new(size_t, ಶೂನ್ಯ*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_ರಾಶಿ_Array() : ptr(ರಾಶಿ_Array), ಸಾಮರ್ಥ್ಯ(ರಾಶಿ_ಸಾಮರ್ಥ್ಯ), need_ಗಮ್ಯ_Release(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_ರಾಶಿ_Array(int32_t new_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2) : Maybe_ರಾಶಿ_Array() {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (ಸಾಮರ್ಥ್ಯ < new_ಸಾಮರ್ಥ್ಯ) {
            ಆದರೆ (resize(new_ಸಾಮರ್ಥ್ಯ) == nullptr) {
                ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            }
        }
    }
    /**
     * Destructor deletes the array (if owned).
     */
    ~Maybe_ರಾಶಿ_Array() { releaseArray(); }
    /**
     * Move constructor: transfers ownership or copies the stack array.
     */
    Maybe_ರಾಶಿ_Array(Maybe_ರಾಶಿ_Array<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_ರಾಶಿ_Array<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>&
    ಕ್ರಿಯಾಕಾರಕ=(Maybe_ರಾಶಿ_Array<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_ಸಾಮರ್ಥ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸಾಮರ್ಥ್ಯ; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* get_Array_ಎಲ್ಲೆ() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ getAlias() + ಸಾಮರ್ಥ್ಯ; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ಸ್ಥಿರಾಂಕ T& ಕ್ರಿಯಾಕಾರಕ[](ptrdiff_t i) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& ಕ್ರಿಯಾಕಾರಕ[](ptrdiff_t i) U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    ಶೂನ್ಯ aliasInstead(T *otherArray, int32_t other_ಸಾಮರ್ಥ್ಯ) {
        ಆದರೆ(otherArray!=nullptr && other_ಸಾಮರ್ಥ್ಯ>0) {
            releaseArray();
            ptr=otherArray;
            ಸಾಮರ್ಥ್ಯ=other_ಸಾಮರ್ಥ್ಯ;
            need_ಗಮ್ಯ_Release=false;
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
    ಸಾಲೊಳಗೆ T* resize(int32_t new_ಸಾಮರ್ಥ್ಯ, int32_t ಉದ್ದಳತೆ = 0) U_LIFETIME_BOUND;
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
    ಸಾಲೊಳಗೆ T *orphanOrClone(int32_t ಉದ್ದಳತೆ, int32_t &ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ);

ಸಂರಕ್ಷಿತ:
    // Resizes the array to the size of src, then copies the contents of src.
    ಶೂನ್ಯ copyFrom(ಸ್ಥಿರಾಂಕ Maybe_ರಾಶಿ_Array &src, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (this->resize(src.ಸಾಮರ್ಥ್ಯ, 0) == nullptr) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        uprv_memcpy(this->ptr, src.ptr, (size_t)ಸಾಮರ್ಥ್ಯ * ಗಾತ್ರ(T));
    }

ಖಾಸಗಿ:
    T *ptr;
    int32_t ಸಾಮರ್ಥ್ಯ;
    UBool need_ಗಮ್ಯ_Release;
    T ರಾಶಿ_Array[ರಾಶಿ_ಸಾಮರ್ಥ್ಯ];
    ಶೂನ್ಯ releaseArray() {
        ಆದರೆ(need_ಗಮ್ಯ_Release) {
            uprv_free(ptr);
        }
    }
    ಶೂನ್ಯ reset_ಗಮ್ಯ_ರಾಶಿ_Array() {
        ptr=ರಾಶಿ_Array;
        ಸಾಮರ್ಥ್ಯ=ರಾಶಿ_ಸಾಮರ್ಥ್ಯ;
        need_ಗಮ್ಯ_Release=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Maybe_ರಾಶಿ_Array & /*other*/) = delete;
    bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Maybe_ರಾಶಿ_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_ರಾಶಿ_Array(ಸ್ಥಿರಾಂಕ Maybe_ರಾಶಿ_Array & /*other*/) = delete;
    ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Maybe_ರಾಶಿ_Array & /*other*/) = delete;
};

ಮಾದರಿ<typename T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>
icu::Maybe_ರಾಶಿ_Array<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>::Maybe_ರಾಶಿ_Array(
        Maybe_ರಾಶಿ_Array <T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>&& src) noexcept
        : ptr(src.ptr), ಸಾಮರ್ಥ್ಯ(src.ಸಾಮರ್ಥ್ಯ), need_ಗಮ್ಯ_Release(src.need_ಗಮ್ಯ_Release) {
    ಆದರೆ (src.ptr == src.ರಾಶಿ_Array) {
        ptr = ರಾಶಿ_Array;
        uprv_memcpy(ರಾಶಿ_Array, src.ರಾಶಿ_Array, ಗಾತ್ರ(T) * src.ಸಾಮರ್ಥ್ಯ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        src.reset_ಗಮ್ಯ_ರಾಶಿ_Array();  // take ownership away from src
    }
}

ಮಾದರಿ <typename T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>
ಸಾಲೊಳಗೆ Maybe_ರಾಶಿ_Array<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>& Maybe_ರಾಶಿ_Array<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>::ಕ್ರಿಯಾಕಾರಕ=(
    Maybe_ರಾಶಿ_Array<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    ಸಾಮರ್ಥ್ಯ = src.ಸಾಮರ್ಥ್ಯ;
    need_ಗಮ್ಯ_Release = src.need_ಗಮ್ಯ_Release;
    ಆದರೆ (src.ptr == src.ರಾಶಿ_Array) {
        ptr = ರಾಶಿ_Array;
        uprv_memcpy(ರಾಶಿ_Array, src.ರಾಶಿ_Array, ಗಾತ್ರ(T) * src.ಸಾಮರ್ಥ್ಯ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ptr = src.ptr;
        src.reset_ಗಮ್ಯ_ರಾಶಿ_Array();  // take ownership away from src
    }
    ಹಿಂತಿರುಗಿ *this;
}

ಮಾದರಿ <typename T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>
ಸಾಲೊಳಗೆ T* Maybe_ರಾಶಿ_Array<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>::resize(int32_t new_ಸಾಮರ್ಥ್ಯ,
                                                    int32_t ಉದ್ದಳತೆ) U_LIFETIME_BOUND {
    ಆದರೆ(new_ಸಾಮರ್ಥ್ಯ>0) {
#ಆದರೆ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
        ::fprintf(::stderr, "MaybeStackArray (resize) alloc %d * %lu\n", new_ಸಾಮರ್ಥ್ಯ, ಗಾತ್ರ(T));
#ಆದರೆ_ಅಂತ್ಯ
        T *p=(T *)uprv_malloc(new_ಸಾಮರ್ಥ್ಯ*ಗಾತ್ರ(T));
        ಆದರೆ(p!=nullptr) {
            ಆದರೆ(ಉದ್ದಳತೆ>0) {
                ಆದರೆ(ಉದ್ದಳತೆ>ಸಾಮರ್ಥ್ಯ) {
                    ಉದ್ದಳತೆ=ಸಾಮರ್ಥ್ಯ;
                }
                ಆದರೆ(ಉದ್ದಳತೆ>new_ಸಾಮರ್ಥ್ಯ) {
                    ಉದ್ದಳತೆ=new_ಸಾಮರ್ಥ್ಯ;
                }
                uprv_memcpy(p, ptr, (size_t)ಉದ್ದಳತೆ*ಗಾತ್ರ(T));
            }
            releaseArray();
            ptr=p;
            ಸಾಮರ್ಥ್ಯ=new_ಸಾಮರ್ಥ್ಯ;
            need_ಗಮ್ಯ_Release=true;
        }
        ಹಿಂತಿರುಗಿ p;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ nullptr;
    }
}

ಮಾದರಿ<typename T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>
ಸಾಲೊಳಗೆ T *Maybe_ರಾಶಿ_Array<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>::orphanOrClone(int32_t ಉದ್ದಳತೆ, int32_t &ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ) {
    T *p;
    ಆದರೆ(need_ಗಮ್ಯ_Release) {
        p=ptr;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಉದ್ದಳತೆ<=0) {
        ಹಿಂತಿರುಗಿ nullptr;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(ಉದ್ದಳತೆ>ಸಾಮರ್ಥ್ಯ) {
            ಉದ್ದಳತೆ=ಸಾಮರ್ಥ್ಯ;
        }
        p=(T *)uprv_malloc(ಉದ್ದಳತೆ*ಗಾತ್ರ(T));
#ಆದರೆ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
      ::fprintf(::stderr,"MaybeStacArray (orphan) alloc %d * %lu\n", ಉದ್ದಳತೆ,ಗಾತ್ರ(T));
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ(p==nullptr) {
            ಹಿಂತಿರುಗಿ nullptr;
        }
        uprv_memcpy(p, ptr, (size_t)ಉದ್ದಳತೆ*ಗಾತ್ರ(T));
    }
    ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ=ಉದ್ದಳತೆ;
    reset_ಗಮ್ಯ_ರಾಶಿ_Array();
    ಹಿಂತಿರುಗಿ p;
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
ಮಾದರಿ<typename H, typename T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>
class Maybe_ರಾಶಿ_ಶೀರ್ಷಿಕೆ_And_Array {
ಸಾರ್ವಜನಿಕ:
    // No heap allocation. Use only on the stack.
    ಸ್ಥಿರ ಶೂನ್ಯ* U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new(size_t) noexcept = delete;
    ಸ್ಥಿರ ಶೂನ್ಯ* U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new[](size_t) noexcept = delete;
    ಸ್ಥಿರ ಶೂನ್ಯ* U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new(size_t, ಶೂನ್ಯ*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_ರಾಶಿ_ಶೀರ್ಷಿಕೆ_And_Array() : ptr(&ರಾಶಿ_ಶೀರ್ಷಿಕೆ), ಸಾಮರ್ಥ್ಯ(ರಾಶಿ_ಸಾಮರ್ಥ್ಯ), need_ಗಮ್ಯ_Release(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_ರಾಶಿ_ಶೀರ್ಷಿಕೆ_And_Array() { release_ಸ್ಮರಣೆ(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_ಸಾಮರ್ಥ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸಾಮರ್ಥ್ಯ; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* get_Array_ಆರಂಭ() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* get_Array_ಎಲ್ಲೆ() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ get_Array_ಆರಂಭ() + ಸಾಮರ್ಥ್ಯ; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    ಕ್ರಿಯಾಕಾರಕ H*() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& ಕ್ರಿಯಾಕಾರಕ[](ptrdiff_t i) U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ get_Array_ಆರಂಭ()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    ಶೂನ್ಯ aliasInstead(H *other_ಸ್ಮರಣೆ, int32_t other_ಸಾಮರ್ಥ್ಯ) {
        ಆದರೆ(other_ಸ್ಮರಣೆ!=nullptr && other_ಸಾಮರ್ಥ್ಯ>0) {
            release_ಸ್ಮರಣೆ();
            ptr=other_ಸ್ಮರಣೆ;
            ಸಾಮರ್ಥ್ಯ=other_ಸಾಮರ್ಥ್ಯ;
            need_ಗಮ್ಯ_Release=false;
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
    ಸಾಲೊಳಗೆ H* resize(int32_t new_ಸಾಮರ್ಥ್ಯ, int32_t ಉದ್ದಳತೆ = 0) U_LIFETIME_BOUND;
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
    ಸಾಲೊಳಗೆ H *orphanOrClone(int32_t ಉದ್ದಳತೆ, int32_t &ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ);
ಖಾಸಗಿ:
    H *ptr;
    int32_t ಸಾಮರ್ಥ್ಯ;
    UBool need_ಗಮ್ಯ_Release;
    // stackHeader must precede stackArray immediately.
    H ರಾಶಿ_ಶೀರ್ಷಿಕೆ;
    T ರಾಶಿ_Array[ರಾಶಿ_ಸಾಮರ್ಥ್ಯ];
    ಶೂನ್ಯ release_ಸ್ಮರಣೆ() {
        ಆದರೆ(need_ಗಮ್ಯ_Release) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Maybe_ರಾಶಿ_ಶೀರ್ಷಿಕೆ_And_Array & /*other*/) {ಹಿಂತಿರುಗಿ false;}
    bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Maybe_ರಾಶಿ_ಶೀರ್ಷಿಕೆ_And_Array & /*other*/) {ಹಿಂತಿರುಗಿ true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_ರಾಶಿ_ಶೀರ್ಷಿಕೆ_And_Array(ಸ್ಥಿರಾಂಕ Maybe_ರಾಶಿ_ಶೀರ್ಷಿಕೆ_And_Array & /*other*/) {}
    ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Maybe_ರಾಶಿ_ಶೀರ್ಷಿಕೆ_And_Array & /*other*/) {}
};

ಮಾದರಿ <typename H, typename T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>
ಸಾಲೊಳಗೆ H* Maybe_ರಾಶಿ_ಶೀರ್ಷಿಕೆ_And_Array<H, T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>::resize(int32_t new_ಸಾಮರ್ಥ್ಯ,
                                                                int32_t ಉದ್ದಳತೆ) U_LIFETIME_BOUND {
    ಆದರೆ(new_ಸಾಮರ್ಥ್ಯ>=0) {
#ಆದರೆ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", ಗಾತ್ರ(H),new_ಸಾಮರ್ಥ್ಯ,ಗಾತ್ರ(T));
#ಆದರೆ_ಅಂತ್ಯ
        H *p=(H *)uprv_malloc(ಗಾತ್ರ(H)+new_ಸಾಮರ್ಥ್ಯ*ಗಾತ್ರ(T));
        ಆದರೆ(p!=nullptr) {
            ಆದರೆ(ಉದ್ದಳತೆ<0) {
                ಉದ್ದಳತೆ=0;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಉದ್ದಳತೆ>0) {
                ಆದರೆ(ಉದ್ದಳತೆ>ಸಾಮರ್ಥ್ಯ) {
                    ಉದ್ದಳತೆ=ಸಾಮರ್ಥ್ಯ;
                }
                ಆದರೆ(ಉದ್ದಳತೆ>new_ಸಾಮರ್ಥ್ಯ) {
                    ಉದ್ದಳತೆ=new_ಸಾಮರ್ಥ್ಯ;
                }
            }
            uprv_memcpy(p, ptr, ಗಾತ್ರ(H)+(size_t)ಉದ್ದಳತೆ*ಗಾತ್ರ(T));
            release_ಸ್ಮರಣೆ();
            ptr=p;
            ಸಾಮರ್ಥ್ಯ=new_ಸಾಮರ್ಥ್ಯ;
            need_ಗಮ್ಯ_Release=true;
        }
        ಹಿಂತಿರುಗಿ p;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ nullptr;
    }
}

ಮಾದರಿ<typename H, typename T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>
ಸಾಲೊಳಗೆ H *Maybe_ರಾಶಿ_ಶೀರ್ಷಿಕೆ_And_Array<H, T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>::orphanOrClone(int32_t ಉದ್ದಳತೆ,
                                                                       int32_t &ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ) {
    H *p;
    ಆದರೆ(need_ಗಮ್ಯ_Release) {
        p=ptr;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(ಉದ್ದಳತೆ<0) {
            ಉದ್ದಳತೆ=0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಉದ್ದಳತೆ>ಸಾಮರ್ಥ್ಯ) {
            ಉದ್ದಳತೆ=ಸಾಮರ್ಥ್ಯ;
        }
#ಆದರೆ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", ಗಾತ್ರ(H),ಉದ್ದಳತೆ,ಗಾತ್ರ(T));
#ಆದರೆ_ಅಂತ್ಯ
        p=(H *)uprv_malloc(ಗಾತ್ರ(H)+ಉದ್ದಳತೆ*ಗಾತ್ರ(T));
        ಆದರೆ(p==nullptr) {
            ಹಿಂತಿರುಗಿ nullptr;
        }
        uprv_memcpy(p, ptr, ಗಾತ್ರ(H)+(size_t)ಉದ್ದಳತೆ*ಗಾತ್ರ(T));
    }
    ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ=ಉದ್ದಳತೆ;
    ptr=&ರಾಶಿ_ಶೀರ್ಷಿಕೆ;
    ಸಾಮರ್ಥ್ಯ=ರಾಶಿ_ಸಾಮರ್ಥ್ಯ;
    need_ಗಮ್ಯ_Release=false;
    ಹಿಂತಿರುಗಿ p;
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
ಮಾದರಿ<typename T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ = 8>
class ಸ್ಮರಣೆ_Pool : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಮರಣೆ_Pool() : f_ಎಣಿಕೆ(0), fPool() {}

    ~ಸ್ಮರಣೆ_Pool() {
        ಗಾಗಿ (int32_t i = 0; i < f_ಎಣಿಕೆ; ++i) {
            delete fPool[i];
        }
    }

    ಸ್ಮರಣೆ_Pool(ಸ್ಥಿರಾಂಕ ಸ್ಮರಣೆ_Pool&) = delete;
    ಸ್ಮರಣೆ_Pool& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಸ್ಮರಣೆ_Pool&) = delete;

    ಸ್ಮರಣೆ_Pool(ಸ್ಮರಣೆ_Pool&& other) noexcept : f_ಎಣಿಕೆ(other.f_ಎಣಿಕೆ),
                                                fPool(std::move(other.fPool)) {
        other.f_ಎಣಿಕೆ = 0;
    }

    ಸ್ಮರಣೆ_Pool& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಮರಣೆ_Pool&& other) noexcept U_LIFETIME_BOUND {
        // Since `this` may contain instances that need to be deleted, we can't
        // just throw them away and replace them with `other`. The normal way of
        // dealing with this in C++ is to swap `this` and `other`, rather than
        // simply overwrite: the destruction of `other` can then take care of
        // running MemoryPool::~MemoryPool() over the still-to-be-deallocated
        // instances.
        std::ಅದಲುಬದಲು(f_ಎಣಿಕೆ, other.f_ಎಣಿಕೆ);
        std::ಅದಲುಬದಲು(fPool, other.fPool);
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * Creates a new object of typename T, by forwarding any and all arguments
     * to the typename T constructor.
     *
     * @param args Arguments to be forwarded to the typename T constructor.
     * @return A pointer to the newly created object, or nullptr on error.
     */
    ಮಾದರಿ <typename... ವಾದಗಳು>
    T* create(ವಾದಗಳು&&... ವಾದಗಳು_2) U_LIFETIME_BOUND {
        int32_t ಸಾಮರ್ಥ್ಯ = fPool.get_ಸಾಮರ್ಥ್ಯ();
        ಆದರೆ (f_ಎಣಿಕೆ == ಸಾಮರ್ಥ್ಯ &&
            fPool.resize(ಸಾಮರ್ಥ್ಯ == ರಾಶಿ_ಸಾಮರ್ಥ್ಯ ? 4 * ಸಾಮರ್ಥ್ಯ : 2 * ಸಾಮರ್ಥ್ಯ,
                         ಸಾಮರ್ಥ್ಯ) == nullptr) {
            ಹಿಂತಿರುಗಿ nullptr;
        }
        ಹಿಂತಿರುಗಿ fPool[f_ಎಣಿಕೆ++] = new T(std::forward<ವಾದಗಳು>(ವಾದಗಳು_2)...);
    }

    ಮಾದರಿ <typename... ವಾದಗಳು>
    T* create_And_Check_Error_ಸಂಕೇತ(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2, ವಾದಗಳು&&... ವಾದಗಳು_2) U_LIFETIME_BOUND {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ nullptr;
        }
        T *ಸೂಚಕ = this->create(ವಾದಗಳು_2...);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && ಸೂಚಕ == nullptr) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
        ಹಿಂತಿರುಗಿ ಸೂಚಕ;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t ಎಣಿಕೆ_2() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ f_ಎಣಿಕೆ;
    }

ಸಂರಕ್ಷಿತ:
    int32_t f_ಎಣಿಕೆ;
    Maybe_ರಾಶಿ_Array<T*, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ> fPool;
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
ಮಾದರಿ<typename T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ = 8>
class Maybe_ರಾಶಿ_ಸದಿಶ : ಸಂರಕ್ಷಿತ ಸ್ಮರಣೆ_Pool<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ> {
ಸಾರ್ವಜನಿಕ:
    ಮಾದರಿ <typename... ವಾದಗಳು>
    T* emplace_ಹಿಂಭಾಗ(ವಾದಗಳು&&... ವಾದಗಳು_2) U_LIFETIME_BOUND {
        ಹಿಂತಿರುಗಿ this->create(ವಾದಗಳು_2...);
    }

    ಮಾದರಿ <typename... ವಾದಗಳು>
    T* emplace_ಹಿಂಭಾಗ_And_Check_Error_ಸಂಕೇತ(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2, ವಾದಗಳು&&... ವಾದಗಳು_2) U_LIFETIME_BOUND {
        ಹಿಂತಿರುಗಿ this->create_And_Check_Error_ಸಂಕೇತ(ಸ್ಥಿತಿ_2, ವಾದಗಳು_2...);
    }

    int32_t ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ this->f_ಎಣಿಕೆ;
    }

    T** getAlias() U_LIFETIME_BOUND {
        ಹಿಂತಿರುಗಿ this->fPool.getAlias();
    }

    ಸ್ಥಿರಾಂಕ T* ಸ್ಥಿರಾಂಕ* getAlias() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
        ಹಿಂತಿರುಗಿ this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ಸ್ಥಿರಾಂಕ T* ಕ್ರಿಯಾಕಾರಕ[](ptrdiff_t i) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
        ಹಿಂತಿರುಗಿ this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* ಕ್ರಿಯಾಕಾರಕ[](ptrdiff_t i) U_LIFETIME_BOUND {
        ಹಿಂತಿರುಗಿ this->fPool[i];
    }
};

namespace prv {

#ಆದರೆ U_OVERRIDE_CXX_ಹಂಚಿಕೆ

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
ಮಾದರಿ <typename T, ಶೂನ್ಯ deallocate(ಶೂನ್ಯ *), typename = ಶೂನ್ಯ> class Deleter {
  ಸಾರ್ವಜನಿಕ:
    Deleter() = ಪೂರ್ವನಿಯೋಜಿತ;
    ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ()(T *ಸ್ಥಿರಾಂಕ ptr) ಸ್ಥಿರಾಂಕ {
        ಆದರೆ constexpr (!std::is_trivially_destructible_v<T>) {
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
ಮಾದರಿ <typename T, ಶೂನ್ಯ deallocate(ಶೂನ್ಯ *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  ಸಾರ್ವಜನಿಕ:
    Deleter() = ಪೂರ್ವನಿಯೋಜಿತ;
    ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ()(T *ಸ್ಥಿರಾಂಕ ptr) ಸ್ಥಿರಾಂಕ { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
ಮಾದರಿ <typename T, ಶೂನ್ಯ deallocate(ಶೂನ್ಯ *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  ಸಾರ್ವಜನಿಕ:
    Deleter(ಸ್ಥಿರಾಂಕ std::size_t ಗಾತ್ರ_3) : ಗಾತ್ರ_2(ಗಾತ್ರ_3) {}
    ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ()(T *ಸ್ಥಿರಾಂಕ ptr) ಸ್ಥಿರಾಂಕ {
        ಗಾಗಿ (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(ಗಾತ್ರ_2) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ std::size_t ಗಾತ್ರ_2;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
ಮಾದರಿ <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#ಇಲ್ಲದಿದ್ದರೆ

using std::unique_ptr;

#ಆದರೆ_ಅಂತ್ಯ

/**
 * Helper for make_unique<T, Args...>; forwards all but the last of args to the constructor of
 * T, uses the last of args as the error code for allocation errors.
 *
 * As usual, does nothing if the incoming UErrorCode already is U_FAILURE.
 *
 * @internal
 */
ಮಾದರಿ <typename T, typename... ವಾದಗಳು, std::size_t... i>
unique_ptr<T> make_unique_impl(std::tuple<ವಾದಗಳು...> ವಾದಗಳು_2,
                               std::ಸೂಚ್ಯಂಕ_sequence<i...> constructor_arg_indices) {
    static_assert(std::is_same_v<decltype(constructor_arg_indices),
                                 std::make_ಸೂಚ್ಯಂಕ_sequence<ಗಾತ್ರ...(ವಾದಗಳು) - 1>>);
    ಸ್ವಯಂಚಾಲಿತ &&error = std::get<ಗಾತ್ರ...(ವಾದಗಳು) - 1>(ವಾದಗಳು_2);
    static_assert(std::is_same_v<decltype(error), UError_ಸಂಕೇತ &>);
    ಆದರೆ (U_FAILURE(error)) {
        ಹಿಂತಿರುಗಿ nullptr;
    }
#ಆದರೆ U_OVERRIDE_CXX_ಹಂಚಿಕೆ
    ಶೂನ್ಯ *ಸ್ಥಿರಾಂಕ ಬಫರ್ = uprv_malloc(ಗಾತ್ರ(T));
    ಆದರೆ (ಬಫರ್ == nullptr) {
        error = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ nullptr;
    }
    ಹಿಂತಿರುಗಿ unique_ptr<T>(new (ಬಫರ್) T(std::get<i>(ವಾದಗಳು_2)...));
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ std::make_unique<T>(std::get<i>(ವಾದಗಳು_2)...);
#ಆದರೆ_ಅಂತ್ಯ
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
ಮಾದರಿ <typename T, typename... ವಾದಗಳು, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique(ವಾದಗಳು &&...ವಾದಗಳು_2) {
    ಹಿಂತಿರುಗಿ make_unique_impl<T>(std::forward_as_tuple(ವಾದಗಳು_2...),
                               std::make_ಸೂಚ್ಯಂಕ_sequence<ಗಾತ್ರ...(ವಾದಗಳು) - 1>());
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
ಮಾದರಿ <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique(ಸ್ಥಿರಾಂಕ std::size_t n, UError_ಸಂಕೇತ &error) {
    ಆದರೆ (U_FAILURE(error)) {
        ಹಿಂತಿರುಗಿ nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_ಗಾತ್ರ = SIZE_MAX / ಗಾತ್ರ(std::remove_extent_t<T>);
    ಆದರೆ (n > max_ಗಾತ್ರ) {
        error = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ nullptr;
    }
#ಆದರೆ U_OVERRIDE_CXX_ಹಂಚಿಕೆ
    ಶೂನ್ಯ *ಸ್ಥಿರಾಂಕ ಬಫರ್ = uprv_malloc(ಗಾತ್ರ(std::remove_extent_t<T>) * n);
    ಆದರೆ (ಬಫರ್ == nullptr) {
        error = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಆದರೆ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            ಹಿಂತಿರುಗಿ nullptr;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    ಆದರೆ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        ಹಿಂತಿರುಗಿ unique_ptr<T>(new (ಬಫರ್) std::remove_extent_t<T>[n]());
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ unique_ptr<T>(new (ಬಫರ್) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ std::make_unique<T>(n);
#ಆದರೆ_ಅಂತ್ಯ
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
ಮಾದರಿ <typename T, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique_for_overwrite(UError_ಸಂಕೇತ &error) {
    ಆದರೆ (U_FAILURE(error)) {
        ಹಿಂತಿರುಗಿ nullptr;
    }
#ಆದರೆ U_OVERRIDE_CXX_ಹಂಚಿಕೆ
    ಶೂನ್ಯ *ಸ್ಥಿರಾಂಕ ಬಫರ್ = uprv_malloc(ಗಾತ್ರ(T));
    ಆದರೆ (ಬಫರ್ == nullptr) {
        error = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ nullptr;
    }
    ಹಿಂತಿರುಗಿ unique_ptr<T>(new (ಬಫರ್) T);
#ಅಥವಾಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ >= 20
    ಹಿಂತಿರುಗಿ std::make_unique_for_overwrite<T>();
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ unique_ptr<T>(new T);
#ಆದರೆ_ಅಂತ್ಯ
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
ಮಾದರಿ <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique_for_overwrite(ಸ್ಥಿರಾಂಕ std::size_t n, UError_ಸಂಕೇತ &error) {
    ಆದರೆ (U_FAILURE(error)) {
        ಹಿಂತಿರುಗಿ nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_ಗಾತ್ರ = SIZE_MAX / ಗಾತ್ರ(std::remove_extent_t<T>);
    ಆದರೆ (n > max_ಗಾತ್ರ) {
        error = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ nullptr;
    }
#ಆದರೆ U_OVERRIDE_CXX_ಹಂಚಿಕೆ
    ಶೂನ್ಯ *ಸ್ಥಿರಾಂಕ ಬಫರ್ = uprv_malloc(ಗಾತ್ರ(std::remove_extent_t<T>) * n);
    ಆದರೆ (ಬಫರ್ == nullptr) {
        error = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಆದರೆ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            ಹಿಂತಿರುಗಿ nullptr;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    ಆದರೆ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        ಹಿಂತಿರುಗಿ unique_ptr<T>(new (ಬಫರ್) std::remove_extent_t<T>[n]);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ unique_ptr<T>(new (ಬಫರ್) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#ಅಥವಾಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ >= 20
    ಹಿಂತಿರುಗಿ std::make_unique_for_overwrite<T>(n);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ unique_ptr<T>(new std::remove_extent_t<T>[n]);
#ಆದರೆ_ಅಂತ್ಯ
}

}  // namespace prv

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  /* __cplusplus */
#ಆದರೆ_ಅಂತ್ಯ  /* CMEMORY_H */
