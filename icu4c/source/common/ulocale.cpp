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
#وصف_ڪريو EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#وصف_ڪريو CONST_اندروني(e) (reinterpret_cast<ثابت icu::Locale*>(e))
#وصف_ڪريو اندروني(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ثابت حرف* localeID, int32_t length, UError_ڪوڊ* err) {
    إذا (U_FAILURE(*err)) { أرجع خالي_اشارو; }
    إذا (length < 0) {
        أرجع EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    أرجع EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_open_For_Language_ٽيگ(ثابت حرف* ٽيگ, int32_t length, UError_ڪوڊ* err) {
  إذا (U_FAILURE(*err)) { أرجع خالي_اشارو; }
  Locale l = icu::Locale::for_Language_ٽيگ(length < 0 ? StringPiece(ٽيگ) : StringPiece(ٽيگ, length), *err);
  إذا (U_FAILURE(*err)) { أرجع خالي_اشارو; }
  أرجع EXTERNAL(l.clone());
}

فراغ
ulocale_close(ULocale* locale) {
    ميٽيو اندروني(locale);
}

#وصف_ڪريو IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ثابت حرف* ulocale_get ## N1(ثابت ULocale* locale U_LIFETIME_BOUND) { \
    إذا (locale == خالي_اشارو) أرجع خالي_اشارو; \
    أرجع CONST_اندروني(locale)->get ## N2(); \
}

#وصف_ڪريو IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#وصف_ڪريو IMPL_ULOCALE_GET_KEYWORD_قدر(N) \
int32_t ulocale_get ##N ( \
    ثابت ULocale* locale, ثابت حرف* keyword, int32_t keyword_ڊيگهائي, \
    حرف* قدر_بفر, int32_t بفر_صلاحيت, UError_ڪوڊ *err) { \
    إذا (U_FAILURE(*err)) أرجع 0; \
    إذا (locale == خالي_اشارو) { \
        *err = U_ILLEGAL_دليل_ERROR; \
        أرجع 0; \
    } \
    أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars( \
        قدر_بفر, بفر_صلاحيت, \
        [&](ByteSink& sink, UError_ڪوڊ& حالت) { \
            CONST_اندروني(locale)->get ## N( \
                keyword_ڊيگهائي < 0 ? StringPiece(keyword) : StringPiece(keyword, keyword_ڊيگهائي), \
                sink, حالت); \
        }, \
        *err); \
}

#وصف_ڪريو IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ثابت ULocale* locale, UError_ڪوڊ *err) { \
    إذا (U_FAILURE(*err)) أرجع خالي_اشارو; \
    إذا (locale == خالي_اشارو) { \
        *err = U_ILLEGAL_دليل_ERROR; \
        أرجع خالي_اشارو; \
    } \
    أرجع uenum_openFromStringEnumeration( \
        CONST_اندروني(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(بنياد_Name_2)
IMPL_ULOCALE_GET_KEYWORD_قدر(Keyword_قدر)
IMPL_ULOCALE_GET_KEYWORD_قدر(Unicode_Keyword_قدر)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

منطقي ulocale_isBogus(ثابت ULocale* locale) {
    إذا (locale == خالي_اشارو) أرجع خطأ;
    أرجع CONST_اندروني(locale)->isBogus();
}

/*eof*/
