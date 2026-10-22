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
#define CONST_INTERNAL(e) (reinterpret_cast<अचर icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(अचर वर्ण* localeID, int32_t length, UErrorCode* err) {
    यदि (U_FAILURE(*err)) { फर्काऊ nullptr; }
    यदि (length < 0) {
        फर्काऊ EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    फर्काऊ EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(अचर वर्ण* tag, int32_t length, UErrorCode* err) {
  यदि (U_FAILURE(*err)) { फर्काऊ nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  यदि (U_FAILURE(*err)) { फर्काऊ nullptr; }
  फर्काऊ EXTERNAL(l.clone());
}

शून्य
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
अचर वर्ण* ulocale_get ## N1(अचर ULocale* locale U_LIFETIME_BOUND) { \
    यदि (locale == nullptr) फर्काऊ nullptr; \
    फर्काऊ CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_मान(N) \
int32_t ulocale_get ##N ( \
    अचर ULocale* locale, अचर वर्ण* keyword, int32_t keywordLength, \
    वर्ण* मान_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    यदि (U_FAILURE(*err)) फर्काऊ 0; \
    यदि (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        फर्काऊ 0; \
    } \
    फर्काऊ ByteSinkUtil::viaByteSinkToTerminatedChars( \
        मान_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(अचर ULocale* locale, UErrorCode *err) { \
    यदि (U_FAILURE(*err)) फर्काऊ nullptr; \
    यदि (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        फर्काऊ nullptr; \
    } \
    फर्काऊ uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_मान(Keyword_मान)
IMPL_ULOCALE_GET_KEYWORD_मान(Unicode_Keyword_मान)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

तार्किक ulocale_isBogus(अचर ULocale* locale) {
    यदि (locale == nullptr) फर्काऊ असत्य;
    फर्काऊ CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
