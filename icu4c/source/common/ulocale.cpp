// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#அந்தர்பாவ "unicode/bytestream.h"
#அந்தர்பாவ "unicode/errorcode.h"
#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/ulocale.h"
#அந்தர்பாவ "unicode/locid.h"

#அந்தர்பாவ "bytesinkutil.h"
#அந்தர்பாவ "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<மாறிலி icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(மாறிலி எழுத்து* localeID, int32_t length, UErrorCode* err) {
    எனில் (U_FAILURE(*err)) { திரும்பு nullptr; }
    எனில் (length < 0) {
        திரும்பு EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    திரும்பு EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(மாறிலி எழுத்து* tag, int32_t length, UErrorCode* err) {
  எனில் (U_FAILURE(*err)) { திரும்பு nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  எனில் (U_FAILURE(*err)) { திரும்பு nullptr; }
  திரும்பு EXTERNAL(l.clone());
}

வெறுமை
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
மாறிலி எழுத்து* ulocale_get ## N1(மாறிலி ULocale* locale U_LIFETIME_BOUND) { \
    எனில் (locale == nullptr) திரும்பு nullptr; \
    திரும்பு CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_மதிப்பு(N) \
int32_t ulocale_get ##N ( \
    மாறிலி ULocale* locale, மாறிலி எழுத்து* keyword, int32_t keywordLength, \
    எழுத்து* மதிப்பு_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    எனில் (U_FAILURE(*err)) திரும்பு 0; \
    எனில் (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        திரும்பு 0; \
    } \
    திரும்பு ByteSinkUtil::viaByteSinkToTerminatedChars( \
        மதிப்பு_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(மாறிலி ULocale* locale, UErrorCode *err) { \
    எனில் (U_FAILURE(*err)) திரும்பு nullptr; \
    எனில் (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        திரும்பு nullptr; \
    } \
    திரும்பு uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_மதிப்பு(Keyword_மதிப்பு)
IMPL_ULOCALE_GET_KEYWORD_மதிப்பு(Unicode_Keyword_மதிப்பு)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

தருக்க ulocale_isBogus(மாறிலி ULocale* locale) {
    எனில் (locale == nullptr) திரும்பு பொய்;
    திரும்பு CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
