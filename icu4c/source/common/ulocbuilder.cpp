// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#অন্তর্ভাব <utility>

#অন্তর্ভাব "unicode/bytestream.h"
#অন্তর্ভাব "unicode/localebuilder.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "unicode/ulocbuilder.h"
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "bytesinkutil.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "ustr_imp.h"

using icu::StringPiece;

#define EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#define INTERNAL(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#define CONST_INTERNAL(e) (reinterpret_cast<ধ্রুব icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    প্রত্যাগম EXTERNAL(নূতন icu::LocaleBuilder());
}

শূন্য ulocbld_close(ULocaleBuilder* builder) {
    যদি (builder == nullptr) প্রত্যাগম;
    delete INTERNAL(builder);
}

শূন্য ulocbld_setLocale(ULocaleBuilder* builder, ধ্রুব বর্ণ* locale, int32_t length) {
    যদি (builder == nullptr) প্রত্যাগম;
    icu::Locale l;
    যদি (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } অন্যথা {
        যদি (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } অন্যথা {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            বর্ণ buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

শূন্য
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    যদি (builder == nullptr) প্রত্যাগম;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<ধ্রুব icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
শূন্য ulocbld_##N(ULocaleBuilder* bld, ধ্রুব বর্ণ* s, int32_t l) { \
    যদি (bld == nullptr) প্রত্যাগম; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

শূন্য ulocbld_setExtension(ULocaleBuilder* builder, বর্ণ চাবি, ধ্রুব বর্ণ* value, int32_t length) {
    যদি (builder == nullptr) প্রত্যাগম;
    INTERNAL(builder)->setExtension(চাবি, STRING_PIECE(value, length));
}

শূন্য ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, ধ্রুব বর্ণ* চাবি, int32_t চাবি_Length,
    ধ্রুব বর্ণ* type, int32_t typeLength) {
    যদি (builder == nullptr) প্রত্যাগম;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(চাবি, চাবি_Length), STRING_PIECE(type, typeLength));
}

শূন্য ulocbld_clear(ULocaleBuilder* builder) {
    যদি (builder == nullptr) প্রত্যাগম;
    INTERNAL(builder)->clear();
}

শূন্য ulocbld_clearExtensions(ULocaleBuilder* builder) {
    যদি (builder == nullptr) প্রত্যাগম;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    যদি (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    যদি (U_FAILURE(*err)) প্রত্যাগম nullptr;
    icu::Locale* r = l.clone();
    যদি (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম nullptr;
    }
    প্রত্যাগম reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              বর্ণ* buffer, int32_t bufferCapacity, UErrorCode* err) {
    যদি (U_FAILURE(*err)) { প্রত্যাগম 0; }
    যদি (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    যদি (U_FAILURE(*err)) { প্রত্যাগম 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    যদি (0 < length && length <= bufferCapacity) {
        uprv_memcpy(buffer, l.getName(), length);
    }
    প্রত্যাগম u_terminateChars(buffer, bufferCapacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  বর্ণ* buffer, int32_t bufferCapacity, UErrorCode* err) {
    যদি (U_FAILURE(*err)) { প্রত্যাগম 0; }
    যদি (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    প্রত্যাগম icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        buffer, bufferCapacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(ধ্রুব ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    যদি (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম সত্য;
    }
    প্রত্যাগম CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
