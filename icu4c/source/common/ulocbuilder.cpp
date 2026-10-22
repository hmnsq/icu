// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#شامل <utility>

#شامل "unicode/bytestream.h"
#شامل "unicode/localebuilder.h"
#شامل "unicode/locid.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/ulocbuilder.h"
#شامل "unicode/utypes.h"
#شامل "bytesinkutil.h"
#شامل "cstring.h"
#شامل "ustr_imp.h"

استعمال icu::StringPiece;

#تعریف_کرو EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#تعریف_کرو اندرونی_2(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#تعریف_کرو CONST_اندرونی(e) (reinterpret_cast<ثابت icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    واپس EXTERNAL(نیا icu::LocaleBuilder());
}

خالی ulocbld_close(ULocaleBuilder* builder) {
    اگر (builder == خالی_اشاریہ) واپس;
    حذف اندرونی_2(builder);
}

خالی ulocbld_سیٹ_Locale(ULocaleBuilder* builder, ثابت حرف* locale, int32_t length) {
    اگر (builder == خالی_اشاریہ) واپس;
    icu::Locale l;
    اگر (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } ورنہ {
        اگر (length >= ULOC_FULLNAME_استعداد) {
            l.سیٹ_مقصد_Bogus();
        } ورنہ {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            حرف buf[ULOC_FULLNAME_استعداد];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    اندرونی_2(builder)->سیٹ_Locale(l);
}

خالی
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    اگر (builder == خالی_اشاریہ) واپس;
    اندرونی_2(builder)->سیٹ_Locale(*(reinterpret_cast<ثابت icu::Locale*>(locale)));
    ulocale_close(locale);
}

#تعریف_کرو STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#تعریف_کرو IMPL_ULOCBLD_SETTER(N) \
خالی ulocbld_##N(ULocaleBuilder* bld, ثابت حرف* s, int32_t l) { \
    اگر (bld == خالی_اشاریہ) واپس; \
    اندرونی_2(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(سیٹ_Language_لیبل)
IMPL_ULOCBLD_SETTER(سیٹ_Language)
IMPL_ULOCBLD_SETTER(سیٹ_Script)
IMPL_ULOCBLD_SETTER(سیٹ_Region)
IMPL_ULOCBLD_SETTER(سیٹ_Variant)
IMPL_ULOCBLD_SETTER(جوڑو_Unicode_Locale_Attribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

خالی ulocbld_سیٹ_Extension(ULocaleBuilder* builder, حرف کلید, ثابت حرف* value, int32_t length) {
    اگر (builder == خالی_اشاریہ) واپس;
    اندرونی_2(builder)->سیٹ_Extension(کلید, STRING_PIECE(value, length));
}

خالی ulocbld_سیٹ_Unicode_Locale_Keyword(
    ULocaleBuilder* builder, ثابت حرف* کلید, int32_t کلید_لمبائی,
    ثابت حرف* type, int32_t type_لمبائی) {
    اگر (builder == خالی_اشاریہ) واپس;
    اندرونی_2(builder)->سیٹ_Unicode_Locale_Keyword(
        STRING_PIECE(کلید, کلید_لمبائی), STRING_PIECE(type, type_لمبائی));
}

خالی ulocbld_clear(ULocaleBuilder* builder) {
    اگر (builder == خالی_اشاریہ) واپس;
    اندرونی_2(builder)->clear();
}

خالی ulocbld_clearExtensions(ULocaleBuilder* builder) {
    اگر (builder == خالی_اشاریہ) واپس;
    اندرونی_2(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UError_رمز* err) {
    اگر (builder == خالی_اشاریہ) {
        *err = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    icu::Locale l = اندرونی_2(builder)->build(*err);
    اگر (U_FAILURE(*err)) واپس خالی_اشاریہ;
    icu::Locale* r = l.clone();
    اگر (r == خالی_اشاریہ) {
        *err = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    واپس reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              حرف* بفر, int32_t بفر_استعداد, UError_رمز* err) {
    اگر (U_FAILURE(*err)) { واپس 0; }
    اگر (builder == خالی_اشاریہ) {
        *err = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    icu::Locale l = اندرونی_2(builder)->build(*err);
    اگر (U_FAILURE(*err)) { واپس 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    اگر (0 < length && length <= بفر_استعداد) {
        uprv_memcpy(بفر, l.getName(), length);
    }
    واپس u_terminateChars(بفر, بفر_استعداد, length, err);
}

int32_t ulocbld_build_Language_لیبل(ULocaleBuilder* builder,
                  حرف* بفر, int32_t بفر_استعداد, UError_رمز* err) {
    اگر (U_FAILURE(*err)) { واپس 0; }
    اگر (builder == خالی_اشاریہ) {
        *err = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    icu::Locale l = اندرونی_2(builder)->build(*err);
    واپس icu::ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        بفر, بفر_استعداد,
        [&](icu::ByteSink& sink, UError_رمز& حالت_2) {
            l.مقصد_Language_لیبل(sink, حالت_2);
        },
        *err);
}

UBool ulocbld_copy_Error_مقصد(ثابت ULocaleBuilder* builder, UError_رمز *برآمد_Error_رمز) {
    اگر (builder == خالی_اشاریہ) {
        *برآمد_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس سچ;
    }
    واپس CONST_اندرونی(builder)->copy_Error_مقصد(*برآمد_Error_رمز);
}
