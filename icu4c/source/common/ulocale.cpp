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
#تعریف_کړه EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#تعریف_کړه CONST_دننی(e) (reinterpret_cast<ثابت icu::Locale*>(e))
#تعریف_کړه دننی(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ثابت حرف* localeID, int32_t length, UError_کوډ* err) {
    إذا (U_FAILURE(*err)) { أرجع تش_اشاره; }
    إذا (length < 0) {
        أرجع EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    أرجع EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_open_For_Language_نښان(ثابت حرف* نښان, int32_t length, UError_کوډ* err) {
  إذا (U_FAILURE(*err)) { أرجع تش_اشاره; }
  Locale l = icu::Locale::for_Language_نښان(length < 0 ? StringPiece(نښان) : StringPiece(نښان, length), *err);
  إذا (U_FAILURE(*err)) { أرجع تش_اشاره; }
  أرجع EXTERNAL(l.clone());
}

فراغ
ulocale_close(ULocale* locale) {
    ړنګول دننی(locale);
}

#تعریف_کړه IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ثابت حرف* ulocale_get ## N1(ثابت ULocale* locale U_LIFETIME_BOUND) { \
    إذا (locale == تش_اشاره) أرجع تش_اشاره; \
    أرجع CONST_دننی(locale)->get ## N2(); \
}

#تعریف_کړه IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#تعریف_کړه IMPL_ULOCALE_GET_KEYWORD_ارزښت(N) \
int32_t ulocale_get ##N ( \
    ثابت ULocale* locale, ثابت حرف* keyword, int32_t keyword_اوږدتیا, \
    حرف* ارزښت_بفر, int32_t بفر_ظرفیت, UError_کوډ *err) { \
    إذا (U_FAILURE(*err)) أرجع 0; \
    إذا (locale == تش_اشاره) { \
        *err = U_ILLEGAL_دلیل_ERROR; \
        أرجع 0; \
    } \
    أرجع ByteSinkUtil::له_لارې_Byte_Sink_منزل_Terminated_Chars( \
        ارزښت_بفر, بفر_ظرفیت, \
        [&](ByteSink& sink, UError_کوډ& حالت) { \
            CONST_دننی(locale)->get ## N( \
                keyword_اوږدتیا < 0 ? StringPiece(keyword) : StringPiece(keyword, keyword_اوږدتیا), \
                sink, حالت); \
        }, \
        *err); \
}

#تعریف_کړه IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ثابت ULocale* locale, UError_کوډ *err) { \
    إذا (U_FAILURE(*err)) أرجع تش_اشاره; \
    إذا (locale == تش_اشاره) { \
        *err = U_ILLEGAL_دلیل_ERROR; \
        أرجع تش_اشاره; \
    } \
    أرجع uenum_openFromStringEnumeration( \
        CONST_دننی(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(بنسټ_Name_2)
IMPL_ULOCALE_GET_KEYWORD_ارزښت(Keyword_ارزښت)
IMPL_ULOCALE_GET_KEYWORD_ارزښت(Unicode_Keyword_ارزښت)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

منطقي ulocale_isBogus(ثابت ULocale* locale) {
    إذا (locale == تش_اشاره) أرجع خطأ;
    أرجع CONST_دننی(locale)->isBogus();
}

/*eof*/
