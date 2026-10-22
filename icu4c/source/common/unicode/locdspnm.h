// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ifndef LOCDSPNM_H
#define LOCDSPNM_H

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_SHOW_CPLUSPLUS_API

/**
 * \file
 * \brief C++ API: Provides display names of Locale and its components.
 */

#যদি !UCONFIG_NO_FORMATTING

#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/strenum.h"
#অন্তর্ভাব "unicode/uscript.h"
#অন্তর্ভাব "unicode/uldnames.h"
#অন্তর্ভাব "unicode/udisplaycontext.h"

U_NAMESPACE_BEGIN

/**
 * Returns display names of Locales and components of Locales. For
 * more information on language, script, region, variant, key, and
 * values, see Locale.
 * @stable ICU 4.4
 */
class U_COMMON_API LocaleDisplayNames : সর্বজনীন UObject {
সর্বজনীন:
    /**
     * Destructor.
     * @stable ICU 4.4
     */
    আভাসী ~LocaleDisplayNames();

    /**
     * Convenience overload of
     * {@link #createInstance(const Locale& locale, UDialectHandling dialectHandling)}
     * that specifies STANDARD dialect handling.
     * @param locale the display locale
     * @return a LocaleDisplayNames instance
     * @stable ICU 4.4
     */
    অন্তঃরেখ স্থির LocaleDisplayNames* U_EXPORT2 createInstance(ধ্রুবক Locale& locale);

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
    স্থির LocaleDisplayNames* U_EXPORT2 createInstance(ধ্রুবক Locale& locale,
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
    স্থির LocaleDisplayNames* U_EXPORT2 createInstance(ধ্রুবক Locale& locale,
                            UDisplayContext *contexts, int32_t দৈর্ঘ্য);

    // getters for state
    /**
     * Returns the locale used to determine the display names. This is
     * not necessarily the same locale passed to {@link #createInstance}.
     * @return the display locale
     * @stable ICU 4.4
     */
    আভাসী ধ্রুবক Locale& getLocale() ধ্রুবক U_LIFETIME_BOUND = 0;

    /**
     * Returns the dialect handling used in the display names.
     * @return the dialect handling enum
     * @stable ICU 4.4
     */
    আভাসী UDialectHandling getDialectHandling() ধ্রুবক = 0;

    /**
     * Returns the UDisplayContext value for the specified UDisplayContextType.
     * @param type the UDisplayContextType whose value to return
     * @return the UDisplayContext for the specified type.
     * @stable ICU 51
     */
    আভাসী UDisplayContext getContext(UDisplayContextType type) ধ্রুবক = 0;

    // names for entire locales
    /**
     * Returns the display name of the provided locale.
     * @param locale the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    আভাসী UnicodeString& localeDisplayName(ধ্রুবক Locale& locale,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক = 0;

    /**
     * Returns the display name of the provided locale id.
     * @param localeId the id of the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    আভাসী UnicodeString& localeDisplayName(ধ্রুবক অক্ষর* localeId,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক = 0;

    // names for components of a locale id
    /**
     * Returns the display name of the provided language code.
     * @param lang the language code
     * @param result receives the language code's display name
     * @return the display name of the provided language code
     * @stable ICU 4.4
     */
    আভাসী UnicodeString& languageDisplayName(ধ্রুবক অক্ষর* lang,
                                               UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক = 0;

    /**
     * Returns the display name of the provided script code.
     * @param script the script code
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    আভাসী UnicodeString& scriptDisplayName(ধ্রুবক অক্ষর* script,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক = 0;

    /**
     * Returns the display name of the provided script code.
     * @param scriptCode the script code number
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    আভাসী UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক = 0;

    /**
     * Returns the display name of the provided region code.
     * @param region the region code
     * @param result receives the region code's display name
     * @return the display name of the provided region code
     * @stable ICU 4.4
     */
    আভাসী UnicodeString& regionDisplayName(ধ্রুবক অক্ষর* region,
                                             UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক = 0;

    /**
     * Returns the display name of the provided variant.
     * @param variant the variant string
     * @param result receives the variant's display name
     * @return the display name of the provided variant
     * @stable ICU 4.4
     */
    আভাসী UnicodeString& variantDisplayName(ধ্রুবক অক্ষর* variant,
                                              UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক = 0;

    /**
     * Returns the display name of the provided locale key.
     * @param key the locale key name
     * @param result receives the locale key's display name
     * @return the display name of the provided locale key
     * @stable ICU 4.4
     */
    আভাসী UnicodeString& চাবি_Display_Name(ধ্রুবক অক্ষর* চাবি,
                                          UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক = 0;

    /**
     * Returns the display name of the provided value (used with the provided key).
     * @param key the locale key name
     * @param value the locale key's value
     * @param result receives the value's display name
     * @return the display name of the provided value
     * @stable ICU 4.4
     */
    আভাসী UnicodeString& চাবি_মান_Display_Name(ধ্রুবক অক্ষর* চাবি, ধ্রুবক অক্ষর* মান_2,
                                               UnicodeString& ফলাফল U_LIFETIME_BOUND) ধ্রুবক = 0;
};

অন্তঃরেখ LocaleDisplayNames* LocaleDisplayNames::createInstance(ধ্রুবক Locale& locale) {
  ফেরত LocaleDisplayNames::createInstance(locale, ULDN_STANDARD_NAMES);
}

U_NAMESPACE_END

#endif

#endif /* U_SHOW_CPLUSPLUS_API */

#endif
