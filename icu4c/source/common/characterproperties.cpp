// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/localpointer.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/ucpmap.h"
#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/umutablecptrie.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "emojiprops.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "normalizer2impl.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ubidi_props.h"
#అన్తర్భావ "ucase.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "uprops.h"

ఉపయోగించి icu::Local_సూచిక;
#ఒకవేళ !UCONFIG_NO_NORMALIZATION
ఉపయోగించి icu::Normalizer2Factory;
ఉపయోగించి icu::Normalizer2Impl;
#అయితే_ముగింపు
ఉపయోగించి icu::UInitOnce;
ఉపయోగించి icu::Unicode_సమితి;

నామాకాశ {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t అంకె_INCLUSIONS = UPROPS_SRC_లెక్క + (UCHAR_INT_హద్దు - UCHAR_INT_ప్రారంభం);

నిర్మాణం Inclusion {
    Unicode_సమితి  *f_సమితి = శూన్యసూచిక;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[అంకె_INCLUSIONS]; // cached getInclusions()

Unicode_సమితి *sets[UCHAR_ద్విమాన_హద్దు] = {};

UCPMap *maps[UCHAR_INT_హద్దు - UCHAR_INT_ప్రారంభం] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
శూన్య U_CALLCONV
సమితి_కూడు(USet *సమితి, UChar32 c) {
    reinterpret_cast<Unicode_సమితి*>(సమితి)->కూడు(c);
}

శూన్య U_CALLCONV
సమితి_కూడు_Range(USet *సమితి, UChar32 ప్రారంభం, UChar32 చివర) {
    reinterpret_cast<Unicode_సమితి*>(సమితి)->కూడు(ప్రారంభం, చివర);
}

శూన్య U_CALLCONV
సమితి_కూడు_String(USet *సమితి, స్థిరరాశి char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_సమితి*>(సమితి)->కూడు(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    కొరకు (Inclusion &in: gInclusions) {
        తొలగించు in.f_సమితి;
        in.f_సమితి = శూన్యసూచిక;
        in.fInitOnce.reset();
    }
    కొరకు (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        తొలగించు sets[i];
        sets[i] = శూన్యసూచిక;
    }
    కొరకు (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = శూన్యసూచిక;
    }
    తిరిగి నిజం;
}

శూన్య U_CALLCONV initInclusion(UProperty_ఆధారం src, UError_సంకేతం &error_సంకేతం) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_లెక్క);
    ఒకవేళ (src == UPROPS_SRC_NONE) {
        error_సంకేతం = U_అంతర్గత_కార్యక్రమం_ERROR;
        తిరిగి;
    }
    U_ASSERT(gInclusions[src].f_సమితి == శూన్యసూచిక);

    Local_సూచిక<Unicode_సమితి> incl(కొత్త Unicode_సమితి());
    ఒకవేళ (incl.isNull()) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        సమితి_కూడు,
        సమితి_కూడు_Range,
        సమితి_కూడు_String,
        శూన్యసూచిక, // don't need remove()
        శూన్యసూచిక // don't need removeRange()
    };

    ఎంపిక(src) {
    స్థితి UPROPS_SRC_CHAR:
        uchar_కూడు_Property_Starts(&sa, &error_సంకేతం);
        ఆపు;
    స్థితి UPROPS_SRC_PROPSVEC:
        upropsvec_కూడు_Property_Starts(&sa, &error_సంకేతం);
        ఆపు;
    స్థితి UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_కూడు_Property_Starts(&sa, &error_సంకేతం);
        upropsvec_కూడు_Property_Starts(&sa, &error_సంకేతం);
        ఆపు;
#ఒకవేళ !UCONFIG_NO_NORMALIZATION
    స్థితి UPROPS_SRC_CASE_AND_NORM: {
        స్థిరరాశి Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_సంకేతం);
        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            impl->కూడు_Property_Starts(&sa, error_సంకేతం);
        }
        ucase_కూడు_Property_Starts(&sa, &error_సంకేతం);
        ఆపు;
    }
    స్థితి UPROPS_SRC_NFC: {
        స్థిరరాశి Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_సంకేతం);
        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            impl->కూడు_Property_Starts(&sa, error_సంకేతం);
        }
        ఆపు;
    }
    స్థితి UPROPS_SRC_NFKC: {
        స్థిరరాశి Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(error_సంకేతం);
        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            impl->కూడు_Property_Starts(&sa, error_సంకేతం);
        }
        ఆపు;
    }
    స్థితి UPROPS_SRC_NFKC_CF: {
        స్థిరరాశి Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(error_సంకేతం);
        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            impl->కూడు_Property_Starts(&sa, error_సంకేతం);
        }
        ఆపు;
    }
    స్థితి UPROPS_SRC_NFC_CANON_ITER: {
        స్థిరరాశి Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_సంకేతం);
        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            impl->కూడు_Canon_Iter_Property_Starts(&sa, error_సంకేతం);
        }
        ఆపు;
    }
#అయితే_ముగింపు
    స్థితి UPROPS_SRC_CASE:
        ucase_కూడు_Property_Starts(&sa, &error_సంకేతం);
        ఆపు;
    స్థితి UPROPS_SRC_BIDI:
        ubidi_కూడు_Property_Starts(&sa, &error_సంకేతం);
        ఆపు;
    స్థితి UPROPS_SRC_INPC:
    స్థితి UPROPS_SRC_INSC:
    స్థితి UPROPS_SRC_VO:
        uprops_కూడు_Property_Starts(src, &sa, &error_సంకేతం);
        ఆపు;
    స్థితి UPROPS_SRC_EMOJI: {
        స్థిరరాశి icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_సంకేతం);
        ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
            ep->కూడు_Property_Starts(&sa, error_సంకేతం);
        }
        ఆపు;
    }
    స్థితి UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.కూడు(sa.సమితి, 0x2FFE);
        sa.కూడు(sa.సమితి, 0x2FFF + 1);
        ఆపు;
    స్థితి UPROPS_SRC_ID_COMPAT_MATH:
    స్థితి UPROPS_SRC_MCM:
        uprops_కూడు_Property_Starts(src, &sa, &error_సంకేతం);
        ఆపు;
    స్థితి UPROPS_SRC_ఖండం:
        ublock_కూడు_Property_Starts(&sa, error_సంకేతం);
        ఆపు;
    అప్రమేయ:
        error_సంకేతం = U_అంతర్గత_కార్యక్రమం_ERROR;
        ఆపు;
    }

    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ (incl->isBogus()) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].f_సమితి = incl.orphan();
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

స్థిరరాశి Unicode_సమితి *get_Inclusions_For_ఆధారం(UProperty_ఆధారం src, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    ఒకవేళ (src < 0 || UPROPS_SRC_లెక్క <= src) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, error_సంకేతం);
    తిరిగి i.f_సమితి;
}

శూన్య U_CALLCONV initIntPropInclusion(UProperty prop, UError_సంకేతం &error_సంకేతం) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_ప్రారంభం <= prop && prop < UCHAR_INT_హద్దు);
    int32_t incl_సూచీ = UPROPS_SRC_లెక్క + (prop - UCHAR_INT_ప్రారంభం);
    U_ASSERT(gInclusions[incl_సూచీ].f_సమితి == శూన్యసూచిక);
    UProperty_ఆధారం src = uprops_get_ఆధారం(prop);
    స్థిరరాశి Unicode_సమితి *incl = get_Inclusions_For_ఆధారం(src, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }

    Local_సూచిక<Unicode_సమితి> intPropIncl(కొత్త Unicode_సమితి(0, 0));
    ఒకవేళ (intPropIncl.isNull()) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    int32_t అంకె_Ranges = incl->get_Range_లెక్క();
    int32_t prev_విలువ = 0;
    కొరకు (int32_t i = 0; i < అంకె_Ranges; ++i) {
        UChar32 range_చివర = incl->get_Range_చివర(i);
        కొరకు (UChar32 c = incl->get_Range_ప్రారంభం(i); c <= range_చివర; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_విలువ(c, prop);
            ఒకవేళ (value != prev_విలువ) {
                intPropIncl->కూడు(c);
                prev_విలువ = value;
            }
        }
    }

    ఒకవేళ (intPropIncl->isBogus()) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[incl_సూచీ].f_సమితి = intPropIncl.orphan();
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

స్థిరరాశి Unicode_సమితి *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    ఒకవేళ (UCHAR_INT_ప్రారంభం <= prop && prop < UCHAR_INT_హద్దు) {
        int32_t incl_సూచీ = UPROPS_SRC_లెక్క + (prop - UCHAR_INT_ప్రారంభం);
        Inclusion &i = gInclusions[incl_సూచీ];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, error_సంకేతం);
        తిరిగి i.f_సమితి;
    } లేకపోతే {
        UProperty_ఆధారం src = uprops_get_ఆధారం(prop);
        తిరిగి get_Inclusions_For_ఆధారం(src, error_సంకేతం);
    }
}

U_NAMESPACE_చివర

నామాకాశ {

Unicode_సమితి *make_సమితి(UProperty property, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    Local_సూచిక<Unicode_సమితి> సమితి(కొత్త Unicode_సమితి());
    ఒకవేళ (సమితి.isNull()) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        స్థిరరాశి icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
        USetAdder sa = {
            reinterpret_cast<USet*>(సమితి.getAlias()),
            సమితి_కూడు,
            సమితి_కూడు_Range,
            సమితి_కూడు_String,
            శూన్యసూచిక, // don't need remove()
            శూన్యసూచిక // don't need removeRange()
        };
        ep->కూడు_అక్షరశ్రేణులు(&sa, property, error_సంకేతం);
        ఒకవేళ (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            సమితి->freeze();
            తిరిగి సమితి.orphan();
        }
    }

    స్థిరరాశి Unicode_సమితి *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    int32_t అంకె_Ranges = inclusions->get_Range_లెక్క();
    UChar32 ప్రారంభం_Has_Property = -1;

    కొరకు (int32_t i = 0; i < అంకె_Ranges; ++i) {
        UChar32 range_చివర = inclusions->get_Range_చివర(i);
        కొరకు (UChar32 c = inclusions->get_Range_ప్రారంభం(i); c <= range_చివర; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            ఒకవేళ (u_has_ద్విమాన_Property(c, property)) {
                ఒకవేళ (ప్రారంభం_Has_Property < 0) {
                    // Transition from false to true.
                    ప్రారంభం_Has_Property = c;
                }
            } లేకపోతే ఒకవేళ (ప్రారంభం_Has_Property >= 0) {
                // Transition from true to false.
                సమితి->కూడు(ప్రారంభం_Has_Property, c - 1);
                ప్రారంభం_Has_Property = -1;
            }
        }
    }
    ఒకవేళ (ప్రారంభం_Has_Property >= 0) {
        సమితి->కూడు(ప్రారంభం_Has_Property, 0x10FFFF);
    }
    సమితి->freeze();
    తిరిగి సమితి.orphan();
}

UCPMap *makeMap(UProperty property, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    uint32_t null_విలువ = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::Local_UMutable_CPTrie_సూచిక mutableTrie(
        umutablecptrie_open(null_విలువ, null_విలువ, &error_సంకేతం));
    స్థిరరాశి Unicode_సమితి *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    int32_t అంకె_Ranges = inclusions->get_Range_లెక్క();
    UChar32 ప్రారంభం = 0;
    uint32_t value = null_విలువ;

    కొరకు (int32_t i = 0; i < అంకె_Ranges; ++i) {
        UChar32 range_చివర = inclusions->get_Range_చివర(i);
        కొరకు (UChar32 c = inclusions->get_Range_ప్రారంభం(i); c <= range_చివర; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t తదుపరి_విలువ = u_get_Int_Property_విలువ(c, property);
            ఒకవేళ (value != తదుపరి_విలువ) {
                ఒకవేళ (value != null_విలువ) {
                    umutablecptrie_సమితి_Range(mutableTrie.getAlias(), ప్రారంభం, c - 1, value, &error_సంకేతం);
                }
                ప్రారంభం = c;
                value = తదుపరి_విలువ;
            }
        }
    }
    ఒకవేళ (value != 0) {
        umutablecptrie_సమితి_Range(mutableTrie.getAlias(), ప్రారంభం, 0x10FFFF, value, &error_సంకేతం);
    }

    UCPTrieType type;
    ఒకవేళ (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } లేకపోతే {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_విలువ_వెడల్పు విలువ_వెడల్పు;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_విలువ(property);
    ఒకవేళ (max <= 0xff) {
        విలువ_వెడల్పు = UCPTRIE_విలువ_బిట్లు_8;
    } లేకపోతే ఒకవేళ (max <= 0xffff) {
        విలువ_వెడల్పు = UCPTRIE_విలువ_బిట్లు_16;
    } లేకపోతే {
        విలువ_వెడల్పు = UCPTRIE_విలువ_బిట్లు_32;
    }
    తిరిగి reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, విలువ_వెడల్పు, &error_సంకేతం));
}

}  // namespace

U_NAMESPACE_BEGIN

స్థిరరాశి Unicode_సమితి *CharacterProperties::get_ద్విమాన_Property_సమితి(UProperty property, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    ఒకవేళ (property < 0 || UCHAR_ద్విమాన_హద్దు <= property) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    Mutex m(&cpMutex);
    Unicode_సమితి *సమితి = sets[property];
    ఒకవేళ (సమితి == శూన్యసూచిక) {
        sets[property] = సమితి = make_సమితి(property, error_సంకేతం);
    }
    తిరిగి సమితి;
}

U_NAMESPACE_చివర

U_NAMESPACE_USE

U_CAPI స్థిరరాశి USet * U_EXPORT2
u_get_ద్విమాన_Property_సమితి(UProperty property, UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి Unicode_సమితి *సమితి = CharacterProperties::get_ద్విమాన_Property_సమితి(property, *p_Error_సంకేతం);
    తిరిగి U_SUCCESS(*p_Error_సంకేతం) ? సమితి->గమ్యం_USet() : శూన్యసూచిక;
}

U_CAPI స్థిరరాశి UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    ఒకవేళ (property < UCHAR_INT_ప్రారంభం || UCHAR_INT_హద్దు <= property) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_ప్రారంభం];
    ఒకవేళ (map == శూన్యసూచిక) {
        maps[property - UCHAR_INT_ప్రారంభం] = map = makeMap(property, *p_Error_సంకేతం);
    }
    తిరిగి map;
}
