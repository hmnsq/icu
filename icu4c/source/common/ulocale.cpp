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
    यदि (U_FAILURE(*err)) { प्रत्यागम nullptr; }
    यदि (length < 0) {
        प्रत्यागम EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    प्रत्यागम EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(ध्रुव वर्ण* tag, int32_t length, UErrorCode* err) {
  यदि (U_FAILURE(*err)) { प्रत्यागम nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  यदि (U_FAILURE(*err)) { प्रत्यागम nullptr; }
  प्रत्यागम EXTERNAL(l.clone());
}

शून्य
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ध्रुव वर्ण* ulocale_get ## N1(ध्रुव ULocale* locale U_LIFETIME_BOUND) { \
    यदि (locale == nullptr) प्रत्यागम nullptr; \
    प्रत्यागम CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_मूल्य(N) \
int32_t ulocale_get ##N ( \
    ध्रुव ULocale* locale, ध्रुव वर्ण* keyword, int32_t keywordLength, \
    वर्ण* मूल्य_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    यदि (U_FAILURE(*err)) प्रत्यागम 0; \
    यदि (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        प्रत्यागम 0; \
    } \
    प्रत्यागम ByteSinkUtil::viaByteSinkToTerminatedChars( \
        मूल्य_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ध्रुव ULocale* locale, UErrorCode *err) { \
    यदि (U_FAILURE(*err)) प्रत्यागम nullptr; \
    यदि (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        प्रत्यागम nullptr; \
    } \
    प्रत्यागम uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_मूल्य(Keyword_मूल्य)
IMPL_ULOCALE_GET_KEYWORD_मूल्य(Unicode_Keyword_मूल्य)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

तार्किक ulocale_isBogus(ध्रुव ULocale* locale) {
    यदि (locale == nullptr) प्रत्यागम असत्य;
    प्रत्यागम CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
