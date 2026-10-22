// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#અન્તર્ભાવ "fixedstring.h"

#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT શૂન્ય copyInvariantChars(અચલ UnicodeString& src, FixedString& dst, UErrorCode& status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }

    જો (src.isEmpty()) {
        dst.clear();
        પાછા;
    }

    int32_t length = src.length();
    જો (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
