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
UnicodeMatcher* UnicodeFilter::toMatcher() ثابت U_LIFETIME_BOUND {
  إرجاع const_cast<UnicodeFilter *>(هذا);
}

فراغ UnicodeFilter::setData(ثابت TransliterationRuleData*) {}

/**
 * Default implementation of UnicodeMatcher::matches() for Unicode
 * filters.  Matches a single code point at offset (either one or
 * two 16-bit code units).
 */
UMatchDegree UnicodeFilter::matches(ثابت Replaceable& text,
                                    int32_t& offset,
                                    int32_t limit,
                                    UBool incremental) {
    UChar32 c;
    إذا (offset < limit &&
        contains(c = text.char32At(offset))) {
        offset += U16_LENGTH(c);
        إرجاع U_MATCH;
    }
    إذا (offset > limit &&
        contains(c = text.char32At(offset))) {
        // Backup offset by 1, unless the preceding character is a
        // surrogate pair -- then backup by 2 (keep offset pointing at
        // the lead surrogate).
        --offset;
        إذا (offset >= 0) {
            offset -= U16_LENGTH(text.char32At(offset)) - 1;
        }
        إرجاع U_MATCH;
    }
    إذا (incremental && offset == limit) {
        إرجاع U_PARTIAL_MATCH;
    }
    إرجاع U_MISMATCH;
}

U_NAMESPACE_END

//eof
