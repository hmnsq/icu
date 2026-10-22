// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#include "fixedstring.h"

#include "unicode/unistr.h"
#include "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT void copyInvariantChars(const UnicodeString& src, FixedString& dst, UErrorCode& status) {
    නම් (U_FAILURE(status)) {
        ආපසු;
    }

    නම් (src.isEmpty()) {
        dst.clear();
        ආපසු;
    }

    int32_t length = src.length();
    නම් (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ආපසු;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
