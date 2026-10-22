// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#अन्तर्भाव "fixedstring.h"

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT शून्य copyInvariantChars(ध्रुव UnicodeString& src, FixedString& dst, UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    यदि (src.is_रिक्तानि()) {
        dst.clear();
        प्रत्यागम;
    }

    int32_t length = src.length();
    यदि (!dst.reserve(length + 1)) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_अन्तः
