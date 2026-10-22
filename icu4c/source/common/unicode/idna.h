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

#اگر_غیر_تعریف_شدہ __IDNA_H__
#تعریف_کرو __IDNA_H__

/**
 * \file
 * \brief C++ API: Internationalizing Domain Names in Applications (IDNA)
 */

#شامل "unicode/utypes.h"

#اگر U_SHOW_CPLUSPLUS_API

#اگر !UCONFIG_NO_IDNA

#شامل "unicode/bytestream.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/uidna.h"
#شامل "unicode/unistr.h"

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
class U_COMMON_API IDNA : عوامی UObject {
عوامی:
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
    ساکن IDNA *
    createUTS46Instance(uint32_t options, UError_رمز &error_رمز);

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
    مجازی UnicodeString&
    label_مقصد_ASCII(ثابت UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UError_رمز& error_رمز) ثابت = 0;

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
    مجازی UnicodeString&
    label_مقصد_Unicode(ثابت UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_رمز& error_رمز) ثابت = 0;

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
    مجازی UnicodeString&
    name_مقصد_ASCII(ثابت UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UError_رمز& error_رمز) ثابت = 0;

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
    مجازی UnicodeString&
    name_مقصد_Unicode(ثابت UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UError_رمز& error_رمز) ثابت = 0;

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
    مجازی خالی
    label_مقصد_ASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UError_رمز &error_رمز) ثابت;

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
    مجازی خالی
    label_مقصد_Unicode_UTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UError_رمز &error_رمز) ثابت;

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
    مجازی خالی
    name_مقصد_ASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UError_رمز &error_رمز) ثابت;

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
    مجازی خالی
    name_مقصد_Unicode_UTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UError_رمز &error_رمز) ثابت;
};

class UTS46;

/**
 * Output container for IDNA processing errors.
 * The IDNAInfo class is not suitable for subclassing.
 * @stable ICU 4.6
 */
class U_COMMON_API IDNAInfo : عوامی UMemory {
عوامی:
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
    UBool hasErrors() ثابت { واپس errors!=0; }
    /**
     * Returns a bit set indicating IDNA processing errors.
     * See UIDNA_ERROR_... constants in uidna.h.
     * @return bit set of processing errors
     * @stable ICU 4.6
     */
    uint32_t getErrors() ثابت { واپس errors; }
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
    UBool isTransitionalDifferent() ثابت { واپس isTransDiff; }

نجی:
    دوست class UTS46;

    IDNAInfo(ثابت IDNAInfo &other) = delete;  // no copying
    IDNAInfo &عامل=(ثابت IDNAInfo &other) = delete;  // no copying

    خالی reset() {
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

U_NAMESPACE_انتہا

#اگر_ختم  // UCONFIG_NO_IDNA

#اگر_ختم /* U_SHOW_CPLUSPLUS_API */

#اگر_ختم  // __IDNA_H__
