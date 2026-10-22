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

using icu::LocalPointer;
#ಯದಿ !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

ನಾಮಾಕಾಶ {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t ಅಂಕೆ_INCLUSIONS = UPROPS_SRC_ಎಣಿಕೆ + (UCHAR_INT_LIMIT - UCHAR_INT_START);

ಸಂರಚನಾ Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[ಅಂಕೆ_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_ದ್ವಿಮಾನ_LIMIT] = {};

UCPMap *maps[UCHAR_INT_LIMIT - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ಶೂನ್ಯ U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

ಶೂನ್ಯ U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

ಶೂನ್ಯ U_CALLCONV
_set_addString(USet *set, ಧ್ರುವ char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    ಪ್ರತಿ (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    ಪ್ರತಿ (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    ಪ್ರತಿ (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_ಎಣಿಕೆ);
    ಯದಿ (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(ನೂತನ UnicodeSet());
    ಯದಿ (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    ವಿಕಲ್ಪ(src) {
    ಪಕ್ಷ UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        ಭಙ್ಗ;
    ಪಕ್ಷ UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        ಭಙ್ಗ;
    ಪಕ್ಷ UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        ಭಙ್ಗ;
#ಯದಿ !UCONFIG_NO_NORMALIZATION
    ಪಕ್ಷ UPROPS_SRC_CASE_AND_NORM: {
        ಧ್ರುವ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ಯದಿ(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ UPROPS_SRC_NFC: {
        ಧ್ರುವ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ಯದಿ(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ UPROPS_SRC_NFKC: {
        ಧ್ರುವ Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        ಯದಿ(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ UPROPS_SRC_NFKC_CF: {
        ಧ್ರುವ Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        ಯದಿ(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ UPROPS_SRC_NFC_CANON_ITER: {
        ಧ್ರುವ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ಯದಿ(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        ಭಙ್ಗ;
    }
#endif
    ಪಕ್ಷ UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        ಭಙ್ಗ;
    ಪಕ್ಷ UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        ಭಙ್ಗ;
    ಪಕ್ಷ UPROPS_SRC_INPC:
    ಪಕ್ಷ UPROPS_SRC_INSC:
    ಪಕ್ಷ UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        ಭಙ್ಗ;
    ಪಕ್ಷ UPROPS_SRC_EMOJI: {
        ಧ್ರುವ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        ಯದಿ (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        ಭಙ್ಗ;
    ಪಕ್ಷ UPROPS_SRC_ID_COMPAT_MATH:
    ಪಕ್ಷ UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        ಭಙ್ಗ;
    ಪಕ್ಷ UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        ಭಙ್ಗ;
    }

    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ಧ್ರುವ UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    ಯದಿ (src < 0 || UPROPS_SRC_ಎಣಿಕೆ <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    ಪ್ರತ್ಯಾಗಮ i.fSet;
}

ಶೂನ್ಯ U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_ಎಣಿಕೆ + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    ಧ್ರುವ UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    LocalPointer<UnicodeSet> intPropIncl(ನೂತನ UnicodeSet(0, 0));
    ಯದಿ (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    int32_t ಅಂಕೆ_Ranges = incl->get_Range_ಎಣಿಕೆ();
    int32_t prev_ಮೌಲ್ಯ = 0;
    ಪ್ರತಿ (int32_t i = 0; i < ಅಂಕೆ_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        ಪ್ರತಿ (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_ಮೌಲ್ಯ(c, prop);
            ಯದಿ (value != prev_ಮೌಲ್ಯ) {
                intPropIncl->add(c);
                prev_ಮೌಲ್ಯ = value;
            }
        }
    }

    ಯದಿ (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[inclIndex].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ಧ್ರುವ UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    ಯದಿ (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_ಎಣಿಕೆ + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        ಪ್ರತ್ಯಾಗಮ i.fSet;
    } ಅನ್ಯಥಾ {
        UPropertySource src = uprops_getSource(prop);
        ಪ್ರತ್ಯಾಗಮ getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

ನಾಮಾಕಾಶ {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    LocalPointer<UnicodeSet> set(ನೂತನ UnicodeSet());
    ಯದಿ (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ಧ್ರುವ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        ಯದಿ (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            ಪ್ರತ್ಯಾಗಮ set.orphan();
        }
    }

    ಧ್ರುವ UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    int32_t ಅಂಕೆ_Ranges = inclusions->get_Range_ಎಣಿಕೆ();
    UChar32 startHasProperty = -1;

    ಪ್ರತಿ (int32_t i = 0; i < ಅಂಕೆ_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        ಪ್ರತಿ (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            ಯದಿ (u_has_ದ್ವಿಮಾನ_Property(c, property)) {
                ಯದಿ (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } ಅನ್ಯಥಾ ಯದಿ (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    ಯದಿ (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    ಪ್ರತ್ಯಾಗಮ set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    uint32_t null_ಮೌಲ್ಯ = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_ಮೌಲ್ಯ, null_ಮೌಲ್ಯ, &errorCode));
    ಧ್ರುವ UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    int32_t ಅಂಕೆ_Ranges = inclusions->get_Range_ಎಣಿಕೆ();
    UChar32 start = 0;
    uint32_t value = null_ಮೌಲ್ಯ;

    ಪ್ರತಿ (int32_t i = 0; i < ಅಂಕೆ_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        ಪ್ರತಿ (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t next_ಮೌಲ್ಯ = u_get_Int_Property_ಮೌಲ್ಯ(c, property);
            ಯದಿ (value != next_ಮೌಲ್ಯ) {
                ಯದಿ (value != null_ಮೌಲ್ಯ) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = next_ಮೌಲ್ಯ;
            }
        }
    }
    ಯದಿ (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    ಯದಿ (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } ಅನ್ಯಥಾ {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_ಮೌಲ್ಯ_Width ಮೌಲ್ಯ_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_ಮೌಲ್ಯ(property);
    ಯದಿ (max <= 0xff) {
        ಮೌಲ್ಯ_Width = UCPTRIE_ಮೌಲ್ಯ_BITS_8;
    } ಅನ್ಯಥಾ ಯದಿ (max <= 0xffff) {
        ಮೌಲ್ಯ_Width = UCPTRIE_ಮೌಲ್ಯ_BITS_16;
    } ಅನ್ಯಥಾ {
        ಮೌಲ್ಯ_Width = UCPTRIE_ಮೌಲ್ಯ_BITS_32;
    }
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, ಮೌಲ್ಯ_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

ಧ್ರುವ UnicodeSet *CharacterProperties::get_ದ್ವಿಮಾನ_Property_Set(UProperty property, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    ಯದಿ (property < 0 || UCHAR_ದ್ವಿಮಾನ_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    ಯದಿ (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    ಪ್ರತ್ಯಾಗಮ set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI ಧ್ರುವ USet * U_EXPORT2
u_get_ದ್ವಿಮಾನ_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    ಧ್ರುವ UnicodeSet *set = CharacterProperties::get_ದ್ವಿಮಾನ_Property_Set(property, *pErrorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI ಧ್ರುವ UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    ಯದಿ (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    ಯದಿ (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    ಪ್ರತ್ಯಾಗಮ map;
}
