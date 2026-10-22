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

#ଅନ୍ତର୍ଭାଵ <string_view>
#ଅନ୍ତର୍ଭାଵ <utility>

#ଅନ୍ତର୍ଭାଵ "unicode/bytestream.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uscript.h"
#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "loclikelysubtags.h"
#ଅନ୍ତର୍ଭାଵ "ulocimp.h"

ନାମାକାଶ {

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
ଶୂନ୍ୟ U_CALLCONV
createTagStringWithAlternates(
    ଅଚଳ ଅକ୍ଷର* lang,
    int32_t langLength,
    ଅଚଳ ଅକ୍ଷର* script,
    int32_t scriptLength,
    ଅଚଳ ଅକ୍ଷର* region,
    int32_t regionLength,
    ଅଚଳ ଅକ୍ଷର* variant,
    int32_t variantLength,
    ଅଚଳ ଅକ୍ଷର* trailing,
    int32_t trailingLength,
    icu::ByteSink& sink,
    UErrorCode& err) {
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି (langLength >= ULOC_LANG_CAPACITY ||
            scriptLength >= ULOC_SCRIPT_CAPACITY ||
            regionLength >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    ଯଦି (langLength > 0) {
        sink.Append(lang, langLength);
    }

    ଯଦି (scriptLength > 0) {
        sink.Append("_", 1);
        sink.Append(script, scriptLength);
    }

    ଯଦି (regionLength > 0) {
        sink.Append("_", 1);
        sink.Append(region, regionLength);
    }

    ଯଦି (variantLength > 0) {
        ଯଦି (regionLength == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variantLength);
    }

    ଯଦି (trailingLength > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailingLength);
    }
}

ତାର୍କିକ CHECK_TRAILING_VARIANT_SIZE(ଅଚଳ ଅକ୍ଷର* variant, int32_t variantLength) {
    int32_t count = 0;
    ପାଇଁ (int32_t i = 0; i < variantLength; i++) {
        ଯଦି (_isIDSeparator(variant[i])) {
            count = 0;
        } ଅନ୍ୟଥା ଯଦି (count == 8) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        } ଅନ୍ୟଥା {
            count++;
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ
_uloc_addLikelySubtags(ଅଚଳ ଅକ୍ଷର* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ଅଚଳ ଅକ୍ଷର* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    ଯଦି (lang.length() == 4) {
        ଯଦି (script.isEmpty()) {
            script = std::move(lang);
            lang.clear();
        } ଅନ୍ୟଥା {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ;
        }
    } ଅନ୍ୟଥା ଯଦି (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ଅଚଳ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    ଯଦି (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, ସତ୍ୟ, err);
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }
    ଅଚଳ ଅକ୍ଷର* language = lsr.language;
    ଯଦି (uprv_strcmp(language, "und") == 0) {
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

ଶୂନ୍ୟ
_uloc_minimizeSubtags(ଅଚଳ ଅକ୍ଷର* localeID,
                      icu::ByteSink& sink,
                      ତାର୍କିକ favorScript,
                      UErrorCode& err) {
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ଅଚଳ ଅକ୍ଷର* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ଅଚଳ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }
    ଅଚଳ ଅକ୍ଷର* language = lsr.language;
    ଯଦି (uprv_strcmp(language, "und") == 0) {
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
uloc_addLikelySubtags(ଅଚଳ ଅକ୍ଷର* localeID,
                      ଅକ୍ଷର* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    ଫେରନ୍ତୁ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(ଅଚଳ ଅକ୍ଷର* localeID,
                         UErrorCode& status) {
    ଫେରନ୍ତୁ icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT ଶୂନ୍ୟ
ulocimp_addLikelySubtags(ଅଚଳ ଅକ୍ଷର* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
    ଯଦି (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(localeBuffer.data(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(ଅଚଳ ଅକ୍ଷର* localeID,
                     ଅକ୍ଷର* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    ଫେରନ୍ତୁ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, ମିଥ୍ୟା, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(ଅଚଳ ଅକ୍ଷର* localeID,
                        ତାର୍କିକ favorScript,
                        UErrorCode& status) {
    ଫେରନ୍ତୁ icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT ଶୂନ୍ୟ
ulocimp_minimizeSubtags(ଅଚଳ ଅକ୍ଷର* localeID,
                        icu::ByteSink& sink,
                        ତାର୍କିକ favorScript,
                        UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
    ଯଦି (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(localeBuffer.data(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_ଡାହାଣ_To_ବାମ(ଅଚଳ ଅକ୍ଷର *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    ଯଦି (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        ଯଦି (!lang.isEmpty()) {
            ଅଚଳ ଅକ୍ଷର* langPtr = uprv_strstr(LANG_DIR_STRING, lang.data());
            ଯଦି (langPtr != nullptr) {
                ବାଛ (langPtr[lang.length()]) {
                ସ୍ଥିତି '-': ଫେରନ୍ତୁ ମିଥ୍ୟା;
                ସ୍ଥିତି '+': ଫେରନ୍ତୁ ସତ୍ୟ;
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଭାଙ୍ଗ;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        ଯଦି (U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        ଯଦି (U_FAILURE(errorCode) || script.isEmpty()) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_get_Property_ମୂଲ୍ୟ_Enum(UCHAR_SCRIPT, script.data());
    ଫେରନ୍ତୁ uscript_is_ଡାହାଣ_To_ବାମ(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_ଡାହାଣ_To_ବାମ() ଅଚଳ {
    ଫେରନ୍ତୁ uloc_is_ଡାହାଣ_To_ବାମ(getBaseName());
}

U_NAMESPACE_END

ନାମାକାଶ {
icu::CharString
Get_Region_From_ଚାବି(ଅଚଳ ଅକ୍ଷର* localeID, std::string_view ଚାବି, UErrorCode& status) {
    icu::CharString result;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_ମୂଲ୍ୟ(localeID, ଚାବି, status);
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
    ଯଦି (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        ଅଚଳ ଅକ୍ଷର region[] = {kw[0], kw[1], '\0'};
        ଯଦି (valid.isSet(region)) {
            result.append(uprv_toupper(kw[0]), status);
            result.append(uprv_toupper(kw[1]), status);
        }
    }
    ଫେରନ୍ତୁ result;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_getRegionForSupplementalData(ଅଚଳ ଅକ୍ଷର *localeID, ତାର୍କିକ inferRegion,
                                     UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ {};
    }
    icu::CharString rgBuf = Get_Region_From_ଚାବି(localeID, "rg", status);
    ଯଦି (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        ଯଦି (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_ଚାବି(localeID, "sd", status);
            ଯଦି (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                ଯଦି (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    ଫେରନ୍ତୁ rgBuf;
}

ନାମାକାଶ {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
ଅଚଳ uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, ଆକାର(map));
}

RegionValidateMap::~RegionValidateMap() {
}

ତାର୍କିକ RegionValidateMap::isSet(ଅଚଳ ଅକ୍ଷର* region) ଅଚଳ {
    int32_t index = value(region);
    ଯଦି (index < 0) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ 0 != (map[index / 32] & (1L << (index % 32)));
}

ତାର୍କିକ RegionValidateMap::equals(ଅଚଳ RegionValidateMap& that) ଅଚଳ {
    ଫେରନ୍ତୁ uprv_memcmp(map, that.map, ଆକାର(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(ଅଚଳ ଅକ୍ଷର* region) ଅଚଳ {
    ଯଦି (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        ଫେରନ୍ତୁ (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    ଫେରନ୍ତୁ -1;
}

U_NAMESPACE_END
