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
    प्रत्यागम EXTERNAL(नूतन icu::LocaleBuilder());
}

शून्य ulocbld_close(ULocaleBuilder* builder) {
    जइ (builder == nullptr) प्रत्यागम;
    delete INTERNAL(builder);
}

शून्य ulocbld_setLocale(ULocaleBuilder* builder, ध्रुव वर्ण* locale, int32_t length) {
    जइ (builder == nullptr) प्रत्यागम;
    icu::Locale l;
    जइ (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } अण्णहा {
        जइ (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } अण्णहा {
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
    जइ (builder == nullptr) प्रत्यागम;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<ध्रुव icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
शून्य ulocbld_##N(ULocaleBuilder* bld, ध्रुव वर्ण* s, int32_t l) { \
    जइ (bld == nullptr) प्रत्यागम; \
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
    जइ (builder == nullptr) प्रत्यागम;
    INTERNAL(builder)->setExtension(कुञ्चिका, STRING_PIECE(value, length));
}

शून्य ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, ध्रुव वर्ण* कुञ्चिका, int32_t कुञ्चिका_Length,
    ध्रुव वर्ण* type, int32_t typeLength) {
    जइ (builder == nullptr) प्रत्यागम;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(कुञ्चिका, कुञ्चिका_Length), STRING_PIECE(type, typeLength));
}

शून्य ulocbld_clear(ULocaleBuilder* builder) {
    जइ (builder == nullptr) प्रत्यागम;
    INTERNAL(builder)->clear();
}

शून्य ulocbld_clearExtensions(ULocaleBuilder* builder) {
    जइ (builder == nullptr) प्रत्यागम;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    जइ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    जइ (U_FAILURE(*err)) प्रत्यागम nullptr;
    icu::Locale* r = l.clone();
    जइ (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    }
    प्रत्यागम reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              वर्ण* buffer, int32_t bufferCapacity, UErrorCode* err) {
    जइ (U_FAILURE(*err)) { प्रत्यागम 0; }
    जइ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    जइ (U_FAILURE(*err)) { प्रत्यागम 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    जइ (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    प्रत्यागम u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  वर्ण* buffer, int32_t bufferCapacity, UErrorCode* err) {
    जइ (U_FAILURE(*err)) { प्रत्यागम 0; }
    जइ (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    प्रत्यागम icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(ध्रुव ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    जइ (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम सच्च;
    }
    प्रत्यागम CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
