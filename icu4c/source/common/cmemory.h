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

#अन्तर्भाव "unicode/utypes.h"

#अन्तर्भाव <limits.h>
#अन्तर्भाव <stddef.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "uassert.h"

#जर U_DEBUG && defined(UPRV_MALLOC_मोजणी)
#अन्तर्भाव <stdio.h>
#endif

// uprv_memcpy and uprv_memmove
#जर defined(__clang__)
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE स_नकल(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE स_हटाओ(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#किंवाजर defined(__GNUC__)
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE स_नकल(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE स_हटाओ(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#अन्यथा
#define uprv_memcpy(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    U_STANDARD_CPP_NAMESPACE स_नकल(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#define uprv_memmove(dst, src, size) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != रिक्त); \
    U_ASSERT(src != रिक्त); \
    U_STANDARD_CPP_NAMESPACE स_हटाओ(dst, src, size); \
} UPRV_BLOCK_MACRO_END
#endif

/**
 * \def UPRV_LENGTHOF
 * Convenience macro to determine the length of a fixed array at compile-time.
 * @param array A fixed length array
 * @return The length of the array, in elements
 * @internal
 */
#define UPRV_LENGTHOF(array) (int32_t)(आकार(array)/आकार((array)[0]))
#define uprv_memset(buffer, mark, size) U_STANDARD_CPP_NAMESPACE स_रखो(buffer, mark, size)
#define uprv_memcmp(buffer1, buffer2, size) U_STANDARD_CPP_NAMESPACE स_भेद(buffer1, buffer2,size)
#define uprv_memchr(ptr, मूल्य_2, अंक) U_STANDARD_CPP_NAMESPACE स_प्रथम(ptr, मूल्य_2, अंक)

U_CAPI शून्य * U_EXPORT2
uprv_malloc(माप_प्रकार s) U_MALLOC_ATTR U_ALLOC_SIZE_ATTR(1);

U_CAPI शून्य * U_EXPORT2
uprv_realloc(शून्य *mem, माप_प्रकार size) U_ALLOC_SIZE_ATTR(2);

U_CAPI शून्य U_EXPORT2
uprv_free(शून्य *mem);

U_CAPI शून्य * U_EXPORT2
uprv_calloc(माप_प्रकार अंक, माप_प्रकार size) U_MALLOC_ATTR U_ALLOC_SIZE_ATTR2(1,2);

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
    alignas(type) स्थिर अक्षर storage[आकार(type)]; \
    परत new(storage) type();} ()

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
प्रकारव्याख्या शून्य U_CALLCONV UObjectDeleter(शून्य* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI शून्य U_EXPORT2
uprv_deleteUObject(शून्य *obj);

#ifdef __cplusplus

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
ढाँचा<typename T>
class LocalMemory : सार्वजनिक LocalPointerBase<T> {
सार्वजनिक:
    using LocalPointerBase<T>::चालक*;
    using LocalPointerBase<T>::चालक->;
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
    LocalMemory<T>& चालक=(LocalMemory<T>&& src) noexcept U_LIFETIME_BOUND {
        uprv_free(LocalPointerBase<T>::ptr);
        LocalPointerBase<T>::ptr=src.ptr;
        src.ptr=nullptr;
        परत *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    शून्य swap(LocalMemory<T> &other) noexcept {
        T *temp=LocalPointerBase<T>::ptr;
        LocalPointerBase<T>::ptr=other.ptr;
        other.ptr=temp;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    मित्र ओळीत शून्य swap(LocalMemory<T> &p1, LocalMemory<T> &p2) noexcept {
        p1.swap(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    शून्य adoptInstead(T *p) {
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
    ओळीत T* allocateInsteadAndReset(int32_t newCapacity = 1) U_LIFETIME_BOUND;
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
    ओळीत T* allocateInsteadAndCopy(int32_t newCapacity = 1, int32_t length = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& चालक[](सूचक_भेद_प्रकार i) स्थिरांक U_LIFETIME_BOUND { परत LocalPointerBase<T>::ptr[i]; }
};

ढाँचा <typename T>
ओळीत T* LocalMemory<T>::allocateInsteadAndReset(int32_t newCapacity) U_LIFETIME_BOUND {
    जर(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*आकार(T));
        जर(p!=nullptr) {
            uprv_memset(p, 0, newCapacity*आकार(T));
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        परत p;
    } अन्यथा {
        परत nullptr;
    }
}

ढाँचा <typename T>
ओळीत T* LocalMemory<T>::allocateInsteadAndCopy(int32_t newCapacity,
                                                 int32_t length) U_LIFETIME_BOUND {
    जर(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*आकार(T));
        जर(p!=nullptr) {
            जर(length>0) {
                जर(length>newCapacity) {
                    length=newCapacity;
                }
                uprv_memcpy(p, LocalPointerBase<T>::ptr, (माप_प्रकार)length*आकार(T));
            }
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        परत p;
    } अन्यथा {
        परत nullptr;
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
ढाँचा<typename T, int32_t चळत_Capacity>
class Maybe_चळत_Array {
सार्वजनिक:
    // No heap allocation. Use only on the stack.
    स्थिर शून्य* U_EXPORT2 चालक new(माप_प्रकार) noexcept = delete;
    स्थिर शून्य* U_EXPORT2 चालक new[](माप_प्रकार) noexcept = delete;
    स्थिर शून्य* U_EXPORT2 चालक new(माप_प्रकार, शून्य*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_चळत_Array() : ptr(चळत_Array), capacity(चळत_Capacity), needToRelease(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_चळत_Array(int32_t newCapacity, UErrorCode status) : Maybe_चळत_Array() {
        जर (U_FAILURE(status)) {
            परत;
        }
        जर (capacity < newCapacity) {
            जर (resize(newCapacity) == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }
    /**
     * Destructor deletes the array (if owned).
     */
    ~Maybe_चळत_Array() { releaseArray(); }
    /**
     * Move constructor: transfers ownership or copies the stack array.
     */
    Maybe_चळत_Array(Maybe_चळत_Array<T, चळत_Capacity> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_चळत_Array<T, चळत_Capacity>&
    चालक=(Maybe_चळत_Array<T, चळत_Capacity>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() स्थिरांक { परत capacity; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() स्थिरांक U_LIFETIME_BOUND { परत ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* getArrayLimit() स्थिरांक U_LIFETIME_BOUND { परत getAlias() + capacity; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    स्थिरांक T& चालक[](सूचक_भेद_प्रकार i) स्थिरांक U_LIFETIME_BOUND { परत ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& चालक[](सूचक_भेद_प्रकार i) U_LIFETIME_BOUND { परत ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    शून्य aliasInstead(T *otherArray, int32_t otherCapacity) {
        जर(otherArray!=nullptr && otherCapacity>0) {
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
    ओळीत T* resize(int32_t newCapacity, int32_t length = 0) U_LIFETIME_BOUND;
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
    ओळीत T *orphanOrClone(int32_t length, int32_t &resultCapacity);

संरक्षित:
    // Resizes the array to the size of src, then copies the contents of src.
    शून्य copyFrom(स्थिरांक Maybe_चळत_Array &src, UErrorCode &status) {
        जर (U_FAILURE(status)) {
            परत;
        }
        जर (this->resize(src.capacity, 0) == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            परत;
        }
        uprv_memcpy(this->ptr, src.ptr, (माप_प्रकार)capacity * आकार(T));
    }

खाजगी:
    T *ptr;
    int32_t capacity;
    UBool needToRelease;
    T चळत_Array[चळत_Capacity];
    शून्य releaseArray() {
        जर(needToRelease) {
            uprv_free(ptr);
        }
    }
    शून्य reset_To_चळत_Array() {
        ptr=चळत_Array;
        capacity=चळत_Capacity;
        needToRelease=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool चालक==(स्थिरांक Maybe_चळत_Array & /*other*/) = delete;
    bool चालक!=(स्थिरांक Maybe_चळत_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_चळत_Array(स्थिरांक Maybe_चळत_Array & /*other*/) = delete;
    शून्य चालक=(स्थिरांक Maybe_चळत_Array & /*other*/) = delete;
};

ढाँचा<typename T, int32_t चळत_Capacity>
icu::Maybe_चळत_Array<T, चळत_Capacity>::Maybe_चळत_Array(
        Maybe_चळत_Array <T, चळत_Capacity>&& src) noexcept
        : ptr(src.ptr), capacity(src.capacity), needToRelease(src.needToRelease) {
    जर (src.ptr == src.चळत_Array) {
        ptr = चळत_Array;
        uprv_memcpy(चळत_Array, src.चळत_Array, आकार(T) * src.capacity);
    } अन्यथा {
        src.reset_To_चळत_Array();  // take ownership away from src
    }
}

ढाँचा <typename T, int32_t चळत_Capacity>
ओळीत Maybe_चळत_Array<T, चळत_Capacity>& Maybe_चळत_Array<T, चळत_Capacity>::चालक=(
    Maybe_चळत_Array<T, चळत_Capacity>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    capacity = src.capacity;
    needToRelease = src.needToRelease;
    जर (src.ptr == src.चळत_Array) {
        ptr = चळत_Array;
        uprv_memcpy(चळत_Array, src.चळत_Array, आकार(T) * src.capacity);
    } अन्यथा {
        ptr = src.ptr;
        src.reset_To_चळत_Array();  // take ownership away from src
    }
    परत *this;
}

ढाँचा <typename T, int32_t चळत_Capacity>
ओळीत T* Maybe_चळत_Array<T, चळत_Capacity>::resize(int32_t newCapacity,
                                                    int32_t length) U_LIFETIME_BOUND {
    जर(newCapacity>0) {
#जर U_DEBUG && defined(UPRV_MALLOC_मोजणी)
        ::ख_लिखो(::मानक_त्रुटि, "MaybeStackArray (resize) alloc %d * %lu\n", newCapacity, आकार(T));
#endif
        T *p=(T *)uprv_malloc(newCapacity*आकार(T));
        जर(p!=nullptr) {
            जर(length>0) {
                जर(length>capacity) {
                    length=capacity;
                }
                जर(length>newCapacity) {
                    length=newCapacity;
                }
                uprv_memcpy(p, ptr, (माप_प्रकार)length*आकार(T));
            }
            releaseArray();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        परत p;
    } अन्यथा {
        परत nullptr;
    }
}

ढाँचा<typename T, int32_t चळत_Capacity>
ओळीत T *Maybe_चळत_Array<T, चळत_Capacity>::orphanOrClone(int32_t length, int32_t &resultCapacity) {
    T *p;
    जर(needToRelease) {
        p=ptr;
    } अन्यथा जर(length<=0) {
        परत nullptr;
    } अन्यथा {
        जर(length>capacity) {
            length=capacity;
        }
        p=(T *)uprv_malloc(length*आकार(T));
#जर U_DEBUG && defined(UPRV_MALLOC_मोजणी)
      ::ख_लिखो(::मानक_त्रुटि,"MaybeStacArray (orphan) alloc %d * %lu\n", length,आकार(T));
#endif
        जर(p==nullptr) {
            परत nullptr;
        }
        uprv_memcpy(p, ptr, (माप_प्रकार)length*आकार(T));
    }
    resultCapacity=length;
    reset_To_चळत_Array();
    परत p;
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
ढाँचा<typename H, typename T, int32_t चळत_Capacity>
class Maybe_चळत_Header_And_Array {
सार्वजनिक:
    // No heap allocation. Use only on the stack.
    स्थिर शून्य* U_EXPORT2 चालक new(माप_प्रकार) noexcept = delete;
    स्थिर शून्य* U_EXPORT2 चालक new[](माप_प्रकार) noexcept = delete;
    स्थिर शून्य* U_EXPORT2 चालक new(माप_प्रकार, शून्य*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_चळत_Header_And_Array() : ptr(&चळत_Header), capacity(चळत_Capacity), needToRelease(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_चळत_Header_And_Array() { releaseMemory(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() स्थिरांक { परत capacity; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() स्थिरांक U_LIFETIME_BOUND { परत ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* getArrayStart() स्थिरांक U_LIFETIME_BOUND { परत reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* getArrayLimit() स्थिरांक U_LIFETIME_BOUND { परत getArrayStart() + capacity; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    चालक H*() स्थिरांक U_LIFETIME_BOUND { परत ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& चालक[](सूचक_भेद_प्रकार i) U_LIFETIME_BOUND { परत getArrayStart()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    शून्य aliasInstead(H *otherMemory, int32_t otherCapacity) {
        जर(otherMemory!=nullptr && otherCapacity>0) {
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
    ओळीत H* resize(int32_t newCapacity, int32_t length = 0) U_LIFETIME_BOUND;
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
    ओळीत H *orphanOrClone(int32_t length, int32_t &resultCapacity);
खाजगी:
    H *ptr;
    int32_t capacity;
    UBool needToRelease;
    // stackHeader must precede stackArray immediately.
    H चळत_Header;
    T चळत_Array[चळत_Capacity];
    शून्य releaseMemory() {
        जर(needToRelease) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool चालक==(स्थिरांक Maybe_चळत_Header_And_Array & /*other*/) {परत false;}
    bool चालक!=(स्थिरांक Maybe_चळत_Header_And_Array & /*other*/) {परत true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_चळत_Header_And_Array(स्थिरांक Maybe_चळत_Header_And_Array & /*other*/) {}
    शून्य चालक=(स्थिरांक Maybe_चळत_Header_And_Array & /*other*/) {}
};

ढाँचा <typename H, typename T, int32_t चळत_Capacity>
ओळीत H* Maybe_चळत_Header_And_Array<H, T, चळत_Capacity>::resize(int32_t newCapacity,
                                                                int32_t length) U_LIFETIME_BOUND {
    जर(newCapacity>=0) {
#जर U_DEBUG && defined(UPRV_MALLOC_मोजणी)
      ::ख_लिखो(::मानक_त्रुटि,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", आकार(H),newCapacity,आकार(T));
#endif
        H *p=(H *)uprv_malloc(आकार(H)+newCapacity*आकार(T));
        जर(p!=nullptr) {
            जर(length<0) {
                length=0;
            } अन्यथा जर(length>0) {
                जर(length>capacity) {
                    length=capacity;
                }
                जर(length>newCapacity) {
                    length=newCapacity;
                }
            }
            uprv_memcpy(p, ptr, आकार(H)+(माप_प्रकार)length*आकार(T));
            releaseMemory();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        परत p;
    } अन्यथा {
        परत nullptr;
    }
}

ढाँचा<typename H, typename T, int32_t चळत_Capacity>
ओळीत H *Maybe_चळत_Header_And_Array<H, T, चळत_Capacity>::orphanOrClone(int32_t length,
                                                                       int32_t &resultCapacity) {
    H *p;
    जर(needToRelease) {
        p=ptr;
    } अन्यथा {
        जर(length<0) {
            length=0;
        } अन्यथा जर(length>capacity) {
            length=capacity;
        }
#जर U_DEBUG && defined(UPRV_MALLOC_मोजणी)
      ::ख_लिखो(::मानक_त्रुटि,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", आकार(H),length,आकार(T));
#endif
        p=(H *)uprv_malloc(आकार(H)+length*आकार(T));
        जर(p==nullptr) {
            परत nullptr;
        }
        uprv_memcpy(p, ptr, आकार(H)+(माप_प्रकार)length*आकार(T));
    }
    resultCapacity=length;
    ptr=&चळत_Header;
    capacity=चळत_Capacity;
    needToRelease=false;
    परत p;
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
ढाँचा<typename T, int32_t चळत_Capacity = 8>
class MemoryPool : सार्वजनिक UMemory {
सार्वजनिक:
    MemoryPool() : f_मोजणी(0), fPool() {}

    ~MemoryPool() {
        साठी (int32_t i = 0; i < f_मोजणी; ++i) {
            delete fPool[i];
        }
    }

    MemoryPool(स्थिरांक MemoryPool&) = delete;
    MemoryPool& चालक=(स्थिरांक MemoryPool&) = delete;

    MemoryPool(MemoryPool&& other) noexcept : f_मोजणी(other.f_मोजणी),
                                                fPool(std::move(other.fPool)) {
        other.f_मोजणी = 0;
    }

    MemoryPool& चालक=(MemoryPool&& other) noexcept U_LIFETIME_BOUND {
        // Since `this` may contain instances that need to be deleted, we can't
        // just throw them away and replace them with `other`. The normal way of
        // dealing with this in C++ is to swap `this` and `other`, rather than
        // simply overwrite: the destruction of `other` can then take care of
        // running MemoryPool::~MemoryPool() over the still-to-be-deallocated
        // instances.
        std::swap(f_मोजणी, other.f_मोजणी);
        std::swap(fPool, other.fPool);
        परत *this;
    }

    /**
     * Creates a new object of typename T, by forwarding any and all arguments
     * to the typename T constructor.
     *
     * @param args Arguments to be forwarded to the typename T constructor.
     * @return A pointer to the newly created object, or nullptr on error.
     */
    ढाँचा <typename... Args>
    T* create(Args&&... args) U_LIFETIME_BOUND {
        int32_t capacity = fPool.getCapacity();
        जर (f_मोजणी == capacity &&
            fPool.resize(capacity == चळत_Capacity ? 4 * capacity : 2 * capacity,
                         capacity) == nullptr) {
            परत nullptr;
        }
        परत fPool[f_मोजणी++] = new T(std::forward<Args>(args)...);
    }

    ढाँचा <typename... Args>
    T* createAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        जर (U_FAILURE(status)) {
            परत nullptr;
        }
        T *pointer = this->create(args...);
        जर (U_SUCCESS(status) && pointer == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        परत pointer;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t मोजणी() स्थिरांक {
        परत f_मोजणी;
    }

संरक्षित:
    int32_t f_मोजणी;
    Maybe_चळत_Array<T*, चळत_Capacity> fPool;
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
ढाँचा<typename T, int32_t चळत_Capacity = 8>
class Maybe_चळत_Vector : संरक्षित MemoryPool<T, चळत_Capacity> {
सार्वजनिक:
    ढाँचा <typename... Args>
    T* emplaceBack(Args&&... args) U_LIFETIME_BOUND {
        परत this->create(args...);
    }

    ढाँचा <typename... Args>
    T* emplaceBackAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        परत this->createAndCheckErrorCode(status, args...);
    }

    int32_t length() स्थिरांक {
        परत this->f_मोजणी;
    }

    T** getAlias() U_LIFETIME_BOUND {
        परत this->fPool.getAlias();
    }

    स्थिरांक T* स्थिरांक* getAlias() स्थिरांक U_LIFETIME_BOUND {
        परत this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    स्थिरांक T* चालक[](सूचक_भेद_प्रकार i) स्थिरांक U_LIFETIME_BOUND {
        परत this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* चालक[](सूचक_भेद_प्रकार i) U_LIFETIME_BOUND {
        परत this->fPool[i];
    }
};

namespace prv {

#जर U_OVERRIDE_CXX_ALLOCATION

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
ढाँचा <typename T, शून्य deallocate(शून्य *), typename = शून्य> class Deleter {
  सार्वजनिक:
    Deleter() = पूर्वनिर्धारित;
    शून्य चालक()(T *स्थिरांक ptr) स्थिरांक {
        जर constexpr (!std::is_trivially_destructible_v<T>) {
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
ढाँचा <typename T, शून्य deallocate(शून्य *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  सार्वजनिक:
    Deleter() = पूर्वनिर्धारित;
    शून्य चालक()(T *स्थिरांक ptr) स्थिरांक { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
ढाँचा <typename T, शून्य deallocate(शून्य *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  सार्वजनिक:
    Deleter(स्थिरांक std::माप_प्रकार size) : size_(size) {}
    शून्य चालक()(T *स्थिरांक ptr) स्थिरांक {
        साठी (std::सूचक_भेद_प्रकार i = static_cast<std::सूचक_भेद_प्रकार>(size_) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  खाजगी:
    स्थिरांक std::माप_प्रकार size_;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
ढाँचा <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#अन्यथा

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
ढाँचा <typename T, typename... Args, std::माप_प्रकार... i>
unique_ptr<T> make_unique_impl(std::tuple<Args...> args,
                               std::index_sequence<i...> constructor_arg_indices) {
    static_assert(std::is_same_v<decltype(constructor_arg_indices),
                                 std::make_index_sequence<आकार...(Args) - 1>>);
    स्वतः &&error = std::get<आकार...(Args) - 1>(args);
    static_assert(std::is_same_v<decltype(error), UErrorCode &>);
    जर (U_FAILURE(error)) {
        परत nullptr;
    }
#जर U_OVERRIDE_CXX_ALLOCATION
    शून्य *स्थिरांक buffer = uprv_malloc(आकार(T));
    जर (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    परत unique_ptr<T>(new (buffer) T(std::get<i>(args)...));
#अन्यथा
    परत std::make_unique<T>(std::get<i>(args)...);
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
ढाँचा <typename T, typename... Args, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique(Args &&...args) {
    परत make_unique_impl<T>(std::forward_as_tuple(args...),
                               std::make_index_sequence<आकार...(Args) - 1>());
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
ढाँचा <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique(स्थिरांक std::माप_प्रकार n, UErrorCode &error) {
    जर (U_FAILURE(error)) {
        परत nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::माप_प्रकार maxSize = SIZE_MAX / आकार(std::remove_extent_t<T>);
    जर (n > maxSize) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
#जर U_OVERRIDE_CXX_ALLOCATION
    शून्य *स्थिरांक buffer = uprv_malloc(आकार(std::remove_extent_t<T>) * n);
    जर (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        जर constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            परत nullptr;
        } अन्यथा {
            परत unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    जर constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        परत unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n]());
    } अन्यथा {
        परत unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#अन्यथा
    परत std::make_unique<T>(n);
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
ढाँचा <typename T, typename = std::enable_if_t<!std::is_array_v<T>>>
unique_ptr<T> make_unique_for_overwrite(UErrorCode &error) {
    जर (U_FAILURE(error)) {
        परत nullptr;
    }
#जर U_OVERRIDE_CXX_ALLOCATION
    शून्य *स्थिरांक buffer = uprv_malloc(आकार(T));
    जर (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    परत unique_ptr<T>(new (buffer) T);
#किंवाजर U_CPLUSPLUS_VERSION >= 20
    परत std::make_unique_for_overwrite<T>();
#अन्यथा
    परत unique_ptr<T>(new T);
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
ढाँचा <typename T,
          // In C++20, this could require is_unbounded_array_v<T>.
          typename = std::enable_if_t<std::is_same_v<std::remove_extent_t<T>[], T>>>
unique_ptr<T> make_unique_for_overwrite(स्थिरांक std::माप_प्रकार n, UErrorCode &error) {
    जर (U_FAILURE(error)) {
        परत nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::माप_प्रकार maxSize = SIZE_MAX / आकार(std::remove_extent_t<T>);
    जर (n > maxSize) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
#जर U_OVERRIDE_CXX_ALLOCATION
    शून्य *स्थिरांक buffer = uprv_malloc(आकार(std::remove_extent_t<T>) * n);
    जर (buffer == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        जर constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            परत nullptr;
        } अन्यथा {
            परत unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    जर constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        परत unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n]);
    } अन्यथा {
        परत unique_ptr<T>(new (buffer) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#किंवाजर U_CPLUSPLUS_VERSION >= 20
    परत std::make_unique_for_overwrite<T>(n);
#अन्यथा
    परत unique_ptr<T>(new std::remove_extent_t<T>[n]);
#endif
}

}  // namespace prv

U_NAMESPACE_END

#endif  /* __cplusplus */
#endif  /* CMEMORY_H */
