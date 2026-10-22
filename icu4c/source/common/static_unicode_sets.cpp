// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#define UNISTR_FROM_STRING_EXPLICIT

#അന്തര്ഭാവ "static_unicode_sets.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "uassert.h"

using നാമാകാശ icu;
using നാമാകാശ icu::unisets;


നാമാകാശ {

UnicodeSet* gUnicodeSets[UNISETS_താക്കോൽ_എണ്ണം] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
അക്ഷരം gEmptyUnicodeSet[വലുപ്പം(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = അസത്യം;

വരിയിൽ UnicodeSet* getImpl(താക്കോൽ_2 താക്കോൽ) {
    UnicodeSet* candidate = gUnicodeSets[താക്കോൽ];
    എങ്കിൽ (candidate == nullptr) {
        തിരികെ reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    തിരികെ candidate;
}

UnicodeSet* computeUnion(താക്കോൽ_2 k1, താക്കോൽ_2 k2) {
    UnicodeSet* result = പുതിയ UnicodeSet();
    എങ്കിൽ (result == nullptr) {
        തിരികെ nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    തിരികെ result;
}

UnicodeSet* computeUnion(താക്കോൽ_2 k1, താക്കോൽ_2 k2, താക്കോൽ_2 k3) {
    UnicodeSet* result = പുതിയ UnicodeSet();
    എങ്കിൽ (result == nullptr) {
        തിരികെ nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    തിരികെ result;
}


ശൂന്യം saveSet(താക്കോൽ_2 താക്കോൽ, സ്ഥിരം UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[താക്കോൽ] = പുതിയ UnicodeSet(unicodeSetPattern, status);
}

വർഗ്ഗം ParseDataSink : പൊതു ResourceSink {
  പൊതു:
    ശൂന്യം put(സ്ഥിരം അക്ഷരം* താക്കോൽ, Resource_മൂല്യം& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
        വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; contextsTable.get_താക്കോൽ_And_മൂല്യം(i, താക്കോൽ, value); i++) {
            എങ്കിൽ (uprv_strcmp(താക്കോൽ, "date") == 0) {
                // ignore
            } അല്ലെങ്കിൽ {
                ResourceTable strictnessTable = value.getTable(status);
                എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
                വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; strictnessTable.get_താക്കോൽ_And_മൂല്യം(j, താക്കോൽ, value); j++) {
                    തർക്കം isLenient = (uprv_strcmp(താക്കോൽ, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
                    വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < array.getSize(); k++) {
                        array.get_മൂല്യം(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        എങ്കിൽ (str.indexOf(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } അല്ലെങ്കിൽ എങ്കിൽ (str.indexOf(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } അല്ലെങ്കിൽ എങ്കിൽ (str.indexOf(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } അല്ലെങ്കിൽ എങ്കിൽ (str.indexOf(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } അല്ലെങ്കിൽ എങ്കിൽ (str.indexOf(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } അല്ലെങ്കിൽ എങ്കിൽ (str.indexOf(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } അല്ലെങ്കിൽ എങ്കിൽ (str.indexOf(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } അല്ലെങ്കിൽ എങ്കിൽ (str.indexOf(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } അല്ലെങ്കിൽ എങ്കിൽ (str.indexOf(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } അല്ലെങ്കിൽ എങ്കിൽ (str.indexOf(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } അല്ലെങ്കിൽ എങ്കിൽ (str.indexOf(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } അല്ലെങ്കിൽ എങ്കിൽ (str.indexOf(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } അല്ലെങ്കിൽ {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(അസത്യം);
                        }
                        എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_സംഖ്യ_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_സംഖ്യ_Parse_Uni_Sets() {
    എങ്കിൽ (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = അസത്യം;
    }
    വേണ്ടി (int32_t i = 0; i < UNISETS_താക്കോൽ_എണ്ണം; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_സംഖ്യ_Parse_Uni_Sets_Init_Once.reset();
    തിരികെ സത്യം;
}

ശൂന്യം U_CALLCONV init_സംഖ്യ_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_സംഖ്യ_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    പുതിയ(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = സത്യം;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = പുതിയ UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = പുതിയ UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
    ParseDataSink sink;
    ures_get_All_ഇനങ്ങൾ_With_Fallback(rb.getAlias(), "parse", sink, status);
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(പുതിയ UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
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
    gUnicodeSets[INFINITY_SIGN] = പുതിയ UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = പുതിയ UnicodeSet(u"[∼~≈≃約]", status);
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = പുതിയ UnicodeSet(u"[:digit:]", status);
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    വേണ്ടി (auto* uniset : gUnicodeSets) {
        എങ്കിൽ (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

സ്ഥിരം UnicodeSet* unisets::get(താക്കോൽ_2 താക്കോൽ) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_സംഖ്യ_Parse_Uni_Sets_Init_Once, &init_സംഖ്യ_Parse_Uni_Sets, localStatus);
    എങ്കിൽ (U_FAILURE(localStatus)) {
        തിരികെ reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    തിരികെ getImpl(താക്കോൽ);
}

താക്കോൽ_2 unisets::chooseFrom(UnicodeString str, താക്കോൽ_2 key1) {
    തിരികെ get(key1)->contains(str) ? key1 : NONE;
}

താക്കോൽ_2 unisets::chooseFrom(UnicodeString str, താക്കോൽ_2 key1, താക്കോൽ_2 key2) {
    തിരികെ get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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
