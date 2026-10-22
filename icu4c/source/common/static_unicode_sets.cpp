// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#define UNISTR_FROM_STRING_EXPLICIT

#অন্তর্ভাব "static_unicode_sets.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "uassert.h"

using নামাকাশ icu;
using নামাকাশ icu::unisets;


নামাকাশ {

UnicodeSet* gUnicodeSets[UNISETS_চাবি_গুনতি] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
বর্ণ gEmptyUnicodeSet[পরিমাণ(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = অসত্য;

অন্তঃস্থ UnicodeSet* getImpl(চাবি_2 চাবি) {
    UnicodeSet* candidate = gUnicodeSets[চাবি];
    যদি (candidate == nullptr) {
        প্রত্যাগম reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    প্রত্যাগম candidate;
}

UnicodeSet* computeUnion(চাবি_2 k1, চাবি_2 k2) {
    UnicodeSet* result = নূতন UnicodeSet();
    যদি (result == nullptr) {
        প্রত্যাগম nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    প্রত্যাগম result;
}

UnicodeSet* computeUnion(চাবি_2 k1, চাবি_2 k2, চাবি_2 k3) {
    UnicodeSet* result = নূতন UnicodeSet();
    যদি (result == nullptr) {
        প্রত্যাগম nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    প্রত্যাগম result;
}


শূন্য saveSet(চাবি_2 চাবি, ধ্রুব UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[চাবি] = নূতন UnicodeSet(unicodeSetPattern, status);
}

বর্গ ParseDataSink : সার্বজনিক ResourceSink {
  সার্বজনিক:
    শূন্য put(ধ্রুব বর্ণ* চাবি, Resource_মান& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        যদি (U_FAILURE(status)) { প্রত্যাগম; }
        প্রতি (পূর্ণাঙ্ক i = 0; contextsTable.get_চাবি_And_মান(i, চাবি, value); i++) {
            যদি (uprv_strcmp(চাবি, "date") == 0) {
                // ignore
            } অন্যথা {
                ResourceTable strictnessTable = value.getTable(status);
                যদি (U_FAILURE(status)) { প্রত্যাগম; }
                প্রতি (পূর্ণাঙ্ক j = 0; strictnessTable.get_চাবি_And_মান(j, চাবি, value); j++) {
                    তার্কিক isLenient = (uprv_strcmp(চাবি, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    যদি (U_FAILURE(status)) { প্রত্যাগম; }
                    প্রতি (পূর্ণাঙ্ক k = 0; k < array.getSize(); k++) {
                        array.get_মান(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        যদি (U_FAILURE(status)) { প্রত্যাগম; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        যদি (str.indexOf(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } অন্যথা যদি (str.indexOf(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } অন্যথা যদি (str.indexOf(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } অন্যথা যদি (str.indexOf(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } অন্যথা যদি (str.indexOf(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } অন্যথা যদি (str.indexOf(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } অন্যথা যদি (str.indexOf(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } অন্যথা যদি (str.indexOf(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } অন্যথা যদি (str.indexOf(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } অন্যথা যদি (str.indexOf(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } অন্যথা যদি (str.indexOf(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } অন্যথা যদি (str.indexOf(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } অন্যথা {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(অসত্য);
                        }
                        যদি (U_FAILURE(status)) { প্রত্যাগম; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_সংখ্যা_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_সংখ্যা_Parse_Uni_Sets() {
    যদি (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = অসত্য;
    }
    প্রতি (int32_t i = 0; i < UNISETS_চাবি_গুনতি; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_সংখ্যা_Parse_Uni_Sets_Init_Once.reset();
    প্রত্যাগম সত্য;
}

শূন্য U_CALLCONV init_সংখ্যা_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_সংখ্যা_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    নূতন(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = সত্য;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = নূতন UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = নূতন UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    যদি (U_FAILURE(status)) { প্রত্যাগম; }
    ParseDataSink sink;
    ures_get_All_বস্তুসমূহ_With_Fallback(rb.getAlias(), "parse", sink, status);
    যদি (U_FAILURE(status)) { প্রত্যাগম; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(নূতন UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    যদি (U_FAILURE(status)) { প্রত্যাগম; }
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
    gUnicodeSets[INFINITY_SIGN] = নূতন UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = নূতন UnicodeSet(u"[∼~≈≃約]", status);
    যদি (U_FAILURE(status)) { প্রত্যাগম; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = নূতন UnicodeSet(u"[:digit:]", status);
    যদি (U_FAILURE(status)) { প্রত্যাগম; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    প্রতি (auto* uniset : gUnicodeSets) {
        যদি (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

ধ্রুব UnicodeSet* unisets::get(চাবি_2 চাবি) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_সংখ্যা_Parse_Uni_Sets_Init_Once, &init_সংখ্যা_Parse_Uni_Sets, localStatus);
    যদি (U_FAILURE(localStatus)) {
        প্রত্যাগম reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    প্রত্যাগম getImpl(চাবি);
}

চাবি_2 unisets::chooseFrom(UnicodeString str, চাবি_2 key1) {
    প্রত্যাগম get(key1)->contains(str) ? key1 : NONE;
}

চাবি_2 unisets::chooseFrom(UnicodeString str, চাবি_2 key1, চাবি_2 key2) {
    প্রত্যাগম get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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
