// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#समावेश "unicode/usetiter.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/unistr.h"
#समावेश "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSetIterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
UnicodeSetIterator::UnicodeSetIterator(स्थिर UnicodeSet& uSet U_LIFETIME_BOUND) {
    cpString  = nullptr;
    reset(uSet);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
UnicodeSetIterator::UnicodeSetIterator() {
    यह->set = nullptr;
    cpString  = nullptr;
    reset();
}

UnicodeSetIterator::~UnicodeSetIterator() {
    मिटाओ cpString;
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
UBool UnicodeSetIterator::next() {
    अगर (अग्र_Element <= endElement) {
        codepoint = codepointEnd = अग्र_Element++;
        string = nullptr;
        वापस true;
    }
    अगर (range < endRange) {
        loadRange(++range);
        codepoint = codepointEnd = अग्र_Element++;
        string = nullptr;
        वापस true;
    }

    अगर (अग्र_String >= string_गिनती) वापस false;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<स्थिर UnicodeString*>(set->strings_->elementAt(अग्र_String++));
    वापस true;
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
UBool UnicodeSetIterator::अग्र_Range() {
    string = nullptr;
    अगर (अग्र_Element <= endElement) {
        codepointEnd = endElement;
        codepoint = अग्र_Element;
        अग्र_Element = endElement+1;
        वापस true;
    }
    अगर (range < endRange) {
        loadRange(++range);
        codepointEnd = endElement;
        codepoint = अग्र_Element;
        अग्र_Element = endElement+1;
        वापस true;
    }

    अगर (अग्र_String >= string_गिनती) वापस false;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<स्थिर UnicodeString*>(set->strings_->elementAt(अग्र_String++));
    वापस true;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
व्योम UnicodeSetIterator::reset(स्थिर UnicodeSet& uSet) {
    यह->set = &uSet;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
व्योम UnicodeSetIterator::reset() {
    अगर (set == nullptr) {
        // Set up indices to empty iteration
        endRange = -1;
        string_गिनती = 0;
    } अन्यथा {
        endRange = set->get_Range_गिनती() - 1;
        string_गिनती = set->strings_आकार();
    }
    range = 0;
    endElement = -1;
    अग्र_Element = 0;            
    अगर (endRange >= 0) {
        loadRange(range);
    }
    अग्र_String = 0;
    string = nullptr;
}

व्योम UnicodeSetIterator::loadRange(int32_t iRange) {
    अग्र_Element = set->getRangeStart(iRange);
    endElement = set->getRangeEnd(iRange);
}


स्थिर UnicodeString& UnicodeSetIterator::getString() U_LIFETIME_BOUND {
    अगर (string == nullptr && codepoint != static_cast<UChar32>(IS_STRING)) {
       अगर (cpString == nullptr) {
          cpString = नया UnicodeString();
       }
       अगर (cpString != nullptr) {
          cpString->setTo(codepoint);
       }
       string = cpString;
    }
    वापस *string;
}

U_NAMESPACE_END

//eof
