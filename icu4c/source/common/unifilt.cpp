// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2001-2012, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   07/18/01    aliu        Creation.
**********************************************************************
*/

#అన్తర్భావ "unicode/unifilt.h"
#అన్తర్భావ "unicode/rep.h"
#అన్తర్భావ "unicode/utf16.h"

U_NAMESPACE_BEGIN
UOBJECT_DEFINE_ABSTRACT_RTTI_IMPLEMENTATION(UnicodeFilter)


/* Define this here due to the lack of another file.
   It can't be defined in the header */
UnicodeMatcher::~UnicodeMatcher() {}

UnicodeFilter::~UnicodeFilter() {}

/**
 * UnicodeFunctor API.
 *   Note that UnicodeMatcher is a base class of UnicodeFilter.
 */
UnicodeMatcher* UnicodeFilter::గమ్యం_Matcher() స్థిరరాశి U_LIFETIME_BOUND {
  తిరిగి const_cast<UnicodeFilter *>(ఇది);
}

శూన్య UnicodeFilter::సమితి_దత్తాంశం(స్థిరరాశి Transliteration_నియమం_దత్తాంశం*) {}

/**
 * Default implementation of UnicodeMatcher::matches() for Unicode
 * filters.  Matches a single code point at offset (either one or
 * two 16-bit code units).
 */
UMatchDegree UnicodeFilter::matches(స్థిరరాశి Replaceable& పాఠ్యం,
                                    int32_t& స్థానభ్రంశం,
                                    int32_t హద్దు,
                                    UBool incremental) {
    UChar32 c;
    ఒకవేళ (స్థానభ్రంశం < హద్దు &&
        కలిగియుంది(c = పాఠ్యం.char32At(స్థానభ్రంశం))) {
        స్థానభ్రంశం += U16_నిడివి(c);
        తిరిగి U_సరిపోలిక;
    }
    ఒకవేళ (స్థానభ్రంశం > హద్దు &&
        కలిగియుంది(c = పాఠ్యం.char32At(స్థానభ్రంశం))) {
        // Backup offset by 1, unless the preceding character is a
        // surrogate pair -- then backup by 2 (keep offset pointing at
        // the lead surrogate).
        --స్థానభ్రంశం;
        ఒకవేళ (స్థానభ్రంశం >= 0) {
            స్థానభ్రంశం -= U16_నిడివి(పాఠ్యం.char32At(స్థానభ్రంశం)) - 1;
        }
        తిరిగి U_సరిపోలిక;
    }
    ఒకవేళ (incremental && స్థానభ్రంశం == హద్దు) {
        తిరిగి U_PARTIAL_సరిపోలిక;
    }
    తిరిగి U_MISMATCH;
}

U_NAMESPACE_చివర

//eof
