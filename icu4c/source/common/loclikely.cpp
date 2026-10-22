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

#అన్తర్భావ <string_view>
#అన్తర్భావ <utility>

#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "loclikelysubtags.h"
#అన్తర్భావ "ulocimp.h"

నామాకాశ {

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
శూన్య U_CALLCONV
create_ట్యాగ్_String_With_Alternates(
    స్థిరరాశి అక్షరం* lang,
    int32_t lang_నిడివి,
    స్థిరరాశి అక్షరం* script,
    int32_t script_నిడివి,
    స్థిరరాశి అక్షరం* region,
    int32_t region_నిడివి,
    స్థిరరాశి అక్షరం* variant,
    int32_t variant_నిడివి,
    స్థిరరాశి అక్షరం* trailing,
    int32_t trailing_నిడివి,
    icu::ByteSink& sink,
    UError_సంకేతం& err) {
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }

    ఒకవేళ (lang_నిడివి >= ULOC_LANG_సామర్థ్యం ||
            script_నిడివి >= ULOC_SCRIPT_సామర్థ్యం ||
            region_నిడివి >= ULOC_COUNTRY_సామర్థ్యం) {
        err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    ఒకవేళ (lang_నిడివి > 0) {
        sink.Append(lang, lang_నిడివి);
    }

    ఒకవేళ (script_నిడివి > 0) {
        sink.Append("_", 1);
        sink.Append(script, script_నిడివి);
    }

    ఒకవేళ (region_నిడివి > 0) {
        sink.Append("_", 1);
        sink.Append(region, region_నిడివి);
    }

    ఒకవేళ (variant_నిడివి > 0) {
        ఒకవేళ (region_నిడివి == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variant_నిడివి);
    }

    ఒకవేళ (trailing_నిడివి > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailing_నిడివి);
    }
}

తార్కిక CHECK_TRAILING_VARIANT_కొలత(స్థిరరాశి అక్షరం* variant, int32_t variant_నిడివి) {
    int32_t count = 0;
    కొరకు (int32_t i = 0; i < variant_నిడివి; i++) {
        ఒకవేళ (_isIDSeparator(variant[i])) {
            count = 0;
        } లేకపోతే ఒకవేళ (count == 8) {
            తిరిగి అబద్ధం;
        } లేకపోతే {
            count++;
        }
    }
    తిరిగి నిజం;
}

శూన్య
uloc_కూడు_Likely_Subtags(స్థిరరాశి అక్షరం* localeID,
                       icu::ByteSink& sink,
                       UError_సంకేతం& err) {
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }

    ఒకవేళ (localeID == శూన్యసూచిక) {
        err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    స్థిరరాశి అక్షరం* trailing = శూన్యసూచిక;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }

    ఒకవేళ (!CHECK_TRAILING_VARIANT_కొలత(variant.దత్తాంశం(), variant.length())) {
        err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    ఒకవేళ (lang.length() == 4) {
        ఒకవేళ (script.is_ఖాళీలు()) {
            script = std::move(lang);
            lang.clear();
        } లేకపోతే {
            err = U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }
    } లేకపోతే ఒకవేళ (lang.length() > 8) {
        err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    int32_t trailing_నిడివి = static_cast<int32_t>(uprv_strlen(trailing));

    స్థిరరాశి icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    ఒకవేళ (l.isBogus()) {
        err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, నిజం, err);
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }
    స్థిరరాశి అక్షరం* language = lsr.language;
    ఒకవేళ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    create_ట్యాగ్_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.దత్తాంశం(),
        variant.length(),
        trailing,
        trailing_నిడివి,
        sink,
        err);
}

శూన్య
_uloc_minimizeSubtags(స్థిరరాశి అక్షరం* localeID,
                      icu::ByteSink& sink,
                      తార్కిక favorScript,
                      UError_సంకేతం& err) {
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }

    ఒకవేళ (localeID == శూన్యసూచిక) {
        err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    స్థిరరాశి అక్షరం* trailing = శూన్యసూచిక;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }

    ఒకవేళ (!CHECK_TRAILING_VARIANT_కొలత(variant.దత్తాంశం(), variant.length())) {
        err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    int32_t trailing_నిడివి = static_cast<int32_t>(uprv_strlen(trailing));

    స్థిరరాశి icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.గమ్యం_String_Piece(),
        script.గమ్యం_String_Piece(),
        region.గమ్యం_String_Piece(),
        favorScript,
        err);
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }
    స్థిరరాశి అక్షరం* language = lsr.language;
    ఒకవేళ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    create_ట్యాగ్_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.దత్తాంశం(),
        variant.length(),
        trailing,
        trailing_నిడివి,
        sink,
        err);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_కూడు_Likely_Subtags_2(స్థిరరాశి అక్షరం* localeID,
                      అక్షరం* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UError_సంకేతం* స్థితి_2) {
    తిరిగి icu::ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_కూడు_Likely_Subtags(localeID, sink, స్థితి_2);
        },
        *స్థితి_2);
}

U_EXPORT icu::CharString
ulocimp_కూడు_Likely_Subtags(స్థిరరాశి అక్షరం* localeID,
                         UError_సంకేతం& స్థితి_2) {
    తిరిగి icu::ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](icu::ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_కూడు_Likely_Subtags(localeID, sink, స్థితి_2);
        },
        స్థితి_2);
}

U_EXPORT శూన్య
ulocimp_కూడు_Likely_Subtags(స్థిరరాశి అక్షరం* localeID,
                         icu::ByteSink& sink,
                         UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
    ఒకవేళ (localeID == శూన్యసూచిక) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_బఫరు = ulocimp_canonicalize(localeID, స్థితి_2);
    uloc_కూడు_Likely_Subtags(locale_బఫరు.దత్తాంశం(), sink, స్థితి_2);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(స్థిరరాశి అక్షరం* localeID,
                     అక్షరం* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UError_సంకేతం* స్థితి_2) {
    తిరిగి icu::ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_minimizeSubtags(localeID, sink, అబద్ధం, స్థితి_2);
        },
        *స్థితి_2);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(స్థిరరాశి అక్షరం* localeID,
                        తార్కిక favorScript,
                        UError_సంకేతం& స్థితి_2) {
    తిరిగి icu::ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Char_String(
        [&](icu::ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, స్థితి_2);
        },
        స్థితి_2);
}

U_EXPORT శూన్య
ulocimp_minimizeSubtags(స్థిరరాశి అక్షరం* localeID,
                        icu::ByteSink& sink,
                        తార్కిక favorScript,
                        UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
    ఒకవేళ (localeID == శూన్యసూచిక) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_బఫరు = ulocimp_canonicalize(localeID, స్థితి_2);
    _uloc_minimizeSubtags(locale_బఫరు.దత్తాంశం(), sink, favorScript, స్థితి_2);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
స్థిర స్థిరరాశి అక్షరం LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_కుడి_గమ్యం_ఎడమ(స్థిరరాశి అక్షరం *locale) {
    UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == శూన్యసూచిక ? uloc_getDefault() : locale,
        &lang, &script, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం) || script.is_ఖాళీలు()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        ఒకవేళ (!lang.is_ఖాళీలు()) {
            స్థిరరాశి అక్షరం* langPtr = uprv_strstr(LANG_DIR_STRING, lang.దత్తాంశం());
            ఒకవేళ (langPtr != శూన్యసూచిక) {
                ఎంపిక (langPtr[lang.length()]) {
                స్థితి '-': తిరిగి అబద్ధం;
                స్థితి '+': తిరిగి నిజం;
                అప్రమేయ: ఆపు;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        error_సంకేతం = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_కూడు_Likely_Subtags(locale, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) {
            తిరిగి అబద్ధం;
        }
        ulocimp_getSubtags(likely.గమ్యం_String_Piece(), శూన్యసూచిక, &script, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం) || script.is_ఖాళీలు()) {
            తిరిగి అబద్ధం;
        }
    }
    UScript_సంకేతం script_సంకేతం = (UScript_సంకేతం)u_get_Property_విలువ_Enum(UCHAR_SCRIPT, script.దత్తాంశం());
    తిరిగి uscript_is_కుడి_గమ్యం_ఎడమ(script_సంకేతం);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_కుడి_గమ్యం_ఎడమ() స్థిరరాశి {
    తిరిగి uloc_is_కుడి_గమ్యం_ఎడమ(get_పునాది_Name());
}

U_NAMESPACE_చివర

నామాకాశ {
icu::CharString
Get_Region_From_కీలకం(స్థిరరాశి అక్షరం* localeID, std::string_view కీలకం, UError_సంకేతం& స్థితి_2) {
    icu::CharString ఫలితం;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_విలువ(localeID, కీలకం, స్థితి_2);
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
    ఒకవేళ (U_SUCCESS(స్థితి_2) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        స్థిరరాశి అక్షరం region[] = {kw[0], kw[1], '\0'};
        ఒకవేళ (valid.is_సమితి(region)) {
            ఫలితం.append(uprv_toupper(kw[0]), స్థితి_2);
            ఫలితం.append(uprv_toupper(kw[1]), స్థితి_2);
        }
    }
    తిరిగి ఫలితం;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_get_Region_For_Supplemental_దత్తాంశం(స్థిరరాశి అక్షరం *localeID, తార్కిక inferRegion,
                                     UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి {};
    }
    icu::CharString rgBuf = Get_Region_From_కీలకం(localeID, "rg", స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2) && rgBuf.is_ఖాళీలు()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == శూన్యసూచిక ? uloc_getDefault() : localeID, స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2) && rgBuf.is_ఖాళీలు() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_కీలకం(localeID, "sd", స్థితి_2);
            ఒకవేళ (U_SUCCESS(స్థితి_2) && rgBuf.is_ఖాళీలు()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UError_సంకేతం rg_స్థితి = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_కూడు_Likely_Subtags(localeID, rg_స్థితి);
                ఒకవేళ (U_SUCCESS(rg_స్థితి)) {
                    rgBuf = ulocimp_getRegion(locBuf.గమ్యం_String_Piece(), స్థితి_2);
                }
            }
        }
    }

    తిరిగి rgBuf;
}

నామాకాశ {

// The following data is generated by unit test code inside
// test/intltest/regiontst.cpp from the resource data while
// the test failed.
స్థిరరాశి uint32_t gValidRegionMap[] = {
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
    uprv_memcpy(map, gValidRegionMap, పరిమాణం(map));
}

RegionValidateMap::~RegionValidateMap() {
}

తార్కిక RegionValidateMap::is_సమితి(స్థిరరాశి అక్షరం* region) స్థిరరాశి {
    int32_t సూచీ = value(region);
    ఒకవేళ (సూచీ < 0) {
        తిరిగి అబద్ధం;
    }
    తిరిగి 0 != (map[సూచీ / 32] & (1L << (సూచీ % 32)));
}

తార్కిక RegionValidateMap::equals(స్థిరరాశి RegionValidateMap& that) స్థిరరాశి {
    తిరిగి uprv_memcmp(map, that.map, పరిమాణం(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(స్థిరరాశి అక్షరం* region) స్థిరరాశి {
    ఒకవేళ (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        తిరిగి (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    తిరిగి -1;
}

U_NAMESPACE_చివర
