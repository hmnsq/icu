// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#ଅନ୍ତର୍ଭାଵ "unicode/usetiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSetIterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
UnicodeSetIterator::UnicodeSetIterator(ଅଚଳ UnicodeSet& uSet U_LIFETIME_BOUND) {
    cpString  = nullptr;
    reset(uSet);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
UnicodeSetIterator::UnicodeSetIterator() {
    ଏହି->set = nullptr;
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
    ଯଦି (nextElement <= endElement) {
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (range < endRange) {
        loadRange(++range);
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    ଯଦି (nextString >= string_ଗଣନା) ଫେରନ୍ତୁ ମିଥ୍ୟା;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ଅଚଳ UnicodeString*>(set->strings_->elementAt(nextString++));
    ଫେରନ୍ତୁ ସତ୍ୟ;
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
    ଯଦି (nextElement <= endElement) {
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (range < endRange) {
        loadRange(++range);
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    ଯଦି (nextString >= string_ଗଣନା) ଫେରନ୍ତୁ ମିଥ୍ୟା;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ଅଚଳ UnicodeString*>(set->strings_->elementAt(nextString++));
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
ଶୂନ୍ୟ UnicodeSetIterator::reset(ଅଚଳ UnicodeSet& uSet) {
    ଏହି->set = &uSet;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
ଶୂନ୍ୟ UnicodeSetIterator::reset() {
    ଯଦି (set == nullptr) {
        // Set up indices to empty iteration
        endRange = -1;
        string_ଗଣନା = 0;
    } ଅନ୍ୟଥା {
        endRange = set->get_Range_ଗଣନା() - 1;
        string_ଗଣନା = set->stringsSize();
    }
    range = 0;
    endElement = -1;
    nextElement = 0;            
    ଯଦି (endRange >= 0) {
        loadRange(range);
    }
    nextString = 0;
    string = nullptr;
}

ଶୂନ୍ୟ UnicodeSetIterator::loadRange(int32_t iRange) {
    nextElement = set->getRangeStart(iRange);
    endElement = set->getRangeEnd(iRange);
}


ଅଚଳ UnicodeString& UnicodeSetIterator::getString() U_LIFETIME_BOUND {
    ଯଦି (string == nullptr && codepoint != static_cast<UChar32>(IS_STRING)) {
       ଯଦି (cpString == nullptr) {
          cpString = ନୂତନ UnicodeString();
       }
       ଯଦି (cpString != nullptr) {
          cpString->setTo(codepoint);
       }
       string = cpString;
    }
    ଫେରନ୍ତୁ *string;
}

U_NAMESPACE_END

//eof
