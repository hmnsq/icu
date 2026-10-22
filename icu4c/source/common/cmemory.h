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

#यदि_अपरिभाषितम् CMEMORY_H
#परिभाषय CMEMORY_H

#अन्तर्भाव "unicode/utypes.h"

#अन्तर्भाव <limits.h>
#अन्तर्भाव <stddef.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "uassert.h"

#यदि U_DEBUG && defined(UPRV_MALLOC_गणनम्)
#अन्तर्भाव <stdio.h>
#यदि_अन्तः

// uprv_memcpy and uprv_memmove
#यदि defined(__clang__)
#परिभाषय uprv_memcpy(dst, src, आकार_2) UPRV_खण्डम्_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE स्मृतिं_प्रतिलिख(dst, src, आकार_2); \
} UPRV_खण्डम्_MACRO_अन्तः
#परिभाषय uprv_memmove(dst, src, आकार_2) UPRV_खण्डम्_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, आकार_2); \
} UPRV_खण्डम्_MACRO_अन्तः
#नोचेद्यदि defined(__GNUC__)
#परिभाषय uprv_memcpy(dst, src, आकार_2) UPRV_खण्डम्_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE स्मृतिं_प्रतिलिख(dst, src, आकार_2); \
} UPRV_खण्डम्_MACRO_अन्तः
#परिभाषय uprv_memmove(dst, src, आकार_2) UPRV_खण्डम्_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, आकार_2); \
} UPRV_खण्डम्_MACRO_अन्तः
#अन्यथा
#परिभाषय uprv_memcpy(dst, src, आकार_2) UPRV_खण्डम्_MACRO_BEGIN { \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    U_STANDARD_CPP_NAMESPACE स्मृतिं_प्रतिलिख(dst, src, आकार_2); \
} UPRV_खण्डम्_MACRO_अन्तः
#परिभाषय uprv_memmove(dst, src, आकार_2) UPRV_खण्डम्_MACRO_BEGIN { \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, आकार_2); \
} UPRV_खण्डम्_MACRO_अन्तः
#यदि_अन्तः

/**
 * \def UPRV_LENGTHOF
 * Convenience macro to determine the length of a fixed array at compile-time.
 * @param array A fixed length array
 * @return The length of the array, in elements
 * @internal
 */
#परिभाषय UPRV_LENGTHOF(array) (int32_t)(परिमाण(array)/परिमाण((array)[0]))
#परिभाषय uprv_memset(बफर, mark, आकार_2) U_STANDARD_CPP_NAMESPACE स्मृतिं_पूरय(बफर, mark, आकार_2)
#परिभाषय uprv_memcmp(buffer1, buffer2, आकार_2) U_STANDARD_CPP_NAMESPACE memcmp(buffer1, buffer2,आकार_2)
#परिभाषय uprv_memchr(ptr, मानम्_2, अङ्कः) U_STANDARD_CPP_NAMESPACE memchr(ptr, मानम्_2, अङ्कः)

U_CAPI शून्य * U_EXPORT2
uprv_malloc(size_t s) U_MALLOC_ATTR U_ALLOC_आकार_ATTR(1);

U_CAPI शून्य * U_EXPORT2
uprv_realloc(शून्य *mem, size_t आकार_2) U_ALLOC_आकार_ATTR(2);

U_CAPI शून्य U_EXPORT2
uprv_free(शून्य *mem);

U_CAPI शून्य * U_EXPORT2
uprv_calloc(size_t अङ्कः, size_t आकार_2) U_MALLOC_ATTR U_ALLOC_आकार_ATTR2(1,2);

/**
 * Get the least significant bits of a pointer (a memory address).
 * For example, with a mask of 3, the macro gets the 2 least significant bits,
 * which will be 0 if the pointer is 32-bit (4-byte) aligned.
 *
 * uintptr_t is the most appropriate integer type to cast to.
 */
#परिभाषय U_POINTER_आवरणम्_LSB(ptr, आवरणम्) ((uintptr_t)(ptr) & (आवरणम्))

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
#परिभाषय STATIC_NEW(type) [] () { \
    alignas(type) स्थिर वर्ण storage[परिमाण(type)]; \
    प्रत्यागम new(storage) type();} ()

/**
  *  Heap clean up function, called from u_cleanup()
  *    Clears any user heap functions from u_setMemoryFunctions()
  *    Does NOT deallocate any remaining allocated memory.
  */
U_CFUNC UBool 
cmemory_cleanup(शून्य);

/**
 * A function called by <TT>uhash_remove</TT>,
 * <TT>uhash_close</TT>, or <TT>uhash_put</TT> to delete
 * an existing key or value.
 * @param obj A key or value stored in a hashtable
 * @see uprv_deleteUObject
 */
प्रकारनिर्देश शून्य U_CALLCONV UObjectDeleter(शून्य* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI शून्य U_EXPORT2
uprv_deleteUObject(शून्य *obj);

#यदि_परिभाषितम् __cplusplus

#अन्तर्भाव <memory>
#अन्तर्भाव <utility>
#अन्तर्भाव <type_traits>
#अन्तर्भाव "unicode/uobject.h"

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
प्रतिरूपम्<typename T>
class Local_स्मृतिः : सार्वजनिक Local_Pointer_आधारः<T> {
सार्वजनिक:
    using Local_Pointer_आधारः<T>::संक्रियकः*;
    using Local_Pointer_आधारः<T>::संक्रियकः->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an array of T items that is adopted
     */
    explicit Local_स्मृतिः(T *p=nullptr) : Local_Pointer_आधारः<T>(p) {}
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     */
    Local_स्मृतिः(Local_स्मृतिः<T> &&src) noexcept : Local_Pointer_आधारः<T>(src.ptr) {
        src.ptr=nullptr;
    }
    /**
     * Destructor deletes the memory it owns.
     */
    ~Local_स्मृतिः() {
        uprv_free(Local_Pointer_आधारः<T>::ptr);
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     */
    Local_स्मृतिः<T>& संक्रियकः=(Local_स्मृतिः<T>&& src) noexcept U_LIFETIME_BOUND {
        uprv_free(Local_Pointer_आधारः<T>::ptr);
        Local_Pointer_आधारः<T>::ptr=src.ptr;
        src.ptr=nullptr;
        प्रत्यागम *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    शून्य swap(Local_स्मृतिः<T> &other) noexcept {
        T *अस्थायी=Local_Pointer_आधारः<T>::ptr;
        Local_Pointer_आधारः<T>::ptr=other.ptr;
        other.ptr=अस्थायी;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    मित्रम् अन्तःस्थ शून्य swap(Local_स्मृतिः<T> &p1, Local_स्मृतिः<T> &p2) noexcept {
        p1.swap(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    शून्य adoptInstead(T *p) {
        uprv_free(Local_Pointer_आधारः<T>::ptr);
        Local_Pointer_आधारः<T>::ptr=p;
    }
    /**
     * Deletes the array it owns, allocates a new one and reset its bytes to 0.
     * Returns the new array pointer.
     * If the allocation fails, then the current array is unchanged and
     * this method returns nullptr.
     * @param newCapacity must be >0
     * @return the allocated array pointer, or nullptr if the allocation failed
     */
    अन्तःस्थ T* allocateInsteadAndReset(int32_t new_क्षमता = 1) U_LIFETIME_BOUND;
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
    अन्तःस्थ T* allocateInsteadAndCopy(int32_t new_क्षमता = 1, int32_t दीर्घता = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& संक्रियकः[](ptrdiff_t i) ध्रुव U_LIFETIME_BOUND { प्रत्यागम Local_Pointer_आधारः<T>::ptr[i]; }
};

प्रतिरूपम् <typename T>
अन्तःस्थ T* Local_स्मृतिः<T>::allocateInsteadAndReset(int32_t new_क्षमता) U_LIFETIME_BOUND {
    यदि(new_क्षमता>0) {
        T *p=(T *)uprv_malloc(new_क्षमता*परिमाण(T));
        यदि(p!=nullptr) {
            uprv_memset(p, 0, new_क्षमता*परिमाण(T));
            uprv_free(Local_Pointer_आधारः<T>::ptr);
            Local_Pointer_आधारः<T>::ptr=p;
        }
        प्रत्यागम p;
    } अन्यथा {
        प्रत्यागम nullptr;
    }
}

प्रतिरूपम् <typename T>
अन्तःस्थ T* Local_स्मृतिः<T>::allocateInsteadAndCopy(int32_t new_क्षमता,
                                                 int32_t दीर्घता) U_LIFETIME_BOUND {
    यदि(new_क्षमता>0) {
        T *p=(T *)uprv_malloc(new_क्षमता*परिमाण(T));
        यदि(p!=nullptr) {
            यदि(दीर्घता>0) {
                यदि(दीर्घता>new_क्षमता) {
                    दीर्घता=new_क्षमता;
                }
                uprv_memcpy(p, Local_Pointer_आधारः<T>::ptr, (size_t)दीर्घता*परिमाण(T));
            }
            uprv_free(Local_Pointer_आधारः<T>::ptr);
            Local_Pointer_आधारः<T>::ptr=p;
        }
        प्रत्यागम p;
    } अन्यथा {
        प्रत्यागम nullptr;
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
प्रतिरूपम्<typename T, int32_t राशिः_क्षमता>
class Maybe_राशिः_Array {
सार्वजनिक:
    // No heap allocation. Use only on the stack.
    स्थिर शून्य* U_EXPORT2 संक्रियकः new(size_t) noexcept = delete;
    स्थिर शून्य* U_EXPORT2 संक्रियकः new[](size_t) noexcept = delete;
    स्थिर शून्य* U_EXPORT2 संक्रियकः new(size_t, शून्य*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_राशिः_Array() : ptr(राशिः_Array), क्षमता(राशिः_क्षमता), need_गन्तव्यम्_Release(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_राशिः_Array(int32_t new_क्षमता, UError_संहिता स्थितिः) : Maybe_राशिः_Array() {
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम;
        }
        यदि (क्षमता < new_क्षमता) {
            यदि (resize(new_क्षमता) == nullptr) {
                स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            }
        }
    }
    /**
     * Destructor deletes the array (if owned).
     */
    ~Maybe_राशिः_Array() { releaseArray(); }
    /**
     * Move constructor: transfers ownership or copies the stack array.
     */
    Maybe_राशिः_Array(Maybe_राशिः_Array<T, राशिः_क्षमता> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_राशिः_Array<T, राशिः_क्षमता>&
    संक्रियकः=(Maybe_राशिः_Array<T, राशिः_क्षमता>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_क्षमता() ध्रुव { प्रत्यागम क्षमता; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() ध्रुव U_LIFETIME_BOUND { प्रत्यागम ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* get_Array_सीमा() ध्रुव U_LIFETIME_BOUND { प्रत्यागम getAlias() + क्षमता; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ध्रुव T& संक्रियकः[](ptrdiff_t i) ध्रुव U_LIFETIME_BOUND { प्रत्यागम ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& संक्रियकः[](ptrdiff_t i) U_LIFETIME_BOUND { प्रत्यागम ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    शून्य aliasInstead(T *otherArray, int32_t other_क्षमता) {
        यदि(otherArray!=nullptr && other_क्षमता>0) {
            releaseArray();
            ptr=otherArray;
            क्षमता=other_क्षमता;
            need_गन्तव्यम्_Release=false;
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
    अन्तःस्थ T* resize(int32_t new_क्षमता, int32_t दीर्घता = 0) U_LIFETIME_BOUND;
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
    अन्तःस्थ T *orphanOrClone(int32_t दीर्घता, int32_t &परिणाम_क्षमता);

रक्षित:
    // Resizes the array to the size of src, then copies the contents of src.
    शून्य copyFrom(ध्रुव Maybe_राशिः_Array &src, UError_संहिता &स्थितिः) {
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम;
        }
        यदि (this->resize(src.क्षमता, 0) == nullptr) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
        uprv_memcpy(this->ptr, src.ptr, (size_t)क्षमता * परिमाण(T));
    }

गुप्त:
    T *ptr;
    int32_t क्षमता;
    UBool need_गन्तव्यम्_Release;
    T राशिः_Array[राशिः_क्षमता];
    शून्य releaseArray() {
        यदि(need_गन्तव्यम्_Release) {
            uprv_free(ptr);
        }
    }
    शून्य reset_गन्तव्यम्_राशिः_Array() {
        ptr=राशिः_Array;
        क्षमता=राशिः_क्षमता;
        need_गन्तव्यम्_Release=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool संक्रियकः==(ध्रुव Maybe_राशिः_Array & /*other*/) = delete;
    bool संक्रियकः!=(ध्रुव Maybe_राशिः_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_राशिः_Array(ध्रुव Maybe_राशिः_Array & /*other*/) = delete;
    शून्य संक्रियकः=(ध्रुव Maybe_राशिः_Array & /*other*/) = delete;
};

प्रतिरूपम्<typename T, int32_t राशिः_क्षमता>
icu::Maybe_राशिः_Array<T, राशिः_क्षमता>::Maybe_राशिः_Array(
        Maybe_राशिः_Array <T, राशिः_क्षमता>&& src) noexcept
        : ptr(src.ptr), क्षमता(src.क्षमता), need_गन्तव्यम्_Release(src.need_गन्तव्यम्_Release) {
    यदि (src.ptr == src.राशिः_Array) {
        ptr = राशिः_Array;
        uprv_memcpy(राशिः_Array, src.राशिः_Array, परिमाण(T) * src.क्षमता);
    } अन्यथा {
        src.reset_गन्तव्यम्_राशिः_Array();  // take ownership away from src
    }
}

प्रतिरूपम् <typename T, int32_t राशिः_क्षमता>
अन्तःस्थ Maybe_राशिः_Array<T, राशिः_क्षमता>& Maybe_राशिः_Array<T, राशिः_क्षमता>::संक्रियकः=(
    Maybe_राशिः_Array<T, राशिः_क्षमता>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    क्षमता = src.क्षमता;
    need_गन्तव्यम्_Release = src.need_गन्तव्यम्_Release;
    यदि (src.ptr == src.राशिः_Array) {
        ptr = राशिः_Array;
        uprv_memcpy(राशिः_Array, src.राशिः_Array, परिमाण(T) * src.क्षमता);
    } अन्यथा {
        ptr = src.ptr;
        src.reset_गन्तव्यम्_राशिः_Array();  // take ownership away from src
    }
    प्रत्यागम *this;
}

प्रतिरूपम् <typename T, int32_t राशिः_क्षमता>
अन्तःस्थ T* Maybe_राशिः_Array<T, राशिः_क्षमता>::resize(int32_t new_क्षमता,
                                                    int32_t दीर्घता) U_LIFETIME_BOUND {
    यदि(new_क्षमता>0) {
#यदि U_DEBUG && defined(UPRV_MALLOC_गणनम्)
        ::fprintf(::stderr, "MaybeStackArray (resize) alloc %d * %lu\n", new_क्षमता, परिमाण(T));
#यदि_अन्तः
        T *p=(T *)uprv_malloc(new_क्षमता*परिमाण(T));
        यदि(p!=nullptr) {
            यदि(दीर्घता>0) {
                यदि(दीर्घता>क्षमता) {
                    दीर्घता=क्षमता;
                }
                यदि(दीर्घता>new_क्षमता) {
                    दीर्घता=new_क्षमता;
                }
                uprv_memcpy(p, ptr, (size_t)दीर्घता*परिमाण(T));
            }
            releaseArray();
            ptr=p;
            क्षमता=new_क्षमता;
            need_गन्तव्यम्_Release=true;
        }
        प्रत्यागम p;
    } अन्यथा {
        प्रत्यागम nullptr;
    }
}

प्रतिरूपम्<typename T, int32_t राशिः_क्षमता>
अन्तःस्थ T *Maybe_राशिः_Array<T, राशिः_क्षमता>::orphanOrClone(int32_t दीर्घता, int32_t &परिणाम_क्षमता) {
    T *p;
    यदि(need_गन्तव्यम्_Release) {
        p=ptr;
    } अन्यथा यदि(दीर्घता<=0) {
        प्रत्यागम nullptr;
    } अन्यथा {
        यदि(दीर्घता>क्षमता) {
            दीर्घता=क्षमता;
        }
        p=(T *)uprv_malloc(दीर्घता*परिमाण(T));
#यदि U_DEBUG && defined(UPRV_MALLOC_गणनम्)
      ::fprintf(::stderr,"MaybeStacArray (orphan) alloc %d * %lu\n", दीर्घता,परिमाण(T));
#यदि_अन्तः
        यदि(p==nullptr) {
            प्रत्यागम nullptr;
        }
        uprv_memcpy(p, ptr, (size_t)दीर्घता*परिमाण(T));
    }
    परिणाम_क्षमता=दीर्घता;
    reset_गन्तव्यम्_राशिः_Array();
    प्रत्यागम p;
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
प्रतिरूपम्<typename H, typename T, int32_t राशिः_क्षमता>
class Maybe_राशिः_शीर्षकम्_And_Array {
सार्वजनिक:
    // No heap allocation. Use only on the stack.
    स्थिर शून्य* U_EXPORT2 संक्रियकः new(size_t) noexcept = delete;
    स्थिर शून्य* U_EXPORT2 संक्रियकः new[](size_t) noexcept = delete;
    स्थिर शून्य* U_EXPORT2 संक्रियकः new(size_t, शून्य*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_राशिः_शीर्षकम्_And_Array() : ptr(&राशिः_शीर्षकम्), क्षमता(राशिः_क्षमता), need_गन्तव्यम्_Release(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_राशिः_शीर्षकम्_And_Array() { release_स्मृतिः(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_क्षमता() ध्रुव { प्रत्यागम क्षमता; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() ध्रुव U_LIFETIME_BOUND { प्रत्यागम ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* getArrayStart() ध्रुव U_LIFETIME_BOUND { प्रत्यागम reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* get_Array_सीमा() ध्रुव U_LIFETIME_BOUND { प्रत्यागम getArrayStart() + क्षमता; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    संक्रियकः H*() ध्रुव U_LIFETIME_BOUND { प्रत्यागम ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& संक्रियकः[](ptrdiff_t i) U_LIFETIME_BOUND { प्रत्यागम getArrayStart()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    शून्य aliasInstead(H *other_स्मृतिः, int32_t other_क्षमता) {
        यदि(other_स्मृतिः!=nullptr && other_क्षमता>0) {
            release_स्मृतिः();
            ptr=other_स्मृतिः;
            क्षमता=other_क्षमता;
            need_गन्तव्यम्_Release=false;
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
    अन्तःस्थ H* resize(int32_t new_क्षमता, int32_t दीर्घता = 0) U_LIFETIME_BOUND;
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
    अन्तःस्थ H *orphanOrClone(int32_t दीर्घता, int32_t &परिणाम_क्षमता);
गुप्त:
    H *ptr;
    int32_t क्षमता;
    UBool need_गन्तव्यम्_Release;
    // stackHeader must precede stackArray immediately.
    H राशिः_शीर्षकम्;
    T राशिः_Array[राशिः_क्षमता];
    शून्य release_स्मृतिः() {
        यदि(need_गन्तव्यम्_Release) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool संक्रियकः==(ध्रुव Maybe_राशिः_शीर्षकम्_And_Array & /*other*/) {प्रत्यागम false;}
    bool संक्रियकः!=(ध्रुव Maybe_राशिः_शीर्षकम्_And_Array & /*other*/) {प्रत्यागम true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_राशिः_शीर्षकम्_And_Array(ध्रुव Maybe_राशिः_शीर्षकम्_And_Array & /*other*/) {}
    शून्य संक्रियकः=(ध्रुव Maybe_राशिः_शीर्षकम्_And_Array & /*other*/) {}
};

प्रतिरूपम् <typename H, typename T, int32_t राशिः_क्षमता>
अन्तःस्थ H* Maybe_राशिः_शीर्षकम्_And_Array<H, T, राशिः_क्षमता>::resize(int32_t new_क्षमता,
                                                                int32_t दीर्घता) U_LIFETIME_BOUND {
    यदि(new_क्षमता>=0) {
#यदि U_DEBUG && defined(UPRV_MALLOC_गणनम्)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", परिमाण(H),new_क्षमता,परिमाण(T));
#यदि_अन्तः
        H *p=(H *)uprv_malloc(परिमाण(H)+new_क्षमता*परिमाण(T));
        यदि(p!=nullptr) {
            यदि(दीर्घता<0) {
                दीर्घता=0;
            } अन्यथा यदि(दीर्घता>0) {
                यदि(दीर्घता>क्षमता) {
                    दीर्घता=क्षमता;
                }
                यदि(दीर्घता>new_क्षमता) {
                    दीर्घता=new_क्षमता;
                }
            }
            uprv_memcpy(p, ptr, परिमाण(H)+(size_t)दीर्घता*परिमाण(T));
            release_स्मृतिः();
            ptr=p;
            क्षमता=new_क्षमता;
            need_गन्तव्यम्_Release=true;
        }
        प्रत्यागम p;
    } अन्यथा {
        प्रत्यागम nullptr;
    }
}

प्रतिरूपम्<typename H, typename T, int32_t राशिः_क्षमता>
अन्तःस्थ H *Maybe_राशिः_शीर्षकम्_And_Array<H, T, राशिः_क्षमता>::orphanOrClone(int32_t दीर्घता,
                                                                       int32_t &परिणाम_क्षमता) {
    H *p;
    यदि(need_गन्तव्यम्_Release) {
        p=ptr;
    } अन्यथा {
        यदि(दीर्घता<0) {
            दीर्घता=0;
        } अन्यथा यदि(दीर्घता>क्षमता) {
            दीर्घता=क्षमता;
        }
#यदि U_DEBUG && defined(UPRV_MALLOC_गणनम्)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", परिमाण(H),दीर्घता,परिमाण(T));
#यदि_अन्तः
        p=(H *)uprv_malloc(परिमाण(H)+दीर्घता*परिमाण(T));
        यदि(p==nullptr) {
            प्रत्यागम nullptr;
        }
        uprv_memcpy(p, ptr, परिमाण(H)+(size_t)दीर्घता*परिमाण(T));
    }
    परिणाम_क्षमता=दीर्घता;
    ptr=&राशिः_शीर्षकम्;
    क्षमता=राशिः_क्षमता;
    need_गन्तव्यम्_Release=false;
    प्रत्यागम p;
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
प्रतिरूपम्<typename T, int32_t राशिः_क्षमता = 8>
class स्मृतिः_Pool : सार्वजनिक UMemory {
सार्वजनिक:
    स्मृतिः_Pool() : f_गणनम्(0), fPool() {}

    ~स्मृतिः_Pool() {
        प्रति (int32_t i = 0; i < f_गणनम्; ++i) {
            delete fPool[i];
        }
    }

    स्मृतिः_Pool(ध्रुव स्मृतिः_Pool&) = delete;
    स्मृतिः_Pool& संक्रियकः=(ध्रुव स्मृतिः_Pool&) = delete;

    स्मृतिः_Pool(स्मृतिः_Pool&& other) noexcept : f_गणनम्(other.f_गणनम्),
                                                fPool(std::move(other.fPool)) {
        other.f_गणनम् = 0;
    }

    स्मृतिः_Pool& संक्रियकः=(स्मृतिः_Pool&& other) noexcept U_LIFETIME_BOUND {
        // Since `this` may contain instances that need to be deleted, we can't
        // just throw them away and replace them with `other`. The normal way of
        // dealing with this in C++ is to swap `this` and `other`, rather than
        // simply overwrite: the destruction of `other` can then take care of
        // running MemoryPool::~MemoryPool() over the still-to-be-deallocated
        // instances.
        std::swap(f_गणनम्, other.f_गणनम्);
        std::swap(fPool, other.fPool);
        प्रत्यागम *this;
    }

    /**
     * Creates a new object of typename T, by forwarding any and all arguments
     * to the typename T constructor.
     *
     * @param args Arguments to be forwarded to the typename T constructor.
     * @return A pointer to the newly created object, or nullptr on error.
     */
    प्रतिरूपम् <typename... तर्काः>
    T* create(तर्काः&&... तर्काः_2) U_LIFETIME_BOUND {
        int32_t क्षमता = fPool.get_क्षमता();
        यदि (f_गणनम् == क्षमता &&
            fPool.resize(क्षमता == राशिः_क्षमता ? 4 * क्षमता : 2 * क्षमता,
                         क्षमता) == nullptr) {
            प्रत्यागम nullptr;
        }
        प्रत्यागम fPool[f_गणनम्++] = new T(std::forward<तर्काः>(तर्काः_2)...);
    }

    प्रतिरूपम् <typename... तर्काः>
    T* create_And_Check_Error_संहिता(UError_संहिता& स्थितिः, तर्काः&&... तर्काः_2) U_LIFETIME_BOUND {
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम nullptr;
        }
        T *pointer = this->create(तर्काः_2...);
        यदि (U_SUCCESS(स्थितिः) && pointer == nullptr) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        }
        प्रत्यागम pointer;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t गणनम्() ध्रुव {
        प्रत्यागम f_गणनम्;
    }

रक्षित:
    int32_t f_गणनम्;
    Maybe_राशिः_Array<T*, राशिः_क्षमता> fPool;
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
प्रतिरूपम्<typename T, int32_t राशिः_क्षमता = 8>
class Maybe_राशिः_सदिशम् : रक्षित स्मृतिः_Pool<T, राशिः_क्षमता> {
सार्वजनिक:
    प्रतिरूपम् <typename... तर्काः>
    T* emplace_पृष्ठभागः(तर्काः&&... तर्काः_2) U_LIFETIME_BOUND {
        प्रत्यागम this->create(तर्काः_2...);
    }

    प्रतिरूपम् <typename... तर्काः>
    T* emplace_पृष्ठभागः_And_Check_Error_संहिता(UError_संहिता& स्थितिः, तर्काः&&... तर्काः_2) U_LIFETIME_BOUND {
        प्रत्यागम this->create_And_Check_Error_संहिता(स्थितिः, तर्काः_2...);
    }

    int32_t दीर्घता() ध्रुव {
        प्रत्यागम this->f_गणनम्;
    }

    T** getAlias() U_LIFETIME_BOUND {
        प्रत्यागम this->fPool.getAlias();
    }

    ध्रुव T* ध्रुव* getAlias() ध्रुव U_LIFETIME_BOUND {
        प्रत्यागम this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    ध्रुव T* संक्रियकः[](ptrdiff_t i) ध्रुव U_LIFETIME_BOUND {
        प्रत्यागम this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* संक्रियकः[](ptrdiff_t i) U_LIFETIME_BOUND {
        प्रत्यागम this->fPool[i];
    }
};

namespace prv {

#यदि U_OVERRIDE_CXX_आवण्टनम्

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
प्रतिरूपम् <typename T, शून्य deallocate(शून्य *), typename = शून्य> class Deleter {
  सार्वजनिक:
    Deleter() = स्वभाव;
    शून्य संक्रियकः()(T *ध्रुव ptr) ध्रुव {
        यदि constexpr (!std::is_trivially_destructible_v<T>) {
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
प्रतिरूपम् <typename T, शून्य deallocate(शून्य *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  सार्वजनिक:
    Deleter() = स्वभाव;
    शून्य संक्रियकः()(T *ध्रुव ptr) ध्रुव { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
प्रतिरूपम् <typename T, शून्य deallocate(शून्य *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  सार्वजनिक:
    Deleter(ध्रुव std::size_t आकार_2) : आकार(आकार_2) {}
    शून्य संक्रियकः()(T *ध्रुव ptr) ध्रुव {
        प्रति (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(आकार) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  गुप्त:
    ध्रुव std::size_t आकार;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
प्रतिरूपम् <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#अन्यथा

using std::unique_ptr;

#यदि_अन्तः

/**
 * Helper for make_unique<T, Args...>; forwards all but the last of args to the constructor of
 * T, uses the last of args as the error code for allocation errors.
 *
 * As usual, does nothing if the incoming UErrorCode already is U_FAILURE.
 *
 * @internal
 */
प्रतिरूपम् <typename T, typename... तर्काः, std::size_t... i>
unique_ptr<T> make_unique_impl(std::tuple<तर्काः...> तर्काः_2,
                               std::सूचकांक_sequence<i...> constructor_arg_indices) {
    static_assert(std::is_same_v<decltype(constructor_arg_indices),
                                 std::make_सूचकांक_sequence<परिमाण...(तर्काः) - 1>>);
    स्वचालित &&error = std::get<परिमाण...(तर्काः) - 1>(तर्काः_2);
    static_assert(std::is_same_v<decltype(error), UError_संहिता &>);
    यदि (U_FAILURE(error)) {
        प्रत्यागम nullptr;
    }
#यदि U_OVERRIDE_CXX_आवण्टनम्
    शून्य *ध्रुव बफर = uprv_malloc(परिमाण(T));
    यदि (बफर == nullptr) {
        error = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम nullptr;
    }
    प्रत्यागम unique_ptr<T>(new (बफर) T(std::get<i>(तर्काः_2)...));
#अन्यथा
    प्रत्यागम std::make_unique<T>(std::get<i>(तर्काः_2)...);
#यदि_अन्तः
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
प्रतिरूपम् <typename T, typename... तर्काः, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique(तर्काः &&...तर्काः_2) {
    प्रत्यागम make_unique_impl<T>(std::forward_as_tuple(तर्काः_2...),
                               std::make_सूचकांक_sequence<परिमाण...(तर्काः) - 1>());
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
प्रतिरूपम् <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique(ध्रुव std::size_t n, UError_संहिता &error) {
    यदि (U_FAILURE(error)) {
        प्रत्यागम nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_आकार = SIZE_MAX / परिमाण(std::remove_extent_t<T>);
    यदि (n > max_आकार) {
        error = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम nullptr;
    }
#यदि U_OVERRIDE_CXX_आवण्टनम्
    शून्य *ध्रुव बफर = uprv_malloc(परिमाण(std::remove_extent_t<T>) * n);
    यदि (बफर == nullptr) {
        error = U_स्मृतिः_आवण्टनम्_ERROR;
        यदि constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            प्रत्यागम nullptr;
        } अन्यथा {
            प्रत्यागम unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    यदि constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        प्रत्यागम unique_ptr<T>(new (बफर) std::remove_extent_t<T>[n]());
    } अन्यथा {
        प्रत्यागम unique_ptr<T>(new (बफर) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#अन्यथा
    प्रत्यागम std::make_unique<T>(n);
#यदि_अन्तः
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
प्रतिरूपम् <typename T, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique_for_overwrite(UError_संहिता &error) {
    यदि (U_FAILURE(error)) {
        प्रत्यागम nullptr;
    }
#यदि U_OVERRIDE_CXX_आवण्टनम्
    शून्य *ध्रुव बफर = uprv_malloc(परिमाण(T));
    यदि (बफर == nullptr) {
        error = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम nullptr;
    }
    प्रत्यागम unique_ptr<T>(new (बफर) T);
#नोचेद्यदि U_CPLUSPLUS_संस्करणम् >= 20
    प्रत्यागम std::make_unique_for_overwrite<T>();
#अन्यथा
    प्रत्यागम unique_ptr<T>(new T);
#यदि_अन्तः
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
प्रतिरूपम् <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique_for_overwrite(ध्रुव std::size_t n, UError_संहिता &error) {
    यदि (U_FAILURE(error)) {
        प्रत्यागम nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_आकार = SIZE_MAX / परिमाण(std::remove_extent_t<T>);
    यदि (n > max_आकार) {
        error = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम nullptr;
    }
#यदि U_OVERRIDE_CXX_आवण्टनम्
    शून्य *ध्रुव बफर = uprv_malloc(परिमाण(std::remove_extent_t<T>) * n);
    यदि (बफर == nullptr) {
        error = U_स्मृतिः_आवण्टनम्_ERROR;
        यदि constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            प्रत्यागम nullptr;
        } अन्यथा {
            प्रत्यागम unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    यदि constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        प्रत्यागम unique_ptr<T>(new (बफर) std::remove_extent_t<T>[n]);
    } अन्यथा {
        प्रत्यागम unique_ptr<T>(new (बफर) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#नोचेद्यदि U_CPLUSPLUS_संस्करणम् >= 20
    प्रत्यागम std::make_unique_for_overwrite<T>(n);
#अन्यथा
    प्रत्यागम unique_ptr<T>(new std::remove_extent_t<T>[n]);
#यदि_अन्तः
}

}  // namespace prv

U_NAMESPACE_अन्तः

#यदि_अन्तः  /* __cplusplus */
#यदि_अन्तः  /* CMEMORY_H */
