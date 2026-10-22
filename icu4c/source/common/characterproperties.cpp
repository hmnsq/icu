// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#ضمن "unicode/utypes.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/ucpmap.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/uscript.h"
#ضمن "unicode/uset.h"
#ضمن "cmemory.h"
#ضمن "emojiprops.h"
#ضمن "mutex.h"
#ضمن "normalizer2impl.h"
#ضمن "uassert.h"
#ضمن "ubidi_props.h"
#ضمن "ucase.h"
#ضمن "ucln_cmn.h"
#ضمن "umutex.h"
#ضمن "uprops.h"

using icu::LocalPointer;
#إذا !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

نطاق {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t رقم_INCLUSIONS = UPROPS_SRC_عدد + (UCHAR_INT_LIMIT - UCHAR_INT_START);

بنية Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[رقم_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_ثنائي_LIMIT] = {};

UCPMap *maps[UCHAR_INT_LIMIT - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
فراغ U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

فراغ U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

فراغ U_CALLCONV
_set_addString(USet *set, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    لكل (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    أرجع true;
}

فراغ U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_عدد);
    إذا (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        أرجع;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(جديد UnicodeSet());
    إذا (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    بدل(src) {
    حالة UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        اكسر;
    حالة UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        اكسر;
    حالة UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        اكسر;
#إذا !UCONFIG_NO_NORMALIZATION
    حالة UPROPS_SRC_CASE_AND_NORM: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        إذا(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        اكسر;
    }
    حالة UPROPS_SRC_NFC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        إذا(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFKC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        إذا(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFKC_CF: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        إذا(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        اكسر;
    }
    حالة UPROPS_SRC_NFC_CANON_ITER: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        إذا(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        اكسر;
    }
#endif
    حالة UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        اكسر;
    حالة UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        اكسر;
    حالة UPROPS_SRC_INPC:
    حالة UPROPS_SRC_INSC:
    حالة UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        اكسر;
    حالة UPROPS_SRC_EMOJI: {
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        إذا (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        اكسر;
    }
    حالة UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        اكسر;
    حالة UPROPS_SRC_ID_COMPAT_MATH:
    حالة UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        اكسر;
    حالة UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        اكسر;
    افتراضي:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        اكسر;
    }

    إذا (U_FAILURE(errorCode)) {
        أرجع;
    }
    إذا (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ثابت UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode)) { أرجع nullptr; }
    إذا (src < 0 || UPROPS_SRC_عدد <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    أرجع i.fSet;
}

فراغ U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_عدد + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    ثابت UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    إذا (U_FAILURE(errorCode)) {
        أرجع;
    }

    LocalPointer<UnicodeSet> intPropIncl(جديد UnicodeSet(0, 0));
    إذا (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    int32_t رقم_Ranges = incl->get_Range_عدد();
    int32_t prev_قيمة = 0;
    لكل (int32_t i = 0; i < رقم_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        لكل (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_قيمة(c, prop);
            إذا (value != prev_قيمة) {
                intPropIncl->add(c);
                prev_قيمة = value;
            }
        }
    }

    إذا (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[inclIndex].fSet = intPropIncl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت UnicodeSet *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode)) { أرجع nullptr; }
    إذا (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_عدد + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        أرجع i.fSet;
    } وإلا {
        UPropertySource src = uprops_getSource(prop);
        أرجع getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

نطاق {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode)) { أرجع nullptr; }
    LocalPointer<UnicodeSet> set(جديد UnicodeSet());
    إذا (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    إذا (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        إذا (U_FAILURE(errorCode)) { أرجع nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        إذا (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            أرجع set.orphan();
        }
    }

    ثابت UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    إذا (U_FAILURE(errorCode)) { أرجع nullptr; }
    int32_t رقم_Ranges = inclusions->get_Range_عدد();
    UChar32 startHasProperty = -1;

    لكل (int32_t i = 0; i < رقم_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        لكل (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            إذا (u_has_ثنائي_Property(c, property)) {
                إذا (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } وإلا إذا (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    إذا (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    أرجع set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode)) { أرجع nullptr; }
    uint32_t null_قيمة = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_قيمة, null_قيمة, &errorCode));
    ثابت UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    إذا (U_FAILURE(errorCode)) { أرجع nullptr; }
    int32_t رقم_Ranges = inclusions->get_Range_عدد();
    UChar32 start = 0;
    uint32_t value = null_قيمة;

    لكل (int32_t i = 0; i < رقم_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        لكل (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t next_قيمة = u_get_Int_Property_قيمة(c, property);
            إذا (value != next_قيمة) {
                إذا (value != null_قيمة) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = next_قيمة;
            }
        }
    }
    إذا (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    إذا (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } وإلا {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_قيمة_Width قيمة_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_قيمة(property);
    إذا (max <= 0xff) {
        قيمة_Width = UCPTRIE_قيمة_BITS_8;
    } وإلا إذا (max <= 0xffff) {
        قيمة_Width = UCPTRIE_قيمة_BITS_16;
    } وإلا {
        قيمة_Width = UCPTRIE_قيمة_BITS_32;
    }
    أرجع reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, قيمة_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت UnicodeSet *CharacterProperties::get_ثنائي_Property_Set(UProperty property, UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode)) { أرجع nullptr; }
    إذا (property < 0 || UCHAR_ثنائي_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    إذا (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    أرجع set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI ثابت USet * U_EXPORT2
u_get_ثنائي_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    ثابت UnicodeSet *set = CharacterProperties::get_ثنائي_Property_Set(property, *pErrorCode);
    أرجع U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI ثابت UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    إذا (U_FAILURE(*pErrorCode)) { أرجع nullptr; }
    إذا (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    إذا (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    أرجع map;
}
