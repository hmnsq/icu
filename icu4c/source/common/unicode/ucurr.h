// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#यदि_अपरिभाषितम् _UCURR_H_
#परिभाषय _UCURR_H_

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uenum.h"

/**
 * \file 
 * \brief C API: Encapsulates information about a currency.
 *
 * The ucurr API encapsulates information about a currency, as defined by
 * ISO 4217.  A currency is represented by a 3-character string
 * containing its ISO 4217 code.  This API can return various data
 * necessary the proper display of a currency:
 *
 * <ul><li>A display symbol, for a specific locale
 * <li>The number of fraction digits to display
 * <li>A rounding increment
 * </ul>
 *
 * The <tt>DecimalFormat</tt> class uses these data to display
 * currencies.
 * @author Alan Liu
 * @since ICU 2.2
 */

#यदि !UCONFIG_NO_FORMATTING

/**
 * Currency Usage used for Decimal Format
 * @stable ICU 54
 */
गणना UCurrencyUsage {
    /**
     * a setting to specify currency usage which determines currency digit
     * and rounding for standard usage, for example: "50.00 NT$"
     * used as DEFAULT value
     * @stable ICU 54
     */
    UCURR_USAGE_STANDARD=0,
    /**
     * a setting to specify currency usage which determines currency digit
     * and rounding for cash usage, for example: "50 NT$"
     * @stable ICU 54
     */
    UCURR_USAGE_CASH=1,
#यदि_अपरिभाषितम् U_HIDE_DEPRECATED_API
    /**
     * One higher than the last enum UCurrencyUsage constant.
     * @deprecated ICU 58 The numeric value may change over time, see ICU ticket #12420.
     */
    UCURR_USAGE_गणनम्=2
#यदि_अन्तः  // U_HIDE_DEPRECATED_API
};
/** Currency Usage used for Decimal Format */
प्रकारनिर्देश गणना UCurrencyUsage UCurrencyUsage; 

/**
 * Finds a currency code for the given locale.
 * @param locale the locale for which to retrieve a currency code. 
 *               Currency can be specified by the "currency" keyword
 *               in which case it overrides the default currency code
 * @param buff   fill in buffer. Can be NULL for preflighting.
 * @param buffCapacity capacity of the fill in buffer. Can be 0 for
 *               preflighting. If it is non-zero, the buff parameter
 *               must not be NULL.
 * @param ec error code
 * @return length of the currency string. It should always be 3. If 0,
 *                currency couldn't be found or the input values are 
 *                invalid. 
 * @stable ICU 2.8
 */
U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ध्रुव वर्ण* locale,
                UChar* buff,
                int32_t buff_क्षमता,
                UError_संहिता* ec);

/**
 * Selector constants for ucurr_getName().
 *
 * @see ucurr_getName
 * @stable ICU 2.6
 */
प्रकारनिर्देश गणना UCurrNameStyle {
    /**
     * Selector for ucurr_getName indicating a symbolic name for a
     * currency, such as "$" for USD.
     * @stable ICU 2.6
     */
    UCURR_चिह्नम्_NAME,

    /**
     * Selector for ucurr_getName indicating the long name for a
     * currency, such as "US Dollar" for USD.
     * @stable ICU 2.6
     */
    UCURR_LONG_NAME,

    /**
     * Selector for getName() indicating the narrow currency symbol.
     * The narrow currency symbol is similar to the regular currency
     * symbol, but it always takes the shortest form: for example,
     * "$" instead of "US$" for USD in en-CA.
     *
     * @stable ICU 61
     */
    UCURR_NARROW_चिह्नम्_NAME,

    /**
     * Selector for getName() indicating the formal currency symbol.
     * The formal currency symbol is similar to the regular currency
     * symbol, but it always takes the form used in formal settings
     * such as banking; for example, "NT$" instead of "$" for TWD in zh-TW.
     *
     * @stable ICU 68
     */
    UCURR_FORMAL_चिह्नम्_NAME,

    /**
     * Selector for getName() indicating the variant currency symbol.
     * The variant symbol for a currency is an alternative symbol
     * that is not necessarily as widely used as the regular symbol.
     *
     * @stable ICU 68
     */
    UCURR_VARIANT_चिह्नम्_NAME
    
} UCurrNameStyle;

#यदि !UCONFIG_NO_SERVICE
/**
 * @stable ICU 2.6
 */
प्रकारनिर्देश ध्रुव शून्य* UCurr_Registry_कुञ्चिका;

/**
 * Register an (existing) ISO 4217 currency code for the given locale.
 * Only the country code and the two variants EURO and PRE_EURO are
 * recognized.
 * @param isoCode the three-letter ISO 4217 currency code
 * @param locale  the locale for which to register this currency code
 * @param status the in/out status code
 * @return a registry key that can be used to unregister this currency code, or NULL
 * if there was an error.
 * @stable ICU 2.6
 */
U_CAPI UCurr_Registry_कुञ्चिका U_EXPORT2
ucurr_पञ्जिकाकोशः(ध्रुव UChar* iso_संहिता, 
                   ध्रुव वर्ण* locale,  
                   UError_संहिता* स्थितिः);
/**
 * Unregister the previously-registered currency definitions using the
 * URegistryKey returned from ucurr_register.  Key becomes invalid after
 * a successful call and should not be used again.  Any currency 
 * that might have been hidden by the original ucurr_register call is 
 * restored.
 * @param key the registry key returned by a previous call to ucurr_register
 * @param status the in/out status code, no special meanings are assigned
 * @return true if the currency for this key was successfully unregistered
 * @stable ICU 2.6
 */
U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_कुञ्चिका कुञ्चिका, UError_संहिता* स्थितिः);
#यदि_अन्तः /* UCONFIG_NO_SERVICE */

/**
 * Returns the display name for the given currency in the
 * given locale.  For example, the display name for the USD
 * currency object in the en_US locale is "$".
 * @param currency null-terminated 3-letter ISO 4217 code
 * @param locale locale in which to display currency
 * @param nameStyle selector for which kind of name to return
 * @param isChoiceFormat always set to false, or can be NULL;
 *     display names are static strings;
 *     since ICU 4.4, ChoiceFormat patterns are no longer supported
 * @param len fill-in parameter to receive length of result
 * @param ec error code
 * @return pointer to display string of 'len' UChars.  If the resource
 * data contains no entry for 'currency', then 'currency' itself is
 * returned.
 * @stable ICU 2.6
 */
U_CAPI ध्रुव UChar* U_EXPORT2
ucurr_getName(ध्रुव UChar* currency U_LIFETIME_BOUND,
              ध्रुव वर्ण* locale,
              UCurrNameStyle nameStyle,
              UBool* isChoiceFormat,
              int32_t* len,
              UError_संहिता* ec);

/**
 * Returns the plural name for the given currency in the
 * given locale.  For example, the plural name for the USD
 * currency object in the en_US locale is "US dollar" or "US dollars".
 * @param currency null-terminated 3-letter ISO 4217 code
 * @param locale locale in which to display currency
 * @param isChoiceFormat always set to false, or can be NULL;
 *     display names are static strings;
 *     since ICU 4.4, ChoiceFormat patterns are no longer supported
 * @param pluralCount plural count
 * @param len fill-in parameter to receive length of result
 * @param ec error code
 * @return pointer to display string of 'len' UChars.  If the resource
 * data contains no entry for 'currency', then 'currency' itself is
 * returned.
 * @stable ICU 4.2
 */
U_CAPI ध्रुव UChar* U_EXPORT2
ucurr_getPluralName(ध्रुव UChar* currency U_LIFETIME_BOUND,
                    ध्रुव वर्ण* locale,
                    UBool* isChoiceFormat,
                    ध्रुव वर्ण* plural_गणनम्,
                    int32_t* len,
                    UError_संहिता* ec);

/**
 * Returns the number of the number of fraction digits that should
 * be displayed for the given currency.
 * This is equivalent to ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
 *
 * Important: The number of fraction digits for a given currency is NOT
 * guaranteed to be constant across versions of ICU or CLDR. For example,
 * do NOT use this value as a mechanism for deciding the magnitude used
 * to store currency values in a database. You should use this value for
 * display purposes only.
 *
 * @param currency null-terminated 3-letter ISO 4217 code
 * @param ec input-output error code
 * @return a non-negative number of fraction digits to be
 * displayed, or 0 if there is an error
 * @stable ICU 3.0
 */
U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ध्रुव UChar* currency,
                               UError_संहिता* ec);

/**
 * Returns the number of the number of fraction digits that should
 * be displayed for the given currency with usage.
 *
 * Important: The number of fraction digits for a given currency is NOT
 * guaranteed to be constant across versions of ICU or CLDR. For example,
 * do NOT use this value as a mechanism for deciding the magnitude used
 * to store currency values in a database. You should use this value for
 * display purposes only.
 *
 * @param currency null-terminated 3-letter ISO 4217 code
 * @param usage enum usage for the currency
 * @param ec input-output error code
 * @return a non-negative number of fraction digits to be
 * displayed, or 0 if there is an error
 * @stable ICU 54
 */
U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ध्रुव UChar* currency, 
                                       ध्रुव UCurrencyUsage usage,
                                       UError_संहिता* ec);

/**
 * Returns the rounding increment for the given currency, or 0.0 if no
 * rounding is done by the currency.
 * This is equivalent to ucurr_getRoundingIncrementForUsage(currency,UCURR_USAGE_STANDARD,ec);
 * @param currency null-terminated 3-letter ISO 4217 code
 * @param ec input-output error code
 * @return the non-negative rounding increment, or 0.0 if none,
 * or 0.0 if there is an error
 * @stable ICU 3.0
 */
U_CAPI द्विगुण U_EXPORT2
ucurr_getRoundingIncrement(ध्रुव UChar* currency,
                           UError_संहिता* ec);

/**
 * Returns the rounding increment for the given currency, or 0.0 if no
 * rounding is done by the currency given usage.
 * @param currency null-terminated 3-letter ISO 4217 code
 * @param usage enum usage for the currency
 * @param ec input-output error code
 * @return the non-negative rounding increment, or 0.0 if none,
 * or 0.0 if there is an error
 * @stable ICU 54
 */
U_CAPI द्विगुण U_EXPORT2
ucurr_getRoundingIncrementForUsage(ध्रुव UChar* currency,
                                   ध्रुव UCurrencyUsage usage,
                                   UError_संहिता* ec);

/**
 * Selector constants for ucurr_openCurrencies().
 *
 * @see ucurr_openCurrencies
 * @stable ICU 3.2
 */
प्रकारनिर्देश गणना UCurrCurrencyType {
    /**
     * Select all ISO-4217 currency codes.
     * @stable ICU 3.2
     */
    UCURR_ALL = INT32_MAX,
    /**
     * Select only ISO-4217 commonly used currency codes.
     * These currencies can be found in common use, and they usually have
     * bank notes or coins associated with the currency code.
     * This does not include fund codes, precious metals and other
     * various ISO-4217 codes limited to special financial products.
     * @stable ICU 3.2
     */
    UCURR_COMMON = 1,
    /**
     * Select ISO-4217 uncommon currency codes.
     * These codes respresent fund codes, precious metals and other
     * various ISO-4217 codes limited to special financial products.
     * A fund code is a monetary resource associated with a currency.
     * @stable ICU 3.2
     */
    UCURR_UNCOMMON = 2,
    /**
     * Select only deprecated ISO-4217 codes.
     * These codes are no longer in general public use.
     * @stable ICU 3.2
     */
    UCURR_DEPRECATED = 4,
    /**
     * Select only non-deprecated ISO-4217 codes.
     * These codes are in general public use.
     * @stable ICU 3.2
     */
    UCURR_NON_DEPRECATED = 8
} UCurrCurrencyType;

/**
 * Provides a UEnumeration object for listing ISO-4217 codes.
 * @param currType You can use one of several UCurrCurrencyType values for this
 *      variable. You can also | (or) them together to get a specific list of
 *      currencies. Most people will want to use the (UCURR_COMMON|UCURR_NON_DEPRECATED) value to
 *      get a list of current currencies.
 * @param pErrorCode Error code
 * @stable ICU 3.2
 */
U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UError_संहिता *p_Error_संहिता);

/**
  * Queries if the given ISO 4217 3-letter code is available on the specified date range. 
  * 
  * Note: For checking availability of a currency on a specific date, specify the date on both 'from' and 'to' 
  * 
  * When 'from' is U_DATE_MIN and 'to' is U_DATE_MAX, this method checks if the specified currency is available any time. 
  * If 'from' and 'to' are same UDate value, this method checks if the specified currency is available on that date.
  * 
  * @param isoCode 
  *            The ISO 4217 3-letter code. 
  * 
  * @param from 
  *            The lower bound of the date range, inclusive. When 'from' is U_DATE_MIN, check the availability 
  *            of the currency any date before 'to' 
  * 
  * @param to 
  *            The upper bound of the date range, inclusive. When 'to' is U_DATE_MAX, check the availability of 
  *            the currency any date after 'from' 
  * 
  * @param errorCode 
  *            ICU error code 
   * 
  * @return true if the given ISO 4217 3-letter code is supported on the specified date range. 
  * 
  * @stable ICU 4.8 
  */ 
U_CAPI UBool U_EXPORT2
ucurr_is_उपलब्धम्(ध्रुव UChar* iso_संहिता, 
             UDate from, 
             UDate गन्तव्यम्, 
             UError_संहिता* error_संहिता);

/** 
 * Finds the number of valid currency codes for the
 * given locale and date.
 * @param locale the locale for which to retrieve the
 *               currency count.
 * @param date   the date for which to retrieve the
 *               currency count for the given locale.
 * @param ec     error code
 * @return       the number of currency codes for the
 *               given locale and date.  If 0, currency
 *               codes couldn't be found for the input
 *               values are invalid.
 * @stable ICU 4.0
 */
U_CAPI int32_t U_EXPORT2
ucurr_गणनम्_Currencies(ध्रुव वर्ण* locale, 
                 UDate date, 
                 UError_संहिता* ec); 

/** 
 * Finds a currency code for the given locale and date 
 * @param locale the locale for which to retrieve a currency code.  
 *               Currency can be specified by the "currency" keyword 
 *               in which case it overrides the default currency code 
 * @param date   the date for which to retrieve a currency code for 
 *               the given locale. 
 * @param index  the index within the available list of currency codes
 *               for the given locale on the given date.
 * @param buff   fill in buffer. Can be NULL for preflighting. 
 * @param buffCapacity capacity of the fill in buffer. Can be 0 for 
 *               preflighting. If it is non-zero, the buff parameter 
 *               must not be NULL. 
 * @param ec     error code 
 * @return       length of the currency string. It should always be 3. 
 *               If 0, currency couldn't be found or the input values are  
 *               invalid.  
 * @stable ICU 4.0 
 */ 
U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ध्रुव वर्ण* locale, 
                UDate date, 
                int32_t सूचकांक,
                UChar* buff, 
                int32_t buff_क्षमता, 
                UError_संहिता* ec); 

/**
 * Given a key and a locale, returns an array of string values in a preferred
 * order that would make a difference. These are all and only those values where
 * the open (creation) of the service with the locale formed from the input locale
 * plus input keyword and that value has different behavior than creation with the
 * input locale alone.
 * @param key           one of the keys supported by this service.  For now, only
 *                      "currency" is supported.
 * @param locale        the locale
 * @param commonlyUsed  if set to true it will return only commonly used values
 *                      with the given locale in preferred order.  Otherwise,
 *                      it will return all the available values for the locale.
 * @param status error status
 * @return a string enumeration over keyword values for the given key and the locale.
 * @stable ICU 4.2
 */
U_CAPI UEnumeration* U_EXPORT2
ucurr_get_Keyword_मानानि_For_Locale(ध्रुव वर्ण* कुञ्चिका,
                                ध्रुव वर्ण* locale,
                                UBool commonly_प्रयुक्तम्,
                                UError_संहिता* स्थितिः);

/**
 * Returns the ISO 4217 numeric code for the currency.
 * <p>Note: If the ISO 4217 numeric code is not assigned for the currency or
 * the currency is unknown, this function returns 0.
 *
 * @param currency null-terminated 3-letter ISO 4217 code
 * @return The ISO 4217 numeric code of the currency
 * @stable ICU 49
 */
U_CAPI int32_t U_EXPORT2
ucurr_get_Numeric_संहिता(ध्रुव UChar* currency);

/**
 * The number of digits to be used when formatting
 * a currency amount when the default number of
 * digits cannot be determined from the currency name.
 *
 * @internal ICU 79 technology preview
 * @deprecated This API is for technology preview only.
 */
#परिभाषय U_CURR_DEFAULT_DIGITS 2

#यदि_अन्तः /* #if !UCONFIG_NO_FORMATTING */

#यदि_अन्तः
