// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#समावेश <utility>

#समावेश "unicode/bytestream.h"
#समावेश "unicode/localebuilder.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/stringpiece.h"
#समावेश "unicode/ulocbuilder.h"
#समावेश "unicode/utypes.h"
#समावेश "bytesinkutil.h"
#समावेश "cstring.h"
#समावेश "ustr_imp.h"

using icu::StringPiece;

#घोषणा EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#घोषणा INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#घोषणा CONST_INTERNAL(e) (reinterpret_cast<स्थिर icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    वापस EXTERNAL(नया icu::LocaleBuilder());
}

व्योम ulocbld_close(ULocaleBuilder* builder) {
    अगर (builder == nullptr) वापस;
    मिटाओ INTERNAL(builder);
}

व्योम ulocbld_setLocale(ULocaleBuilder* builder, स्थिर अक्षर* locale, int32_t length) {
    अगर (builder == nullptr) वापस;
    icu::Locale l;
    अगर (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } अन्यथा {
        अगर (length >= ULOC_FULLNAME_CAPACITY) {
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

व्योम
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    अगर (builder == nullptr) वापस;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<स्थिर icu::Locale*>(locale)));
    ulocale_close(locale);
}

#घोषणा STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#घोषणा IMPL_ULOCBLD_SETTER(N) \
व्योम ulocbld_##N(ULocaleBuilder* bld, स्थिर अक्षर* s, int32_t l) { \
    अगर (bld == nullptr) वापस; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

व्योम ulocbld_setExtension(ULocaleBuilder* builder, अक्षर कुंजी, स्थिर अक्षर* value, int32_t length) {
    अगर (builder == nullptr) वापस;
    INTERNAL(builder)->setExtension(कुंजी, STRING_PIECE(value, length));
}

व्योम ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, स्थिर अक्षर* कुंजी, int32_t कुंजी_लंबाई,
    स्थिर अक्षर* type, int32_t type_लंबाई) {
    अगर (builder == nullptr) वापस;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(कुंजी, कुंजी_लंबाई), STRING_PIECE(type, type_लंबाई));
}

व्योम ulocbld_clear(ULocaleBuilder* builder) {
    अगर (builder == nullptr) वापस;
    INTERNAL(builder)->सफा();
}

व्योम ulocbld_clearExtensions(ULocaleBuilder* builder) {
    अगर (builder == nullptr) वापस;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    अगर (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    अगर (U_FAILURE(*err)) वापस nullptr;
    icu::Locale* r = l.clone();
    अगर (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    वापस reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              अक्षर* बफर, int32_t बफर_Capacity, UErrorCode* err) {
    अगर (U_FAILURE(*err)) { वापस 0; }
    अगर (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    अगर (U_FAILURE(*err)) { वापस 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    अगर (0 < length && length <= बफर_Capacity) {
        uprv_memcpy(बफर, l.getName(), length);
    }
    वापस u_terminateChars(बफर, बफर_Capacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  अक्षर* बफर, int32_t बफर_Capacity, UErrorCode* err) {
    अगर (U_FAILURE(*err)) { वापस 0; }
    अगर (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    वापस icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        बफर, बफर_Capacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(स्थिर ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    अगर (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस true;
    }
    वापस CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
