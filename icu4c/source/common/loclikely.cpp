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

#ضمن <string_view>
#ضمن <utility>

#ضمن "unicode/bytestream.h"
#ضمن "unicode/utypes.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/putil.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/uloc.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/uscript.h"
#ضمن "bytesinkutil.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "loclikelysubtags.h"
#ضمن "ulocimp.h"

نطاق {

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
فراغ U_CALLCONV
createTagStringWithAlternates(
    ثابت حرف* lang,
    int32_t langLength,
    ثابت حرف* script,
    int32_t scriptLength,
    ثابت حرف* region,
    int32_t regionLength,
    ثابت حرف* variant,
    int32_t variantLength,
    ثابت حرف* trailing,
    int32_t trailingLength,
    icu::ByteSink& sink,
    UErrorCode& err) {
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    إذا (langLength >= ULOC_LANG_CAPACITY ||
            scriptLength >= ULOC_SCRIPT_CAPACITY ||
            regionLength >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

    إذا (langLength > 0) {
        sink.Append(lang, langLength);
    }

    إذا (scriptLength > 0) {
        sink.Append("_", 1);
        sink.Append(script, scriptLength);
    }

    إذا (regionLength > 0) {
        sink.Append("_", 1);
        sink.Append(region, regionLength);
    }

    إذا (variantLength > 0) {
        إذا (regionLength == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variantLength);
    }

    إذا (trailingLength > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailingLength);
    }
}

منطقي CHECK_TRAILING_VARIANT_SIZE(ثابت حرف* variant, int32_t variantLength) {
    int32_t count = 0;
    لكل (int32_t i = 0; i < variantLength; i++) {
        إذا (_isIDSeparator(variant[i])) {
            count = 0;
        } وإلا إذا (count == 8) {
            أرجع خطأ;
        } وإلا {
            count++;
        }
    }
    أرجع true;
}

فراغ
_uloc_addLikelySubtags(ثابت حرف* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    إذا (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ثابت حرف* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    إذا (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

    إذا (lang.length() == 4) {
        إذا (script.isEmpty()) {
            script = std::move(lang);
            lang.clear();
        } وإلا {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع;
        }
    } وإلا إذا (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ثابت icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    إذا (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, true, err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    ثابت حرف* language = lsr.language;
    إذا (uprv_strcmp(language, "und") == 0) {
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

فراغ
_uloc_minimizeSubtags(ثابت حرف* localeID,
                      icu::ByteSink& sink,
                      منطقي favorScript,
                      UErrorCode& err) {
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    إذا (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ثابت حرف* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    إذا (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ثابت icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    ثابت حرف* language = lsr.language;
    إذا (uprv_strcmp(language, "und") == 0) {
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
uloc_addLikelySubtags(ثابت حرف* localeID,
                      حرف* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    أرجع icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(ثابت حرف* localeID,
                         UErrorCode& status) {
    أرجع icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT فراغ
ulocimp_addLikelySubtags(ثابت حرف* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    إذا (U_FAILURE(status)) { أرجع; }
    إذا (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(localeBuffer.data(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(ثابت حرف* localeID,
                     حرف* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    أرجع icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, خطأ, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(ثابت حرف* localeID,
                        منطقي favorScript,
                        UErrorCode& status) {
    أرجع icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT فراغ
ulocimp_minimizeSubtags(ثابت حرف* localeID,
                        icu::ByteSink& sink,
                        منطقي favorScript,
                        UErrorCode& status) {
    إذا (U_FAILURE(status)) { أرجع; }
    إذا (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(localeBuffer.data(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
ساكن ثابت حرف LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_کانن_To_کيري(ثابت حرف *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    إذا (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        إذا (!lang.isEmpty()) {
            ثابت حرف* langPtr = uprv_strstr(LANG_DIR_STRING, lang.data());
            إذا (langPtr != nullptr) {
                بدل (langPtr[lang.length()]) {
                حالة '-': أرجع خطأ;
                حالة '+': أرجع true;
                افتراضي: اكسر;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        إذا (U_FAILURE(errorCode)) {
            أرجع خطأ;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        إذا (U_FAILURE(errorCode) || script.isEmpty()) {
            أرجع خطأ;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_get_Property_نيلاي_Enum(UCHAR_SCRIPT, script.data());
    أرجع uscript_is_کانن_To_کيري(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_کانن_To_کيري() ثابت {
    أرجع uloc_is_کانن_To_کيري(getBaseName());
}

U_NAMESPACE_END

نطاق {
icu::CharString
Get_Region_From_کونچي(ثابت حرف* localeID, std::string_view کونچي, UErrorCode& status) {
    icu::CharString result;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_نيلاي(localeID, کونچي, status);
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
    إذا (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        ثابت حرف region[] = {kw[0], kw[1], '\0'};
        إذا (valid.isSet(region)) {
            result.append(uprv_toupper(kw[0]), status);
            result.append(uprv_toupper(kw[1]), status);
        }
    }
    أرجع result;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_getRegionForSupplementalData(ثابت حرف *localeID, منطقي inferRegion,
                                     UErrorCode& status) {
    إذا (U_FAILURE(status)) {
        أرجع {};
    }
    icu::CharString rgBuf = Get_Region_From_کونچي(localeID, "rg", status);
    إذا (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        إذا (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_کونچي(localeID, "sd", status);
            إذا (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                إذا (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    أرجع rgBuf;
}

نطاق {

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
    uprv_memcpy(map, gValidRegionMap, حجم(map));
}

RegionValidateMap::~RegionValidateMap() {
}

منطقي RegionValidateMap::isSet(ثابت حرف* region) ثابت {
    int32_t index = value(region);
    إذا (index < 0) {
        أرجع خطأ;
    }
    أرجع 0 != (map[index / 32] & (1L << (index % 32)));
}

منطقي RegionValidateMap::equals(ثابت RegionValidateMap& that) ثابت {
    أرجع uprv_memcmp(map, that.map, حجم(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(ثابت حرف* region) ثابت {
    إذا (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        أرجع (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    أرجع -1;
}

U_NAMESPACE_END
