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

#ضمن "unicode/unifilt.h"
#ضمن "unicode/rep.h"
#ضمن "unicode/utf16.h"

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
UnicodeMatcher* UnicodeFilter::منزل_Matcher() ثابت U_LIFETIME_BOUND {
  أرجع const_cast<UnicodeFilter *>(هذا);
}

فراغ UnicodeFilter::سيٽ_معلومات(ثابت Transliteration_قاعدو_معلومات*) {}

/**
 * Default implementation of UnicodeMatcher::matches() for Unicode
 * filters.  Matches a single code point at offset (either one or
 * two 16-bit code units).
 */
UMatchDegree UnicodeFilter::matches(ثابت Replaceable& متن,
                                    int32_t& وٿي,
                                    int32_t حد_بندي,
                                    UBool incremental) {
    UChar32 c;
    إذا (وٿي < حد_بندي &&
        شامل_آهي(c = متن.char32At(وٿي))) {
        وٿي += U16_ڊيگهائي(c);
        أرجع U_ميلاپ;
    }
    إذا (وٿي > حد_بندي &&
        شامل_آهي(c = متن.char32At(وٿي))) {
        // Backup offset by 1, unless the preceding character is a
        // surrogate pair -- then backup by 2 (keep offset pointing at
        // the lead surrogate).
        --وٿي;
        إذا (وٿي >= 0) {
            وٿي -= U16_ڊيگهائي(متن.char32At(وٿي)) - 1;
        }
        أرجع U_ميلاپ;
    }
    إذا (incremental && وٿي == حد_بندي) {
        أرجع U_PARTIAL_ميلاپ;
    }
    أرجع U_MISMATCH;
}

U_NAMESPACE_پڇاڙي

//eof
