// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __CHAR16PTR_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __CHAR16PTR_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_ಶೀರ್ಷಿಕೆ_API

#ಅನ್ತರ್ಭಾವ <cstddef>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <type_traits>

#ಆದರೆ_ಅಂತ್ಯ

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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER
    // Use the predefined value.
#ಅಥವಾಆದರೆ (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) asm ಅಸ್ಥಿರ("" : : "rm"(ptr) : "memory")
#ಅಥವಾಆದರೆ defined(U_ಒಳ_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#ಆದರೆ_ಅಂತ್ಯ

// ICU DLL-exported
#ಆದರೆ U_SHOW_CPLUSPLUS_API

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
    ಸಾಲೊಳಗೆ Char16Ptr(char16_t *p);
#ಆದರೆ !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ Char16Ptr(uint16_t *p);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ U_SIZEOF_WCHAR_T==2 || defined(U_ಒಳ_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ Char16Ptr(wchar_t *p);
#ಆದರೆ_ಅಂತ್ಯ
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ char16_t *get() ಸ್ಥಿರಾಂಕ;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ ಕ್ರಿಯಾಕಾರಕ char16_t *() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ get(); }

ಖಾಸಗಿ:
    Char16Ptr() = delete;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER
    ಮಾದರಿ<typename T> ಸ್ಥಿರ char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        ಹಿಂತಿರುಗಿ reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#ಇಲ್ಲದಿದ್ದರೆ
    ಒಕ್ಕೂಟ {
        char16_t *cp;
        uint16_t *ಮೇಲೆ;
        wchar_t *wp;
    } u_;
#ಆದರೆ_ಅಂತ್ಯ
};

/// \cond
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#ಆದರೆ !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) : p_(cast(p)) {}
#ಆದರೆ_ಅಂತ್ಯ
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ p_; }

#ಇಲ್ಲದಿದ್ದರೆ

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#ಆದರೆ !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.ಮೇಲೆ = p; }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) { u_.wp = p; }
#ಆದರೆ_ಅಂತ್ಯ
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ u_.cp; }

#ಆದರೆ_ಅಂತ್ಯ
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
    ಸಾಲೊಳಗೆ ConstChar16Ptr(ಸ್ಥಿರಾಂಕ char16_t *p);
#ಆದರೆ !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ ConstChar16Ptr(ಸ್ಥಿರಾಂಕ uint16_t *p);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ U_SIZEOF_WCHAR_T==2 || defined(U_ಒಳ_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ ConstChar16Ptr(ಸ್ಥಿರಾಂಕ wchar_t *p);
#ಆದರೆ_ಅಂತ್ಯ
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ ConstChar16Ptr(ಸ್ಥಿರಾಂಕ std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ char16_t *get() ಸ್ಥಿರಾಂಕ;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ಸಾಲೊಳಗೆ ಕ್ರಿಯಾಕಾರಕ ಸ್ಥಿರಾಂಕ char16_t *() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ get(); }

ಖಾಸಗಿ:
    ConstChar16Ptr() = delete;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER
    ಮಾದರಿ<typename T> ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t *cast(ಸ್ಥಿರಾಂಕ T *t) {
        U_ALIASING_BARRIER(t);
        ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t *>(t);
    }

    ಸ್ಥಿರಾಂಕ char16_t *p_;
#ಇಲ್ಲದಿದ್ದರೆ
    ಒಕ್ಕೂಟ {
        ಸ್ಥಿರಾಂಕ char16_t *cp;
        ಸ್ಥಿರಾಂಕ uint16_t *ಮೇಲೆ;
        ಸ್ಥಿರಾಂಕ wchar_t *wp;
    } u_;
#ಆದರೆ_ಅಂತ್ಯ
};

/// \cond
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(ಸ್ಥಿರಾಂಕ char16_t *p) : p_(p) {}
#ಆದರೆ !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ಸ್ಥಿರಾಂಕ uint16_t *p) : p_(cast(p)) {}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ಸ್ಥಿರಾಂಕ wchar_t *p) : p_(cast(p)) {}
#ಆದರೆ_ಅಂತ್ಯ
ConstChar16Ptr::ConstChar16Ptr(ಸ್ಥಿರಾಂಕ std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

ಸ್ಥಿರಾಂಕ char16_t *ConstChar16Ptr::get() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ p_; }

#ಇಲ್ಲದಿದ್ದರೆ

ConstChar16Ptr::ConstChar16Ptr(ಸ್ಥಿರಾಂಕ char16_t *p) { u_.cp = p; }
#ಆದರೆ !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ಸ್ಥಿರಾಂಕ uint16_t *p) { u_.ಮೇಲೆ = p; }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ಸ್ಥಿರಾಂಕ wchar_t *p) { u_.wp = p; }
#ಆದರೆ_ಅಂತ್ಯ
ConstChar16Ptr::ConstChar16Ptr(ಸ್ಥಿರಾಂಕ std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

ಸ್ಥಿರಾಂಕ char16_t *ConstChar16Ptr::get() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ u_.cp; }

#ಆದರೆ_ಅಂತ್ಯ
/// \endcond

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#ಆದರೆ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_ಶೀರ್ಷಿಕೆ_API

namespace U_ICU_NAMESPACE_OR_ಆಂತರಿಕ {

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_FORCE_HIDE_ಆಂತರಿಕ_API
/** @internal */
ಮಾದರಿ <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ char16_t* uprv_char16PtrFromUChar(ಸ್ಥಿರಾಂಕ T* p U_LIFETIME_BOUND) {
    ಆದರೆ constexpr (std::is_same_v<UChar, char16_t>) {
        ಹಿಂತಿರುಗಿ p;
    } ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ U_SHOW_CPLUSPLUS_API
        ಹಿಂತಿರುಗಿ ConstChar16Ptr(p).get();
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t *>(p);
#ಆದರೆ_ಅಂತ್ಯ
    }
}
#ಆದರೆ !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_ಆವೃತ್ತಿ) || LIBCPP_ಆವೃತ್ತಿ < 180000)
/** @internal */
ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ char16_t* uprv_char16PtrFromUint16(ಸ್ಥಿರಾಂಕ uint16_t* p U_LIFETIME_BOUND) {
#ಆದರೆ U_SHOW_CPLUSPLUS_API
    ಹಿಂತಿರುಗಿ ConstChar16Ptr(p).get();
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t *>(p);
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ U_SIZEOF_WCHAR_T==2
/** @internal */
ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ char16_t* uprv_char16PtrFromWchar(ಸ್ಥಿರಾಂಕ wchar_t* p U_LIFETIME_BOUND) {
#ಆದರೆ U_SHOW_CPLUSPLUS_API
    ಹಿಂತಿರುಗಿ ConstChar16Ptr(p).get();
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t *>(p);
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Converts from const char16_t * to const UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const UChar *
 * @stable ICU 59
 */
ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ UChar* ಗಮ್ಯ_UChar_Ptr(ಸ್ಥಿರಾಂಕ char16_t* p U_LIFETIME_BOUND) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
ಸಾಲೊಳಗೆ UChar* ಗಮ್ಯ_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ OldUChar* ಗಮ್ಯ_Old_UChar_Ptr(ಸ್ಥಿರಾಂಕ char16_t* p U_LIFETIME_BOUND) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
ಸಾಲೊಳಗೆ OldUChar* ಗಮ್ಯ_Old_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#ಆದರೆ_ಅಂತ್ಯ  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#ಆದರೆ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_FORCE_HIDE_ಆಂತರಿಕ_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
ಮಾದರಿ<typename T>
constexpr bool Convertible_ಗಮ್ಯ_U16_String_View =
    std::is_convertible_v<T, std::u16string_view>
#ಆದರೆ !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_ಆವೃತ್ತಿ) || LIBCPP_ಆವೃತ್ತಿ < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#ಆದರೆ_ಅಂತ್ಯ
    ;

namespace ಆಂತರಿಕ_2 {
/**
 * Pass-through overload.
 * @internal
 */
ಸಾಲೊಳಗೆ std::u16string_view ಗಮ್ಯ_U16_String_View(std::u16string_view sv U_LIFETIME_BOUND) { ಹಿಂತಿರುಗಿ sv; }

#ಆದರೆ !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_ಆವೃತ್ತಿ) || LIBCPP_ಆವೃತ್ತಿ < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
ಸಾಲೊಳಗೆ std::u16string_view ಗಮ್ಯ_U16_String_View(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    ಹಿಂತಿರುಗಿ { ConstChar16Ptr(sv.ದತ್ತಾಂಶ()), sv.ಉದ್ದಳತೆ() };
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
ಸಾಲೊಳಗೆ std::u16string_view ಗಮ್ಯ_U16_String_View(std::wstring_view sv U_LIFETIME_BOUND) {
    ಹಿಂತಿರುಗಿ { ConstChar16Ptr(sv.ದತ್ತಾಂಶ()), sv.ಉದ್ದಳತೆ() };
}
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Pass-through overload.
 * @internal
 */
ಮಾದರಿ <typename T,
          typename = typename std::enable_if_t<!std::is_ಸೂಚಕ_v<std::remove_reference_t<T>>>>
ಸಾಲೊಳಗೆ std::u16string_view ಗಮ್ಯ_U16_String_View_Nullable(ಸ್ಥಿರಾಂಕ T& ಪಠ್ಯ U_LIFETIME_BOUND) {
    ಹಿಂತಿರುಗಿ ಗಮ್ಯ_U16_String_View(ಪಠ್ಯ);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
ಮಾದರಿ <typename T,
          typename = typename std::enable_if_t<std::is_ಸೂಚಕ_v<std::remove_reference_t<T>>>,
          typename = ಶೂನ್ಯ>
ಸಾಲೊಳಗೆ std::u16string_view ಗಮ್ಯ_U16_String_View_Nullable(ಸ್ಥಿರಾಂಕ T& ಪಠ್ಯ U_LIFETIME_BOUND) {
    ಆದರೆ (ಪಠ್ಯ == nullptr) ಹಿಂತಿರುಗಿ {};  // For backward compatibility.
    ಹಿಂತಿರುಗಿ ಗಮ್ಯ_U16_String_View(ಪಠ್ಯ);
}

}  //ಆಂತರಿಕ
#ಆದರೆ_ಅಂತ್ಯ  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // U_SHOW_CPLUSPLUS_API

#ಆದರೆ_ಅಂತ್ಯ  // __CHAR16PTR_H__
