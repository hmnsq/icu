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

#شامل "unicode/unifilt.h"
#شامل "unicode/rep.h"
#شامل "unicode/utf16.h"

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
UnicodeMatcher* UnicodeFilter::مقصد_Matcher() ثابت U_LIFETIME_BOUND {
  واپس const_cast<UnicodeFilter *>(یہ);
}

خالی UnicodeFilter::سیٹ_ڈیٹا(ثابت Transliteration_قاعدہ_ڈیٹا*) {}

/**
 * Default implementation of UnicodeMatcher::matches() for Unicode
 * filters.  Matches a single code point at offset (either one or
 * two 16-bit code units).
 */
UMatchDegree UnicodeFilter::matches(ثابت Replaceable& متن,
                                    int32_t& انحراف,
                                    int32_t حد_بندی,
                                    UBool incremental) {
    UChar32 c;
    اگر (انحراف < حد_بندی &&
        شامل_ہے(c = متن.char32At(انحراف))) {
        انحراف += U16_لمبائی(c);
        واپس U_مطابقت;
    }
    اگر (انحراف > حد_بندی &&
        شامل_ہے(c = متن.char32At(انحراف))) {
        // Backup offset by 1, unless the preceding character is a
        // surrogate pair -- then backup by 2 (keep offset pointing at
        // the lead surrogate).
        --انحراف;
        اگر (انحراف >= 0) {
            انحراف -= U16_لمبائی(متن.char32At(انحراف)) - 1;
        }
        واپس U_مطابقت;
    }
    اگر (incremental && انحراف == حد_بندی) {
        واپس U_PARTIAL_مطابقت;
    }
    واپس U_MISMATCH;
}

U_NAMESPACE_انتہا

//eof
