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
create_चिह्नकम्_String_With_Alternates(
    ध्रुव वर्ण* lang,
    int32_t lang_दीर्घता,
    ध्रुव वर्ण* script,
    int32_t script_दीर्घता,
    ध्रुव वर्ण* region,
    int32_t region_दीर्घता,
    ध्रुव वर्ण* variant,
    int32_t variant_दीर्घता,
    ध्रुव वर्ण* trailing,
    int32_t trailing_दीर्घता,
    icu::ByteSink& sink,
    UError_संहिता& err) {
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }

    यदि (lang_दीर्घता >= ULOC_LANG_क्षमता ||
            script_दीर्घता >= ULOC_SCRIPT_क्षमता ||
            region_दीर्घता >= ULOC_COUNTRY_क्षमता) {
        err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    यदि (lang_दीर्घता > 0) {
        sink.Append(lang, lang_दीर्घता);
    }

    यदि (script_दीर्घता > 0) {
        sink.Append("_", 1);
        sink.Append(script, script_दीर्घता);
    }

    यदि (region_दीर्घता > 0) {
        sink.Append("_", 1);
        sink.Append(region, region_दीर्घता);
    }

    यदि (variant_दीर्घता > 0) {
        यदि (region_दीर्घता == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variant_दीर्घता);
    }

    यदि (trailing_दीर्घता > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailing_दीर्घता);
    }
}

तार्किक CHECK_TRAILING_VARIANT_आकार(ध्रुव वर्ण* variant, int32_t variant_दीर्घता) {
    int32_t count = 0;
    प्रति (int32_t i = 0; i < variant_दीर्घता; i++) {
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
uloc_सङ्कलय_Likely_Subtags(ध्रुव वर्ण* localeID,
                       icu::ByteSink& sink,
                       UError_संहिता& err) {
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }

    यदि (localeID == शून्यसूचकः) {
        err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ध्रुव वर्ण* trailing = शून्यसूचकः;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }

    यदि (!CHECK_TRAILING_VARIANT_आकार(variant.दत्त(), variant.length())) {
        err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    यदि (lang.length() == 4) {
        यदि (script.is_रिक्तानि()) {
            script = std::move(lang);
            lang.clear();
        } अन्यथा {
            err = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }
    } अन्यथा यदि (lang.length() > 8) {
        err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    int32_t trailing_दीर्घता = static_cast<int32_t>(uprv_strlen(trailing));

    ध्रुव icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    यदि (l.isBogus()) {
        err = U_ILLEGAL_तर्कः_ERROR;
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
    create_चिह्नकम्_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.दत्त(),
        variant.length(),
        trailing,
        trailing_दीर्घता,
        sink,
        err);
}

शून्य
_uloc_minimizeSubtags(ध्रुव वर्ण* localeID,
                      icu::ByteSink& sink,
                      तार्किक favorScript,
                      UError_संहिता& err) {
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }

    यदि (localeID == शून्यसूचकः) {
        err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ध्रुव वर्ण* trailing = शून्यसूचकः;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }

    यदि (!CHECK_TRAILING_VARIANT_आकार(variant.दत्त(), variant.length())) {
        err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    int32_t trailing_दीर्घता = static_cast<int32_t>(uprv_strlen(trailing));

    ध्रुव icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.गन्तव्यम्_String_Piece(),
        script.गन्तव्यम्_String_Piece(),
        region.गन्तव्यम्_String_Piece(),
        favorScript,
        err);
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }
    ध्रुव वर्ण* language = lsr.language;
    यदि (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    create_चिह्नकम्_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.दत्त(),
        variant.length(),
        trailing,
        trailing_दीर्घता,
        sink,
        err);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_सङ्कलय_Likely_Subtags_2(ध्रुव वर्ण* localeID,
                      वर्ण* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UError_संहिता* स्थितिः) {
    प्रत्यागम icu::ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_सङ्कलय_Likely_Subtags(localeID, sink, स्थितिः);
        },
        *स्थितिः);
}

U_EXPORT icu::CharString
ulocimp_सङ्कलय_Likely_Subtags(ध्रुव वर्ण* localeID,
                         UError_संहिता& स्थितिः) {
    प्रत्यागम icu::ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](icu::ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_सङ्कलय_Likely_Subtags(localeID, sink, स्थितिः);
        },
        स्थितिः);
}

U_EXPORT शून्य
ulocimp_सङ्कलय_Likely_Subtags(ध्रुव वर्ण* localeID,
                         icu::ByteSink& sink,
                         UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
    यदि (localeID == शून्यसूचकः) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_बफर = ulocimp_canonicalize(localeID, स्थितिः);
    uloc_सङ्कलय_Likely_Subtags(locale_बफर.दत्त(), sink, स्थितिः);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(ध्रुव वर्ण* localeID,
                     वर्ण* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UError_संहिता* स्थितिः) {
    प्रत्यागम icu::ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_minimizeSubtags(localeID, sink, असत्य, स्थितिः);
        },
        *स्थितिः);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(ध्रुव वर्ण* localeID,
                        तार्किक favorScript,
                        UError_संहिता& स्थितिः) {
    प्रत्यागम icu::ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Char_String(
        [&](icu::ByteSink& sink, UError_संहिता& स्थितिः) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, स्थितिः);
        },
        स्थितिः);
}

U_EXPORT शून्य
ulocimp_minimizeSubtags(ध्रुव वर्ण* localeID,
                        icu::ByteSink& sink,
                        तार्किक favorScript,
                        UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
    यदि (localeID == शून्यसूचकः) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_बफर = ulocimp_canonicalize(localeID, स्थितिः);
    _uloc_minimizeSubtags(locale_बफर.दत्त(), sink, favorScript, स्थितिः);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
स्थिर ध्रुव वर्ण LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_दक्षिणः_गन्तव्यम्_वामः(ध्रुव वर्ण *locale) {
    UError_संहिता error_संहिता = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == शून्यसूचकः ? uloc_getDefault() : locale,
        &lang, &script, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, error_संहिता);
    यदि (U_FAILURE(error_संहिता) || script.is_रिक्तानि()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        यदि (!lang.is_रिक्तानि()) {
            ध्रुव वर्ण* langPtr = uprv_strstr(LANG_DIR_STRING, lang.दत्त());
            यदि (langPtr != शून्यसूचकः) {
                विकल्प (langPtr[lang.length()]) {
                पक्ष '-': प्रत्यागम असत्य;
                पक्ष '+': प्रत्यागम सत्य;
                स्वभाव: भङ्ग;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        error_संहिता = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_सङ्कलय_Likely_Subtags(locale, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) {
            प्रत्यागम असत्य;
        }
        ulocimp_getSubtags(likely.गन्तव्यम्_String_Piece(), शून्यसूचकः, &script, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, error_संहिता);
        यदि (U_FAILURE(error_संहिता) || script.is_रिक्तानि()) {
            प्रत्यागम असत्य;
        }
    }
    UScript_संहिता script_संहिता = (UScript_संहिता)u_get_Property_मानम्_Enum(UCHAR_SCRIPT, script.दत्त());
    प्रत्यागम uscript_is_दक्षिणः_गन्तव्यम्_वामः(script_संहिता);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_दक्षिणः_गन्तव्यम्_वामः() ध्रुव {
    प्रत्यागम uloc_is_दक्षिणः_गन्तव्यम्_वामः(get_आधारः_Name());
}

U_NAMESPACE_अन्तः

नामाकाश {
icu::CharString
Get_Region_From_कुञ्चिका(ध्रुव वर्ण* localeID, std::string_view कुञ्चिका, UError_संहिता& स्थितिः) {
    icu::CharString परिणाम;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_मानम्(localeID, कुञ्चिका, स्थितिः);
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
    यदि (U_SUCCESS(स्थितिः) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        ध्रुव वर्ण region[] = {kw[0], kw[1], '\0'};
        यदि (valid.is_समुच्चयः(region)) {
            परिणाम.append(uprv_toupper(kw[0]), स्थितिः);
            परिणाम.append(uprv_toupper(kw[1]), स्थितिः);
        }
    }
    प्रत्यागम परिणाम;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_get_Region_For_Supplemental_दत्त(ध्रुव वर्ण *localeID, तार्किक inferRegion,
                                     UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम {};
    }
    icu::CharString rgBuf = Get_Region_From_कुञ्चिका(localeID, "rg", स्थितिः);
    यदि (U_SUCCESS(स्थितिः) && rgBuf.is_रिक्तानि()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == शून्यसूचकः ? uloc_getDefault() : localeID, स्थितिः);
        यदि (U_SUCCESS(स्थितिः) && rgBuf.is_रिक्तानि() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_कुञ्चिका(localeID, "sd", स्थितिः);
            यदि (U_SUCCESS(स्थितिः) && rgBuf.is_रिक्तानि()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UError_संहिता rg_स्थितिः = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_सङ्कलय_Likely_Subtags(localeID, rg_स्थितिः);
                यदि (U_SUCCESS(rg_स्थितिः)) {
                    rgBuf = ulocimp_getRegion(locBuf.गन्तव्यम्_String_Piece(), स्थितिः);
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

तार्किक RegionValidateMap::is_समुच्चयः(ध्रुव वर्ण* region) ध्रुव {
    int32_t सूचकांक = value(region);
    यदि (सूचकांक < 0) {
        प्रत्यागम असत्य;
    }
    प्रत्यागम 0 != (map[सूचकांक / 32] & (1L << (सूचकांक % 32)));
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

U_NAMESPACE_अन्तः
