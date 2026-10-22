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

#अन्तर्भाव <string_view>
#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "loclikelysubtags.h"
#अन्तर्भाव "ulocimp.h"

नामाकाश {

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
शून्य U_CALLCONV
createTagStringWithAlternates(
    ध्रुव वर्ण* lang,
    int32_t langLength,
    ध्रुव वर्ण* script,
    int32_t scriptLength,
    ध्रुव वर्ण* region,
    int32_t regionLength,
    ध्रुव वर्ण* variant,
    int32_t variantLength,
    ध्रुव वर्ण* trailing,
    int32_t trailingLength,
    icu::ByteSink& sink,
    UErrorCode& err) {
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }

    यदि (langLength >= ULOC_LANG_CAPACITY ||
            scriptLength >= ULOC_SCRIPT_CAPACITY ||
            regionLength >= ULOC_COUNTRY_CAPACITY) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    यदि (langLength > 0) {
        sink.Append(lang, langLength);
    }

    यदि (scriptLength > 0) {
        sink.Append("_", 1);
        sink.Append(script, scriptLength);
    }

    यदि (regionLength > 0) {
        sink.Append("_", 1);
        sink.Append(region, regionLength);
    }

    यदि (variantLength > 0) {
        यदि (regionLength == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variantLength);
    }

    यदि (trailingLength > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailingLength);
    }
}

तार्किक CHECK_TRAILING_VARIANT_SIZE(ध्रुव वर्ण* variant, int32_t variantLength) {
    int32_t count = 0;
    प्रति (int32_t i = 0; i < variantLength; i++) {
        यदि (_isIDSeparator(variant[i])) {
            count = 0;
        } अन्यथा यदि (count == 8) {
            प्रत्यागम असत्य;
        } अन्यथा {
            count++;
        }
    }
    प्रत्यागम सत्य;
}

शून्य
_uloc_addLikelySubtags(ध्रुव वर्ण* localeID,
                       icu::ByteSink& sink,
                       UErrorCode& err) {
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }

    यदि (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ध्रुव वर्ण* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }

    यदि (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    यदि (lang.length() == 4) {
        यदि (script.isEmpty()) {
            script = std::move(lang);
            lang.clear();
        } अन्यथा {
            err = U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम;
        }
    } अन्यथा यदि (lang.length() > 8) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ध्रुव icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    यदि (l.isBogus()) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, सत्य, err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }
    ध्रुव वर्ण* language = lsr.language;
    यदि (uprv_strcmp(language, "und") == 0) {
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

शून्य
_uloc_minimizeSubtags(ध्रुव वर्ण* localeID,
                      icu::ByteSink& sink,
                      तार्किक favorScript,
                      UErrorCode& err) {
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }

    यदि (localeID == nullptr) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ध्रुव वर्ण* trailing = nullptr;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }

    यदि (!CHECK_TRAILING_VARIANT_SIZE(variant.data(), variant.length())) {
        err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    int32_t trailingLength = static_cast<int32_t>(uprv_strlen(trailing));

    ध्रुव icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.toStringPiece(),
        script.toStringPiece(),
        region.toStringPiece(),
        favorScript,
        err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }
    ध्रुव वर्ण* language = lsr.language;
    यदि (uprv_strcmp(language, "und") == 0) {
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
uloc_addLikelySubtags(ध्रुव वर्ण* localeID,
                      वर्ण* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UErrorCode* status) {
    प्रत्यागम icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_addLikelySubtags(ध्रुव वर्ण* localeID,
                         UErrorCode& status) {
    प्रत्यागम icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_addLikelySubtags(localeID, sink, status);
        },
        status);
}

U_EXPORT शून्य
ulocimp_addLikelySubtags(ध्रुव वर्ण* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& status) {
    यदि (U_FAILURE(status)) { प्रत्यागम; }
    यदि (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_addLikelySubtags(localeBuffer.data(), sink, status);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(ध्रुव वर्ण* localeID,
                     वर्ण* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UErrorCode* status) {
    प्रत्यागम icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, असत्य, status);
        },
        *status);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(ध्रुव वर्ण* localeID,
                        तार्किक favorScript,
                        UErrorCode& status) {
    प्रत्यागम icu::ByteSinkUtil::viaByteSinkToCharString(
        [&](icu::ByteSink& sink, UErrorCode& status) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, status);
        },
        status);
}

U_EXPORT शून्य
ulocimp_minimizeSubtags(ध्रुव वर्ण* localeID,
                        icu::ByteSink& sink,
                        तार्किक favorScript,
                        UErrorCode& status) {
    यदि (U_FAILURE(status)) { प्रत्यागम; }
    यदि (localeID == nullptr) {
        localeID = uloc_getDefault();
    }
    icu::CharString localeBuffer = ulocimp_canonicalize(localeID, status);
    _uloc_minimizeSubtags(localeBuffer.data(), sink, favorScript, status);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
स्थिर ध्रुव वर्ण LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_उजवा_To_डावा(ध्रुव वर्ण *locale) {
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == nullptr ? uloc_getDefault() : locale,
        &lang, &script, nullptr, nullptr, nullptr, errorCode);
    यदि (U_FAILURE(errorCode) || script.isEmpty()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        यदि (!lang.isEmpty()) {
            ध्रुव वर्ण* langPtr = uprv_strstr(LANG_DIR_STRING, lang.data());
            यदि (langPtr != nullptr) {
                विकल्प (langPtr[lang.length()]) {
                पक्ष '-': प्रत्यागम असत्य;
                पक्ष '+': प्रत्यागम सत्य;
                स्वभाव: भङ्ग;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        errorCode = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_addLikelySubtags(locale, errorCode);
        यदि (U_FAILURE(errorCode)) {
            प्रत्यागम असत्य;
        }
        ulocimp_getSubtags(likely.toStringPiece(), nullptr, &script, nullptr, nullptr, nullptr, errorCode);
        यदि (U_FAILURE(errorCode) || script.isEmpty()) {
            प्रत्यागम असत्य;
        }
    }
    UScriptCode scriptCode = (UScriptCode)u_get_Property_मूल्य_Enum(UCHAR_SCRIPT, script.data());
    प्रत्यागम uscript_is_उजवा_To_डावा(scriptCode);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_उजवा_To_डावा() ध्रुव {
    प्रत्यागम uloc_is_उजवा_To_डावा(getBaseName());
}

U_NAMESPACE_END

नामाकाश {
icu::CharString
Get_Region_From_किल्ली(ध्रुव वर्ण* localeID, std::string_view किल्ली, UErrorCode& status) {
    icu::CharString result;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_मूल्य(localeID, किल्ली, status);
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
    यदि (U_SUCCESS(status) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        ध्रुव वर्ण region[] = {kw[0], kw[1], '\0'};
        यदि (valid.isSet(region)) {
            result.append(uprv_toupper(kw[0]), status);
            result.append(uprv_toupper(kw[1]), status);
        }
    }
    प्रत्यागम result;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_getRegionForSupplementalData(ध्रुव वर्ण *localeID, तार्किक inferRegion,
                                     UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        प्रत्यागम {};
    }
    icu::CharString rgBuf = Get_Region_From_किल्ली(localeID, "rg", status);
    यदि (U_SUCCESS(status) && rgBuf.isEmpty()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == nullptr ? uloc_getDefault() : localeID, status);
        यदि (U_SUCCESS(status) && rgBuf.isEmpty() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_किल्ली(localeID, "sd", status);
            यदि (U_SUCCESS(status) && rgBuf.isEmpty()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UErrorCode rgStatus = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_addLikelySubtags(localeID, rgStatus);
                यदि (U_SUCCESS(rgStatus)) {
                    rgBuf = ulocimp_getRegion(locBuf.toStringPiece(), status);
                }
            }
        }
    }

    प्रत्यागम rgBuf;
}

नामाकाश {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
ध्रुव uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, परिमाण(map));
}

RegionValidateMap::~RegionValidateMap() {
}

तार्किक RegionValidateMap::isSet(ध्रुव वर्ण* region) ध्रुव {
    int32_t index = value(region);
    यदि (index < 0) {
        प्रत्यागम असत्य;
    }
    प्रत्यागम 0 != (map[index / 32] & (1L << (index % 32)));
}

तार्किक RegionValidateMap::equals(ध्रुव RegionValidateMap& that) ध्रुव {
    प्रत्यागम uprv_memcmp(map, that.map, परिमाण(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(ध्रुव वर्ण* region) ध्रुव {
    यदि (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        प्रत्यागम (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    प्रत्यागम -1;
}

U_NAMESPACE_END
