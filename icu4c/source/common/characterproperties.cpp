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
#जर !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

नामाकाश {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t अंक_INCLUSIONS = UPROPS_SRC_मोजणी + (UCHAR_INT_LIMIT - UCHAR_INT_START);

संरचना Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[अंक_INCLUSIONS]; // cached getInclusions()

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
_set_addString(USet *set, स्थिरांक char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    साठी (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    साठी (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    साठी (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    परत सत्य;
}

शून्य U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_मोजणी);
    जर (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        परत;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(नवीन UnicodeSet());
    जर (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    निवड(src) {
    स्थिती UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        थांबा;
    स्थिती UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        थांबा;
    स्थिती UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        थांबा;
#जर !UCONFIG_NO_NORMALIZATION
    स्थिती UPROPS_SRC_CASE_AND_NORM: {
        स्थिरांक Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        जर(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        थांबा;
    }
    स्थिती UPROPS_SRC_NFC: {
        स्थिरांक Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        जर(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        थांबा;
    }
    स्थिती UPROPS_SRC_NFKC: {
        स्थिरांक Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        जर(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        थांबा;
    }
    स्थिती UPROPS_SRC_NFKC_CF: {
        स्थिरांक Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        जर(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        थांबा;
    }
    स्थिती UPROPS_SRC_NFC_CANON_ITER: {
        स्थिरांक Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        जर(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        थांबा;
    }
#endif
    स्थिती UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        थांबा;
    स्थिती UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        थांबा;
    स्थिती UPROPS_SRC_INPC:
    स्थिती UPROPS_SRC_INSC:
    स्थिती UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        थांबा;
    स्थिती UPROPS_SRC_EMOJI: {
        स्थिरांक icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        जर (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        थांबा;
    }
    स्थिती UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        थांबा;
    स्थिती UPROPS_SRC_ID_COMPAT_MATH:
    स्थिती UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        थांबा;
    स्थिती UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        थांबा;
    पूर्वनिर्धारित:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        थांबा;
    }

    जर (U_FAILURE(errorCode)) {
        परत;
    }
    जर (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

स्थिरांक UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    जर (src < 0 || UPROPS_SRC_मोजणी <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    परत i.fSet;
}

शून्य U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_मोजणी + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    स्थिरांक UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    जर (U_FAILURE(errorCode)) {
        परत;
    }

    LocalPointer<UnicodeSet> intPropIncl(नवीन UnicodeSet(0, 0));
    जर (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    int32_t अंक_Ranges = incl->get_Range_मोजणी();
    int32_t prev_मूल्य = 0;
    साठी (int32_t i = 0; i < अंक_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        साठी (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_मूल्य(c, prop);
            जर (value != prev_मूल्य) {
                intPropIncl->add(c);
                prev_मूल्य = value;
            }
        }
    }

    जर (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[inclIndex].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

स्थिरांक UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    जर (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_मोजणी + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        परत i.fSet;
    } अन्यथा {
        UPropertySource src = uprops_getSource(prop);
        परत getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

नामाकाश {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    LocalPointer<UnicodeSet> set(नवीन UnicodeSet());
    जर (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    जर (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        स्थिरांक icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        जर (U_FAILURE(errorCode)) { परत nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        जर (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            परत set.orphan();
        }
    }

    स्थिरांक UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    int32_t अंक_Ranges = inclusions->get_Range_मोजणी();
    UChar32 startHasProperty = -1;

    साठी (int32_t i = 0; i < अंक_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        साठी (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            जर (u_has_द्विमान_Property(c, property)) {
                जर (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } अन्यथा जर (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    जर (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    परत set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    uint32_t null_मूल्य = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_मूल्य, null_मूल्य, &errorCode));
    स्थिरांक UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    int32_t अंक_Ranges = inclusions->get_Range_मोजणी();
    UChar32 start = 0;
    uint32_t value = null_मूल्य;

    साठी (int32_t i = 0; i < अंक_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        साठी (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t next_मूल्य = u_get_Int_Property_मूल्य(c, property);
            जर (value != next_मूल्य) {
                जर (value != null_मूल्य) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = next_मूल्य;
            }
        }
    }
    जर (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    जर (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } अन्यथा {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_मूल्य_Width मूल्य_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_मूल्य(property);
    जर (max <= 0xff) {
        मूल्य_Width = UCPTRIE_मूल्य_BITS_8;
    } अन्यथा जर (max <= 0xffff) {
        मूल्य_Width = UCPTRIE_मूल्य_BITS_16;
    } अन्यथा {
        मूल्य_Width = UCPTRIE_मूल्य_BITS_32;
    }
    परत reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, मूल्य_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

स्थिरांक UnicodeSet *CharacterProperties::get_द्विमान_Property_Set(UProperty property, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    जर (property < 0 || UCHAR_द्विमान_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    जर (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    परत set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI स्थिरांक USet * U_EXPORT2
u_get_द्विमान_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    स्थिरांक UnicodeSet *set = CharacterProperties::get_द्विमान_Property_Set(property, *pErrorCode);
    परत U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI स्थिरांक UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    जर (U_FAILURE(*pErrorCode)) { परत nullptr; }
    जर (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    जर (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    परत map;
}
