// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#تعریف_کرو UNISTR_FROM_STRING_EXPLICIT

#ضمن "static_unicode_sets.h"
#ضمن "umutex.h"
#ضمن "ucln_cmn.h"
#ضمن "unicode/uniset.h"
#ضمن "uresimp.h"
#ضمن "cstring.h"
#ضمن "uassert.h"

ورتوں نطاق icu;
ورتوں نطاق icu::unisets;


نطاق {

Unicode_سیٹ* gUnicodeSets[UNISETS_کلید_گنتی] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(Unicode_سیٹ)
حرف g_خالی_تھاواں_Unicode_سیٹ[حجم(Unicode_سیٹ)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool g_خالی_تھاواں_Unicode_سیٹ_Initialized = خطأ;

مضمن Unicode_سیٹ* getImpl(کلید_2 کلید) {
    Unicode_سیٹ* candidate = gUnicodeSets[کلید];
    إذا (candidate == خالی_سوچک) {
        أرجع reinterpret_cast<Unicode_سیٹ*>(g_خالی_تھاواں_Unicode_سیٹ);
    }
    أرجع candidate;
}

Unicode_سیٹ* computeUnion(کلید_2 k1, کلید_2 k2) {
    Unicode_سیٹ* نتیجہ = جديد Unicode_سیٹ();
    إذا (نتیجہ == خالی_سوچک) {
        أرجع خالی_سوچک;
    }
    نتیجہ->جوڑو_All(*getImpl(k1));
    نتیجہ->جوڑو_All(*getImpl(k2));
    نتیجہ->freeze();
    أرجع نتیجہ;
}

Unicode_سیٹ* computeUnion(کلید_2 k1, کلید_2 k2, کلید_2 k3) {
    Unicode_سیٹ* نتیجہ = جديد Unicode_سیٹ();
    إذا (نتیجہ == خالی_سوچک) {
        أرجع خالی_سوچک;
    }
    نتیجہ->جوڑو_All(*getImpl(k1));
    نتیجہ->جوڑو_All(*getImpl(k2));
    نتیجہ->جوڑو_All(*getImpl(k3));
    نتیجہ->freeze();
    أرجع نتیجہ;
}


فراغ save_سیٹ(کلید_2 کلید, ثابت UnicodeString& unicode_سیٹ_نمونہ, UError_کوڈ& حالت) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[کلید] = جديد Unicode_سیٹ(unicode_سیٹ_نمونہ, حالت);
}

صنف Parse_انکڑے_Sink : عام ResourceSink {
  عام:
    فراغ put(ثابت حرف* کلید, Resource_قدر& value, UBool /*noFallback*/, UError_کوڈ& حالت) override {
        Resource_جدول contexts_جدول = value.get_جدول(حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
        لكل (صحيح i = 0; contexts_جدول.get_کلید_And_قدر(i, کلید, value); i++) {
            إذا (uprv_strcmp(کلید, "date") == 0) {
                // ignore
            } وإلا {
                Resource_جدول strictness_جدول = value.get_جدول(حالت);
                إذا (U_FAILURE(حالت)) { أرجع; }
                لكل (صحيح j = 0; strictness_جدول.get_کلید_And_قدر(j, کلید, value); j++) {
                    منطقي isLenient = (uprv_strcmp(کلید, "lenient") == 0);
                    ResourceArray array = value.getArray(حالت);
                    إذا (U_FAILURE(حالت)) { أرجع; }
                    لكل (صحيح k = 0; k < array.get_ناپ(); k++) {
                        array.get_قدر(k, value);
                        UnicodeString str = value.getUnicodeString(حالت);
                        إذا (U_FAILURE(حالت)) { أرجع; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        إذا (str.اشاریہ_دا(u'.') != -1) {
                            save_سیٹ(isLenient ? PERIOD : STRICT_PERIOD, str, حالت);
                        } وإلا إذا (str.اشاریہ_دا(u',') != -1) {
                            save_سیٹ(isLenient ? COMMA : STRICT_COMMA, str, حالت);
                        } وإلا إذا (str.اشاریہ_دا(u'+') != -1) {
                            save_سیٹ(PLUS_SIGN, str, حالت);
                        } وإلا إذا (str.اشاریہ_دا(u'-') != -1) {
                            save_سیٹ(MINUS_SIGN, str, حالت);
                        } وإلا إذا (str.اشاریہ_دا(u'$') != -1) {
                            save_سیٹ(DOLLAR_SIGN, str, حالت);
                        } وإلا إذا (str.اشاریہ_دا(u'£') != -1) {
                            save_سیٹ(POUND_SIGN, str, حالت);
                        } وإلا إذا (str.اشاریہ_دا(u'₹') != -1) {
                            save_سیٹ(RUPEE_SIGN, str, حالت);
                        } وإلا إذا (str.اشاریہ_دا(u'¥') != -1) {
                            save_سیٹ(YEN_SIGN, str, حالت);
                        } وإلا إذا (str.اشاریہ_دا(u'₩') != -1) {
                            save_سیٹ(WON_SIGN, str, حالت);
                        } وإلا إذا (str.اشاریہ_دا(u'%') != -1) {
                            save_سیٹ(PERCENT_SIGN, str, حالت);
                        } وإلا إذا (str.اشاریہ_دا(u'‰') != -1) {
                            save_سیٹ(PERMILLE_SIGN, str, حالت);
                        } وإلا إذا (str.اشاریہ_دا(u'’') != -1) {
                            save_سیٹ(APOSTROPHE_SIGN, str, حالت);
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


icu::UInitOnce g_نمبر_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_نمبر_Parse_Uni_Sets() {
    إذا (g_خالی_تھاواں_Unicode_سیٹ_Initialized) {
        reinterpret_cast<Unicode_سیٹ*>(g_خالی_تھاواں_Unicode_سیٹ)->~Unicode_سیٹ();
        g_خالی_تھاواں_Unicode_سیٹ_Initialized = خطأ;
    }
    لكل (int32_t i = 0; i < UNISETS_کلید_گنتی; i++) {
        مٹاؤ gUnicodeSets[i];
        gUnicodeSets[i] = خالی_سوچک;
    }
    g_نمبر_Parse_Uni_Sets_Init_Once.reset();
    أرجع سچ;
}

فراغ U_CALLCONV init_نمبر_Parse_Uni_Sets(UError_کوڈ& حالت) {
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_نمبر_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    جديد(g_خالی_تھاواں_Unicode_سیٹ) Unicode_سیٹ();
    reinterpret_cast<Unicode_سیٹ*>(g_خالی_تھاواں_Unicode_سیٹ)->freeze();
    g_خالی_تھاواں_Unicode_سیٹ_Initialized = سچ;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = جديد Unicode_سیٹ(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", حالت);
    gUnicodeSets[STRICT_IGNORABLES] = جديد Unicode_سیٹ(u"[[:Bidi_Control:]]", حالت);

    Local_UResource_Bundle_اشارہ_گر rb(ures_open(خالی_سوچک, "root", &حالت));
    إذا (U_FAILURE(حالت)) { أرجع; }
    Parse_انکڑے_Sink sink;
    ures_get_All_اشیاء_With_Fallback(rb.getAlias(), "parse", sink, حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != خالی_سوچک);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != خالی_سوچک);
    U_ASSERT(gUnicodeSets[PERIOD] != خالی_سوچک);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != خالی_سوچک);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != خالی_سوچک);

    Local_اشارہ_گر<Unicode_سیٹ> otherGrouping(جديد Unicode_سیٹ(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        حالت
    ), حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }
    otherGrouping->جوڑو_All(*gUnicodeSets[APOSTROPHE_SIGN]);
    gUnicodeSets[OTHER_GROUPING_SEPARATORS] = otherGrouping.orphan();
    gUnicodeSets[ALL_SEPARATORS] = computeUnion(COMMA, PERIOD, OTHER_GROUPING_SEPARATORS);
    gUnicodeSets[STRICT_ALL_SEPARATORS] = computeUnion(
            STRICT_COMMA, STRICT_PERIOD, OTHER_GROUPING_SEPARATORS);

    U_ASSERT(gUnicodeSets[MINUS_SIGN] != خالی_سوچک);
    U_ASSERT(gUnicodeSets[PLUS_SIGN] != خالی_سوچک);
    U_ASSERT(gUnicodeSets[PERCENT_SIGN] != خالی_سوچک);
    U_ASSERT(gUnicodeSets[PERMILLE_SIGN] != خالی_سوچک);

    // The following don't currently have parseLenients in data.
    U_ASSERT(gUnicodeSets[INFINITY_SIGN] == خالی_سوچک);
    gUnicodeSets[INFINITY_SIGN] = جديد Unicode_سیٹ(u"[∞]", حالت);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == خالی_سوچک);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = جديد Unicode_سیٹ(u"[∼~≈≃約]", حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != خالی_سوچک);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != خالی_سوچک);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != خالی_سوچک);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != خالی_سوچک);
    U_ASSERT(gUnicodeSets[WON_SIGN] != خالی_سوچک);

    gUnicodeSets[DIGITS] = جديد Unicode_سیٹ(u"[:digit:]", حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    لكل (خودکار* uniset : gUnicodeSets) {
        إذا (uniset != خالی_سوچک) {
            uniset->freeze();
        }
    }
}

}

ثابت Unicode_سیٹ* unisets::get(کلید_2 کلید) {
    UError_کوڈ local_حالت = U_ZERO_ERROR;
    umtx_initOnce(g_نمبر_Parse_Uni_Sets_Init_Once, &init_نمبر_Parse_Uni_Sets, local_حالت);
    إذا (U_FAILURE(local_حالت)) {
        أرجع reinterpret_cast<Unicode_سیٹ*>(g_خالی_تھاواں_Unicode_سیٹ);
    }
    أرجع getImpl(کلید);
}

کلید_2 unisets::چنو_From(UnicodeString str, کلید_2 key1) {
    أرجع get(key1)->شامل_اے(str) ? key1 : NONE;
}

کلید_2 unisets::چنو_From(UnicodeString str, کلید_2 key1, کلید_2 key2) {
    أرجع get(key1)->شامل_اے(str) ? key1 : چنو_From(str, key2);
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


#جے_انت /* #if !UCONFIG_NO_FORMATTING */
