// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#అన్తర్భావ <utility>

#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/localebuilder.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/ulocbuilder.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "ustr_imp.h"

ఉపయోగించి icu::StringPiece;

#నిర్వచించు EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#నిర్వచించు అంతర్గత(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#నిర్వచించు CONST_అంతర్గత(e) (reinterpret_cast<స్థిరరాశి icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    తిరిగి EXTERNAL(కొత్త icu::LocaleBuilder());
}

శూన్య ulocbld_close(ULocaleBuilder* builder) {
    ఒకవేళ (builder == శూన్యసూచిక) తిరిగి;
    తొలగించు అంతర్గత(builder);
}

శూన్య ulocbld_సమితి_Locale(ULocaleBuilder* builder, స్థిరరాశి అక్షరం* locale, int32_t length) {
    ఒకవేళ (builder == శూన్యసూచిక) తిరిగి;
    icu::Locale l;
    ఒకవేళ (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } లేకపోతే {
        ఒకవేళ (length >= ULOC_FULLNAME_సామర్థ్యం) {
            l.సమితి_గమ్యం_Bogus();
        } లేకపోతే {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            అక్షరం buf[ULOC_FULLNAME_సామర్థ్యం];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    అంతర్గత(builder)->సమితి_Locale(l);
}

శూన్య
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    ఒకవేళ (builder == శూన్యసూచిక) తిరిగి;
    అంతర్గత(builder)->సమితి_Locale(*(reinterpret_cast<స్థిరరాశి icu::Locale*>(locale)));
    ulocale_close(locale);
}

#నిర్వచించు STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#నిర్వచించు IMPL_ULOCBLD_SETTER(N) \
శూన్య ulocbld_##N(ULocaleBuilder* bld, స్థిరరాశి అక్షరం* s, int32_t l) { \
    ఒకవేళ (bld == శూన్యసూచిక) తిరిగి; \
    అంతర్గత(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(సమితి_Language_ట్యాగ్)
IMPL_ULOCBLD_SETTER(సమితి_Language)
IMPL_ULOCBLD_SETTER(సమితి_Script)
IMPL_ULOCBLD_SETTER(సమితి_Region)
IMPL_ULOCBLD_SETTER(సమితి_Variant)
IMPL_ULOCBLD_SETTER(కూడు_Unicode_Locale_Attribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

శూన్య ulocbld_సమితి_Extension(ULocaleBuilder* builder, అక్షరం కీలకం, స్థిరరాశి అక్షరం* value, int32_t length) {
    ఒకవేళ (builder == శూన్యసూచిక) తిరిగి;
    అంతర్గత(builder)->సమితి_Extension(కీలకం, STRING_PIECE(value, length));
}

శూన్య ulocbld_సమితి_Unicode_Locale_Keyword(
    ULocaleBuilder* builder, స్థిరరాశి అక్షరం* కీలకం, int32_t కీలకం_నిడివి,
    స్థిరరాశి అక్షరం* type, int32_t type_నిడివి) {
    ఒకవేళ (builder == శూన్యసూచిక) తిరిగి;
    అంతర్గత(builder)->సమితి_Unicode_Locale_Keyword(
        STRING_PIECE(కీలకం, కీలకం_నిడివి), STRING_PIECE(type, type_నిడివి));
}

శూన్య ulocbld_clear(ULocaleBuilder* builder) {
    ఒకవేళ (builder == శూన్యసూచిక) తిరిగి;
    అంతర్గత(builder)->clear();
}

శూన్య ulocbld_clearExtensions(ULocaleBuilder* builder) {
    ఒకవేళ (builder == శూన్యసూచిక) తిరిగి;
    అంతర్గత(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UError_సంకేతం* err) {
    ఒకవేళ (builder == శూన్యసూచిక) {
        *err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    icu::Locale l = అంతర్గత(builder)->build(*err);
    ఒకవేళ (U_FAILURE(*err)) తిరిగి శూన్యసూచిక;
    icu::Locale* r = l.clone();
    ఒకవేళ (r == శూన్యసూచిక) {
        *err = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              అక్షరం* బఫరు, int32_t బఫరు_సామర్థ్యం, UError_సంకేతం* err) {
    ఒకవేళ (U_FAILURE(*err)) { తిరిగి 0; }
    ఒకవేళ (builder == శూన్యసూచిక) {
        *err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    icu::Locale l = అంతర్గత(builder)->build(*err);
    ఒకవేళ (U_FAILURE(*err)) { తిరిగి 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    ఒకవేళ (0 < length && length <= బఫరు_సామర్థ్యం) {
        uprv_memcpy(బఫరు, l.getName(), length);
    }
    తిరిగి u_terminateChars(బఫరు, బఫరు_సామర్థ్యం, length, err);
}

int32_t ulocbld_build_Language_ట్యాగ్(ULocaleBuilder* builder,
                  అక్షరం* బఫరు, int32_t బఫరు_సామర్థ్యం, UError_సంకేతం* err) {
    ఒకవేళ (U_FAILURE(*err)) { తిరిగి 0; }
    ఒకవేళ (builder == శూన్యసూచిక) {
        *err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    icu::Locale l = అంతర్గత(builder)->build(*err);
    తిరిగి icu::ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        బఫరు, బఫరు_సామర్థ్యం,
        [&](icu::ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            l.గమ్యం_Language_ట్యాగ్(sink, స్థితి_2);
        },
        *err);
}

UBool ulocbld_copy_Error_గమ్యం(స్థిరరాశి ULocaleBuilder* builder, UError_సంకేతం *బహిర్గతం_Error_సంకేతం) {
    ఒకవేళ (builder == శూన్యసూచిక) {
        *బహిర్గతం_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి నిజం;
    }
    తిరిగి CONST_అంతర్గత(builder)->copy_Error_గమ్యం(*బహిర్గతం_Error_సంకేతం);
}
