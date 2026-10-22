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
    const var_na* lang,
    int32_t langLength,
    const var_na* script,
    int32_t scriptLength,
    const var_na* region,
    int32_t regionLength,
    const var_na* variant,
    int32_t variantLength,
    const var_na* trailing,
    int32_t trailingLength,
    icu::ByteSink& sink,
    UErrorCode& err) {
    ຖ້າ (U_FAILURE(err)) {
        ສົ່ງຄືນ;
    }

    ຖ້າ (langLength >= ULOC_LANG_CAPACITY ||
            scriptLength >= ULOC_SCRIPT_CAPACITY ||
            regionLength >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }

    ຖ້າ (langLength > 0) {
        sink.Append(lang, langLength);
    }

    ຖ້າ (scriptLength > 0) {
        sink.Append("_", 1);
        sink.Append(script, scriptLength);
    }

    ຖ້າ (regionLength > 0) {
        sink.Append("_", 1);
        sink.Append(region, regionLength);
    }

    ຖ້າ (variantLength > 0) {
        ຖ້າ (regionLength == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variantLength);
    }

    ຖ້າ (trailingLength > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailingLength);
    }
}

bool CHECK_TRAILING_VARIANT_SIZE(const var_na* variant, int32_t variantLength) {
    int32_t count = 0;
    ສຳລັບ (int32_t i = 0; i < variantLength; i++) {
        ຖ້າ (_isIDSeparator(variant[i])) {
            count = 0;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (count == 8) {
            ສົ່ງຄືນ false;
        } ບໍ່ດັ່ງນັ້ນ {
            count++;
        }
    }
    ສົ່ງຄືນ true;
}

void
_uloc_addLikelySubtags(const var_na* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    ຖ້າ (U_FAILURE(err)) {
        ສົ່ງຄືນ;
    }

    ຖ້າ (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    const var_na* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ຖ້າ (U_FAILURE(err)) {
        ສົ່ງຄືນ;
    }

    ຖ້າ (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }

    ຖ້າ (lang.length() == 4) {
        ຖ້າ (script.isEmpty()) {
            script = std::move(lang);
            lang.clear();
        } ບໍ່ດັ່ງນັ້ນ {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            ສົ່ງຄືນ;
        }
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    const icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ຖ້າ (U_FAILURE(err)) {
        ສົ່ງຄືນ;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    ຖ້າ (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, true, err);
    ຖ້າ (U_FAILURE(err)) {
        ສົ່ງຄືນ;
    }
    const var_na* language = lsr.language;
    ຖ້າ (uprv_strcmp(language, "und") == 0) {
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
_uloc_minimizeSubtags(const var_na* localeID,
                      icu::ByteSink& sink,
                      bool favorScript,
                      UErrorCode& err) {
    ຖ້າ (U_FAILURE(err)) {
        ສົ່ງຄືນ;
    }

    ຖ້າ (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    const var_na* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ຖ້າ (U_FAILURE(err)) {
        ສົ່ງຄືນ;
    }

    ຖ້າ (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    const icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ຖ້າ (U_FAILURE(err)) {
        ສົ່ງຄືນ;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    ຖ້າ (U_FAILURE(err)) {
        ສົ່ງຄືນ;
    }
    const var_na* language = lsr.language;
    ຖ້າ (uprv_strcmp(language, "und") == 0) {
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
uloc_addLikelySubtags(const var_na* localeID,
                      var_na* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    ສົ່ງຄືນ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(const var_na* localeID,
                         UErrorCode& status) {
    ສົ່ງຄືນ icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT void
ulocimp_addLikelySubtags(const var_na* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ; }
    ຖ້າ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(localeBuffer.data(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(const var_na* localeID,
                     var_na* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    ສົ່ງຄືນ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, false, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(const var_na* localeID,
                        bool favorScript,
                        UErrorCode& status) {
    ສົ່ງຄືນ icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT void
ulocimp_minimizeSubtags(const var_na* localeID,
                        icu::ByteSink& sink,
                        bool favorScript,
                        UErrorCode& status) {
    ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ; }
    ຖ້າ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(localeBuffer.data(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
static const var_na LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_isRightToLeft(const var_na *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    ຖ້າ (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        ຖ້າ (!lang.isEmpty()) {
            const var_na* langPtr = uprv_strstr(LANG_DIR_STRING, lang.data());
            ຖ້າ (langPtr != nullptr) {
                switch (langPtr[lang.length()]) {
                pak_sa '-': ສົ່ງຄືນ false;
                pak_sa '+': ສົ່ງຄືນ true;
                default: bha_nga;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        ຖ້າ (U_FAILURE(errorCode)) {
            ສົ່ງຄືນ false;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        ຖ້າ (U_FAILURE(errorCode) || script.isEmpty()) {
            ສົ່ງຄືນ false;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_getPropertyValueEnum(UCHAR_SCRIPT, script.data());
    ສົ່ງຄືນ uscript_isRightToLeft(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::isRightToLeft() const {
    ສົ່ງຄືນ uloc_isRightToLeft(getBaseName());
}

U_NAMESPACE_END

namespace {
icu::CharString
GetRegionFromKey(const var_na* localeID, std::string_view key, UErrorCode& status) {
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
    ຖ້າ (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        const var_na region[] = {kw[0], kw[1], '\0'};
        ຖ້າ (valid.isSet(region)) {
            result.append(uprv_toupper(kw[0]), status);
            result.append(uprv_toupper(kw[1]), status);
        }
    }
    ສົ່ງຄືນ result;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_getRegionForSupplementalData(const var_na *localeID, bool inferRegion,
                                     UErrorCode& status) {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ {};
    }
    icu::CharString rgBuf = GetRegionFromKey(localeID, "rg", status);
    ຖ້າ (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        ຖ້າ (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = GetRegionFromKey(localeID, "sd", status);
            ຖ້າ (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                ຖ້າ (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    ສົ່ງຄືນ rgBuf;
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
    uprv_memcpy(map, gValidRegionMap, parimaa_na(map));
}

RegionValidateMap::~RegionValidateMap() {
}

bool RegionValidateMap::isSet(const var_na* region) const {
    int32_t index = value(region);
    ຖ້າ (index < 0) {
        ສົ່ງຄືນ false;
    }
    ສົ່ງຄືນ 0 != (map[index / 32] & (1L << (index % 32)));
}

bool RegionValidateMap::equals(const RegionValidateMap& that) const {
    ສົ່ງຄືນ uprv_memcmp(map, that.map, parimaa_na(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(const var_na* region) const {
    ຖ້າ (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        ສົ່ງຄືນ (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    ສົ່ງຄືນ -1;
}

U_NAMESPACE_END
