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

#اگر_تعریف_نشده __LOCALPOINTER_H__
#تعریف_کن __LOCALPOINTER_H__

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

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن <memory>

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
قالب<typename T>
class Local_اشاره_گر_پایه {
عام:
    // No heap allocation. Use only on the stack.
    ساكن فراغ* U_EXPORT2 عملگر new(size_t) = delete;
    ساكن فراغ* U_EXPORT2 عملگر new[](size_t) = delete;
    ساكن فراغ* U_EXPORT2 عملگر new(size_t, فراغ*) = delete;

    /**
     * Constructor takes ownership.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    explicit Local_اشاره_گر_پایه(T *p=nullptr) : ptr(p) {}
    /**
     * Destructor deletes the object it owns.
     * Subclass must override: Base class does nothing.
     * @stable ICU 4.4
     */
    ~Local_اشاره_گر_پایه() { /* delete ptr; */ }
    /**
     * nullptr check.
     * @return true if ==nullptr
     * @stable ICU 4.4
     */
    UBool isNull() ثابت { أرجع ptr==nullptr; }
    /**
     * nullptr check.
     * @return true if !=nullptr
     * @stable ICU 4.4
     */
    UBool isValid() ثابت { أرجع ptr!=nullptr; }
    /**
     * Comparison with a simple pointer, so that existing code
     * with ==nullptr need not be changed.
     * @param other simple pointer for comparison
     * @return true if this pointer value equals other
     * @stable ICU 4.4
     */
    bool عملگر==(ثابت T *other) ثابت { أرجع ptr==other; }
    /**
     * Comparison with a simple pointer, so that existing code
     * with !=nullptr need not be changed.
     * @param other simple pointer for comparison
     * @return true if this pointer value differs from other
     * @stable ICU 4.4
     */
    bool عملگر!=(ثابت T *other) ثابت { أرجع ptr!=other; }
    /**
     * Access without ownership change.
     * @return the pointer value
     * @stable ICU 4.4
     */
    T* getAlias() ثابت U_LIFETIME_BOUND { أرجع ptr; }
    /**
     * Access without ownership change.
     * @return the pointer value as a reference
     * @stable ICU 4.4
     */
    T& عملگر*() ثابت U_LIFETIME_BOUND { أرجع *ptr; }
    /**
     * Access without ownership change.
     * @return the pointer value
     * @stable ICU 4.4
     */
    T* عملگر->() ثابت U_LIFETIME_BOUND { أرجع ptr; }
    /**
     * Gives up ownership; the internal pointer becomes nullptr.
     * @return the pointer value;
     *         caller becomes responsible for deleting the object
     * @stable ICU 4.4
     */
    T *orphan() {
        T *p=ptr;
        ptr=nullptr;
        أرجع p;
    }
    /**
     * Deletes the object it owns,
     * and adopts (takes ownership of) the one passed in.
     * Subclass must override: Base class does not delete the object.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    فراغ adoptInstead(T *p) {
        // delete ptr;
        ptr=p;
    }
محمي:
    /**
     * Actual pointer.
     * @internal
     */
    T *ptr;
خاص:
    // No comparison operators with other LocalPointerBases.
    bool عملگر==(ثابت Local_اشاره_گر_پایه<T> &other) = delete;
    bool عملگر!=(ثابت Local_اشاره_گر_پایه<T> &other) = delete;
    // No ownership sharing: No copy constructor, no assignment operator.
    Local_اشاره_گر_پایه(ثابت Local_اشاره_گر_پایه<T> &other) = delete;
    فراغ عملگر=(ثابت Local_اشاره_گر_پایه<T> &other) = delete;
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
قالب<typename T>
class Local_اشاره_گر : عام Local_اشاره_گر_پایه<T> {
عام:
    using Local_اشاره_گر_پایه<T>::عملگر*;
    using Local_اشاره_گر_پایه<T>::عملگر->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    explicit Local_اشاره_گر(T *p=nullptr) : Local_اشاره_گر_پایه<T>(p) {}
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
    Local_اشاره_گر(T *p, UError_کد &error_کد) : Local_اشاره_گر_پایه<T>(p) {
        إذا(p==nullptr && U_SUCCESS(error_کد)) {
            error_کد=U_حافظه_تخصیص_ERROR;
        }
    }
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     * @stable ICU 56
     */
    Local_اشاره_گر(Local_اشاره_گر<T> &&src) noexcept : Local_اشاره_گر_پایه<T>(src.ptr) {
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
    explicit Local_اشاره_گر(std::unique_ptr<T> &&p)
        : Local_اشاره_گر_پایه<T>(p.release()) {}

    /**
     * Destructor deletes the object it owns.
     * @stable ICU 4.4
     */
    ~Local_اشاره_گر() {
        delete Local_اشاره_گر_پایه<T>::ptr;
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     * @stable ICU 56
     */
    Local_اشاره_گر<T>& عملگر=(Local_اشاره_گر<T>&& src) noexcept U_LIFETIME_BOUND {
        delete Local_اشاره_گر_پایه<T>::ptr;
        Local_اشاره_گر_پایه<T>::ptr=src.ptr;
        src.ptr=nullptr;
        أرجع *this;
    }

    /**
     * Move-assign from an std::unique_ptr to this LocalPointer.
     * Steals the pointer from the std::unique_ptr.
     *
     * @param p The std::unique_ptr from which the pointer will be stolen.
     * @return *this
     * @stable ICU 64
     */
    Local_اشاره_گر<T>& عملگر=(std::unique_ptr<T>&& p) noexcept U_LIFETIME_BOUND {
        adoptInstead(p.release());
        أرجع *this;
    }

    /**
     * Swap pointers.
     * @param other other smart pointer
     * @stable ICU 56
     */
    فراغ جابجا(Local_اشاره_گر<T> &other) noexcept {
        T *موقت=Local_اشاره_گر_پایه<T>::ptr;
        Local_اشاره_گر_پایه<T>::ptr=other.ptr;
        other.ptr=موقت;
    }
    /**
     * Non-member LocalPointer swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     * @stable ICU 56
     */
    دوست مضمن فراغ جابجا(Local_اشاره_گر<T> &p1, Local_اشاره_گر<T> &p2) noexcept {
        p1.جابجا(p2);
    }
    /**
     * Deletes the object it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    فراغ adoptInstead(T *p) {
        delete Local_اشاره_گر_پایه<T>::ptr;
        Local_اشاره_گر_پایه<T>::ptr=p;
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
    فراغ adopt_Instead_And_Check_Error_کد(T *p, UError_کد &error_کد) {
        إذا(U_SUCCESS(error_کد)) {
            delete Local_اشاره_گر_پایه<T>::ptr;
            Local_اشاره_گر_پایه<T>::ptr=p;
            إذا(p==nullptr) {
                error_کد=U_حافظه_تخصیص_ERROR;
            }
        } وإلا {
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
    عملگر std::unique_ptr<T> () && {
        أرجع std::unique_ptr<T>(Local_اشاره_گر_پایه<T>::orphan());
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
قالب<typename T>
class LocalArray : عام Local_اشاره_گر_پایه<T> {
عام:
    using Local_اشاره_گر_پایه<T>::عملگر*;
    using Local_اشاره_گر_پایه<T>::عملگر->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an array of T objects that is adopted
     * @stable ICU 4.4
     */
    explicit LocalArray(T *p=nullptr) : Local_اشاره_گر_پایه<T>(p) {}
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
    LocalArray(T *p, UError_کد &error_کد) : Local_اشاره_گر_پایه<T>(p) {
        إذا(p==nullptr && U_SUCCESS(error_کد)) {
            error_کد=U_حافظه_تخصیص_ERROR;
        }
    }
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     * @stable ICU 56
     */
    LocalArray(LocalArray<T> &&src) noexcept : Local_اشاره_گر_پایه<T>(src.ptr) {
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
        : Local_اشاره_گر_پایه<T>(p.release()) {}

    /**
     * Destructor deletes the array it owns.
     * @stable ICU 4.4
     */
    ~LocalArray() {
        delete[] Local_اشاره_گر_پایه<T>::ptr;
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     * @stable ICU 56
     */
    LocalArray<T>& عملگر=(LocalArray<T>&& src) noexcept U_LIFETIME_BOUND {
        delete[] Local_اشاره_گر_پایه<T>::ptr;
        Local_اشاره_گر_پایه<T>::ptr=src.ptr;
        src.ptr=nullptr;
        أرجع *this;
    }

    /**
     * Move-assign from an std::unique_ptr to this LocalPointer.
     * Steals the array from the std::unique_ptr.
     *
     * @param p The std::unique_ptr from which the array will be stolen.
     * @return *this
     * @stable ICU 64
     */
    LocalArray<T>& عملگر=(std::unique_ptr<T[]>&& p) noexcept U_LIFETIME_BOUND {
        adoptInstead(p.release());
        أرجع *this;
    }

    /**
     * Swap pointers.
     * @param other other smart pointer
     * @stable ICU 56
     */
    فراغ جابجا(LocalArray<T> &other) noexcept {
        T *موقت=Local_اشاره_گر_پایه<T>::ptr;
        Local_اشاره_گر_پایه<T>::ptr=other.ptr;
        other.ptr=موقت;
    }
    /**
     * Non-member LocalArray swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     * @stable ICU 56
     */
    دوست مضمن فراغ جابجا(LocalArray<T> &p1, LocalArray<T> &p2) noexcept {
        p1.جابجا(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T objects that is adopted
     * @stable ICU 4.4
     */
    فراغ adoptInstead(T *p) {
        delete[] Local_اشاره_گر_پایه<T>::ptr;
        Local_اشاره_گر_پایه<T>::ptr=p;
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
    فراغ adopt_Instead_And_Check_Error_کد(T *p, UError_کد &error_کد) {
        إذا(U_SUCCESS(error_کد)) {
            delete[] Local_اشاره_گر_پایه<T>::ptr;
            Local_اشاره_گر_پایه<T>::ptr=p;
            إذا(p==nullptr) {
                error_کد=U_حافظه_تخصیص_ERROR;
            }
        } وإلا {
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
    T& عملگر[](ptrdiff_t i) ثابت U_LIFETIME_BOUND { أرجع Local_اشاره_گر_پایه<T>::ptr[i]; }

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
    عملگر std::unique_ptr<T[]> () && {
        أرجع std::unique_ptr<T[]>(Local_اشاره_گر_پایه<T>::orphan());
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
#تعریف_کن U_DEFINE_LOCAL_OPEN_اشاره_گر(Local_اشاره_گر_Class_Name, Type, closeFunction) \
    using Local_اشاره_گر_Class_Name = داخلی_2::Local_Open_اشاره_گر<Type, closeFunction>

#اگر_تعریف_نشده U_درون_DOXYGEN
namespace داخلی_2 {
/**
 * Implementation, do not use directly: use U_DEFINE_LOCAL_OPEN_POINTER.
 *
 * @see U_DEFINE_LOCAL_OPEN_POINTER
 * @internal
 */
قالب <typename Type, خودکار closeFunction>
class Local_Open_اشاره_گر : عام Local_اشاره_گر_پایه<Type> {
    using Local_اشاره_گر_پایه<Type>::ptr;
عام:
    using Local_اشاره_گر_پایه<Type>::عملگر*;
    using Local_اشاره_گر_پایه<Type>::عملگر->;
    explicit Local_Open_اشاره_گر(Type *p=nullptr) : Local_اشاره_گر_پایه<Type>(p) {}
    Local_Open_اشاره_گر(Local_Open_اشاره_گر &&src) noexcept
            : Local_اشاره_گر_پایه<Type>(src.ptr) {
        src.ptr=nullptr;
    }
    /* TODO: Be agnostic of the deleter function signature from the user-provided std::unique_ptr? */
    explicit Local_Open_اشاره_گر(std::unique_ptr<Type, decltype(closeFunction)> &&p)
            : Local_اشاره_گر_پایه<Type>(p.release()) {}
    ~Local_Open_اشاره_گر() { إذا (ptr != nullptr) { closeFunction(ptr); } }
    Local_Open_اشاره_گر& عملگر=(Local_Open_اشاره_گر&& src) noexcept U_LIFETIME_BOUND {
        إذا (ptr != nullptr) { closeFunction(ptr); }
        Local_اشاره_گر_پایه<Type>::ptr=src.ptr;
        src.ptr=nullptr;
        أرجع *this;
    }
    /* TODO: Be agnostic of the deleter function signature from the user-provided std::unique_ptr? */
    Local_Open_اشاره_گر&
    عملگر=(std::unique_ptr<Type, decltype(closeFunction)>&& p) U_LIFETIME_BOUND {
        adoptInstead(p.release());
        أرجع *this;
    }
    فراغ جابجا(Local_Open_اشاره_گر &other) noexcept {
        Type *موقت=Local_اشاره_گر_پایه<Type>::ptr;
        Local_اشاره_گر_پایه<Type>::ptr=other.ptr;
        other.ptr=موقت;
    }
    دوست مضمن فراغ جابجا(Local_Open_اشاره_گر &p1, Local_Open_اشاره_گر &p2) noexcept {
        p1.جابجا(p2);
    }
    فراغ adoptInstead(Type *p) {
        إذا (ptr != nullptr) { closeFunction(ptr); }
        ptr=p;
    }
    عملگر std::unique_ptr<Type, decltype(closeFunction)> () && {
        أرجع std::unique_ptr<Type, decltype(closeFunction)>(Local_اشاره_گر_پایه<Type>::orphan(), closeFunction);
    }
};
}  // namespace internal
#پایان_اگر

U_NAMESPACE_انتها

#پایان_اگر  /* U_SHOW_CPLUSPLUS_API */
#پایان_اگر  /* __LOCALPOINTER_H__ */
