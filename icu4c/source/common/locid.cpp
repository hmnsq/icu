// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 **********************************************************************
 *   Copyright (C) 1997-2016, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 **********************************************************************
*
* File locid.cpp
*
* Created by: Richard Gillam
*
* Modification History:
*
*   Date        Name        Description
*   02/11/97    aliu        Changed gLocPath to fgDataDirectory and added
*                           methods to get and set it.
*   04/02/97    aliu        Made operator!= inline; fixed return value
*                           of getName().
*   04/15/97    aliu        Cleanup for AIX/Win32.
*   04/24/97    aliu        Numerous changes per code review.
*   08/18/98    stephen     Changed getDisplayName()
*                           Added SIMPLIFIED_CHINESE, TRADITIONAL_CHINESE
*                           Added getISOCountries(), getISOLanguages(),
*                           getLanguagesForCountry()
*   03/16/99    bertrand    rehaul.
*   07/21/99    stephen     Added U_CFUNC setDefault
*   11/09/99    weiv        Added const char * getName() const;
*   04/12/00    srl         removing unicodestring api's and cached hash code
*   08/10/01    grhoten     Change the static Locales to accessor functions
******************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ <cstddef>
#ਅਨ੍ਤਰ੍ਭਾਵ <optional>
#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>
#ਅਨ੍ਤਰ੍ਭਾਵ <type_traits>
#ਅਨ੍ਤਰ੍ਭਾਵ <utility>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localebuilder.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localpointer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/strenum.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstrmap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fixedstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulocimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uniquecharstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

#ਜੇ UPRV_HAS_FEATURE(ਸਮ੍ਰਿਤੀ_sanitizer)
#ਅਨ੍ਤਰ੍ਭਾਵ <sanitizer/msan_interface.h>
#ਜੇ_ਅੰਤ

U_NAMESPACE_BEGIN

ਸਥਿਰ Locale   *gLocaleCache = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
ਸਥਿਰ UMutex gDefaultLocaleMutex;
ਸਥਿਰ UHashtable *gDefaultLocalesHashT = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ Locale *gDefaultLocale = ਖਾਲੀ_ਸੂਚਕ;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULOC_STRING_ਹੱਦਬੰਦੀ 357913941

U_NAMESPACE_ਸਿਰਾ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ELocalePos {
    eENGLISH,
    eFRENCH,
    eGERMAN,
    eITALIAN,
    eJAPANESE,
    eKOREAN,
    eCHINESE,

    eFRANCE,
    eGERMANY,
    eITALY,
    eJAPAN,
    eKOREA,
    eCHINA,      /* Alias for PRC */
    eTAIWAN,
    eUK,
    eUS,
    eCANADA,
    eCANADA_FRENCH,
    e_ਜੜ੍ਹ,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

ਨਾਮਾਕਾਸ਼ {

//
// Deleter function for Locales owned by the default Locale hash table/
//
ਖਾਲੀ U_CALLCONV
deleteLocale(ਖਾਲੀ *obj) {
    ਮਿਟਾਓ static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    ਮਿਟਾਓ [] gLocaleCache;
    gLocaleCache = ਖਾਲੀ_ਸੂਚਕ;
    gLocaleCacheInitOnce.reset();

    ਜੇ (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = ਖਾਲੀ_ਸੂਚਕ;
    }
    gDefaultLocale = ਖਾਲੀ_ਸੂਚਕ;
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ U_CALLCONV locale_init(UError_ਕੋਡ &ਸਥਿਤੀ) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == ਖਾਲੀ_ਸੂਚਕ);
    gLocaleCache = ਨਵਾਂ Locale[static_cast<ਪੂਰਨ>(eMAX_LOCALES)];
    ਜੇ (gLocaleCache == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_ਜੜ੍ਹ]          = Locale("");
    gLocaleCache[eENGLISH]       = Locale("en");
    gLocaleCache[eFRENCH]        = Locale("fr");
    gLocaleCache[eGERMAN]        = Locale("de");
    gLocaleCache[eITALIAN]       = Locale("it");
    gLocaleCache[eJAPANESE]      = Locale("ja");
    gLocaleCache[eKOREAN]        = Locale("ko");
    gLocaleCache[eCHINESE]       = Locale("zh");
    gLocaleCache[eFRANCE]        = Locale("fr", "FR");
    gLocaleCache[eGERMANY]       = Locale("de", "DE");
    gLocaleCache[eITALY]         = Locale("it", "IT");
    gLocaleCache[eJAPAN]         = Locale("ja", "JP");
    gLocaleCache[eKOREA]         = Locale("ko", "KR");
    gLocaleCache[eCHINA]         = Locale("zh", "CN");
    gLocaleCache[eTAIWAN]        = Locale("zh", "TW");
    gLocaleCache[eUK]            = Locale("en", "GB");
    gLocaleCache[eUS]            = Locale("en", "US");
    gLocaleCache[eCANADA]        = Locale("en", "CA");
    gLocaleCache[eCANADA_FRENCH] = Locale("fr", "CA");
}

}  // namespace

U_NAMESPACE_BEGIN

Locale *locale_ਸੈੱਟ_default_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *id, UError_ਕੋਡ& ਸਥਿਤੀ) {
    // Synchronize this entire function.
    Mutex ਤਾਲਾ(&gDefaultLocaleMutex);

    UBool canonicalize = ਝੂਠ;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    ਜੇ (id == ਖਾਲੀ_ਸੂਚਕ) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = ਸੱਚ; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, ਸਥਿਤੀ) : ulocimp_getName(id, ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ gDefaultLocale;
    }

    ਜੇ (gDefaultLocalesHashT == ਖਾਲੀ_ਸੂਚਕ) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ gDefaultLocale;
        }
        uhash_ਸੈੱਟ_ਮੁੱਲ_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.ਅੰਕੜੇ()));
    ਜੇ (newDefault == ਖਾਲੀ_ਸੂਚਕ) {
        newDefault = ਨਵਾਂ Locale(Locale::eBOGUS);
        ਜੇ (newDefault == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ gDefaultLocale;
        }
        newDefault->init(localeNameBuf.ਅੰਕੜੇ(), ਝੂਠ);
        uhash_put(gDefaultLocalesHashT, const_cast<ਅੱਖਰ*>(newDefault->getName()), newDefault, &ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    ਵਾਪਸ gDefaultLocale;
}

U_NAMESPACE_ਸਿਰਾ

/* sfb 07/21/99 */
U_CFUNC ਖਾਲੀ
locale_ਸੈੱਟ_default(ਅਚਲ ਅੱਖਰ *id)
{
    U_NAMESPACE_USE
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    locale_ਸੈੱਟ_default_ਅੰਦਰੂਨੀ(id, ਸਥਿਤੀ);
}
/*ਸਿਰਾ*/

U_CFUNC ਅਚਲ ਅੱਖਰ *
locale_get_default()
{
    U_NAMESPACE_USE
    ਵਾਪਸ Locale::getDefault().getName();
}

ਨਾਮਾਕਾਸ਼ {

ਸਾਂਚਾ <ਸਵੈਚਾਲਿਤ ਖੇਤਰ, ਕਿਸਮ_ਨਾਂ T>
ਖਾਲੀ copy_ਮੰਜ਼ਿਲ_Array(std::string_view sv, T* that) {
    ਸਵੈਚਾਲਿਤ& ਖੇਤਰ_2 = that->*ਖੇਤਰ;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(ਖੇਤਰ_2)>>;
    static_assert(capacity > 0);
    ਜੇ (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(ਖੇਤਰ_2, sv.ਅੰਕੜੇ(), sv.size());
    }
    ਖੇਤਰ_2[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

ਖਾਲੀ Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copy_ਮੰਜ਼ਿਲ_Array<&Nest::language>(language, ਇਹ);
    copy_ਮੰਜ਼ਿਲ_Array<&Nest::script>(script, ਇਹ);
    copy_ਮੰਜ਼ਿਲ_Array<&Nest::region>(region, ਇਹ);
    ਇਹ->variantBegin = variantBegin;
}

Locale::Nest::Nest(ਢੇਰ_2&& ਢੇਰ_3, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(ਢੇਰ_2, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(ਢੇਰ_2, region));
    U_ASSERT(ਇਹ == reinterpret_cast<Nest*>(&ਢੇਰ_3));
#ਜੇ UPRV_HAS_FEATURE(ਸਮ੍ਰਿਤੀ_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&ਢੇਰ_3, ਆਕਾਰ(ਢੇਰ_2));
#ਜੇ_ਅੰਤ
    copy_ਮੰਜ਼ਿਲ_Array<&Nest::script>(ਢੇਰ_3.script, ਇਹ);
    copy_ਮੰਜ਼ਿਲ_Array<&Nest::region>(ਢੇਰ_3.region, ਇਹ);
    ਇਹ->variantBegin = variantBegin;
    *ਇਹ->ਅਧਾਰ_Name = '\0';
}

ਬਣਤਰ Locale::ਢੇਰ_2::Alloc : ਜਨਤਕ UMemory {
    FixedString ਭਰਿਆ_Name;
    FixedString ਅਧਾਰ_Name;
    int32_t variantBegin;

    ਅਚਲ ਅੱਖਰ* getVariant() ਅਚਲ U_LIFETIME_BOUND {
        ਵਾਪਸ variantBegin == 0 ? "" : get_ਅਧਾਰ_Name() + variantBegin;
    }
    ਅਚਲ ਅੱਖਰ* get_ਭਰਿਆ_Name() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ ਭਰਿਆ_Name.ਅੰਕੜੇ(); }
    ਅਚਲ ਅੱਖਰ* get_ਅਧਾਰ_Name() ਅਚਲ U_LIFETIME_BOUND {
        ਜੇ (ਅਧਾਰ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            ਜੇ (ਅਚਲ ਅੱਖਰ* name = ਭਰਿਆ_Name.ਅੰਕੜੇ(); *name != '@') {
                ਵਾਪਸ name;
            }
        }
        ਵਾਪਸ ਅਧਾਰ_Name.ਅੰਕੜੇ();
    }

    Alloc(int32_t variantBegin) : ਭਰਿਆ_Name(), ਅਧਾਰ_Name(), variantBegin(variantBegin) {}

    Alloc(ਅਚਲ Alloc& other, UError_ਕੋਡ& ਸਥਿਤੀ)
        : ਭਰਿਆ_Name(), ਅਧਾਰ_Name(), variantBegin(other.variantBegin) {
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            ਜੇ (!other.ਭਰਿਆ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                ਭਰਿਆ_Name = other.ਭਰਿਆ_Name;
                ਜੇ (ਭਰਿਆ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                    ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                } ਨਹੀਂਤਾਂ {
                    ਜੇ (!other.ਅਧਾਰ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                        ਅਧਾਰ_Name = other.ਅਧਾਰ_Name;
                        ਜੇ (ਅਧਾਰ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = ਮਿਟਾਓ;

    ~Alloc() = ਮੂਲ;
};

ਅਚਲ ਅੱਖਰ* Locale::ਢੇਰ_2::getVariant() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ ptr->getVariant(); }
ਅਚਲ ਅੱਖਰ* Locale::ਢੇਰ_2::get_ਭਰਿਆ_Name() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ ptr->get_ਭਰਿਆ_Name(); }
ਅਚਲ ਅੱਖਰ* Locale::ਢੇਰ_2::get_ਅਧਾਰ_Name() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ ptr->get_ਅਧਾਰ_Name(); }

Locale::ਢੇਰ_2::ਢੇਰ_2(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = ਨਵਾਂ Alloc(variantBegin);
    ਜੇ (ptr == ਖਾਲੀ_ਸੂਚਕ) {
        type = eBOGUS;
    } ਨਹੀਂਤਾਂ {
        type = e_ਢੇਰ;
        copy_ਮੰਜ਼ਿਲ_Array<&ਢੇਰ_2::language>(language, ਇਹ);
        copy_ਮੰਜ਼ਿਲ_Array<&ਢੇਰ_2::script>(script, ਇਹ);
        copy_ਮੰਜ਼ਿਲ_Array<&ਢੇਰ_2::region>(region, ਇਹ);
    }
}

Locale::ਢੇਰ_2::~ਢੇਰ_2() {
    U_ASSERT(type == e_ਢੇਰ);
    ਮਿਟਾਓ ptr;
}

Locale::ਢੇਰ_2& Locale::ਢੇਰ_2::ਸੰਚਾਲਕ=(ਅਚਲ ਢੇਰ_2& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    ptr = ਨਵਾਂ Alloc(*other.ptr, ਸਥਿਤੀ);
    ਜੇ (ptr == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(ਸਥਿਤੀ)) {
        ਮਿਟਾਓ ptr;
    } ਨਹੀਂਤਾਂ {
        type = e_ਢੇਰ;
        uprv_memcpy(language, other.language, ਆਕਾਰ language);
        uprv_memcpy(script, other.script, ਆਕਾਰ script);
        uprv_memcpy(region, other.region, ਆਕਾਰ region);
    }
    ਵਾਪਸ *ਇਹ;
}

Locale::ਢੇਰ_2& Locale::ਢੇਰ_2::ਸੰਚਾਲਕ=(ਢੇਰ_2&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = e_ਢੇਰ;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, ਆਕਾਰ language);
    uprv_memcpy(script, other.script, ਆਕਾਰ script);
    uprv_memcpy(region, other.region, ਆਕਾਰ region);
    ਵਾਪਸ *ਇਹ;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ BogusFn, ਕਿਸਮ_ਨਾਂ NestFn, ਕਿਸਮ_ਨਾਂ ਢੇਰ_Fn, ਕਿਸਮ_ਨਾਂ... ਦਲੀਲਾਂ>
ਸਵੈਚਾਲਿਤ Locale::Payload::ਦੇਖੋ(BogusFn bogusFn, NestFn nestFn, ਢੇਰ_Fn ਢੇਰ_Fn_2, ਦਲੀਲਾਂ... ਦਲੀਲਾਂ_2) ਅਚਲ {
    ਚੋਣ (type) {
        ਹਾਲਤ eBOGUS:
            ਵਾਪਸ bogusFn(ਦਲੀਲਾਂ_2...);
        ਹਾਲਤ eNEST:
            ਵਾਪਸ nestFn(nest, ਦਲੀਲਾਂ_2...);
        ਹਾਲਤ e_ਢੇਰ:
            ਵਾਪਸ ਢੇਰ_Fn_2(ਢੇਰ_3, ਦਲੀਲਾਂ_2...);
        ਮੂਲ:
            UPRV_UNREACHABLE_EXIT;
    };
}

ਖਾਲੀ Locale::Payload::copy(ਅਚਲ Payload& other) {
    other.ਦੇਖੋ([](Payload*) {},
                [](ਅਚਲ Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ਅਚਲ ਢੇਰ_2& ਢੇਰ_3, Payload* dst) { dst->ਢੇਰ_3 = ਢੇਰ_3; },
                ਇਹ);
}

ਖਾਲੀ Locale::Payload::move(Payload&& other) noexcept {
    other.ਦੇਖੋ(
        [](Payload*) {},
        [](ਅਚਲ Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ਅਚਲ ਢੇਰ_2& ਢੇਰ_3, Payload* dst) { dst->ਢੇਰ_3 = std::move(const_cast<ਢੇਰ_2&>(ਢੇਰ_3)); },
        ਇਹ);
}

Locale::Payload::~Payload() {
    ਜੇ (type == e_ਢੇਰ) { ਢੇਰ_3.~ਢੇਰ_2(); }
}

Locale::Payload::Payload(ਅਚਲ Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::ਸੰਚਾਲਕ=(ਅਚਲ Payload& other) U_LIFETIME_BOUND {
    ਜੇ (ਇਹ != &other) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        copy(other);
    }
    ਵਾਪਸ *ਇਹ;
}

Locale::Payload& Locale::Payload::ਸੰਚਾਲਕ=(Payload&& other) noexcept U_LIFETIME_BOUND {
    ਜੇ (ਇਹ != &other) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        move(std::move(other));
    }
    ਵਾਪਸ *ਇਹ;
}

ਖਾਲੀ Locale::Payload::ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus() {
    ਇਹ->~Payload();
    type = eBOGUS;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ T, ਕਿਸਮ_ਨਾਂ... ਦਲੀਲਾਂ> T& Locale::Payload::emplace(ਦਲੀਲਾਂ&&... ਦਲੀਲਾਂ_2) U_LIFETIME_BOUND {
    ਜੇ constexpr (std::is_same_v<T, Nest>) {
        ਇਹ->~Payload();
        ::ਨਵਾਂ (&nest) Nest(std::forward<ਦਲੀਲਾਂ>(ਦਲੀਲਾਂ_2)...);
        ਵਾਪਸ nest;
    }
    ਜੇ constexpr (std::is_same_v<T, ਢੇਰ_2>) {
        U_ASSERT(type != e_ਢੇਰ);
        ::ਨਵਾਂ (&ਢੇਰ_3) ਢੇਰ_2(std::forward<ਦਲੀਲਾਂ>(ਦਲੀਲਾਂ_2)...);
        ਵਾਪਸ ਢੇਰ_3;
    }
}

ਸਾਂਚਾ <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    ਵਾਪਸ type == eNEST ? &nest : ਖਾਲੀ_ਸੂਚਕ;
}
ਸਾਂਚਾ <> Locale::ਢੇਰ_2* Locale::Payload::get() U_LIFETIME_BOUND {
    ਵਾਪਸ type == e_ਢੇਰ ? &ਢੇਰ_3 : ਖਾਲੀ_ਸੂਚਕ;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEP_CHAR '_'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NULL_CHAR '\0'

Locale::~Locale() = ਮੂਲ;

Locale::Locale()
    : UObject(), payload()
{
    init(ਖਾਲੀ_ਸੂਚਕ, ਝੂਠ);
}

/*
 * Internal constructor to allow construction of a locale object with
 *   NO side effects.   (Default constructor tries to get
 *   the default locale.)
 */
Locale::Locale(Locale::ELocaleType)
    : UObject(), payload()
{
}


Locale::Locale( ਅਚਲ   ਅੱਖਰ * newLanguage,
                ਅਚਲ   ਅੱਖਰ * newCountry,
                ਅਚਲ   ਅੱਖਰ * newVariant,
                ਅਚਲ   ਅੱਖਰ * newKeywords)
    : UObject(), payload()
{
    ਜੇ( (newLanguage==ਖਾਲੀ_ਸੂਚਕ) && (newCountry == ਖਾਲੀ_ਸੂਚਕ) && (newVariant == ਖਾਲੀ_ਸੂਚਕ) )
    {
        init(ਖਾਲੀ_ਸੂਚਕ, ਝੂਠ); /* shortcut */
    }
    ਨਹੀਂਤਾਂ
    {
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        ਜੇ ( newLanguage != ਖਾਲੀ_ਸੂਚਕ )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            ਜੇ ( lsize < 0 || lsize > ULOC_STRING_ਹੱਦਬੰਦੀ ) { // int32 wrap
                ਵਾਪਸ;
            }
        }

        CharString togo(newLanguage, lsize, ਸਥਿਤੀ); // start with newLanguage

        // _Country
        ਜੇ ( newCountry != ਖਾਲੀ_ਸੂਚਕ )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            ਜੇ ( csize < 0 || csize > ULOC_STRING_ਹੱਦਬੰਦੀ ) { // int32 wrap
                ਵਾਪਸ;
            }
        }

        // _Variant
        ਜੇ ( newVariant != ਖਾਲੀ_ਸੂਚਕ )
        {
            // remove leading _'s
            ਜਦੋਂ(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            ਜੇ ( vsize < 0 || vsize > ULOC_STRING_ਹੱਦਬੰਦੀ ) { // int32 wrap
                ਵਾਪਸ;
            }
            ਜਦੋਂ( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        ਜੇ ( newKeywords != ਖਾਲੀ_ਸੂਚਕ)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            ਜੇ ( ksize < 0 || ksize > ULOC_STRING_ਹੱਦਬੰਦੀ ) {
              ਵਾਪਸ;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        ਜੇ ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, ਸਥਿਤੀ);
        }

        ਜੇ ( csize != 0 )
        {
            togo.append(newCountry, ਸਥਿਤੀ);
        }

        ਜੇ ( vsize != 0)
        {
            togo.append(SEP_CHAR, ਸਥਿਤੀ)
                .append(newVariant, vsize, ਸਥਿਤੀ);
        }

        ਜੇ ( ksize != 0)
        {
            ਜੇ (uprv_strchr(newKeywords, '=')) {
                togo.append('@', ਸਥਿਤੀ); /* keyword parsing */
            }
            ਨਹੀਂਤਾਂ {
                togo.append('_', ਸਥਿਤੀ); /* Variant parsing with a script */
                ਜੇ ( vsize == 0) {
                    togo.append('_', ਸਥਿਤੀ); /* No country found */
                }
            }
            togo.append(newKeywords, ਸਥਿਤੀ);
        }

        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            // Something went wrong with appending, etc.
            ਵਾਪਸ;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.ਅੰਕੜੇ(), ਝੂਠ);
    }
}

Locale::Locale(ਅਚਲ Locale&) = ਮੂਲ;
Locale::Locale(Locale&&) noexcept = ਮੂਲ;

Locale& Locale::ਸੰਚਾਲਕ=(ਅਚਲ Locale&) U_LIFETIME_BOUND = ਮੂਲ;
Locale& Locale::ਸੰਚਾਲਕ=(Locale&&) noexcept U_LIFETIME_BOUND = ਮੂਲ;

Locale *
Locale::clone() ਅਚਲ {
    ਵਾਪਸ ਨਵਾਂ Locale(*ਇਹ);
}

ਤਾਰਕਿਕ
Locale::ਸੰਚਾਲਕ==( ਅਚਲ   Locale& other) ਅਚਲ
{
    ਵਾਪਸ uprv_strcmp(other.getName(), getName()) == 0;
}

ਨਾਮਾਕਾਸ਼ {

UInitOnce g_ਗਿਆਤ_Canonicalized_Init_Once {};
UHashtable *g_ਗਿਆਤ_Canonicalized = ਖਾਲੀ_ਸੂਚਕ;

constexpr ਅਚਲ ਅੱਖਰ* ਗਿਆਤ_CANONICALIZED[] = {
    "c",
    // Commonly used locales known are already canonicalized
    "af", "af_ZA", "am", "am_ET", "ar", "ar_001", "as", "as_IN", "az", "az_AZ",
    "be", "be_BY", "bg", "bg_BG", "bn", "bn_IN", "bs", "bs_BA", "ca", "ca_ES",
    "cs", "cs_CZ", "cy", "cy_GB", "da", "da_DK", "de", "de_DE", "el", "el_GR",
    "en", "en_GB", "en_US", "es", "es_419", "es_ES", "et", "et_EE", "eu",
    "eu_ES", "fa", "fa_IR", "fi", "fi_FI", "fil", "fil_PH", "fr", "fr_FR",
    "ga", "ga_IE", "gl", "gl_ES", "gu", "gu_IN", "he", "he_IL", "hi", "hi_IN",
    "hr", "hr_HR", "hu", "hu_HU", "hy", "hy_AM", "id", "id_ID", "is", "is_IS",
    "it", "it_IT", "ja", "ja_JP", "jv", "jv_ID", "ka", "ka_GE", "kk", "kk_KZ",
    "km", "km_KH", "kn", "kn_IN", "ko", "ko_KR", "ky", "ky_KG", "lo", "lo_LA",
    "lt", "lt_LT", "lv", "lv_LV", "mk", "mk_MK", "ml", "ml_IN", "mn", "mn_MN",
    "mr", "mr_IN", "ms", "ms_MY", "my", "my_MM", "nb", "nb_NO", "ne", "ne_NP",
    "nl", "nl_NL", "no", "or", "or_IN", "pa", "pa_IN", "pl", "pl_PL", "ps", "ps_AF",
    "pt", "pt_BR", "pt_PT", "ro", "ro_RO", "ru", "ru_RU", "sd", "sd_IN", "si",
    "si_LK", "sk", "sk_SK", "sl", "sl_SI", "so", "so_SO", "sq", "sq_AL", "sr",
    "sr_Cyrl_RS", "sr_Latn", "sr_RS", "sv", "sv_SE", "sw", "sw_TZ", "ta",
    "ta_IN", "te", "te_IN", "th", "th_TH", "tk", "tk_TM", "tr", "tr_TR", "uk",
    "uk_UA", "ur", "ur_PK", "uz", "uz_UZ", "vi", "vi_VN", "yue", "yue_Hant",
    "yue_Hant_HK", "yue_HK", "zh", "zh_CN", "zh_Hans", "zh_Hans_CN", "zh_Hant",
    "zh_Hant_TW", "zh_TW", "zu", "zu_ZA"
};

UBool U_CALLCONV cleanup_ਗਿਆਤ_Canonicalized() {
    g_ਗਿਆਤ_Canonicalized_Init_Once.reset();
    ਜੇ (g_ਗਿਆਤ_Canonicalized) { uhash_close(g_ਗਿਆਤ_Canonicalized); }
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ U_CALLCONV ਲੱਦੋ_ਗਿਆਤ_Canonicalized(UError_ਕੋਡ &ਸਥਿਤੀ) {
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_LOCALE_ਗਿਆਤ_CANONICALIZED,
                                cleanup_ਗਿਆਤ_Canonicalized);
    Local_UHashtable_ਸੂਚਕ new_ਗਿਆਤ_Canonicalized_Map(
        uhash_open(uhash_hashChars, uhash_compareChars, ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));
    ਲਈ (int32_t i = 0;
            U_SUCCESS(ਸਥਿਤੀ) && i < UPRV_LENGTHOF(ਗਿਆਤ_CANONICALIZED);
            i++) {
        uhash_puti(new_ਗਿਆਤ_Canonicalized_Map.getAlias(),
                   (ਖਾਲੀ*)ਗਿਆਤ_CANONICALIZED[i],
                   1, &ਸਥਿਤੀ);
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    g_ਗਿਆਤ_Canonicalized = new_ਗਿਆਤ_Canonicalized_Map.orphan();
}

ਵਰਗ Alias_ਅੰਕੜੇ;

/**
 * A Builder class to build the alias data.
 */
ਵਰਗ Alias_ਅੰਕੜੇ_Builder {
ਜਨਤਕ:
    Alias_ਅੰਕੜੇ_Builder() {
    }

    // Build the AliasData from resource.
    Alias_ਅੰਕੜੇ* build(UError_ਕੋਡ &ਸਥਿਤੀ);

ਨਿਜੀ:
    ਖਾਲੀ readAlias(UResourceBundle* alias,
                   Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
                   Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
                   Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
                   int32_t &length,
                   ਖਾਲੀ (*checkType)(ਅਚਲ ਅੱਖਰ* type),
                   ਖਾਲੀ (*checkReplacement)(ਅਚਲ UChar* replacement),
                   UError_ਕੋਡ &ਸਥਿਤੀ);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ਖਾਲੀ readLanguageAlias(UResourceBundle* alias,
                           Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
                           Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
                           Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
                           int32_t &length,
                           UError_ਕੋਡ &ਸਥਿਤੀ);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ਖਾਲੀ readScriptAlias(UResourceBundle* alias,
                         Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
                         Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
                         Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
                         int32_t &length, UError_ਕੋਡ &ਸਥਿਤੀ);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ਖਾਲੀ readTerritoryAlias(UResourceBundle* alias,
                            Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
                            Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
                            Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
                            int32_t &length, UError_ਕੋਡ &ਸਥਿਤੀ);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    ਖਾਲੀ readVariantAlias(UResourceBundle* alias,
                          Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
                          Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
                          Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
                          int32_t &length, UError_ਕੋਡ &ਸਥਿਤੀ);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    ਖਾਲੀ readSubdivisionAlias(UResourceBundle* alias,
                          Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
                          Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
                          Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
                          int32_t &length, UError_ਕੋਡ &ਸਥਿਤੀ);
};

/**
 * A class to hold the Alias Data.
 */
ਵਰਗ Alias_ਅੰਕੜੇ : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    ਸਥਿਰ ਅਚਲ Alias_ਅੰਕੜੇ* singleton(UError_ਕੋਡ& ਸਥਿਤੀ) {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            // Do not get into loadData if the status already has error.
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        umtx_initOnce(Alias_ਅੰਕੜੇ::gInitOnce, &Alias_ਅੰਕੜੇ::ਲੱਦੋ_ਅੰਕੜੇ, ਸਥਿਤੀ);
        ਵਾਪਸ gSingleton;
    }

    ਅਚਲ CharStringMap& languageMap() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ language; }
    ਅਚਲ CharStringMap& scriptMap() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ script; }
    ਅਚਲ CharStringMap& territoryMap() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ territory; }
    ਅਚਲ CharStringMap& variantMap() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ variant; }
    ਅਚਲ CharStringMap& subdivisionMap() ਅਚਲ U_LIFETIME_BOUND { ਵਾਪਸ subdivision; }

    ਸਥਿਰ ਖਾਲੀ U_CALLCONV ਲੱਦੋ_ਅੰਕੜੇ(UError_ਕੋਡ &ਸਥਿਤੀ);
    ਸਥਿਰ UBool U_CALLCONV cleanup();

    ਸਥਿਰ UInitOnce gInitOnce;

ਨਿਜੀ:
    Alias_ਅੰਕੜੇ(CharStringMap languageMap,
              CharStringMap scriptMap,
              CharStringMap territoryMap,
              CharStringMap variantMap,
              CharStringMap subdivisionMap,
              CharString* ਲੜੀਆਂ)
        : language(std::move(languageMap)),
          script(std::move(scriptMap)),
          territory(std::move(territoryMap)),
          variant(std::move(variantMap)),
          subdivision(std::move(subdivisionMap)),
          ਲੜੀਆਂ(ਲੜੀਆਂ) {
    }

    ~Alias_ਅੰਕੜੇ() {
        ਮਿਟਾਓ ਲੜੀਆਂ;
    }

    ਸਥਿਰ ਅਚਲ Alias_ਅੰਕੜੇ* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* ਲੜੀਆਂ;

    ਮਿੱਤਰ ਵਰਗ Alias_ਅੰਕੜੇ_Builder;
};


ਅਚਲ Alias_ਅੰਕੜੇ* Alias_ਅੰਕੜੇ::gSingleton = ਖਾਲੀ_ਸੂਚਕ;
UInitOnce Alias_ਅੰਕੜੇ::gInitOnce {};

UBool U_CALLCONV
Alias_ਅੰਕੜੇ::cleanup()
{
    gInitOnce.reset();
    ਮਿਟਾਓ gSingleton;
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ
Alias_ਅੰਕੜੇ_Builder::readAlias(
        UResourceBundle* alias,
        Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
        Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
        Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
        int32_t &length,
        ਖਾਲੀ (*checkType)(ਅਚਲ ਅੱਖਰ* type),
        ਖਾਲੀ (*checkReplacement)(ਅਚਲ UChar* replacement),
        UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    length = ures_get_ਮਾਪ(alias);
    ਅਚਲ ਅੱਖਰ** rawTypes = types.allocateInsteadAndCopy(length);
    ਜੇ (rawTypes == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    ਜੇ (rawIndexes == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਲਈ (ਪੂਰਨ i = 0; U_SUCCESS(ਸਥਿਤੀ) && ures_has_ਅਗਲਾ(alias); i++) {
        Local_UResource_Bundle_ਸੂਚਕ res(
            ures_get_ਅਗਲਾ_Resource(alias, ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));
        ਅਚਲ ਅੱਖਰ* aliasFrom = ures_get_ਕੁੰਜੀ(res.getAlias());
        ਅਚਲ UChar* alias_ਮੰਜ਼ਿਲ =
            ures_get_String_By_ਕੁੰਜੀ(res.getAlias(), "replacement", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;

        checkType(aliasFrom);
        checkReplacement(alias_ਮੰਜ਼ਿਲ);

        rawTypes[i] = aliasFrom;
        rawIndexes[i] = ਲੜੀਆਂ->ਜੋੜੋ(alias_ਮੰਜ਼ਿਲ, ਸਥਿਤੀ);
    }
}

/**
 * Read the languageAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement language.
 */
ਖਾਲੀ
Alias_ਅੰਕੜੇ_Builder::readLanguageAlias(
        UResourceBundle* alias,
        Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
        Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
        Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ਕੋਡ &ਸਥਿਤੀ)
{
    ਵਾਪਸ readAlias(
        alias, ਲੜੀਆਂ, types, replacementIndexes, length,
#ਜੇ U_DEBUG
        [](ਅਚਲ ਅੱਖਰ* type) {
            // Assert the aliasFrom only contains the following possibilities
            // language_REGION_variant
            // language_REGION
            // language_variant
            // language
            // und_variant
            Locale test(type);
            // Assert no script in aliasFrom
            U_ASSERT(test.getScript()[0] == '\0');
            // Assert when language is und, no REGION in aliasFrom.
            U_ASSERT(test.getLanguage()[0] != '\0' || test.getCountry()[0] == '\0');
        },
#ਨਹੀਂਤਾਂ
        [](ਅਚਲ ਅੱਖਰ*) {},
#ਜੇ_ਅੰਤ
        [](ਅਚਲ UChar*) {}, ਸਥਿਤੀ);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
ਖਾਲੀ
Alias_ਅੰਕੜੇ_Builder::readScriptAlias(
        UResourceBundle* alias,
        Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
        Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
        Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ਕੋਡ &ਸਥਿਤੀ)
{
    ਵਾਪਸ readAlias(
        alias, ਲੜੀਆਂ, types, replacementIndexes, length,
#ਜੇ U_DEBUG
        [](ਅਚਲ ਅੱਖਰ* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ਅਚਲ UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#ਨਹੀਂਤਾਂ
        [](ਅਚਲ ਅੱਖਰ*) {},
        [](ਅਚਲ UChar*) { },
#ਜੇ_ਅੰਤ
        ਸਥਿਤੀ);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
ਖਾਲੀ
Alias_ਅੰਕੜੇ_Builder::readTerritoryAlias(
        UResourceBundle* alias,
        Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
        Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
        Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ਕੋਡ &ਸਥਿਤੀ)
{
    ਵਾਪਸ readAlias(
        alias, ਲੜੀਆਂ, types, replacementIndexes, length,
#ਜੇ U_DEBUG
        [](ਅਚਲ ਅੱਖਰ* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#ਨਹੀਂਤਾਂ
        [](ਅਚਲ ਅੱਖਰ*) {},
#ਜੇ_ਅੰਤ
        [](ਅਚਲ UChar*) { },
        ਸਥਿਤੀ);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
ਖਾਲੀ
Alias_ਅੰਕੜੇ_Builder::readVariantAlias(
        UResourceBundle* alias,
        Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
        Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
        Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ਕੋਡ &ਸਥਿਤੀ)
{
    ਵਾਪਸ readAlias(
        alias, ਲੜੀਆਂ, types, replacementIndexes, length,
#ਜੇ U_DEBUG
        [](ਅਚਲ ਅੱਖਰ* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](ਅਚਲ UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#ਨਹੀਂਤਾਂ
        [](ਅਚਲ ਅੱਖਰ*) {},
        [](ਅਚਲ UChar*) { },
#ਜੇ_ਅੰਤ
        ਸਥਿਤੀ);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
ਖਾਲੀ
Alias_ਅੰਕੜੇ_Builder::readSubdivisionAlias(
        UResourceBundle* alias,
        Unique_Char_ਲੜੀਆਂ* ਲੜੀਆਂ,
        Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*>& types,
        Local_ਸਮ੍ਰਿਤੀ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ਕੋਡ &ਸਥਿਤੀ)
{
    ਵਾਪਸ readAlias(
        alias, ਲੜੀਆਂ, types, replacementIndexes, length,
#ਜੇ U_DEBUG
        [](ਅਚਲ ਅੱਖਰ* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#ਨਹੀਂਤਾਂ
        [](ਅਚਲ ਅੱਖਰ*) {},
#ਜੇ_ਅੰਤ
        [](ਅਚਲ UChar*) { },
        ਸਥਿਤੀ);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
ਖਾਲੀ U_CALLCONV
Alias_ਅੰਕੜੇ::ਲੱਦੋ_ਅੰਕੜੇ(UError_ਕੋਡ &ਸਥਿਤੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOCALE_CANONICALIZATION_DEBUG
    UDate ਸ਼ੁਰੂ = uprv_getRawUTCtime();
#ਜੇ_ਅੰਤ  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    Alias_ਅੰਕੜੇ_Builder builder;
    gSingleton = builder.build(ਸਥਿਤੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOCALE_CANONICALIZATION_DEBUG
    UDate ਸਿਰਾ = uprv_getRawUTCtime();
    ਲਿਖੋ("AliasData::loadData took total %f ms\n", ਸਿਰਾ - ਸ਼ੁਰੂ);
#ਜੇ_ਅੰਤ  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
Alias_ਅੰਕੜੇ*
Alias_ਅੰਕੜੇ_Builder::build(UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }

    Local_UResource_Bundle_ਸੂਚਕ metadata(
        ures_open_ਸਿੱਧਾ(ਖਾਲੀ_ਸੂਚਕ, "metadata", &ਸਥਿਤੀ));
    Local_UResource_Bundle_ਸੂਚਕ metadataAlias(
        ures_get_By_ਕੁੰਜੀ(metadata.getAlias(), "alias", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));
    Local_UResource_Bundle_ਸੂਚਕ languageAlias(
        ures_get_By_ਕੁੰਜੀ(metadataAlias.getAlias(), "language", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));
    Local_UResource_Bundle_ਸੂਚਕ scriptAlias(
        ures_get_By_ਕੁੰਜੀ(metadataAlias.getAlias(), "script", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));
    Local_UResource_Bundle_ਸੂਚਕ territoryAlias(
        ures_get_By_ਕੁੰਜੀ(metadataAlias.getAlias(), "territory", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));
    Local_UResource_Bundle_ਸੂਚਕ variantAlias(
        ures_get_By_ਕੁੰਜੀ(metadataAlias.getAlias(), "variant", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));
    Local_UResource_Bundle_ਸੂਚਕ subdivisionAlias(
        ures_get_By_ਕੁੰਜੀ(metadataAlias.getAlias(), "subdivision", ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    int32_t languages_ਲੰਮਾਈ = 0, script_ਲੰਮਾਈ = 0, territory_ਲੰਮਾਈ = 0,
            variant_ਲੰਮਾਈ = 0, subdivision_ਲੰਮਾਈ = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    Unique_Char_ਲੜੀਆਂ ਲੜੀਆਂ(ਸਥਿਤੀ);
    Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*> languageTypes;
    Local_ਸਮ੍ਰਿਤੀ<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &ਲੜੀਆਂ,
                      languageTypes,
                      languageReplacementIndexes,
                      languages_ਲੰਮਾਈ,
                      ਸਥਿਤੀ);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*> scriptTypes;
    Local_ਸਮ੍ਰਿਤੀ<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &ਲੜੀਆਂ,
                    scriptTypes,
                    scriptReplacementIndexes,
                    script_ਲੰਮਾਈ,
                    ਸਥਿਤੀ);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*> territoryTypes;
    Local_ਸਮ੍ਰਿਤੀ<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &ਲੜੀਆਂ,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territory_ਲੰਮਾਈ, ਸਥਿਤੀ);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*> variantTypes;
    Local_ਸਮ੍ਰਿਤੀ<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &ਲੜੀਆਂ,
                     variantTypes,
                     variantReplacementIndexes,
                     variant_ਲੰਮਾਈ, ਸਥਿਤੀ);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    Local_ਸਮ੍ਰਿਤੀ<ਅਚਲ ਅੱਖਰ*> subdivisionTypes;
    Local_ਸਮ੍ਰਿਤੀ<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &ਲੜੀਆਂ,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivision_ਲੰਮਾਈ, ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // We can only use strings after freeze it.
    ਲੜੀਆਂ.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, ਸਥਿਤੀ);
    ਲਈ (int32_t i = 0; U_SUCCESS(ਸਥਿਤੀ) && i < languages_ਲੰਮਾਈ; i++) {
        languageMap.put(languageTypes[i],
                        ਲੜੀਆਂ.get(languageReplacementIndexes[i]),
                        ਸਥਿਤੀ);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, ਸਥਿਤੀ);
    ਲਈ (int32_t i = 0; U_SUCCESS(ਸਥਿਤੀ) && i < script_ਲੰਮਾਈ; i++) {
        scriptMap.put(scriptTypes[i],
                      ਲੜੀਆਂ.get(scriptReplacementIndexes[i]),
                      ਸਥਿਤੀ);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, ਸਥਿਤੀ);
    ਲਈ (int32_t i = 0; U_SUCCESS(ਸਥਿਤੀ) && i < territory_ਲੰਮਾਈ; i++) {
        territoryMap.put(territoryTypes[i],
                         ਲੜੀਆਂ.get(territoryReplacementIndexes[i]),
                         ਸਥਿਤੀ);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, ਸਥਿਤੀ);
    ਲਈ (int32_t i = 0; U_SUCCESS(ਸਥਿਤੀ) && i < variant_ਲੰਮਾਈ; i++) {
        variantMap.put(variantTypes[i],
                       ਲੜੀਆਂ.get(variantReplacementIndexes[i]),
                       ਸਥਿਤੀ);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, ਸਥਿਤੀ);
    ਲਈ (int32_t i = 0; U_SUCCESS(ਸਥਿਤੀ) && i < subdivision_ਲੰਮਾਈ; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       ਲੜੀਆਂ.get(subdivisionReplacementIndexes[i]),
                       ਸਥਿਤੀ);
    }

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // copy hashtables
    ਸਵੈਚਾਲਿਤ *ਅੰਕੜੇ = ਨਵਾਂ Alias_ਅੰਕੜੇ(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        ਲੜੀਆਂ.orphan_Char_ਲੜੀਆਂ());

    ਜੇ (ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਵਾਪਸ ਅੰਕੜੇ;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
ਵਰਗ AliasReplacer {
ਜਨਤਕ:
    AliasReplacer(UError_ਕੋਡ& ਸਥਿਤੀ) :
            language(ਖਾਲੀ_ਸੂਚਕ), script(ਖਾਲੀ_ਸੂਚਕ), region(ਖਾਲੀ_ਸੂਚਕ),
            extensions(ਖਾਲੀ_ਸੂਚਕ),
            // store value in variants only once
            variants(ਖਾਲੀ_ਸੂਚਕ,
                     ([](UElement e1, UElement e2) -> UBool {
                       ਵਾਪਸ 0==uprv_strcmp((ਅਚਲ ਅੱਖਰ*)e1.ਸੂਚਕ,
                                             (ਅਚਲ ਅੱਖਰ*)e2.ਸੂਚਕ);}),
                     ਸਥਿਤੀ),
            ਅੰਕੜੇ(ਖਾਲੀ_ਸੂਚਕ) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    ਤਾਰਕਿਕ replace(
        ਅਚਲ Locale& locale, CharString& out, UError_ਕੋਡ& ਸਥਿਤੀ);

ਨਿਜੀ:
    ਅਚਲ ਅੱਖਰ* language;
    ਅਚਲ ਅੱਖਰ* script;
    ਅਚਲ ਅੱਖਰ* region;
    ਅਚਲ ਅੱਖਰ* extensions;
    UVector variants;

    ਅਚਲ Alias_ਅੰਕੜੇ* ਅੰਕੜੇ;

    ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ not_ਖਾਲੀ_ਥਾਵਾਂ(ਅਚਲ ਅੱਖਰ* str) {
        ਵਾਪਸ str && str[0] != NULL_CHAR;
    }

    /**
     * If replacement is neither null nor empty and input is either null or empty,
     * return replacement.
     * If replacement is neither null nor empty but input is not empty, return input.
     * If replacement is either null or empty and type is either null or empty,
     * return input.
     * Otherwise return null.
     *   replacement     input      type        return
     *    AAA             nullptr    *           AAA
     *    AAA             BBB        *           BBB
     *    nullptr || ""   CCC        nullptr     CCC
     *    nullptr || ""   *          DDD         nullptr
     */
    ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* deleteOrReplace(ਅਚਲ ਅੱਖਰ* input U_LIFETIME_BOUND,
                                       ਅਚਲ ਅੱਖਰ* type,
                                       ਅਚਲ ਅੱਖਰ* replacement U_LIFETIME_BOUND) {
        ਵਾਪਸ not_ਖਾਲੀ_ਥਾਵਾਂ(replacement) ?
            ((input == ਖਾਲੀ_ਸੂਚਕ) ?  replacement : input) :
            ((type == ਖਾਲੀ_ਸੂਚਕ) ? input  : ਖਾਲੀ_ਸੂਚਕ);
    }

    ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ same(ਅਚਲ ਅੱਖਰ* a, ਅਚਲ ਅੱਖਰ* b) {
        ਜੇ (a == ਖਾਲੀ_ਸੂਚਕ && b == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ ਸੱਚ;
        }
        ਜੇ ((a == ਖਾਲੀ_ਸੂਚਕ && b != ਖਾਲੀ_ਸੂਚਕ) ||
            (a != ਖਾਲੀ_ਸੂਚਕ && b == ਖਾਲੀ_ਸੂਚਕ)) {
          ਵਾਪਸ ਝੂਠ;
        }
        ਵਾਪਸ uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& output_ਮੰਜ਼ਿਲ_String(CharString& out U_LIFETIME_BOUND, UError_ਕੋਡ& ਸਥਿਤੀ);

    // Generate the lookup key.
    CharString& generate_ਕੁੰਜੀ(ਅਚਲ ਅੱਖਰ* language, ਅਚਲ ਅੱਖਰ* region,
                            ਅਚਲ ਅੱਖਰ* variant, CharString& out U_LIFETIME_BOUND,
                            UError_ਕੋਡ& ਸਥਿਤੀ);

    ਖਾਲੀ parseLanguageReplacement(ਅਚਲ ਅੱਖਰ* replacement,
                                  ਅਚਲ ਅੱਖਰ*& replaceLanguage,
                                  ਅਚਲ ਅੱਖਰ*& replaceScript,
                                  ਅਚਲ ਅੱਖਰ*& replaceRegion,
                                  ਅਚਲ ਅੱਖਰ*& replaceVariant,
                                  ਅਚਲ ਅੱਖਰ*& replaceExtensions,
                                  UVector& ਮੰਜ਼ਿਲ_Be_Freed,
                                  UError_ਕੋਡ& ਸਥਿਤੀ);

    // Replace by using languageAlias.
    ਤਾਰਕਿਕ replaceLanguage(ਤਾਰਕਿਕ checkLanguage, ਤਾਰਕਿਕ checkRegion,
                         ਤਾਰਕਿਕ checkVariants, UVector& ਮੰਜ਼ਿਲ_Be_Freed,
                         UError_ਕੋਡ& ਸਥਿਤੀ);

    // Replace by using territoryAlias.
    ਤਾਰਕਿਕ replaceTerritory(UVector& ਮੰਜ਼ਿਲ_Be_Freed, UError_ਕੋਡ& ਸਥਿਤੀ);

    // Replace by using scriptAlias.
    ਤਾਰਕਿਕ replaceScript(UError_ਕੋਡ& ਸਥਿਤੀ);

    // Replace by using variantAlias.
    ਤਾਰਕਿਕ replaceVariant(UError_ਕੋਡ& ਸਥਿਤੀ);

    // Replace by using subdivisionAlias.
    ਤਾਰਕਿਕ replaceSubdivision(StringPiece subdivision,
                            CharString& output, UError_ਕੋਡ& ਸਥਿਤੀ);

    // Replace transformed extensions.
    ਤਾਰਕਿਕ replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UError_ਕੋਡ& ਸਥਿਤੀ);
};

CharString&
AliasReplacer::generate_ਕੁੰਜੀ(
        ਅਚਲ ਅੱਖਰ* language, ਅਚਲ ਅੱਖਰ* region, ਅਚਲ ਅੱਖਰ* variant,
        CharString& out U_LIFETIME_BOUND, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ out; }
    out.append(language, ਸਥਿਤੀ);
    ਜੇ (not_ਖਾਲੀ_ਥਾਵਾਂ(region)) {
        out.append(SEP_CHAR, ਸਥਿਤੀ)
            .append(region, ਸਥਿਤੀ);
    }
    ਜੇ (not_ਖਾਲੀ_ਥਾਵਾਂ(variant)) {
       out.append(SEP_CHAR, ਸਥਿਤੀ)
           .append(variant, ਸਥਿਤੀ);
    }
    ਵਾਪਸ out;
}

ਖਾਲੀ
AliasReplacer::parseLanguageReplacement(
    ਅਚਲ ਅੱਖਰ* replacement,
    ਅਚਲ ਅੱਖਰ*& replacedLanguage,
    ਅਚਲ ਅੱਖਰ*& replacedScript,
    ਅਚਲ ਅੱਖਰ*& replacedRegion,
    ਅਚਲ ਅੱਖਰ*& replacedVariant,
    ਅਚਲ ਅੱਖਰ*& replacedExtensions,
    UVector& ਮੰਜ਼ਿਲ_Be_Freed,
    UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (uprv_strchr(replacement, '_') == ਖਾਲੀ_ਸੂਚਕ) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        ਵਾਪਸ;
    }
    // We have multiple field so we have to allocate and parse
    ਸਵੈਚਾਲਿਤ str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), ਸਥਿਤੀ, ਸਥਿਤੀ);
    ਅੱਖਰ* ਅੰਕੜੇ = str->ਅੰਕੜੇ();
    ਮੰਜ਼ਿਲ_Be_Freed.adoptElement(str.release(), ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    replacedLanguage = (ਅਚਲ ਅੱਖਰ*) ਅੰਕੜੇ;
    ਅੱਖਰ* ਸਿਰਾ_ਦਾ_ਖੇਤਰ = uprv_strchr(ਅੰਕੜੇ, '_');
    *ਸਿਰਾ_ਦਾ_ਖੇਤਰ = '\0'; // null terminiate it.
    ਸਿਰਾ_ਦਾ_ਖੇਤਰ++;
    ਅਚਲ ਅੱਖਰ* ਸ਼ੁਰੂ = ਸਿਰਾ_ਦਾ_ਖੇਤਰ;
    ਸਿਰਾ_ਦਾ_ਖੇਤਰ = const_cast<ਅੱਖਰ*>(uprv_strchr(ਸ਼ੁਰੂ, '_'));
    size_t len = 0;
    ਜੇ (ਸਿਰਾ_ਦਾ_ਖੇਤਰ == ਖਾਲੀ_ਸੂਚਕ) {
        len = uprv_strlen(ਸ਼ੁਰੂ);
    } ਨਹੀਂਤਾਂ {
        len = ਸਿਰਾ_ਦਾ_ਖੇਤਰ - ਸ਼ੁਰੂ;
        *ਸਿਰਾ_ਦਾ_ਖੇਤਰ = '\0'; // null terminiate it.
    }
    ਜੇ (len == 4 && uprv_isASCIILetter(*ਸ਼ੁਰੂ)) {
        // Got a script
        replacedScript = ਸ਼ੁਰੂ;
        ਜੇ (ਸਿਰਾ_ਦਾ_ਖੇਤਰ == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ;
        }
        ਸ਼ੁਰੂ = ਸਿਰਾ_ਦਾ_ਖੇਤਰ++;
        ਸਿਰਾ_ਦਾ_ਖੇਤਰ = const_cast<ਅੱਖਰ*>(uprv_strchr(ਸ਼ੁਰੂ, '_'));
        ਜੇ (ਸਿਰਾ_ਦਾ_ਖੇਤਰ == ਖਾਲੀ_ਸੂਚਕ) {
            len = uprv_strlen(ਸ਼ੁਰੂ);
        } ਨਹੀਂਤਾਂ {
            len = ਸਿਰਾ_ਦਾ_ਖੇਤਰ - ਸ਼ੁਰੂ;
            *ਸਿਰਾ_ਦਾ_ਖੇਤਰ = '\0'; // null terminiate it.
        }
    }
    ਜੇ (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = ਸ਼ੁਰੂ;
        ਜੇ (ਸਿਰਾ_ਦਾ_ਖੇਤਰ == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ;
        }
        ਸ਼ੁਰੂ = ਸਿਰਾ_ਦਾ_ਖੇਤਰ++;
        ਸਿਰਾ_ਦਾ_ਖੇਤਰ = const_cast<ਅੱਖਰ*>(uprv_strchr(ਸ਼ੁਰੂ, '_'));
        ਜੇ (ਸਿਰਾ_ਦਾ_ਖੇਤਰ == ਖਾਲੀ_ਸੂਚਕ) {
            len = uprv_strlen(ਸ਼ੁਰੂ);
        } ਨਹੀਂਤਾਂ {
            len = ਸਿਰਾ_ਦਾ_ਖੇਤਰ - ਸ਼ੁਰੂ;
            *ਸਿਰਾ_ਦਾ_ਖੇਤਰ = '\0'; // null terminiate it.
        }
    }
    ਜੇ (len >= 4) {
        // Got a variant
        replacedVariant = ਸ਼ੁਰੂ;
        ਜੇ (ਸਿਰਾ_ਦਾ_ਖੇਤਰ == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ;
        }
        ਸ਼ੁਰੂ = ਸਿਰਾ_ਦਾ_ਖੇਤਰ++;
    }
    replacedExtensions = ਸ਼ੁਰੂ;
}

ਤਾਰਕਿਕ
AliasReplacer::replaceLanguage(
        ਤਾਰਕਿਕ checkLanguage, ਤਾਰਕਿਕ checkRegion,
        ਤਾਰਕਿਕ checkVariants, UVector& ਮੰਜ਼ਿਲ_Be_Freed, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (    (checkRegion && region == ਖਾਲੀ_ਸੂਚਕ) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        ਵਾਪਸ ਝੂਠ;
    }
    int32_t variant_ਮਾਪ = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ਅਚਲ ਅੱਖਰ* ਖੋਜ_Language = checkLanguage ? language : "und";
    ਅਚਲ ਅੱਖਰ* ਖੋਜ_Region = checkRegion ? region : ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ ਅੱਖਰ* ਖੋਜ_Variant = ਖਾਲੀ_ਸੂਚਕ;
    ਲਈ (int32_t variant_ਸੂਚਕਾਂਕ = 0;
            variant_ਸੂਚਕਾਂਕ < variant_ਮਾਪ;
            variant_ਸੂਚਕਾਂਕ++) {
        ਜੇ (checkVariants) {
            U_ASSERT(variant_ਸੂਚਕਾਂਕ < variant_ਮਾਪ);
            ਖੋਜ_Variant = static_cast<ਅਚਲ ਅੱਖਰ*>(variants.elementAt(variant_ਸੂਚਕਾਂਕ));
        }

        ਜੇ (ਖੋਜ_Variant != ਖਾਲੀ_ਸੂਚਕ && uprv_strlen(ਖੋਜ_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            ਖੋਜ_Variant = ਖਾਲੀ_ਸੂਚਕ;
        }
        CharString type_ਕੁੰਜੀ;
        generate_ਕੁੰਜੀ(ਖੋਜ_Language, ਖੋਜ_Region, ਖੋਜ_Variant, type_ਕੁੰਜੀ,
                    ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ ਝੂਠ;
        }
        ਅਚਲ ਅੱਖਰ *replacement = ਅੰਕੜੇ->languageMap().get(type_ਕੁੰਜੀ.ਅੰਕੜੇ());
        ਜੇ (replacement == ਖਾਲੀ_ਸੂਚਕ) {
            // Found no replacement data.
            ਜਾਰੀ;
        }

        ਅਚਲ ਅੱਖਰ* replacedLanguage = ਖਾਲੀ_ਸੂਚਕ;
        ਅਚਲ ਅੱਖਰ* replacedScript = ਖਾਲੀ_ਸੂਚਕ;
        ਅਚਲ ਅੱਖਰ* replacedRegion = ਖਾਲੀ_ਸੂਚਕ;
        ਅਚਲ ਅੱਖਰ* replacedVariant = ਖਾਲੀ_ਸੂਚਕ;
        ਅਚਲ ਅੱਖਰ* replacedExtensions = ਖਾਲੀ_ਸੂਚਕ;
        parseLanguageReplacement(replacement,
                                 replacedLanguage,
                                 replacedScript,
                                 replacedRegion,
                                 replacedVariant,
                                 replacedExtensions,
                                 ਮੰਜ਼ਿਲ_Be_Freed,
                                 ਸਥਿਤੀ);
        replacedLanguage =
            (replacedLanguage != ਖਾਲੀ_ਸੂਚਕ && uprv_strcmp(replacedLanguage, "und") == 0) ?
            language : replacedLanguage;
        replacedScript = deleteOrReplace(script, ਖਾਲੀ_ਸੂਚਕ, replacedScript);
        replacedRegion = deleteOrReplace(region, ਖੋਜ_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            ਖੋਜ_Variant, ਖੋਜ_Variant, replacedVariant);

        ਜੇ (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(ਖੋਜ_Variant, replacedVariant) &&
                replacedExtensions == ਖਾਲੀ_ਸੂਚਕ) {
            // Replacement produce no changes.
            ਜਾਰੀ;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        ਜੇ (ਖੋਜ_Variant != ਖਾਲੀ_ਸੂਚਕ) {
            ਜੇ (not_ਖਾਲੀ_ਥਾਵਾਂ(replacedVariant)) {
                variants.ਸੈੱਟ_Element_At((ਖਾਲੀ*)replacedVariant, variant_ਸੂਚਕਾਂਕ);
            } ਨਹੀਂਤਾਂ {
                variants.removeElementAt(variant_ਸੂਚਕਾਂਕ);
            }
        }
        ਜੇ (replacedExtensions != ਖਾਲੀ_ਸੂਚਕ) {
            // DO NOTHING
            // UTS35 does not specify what should we do if we have extensions in the
            // replacement. Currently we know only the following 4 "BCP47 LegacyRules" have
            // extensions in them languageAlias:
            //  i_default => en_x_i_default
            //  i_enochian => und_x_i_enochian
            //  i_mingo => see_x_i_mingo
            //  zh_min => nan_x_zh_min
            // But all of them are already changed by code inside ultag_parse() before
            // hitting this code.
        }

        // Something changed by language alias data.
        ਵਾਪਸ ਸੱਚ;
    }
    // Nothing changed by language alias data.
    ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ
AliasReplacer::replaceTerritory(UVector& ਮੰਜ਼ਿਲ_Be_Freed, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (region == ਖਾਲੀ_ਸੂਚਕ) {
        // No region to search.
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ ਅੱਖਰ *replacement = ਅੰਕੜੇ->territoryMap().get(region);
    ਜੇ (replacement == ਖਾਲੀ_ਸੂਚਕ) {
        // Found no replacement data for this region.
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ ਅੱਖਰ* replacedRegion = replacement;
    ਅਚਲ ਅੱਖਰ* ਪਹਿਲਾ_Space = uprv_strchr(replacement, ' ');
    ਜੇ (ਪਹਿਲਾ_Space != ਖਾਲੀ_ਸੂਚਕ) {
        // If there are are more than one region in the replacement.
        // We need to check which one match based on the language.
        // Cannot use nullptr for language because that will construct
        // the default locale, in that case, use "und" to get the correct
        // locale.
        Locale l = LocaleBuilder()
            .ਸੈੱਟ_Language(language == ਖਾਲੀ_ਸੂਚਕ ? "und" : language)
            .ਸੈੱਟ_Script(script)
            .build(ਸਥਿਤੀ);
        l.ਜੋੜੋ_Likely_Subtags(ਸਥਿਤੀ);
        ਅਚਲ ਅੱਖਰ* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> ਵਸਤੂ;
        ਜੇ (likelyRegion != ਖਾਲੀ_ਸੂਚਕ && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ਅਚਲ ਅੱਖਰ* ਮਿਲਿਆ_ਵਿੱਚ_Replacement = uprv_strstr(replacement,
                                                         likelyRegion);
            ਜੇ (ਮਿਲਿਆ_ਵਿੱਚ_Replacement != ਖਾਲੀ_ਸੂਚਕ) {
                // Assuming the case there are no three letter region code in
                // the replacement of territoryAlias
                U_ASSERT(ਮਿਲਿਆ_ਵਿੱਚ_Replacement == replacement ||
                         *(ਮਿਲਿਆ_ਵਿੱਚ_Replacement-1) == ' ');
                U_ASSERT(ਮਿਲਿਆ_ਵਿੱਚ_Replacement[len] == ' ' ||
                         ਮਿਲਿਆ_ਵਿੱਚ_Replacement[len] == '\0');
                ਵਸਤੂ = prv::make_unique<CharString>(
                    ਮਿਲਿਆ_ਵਿੱਚ_Replacement, static_cast<int32_t>(len), ਸਥਿਤੀ, ਸਥਿਤੀ);
            }
        }
        ਜੇ (!ਵਸਤੂ && U_SUCCESS(ਸਥਿਤੀ)) {
            ਵਸਤੂ = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(ਪਹਿਲਾ_Space - replacement), ਸਥਿਤੀ, ਸਥਿਤੀ);
        }
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਝੂਠ; }
        replacedRegion = ਵਸਤੂ->ਅੰਕੜੇ();
        ਮੰਜ਼ਿਲ_Be_Freed.adoptElement(ਵਸਤੂ.release(), ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਝੂਠ; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ
AliasReplacer::replaceScript(UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (script == ਖਾਲੀ_ਸੂਚਕ) {
        // No script to search.
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ ਅੱਖਰ *replacement = ਅੰਕੜੇ->scriptMap().get(script);
    ਜੇ (replacement == ਖਾਲੀ_ਸੂਚਕ) {
        // Found no replacement data for this script.
        ਵਾਪਸ ਝੂਠ;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ
AliasReplacer::replaceVariant(UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    // Since we may have more than one variant, we need to loop through them.
    ਲਈ (int32_t i = 0; i < variants.size(); i++) {
        ਅਚਲ ਅੱਖਰ* variant = static_cast<ਅਚਲ ਅੱਖਰ*>(variants.elementAt(i));
        ਅਚਲ ਅੱਖਰ *replacement = ਅੰਕੜੇ->variantMap().get(variant);
        ਜੇ (replacement == ਖਾਲੀ_ਸੂਚਕ) {
            // Found no replacement data for this variant.
            ਜਾਰੀ;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        ਜੇ (!same(variant, replacement)) {
            variants.ਸੈੱਟ_Element_At((ਖਾਲੀ*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            ਜੇ (uprv_strcmp(variant, "heploc") == 0) {
                ਲਈ (int32_t j = 0; j < variants.size(); j++) {
                     ਜੇ (uprv_strcmp((ਅਚਲ ਅੱਖਰ*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            ਵਾਪਸ ਸੱਚ;
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ ਅੱਖਰ *replacement = ਅੰਕੜੇ->subdivisionMap().get(subdivision.ਅੰਕੜੇ());
    ਜੇ (replacement != ਖਾਲੀ_ਸੂਚਕ) {
        ਅਚਲ ਅੱਖਰ* ਪਹਿਲਾ_Space = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (ਪਹਿਲਾ_Space != ਖਾਲੀ_ਸੂਚਕ) ?
            (ਪਹਿਲਾ_Space - replacement) : uprv_strlen(replacement);
        ਜੇ (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), ਸਥਿਤੀ);
            ਜੇ (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, ਸਥਿਤੀ);
            }
        }
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    int32_t len = transformedExtensions.length();
    ਅਚਲ ਅੱਖਰ* str = transformedExtensions.ਅੰਕੜੇ();
    ਅਚਲ ਅੱਖਰ* tkey = ultag_get_TKey_ਸ਼ੁਰੂ(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == ਖਾਲੀ_ਸੂਚਕ) ? len : static_cast<int32_t>((tkey - str - 1)));
    ਜੇ (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .ਸੈੱਟ_Language_ਟੈਗ(StringPiece(str, tlangLen))
            .build(ਸਥਿਤੀ);
        tlang.canonicalize(ਸਥਿਤੀ);
        output = tlang.ਮੰਜ਼ਿਲ_Language_ਟੈਗ<CharString>(ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ ਝੂਠ;
        }
        T_CString_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ_Case(output.ਅੰਕੜੇ());
    }
    ਜੇ (tkey != ਖਾਲੀ_ਸੂਚਕ) {
        // We need to sort the tfields by tkey
        UVector tfields(ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ ਝੂਠ;
        }
        ਕਰੋ {
            ਅਚਲ ਅੱਖਰ* tvalue = uprv_strchr(tkey, '-');
            ਜੇ (tvalue == ਖਾਲੀ_ਸੂਚਕ) {
                ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                ਵਾਪਸ ਝੂਠ;
            }
            ਅਚਲ ਅੱਖਰ* ਅਗਲਾ_TKey = ultag_get_TKey_ਸ਼ੁਰੂ(tvalue);
            ਜੇ (ਅਗਲਾ_TKey != ਖਾਲੀ_ਸੂਚਕ) {
                *const_cast<ਅੱਖਰ*>(ਅਗਲਾ_TKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.ਪਾਓ_Element_At((ਖਾਲੀ*)tkey, tfields.size(), ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਵਾਪਸ ਝੂਠ;
            }
            tkey = ਅਗਲਾ_TKey;
        } ਜਦੋਂ (tkey != ਖਾਲੀ_ਸੂਚਕ);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            ਵਾਪਸ uprv_strcmp((ਅਚਲ ਅੱਖਰ*)e1.ਸੂਚਕ, (ਅਚਲ ਅੱਖਰ*)e2.ਸੂਚਕ);
        }, ਸਥਿਤੀ);
        ਲਈ (int32_t i = 0; i < tfields.size(); i++) {
             ਜੇ (output.length() > 0) {
                 output.append('-', ਸਥਿਤੀ);
             }
             ਅਚਲ ਅੱਖਰ* tfield = static_cast<ਅਚਲ ਅੱਖਰ*>(tfields.elementAt(i));
             ਅਚਲ ਅੱਖਰ* tvalue = uprv_strchr(tfield, '-');
             ਜੇ (tvalue == ਖਾਲੀ_ਸੂਚਕ) {
                 ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                 ਵਾਪਸ ਝੂਠ;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<ਅੱਖਰ*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, ਸਥਿਤੀ).append('-', ਸਥਿਤੀ);
             std::optional<std::string_view> bcpTValue = ulocimp_ਮੰਜ਼ਿਲ_Bcp_Type(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, ਸਥਿਤੀ);
        }
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ ਸੱਚ;
}

CharString&
AliasReplacer::output_ਮੰਜ਼ਿਲ_String(
    CharString& out U_LIFETIME_BOUND, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ out; }
    out.append(language, ਸਥਿਤੀ);
    ਜੇ (not_ਖਾਲੀ_ਥਾਵਾਂ(script)) {
        out.append(SEP_CHAR, ਸਥਿਤੀ)
            .append(script, ਸਥਿਤੀ);
    }
    ਜੇ (not_ਖਾਲੀ_ਥਾਵਾਂ(region)) {
        out.append(SEP_CHAR, ਸਥਿਤੀ)
            .append(region, ਸਥਿਤੀ);
    }
    ਜੇ (variants.size() > 0) {
        ਜੇ (!not_ਖਾਲੀ_ਥਾਵਾਂ(script) && !not_ਖਾਲੀ_ਥਾਵਾਂ(region)) {
          out.append(SEP_CHAR, ਸਥਿਤੀ);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            ਵਾਪਸ uprv_strcmp((ਅਚਲ ਅੱਖਰ*)e1.ਸੂਚਕ, (ਅਚਲ ਅੱਖਰ*)e2.ਸੂਚਕ);
        }, ਸਥਿਤੀ);
        int32_t variants_ਸ਼ੁਰੂ = out.length();
        ਲਈ (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, ਸਥਿਤੀ)
                 .append(static_cast<ਅਚਲ ਅੱਖਰ*>(variants.elementAt(i)),
                         ਸਥਿਤੀ);
        }
        T_CString_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_Case(out.ਅੰਕੜੇ() + variants_ਸ਼ੁਰੂ);
    }
    ਜੇ (not_ਖਾਲੀ_ਥਾਵਾਂ(extensions)) {
        CharString tmp("und_", ਸਥਿਤੀ);
        tmp.append(extensions, ਸਥਿਤੀ);
        Locale tmpLocale(tmp.ਅੰਕੜੇ());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, ਸਥਿਤੀ);
    }
    ਵਾਪਸ out;
}

ਤਾਰਕਿਕ
AliasReplacer::replace(ਅਚਲ Locale& locale, CharString& out, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਅੰਕੜੇ = Alias_ਅੰਕੜੇ::singleton(ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    U_ASSERT(ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ);
    out.clear();
    language = locale.getLanguage();
    ਜੇ (!not_ਖਾਲੀ_ਥਾਵਾਂ(language)) {
        language = ਖਾਲੀ_ਸੂਚਕ;
    }
    script = locale.getScript();
    ਜੇ (!not_ਖਾਲੀ_ਥਾਵਾਂ(script)) {
        script = ਖਾਲੀ_ਸੂਚਕ;
    }
    region = locale.getCountry();
    ਜੇ (!not_ਖਾਲੀ_ਥਾਵਾਂ(region)) {
        region = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ ਅੱਖਰ* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, ਸਥਿਤੀ);
    ਜੇ (!variantsBuff.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਝੂਠ; }
        ਅੱਖਰ* ਸ਼ੁਰੂ = variantsBuff.ਅੰਕੜੇ();
        T_CString_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ_Case(ਸ਼ੁਰੂ);
        ਅੱਖਰ* ਸਿਰਾ;
        ਜਦੋਂ ((ਸਿਰਾ = uprv_strchr(ਸ਼ੁਰੂ, SEP_CHAR)) != ਖਾਲੀ_ਸੂਚਕ &&
               U_SUCCESS(ਸਥਿਤੀ)) {
            *ਸਿਰਾ = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            ਜੇ (*ਸ਼ੁਰੂ && !variants.ਸ਼ਾਮਲ_ਹੈ(ਸ਼ੁਰੂ)) {
                variants.ਜੋੜੋ_Element(ਸ਼ੁਰੂ, ਸਥਿਤੀ);
            }
            ਸ਼ੁਰੂ = ਸਿਰਾ + 1;
        }
        // do not add "" or duplicate data to variants
        ਜੇ (*ਸ਼ੁਰੂ && !variants.ਸ਼ਾਮਲ_ਹੈ(ਸ਼ੁਰੂ)) {
            variants.ਜੋੜੋ_Element(ਸ਼ੁਰੂ, ਸਥਿਤੀ);
        }
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਝੂਠ; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        ਵਾਪਸ uprv_strcmp((ਅਚਲ ਅੱਖਰ*)e1.ਸੂਚਕ, (ਅਚਲ ਅੱਖਰ*)e2.ਸੂਚਕ);
    }, ਸਥਿਤੀ);

    // A changed count to assert when loop too many times.
    int32_t ਬਦਲਿਆ = 0;
    ਅਚਲ int32_t max_ਬਦਲਿਆ = 4 + variants.size();
    (ਖਾਲੀ)max_ਬਦਲਿਆ; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector ਲੜੀਆਂ_ਮੰਜ਼ਿਲ_Be_Freed([](ਖਾਲੀ *obj) { ਮਿਟਾਓ static_cast<CharString*>(obj); },
                             ਖਾਲੀ_ਸੂਚਕ, 10, ਸਥਿਤੀ);
    ਜਦੋਂ (U_SUCCESS(ਸਥਿਤੀ)) {
        // Something wrong with the data cause looping here more than maxChanged
        // times already.
        U_ASSERT(ਬਦਲਿਆ <= max_ਬਦਲਿਆ);
        // From observation of key in data/misc/metadata.txt
        // we know currently we only need to search in the following combination
        // of fields for type in languageAlias:
        // * lang_region_variant
        // * lang_region
        // * lang_variant
        // * lang
        // * und_variant
        // This assumption is ensured by the U_ASSERT in readLanguageAlias
        //
        //                      lang  REGION variant
        ਜੇ (    replaceLanguage(ਸੱਚ, ਸੱਚ,  ਸੱਚ,  ਲੜੀਆਂ_ਮੰਜ਼ਿਲ_Be_Freed, ਸਥਿਤੀ) ||
                replaceLanguage(ਸੱਚ, ਸੱਚ,  ਝੂਠ, ਲੜੀਆਂ_ਮੰਜ਼ਿਲ_Be_Freed, ਸਥਿਤੀ) ||
                replaceLanguage(ਸੱਚ, ਝੂਠ, ਸੱਚ,  ਲੜੀਆਂ_ਮੰਜ਼ਿਲ_Be_Freed, ਸਥਿਤੀ) ||
                replaceLanguage(ਸੱਚ, ਝੂਠ, ਝੂਠ, ਲੜੀਆਂ_ਮੰਜ਼ਿਲ_Be_Freed, ਸਥਿਤੀ) ||
                replaceLanguage(ਝੂਠ,ਝੂਠ, ਸੱਚ,  ਲੜੀਆਂ_ਮੰਜ਼ਿਲ_Be_Freed, ਸਥਿਤੀ) ||
                replaceTerritory(ਲੜੀਆਂ_ਮੰਜ਼ਿਲ_Be_Freed, ਸਥਿਤੀ) ||
                replaceScript(ਸਥਿਤੀ) ||
                replaceVariant(ਸਥਿਤੀ)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            ਬਦਲਿਆ++;
            ਜਾਰੀ;
        }
        // Nothing changed. Break out.
        ਰੋਕੋ;
    }  // while(1)

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਝੂਠ; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ਅਚਲ ਅੱਖਰ* extensionsStr = locale_get_Keywords_ਸ਼ੁਰੂ(locale.getName());
    ਜੇ (ਬਦਲਿਆ == 0 && variants.size() <= 1 && extensionsStr == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;
    }
    output_ਮੰਜ਼ਿਲ_String(out, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (extensionsStr != ਖਾਲੀ_ਸੂਚਕ) {
        ਬਦਲਿਆ = 0;
        Locale ਆਰਜ਼ੀ(locale);
        Local_ਸੂਚਕ<icu::StringEnumeration> iter(locale.createKeywords(ਸਥਿਤੀ));
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && !iter.isNull()) {
            ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ;
            ਜਦੋਂ ((ਕੁੰਜੀ = iter->next(ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ)) != ਖਾਲੀ_ਸੂਚਕ) {
                ਜੇ (uprv_strcmp("sd", ਕੁੰਜੀ) == 0 || uprv_strcmp("rg", ਕੁੰਜੀ) == 0 ||
                        uprv_strcmp("t", ਕੁੰਜੀ) == 0) {
                    ਸਵੈਚਾਲਿਤ value = locale.get_Keyword_ਮੁੱਲ<CharString>(ਕੁੰਜੀ, ਸਥਿਤੀ);
                    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                        ਸਥਿਤੀ = U_ZERO_ERROR;
                        ਜਾਰੀ;
                    }
                    CharString replacement;
                    ਜੇ (uprv_strlen(ਕੁੰਜੀ) == 2) {
                        ਜੇ (replaceSubdivision(value.ਮੰਜ਼ਿਲ_String_Piece(), replacement, ਸਥਿਤੀ)) {
                            ਬਦਲਿਆ++;
                            ਆਰਜ਼ੀ.ਸੈੱਟ_Keyword_ਮੁੱਲ(ਕੁੰਜੀ, replacement.ਅੰਕੜੇ(), ਸਥਿਤੀ);
                        }
                    } ਨਹੀਂਤਾਂ {
                        U_ASSERT(uprv_strcmp(ਕੁੰਜੀ, "t") == 0);
                        ਜੇ (replaceTransformedExtensions(value, replacement, ਸਥਿਤੀ)) {
                            ਬਦਲਿਆ++;
                            ਆਰਜ਼ੀ.ਸੈੱਟ_Keyword_ਮੁੱਲ(ਕੁੰਜੀ, replacement.ਅੰਕੜੇ(), ਸਥਿਤੀ);
                        }
                    }
                    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                        ਵਾਪਸ ਝੂਠ;
                    }
                }
            }
        }
        ਜੇ (ਬਦਲਿਆ != 0) {
            extensionsStr = locale_get_Keywords_ਸ਼ੁਰੂ(ਆਰਜ਼ੀ.getName());
        }
        out.append(extensionsStr, ਸਥਿਤੀ);
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    // If the tag is not changed, return.
    ਜੇ (uprv_strcmp(out.ਅੰਕੜੇ(), locale.getName()) == 0) {
        out.clear();
        ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ ਸੱਚ;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
ਤਾਰਕਿਕ
canonicalizeLocale(ਅਚਲ Locale& locale, CharString& out, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਝੂਠ; }
    AliasReplacer replacer(ਸਥਿਤੀ);
    ਵਾਪਸ replacer.replace(locale, out, ਸਥਿਤੀ);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
ਤਾਰਕਿਕ
is_ਗਿਆਤ_Canonicalized_Locale(ਅਚਲ ਅੱਖਰ* locale, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਝੂਠ; }

    ਜੇ (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        ਵਾਪਸ ਸੱਚ;
    }

    // common well-known Canonicalized.
    umtx_initOnce(g_ਗਿਆਤ_Canonicalized_Init_Once,
                  &ਲੱਦੋ_ਗਿਆਤ_Canonicalized, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    U_ASSERT(g_ਗਿਆਤ_Canonicalized != ਖਾਲੀ_ਸੂਚਕ);
    ਵਾਪਸ uhash_geti(g_ਗਿਆਤ_Canonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_ਸਿਰਾ

// Function for testing.
U_EXPORT ਅਚਲ ਅੱਖਰ* ਅਚਲ*
ulocimp_get_ਗਿਆਤ_Canonicalized_Locale_For_Test(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(ਗਿਆਤ_CANONICALIZED);
    ਵਾਪਸ ਗਿਆਤ_CANONICALIZED;
}

// Function for testing.
U_EXPORT ਤਾਰਕਿਕ
ulocimp_isCanonicalizedLocaleForTest(ਅਚਲ ਅੱਖਰ* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    CharString ਆਰਜ਼ੀ;
    ਵਾਪਸ !canonicalizeLocale(l, ਆਰਜ਼ੀ, ਸਥਿਤੀ) && U_SUCCESS(ਸਥਿਤੀ);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ਅਚਲ ਅੱਖਰ* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    ਵਾਪਸ localeID == ਖਾਲੀ_ਸੂਚਕ ? *ਇਹ = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    ਕਰੋ {
        ਅੱਖਰ *separator;
        ਅੱਖਰ *ਖੇਤਰ_2[5] = {ਖਾਲੀ_ਸੂਚਕ};
        int32_t ਖੇਤਰ_Len[5] = {0};
        int32_t ਖੇਤਰ_Idx;
        int32_t variant_ਖੇਤਰ;
        int32_t length;
        UError_ਕੋਡ err;

        ਅਚਲ ਸਵੈਚਾਲਿਤ parse = [canonicalize](std::string_view localeID,
                                          ਅੱਖਰ* name,
                                          int32_t name_ਸਮਰੱਥਾ,
                                          UError_ਕੋਡ& ਸਥਿਤੀ) {
            ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
                name, name_ਸਮਰੱਥਾ,
                [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
                    ਜੇ (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, ਸਥਿਤੀ);
                    } ਨਹੀਂਤਾਂ {
                        ulocimp_getName(localeID, sink, ਸਥਿਤੀ);
                    }
                },
                ਸਥਿਤੀ);
        };

        // "canonicalize" the locale ID to ICU/Java format
        ਅੱਖਰ* ਭਰਿਆ_Name = nest.ਅਧਾਰ_Name;
        err = U_ZERO_ERROR;
        length = parse(localeID, ਭਰਿਆ_Name, ਆਕਾਰ Nest::ਅਧਾਰ_Name, err);

        FixedString ਭਰਿਆ_Name_ਬਫਰ;
        ਜੇ (err == U_ਬਫਰ_ਉਛਾਲ_ERROR || length >= static_cast<int32_t>(ਆਕਾਰ Nest::ਅਧਾਰ_Name)) {
            /*Go to heap for the fullName if necessary*/
            ਜੇ (!ਭਰਿਆ_Name_ਬਫਰ.reserve(length + 1)) {
                ਰੋਕੋ; // error: out of memory
            }
            ਭਰਿਆ_Name = ਭਰਿਆ_Name_ਬਫਰ.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, ਭਰਿਆ_Name, length + 1, err);
        }
        ਜੇ(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            ਰੋਕੋ;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = ਖੇਤਰ_2[0] = ਭਰਿਆ_Name;
        ਖੇਤਰ_Idx = 1;
        ਅੱਖਰ* at = uprv_strchr(ਭਰਿਆ_Name, '@');
        ਜਦੋਂ ((separator = uprv_strchr(ਖੇਤਰ_2[ਖੇਤਰ_Idx-1], SEP_CHAR)) != ਖਾਲੀ_ਸੂਚਕ &&
               ਖੇਤਰ_Idx < UPRV_LENGTHOF(ਖੇਤਰ_2)-1 &&
               (at == ਖਾਲੀ_ਸੂਚਕ || separator < at)) {
            ਖੇਤਰ_2[ਖੇਤਰ_Idx] = separator + 1;
            ਖੇਤਰ_Len[ਖੇਤਰ_Idx - 1] = static_cast<int32_t>(separator - ਖੇਤਰ_2[ਖੇਤਰ_Idx - 1]);
            ਖੇਤਰ_Idx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(ਖੇਤਰ_2[ਖੇਤਰ_Idx-1], '@');
        ਅੱਖਰ* sep2 = uprv_strchr(ਖੇਤਰ_2[ਖੇਤਰ_Idx-1], '.');
        ਜੇ (separator!=ਖਾਲੀ_ਸੂਚਕ || sep2!=ਖਾਲੀ_ਸੂਚਕ) {
            ਜੇ (separator==ਖਾਲੀ_ਸੂਚਕ || (sep2!=ਖਾਲੀ_ਸੂਚਕ && separator > sep2)) {
                separator = sep2;
            }
            ਖੇਤਰ_Len[ਖੇਤਰ_Idx - 1] = static_cast<int32_t>(separator - ਖੇਤਰ_2[ਖੇਤਰ_Idx - 1]);
        } ਨਹੀਂਤਾਂ {
            ਖੇਤਰ_Len[ਖੇਤਰ_Idx - 1] = length - static_cast<int32_t>(ਖੇਤਰ_2[ਖੇਤਰ_Idx - 1] - ਭਰਿਆ_Name);
        }
        ਤਾਰਕਿਕ hasKeywords = at != ਖਾਲੀ_ਸੂਚਕ && uprv_strchr(at + 1, '=') != ਖਾਲੀ_ਸੂਚਕ;

        ਜੇ (ਖੇਤਰ_Len[0] >= ULOC_LANG_ਸਮਰੱਥਾ)
        {
            ਰੋਕੋ; // error: the language field is too long
        }

        variant_ਖੇਤਰ = 1; /* Usually the 2nd one, except when a script or country is also used. */
        ਜੇ (ਖੇਤਰ_Len[0] > 0) {
            /* We have a language */
            language = {ਭਰਿਆ_Name, static_cast<std::string_view::size_type>(ਖੇਤਰ_Len[0])};
        }
        ਜੇ (ਖੇਤਰ_Len[1] == 4 && uprv_isASCIILetter(ਖੇਤਰ_2[1][0]) &&
                uprv_isASCIILetter(ਖੇਤਰ_2[1][1]) && uprv_isASCIILetter(ਖੇਤਰ_2[1][2]) &&
                uprv_isASCIILetter(ਖੇਤਰ_2[1][3])) {
            /* We have at least a script */
            script = {ਖੇਤਰ_2[1], static_cast<std::string_view::size_type>(ਖੇਤਰ_Len[1])};
            variant_ਖੇਤਰ++;
        }

        ਜੇ (ਖੇਤਰ_Len[variant_ਖੇਤਰ] == 2 || ਖੇਤਰ_Len[variant_ਖੇਤਰ] == 3) {
            /* We have a country */
            region = {ਖੇਤਰ_2[variant_ਖੇਤਰ], static_cast<std::string_view::size_type>(ਖੇਤਰ_Len[variant_ਖੇਤਰ])};
            variant_ਖੇਤਰ++;
        } ਨਹੀਂਤਾਂ ਜੇ (ਖੇਤਰ_Len[variant_ਖੇਤਰ] == 0) {
            variant_ਖੇਤਰ++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        ਜੇ (ਖੇਤਰ_Len[variant_ਖੇਤਰ] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(ਖੇਤਰ_2[variant_ਖੇਤਰ] - ਭਰਿਆ_Name);
        } ਨਹੀਂਤਾਂ ਜੇ (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - ਭਰਿਆ_Name);
        }

        ਜੇ (!hasKeywords && Nest::ਸਮਾਉਂਦਾ(length, language, script, region)) {
            U_ASSERT(ਭਰਿਆ_Name == nest.ਅਧਾਰ_Name);
            U_ASSERT(ਭਰਿਆ_Name_ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ());
            nest.init(language, script, region, variantBegin);
        } ਨਹੀਂਤਾਂ {
            ਜੇ (ਭਰਿਆ_Name == nest.ਅਧਾਰ_Name) {
                U_ASSERT(ਭਰਿਆ_Name_ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ());
                ਭਰਿਆ_Name_ਬਫਰ = {ਭਰਿਆ_Name, static_cast<std::string_view::size_type>(length)};
                ਜੇ (ਭਰਿਆ_Name_ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                    ਰੋਕੋ; // error: out of memory
                }
                ਜੇ (!language.empty()) {
                    language = {ਭਰਿਆ_Name_ਬਫਰ.ਅੰਕੜੇ(), language.size()};
                }
                ਜੇ (!script.empty()) {
                    script = {ਭਰਿਆ_Name_ਬਫਰ.ਅੰਕੜੇ() + (script.ਅੰਕੜੇ() - ਭਰਿਆ_Name), script.size()};
                }
                ਜੇ (!region.empty()) {
                    region = {ਭਰਿਆ_Name_ਬਫਰ.ਅੰਕੜੇ() + (region.ਅੰਕੜੇ() - ਭਰਿਆ_Name), region.size()};
                }
            }
            ਢੇਰ_2& ਢੇਰ_3 = payload.emplace<ਢੇਰ_2>(language, script, region, variantBegin);
            ਜੇ (isBogus()) {
                ਰੋਕੋ; // error: out of memory
            }
            U_ASSERT(!ਭਰਿਆ_Name_ਬਫਰ.is_ਖਾਲੀ_ਥਾਵਾਂ());
            ਢੇਰ_3.ptr->ਭਰਿਆ_Name = std::move(ਭਰਿਆ_Name_ਬਫਰ);
            ਜੇ (hasKeywords) {
                ਜੇ (std::string_view::size_type ਅਧਾਰ_Name_ਲੰਮਾਈ = at - ਭਰਿਆ_Name; ਅਧਾਰ_Name_ਲੰਮਾਈ > 0) {
                    ਢੇਰ_3.ptr->ਅਧਾਰ_Name = {ਢੇਰ_3.ptr->ਭਰਿਆ_Name.ਅੰਕੜੇ(), ਅਧਾਰ_Name_ਲੰਮਾਈ};
                    ਜੇ (ਢੇਰ_3.ptr->ਅਧਾਰ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                        ਰੋਕੋ; // error: out of memory
                    }
                }
            }
        }

        ਜੇ (canonicalize) {
            ਜੇ (!is_ਗਿਆਤ_Canonicalized_Locale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                ਜੇ (canonicalizeLocale(*ਇਹ, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.ਅੰਕੜੇ(), ਝੂਠ);
                }
                ਜੇ (U_FAILURE(err)) {
                    ਰੋਕੋ;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        ਵਾਪਸ *ਇਹ;
    } ਜਦੋਂ(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();

    ਵਾਪਸ *ਇਹ;
}

int32_t
Locale::hash_ਕੋਡ() ਅਚਲ
{
    ਵਾਪਸ ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

ਖਾਲੀ
Locale::ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus() {
    /* Free our current storage */
    payload.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
}

ਅਚਲ Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex ਤਾਲਾ(&gDefaultLocaleMutex);
        ਜੇ (gDefaultLocale != ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ *gDefaultLocale;
        }
    }
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    ਵਾਪਸ *locale_ਸੈੱਟ_default_ਅੰਦਰੂਨੀ(ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
}



ਖਾਲੀ U_EXPORT2
Locale::ਸੈੱਟ_Default( ਅਚਲ   Locale&     newLocale,
                            UError_ਕੋਡ&  ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ਅਚਲ ਅੱਖਰ *localeID = newLocale.getName();
    locale_ਸੈੱਟ_default_ਅੰਦਰੂਨੀ(localeID, ਸਥਿਤੀ);
}

ਖਾਲੀ
Locale::ਜੋੜੋ_Likely_Subtags(UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    CharString maximizedLocaleID = ulocimp_ਜੋੜੋ_Likely_Subtags(getName(), ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਜੇ (ਸਥਿਤੀ == U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR) {
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        }
        ਵਾਪਸ;
    }

    init(maximizedLocaleID.ਅੰਕੜੇ(), /*canonicalize=*/ਝੂਠ);
    ਜੇ (isBogus()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
}

ਖਾਲੀ
Locale::minimizeSubtags(UError_ਕੋਡ& ਸਥਿਤੀ) {
    Locale::minimizeSubtags(ਝੂਠ, ਸਥਿਤੀ);
}
ਖਾਲੀ
Locale::minimizeSubtags(ਤਾਰਕਿਕ favorScript, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਜੇ (ਸਥਿਤੀ == U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR) {
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        }
        ਵਾਪਸ;
    }

    init(minimizedLocaleID.ਅੰਕੜੇ(), /*canonicalize=*/ਝੂਠ);
    ਜੇ (isBogus()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
}

ਖਾਲੀ
Locale::canonicalize(UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (isBogus()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    CharString uncanonicalized(getName(), ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਜੇ (ਸਥਿਤੀ == U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR) {
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        }
        ਵਾਪਸ;
    }
    init(uncanonicalized.ਅੰਕੜੇ(), /*canonicalize=*/ਸੱਚ);
    ਜੇ (isBogus()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
}

Locale U_EXPORT2
Locale::for_Language_ਟੈਗ(StringPiece ਟੈਗ, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    Locale ਨਤੀਜਾ(Locale::eBOGUS);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    // If a BCP 47 language tag is passed as the language parameter to the
    // normal Locale constructor, it will actually fall back to invoking
    // uloc_forLanguageTag() to parse it if it somehow is able to detect that
    // the string actually is BCP 47. This works well for things like strings
    // using BCP 47 extensions, but it does not at all work for things like
    // legacy language tags (marked as “Type: grandfathered” in BCP 47,
    // e.g., "en-GB-oed") which are possible to also
    // interpret as ICU locale IDs and because of that won't trigger the BCP 47
    // parsing. Therefore the code here explicitly calls uloc_forLanguageTag()
    // and then Locale::init(), instead of just calling the normal constructor.

    int32_t parsed_ਲੰਮਾਈ;
    CharString localeID = ulocimp_for_Language_ਟੈਗ(
            ਟੈਗ.ਅੰਕੜੇ(),
            ਟੈਗ.length(),
            &parsed_ਲੰਮਾਈ,
            ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਜੇ (parsed_ਲੰਮਾਈ != ਟੈਗ.size()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਨਤੀਜਾ.init(localeID.ਅੰਕੜੇ(), /*canonicalize=*/ਝੂਠ);
    ਜੇ (ਨਤੀਜਾ.isBogus()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ
Locale::ਮੰਜ਼ਿਲ_Language_ਟੈਗ(ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    ਜੇ (isBogus()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ulocimp_ਮੰਜ਼ਿਲ_Language_ਟੈਗ(getName(), sink, /*strict=*/ਝੂਠ, ਸਥਿਤੀ);
}

Locale U_EXPORT2
Locale::createFromName (ਅਚਲ ਅੱਖਰ *name)
{
    ਜੇ (name) {
        Locale l("");
        l.init(name, ਝੂਠ);
        ਵਾਪਸ l;
    }
    ਨਹੀਂਤਾਂ {
        ਵਾਪਸ getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, ਝੂਠ);
    ਵਾਪਸ loc;
}

Locale U_EXPORT2
Locale::createCanonical(ਅਚਲ ਅੱਖਰ* name) {
    Locale loc("");
    loc.init(name, ਸੱਚ);
    ਵਾਪਸ loc;
}

ਅਚਲ ਅੱਖਰ *
Locale::getISO3Language() ਅਚਲ
{
    ਵਾਪਸ uloc_getISO3Language(getName());
}


ਅਚਲ ਅੱਖਰ *
Locale::getISO3Country() ਅਚਲ
{
    ਵਾਪਸ uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() ਅਚਲ
{
    ਵਾਪਸ uloc_getLCID(getName());
}

ਅਚਲ ਅੱਖਰ* ਅਚਲ* U_EXPORT2 Locale::getISOCountries()
{
    ਵਾਪਸ uloc_getISOCountries();
}

ਅਚਲ ਅੱਖਰ* ਅਚਲ* U_EXPORT2 Locale::getISOLanguages()
{
    ਵਾਪਸ uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
ਖਾਲੀ Locale::ਸੈੱਟ_From_POSIXID(ਅਚਲ ਅੱਖਰ *posixID)
{
    init(posixID, ਸੱਚ);
}

ਅਚਲ Locale & U_EXPORT2
Locale::get_ਜੜ੍ਹ()
{
    ਵਾਪਸ getLocale(e_ਜੜ੍ਹ);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getEnglish()
{
    ਵਾਪਸ getLocale(eENGLISH);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getFrench()
{
    ਵਾਪਸ getLocale(eFRENCH);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getGerman()
{
    ਵਾਪਸ getLocale(eGERMAN);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getItalian()
{
    ਵਾਪਸ getLocale(eITALIAN);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getJapanese()
{
    ਵਾਪਸ getLocale(eJAPANESE);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getKorean()
{
    ਵਾਪਸ getLocale(eKOREAN);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getChinese()
{
    ਵਾਪਸ getLocale(eCHINESE);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    ਵਾਪਸ getLocale(eCHINA);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    ਵਾਪਸ getLocale(eTAIWAN);
}


ਅਚਲ Locale & U_EXPORT2
Locale::getFrance()
{
    ਵਾਪਸ getLocale(eFRANCE);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getGermany()
{
    ਵਾਪਸ getLocale(eGERMANY);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getItaly()
{
    ਵਾਪਸ getLocale(eITALY);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getJapan()
{
    ਵਾਪਸ getLocale(eJAPAN);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getKorea()
{
    ਵਾਪਸ getLocale(eKOREA);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getChina()
{
    ਵਾਪਸ getLocale(eCHINA);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getPRC()
{
    ਵਾਪਸ getLocale(eCHINA);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getTaiwan()
{
    ਵਾਪਸ getLocale(eTAIWAN);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getUK()
{
    ਵਾਪਸ getLocale(eUK);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getUS()
{
    ਵਾਪਸ getLocale(eUS);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getCanada()
{
    ਵਾਪਸ getLocale(eCANADA);
}

ਅਚਲ Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    ਵਾਪਸ getLocale(eCANADA_FRENCH);
}

ਅਚਲ Locale &
Locale::getLocale(ਪੂਰਨ locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    ਜੇ (localeCache == ਖਾਲੀ_ਸੂਚਕ) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    ਵਾਪਸ localeCache[locid]; /*operating on nullptr*/
}

/*
This function is defined this way in order to get around static
initialization and static destruction.
 */
Locale *
Locale::getLocaleCache()
{
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    umtx_initOnce(gLocaleCacheInitOnce, locale_init, ਸਥਿਤੀ);
    ਵਾਪਸ gLocaleCache;
}

ਵਰਗ KeywordEnumeration : ਜਨਤਕ StringEnumeration {
ਸੁਰੱਖਿਅਤ:
    FixedString keywords;
ਨਿਜੀ:
    int32_t length;
    ਅਚਲ ਅੱਖਰ *current;
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ fgClassID;

ਜਨਤਕ:
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID() { ਵਾਪਸ (UClassID)&fgClassID; }
    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override { ਵਾਪਸ getStaticClassID(); }
ਜਨਤਕ:
    KeywordEnumeration(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀਆਂ, int32_t keywordLen, int32_t current_ਸੂਚਕਾਂਕ, UError_ਕੋਡ &ਸਥਿਤੀ)
        : keywords(), length(keywordLen), current(ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(U_SUCCESS(ਸਥਿਤੀ) && keywordLen != 0) {
            ਜੇ(ਕੁੰਜੀਆਂ == ਖਾਲੀ_ਸੂਚਕ || keywordLen < 0) {
                ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            } ਨਹੀਂਤਾਂ {
                keywords = {ਕੁੰਜੀਆਂ, static_cast<std::string_view::size_type>(length)};
                ਜੇ (keywords.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                    ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                } ਨਹੀਂਤਾਂ {
                    current = keywords.ਅੰਕੜੇ() + current_ਸੂਚਕਾਂਕ;
                }
            }
        }
    }

    ਆਭਾਸੀ ~KeywordEnumeration();

    ਆਭਾਸੀ StringEnumeration * clone() ਅਚਲ override
    {
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        ਵਾਪਸ ਨਵਾਂ KeywordEnumeration(
                keywords.ਅੰਕੜੇ(), length,
                static_cast<int32_t>(current - keywords.ਅੰਕੜੇ()), ਸਥਿਤੀ);
    }

    ਆਭਾਸੀ int32_t count(UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ 0; }
        ਅਚਲ ਅੱਖਰ *kw = keywords.ਅੰਕੜੇ();
        int32_t ਨਤੀਜਾ = 0;
        ਜਦੋਂ(*kw) {
            ਨਤੀਜਾ++;
            kw += uprv_strlen(kw)+1;
        }
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਆਭਾਸੀ ਅਚਲ ਅੱਖਰ* next(int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ, UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND override {
        ਅਚਲ ਅੱਖਰ* ਨਤੀਜਾ;
        int32_t len;
        ਜੇ(U_SUCCESS(ਸਥਿਤੀ) && *current != 0) {
            ਨਤੀਜਾ = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            ਜੇ(ਨਤੀਜਾ_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) {
                *ਨਤੀਜਾ_ਲੰਮਾਈ = len;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ(ਨਤੀਜਾ_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) {
                *ਨਤੀਜਾ_ਲੰਮਾਈ = 0;
            }
            ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
        }
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਆਭਾਸੀ ਅਚਲ UnicodeString* snext(UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND override {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
        int32_t ਨਤੀਜਾ_ਲੰਮਾਈ = 0;
        ਅਚਲ ਅੱਖਰ *s = next(&ਨਤੀਜਾ_ਲੰਮਾਈ, ਸਥਿਤੀ);
        ਵਾਪਸ ਸੈੱਟ_Chars(s, ਨਤੀਜਾ_ਲੰਮਾਈ, ਸਥਿਤੀ);
    }

    ਆਭਾਸੀ ਖਾਲੀ reset(UError_ਕੋਡ& ਸਥਿਤੀ) override {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
        current = keywords.ਅੰਕੜੇ();
    }
};

ਅਚਲ ਅੱਖਰ KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = ਮੂਲ;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
ਵਰਗ UnicodeKeywordEnumeration : ਜਨਤਕ KeywordEnumeration {
ਜਨਤਕ:
    ਵਰਤੋਂ KeywordEnumeration::KeywordEnumeration;
    ਆਭਾਸੀ ~UnicodeKeywordEnumeration();

    ਆਭਾਸੀ ਅਚਲ ਅੱਖਰ* next(int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ, UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND override {
        ਅਚਲ ਅੱਖਰ* legacy_ਕੁੰਜੀ = KeywordEnumeration::next(ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
        ਜਦੋਂ (U_SUCCESS(ਸਥਿਤੀ) && legacy_ਕੁੰਜੀ != ਖਾਲੀ_ਸੂਚਕ) {
            ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ = uloc_ਮੰਜ਼ਿਲ_Unicode_Locale_ਕੁੰਜੀ(legacy_ਕੁੰਜੀ);
            ਜੇ (ਕੁੰਜੀ != ਖਾਲੀ_ਸੂਚਕ) {
                ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) {
                    *ਨਤੀਜਾ_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(ਕੁੰਜੀ));
                }
                ਵਾਪਸ ਕੁੰਜੀ;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_ਕੁੰਜੀ = KeywordEnumeration::next(ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
        }
        ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) *ਨਤੀਜਾ_ਲੰਮਾਈ = 0;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਆਭਾਸੀ int32_t count(UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override {
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ 0; }
        ਅਚਲ ਅੱਖਰ *kw = keywords.ਅੰਕੜੇ();
        int32_t ਨਤੀਜਾ = 0;
        ਜਦੋਂ(*kw) {
            ਜੇ (uloc_ਮੰਜ਼ਿਲ_Unicode_Locale_ਕੁੰਜੀ(kw) != ਖਾਲੀ_ਸੂਚਕ) {
                ਨਤੀਜਾ++;
            }
            kw += uprv_strlen(kw)+1;
        }
        ਵਾਪਸ ਨਤੀਜਾ;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = ਮੂਲ;

StringEnumeration *
Locale::createKeywords(UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ
{
    StringEnumeration *ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਅਚਲ ਅੱਖਰ* variant_ਸ਼ੁਰੂ = uprv_strchr(getName(), '@');
    ਅਚਲ ਅੱਖਰ* assignment = uprv_strchr(getName(), '=');
    ਜੇ(variant_ਸ਼ੁਰੂ) {
        ਜੇ(assignment > variant_ਸ਼ੁਰੂ) {
            CharString keywords = ulocimp_getKeywords(variant_ਸ਼ੁਰੂ + 1, '@', ਝੂਠ, ਸਥਿਤੀ);
            ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && !keywords.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                ਨਤੀਜਾ = ਨਵਾਂ KeywordEnumeration(keywords.ਅੰਕੜੇ(), keywords.length(), 0, ਸਥਿਤੀ);
                ਜੇ (!ਨਤੀਜਾ) {
                    ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                }
            }
        } ਨਹੀਂਤਾਂ {
            ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

StringEnumeration *
Locale::createUnicodeKeywords(UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ
{
    StringEnumeration *ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਅਚਲ ਅੱਖਰ* variant_ਸ਼ੁਰੂ = uprv_strchr(getName(), '@');
    ਅਚਲ ਅੱਖਰ* assignment = uprv_strchr(getName(), '=');
    ਜੇ(variant_ਸ਼ੁਰੂ) {
        ਜੇ(assignment > variant_ਸ਼ੁਰੂ) {
            CharString keywords = ulocimp_getKeywords(variant_ਸ਼ੁਰੂ + 1, '@', ਝੂਠ, ਸਥਿਤੀ);
            ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && !keywords.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                ਨਤੀਜਾ = ਨਵਾਂ UnicodeKeywordEnumeration(keywords.ਅੰਕੜੇ(), keywords.length(), 0, ਸਥਿਤੀ);
                ਜੇ (!ਨਤੀਜਾ) {
                    ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                }
            }
        } ਨਹੀਂਤਾਂ {
            ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

int32_t
Locale::get_Keyword_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ* keywordName, ਅੱਖਰ *ਬਫਰ, int32_t bufLen, UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ
{
    ਵਾਪਸ uloc_get_Keyword_ਮੁੱਲ(getName(), keywordName, ਬਫਰ, bufLen, &ਸਥਿਤੀ);
}

ਖਾਲੀ
Locale::get_Keyword_ਮੁੱਲ(StringPiece keywordName, ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    ਜੇ (isBogus()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ulocimp_get_Keyword_ਮੁੱਲ(getName(), keywordName, sink, ਸਥਿਤੀ);
}

ਖਾਲੀ
Locale::get_Unicode_Keyword_ਮੁੱਲ(StringPiece keywordName,
                               ByteSink& sink,
                               UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    std::optional<std::string_view> legacy_ਕੁੰਜੀ = ulocimp_ਮੰਜ਼ਿਲ_Legacy_ਕੁੰਜੀ_With_Fallback(keywordName);
    ਜੇ (!legacy_ਕੁੰਜੀ.has_value()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ਸਵੈਚਾਲਿਤ legacy_ਮੁੱਲ = get_Keyword_ਮੁੱਲ<CharString>(*legacy_ਕੁੰਜੀ, ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    std::optional<std::string_view> unicode_ਮੁੱਲ =
        ulocimp_ਮੰਜ਼ਿਲ_Bcp_Type_With_Fallback(keywordName, legacy_ਮੁੱਲ.ਮੰਜ਼ਿਲ_String_Piece());
    ਜੇ (!unicode_ਮੁੱਲ.has_value()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    sink.Append(unicode_ਮੁੱਲ->ਅੰਕੜੇ(), static_cast<int32_t>(unicode_ਮੁੱਲ->size()));
}

ਖਾਲੀ
Locale::ਸੈੱਟ_Keyword_ਮੁੱਲ(StringPiece keywordName,
                        StringPiece keyword_ਮੁੱਲ,
                        UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
    ਜੇ (keywordName.empty()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ (ਸਥਿਤੀ == U_STRING_NOT_TERMINATED_WARNING) {
        ਸਥਿਤੀ = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, ਸਥਿਤੀ);
    ulocimp_ਸੈੱਟ_Keyword_ਮੁੱਲ(keywordName, keyword_ਮੁੱਲ, localeID, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਜੇ (ਸਥਿਤੀ == U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR) {
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        }
        ਵਾਪਸ;
    }

    ਅਚਲ ਅੱਖਰ* at = locale_get_Keywords_ਸ਼ੁਰੂ(localeID.ਮੰਜ਼ਿਲ_String_Piece());
    ਤਾਰਕਿਕ hasKeywords = at != ਖਾਲੀ_ਸੂਚਕ && uprv_strchr(at + 1, '=') != ਖਾਲੀ_ਸੂਚਕ;

    Nest* nest = payload.get<Nest>();
    ਜੇ (!hasKeywords) {
        ਜੇ (nest == ਖਾਲੀ_ਸੂਚਕ) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            ਢੇਰ_2* ਢੇਰ_3 = payload.get<ਢੇਰ_2>();
            U_ASSERT(ਢੇਰ_3 != ਖਾਲੀ_ਸੂਚਕ);
            ਜੇ (Nest::ਸਮਾਉਂਦਾ(localeID.length(), ਢੇਰ_3->language, ਢੇਰ_3->script, ਢੇਰ_3->region)) {
                int32_t variantBegin = ਢੇਰ_3->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < ਆਕਾਰ Nest::ਅਧਾਰ_Name);
                nest = &payload.emplace<Nest>(std::move(*ਢੇਰ_3), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->ਅਧਾਰ_Name, ਆਕਾਰ Nest::ਅਧਾਰ_Name, ਸਥਿਤੀ);
            } ਨਹੀਂਤਾਂ {
                ਢੇਰ_3->ptr->ਅਧਾਰ_Name.clear();
                ਢੇਰ_3->ptr->ਭਰਿਆ_Name = localeID.ਮੰਜ਼ਿਲ_String_Piece();
                ਜੇ (ਢੇਰ_3->ptr->ਭਰਿਆ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                    ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
                    ਵਾਪਸ;
                }
            }
        }
    } ਨਹੀਂਤਾਂ {
        ਢੇਰ_2* ਢੇਰ_3 = ਖਾਲੀ_ਸੂਚਕ;
        ਜੇ (nest != ਖਾਲੀ_ਸੂਚਕ) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            ਢੇਰ_3 = &payload.emplace<ਢੇਰ_2>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            ਜੇ (isBogus()) {
                ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
        } ਨਹੀਂਤਾਂ {
            ਢੇਰ_3 = payload.get<ਢੇਰ_2>();
        }
        U_ASSERT(ਢੇਰ_3 != ਖਾਲੀ_ਸੂਚਕ);
        ਢੇਰ_3->ptr->ਭਰਿਆ_Name = localeID.ਮੰਜ਼ਿਲ_String_Piece();
        ਜੇ (ਢੇਰ_3->ptr->ਭਰਿਆ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
            ਵਾਪਸ;
        }

        ਜੇ (ਢੇਰ_3->ptr->ਅਧਾਰ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            ਜੇ (std::string_view::size_type ਅਧਾਰ_Name_ਲੰਮਾਈ = at - localeID.ਅੰਕੜੇ(); ਅਧਾਰ_Name_ਲੰਮਾਈ > 0) {
                ਢੇਰ_3->ptr->ਅਧਾਰ_Name = {ਢੇਰ_3->ptr->ਭਰਿਆ_Name.ਅੰਕੜੇ(), ਅਧਾਰ_Name_ਲੰਮਾਈ};
                ਜੇ (ਢੇਰ_3->ptr->ਅਧਾਰ_Name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                    ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
                    ਵਾਪਸ;
                }
            }
        }
    }
}

ਖਾਲੀ
Locale::ਸੈੱਟ_Unicode_Keyword_ਮੁੱਲ(StringPiece keywordName,
                               StringPiece keyword_ਮੁੱਲ,
                               UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    std::optional<std::string_view> legacy_ਕੁੰਜੀ = ulocimp_ਮੰਜ਼ਿਲ_Legacy_ਕੁੰਜੀ_With_Fallback(keywordName);
    ਜੇ (!legacy_ਕੁੰਜੀ.has_value()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    std::string_view value;

    ਜੇ (!keyword_ਮੁੱਲ.empty()) {
        std::optional<std::string_view> legacy_ਮੁੱਲ =
            ulocimp_ਮੰਜ਼ਿਲ_Legacy_Type_With_Fallback(keywordName, keyword_ਮੁੱਲ);
        ਜੇ (!legacy_ਮੁੱਲ.has_value()) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }
        value = *legacy_ਮੁੱਲ;
    }

    ਸੈੱਟ_Keyword_ਮੁੱਲ(*legacy_ਕੁੰਜੀ, value, ਸਥਿਤੀ);
}

ਅਚਲ ਅੱਖਰ*
Locale::getCountry() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ get_ਖੇਤਰ<&Nest::getRegion, &ਢੇਰ_2::getRegion>();
}

ਅਚਲ ਅੱਖਰ*
Locale::getLanguage() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ get_ਖੇਤਰ<&Nest::getLanguage, &ਢੇਰ_2::getLanguage>();
}

ਅਚਲ ਅੱਖਰ*
Locale::getScript() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ get_ਖੇਤਰ<&Nest::getScript, &ਢੇਰ_2::getScript>();
}

ਅਚਲ ਅੱਖਰ*
Locale::getVariant() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ get_ਖੇਤਰ<&Nest::getVariant, &ਢੇਰ_2::getVariant>();
}

ਅਚਲ ਅੱਖਰ*
Locale::getName() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ get_ਖੇਤਰ<&Nest::get_ਅਧਾਰ_Name, &ਢੇਰ_2::get_ਭਰਿਆ_Name>();
}

ਅਚਲ ਅੱਖਰ*
Locale::get_ਅਧਾਰ_Name() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ get_ਖੇਤਰ<&Nest::get_ਅਧਾਰ_Name, &ਢੇਰ_2::get_ਅਧਾਰ_Name>();
}

ਸਾਂਚਾ <ਅਚਲ ਅੱਖਰ* (Locale::Nest::*ਅਚਲ NEST)() ਅਚਲ,
          ਅਚਲ ਅੱਖਰ* (Locale::ਢੇਰ_2::*ਅਚਲ ਢੇਰ_4)() ਅਚਲ>
ਅਚਲ ਅੱਖਰ* Locale::get_ਖੇਤਰ() ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ payload.ਦੇਖੋ([] { ਵਾਪਸ ""; },
                         [](ਅਚਲ Nest& nest) { ਵਾਪਸ (nest.*NEST)(); },
                         [](ਅਚਲ ਢੇਰ_2& ਢੇਰ_3) { ਵਾਪਸ (ਢੇਰ_3.*ਢੇਰ_4)(); });
}

Locale::Iterator::~Iterator() = ਮੂਲ;

//eof
U_NAMESPACE_ਸਿਰਾ
