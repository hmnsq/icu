// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#அந்தர்பாவ <utility>

#அந்தர்பாவ "unicode/bytestream.h"
#அந்தர்பாவ "unicode/localebuilder.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "unicode/ulocbuilder.h"
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "bytesinkutil.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "ustr_imp.h"

using icu::StringPiece;

#define EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#define INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#define CONST_INTERNAL(e) (reinterpret_cast<மாறிலி icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    திரும்பு EXTERNAL(புதிய icu::LocaleBuilder());
}

வெறுமை ulocbld_close(ULocaleBuilder* builder) {
    எனில் (builder == nullptr) திரும்பு;
    delete INTERNAL(builder);
}

வெறுமை ulocbld_setLocale(ULocaleBuilder* builder, மாறிலி எழுத்து* locale, int32_t length) {
    எனில் (builder == nullptr) திரும்பு;
    icu::Locale l;
    எனில் (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } இல்லையெனில் {
        எனில் (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } இல்லையெனில் {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            எழுத்து buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

வெறுமை
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    எனில் (builder == nullptr) திரும்பு;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<மாறிலி icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
வெறுமை ulocbld_##N(ULocaleBuilder* bld, மாறிலி எழுத்து* s, int32_t l) { \
    எனில் (bld == nullptr) திரும்பு; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

வெறுமை ulocbld_setExtension(ULocaleBuilder* builder, எழுத்து சாவி, மாறிலி எழுத்து* value, int32_t length) {
    எனில் (builder == nullptr) திரும்பு;
    INTERNAL(builder)->setExtension(சாவி, STRING_PIECE(value, length));
}

வெறுமை ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, மாறிலி எழுத்து* சாவி, int32_t சாவி_Length,
    மாறிலி எழுத்து* type, int32_t typeLength) {
    எனில் (builder == nullptr) திரும்பு;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(சாவி, சாவி_Length), STRING_PIECE(type, typeLength));
}

வெறுமை ulocbld_clear(ULocaleBuilder* builder) {
    எனில் (builder == nullptr) திரும்பு;
    INTERNAL(builder)->clear();
}

வெறுமை ulocbld_clearExtensions(ULocaleBuilder* builder) {
    எனில் (builder == nullptr) திரும்பு;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    எனில் (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    எனில் (U_FAILURE(*err)) திரும்பு nullptr;
    icu::Locale* r = l.clone();
    எனில் (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    திரும்பு reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              எழுத்து* buffer, int32_t bufferCapacity, UErrorCode* err) {
    எனில் (U_FAILURE(*err)) { திரும்பு 0; }
    எனில் (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    எனில் (U_FAILURE(*err)) { திரும்பு 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    எனில் (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    திரும்பு u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  எழுத்து* buffer, int32_t bufferCapacity, UErrorCode* err) {
    எனில் (U_FAILURE(*err)) { திரும்பு 0; }
    எனில் (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    திரும்பு icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(மாறிலி ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    எனில் (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு உண்மை;
    }
    திரும்பு CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
