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

#അന്തര്ഭാവ <string_view>
#അന്തര്ഭാവ <utility>

#അന്തര്ഭാവ "unicode/bytestream.h"
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "unicode/uscript.h"
#അന്തര്ഭാവ "bytesinkutil.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "loclikelysubtags.h"
#അന്തര്ഭാവ "ulocimp.h"

നാമാകാശ {

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
ശൂന്യം U_CALLCONV
createTagStringWithAlternates(
    സ്ഥിരം അക്ഷരം* lang,
    int32_t langLength,
    സ്ഥിരം അക്ഷരം* script,
    int32_t scriptLength,
    സ്ഥിരം അക്ഷരം* region,
    int32_t regionLength,
    സ്ഥിരം അക്ഷരം* variant,
    int32_t variantLength,
    സ്ഥിരം അക്ഷരം* trailing,
    int32_t trailingLength,
    icu::ByteSink& sink,
    UErrorCode& err) {
    എങ്കിൽ (U_FAILURE(err)) {
        തിരികെ;
    }

    എങ്കിൽ (langLength >= ULOC_LANG_CAPACITY ||
            scriptLength >= ULOC_SCRIPT_CAPACITY ||
            regionLength >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    എങ്കിൽ (langLength > 0) {
        sink.Append(lang, langLength);
    }

    എങ്കിൽ (scriptLength > 0) {
        sink.Append("_", 1);
        sink.Append(script, scriptLength);
    }

    എങ്കിൽ (regionLength > 0) {
        sink.Append("_", 1);
        sink.Append(region, regionLength);
    }

    എങ്കിൽ (variantLength > 0) {
        എങ്കിൽ (regionLength == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variantLength);
    }

    എങ്കിൽ (trailingLength > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailingLength);
    }
}

തർക്കം CHECK_TRAILING_VARIANT_SIZE(സ്ഥിരം അക്ഷരം* variant, int32_t variantLength) {
    int32_t count = 0;
    വേണ്ടി (int32_t i = 0; i < variantLength; i++) {
        എങ്കിൽ (_isIDSeparator(variant[i])) {
            count = 0;
        } അല്ലെങ്കിൽ എങ്കിൽ (count == 8) {
            തിരികെ അസത്യം;
        } അല്ലെങ്കിൽ {
            count++;
        }
    }
    തിരികെ സത്യം;
}

ശൂന്യം
_uloc_addLikelySubtags(സ്ഥിരം അക്ഷരം* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    എങ്കിൽ (U_FAILURE(err)) {
        തിരികെ;
    }

    എങ്കിൽ (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    സ്ഥിരം അക്ഷരം* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    എങ്കിൽ (U_FAILURE(err)) {
        തിരികെ;
    }

    എങ്കിൽ (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    എങ്കിൽ (lang.length() == 4) {
        എങ്കിൽ (script.isEmpty()) {
            script = std::move(lang);
            lang.clear();
        } അല്ലെങ്കിൽ {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    സ്ഥിരം icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    എങ്കിൽ (U_FAILURE(err)) {
        തിരികെ;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    എങ്കിൽ (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, സത്യം, err);
    എങ്കിൽ (U_FAILURE(err)) {
        തിരികെ;
    }
    സ്ഥിരം അക്ഷരം* language = lsr.language;
    എങ്കിൽ (uprv_strcmp(language, "und") == 0) {
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

ശൂന്യം
_uloc_minimizeSubtags(സ്ഥിരം അക്ഷരം* localeID,
                      icu::ByteSink& sink,
                      തർക്കം favorScript,
                      UErrorCode& err) {
    എങ്കിൽ (U_FAILURE(err)) {
        തിരികെ;
    }

    എങ്കിൽ (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    സ്ഥിരം അക്ഷരം* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    എങ്കിൽ (U_FAILURE(err)) {
        തിരികെ;
    }

    എങ്കിൽ (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    സ്ഥിരം icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    എങ്കിൽ (U_FAILURE(err)) {
        തിരികെ;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    എങ്കിൽ (U_FAILURE(err)) {
        തിരികെ;
    }
    സ്ഥിരം അക്ഷരം* language = lsr.language;
    എങ്കിൽ (uprv_strcmp(language, "und") == 0) {
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
uloc_addLikelySubtags(സ്ഥിരം അക്ഷരം* localeID,
                      അക്ഷരം* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    തിരികെ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(സ്ഥിരം അക്ഷരം* localeID,
                         UErrorCode& status) {
    തിരികെ icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT ശൂന്യം
ulocimp_addLikelySubtags(സ്ഥിരം അക്ഷരം* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
    എങ്കിൽ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(localeBuffer.data(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(സ്ഥിരം അക്ഷരം* localeID,
                     അക്ഷരം* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    തിരികെ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, അസത്യം, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(സ്ഥിരം അക്ഷരം* localeID,
                        തർക്കം favorScript,
                        UErrorCode& status) {
    തിരികെ icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT ശൂന്യം
ulocimp_minimizeSubtags(സ്ഥിരം അക്ഷരം* localeID,
                        icu::ByteSink& sink,
                        തർക്കം favorScript,
                        UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
    എങ്കിൽ (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(localeBuffer.data(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
സ്ഥിതം സ്ഥിരം അക്ഷരം LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_വലത്_To_ഇടത്(സ്ഥിരം അക്ഷരം *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        എങ്കിൽ (!lang.isEmpty()) {
            സ്ഥിരം അക്ഷരം* langPtr = uprv_strstr(LANG_DIR_STRING, lang.data());
            എങ്കിൽ (langPtr != nullptr) {
                തിരഞ്ഞെടുപ്പ് (langPtr[lang.length()]) {
                അവസ്ഥ '-': തിരികെ അസത്യം;
                അവസ്ഥ '+': തിരികെ സത്യം;
                സ്ഥിരസ്ഥിതി: നിർത്തുക;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) {
            തിരികെ അസത്യം;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode) || script.isEmpty()) {
            തിരികെ അസത്യം;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_get_Property_മൂല്യം_Enum(UCHAR_SCRIPT, script.data());
    തിരികെ uscript_is_വലത്_To_ഇടത്(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_വലത്_To_ഇടത്() സ്ഥിരം {
    തിരികെ uloc_is_വലത്_To_ഇടത്(getBaseName());
}

U_NAMESPACE_END

നാമാകാശ {
icu::CharString
Get_Region_From_താക്കോൽ(സ്ഥിരം അക്ഷരം* localeID, std::string_view താക്കോൽ, UErrorCode& status) {
    icu::CharString result;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_മൂല്യം(localeID, താക്കോൽ, status);
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
    എങ്കിൽ (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        സ്ഥിരം അക്ഷരം region[] = {kw[0], kw[1], '\0'};
        എങ്കിൽ (valid.isSet(region)) {
            result.append(uprv_toupper(kw[0]), status);
            result.append(uprv_toupper(kw[1]), status);
        }
    }
    തിരികെ result;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_getRegionForSupplementalData(സ്ഥിരം അക്ഷരം *localeID, തർക്കം inferRegion,
                                     UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ {};
    }
    icu::CharString rgBuf = Get_Region_From_താക്കോൽ(localeID, "rg", status);
    എങ്കിൽ (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        എങ്കിൽ (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_താക്കോൽ(localeID, "sd", status);
            എങ്കിൽ (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                എങ്കിൽ (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    തിരികെ rgBuf;
}

നാമാകാശ {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
സ്ഥിരം uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, വലുപ്പം(map));
}

RegionValidateMap::~RegionValidateMap() {
}

തർക്കം RegionValidateMap::isSet(സ്ഥിരം അക്ഷരം* region) സ്ഥിരം {
    int32_t index = value(region);
    എങ്കിൽ (index < 0) {
        തിരികെ അസത്യം;
    }
    തിരികെ 0 != (map[index / 32] & (1L << (index % 32)));
}

തർക്കം RegionValidateMap::equals(സ്ഥിരം RegionValidateMap& that) സ്ഥിരം {
    തിരികെ uprv_memcmp(map, that.map, വലുപ്പം(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(സ്ഥിരം അക്ഷരം* region) സ്ഥിരം {
    എങ്കിൽ (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        തിരികെ (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    തിരികെ -1;
}

U_NAMESPACE_END
