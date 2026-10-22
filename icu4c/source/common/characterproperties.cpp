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

استعمال icu::Local_اشارو;
#إذا !UCONFIG_NO_NORMALIZATION
استعمال icu::Normalizer2Factory;
استعمال icu::Normalizer2Impl;
#جيڪڏهن_پورو
استعمال icu::UInitOnce;
استعمال icu::Unicode_سيٽ;

نطاق {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t انگ_INCLUSIONS = UPROPS_SRC_ڳڻپ + (UCHAR_INT_حد_بندي - UCHAR_INT_شروعات);

بنية Inclusion {
    Unicode_سيٽ  *f_سيٽ = خالي_اشارو;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[انگ_INCLUSIONS]; // cached getInclusions()

Unicode_سيٽ *sets[UCHAR_ٻٽو_حد_بندي] = {};

UCPMap *maps[UCHAR_INT_حد_بندي - UCHAR_INT_شروعات] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
فراغ U_CALLCONV
سيٽ_جوڙيو(USet *سيٽ, UChar32 c) {
    reinterpret_cast<Unicode_سيٽ*>(سيٽ)->جوڙيو(c);
}

فراغ U_CALLCONV
سيٽ_جوڙيو_Range(USet *سيٽ, UChar32 شروعات, UChar32 پڇاڙي) {
    reinterpret_cast<Unicode_سيٽ*>(سيٽ)->جوڙيو(شروعات, پڇاڙي);
}

فراغ U_CALLCONV
سيٽ_جوڙيو_String(USet *سيٽ, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_سيٽ*>(سيٽ)->جوڙيو(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    لكل (Inclusion &in: gInclusions) {
        ميٽيو in.f_سيٽ;
        in.f_سيٽ = خالي_اشارو;
        in.fInitOnce.reset();
    }
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        ميٽيو sets[i];
        sets[i] = خالي_اشارو;
    }
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = خالي_اشارو;
    }
    أرجع سچ;
}

فراغ U_CALLCONV initInclusion(UProperty_ذريعو src, UError_ڪوڊ &error_ڪوڊ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_ڳڻپ);
    إذا (src == UPROPS_SRC_NONE) {
        error_ڪوڊ = U_اندروني_پروگرام_ERROR;
        أرجع;
    }
    U_ASSERT(gInclusions[src].f_سيٽ == خالي_اشارو);

    Local_اشارو<Unicode_سيٽ> incl(جديد Unicode_سيٽ());
    إذا (incl.isNull()) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        سيٽ_جوڙيو,
        سيٽ_جوڙيو_Range,
        سيٽ_جوڙيو_String,
        خالي_اشارو, // don't need remove()
        خالي_اشارو // don't need removeRange()
    };

    بدل(src) {
    حالة UPROPS_SRC_CHAR:
        uchar_جوڙيو_Property_Starts(&sa, &error_ڪوڊ);
        اكسر;
    حالة UPROPS_SRC_PROPSVEC:
        upropsvec_جوڙيو_Property_Starts(&sa, &error_ڪوڊ);
        اكسر;
    حالة UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_جوڙيو_Property_Starts(&sa, &error_ڪوڊ);
        upropsvec_جوڙيو_Property_Starts(&sa, &error_ڪوڊ);
        اكسر;
#إذا !UCONFIG_NO_NORMALIZATION
    حالة UPROPS_SRC_CASE_AND_NORM: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ڪوڊ);
        إذا(U_SUCCESS(error_ڪوڊ)) {
            impl->جوڙيو_Property_Starts(&sa, error_ڪوڊ);
        }
        ucase_جوڙيو_Property_Starts(&sa, &error_ڪوڊ);
        اكسر;
    }
    حالة UPROPS_SRC_NFC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ڪوڊ);
        إذا(U_SUCCESS(error_ڪوڊ)) {
            impl->جوڙيو_Property_Starts(&sa, error_ڪوڊ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFKC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(error_ڪوڊ);
        إذا(U_SUCCESS(error_ڪوڊ)) {
            impl->جوڙيو_Property_Starts(&sa, error_ڪوڊ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFKC_CF: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(error_ڪوڊ);
        إذا(U_SUCCESS(error_ڪوڊ)) {
            impl->جوڙيو_Property_Starts(&sa, error_ڪوڊ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFC_CANON_ITER: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ڪوڊ);
        إذا(U_SUCCESS(error_ڪوڊ)) {
            impl->جوڙيو_Canon_Iter_Property_Starts(&sa, error_ڪوڊ);
        }
        اكسر;
    }
#جيڪڏهن_پورو
    حالة UPROPS_SRC_CASE:
        ucase_جوڙيو_Property_Starts(&sa, &error_ڪوڊ);
        اكسر;
    حالة UPROPS_SRC_BIDI:
        ubidi_جوڙيو_Property_Starts(&sa, &error_ڪوڊ);
        اكسر;
    حالة UPROPS_SRC_INPC:
    حالة UPROPS_SRC_INSC:
    حالة UPROPS_SRC_VO:
        uprops_جوڙيو_Property_Starts(src, &sa, &error_ڪوڊ);
        اكسر;
    حالة UPROPS_SRC_EMOJI: {
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_ڪوڊ);
        إذا (U_SUCCESS(error_ڪوڊ)) {
            ep->جوڙيو_Property_Starts(&sa, error_ڪوڊ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.جوڙيو(sa.سيٽ, 0x2FFE);
        sa.جوڙيو(sa.سيٽ, 0x2FFF + 1);
        اكسر;
    حالة UPROPS_SRC_ID_COMPAT_MATH:
    حالة UPROPS_SRC_MCM:
        uprops_جوڙيو_Property_Starts(src, &sa, &error_ڪوڊ);
        اكسر;
    حالة UPROPS_SRC_بلاڪ:
        ublock_جوڙيو_Property_Starts(&sa, error_ڪوڊ);
        اكسر;
    افتراضي:
        error_ڪوڊ = U_اندروني_پروگرام_ERROR;
        اكسر;
    }

    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    إذا (incl->isBogus()) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].f_سيٽ = incl.orphan();
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ثابت Unicode_سيٽ *get_Inclusions_For_ذريعو(UProperty_ذريعو src, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    إذا (src < 0 || UPROPS_SRC_ڳڻپ <= src) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, error_ڪوڊ);
    أرجع i.f_سيٽ;
}

فراغ U_CALLCONV initIntPropInclusion(UProperty prop, UError_ڪوڊ &error_ڪوڊ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_شروعات <= prop && prop < UCHAR_INT_حد_بندي);
    int32_t incl_اشاريو = UPROPS_SRC_ڳڻپ + (prop - UCHAR_INT_شروعات);
    U_ASSERT(gInclusions[incl_اشاريو].f_سيٽ == خالي_اشارو);
    UProperty_ذريعو src = uprops_get_ذريعو(prop);
    ثابت Unicode_سيٽ *incl = get_Inclusions_For_ذريعو(src, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }

    Local_اشارو<Unicode_سيٽ> intPropIncl(جديد Unicode_سيٽ(0, 0));
    إذا (intPropIncl.isNull()) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    int32_t انگ_Ranges = incl->get_Range_ڳڻپ();
    int32_t prev_قدر = 0;
    لكل (int32_t i = 0; i < انگ_Ranges; ++i) {
        UChar32 range_پڇاڙي = incl->get_Range_پڇاڙي(i);
        لكل (UChar32 c = incl->get_Range_شروعات(i); c <= range_پڇاڙي; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_قدر(c, prop);
            إذا (value != prev_قدر) {
                intPropIncl->جوڙيو(c);
                prev_قدر = value;
            }
        }
    }

    إذا (intPropIncl->isBogus()) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[incl_اشاريو].f_سيٽ = intPropIncl.orphan();
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت Unicode_سيٽ *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    إذا (UCHAR_INT_شروعات <= prop && prop < UCHAR_INT_حد_بندي) {
        int32_t incl_اشاريو = UPROPS_SRC_ڳڻپ + (prop - UCHAR_INT_شروعات);
        Inclusion &i = gInclusions[incl_اشاريو];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, error_ڪوڊ);
        أرجع i.f_سيٽ;
    } وإلا {
        UProperty_ذريعو src = uprops_get_ذريعو(prop);
        أرجع get_Inclusions_For_ذريعو(src, error_ڪوڊ);
    }
}

U_NAMESPACE_پڇاڙي

نطاق {

Unicode_سيٽ *make_سيٽ(UProperty property, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    Local_اشارو<Unicode_سيٽ> سيٽ(جديد Unicode_سيٽ());
    إذا (سيٽ.isNull()) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    إذا (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
        USetAdder sa = {
            reinterpret_cast<USet*>(سيٽ.getAlias()),
            سيٽ_جوڙيو,
            سيٽ_جوڙيو_Range,
            سيٽ_جوڙيو_String,
            خالي_اشارو, // don't need remove()
            خالي_اشارو // don't need removeRange()
        };
        ep->جوڙيو_لڙيون(&sa, property, error_ڪوڊ);
        إذا (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            سيٽ->freeze();
            أرجع سيٽ.orphan();
        }
    }

    ثابت Unicode_سيٽ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    int32_t انگ_Ranges = inclusions->get_Range_ڳڻپ();
    UChar32 شروعات_Has_Property = -1;

    لكل (int32_t i = 0; i < انگ_Ranges; ++i) {
        UChar32 range_پڇاڙي = inclusions->get_Range_پڇاڙي(i);
        لكل (UChar32 c = inclusions->get_Range_شروعات(i); c <= range_پڇاڙي; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            إذا (u_has_ٻٽو_Property(c, property)) {
                إذا (شروعات_Has_Property < 0) {
                    // Transition from false to true.
                    شروعات_Has_Property = c;
                }
            } وإلا إذا (شروعات_Has_Property >= 0) {
                // Transition from true to false.
                سيٽ->جوڙيو(شروعات_Has_Property, c - 1);
                شروعات_Has_Property = -1;
            }
        }
    }
    إذا (شروعات_Has_Property >= 0) {
        سيٽ->جوڙيو(شروعات_Has_Property, 0x10FFFF);
    }
    سيٽ->freeze();
    أرجع سيٽ.orphan();
}

UCPMap *makeMap(UProperty property, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    uint32_t null_قدر = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::Local_UMutable_CPTrie_اشارو mutableTrie(
        umutablecptrie_open(null_قدر, null_قدر, &error_ڪوڊ));
    ثابت Unicode_سيٽ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    int32_t انگ_Ranges = inclusions->get_Range_ڳڻپ();
    UChar32 شروعات = 0;
    uint32_t value = null_قدر;

    لكل (int32_t i = 0; i < انگ_Ranges; ++i) {
        UChar32 range_پڇاڙي = inclusions->get_Range_پڇاڙي(i);
        لكل (UChar32 c = inclusions->get_Range_شروعات(i); c <= range_پڇاڙي; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t ايندڙ_قدر = u_get_Int_Property_قدر(c, property);
            إذا (value != ايندڙ_قدر) {
                إذا (value != null_قدر) {
                    umutablecptrie_سيٽ_Range(mutableTrie.getAlias(), شروعات, c - 1, value, &error_ڪوڊ);
                }
                شروعات = c;
                value = ايندڙ_قدر;
            }
        }
    }
    إذا (value != 0) {
        umutablecptrie_سيٽ_Range(mutableTrie.getAlias(), شروعات, 0x10FFFF, value, &error_ڪوڊ);
    }

    UCPTrieType type;
    إذا (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } وإلا {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_قدر_ويڪر قدر_ويڪر;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_قدر(property);
    إذا (max <= 0xff) {
        قدر_ويڪر = UCPTRIE_قدر_بٽ_8;
    } وإلا إذا (max <= 0xffff) {
        قدر_ويڪر = UCPTRIE_قدر_بٽ_16;
    } وإلا {
        قدر_ويڪر = UCPTRIE_قدر_بٽ_32;
    }
    أرجع reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, قدر_ويڪر, &error_ڪوڊ));
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت Unicode_سيٽ *CharacterProperties::get_ٻٽو_Property_سيٽ(UProperty property, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    إذا (property < 0 || UCHAR_ٻٽو_حد_بندي <= property) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    Mutex m(&cpMutex);
    Unicode_سيٽ *سيٽ = sets[property];
    إذا (سيٽ == خالي_اشارو) {
        sets[property] = سيٽ = make_سيٽ(property, error_ڪوڊ);
    }
    أرجع سيٽ;
}

U_NAMESPACE_پڇاڙي

U_NAMESPACE_USE

U_CAPI ثابت USet * U_EXPORT2
u_get_ٻٽو_Property_سيٽ(UProperty property, UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت Unicode_سيٽ *سيٽ = CharacterProperties::get_ٻٽو_Property_سيٽ(property, *p_Error_ڪوڊ);
    أرجع U_SUCCESS(*p_Error_ڪوڊ) ? سيٽ->منزل_USet() : خالي_اشارو;
}

U_CAPI ثابت UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) { أرجع خالي_اشارو; }
    إذا (property < UCHAR_INT_شروعات || UCHAR_INT_حد_بندي <= property) {
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_شروعات];
    إذا (map == خالي_اشارو) {
        maps[property - UCHAR_INT_شروعات] = map = makeMap(property, *p_Error_ڪوڊ);
    }
    أرجع map;
}
