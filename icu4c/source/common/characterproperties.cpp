// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#समावेश "unicode/utypes.h"
#समावेश "unicode/localpointer.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/ucpmap.h"
#समावेश "unicode/ucptrie.h"
#समावेश "unicode/umutablecptrie.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/uscript.h"
#समावेश "unicode/uset.h"
#समावेश "cmemory.h"
#समावेश "emojiprops.h"
#समावेश "mutex.h"
#समावेश "normalizer2impl.h"
#समावेश "uassert.h"
#समावेश "ubidi_props.h"
#समावेश "ucase.h"
#समावेश "ucln_cmn.h"
#समावेश "umutex.h"
#समावेश "uprops.h"

using icu::LocalPointer;
#अगर !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#पूर्ण_अगर
using icu::UInitOnce;
using icu::UnicodeSet;

namespace {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t अंक_INCLUSIONS = UPROPS_SRC_गिनती + (UCHAR_INT_LIMIT - UCHAR_INT_START);

काष्ठा Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[अंक_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_द्विआधारी_LIMIT] = {};

UCPMap *maps[UCHAR_INT_LIMIT - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
व्योम U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

व्योम U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

व्योम U_CALLCONV
_set_addString(USet *set, स्थिर char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    क्रम (Inclusion &in: gInclusions) {
        मिटाओ in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    क्रम (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        मिटाओ sets[i];
        sets[i] = nullptr;
    }
    क्रम (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    वापस true;
}

व्योम U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_गिनती);
    अगर (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        वापस;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(नया UnicodeSet());
    अगर (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    चयन(src) {
    हाल UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        अवरोध;
    हाल UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        अवरोध;
    हाल UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        अवरोध;
#अगर !UCONFIG_NO_NORMALIZATION
    हाल UPROPS_SRC_CASE_AND_NORM: {
        स्थिर Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        अगर(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        अवरोध;
    }
    हाल UPROPS_SRC_NFC: {
        स्थिर Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        अगर(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        अवरोध;
    }
    हाल UPROPS_SRC_NFKC: {
        स्थिर Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        अगर(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        अवरोध;
    }
    हाल UPROPS_SRC_NFKC_CF: {
        स्थिर Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        अगर(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        अवरोध;
    }
    हाल UPROPS_SRC_NFC_CANON_ITER: {
        स्थिर Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        अगर(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        अवरोध;
    }
#पूर्ण_अगर
    हाल UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        अवरोध;
    हाल UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        अवरोध;
    हाल UPROPS_SRC_INPC:
    हाल UPROPS_SRC_INSC:
    हाल UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        अवरोध;
    हाल UPROPS_SRC_EMOJI: {
        स्थिर icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        अगर (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        अवरोध;
    }
    हाल UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        अवरोध;
    हाल UPROPS_SRC_ID_COMPAT_MATH:
    हाल UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        अवरोध;
    हाल UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        अवरोध;
    शेष:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        अवरोध;
    }

    अगर (U_FAILURE(errorCode)) {
        वापस;
    }
    अगर (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

स्थिर UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    अगर (src < 0 || UPROPS_SRC_गिनती <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    वापस i.fSet;
}

व्योम U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t incl_सूचकांक = UPROPS_SRC_गिनती + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[incl_सूचकांक].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    स्थिर UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    अगर (U_FAILURE(errorCode)) {
        वापस;
    }

    LocalPointer<UnicodeSet> intPropIncl(नया UnicodeSet(0, 0));
    अगर (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    int32_t अंक_Ranges = incl->get_Range_गिनती();
    int32_t पूर्व_मूल्य = 0;
    क्रम (int32_t i = 0; i < अंक_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        क्रम (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_मूल्य(c, prop);
            अगर (value != पूर्व_मूल्य) {
                intPropIncl->add(c);
                पूर्व_मूल्य = value;
            }
        }
    }

    अगर (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[incl_सूचकांक].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

स्थिर UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    अगर (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t incl_सूचकांक = UPROPS_SRC_गिनती + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[incl_सूचकांक];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        वापस i.fSet;
    } अन्यथा {
        UPropertySource src = uprops_getSource(prop);
        वापस getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

namespace {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    LocalPointer<UnicodeSet> set(नया UnicodeSet());
    अगर (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    अगर (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        स्थिर icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        अगर (U_FAILURE(errorCode)) { वापस nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        अगर (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            वापस set.orphan();
        }
    }

    स्थिर UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    int32_t अंक_Ranges = inclusions->get_Range_गिनती();
    UChar32 startHasProperty = -1;

    क्रम (int32_t i = 0; i < अंक_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        क्रम (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            अगर (u_has_द्विआधारी_Property(c, property)) {
                अगर (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } अन्यथा अगर (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    अगर (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    वापस set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    uint32_t null_मूल्य = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_मूल्य, null_मूल्य, &errorCode));
    स्थिर UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    int32_t अंक_Ranges = inclusions->get_Range_गिनती();
    UChar32 start = 0;
    uint32_t value = null_मूल्य;

    क्रम (int32_t i = 0; i < अंक_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        क्रम (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t अग्र_मूल्य = u_get_Int_Property_मूल्य(c, property);
            अगर (value != अग्र_मूल्य) {
                अगर (value != null_मूल्य) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = अग्र_मूल्य;
            }
        }
    }
    अगर (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    अगर (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } अन्यथा {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_मूल्य_Width मूल्य_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_मूल्य(property);
    अगर (max <= 0xff) {
        मूल्य_Width = UCPTRIE_मूल्य_BITS_8;
    } अन्यथा अगर (max <= 0xffff) {
        मूल्य_Width = UCPTRIE_मूल्य_BITS_16;
    } अन्यथा {
        मूल्य_Width = UCPTRIE_मूल्य_BITS_32;
    }
    वापस reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, मूल्य_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

स्थिर UnicodeSet *CharacterProperties::get_द्विआधारी_Property_Set(UProperty property, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    अगर (property < 0 || UCHAR_द्विआधारी_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    अगर (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    वापस set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI स्थिर USet * U_EXPORT2
u_get_द्विआधारी_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    स्थिर UnicodeSet *set = CharacterProperties::get_द्विआधारी_Property_Set(property, *pErrorCode);
    वापस U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI स्थिर UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) { वापस nullptr; }
    अगर (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    अगर (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    वापस map;
}
