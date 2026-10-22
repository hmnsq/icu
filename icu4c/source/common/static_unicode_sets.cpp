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
অক্ষর gEmptyUnicodeSet[আকার(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = মিথ্যা;

অন্তঃরেখ UnicodeSet* getImpl(চাবি_2 চাবি) {
    UnicodeSet* candidate = gUnicodeSets[চাবি];
    যদি (candidate == nullptr) {
        ফেরত reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    ফেরত candidate;
}

UnicodeSet* computeUnion(চাবি_2 k1, চাবি_2 k2) {
    UnicodeSet* ফলাফল = নতুন UnicodeSet();
    যদি (ফলাফল == nullptr) {
        ফেরত nullptr;
    }
    ফলাফল->addAll(*getImpl(k1));
    ফলাফল->addAll(*getImpl(k2));
    ফলাফল->freeze();
    ফেরত ফলাফল;
}

UnicodeSet* computeUnion(চাবি_2 k1, চাবি_2 k2, চাবি_2 k3) {
    UnicodeSet* ফলাফল = নতুন UnicodeSet();
    যদি (ফলাফল == nullptr) {
        ফেরত nullptr;
    }
    ফলাফল->addAll(*getImpl(k1));
    ফলাফল->addAll(*getImpl(k2));
    ফলাফল->addAll(*getImpl(k3));
    ফলাফল->freeze();
    ফেরত ফলাফল;
}


শূন্য saveSet(চাবি_2 চাবি, ধ্রুবক UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[চাবি] = নতুন UnicodeSet(unicodeSetPattern, status);
}

শ্রেণী Parse_তথ্য_Sink : সর্বজনীন ResourceSink {
  সর্বজনীন:
    শূন্য put(ধ্রুবক অক্ষর* চাবি, Resource_মান& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        যদি (U_FAILURE(status)) { ফেরত; }
        জন্য (পূর্ণসংখ্যা i = 0; contextsTable.get_চাবি_And_মান(i, চাবি, value); i++) {
            যদি (uprv_strcmp(চাবি, "date") == 0) {
                // ignore
            } অন্যথা {
                ResourceTable strictnessTable = value.getTable(status);
                যদি (U_FAILURE(status)) { ফেরত; }
                জন্য (পূর্ণসংখ্যা j = 0; strictnessTable.get_চাবি_And_মান(j, চাবি, value); j++) {
                    যৌক্তিক isLenient = (uprv_strcmp(চাবি, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    যদি (U_FAILURE(status)) { ফেরত; }
                    জন্য (পূর্ণসংখ্যা k = 0; k < array.get_আকার(); k++) {
                        array.get_মান(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        যদি (U_FAILURE(status)) { ফেরত; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        যদি (str.সূচক_Of(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } অন্যথা যদি (str.সূচক_Of(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } অন্যথা যদি (str.সূচক_Of(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } অন্যথা যদি (str.সূচক_Of(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } অন্যথা যদি (str.সূচক_Of(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } অন্যথা যদি (str.সূচক_Of(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } অন্যথা যদি (str.সূচক_Of(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } অন্যথা যদি (str.সূচক_Of(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } অন্যথা যদি (str.সূচক_Of(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } অন্যথা যদি (str.সূচক_Of(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } অন্যথা যদি (str.সূচক_Of(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } অন্যথা যদি (str.সূচক_Of(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } অন্যথা {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(মিথ্যা);
                        }
                        যদি (U_FAILURE(status)) { ফেরত; }
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
        gEmptyUnicodeSetInitialized = মিথ্যা;
    }
    জন্য (int32_t i = 0; i < UNISETS_চাবি_গুনতি; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_সংখ্যা_Parse_Uni_Sets_Init_Once.reset();
    ফেরত সত্য;
}

শূন্য U_CALLCONV init_সংখ্যা_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_সংখ্যা_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    নতুন(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = সত্য;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = নতুন UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = নতুন UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    যদি (U_FAILURE(status)) { ফেরত; }
    Parse_তথ্য_Sink sink;
    ures_get_All_বস্তুসমূহ_With_Fallback(rb.getAlias(), "parse", sink, status);
    যদি (U_FAILURE(status)) { ফেরত; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(নতুন UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    যদি (U_FAILURE(status)) { ফেরত; }
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
    gUnicodeSets[INFINITY_SIGN] = নতুন UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = নতুন UnicodeSet(u"[∼~≈≃約]", status);
    যদি (U_FAILURE(status)) { ফেরত; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = নতুন UnicodeSet(u"[:digit:]", status);
    যদি (U_FAILURE(status)) { ফেরত; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    জন্য (auto* uniset : gUnicodeSets) {
        যদি (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

ধ্রুবক UnicodeSet* unisets::get(চাবি_2 চাবি) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_সংখ্যা_Parse_Uni_Sets_Init_Once, &init_সংখ্যা_Parse_Uni_Sets, localStatus);
    যদি (U_FAILURE(localStatus)) {
        ফেরত reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    ফেরত getImpl(চাবি);
}

চাবি_2 unisets::chooseFrom(UnicodeString str, চাবি_2 key1) {
    ফেরত get(key1)->contains(str) ? key1 : NONE;
}

চাবি_2 unisets::chooseFrom(UnicodeString str, চাবি_2 key1, চাবি_2 key2) {
    ফেরত get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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
