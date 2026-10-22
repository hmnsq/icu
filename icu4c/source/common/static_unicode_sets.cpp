// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#تعریف_کړه UNISTR_FROM_STRING_EXPLICIT

#ضمن "static_unicode_sets.h"
#ضمن "umutex.h"
#ضمن "ucln_cmn.h"
#ضمن "unicode/uniset.h"
#ضمن "uresimp.h"
#ضمن "cstring.h"
#ضمن "uassert.h"

کارول نطاق icu;
کارول نطاق icu::unisets;


نطاق {

Unicode_سېټ* gUnicodeSets[UNISETS_کلی_شمېر] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(Unicode_سېټ)
حرف g_تشه_Unicode_سېټ[حجم(Unicode_سېټ)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool g_تشه_Unicode_سېټ_Initialized = خطأ;

مضمن Unicode_سېټ* getImpl(کلی_2 کلی) {
    Unicode_سېټ* candidate = gUnicodeSets[کلی];
    إذا (candidate == تش_اشاره) {
        أرجع reinterpret_cast<Unicode_سېټ*>(g_تشه_Unicode_سېټ);
    }
    أرجع candidate;
}

Unicode_سېټ* computeUnion(کلی_2 k1, کلی_2 k2) {
    Unicode_سېټ* پایله = جديد Unicode_سېټ();
    إذا (پایله == تش_اشاره) {
        أرجع تش_اشاره;
    }
    پایله->جمع_کړه_All(*getImpl(k1));
    پایله->جمع_کړه_All(*getImpl(k2));
    پایله->freeze();
    أرجع پایله;
}

Unicode_سېټ* computeUnion(کلی_2 k1, کلی_2 k2, کلی_2 k3) {
    Unicode_سېټ* پایله = جديد Unicode_سېټ();
    إذا (پایله == تش_اشاره) {
        أرجع تش_اشاره;
    }
    پایله->جمع_کړه_All(*getImpl(k1));
    پایله->جمع_کړه_All(*getImpl(k2));
    پایله->جمع_کړه_All(*getImpl(k3));
    پایله->freeze();
    أرجع پایله;
}


فراغ save_سېټ(کلی_2 کلی, ثابت UnicodeString& unicode_سېټ_نمونه, UError_کوډ& حالت) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[کلی] = جديد Unicode_سېټ(unicode_سېټ_نمونه, حالت);
}

صنف Parse_معلومات_Sink : عام ResourceSink {
  عام:
    فراغ put(ثابت حرف* کلی, Resource_ارزښت& value, UBool /*noFallback*/, UError_کوډ& حالت) override {
        Resource_جدول contexts_جدول = value.get_جدول(حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
        لكل (صحيح i = 0; contexts_جدول.get_کلی_And_ارزښت(i, کلی, value); i++) {
            إذا (uprv_strcmp(کلی, "date") == 0) {
                // ignore
            } وإلا {
                Resource_جدول strictness_جدول = value.get_جدول(حالت);
                إذا (U_FAILURE(حالت)) { أرجع; }
                لكل (صحيح j = 0; strictness_جدول.get_کلی_And_ارزښت(j, کلی, value); j++) {
                    منطقي isLenient = (uprv_strcmp(کلی, "lenient") == 0);
                    ResourceArray array = value.getArray(حالت);
                    إذا (U_FAILURE(حالت)) { أرجع; }
                    لكل (صحيح k = 0; k < array.get_اندازه(); k++) {
                        array.get_ارزښت(k, value);
                        UnicodeString str = value.getUnicodeString(حالت);
                        إذا (U_FAILURE(حالت)) { أرجع; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        إذا (str.شاخص_د(u'.') != -1) {
                            save_سېټ(isLenient ? PERIOD : STRICT_PERIOD, str, حالت);
                        } وإلا إذا (str.شاخص_د(u',') != -1) {
                            save_سېټ(isLenient ? COMMA : STRICT_COMMA, str, حالت);
                        } وإلا إذا (str.شاخص_د(u'+') != -1) {
                            save_سېټ(PLUS_SIGN, str, حالت);
                        } وإلا إذا (str.شاخص_د(u'-') != -1) {
                            save_سېټ(MINUS_SIGN, str, حالت);
                        } وإلا إذا (str.شاخص_د(u'$') != -1) {
                            save_سېټ(DOLLAR_SIGN, str, حالت);
                        } وإلا إذا (str.شاخص_د(u'£') != -1) {
                            save_سېټ(POUND_SIGN, str, حالت);
                        } وإلا إذا (str.شاخص_د(u'₹') != -1) {
                            save_سېټ(RUPEE_SIGN, str, حالت);
                        } وإلا إذا (str.شاخص_د(u'¥') != -1) {
                            save_سېټ(YEN_SIGN, str, حالت);
                        } وإلا إذا (str.شاخص_د(u'₩') != -1) {
                            save_سېټ(WON_SIGN, str, حالت);
                        } وإلا إذا (str.شاخص_د(u'%') != -1) {
                            save_سېټ(PERCENT_SIGN, str, حالت);
                        } وإلا إذا (str.شاخص_د(u'‰') != -1) {
                            save_سېټ(PERMILLE_SIGN, str, حالت);
                        } وإلا إذا (str.شاخص_د(u'’') != -1) {
                            save_سېټ(APOSTROPHE_SIGN, str, حالت);
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


icu::UInitOnce g_شمېره_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_شمېره_Parse_Uni_Sets() {
    إذا (g_تشه_Unicode_سېټ_Initialized) {
        reinterpret_cast<Unicode_سېټ*>(g_تشه_Unicode_سېټ)->~Unicode_سېټ();
        g_تشه_Unicode_سېټ_Initialized = خطأ;
    }
    لكل (int32_t i = 0; i < UNISETS_کلی_شمېر; i++) {
        ړنګول gUnicodeSets[i];
        gUnicodeSets[i] = تش_اشاره;
    }
    g_شمېره_Parse_Uni_Sets_Init_Once.reset();
    أرجع ریښتیا;
}

فراغ U_CALLCONV init_شمېره_Parse_Uni_Sets(UError_کوډ& حالت) {
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_شمېره_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    جديد(g_تشه_Unicode_سېټ) Unicode_سېټ();
    reinterpret_cast<Unicode_سېټ*>(g_تشه_Unicode_سېټ)->freeze();
    g_تشه_Unicode_سېټ_Initialized = ریښتیا;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = جديد Unicode_سېټ(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", حالت);
    gUnicodeSets[STRICT_IGNORABLES] = جديد Unicode_سېټ(u"[[:Bidi_Control:]]", حالت);

    Local_UResource_Bundle_اشاره_ګر rb(ures_open(تش_اشاره, "root", &حالت));
    إذا (U_FAILURE(حالت)) { أرجع; }
    Parse_معلومات_Sink sink;
    ures_get_All_توکي_With_Fallback(rb.getAlias(), "parse", sink, حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != تش_اشاره);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != تش_اشاره);
    U_ASSERT(gUnicodeSets[PERIOD] != تش_اشاره);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != تش_اشاره);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != تش_اشاره);

    Local_اشاره_ګر<Unicode_سېټ> otherGrouping(جديد Unicode_سېټ(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        حالت
    ), حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }
    otherGrouping->جمع_کړه_All(*gUnicodeSets[APOSTROPHE_SIGN]);
    gUnicodeSets[OTHER_GROUPING_SEPARATORS] = otherGrouping.orphan();
    gUnicodeSets[ALL_SEPARATORS] = computeUnion(COMMA, PERIOD, OTHER_GROUPING_SEPARATORS);
    gUnicodeSets[STRICT_ALL_SEPARATORS] = computeUnion(
            STRICT_COMMA, STRICT_PERIOD, OTHER_GROUPING_SEPARATORS);

    U_ASSERT(gUnicodeSets[MINUS_SIGN] != تش_اشاره);
    U_ASSERT(gUnicodeSets[PLUS_SIGN] != تش_اشاره);
    U_ASSERT(gUnicodeSets[PERCENT_SIGN] != تش_اشاره);
    U_ASSERT(gUnicodeSets[PERMILLE_SIGN] != تش_اشاره);

    // The following don't currently have parseLenients in data.
    U_ASSERT(gUnicodeSets[INFINITY_SIGN] == تش_اشاره);
    gUnicodeSets[INFINITY_SIGN] = جديد Unicode_سېټ(u"[∞]", حالت);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == تش_اشاره);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = جديد Unicode_سېټ(u"[∼~≈≃約]", حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != تش_اشاره);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != تش_اشاره);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != تش_اشاره);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != تش_اشاره);
    U_ASSERT(gUnicodeSets[WON_SIGN] != تش_اشاره);

    gUnicodeSets[DIGITS] = جديد Unicode_سېټ(u"[:digit:]", حالت);
    إذا (U_FAILURE(حالت)) { أرجع; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    لكل (خپلکاری* uniset : gUnicodeSets) {
        إذا (uniset != تش_اشاره) {
            uniset->freeze();
        }
    }
}

}

ثابت Unicode_سېټ* unisets::get(کلی_2 کلی) {
    UError_کوډ local_حالت = U_ZERO_ERROR;
    umtx_initOnce(g_شمېره_Parse_Uni_Sets_Init_Once, &init_شمېره_Parse_Uni_Sets, local_حالت);
    إذا (U_FAILURE(local_حالت)) {
        أرجع reinterpret_cast<Unicode_سېټ*>(g_تشه_Unicode_سېټ);
    }
    أرجع getImpl(کلی);
}

کلی_2 unisets::ټاکنه_From(UnicodeString str, کلی_2 key1) {
    أرجع get(key1)->لري(str) ? key1 : NONE;
}

کلی_2 unisets::ټاکنه_From(UnicodeString str, کلی_2 key1, کلی_2 key2) {
    أرجع get(key1)->لري(str) ? key1 : ټاکنه_From(str, key2);
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


#د_که_پای /* #if !UCONFIG_NO_FORMATTING */
