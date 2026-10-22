// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#अन्तर्भाव "unicode/usetiter.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_समुच्चयः_Iterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
Unicode_समुच्चयः_Iterator::Unicode_समुच्चयः_Iterator(ध्रुव Unicode_समुच्चयः& u_समुच्चयः U_LIFETIME_BOUND) {
    cpString  = शून्यसूचकः;
    reset(u_समुच्चयः);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
Unicode_समुच्चयः_Iterator::Unicode_समुच्चयः_Iterator() {
    एतत्->समुच्चयः = शून्यसूचकः;
    cpString  = शून्यसूचकः;
    reset();
}

Unicode_समुच्चयः_Iterator::~Unicode_समुच्चयः_Iterator() {
    विलोपय cpString;
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
UBool Unicode_समुच्चयः_Iterator::next() {
    यदि (अग्र_Element <= अन्तः_Element) {
        codepoint = codepoint_अन्तः = अग्र_Element++;
        string = शून्यसूचकः;
        प्रत्यागम सत्य;
    }
    यदि (range < अन्तः_Range) {
        आरोपय_Range(++range);
        codepoint = codepoint_अन्तः = अग्र_Element++;
        string = शून्यसूचकः;
        प्रत्यागम सत्य;
    }

    यदि (अग्र_String >= string_गणनम्) प्रत्यागम असत्य;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ध्रुव UnicodeString*>(समुच्चयः->वाक्यानि_2->elementAt(अग्र_String++));
    प्रत्यागम सत्य;
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
UBool Unicode_समुच्चयः_Iterator::अग्र_Range() {
    string = शून्यसूचकः;
    यदि (अग्र_Element <= अन्तः_Element) {
        codepoint_अन्तः = अन्तः_Element;
        codepoint = अग्र_Element;
        अग्र_Element = अन्तः_Element+1;
        प्रत्यागम सत्य;
    }
    यदि (range < अन्तः_Range) {
        आरोपय_Range(++range);
        codepoint_अन्तः = अन्तः_Element;
        codepoint = अग्र_Element;
        अग्र_Element = अन्तः_Element+1;
        प्रत्यागम सत्य;
    }

    यदि (अग्र_String >= string_गणनम्) प्रत्यागम असत्य;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ध्रुव UnicodeString*>(समुच्चयः->वाक्यानि_2->elementAt(अग्र_String++));
    प्रत्यागम सत्य;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
शून्य Unicode_समुच्चयः_Iterator::reset(ध्रुव Unicode_समुच्चयः& u_समुच्चयः) {
    एतत्->समुच्चयः = &u_समुच्चयः;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
शून्य Unicode_समुच्चयः_Iterator::reset() {
    यदि (समुच्चयः == शून्यसूचकः) {
        // Set up indices to empty iteration
        अन्तः_Range = -1;
        string_गणनम् = 0;
    } अन्यथा {
        अन्तः_Range = समुच्चयः->get_Range_गणनम्() - 1;
        string_गणनम् = समुच्चयः->वाक्यानि_आकार();
    }
    range = 0;
    अन्तः_Element = -1;
    अग्र_Element = 0;            
    यदि (अन्तः_Range >= 0) {
        आरोपय_Range(range);
    }
    अग्र_String = 0;
    string = शून्यसूचकः;
}

शून्य Unicode_समुच्चयः_Iterator::आरोपय_Range(int32_t iRange) {
    अग्र_Element = समुच्चयः->getRangeStart(iRange);
    अन्तः_Element = समुच्चयः->get_Range_अन्तः(iRange);
}


ध्रुव UnicodeString& Unicode_समुच्चयः_Iterator::getString() U_LIFETIME_BOUND {
    यदि (string == शून्यसूचकः && codepoint != static_cast<UChar32>(IS_STRING)) {
       यदि (cpString == शून्यसूचकः) {
          cpString = नूतन UnicodeString();
       }
       यदि (cpString != शून्यसूचकः) {
          cpString->समुच्चयः_गन्तव्यम्(codepoint);
       }
       string = cpString;
    }
    प्रत्यागम *string;
}

U_NAMESPACE_अन्तः

//eof
