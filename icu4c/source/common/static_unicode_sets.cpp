// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#define UNISTR_FROM_STRING_EXPLICIT

#ضمن "static_unicode_sets.h"
#ضمن "umutex.h"
#ضمن "ucln_cmn.h"
#ضمن "unicode/uniset.h"
#ضمن "uresimp.h"
#ضمن "cstring.h"
#ضمن "uassert.h"

using نطاق icu;
using نطاق icu::unisets;


نطاق {

UnicodeSet* gUnicodeSets[UNISETS_مفتاح_تعداد] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
حرف gEmptyUnicodeSet[حجم(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = خطأ;

مضمن UnicodeSet* getImpl(مفتاح_2 مفتاح) {
    UnicodeSet* candidate = gUnicodeSets[مفتاح];
    إذا (candidate == nullptr) {
        إرجاع reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    إرجاع candidate;
}

UnicodeSet* computeUnion(مفتاح_2 k1, مفتاح_2 k2) {
    UnicodeSet* result = جديد UnicodeSet();
    إذا (result == nullptr) {
        إرجاع nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    إرجاع result;
}

UnicodeSet* computeUnion(مفتاح_2 k1, مفتاح_2 k2, مفتاح_2 k3) {
    UnicodeSet* result = جديد UnicodeSet();
    إذا (result == nullptr) {
        إرجاع nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    إرجاع result;
}


فراغ saveSet(مفتاح_2 مفتاح, ثابت UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[مفتاح] = جديد UnicodeSet(unicodeSetPattern, status);
}

صنف ParseDataSink : عام ResourceSink {
  عام:
    فراغ put(ثابت حرف* مفتاح, Resource_قيمة& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        إذا (U_FAILURE(status)) { إرجاع; }
        لكل (صحيح i = 0; contextsTable.get_مفتاح_And_قيمة(i, مفتاح, value); i++) {
            إذا (uprv_strcmp(مفتاح, "date") == 0) {
                // ignore
            } وإلا {
                ResourceTable strictnessTable = value.getTable(status);
                إذا (U_FAILURE(status)) { إرجاع; }
                لكل (صحيح j = 0; strictnessTable.get_مفتاح_And_قيمة(j, مفتاح, value); j++) {
                    منطقي isLenient = (uprv_strcmp(مفتاح, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    إذا (U_FAILURE(status)) { إرجاع; }
                    لكل (صحيح k = 0; k < array.getSize(); k++) {
                        array.get_قيمة(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        إذا (U_FAILURE(status)) { إرجاع; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        إذا (str.indexOf(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } وإلا إذا (str.indexOf(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } وإلا إذا (str.indexOf(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } وإلا إذا (str.indexOf(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } وإلا إذا (str.indexOf(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } وإلا إذا (str.indexOf(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } وإلا إذا (str.indexOf(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } وإلا إذا (str.indexOf(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } وإلا إذا (str.indexOf(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } وإلا إذا (str.indexOf(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } وإلا إذا (str.indexOf(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } وإلا إذا (str.indexOf(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } وإلا {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(خطأ);
                        }
                        إذا (U_FAILURE(status)) { إرجاع; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_العدد_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_العدد_Parse_Uni_Sets() {
    إذا (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = خطأ;
    }
    لكل (int32_t i = 0; i < UNISETS_مفتاح_تعداد; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_العدد_Parse_Uni_Sets_Init_Once.reset();
    إرجاع true;
}

فراغ U_CALLCONV init_العدد_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_العدد_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    جديد(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = true;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = جديد UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = جديد UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    إذا (U_FAILURE(status)) { إرجاع; }
    ParseDataSink sink;
    ures_get_All_عناصر_With_Fallback(rb.getAlias(), "parse", sink, status);
    إذا (U_FAILURE(status)) { إرجاع; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(جديد UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    إذا (U_FAILURE(status)) { إرجاع; }
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
    gUnicodeSets[INFINITY_SIGN] = جديد UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = جديد UnicodeSet(u"[∼~≈≃約]", status);
    إذا (U_FAILURE(status)) { إرجاع; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = جديد UnicodeSet(u"[:digit:]", status);
    إذا (U_FAILURE(status)) { إرجاع; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    لكل (auto* uniset : gUnicodeSets) {
        إذا (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

ثابت UnicodeSet* unisets::get(مفتاح_2 مفتاح) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_العدد_Parse_Uni_Sets_Init_Once, &init_العدد_Parse_Uni_Sets, localStatus);
    إذا (U_FAILURE(localStatus)) {
        إرجاع reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    إرجاع getImpl(مفتاح);
}

مفتاح_2 unisets::chooseFrom(UnicodeString str, مفتاح_2 key1) {
    إرجاع get(key1)->contains(str) ? key1 : NONE;
}

مفتاح_2 unisets::chooseFrom(UnicodeString str, مفتاح_2 key1, مفتاح_2 key2) {
    إرجاع get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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
