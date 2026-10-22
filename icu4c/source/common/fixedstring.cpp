// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ضمن "fixedstring.h"

#ضمن "unicode/unistr.h"
#ضمن "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT فراغ copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UErrorCode& status) {
    אם (U_FAILURE(status)) {
        החזר;
    }

    אם (src.isEmpty()) {
        dst.clear();
        החזר;
    }

    int32_t length = src.length();
    אם (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        החזר;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
