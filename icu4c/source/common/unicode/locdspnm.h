// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LOCDSPNM_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOCDSPNM_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API

/**
 * \file
 * \brief C++ API: Provides display names of Locale and its components.
 */

#ਜੇ !UCONFIG_NO_FORMATTING

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/strenum.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uldnames.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udisplaycontext.h"

U_NAMESPACE_BEGIN

/**
 * Returns display names of Locales and components of Locales. For
 * more information on language, script, region, variant, key, and
 * values, see Locale.
 * @stable ICU 4.4
 */
class U_COMMON_API Locale_Display_ਨਾਂ : ਜਨਤਕ UObject {
ਜਨਤਕ:
    /**
     * Destructor.
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ ~Locale_Display_ਨਾਂ();

    /**
     * Convenience overload of
     * {@link #createInstance(const Locale& locale, UDialectHandling dialectHandling)}
     * that specifies STANDARD dialect handling.
     * @param locale the display locale
     * @return a LocaleDisplayNames instance
     * @stable ICU 4.4
     */
    ਸਤਰਅੰਦਰ ਸਥਿਰ Locale_Display_ਨਾਂ* U_EXPORT2 createInstance(ਅਚਲ Locale& locale);

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
    ਸਥਿਰ Locale_Display_ਨਾਂ* U_EXPORT2 createInstance(ਅਚਲ Locale& locale,
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
    ਸਥਿਰ Locale_Display_ਨਾਂ* U_EXPORT2 createInstance(ਅਚਲ Locale& locale,
                            UDisplayContext *contexts, int32_t ਲੰਮਾਈ);

    // getters for state
    /**
     * Returns the locale used to determine the display names. This is
     * not necessarily the same locale passed to {@link #createInstance}.
     * @return the display locale
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ ਅਚਲ Locale& getLocale() ਅਚਲ U_LIFETIME_BOUND = 0;

    /**
     * Returns the dialect handling used in the display names.
     * @return the dialect handling enum
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ UDialectHandling getDialectHandling() ਅਚਲ = 0;

    /**
     * Returns the UDisplayContext value for the specified UDisplayContextType.
     * @param type the UDisplayContextType whose value to return
     * @return the UDisplayContext for the specified type.
     * @stable ICU 51
     */
    ਆਭਾਸੀ UDisplayContext getContext(UDisplayContextType type) ਅਚਲ = 0;

    // names for entire locales
    /**
     * Returns the display name of the provided locale.
     * @param locale the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ UnicodeString& localeDisplayName(ਅਚਲ Locale& locale,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ = 0;

    /**
     * Returns the display name of the provided locale id.
     * @param localeId the id of the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ UnicodeString& localeDisplayName(ਅਚਲ ਅੱਖਰ* localeId,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ = 0;

    // names for components of a locale id
    /**
     * Returns the display name of the provided language code.
     * @param lang the language code
     * @param result receives the language code's display name
     * @return the display name of the provided language code
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ UnicodeString& languageDisplayName(ਅਚਲ ਅੱਖਰ* lang,
                                               UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ = 0;

    /**
     * Returns the display name of the provided script code.
     * @param script the script code
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ UnicodeString& scriptDisplayName(ਅਚਲ ਅੱਖਰ* script,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ = 0;

    /**
     * Returns the display name of the provided script code.
     * @param scriptCode the script code number
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ UnicodeString& scriptDisplayName(UScript_ਕੋਡ script_ਕੋਡ,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ = 0;

    /**
     * Returns the display name of the provided region code.
     * @param region the region code
     * @param result receives the region code's display name
     * @return the display name of the provided region code
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ UnicodeString& regionDisplayName(ਅਚਲ ਅੱਖਰ* region,
                                             UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ = 0;

    /**
     * Returns the display name of the provided variant.
     * @param variant the variant string
     * @param result receives the variant's display name
     * @return the display name of the provided variant
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ UnicodeString& variantDisplayName(ਅਚਲ ਅੱਖਰ* variant,
                                              UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ = 0;

    /**
     * Returns the display name of the provided locale key.
     * @param key the locale key name
     * @param result receives the locale key's display name
     * @return the display name of the provided locale key
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ UnicodeString& ਕੁੰਜੀ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ,
                                          UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ = 0;

    /**
     * Returns the display name of the provided value (used with the provided key).
     * @param key the locale key name
     * @param value the locale key's value
     * @param result receives the value's display name
     * @return the display name of the provided value
     * @stable ICU 4.4
     */
    ਆਭਾਸੀ UnicodeString& ਕੁੰਜੀ_ਮੁੱਲ_Display_Name(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ* ਮੁੱਲ_2,
                                               UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) ਅਚਲ = 0;
};

ਸਤਰਅੰਦਰ Locale_Display_ਨਾਂ* Locale_Display_ਨਾਂ::createInstance(ਅਚਲ Locale& locale) {
  ਵਾਪਸ Locale_Display_ਨਾਂ::createInstance(locale, ULDN_STANDARD_ਨਾਂ);
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* U_SHOW_CPLUSPLUS_API */

#ਜੇ_ਅੰਤ
