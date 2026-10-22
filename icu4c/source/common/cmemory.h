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

#నిర్వచితం_కాకపోతే CMEMORY_H
#నిర్వచించు CMEMORY_H

#అన్తర్భావ "unicode/utypes.h"

#అన్తర్భావ <limits.h>
#అన్తర్భావ <stddef.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ "unicode/localpointer.h"
#అన్తర్భావ "uassert.h"

#ఒకవేళ U_DEBUG && defined(UPRV_MALLOC_లెక్క)
#అన్తర్భావ <stdio.h>
#అయితే_ముగింపు

// uprv_memcpy and uprv_memmove
#ఒకవేళ defined(__clang__)
#నిర్వచించు uprv_memcpy(dst, src, కొలత_2) UPRV_ఖండం_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ఖాళీ); \
    U_ASSERT(src != ఖాళీ); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE జ్ఞాపకం_నకలు(dst, src, కొలత_2); \
} UPRV_ఖండం_MACRO_చివర
#నిర్వచించు uprv_memmove(dst, src, కొలత_2) UPRV_ఖండం_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ఖాళీ); \
    U_ASSERT(src != ఖాళీ); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, కొలత_2); \
} UPRV_ఖండం_MACRO_చివర
#లేదావేళ defined(__GNUC__)
#నిర్వచించు uprv_memcpy(dst, src, కొలత_2) UPRV_ఖండం_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ఖాళీ); \
    U_ASSERT(src != ఖాళీ); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE జ్ఞాపకం_నకలు(dst, src, కొలత_2); \
} UPRV_ఖండం_MACRO_చివర
#నిర్వచించు uprv_memmove(dst, src, కొలత_2) UPRV_ఖండం_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != ఖాళీ); \
    U_ASSERT(src != ఖాళీ); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, కొలత_2); \
} UPRV_ఖండం_MACRO_చివర
#లేకపోతే
#నిర్వచించు uprv_memcpy(dst, src, కొలత_2) UPRV_ఖండం_MACRO_BEGIN { \
    U_ASSERT(dst != ఖాళీ); \
    U_ASSERT(src != ఖాళీ); \
    U_STANDARD_CPP_NAMESPACE జ్ఞాపకం_నకలు(dst, src, కొలత_2); \
} UPRV_ఖండం_MACRO_చివర
#నిర్వచించు uprv_memmove(dst, src, కొలత_2) UPRV_ఖండం_MACRO_BEGIN { \
    U_ASSERT(dst != ఖాళీ); \
    U_ASSERT(src != ఖాళీ); \
    U_STANDARD_CPP_NAMESPACE memmove(dst, src, కొలత_2); \
} UPRV_ఖండం_MACRO_చివర
#అయితే_ముగింపు

/**
 * \def UPRV_LENGTHOF
 * Convenience macro to determine the length of a fixed array at compile-time.
 * @param array A fixed length array
 * @return The length of the array, in elements
 * @internal
 */
#నిర్వచించు UPRV_LENGTHOF(array) (int32_t)(పరిమాణం(array)/పరిమాణం((array)[0]))
#నిర్వచించు uprv_memset(బఫరు, mark, కొలత_2) U_STANDARD_CPP_NAMESPACE memset(బఫరు, mark, కొలత_2)
#నిర్వచించు uprv_memcmp(buffer1, buffer2, కొలత_2) U_STANDARD_CPP_NAMESPACE memcmp(buffer1, buffer2,కొలత_2)
#నిర్వచించు uprv_memchr(ptr, విలువ_2, అంకె) U_STANDARD_CPP_NAMESPACE memchr(ptr, విలువ_2, అంకె)

U_CAPI శూన్య * U_EXPORT2
uprv_malloc(size_t s) U_MALLOC_ATTR U_ALLOC_కొలత_ATTR(1);

U_CAPI శూన్య * U_EXPORT2
uprv_realloc(శూన్య *mem, size_t కొలత_2) U_ALLOC_కొలత_ATTR(2);

U_CAPI శూన్య U_EXPORT2
uprv_free(శూన్య *mem);

U_CAPI శూన్య * U_EXPORT2
uprv_calloc(size_t అంకె, size_t కొలత_2) U_MALLOC_ATTR U_ALLOC_కొలత_ATTR2(1,2);

/**
 * Get the least significant bits of a pointer (a memory address).
 * For example, with a mask of 3, the macro gets the 2 least significant bits,
 * which will be 0 if the pointer is 32-bit (4-byte) aligned.
 *
 * uintptr_t is the most appropriate integer type to cast to.
 */
#నిర్వచించు U_సూచిక_ముసుగు_LSB(ptr, ముసుగు) ((uintptr_t)(ptr) & (ముసుగు))

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
#నిర్వచించు STATIC_NEW(type) [] () { \
    alignas(type) స్థిర అక్షరం storage[పరిమాణం(type)]; \
    తిరిగి new(storage) type();} ()

/**
  *  Heap clean up function, called from u_cleanup()
  *    Clears any user heap functions from u_setMemoryFunctions()
  *    Does NOT deallocate any remaining allocated memory.
  */
U_CFUNC UBool 
cmemory_cleanup(శూన్య);

/**
 * A function called by <TT>uhash_remove</TT>,
 * <TT>uhash_close</TT>, or <TT>uhash_put</TT> to delete
 * an existing key or value.
 * @param obj A key or value stored in a hashtable
 * @see uprv_deleteUObject
 */
రకనిర్వచనం శూన్య U_CALLCONV UObjectDeleter(శూన్య* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI శూన్య U_EXPORT2
uprv_deleteUObject(శూన్య *obj);

#నిర్వచితమైతే __cplusplus

#అన్తర్భావ <memory>
#అన్తర్భావ <utility>
#అన్తర్భావ <type_traits>
#అన్తర్భావ "unicode/uobject.h"

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
మూస<typename T>
class Local_జ్ఞాపకం : బహిరంగ Local_సూచిక_పునాది<T> {
బహిరంగ:
    using Local_సూచిక_పునాది<T>::క్రియాకారకం*;
    using Local_సూచిక_పునాది<T>::క్రియాకారకం->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an array of T items that is adopted
     */
    explicit Local_జ్ఞాపకం(T *p=nullptr) : Local_సూచిక_పునాది<T>(p) {}
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     */
    Local_జ్ఞాపకం(Local_జ్ఞాపకం<T> &&src) noexcept : Local_సూచిక_పునాది<T>(src.ptr) {
        src.ptr=nullptr;
    }
    /**
     * Destructor deletes the memory it owns.
     */
    ~Local_జ్ఞాపకం() {
        uprv_free(Local_సూచిక_పునాది<T>::ptr);
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     */
    Local_జ్ఞాపకం<T>& క్రియాకారకం=(Local_జ్ఞాపకం<T>&& src) noexcept U_LIFETIME_BOUND {
        uprv_free(Local_సూచిక_పునాది<T>::ptr);
        Local_సూచిక_పునాది<T>::ptr=src.ptr;
        src.ptr=nullptr;
        తిరిగి *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    శూన్య మార్పిడి(Local_జ్ఞాపకం<T> &other) noexcept {
        T *తాత్కాలిక=Local_సూచిక_పునాది<T>::ptr;
        Local_సూచిక_పునాది<T>::ptr=other.ptr;
        other.ptr=తాత్కాలిక;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    మిత్ర అంతఃవరుస శూన్య మార్పిడి(Local_జ్ఞాపకం<T> &p1, Local_జ్ఞాపకం<T> &p2) noexcept {
        p1.మార్పిడి(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    శూన్య adoptInstead(T *p) {
        uprv_free(Local_సూచిక_పునాది<T>::ptr);
        Local_సూచిక_పునాది<T>::ptr=p;
    }
    /**
     * Deletes the array it owns, allocates a new one and reset its bytes to 0.
     * Returns the new array pointer.
     * If the allocation fails, then the current array is unchanged and
     * this method returns nullptr.
     * @param newCapacity must be >0
     * @return the allocated array pointer, or nullptr if the allocation failed
     */
    అంతఃవరుస T* allocateInsteadAndReset(int32_t new_సామర్థ్యం = 1) U_LIFETIME_BOUND;
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
    అంతఃవరుస T* allocateInsteadAndCopy(int32_t new_సామర్థ్యం = 1, int32_t నిడివి = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& క్రియాకారకం[](ptrdiff_t i) స్థిరరాశి U_LIFETIME_BOUND { తిరిగి Local_సూచిక_పునాది<T>::ptr[i]; }
};

మూస <typename T>
అంతఃవరుస T* Local_జ్ఞాపకం<T>::allocateInsteadAndReset(int32_t new_సామర్థ్యం) U_LIFETIME_BOUND {
    ఒకవేళ(new_సామర్థ్యం>0) {
        T *p=(T *)uprv_malloc(new_సామర్థ్యం*పరిమాణం(T));
        ఒకవేళ(p!=nullptr) {
            uprv_memset(p, 0, new_సామర్థ్యం*పరిమాణం(T));
            uprv_free(Local_సూచిక_పునాది<T>::ptr);
            Local_సూచిక_పునాది<T>::ptr=p;
        }
        తిరిగి p;
    } లేకపోతే {
        తిరిగి nullptr;
    }
}

మూస <typename T>
అంతఃవరుస T* Local_జ్ఞాపకం<T>::allocateInsteadAndCopy(int32_t new_సామర్థ్యం,
                                                 int32_t నిడివి) U_LIFETIME_BOUND {
    ఒకవేళ(new_సామర్థ్యం>0) {
        T *p=(T *)uprv_malloc(new_సామర్థ్యం*పరిమాణం(T));
        ఒకవేళ(p!=nullptr) {
            ఒకవేళ(నిడివి>0) {
                ఒకవేళ(నిడివి>new_సామర్థ్యం) {
                    నిడివి=new_సామర్థ్యం;
                }
                uprv_memcpy(p, Local_సూచిక_పునాది<T>::ptr, (size_t)నిడివి*పరిమాణం(T));
            }
            uprv_free(Local_సూచిక_పునాది<T>::ptr);
            Local_సూచిక_పునాది<T>::ptr=p;
        }
        తిరిగి p;
    } లేకపోతే {
        తిరిగి nullptr;
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
మూస<typename T, int32_t దొంతర_సామర్థ్యం>
class Maybe_దొంతర_Array {
బహిరంగ:
    // No heap allocation. Use only on the stack.
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new(size_t) noexcept = delete;
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new[](size_t) noexcept = delete;
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new(size_t, శూన్య*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_దొంతర_Array() : ptr(దొంతర_Array), సామర్థ్యం(దొంతర_సామర్థ్యం), need_గమ్యం_Release(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_దొంతర_Array(int32_t new_సామర్థ్యం, UError_సంకేతం స్థితి_2) : Maybe_దొంతర_Array() {
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి;
        }
        ఒకవేళ (సామర్థ్యం < new_సామర్థ్యం) {
            ఒకవేళ (resize(new_సామర్థ్యం) == nullptr) {
                స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            }
        }
    }
    /**
     * Destructor deletes the array (if owned).
     */
    ~Maybe_దొంతర_Array() { releaseArray(); }
    /**
     * Move constructor: transfers ownership or copies the stack array.
     */
    Maybe_దొంతర_Array(Maybe_దొంతర_Array<T, దొంతర_సామర్థ్యం> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_దొంతర_Array<T, దొంతర_సామర్థ్యం>&
    క్రియాకారకం=(Maybe_దొంతర_Array<T, దొంతర_సామర్థ్యం>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_సామర్థ్యం() స్థిరరాశి { తిరిగి సామర్థ్యం; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* get_Array_హద్దు() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి getAlias() + సామర్థ్యం; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    స్థిరరాశి T& క్రియాకారకం[](ptrdiff_t i) స్థిరరాశి U_LIFETIME_BOUND { తిరిగి ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& క్రియాకారకం[](ptrdiff_t i) U_LIFETIME_BOUND { తిరిగి ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    శూన్య aliasInstead(T *otherArray, int32_t other_సామర్థ్యం) {
        ఒకవేళ(otherArray!=nullptr && other_సామర్థ్యం>0) {
            releaseArray();
            ptr=otherArray;
            సామర్థ్యం=other_సామర్థ్యం;
            need_గమ్యం_Release=false;
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
    అంతఃవరుస T* resize(int32_t new_సామర్థ్యం, int32_t నిడివి = 0) U_LIFETIME_BOUND;
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
    అంతఃవరుస T *orphanOrClone(int32_t నిడివి, int32_t &ఫలితం_సామర్థ్యం);

రక్షిత:
    // Resizes the array to the size of src, then copies the contents of src.
    శూన్య copyFrom(స్థిరరాశి Maybe_దొంతర_Array &src, UError_సంకేతం &స్థితి_2) {
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి;
        }
        ఒకవేళ (this->resize(src.సామర్థ్యం, 0) == nullptr) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
        uprv_memcpy(this->ptr, src.ptr, (size_t)సామర్థ్యం * పరిమాణం(T));
    }

అంతరంగ:
    T *ptr;
    int32_t సామర్థ్యం;
    UBool need_గమ్యం_Release;
    T దొంతర_Array[దొంతర_సామర్థ్యం];
    శూన్య releaseArray() {
        ఒకవేళ(need_గమ్యం_Release) {
            uprv_free(ptr);
        }
    }
    శూన్య reset_గమ్యం_దొంతర_Array() {
        ptr=దొంతర_Array;
        సామర్థ్యం=దొంతర_సామర్థ్యం;
        need_గమ్యం_Release=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool క్రియాకారకం==(స్థిరరాశి Maybe_దొంతర_Array & /*other*/) = delete;
    bool క్రియాకారకం!=(స్థిరరాశి Maybe_దొంతర_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_దొంతర_Array(స్థిరరాశి Maybe_దొంతర_Array & /*other*/) = delete;
    శూన్య క్రియాకారకం=(స్థిరరాశి Maybe_దొంతర_Array & /*other*/) = delete;
};

మూస<typename T, int32_t దొంతర_సామర్థ్యం>
icu::Maybe_దొంతర_Array<T, దొంతర_సామర్థ్యం>::Maybe_దొంతర_Array(
        Maybe_దొంతర_Array <T, దొంతర_సామర్థ్యం>&& src) noexcept
        : ptr(src.ptr), సామర్థ్యం(src.సామర్థ్యం), need_గమ్యం_Release(src.need_గమ్యం_Release) {
    ఒకవేళ (src.ptr == src.దొంతర_Array) {
        ptr = దొంతర_Array;
        uprv_memcpy(దొంతర_Array, src.దొంతర_Array, పరిమాణం(T) * src.సామర్థ్యం);
    } లేకపోతే {
        src.reset_గమ్యం_దొంతర_Array();  // take ownership away from src
    }
}

మూస <typename T, int32_t దొంతర_సామర్థ్యం>
అంతఃవరుస Maybe_దొంతర_Array<T, దొంతర_సామర్థ్యం>& Maybe_దొంతర_Array<T, దొంతర_సామర్థ్యం>::క్రియాకారకం=(
    Maybe_దొంతర_Array<T, దొంతర_సామర్థ్యం>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    సామర్థ్యం = src.సామర్థ్యం;
    need_గమ్యం_Release = src.need_గమ్యం_Release;
    ఒకవేళ (src.ptr == src.దొంతర_Array) {
        ptr = దొంతర_Array;
        uprv_memcpy(దొంతర_Array, src.దొంతర_Array, పరిమాణం(T) * src.సామర్థ్యం);
    } లేకపోతే {
        ptr = src.ptr;
        src.reset_గమ్యం_దొంతర_Array();  // take ownership away from src
    }
    తిరిగి *this;
}

మూస <typename T, int32_t దొంతర_సామర్థ్యం>
అంతఃవరుస T* Maybe_దొంతర_Array<T, దొంతర_సామర్థ్యం>::resize(int32_t new_సామర్థ్యం,
                                                    int32_t నిడివి) U_LIFETIME_BOUND {
    ఒకవేళ(new_సామర్థ్యం>0) {
#ఒకవేళ U_DEBUG && defined(UPRV_MALLOC_లెక్క)
        ::fprintf(::stderr, "MaybeStackArray (resize) alloc %d * %lu\n", new_సామర్థ్యం, పరిమాణం(T));
#అయితే_ముగింపు
        T *p=(T *)uprv_malloc(new_సామర్థ్యం*పరిమాణం(T));
        ఒకవేళ(p!=nullptr) {
            ఒకవేళ(నిడివి>0) {
                ఒకవేళ(నిడివి>సామర్థ్యం) {
                    నిడివి=సామర్థ్యం;
                }
                ఒకవేళ(నిడివి>new_సామర్థ్యం) {
                    నిడివి=new_సామర్థ్యం;
                }
                uprv_memcpy(p, ptr, (size_t)నిడివి*పరిమాణం(T));
            }
            releaseArray();
            ptr=p;
            సామర్థ్యం=new_సామర్థ్యం;
            need_గమ్యం_Release=true;
        }
        తిరిగి p;
    } లేకపోతే {
        తిరిగి nullptr;
    }
}

మూస<typename T, int32_t దొంతర_సామర్థ్యం>
అంతఃవరుస T *Maybe_దొంతర_Array<T, దొంతర_సామర్థ్యం>::orphanOrClone(int32_t నిడివి, int32_t &ఫలితం_సామర్థ్యం) {
    T *p;
    ఒకవేళ(need_గమ్యం_Release) {
        p=ptr;
    } లేకపోతే ఒకవేళ(నిడివి<=0) {
        తిరిగి nullptr;
    } లేకపోతే {
        ఒకవేళ(నిడివి>సామర్థ్యం) {
            నిడివి=సామర్థ్యం;
        }
        p=(T *)uprv_malloc(నిడివి*పరిమాణం(T));
#ఒకవేళ U_DEBUG && defined(UPRV_MALLOC_లెక్క)
      ::fprintf(::stderr,"MaybeStacArray (orphan) alloc %d * %lu\n", నిడివి,పరిమాణం(T));
#అయితే_ముగింపు
        ఒకవేళ(p==nullptr) {
            తిరిగి nullptr;
        }
        uprv_memcpy(p, ptr, (size_t)నిడివి*పరిమాణం(T));
    }
    ఫలితం_సామర్థ్యం=నిడివి;
    reset_గమ్యం_దొంతర_Array();
    తిరిగి p;
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
మూస<typename H, typename T, int32_t దొంతర_సామర్థ్యం>
class Maybe_దొంతర_శీర్షిక_And_Array {
బహిరంగ:
    // No heap allocation. Use only on the stack.
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new(size_t) noexcept = delete;
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new[](size_t) noexcept = delete;
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new(size_t, శూన్య*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_దొంతర_శీర్షిక_And_Array() : ptr(&దొంతర_శీర్షిక), సామర్థ్యం(దొంతర_సామర్థ్యం), need_గమ్యం_Release(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_దొంతర_శీర్షిక_And_Array() { release_జ్ఞాపకం(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t get_సామర్థ్యం() స్థిరరాశి { తిరిగి సామర్థ్యం; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* get_Array_ప్రారంభం() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* get_Array_హద్దు() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి get_Array_ప్రారంభం() + సామర్థ్యం; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    క్రియాకారకం H*() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& క్రియాకారకం[](ptrdiff_t i) U_LIFETIME_BOUND { తిరిగి get_Array_ప్రారంభం()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    శూన్య aliasInstead(H *other_జ్ఞాపకం, int32_t other_సామర్థ్యం) {
        ఒకవేళ(other_జ్ఞాపకం!=nullptr && other_సామర్థ్యం>0) {
            release_జ్ఞాపకం();
            ptr=other_జ్ఞాపకం;
            సామర్థ్యం=other_సామర్థ్యం;
            need_గమ్యం_Release=false;
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
    అంతఃవరుస H* resize(int32_t new_సామర్థ్యం, int32_t నిడివి = 0) U_LIFETIME_BOUND;
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
    అంతఃవరుస H *orphanOrClone(int32_t నిడివి, int32_t &ఫలితం_సామర్థ్యం);
అంతరంగ:
    H *ptr;
    int32_t సామర్థ్యం;
    UBool need_గమ్యం_Release;
    // stackHeader must precede stackArray immediately.
    H దొంతర_శీర్షిక;
    T దొంతర_Array[దొంతర_సామర్థ్యం];
    శూన్య release_జ్ఞాపకం() {
        ఒకవేళ(need_గమ్యం_Release) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool క్రియాకారకం==(స్థిరరాశి Maybe_దొంతర_శీర్షిక_And_Array & /*other*/) {తిరిగి false;}
    bool క్రియాకారకం!=(స్థిరరాశి Maybe_దొంతర_శీర్షిక_And_Array & /*other*/) {తిరిగి true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_దొంతర_శీర్షిక_And_Array(స్థిరరాశి Maybe_దొంతర_శీర్షిక_And_Array & /*other*/) {}
    శూన్య క్రియాకారకం=(స్థిరరాశి Maybe_దొంతర_శీర్షిక_And_Array & /*other*/) {}
};

మూస <typename H, typename T, int32_t దొంతర_సామర్థ్యం>
అంతఃవరుస H* Maybe_దొంతర_శీర్షిక_And_Array<H, T, దొంతర_సామర్థ్యం>::resize(int32_t new_సామర్థ్యం,
                                                                int32_t నిడివి) U_LIFETIME_BOUND {
    ఒకవేళ(new_సామర్థ్యం>=0) {
#ఒకవేళ U_DEBUG && defined(UPRV_MALLOC_లెక్క)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", పరిమాణం(H),new_సామర్థ్యం,పరిమాణం(T));
#అయితే_ముగింపు
        H *p=(H *)uprv_malloc(పరిమాణం(H)+new_సామర్థ్యం*పరిమాణం(T));
        ఒకవేళ(p!=nullptr) {
            ఒకవేళ(నిడివి<0) {
                నిడివి=0;
            } లేకపోతే ఒకవేళ(నిడివి>0) {
                ఒకవేళ(నిడివి>సామర్థ్యం) {
                    నిడివి=సామర్థ్యం;
                }
                ఒకవేళ(నిడివి>new_సామర్థ్యం) {
                    నిడివి=new_సామర్థ్యం;
                }
            }
            uprv_memcpy(p, ptr, పరిమాణం(H)+(size_t)నిడివి*పరిమాణం(T));
            release_జ్ఞాపకం();
            ptr=p;
            సామర్థ్యం=new_సామర్థ్యం;
            need_గమ్యం_Release=true;
        }
        తిరిగి p;
    } లేకపోతే {
        తిరిగి nullptr;
    }
}

మూస<typename H, typename T, int32_t దొంతర_సామర్థ్యం>
అంతఃవరుస H *Maybe_దొంతర_శీర్షిక_And_Array<H, T, దొంతర_సామర్థ్యం>::orphanOrClone(int32_t నిడివి,
                                                                       int32_t &ఫలితం_సామర్థ్యం) {
    H *p;
    ఒకవేళ(need_గమ్యం_Release) {
        p=ptr;
    } లేకపోతే {
        ఒకవేళ(నిడివి<0) {
            నిడివి=0;
        } లేకపోతే ఒకవేళ(నిడివి>సామర్థ్యం) {
            నిడివి=సామర్థ్యం;
        }
#ఒకవేళ U_DEBUG && defined(UPRV_MALLOC_లెక్క)
      ::fprintf(::stderr,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", పరిమాణం(H),నిడివి,పరిమాణం(T));
#అయితే_ముగింపు
        p=(H *)uprv_malloc(పరిమాణం(H)+నిడివి*పరిమాణం(T));
        ఒకవేళ(p==nullptr) {
            తిరిగి nullptr;
        }
        uprv_memcpy(p, ptr, పరిమాణం(H)+(size_t)నిడివి*పరిమాణం(T));
    }
    ఫలితం_సామర్థ్యం=నిడివి;
    ptr=&దొంతర_శీర్షిక;
    సామర్థ్యం=దొంతర_సామర్థ్యం;
    need_గమ్యం_Release=false;
    తిరిగి p;
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
మూస<typename T, int32_t దొంతర_సామర్థ్యం = 8>
class జ్ఞాపకం_Pool : బహిరంగ UMemory {
బహిరంగ:
    జ్ఞాపకం_Pool() : f_లెక్క(0), fPool() {}

    ~జ్ఞాపకం_Pool() {
        కొరకు (int32_t i = 0; i < f_లెక్క; ++i) {
            delete fPool[i];
        }
    }

    జ్ఞాపకం_Pool(స్థిరరాశి జ్ఞాపకం_Pool&) = delete;
    జ్ఞాపకం_Pool& క్రియాకారకం=(స్థిరరాశి జ్ఞాపకం_Pool&) = delete;

    జ్ఞాపకం_Pool(జ్ఞాపకం_Pool&& other) noexcept : f_లెక్క(other.f_లెక్క),
                                                fPool(std::move(other.fPool)) {
        other.f_లెక్క = 0;
    }

    జ్ఞాపకం_Pool& క్రియాకారకం=(జ్ఞాపకం_Pool&& other) noexcept U_LIFETIME_BOUND {
        // Since `this` may contain instances that need to be deleted, we can't
        // just throw them away and replace them with `other`. The normal way of
        // dealing with this in C++ is to swap `this` and `other`, rather than
        // simply overwrite: the destruction of `other` can then take care of
        // running MemoryPool::~MemoryPool() over the still-to-be-deallocated
        // instances.
        std::మార్పిడి(f_లెక్క, other.f_లెక్క);
        std::మార్పిడి(fPool, other.fPool);
        తిరిగి *this;
    }

    /**
     * Creates a new object of typename T, by forwarding any and all arguments
     * to the typename T constructor.
     *
     * @param args Arguments to be forwarded to the typename T constructor.
     * @return A pointer to the newly created object, or nullptr on error.
     */
    మూస <typename... వాదనలు>
    T* create(వాదనలు&&... వాదనలు_2) U_LIFETIME_BOUND {
        int32_t సామర్థ్యం = fPool.get_సామర్థ్యం();
        ఒకవేళ (f_లెక్క == సామర్థ్యం &&
            fPool.resize(సామర్థ్యం == దొంతర_సామర్థ్యం ? 4 * సామర్థ్యం : 2 * సామర్థ్యం,
                         సామర్థ్యం) == nullptr) {
            తిరిగి nullptr;
        }
        తిరిగి fPool[f_లెక్క++] = new T(std::forward<వాదనలు>(వాదనలు_2)...);
    }

    మూస <typename... వాదనలు>
    T* create_And_Check_Error_సంకేతం(UError_సంకేతం& స్థితి_2, వాదనలు&&... వాదనలు_2) U_LIFETIME_BOUND {
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి nullptr;
        }
        T *సూచిక = this->create(వాదనలు_2...);
        ఒకవేళ (U_SUCCESS(స్థితి_2) && సూచిక == nullptr) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
        తిరిగి సూచిక;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t లెక్క() స్థిరరాశి {
        తిరిగి f_లెక్క;
    }

రక్షిత:
    int32_t f_లెక్క;
    Maybe_దొంతర_Array<T*, దొంతర_సామర్థ్యం> fPool;
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
మూస<typename T, int32_t దొంతర_సామర్థ్యం = 8>
class Maybe_దొంతర_సదిశ : రక్షిత జ్ఞాపకం_Pool<T, దొంతర_సామర్థ్యం> {
బహిరంగ:
    మూస <typename... వాదనలు>
    T* emplace_వెనుక(వాదనలు&&... వాదనలు_2) U_LIFETIME_BOUND {
        తిరిగి this->create(వాదనలు_2...);
    }

    మూస <typename... వాదనలు>
    T* emplace_వెనుక_And_Check_Error_సంకేతం(UError_సంకేతం& స్థితి_2, వాదనలు&&... వాదనలు_2) U_LIFETIME_BOUND {
        తిరిగి this->create_And_Check_Error_సంకేతం(స్థితి_2, వాదనలు_2...);
    }

    int32_t నిడివి() స్థిరరాశి {
        తిరిగి this->f_లెక్క;
    }

    T** getAlias() U_LIFETIME_BOUND {
        తిరిగి this->fPool.getAlias();
    }

    స్థిరరాశి T* స్థిరరాశి* getAlias() స్థిరరాశి U_LIFETIME_BOUND {
        తిరిగి this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    స్థిరరాశి T* క్రియాకారకం[](ptrdiff_t i) స్థిరరాశి U_LIFETIME_BOUND {
        తిరిగి this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* క్రియాకారకం[](ptrdiff_t i) U_LIFETIME_BOUND {
        తిరిగి this->fPool[i];
    }
};

namespace prv {

#ఒకవేళ U_OVERRIDE_CXX_కేటాయింపు

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
మూస <typename T, శూన్య deallocate(శూన్య *), typename = శూన్య> class Deleter {
  బహిరంగ:
    Deleter() = అప్రమేయ;
    శూన్య క్రియాకారకం()(T *స్థిరరాశి ptr) స్థిరరాశి {
        ఒకవేళ constexpr (!std::is_trivially_destructible_v<T>) {
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
మూస <typename T, శూన్య deallocate(శూన్య *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  బహిరంగ:
    Deleter() = అప్రమేయ;
    శూన్య క్రియాకారకం()(T *స్థిరరాశి ptr) స్థిరరాశి { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
మూస <typename T, శూన్య deallocate(శూన్య *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  బహిరంగ:
    Deleter(స్థిరరాశి std::size_t కొలత_2) : కొలత(కొలత_2) {}
    శూన్య క్రియాకారకం()(T *స్థిరరాశి ptr) స్థిరరాశి {
        కొరకు (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(కొలత) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  అంతరంగ:
    స్థిరరాశి std::size_t కొలత;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
మూస <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#లేకపోతే

using std::unique_ptr;

#అయితే_ముగింపు

/**
 * Helper for make_unique<T, Args...>; forwards all but the last of args to the constructor of
 * T, uses the last of args as the error code for allocation errors.
 *
 * As usual, does nothing if the incoming UErrorCode already is U_FAILURE.
 *
 * @internal
 */
మూస <typename T, typename... వాదనలు, std::size_t... i>
unique_ptr<T> make_unique_impl(std::tuple<వాదనలు...> వాదనలు_2,
                               std::సూచీ_sequence<i...> constructor_arg_indices) {
    static_assert(std::is_same_v<decltype(constructor_arg_indices),
                                 std::make_సూచీ_sequence<పరిమాణం...(వాదనలు) - 1>>);
    స్వయంచాలక &&error = std::get<పరిమాణం...(వాదనలు) - 1>(వాదనలు_2);
    static_assert(std::is_same_v<decltype(error), UError_సంకేతం &>);
    ఒకవేళ (U_FAILURE(error)) {
        తిరిగి nullptr;
    }
#ఒకవేళ U_OVERRIDE_CXX_కేటాయింపు
    శూన్య *స్థిరరాశి బఫరు = uprv_malloc(పరిమాణం(T));
    ఒకవేళ (బఫరు == nullptr) {
        error = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి nullptr;
    }
    తిరిగి unique_ptr<T>(new (బఫరు) T(std::get<i>(వాదనలు_2)...));
#లేకపోతే
    తిరిగి std::make_unique<T>(std::get<i>(వాదనలు_2)...);
#అయితే_ముగింపు
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
మూస <typename T, typename... వాదనలు, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique(వాదనలు &&...వాదనలు_2) {
    తిరిగి make_unique_impl<T>(std::forward_as_tuple(వాదనలు_2...),
                               std::make_సూచీ_sequence<పరిమాణం...(వాదనలు) - 1>());
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
మూస <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique(స్థిరరాశి std::size_t n, UError_సంకేతం &error) {
    ఒకవేళ (U_FAILURE(error)) {
        తిరిగి nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_కొలత = SIZE_MAX / పరిమాణం(std::remove_extent_t<T>);
    ఒకవేళ (n > max_కొలత) {
        error = U_ILLEGAL_వాదం_ERROR;
        తిరిగి nullptr;
    }
#ఒకవేళ U_OVERRIDE_CXX_కేటాయింపు
    శూన్య *స్థిరరాశి బఫరు = uprv_malloc(పరిమాణం(std::remove_extent_t<T>) * n);
    ఒకవేళ (బఫరు == nullptr) {
        error = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        ఒకవేళ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            తిరిగి nullptr;
        } లేకపోతే {
            తిరిగి unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    ఒకవేళ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        తిరిగి unique_ptr<T>(new (బఫరు) std::remove_extent_t<T>[n]());
    } లేకపోతే {
        తిరిగి unique_ptr<T>(new (బఫరు) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#లేకపోతే
    తిరిగి std::make_unique<T>(n);
#అయితే_ముగింపు
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
మూస <typename T, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique_for_overwrite(UError_సంకేతం &error) {
    ఒకవేళ (U_FAILURE(error)) {
        తిరిగి nullptr;
    }
#ఒకవేళ U_OVERRIDE_CXX_కేటాయింపు
    శూన్య *స్థిరరాశి బఫరు = uprv_malloc(పరిమాణం(T));
    ఒకవేళ (బఫరు == nullptr) {
        error = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి nullptr;
    }
    తిరిగి unique_ptr<T>(new (బఫరు) T);
#లేదావేళ U_CPLUSPLUS_సంస్కరణ >= 20
    తిరిగి std::make_unique_for_overwrite<T>();
#లేకపోతే
    తిరిగి unique_ptr<T>(new T);
#అయితే_ముగింపు
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
మూస <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique_for_overwrite(స్థిరరాశి std::size_t n, UError_సంకేతం &error) {
    ఒకవేళ (U_FAILURE(error)) {
        తిరిగి nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::size_t max_కొలత = SIZE_MAX / పరిమాణం(std::remove_extent_t<T>);
    ఒకవేళ (n > max_కొలత) {
        error = U_ILLEGAL_వాదం_ERROR;
        తిరిగి nullptr;
    }
#ఒకవేళ U_OVERRIDE_CXX_కేటాయింపు
    శూన్య *స్థిరరాశి బఫరు = uprv_malloc(పరిమాణం(std::remove_extent_t<T>) * n);
    ఒకవేళ (బఫరు == nullptr) {
        error = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        ఒకవేళ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            తిరిగి nullptr;
        } లేకపోతే {
            తిరిగి unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    ఒకవేళ constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        తిరిగి unique_ptr<T>(new (బఫరు) std::remove_extent_t<T>[n]);
    } లేకపోతే {
        తిరిగి unique_ptr<T>(new (బఫరు) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#లేదావేళ U_CPLUSPLUS_సంస్కరణ >= 20
    తిరిగి std::make_unique_for_overwrite<T>(n);
#లేకపోతే
    తిరిగి unique_ptr<T>(new std::remove_extent_t<T>[n]);
#అయితే_ముగింపు
}

}  // namespace prv

U_NAMESPACE_చివర

#అయితే_ముగింపు  /* __cplusplus */
#అయితే_ముగింపు  /* CMEMORY_H */
