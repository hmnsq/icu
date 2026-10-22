// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#شامل "unicode/usetiter.h"
#شامل "unicode/uniset.h"
#شامل "unicode/unistr.h"
#شامل "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_سیٹ_Iterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
Unicode_سیٹ_Iterator::Unicode_سیٹ_Iterator(ثابت Unicode_سیٹ& u_سیٹ U_LIFETIME_BOUND) {
    cpString  = خالی_اشاریہ;
    reset(u_سیٹ);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
Unicode_سیٹ_Iterator::Unicode_سیٹ_Iterator() {
    یہ->سیٹ = خالی_اشاریہ;
    cpString  = خالی_اشاریہ;
    reset();
}

Unicode_سیٹ_Iterator::~Unicode_سیٹ_Iterator() {
    حذف cpString;
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
    اگر (اگلا_Element <= انتہا_Element) {
        codepoint = codepoint_انتہا = اگلا_Element++;
        string = خالی_اشاریہ;
        واپس سچ;
    }
    اگر (range < انتہا_Range) {
        بھرو_Range(++range);
        codepoint = codepoint_انتہا = اگلا_Element++;
        string = خالی_اشاریہ;
        واپس سچ;
    }

    اگر (اگلا_String >= string_گنتی) واپس جھوٹ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ثابت UnicodeString*>(سیٹ->لڑیاں_2->elementAt(اگلا_String++));
    واپس سچ;
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
    string = خالی_اشاریہ;
    اگر (اگلا_Element <= انتہا_Element) {
        codepoint_انتہا = انتہا_Element;
        codepoint = اگلا_Element;
        اگلا_Element = انتہا_Element+1;
        واپس سچ;
    }
    اگر (range < انتہا_Range) {
        بھرو_Range(++range);
        codepoint_انتہا = انتہا_Element;
        codepoint = اگلا_Element;
        اگلا_Element = انتہا_Element+1;
        واپس سچ;
    }

    اگر (اگلا_String >= string_گنتی) واپس جھوٹ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ثابت UnicodeString*>(سیٹ->لڑیاں_2->elementAt(اگلا_String++));
    واپس سچ;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
خالی Unicode_سیٹ_Iterator::reset(ثابت Unicode_سیٹ& u_سیٹ) {
    یہ->سیٹ = &u_سیٹ;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
خالی Unicode_سیٹ_Iterator::reset() {
    اگر (سیٹ == خالی_اشاریہ) {
        // Set up indices to empty iteration
        انتہا_Range = -1;
        string_گنتی = 0;
    } ورنہ {
        انتہا_Range = سیٹ->get_Range_گنتی() - 1;
        string_گنتی = سیٹ->لڑیاں_سائز();
    }
    range = 0;
    انتہا_Element = -1;
    اگلا_Element = 0;            
    اگر (انتہا_Range >= 0) {
        بھرو_Range(range);
    }
    اگلا_String = 0;
    string = خالی_اشاریہ;
}

خالی Unicode_سیٹ_Iterator::بھرو_Range(int32_t iRange) {
    اگلا_Element = سیٹ->get_Range_آغاز(iRange);
    انتہا_Element = سیٹ->get_Range_انتہا(iRange);
}


ثابت UnicodeString& Unicode_سیٹ_Iterator::getString() U_LIFETIME_BOUND {
    اگر (string == خالی_اشاریہ && codepoint != static_cast<UChar32>(IS_STRING)) {
       اگر (cpString == خالی_اشاریہ) {
          cpString = نیا UnicodeString();
       }
       اگر (cpString != خالی_اشاریہ) {
          cpString->سیٹ_مقصد(codepoint);
       }
       string = cpString;
    }
    واپس *string;
}

U_NAMESPACE_انتہا

//eof
