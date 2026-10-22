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
#تعریف_کن EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#تعریف_کن CONST_داخلی(e) (reinterpret_cast<ثابت icu::Locale*>(e))
#تعریف_کن داخلی(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ثابت حرف* localeID, int32_t length, UError_کد* err) {
    إذا (U_FAILURE(*err)) { أرجع اشاره_تهی; }
    إذا (length < 0) {
        أرجع EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    أرجع EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_open_For_Language_برچسب(ثابت حرف* برچسب, int32_t length, UError_کد* err) {
  إذا (U_FAILURE(*err)) { أرجع اشاره_تهی; }
  Locale l = icu::Locale::for_Language_برچسب(length < 0 ? StringPiece(برچسب) : StringPiece(برچسب, length), *err);
  إذا (U_FAILURE(*err)) { أرجع اشاره_تهی; }
  أرجع EXTERNAL(l.clone());
}

فراغ
ulocale_close(ULocale* locale) {
    حذف داخلی(locale);
}

#تعریف_کن IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ثابت حرف* ulocale_get ## N1(ثابت ULocale* locale U_LIFETIME_BOUND) { \
    إذا (locale == اشاره_تهی) أرجع اشاره_تهی; \
    أرجع CONST_داخلی(locale)->get ## N2(); \
}

#تعریف_کن IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#تعریف_کن IMPL_ULOCALE_GET_KEYWORD_مقدار(N) \
int32_t ulocale_get ##N ( \
    ثابت ULocale* locale, ثابت حرف* keyword, int32_t keyword_درازی, \
    حرف* مقدار_میانگیر, int32_t میانگیر_ظرفیت, UError_کد *err) { \
    إذا (U_FAILURE(*err)) أرجع 0; \
    إذا (locale == اشاره_تهی) { \
        *err = U_ILLEGAL_آرگومان_ERROR; \
        أرجع 0; \
    } \
    أرجع ByteSinkUtil::از_طریق_Byte_Sink_مقصد_Terminated_Chars( \
        مقدار_میانگیر, میانگیر_ظرفیت, \
        [&](ByteSink& sink, UError_کد& وضعیت) { \
            CONST_داخلی(locale)->get ## N( \
                keyword_درازی < 0 ? StringPiece(keyword) : StringPiece(keyword, keyword_درازی), \
                sink, وضعیت); \
        }, \
        *err); \
}

#تعریف_کن IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ثابت ULocale* locale, UError_کد *err) { \
    إذا (U_FAILURE(*err)) أرجع اشاره_تهی; \
    إذا (locale == اشاره_تهی) { \
        *err = U_ILLEGAL_آرگومان_ERROR; \
        أرجع اشاره_تهی; \
    } \
    أرجع uenum_openFromStringEnumeration( \
        CONST_داخلی(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(پایه_Name_2)
IMPL_ULOCALE_GET_KEYWORD_مقدار(Keyword_مقدار)
IMPL_ULOCALE_GET_KEYWORD_مقدار(Unicode_Keyword_مقدار)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

منطقي ulocale_isBogus(ثابت ULocale* locale) {
    إذا (locale == اشاره_تهی) أرجع خطأ;
    أرجع CONST_داخلی(locale)->isBogus();
}

/*eof*/
