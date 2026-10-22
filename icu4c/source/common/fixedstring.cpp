// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#include "fixedstring.h"

#include "unicode/unistr.h"
#include "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT void copyInvariantChars(const UnicodeString& src, FixedString& dst, UErrorCode& status) {
    ᮚᮦᮔ᮪ (U_FAILURE(status)) {
        ᮝᮜᮤ;
    }

    ᮚᮦᮔ᮪ (src.isEmpty()) {
        dst.clear();
        ᮝᮜᮤ;
    }

    int32_t length = src.length();
    ᮚᮦᮔ᮪ (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ᮝᮜᮤ;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
