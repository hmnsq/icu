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

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSetIterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
UnicodeSetIterator::UnicodeSetIterator(स्थिरांक UnicodeSet& uSet U_LIFETIME_BOUND) {
    cpString  = nullptr;
    reset(uSet);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
UnicodeSetIterator::UnicodeSetIterator() {
    हे->set = nullptr;
    cpString  = nullptr;
    reset();
}

UnicodeSetIterator::~UnicodeSetIterator() {
    delete cpString;
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
    जर (nextElement <= endElement) {
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        परत सत्य;
    }
    जर (range < endRange) {
        loadRange(++range);
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        परत सत्य;
    }

    जर (nextString >= string_मोजणी) परत असत्य;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<स्थिरांक UnicodeString*>(set->strings_->elementAt(nextString++));
    परत सत्य;
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
UBool UnicodeSetIterator::nextRange() {
    string = nullptr;
    जर (nextElement <= endElement) {
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        परत सत्य;
    }
    जर (range < endRange) {
        loadRange(++range);
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        परत सत्य;
    }

    जर (nextString >= string_मोजणी) परत असत्य;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<स्थिरांक UnicodeString*>(set->strings_->elementAt(nextString++));
    परत सत्य;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
शून्य UnicodeSetIterator::reset(स्थिरांक UnicodeSet& uSet) {
    हे->set = &uSet;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
शून्य UnicodeSetIterator::reset() {
    जर (set == nullptr) {
        // Set up indices to empty iteration
        endRange = -1;
        string_मोजणी = 0;
    } अन्यथा {
        endRange = set->get_Range_मोजणी() - 1;
        string_मोजणी = set->stringsSize();
    }
    range = 0;
    endElement = -1;
    nextElement = 0;            
    जर (endRange >= 0) {
        loadRange(range);
    }
    nextString = 0;
    string = nullptr;
}

शून्य UnicodeSetIterator::loadRange(int32_t iRange) {
    nextElement = set->getRangeStart(iRange);
    endElement = set->getRangeEnd(iRange);
}


स्थिरांक UnicodeString& UnicodeSetIterator::getString() U_LIFETIME_BOUND {
    जर (string == nullptr && codepoint != static_cast<UChar32>(IS_STRING)) {
       जर (cpString == nullptr) {
          cpString = नवीन UnicodeString();
       }
       जर (cpString != nullptr) {
          cpString->setTo(codepoint);
       }
       string = cpString;
    }
    परत *string;
}

U_NAMESPACE_END

//eof
