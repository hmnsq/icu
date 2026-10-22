// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#include "fixedstring.h"

#include "unicode/unistr.h"
#include "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT void copyInvariantChars(const UnicodeString& src, FixedString& dst, UErrorCode& status) {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }

    ຖ້າ (src.isEmpty()) {
        dst.clear();
        ສົ່ງຄືນ;
    }

    int32_t length = src.length();
    ຖ້າ (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
