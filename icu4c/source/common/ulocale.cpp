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
#تعریف_کرو EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#تعریف_کرو CONST_اندرونی(e) (reinterpret_cast<ثابت icu::Locale*>(e))
#تعریف_کرو اندرونی_2(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ثابت حرف* localeID, int32_t length, UError_رمز* err) {
    اگر (U_FAILURE(*err)) { واپس خالی_اشاریہ; }
    اگر (length < 0) {
        واپس EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    واپس EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_open_For_Language_لیبل(ثابت حرف* لیبل, int32_t length, UError_رمز* err) {
  اگر (U_FAILURE(*err)) { واپس خالی_اشاریہ; }
  Locale l = icu::Locale::for_Language_لیبل(length < 0 ? StringPiece(لیبل) : StringPiece(لیبل, length), *err);
  اگر (U_FAILURE(*err)) { واپس خالی_اشاریہ; }
  واپس EXTERNAL(l.clone());
}

خالی
ulocale_close(ULocale* locale) {
    حذف اندرونی_2(locale);
}

#تعریف_کرو IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ثابت حرف* ulocale_get ## N1(ثابت ULocale* locale U_LIFETIME_BOUND) { \
    اگر (locale == خالی_اشاریہ) واپس خالی_اشاریہ; \
    واپس CONST_اندرونی(locale)->get ## N2(); \
}

#تعریف_کرو IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#تعریف_کرو IMPL_ULOCALE_GET_KEYWORD_قدر(N) \
int32_t ulocale_get ##N ( \
    ثابت ULocale* locale, ثابت حرف* keyword, int32_t keyword_لمبائی, \
    حرف* قدر_بفر, int32_t بفر_استعداد, UError_رمز *err) { \
    اگر (U_FAILURE(*err)) واپس 0; \
    اگر (locale == خالی_اشاریہ) { \
        *err = U_ILLEGAL_دلیل_ERROR; \
        واپس 0; \
    } \
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars( \
        قدر_بفر, بفر_استعداد, \
        [&](ByteSink& sink, UError_رمز& حالت_2) { \
            CONST_اندرونی(locale)->get ## N( \
                keyword_لمبائی < 0 ? StringPiece(keyword) : StringPiece(keyword, keyword_لمبائی), \
                sink, حالت_2); \
        }, \
        *err); \
}

#تعریف_کرو IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ثابت ULocale* locale, UError_رمز *err) { \
    اگر (U_FAILURE(*err)) واپس خالی_اشاریہ; \
    اگر (locale == خالی_اشاریہ) { \
        *err = U_ILLEGAL_دلیل_ERROR; \
        واپس خالی_اشاریہ; \
    } \
    واپس uenum_openFromStringEnumeration( \
        CONST_اندرونی(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(بنیاد_Name_2)
IMPL_ULOCALE_GET_KEYWORD_قدر(Keyword_قدر)
IMPL_ULOCALE_GET_KEYWORD_قدر(Unicode_Keyword_قدر)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

منطقی ulocale_isBogus(ثابت ULocale* locale) {
    اگر (locale == خالی_اشاریہ) واپس جھوٹ;
    واپس CONST_اندرونی(locale)->isBogus();
}

/*eof*/
