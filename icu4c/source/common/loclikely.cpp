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

#شامل <string_view>
#شامل <utility>

#شامل "unicode/bytestream.h"
#شامل "unicode/utypes.h"
#شامل "unicode/locid.h"
#شامل "unicode/putil.h"
#شامل "unicode/uchar.h"
#شامل "unicode/uloc.h"
#شامل "unicode/ures.h"
#شامل "unicode/uscript.h"
#شامل "bytesinkutil.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "loclikelysubtags.h"
#شامل "ulocimp.h"

فضاینام {

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
تهی U_CALLCONV
createTagStringWithAlternates(
    ثابت نویسه* lang,
    int32_t langLength,
    ثابت نویسه* script,
    int32_t scriptLength,
    ثابت نویسه* region,
    int32_t regionLength,
    ثابت نویسه* variant,
    int32_t variantLength,
    ثابت نویسه* trailing,
    int32_t trailingLength,
    icu::ByteSink& sink,
    UErrorCode& err) {
    اگر (U_FAILURE(err)) {
        بازگشت;
    }

    اگر (langLength >= ULOC_LANG_CAPACITY ||
            scriptLength >= ULOC_SCRIPT_CAPACITY ||
            regionLength >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    اگر (langLength > 0) {
        sink.Append(lang, langLength);
    }

    اگر (scriptLength > 0) {
        sink.Append("_", 1);
        sink.Append(script, scriptLength);
    }

    اگر (regionLength > 0) {
        sink.Append("_", 1);
        sink.Append(region, regionLength);
    }

    اگر (variantLength > 0) {
        اگر (regionLength == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variantLength);
    }

    اگر (trailingLength > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailingLength);
    }
}

منطقی CHECK_TRAILING_VARIANT_SIZE(ثابت نویسه* variant, int32_t variantLength) {
    int32_t count = 0;
    برای (int32_t i = 0; i < variantLength; i++) {
        اگر (_isIDSeparator(variant[i])) {
            count = 0;
        } وگرنه اگر (count == 8) {
            بازگشت نادرست;
        } وگرنه {
            count++;
        }
    }
    بازگشت درست;
}

تهی
_uloc_addLikelySubtags(ثابت نویسه* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    اگر (U_FAILURE(err)) {
        بازگشت;
    }

    اگر (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ثابت نویسه* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    اگر (U_FAILURE(err)) {
        بازگشت;
    }

    اگر (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    اگر (lang.length() == 4) {
        اگر (script.isEmpty()) {
            script = std::move(lang);
            lang.clear();
        } وگرنه {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت;
        }
    } وگرنه اگر (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ثابت icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    اگر (U_FAILURE(err)) {
        بازگشت;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    اگر (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, درست, err);
    اگر (U_FAILURE(err)) {
        بازگشت;
    }
    ثابت نویسه* language = lsr.language;
    اگر (uprv_strcmp(language, "und") == 0) {
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

تهی
_uloc_minimizeSubtags(ثابت نویسه* localeID,
                      icu::ByteSink& sink,
                      منطقی favorScript,
                      UErrorCode& err) {
    اگر (U_FAILURE(err)) {
        بازگشت;
    }

    اگر (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ثابت نویسه* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    اگر (U_FAILURE(err)) {
        بازگشت;
    }

    اگر (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ثابت icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    اگر (U_FAILURE(err)) {
        بازگشت;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    اگر (U_FAILURE(err)) {
        بازگشت;
    }
    ثابت نویسه* language = lsr.language;
    اگر (uprv_strcmp(language, "und") == 0) {
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
uloc_addLikelySubtags(ثابت نویسه* localeID,
                      نویسه* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    بازگشت icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(ثابت نویسه* localeID,
                         UErrorCode& status) {
    بازگشت icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT تهی
ulocimp_addLikelySubtags(ثابت نویسه* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    اگر (U_FAILURE(status)) { بازگشت; }
    اگر (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(localeBuffer.data(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(ثابت نویسه* localeID,
                     نویسه* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    بازگشت icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, نادرست, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(ثابت نویسه* localeID,
                        منطقی favorScript,
                        UErrorCode& status) {
    بازگشت icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT تهی
ulocimp_minimizeSubtags(ثابت نویسه* localeID,
                        icu::ByteSink& sink,
                        منطقی favorScript,
                        UErrorCode& status) {
    اگر (U_FAILURE(status)) { بازگشت; }
    اگر (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(localeBuffer.data(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
ایستا ثابت نویسه LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_راست_To_چپ(ثابت نویسه *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    اگر (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        اگر (!lang.isEmpty()) {
            ثابت نویسه* langPtr = uprv_strstr(LANG_DIR_STRING, lang.data());
            اگر (langPtr != nullptr) {
                انتخاب (langPtr[lang.length()]) {
                حالت '-': بازگشت نادرست;
                حالت '+': بازگشت درست;
                پیشفرض: بشکن;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        اگر (U_FAILURE(errorCode)) {
            بازگشت نادرست;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        اگر (U_FAILURE(errorCode) || script.isEmpty()) {
            بازگشت نادرست;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_get_Property_مقدار_Enum(UCHAR_SCRIPT, script.data());
    بازگشت uscript_is_راست_To_چپ(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_راست_To_چپ() ثابت {
    بازگشت uloc_is_راست_To_چپ(getBaseName());
}

U_NAMESPACE_END

فضاینام {
icu::CharString
Get_Region_From_کلید(ثابت نویسه* localeID, std::string_view کلید, UErrorCode& status) {
    icu::CharString result;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_مقدار(localeID, کلید, status);
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
    اگر (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        ثابت نویسه region[] = {kw[0], kw[1], '\0'};
        اگر (valid.isSet(region)) {
            result.append(uprv_toupper(kw[0]), status);
            result.append(uprv_toupper(kw[1]), status);
        }
    }
    بازگشت result;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_getRegionForSupplementalData(ثابت نویسه *localeID, منطقی inferRegion,
                                     UErrorCode& status) {
    اگر (U_FAILURE(status)) {
        بازگشت {};
    }
    icu::CharString rgBuf = Get_Region_From_کلید(localeID, "rg", status);
    اگر (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        اگر (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_کلید(localeID, "sd", status);
            اگر (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                اگر (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    بازگشت rgBuf;
}

فضاینام {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
ثابت uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, اندازه(map));
}

RegionValidateMap::~RegionValidateMap() {
}

منطقی RegionValidateMap::isSet(ثابت نویسه* region) ثابت {
    int32_t index = value(region);
    اگر (index < 0) {
        بازگشت نادرست;
    }
    بازگشت 0 != (map[index / 32] & (1L << (index % 32)));
}

منطقی RegionValidateMap::equals(ثابت RegionValidateMap& that) ثابت {
    بازگشت uprv_memcmp(map, that.map, اندازه(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(ثابت نویسه* region) ثابت {
    اگر (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        بازگشت (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    بازگشت -1;
}

U_NAMESPACE_END
