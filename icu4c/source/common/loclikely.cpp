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

#include <string_view>
#include <utility>

#include "unicode/bytestream.h"
#include "unicode/utypes.h"
#include "unicode/locid.h"
#include "unicode/putil.h"
#include "unicode/uchar.h"
#include "unicode/uloc.h"
#include "unicode/ures.h"
#include "unicode/uscript.h"
#include "bytesinkutil.h"
#include "charstr.h"
#include "cmemory.h"
#include "cstring.h"
#include "loclikelysubtags.h"
#include "ulocimp.h"

namespace {

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
void U_CALLCONV
createTagStringWithAlternates(
    const char* lang,
    int32_t langLength,
    const char* script,
    int32_t scriptLength,
    const char* region,
    int32_t regionLength,
    const char* variant,
    int32_t variantLength,
    const char* trailing,
    int32_t trailingLength,
    icu::ByteSink& sink,
    UErrorCode& err) {
    ако (U_FAILURE(err)) {
        врати;
    }

    ако (langLength >= ULOC_LANG_CAPACITY ||
            scriptLength >= ULOC_SCRIPT_CAPACITY ||
            regionLength >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        врати;
    }

    ако (langLength > 0) {
        sink.Append(lang, langLength);
    }

    ако (scriptLength > 0) {
        sink.Append("_", 1);
        sink.Append(script, scriptLength);
    }

    ако (regionLength > 0) {
        sink.Append("_", 1);
        sink.Append(region, regionLength);
    }

    ако (variantLength > 0) {
        ако (regionLength == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variantLength);
    }

    ако (trailingLength > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailingLength);
    }
}

bool CHECK_TRAILING_VARIANT_SIZE(const char* variant, int32_t variantLength) {
    int32_t count = 0;
    за (int32_t i = 0; i < variantLength; i++) {
        ако (_isIDSeparator(variant[i])) {
            count = 0;
        } инако ако (count == 8) {
            врати false;
        } инако {
            count++;
        }
    }
    врати true;
}

void
_uloc_addLikelySubtags(const char* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    ако (U_FAILURE(err)) {
        врати;
    }

    ако (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        врати;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    const char* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ако (U_FAILURE(err)) {
        врати;
    }

    ако (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        врати;
    }

    ако (lang.length() == 4) {
        ако (script.isEmpty()) {
            script = std::move(lang);
            lang.clear();
        } инако {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            врати;
        }
    } инако ако (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        врати;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    const icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ако (U_FAILURE(err)) {
        врати;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    ако (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        врати;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, true, err);
    ако (U_FAILURE(err)) {
        врати;
    }
    const char* language = lsr.language;
    ако (uprv_strcmp(language, "und") == 0) {
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

void
_uloc_minimizeSubtags(const char* localeID,
                      icu::ByteSink& sink,
                      bool favorScript,
                      UErrorCode& err) {
    ако (U_FAILURE(err)) {
        врати;
    }

    ако (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        врати;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    const char* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ако (U_FAILURE(err)) {
        врати;
    }

    ако (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        врати;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    const icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ако (U_FAILURE(err)) {
        врати;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    ако (U_FAILURE(err)) {
        врати;
    }
    const char* language = lsr.language;
    ако (uprv_strcmp(language, "und") == 0) {
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
uloc_addLikelySubtags(const char* localeID,
                      char* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    врати icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(const char* localeID,
                         UErrorCode& status) {
    врати icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT void
ulocimp_addLikelySubtags(const char* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    ако (U_FAILURE(status)) { врати; }
    ако (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(localeBuffer.data(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(const char* localeID,
                     char* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    врати icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, false, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(const char* localeID,
                        bool favorScript,
                        UErrorCode& status) {
    врати icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT void
ulocimp_minimizeSubtags(const char* localeID,
                        icu::ByteSink& sink,
                        bool favorScript,
                        UErrorCode& status) {
    ако (U_FAILURE(status)) { врати; }
    ако (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(localeBuffer.data(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
static const char LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_isRightToLeft(const char *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    ако (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        ако (!lang.isEmpty()) {
            const char* langPtr = uprv_strstr(LANG_DIR_STRING, lang.data());
            ако (langPtr != nullptr) {
                switch (langPtr[lang.length()]) {
                case '-': врати false;
                case '+': врати true;
                default: break;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        ако (U_FAILURE(errorCode)) {
            врати false;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        ако (U_FAILURE(errorCode) || script.isEmpty()) {
            врати false;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_getPropertyValueEnum(UCHAR_SCRIPT, script.data());
    врати uscript_isRightToLeft(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::isRightToLeft() const {
    врати uloc_isRightToLeft(getBaseName());
}

U_NAMESPACE_END

namespace {
icu::CharString
GetRegionFromKey(const char* localeID, std::string_view key, UErrorCode& status) {
    icu::CharString result;
    // First check for keyword value
    icu::CharString kw = ulocimp_getKeywordValue(localeID, key, status);
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
    ако (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        const char region[] = {kw[0], kw[1], '\0'};
        ако (valid.isSet(region)) {
            result.append(uprv_toupper(kw[0]), status);
            result.append(uprv_toupper(kw[1]), status);
        }
    }
    врати result;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_getRegionForSupplementalData(const char *localeID, bool inferRegion,
                                     UErrorCode& status) {
    ако (U_FAILURE(status)) {
        врати {};
    }
    icu::CharString rgBuf = GetRegionFromKey(localeID, "rg", status);
    ако (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        ако (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = GetRegionFromKey(localeID, "sd", status);
            ако (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                ако (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    врати rgBuf;
}

namespace {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
const uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, sizeof(map));
}

RegionValidateMap::~RegionValidateMap() {
}

bool RegionValidateMap::isSet(const char* region) const {
    int32_t index = value(region);
    ако (index < 0) {
        врати false;
    }
    врати 0 != (map[index / 32] & (1L << (index % 32)));
}

bool RegionValidateMap::equals(const RegionValidateMap& that) const {
    врати uprv_memcmp(map, that.map, sizeof(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(const char* region) const {
    ако (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        врати (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    врати -1;
}

U_NAMESPACE_END
