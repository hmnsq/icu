// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#समावेश "fixedstring.h"

#समावेश "unicode/unistr.h"
#समावेश "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT व्योम copyInvariantChars(स्थिर UnicodeString& src, FixedString& dst, UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }

    अगर (src.isEmpty()) {
        dst.सफा();
        वापस;
    }

    int32_t length = src.length();
    अगर (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
