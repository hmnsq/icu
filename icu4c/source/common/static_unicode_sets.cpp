// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#define UNISTR_FROM_STRING_EXPLICIT

#ಅನ್ತರ್ಭಾವ "static_unicode_sets.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

using ನಾಮಾಕಾಶ icu;
using ನಾಮಾಕಾಶ icu::unisets;


ನಾಮಾಕಾಶ {

UnicodeSet* gUnicodeSets[UNISETS_ಕೀಲಿ_ಎಣಿಕೆ] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
ವರ್ಣ gEmptyUnicodeSet[ಪರಿಮಾಣ(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = ಅಸತ್ಯ;

ಅನ್ತಃಸ್ಥ UnicodeSet* getImpl(ಕೀಲಿ_2 ಕೀಲಿ) {
    UnicodeSet* candidate = gUnicodeSets[ಕೀಲಿ];
    ಯದಿ (candidate == nullptr) {
        ಪ್ರತ್ಯಾಗಮ reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    ಪ್ರತ್ಯಾಗಮ candidate;
}

UnicodeSet* computeUnion(ಕೀಲಿ_2 k1, ಕೀಲಿ_2 k2) {
    UnicodeSet* result = ನೂತನ UnicodeSet();
    ಯದಿ (result == nullptr) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeSet* computeUnion(ಕೀಲಿ_2 k1, ಕೀಲಿ_2 k2, ಕೀಲಿ_2 k3) {
    UnicodeSet* result = ನೂತನ UnicodeSet();
    ಯದಿ (result == nullptr) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    ಪ್ರತ್ಯಾಗಮ result;
}


ಶೂನ್ಯ saveSet(ಕೀಲಿ_2 ಕೀಲಿ, ಧ್ರುವ UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[ಕೀಲಿ] = ನೂತನ UnicodeSet(unicodeSetPattern, status);
}

ವರ್ಗ ParseDataSink : ಸಾರ್ವಜನಿಕ ResourceSink {
  ಸಾರ್ವಜನಿಕ:
    ಶೂನ್ಯ put(ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ, Resource_ಮೌಲ್ಯ& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; contextsTable.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(i, ಕೀಲಿ, value); i++) {
            ಯದಿ (uprv_strcmp(ಕೀಲಿ, "date") == 0) {
                // ignore
            } ಅನ್ಯಥಾ {
                ResourceTable strictnessTable = value.getTable(status);
                ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
                ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; strictnessTable.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(j, ಕೀಲಿ, value); j++) {
                    ತಾರ್ಕಿಕ isLenient = (uprv_strcmp(ಕೀಲಿ, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
                    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < array.getSize(); k++) {
                        array.get_ಮೌಲ್ಯ(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        ಯದಿ (str.indexOf(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } ಅನ್ಯಥಾ ಯದಿ (str.indexOf(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } ಅನ್ಯಥಾ ಯದಿ (str.indexOf(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } ಅನ್ಯಥಾ ಯದಿ (str.indexOf(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } ಅನ್ಯಥಾ ಯದಿ (str.indexOf(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } ಅನ್ಯಥಾ ಯದಿ (str.indexOf(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } ಅನ್ಯಥಾ ಯದಿ (str.indexOf(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } ಅನ್ಯಥಾ ಯದಿ (str.indexOf(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } ಅನ್ಯಥಾ ಯದಿ (str.indexOf(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } ಅನ್ಯಥಾ ಯದಿ (str.indexOf(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } ಅನ್ಯಥಾ ಯದಿ (str.indexOf(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } ಅನ್ಯಥಾ ಯದಿ (str.indexOf(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } ಅನ್ಯಥಾ {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(ಅಸತ್ಯ);
                        }
                        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_ಸಂಖ್ಯೆ_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_ಸಂಖ್ಯೆ_Parse_Uni_Sets() {
    ಯದಿ (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = ಅಸತ್ಯ;
    }
    ಪ್ರತಿ (int32_t i = 0; i < UNISETS_ಕೀಲಿ_ಎಣಿಕೆ; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_ಸಂಖ್ಯೆ_Parse_Uni_Sets_Init_Once.reset();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ U_CALLCONV init_ಸಂಖ್ಯೆ_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_ಸಂಖ್ಯೆ_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    ನೂತನ(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = ಸತ್ಯ;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = ನೂತನ UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = ನೂತನ UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
    ParseDataSink sink;
    ures_get_All_ವಸ್ತುಗಳು_With_Fallback(rb.getAlias(), "parse", sink, status);
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(ನೂತನ UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
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
    gUnicodeSets[INFINITY_SIGN] = ನೂತನ UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = ನೂತನ UnicodeSet(u"[∼~≈≃約]", status);
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = ನೂತನ UnicodeSet(u"[:digit:]", status);
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    ಪ್ರತಿ (auto* uniset : gUnicodeSets) {
        ಯದಿ (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

ಧ್ರುವ UnicodeSet* unisets::get(ಕೀಲಿ_2 ಕೀಲಿ) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_ಸಂಖ್ಯೆ_Parse_Uni_Sets_Init_Once, &init_ಸಂಖ್ಯೆ_Parse_Uni_Sets, localStatus);
    ಯದಿ (U_FAILURE(localStatus)) {
        ಪ್ರತ್ಯಾಗಮ reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    ಪ್ರತ್ಯಾಗಮ getImpl(ಕೀಲಿ);
}

ಕೀಲಿ_2 unisets::chooseFrom(UnicodeString str, ಕೀಲಿ_2 key1) {
    ಪ್ರತ್ಯಾಗಮ get(key1)->contains(str) ? key1 : NONE;
}

ಕೀಲಿ_2 unisets::chooseFrom(UnicodeString str, ಕೀಲಿ_2 key1, ಕೀಲಿ_2 key2) {
    ಪ್ರತ್ಯಾಗಮ get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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
