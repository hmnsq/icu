// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ਅਨ੍ਤਰ੍ਭਾਵ <utility>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localebuilder.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ulocbuilder.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

ਵਰਤੋਂ icu::StringPiece;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਦਰੂਨੀ(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONST_ਅੰਦਰੂਨੀ(e) (reinterpret_cast<ਅਚਲ icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    ਵਾਪਸ EXTERNAL(ਨਵਾਂ icu::LocaleBuilder());
}

ਖਾਲੀ ulocbld_close(ULocaleBuilder* builder) {
    ਜੇ (builder == ਖਾਲੀ_ਸੂਚਕ) ਵਾਪਸ;
    ਮਿਟਾਓ ਅੰਦਰੂਨੀ(builder);
}

ਖਾਲੀ ulocbld_ਸੈੱਟ_Locale(ULocaleBuilder* builder, ਅਚਲ ਅੱਖਰ* locale, int32_t length) {
    ਜੇ (builder == ਖਾਲੀ_ਸੂਚਕ) ਵਾਪਸ;
    icu::Locale l;
    ਜੇ (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } ਨਹੀਂਤਾਂ {
        ਜੇ (length >= ULOC_FULLNAME_ਸਮਰੱਥਾ) {
            l.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        } ਨਹੀਂਤਾਂ {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            ਅੱਖਰ buf[ULOC_FULLNAME_ਸਮਰੱਥਾ];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    ਅੰਦਰੂਨੀ(builder)->ਸੈੱਟ_Locale(l);
}

ਖਾਲੀ
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    ਜੇ (builder == ਖਾਲੀ_ਸੂਚਕ) ਵਾਪਸ;
    ਅੰਦਰੂਨੀ(builder)->ਸੈੱਟ_Locale(*(reinterpret_cast<ਅਚਲ icu::Locale*>(locale)));
    ulocale_close(locale);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMPL_ULOCBLD_SETTER(N) \
ਖਾਲੀ ulocbld_##N(ULocaleBuilder* bld, ਅਚਲ ਅੱਖਰ* s, int32_t l) { \
    ਜੇ (bld == ਖਾਲੀ_ਸੂਚਕ) ਵਾਪਸ; \
    ਅੰਦਰੂਨੀ(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(ਸੈੱਟ_Language_ਟੈਗ)
IMPL_ULOCBLD_SETTER(ਸੈੱਟ_Language)
IMPL_ULOCBLD_SETTER(ਸੈੱਟ_Script)
IMPL_ULOCBLD_SETTER(ਸੈੱਟ_Region)
IMPL_ULOCBLD_SETTER(ਸੈੱਟ_Variant)
IMPL_ULOCBLD_SETTER(ਜੋੜੋ_Unicode_Locale_Attribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

ਖਾਲੀ ulocbld_ਸੈੱਟ_Extension(ULocaleBuilder* builder, ਅੱਖਰ ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* value, int32_t length) {
    ਜੇ (builder == ਖਾਲੀ_ਸੂਚਕ) ਵਾਪਸ;
    ਅੰਦਰੂਨੀ(builder)->ਸੈੱਟ_Extension(ਕੁੰਜੀ, STRING_PIECE(value, length));
}

ਖਾਲੀ ulocbld_ਸੈੱਟ_Unicode_Locale_Keyword(
    ULocaleBuilder* builder, ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ, int32_t ਕੁੰਜੀ_ਲੰਮਾਈ,
    ਅਚਲ ਅੱਖਰ* type, int32_t type_ਲੰਮਾਈ) {
    ਜੇ (builder == ਖਾਲੀ_ਸੂਚਕ) ਵਾਪਸ;
    ਅੰਦਰੂਨੀ(builder)->ਸੈੱਟ_Unicode_Locale_Keyword(
        STRING_PIECE(ਕੁੰਜੀ, ਕੁੰਜੀ_ਲੰਮਾਈ), STRING_PIECE(type, type_ਲੰਮਾਈ));
}

ਖਾਲੀ ulocbld_clear(ULocaleBuilder* builder) {
    ਜੇ (builder == ਖਾਲੀ_ਸੂਚਕ) ਵਾਪਸ;
    ਅੰਦਰੂਨੀ(builder)->clear();
}

ਖਾਲੀ ulocbld_clearExtensions(ULocaleBuilder* builder) {
    ਜੇ (builder == ਖਾਲੀ_ਸੂਚਕ) ਵਾਪਸ;
    ਅੰਦਰੂਨੀ(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UError_ਕੋਡ* err) {
    ਜੇ (builder == ਖਾਲੀ_ਸੂਚਕ) {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    icu::Locale l = ਅੰਦਰੂਨੀ(builder)->build(*err);
    ਜੇ (U_FAILURE(*err)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    icu::Locale* r = l.clone();
    ਜੇ (r == ਖਾਲੀ_ਸੂਚਕ) {
        *err = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              ਅੱਖਰ* ਬਫਰ, int32_t ਬਫਰ_ਸਮਰੱਥਾ, UError_ਕੋਡ* err) {
    ਜੇ (U_FAILURE(*err)) { ਵਾਪਸ 0; }
    ਜੇ (builder == ਖਾਲੀ_ਸੂਚਕ) {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    icu::Locale l = ਅੰਦਰੂਨੀ(builder)->build(*err);
    ਜੇ (U_FAILURE(*err)) { ਵਾਪਸ 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    ਜੇ (0 < length && length <= ਬਫਰ_ਸਮਰੱਥਾ) {
        uprv_memcpy(ਬਫਰ, l.getName(), length);
    }
    ਵਾਪਸ u_terminateChars(ਬਫਰ, ਬਫਰ_ਸਮਰੱਥਾ, length, err);
}

int32_t ulocbld_build_Language_ਟੈਗ(ULocaleBuilder* builder,
                  ਅੱਖਰ* ਬਫਰ, int32_t ਬਫਰ_ਸਮਰੱਥਾ, UError_ਕੋਡ* err) {
    ਜੇ (U_FAILURE(*err)) { ਵਾਪਸ 0; }
    ਜੇ (builder == ਖਾਲੀ_ਸੂਚਕ) {
        *err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    icu::Locale l = ਅੰਦਰੂਨੀ(builder)->build(*err);
    ਵਾਪਸ icu::ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        ਬਫਰ, ਬਫਰ_ਸਮਰੱਥਾ,
        [&](icu::ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            l.ਮੰਜ਼ਿਲ_Language_ਟੈਗ(sink, ਸਥਿਤੀ);
        },
        *err);
}

UBool ulocbld_copy_Error_ਮੰਜ਼ਿਲ(ਅਚਲ ULocaleBuilder* builder, UError_ਕੋਡ *ਬਾਹਰਲਾ_Error_ਕੋਡ) {
    ਜੇ (builder == ਖਾਲੀ_ਸੂਚਕ) {
        *ਬਾਹਰਲਾ_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ CONST_ਅੰਦਰੂਨੀ(builder)->copy_Error_ਮੰਜ਼ਿਲ(*ਬਾਹਰਲਾ_Error_ਕੋਡ);
}
