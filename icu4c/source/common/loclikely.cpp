// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1997-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  loclikely.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010feb25
*   created by: Markus W. Scherer
*
*   Code for likely and minimized locale subtags, separated out from other .cpp files
*   that then do not depend on resource bundle code and likely-subtags data.
*/

#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "loclikelysubtags.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"

ನಾಮಾಕಾಶ {

/**
 * Create a tag string from the supplied parameters.  The lang, script and region
 * parameters may be nullptr pointers. If they are, their corresponding length parameters
 * must be less than or equal to 0.
 *
 * If an illegal argument is provided, the function returns the error
 * U_ILLEGAL_ARGUMENT_ERROR.
 *
 * @param lang The language tag to use.
 * @param langLength The length of the language tag.
 * @param script The script tag to use.
 * @param scriptLength The length of the script tag.
 * @param region The region tag to use.
 * @param regionLength The length of the region tag.
 * @param variant The region tag to use.
 * @param variantLength The length of the region tag.
 * @param trailing Any trailing data to append to the new tag.
 * @param trailingLength The length of the trailing data.
 * @param sink The output sink receiving the tag string.
 * @param err A pointer to a UErrorCode for error reporting.
 **/
ಶೂನ್ಯ U_CALLCONV
create_ಟ್ಯಾಗ್_String_With_Alternates(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* lang,
    int32_t lang_ಉದ್ದಳತೆ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* script,
    int32_t script_ಉದ್ದಳತೆ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region,
    int32_t region_ಉದ್ದಳತೆ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant,
    int32_t variant_ಉದ್ದಳತೆ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* trailing,
    int32_t trailing_ಉದ್ದಳತೆ,
    icu::ByteSink& sink,
    UError_ಸಂಕೇತ& err) {
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (lang_ಉದ್ದಳತೆ >= ULOC_LANG_ಸಾಮರ್ಥ್ಯ ||
            script_ಉದ್ದಳತೆ >= ULOC_SCRIPT_ಸಾಮರ್ಥ್ಯ ||
            region_ಉದ್ದಳತೆ >= ULOC_COUNTRY_ಸಾಮರ್ಥ್ಯ) {
        err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (lang_ಉದ್ದಳತೆ > 0) {
        sink.Append(lang, lang_ಉದ್ದಳತೆ);
    }

    ಆದರೆ (script_ಉದ್ದಳತೆ > 0) {
        sink.Append("_", 1);
        sink.Append(script, script_ಉದ್ದಳತೆ);
    }

    ಆದರೆ (region_ಉದ್ದಳತೆ > 0) {
        sink.Append("_", 1);
        sink.Append(region, region_ಉದ್ದಳತೆ);
    }

    ಆದರೆ (variant_ಉದ್ದಳತೆ > 0) {
        ಆದರೆ (region_ಉದ್ದಳತೆ == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variant_ಉದ್ದಳತೆ);
    }

    ಆದರೆ (trailing_ಉದ್ದಳತೆ > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailing_ಉದ್ದಳತೆ);
    }
}

ತಾರ್ಕಿಕ CHECK_TRAILING_VARIANT_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant, int32_t variant_ಉದ್ದಳತೆ) {
    int32_t count = 0;
    ಗಾಗಿ (int32_t i = 0; i < variant_ಉದ್ದಳತೆ; i++) {
        ಆದರೆ (_isIDSeparator(variant[i])) {
            count = 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (count == 8) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            count++;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ
uloc_ಕೂಡಿಸು_Likely_Subtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                       icu::ByteSink& sink,
                       UError_ಸಂಕೇತ& err) {
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* trailing = ಶೂನ್ಯಸೂಚಕ;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!CHECK_TRAILING_VARIANT_ಗಾತ್ರ(variant.ದತ್ತಾಂಶ(), variant.length())) {
        err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (lang.length() == 4) {
        ಆದರೆ (script.is_ಖಾಲಿಗಳು()) {
            script = std::move(lang);
            lang.clear();
        } ಇಲ್ಲದಿದ್ದರೆ {
            err = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lang.length() > 8) {
        err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    int32_t trailing_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(trailing));

    ಸ್ಥಿರಾಂಕ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    ಆದರೆ (l.isBogus()) {
        err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, ನಿಜ, err);
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* language = lsr.language;
    ಆದರೆ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    create_ಟ್ಯಾಗ್_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.ದತ್ತಾಂಶ(),
        variant.length(),
        trailing,
        trailing_ಉದ್ದಳತೆ,
        sink,
        err);
}

ಶೂನ್ಯ
_uloc_minimizeSubtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                      icu::ByteSink& sink,
                      ತಾರ್ಕಿಕ favorScript,
                      UError_ಸಂಕೇತ& err) {
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* trailing = ಶೂನ್ಯಸೂಚಕ;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!CHECK_TRAILING_VARIANT_ಗಾತ್ರ(variant.ದತ್ತಾಂಶ(), variant.length())) {
        err = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    int32_t trailing_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(trailing));

    ಸ್ಥಿರಾಂಕ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.ಗಮ್ಯ_String_Piece(),
        script.ಗಮ್ಯ_String_Piece(),
        region.ಗಮ್ಯ_String_Piece(),
        favorScript,
        err);
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* language = lsr.language;
    ಆದರೆ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    create_ಟ್ಯಾಗ್_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.ದತ್ತಾಂಶ(),
        variant.length(),
        trailing,
        trailing_ಉದ್ದಳತೆ,
        sink,
        err);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_ಕೂಡಿಸು_Likely_Subtags_2(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                      ಅಕ್ಷರ* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ icu::ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_ಕೂಡಿಸು_Likely_Subtags(localeID, sink, ಸ್ಥಿತಿ_2);
        },
        *ಸ್ಥಿತಿ_2);
}

U_EXPORT icu::CharString
ulocimp_ಕೂಡಿಸು_Likely_Subtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                         UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ icu::ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](icu::ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_ಕೂಡಿಸು_Likely_Subtags(localeID, sink, ಸ್ಥಿತಿ_2);
        },
        ಸ್ಥಿತಿ_2);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_ಕೂಡಿಸು_Likely_Subtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                         icu::ByteSink& sink,
                         UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_ಬಫರ್ = ulocimp_canonicalize(localeID, ಸ್ಥಿತಿ_2);
    uloc_ಕೂಡಿಸು_Likely_Subtags(locale_ಬಫರ್.ದತ್ತಾಂಶ(), sink, ಸ್ಥಿತಿ_2);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                     ಅಕ್ಷರ* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ icu::ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_minimizeSubtags(localeID, sink, ಸುಳ್ಳು, ಸ್ಥಿತಿ_2);
        },
        *ಸ್ಥಿತಿ_2);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                        ತಾರ್ಕಿಕ favorScript,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ icu::ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Char_String(
        [&](icu::ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, ಸ್ಥಿತಿ_2);
        },
        ಸ್ಥಿತಿ_2);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_minimizeSubtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                        icu::ByteSink& sink,
                        ತಾರ್ಕಿಕ favorScript,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_ಬಫರ್ = ulocimp_canonicalize(localeID, ಸ್ಥಿತಿ_2);
    _uloc_minimizeSubtags(locale_ಬಫರ್.ದತ್ತಾಂಶ(), sink, favorScript, ಸ್ಥಿತಿ_2);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_ಬಲ_ಗಮ್ಯ_ಎಡ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == ಶೂನ್ಯಸೂಚಕ ? uloc_getDefault() : locale,
        &lang, &script, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || script.is_ಖಾಲಿಗಳು()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        ಆದರೆ (!lang.is_ಖಾಲಿಗಳು()) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* langPtr = uprv_strstr(LANG_DIR_STRING, lang.ದತ್ತಾಂಶ());
            ಆದರೆ (langPtr != ಶೂನ್ಯಸೂಚಕ) {
                ಆಯ್ಕೆ (langPtr[lang.length()]) {
                ಸ್ಥಿತಿ '-': ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                ಸ್ಥಿತಿ '+': ಹಿಂತಿರುಗಿ ನಿಜ;
                ಪೂರ್ವನಿಯೋಜಿತ: ನಿಲ್ಲಿಸು;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        error_ಸಂಕೇತ = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_ಕೂಡಿಸು_Likely_Subtags(locale, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        ulocimp_getSubtags(likely.ಗಮ್ಯ_String_Piece(), ಶೂನ್ಯಸೂಚಕ, &script, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || script.is_ಖಾಲಿಗಳು()) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    UScript_ಸಂಕೇತ script_ಸಂಕೇತ = (UScript_ಸಂಕೇತ)u_get_Property_ಮೌಲ್ಯ_Enum(UCHAR_SCRIPT, script.ದತ್ತಾಂಶ());
    ಹಿಂತಿರುಗಿ uscript_is_ಬಲ_ಗಮ್ಯ_ಎಡ(script_ಸಂಕೇತ);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_ಬಲ_ಗಮ್ಯ_ಎಡ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ uloc_is_ಬಲ_ಗಮ್ಯ_ಎಡ(get_ತಳ_Name());
}

U_NAMESPACE_ಕೊನೆ

ನಾಮಾಕಾಶ {
icu::CharString
Get_Region_From_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID, std::string_view ಕೀಲಿ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    icu::CharString ಫಲಿತಾಂಶ;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_ಮೌಲ್ಯ(localeID, ಕೀಲಿ, ಸ್ಥಿತಿ_2);
    int32_t len = kw.length();
    // In UTS35
    //   type = alphanum{3,8} (sep alphanum{3,8})* ;
    // so we know the subdivision must fit the type already.
    //
    //   unicode_subdivision_id = unicode_region_subtag unicode_subdivision_suffix ;
    //   unicode_region_subtag = (alpha{2} | digit{3}) ;
    //   unicode_subdivision_suffix = alphanum{1,4} ;
    // But we also know there are no id in start with digit{3} in
    // https://github.com/unicode-org/cldr/blob/main/common/validity/subdivision.xml
    // Therefore we can simplify as
    // unicode_subdivision_id = alpha{2} alphanum{1,4}
    //
    // and only need to accept/reject the code based on the alpha{2} and the length.
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ region[] = {kw[0], kw[1], '\0'};
        ಆದರೆ (valid.is_ಗಣ(region)) {
            ಫಲಿತಾಂಶ.append(uprv_toupper(kw[0]), ಸ್ಥಿತಿ_2);
            ಫಲಿತಾಂಶ.append(uprv_toupper(kw[1]), ಸ್ಥಿತಿ_2);
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_get_Region_For_Supplemental_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *localeID, ತಾರ್ಕಿಕ inferRegion,
                                     UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ {};
    }
    icu::CharString rgBuf = Get_Region_From_ಕೀಲಿ(localeID, "rg", ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && rgBuf.is_ಖಾಲಿಗಳು()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == ಶೂನ್ಯಸೂಚಕ ? uloc_getDefault() : localeID, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && rgBuf.is_ಖಾಲಿಗಳು() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_ಕೀಲಿ(localeID, "sd", ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && rgBuf.is_ಖಾಲಿಗಳು()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UError_ಸಂಕೇತ rg_ಸ್ಥಿತಿ = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_ಕೂಡಿಸು_Likely_Subtags(localeID, rg_ಸ್ಥಿತಿ);
                ಆದರೆ (U_SUCCESS(rg_ಸ್ಥಿತಿ)) {
                    rgBuf = ulocimp_getRegion(locBuf.ಗಮ್ಯ_String_Piece(), ಸ್ಥಿತಿ_2);
                }
            }
        }
    }

    ಹಿಂತಿರುಗಿ rgBuf;
}

ನಾಮಾಕಾಶ {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
ಸ್ಥಿರಾಂಕ uint32_t gValidRegionMap[] = {
    0xeedf597c, 0xdeddbdef, 0x15943f3f, 0x0e00d580, 
    0xb0095c00, 0x0015fb9f, 0x781c068d, 0x0340400f, 
    0xf42b1d00, 0xfd4f8141, 0x25d7fffc, 0x0100084b, 
    0x538f3c40, 0x40000001, 0xfdf15100, 0x9fbb7ae7, 
    0x0410419a, 0x00408557, 0x00004002, 0x00100001, 
    0x00400408, 0x00000001, 
};

}  // namespace
   //
U_NAMESPACE_BEGIN
RegionValidateMap::RegionValidateMap() {
    uprv_memcpy(map, gValidRegionMap, ಗಾತ್ರ(map));
}

RegionValidateMap::~RegionValidateMap() {
}

ತಾರ್ಕಿಕ RegionValidateMap::is_ಗಣ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region) ಸ್ಥಿರಾಂಕ {
    int32_t ಸೂಚ್ಯಂಕ = value(region);
    ಆದರೆ (ಸೂಚ್ಯಂಕ < 0) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ 0 != (map[ಸೂಚ್ಯಂಕ / 32] & (1L << (ಸೂಚ್ಯಂಕ % 32)));
}

ತಾರ್ಕಿಕ RegionValidateMap::equals(ಸ್ಥಿರಾಂಕ RegionValidateMap& that) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ uprv_memcmp(map, that.map, ಗಾತ್ರ(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        ಹಿಂತಿರುಗಿ (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    ಹಿಂತಿರುಗಿ -1;
}

U_NAMESPACE_ಕೊನೆ
