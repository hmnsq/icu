// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#ضمن "unicode/bytestream.h"
#ضمن "unicode/errorcode.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/utypes.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/ulocale.h"
#ضمن "unicode/locid.h"

#ضمن "bytesinkutil.h"
#ضمن "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<ثابت icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ثابت حرف* localeID, int32_t length, UErrorCode* err) {
    إذا (U_FAILURE(*err)) { أرجع nullptr; }
    إذا (length < 0) {
        أرجع EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    أرجع EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(ثابت حرف* tag, int32_t length, UErrorCode* err) {
  إذا (U_FAILURE(*err)) { أرجع nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  إذا (U_FAILURE(*err)) { أرجع nullptr; }
  أرجع EXTERNAL(l.clone());
}

فراغ
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ثابت حرف* ulocale_get ## N1(ثابت ULocale* locale U_LIFETIME_BOUND) { \
    إذا (locale == nullptr) أرجع nullptr; \
    أرجع CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_نيلاي(N) \
int32_t ulocale_get ##N ( \
    ثابت ULocale* locale, ثابت حرف* keyword, int32_t keywordLength, \
    حرف* نيلاي_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    إذا (U_FAILURE(*err)) أرجع 0; \
    إذا (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        أرجع 0; \
    } \
    أرجع ByteSinkUtil::viaByteSinkToTerminatedChars( \
        نيلاي_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ثابت ULocale* locale, UErrorCode *err) { \
    إذا (U_FAILURE(*err)) أرجع nullptr; \
    إذا (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        أرجع nullptr; \
    } \
    أرجع uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_نيلاي(Keyword_نيلاي)
IMPL_ULOCALE_GET_KEYWORD_نيلاي(Unicode_Keyword_نيلاي)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

منطقي ulocale_isBogus(ثابت ULocale* locale) {
    إذا (locale == nullptr) أرجع خطأ;
    أرجع CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
