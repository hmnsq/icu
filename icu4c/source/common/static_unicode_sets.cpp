// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#وصف_ڪريو UNISTR_FROM_STRING_EXPLICIT

#ضمن "static_unicode_sets.h"
#ضمن "umutex.h"
#ضمن "ucln_cmn.h"
#ضمن "unicode/uniset.h"
#ضمن "uresimp.h"
#ضمن "cstring.h"
#ضمن "uassert.h"

استعمال نطاق icu;
استعمال نطاق icu::unisets;


نطاق {

Unicode_سيٽ* gUnicodeSets[UNISETS_ڪنجي_ڳڻپ] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(Unicode_سيٽ)
حرف g_خالي_جايون_Unicode_سيٽ[حجم(Unicode_سيٽ)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool g_خالي_جايون_Unicode_سيٽ_Initialized = خطأ;

مضمن Unicode_سيٽ* getImpl(ڪنجي_2 ڪنجي) {
    Unicode_سيٽ* candidate = gUnicodeSets[ڪنجي];
    إذا (candidate == خالي_اشارو) {
        أرجع reinterpret_cast<Unicode_سيٽ*>(g_خالي_جايون_Unicode_سيٽ);
    }
    أرجع candidate;
}

Unicode_سيٽ* computeUnion(ڪنجي_2 k1, ڪنجي_2 k2) {
    Unicode_سيٽ* نتيجو = جديد Unicode_سيٽ();
    إذا (نتيجو == خالي_اشارو) {
        أرجع خالي_اشارو;
    }
    نتيجو->جوڙيو_All(*getImpl(k1));
    نتيجو->جوڙيو_All(*getImpl(k2));
    نتيجو->freeze();
    أرجع نتيجو;
}

Unicode_سيٽ* computeUnion(ڪنجي_2 k1, ڪنجي_2 k2, ڪنجي_2 k3) {
    Unicode_سيٽ* نتيجو = جديد Unicode_سيٽ();
    إذا (نتيجو == خالي_اشارو) {
        أرجع خالي_اشارو;
    }
    نتيجو->جوڙيو_All(*getImpl(k1));
    نتيجو->جوڙيو_All(*getImpl(k2));
    نتيجو->جوڙيو_All(*getImpl(k3));
    نتيجو->freeze();
    أرجع نتيجو;
}


فراغ save_سيٽ(ڪنجي_2 ڪنجي, ثابت UnicodeString& unicode_سيٽ_نمونو, UError_ڪوڊ& حالت) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[ڪنجي] = جديد Unicode_سيٽ(unicode_سيٽ_نمونو, حالت);
}

صنف Parse_معلومات_Sink : عام ResourceSink {
  عام:
    فراغ put(ثابت حرف* ڪنجي, Resource_قدر& value, UBool /*noFallback*/, UError_ڪوڊ& حالت) override {
        Resource_جدول contexts_جدول = value.get_جدول(حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
        لكل (صحيح i = 0; contexts_جدول.get_ڪنجي_And_قدر(i, ڪنجي, value); i++) {
            إذا (uprv_strcmp(ڪنجي, "date") == 0) {
                // ignore
            } وإلا {
                Resource_جدول strictness_جدول = value.get_جدول(حالت);
                إذا (U_FAILURE(حالت)) { أرجع; }
                لكل (صحيح j = 0; strictness_جدول.get_ڪنجي_And_قدر(j, ڪنجي, value); j++) {
                    منطقي isLenient = (uprv_strcmp(ڪنجي, "lenient") == 0);
                    ResourceArray array = value.getArray(حالت);
                    إذا (U_FAILURE(حالت)) { أرجع; }
                    لكل (صحيح k = 0; k < array.get_سائيز(); k++) {
                        array.get_قدر(k, value);
                        UnicodeString str = value.getUnicodeString(حالت);
                        إذا (U_FAILURE(حالت)) { أرجع; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        إذا (str.اشاريو_جو(u'.') != -1) {
                            save_سيٽ(isLenient ? PERIOD : STRICT_PERIOD, str, حالت);
                        } وإلا إذا (str.اشاريو_جو(u',') != -1) {
                            save_سيٽ(isLenient ? COMMA : STRICT_COMMA, str, حالت);
                        } وإلا إذا (str.اشاريو_جو(u'+') != -1) {
                            save_سيٽ(PLUS_SIGN, str, حالت);
                        } وإلا إذا (str.اشاريو_جو(u'-') != -1) {
                            save_سيٽ(MINUS_SIGN, str, حالت);
                        } وإلا إذا (str.اشاريو_جو(u'$') != -1) {
                            save_سيٽ(DOLLAR_SIGN, str, حالت);
                        } وإلا إذا (str.اشاريو_جو(u'£') != -1) {
                            save_سيٽ(POUND_SIGN, str, حالت);
                        } وإلا إذا (str.اشاريو_جو(u'₹') != -1) {
                            save_سيٽ(RUPEE_SIGN, str, حالت);
                        } وإلا إذا (str.اشاريو_جو(u'¥') != -1) {
                            save_سيٽ(YEN_SIGN, str, حالت);
                        } وإلا إذا (str.اشاريو_جو(u'₩') != -1) {
                            save_سيٽ(WON_SIGN, str, حالت);
                        } وإلا إذا (str.اشاريو_جو(u'%') != -1) {
                            save_سيٽ(PERCENT_SIGN, str, حالت);
                        } وإلا إذا (str.اشاريو_جو(u'‰') != -1) {
                            save_سيٽ(PERMILLE_SIGN, str, حالت);
                        } وإلا إذا (str.اشاريو_جو(u'’') != -1) {
                            save_سيٽ(APOSTROPHE_SIGN, str, حالت);
                        } وإلا {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(خطأ);
                        }
                        إذا (U_FAILURE(حالت)) { أرجع; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_عدد_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_عدد_Parse_Uni_Sets() {
    إذا (g_خالي_جايون_Unicode_سيٽ_Initialized) {
        reinterpret_cast<Unicode_سيٽ*>(g_خالي_جايون_Unicode_سيٽ)->~Unicode_سيٽ();
        g_خالي_جايون_Unicode_سيٽ_Initialized = خطأ;
    }
    لكل (int32_t i = 0; i < UNISETS_ڪنجي_ڳڻپ; i++) {
        ميٽيو gUnicodeSets[i];
        gUnicodeSets[i] = خالي_اشارو;
    }
    g_عدد_Parse_Uni_Sets_Init_Once.reset();
    أرجع سچ;
}

فراغ U_CALLCONV init_عدد_Parse_Uni_Sets(UError_ڪوڊ& حالت) {
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_عدد_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    جديد(g_خالي_جايون_Unicode_سيٽ) Unicode_سيٽ();
    reinterpret_cast<Unicode_سيٽ*>(g_خالي_جايون_Unicode_سيٽ)->freeze();
    g_خالي_جايون_Unicode_سيٽ_Initialized = سچ;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = جديد Unicode_سيٽ(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", حالت);
    gUnicodeSets[STRICT_IGNORABLES] = جديد Unicode_سيٽ(u"[[:Bidi_Control:]]", حالت);

    Local_UResource_Bundle_اشارو rb(ures_open(خالي_اشارو, "root", &حالت));
    إذا (U_FAILURE(حالت)) { أرجع; }
    Parse_معلومات_Sink sink;
    ures_get_All_شيون_With_Fallback(rb.getAlias(), "parse", sink, حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != خالي_اشارو);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != خالي_اشارو);
    U_ASSERT(gUnicodeSets[PERIOD] != خالي_اشارو);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != خالي_اشارو);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != خالي_اشارو);

    Local_اشارو<Unicode_سيٽ> otherGrouping(جديد Unicode_سيٽ(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        حالت
    ), حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }
    otherGrouping->جوڙيو_All(*gUnicodeSets[APOSTROPHE_SIGN]);
    gUnicodeSets[OTHER_GROUPING_SEPARATORS] = otherGrouping.orphan();
    gUnicodeSets[ALL_SEPARATORS] = computeUnion(COMMA, PERIOD, OTHER_GROUPING_SEPARATORS);
    gUnicodeSets[STRICT_ALL_SEPARATORS] = computeUnion(
            STRICT_COMMA, STRICT_PERIOD, OTHER_GROUPING_SEPARATORS);

    U_ASSERT(gUnicodeSets[MINUS_SIGN] != خالي_اشارو);
    U_ASSERT(gUnicodeSets[PLUS_SIGN] != خالي_اشارو);
    U_ASSERT(gUnicodeSets[PERCENT_SIGN] != خالي_اشارو);
    U_ASSERT(gUnicodeSets[PERMILLE_SIGN] != خالي_اشارو);

    // The following don't currently have parseLenients in data.
    U_ASSERT(gUnicodeSets[INFINITY_SIGN] == خالي_اشارو);
    gUnicodeSets[INFINITY_SIGN] = جديد Unicode_سيٽ(u"[∞]", حالت);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == خالي_اشارو);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = جديد Unicode_سيٽ(u"[∼~≈≃約]", حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != خالي_اشارو);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != خالي_اشارو);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != خالي_اشارو);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != خالي_اشارو);
    U_ASSERT(gUnicodeSets[WON_SIGN] != خالي_اشارو);

    gUnicodeSets[DIGITS] = جديد Unicode_سيٽ(u"[:digit:]", حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    لكل (خودڪار* uniset : gUnicodeSets) {
        إذا (uniset != خالي_اشارو) {
            uniset->freeze();
        }
    }
}

}

ثابت Unicode_سيٽ* unisets::get(ڪنجي_2 ڪنجي) {
    UError_ڪوڊ local_حالت = U_ZERO_ERROR;
    umtx_initOnce(g_عدد_Parse_Uni_Sets_Init_Once, &init_عدد_Parse_Uni_Sets, local_حالت);
    إذا (U_FAILURE(local_حالت)) {
        أرجع reinterpret_cast<Unicode_سيٽ*>(g_خالي_جايون_Unicode_سيٽ);
    }
    أرجع getImpl(ڪنجي);
}

ڪنجي_2 unisets::چونڊيو_From(UnicodeString str, ڪنجي_2 key1) {
    أرجع get(key1)->شامل_آهي(str) ? key1 : NONE;
}

ڪنجي_2 unisets::چونڊيو_From(UnicodeString str, ڪنجي_2 key1, ڪنجي_2 key2) {
    أرجع get(key1)->شامل_آهي(str) ? key1 : چونڊيو_From(str, key2);
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


#جيڪڏهن_پورو /* #if !UCONFIG_NO_FORMATTING */
