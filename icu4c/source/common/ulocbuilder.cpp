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

उपयुज्य icu::StringPiece;

#परिभाषय EXTERNAL(i) (reinterpret_cast<ULocaleBuilder*>(i))
#परिभाषय आन्तरिकम्(e) (reinterpret_cast<icu::LocaleBuilder*>(e))
#परिभाषय CONST_आन्तरिकम्(e) (reinterpret_cast<ध्रुव icu::LocaleBuilder*>(e))

ULocaleBuilder* ulocbld_open() {
    प्रत्यागम EXTERNAL(नूतन icu::LocaleBuilder());
}

शून्य ulocbld_close(ULocaleBuilder* builder) {
    यदि (builder == शून्यसूचकः) प्रत्यागम;
    विलोपय आन्तरिकम्(builder);
}

शून्य ulocbld_समुच्चयः_Locale(ULocaleBuilder* builder, ध्रुव वर्ण* locale, int32_t length) {
    यदि (builder == शून्यसूचकः) प्रत्यागम;
    icu::Locale l;
    यदि (length < 0 || locale[length] == '\0') {
        l = icu::Locale(locale);
    } अन्यथा {
        यदि (length >= ULOC_FULLNAME_क्षमता) {
            l.समुच्चयः_गन्तव्यम्_Bogus();
        } अन्यथा {
            // locale is not null termined but Locale API require one.
            // Create a null termined version in buf.
            वर्ण buf[ULOC_FULLNAME_क्षमता];
            uprv_memcpy(buf, locale, length);
            buf[length] = '\0';
            l = icu::Locale(buf);
        }
    }
    आन्तरिकम्(builder)->समुच्चयः_Locale(l);
}

शून्य
ulocbld_adoptULocale(ULocaleBuilder* builder, ULocale* locale) {
    यदि (builder == शून्यसूचकः) प्रत्यागम;
    आन्तरिकम्(builder)->समुच्चयः_Locale(*(reinterpret_cast<ध्रुव icu::Locale*>(locale)));
    ulocale_close(locale);
}

#परिभाषय STRING_PIECE(s, l) ((l)<0 ? StringPiece(s) : StringPiece((s), (l)))

#परिभाषय IMPL_ULOCBLD_SETTER(N) \
शून्य ulocbld_##N(ULocaleBuilder* bld, ध्रुव वर्ण* s, int32_t l) { \
    यदि (bld == शून्यसूचकः) प्रत्यागम; \
    आन्तरिकम्(bld)->N(STRING_PIECE(s,l)); \
}

IMPL_ULOCBLD_SETTER(समुच्चयः_Language_चिह्नकम्)
IMPL_ULOCBLD_SETTER(समुच्चयः_Language)
IMPL_ULOCBLD_SETTER(समुच्चयः_Script)
IMPL_ULOCBLD_SETTER(समुच्चयः_Region)
IMPL_ULOCBLD_SETTER(समुच्चयः_Variant)
IMPL_ULOCBLD_SETTER(सङ्कलय_Unicode_Locale_Attribute)
IMPL_ULOCBLD_SETTER(removeUnicodeLocaleAttribute)

शून्य ulocbld_समुच्चयः_Extension(ULocaleBuilder* builder, वर्ण कुञ्चिका, ध्रुव वर्ण* value, int32_t length) {
    यदि (builder == शून्यसूचकः) प्रत्यागम;
    आन्तरिकम्(builder)->समुच्चयः_Extension(कुञ्चिका, STRING_PIECE(value, length));
}

शून्य ulocbld_समुच्चयः_Unicode_Locale_Keyword(
    ULocaleBuilder* builder, ध्रुव वर्ण* कुञ्चिका, int32_t कुञ्चिका_दीर्घता,
    ध्रुव वर्ण* type, int32_t type_दीर्घता) {
    यदि (builder == शून्यसूचकः) प्रत्यागम;
    आन्तरिकम्(builder)->समुच्चयः_Unicode_Locale_Keyword(
        STRING_PIECE(कुञ्चिका, कुञ्चिका_दीर्घता), STRING_PIECE(type, type_दीर्घता));
}

शून्य ulocbld_clear(ULocaleBuilder* builder) {
    यदि (builder == शून्यसूचकः) प्रत्यागम;
    आन्तरिकम्(builder)->clear();
}

शून्य ulocbld_clearExtensions(ULocaleBuilder* builder) {
    यदि (builder == शून्यसूचकः) प्रत्यागम;
    आन्तरिकम्(builder)->clearExtensions();
}


ULocale* ulocbld_buildULocale(ULocaleBuilder* builder, UError_संहिता* err) {
    यदि (builder == शून्यसूचकः) {
        *err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    icu::Locale l = आन्तरिकम्(builder)->build(*err);
    यदि (U_FAILURE(*err)) प्रत्यागम शून्यसूचकः;
    icu::Locale* r = l.clone();
    यदि (r == शून्यसूचकः) {
        *err = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम reinterpret_cast<ULocale*>(r);
}

int32_t ulocbld_buildLocaleID(ULocaleBuilder* builder,
                              वर्ण* बफर, int32_t बफर_क्षमता, UError_संहिता* err) {
    यदि (U_FAILURE(*err)) { प्रत्यागम 0; }
    यदि (builder == शून्यसूचकः) {
        *err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    icu::Locale l = आन्तरिकम्(builder)->build(*err);
    यदि (U_FAILURE(*err)) { प्रत्यागम 0; }
    int32_t length = static_cast<int32_t>(uprv_strlen(l.getName()));
    यदि (0 < length && length <= बफर_क्षमता) {
        uprv_memcpy(बफर, l.getName(), length);
    }
    प्रत्यागम u_terminateChars(बफर, बफर_क्षमता, length, err);
}

int32_t ulocbld_build_Language_चिह्नकम्(ULocaleBuilder* builder,
                  वर्ण* बफर, int32_t बफर_क्षमता, UError_संहिता* err) {
    यदि (U_FAILURE(*err)) { प्रत्यागम 0; }
    यदि (builder == शून्यसूचकः) {
        *err = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    icu::Locale l = आन्तरिकम्(builder)->build(*err);
    प्रत्यागम icu::ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        बफर, बफर_क्षमता,
        [&](icu::ByteSink& sink, UError_संहिता& स्थितिः) {
            l.गन्तव्यम्_Language_चिह्नकम्(sink, स्थितिः);
        },
        *err);
}

UBool ulocbld_copy_Error_गन्तव्यम्(ध्रुव ULocaleBuilder* builder, UError_संहिता *बहिर्गमनम्_Error_संहिता) {
    यदि (builder == शून्यसूचकः) {
        *बहिर्गमनम्_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम सत्य;
    }
    प्रत्यागम CONST_आन्तरिकम्(builder)->copy_Error_गन्तव्यम्(*बहिर्गमनम्_Error_संहिता);
}
