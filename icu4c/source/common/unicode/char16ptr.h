// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#ifndef __CHAR16PTR_H__
#define __CHAR16PTR_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

#ضمن <cstddef>
#ضمن <string_view>
#ضمن <type_traits>

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
#وإلاإذا (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) asm متطاير("" : : "rm"(ptr) : "memory")
#وإلاإذا defined(U_IN_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#endif

// ICU DLL-exported
#إذا U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API Char16Ptr final {
عام:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    مضمن Char16Ptr(char16_t *p);
#إذا !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    مضمن Char16Ptr(uint16_t *p);
#endif
#إذا U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    مضمن Char16Ptr(wchar_t *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    مضمن Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    مضمن ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    مضمن char16_t *get() ثابت;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    مضمن operator char16_t *() ثابت { أرجع get(); }

خاص:
    Char16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    template<typename T> ساكن char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        أرجع reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#وإلا
    اتحاد {
        char16_t *cp;
        uint16_t *up;
        wchar_t *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#إذا !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#endif
#إذا U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) : p_(cast(p)) {}
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() ثابت { أرجع p_; }

#وإلا

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#إذا !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.up = p; }
#endif
#إذا U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) { u_.wp = p; }
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() ثابت { أرجع u_.cp; }

#endif
/// \endcond

/**
 * const char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API ConstChar16Ptr final {
عام:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    مضمن ConstChar16Ptr(ثابت char16_t *p);
#إذا !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    مضمن ConstChar16Ptr(ثابت uint16_t *p);
#endif
#إذا U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    مضمن ConstChar16Ptr(ثابت wchar_t *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    مضمن ConstChar16Ptr(ثابت std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    مضمن ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    مضمن ثابت char16_t *get() ثابت;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    مضمن operator ثابت char16_t *() ثابت { أرجع get(); }

خاص:
    ConstChar16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    template<typename T> ساكن ثابت char16_t *cast(ثابت T *t) {
        U_ALIASING_BARRIER(t);
        أرجع reinterpret_cast<ثابت char16_t *>(t);
    }

    ثابت char16_t *p_;
#وإلا
    اتحاد {
        ثابت char16_t *cp;
        ثابت uint16_t *up;
        ثابت wchar_t *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(ثابت char16_t *p) : p_(p) {}
#إذا !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ثابت uint16_t *p) : p_(cast(p)) {}
#endif
#إذا U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ثابت wchar_t *p) : p_(cast(p)) {}
#endif
ConstChar16Ptr::ConstChar16Ptr(ثابت std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

ثابت char16_t *ConstChar16Ptr::get() ثابت { أرجع p_; }

#وإلا

ConstChar16Ptr::ConstChar16Ptr(ثابت char16_t *p) { u_.cp = p; }
#إذا !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ثابت uint16_t *p) { u_.up = p; }
#endif
#إذا U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ثابت wchar_t *p) { u_.wp = p; }
#endif
ConstChar16Ptr::ConstChar16Ptr(ثابت std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

ثابت char16_t *ConstChar16Ptr::get() ثابت { أرجع u_.cp; }

#endif
/// \endcond

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#إذا U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

namespace U_ICU_NAMESPACE_OR_INTERNAL {

#ifndef U_FORCE_HIDE_INTERNAL_API
/** @internal */
template <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
مضمن ثابت char16_t* uprv_char16PtrFromUChar(ثابت T* p U_LIFETIME_BOUND) {
    إذا constexpr (std::is_same_v<UChar, char16_t>) {
        أرجع p;
    } وإلا {
#إذا U_SHOW_CPLUSPLUS_API
        أرجع ConstChar16Ptr(p).get();
#وإلا
#ifdef U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#endif
        أرجع reinterpret_cast<ثابت char16_t *>(p);
#endif
    }
}
#إذا !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/** @internal */
مضمن ثابت char16_t* uprv_char16PtrFromUint16(ثابت uint16_t* p U_LIFETIME_BOUND) {
#إذا U_SHOW_CPLUSPLUS_API
    أرجع ConstChar16Ptr(p).get();
#وإلا
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    أرجع reinterpret_cast<ثابت char16_t *>(p);
#endif
}
#endif
#إذا U_SIZEOF_WCHAR_T==2
/** @internal */
مضمن ثابت char16_t* uprv_char16PtrFromWchar(ثابت wchar_t* p U_LIFETIME_BOUND) {
#إذا U_SHOW_CPLUSPLUS_API
    أرجع ConstChar16Ptr(p).get();
#وإلا
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    أرجع reinterpret_cast<ثابت char16_t *>(p);
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
مضمن ثابت UChar* toUCharPtr(ثابت char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    أرجع reinterpret_cast<ثابت UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
مضمن UChar* toUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    أرجع reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
مضمن ثابت OldUChar* toOldUCharPtr(ثابت char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    أرجع reinterpret_cast<ثابت OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
مضمن OldUChar* toOldUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    أرجع reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#endif  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#إذا U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#ifndef U_FORCE_HIDE_INTERNAL_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
template<typename T>
constexpr bool ConvertibleToU16StringView =
    std::is_convertible_v<T, std::u16string_view>
#إذا !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#endif
#إذا U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#endif
    ;

namespace internal {
/**
 * Pass-through overload.
 * @internal
 */
مضمن std::u16string_view toU16StringView(std::u16string_view sv U_LIFETIME_BOUND) { أرجع sv; }

#إذا !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
مضمن std::u16string_view toU16StringView(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    أرجع { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

#إذا U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
مضمن std::u16string_view toU16StringView(std::wstring_view sv U_LIFETIME_BOUND) {
    أرجع { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

/**
 * Pass-through overload.
 * @internal
 */
template <typename T,
          typename = typename std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>>>
مضمن std::u16string_view toU16StringViewNullable(ثابت T& text U_LIFETIME_BOUND) {
    أرجع toU16StringView(text);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
template <typename T,
          typename = typename std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>>,
          typename = فراغ>
مضمن std::u16string_view toU16StringViewNullable(ثابت T& text U_LIFETIME_BOUND) {
    إذا (text == nullptr) أرجع {};  // For backward compatibility.
    أرجع toU16StringView(text);
}

}  // internal
#endif  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

#endif  // __CHAR16PTR_H__
