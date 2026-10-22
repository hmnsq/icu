// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#تعریف_کن UNISTR_FROM_STRING_EXPLICIT

#ضمن "static_unicode_sets.h"
#ضمن "umutex.h"
#ضمن "ucln_cmn.h"
#ضمن "unicode/uniset.h"
#ضمن "uresimp.h"
#ضمن "cstring.h"
#ضمن "uassert.h"

استفاده نطاق icu;
استفاده نطاق icu::unisets;


نطاق {

Unicode_مجموعه* gUnicodeSets[UNISETS_کلید_شمار] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(Unicode_مجموعه)
حرف g_خالی_Unicode_مجموعه[حجم(Unicode_مجموعه)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool g_خالی_Unicode_مجموعه_Initialized = خطأ;

مضمن Unicode_مجموعه* getImpl(کلید_2 کلید) {
    Unicode_مجموعه* candidate = gUnicodeSets[کلید];
    إذا (candidate == اشاره_تهی) {
        أرجع reinterpret_cast<Unicode_مجموعه*>(g_خالی_Unicode_مجموعه);
    }
    أرجع candidate;
}

Unicode_مجموعه* computeUnion(کلید_2 k1, کلید_2 k2) {
    Unicode_مجموعه* نتیجه = جديد Unicode_مجموعه();
    إذا (نتیجه == اشاره_تهی) {
        أرجع اشاره_تهی;
    }
    نتیجه->جمع_کن_All(*getImpl(k1));
    نتیجه->جمع_کن_All(*getImpl(k2));
    نتیجه->freeze();
    أرجع نتیجه;
}

Unicode_مجموعه* computeUnion(کلید_2 k1, کلید_2 k2, کلید_2 k3) {
    Unicode_مجموعه* نتیجه = جديد Unicode_مجموعه();
    إذا (نتیجه == اشاره_تهی) {
        أرجع اشاره_تهی;
    }
    نتیجه->جمع_کن_All(*getImpl(k1));
    نتیجه->جمع_کن_All(*getImpl(k2));
    نتیجه->جمع_کن_All(*getImpl(k3));
    نتیجه->freeze();
    أرجع نتیجه;
}


فراغ save_مجموعه(کلید_2 کلید, ثابت UnicodeString& unicode_مجموعه_الگو, UError_کد& وضعیت) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[کلید] = جديد Unicode_مجموعه(unicode_مجموعه_الگو, وضعیت);
}

صنف Parse_داده_Sink : عام ResourceSink {
  عام:
    فراغ put(ثابت حرف* کلید, Resource_مقدار& value, UBool /*noFallback*/, UError_کد& وضعیت) override {
        Resource_جدول contexts_جدول = value.get_جدول(وضعیت);
        إذا (U_FAILURE(وضعیت)) { أرجع; }
        لكل (صحيح i = 0; contexts_جدول.get_کلید_And_مقدار(i, کلید, value); i++) {
            إذا (uprv_strcmp(کلید, "date") == 0) {
                // ignore
            } وإلا {
                Resource_جدول strictness_جدول = value.get_جدول(وضعیت);
                إذا (U_FAILURE(وضعیت)) { أرجع; }
                لكل (صحيح j = 0; strictness_جدول.get_کلید_And_مقدار(j, کلید, value); j++) {
                    منطقي isLenient = (uprv_strcmp(کلید, "lenient") == 0);
                    ResourceArray array = value.getArray(وضعیت);
                    إذا (U_FAILURE(وضعیت)) { أرجع; }
                    لكل (صحيح k = 0; k < array.get_اندازه(); k++) {
                        array.get_مقدار(k, value);
                        UnicodeString str = value.getUnicodeString(وضعیت);
                        إذا (U_FAILURE(وضعیت)) { أرجع; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        إذا (str.شاخص_از(u'.') != -1) {
                            save_مجموعه(isLenient ? PERIOD : STRICT_PERIOD, str, وضعیت);
                        } وإلا إذا (str.شاخص_از(u',') != -1) {
                            save_مجموعه(isLenient ? COMMA : STRICT_COMMA, str, وضعیت);
                        } وإلا إذا (str.شاخص_از(u'+') != -1) {
                            save_مجموعه(PLUS_SIGN, str, وضعیت);
                        } وإلا إذا (str.شاخص_از(u'-') != -1) {
                            save_مجموعه(MINUS_SIGN, str, وضعیت);
                        } وإلا إذا (str.شاخص_از(u'$') != -1) {
                            save_مجموعه(DOLLAR_SIGN, str, وضعیت);
                        } وإلا إذا (str.شاخص_از(u'£') != -1) {
                            save_مجموعه(POUND_SIGN, str, وضعیت);
                        } وإلا إذا (str.شاخص_از(u'₹') != -1) {
                            save_مجموعه(RUPEE_SIGN, str, وضعیت);
                        } وإلا إذا (str.شاخص_از(u'¥') != -1) {
                            save_مجموعه(YEN_SIGN, str, وضعیت);
                        } وإلا إذا (str.شاخص_از(u'₩') != -1) {
                            save_مجموعه(WON_SIGN, str, وضعیت);
                        } وإلا إذا (str.شاخص_از(u'%') != -1) {
                            save_مجموعه(PERCENT_SIGN, str, وضعیت);
                        } وإلا إذا (str.شاخص_از(u'‰') != -1) {
                            save_مجموعه(PERMILLE_SIGN, str, وضعیت);
                        } وإلا إذا (str.شاخص_از(u'’') != -1) {
                            save_مجموعه(APOSTROPHE_SIGN, str, وضعیت);
                        } وإلا {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(خطأ);
                        }
                        إذا (U_FAILURE(وضعیت)) { أرجع; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce g_عدد_Parse_Uni_Sets_Init_Once {};

UBool U_CALLCONV cleanup_عدد_Parse_Uni_Sets() {
    إذا (g_خالی_Unicode_مجموعه_Initialized) {
        reinterpret_cast<Unicode_مجموعه*>(g_خالی_Unicode_مجموعه)->~Unicode_مجموعه();
        g_خالی_Unicode_مجموعه_Initialized = خطأ;
    }
    لكل (int32_t i = 0; i < UNISETS_کلید_شمار; i++) {
        حذف gUnicodeSets[i];
        gUnicodeSets[i] = اشاره_تهی;
    }
    g_عدد_Parse_Uni_Sets_Init_Once.reset();
    أرجع درست;
}

فراغ U_CALLCONV init_عدد_Parse_Uni_Sets(UError_کد& وضعیت) {
    ucln_common_ثبات_Cleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanup_عدد_Parse_Uni_Sets);

    // Initialize the empty instance for well-defined fallback behavior
    جديد(g_خالی_Unicode_مجموعه) Unicode_مجموعه();
    reinterpret_cast<Unicode_مجموعه*>(g_خالی_Unicode_مجموعه)->freeze();
    g_خالی_Unicode_مجموعه_Initialized = درست;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = جديد Unicode_مجموعه(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", وضعیت);
    gUnicodeSets[STRICT_IGNORABLES] = جديد Unicode_مجموعه(u"[[:Bidi_Control:]]", وضعیت);

    Local_UResource_Bundle_اشاره_گر rb(ures_open(اشاره_تهی, "root", &وضعیت));
    إذا (U_FAILURE(وضعیت)) { أرجع; }
    Parse_داده_Sink sink;
    ures_get_All_اقلام_With_Fallback(rb.getAlias(), "parse", sink, وضعیت);
    إذا (U_FAILURE(وضعیت)) { أرجع; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != اشاره_تهی);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != اشاره_تهی);
    U_ASSERT(gUnicodeSets[PERIOD] != اشاره_تهی);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != اشاره_تهی);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != اشاره_تهی);

    Local_اشاره_گر<Unicode_مجموعه> otherGrouping(جديد Unicode_مجموعه(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        وضعیت
    ), وضعیت);
    إذا (U_FAILURE(وضعیت)) { أرجع; }
    otherGrouping->جمع_کن_All(*gUnicodeSets[APOSTROPHE_SIGN]);
    gUnicodeSets[OTHER_GROUPING_SEPARATORS] = otherGrouping.orphan();
    gUnicodeSets[ALL_SEPARATORS] = computeUnion(COMMA, PERIOD, OTHER_GROUPING_SEPARATORS);
    gUnicodeSets[STRICT_ALL_SEPARATORS] = computeUnion(
            STRICT_COMMA, STRICT_PERIOD, OTHER_GROUPING_SEPARATORS);

    U_ASSERT(gUnicodeSets[MINUS_SIGN] != اشاره_تهی);
    U_ASSERT(gUnicodeSets[PLUS_SIGN] != اشاره_تهی);
    U_ASSERT(gUnicodeSets[PERCENT_SIGN] != اشاره_تهی);
    U_ASSERT(gUnicodeSets[PERMILLE_SIGN] != اشاره_تهی);

    // The following don't currently have parseLenients in data.
    U_ASSERT(gUnicodeSets[INFINITY_SIGN] == اشاره_تهی);
    gUnicodeSets[INFINITY_SIGN] = جديد Unicode_مجموعه(u"[∞]", وضعیت);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == اشاره_تهی);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = جديد Unicode_مجموعه(u"[∼~≈≃約]", وضعیت);
    إذا (U_FAILURE(وضعیت)) { أرجع; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != اشاره_تهی);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != اشاره_تهی);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != اشاره_تهی);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != اشاره_تهی);
    U_ASSERT(gUnicodeSets[WON_SIGN] != اشاره_تهی);

    gUnicodeSets[DIGITS] = جديد Unicode_مجموعه(u"[:digit:]", وضعیت);
    إذا (U_FAILURE(وضعیت)) { أرجع; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    لكل (خودکار* uniset : gUnicodeSets) {
        إذا (uniset != اشاره_تهی) {
            uniset->freeze();
        }
    }
}

}

ثابت Unicode_مجموعه* unisets::get(کلید_2 کلید) {
    UError_کد local_وضعیت = U_ZERO_ERROR;
    umtx_initOnce(g_عدد_Parse_Uni_Sets_Init_Once, &init_عدد_Parse_Uni_Sets, local_وضعیت);
    إذا (U_FAILURE(local_وضعیت)) {
        أرجع reinterpret_cast<Unicode_مجموعه*>(g_خالی_Unicode_مجموعه);
    }
    أرجع getImpl(کلید);
}

کلید_2 unisets::انتخاب_From(UnicodeString str, کلید_2 key1) {
    أرجع get(key1)->شامل_است(str) ? key1 : NONE;
}

کلید_2 unisets::انتخاب_From(UnicodeString str, کلید_2 key1, کلید_2 key2) {
    أرجع get(key1)->شامل_است(str) ? key1 : انتخاب_From(str, key2);
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


#پایان_اگر /* #if !UCONFIG_NO_FORMATTING */
