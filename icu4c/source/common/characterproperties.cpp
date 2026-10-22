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

استعمال icu::Local_اشارہ_گر;
#اگر !UCONFIG_NO_NORMALIZATION
استعمال icu::Normalizer2Factory;
استعمال icu::Normalizer2Impl;
#اگر_ختم
استعمال icu::UInitOnce;
استعمال icu::Unicode_سیٹ;

نامخلا {

UBool U_CALLCONV characterproperties_cleanup();

constexpr int32_t عدد_INCLUSIONS = UPROPS_SRC_گنتی + (UCHAR_INT_حد_بندی - UCHAR_INT_آغاز);

ساخت Inclusion {
    Unicode_سیٹ  *f_سیٹ = خالی_اشاریہ;
    UInitOnce    fInitOnce {};
};
Inclusion gInclusions[عدد_INCLUSIONS]; // cached getInclusions()

Unicode_سیٹ *sets[UCHAR_ثنائی_حد_بندی] = {};

UCPMap *maps[UCHAR_INT_حد_بندی - UCHAR_INT_آغاز] = {};

icu::UMutex cpMutex;

//----------------------------------------------------------------
// Inclusions list
//----------------------------------------------------------------

// USetAdder implementation
// Does not use uset.h to reduce code dependencies
خالی U_CALLCONV
سیٹ_جوڑو(USet *سیٹ, UChar32 c) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(c);
}

خالی U_CALLCONV
سیٹ_جوڑو_Range(USet *سیٹ, UChar32 آغاز, UChar32 انتہا) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(آغاز, انتہا);
}

خالی U_CALLCONV
سیٹ_جوڑو_String(USet *سیٹ, ثابت char16_t *str, int32_t length) {
    reinterpret_cast<Unicode_سیٹ*>(سیٹ)->جوڑو(icu::UnicodeString(static_cast<UBool>(length < 0), str, length));
}

UBool U_CALLCONV characterproperties_cleanup() {
    برائے (Inclusion &in: gInclusions) {
        حذف in.f_سیٹ;
        in.f_سیٹ = خالی_اشاریہ;
        in.fInitOnce.reset();
    }
    برائے (int32_t i = 0; i < UPRV_LENGTHOF(sets); ++i) {
        حذف sets[i];
        sets[i] = خالی_اشاریہ;
    }
    برائے (int32_t i = 0; i < UPRV_LENGTHOF(maps); ++i) {
        ucptrie_close(reinterpret_cast<UCPTrie *>(maps[i]));
        maps[i] = خالی_اشاریہ;
    }
    واپس سچ;
}

خالی U_CALLCONV initInclusion(UProperty_ماخذ src, UError_رمز &error_رمز) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(0 <= src && src < UPROPS_SRC_گنتی);
    اگر (src == UPROPS_SRC_NONE) {
        error_رمز = U_اندرونی_پروگرام_ERROR;
        واپس;
    }
    U_ASSERT(gInclusions[src].f_سیٹ == خالی_اشاریہ);

    Local_اشارہ_گر<Unicode_سیٹ> incl(نیا Unicode_سیٹ());
    اگر (incl.isNull()) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    USetAdder sa = {
        reinterpret_cast<USet*>(incl.getAlias()),
        سیٹ_جوڑو,
        سیٹ_جوڑو_Range,
        سیٹ_جوڑو_String,
        خالی_اشاریہ, // don't need remove()
        خالی_اشاریہ // don't need removeRange()
    };

    انتخاب(src) {
    حالت UPROPS_SRC_CHAR:
        uchar_جوڑو_Property_Starts(&sa, &error_رمز);
        توڑو;
    حالت UPROPS_SRC_PROPSVEC:
        upropsvec_جوڑو_Property_Starts(&sa, &error_رمز);
        توڑو;
    حالت UPROPS_SRC_CHAR_AND_PROPSVEC:
        uchar_جوڑو_Property_Starts(&sa, &error_رمز);
        upropsvec_جوڑو_Property_Starts(&sa, &error_رمز);
        توڑو;
#اگر !UCONFIG_NO_NORMALIZATION
    حالت UPROPS_SRC_CASE_AND_NORM: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_رمز);
        اگر(U_SUCCESS(error_رمز)) {
            impl->جوڑو_Property_Starts(&sa, error_رمز);
        }
        ucase_جوڑو_Property_Starts(&sa, &error_رمز);
        توڑو;
    }
    حالت UPROPS_SRC_NFC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_رمز);
        اگر(U_SUCCESS(error_رمز)) {
            impl->جوڑو_Property_Starts(&sa, error_رمز);
        }
        توڑو;
    }
    حالت UPROPS_SRC_NFKC: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKCImpl(error_رمز);
        اگر(U_SUCCESS(error_رمز)) {
            impl->جوڑو_Property_Starts(&sa, error_رمز);
        }
        توڑو;
    }
    حالت UPROPS_SRC_NFKC_CF: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFKC_CFImpl(error_رمز);
        اگر(U_SUCCESS(error_رمز)) {
            impl->جوڑو_Property_Starts(&sa, error_رمز);
        }
        توڑو;
    }
    حالت UPROPS_SRC_NFC_CANON_ITER: {
        ثابت Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(error_رمز);
        اگر(U_SUCCESS(error_رمز)) {
            impl->جوڑو_Canon_Iter_Property_Starts(&sa, error_رمز);
        }
        توڑو;
    }
#اگر_ختم
    حالت UPROPS_SRC_CASE:
        ucase_جوڑو_Property_Starts(&sa, &error_رمز);
        توڑو;
    حالت UPROPS_SRC_BIDI:
        ubidi_جوڑو_Property_Starts(&sa, &error_رمز);
        توڑو;
    حالت UPROPS_SRC_INPC:
    حالت UPROPS_SRC_INSC:
    حالت UPROPS_SRC_VO:
        uprops_جوڑو_Property_Starts(src, &sa, &error_رمز);
        توڑو;
    حالت UPROPS_SRC_EMOJI: {
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_رمز);
        اگر (U_SUCCESS(error_رمز)) {
            ep->جوڑو_Property_Starts(&sa, error_رمز);
        }
        توڑو;
    }
    حالت UPROPS_SRC_IDSU:
        // New in Unicode 15.1 for just two characters.
        sa.جوڑو(sa.سیٹ, 0x2FFE);
        sa.جوڑو(sa.سیٹ, 0x2FFF + 1);
        توڑو;
    حالت UPROPS_SRC_ID_COMPAT_MATH:
    حالت UPROPS_SRC_MCM:
        uprops_جوڑو_Property_Starts(src, &sa, &error_رمز);
        توڑو;
    حالت UPROPS_SRC_بلاک:
        ublock_جوڑو_Property_Starts(&sa, error_رمز);
        توڑو;
    طےشدہ:
        error_رمز = U_اندرونی_پروگرام_ERROR;
        توڑو;
    }

    اگر (U_FAILURE(error_رمز)) {
        واپس;
    }
    اگر (incl->isBogus()) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    // Compact for caching.
    incl->compact();
    gInclusions[src].f_سیٹ = incl.orphan();
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

ثابت Unicode_سیٹ *get_Inclusions_For_ماخذ(UProperty_ماخذ src, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    اگر (src < 0 || UPROPS_SRC_گنتی <= src) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    Inclusion &i = gInclusions[src];
    umtx_initOnce(i.fInitOnce, &initInclusion, src, error_رمز);
    واپس i.f_سیٹ;
}

خالی U_CALLCONV initIntPropInclusion(UProperty prop, UError_رمز &error_رمز) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(UCHAR_INT_آغاز <= prop && prop < UCHAR_INT_حد_بندی);
    int32_t incl_اشاریہ = UPROPS_SRC_گنتی + (prop - UCHAR_INT_آغاز);
    U_ASSERT(gInclusions[incl_اشاریہ].f_سیٹ == خالی_اشاریہ);
    UProperty_ماخذ src = uprops_get_ماخذ(prop);
    ثابت Unicode_سیٹ *incl = get_Inclusions_For_ماخذ(src, error_رمز);
    اگر (U_FAILURE(error_رمز)) {
        واپس;
    }

    Local_اشارہ_گر<Unicode_سیٹ> intPropIncl(نیا Unicode_سیٹ(0, 0));
    اگر (intPropIncl.isNull()) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    int32_t عدد_Ranges = incl->get_Range_گنتی();
    int32_t پچھلا_قدر = 0;
    برائے (int32_t i = 0; i < عدد_Ranges; ++i) {
        UChar32 range_انتہا = incl->get_Range_انتہا(i);
        برائے (UChar32 c = incl->get_Range_آغاز(i); c <= range_انتہا; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            int32_t value = u_get_Int_Property_قدر(c, prop);
            اگر (value != پچھلا_قدر) {
                intPropIncl->جوڑو(c);
                پچھلا_قدر = value;
            }
        }
    }

    اگر (intPropIncl->isBogus()) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    // Compact for caching.
    intPropIncl->compact();
    gInclusions[incl_اشاریہ].f_سیٹ = intPropIncl.orphan();
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CHARACTERPROPERTIES, characterproperties_cleanup);
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت Unicode_سیٹ *CharacterProperties::getInclusionsForProperty(
        UProperty prop, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    اگر (UCHAR_INT_آغاز <= prop && prop < UCHAR_INT_حد_بندی) {
        int32_t incl_اشاریہ = UPROPS_SRC_گنتی + (prop - UCHAR_INT_آغاز);
        Inclusion &i = gInclusions[incl_اشاریہ];
        umtx_initOnce(i.fInitOnce, &initIntPropInclusion, prop, error_رمز);
        واپس i.f_سیٹ;
    } ورنہ {
        UProperty_ماخذ src = uprops_get_ماخذ(prop);
        واپس get_Inclusions_For_ماخذ(src, error_رمز);
    }
}

U_NAMESPACE_انتہا

نامخلا {

Unicode_سیٹ *make_سیٹ(UProperty property, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    Local_اشارہ_گر<Unicode_سیٹ> سیٹ(نیا Unicode_سیٹ());
    اگر (سیٹ.isNull()) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    اگر (UCHAR_BASIC_EMOJI <= property && property <= UCHAR_RGI_EMOJI) {
        // property of strings
        ثابت icu::EmojiProps *ep = icu::EmojiProps::getSingleton(error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
        USetAdder sa = {
            reinterpret_cast<USet*>(سیٹ.getAlias()),
            سیٹ_جوڑو,
            سیٹ_جوڑو_Range,
            سیٹ_جوڑو_String,
            خالی_اشاریہ, // don't need remove()
            خالی_اشاریہ // don't need removeRange()
        };
        ep->جوڑو_لڑیاں(&sa, property, error_رمز);
        اگر (property != UCHAR_BASIC_EMOJI && property != UCHAR_RGI_EMOJI) {
            // property of _only_ strings
            سیٹ->freeze();
            واپس سیٹ.orphan();
        }
    }

    ثابت Unicode_سیٹ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    int32_t عدد_Ranges = inclusions->get_Range_گنتی();
    UChar32 آغاز_Has_Property = -1;

    برائے (int32_t i = 0; i < عدد_Ranges; ++i) {
        UChar32 range_انتہا = inclusions->get_Range_انتہا(i);
        برائے (UChar32 c = inclusions->get_Range_آغاز(i); c <= range_انتہا; ++c) {
            // TODO: Get a UCharacterProperty.BinaryProperty to avoid the property dispatch.
            اگر (u_has_ثنائی_Property(c, property)) {
                اگر (آغاز_Has_Property < 0) {
                    // Transition from false to true.
                    آغاز_Has_Property = c;
                }
            } ورنہ اگر (آغاز_Has_Property >= 0) {
                // Transition from true to false.
                سیٹ->جوڑو(آغاز_Has_Property, c - 1);
                آغاز_Has_Property = -1;
            }
        }
    }
    اگر (آغاز_Has_Property >= 0) {
        سیٹ->جوڑو(آغاز_Has_Property, 0x10FFFF);
    }
    سیٹ->freeze();
    واپس سیٹ.orphan();
}

UCPMap *makeMap(UProperty property, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    uint32_t null_قدر = property == UCHAR_SCRIPT ? USCRIPT_UNKNOWN : 0;
    icu::Local_UMutable_CPTrie_اشارہ_گر mutableTrie(
        umutablecptrie_open(null_قدر, null_قدر, &error_رمز));
    ثابت Unicode_سیٹ *inclusions =
        icu::CharacterProperties::getInclusionsForProperty(property, error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    int32_t عدد_Ranges = inclusions->get_Range_گنتی();
    UChar32 آغاز = 0;
    uint32_t value = null_قدر;

    برائے (int32_t i = 0; i < عدد_Ranges; ++i) {
        UChar32 range_انتہا = inclusions->get_Range_انتہا(i);
        برائے (UChar32 c = inclusions->get_Range_آغاز(i); c <= range_انتہا; ++c) {
            // TODO: Get a UCharacterProperty.IntProperty to avoid the property dispatch.
            uint32_t اگلا_قدر = u_get_Int_Property_قدر(c, property);
            اگر (value != اگلا_قدر) {
                اگر (value != null_قدر) {
                    umutablecptrie_سیٹ_Range(mutableTrie.getAlias(), آغاز, c - 1, value, &error_رمز);
                }
                آغاز = c;
                value = اگلا_قدر;
            }
        }
    }
    اگر (value != 0) {
        umutablecptrie_سیٹ_Range(mutableTrie.getAlias(), آغاز, 0x10FFFF, value, &error_رمز);
    }

    UCPTrieType type;
    اگر (property == UCHAR_BIDI_CLASS || property == UCHAR_GENERAL_CATEGORY) {
        type = UCPTRIE_TYPE_FAST;
    } ورنہ {
        type = UCPTRIE_TYPE_SMALL;
    }
    UCPTrie_قدر_چوڑائی قدر_چوڑائی;
    // TODO: UCharacterProperty.IntProperty
    int32_t max = u_get_Int_Property_Max_قدر(property);
    اگر (max <= 0xff) {
        قدر_چوڑائی = UCPTRIE_قدر_بٹ_8;
    } ورنہ اگر (max <= 0xffff) {
        قدر_چوڑائی = UCPTRIE_قدر_بٹ_16;
    } ورنہ {
        قدر_چوڑائی = UCPTRIE_قدر_بٹ_32;
    }
    واپس reinterpret_cast<UCPMap *>(
        umutablecptrie_buildImmutable(mutableTrie.getAlias(), type, قدر_چوڑائی, &error_رمز));
}

}  // namespace

U_NAMESPACE_BEGIN

ثابت Unicode_سیٹ *CharacterProperties::get_ثنائی_Property_سیٹ(UProperty property, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    اگر (property < 0 || UCHAR_ثنائی_حد_بندی <= property) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    Mutex m(&cpMutex);
    Unicode_سیٹ *سیٹ = sets[property];
    اگر (سیٹ == خالی_اشاریہ) {
        sets[property] = سیٹ = make_سیٹ(property, error_رمز);
    }
    واپس سیٹ;
}

U_NAMESPACE_انتہا

U_NAMESPACE_USE

U_CAPI ثابت USet * U_EXPORT2
u_get_ثنائی_Property_سیٹ(UProperty property, UError_رمز *p_Error_رمز) {
    ثابت Unicode_سیٹ *سیٹ = CharacterProperties::get_ثنائی_Property_سیٹ(property, *p_Error_رمز);
    واپس U_SUCCESS(*p_Error_رمز) ? سیٹ->مقصد_USet() : خالی_اشاریہ;
}

U_CAPI ثابت UCPMap * U_EXPORT2
u_getIntPropertyMap(UProperty property, UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) { واپس خالی_اشاریہ; }
    اگر (property < UCHAR_INT_آغاز || UCHAR_INT_حد_بندی <= property) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    Mutex m(&cpMutex);
    UCPMap *map = maps[property - UCHAR_INT_آغاز];
    اگر (map == خالی_اشاریہ) {
        maps[property - UCHAR_INT_آغاز] = map = makeMap(property, *p_Error_رمز);
    }
    واپس map;
}
