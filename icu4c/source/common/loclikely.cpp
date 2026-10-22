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

#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>
#ਅਨ੍ਤਰ੍ਭਾਵ <utility>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "loclikelysubtags.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulocimp.h"

ਨਾਮਾਕਾਸ਼ {

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
ਖਾਲੀ U_CALLCONV
create_ਟੈਗ_String_With_Alternates(
    ਅਚਲ ਅੱਖਰ* lang,
    int32_t lang_ਲੰਮਾਈ,
    ਅਚਲ ਅੱਖਰ* script,
    int32_t script_ਲੰਮਾਈ,
    ਅਚਲ ਅੱਖਰ* region,
    int32_t region_ਲੰਮਾਈ,
    ਅਚਲ ਅੱਖਰ* variant,
    int32_t variant_ਲੰਮਾਈ,
    ਅਚਲ ਅੱਖਰ* trailing,
    int32_t trailing_ਲੰਮਾਈ,
    icu::ByteSink& sink,
    UError_ਕੋਡ& err) {
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }

    ਜੇ (lang_ਲੰਮਾਈ >= ULOC_LANG_ਸਮਰੱਥਾ ||
            script_ਲੰਮਾਈ >= ULOC_SCRIPT_ਸਮਰੱਥਾ ||
            region_ਲੰਮਾਈ >= ULOC_COUNTRY_ਸਮਰੱਥਾ) {
        err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ਜੇ (lang_ਲੰਮਾਈ > 0) {
        sink.Append(lang, lang_ਲੰਮਾਈ);
    }

    ਜੇ (script_ਲੰਮਾਈ > 0) {
        sink.Append("_", 1);
        sink.Append(script, script_ਲੰਮਾਈ);
    }

    ਜੇ (region_ਲੰਮਾਈ > 0) {
        sink.Append("_", 1);
        sink.Append(region, region_ਲੰਮਾਈ);
    }

    ਜੇ (variant_ਲੰਮਾਈ > 0) {
        ਜੇ (region_ਲੰਮਾਈ == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variant_ਲੰਮਾਈ);
    }

    ਜੇ (trailing_ਲੰਮਾਈ > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailing_ਲੰਮਾਈ);
    }
}

ਤਾਰਕਿਕ CHECK_TRAILING_VARIANT_ਮਾਪ(ਅਚਲ ਅੱਖਰ* variant, int32_t variant_ਲੰਮਾਈ) {
    int32_t count = 0;
    ਲਈ (int32_t i = 0; i < variant_ਲੰਮਾਈ; i++) {
        ਜੇ (_isIDSeparator(variant[i])) {
            count = 0;
        } ਨਹੀਂਤਾਂ ਜੇ (count == 8) {
            ਵਾਪਸ ਝੂਠ;
        } ਨਹੀਂਤਾਂ {
            count++;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ
uloc_ਜੋੜੋ_Likely_Subtags(ਅਚਲ ਅੱਖਰ* localeID,
                       icu::ByteSink& sink,
                       UError_ਕੋਡ& err) {
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }

    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ਅਚਲ ਅੱਖਰ* trailing = ਖਾਲੀ_ਸੂਚਕ;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }

    ਜੇ (!CHECK_TRAILING_VARIANT_ਮਾਪ(variant.ਅੰਕੜੇ(), variant.length())) {
        err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ਜੇ (lang.length() == 4) {
        ਜੇ (script.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            script = std::move(lang);
            lang.clear();
        } ਨਹੀਂਤਾਂ {
            err = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }
    } ਨਹੀਂਤਾਂ ਜੇ (lang.length() > 8) {
        err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    int32_t trailing_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(trailing));

    ਅਚਲ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    ਜੇ (l.isBogus()) {
        err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, ਸੱਚ, err);
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }
    ਅਚਲ ਅੱਖਰ* language = lsr.language;
    ਜੇ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    create_ਟੈਗ_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.ਅੰਕੜੇ(),
        variant.length(),
        trailing,
        trailing_ਲੰਮਾਈ,
        sink,
        err);
}

ਖਾਲੀ
_uloc_minimizeSubtags(ਅਚਲ ਅੱਖਰ* localeID,
                      icu::ByteSink& sink,
                      ਤਾਰਕਿਕ favorScript,
                      UError_ਕੋਡ& err) {
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }

    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ਅਚਲ ਅੱਖਰ* trailing = ਖਾਲੀ_ਸੂਚਕ;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }

    ਜੇ (!CHECK_TRAILING_VARIANT_ਮਾਪ(variant.ਅੰਕੜੇ(), variant.length())) {
        err = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    int32_t trailing_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(trailing));

    ਅਚਲ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.ਮੰਜ਼ਿਲ_String_Piece(),
        script.ਮੰਜ਼ਿਲ_String_Piece(),
        region.ਮੰਜ਼ਿਲ_String_Piece(),
        favorScript,
        err);
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }
    ਅਚਲ ਅੱਖਰ* language = lsr.language;
    ਜੇ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    create_ਟੈਗ_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.ਅੰਕੜੇ(),
        variant.length(),
        trailing,
        trailing_ਲੰਮਾਈ,
        sink,
        err);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_ਜੋੜੋ_Likely_Subtags_2(ਅਚਲ ਅੱਖਰ* localeID,
                      ਅੱਖਰ* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਵਾਪਸ icu::ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_ਜੋੜੋ_Likely_Subtags(localeID, sink, ਸਥਿਤੀ);
        },
        *ਸਥਿਤੀ);
}

U_EXPORT icu::CharString
ulocimp_ਜੋੜੋ_Likely_Subtags(ਅਚਲ ਅੱਖਰ* localeID,
                         UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਵਾਪਸ icu::ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](icu::ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_ਜੋੜੋ_Likely_Subtags(localeID, sink, ਸਥਿਤੀ);
        },
        ਸਥਿਤੀ);
}

U_EXPORT ਖਾਲੀ
ulocimp_ਜੋੜੋ_Likely_Subtags(ਅਚਲ ਅੱਖਰ* localeID,
                         icu::ByteSink& sink,
                         UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_ਬਫਰ = ulocimp_canonicalize(localeID, ਸਥਿਤੀ);
    uloc_ਜੋੜੋ_Likely_Subtags(locale_ਬਫਰ.ਅੰਕੜੇ(), sink, ਸਥਿਤੀ);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(ਅਚਲ ਅੱਖਰ* localeID,
                     ਅੱਖਰ* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਵਾਪਸ icu::ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_minimizeSubtags(localeID, sink, ਝੂਠ, ਸਥਿਤੀ);
        },
        *ਸਥਿਤੀ);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(ਅਚਲ ਅੱਖਰ* localeID,
                        ਤਾਰਕਿਕ favorScript,
                        UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਵਾਪਸ icu::ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Char_String(
        [&](icu::ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, ਸਥਿਤੀ);
        },
        ਸਥਿਤੀ);
}

U_EXPORT ਖਾਲੀ
ulocimp_minimizeSubtags(ਅਚਲ ਅੱਖਰ* localeID,
                        icu::ByteSink& sink,
                        ਤਾਰਕਿਕ favorScript,
                        UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_ਬਫਰ = ulocimp_canonicalize(localeID, ਸਥਿਤੀ);
    _uloc_minimizeSubtags(locale_ਬਫਰ.ਅੰਕੜੇ(), sink, favorScript, ਸਥਿਤੀ);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
ਸਥਿਰ ਅਚਲ ਅੱਖਰ LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ(ਅਚਲ ਅੱਖਰ *locale) {
    UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == ਖਾਲੀ_ਸੂਚਕ ? uloc_getDefault() : locale,
        &lang, &script, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ) || script.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        ਜੇ (!lang.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            ਅਚਲ ਅੱਖਰ* langPtr = uprv_strstr(LANG_DIR_STRING, lang.ਅੰਕੜੇ());
            ਜੇ (langPtr != ਖਾਲੀ_ਸੂਚਕ) {
                ਚੋਣ (langPtr[lang.length()]) {
                ਹਾਲਤ '-': ਵਾਪਸ ਝੂਠ;
                ਹਾਲਤ '+': ਵਾਪਸ ਸੱਚ;
                ਮੂਲ: ਰੋਕੋ;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        error_ਕੋਡ = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_ਜੋੜੋ_Likely_Subtags(locale, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ ਝੂਠ;
        }
        ulocimp_getSubtags(likely.ਮੰਜ਼ਿਲ_String_Piece(), ਖਾਲੀ_ਸੂਚਕ, &script, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ) || script.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    UScript_ਕੋਡ script_ਕੋਡ = (UScript_ਕੋਡ)u_get_Property_ਮੁੱਲ_Enum(UCHAR_SCRIPT, script.ਅੰਕੜੇ());
    ਵਾਪਸ uscript_is_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ(script_ਕੋਡ);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ() ਅਚਲ {
    ਵਾਪਸ uloc_is_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ(get_ਅਧਾਰ_Name());
}

U_NAMESPACE_ਸਿਰਾ

ਨਾਮਾਕਾਸ਼ {
icu::CharString
Get_Region_From_ਕੁੰਜੀ(ਅਚਲ ਅੱਖਰ* localeID, std::string_view ਕੁੰਜੀ, UError_ਕੋਡ& ਸਥਿਤੀ) {
    icu::CharString ਨਤੀਜਾ;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_ਮੁੱਲ(localeID, ਕੁੰਜੀ, ਸਥਿਤੀ);
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
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        ਅਚਲ ਅੱਖਰ region[] = {kw[0], kw[1], '\0'};
        ਜੇ (valid.is_ਸੈੱਟ(region)) {
            ਨਤੀਜਾ.append(uprv_toupper(kw[0]), ਸਥਿਤੀ);
            ਨਤੀਜਾ.append(uprv_toupper(kw[1]), ਸਥਿਤੀ);
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_get_Region_For_Supplemental_ਅੰਕੜੇ(ਅਚਲ ਅੱਖਰ *localeID, ਤਾਰਕਿਕ inferRegion,
                                     UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ {};
    }
    icu::CharString rgBuf = Get_Region_From_ਕੁੰਜੀ(localeID, "rg", ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && rgBuf.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == ਖਾਲੀ_ਸੂਚਕ ? uloc_getDefault() : localeID, ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && rgBuf.is_ਖਾਲੀ_ਥਾਵਾਂ() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_ਕੁੰਜੀ(localeID, "sd", ਸਥਿਤੀ);
            ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && rgBuf.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UError_ਕੋਡ rg_ਸਥਿਤੀ = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_ਜੋੜੋ_Likely_Subtags(localeID, rg_ਸਥਿਤੀ);
                ਜੇ (U_SUCCESS(rg_ਸਥਿਤੀ)) {
                    rgBuf = ulocimp_getRegion(locBuf.ਮੰਜ਼ਿਲ_String_Piece(), ਸਥਿਤੀ);
                }
            }
        }
    }

    ਵਾਪਸ rgBuf;
}

ਨਾਮਾਕਾਸ਼ {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
ਅਚਲ uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, ਆਕਾਰ(map));
}

RegionValidateMap::~RegionValidateMap() {
}

ਤਾਰਕਿਕ RegionValidateMap::is_ਸੈੱਟ(ਅਚਲ ਅੱਖਰ* region) ਅਚਲ {
    int32_t ਸੂਚਕਾਂਕ = value(region);
    ਜੇ (ਸੂਚਕਾਂਕ < 0) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ 0 != (map[ਸੂਚਕਾਂਕ / 32] & (1L << (ਸੂਚਕਾਂਕ % 32)));
}

ਤਾਰਕਿਕ RegionValidateMap::equals(ਅਚਲ RegionValidateMap& that) ਅਚਲ {
    ਵਾਪਸ uprv_memcmp(map, that.map, ਆਕਾਰ(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(ਅਚਲ ਅੱਖਰ* region) ਅਚਲ {
    ਜੇ (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        ਵਾਪਸ (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    ਵਾਪਸ -1;
}

U_NAMESPACE_ਸਿਰਾ
