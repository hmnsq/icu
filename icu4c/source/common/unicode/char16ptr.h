// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#ifndef __CHAR16PTR_H__
#define __CHAR16PTR_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

#ಅನ್ತರ್ಭಾವ <cstddef>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <type_traits>

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
#ನೋಚೇದ್ಯದಿ (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) asm ಚಞ್ಚಲ("" : : "rm"(ptr) : "memory")
#ನೋಚೇದ್ಯದಿ defined(U_IN_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#endif

// ICU DLL-exported
#ಯದಿ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API Char16Ptr final {
ಸಾರ್ವಜನಿಕ:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ Char16Ptr(char16_t *p);
#ಯದಿ !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ Char16Ptr(uint16_t *p);
#endif
#ಯದಿ U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ Char16Ptr(wchar_t *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ char16_t *get() ಧ್ರುವ;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ operator char16_t *() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ get(); }

ಗುಪ್ತ:
    Char16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    template<typename T> ಸ್ಥಿರ char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        ಪ್ರತ್ಯಾಗಮ reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#ಅನ್ಯಥಾ
    ಸಙ್ಘ {
        char16_t *cp;
        uint16_t *up;
        wchar_t *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#ಯದಿ !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#endif
#ಯದಿ U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) : p_(cast(p)) {}
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ p_; }

#ಅನ್ಯಥಾ

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#ಯದಿ !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.up = p; }
#endif
#ಯದಿ U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) { u_.wp = p; }
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ u_.cp; }

#endif
/// \endcond

/**
 * const char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API ConstChar16Ptr final {
ಸಾರ್ವಜನಿಕ:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ ConstChar16Ptr(ಧ್ರುವ char16_t *p);
#ಯದಿ !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ ConstChar16Ptr(ಧ್ರುವ uint16_t *p);
#endif
#ಯದಿ U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ ConstChar16Ptr(ಧ್ರುವ wchar_t *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ ConstChar16Ptr(ಧ್ರುವ std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ ಧ್ರುವ char16_t *get() ಧ್ರುವ;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ಅನ್ತಃಸ್ಥ operator ಧ್ರುವ char16_t *() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ get(); }

ಗುಪ್ತ:
    ConstChar16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    template<typename T> ಸ್ಥಿರ ಧ್ರುವ char16_t *cast(ಧ್ರುವ T *t) {
        U_ALIASING_BARRIER(t);
        ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ char16_t *>(t);
    }

    ಧ್ರುವ char16_t *p_;
#ಅನ್ಯಥಾ
    ಸಙ್ಘ {
        ಧ್ರುವ char16_t *cp;
        ಧ್ರುವ uint16_t *up;
        ಧ್ರುವ wchar_t *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(ಧ್ರುವ char16_t *p) : p_(p) {}
#ಯದಿ !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ಧ್ರುವ uint16_t *p) : p_(cast(p)) {}
#endif
#ಯದಿ U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ಧ್ರುವ wchar_t *p) : p_(cast(p)) {}
#endif
ConstChar16Ptr::ConstChar16Ptr(ಧ್ರುವ std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

ಧ್ರುವ char16_t *ConstChar16Ptr::get() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ p_; }

#ಅನ್ಯಥಾ

ConstChar16Ptr::ConstChar16Ptr(ಧ್ರುವ char16_t *p) { u_.cp = p; }
#ಯದಿ !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ಧ್ರುವ uint16_t *p) { u_.up = p; }
#endif
#ಯದಿ U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ಧ್ರುವ wchar_t *p) { u_.wp = p; }
#endif
ConstChar16Ptr::ConstChar16Ptr(ಧ್ರುವ std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

ಧ್ರುವ char16_t *ConstChar16Ptr::get() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ u_.cp; }

#endif
/// \endcond

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#ಯದಿ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

namespace U_ICU_NAMESPACE_OR_INTERNAL {

#ifndef U_FORCE_HIDE_INTERNAL_API
/** @internal */
template <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
ಅನ್ತಃಸ್ಥ ಧ್ರುವ char16_t* uprv_char16PtrFromUChar(ಧ್ರುವ T* p U_LIFETIME_BOUND) {
    ಯದಿ constexpr (std::is_same_v<UChar, char16_t>) {
        ಪ್ರತ್ಯಾಗಮ p;
    } ಅನ್ಯಥಾ {
#ಯದಿ U_SHOW_CPLUSPLUS_API
        ಪ್ರತ್ಯಾಗಮ ConstChar16Ptr(p).get();
#ಅನ್ಯಥಾ
#ifdef U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#endif
        ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ char16_t *>(p);
#endif
    }
}
#ಯದಿ !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/** @internal */
ಅನ್ತಃಸ್ಥ ಧ್ರುವ char16_t* uprv_char16PtrFromUint16(ಧ್ರುವ uint16_t* p U_LIFETIME_BOUND) {
#ಯದಿ U_SHOW_CPLUSPLUS_API
    ಪ್ರತ್ಯಾಗಮ ConstChar16Ptr(p).get();
#ಅನ್ಯಥಾ
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ char16_t *>(p);
#endif
}
#endif
#ಯದಿ U_SIZEOF_WCHAR_T==2
/** @internal */
ಅನ್ತಃಸ್ಥ ಧ್ರುವ char16_t* uprv_char16PtrFromWchar(ಧ್ರುವ wchar_t* p U_LIFETIME_BOUND) {
#ಯದಿ U_SHOW_CPLUSPLUS_API
    ಪ್ರತ್ಯಾಗಮ ConstChar16Ptr(p).get();
#ಅನ್ಯಥಾ
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ char16_t *>(p);
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
ಅನ್ತಃಸ್ಥ ಧ್ರುವ UChar* toUCharPtr(ಧ್ರುವ char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
ಅನ್ತಃಸ್ಥ UChar* toUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
ಅನ್ತಃಸ್ಥ ಧ್ರುವ OldUChar* toOldUCharPtr(ಧ್ರುವ char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
ಅನ್ತಃಸ್ಥ OldUChar* toOldUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#endif  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#ಯದಿ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#ifndef U_FORCE_HIDE_INTERNAL_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
template<typename T>
constexpr bool ConvertibleToU16StringView =
    std::is_convertible_v<T, std::u16string_view>
#ಯದಿ !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#endif
#ಯದಿ U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#endif
    ;

namespace internal {
/**
 * Pass-through overload.
 * @internal
 */
ಅನ್ತಃಸ್ಥ std::u16string_view toU16StringView(std::u16string_view sv U_LIFETIME_BOUND) { ಪ್ರತ್ಯಾಗಮ sv; }

#ಯದಿ !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
ಅನ್ತಃಸ್ಥ std::u16string_view toU16StringView(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    ಪ್ರತ್ಯಾಗಮ { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

#ಯದಿ U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
ಅನ್ತಃಸ್ಥ std::u16string_view toU16StringView(std::wstring_view sv U_LIFETIME_BOUND) {
    ಪ್ರತ್ಯಾಗಮ { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

/**
 * Pass-through overload.
 * @internal
 */
template <typename T,
          typename = typename std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>>>
ಅನ್ತಃಸ್ಥ std::u16string_view toU16StringViewNullable(ಧ್ರುವ T& text U_LIFETIME_BOUND) {
    ಪ್ರತ್ಯಾಗಮ toU16StringView(text);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
template <typename T,
          typename = typename std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>>,
          typename = ಶೂನ್ಯ>
ಅನ್ತಃಸ್ಥ std::u16string_view toU16StringViewNullable(ಧ್ರುವ T& text U_LIFETIME_BOUND) {
    ಯದಿ (text == nullptr) ಪ್ರತ್ಯಾಗಮ {};  // For backward compatibility.
    ಪ್ರತ್ಯಾಗಮ toU16StringView(text);
}

}  // internal
#endif  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

#endif  // __CHAR16PTR_H__
