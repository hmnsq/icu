// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localpointer.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucpmap.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/umutablecptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uscript.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uset.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "emojiprops.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "normalizer2impl.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ubidi_props.h"
#ଅନ୍ତର୍ଭାଵ "ucase.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "uprops.h"

using icu::LocalPointer;
#ଯଦି !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

ନାମାକାଶ {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t ଅଙ୍କ_INCLUSIONS = UPROPS_SRC_ଗଣନା + (UCHAR_INT_LIMIT - UCHAR_INT_START);

ସଂରଚନା Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[ଅଙ୍କ_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_ଦ୍ୱିମିକ_LIMIT] = {};

UCPMap *maps[UCHAR_INT_LIMIT - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
ଶୂନ୍ୟ U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

ଶୂନ୍ୟ U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

ଶୂନ୍ୟ U_CALLCONV
_set_addString(USet *set, ଅଚଳ char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    ପାଇଁ (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    ପାଇଁ (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    ପାଇଁ (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_ଗଣନା);
    ଯଦି (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        ଫେରନ୍ତୁ;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(ନୂତନ UnicodeSet());
    ଯଦି (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    ବାଛ(src) {
    ସ୍ଥିତି UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        ଭାଙ୍ଗ;
#ଯଦି !UCONFIG_NO_NORMALIZATION
    ସ୍ଥିତି UPROPS_SRC_CASE_AND_NORM: {
        ଅଚଳ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ଯଦି(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି UPROPS_SRC_NFC: {
        ଅଚଳ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ଯଦି(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି UPROPS_SRC_NFKC: {
        ଅଚଳ Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        ଯଦି(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି UPROPS_SRC_NFKC_CF: {
        ଅଚଳ Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        ଯଦି(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି UPROPS_SRC_NFC_CANON_ITER: {
        ଅଚଳ Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        ଯଦି(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        ଭାଙ୍ଗ;
    }
#endif
    ସ୍ଥିତି UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UPROPS_SRC_INPC:
    ସ୍ଥିତି UPROPS_SRC_INSC:
    ସ୍ଥିତି UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UPROPS_SRC_EMOJI: {
        ଅଚଳ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        ଯଦି (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UPROPS_SRC_ID_COMPAT_MATH:
    ସ୍ଥିତି UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        ଭାଙ୍ଗ;
    }

    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ଅଚଳ UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    ଯଦି (src < 0 || UPROPS_SRC_ଗଣନା <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    ଫେରନ୍ତୁ i.fSet;
}

ଶୂନ୍ୟ U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_ଗଣନା + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    ଅଚଳ UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }

    LocalPointer<UnicodeSet> intPropIncl(ନୂତନ UnicodeSet(0, 0));
    ଯଦି (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    int32_t ଅଙ୍କ_Ranges = incl->get_Range_ଗଣନା();
    int32_t prev_ମୂଲ୍ୟ = 0;
    ପାଇଁ (int32_t i = 0; i < ଅଙ୍କ_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        ପାଇଁ (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_ମୂଲ୍ୟ(c, prop);
            ଯଦି (value != prev_ମୂଲ୍ୟ) {
                intPropIncl->add(c);
                prev_ମୂଲ୍ୟ = value;
            }
        }
    }

    ଯଦି (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[inclIndex].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ଅଚଳ UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    ଯଦି (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_ଗଣନା + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        ଫେରନ୍ତୁ i.fSet;
    } ଅନ୍ୟଥା {
        UPropertySource src = uprops_getSource(prop);
        ଫେରନ୍ତୁ getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

ନାମାକାଶ {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    LocalPointer<UnicodeSet> set(ନୂତନ UnicodeSet());
    ଯଦି (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ଅଚଳ icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        ଯଦି (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            ଫେରନ୍ତୁ set.orphan();
        }
    }

    ଅଚଳ UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    int32_t ଅଙ୍କ_Ranges = inclusions->get_Range_ଗଣନା();
    UChar32 startHasProperty = -1;

    ପାଇଁ (int32_t i = 0; i < ଅଙ୍କ_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        ପାଇଁ (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            ଯଦି (u_has_ଦ୍ୱିମିକ_Property(c, property)) {
                ଯଦି (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } ଅନ୍ୟଥା ଯଦି (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    ଯଦି (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    ଫେରନ୍ତୁ set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    uint32_t null_ମୂଲ୍ୟ = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_ମୂଲ୍ୟ, null_ମୂଲ୍ୟ, &errorCode));
    ଅଚଳ UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    int32_t ଅଙ୍କ_Ranges = inclusions->get_Range_ଗଣନା();
    UChar32 start = 0;
    uint32_t value = null_ମୂଲ୍ୟ;

    ପାଇଁ (int32_t i = 0; i < ଅଙ୍କ_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        ପାଇଁ (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t next_ମୂଲ୍ୟ = u_get_Int_Property_ମୂଲ୍ୟ(c, property);
            ଯଦି (value != next_ମୂଲ୍ୟ) {
                ଯଦି (value != null_ମୂଲ୍ୟ) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = next_ମୂଲ୍ୟ;
            }
        }
    }
    ଯଦି (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    ଯଦି (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } ଅନ୍ୟଥା {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_ମୂଲ୍ୟ_Width ମୂଲ୍ୟ_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_ମୂଲ୍ୟ(property);
    ଯଦି (max <= 0xff) {
        ମୂଲ୍ୟ_Width = UCPTRIE_ମୂଲ୍ୟ_BITS_8;
    } ଅନ୍ୟଥା ଯଦି (max <= 0xffff) {
        ମୂଲ୍ୟ_Width = UCPTRIE_ମୂଲ୍ୟ_BITS_16;
    } ଅନ୍ୟଥା {
        ମୂଲ୍ୟ_Width = UCPTRIE_ମୂଲ୍ୟ_BITS_32;
    }
    ଫେରନ୍ତୁ reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, ମୂଲ୍ୟ_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

ଅଚଳ UnicodeSet *CharacterProperties::get_ଦ୍ୱିମିକ_Property_Set(UProperty property, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    ଯଦି (property < 0 || UCHAR_ଦ୍ୱିମିକ_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    ଯଦି (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    ଫେରନ୍ତୁ set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI ଅଚଳ USet * U_EXPORT2
u_get_ଦ୍ୱିମିକ_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    ଅଚଳ UnicodeSet *set = CharacterProperties::get_ଦ୍ୱିମିକ_Property_Set(property, *pErrorCode);
    ଫେରନ୍ତୁ U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI ଅଚଳ UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) { ଫେରନ୍ତୁ nullptr; }
    ଯଦି (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    ଯଦି (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    ଫେରନ୍ତୁ map;
}
