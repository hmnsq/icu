// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ਅਨ੍ਤਰ੍ਭਾਵ "fixedstring.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT ਖਾਲੀ copyInvariantChars(ਅਚਲ UnicodeString& src, FixedString& dst, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    ਜੇ (src.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        dst.clear();
        ਵਾਪਸ;
    }

    int32_t length = src.length();
    ਜੇ (!dst.reserve(length + 1)) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_ਸਿਰਾ
