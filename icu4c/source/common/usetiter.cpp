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

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_سيٽ_Iterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
Unicode_سيٽ_Iterator::Unicode_سيٽ_Iterator(ثابت Unicode_سيٽ& u_سيٽ U_LIFETIME_BOUND) {
    cpString  = خالي_اشارو;
    reset(u_سيٽ);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
Unicode_سيٽ_Iterator::Unicode_سيٽ_Iterator() {
    هذا->سيٽ = خالي_اشارو;
    cpString  = خالي_اشارو;
    reset();
}

Unicode_سيٽ_Iterator::~Unicode_سيٽ_Iterator() {
    ميٽيو cpString;
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
UBool Unicode_سيٽ_Iterator::next() {
    إذا (ايندڙ_Element <= پڇاڙي_Element) {
        codepoint = codepoint_پڇاڙي = ايندڙ_Element++;
        string = خالي_اشارو;
        أرجع سچ;
    }
    إذا (range < پڇاڙي_Range) {
        ڀريو_Range(++range);
        codepoint = codepoint_پڇاڙي = ايندڙ_Element++;
        string = خالي_اشارو;
        أرجع سچ;
    }

    إذا (ايندڙ_String >= string_ڳڻپ) أرجع خطأ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ثابت UnicodeString*>(سيٽ->لڙيون_2->elementAt(ايندڙ_String++));
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
UBool Unicode_سيٽ_Iterator::ايندڙ_Range() {
    string = خالي_اشارو;
    إذا (ايندڙ_Element <= پڇاڙي_Element) {
        codepoint_پڇاڙي = پڇاڙي_Element;
        codepoint = ايندڙ_Element;
        ايندڙ_Element = پڇاڙي_Element+1;
        أرجع سچ;
    }
    إذا (range < پڇاڙي_Range) {
        ڀريو_Range(++range);
        codepoint_پڇاڙي = پڇاڙي_Element;
        codepoint = ايندڙ_Element;
        ايندڙ_Element = پڇاڙي_Element+1;
        أرجع سچ;
    }

    إذا (ايندڙ_String >= string_ڳڻپ) أرجع خطأ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ثابت UnicodeString*>(سيٽ->لڙيون_2->elementAt(ايندڙ_String++));
    أرجع سچ;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
فراغ Unicode_سيٽ_Iterator::reset(ثابت Unicode_سيٽ& u_سيٽ) {
    هذا->سيٽ = &u_سيٽ;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
فراغ Unicode_سيٽ_Iterator::reset() {
    إذا (سيٽ == خالي_اشارو) {
        // Set up indices to empty iteration
        پڇاڙي_Range = -1;
        string_ڳڻپ = 0;
    } وإلا {
        پڇاڙي_Range = سيٽ->get_Range_ڳڻپ() - 1;
        string_ڳڻپ = سيٽ->لڙيون_سائيز();
    }
    range = 0;
    پڇاڙي_Element = -1;
    ايندڙ_Element = 0;            
    إذا (پڇاڙي_Range >= 0) {
        ڀريو_Range(range);
    }
    ايندڙ_String = 0;
    string = خالي_اشارو;
}

فراغ Unicode_سيٽ_Iterator::ڀريو_Range(int32_t iRange) {
    ايندڙ_Element = سيٽ->get_Range_شروعات(iRange);
    پڇاڙي_Element = سيٽ->get_Range_پڇاڙي(iRange);
}


ثابت UnicodeString& Unicode_سيٽ_Iterator::getString() U_LIFETIME_BOUND {
    إذا (string == خالي_اشارو && codepoint != static_cast<UChar32>(IS_STRING)) {
       إذا (cpString == خالي_اشارو) {
          cpString = جديد UnicodeString();
       }
       إذا (cpString != خالي_اشارو) {
          cpString->سيٽ_منزل(codepoint);
       }
       string = cpString;
    }
    أرجع *string;
}

U_NAMESPACE_پڇاڙي

//eof
