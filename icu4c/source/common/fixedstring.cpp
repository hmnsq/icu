// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ "fixedstring.h"

#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT ಶೂನ್ಯ copyInvariantChars(ಸ್ಥಿರಾಂಕ UnicodeString& src, FixedString& dst, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (src.is_ಖಾಲಿಗಳು()) {
        dst.clear();
        ಹಿಂತಿರುಗಿ;
    }

    int32_t length = src.length();
    ಆದರೆ (!dst.reserve(length + 1)) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_ಕೊನೆ
