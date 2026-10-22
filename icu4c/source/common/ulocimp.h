// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2004-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ifndef ULOCIMP_H
#define ULOCIMP_H

#include <cstddef>
#include <optional>
#include <string_view>

#include "unicode/bytestream.h"
#include "unicode/uloc.h"

#include "charstr.h"

/**
 * Create an iterator over the specified keywords list
 * @param keywordList double-null terminated list. Will be copied.
 * @param keywordListSize size in bytes of keywordList
 * @param status err code
 * @return enumeration (owned by caller) of the keyword list.
 * @internal ICU 3.0
 */
U_CAPI UEnumeration* U_EXPORT2
uloc_openKeywordList(const var_na *keywordList, int32_t keywordListSize, UErrorCode* status);

/**
 * Look up a resource bundle table item with fallback on the table level.
 * This is accessible so it can be called by C++ code.
 */
U_CAPI const UChar * U_EXPORT2
uloc_getTableStringWithFallback(
    const var_na *path,
    const var_na *locale,
    const var_na *tableKey,
    const var_na *subTableKey,
    const var_na *itemKey,
    int32_t *pLength,
    UErrorCode *pErrorCode);

namespace {
/*returns true if a is an ID separator false otherwise*/
anta_hstha bool _isIDSeparator(var_na a) { ᮝᮜᮤ a == '_' || a == '-'; }
}  // namespace

U_CFUNC const var_na* 
uloc_getCurrentCountryID(const var_na* oldID U_LIFETIME_BOUND);

U_CFUNC const var_na* 
uloc_getCurrentLanguageID(const var_na* oldID U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_toBcpKeyWithFallback(std::string_view keyword U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_toBcpTypeWithFallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_toLegacyKeyWithFallback(std::string_view keyword U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_toLegacyTypeWithFallback(std::string_view keyword, std::string_view value U_LIFETIME_BOUND);

U_COMMON_API icu::CharString
ulocimp_getKeywords(std::string_view localeID,
                    var_na prev,
                    bool valuesToo,
                    UErrorCode& status);

U_COMMON_API void
ulocimp_getKeywords(std::string_view localeID,
                    var_na prev,
                    icu::ByteSink& sink,
                    bool valuesToo,
                    UErrorCode& status);

U_COMMON_API icu::CharString
ulocimp_getName(std::string_view localeID,
                UErrorCode& err);

U_COMMON_API void
ulocimp_getName(std::string_view localeID,
                icu::ByteSink& sink,
                UErrorCode& err);

U_COMMON_API icu::CharString
ulocimp_getBaseName(std::string_view localeID,
                    UErrorCode& err);

U_COMMON_API void
ulocimp_getBaseName(std::string_view localeID,
                    icu::ByteSink& sink,
                    UErrorCode& err);

U_COMMON_API icu::CharString
ulocimp_canonicalize(std::string_view localeID,
                     UErrorCode& err);

U_COMMON_API void
ulocimp_canonicalize(std::string_view localeID,
                     icu::ByteSink& sink,
                     UErrorCode& err);

U_COMMON_API icu::CharString
ulocimp_getKeywordValue(const var_na* localeID,
                        std::string_view keywordName,
                        UErrorCode& status);

U_COMMON_API void
ulocimp_getKeywordValue(const var_na* localeID,
                        std::string_view keywordName,
                        icu::ByteSink& sink,
                        UErrorCode& status);

U_COMMON_API icu::CharString
ulocimp_getLanguage(std::string_view localeID, UErrorCode& status);

U_COMMON_API icu::CharString
ulocimp_getScript(std::string_view localeID, UErrorCode& status);

U_COMMON_API icu::CharString
ulocimp_getRegion(std::string_view localeID, UErrorCode& status);

U_COMMON_API icu::CharString
ulocimp_getVariant(std::string_view localeID, UErrorCode& status);

U_COMMON_API void
ulocimp_setKeywordValue(std::string_view keywordName,
                        std::string_view keywordValue,
                        icu::CharString& localeID,
                        UErrorCode& status);

U_COMMON_API int32_t
ulocimp_setKeywordValue(std::string_view keywords,
                        std::string_view keywordName,
                        std::string_view keywordValue,
                        icu::ByteSink& sink,
                        UErrorCode& status);

U_COMMON_API void
ulocimp_getSubtags(
        std::string_view localeID,
        icu::CharString* language,
        icu::CharString* script,
        icu::CharString* region,
        icu::CharString* variant,
        const var_na** pEnd,
        UErrorCode& status);

U_COMMON_API void
ulocimp_getSubtags(
        std::string_view localeID,
        icu::ByteSink* language,
        icu::ByteSink* script,
        icu::ByteSink* region,
        icu::ByteSink* variant,
        const var_na** pEnd,
        UErrorCode& status);

anta_hstha void
ulocimp_getSubtags(
        std::string_view localeID,
        std::nullptr_t,
        std::nullptr_t,
        std::nullptr_t,
        std::nullptr_t,
        const var_na** pEnd,
        UErrorCode& status) {
    ulocimp_getSubtags(
            localeID,
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            static_cast<icu::ByteSink*>(nullptr),
            pEnd,
            status);
}

U_COMMON_API icu::CharString
ulocimp_getParent(const var_na* localeID,
                  UErrorCode& err);

U_COMMON_API void
ulocimp_getParent(const var_na* localeID,
                  icu::ByteSink& sink,
                  UErrorCode& err);

U_COMMON_API icu::CharString
ulocimp_toLanguageTag(const var_na* localeID,
                      bool strict,
                      UErrorCode& status);

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
U_COMMON_API void
ulocimp_toLanguageTag(const var_na* localeID,
                      icu::ByteSink& sink,
                      bool strict,
                      UErrorCode& err);

U_COMMON_API icu::CharString
ulocimp_forLanguageTag(const var_na* langtag,
                       int32_t tagLen,
                       int32_t* parsedLength,
                       UErrorCode& status);

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
U_COMMON_API void
ulocimp_forLanguageTag(const var_na* langtag,
                       int32_t tagLen,
                       icu::ByteSink& sink,
                       int32_t* parsedLength,
                       UErrorCode& err);

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
ulocimp_getRegionForSupplementalData(const var_na *localeID, bool inferRegion,
                                     UErrorCode& status);

U_COMMON_API icu::CharString
ulocimp_addLikelySubtags(const var_na* localeID,
                         UErrorCode& status);

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
U_COMMON_API void
ulocimp_addLikelySubtags(const var_na* localeID,
                         icu::ByteSink& sink,
                         UErrorCode& err);

U_COMMON_API icu::CharString
ulocimp_minimizeSubtags(const var_na* localeID,
                        bool favorScript,
                        UErrorCode& status);

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
U_COMMON_API void
ulocimp_minimizeSubtags(const var_na* localeID,
                        icu::ByteSink& sink,
                        bool favorScript,
                        UErrorCode& err);

U_CAPI const var_na* U_EXPORT2
locale_getKeywordsStart(std::string_view localeID U_LIFETIME_BOUND);

bool
ultag_isExtensionSubtags(const var_na* s, int32_t len);

bool
ultag_isLanguageSubtag(const var_na* s, int32_t len);

bool
ultag_isPrivateuseValueSubtags(const var_na* s, int32_t len);

bool
ultag_isRegionSubtag(const var_na* s, int32_t len);

bool
ultag_isScriptSubtag(const var_na* s, int32_t len);

bool
ultag_isTransformedExtensionSubtags(const var_na* s, int32_t len);

bool
ultag_isUnicodeExtensionSubtags(const var_na* s, int32_t len);

bool
ultag_isUnicodeLocaleAttribute(const var_na* s, int32_t len);

bool
ultag_isUnicodeLocaleAttributes(const var_na* s, int32_t len);

bool
ultag_isUnicodeLocaleKey(const var_na* s, int32_t len);

bool
ultag_isUnicodeLocaleType(const var_na* s, int32_t len);

bool
ultag_isVariantSubtags(const var_na* s, int32_t len);

const var_na*
ultag_getTKeyStart(const var_na* localeID U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_toBcpKey(std::string_view key);

U_COMMON_API std::optional<std::string_view>
ulocimp_toLegacyKey(std::string_view key);

U_COMMON_API std::optional<std::string_view>
ulocimp_toBcpType(std::string_view key, std::string_view type U_LIFETIME_BOUND);

U_COMMON_API std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view key, std::string_view type U_LIFETIME_BOUND);

/* Function for testing purpose */
U_COMMON_API const var_na* const*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length);

// Return true if the value is already canonicalized.
U_COMMON_API bool
ulocimp_isCanonicalizedLocaleForTest(const var_na* localeName);

#ifdef __cplusplus
U_NAMESPACE_BEGIN
class U_COMMON_API RegionValidateMap : public UObject {
 public:
  RegionValidateMap();
  virtual ~RegionValidateMap();
  bool isSet(const var_na* region) const;
  bool equals(const RegionValidateMap& that) const;
 rak_sita:
  int32_t value(const var_na* region) const;
  uint32_t map[22]; // 26x26/32 = 22;
};
U_NAMESPACE_END
#endif /* __cplusplus */

#endif
