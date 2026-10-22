// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include <utility>

#include "unicode/bytestream.h"
#include "unicode/localebuilder.h"
#include "unicode/locid.h"
#include "unicode/stringpiece.h"
#include "unicode/ulocbuilder.h"
#include "unicode/utypes.h"
#include "bytesinkutil.h"
#include "cstring.h"
#include "ustr_imp.h"

using icu::StringPiece;

#define EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#define INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#define CONST_INTERNAL(e) (reinterpret_cast<const icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    คืน EXTERNAL(new icu::LocaleBuilder());
}

void ulocbld_close(ULocaleBuilder* builder) {
    ถ้า (builder == nullptr) คืน;
    delete INTERNAL(builder);
}

void ulocbld_setLocale(ULocaleBuilder* builder, const var_na* locale, int32_t length) {
    ถ้า (builder == nullptr) คืน;
    icu::Locale l;
    ถ้า (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } มิฉะนั้น {
        ถ้า (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } มิฉะนั้น {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            var_na buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

void
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    ถ้า (builder == nullptr) คืน;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<const icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
void ulocbld_##N(ULocaleBuilder* bld, const var_na* s, int32_t l) { \
    ถ้า (bld == nullptr) คืน; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

void ulocbld_setExtension(ULocaleBuilder* builder, var_na key, const var_na* value, int32_t length) {
    ถ้า (builder == nullptr) คืน;
    INTERNAL(builder)->setExtension(key, STRING_PIECE(value, length));
}

void ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, const var_na* key, int32_t keyLength,
    const var_na* type, int32_t typeLength) {
    ถ้า (builder == nullptr) คืน;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(key, keyLength), STRING_PIECE(type, typeLength));
}

void ulocbld_clear(ULocaleBuilder* builder) {
    ถ้า (builder == nullptr) คืน;
    INTERNAL(builder)->clear();
}

void ulocbld_clearExtensions(ULocaleBuilder* builder) {
    ถ้า (builder == nullptr) คืน;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    ถ้า (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        คืน nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ถ้า (U_FAILURE(*err)) คืน nullptr;
    icu::Locale* r = l.clone();
    ถ้า (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        คืน nullptr;
    }
    คืน reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              var_na* buffer, int32_t bufferCapacity, UErrorCode* err) {
    ถ้า (U_FAILURE(*err)) { คืน 0; }
    ถ้า (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        คืน 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ถ้า (U_FAILURE(*err)) { คืน 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    ถ้า (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    คืน u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  var_na* buffer, int32_t bufferCapacity, UErrorCode* err) {
    ถ้า (U_FAILURE(*err)) { คืน 0; }
    ถ้า (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        คืน 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    คืน icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(const ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    ถ้า (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        คืน true;
    }
    คืน CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
