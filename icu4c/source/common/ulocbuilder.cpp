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
#define CONST_INTERNAL(e) (reinterpret_cast<स्थिरांक icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    परत EXTERNAL(नवीन icu::LocaleBuilder());
}

शून्य ulocbld_close(ULocaleBuilder* builder) {
    जर (builder == nullptr) परत;
    delete INTERNAL(builder);
}

शून्य ulocbld_setLocale(ULocaleBuilder* builder, स्थिरांक अक्षर* locale, int32_t length) {
    जर (builder == nullptr) परत;
    icu::Locale l;
    जर (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } अन्यथा {
        जर (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } अन्यथा {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            अक्षर buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

शून्य
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    जर (builder == nullptr) परत;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<स्थिरांक icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
शून्य ulocbld_##N(ULocaleBuilder* bld, स्थिरांक अक्षर* s, int32_t l) { \
    जर (bld == nullptr) परत; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

शून्य ulocbld_setExtension(ULocaleBuilder* builder, अक्षर किल्ली, स्थिरांक अक्षर* value, int32_t length) {
    जर (builder == nullptr) परत;
    INTERNAL(builder)->setExtension(किल्ली, STRING_PIECE(value, length));
}

शून्य ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, स्थिरांक अक्षर* किल्ली, int32_t किल्ली_Length,
    स्थिरांक अक्षर* type, int32_t typeLength) {
    जर (builder == nullptr) परत;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(किल्ली, किल्ली_Length), STRING_PIECE(type, typeLength));
}

शून्य ulocbld_clear(ULocaleBuilder* builder) {
    जर (builder == nullptr) परत;
    INTERNAL(builder)->clear();
}

शून्य ulocbld_clearExtensions(ULocaleBuilder* builder) {
    जर (builder == nullptr) परत;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    जर (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    जर (U_FAILURE(*err)) परत nullptr;
    icu::Locale* r = l.clone();
    जर (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    परत reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              अक्षर* buffer, int32_t bufferCapacity, UErrorCode* err) {
    जर (U_FAILURE(*err)) { परत 0; }
    जर (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    जर (U_FAILURE(*err)) { परत 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    जर (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    परत u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  अक्षर* buffer, int32_t bufferCapacity, UErrorCode* err) {
    जर (U_FAILURE(*err)) { परत 0; }
    जर (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    परत icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(स्थिरांक ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    जर (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत सत्य;
    }
    परत CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
