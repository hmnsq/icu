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

استعمال icu::StringPiece;

#وصف_ڪريو EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#وصف_ڪريو اندروني(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#وصف_ڪريو CONST_اندروني(e) (reinterpret_cast<ثابت icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    أرجع EXTERNAL(جديد icu::LocaleBuilder());
}

فراغ ulocbld_close(ULocaleBuilder* builder) {
    إذا (builder == خالي_اشارو) أرجع;
    ميٽيو اندروني(builder);
}

فراغ ulocbld_سيٽ_Locale(ULocaleBuilder* builder, ثابت حرف* locale, int32_t length) {
    إذا (builder == خالي_اشارو) أرجع;
    icu::Locale l;
    إذا (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } وإلا {
        إذا (length >= ULOC_FULLNAME_صلاحيت) {
            l.سيٽ_منزل_Bogus();
        } وإلا {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            حرف buf[ULOC_FULLNAME_صلاحيت];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    اندروني(builder)->سيٽ_Locale(l);
}

فراغ
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    إذا (builder == خالي_اشارو) أرجع;
    اندروني(builder)->سيٽ_Locale(*(reinterpret_cast<ثابت icu::Locale*>(locale)));
    ulocale_close(locale);
}

#وصف_ڪريو STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#وصف_ڪريو IMPL_ULOCBLD_SETTER(N) \
فراغ ulocbld_##N(ULocaleBuilder* bld, ثابت حرف* s, int32_t l) { \
    إذا (bld == خالي_اشارو) أرجع; \
    اندروني(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(سيٽ_Language_ٽيگ)
IMPL_ULOCBLD_SETTER(سيٽ_Language)
IMPL_ULOCBLD_SETTER(سيٽ_Script)
IMPL_ULOCBLD_SETTER(سيٽ_Region)
IMPL_ULOCBLD_SETTER(سيٽ_Variant)
IMPL_ULOCBLD_SETTER(جوڙيو_Unicode_Locale_Attribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

فراغ ulocbld_سيٽ_Extension(ULocaleBuilder* builder, حرف ڪنجي, ثابت حرف* value, int32_t length) {
    إذا (builder == خالي_اشارو) أرجع;
    اندروني(builder)->سيٽ_Extension(ڪنجي, STRING_PIECE(value, length));
}

فراغ ulocbld_سيٽ_Unicode_Locale_Keyword(
    ULocaleBuilder* builder, ثابت حرف* ڪنجي, int32_t ڪنجي_ڊيگهائي,
    ثابت حرف* type, int32_t type_ڊيگهائي) {
    إذا (builder == خالي_اشارو) أرجع;
    اندروني(builder)->سيٽ_Unicode_Locale_Keyword(
        STRING_PIECE(ڪنجي, ڪنجي_ڊيگهائي), STRING_PIECE(type, type_ڊيگهائي));
}

فراغ ulocbld_clear(ULocaleBuilder* builder) {
    إذا (builder == خالي_اشارو) أرجع;
    اندروني(builder)->clear();
}

فراغ ulocbld_clearExtensions(ULocaleBuilder* builder) {
    إذا (builder == خالي_اشارو) أرجع;
    اندروني(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UError_ڪوڊ* err) {
    إذا (builder == خالي_اشارو) {
        *err = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    icu::Locale l = اندروني(builder)->build(*err);
    إذا (U_FAILURE(*err)) أرجع خالي_اشارو;
    icu::Locale* r = l.clone();
    إذا (r == خالي_اشارو) {
        *err = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    أرجع reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              حرف* بفر, int32_t بفر_صلاحيت, UError_ڪوڊ* err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    إذا (builder == خالي_اشارو) {
        *err = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    icu::Locale l = اندروني(builder)->build(*err);
    إذا (U_FAILURE(*err)) { أرجع 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    إذا (0 < length && length <= بفر_صلاحيت) {
        uprv_memcpy(بفر, l.getName(), length);
    }
    أرجع u_terminateChars(بفر, بفر_صلاحيت, length, err);
}

int32_t ulocbld_build_Language_ٽيگ(ULocaleBuilder* builder,
                  حرف* بفر, int32_t بفر_صلاحيت, UError_ڪوڊ* err) {
    إذا (U_FAILURE(*err)) { أرجع 0; }
    إذا (builder == خالي_اشارو) {
        *err = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    icu::Locale l = اندروني(builder)->build(*err);
    أرجع icu::ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
        بفر, بفر_صلاحيت,
        [&](icu::ByteSink& sink, UError_ڪوڊ& حالت) {
            l.منزل_Language_ٽيگ(sink, حالت);
        },
        *err);
}

UBool ulocbld_copy_Error_منزل(ثابت ULocaleBuilder* builder, UError_ڪوڊ *ٻاهريون_Error_ڪوڊ) {
    إذا (builder == خالي_اشارو) {
        *ٻاهريون_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع سچ;
    }
    أرجع CONST_اندروني(builder)->copy_Error_منزل(*ٻاهريون_Error_ڪوڊ);
}
