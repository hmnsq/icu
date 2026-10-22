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

#અન્તર્ભાવ <string_view>
#અન્તર્ભાવ <utility>

#અન્તર્ભાવ "unicode/bytestream.h"
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/uloc.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "unicode/uscript.h"
#અન્તર્ભાવ "bytesinkutil.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "loclikelysubtags.h"
#અન્તર્ભાવ "ulocimp.h"

નામાકાશ {

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
શૂન્ય U_CALLCONV
createTagStringWithAlternates(
    અચલ અક્ષર* lang,
    int32_t langLength,
    અચલ અક્ષર* script,
    int32_t scriptLength,
    અચલ અક્ષર* region,
    int32_t regionLength,
    અચલ અક્ષર* variant,
    int32_t variantLength,
    અચલ અક્ષર* trailing,
    int32_t trailingLength,
    icu::ByteSink& sink,
    UErrorCode& err) {
    જો (U_FAILURE(err)) {
        પાછા;
    }

    જો (langLength >= ULOC_LANG_CAPACITY ||
            scriptLength >= ULOC_SCRIPT_CAPACITY ||
            regionLength >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    જો (langLength > 0) {
        sink.Append(lang, langLength);
    }

    જો (scriptLength > 0) {
        sink.Append("_", 1);
        sink.Append(script, scriptLength);
    }

    જો (regionLength > 0) {
        sink.Append("_", 1);
        sink.Append(region, regionLength);
    }

    જો (variantLength > 0) {
        જો (regionLength == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variantLength);
    }

    જો (trailingLength > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailingLength);
    }
}

તાર્કિક CHECK_TRAILING_VARIANT_SIZE(અચલ અક્ષર* variant, int32_t variantLength) {
    int32_t count = 0;
    માટે (int32_t i = 0; i < variantLength; i++) {
        જો (_isIDSeparator(variant[i])) {
            count = 0;
        } નહીંતર જો (count == 8) {
            પાછા ખોટું;
        } નહીંતર {
            count++;
        }
    }
    પાછા સાચું;
}

શૂન્ય
_uloc_addLikelySubtags(અચલ અક્ષર* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    જો (U_FAILURE(err)) {
        પાછા;
    }

    જો (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    અચલ અક્ષર* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    જો (U_FAILURE(err)) {
        પાછા;
    }

    જો (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    જો (lang.length() == 4) {
        જો (script.isEmpty()) {
            script = std::move(lang);
            lang.clear();
        } નહીંતર {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા;
        }
    } નહીંતર જો (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    અચલ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    જો (U_FAILURE(err)) {
        પાછા;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    જો (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, સાચું, err);
    જો (U_FAILURE(err)) {
        પાછા;
    }
    અચલ અક્ષર* language = lsr.language;
    જો (uprv_strcmp(language, "und") == 0) {
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

શૂન્ય
_uloc_minimizeSubtags(અચલ અક્ષર* localeID,
                      icu::ByteSink& sink,
                      તાર્કિક favorScript,
                      UErrorCode& err) {
    જો (U_FAILURE(err)) {
        પાછા;
    }

    જો (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    અચલ અક્ષર* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    જો (U_FAILURE(err)) {
        પાછા;
    }

    જો (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    અચલ icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    જો (U_FAILURE(err)) {
        પાછા;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    જો (U_FAILURE(err)) {
        પાછા;
    }
    અચલ અક્ષર* language = lsr.language;
    જો (uprv_strcmp(language, "und") == 0) {
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
uloc_addLikelySubtags(અચલ અક્ષર* localeID,
                      અક્ષર* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    પાછા icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(અચલ અક્ષર* localeID,
                         UErrorCode& status) {
    પાછા icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT શૂન્ય
ulocimp_addLikelySubtags(અચલ અક્ષર* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    જો (U_FAILURE(status)) { પાછા; }
    જો (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(localeBuffer.data(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(અચલ અક્ષર* localeID,
                     અક્ષર* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    પાછા icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, ખોટું, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(અચલ અક્ષર* localeID,
                        તાર્કિક favorScript,
                        UErrorCode& status) {
    પાછા icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT શૂન્ય
ulocimp_minimizeSubtags(અચલ અક્ષર* localeID,
                        icu::ByteSink& sink,
                        તાર્કિક favorScript,
                        UErrorCode& status) {
    જો (U_FAILURE(status)) { પાછા; }
    જો (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(localeBuffer.data(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
સ્થિર અચલ અક્ષર LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_જમણું_To_ડાબું(અચલ અક્ષર *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    જો (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        જો (!lang.isEmpty()) {
            અચલ અક્ષર* langPtr = uprv_strstr(LANG_DIR_STRING, lang.data());
            જો (langPtr != nullptr) {
                પસંદગી (langPtr[lang.length()]) {
                સ્થિતિ '-': પાછા ખોટું;
                સ્થિતિ '+': પાછા સાચું;
                મૂળભૂત: તોડો;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        જો (U_FAILURE(errorCode)) {
            પાછા ખોટું;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        જો (U_FAILURE(errorCode) || script.isEmpty()) {
            પાછા ખોટું;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_get_Property_કિંમત_Enum(UCHAR_SCRIPT, script.data());
    પાછા uscript_is_જમણું_To_ડાબું(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_જમણું_To_ડાબું() અચલ {
    પાછા uloc_is_જમણું_To_ડાબું(getBaseName());
}

U_NAMESPACE_END

નામાકાશ {
icu::CharString
Get_Region_From_ચાવી(અચલ અક્ષર* localeID, std::string_view ચાવી, UErrorCode& status) {
    icu::CharString result;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_કિંમત(localeID, ચાવી, status);
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
    જો (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        અચલ અક્ષર region[] = {kw[0], kw[1], '\0'};
        જો (valid.isSet(region)) {
            result.append(uprv_toupper(kw[0]), status);
            result.append(uprv_toupper(kw[1]), status);
        }
    }
    પાછા result;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_getRegionForSupplementalData(અચલ અક્ષર *localeID, તાર્કિક inferRegion,
                                     UErrorCode& status) {
    જો (U_FAILURE(status)) {
        પાછા {};
    }
    icu::CharString rgBuf = Get_Region_From_ચાવી(localeID, "rg", status);
    જો (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        જો (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_ચાવી(localeID, "sd", status);
            જો (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                જો (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    પાછા rgBuf;
}

નામાકાશ {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
અચલ uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, કદ(map));
}

RegionValidateMap::~RegionValidateMap() {
}

તાર્કિક RegionValidateMap::isSet(અચલ અક્ષર* region) અચલ {
    int32_t index = value(region);
    જો (index < 0) {
        પાછા ખોટું;
    }
    પાછા 0 != (map[index / 32] & (1L << (index % 32)));
}

તાર્કિક RegionValidateMap::equals(અચલ RegionValidateMap& that) અચલ {
    પાછા uprv_memcmp(map, that.map, કદ(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(અચલ અક્ષર* region) અચલ {
    જો (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        પાછા (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    પાછા -1;
}

U_NAMESPACE_END
