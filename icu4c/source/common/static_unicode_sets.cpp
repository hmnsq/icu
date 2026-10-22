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

UnicodeSet* gUnicodeSets[UNISETS_साँचो_गन्ती] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
वर्ण gEmptyUnicodeSet[परिमाण(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = असत्य;

अन्तःस्थ UnicodeSet* getImpl(साँचो_2 साँचो) {
    UnicodeSet* candidate = gUnicodeSets[साँचो];
    यदि (candidate == nullptr) {
        फर्काऊ reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    फर्काऊ candidate;
}

UnicodeSet* computeUnion(साँचो_2 k1, साँचो_2 k2) {
    UnicodeSet* result = नूतन UnicodeSet();
    यदि (result == nullptr) {
        फर्काऊ nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    फर्काऊ result;
}

UnicodeSet* computeUnion(साँचो_2 k1, साँचो_2 k2, साँचो_2 k3) {
    UnicodeSet* result = नूतन UnicodeSet();
    यदि (result == nullptr) {
        फर्काऊ nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    फर्काऊ result;
}


शून्य saveSet(साँचो_2 साँचो, अचर UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[साँचो] = नूतन UnicodeSet(unicodeSetPattern, status);
}

वर्ग ParseDataSink : सार्वजनिक ResourceSink {
  सार्वजनिक:
    शून्य put(अचर वर्ण* साँचो, Resource_मान& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        यदि (U_FAILURE(status)) { फर्काऊ; }
        लागि (पूर्णाङ्क i = 0; contextsTable.get_साँचो_And_मान(i, साँचो, value); i++) {
            यदि (uprv_strcmp(साँचो, "date") == 0) {
                // ignore
            } अन्यथा {
                ResourceTable strictnessTable = value.getTable(status);
                यदि (U_FAILURE(status)) { फर्काऊ; }
                लागि (पूर्णाङ्क j = 0; strictnessTable.get_साँचो_And_मान(j, साँचो, value); j++) {
                    तार्किक isLenient = (uprv_strcmp(साँचो, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    यदि (U_FAILURE(status)) { फर्काऊ; }
                    लागि (पूर्णाङ्क k = 0; k < array.getSize(); k++) {
                        array.get_मान(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        यदि (U_FAILURE(status)) { फर्काऊ; }
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
                        यदि (U_FAILURE(status)) { फर्काऊ; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_सङ्ख्या_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_सङ्ख्या_Parse_Uni_Sets() {
    यदि (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = असत्य;
    }
    लागि (int32_t i = 0; i < UNISETS_साँचो_गन्ती; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_सङ्ख्या_Parse_Uni_Sets_Init_Once.reset();
    फर्काऊ सत्य;
}

शून्य U_CALLCONV init_सङ्ख्या_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_सङ्ख्या_Parse_Uni_Sets);

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
    यदि (U_FAILURE(status)) { फर्काऊ; }
    ParseDataSink sink;
    ures_get_All_वस्तुहरू_With_Fallback(rb.getAlias(), "parse", sink, status);
    यदि (U_FAILURE(status)) { फर्काऊ; }

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
    यदि (U_FAILURE(status)) { फर्काऊ; }
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
    यदि (U_FAILURE(status)) { फर्काऊ; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = नूतन UnicodeSet(u"[:digit:]", status);
    यदि (U_FAILURE(status)) { फर्काऊ; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    लागि (स्वतः* uniset : gUnicodeSets) {
        यदि (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

अचर UnicodeSet* unisets::get(साँचो_2 साँचो) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_सङ्ख्या_Parse_Uni_Sets_Init_Once, &init_सङ्ख्या_Parse_Uni_Sets, localStatus);
    यदि (U_FAILURE(localStatus)) {
        फर्काऊ reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    फर्काऊ getImpl(साँचो);
}

साँचो_2 unisets::chooseFrom(UnicodeString str, साँचो_2 key1) {
    फर्काऊ get(key1)->contains(str) ? key1 : NONE;
}

साँचो_2 unisets::chooseFrom(UnicodeString str, साँचो_2 key1, साँचो_2 key2) {
    फर्काऊ get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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
