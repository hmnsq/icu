// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ଅନ୍ତର୍ଭାଵ "fixedstring.h"

#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT ଶୂନ୍ୟ copyInvariantChars(ଅଚଳ UnicodeString& src, FixedString& dst, UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି (src.isEmpty()) {
        dst.clear();
        ଫେରନ୍ତୁ;
    }

    int32_t length = src.length();
    ଯଦି (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
