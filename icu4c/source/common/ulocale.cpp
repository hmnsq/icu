// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/errorcode.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ulocale.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"

#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_USE
#ವ್ಯಾಖ್ಯಾನಿಸು EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#ವ್ಯಾಖ್ಯಾನಿಸು CONST_ಆಂತರಿಕ(e) (reinterpret_cast<ಸ್ಥಿರಾಂಕ icu::Locale*>(e))
#ವ್ಯಾಖ್ಯಾನಿಸು ಆಂತರಿಕ(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID, int32_t length, UError_ಸಂಕೇತ* err) {
    ಆದರೆ (U_FAILURE(*err)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    ಆದರೆ (length < 0) {
        ಹಿಂತಿರುಗಿ EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    ಹಿಂತಿರುಗಿ EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_open_For_Language_ಟ್ಯಾಗ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಟ್ಯಾಗ್, int32_t length, UError_ಸಂಕೇತ* err) {
  ಆದರೆ (U_FAILURE(*err)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
  Locale l = icu::Locale::for_Language_ಟ್ಯಾಗ್(length < 0 ? StringPiece(ಟ್ಯಾಗ್) : StringPiece(ಟ್ಯಾಗ್, length), *err);
  ಆದರೆ (U_FAILURE(*err)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
  ಹಿಂತಿರುಗಿ EXTERNAL(l.clone());
}

ಶೂನ್ಯ
ulocale_close(ULocale* locale) {
    ಅಳಿಸು ಆಂತರಿಕ(locale);
}

#ವ್ಯಾಖ್ಯಾನಿಸು IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ulocale_get ## N1(ಸ್ಥಿರಾಂಕ ULocale* locale U_LIFETIME_BOUND) { \
    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; \
    ಹಿಂತಿರುಗಿ CONST_ಆಂತರಿಕ(locale)->get ## N2(); \
}

#ವ್ಯಾಖ್ಯಾನಿಸು IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#ವ್ಯಾಖ್ಯಾನಿಸು IMPL_ULOCALE_GET_KEYWORD_ಮೌಲ್ಯ(N) \
int32_t ulocale_get ##N ( \
    ಸ್ಥಿರಾಂಕ ULocale* locale, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* keyword, int32_t keyword_ಉದ್ದಳತೆ, \
    ಅಕ್ಷರ* ಮೌಲ್ಯ_ಬಫರ್, int32_t ಬಫರ್_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ *err) { \
    ಆದರೆ (U_FAILURE(*err)) ಹಿಂತಿರುಗಿ 0; \
    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) { \
        *err = U_ILLEGAL_ವಾದ_ERROR; \
        ಹಿಂತಿರುಗಿ 0; \
    } \
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars( \
        ಮೌಲ್ಯ_ಬಫರ್, ಬಫರ್_ಸಾಮರ್ಥ್ಯ, \
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) { \
            CONST_ಆಂತರಿಕ(locale)->get ## N( \
                keyword_ಉದ್ದಳತೆ < 0 ? StringPiece(keyword) : StringPiece(keyword, keyword_ಉದ್ದಳತೆ), \
                sink, ಸ್ಥಿತಿ_2); \
        }, \
        *err); \
}

#ವ್ಯಾಖ್ಯಾನಿಸು IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ಸ್ಥಿರಾಂಕ ULocale* locale, UError_ಸಂಕೇತ *err) { \
    ಆದರೆ (U_FAILURE(*err)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; \
    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) { \
        *err = U_ILLEGAL_ವಾದ_ERROR; \
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; \
    } \
    ಹಿಂತಿರುಗಿ uenum_openFromStringEnumeration( \
        CONST_ಆಂತರಿಕ(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(ತಳ_Name_2)
IMPL_ULOCALE_GET_KEYWORD_ಮೌಲ್ಯ(Keyword_ಮೌಲ್ಯ)
IMPL_ULOCALE_GET_KEYWORD_ಮೌಲ್ಯ(Unicode_Keyword_ಮೌಲ್ಯ)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

ತಾರ್ಕಿಕ ulocale_isBogus(ಸ್ಥಿರಾಂಕ ULocale* locale) {
    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    ಹಿಂತಿರುಗಿ CONST_ಆಂತರಿಕ(locale)->isBogus();
}

/*eof*/
