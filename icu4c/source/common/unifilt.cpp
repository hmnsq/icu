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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unifilt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/rep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"

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
UnicodeMatcher* UnicodeFilter::ਮੰਜ਼ਿਲ_Matcher() ਅਚਲ U_LIFETIME_BOUND {
  ਵਾਪਸ const_cast<UnicodeFilter *>(ਇਹ);
}

ਖਾਲੀ UnicodeFilter::ਸੈੱਟ_ਅੰਕੜੇ(ਅਚਲ Transliteration_ਨਿਯਮ_ਅੰਕੜੇ*) {}

/**
 * Default implementation of UnicodeMatcher::matches() for Unicode
 * filters.  Matches a single code point at offset (either one or
 * two 16-bit code units).
 */
UMatchDegree UnicodeFilter::matches(ਅਚਲ Replaceable& ਪਾਠ,
                                    int32_t& ਵਿੱਥ,
                                    int32_t ਹੱਦਬੰਦੀ,
                                    UBool incremental) {
    UChar32 c;
    ਜੇ (ਵਿੱਥ < ਹੱਦਬੰਦੀ &&
        ਸ਼ਾਮਲ_ਹੈ(c = ਪਾਠ.char32At(ਵਿੱਥ))) {
        ਵਿੱਥ += U16_ਲੰਮਾਈ(c);
        ਵਾਪਸ U_ਮੇਲ;
    }
    ਜੇ (ਵਿੱਥ > ਹੱਦਬੰਦੀ &&
        ਸ਼ਾਮਲ_ਹੈ(c = ਪਾਠ.char32At(ਵਿੱਥ))) {
        // Backup offset by 1, unless the preceding character is a
        // surrogate pair -- then backup by 2 (keep offset pointing at
        // the lead surrogate).
        --ਵਿੱਥ;
        ਜੇ (ਵਿੱਥ >= 0) {
            ਵਿੱਥ -= U16_ਲੰਮਾਈ(ਪਾਠ.char32At(ਵਿੱਥ)) - 1;
        }
        ਵਾਪਸ U_ਮੇਲ;
    }
    ਜੇ (incremental && ਵਿੱਥ == ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ U_PARTIAL_ਮੇਲ;
    }
    ਵਾਪਸ U_MISMATCH;
}

U_NAMESPACE_ਸਿਰਾ

//eof
