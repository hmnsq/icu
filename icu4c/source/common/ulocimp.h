// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2004-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ULOCIMP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULOCIMP_H

#ਅਨ੍ਤਰ੍ਭਾਵ <cstddef>
#ਅਨ੍ਤਰ੍ਭਾਵ <optional>
#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"

/**
 * Create an iterator over the specified keywords list
 * @param keywordList double-null terminated list. Will be copied.
 * @param keywordListSize size in bytes of keywordList
 * @param status err code
 * @return enumeration (owned by caller) of the keyword list.
 * @internal ICU 3.0
 */
U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ਅਚਲ ਅੱਖਰ *keywordList, int32_t keyword_List_ਮਾਪ, UError_ਕੋਡ* ਸਥਿਤੀ);

/**
 * Look up a resource bundle table item with fallback on the table level.
 * This is accessible so it can be called by C++ code.
 */
U_CAPI ਅਚਲ UChar * U_EXPORT2
uloc_get_ਸਾਰਣੀ_String_With_Fallback(
    ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
    ਅਚਲ ਅੱਖਰ *locale,
    ਅਚਲ ਅੱਖਰ *ਸਾਰਣੀ_ਕੁੰਜੀ,
    ਅਚਲ ਅੱਖਰ *sub_ਸਾਰਣੀ_ਕੁੰਜੀ,
    ਅਚਲ ਅੱਖਰ *ਵਸਤੂ_ਕੁੰਜੀ,
    int32_t *p_ਲੰਮਾਈ,
    UError_ਕੋਡ *p_Error_ਕੋਡ);

namespace {
/*returns true if a is an ID separator false otherwise*/
ਸਤਰਅੰਦਰ bool _isIDSeparator(ਅੱਖਰ a) { ਵਾਪਸ a == '_' || a == '-'; }
}  // namespace

U_CFUNC ਅਚਲ ਅੱਖਰ* 
uloc_getCurrentCountryID(ਅਚਲ ਅੱਖਰ* oldID U_LIFETIME_BOUND);

U_CFUNC ਅਚਲ ਅੱਖਰ* 
uloc_getCurrentLanguageID(ਅਚਲ ਅੱਖਰ* oldID U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Bcp_ਕੁੰਜੀ_With_Fallback(std::string_view keyword U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Bcp_Type_With_Fallback(std::string_view keyword, std::string_view ਮੁੱਲ_2 U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Legacy_ਕੁੰਜੀ_With_Fallback(std::string_view keyword U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Legacy_Type_With_Fallback(std::string_view keyword, std::string_view ਮੁੱਲ_2 U_LIFETIME_BOUND);

U_COMMON_API icu::CharString
ulocimp_getKeywords(std::string_view localeID,
                    ਅੱਖਰ prev,
                    bool ਮੁੱਲਾਂ_Too,
                    UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API ਖਾਲੀ
ulocimp_getKeywords(std::string_view localeID,
                    ਅੱਖਰ prev,
                    icu::ByteSink& sink,
                    bool ਮੁੱਲਾਂ_Too,
                    UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API icu::CharString
ulocimp_getName(std::string_view localeID,
                UError_ਕੋਡ& err);

U_COMMON_API ਖਾਲੀ
ulocimp_getName(std::string_view localeID,
                icu::ByteSink& sink,
                UError_ਕੋਡ& err);

U_COMMON_API icu::CharString
ulocimp_get_ਅਧਾਰ_Name(std::string_view localeID,
                    UError_ਕੋਡ& err);

U_COMMON_API ਖਾਲੀ
ulocimp_get_ਅਧਾਰ_Name(std::string_view localeID,
                    icu::ByteSink& sink,
                    UError_ਕੋਡ& err);

U_COMMON_API icu::CharString
ulocimp_canonicalize(std::string_view localeID,
                     UError_ਕੋਡ& err);

U_COMMON_API ਖਾਲੀ
ulocimp_canonicalize(std::string_view localeID,
                     icu::ByteSink& sink,
                     UError_ਕੋਡ& err);

U_COMMON_API icu::CharString
ulocimp_get_Keyword_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ* localeID,
                        std::string_view keywordName,
                        UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API ਖਾਲੀ
ulocimp_get_Keyword_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API icu::CharString
ulocimp_getLanguage(std::string_view localeID, UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API icu::CharString
ulocimp_getScript(std::string_view localeID, UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API icu::CharString
ulocimp_getRegion(std::string_view localeID, UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API icu::CharString
ulocimp_getVariant(std::string_view localeID, UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API ਖਾਲੀ
ulocimp_ਸੈੱਟ_Keyword_ਮੁੱਲ(std::string_view keywordName,
                        std::string_view keyword_ਮੁੱਲ,
                        icu::CharString& localeID,
                        UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API int32_t
ulocimp_ਸੈੱਟ_Keyword_ਮੁੱਲ(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_ਮੁੱਲ,
                        icu::ByteSink& sink,
                        UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API ਖਾਲੀ
ulocimp_getSubtags(
        std::string_view localeID,
        icu::CharString* language,
        icu::CharString* script,
        icu::CharString* region,
        icu::CharString* variant,
        ਅਚਲ ਅੱਖਰ** p_ਸਿਰਾ,
        UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API ਖਾਲੀ
ulocimp_getSubtags(
        std::string_view localeID,
        icu::ByteSink* language,
        icu::ByteSink* script,
        icu::ByteSink* region,
        icu::ByteSink* variant,
        ਅਚਲ ਅੱਖਰ** p_ਸਿਰਾ,
        UError_ਕੋਡ& ਸਥਿਤੀ);

ਸਤਰਅੰਦਰ ਖਾਲੀ
ulocimp_getSubtags(
        std::string_view localeID,
        std::nullptr_t,
        std::nullptr_t,
        std::nullptr_t,
        std::nullptr_t,
        ਅਚਲ ਅੱਖਰ** p_ਸਿਰਾ,
        UError_ਕੋਡ& ਸਥਿਤੀ) {
    ulocimp_getSubtags(
            localeID,
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            p_ਸਿਰਾ,
            ਸਥਿਤੀ);
}

U_COMMON_API icu::CharString
ulocimp_get_ਮਾਪਾ(ਅਚਲ ਅੱਖਰ* localeID,
                  UError_ਕੋਡ& err);

U_COMMON_API ਖਾਲੀ
ulocimp_get_ਮਾਪਾ(ਅਚਲ ਅੱਖਰ* localeID,
                  icu::ByteSink& sink,
                  UError_ਕੋਡ& err);

U_COMMON_API icu::CharString
ulocimp_ਮੰਜ਼ਿਲ_Language_ਟੈਗ(ਅਚਲ ਅੱਖਰ* localeID,
                      bool strict,
                      UError_ਕੋਡ& ਸਥਿਤੀ);

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
U_COMMON_API ਖਾਲੀ
ulocimp_ਮੰਜ਼ਿਲ_Language_ਟੈਗ(ਅਚਲ ਅੱਖਰ* localeID,
                      icu::ByteSink& sink,
                      bool strict,
                      UError_ਕੋਡ& err);

U_COMMON_API icu::CharString
ulocimp_for_Language_ਟੈਗ(ਅਚਲ ਅੱਖਰ* langtag,
                       int32_t ਟੈਗ_Len,
                       int32_t* parsed_ਲੰਮਾਈ,
                       UError_ਕੋਡ& ਸਥਿਤੀ);

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
U_COMMON_API ਖਾਲੀ
ulocimp_for_Language_ਟੈਗ(ਅਚਲ ਅੱਖਰ* langtag,
                       int32_t ਟੈਗ_Len,
                       icu::ByteSink& sink,
                       int32_t* parsed_ਲੰਮਾਈ,
                       UError_ਕੋਡ& err);

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
ulocimp_get_Region_For_Supplemental_ਅੰਕੜੇ(ਅਚਲ ਅੱਖਰ *localeID, bool inferRegion,
                                     UError_ਕੋਡ& ਸਥਿਤੀ);

U_COMMON_API icu::CharString
ulocimp_ਜੋੜੋ_Likely_Subtags(ਅਚਲ ਅੱਖਰ* localeID,
                         UError_ਕੋਡ& ਸਥਿਤੀ);

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
U_COMMON_API ਖਾਲੀ
ulocimp_ਜੋੜੋ_Likely_Subtags(ਅਚਲ ਅੱਖਰ* localeID,
                         icu::ByteSink& sink,
                         UError_ਕੋਡ& err);

U_COMMON_API icu::CharString
ulocimp_minimizeSubtags(ਅਚਲ ਅੱਖਰ* localeID,
                        bool favorScript,
                        UError_ਕੋਡ& ਸਥਿਤੀ);

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
U_COMMON_API ਖਾਲੀ
ulocimp_minimizeSubtags(ਅਚਲ ਅੱਖਰ* localeID,
                        icu::ByteSink& sink,
                        bool favorScript,
                        UError_ਕੋਡ& err);

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
locale_get_Keywords_ਸ਼ੁਰੂ(std::string_view localeID U_LIFETIME_BOUND);

bool
ultag_isExtensionSubtags(ਅਚਲ ਅੱਖਰ* s, int32_t len);

bool
ultag_isLanguageSubtag(ਅਚਲ ਅੱਖਰ* s, int32_t len);

bool
ultag_is_Privateuse_ਮੁੱਲ_Subtags(ਅਚਲ ਅੱਖਰ* s, int32_t len);

bool
ultag_isRegionSubtag(ਅਚਲ ਅੱਖਰ* s, int32_t len);

bool
ultag_isScriptSubtag(ਅਚਲ ਅੱਖਰ* s, int32_t len);

bool
ultag_isTransformedExtensionSubtags(ਅਚਲ ਅੱਖਰ* s, int32_t len);

bool
ultag_isUnicodeExtensionSubtags(ਅਚਲ ਅੱਖਰ* s, int32_t len);

bool
ultag_isUnicodeLocaleAttribute(ਅਚਲ ਅੱਖਰ* s, int32_t len);

bool
ultag_is_Unicode_Locale_ਗੁਣ(ਅਚਲ ਅੱਖਰ* s, int32_t len);

bool
ultag_is_Unicode_Locale_ਕੁੰਜੀ(ਅਚਲ ਅੱਖਰ* s, int32_t len);

bool
ultag_isUnicodeLocaleType(ਅਚਲ ਅੱਖਰ* s, int32_t len);

bool
ultag_isVariantSubtags(ਅਚਲ ਅੱਖਰ* s, int32_t len);

ਅਚਲ ਅੱਖਰ*
ultag_get_TKey_ਸ਼ੁਰੂ(ਅਚਲ ਅੱਖਰ* localeID U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Bcp_ਕੁੰਜੀ(std::string_view ਕੁੰਜੀ);

U_COMMON_API std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Legacy_ਕੁੰਜੀ(std::string_view ਕੁੰਜੀ);

U_COMMON_API std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Bcp_Type(std::string_view ਕੁੰਜੀ, std::string_view type U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Legacy_Type(std::string_view ਕੁੰਜੀ, std::string_view type U_LIFETIME_BOUND);

/* Function for testing purpose */
U_COMMON_API ਅਚਲ ਅੱਖਰ* ਅਚਲ*
ulocimp_get_ਗਿਆਤ_Canonicalized_Locale_For_Test(int32_t& ਲੰਮਾਈ);

// Return true if the value is already canonicalized.
U_COMMON_API bool
ulocimp_isCanonicalizedLocaleForTest(ਅਚਲ ਅੱਖਰ* localeName);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
U_NAMESPACE_BEGIN
class U_COMMON_API RegionValidateMap : ਜਨਤਕ UObject {
 ਜਨਤਕ:
  RegionValidateMap();
  ਆਭਾਸੀ ~RegionValidateMap();
  bool is_ਸੈੱਟ(ਅਚਲ ਅੱਖਰ* region) ਅਚਲ;
  bool equals(ਅਚਲ RegionValidateMap& that) ਅਚਲ;
 ਸੁਰੱਖਿਅਤ:
  int32_t ਮੁੱਲ_2(ਅਚਲ ਅੱਖਰ* region) ਅਚਲ;
  uint32_t map[22]; // 26x26/32 = 22;
};
U_NAMESPACE_ਸਿਰਾ
#ਜੇ_ਅੰਤ /* __cplusplus */

#ਜੇ_ਅੰਤ
