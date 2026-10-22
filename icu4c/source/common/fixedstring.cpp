// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#include "fixedstring.h"

#include "unicode/unistr.h"
#include "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT void copyInvariantChars(const UnicodeString& src, FixedString& dst, UErrorCode& status) {
    ถ้า (U_FAILURE(status)) {
        คืน;
    }

    ถ้า (src.isEmpty()) {
        dst.clear();
        คืน;
    }

    int32_t length = src.length();
    ถ้า (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        คืน;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
