// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#ifndef __CHAR16PTR_H__
#define __CHAR16PTR_H__

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

#অন্তর্ভাব <cstddef>
#অন্তর্ভাব <string_view>
#অন্তর্ভাব <type_traits>

#endif

/**
 * \file
 * \brief C++ API: char16_t pointer wrappers with
 *        implicit conversion from bit-compatible raw pointer types.
 *        Also conversion functions from char16_t * to UChar * and OldUChar *.
 */

/**
 * \def U_ALIASING_BARRIER
 * Barrier for pointer anti-aliasing optimizations even across function boundaries.
 * @internal
 */
#ifdef U_ALIASING_BARRIER
    // Use the predefined value.
#নতুবা_যদি (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) asm অস্থির("" : : "rm"(ptr) : "memory")
#নতুবা_যদি defined(U_IN_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#endif

// ICU DLL-exported
#যদি U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API Char16Ptr final {
সর্বজনীন:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    অন্তঃরেখ Char16Ptr(char16_t *p);
#যদি !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    অন্তঃরেখ Char16Ptr(uint16_t *p);
#endif
#যদি U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    অন্তঃরেখ Char16Ptr(wchar_t *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    অন্তঃরেখ Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    অন্তঃরেখ ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    অন্তঃরেখ char16_t *get() ধ্রুবক;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    অন্তঃরেখ operator char16_t *() ধ্রুবক { ফেরত get(); }

ব্যক্তিগত:
    Char16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    template<typename T> স্থির char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        ফেরত reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#অন্যথা
    সংঘ {
        char16_t *cp;
        uint16_t *up;
        wchar_t *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#যদি !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#endif
#যদি U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) : p_(cast(p)) {}
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() ধ্রুবক { ফেরত p_; }

#অন্যথা

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#যদি !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.up = p; }
#endif
#যদি U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) { u_.wp = p; }
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() ধ্রুবক { ফেরত u_.cp; }

#endif
/// \endcond

/**
 * const char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API ConstChar16Ptr final {
সর্বজনীন:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    অন্তঃরেখ ConstChar16Ptr(ধ্রুবক char16_t *p);
#যদি !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    অন্তঃরেখ ConstChar16Ptr(ধ্রুবক uint16_t *p);
#endif
#যদি U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    অন্তঃরেখ ConstChar16Ptr(ধ্রুবক wchar_t *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    অন্তঃরেখ ConstChar16Ptr(ধ্রুবক std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    অন্তঃরেখ ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    অন্তঃরেখ ধ্রুবক char16_t *get() ধ্রুবক;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    অন্তঃরেখ operator ধ্রুবক char16_t *() ধ্রুবক { ফেরত get(); }

ব্যক্তিগত:
    ConstChar16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    template<typename T> স্থির ধ্রুবক char16_t *cast(ধ্রুবক T *t) {
        U_ALIASING_BARRIER(t);
        ফেরত reinterpret_cast<ধ্রুবক char16_t *>(t);
    }

    ধ্রুবক char16_t *p_;
#অন্যথা
    সংঘ {
        ধ্রুবক char16_t *cp;
        ধ্রুবক uint16_t *up;
        ধ্রুবক wchar_t *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(ধ্রুবক char16_t *p) : p_(p) {}
#যদি !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ধ্রুবক uint16_t *p) : p_(cast(p)) {}
#endif
#যদি U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ধ্রুবক wchar_t *p) : p_(cast(p)) {}
#endif
ConstChar16Ptr::ConstChar16Ptr(ধ্রুবক std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

ধ্রুবক char16_t *ConstChar16Ptr::get() ধ্রুবক { ফেরত p_; }

#অন্যথা

ConstChar16Ptr::ConstChar16Ptr(ধ্রুবক char16_t *p) { u_.cp = p; }
#যদি !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ধ্রুবক uint16_t *p) { u_.up = p; }
#endif
#যদি U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ধ্রুবক wchar_t *p) { u_.wp = p; }
#endif
ConstChar16Ptr::ConstChar16Ptr(ধ্রুবক std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

ধ্রুবক char16_t *ConstChar16Ptr::get() ধ্রুবক { ফেরত u_.cp; }

#endif
/// \endcond

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#যদি U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

namespace U_ICU_NAMESPACE_OR_INTERNAL {

#ifndef U_FORCE_HIDE_INTERNAL_API
/** @internal */
template <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
অন্তঃরেখ ধ্রুবক char16_t* uprv_char16PtrFromUChar(ধ্রুবক T* p U_LIFETIME_BOUND) {
    যদি constexpr (std::is_same_v<UChar, char16_t>) {
        ফেরত p;
    } অন্যথা {
#যদি U_SHOW_CPLUSPLUS_API
        ফেরত ConstChar16Ptr(p).get();
#অন্যথা
#ifdef U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#endif
        ফেরত reinterpret_cast<ধ্রুবক char16_t *>(p);
#endif
    }
}
#যদি !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/** @internal */
অন্তঃরেখ ধ্রুবক char16_t* uprv_char16PtrFromUint16(ধ্রুবক uint16_t* p U_LIFETIME_BOUND) {
#যদি U_SHOW_CPLUSPLUS_API
    ফেরত ConstChar16Ptr(p).get();
#অন্যথা
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ফেরত reinterpret_cast<ধ্রুবক char16_t *>(p);
#endif
}
#endif
#যদি U_SIZEOF_WCHAR_T==2
/** @internal */
অন্তঃরেখ ধ্রুবক char16_t* uprv_char16PtrFromWchar(ধ্রুবক wchar_t* p U_LIFETIME_BOUND) {
#যদি U_SHOW_CPLUSPLUS_API
    ফেরত ConstChar16Ptr(p).get();
#অন্যথা
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ফেরত reinterpret_cast<ধ্রুবক char16_t *>(p);
#endif
}
#endif
#endif

/**
 * Converts from const char16_t * to const UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const UChar *
 * @stable ICU 59
 */
অন্তঃরেখ ধ্রুবক UChar* toUCharPtr(ধ্রুবক char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ফেরত reinterpret_cast<ধ্রুবক UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
অন্তঃরেখ UChar* toUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ফেরত reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
অন্তঃরেখ ধ্রুবক OldUChar* toOldUCharPtr(ধ্রুবক char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ফেরত reinterpret_cast<ধ্রুবক OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
অন্তঃরেখ OldUChar* toOldUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ফেরত reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#endif  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#যদি U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#ifndef U_FORCE_HIDE_INTERNAL_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
template<typename T>
constexpr bool ConvertibleToU16StringView =
    std::is_convertible_v<T, std::u16string_view>
#যদি !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#endif
#যদি U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#endif
    ;

namespace internal {
/**
 * Pass-through overload.
 * @internal
 */
অন্তঃরেখ std::u16string_view toU16StringView(std::u16string_view sv U_LIFETIME_BOUND) { ফেরত sv; }

#যদি !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
অন্তঃরেখ std::u16string_view toU16StringView(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    ফেরত { ConstChar16Ptr(sv.তথ্য()), sv.দৈর্ঘ্য() };
}
#endif

#যদি U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
অন্তঃরেখ std::u16string_view toU16StringView(std::wstring_view sv U_LIFETIME_BOUND) {
    ফেরত { ConstChar16Ptr(sv.তথ্য()), sv.দৈর্ঘ্য() };
}
#endif

/**
 * Pass-through overload.
 * @internal
 */
template <typename T,
          typename = typename std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>>>
অন্তঃরেখ std::u16string_view toU16StringViewNullable(ধ্রুবক T& text U_LIFETIME_BOUND) {
    ফেরত toU16StringView(text);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
template <typename T,
          typename = typename std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>>,
          typename = শূন্য>
অন্তঃরেখ std::u16string_view toU16StringViewNullable(ধ্রুবক T& text U_LIFETIME_BOUND) {
    যদি (text == nullptr) ফেরত {};  // For backward compatibility.
    ফেরত toU16StringView(text);
}

}  // internal
#endif  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

#endif  // __CHAR16PTR_H__
