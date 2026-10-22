// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2016, International Business Machines Corporation and
*   others.  All Rights Reserved.
*******************************************************************************
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ULDNAMES_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __ULDNAMES_H__

/**
 * \file
 * \brief C API: Provides display names of Locale ids and their components.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udisplaycontext.h"

#ਜੇ U_SHOW_CPLUSPLUS_API
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localpointer.h"
#ਜੇ_ਅੰਤ   // U_SHOW_CPLUSPLUS_API

/**
 * Enum used in LocaleDisplayNames::createInstance.
 * @stable ICU 4.4
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    /**
     * Use standard names when generating a locale name,
     * e.g. en_GB displays as 'English (United Kingdom)'.
     * @stable ICU 4.4
     */
    ULDN_STANDARD_ਨਾਂ = 0,
    /**
     * Use dialect names, when generating a locale name,
     * e.g. en_GB displays as 'British English'.
     * @stable ICU 4.4
     */
    ULDN_DIALECT_ਨਾਂ
} UDialectHandling;

/**
 * Opaque C service object type for the locale display names API
 * @stable ICU 4.4
 */
ਬਣਤਰ ULocale_Display_ਨਾਂ;

/** 
 * C typedef for struct ULocaleDisplayNames. 
 * @stable ICU 4.4 
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ULocale_Display_ਨਾਂ ULocale_Display_ਨਾਂ;  

#ਜੇ !UCONFIG_NO_FORMATTING

/**
 * Returns an instance of LocaleDisplayNames that returns names
 * formatted for the provided locale, using the provided
 * dialectHandling.  The usual value for dialectHandling is
 * ULOC_STANDARD_NAMES.
 *
 * @param locale the display locale 
 * @param dialectHandling how to select names for locales 
 * @return a ULocaleDisplayNames instance 
 * @param pErrorCode the status code
 * @stable ICU 4.4
 */
U_CAPI ULocale_Display_ਨਾਂ * U_EXPORT2
uldn_open(ਅਚਲ ਅੱਖਰ * locale,
          UDialectHandling dialectHandling,
          UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Closes a ULocaleDisplayNames instance obtained from uldn_open().
 * @param ldn the ULocaleDisplayNames instance to be closed
 * @stable ICU 4.4
 */
U_CAPI ਖਾਲੀ U_EXPORT2
uldn_close(ULocale_Display_ਨਾਂ *ldn);

#ਜੇ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * \class LocalULocaleDisplayNamesPointer
 * "Smart pointer" class, closes a ULocaleDisplayNames via uldn_close().
 * For most methods see the LocalPointerBase base class.
 *
 * @see LocalPointerBase
 * @see LocalPointer
 * @stable ICU 4.4
 */
U_DEFINE_LOCAL_OPEN_ਸੂਚਕ(Local_ULocale_Display_ਨਾਂ_ਸੂਚਕ, ULocale_Display_ਨਾਂ, uldn_close);

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ

/* getters for state */

/**
 * Returns the locale used to determine the display names. This is
 * not necessarily the same locale passed to {@link #uldn_open}.
 * @param ldn the LocaleDisplayNames instance
 * @return the display locale 
 * @stable ICU 4.4
 */
U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uldn_getLocale(ਅਚਲ ULocale_Display_ਨਾਂ* ldn U_LIFETIME_BOUND);

/**
 * Returns the dialect handling used in the display names.
 * @param ldn the LocaleDisplayNames instance
 * @return the dialect handling enum
 * @stable ICU 4.4
 */
U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ਅਚਲ ULocale_Display_ਨਾਂ *ldn);

/* names for entire locales */

/**
 * Returns the display name of the provided locale.
 * @param ldn the LocaleDisplayNames instance
 * @param locale the locale whose display name to return
 * @param result receives the display name
 * @param maxResultSize the size of the result buffer
 * @param pErrorCode the status code
 * @return the actual buffer size needed for the display name.  If it's
 * greater than maxResultSize, the returned name will be truncated.
 * @stable ICU 4.4
 */
U_CAPI int32_t U_EXPORT2
uldn_localeDisplayName(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                       ਅਚਲ ਅੱਖਰ *locale,
                       UChar *ਨਤੀਜਾ,
                       int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                       UError_ਕੋਡ *p_Error_ਕੋਡ);

/* names for components of a locale */

/**
 * Returns the display name of the provided language code.
 * @param ldn the LocaleDisplayNames instance
 * @param lang the language code whose display name to return
 * @param result receives the display name
 * @param maxResultSize the size of the result buffer
 * @param pErrorCode the status code
 * @return the actual buffer size needed for the display name.  If it's
 * greater than maxResultSize, the returned name will be truncated.
 * @stable ICU 4.4
 */
U_CAPI int32_t U_EXPORT2
uldn_languageDisplayName(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                         ਅਚਲ ਅੱਖਰ *lang,
                         UChar *ਨਤੀਜਾ,
                         int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                         UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Returns the display name of the provided script.
 * @param ldn the LocaleDisplayNames instance
 * @param script the script whose display name to return
 * @param result receives the display name
 * @param maxResultSize the size of the result buffer
 * @param pErrorCode the status code
 * @return the actual buffer size needed for the display name.  If it's
 * greater than maxResultSize, the returned name will be truncated.
 * @stable ICU 4.4
 */
U_CAPI int32_t U_EXPORT2
uldn_scriptDisplayName(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                       ਅਚਲ ਅੱਖਰ *script,
                       UChar *ਨਤੀਜਾ,
                       int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                       UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Returns the display name of the provided script code.
 * @param ldn the LocaleDisplayNames instance
 * @param scriptCode the script code whose display name to return
 * @param result receives the display name
 * @param maxResultSize the size of the result buffer
 * @param pErrorCode the status code
 * @return the actual buffer size needed for the display name.  If it's
 * greater than maxResultSize, the returned name will be truncated.
 * @stable ICU 4.4
 */
U_CAPI int32_t U_EXPORT2
uldn_script_ਕੋਡ_Display_Name(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                           UScript_ਕੋਡ script_ਕੋਡ,
                           UChar *ਨਤੀਜਾ,
                           int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                           UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Returns the display name of the provided region code.
 * @param ldn the LocaleDisplayNames instance
 * @param region the region code whose display name to return
 * @param result receives the display name
 * @param maxResultSize the size of the result buffer
 * @param pErrorCode the status code
 * @return the actual buffer size needed for the display name.  If it's
 * greater than maxResultSize, the returned name will be truncated.
 * @stable ICU 4.4
 */
U_CAPI int32_t U_EXPORT2
uldn_regionDisplayName(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                       ਅਚਲ ਅੱਖਰ *region,
                       UChar *ਨਤੀਜਾ,
                       int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                       UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Returns the display name of the provided variant
 * @param ldn the LocaleDisplayNames instance
 * @param variant the variant whose display name to return
 * @param result receives the display name
 * @param maxResultSize the size of the result buffer
 * @param pErrorCode the status code
 * @return the actual buffer size needed for the display name.  If it's
 * greater than maxResultSize, the returned name will be truncated.
 * @stable ICU 4.4
 */
U_CAPI int32_t U_EXPORT2
uldn_variantDisplayName(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                        ਅਚਲ ਅੱਖਰ *variant,
                        UChar *ਨਤੀਜਾ,
                        int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                        UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Returns the display name of the provided locale key
 * @param ldn the LocaleDisplayNames instance
 * @param key the locale key whose display name to return
 * @param result receives the display name
 * @param maxResultSize the size of the result buffer
 * @param pErrorCode the status code
 * @return the actual buffer size needed for the display name.  If it's
 * greater than maxResultSize, the returned name will be truncated.
 * @stable ICU 4.4
 */
U_CAPI int32_t U_EXPORT2
uldn_ਕੁੰਜੀ_Display_Name(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                    ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ,
                    UChar *ਨਤੀਜਾ,
                    int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                    UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Returns the display name of the provided value (used with the provided key).
 * @param ldn the LocaleDisplayNames instance
 * @param key the locale key
 * @param value the locale key's value
 * @param result receives the display name
 * @param maxResultSize the size of the result buffer
 * @param pErrorCode the status code
 * @return the actual buffer size needed for the display name.  If it's
 * greater than maxResultSize, the returned name will be truncated.
 * @stable ICU 4.4
 */
U_CAPI int32_t U_EXPORT2
uldn_ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਅਚਲ ULocale_Display_ਨਾਂ *ldn,
                         ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ,
                         ਅਚਲ ਅੱਖਰ *ਮੁੱਲ_2,
                         UChar *ਨਤੀਜਾ,
                         int32_t max_ਨਤੀਜਾ_ਮਾਪ,
                         UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
* Returns an instance of LocaleDisplayNames that returns names formatted
* for the provided locale, using the provided UDisplayContext settings.
*
* @param locale The display locale 
* @param contexts List of one or more context settings (e.g. for dialect
*               handling, capitalization, etc.
* @param length Number of items in the contexts list
* @param pErrorCode Pointer to UErrorCode input/output status. If at entry this indicates
*               a failure status, the function will do nothing; otherwise this will be
*               updated with any new status from the function. 
* @return a ULocaleDisplayNames instance 
* @stable ICU 51
*/
U_CAPI ULocale_Display_ਨਾਂ * U_EXPORT2
uldn_openForContext(ਅਚਲ ਅੱਖਰ * locale, UDisplayContext *contexts,
                    int32_t ਲੰਮਾਈ, UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
* Returns the UDisplayContext value for the specified UDisplayContextType.
* @param ldn the ULocaleDisplayNames instance
* @param type the UDisplayContextType whose value to return
* @param pErrorCode Pointer to UErrorCode input/output status. If at entry this indicates
*               a failure status, the function will do nothing; otherwise this will be
*               updated with any new status from the function. 
* @return the UDisplayContextValue for the specified type.
* @stable ICU 51
*/
U_CAPI UDisplayContext U_EXPORT2
uldn_getContext(ਅਚਲ ULocale_Display_ਨਾਂ *ldn, UDisplayContextType type,
                UError_ਕੋਡ *p_Error_ਕੋਡ);

#ਜੇ_ਅੰਤ  /* !UCONFIG_NO_FORMATTING */
#ਜੇ_ਅੰਤ  /* __ULDNAMES_H__ */
