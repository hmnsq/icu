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
create_ٹیگ_String_With_Alternates(
    ثابت حرف* lang,
    int32_t lang_لمائی,
    ثابت حرف* script,
    int32_t script_لمائی,
    ثابت حرف* region,
    int32_t region_لمائی,
    ثابت حرف* variant,
    int32_t variant_لمائی,
    ثابت حرف* trailing,
    int32_t trailing_لمائی,
    icu::ByteSink& sink,
    UError_کوڈ& err) {
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    إذا (lang_لمائی >= ULOC_LANG_سمرتھا ||
            script_لمائی >= ULOC_SCRIPT_سمرتھا ||
            region_لمائی >= ULOC_COUNTRY_سمرتھا) {
        err = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    إذا (lang_لمائی > 0) {
        sink.Append(lang, lang_لمائی);
    }

    إذا (script_لمائی > 0) {
        sink.Append("_", 1);
        sink.Append(script, script_لمائی);
    }

    إذا (region_لمائی > 0) {
        sink.Append("_", 1);
        sink.Append(region, region_لمائی);
    }

    إذا (variant_لمائی > 0) {
        إذا (region_لمائی == 0) {
            /* extra separator is required */
            sink.Append("_", 1);
        }
        sink.Append("_", 1);
        sink.Append(variant, variant_لمائی);
    }

    إذا (trailing_لمائی > 0) {
        /*
         * Copy the trailing data into the supplied buffer.
         */
        sink.Append(trailing, trailing_لمائی);
    }
}

منطقي CHECK_TRAILING_VARIANT_ناپ(ثابت حرف* variant, int32_t variant_لمائی) {
    int32_t count = 0;
    لكل (int32_t i = 0; i < variant_لمائی; i++) {
        إذا (_isIDSeparator(variant[i])) {
            count = 0;
        } وإلا إذا (count == 8) {
            أرجع خطأ;
        } وإلا {
            count++;
        }
    }
    أرجع سچ;
}

فراغ
uloc_جوڑو_Likely_Subtags(ثابت حرف* localeID,
                       icu::ByteSink& sink,
                       UError_کوڈ& err) {
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    إذا (localeID == خالی_سوچک) {
        err = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ثابت حرف* trailing = خالی_سوچک;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    إذا (!CHECK_TRAILING_VARIANT_ناپ(variant.انکڑے(), variant.length())) {
        err = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    إذا (lang.length() == 4) {
        إذا (script.is_خالی_تھاواں()) {
            script = std::move(lang);
            lang.clear();
        } وإلا {
            err = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
    } وإلا إذا (lang.length() > 8) {
        err = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    int32_t trailing_لمائی = static_cast<int32_t>(uprv_strlen(trailing));

    ثابت icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    // We need to keep l on the stack because lsr may point into internal
    // memory of l.
    icu::Locale l = icu::Locale::createFromName(localeID);
    إذا (l.isBogus()) {
        err = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    icu::LSR lsr = likelySubtags->makeMaximizedLsrFrom(l, سچ, err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    ثابت حرف* language = lsr.language;
    إذا (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    create_ٹیگ_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.انکڑے(),
        variant.length(),
        trailing,
        trailing_لمائی,
        sink,
        err);
}

فراغ
_uloc_minimizeSubtags(ثابت حرف* localeID,
                      icu::ByteSink& sink,
                      منطقي favorScript,
                      UError_کوڈ& err) {
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    إذا (localeID == خالی_سوچک) {
        err = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    icu::CharString lang;
    icu::CharString script;
    icu::CharString region;
    icu::CharString variant;
    ثابت حرف* trailing = خالی_سوچک;
    ulocimp_getSubtags(localeID, &lang, &script, &region, &variant, &trailing, err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }

    إذا (!CHECK_TRAILING_VARIANT_ناپ(variant.انکڑے(), variant.length())) {
        err = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    int32_t trailing_لمائی = static_cast<int32_t>(uprv_strlen(trailing));

    ثابت icu::LikelySubtags* likelySubtags = icu::LikelySubtags::getSingleton(err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    icu::LSR lsr = likelySubtags->minimizeSubtags(
        lang.منزل_String_Piece(),
        script.منزل_String_Piece(),
        region.منزل_String_Piece(),
        favorScript,
        err);
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    ثابت حرف* language = lsr.language;
    إذا (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    create_ٹیگ_String_With_Alternates(
        language,
        static_cast<int32_t>(uprv_strlen(language)),
        lsr.script,
        static_cast<int32_t>(uprv_strlen(lsr.script)),
        lsr.region,
        static_cast<int32_t>(uprv_strlen(lsr.region)),
        variant.انکڑے(),
        variant.length(),
        trailing,
        trailing_لمائی,
        sink,
        err);
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_جوڑو_Likely_Subtags_2(ثابت حرف* localeID,
                      حرف* maximizedLocaleID,
                      int32_t maximizedLocaleIDCapacity,
                      UError_کوڈ* حالت) {
    أرجع icu::ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        maximizedLocaleID, maximizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_جوڑو_Likely_Subtags(localeID, sink, حالت);
        },
        *حالت);
}

U_EXPORT icu::CharString
ulocimp_جوڑو_Likely_Subtags(ثابت حرف* localeID,
                         UError_کوڈ& حالت) {
    أرجع icu::ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](icu::ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_جوڑو_Likely_Subtags(localeID, sink, حالت);
        },
        حالت);
}

U_EXPORT فراغ
ulocimp_جوڑو_Likely_Subtags(ثابت حرف* localeID,
                         icu::ByteSink& sink,
                         UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }
    إذا (localeID == خالی_سوچک) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_بفر = ulocimp_canonicalize(localeID, حالت);
    uloc_جوڑو_Likely_Subtags(locale_بفر.انکڑے(), sink, حالت);
}

U_CAPI int32_t U_EXPORT2
uloc_minimizeSubtags(ثابت حرف* localeID,
                     حرف* minimizedLocaleID,
                     int32_t minimizedLocaleIDCapacity,
                     UError_کوڈ* حالت) {
    أرجع icu::ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        minimizedLocaleID, minimizedLocaleIDCapacity,
        [&](icu::ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_minimizeSubtags(localeID, sink, خطأ, حالت);
        },
        *حالت);
}

U_EXPORT icu::CharString
ulocimp_minimizeSubtags(ثابت حرف* localeID,
                        منطقي favorScript,
                        UError_کوڈ& حالت) {
    أرجع icu::ByteSinkUtil::راہیں_Byte_Sink_منزل_Char_String(
        [&](icu::ByteSink& sink, UError_کوڈ& حالت) {
            ulocimp_minimizeSubtags(localeID, sink, favorScript, حالت);
        },
        حالت);
}

U_EXPORT فراغ
ulocimp_minimizeSubtags(ثابت حرف* localeID,
                        icu::ByteSink& sink,
                        منطقي favorScript,
                        UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }
    إذا (localeID == خالی_سوچک) {
        localeID = uloc_getDefault();
    }
    icu::CharString locale_بفر = ulocimp_canonicalize(localeID, حالت);
    _uloc_minimizeSubtags(locale_بفر.انکڑے(), sink, favorScript, حالت);
}

// Pairs of (language subtag, + or -) for finding out fast if common languages
// are LTR (minus) or RTL (plus).
ساكن ثابت حرف LANG_DIR_STRING[] =
        "root-en-es-pt-zh-ja-ko-de-fr-it-ar+he+fa+ru-nl-pl-th-tr-";

// Implemented here because this calls ulocimp_addLikelySubtags().
U_CAPI UBool U_EXPORT2
uloc_is_دایاں_منزل_بایاں(ثابت حرف *locale) {
    UError_کوڈ error_کوڈ = U_ZERO_ERROR;
    icu::CharString lang;
    icu::CharString script;
    ulocimp_getSubtags(
        locale == خالی_سوچک ? uloc_getDefault() : locale,
        &lang, &script, خالی_سوچک, خالی_سوچک, خالی_سوچک, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ) || script.is_خالی_تھاواں()) {
        // Fastpath: We know the likely scripts and their writing direction
        // for some common languages.
        إذا (!lang.is_خالی_تھاواں()) {
            ثابت حرف* langPtr = uprv_strstr(LANG_DIR_STRING, lang.انکڑے());
            إذا (langPtr != خالی_سوچک) {
                بدل (langPtr[lang.length()]) {
                حالة '-': أرجع خطأ;
                حالة '+': أرجع سچ;
                افتراضي: اكسر;  // partial match of a longer code
                }
            }
        }
        // Otherwise, find the likely script.
        error_کوڈ = U_ZERO_ERROR;
        icu::CharString likely = ulocimp_جوڑو_Likely_Subtags(locale, error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) {
            أرجع خطأ;
        }
        ulocimp_getSubtags(likely.منزل_String_Piece(), خالی_سوچک, &script, خالی_سوچک, خالی_سوچک, خالی_سوچک, error_کوڈ);
        إذا (U_FAILURE(error_کوڈ) || script.is_خالی_تھاواں()) {
            أرجع خطأ;
        }
    }
    UScript_کوڈ script_کوڈ = (UScript_کوڈ)u_get_Property_قدر_Enum(UCHAR_SCRIPT, script.انکڑے());
    أرجع uscript_is_دایاں_منزل_بایاں(script_کوڈ);
}

U_NAMESPACE_BEGIN

UBool
Locale::is_دایاں_منزل_بایاں() ثابت {
    أرجع uloc_is_دایاں_منزل_بایاں(get_ادھار_Name());
}

U_NAMESPACE_سرا

نطاق {
icu::CharString
Get_Region_From_کلید(ثابت حرف* localeID, std::string_view کلید, UError_کوڈ& حالت) {
    icu::CharString نتیجہ;
    // First check for keyword value
    icu::CharString kw = ulocimp_get_Keyword_قدر(localeID, کلید, حالت);
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
    إذا (U_SUCCESS(حالت) && len >= 3 && len <= 6 &&
        uprv_isASCIILetter(kw[0]) && uprv_isASCIILetter(kw[1])) {
        // Additional Check
        icu::RegionValidateMap valid;
        ثابت حرف region[] = {kw[0], kw[1], '\0'};
        إذا (valid.is_سیٹ(region)) {
            نتیجہ.append(uprv_toupper(kw[0]), حالت);
            نتیجہ.append(uprv_toupper(kw[1]), حالت);
        }
    }
    أرجع نتیجہ;
}
}  // namespace

U_EXPORT icu::CharString
ulocimp_get_Region_For_Supplemental_انکڑے(ثابت حرف *localeID, منطقي inferRegion,
                                     UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع {};
    }
    icu::CharString rgBuf = Get_Region_From_کلید(localeID, "rg", حالت);
    إذا (U_SUCCESS(حالت) && rgBuf.is_خالی_تھاواں()) {
        // No valid rg keyword value, try for unicode_region_subtag
        rgBuf = ulocimp_getRegion(localeID == خالی_سوچک ? uloc_getDefault() : localeID, حالت);
        إذا (U_SUCCESS(حالت) && rgBuf.is_خالی_تھاواں() && inferRegion) {
            // Second check for sd keyword value
            rgBuf = Get_Region_From_کلید(localeID, "sd", حالت);
            إذا (U_SUCCESS(حالت) && rgBuf.is_خالی_تھاواں()) {
                // no unicode_region_subtag but inferRegion true, try likely subtags
                UError_کوڈ rg_حالت = U_ZERO_ERROR;
                icu::CharString locBuf = ulocimp_جوڑو_Likely_Subtags(localeID, rg_حالت);
                إذا (U_SUCCESS(rg_حالت)) {
                    rgBuf = ulocimp_getRegion(locBuf.منزل_String_Piece(), حالت);
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

منطقي RegionValidateMap::is_سیٹ(ثابت حرف* region) ثابت {
    int32_t اشاریہ = value(region);
    إذا (اشاریہ < 0) {
        أرجع خطأ;
    }
    أرجع 0 != (map[اشاریہ / 32] & (1L << (اشاریہ % 32)));
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

U_NAMESPACE_سرا
