// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#شامل <utility>

#شامل "unicode/bytestream.h"
#شامل "unicode/localebuilder.h"
#شامل "unicode/locid.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/ulocbuilder.h"
#شامل "unicode/utypes.h"
#شامل "bytesinkutil.h"
#شامل "cstring.h"
#شامل "ustr_imp.h"

using icu::StringPiece;

#define EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#define INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#define CONST_INTERNAL(e) (reinterpret_cast<ثابت icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    بازگشت EXTERNAL(جدید icu::LocaleBuilder());
}

تهی ulocbld_close(ULocaleBuilder* builder) {
    اگر (builder == nullptr) بازگشت;
    delete INTERNAL(builder);
}

تهی ulocbld_setLocale(ULocaleBuilder* builder, ثابت نویسه* locale, int32_t length) {
    اگر (builder == nullptr) بازگشت;
    icu::Locale l;
    اگر (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } وگرنه {
        اگر (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } وگرنه {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            نویسه buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

تهی
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    اگر (builder == nullptr) بازگشت;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<ثابت icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
تهی ulocbld_##N(ULocaleBuilder* bld, ثابت نویسه* s, int32_t l) { \
    اگر (bld == nullptr) بازگشت; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

تهی ulocbld_setExtension(ULocaleBuilder* builder, نویسه کلید, ثابت نویسه* value, int32_t length) {
    اگر (builder == nullptr) بازگشت;
    INTERNAL(builder)->setExtension(کلید, STRING_PIECE(value, length));
}

تهی ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, ثابت نویسه* کلید, int32_t کلید_Length,
    ثابت نویسه* type, int32_t typeLength) {
    اگر (builder == nullptr) بازگشت;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(کلید, کلید_Length), STRING_PIECE(type, typeLength));
}

تهی ulocbld_clear(ULocaleBuilder* builder) {
    اگر (builder == nullptr) بازگشت;
    INTERNAL(builder)->clear();
}

تهی ulocbld_clearExtensions(ULocaleBuilder* builder) {
    اگر (builder == nullptr) بازگشت;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    اگر (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    اگر (U_FAILURE(*err)) بازگشت nullptr;
    icu::Locale* r = l.clone();
    اگر (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    بازگشت reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              نویسه* buffer, int32_t bufferCapacity, UErrorCode* err) {
    اگر (U_FAILURE(*err)) { بازگشت 0; }
    اگر (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    اگر (U_FAILURE(*err)) { بازگشت 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    اگر (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    بازگشت u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  نویسه* buffer, int32_t bufferCapacity, UErrorCode* err) {
    اگر (U_FAILURE(*err)) { بازگشت 0; }
    اگر (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    بازگشت icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(ثابت ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    اگر (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت درست;
    }
    بازگشت CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
