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

#समावेश <string_view>
#समावेश <utility>

#समावेश "unicode/bytestream.h"
#समावेश "unicode/utypes.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/putil.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/uloc.h"
#समावेश "unicode/ures.h"
#समावेश "unicode/uscript.h"
#समावेश "bytesinkutil.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "loclikelysubtags.h"
#समावेश "ulocimp.h"

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
व्योम U_CALLCONV
createTagStringWithAlternates(
    स्थिर अक्षर* lang,
    int32_t lang_लंबाई,
    स्थिर अक्षर* script,
    int32_t script_लंबाई,
    स्थिर अक्षर* region,
    int32_t region_लंबाई,
    स्थिर अक्षर* variant,
    int32_t variant_लंबाई,
    स्थिर अक्षर* trailing,
    int32_t trailing_लंबाई,
    icu::ByteSink& sink,
    UErrorCode& err) {
    अगर (U_FAILURE(err)) {
        वापस;
    }

    अगर (lang_लंबाई >= ULOC_LANG_CAPACITY ||
            script_लंबाई >= ULOC_SCRIPT_CAPACITY ||
            region_लंबाई >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    अगर (lang_लंबाई > 0) {
        sink.Append(lang, lang_लंबाई);
    }

    अगर (script_लंबाई > 0) {
        sink.Append("_", 1);
        sink.Append(script, script_लंबाई);
    }

    अगर (region_लंबाई > 0) {
        sink.Append("_", 1);
        sink.Append(region, region_लंबाई);
    }

    अगर (variant_लंबाई > 0) {
        अगर (region_लंबाई == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variant_लंबाई);
    }

    अगर (trailing_लंबाई > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailing_लंबाई);
    }
}

bool CHECK_TRAILING_VARIANT_आकार(स्थिर अक्षर* variant, int32_t variant_लंबाई) {
    int32_t count = 0;
    क्रम (int32_t i = 0; i < variant_लंबाई; i++) {
        अगर (_isIDSeparator(variant[i])) {
            count = 0;
        } अन्यथा अगर (count == 8) {
            वापस false;
        } अन्यथा {
            count++;
        }
    }
    वापस true;
}

व्योम
_uloc_addLikelySubtags(स्थिर अक्षर* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    अगर (U_FAILURE(err)) {
        वापस;
    }

    अगर (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    स्थिर अक्षर* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    अगर (U_FAILURE(err)) {
        वापस;
    }

    अगर (!CHECK_TRAILING_VARIANT_आकार(variant.तथ्य(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    अगर (lang.length() == 4) {
        अगर (script.isEmpty()) {
            script = std::move(lang);
            lang.सफा();
        } अन्यथा {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }
    } अन्यथा अगर (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    int32_t trailing_लंबाई = static_cast<int32_t>(uprv_strlen(trailing));

    स्थिर icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    अगर (U_FAILURE(err)) {
        वापस;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    अगर (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, true, err);
    अगर (U_FAILURE(err)) {
        वापस;
    }
    स्थिर अक्षर* language = lsr.language;
    अगर (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    createTagStringWithAlternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.तथ्य(),
        variant.length(),
        trailing,
        trailing_लंबाई,
        sink,
        err);
}

व्योम
_uloc_minimizeSubtags(स्थिर अक्षर* localeID,
                      icu::ByteSink& sink,
                      bool favorScript,
                      UErrorCode& err) {
    अगर (U_FAILURE(err)) {
        वापस;
    }

    अगर (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    स्थिर अक्षर* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    अगर (U_FAILURE(err)) {
        वापस;
    }

    अगर (!CHECK_TRAILING_VARIANT_आकार(variant.तथ्य(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    int32_t trailing_लंबाई = static_cast<int32_t>(uprv_strlen(trailing));

    स्थिर icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    अगर (U_FAILURE(err)) {
        वापस;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    अगर (U_FAILURE(err)) {
        वापस;
    }
    स्थिर अक्षर* language = lsr.language;
    अगर (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    createTagStringWithAlternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.तथ्य(),
        variant.length(),
        trailing,
        trailing_लंबाई,
        sink,
        err);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_addLikelySubtags(स्थिर अक्षर* localeID,
                      अक्षर* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    वापस icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(स्थिर अक्षर* localeID,
                         UErrorCode& status) {
    वापस icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT व्योम
ulocimp_addLikelySubtags(स्थिर अक्षर* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    अगर (U_FAILURE(status)) { वापस; }
    अगर (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_बफर = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(locale_बफर.तथ्य(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(स्थिर अक्षर* localeID,
                     अक्षर* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    वापस icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, false, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(स्थिर अक्षर* localeID,
                        bool favorScript,
                        UErrorCode& status) {
    वापस icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT व्योम
ulocimp_minimizeSubtags(स्थिर अक्षर* localeID,
                        icu::ByteSink& sink,
                        bool favorScript,
                        UErrorCode& status) {
    अगर (U_FAILURE(status)) { वापस; }
    अगर (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_बफर = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(locale_बफर.तथ्य(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
अटल स्थिर अक्षर LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_दायाँ_To_बायाँ(स्थिर अक्षर *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    अगर (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        अगर (!lang.isEmpty()) {
            स्थिर अक्षर* langPtr = uprv_strstr(LANG_DIR_STRING, lang.तथ्य());
            अगर (langPtr != nullptr) {
                चयन (langPtr[lang.length()]) {
                हाल '-': वापस false;
                हाल '+': वापस true;
                शेष: अवरोध;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        अगर (U_FAILURE(errorCode)) {
            वापस false;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        अगर (U_FAILURE(errorCode) || script.isEmpty()) {
            वापस false;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_get_Property_मूल्य_Enum(UCHAR_SCRIPT, script.तथ्य());
    वापस uscript_is_दायाँ_To_बायाँ(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_दायाँ_To_बायाँ() स्थिर {
    वापस uloc_is_दायाँ_To_बायाँ(getBaseName());
}

U_NAMESPACE_END

namespace {
icu::CharString
Get_Region_From_कुंजी(स्थिर अक्षर* localeID, std::string_view कुंजी, UErrorCode& status) {
    icu::CharString परिणाम;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_मूल्य(localeID, कुंजी, status);
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
    अगर (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        स्थिर अक्षर region[] = {kw[0], kw[1], '\0'};
        अगर (valid.isSet(region)) {
            परिणाम.append(uprv_toupper(kw[0]), status);
            परिणाम.append(uprv_toupper(kw[1]), status);
        }
    }
    वापस परिणाम;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_get_Region_For_Supplemental_तथ्य(स्थिर अक्षर *localeID, bool inferRegion,
                                     UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस {};
    }
    icu::CharString rgBuf = Get_Region_From_कुंजी(localeID, "rg", status);
    अगर (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        अगर (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_कुंजी(localeID, "sd", status);
            अगर (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                अगर (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    वापस rgBuf;
}

namespace {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
स्थिर uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, माप(map));
}

RegionValidateMap::~RegionValidateMap() {
}

bool RegionValidateMap::isSet(स्थिर अक्षर* region) स्थिर {
    int32_t सूचकांक = value(region);
    अगर (सूचकांक < 0) {
        वापस false;
    }
    वापस 0 != (map[सूचकांक / 32] & (1L << (सूचकांक % 32)));
}

bool RegionValidateMap::equals(स्थिर RegionValidateMap& that) स्थिर {
    वापस uprv_memcmp(map, that.map, माप(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(स्थिर अक्षर* region) स्थिर {
    अगर (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        वापस (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    वापस -1;
}

U_NAMESPACE_END
