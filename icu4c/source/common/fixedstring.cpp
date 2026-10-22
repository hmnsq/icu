// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#അന്തര്ഭാവ "fixedstring.h"

#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT ശൂന്യം copyInvariantChars(സ്ഥിരം UnicodeString& src, FixedString& dst, UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    എങ്കിൽ (src.isEmpty()) {
        dst.clear();
        തിരികെ;
    }

    int32_t length = src.length();
    എങ്കിൽ (!dst.reserve(length + 1)) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_END
