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
#define CONST_INTERNAL(e) (reinterpret_cast<ধ্রুবক icu::Locale*>(e))
#define INTERNAL(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ধ্রুবক অক্ষর* localeID, int32_t length, UErrorCode* err) {
    যদি (U_FAILURE(*err)) { ফেরত nullptr; }
    যদি (length < 0) {
        ফেরত EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    ফেরত EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_openForLanguageTag(ধ্রুবক অক্ষর* tag, int32_t length, UErrorCode* err) {
  যদি (U_FAILURE(*err)) { ফেরত nullptr; }
  Locale l = icu::Locale::forLanguageTag(length < 0 ? StringPiece(tag) : StringPiece(tag, length), *err);
  যদি (U_FAILURE(*err)) { ফেরত nullptr; }
  ফেরত EXTERNAL(l.clone());
}

শূন্য
ulocale_close(ULocale* locale) {
    delete INTERNAL(locale);
}

#define IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ধ্রুবক অক্ষর* ulocale_get ## N1(ধ্রুবক ULocale* locale U_LIFETIME_BOUND) { \
    যদি (locale == nullptr) ফেরত nullptr; \
    ফেরত CONST_INTERNAL(locale)->get ## N2(); \
}

#define IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#define IMPL_ULOCALE_GET_KEYWORD_মান(N) \
int32_t ulocale_get ##N ( \
    ধ্রুবক ULocale* locale, ধ্রুবক অক্ষর* keyword, int32_t keyword_দৈর্ঘ্য, \
    অক্ষর* মান_বাফার, int32_t বাফার_Capacity, UErrorCode *err) { \
    যদি (U_FAILURE(*err)) ফেরত 0; \
    যদি (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        ফেরত 0; \
    } \
    ফেরত ByteSinkUtil::viaByteSinkToTerminatedChars( \
        মান_বাফার, বাফার_Capacity, \
        [&](ByteSink& sink, UErrorCode& status) { \
            CONST_INTERNAL(locale)->get ## N( \
                keyword_দৈর্ঘ্য < 0 ? StringPiece(keyword) : StringPiece(keyword, keyword_দৈর্ঘ্য), \
                sink, status); \
        }, \
        *err); \
}

#define IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ধ্রুবক ULocale* locale, UErrorCode *err) { \
    যদি (U_FAILURE(*err)) ফেরত nullptr; \
    যদি (locale == nullptr) { \
        *err = U_ILLEGAL_ARGUMENT_ERROR; \
        ফেরত nullptr; \
    } \
    ফেরত uenum_openFromStringEnumeration( \
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

যৌক্তিক ulocale_isBogus(ধ্রুবক ULocale* locale) {
    যদি (locale == nullptr) ফেরত মিথ্যা;
    ফেরত CONST_INTERNAL(locale)->isBogus();
}

/*eof*/
