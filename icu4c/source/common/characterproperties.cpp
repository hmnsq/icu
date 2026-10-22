// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/ucpmap.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/umutablecptrie.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "emojiprops.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "normalizer2impl.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ubidi_props.h"
#अन्तर्भाव "ucase.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uprops.h"

using icu::LocalPointer;
#सचे !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

नामाकाश {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t अङ्कः_INCLUSIONS = UPROPS_SRC_गणनम् + (UCHAR_INT_LIMIT - UCHAR_INT_START);

संरचना Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[अङ्कः_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_द्विमान_LIMIT] = {};

UCPMap *maps[UCHAR_INT_LIMIT - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
शून्य U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

शून्य U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

शून्य U_CALLCONV
_set_addString(USet *set, ध्रुव char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    अत्थाय (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    अत्थाय (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    अत्थाय (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    पटिनिवत्त सच्च;
}

शून्य U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_गणनम्);
    सचे (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        पटिनिवत्त;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(नूतन UnicodeSet());
    सचे (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    विकल्प(src) {
    पक्ष UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        भङ्ग;
    पक्ष UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        भङ्ग;
    पक्ष UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        भङ्ग;
#सचे !UCONFIG_NO_NORMALIZATION
    पक्ष UPROPS_SRC_CASE_AND_NORM: {
        ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        सचे(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        भङ्ग;
    }
    पक्ष UPROPS_SRC_NFC: {
        ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        सचे(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        भङ्ग;
    }
    पक्ष UPROPS_SRC_NFKC: {
        ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        सचे(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        भङ्ग;
    }
    पक्ष UPROPS_SRC_NFKC_CF: {
        ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        सचे(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        भङ्ग;
    }
    पक्ष UPROPS_SRC_NFC_CANON_ITER: {
        ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        सचे(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        भङ्ग;
    }
#endif
    पक्ष UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        भङ्ग;
    पक्ष UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        भङ्ग;
    पक्ष UPROPS_SRC_INPC:
    पक्ष UPROPS_SRC_INSC:
    पक्ष UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        भङ्ग;
    पक्ष UPROPS_SRC_EMOJI: {
        ध्रुव icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        सचे (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        भङ्ग;
    }
    पक्ष UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        भङ्ग;
    पक्ष UPROPS_SRC_ID_COMPAT_MATH:
    पक्ष UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        भङ्ग;
    पक्ष UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        भङ्ग;
    स्वभाव:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        भङ्ग;
    }

    सचे (U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    सचे (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ध्रुव UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    सचे (src < 0 || UPROPS_SRC_गणनम् <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    पटिनिवत्त i.fSet;
}

शून्य U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_गणनम् + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    ध्रुव UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    सचे (U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }

    LocalPointer<UnicodeSet> intPropIncl(नूतन UnicodeSet(0, 0));
    सचे (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    int32_t अङ्कः_Ranges = incl->get_Range_गणनम्();
    int32_t prev_मानम् = 0;
    अत्थाय (int32_t i = 0; i < अङ्कः_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        अत्थाय (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_मानम्(c, prop);
            सचे (value != prev_मानम्) {
                intPropIncl->add(c);
                prev_मानम् = value;
            }
        }
    }

    सचे (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[inclIndex].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ध्रुव UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    सचे (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_गणनम् + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        पटिनिवत्त i.fSet;
    } अञ्ञथा {
        UPropertySource src = uprops_getSource(prop);
        पटिनिवत्त getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

नामाकाश {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    LocalPointer<UnicodeSet> set(नूतन UnicodeSet());
    सचे (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }
    सचे (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ध्रुव icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        सचे (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            पटिनिवत्त set.orphan();
        }
    }

    ध्रुव UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    int32_t अङ्कः_Ranges = inclusions->get_Range_गणनम्();
    UChar32 startHasProperty = -1;

    अत्थाय (int32_t i = 0; i < अङ्कः_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        अत्थाय (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            सचे (u_has_द्विमान_Property(c, property)) {
                सचे (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } अञ्ञथा सचे (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    सचे (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    पटिनिवत्त set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    uint32_t null_मानम् = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_मानम्, null_मानम्, &errorCode));
    ध्रुव UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    int32_t अङ्कः_Ranges = inclusions->get_Range_गणनम्();
    UChar32 start = 0;
    uint32_t value = null_मानम्;

    अत्थाय (int32_t i = 0; i < अङ्कः_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        अत्थाय (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t next_मानम् = u_get_Int_Property_मानम्(c, property);
            सचे (value != next_मानम्) {
                सचे (value != null_मानम्) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = next_मानम्;
            }
        }
    }
    सचे (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    सचे (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } अञ्ञथा {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_मानम्_Width मानम्_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_मानम्(property);
    सचे (max <= 0xff) {
        मानम्_Width = UCPTRIE_मानम्_BITS_8;
    } अञ्ञथा सचे (max <= 0xffff) {
        मानम्_Width = UCPTRIE_मानम्_BITS_16;
    } अञ्ञथा {
        मानम्_Width = UCPTRIE_मानम्_BITS_32;
    }
    पटिनिवत्त reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, मानम्_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

ध्रुव UnicodeSet *CharacterProperties::get_द्विमान_Property_Set(UProperty property, UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    सचे (property < 0 || UCHAR_द्विमान_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    सचे (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    पटिनिवत्त set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI ध्रुव USet * U_EXPORT2
u_get_द्विमान_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    ध्रुव UnicodeSet *set = CharacterProperties::get_द्विमान_Property_Set(property, *pErrorCode);
    पटिनिवत्त U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI ध्रुव UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    सचे (U_FAILURE(*pErrorCode)) { पटिनिवत्त nullptr; }
    सचे (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    सचे (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    पटिनिवत्त map;
}
