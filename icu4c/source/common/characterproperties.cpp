// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// characterproperties.cpp
// created: 2018sep03 Markus W. Scherer

#شامل "unicode/utypes.h"
#شامل "unicode/localpointer.h"
#شامل "unicode/uchar.h"
#شامل "unicode/ucpmap.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/umutablecptrie.h"
#شامل "unicode/uniset.h"
#شامل "unicode/uscript.h"
#شامل "unicode/uset.h"
#شامل "cmemory.h"
#شامل "emojiprops.h"
#شامل "mutex.h"
#شامل "normalizer2impl.h"
#شامل "uassert.h"
#شامل "ubidi_props.h"
#شامل "ucase.h"
#شامل "ucln_cmn.h"
#شامل "umutex.h"
#شامل "uprops.h"

using icu::LocalPointer;
#اگر !UCONFIG_NO_NORMALIZATION
using icu::Normalizer2Factory;
using icu::Normalizer2Impl;
#endif
using icu::UInitOnce;
using icu::UnicodeSet;

فضاینام {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t رقم_INCLUSIONS = UPROPS_SRC_شمار + (UCHAR_INT_LIMIT - UCHAR_INT_START);

ساختار Inclusion {
    UnicodeSet  *fSet = nullptr;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[رقم_INCLUSIONS]; // cached getInclusions()

UnicodeSet *sets[UCHAR_دودویی_LIMIT] = {};

UCPMap *maps[UCHAR_INT_LIMIT - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
تهی U_CALLCONV
_set_add(USet *set, UChar32 c) {
    reinterpret_cast<UnicodeSet*>(set)->add(c);
}

تهی U_CALLCONV
_set_addRange(USet *set, UChar32 start, UChar32 end) {
    reinterpret_cast<UnicodeSet*>(set)->add(start, end);
}

تهی U_CALLCONV
_set_addString(USet *set, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<UnicodeSet*>(set)->add(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    برای (Inclusion &in: gInclusions) {
        delete in.fSet;
        in.fSet = nullptr;
        in.fInitOnce.reset();
    }
    برای (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        delete sets[i];
        sets[i] = nullptr;
    }
    برای (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = nullptr;
    }
    بازگشت درست;
}

تهی U_CALLCONV initInclusion(UPropertySource src, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_شمار);
    اگر (src == UPROPS_SRC_NONE) {
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        بازگشت;
    }
    U_ASSERT(gInclusions[src].fSet == nullptr);

    LocalPointer<UnicodeSet> incl(جدید UnicodeSet());
    اگر (incl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        _set_add,
        _set_addRange,
        _set_addString,
        nullptr, // don't need remove()
        nullptr // don't need removeRange()
    };

    انتخاب(src) {
    حالت UPROPS_SRC_CHAR:
        uchar_addPropertyStarts(&sa, &errorCode);
        بشکن;
    حالت UPROPS_SRC_PROPSVEC:
        upropsvec_addPropertyStarts(&sa, &errorCode);
        بشکن;
    حالت UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_addPropertyStarts(&sa, &errorCode);
        upropsvec_addPropertyStarts(&sa, &errorCode);
        بشکن;
#اگر !UCONFIG_NO_NORMALIZATION
    حالت UPROPS_SRC_CASE_AND_NORM: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        اگر(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        ucase_addPropertyStarts(&sa, &errorCode);
        بشکن;
    }
    حالت UPROPS_SRC_NFC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        اگر(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        بشکن;
    }
    حالت UPROPS_SRC_NFKC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(errorCode);
        اگر(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        بشکن;
    }
    حالت UPROPS_SRC_NFKC_CF: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(errorCode);
        اگر(U_SUCCESS(errorCode)) {
            impl->addPropertyStarts(&sa, errorCode);
        }
        بشکن;
    }
    حالت UPROPS_SRC_NFC_CANON_ITER: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
        اگر(U_SUCCESS(errorCode)) {
            impl->addCanonIterPropertyStarts(&sa, errorCode);
        }
        بشکن;
    }
#endif
    حالت UPROPS_SRC_CASE:
        ucase_addPropertyStarts(&sa, &errorCode);
        بشکن;
    حالت UPROPS_SRC_BIDI:
        ubidi_addPropertyStarts(&sa, &errorCode);
        بشکن;
    حالت UPROPS_SRC_INPC:
    حالت UPROPS_SRC_INSC:
    حالت UPROPS_SRC_VO:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        بشکن;
    حالت UPROPS_SRC_EMOJI: {
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        اگر (U_SUCCESS(errorCode)) {
            ep->addPropertyStarts(&sa, errorCode);
        }
        بشکن;
    }
    حالت UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.add(sa.set, 0x2FFE);
        sa.add(sa.set, 0x2FFF + 1);
        بشکن;
    حالت UPROPS_SRC_ID_COMPAT_MATH:
    حالت UPROPS_SRC_MCM:
        uprops_addPropertyStarts(src, &sa, &errorCode);
        بشکن;
    حالت UPROPS_SRC_BLOCK:
        ublock_addPropertyStarts(&sa, errorCode);
        بشکن;
    پیشفرض:
        errorCode = U_INTERNAL_PROGRAM_ERROR;
        بشکن;
    }

    اگر (U_FAILURE(errorCode)) {
        بازگشت;
    }
    اگر (incl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].fSet = incl.orphan();
    ucln_common_registerCleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ثابت UnicodeSet *getInclusionsForSource(UPropertySource src, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    اگر (src < 0 || UPROPS_SRC_شمار <= src) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, errorCode);
    بازگشت i.fSet;
}

تهی U_CALLCONV initIntPropInclusion(UProperty prop, UErrorCode &errorCode) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT);
    int32_t inclIndex = UPROPS_SRC_شمار + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[inclIndex].fSet == nullptr);
    UPropertySource src = uprops_getSource(prop);
    ثابت UnicodeSet *incl = getInclusionsForSource(src, errorCode);
    اگر (U_FAILURE(errorCode)) {
        بازگشت;
    }

    LocalPointer<UnicodeSet> intPropIncl(جدید UnicodeSet(0, 0));
    اگر (intPropIncl.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    int32_t رقم_Ranges = incl->get_Range_شمار();
    int32_t prev_مقدار = 0;
    برای (int32_t i = 0; i < رقم_Ranges; ++i) {
        UChar32 rangeEnd = incl->getRangeEnd(i);
        برای (UChar32 c = incl->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_مقدار(c, prop);
            اگر (value != prev_مقدار) {
                intPropIncl->add(c);
                prev_مقدار = value;
            }
        }
    }

    اگر (intPropIncl->isBogus()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
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
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    اگر (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        int32_t inclIndex = UPROPS_SRC_شمار + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[inclIndex];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, errorCode);
        بازگشت i.fSet;
    } وگرنه {
        UPropertySource src = uprops_getSource(prop);
        بازگشت getInclusionsForSource(src, errorCode);
    }
}

U_NAMESPACE_END

فضاینام {

UnicodeSet *makeSet(UProperty property, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    LocalPointer<UnicodeSet> set(جدید UnicodeSet());
    اگر (set.isNull()) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    اگر (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
        USetAdder sa = {
            reinterpret_cast<USet*>(set.getAlias()),
            _set_add,
            _set_addRange,
            _set_addString,
            nullptr, // don't need remove()
            nullptr // don't need removeRange()
        };
        ep->addStrings(&sa, property, errorCode);
        اگر (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            set->freeze();
            بازگشت set.orphan();
        }
    }

    ثابت UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    int32_t رقم_Ranges = inclusions->get_Range_شمار();
    UChar32 startHasProperty = -1;

    برای (int32_t i = 0; i < رقم_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        برای (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            اگر (u_has_دودویی_Property(c, property)) {
                اگر (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } وگرنه اگر (startHasProperty >= 0) {
                // Transition from true to false.
                set->add(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    اگر (startHasProperty >= 0) {
        set->add(startHasProperty, 0x10FFFF);
    }
    set->freeze();
    بازگشت set.orphan();
}

UCPMap *makeMap(UProperty property, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    uint32_t null_مقدار = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_مقدار, null_مقدار, &errorCode));
    ثابت UnicodeSet *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    int32_t رقم_Ranges = inclusions->get_Range_شمار();
    UChar32 start = 0;
    uint32_t value = null_مقدار;

    برای (int32_t i = 0; i < رقم_Ranges; ++i) {
        UChar32 rangeEnd = inclusions->getRangeEnd(i);
        برای (UChar32 c = inclusions->getRangeStart(i); c <= rangeEnd; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t next_مقدار = u_get_Int_Property_مقدار(c, property);
            اگر (value != next_مقدار) {
                اگر (value != null_مقدار) {
                    umutablecptrie_setRange(mutableTrie.getAlias(), start, c - 1, value, &errorCode);
                }
                start = c;
                value = next_مقدار;
            }
        }
    }
    اگر (value != 0) {
        umutablecptrie_setRange(mutableTrie.getAlias(), start, 0x10FFFF, value, &errorCode);
    }

    UCPTrieType type;
    اگر (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } وگرنه {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_مقدار_Width مقدار_Width;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_مقدار(property);
    اگر (max <= 0xff) {
        مقدار_Width = UCPTRIE_مقدار_BITS_8;
    } وگرنه اگر (max <= 0xffff) {
        مقدار_Width = UCPTRIE_مقدار_BITS_16;
    } وگرنه {
        مقدار_Width = UCPTRIE_مقدار_BITS_32;
    }
    بازگشت reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, مقدار_Width, &errorCode));
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت UnicodeSet *CharacterProperties::get_دودویی_Property_Set(UProperty property, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    اگر (property < 0 || UCHAR_دودویی_LIMIT <= property) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    Mutex m(&cpMutex);
    UnicodeSet *set = sets[property];
    اگر (set == nullptr) {
        sets[property] = set = makeSet(property, errorCode);
    }
    بازگشت set;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI ثابت USet * U_EXPORT2
u_get_دودویی_Property_Set(UProperty property, UErrorCode *pErrorCode) {
    ثابت UnicodeSet *set = CharacterProperties::get_دودویی_Property_Set(property, *pErrorCode);
    بازگشت U_SUCCESS(*pErrorCode) ? set->toUSet() : nullptr;
}

U_CAPI ثابت UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UErrorCode *pErrorCode) {
    اگر (U_FAILURE(*pErrorCode)) { بازگشت nullptr; }
    اگر (property < UCHAR_INT_START || UCHAR_INT_LIMIT <= property) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    اگر (map == nullptr) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *pErrorCode);
    }
    بازگشت map;
}
