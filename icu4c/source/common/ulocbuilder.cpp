// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/localebuilder.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/ulocbuilder.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "ustr_imp.h"

using icu::StringPiece;

#define EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#define INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#define CONST_INTERNAL(e) (reinterpret_cast<ध्रुव icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    पटिनिवत्त EXTERNAL(नूतन icu::LocaleBuilder());
}

शून्य ulocbld_close(ULocaleBuilder* builder) {
    सचे (builder == nullptr) पटिनिवत्त;
    delete INTERNAL(builder);
}

शून्य ulocbld_setLocale(ULocaleBuilder* builder, ध्रुव वर्ण* locale, int32_t length) {
    सचे (builder == nullptr) पटिनिवत्त;
    icu::Locale l;
    सचे (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } अञ्ञथा {
        सचे (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } अञ्ञथा {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            वर्ण buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

शून्य
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    सचे (builder == nullptr) पटिनिवत्त;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<ध्रुव icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
शून्य ulocbld_##N(ULocaleBuilder* bld, ध्रुव वर्ण* s, int32_t l) { \
    सचे (bld == nullptr) पटिनिवत्त; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

शून्य ulocbld_setExtension(ULocaleBuilder* builder, वर्ण कुञ्चिका, ध्रुव वर्ण* value, int32_t length) {
    सचे (builder == nullptr) पटिनिवत्त;
    INTERNAL(builder)->setExtension(कुञ्चिका, STRING_PIECE(value, length));
}

शून्य ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, ध्रुव वर्ण* कुञ्चिका, int32_t कुञ्चिका_Length,
    ध्रुव वर्ण* type, int32_t typeLength) {
    सचे (builder == nullptr) पटिनिवत्त;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(कुञ्चिका, कुञ्चिका_Length), STRING_PIECE(type, typeLength));
}

शून्य ulocbld_clear(ULocaleBuilder* builder) {
    सचे (builder == nullptr) पटिनिवत्त;
    INTERNAL(builder)->clear();
}

शून्य ulocbld_clearExtensions(ULocaleBuilder* builder) {
    सचे (builder == nullptr) पटिनिवत्त;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    सचे (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    सचे (U_FAILURE(*err)) पटिनिवत्त nullptr;
    icu::Locale* r = l.clone();
    सचे (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              वर्ण* buffer, int32_t bufferCapacity, UErrorCode* err) {
    सचे (U_FAILURE(*err)) { पटिनिवत्त 0; }
    सचे (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    सचे (U_FAILURE(*err)) { पटिनिवत्त 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    सचे (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    पटिनिवत्त u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  वर्ण* buffer, int32_t bufferCapacity, UErrorCode* err) {
    सचे (U_FAILURE(*err)) { पटिनिवत्त 0; }
    सचे (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    पटिनिवत्त icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(ध्रुव ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    सचे (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त सच्च;
    }
    पटिनिवत्त CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
