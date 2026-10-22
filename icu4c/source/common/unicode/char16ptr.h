// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __CHAR16PTR_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __CHAR16PTR_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_ਸਿਰਲੇਖ_API

#ਅਨ੍ਤਰ੍ਭਾਵ <cstddef>
#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>
#ਅਨ੍ਤਰ੍ਭਾਵ <type_traits>

#ਜੇ_ਅੰਤ

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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER
    // Use the predefined value.
#ਜਾਂਜੇ (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) asm ਅਸਥਿਰ("" : : "rm"(ptr) : "memory")
#ਜਾਂਜੇ defined(U_ਵਿੱਚ_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#ਜੇ_ਅੰਤ

// ICU DLL-exported
#ਜੇ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API Char16Ptr final {
ਜਨਤਕ:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ Char16Ptr(char16_t *p);
#ਜੇ !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ Char16Ptr(uint16_t *p);
#ਜੇ_ਅੰਤ
#ਜੇ U_SIZEOF_WCHAR_T==2 || defined(U_ਵਿੱਚ_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ Char16Ptr(wchar_t *p);
#ਜੇ_ਅੰਤ
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ char16_t *get() ਅਚਲ;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ ਸੰਚਾਲਕ char16_t *() ਅਚਲ { ਵਾਪਸ get(); }

ਨਿਜੀ:
    Char16Ptr() = delete;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER
    ਸਾਂਚਾ<typename T> ਸਥਿਰ char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        ਵਾਪਸ reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#ਨਹੀਂਤਾਂ
    ਸੰਘ {
        char16_t *cp;
        uint16_t *ਉੱਪਰ;
        wchar_t *wp;
    } u_;
#ਜੇ_ਅੰਤ
};

/// \cond
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#ਜੇ !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#ਜੇ_ਅੰਤ
#ਜੇ U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) : p_(cast(p)) {}
#ਜੇ_ਅੰਤ
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() ਅਚਲ { ਵਾਪਸ p_; }

#ਨਹੀਂਤਾਂ

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#ਜੇ !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.ਉੱਪਰ = p; }
#ਜੇ_ਅੰਤ
#ਜੇ U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) { u_.wp = p; }
#ਜੇ_ਅੰਤ
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() ਅਚਲ { ਵਾਪਸ u_.cp; }

#ਜੇ_ਅੰਤ
/// \endcond

/**
 * const char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API ConstChar16Ptr final {
ਜਨਤਕ:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ ConstChar16Ptr(ਅਚਲ char16_t *p);
#ਜੇ !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ ConstChar16Ptr(ਅਚਲ uint16_t *p);
#ਜੇ_ਅੰਤ
#ਜੇ U_SIZEOF_WCHAR_T==2 || defined(U_ਵਿੱਚ_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ ConstChar16Ptr(ਅਚਲ wchar_t *p);
#ਜੇ_ਅੰਤ
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ ConstChar16Ptr(ਅਚਲ std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ ਅਚਲ char16_t *get() ਅਚਲ;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ਸਤਰਅੰਦਰ ਸੰਚਾਲਕ ਅਚਲ char16_t *() ਅਚਲ { ਵਾਪਸ get(); }

ਨਿਜੀ:
    ConstChar16Ptr() = delete;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER
    ਸਾਂਚਾ<typename T> ਸਥਿਰ ਅਚਲ char16_t *cast(ਅਚਲ T *t) {
        U_ALIASING_BARRIER(t);
        ਵਾਪਸ reinterpret_cast<ਅਚਲ char16_t *>(t);
    }

    ਅਚਲ char16_t *p_;
#ਨਹੀਂਤਾਂ
    ਸੰਘ {
        ਅਚਲ char16_t *cp;
        ਅਚਲ uint16_t *ਉੱਪਰ;
        ਅਚਲ wchar_t *wp;
    } u_;
#ਜੇ_ਅੰਤ
};

/// \cond
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(ਅਚਲ char16_t *p) : p_(p) {}
#ਜੇ !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ਅਚਲ uint16_t *p) : p_(cast(p)) {}
#ਜੇ_ਅੰਤ
#ਜੇ U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ਅਚਲ wchar_t *p) : p_(cast(p)) {}
#ਜੇ_ਅੰਤ
ConstChar16Ptr::ConstChar16Ptr(ਅਚਲ std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

ਅਚਲ char16_t *ConstChar16Ptr::get() ਅਚਲ { ਵਾਪਸ p_; }

#ਨਹੀਂਤਾਂ

ConstChar16Ptr::ConstChar16Ptr(ਅਚਲ char16_t *p) { u_.cp = p; }
#ਜੇ !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ਅਚਲ uint16_t *p) { u_.ਉੱਪਰ = p; }
#ਜੇ_ਅੰਤ
#ਜੇ U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ਅਚਲ wchar_t *p) { u_.wp = p; }
#ਜੇ_ਅੰਤ
ConstChar16Ptr::ConstChar16Ptr(ਅਚਲ std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

ਅਚਲ char16_t *ConstChar16Ptr::get() ਅਚਲ { ਵਾਪਸ u_.cp; }

#ਜੇ_ਅੰਤ
/// \endcond

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#ਜੇ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_ਸਿਰਲੇਖ_API

namespace U_ICU_NAMESPACE_OR_ਅੰਦਰੂਨੀ {

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_FORCE_HIDE_ਅੰਦਰੂਨੀ_API
/** @internal */
ਸਾਂਚਾ <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
ਸਤਰਅੰਦਰ ਅਚਲ char16_t* uprv_char16PtrFromUChar(ਅਚਲ T* p U_LIFETIME_BOUND) {
    ਜੇ constexpr (std::is_same_v<UChar, char16_t>) {
        ਵਾਪਸ p;
    } ਨਹੀਂਤਾਂ {
#ਜੇ U_SHOW_CPLUSPLUS_API
        ਵਾਪਸ ConstChar16Ptr(p).get();
#ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#ਜੇ_ਅੰਤ
        ਵਾਪਸ reinterpret_cast<ਅਚਲ char16_t *>(p);
#ਜੇ_ਅੰਤ
    }
}
#ਜੇ !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_ਸੰਸਕਰਣ) || LIBCPP_ਸੰਸਕਰਣ < 180000)
/** @internal */
ਸਤਰਅੰਦਰ ਅਚਲ char16_t* uprv_char16PtrFromUint16(ਅਚਲ uint16_t* p U_LIFETIME_BOUND) {
#ਜੇ U_SHOW_CPLUSPLUS_API
    ਵਾਪਸ ConstChar16Ptr(p).get();
#ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ reinterpret_cast<ਅਚਲ char16_t *>(p);
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ
#ਜੇ U_SIZEOF_WCHAR_T==2
/** @internal */
ਸਤਰਅੰਦਰ ਅਚਲ char16_t* uprv_char16PtrFromWchar(ਅਚਲ wchar_t* p U_LIFETIME_BOUND) {
#ਜੇ U_SHOW_CPLUSPLUS_API
    ਵਾਪਸ ConstChar16Ptr(p).get();
#ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ reinterpret_cast<ਅਚਲ char16_t *>(p);
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

/**
 * Converts from const char16_t * to const UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const UChar *
 * @stable ICU 59
 */
ਸਤਰਅੰਦਰ ਅਚਲ UChar* ਮੰਜ਼ਿਲ_UChar_Ptr(ਅਚਲ char16_t* p U_LIFETIME_BOUND) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ reinterpret_cast<ਅਚਲ UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
ਸਤਰਅੰਦਰ UChar* ਮੰਜ਼ਿਲ_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
ਸਤਰਅੰਦਰ ਅਚਲ OldUChar* ਮੰਜ਼ਿਲ_Old_UChar_Ptr(ਅਚਲ char16_t* p U_LIFETIME_BOUND) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ reinterpret_cast<ਅਚਲ OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
ਸਤਰਅੰਦਰ OldUChar* ਮੰਜ਼ਿਲ_Old_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#ਜੇ_ਅੰਤ  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#ਜੇ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_FORCE_HIDE_ਅੰਦਰੂਨੀ_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
ਸਾਂਚਾ<typename T>
constexpr bool Convertible_ਮੰਜ਼ਿਲ_U16_String_View =
    std::is_convertible_v<T, std::u16string_view>
#ਜੇ !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_ਸੰਸਕਰਣ) || LIBCPP_ਸੰਸਕਰਣ < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#ਜੇ_ਅੰਤ
#ਜੇ U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#ਜੇ_ਅੰਤ
    ;

namespace ਅੰਦਰੂਨੀ_2 {
/**
 * Pass-through overload.
 * @internal
 */
ਸਤਰਅੰਦਰ std::u16string_view ਮੰਜ਼ਿਲ_U16_String_View(std::u16string_view sv U_LIFETIME_BOUND) { ਵਾਪਸ sv; }

#ਜੇ !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_ਸੰਸਕਰਣ) || LIBCPP_ਸੰਸਕਰਣ < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
ਸਤਰਅੰਦਰ std::u16string_view ਮੰਜ਼ਿਲ_U16_String_View(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    ਵਾਪਸ { ConstChar16Ptr(sv.ਅੰਕੜੇ()), sv.ਲੰਮਾਈ() };
}
#ਜੇ_ਅੰਤ

#ਜੇ U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
ਸਤਰਅੰਦਰ std::u16string_view ਮੰਜ਼ਿਲ_U16_String_View(std::wstring_view sv U_LIFETIME_BOUND) {
    ਵਾਪਸ { ConstChar16Ptr(sv.ਅੰਕੜੇ()), sv.ਲੰਮਾਈ() };
}
#ਜੇ_ਅੰਤ

/**
 * Pass-through overload.
 * @internal
 */
ਸਾਂਚਾ <typename T,
          typename = typename std::enable_if_t<!std::is_ਸੂਚਕ_v<std::remove_reference_t<T>>>>
ਸਤਰਅੰਦਰ std::u16string_view ਮੰਜ਼ਿਲ_U16_String_View_Nullable(ਅਚਲ T& ਪਾਠ U_LIFETIME_BOUND) {
    ਵਾਪਸ ਮੰਜ਼ਿਲ_U16_String_View(ਪਾਠ);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
ਸਾਂਚਾ <typename T,
          typename = typename std::enable_if_t<std::is_ਸੂਚਕ_v<std::remove_reference_t<T>>>,
          typename = ਖਾਲੀ>
ਸਤਰਅੰਦਰ std::u16string_view ਮੰਜ਼ਿਲ_U16_String_View_Nullable(ਅਚਲ T& ਪਾਠ U_LIFETIME_BOUND) {
    ਜੇ (ਪਾਠ == nullptr) ਵਾਪਸ {};  // For backward compatibility.
    ਵਾਪਸ ਮੰਜ਼ਿਲ_U16_String_View(ਪਾਠ);
}

}  //ਅੰਦਰੂਨੀ
#ਜੇ_ਅੰਤ  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // U_SHOW_CPLUSPLUS_API

#ਜੇ_ਅੰਤ  // __CHAR16PTR_H__
