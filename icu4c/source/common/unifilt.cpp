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

فراغ UnicodeFilter::سیٹ_انکڑے(ثابت Transliteration_قاعدہ_انکڑے*) {}

/**
 * Default implementation of UnicodeMatcher::matches() for Unicode
 * filters.  Matches a single code point at offset (either one or
 * two 16-bit code units).
 */
UMatchDegree UnicodeFilter::matches(ثابت Replaceable& متن,
                                    int32_t& وتھ,
                                    int32_t حد_بندی,
                                    UBool incremental) {
    UChar32 c;
    إذا (وتھ < حد_بندی &&
        شامل_اے(c = متن.char32At(وتھ))) {
        وتھ += U16_لمائی(c);
        أرجع U_میل;
    }
    إذا (وتھ > حد_بندی &&
        شامل_اے(c = متن.char32At(وتھ))) {
        // Backup offset by 1, unless the preceding character is a
        // surrogate pair -- then backup by 2 (keep offset pointing at
        // the lead surrogate).
        --وتھ;
        إذا (وتھ >= 0) {
            وتھ -= U16_لمائی(متن.char32At(وتھ)) - 1;
        }
        أرجع U_میل;
    }
    إذا (incremental && وتھ == حد_بندی) {
        أرجع U_PARTIAL_میل;
    }
    أرجع U_MISMATCH;
}

U_NAMESPACE_سرا

//eof
