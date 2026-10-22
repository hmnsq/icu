// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#அந்தர்பாவ "unicode/usetiter.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSetIterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
UnicodeSetIterator::UnicodeSetIterator(மாறிலி UnicodeSet& uSet U_LIFETIME_BOUND) {
    cpString  = nullptr;
    reset(uSet);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
UnicodeSetIterator::UnicodeSetIterator() {
    இது->set = nullptr;
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
    எனில் (nextElement <= endElement) {
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        திரும்பு உண்மை;
    }
    எனில் (range < endRange) {
        loadRange(++range);
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        திரும்பு உண்மை;
    }

    எனில் (nextString >= string_மொத்தம்) திரும்பு பொய்;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<மாறிலி UnicodeString*>(set->strings_->elementAt(nextString++));
    திரும்பு உண்மை;
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
    எனில் (nextElement <= endElement) {
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        திரும்பு உண்மை;
    }
    எனில் (range < endRange) {
        loadRange(++range);
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        திரும்பு உண்மை;
    }

    எனில் (nextString >= string_மொத்தம்) திரும்பு பொய்;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<மாறிலி UnicodeString*>(set->strings_->elementAt(nextString++));
    திரும்பு உண்மை;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
வெறுமை UnicodeSetIterator::reset(மாறிலி UnicodeSet& uSet) {
    இது->set = &uSet;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
வெறுமை UnicodeSetIterator::reset() {
    எனில் (set == nullptr) {
        // Set up indices to empty iteration
        endRange = -1;
        string_மொத்தம் = 0;
    } இல்லையெனில் {
        endRange = set->get_Range_மொத்தம்() - 1;
        string_மொத்தம் = set->stringsSize();
    }
    range = 0;
    endElement = -1;
    nextElement = 0;            
    எனில் (endRange >= 0) {
        loadRange(range);
    }
    nextString = 0;
    string = nullptr;
}

வெறுமை UnicodeSetIterator::loadRange(int32_t iRange) {
    nextElement = set->getRangeStart(iRange);
    endElement = set->getRangeEnd(iRange);
}


மாறிலி UnicodeString& UnicodeSetIterator::getString() U_LIFETIME_BOUND {
    எனில் (string == nullptr && codepoint != static_cast<UChar32>(IS_STRING)) {
       எனில் (cpString == nullptr) {
          cpString = புதிய UnicodeString();
       }
       எனில் (cpString != nullptr) {
          cpString->setTo(codepoint);
       }
       string = cpString;
    }
    திரும்பு *string;
}

U_NAMESPACE_END

//eof
