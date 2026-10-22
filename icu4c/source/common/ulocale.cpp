// © 2023 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/errorcode.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ulocale.h"
#अन्तर्भाव "unicode/locid.h"

#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_USE
#परिभाषय EXTERNAL(i) (reinterpret_cast<ULocale*>(i))
#परिभाषय CONST_आन्तरिकम्(e) (reinterpret_cast<ध्रुव icu::Locale*>(e))
#परिभाषय आन्तरिकम्(e) (reinterpret_cast<icu::Locale*>(e))

ULocale*
ulocale_openForLocaleID(ध्रुव वर्ण* localeID, int32_t length, UError_संहिता* err) {
    यदि (U_FAILURE(*err)) { प्रत्यागम शून्यसूचकः; }
    यदि (length < 0) {
        प्रत्यागम EXTERNAL(icu::Locale::createFromName(localeID).clone());
    }
    प्रत्यागम EXTERNAL(icu::Locale::createFromName(StringPiece{localeID, length}).clone());
}

ULocale*
ulocale_open_For_Language_चिह्नकम्(ध्रुव वर्ण* चिह्नकम्, int32_t length, UError_संहिता* err) {
  यदि (U_FAILURE(*err)) { प्रत्यागम शून्यसूचकः; }
  Locale l = icu::Locale::for_Language_चिह्नकम्(length < 0 ? StringPiece(चिह्नकम्) : StringPiece(चिह्नकम्, length), *err);
  यदि (U_FAILURE(*err)) { प्रत्यागम शून्यसूचकः; }
  प्रत्यागम EXTERNAL(l.clone());
}

शून्य
ulocale_close(ULocale* locale) {
    विलोपय आन्तरिकम्(locale);
}

#परिभाषय IMPL_ULOCALE_STRING_GETTER(N1, N2) \
ध्रुव वर्ण* ulocale_get ## N1(ध्रुव ULocale* locale U_LIFETIME_BOUND) { \
    यदि (locale == शून्यसूचकः) प्रत्यागम शून्यसूचकः; \
    प्रत्यागम CONST_आन्तरिकम्(locale)->get ## N2(); \
}

#परिभाषय IMPL_ULOCALE_STRING_IDENTICAL_GETTER(N) IMPL_ULOCALE_STRING_GETTER(N, N)

#परिभाषय IMPL_ULOCALE_GET_KEYWORD_मानम्(N) \
int32_t ulocale_get ##N ( \
    ध्रुव ULocale* locale, ध्रुव वर्ण* keyword, int32_t keyword_दीर्घता, \
    वर्ण* मानम्_बफर, int32_t बफर_क्षमता, UError_संहिता *err) { \
    यदि (U_FAILURE(*err)) प्रत्यागम 0; \
    यदि (locale == शून्यसूचकः) { \
        *err = U_ILLEGAL_तर्कः_ERROR; \
        प्रत्यागम 0; \
    } \
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars( \
        मानम्_बफर, बफर_क्षमता, \
        [&](ByteSink& sink, UError_संहिता& स्थितिः) { \
            CONST_आन्तरिकम्(locale)->get ## N( \
                keyword_दीर्घता < 0 ? StringPiece(keyword) : StringPiece(keyword, keyword_दीर्घता), \
                sink, स्थितिः); \
        }, \
        *err); \
}

#परिभाषय IMPL_ULOCALE_GET_KEYWORDS(N) \
UEnumeration* ulocale_get ## N(ध्रुव ULocale* locale, UError_संहिता *err) { \
    यदि (U_FAILURE(*err)) प्रत्यागम शून्यसूचकः; \
    यदि (locale == शून्यसूचकः) { \
        *err = U_ILLEGAL_तर्कः_ERROR; \
        प्रत्यागम शून्यसूचकः; \
    } \
    प्रत्यागम uenum_openFromStringEnumeration( \
        CONST_आन्तरिकम्(locale)->create ## N(*err), err); \
}

IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Language)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Script)
IMPL_ULOCALE_STRING_GETTER(Region, Country)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(Variant)
IMPL_ULOCALE_STRING_GETTER(LocaleID, Name)
IMPL_ULOCALE_STRING_IDENTICAL_GETTER(आधारः_Name_2)
IMPL_ULOCALE_GET_KEYWORD_मानम्(Keyword_मानम्)
IMPL_ULOCALE_GET_KEYWORD_मानम्(Unicode_Keyword_मानम्)
IMPL_ULOCALE_GET_KEYWORDS(Keywords)
IMPL_ULOCALE_GET_KEYWORDS(UnicodeKeywords)

तार्किक ulocale_isBogus(ध्रुव ULocale* locale) {
    यदि (locale == शून्यसूचकः) प्रत्यागम असत्य;
    प्रत्यागम CONST_आन्तरिकम्(locale)->isBogus();
}

/*eof*/
