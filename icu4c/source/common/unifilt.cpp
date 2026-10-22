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
UnicodeMatcher* UnicodeFilter::toMatcher() ध्रुव U_LIFETIME_BOUND {
  प्रत्यागम const_cast<UnicodeFilter *>(एतत्);
}

शून्य UnicodeFilter::setData(ध्रुव TransliterationRuleData*) {}

/**
 * Default implementation of UnicodeMatcher::matches() for Unicode
 * filters.  Matches a single code point at offset (either one or
 * two 16-bit code units).
 */
UMatchDegree UnicodeFilter::matches(ध्रुव Replaceable& text,
                                    int32_t& offset,
                                    int32_t limit,
                                    UBool incremental) {
    UChar32 c;
    यदि (offset < limit &&
        contains(c = text.char32At(offset))) {
        offset += U16_LENGTH(c);
        प्रत्यागम U_MATCH;
    }
    यदि (offset > limit &&
        contains(c = text.char32At(offset))) {
        // Backup offset by 1, unless the preceding character is a
        // surrogate pair -- then backup by 2 (keep offset pointing at
        // the lead surrogate).
        --offset;
        यदि (offset >= 0) {
            offset -= U16_LENGTH(text.char32At(offset)) - 1;
        }
        प्रत्यागम U_MATCH;
    }
    यदि (incremental && offset == limit) {
        प्रत्यागम U_PARTIAL_MATCH;
    }
    प्रत्यागम U_MISMATCH;
}

U_NAMESPACE_END

//eof
