// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2016, International Business Machines Corporation and
*   others.  All Rights Reserved.
*******************************************************************************
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ULDNAMES_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __ULDNAMES_H__

/**
 * \file
 * \brief C API: Provides display names of Locale ids and their components.
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "unicode/udisplaycontext.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಆದರೆ_ಅಂತ್ಯ   // U_SHOW_CPLUSPLUS_API

/**
 * Enum used in LocaleDisplayNames::createInstance.
 * @stable ICU 4.4
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    /**
     * Use standard names when generating a locale name,
     * e.g. en_GB displays as 'English (United Kingdom)'.
     * @stable ICU 4.4
     */
    ULDN_STANDARD_ಹೆಸರುಗಳು = 0,
    /**
     * Use dialect names, when generating a locale name,
     * e.g. en_GB displays as 'British English'.
     * @stable ICU 4.4
     */
    ULDN_DIALECT_ಹೆಸರುಗಳು
} UDialectHandling;

/**
 * Opaque C service object type for the locale display names API
 * @stable ICU 4.4
 */
ರಚನೆ ULocale_Display_ಹೆಸರುಗಳು;

/** 
 * C typedef for struct ULocaleDisplayNames. 
 * @stable ICU 4.4 
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ULocale_Display_ಹೆಸರುಗಳು ULocale_Display_ಹೆಸರುಗಳು;  

#ಆದರೆ !UCONFIG_NO_FORMATTING

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
U_CAPI ULocale_Display_ಹೆಸರುಗಳು * U_EXPORT2
uldn_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * locale,
          UDialectHandling dialectHandling,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/**
 * Closes a ULocaleDisplayNames instance obtained from uldn_open().
 * @param ldn the ULocaleDisplayNames instance to be closed
 * @stable ICU 4.4
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
uldn_close(ULocale_Display_ಹೆಸರುಗಳು *ldn);

#ಆದರೆ U_SHOW_CPLUSPLUS_API

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
U_DEFINE_LOCAL_OPEN_ಸೂಚಕ(Local_ULocale_Display_ಹೆಸರುಗಳು_ಸೂಚಕ, ULocale_Display_ಹೆಸರುಗಳು, uldn_close);

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ

/* getters for state */

/**
 * Returns the locale used to determine the display names. This is
 * not necessarily the same locale passed to {@link #uldn_open}.
 * @param ldn the LocaleDisplayNames instance
 * @return the display locale 
 * @stable ICU 4.4
 */
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uldn_getLocale(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು* ldn U_LIFETIME_BOUND);

/**
 * Returns the dialect handling used in the display names.
 * @param ldn the LocaleDisplayNames instance
 * @return the dialect handling enum
 * @stable ICU 4.4
 */
U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn);

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
uldn_localeDisplayName(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
                       UChar *ಫಲಿತಾಂಶ,
                       int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

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
uldn_languageDisplayName(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lang,
                         UChar *ಫಲಿತಾಂಶ,
                         int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                         UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

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
uldn_scriptDisplayName(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *script,
                       UChar *ಫಲಿತಾಂಶ,
                       int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

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
uldn_script_ಸಂಕೇತ_Display_Name(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                           UScript_ಸಂಕೇತ script_ಸಂಕೇತ,
                           UChar *ಫಲಿತಾಂಶ,
                           int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

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
uldn_regionDisplayName(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *region,
                       UChar *ಫಲಿತಾಂಶ,
                       int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

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
uldn_variantDisplayName(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variant,
                        UChar *ಫಲಿತಾಂಶ,
                        int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                        UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

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
uldn_ಕೀಲಿ_Display_Name(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ,
                    UChar *ಫಲಿತಾಂಶ,
                    int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

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
uldn_ಕೀಲಿ_ಮೌಲ್ಯ_Display_Name(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ_2,
                         UChar *ಫಲಿತಾಂಶ,
                         int32_t max_ಫಲಿತಾಂಶ_ಗಾತ್ರ,
                         UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

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
U_CAPI ULocale_Display_ಹೆಸರುಗಳು * U_EXPORT2
uldn_openForContext(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * locale, UDisplayContext *contexts,
                    int32_t ಉದ್ದಳತೆ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

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
uldn_getContext(ಸ್ಥಿರಾಂಕ ULocale_Display_ಹೆಸರುಗಳು *ldn, UDisplayContextType type,
                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

#ಆದರೆ_ಅಂತ್ಯ  /* !UCONFIG_NO_FORMATTING */
#ಆದರೆ_ಅಂತ್ಯ  /* __ULDNAMES_H__ */
