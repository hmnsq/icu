// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#شامل "unicode/bytestream.h"
#شامل "unicode/errorcode.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/utypes.h"
#شامل "unicode/ustring.h"
#شامل "unicode/ulocale.h"
#شامل "unicode/locid.h"

#شامل "bytesinkutil.h"
#شامل "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<ثابت icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ثابت نویسه* localeID, int32_t length, UErrorCode* err) {
    اگر (U_FAILURE(*err)) { بازگشت nullptr; }
    اگر (length < 0) {
        بازگشت EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    بازگشت EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(ثابت نویسه* tag, int32_t length, UErrorCode* err) {
  اگر (U_FAILURE(*err)) { بازگشت nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  اگر (U_FAILURE(*err)) { بازگشت nullptr; }
  بازگشت EXTERNAL(l.clone());
}

تهی
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ثابت نویسه* ulocale_get ## N1(ثابت ULocale* locale U_LIFETIME_BOUND) { \
    اگر (locale == nullptr) بازگشت nullptr; \
    بازگشت CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_مقدار(N) \
int32_t ulocale_get ##N ( \
    ثابت ULocale* locale, ثابت نویسه* keyword, int32_t keywordLength, \
    نویسه* مقدار_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    اگر (U_FAILURE(*err)) بازگشت 0; \
    اگر (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        بازگشت 0; \
    } \
    بازگشت ByteSinkUtil::viaByteSinkToTerminatedChars( \
        مقدار_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ثابت ULocale* locale, UErrorCode *err) { \
    اگر (U_FAILURE(*err)) بازگشت nullptr; \
    اگر (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        بازگشت nullptr; \
    } \
    بازگشت uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_مقدار(Keyword_مقدار)
IMPL_ULOCALE_GET_KEYWORD_مقدار(Unicode_Keyword_مقدار)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

منطقی ulocale_isBogus(ثابت ULocale* locale) {
    اگر (locale == nullptr) بازگشت نادرست;
    بازگشت CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
