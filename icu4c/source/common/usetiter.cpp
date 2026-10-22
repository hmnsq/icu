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

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSetIterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
UnicodeSetIterator::UnicodeSetIterator(ಧ್ರುವ UnicodeSet& uSet U_LIFETIME_BOUND) {
    cpString  = nullptr;
    reset(uSet);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
UnicodeSetIterator::UnicodeSetIterator() {
    ಏತತ್->set = nullptr;
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
    ಯದಿ (nextElement <= endElement) {
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (range < endRange) {
        loadRange(++range);
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    ಯದಿ (nextString >= string_ಎಣಿಕೆ) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ಧ್ರುವ UnicodeString*>(set->strings_->elementAt(nextString++));
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
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
    ಯದಿ (nextElement <= endElement) {
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (range < endRange) {
        loadRange(++range);
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    ಯದಿ (nextString >= string_ಎಣಿಕೆ) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<ಧ್ರುವ UnicodeString*>(set->strings_->elementAt(nextString++));
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
ಶೂನ್ಯ UnicodeSetIterator::reset(ಧ್ರುವ UnicodeSet& uSet) {
    ಏತತ್->set = &uSet;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
ಶೂನ್ಯ UnicodeSetIterator::reset() {
    ಯದಿ (set == nullptr) {
        // Set up indices to empty iteration
        endRange = -1;
        string_ಎಣಿಕೆ = 0;
    } ಅನ್ಯಥಾ {
        endRange = set->get_Range_ಎಣಿಕೆ() - 1;
        string_ಎಣಿಕೆ = set->stringsSize();
    }
    range = 0;
    endElement = -1;
    nextElement = 0;            
    ಯದಿ (endRange >= 0) {
        loadRange(range);
    }
    nextString = 0;
    string = nullptr;
}

ಶೂನ್ಯ UnicodeSetIterator::loadRange(int32_t iRange) {
    nextElement = set->getRangeStart(iRange);
    endElement = set->getRangeEnd(iRange);
}


ಧ್ರುವ UnicodeString& UnicodeSetIterator::getString() U_LIFETIME_BOUND {
    ಯದಿ (string == nullptr && codepoint != static_cast<UChar32>(IS_STRING)) {
       ಯದಿ (cpString == nullptr) {
          cpString = ನೂತನ UnicodeString();
       }
       ಯದಿ (cpString != nullptr) {
          cpString->setTo(codepoint);
       }
       string = cpString;
    }
    ಪ್ರತ್ಯಾಗಮ *string;
}

U_NAMESPACE_END

//eof
