// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ضمن "fixedstring.h"

#ضمن "unicode/unistr.h"
#ضمن "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT فراغ copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    إذا (src.is_تشه()) {
        dst.clear();
        أرجع;
    }

    int32_t length = src.length();
    إذا (!dst.reserve(length + 1)) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_پای_ټکی
