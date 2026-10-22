// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2009-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  errorcode.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2009mar10
*   created by: Markus W. Scherer
*/

#اگر_تعریف_نشده __ERRORCODE_H__
#تعریف_کن __ERRORCODE_H__

/**
 * \file 
 * \brief C++ API: ErrorCode class intended to make it easier to use
 *                 ICU C and C++ APIs from C++ user code.
 */

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/uobject.h"

U_NAMESPACE_BEGIN

/**
 * Wrapper class for UErrorCode, with conversion operators for direct use
 * in ICU C and C++ APIs.
 * Intended to be used as a base class, where a subclass overrides
 * the handleFailure() function so that it throws an exception,
 * does an assert(), logs an error, etc.
 * This is not an abstract base class. This class can be used and instantiated
 * by itself, although it will be more useful when subclassed.
 *
 * Features:
 * - The constructor initializes the internal UErrorCode to U_ZERO_ERROR,
 *   removing one common source of errors.
 * - Same use in C APIs taking a UErrorCode * (pointer)
 *   and C++ taking UErrorCode & (reference) via conversion operators.
 * - Possible automatic checking for success when it goes out of scope.
 *
 * Note: For automatic checking for success in the destructor, a subclass
 * must implement such logic in its own destructor because the base class
 * destructor cannot call a subclass function (like handleFailure()).
 * The ErrorCode base class destructor does nothing.
 *
 * Note also: While it is possible for a destructor to throw an exception,
 * it is generally unsafe to do so. This means that in a subclass the destructor
 * and the handleFailure() function may need to take different actions.
 *
 * Sample code:
 * \code
 *   class IcuErrorCode: public icu::ErrorCode {
 *   public:
 *     virtual ~IcuErrorCode() {  // should be defined in .cpp as "key function"
 *       // Safe because our handleFailure() does not throw exceptions.
 *       if(isFailure()) { handleFailure(); }
 *     }
 *   protected:
 *     virtual void handleFailure() const {
 *       log_failure(u_errorName(errorCode));
 *       exit(errorCode);
 *     }
 *   };
 *   IcuErrorCode error_code;
 *   UConverter *cnv = ucnv_open("Shift-JIS", error_code);
 *   length = ucnv_fromUChars(dest, capacity, src, length, error_code);
 *   ucnv_close(cnv);
 *   // IcuErrorCode destructor checks for success.
 * \endcode
 *
 * @stable ICU 4.2
 */
class U_COMMON_API Error_کد: عام UMemory {
عام:
    /**
     * Default constructor. Initializes its UErrorCode to U_ZERO_ERROR.
     * @stable ICU 4.2
     */
    Error_کد() : error_کد(U_ZERO_ERROR) {}
    /** Destructor, does nothing. See class documentation for details. @stable ICU 4.2 */
    مجازی ~Error_کد();
    /** Conversion operator, returns a reference. @stable ICU 4.2 */
    عملگر UError_کد&() U_LIFETIME_BOUND { أرجع error_کد; }
    /** Conversion operator, returns a pointer. @stable ICU 4.2 */
    عملگر UError_کد*() U_LIFETIME_BOUND { أرجع &error_کد; }
    /** Tests for U_SUCCESS(). @stable ICU 4.2 */
    UBool isSuccess() ثابت { أرجع U_SUCCESS(error_کد); }
    /** Tests for U_FAILURE(). @stable ICU 4.2 */
    UBool isFailure() ثابت { أرجع U_FAILURE(error_کد); }
    /** Returns the UErrorCode value. @stable ICU 4.2 */
    UError_کد get() ثابت { أرجع error_کد; }
    /** Sets the UErrorCode value. @stable ICU 4.2 */
    فراغ مجموعه(UError_کد مقدار_2) { error_کد=مقدار_2; }
    /** Returns the UErrorCode value and resets it to U_ZERO_ERROR. @stable ICU 4.2 */
    UError_کد reset();
    /**
     * Asserts isSuccess().
     * In other words, this method checks for a failure code,
     * and the base class handles it like this:
     * \code
     *   if(isFailure()) { handleFailure(); }
     * \endcode
     * @stable ICU 4.4
     */
    فراغ assertSuccess() ثابت;
    /**
     * Return a string for the UErrorCode value.
     * The string will be the same as the name of the error code constant
     * in the UErrorCode enum.
     * @stable ICU 4.4
     */
    ثابت حرف* errorName() ثابت;

محمي:
    /**
     * Internal UErrorCode, accessible to subclasses.
     * @stable ICU 4.2
     */
    UError_کد error_کد;
    /**
     * Called by assertSuccess() if isFailure() is true.
     * A subclass should override this function to deal with a failure code:
     * Throw an exception, log an error, terminate the program, or similar.
     * @stable ICU 4.2
     */
    مجازی فراغ handleFailure() ثابت {}
};

U_NAMESPACE_انتها

#پایان_اگر /* U_SHOW_CPLUSPLUS_API */

#پایان_اگر  // __ERRORCODE_H__
