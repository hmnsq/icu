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

#अन्तर्भाव "unicode/utypes.h"

#सचे U_SHOW_CPLUSPLUS_API

/**
 * \file
 * \brief C++ API: Provides display names of Locale and its components.
 */

#सचे !UCONFIG_NO_FORMATTING

#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/strenum.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "unicode/uldnames.h"
#अन्तर्भाव "unicode/udisplaycontext.h"

U_NAMESPACE_BEGIN

/**
 * Returns display names of Locales and components of Locales. For
 * more information on language, script, region, variant, key, and
 * values, see Locale.
 * @stable ICU 4.4
 */
class U_COMMON_API LocaleDisplayNames : सार्वजनिक UObject {
सार्वजनिक:
    /**
     * Destructor.
     * @stable ICU 4.4
     */
    आभास ~LocaleDisplayNames();

    /**
     * Convenience overload of
     * {@link #createInstance(const Locale& locale, UDialectHandling dialectHandling)}
     * that specifies STANDARD dialect handling.
     * @param locale the display locale
     * @return a LocaleDisplayNames instance
     * @stable ICU 4.4
     */
    अन्तःस्थ स्थिर LocaleDisplayNames* U_EXPORT2 createInstance(ध्रुव Locale& locale);

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
    स्थिर LocaleDisplayNames* U_EXPORT2 createInstance(ध्रुव Locale& locale,
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
    स्थिर LocaleDisplayNames* U_EXPORT2 createInstance(ध्रुव Locale& locale,
                            UDisplayContext *contexts, int32_t length);

    // getters for state
    /**
     * Returns the locale used to determine the display names. This is
     * not necessarily the same locale passed to {@link #createInstance}.
     * @return the display locale
     * @stable ICU 4.4
     */
    आभास ध्रुव Locale& getLocale() ध्रुव U_LIFETIME_BOUND = 0;

    /**
     * Returns the dialect handling used in the display names.
     * @return the dialect handling enum
     * @stable ICU 4.4
     */
    आभास UDialectHandling getDialectHandling() ध्रुव = 0;

    /**
     * Returns the UDisplayContext value for the specified UDisplayContextType.
     * @param type the UDisplayContextType whose value to return
     * @return the UDisplayContext for the specified type.
     * @stable ICU 51
     */
    आभास UDisplayContext getContext(UDisplayContextType type) ध्रुव = 0;

    // names for entire locales
    /**
     * Returns the display name of the provided locale.
     * @param locale the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    आभास UnicodeString& localeDisplayName(ध्रुव Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव = 0;

    /**
     * Returns the display name of the provided locale id.
     * @param localeId the id of the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    आभास UnicodeString& localeDisplayName(ध्रुव वर्ण* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव = 0;

    // names for components of a locale id
    /**
     * Returns the display name of the provided language code.
     * @param lang the language code
     * @param result receives the language code's display name
     * @return the display name of the provided language code
     * @stable ICU 4.4
     */
    आभास UnicodeString& languageDisplayName(ध्रुव वर्ण* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) ध्रुव = 0;

    /**
     * Returns the display name of the provided script code.
     * @param script the script code
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    आभास UnicodeString& scriptDisplayName(ध्रुव वर्ण* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव = 0;

    /**
     * Returns the display name of the provided script code.
     * @param scriptCode the script code number
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    आभास UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव = 0;

    /**
     * Returns the display name of the provided region code.
     * @param region the region code
     * @param result receives the region code's display name
     * @return the display name of the provided region code
     * @stable ICU 4.4
     */
    आभास UnicodeString& regionDisplayName(ध्रुव वर्ण* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ध्रुव = 0;

    /**
     * Returns the display name of the provided variant.
     * @param variant the variant string
     * @param result receives the variant's display name
     * @return the display name of the provided variant
     * @stable ICU 4.4
     */
    आभास UnicodeString& variantDisplayName(ध्रुव वर्ण* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) ध्रुव = 0;

    /**
     * Returns the display name of the provided locale key.
     * @param key the locale key name
     * @param result receives the locale key's display name
     * @return the display name of the provided locale key
     * @stable ICU 4.4
     */
    आभास UnicodeString& कुञ्चिका_Display_Name(ध्रुव वर्ण* कुञ्चिका,
                                          UnicodeString& result U_LIFETIME_BOUND) ध्रुव = 0;

    /**
     * Returns the display name of the provided value (used with the provided key).
     * @param key the locale key name
     * @param value the locale key's value
     * @param result receives the value's display name
     * @return the display name of the provided value
     * @stable ICU 4.4
     */
    आभास UnicodeString& कुञ्चिका_मानम्_Display_Name(ध्रुव वर्ण* कुञ्चिका, ध्रुव वर्ण* मानम्_2,
                                               UnicodeString& result U_LIFETIME_BOUND) ध्रुव = 0;
};

अन्तःस्थ LocaleDisplayNames* LocaleDisplayNames::createInstance(ध्रुव Locale& locale) {
  पटिनिवत्त LocaleDisplayNames::createInstance(locale, ULDN_STANDARD_NAMES);
}

U_NAMESPACE_END

#endif

#endif /* U_SHOW_CPLUSPLUS_API */

#endif
