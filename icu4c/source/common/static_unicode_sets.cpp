// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#घोषणा UNISTR_FROM_STRING_EXPLICIT

#समावेश "static_unicode_sets.h"
#समावेश "umutex.h"
#समावेश "ucln_cmn.h"
#समावेश "unicode/uniset.h"
#समावेश "uresimp.h"
#समावेश "cstring.h"
#समावेश "uassert.h"

using namespace icu;
using namespace icu::unisets;


namespace {

UnicodeSet* gUnicodeSets[UNISETS_कुंजी_गिनती] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
अक्षर gEmptyUnicodeSet[माप(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = false;

अंतरभूत UnicodeSet* getImpl(कुंजी_2 कुंजी) {
    UnicodeSet* candidate = gUnicodeSets[कुंजी];
    अगर (candidate == nullptr) {
        वापस reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    वापस candidate;
}

UnicodeSet* computeUnion(कुंजी_2 k1, कुंजी_2 k2) {
    UnicodeSet* परिणाम = नया UnicodeSet();
    अगर (परिणाम == nullptr) {
        वापस nullptr;
    }
    परिणाम->addAll(*getImpl(k1));
    परिणाम->addAll(*getImpl(k2));
    परिणाम->freeze();
    वापस परिणाम;
}

UnicodeSet* computeUnion(कुंजी_2 k1, कुंजी_2 k2, कुंजी_2 k3) {
    UnicodeSet* परिणाम = नया UnicodeSet();
    अगर (परिणाम == nullptr) {
        वापस nullptr;
    }
    परिणाम->addAll(*getImpl(k1));
    परिणाम->addAll(*getImpl(k2));
    परिणाम->addAll(*getImpl(k3));
    परिणाम->freeze();
    वापस परिणाम;
}


व्योम saveSet(कुंजी_2 कुंजी, स्थिर UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[कुंजी] = नया UnicodeSet(unicodeSetPattern, status);
}

श्रेणी Parse_तथ्य_Sink : खुला ResourceSink {
  खुला:
    व्योम दो(स्थिर अक्षर* कुंजी, Resource_मूल्य& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        अगर (U_FAILURE(status)) { वापस; }
        क्रम (पूर्णांक i = 0; contextsTable.get_कुंजी_And_मूल्य(i, कुंजी, value); i++) {
            अगर (uprv_strcmp(कुंजी, "date") == 0) {
                // ignore
            } अन्यथा {
                ResourceTable strictnessTable = value.getTable(status);
                अगर (U_FAILURE(status)) { वापस; }
                क्रम (पूर्णांक j = 0; strictnessTable.get_कुंजी_And_मूल्य(j, कुंजी, value); j++) {
                    bool isLenient = (uprv_strcmp(कुंजी, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    अगर (U_FAILURE(status)) { वापस; }
                    क्रम (पूर्णांक k = 0; k < array.get_आकार(); k++) {
                        array.get_मूल्य(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        अगर (U_FAILURE(status)) { वापस; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        अगर (str.सूचकांक_Of(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } अन्यथा अगर (str.सूचकांक_Of(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } अन्यथा अगर (str.सूचकांक_Of(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } अन्यथा अगर (str.सूचकांक_Of(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } अन्यथा अगर (str.सूचकांक_Of(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } अन्यथा अगर (str.सूचकांक_Of(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } अन्यथा अगर (str.सूचकांक_Of(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } अन्यथा अगर (str.सूचकांक_Of(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } अन्यथा अगर (str.सूचकांक_Of(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } अन्यथा अगर (str.सूचकांक_Of(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } अन्यथा अगर (str.सूचकांक_Of(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } अन्यथा अगर (str.सूचकांक_Of(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } अन्यथा {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(false);
                        }
                        अगर (U_FAILURE(status)) { वापस; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_संख्या_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_संख्या_Parse_Uni_Sets() {
    अगर (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = false;
    }
    क्रम (int32_t i = 0; i < UNISETS_कुंजी_गिनती; i++) {
        मिटाओ gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_संख्या_Parse_Uni_Sets_Init_Once.reset();
    वापस true;
}

व्योम U_CALLCONV init_संख्या_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_संख्या_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    नया(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = true;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = नया UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = नया UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    अगर (U_FAILURE(status)) { वापस; }
    Parse_तथ्य_Sink sink;
    ures_get_All_वस्तुएँ_With_Fallback(rb.getAlias(), "parse", sink, status);
    अगर (U_FAILURE(status)) { वापस; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(नया UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    अगर (U_FAILURE(status)) { वापस; }
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
    gUnicodeSets[INFINITY_SIGN] = नया UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = नया UnicodeSet(u"[∼~≈≃約]", status);
    अगर (U_FAILURE(status)) { वापस; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = नया UnicodeSet(u"[:digit:]", status);
    अगर (U_FAILURE(status)) { वापस; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    क्रम (स्वतः* uniset : gUnicodeSets) {
        अगर (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

स्थिर UnicodeSet* unisets::लो(कुंजी_2 कुंजी) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_संख्या_Parse_Uni_Sets_Init_Once, &init_संख्या_Parse_Uni_Sets, localStatus);
    अगर (U_FAILURE(localStatus)) {
        वापस reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    वापस getImpl(कुंजी);
}

कुंजी_2 unisets::chooseFrom(UnicodeString str, कुंजी_2 key1) {
    वापस लो(key1)->contains(str) ? key1 : NONE;
}

कुंजी_2 unisets::chooseFrom(UnicodeString str, कुंजी_2 key1, कुंजी_2 key2) {
    वापस लो(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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


#पूर्ण_अगर /* #if !UCONFIG_NO_FORMATTING */
