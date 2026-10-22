// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#నిర్వచితం_కాకపోతే LOCDSPNM_H
#నిర్వచించు LOCDSPNM_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

/**
 * \file
 * \brief C++ API: Provides display names of Locale and its components.
 */

#ఒకవేళ !UCONFIG_NO_FORMATTING

#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/strenum.h"
#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "unicode/uldnames.h"
#అన్తర్భావ "unicode/udisplaycontext.h"

U_NAMESPACE_BEGIN

/**
 * Returns display names of Locales and components of Locales. For
 * more information on language, script, region, variant, key, and
 * values, see Locale.
 * @stable ICU 4.4
 */
class U_COMMON_API Locale_Display_పేర్లు : బహిరంగ UObject {
బహిరంగ:
    /**
     * Destructor.
     * @stable ICU 4.4
     */
    మాయా ~Locale_Display_పేర్లు();

    /**
     * Convenience overload of
     * {@link #createInstance(const Locale& locale, UDialectHandling dialectHandling)}
     * that specifies STANDARD dialect handling.
     * @param locale the display locale
     * @return a LocaleDisplayNames instance
     * @stable ICU 4.4
     */
    అంతఃవరుస స్థిర Locale_Display_పేర్లు* U_EXPORT2 createInstance(స్థిరరాశి Locale& locale);

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
    స్థిర Locale_Display_పేర్లు* U_EXPORT2 createInstance(స్థిరరాశి Locale& locale,
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
    స్థిర Locale_Display_పేర్లు* U_EXPORT2 createInstance(స్థిరరాశి Locale& locale,
                            UDisplayContext *contexts, int32_t నిడివి);

    // getters for state
    /**
     * Returns the locale used to determine the display names. This is
     * not necessarily the same locale passed to {@link #createInstance}.
     * @return the display locale
     * @stable ICU 4.4
     */
    మాయా స్థిరరాశి Locale& getLocale() స్థిరరాశి U_LIFETIME_BOUND = 0;

    /**
     * Returns the dialect handling used in the display names.
     * @return the dialect handling enum
     * @stable ICU 4.4
     */
    మాయా UDialectHandling getDialectHandling() స్థిరరాశి = 0;

    /**
     * Returns the UDisplayContext value for the specified UDisplayContextType.
     * @param type the UDisplayContextType whose value to return
     * @return the UDisplayContext for the specified type.
     * @stable ICU 51
     */
    మాయా UDisplayContext getContext(UDisplayContextType type) స్థిరరాశి = 0;

    // names for entire locales
    /**
     * Returns the display name of the provided locale.
     * @param locale the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    మాయా UnicodeString& localeDisplayName(స్థిరరాశి Locale& locale,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి = 0;

    /**
     * Returns the display name of the provided locale id.
     * @param localeId the id of the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    మాయా UnicodeString& localeDisplayName(స్థిరరాశి అక్షరం* localeId,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి = 0;

    // names for components of a locale id
    /**
     * Returns the display name of the provided language code.
     * @param lang the language code
     * @param result receives the language code's display name
     * @return the display name of the provided language code
     * @stable ICU 4.4
     */
    మాయా UnicodeString& languageDisplayName(స్థిరరాశి అక్షరం* lang,
                                               UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి = 0;

    /**
     * Returns the display name of the provided script code.
     * @param script the script code
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    మాయా UnicodeString& scriptDisplayName(స్థిరరాశి అక్షరం* script,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి = 0;

    /**
     * Returns the display name of the provided script code.
     * @param scriptCode the script code number
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    మాయా UnicodeString& scriptDisplayName(UScript_సంకేతం script_సంకేతం,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి = 0;

    /**
     * Returns the display name of the provided region code.
     * @param region the region code
     * @param result receives the region code's display name
     * @return the display name of the provided region code
     * @stable ICU 4.4
     */
    మాయా UnicodeString& regionDisplayName(స్థిరరాశి అక్షరం* region,
                                             UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి = 0;

    /**
     * Returns the display name of the provided variant.
     * @param variant the variant string
     * @param result receives the variant's display name
     * @return the display name of the provided variant
     * @stable ICU 4.4
     */
    మాయా UnicodeString& variantDisplayName(స్థిరరాశి అక్షరం* variant,
                                              UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి = 0;

    /**
     * Returns the display name of the provided locale key.
     * @param key the locale key name
     * @param result receives the locale key's display name
     * @return the display name of the provided locale key
     * @stable ICU 4.4
     */
    మాయా UnicodeString& కీలకం_Display_Name(స్థిరరాశి అక్షరం* కీలకం,
                                          UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి = 0;

    /**
     * Returns the display name of the provided value (used with the provided key).
     * @param key the locale key name
     * @param value the locale key's value
     * @param result receives the value's display name
     * @return the display name of the provided value
     * @stable ICU 4.4
     */
    మాయా UnicodeString& కీలకం_విలువ_Display_Name(స్థిరరాశి అక్షరం* కీలకం, స్థిరరాశి అక్షరం* విలువ_2,
                                               UnicodeString& ఫలితం U_LIFETIME_BOUND) స్థిరరాశి = 0;
};

అంతఃవరుస Locale_Display_పేర్లు* Locale_Display_పేర్లు::createInstance(స్థిరరాశి Locale& locale) {
  తిరిగి Locale_Display_పేర్లు::createInstance(locale, ULDN_STANDARD_పేర్లు);
}

U_NAMESPACE_చివర

#అయితే_ముగింపు

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు
