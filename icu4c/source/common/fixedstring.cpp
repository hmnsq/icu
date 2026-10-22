// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

#ضمن "fixedstring.h"

#ضمن "unicode/unistr.h"
#ضمن "unicode/utypes.h"

U_NAMESPACE_BEGIN

U_EXPORT فراغ copyInvariantChars(ثابت UnicodeString& src, FixedString& dst, UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    إذا (src.is_خالی()) {
        dst.clear();
        أرجع;
    }

    int32_t length = src.length();
    إذا (!dst.reserve(length + 1)) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    src.extract(0, length, dst.getAlias(), length + 1, US_INV);
}

U_NAMESPACE_انتها
