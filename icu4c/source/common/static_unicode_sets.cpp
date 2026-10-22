// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#నిర్వచించు UNISTR_FROM_STRING_EXPLICIT

#అన్తర్భావ "static_unicode_sets.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uassert.h"

ఉపయోగించి నామాకాశ icu;
ఉపయోగించి నామాకాశ icu::unisets;


నామాకాశ {

Unicode_సమితి* gUnicodeSets[UNISETS_కీలకం_లెక్క] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(Unicode_సమితి)
అక్షరం g_ఖాళీలు_Unicode_సమితి[పరిమాణం(Unicode_సమితి)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool g_ఖాళీలు_Unicode_సమితి_Initialized = అబద్ధం;

అంతఃవరుస Unicode_సమితి* getImpl(కీలకం_2 కీలకం) {
    Unicode_సమితి* candidate = gUnicodeSets[కీలకం];
    ఒకవేళ (candidate == శూన్యసూచిక) {
        తిరిగి reinterpret_cast<Unicode_సమితి*>(g_ఖాళీలు_Unicode_సమితి);
    }
    తిరిగి candidate;
}

Unicode_సమితి* computeUnion(కీలకం_2 k1, కీలకం_2 k2) {
    Unicode_సమితి* ఫలితం = కొత్త Unicode_సమితి();
    ఒకవేళ (ఫలితం == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }
    ఫలితం->కూడు_All(*getImpl(k1));
    ఫలితం->కూడు_All(*getImpl(k2));
    ఫలితం->freeze();
    తిరిగి ఫలితం;
}

Unicode_సమితి* computeUnion(కీలకం_2 k1, కీలకం_2 k2, కీలకం_2 k3) {
    Unicode_సమితి* ఫలితం = కొత్త Unicode_సమితి();
    ఒకవేళ (ఫలితం == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }
    ఫలితం->కూడు_All(*getImpl(k1));
    ఫలితం->కూడు_All(*getImpl(k2));
    ఫలితం->కూడు_All(*getImpl(k3));
    ఫలితం->freeze();
    తిరిగి ఫలితం;
}


శూన్య save_సమితి(కీలకం_2 కీలకం, స్థిరరాశి UnicodeString& unicode_సమితి_నమూనా, UError_సంకేతం& స్థితి_2) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[కీలకం] = కొత్త Unicode_సమితి(unicode_సమితి_నమూనా, స్థితి_2);
}

తరగతి Parse_దత్తాంశం_Sink : బహిరంగ ResourceSink {
  బహిరంగ:
    శూన్య put(స్థిరరాశి అక్షరం* కీలకం, Resource_విలువ& value, UBool /*noFallback*/, UError_సంకేతం& స్థితి_2) override {
        Resource_పట్టిక contexts_పట్టిక = value.get_పట్టిక(స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
        కొరకు (పూర్ణాంకం i = 0; contexts_పట్టిక.get_కీలకం_And_విలువ(i, కీలకం, value); i++) {
            ఒకవేళ (uprv_strcmp(కీలకం, "date") == 0) {
                // ignore
            } లేకపోతే {
                Resource_పట్టిక strictness_పట్టిక = value.get_పట్టిక(స్థితి_2);
                ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
                కొరకు (పూర్ణాంకం j = 0; strictness_పట్టిక.get_కీలకం_And_విలువ(j, కీలకం, value); j++) {
                    తార్కిక isLenient = (uprv_strcmp(కీలకం, "lenient") == 0);
                    ResourceArray array = value.getArray(స్థితి_2);
                    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
                    కొరకు (పూర్ణాంకం k = 0; k < array.get_కొలత(); k++) {
                        array.get_విలువ(k, value);
                        UnicodeString str = value.getUnicodeString(స్థితి_2);
                        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        ఒకవేళ (str.సూచీ_యొక్క(u'.') != -1) {
                            save_సమితి(isLenient ? PERIOD : STRICT_PERIOD, str, స్థితి_2);
                        } లేకపోతే ఒకవేళ (str.సూచీ_యొక్క(u',') != -1) {
                            save_సమితి(isLenient ? COMMA : STRICT_COMMA, str, స్థితి_2);
                        } లేకపోతే ఒకవేళ (str.సూచీ_యొక్క(u'+') != -1) {
                            save_సమితి(PLUS_SIGN, str, స్థితి_2);
                        } లేకపోతే ఒకవేళ (str.సూచీ_యొక్క(u'-') != -1) {
                            save_సమితి(MINUS_SIGN, str, స్థితి_2);
                        } లేకపోతే ఒకవేళ (str.సూచీ_యొక్క(u'$') != -1) {
                            save_సమితి(DOLLAR_SIGN, str, స్థితి_2);
                        } లేకపోతే ఒకవేళ (str.సూచీ_యొక్క(u'£') != -1) {
                            save_సమితి(POUND_SIGN, str, స్థితి_2);
                        } లేకపోతే ఒకవేళ (str.సూచీ_యొక్క(u'₹') != -1) {
                            save_సమితి(RUPEE_SIGN, str, స్థితి_2);
                        } లేకపోతే ఒకవేళ (str.సూచీ_యొక్క(u'¥') != -1) {
                            save_సమితి(YEN_SIGN, str, స్థితి_2);
                        } లేకపోతే ఒకవేళ (str.సూచీ_యొక్క(u'₩') != -1) {
                            save_సమితి(WON_SIGN, str, స్థితి_2);
                        } లేకపోతే ఒకవేళ (str.సూచీ_యొక్క(u'%') != -1) {
                            save_సమితి(PERCENT_SIGN, str, స్థితి_2);
                        } లేకపోతే ఒకవేళ (str.సూచీ_యొక్క(u'‰') != -1) {
                            save_సమితి(PERMILLE_SIGN, str, స్థితి_2);
                        } లేకపోతే ఒకవేళ (str.సూచీ_యొక్క(u'’') != -1) {
                            save_సమితి(APOSTROPHE_SIGN, str, స్థితి_2);
                        } లేకపోతే {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(అబద్ధం);
                        }
                        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_సంఖ్య_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_సంఖ్య_Parse_Uni_Sets() {
    ఒకవేళ (g_ఖాళీలు_Unicode_సమితి_Initialized) {
        reinterpret_cast<Unicode_సమితి*>(g_ఖాళీలు_Unicode_సమితి)->~Unicode_సమితి();
        g_ఖాళీలు_Unicode_సమితి_Initialized = అబద్ధం;
    }
    కొరకు (int32_t i = 0; i < UNISETS_కీలకం_లెక్క; i++) {
        తొలగించు gUnicodeSets[i];
        gUnicodeSets[i] = శూన్యసూచిక;
    }
    g_సంఖ్య_Parse_Uni_Sets_Init_Once.reset();
    తిరిగి నిజం;
}

శూన్య U_CALLCONV init_సంఖ్య_Parse_Uni_Sets(UError_సంకేతం& స్థితి_2) {
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_సంఖ్య_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    కొత్త(g_ఖాళీలు_Unicode_సమితి) Unicode_సమితి();
    reinterpret_cast<Unicode_సమితి*>(g_ఖాళీలు_Unicode_సమితి)->freeze();
    g_ఖాళీలు_Unicode_సమితి_Initialized = నిజం;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = కొత్త Unicode_సమితి(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", స్థితి_2);
    gUnicodeSets[STRICT_IGNORABLES] = కొత్త Unicode_సమితి(u"[[:Bidi_Control:]]", స్థితి_2);

    Local_UResource_Bundle_సూచిక rb(ures_open(శూన్యసూచిక, "root", &స్థితి_2));
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
    Parse_దత్తాంశం_Sink sink;
    ures_get_All_వస్తువులు_With_Fallback(rb.getAlias(), "parse", sink, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != శూన్యసూచిక);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != శూన్యసూచిక);
    U_ASSERT(gUnicodeSets[PERIOD] != శూన్యసూచిక);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != శూన్యసూచిక);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != శూన్యసూచిక);

    Local_సూచిక<Unicode_సమితి> otherGrouping(కొత్త Unicode_సమితి(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        స్థితి_2
    ), స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
    otherGrouping->కూడు_All(*gUnicodeSets[APOSTROPHE_SIGN]);
    gUnicodeSets[OTHER_GROUPING_SEPARATORS] = otherGrouping.orphan();
    gUnicodeSets[ALL_SEPARATORS] = computeUnion(COMMA, PERIOD, OTHER_GROUPING_SEPARATORS);
    gUnicodeSets[STRICT_ALL_SEPARATORS] = computeUnion(
            STRICT_COMMA, STRICT_PERIOD, OTHER_GROUPING_SEPARATORS);

    U_ASSERT(gUnicodeSets[MINUS_SIGN] != శూన్యసూచిక);
    U_ASSERT(gUnicodeSets[PLUS_SIGN] != శూన్యసూచిక);
    U_ASSERT(gUnicodeSets[PERCENT_SIGN] != శూన్యసూచిక);
    U_ASSERT(gUnicodeSets[PERMILLE_SIGN] != శూన్యసూచిక);

    // The following don't currently have parseLenients in data.
    U_ASSERT(gUnicodeSets[INFINITY_SIGN] == శూన్యసూచిక);
    gUnicodeSets[INFINITY_SIGN] = కొత్త Unicode_సమితి(u"[∞]", స్థితి_2);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == శూన్యసూచిక);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = కొత్త Unicode_సమితి(u"[∼~≈≃約]", స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != శూన్యసూచిక);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != శూన్యసూచిక);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != శూన్యసూచిక);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != శూన్యసూచిక);
    U_ASSERT(gUnicodeSets[WON_SIGN] != శూన్యసూచిక);

    gUnicodeSets[DIGITS] = కొత్త Unicode_సమితి(u"[:digit:]", స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    కొరకు (స్వయంచాలక* uniset : gUnicodeSets) {
        ఒకవేళ (uniset != శూన్యసూచిక) {
            uniset->freeze();
        }
    }
}

}

స్థిరరాశి Unicode_సమితి* unisets::get(కీలకం_2 కీలకం) {
    UError_సంకేతం local_స్థితి = U_ZERO_ERROR;
    umtx_initOnce(g_సంఖ్య_Parse_Uni_Sets_Init_Once, &init_సంఖ్య_Parse_Uni_Sets, local_స్థితి);
    ఒకవేళ (U_FAILURE(local_స్థితి)) {
        తిరిగి reinterpret_cast<Unicode_సమితి*>(g_ఖాళీలు_Unicode_సమితి);
    }
    తిరిగి getImpl(కీలకం);
}

కీలకం_2 unisets::ఎంచుకో_From(UnicodeString str, కీలకం_2 key1) {
    తిరిగి get(key1)->కలిగియుంది(str) ? key1 : NONE;
}

కీలకం_2 unisets::ఎంచుకో_From(UnicodeString str, కీలకం_2 key1, కీలకం_2 key2) {
    తిరిగి get(key1)->కలిగియుంది(str) ? key1 : ఎంచుకో_From(str, key2);
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


#అయితే_ముగింపు /* #if !UCONFIG_NO_FORMATTING */
