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

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

/**
 * \file
 * \brief C++ API: Provides display names of Locale and its components.
 */

#إذا !UCONFIG_NO_FORMATTING

#ضمن "unicode/locid.h"
#ضمن "unicode/strenum.h"
#ضمن "unicode/uscript.h"
#ضمن "unicode/uldnames.h"
#ضمن "unicode/udisplaycontext.h"

U_NAMESPACE_BEGIN

/**
 * Returns display names of Locales and components of Locales. For
 * more information on language, script, region, variant, key, and
 * values, see Locale.
 * @stable ICU 4.4
 */
class U_COMMON_API LocaleDisplayNames : عام UObject {
عام:
    /**
     * Destructor.
     * @stable ICU 4.4
     */
    virtual ~LocaleDisplayNames();

    /**
     * Convenience overload of
     * {@link #createInstance(const Locale& locale, UDialectHandling dialectHandling)}
     * that specifies STANDARD dialect handling.
     * @param locale the display locale
     * @return a LocaleDisplayNames instance
     * @stable ICU 4.4
     */
    مضمن ساكن LocaleDisplayNames* U_EXPORT2 createInstance(ثابت Locale& locale);

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
    ساكن LocaleDisplayNames* U_EXPORT2 createInstance(ثابت Locale& locale,
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
    ساكن LocaleDisplayNames* U_EXPORT2 createInstance(ثابت Locale& locale,
                            UDisplayContext *contexts, int32_t length);

    // getters for state
    /**
     * Returns the locale used to determine the display names. This is
     * not necessarily the same locale passed to {@link #createInstance}.
     * @return the display locale
     * @stable ICU 4.4
     */
    virtual ثابت Locale& getLocale() ثابت U_LIFETIME_BOUND = 0;

    /**
     * Returns the dialect handling used in the display names.
     * @return the dialect handling enum
     * @stable ICU 4.4
     */
    virtual UDialectHandling getDialectHandling() ثابت = 0;

    /**
     * Returns the UDisplayContext value for the specified UDisplayContextType.
     * @param type the UDisplayContextType whose value to return
     * @return the UDisplayContext for the specified type.
     * @stable ICU 51
     */
    virtual UDisplayContext getContext(UDisplayContextType type) ثابت = 0;

    // names for entire locales
    /**
     * Returns the display name of the provided locale.
     * @param locale the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    virtual UnicodeString& localeDisplayName(ثابت Locale& locale,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided locale id.
     * @param localeId the id of the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    virtual UnicodeString& localeDisplayName(ثابت حرف* localeId,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت = 0;

    // names for components of a locale id
    /**
     * Returns the display name of the provided language code.
     * @param lang the language code
     * @param result receives the language code's display name
     * @return the display name of the provided language code
     * @stable ICU 4.4
     */
    virtual UnicodeString& languageDisplayName(ثابت حرف* lang,
                                               UnicodeString& result U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided script code.
     * @param script the script code
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    virtual UnicodeString& scriptDisplayName(ثابت حرف* script,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided script code.
     * @param scriptCode the script code number
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    virtual UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided region code.
     * @param region the region code
     * @param result receives the region code's display name
     * @return the display name of the provided region code
     * @stable ICU 4.4
     */
    virtual UnicodeString& regionDisplayName(ثابت حرف* region,
                                             UnicodeString& result U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided variant.
     * @param variant the variant string
     * @param result receives the variant's display name
     * @return the display name of the provided variant
     * @stable ICU 4.4
     */
    virtual UnicodeString& variantDisplayName(ثابت حرف* variant,
                                              UnicodeString& result U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided locale key.
     * @param key the locale key name
     * @param result receives the locale key's display name
     * @return the display name of the provided locale key
     * @stable ICU 4.4
     */
    virtual UnicodeString& مفتاح_Display_Name(ثابت حرف* مفتاح,
                                          UnicodeString& result U_LIFETIME_BOUND) ثابت = 0;

    /**
     * Returns the display name of the provided value (used with the provided key).
     * @param key the locale key name
     * @param value the locale key's value
     * @param result receives the value's display name
     * @return the display name of the provided value
     * @stable ICU 4.4
     */
    virtual UnicodeString& مفتاح_قيمة_Display_Name(ثابت حرف* مفتاح, ثابت حرف* قيمة_2,
                                               UnicodeString& result U_LIFETIME_BOUND) ثابت = 0;
};

مضمن LocaleDisplayNames* LocaleDisplayNames::createInstance(ثابت Locale& locale) {
  أرجع LocaleDisplayNames::createInstance(locale, ULDN_STANDARD_NAMES);
}

U_NAMESPACE_END

#endif

#endif /* U_SHOW_CPLUSPLUS_API */

#endif
