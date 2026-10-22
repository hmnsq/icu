// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#অন্তর্ভাব "unicode/usetiter.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSetIterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
UnicodeSetIterator::UnicodeSetIterator(ধ্রুবক UnicodeSet& uSet U_LIFETIME_BOUND) {
    cpString  = nullptr;
    reset(uSet);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
UnicodeSetIterator::UnicodeSetIterator() {
    এই->set = nullptr;
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
    যদি (পরবর্তী_Element <= endElement) {
        codepoint = codepointEnd = পরবর্তী_Element++;
        string = nullptr;
        ফেরত সত্য;
    }
    যদি (range < endRange) {
        loadRange(++range);
        codepoint = codepointEnd = পরবর্তী_Element++;
        string = nullptr;
        ফেরত সত্য;
    }

    যদি (পরবর্তী_String >= string_গুনতি) ফেরত মিথ্যা;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ধ্রুবক UnicodeString*>(set->strings_->elementAt(পরবর্তী_String++));
    ফেরত সত্য;
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
UBool UnicodeSetIterator::পরবর্তী_Range() {
    string = nullptr;
    যদি (পরবর্তী_Element <= endElement) {
        codepointEnd = endElement;
        codepoint = পরবর্তী_Element;
        পরবর্তী_Element = endElement+1;
        ফেরত সত্য;
    }
    যদি (range < endRange) {
        loadRange(++range);
        codepointEnd = endElement;
        codepoint = পরবর্তী_Element;
        পরবর্তী_Element = endElement+1;
        ফেরত সত্য;
    }

    যদি (পরবর্তী_String >= string_গুনতি) ফেরত মিথ্যা;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ধ্রুবক UnicodeString*>(set->strings_->elementAt(পরবর্তী_String++));
    ফেরত সত্য;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
শূন্য UnicodeSetIterator::reset(ধ্রুবক UnicodeSet& uSet) {
    এই->set = &uSet;
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
        string_গুনতি = set->strings_আকার();
    }
    range = 0;
    endElement = -1;
    পরবর্তী_Element = 0;            
    যদি (endRange >= 0) {
        loadRange(range);
    }
    পরবর্তী_String = 0;
    string = nullptr;
}

শূন্য UnicodeSetIterator::loadRange(int32_t iRange) {
    পরবর্তী_Element = set->getRangeStart(iRange);
    endElement = set->getRangeEnd(iRange);
}


ধ্রুবক UnicodeString& UnicodeSetIterator::getString() U_LIFETIME_BOUND {
    যদি (string == nullptr && codepoint != static_cast<UChar32>(IS_STRING)) {
       যদি (cpString == nullptr) {
          cpString = নতুন UnicodeString();
       }
       যদি (cpString != nullptr) {
          cpString->setTo(codepoint);
       }
       string = cpString;
    }
    ফেরত *string;
}

U_NAMESPACE_END

//eof
