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

#ಅನ್ತರ್ಭಾವ "unicode/unifilt.h"
#ಅನ್ತರ್ಭಾವ "unicode/rep.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"

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
UnicodeMatcher* UnicodeFilter::ಗಮ್ಯ_Matcher() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
  ಹಿಂತಿರುಗಿ const_cast<UnicodeFilter *>(ಇದು);
}

ಶೂನ್ಯ UnicodeFilter::ಗಣ_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ Transliteration_ನಿಯಮ_ದತ್ತಾಂಶ*) {}

/**
 * Default implementation of UnicodeMatcher::matches() for Unicode
 * filters.  Matches a single code point at offset (either one or
 * two 16-bit code units).
 */
UMatchDegree UnicodeFilter::matches(ಸ್ಥಿರಾಂಕ Replaceable& ಪಠ್ಯ,
                                    int32_t& ಸ್ಥಾನಾಂತರ,
                                    int32_t ಎಲ್ಲೆ,
                                    UBool incremental) {
    UChar32 c;
    ಆದರೆ (ಸ್ಥಾನಾಂತರ < ಎಲ್ಲೆ &&
        ಒಳಗೊಂಡಿದೆ(c = ಪಠ್ಯ.char32At(ಸ್ಥಾನಾಂತರ))) {
        ಸ್ಥಾನಾಂತರ += U16_ಉದ್ದಳತೆ(c);
        ಹಿಂತಿರುಗಿ U_ಹೊಂದಿಕೆ;
    }
    ಆದರೆ (ಸ್ಥಾನಾಂತರ > ಎಲ್ಲೆ &&
        ಒಳಗೊಂಡಿದೆ(c = ಪಠ್ಯ.char32At(ಸ್ಥಾನಾಂತರ))) {
        // Backup offset by 1, unless the preceding character is a
        // surrogate pair -- then backup by 2 (keep offset pointing at
        // the lead surrogate).
        --ಸ್ಥಾನಾಂತರ;
        ಆದರೆ (ಸ್ಥಾನಾಂತರ >= 0) {
            ಸ್ಥಾನಾಂತರ -= U16_ಉದ್ದಳತೆ(ಪಠ್ಯ.char32At(ಸ್ಥಾನಾಂತರ)) - 1;
        }
        ಹಿಂತಿರುಗಿ U_ಹೊಂದಿಕೆ;
    }
    ಆದರೆ (incremental && ಸ್ಥಾನಾಂತರ == ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ U_PARTIAL_ಹೊಂದಿಕೆ;
    }
    ಹಿಂತಿರುಗಿ U_MISMATCH;
}

U_NAMESPACE_ಕೊನೆ

//eof
