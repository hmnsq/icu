// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#شامل "fixedstring.h"

#شامل "unicode/unistr.h"
#شامل "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT خالی copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    اگر (src.is_خالی_خانے()) {
        dst.clear();
        واپس;
    }

    int32_t length = src.length();
    اگر (!dst.reserve(length + 1)) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_انتہا
