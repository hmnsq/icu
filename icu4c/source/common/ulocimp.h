// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2004-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ULOCIMP_H
#ವ್ಯಾಖ್ಯಾನಿಸು ULOCIMP_H

#ಅನ್ತರ್ಭಾವ <cstddef>
#ಅನ್ತರ್ಭಾವ <optional>
#ಅನ್ತರ್ಭಾವ <string_view>

#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"

#ಅನ್ತರ್ಭಾವ "charstr.h"

/**
 * Create an iterator over the specified keywords list
 * @param keywordList double-null terminated list. Will be copied.
 * @param keywordListSize size in bytes of keywordList
 * @param status err code
 * @return enumeration (owned by caller) of the keyword list.
 * @internal ICU 3.0
 */
U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *keywordList, int32_t keyword_List_ಗಾತ್ರ, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2);

/**
 * Look up a resource bundle table item with fallback on the table level.
 * This is accessible so it can be called by C++ code.
 */
U_CAPI ಸ್ಥಿರಾಂಕ UChar * U_EXPORT2
uloc_get_ಕೋಷ್ಟಕ_String_With_Fallback(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೋಷ್ಟಕ_ಕೀಲಿ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sub_ಕೋಷ್ಟಕ_ಕೀಲಿ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಅಂಶ_ಕೀಲಿ,
    int32_t *p_ಉದ್ದಳತೆ,
    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

namespace {
/*returns true if a is an ID separator false otherwise*/
ಸಾಲೊಳಗೆ bool _isIDSeparator(ಅಕ್ಷರ a) { ಹಿಂತಿರುಗಿ a == '_' || a == '-'; }
}  // namespace

U_CFUNC ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* 
uloc_getCurrentCountryID(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* oldID U_LIFETIME_BOUND);

U_CFUNC ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* 
uloc_getCurrentLanguageID(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* oldID U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Bcp_ಕೀಲಿ_With_Fallback(std::string_view keyword U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Bcp_Type_With_Fallback(std::string_view keyword, std::string_view ಮೌಲ್ಯ_2 U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Legacy_ಕೀಲಿ_With_Fallback(std::string_view keyword U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Legacy_Type_With_Fallback(std::string_view keyword, std::string_view ಮೌಲ್ಯ_2 U_LIFETIME_BOUND);

U_COMMON_API icu::CharString
ulocimp_getKeywords(std::string_view localeID,
                    ಅಕ್ಷರ prev,
                    bool ಮೌಲ್ಯಗಳು_Too,
                    UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API ಶೂನ್ಯ
ulocimp_getKeywords(std::string_view localeID,
                    ಅಕ್ಷರ prev,
                    icu::ByteSink& sink,
                    bool ಮೌಲ್ಯಗಳು_Too,
                    UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API icu::CharString
ulocimp_getName(std::string_view localeID,
                UError_ಸಂಕೇತ& err);

U_COMMON_API ಶೂನ್ಯ
ulocimp_getName(std::string_view localeID,
                icu::ByteSink& sink,
                UError_ಸಂಕೇತ& err);

U_COMMON_API icu::CharString
ulocimp_get_ತಳ_Name(std::string_view localeID,
                    UError_ಸಂಕೇತ& err);

U_COMMON_API ಶೂನ್ಯ
ulocimp_get_ತಳ_Name(std::string_view localeID,
                    icu::ByteSink& sink,
                    UError_ಸಂಕೇತ& err);

U_COMMON_API icu::CharString
ulocimp_canonicalize(std::string_view localeID,
                     UError_ಸಂಕೇತ& err);

U_COMMON_API ಶೂನ್ಯ
ulocimp_canonicalize(std::string_view localeID,
                     icu::ByteSink& sink,
                     UError_ಸಂಕೇತ& err);

U_COMMON_API icu::CharString
ulocimp_get_Keyword_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                        std::string_view keywordName,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API ಶೂನ್ಯ
ulocimp_get_Keyword_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API icu::CharString
ulocimp_getLanguage(std::string_view localeID, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API icu::CharString
ulocimp_getScript(std::string_view localeID, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API icu::CharString
ulocimp_getRegion(std::string_view localeID, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API icu::CharString
ulocimp_getVariant(std::string_view localeID, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API ಶೂನ್ಯ
ulocimp_ಗಣ_Keyword_ಮೌಲ್ಯ(std::string_view keywordName,
                        std::string_view keyword_ಮೌಲ್ಯ,
                        icu::CharString& localeID,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API int32_t
ulocimp_ಗಣ_Keyword_ಮೌಲ್ಯ(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keyword_ಮೌಲ್ಯ,
                        icu::ByteSink& sink,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API ಶೂನ್ಯ
ulocimp_getSubtags(
        std::string_view localeID,
        icu::CharString* language,
        icu::CharString* script,
        icu::CharString* region,
        icu::CharString* variant,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** p_ಕೊನೆ,
        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API ಶೂನ್ಯ
ulocimp_getSubtags(
        std::string_view localeID,
        icu::ByteSink* language,
        icu::ByteSink* script,
        icu::ByteSink* region,
        icu::ByteSink* variant,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** p_ಕೊನೆ,
        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

ಸಾಲೊಳಗೆ ಶೂನ್ಯ
ulocimp_getSubtags(
        std::string_view localeID,
        std::nullptr_t,
        std::nullptr_t,
        std::nullptr_t,
        std::nullptr_t,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** p_ಕೊನೆ,
        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ulocimp_getSubtags(
            localeID,
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            p_ಕೊನೆ,
            ಸ್ಥಿತಿ_2);
}

U_COMMON_API icu::CharString
ulocimp_get_ಪೋಷಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                  UError_ಸಂಕೇತ& err);

U_COMMON_API ಶೂನ್ಯ
ulocimp_get_ಪೋಷಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                  icu::ByteSink& sink,
                  UError_ಸಂಕೇತ& err);

U_COMMON_API icu::CharString
ulocimp_ಗಮ್ಯ_Language_ಟ್ಯಾಗ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                      bool strict,
                      UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

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
U_COMMON_API ಶೂನ್ಯ
ulocimp_ಗಮ್ಯ_Language_ಟ್ಯಾಗ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                      icu::ByteSink& sink,
                      bool strict,
                      UError_ಸಂಕೇತ& err);

U_COMMON_API icu::CharString
ulocimp_for_Language_ಟ್ಯಾಗ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* langtag,
                       int32_t ಟ್ಯಾಗ್_Len,
                       int32_t* parsed_ಉದ್ದಳತೆ,
                       UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

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
U_COMMON_API ಶೂನ್ಯ
ulocimp_for_Language_ಟ್ಯಾಗ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* langtag,
                       int32_t ಟ್ಯಾಗ್_Len,
                       icu::ByteSink& sink,
                       int32_t* parsed_ಉದ್ದಳತೆ,
                       UError_ಸಂಕೇತ& err);

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
ulocimp_get_Region_For_Supplemental_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *localeID, bool inferRegion,
                                     UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_COMMON_API icu::CharString
ulocimp_ಕೂಡಿಸು_Likely_Subtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                         UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

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
U_COMMON_API ಶೂನ್ಯ
ulocimp_ಕೂಡಿಸು_Likely_Subtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                         icu::ByteSink& sink,
                         UError_ಸಂಕೇತ& err);

U_COMMON_API icu::CharString
ulocimp_minimizeSubtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                        bool favorScript,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

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
U_COMMON_API ಶೂನ್ಯ
ulocimp_minimizeSubtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID,
                        icu::ByteSink& sink,
                        bool favorScript,
                        UError_ಸಂಕೇತ& err);

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
locale_get_Keywords_ಆರಂಭ(std::string_view localeID U_LIFETIME_BOUND);

bool
ultag_isExtensionSubtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

bool
ultag_isLanguageSubtag(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

bool
ultag_is_Privateuse_ಮೌಲ್ಯ_Subtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

bool
ultag_isRegionSubtag(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

bool
ultag_isScriptSubtag(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

bool
ultag_isTransformedExtensionSubtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

bool
ultag_isUnicodeExtensionSubtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

bool
ultag_isUnicodeLocaleAttribute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

bool
ultag_is_Unicode_Locale_ಗುಣಲಕ್ಷಣಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

bool
ultag_is_Unicode_Locale_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

bool
ultag_isUnicodeLocaleType(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

bool
ultag_isVariantSubtags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t len);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
ultag_get_TKey_ಆರಂಭ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Bcp_ಕೀಲಿ(std::string_view ಕೀಲಿ);

U_COMMON_API std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Legacy_ಕೀಲಿ(std::string_view ಕೀಲಿ);

U_COMMON_API std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Bcp_Type(std::string_view ಕೀಲಿ, std::string_view type U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Legacy_Type(std::string_view ಕೀಲಿ, std::string_view type U_LIFETIME_BOUND);

/* Function for testing purpose */
U_COMMON_API ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ*
ulocimp_get_ತಿಳಿದ_Canonicalized_Locale_For_Test(int32_t& ಉದ್ದಳತೆ);

// Return true if the value is already canonicalized.
U_COMMON_API bool
ulocimp_isCanonicalizedLocaleForTest(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeName);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
U_NAMESPACE_BEGIN
class U_COMMON_API RegionValidateMap : ಸಾರ್ವಜನಿಕ UObject {
 ಸಾರ್ವಜನಿಕ:
  RegionValidateMap();
  ವಾಸ್ತವಿಕ ~RegionValidateMap();
  bool is_ಗಣ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region) ಸ್ಥಿರಾಂಕ;
  bool equals(ಸ್ಥಿರಾಂಕ RegionValidateMap& that) ಸ್ಥಿರಾಂಕ;
 ಸಂರಕ್ಷಿತ:
  int32_t ಮೌಲ್ಯ_2(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region) ಸ್ಥಿರಾಂಕ;
  uint32_t map[22]; // 26x26/32 = 22;
};
U_NAMESPACE_ಕೊನೆ
#ಆದರೆ_ಅಂತ್ಯ /* __cplusplus */

#ಆದರೆ_ಅಂತ್ಯ
