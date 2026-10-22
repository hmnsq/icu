// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#अन्तर्भाव "fixedstring.h"

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT शून्य copyInvariantChars(स्थिरांक UnicodeString& src, FixedString& dst, UErrorCode& status) {
    जर (U_FAILURE(status)) {
        परत;
    }

    जर (src.isEmpty()) {
        dst.clear();
        परत;
    }

    int32_t length = src.length();
    जर (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
