// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#include "unicode/utypes.h"
#include "unicode/localpointer.h"
#include "unicode/uchar.h"
#include "unicode/ucpmap.h"
#include "unicode/ucptrie.h"
#include "unicode/umutablecptrie.h"
#include "unicode/uniset.h"
#include "unicode/uscript.h"
#include "unicode/uset.h"
#include "cmemory.h"
#include "emojiprops.h"
#include "mutex.h"
#include "normalizer2impl.h"
#include "uassert.h"
#include "ubidi_props.h"
#include "ucase.h"
#include "ucln_cmn.h"
#include "umutex.h"
#include "uprops.h"

using icu::LocalPointer;
#ꦪꦼꦤ꧀ !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

namespace {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t NUM_INCLUSIONS = UPROPS_SRC_COUNT + (UCHAR_INT_LIMIT - UCHAR_INT_START);

sa_nracanaa Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[NUM_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_BINARY_LIMIT] = {};

UCPMap *maps[UCHAR_INT_LIMIT - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
void U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

void U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

void U_CALLCONV
_set_addString(USet *set, const char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    ꦏꦁꦒꦺꦴ (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    ꦏꦁꦒꦺꦴ (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    ꦏꦁꦒꦺꦴ (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    ꦮꦭꦶ true;
}

void U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_COUNT);
    ꦪꦼꦤ꧀ (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        ꦮꦭꦶ;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(new UnicodeSet());
    ꦪꦼꦤ꧀ (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ꦮꦭꦶ;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    switch(src) {
    pak_sa UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        bha_nga;
    pak_sa UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        bha_nga;
    pak_sa UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        bha_nga;
#ꦪꦼꦤ꧀ !UCONFIG_NO_NORMALIZATION
    pak_sa UPROPS_SRC_CASE_AND_NORM: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ꦪꦼꦤ꧀(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        bha_nga;
    }
    pak_sa UPROPS_SRC_NFC: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ꦪꦼꦤ꧀(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        bha_nga;
    }
    pak_sa UPROPS_SRC_NFKC: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        ꦪꦼꦤ꧀(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        bha_nga;
    }
    pak_sa UPROPS_SRC_NFKC_CF: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        ꦪꦼꦤ꧀(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        bha_nga;
    }
    pak_sa UPROPS_SRC_NFC_CANON_ITER: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ꦪꦼꦤ꧀(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        bha_nga;
    }
#endif
    pak_sa UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        bha_nga;
    pak_sa UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        bha_nga;
    pak_sa UPROPS_SRC_INPC:
    pak_sa UPROPS_SRC_INSC:
    pak_sa UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        bha_nga;
    pak_sa UPROPS_SRC_EMOJI: {
        const icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        ꦪꦼꦤ꧀ (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        bha_nga;
    }
    pak_sa UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        bha_nga;
    pak_sa UPROPS_SRC_ID_COMPAT_MATH:
    pak_sa UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        bha_nga;
    pak_sa UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        bha_nga;
    default:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        bha_nga;
    }

    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) {
        ꦮꦭꦶ;
    }
    ꦪꦼꦤ꧀ (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ꦮꦭꦶ;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

const UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ nullptr; }
    ꦪꦼꦤ꧀ (src < 0 || UPROPS_SRC_COUNT <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    ꦮꦭꦶ i.fSet;
}

void U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_COUNT + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    const UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) {
        ꦮꦭꦶ;
    }

    LocalPointer<UnicodeSet> intPropIncl(new UnicodeSet(0, 0));
    ꦪꦼꦤ꧀ (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ꦮꦭꦶ;
    }
    int32_t numRanges = incl->getRangeCount();
    int32_t prevValue = 0;
    ꦏꦁꦒꦺꦴ (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        ꦏꦁꦒꦺꦴ (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_getIntPropertyValue(c, prop);
            ꦪꦼꦤ꧀ (value != prevValue) {
                intPropIncl->add(c);
                prevValue = value;
            }
        }
    }

    ꦪꦼꦤ꧀ (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ꦮꦭꦶ;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[inclIndex].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

const UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ nullptr; }
    ꦪꦼꦤ꧀ (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_COUNT + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        ꦮꦭꦶ i.fSet;
    } ꦭꦶꦪ {
        UPropertySource src = uprops_getSource(prop);
        ꦮꦭꦶ getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

namespace {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ nullptr; }
    LocalPointer<UnicodeSet> set(new UnicodeSet());
    ꦪꦼꦤ꧀ (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ꦮꦭꦶ nullptr;
    }
    ꦪꦼꦤ꧀ (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        const icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        ꦪꦼꦤ꧀ (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            ꦮꦭꦶ set.orphan();
        }
    }

    const UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ nullptr; }
    int32_t numRanges = inclusions->getRangeCount();
    UChar32 startHasProperty = -1;

    ꦏꦁꦒꦺꦴ (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        ꦏꦁꦒꦺꦴ (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            ꦪꦼꦤ꧀ (u_hasBinaryProperty(c, property)) {
                ꦪꦼꦤ꧀ (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } ꦭꦶꦪ ꦪꦼꦤ꧀ (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    ꦪꦼꦤ꧀ (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    ꦮꦭꦶ set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ nullptr; }
    uint32_t nullValue = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(nullValue, nullValue, &errorCode));
    const UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ nullptr; }
    int32_t numRanges = inclusions->getRangeCount();
    UChar32 start = 0;
    uint32_t value = nullValue;

    ꦏꦁꦒꦺꦴ (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        ꦏꦁꦒꦺꦴ (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t nextValue = u_getIntPropertyValue(c, property);
            ꦪꦼꦤ꧀ (value != nextValue) {
                ꦪꦼꦤ꧀ (value != nullValue) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = nextValue;
            }
        }
    }
    ꦪꦼꦤ꧀ (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    ꦪꦼꦤ꧀ (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } ꦭꦶꦪ {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrieValueWidth valueWidth;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_getIntPropertyMaxValue(property);
    ꦪꦼꦤ꧀ (max <= 0xff) {
        valueWidth = UCPTRIE_VALUE_BITS_8;
    } ꦭꦶꦪ ꦪꦼꦤ꧀ (max <= 0xffff) {
        valueWidth = UCPTRIE_VALUE_BITS_16;
    } ꦭꦶꦪ {
        valueWidth = UCPTRIE_VALUE_BITS_32;
    }
    ꦮꦭꦶ reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, valueWidth, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

const UnicodeSet *CharacterProperties::getBinaryPropertySet(UProperty property, UErrorCode &errorCode) {
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ nullptr; }
    ꦪꦼꦤ꧀ (property < 0 || UCHAR_BINARY_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    ꦪꦼꦤ꧀ (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    ꦮꦭꦶ set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI const USet * U_EXPORT2
u_getBinaryPropertySet(UProperty property, UErrorCode *pErrorCode) {
    const UnicodeSet *set = CharacterProperties::getBinaryPropertySet(property, *pErrorCode);
    ꦮꦭꦶ U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI const UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    ꦪꦼꦤ꧀ (U_FAILURE(*pErrorCode)) { ꦮꦭꦶ nullptr; }
    ꦪꦼꦤ꧀ (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    ꦪꦼꦤ꧀ (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    ꦮꦭꦶ map;
}
