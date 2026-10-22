// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#અન્તર્ભાવ <utility>

#અન્તર્ભાવ "unicode/bytestream.h"
#અન્તર્ભાવ "unicode/localebuilder.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/stringpiece.h"
#અન્તર્ભાવ "unicode/ulocbuilder.h"
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "bytesinkutil.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "ustr_imp.h"

using icu::StringPiece;

#define EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#define INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#define CONST_INTERNAL(e) (reinterpret_cast<અચલ icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    પાછા EXTERNAL(નવું icu::LocaleBuilder());
}

શૂન્ય ulocbld_close(ULocaleBuilder* builder) {
    જો (builder == nullptr) પાછા;
    delete INTERNAL(builder);
}

શૂન્ય ulocbld_setLocale(ULocaleBuilder* builder, અચલ અક્ષર* locale, int32_t length) {
    જો (builder == nullptr) પાછા;
    icu::Locale l;
    જો (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } નહીંતર {
        જો (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } નહીંતર {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            અક્ષર buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

શૂન્ય
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    જો (builder == nullptr) પાછા;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<અચલ icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
શૂન્ય ulocbld_##N(ULocaleBuilder* bld, અચલ અક્ષર* s, int32_t l) { \
    જો (bld == nullptr) પાછા; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

શૂન્ય ulocbld_setExtension(ULocaleBuilder* builder, અક્ષર ચાવી, અચલ અક્ષર* value, int32_t length) {
    જો (builder == nullptr) પાછા;
    INTERNAL(builder)->setExtension(ચાવી, STRING_PIECE(value, length));
}

શૂન્ય ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, અચલ અક્ષર* ચાવી, int32_t ચાવી_Length,
    અચલ અક્ષર* type, int32_t typeLength) {
    જો (builder == nullptr) પાછા;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(ચાવી, ચાવી_Length), STRING_PIECE(type, typeLength));
}

શૂન્ય ulocbld_clear(ULocaleBuilder* builder) {
    જો (builder == nullptr) પાછા;
    INTERNAL(builder)->clear();
}

શૂન્ય ulocbld_clearExtensions(ULocaleBuilder* builder) {
    જો (builder == nullptr) પાછા;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    જો (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    જો (U_FAILURE(*err)) પાછા nullptr;
    icu::Locale* r = l.clone();
    જો (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    પાછા reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              અક્ષર* buffer, int32_t bufferCapacity, UErrorCode* err) {
    જો (U_FAILURE(*err)) { પાછા 0; }
    જો (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    જો (U_FAILURE(*err)) { પાછા 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    જો (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    પાછા u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  અક્ષર* buffer, int32_t bufferCapacity, UErrorCode* err) {
    જો (U_FAILURE(*err)) { પાછા 0; }
    જો (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    પાછા icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(અચલ ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    જો (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા સાચું;
    }
    પાછા CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
