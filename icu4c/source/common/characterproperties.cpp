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
#如果 !UCONFIG_NO_NORMALIZATION
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
    为 (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    为 (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    为 (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    返回 true;
}

void U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_COUNT);
    如果 (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        返回;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(new UnicodeSet());
    如果 (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        返回;
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
#如果 !UCONFIG_NO_NORMALIZATION
    case UPROPS_SRC_CASE_AND_NORM: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        如果(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        break;
    }
    case UPROPS_SRC_NFC: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        如果(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        break;
    }
    case UPROPS_SRC_NFKC: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        如果(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        break;
    }
    case UPROPS_SRC_NFKC_CF: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        如果(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        break;
    }
    case UPROPS_SRC_NFC_CANON_ITER: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        如果(U_SUCCESS(errorCode)) {
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
        如果 (U_SUCCESS(errorCode)) {
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

    如果 (U_FAILURE(errorCode)) {
        返回;
    }
    如果 (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        返回;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

const UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    如果 (U_FAILURE(errorCode)) { 返回 nullptr; }
    如果 (src < 0 || UPROPS_SRC_COUNT <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        返回 nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    返回 i.fSet;
}

void U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_COUNT + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    const UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    如果 (U_FAILURE(errorCode)) {
        返回;
    }

    LocalPointer<UnicodeSet> intPropIncl(new UnicodeSet(0, 0));
    如果 (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        返回;
    }
    int32_t numRanges = incl->getRangeCount();
    int32_t prevValue = 0;
    为 (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        为 (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_getIntPropertyValue(c, prop);
            如果 (value != prevValue) {
                intPropIncl->add(c);
                prevValue = value;
            }
        }
    }

    如果 (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        返回;
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
    如果 (U_FAILURE(errorCode)) { 返回 nullptr; }
    如果 (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_COUNT + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        返回 i.fSet;
    } 否则 {
        UPropertySource src = uprops_getSource(prop);
        返回 getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

namespace {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    如果 (U_FAILURE(errorCode)) { 返回 nullptr; }
    LocalPointer<UnicodeSet> set(new UnicodeSet());
    如果 (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        返回 nullptr;
    }
    如果 (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        const icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        如果 (U_FAILURE(errorCode)) { 返回 nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        如果 (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            返回 set.orphan();
        }
    }

    const UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    如果 (U_FAILURE(errorCode)) { 返回 nullptr; }
    int32_t numRanges = inclusions->getRangeCount();
    UChar32 startHasProperty = -1;

    为 (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        为 (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            如果 (u_hasBinaryProperty(c, property)) {
                如果 (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } 否则 如果 (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    如果 (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    返回 set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    如果 (U_FAILURE(errorCode)) { 返回 nullptr; }
    uint32_t nullValue = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(nullValue, nullValue, &errorCode));
    const UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    如果 (U_FAILURE(errorCode)) { 返回 nullptr; }
    int32_t numRanges = inclusions->getRangeCount();
    UChar32 start = 0;
    uint32_t value = nullValue;

    为 (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        为 (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t nextValue = u_getIntPropertyValue(c, property);
            如果 (value != nextValue) {
                如果 (value != nullValue) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = nextValue;
            }
        }
    }
    如果 (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    如果 (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } 否则 {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrieValueWidth valueWidth;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_getIntPropertyMaxValue(property);
    如果 (max <= 0xff) {
        valueWidth = UCPTRIE_VALUE_BITS_8;
    } 否则 如果 (max <= 0xffff) {
        valueWidth = UCPTRIE_VALUE_BITS_16;
    } 否则 {
        valueWidth = UCPTRIE_VALUE_BITS_32;
    }
    返回 reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, valueWidth, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

const UnicodeSet *CharacterProperties::getBinaryPropertySet(UProperty property, UErrorCode &errorCode) {
    如果 (U_FAILURE(errorCode)) { 返回 nullptr; }
    如果 (property < 0 || UCHAR_BINARY_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        返回 nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    如果 (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    返回 set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI const USet * U_EXPORT2
u_getBinaryPropertySet(UProperty property, UErrorCode *pErrorCode) {
    const UnicodeSet *set = CharacterProperties::getBinaryPropertySet(property, *pErrorCode);
    返回 U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI const UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    如果 (U_FAILURE(*pErrorCode)) { 返回 nullptr; }
    如果 (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        返回 nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    如果 (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    返回 map;
}
