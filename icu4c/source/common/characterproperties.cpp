// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/localpointer.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/ucpmap.h"
#അന്തര്ഭാവ "unicode/ucptrie.h"
#അന്തര്ഭാവ "unicode/umutablecptrie.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/uscript.h"
#അന്തര്ഭാവ "unicode/uset.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "emojiprops.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "normalizer2impl.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ubidi_props.h"
#അന്തര്ഭാവ "ucase.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "uprops.h"

using icu::LocalPointer;
#എങ്കിൽ !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

നാമാകാശ {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t അക്കം_INCLUSIONS = UPROPS_SRC_എണ്ണം + (UCHAR_INT_LIMIT - UCHAR_INT_START);

ഘടന Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[അക്കം_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_ദ്വിമാന_LIMIT] = {};

UCPMap *maps[UCHAR_INT_LIMIT - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ശൂന്യം U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

ശൂന്യം U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

ശൂന്യം U_CALLCONV
_set_addString(USet *set, സ്ഥിരം char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    വേണ്ടി (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    വേണ്ടി (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    വേണ്ടി (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    തിരികെ സത്യം;
}

ശൂന്യം U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_എണ്ണം);
    എങ്കിൽ (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        തിരികെ;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(പുതിയ UnicodeSet());
    എങ്കിൽ (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    തിരഞ്ഞെടുപ്പ്(src) {
    അവസ്ഥ UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        നിർത്തുക;
    അവസ്ഥ UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        നിർത്തുക;
    അവസ്ഥ UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        നിർത്തുക;
#എങ്കിൽ !UCONFIG_NO_NORMALIZATION
    അവസ്ഥ UPROPS_SRC_CASE_AND_NORM: {
        സ്ഥിരം Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        എങ്കിൽ(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        നിർത്തുക;
    }
    അവസ്ഥ UPROPS_SRC_NFC: {
        സ്ഥിരം Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        എങ്കിൽ(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        നിർത്തുക;
    }
    അവസ്ഥ UPROPS_SRC_NFKC: {
        സ്ഥിരം Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        എങ്കിൽ(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        നിർത്തുക;
    }
    അവസ്ഥ UPROPS_SRC_NFKC_CF: {
        സ്ഥിരം Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        എങ്കിൽ(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        നിർത്തുക;
    }
    അവസ്ഥ UPROPS_SRC_NFC_CANON_ITER: {
        സ്ഥിരം Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        എങ്കിൽ(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        നിർത്തുക;
    }
#endif
    അവസ്ഥ UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        നിർത്തുക;
    അവസ്ഥ UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        നിർത്തുക;
    അവസ്ഥ UPROPS_SRC_INPC:
    അവസ്ഥ UPROPS_SRC_INSC:
    അവസ്ഥ UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        നിർത്തുക;
    അവസ്ഥ UPROPS_SRC_EMOJI: {
        സ്ഥിരം icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        എങ്കിൽ (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        നിർത്തുക;
    }
    അവസ്ഥ UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        നിർത്തുക;
    അവസ്ഥ UPROPS_SRC_ID_COMPAT_MATH:
    അവസ്ഥ UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        നിർത്തുക;
    അവസ്ഥ UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        നിർത്തുക;
    }

    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ;
    }
    എങ്കിൽ (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

സ്ഥിരം UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    എങ്കിൽ (src < 0 || UPROPS_SRC_എണ്ണം <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    തിരികെ i.fSet;
}

ശൂന്യം U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_എണ്ണം + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    സ്ഥിരം UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ;
    }

    LocalPointer<UnicodeSet> intPropIncl(പുതിയ UnicodeSet(0, 0));
    എങ്കിൽ (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    int32_t അക്കം_Ranges = incl->get_Range_എണ്ണം();
    int32_t prev_മൂല്യം = 0;
    വേണ്ടി (int32_t i = 0; i < അക്കം_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        വേണ്ടി (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_മൂല്യം(c, prop);
            എങ്കിൽ (value != prev_മൂല്യം) {
                intPropIncl->add(c);
                prev_മൂല്യം = value;
            }
        }
    }

    എങ്കിൽ (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[inclIndex].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

സ്ഥിരം UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    എങ്കിൽ (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_എണ്ണം + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        തിരികെ i.fSet;
    } അല്ലെങ്കിൽ {
        UPropertySource src = uprops_getSource(prop);
        തിരികെ getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

നാമാകാശ {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    LocalPointer<UnicodeSet> set(പുതിയ UnicodeSet());
    എങ്കിൽ (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    എങ്കിൽ (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        സ്ഥിരം icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        എങ്കിൽ (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            തിരികെ set.orphan();
        }
    }

    സ്ഥിരം UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    int32_t അക്കം_Ranges = inclusions->get_Range_എണ്ണം();
    UChar32 startHasProperty = -1;

    വേണ്ടി (int32_t i = 0; i < അക്കം_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        വേണ്ടി (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            എങ്കിൽ (u_has_ദ്വിമാന_Property(c, property)) {
                എങ്കിൽ (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    എങ്കിൽ (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    തിരികെ set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    uint32_t null_മൂല്യം = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_മൂല്യം, null_മൂല്യം, &errorCode));
    സ്ഥിരം UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    int32_t അക്കം_Ranges = inclusions->get_Range_എണ്ണം();
    UChar32 start = 0;
    uint32_t value = null_മൂല്യം;

    വേണ്ടി (int32_t i = 0; i < അക്കം_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        വേണ്ടി (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t next_മൂല്യം = u_get_Int_Property_മൂല്യം(c, property);
            എങ്കിൽ (value != next_മൂല്യം) {
                എങ്കിൽ (value != null_മൂല്യം) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = next_മൂല്യം;
            }
        }
    }
    എങ്കിൽ (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    എങ്കിൽ (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } അല്ലെങ്കിൽ {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_മൂല്യം_Width മൂല്യം_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_മൂല്യം(property);
    എങ്കിൽ (max <= 0xff) {
        മൂല്യം_Width = UCPTRIE_മൂല്യം_BITS_8;
    } അല്ലെങ്കിൽ എങ്കിൽ (max <= 0xffff) {
        മൂല്യം_Width = UCPTRIE_മൂല്യം_BITS_16;
    } അല്ലെങ്കിൽ {
        മൂല്യം_Width = UCPTRIE_മൂല്യം_BITS_32;
    }
    തിരികെ reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, മൂല്യം_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

സ്ഥിരം UnicodeSet *CharacterProperties::get_ദ്വിമാന_Property_Set(UProperty property, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    എങ്കിൽ (property < 0 || UCHAR_ദ്വിമാന_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    എങ്കിൽ (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    തിരികെ set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI സ്ഥിരം USet * U_EXPORT2
u_get_ദ്വിമാന_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    സ്ഥിരം UnicodeSet *set = CharacterProperties::get_ദ്വിമാന_Property_Set(property, *pErrorCode);
    തിരികെ U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI സ്ഥിരം UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    എങ്കിൽ (U_FAILURE(*pErrorCode)) { തിരികെ nullptr; }
    എങ്കിൽ (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    എങ്കിൽ (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    തിരികെ map;
}
