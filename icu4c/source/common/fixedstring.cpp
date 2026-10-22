// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#شامل "fixedstring.h"

#شامل "unicode/unistr.h"
#شامل "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT تهی copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UErrorCode& status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    اگر (src.isEmpty()) {
        dst.clear();
        بازگشت;
    }

    int32_t length = src.length();
    اگر (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
