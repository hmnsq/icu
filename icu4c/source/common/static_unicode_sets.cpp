// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#تعریف_کرو UNISTR_FROM_STRING_EXPLICIT

#شامل "static_unicode_sets.h"
#شامل "umutex.h"
#شامل "ucln_cmn.h"
#شامل "unicode/uniset.h"
#شامل "uresimp.h"
#شامل "cstring.h"
#شامل "uassert.h"

استعمال نامخلا icu;
استعمال نامخلا icu::unisets;


نامخلا {

Unicode_سیٹ* gUnicodeSets[UNISETS_کلید_گنتی] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(Unicode_سیٹ)
حرف g_خالی_خانے_Unicode_سیٹ[حجم(Unicode_سیٹ)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool g_خالی_خانے_Unicode_سیٹ_Initialized = جھوٹ;

اندرونی Unicode_سیٹ* getImpl(کلید_2 کلید) {
    Unicode_سیٹ* candidate = gUnicodeSets[کلید];
    اگر (candidate == خالی_اشاریہ) {
        واپس reinterpret_cast<Unicode_سیٹ*>(g_خالی_خانے_Unicode_سیٹ);
    }
    واپس candidate;
}

Unicode_سیٹ* computeUnion(کلید_2 k1, کلید_2 k2) {
    Unicode_سیٹ* نتیجہ = نیا Unicode_سیٹ();
    اگر (نتیجہ == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }
    نتیجہ->جوڑو_All(*getImpl(k1));
    نتیجہ->جوڑو_All(*getImpl(k2));
    نتیجہ->freeze();
    واپس نتیجہ;
}

Unicode_سیٹ* computeUnion(کلید_2 k1, کلید_2 k2, کلید_2 k3) {
    Unicode_سیٹ* نتیجہ = نیا Unicode_سیٹ();
    اگر (نتیجہ == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }
    نتیجہ->جوڑو_All(*getImpl(k1));
    نتیجہ->جوڑو_All(*getImpl(k2));
    نتیجہ->جوڑو_All(*getImpl(k3));
    نتیجہ->freeze();
    واپس نتیجہ;
}


خالی save_سیٹ(کلید_2 کلید, ثابت UnicodeString& unicode_سیٹ_نمونہ, UError_رمز& حالت_2) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[کلید] = نیا Unicode_سیٹ(unicode_سیٹ_نمونہ, حالت_2);
}

جماعت Parse_ڈیٹا_Sink : عوامی ResourceSink {
  عوامی:
    خالی put(ثابت حرف* کلید, Resource_قدر& value, UBool /*noFallback*/, UError_رمز& حالت_2) override {
        Resource_جدول contexts_جدول = value.get_جدول(حالت_2);
        اگر (U_FAILURE(حالت_2)) { واپس; }
        برائے (صحیح i = 0; contexts_جدول.get_کلید_And_قدر(i, کلید, value); i++) {
            اگر (uprv_strcmp(کلید, "date") == 0) {
                // ignore
            } ورنہ {
                Resource_جدول strictness_جدول = value.get_جدول(حالت_2);
                اگر (U_FAILURE(حالت_2)) { واپس; }
                برائے (صحیح j = 0; strictness_جدول.get_کلید_And_قدر(j, کلید, value); j++) {
                    منطقی isLenient = (uprv_strcmp(کلید, "lenient") == 0);
                    ResourceArray array = value.getArray(حالت_2);
                    اگر (U_FAILURE(حالت_2)) { واپس; }
                    برائے (صحیح k = 0; k < array.get_سائز(); k++) {
                        array.get_قدر(k, value);
                        UnicodeString str = value.getUnicodeString(حالت_2);
                        اگر (U_FAILURE(حالت_2)) { واپس; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        اگر (str.اشاریہ_کا(u'.') != -1) {
                            save_سیٹ(isLenient ? PERIOD : STRICT_PERIOD, str, حالت_2);
                        } ورنہ اگر (str.اشاریہ_کا(u',') != -1) {
                            save_سیٹ(isLenient ? COMMA : STRICT_COMMA, str, حالت_2);
                        } ورنہ اگر (str.اشاریہ_کا(u'+') != -1) {
                            save_سیٹ(PLUS_SIGN, str, حالت_2);
                        } ورنہ اگر (str.اشاریہ_کا(u'-') != -1) {
                            save_سیٹ(MINUS_SIGN, str, حالت_2);
                        } ورنہ اگر (str.اشاریہ_کا(u'$') != -1) {
                            save_سیٹ(DOLLAR_SIGN, str, حالت_2);
                        } ورنہ اگر (str.اشاریہ_کا(u'£') != -1) {
                            save_سیٹ(POUND_SIGN, str, حالت_2);
                        } ورنہ اگر (str.اشاریہ_کا(u'₹') != -1) {
                            save_سیٹ(RUPEE_SIGN, str, حالت_2);
                        } ورنہ اگر (str.اشاریہ_کا(u'¥') != -1) {
                            save_سیٹ(YEN_SIGN, str, حالت_2);
                        } ورنہ اگر (str.اشاریہ_کا(u'₩') != -1) {
                            save_سیٹ(WON_SIGN, str, حالت_2);
                        } ورنہ اگر (str.اشاریہ_کا(u'%') != -1) {
                            save_سیٹ(PERCENT_SIGN, str, حالت_2);
                        } ورنہ اگر (str.اشاریہ_کا(u'‰') != -1) {
                            save_سیٹ(PERMILLE_SIGN, str, حالت_2);
                        } ورنہ اگر (str.اشاریہ_کا(u'’') != -1) {
                            save_سیٹ(APOSTROPHE_SIGN, str, حالت_2);
                        } ورنہ {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(جھوٹ);
                        }
                        اگر (U_FAILURE(حالت_2)) { واپس; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_نمبر_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_نمبر_Parse_Uni_Sets() {
    اگر (g_خالی_خانے_Unicode_سیٹ_Initialized) {
        reinterpret_cast<Unicode_سیٹ*>(g_خالی_خانے_Unicode_سیٹ)->~Unicode_سیٹ();
        g_خالی_خانے_Unicode_سیٹ_Initialized = جھوٹ;
    }
    برائے (int32_t i = 0; i < UNISETS_کلید_گنتی; i++) {
        حذف gUnicodeSets[i];
        gUnicodeSets[i] = خالی_اشاریہ;
    }
    g_نمبر_Parse_Uni_Sets_Init_Once.reset();
    واپس سچ;
}

خالی U_CALLCONV init_نمبر_Parse_Uni_Sets(UError_رمز& حالت_2) {
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_نمبر_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    نیا(g_خالی_خانے_Unicode_سیٹ) Unicode_سیٹ();
    reinterpret_cast<Unicode_سیٹ*>(g_خالی_خانے_Unicode_سیٹ)->freeze();
    g_خالی_خانے_Unicode_سیٹ_Initialized = سچ;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = نیا Unicode_سیٹ(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", حالت_2);
    gUnicodeSets[STRICT_IGNORABLES] = نیا Unicode_سیٹ(u"[[:Bidi_Control:]]", حالت_2);

    Local_UResource_Bundle_اشارہ_گر rb(ures_open(خالی_اشاریہ, "root", &حالت_2));
    اگر (U_FAILURE(حالت_2)) { واپس; }
    Parse_ڈیٹا_Sink sink;
    ures_get_All_اشیاء_With_Fallback(rb.getAlias(), "parse", sink, حالت_2);
    اگر (U_FAILURE(حالت_2)) { واپس; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != خالی_اشاریہ);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != خالی_اشاریہ);
    U_ASSERT(gUnicodeSets[PERIOD] != خالی_اشاریہ);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != خالی_اشاریہ);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != خالی_اشاریہ);

    Local_اشارہ_گر<Unicode_سیٹ> otherGrouping(نیا Unicode_سیٹ(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        حالت_2
    ), حالت_2);
    اگر (U_FAILURE(حالت_2)) { واپس; }
    otherGrouping->جوڑو_All(*gUnicodeSets[APOSTROPHE_SIGN]);
    gUnicodeSets[OTHER_GROUPING_SEPARATORS] = otherGrouping.orphan();
    gUnicodeSets[ALL_SEPARATORS] = computeUnion(COMMA, PERIOD, OTHER_GROUPING_SEPARATORS);
    gUnicodeSets[STRICT_ALL_SEPARATORS] = computeUnion(
            STRICT_COMMA, STRICT_PERIOD, OTHER_GROUPING_SEPARATORS);

    U_ASSERT(gUnicodeSets[MINUS_SIGN] != خالی_اشاریہ);
    U_ASSERT(gUnicodeSets[PLUS_SIGN] != خالی_اشاریہ);
    U_ASSERT(gUnicodeSets[PERCENT_SIGN] != خالی_اشاریہ);
    U_ASSERT(gUnicodeSets[PERMILLE_SIGN] != خالی_اشاریہ);

    // The following don't currently have parseLenients in data.
    U_ASSERT(gUnicodeSets[INFINITY_SIGN] == خالی_اشاریہ);
    gUnicodeSets[INFINITY_SIGN] = نیا Unicode_سیٹ(u"[∞]", حالت_2);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == خالی_اشاریہ);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = نیا Unicode_سیٹ(u"[∼~≈≃約]", حالت_2);
    اگر (U_FAILURE(حالت_2)) { واپس; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != خالی_اشاریہ);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != خالی_اشاریہ);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != خالی_اشاریہ);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != خالی_اشاریہ);
    U_ASSERT(gUnicodeSets[WON_SIGN] != خالی_اشاریہ);

    gUnicodeSets[DIGITS] = نیا Unicode_سیٹ(u"[:digit:]", حالت_2);
    اگر (U_FAILURE(حالت_2)) { واپس; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    برائے (خودکار* uniset : gUnicodeSets) {
        اگر (uniset != خالی_اشاریہ) {
            uniset->freeze();
        }
    }
}

}

ثابت Unicode_سیٹ* unisets::get(کلید_2 کلید) {
    UError_رمز local_حالت = U_ZERO_ERROR;
    umtx_initOnce(g_نمبر_Parse_Uni_Sets_Init_Once, &init_نمبر_Parse_Uni_Sets, local_حالت);
    اگر (U_FAILURE(local_حالت)) {
        واپس reinterpret_cast<Unicode_سیٹ*>(g_خالی_خانے_Unicode_سیٹ);
    }
    واپس getImpl(کلید);
}

کلید_2 unisets::انتخاب_From(UnicodeString str, کلید_2 key1) {
    واپس get(key1)->شامل_ہے(str) ? key1 : NONE;
}

کلید_2 unisets::انتخاب_From(UnicodeString str, کلید_2 key1, کلید_2 key2) {
    واپس get(key1)->شامل_ہے(str) ? key1 : انتخاب_From(str, key2);
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


#اگر_ختم /* #if !UCONFIG_NO_FORMATTING */
