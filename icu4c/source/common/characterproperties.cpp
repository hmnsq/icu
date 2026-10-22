// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#ضمن "unicode/utypes.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/ucpmap.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/uscript.h"
#ضمن "unicode/uset.h"
#ضمن "cmemory.h"
#ضمن "emojiprops.h"
#ضمن "mutex.h"
#ضمن "normalizer2impl.h"
#ضمن "uassert.h"
#ضمن "ubidi_props.h"
#ضمن "ucase.h"
#ضمن "ucln_cmn.h"
#ضمن "umutex.h"
#ضمن "uprops.h"

استفاده icu::Local_اشاره_گر;
#إذا !UCONFIG_NO_NORMALIZATION
استفاده icu::Normalizer2Factory;
استفاده icu::Normalizer2Impl;
#پایان_اگر
استفاده icu::UInitOnce;
استفاده icu::Unicode_مجموعه;

نطاق {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t رقم_INCLUSIONS = UPROPS_SRC_شمار + (UCHAR_INT_حد - UCHAR_INT_شروع);

بنية Inclusion {
    Unicode_مجموعه  *f_مجموعه = اشاره_تهی;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[رقم_INCLUSIONS]; // cached getInclusions()

Unicode_مجموعه *sets[UCHAR_دودویی_حد] = {};

UCPMap *maps[UCHAR_INT_حد - UCHAR_INT_شروع] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
فراغ U_CALLCONV
مجموعه_جمع_کن(USet *مجموعه, UChar32 c) {
    reinterpret_cast<Unicode_مجموعه*>(مجموعه)->جمع_کن(c);
}

فراغ U_CALLCONV
مجموعه_جمع_کن_Range(USet *مجموعه, UChar32 شروع, UChar32 انتها) {
    reinterpret_cast<Unicode_مجموعه*>(مجموعه)->جمع_کن(شروع, انتها);
}

فراغ U_CALLCONV
مجموعه_جمع_کن_String(USet *مجموعه, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_مجموعه*>(مجموعه)->جمع_کن(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    لكل (Inclusion &in: gInclusions) {
        حذف in.f_مجموعه;
        in.f_مجموعه = اشاره_تهی;
        in.fInitOnce.reset();
    }
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        حذف sets[i];
        sets[i] = اشاره_تهی;
    }
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = اشاره_تهی;
    }
    أرجع درست;
}

فراغ U_CALLCONV initInclusion(UProperty_منبع src, UError_کد &error_کد) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_شمار);
    إذا (src == UPROPS_SRC_NONE) {
        error_کد = U_داخلی_برنامه_ERROR;
        أرجع;
    }
    U_ASSERT(gInclusions[src].f_مجموعه == اشاره_تهی);

    Local_اشاره_گر<Unicode_مجموعه> incl(جديد Unicode_مجموعه());
    إذا (incl.isNull()) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        مجموعه_جمع_کن,
        مجموعه_جمع_کن_Range,
        مجموعه_جمع_کن_String,
        اشاره_تهی, // don't need remove()
        اشاره_تهی // don't need removeRange()
    };

    بدل(src) {
    حالة UPROPS_SRC_CHAR:
        uchar_جمع_کن_Property_Starts(&sa, &error_کد);
        اكسر;
    حالة UPROPS_SRC_PROPSVEC:
        upropsvec_جمع_کن_Property_Starts(&sa, &error_کد);
        اكسر;
    حالة UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_جمع_کن_Property_Starts(&sa, &error_کد);
        upropsvec_جمع_کن_Property_Starts(&sa, &error_کد);
        اكسر;
#إذا !UCONFIG_NO_NORMALIZATION
    حالة UPROPS_SRC_CASE_AND_NORM: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کد);
        إذا(U_SUCCESS(error_کد)) {
            impl->جمع_کن_Property_Starts(&sa, error_کد);
        }
        ucase_جمع_کن_Property_Starts(&sa, &error_کد);
        اكسر;
    }
    حالة UPROPS_SRC_NFC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کد);
        إذا(U_SUCCESS(error_کد)) {
            impl->جمع_کن_Property_Starts(&sa, error_کد);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFKC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(error_کد);
        إذا(U_SUCCESS(error_کد)) {
            impl->جمع_کن_Property_Starts(&sa, error_کد);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFKC_CF: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(error_کد);
        إذا(U_SUCCESS(error_کد)) {
            impl->جمع_کن_Property_Starts(&sa, error_کد);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFC_CANON_ITER: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کد);
        إذا(U_SUCCESS(error_کد)) {
            impl->جمع_کن_Canon_Iter_Property_Starts(&sa, error_کد);
        }
        اكسر;
    }
#پایان_اگر
    حالة UPROPS_SRC_CASE:
        ucase_جمع_کن_Property_Starts(&sa, &error_کد);
        اكسر;
    حالة UPROPS_SRC_BIDI:
        ubidi_جمع_کن_Property_Starts(&sa, &error_کد);
        اكسر;
    حالة UPROPS_SRC_INPC:
    حالة UPROPS_SRC_INSC:
    حالة UPROPS_SRC_VO:
        uprops_جمع_کن_Property_Starts(src, &sa, &error_کد);
        اكسر;
    حالة UPROPS_SRC_EMOJI: {
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_کد);
        إذا (U_SUCCESS(error_کد)) {
            ep->جمع_کن_Property_Starts(&sa, error_کد);
        }
        اكسر;
    }
    حالة UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.جمع_کن(sa.مجموعه, 0x2FFE);
        sa.جمع_کن(sa.مجموعه, 0x2FFF + 1);
        اكسر;
    حالة UPROPS_SRC_ID_COMPAT_MATH:
    حالة UPROPS_SRC_MCM:
        uprops_جمع_کن_Property_Starts(src, &sa, &error_کد);
        اكسر;
    حالة UPROPS_SRC_بلوک:
        ublock_جمع_کن_Property_Starts(&sa, error_کد);
        اكسر;
    افتراضي:
        error_کد = U_داخلی_برنامه_ERROR;
        اكسر;
    }

    إذا (U_FAILURE(error_کد)) {
        أرجع;
    }
    إذا (incl->isBogus()) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].f_مجموعه = incl.orphan();
    ucln_common_ثبات_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ثابت Unicode_مجموعه *get_Inclusions_For_منبع(UProperty_منبع src, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    إذا (src < 0 || UPROPS_SRC_شمار <= src) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, error_کد);
    أرجع i.f_مجموعه;
}

فراغ U_CALLCONV initIntPropInclusion(UProperty prop, UError_کد &error_کد) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_شروع <= prop && prop < UCHAR_INT_حد);
    int32_t incl_شاخص = UPROPS_SRC_شمار + (prop - UCHAR_INT_شروع);
    U_ASSERT(gInclusions[incl_شاخص].f_مجموعه == اشاره_تهی);
    UProperty_منبع src = uprops_get_منبع(prop);
    ثابت Unicode_مجموعه *incl = get_Inclusions_For_منبع(src, error_کد);
    إذا (U_FAILURE(error_کد)) {
        أرجع;
    }

    Local_اشاره_گر<Unicode_مجموعه> intPropIncl(جديد Unicode_مجموعه(0, 0));
    إذا (intPropIncl.isNull()) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    int32_t رقم_Ranges = incl->get_Range_شمار();
    int32_t prev_مقدار = 0;
    لكل (int32_t i = 0; i < رقم_Ranges; ++i) {
        UChar32 range_انتها = incl->get_Range_انتها(i);
        لكل (UChar32 c = incl->get_Range_شروع(i); c <= range_انتها; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_مقدار(c, prop);
            إذا (value != prev_مقدار) {
                intPropIncl->جمع_کن(c);
                prev_مقدار = value;
            }
        }
    }

    إذا (intPropIncl->isBogus()) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[incl_شاخص].f_مجموعه = intPropIncl.orphan();
    ucln_common_ثبات_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت Unicode_مجموعه *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    إذا (UCHAR_INT_شروع <= prop && prop < UCHAR_INT_حد) {
        int32_t incl_شاخص = UPROPS_SRC_شمار + (prop - UCHAR_INT_شروع);
        Inclusion &i = gInclusions[incl_شاخص];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, error_کد);
        أرجع i.f_مجموعه;
    } وإلا {
        UProperty_منبع src = uprops_get_منبع(prop);
        أرجع get_Inclusions_For_منبع(src, error_کد);
    }
}

U_NAMESPACE_انتها

نطاق {

Unicode_مجموعه *make_مجموعه(UProperty property, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    Local_اشاره_گر<Unicode_مجموعه> مجموعه(جديد Unicode_مجموعه());
    إذا (مجموعه.isNull()) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    إذا (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
        USetAdder sa = {
            reinterpret_cast<USet*>(مجموعه.getAlias()),
            مجموعه_جمع_کن,
            مجموعه_جمع_کن_Range,
            مجموعه_جمع_کن_String,
            اشاره_تهی, // don't need remove()
            اشاره_تهی // don't need removeRange()
        };
        ep->جمع_کن_رشتهها(&sa, property, error_کد);
        إذا (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            مجموعه->freeze();
            أرجع مجموعه.orphan();
        }
    }

    ثابت Unicode_مجموعه *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    int32_t رقم_Ranges = inclusions->get_Range_شمار();
    UChar32 شروع_Has_Property = -1;

    لكل (int32_t i = 0; i < رقم_Ranges; ++i) {
        UChar32 range_انتها = inclusions->get_Range_انتها(i);
        لكل (UChar32 c = inclusions->get_Range_شروع(i); c <= range_انتها; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            إذا (u_has_دودویی_Property(c, property)) {
                إذا (شروع_Has_Property < 0) {
                    // Transition from false to true.
                    شروع_Has_Property = c;
                }
            } وإلا إذا (شروع_Has_Property >= 0) {
                // Transition from true to false.
                مجموعه->جمع_کن(شروع_Has_Property, c - 1);
                شروع_Has_Property = -1;
            }
        }
    }
    إذا (شروع_Has_Property >= 0) {
        مجموعه->جمع_کن(شروع_Has_Property, 0x10FFFF);
    }
    مجموعه->freeze();
    أرجع مجموعه.orphan();
}

UCPMap *makeMap(UProperty property, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    uint32_t null_مقدار = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::Local_UMutable_CPTrie_اشاره_گر mutableTrie(
        umutablecptrie_open(null_مقدار, null_مقدار, &error_کد));
    ثابت Unicode_مجموعه *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    int32_t رقم_Ranges = inclusions->get_Range_شمار();
    UChar32 شروع = 0;
    uint32_t value = null_مقدار;

    لكل (int32_t i = 0; i < رقم_Ranges; ++i) {
        UChar32 range_انتها = inclusions->get_Range_انتها(i);
        لكل (UChar32 c = inclusions->get_Range_شروع(i); c <= range_انتها; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t بعدی_مقدار = u_get_Int_Property_مقدار(c, property);
            إذا (value != بعدی_مقدار) {
                إذا (value != null_مقدار) {
                    umutablecptrie_مجموعه_Range(mutableTrie.getAlias(), شروع, c - 1, value, &error_کد);
                }
                شروع = c;
                value = بعدی_مقدار;
            }
        }
    }
    إذا (value != 0) {
        umutablecptrie_مجموعه_Range(mutableTrie.getAlias(), شروع, 0x10FFFF, value, &error_کد);
    }

    UCPTrieType type;
    إذا (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } وإلا {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_مقدار_عرض مقدار_عرض;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_مقدار(property);
    إذا (max <= 0xff) {
        مقدار_عرض = UCPTRIE_مقدار_بیتها_8;
    } وإلا إذا (max <= 0xffff) {
        مقدار_عرض = UCPTRIE_مقدار_بیتها_16;
    } وإلا {
        مقدار_عرض = UCPTRIE_مقدار_بیتها_32;
    }
    أرجع reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, مقدار_عرض, &error_کد));
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت Unicode_مجموعه *CharacterProperties::get_دودویی_Property_مجموعه(UProperty property, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    إذا (property < 0 || UCHAR_دودویی_حد <= property) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    Mutex m(&cpMutex);
    Unicode_مجموعه *مجموعه = sets[property];
    إذا (مجموعه == اشاره_تهی) {
        sets[property] = مجموعه = make_مجموعه(property, error_کد);
    }
    أرجع مجموعه;
}

U_NAMESPACE_انتها

U_NAMESPACE_USE

U_CAPI ثابت USet * U_EXPORT2
u_get_دودویی_Property_مجموعه(UProperty property, UError_کد *p_Error_کد) {
    ثابت Unicode_مجموعه *مجموعه = CharacterProperties::get_دودویی_Property_مجموعه(property, *p_Error_کد);
    أرجع U_SUCCESS(*p_Error_کد) ? مجموعه->مقصد_USet() : اشاره_تهی;
}

U_CAPI ثابت UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) { أرجع اشاره_تهی; }
    إذا (property < UCHAR_INT_شروع || UCHAR_INT_حد <= property) {
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_شروع];
    إذا (map == اشاره_تهی) {
        maps[property - UCHAR_INT_شروع] = map = makeMap(property, *p_Error_کد);
    }
    أرجع map;
}
