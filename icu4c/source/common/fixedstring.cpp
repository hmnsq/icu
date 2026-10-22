// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#அந்தர்பாவ "fixedstring.h"

#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT வெறுமை copyInvariantChars(மாறிலி UnicodeString& src, FixedString& dst, UErrorCode& status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    எனில் (src.isEmpty()) {
        dst.clear();
        திரும்பு;
    }

    int32_t length = src.length();
    எனில் (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
