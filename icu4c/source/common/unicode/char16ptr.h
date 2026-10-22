// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#ifndef __CHAR16PTR_H__
#define __CHAR16PTR_H__

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

#ଅନ୍ତର୍ଭାଵ <cstddef>
#ଅନ୍ତର୍ଭାଵ <string_view>
#ଅନ୍ତର୍ଭାଵ <type_traits>

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
#ନଚେତଯଦି (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) asm ଅସ୍ଥିର("" : : "rm"(ptr) : "memory")
#ନଚେତଯଦି defined(U_IN_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#endif

// ICU DLL-exported
#ଯଦି U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API Char16Ptr final {
ସର୍ବସାଧାରଣ:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ Char16Ptr(char16_t *p);
#ଯଦି !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ Char16Ptr(uint16_t *p);
#endif
#ଯଦି U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ Char16Ptr(wchar_t *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ char16_t *get() ଅଚଳ;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ operator char16_t *() ଅଚଳ { ଫେରନ୍ତୁ get(); }

ବ୍ୟକ୍ତିଗତ:
    Char16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    template<typename T> ସ୍ଥିର char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        ଫେରନ୍ତୁ reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#ଅନ୍ୟଥା
    ସଂଘ {
        char16_t *cp;
        uint16_t *up;
        wchar_t *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#ଯଦି !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#endif
#ଯଦି U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) : p_(cast(p)) {}
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() ଅଚଳ { ଫେରନ୍ତୁ p_; }

#ଅନ୍ୟଥା

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#ଯଦି !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.up = p; }
#endif
#ଯଦି U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) { u_.wp = p; }
#endif
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() ଅଚଳ { ଫେରନ୍ତୁ u_.cp; }

#endif
/// \endcond

/**
 * const char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API ConstChar16Ptr final {
ସର୍ବସାଧାରଣ:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ ConstChar16Ptr(ଅଚଳ char16_t *p);
#ଯଦି !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ ConstChar16Ptr(ଅଚଳ uint16_t *p);
#endif
#ଯଦି U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ ConstChar16Ptr(ଅଚଳ wchar_t *p);
#endif
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ ConstChar16Ptr(ଅଚଳ std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ ଅଚଳ char16_t *get() ଅଚଳ;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    ଧାଡ଼ିଭିତରେ operator ଅଚଳ char16_t *() ଅଚଳ { ଫେରନ୍ତୁ get(); }

ବ୍ୟକ୍ତିଗତ:
    ConstChar16Ptr() = delete;

#ifdef U_ALIASING_BARRIER
    template<typename T> ସ୍ଥିର ଅଚଳ char16_t *cast(ଅଚଳ T *t) {
        U_ALIASING_BARRIER(t);
        ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ char16_t *>(t);
    }

    ଅଚଳ char16_t *p_;
#ଅନ୍ୟଥା
    ସଂଘ {
        ଅଚଳ char16_t *cp;
        ଅଚଳ uint16_t *up;
        ଅଚଳ wchar_t *wp;
    } u_;
#endif
};

/// \cond
#ifdef U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(ଅଚଳ char16_t *p) : p_(p) {}
#ଯଦି !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ଅଚଳ uint16_t *p) : p_(cast(p)) {}
#endif
#ଯଦି U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ଅଚଳ wchar_t *p) : p_(cast(p)) {}
#endif
ConstChar16Ptr::ConstChar16Ptr(ଅଚଳ std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

ଅଚଳ char16_t *ConstChar16Ptr::get() ଅଚଳ { ଫେରନ୍ତୁ p_; }

#ଅନ୍ୟଥା

ConstChar16Ptr::ConstChar16Ptr(ଅଚଳ char16_t *p) { u_.cp = p; }
#ଯଦି !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ଅଚଳ uint16_t *p) { u_.up = p; }
#endif
#ଯଦି U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ଅଚଳ wchar_t *p) { u_.wp = p; }
#endif
ConstChar16Ptr::ConstChar16Ptr(ଅଚଳ std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

ଅଚଳ char16_t *ConstChar16Ptr::get() ଅଚଳ { ଫେରନ୍ତୁ u_.cp; }

#endif
/// \endcond

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#ଯଦି U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

namespace U_ICU_NAMESPACE_OR_INTERNAL {

#ifndef U_FORCE_HIDE_INTERNAL_API
/** @internal */
template <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
ଧାଡ଼ିଭିତରେ ଅଚଳ char16_t* uprv_char16PtrFromUChar(ଅଚଳ T* p U_LIFETIME_BOUND) {
    ଯଦି constexpr (std::is_same_v<UChar, char16_t>) {
        ଫେରନ୍ତୁ p;
    } ଅନ୍ୟଥା {
#ଯଦି U_SHOW_CPLUSPLUS_API
        ଫେରନ୍ତୁ ConstChar16Ptr(p).get();
#ଅନ୍ୟଥା
#ifdef U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#endif
        ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ char16_t *>(p);
#endif
    }
}
#ଯଦି !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/** @internal */
ଧାଡ଼ିଭିତରେ ଅଚଳ char16_t* uprv_char16PtrFromUint16(ଅଚଳ uint16_t* p U_LIFETIME_BOUND) {
#ଯଦି U_SHOW_CPLUSPLUS_API
    ଫେରନ୍ତୁ ConstChar16Ptr(p).get();
#ଅନ୍ୟଥା
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ char16_t *>(p);
#endif
}
#endif
#ଯଦି U_SIZEOF_WCHAR_T==2
/** @internal */
ଧାଡ଼ିଭିତରେ ଅଚଳ char16_t* uprv_char16PtrFromWchar(ଅଚଳ wchar_t* p U_LIFETIME_BOUND) {
#ଯଦି U_SHOW_CPLUSPLUS_API
    ଫେରନ୍ତୁ ConstChar16Ptr(p).get();
#ଅନ୍ୟଥା
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ char16_t *>(p);
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
ଧାଡ଼ିଭିତରେ ଅଚଳ UChar* toUCharPtr(ଅଚଳ char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
ଧାଡ଼ିଭିତରେ UChar* toUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ଫେରନ୍ତୁ reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
ଧାଡ଼ିଭିତରେ ଅଚଳ OldUChar* toOldUCharPtr(ଅଚଳ char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
ଧାଡ଼ିଭିତରେ OldUChar* toOldUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#ifdef U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#endif
    ଫେରନ୍ତୁ reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#endif  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#ଯଦି U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#ifndef U_FORCE_HIDE_INTERNAL_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
template<typename T>
constexpr bool ConvertibleToU16StringView =
    std::is_convertible_v<T, std::u16string_view>
#ଯଦି !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#endif
#ଯଦି U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#endif
    ;

namespace internal {
/**
 * Pass-through overload.
 * @internal
 */
ଧାଡ଼ିଭିତରେ std::u16string_view toU16StringView(std::u16string_view sv U_LIFETIME_BOUND) { ଫେରନ୍ତୁ sv; }

#ଯଦି !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
ଧାଡ଼ିଭିତରେ std::u16string_view toU16StringView(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    ଫେରନ୍ତୁ { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

#ଯଦି U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
ଧାଡ଼ିଭିତରେ std::u16string_view toU16StringView(std::wstring_view sv U_LIFETIME_BOUND) {
    ଫେରନ୍ତୁ { ConstChar16Ptr(sv.data()), sv.length() };
}
#endif

/**
 * Pass-through overload.
 * @internal
 */
template <typename T,
          typename = typename std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>>>
ଧାଡ଼ିଭିତରେ std::u16string_view toU16StringViewNullable(ଅଚଳ T& text U_LIFETIME_BOUND) {
    ଫେରନ୍ତୁ toU16StringView(text);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
template <typename T,
          typename = typename std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>>,
          typename = ଶୂନ୍ୟ>
ଧାଡ଼ିଭିତରେ std::u16string_view toU16StringViewNullable(ଅଚଳ T& text U_LIFETIME_BOUND) {
    ଯଦି (text == nullptr) ଫେରନ୍ତୁ {};  // For backward compatibility.
    ଫେରନ୍ତୁ toU16StringView(text);
}

}  // internal
#endif  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_END

#endif  // U_SHOW_CPLUSPLUS_API

#endif  // __CHAR16PTR_H__
