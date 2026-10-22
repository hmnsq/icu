// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#اگر_غیر_تعریف_شدہ LOCDSPNM_H
#تعریف_کرو LOCDSPNM_H

#شامل "unicode/utypes.h"

#اگر U_SHOW_CPLUSPLUS_API

/**
 * \file
 * \brief C++ API: Provides display names of Locale and its components.
 */

#اگر !UCONFIG_NO_FORMATTING

#شامل "unicode/locid.h"
#شامل "unicode/strenum.h"
#شامل "unicode/uscript.h"
#شامل "unicode/uldnames.h"
#شامل "unicode/udisplaycontext.h"

U_NAMESPACE_BEGIN

/**
 * Returns display names of Locales and components of Locales. For
 * more information on language, script, region, variant, key, and
 * values, see Locale.
 * @stable ICU 4.4
 */
class U_COMMON_API Locale_Display_نام : عوامی UObject {
عوامی:
    /**
     * Destructor.
     * @stable ICU 4.4
     */
    مجازی ~Locale_Display_نام();

    /**
     * Convenience overload of
     * {@link #createInstance(const Locale& locale, UDialectHandling dialectHandling)}
     * that specifies STANDARD dialect handling.
     * @param locale the display locale
     * @return a LocaleDisplayNames instance
     * @stable ICU 4.4
     */
    اندرونی ساکن Locale_Display_نام* U_EXPORT2 createInstance(ثابت Locale& locale);

    /**
     * Returns an instance of LocaleDisplayNames that returns names
     * formatted for the provided locale, using the provided
     * dialectHandling.
     *
     * @param locale the display locale
     * @param dialectHandling how to select names for locales
     * @return a LocaleDisplayNames instance
     * @stable ICU 4.4
     */
    ساکن Locale_Display_نام* U_EXPORT2 createInstance(ثابت Locale& locale,
                            UDialectHandling dialectHandling);

    /**
     * Returns an instance of LocaleDisplayNames that returns names formatted
     * for the provided locale, using the provided UDisplayContext settings.
     *
     * @param locale the display locale
     * @param contexts List of one or more context settings (e.g. for dialect
     *               handling, capitalization, etc.
     * @param length Number of items in the contexts list
     * @return a LocaleDisplayNames instance
     * @stable ICU 51
     */
    ساکن Locale_Display_نام* U_EXPORT2 createInstance(ثابت Locale& locale,
                            UDisplayContext *contexts, int32_t لمبائی);

    // getters for state
    /**
     * Returns the locale used to determine the display names. This is
     * not necessarily the same locale passed to {@link #createInstance}.
     * @return the display locale
     * @stable ICU 4.4
     */
    مجازی ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND = 0;

    /**
     * Returns the dialect handling used in the display names.
     * @return the dialect handling enum
     * @stable ICU 4.4
     */
    مجازی UDialectHandling getDialectHandling() ثابت = 0;

    /**
     * Returns the UDisplayContext value for the specified UDisplayContextType.
     * @param type the UDisplayContextType whose value to return
     * @return the UDisplayContext for the specified type.
     * @stable ICU 51
     */
    مجازی UDisplayContext getContext(UDisplayContextType type) ثابت = 0;

    // names for entire locales
    /**
     * Returns the display name of the provided locale.
     * @param locale the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    مجازی UnicodeString& localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided locale id.
     * @param localeId the id of the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    مجازی UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت = 0;

    // names for components of a locale id
    /**
     * Returns the display name of the provided language code.
     * @param lang the language code
     * @param result receives the language code's display name
     * @return the display name of the provided language code
     * @stable ICU 4.4
     */
    مجازی UnicodeString& languageDisplayName(ثابت حرف* lang,
                                               UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided script code.
     * @param script the script code
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    مجازی UnicodeString& scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided script code.
     * @param scriptCode the script code number
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    مجازی UnicodeString& scriptDisplayName(UScript_رمز script_رمز,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided region code.
     * @param region the region code
     * @param result receives the region code's display name
     * @return the display name of the provided region code
     * @stable ICU 4.4
     */
    مجازی UnicodeString& regionDisplayName(ثابت حرف* region,
                                             UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided variant.
     * @param variant the variant string
     * @param result receives the variant's display name
     * @return the display name of the provided variant
     * @stable ICU 4.4
     */
    مجازی UnicodeString& variantDisplayName(ثابت حرف* variant,
                                              UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided locale key.
     * @param key the locale key name
     * @param result receives the locale key's display name
     * @return the display name of the provided locale key
     * @stable ICU 4.4
     */
    مجازی UnicodeString& کلید_Display_Name(ثابت حرف* کلید,
                                          UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided value (used with the provided key).
     * @param key the locale key name
     * @param value the locale key's value
     * @param result receives the value's display name
     * @return the display name of the provided value
     * @stable ICU 4.4
     */
    مجازی UnicodeString& کلید_قدر_Display_Name(ثابت حرف* کلید, ثابت حرف* قدر_2,
                                               UnicodeString& نتیجہ U_LIFETIME_BOUND) ثابت = 0;
};

اندرونی Locale_Display_نام* Locale_Display_نام::createInstance(ثابت Locale& locale) {
  واپس Locale_Display_نام::createInstance(locale, ULDN_STANDARD_نام);
}

U_NAMESPACE_انتہا

#اگر_ختم

#اگر_ختم /* U_SHOW_CPLUSPLUS_API */

#اگر_ختم
