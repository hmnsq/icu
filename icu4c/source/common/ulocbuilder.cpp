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

ورتوں icu::StringPiece;

#تعریف_کرو EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#تعریف_کرو اندرونی(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#تعریف_کرو CONST_اندرونی(e) (reinterpret_cast<ثابت icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    أرجع EXTERNAL(جديد icu::LocaleBuilder());
}

فراغ ulocbld_close(ULocaleBuilder* builder) {
    إذا (builder == خالی_سوچک) أرجع;
    مٹاؤ اندرونی(builder);
}

فراغ ulocbld_سیٹ_Locale(ULocaleBuilder* builder, ثابت حرف* locale, int32_t length) {
    إذا (builder == خالی_سوچک) أرجع;
    icu::Locale l;
    إذا (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } وإلا {
        إذا (length >= ULOC_FULLNAME_سمرتھا) {
            l.سیٹ_منزل_Bogus();
        } وإلا {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            حرف buf[ULOC_FULLNAME_سمرتھا];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    اندرونی(builder)->سیٹ_Locale(l);
}

فراغ
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    إذا (builder == خالی_سوچک) أرجع;
    اندرونی(builder)->سیٹ_Locale(*(reinterpret_cast<ثابت icu::Locale*>(locale)));
    ulocale_close(locale);
}

#تعریف_کرو STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#تعریف_کرو IMPL_ULOCBLD_SETTER(N) \
فراغ ulocbld_##N(ULocaleBuilder* bld, ثابت حرف* s, int32_t l) { \
    إذا (bld == خالی_سوچک) أرجع; \
    اندرونی(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(سیٹ_Language_ٹیگ)
IMPL_ULOCBLD_SETTER(سیٹ_Language)
IMPL_ULOCBLD_SETTER(سیٹ_Script)
IMPL_ULOCBLD_SETTER(سیٹ_Region)
IMPL_ULOCBLD_SETTER(سیٹ_Variant)
IMPL_ULOCBLD_SETTER(جوڑو_Unicode_Locale_Attribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

فراغ ulocbld_سیٹ_Extension(ULocaleBuilder* builder, حرف کلید, ثابت حرف* value, int32_t length) {
    إذا (builder == خالی_سوچک) أرجع;
    اندرونی(builder)->سیٹ_Extension(کلید, STRING_PIECE(value, length));
}

فراغ ulocbld_سیٹ_Unicode_Locale_Keyword(
    ULocaleBuilder* builder, ثابت حرف* کلید, int32_t کلید_لمائی,
    ثابت حرف* type, int32_t type_لمائی) {
    إذا (builder == خالی_سوچک) أرجع;
    اندرونی(builder)->سیٹ_Unicode_Locale_Keyword(
        STRING_PIECE(کلید, کلید_لمائی), STRING_PIECE(type, type_لمائی));
}

فراغ ulocbld_clear(ULocaleBuilder* builder) {
    إذا (builder == خالی_سوچک) أرجع;
    اندرونی(builder)->clear();
}

فراغ ulocbld_clearExtensions(ULocaleBuilder* builder) {
    إذا (builder == خالی_سوچک) أرجع;
    اندرونی(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UError_کوڈ* err) {
    إذا (builder == خالی_سوچک) {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    icu::Locale l = اندرونی(builder)->build(*err);
    إذا (U_FAILURE(*err)) أرجع خالی_سوچک;
    icu::Locale* r = l.clone();
    إذا (r == خالی_سوچک) {
        *err = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    أرجع reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              حرف* بفر, int32_t بفر_سمرتھا, UError_کوڈ* err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    إذا (builder == خالی_سوچک) {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    icu::Locale l = اندرونی(builder)->build(*err);
    إذا (U_FAILURE(*err)) { أرجع 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    إذا (0 < length && length <= بفر_سمرتھا) {
        uprv_memcpy(بفر, l.getName(), length);
    }
    أرجع u_terminateChars(بفر, بفر_سمرتھا, length, err);
}

int32_t ulocbld_build_Language_ٹیگ(ULocaleBuilder* builder,
                  حرف* بفر, int32_t بفر_سمرتھا, UError_کوڈ* err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    إذا (builder == خالی_سوچک) {
        *err = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    icu::Locale l = اندرونی(builder)->build(*err);
    أرجع icu::ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        بفر, بفر_سمرتھا,
        [&](icu::ByteSink& sink, UError_کوڈ& حالت) {
            l.منزل_Language_ٹیگ(sink, حالت);
        },
        *err);
}

UBool ulocbld_copy_Error_منزل(ثابت ULocaleBuilder* builder, UError_کوڈ *باہرلا_Error_کوڈ) {
    إذا (builder == خالی_سوچک) {
        *باہرلا_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع سچ;
    }
    أرجع CONST_اندرونی(builder)->copy_Error_منزل(*باہرلا_Error_کوڈ);
}
