// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#ಅನ್ತರ್ಭಾವ "unicode/usetiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_ಗಣ_Iterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
Unicode_ಗಣ_Iterator::Unicode_ಗಣ_Iterator(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& u_ಗಣ U_LIFETIME_BOUND) {
    cpString  = ಶೂನ್ಯಸೂಚಕ;
    reset(u_ಗಣ);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
Unicode_ಗಣ_Iterator::Unicode_ಗಣ_Iterator() {
    ಇದು->ಗಣ = ಶೂನ್ಯಸೂಚಕ;
    cpString  = ಶೂನ್ಯಸೂಚಕ;
    reset();
}

Unicode_ಗಣ_Iterator::~Unicode_ಗಣ_Iterator() {
    ಅಳಿಸು cpString;
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
UBool Unicode_ಗಣ_Iterator::next() {
    ಆದರೆ (ಮುಂದಿನ_Element <= ಕೊನೆ_Element) {
        codepoint = codepoint_ಕೊನೆ = ಮುಂದಿನ_Element++;
        string = ಶೂನ್ಯಸೂಚಕ;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (range < ಕೊನೆ_Range) {
        ತುಂಬು_Range(++range);
        codepoint = codepoint_ಕೊನೆ = ಮುಂದಿನ_Element++;
        string = ಶೂನ್ಯಸೂಚಕ;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    ಆದರೆ (ಮುಂದಿನ_String >= string_ಎಣಿಕೆ) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಗಣ->ಅಕ್ಷರಸರಣಿಗಳು_2->elementAt(ಮುಂದಿನ_String++));
    ಹಿಂತಿರುಗಿ ನಿಜ;
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
UBool Unicode_ಗಣ_Iterator::ಮುಂದಿನ_Range() {
    string = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (ಮುಂದಿನ_Element <= ಕೊನೆ_Element) {
        codepoint_ಕೊನೆ = ಕೊನೆ_Element;
        codepoint = ಮುಂದಿನ_Element;
        ಮುಂದಿನ_Element = ಕೊನೆ_Element+1;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (range < ಕೊನೆ_Range) {
        ತುಂಬು_Range(++range);
        codepoint_ಕೊನೆ = ಕೊನೆ_Element;
        codepoint = ಮುಂದಿನ_Element;
        ಮುಂದಿನ_Element = ಕೊನೆ_Element+1;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    ಆದರೆ (ಮುಂದಿನ_String >= string_ಎಣಿಕೆ) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಗಣ->ಅಕ್ಷರಸರಣಿಗಳು_2->elementAt(ಮುಂದಿನ_String++));
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
ಶೂನ್ಯ Unicode_ಗಣ_Iterator::reset(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& u_ಗಣ) {
    ಇದು->ಗಣ = &u_ಗಣ;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
ಶೂನ್ಯ Unicode_ಗಣ_Iterator::reset() {
    ಆದರೆ (ಗಣ == ಶೂನ್ಯಸೂಚಕ) {
        // Set up indices to empty iteration
        ಕೊನೆ_Range = -1;
        string_ಎಣಿಕೆ = 0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಕೊನೆ_Range = ಗಣ->get_Range_ಎಣಿಕೆ() - 1;
        string_ಎಣಿಕೆ = ಗಣ->ಅಕ್ಷರಸರಣಿಗಳು_ಗಾತ್ರ();
    }
    range = 0;
    ಕೊನೆ_Element = -1;
    ಮುಂದಿನ_Element = 0;            
    ಆದರೆ (ಕೊನೆ_Range >= 0) {
        ತುಂಬು_Range(range);
    }
    ಮುಂದಿನ_String = 0;
    string = ಶೂನ್ಯಸೂಚಕ;
}

ಶೂನ್ಯ Unicode_ಗಣ_Iterator::ತುಂಬು_Range(int32_t iRange) {
    ಮುಂದಿನ_Element = ಗಣ->get_Range_ಆರಂಭ(iRange);
    ಕೊನೆ_Element = ಗಣ->get_Range_ಕೊನೆ(iRange);
}


ಸ್ಥಿರಾಂಕ UnicodeString& Unicode_ಗಣ_Iterator::getString() U_LIFETIME_BOUND {
    ಆದರೆ (string == ಶೂನ್ಯಸೂಚಕ && codepoint != static_cast<UChar32>(IS_STRING)) {
       ಆದರೆ (cpString == ಶೂನ್ಯಸೂಚಕ) {
          cpString = ಹೊಸ UnicodeString();
       }
       ಆದರೆ (cpString != ಶೂನ್ಯಸೂಚಕ) {
          cpString->ಗಣ_ಗಮ್ಯ(codepoint);
       }
       string = cpString;
    }
    ಹಿಂತಿರುಗಿ *string;
}

U_NAMESPACE_ಕೊನೆ

//eof
