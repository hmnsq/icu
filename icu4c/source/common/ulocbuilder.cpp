// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن <utility>

#ضمن "unicode/bytestream.h"
#ضمن "unicode/localebuilder.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/ulocbuilder.h"
#ضمن "unicode/utypes.h"
#ضمن "bytesinkutil.h"
#ضمن "cstring.h"
#ضمن "ustr_imp.h"

using icu::StringPiece;

#define EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#define INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#define CONST_INTERNAL(e) (reinterpret_cast<ثابت icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    ܦܢܐ EXTERNAL(جديد icu::LocaleBuilder());
}

فراغ ulocbld_close(ULocaleBuilder* builder) {
    ܐܢ (builder == nullptr) ܦܢܐ;
    delete INTERNAL(builder);
}

فراغ ulocbld_setLocale(ULocaleBuilder* builder, ثابت حرف* locale, int32_t length) {
    ܐܢ (builder == nullptr) ܦܢܐ;
    icu::Locale l;
    ܐܢ (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } ܐܚܪܢܐ {
        ܐܢ (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } ܐܚܪܢܐ {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            حرف buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

فراغ
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    ܐܢ (builder == nullptr) ܦܢܐ;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<ثابت icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
فراغ ulocbld_##N(ULocaleBuilder* bld, ثابت حرف* s, int32_t l) { \
    ܐܢ (bld == nullptr) ܦܢܐ; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

فراغ ulocbld_setExtension(ULocaleBuilder* builder, حرف key, ثابت حرف* value, int32_t length) {
    ܐܢ (builder == nullptr) ܦܢܐ;
    INTERNAL(builder)->setExtension(key, STRING_PIECE(value, length));
}

فراغ ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, ثابت حرف* key, int32_t keyLength,
    ثابت حرف* type, int32_t typeLength) {
    ܐܢ (builder == nullptr) ܦܢܐ;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(key, keyLength), STRING_PIECE(type, typeLength));
}

فراغ ulocbld_clear(ULocaleBuilder* builder) {
    ܐܢ (builder == nullptr) ܦܢܐ;
    INTERNAL(builder)->clear();
}

فراغ ulocbld_clearExtensions(ULocaleBuilder* builder) {
    ܐܢ (builder == nullptr) ܦܢܐ;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    ܐܢ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ܐܢ (U_FAILURE(*err)) ܦܢܐ nullptr;
    icu::Locale* r = l.clone();
    ܐܢ (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ nullptr;
    }
    ܦܢܐ reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              حرف* buffer, int32_t bufferCapacity, UErrorCode* err) {
    ܐܢ (U_FAILURE(*err)) { ܦܢܐ 0; }
    ܐܢ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ܐܢ (U_FAILURE(*err)) { ܦܢܐ 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    ܐܢ (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    ܦܢܐ u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  حرف* buffer, int32_t bufferCapacity, UErrorCode* err) {
    ܐܢ (U_FAILURE(*err)) { ܦܢܐ 0; }
    ܐܢ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ܦܢܐ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(ثابت ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    ܐܢ (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ ܫܪܪܐ;
    }
    ܦܢܐ CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
