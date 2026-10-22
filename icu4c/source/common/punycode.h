// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  punycode.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jan31
*   created by: Markus W. Scherer
*/

/* This ICU code derived from: */
/*
punycode.c 0.4.0 (2001-Nov-17-Sat)
http://www.cs.berkeley.edu/~amc/idn/
Adam M. Costello
http://www.nicemice.net/amc/
*/

#నిర్వచితం_కాకపోతే __PUNYCODE_H__
#నిర్వచించు __PUNYCODE_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_IDNA

/**
 * u_strToPunycode() converts Unicode to Punycode.
 *
 * The input string must not contain single, unpaired surrogates.
 * The output will be represented as an array of ASCII code points.
 *
 * The output string is NUL-terminated according to normal ICU
 * string output rules.
 *
 * @param src Input Unicode string.
 *            This function handles a limited amount of code points
 *            (the limit is >=64).
 *            U_INDEX_OUTOFBOUNDS_ERROR is set if the limit is exceeded.
 * @param srcLength Number of UChars in src, or -1 if NUL-terminated.
 * @param dest Output Punycode array.
 * @param destCapacity Size of dest.
 * @param caseFlags Vector of boolean values, one per input UChar,
 *                  indicating that the corresponding character is to be
 *                  marked for the decoder optionally
 *                  uppercasing (true) or lowercasing (false)
 *                  the character.
 *                  ASCII characters are output directly in the case as marked.
 *                  Flags corresponding to trail surrogates are ignored.
 *                  If caseFlags==NULL then input characters are not
 *                  case-mapped.
 * @param pErrorCode ICU in/out error code parameter.
 *                   U_INVALID_CHAR_FOUND if src contains
 *                   unmatched single surrogates.
 *                   U_INDEX_OUTOFBOUNDS_ERROR if src contains
 *                   too many code points.
 * @return Number of ASCII characters in puny.
 *
 * @see u_strFromPunycode
 */
U_CAPI int32_t
u_str_గమ్యం_Punycode(స్థిరరాశి UChar *src, int32_t src_నిడివి,
                UChar *dest, int32_t dest_సామర్థ్యం,
                స్థిరరాశి UBool *caseFlags,
                UError_సంకేతం *p_Error_సంకేతం);

/**
 * u_strFromPunycode() converts Punycode to Unicode.
 * The Unicode string will be at most as long (in UChars)
 * than the Punycode string (in chars).
 *
 * @param src Input Punycode string.
 * @param srcLength Length of puny, or -1 if NUL-terminated
 * @param dest Output Unicode string buffer.
 * @param destCapacity Size of dest in number of UChars,
 *                     and of caseFlags in numbers of UBools.
 * @param caseFlags Output array for case flags as
 *                  defined by the Punycode string.
 *                  The caller should uppercase (true) or lowercase (FASLE)
 *                  the corresponding character in dest.
 *                  For supplementary characters, only the lead surrogate
 *                  is marked, and false is stored for the trail surrogate.
 *                  This is redundant and not necessary for ASCII characters
 *                  because they are already in the case indicated.
 *                  Can be NULL if the case flags are not needed.
 * @param pErrorCode ICU in/out error code parameter.
 *                   U_INVALID_CHAR_FOUND if a non-ASCII character
 *                   precedes the last delimiter ('-'),
 *                   or if an invalid character (not a-zA-Z0-9) is found
 *                   after the last delimiter.
 *                   U_ILLEGAL_CHAR_FOUND if the delta sequence is ill-formed.
 * @return Number of UChars written to dest.
 *
 * @see u_strToPunycode
 */
U_CAPI int32_t
u_strFromPunycode(స్థిరరాశి UChar *src, int32_t src_నిడివి,
                  UChar *dest, int32_t dest_సామర్థ్యం,
                  UBool *caseFlags,
                  UError_సంకేతం *p_Error_సంకేతం);

#అయితే_ముగింపు /* #if !UCONFIG_NO_IDNA */

#అయితే_ముగింపు

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
