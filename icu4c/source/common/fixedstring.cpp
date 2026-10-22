// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ "fixedstring.h"

#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT ಶೂನ್ಯ copyInvariantChars(ಧ್ರುವ UnicodeString& src, FixedString& dst, UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (src.isEmpty()) {
        dst.clear();
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t length = src.length();
    ಯದಿ (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
