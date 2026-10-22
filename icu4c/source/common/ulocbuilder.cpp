// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#অন্তর্ভাৱ <utility>

#অন্তর্ভাৱ "unicode/bytestream.h"
#অন্তর্ভাৱ "unicode/localebuilder.h"
#অন্তর্ভাৱ "unicode/locid.h"
#অন্তর্ভাৱ "unicode/stringpiece.h"
#অন্তর্ভাৱ "unicode/ulocbuilder.h"
#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "bytesinkutil.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "ustr_imp.h"

using icu::StringPiece;

#define EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#define INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#define CONST_INTERNAL(e) (reinterpret_cast<ধ্রুৱ icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    ঘূৰাই EXTERNAL(নূতন icu::LocaleBuilder());
}

শূন্য ulocbld_close(ULocaleBuilder* builder) {
    যদি (builder == nullptr) ঘূৰাই;
    delete INTERNAL(builder);
}

শূন্য ulocbld_setLocale(ULocaleBuilder* builder, ধ্রুৱ ৱর্ণ* locale, int32_t length) {
    যদি (builder == nullptr) ঘূৰাই;
    icu::Locale l;
    যদি (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } অন্যথা {
        যদি (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } অন্যথা {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            ৱর্ণ buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

শূন্য
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    যদি (builder == nullptr) ঘূৰাই;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<ধ্রুৱ icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
শূন্য ulocbld_##N(ULocaleBuilder* bld, ধ্রুৱ ৱর্ণ* s, int32_t l) { \
    যদি (bld == nullptr) ঘূৰাই; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

শূন্য ulocbld_setExtension(ULocaleBuilder* builder, ৱর্ণ চাবি, ধ্রুৱ ৱর্ণ* value, int32_t length) {
    যদি (builder == nullptr) ঘূৰাই;
    INTERNAL(builder)->setExtension(চাবি, STRING_PIECE(value, length));
}

শূন্য ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, ধ্রুৱ ৱর্ণ* চাবি, int32_t চাবি_Length,
    ধ্রুৱ ৱর্ণ* type, int32_t typeLength) {
    যদি (builder == nullptr) ঘূৰাই;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(চাবি, চাবি_Length), STRING_PIECE(type, typeLength));
}

শূন্য ulocbld_clear(ULocaleBuilder* builder) {
    যদি (builder == nullptr) ঘূৰাই;
    INTERNAL(builder)->clear();
}

শূন্য ulocbld_clearExtensions(ULocaleBuilder* builder) {
    যদি (builder == nullptr) ঘূৰাই;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    যদি (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    যদি (U_FAILURE(*err)) ঘূৰাই nullptr;
    icu::Locale* r = l.clone();
    যদি (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }
    ঘূৰাই reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              ৱর্ণ* buffer, int32_t bufferCapacity, UErrorCode* err) {
    যদি (U_FAILURE(*err)) { ঘূৰাই 0; }
    যদি (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    যদি (U_FAILURE(*err)) { ঘূৰাই 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    যদি (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    ঘূৰাই u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  ৱর্ণ* buffer, int32_t bufferCapacity, UErrorCode* err) {
    যদি (U_FAILURE(*err)) { ঘূৰাই 0; }
    যদি (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ঘূৰাই icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(ধ্রুৱ ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    যদি (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই সঁচা;
    }
    ঘূৰাই CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
