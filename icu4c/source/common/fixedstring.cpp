// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#include "fixedstring.h"

#include "unicode/unistr.h"
#include "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT void copyInvariantChars(const UnicodeString& src, FixedString& dst, UErrorCode& status) {
    အကယ်၍ (U_FAILURE(status)) {
        ပြန်ပေး;
    }

    အကယ်၍ (src.isEmpty()) {
        dst.clear();
        ပြန်ပေး;
    }

    int32_t length = src.length();
    အကယ်၍ (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ပြန်ပေး;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
