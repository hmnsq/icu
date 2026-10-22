// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#اگر_غیر_تعریف_شدہ __CHAR16PTR_H__
#تعریف_کرو __CHAR16PTR_H__

#شامل "unicode/utypes.h"

#اگر U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_سرخی_API

#شامل <cstddef>
#شامل <string_view>
#شامل <type_traits>

#اگر_ختم

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
#اگر_تعریف_شدہ U_ALIASING_BARRIER
    // Use the predefined value.
#وگرنہ_اگر (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) asm متلون("" : : "rm"(ptr) : "memory")
#وگرنہ_اگر defined(U_اندر_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#اگر_ختم

// ICU DLL-exported
#اگر U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API Char16Ptr final {
عوامی:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    اندرونی Char16Ptr(char16_t *p);
#اگر !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    اندرونی Char16Ptr(uint16_t *p);
#اگر_ختم
#اگر U_SIZEOF_WCHAR_T==2 || defined(U_اندر_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    اندرونی Char16Ptr(wchar_t *p);
#اگر_ختم
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    اندرونی Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    اندرونی ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    اندرونی char16_t *get() ثابت;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    اندرونی عامل char16_t *() ثابت { واپس get(); }

نجی:
    Char16Ptr() = delete;

#اگر_تعریف_شدہ U_ALIASING_BARRIER
    سانچہ<typename T> ساکن char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        واپس reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#ورنہ
    اتحاد {
        char16_t *cp;
        uint16_t *اوپر;
        wchar_t *wp;
    } u_;
#اگر_ختم
};

/// \cond
#اگر_تعریف_شدہ U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#اگر !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#اگر_ختم
#اگر U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) : p_(cast(p)) {}
#اگر_ختم
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() ثابت { واپس p_; }

#ورنہ

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#اگر !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.اوپر = p; }
#اگر_ختم
#اگر U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) { u_.wp = p; }
#اگر_ختم
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() ثابت { واپس u_.cp; }

#اگر_ختم
/// \endcond

/**
 * const char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API ConstChar16Ptr final {
عوامی:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    اندرونی ConstChar16Ptr(ثابت char16_t *p);
#اگر !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    اندرونی ConstChar16Ptr(ثابت uint16_t *p);
#اگر_ختم
#اگر U_SIZEOF_WCHAR_T==2 || defined(U_اندر_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    اندرونی ConstChar16Ptr(ثابت wchar_t *p);
#اگر_ختم
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    اندرونی ConstChar16Ptr(ثابت std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    اندرونی ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    اندرونی ثابت char16_t *get() ثابت;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    اندرونی عامل ثابت char16_t *() ثابت { واپس get(); }

نجی:
    ConstChar16Ptr() = delete;

#اگر_تعریف_شدہ U_ALIASING_BARRIER
    سانچہ<typename T> ساکن ثابت char16_t *cast(ثابت T *t) {
        U_ALIASING_BARRIER(t);
        واپس reinterpret_cast<ثابت char16_t *>(t);
    }

    ثابت char16_t *p_;
#ورنہ
    اتحاد {
        ثابت char16_t *cp;
        ثابت uint16_t *اوپر;
        ثابت wchar_t *wp;
    } u_;
#اگر_ختم
};

/// \cond
#اگر_تعریف_شدہ U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(ثابت char16_t *p) : p_(p) {}
#اگر !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ثابت uint16_t *p) : p_(cast(p)) {}
#اگر_ختم
#اگر U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ثابت wchar_t *p) : p_(cast(p)) {}
#اگر_ختم
ConstChar16Ptr::ConstChar16Ptr(ثابت std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

ثابت char16_t *ConstChar16Ptr::get() ثابت { واپس p_; }

#ورنہ

ConstChar16Ptr::ConstChar16Ptr(ثابت char16_t *p) { u_.cp = p; }
#اگر !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ثابت uint16_t *p) { u_.اوپر = p; }
#اگر_ختم
#اگر U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ثابت wchar_t *p) { u_.wp = p; }
#اگر_ختم
ConstChar16Ptr::ConstChar16Ptr(ثابت std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

ثابت char16_t *ConstChar16Ptr::get() ثابت { واپس u_.cp; }

#اگر_ختم
/// \endcond

U_NAMESPACE_انتہا

#اگر_ختم  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#اگر U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_سرخی_API

namespace U_ICU_NAMESPACE_OR_اندرونی {

#اگر_غیر_تعریف_شدہ U_FORCE_HIDE_اندرونی_API
/** @internal */
سانچہ <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
اندرونی ثابت char16_t* uprv_char16PtrFromUChar(ثابت T* p U_LIFETIME_BOUND) {
    اگر constexpr (std::is_same_v<UChar, char16_t>) {
        واپس p;
    } ورنہ {
#اگر U_SHOW_CPLUSPLUS_API
        واپس ConstChar16Ptr(p).get();
#ورنہ
#اگر_تعریف_شدہ U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#اگر_ختم
        واپس reinterpret_cast<ثابت char16_t *>(p);
#اگر_ختم
    }
}
#اگر !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_نسخہ) || LIBCPP_نسخہ < 180000)
/** @internal */
اندرونی ثابت char16_t* uprv_char16PtrFromUint16(ثابت uint16_t* p U_LIFETIME_BOUND) {
#اگر U_SHOW_CPLUSPLUS_API
    واپس ConstChar16Ptr(p).get();
#ورنہ
#اگر_تعریف_شدہ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#اگر_ختم
    واپس reinterpret_cast<ثابت char16_t *>(p);
#اگر_ختم
}
#اگر_ختم
#اگر U_SIZEOF_WCHAR_T==2
/** @internal */
اندرونی ثابت char16_t* uprv_char16PtrFromWchar(ثابت wchar_t* p U_LIFETIME_BOUND) {
#اگر U_SHOW_CPLUSPLUS_API
    واپس ConstChar16Ptr(p).get();
#ورنہ
#اگر_تعریف_شدہ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#اگر_ختم
    واپس reinterpret_cast<ثابت char16_t *>(p);
#اگر_ختم
}
#اگر_ختم
#اگر_ختم

/**
 * Converts from const char16_t * to const UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const UChar *
 * @stable ICU 59
 */
اندرونی ثابت UChar* مقصد_UChar_Ptr(ثابت char16_t* p U_LIFETIME_BOUND) {
#اگر_تعریف_شدہ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#اگر_ختم
    واپس reinterpret_cast<ثابت UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
اندرونی UChar* مقصد_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#اگر_تعریف_شدہ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#اگر_ختم
    واپس reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
اندرونی ثابت OldUChar* مقصد_Old_UChar_Ptr(ثابت char16_t* p U_LIFETIME_BOUND) {
#اگر_تعریف_شدہ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#اگر_ختم
    واپس reinterpret_cast<ثابت OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
اندرونی OldUChar* مقصد_Old_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#اگر_تعریف_شدہ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#اگر_ختم
    واپس reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#اگر_ختم  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#اگر U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#اگر_غیر_تعریف_شدہ U_FORCE_HIDE_اندرونی_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
سانچہ<typename T>
constexpr bool Convertible_مقصد_U16_String_View =
    std::is_convertible_v<T, std::u16string_view>
#اگر !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_نسخہ) || LIBCPP_نسخہ < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#اگر_ختم
#اگر U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#اگر_ختم
    ;

namespace اندرونی_3 {
/**
 * Pass-through overload.
 * @internal
 */
اندرونی std::u16string_view مقصد_U16_String_View(std::u16string_view sv U_LIFETIME_BOUND) { واپس sv; }

#اگر !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_نسخہ) || LIBCPP_نسخہ < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
اندرونی std::u16string_view مقصد_U16_String_View(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    واپس { ConstChar16Ptr(sv.ڈیٹا()), sv.لمبائی() };
}
#اگر_ختم

#اگر U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
اندرونی std::u16string_view مقصد_U16_String_View(std::wstring_view sv U_LIFETIME_BOUND) {
    واپس { ConstChar16Ptr(sv.ڈیٹا()), sv.لمبائی() };
}
#اگر_ختم

/**
 * Pass-through overload.
 * @internal
 */
سانچہ <typename T,
          typename = typename std::enable_if_t<!std::is_اشارہ_گر_v<std::remove_reference_t<T>>>>
اندرونی std::u16string_view مقصد_U16_String_View_Nullable(ثابت T& متن U_LIFETIME_BOUND) {
    واپس مقصد_U16_String_View(متن);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
سانچہ <typename T,
          typename = typename std::enable_if_t<std::is_اشارہ_گر_v<std::remove_reference_t<T>>>,
          typename = خالی>
اندرونی std::u16string_view مقصد_U16_String_View_Nullable(ثابت T& متن U_LIFETIME_BOUND) {
    اگر (متن == nullptr) واپس {};  // For backward compatibility.
    واپس مقصد_U16_String_View(متن);
}

}  //اندرونی
#اگر_ختم  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_انتہا

#اگر_ختم  // U_SHOW_CPLUSPLUS_API

#اگر_ختم  // __CHAR16PTR_H__
