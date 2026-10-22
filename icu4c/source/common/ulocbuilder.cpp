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
#define CONST_INTERNAL(e) (reinterpret_cast<ধ্রুবক icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    ফেরত EXTERNAL(নতুন icu::LocaleBuilder());
}

শূন্য ulocbld_close(ULocaleBuilder* builder) {
    যদি (builder == nullptr) ফেরত;
    delete INTERNAL(builder);
}

শূন্য ulocbld_setLocale(ULocaleBuilder* builder, ধ্রুবক অক্ষর* locale, int32_t length) {
    যদি (builder == nullptr) ফেরত;
    icu::Locale l;
    যদি (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } অন্যথা {
        যদি (length >= ULOC_FULLNAME_CAPACITY) {
            l.setToBogus();
        } অন্যথা {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            অক্ষর buf[ULOC_FULLNAME_CAPACITY];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    INTERNAL(builder)->setLocale(l);
}

শূন্য
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    যদি (builder == nullptr) ফেরত;
    INTERNAL(builder)->setLocale(*(reinterpret_cast<ধ্রুবক icu::Locale*>(locale)));
    ulocale_close(locale);
}

#define STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#define IMPL_ULOCBLD_SETTER(N) \
শূন্য ulocbld_##N(ULocaleBuilder* bld, ধ্রুবক অক্ষর* s, int32_t l) { \
    যদি (bld == nullptr) ফেরত; \
    INTERNAL(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(setLanguageTag)
IMPL_ULOCBLD_SETTER(setLanguage)
IMPL_ULOCBLD_SETTER(setScript)
IMPL_ULOCBLD_SETTER(setRegion)
IMPL_ULOCBLD_SETTER(setVariant)
IMPL_ULOCBLD_SETTER(addUnicodeLocaleAttribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

শূন্য ulocbld_setExtension(ULocaleBuilder* builder, অক্ষর চাবি, ধ্রুবক অক্ষর* value, int32_t length) {
    যদি (builder == nullptr) ফেরত;
    INTERNAL(builder)->setExtension(চাবি, STRING_PIECE(value, length));
}

শূন্য ulocbld_setUnicodeLocaleKeyword(
    ULocaleBuilder* builder, ধ্রুবক অক্ষর* চাবি, int32_t চাবি_দৈর্ঘ্য,
    ধ্রুবক অক্ষর* type, int32_t type_দৈর্ঘ্য) {
    যদি (builder == nullptr) ফেরত;
    INTERNAL(builder)->setUnicodeLocaleKeyword(
        STRING_PIECE(চাবি, চাবি_দৈর্ঘ্য), STRING_PIECE(type, type_দৈর্ঘ্য));
}

শূন্য ulocbld_clear(ULocaleBuilder* builder) {
    যদি (builder == nullptr) ফেরত;
    INTERNAL(builder)->clear();
}

শূন্য ulocbld_clearExtensions(ULocaleBuilder* builder) {
    যদি (builder == nullptr) ফেরত;
    INTERNAL(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UErrorCode* err) {
    যদি (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    যদি (U_FAILURE(*err)) ফেরত nullptr;
    icu::Locale* r = l.clone();
    যদি (r == nullptr) {
        *err = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    ফেরত reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              অক্ষর* বাফার, int32_t বাফার_Capacity, UErrorCode* err) {
    যদি (U_FAILURE(*err)) { ফেরত 0; }
    যদি (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    যদি (U_FAILURE(*err)) { ফেরত 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    যদি (0 < length && length <= বাফার_Capacity) {
        uprv_memcpy(বাফার, l.getName(), length);
    }
    ফেরত u_terminateChars(বাফার, বাফার_Capacity, length, err);
}

int32_t ulocbld_buildLanguageTag(ULocaleBuilder* builder,
                  অক্ষর* বাফার, int32_t বাফার_Capacity, UErrorCode* err) {
    যদি (U_FAILURE(*err)) { ফেরত 0; }
    যদি (builder == nullptr) {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    icu::Locale l = INTERNAL(builder)->build(*err);
    ফেরত icu::ByteSinkUtil::viaByteSinkToTerminatedChars(
        বাফার, বাফার_Capacity,
        [&](icu::ByteSink& sink, UErrorCode& status) {
            l.toLanguageTag(sink, status);
        },
        *err);
}

UBool ulocbld_copyErrorTo(ধ্রুবক ULocaleBuilder* builder, UErrorCode *outErrorCode) {
    যদি (builder == nullptr) {
        *outErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত সত্য;
    }
    ফেরত CONST_INTERNAL(builder)->copyErrorTo(*outErrorCode);
}
