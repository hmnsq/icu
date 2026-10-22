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
#تعریف_کرو EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#تعریف_کرو CONST_اندرونی(e) (reinterpret_cast<ثابت icu::Locale*>(e))
#تعریف_کرو اندرونی(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ثابت حرف* localeID, int32_t length, UError_کوڈ* err) {
    إذا (U_FAILURE(*err)) { أرجع خالی_سوچک; }
    إذا (length < 0) {
        أرجع EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    أرجع EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_open_For_Language_ٹیگ(ثابت حرف* ٹیگ, int32_t length, UError_کوڈ* err) {
  إذا (U_FAILURE(*err)) { أرجع خالی_سوچک; }
  Locale l = icu::Locale::for_Language_ٹیگ(length < 0 ? StringPiece(ٹیگ) : StringPiece(ٹیگ, length), *err);
  إذا (U_FAILURE(*err)) { أرجع خالی_سوچک; }
  أرجع EXTERNAL(l.clone());
}

فراغ
ulocale_close(ULocale* locale) {
    مٹاؤ اندرونی(locale);
}

#تعریف_کرو IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ثابت حرف* ulocale_get ## N1(ثابت ULocale* locale U_LIFETIME_BOUND) { \
    إذا (locale == خالی_سوچک) أرجع خالی_سوچک; \
    أرجع CONST_اندرونی(locale)->get ## N2(); \
}

#تعریف_کرو IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#تعریف_کرو IMPL_ULOCALE_GET_KEYWORD_قدر(N) \
int32_t ulocale_get ##N ( \
    ثابت ULocale* locale, ثابت حرف* keyword, int32_t keyword_لمائی, \
    حرف* قدر_بفر, int32_t بفر_سمرتھا, UError_کوڈ *err) { \
    إذا (U_FAILURE(*err)) أرجع 0; \
    إذا (locale == خالی_سوچک) { \
        *err = U_ILLEGAL_دلیل_ERROR; \
        أرجع 0; \
    } \
    أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars( \
        قدر_بفر, بفر_سمرتھا, \
        [&](ByteSink& sink, UError_کوڈ& حالت) { \
            CONST_اندرونی(locale)->get ## N( \
                keyword_لمائی < 0 ? StringPiece(keyword) : StringPiece(keyword, keyword_لمائی), \
                sink, حالت); \
        }, \
        *err); \
}

#تعریف_کرو IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ثابت ULocale* locale, UError_کوڈ *err) { \
    إذا (U_FAILURE(*err)) أرجع خالی_سوچک; \
    إذا (locale == خالی_سوچک) { \
        *err = U_ILLEGAL_دلیل_ERROR; \
        أرجع خالی_سوچک; \
    } \
    أرجع uenum_openFromStringEnumeration( \
        CONST_اندرونی(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(ادھار_Name_2)
IMPL_ULOCALE_GET_KEYWORD_قدر(Keyword_قدر)
IMPL_ULOCALE_GET_KEYWORD_قدر(Unicode_Keyword_قدر)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

منطقي ulocale_isBogus(ثابت ULocale* locale) {
    إذا (locale == خالی_سوچک) أرجع خطأ;
    أرجع CONST_اندرونی(locale)->isBogus();
}

/*eof*/
