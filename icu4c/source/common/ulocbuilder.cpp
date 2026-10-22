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
    ꦮꦭꦶ EXTERNAL(new icu::LocaleBuilder());
}

void ulocbld_close(ULocaleBuilder* builder) {
    ꦪꦼꦤ꧀ (builder == nullptr) ꦮꦭꦶ;
    delete INTERNAL(builder);
}

void ulocbld_setLocale(ULocaleBuilder* builder, const var_na* locale, int32_t length) {
    ꦪꦼꦤ꧀ (builder == nullptr) ꦮꦭꦶ;
    icu::Locale l;
    ꦪꦼꦤ꧀ (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } ꦭꦶꦪ {
        ꦪꦼꦤ꧀ (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } ꦭꦶꦪ {
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
    ꦪꦼꦤ꧀ (builder == nullptr) ꦮꦭꦶ;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<const icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
void ulocbld_##N(ULocaleBuilder* bld, const var_na* s, int32_t l) { \
    ꦪꦼꦤ꧀ (bld == nullptr) ꦮꦭꦶ; \
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
    ꦪꦼꦤ꧀ (builder == nullptr) ꦮꦭꦶ;
    INTERNAL(builder)->setExtension(key, STRING_PIECE(value, length));
}

void ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, const var_na* key, int32_t keyLength,
    const var_na* type, int32_t typeLength) {
    ꦪꦼꦤ꧀ (builder == nullptr) ꦮꦭꦶ;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(key, keyLength), STRING_PIECE(type, typeLength));
}

void ulocbld_clear(ULocaleBuilder* builder) {
    ꦪꦼꦤ꧀ (builder == nullptr) ꦮꦭꦶ;
    INTERNAL(builder)->clear();
}

void ulocbld_clearExtensions(ULocaleBuilder* builder) {
    ꦪꦼꦤ꧀ (builder == nullptr) ꦮꦭꦶ;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    ꦪꦼꦤ꧀ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ꦪꦼꦤ꧀ (U_FAILURE(*err)) ꦮꦭꦶ nullptr;
    icu::Locale* r = l.clone();
    ꦪꦼꦤ꧀ (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        ꦮꦭꦶ nullptr;
    }
    ꦮꦭꦶ reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              var_na* buffer, int32_t bufferCapacity, UErrorCode* err) {
    ꦪꦼꦤ꧀ (U_FAILURE(*err)) { ꦮꦭꦶ 0; }
    ꦪꦼꦤ꧀ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ꦪꦼꦤ꧀ (U_FAILURE(*err)) { ꦮꦭꦶ 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    ꦪꦼꦤ꧀ (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    ꦮꦭꦶ u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  var_na* buffer, int32_t bufferCapacity, UErrorCode* err) {
    ꦪꦼꦤ꧀ (U_FAILURE(*err)) { ꦮꦭꦶ 0; }
    ꦪꦼꦤ꧀ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ꦮꦭꦶ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(const ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    ꦪꦼꦤ꧀ (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ true;
    }
    ꦮꦭꦶ CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
