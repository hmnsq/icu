// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ଅନ୍ତର୍ଭାଵ <utility>

#ଅନ୍ତର୍ଭାଵ "unicode/bytestream.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localebuilder.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ulocbuilder.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

using icu::StringPiece;

#define EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#define INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#define CONST_INTERNAL(e) (reinterpret_cast<ଅଚଳ icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    ଫେରନ୍ତୁ EXTERNAL(ନୂତନ icu::LocaleBuilder());
}

ଶୂନ୍ୟ ulocbld_close(ULocaleBuilder* builder) {
    ଯଦି (builder == nullptr) ଫେରନ୍ତୁ;
    delete INTERNAL(builder);
}

ଶୂନ୍ୟ ulocbld_setLocale(ULocaleBuilder* builder, ଅଚଳ ଅକ୍ଷର* locale, int32_t length) {
    ଯଦି (builder == nullptr) ଫେରନ୍ତୁ;
    icu::Locale l;
    ଯଦି (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } ଅନ୍ୟଥା {
        ଯଦି (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } ଅନ୍ୟଥା {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            ଅକ୍ଷର buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

ଶୂନ୍ୟ
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    ଯଦି (builder == nullptr) ଫେରନ୍ତୁ;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<ଅଚଳ icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
ଶୂନ୍ୟ ulocbld_##N(ULocaleBuilder* bld, ଅଚଳ ଅକ୍ଷର* s, int32_t l) { \
    ଯଦି (bld == nullptr) ଫେରନ୍ତୁ; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

ଶୂନ୍ୟ ulocbld_setExtension(ULocaleBuilder* builder, ଅକ୍ଷର ଚାବି, ଅଚଳ ଅକ୍ଷର* value, int32_t length) {
    ଯଦି (builder == nullptr) ଫେରନ୍ତୁ;
    INTERNAL(builder)->setExtension(ଚାବି, STRING_PIECE(value, length));
}

ଶୂନ୍ୟ ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, ଅଚଳ ଅକ୍ଷର* ଚାବି, int32_t ଚାବି_Length,
    ଅଚଳ ଅକ୍ଷର* type, int32_t typeLength) {
    ଯଦି (builder == nullptr) ଫେରନ୍ତୁ;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(ଚାବି, ଚାବି_Length), STRING_PIECE(type, typeLength));
}

ଶୂନ୍ୟ ulocbld_clear(ULocaleBuilder* builder) {
    ଯଦି (builder == nullptr) ଫେରନ୍ତୁ;
    INTERNAL(builder)->clear();
}

ଶୂନ୍ୟ ulocbld_clearExtensions(ULocaleBuilder* builder) {
    ଯଦି (builder == nullptr) ଫେରନ୍ତୁ;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    ଯଦି (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ଯଦି (U_FAILURE(*err)) ଫେରନ୍ତୁ nullptr;
    icu::Locale* r = l.clone();
    ଯଦି (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              ଅକ୍ଷର* buffer, int32_t bufferCapacity, UErrorCode* err) {
    ଯଦି (U_FAILURE(*err)) { ଫେରନ୍ତୁ 0; }
    ଯଦି (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ଯଦି (U_FAILURE(*err)) { ଫେରନ୍ତୁ 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    ଯଦି (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    ଫେରନ୍ତୁ u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  ଅକ୍ଷର* buffer, int32_t bufferCapacity, UErrorCode* err) {
    ଯଦି (U_FAILURE(*err)) { ଫେରନ୍ତୁ 0; }
    ଯଦି (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ଫେରନ୍ତୁ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(ଅଚଳ ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    ଯଦି (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଫେରନ୍ତୁ CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
