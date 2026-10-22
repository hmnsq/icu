// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/localebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/ulocbuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

ಬಳಸಿ icu::StringPiece;

#ವ್ಯಾಖ್ಯಾನಿಸು EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#ವ್ಯಾಖ್ಯಾನಿಸು ಆಂತರಿಕ(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#ವ್ಯಾಖ್ಯಾನಿಸು CONST_ಆಂತರಿಕ(e) (reinterpret_cast<ಸ್ಥಿರಾಂಕ icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    ಹಿಂತಿರುಗಿ EXTERNAL(ಹೊಸ icu::LocaleBuilder());
}

ಶೂನ್ಯ ulocbld_close(ULocaleBuilder* builder) {
    ಆದರೆ (builder == ಶೂನ್ಯಸೂಚಕ) ಹಿಂತಿರುಗಿ;
    ಅಳಿಸು ಆಂತರಿಕ(builder);
}

ಶೂನ್ಯ ulocbld_ಗಣ_Locale(ULocaleBuilder* builder, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale, int32_t length) {
    ಆದರೆ (builder == ಶೂನ್ಯಸೂಚಕ) ಹಿಂತಿರುಗಿ;
    icu::Locale l;
    ಆದರೆ (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (length >= ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ) {
            l.ಗಣ_ಗಮ್ಯ_Bogus();
        } ಇಲ್ಲದಿದ್ದರೆ {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            ಅಕ್ಷರ buf[ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    ಆಂತರಿಕ(builder)->ಗಣ_Locale(l);
}

ಶೂನ್ಯ
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    ಆದರೆ (builder == ಶೂನ್ಯಸೂಚಕ) ಹಿಂತಿರುಗಿ;
    ಆಂತರಿಕ(builder)->ಗಣ_Locale(*(reinterpret_cast<ಸ್ಥಿರಾಂಕ icu::Locale*>(locale)));
    ulocale_close(locale);
}

#ವ್ಯಾಖ್ಯಾನಿಸು STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#ವ್ಯಾಖ್ಯಾನಿಸು IMPL_ULOCBLD_SETTER(N) \
ಶೂನ್ಯ ulocbld_##N(ULocaleBuilder* bld, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t l) { \
    ಆದರೆ (bld == ಶೂನ್ಯಸೂಚಕ) ಹಿಂತಿರುಗಿ; \
    ಆಂತರಿಕ(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(ಗಣ_Language_ಟ್ಯಾಗ್)
IMPL_ULOCBLD_SETTER(ಗಣ_Language)
IMPL_ULOCBLD_SETTER(ಗಣ_Script)
IMPL_ULOCBLD_SETTER(ಗಣ_Region)
IMPL_ULOCBLD_SETTER(ಗಣ_Variant)
IMPL_ULOCBLD_SETTER(ಕೂಡಿಸು_Unicode_Locale_Attribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

ಶೂನ್ಯ ulocbld_ಗಣ_Extension(ULocaleBuilder* builder, ಅಕ್ಷರ ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value, int32_t length) {
    ಆದರೆ (builder == ಶೂನ್ಯಸೂಚಕ) ಹಿಂತಿರುಗಿ;
    ಆಂತರಿಕ(builder)->ಗಣ_Extension(ಕೀಲಿ, STRING_PIECE(value, length));
}

ಶೂನ್ಯ ulocbld_ಗಣ_Unicode_Locale_Keyword(
    ULocaleBuilder* builder, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ, int32_t ಕೀಲಿ_ಉದ್ದಳತೆ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type, int32_t type_ಉದ್ದಳತೆ) {
    ಆದರೆ (builder == ಶೂನ್ಯಸೂಚಕ) ಹಿಂತಿರುಗಿ;
    ಆಂತರಿಕ(builder)->ಗಣ_Unicode_Locale_Keyword(
        STRING_PIECE(ಕೀಲಿ, ಕೀಲಿ_ಉದ್ದಳತೆ), STRING_PIECE(type, type_ಉದ್ದಳತೆ));
}

ಶೂನ್ಯ ulocbld_clear(ULocaleBuilder* builder) {
    ಆದರೆ (builder == ಶೂನ್ಯಸೂಚಕ) ಹಿಂತಿರುಗಿ;
    ಆಂತರಿಕ(builder)->clear();
}

ಶೂನ್ಯ ulocbld_clearExtensions(ULocaleBuilder* builder) {
    ಆದರೆ (builder == ಶೂನ್ಯಸೂಚಕ) ಹಿಂತಿರುಗಿ;
    ಆಂತರಿಕ(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UError_ಸಂಕೇತ* err) {
    ಆದರೆ (builder == ಶೂನ್ಯಸೂಚಕ) {
        *err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    icu::Locale l = ಆಂತರಿಕ(builder)->build(*err);
    ಆದರೆ (U_FAILURE(*err)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    icu::Locale* r = l.clone();
    ಆದರೆ (r == ಶೂನ್ಯಸೂಚಕ) {
        *err = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              ಅಕ್ಷರ* ಬಫರ್, int32_t ಬಫರ್_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ* err) {
    ಆದರೆ (U_FAILURE(*err)) { ಹಿಂತಿರುಗಿ 0; }
    ಆದರೆ (builder == ಶೂನ್ಯಸೂಚಕ) {
        *err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    icu::Locale l = ಆಂತರಿಕ(builder)->build(*err);
    ಆದರೆ (U_FAILURE(*err)) { ಹಿಂತಿರುಗಿ 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    ಆದರೆ (0 < length && length <= ಬಫರ್_ಸಾಮರ್ಥ್ಯ) {
        uprv_memcpy(ಬಫರ್, l.getName(), length);
    }
    ಹಿಂತಿರುಗಿ u_terminateChars(ಬಫರ್, ಬಫರ್_ಸಾಮರ್ಥ್ಯ, length, err);
}

int32_t ulocbld_build_Language_ಟ್ಯಾಗ್(ULocaleBuilder* builder,
                  ಅಕ್ಷರ* ಬಫರ್, int32_t ಬಫರ್_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ* err) {
    ಆದರೆ (U_FAILURE(*err)) { ಹಿಂತಿರುಗಿ 0; }
    ಆದರೆ (builder == ಶೂನ್ಯಸೂಚಕ) {
        *err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    icu::Locale l = ಆಂತರಿಕ(builder)->build(*err);
    ಹಿಂತಿರುಗಿ icu::ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        ಬಫರ್, ಬಫರ್_ಸಾಮರ್ಥ್ಯ,
        [&](icu::ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            l.ಗಮ್ಯ_Language_ಟ್ಯಾಗ್(sink, ಸ್ಥಿತಿ_2);
        },
        *err);
}

UBool ulocbld_copy_Error_ಗಮ್ಯ(ಸ್ಥಿರಾಂಕ ULocaleBuilder* builder, UError_ಸಂಕೇತ *ಹೊರಹೋಗು_Error_ಸಂಕೇತ) {
    ಆದರೆ (builder == ಶೂನ್ಯಸೂಚಕ) {
        *ಹೊರಹೋಗು_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ CONST_ಆಂತರಿಕ(builder)->copy_Error_ಗಮ್ಯ(*ಹೊರಹೋಗು_Error_ಸಂಕೇತ);
}
