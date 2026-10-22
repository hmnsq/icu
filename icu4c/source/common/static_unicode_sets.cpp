// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#परिभाषय UNISTR_FROM_STRING_EXPLICIT

#अन्तर्भाव "static_unicode_sets.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uassert.h"

उपयुज्य नामाकाश icu;
उपयुज्य नामाकाश icu::unisets;


नामाकाश {

Unicode_समुच्चयः* gUnicodeSets[UNISETS_कुञ्चिका_गणनम्] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(Unicode_समुच्चयः)
वर्ण g_रिक्तानि_Unicode_समुच्चयः[परिमाण(Unicode_समुच्चयः)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool g_रिक्तानि_Unicode_समुच्चयः_Initialized = असत्य;

अन्तःस्थ Unicode_समुच्चयः* getImpl(कुञ्चिका_2 कुञ्चिका) {
    Unicode_समुच्चयः* candidate = gUnicodeSets[कुञ्चिका];
    यदि (candidate == शून्यसूचकः) {
        प्रत्यागम reinterpret_cast<Unicode_समुच्चयः*>(g_रिक्तानि_Unicode_समुच्चयः);
    }
    प्रत्यागम candidate;
}

Unicode_समुच्चयः* computeUnion(कुञ्चिका_2 k1, कुञ्चिका_2 k2) {
    Unicode_समुच्चयः* परिणाम = नूतन Unicode_समुच्चयः();
    यदि (परिणाम == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }
    परिणाम->सङ्कलय_All(*getImpl(k1));
    परिणाम->सङ्कलय_All(*getImpl(k2));
    परिणाम->freeze();
    प्रत्यागम परिणाम;
}

Unicode_समुच्चयः* computeUnion(कुञ्चिका_2 k1, कुञ्चिका_2 k2, कुञ्चिका_2 k3) {
    Unicode_समुच्चयः* परिणाम = नूतन Unicode_समुच्चयः();
    यदि (परिणाम == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }
    परिणाम->सङ्कलय_All(*getImpl(k1));
    परिणाम->सङ्कलय_All(*getImpl(k2));
    परिणाम->सङ्कलय_All(*getImpl(k3));
    परिणाम->freeze();
    प्रत्यागम परिणाम;
}


शून्य save_समुच्चयः(कुञ्चिका_2 कुञ्चिका, ध्रुव UnicodeString& unicode_समुच्चयः_आदर्शः, UError_संहिता& स्थितिः) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[कुञ्चिका] = नूतन Unicode_समुच्चयः(unicode_समुच्चयः_आदर्शः, स्थितिः);
}

वर्ग Parse_दत्त_Sink : सार्वजनिक ResourceSink {
  सार्वजनिक:
    शून्य put(ध्रुव वर्ण* कुञ्चिका, Resource_मानम्& value, UBool /*noFallback*/, UError_संहिता& स्थितिः) override {
        Resource_सारणी contexts_सारणी = value.get_सारणी(स्थितिः);
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
        प्रति (पूर्णाङ्क i = 0; contexts_सारणी.get_कुञ्चिका_And_मानम्(i, कुञ्चिका, value); i++) {
            यदि (uprv_strcmp(कुञ्चिका, "date") == 0) {
                // ignore
            } अन्यथा {
                Resource_सारणी strictness_सारणी = value.get_सारणी(स्थितिः);
                यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
                प्रति (पूर्णाङ्क j = 0; strictness_सारणी.get_कुञ्चिका_And_मानम्(j, कुञ्चिका, value); j++) {
                    तार्किक isLenient = (uprv_strcmp(कुञ्चिका, "lenient") == 0);
                    ResourceArray array = value.getArray(स्थितिः);
                    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
                    प्रति (पूर्णाङ्क k = 0; k < array.get_आकार(); k++) {
                        array.get_मानम्(k, value);
                        UnicodeString str = value.getUnicodeString(स्थितिः);
                        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        यदि (str.सूचकांक_सम्बन्धि(u'.') != -1) {
                            save_समुच्चयः(isLenient ? PERIOD : STRICT_PERIOD, str, स्थितिः);
                        } अन्यथा यदि (str.सूचकांक_सम्बन्धि(u',') != -1) {
                            save_समुच्चयः(isLenient ? COMMA : STRICT_COMMA, str, स्थितिः);
                        } अन्यथा यदि (str.सूचकांक_सम्बन्धि(u'+') != -1) {
                            save_समुच्चयः(PLUS_SIGN, str, स्थितिः);
                        } अन्यथा यदि (str.सूचकांक_सम्बन्धि(u'-') != -1) {
                            save_समुच्चयः(MINUS_SIGN, str, स्थितिः);
                        } अन्यथा यदि (str.सूचकांक_सम्बन्धि(u'$') != -1) {
                            save_समुच्चयः(DOLLAR_SIGN, str, स्थितिः);
                        } अन्यथा यदि (str.सूचकांक_सम्बन्धि(u'£') != -1) {
                            save_समुच्चयः(POUND_SIGN, str, स्थितिः);
                        } अन्यथा यदि (str.सूचकांक_सम्बन्धि(u'₹') != -1) {
                            save_समुच्चयः(RUPEE_SIGN, str, स्थितिः);
                        } अन्यथा यदि (str.सूचकांक_सम्बन्धि(u'¥') != -1) {
                            save_समुच्चयः(YEN_SIGN, str, स्थितिः);
                        } अन्यथा यदि (str.सूचकांक_सम्बन्धि(u'₩') != -1) {
                            save_समुच्चयः(WON_SIGN, str, स्थितिः);
                        } अन्यथा यदि (str.सूचकांक_सम्बन्धि(u'%') != -1) {
                            save_समुच्चयः(PERCENT_SIGN, str, स्थितिः);
                        } अन्यथा यदि (str.सूचकांक_सम्बन्धि(u'‰') != -1) {
                            save_समुच्चयः(PERMILLE_SIGN, str, स्थितिः);
                        } अन्यथा यदि (str.सूचकांक_सम्बन्धि(u'’') != -1) {
                            save_समुच्चयः(APOSTROPHE_SIGN, str, स्थितिः);
                        } अन्यथा {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(असत्य);
                        }
                        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_सङ्ख्या_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_सङ्ख्या_Parse_Uni_Sets() {
    यदि (g_रिक्तानि_Unicode_समुच्चयः_Initialized) {
        reinterpret_cast<Unicode_समुच्चयः*>(g_रिक्तानि_Unicode_समुच्चयः)->~Unicode_समुच्चयः();
        g_रिक्तानि_Unicode_समुच्चयः_Initialized = असत्य;
    }
    प्रति (int32_t i = 0; i < UNISETS_कुञ्चिका_गणनम्; i++) {
        विलोपय gUnicodeSets[i];
        gUnicodeSets[i] = शून्यसूचकः;
    }
    g_सङ्ख्या_Parse_Uni_Sets_Init_Once.reset();
    प्रत्यागम सत्य;
}

शून्य U_CALLCONV init_सङ्ख्या_Parse_Uni_Sets(UError_संहिता& स्थितिः) {
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_सङ्ख्या_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    नूतन(g_रिक्तानि_Unicode_समुच्चयः) Unicode_समुच्चयः();
    reinterpret_cast<Unicode_समुच्चयः*>(g_रिक्तानि_Unicode_समुच्चयः)->freeze();
    g_रिक्तानि_Unicode_समुच्चयः_Initialized = सत्य;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = नूतन Unicode_समुच्चयः(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", स्थितिः);
    gUnicodeSets[STRICT_IGNORABLES] = नूतन Unicode_समुच्चयः(u"[[:Bidi_Control:]]", स्थितिः);

    LocalUResourceBundlePointer rb(ures_open(शून्यसूचकः, "root", &स्थितिः));
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
    Parse_दत्त_Sink sink;
    ures_get_All_वस्तूनि_With_Fallback(rb.getAlias(), "parse", sink, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != शून्यसूचकः);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != शून्यसूचकः);
    U_ASSERT(gUnicodeSets[PERIOD] != शून्यसूचकः);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != शून्यसूचकः);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != शून्यसूचकः);

    LocalPointer<Unicode_समुच्चयः> otherGrouping(नूतन Unicode_समुच्चयः(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        स्थितिः
    ), स्थितिः);
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
    otherGrouping->सङ्कलय_All(*gUnicodeSets[APOSTROPHE_SIGN]);
    gUnicodeSets[OTHER_GROUPING_SEPARATORS] = otherGrouping.orphan();
    gUnicodeSets[ALL_SEPARATORS] = computeUnion(COMMA, PERIOD, OTHER_GROUPING_SEPARATORS);
    gUnicodeSets[STRICT_ALL_SEPARATORS] = computeUnion(
            STRICT_COMMA, STRICT_PERIOD, OTHER_GROUPING_SEPARATORS);

    U_ASSERT(gUnicodeSets[MINUS_SIGN] != शून्यसूचकः);
    U_ASSERT(gUnicodeSets[PLUS_SIGN] != शून्यसूचकः);
    U_ASSERT(gUnicodeSets[PERCENT_SIGN] != शून्यसूचकः);
    U_ASSERT(gUnicodeSets[PERMILLE_SIGN] != शून्यसूचकः);

    // The following don't currently have parseLenients in data.
    U_ASSERT(gUnicodeSets[INFINITY_SIGN] == शून्यसूचकः);
    gUnicodeSets[INFINITY_SIGN] = नूतन Unicode_समुच्चयः(u"[∞]", स्थितिः);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == शून्यसूचकः);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = नूतन Unicode_समुच्चयः(u"[∼~≈≃約]", स्थितिः);
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != शून्यसूचकः);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != शून्यसूचकः);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != शून्यसूचकः);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != शून्यसूचकः);
    U_ASSERT(gUnicodeSets[WON_SIGN] != शून्यसूचकः);

    gUnicodeSets[DIGITS] = नूतन Unicode_समुच्चयः(u"[:digit:]", स्थितिः);
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    प्रति (स्वचालित* uniset : gUnicodeSets) {
        यदि (uniset != शून्यसूचकः) {
            uniset->freeze();
        }
    }
}

}

ध्रुव Unicode_समुच्चयः* unisets::get(कुञ्चिका_2 कुञ्चिका) {
    UError_संहिता local_स्थितिः = U_ZERO_ERROR;
    umtx_initOnce(g_सङ्ख्या_Parse_Uni_Sets_Init_Once, &init_सङ्ख्या_Parse_Uni_Sets, local_स्थितिः);
    यदि (U_FAILURE(local_स्थितिः)) {
        प्रत्यागम reinterpret_cast<Unicode_समुच्चयः*>(g_रिक्तानि_Unicode_समुच्चयः);
    }
    प्रत्यागम getImpl(कुञ्चिका);
}

कुञ्चिका_2 unisets::वृणु_From(UnicodeString str, कुञ्चिका_2 key1) {
    प्रत्यागम get(key1)->अन्तर्भवति(str) ? key1 : NONE;
}

कुञ्चिका_2 unisets::वृणु_From(UnicodeString str, कुञ्चिका_2 key1, कुञ्चिका_2 key2) {
    प्रत्यागम get(key1)->अन्तर्भवति(str) ? key1 : वृणु_From(str, key2);
}

//Key unisets::chooseCurrency(UnicodeString str) {
//    if (get(DOLLAR_SIGN)->contains(str)) {
//        return DOLLAR_SIGN;
//    } else if (get(POUND_SIGN)->contains(str)) {
//        return POUND_SIGN;
//    } else if (get(RUPEE_SIGN)->contains(str)) {
//        return RUPEE_SIGN;
//    } else if (get(YEN_SIGN)->contains(str)) {
//        return YEN_SIGN;
//    } else {
//        return NONE;
//    }
//}


#यदि_अन्तः /* #if !UCONFIG_NO_FORMATTING */
