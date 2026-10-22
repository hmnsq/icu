// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#यदि_अपरिभाषितम् __CHAR16PTR_H__
#परिभाषय __CHAR16PTR_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_शीर्षकम्_API

#अन्तर्भाव <cstddef>
#अन्तर्भाव <string_view>
#अन्तर्भाव <type_traits>

#यदि_अन्तः

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
#यदि_परिभाषितम् U_ALIASING_BARRIER
    // Use the predefined value.
#नोचेद्यदि (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) asm चञ्चल("" : : "rm"(ptr) : "memory")
#नोचेद्यदि defined(U_अन्तर्_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#यदि_अन्तः

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
#यदि_अन्तः
#यदि U_SIZEOF_WCHAR_T==2 || defined(U_अन्तर्_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    अन्तःस्थ Char16Ptr(wchar_t *p);
#यदि_अन्तः
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
    अन्तःस्थ char16_t *get() ध्रुव;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    अन्तःस्थ संक्रियकः char16_t *() ध्रुव { प्रत्यागम get(); }

गुप्त:
    Char16Ptr() = delete;

#यदि_परिभाषितम् U_ALIASING_BARRIER
    प्रतिरूपम्<typename T> स्थिर char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        प्रत्यागम reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#अन्यथा
    सङ्घ {
        char16_t *cp;
        uint16_t *ऊर्ध्वम्;
        wchar_t *wp;
    } u_;
#यदि_अन्तः
};

/// \cond
#यदि_परिभाषितम् U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#यदि !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#यदि_अन्तः
#यदि U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) : p_(cast(p)) {}
#यदि_अन्तः
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() ध्रुव { प्रत्यागम p_; }

#अन्यथा

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#यदि !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.ऊर्ध्वम् = p; }
#यदि_अन्तः
#यदि U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) { u_.wp = p; }
#यदि_अन्तः
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() ध्रुव { प्रत्यागम u_.cp; }

#यदि_अन्तः
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
    अन्तःस्थ ConstChar16Ptr(ध्रुव char16_t *p);
#यदि !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    अन्तःस्थ ConstChar16Ptr(ध्रुव uint16_t *p);
#यदि_अन्तः
#यदि U_SIZEOF_WCHAR_T==2 || defined(U_अन्तर्_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    अन्तःस्थ ConstChar16Ptr(ध्रुव wchar_t *p);
#यदि_अन्तः
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    अन्तःस्थ ConstChar16Ptr(ध्रुव std::nullptr_t p);

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
    अन्तःस्थ ध्रुव char16_t *get() ध्रुव;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    अन्तःस्थ संक्रियकः ध्रुव char16_t *() ध्रुव { प्रत्यागम get(); }

गुप्त:
    ConstChar16Ptr() = delete;

#यदि_परिभाषितम् U_ALIASING_BARRIER
    प्रतिरूपम्<typename T> स्थिर ध्रुव char16_t *cast(ध्रुव T *t) {
        U_ALIASING_BARRIER(t);
        प्रत्यागम reinterpret_cast<ध्रुव char16_t *>(t);
    }

    ध्रुव char16_t *p_;
#अन्यथा
    सङ्घ {
        ध्रुव char16_t *cp;
        ध्रुव uint16_t *ऊर्ध्वम्;
        ध्रुव wchar_t *wp;
    } u_;
#यदि_अन्तः
};

/// \cond
#यदि_परिभाषितम् U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(ध्रुव char16_t *p) : p_(p) {}
#यदि !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ध्रुव uint16_t *p) : p_(cast(p)) {}
#यदि_अन्तः
#यदि U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ध्रुव wchar_t *p) : p_(cast(p)) {}
#यदि_अन्तः
ConstChar16Ptr::ConstChar16Ptr(ध्रुव std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

ध्रुव char16_t *ConstChar16Ptr::get() ध्रुव { प्रत्यागम p_; }

#अन्यथा

ConstChar16Ptr::ConstChar16Ptr(ध्रुव char16_t *p) { u_.cp = p; }
#यदि !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(ध्रुव uint16_t *p) { u_.ऊर्ध्वम् = p; }
#यदि_अन्तः
#यदि U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(ध्रुव wchar_t *p) { u_.wp = p; }
#यदि_अन्तः
ConstChar16Ptr::ConstChar16Ptr(ध्रुव std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

ध्रुव char16_t *ConstChar16Ptr::get() ध्रुव { प्रत्यागम u_.cp; }

#यदि_अन्तः
/// \endcond

U_NAMESPACE_अन्तः

#यदि_अन्तः  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#यदि U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_शीर्षकम्_API

namespace U_ICU_NAMESPACE_OR_आन्तरिकम् {

#यदि_अपरिभाषितम् U_FORCE_HIDE_आन्तरिकम्_API
/** @internal */
प्रतिरूपम् <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
अन्तःस्थ ध्रुव char16_t* uprv_char16PtrFromUChar(ध्रुव T* p U_LIFETIME_BOUND) {
    यदि constexpr (std::is_same_v<UChar, char16_t>) {
        प्रत्यागम p;
    } अन्यथा {
#यदि U_SHOW_CPLUSPLUS_API
        प्रत्यागम ConstChar16Ptr(p).get();
#अन्यथा
#यदि_परिभाषितम् U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#यदि_अन्तः
        प्रत्यागम reinterpret_cast<ध्रुव char16_t *>(p);
#यदि_अन्तः
    }
}
#यदि !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_संस्करणम्) || LIBCPP_संस्करणम् < 180000)
/** @internal */
अन्तःस्थ ध्रुव char16_t* uprv_char16PtrFromUint16(ध्रुव uint16_t* p U_LIFETIME_BOUND) {
#यदि U_SHOW_CPLUSPLUS_API
    प्रत्यागम ConstChar16Ptr(p).get();
#अन्यथा
#यदि_परिभाषितम् U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#यदि_अन्तः
    प्रत्यागम reinterpret_cast<ध्रुव char16_t *>(p);
#यदि_अन्तः
}
#यदि_अन्तः
#यदि U_SIZEOF_WCHAR_T==2
/** @internal */
अन्तःस्थ ध्रुव char16_t* uprv_char16PtrFromWchar(ध्रुव wchar_t* p U_LIFETIME_BOUND) {
#यदि U_SHOW_CPLUSPLUS_API
    प्रत्यागम ConstChar16Ptr(p).get();
#अन्यथा
#यदि_परिभाषितम् U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#यदि_अन्तः
    प्रत्यागम reinterpret_cast<ध्रुव char16_t *>(p);
#यदि_अन्तः
}
#यदि_अन्तः
#यदि_अन्तः

/**
 * Converts from const char16_t * to const UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const UChar *
 * @stable ICU 59
 */
अन्तःस्थ ध्रुव UChar* गन्तव्यम्_UChar_Ptr(ध्रुव char16_t* p U_LIFETIME_BOUND) {
#यदि_परिभाषितम् U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#यदि_अन्तः
    प्रत्यागम reinterpret_cast<ध्रुव UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
अन्तःस्थ UChar* गन्तव्यम्_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#यदि_परिभाषितम् U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#यदि_अन्तः
    प्रत्यागम reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
अन्तःस्थ ध्रुव OldUChar* गन्तव्यम्_Old_UChar_Ptr(ध्रुव char16_t* p U_LIFETIME_BOUND) {
#यदि_परिभाषितम् U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#यदि_अन्तः
    प्रत्यागम reinterpret_cast<ध्रुव OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
अन्तःस्थ OldUChar* गन्तव्यम्_Old_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#यदि_परिभाषितम् U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#यदि_अन्तः
    प्रत्यागम reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#यदि_अन्तः  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#यदि U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#यदि_अपरिभाषितम् U_FORCE_HIDE_आन्तरिकम्_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
प्रतिरूपम्<typename T>
constexpr bool Convertible_गन्तव्यम्_U16_String_View =
    std::is_convertible_v<T, std::u16string_view>
#यदि !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_संस्करणम्) || LIBCPP_संस्करणम् < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#यदि_अन्तः
#यदि U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#यदि_अन्तः
    ;

namespace आन्तरिकम्_2 {
/**
 * Pass-through overload.
 * @internal
 */
अन्तःस्थ std::u16string_view गन्तव्यम्_U16_String_View(std::u16string_view sv U_LIFETIME_BOUND) { प्रत्यागम sv; }

#यदि !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_संस्करणम्) || LIBCPP_संस्करणम् < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
अन्तःस्थ std::u16string_view गन्तव्यम्_U16_String_View(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    प्रत्यागम { ConstChar16Ptr(sv.दत्त()), sv.दीर्घता() };
}
#यदि_अन्तः

#यदि U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
अन्तःस्थ std::u16string_view गन्तव्यम्_U16_String_View(std::wstring_view sv U_LIFETIME_BOUND) {
    प्रत्यागम { ConstChar16Ptr(sv.दत्त()), sv.दीर्घता() };
}
#यदि_अन्तः

/**
 * Pass-through overload.
 * @internal
 */
प्रतिरूपम् <typename T,
          typename = typename std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>>>
अन्तःस्थ std::u16string_view गन्तव्यम्_U16_String_View_Nullable(ध्रुव T& text U_LIFETIME_BOUND) {
    प्रत्यागम गन्तव्यम्_U16_String_View(text);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
प्रतिरूपम् <typename T,
          typename = typename std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>>,
          typename = शून्य>
अन्तःस्थ std::u16string_view गन्तव्यम्_U16_String_View_Nullable(ध्रुव T& text U_LIFETIME_BOUND) {
    यदि (text == nullptr) प्रत्यागम {};  // For backward compatibility.
    प्रत्यागम गन्तव्यम्_U16_String_View(text);
}

}  //आन्तरिकम्
#यदि_अन्तः  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_अन्तः

#यदि_अन्तः  // U_SHOW_CPLUSPLUS_API

#यदि_अन्तः  // __CHAR16PTR_H__
