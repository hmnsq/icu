// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#അന്തര്ഭാവ "unicode/usetiter.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSetIterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
UnicodeSetIterator::UnicodeSetIterator(സ്ഥിരം UnicodeSet& uSet U_LIFETIME_BOUND) {
    cpString  = nullptr;
    reset(uSet);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
UnicodeSetIterator::UnicodeSetIterator() {
    ഇത്->set = nullptr;
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
    എങ്കിൽ (nextElement <= endElement) {
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        തിരികെ സത്യം;
    }
    എങ്കിൽ (range < endRange) {
        loadRange(++range);
        codepoint = codepointEnd = nextElement++;
        string = nullptr;
        തിരികെ സത്യം;
    }

    എങ്കിൽ (nextString >= string_എണ്ണം) തിരികെ അസത്യം;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<സ്ഥിരം UnicodeString*>(set->strings_->elementAt(nextString++));
    തിരികെ സത്യം;
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
    എങ്കിൽ (nextElement <= endElement) {
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        തിരികെ സത്യം;
    }
    എങ്കിൽ (range < endRange) {
        loadRange(++range);
        codepointEnd = endElement;
        codepoint = nextElement;
        nextElement = endElement+1;
        തിരികെ സത്യം;
    }

    എങ്കിൽ (nextString >= string_എണ്ണം) തിരികെ അസത്യം;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<സ്ഥിരം UnicodeString*>(set->strings_->elementAt(nextString++));
    തിരികെ സത്യം;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
ശൂന്യം UnicodeSetIterator::reset(സ്ഥിരം UnicodeSet& uSet) {
    ഇത്->set = &uSet;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
ശൂന്യം UnicodeSetIterator::reset() {
    എങ്കിൽ (set == nullptr) {
        // Set up indices to empty iteration
        endRange = -1;
        string_എണ്ണം = 0;
    } അല്ലെങ്കിൽ {
        endRange = set->get_Range_എണ്ണം() - 1;
        string_എണ്ണം = set->stringsSize();
    }
    range = 0;
    endElement = -1;
    nextElement = 0;            
    എങ്കിൽ (endRange >= 0) {
        loadRange(range);
    }
    nextString = 0;
    string = nullptr;
}

ശൂന്യം UnicodeSetIterator::loadRange(int32_t iRange) {
    nextElement = set->getRangeStart(iRange);
    endElement = set->getRangeEnd(iRange);
}


സ്ഥിരം UnicodeString& UnicodeSetIterator::getString() U_LIFETIME_BOUND {
    എങ്കിൽ (string == nullptr && codepoint != static_cast<UChar32>(IS_STRING)) {
       എങ്കിൽ (cpString == nullptr) {
          cpString = പുതിയ UnicodeString();
       }
       എങ്കിൽ (cpString != nullptr) {
          cpString->setTo(codepoint);
       }
       string = cpString;
    }
    തിരികെ *string;
}

U_NAMESPACE_END

//eof
