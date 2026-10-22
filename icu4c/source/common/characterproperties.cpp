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
#ถ้า !UCONFIG_NO_NORMALIZATION
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
    สำหรับ (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    สำหรับ (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    สำหรับ (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    คืน true;
}

void U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_COUNT);
    ถ้า (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        คืน;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(new UnicodeSet());
    ถ้า (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        คืน;
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
#ถ้า !UCONFIG_NO_NORMALIZATION
    pak_sa UPROPS_SRC_CASE_AND_NORM: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ถ้า(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        bha_nga;
    }
    pak_sa UPROPS_SRC_NFC: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ถ้า(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        bha_nga;
    }
    pak_sa UPROPS_SRC_NFKC: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        ถ้า(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        bha_nga;
    }
    pak_sa UPROPS_SRC_NFKC_CF: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        ถ้า(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        bha_nga;
    }
    pak_sa UPROPS_SRC_NFC_CANON_ITER: {
        const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ถ้า(U_SUCCESS(errorCode)) {
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
        ถ้า (U_SUCCESS(errorCode)) {
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

    ถ้า (U_FAILURE(errorCode)) {
        คืน;
    }
    ถ้า (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        คืน;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

const UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    ถ้า (U_FAILURE(errorCode)) { คืน nullptr; }
    ถ้า (src < 0 || UPROPS_SRC_COUNT <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        คืน nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    คืน i.fSet;
}

void U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_COUNT + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    const UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    ถ้า (U_FAILURE(errorCode)) {
        คืน;
    }

    LocalPointer<UnicodeSet> intPropIncl(new UnicodeSet(0, 0));
    ถ้า (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        คืน;
    }
    int32_t numRanges = incl->getRangeCount();
    int32_t prevValue = 0;
    สำหรับ (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        สำหรับ (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_getIntPropertyValue(c, prop);
            ถ้า (value != prevValue) {
                intPropIncl->add(c);
                prevValue = value;
            }
        }
    }

    ถ้า (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        คืน;
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
    ถ้า (U_FAILURE(errorCode)) { คืน nullptr; }
    ถ้า (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_COUNT + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        คืน i.fSet;
    } มิฉะนั้น {
        UPropertySource src = uprops_getSource(prop);
        คืน getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

namespace {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    ถ้า (U_FAILURE(errorCode)) { คืน nullptr; }
    LocalPointer<UnicodeSet> set(new UnicodeSet());
    ถ้า (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        คืน nullptr;
    }
    ถ้า (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        const icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        ถ้า (U_FAILURE(errorCode)) { คืน nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        ถ้า (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            คืน set.orphan();
        }
    }

    const UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    ถ้า (U_FAILURE(errorCode)) { คืน nullptr; }
    int32_t numRanges = inclusions->getRangeCount();
    UChar32 startHasProperty = -1;

    สำหรับ (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        สำหรับ (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            ถ้า (u_hasBinaryProperty(c, property)) {
                ถ้า (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } มิฉะนั้น ถ้า (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    ถ้า (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    คืน set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    ถ้า (U_FAILURE(errorCode)) { คืน nullptr; }
    uint32_t nullValue = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(nullValue, nullValue, &errorCode));
    const UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    ถ้า (U_FAILURE(errorCode)) { คืน nullptr; }
    int32_t numRanges = inclusions->getRangeCount();
    UChar32 start = 0;
    uint32_t value = nullValue;

    สำหรับ (int32_t i = 0; i < numRanges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        สำหรับ (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t nextValue = u_getIntPropertyValue(c, property);
            ถ้า (value != nextValue) {
                ถ้า (value != nullValue) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = nextValue;
            }
        }
    }
    ถ้า (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    ถ้า (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } มิฉะนั้น {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrieValueWidth valueWidth;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_getIntPropertyMaxValue(property);
    ถ้า (max <= 0xff) {
        valueWidth = UCPTRIE_VALUE_BITS_8;
    } มิฉะนั้น ถ้า (max <= 0xffff) {
        valueWidth = UCPTRIE_VALUE_BITS_16;
    } มิฉะนั้น {
        valueWidth = UCPTRIE_VALUE_BITS_32;
    }
    คืน reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, valueWidth, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

const UnicodeSet *CharacterProperties::getBinaryPropertySet(UProperty property, UErrorCode &errorCode) {
    ถ้า (U_FAILURE(errorCode)) { คืน nullptr; }
    ถ้า (property < 0 || UCHAR_BINARY_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        คืน nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    ถ้า (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    คืน set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI const USet * U_EXPORT2
u_getBinaryPropertySet(UProperty property, UErrorCode *pErrorCode) {
    const UnicodeSet *set = CharacterProperties::getBinaryPropertySet(property, *pErrorCode);
    คืน U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI const UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    ถ้า (U_FAILURE(*pErrorCode)) { คืน nullptr; }
    ถ้า (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        คืน nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    ถ้า (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    คืน map;
}
