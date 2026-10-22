// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#ضمن "unicode/usetiter.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/unistr.h"
#ضمن "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_سیٹ_Iterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
Unicode_سیٹ_Iterator::Unicode_سیٹ_Iterator(ثابت Unicode_سیٹ& u_سیٹ U_LIFETIME_BOUND) {
    cpString  = خالی_سوچک;
    reset(u_سیٹ);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
Unicode_سیٹ_Iterator::Unicode_سیٹ_Iterator() {
    هذا->سیٹ = خالی_سوچک;
    cpString  = خالی_سوچک;
    reset();
}

Unicode_سیٹ_Iterator::~Unicode_سیٹ_Iterator() {
    مٹاؤ cpString;
}

/**
 * Returns the next element in the set.
 * @return true if there was another element in the set.
 * if so, if codepoint == IS_STRING, the value is a string in the string field
 * else the value is a single code point in the codepoint field.
 * <br>You are guaranteed that the codepoints are in sorted order, and the strings are in sorted order,
 * and that all code points are returned before any strings are returned.
 * <br>Note also that the codepointEnd is undefined after calling this method.
 */
UBool Unicode_سیٹ_Iterator::next() {
    إذا (اگلا_Element <= سرا_Element) {
        codepoint = codepoint_سرا = اگلا_Element++;
        string = خالی_سوچک;
        أرجع سچ;
    }
    إذا (range < سرا_Range) {
        لدو_Range(++range);
        codepoint = codepoint_سرا = اگلا_Element++;
        string = خالی_سوچک;
        أرجع سچ;
    }

    إذا (اگلا_String >= string_گنتی) أرجع خطأ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ثابت UnicodeString*>(سیٹ->لڑیاں_2->elementAt(اگلا_String++));
    أرجع سچ;
}

/**
 * @return true if there was another element in the set.
 * if so, if codepoint == IS_STRING, the value is a string in the string field
 * else the value is a range of codepoints in the <codepoint, codepointEnd> fields.
 * <br>Note that the codepoints are in sorted order, and the strings are in sorted order,
 * and that all code points are returned before any strings are returned.
 * <br>You are guaranteed that the ranges are in sorted order, and the strings are in sorted order,
 * and that all ranges are returned before any strings are returned.
 * <br>You are also guaranteed that ranges are disjoint and non-contiguous.
 * <br>Note also that the codepointEnd is undefined after calling this method.
 */
UBool Unicode_سیٹ_Iterator::اگلا_Range() {
    string = خالی_سوچک;
    إذا (اگلا_Element <= سرا_Element) {
        codepoint_سرا = سرا_Element;
        codepoint = اگلا_Element;
        اگلا_Element = سرا_Element+1;
        أرجع سچ;
    }
    إذا (range < سرا_Range) {
        لدو_Range(++range);
        codepoint_سرا = سرا_Element;
        codepoint = اگلا_Element;
        اگلا_Element = سرا_Element+1;
        أرجع سچ;
    }

    إذا (اگلا_String >= string_گنتی) أرجع خطأ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ثابت UnicodeString*>(سیٹ->لڑیاں_2->elementAt(اگلا_String++));
    أرجع سچ;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
فراغ Unicode_سیٹ_Iterator::reset(ثابت Unicode_سیٹ& u_سیٹ) {
    هذا->سیٹ = &u_سیٹ;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
فراغ Unicode_سیٹ_Iterator::reset() {
    إذا (سیٹ == خالی_سوچک) {
        // Set up indices to empty iteration
        سرا_Range = -1;
        string_گنتی = 0;
    } وإلا {
        سرا_Range = سیٹ->get_Range_گنتی() - 1;
        string_گنتی = سیٹ->لڑیاں_ناپ();
    }
    range = 0;
    سرا_Element = -1;
    اگلا_Element = 0;            
    إذا (سرا_Range >= 0) {
        لدو_Range(range);
    }
    اگلا_String = 0;
    string = خالی_سوچک;
}

فراغ Unicode_سیٹ_Iterator::لدو_Range(int32_t iRange) {
    اگلا_Element = سیٹ->get_Range_شروع(iRange);
    سرا_Element = سیٹ->get_Range_سرا(iRange);
}


ثابت UnicodeString& Unicode_سیٹ_Iterator::getString() U_LIFETIME_BOUND {
    إذا (string == خالی_سوچک && codepoint != static_cast<UChar32>(IS_STRING)) {
       إذا (cpString == خالی_سوچک) {
          cpString = جديد UnicodeString();
       }
       إذا (cpString != خالی_سوچک) {
          cpString->سیٹ_منزل(codepoint);
       }
       string = cpString;
    }
    أرجع *string;
}

U_NAMESPACE_سرا

//eof
