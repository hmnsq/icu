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

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_مجموعه_Iterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
Unicode_مجموعه_Iterator::Unicode_مجموعه_Iterator(ثابت Unicode_مجموعه& u_مجموعه U_LIFETIME_BOUND) {
    cpString  = اشاره_تهی;
    reset(u_مجموعه);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
Unicode_مجموعه_Iterator::Unicode_مجموعه_Iterator() {
    هذا->مجموعه = اشاره_تهی;
    cpString  = اشاره_تهی;
    reset();
}

Unicode_مجموعه_Iterator::~Unicode_مجموعه_Iterator() {
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
UBool Unicode_مجموعه_Iterator::next() {
    إذا (بعدی_Element <= انتها_Element) {
        codepoint = codepoint_انتها = بعدی_Element++;
        string = اشاره_تهی;
        أرجع درست;
    }
    إذا (range < انتها_Range) {
        بارگیری_Range(++range);
        codepoint = codepoint_انتها = بعدی_Element++;
        string = اشاره_تهی;
        أرجع درست;
    }

    إذا (بعدی_String >= string_شمار) أرجع خطأ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ثابت UnicodeString*>(مجموعه->رشتهها_2->elementAt(بعدی_String++));
    أرجع درست;
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
UBool Unicode_مجموعه_Iterator::بعدی_Range() {
    string = اشاره_تهی;
    إذا (بعدی_Element <= انتها_Element) {
        codepoint_انتها = انتها_Element;
        codepoint = بعدی_Element;
        بعدی_Element = انتها_Element+1;
        أرجع درست;
    }
    إذا (range < انتها_Range) {
        بارگیری_Range(++range);
        codepoint_انتها = انتها_Element;
        codepoint = بعدی_Element;
        بعدی_Element = انتها_Element+1;
        أرجع درست;
    }

    إذا (بعدی_String >= string_شمار) أرجع خطأ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ثابت UnicodeString*>(مجموعه->رشتهها_2->elementAt(بعدی_String++));
    أرجع درست;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
فراغ Unicode_مجموعه_Iterator::reset(ثابت Unicode_مجموعه& u_مجموعه) {
    هذا->مجموعه = &u_مجموعه;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
فراغ Unicode_مجموعه_Iterator::reset() {
    إذا (مجموعه == اشاره_تهی) {
        // Set up indices to empty iteration
        انتها_Range = -1;
        string_شمار = 0;
    } وإلا {
        انتها_Range = مجموعه->get_Range_شمار() - 1;
        string_شمار = مجموعه->رشتهها_اندازه();
    }
    range = 0;
    انتها_Element = -1;
    بعدی_Element = 0;            
    إذا (انتها_Range >= 0) {
        بارگیری_Range(range);
    }
    بعدی_String = 0;
    string = اشاره_تهی;
}

فراغ Unicode_مجموعه_Iterator::بارگیری_Range(int32_t iRange) {
    بعدی_Element = مجموعه->get_Range_شروع(iRange);
    انتها_Element = مجموعه->get_Range_انتها(iRange);
}


ثابت UnicodeString& Unicode_مجموعه_Iterator::getString() U_LIFETIME_BOUND {
    إذا (string == اشاره_تهی && codepoint != static_cast<UChar32>(IS_STRING)) {
       إذا (cpString == اشاره_تهی) {
          cpString = جديد UnicodeString();
       }
       إذا (cpString != اشاره_تهی) {
          cpString->مجموعه_مقصد(codepoint);
       }
       string = cpString;
    }
    أرجع *string;
}

U_NAMESPACE_انتها

//eof
