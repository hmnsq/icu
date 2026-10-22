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

उपयुज्य icu::LocalPointer;
#यदि !UCONFIG_NO_NORMALIZATION
उपयुज्य icu::Normalizer2Factory;
उपयुज्य icu::Normalizer2Impl;
#यदि_अन्तः
उपयुज्य icu::UInitOnce;
उपयुज्य icu::Unicode_समुच्चयः;

नामाकाश {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t अङ्कः_INCLUSIONS = UPROPS_SRC_गणनम् + (UCHAR_INT_सीमा - UCHAR_INT_START);

संरचना Inclusion {
    Unicode_समुच्चयः  *f_समुच्चयः = शून्यसूचकः;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[अङ्कः_INCLUSIONS]; // cached getInclusions()

Unicode_समुच्चयः *sets[UCHAR_द्विमान_सीमा] = {};

UCPMap *maps[UCHAR_INT_सीमा - UCHAR_INT_START] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
शून्य U_CALLCONV
समुच्चयः_सङ्कलय(USet *समुच्चयः, UChar32 c) {
    reinterpret_cast<Unicode_समुच्चयः*>(समुच्चयः)->सङ्कलय(c);
}

शून्य U_CALLCONV
समुच्चयः_सङ्कलय_Range(USet *समुच्चयः, UChar32 start, UChar32 अन्तः) {
    reinterpret_cast<Unicode_समुच्चयः*>(समुच्चयः)->सङ्कलय(start, अन्तः);
}

शून्य U_CALLCONV
समुच्चयः_सङ्कलय_String(USet *समुच्चयः, ध्रुव char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_समुच्चयः*>(समुच्चयः)->सङ्कलय(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    प्रति (Inclusion &in: gInclusions) {
        विलोपय in.f_समुच्चयः;
        in.f_समुच्चयः = शून्यसूचकः;
        in.fInitOnce.reset();
    }
    प्रति (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        विलोपय sets[i];
        sets[i] = शून्यसूचकः;
    }
    प्रति (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = शून्यसूचकः;
    }
    प्रत्यागम सत्य;
}

शून्य U_CALLCONV initInclusion(UPropertySource src, UError_संहिता &error_संहिता) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_गणनम्);
    यदि (src == UPROPS_SRC_NONE) {
        error_संहिता = U_आन्तरिकम्_क्रमादेशः_ERROR;
        प्रत्यागम;
    }
    U_ASSERT(gInclusions[src].f_समुच्चयः == शून्यसूचकः);

    LocalPointer<Unicode_समुच्चयः> incl(नूतन Unicode_समुच्चयः());
    यदि (incl.isNull()) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        समुच्चयः_सङ्कलय,
        समुच्चयः_सङ्कलय_Range,
        समुच्चयः_सङ्कलय_String,
        शून्यसूचकः, // don't need remove()
        शून्यसूचकः // don't need removeRange()
    };

    विकल्प(src) {
    पक्ष UPROPS_SRC_CHAR:
        uchar_सङ्कलय_Property_Starts(&sa, &error_संहिता);
        भङ्ग;
    पक्ष UPROPS_SRC_PROPSVEC:
        upropsvec_सङ्कलय_Property_Starts(&sa, &error_संहिता);
        भङ्ग;
    पक्ष UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_सङ्कलय_Property_Starts(&sa, &error_संहिता);
        upropsvec_सङ्कलय_Property_Starts(&sa, &error_संहिता);
        भङ्ग;
#यदि !UCONFIG_NO_NORMALIZATION
    पक्ष UPROPS_SRC_CASE_AND_NORM: {
        ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_संहिता);
        यदि(U_SUCCESS(error_संहिता)) {
            impl->सङ्कलय_Property_Starts(&sa, error_संहिता);
        }
        ucase_सङ्कलय_Property_Starts(&sa, &error_संहिता);
        भङ्ग;
    }
    पक्ष UPROPS_SRC_NFC: {
        ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_संहिता);
        यदि(U_SUCCESS(error_संहिता)) {
            impl->सङ्कलय_Property_Starts(&sa, error_संहिता);
        }
        भङ्ग;
    }
    पक्ष UPROPS_SRC_NFKC: {
        ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(error_संहिता);
        यदि(U_SUCCESS(error_संहिता)) {
            impl->सङ्कलय_Property_Starts(&sa, error_संहिता);
        }
        भङ्ग;
    }
    पक्ष UPROPS_SRC_NFKC_CF: {
        ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(error_संहिता);
        यदि(U_SUCCESS(error_संहिता)) {
            impl->सङ्कलय_Property_Starts(&sa, error_संहिता);
        }
        भङ्ग;
    }
    पक्ष UPROPS_SRC_NFC_CANON_ITER: {
        ध्रुव Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_संहिता);
        यदि(U_SUCCESS(error_संहिता)) {
            impl->सङ्कलय_Canon_Iter_Property_Starts(&sa, error_संहिता);
        }
        भङ्ग;
    }
#यदि_अन्तः
    पक्ष UPROPS_SRC_CASE:
        ucase_सङ्कलय_Property_Starts(&sa, &error_संहिता);
        भङ्ग;
    पक्ष UPROPS_SRC_BIDI:
        ubidi_सङ्कलय_Property_Starts(&sa, &error_संहिता);
        भङ्ग;
    पक्ष UPROPS_SRC_INPC:
    पक्ष UPROPS_SRC_INSC:
    पक्ष UPROPS_SRC_VO:
        uprops_सङ्कलय_Property_Starts(src, &sa, &error_संहिता);
        भङ्ग;
    पक्ष UPROPS_SRC_EMOJI: {
        ध्रुव icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_संहिता);
        यदि (U_SUCCESS(error_संहिता)) {
            ep->सङ्कलय_Property_Starts(&sa, error_संहिता);
        }
        भङ्ग;
    }
    पक्ष UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.सङ्कलय(sa.समुच्चयः, 0x2FFE);
        sa.सङ्कलय(sa.समुच्चयः, 0x2FFF + 1);
        भङ्ग;
    पक्ष UPROPS_SRC_ID_COMPAT_MATH:
    पक्ष UPROPS_SRC_MCM:
        uprops_सङ्कलय_Property_Starts(src, &sa, &error_संहिता);
        भङ्ग;
    पक्ष UPROPS_SRC_खण्डम्:
        ublock_सङ्कलय_Property_Starts(&sa, error_संहिता);
        भङ्ग;
    स्वभाव:
        error_संहिता = U_आन्तरिकम्_क्रमादेशः_ERROR;
        भङ्ग;
    }

    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    यदि (incl->isBogus()) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].f_समुच्चयः = incl.orphan();
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ध्रुव Unicode_समुच्चयः *getInclusionsForSource(UPropertySource src, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    यदि (src < 0 || UPROPS_SRC_गणनम् <= src) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, error_संहिता);
    प्रत्यागम i.f_समुच्चयः;
}

शून्य U_CALLCONV initIntPropInclusion(UProperty prop, UError_संहिता &error_संहिता) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_START <= prop && prop < UCHAR_INT_सीमा);
    int32_t incl_सूचकांक = UPROPS_SRC_गणनम् + (prop - UCHAR_INT_START);
    U_ASSERT(gInclusions[incl_सूचकांक].f_समुच्चयः == शून्यसूचकः);
    UPropertySource src = uprops_getSource(prop);
    ध्रुव Unicode_समुच्चयः *incl = getInclusionsForSource(src, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }

    LocalPointer<Unicode_समुच्चयः> intPropIncl(नूतन Unicode_समुच्चयः(0, 0));
    यदि (intPropIncl.isNull()) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    int32_t अङ्कः_Ranges = incl->get_Range_गणनम्();
    int32_t पूर्व_मानम् = 0;
    प्रति (int32_t i = 0; i < अङ्कः_Ranges; ++i) {
        UChar32 range_अन्तः = incl->get_Range_अन्तः(i);
        प्रति (UChar32 c = incl->getRangeStart(i); c <= range_अन्तः; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_मानम्(c, prop);
            यदि (value != पूर्व_मानम्) {
                intPropIncl->सङ्कलय(c);
                पूर्व_मानम् = value;
            }
        }
    }

    यदि (intPropIncl->isBogus()) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[incl_सूचकांक].f_समुच्चयः = intPropIncl.orphan();
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ध्रुव Unicode_समुच्चयः *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    यदि (UCHAR_INT_START <= prop && prop < UCHAR_INT_सीमा) {
        int32_t incl_सूचकांक = UPROPS_SRC_गणनम् + (prop - UCHAR_INT_START);
        Inclusion &i = gInclusions[incl_सूचकांक];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, error_संहिता);
        प्रत्यागम i.f_समुच्चयः;
    } अन्यथा {
        UPropertySource src = uprops_getSource(prop);
        प्रत्यागम getInclusionsForSource(src, error_संहिता);
    }
}

U_NAMESPACE_अन्तः

नामाकाश {

Unicode_समुच्चयः *make_समुच्चयः(UProperty property, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    LocalPointer<Unicode_समुच्चयः> समुच्चयः(नूतन Unicode_समुच्चयः());
    यदि (समुच्चयः.isNull()) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    यदि (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ध्रुव icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
        USetAdder sa = {
            reinterpret_cast<USet*>(समुच्चयः.getAlias()),
            समुच्चयः_सङ्कलय,
            समुच्चयः_सङ्कलय_Range,
            समुच्चयः_सङ्कलय_String,
            शून्यसूचकः, // don't need remove()
            शून्यसूचकः // don't need removeRange()
        };
        ep->सङ्कलय_वाक्यानि(&sa, property, error_संहिता);
        यदि (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            समुच्चयः->freeze();
            प्रत्यागम समुच्चयः.orphan();
        }
    }

    ध्रुव Unicode_समुच्चयः *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    int32_t अङ्कः_Ranges = inclusions->get_Range_गणनम्();
    UChar32 startHasProperty = -1;

    प्रति (int32_t i = 0; i < अङ्कः_Ranges; ++i) {
        UChar32 range_अन्तः = inclusions->get_Range_अन्तः(i);
        प्रति (UChar32 c = inclusions->getRangeStart(i); c <= range_अन्तः; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            यदि (u_has_द्विमान_Property(c, property)) {
                यदि (startHasProperty < 0) {
                    // Transition from false to true.
                    startHasProperty = c;
                }
            } अन्यथा यदि (startHasProperty >= 0) {
                // Transition from true to false.
                समुच्चयः->सङ्कलय(startHasProperty, c - 1);
                startHasProperty = -1;
            }
        }
    }
    यदि (startHasProperty >= 0) {
        समुच्चयः->सङ्कलय(startHasProperty, 0x10FFFF);
    }
    समुच्चयः->freeze();
    प्रत्यागम समुच्चयः.orphan();
}

UCPMap *makeMap(UProperty property, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    uint32_t null_मानम् = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::LocalUMutableCPTriePointer mutableTrie(
        umutablecptrie_open(null_मानम्, null_मानम्, &error_संहिता));
    ध्रुव Unicode_समुच्चयः *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    int32_t अङ्कः_Ranges = inclusions->get_Range_गणनम्();
    UChar32 start = 0;
    uint32_t value = null_मानम्;

    प्रति (int32_t i = 0; i < अङ्कः_Ranges; ++i) {
        UChar32 range_अन्तः = inclusions->get_Range_अन्तः(i);
        प्रति (UChar32 c = inclusions->getRangeStart(i); c <= range_अन्तः; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t अग्र_मानम् = u_get_Int_Property_मानम्(c, property);
            यदि (value != अग्र_मानम्) {
                यदि (value != null_मानम्) {
                    umutablecptrie_समुच्चयः_Range(mutableTrie.getAlias(), start, c - 1, value, &error_संहिता);
                }
                start = c;
                value = अग्र_मानम्;
            }
        }
    }
    यदि (value != 0) {
        umutablecptrie_समुच्चयः_Range(mutableTrie.getAlias(), start, 0x10FFFF, value, &error_संहिता);
    }

    UCPTrieType type;
    यदि (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } अन्यथा {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_मानम्_विस्तारः मानम्_विस्तारः;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_मानम्(property);
    यदि (max <= 0xff) {
        मानम्_विस्तारः = UCPTRIE_मानम्_द्वयङ्काः_8;
    } अन्यथा यदि (max <= 0xffff) {
        मानम्_विस्तारः = UCPTRIE_मानम्_द्वयङ्काः_16;
    } अन्यथा {
        मानम्_विस्तारः = UCPTRIE_मानम्_द्वयङ्काः_32;
    }
    प्रत्यागम reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, मानम्_विस्तारः, &error_संहिता));
}

}  // namespace

U_NAMESPACE_BEGIN

ध्रुव Unicode_समुच्चयः *CharacterProperties::get_द्विमान_Property_समुच्चयः(UProperty property, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    यदि (property < 0 || UCHAR_द्विमान_सीमा <= property) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    Mutex m(&cpMutex);
    Unicode_समुच्चयः *समुच्चयः = sets[property];
    यदि (समुच्चयः == शून्यसूचकः) {
        sets[property] = समुच्चयः = make_समुच्चयः(property, error_संहिता);
    }
    प्रत्यागम समुच्चयः;
}

U_NAMESPACE_अन्तः

U_NAMESPACE_USE

U_CAPI ध्रुव USet * U_EXPORT2
u_get_द्विमान_Property_समुच्चयः(UProperty property, UError_संहिता *p_Error_संहिता) {
    ध्रुव Unicode_समुच्चयः *समुच्चयः = CharacterProperties::get_द्विमान_Property_समुच्चयः(property, *p_Error_संहिता);
    प्रत्यागम U_SUCCESS(*p_Error_संहिता) ? समुच्चयः->गन्तव्यम्_USet() : शून्यसूचकः;
}

U_CAPI ध्रुव UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    यदि (property < UCHAR_INT_START || UCHAR_INT_सीमा <= property) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_START];
    यदि (map == शून्यसूचकः) {
        maps[property - UCHAR_INT_START] = map = makeMap(property, *p_Error_संहिता);
    }
    प्रत्यागम map;
}
