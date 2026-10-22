// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/localpointer.h"
#অন্তর্ভাৱ "unicode/uchar.h"
#অন্তর্ভাৱ "unicode/ucpmap.h"
#অন্তর্ভাৱ "unicode/ucptrie.h"
#অন্তর্ভাৱ "unicode/umutablecptrie.h"
#অন্তর্ভাৱ "unicode/uniset.h"
#অন্তর্ভাৱ "unicode/uscript.h"
#অন্তর্ভাৱ "unicode/uset.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "emojiprops.h"
#অন্তর্ভাৱ "mutex.h"
#অন্তর্ভাৱ "normalizer2impl.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "ubidi_props.h"
#অন্তর্ভাৱ "ucase.h"
#অন্তর্ভাৱ "ucln_cmn.h"
#অন্তর্ভাৱ "umutex.h"
#অন্তর্ভাৱ "uprops.h"

using icu::LocalPointer;
#যদি !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

নামাকাশ {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t অঙ্ক_INCLUSIONS = UPROPS_SRC_গুনতি + (UCHAR_INT_LIMIT - UCHAR_INT_START);

সংরচনা Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[অঙ্ক_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_দ্বিমিক_LIMIT] = {};

UCPMap *maps[UCHAR_INT_LIMIT - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
শূন্য U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

শূন্য U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

শূন্য U_CALLCONV
_set_addString(USet *set, ধ্রুৱ char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    বাবে (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    বাবে (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    বাবে (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    ঘূৰাই সঁচা;
}

শূন্য U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_গুনতি);
    যদি (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        ঘূৰাই;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(নূতন UnicodeSet());
    যদি (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    ৱিকল্প(src) {
    পক্ষ UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        ভঙ্গ;
    পক্ষ UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        ভঙ্গ;
    পক্ষ UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        ভঙ্গ;
#যদি !UCONFIG_NO_NORMALIZATION
    পক্ষ UPROPS_SRC_CASE_AND_NORM: {
        ধ্রুৱ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        যদি(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        ভঙ্গ;
    }
    পক্ষ UPROPS_SRC_NFC: {
        ধ্রুৱ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        যদি(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ভঙ্গ;
    }
    পক্ষ UPROPS_SRC_NFKC: {
        ধ্রুৱ Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        যদি(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ভঙ্গ;
    }
    পক্ষ UPROPS_SRC_NFKC_CF: {
        ধ্রুৱ Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        যদি(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ভঙ্গ;
    }
    পক্ষ UPROPS_SRC_NFC_CANON_ITER: {
        ধ্রুৱ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        যদি(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        ভঙ্গ;
    }
#endif
    পক্ষ UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        ভঙ্গ;
    পক্ষ UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        ভঙ্গ;
    পক্ষ UPROPS_SRC_INPC:
    পক্ষ UPROPS_SRC_INSC:
    পক্ষ UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        ভঙ্গ;
    পক্ষ UPROPS_SRC_EMOJI: {
        ধ্রুৱ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        যদি (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        ভঙ্গ;
    }
    পক্ষ UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        ভঙ্গ;
    পক্ষ UPROPS_SRC_ID_COMPAT_MATH:
    পক্ষ UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        ভঙ্গ;
    পক্ষ UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        ভঙ্গ;
    স্ৱভাৱ:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        ভঙ্গ;
    }

    যদি (U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    যদি (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ধ্রুৱ UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    যদি (src < 0 || UPROPS_SRC_গুনতি <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    ঘূৰাই i.fSet;
}

শূন্য U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_গুনতি + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    ধ্রুৱ UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    যদি (U_FAILURE(errorCode)) {
        ঘূৰাই;
    }

    LocalPointer<UnicodeSet> intPropIncl(নূতন UnicodeSet(0, 0));
    যদি (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }
    int32_t অঙ্ক_Ranges = incl->get_Range_গুনতি();
    int32_t prev_মান = 0;
    বাবে (int32_t i = 0; i < অঙ্ক_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        বাবে (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_মান(c, prop);
            যদি (value != prev_মান) {
                intPropIncl->add(c);
                prev_মান = value;
            }
        }
    }

    যদি (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[inclIndex].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ধ্রুৱ UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    যদি (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_গুনতি + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        ঘূৰাই i.fSet;
    } অন্যথা {
        UPropertySource src = uprops_getSource(prop);
        ঘূৰাই getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

নামাকাশ {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    LocalPointer<UnicodeSet> set(নূতন UnicodeSet());
    যদি (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }
    যদি (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ধ্রুৱ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        যদি (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            ঘূৰাই set.orphan();
        }
    }

    ধ্রুৱ UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    int32_t অঙ্ক_Ranges = inclusions->get_Range_গুনতি();
    UChar32 startHasProperty = -1;

    বাবে (int32_t i = 0; i < অঙ্ক_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        বাবে (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            যদি (u_has_দ্বিমিক_Property(c, property)) {
                যদি (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } অন্যথা যদি (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    যদি (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    ঘূৰাই set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    uint32_t null_মান = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_মান, null_মান, &errorCode));
    ধ্রুৱ UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    int32_t অঙ্ক_Ranges = inclusions->get_Range_গুনতি();
    UChar32 start = 0;
    uint32_t value = null_মান;

    বাবে (int32_t i = 0; i < অঙ্ক_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        বাবে (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t next_মান = u_get_Int_Property_মান(c, property);
            যদি (value != next_মান) {
                যদি (value != null_মান) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = next_মান;
            }
        }
    }
    যদি (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    যদি (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } অন্যথা {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_মান_Width মান_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_মান(property);
    যদি (max <= 0xff) {
        মান_Width = UCPTRIE_মান_BITS_8;
    } অন্যথা যদি (max <= 0xffff) {
        মান_Width = UCPTRIE_মান_BITS_16;
    } অন্যথা {
        মান_Width = UCPTRIE_মান_BITS_32;
    }
    ঘূৰাই reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, মান_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

ধ্রুৱ UnicodeSet *CharacterProperties::get_দ্বিমিক_Property_Set(UProperty property, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    যদি (property < 0 || UCHAR_দ্বিমিক_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    যদি (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    ঘূৰাই set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI ধ্রুৱ USet * U_EXPORT2
u_get_দ্বিমিক_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    ধ্রুৱ UnicodeSet *set = CharacterProperties::get_দ্বিমিক_Property_Set(property, *pErrorCode);
    ঘূৰাই U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI ধ্রুৱ UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) { ঘূৰাই nullptr; }
    যদি (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    যদি (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    ঘূৰাই map;
}
