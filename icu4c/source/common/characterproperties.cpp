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
#nếu !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

namespace {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t NUM_INCLUSIONS = UPROPS_SRC_COUNT + (UCHAR_INT_LIMIT - UCHAR_INT_START);

struct Inclusion {
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
    cho (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    cho (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    cho (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    trả_về true;
}

void U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_COUNT);
    nếu (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        trả_về;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(new UnicodeSet());
    nếu (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        trả_về;
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
    case UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        break;
    case UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        break;
    case UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        break;
#nếu !UCONFIG_NO_NORMALIZATION
    case UPROPS_SRC_CASE_AND_NORM: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        nếu(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        break;
    }
    case UPROPS_SRC_NFC: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        nếu(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        break;
    }
    case UPROPS_SRC_NFKC: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        nếu(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        break;
    }
    case UPROPS_SRC_NFKC_CF: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        nếu(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        break;
    }
    case UPROPS_SRC_NFC_CANON_ITER: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        nếu(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        break;
    }
#endif
    case UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        break;
    case UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        break;
    case UPROPS_SRC_INPC:
    case UPROPS_SRC_INSC:
    case UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        break;
    case UPROPS_SRC_EMOJI: {
        const icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        nếu (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        break;
    }
    case UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        break;
    case UPROPS_SRC_ID_COMPAT_MATH:
    case UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        break;
    case UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        break;
    default:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        break;
    }

    nếu (U_FAILURE(errorCode)) {
        trả_về;
    }
    nếu (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        trả_về;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

const UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    nếu (U_FAILURE(errorCode)) { trả_về nullptr; }
    nếu (src < 0 || UPROPS_SRC_COUNT <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    trả_về i.fSet;
}

void U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_COUNT + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    const UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    nếu (U_FAILURE(errorCode)) {
        trả_về;
    }

    LocalPointer<UnicodeSet> intPropIncl(new UnicodeSet(0, 0));
    nếu (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        trả_về;
    }
    int32_t numRanges = incl->getRangeCount();
    int32_t prevValue = 0;
    cho (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        cho (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_getIntPropertyValue(c, prop);
            nếu (value != prevValue) {
                intPropIncl->add(c);
                prevValue = value;
            }
        }
    }

    nếu (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        trả_về;
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
    nếu (U_FAILURE(errorCode)) { trả_về nullptr; }
    nếu (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_COUNT + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        trả_về i.fSet;
    } không_thì {
        UPropertySource src = uprops_getSource(prop);
        trả_về getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

namespace {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    nếu (U_FAILURE(errorCode)) { trả_về nullptr; }
    LocalPointer<UnicodeSet> set(new UnicodeSet());
    nếu (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        trả_về nullptr;
    }
    nếu (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        const icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        nếu (U_FAILURE(errorCode)) { trả_về nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        nếu (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            trả_về set.orphan();
        }
    }

    const UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    nếu (U_FAILURE(errorCode)) { trả_về nullptr; }
    int32_t numRanges = inclusions->getRangeCount();
    UChar32 startHasProperty = -1;

    cho (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        cho (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            nếu (u_hasBinaryProperty(c, property)) {
                nếu (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } không_thì nếu (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    nếu (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    trả_về set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    nếu (U_FAILURE(errorCode)) { trả_về nullptr; }
    uint32_t nullValue = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(nullValue, nullValue, &errorCode));
    const UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    nếu (U_FAILURE(errorCode)) { trả_về nullptr; }
    int32_t numRanges = inclusions->getRangeCount();
    UChar32 start = 0;
    uint32_t value = nullValue;

    cho (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        cho (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t nextValue = u_getIntPropertyValue(c, property);
            nếu (value != nextValue) {
                nếu (value != nullValue) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = nextValue;
            }
        }
    }
    nếu (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    nếu (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } không_thì {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrieValueWidth valueWidth;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_getIntPropertyMaxValue(property);
    nếu (max <= 0xff) {
        valueWidth = UCPTRIE_VALUE_BITS_8;
    } không_thì nếu (max <= 0xffff) {
        valueWidth = UCPTRIE_VALUE_BITS_16;
    } không_thì {
        valueWidth = UCPTRIE_VALUE_BITS_32;
    }
    trả_về reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, valueWidth, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

const UnicodeSet *CharacterProperties::getBinaryPropertySet(UProperty property, UErrorCode &errorCode) {
    nếu (U_FAILURE(errorCode)) { trả_về nullptr; }
    nếu (property < 0 || UCHAR_BINARY_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    nếu (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    trả_về set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI const USet * U_EXPORT2
u_getBinaryPropertySet(UProperty property, UErrorCode *pErrorCode) {
    const UnicodeSet *set = CharacterProperties::getBinaryPropertySet(property, *pErrorCode);
    trả_về U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI const UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    nếu (U_FAILURE(*pErrorCode)) { trả_về nullptr; }
    nếu (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    nếu (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    trả_về map;
}
