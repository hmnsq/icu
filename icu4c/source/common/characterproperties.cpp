// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/localpointer.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/ucpmap.h"
#અન્તર્ભાવ "unicode/ucptrie.h"
#અન્તર્ભાવ "unicode/umutablecptrie.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/uscript.h"
#અન્તર્ભાવ "unicode/uset.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "emojiprops.h"
#અન્તર્ભાવ "mutex.h"
#અન્તર્ભાવ "normalizer2impl.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ubidi_props.h"
#અન્તર્ભાવ "ucase.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "uprops.h"

using icu::LocalPointer;
#જો !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

નામાકાશ {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t અંક_INCLUSIONS = UPROPS_SRC_ગણતરી + (UCHAR_INT_LIMIT - UCHAR_INT_START);

સંરચના Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[અંક_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_દ્વિઅંકી_LIMIT] = {};

UCPMap *maps[UCHAR_INT_LIMIT - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
શૂન્ય U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

શૂન્ય U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

શૂન્ય U_CALLCONV
_set_addString(USet *set, અચલ char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    માટે (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    માટે (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    માટે (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    પાછા સાચું;
}

શૂન્ય U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_ગણતરી);
    જો (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        પાછા;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(નવું UnicodeSet());
    જો (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    પસંદગી(src) {
    સ્થિતિ UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        તોડો;
    સ્થિતિ UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        તોડો;
    સ્થિતિ UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        તોડો;
#જો !UCONFIG_NO_NORMALIZATION
    સ્થિતિ UPROPS_SRC_CASE_AND_NORM: {
        અચલ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        જો(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        તોડો;
    }
    સ્થિતિ UPROPS_SRC_NFC: {
        અચલ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        જો(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        તોડો;
    }
    સ્થિતિ UPROPS_SRC_NFKC: {
        અચલ Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        જો(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        તોડો;
    }
    સ્થિતિ UPROPS_SRC_NFKC_CF: {
        અચલ Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        જો(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        તોડો;
    }
    સ્થિતિ UPROPS_SRC_NFC_CANON_ITER: {
        અચલ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        જો(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        તોડો;
    }
#endif
    સ્થિતિ UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        તોડો;
    સ્થિતિ UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        તોડો;
    સ્થિતિ UPROPS_SRC_INPC:
    સ્થિતિ UPROPS_SRC_INSC:
    સ્થિતિ UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        તોડો;
    સ્થિતિ UPROPS_SRC_EMOJI: {
        અચલ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        જો (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        તોડો;
    }
    સ્થિતિ UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        તોડો;
    સ્થિતિ UPROPS_SRC_ID_COMPAT_MATH:
    સ્થિતિ UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        તોડો;
    સ્થિતિ UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        તોડો;
    મૂળભૂત:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        તોડો;
    }

    જો (U_FAILURE(errorCode)) {
        પાછા;
    }
    જો (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

અચલ UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    જો (src < 0 || UPROPS_SRC_ગણતરી <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    પાછા i.fSet;
}

શૂન્ય U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_ગણતરી + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    અચલ UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    જો (U_FAILURE(errorCode)) {
        પાછા;
    }

    LocalPointer<UnicodeSet> intPropIncl(નવું UnicodeSet(0, 0));
    જો (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    int32_t અંક_Ranges = incl->get_Range_ગણતરી();
    int32_t prev_કિંમત = 0;
    માટે (int32_t i = 0; i < અંક_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        માટે (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_કિંમત(c, prop);
            જો (value != prev_કિંમત) {
                intPropIncl->add(c);
                prev_કિંમત = value;
            }
        }
    }

    જો (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[inclIndex].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

અચલ UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    જો (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_ગણતરી + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        પાછા i.fSet;
    } નહીંતર {
        UPropertySource src = uprops_getSource(prop);
        પાછા getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

નામાકાશ {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    LocalPointer<UnicodeSet> set(નવું UnicodeSet());
    જો (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    જો (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        અચલ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        જો (U_FAILURE(errorCode)) { પાછા nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        જો (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            પાછા set.orphan();
        }
    }

    અચલ UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    int32_t અંક_Ranges = inclusions->get_Range_ગણતરી();
    UChar32 startHasProperty = -1;

    માટે (int32_t i = 0; i < અંક_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        માટે (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            જો (u_has_દ્વિઅંકી_Property(c, property)) {
                જો (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } નહીંતર જો (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    જો (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    પાછા set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    uint32_t null_કિંમત = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_કિંમત, null_કિંમત, &errorCode));
    અચલ UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    int32_t અંક_Ranges = inclusions->get_Range_ગણતરી();
    UChar32 start = 0;
    uint32_t value = null_કિંમત;

    માટે (int32_t i = 0; i < અંક_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        માટે (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t next_કિંમત = u_get_Int_Property_કિંમત(c, property);
            જો (value != next_કિંમત) {
                જો (value != null_કિંમત) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = next_કિંમત;
            }
        }
    }
    જો (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    જો (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } નહીંતર {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_કિંમત_Width કિંમત_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_કિંમત(property);
    જો (max <= 0xff) {
        કિંમત_Width = UCPTRIE_કિંમત_BITS_8;
    } નહીંતર જો (max <= 0xffff) {
        કિંમત_Width = UCPTRIE_કિંમત_BITS_16;
    } નહીંતર {
        કિંમત_Width = UCPTRIE_કિંમત_BITS_32;
    }
    પાછા reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, કિંમત_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

અચલ UnicodeSet *CharacterProperties::get_દ્વિઅંકી_Property_Set(UProperty property, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    જો (property < 0 || UCHAR_દ્વિઅંકી_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    જો (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    પાછા set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI અચલ USet * U_EXPORT2
u_get_દ્વિઅંકી_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    અચલ UnicodeSet *set = CharacterProperties::get_દ્વિઅંકી_Property_Set(property, *pErrorCode);
    પાછા U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI અચલ UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) { પાછા nullptr; }
    જો (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    જો (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    પાછા map;
}
