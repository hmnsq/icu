// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/usetiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_ਸੈੱਟ_Iterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
Unicode_ਸੈੱਟ_Iterator::Unicode_ਸੈੱਟ_Iterator(ਅਚਲ Unicode_ਸੈੱਟ& u_ਸੈੱਟ U_LIFETIME_BOUND) {
    cpString  = ਖਾਲੀ_ਸੂਚਕ;
    reset(u_ਸੈੱਟ);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
Unicode_ਸੈੱਟ_Iterator::Unicode_ਸੈੱਟ_Iterator() {
    ਇਹ->ਸੈੱਟ = ਖਾਲੀ_ਸੂਚਕ;
    cpString  = ਖਾਲੀ_ਸੂਚਕ;
    reset();
}

Unicode_ਸੈੱਟ_Iterator::~Unicode_ਸੈੱਟ_Iterator() {
    ਮਿਟਾਓ cpString;
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
UBool Unicode_ਸੈੱਟ_Iterator::next() {
    ਜੇ (ਅਗਲਾ_Element <= ਸਿਰਾ_Element) {
        codepoint = codepoint_ਸਿਰਾ = ਅਗਲਾ_Element++;
        string = ਖਾਲੀ_ਸੂਚਕ;
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (range < ਸਿਰਾ_Range) {
        ਲੱਦੋ_Range(++range);
        codepoint = codepoint_ਸਿਰਾ = ਅਗਲਾ_Element++;
        string = ਖਾਲੀ_ਸੂਚਕ;
        ਵਾਪਸ ਸੱਚ;
    }

    ਜੇ (ਅਗਲਾ_String >= string_ਗਿਣਤੀ) ਵਾਪਸ ਝੂਠ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ਅਚਲ UnicodeString*>(ਸੈੱਟ->ਲੜੀਆਂ_2->elementAt(ਅਗਲਾ_String++));
    ਵਾਪਸ ਸੱਚ;
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
UBool Unicode_ਸੈੱਟ_Iterator::ਅਗਲਾ_Range() {
    string = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (ਅਗਲਾ_Element <= ਸਿਰਾ_Element) {
        codepoint_ਸਿਰਾ = ਸਿਰਾ_Element;
        codepoint = ਅਗਲਾ_Element;
        ਅਗਲਾ_Element = ਸਿਰਾ_Element+1;
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (range < ਸਿਰਾ_Range) {
        ਲੱਦੋ_Range(++range);
        codepoint_ਸਿਰਾ = ਸਿਰਾ_Element;
        codepoint = ਅਗਲਾ_Element;
        ਅਗਲਾ_Element = ਸਿਰਾ_Element+1;
        ਵਾਪਸ ਸੱਚ;
    }

    ਜੇ (ਅਗਲਾ_String >= string_ਗਿਣਤੀ) ਵਾਪਸ ਝੂਠ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ਅਚਲ UnicodeString*>(ਸੈੱਟ->ਲੜੀਆਂ_2->elementAt(ਅਗਲਾ_String++));
    ਵਾਪਸ ਸੱਚ;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
ਖਾਲੀ Unicode_ਸੈੱਟ_Iterator::reset(ਅਚਲ Unicode_ਸੈੱਟ& u_ਸੈੱਟ) {
    ਇਹ->ਸੈੱਟ = &u_ਸੈੱਟ;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
ਖਾਲੀ Unicode_ਸੈੱਟ_Iterator::reset() {
    ਜੇ (ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ) {
        // Set up indices to empty iteration
        ਸਿਰਾ_Range = -1;
        string_ਗਿਣਤੀ = 0;
    } ਨਹੀਂਤਾਂ {
        ਸਿਰਾ_Range = ਸੈੱਟ->get_Range_ਗਿਣਤੀ() - 1;
        string_ਗਿਣਤੀ = ਸੈੱਟ->ਲੜੀਆਂ_ਮਾਪ();
    }
    range = 0;
    ਸਿਰਾ_Element = -1;
    ਅਗਲਾ_Element = 0;            
    ਜੇ (ਸਿਰਾ_Range >= 0) {
        ਲੱਦੋ_Range(range);
    }
    ਅਗਲਾ_String = 0;
    string = ਖਾਲੀ_ਸੂਚਕ;
}

ਖਾਲੀ Unicode_ਸੈੱਟ_Iterator::ਲੱਦੋ_Range(int32_t iRange) {
    ਅਗਲਾ_Element = ਸੈੱਟ->get_Range_ਸ਼ੁਰੂ(iRange);
    ਸਿਰਾ_Element = ਸੈੱਟ->get_Range_ਸਿਰਾ(iRange);
}


ਅਚਲ UnicodeString& Unicode_ਸੈੱਟ_Iterator::getString() U_LIFETIME_BOUND {
    ਜੇ (string == ਖਾਲੀ_ਸੂਚਕ && codepoint != static_cast<UChar32>(IS_STRING)) {
       ਜੇ (cpString == ਖਾਲੀ_ਸੂਚਕ) {
          cpString = ਨਵਾਂ UnicodeString();
       }
       ਜੇ (cpString != ਖਾਲੀ_ਸੂਚਕ) {
          cpString->ਸੈੱਟ_ਮੰਜ਼ਿਲ(codepoint);
       }
       string = cpString;
    }
    ਵਾਪਸ *string;
}

U_NAMESPACE_ਸਿਰਾ

//eof
