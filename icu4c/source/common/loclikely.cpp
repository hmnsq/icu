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

#অন্তর্ভাৱ <string_view>
#অন্তর্ভাৱ <utility>

#অন্তর্ভাৱ "unicode/bytestream.h"
#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/locid.h"
#অন্তর্ভাৱ "unicode/putil.h"
#অন্তর্ভাৱ "unicode/uchar.h"
#অন্তর্ভাৱ "unicode/uloc.h"
#অন্তর্ভাৱ "unicode/ures.h"
#অন্তর্ভাৱ "unicode/uscript.h"
#অন্তর্ভাৱ "bytesinkutil.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "loclikelysubtags.h"
#অন্তর্ভাৱ "ulocimp.h"

নামাকাশ {

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
শূন্য U_CALLCONV
createTagStringWithAlternates(
    ধ্রুৱ ৱর্ণ* lang,
    int32_t langLength,
    ধ্রুৱ ৱর্ণ* script,
    int32_t scriptLength,
    ধ্রুৱ ৱর্ণ* region,
    int32_t regionLength,
    ধ্রুৱ ৱর্ণ* variant,
    int32_t variantLength,
    ধ্রুৱ ৱর্ণ* trailing,
    int32_t trailingLength,
    icu::ByteSink& sink,
    UErrorCode& err) {
    যদি (U_FAILURE(err)) {
        ঘূৰাই;
    }

    যদি (langLength >= ULOC_LANG_CAPACITY ||
            scriptLength >= ULOC_SCRIPT_CAPACITY ||
            regionLength >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    যদি (langLength > 0) {
        sink.Append(lang, langLength);
    }

    যদি (scriptLength > 0) {
        sink.Append("_", 1);
        sink.Append(script, scriptLength);
    }

    যদি (regionLength > 0) {
        sink.Append("_", 1);
        sink.Append(region, regionLength);
    }

    যদি (variantLength > 0) {
        যদি (regionLength == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variantLength);
    }

    যদি (trailingLength > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailingLength);
    }
}

তার্কিক CHECK_TRAILING_VARIANT_SIZE(ধ্রুৱ ৱর্ণ* variant, int32_t variantLength) {
    int32_t count = 0;
    বাবে (int32_t i = 0; i < variantLength; i++) {
        যদি (_isIDSeparator(variant[i])) {
            count = 0;
        } অন্যথা যদি (count == 8) {
            ঘূৰাই মিছা;
        } অন্যথা {
            count++;
        }
    }
    ঘূৰাই সঁচা;
}

শূন্য
_uloc_addLikelySubtags(ধ্রুৱ ৱর্ণ* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    যদি (U_FAILURE(err)) {
        ঘূৰাই;
    }

    যদি (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ধ্রুৱ ৱর্ণ* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    যদি (U_FAILURE(err)) {
        ঘূৰাই;
    }

    যদি (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    যদি (lang.length() == 4) {
        যদি (script.isEmpty()) {
            script = std::move(lang);
            lang.clear();
        } অন্যথা {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই;
        }
    } অন্যথা যদি (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ধ্রুৱ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    যদি (U_FAILURE(err)) {
        ঘূৰাই;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    যদি (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, সঁচা, err);
    যদি (U_FAILURE(err)) {
        ঘূৰাই;
    }
    ধ্রুৱ ৱর্ণ* language = lsr.language;
    যদি (uprv_strcmp(language, "und") == 0) {
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

শূন্য
_uloc_minimizeSubtags(ধ্রুৱ ৱর্ণ* localeID,
                      icu::ByteSink& sink,
                      তার্কিক favorScript,
                      UErrorCode& err) {
    যদি (U_FAILURE(err)) {
        ঘূৰাই;
    }

    যদি (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ধ্রুৱ ৱর্ণ* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    যদি (U_FAILURE(err)) {
        ঘূৰাই;
    }

    যদি (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ধ্রুৱ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    যদি (U_FAILURE(err)) {
        ঘূৰাই;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    যদি (U_FAILURE(err)) {
        ঘূৰাই;
    }
    ধ্রুৱ ৱর্ণ* language = lsr.language;
    যদি (uprv_strcmp(language, "und") == 0) {
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
uloc_addLikelySubtags(ধ্রুৱ ৱর্ণ* localeID,
                      ৱর্ণ* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    ঘূৰাই icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(ধ্রুৱ ৱর্ণ* localeID,
                         UErrorCode& status) {
    ঘূৰাই icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT শূন্য
ulocimp_addLikelySubtags(ধ্রুৱ ৱর্ণ* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    যদি (U_FAILURE(status)) { ঘূৰাই; }
    যদি (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(localeBuffer.data(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(ধ্রুৱ ৱর্ণ* localeID,
                     ৱর্ণ* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    ঘূৰাই icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, মিছা, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(ধ্রুৱ ৱর্ণ* localeID,
                        তার্কিক favorScript,
                        UErrorCode& status) {
    ঘূৰাই icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT শূন্য
ulocimp_minimizeSubtags(ধ্রুৱ ৱর্ণ* localeID,
                        icu::ByteSink& sink,
                        তার্কিক favorScript,
                        UErrorCode& status) {
    যদি (U_FAILURE(status)) { ঘূৰাই; }
    যদি (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(localeBuffer.data(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
স্থির ধ্রুৱ ৱর্ণ LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_ডান_To_বাম(ধ্রুৱ ৱর্ণ *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    যদি (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        যদি (!lang.isEmpty()) {
            ধ্রুৱ ৱর্ণ* langPtr = uprv_strstr(LANG_DIR_STRING, lang.data());
            যদি (langPtr != nullptr) {
                ৱিকল্প (langPtr[lang.length()]) {
                পক্ষ '-': ঘূৰাই মিছা;
                পক্ষ '+': ঘূৰাই সঁচা;
                স্ৱভাৱ: ভঙ্গ;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        যদি (U_FAILURE(errorCode)) {
            ঘূৰাই মিছা;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        যদি (U_FAILURE(errorCode) || script.isEmpty()) {
            ঘূৰাই মিছা;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_get_Property_মান_Enum(UCHAR_SCRIPT, script.data());
    ঘূৰাই uscript_is_ডান_To_বাম(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_ডান_To_বাম() ধ্রুৱ {
    ঘূৰাই uloc_is_ডান_To_বাম(getBaseName());
}

U_NAMESPACE_END

নামাকাশ {
icu::CharString
Get_Region_From_চাবি(ধ্রুৱ ৱর্ণ* localeID, std::string_view চাবি, UErrorCode& status) {
    icu::CharString result;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_মান(localeID, চাবি, status);
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
    যদি (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        ধ্রুৱ ৱর্ণ region[] = {kw[0], kw[1], '\0'};
        যদি (valid.isSet(region)) {
            result.append(uprv_toupper(kw[0]), status);
            result.append(uprv_toupper(kw[1]), status);
        }
    }
    ঘূৰাই result;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_getRegionForSupplementalData(ধ্রুৱ ৱর্ণ *localeID, তার্কিক inferRegion,
                                     UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই {};
    }
    icu::CharString rgBuf = Get_Region_From_চাবি(localeID, "rg", status);
    যদি (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        যদি (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_চাবি(localeID, "sd", status);
            যদি (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                যদি (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    ঘূৰাই rgBuf;
}

নামাকাশ {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
ধ্রুৱ uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, পরিমাণ(map));
}

RegionValidateMap::~RegionValidateMap() {
}

তার্কিক RegionValidateMap::isSet(ধ্রুৱ ৱর্ণ* region) ধ্রুৱ {
    int32_t index = value(region);
    যদি (index < 0) {
        ঘূৰাই মিছা;
    }
    ঘূৰাই 0 != (map[index / 32] & (1L << (index % 32)));
}

তার্কিক RegionValidateMap::equals(ধ্রুৱ RegionValidateMap& that) ধ্রুৱ {
    ঘূৰাই uprv_memcmp(map, that.map, পরিমাণ(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(ধ্রুৱ ৱর্ণ* region) ধ্রুৱ {
    যদি (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        ঘূৰাই (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    ঘূৰাই -1;
}

U_NAMESPACE_END
