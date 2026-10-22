// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/localebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/ulocbuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

using icu::StringPiece;

#define EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#define INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#define CONST_INTERNAL(e) (reinterpret_cast<ಧ್ರುವ icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    ಪ್ರತ್ಯಾಗಮ EXTERNAL(ನೂತನ icu::LocaleBuilder());
}

ಶೂನ್ಯ ulocbld_close(ULocaleBuilder* builder) {
    ಯದಿ (builder == nullptr) ಪ್ರತ್ಯಾಗಮ;
    delete INTERNAL(builder);
}

ಶೂನ್ಯ ulocbld_setLocale(ULocaleBuilder* builder, ಧ್ರುವ ವರ್ಣ* locale, int32_t length) {
    ಯದಿ (builder == nullptr) ಪ್ರತ್ಯಾಗಮ;
    icu::Locale l;
    ಯದಿ (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } ಅನ್ಯಥಾ {
        ಯದಿ (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } ಅನ್ಯಥಾ {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            ವರ್ಣ buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

ಶೂನ್ಯ
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    ಯದಿ (builder == nullptr) ಪ್ರತ್ಯಾಗಮ;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<ಧ್ರುವ icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
ಶೂನ್ಯ ulocbld_##N(ULocaleBuilder* bld, ಧ್ರುವ ವರ್ಣ* s, int32_t l) { \
    ಯದಿ (bld == nullptr) ಪ್ರತ್ಯಾಗಮ; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

ಶೂನ್ಯ ulocbld_setExtension(ULocaleBuilder* builder, ವರ್ಣ ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ* value, int32_t length) {
    ಯದಿ (builder == nullptr) ಪ್ರತ್ಯಾಗಮ;
    INTERNAL(builder)->setExtension(ಕೀಲಿ, STRING_PIECE(value, length));
}

ಶೂನ್ಯ ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ, int32_t ಕೀಲಿ_Length,
    ಧ್ರುವ ವರ್ಣ* type, int32_t typeLength) {
    ಯದಿ (builder == nullptr) ಪ್ರತ್ಯಾಗಮ;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(ಕೀಲಿ, ಕೀಲಿ_Length), STRING_PIECE(type, typeLength));
}

ಶೂನ್ಯ ulocbld_clear(ULocaleBuilder* builder) {
    ಯದಿ (builder == nullptr) ಪ್ರತ್ಯಾಗಮ;
    INTERNAL(builder)->clear();
}

ಶೂನ್ಯ ulocbld_clearExtensions(ULocaleBuilder* builder) {
    ಯದಿ (builder == nullptr) ಪ್ರತ್ಯಾಗಮ;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    ಯದಿ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ಯದಿ (U_FAILURE(*err)) ಪ್ರತ್ಯಾಗಮ nullptr;
    icu::Locale* r = l.clone();
    ಯದಿ (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              ವರ್ಣ* buffer, int32_t bufferCapacity, UErrorCode* err) {
    ಯದಿ (U_FAILURE(*err)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ಯದಿ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ಯದಿ (U_FAILURE(*err)) { ಪ್ರತ್ಯಾಗಮ 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    ಯದಿ (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    ಪ್ರತ್ಯಾಗಮ u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  ವರ್ಣ* buffer, int32_t bufferCapacity, UErrorCode* err) {
    ಯದಿ (U_FAILURE(*err)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ಯದಿ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ಪ್ರತ್ಯಾಗಮ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(ಧ್ರುವ ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    ಯದಿ (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
