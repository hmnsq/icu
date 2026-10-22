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

استفاده icu::StringPiece;

#تعریف_کن EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#تعریف_کن داخلی(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#تعریف_کن CONST_داخلی(e) (reinterpret_cast<ثابت icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    أرجع EXTERNAL(جديد icu::LocaleBuilder());
}

فراغ ulocbld_close(ULocaleBuilder* builder) {
    إذا (builder == اشاره_تهی) أرجع;
    حذف داخلی(builder);
}

فراغ ulocbld_مجموعه_Locale(ULocaleBuilder* builder, ثابت حرف* locale, int32_t length) {
    إذا (builder == اشاره_تهی) أرجع;
    icu::Locale l;
    إذا (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } وإلا {
        إذا (length >= ULOC_FULLNAME_ظرفیت) {
            l.مجموعه_مقصد_Bogus();
        } وإلا {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            حرف buf[ULOC_FULLNAME_ظرفیت];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    داخلی(builder)->مجموعه_Locale(l);
}

فراغ
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    إذا (builder == اشاره_تهی) أرجع;
    داخلی(builder)->مجموعه_Locale(*(reinterpret_cast<ثابت icu::Locale*>(locale)));
    ulocale_close(locale);
}

#تعریف_کن STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#تعریف_کن IMPL_ULOCBLD_SETTER(N) \
فراغ ulocbld_##N(ULocaleBuilder* bld, ثابت حرف* s, int32_t l) { \
    إذا (bld == اشاره_تهی) أرجع; \
    داخلی(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(مجموعه_Language_برچسب)
IMPL_ULOCBLD_SETTER(مجموعه_Language)
IMPL_ULOCBLD_SETTER(مجموعه_Script)
IMPL_ULOCBLD_SETTER(مجموعه_Region)
IMPL_ULOCBLD_SETTER(مجموعه_Variant)
IMPL_ULOCBLD_SETTER(جمع_کن_Unicode_Locale_Attribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

فراغ ulocbld_مجموعه_Extension(ULocaleBuilder* builder, حرف کلید, ثابت حرف* value, int32_t length) {
    إذا (builder == اشاره_تهی) أرجع;
    داخلی(builder)->مجموعه_Extension(کلید, STRING_PIECE(value, length));
}

فراغ ulocbld_مجموعه_Unicode_Locale_Keyword(
    ULocaleBuilder* builder, ثابت حرف* کلید, int32_t کلید_درازی,
    ثابت حرف* type, int32_t type_درازی) {
    إذا (builder == اشاره_تهی) أرجع;
    داخلی(builder)->مجموعه_Unicode_Locale_Keyword(
        STRING_PIECE(کلید, کلید_درازی), STRING_PIECE(type, type_درازی));
}

فراغ ulocbld_clear(ULocaleBuilder* builder) {
    إذا (builder == اشاره_تهی) أرجع;
    داخلی(builder)->clear();
}

فراغ ulocbld_clearExtensions(ULocaleBuilder* builder) {
    إذا (builder == اشاره_تهی) أرجع;
    داخلی(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UError_کد* err) {
    إذا (builder == اشاره_تهی) {
        *err = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    icu::Locale l = داخلی(builder)->build(*err);
    إذا (U_FAILURE(*err)) أرجع اشاره_تهی;
    icu::Locale* r = l.clone();
    إذا (r == اشاره_تهی) {
        *err = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    أرجع reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              حرف* میانگیر, int32_t میانگیر_ظرفیت, UError_کد* err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    إذا (builder == اشاره_تهی) {
        *err = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    icu::Locale l = داخلی(builder)->build(*err);
    إذا (U_FAILURE(*err)) { أرجع 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    إذا (0 < length && length <= میانگیر_ظرفیت) {
        uprv_memcpy(میانگیر, l.getName(), length);
    }
    أرجع u_terminateChars(میانگیر, میانگیر_ظرفیت, length, err);
}

int32_t ulocbld_build_Language_برچسب(ULocaleBuilder* builder,
                  حرف* میانگیر, int32_t میانگیر_ظرفیت, UError_کد* err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    إذا (builder == اشاره_تهی) {
        *err = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    icu::Locale l = داخلی(builder)->build(*err);
    أرجع icu::ByteSinkUtil::از_طریق_Byte_Sink_مقصد_Terminated_Chars(
        میانگیر, میانگیر_ظرفیت,
        [&](icu::ByteSink& sink, UError_کد& وضعیت) {
            l.مقصد_Language_برچسب(sink, وضعیت);
        },
        *err);
}

UBool ulocbld_copy_Error_مقصد(ثابت ULocaleBuilder* builder, UError_کد *بیرون_Error_کد) {
    إذا (builder == اشاره_تهی) {
        *بیرون_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع درست;
    }
    أرجع CONST_داخلی(builder)->copy_Error_مقصد(*بیرون_Error_کد);
}
