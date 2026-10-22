// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#ವ್ಯಾಖ್ಯಾನಿಸು UNISTR_FROM_STRING_EXPLICIT

#ಅನ್ತರ್ಭಾವ "static_unicode_sets.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

ಬಳಸಿ ನಾಮಾಕಾಶ icu;
ಬಳಸಿ ನಾಮಾಕಾಶ icu::unisets;


ನಾಮಾಕಾಶ {

Unicode_ಗಣ* gUnicodeSets[UNISETS_ಕೀಲಿ_ಎಣಿಕೆ] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(Unicode_ಗಣ)
ಅಕ್ಷರ g_ಖಾಲಿಗಳು_Unicode_ಗಣ[ಗಾತ್ರ(Unicode_ಗಣ)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool g_ಖಾಲಿಗಳು_Unicode_ಗಣ_Initialized = ಸುಳ್ಳು;

ಸಾಲೊಳಗೆ Unicode_ಗಣ* getImpl(ಕೀಲಿ_2 ಕೀಲಿ) {
    Unicode_ಗಣ* candidate = gUnicodeSets[ಕೀಲಿ];
    ಆದರೆ (candidate == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ reinterpret_cast<Unicode_ಗಣ*>(g_ಖಾಲಿಗಳು_Unicode_ಗಣ);
    }
    ಹಿಂತಿರುಗಿ candidate;
}

Unicode_ಗಣ* computeUnion(ಕೀಲಿ_2 k1, ಕೀಲಿ_2 k2) {
    Unicode_ಗಣ* ಫಲಿತಾಂಶ = ಹೊಸ Unicode_ಗಣ();
    ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಫಲಿತಾಂಶ->ಕೂಡಿಸು_All(*getImpl(k1));
    ಫಲಿತಾಂಶ->ಕೂಡಿಸು_All(*getImpl(k2));
    ಫಲಿತಾಂಶ->freeze();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

Unicode_ಗಣ* computeUnion(ಕೀಲಿ_2 k1, ಕೀಲಿ_2 k2, ಕೀಲಿ_2 k3) {
    Unicode_ಗಣ* ಫಲಿತಾಂಶ = ಹೊಸ Unicode_ಗಣ();
    ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಫಲಿತಾಂಶ->ಕೂಡಿಸು_All(*getImpl(k1));
    ಫಲಿತಾಂಶ->ಕೂಡಿಸು_All(*getImpl(k2));
    ಫಲಿತಾಂಶ->ಕೂಡಿಸು_All(*getImpl(k3));
    ಫಲಿತಾಂಶ->freeze();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


ಶೂನ್ಯ save_ಗಣ(ಕೀಲಿ_2 ಕೀಲಿ, ಸ್ಥಿರಾಂಕ UnicodeString& unicode_ಗಣ_ನಮೂನೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[ಕೀಲಿ] = ಹೊಸ Unicode_ಗಣ(unicode_ಗಣ_ನಮೂನೆ, ಸ್ಥಿತಿ_2);
}

ವರ್ಗ Parse_ದತ್ತಾಂಶ_Sink : ಸಾರ್ವಜನಿಕ ResourceSink {
  ಸಾರ್ವಜನಿಕ:
    ಶೂನ್ಯ put(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ, Resource_ಮೌಲ್ಯ& value, UBool /*noFallback*/, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) override {
        Resource_ಕೋಷ್ಟಕ contexts_ಕೋಷ್ಟಕ = value.get_ಕೋಷ್ಟಕ(ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; contexts_ಕೋಷ್ಟಕ.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(i, ಕೀಲಿ, value); i++) {
            ಆದರೆ (uprv_strcmp(ಕೀಲಿ, "date") == 0) {
                // ignore
            } ಇಲ್ಲದಿದ್ದರೆ {
                Resource_ಕೋಷ್ಟಕ strictness_ಕೋಷ್ಟಕ = value.get_ಕೋಷ್ಟಕ(ಸ್ಥಿತಿ_2);
                ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
                ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; strictness_ಕೋಷ್ಟಕ.get_ಕೀಲಿ_And_ಮೌಲ್ಯ(j, ಕೀಲಿ, value); j++) {
                    ತಾರ್ಕಿಕ isLenient = (uprv_strcmp(ಕೀಲಿ, "lenient") == 0);
                    ResourceArray array = value.getArray(ಸ್ಥಿತಿ_2);
                    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
                    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < array.get_ಗಾತ್ರ(); k++) {
                        array.get_ಮೌಲ್ಯ(k, value);
                        UnicodeString str = value.getUnicodeString(ಸ್ಥಿತಿ_2);
                        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u'.') != -1) {
                            save_ಗಣ(isLenient ? PERIOD : STRICT_PERIOD, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u',') != -1) {
                            save_ಗಣ(isLenient ? COMMA : STRICT_COMMA, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u'+') != -1) {
                            save_ಗಣ(PLUS_SIGN, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u'-') != -1) {
                            save_ಗಣ(MINUS_SIGN, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u'$') != -1) {
                            save_ಗಣ(DOLLAR_SIGN, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u'£') != -1) {
                            save_ಗಣ(POUND_SIGN, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u'₹') != -1) {
                            save_ಗಣ(RUPEE_SIGN, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u'¥') != -1) {
                            save_ಗಣ(YEN_SIGN, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u'₩') != -1) {
                            save_ಗಣ(WON_SIGN, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u'%') != -1) {
                            save_ಗಣ(PERCENT_SIGN, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u'‰') != -1) {
                            save_ಗಣ(PERMILLE_SIGN, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str.ಸೂಚ್ಯಂಕ_ದ(u'’') != -1) {
                            save_ಗಣ(APOSTROPHE_SIGN, str, ಸ್ಥಿತಿ_2);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(ಸುಳ್ಳು);
                        }
                        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_ಸಂಖ್ಯೆ_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_ಸಂಖ್ಯೆ_Parse_Uni_Sets() {
    ಆದರೆ (g_ಖಾಲಿಗಳು_Unicode_ಗಣ_Initialized) {
        reinterpret_cast<Unicode_ಗಣ*>(g_ಖಾಲಿಗಳು_Unicode_ಗಣ)->~Unicode_ಗಣ();
        g_ಖಾಲಿಗಳು_Unicode_ಗಣ_Initialized = ಸುಳ್ಳು;
    }
    ಗಾಗಿ (int32_t i = 0; i < UNISETS_ಕೀಲಿ_ಎಣಿಕೆ; i++) {
        ಅಳಿಸು gUnicodeSets[i];
        gUnicodeSets[i] = ಶೂನ್ಯಸೂಚಕ;
    }
    g_ಸಂಖ್ಯೆ_Parse_Uni_Sets_Init_Once.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ U_CALLCONV init_ಸಂಖ್ಯೆ_Parse_Uni_Sets(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_ಸಂಖ್ಯೆ_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    ಹೊಸ(g_ಖಾಲಿಗಳು_Unicode_ಗಣ) Unicode_ಗಣ();
    reinterpret_cast<Unicode_ಗಣ*>(g_ಖಾಲಿಗಳು_Unicode_ಗಣ)->freeze();
    g_ಖಾಲಿಗಳು_Unicode_ಗಣ_Initialized = ನಿಜ;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = ಹೊಸ Unicode_ಗಣ(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", ಸ್ಥಿತಿ_2);
    gUnicodeSets[STRICT_IGNORABLES] = ಹೊಸ Unicode_ಗಣ(u"[[:Bidi_Control:]]", ಸ್ಥಿತಿ_2);

    Local_UResource_Bundle_ಸೂಚಕ rb(ures_open(ಶೂನ್ಯಸೂಚಕ, "root", &ಸ್ಥಿತಿ_2));
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
    Parse_ದತ್ತಾಂಶ_Sink sink;
    ures_get_All_ವಸ್ತುಗಳು_With_Fallback(rb.getAlias(), "parse", sink, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(gUnicodeSets[PERIOD] != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != ಶೂನ್ಯಸೂಚಕ);

    Local_ಸೂಚಕ<Unicode_ಗಣ> otherGrouping(ಹೊಸ Unicode_ಗಣ(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        ಸ್ಥಿತಿ_2
    ), ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
    otherGrouping->ಕೂಡಿಸು_All(*gUnicodeSets[APOSTROPHE_SIGN]);
    gUnicodeSets[OTHER_GROUPING_SEPARATORS] = otherGrouping.orphan();
    gUnicodeSets[ALL_SEPARATORS] = computeUnion(COMMA, PERIOD, OTHER_GROUPING_SEPARATORS);
    gUnicodeSets[STRICT_ALL_SEPARATORS] = computeUnion(
            STRICT_COMMA, STRICT_PERIOD, OTHER_GROUPING_SEPARATORS);

    U_ASSERT(gUnicodeSets[MINUS_SIGN] != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(gUnicodeSets[PLUS_SIGN] != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(gUnicodeSets[PERCENT_SIGN] != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(gUnicodeSets[PERMILLE_SIGN] != ಶೂನ್ಯಸೂಚಕ);

    // The following don't currently have parseLenients in data.
    U_ASSERT(gUnicodeSets[INFINITY_SIGN] == ಶೂನ್ಯಸೂಚಕ);
    gUnicodeSets[INFINITY_SIGN] = ಹೊಸ Unicode_ಗಣ(u"[∞]", ಸ್ಥಿತಿ_2);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == ಶೂನ್ಯಸೂಚಕ);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = ಹೊಸ Unicode_ಗಣ(u"[∼~≈≃約]", ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(gUnicodeSets[WON_SIGN] != ಶೂನ್ಯಸೂಚಕ);

    gUnicodeSets[DIGITS] = ಹೊಸ Unicode_ಗಣ(u"[:digit:]", ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ* uniset : gUnicodeSets) {
        ಆದರೆ (uniset != ಶೂನ್ಯಸೂಚಕ) {
            uniset->freeze();
        }
    }
}

}

ಸ್ಥಿರಾಂಕ Unicode_ಗಣ* unisets::get(ಕೀಲಿ_2 ಕೀಲಿ) {
    UError_ಸಂಕೇತ local_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    umtx_initOnce(g_ಸಂಖ್ಯೆ_Parse_Uni_Sets_Init_Once, &init_ಸಂಖ್ಯೆ_Parse_Uni_Sets, local_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(local_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ reinterpret_cast<Unicode_ಗಣ*>(g_ಖಾಲಿಗಳು_Unicode_ಗಣ);
    }
    ಹಿಂತಿರುಗಿ getImpl(ಕೀಲಿ);
}

ಕೀಲಿ_2 unisets::ಆರಿಸು_From(UnicodeString str, ಕೀಲಿ_2 key1) {
    ಹಿಂತಿರುಗಿ get(key1)->ಒಳಗೊಂಡಿದೆ(str) ? key1 : NONE;
}

ಕೀಲಿ_2 unisets::ಆರಿಸು_From(UnicodeString str, ಕೀಲಿ_2 key1, ಕೀಲಿ_2 key2) {
    ಹಿಂತಿರುಗಿ get(key1)->ಒಳಗೊಂಡಿದೆ(str) ? key1 : ಆರಿಸು_From(str, key2);
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


#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_FORMATTING */
