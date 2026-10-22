// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#اگر_تعریف_نشده __CHAR16PTR_H__
#تعریف_کن __CHAR16PTR_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_سرآیند_API

#ضمن <cstddef>
#ضمن <string_view>
#ضمن <type_traits>

#پایان_اگر

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
#اگر_تعریف_شده U_ALIASING_BARRIER
    // Use the predefined value.
#وإلاإذا (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) asm متطاير("" : : "rm"(ptr) : "memory")
#وإلاإذا defined(U_درون_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#پایان_اگر

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
#پایان_اگر
#إذا U_SIZEOF_WCHAR_T==2 || defined(U_درون_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    مضمن Char16Ptr(wchar_t *p);
#پایان_اگر
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
    مضمن عملگر char16_t *() ثابت { أرجع get(); }

خاص:
    Char16Ptr() = delete;

#اگر_تعریف_شده U_ALIASING_BARRIER
    قالب<typename T> ساكن char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        أرجع reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#وإلا
    اتحاد {
        char16_t *cp;
        uint16_t *به_بالا;
        wchar_t *wp;
    } u_;
#پایان_اگر
};

/// \cond
#اگر_تعریف_شده U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#إذا !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#پایان_اگر
#إذا U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) : p_(cast(p)) {}
#پایان_اگر
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() ثابت { أرجع p_; }

#وإلا

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#إذا !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.به_بالا = p; }
#پایان_اگر
#إذا U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) { u_.wp = p; }
#پایان_اگر
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() ثابت { أرجع u_.cp; }

#پایان_اگر
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
#پایان_اگر
#إذا U_SIZEOF_WCHAR_T==2 || defined(U_درون_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    مضمن ConstChar16Ptr(ثابت wchar_t *p);
#پایان_اگر
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
    مضمن عملگر ثابت char16_t *() ثابت { أرجع get(); }

خاص:
    ConstChar16Ptr() = delete;

#اگر_تعریف_شده U_ALIASING_BARRIER
    قالب<typename T> ساكن ثابت char16_t *cast(ثابت T *t) {
        U_ALIASING_BARRIER(t);
        أرجع reinterpret_cast<ثابت char16_t *>(t);
    }

    ثابت char16_t *p_;
#وإلا
    اتحاد {
        ثابت char16_t *cp;
        ثابت uint16_t *به_بالا;
        ثابت wchar_t *wp;
    } u_;
#پایان_اگر
};

/// \cond
#اگر_تعریف_شده U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(ثابت char16_t *p) : p_(p) {}
#إذا !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ثابت uint16_t *p) : p_(cast(p)) {}
#پایان_اگر
#إذا U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ثابت wchar_t *p) : p_(cast(p)) {}
#پایان_اگر
ConstChar16Ptr::ConstChar16Ptr(ثابت std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

ثابت char16_t *ConstChar16Ptr::get() ثابت { أرجع p_; }

#وإلا

ConstChar16Ptr::ConstChar16Ptr(ثابت char16_t *p) { u_.cp = p; }
#إذا !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ثابت uint16_t *p) { u_.به_بالا = p; }
#پایان_اگر
#إذا U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ثابت wchar_t *p) { u_.wp = p; }
#پایان_اگر
ConstChar16Ptr::ConstChar16Ptr(ثابت std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

ثابت char16_t *ConstChar16Ptr::get() ثابت { أرجع u_.cp; }

#پایان_اگر
/// \endcond

U_NAMESPACE_انتها

#پایان_اگر  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#إذا U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_سرآیند_API

namespace U_ICU_NAMESPACE_OR_داخلی {

#اگر_تعریف_نشده U_FORCE_HIDE_داخلی_API
/** @internal */
قالب <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
مضمن ثابت char16_t* uprv_char16PtrFromUChar(ثابت T* p U_LIFETIME_BOUND) {
    إذا constexpr (std::is_same_v<UChar, char16_t>) {
        أرجع p;
    } وإلا {
#إذا U_SHOW_CPLUSPLUS_API
        أرجع ConstChar16Ptr(p).get();
#وإلا
#اگر_تعریف_شده U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#پایان_اگر
        أرجع reinterpret_cast<ثابت char16_t *>(p);
#پایان_اگر
    }
}
#إذا !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_نسخه) || LIBCPP_نسخه < 180000)
/** @internal */
مضمن ثابت char16_t* uprv_char16PtrFromUint16(ثابت uint16_t* p U_LIFETIME_BOUND) {
#إذا U_SHOW_CPLUSPLUS_API
    أرجع ConstChar16Ptr(p).get();
#وإلا
#اگر_تعریف_شده U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#پایان_اگر
    أرجع reinterpret_cast<ثابت char16_t *>(p);
#پایان_اگر
}
#پایان_اگر
#إذا U_SIZEOF_WCHAR_T==2
/** @internal */
مضمن ثابت char16_t* uprv_char16PtrFromWchar(ثابت wchar_t* p U_LIFETIME_BOUND) {
#إذا U_SHOW_CPLUSPLUS_API
    أرجع ConstChar16Ptr(p).get();
#وإلا
#اگر_تعریف_شده U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#پایان_اگر
    أرجع reinterpret_cast<ثابت char16_t *>(p);
#پایان_اگر
}
#پایان_اگر
#پایان_اگر

/**
 * Converts from const char16_t * to const UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const UChar *
 * @stable ICU 59
 */
مضمن ثابت UChar* مقصد_UChar_Ptr(ثابت char16_t* p U_LIFETIME_BOUND) {
#اگر_تعریف_شده U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#پایان_اگر
    أرجع reinterpret_cast<ثابت UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
مضمن UChar* مقصد_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#اگر_تعریف_شده U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#پایان_اگر
    أرجع reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
مضمن ثابت OldUChar* مقصد_Old_UChar_Ptr(ثابت char16_t* p U_LIFETIME_BOUND) {
#اگر_تعریف_شده U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#پایان_اگر
    أرجع reinterpret_cast<ثابت OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
مضمن OldUChar* مقصد_Old_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#اگر_تعریف_شده U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#پایان_اگر
    أرجع reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#پایان_اگر  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#إذا U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#اگر_تعریف_نشده U_FORCE_HIDE_داخلی_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
قالب<typename T>
constexpr bool Convertible_مقصد_U16_String_View =
    std::is_convertible_v<T, std::u16string_view>
#إذا !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_نسخه) || LIBCPP_نسخه < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#پایان_اگر
#إذا U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#پایان_اگر
    ;

namespace داخلی_2 {
/**
 * Pass-through overload.
 * @internal
 */
مضمن std::u16string_view مقصد_U16_String_View(std::u16string_view sv U_LIFETIME_BOUND) { أرجع sv; }

#إذا !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_نسخه) || LIBCPP_نسخه < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
مضمن std::u16string_view مقصد_U16_String_View(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    أرجع { ConstChar16Ptr(sv.داده()), sv.درازی() };
}
#پایان_اگر

#إذا U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
مضمن std::u16string_view مقصد_U16_String_View(std::wstring_view sv U_LIFETIME_BOUND) {
    أرجع { ConstChar16Ptr(sv.داده()), sv.درازی() };
}
#پایان_اگر

/**
 * Pass-through overload.
 * @internal
 */
قالب <typename T,
          typename = typename std::enable_if_t<!std::is_اشاره_گر_v<std::remove_reference_t<T>>>>
مضمن std::u16string_view مقصد_U16_String_View_Nullable(ثابت T& متن U_LIFETIME_BOUND) {
    أرجع مقصد_U16_String_View(متن);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
قالب <typename T,
          typename = typename std::enable_if_t<std::is_اشاره_گر_v<std::remove_reference_t<T>>>,
          typename = فراغ>
مضمن std::u16string_view مقصد_U16_String_View_Nullable(ثابت T& متن U_LIFETIME_BOUND) {
    إذا (متن == nullptr) أرجع {};  // For backward compatibility.
    أرجع مقصد_U16_String_View(متن);
}

}  //داخلی
#پایان_اگر  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_انتها

#پایان_اگر  // U_SHOW_CPLUSPLUS_API

#پایان_اگر  // __CHAR16PTR_H__
