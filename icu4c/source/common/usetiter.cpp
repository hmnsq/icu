// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2006, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#అన్తర్భావ "unicode/usetiter.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "uvector.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_సమితి_Iterator)

/**
 * Create an iterator
 * @param set set to iterate over
 */
Unicode_సమితి_Iterator::Unicode_సమితి_Iterator(స్థిరరాశి Unicode_సమితి& u_సమితి U_LIFETIME_BOUND) {
    cpString  = శూన్యసూచిక;
    reset(u_సమితి);
}

/**
 * Create an iterator. Convenience for when the contents are to be set later.
 */
Unicode_సమితి_Iterator::Unicode_సమితి_Iterator() {
    ఇది->సమితి = శూన్యసూచిక;
    cpString  = శూన్యసూచిక;
    reset();
}

Unicode_సమితి_Iterator::~Unicode_సమితి_Iterator() {
    తొలగించు cpString;
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
UBool Unicode_సమితి_Iterator::next() {
    ఒకవేళ (తదుపరి_Element <= చివర_Element) {
        codepoint = codepoint_చివర = తదుపరి_Element++;
        string = శూన్యసూచిక;
        తిరిగి నిజం;
    }
    ఒకవేళ (range < చివర_Range) {
        ఎక్కించు_Range(++range);
        codepoint = codepoint_చివర = తదుపరి_Element++;
        string = శూన్యసూచిక;
        తిరిగి నిజం;
    }

    ఒకవేళ (తదుపరి_String >= string_లెక్క) తిరిగి అబద్ధం;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<స్థిరరాశి UnicodeString*>(సమితి->అక్షరశ్రేణులు_2->elementAt(తదుపరి_String++));
    తిరిగి నిజం;
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
UBool Unicode_సమితి_Iterator::తదుపరి_Range() {
    string = శూన్యసూచిక;
    ఒకవేళ (తదుపరి_Element <= చివర_Element) {
        codepoint_చివర = చివర_Element;
        codepoint = తదుపరి_Element;
        తదుపరి_Element = చివర_Element+1;
        తిరిగి నిజం;
    }
    ఒకవేళ (range < చివర_Range) {
        ఎక్కించు_Range(++range);
        codepoint_చివర = చివర_Element;
        codepoint = తదుపరి_Element;
        తదుపరి_Element = చివర_Element+1;
        తిరిగి నిజం;
    }

    ఒకవేళ (తదుపరి_String >= string_లెక్క) తిరిగి అబద్ధం;
    codepoint = static_cast<UChar32>(IS_STRING); // signal that value is actually a string
    string = static_cast<స్థిరరాశి UnicodeString*>(సమితి->అక్షరశ్రేణులు_2->elementAt(తదుపరి_String++));
    తిరిగి నిజం;
}

/**
 *@param set the set to iterate over. This allows reuse of the iterator.
 */
శూన్య Unicode_సమితి_Iterator::reset(స్థిరరాశి Unicode_సమితి& u_సమితి) {
    ఇది->సమితి = &u_సమితి;
    reset();
}

/**
 * Resets to the start, to allow the iteration to start over again.
 */
శూన్య Unicode_సమితి_Iterator::reset() {
    ఒకవేళ (సమితి == శూన్యసూచిక) {
        // Set up indices to empty iteration
        చివర_Range = -1;
        string_లెక్క = 0;
    } లేకపోతే {
        చివర_Range = సమితి->get_Range_లెక్క() - 1;
        string_లెక్క = సమితి->అక్షరశ్రేణులు_కొలత();
    }
    range = 0;
    చివర_Element = -1;
    తదుపరి_Element = 0;            
    ఒకవేళ (చివర_Range >= 0) {
        ఎక్కించు_Range(range);
    }
    తదుపరి_String = 0;
    string = శూన్యసూచిక;
}

శూన్య Unicode_సమితి_Iterator::ఎక్కించు_Range(int32_t iRange) {
    తదుపరి_Element = సమితి->get_Range_ప్రారంభం(iRange);
    చివర_Element = సమితి->get_Range_చివర(iRange);
}


స్థిరరాశి UnicodeString& Unicode_సమితి_Iterator::getString() U_LIFETIME_BOUND {
    ఒకవేళ (string == శూన్యసూచిక && codepoint != static_cast<UChar32>(IS_STRING)) {
       ఒకవేళ (cpString == శూన్యసూచిక) {
          cpString = కొత్త UnicodeString();
       }
       ఒకవేళ (cpString != శూన్యసూచిక) {
          cpString->సమితి_గమ్యం(codepoint);
       }
       string = cpString;
    }
    తిరిగి *string;
}

U_NAMESPACE_చివర

//eof
