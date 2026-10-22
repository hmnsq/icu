// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#اگر_غیر_تعریف_شدہ _UCURR_IMP_H_
#تعریف_کرو _UCURR_IMP_H_

#شامل "unicode/utypes.h"
#شامل "unicode/unistr.h"
#شامل "unicode/parsepos.h"
#شامل "unicode/uniset.h"

/**
 * Internal method.  Given a currency ISO code and a locale, return
 * the "static" currency name.  This is usually the same as the
 * UCURR_SYMBOL_NAME, but if the latter is a choice format, then the
 * format is applied to the number 2.0 (to yield the more common
 * plural) to return a static name.
 *
 * This is used for backward compatibility with old currency logic in
 * DecimalFormat and DecimalFormatSymbols.
 */
U_CAPI خالی
uprv_getStaticCurrencyName(ثابت UChar* iso, ثابت حرف* loc,
                           icu::UnicodeString& نتیجہ, UError_رمز& ec);

/**
 * Attempt to parse the given string as a currency, either as a
 * display name in the given locale, or as a 3-letter ISO 4217
 * code.  If multiple display names match, then the longest one is
 * selected.  If both a display name and a 3-letter ISO code
 * match, then the display name is preferred, unless it's length
 * is less than 3.
 *
 * The parameters must not be NULL.
 *
 * @param locale the locale of the display names to match
 * @param text the text to parse
 * @param pos input-output position; on input, the position within
 * text to match; must have 0 <= pos.getIndex() < text.length();
 * on output, the position after the last matched character. If
 * the parse fails, the position in unchanged upon output.
 * @param type currency type to parse against, LONG_NAME only or not
 * @param partialMatchLen The length of the longest matching prefix;
 * this may be nonzero even if no full currency was matched.
 * @return the ISO 4217 code, as a string, of the best match, or
 * null if there is no match
 *
 * @internal
 */
U_CAPI خالی
uprv_parseCurrency(ثابت حرف* locale,
                   ثابت icu::UnicodeString& متن,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partial_مطابقت_Len,
                   UChar* نتیجہ,
                   UError_رمز& ec);

/**
 * Puts all possible first-characters of a currency into the
 * specified UnicodeSet.
 *
 * @param locale the locale of the display names of interest
 * @param result the UnicodeSet to which to add the starting characters
 */
خالی uprv_currencyLeads(ثابت حرف* locale, icu::Unicode_سیٹ& نتیجہ, UError_رمز& ec);



#اگر_ختم /* #ifndef _UCURR_IMP_H_ */

//eof
