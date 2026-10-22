// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#define UNISTR_FROM_STRING_EXPLICIT

#अन्तर्भाव "static_unicode_sets.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uassert.h"

using नामाकाश icu;
using नामाकाश icu::unisets;


नामाकाश {

UnicodeSet* gUnicodeSets[UNISETS_कुञ्चिका_गणनम्] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
वर्ण gEmptyUnicodeSet[परिमाण(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = मिच्छा;

अन्तःस्थ UnicodeSet* getImpl(कुञ्चिका_2 कुञ्चिका) {
    UnicodeSet* candidate = gUnicodeSets[कुञ्चिका];
    सचे (candidate == nullptr) {
        पटिनिवत्त reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    पटिनिवत्त candidate;
}

UnicodeSet* computeUnion(कुञ्चिका_2 k1, कुञ्चिका_2 k2) {
    UnicodeSet* result = नूतन UnicodeSet();
    सचे (result == nullptr) {
        पटिनिवत्त nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    पटिनिवत्त result;
}

UnicodeSet* computeUnion(कुञ्चिका_2 k1, कुञ्चिका_2 k2, कुञ्चिका_2 k3) {
    UnicodeSet* result = नूतन UnicodeSet();
    सचे (result == nullptr) {
        पटिनिवत्त nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    पटिनिवत्त result;
}


शून्य saveSet(कुञ्चिका_2 कुञ्चिका, ध्रुव UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[कुञ्चिका] = नूतन UnicodeSet(unicodeSetPattern, status);
}

वग्ग ParseDataSink : सार्वजनिक ResourceSink {
  सार्वजनिक:
    शून्य put(ध्रुव वर्ण* कुञ्चिका, Resource_मानम्& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        सचे (U_FAILURE(status)) { पटिनिवत्त; }
        अत्थाय (पूर्णाङ्क i = 0; contextsTable.get_कुञ्चिका_And_मानम्(i, कुञ्चिका, value); i++) {
            सचे (uprv_strcmp(कुञ्चिका, "date") == 0) {
                // ignore
            } अञ्ञथा {
                ResourceTable strictnessTable = value.getTable(status);
                सचे (U_FAILURE(status)) { पटिनिवत्त; }
                अत्थाय (पूर्णाङ्क j = 0; strictnessTable.get_कुञ्चिका_And_मानम्(j, कुञ्चिका, value); j++) {
                    तार्किक isLenient = (uprv_strcmp(कुञ्चिका, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    सचे (U_FAILURE(status)) { पटिनिवत्त; }
                    अत्थाय (पूर्णाङ्क k = 0; k < array.getSize(); k++) {
                        array.get_मानम्(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        सचे (U_FAILURE(status)) { पटिनिवत्त; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        सचे (str.indexOf(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } अञ्ञथा सचे (str.indexOf(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } अञ्ञथा सचे (str.indexOf(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } अञ्ञथा सचे (str.indexOf(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } अञ्ञथा सचे (str.indexOf(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } अञ्ञथा सचे (str.indexOf(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } अञ्ञथा सचे (str.indexOf(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } अञ्ञथा सचे (str.indexOf(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } अञ्ञथा सचे (str.indexOf(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } अञ्ञथा सचे (str.indexOf(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } अञ्ञथा सचे (str.indexOf(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } अञ्ञथा सचे (str.indexOf(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } अञ्ञथा {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(मिच्छा);
                        }
                        सचे (U_FAILURE(status)) { पटिनिवत्त; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_सङ्ख्या_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_सङ्ख्या_Parse_Uni_Sets() {
    सचे (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = मिच्छा;
    }
    अत्थाय (int32_t i = 0; i < UNISETS_कुञ्चिका_गणनम्; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_सङ्ख्या_Parse_Uni_Sets_Init_Once.reset();
    पटिनिवत्त सच्च;
}

शून्य U_CALLCONV init_सङ्ख्या_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_सङ्ख्या_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    नूतन(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = सच्च;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = नूतन UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = नूतन UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    सचे (U_FAILURE(status)) { पटिनिवत्त; }
    ParseDataSink sink;
    ures_get_All_वस्तूनि_With_Fallback(rb.getAlias(), "parse", sink, status);
    सचे (U_FAILURE(status)) { पटिनिवत्त; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(नूतन UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    सचे (U_FAILURE(status)) { पटिनिवत्त; }
    otherGrouping->addAll(*gUnicodeSets[APOSTROPHE_SIGN]);
    gUnicodeSets[OTHER_GROUPING_SEPARATORS] = otherGrouping.orphan();
    gUnicodeSets[ALL_SEPARATORS] = computeUnion(COMMA, PERIOD, OTHER_GROUPING_SEPARATORS);
    gUnicodeSets[STRICT_ALL_SEPARATORS] = computeUnion(
            STRICT_COMMA, STRICT_PERIOD, OTHER_GROUPING_SEPARATORS);

    U_ASSERT(gUnicodeSets[MINUS_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[PLUS_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[PERCENT_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[PERMILLE_SIGN] != nullptr);

    // The following don't currently have parseLenients in data.
    U_ASSERT(gUnicodeSets[INFINITY_SIGN] == nullptr);
    gUnicodeSets[INFINITY_SIGN] = नूतन UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = नूतन UnicodeSet(u"[∼~≈≃約]", status);
    सचे (U_FAILURE(status)) { पटिनिवत्त; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = नूतन UnicodeSet(u"[:digit:]", status);
    सचे (U_FAILURE(status)) { पटिनिवत्त; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    अत्थाय (auto* uniset : gUnicodeSets) {
        सचे (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

ध्रुव UnicodeSet* unisets::get(कुञ्चिका_2 कुञ्चिका) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_सङ्ख्या_Parse_Uni_Sets_Init_Once, &init_सङ्ख्या_Parse_Uni_Sets, localStatus);
    सचे (U_FAILURE(localStatus)) {
        पटिनिवत्त reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    पटिनिवत्त getImpl(कुञ्चिका);
}

कुञ्चिका_2 unisets::chooseFrom(UnicodeString str, कुञ्चिका_2 key1) {
    पटिनिवत्त get(key1)->contains(str) ? key1 : NONE;
}

कुञ्चिका_2 unisets::chooseFrom(UnicodeString str, कुञ्चिका_2 key1, कुञ्चिका_2 key2) {
    पटिनिवत्त get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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


#endif /* #if !UCONFIG_NO_FORMATTING */
