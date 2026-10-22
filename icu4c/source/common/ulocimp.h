// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2004-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#నిర్వచితం_కాకపోతే ULOCIMP_H
#నిర్వచించు ULOCIMP_H

#అన్తర్భావ <cstddef>
#అన్తర్భావ <optional>
#అన్తర్భావ <string_view>

#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/uloc.h"

#అన్తర్భావ "charstr.h"

/**
 * Create an iterator over the specified keywords list
 * @param keywordList double-null terminated list. Will be copied.
 * @param keywordListSize size in bytes of keywordList
 * @param status err code
 * @return enumeration (owned by caller) of the keyword list.
 * @internal ICU 3.0
 */
U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(స్థిరరాశి అక్షరం *keywordList, int32_t keyword_List_కొలత, UError_సంకేతం* స్థితి_2);

/**
 * Look up a resource bundle table item with fallback on the table level.
 * This is accessible so it can be called by C++ code.
 */
U_CAPI స్థిరరాశి UChar * U_EXPORT2
uloc_get_పట్టిక_String_With_Fallback(
    స్థిరరాశి అక్షరం *మార్గం,
    స్థిరరాశి అక్షరం *locale,
    స్థిరరాశి అక్షరం *పట్టిక_కీలకం,
    స్థిరరాశి అక్షరం *sub_పట్టిక_కీలకం,
    స్థిరరాశి అక్షరం *అంశం_కీలకం,
    int32_t *p_నిడివి,
    UError_సంకేతం *p_Error_సంకేతం);

namespace {
/*returns true if a is an ID separator false otherwise*/
అంతఃవరుస bool _isIDSeparator(అక్షరం a) { తిరిగి a == '_' || a == '-'; }
}  // namespace

U_CFUNC స్థిరరాశి అక్షరం* 
uloc_getCurrentCountryID(స్థిరరాశి అక్షరం* oldID U_LIFETIME_BOUND);

U_CFUNC స్థిరరాశి అక్షరం* 
uloc_getCurrentLanguageID(స్థిరరాశి అక్షరం* oldID U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_గమ్యం_Bcp_కీలకం_With_Fallback(std::string_view keyword U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_గమ్యం_Bcp_Type_With_Fallback(std::string_view keyword, std::string_view విలువ_2 U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_గమ్యం_Legacy_కీలకం_With_Fallback(std::string_view keyword U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_గమ్యం_Legacy_Type_With_Fallback(std::string_view keyword, std::string_view విలువ_2 U_LIFETIME_BOUND);

U_COMMON_API icu::CharString
ulocimp_getKeywords(std::string_view localeID,
                    అక్షరం prev,
                    bool విలువలు_Too,
                    UError_సంకేతం& స్థితి_2);

U_COMMON_API శూన్య
ulocimp_getKeywords(std::string_view localeID,
                    అక్షరం prev,
                    icu::ByteSink& sink,
                    bool విలువలు_Too,
                    UError_సంకేతం& స్థితి_2);

U_COMMON_API icu::CharString
ulocimp_getName(std::string_view localeID,
                UError_సంకేతం& err);

U_COMMON_API శూన్య
ulocimp_getName(std::string_view localeID,
                icu::ByteSink& sink,
                UError_సంకేతం& err);

U_COMMON_API icu::CharString
ulocimp_get_పునాది_Name(std::string_view localeID,
                    UError_సంకేతం& err);

U_COMMON_API శూన్య
ulocimp_get_పునాది_Name(std::string_view localeID,
                    icu::ByteSink& sink,
                    UError_సంకేతం& err);

U_COMMON_API icu::CharString
ulocimp_canonicalize(std::string_view localeID,
                     UError_సంకేతం& err);

U_COMMON_API శూన్య
ulocimp_canonicalize(std::string_view localeID,
                     icu::ByteSink& sink,
                     UError_సంకేతం& err);

U_COMMON_API icu::CharString
ulocimp_get_Keyword_విలువ(స్థిరరాశి అక్షరం* localeID,
                        std::string_view keywordName,
                        UError_సంకేతం& స్థితి_2);

U_COMMON_API శూన్య
ulocimp_get_Keyword_విలువ(స్థిరరాశి అక్షరం* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UError_సంకేతం& స్థితి_2);

U_COMMON_API icu::CharString
ulocimp_getLanguage(std::string_view localeID, UError_సంకేతం& స్థితి_2);

U_COMMON_API icu::CharString
ulocimp_getScript(std::string_view localeID, UError_సంకేతం& స్థితి_2);

U_COMMON_API icu::CharString
ulocimp_getRegion(std::string_view localeID, UError_సంకేతం& స్థితి_2);

U_COMMON_API icu::CharString
ulocimp_getVariant(std::string_view localeID, UError_సంకేతం& స్థితి_2);

U_COMMON_API శూన్య
ulocimp_సమితి_Keyword_విలువ(std::string_view keywordName,
                        std::string_view keyword_విలువ,
                        icu::CharString& localeID,
                        UError_సంకేతం& స్థితి_2);

U_COMMON_API int32_t
ulocimp_సమితి_Keyword_విలువ(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_విలువ,
                        icu::ByteSink& sink,
                        UError_సంకేతం& స్థితి_2);

U_COMMON_API శూన్య
ulocimp_getSubtags(
        std::string_view localeID,
        icu::CharString* language,
        icu::CharString* script,
        icu::CharString* region,
        icu::CharString* variant,
        స్థిరరాశి అక్షరం** p_చివర,
        UError_సంకేతం& స్థితి_2);

U_COMMON_API శూన్య
ulocimp_getSubtags(
        std::string_view localeID,
        icu::ByteSink* language,
        icu::ByteSink* script,
        icu::ByteSink* region,
        icu::ByteSink* variant,
        స్థిరరాశి అక్షరం** p_చివర,
        UError_సంకేతం& స్థితి_2);

అంతఃవరుస శూన్య
ulocimp_getSubtags(
        std::string_view localeID,
        std::nullptr_t,
        std::nullptr_t,
        std::nullptr_t,
        std::nullptr_t,
        స్థిరరాశి అక్షరం** p_చివర,
        UError_సంకేతం& స్థితి_2) {
    ulocimp_getSubtags(
            localeID,
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            p_చివర,
            స్థితి_2);
}

U_COMMON_API icu::CharString
ulocimp_get_మాతృ(స్థిరరాశి అక్షరం* localeID,
                  UError_సంకేతం& err);

U_COMMON_API శూన్య
ulocimp_get_మాతృ(స్థిరరాశి అక్షరం* localeID,
                  icu::ByteSink& sink,
                  UError_సంకేతం& err);

U_COMMON_API icu::CharString
ulocimp_గమ్యం_Language_ట్యాగ్(స్థిరరాశి అక్షరం* localeID,
                      bool strict,
                      UError_సంకేతం& స్థితి_2);

/**
 * Writes a well-formed language tag for this locale ID.
 *
 * **Note**: When `strict` is false, any locale fields which do not satisfy the
 * BCP47 syntax requirement will be omitted from the result.  When `strict` is
 * true, this function sets U_ILLEGAL_ARGUMENT_ERROR to the `err` if any locale
 * fields do not satisfy the BCP47 syntax requirement.
 *
 * @param localeID  the input locale ID
 * @param sink      the output sink receiving the BCP47 language
 *                  tag for this Locale.
 * @param strict    boolean value indicating if the function returns
 *                  an error for an ill-formed input locale ID.
 * @param err       error information if receiving the language
 *                  tag failed.
 * @return          The length of the BCP47 language tag.
 *
 * @internal ICU 64
 */
U_COMMON_API శూన్య
ulocimp_గమ్యం_Language_ట్యాగ్(స్థిరరాశి అక్షరం* localeID,
                      icu::ByteSink& sink,
                      bool strict,
                      UError_సంకేతం& err);

U_COMMON_API icu::CharString
ulocimp_for_Language_ట్యాగ్(స్థిరరాశి అక్షరం* langtag,
                       int32_t ట్యాగ్_Len,
                       int32_t* parsed_నిడివి,
                       UError_సంకేతం& స్థితి_2);

/**
 * Returns a locale ID for the specified BCP47 language tag string.
 * If the specified language tag contains any ill-formed subtags,
 * the first such subtag and all following subtags are ignored.
 * <p>
 * This implements the 'Language-Tag' production of BCP 47, and so
 * supports legacy language tags (marked as “Type: grandfathered” in BCP 47)
 * (regular and irregular) as well as private use language tags.
 *
 * Private use tags are represented as 'x-whatever',
 * and legacy tags are converted to their canonical replacements where they exist.
 *
 * Note that a few legacy tags have no modern replacement;
 * these will be converted using the fallback described in
 * the first paragraph, so some information might be lost.
 *
 * @param langtag   the input BCP47 language tag.
 * @param tagLen    the length of langtag, or -1 to call uprv_strlen().
 * @param sink      the output sink receiving a locale ID for the
 *                  specified BCP47 language tag.
 * @param parsedLength  if not NULL, successfully parsed length
 *                      for the input language tag is set.
 * @param err       error information if receiving the locald ID
 *                  failed.
 * @internal ICU 63
 */
U_COMMON_API శూన్య
ulocimp_for_Language_ట్యాగ్(స్థిరరాశి అక్షరం* langtag,
                       int32_t ట్యాగ్_Len,
                       icu::ByteSink& sink,
                       int32_t* parsed_నిడివి,
                       UError_సంకేతం& err);

/**
 * Get the region to use for supplemental data lookup. Uses
 * (1) any region specified by locale tag "rg"; if none then
 * (2) any unicode_region_tag in the locale ID; if none then
 * (3) if inferRegion is true, the region suggested by
 * getLikelySubtags on the localeID.
 * If no region is found, returns an empty string.
 *
 * @param localeID
 *     The complete locale ID (with keywords) from which
 *     to get the region to use for supplemental data.
 * @param inferRegion
 *     If true, will try to infer region from localeID if
 *     no other region is found.
 * @param status
 *     Pointer to in/out UErrorCode value for latest status.
 * @return
 *     The region code found, empty if none found.
 * @internal ICU 57
 */
U_COMMON_API icu::CharString
ulocimp_get_Region_For_Supplemental_దత్తాంశం(స్థిరరాశి అక్షరం *localeID, bool inferRegion,
                                     UError_సంకేతం& స్థితి_2);

U_COMMON_API icu::CharString
ulocimp_కూడు_Likely_Subtags(స్థిరరాశి అక్షరం* localeID,
                         UError_సంకేతం& స్థితి_2);

/**
 * Add the likely subtags for a provided locale ID, per the algorithm described
 * in the following CLDR technical report:
 *
 *   http://www.unicode.org/reports/tr35/#Likely_Subtags
 *
 * If localeID is already in the maximal form, or there is no data available
 * for maximization, it will be copied to the output buffer.  For example,
 * "und-Zzzz" cannot be maximized, since there is no reasonable maximization.
 *
 * Examples:
 *
 * "en" maximizes to "en_Latn_US"
 *
 * "de" maximizes to "de_Latn_US"
 *
 * "sr" maximizes to "sr_Cyrl_RS"
 *
 * "sh" maximizes to "sr_Latn_RS" (Note this will not reverse.)
 *
 * "zh_Hani" maximizes to "zh_Hans_CN" (Note this will not reverse.)
 *
 * @param localeID The locale to maximize
 * @param sink The output sink receiving the maximized locale
 * @param err Error information if maximizing the locale failed.  If the length
 * of the localeID and the null-terminator is greater than the maximum allowed size,
 * or the localeId is not well-formed, the error code is U_ILLEGAL_ARGUMENT_ERROR.
 * @internal ICU 64
 */
U_COMMON_API శూన్య
ulocimp_కూడు_Likely_Subtags(స్థిరరాశి అక్షరం* localeID,
                         icu::ByteSink& sink,
                         UError_సంకేతం& err);

U_COMMON_API icu::CharString
ulocimp_minimizeSubtags(స్థిరరాశి అక్షరం* localeID,
                        bool favorScript,
                        UError_సంకేతం& స్థితి_2);

/**
 * Minimize the subtags for a provided locale ID, per the algorithm described
 * in the following CLDR technical report:
 *
 *   http://www.unicode.org/reports/tr35/#Likely_Subtags
 *
 * If localeID is already in the minimal form, or there is no data available
 * for minimization, it will be copied to the output buffer.  Since the
 * minimization algorithm relies on proper maximization, see the comments
 * for ulocimp_addLikelySubtags for reasons why there might not be any data.
 *
 * Examples:
 *
 * "en_Latn_US" minimizes to "en"
 *
 * "de_Latn_US" minimizes to "de"
 *
 * "sr_Cyrl_RS" minimizes to "sr"
 *
 * "zh_Hant_TW" minimizes to "zh_TW" (The region is preferred to the
 * script, and minimizing to "zh" would imply "zh_Hans_CN".)
 *
 * @param localeID The locale to minimize
 * @param sink The output sink receiving the maximized locale
 * @param favorScript favor to keep script if true, region if false.
 * @param err Error information if minimizing the locale failed.  If the length
 * of the localeID and the null-terminator is greater than the maximum allowed size,
 * or the localeId is not well-formed, the error code is U_ILLEGAL_ARGUMENT_ERROR.
 * @internal ICU 64
 */
U_COMMON_API శూన్య
ulocimp_minimizeSubtags(స్థిరరాశి అక్షరం* localeID,
                        icu::ByteSink& sink,
                        bool favorScript,
                        UError_సంకేతం& err);

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
locale_get_Keywords_ప్రారంభం(std::string_view localeID U_LIFETIME_BOUND);

bool
ultag_isExtensionSubtags(స్థిరరాశి అక్షరం* s, int32_t len);

bool
ultag_isLanguageSubtag(స్థిరరాశి అక్షరం* s, int32_t len);

bool
ultag_is_Privateuse_విలువ_Subtags(స్థిరరాశి అక్షరం* s, int32_t len);

bool
ultag_isRegionSubtag(స్థిరరాశి అక్షరం* s, int32_t len);

bool
ultag_isScriptSubtag(స్థిరరాశి అక్షరం* s, int32_t len);

bool
ultag_isTransformedExtensionSubtags(స్థిరరాశి అక్షరం* s, int32_t len);

bool
ultag_isUnicodeExtensionSubtags(స్థిరరాశి అక్షరం* s, int32_t len);

bool
ultag_isUnicodeLocaleAttribute(స్థిరరాశి అక్షరం* s, int32_t len);

bool
ultag_is_Unicode_Locale_లక్షణాలు(స్థిరరాశి అక్షరం* s, int32_t len);

bool
ultag_is_Unicode_Locale_కీలకం(స్థిరరాశి అక్షరం* s, int32_t len);

bool
ultag_isUnicodeLocaleType(స్థిరరాశి అక్షరం* s, int32_t len);

bool
ultag_isVariantSubtags(స్థిరరాశి అక్షరం* s, int32_t len);

స్థిరరాశి అక్షరం*
ultag_get_TKey_ప్రారంభం(స్థిరరాశి అక్షరం* localeID U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_గమ్యం_Bcp_కీలకం(std::string_view కీలకం);

U_COMMON_API std::optional<std::string_view>
ulocimp_గమ్యం_Legacy_కీలకం(std::string_view కీలకం);

U_COMMON_API std::optional<std::string_view>
ulocimp_గమ్యం_Bcp_Type(std::string_view కీలకం, std::string_view type U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_గమ్యం_Legacy_Type(std::string_view కీలకం, std::string_view type U_LIFETIME_BOUND);

/* Function for testing purpose */
U_COMMON_API స్థిరరాశి అక్షరం* స్థిరరాశి*
ulocimp_get_తెలిసిన_Canonicalized_Locale_For_Test(int32_t& నిడివి);

// Return true if the value is already canonicalized.
U_COMMON_API bool
ulocimp_isCanonicalizedLocaleForTest(స్థిరరాశి అక్షరం* localeName);

#నిర్వచితమైతే __cplusplus
U_NAMESPACE_BEGIN
class U_COMMON_API RegionValidateMap : బహిరంగ UObject {
 బహిరంగ:
  RegionValidateMap();
  మాయా ~RegionValidateMap();
  bool is_సమితి(స్థిరరాశి అక్షరం* region) స్థిరరాశి;
  bool equals(స్థిరరాశి RegionValidateMap& that) స్థిరరాశి;
 రక్షిత:
  int32_t విలువ_2(స్థిరరాశి అక్షరం* region) స్థిరరాశి;
  uint32_t map[22]; // 26x26/32 = 22;
};
U_NAMESPACE_చివర
#అయితే_ముగింపు /* __cplusplus */

#అయితే_ముగింపు
