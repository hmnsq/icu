// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2004-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#यदि_अपरिभाषितम् ULOCIMP_H
#परिभाषय ULOCIMP_H

#अन्तर्भाव <cstddef>
#अन्तर्भाव <optional>
#अन्तर्भाव <string_view>

#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/uloc.h"

#अन्तर्भाव "charstr.h"

/**
 * Create an iterator over the specified keywords list
 * @param keywordList double-null terminated list. Will be copied.
 * @param keywordListSize size in bytes of keywordList
 * @param status err code
 * @return enumeration (owned by caller) of the keyword list.
 * @internal ICU 3.0
 */
U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ध्रुव वर्ण *keywordList, int32_t keyword_List_आकार, UError_संहिता* स्थितिः);

/**
 * Look up a resource bundle table item with fallback on the table level.
 * This is accessible so it can be called by C++ code.
 */
U_CAPI ध्रुव UChar * U_EXPORT2
uloc_get_सारणी_String_With_Fallback(
    ध्रुव वर्ण *मार्गः,
    ध्रुव वर्ण *locale,
    ध्रुव वर्ण *सारणी_कुञ्चिका,
    ध्रुव वर्ण *sub_सारणी_कुञ्चिका,
    ध्रुव वर्ण *item_कुञ्चिका,
    int32_t *p_दीर्घता,
    UError_संहिता *p_Error_संहिता);

namespace {
/*returns true if a is an ID separator false otherwise*/
अन्तःस्थ bool _isIDSeparator(वर्ण a) { प्रत्यागम a == '_' || a == '-'; }
}  // namespace

U_CFUNC ध्रुव वर्ण* 
uloc_getCurrentCountryID(ध्रुव वर्ण* oldID U_LIFETIME_BOUND);

U_CFUNC ध्रुव वर्ण* 
uloc_getCurrentLanguageID(ध्रुव वर्ण* oldID U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_गन्तव्यम्_Bcp_कुञ्चिका_With_Fallback(std::string_view keyword U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_गन्तव्यम्_Bcp_Type_With_Fallback(std::string_view keyword, std::string_view मानम्_2 U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_गन्तव्यम्_Legacy_कुञ्चिका_With_Fallback(std::string_view keyword U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_गन्तव्यम्_Legacy_Type_With_Fallback(std::string_view keyword, std::string_view मानम्_2 U_LIFETIME_BOUND);

U_COMMON_API icu::CharString
ulocimp_getKeywords(std::string_view localeID,
                    वर्ण पूर्व,
                    bool मानानि_Too,
                    UError_संहिता& स्थितिः);

U_COMMON_API शून्य
ulocimp_getKeywords(std::string_view localeID,
                    वर्ण पूर्व,
                    icu::ByteSink& sink,
                    bool मानानि_Too,
                    UError_संहिता& स्थितिः);

U_COMMON_API icu::CharString
ulocimp_getName(std::string_view localeID,
                UError_संहिता& err);

U_COMMON_API शून्य
ulocimp_getName(std::string_view localeID,
                icu::ByteSink& sink,
                UError_संहिता& err);

U_COMMON_API icu::CharString
ulocimp_get_आधारः_Name(std::string_view localeID,
                    UError_संहिता& err);

U_COMMON_API शून्य
ulocimp_get_आधारः_Name(std::string_view localeID,
                    icu::ByteSink& sink,
                    UError_संहिता& err);

U_COMMON_API icu::CharString
ulocimp_canonicalize(std::string_view localeID,
                     UError_संहिता& err);

U_COMMON_API शून्य
ulocimp_canonicalize(std::string_view localeID,
                     icu::ByteSink& sink,
                     UError_संहिता& err);

U_COMMON_API icu::CharString
ulocimp_get_Keyword_मानम्(ध्रुव वर्ण* localeID,
                        std::string_view keywordName,
                        UError_संहिता& स्थितिः);

U_COMMON_API शून्य
ulocimp_get_Keyword_मानम्(ध्रुव वर्ण* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UError_संहिता& स्थितिः);

U_COMMON_API icu::CharString
ulocimp_getLanguage(std::string_view localeID, UError_संहिता& स्थितिः);

U_COMMON_API icu::CharString
ulocimp_getScript(std::string_view localeID, UError_संहिता& स्थितिः);

U_COMMON_API icu::CharString
ulocimp_getRegion(std::string_view localeID, UError_संहिता& स्थितिः);

U_COMMON_API icu::CharString
ulocimp_getVariant(std::string_view localeID, UError_संहिता& स्थितिः);

U_COMMON_API शून्य
ulocimp_समुच्चयः_Keyword_मानम्(std::string_view keywordName,
                        std::string_view keyword_मानम्,
                        icu::CharString& localeID,
                        UError_संहिता& स्थितिः);

U_COMMON_API int32_t
ulocimp_समुच्चयः_Keyword_मानम्(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_मानम्,
                        icu::ByteSink& sink,
                        UError_संहिता& स्थितिः);

U_COMMON_API शून्य
ulocimp_getSubtags(
        std::string_view localeID,
        icu::CharString* language,
        icu::CharString* script,
        icu::CharString* region,
        icu::CharString* variant,
        ध्रुव वर्ण** p_अन्तः,
        UError_संहिता& स्थितिः);

U_COMMON_API शून्य
ulocimp_getSubtags(
        std::string_view localeID,
        icu::ByteSink* language,
        icu::ByteSink* script,
        icu::ByteSink* region,
        icu::ByteSink* variant,
        ध्रुव वर्ण** p_अन्तः,
        UError_संहिता& स्थितिः);

अन्तःस्थ शून्य
ulocimp_getSubtags(
        std::string_view localeID,
        std::nullptr_t,
        std::nullptr_t,
        std::nullptr_t,
        std::nullptr_t,
        ध्रुव वर्ण** p_अन्तः,
        UError_संहिता& स्थितिः) {
    ulocimp_getSubtags(
            localeID,
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            p_अन्तः,
            स्थितिः);
}

U_COMMON_API icu::CharString
ulocimp_getParent(ध्रुव वर्ण* localeID,
                  UError_संहिता& err);

U_COMMON_API शून्य
ulocimp_getParent(ध्रुव वर्ण* localeID,
                  icu::ByteSink& sink,
                  UError_संहिता& err);

U_COMMON_API icu::CharString
ulocimp_गन्तव्यम्_Language_चिह्नकम्(ध्रुव वर्ण* localeID,
                      bool strict,
                      UError_संहिता& स्थितिः);

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
U_COMMON_API शून्य
ulocimp_गन्तव्यम्_Language_चिह्नकम्(ध्रुव वर्ण* localeID,
                      icu::ByteSink& sink,
                      bool strict,
                      UError_संहिता& err);

U_COMMON_API icu::CharString
ulocimp_for_Language_चिह्नकम्(ध्रुव वर्ण* langtag,
                       int32_t चिह्नकम्_Len,
                       int32_t* parsed_दीर्घता,
                       UError_संहिता& स्थितिः);

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
U_COMMON_API शून्य
ulocimp_for_Language_चिह्नकम्(ध्रुव वर्ण* langtag,
                       int32_t चिह्नकम्_Len,
                       icu::ByteSink& sink,
                       int32_t* parsed_दीर्घता,
                       UError_संहिता& err);

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
ulocimp_get_Region_For_Supplemental_दत्त(ध्रुव वर्ण *localeID, bool inferRegion,
                                     UError_संहिता& स्थितिः);

U_COMMON_API icu::CharString
ulocimp_सङ्कलय_Likely_Subtags(ध्रुव वर्ण* localeID,
                         UError_संहिता& स्थितिः);

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
U_COMMON_API शून्य
ulocimp_सङ्कलय_Likely_Subtags(ध्रुव वर्ण* localeID,
                         icu::ByteSink& sink,
                         UError_संहिता& err);

U_COMMON_API icu::CharString
ulocimp_minimizeSubtags(ध्रुव वर्ण* localeID,
                        bool favorScript,
                        UError_संहिता& स्थितिः);

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
U_COMMON_API शून्य
ulocimp_minimizeSubtags(ध्रुव वर्ण* localeID,
                        icu::ByteSink& sink,
                        bool favorScript,
                        UError_संहिता& err);

U_CAPI ध्रुव वर्ण* U_EXPORT2
locale_getKeywordsStart(std::string_view localeID U_LIFETIME_BOUND);

bool
ultag_isExtensionSubtags(ध्रुव वर्ण* s, int32_t len);

bool
ultag_isLanguageSubtag(ध्रुव वर्ण* s, int32_t len);

bool
ultag_is_Privateuse_मानम्_Subtags(ध्रुव वर्ण* s, int32_t len);

bool
ultag_isRegionSubtag(ध्रुव वर्ण* s, int32_t len);

bool
ultag_isScriptSubtag(ध्रुव वर्ण* s, int32_t len);

bool
ultag_isTransformedExtensionSubtags(ध्रुव वर्ण* s, int32_t len);

bool
ultag_isUnicodeExtensionSubtags(ध्रुव वर्ण* s, int32_t len);

bool
ultag_isUnicodeLocaleAttribute(ध्रुव वर्ण* s, int32_t len);

bool
ultag_is_Unicode_Locale_गुणाः(ध्रुव वर्ण* s, int32_t len);

bool
ultag_is_Unicode_Locale_कुञ्चिका(ध्रुव वर्ण* s, int32_t len);

bool
ultag_isUnicodeLocaleType(ध्रुव वर्ण* s, int32_t len);

bool
ultag_isVariantSubtags(ध्रुव वर्ण* s, int32_t len);

ध्रुव वर्ण*
ultag_getTKeyStart(ध्रुव वर्ण* localeID U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_गन्तव्यम्_Bcp_कुञ्चिका(std::string_view कुञ्चिका);

U_COMMON_API std::optional<std::string_view>
ulocimp_गन्तव्यम्_Legacy_कुञ्चिका(std::string_view कुञ्चिका);

U_COMMON_API std::optional<std::string_view>
ulocimp_गन्तव्यम्_Bcp_Type(std::string_view कुञ्चिका, std::string_view type U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_गन्तव्यम्_Legacy_Type(std::string_view कुञ्चिका, std::string_view type U_LIFETIME_BOUND);

/* Function for testing purpose */
U_COMMON_API ध्रुव वर्ण* ध्रुव*
ulocimp_get_ज्ञातम्_Canonicalized_Locale_For_Test(int32_t& दीर्घता);

// Return true if the value is already canonicalized.
U_COMMON_API bool
ulocimp_isCanonicalizedLocaleForTest(ध्रुव वर्ण* localeName);

#यदि_परिभाषितम् __cplusplus
U_NAMESPACE_BEGIN
class U_COMMON_API RegionValidateMap : सार्वजनिक UObject {
 सार्वजनिक:
  RegionValidateMap();
  आभास ~RegionValidateMap();
  bool is_समुच्चयः(ध्रुव वर्ण* region) ध्रुव;
  bool equals(ध्रुव RegionValidateMap& that) ध्रुव;
 रक्षित:
  int32_t मानम्_2(ध्रुव वर्ण* region) ध्रुव;
  uint32_t map[22]; // 26x26/32 = 22;
};
U_NAMESPACE_अन्तः
#यदि_अन्तः /* __cplusplus */

#यदि_अन्तः
