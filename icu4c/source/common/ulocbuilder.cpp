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
    trả_về EXTERNAL(new icu::LocaleBuilder());
}

void ulocbld_close(ULocaleBuilder* builder) {
    nếu (builder == nullptr) trả_về;
    delete INTERNAL(builder);
}

void ulocbld_setLocale(ULocaleBuilder* builder, const char* locale, int32_t length) {
    nếu (builder == nullptr) trả_về;
    icu::Locale l;
    nếu (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } không_thì {
        nếu (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } không_thì {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            char buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

void
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    nếu (builder == nullptr) trả_về;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<const icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
void ulocbld_##N(ULocaleBuilder* bld, const char* s, int32_t l) { \
    nếu (bld == nullptr) trả_về; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

void ulocbld_setExtension(ULocaleBuilder* builder, char key, const char* value, int32_t length) {
    nếu (builder == nullptr) trả_về;
    INTERNAL(builder)->setExtension(key, STRING_PIECE(value, length));
}

void ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, const char* key, int32_t keyLength,
    const char* type, int32_t typeLength) {
    nếu (builder == nullptr) trả_về;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(key, keyLength), STRING_PIECE(type, typeLength));
}

void ulocbld_clear(ULocaleBuilder* builder) {
    nếu (builder == nullptr) trả_về;
    INTERNAL(builder)->clear();
}

void ulocbld_clearExtensions(ULocaleBuilder* builder) {
    nếu (builder == nullptr) trả_về;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    nếu (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    nếu (U_FAILURE(*err)) trả_về nullptr;
    icu::Locale* r = l.clone();
    nếu (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        trả_về nullptr;
    }
    trả_về reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              char* buffer, int32_t bufferCapacity, UErrorCode* err) {
    nếu (U_FAILURE(*err)) { trả_về 0; }
    nếu (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    nếu (U_FAILURE(*err)) { trả_về 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    nếu (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    trả_về u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  char* buffer, int32_t bufferCapacity, UErrorCode* err) {
    nếu (U_FAILURE(*err)) { trả_về 0; }
    nếu (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    trả_về icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(const ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    nếu (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về true;
    }
    trả_về CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
