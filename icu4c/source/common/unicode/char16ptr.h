// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#ifndef __CHAR16PTR_H__
#define __CHAR16PTR_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

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
#अथवायदि (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) यंत्र चञ्चल("" : : "rm"(ptr) : "memory")
#अथवायदि defined(U_IN_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#endif

// ICU DLL-exported
#यदि U_SHOW_CPLUSPLUS_API

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
    अन्तःस्थ Char16Ptr(char16_t *p);
#यदि !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    अन्तःस्थ Char16Ptr(uint16_t *p);
#endif
#यदि U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    अन्तःस्थ Char16Ptr(ब_अक्षर_प्रकार *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    अन्तःस्थ Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    अन्तःस्थ ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    अन्तःस्थ char16_t *get() अचर;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    अन्तःस्थ चालक char16_t *() अचर { फर्काऊ get(); }

गुप्त:
    Char16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    ढाँचा<typename T> स्थिर char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        फर्काऊ reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#अन्यथा
    सङ्घ {
        char16_t *cp;
        uint16_t *up;
        ब_अक्षर_प्रकार *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#यदि !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#endif
#यदि U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(ब_अक्षर_प्रकार *p) : p_(cast(p)) {}
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() अचर { फर्काऊ p_; }

#अन्यथा

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#यदि !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.up = p; }
#endif
#यदि U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(ब_अक्षर_प्रकार *p) { u_.wp = p; }
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() अचर { फर्काऊ u_.cp; }

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
    अन्तःस्थ ConstChar16Ptr(अचर char16_t *p);
#यदि !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    अन्तःस्थ ConstChar16Ptr(अचर uint16_t *p);
#endif
#यदि U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    अन्तःस्थ ConstChar16Ptr(अचर ब_अक्षर_प्रकार *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    अन्तःस्थ ConstChar16Ptr(अचर std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    अन्तःस्थ ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    अन्तःस्थ अचर char16_t *get() अचर;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    अन्तःस्थ चालक अचर char16_t *() अचर { फर्काऊ get(); }

गुप्त:
    ConstChar16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    ढाँचा<typename T> स्थिर अचर char16_t *cast(अचर T *t) {
        U_ALIASING_BARRIER(t);
        फर्काऊ reinterpret_cast<अचर char16_t *>(t);
    }

    अचर char16_t *p_;
#अन्यथा
    सङ्घ {
        अचर char16_t *cp;
        अचर uint16_t *up;
        अचर ब_अक्षर_प्रकार *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(अचर char16_t *p) : p_(p) {}
#यदि !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(अचर uint16_t *p) : p_(cast(p)) {}
#endif
#यदि U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(अचर ब_अक्षर_प्रकार *p) : p_(cast(p)) {}
#endif
ConstChar16Ptr::ConstChar16Ptr(अचर std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

अचर char16_t *ConstChar16Ptr::get() अचर { फर्काऊ p_; }

#अन्यथा

ConstChar16Ptr::ConstChar16Ptr(अचर char16_t *p) { u_.cp = p; }
#यदि !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(अचर uint16_t *p) { u_.up = p; }
#endif
#यदि U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(अचर ब_अक्षर_प्रकार *p) { u_.wp = p; }
#endif
ConstChar16Ptr::ConstChar16Ptr(अचर std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

अचर char16_t *ConstChar16Ptr::get() अचर { फर्काऊ u_.cp; }

#endif
/// \endcond

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#यदि U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

namespace U_ICU_NAMESPACE_OR_INTERNAL {

#ifndef U_FORCE_HIDE_INTERNAL_API
/** @internal */
ढाँचा <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
अन्तःस्थ अचर char16_t* uprv_char16PtrFromUChar(अचर T* p U_LIFETIME_BOUND) {
    यदि constexpr (std::is_same_v<UChar, char16_t>) {
        फर्काऊ p;
    } अन्यथा {
#यदि U_SHOW_CPLUSPLUS_API
        फर्काऊ ConstChar16Ptr(p).get();
#अन्यथा
#ifdef U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#endif
        फर्काऊ reinterpret_cast<अचर char16_t *>(p);
#endif
    }
}
#यदि !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/** @internal */
अन्तःस्थ अचर char16_t* uprv_char16PtrFromUint16(अचर uint16_t* p U_LIFETIME_BOUND) {
#यदि U_SHOW_CPLUSPLUS_API
    फर्काऊ ConstChar16Ptr(p).get();
#अन्यथा
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    फर्काऊ reinterpret_cast<अचर char16_t *>(p);
#endif
}
#endif
#यदि U_SIZEOF_WCHAR_T==2
/** @internal */
अन्तःस्थ अचर char16_t* uprv_char16PtrFromWchar(अचर ब_अक्षर_प्रकार* p U_LIFETIME_BOUND) {
#यदि U_SHOW_CPLUSPLUS_API
    फर्काऊ ConstChar16Ptr(p).get();
#अन्यथा
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    फर्काऊ reinterpret_cast<अचर char16_t *>(p);
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
अन्तःस्थ अचर UChar* toUCharPtr(अचर char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    फर्काऊ reinterpret_cast<अचर UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
अन्तःस्थ UChar* toUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    फर्काऊ reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
अन्तःस्थ अचर OldUChar* toOldUCharPtr(अचर char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    फर्काऊ reinterpret_cast<अचर OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
अन्तःस्थ OldUChar* toOldUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    फर्काऊ reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#endif  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#यदि U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#ifndef U_FORCE_HIDE_INTERNAL_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
ढाँचा<typename T>
constexpr bool ConvertibleToU16StringView =
    std::is_convertible_v<T, std::u16string_view>
#यदि !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#endif
#यदि U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#endif
    ;

namespace internal {
/**
 * Pass-through overload.
 * @internal
 */
अन्तःस्थ std::u16string_view toU16StringView(std::u16string_view sv U_LIFETIME_BOUND) { फर्काऊ sv; }

#यदि !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
अन्तःस्थ std::u16string_view toU16StringView(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    फर्काऊ { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

#यदि U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
अन्तःस्थ std::u16string_view toU16StringView(std::wstring_view sv U_LIFETIME_BOUND) {
    फर्काऊ { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

/**
 * Pass-through overload.
 * @internal
 */
ढाँचा <typename T,
          typename = typename std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>>>
अन्तःस्थ std::u16string_view toU16StringViewNullable(अचर T& text U_LIFETIME_BOUND) {
    फर्काऊ toU16StringView(text);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
ढाँचा <typename T,
          typename = typename std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>>,
          typename = शून्य>
अन्तःस्थ std::u16string_view toU16StringViewNullable(अचर T& text U_LIFETIME_BOUND) {
    यदि (text == nullptr) फर्काऊ {};  // For backward compatibility.
    फर्काऊ toU16StringView(text);
}

}  // internal
#endif  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

#endif  // __CHAR16PTR_H__
