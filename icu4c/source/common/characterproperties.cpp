// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucpmap.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/umutablecptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "emojiprops.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "normalizer2impl.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ubidi_props.h"
#ಅನ್ತರ್ಭಾವ "ucase.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uprops.h"

ಬಳಸಿ icu::Local_ಸೂಚಕ;
#ಆದರೆ !UCONFIG_NO_NORMALIZATION
ಬಳಸಿ icu::Normalizer2Factory;
ಬಳಸಿ icu::Normalizer2Impl;
#ಆದರೆ_ಅಂತ್ಯ
ಬಳಸಿ icu::UInitOnce;
ಬಳಸಿ icu::Unicode_ಗಣ;

ನಾಮಾಕಾಶ {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t ಅಂಕೆ_INCLUSIONS = UPROPS_SRC_ಎಣಿಕೆ + (UCHAR_INT_ಎಲ್ಲೆ - UCHAR_INT_ಆರಂಭ);

ರಚನೆ Inclusion {
    Unicode_ಗಣ  *f_ಗಣ = ಶೂನ್ಯಸೂಚಕ;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[ಅಂಕೆ_INCLUSIONS]; // cached getInclusions()

Unicode_ಗಣ *sets[UCHAR_ದ್ವಿಮಾನ_ಎಲ್ಲೆ] = {};

UCPMap *maps[UCHAR_INT_ಎಲ್ಲೆ - UCHAR_INT_ಆರಂಭ] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ಶೂನ್ಯ U_CALLCONV
ಗಣ_ಕೂಡಿಸು(USet *ಗಣ, UChar32 c) {
    reinterpret_cast<Unicode_ಗಣ*>(ಗಣ)->ಕೂಡಿಸು(c);
}

ಶೂನ್ಯ U_CALLCONV
ಗಣ_ಕೂಡಿಸು_Range(USet *ಗಣ, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) {
    reinterpret_cast<Unicode_ಗಣ*>(ಗಣ)->ಕೂಡಿಸು(ಆರಂಭ, ಕೊನೆ);
}

ಶೂನ್ಯ U_CALLCONV
ಗಣ_ಕೂಡಿಸು_String(USet *ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_ಗಣ*>(ಗಣ)->ಕೂಡಿಸು(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    ಗಾಗಿ (Inclusion &in: gInclusions) {
        ಅಳಿಸು in.f_ಗಣ;
        in.f_ಗಣ = ಶೂನ್ಯಸೂಚಕ;
        in.fInitOnce.reset();
    }
    ಗಾಗಿ (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        ಅಳಿಸು sets[i];
        sets[i] = ಶೂನ್ಯಸೂಚಕ;
    }
    ಗಾಗಿ (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ U_CALLCONV initInclusion(UProperty_ಮೂಲ src, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_ಎಣಿಕೆ);
    ಆದರೆ (src == UPROPS_SRC_NONE) {
        error_ಸಂಕೇತ = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    U_ASSERT(gInclusions[src].f_ಗಣ == ಶೂನ್ಯಸೂಚಕ);

    Local_ಸೂಚಕ<Unicode_ಗಣ> incl(ಹೊಸ Unicode_ಗಣ());
    ಆದರೆ (incl.isNull()) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        ಗಣ_ಕೂಡಿಸು,
        ಗಣ_ಕೂಡಿಸು_Range,
        ಗಣ_ಕೂಡಿಸು_String,
        ಶೂನ್ಯಸೂಚಕ, // don't need remove()
        ಶೂನ್ಯಸೂಚಕ // don't need removeRange()
    };

    ಆಯ್ಕೆ(src) {
    ಸ್ಥಿತಿ UPROPS_SRC_CHAR:
        uchar_ಕೂಡಿಸು_Property_Starts(&sa, &error_ಸಂಕೇತ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UPROPS_SRC_PROPSVEC:
        upropsvec_ಕೂಡಿಸು_Property_Starts(&sa, &error_ಸಂಕೇತ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_ಕೂಡಿಸು_Property_Starts(&sa, &error_ಸಂಕೇತ);
        upropsvec_ಕೂಡಿಸು_Property_Starts(&sa, &error_ಸಂಕೇತ);
        ನಿಲ್ಲಿಸು;
#ಆದರೆ !UCONFIG_NO_NORMALIZATION
    ಸ್ಥಿತಿ UPROPS_SRC_CASE_AND_NORM: {
        ಸ್ಥಿರಾಂಕ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ಸಂಕೇತ);
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            impl->ಕೂಡಿಸು_Property_Starts(&sa, error_ಸಂಕೇತ);
        }
        ucase_ಕೂಡಿಸು_Property_Starts(&sa, &error_ಸಂಕೇತ);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ UPROPS_SRC_NFC: {
        ಸ್ಥಿರಾಂಕ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ಸಂಕೇತ);
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            impl->ಕೂಡಿಸು_Property_Starts(&sa, error_ಸಂಕೇತ);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ UPROPS_SRC_NFKC: {
        ಸ್ಥಿರಾಂಕ Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(error_ಸಂಕೇತ);
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            impl->ಕೂಡಿಸು_Property_Starts(&sa, error_ಸಂಕೇತ);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ UPROPS_SRC_NFKC_CF: {
        ಸ್ಥಿರಾಂಕ Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(error_ಸಂಕೇತ);
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            impl->ಕೂಡಿಸು_Property_Starts(&sa, error_ಸಂಕೇತ);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ UPROPS_SRC_NFC_CANON_ITER: {
        ಸ್ಥಿರಾಂಕ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_ಸಂಕೇತ);
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            impl->ಕೂಡಿಸು_Canon_Iter_Property_Starts(&sa, error_ಸಂಕೇತ);
        }
        ನಿಲ್ಲಿಸು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ UPROPS_SRC_CASE:
        ucase_ಕೂಡಿಸು_Property_Starts(&sa, &error_ಸಂಕೇತ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UPROPS_SRC_BIDI:
        ubidi_ಕೂಡಿಸು_Property_Starts(&sa, &error_ಸಂಕೇತ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UPROPS_SRC_INPC:
    ಸ್ಥಿತಿ UPROPS_SRC_INSC:
    ಸ್ಥಿತಿ UPROPS_SRC_VO:
        uprops_ಕೂಡಿಸು_Property_Starts(src, &sa, &error_ಸಂಕೇತ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UPROPS_SRC_EMOJI: {
        ಸ್ಥಿರಾಂಕ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_ಸಂಕೇತ);
        ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
            ep->ಕೂಡಿಸು_Property_Starts(&sa, error_ಸಂಕೇತ);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.ಕೂಡಿಸು(sa.ಗಣ, 0x2FFE);
        sa.ಕೂಡಿಸು(sa.ಗಣ, 0x2FFF + 1);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UPROPS_SRC_ID_COMPAT_MATH:
    ಸ್ಥಿತಿ UPROPS_SRC_MCM:
        uprops_ಕೂಡಿಸು_Property_Starts(src, &sa, &error_ಸಂಕೇತ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UPROPS_SRC_ಖಂಡ:
        ublock_ಕೂಡಿಸು_Property_Starts(&sa, error_ಸಂಕೇತ);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        error_ಸಂಕೇತ = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
        ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (incl->isBogus()) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].f_ಗಣ = incl.orphan();
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *get_Inclusions_For_ಮೂಲ(UProperty_ಮೂಲ src, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    ಆದರೆ (src < 0 || UPROPS_SRC_ಎಣಿಕೆ <= src) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ i.f_ಗಣ;
}

ಶೂನ್ಯ U_CALLCONV initIntPropInclusion(UProperty prop, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_ಆರಂಭ <= prop && prop < UCHAR_INT_ಎಲ್ಲೆ);
    int32_t incl_ಸೂಚ್ಯಂಕ = UPROPS_SRC_ಎಣಿಕೆ + (prop - UCHAR_INT_ಆರಂಭ);
    U_ASSERT(gInclusions[incl_ಸೂಚ್ಯಂಕ].f_ಗಣ == ಶೂನ್ಯಸೂಚಕ);
    UProperty_ಮೂಲ src = uprops_get_ಮೂಲ(prop);
    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *incl = get_Inclusions_For_ಮೂಲ(src, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    Local_ಸೂಚಕ<Unicode_ಗಣ> intPropIncl(ಹೊಸ Unicode_ಗಣ(0, 0));
    ಆದರೆ (intPropIncl.isNull()) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    int32_t ಅಂಕೆ_Ranges = incl->get_Range_ಎಣಿಕೆ();
    int32_t prev_ಮೌಲ್ಯ = 0;
    ಗಾಗಿ (int32_t i = 0; i < ಅಂಕೆ_Ranges; ++i) {
        UChar32 range_ಕೊನೆ = incl->get_Range_ಕೊನೆ(i);
        ಗಾಗಿ (UChar32 c = incl->get_Range_ಆರಂಭ(i); c <= range_ಕೊನೆ; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_ಮೌಲ್ಯ(c, prop);
            ಆದರೆ (value != prev_ಮೌಲ್ಯ) {
                intPropIncl->ಕೂಡಿಸು(c);
                prev_ಮೌಲ್ಯ = value;
            }
        }
    }

    ಆದರೆ (intPropIncl->isBogus()) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[incl_ಸೂಚ್ಯಂಕ].f_ಗಣ = intPropIncl.orphan();
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    ಆದರೆ (UCHAR_INT_ಆರಂಭ <= prop && prop < UCHAR_INT_ಎಲ್ಲೆ) {
        int32_t incl_ಸೂಚ್ಯಂಕ = UPROPS_SRC_ಎಣಿಕೆ + (prop - UCHAR_INT_ಆರಂಭ);
        Inclusion &i = gInclusions[incl_ಸೂಚ್ಯಂಕ];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ i.f_ಗಣ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        UProperty_ಮೂಲ src = uprops_get_ಮೂಲ(prop);
        ಹಿಂತಿರುಗಿ get_Inclusions_For_ಮೂಲ(src, error_ಸಂಕೇತ);
    }
}

U_NAMESPACE_ಕೊನೆ

ನಾಮಾಕಾಶ {

Unicode_ಗಣ *make_ಗಣ(UProperty property, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    Local_ಸೂಚಕ<Unicode_ಗಣ> ಗಣ(ಹೊಸ Unicode_ಗಣ());
    ಆದರೆ (ಗಣ.isNull()) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ಸ್ಥಿರಾಂಕ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
        USetAdder sa = {
            reinterpret_cast<USet*>(ಗಣ.getAlias()),
            ಗಣ_ಕೂಡಿಸು,
            ಗಣ_ಕೂಡಿಸು_Range,
            ಗಣ_ಕೂಡಿಸು_String,
            ಶೂನ್ಯಸೂಚಕ, // don't need remove()
            ಶೂನ್ಯಸೂಚಕ // don't need removeRange()
        };
        ep->ಕೂಡಿಸು_ಅಕ್ಷರಸರಣಿಗಳು(&sa, property, error_ಸಂಕೇತ);
        ಆದರೆ (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            ಗಣ->freeze();
            ಹಿಂತಿರುಗಿ ಗಣ.orphan();
        }
    }

    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    int32_t ಅಂಕೆ_Ranges = inclusions->get_Range_ಎಣಿಕೆ();
    UChar32 ಆರಂಭ_Has_Property = -1;

    ಗಾಗಿ (int32_t i = 0; i < ಅಂಕೆ_Ranges; ++i) {
        UChar32 range_ಕೊನೆ = inclusions->get_Range_ಕೊನೆ(i);
        ಗಾಗಿ (UChar32 c = inclusions->get_Range_ಆರಂಭ(i); c <= range_ಕೊನೆ; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            ಆದರೆ (u_has_ದ್ವಿಮಾನ_Property(c, property)) {
                ಆದರೆ (ಆರಂಭ_Has_Property < 0) {
                    // Transition from false to true.
                    ಆರಂಭ_Has_Property = c;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆರಂಭ_Has_Property >= 0) {
                // Transition from true to false.
                ಗಣ->ಕೂಡಿಸು(ಆರಂಭ_Has_Property, c - 1);
                ಆರಂಭ_Has_Property = -1;
            }
        }
    }
    ಆದರೆ (ಆರಂಭ_Has_Property >= 0) {
        ಗಣ->ಕೂಡಿಸು(ಆರಂಭ_Has_Property, 0x10FFFF);
    }
    ಗಣ->freeze();
    ಹಿಂತಿರುಗಿ ಗಣ.orphan();
}

UCPMap *makeMap(UProperty property, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    uint32_t null_ಮೌಲ್ಯ = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::Local_UMutable_CPTrie_ಸೂಚಕ mutableTrie(
        umutablecptrie_open(null_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ, &error_ಸಂಕೇತ));
    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    int32_t ಅಂಕೆ_Ranges = inclusions->get_Range_ಎಣಿಕೆ();
    UChar32 ಆರಂಭ = 0;
    uint32_t value = null_ಮೌಲ್ಯ;

    ಗಾಗಿ (int32_t i = 0; i < ಅಂಕೆ_Ranges; ++i) {
        UChar32 range_ಕೊನೆ = inclusions->get_Range_ಕೊನೆ(i);
        ಗಾಗಿ (UChar32 c = inclusions->get_Range_ಆರಂಭ(i); c <= range_ಕೊನೆ; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t ಮುಂದಿನ_ಮೌಲ್ಯ = u_get_Int_Property_ಮೌಲ್ಯ(c, property);
            ಆದರೆ (value != ಮುಂದಿನ_ಮೌಲ್ಯ) {
                ಆದರೆ (value != null_ಮೌಲ್ಯ) {
                    umutablecptrie_ಗಣ_Range(mutableTrie.getAlias(), ಆರಂಭ, c - 1, value, &error_ಸಂಕೇತ);
                }
                ಆರಂಭ = c;
                value = ಮುಂದಿನ_ಮೌಲ್ಯ;
            }
        }
    }
    ಆದರೆ (value != 0) {
        umutablecptrie_ಗಣ_Range(mutableTrie.getAlias(), ಆರಂಭ, 0x10FFFF, value, &error_ಸಂಕೇತ);
    }

    UCPTrieType type;
    ಆದರೆ (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } ಇಲ್ಲದಿದ್ದರೆ {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_ಮೌಲ್ಯ_ಅಗಲ ಮೌಲ್ಯ_ಅಗಲ;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_ಮೌಲ್ಯ(property);
    ಆದರೆ (max <= 0xff) {
        ಮೌಲ್ಯ_ಅಗಲ = UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (max <= 0xffff) {
        ಮೌಲ್ಯ_ಅಗಲ = UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಮೌಲ್ಯ_ಅಗಲ = UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32;
    }
    ಹಿಂತಿರುಗಿ reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, ಮೌಲ್ಯ_ಅಗಲ, &error_ಸಂಕೇತ));
}

}  // namespace

U_NAMESPACE_BEGIN

ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *CharacterProperties::get_ದ್ವಿಮಾನ_Property_ಗಣ(UProperty property, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    ಆದರೆ (property < 0 || UCHAR_ದ್ವಿಮಾನ_ಎಲ್ಲೆ <= property) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    Mutex m(&cpMutex);
    Unicode_ಗಣ *ಗಣ = sets[property];
    ಆದರೆ (ಗಣ == ಶೂನ್ಯಸೂಚಕ) {
        sets[property] = ಗಣ = make_ಗಣ(property, error_ಸಂಕೇತ);
    }
    ಹಿಂತಿರುಗಿ ಗಣ;
}

U_NAMESPACE_ಕೊನೆ

U_NAMESPACE_USE

U_CAPI ಸ್ಥಿರಾಂಕ USet * U_EXPORT2
u_get_ದ್ವಿಮಾನ_Property_ಗಣ(UProperty property, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *ಗಣ = CharacterProperties::get_ದ್ವಿಮಾನ_Property_ಗಣ(property, *p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(*p_Error_ಸಂಕೇತ) ? ಗಣ->ಗಮ್ಯ_USet() : ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI ಸ್ಥಿರಾಂಕ UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    ಆದರೆ (property < UCHAR_INT_ಆರಂಭ || UCHAR_INT_ಎಲ್ಲೆ <= property) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_ಆರಂಭ];
    ಆದರೆ (map == ಶೂನ್ಯಸೂಚಕ) {
        maps[property - UCHAR_INT_ಆರಂಭ] = map = makeMap(property, *p_Error_ಸಂಕೇತ);
    }
    ಹಿಂತಿರುಗಿ map;
}
