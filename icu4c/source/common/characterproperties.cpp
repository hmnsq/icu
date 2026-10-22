// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/localpointer.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/ucpmap.h"
#அந்தர்பாவ "unicode/ucptrie.h"
#அந்தர்பாவ "unicode/umutablecptrie.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/uscript.h"
#அந்தர்பாவ "unicode/uset.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "emojiprops.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "normalizer2impl.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ubidi_props.h"
#அந்தர்பாவ "ucase.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "uprops.h"

using icu::LocalPointer;
#எனில் !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

நாமாகாஶ {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t இலக்கம்_INCLUSIONS = UPROPS_SRC_மொத்தம் + (UCHAR_INT_LIMIT - UCHAR_INT_START);

கட்டமைப்பு Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[இலக்கம்_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_இருமம்_LIMIT] = {};

UCPMap *maps[UCHAR_INT_LIMIT - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
வெறுமை U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

வெறுமை U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

வெறுமை U_CALLCONV
_set_addString(USet *set, மாறிலி char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    க்கு (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    க்கு (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    க்கு (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    திரும்பு உண்மை;
}

வெறுமை U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_மொத்தம்);
    எனில் (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        திரும்பு;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(புதிய UnicodeSet());
    எனில் (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    தேர்வு(src) {
    நிலை UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        நிறுத்து;
    நிலை UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        நிறுத்து;
    நிலை UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        நிறுத்து;
#எனில் !UCONFIG_NO_NORMALIZATION
    நிலை UPROPS_SRC_CASE_AND_NORM: {
        மாறிலி Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        எனில்(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        நிறுத்து;
    }
    நிலை UPROPS_SRC_NFC: {
        மாறிலி Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        எனில்(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        நிறுத்து;
    }
    நிலை UPROPS_SRC_NFKC: {
        மாறிலி Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        எனில்(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        நிறுத்து;
    }
    நிலை UPROPS_SRC_NFKC_CF: {
        மாறிலி Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        எனில்(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        நிறுத்து;
    }
    நிலை UPROPS_SRC_NFC_CANON_ITER: {
        மாறிலி Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        எனில்(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        நிறுத்து;
    }
#endif
    நிலை UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        நிறுத்து;
    நிலை UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        நிறுத்து;
    நிலை UPROPS_SRC_INPC:
    நிலை UPROPS_SRC_INSC:
    நிலை UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        நிறுத்து;
    நிலை UPROPS_SRC_EMOJI: {
        மாறிலி icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        எனில் (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        நிறுத்து;
    }
    நிலை UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        நிறுத்து;
    நிலை UPROPS_SRC_ID_COMPAT_MATH:
    நிலை UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        நிறுத்து;
    நிலை UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        நிறுத்து;
    இயல்பு:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        நிறுத்து;
    }

    எனில் (U_FAILURE(errorCode)) {
        திரும்பு;
    }
    எனில் (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

மாறிலி UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    எனில் (src < 0 || UPROPS_SRC_மொத்தம் <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    திரும்பு i.fSet;
}

வெறுமை U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_மொத்தம் + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    மாறிலி UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு;
    }

    LocalPointer<UnicodeSet> intPropIncl(புதிய UnicodeSet(0, 0));
    எனில் (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    int32_t இலக்கம்_Ranges = incl->get_Range_மொத்தம்();
    int32_t prev_மதிப்பு = 0;
    க்கு (int32_t i = 0; i < இலக்கம்_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        க்கு (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_மதிப்பு(c, prop);
            எனில் (value != prev_மதிப்பு) {
                intPropIncl->add(c);
                prev_மதிப்பு = value;
            }
        }
    }

    எனில் (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[inclIndex].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

மாறிலி UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    எனில் (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_மொத்தம் + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        திரும்பு i.fSet;
    } இல்லையெனில் {
        UPropertySource src = uprops_getSource(prop);
        திரும்பு getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

நாமாகாஶ {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    LocalPointer<UnicodeSet> set(புதிய UnicodeSet());
    எனில் (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    எனில் (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        மாறிலி icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        எனில் (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            திரும்பு set.orphan();
        }
    }

    மாறிலி UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    int32_t இலக்கம்_Ranges = inclusions->get_Range_மொத்தம்();
    UChar32 startHasProperty = -1;

    க்கு (int32_t i = 0; i < இலக்கம்_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        க்கு (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            எனில் (u_has_இருமம்_Property(c, property)) {
                எனில் (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } இல்லையெனில் எனில் (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    எனில் (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    திரும்பு set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    uint32_t null_மதிப்பு = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_மதிப்பு, null_மதிப்பு, &errorCode));
    மாறிலி UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    int32_t இலக்கம்_Ranges = inclusions->get_Range_மொத்தம்();
    UChar32 start = 0;
    uint32_t value = null_மதிப்பு;

    க்கு (int32_t i = 0; i < இலக்கம்_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        க்கு (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t next_மதிப்பு = u_get_Int_Property_மதிப்பு(c, property);
            எனில் (value != next_மதிப்பு) {
                எனில் (value != null_மதிப்பு) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = next_மதிப்பு;
            }
        }
    }
    எனில் (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    எனில் (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } இல்லையெனில் {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_மதிப்பு_Width மதிப்பு_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_மதிப்பு(property);
    எனில் (max <= 0xff) {
        மதிப்பு_Width = UCPTRIE_மதிப்பு_BITS_8;
    } இல்லையெனில் எனில் (max <= 0xffff) {
        மதிப்பு_Width = UCPTRIE_மதிப்பு_BITS_16;
    } இல்லையெனில் {
        மதிப்பு_Width = UCPTRIE_மதிப்பு_BITS_32;
    }
    திரும்பு reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, மதிப்பு_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

மாறிலி UnicodeSet *CharacterProperties::get_இருமம்_Property_Set(UProperty property, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    எனில் (property < 0 || UCHAR_இருமம்_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    எனில் (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    திரும்பு set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI மாறிலி USet * U_EXPORT2
u_get_இருமம்_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    மாறிலி UnicodeSet *set = CharacterProperties::get_இருமம்_Property_Set(property, *pErrorCode);
    திரும்பு U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI மாறிலி UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) { திரும்பு nullptr; }
    எனில் (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    எனில் (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    திரும்பு map;
}
