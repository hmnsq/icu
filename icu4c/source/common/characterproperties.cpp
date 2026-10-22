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

ورتوں icu::Local_اشارہ_گر;
#إذا !UCONFIG_NO_NORMALIZATION
ورتوں icu::Normalizer2Factory;
ورتوں icu::Normalizer2Impl;
#جے_انت
ورتوں icu::UInitOnce;
ورتوں icu::Unicode_سیٹ;

نطاق {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t عدد_INCLUSIONS = UPROPS_SRC_گنتی + (UCHAR_INT_حد_بندی - UCHAR_INT_شروع);

بنية Inclusion {
    Unicode_سیٹ  *f_سیٹ = خالی_سوچک;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[عدد_INCLUSIONS]; // cached getInclusions()

Unicode_سیٹ *sets[UCHAR_ثنائی_حد_بندی] = {};

UCPMap *maps[UCHAR_INT_حد_بندی - UCHAR_INT_شروع] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
فراغ U_CALLCONV
سیٹ_جوڑو(USet *سیٹ, UChar32 c) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(c);
}

فراغ U_CALLCONV
سیٹ_جوڑو_Range(USet *سیٹ, UChar32 شروع, UChar32 سرا) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(شروع, سرا);
}

فراغ U_CALLCONV
سیٹ_جوڑو_String(USet *سیٹ, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    لكل (Inclusion &in: gInclusions) {
        مٹاؤ in.f_سیٹ;
        in.f_سیٹ = خالی_سوچک;
        in.fInitOnce.reset();
    }
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        مٹاؤ sets[i];
        sets[i] = خالی_سوچک;
    }
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = خالی_سوچک;
    }
    أرجع سچ;
}

فراغ U_CALLCONV initInclusion(UProperty_سروت src, UError_کوڈ &error_کوڈ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_گنتی);
    إذا (src == UPROPS_SRC_NONE) {
        error_کوڈ = U_اندرونی_پروگرام_ERROR;
        أرجع;
    }
    U_ASSERT(gInclusions[src].f_سیٹ == خالی_سوچک);

    Local_اشارہ_گر<Unicode_سیٹ> incl(جديد Unicode_سیٹ());
    إذا (incl.isNull()) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        سیٹ_جوڑو,
        سیٹ_جوڑو_Range,
        سیٹ_جوڑو_String,
        خالی_سوچک, // don't need remove()
        خالی_سوچک // don't need removeRange()
    };

    بدل(src) {
    حالة UPROPS_SRC_CHAR:
        uchar_جوڑو_Property_Starts(&sa, &error_کوڈ);
        اكسر;
    حالة UPROPS_SRC_PROPSVEC:
        upropsvec_جوڑو_Property_Starts(&sa, &error_کوڈ);
        اكسر;
    حالة UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_جوڑو_Property_Starts(&sa, &error_کوڈ);
        upropsvec_جوڑو_Property_Starts(&sa, &error_کوڈ);
        اكسر;
#إذا !UCONFIG_NO_NORMALIZATION
    حالة UPROPS_SRC_CASE_AND_NORM: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کوڈ);
        إذا(U_SUCCESS(error_کوڈ)) {
            impl->جوڑو_Property_Starts(&sa, error_کوڈ);
        }
        ucase_جوڑو_Property_Starts(&sa, &error_کوڈ);
        اكسر;
    }
    حالة UPROPS_SRC_NFC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کوڈ);
        إذا(U_SUCCESS(error_کوڈ)) {
            impl->جوڑو_Property_Starts(&sa, error_کوڈ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFKC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(error_کوڈ);
        إذا(U_SUCCESS(error_کوڈ)) {
            impl->جوڑو_Property_Starts(&sa, error_کوڈ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFKC_CF: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(error_کوڈ);
        إذا(U_SUCCESS(error_کوڈ)) {
            impl->جوڑو_Property_Starts(&sa, error_کوڈ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFC_CANON_ITER: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_کوڈ);
        إذا(U_SUCCESS(error_کوڈ)) {
            impl->جوڑو_Canon_Iter_Property_Starts(&sa, error_کوڈ);
        }
        اكسر;
    }
#جے_انت
    حالة UPROPS_SRC_CASE:
        ucase_جوڑو_Property_Starts(&sa, &error_کوڈ);
        اكسر;
    حالة UPROPS_SRC_BIDI:
        ubidi_جوڑو_Property_Starts(&sa, &error_کوڈ);
        اكسر;
    حالة UPROPS_SRC_INPC:
    حالة UPROPS_SRC_INSC:
    حالة UPROPS_SRC_VO:
        uprops_جوڑو_Property_Starts(src, &sa, &error_کوڈ);
        اكسر;
    حالة UPROPS_SRC_EMOJI: {
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_کوڈ);
        إذا (U_SUCCESS(error_کوڈ)) {
            ep->جوڑو_Property_Starts(&sa, error_کوڈ);
        }
        اكسر;
    }
    حالة UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.جوڑو(sa.سیٹ, 0x2FFE);
        sa.جوڑو(sa.سیٹ, 0x2FFF + 1);
        اكسر;
    حالة UPROPS_SRC_ID_COMPAT_MATH:
    حالة UPROPS_SRC_MCM:
        uprops_جوڑو_Property_Starts(src, &sa, &error_کوڈ);
        اكسر;
    حالة UPROPS_SRC_بلاک:
        ublock_جوڑو_Property_Starts(&sa, error_کوڈ);
        اكسر;
    افتراضي:
        error_کوڈ = U_اندرونی_پروگرام_ERROR;
        اكسر;
    }

    إذا (U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    إذا (incl->isBogus()) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].f_سیٹ = incl.orphan();
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ثابت Unicode_سیٹ *get_Inclusions_For_سروت(UProperty_سروت src, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    إذا (src < 0 || UPROPS_SRC_گنتی <= src) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, error_کوڈ);
    أرجع i.f_سیٹ;
}

فراغ U_CALLCONV initIntPropInclusion(UProperty prop, UError_کوڈ &error_کوڈ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_شروع <= prop && prop < UCHAR_INT_حد_بندی);
    int32_t incl_اشاریہ = UPROPS_SRC_گنتی + (prop - UCHAR_INT_شروع);
    U_ASSERT(gInclusions[incl_اشاریہ].f_سیٹ == خالی_سوچک);
    UProperty_سروت src = uprops_get_سروت(prop);
    ثابت Unicode_سیٹ *incl = get_Inclusions_For_سروت(src, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع;
    }

    Local_اشارہ_گر<Unicode_سیٹ> intPropIncl(جديد Unicode_سیٹ(0, 0));
    إذا (intPropIncl.isNull()) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    int32_t عدد_Ranges = incl->get_Range_گنتی();
    int32_t prev_قدر = 0;
    لكل (int32_t i = 0; i < عدد_Ranges; ++i) {
        UChar32 range_سرا = incl->get_Range_سرا(i);
        لكل (UChar32 c = incl->get_Range_شروع(i); c <= range_سرا; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_قدر(c, prop);
            إذا (value != prev_قدر) {
                intPropIncl->جوڑو(c);
                prev_قدر = value;
            }
        }
    }

    إذا (intPropIncl->isBogus()) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[incl_اشاریہ].f_سیٹ = intPropIncl.orphan();
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت Unicode_سیٹ *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    إذا (UCHAR_INT_شروع <= prop && prop < UCHAR_INT_حد_بندی) {
        int32_t incl_اشاریہ = UPROPS_SRC_گنتی + (prop - UCHAR_INT_شروع);
        Inclusion &i = gInclusions[incl_اشاریہ];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, error_کوڈ);
        أرجع i.f_سیٹ;
    } وإلا {
        UProperty_سروت src = uprops_get_سروت(prop);
        أرجع get_Inclusions_For_سروت(src, error_کوڈ);
    }
}

U_NAMESPACE_سرا

نطاق {

Unicode_سیٹ *make_سیٹ(UProperty property, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    Local_اشارہ_گر<Unicode_سیٹ> سیٹ(جديد Unicode_سیٹ());
    إذا (سیٹ.isNull()) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    إذا (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
        USetAdder sa = {
            reinterpret_cast<USet*>(سیٹ.getAlias()),
            سیٹ_جوڑو,
            سیٹ_جوڑو_Range,
            سیٹ_جوڑو_String,
            خالی_سوچک, // don't need remove()
            خالی_سوچک // don't need removeRange()
        };
        ep->جوڑو_لڑیاں(&sa, property, error_کوڈ);
        إذا (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            سیٹ->freeze();
            أرجع سیٹ.orphan();
        }
    }

    ثابت Unicode_سیٹ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    int32_t عدد_Ranges = inclusions->get_Range_گنتی();
    UChar32 شروع_Has_Property = -1;

    لكل (int32_t i = 0; i < عدد_Ranges; ++i) {
        UChar32 range_سرا = inclusions->get_Range_سرا(i);
        لكل (UChar32 c = inclusions->get_Range_شروع(i); c <= range_سرا; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            إذا (u_has_ثنائی_Property(c, property)) {
                إذا (شروع_Has_Property < 0) {
                    // Transition from false to true.
                    شروع_Has_Property = c;
                }
            } وإلا إذا (شروع_Has_Property >= 0) {
                // Transition from true to false.
                سیٹ->جوڑو(شروع_Has_Property, c - 1);
                شروع_Has_Property = -1;
            }
        }
    }
    إذا (شروع_Has_Property >= 0) {
        سیٹ->جوڑو(شروع_Has_Property, 0x10FFFF);
    }
    سیٹ->freeze();
    أرجع سیٹ.orphan();
}

UCPMap *makeMap(UProperty property, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    uint32_t null_قدر = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::Local_UMutable_CPTrie_اشارہ_گر mutableTrie(
        umutablecptrie_open(null_قدر, null_قدر, &error_کوڈ));
    ثابت Unicode_سیٹ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    int32_t عدد_Ranges = inclusions->get_Range_گنتی();
    UChar32 شروع = 0;
    uint32_t value = null_قدر;

    لكل (int32_t i = 0; i < عدد_Ranges; ++i) {
        UChar32 range_سرا = inclusions->get_Range_سرا(i);
        لكل (UChar32 c = inclusions->get_Range_شروع(i); c <= range_سرا; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t اگلا_قدر = u_get_Int_Property_قدر(c, property);
            إذا (value != اگلا_قدر) {
                إذا (value != null_قدر) {
                    umutablecptrie_سیٹ_Range(mutableTrie.getAlias(), شروع, c - 1, value, &error_کوڈ);
                }
                شروع = c;
                value = اگلا_قدر;
            }
        }
    }
    إذا (value != 0) {
        umutablecptrie_سیٹ_Range(mutableTrie.getAlias(), شروع, 0x10FFFF, value, &error_کوڈ);
    }

    UCPTrieType type;
    إذا (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } وإلا {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_قدر_چوڑائی قدر_چوڑائی;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_قدر(property);
    إذا (max <= 0xff) {
        قدر_چوڑائی = UCPTRIE_قدر_بٹ_8;
    } وإلا إذا (max <= 0xffff) {
        قدر_چوڑائی = UCPTRIE_قدر_بٹ_16;
    } وإلا {
        قدر_چوڑائی = UCPTRIE_قدر_بٹ_32;
    }
    أرجع reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, قدر_چوڑائی, &error_کوڈ));
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت Unicode_سیٹ *CharacterProperties::get_ثنائی_Property_سیٹ(UProperty property, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    إذا (property < 0 || UCHAR_ثنائی_حد_بندی <= property) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    Mutex m(&cpMutex);
    Unicode_سیٹ *سیٹ = sets[property];
    إذا (سیٹ == خالی_سوچک) {
        sets[property] = سیٹ = make_سیٹ(property, error_کوڈ);
    }
    أرجع سیٹ;
}

U_NAMESPACE_سرا

U_NAMESPACE_USE

U_CAPI ثابت USet * U_EXPORT2
u_get_ثنائی_Property_سیٹ(UProperty property, UError_کوڈ *p_Error_کوڈ) {
    ثابت Unicode_سیٹ *سیٹ = CharacterProperties::get_ثنائی_Property_سیٹ(property, *p_Error_کوڈ);
    أرجع U_SUCCESS(*p_Error_کوڈ) ? سیٹ->منزل_USet() : خالی_سوچک;
}

U_CAPI ثابت UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UError_کوڈ *p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) { أرجع خالی_سوچک; }
    إذا (property < UCHAR_INT_شروع || UCHAR_INT_حد_بندی <= property) {
        *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_شروع];
    إذا (map == خالی_سوچک) {
        maps[property - UCHAR_INT_شروع] = map = makeMap(property, *p_Error_کوڈ);
    }
    أرجع map;
}
