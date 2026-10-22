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

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_سېټ_Iterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
Unicode_سېټ_Iterator::Unicode_سېټ_Iterator(ثابت Unicode_سېټ& u_سېټ U_LIFETIME_BOUND) {
    cpString  = تش_اشاره;
    reset(u_سېټ);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
Unicode_سېټ_Iterator::Unicode_سېټ_Iterator() {
    هذا->سېټ = تش_اشاره;
    cpString  = تش_اشاره;
    reset();
}

Unicode_سېټ_Iterator::~Unicode_سېټ_Iterator() {
    ړنګول cpString;
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
UBool Unicode_سېټ_Iterator::next() {
    إذا (بل_Element <= پای_ټکی_Element) {
        codepoint = codepoint_پای_ټکی = بل_Element++;
        string = تش_اشاره;
        أرجع ریښتیا;
    }
    إذا (range < پای_ټکی_Range) {
        بارول_Range(++range);
        codepoint = codepoint_پای_ټکی = بل_Element++;
        string = تش_اشاره;
        أرجع ریښتیا;
    }

    إذا (بل_String >= string_شمېر) أرجع خطأ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ثابت UnicodeString*>(سېټ->مزي_ګانې_2->elementAt(بل_String++));
    أرجع ریښتیا;
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
UBool Unicode_سېټ_Iterator::بل_Range() {
    string = تش_اشاره;
    إذا (بل_Element <= پای_ټکی_Element) {
        codepoint_پای_ټکی = پای_ټکی_Element;
        codepoint = بل_Element;
        بل_Element = پای_ټکی_Element+1;
        أرجع ریښتیا;
    }
    إذا (range < پای_ټکی_Range) {
        بارول_Range(++range);
        codepoint_پای_ټکی = پای_ټکی_Element;
        codepoint = بل_Element;
        بل_Element = پای_ټکی_Element+1;
        أرجع ریښتیا;
    }

    إذا (بل_String >= string_شمېر) أرجع خطأ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ثابت UnicodeString*>(سېټ->مزي_ګانې_2->elementAt(بل_String++));
    أرجع ریښتیا;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
فراغ Unicode_سېټ_Iterator::reset(ثابت Unicode_سېټ& u_سېټ) {
    هذا->سېټ = &u_سېټ;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
فراغ Unicode_سېټ_Iterator::reset() {
    إذا (سېټ == تش_اشاره) {
        // Set up indices to empty iteration
        پای_ټکی_Range = -1;
        string_شمېر = 0;
    } وإلا {
        پای_ټکی_Range = سېټ->get_Range_شمېر() - 1;
        string_شمېر = سېټ->مزي_ګانې_اندازه();
    }
    range = 0;
    پای_ټکی_Element = -1;
    بل_Element = 0;            
    إذا (پای_ټکی_Range >= 0) {
        بارول_Range(range);
    }
    بل_String = 0;
    string = تش_اشاره;
}

فراغ Unicode_سېټ_Iterator::بارول_Range(int32_t iRange) {
    بل_Element = سېټ->get_Range_پیل(iRange);
    پای_ټکی_Element = سېټ->get_Range_پای_ټکی(iRange);
}


ثابت UnicodeString& Unicode_سېټ_Iterator::getString() U_LIFETIME_BOUND {
    إذا (string == تش_اشاره && codepoint != static_cast<UChar32>(IS_STRING)) {
       إذا (cpString == تش_اشاره) {
          cpString = جديد UnicodeString();
       }
       إذا (cpString != تش_اشاره) {
          cpString->سېټ_منزل(codepoint);
       }
       string = cpString;
    }
    أرجع *string;
}

U_NAMESPACE_پای_ټکی

//eof
