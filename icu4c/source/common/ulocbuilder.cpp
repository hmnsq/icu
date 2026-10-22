// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن <utility>

#ضمن "unicode/bytestream.h"
#ضمن "unicode/localebuilder.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/ulocbuilder.h"
#ضمن "unicode/utypes.h"
#ضمن "bytesinkutil.h"
#ضمن "cstring.h"
#ضمن "ustr_imp.h"

کارول icu::StringPiece;

#تعریف_کړه EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#تعریف_کړه دننی(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#تعریف_کړه CONST_دننی(e) (reinterpret_cast<ثابت icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    أرجع EXTERNAL(جديد icu::LocaleBuilder());
}

فراغ ulocbld_close(ULocaleBuilder* builder) {
    إذا (builder == تش_اشاره) أرجع;
    ړنګول دننی(builder);
}

فراغ ulocbld_سېټ_Locale(ULocaleBuilder* builder, ثابت حرف* locale, int32_t length) {
    إذا (builder == تش_اشاره) أرجع;
    icu::Locale l;
    إذا (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } وإلا {
        إذا (length >= ULOC_FULLNAME_ظرفیت) {
            l.سېټ_منزل_Bogus();
        } وإلا {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            حرف buf[ULOC_FULLNAME_ظرفیت];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    دننی(builder)->سېټ_Locale(l);
}

فراغ
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    إذا (builder == تش_اشاره) أرجع;
    دننی(builder)->سېټ_Locale(*(reinterpret_cast<ثابت icu::Locale*>(locale)));
    ulocale_close(locale);
}

#تعریف_کړه STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#تعریف_کړه IMPL_ULOCBLD_SETTER(N) \
فراغ ulocbld_##N(ULocaleBuilder* bld, ثابت حرف* s, int32_t l) { \
    إذا (bld == تش_اشاره) أرجع; \
    دننی(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(سېټ_Language_نښان)
IMPL_ULOCBLD_SETTER(سېټ_Language)
IMPL_ULOCBLD_SETTER(سېټ_Script)
IMPL_ULOCBLD_SETTER(سېټ_Region)
IMPL_ULOCBLD_SETTER(سېټ_Variant)
IMPL_ULOCBLD_SETTER(جمع_کړه_Unicode_Locale_Attribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

فراغ ulocbld_سېټ_Extension(ULocaleBuilder* builder, حرف کلی, ثابت حرف* value, int32_t length) {
    إذا (builder == تش_اشاره) أرجع;
    دننی(builder)->سېټ_Extension(کلی, STRING_PIECE(value, length));
}

فراغ ulocbld_سېټ_Unicode_Locale_Keyword(
    ULocaleBuilder* builder, ثابت حرف* کلی, int32_t کلی_اوږدتیا,
    ثابت حرف* type, int32_t type_اوږدتیا) {
    إذا (builder == تش_اشاره) أرجع;
    دننی(builder)->سېټ_Unicode_Locale_Keyword(
        STRING_PIECE(کلی, کلی_اوږدتیا), STRING_PIECE(type, type_اوږدتیا));
}

فراغ ulocbld_clear(ULocaleBuilder* builder) {
    إذا (builder == تش_اشاره) أرجع;
    دننی(builder)->clear();
}

فراغ ulocbld_clearExtensions(ULocaleBuilder* builder) {
    إذا (builder == تش_اشاره) أرجع;
    دننی(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UError_کوډ* err) {
    إذا (builder == تش_اشاره) {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    icu::Locale l = دننی(builder)->build(*err);
    إذا (U_FAILURE(*err)) أرجع تش_اشاره;
    icu::Locale* r = l.clone();
    إذا (r == تش_اشاره) {
        *err = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    أرجع reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              حرف* بفر, int32_t بفر_ظرفیت, UError_کوډ* err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    إذا (builder == تش_اشاره) {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    icu::Locale l = دننی(builder)->build(*err);
    إذا (U_FAILURE(*err)) { أرجع 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    إذا (0 < length && length <= بفر_ظرفیت) {
        uprv_memcpy(بفر, l.getName(), length);
    }
    أرجع u_terminateChars(بفر, بفر_ظرفیت, length, err);
}

int32_t ulocbld_build_Language_نښان(ULocaleBuilder* builder,
                  حرف* بفر, int32_t بفر_ظرفیت, UError_کوډ* err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    إذا (builder == تش_اشاره) {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    icu::Locale l = دننی(builder)->build(*err);
    أرجع icu::ByteSinkUtil::له_لارې_Byte_Sink_منزل_Terminated_Chars(
        بفر, بفر_ظرفیت,
        [&](icu::ByteSink& sink, UError_کوډ& حالت) {
            l.منزل_Language_نښان(sink, حالت);
        },
        *err);
}

UBool ulocbld_copy_Error_منزل(ثابت ULocaleBuilder* builder, UError_کوډ *بهر_Error_کوډ) {
    إذا (builder == تش_اشاره) {
        *بهر_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع ریښتیا;
    }
    أرجع CONST_دننی(builder)->copy_Error_منزل(*بهر_Error_کوډ);
}
