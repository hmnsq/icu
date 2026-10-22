// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#define UNISTR_FROM_STRING_EXPLICIT

#અન્તર્ભાવ "static_unicode_sets.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "uassert.h"

using નામાકાશ icu;
using નામાકાશ icu::unisets;


નામાકાશ {

UnicodeSet* gUnicodeSets[UNISETS_ચાવી_ગણતરી] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
અક્ષર gEmptyUnicodeSet[કદ(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = ખોટું;

અંતઃરેખ UnicodeSet* getImpl(ચાવી_2 ચાવી) {
    UnicodeSet* candidate = gUnicodeSets[ચાવી];
    જો (candidate == nullptr) {
        પાછા reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    પાછા candidate;
}

UnicodeSet* computeUnion(ચાવી_2 k1, ચાવી_2 k2) {
    UnicodeSet* result = નવું UnicodeSet();
    જો (result == nullptr) {
        પાછા nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    પાછા result;
}

UnicodeSet* computeUnion(ચાવી_2 k1, ચાવી_2 k2, ચાવી_2 k3) {
    UnicodeSet* result = નવું UnicodeSet();
    જો (result == nullptr) {
        પાછા nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    પાછા result;
}


શૂન્ય saveSet(ચાવી_2 ચાવી, અચલ UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[ચાવી] = નવું UnicodeSet(unicodeSetPattern, status);
}

વર્ગ ParseDataSink : જાહેર ResourceSink {
  જાહેર:
    શૂન્ય put(અચલ અક્ષર* ચાવી, Resource_કિંમત& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        જો (U_FAILURE(status)) { પાછા; }
        માટે (પૂર્ણાંક i = 0; contextsTable.get_ચાવી_And_કિંમત(i, ચાવી, value); i++) {
            જો (uprv_strcmp(ચાવી, "date") == 0) {
                // ignore
            } નહીંતર {
                ResourceTable strictnessTable = value.getTable(status);
                જો (U_FAILURE(status)) { પાછા; }
                માટે (પૂર્ણાંક j = 0; strictnessTable.get_ચાવી_And_કિંમત(j, ચાવી, value); j++) {
                    તાર્કિક isLenient = (uprv_strcmp(ચાવી, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    જો (U_FAILURE(status)) { પાછા; }
                    માટે (પૂર્ણાંક k = 0; k < array.getSize(); k++) {
                        array.get_કિંમત(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        જો (U_FAILURE(status)) { પાછા; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        જો (str.indexOf(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } નહીંતર જો (str.indexOf(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } નહીંતર જો (str.indexOf(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } નહીંતર જો (str.indexOf(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } નહીંતર જો (str.indexOf(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } નહીંતર જો (str.indexOf(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } નહીંતર જો (str.indexOf(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } નહીંતર જો (str.indexOf(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } નહીંતર જો (str.indexOf(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } નહીંતર જો (str.indexOf(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } નહીંતર જો (str.indexOf(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } નહીંતર જો (str.indexOf(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } નહીંતર {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(ખોટું);
                        }
                        જો (U_FAILURE(status)) { પાછા; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_સંખ્યા_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_સંખ્યા_Parse_Uni_Sets() {
    જો (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = ખોટું;
    }
    માટે (int32_t i = 0; i < UNISETS_ચાવી_ગણતરી; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    g_સંખ્યા_Parse_Uni_Sets_Init_Once.reset();
    પાછા સાચું;
}

શૂન્ય U_CALLCONV init_સંખ્યા_Parse_Uni_Sets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_સંખ્યા_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    નવું(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = સાચું;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = નવું UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = નવું UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    જો (U_FAILURE(status)) { પાછા; }
    ParseDataSink sink;
    ures_get_All_વસ્તુઓ_With_Fallback(rb.getAlias(), "parse", sink, status);
    જો (U_FAILURE(status)) { પાછા; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(નવું UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    જો (U_FAILURE(status)) { પાછા; }
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
    gUnicodeSets[INFINITY_SIGN] = નવું UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = નવું UnicodeSet(u"[∼~≈≃約]", status);
    જો (U_FAILURE(status)) { પાછા; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = નવું UnicodeSet(u"[:digit:]", status);
    જો (U_FAILURE(status)) { પાછા; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    માટે (auto* uniset : gUnicodeSets) {
        જો (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

અચલ UnicodeSet* unisets::get(ચાવી_2 ચાવી) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(g_સંખ્યા_Parse_Uni_Sets_Init_Once, &init_સંખ્યા_Parse_Uni_Sets, localStatus);
    જો (U_FAILURE(localStatus)) {
        પાછા reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    પાછા getImpl(ચાવી);
}

ચાવી_2 unisets::chooseFrom(UnicodeString str, ચાવી_2 key1) {
    પાછા get(key1)->contains(str) ? key1 : NONE;
}

ચાવી_2 unisets::chooseFrom(UnicodeString str, ચાવી_2 key1, ચાવી_2 key2) {
    પાછા get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
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
