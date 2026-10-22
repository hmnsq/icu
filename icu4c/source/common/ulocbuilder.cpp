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
#define CONST_INTERNAL(e) (reinterpret_cast<अचर icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    फर्काऊ EXTERNAL(नूतन icu::LocaleBuilder());
}

शून्य ulocbld_close(ULocaleBuilder* builder) {
    यदि (builder == nullptr) फर्काऊ;
    delete INTERNAL(builder);
}

शून्य ulocbld_setLocale(ULocaleBuilder* builder, अचर वर्ण* locale, int32_t length) {
    यदि (builder == nullptr) फर्काऊ;
    icu::Locale l;
    यदि (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } अन्यथा {
        यदि (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } अन्यथा {
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
    यदि (builder == nullptr) फर्काऊ;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<अचर icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
शून्य ulocbld_##N(ULocaleBuilder* bld, अचर वर्ण* s, int32_t l) { \
    यदि (bld == nullptr) फर्काऊ; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

शून्य ulocbld_setExtension(ULocaleBuilder* builder, वर्ण साँचो, अचर वर्ण* value, int32_t length) {
    यदि (builder == nullptr) फर्काऊ;
    INTERNAL(builder)->setExtension(साँचो, STRING_PIECE(value, length));
}

शून्य ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, अचर वर्ण* साँचो, int32_t साँचो_Length,
    अचर वर्ण* type, int32_t typeLength) {
    यदि (builder == nullptr) फर्काऊ;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(साँचो, साँचो_Length), STRING_PIECE(type, typeLength));
}

शून्य ulocbld_clear(ULocaleBuilder* builder) {
    यदि (builder == nullptr) फर्काऊ;
    INTERNAL(builder)->clear();
}

शून्य ulocbld_clearExtensions(ULocaleBuilder* builder) {
    यदि (builder == nullptr) फर्काऊ;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    यदि (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    यदि (U_FAILURE(*err)) फर्काऊ nullptr;
    icu::Locale* r = l.clone();
    यदि (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    फर्काऊ reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              वर्ण* buffer, int32_t bufferCapacity, UErrorCode* err) {
    यदि (U_FAILURE(*err)) { फर्काऊ 0; }
    यदि (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    यदि (U_FAILURE(*err)) { फर्काऊ 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    यदि (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    फर्काऊ u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  वर्ण* buffer, int32_t bufferCapacity, UErrorCode* err) {
    यदि (U_FAILURE(*err)) { फर्काऊ 0; }
    यदि (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    फर्काऊ icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(अचर ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    यदि (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ सत्य;
    }
    फर्काऊ CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
