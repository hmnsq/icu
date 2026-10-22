// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#ifndef __CHAR16PTR_H__
#define __CHAR16PTR_H__

#include "unicode/utypes.h"

#အကယ်၍ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

#include <cstddef>
#include <string_view>
#include <type_traits>

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
#elif (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) asm ca_ncala("" : : "rm"(ptr) : "memory")
#elif defined(U_IN_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#endif

// ICU DLL-exported
#အကယ်၍ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API Char16Ptr final {
public:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    anta_hstha Char16Ptr(char16_t *p);
#အကယ်၍ !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    anta_hstha Char16Ptr(uint16_t *p);
#endif
#အကယ်၍ U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    anta_hstha Char16Ptr(wchar_t *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    anta_hstha Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    anta_hstha ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    anta_hstha char16_t *get() const;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    anta_hstha operator char16_t *() const { ပြန်ပေး get(); }

private:
    Char16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    template<typename T> static char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        ပြန်ပေး reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#မဟုတ်ပါက
    sa_ngha {
        char16_t *cp;
        uint16_t *up;
        wchar_t *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#အကယ်၍ !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#endif
#အကယ်၍ U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) : p_(cast(p)) {}
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() const { ပြန်ပေး p_; }

#မဟုတ်ပါက

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#အကယ်၍ !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.up = p; }
#endif
#အကယ်၍ U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) { u_.wp = p; }
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() const { ပြန်ပေး u_.cp; }

#endif
/// \endcond

/**
 * const char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API ConstChar16Ptr final {
public:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    anta_hstha ConstChar16Ptr(const char16_t *p);
#အကယ်၍ !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    anta_hstha ConstChar16Ptr(const uint16_t *p);
#endif
#အကယ်၍ U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    anta_hstha ConstChar16Ptr(const wchar_t *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    anta_hstha ConstChar16Ptr(const std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    anta_hstha ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    anta_hstha const char16_t *get() const;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    anta_hstha operator const char16_t *() const { ပြန်ပေး get(); }

private:
    ConstChar16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    template<typename T> static const char16_t *cast(const T *t) {
        U_ALIASING_BARRIER(t);
        ပြန်ပေး reinterpret_cast<const char16_t *>(t);
    }

    const char16_t *p_;
#မဟုတ်ပါက
    sa_ngha {
        const char16_t *cp;
        const uint16_t *up;
        const wchar_t *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(const char16_t *p) : p_(p) {}
#အကယ်၍ !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(const uint16_t *p) : p_(cast(p)) {}
#endif
#အကယ်၍ U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(const wchar_t *p) : p_(cast(p)) {}
#endif
ConstChar16Ptr::ConstChar16Ptr(const std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

const char16_t *ConstChar16Ptr::get() const { ပြန်ပေး p_; }

#မဟုတ်ပါက

ConstChar16Ptr::ConstChar16Ptr(const char16_t *p) { u_.cp = p; }
#အကယ်၍ !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(const uint16_t *p) { u_.up = p; }
#endif
#အကယ်၍ U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(const wchar_t *p) { u_.wp = p; }
#endif
ConstChar16Ptr::ConstChar16Ptr(const std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

const char16_t *ConstChar16Ptr::get() const { ပြန်ပေး u_.cp; }

#endif
/// \endcond

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#အကယ်၍ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

namespace U_ICU_NAMESPACE_OR_INTERNAL {

#ifndef U_FORCE_HIDE_INTERNAL_API
/** @internal */
template <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
anta_hstha const char16_t* uprv_char16PtrFromUChar(const T* p U_LIFETIME_BOUND) {
    အကယ်၍ constexpr (std::is_same_v<UChar, char16_t>) {
        ပြန်ပေး p;
    } မဟုတ်ပါက {
#အကယ်၍ U_SHOW_CPLUSPLUS_API
        ပြန်ပေး ConstChar16Ptr(p).get();
#မဟုတ်ပါက
#ifdef U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#endif
        ပြန်ပေး reinterpret_cast<const char16_t *>(p);
#endif
    }
}
#အကယ်၍ !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/** @internal */
anta_hstha const char16_t* uprv_char16PtrFromUint16(const uint16_t* p U_LIFETIME_BOUND) {
#အကယ်၍ U_SHOW_CPLUSPLUS_API
    ပြန်ပေး ConstChar16Ptr(p).get();
#မဟုတ်ပါက
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ပြန်ပေး reinterpret_cast<const char16_t *>(p);
#endif
}
#endif
#အကယ်၍ U_SIZEOF_WCHAR_T==2
/** @internal */
anta_hstha const char16_t* uprv_char16PtrFromWchar(const wchar_t* p U_LIFETIME_BOUND) {
#အကယ်၍ U_SHOW_CPLUSPLUS_API
    ပြန်ပေး ConstChar16Ptr(p).get();
#မဟုတ်ပါက
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ပြန်ပေး reinterpret_cast<const char16_t *>(p);
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
anta_hstha const UChar* toUCharPtr(const char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ပြန်ပေး reinterpret_cast<const UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
anta_hstha UChar* toUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ပြန်ပေး reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
anta_hstha const OldUChar* toOldUCharPtr(const char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ပြန်ပေး reinterpret_cast<const OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
anta_hstha OldUChar* toOldUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ပြန်ပေး reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#endif  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#အကယ်၍ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#ifndef U_FORCE_HIDE_INTERNAL_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
template<typename T>
constexpr bool ConvertibleToU16StringView =
    std::is_convertible_v<T, std::u16string_view>
#အကယ်၍ !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#endif
#အကယ်၍ U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#endif
    ;

namespace internal {
/**
 * Pass-through overload.
 * @internal
 */
anta_hstha std::u16string_view toU16StringView(std::u16string_view sv U_LIFETIME_BOUND) { ပြန်ပေး sv; }

#အကယ်၍ !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
anta_hstha std::u16string_view toU16StringView(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    ပြန်ပေး { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

#အကယ်၍ U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
anta_hstha std::u16string_view toU16StringView(std::wstring_view sv U_LIFETIME_BOUND) {
    ပြန်ပေး { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

/**
 * Pass-through overload.
 * @internal
 */
template <typename T,
          typename = typename std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>>>
anta_hstha std::u16string_view toU16StringViewNullable(const T& text U_LIFETIME_BOUND) {
    ပြန်ပေး toU16StringView(text);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
template <typename T,
          typename = typename std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>>,
          typename = void>
anta_hstha std::u16string_view toU16StringViewNullable(const T& text U_LIFETIME_BOUND) {
    အကယ်၍ (text == nullptr) ပြန်ပေး {};  // For backward compatibility.
    ပြန်ပေး toU16StringView(text);
}

}  // internal
#endif  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

#endif  // __CHAR16PTR_H__
