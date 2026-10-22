// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#define UNISTR_FROM_STRING_EXPLICIT

#شامل "static_unicode_sets.h"
#شامل "umutex.h"
#شامل "ucln_cmn.h"
#شامل "unicode/uniset.h"
#شامل "uresimp.h"
#شامل "cstring.h"
#شامل "uassert.h"

using فضاینام icu;
using فضاینام icu::unisets;


فضاینام {

UnicodeSet* gUnicodeSets[UNISETS_کلید_شمار] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
نویسه gEmptyUnicodeSet[اندازه(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = نادرست;

درونخطی UnicodeSet* getImpl(کلید_2 کلید) {
    UnicodeSet* candidate = gUnicodeSets[کلید];
    اگر (candidate == nullptr) {
        بازگشت reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    بازگشت candidate;
}

UnicodeSet* computeUnion(کلید_2 k1, کلید_2 k2) {
    UnicodeSet* result = جدید UnicodeSet();
    اگر (result == nullptr) {
        بازگشت nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    بازگشت result;
}

UnicodeSet* computeUnion(کلید_2 k1, کلید_2 k2, کلید_2 k3) {
    UnicodeSet* result = جدید UnicodeSet();
    اگر (result == nullptr) {
        بازگشت nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    بازگشت result;
}


تهی saveSet(کلید_2 کلید, ثابت UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[کلید] = جدید UnicodeSet(unicodeSetPattern, status);
}

رده ParseDataSink : عمومی ResourceSink {
  عمومی:
    تهی put(ثابت نویسه* کلید, Resource_مقدار& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        اگر (U_FAILURE(status)) { بازگشت; }
        برای (صحیح i = 0; contextsTable.get_کلید_And_مقدار(i, کلید, value); i++) {
            اگر (uprv_strcmp(کلید, "date") == 0) {
                // ignore
            } وگرنه {
                ResourceTable strictnessTable = value.getTable(status);
                اگر (U_FAILURE(status)) { بازگشت; }
                برای (صحیح j = 0; strictnessTable.get_کلید_And_مقدار(j, کلید, value); j++) {
                    منطقی isLenient = (uprv_strcmp(کلید, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    اگر (U_FAILURE(status)) { بازگشت; }
                    برای (صحیح k = 0; k < array.getSize(); k++) {
                        array.get_مقدار(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        اگر (U_FAILURE(status)) { بازگشت; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        اگر (str.indexOf(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } وگرنه اگر (str.indexOf(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } وگرنه اگر (str.indexOf(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } وگرنه اگر (str.indexOf(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } وگرنه اگر (str.indexOf(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } وگرنه اگر (str.indexOf(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } وگرنه اگر (str.indexOf(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } وگرنه اگر (str.indexOf(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } وگرنه اگر (str.indexOf(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } وگرنه اگر (str.indexOf(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } وگرنه اگر (str.indexOf(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } وگرنه اگر (str.indexOf(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } وگرنه {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(نادرست);
                        }
                        اگر (U_FAILURE(status)) { بازگشت; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_عدد_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_عدد_Parse_Uni_Sets() {
    اگر (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = نادرست;
    }
    برای (int32_t i = 0; i < UNISETS_کلید_شمار; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_عدد_Parse_Uni_Sets_Init_Once.reset();
    بازگشت درست;
}

تهی U_CALLCONV init_عدد_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_عدد_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    جدید(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = درست;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = جدید UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = جدید UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    اگر (U_FAILURE(status)) { بازگشت; }
    ParseDataSink sink;
    ures_get_All_اقلام_With_Fallback(rb.getAlias(), "parse", sink, status);
    اگر (U_FAILURE(status)) { بازگشت; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(جدید UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    اگر (U_FAILURE(status)) { بازگشت; }
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
    gUnicodeSets[INFINITY_SIGN] = جدید UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = جدید UnicodeSet(u"[∼~≈≃約]", status);
    اگر (U_FAILURE(status)) { بازگشت; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = جدید UnicodeSet(u"[:digit:]", status);
    اگر (U_FAILURE(status)) { بازگشت; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    برای (auto* uniset : gUnicodeSets) {
        اگر (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

ثابت UnicodeSet* unisets::get(کلید_2 کلید) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_عدد_Parse_Uni_Sets_Init_Once, &init_عدد_Parse_Uni_Sets, localStatus);
    اگر (U_FAILURE(localStatus)) {
        بازگشت reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    بازگشت getImpl(کلید);
}

کلید_2 unisets::chooseFrom(UnicodeString str, کلید_2 key1) {
    بازگشت get(key1)->contains(str) ? key1 : NONE;
}

کلید_2 unisets::chooseFrom(UnicodeString str, کلید_2 key1, کلید_2 key2) {
    بازگشت get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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
