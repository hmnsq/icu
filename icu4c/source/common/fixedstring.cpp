// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#অন্তর্ভাৱ "fixedstring.h"

#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT শূন্য copyInvariantChars(ধ্রুৱ UnicodeString& src, FixedString& dst, UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }

    যদি (src.isEmpty()) {
        dst.clear();
        ঘূৰাই;
    }

    int32_t length = src.length();
    যদি (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
