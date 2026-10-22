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

#अन्तर्भाव "unicode/unifilt.h"
#अन्तर्भाव "unicode/rep.h"
#अन्तर्भाव "unicode/utf16.h"

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
UnicodeMatcher* UnicodeFilter::गन्तव्यम्_Matcher() ध्रुव U_LIFETIME_BOUND {
  प्रत्यागम const_cast<UnicodeFilter *>(एतत्);
}

शून्य UnicodeFilter::समुच्चयः_दत्त(ध्रुव Transliteration_नियमः_दत्त*) {}

/**
 * Default implementation of UnicodeMatcher::matches() for Unicode
 * filters.  Matches a single code point at offset (either one or
 * two 16-bit code units).
 */
UMatchDegree UnicodeFilter::matches(ध्रुव Replaceable& text,
                                    int32_t& विस्थापनम्,
                                    int32_t सीमा,
                                    UBool incremental) {
    UChar32 c;
    यदि (विस्थापनम् < सीमा &&
        अन्तर्भवति(c = text.char32At(विस्थापनम्))) {
        विस्थापनम् += U16_दीर्घता(c);
        प्रत्यागम U_मेलनम्;
    }
    यदि (विस्थापनम् > सीमा &&
        अन्तर्भवति(c = text.char32At(विस्थापनम्))) {
        // Backup offset by 1, unless the preceding character is a
        // surrogate pair -- then backup by 2 (keep offset pointing at
        // the lead surrogate).
        --विस्थापनम्;
        यदि (विस्थापनम् >= 0) {
            विस्थापनम् -= U16_दीर्घता(text.char32At(विस्थापनम्)) - 1;
        }
        प्रत्यागम U_मेलनम्;
    }
    यदि (incremental && विस्थापनम् == सीमा) {
        प्रत्यागम U_PARTIAL_मेलनम्;
    }
    प्रत्यागम U_MISMATCH;
}

U_NAMESPACE_अन्तः

//eof
