// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_FORMATTING

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

UnicodeSet* gUnicodeSets[UNISETS_किल्ली_मोजणी] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
वर्ण gEmptyUnicodeSet[परिमाण(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = असत्य;

अन्तःस्थ UnicodeSet* getImpl(किल्ली_2 किल्ली) {
    UnicodeSet* candidate = gUnicodeSets[किल्ली];
    यदि (candidate == nullptr) {
        प्रत्यागम reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    प्रत्यागम candidate;
}

UnicodeSet* computeUnion(किल्ली_2 k1, किल्ली_2 k2) {
    UnicodeSet* result = नूतन UnicodeSet();
    यदि (result == nullptr) {
        प्रत्यागम nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    प्रत्यागम result;
}

UnicodeSet* computeUnion(किल्ली_2 k1, किल्ली_2 k2, किल्ली_2 k3) {
    UnicodeSet* result = नूतन UnicodeSet();
    यदि (result == nullptr) {
        प्रत्यागम nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    प्रत्यागम result;
}


शून्य saveSet(किल्ली_2 किल्ली, ध्रुव UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[किल्ली] = नूतन UnicodeSet(unicodeSetPattern, status);
}

वर्ग ParseDataSink : सार्वजनिक ResourceSink {
  सार्वजनिक:
    शून्य put(ध्रुव वर्ण* किल्ली, Resource_मूल्य& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        यदि (U_FAILURE(status)) { प्रत्यागम; }
        प्रति (पूर्णाङ्क i = 0; contextsTable.get_किल्ली_And_मूल्य(i, किल्ली, value); i++) {
            यदि (uprv_strcmp(किल्ली, "date") == 0) {
                // ignore
            } अन्यथा {
                ResourceTable strictnessTable = value.getTable(status);
                यदि (U_FAILURE(status)) { प्रत्यागम; }
                प्रति (पूर्णाङ्क j = 0; strictnessTable.get_किल्ली_And_मूल्य(j, किल्ली, value); j++) {
                    तार्किक isLenient = (uprv_strcmp(किल्ली, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    यदि (U_FAILURE(status)) { प्रत्यागम; }
                    प्रति (पूर्णाङ्क k = 0; k < array.getSize(); k++) {
                        array.get_मूल्य(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        यदि (U_FAILURE(status)) { प्रत्यागम; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        यदि (str.indexOf(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } अन्यथा यदि (str.indexOf(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } अन्यथा यदि (str.indexOf(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } अन्यथा यदि (str.indexOf(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } अन्यथा यदि (str.indexOf(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } अन्यथा यदि (str.indexOf(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } अन्यथा यदि (str.indexOf(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } अन्यथा यदि (str.indexOf(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } अन्यथा यदि (str.indexOf(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } अन्यथा यदि (str.indexOf(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } अन्यथा यदि (str.indexOf(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } अन्यथा यदि (str.indexOf(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } अन्यथा {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(असत्य);
                        }
                        यदि (U_FAILURE(status)) { प्रत्यागम; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_संख्या_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_संख्या_Parse_Uni_Sets() {
    यदि (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = असत्य;
    }
    प्रति (int32_t i = 0; i < UNISETS_किल्ली_मोजणी; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_संख्या_Parse_Uni_Sets_Init_Once.reset();
    प्रत्यागम सत्य;
}

शून्य U_CALLCONV init_संख्या_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_संख्या_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    नूतन(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = सत्य;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = नूतन UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = नूतन UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    यदि (U_FAILURE(status)) { प्रत्यागम; }
    ParseDataSink sink;
    ures_get_All_वस्तुसंच_With_Fallback(rb.getAlias(), "parse", sink, status);
    यदि (U_FAILURE(status)) { प्रत्यागम; }

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
    यदि (U_FAILURE(status)) { प्रत्यागम; }
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
    यदि (U_FAILURE(status)) { प्रत्यागम; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = नूतन UnicodeSet(u"[:digit:]", status);
    यदि (U_FAILURE(status)) { प्रत्यागम; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    प्रति (auto* uniset : gUnicodeSets) {
        यदि (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

ध्रुव UnicodeSet* unisets::get(किल्ली_2 किल्ली) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_संख्या_Parse_Uni_Sets_Init_Once, &init_संख्या_Parse_Uni_Sets, localStatus);
    यदि (U_FAILURE(localStatus)) {
        प्रत्यागम reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    प्रत्यागम getImpl(किल्ली);
}

किल्ली_2 unisets::chooseFrom(UnicodeString str, किल्ली_2 key1) {
    प्रत्यागम get(key1)->contains(str) ? key1 : NONE;
}

किल्ली_2 unisets::chooseFrom(UnicodeString str, किल्ली_2 key1, किल्ली_2 key2) {
    प्रत्यागम get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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
