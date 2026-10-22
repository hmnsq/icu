// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/errorcode.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ulocale.h"
#अन्तर्भाव "unicode/locid.h"

#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<ध्रुव icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ध्रुव वर्ण* localeID, int32_t length, UErrorCode* err) {
    सचे (U_FAILURE(*err)) { पटिनिवत्त nullptr; }
    सचे (length < 0) {
        पटिनिवत्त EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    पटिनिवत्त EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(ध्रुव वर्ण* tag, int32_t length, UErrorCode* err) {
  सचे (U_FAILURE(*err)) { पटिनिवत्त nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  सचे (U_FAILURE(*err)) { पटिनिवत्त nullptr; }
  पटिनिवत्त EXTERNAL(l.clone());
}

शून्य
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ध्रुव वर्ण* ulocale_get ## N1(ध्रुव ULocale* locale U_LIFETIME_BOUND) { \
    सचे (locale == nullptr) पटिनिवत्त nullptr; \
    पटिनिवत्त CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_मानम्(N) \
int32_t ulocale_get ##N ( \
    ध्रुव ULocale* locale, ध्रुव वर्ण* keyword, int32_t keywordLength, \
    वर्ण* मानम्_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    सचे (U_FAILURE(*err)) पटिनिवत्त 0; \
    सचे (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        पटिनिवत्त 0; \
    } \
    पटिनिवत्त ByteSinkUtil::viaByteSinkToTerminatedChars( \
        मानम्_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ध्रुव ULocale* locale, UErrorCode *err) { \
    सचे (U_FAILURE(*err)) पटिनिवत्त nullptr; \
    सचे (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        पटिनिवत्त nullptr; \
    } \
    पटिनिवत्त uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_मानम्(Keyword_मानम्)
IMPL_ULOCALE_GET_KEYWORD_मानम्(Unicode_Keyword_मानम्)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

तार्किक ulocale_isBogus(ध्रुव ULocale* locale) {
    सचे (locale == nullptr) पटिनिवत्त मिच्छा;
    पटिनिवत्त CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
