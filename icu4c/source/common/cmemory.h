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

#अगर_अघोषित CMEMORY_H
#घोषणा CMEMORY_H

#समावेश "unicode/utypes.h"

#समावेश <limits.h>
#समावेश <stddef.h>
#समावेश <string.h>
#समावेश "unicode/localpointer.h"
#समावेश "uassert.h"

#अगर U_DEBUG && defined(UPRV_MALLOC_गिनती)
#समावेश <stdio.h>
#पूर्ण_अगर

// uprv_memcpy and uprv_memmove
#अगर defined(__clang__)
#घोषणा uprv_memcpy(dst, src, आकार_2) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != शून्य); \
    U_ASSERT(src != शून्य); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE स_नकल(dst, src, आकार_2); \
} UPRV_BLOCK_MACRO_END
#घोषणा uprv_memmove(dst, src, आकार_2) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != शून्य); \
    U_ASSERT(src != शून्य); \
    _Pragma("clang diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE स_हटाओ(dst, src, आकार_2); \
} UPRV_BLOCK_MACRO_END
#या_अगर defined(__GNUC__)
#घोषणा uprv_memcpy(dst, src, आकार_2) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != शून्य); \
    U_ASSERT(src != शून्य); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE स_नकल(dst, src, आकार_2); \
} UPRV_BLOCK_MACRO_END
#घोषणा uprv_memmove(dst, src, आकार_2) UPRV_BLOCK_MACRO_BEGIN { \
    /* Suppress warnings about addresses that will never be NULL */ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Waddress\"") \
    U_ASSERT(dst != शून्य); \
    U_ASSERT(src != शून्य); \
    _Pragma("GCC diagnostic pop") \
    U_STANDARD_CPP_NAMESPACE स_हटाओ(dst, src, आकार_2); \
} UPRV_BLOCK_MACRO_END
#अन्यथा
#घोषणा uprv_memcpy(dst, src, आकार_2) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != शून्य); \
    U_ASSERT(src != शून्य); \
    U_STANDARD_CPP_NAMESPACE स_नकल(dst, src, आकार_2); \
} UPRV_BLOCK_MACRO_END
#घोषणा uprv_memmove(dst, src, आकार_2) UPRV_BLOCK_MACRO_BEGIN { \
    U_ASSERT(dst != शून्य); \
    U_ASSERT(src != शून्य); \
    U_STANDARD_CPP_NAMESPACE स_हटाओ(dst, src, आकार_2); \
} UPRV_BLOCK_MACRO_END
#पूर्ण_अगर

/**
 * \def UPRV_LENGTHOF
 * Convenience macro to determine the length of a fixed array at compile-time.
 * @param array A fixed length array
 * @return The length of the array, in elements
 * @internal
 */
#घोषणा UPRV_LENGTHOF(array) (int32_t)(माप(array)/माप((array)[0]))
#घोषणा uprv_memset(बफर, mark, आकार_2) U_STANDARD_CPP_NAMESPACE स_रखो(बफर, mark, आकार_2)
#घोषणा uprv_memcmp(buffer1, buffer2, आकार_2) U_STANDARD_CPP_NAMESPACE स_भेद(buffer1, buffer2,आकार_2)
#घोषणा uprv_memchr(ptr, मूल्य_2, अंक) U_STANDARD_CPP_NAMESPACE स_प्रथम(ptr, मूल्य_2, अंक)

U_CAPI व्योम * U_EXPORT2
uprv_malloc(माप_प्रकार s) U_MALLOC_ATTR U_ALLOC_आकार_ATTR(1);

U_CAPI व्योम * U_EXPORT2
uprv_realloc(व्योम *mem, माप_प्रकार आकार_2) U_ALLOC_आकार_ATTR(2);

U_CAPI व्योम U_EXPORT2
uprv_free(व्योम *mem);

U_CAPI व्योम * U_EXPORT2
uprv_calloc(माप_प्रकार अंक, माप_प्रकार आकार_2) U_MALLOC_ATTR U_ALLOC_आकार_ATTR2(1,2);

/**
 * Get the least significant bits of a pointer (a memory address).
 * For example, with a mask of 3, the macro gets the 2 least significant bits,
 * which will be 0 if the pointer is 32-bit (4-byte) aligned.
 *
 * uintptr_t is the most appropriate integer type to cast to.
 */
#घोषणा U_POINTER_MASK_LSB(ptr, mask) ((uintptr_t)(ptr) & (mask))

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
#घोषणा STATIC_NEW(type) [] () { \
    alignas(type) अटल अक्षर storage[माप(type)]; \
    वापस new(storage) type();} ()

/**
  *  Heap clean up function, called from u_cleanup()
  *    Clears any user heap functions from u_setMemoryFunctions()
  *    Does NOT deallocate any remaining allocated memory.
  */
U_CFUNC UBool 
cmemory_cleanup(व्योम);

/**
 * A function called by <TT>uhash_remove</TT>,
 * <TT>uhash_close</TT>, or <TT>uhash_put</TT> to delete
 * an existing key or value.
 * @param obj A key or value stored in a hashtable
 * @see uprv_deleteUObject
 */
प्रकार व्योम U_CALLCONV UObjectDeleter(व्योम* obj);

/**
 * Deleter for UObject instances.
 * Works for all subclasses of UObject because it has a virtual destructor.
 */
U_CAPI व्योम U_EXPORT2
uprv_deleteUObject(व्योम *obj);

#अगर_घोषित __cplusplus

#समावेश <memory>
#समावेश <utility>
#समावेश <type_traits>
#समावेश "unicode/uobject.h"

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
class LocalMemory : खुला LocalPointerBase<T> {
खुला:
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
        वापस *this;
    }
    /**
     * Swap pointers.
     * @param other other smart pointer
     */
    व्योम swap(LocalMemory<T> &other) noexcept {
        T *अस्थायी=LocalPointerBase<T>::ptr;
        LocalPointerBase<T>::ptr=other.ptr;
        other.ptr=अस्थायी;
    }
    /**
     * Non-member LocalMemory swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     */
    मित्र अंतरभूत व्योम swap(LocalMemory<T> &p1, LocalMemory<T> &p2) noexcept {
        p1.swap(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T items that is adopted
     */
    व्योम adoptInstead(T *p) {
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
    अंतरभूत T* allocateInsteadAndReset(int32_t newCapacity = 1) U_LIFETIME_BOUND;
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
    अंतरभूत T* allocateInsteadAndCopy(int32_t newCapacity = 1, int32_t लंबाई = 0) U_LIFETIME_BOUND;
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& चालक[](सूचक_भेद_प्रकार i) स्थिर U_LIFETIME_BOUND { वापस LocalPointerBase<T>::ptr[i]; }
};

ढाँचा <typename T>
अंतरभूत T* LocalMemory<T>::allocateInsteadAndReset(int32_t newCapacity) U_LIFETIME_BOUND {
    अगर(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*माप(T));
        अगर(p!=nullptr) {
            uprv_memset(p, 0, newCapacity*माप(T));
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        वापस p;
    } अन्यथा {
        वापस nullptr;
    }
}

ढाँचा <typename T>
अंतरभूत T* LocalMemory<T>::allocateInsteadAndCopy(int32_t newCapacity,
                                                 int32_t लंबाई) U_LIFETIME_BOUND {
    अगर(newCapacity>0) {
        T *p=(T *)uprv_malloc(newCapacity*माप(T));
        अगर(p!=nullptr) {
            अगर(लंबाई>0) {
                अगर(लंबाई>newCapacity) {
                    लंबाई=newCapacity;
                }
                uprv_memcpy(p, LocalPointerBase<T>::ptr, (माप_प्रकार)लंबाई*माप(T));
            }
            uprv_free(LocalPointerBase<T>::ptr);
            LocalPointerBase<T>::ptr=p;
        }
        वापस p;
    } अन्यथा {
        वापस nullptr;
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
ढाँचा<typename T, int32_t ढेर_Capacity>
class Maybe_ढेर_Array {
खुला:
    // No heap allocation. Use only on the stack.
    अटल व्योम* U_EXPORT2 चालक new(माप_प्रकार) noexcept = delete;
    अटल व्योम* U_EXPORT2 चालक new[](माप_प्रकार) noexcept = delete;
    अटल व्योम* U_EXPORT2 चालक new(माप_प्रकार, व्योम*) noexcept = delete;

    /**
     * Default constructor initializes with internal T[stackCapacity] buffer.
     */
    Maybe_ढेर_Array() : ptr(ढेर_Array), capacity(ढेर_Capacity), needToRelease(false) {}
    /**
     * Automatically allocates the heap array if the argument is larger than the stack capacity.
     * Intended for use when an approximate capacity is known at compile time but the true
     * capacity is not known until runtime.
     */
    Maybe_ढेर_Array(int32_t newCapacity, UErrorCode status) : Maybe_ढेर_Array() {
        अगर (U_FAILURE(status)) {
            वापस;
        }
        अगर (capacity < newCapacity) {
            अगर (resize(newCapacity) == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }
    /**
     * Destructor deletes the array (if owned).
     */
    ~Maybe_ढेर_Array() { releaseArray(); }
    /**
     * Move constructor: transfers ownership or copies the stack array.
     */
    Maybe_ढेर_Array(Maybe_ढेर_Array<T, ढेर_Capacity> &&src) noexcept;
    /**
     * Move assignment: transfers ownership or copies the stack array.
     */
    Maybe_ढेर_Array<T, ढेर_Capacity>&
    चालक=(Maybe_ढेर_Array<T, ढेर_Capacity>&& src) noexcept U_LIFETIME_BOUND;
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() स्थिर { वापस capacity; }
    /**
     * Access without ownership change.
     * @return the array pointer
     */
    T* getAlias() स्थिर U_LIFETIME_BOUND { वापस ptr; }
    /**
     * Returns the array limit. Simple convenience method.
     * @return getAlias()+getCapacity()
     */
    T* getArrayLimit() स्थिर U_LIFETIME_BOUND { वापस getAlias() + capacity; }
    // No "operator T *() const" because that can make
    // expressions like mbs[index] ambiguous for some compilers.
    /**
     * Array item access (const).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    स्थिर T& चालक[](सूचक_भेद_प्रकार i) स्थिर U_LIFETIME_BOUND { वापस ptr[i]; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& चालक[](सूचक_भेद_प्रकार i) U_LIFETIME_BOUND { वापस ptr[i]; }
    /**
     * Deletes the array (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current array is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    व्योम aliasInstead(T *otherArray, int32_t otherCapacity) {
        अगर(otherArray!=nullptr && otherCapacity>0) {
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
    अंतरभूत T* resize(int32_t newCapacity, int32_t लंबाई = 0) U_LIFETIME_BOUND;
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
    अंतरभूत T *orphanOrClone(int32_t लंबाई, int32_t &परिणाम_Capacity);

रक्षित:
    // Resizes the array to the size of src, then copies the contents of src.
    व्योम copyFrom(स्थिर Maybe_ढेर_Array &src, UErrorCode &status) {
        अगर (U_FAILURE(status)) {
            वापस;
        }
        अगर (this->resize(src.capacity, 0) == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
        uprv_memcpy(this->ptr, src.ptr, (माप_प्रकार)capacity * माप(T));
    }

निजी:
    T *ptr;
    int32_t capacity;
    UBool needToRelease;
    T ढेर_Array[ढेर_Capacity];
    व्योम releaseArray() {
        अगर(needToRelease) {
            uprv_free(ptr);
        }
    }
    व्योम reset_To_ढेर_Array() {
        ptr=ढेर_Array;
        capacity=ढेर_Capacity;
        needToRelease=false;
    }
    /* No comparison operators with other MaybeStackArray's. */
    bool चालक==(स्थिर Maybe_ढेर_Array & /*other*/) = delete;
    bool चालक!=(स्थिर Maybe_ढेर_Array & /*other*/) = delete;
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_ढेर_Array(स्थिर Maybe_ढेर_Array & /*other*/) = delete;
    व्योम चालक=(स्थिर Maybe_ढेर_Array & /*other*/) = delete;
};

ढाँचा<typename T, int32_t ढेर_Capacity>
icu::Maybe_ढेर_Array<T, ढेर_Capacity>::Maybe_ढेर_Array(
        Maybe_ढेर_Array <T, ढेर_Capacity>&& src) noexcept
        : ptr(src.ptr), capacity(src.capacity), needToRelease(src.needToRelease) {
    अगर (src.ptr == src.ढेर_Array) {
        ptr = ढेर_Array;
        uprv_memcpy(ढेर_Array, src.ढेर_Array, माप(T) * src.capacity);
    } अन्यथा {
        src.reset_To_ढेर_Array();  // take ownership away from src
    }
}

ढाँचा <typename T, int32_t ढेर_Capacity>
अंतरभूत Maybe_ढेर_Array<T, ढेर_Capacity>& Maybe_ढेर_Array<T, ढेर_Capacity>::चालक=(
    Maybe_ढेर_Array<T, ढेर_Capacity>&& src) noexcept U_LIFETIME_BOUND {
    releaseArray();  // in case this instance had its own memory allocated
    capacity = src.capacity;
    needToRelease = src.needToRelease;
    अगर (src.ptr == src.ढेर_Array) {
        ptr = ढेर_Array;
        uprv_memcpy(ढेर_Array, src.ढेर_Array, माप(T) * src.capacity);
    } अन्यथा {
        ptr = src.ptr;
        src.reset_To_ढेर_Array();  // take ownership away from src
    }
    वापस *this;
}

ढाँचा <typename T, int32_t ढेर_Capacity>
अंतरभूत T* Maybe_ढेर_Array<T, ढेर_Capacity>::resize(int32_t newCapacity,
                                                    int32_t लंबाई) U_LIFETIME_BOUND {
    अगर(newCapacity>0) {
#अगर U_DEBUG && defined(UPRV_MALLOC_गिनती)
        ::ख_लिखो(::मानक_त्रुटि, "MaybeStackArray (resize) alloc %d * %lu\n", newCapacity, माप(T));
#पूर्ण_अगर
        T *p=(T *)uprv_malloc(newCapacity*माप(T));
        अगर(p!=nullptr) {
            अगर(लंबाई>0) {
                अगर(लंबाई>capacity) {
                    लंबाई=capacity;
                }
                अगर(लंबाई>newCapacity) {
                    लंबाई=newCapacity;
                }
                uprv_memcpy(p, ptr, (माप_प्रकार)लंबाई*माप(T));
            }
            releaseArray();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        वापस p;
    } अन्यथा {
        वापस nullptr;
    }
}

ढाँचा<typename T, int32_t ढेर_Capacity>
अंतरभूत T *Maybe_ढेर_Array<T, ढेर_Capacity>::orphanOrClone(int32_t लंबाई, int32_t &परिणाम_Capacity) {
    T *p;
    अगर(needToRelease) {
        p=ptr;
    } अन्यथा अगर(लंबाई<=0) {
        वापस nullptr;
    } अन्यथा {
        अगर(लंबाई>capacity) {
            लंबाई=capacity;
        }
        p=(T *)uprv_malloc(लंबाई*माप(T));
#अगर U_DEBUG && defined(UPRV_MALLOC_गिनती)
      ::ख_लिखो(::मानक_त्रुटि,"MaybeStacArray (orphan) alloc %d * %lu\n", लंबाई,माप(T));
#पूर्ण_अगर
        अगर(p==nullptr) {
            वापस nullptr;
        }
        uprv_memcpy(p, ptr, (माप_प्रकार)लंबाई*माप(T));
    }
    परिणाम_Capacity=लंबाई;
    reset_To_ढेर_Array();
    वापस p;
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
ढाँचा<typename H, typename T, int32_t ढेर_Capacity>
class Maybe_ढेर_Header_And_Array {
खुला:
    // No heap allocation. Use only on the stack.
    अटल व्योम* U_EXPORT2 चालक new(माप_प्रकार) noexcept = delete;
    अटल व्योम* U_EXPORT2 चालक new[](माप_प्रकार) noexcept = delete;
    अटल व्योम* U_EXPORT2 चालक new(माप_प्रकार, व्योम*) noexcept = delete;

    /**
     * Default constructor initializes with internal H+T[stackCapacity] buffer.
     */
    Maybe_ढेर_Header_And_Array() : ptr(&ढेर_Header), capacity(ढेर_Capacity), needToRelease(false) {}
    /**
     * Destructor deletes the memory (if owned).
     */
    ~Maybe_ढेर_Header_And_Array() { releaseMemory(); }
    /**
     * Returns the array capacity (number of T items).
     * @return array capacity
     */
    int32_t getCapacity() स्थिर { वापस capacity; }
    /**
     * Access without ownership change.
     * @return the header pointer
     */
    H* getAlias() स्थिर U_LIFETIME_BOUND { वापस ptr; }
    /**
     * Returns the array start.
     * @return array start, same address as getAlias()+1
     */
    T* getArrayStart() स्थिर U_LIFETIME_BOUND { वापस reinterpret_cast<T*>(getAlias() + 1); }
    /**
     * Returns the array limit.
     * @return array limit
     */
    T* getArrayLimit() स्थिर U_LIFETIME_BOUND { वापस getArrayStart() + capacity; }
    /**
     * Access without ownership change. Same as getAlias().
     * A class instance can be used directly in expressions that take a T *.
     * @return the header pointer
     */
    चालक H*() स्थिर U_LIFETIME_BOUND { वापस ptr; }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T& चालक[](सूचक_भेद_प्रकार i) U_LIFETIME_BOUND { वापस getArrayStart()[i]; }
    /**
     * Deletes the memory block (if owned) and aliases another one, no transfer of ownership.
     * If the arguments are illegal, then the current memory is unchanged.
     * @param otherArray must not be nullptr
     * @param otherCapacity must be >0
     */
    व्योम aliasInstead(H *otherMemory, int32_t otherCapacity) {
        अगर(otherMemory!=nullptr && otherCapacity>0) {
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
    अंतरभूत H* resize(int32_t newCapacity, int32_t लंबाई = 0) U_LIFETIME_BOUND;
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
    अंतरभूत H *orphanOrClone(int32_t लंबाई, int32_t &परिणाम_Capacity);
निजी:
    H *ptr;
    int32_t capacity;
    UBool needToRelease;
    // stackHeader must precede stackArray immediately.
    H ढेर_Header;
    T ढेर_Array[ढेर_Capacity];
    व्योम releaseMemory() {
        अगर(needToRelease) {
            uprv_free(ptr);
        }
    }
    /* No comparison operators with other MaybeStackHeaderAndArray's. */
    bool चालक==(स्थिर Maybe_ढेर_Header_And_Array & /*other*/) {वापस false;}
    bool चालक!=(स्थिर Maybe_ढेर_Header_And_Array & /*other*/) {वापस true;}
    /* No ownership transfer: No copy constructor, no assignment operator. */
    Maybe_ढेर_Header_And_Array(स्थिर Maybe_ढेर_Header_And_Array & /*other*/) {}
    व्योम चालक=(स्थिर Maybe_ढेर_Header_And_Array & /*other*/) {}
};

ढाँचा <typename H, typename T, int32_t ढेर_Capacity>
अंतरभूत H* Maybe_ढेर_Header_And_Array<H, T, ढेर_Capacity>::resize(int32_t newCapacity,
                                                                int32_t लंबाई) U_LIFETIME_BOUND {
    अगर(newCapacity>=0) {
#अगर U_DEBUG && defined(UPRV_MALLOC_गिनती)
      ::ख_लिखो(::मानक_त्रुटि,"MaybeStackHeaderAndArray alloc %d + %d * %ul\n", माप(H),newCapacity,माप(T));
#पूर्ण_अगर
        H *p=(H *)uprv_malloc(माप(H)+newCapacity*माप(T));
        अगर(p!=nullptr) {
            अगर(लंबाई<0) {
                लंबाई=0;
            } अन्यथा अगर(लंबाई>0) {
                अगर(लंबाई>capacity) {
                    लंबाई=capacity;
                }
                अगर(लंबाई>newCapacity) {
                    लंबाई=newCapacity;
                }
            }
            uprv_memcpy(p, ptr, माप(H)+(माप_प्रकार)लंबाई*माप(T));
            releaseMemory();
            ptr=p;
            capacity=newCapacity;
            needToRelease=true;
        }
        वापस p;
    } अन्यथा {
        वापस nullptr;
    }
}

ढाँचा<typename H, typename T, int32_t ढेर_Capacity>
अंतरभूत H *Maybe_ढेर_Header_And_Array<H, T, ढेर_Capacity>::orphanOrClone(int32_t लंबाई,
                                                                       int32_t &परिणाम_Capacity) {
    H *p;
    अगर(needToRelease) {
        p=ptr;
    } अन्यथा {
        अगर(लंबाई<0) {
            लंबाई=0;
        } अन्यथा अगर(लंबाई>capacity) {
            लंबाई=capacity;
        }
#अगर U_DEBUG && defined(UPRV_MALLOC_गिनती)
      ::ख_लिखो(::मानक_त्रुटि,"MaybeStackHeaderAndArray (orphan) alloc %ul + %d * %lu\n", माप(H),लंबाई,माप(T));
#पूर्ण_अगर
        p=(H *)uprv_malloc(माप(H)+लंबाई*माप(T));
        अगर(p==nullptr) {
            वापस nullptr;
        }
        uprv_memcpy(p, ptr, माप(H)+(माप_प्रकार)लंबाई*माप(T));
    }
    परिणाम_Capacity=लंबाई;
    ptr=&ढेर_Header;
    capacity=ढेर_Capacity;
    needToRelease=false;
    वापस p;
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
ढाँचा<typename T, int32_t ढेर_Capacity = 8>
class MemoryPool : खुला UMemory {
खुला:
    MemoryPool() : f_गिनती(0), fPool() {}

    ~MemoryPool() {
        क्रम (int32_t i = 0; i < f_गिनती; ++i) {
            delete fPool[i];
        }
    }

    MemoryPool(स्थिर MemoryPool&) = delete;
    MemoryPool& चालक=(स्थिर MemoryPool&) = delete;

    MemoryPool(MemoryPool&& other) noexcept : f_गिनती(other.f_गिनती),
                                                fPool(std::move(other.fPool)) {
        other.f_गिनती = 0;
    }

    MemoryPool& चालक=(MemoryPool&& other) noexcept U_LIFETIME_BOUND {
        // Since `this` may contain instances that need to be deleted, we can't
        // just throw them away and replace them with `other`. The normal way of
        // dealing with this in C++ is to swap `this` and `other`, rather than
        // simply overwrite: the destruction of `other` can then take care of
        // running MemoryPool::~MemoryPool() over the still-to-be-deallocated
        // instances.
        std::swap(f_गिनती, other.f_गिनती);
        std::swap(fPool, other.fPool);
        वापस *this;
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
        अगर (f_गिनती == capacity &&
            fPool.resize(capacity == ढेर_Capacity ? 4 * capacity : 2 * capacity,
                         capacity) == nullptr) {
            वापस nullptr;
        }
        वापस fPool[f_गिनती++] = new T(std::forward<Args>(args)...);
    }

    ढाँचा <typename... Args>
    T* createAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        अगर (U_FAILURE(status)) {
            वापस nullptr;
        }
        T *pointer = this->create(args...);
        अगर (U_SUCCESS(status) && pointer == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        वापस pointer;
    }

    /**
     * @return Number of elements that have been allocated.
     */
    int32_t गिनती() स्थिर {
        वापस f_गिनती;
    }

रक्षित:
    int32_t f_गिनती;
    Maybe_ढेर_Array<T*, ढेर_Capacity> fPool;
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
ढाँचा<typename T, int32_t ढेर_Capacity = 8>
class Maybe_ढेर_Vector : रक्षित MemoryPool<T, ढेर_Capacity> {
खुला:
    ढाँचा <typename... Args>
    T* emplaceBack(Args&&... args) U_LIFETIME_BOUND {
        वापस this->create(args...);
    }

    ढाँचा <typename... Args>
    T* emplaceBackAndCheckErrorCode(UErrorCode& status, Args&&... args) U_LIFETIME_BOUND {
        वापस this->createAndCheckErrorCode(status, args...);
    }

    int32_t लंबाई() स्थिर {
        वापस this->f_गिनती;
    }

    T** getAlias() U_LIFETIME_BOUND {
        वापस this->fPool.getAlias();
    }

    स्थिर T* स्थिर* getAlias() स्थिर U_LIFETIME_BOUND {
        वापस this->fPool.getAlias();
    }

    /**
     * Array item access (read-only).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    स्थिर T* चालक[](सूचक_भेद_प्रकार i) स्थिर U_LIFETIME_BOUND {
        वापस this->fPool[i];
    }

    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     */
    T* चालक[](सूचक_भेद_प्रकार i) U_LIFETIME_BOUND {
        वापस this->fPool[i];
    }
};

namespace prv {

#अगर U_OVERRIDE_CXX_ALLOCATION

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`.
 * @internal
 */
ढाँचा <typename T, व्योम deallocate(व्योम *), typename = व्योम> class Deleter {
  खुला:
    Deleter() = शेष;
    व्योम चालक()(T *स्थिर ptr) स्थिर {
        अगर constexpr (!std::is_trivially_destructible_v<T>) {
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
ढाँचा <typename T, व्योम deallocate(व्योम *)>
class Deleter<T[], deallocate, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
  खुला:
    Deleter() = शेष;
    व्योम चालक()(T *स्थिर ptr) स्थिर { deallocate(ptr); }
};

/**
 * A deleter (as used by std::unique_ptr) that deallocates using `deallocate`: specialization for
 * arrays of nontrivially destructible objects, e.g., UnicodeString[].
 * @internal
 */
ढाँचा <typename T, व्योम deallocate(व्योम *)>
class Deleter<T[], deallocate, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
  खुला:
    Deleter(स्थिर std::माप_प्रकार आकार_2) : आकार(आकार_2) {}
    व्योम चालक()(T *स्थिर ptr) स्थिर {
        क्रम (std::सूचक_भेद_प्रकार i = static_cast<std::सूचक_भेद_प्रकार>(आकार) - 1; i >= 0; --i) {
            ptr[i]->~T();
        }
        deallocate(ptr);
    }
  निजी:
    स्थिर std::माप_प्रकार आकार;
};

/**
 * An std::unique_ptr that deallocates using uprv_free.
 * @internal
 */
ढाँचा <typename T> using unique_ptr = std::unique_ptr<T, Deleter<T, uprv_free>>;

#अन्यथा

using std::unique_ptr;

#पूर्ण_अगर

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
                               std::सूचकांक_sequence<i...> constructor_arg_indices) {
    static_assert(std::is_same_v<decltype(constructor_arg_indices),
                                 std::make_सूचकांक_sequence<माप...(Args) - 1>>);
    स्वतः &&error = std::get<माप...(Args) - 1>(args);
    static_assert(std::is_same_v<decltype(error), UErrorCode &>);
    अगर (U_FAILURE(error)) {
        वापस nullptr;
    }
#अगर U_OVERRIDE_CXX_ALLOCATION
    व्योम *स्थिर बफर = uprv_malloc(माप(T));
    अगर (बफर == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    वापस unique_ptr<T>(new (बफर) T(std::get<i>(args)...));
#अन्यथा
    वापस std::make_unique<T>(std::get<i>(args)...);
#पूर्ण_अगर
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
    वापस make_unique_impl<T>(std::forward_as_tuple(args...),
                               std::make_सूचकांक_sequence<माप...(Args) - 1>());
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
unique_ptr<T> make_unique(स्थिर std::माप_प्रकार n, UErrorCode &error) {
    अगर (U_FAILURE(error)) {
        वापस nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::माप_प्रकार max_आकार = SIZE_MAX / माप(std::remove_extent_t<T>);
    अगर (n > max_आकार) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
#अगर U_OVERRIDE_CXX_ALLOCATION
    व्योम *स्थिर बफर = uprv_malloc(माप(std::remove_extent_t<T>) * n);
    अगर (बफर == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        अगर constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            वापस nullptr;
        } अन्यथा {
            वापस unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    अगर constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        वापस unique_ptr<T>(new (बफर) std::remove_extent_t<T>[n]());
    } अन्यथा {
        वापस unique_ptr<T>(new (बफर) std::remove_extent_t<T>[n](), Deleter<T, uprv_free>(n));
    }
#अन्यथा
    वापस std::make_unique<T>(n);
#पूर्ण_अगर
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
    अगर (U_FAILURE(error)) {
        वापस nullptr;
    }
#अगर U_OVERRIDE_CXX_ALLOCATION
    व्योम *स्थिर बफर = uprv_malloc(माप(T));
    अगर (बफर == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    वापस unique_ptr<T>(new (बफर) T);
#या_अगर U_CPLUSPLUS_VERSION >= 20
    वापस std::make_unique_for_overwrite<T>();
#अन्यथा
    वापस unique_ptr<T>(new T);
#पूर्ण_अगर
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
unique_ptr<T> make_unique_for_overwrite(स्थिर std::माप_प्रकार n, UErrorCode &error) {
    अगर (U_FAILURE(error)) {
        वापस nullptr;
    }
    // In C23/C++26, we could use ckd_mul.
    constexpr std::माप_प्रकार max_आकार = SIZE_MAX / माप(std::remove_extent_t<T>);
    अगर (n > max_आकार) {
        error = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
#अगर U_OVERRIDE_CXX_ALLOCATION
    व्योम *स्थिर बफर = uprv_malloc(माप(std::remove_extent_t<T>) * n);
    अगर (बफर == nullptr) {
        error = U_MEMORY_ALLOCATION_ERROR;
        अगर constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
            वापस nullptr;
        } अन्यथा {
            वापस unique_ptr<T>(nullptr, Deleter<T, uprv_free>(0));
        }
    }
    अगर constexpr (std::is_trivially_destructible_v<std::remove_extent_t<T>>) {
        वापस unique_ptr<T>(new (बफर) std::remove_extent_t<T>[n]);
    } अन्यथा {
        वापस unique_ptr<T>(new (बफर) std::remove_extent_t<T>[n], Deleter<T, uprv_free>(n));
    }
#या_अगर U_CPLUSPLUS_VERSION >= 20
    वापस std::make_unique_for_overwrite<T>(n);
#अन्यथा
    वापस unique_ptr<T>(new std::remove_extent_t<T>[n]);
#पूर्ण_अगर
}

}  // namespace prv

U_NAMESPACE_END

#पूर्ण_अगर  /* __cplusplus */
#पूर्ण_अगर  /* CMEMORY_H */
