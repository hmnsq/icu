// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#অন্তর্ভাব "unicode/bytestream.h"
#অন্তর্ভাব "unicode/errorcode.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/ulocale.h"
#অন্তর্ভাব "unicode/locid.h"

#অন্তর্ভাব "bytesinkutil.h"
#অন্তর্ভাব "cmemory.h"

U_NAMESPACE_USE
#define EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#define CONST_INTERNAL(e) (reinterpret_cast<ধ্রুব icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ধ্রুব বর্ণ* localeID, int32_t length, UErrorCode* err) {
    যদি (U_FAILURE(*err)) { প্রত্যাগম nullptr; }
    যদি (length < 0) {
        প্রত্যাগম EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    প্রত্যাগম EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(ধ্রুব বর্ণ* tag, int32_t length, UErrorCode* err) {
  যদি (U_FAILURE(*err)) { প্রত্যাগম nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  যদি (U_FAILURE(*err)) { প্রত্যাগম nullptr; }
  প্রত্যাগম EXTERNAL(l.clone());
}

শূন্য
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ধ্রুব বর্ণ* ulocale_get ## N1(ধ্রুব ULocale* locale U_LIFETIME_BOUND) { \
    যদি (locale == nullptr) প্রত্যাগম nullptr; \
    প্রত্যাগম CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_মান(N) \
int32_t ulocale_get ##N ( \
    ধ্রুব ULocale* locale, ধ্রুব বর্ণ* keyword, int32_t keywordLength, \
    বর্ণ* মান_Buffer, int32_t bufferCapacity, UErrorCode *err) { \
    যদি (U_FAILURE(*err)) প্রত্যাগম 0; \
    যদি (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        প্রত্যাগম 0; \
    } \
    প্রত্যাগম ByteSinkUtil::viaByteSinkToTerminatedChars( \
        মান_Buffer, bufferCapacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keywordLength < 0 ? StringPiece(keyword) : StringPiece(keyword, keywordLength), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ধ্রুব ULocale* locale, UErrorCode *err) { \
    যদি (U_FAILURE(*err)) প্রত্যাগম nullptr; \
    যদি (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        প্রত্যাগম nullptr; \
    } \
    প্রত্যাগম uenum_openFromStringEnumeration( \
        CONST_INTERNAL(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(BaseName)
IMPL_ULOCALE_GET_KEYWORD_মান(Keyword_মান)
IMPL_ULOCALE_GET_KEYWORD_মান(Unicode_Keyword_মান)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

তার্কিক ulocale_isBogus(ধ্রুব ULocale* locale) {
    যদি (locale == nullptr) প্রত্যাগম অসত্য;
    প্রত্যাগম CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
