// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2016, International Business Machines Corporation and
*   others.  All Rights Reserved.
*******************************************************************************
*/

#నిర్వచితం_కాకపోతే __ULDNAMES_H__
#నిర్వచించు __ULDNAMES_H__

/**
 * \file
 * \brief C API: Provides display names of Locale ids and their components.
 */

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "unicode/udisplaycontext.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API
#అన్తర్భావ "unicode/localpointer.h"
#అయితే_ముగింపు   // U_SHOW_CPLUSPLUS_API

/**
 * Enum used in LocaleDisplayNames::createInstance.
 * @stable ICU 4.4
 */
రకనిర్వచనం గణన {
    /**
     * Use standard names when generating a locale name,
     * e.g. en_GB displays as 'English (United Kingdom)'.
     * @stable ICU 4.4
     */
    ULDN_STANDARD_పేర్లు = 0,
    /**
     * Use dialect names, when generating a locale name,
     * e.g. en_GB displays as 'British English'.
     * @stable ICU 4.4
     */
    ULDN_DIALECT_పేర్లు
} UDialectHandling;

/**
 * Opaque C service object type for the locale display names API
 * @stable ICU 4.4
 */
నిర్మాణం ULocale_Display_పేర్లు;

/** 
 * C typedef for struct ULocaleDisplayNames. 
 * @stable ICU 4.4 
 */
రకనిర్వచనం నిర్మాణం ULocale_Display_పేర్లు ULocale_Display_పేర్లు;  

#ఒకవేళ !UCONFIG_NO_FORMATTING

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
U_CAPI ULocale_Display_పేర్లు * U_EXPORT2
uldn_open(స్థిరరాశి అక్షరం * locale,
          UDialectHandling dialectHandling,
          UError_సంకేతం *p_Error_సంకేతం);

/**
 * Closes a ULocaleDisplayNames instance obtained from uldn_open().
 * @param ldn the ULocaleDisplayNames instance to be closed
 * @stable ICU 4.4
 */
U_CAPI శూన్య U_EXPORT2
uldn_close(ULocale_Display_పేర్లు *ldn);

#ఒకవేళ U_SHOW_CPLUSPLUS_API

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
U_DEFINE_LOCAL_OPEN_సూచిక(Local_ULocale_Display_పేర్లు_సూచిక, ULocale_Display_పేర్లు, uldn_close);

U_NAMESPACE_చివర

#అయితే_ముగింపు

/* getters for state */

/**
 * Returns the locale used to determine the display names. This is
 * not necessarily the same locale passed to {@link #uldn_open}.
 * @param ldn the LocaleDisplayNames instance
 * @return the display locale 
 * @stable ICU 4.4
 */
U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uldn_getLocale(స్థిరరాశి ULocale_Display_పేర్లు* ldn U_LIFETIME_BOUND);

/**
 * Returns the dialect handling used in the display names.
 * @param ldn the LocaleDisplayNames instance
 * @return the dialect handling enum
 * @stable ICU 4.4
 */
U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(స్థిరరాశి ULocale_Display_పేర్లు *ldn);

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
uldn_localeDisplayName(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                       స్థిరరాశి అక్షరం *locale,
                       UChar *ఫలితం,
                       int32_t max_ఫలితం_కొలత,
                       UError_సంకేతం *p_Error_సంకేతం);

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
uldn_languageDisplayName(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                         స్థిరరాశి అక్షరం *lang,
                         UChar *ఫలితం,
                         int32_t max_ఫలితం_కొలత,
                         UError_సంకేతం *p_Error_సంకేతం);

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
uldn_scriptDisplayName(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                       స్థిరరాశి అక్షరం *script,
                       UChar *ఫలితం,
                       int32_t max_ఫలితం_కొలత,
                       UError_సంకేతం *p_Error_సంకేతం);

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
uldn_script_సంకేతం_Display_Name(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                           UScript_సంకేతం script_సంకేతం,
                           UChar *ఫలితం,
                           int32_t max_ఫలితం_కొలత,
                           UError_సంకేతం *p_Error_సంకేతం);

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
uldn_regionDisplayName(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                       స్థిరరాశి అక్షరం *region,
                       UChar *ఫలితం,
                       int32_t max_ఫలితం_కొలత,
                       UError_సంకేతం *p_Error_సంకేతం);

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
uldn_variantDisplayName(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                        స్థిరరాశి అక్షరం *variant,
                        UChar *ఫలితం,
                        int32_t max_ఫలితం_కొలత,
                        UError_సంకేతం *p_Error_సంకేతం);

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
uldn_కీలకం_Display_Name(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                    స్థిరరాశి అక్షరం *కీలకం,
                    UChar *ఫలితం,
                    int32_t max_ఫలితం_కొలత,
                    UError_సంకేతం *p_Error_సంకేతం);

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
uldn_కీలకం_విలువ_Display_Name(స్థిరరాశి ULocale_Display_పేర్లు *ldn,
                         స్థిరరాశి అక్షరం *కీలకం,
                         స్థిరరాశి అక్షరం *విలువ_2,
                         UChar *ఫలితం,
                         int32_t max_ఫలితం_కొలత,
                         UError_సంకేతం *p_Error_సంకేతం);

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
U_CAPI ULocale_Display_పేర్లు * U_EXPORT2
uldn_openForContext(స్థిరరాశి అక్షరం * locale, UDisplayContext *contexts,
                    int32_t నిడివి, UError_సంకేతం *p_Error_సంకేతం);

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
uldn_getContext(స్థిరరాశి ULocale_Display_పేర్లు *ldn, UDisplayContextType type,
                UError_సంకేతం *p_Error_సంకేతం);

#అయితే_ముగింపు  /* !UCONFIG_NO_FORMATTING */
#అయితే_ముగింపు  /* __ULDNAMES_H__ */
