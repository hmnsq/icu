// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#অন্তর্ভাৱ "unicode/usetiter.h"
#অন্তর্ভাৱ "unicode/uniset.h"
#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSetIterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
UnicodeSetIterator::UnicodeSetIterator(ধ্রুৱ UnicodeSet& uSet U_LIFETIME_BOUND) {
    cpString  = nullptr;
    reset(uSet);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
UnicodeSetIterator::UnicodeSetIterator() {
    এতত্->set = nullptr;
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
    যদি (nextElement <= endElement) {
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        ঘূৰাই সঁচা;
    }
    যদি (range < endRange) {
        loadRange(++range);
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        ঘূৰাই সঁচা;
    }

    যদি (nextString >= string_গুনতি) ঘূৰাই মিছা;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ধ্রুৱ UnicodeString*>(set->strings_->elementAt(nextString++));
    ঘূৰাই সঁচা;
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
    যদি (nextElement <= endElement) {
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        ঘূৰাই সঁচা;
    }
    যদি (range < endRange) {
        loadRange(++range);
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        ঘূৰাই সঁচা;
    }

    যদি (nextString >= string_গুনতি) ঘূৰাই মিছা;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ধ্রুৱ UnicodeString*>(set->strings_->elementAt(nextString++));
    ঘূৰাই সঁচা;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
শূন্য UnicodeSetIterator::reset(ধ্রুৱ UnicodeSet& uSet) {
    এতত্->set = &uSet;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
শূন্য UnicodeSetIterator::reset() {
    যদি (set == nullptr) {
        // Set up indices to empty iteration
        endRange = -1;
        string_গুনতি = 0;
    } অন্যথা {
        endRange = set->get_Range_গুনতি() - 1;
        string_গুনতি = set->stringsSize();
    }
    range = 0;
    endElement = -1;
    nextElement = 0;            
    যদি (endRange >= 0) {
        loadRange(range);
    }
    nextString = 0;
    string = nullptr;
}

শূন্য UnicodeSetIterator::loadRange(int32_t iRange) {
    nextElement = set->getRangeStart(iRange);
    endElement = set->getRangeEnd(iRange);
}


ধ্রুৱ UnicodeString& UnicodeSetIterator::getString() U_LIFETIME_BOUND {
    যদি (string == nullptr && codepoint != static_cast<UChar32>(IS_STRING)) {
       যদি (cpString == nullptr) {
          cpString = নূতন UnicodeString();
       }
       যদি (cpString != nullptr) {
          cpString->setTo(codepoint);
       }
       string = cpString;
    }
    ঘূৰাই *string;
}

U_NAMESPACE_END

//eof
