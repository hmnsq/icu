// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#define UNISTR_FROM_STRING_EXPLICIT

#ଅନ୍ତର୍ଭାଵ "static_unicode_sets.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

using ନାମାକାଶ icu;
using ନାମାକାଶ icu::unisets;


ନାମାକାଶ {

UnicodeSet* gUnicodeSets[UNISETS_ଚାବି_ଗଣନା] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
ଅକ୍ଷର gEmptyUnicodeSet[ଆକାର(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = ମିଥ୍ୟା;

ଧାଡ଼ିଭିତରେ UnicodeSet* getImpl(ଚାବି_2 ଚାବି) {
    UnicodeSet* candidate = gUnicodeSets[ଚାବି];
    ଯଦି (candidate == nullptr) {
        ଫେରନ୍ତୁ reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    ଫେରନ୍ତୁ candidate;
}

UnicodeSet* computeUnion(ଚାବି_2 k1, ଚାବି_2 k2) {
    UnicodeSet* result = ନୂତନ UnicodeSet();
    ଯଦି (result == nullptr) {
        ଫେରନ୍ତୁ nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    ଫେରନ୍ତୁ result;
}

UnicodeSet* computeUnion(ଚାବି_2 k1, ଚାବି_2 k2, ଚାବି_2 k3) {
    UnicodeSet* result = ନୂତନ UnicodeSet();
    ଯଦି (result == nullptr) {
        ଫେରନ୍ତୁ nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    ଫେରନ୍ତୁ result;
}


ଶୂନ୍ୟ saveSet(ଚାବି_2 ଚାବି, ଅଚଳ UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[ଚାବି] = ନୂତନ UnicodeSet(unicodeSetPattern, status);
}

ଶ୍ରେଣୀ ParseDataSink : ସର୍ବସାଧାରଣ ResourceSink {
  ସର୍ବସାଧାରଣ:
    ଶୂନ୍ୟ put(ଅଚଳ ଅକ୍ଷର* ଚାବି, Resource_ମୂଲ୍ୟ& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; contextsTable.get_ଚାବି_And_ମୂଲ୍ୟ(i, ଚାବି, value); i++) {
            ଯଦି (uprv_strcmp(ଚାବି, "date") == 0) {
                // ignore
            } ଅନ୍ୟଥା {
                ResourceTable strictnessTable = value.getTable(status);
                ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
                ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; strictnessTable.get_ଚାବି_And_ମୂଲ୍ୟ(j, ଚାବି, value); j++) {
                    ତାର୍କିକ isLenient = (uprv_strcmp(ଚାବି, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
                    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < array.getSize(); k++) {
                        array.get_ମୂଲ୍ୟ(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        ଯଦି (str.indexOf(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } ଅନ୍ୟଥା ଯଦି (str.indexOf(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } ଅନ୍ୟଥା ଯଦି (str.indexOf(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } ଅନ୍ୟଥା ଯଦି (str.indexOf(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } ଅନ୍ୟଥା ଯଦି (str.indexOf(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } ଅନ୍ୟଥା ଯଦି (str.indexOf(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } ଅନ୍ୟଥା ଯଦି (str.indexOf(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } ଅନ୍ୟଥା ଯଦି (str.indexOf(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } ଅନ୍ୟଥା ଯଦି (str.indexOf(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } ଅନ୍ୟଥା ଯଦି (str.indexOf(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } ଅନ୍ୟଥା ଯଦି (str.indexOf(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } ଅନ୍ୟଥା ଯଦି (str.indexOf(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } ଅନ୍ୟଥା {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(ମିଥ୍ୟା);
                        }
                        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_ସଂଖ୍ୟା_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_ସଂଖ୍ୟା_Parse_Uni_Sets() {
    ଯଦି (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = ମିଥ୍ୟା;
    }
    ପାଇଁ (int32_t i = 0; i < UNISETS_ଚାବି_ଗଣନା; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_ସଂଖ୍ୟା_Parse_Uni_Sets_Init_Once.reset();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ U_CALLCONV init_ସଂଖ୍ୟା_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_ସଂଖ୍ୟା_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    ନୂତନ(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = ସତ୍ୟ;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = ନୂତନ UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = ନୂତନ UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
    ParseDataSink sink;
    ures_get_All_ବସ୍ତୁଗୁଡ଼ିକ_With_Fallback(rb.getAlias(), "parse", sink, status);
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(ନୂତନ UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
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
    gUnicodeSets[INFINITY_SIGN] = ନୂତନ UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = ନୂତନ UnicodeSet(u"[∼~≈≃約]", status);
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = ନୂତନ UnicodeSet(u"[:digit:]", status);
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    ପାଇଁ (auto* uniset : gUnicodeSets) {
        ଯଦି (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

ଅଚଳ UnicodeSet* unisets::get(ଚାବି_2 ଚାବି) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_ସଂଖ୍ୟା_Parse_Uni_Sets_Init_Once, &init_ସଂଖ୍ୟା_Parse_Uni_Sets, localStatus);
    ଯଦି (U_FAILURE(localStatus)) {
        ଫେରନ୍ତୁ reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    ଫେରନ୍ତୁ getImpl(ଚାବି);
}

ଚାବି_2 unisets::chooseFrom(UnicodeString str, ଚାବି_2 key1) {
    ଫେରନ୍ତୁ get(key1)->contains(str) ? key1 : NONE;
}

ଚାବି_2 unisets::chooseFrom(UnicodeString str, ଚାବି_2 key1, ଚାବି_2 key2) {
    ଫେରନ୍ତୁ get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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
