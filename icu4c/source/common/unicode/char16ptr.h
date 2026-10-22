// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#अगर_अघोषित __CHAR16PTR_H__
#घोषणा __CHAR16PTR_H__

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

#समावेश <cstddef>
#समावेश <string_view>
#समावेश <type_traits>

#पूर्ण_अगर

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
#अगर_घोषित U_ALIASING_BARRIER
    // Use the predefined value.
#या_अगर (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) यंत्र अस्थिर("" : : "rm"(ptr) : "memory")
#या_अगर defined(U_IN_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#पूर्ण_अगर

// ICU DLL-exported
#अगर U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API Char16Ptr final {
खुला:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    अंतरभूत Char16Ptr(char16_t *p);
#अगर !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    अंतरभूत Char16Ptr(uint16_t *p);
#पूर्ण_अगर
#अगर U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    अंतरभूत Char16Ptr(ब_अक्षर_प्रकार *p);
#पूर्ण_अगर
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    अंतरभूत Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    अंतरभूत ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    अंतरभूत char16_t *get() स्थिर;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    अंतरभूत चालक char16_t *() स्थिर { वापस get(); }

निजी:
    Char16Ptr() = delete;

#अगर_घोषित U_ALIASING_BARRIER
    ढाँचा<typename T> अटल char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        वापस reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#अन्यथा
    जोड़ {
        char16_t *cp;
        uint16_t *up;
        ब_अक्षर_प्रकार *wp;
    } u_;
#पूर्ण_अगर
};

/// \cond
#अगर_घोषित U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#अगर !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#पूर्ण_अगर
#अगर U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(ब_अक्षर_प्रकार *p) : p_(cast(p)) {}
#पूर्ण_अगर
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() स्थिर { वापस p_; }

#अन्यथा

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#अगर !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.up = p; }
#पूर्ण_अगर
#अगर U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(ब_अक्षर_प्रकार *p) { u_.wp = p; }
#पूर्ण_अगर
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() स्थिर { वापस u_.cp; }

#पूर्ण_अगर
/// \endcond

/**
 * const char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API ConstChar16Ptr final {
खुला:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    अंतरभूत ConstChar16Ptr(स्थिर char16_t *p);
#अगर !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    अंतरभूत ConstChar16Ptr(स्थिर uint16_t *p);
#पूर्ण_अगर
#अगर U_SIZEOF_WCHAR_T==2 || defined(U_IN_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    अंतरभूत ConstChar16Ptr(स्थिर ब_अक्षर_प्रकार *p);
#पूर्ण_अगर
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    अंतरभूत ConstChar16Ptr(स्थिर std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    अंतरभूत ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    अंतरभूत स्थिर char16_t *get() स्थिर;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    अंतरभूत चालक स्थिर char16_t *() स्थिर { वापस get(); }

निजी:
    ConstChar16Ptr() = delete;

#अगर_घोषित U_ALIASING_BARRIER
    ढाँचा<typename T> अटल स्थिर char16_t *cast(स्थिर T *t) {
        U_ALIASING_BARRIER(t);
        वापस reinterpret_cast<स्थिर char16_t *>(t);
    }

    स्थिर char16_t *p_;
#अन्यथा
    जोड़ {
        स्थिर char16_t *cp;
        स्थिर uint16_t *up;
        स्थिर ब_अक्षर_प्रकार *wp;
    } u_;
#पूर्ण_अगर
};

/// \cond
#अगर_घोषित U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(स्थिर char16_t *p) : p_(p) {}
#अगर !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(स्थिर uint16_t *p) : p_(cast(p)) {}
#पूर्ण_अगर
#अगर U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(स्थिर ब_अक्षर_प्रकार *p) : p_(cast(p)) {}
#पूर्ण_अगर
ConstChar16Ptr::ConstChar16Ptr(स्थिर std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

स्थिर char16_t *ConstChar16Ptr::get() स्थिर { वापस p_; }

#अन्यथा

ConstChar16Ptr::ConstChar16Ptr(स्थिर char16_t *p) { u_.cp = p; }
#अगर !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(स्थिर uint16_t *p) { u_.up = p; }
#पूर्ण_अगर
#अगर U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(स्थिर ब_अक्षर_प्रकार *p) { u_.wp = p; }
#पूर्ण_अगर
ConstChar16Ptr::ConstChar16Ptr(स्थिर std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

स्थिर char16_t *ConstChar16Ptr::get() स्थिर { वापस u_.cp; }

#पूर्ण_अगर
/// \endcond

U_NAMESPACE_END

#पूर्ण_अगर  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#अगर U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

namespace U_ICU_NAMESPACE_OR_INTERNAL {

#अगर_अघोषित U_FORCE_HIDE_INTERNAL_API
/** @internal */
ढाँचा <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
अंतरभूत स्थिर char16_t* uprv_char16PtrFromUChar(स्थिर T* p U_LIFETIME_BOUND) {
    अगर constexpr (std::is_same_v<UChar, char16_t>) {
        वापस p;
    } अन्यथा {
#अगर U_SHOW_CPLUSPLUS_API
        वापस ConstChar16Ptr(p).get();
#अन्यथा
#अगर_घोषित U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#पूर्ण_अगर
        वापस reinterpret_cast<स्थिर char16_t *>(p);
#पूर्ण_अगर
    }
}
#अगर !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/** @internal */
अंतरभूत स्थिर char16_t* uprv_char16PtrFromUint16(स्थिर uint16_t* p U_LIFETIME_BOUND) {
#अगर U_SHOW_CPLUSPLUS_API
    वापस ConstChar16Ptr(p).get();
#अन्यथा
#अगर_घोषित U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#पूर्ण_अगर
    वापस reinterpret_cast<स्थिर char16_t *>(p);
#पूर्ण_अगर
}
#पूर्ण_अगर
#अगर U_SIZEOF_WCHAR_T==2
/** @internal */
अंतरभूत स्थिर char16_t* uprv_char16PtrFromWchar(स्थिर ब_अक्षर_प्रकार* p U_LIFETIME_BOUND) {
#अगर U_SHOW_CPLUSPLUS_API
    वापस ConstChar16Ptr(p).get();
#अन्यथा
#अगर_घोषित U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#पूर्ण_अगर
    वापस reinterpret_cast<स्थिर char16_t *>(p);
#पूर्ण_अगर
}
#पूर्ण_अगर
#पूर्ण_अगर

/**
 * Converts from const char16_t * to const UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const UChar *
 * @stable ICU 59
 */
अंतरभूत स्थिर UChar* toUCharPtr(स्थिर char16_t* p U_LIFETIME_BOUND) {
#अगर_घोषित U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#पूर्ण_अगर
    वापस reinterpret_cast<स्थिर UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
अंतरभूत UChar* toUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#अगर_घोषित U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#पूर्ण_अगर
    वापस reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
अंतरभूत स्थिर OldUChar* toOldUCharPtr(स्थिर char16_t* p U_LIFETIME_BOUND) {
#अगर_घोषित U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#पूर्ण_अगर
    वापस reinterpret_cast<स्थिर OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
अंतरभूत OldUChar* toOldUCharPtr(char16_t* p U_LIFETIME_BOUND) {
#अगर_घोषित U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#पूर्ण_अगर
    वापस reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#पूर्ण_अगर  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#अगर U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#अगर_अघोषित U_FORCE_HIDE_INTERNAL_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
ढाँचा<typename T>
constexpr bool ConvertibleToU16StringView =
    std::is_convertible_v<T, std::u16string_view>
#अगर !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#पूर्ण_अगर
#अगर U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#पूर्ण_अगर
    ;

namespace internal {
/**
 * Pass-through overload.
 * @internal
 */
अंतरभूत std::u16string_view toU16StringView(std::u16string_view sv U_LIFETIME_BOUND) { वापस sv; }

#अगर !U_CHAR16_IS_TYPEDEF && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
अंतरभूत std::u16string_view toU16StringView(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    वापस { ConstChar16Ptr(sv.तथ्य()), sv.लंबाई() };
}
#पूर्ण_अगर

#अगर U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
अंतरभूत std::u16string_view toU16StringView(std::wstring_view sv U_LIFETIME_BOUND) {
    वापस { ConstChar16Ptr(sv.तथ्य()), sv.लंबाई() };
}
#पूर्ण_अगर

/**
 * Pass-through overload.
 * @internal
 */
ढाँचा <typename T,
          typename = typename std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<T>>>>
अंतरभूत std::u16string_view toU16StringViewNullable(स्थिर T& text U_LIFETIME_BOUND) {
    वापस toU16StringView(text);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
ढाँचा <typename T,
          typename = typename std::enable_if_t<std::is_pointer_v<std::remove_reference_t<T>>>,
          typename = व्योम>
अंतरभूत std::u16string_view toU16StringViewNullable(स्थिर T& text U_LIFETIME_BOUND) {
    अगर (text == nullptr) वापस {};  // For backward compatibility.
    वापस toU16StringView(text);
}

}  // internal
#पूर्ण_अगर  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_END

#पूर्ण_अगर  // U_SHOW_CPLUSPLUS_API

#पूर्ण_अगर  // __CHAR16PTR_H__
