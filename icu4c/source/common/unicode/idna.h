// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  idna.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010mar05
*   created by: Markus W. Scherer
*/

#ifndef __IDNA_H__
#define __IDNA_H__

/**
 * \file
 * \brief C++ API: Internationalizing Domain Names in Applications (IDNA)
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ U_SHOW_CPLUSPLUS_API

#ಯದಿ !UCONFIG_NO_IDNA

#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/uidna.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"

U_NAMESPACE_BEGIN

class IDNAInfo;

/**
 * Abstract base class for IDNA processing.
 * See http://www.unicode.org/reports/tr46/
 * and http://www.ietf.org/rfc/rfc3490.txt
 *
 * The IDNA class is not intended for public subclassing.
 *
 * This C++ API currently only implements UTS #46.
 * The uidna.h C API implements both UTS #46 (functions using UIDNA service object)
 * and IDNA2003 (functions that do not use a service object).
 * @stable ICU 4.6
 */
class U_COMMON_API IDNA : ಸಾರ್ವಜನಿಕ UObject {
ಸಾರ್ವಜನಿಕ:
    /**
     * Destructor.
     * @stable ICU 4.6
     */
    ~IDNA();

    /**
     * Returns an IDNA instance which implements UTS #46.
     * Returns an unmodifiable instance, owned by the caller.
     * Cache it for multiple operations, and delete it when done.
     * The instance is thread-safe, that is, it can be used concurrently.
     *
     * UTS #46 defines Unicode IDNA Compatibility Processing,
     * updated to the latest version of Unicode and compatible with both
     * IDNA2003 and IDNA2008.
     *
     * The worker functions use transitional processing, including deviation mappings,
     * unless UIDNA_NONTRANSITIONAL_TO_ASCII or UIDNA_NONTRANSITIONAL_TO_UNICODE
     * is used in which case the deviation characters are passed through without change.
     * <b>Unicode 15.1 UTS #46 deprecated transitional processing.</b>
     *
     * Disallowed characters are mapped to U+FFFD.
     *
     * For available options see the uidna.h header.
     * Operations with the UTS #46 instance do not support the
     * UIDNA_ALLOW_UNASSIGNED option.
     *
     * By default, the UTS #46 implementation allows all ASCII characters (as valid or mapped).
     * When the UIDNA_USE_STD3_RULES option is used, ASCII characters other than
     * letters, digits, hyphen (LDH) and dot/full stop are disallowed and mapped to U+FFFD.
     *
     * @param options Bit set to modify the processing and error checking.
     *                These should include UIDNA_DEFAULT, or
     *                UIDNA_NONTRANSITIONAL_TO_ASCII | UIDNA_NONTRANSITIONAL_TO_UNICODE.
     *                See option bit set values in uidna.h.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return the UTS #46 IDNA instance, if successful
     * @stable ICU 4.6
     */
    ಸ್ಥಿರ IDNA *
    createUTS46Instance(uint32_t options, UErrorCode &errorCode);

    /**
     * Converts a single domain name label into its ASCII form for DNS lookup.
     * If any processing step fails, then info.hasErrors() will be true and
     * the result might not be an ASCII string.
     * The label might be modified according to the types of errors.
     * Labels with severe errors will be left in (or turned into) their Unicode form.
     *
     * The UErrorCode indicates an error only in exceptional cases,
     * such as a U_MEMORY_ALLOCATION_ERROR.
     *
     * @param label Input domain name label
     * @param dest Destination string object
     * @param info Output container of IDNA processing details.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return dest
     * @stable ICU 4.6
     */
    ಆಭಾಸ UnicodeString&
    labelToASCII(ಧ್ರುವ UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UErrorCode& errorCode) ಧ್ರುವ = 0;

    /**
     * Converts a single domain name label into its Unicode form for human-readable display.
     * If any processing step fails, then info.hasErrors() will be true.
     * The label might be modified according to the types of errors.
     *
     * The UErrorCode indicates an error only in exceptional cases,
     * such as a U_MEMORY_ALLOCATION_ERROR.
     *
     * @param label Input domain name label
     * @param dest Destination string object
     * @param info Output container of IDNA processing details.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return dest
     * @stable ICU 4.6
     */
    ಆಭಾಸ UnicodeString&
    labelToUnicode(ಧ್ರುವ UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ಧ್ರುವ = 0;

    /**
     * Converts a whole domain name into its ASCII form for DNS lookup.
     * If any processing step fails, then info.hasErrors() will be true and
     * the result might not be an ASCII string.
     * The domain name might be modified according to the types of errors.
     * Labels with severe errors will be left in (or turned into) their Unicode form.
     *
     * The UErrorCode indicates an error only in exceptional cases,
     * such as a U_MEMORY_ALLOCATION_ERROR.
     *
     * @param name Input domain name
     * @param dest Destination string object
     * @param info Output container of IDNA processing details.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return dest
     * @stable ICU 4.6
     */
    ಆಭಾಸ UnicodeString&
    nameToASCII(ಧ್ರುವ UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UErrorCode& errorCode) ಧ್ರುವ = 0;

    /**
     * Converts a whole domain name into its Unicode form for human-readable display.
     * If any processing step fails, then info.hasErrors() will be true.
     * The domain name might be modified according to the types of errors.
     *
     * The UErrorCode indicates an error only in exceptional cases,
     * such as a U_MEMORY_ALLOCATION_ERROR.
     *
     * @param name Input domain name
     * @param dest Destination string object
     * @param info Output container of IDNA processing details.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return dest
     * @stable ICU 4.6
     */
    ಆಭಾಸ UnicodeString&
    nameToUnicode(ಧ್ರುವ UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UErrorCode& errorCode) ಧ್ರುವ = 0;

    // UTF-8 versions of the processing methods ---------------------------- ***

    /**
     * Converts a single domain name label into its ASCII form for DNS lookup.
     * UTF-8 version of labelToASCII(), same behavior.
     *
     * @param label Input domain name label
     * @param dest Destination byte sink; Flush()ed if successful
     * @param info Output container of IDNA processing details.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @stable ICU 4.6
     */
    ಆಭಾಸ ಶೂನ್ಯ
    labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ;

    /**
     * Converts a single domain name label into its Unicode form for human-readable display.
     * UTF-8 version of labelToUnicode(), same behavior.
     *
     * @param label Input domain name label
     * @param dest Destination byte sink; Flush()ed if successful
     * @param info Output container of IDNA processing details.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @stable ICU 4.6
     */
    ಆಭಾಸ ಶೂನ್ಯ
    labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ;

    /**
     * Converts a whole domain name into its ASCII form for DNS lookup.
     * UTF-8 version of nameToASCII(), same behavior.
     *
     * @param name Input domain name
     * @param dest Destination byte sink; Flush()ed if successful
     * @param info Output container of IDNA processing details.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @stable ICU 4.6
     */
    ಆಭಾಸ ಶೂನ್ಯ
    nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ;

    /**
     * Converts a whole domain name into its Unicode form for human-readable display.
     * UTF-8 version of nameToUnicode(), same behavior.
     *
     * @param name Input domain name
     * @param dest Destination byte sink; Flush()ed if successful
     * @param info Output container of IDNA processing details.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @stable ICU 4.6
     */
    ಆಭಾಸ ಶೂನ್ಯ
    nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ;
};

class UTS46;

/**
 * Output container for IDNA processing errors.
 * The IDNAInfo class is not suitable for subclassing.
 * @stable ICU 4.6
 */
class U_COMMON_API IDNAInfo : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    /**
     * Constructor for stack allocation.
     * @stable ICU 4.6
     */
    IDNAInfo() : errors(0), labelErrors(0), isTransDiff(false), isBiDi(false), isOkBiDi(true) {}
    /**
     * Were there IDNA processing errors?
     * @return true if there were processing errors
     * @stable ICU 4.6
     */
    UBool hasErrors() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ errors!=0; }
    /**
     * Returns a bit set indicating IDNA processing errors.
     * See UIDNA_ERROR_... constants in uidna.h.
     * @return bit set of processing errors
     * @stable ICU 4.6
     */
    uint32_t getErrors() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ errors; }
    /**
     * Returns true if transitional and nontransitional processing produce different results.
     * This is the case when the input label or domain name contains
     * one or more deviation characters outside a Punycode label (see UTS #46).
     * <ul>
     * <li>With nontransitional processing, such characters are
     * copied to the destination string.
     * <li>With transitional processing, such characters are
     * mapped (sharp s/sigma) or removed (joiner/nonjoiner).
     * </ul>
     * @return true if transitional and nontransitional processing produce different results
     * @stable ICU 4.6
     */
    UBool isTransitionalDifferent() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ isTransDiff; }

ಗುಪ್ತ:
    friend class UTS46;

    IDNAInfo(ಧ್ರುವ IDNAInfo &other) = delete;  // no copying
    IDNAInfo &operator=(ಧ್ರುವ IDNAInfo &other) = delete;  // no copying

    ಶೂನ್ಯ reset() {
        errors=labelErrors=0;
        isTransDiff=false;
        isBiDi=false;
        isOkBiDi=true;
    }

    uint32_t errors, labelErrors;
    UBool isTransDiff;
    UBool isBiDi;
    UBool isOkBiDi;
};

U_NAMESPACE_END

#endif  // UCONFIG_NO_IDNA

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __IDNA_H__
