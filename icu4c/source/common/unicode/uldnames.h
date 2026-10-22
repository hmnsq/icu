// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2016, International Business Machines Corporation and
*   others.  All Rights Reserved.
*******************************************************************************
*/

#که_نه_تعریف_شوی __ULDNAMES_H__
#تعریف_کړه __ULDNAMES_H__

/**
 * \file
 * \brief C API: Provides display names of Locale ids and their components.
 */

#ضمن "unicode/utypes.h"
#ضمن "unicode/uscript.h"
#ضمن "unicode/udisplaycontext.h"

#إذا U_SHOW_CPLUSPLUS_API
#ضمن "unicode/localpointer.h"
#د_که_پای   // U_SHOW_CPLUSPLUS_API

/**
 * Enum used in LocaleDisplayNames::createInstance.
 * @stable ICU 4.4
 */
تعريفنوع تعداد {
    /**
     * Use standard names when generating a locale name,
     * e.g. en_GB displays as 'English (United Kingdom)'.
     * @stable ICU 4.4
     */
    ULDN_STANDARD_نومونه = 0,
    /**
     * Use dialect names, when generating a locale name,
     * e.g. en_GB displays as 'British English'.
     * @stable ICU 4.4
     */
    ULDN_DIALECT_نومونه
} UDialectHandling;

/**
 * Opaque C service object type for the locale display names API
 * @stable ICU 4.4
 */
بنية ULocale_Display_نومونه;

/** 
 * C typedef for struct ULocaleDisplayNames. 
 * @stable ICU 4.4 
 */
تعريفنوع بنية ULocale_Display_نومونه ULocale_Display_نومونه;  

#إذا !UCONFIG_NO_FORMATTING

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
U_CAPI ULocale_Display_نومونه * U_EXPORT2
uldn_open(ثابت حرف * locale,
          UDialectHandling dialectHandling,
          UError_کوډ *p_Error_کوډ);

/**
 * Closes a ULocaleDisplayNames instance obtained from uldn_open().
 * @param ldn the ULocaleDisplayNames instance to be closed
 * @stable ICU 4.4
 */
U_CAPI فراغ U_EXPORT2
uldn_close(ULocale_Display_نومونه *ldn);

#إذا U_SHOW_CPLUSPLUS_API

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
U_DEFINE_LOCAL_OPEN_اشاره_ګر(Local_ULocale_Display_نومونه_اشاره_ګر, ULocale_Display_نومونه, uldn_close);

U_NAMESPACE_پای_ټکی

#د_که_پای

/* getters for state */

/**
 * Returns the locale used to determine the display names. This is
 * not necessarily the same locale passed to {@link #uldn_open}.
 * @param ldn the LocaleDisplayNames instance
 * @return the display locale 
 * @stable ICU 4.4
 */
U_CAPI ثابت حرف* U_EXPORT2
uldn_getLocale(ثابت ULocale_Display_نومونه* ldn U_LIFETIME_BOUND);

/**
 * Returns the dialect handling used in the display names.
 * @param ldn the LocaleDisplayNames instance
 * @return the dialect handling enum
 * @stable ICU 4.4
 */
U_CAPI UDialectHandling U_EXPORT2
uldn_getDialectHandling(ثابت ULocale_Display_نومونه *ldn);

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
uldn_localeDisplayName(ثابت ULocale_Display_نومونه *ldn,
                       ثابت حرف *locale,
                       UChar *پایله,
                       int32_t max_پایله_اندازه,
                       UError_کوډ *p_Error_کوډ);

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
uldn_languageDisplayName(ثابت ULocale_Display_نومونه *ldn,
                         ثابت حرف *lang,
                         UChar *پایله,
                         int32_t max_پایله_اندازه,
                         UError_کوډ *p_Error_کوډ);

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
uldn_scriptDisplayName(ثابت ULocale_Display_نومونه *ldn,
                       ثابت حرف *script,
                       UChar *پایله,
                       int32_t max_پایله_اندازه,
                       UError_کوډ *p_Error_کوډ);

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
uldn_script_کوډ_Display_Name(ثابت ULocale_Display_نومونه *ldn,
                           UScript_کوډ script_کوډ,
                           UChar *پایله,
                           int32_t max_پایله_اندازه,
                           UError_کوډ *p_Error_کوډ);

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
uldn_regionDisplayName(ثابت ULocale_Display_نومونه *ldn,
                       ثابت حرف *region,
                       UChar *پایله,
                       int32_t max_پایله_اندازه,
                       UError_کوډ *p_Error_کوډ);

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
uldn_variantDisplayName(ثابت ULocale_Display_نومونه *ldn,
                        ثابت حرف *variant,
                        UChar *پایله,
                        int32_t max_پایله_اندازه,
                        UError_کوډ *p_Error_کوډ);

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
uldn_کلی_Display_Name(ثابت ULocale_Display_نومونه *ldn,
                    ثابت حرف *کلی,
                    UChar *پایله,
                    int32_t max_پایله_اندازه,
                    UError_کوډ *p_Error_کوډ);

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
uldn_کلی_ارزښت_Display_Name(ثابت ULocale_Display_نومونه *ldn,
                         ثابت حرف *کلی,
                         ثابت حرف *ارزښت_2,
                         UChar *پایله,
                         int32_t max_پایله_اندازه,
                         UError_کوډ *p_Error_کوډ);

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
U_CAPI ULocale_Display_نومونه * U_EXPORT2
uldn_openForContext(ثابت حرف * locale, UDisplayContext *contexts,
                    int32_t اوږدتیا, UError_کوډ *p_Error_کوډ);

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
uldn_getContext(ثابت ULocale_Display_نومونه *ldn, UDisplayContextType type,
                UError_کوډ *p_Error_کوډ);

#د_که_پای  /* !UCONFIG_NO_FORMATTING */
#د_که_پای  /* __ULDNAMES_H__ */
