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

#அந்தர்பாவ <string_view>
#அந்தர்பாவ <utility>

#அந்தர்பாவ "unicode/bytestream.h"
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/uloc.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "unicode/uscript.h"
#அந்தர்பாவ "bytesinkutil.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "loclikelysubtags.h"
#அந்தர்பாவ "ulocimp.h"

நாமாகாஶ {

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
வெறுமை U_CALLCONV
createTagStringWithAlternates(
    மாறிலி எழுத்து* lang,
    int32_t langLength,
    மாறிலி எழுத்து* script,
    int32_t scriptLength,
    மாறிலி எழுத்து* region,
    int32_t regionLength,
    மாறிலி எழுத்து* variant,
    int32_t variantLength,
    மாறிலி எழுத்து* trailing,
    int32_t trailingLength,
    icu::ByteSink& sink,
    UErrorCode& err) {
    எனில் (U_FAILURE(err)) {
        திரும்பு;
    }

    எனில் (langLength >= ULOC_LANG_CAPACITY ||
            scriptLength >= ULOC_SCRIPT_CAPACITY ||
            regionLength >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    எனில் (langLength > 0) {
        sink.Append(lang, langLength);
    }

    எனில் (scriptLength > 0) {
        sink.Append("_", 1);
        sink.Append(script, scriptLength);
    }

    எனில் (regionLength > 0) {
        sink.Append("_", 1);
        sink.Append(region, regionLength);
    }

    எனில் (variantLength > 0) {
        எனில் (regionLength == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variantLength);
    }

    எனில் (trailingLength > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailingLength);
    }
}

தருக்க CHECK_TRAILING_VARIANT_SIZE(மாறிலி எழுத்து* variant, int32_t variantLength) {
    int32_t count = 0;
    க்கு (int32_t i = 0; i < variantLength; i++) {
        எனில் (_isIDSeparator(variant[i])) {
            count = 0;
        } இல்லையெனில் எனில் (count == 8) {
            திரும்பு பொய்;
        } இல்லையெனில் {
            count++;
        }
    }
    திரும்பு உண்மை;
}

வெறுமை
_uloc_addLikelySubtags(மாறிலி எழுத்து* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    எனில் (U_FAILURE(err)) {
        திரும்பு;
    }

    எனில் (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    மாறிலி எழுத்து* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    எனில் (U_FAILURE(err)) {
        திரும்பு;
    }

    எனில் (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    எனில் (lang.length() == 4) {
        எனில் (script.isEmpty()) {
            script = std::move(lang);
            lang.clear();
        } இல்லையெனில் {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு;
        }
    } இல்லையெனில் எனில் (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    மாறிலி icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    எனில் (U_FAILURE(err)) {
        திரும்பு;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    எனில் (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, உண்மை, err);
    எனில் (U_FAILURE(err)) {
        திரும்பு;
    }
    மாறிலி எழுத்து* language = lsr.language;
    எனில் (uprv_strcmp(language, "und") == 0) {
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

வெறுமை
_uloc_minimizeSubtags(மாறிலி எழுத்து* localeID,
                      icu::ByteSink& sink,
                      தருக்க favorScript,
                      UErrorCode& err) {
    எனில் (U_FAILURE(err)) {
        திரும்பு;
    }

    எனில் (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    மாறிலி எழுத்து* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    எனில் (U_FAILURE(err)) {
        திரும்பு;
    }

    எனில் (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    மாறிலி icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    எனில் (U_FAILURE(err)) {
        திரும்பு;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    எனில் (U_FAILURE(err)) {
        திரும்பு;
    }
    மாறிலி எழுத்து* language = lsr.language;
    எனில் (uprv_strcmp(language, "und") == 0) {
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
uloc_addLikelySubtags(மாறிலி எழுத்து* localeID,
                      எழுத்து* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    திரும்பு icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(மாறிலி எழுத்து* localeID,
                         UErrorCode& status) {
    திரும்பு icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT வெறுமை
ulocimp_addLikelySubtags(மாறிலி எழுத்து* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    எனில் (U_FAILURE(status)) { திரும்பு; }
    எனில் (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(localeBuffer.data(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(மாறிலி எழுத்து* localeID,
                     எழுத்து* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    திரும்பு icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, பொய், status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(மாறிலி எழுத்து* localeID,
                        தருக்க favorScript,
                        UErrorCode& status) {
    திரும்பு icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT வெறுமை
ulocimp_minimizeSubtags(மாறிலி எழுத்து* localeID,
                        icu::ByteSink& sink,
                        தருக்க favorScript,
                        UErrorCode& status) {
    எனில் (U_FAILURE(status)) { திரும்பு; }
    எனில் (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(localeBuffer.data(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
நிலையான மாறிலி எழுத்து LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_வலது_To_இடது(மாறிலி எழுத்து *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    எனில் (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        எனில் (!lang.isEmpty()) {
            மாறிலி எழுத்து* langPtr = uprv_strstr(LANG_DIR_STRING, lang.data());
            எனில் (langPtr != nullptr) {
                தேர்வு (langPtr[lang.length()]) {
                நிலை '-': திரும்பு பொய்;
                நிலை '+': திரும்பு உண்மை;
                இயல்பு: நிறுத்து;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        எனில் (U_FAILURE(errorCode)) {
            திரும்பு பொய்;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        எனில் (U_FAILURE(errorCode) || script.isEmpty()) {
            திரும்பு பொய்;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_get_Property_மதிப்பு_Enum(UCHAR_SCRIPT, script.data());
    திரும்பு uscript_is_வலது_To_இடது(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_வலது_To_இடது() மாறிலி {
    திரும்பு uloc_is_வலது_To_இடது(getBaseName());
}

U_NAMESPACE_END

நாமாகாஶ {
icu::CharString
Get_Region_From_சாவி(மாறிலி எழுத்து* localeID, std::string_view சாவி, UErrorCode& status) {
    icu::CharString result;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_மதிப்பு(localeID, சாவி, status);
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
    எனில் (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        மாறிலி எழுத்து region[] = {kw[0], kw[1], '\0'};
        எனில் (valid.isSet(region)) {
            result.append(uprv_toupper(kw[0]), status);
            result.append(uprv_toupper(kw[1]), status);
        }
    }
    திரும்பு result;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_getRegionForSupplementalData(மாறிலி எழுத்து *localeID, தருக்க inferRegion,
                                     UErrorCode& status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு {};
    }
    icu::CharString rgBuf = Get_Region_From_சாவி(localeID, "rg", status);
    எனில் (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        எனில் (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_சாவி(localeID, "sd", status);
            எனில் (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                எனில் (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    திரும்பு rgBuf;
}

நாமாகாஶ {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
மாறிலி uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, அளவு(map));
}

RegionValidateMap::~RegionValidateMap() {
}

தருக்க RegionValidateMap::isSet(மாறிலி எழுத்து* region) மாறிலி {
    int32_t index = value(region);
    எனில் (index < 0) {
        திரும்பு பொய்;
    }
    திரும்பு 0 != (map[index / 32] & (1L << (index % 32)));
}

தருக்க RegionValidateMap::equals(மாறிலி RegionValidateMap& that) மாறிலி {
    திரும்பு uprv_memcmp(map, that.map, அளவு(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(மாறிலி எழுத்து* region) மாறிலி {
    எனில் (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        திரும்பு (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    திரும்பு -1;
}

U_NAMESPACE_END
