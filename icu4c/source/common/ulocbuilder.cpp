// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#അന്തര്ഭാവ <utility>

#അന്തര്ഭാവ "unicode/bytestream.h"
#അന്തര്ഭാവ "unicode/localebuilder.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "unicode/ulocbuilder.h"
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "bytesinkutil.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "ustr_imp.h"

using icu::StringPiece;

#define EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#define INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#define CONST_INTERNAL(e) (reinterpret_cast<സ്ഥിരം icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    തിരികെ EXTERNAL(പുതിയ icu::LocaleBuilder());
}

ശൂന്യം ulocbld_close(ULocaleBuilder* builder) {
    എങ്കിൽ (builder == nullptr) തിരികെ;
    delete INTERNAL(builder);
}

ശൂന്യം ulocbld_setLocale(ULocaleBuilder* builder, സ്ഥിരം അക്ഷരം* locale, int32_t length) {
    എങ്കിൽ (builder == nullptr) തിരികെ;
    icu::Locale l;
    എങ്കിൽ (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } അല്ലെങ്കിൽ {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            അക്ഷരം buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

ശൂന്യം
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    എങ്കിൽ (builder == nullptr) തിരികെ;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<സ്ഥിരം icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
ശൂന്യം ulocbld_##N(ULocaleBuilder* bld, സ്ഥിരം അക്ഷരം* s, int32_t l) { \
    എങ്കിൽ (bld == nullptr) തിരികെ; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

ശൂന്യം ulocbld_setExtension(ULocaleBuilder* builder, അക്ഷരം താക്കോൽ, സ്ഥിരം അക്ഷരം* value, int32_t length) {
    എങ്കിൽ (builder == nullptr) തിരികെ;
    INTERNAL(builder)->setExtension(താക്കോൽ, STRING_PIECE(value, length));
}

ശൂന്യം ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, സ്ഥിരം അക്ഷരം* താക്കോൽ, int32_t താക്കോൽ_Length,
    സ്ഥിരം അക്ഷരം* type, int32_t typeLength) {
    എങ്കിൽ (builder == nullptr) തിരികെ;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(താക്കോൽ, താക്കോൽ_Length), STRING_PIECE(type, typeLength));
}

ശൂന്യം ulocbld_clear(ULocaleBuilder* builder) {
    എങ്കിൽ (builder == nullptr) തിരികെ;
    INTERNAL(builder)->clear();
}

ശൂന്യം ulocbld_clearExtensions(ULocaleBuilder* builder) {
    എങ്കിൽ (builder == nullptr) തിരികെ;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    എങ്കിൽ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    എങ്കിൽ (U_FAILURE(*err)) തിരികെ nullptr;
    icu::Locale* r = l.clone();
    എങ്കിൽ (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    തിരികെ reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              അക്ഷരം* buffer, int32_t bufferCapacity, UErrorCode* err) {
    എങ്കിൽ (U_FAILURE(*err)) { തിരികെ 0; }
    എങ്കിൽ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    എങ്കിൽ (U_FAILURE(*err)) { തിരികെ 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    എങ്കിൽ (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    തിരികെ u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  അക്ഷരം* buffer, int32_t bufferCapacity, UErrorCode* err) {
    എങ്കിൽ (U_FAILURE(*err)) { തിരികെ 0; }
    എങ്കിൽ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    തിരികെ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(സ്ഥിരം ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    എങ്കിൽ (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ സത്യം;
    }
    തിരികെ CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
