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
#यदि !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

नामाकाश {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t अङ्क_INCLUSIONS = UPROPS_SRC_गन्ती + (UCHAR_INT_LIMIT - UCHAR_INT_START);

संरचना Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[अङ्क_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_द्विआधारी_LIMIT] = {};

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
_set_addString(USet *set, अचर char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    लागि (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    लागि (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    लागि (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    फर्काऊ सत्य;
}

शून्य U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_गन्ती);
    यदि (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        फर्काऊ;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(नूतन UnicodeSet());
    यदि (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
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
        रोक;
    पक्ष UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        रोक;
    पक्ष UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        रोक;
#यदि !UCONFIG_NO_NORMALIZATION
    पक्ष UPROPS_SRC_CASE_AND_NORM: {
        अचर Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        यदि(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        रोक;
    }
    पक्ष UPROPS_SRC_NFC: {
        अचर Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        यदि(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        रोक;
    }
    पक्ष UPROPS_SRC_NFKC: {
        अचर Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        यदि(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        रोक;
    }
    पक्ष UPROPS_SRC_NFKC_CF: {
        अचर Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        यदि(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        रोक;
    }
    पक्ष UPROPS_SRC_NFC_CANON_ITER: {
        अचर Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        यदि(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        रोक;
    }
#endif
    पक्ष UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        रोक;
    पक्ष UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        रोक;
    पक्ष UPROPS_SRC_INPC:
    पक्ष UPROPS_SRC_INSC:
    पक्ष UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        रोक;
    पक्ष UPROPS_SRC_EMOJI: {
        अचर icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        यदि (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        रोक;
    }
    पक्ष UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        रोक;
    पक्ष UPROPS_SRC_ID_COMPAT_MATH:
    पक्ष UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        रोक;
    पक्ष UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        रोक;
    स्वभाव:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        रोक;
    }

    यदि (U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    यदि (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

अचर UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    यदि (src < 0 || UPROPS_SRC_गन्ती <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    फर्काऊ i.fSet;
}

शून्य U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_गन्ती + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    अचर UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    यदि (U_FAILURE(errorCode)) {
        फर्काऊ;
    }

    LocalPointer<UnicodeSet> intPropIncl(नूतन UnicodeSet(0, 0));
    यदि (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    int32_t अङ्क_Ranges = incl->get_Range_गन्ती();
    int32_t prev_मान = 0;
    लागि (int32_t i = 0; i < अङ्क_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        लागि (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_मान(c, prop);
            यदि (value != prev_मान) {
                intPropIncl->add(c);
                prev_मान = value;
            }
        }
    }

    यदि (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[inclIndex].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

अचर UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    यदि (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_गन्ती + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        फर्काऊ i.fSet;
    } अन्यथा {
        UPropertySource src = uprops_getSource(prop);
        फर्काऊ getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

नामाकाश {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    LocalPointer<UnicodeSet> set(नूतन UnicodeSet());
    यदि (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    यदि (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        अचर icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        यदि (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            फर्काऊ set.orphan();
        }
    }

    अचर UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    int32_t अङ्क_Ranges = inclusions->get_Range_गन्ती();
    UChar32 startHasProperty = -1;

    लागि (int32_t i = 0; i < अङ्क_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        लागि (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            यदि (u_has_द्विआधारी_Property(c, property)) {
                यदि (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } अन्यथा यदि (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    यदि (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    फर्काऊ set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    uint32_t null_मान = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_मान, null_मान, &errorCode));
    अचर UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    int32_t अङ्क_Ranges = inclusions->get_Range_गन्ती();
    UChar32 start = 0;
    uint32_t value = null_मान;

    लागि (int32_t i = 0; i < अङ्क_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        लागि (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t next_मान = u_get_Int_Property_मान(c, property);
            यदि (value != next_मान) {
                यदि (value != null_मान) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = next_मान;
            }
        }
    }
    यदि (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    यदि (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } अन्यथा {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_मान_Width मान_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_मान(property);
    यदि (max <= 0xff) {
        मान_Width = UCPTRIE_मान_BITS_8;
    } अन्यथा यदि (max <= 0xffff) {
        मान_Width = UCPTRIE_मान_BITS_16;
    } अन्यथा {
        मान_Width = UCPTRIE_मान_BITS_32;
    }
    फर्काऊ reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, मान_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

अचर UnicodeSet *CharacterProperties::get_द्विआधारी_Property_Set(UProperty property, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    यदि (property < 0 || UCHAR_द्विआधारी_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    यदि (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    फर्काऊ set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI अचर USet * U_EXPORT2
u_get_द्विआधारी_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    अचर UnicodeSet *set = CharacterProperties::get_द्विआधारी_Property_Set(property, *pErrorCode);
    फर्काऊ U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI अचर UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) { फर्काऊ nullptr; }
    यदि (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    यदि (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    फर्काऊ map;
}
