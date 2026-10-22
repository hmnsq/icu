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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ U_SHOW_CPLUSPLUS_API

/**
 * \file
 * \brief C++ API: Provides display names of Locale and its components.
 */

#എങ്കിൽ !UCONFIG_NO_FORMATTING

#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/strenum.h"
#അന്തര്ഭാവ "unicode/uscript.h"
#അന്തര്ഭാവ "unicode/uldnames.h"
#അന്തര്ഭാവ "unicode/udisplaycontext.h"

U_NAMESPACE_BEGIN

/**
 * Returns display names of Locales and components of Locales. For
 * more information on language, script, region, variant, key, and
 * values, see Locale.
 * @stable ICU 4.4
 */
class U_COMMON_API LocaleDisplayNames : പൊതു UObject {
പൊതു:
    /**
     * Destructor.
     * @stable ICU 4.4
     */
    മായാ ~LocaleDisplayNames();

    /**
     * Convenience overload of
     * {@link #createInstance(const Locale& locale, UDialectHandling dialectHandling)}
     * that specifies STANDARD dialect handling.
     * @param locale the display locale
     * @return a LocaleDisplayNames instance
     * @stable ICU 4.4
     */
    വരിയിൽ സ്ഥിതം LocaleDisplayNames* U_EXPORT2 createInstance(സ്ഥിരം Locale& locale);

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
    സ്ഥിതം LocaleDisplayNames* U_EXPORT2 createInstance(സ്ഥിരം Locale& locale,
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
    സ്ഥിതം LocaleDisplayNames* U_EXPORT2 createInstance(സ്ഥിരം Locale& locale,
                            UDisplayContext *contexts, int32_t length);

    // getters for state
    /**
     * Returns the locale used to determine the display names. This is
     * not necessarily the same locale passed to {@link #createInstance}.
     * @return the display locale
     * @stable ICU 4.4
     */
    മായാ സ്ഥിരം Locale& getLocale() സ്ഥിരം U_LIFETIME_BOUND = 0;

    /**
     * Returns the dialect handling used in the display names.
     * @return the dialect handling enum
     * @stable ICU 4.4
     */
    മായാ UDialectHandling getDialectHandling() സ്ഥിരം = 0;

    /**
     * Returns the UDisplayContext value for the specified UDisplayContextType.
     * @param type the UDisplayContextType whose value to return
     * @return the UDisplayContext for the specified type.
     * @stable ICU 51
     */
    മായാ UDisplayContext getContext(UDisplayContextType type) സ്ഥിരം = 0;

    // names for entire locales
    /**
     * Returns the display name of the provided locale.
     * @param locale the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    മായാ UnicodeString& localeDisplayName(സ്ഥിരം Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം = 0;

    /**
     * Returns the display name of the provided locale id.
     * @param localeId the id of the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    മായാ UnicodeString& localeDisplayName(സ്ഥിരം അക്ഷരം* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം = 0;

    // names for components of a locale id
    /**
     * Returns the display name of the provided language code.
     * @param lang the language code
     * @param result receives the language code's display name
     * @return the display name of the provided language code
     * @stable ICU 4.4
     */
    മായാ UnicodeString& languageDisplayName(സ്ഥിരം അക്ഷരം* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം = 0;

    /**
     * Returns the display name of the provided script code.
     * @param script the script code
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    മായാ UnicodeString& scriptDisplayName(സ്ഥിരം അക്ഷരം* script,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം = 0;

    /**
     * Returns the display name of the provided script code.
     * @param scriptCode the script code number
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    മായാ UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം = 0;

    /**
     * Returns the display name of the provided region code.
     * @param region the region code
     * @param result receives the region code's display name
     * @return the display name of the provided region code
     * @stable ICU 4.4
     */
    മായാ UnicodeString& regionDisplayName(സ്ഥിരം അക്ഷരം* region,
                                             UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം = 0;

    /**
     * Returns the display name of the provided variant.
     * @param variant the variant string
     * @param result receives the variant's display name
     * @return the display name of the provided variant
     * @stable ICU 4.4
     */
    മായാ UnicodeString& variantDisplayName(സ്ഥിരം അക്ഷരം* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം = 0;

    /**
     * Returns the display name of the provided locale key.
     * @param key the locale key name
     * @param result receives the locale key's display name
     * @return the display name of the provided locale key
     * @stable ICU 4.4
     */
    മായാ UnicodeString& താക്കോൽ_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ,
                                          UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം = 0;

    /**
     * Returns the display name of the provided value (used with the provided key).
     * @param key the locale key name
     * @param value the locale key's value
     * @param result receives the value's display name
     * @return the display name of the provided value
     * @stable ICU 4.4
     */
    മായാ UnicodeString& താക്കോൽ_മൂല്യം_Display_Name(സ്ഥിരം അക്ഷരം* താക്കോൽ, സ്ഥിരം അക്ഷരം* മൂല്യം_2,
                                               UnicodeString& result U_LIFETIME_BOUND) സ്ഥിരം = 0;
};

വരിയിൽ LocaleDisplayNames* LocaleDisplayNames::createInstance(സ്ഥിരം Locale& locale) {
  തിരികെ LocaleDisplayNames::createInstance(locale, ULDN_STANDARD_NAMES);
}

U_NAMESPACE_END

#endif

#endif /* U_SHOW_CPLUSPLUS_API */

#endif
