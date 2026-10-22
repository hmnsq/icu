// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  localpointer.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2009nov13
*   created by: Markus W. Scherer
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __LOCALPOINTER_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __LOCALPOINTER_H__

/**
 * \file
 * \brief C++ API: "Smart pointers" for use with and in ICU4C C++ code.
 *
 * These classes are inspired by
 * - std::auto_ptr
 * - boost::scoped_ptr & boost::scoped_array
 * - Taligent Safe Pointers (TOnlyPointerTo)
 *
 * but none of those provide for all of the goals for ICU smart pointers:
 * - Smart pointer owns the object and releases it when it goes out of scope.
 * - No transfer of ownership via copy/assignment to reduce misuse. Simpler & more robust.
 * - ICU-compatible: No exceptions.
 * - Need to be able to orphan/release the pointer and its ownership.
 * - Need variants for normal C++ object pointers, C++ arrays, and ICU C service objects.
 *
 * For details see https://icu.unicode.org/design/cpp/scoped_ptr
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ <memory>

U_NAMESPACE_BEGIN

/**
 * "Smart pointer" base class; do not use directly: use LocalPointer etc.
 *
 * Base class for smart pointer classes that do not throw exceptions.
 *
 * Do not use this base class directly, since it does not delete its pointer.
 * A subclass must implement methods that delete the pointer:
 * Destructor and adoptInstead().
 *
 * There is no operator T *() provided because the programmer must decide
 * whether to use getAlias() (without transfer of ownership) or orphan()
 * (with transfer of ownership and NULLing of the pointer).
 *
 * @see LocalPointer
 * @see LocalArray
 * @see U_DEFINE_LOCAL_OPEN_POINTER
 * @stable ICU 4.4
 */
ಮಾದರಿ<typename T>
class Local_ಸೂಚಕ_ತಳ {
ಸಾರ್ವಜನಿಕ:
    // No heap allocation. Use only on the stack.
    ಸ್ಥಿರ ಶೂನ್ಯ* U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new(size_t) = delete;
    ಸ್ಥಿರ ಶೂನ್ಯ* U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new[](size_t) = delete;
    ಸ್ಥಿರ ಶೂನ್ಯ* U_EXPORT2 ಕ್ರಿಯಾಕಾರಕ new(size_t, ಶೂನ್ಯ*) = delete;

    /**
     * Constructor takes ownership.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    explicit Local_ಸೂಚಕ_ತಳ(T *p=nullptr) : ptr(p) {}
    /**
     * Destructor deletes the object it owns.
     * Subclass must override: Base class does nothing.
     * @stable ICU 4.4
     */
    ~Local_ಸೂಚಕ_ತಳ() { /* delete ptr; */ }
    /**
     * nullptr check.
     * @return true if ==nullptr
     * @stable ICU 4.4
     */
    UBool isNull() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ptr==nullptr; }
    /**
     * nullptr check.
     * @return true if !=nullptr
     * @stable ICU 4.4
     */
    UBool isValid() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ptr!=nullptr; }
    /**
     * Comparison with a simple pointer, so that existing code
     * with ==nullptr need not be changed.
     * @param other simple pointer for comparison
     * @return true if this pointer value equals other
     * @stable ICU 4.4
     */
    bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ T *other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ptr==other; }
    /**
     * Comparison with a simple pointer, so that existing code
     * with !=nullptr need not be changed.
     * @param other simple pointer for comparison
     * @return true if this pointer value differs from other
     * @stable ICU 4.4
     */
    bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ T *other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ptr!=other; }
    /**
     * Access without ownership change.
     * @return the pointer value
     * @stable ICU 4.4
     */
    T* getAlias() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ptr; }
    /**
     * Access without ownership change.
     * @return the pointer value as a reference
     * @stable ICU 4.4
     */
    T& ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ *ptr; }
    /**
     * Access without ownership change.
     * @return the pointer value
     * @stable ICU 4.4
     */
    T* ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ptr; }
    /**
     * Gives up ownership; the internal pointer becomes nullptr.
     * @return the pointer value;
     *         caller becomes responsible for deleting the object
     * @stable ICU 4.4
     */
    T *orphan() {
        T *p=ptr;
        ptr=nullptr;
        ಹಿಂತಿರುಗಿ p;
    }
    /**
     * Deletes the object it owns,
     * and adopts (takes ownership of) the one passed in.
     * Subclass must override: Base class does not delete the object.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    ಶೂನ್ಯ adoptInstead(T *p) {
        // delete ptr;
        ptr=p;
    }
ಸಂರಕ್ಷಿತ:
    /**
     * Actual pointer.
     * @internal
     */
    T *ptr;
ಖಾಸಗಿ:
    // No comparison operators with other LocalPointerBases.
    bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Local_ಸೂಚಕ_ತಳ<T> &other) = delete;
    bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Local_ಸೂಚಕ_ತಳ<T> &other) = delete;
    // No ownership sharing: No copy constructor, no assignment operator.
    Local_ಸೂಚಕ_ತಳ(ಸ್ಥಿರಾಂಕ Local_ಸೂಚಕ_ತಳ<T> &other) = delete;
    ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Local_ಸೂಚಕ_ತಳ<T> &other) = delete;
};

/**
 * "Smart pointer" class, deletes objects via the standard C++ delete operator.
 * For most methods see the LocalPointerBase base class.
 *
 * Usage example:
 * \code
 * LocalPointer<UnicodeString> s(new UnicodeString((UChar32)0x50005));
 * int32_t length=s->length();  // 2
 * char16_t lead=s->charAt(0);  // 0xd900
 * if(some condition) { return; }  // no need to explicitly delete the pointer
 * s.adoptInstead(new UnicodeString((char16_t)0xfffc));
 * length=s->length();  // 1
 * // no need to explicitly delete the pointer
 * \endcode
 *
 * @see LocalPointerBase
 * @stable ICU 4.4
 */
ಮಾದರಿ<typename T>
class Local_ಸೂಚಕ : ಸಾರ್ವಜನಿಕ Local_ಸೂಚಕ_ತಳ<T> {
ಸಾರ್ವಜನಿಕ:
    using Local_ಸೂಚಕ_ತಳ<T>::ಕ್ರಿಯಾಕಾರಕ*;
    using Local_ಸೂಚಕ_ತಳ<T>::ಕ್ರಿಯಾಕಾರಕ->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    explicit Local_ಸೂಚಕ(T *p=nullptr) : Local_ಸೂಚಕ_ತಳ<T>(p) {}
    /**
     * Constructor takes ownership and reports an error if nullptr.
     *
     * This constructor is intended to be used with other-class constructors
     * that may report a failure UErrorCode,
     * so that callers need to check only for U_FAILURE(errorCode)
     * and not also separately for isNull().
     *
     * @param p simple pointer to an object that is adopted
     * @param errorCode in/out UErrorCode, set to U_MEMORY_ALLOCATION_ERROR
     *     if p==nullptr and no other failure code had been set
     * @stable ICU 55
     */
    Local_ಸೂಚಕ(T *p, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) : Local_ಸೂಚಕ_ತಳ<T>(p) {
        ಆದರೆ(p==nullptr && U_SUCCESS(error_ಸಂಕೇತ)) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
    }
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     * @stable ICU 56
     */
    Local_ಸೂಚಕ(Local_ಸೂಚಕ<T> &&src) noexcept : Local_ಸೂಚಕ_ತಳ<T>(src.ptr) {
        src.ptr=nullptr;
    }

    /**
     * Constructs a LocalPointer from a C++11 std::unique_ptr.
     * The LocalPointer steals the object owned by the std::unique_ptr.
     *
     * This constructor works via move semantics. If your std::unique_ptr is
     * in a local variable, you must use std::move.
     *
     * @param p The std::unique_ptr from which the pointer will be stolen.
     * @stable ICU 64
     */
    explicit Local_ಸೂಚಕ(std::unique_ptr<T> &&p)
        : Local_ಸೂಚಕ_ತಳ<T>(p.release()) {}

    /**
     * Destructor deletes the object it owns.
     * @stable ICU 4.4
     */
    ~Local_ಸೂಚಕ() {
        delete Local_ಸೂಚಕ_ತಳ<T>::ptr;
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     * @stable ICU 56
     */
    Local_ಸೂಚಕ<T>& ಕ್ರಿಯಾಕಾರಕ=(Local_ಸೂಚಕ<T>&& src) noexcept U_LIFETIME_BOUND {
        delete Local_ಸೂಚಕ_ತಳ<T>::ptr;
        Local_ಸೂಚಕ_ತಳ<T>::ptr=src.ptr;
        src.ptr=nullptr;
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * Move-assign from an std::unique_ptr to this LocalPointer.
     * Steals the pointer from the std::unique_ptr.
     *
     * @param p The std::unique_ptr from which the pointer will be stolen.
     * @return *this
     * @stable ICU 64
     */
    Local_ಸೂಚಕ<T>& ಕ್ರಿಯಾಕಾರಕ=(std::unique_ptr<T>&& p) noexcept U_LIFETIME_BOUND {
        adoptInstead(p.release());
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * Swap pointers.
     * @param other other smart pointer
     * @stable ICU 56
     */
    ಶೂನ್ಯ ಅದಲುಬದಲು(Local_ಸೂಚಕ<T> &other) noexcept {
        T *ತಾತ್ಕಾಲಿಕ=Local_ಸೂಚಕ_ತಳ<T>::ptr;
        Local_ಸೂಚಕ_ತಳ<T>::ptr=other.ptr;
        other.ptr=ತಾತ್ಕಾಲಿಕ;
    }
    /**
     * Non-member LocalPointer swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     * @stable ICU 56
     */
    ಮಿತ್ರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಅದಲುಬದಲು(Local_ಸೂಚಕ<T> &p1, Local_ಸೂಚಕ<T> &p2) noexcept {
        p1.ಅದಲುಬದಲು(p2);
    }
    /**
     * Deletes the object it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    ಶೂನ್ಯ adoptInstead(T *p) {
        delete Local_ಸೂಚಕ_ತಳ<T>::ptr;
        Local_ಸೂಚಕ_ತಳ<T>::ptr=p;
    }
    /**
     * Deletes the object it owns,
     * and adopts (takes ownership of) the one passed in.
     *
     * If U_FAILURE(errorCode), then the current object is retained and the new one deleted.
     *
     * If U_SUCCESS(errorCode) but the input pointer is nullptr,
     * then U_MEMORY_ALLOCATION_ERROR is set,
     * the current object is deleted, and nullptr is set.
     *
     * @param p simple pointer to an object that is adopted
     * @param errorCode in/out UErrorCode, set to U_MEMORY_ALLOCATION_ERROR
     *     if p==nullptr and no other failure code had been set
     * @stable ICU 55
     */
    ಶೂನ್ಯ adopt_Instead_And_Check_Error_ಸಂಕೇತ(T *p, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            delete Local_ಸೂಚಕ_ತಳ<T>::ptr;
            Local_ಸೂಚಕ_ತಳ<T>::ptr=p;
            ಆದರೆ(p==nullptr) {
                error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            delete p;
        }
    }

    /**
     * Conversion operator to a C++11 std::unique_ptr.
     * Disowns the object and gives it to the returned std::unique_ptr.
     *
     * This operator works via move semantics. If your LocalPointer is
     * in a local variable, you must use std::move.
     *
     * @return An std::unique_ptr owning the pointer previously owned by this
     *         icu::LocalPointer.
     * @stable ICU 64
     */
    ಕ್ರಿಯಾಕಾರಕ std::unique_ptr<T> () && {
        ಹಿಂತಿರುಗಿ std::unique_ptr<T>(Local_ಸೂಚಕ_ತಳ<T>::orphan());
    }
};

/**
 * "Smart pointer" class, deletes objects via the C++ array delete[] operator.
 * For most methods see the LocalPointerBase base class.
 * Adds operator[] for array item access.
 *
 * Usage example:
 * \code
 * LocalArray<UnicodeString> a(new UnicodeString[2]);
 * a[0].append((char16_t)0x61);
 * if(some condition) { return; }  // no need to explicitly delete the array
 * a.adoptInstead(new UnicodeString[4]);
 * a[3].append((char16_t)0x62).append((char16_t)0x63).reverse();
 * // no need to explicitly delete the array
 * \endcode
 *
 * @see LocalPointerBase
 * @stable ICU 4.4
 */
ಮಾದರಿ<typename T>
class LocalArray : ಸಾರ್ವಜನಿಕ Local_ಸೂಚಕ_ತಳ<T> {
ಸಾರ್ವಜನಿಕ:
    using Local_ಸೂಚಕ_ತಳ<T>::ಕ್ರಿಯಾಕಾರಕ*;
    using Local_ಸೂಚಕ_ತಳ<T>::ಕ್ರಿಯಾಕಾರಕ->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an array of T objects that is adopted
     * @stable ICU 4.4
     */
    explicit LocalArray(T *p=nullptr) : Local_ಸೂಚಕ_ತಳ<T>(p) {}
    /**
     * Constructor takes ownership and reports an error if nullptr.
     *
     * This constructor is intended to be used with other-class constructors
     * that may report a failure UErrorCode,
     * so that callers need to check only for U_FAILURE(errorCode)
     * and not also separately for isNull().
     *
     * @param p simple pointer to an array of T objects that is adopted
     * @param errorCode in/out UErrorCode, set to U_MEMORY_ALLOCATION_ERROR
     *     if p==nullptr and no other failure code had been set
     * @stable ICU 56
     */
    LocalArray(T *p, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) : Local_ಸೂಚಕ_ತಳ<T>(p) {
        ಆದರೆ(p==nullptr && U_SUCCESS(error_ಸಂಕೇತ)) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
    }
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     * @stable ICU 56
     */
    LocalArray(LocalArray<T> &&src) noexcept : Local_ಸೂಚಕ_ತಳ<T>(src.ptr) {
        src.ptr=nullptr;
    }

    /**
     * Constructs a LocalArray from a C++11 std::unique_ptr of an array type.
     * The LocalPointer steals the array owned by the std::unique_ptr.
     *
     * This constructor works via move semantics. If your std::unique_ptr is
     * in a local variable, you must use std::move.
     *
     * @param p The std::unique_ptr from which the array will be stolen.
     * @stable ICU 64
     */
    explicit LocalArray(std::unique_ptr<T[]> &&p)
        : Local_ಸೂಚಕ_ತಳ<T>(p.release()) {}

    /**
     * Destructor deletes the array it owns.
     * @stable ICU 4.4
     */
    ~LocalArray() {
        delete[] Local_ಸೂಚಕ_ತಳ<T>::ptr;
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     * @stable ICU 56
     */
    LocalArray<T>& ಕ್ರಿಯಾಕಾರಕ=(LocalArray<T>&& src) noexcept U_LIFETIME_BOUND {
        delete[] Local_ಸೂಚಕ_ತಳ<T>::ptr;
        Local_ಸೂಚಕ_ತಳ<T>::ptr=src.ptr;
        src.ptr=nullptr;
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * Move-assign from an std::unique_ptr to this LocalPointer.
     * Steals the array from the std::unique_ptr.
     *
     * @param p The std::unique_ptr from which the array will be stolen.
     * @return *this
     * @stable ICU 64
     */
    LocalArray<T>& ಕ್ರಿಯಾಕಾರಕ=(std::unique_ptr<T[]>&& p) noexcept U_LIFETIME_BOUND {
        adoptInstead(p.release());
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * Swap pointers.
     * @param other other smart pointer
     * @stable ICU 56
     */
    ಶೂನ್ಯ ಅದಲುಬದಲು(LocalArray<T> &other) noexcept {
        T *ತಾತ್ಕಾಲಿಕ=Local_ಸೂಚಕ_ತಳ<T>::ptr;
        Local_ಸೂಚಕ_ತಳ<T>::ptr=other.ptr;
        other.ptr=ತಾತ್ಕಾಲಿಕ;
    }
    /**
     * Non-member LocalArray swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     * @stable ICU 56
     */
    ಮಿತ್ರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಅದಲುಬದಲು(LocalArray<T> &p1, LocalArray<T> &p2) noexcept {
        p1.ಅದಲುಬದಲು(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T objects that is adopted
     * @stable ICU 4.4
     */
    ಶೂನ್ಯ adoptInstead(T *p) {
        delete[] Local_ಸೂಚಕ_ತಳ<T>::ptr;
        Local_ಸೂಚಕ_ತಳ<T>::ptr=p;
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     *
     * If U_FAILURE(errorCode), then the current array is retained and the new one deleted.
     *
     * If U_SUCCESS(errorCode) but the input pointer is nullptr,
     * then U_MEMORY_ALLOCATION_ERROR is set,
     * the current array is deleted, and nullptr is set.
     *
     * @param p simple pointer to an array of T objects that is adopted
     * @param errorCode in/out UErrorCode, set to U_MEMORY_ALLOCATION_ERROR
     *     if p==nullptr and no other failure code had been set
     * @stable ICU 56
     */
    ಶೂನ್ಯ adopt_Instead_And_Check_Error_ಸಂಕೇತ(T *p, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            delete[] Local_ಸೂಚಕ_ತಳ<T>::ptr;
            Local_ಸೂಚಕ_ತಳ<T>::ptr=p;
            ಆದರೆ(p==nullptr) {
                error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            delete[] p;
        }
    }
    /**
     * Array item access (writable).
     * No index bounds check.
     * @param i array index
     * @return reference to the array item
     * @stable ICU 4.4
     */
    T& ಕ್ರಿಯಾಕಾರಕ[](ptrdiff_t i) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ Local_ಸೂಚಕ_ತಳ<T>::ptr[i]; }

    /**
     * Conversion operator to a C++11 std::unique_ptr.
     * Disowns the object and gives it to the returned std::unique_ptr.
     *
     * This operator works via move semantics. If your LocalPointer is
     * in a local variable, you must use std::move.
     *
     * @return An std::unique_ptr owning the pointer previously owned by this
     *         icu::LocalPointer.
     * @stable ICU 64
     */
    ಕ್ರಿಯಾಕಾರಕ std::unique_ptr<T[]> () && {
        ಹಿಂತಿರುಗಿ std::unique_ptr<T[]>(Local_ಸೂಚಕ_ತಳ<T>::orphan());
    }
};

/**
 * \def U_DEFINE_LOCAL_OPEN_POINTER
 * "Smart pointer" definition macro, deletes objects via the closeFunction.
 * Defines a subclass of LocalPointerBase which works just
 * like LocalPointer<Type> except that this subclass will use the closeFunction
 * rather than the C++ delete operator.
 *
 * Usage example:
 * \code
 * LocalUCaseMapPointer csm(ucasemap_open(localeID, options, &errorCode));
 * utf8OutLength=ucasemap_utf8ToLower(csm.getAlias(),
 *     utf8Out, (int32_t)sizeof(utf8Out),
 *     utf8In, utf8InLength, &errorCode);
 * if(U_FAILURE(errorCode)) { return; }  // no need to explicitly delete the UCaseMap
 * \endcode
 *
 * @see LocalPointerBase
 * @see LocalPointer
 * @stable ICU 4.4
 */
#ವ್ಯಾಖ್ಯಾನಿಸು U_DEFINE_LOCAL_OPEN_ಸೂಚಕ(Local_ಸೂಚಕ_Class_Name, Type, closeFunction) \
    using Local_ಸೂಚಕ_Class_Name = ಆಂತರಿಕ_2::Local_Open_ಸೂಚಕ<Type, closeFunction>

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
namespace ಆಂತರಿಕ_2 {
/**
 * Implementation, do not use directly: use U_DEFINE_LOCAL_OPEN_POINTER.
 *
 * @see U_DEFINE_LOCAL_OPEN_POINTER
 * @internal
 */
ಮಾದರಿ <typename Type, ಸ್ವಯಂಚಾಲಿತ closeFunction>
class Local_Open_ಸೂಚಕ : ಸಾರ್ವಜನಿಕ Local_ಸೂಚಕ_ತಳ<Type> {
    using Local_ಸೂಚಕ_ತಳ<Type>::ptr;
ಸಾರ್ವಜನಿಕ:
    using Local_ಸೂಚಕ_ತಳ<Type>::ಕ್ರಿಯಾಕಾರಕ*;
    using Local_ಸೂಚಕ_ತಳ<Type>::ಕ್ರಿಯಾಕಾರಕ->;
    explicit Local_Open_ಸೂಚಕ(Type *p=nullptr) : Local_ಸೂಚಕ_ತಳ<Type>(p) {}
    Local_Open_ಸೂಚಕ(Local_Open_ಸೂಚಕ &&src) noexcept
            : Local_ಸೂಚಕ_ತಳ<Type>(src.ptr) {
        src.ptr=nullptr;
    }
    /* TODO: Be agnostic of the deleter function signature from the user-provided std::unique_ptr? */
    explicit Local_Open_ಸೂಚಕ(std::unique_ptr<Type, decltype(closeFunction)> &&p)
            : Local_ಸೂಚಕ_ತಳ<Type>(p.release()) {}
    ~Local_Open_ಸೂಚಕ() { ಆದರೆ (ptr != nullptr) { closeFunction(ptr); } }
    Local_Open_ಸೂಚಕ& ಕ್ರಿಯಾಕಾರಕ=(Local_Open_ಸೂಚಕ&& src) noexcept U_LIFETIME_BOUND {
        ಆದರೆ (ptr != nullptr) { closeFunction(ptr); }
        Local_ಸೂಚಕ_ತಳ<Type>::ptr=src.ptr;
        src.ptr=nullptr;
        ಹಿಂತಿರುಗಿ *this;
    }
    /* TODO: Be agnostic of the deleter function signature from the user-provided std::unique_ptr? */
    Local_Open_ಸೂಚಕ&
    ಕ್ರಿಯಾಕಾರಕ=(std::unique_ptr<Type, decltype(closeFunction)>&& p) U_LIFETIME_BOUND {
        adoptInstead(p.release());
        ಹಿಂತಿರುಗಿ *this;
    }
    ಶೂನ್ಯ ಅದಲುಬದಲು(Local_Open_ಸೂಚಕ &other) noexcept {
        Type *ತಾತ್ಕಾಲಿಕ=Local_ಸೂಚಕ_ತಳ<Type>::ptr;
        Local_ಸೂಚಕ_ತಳ<Type>::ptr=other.ptr;
        other.ptr=ತಾತ್ಕಾಲಿಕ;
    }
    ಮಿತ್ರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಅದಲುಬದಲು(Local_Open_ಸೂಚಕ &p1, Local_Open_ಸೂಚಕ &p2) noexcept {
        p1.ಅದಲುಬದಲು(p2);
    }
    ಶೂನ್ಯ adoptInstead(Type *p) {
        ಆದರೆ (ptr != nullptr) { closeFunction(ptr); }
        ptr=p;
    }
    ಕ್ರಿಯಾಕಾರಕ std::unique_ptr<Type, decltype(closeFunction)> () && {
        ಹಿಂತಿರುಗಿ std::unique_ptr<Type, decltype(closeFunction)>(Local_ಸೂಚಕ_ತಳ<Type>::orphan(), closeFunction);
    }
};
}  // namespace internal
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  /* U_SHOW_CPLUSPLUS_API */
#ಆದರೆ_ಅಂತ್ಯ  /* __LOCALPOINTER_H__ */
