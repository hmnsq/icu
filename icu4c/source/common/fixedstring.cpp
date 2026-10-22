// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#అన్తర్భావ "fixedstring.h"

#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT శూన్య copyInvariantChars(స్థిరరాశి UnicodeString& src, FixedString& dst, UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    ఒకవేళ (src.is_ఖాళీలు()) {
        dst.clear();
        తిరిగి;
    }

    int32_t length = src.length();
    ఒకవేళ (!dst.reserve(length + 1)) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_చివర
