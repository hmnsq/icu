// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localpointer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucpmap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/umutablecptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "emojiprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "normalizer2impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubidi_props.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uprops.h"

ਵਰਤੋਂ icu::Local_ਸੂਚਕ;
#ਜੇ !UCONFIG_NO_NORMALIZATION
ਵਰਤੋਂ icu::Normalizer2Factory;
ਵਰਤੋਂ icu::Normalizer2Impl;
#ਜੇ_ਅੰਤ
ਵਰਤੋਂ icu::UInitOnce;
ਵਰਤੋਂ icu::Unicode_ਸੈੱਟ;

ਨਾਮਾਕਾਸ਼ {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t ਅੰਕ_INCLUSIONS = UPROPS_SRC_ਗਿਣਤੀ + (UCHAR_INT_ਹੱਦਬੰਦੀ - UCHAR_INT_ਸ਼ੁਰੂ);

ਬਣਤਰ Inclusion {
    Unicode_ਸੈੱਟ  *f_ਸੈੱਟ = ਖਾਲੀ_ਸੂਚਕ;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[ਅੰਕ_INCLUSIONS]; // cached getInclusions()

Unicode_ਸੈੱਟ *sets[UCHAR_ਦੁਆਧਾਰੀ_ਹੱਦਬੰਦੀ] = {};

UCPMap *maps[UCHAR_INT_ਹੱਦਬੰਦੀ - UCHAR_INT_ਸ਼ੁਰੂ] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ਖਾਲੀ U_CALLCONV
ਸੈੱਟ_ਜੋੜੋ(USet *ਸੈੱਟ, UChar32 c) {
    reinterpret_cast<Unicode_ਸੈੱਟ*>(ਸੈੱਟ)->ਜੋੜੋ(c);
}

ਖਾਲੀ U_CALLCONV
ਸੈੱਟ_ਜੋੜੋ_Range(USet *ਸੈੱਟ, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) {
    reinterpret_cast<Unicode_ਸੈੱਟ*>(ਸੈੱਟ)->ਜੋੜੋ(ਸ਼ੁਰੂ, ਸਿਰਾ);
}

ਖਾਲੀ U_CALLCONV
ਸੈੱਟ_ਜੋੜੋ_String(USet *ਸੈੱਟ, ਅਚਲ char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_ਸੈੱਟ*>(ਸੈੱਟ)->ਜੋੜੋ(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    ਲਈ (Inclusion &in: gInclusions) {
        ਮਿਟਾਓ in.f_ਸੈੱਟ;
        in.f_ਸੈੱਟ = ਖਾਲੀ_ਸੂਚਕ;
        in.fInitOnce.reset();
    }
    ਲਈ (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        ਮਿਟਾਓ sets[i];
        sets[i] = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਲਈ (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ U_CALLCONV initInclusion(UProperty_ਸਰੋਤ src, UError_ਕੋਡ &error_ਕੋਡ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_ਗਿਣਤੀ);
    ਜੇ (src == UPROPS_SRC_NONE) {
        error_ਕੋਡ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
        ਵਾਪਸ;
    }
    U_ASSERT(gInclusions[src].f_ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ);

    Local_ਸੂਚਕ<Unicode_ਸੈੱਟ> incl(ਨਵਾਂ Unicode_ਸੈੱਟ());
    ਜੇ (incl.isNull()) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        ਸੈੱਟ_ਜੋੜੋ,
        ਸੈੱਟ_ਜੋੜੋ_Range,
        ਸੈੱਟ_ਜੋੜੋ_String,
        ਖਾਲੀ_ਸੂਚਕ, // don't need remove()
        ਖਾਲੀ_ਸੂਚਕ // don't need removeRange()
    };

    ਚੋਣ(src) {
    ਹਾਲਤ UPROPS_SRC_CHAR:
        uchar_ਜੋੜੋ_Property_Starts(&sa, &error_ਕੋਡ);
        ਰੋਕੋ;
    ਹਾਲਤ UPROPS_SRC_PROPSVEC:
        upropsvec_ਜੋੜੋ_Property_Starts(&sa, &error_ਕੋਡ);
        ਰੋਕੋ;
    ਹਾਲਤ UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_ਜੋੜੋ_Property_Starts(&sa, &error_ਕੋਡ);
        upropsvec_ਜੋੜੋ_Property_Starts(&sa, &error_ਕੋਡ);
        ਰੋਕੋ;
#ਜੇ !UCONFIG_NO_NORMALIZATION
    ਹਾਲਤ UPROPS_SRC_CASE_AND_NORM: {
        ਅਚਲ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ਕੋਡ);
        ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
            impl->ਜੋੜੋ_Property_Starts(&sa, error_ਕੋਡ);
        }
        ucase_ਜੋੜੋ_Property_Starts(&sa, &error_ਕੋਡ);
        ਰੋਕੋ;
    }
    ਹਾਲਤ UPROPS_SRC_NFC: {
        ਅਚਲ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ਕੋਡ);
        ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
            impl->ਜੋੜੋ_Property_Starts(&sa, error_ਕੋਡ);
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ UPROPS_SRC_NFKC: {
        ਅਚਲ Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(error_ਕੋਡ);
        ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
            impl->ਜੋੜੋ_Property_Starts(&sa, error_ਕੋਡ);
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ UPROPS_SRC_NFKC_CF: {
        ਅਚਲ Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(error_ਕੋਡ);
        ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
            impl->ਜੋੜੋ_Property_Starts(&sa, error_ਕੋਡ);
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ UPROPS_SRC_NFC_CANON_ITER: {
        ਅਚਲ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ਕੋਡ);
        ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
            impl->ਜੋੜੋ_Canon_Iter_Property_Starts(&sa, error_ਕੋਡ);
        }
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
    ਹਾਲਤ UPROPS_SRC_CASE:
        ucase_ਜੋੜੋ_Property_Starts(&sa, &error_ਕੋਡ);
        ਰੋਕੋ;
    ਹਾਲਤ UPROPS_SRC_BIDI:
        ubidi_ਜੋੜੋ_Property_Starts(&sa, &error_ਕੋਡ);
        ਰੋਕੋ;
    ਹਾਲਤ UPROPS_SRC_INPC:
    ਹਾਲਤ UPROPS_SRC_INSC:
    ਹਾਲਤ UPROPS_SRC_VO:
        uprops_ਜੋੜੋ_Property_Starts(src, &sa, &error_ਕੋਡ);
        ਰੋਕੋ;
    ਹਾਲਤ UPROPS_SRC_EMOJI: {
        ਅਚਲ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_ਕੋਡ);
        ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
            ep->ਜੋੜੋ_Property_Starts(&sa, error_ਕੋਡ);
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.ਜੋੜੋ(sa.ਸੈੱਟ, 0x2FFE);
        sa.ਜੋੜੋ(sa.ਸੈੱਟ, 0x2FFF + 1);
        ਰੋਕੋ;
    ਹਾਲਤ UPROPS_SRC_ID_COMPAT_MATH:
    ਹਾਲਤ UPROPS_SRC_MCM:
        uprops_ਜੋੜੋ_Property_Starts(src, &sa, &error_ਕੋਡ);
        ਰੋਕੋ;
    ਹਾਲਤ UPROPS_SRC_ਖੰਡ:
        ublock_ਜੋੜੋ_Property_Starts(&sa, error_ਕੋਡ);
        ਰੋਕੋ;
    ਮੂਲ:
        error_ਕੋਡ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
        ਰੋਕੋ;
    }

    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ (incl->isBogus()) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].f_ਸੈੱਟ = incl.orphan();
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ਅਚਲ Unicode_ਸੈੱਟ *get_Inclusions_For_ਸਰੋਤ(UProperty_ਸਰੋਤ src, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    ਜੇ (src < 0 || UPROPS_SRC_ਗਿਣਤੀ <= src) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, error_ਕੋਡ);
    ਵਾਪਸ i.f_ਸੈੱਟ;
}

ਖਾਲੀ U_CALLCONV initIntPropInclusion(UProperty prop, UError_ਕੋਡ &error_ਕੋਡ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_ਸ਼ੁਰੂ <= prop && prop < UCHAR_INT_ਹੱਦਬੰਦੀ);
    int32_t incl_ਸੂਚਕਾਂਕ = UPROPS_SRC_ਗਿਣਤੀ + (prop - UCHAR_INT_ਸ਼ੁਰੂ);
    U_ASSERT(gInclusions[incl_ਸੂਚਕਾਂਕ].f_ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ);
    UProperty_ਸਰੋਤ src = uprops_get_ਸਰੋਤ(prop);
    ਅਚਲ Unicode_ਸੈੱਟ *incl = get_Inclusions_For_ਸਰੋਤ(src, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    Local_ਸੂਚਕ<Unicode_ਸੈੱਟ> intPropIncl(ਨਵਾਂ Unicode_ਸੈੱਟ(0, 0));
    ਜੇ (intPropIncl.isNull()) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    int32_t ਅੰਕ_Ranges = incl->get_Range_ਗਿਣਤੀ();
    int32_t prev_ਮੁੱਲ = 0;
    ਲਈ (int32_t i = 0; i < ਅੰਕ_Ranges; ++i) {
        UChar32 range_ਸਿਰਾ = incl->get_Range_ਸਿਰਾ(i);
        ਲਈ (UChar32 c = incl->get_Range_ਸ਼ੁਰੂ(i); c <= range_ਸਿਰਾ; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_ਮੁੱਲ(c, prop);
            ਜੇ (value != prev_ਮੁੱਲ) {
                intPropIncl->ਜੋੜੋ(c);
                prev_ਮੁੱਲ = value;
            }
        }
    }

    ਜੇ (intPropIncl->isBogus()) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[incl_ਸੂਚਕਾਂਕ].f_ਸੈੱਟ = intPropIncl.orphan();
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ਅਚਲ Unicode_ਸੈੱਟ *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    ਜੇ (UCHAR_INT_ਸ਼ੁਰੂ <= prop && prop < UCHAR_INT_ਹੱਦਬੰਦੀ) {
        int32_t incl_ਸੂਚਕਾਂਕ = UPROPS_SRC_ਗਿਣਤੀ + (prop - UCHAR_INT_ਸ਼ੁਰੂ);
        Inclusion &i = gInclusions[incl_ਸੂਚਕਾਂਕ];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, error_ਕੋਡ);
        ਵਾਪਸ i.f_ਸੈੱਟ;
    } ਨਹੀਂਤਾਂ {
        UProperty_ਸਰੋਤ src = uprops_get_ਸਰੋਤ(prop);
        ਵਾਪਸ get_Inclusions_For_ਸਰੋਤ(src, error_ਕੋਡ);
    }
}

U_NAMESPACE_ਸਿਰਾ

ਨਾਮਾਕਾਸ਼ {

Unicode_ਸੈੱਟ *make_ਸੈੱਟ(UProperty property, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    Local_ਸੂਚਕ<Unicode_ਸੈੱਟ> ਸੈੱਟ(ਨਵਾਂ Unicode_ਸੈੱਟ());
    ਜੇ (ਸੈੱਟ.isNull()) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ਅਚਲ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
        USetAdder sa = {
            reinterpret_cast<USet*>(ਸੈੱਟ.getAlias()),
            ਸੈੱਟ_ਜੋੜੋ,
            ਸੈੱਟ_ਜੋੜੋ_Range,
            ਸੈੱਟ_ਜੋੜੋ_String,
            ਖਾਲੀ_ਸੂਚਕ, // don't need remove()
            ਖਾਲੀ_ਸੂਚਕ // don't need removeRange()
        };
        ep->ਜੋੜੋ_ਲੜੀਆਂ(&sa, property, error_ਕੋਡ);
        ਜੇ (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            ਸੈੱਟ->freeze();
            ਵਾਪਸ ਸੈੱਟ.orphan();
        }
    }

    ਅਚਲ Unicode_ਸੈੱਟ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    int32_t ਅੰਕ_Ranges = inclusions->get_Range_ਗਿਣਤੀ();
    UChar32 ਸ਼ੁਰੂ_Has_Property = -1;

    ਲਈ (int32_t i = 0; i < ਅੰਕ_Ranges; ++i) {
        UChar32 range_ਸਿਰਾ = inclusions->get_Range_ਸਿਰਾ(i);
        ਲਈ (UChar32 c = inclusions->get_Range_ਸ਼ੁਰੂ(i); c <= range_ਸਿਰਾ; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            ਜੇ (u_has_ਦੁਆਧਾਰੀ_Property(c, property)) {
                ਜੇ (ਸ਼ੁਰੂ_Has_Property < 0) {
                    // Transition from false to true.
                    ਸ਼ੁਰੂ_Has_Property = c;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (ਸ਼ੁਰੂ_Has_Property >= 0) {
                // Transition from true to false.
                ਸੈੱਟ->ਜੋੜੋ(ਸ਼ੁਰੂ_Has_Property, c - 1);
                ਸ਼ੁਰੂ_Has_Property = -1;
            }
        }
    }
    ਜੇ (ਸ਼ੁਰੂ_Has_Property >= 0) {
        ਸੈੱਟ->ਜੋੜੋ(ਸ਼ੁਰੂ_Has_Property, 0x10FFFF);
    }
    ਸੈੱਟ->freeze();
    ਵਾਪਸ ਸੈੱਟ.orphan();
}

UCPMap *makeMap(UProperty property, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    uint32_t null_ਮੁੱਲ = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::Local_UMutable_CPTrie_ਸੂਚਕ mutableTrie(
        umutablecptrie_open(null_ਮੁੱਲ, null_ਮੁੱਲ, &error_ਕੋਡ));
    ਅਚਲ Unicode_ਸੈੱਟ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    int32_t ਅੰਕ_Ranges = inclusions->get_Range_ਗਿਣਤੀ();
    UChar32 ਸ਼ੁਰੂ = 0;
    uint32_t value = null_ਮੁੱਲ;

    ਲਈ (int32_t i = 0; i < ਅੰਕ_Ranges; ++i) {
        UChar32 range_ਸਿਰਾ = inclusions->get_Range_ਸਿਰਾ(i);
        ਲਈ (UChar32 c = inclusions->get_Range_ਸ਼ੁਰੂ(i); c <= range_ਸਿਰਾ; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t ਅਗਲਾ_ਮੁੱਲ = u_get_Int_Property_ਮੁੱਲ(c, property);
            ਜੇ (value != ਅਗਲਾ_ਮੁੱਲ) {
                ਜੇ (value != null_ਮੁੱਲ) {
                    umutablecptrie_ਸੈੱਟ_Range(mutableTrie.getAlias(), ਸ਼ੁਰੂ, c - 1, value, &error_ਕੋਡ);
                }
                ਸ਼ੁਰੂ = c;
                value = ਅਗਲਾ_ਮੁੱਲ;
            }
        }
    }
    ਜੇ (value != 0) {
        umutablecptrie_ਸੈੱਟ_Range(mutableTrie.getAlias(), ਸ਼ੁਰੂ, 0x10FFFF, value, &error_ਕੋਡ);
    }

    UCPTrieType type;
    ਜੇ (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } ਨਹੀਂਤਾਂ {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ ਮੁੱਲ_ਚੌੜਾਈ;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_ਮੁੱਲ(property);
    ਜੇ (max <= 0xff) {
        ਮੁੱਲ_ਚੌੜਾਈ = UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8;
    } ਨਹੀਂਤਾਂ ਜੇ (max <= 0xffff) {
        ਮੁੱਲ_ਚੌੜਾਈ = UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16;
    } ਨਹੀਂਤਾਂ {
        ਮੁੱਲ_ਚੌੜਾਈ = UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_32;
    }
    ਵਾਪਸ reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, ਮੁੱਲ_ਚੌੜਾਈ, &error_ਕੋਡ));
}

}  // namespace

U_NAMESPACE_BEGIN

ਅਚਲ Unicode_ਸੈੱਟ *CharacterProperties::get_ਦੁਆਧਾਰੀ_Property_ਸੈੱਟ(UProperty property, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    ਜੇ (property < 0 || UCHAR_ਦੁਆਧਾਰੀ_ਹੱਦਬੰਦੀ <= property) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    Mutex m(&cpMutex);
    Unicode_ਸੈੱਟ *ਸੈੱਟ = sets[property];
    ਜੇ (ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ) {
        sets[property] = ਸੈੱਟ = make_ਸੈੱਟ(property, error_ਕੋਡ);
    }
    ਵਾਪਸ ਸੈੱਟ;
}

U_NAMESPACE_ਸਿਰਾ

U_NAMESPACE_USE

U_CAPI ਅਚਲ USet * U_EXPORT2
u_get_ਦੁਆਧਾਰੀ_Property_ਸੈੱਟ(UProperty property, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ Unicode_ਸੈੱਟ *ਸੈੱਟ = CharacterProperties::get_ਦੁਆਧਾਰੀ_Property_ਸੈੱਟ(property, *p_Error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(*p_Error_ਕੋਡ) ? ਸੈੱਟ->ਮੰਜ਼ਿਲ_USet() : ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI ਅਚਲ UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    ਜੇ (property < UCHAR_INT_ਸ਼ੁਰੂ || UCHAR_INT_ਹੱਦਬੰਦੀ <= property) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_ਸ਼ੁਰੂ];
    ਜੇ (map == ਖਾਲੀ_ਸੂਚਕ) {
        maps[property - UCHAR_INT_ਸ਼ੁਰੂ] = map = makeMap(property, *p_Error_ਕੋਡ);
    }
    ਵਾਪਸ map;
}
