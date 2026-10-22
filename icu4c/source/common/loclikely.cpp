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

نامخلا {

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
خالی U_CALLCONV
create_لیبل_String_With_Alternates(
    ثابت حرف* lang,
    int32_t lang_لمبائی,
    ثابت حرف* script,
    int32_t script_لمبائی,
    ثابت حرف* region,
    int32_t region_لمبائی,
    ثابت حرف* variant,
    int32_t variant_لمبائی,
    ثابت حرف* trailing,
    int32_t trailing_لمبائی,
    icu::ByteSink& sink,
    UError_رمز& err) {
    اگر (U_FAILURE(err)) {
        واپس;
    }

    اگر (lang_لمبائی >= ULOC_LANG_استعداد ||
            script_لمبائی >= ULOC_SCRIPT_استعداد ||
            region_لمبائی >= ULOC_COUNTRY_استعداد) {
        err = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    اگر (lang_لمبائی > 0) {
        sink.Append(lang, lang_لمبائی);
    }

    اگر (script_لمبائی > 0) {
        sink.Append("_", 1);
        sink.Append(script, script_لمبائی);
    }

    اگر (region_لمبائی > 0) {
        sink.Append("_", 1);
        sink.Append(region, region_لمبائی);
    }

    اگر (variant_لمبائی > 0) {
        اگر (region_لمبائی == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variant_لمبائی);
    }

    اگر (trailing_لمبائی > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailing_لمبائی);
    }
}

منطقی CHECK_TRAILING_VARIANT_سائز(ثابت حرف* variant, int32_t variant_لمبائی) {
    int32_t count = 0;
    برائے (int32_t i = 0; i < variant_لمبائی; i++) {
        اگر (_isIDSeparator(variant[i])) {
            count = 0;
        } ورنہ اگر (count == 8) {
            واپس جھوٹ;
        } ورنہ {
            count++;
        }
    }
    واپس سچ;
}

خالی
uloc_جوڑو_Likely_Subtags(ثابت حرف* localeID,
                       icu::ByteSink& sink,
                       UError_رمز& err) {
    اگر (U_FAILURE(err)) {
        واپس;
    }

    اگر (localeID == خالی_اشاریہ) {
        err = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ثابت حرف* trailing = خالی_اشاریہ;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    اگر (U_FAILURE(err)) {
        واپس;
    }

    اگر (!CHECK_TRAILING_VARIANT_سائز(variant.ڈیٹا(), variant.length())) {
        err = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    اگر (lang.length() == 4) {
        اگر (script.is_خالی_خانے()) {
            script = std::move(lang);
            lang.clear();
        } ورنہ {
            err = U_ILLEGAL_دلیل_ERROR;
            واپس;
        }
    } ورنہ اگر (lang.length() > 8) {
        err = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    int32_t trailing_لمبائی = static_cast<int32_t>(uprv_strlen(trailing));

    ثابت icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    اگر (U_FAILURE(err)) {
        واپس;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    اگر (l.isBogus()) {
        err = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, سچ, err);
    اگر (U_FAILURE(err)) {
        واپس;
    }
    ثابت حرف* language = lsr.language;
    اگر (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    create_لیبل_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.ڈیٹا(),
        variant.length(),
        trailing,
        trailing_لمبائی,
        sink,
        err);
}

خالی
_uloc_minimizeSubtags(ثابت حرف* localeID,
                      icu::ByteSink& sink,
                      منطقی favorScript,
                      UError_رمز& err) {
    اگر (U_FAILURE(err)) {
        واپس;
    }

    اگر (localeID == خالی_اشاریہ) {
        err = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ثابت حرف* trailing = خالی_اشاریہ;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    اگر (U_FAILURE(err)) {
        واپس;
    }

    اگر (!CHECK_TRAILING_VARIANT_سائز(variant.ڈیٹا(), variant.length())) {
        err = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    int32_t trailing_لمبائی = static_cast<int32_t>(uprv_strlen(trailing));

    ثابت icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    اگر (U_FAILURE(err)) {
        واپس;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.مقصد_String_Piece(),
        script.مقصد_String_Piece(),
        region.مقصد_String_Piece(),
        favorScript,
        err);
    اگر (U_FAILURE(err)) {
        واپس;
    }
    ثابت حرف* language = lsr.language;
    اگر (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    create_لیبل_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.ڈیٹا(),
        variant.length(),
        trailing,
        trailing_لمبائی,
        sink,
        err);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_جوڑو_Likely_Subtags_2(ثابت حرف* localeID,
                      حرف* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UError_رمز* حالت_2) {
    واپس icu::ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_جوڑو_Likely_Subtags(localeID, sink, حالت_2);
        },
        *حالت_2);
}

U_EXPORT icu::CharString
ulocimp_جوڑو_Likely_Subtags(ثابت حرف* localeID,
                         UError_رمز& حالت_2) {
    واپس icu::ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](icu::ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_جوڑو_Likely_Subtags(localeID, sink, حالت_2);
        },
        حالت_2);
}

U_EXPORT خالی
ulocimp_جوڑو_Likely_Subtags(ثابت حرف* localeID,
                         icu::ByteSink& sink,
                         UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) { واپس; }
    اگر (localeID == خالی_اشاریہ) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_بفر = ulocimp_canonicalize(localeID, حالت_2);
    uloc_جوڑو_Likely_Subtags(locale_بفر.ڈیٹا(), sink, حالت_2);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(ثابت حرف* localeID,
                     حرف* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UError_رمز* حالت_2) {
    واپس icu::ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_minimizeSubtags(localeID, sink, جھوٹ, حالت_2);
        },
        *حالت_2);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(ثابت حرف* localeID,
                        منطقی favorScript,
                        UError_رمز& حالت_2) {
    واپس icu::ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Char_String(
        [&](icu::ByteSink& sink, UError_رمز& حالت_2) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, حالت_2);
        },
        حالت_2);
}

U_EXPORT خالی
ulocimp_minimizeSubtags(ثابت حرف* localeID,
                        icu::ByteSink& sink,
                        منطقی favorScript,
                        UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) { واپس; }
    اگر (localeID == خالی_اشاریہ) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_بفر = ulocimp_canonicalize(localeID, حالت_2);
    _uloc_minimizeSubtags(locale_بفر.ڈیٹا(), sink, favorScript, حالت_2);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
ساکن ثابت حرف LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_دایاں_مقصد_بایاں(ثابت حرف *locale) {
    UError_رمز error_رمز = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == خالی_اشاریہ ? uloc_getDefault() : locale,
        &lang, &script, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, error_رمز);
    اگر (U_FAILURE(error_رمز) || script.is_خالی_خانے()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        اگر (!lang.is_خالی_خانے()) {
            ثابت حرف* langPtr = uprv_strstr(LANG_DIR_STRING, lang.ڈیٹا());
            اگر (langPtr != خالی_اشاریہ) {
                انتخاب (langPtr[lang.length()]) {
                حالت '-': واپس جھوٹ;
                حالت '+': واپس سچ;
                طےشدہ: توڑو;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        error_رمز = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_جوڑو_Likely_Subtags(locale, error_رمز);
        اگر (U_FAILURE(error_رمز)) {
            واپس جھوٹ;
        }
        ulocimp_getSubtags(likely.مقصد_String_Piece(), خالی_اشاریہ, &script, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, error_رمز);
        اگر (U_FAILURE(error_رمز) || script.is_خالی_خانے()) {
            واپس جھوٹ;
        }
    }
    UScript_رمز script_رمز = (UScript_رمز)u_get_Property_قدر_Enum(UCHAR_SCRIPT, script.ڈیٹا());
    واپس uscript_is_دایاں_مقصد_بایاں(script_رمز);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_دایاں_مقصد_بایاں() ثابت {
    واپس uloc_is_دایاں_مقصد_بایاں(get_بنیاد_Name());
}

U_NAMESPACE_انتہا

نامخلا {
icu::CharString
Get_Region_From_کلید(ثابت حرف* localeID, std::string_view کلید, UError_رمز& حالت_2) {
    icu::CharString نتیجہ;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_قدر(localeID, کلید, حالت_2);
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
    اگر (U_SUCCESS(حالت_2) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        ثابت حرف region[] = {kw[0], kw[1], '\0'};
        اگر (valid.is_سیٹ(region)) {
            نتیجہ.append(uprv_toupper(kw[0]), حالت_2);
            نتیجہ.append(uprv_toupper(kw[1]), حالت_2);
        }
    }
    واپس نتیجہ;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_get_Region_For_Supplemental_ڈیٹا(ثابت حرف *localeID, منطقی inferRegion,
                                     UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس {};
    }
    icu::CharString rgBuf = Get_Region_From_کلید(localeID, "rg", حالت_2);
    اگر (U_SUCCESS(حالت_2) && rgBuf.is_خالی_خانے()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == خالی_اشاریہ ? uloc_getDefault() : localeID, حالت_2);
        اگر (U_SUCCESS(حالت_2) && rgBuf.is_خالی_خانے() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_کلید(localeID, "sd", حالت_2);
            اگر (U_SUCCESS(حالت_2) && rgBuf.is_خالی_خانے()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UError_رمز rg_حالت = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_جوڑو_Likely_Subtags(localeID, rg_حالت);
                اگر (U_SUCCESS(rg_حالت)) {
                    rgBuf = ulocimp_getRegion(locBuf.مقصد_String_Piece(), حالت_2);
                }
            }
        }
    }

    واپس rgBuf;
}

نامخلا {

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

منطقی RegionValidateMap::is_سیٹ(ثابت حرف* region) ثابت {
    int32_t اشاریہ = value(region);
    اگر (اشاریہ < 0) {
        واپس جھوٹ;
    }
    واپس 0 != (map[اشاریہ / 32] & (1L << (اشاریہ % 32)));
}

منطقی RegionValidateMap::equals(ثابت RegionValidateMap& that) ثابت {
    واپس uprv_memcmp(map, that.map, حجم(map)) == 0;
}

// The code transform two letter a-z to a integer valued between -1, 26x26.
// -1 indicate the region is outside the range of two letter a-z
// the rest of value is between 0 and 676 (= 26x26) and used as an index
// the bigmap in map. The map is an array of 22 int32_t.
// since 32x21 < 676/32 < 32x22 we store this 676 bits bitmap into 22 int32_t.
int32_t RegionValidateMap::value(ثابت حرف* region) ثابت {
    اگر (uprv_isASCIILetter(region[0]) && uprv_isASCIILetter(region[1]) &&
        region[2] == '\0') {
        واپس (uprv_toupper(region[0])-'A') * 26 +
               (uprv_toupper(region[1])-'A');
    }
    واپس -1;
}

U_NAMESPACE_انتہا
