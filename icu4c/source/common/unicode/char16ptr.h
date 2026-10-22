// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// char16ptr.h
// created: 2017feb28 Markus W. Scherer

#నిర్వచితం_కాకపోతే __CHAR16PTR_H__
#నిర్వచించు __CHAR16PTR_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_శీర్షిక_API

#అన్తర్భావ <cstddef>
#అన్తర్భావ <string_view>
#అన్తర్భావ <type_traits>

#అయితే_ముగింపు

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
#నిర్వచితమైతే U_ALIASING_BARRIER
    // Use the predefined value.
#లేదావేళ (defined(__clang__) || defined(__GNUC__)) && U_PLATFORM != U_PF_BROWSER_NATIVE_CLIENT
#   define U_ALIASING_BARRIER(ptr) asm అస్థిర("" : : "rm"(ptr) : "memory")
#లేదావేళ defined(U_లోపల_DOXYGEN)
#   define U_ALIASING_BARRIER(ptr)
#అయితే_ముగింపు

// ICU DLL-exported
#ఒకవేళ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API Char16Ptr final {
బహిరంగ:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    అంతఃవరుస Char16Ptr(char16_t *p);
#ఒకవేళ !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    అంతఃవరుస Char16Ptr(uint16_t *p);
#అయితే_ముగింపు
#ఒకవేళ U_SIZEOF_WCHAR_T==2 || defined(U_లోపల_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    అంతఃవరుస Char16Ptr(wchar_t *p);
#అయితే_ముగింపు
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    అంతఃవరుస Char16Ptr(std::nullptr_t p);
    /**
     * Destructor.
     * @stable ICU 59
     */
    అంతఃవరుస ~Char16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    అంతఃవరుస char16_t *get() స్థిరరాశి;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    అంతఃవరుస క్రియాకారకం char16_t *() స్థిరరాశి { తిరిగి get(); }

అంతరంగ:
    Char16Ptr() = delete;

#నిర్వచితమైతే U_ALIASING_BARRIER
    మూస<typename T> స్థిర char16_t *cast(T *t) {
        U_ALIASING_BARRIER(t);
        తిరిగి reinterpret_cast<char16_t *>(t);
    }

    char16_t *p_;
#లేకపోతే
    సంఘం {
        char16_t *cp;
        uint16_t *పైకి;
        wchar_t *wp;
    } u_;
#అయితే_ముగింపు
};

/// \cond
#నిర్వచితమైతే U_ALIASING_BARRIER

Char16Ptr::Char16Ptr(char16_t *p) : p_(p) {}
#ఒకవేళ !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) : p_(cast(p)) {}
#అయితే_ముగింపు
#ఒకవేళ U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) : p_(cast(p)) {}
#అయితే_ముగింపు
Char16Ptr::Char16Ptr(std::nullptr_t p) : p_(p) {}
Char16Ptr::~Char16Ptr() {
    U_ALIASING_BARRIER(p_);
}

char16_t *Char16Ptr::get() స్థిరరాశి { తిరిగి p_; }

#లేకపోతే

Char16Ptr::Char16Ptr(char16_t *p) { u_.cp = p; }
#ఒకవేళ !U_CHAR16_IS_TYPEDEF
Char16Ptr::Char16Ptr(uint16_t *p) { u_.పైకి = p; }
#అయితే_ముగింపు
#ఒకవేళ U_SIZEOF_WCHAR_T==2
Char16Ptr::Char16Ptr(wchar_t *p) { u_.wp = p; }
#అయితే_ముగింపు
Char16Ptr::Char16Ptr(std::nullptr_t p) { u_.cp = p; }
Char16Ptr::~Char16Ptr() {}

char16_t *Char16Ptr::get() స్థిరరాశి { తిరిగి u_.cp; }

#అయితే_ముగింపు
/// \endcond

/**
 * const char16_t * wrapper with implicit conversion from distinct but bit-compatible pointer types.
 * @stable ICU 59
 */
class U_COMMON_API ConstChar16Ptr final {
బహిరంగ:
    /**
     * Copies the pointer.
     * @param p pointer
     * @stable ICU 59
     */
    అంతఃవరుస ConstChar16Ptr(స్థిరరాశి char16_t *p);
#ఒకవేళ !U_CHAR16_IS_TYPEDEF
    /**
     * Converts the pointer to char16_t *.
     * @param p pointer to be converted
     * @stable ICU 59
     */
    అంతఃవరుస ConstChar16Ptr(స్థిరరాశి uint16_t *p);
#అయితే_ముగింపు
#ఒకవేళ U_SIZEOF_WCHAR_T==2 || defined(U_లోపల_DOXYGEN)
    /**
     * Converts the pointer to char16_t *.
     * (Only defined if U_SIZEOF_WCHAR_T==2.)
     * @param p pointer to be converted
     * @stable ICU 59
     */
    అంతఃవరుస ConstChar16Ptr(స్థిరరాశి wchar_t *p);
#అయితే_ముగింపు
    /**
     * nullptr constructor.
     * @param p nullptr
     * @stable ICU 59
     */
    అంతఃవరుస ConstChar16Ptr(స్థిరరాశి std::nullptr_t p);

    /**
     * Destructor.
     * @stable ICU 59
     */
    అంతఃవరుస ~ConstChar16Ptr();

    /**
     * Pointer access.
     * @return the wrapped pointer
     * @stable ICU 59
     */
    అంతఃవరుస స్థిరరాశి char16_t *get() స్థిరరాశి;
    /**
     * char16_t pointer access via type conversion (e.g., static_cast).
     * @return the wrapped pointer
     * @stable ICU 59
     */
    అంతఃవరుస క్రియాకారకం స్థిరరాశి char16_t *() స్థిరరాశి { తిరిగి get(); }

అంతరంగ:
    ConstChar16Ptr() = delete;

#నిర్వచితమైతే U_ALIASING_BARRIER
    మూస<typename T> స్థిర స్థిరరాశి char16_t *cast(స్థిరరాశి T *t) {
        U_ALIASING_BARRIER(t);
        తిరిగి reinterpret_cast<స్థిరరాశి char16_t *>(t);
    }

    స్థిరరాశి char16_t *p_;
#లేకపోతే
    సంఘం {
        స్థిరరాశి char16_t *cp;
        స్థిరరాశి uint16_t *పైకి;
        స్థిరరాశి wchar_t *wp;
    } u_;
#అయితే_ముగింపు
};

/// \cond
#నిర్వచితమైతే U_ALIASING_BARRIER

ConstChar16Ptr::ConstChar16Ptr(స్థిరరాశి char16_t *p) : p_(p) {}
#ఒకవేళ !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(స్థిరరాశి uint16_t *p) : p_(cast(p)) {}
#అయితే_ముగింపు
#ఒకవేళ U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(స్థిరరాశి wchar_t *p) : p_(cast(p)) {}
#అయితే_ముగింపు
ConstChar16Ptr::ConstChar16Ptr(స్థిరరాశి std::nullptr_t p) : p_(p) {}
ConstChar16Ptr::~ConstChar16Ptr() {
    U_ALIASING_BARRIER(p_);
}

స్థిరరాశి char16_t *ConstChar16Ptr::get() స్థిరరాశి { తిరిగి p_; }

#లేకపోతే

ConstChar16Ptr::ConstChar16Ptr(స్థిరరాశి char16_t *p) { u_.cp = p; }
#ఒకవేళ !U_CHAR16_IS_TYPEDEF
ConstChar16Ptr::ConstChar16Ptr(స్థిరరాశి uint16_t *p) { u_.పైకి = p; }
#అయితే_ముగింపు
#ఒకవేళ U_SIZEOF_WCHAR_T==2
ConstChar16Ptr::ConstChar16Ptr(స్థిరరాశి wchar_t *p) { u_.wp = p; }
#అయితే_ముగింపు
ConstChar16Ptr::ConstChar16Ptr(స్థిరరాశి std::nullptr_t p) { u_.cp = p; }
ConstChar16Ptr::~ConstChar16Ptr() {}

స్థిరరాశి char16_t *ConstChar16Ptr::get() స్థిరరాశి { తిరిగి u_.cp; }

#అయితే_ముగింపు
/// \endcond

U_NAMESPACE_చివర

#అయితే_ముగింపు  // U_SHOW_CPLUSPLUS_API

// Usable in header-only definitions
#ఒకవేళ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_శీర్షిక_API

namespace U_ICU_NAMESPACE_OR_అంతర్గత {

#నిర్వచితం_కాకపోతే U_FORCE_HIDE_అంతర్గత_API
/** @internal */
మూస <typename T, typename = std::enable_if_t<std::is_same_v<T, UChar>>>
అంతఃవరుస స్థిరరాశి char16_t* uprv_char16PtrFromUChar(స్థిరరాశి T* p U_LIFETIME_BOUND) {
    ఒకవేళ constexpr (std::is_same_v<UChar, char16_t>) {
        తిరిగి p;
    } లేకపోతే {
#ఒకవేళ U_SHOW_CPLUSPLUS_API
        తిరిగి ConstChar16Ptr(p).get();
#లేకపోతే
#నిర్వచితమైతే U_ALIASING_BARRIER
        U_ALIASING_BARRIER(p);
#అయితే_ముగింపు
        తిరిగి reinterpret_cast<స్థిరరాశి char16_t *>(p);
#అయితే_ముగింపు
    }
}
#ఒకవేళ !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_సంస్కరణ) || LIBCPP_సంస్కరణ < 180000)
/** @internal */
అంతఃవరుస స్థిరరాశి char16_t* uprv_char16PtrFromUint16(స్థిరరాశి uint16_t* p U_LIFETIME_BOUND) {
#ఒకవేళ U_SHOW_CPLUSPLUS_API
    తిరిగి ConstChar16Ptr(p).get();
#లేకపోతే
#నిర్వచితమైతే U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#అయితే_ముగింపు
    తిరిగి reinterpret_cast<స్థిరరాశి char16_t *>(p);
#అయితే_ముగింపు
}
#అయితే_ముగింపు
#ఒకవేళ U_SIZEOF_WCHAR_T==2
/** @internal */
అంతఃవరుస స్థిరరాశి char16_t* uprv_char16PtrFromWchar(స్థిరరాశి wchar_t* p U_LIFETIME_BOUND) {
#ఒకవేళ U_SHOW_CPLUSPLUS_API
    తిరిగి ConstChar16Ptr(p).get();
#లేకపోతే
#నిర్వచితమైతే U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#అయితే_ముగింపు
    తిరిగి reinterpret_cast<స్థిరరాశి char16_t *>(p);
#అయితే_ముగింపు
}
#అయితే_ముగింపు
#అయితే_ముగింపు

/**
 * Converts from const char16_t * to const UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const UChar *
 * @stable ICU 59
 */
అంతఃవరుస స్థిరరాశి UChar* గమ్యం_UChar_Ptr(స్థిరరాశి char16_t* p U_LIFETIME_BOUND) {
#నిర్వచితమైతే U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#అయితే_ముగింపు
    తిరిగి reinterpret_cast<స్థిరరాశి UChar *>(p);
}

/**
 * Converts from char16_t * to UChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as UChar *
 * @stable ICU 59
 */
అంతఃవరుస UChar* గమ్యం_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#నిర్వచితమైతే U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#అయితే_ముగింపు
    తిరిగి reinterpret_cast<UChar *>(p);
}

/**
 * Converts from const char16_t * to const OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as const OldUChar *
 * @stable ICU 59
 */
అంతఃవరుస స్థిరరాశి OldUChar* గమ్యం_Old_UChar_Ptr(స్థిరరాశి char16_t* p U_LIFETIME_BOUND) {
#నిర్వచితమైతే U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#అయితే_ముగింపు
    తిరిగి reinterpret_cast<స్థిరరాశి OldUChar *>(p);
}

/**
 * Converts from char16_t * to OldUChar *.
 * Includes an aliasing barrier if available.
 * @param p pointer
 * @return p as OldUChar *
 * @stable ICU 59
 */
అంతఃవరుస OldUChar* గమ్యం_Old_UChar_Ptr(char16_t* p U_LIFETIME_BOUND) {
#నిర్వచితమైతే U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#అయితే_ముగింపు
    తిరిగి reinterpret_cast<OldUChar *>(p);
}

}  // U_ICU_NAMESPACE_OR_INTERNAL

#అయితే_ముగింపు  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

// ICU DLL-exported
#ఒకవేళ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

#నిర్వచితం_కాకపోతే U_FORCE_HIDE_అంతర్గత_API
/**
 * Is T convertible to a std::u16string_view or some other 16-bit string view?
 * @internal
 */
మూస<typename T>
constexpr bool Convertible_గమ్యం_U16_String_View =
    std::is_convertible_v<T, std::u16string_view>
#ఒకవేళ !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_సంస్కరణ) || LIBCPP_సంస్కరణ < 180000)
    || std::is_convertible_v<T, std::basic_string_view<uint16_t>>
#అయితే_ముగింపు
#ఒకవేళ U_SIZEOF_WCHAR_T==2
    || std::is_convertible_v<T, std::wstring_view>
#అయితే_ముగింపు
    ;

namespace అంతర్గత_2 {
/**
 * Pass-through overload.
 * @internal
 */
అంతఃవరుస std::u16string_view గమ్యం_U16_String_View(std::u16string_view sv U_LIFETIME_BOUND) { తిరిగి sv; }

#ఒకవేళ !U_CHAR16_IS_TYPEDEF && (!defined(LIBCPP_సంస్కరణ) || LIBCPP_సంస్కరణ < 180000)
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::basic_string_view<uint16_t> to std::u16string_view.
 * @internal
 */
అంతఃవరుస std::u16string_view గమ్యం_U16_String_View(std::basic_string_view<uint16_t> sv U_LIFETIME_BOUND) {
    తిరిగి { ConstChar16Ptr(sv.దత్తాంశం()), sv.నిడివి() };
}
#అయితే_ముగింపు

#ఒకవేళ U_SIZEOF_WCHAR_T==2
/**
 * Basically undefined behavior but sometimes necessary conversion
 * from std::wstring_view to std::u16string_view.
 * @internal
 */
అంతఃవరుస std::u16string_view గమ్యం_U16_String_View(std::wstring_view sv U_LIFETIME_BOUND) {
    తిరిగి { ConstChar16Ptr(sv.దత్తాంశం()), sv.నిడివి() };
}
#అయితే_ముగింపు

/**
 * Pass-through overload.
 * @internal
 */
మూస <typename T,
          typename = typename std::enable_if_t<!std::is_సూచిక_v<std::remove_reference_t<T>>>>
అంతఃవరుస std::u16string_view గమ్యం_U16_String_View_Nullable(స్థిరరాశి T& పాఠ్యం U_LIFETIME_BOUND) {
    తిరిగి గమ్యం_U16_String_View(పాఠ్యం);
}

/**
 * In case of nullptr, return an empty view.
 * @internal
 */
మూస <typename T,
          typename = typename std::enable_if_t<std::is_సూచిక_v<std::remove_reference_t<T>>>,
          typename = శూన్య>
అంతఃవరుస std::u16string_view గమ్యం_U16_String_View_Nullable(స్థిరరాశి T& పాఠ్యం U_LIFETIME_BOUND) {
    ఒకవేళ (పాఠ్యం == nullptr) తిరిగి {};  // For backward compatibility.
    తిరిగి గమ్యం_U16_String_View(పాఠ్యం);
}

}  //అంతర్గత
#అయితే_ముగింపు  // U_FORCE_HIDE_INTERNAL_API

U_NAMESPACE_చివర

#అయితే_ముగింపు  // U_SHOW_CPLUSPLUS_API

#అయితే_ముగింపు  // __CHAR16PTR_H__
