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

#నిర్వచితం_కాకపోతే __LOCALPOINTER_H__
#నిర్వచించు __LOCALPOINTER_H__

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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ <memory>

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
మూస<typename T>
class Local_సూచిక_పునాది {
బహిరంగ:
    // No heap allocation. Use only on the stack.
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new(size_t) = delete;
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new[](size_t) = delete;
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new(size_t, శూన్య*) = delete;

    /**
     * Constructor takes ownership.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    explicit Local_సూచిక_పునాది(T *p=nullptr) : ptr(p) {}
    /**
     * Destructor deletes the object it owns.
     * Subclass must override: Base class does nothing.
     * @stable ICU 4.4
     */
    ~Local_సూచిక_పునాది() { /* delete ptr; */ }
    /**
     * nullptr check.
     * @return true if ==nullptr
     * @stable ICU 4.4
     */
    UBool isNull() స్థిరరాశి { తిరిగి ptr==nullptr; }
    /**
     * nullptr check.
     * @return true if !=nullptr
     * @stable ICU 4.4
     */
    UBool isValid() స్థిరరాశి { తిరిగి ptr!=nullptr; }
    /**
     * Comparison with a simple pointer, so that existing code
     * with ==nullptr need not be changed.
     * @param other simple pointer for comparison
     * @return true if this pointer value equals other
     * @stable ICU 4.4
     */
    bool క్రియాకారకం==(స్థిరరాశి T *other) స్థిరరాశి { తిరిగి ptr==other; }
    /**
     * Comparison with a simple pointer, so that existing code
     * with !=nullptr need not be changed.
     * @param other simple pointer for comparison
     * @return true if this pointer value differs from other
     * @stable ICU 4.4
     */
    bool క్రియాకారకం!=(స్థిరరాశి T *other) స్థిరరాశి { తిరిగి ptr!=other; }
    /**
     * Access without ownership change.
     * @return the pointer value
     * @stable ICU 4.4
     */
    T* getAlias() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి ptr; }
    /**
     * Access without ownership change.
     * @return the pointer value as a reference
     * @stable ICU 4.4
     */
    T& క్రియాకారకం*() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి *ptr; }
    /**
     * Access without ownership change.
     * @return the pointer value
     * @stable ICU 4.4
     */
    T* క్రియాకారకం->() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి ptr; }
    /**
     * Gives up ownership; the internal pointer becomes nullptr.
     * @return the pointer value;
     *         caller becomes responsible for deleting the object
     * @stable ICU 4.4
     */
    T *orphan() {
        T *p=ptr;
        ptr=nullptr;
        తిరిగి p;
    }
    /**
     * Deletes the object it owns,
     * and adopts (takes ownership of) the one passed in.
     * Subclass must override: Base class does not delete the object.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    శూన్య adoptInstead(T *p) {
        // delete ptr;
        ptr=p;
    }
రక్షిత:
    /**
     * Actual pointer.
     * @internal
     */
    T *ptr;
అంతరంగ:
    // No comparison operators with other LocalPointerBases.
    bool క్రియాకారకం==(స్థిరరాశి Local_సూచిక_పునాది<T> &other) = delete;
    bool క్రియాకారకం!=(స్థిరరాశి Local_సూచిక_పునాది<T> &other) = delete;
    // No ownership sharing: No copy constructor, no assignment operator.
    Local_సూచిక_పునాది(స్థిరరాశి Local_సూచిక_పునాది<T> &other) = delete;
    శూన్య క్రియాకారకం=(స్థిరరాశి Local_సూచిక_పునాది<T> &other) = delete;
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
మూస<typename T>
class Local_సూచిక : బహిరంగ Local_సూచిక_పునాది<T> {
బహిరంగ:
    using Local_సూచిక_పునాది<T>::క్రియాకారకం*;
    using Local_సూచిక_పునాది<T>::క్రియాకారకం->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    explicit Local_సూచిక(T *p=nullptr) : Local_సూచిక_పునాది<T>(p) {}
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
    Local_సూచిక(T *p, UError_సంకేతం &error_సంకేతం) : Local_సూచిక_పునాది<T>(p) {
        ఒకవేళ(p==nullptr && U_SUCCESS(error_సంకేతం)) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
    }
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     * @stable ICU 56
     */
    Local_సూచిక(Local_సూచిక<T> &&src) noexcept : Local_సూచిక_పునాది<T>(src.ptr) {
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
    explicit Local_సూచిక(std::unique_ptr<T> &&p)
        : Local_సూచిక_పునాది<T>(p.release()) {}

    /**
     * Destructor deletes the object it owns.
     * @stable ICU 4.4
     */
    ~Local_సూచిక() {
        delete Local_సూచిక_పునాది<T>::ptr;
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     * @stable ICU 56
     */
    Local_సూచిక<T>& క్రియాకారకం=(Local_సూచిక<T>&& src) noexcept U_LIFETIME_BOUND {
        delete Local_సూచిక_పునాది<T>::ptr;
        Local_సూచిక_పునాది<T>::ptr=src.ptr;
        src.ptr=nullptr;
        తిరిగి *this;
    }

    /**
     * Move-assign from an std::unique_ptr to this LocalPointer.
     * Steals the pointer from the std::unique_ptr.
     *
     * @param p The std::unique_ptr from which the pointer will be stolen.
     * @return *this
     * @stable ICU 64
     */
    Local_సూచిక<T>& క్రియాకారకం=(std::unique_ptr<T>&& p) noexcept U_LIFETIME_BOUND {
        adoptInstead(p.release());
        తిరిగి *this;
    }

    /**
     * Swap pointers.
     * @param other other smart pointer
     * @stable ICU 56
     */
    శూన్య మార్పిడి(Local_సూచిక<T> &other) noexcept {
        T *తాత్కాలిక=Local_సూచిక_పునాది<T>::ptr;
        Local_సూచిక_పునాది<T>::ptr=other.ptr;
        other.ptr=తాత్కాలిక;
    }
    /**
     * Non-member LocalPointer swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     * @stable ICU 56
     */
    మిత్ర అంతఃవరుస శూన్య మార్పిడి(Local_సూచిక<T> &p1, Local_సూచిక<T> &p2) noexcept {
        p1.మార్పిడి(p2);
    }
    /**
     * Deletes the object it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an object that is adopted
     * @stable ICU 4.4
     */
    శూన్య adoptInstead(T *p) {
        delete Local_సూచిక_పునాది<T>::ptr;
        Local_సూచిక_పునాది<T>::ptr=p;
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
    శూన్య adopt_Instead_And_Check_Error_సంకేతం(T *p, UError_సంకేతం &error_సంకేతం) {
        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            delete Local_సూచిక_పునాది<T>::ptr;
            Local_సూచిక_పునాది<T>::ptr=p;
            ఒకవేళ(p==nullptr) {
                error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            }
        } లేకపోతే {
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
    క్రియాకారకం std::unique_ptr<T> () && {
        తిరిగి std::unique_ptr<T>(Local_సూచిక_పునాది<T>::orphan());
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
మూస<typename T>
class LocalArray : బహిరంగ Local_సూచిక_పునాది<T> {
బహిరంగ:
    using Local_సూచిక_పునాది<T>::క్రియాకారకం*;
    using Local_సూచిక_పునాది<T>::క్రియాకారకం->;
    /**
     * Constructor takes ownership.
     * @param p simple pointer to an array of T objects that is adopted
     * @stable ICU 4.4
     */
    explicit LocalArray(T *p=nullptr) : Local_సూచిక_పునాది<T>(p) {}
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
    LocalArray(T *p, UError_సంకేతం &error_సంకేతం) : Local_సూచిక_పునాది<T>(p) {
        ఒకవేళ(p==nullptr && U_SUCCESS(error_సంకేతం)) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
    }
    /**
     * Move constructor, leaves src with isNull().
     * @param src source smart pointer
     * @stable ICU 56
     */
    LocalArray(LocalArray<T> &&src) noexcept : Local_సూచిక_పునాది<T>(src.ptr) {
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
        : Local_సూచిక_పునాది<T>(p.release()) {}

    /**
     * Destructor deletes the array it owns.
     * @stable ICU 4.4
     */
    ~LocalArray() {
        delete[] Local_సూచిక_పునాది<T>::ptr;
    }
    /**
     * Move assignment operator, leaves src with isNull().
     * The behavior is undefined if *this and src are the same object.
     * @param src source smart pointer
     * @return *this
     * @stable ICU 56
     */
    LocalArray<T>& క్రియాకారకం=(LocalArray<T>&& src) noexcept U_LIFETIME_BOUND {
        delete[] Local_సూచిక_పునాది<T>::ptr;
        Local_సూచిక_పునాది<T>::ptr=src.ptr;
        src.ptr=nullptr;
        తిరిగి *this;
    }

    /**
     * Move-assign from an std::unique_ptr to this LocalPointer.
     * Steals the array from the std::unique_ptr.
     *
     * @param p The std::unique_ptr from which the array will be stolen.
     * @return *this
     * @stable ICU 64
     */
    LocalArray<T>& క్రియాకారకం=(std::unique_ptr<T[]>&& p) noexcept U_LIFETIME_BOUND {
        adoptInstead(p.release());
        తిరిగి *this;
    }

    /**
     * Swap pointers.
     * @param other other smart pointer
     * @stable ICU 56
     */
    శూన్య మార్పిడి(LocalArray<T> &other) noexcept {
        T *తాత్కాలిక=Local_సూచిక_పునాది<T>::ptr;
        Local_సూచిక_పునాది<T>::ptr=other.ptr;
        other.ptr=తాత్కాలిక;
    }
    /**
     * Non-member LocalArray swap function.
     * @param p1 will get p2's pointer
     * @param p2 will get p1's pointer
     * @stable ICU 56
     */
    మిత్ర అంతఃవరుస శూన్య మార్పిడి(LocalArray<T> &p1, LocalArray<T> &p2) noexcept {
        p1.మార్పిడి(p2);
    }
    /**
     * Deletes the array it owns,
     * and adopts (takes ownership of) the one passed in.
     * @param p simple pointer to an array of T objects that is adopted
     * @stable ICU 4.4
     */
    శూన్య adoptInstead(T *p) {
        delete[] Local_సూచిక_పునాది<T>::ptr;
        Local_సూచిక_పునాది<T>::ptr=p;
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
    శూన్య adopt_Instead_And_Check_Error_సంకేతం(T *p, UError_సంకేతం &error_సంకేతం) {
        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            delete[] Local_సూచిక_పునాది<T>::ptr;
            Local_సూచిక_పునాది<T>::ptr=p;
            ఒకవేళ(p==nullptr) {
                error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            }
        } లేకపోతే {
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
    T& క్రియాకారకం[](ptrdiff_t i) స్థిరరాశి U_LIFETIME_BOUND { తిరిగి Local_సూచిక_పునాది<T>::ptr[i]; }

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
    క్రియాకారకం std::unique_ptr<T[]> () && {
        తిరిగి std::unique_ptr<T[]>(Local_సూచిక_పునాది<T>::orphan());
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
#నిర్వచించు U_DEFINE_LOCAL_OPEN_సూచిక(Local_సూచిక_Class_Name, Type, closeFunction) \
    using Local_సూచిక_Class_Name = అంతర్గత_2::Local_Open_సూచిక<Type, closeFunction>

#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
namespace అంతర్గత_2 {
/**
 * Implementation, do not use directly: use U_DEFINE_LOCAL_OPEN_POINTER.
 *
 * @see U_DEFINE_LOCAL_OPEN_POINTER
 * @internal
 */
మూస <typename Type, స్వయంచాలక closeFunction>
class Local_Open_సూచిక : బహిరంగ Local_సూచిక_పునాది<Type> {
    using Local_సూచిక_పునాది<Type>::ptr;
బహిరంగ:
    using Local_సూచిక_పునాది<Type>::క్రియాకారకం*;
    using Local_సూచిక_పునాది<Type>::క్రియాకారకం->;
    explicit Local_Open_సూచిక(Type *p=nullptr) : Local_సూచిక_పునాది<Type>(p) {}
    Local_Open_సూచిక(Local_Open_సూచిక &&src) noexcept
            : Local_సూచిక_పునాది<Type>(src.ptr) {
        src.ptr=nullptr;
    }
    /* TODO: Be agnostic of the deleter function signature from the user-provided std::unique_ptr? */
    explicit Local_Open_సూచిక(std::unique_ptr<Type, decltype(closeFunction)> &&p)
            : Local_సూచిక_పునాది<Type>(p.release()) {}
    ~Local_Open_సూచిక() { ఒకవేళ (ptr != nullptr) { closeFunction(ptr); } }
    Local_Open_సూచిక& క్రియాకారకం=(Local_Open_సూచిక&& src) noexcept U_LIFETIME_BOUND {
        ఒకవేళ (ptr != nullptr) { closeFunction(ptr); }
        Local_సూచిక_పునాది<Type>::ptr=src.ptr;
        src.ptr=nullptr;
        తిరిగి *this;
    }
    /* TODO: Be agnostic of the deleter function signature from the user-provided std::unique_ptr? */
    Local_Open_సూచిక&
    క్రియాకారకం=(std::unique_ptr<Type, decltype(closeFunction)>&& p) U_LIFETIME_BOUND {
        adoptInstead(p.release());
        తిరిగి *this;
    }
    శూన్య మార్పిడి(Local_Open_సూచిక &other) noexcept {
        Type *తాత్కాలిక=Local_సూచిక_పునాది<Type>::ptr;
        Local_సూచిక_పునాది<Type>::ptr=other.ptr;
        other.ptr=తాత్కాలిక;
    }
    మిత్ర అంతఃవరుస శూన్య మార్పిడి(Local_Open_సూచిక &p1, Local_Open_సూచిక &p2) noexcept {
        p1.మార్పిడి(p2);
    }
    శూన్య adoptInstead(Type *p) {
        ఒకవేళ (ptr != nullptr) { closeFunction(ptr); }
        ptr=p;
    }
    క్రియాకారకం std::unique_ptr<Type, decltype(closeFunction)> () && {
        తిరిగి std::unique_ptr<Type, decltype(closeFunction)>(Local_సూచిక_పునాది<Type>::orphan(), closeFunction);
    }
};
}  // namespace internal
#అయితే_ముగింపు

U_NAMESPACE_చివర

#అయితే_ముగింపు  /* U_SHOW_CPLUSPLUS_API */
#అయితే_ముగింపు  /* __LOCALPOINTER_H__ */
