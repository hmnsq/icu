// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2010-2016, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#अगर_अघोषित LOCDSPNM_H
#घोषणा LOCDSPNM_H

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

/**
 * \file
 * \brief C++ API: Provides display names of Locale and its components.
 */

#अगर !UCONFIG_NO_FORMATTING

#समावेश "unicode/locid.h"
#समावेश "unicode/strenum.h"
#समावेश "unicode/uscript.h"
#समावेश "unicode/uldnames.h"
#समावेश "unicode/udisplaycontext.h"

U_NAMESPACE_BEGIN

/**
 * Returns display names of Locales and components of Locales. For
 * more information on language, script, region, variant, key, and
 * values, see Locale.
 * @stable ICU 4.4
 */
class U_COMMON_API LocaleDisplayNames : खुला UObject {
खुला:
    /**
     * Destructor.
     * @stable ICU 4.4
     */
    भव ~LocaleDisplayNames();

    /**
     * Convenience overload of
     * {@link #createInstance(const Locale& locale, UDialectHandling dialectHandling)}
     * that specifies STANDARD dialect handling.
     * @param locale the display locale
     * @return a LocaleDisplayNames instance
     * @stable ICU 4.4
     */
    अंतरभूत अटल LocaleDisplayNames* U_EXPORT2 createInstance(स्थिर Locale& locale);

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
    अटल LocaleDisplayNames* U_EXPORT2 createInstance(स्थिर Locale& locale,
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
    अटल LocaleDisplayNames* U_EXPORT2 createInstance(स्थिर Locale& locale,
                            UDisplayContext *contexts, int32_t लंबाई);

    // getters for state
    /**
     * Returns the locale used to determine the display names. This is
     * not necessarily the same locale passed to {@link #createInstance}.
     * @return the display locale
     * @stable ICU 4.4
     */
    भव स्थिर Locale& getLocale() स्थिर U_LIFETIME_BOUND = 0;

    /**
     * Returns the dialect handling used in the display names.
     * @return the dialect handling enum
     * @stable ICU 4.4
     */
    भव UDialectHandling getDialectHandling() स्थिर = 0;

    /**
     * Returns the UDisplayContext value for the specified UDisplayContextType.
     * @param type the UDisplayContextType whose value to return
     * @return the UDisplayContext for the specified type.
     * @stable ICU 51
     */
    भव UDisplayContext getContext(UDisplayContextType type) स्थिर = 0;

    // names for entire locales
    /**
     * Returns the display name of the provided locale.
     * @param locale the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    भव UnicodeString& localeDisplayName(स्थिर Locale& locale,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर = 0;

    /**
     * Returns the display name of the provided locale id.
     * @param localeId the id of the locale whose display name to return
     * @param result receives the locale's display name
     * @return the display name of the provided locale
     * @stable ICU 4.4
     */
    भव UnicodeString& localeDisplayName(स्थिर अक्षर* localeId,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर = 0;

    // names for components of a locale id
    /**
     * Returns the display name of the provided language code.
     * @param lang the language code
     * @param result receives the language code's display name
     * @return the display name of the provided language code
     * @stable ICU 4.4
     */
    भव UnicodeString& languageDisplayName(स्थिर अक्षर* lang,
                                               UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर = 0;

    /**
     * Returns the display name of the provided script code.
     * @param script the script code
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    भव UnicodeString& scriptDisplayName(स्थिर अक्षर* script,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर = 0;

    /**
     * Returns the display name of the provided script code.
     * @param scriptCode the script code number
     * @param result receives the script code's display name
     * @return the display name of the provided script code
     * @stable ICU 4.4
     */
    भव UnicodeString& scriptDisplayName(UScriptCode scriptCode,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर = 0;

    /**
     * Returns the display name of the provided region code.
     * @param region the region code
     * @param result receives the region code's display name
     * @return the display name of the provided region code
     * @stable ICU 4.4
     */
    भव UnicodeString& regionDisplayName(स्थिर अक्षर* region,
                                             UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर = 0;

    /**
     * Returns the display name of the provided variant.
     * @param variant the variant string
     * @param result receives the variant's display name
     * @return the display name of the provided variant
     * @stable ICU 4.4
     */
    भव UnicodeString& variantDisplayName(स्थिर अक्षर* variant,
                                              UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर = 0;

    /**
     * Returns the display name of the provided locale key.
     * @param key the locale key name
     * @param result receives the locale key's display name
     * @return the display name of the provided locale key
     * @stable ICU 4.4
     */
    भव UnicodeString& कुंजी_Display_Name(स्थिर अक्षर* कुंजी,
                                          UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर = 0;

    /**
     * Returns the display name of the provided value (used with the provided key).
     * @param key the locale key name
     * @param value the locale key's value
     * @param result receives the value's display name
     * @return the display name of the provided value
     * @stable ICU 4.4
     */
    भव UnicodeString& कुंजी_मूल्य_Display_Name(स्थिर अक्षर* कुंजी, स्थिर अक्षर* मूल्य_2,
                                               UnicodeString& परिणाम U_LIFETIME_BOUND) स्थिर = 0;
};

अंतरभूत LocaleDisplayNames* LocaleDisplayNames::createInstance(स्थिर Locale& locale) {
  वापस LocaleDisplayNames::createInstance(locale, ULDN_STANDARD_NAMES);
}

U_NAMESPACE_END

#पूर्ण_अगर

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */

#पूर्ण_अगर
