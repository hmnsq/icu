// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// This file contains utilities to deal with static-allocated UnicodeSets.
//
// Common use case: you write a "private static final" UnicodeSet in Java, and
// want something similarly easy in C++.  Originally written for number
// parsing, but this header can be used for other applications.
//
// Main entrypoint: `unisets::get(unisets::MY_SET_ID_HERE)`
//
// This file is in common instead of i18n because it is needed by ucurr.cpp.
//
// Author: sffc

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_FORMATTING
#నిర్వచితం_కాకపోతే __STATIC_UNICODE_SETS_H__
#నిర్వచించు __STATIC_UNICODE_SETS_H__

#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/unistr.h"

U_NAMESPACE_BEGIN
namespace unisets {

గణన కీలకం_2 {
    // NONE is used to indicate null in chooseFrom().
    // EMPTY is used to get an empty UnicodeSet.
    NONE = -1,
    ఖాళీలు_2 = 0,

    // Ignorables
    DEFAULT_IGNORABLES,
    STRICT_IGNORABLES,

    // Separators
    // Notes:
    // - COMMA is a superset of STRICT_COMMA
    // - PERIOD is a superset of SCRICT_PERIOD
    // - ALL_SEPARATORS is the union of COMMA, PERIOD, and OTHER_GROUPING_SEPARATORS
    // - STRICT_ALL_SEPARATORS is the union of STRICT_COMMA, STRICT_PERIOD, and OTHER_GRP_SEPARATORS
    COMMA,
    PERIOD,
    STRICT_COMMA,
    STRICT_PERIOD,
    APOSTROPHE_SIGN,
    OTHER_GROUPING_SEPARATORS,
    ALL_SEPARATORS,
    STRICT_ALL_SEPARATORS,

    // Symbols
    MINUS_SIGN,
    PLUS_SIGN,
    PERCENT_SIGN,
    PERMILLE_SIGN,
    INFINITY_SIGN,
    APPROXIMATELY_SIGN,

    // Currency Symbols
    DOLLAR_SIGN,
    POUND_SIGN,
    RUPEE_SIGN,
    YEN_SIGN,
    WON_SIGN,

    // Other
    DIGITS,

    // Combined Separators with Digits (for lead code points)
    DIGITS_OR_ALL_SEPARATORS,
    DIGITS_OR_STRICT_ALL_SEPARATORS,

    // The number of elements in the enum.
    UNISETS_కీలకం_లెక్క
};

/**
 * Gets the static-allocated UnicodeSet according to the provided key. The
 * pointer will be deleted during u_cleanup(); the caller should NOT delete it.
 *
 * Exported as U_COMMON_API for ucurr.cpp
 *
 * This method is always safe and OK to chain: in the case of a memory or other
 * error, it returns an empty set from static memory.
 * 
 * Example:
 * 
 *     UBool hasIgnorables = unisets::get(unisets::DEFAULT_IGNORABLES)->contains(...);
 *
 * @param key The desired UnicodeSet according to the enum in this file.
 * @return The requested UnicodeSet. Guaranteed to be frozen and non-null, but
 *         may be empty if an error occurred during data loading.
 */
U_COMMON_API స్థిరరాశి Unicode_సమితి* get(కీలకం_2 కీలకం);

/**
 * Checks if the UnicodeSet given by key1 contains the given string.
 *
 * Exported as U_COMMON_API for numparse_decimal.cpp
 *
 * @param str The string to check.
 * @param key1 The set to check.
 * @return key1 if the set contains str, or NONE if not.
 */
U_COMMON_API కీలకం_2 ఎంచుకో_From(UnicodeString str, కీలకం_2 key1);

/**
 * Checks if the UnicodeSet given by either key1 or key2 contains the string.
 *
 * Exported as U_COMMON_API for numparse_decimal.cpp
 *
 * @param str The string to check.
 * @param key1 The first set to check.
 * @param key2 The second set to check.
 * @return key1 if that set contains str; key2 if that set contains str; or
 *         NONE if neither set contains str.
 */
U_COMMON_API కీలకం_2 ఎంచుకో_From(UnicodeString str, కీలకం_2 key1, కీలకం_2 key2);

// TODO: Load these from data: ICU-20108
// Unused in C++:
// Key chooseCurrency(UnicodeString str);
// Used instead:
స్థిర స్థిరరాశి నిర్మాణం {
    కీలకం_2 కీలకం;
    UChar32 exemplar;
} kCurrencyEntries[] = {
    {DOLLAR_SIGN, u'$'},
    {POUND_SIGN, u'£'},
    {RUPEE_SIGN, u'₹'},
    {YEN_SIGN, u'¥'},
    {WON_SIGN, u'₩'},
};

} // namespace unisets
U_NAMESPACE_చివర

#అయితే_ముగింపు //__STATIC_UNICODE_SETS_H__
#అయితే_ముగింపు /* #if !UCONFIG_NO_FORMATTING */
