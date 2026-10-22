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
createTagStringWithAlternates(
    ಧ್ರುವ ವರ್ಣ* lang,
    int32_t langLength,
    ಧ್ರುವ ವರ್ಣ* script,
    int32_t scriptLength,
    ಧ್ರುವ ವರ್ಣ* region,
    int32_t regionLength,
    ಧ್ರುವ ವರ್ಣ* variant,
    int32_t variantLength,
    ಧ್ರುವ ವರ್ಣ* trailing,
    int32_t trailingLength,
    icu::ByteSink& sink,
    UErrorCode& err) {
    ಯದಿ (U_FAILURE(err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (langLength >= ULOC_LANG_CAPACITY ||
            scriptLength >= ULOC_SCRIPT_CAPACITY ||
            regionLength >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (langLength > 0) {
        sink.Append(lang, langLength);
    }

    ಯದಿ (scriptLength > 0) {
        sink.Append("_", 1);
        sink.Append(script, scriptLength);
    }

    ಯದಿ (regionLength > 0) {
        sink.Append("_", 1);
        sink.Append(region, regionLength);
    }

    ಯದಿ (variantLength > 0) {
        ಯದಿ (regionLength == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variantLength);
    }

    ಯದಿ (trailingLength > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailingLength);
    }
}

ತಾರ್ಕಿಕ CHECK_TRAILING_VARIANT_SIZE(ಧ್ರುವ ವರ್ಣ* variant, int32_t variantLength) {
    int32_t count = 0;
    ಪ್ರತಿ (int32_t i = 0; i < variantLength; i++) {
        ಯದಿ (_isIDSeparator(variant[i])) {
            count = 0;
        } ಅನ್ಯಥಾ ಯದಿ (count == 8) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        } ಅನ್ಯಥಾ {
            count++;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ
_uloc_addLikelySubtags(ಧ್ರುವ ವರ್ಣ* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    ಯದಿ (U_FAILURE(err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ಧ್ರುವ ವರ್ಣ* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ಯದಿ (U_FAILURE(err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (lang.length() == 4) {
        ಯದಿ (script.isEmpty()) {
            script = std::move(lang);
            lang.clear();
        } ಅನ್ಯಥಾ {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
    } ಅನ್ಯಥಾ ಯದಿ (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ಧ್ರುವ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ಯದಿ (U_FAILURE(err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    ಯದಿ (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, ಸತ್ಯ, err);
    ಯದಿ (U_FAILURE(err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಧ್ರುವ ವರ್ಣ* language = lsr.language;
    ಯದಿ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    createTagStringWithAlternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.data(),
        variant.length(),
        trailing,
        trailingLength,
        sink,
        err);
}

ಶೂನ್ಯ
_uloc_minimizeSubtags(ಧ್ರುವ ವರ್ಣ* localeID,
                      icu::ByteSink& sink,
                      ತಾರ್ಕಿಕ favorScript,
                      UErrorCode& err) {
    ಯದಿ (U_FAILURE(err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ಧ್ರುವ ವರ್ಣ* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ಯದಿ (U_FAILURE(err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ಧ್ರುವ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ಯದಿ (U_FAILURE(err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    ಯದಿ (U_FAILURE(err)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಧ್ರುವ ವರ್ಣ* language = lsr.language;
    ಯದಿ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    createTagStringWithAlternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.data(),
        variant.length(),
        trailing,
        trailingLength,
        sink,
        err);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_addLikelySubtags(ಧ್ರುವ ವರ್ಣ* localeID,
                      ವರ್ಣ* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    ಪ್ರತ್ಯಾಗಮ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(ಧ್ರುವ ವರ್ಣ* localeID,
                         UErrorCode& status) {
    ಪ್ರತ್ಯಾಗಮ icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_addLikelySubtags(ಧ್ರುವ ವರ್ಣ* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
    ಯದಿ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(localeBuffer.data(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(ಧ್ರುವ ವರ್ಣ* localeID,
                     ವರ್ಣ* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    ಪ್ರತ್ಯಾಗಮ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, ಅಸತ್ಯ, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(ಧ್ರುವ ವರ್ಣ* localeID,
                        ತಾರ್ಕಿಕ favorScript,
                        UErrorCode& status) {
    ಪ್ರತ್ಯಾಗಮ icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT ಶೂನ್ಯ
ulocimp_minimizeSubtags(ಧ್ರುವ ವರ್ಣ* localeID,
                        icu::ByteSink& sink,
                        ತಾರ್ಕಿಕ favorScript,
                        UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
    ಯದಿ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(localeBuffer.data(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_ಬಲ_To_ಎಡ(ಧ್ರುವ ವರ್ಣ *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    ಯದಿ (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        ಯದಿ (!lang.isEmpty()) {
            ಧ್ರುವ ವರ್ಣ* langPtr = uprv_strstr(LANG_DIR_STRING, lang.data());
            ಯದಿ (langPtr != nullptr) {
                ವಿಕಲ್ಪ (langPtr[lang.length()]) {
                ಪಕ್ಷ '-': ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                ಪಕ್ಷ '+': ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
                ಸ್ವಭಾವ: ಭಙ್ಗ;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        ಯದಿ (U_FAILURE(errorCode) || script.isEmpty()) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_get_Property_ಮೌಲ್ಯ_Enum(UCHAR_SCRIPT, script.data());
    ಪ್ರತ್ಯಾಗಮ uscript_is_ಬಲ_To_ಎಡ(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_ಬಲ_To_ಎಡ() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ uloc_is_ಬಲ_To_ಎಡ(getBaseName());
}

U_NAMESPACE_END

ನಾಮಾಕಾಶ {
icu::CharString
Get_Region_From_ಕೀಲಿ(ಧ್ರುವ ವರ್ಣ* localeID, std::string_view ಕೀಲಿ, UErrorCode& status) {
    icu::CharString result;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_ಮೌಲ್ಯ(localeID, ಕೀಲಿ, status);
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
    ಯದಿ (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        ಧ್ರುವ ವರ್ಣ region[] = {kw[0], kw[1], '\0'};
        ಯದಿ (valid.isSet(region)) {
            result.append(uprv_toupper(kw[0]), status);
            result.append(uprv_toupper(kw[1]), status);
        }
    }
    ಪ್ರತ್ಯಾಗಮ result;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_getRegionForSupplementalData(ಧ್ರುವ ವರ್ಣ *localeID, ತಾರ್ಕಿಕ inferRegion,
                                     UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ {};
    }
    icu::CharString rgBuf = Get_Region_From_ಕೀಲಿ(localeID, "rg", status);
    ಯದಿ (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        ಯದಿ (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_ಕೀಲಿ(localeID, "sd", status);
            ಯದಿ (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                ಯದಿ (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    ಪ್ರತ್ಯಾಗಮ rgBuf;
}

ನಾಮಾಕಾಶ {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
ಧ್ರುವ uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, ಪರಿಮಾಣ(map));
}

RegionValidateMap::~RegionValidateMap() {
}

ತಾರ್ಕಿಕ RegionValidateMap::isSet(ಧ್ರುವ ವರ್ಣ* region) ಧ್ರುವ {
    int32_t index = value(region);
    ಯದಿ (index < 0) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ 0 != (map[index / 32] & (1L << (index % 32)));
}

ತಾರ್ಕಿಕ RegionValidateMap::equals(ಧ್ರುವ RegionValidateMap& that) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ uprv_memcmp(map, that.map, ಪರಿಮಾಣ(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(ಧ್ರುವ ವರ್ಣ* region) ಧ್ರುವ {
    ಯದಿ (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        ಪ್ರತ್ಯಾಗಮ (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

U_NAMESPACE_END
