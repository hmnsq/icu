// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNISTR_FROM_STRING_EXPLICIT

#ਅਨ੍ਤਰ੍ਭਾਵ "static_unicode_sets.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ icu;
ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ icu::unisets;


ਨਾਮਾਕਾਸ਼ {

Unicode_ਸੈੱਟ* gUnicodeSets[UNISETS_ਕੁੰਜੀ_ਗਿਣਤੀ] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(Unicode_ਸੈੱਟ)
ਅੱਖਰ g_ਖਾਲੀ_ਥਾਵਾਂ_Unicode_ਸੈੱਟ[ਆਕਾਰ(Unicode_ਸੈੱਟ)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool g_ਖਾਲੀ_ਥਾਵਾਂ_Unicode_ਸੈੱਟ_Initialized = ਝੂਠ;

ਸਤਰਅੰਦਰ Unicode_ਸੈੱਟ* getImpl(ਕੁੰਜੀ_2 ਕੁੰਜੀ) {
    Unicode_ਸੈੱਟ* candidate = gUnicodeSets[ਕੁੰਜੀ];
    ਜੇ (candidate == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ reinterpret_cast<Unicode_ਸੈੱਟ*>(g_ਖਾਲੀ_ਥਾਵਾਂ_Unicode_ਸੈੱਟ);
    }
    ਵਾਪਸ candidate;
}

Unicode_ਸੈੱਟ* computeUnion(ਕੁੰਜੀ_2 k1, ਕੁੰਜੀ_2 k2) {
    Unicode_ਸੈੱਟ* ਨਤੀਜਾ = ਨਵਾਂ Unicode_ਸੈੱਟ();
    ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਨਤੀਜਾ->ਜੋੜੋ_All(*getImpl(k1));
    ਨਤੀਜਾ->ਜੋੜੋ_All(*getImpl(k2));
    ਨਤੀਜਾ->freeze();
    ਵਾਪਸ ਨਤੀਜਾ;
}

Unicode_ਸੈੱਟ* computeUnion(ਕੁੰਜੀ_2 k1, ਕੁੰਜੀ_2 k2, ਕੁੰਜੀ_2 k3) {
    Unicode_ਸੈੱਟ* ਨਤੀਜਾ = ਨਵਾਂ Unicode_ਸੈੱਟ();
    ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਨਤੀਜਾ->ਜੋੜੋ_All(*getImpl(k1));
    ਨਤੀਜਾ->ਜੋੜੋ_All(*getImpl(k2));
    ਨਤੀਜਾ->ਜੋੜੋ_All(*getImpl(k3));
    ਨਤੀਜਾ->freeze();
    ਵਾਪਸ ਨਤੀਜਾ;
}


ਖਾਲੀ save_ਸੈੱਟ(ਕੁੰਜੀ_2 ਕੁੰਜੀ, ਅਚਲ UnicodeString& unicode_ਸੈੱਟ_ਨਮੂਨਾ, UError_ਕੋਡ& ਸਥਿਤੀ) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[ਕੁੰਜੀ] = ਨਵਾਂ Unicode_ਸੈੱਟ(unicode_ਸੈੱਟ_ਨਮੂਨਾ, ਸਥਿਤੀ);
}

ਵਰਗ Parse_ਅੰਕੜੇ_Sink : ਜਨਤਕ ResourceSink {
  ਜਨਤਕ:
    ਖਾਲੀ put(ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ, Resource_ਮੁੱਲ& value, UBool /*noFallback*/, UError_ਕੋਡ& ਸਥਿਤੀ) override {
        Resource_ਸਾਰਣੀ contexts_ਸਾਰਣੀ = value.get_ਸਾਰਣੀ(ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
        ਲਈ (ਪੂਰਨ i = 0; contexts_ਸਾਰਣੀ.get_ਕੁੰਜੀ_And_ਮੁੱਲ(i, ਕੁੰਜੀ, value); i++) {
            ਜੇ (uprv_strcmp(ਕੁੰਜੀ, "date") == 0) {
                // ignore
            } ਨਹੀਂਤਾਂ {
                Resource_ਸਾਰਣੀ strictness_ਸਾਰਣੀ = value.get_ਸਾਰਣੀ(ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
                ਲਈ (ਪੂਰਨ j = 0; strictness_ਸਾਰਣੀ.get_ਕੁੰਜੀ_And_ਮੁੱਲ(j, ਕੁੰਜੀ, value); j++) {
                    ਤਾਰਕਿਕ isLenient = (uprv_strcmp(ਕੁੰਜੀ, "lenient") == 0);
                    ResourceArray array = value.getArray(ਸਥਿਤੀ);
                    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
                    ਲਈ (ਪੂਰਨ k = 0; k < array.get_ਮਾਪ(); k++) {
                        array.get_ਮੁੱਲ(k, value);
                        UnicodeString str = value.getUnicodeString(ਸਥਿਤੀ);
                        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u'.') != -1) {
                            save_ਸੈੱਟ(isLenient ? PERIOD : STRICT_PERIOD, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u',') != -1) {
                            save_ਸੈੱਟ(isLenient ? COMMA : STRICT_COMMA, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u'+') != -1) {
                            save_ਸੈੱਟ(PLUS_SIGN, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u'-') != -1) {
                            save_ਸੈੱਟ(MINUS_SIGN, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u'$') != -1) {
                            save_ਸੈੱਟ(DOLLAR_SIGN, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u'£') != -1) {
                            save_ਸੈੱਟ(POUND_SIGN, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u'₹') != -1) {
                            save_ਸੈੱਟ(RUPEE_SIGN, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u'¥') != -1) {
                            save_ਸੈੱਟ(YEN_SIGN, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u'₩') != -1) {
                            save_ਸੈੱਟ(WON_SIGN, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u'%') != -1) {
                            save_ਸੈੱਟ(PERCENT_SIGN, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u'‰') != -1) {
                            save_ਸੈੱਟ(PERMILLE_SIGN, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ ਜੇ (str.ਸੂਚਕਾਂਕ_ਦਾ(u'’') != -1) {
                            save_ਸੈੱਟ(APOSTROPHE_SIGN, str, ਸਥਿਤੀ);
                        } ਨਹੀਂਤਾਂ {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(ਝੂਠ);
                        }
                        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_ਸੰਖਿਆ_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_ਸੰਖਿਆ_Parse_Uni_Sets() {
    ਜੇ (g_ਖਾਲੀ_ਥਾਵਾਂ_Unicode_ਸੈੱਟ_Initialized) {
        reinterpret_cast<Unicode_ਸੈੱਟ*>(g_ਖਾਲੀ_ਥਾਵਾਂ_Unicode_ਸੈੱਟ)->~Unicode_ਸੈੱਟ();
        g_ਖਾਲੀ_ਥਾਵਾਂ_Unicode_ਸੈੱਟ_Initialized = ਝੂਠ;
    }
    ਲਈ (int32_t i = 0; i < UNISETS_ਕੁੰਜੀ_ਗਿਣਤੀ; i++) {
        ਮਿਟਾਓ gUnicodeSets[i];
        gUnicodeSets[i] = ਖਾਲੀ_ਸੂਚਕ;
    }
    g_ਸੰਖਿਆ_Parse_Uni_Sets_Init_Once.reset();
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ U_CALLCONV init_ਸੰਖਿਆ_Parse_Uni_Sets(UError_ਕੋਡ& ਸਥਿਤੀ) {
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_ਸੰਖਿਆ_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    ਨਵਾਂ(g_ਖਾਲੀ_ਥਾਵਾਂ_Unicode_ਸੈੱਟ) Unicode_ਸੈੱਟ();
    reinterpret_cast<Unicode_ਸੈੱਟ*>(g_ਖਾਲੀ_ਥਾਵਾਂ_Unicode_ਸੈੱਟ)->freeze();
    g_ਖਾਲੀ_ਥਾਵਾਂ_Unicode_ਸੈੱਟ_Initialized = ਸੱਚ;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = ਨਵਾਂ Unicode_ਸੈੱਟ(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", ਸਥਿਤੀ);
    gUnicodeSets[STRICT_IGNORABLES] = ਨਵਾਂ Unicode_ਸੈੱਟ(u"[[:Bidi_Control:]]", ਸਥਿਤੀ);

    Local_UResource_Bundle_ਸੂਚਕ rb(ures_open(ਖਾਲੀ_ਸੂਚਕ, "root", &ਸਥਿਤੀ));
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
    Parse_ਅੰਕੜੇ_Sink sink;
    ures_get_All_ਚੀਜ਼ਾਂ_With_Fallback(rb.getAlias(), "parse", sink, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(gUnicodeSets[PERIOD] != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != ਖਾਲੀ_ਸੂਚਕ);

    Local_ਸੂਚਕ<Unicode_ਸੈੱਟ> otherGrouping(ਨਵਾਂ Unicode_ਸੈੱਟ(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        ਸਥਿਤੀ
    ), ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
    otherGrouping->ਜੋੜੋ_All(*gUnicodeSets[APOSTROPHE_SIGN]);
    gUnicodeSets[OTHER_GROUPING_SEPARATORS] = otherGrouping.orphan();
    gUnicodeSets[ALL_SEPARATORS] = computeUnion(COMMA, PERIOD, OTHER_GROUPING_SEPARATORS);
    gUnicodeSets[STRICT_ALL_SEPARATORS] = computeUnion(
            STRICT_COMMA, STRICT_PERIOD, OTHER_GROUPING_SEPARATORS);

    U_ASSERT(gUnicodeSets[MINUS_SIGN] != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(gUnicodeSets[PLUS_SIGN] != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(gUnicodeSets[PERCENT_SIGN] != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(gUnicodeSets[PERMILLE_SIGN] != ਖਾਲੀ_ਸੂਚਕ);

    // The following don't currently have parseLenients in data.
    U_ASSERT(gUnicodeSets[INFINITY_SIGN] == ਖਾਲੀ_ਸੂਚਕ);
    gUnicodeSets[INFINITY_SIGN] = ਨਵਾਂ Unicode_ਸੈੱਟ(u"[∞]", ਸਥਿਤੀ);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == ਖਾਲੀ_ਸੂਚਕ);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = ਨਵਾਂ Unicode_ਸੈੱਟ(u"[∼~≈≃約]", ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(gUnicodeSets[WON_SIGN] != ਖਾਲੀ_ਸੂਚਕ);

    gUnicodeSets[DIGITS] = ਨਵਾਂ Unicode_ਸੈੱਟ(u"[:digit:]", ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    ਲਈ (ਸਵੈਚਾਲਿਤ* uniset : gUnicodeSets) {
        ਜੇ (uniset != ਖਾਲੀ_ਸੂਚਕ) {
            uniset->freeze();
        }
    }
}

}

ਅਚਲ Unicode_ਸੈੱਟ* unisets::get(ਕੁੰਜੀ_2 ਕੁੰਜੀ) {
    UError_ਕੋਡ local_ਸਥਿਤੀ = U_ZERO_ERROR;
    umtx_initOnce(g_ਸੰਖਿਆ_Parse_Uni_Sets_Init_Once, &init_ਸੰਖਿਆ_Parse_Uni_Sets, local_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(local_ਸਥਿਤੀ)) {
        ਵਾਪਸ reinterpret_cast<Unicode_ਸੈੱਟ*>(g_ਖਾਲੀ_ਥਾਵਾਂ_Unicode_ਸੈੱਟ);
    }
    ਵਾਪਸ getImpl(ਕੁੰਜੀ);
}

ਕੁੰਜੀ_2 unisets::ਚੁਣੋ_From(UnicodeString str, ਕੁੰਜੀ_2 key1) {
    ਵਾਪਸ get(key1)->ਸ਼ਾਮਲ_ਹੈ(str) ? key1 : NONE;
}

ਕੁੰਜੀ_2 unisets::ਚੁਣੋ_From(UnicodeString str, ਕੁੰਜੀ_2 key1, ਕੁੰਜੀ_2 key2) {
    ਵਾਪਸ get(key1)->ਸ਼ਾਮਲ_ਹੈ(str) ? key1 : ਚੁਣੋ_From(str, key2);
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


#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_FORMATTING */
