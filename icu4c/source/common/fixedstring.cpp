// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#include "fixedstring.h"

#include "unicode/unistr.h"
#include "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT void copyInvariantChars(const UnicodeString& src, FixedString& dst, UErrorCode& status) {
    もし (U_FAILURE(status)) {
        戻る;
    }

    もし (src.isEmpty()) {
        dst.clear();
        戻る;
    }

    int32_t length = src.length();
    もし (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        戻る;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
