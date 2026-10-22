// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#ifndef __CHAR16PTR_H__
#define __CHAR16PTR_H__

#अन्तर्भाव "unicode/utypes.h"

#जर U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

#अन्तर्भाव <cstddef>
#अन्तर्भाव <string_view>
#अन्तर्भाव <type_traits>

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
#किंवाजर (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) यंत्र अस्थिर("" : : "rm"(ptr) : "memory")
#किंवाजर defined(U_IN_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#endif

// ICU DLL-exported
#जर U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API Char16Ptr final {
सार्वजनिक:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    ओळीत Char16Ptr(char16_t *p);
#जर !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ओळीत Char16Ptr(uint16_t *p);
#endif
#जर U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ओळीत Char16Ptr(ब_अक्षर_प्रकार *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    ओळीत Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    ओळीत ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ओळीत char16_t *get() स्थिरांक;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ओळीत चालक char16_t *() स्थिरांक { परत get(); }

खाजगी:
    Char16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    ढाँचा<typename T> स्थिर char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        परत reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#अन्यथा
    संघ {
        char16_t *cp;
        uint16_t *up;
        ब_अक्षर_प्रकार *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#जर !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#endif
#जर U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(ब_अक्षर_प्रकार *p) : p_(cast(p)) {}
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() स्थिरांक { परत p_; }

#अन्यथा

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#जर !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.up = p; }
#endif
#जर U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(ब_अक्षर_प्रकार *p) { u_.wp = p; }
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() स्थिरांक { परत u_.cp; }

#endif
/// \endcond

/**
 * const char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API ConstChar16Ptr final {
सार्वजनिक:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    ओळीत ConstChar16Ptr(स्थिरांक char16_t *p);
#जर !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ओळीत ConstChar16Ptr(स्थिरांक uint16_t *p);
#endif
#जर U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ओळीत ConstChar16Ptr(स्थिरांक ब_अक्षर_प्रकार *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    ओळीत ConstChar16Ptr(स्थिरांक std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    ओळीत ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ओळीत स्थिरांक char16_t *get() स्थिरांक;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ओळीत चालक स्थिरांक char16_t *() स्थिरांक { परत get(); }

खाजगी:
    ConstChar16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    ढाँचा<typename T> स्थिर स्थिरांक char16_t *cast(स्थिरांक T *t) {
        U_ALIASING_BARRIER(t);
        परत reinterpret_cast<स्थिरांक char16_t *>(t);
    }

    स्थिरांक char16_t *p_;
#अन्यथा
    संघ {
        स्थिरांक char16_t *cp;
        स्थिरांक uint16_t *up;
        स्थिरांक ब_अक्षर_प्रकार *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(स्थिरांक char16_t *p) : p_(p) {}
#जर !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(स्थिरांक uint16_t *p) : p_(cast(p)) {}
#endif
#जर U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(स्थिरांक ब_अक्षर_प्रकार *p) : p_(cast(p)) {}
#endif
ConstChar16Ptr::ConstChar16Ptr(स्थिरांक std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

स्थिरांक char16_t *ConstChar16Ptr::get() स्थिरांक { परत p_; }

#अन्यथा

ConstChar16Ptr::ConstChar16Ptr(स्थिरांक char16_t *p) { u_.cp = p; }
#जर !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(स्थिरांक uint16_t *p) { u_.up = p; }
#endif
#जर U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(स्थिरांक ब_अक्षर_प्रकार *p) { u_.wp = p; }
#endif
ConstChar16Ptr::ConstChar16Ptr(स्थिरांक std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

स्थिरांक char16_t *ConstChar16Ptr::get() स्थिरांक { परत u_.cp; }

#endif
/// \endcond

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#जर U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

namespace U_ICU_NAMESPACE_OR_INTERNAL {

#ifndef U_FORCE_HIDE_INTERNAL_API
/** @internal */
ढाँचा <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
ओळीत स्थिरांक char16_t* uprv_char16PtrFromUChar(स्थिरांक T* p U_LIFETIME_BOUND) {
    जर constexpr (std::is_same_v<UChar, char16_t>) {
        परत p;
    } अन्यथा {
#जर U_SHOW_CPLUSPLUS_API
        परत ConstChar16Ptr(p).get();
#अन्यथा
#ifdef U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#endif
        परत reinterpret_cast<स्थिरांक char16_t *>(p);
#endif
    }
}
#जर !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/** @internal */
ओळीत स्थिरांक char16_t* uprv_char16PtrFromUint16(स्थिरांक uint16_t* p U_LIFETIME_BOUND) {
#जर U_SHOW_CPLUSPLUS_API
    परत ConstChar16Ptr(p).get();
#अन्यथा
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    परत reinterpret_cast<स्थिरांक char16_t *>(p);
#endif
}
#endif
#जर U_SIZEOF_WCHAR_T==2
/** @internal */
ओळीत स्थिरांक char16_t* uprv_char16PtrFromWchar(स्थिरांक ब_अक्षर_प्रकार* p U_LIFETIME_BOUND) {
#जर U_SHOW_CPLUSPLUS_API
    परत ConstChar16Ptr(p).get();
#अन्यथा
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    परत reinterpret_cast<स्थिरांक char16_t *>(p);
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
ओळीत स्थिरांक UChar* toUCharPtr(स्थिरांक char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    परत reinterpret_cast<स्थिरांक UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
ओळीत UChar* toUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    परत reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
ओळीत स्थिरांक OldUChar* toOldUCharPtr(स्थिरांक char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    परत reinterpret_cast<स्थिरांक OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
ओळीत OldUChar* toOldUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    परत reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#endif  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#जर U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#ifndef U_FORCE_HIDE_INTERNAL_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
ढाँचा<typename T>
constexpr bool ConvertibleToU16StringView =
    std::is_convertible_v<T, std::u16string_view>
#जर !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#endif
#जर U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#endif
    ;

namespace internal {
/**
 * Pass-through overload.
 * @internal
 */
ओळीत std::u16string_view toU16StringView(std::u16string_view sv U_LIFETIME_BOUND) { परत sv; }

#जर !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
ओळीत std::u16string_view toU16StringView(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    परत { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

#जर U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
ओळीत std::u16string_view toU16StringView(std::wstring_view sv U_LIFETIME_BOUND) {
    परत { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

/**
 * Pass-through overload.
 * @internal
 */
ढाँचा <typename T,
          typename = typename std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>>>
ओळीत std::u16string_view toU16StringViewNullable(स्थिरांक T& text U_LIFETIME_BOUND) {
    परत toU16StringView(text);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
ढाँचा <typename T,
          typename = typename std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>>,
          typename = शून्य>
ओळीत std::u16string_view toU16StringViewNullable(स्थिरांक T& text U_LIFETIME_BOUND) {
    जर (text == nullptr) परत {};  // For backward compatibility.
    परत toU16StringView(text);
}

}  // internal
#endif  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

#endif  // __CHAR16PTR_H__
