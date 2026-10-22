// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#define UNISTR_FROM_STRING_EXPLICIT

#அந்தர்பாவ "static_unicode_sets.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "uassert.h"

using நாமாகாஶ icu;
using நாமாகாஶ icu::unisets;


நாமாகாஶ {

UnicodeSet* gUnicodeSets[UNISETS_சாவி_மொத்தம்] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
எழுத்து gEmptyUnicodeSet[அளவு(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = பொய்;

உள்வரி UnicodeSet* getImpl(சாவி_2 சாவி) {
    UnicodeSet* candidate = gUnicodeSets[சாவி];
    எனில் (candidate == nullptr) {
        திரும்பு reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    திரும்பு candidate;
}

UnicodeSet* computeUnion(சாவி_2 k1, சாவி_2 k2) {
    UnicodeSet* result = புதிய UnicodeSet();
    எனில் (result == nullptr) {
        திரும்பு nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    திரும்பு result;
}

UnicodeSet* computeUnion(சாவி_2 k1, சாவி_2 k2, சாவி_2 k3) {
    UnicodeSet* result = புதிய UnicodeSet();
    எனில் (result == nullptr) {
        திரும்பு nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    திரும்பு result;
}


வெறுமை saveSet(சாவி_2 சாவி, மாறிலி UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[சாவி] = புதிய UnicodeSet(unicodeSetPattern, status);
}

வகுப்பு ParseDataSink : பொது ResourceSink {
  பொது:
    வெறுமை put(மாறிலி எழுத்து* சாவி, Resource_மதிப்பு& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        எனில் (U_FAILURE(status)) { திரும்பு; }
        க்கு (முழு i = 0; contextsTable.get_சாவி_And_மதிப்பு(i, சாவி, value); i++) {
            எனில் (uprv_strcmp(சாவி, "date") == 0) {
                // ignore
            } இல்லையெனில் {
                ResourceTable strictnessTable = value.getTable(status);
                எனில் (U_FAILURE(status)) { திரும்பு; }
                க்கு (முழு j = 0; strictnessTable.get_சாவி_And_மதிப்பு(j, சாவி, value); j++) {
                    தருக்க isLenient = (uprv_strcmp(சாவி, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    எனில் (U_FAILURE(status)) { திரும்பு; }
                    க்கு (முழு k = 0; k < array.getSize(); k++) {
                        array.get_மதிப்பு(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        எனில் (U_FAILURE(status)) { திரும்பு; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        எனில் (str.indexOf(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } இல்லையெனில் எனில் (str.indexOf(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } இல்லையெனில் எனில் (str.indexOf(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } இல்லையெனில் எனில் (str.indexOf(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } இல்லையெனில் எனில் (str.indexOf(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } இல்லையெனில் எனில் (str.indexOf(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } இல்லையெனில் எனில் (str.indexOf(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } இல்லையெனில் எனில் (str.indexOf(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } இல்லையெனில் எனில் (str.indexOf(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } இல்லையெனில் எனில் (str.indexOf(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } இல்லையெனில் எனில் (str.indexOf(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } இல்லையெனில் எனில் (str.indexOf(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } இல்லையெனில் {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(பொய்);
                        }
                        எனில் (U_FAILURE(status)) { திரும்பு; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_எண்_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_எண்_Parse_Uni_Sets() {
    எனில் (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = பொய்;
    }
    க்கு (int32_t i = 0; i < UNISETS_சாவி_மொத்தம்; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_எண்_Parse_Uni_Sets_Init_Once.reset();
    திரும்பு உண்மை;
}

வெறுமை U_CALLCONV init_எண்_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_எண்_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    புதிய(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = உண்மை;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = புதிய UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = புதிய UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    எனில் (U_FAILURE(status)) { திரும்பு; }
    ParseDataSink sink;
    ures_get_All_பொருட்கள்_With_Fallback(rb.getAlias(), "parse", sink, status);
    எனில் (U_FAILURE(status)) { திரும்பு; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(புதிய UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    எனில் (U_FAILURE(status)) { திரும்பு; }
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
    gUnicodeSets[INFINITY_SIGN] = புதிய UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = புதிய UnicodeSet(u"[∼~≈≃約]", status);
    எனில் (U_FAILURE(status)) { திரும்பு; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = புதிய UnicodeSet(u"[:digit:]", status);
    எனில் (U_FAILURE(status)) { திரும்பு; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    க்கு (auto* uniset : gUnicodeSets) {
        எனில் (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

மாறிலி UnicodeSet* unisets::get(சாவி_2 சாவி) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_எண்_Parse_Uni_Sets_Init_Once, &init_எண்_Parse_Uni_Sets, localStatus);
    எனில் (U_FAILURE(localStatus)) {
        திரும்பு reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    திரும்பு getImpl(சாவி);
}

சாவி_2 unisets::chooseFrom(UnicodeString str, சாவி_2 key1) {
    திரும்பு get(key1)->contains(str) ? key1 : NONE;
}

சாவி_2 unisets::chooseFrom(UnicodeString str, சாவி_2 key1, சாவி_2 key2) {
    திரும்பு get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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
