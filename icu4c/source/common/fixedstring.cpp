// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#include "fixedstring.h"

#include "unicode/unistr.h"
#include "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT void copyInvariantChars(const UnicodeString& src, FixedString& dst, UErrorCode& status) {
    གལ་སྲིད (U_FAILURE(status)) {
        ལོག;
    }

    གལ་སྲིད (src.isEmpty()) {
        dst.clear();
        ལོག;
    }

    int32_t length = src.length();
    གལ་སྲིད (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ལོག;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
