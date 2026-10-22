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

کارول icu::Local_اشاره_ګر;
#إذا !UCONFIG_NO_NORMALIZATION
کارول icu::Normalizer2Factory;
کارول icu::Normalizer2Impl;
#د_که_پای
کارول icu::UInitOnce;
کارول icu::Unicode_سېټ;

نطاق {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t عدد_INCLUSIONS = UPROPS_SRC_شمېر + (UCHAR_INT_حد - UCHAR_INT_پیل);

بنية Inclusion {
    Unicode_سېټ  *f_سېټ = تش_اشاره;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[عدد_INCLUSIONS]; // cached getInclusions()

Unicode_سېټ *sets[UCHAR_دوګونی_حد] = {};

UCPMap *maps[UCHAR_INT_حد - UCHAR_INT_پیل] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
فراغ U_CALLCONV
سېټ_جمع_کړه(USet *سېټ, UChar32 c) {
    reinterpret_cast<Unicode_سېټ*>(سېټ)->جمع_کړه(c);
}

فراغ U_CALLCONV
سېټ_جمع_کړه_Range(USet *سېټ, UChar32 پیل, UChar32 پای_ټکی) {
    reinterpret_cast<Unicode_سېټ*>(سېټ)->جمع_کړه(پیل, پای_ټکی);
}

فراغ U_CALLCONV
سېټ_جمع_کړه_String(USet *سېټ, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_سېټ*>(سېټ)->جمع_کړه(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    لكل (Inclusion &in: gInclusions) {
        ړنګول in.f_سېټ;
        in.f_سېټ = تش_اشاره;
        in.fInitOnce.reset();
    }
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        ړنګول sets[i];
        sets[i] = تش_اشاره;
    }
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = تش_اشاره;
    }
    أرجع ریښتیا;
}

فراغ U_CALLCONV initInclusion(UProperty_سرچینه src, UError_کوډ &error_کوډ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_شمېر);
    إذا (src == UPROPS_SRC_NONE) {
        error_کوډ = U_دننی_پروګرام_ERROR;
        أرجع;
    }
    U_ASSERT(gInclusions[src].f_سېټ == تش_اشاره);

    Local_اشاره_ګر<Unicode_سېټ> incl(جديد Unicode_سېټ());
    إذا (incl.isNull()) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        سېټ_جمع_کړه,
        سېټ_جمع_کړه_Range,
        سېټ_جمع_کړه_String,
        تش_اشاره, // don't need remove()
        تش_اشاره // don't need removeRange()
    };

    بدل(src) {
    حالة UPROPS_SRC_CHAR:
        uchar_جمع_کړه_Property_Starts(&sa, &error_کوډ);
        اكسر;
    حالة UPROPS_SRC_PROPSVEC:
        upropsvec_جمع_کړه_Property_Starts(&sa, &error_کوډ);
        اكسر;
    حالة UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_جمع_کړه_Property_Starts(&sa, &error_کوډ);
        upropsvec_جمع_کړه_Property_Starts(&sa, &error_کوډ);
        اكسر;
#إذا !UCONFIG_NO_NORMALIZATION
    حالة UPROPS_SRC_CASE_AND_NORM: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کوډ);
        إذا(U_SUCCESS(error_کوډ)) {
            impl->جمع_کړه_Property_Starts(&sa, error_کوډ);
        }
        ucase_جمع_کړه_Property_Starts(&sa, &error_کوډ);
        اكسر;
    }
    حالة UPROPS_SRC_NFC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کوډ);
        إذا(U_SUCCESS(error_کوډ)) {
            impl->جمع_کړه_Property_Starts(&sa, error_کوډ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFKC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(error_کوډ);
        إذا(U_SUCCESS(error_کوډ)) {
            impl->جمع_کړه_Property_Starts(&sa, error_کوډ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFKC_CF: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(error_کوډ);
        إذا(U_SUCCESS(error_کوډ)) {
            impl->جمع_کړه_Property_Starts(&sa, error_کوډ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFC_CANON_ITER: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کوډ);
        إذا(U_SUCCESS(error_کوډ)) {
            impl->جمع_کړه_Canon_Iter_Property_Starts(&sa, error_کوډ);
        }
        اكسر;
    }
#د_که_پای
    حالة UPROPS_SRC_CASE:
        ucase_جمع_کړه_Property_Starts(&sa, &error_کوډ);
        اكسر;
    حالة UPROPS_SRC_BIDI:
        ubidi_جمع_کړه_Property_Starts(&sa, &error_کوډ);
        اكسر;
    حالة UPROPS_SRC_INPC:
    حالة UPROPS_SRC_INSC:
    حالة UPROPS_SRC_VO:
        uprops_جمع_کړه_Property_Starts(src, &sa, &error_کوډ);
        اكسر;
    حالة UPROPS_SRC_EMOJI: {
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_کوډ);
        إذا (U_SUCCESS(error_کوډ)) {
            ep->جمع_کړه_Property_Starts(&sa, error_کوډ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.جمع_کړه(sa.سېټ, 0x2FFE);
        sa.جمع_کړه(sa.سېټ, 0x2FFF + 1);
        اكسر;
    حالة UPROPS_SRC_ID_COMPAT_MATH:
    حالة UPROPS_SRC_MCM:
        uprops_جمع_کړه_Property_Starts(src, &sa, &error_کوډ);
        اكسر;
    حالة UPROPS_SRC_بلاک:
        ublock_جمع_کړه_Property_Starts(&sa, error_کوډ);
        اكسر;
    افتراضي:
        error_کوډ = U_دننی_پروګرام_ERROR;
        اكسر;
    }

    إذا (U_FAILURE(error_کوډ)) {
        أرجع;
    }
    إذا (incl->isBogus()) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].f_سېټ = incl.orphan();
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ثابت Unicode_سېټ *get_Inclusions_For_سرچینه(UProperty_سرچینه src, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    إذا (src < 0 || UPROPS_SRC_شمېر <= src) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, error_کوډ);
    أرجع i.f_سېټ;
}

فراغ U_CALLCONV initIntPropInclusion(UProperty prop, UError_کوډ &error_کوډ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_پیل <= prop && prop < UCHAR_INT_حد);
    int32_t incl_شاخص = UPROPS_SRC_شمېر + (prop - UCHAR_INT_پیل);
    U_ASSERT(gInclusions[incl_شاخص].f_سېټ == تش_اشاره);
    UProperty_سرچینه src = uprops_get_سرچینه(prop);
    ثابت Unicode_سېټ *incl = get_Inclusions_For_سرچینه(src, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) {
        أرجع;
    }

    Local_اشاره_ګر<Unicode_سېټ> intPropIncl(جديد Unicode_سېټ(0, 0));
    إذا (intPropIncl.isNull()) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }
    int32_t عدد_Ranges = incl->get_Range_شمېر();
    int32_t prev_ارزښت = 0;
    لكل (int32_t i = 0; i < عدد_Ranges; ++i) {
        UChar32 range_پای_ټکی = incl->get_Range_پای_ټکی(i);
        لكل (UChar32 c = incl->get_Range_پیل(i); c <= range_پای_ټکی; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_ارزښت(c, prop);
            إذا (value != prev_ارزښت) {
                intPropIncl->جمع_کړه(c);
                prev_ارزښت = value;
            }
        }
    }

    إذا (intPropIncl->isBogus()) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[incl_شاخص].f_سېټ = intPropIncl.orphan();
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت Unicode_سېټ *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    إذا (UCHAR_INT_پیل <= prop && prop < UCHAR_INT_حد) {
        int32_t incl_شاخص = UPROPS_SRC_شمېر + (prop - UCHAR_INT_پیل);
        Inclusion &i = gInclusions[incl_شاخص];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, error_کوډ);
        أرجع i.f_سېټ;
    } وإلا {
        UProperty_سرچینه src = uprops_get_سرچینه(prop);
        أرجع get_Inclusions_For_سرچینه(src, error_کوډ);
    }
}

U_NAMESPACE_پای_ټکی

نطاق {

Unicode_سېټ *make_سېټ(UProperty property, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    Local_اشاره_ګر<Unicode_سېټ> سېټ(جديد Unicode_سېټ());
    إذا (سېټ.isNull()) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    إذا (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
        USetAdder sa = {
            reinterpret_cast<USet*>(سېټ.getAlias()),
            سېټ_جمع_کړه,
            سېټ_جمع_کړه_Range,
            سېټ_جمع_کړه_String,
            تش_اشاره, // don't need remove()
            تش_اشاره // don't need removeRange()
        };
        ep->جمع_کړه_مزي_ګانې(&sa, property, error_کوډ);
        إذا (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            سېټ->freeze();
            أرجع سېټ.orphan();
        }
    }

    ثابت Unicode_سېټ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    int32_t عدد_Ranges = inclusions->get_Range_شمېر();
    UChar32 پیل_Has_Property = -1;

    لكل (int32_t i = 0; i < عدد_Ranges; ++i) {
        UChar32 range_پای_ټکی = inclusions->get_Range_پای_ټکی(i);
        لكل (UChar32 c = inclusions->get_Range_پیل(i); c <= range_پای_ټکی; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            إذا (u_has_دوګونی_Property(c, property)) {
                إذا (پیل_Has_Property < 0) {
                    // Transition from false to true.
                    پیل_Has_Property = c;
                }
            } وإلا إذا (پیل_Has_Property >= 0) {
                // Transition from true to false.
                سېټ->جمع_کړه(پیل_Has_Property, c - 1);
                پیل_Has_Property = -1;
            }
        }
    }
    إذا (پیل_Has_Property >= 0) {
        سېټ->جمع_کړه(پیل_Has_Property, 0x10FFFF);
    }
    سېټ->freeze();
    أرجع سېټ.orphan();
}

UCPMap *makeMap(UProperty property, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    uint32_t null_ارزښت = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::Local_UMutable_CPTrie_اشاره_ګر mutableTrie(
        umutablecptrie_open(null_ارزښت, null_ارزښت, &error_کوډ));
    ثابت Unicode_سېټ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    int32_t عدد_Ranges = inclusions->get_Range_شمېر();
    UChar32 پیل = 0;
    uint32_t value = null_ارزښت;

    لكل (int32_t i = 0; i < عدد_Ranges; ++i) {
        UChar32 range_پای_ټکی = inclusions->get_Range_پای_ټکی(i);
        لكل (UChar32 c = inclusions->get_Range_پیل(i); c <= range_پای_ټکی; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t بل_ارزښت = u_get_Int_Property_ارزښت(c, property);
            إذا (value != بل_ارزښت) {
                إذا (value != null_ارزښت) {
                    umutablecptrie_سېټ_Range(mutableTrie.getAlias(), پیل, c - 1, value, &error_کوډ);
                }
                پیل = c;
                value = بل_ارزښت;
            }
        }
    }
    إذا (value != 0) {
        umutablecptrie_سېټ_Range(mutableTrie.getAlias(), پیل, 0x10FFFF, value, &error_کوډ);
    }

    UCPTrieType type;
    إذا (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } وإلا {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_ارزښت_پلنوالی ارزښت_پلنوالی;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_ارزښت(property);
    إذا (max <= 0xff) {
        ارزښت_پلنوالی = UCPTRIE_ارزښت_بټونه_8;
    } وإلا إذا (max <= 0xffff) {
        ارزښت_پلنوالی = UCPTRIE_ارزښت_بټونه_16;
    } وإلا {
        ارزښت_پلنوالی = UCPTRIE_ارزښت_بټونه_32;
    }
    أرجع reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, ارزښت_پلنوالی, &error_کوډ));
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت Unicode_سېټ *CharacterProperties::get_دوګونی_Property_سېټ(UProperty property, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    إذا (property < 0 || UCHAR_دوګونی_حد <= property) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    Mutex m(&cpMutex);
    Unicode_سېټ *سېټ = sets[property];
    إذا (سېټ == تش_اشاره) {
        sets[property] = سېټ = make_سېټ(property, error_کوډ);
    }
    أرجع سېټ;
}

U_NAMESPACE_پای_ټکی

U_NAMESPACE_USE

U_CAPI ثابت USet * U_EXPORT2
u_get_دوګونی_Property_سېټ(UProperty property, UError_کوډ *p_Error_کوډ) {
    ثابت Unicode_سېټ *سېټ = CharacterProperties::get_دوګونی_Property_سېټ(property, *p_Error_کوډ);
    أرجع U_SUCCESS(*p_Error_کوډ) ? سېټ->منزل_USet() : تش_اشاره;
}

U_CAPI ثابت UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) { أرجع تش_اشاره; }
    إذا (property < UCHAR_INT_پیل || UCHAR_INT_حد <= property) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_پیل];
    إذا (map == تش_اشاره) {
        maps[property - UCHAR_INT_پیل] = map = makeMap(property, *p_Error_کوډ);
    }
    أرجع map;
}
