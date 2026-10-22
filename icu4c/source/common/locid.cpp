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

#ಅನ್ತರ್ಭಾವ <cstddef>
#ಅನ್ತರ್ಭಾವ <optional>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <type_traits>
#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/localebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಅನ್ತರ್ಭಾವ "unicode/strenum.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"

#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "charstrmap.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "fixedstring.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uniquecharstr.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

#ಆದರೆ UPRV_HAS_FEATURE(ಸ್ಮರಣೆ_sanitizer)
#ಅನ್ತರ್ಭಾವ <sanitizer/msan_interface.h>
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_BEGIN

ಸ್ಥಿರ Locale   *gLocaleCache = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
ಸ್ಥಿರ UMutex gDefaultLocaleMutex;
ಸ್ಥಿರ UHashtable *gDefaultLocalesHashT = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ Locale *gDefaultLocale = ಶೂನ್ಯಸೂಚಕ;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ULOC_STRING_ಎಲ್ಲೆ 357913941

U_NAMESPACE_ಕೊನೆ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ELocalePos {
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
    e_ಬೇರು,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

ನಾಮಾಕಾಶ {

//
// Deleter function for Locales owned by the default Locale hash table/
//
ಶೂನ್ಯ U_CALLCONV
deleteLocale(ಶೂನ್ಯ *obj) {
    ಅಳಿಸು static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    ಅಳಿಸು [] gLocaleCache;
    gLocaleCache = ಶೂನ್ಯಸೂಚಕ;
    gLocaleCacheInitOnce.reset();

    ಆದರೆ (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = ಶೂನ್ಯಸೂಚಕ;
    }
    gDefaultLocale = ಶೂನ್ಯಸೂಚಕ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ U_CALLCONV locale_init(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == ಶೂನ್ಯಸೂಚಕ);
    gLocaleCache = ಹೊಸ Locale[static_cast<ಪೂರ್ಣಾಂಕ>(eMAX_LOCALES)];
    ಆದರೆ (gLocaleCache == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_ಬೇರು]          = Locale("");
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

Locale *locale_ಗಣ_default_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    // Synchronize this entire function.
    Mutex ಬೀಗ(&gDefaultLocaleMutex);

    UBool canonicalize = ಸುಳ್ಳು;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    ಆದರೆ (id == ಶೂನ್ಯಸೂಚಕ) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = ನಿಜ; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, ಸ್ಥಿತಿ_2) : ulocimp_getName(id, ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ gDefaultLocale;
    }

    ಆದರೆ (gDefaultLocalesHashT == ಶೂನ್ಯಸೂಚಕ) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ gDefaultLocale;
        }
        uhash_ಗಣ_ಮೌಲ್ಯ_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.ದತ್ತಾಂಶ()));
    ಆದರೆ (newDefault == ಶೂನ್ಯಸೂಚಕ) {
        newDefault = ಹೊಸ Locale(Locale::eBOGUS);
        ಆದರೆ (newDefault == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ gDefaultLocale;
        }
        newDefault->init(localeNameBuf.ದತ್ತಾಂಶ(), ಸುಳ್ಳು);
        uhash_put(gDefaultLocalesHashT, const_cast<ಅಕ್ಷರ*>(newDefault->getName()), newDefault, &ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    ಹಿಂತಿರುಗಿ gDefaultLocale;
}

U_NAMESPACE_ಕೊನೆ

/* sfb 07/21/99 */
U_CFUNC ಶೂನ್ಯ
locale_ಗಣ_default(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id)
{
    U_NAMESPACE_USE
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    locale_ಗಣ_default_ಆಂತರಿಕ(id, ಸ್ಥಿತಿ_2);
}
/*ಕೊನೆ*/

U_CFUNC ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
locale_get_default()
{
    U_NAMESPACE_USE
    ಹಿಂತಿರುಗಿ Locale::getDefault().getName();
}

ನಾಮಾಕಾಶ {

ಮಾದರಿ <ಸ್ವಯಂಚಾಲಿತ ಕ್ಷೇತ್ರ, ಬಗೆ_ಹೆಸರು T>
ಶೂನ್ಯ copy_ಗಮ್ಯ_Array(std::string_view sv, T* that) {
    ಸ್ವಯಂಚಾಲಿತ& ಕ್ಷೇತ್ರ_2 = that->*ಕ್ಷೇತ್ರ;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(ಕ್ಷೇತ್ರ_2)>>;
    static_assert(capacity > 0);
    ಆದರೆ (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(ಕ್ಷೇತ್ರ_2, sv.ದತ್ತಾಂಶ(), sv.size());
    }
    ಕ್ಷೇತ್ರ_2[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

ಶೂನ್ಯ Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copy_ಗಮ್ಯ_Array<&Nest::language>(language, ಇದು);
    copy_ಗಮ್ಯ_Array<&Nest::script>(script, ಇದು);
    copy_ಗಮ್ಯ_Array<&Nest::region>(region, ಇದು);
    ಇದು->variantBegin = variantBegin;
}

Locale::Nest::Nest(ರಾಶಿ_2&& ರಾಶಿ_3, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(ರಾಶಿ_2, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(ರಾಶಿ_2, region));
    U_ASSERT(ಇದು == reinterpret_cast<Nest*>(&ರಾಶಿ_3));
#ಆದರೆ UPRV_HAS_FEATURE(ಸ್ಮರಣೆ_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&ರಾಶಿ_3, ಗಾತ್ರ(ರಾಶಿ_2));
#ಆದರೆ_ಅಂತ್ಯ
    copy_ಗಮ್ಯ_Array<&Nest::script>(ರಾಶಿ_3.script, ಇದು);
    copy_ಗಮ್ಯ_Array<&Nest::region>(ರಾಶಿ_3.region, ಇದು);
    ಇದು->variantBegin = variantBegin;
    *ಇದು->ತಳ_Name = '\0';
}

ರಚನೆ Locale::ರಾಶಿ_2::Alloc : ಸಾರ್ವಜನಿಕ UMemory {
    FixedString ತುಂಬಿದ_Name;
    FixedString ತಳ_Name;
    int32_t variantBegin;

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* getVariant() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
        ಹಿಂತಿರುಗಿ variantBegin == 0 ? "" : get_ತಳ_Name() + variantBegin;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* get_ತುಂಬಿದ_Name() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ತುಂಬಿದ_Name.ದತ್ತಾಂಶ(); }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* get_ತಳ_Name() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
        ಆದರೆ (ತಳ_Name.is_ಖಾಲಿಗಳು()) {
            ಆದರೆ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name = ತುಂಬಿದ_Name.ದತ್ತಾಂಶ(); *name != '@') {
                ಹಿಂತಿರುಗಿ name;
            }
        }
        ಹಿಂತಿರುಗಿ ತಳ_Name.ದತ್ತಾಂಶ();
    }

    Alloc(int32_t variantBegin) : ತುಂಬಿದ_Name(), ತಳ_Name(), variantBegin(variantBegin) {}

    Alloc(ಸ್ಥಿರಾಂಕ Alloc& other, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
        : ತುಂಬಿದ_Name(), ತಳ_Name(), variantBegin(other.variantBegin) {
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಆದರೆ (!other.ತುಂಬಿದ_Name.is_ಖಾಲಿಗಳು()) {
                ತುಂಬಿದ_Name = other.ತುಂಬಿದ_Name;
                ಆದರೆ (ತುಂಬಿದ_Name.is_ಖಾಲಿಗಳು()) {
                    ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ (!other.ತಳ_Name.is_ಖಾಲಿಗಳು()) {
                        ತಳ_Name = other.ತಳ_Name;
                        ಆದರೆ (ತಳ_Name.is_ಖಾಲಿಗಳು()) {
                            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = ಅಳಿಸು;

    ~Alloc() = ಪೂರ್ವನಿಯೋಜಿತ;
};

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* Locale::ರಾಶಿ_2::getVariant() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ptr->getVariant(); }
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* Locale::ರಾಶಿ_2::get_ತುಂಬಿದ_Name() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ptr->get_ತುಂಬಿದ_Name(); }
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* Locale::ರಾಶಿ_2::get_ತಳ_Name() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ ptr->get_ತಳ_Name(); }

Locale::ರಾಶಿ_2::ರಾಶಿ_2(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = ಹೊಸ Alloc(variantBegin);
    ಆದರೆ (ptr == ಶೂನ್ಯಸೂಚಕ) {
        type = eBOGUS;
    } ಇಲ್ಲದಿದ್ದರೆ {
        type = e_ರಾಶಿ;
        copy_ಗಮ್ಯ_Array<&ರಾಶಿ_2::language>(language, ಇದು);
        copy_ಗಮ್ಯ_Array<&ರಾಶಿ_2::script>(script, ಇದು);
        copy_ಗಮ್ಯ_Array<&ರಾಶಿ_2::region>(region, ಇದು);
    }
}

Locale::ರಾಶಿ_2::~ರಾಶಿ_2() {
    U_ASSERT(type == e_ರಾಶಿ);
    ಅಳಿಸು ptr;
}

Locale::ರಾಶಿ_2& Locale::ರಾಶಿ_2::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ರಾಶಿ_2& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ptr = ಹೊಸ Alloc(*other.ptr, ಸ್ಥಿತಿ_2);
    ಆದರೆ (ptr == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಅಳಿಸು ptr;
    } ಇಲ್ಲದಿದ್ದರೆ {
        type = e_ರಾಶಿ;
        uprv_memcpy(language, other.language, ಗಾತ್ರ language);
        uprv_memcpy(script, other.script, ಗಾತ್ರ script);
        uprv_memcpy(region, other.region, ಗಾತ್ರ region);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Locale::ರಾಶಿ_2& Locale::ರಾಶಿ_2::ಕ್ರಿಯಾಕಾರಕ=(ರಾಶಿ_2&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = e_ರಾಶಿ;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, ಗಾತ್ರ language);
    uprv_memcpy(script, other.script, ಗಾತ್ರ script);
    uprv_memcpy(region, other.region, ಗಾತ್ರ region);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು BogusFn, ಬಗೆ_ಹೆಸರು NestFn, ಬಗೆ_ಹೆಸರು ರಾಶಿ_Fn, ಬಗೆ_ಹೆಸರು... ವಾದಗಳು>
ಸ್ವಯಂಚಾಲಿತ Locale::Payload::ಭೇಟಿ(BogusFn bogusFn, NestFn nestFn, ರಾಶಿ_Fn ರಾಶಿ_Fn_2, ವಾದಗಳು... ವಾದಗಳು_2) ಸ್ಥಿರಾಂಕ {
    ಆಯ್ಕೆ (type) {
        ಸ್ಥಿತಿ eBOGUS:
            ಹಿಂತಿರುಗಿ bogusFn(ವಾದಗಳು_2...);
        ಸ್ಥಿತಿ eNEST:
            ಹಿಂತಿರುಗಿ nestFn(nest, ವಾದಗಳು_2...);
        ಸ್ಥಿತಿ e_ರಾಶಿ:
            ಹಿಂತಿರುಗಿ ರಾಶಿ_Fn_2(ರಾಶಿ_3, ವಾದಗಳು_2...);
        ಪೂರ್ವನಿಯೋಜಿತ:
            UPRV_UNREACHABLE_EXIT;
    };
}

ಶೂನ್ಯ Locale::Payload::copy(ಸ್ಥಿರಾಂಕ Payload& other) {
    other.ಭೇಟಿ([](Payload*) {},
                [](ಸ್ಥಿರಾಂಕ Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ಸ್ಥಿರಾಂಕ ರಾಶಿ_2& ರಾಶಿ_3, Payload* dst) { dst->ರಾಶಿ_3 = ರಾಶಿ_3; },
                ಇದು);
}

ಶೂನ್ಯ Locale::Payload::move(Payload&& other) noexcept {
    other.ಭೇಟಿ(
        [](Payload*) {},
        [](ಸ್ಥಿರಾಂಕ Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ಸ್ಥಿರಾಂಕ ರಾಶಿ_2& ರಾಶಿ_3, Payload* dst) { dst->ರಾಶಿ_3 = std::move(const_cast<ರಾಶಿ_2&>(ರಾಶಿ_3)); },
        ಇದು);
}

Locale::Payload::~Payload() {
    ಆದರೆ (type == e_ರಾಶಿ) { ರಾಶಿ_3.~ರಾಶಿ_2(); }
}

Locale::Payload::Payload(ಸ್ಥಿರಾಂಕ Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Payload& other) U_LIFETIME_BOUND {
    ಆದರೆ (ಇದು != &other) {
        ಗಣ_ಗಮ್ಯ_Bogus();
        copy(other);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Locale::Payload& Locale::Payload::ಕ್ರಿಯಾಕಾರಕ=(Payload&& other) noexcept U_LIFETIME_BOUND {
    ಆದರೆ (ಇದು != &other) {
        ಗಣ_ಗಮ್ಯ_Bogus();
        move(std::move(other));
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಶೂನ್ಯ Locale::Payload::ಗಣ_ಗಮ್ಯ_Bogus() {
    ಇದು->~Payload();
    type = eBOGUS;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು T, ಬಗೆ_ಹೆಸರು... ವಾದಗಳು> T& Locale::Payload::emplace(ವಾದಗಳು&&... ವಾದಗಳು_2) U_LIFETIME_BOUND {
    ಆದರೆ constexpr (std::is_same_v<T, Nest>) {
        ಇದು->~Payload();
        ::ಹೊಸ (&nest) Nest(std::forward<ವಾದಗಳು>(ವಾದಗಳು_2)...);
        ಹಿಂತಿರುಗಿ nest;
    }
    ಆದರೆ constexpr (std::is_same_v<T, ರಾಶಿ_2>) {
        U_ASSERT(type != e_ರಾಶಿ);
        ::ಹೊಸ (&ರಾಶಿ_3) ರಾಶಿ_2(std::forward<ವಾದಗಳು>(ವಾದಗಳು_2)...);
        ಹಿಂತಿರುಗಿ ರಾಶಿ_3;
    }
}

ಮಾದರಿ <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ type == eNEST ? &nest : ಶೂನ್ಯಸೂಚಕ;
}
ಮಾದರಿ <> Locale::ರಾಶಿ_2* Locale::Payload::get() U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ type == e_ರಾಶಿ ? &ರಾಶಿ_3 : ಶೂನ್ಯಸೂಚಕ;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#ವ್ಯಾಖ್ಯಾನಿಸು SEP_CHAR '_'
#ವ್ಯಾಖ್ಯಾನಿಸು NULL_CHAR '\0'

Locale::~Locale() = ಪೂರ್ವನಿಯೋಜಿತ;

Locale::Locale()
    : UObject(), payload()
{
    init(ಶೂನ್ಯಸೂಚಕ, ಸುಳ್ಳು);
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


Locale::Locale( ಸ್ಥಿರಾಂಕ   ಅಕ್ಷರ * newLanguage,
                ಸ್ಥಿರಾಂಕ   ಅಕ್ಷರ * newCountry,
                ಸ್ಥಿರಾಂಕ   ಅಕ್ಷರ * newVariant,
                ಸ್ಥಿರಾಂಕ   ಅಕ್ಷರ * newKeywords)
    : UObject(), payload()
{
    ಆದರೆ( (newLanguage==ಶೂನ್ಯಸೂಚಕ) && (newCountry == ಶೂನ್ಯಸೂಚಕ) && (newVariant == ಶೂನ್ಯಸೂಚಕ) )
    {
        init(ಶೂನ್ಯಸೂಚಕ, ಸುಳ್ಳು); /* shortcut */
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        ಆದರೆ ( newLanguage != ಶೂನ್ಯಸೂಚಕ )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            ಆದರೆ ( lsize < 0 || lsize > ULOC_STRING_ಎಲ್ಲೆ ) { // int32 wrap
                ಹಿಂತಿರುಗಿ;
            }
        }

        CharString togo(newLanguage, lsize, ಸ್ಥಿತಿ_2); // start with newLanguage

        // _Country
        ಆದರೆ ( newCountry != ಶೂನ್ಯಸೂಚಕ )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            ಆದರೆ ( csize < 0 || csize > ULOC_STRING_ಎಲ್ಲೆ ) { // int32 wrap
                ಹಿಂತಿರುಗಿ;
            }
        }

        // _Variant
        ಆದರೆ ( newVariant != ಶೂನ್ಯಸೂಚಕ )
        {
            // remove leading _'s
            ಆಗ(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            ಆದರೆ ( vsize < 0 || vsize > ULOC_STRING_ಎಲ್ಲೆ ) { // int32 wrap
                ಹಿಂತಿರುಗಿ;
            }
            ಆಗ( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        ಆದರೆ ( newKeywords != ಶೂನ್ಯಸೂಚಕ)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            ಆದರೆ ( ksize < 0 || ksize > ULOC_STRING_ಎಲ್ಲೆ ) {
              ಹಿಂತಿರುಗಿ;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        ಆದರೆ ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, ಸ್ಥಿತಿ_2);
        }

        ಆದರೆ ( csize != 0 )
        {
            togo.append(newCountry, ಸ್ಥಿತಿ_2);
        }

        ಆದರೆ ( vsize != 0)
        {
            togo.append(SEP_CHAR, ಸ್ಥಿತಿ_2)
                .append(newVariant, vsize, ಸ್ಥಿತಿ_2);
        }

        ಆದರೆ ( ksize != 0)
        {
            ಆದರೆ (uprv_strchr(newKeywords, '=')) {
                togo.append('@', ಸ್ಥಿತಿ_2); /* keyword parsing */
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                togo.append('_', ಸ್ಥಿತಿ_2); /* Variant parsing with a script */
                ಆದರೆ ( vsize == 0) {
                    togo.append('_', ಸ್ಥಿತಿ_2); /* No country found */
                }
            }
            togo.append(newKeywords, ಸ್ಥಿತಿ_2);
        }

        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            // Something went wrong with appending, etc.
            ಹಿಂತಿರುಗಿ;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.ದತ್ತಾಂಶ(), ಸುಳ್ಳು);
    }
}

Locale::Locale(ಸ್ಥಿರಾಂಕ Locale&) = ಪೂರ್ವನಿಯೋಜಿತ;
Locale::Locale(Locale&&) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;

Locale& Locale::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Locale&) U_LIFETIME_BOUND = ಪೂರ್ವನಿಯೋಜಿತ;
Locale& Locale::ಕ್ರಿಯಾಕಾರಕ=(Locale&&) noexcept U_LIFETIME_BOUND = ಪೂರ್ವನಿಯೋಜಿತ;

Locale *
Locale::clone() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಹೊಸ Locale(*ಇದು);
}

ತಾರ್ಕಿಕ
Locale::ಕ್ರಿಯಾಕಾರಕ==( ಸ್ಥಿರಾಂಕ   Locale& other) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ uprv_strcmp(other.getName(), getName()) == 0;
}

ನಾಮಾಕಾಶ {

UInitOnce g_ತಿಳಿದ_Canonicalized_Init_Once {};
UHashtable *g_ತಿಳಿದ_Canonicalized = ಶೂನ್ಯಸೂಚಕ;

constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ತಿಳಿದ_CANONICALIZED[] = {
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

UBool U_CALLCONV cleanup_ತಿಳಿದ_Canonicalized() {
    g_ತಿಳಿದ_Canonicalized_Init_Once.reset();
    ಆದರೆ (g_ತಿಳಿದ_Canonicalized) { uhash_close(g_ತಿಳಿದ_Canonicalized); }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ U_CALLCONV ತುಂಬು_ತಿಳಿದ_Canonicalized(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_LOCALE_ತಿಳಿದ_CANONICALIZED,
                                cleanup_ತಿಳಿದ_Canonicalized);
    Local_UHashtable_ಸೂಚಕ new_ತಿಳಿದ_Canonicalized_Map(
        uhash_open(uhash_hashChars, uhash_compareChars, ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));
    ಗಾಗಿ (int32_t i = 0;
            U_SUCCESS(ಸ್ಥಿತಿ_2) && i < UPRV_LENGTHOF(ತಿಳಿದ_CANONICALIZED);
            i++) {
        uhash_puti(new_ತಿಳಿದ_Canonicalized_Map.getAlias(),
                   (ಶೂನ್ಯ*)ತಿಳಿದ_CANONICALIZED[i],
                   1, &ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    g_ತಿಳಿದ_Canonicalized = new_ತಿಳಿದ_Canonicalized_Map.orphan();
}

ವರ್ಗ Alias_ದತ್ತಾಂಶ;

/**
 * A Builder class to build the alias data.
 */
ವರ್ಗ Alias_ದತ್ತಾಂಶ_Builder {
ಸಾರ್ವಜನಿಕ:
    Alias_ದತ್ತಾಂಶ_Builder() {
    }

    // Build the AliasData from resource.
    Alias_ದತ್ತಾಂಶ* build(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

ಖಾಸಗಿ:
    ಶೂನ್ಯ readAlias(UResourceBundle* alias,
                   Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
                   Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
                   Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
                   int32_t &length,
                   ಶೂನ್ಯ (*checkType)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type),
                   ಶೂನ್ಯ (*checkReplacement)(ಸ್ಥಿರಾಂಕ UChar* replacement),
                   UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ಶೂನ್ಯ readLanguageAlias(UResourceBundle* alias,
                           Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
                           Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
                           Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
                           int32_t &length,
                           UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ಶೂನ್ಯ readScriptAlias(UResourceBundle* alias,
                         Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
                         Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
                         Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
                         int32_t &length, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ಶೂನ್ಯ readTerritoryAlias(UResourceBundle* alias,
                            Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
                            Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
                            Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
                            int32_t &length, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    ಶೂನ್ಯ readVariantAlias(UResourceBundle* alias,
                          Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
                          Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
                          Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
                          int32_t &length, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    ಶೂನ್ಯ readSubdivisionAlias(UResourceBundle* alias,
                          Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
                          Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
                          Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
                          int32_t &length, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
};

/**
 * A class to hold the Alias Data.
 */
ವರ್ಗ Alias_ದತ್ತಾಂಶ : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Alias_ದತ್ತಾಂಶ* singleton(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            // Do not get into loadData if the status already has error.
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        umtx_initOnce(Alias_ದತ್ತಾಂಶ::gInitOnce, &Alias_ದತ್ತಾಂಶ::ತುಂಬು_ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ gSingleton;
    }

    ಸ್ಥಿರಾಂಕ CharStringMap& languageMap() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ language; }
    ಸ್ಥಿರಾಂಕ CharStringMap& scriptMap() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ script; }
    ಸ್ಥಿರಾಂಕ CharStringMap& territoryMap() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ territory; }
    ಸ್ಥಿರಾಂಕ CharStringMap& variantMap() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ variant; }
    ಸ್ಥಿರಾಂಕ CharStringMap& subdivisionMap() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ subdivision; }

    ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV ತುಂಬು_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    ಸ್ಥಿರ UBool U_CALLCONV cleanup();

    ಸ್ಥಿರ UInitOnce gInitOnce;

ಖಾಸಗಿ:
    Alias_ದತ್ತಾಂಶ(CharStringMap languageMap,
              CharStringMap scriptMap,
              CharStringMap territoryMap,
              CharStringMap variantMap,
              CharStringMap subdivisionMap,
              CharString* ಅಕ್ಷರಸರಣಿಗಳು)
        : language(std::move(languageMap)),
          script(std::move(scriptMap)),
          territory(std::move(territoryMap)),
          variant(std::move(variantMap)),
          subdivision(std::move(subdivisionMap)),
          ಅಕ್ಷರಸರಣಿಗಳು(ಅಕ್ಷರಸರಣಿಗಳು) {
    }

    ~Alias_ದತ್ತಾಂಶ() {
        ಅಳಿಸು ಅಕ್ಷರಸರಣಿಗಳು;
    }

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Alias_ದತ್ತಾಂಶ* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* ಅಕ್ಷರಸರಣಿಗಳು;

    ಮಿತ್ರ ವರ್ಗ Alias_ದತ್ತಾಂಶ_Builder;
};


ಸ್ಥಿರಾಂಕ Alias_ದತ್ತಾಂಶ* Alias_ದತ್ತಾಂಶ::gSingleton = ಶೂನ್ಯಸೂಚಕ;
UInitOnce Alias_ದತ್ತಾಂಶ::gInitOnce {};

UBool U_CALLCONV
Alias_ದತ್ತಾಂಶ::cleanup()
{
    gInitOnce.reset();
    ಅಳಿಸು gSingleton;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ
Alias_ದತ್ತಾಂಶ_Builder::readAlias(
        UResourceBundle* alias,
        Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
        Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
        Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
        int32_t &length,
        ಶೂನ್ಯ (*checkType)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type),
        ಶೂನ್ಯ (*checkReplacement)(ಸ್ಥಿರಾಂಕ UChar* replacement),
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    length = ures_get_ಗಾತ್ರ(alias);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** rawTypes = types.allocateInsteadAndCopy(length);
    ಆದರೆ (rawTypes == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    ಆದರೆ (rawIndexes == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; U_SUCCESS(ಸ್ಥಿತಿ_2) && ures_has_ಮುಂದಿನ(alias); i++) {
        Local_UResource_Bundle_ಸೂಚಕ res(
            ures_get_ಮುಂದಿನ_Resource(alias, ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* aliasFrom = ures_get_ಕೀಲಿ(res.getAlias());
        ಸ್ಥಿರಾಂಕ UChar* alias_ಗಮ್ಯ =
            ures_get_String_By_ಕೀಲಿ(res.getAlias(), "replacement", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;

        checkType(aliasFrom);
        checkReplacement(alias_ಗಮ್ಯ);

        rawTypes[i] = aliasFrom;
        rawIndexes[i] = ಅಕ್ಷರಸರಣಿಗಳು->ಕೂಡಿಸು(alias_ಗಮ್ಯ, ಸ್ಥಿತಿ_2);
    }
}

/**
 * Read the languageAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement language.
 */
ಶೂನ್ಯ
Alias_ದತ್ತಾಂಶ_Builder::readLanguageAlias(
        UResourceBundle* alias,
        Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
        Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
        Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ readAlias(
        alias, ಅಕ್ಷರಸರಣಿಗಳು, types, replacementIndexes, length,
#ಆದರೆ U_DEBUG
        [](ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type) {
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
#ಇಲ್ಲದಿದ್ದರೆ
        [](ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) {},
#ಆದರೆ_ಅಂತ್ಯ
        [](ಸ್ಥಿರಾಂಕ UChar*) {}, ಸ್ಥಿತಿ_2);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
ಶೂನ್ಯ
Alias_ದತ್ತಾಂಶ_Builder::readScriptAlias(
        UResourceBundle* alias,
        Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
        Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
        Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ readAlias(
        alias, ಅಕ್ಷರಸರಣಿಗಳು, types, replacementIndexes, length,
#ಆದರೆ U_DEBUG
        [](ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ಸ್ಥಿರಾಂಕ UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#ಇಲ್ಲದಿದ್ದರೆ
        [](ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) {},
        [](ಸ್ಥಿರಾಂಕ UChar*) { },
#ಆದರೆ_ಅಂತ್ಯ
        ಸ್ಥಿತಿ_2);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
ಶೂನ್ಯ
Alias_ದತ್ತಾಂಶ_Builder::readTerritoryAlias(
        UResourceBundle* alias,
        Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
        Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
        Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ readAlias(
        alias, ಅಕ್ಷರಸರಣಿಗಳು, types, replacementIndexes, length,
#ಆದರೆ U_DEBUG
        [](ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#ಇಲ್ಲದಿದ್ದರೆ
        [](ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) {},
#ಆದರೆ_ಅಂತ್ಯ
        [](ಸ್ಥಿರಾಂಕ UChar*) { },
        ಸ್ಥಿತಿ_2);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
ಶೂನ್ಯ
Alias_ದತ್ತಾಂಶ_Builder::readVariantAlias(
        UResourceBundle* alias,
        Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
        Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
        Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ readAlias(
        alias, ಅಕ್ಷರಸರಣಿಗಳು, types, replacementIndexes, length,
#ಆದರೆ U_DEBUG
        [](ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](ಸ್ಥಿರಾಂಕ UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#ಇಲ್ಲದಿದ್ದರೆ
        [](ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) {},
        [](ಸ್ಥಿರಾಂಕ UChar*) { },
#ಆದರೆ_ಅಂತ್ಯ
        ಸ್ಥಿತಿ_2);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
ಶೂನ್ಯ
Alias_ದತ್ತಾಂಶ_Builder::readSubdivisionAlias(
        UResourceBundle* alias,
        Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು* ಅಕ್ಷರಸರಣಿಗಳು,
        Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>& types,
        Local_ಸ್ಮರಣೆ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ readAlias(
        alias, ಅಕ್ಷರಸರಣಿಗಳು, types, replacementIndexes, length,
#ಆದರೆ U_DEBUG
        [](ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#ಇಲ್ಲದಿದ್ದರೆ
        [](ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) {},
#ಆದರೆ_ಅಂತ್ಯ
        [](ಸ್ಥಿರಾಂಕ UChar*) { },
        ಸ್ಥಿತಿ_2);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
ಶೂನ್ಯ U_CALLCONV
Alias_ದತ್ತಾಂಶ::ತುಂಬು_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOCALE_CANONICALIZATION_DEBUG
    UDate ಆರಂಭ = uprv_getRawUTCtime();
#ಆದರೆ_ಅಂತ್ಯ  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    Alias_ದತ್ತಾಂಶ_Builder builder;
    gSingleton = builder.build(ಸ್ಥಿತಿ_2);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOCALE_CANONICALIZATION_DEBUG
    UDate ಕೊನೆ = uprv_getRawUTCtime();
    ಬರೆ("AliasData::loadData took total %f ms\n", ಕೊನೆ - ಆರಂಭ);
#ಆದರೆ_ಅಂತ್ಯ  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
Alias_ದತ್ತಾಂಶ*
Alias_ದತ್ತಾಂಶ_Builder::build(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }

    Local_UResource_Bundle_ಸೂಚಕ metadata(
        ures_open_ನೇರ(ಶೂನ್ಯಸೂಚಕ, "metadata", &ಸ್ಥಿತಿ_2));
    Local_UResource_Bundle_ಸೂಚಕ metadataAlias(
        ures_get_By_ಕೀಲಿ(metadata.getAlias(), "alias", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));
    Local_UResource_Bundle_ಸೂಚಕ languageAlias(
        ures_get_By_ಕೀಲಿ(metadataAlias.getAlias(), "language", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));
    Local_UResource_Bundle_ಸೂಚಕ scriptAlias(
        ures_get_By_ಕೀಲಿ(metadataAlias.getAlias(), "script", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));
    Local_UResource_Bundle_ಸೂಚಕ territoryAlias(
        ures_get_By_ಕೀಲಿ(metadataAlias.getAlias(), "territory", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));
    Local_UResource_Bundle_ಸೂಚಕ variantAlias(
        ures_get_By_ಕೀಲಿ(metadataAlias.getAlias(), "variant", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));
    Local_UResource_Bundle_ಸೂಚಕ subdivisionAlias(
        ures_get_By_ಕೀಲಿ(metadataAlias.getAlias(), "subdivision", ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    int32_t languages_ಉದ್ದಳತೆ = 0, script_ಉದ್ದಳತೆ = 0, territory_ಉದ್ದಳತೆ = 0,
            variant_ಉದ್ದಳತೆ = 0, subdivision_ಉದ್ದಳತೆ = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು ಅಕ್ಷರಸರಣಿಗಳು(ಸ್ಥಿತಿ_2);
    Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*> languageTypes;
    Local_ಸ್ಮರಣೆ<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &ಅಕ್ಷರಸರಣಿಗಳು,
                      languageTypes,
                      languageReplacementIndexes,
                      languages_ಉದ್ದಳತೆ,
                      ಸ್ಥಿತಿ_2);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*> scriptTypes;
    Local_ಸ್ಮರಣೆ<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &ಅಕ್ಷರಸರಣಿಗಳು,
                    scriptTypes,
                    scriptReplacementIndexes,
                    script_ಉದ್ದಳತೆ,
                    ಸ್ಥಿತಿ_2);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*> territoryTypes;
    Local_ಸ್ಮರಣೆ<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &ಅಕ್ಷರಸರಣಿಗಳು,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territory_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*> variantTypes;
    Local_ಸ್ಮರಣೆ<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &ಅಕ್ಷರಸರಣಿಗಳು,
                     variantTypes,
                     variantReplacementIndexes,
                     variant_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    Local_ಸ್ಮರಣೆ<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*> subdivisionTypes;
    Local_ಸ್ಮರಣೆ<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &ಅಕ್ಷರಸರಣಿಗಳು,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivision_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // We can only use strings after freeze it.
    ಅಕ್ಷರಸರಣಿಗಳು.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, ಸ್ಥಿತಿ_2);
    ಗಾಗಿ (int32_t i = 0; U_SUCCESS(ಸ್ಥಿತಿ_2) && i < languages_ಉದ್ದಳತೆ; i++) {
        languageMap.put(languageTypes[i],
                        ಅಕ್ಷರಸರಣಿಗಳು.get(languageReplacementIndexes[i]),
                        ಸ್ಥಿತಿ_2);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, ಸ್ಥಿತಿ_2);
    ಗಾಗಿ (int32_t i = 0; U_SUCCESS(ಸ್ಥಿತಿ_2) && i < script_ಉದ್ದಳತೆ; i++) {
        scriptMap.put(scriptTypes[i],
                      ಅಕ್ಷರಸರಣಿಗಳು.get(scriptReplacementIndexes[i]),
                      ಸ್ಥಿತಿ_2);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, ಸ್ಥಿತಿ_2);
    ಗಾಗಿ (int32_t i = 0; U_SUCCESS(ಸ್ಥಿತಿ_2) && i < territory_ಉದ್ದಳತೆ; i++) {
        territoryMap.put(territoryTypes[i],
                         ಅಕ್ಷರಸರಣಿಗಳು.get(territoryReplacementIndexes[i]),
                         ಸ್ಥಿತಿ_2);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, ಸ್ಥಿತಿ_2);
    ಗಾಗಿ (int32_t i = 0; U_SUCCESS(ಸ್ಥಿತಿ_2) && i < variant_ಉದ್ದಳತೆ; i++) {
        variantMap.put(variantTypes[i],
                       ಅಕ್ಷರಸರಣಿಗಳು.get(variantReplacementIndexes[i]),
                       ಸ್ಥಿತಿ_2);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, ಸ್ಥಿತಿ_2);
    ಗಾಗಿ (int32_t i = 0; U_SUCCESS(ಸ್ಥಿತಿ_2) && i < subdivision_ಉದ್ದಳತೆ; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       ಅಕ್ಷರಸರಣಿಗಳು.get(subdivisionReplacementIndexes[i]),
                       ಸ್ಥಿತಿ_2);
    }

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // copy hashtables
    ಸ್ವಯಂಚಾಲಿತ *ದತ್ತಾಂಶ = ಹೊಸ Alias_ದತ್ತಾಂಶ(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        ಅಕ್ಷರಸರಣಿಗಳು.orphan_Char_ಅಕ್ಷರಸರಣಿಗಳು());

    ಆದರೆ (ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
ವರ್ಗ AliasReplacer {
ಸಾರ್ವಜನಿಕ:
    AliasReplacer(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) :
            language(ಶೂನ್ಯಸೂಚಕ), script(ಶೂನ್ಯಸೂಚಕ), region(ಶೂನ್ಯಸೂಚಕ),
            extensions(ಶೂನ್ಯಸೂಚಕ),
            // store value in variants only once
            variants(ಶೂನ್ಯಸೂಚಕ,
                     ([](UElement e1, UElement e2) -> UBool {
                       ಹಿಂತಿರುಗಿ 0==uprv_strcmp((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)e1.ಸೂಚಕ,
                                             (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)e2.ಸೂಚಕ);}),
                     ಸ್ಥಿತಿ_2),
            ದತ್ತಾಂಶ(ಶೂನ್ಯಸೂಚಕ) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    ತಾರ್ಕಿಕ replace(
        ಸ್ಥಿರಾಂಕ Locale& locale, CharString& out, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* language;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* script;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* extensions;
    UVector variants;

    ಸ್ಥಿರಾಂಕ Alias_ದತ್ತಾಂಶ* ದತ್ತಾಂಶ;

    ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ not_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* str) {
        ಹಿಂತಿರುಗಿ str && str[0] != NULL_CHAR;
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
    ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* deleteOrReplace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* input U_LIFETIME_BOUND,
                                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type,
                                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* replacement U_LIFETIME_BOUND) {
        ಹಿಂತಿರುಗಿ not_ಖಾಲಿಗಳು(replacement) ?
            ((input == ಶೂನ್ಯಸೂಚಕ) ?  replacement : input) :
            ((type == ಶೂನ್ಯಸೂಚಕ) ? input  : ಶೂನ್ಯಸೂಚಕ);
    }

    ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ same(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* b) {
        ಆದರೆ (a == ಶೂನ್ಯಸೂಚಕ && b == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        ಆದರೆ ((a == ಶೂನ್ಯಸೂಚಕ && b != ಶೂನ್ಯಸೂಚಕ) ||
            (a != ಶೂನ್ಯಸೂಚಕ && b == ಶೂನ್ಯಸೂಚಕ)) {
          ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        ಹಿಂತಿರುಗಿ uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& output_ಗಮ್ಯ_String(CharString& out U_LIFETIME_BOUND, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    // Generate the lookup key.
    CharString& generate_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* language, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant, CharString& out U_LIFETIME_BOUND,
                            UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    ಶೂನ್ಯ parseLanguageReplacement(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* replacement,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*& replaceLanguage,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*& replaceScript,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*& replaceRegion,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*& replaceVariant,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*& replaceExtensions,
                                  UVector& ಗಮ್ಯ_Be_Freed,
                                  UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    // Replace by using languageAlias.
    ತಾರ್ಕಿಕ replaceLanguage(ತಾರ್ಕಿಕ checkLanguage, ತಾರ್ಕಿಕ checkRegion,
                         ತಾರ್ಕಿಕ checkVariants, UVector& ಗಮ್ಯ_Be_Freed,
                         UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    // Replace by using territoryAlias.
    ತಾರ್ಕಿಕ replaceTerritory(UVector& ಗಮ್ಯ_Be_Freed, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    // Replace by using scriptAlias.
    ತಾರ್ಕಿಕ replaceScript(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    // Replace by using variantAlias.
    ತಾರ್ಕಿಕ replaceVariant(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    // Replace by using subdivisionAlias.
    ತಾರ್ಕಿಕ replaceSubdivision(StringPiece subdivision,
                            CharString& output, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    // Replace transformed extensions.
    ತಾರ್ಕಿಕ replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);
};

CharString&
AliasReplacer::generate_ಕೀಲಿ(
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* language, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* region, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant,
        CharString& out U_LIFETIME_BOUND, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ out; }
    out.append(language, ಸ್ಥಿತಿ_2);
    ಆದರೆ (not_ಖಾಲಿಗಳು(region)) {
        out.append(SEP_CHAR, ಸ್ಥಿತಿ_2)
            .append(region, ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (not_ಖಾಲಿಗಳು(variant)) {
       out.append(SEP_CHAR, ಸ್ಥಿತಿ_2)
           .append(variant, ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ out;
}

ಶೂನ್ಯ
AliasReplacer::parseLanguageReplacement(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* replacement,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*& replacedLanguage,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*& replacedScript,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*& replacedRegion,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*& replacedVariant,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*& replacedExtensions,
    UVector& ಗಮ್ಯ_Be_Freed,
    UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (uprv_strchr(replacement, '_') == ಶೂನ್ಯಸೂಚಕ) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        ಹಿಂತಿರುಗಿ;
    }
    // We have multiple field so we have to allocate and parse
    ಸ್ವಯಂಚಾಲಿತ str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), ಸ್ಥಿತಿ_2, ಸ್ಥಿತಿ_2);
    ಅಕ್ಷರ* ದತ್ತಾಂಶ = str->ದತ್ತಾಂಶ();
    ಗಮ್ಯ_Be_Freed.adoptElement(str.release(), ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    replacedLanguage = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) ದತ್ತಾಂಶ;
    ಅಕ್ಷರ* ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ = uprv_strchr(ದತ್ತಾಂಶ, '_');
    *ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ = '\0'; // null terminiate it.
    ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ++;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಆರಂಭ = ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ;
    ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ = const_cast<ಅಕ್ಷರ*>(uprv_strchr(ಆರಂಭ, '_'));
    size_t len = 0;
    ಆದರೆ (ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ == ಶೂನ್ಯಸೂಚಕ) {
        len = uprv_strlen(ಆರಂಭ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        len = ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ - ಆರಂಭ;
        *ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ = '\0'; // null terminiate it.
    }
    ಆದರೆ (len == 4 && uprv_isASCIILetter(*ಆರಂಭ)) {
        // Got a script
        replacedScript = ಆರಂಭ;
        ಆದರೆ (ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆರಂಭ = ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ++;
        ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ = const_cast<ಅಕ್ಷರ*>(uprv_strchr(ಆರಂಭ, '_'));
        ಆದರೆ (ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ == ಶೂನ್ಯಸೂಚಕ) {
            len = uprv_strlen(ಆರಂಭ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            len = ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ - ಆರಂಭ;
            *ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ = '\0'; // null terminiate it.
        }
    }
    ಆದರೆ (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = ಆರಂಭ;
        ಆದರೆ (ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆರಂಭ = ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ++;
        ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ = const_cast<ಅಕ್ಷರ*>(uprv_strchr(ಆರಂಭ, '_'));
        ಆದರೆ (ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ == ಶೂನ್ಯಸೂಚಕ) {
            len = uprv_strlen(ಆರಂಭ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            len = ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ - ಆರಂಭ;
            *ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ = '\0'; // null terminiate it.
        }
    }
    ಆದರೆ (len >= 4) {
        // Got a variant
        replacedVariant = ಆರಂಭ;
        ಆದರೆ (ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆರಂಭ = ಕೊನೆ_ದ_ಕ್ಷೇತ್ರ++;
    }
    replacedExtensions = ಆರಂಭ;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceLanguage(
        ತಾರ್ಕಿಕ checkLanguage, ತಾರ್ಕಿಕ checkRegion,
        ತಾರ್ಕಿಕ checkVariants, UVector& ಗಮ್ಯ_Be_Freed, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (    (checkRegion && region == ಶೂನ್ಯಸೂಚಕ) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    int32_t variant_ಗಾತ್ರ = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಹುಡುಕು_Language = checkLanguage ? language : "und";
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಹುಡುಕು_Region = checkRegion ? region : ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಹುಡುಕು_Variant = ಶೂನ್ಯಸೂಚಕ;
    ಗಾಗಿ (int32_t variant_ಸೂಚ್ಯಂಕ = 0;
            variant_ಸೂಚ್ಯಂಕ < variant_ಗಾತ್ರ;
            variant_ಸೂಚ್ಯಂಕ++) {
        ಆದರೆ (checkVariants) {
            U_ASSERT(variant_ಸೂಚ್ಯಂಕ < variant_ಗಾತ್ರ);
            ಹುಡುಕು_Variant = static_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(variants.elementAt(variant_ಸೂಚ್ಯಂಕ));
        }

        ಆದರೆ (ಹುಡುಕು_Variant != ಶೂನ್ಯಸೂಚಕ && uprv_strlen(ಹುಡುಕು_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            ಹುಡುಕು_Variant = ಶೂನ್ಯಸೂಚಕ;
        }
        CharString type_ಕೀಲಿ;
        generate_ಕೀಲಿ(ಹುಡುಕು_Language, ಹುಡುಕು_Region, ಹುಡುಕು_Variant, type_ಕೀಲಿ,
                    ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *replacement = ದತ್ತಾಂಶ->languageMap().get(type_ಕೀಲಿ.ದತ್ತಾಂಶ());
        ಆದರೆ (replacement == ಶೂನ್ಯಸೂಚಕ) {
            // Found no replacement data.
            ಮುಂದುವರಿ;
        }

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* replacedLanguage = ಶೂನ್ಯಸೂಚಕ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* replacedScript = ಶೂನ್ಯಸೂಚಕ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* replacedRegion = ಶೂನ್ಯಸೂಚಕ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* replacedVariant = ಶೂನ್ಯಸೂಚಕ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* replacedExtensions = ಶೂನ್ಯಸೂಚಕ;
        parseLanguageReplacement(replacement,
                                 replacedLanguage,
                                 replacedScript,
                                 replacedRegion,
                                 replacedVariant,
                                 replacedExtensions,
                                 ಗಮ್ಯ_Be_Freed,
                                 ಸ್ಥಿತಿ_2);
        replacedLanguage =
            (replacedLanguage != ಶೂನ್ಯಸೂಚಕ && uprv_strcmp(replacedLanguage, "und") == 0) ?
            language : replacedLanguage;
        replacedScript = deleteOrReplace(script, ಶೂನ್ಯಸೂಚಕ, replacedScript);
        replacedRegion = deleteOrReplace(region, ಹುಡುಕು_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            ಹುಡುಕು_Variant, ಹುಡುಕು_Variant, replacedVariant);

        ಆದರೆ (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(ಹುಡುಕು_Variant, replacedVariant) &&
                replacedExtensions == ಶೂನ್ಯಸೂಚಕ) {
            // Replacement produce no changes.
            ಮುಂದುವರಿ;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        ಆದರೆ (ಹುಡುಕು_Variant != ಶೂನ್ಯಸೂಚಕ) {
            ಆದರೆ (not_ಖಾಲಿಗಳು(replacedVariant)) {
                variants.ಗಣ_Element_At((ಶೂನ್ಯ*)replacedVariant, variant_ಸೂಚ್ಯಂಕ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                variants.removeElementAt(variant_ಸೂಚ್ಯಂಕ);
            }
        }
        ಆದರೆ (replacedExtensions != ಶೂನ್ಯಸೂಚಕ) {
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
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    // Nothing changed by language alias data.
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceTerritory(UVector& ಗಮ್ಯ_Be_Freed, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (region == ಶೂನ್ಯಸೂಚಕ) {
        // No region to search.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *replacement = ದತ್ತಾಂಶ->territoryMap().get(region);
    ಆದರೆ (replacement == ಶೂನ್ಯಸೂಚಕ) {
        // Found no replacement data for this region.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* replacedRegion = replacement;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೊದಲ_Space = uprv_strchr(replacement, ' ');
    ಆದರೆ (ಮೊದಲ_Space != ಶೂನ್ಯಸೂಚಕ) {
        // If there are are more than one region in the replacement.
        // We need to check which one match based on the language.
        // Cannot use nullptr for language because that will construct
        // the default locale, in that case, use "und" to get the correct
        // locale.
        Locale l = LocaleBuilder()
            .ಗಣ_Language(language == ಶೂನ್ಯಸೂಚಕ ? "und" : language)
            .ಗಣ_Script(script)
            .build(ಸ್ಥಿತಿ_2);
        l.ಕೂಡಿಸು_Likely_Subtags(ಸ್ಥಿತಿ_2);
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> ಅಂಶ;
        ಆದರೆ (likelyRegion != ಶೂನ್ಯಸೂಚಕ && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸಿಕ್ಕಿತು_ಒಳ_Replacement = uprv_strstr(replacement,
                                                         likelyRegion);
            ಆದರೆ (ಸಿಕ್ಕಿತು_ಒಳ_Replacement != ಶೂನ್ಯಸೂಚಕ) {
                // Assuming the case there are no three letter region code in
                // the replacement of territoryAlias
                U_ASSERT(ಸಿಕ್ಕಿತು_ಒಳ_Replacement == replacement ||
                         *(ಸಿಕ್ಕಿತು_ಒಳ_Replacement-1) == ' ');
                U_ASSERT(ಸಿಕ್ಕಿತು_ಒಳ_Replacement[len] == ' ' ||
                         ಸಿಕ್ಕಿತು_ಒಳ_Replacement[len] == '\0');
                ಅಂಶ = prv::make_unique<CharString>(
                    ಸಿಕ್ಕಿತು_ಒಳ_Replacement, static_cast<int32_t>(len), ಸ್ಥಿತಿ_2, ಸ್ಥಿತಿ_2);
            }
        }
        ಆದರೆ (!ಅಂಶ && U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಅಂಶ = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(ಮೊದಲ_Space - replacement), ಸ್ಥಿತಿ_2, ಸ್ಥಿತಿ_2);
        }
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
        replacedRegion = ಅಂಶ->ದತ್ತಾಂಶ();
        ಗಮ್ಯ_Be_Freed.adoptElement(ಅಂಶ.release(), ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceScript(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (script == ಶೂನ್ಯಸೂಚಕ) {
        // No script to search.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *replacement = ದತ್ತಾಂಶ->scriptMap().get(script);
    ಆದರೆ (replacement == ಶೂನ್ಯಸೂಚಕ) {
        // Found no replacement data for this script.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceVariant(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    // Since we may have more than one variant, we need to loop through them.
    ಗಾಗಿ (int32_t i = 0; i < variants.size(); i++) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant = static_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(variants.elementAt(i));
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *replacement = ದತ್ತಾಂಶ->variantMap().get(variant);
        ಆದರೆ (replacement == ಶೂನ್ಯಸೂಚಕ) {
            // Found no replacement data for this variant.
            ಮುಂದುವರಿ;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        ಆದರೆ (!same(variant, replacement)) {
            variants.ಗಣ_Element_At((ಶೂನ್ಯ*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            ಆದರೆ (uprv_strcmp(variant, "heploc") == 0) {
                ಗಾಗಿ (int32_t j = 0; j < variants.size(); j++) {
                     ಆದರೆ (uprv_strcmp((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *replacement = ದತ್ತಾಂಶ->subdivisionMap().get(subdivision.ದತ್ತಾಂಶ());
    ಆದರೆ (replacement != ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೊದಲ_Space = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (ಮೊದಲ_Space != ಶೂನ್ಯಸೂಚಕ) ?
            (ಮೊದಲ_Space - replacement) : uprv_strlen(replacement);
        ಆದರೆ (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), ಸ್ಥಿತಿ_2);
            ಆದರೆ (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, ಸ್ಥಿತಿ_2);
            }
        }
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    int32_t len = transformedExtensions.length();
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* str = transformedExtensions.ದತ್ತಾಂಶ();
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* tkey = ultag_get_TKey_ಆರಂಭ(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == ಶೂನ್ಯಸೂಚಕ) ? len : static_cast<int32_t>((tkey - str - 1)));
    ಆದರೆ (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .ಗಣ_Language_ಟ್ಯಾಗ್(StringPiece(str, tlangLen))
            .build(ಸ್ಥಿತಿ_2);
        tlang.canonicalize(ಸ್ಥಿತಿ_2);
        output = tlang.ಗಮ್ಯ_Language_ಟ್ಯಾಗ್<CharString>(ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        T_CString_ಗಮ್ಯ_ಅಧೋ_Case(output.ದತ್ತಾಂಶ());
    }
    ಆದರೆ (tkey != ಶೂನ್ಯಸೂಚಕ) {
        // We need to sort the tfields by tkey
        UVector tfields(ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        ಮಾಡು {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* tvalue = uprv_strchr(tkey, '-');
            ಆದರೆ (tvalue == ಶೂನ್ಯಸೂಚಕ) {
                ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮುಂದಿನ_TKey = ultag_get_TKey_ಆರಂಭ(tvalue);
            ಆದರೆ (ಮುಂದಿನ_TKey != ಶೂನ್ಯಸೂಚಕ) {
                *const_cast<ಅಕ್ಷರ*>(ಮುಂದಿನ_TKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.ಸೇರಿಸು_Element_At((ಶೂನ್ಯ*)tkey, tfields.size(), ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            tkey = ಮುಂದಿನ_TKey;
        } ಆಗ (tkey != ಶೂನ್ಯಸೂಚಕ);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            ಹಿಂತಿರುಗಿ uprv_strcmp((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)e1.ಸೂಚಕ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)e2.ಸೂಚಕ);
        }, ಸ್ಥಿತಿ_2);
        ಗಾಗಿ (int32_t i = 0; i < tfields.size(); i++) {
             ಆದರೆ (output.length() > 0) {
                 output.append('-', ಸ್ಥಿತಿ_2);
             }
             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* tfield = static_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(tfields.elementAt(i));
             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* tvalue = uprv_strchr(tfield, '-');
             ಆದರೆ (tvalue == ಶೂನ್ಯಸೂಚಕ) {
                 ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
                 ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<ಅಕ್ಷರ*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, ಸ್ಥಿತಿ_2).append('-', ಸ್ಥಿತಿ_2);
             std::optional<std::string_view> bcpTValue = ulocimp_ಗಮ್ಯ_Bcp_Type(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, ಸ್ಥಿತಿ_2);
        }
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

CharString&
AliasReplacer::output_ಗಮ್ಯ_String(
    CharString& out U_LIFETIME_BOUND, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ out; }
    out.append(language, ಸ್ಥಿತಿ_2);
    ಆದರೆ (not_ಖಾಲಿಗಳು(script)) {
        out.append(SEP_CHAR, ಸ್ಥಿತಿ_2)
            .append(script, ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (not_ಖಾಲಿಗಳು(region)) {
        out.append(SEP_CHAR, ಸ್ಥಿತಿ_2)
            .append(region, ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (variants.size() > 0) {
        ಆದರೆ (!not_ಖಾಲಿಗಳು(script) && !not_ಖಾಲಿಗಳು(region)) {
          out.append(SEP_CHAR, ಸ್ಥಿತಿ_2);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            ಹಿಂತಿರುಗಿ uprv_strcmp((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)e1.ಸೂಚಕ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)e2.ಸೂಚಕ);
        }, ಸ್ಥಿತಿ_2);
        int32_t variants_ಆರಂಭ = out.length();
        ಗಾಗಿ (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, ಸ್ಥಿತಿ_2)
                 .append(static_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(variants.elementAt(i)),
                         ಸ್ಥಿತಿ_2);
        }
        T_CString_ಗಮ್ಯ_ಊರ್ಧ್ವ_Case(out.ದತ್ತಾಂಶ() + variants_ಆರಂಭ);
    }
    ಆದರೆ (not_ಖಾಲಿಗಳು(extensions)) {
        CharString tmp("und_", ಸ್ಥಿತಿ_2);
        tmp.append(extensions, ಸ್ಥಿತಿ_2);
        Locale tmpLocale(tmp.ದತ್ತಾಂಶ());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ out;
}

ತಾರ್ಕಿಕ
AliasReplacer::replace(ಸ್ಥಿರಾಂಕ Locale& locale, CharString& out, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ದತ್ತಾಂಶ = Alias_ದತ್ತಾಂಶ::singleton(ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    U_ASSERT(ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ);
    out.clear();
    language = locale.getLanguage();
    ಆದರೆ (!not_ಖಾಲಿಗಳು(language)) {
        language = ಶೂನ್ಯಸೂಚಕ;
    }
    script = locale.getScript();
    ಆದರೆ (!not_ಖಾಲಿಗಳು(script)) {
        script = ಶೂನ್ಯಸೂಚಕ;
    }
    region = locale.getCountry();
    ಆದರೆ (!not_ಖಾಲಿಗಳು(region)) {
        region = ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, ಸ್ಥಿತಿ_2);
    ಆದರೆ (!variantsBuff.is_ಖಾಲಿಗಳು()) {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
        ಅಕ್ಷರ* ಆರಂಭ = variantsBuff.ದತ್ತಾಂಶ();
        T_CString_ಗಮ್ಯ_ಅಧೋ_Case(ಆರಂಭ);
        ಅಕ್ಷರ* ಕೊನೆ;
        ಆಗ ((ಕೊನೆ = uprv_strchr(ಆರಂಭ, SEP_CHAR)) != ಶೂನ್ಯಸೂಚಕ &&
               U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            *ಕೊನೆ = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            ಆದರೆ (*ಆರಂಭ && !variants.ಒಳಗೊಂಡಿದೆ(ಆರಂಭ)) {
                variants.ಕೂಡಿಸು_Element(ಆರಂಭ, ಸ್ಥಿತಿ_2);
            }
            ಆರಂಭ = ಕೊನೆ + 1;
        }
        // do not add "" or duplicate data to variants
        ಆದರೆ (*ಆರಂಭ && !variants.ಒಳಗೊಂಡಿದೆ(ಆರಂಭ)) {
            variants.ಕೂಡಿಸು_Element(ಆರಂಭ, ಸ್ಥಿತಿ_2);
        }
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        ಹಿಂತಿರುಗಿ uprv_strcmp((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)e1.ಸೂಚಕ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)e2.ಸೂಚಕ);
    }, ಸ್ಥಿತಿ_2);

    // A changed count to assert when loop too many times.
    int32_t ಬದಲಾಯಿತು = 0;
    ಸ್ಥಿರಾಂಕ int32_t max_ಬದಲಾಯಿತು = 4 + variants.size();
    (ಶೂನ್ಯ)max_ಬದಲಾಯಿತು; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector ಅಕ್ಷರಸರಣಿಗಳು_ಗಮ್ಯ_Be_Freed([](ಶೂನ್ಯ *obj) { ಅಳಿಸು static_cast<CharString*>(obj); },
                             ಶೂನ್ಯಸೂಚಕ, 10, ಸ್ಥಿತಿ_2);
    ಆಗ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        // Something wrong with the data cause looping here more than maxChanged
        // times already.
        U_ASSERT(ಬದಲಾಯಿತು <= max_ಬದಲಾಯಿತು);
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
        ಆದರೆ (    replaceLanguage(ನಿಜ, ನಿಜ,  ನಿಜ,  ಅಕ್ಷರಸರಣಿಗಳು_ಗಮ್ಯ_Be_Freed, ಸ್ಥಿತಿ_2) ||
                replaceLanguage(ನಿಜ, ನಿಜ,  ಸುಳ್ಳು, ಅಕ್ಷರಸರಣಿಗಳು_ಗಮ್ಯ_Be_Freed, ಸ್ಥಿತಿ_2) ||
                replaceLanguage(ನಿಜ, ಸುಳ್ಳು, ನಿಜ,  ಅಕ್ಷರಸರಣಿಗಳು_ಗಮ್ಯ_Be_Freed, ಸ್ಥಿತಿ_2) ||
                replaceLanguage(ನಿಜ, ಸುಳ್ಳು, ಸುಳ್ಳು, ಅಕ್ಷರಸರಣಿಗಳು_ಗಮ್ಯ_Be_Freed, ಸ್ಥಿತಿ_2) ||
                replaceLanguage(ಸುಳ್ಳು,ಸುಳ್ಳು, ನಿಜ,  ಅಕ್ಷರಸರಣಿಗಳು_ಗಮ್ಯ_Be_Freed, ಸ್ಥಿತಿ_2) ||
                replaceTerritory(ಅಕ್ಷರಸರಣಿಗಳು_ಗಮ್ಯ_Be_Freed, ಸ್ಥಿತಿ_2) ||
                replaceScript(ಸ್ಥಿತಿ_2) ||
                replaceVariant(ಸ್ಥಿತಿ_2)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            ಬದಲಾಯಿತು++;
            ಮುಂದುವರಿ;
        }
        // Nothing changed. Break out.
        ನಿಲ್ಲಿಸು;
    }  // while(1)

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* extensionsStr = locale_get_Keywords_ಆರಂಭ(locale.getName());
    ಆದರೆ (ಬದಲಾಯಿತು == 0 && variants.size() <= 1 && extensionsStr == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    output_ಗಮ್ಯ_String(out, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (extensionsStr != ಶೂನ್ಯಸೂಚಕ) {
        ಬದಲಾಯಿತು = 0;
        Locale ತಾತ್ಕಾಲಿಕ(locale);
        Local_ಸೂಚಕ<icu::StringEnumeration> iter(locale.createKeywords(ಸ್ಥಿತಿ_2));
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && !iter.isNull()) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ;
            ಆಗ ((ಕೀಲಿ = iter->next(ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2)) != ಶೂನ್ಯಸೂಚಕ) {
                ಆದರೆ (uprv_strcmp("sd", ಕೀಲಿ) == 0 || uprv_strcmp("rg", ಕೀಲಿ) == 0 ||
                        uprv_strcmp("t", ಕೀಲಿ) == 0) {
                    ಸ್ವಯಂಚಾಲಿತ value = locale.get_Keyword_ಮೌಲ್ಯ<CharString>(ಕೀಲಿ, ಸ್ಥಿತಿ_2);
                    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                        ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
                        ಮುಂದುವರಿ;
                    }
                    CharString replacement;
                    ಆದರೆ (uprv_strlen(ಕೀಲಿ) == 2) {
                        ಆದರೆ (replaceSubdivision(value.ಗಮ್ಯ_String_Piece(), replacement, ಸ್ಥಿತಿ_2)) {
                            ಬದಲಾಯಿತು++;
                            ತಾತ್ಕಾಲಿಕ.ಗಣ_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, replacement.ದತ್ತಾಂಶ(), ಸ್ಥಿತಿ_2);
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        U_ASSERT(uprv_strcmp(ಕೀಲಿ, "t") == 0);
                        ಆದರೆ (replaceTransformedExtensions(value, replacement, ಸ್ಥಿತಿ_2)) {
                            ಬದಲಾಯಿತು++;
                            ತಾತ್ಕಾಲಿಕ.ಗಣ_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, replacement.ದತ್ತಾಂಶ(), ಸ್ಥಿತಿ_2);
                        }
                    }
                    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                    }
                }
            }
        }
        ಆದರೆ (ಬದಲಾಯಿತು != 0) {
            extensionsStr = locale_get_Keywords_ಆರಂಭ(ತಾತ್ಕಾಲಿಕ.getName());
        }
        out.append(extensionsStr, ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    // If the tag is not changed, return.
    ಆದರೆ (uprv_strcmp(out.ದತ್ತಾಂಶ(), locale.getName()) == 0) {
        out.clear();
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
ತಾರ್ಕಿಕ
canonicalizeLocale(ಸ್ಥಿರಾಂಕ Locale& locale, CharString& out, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    AliasReplacer replacer(ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ replacer.replace(locale, out, ಸ್ಥಿತಿ_2);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
ತಾರ್ಕಿಕ
is_ತಿಳಿದ_Canonicalized_Locale(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }

    ಆದರೆ (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    // common well-known Canonicalized.
    umtx_initOnce(g_ತಿಳಿದ_Canonicalized_Init_Once,
                  &ತುಂಬು_ತಿಳಿದ_Canonicalized, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    U_ASSERT(g_ತಿಳಿದ_Canonicalized != ಶೂನ್ಯಸೂಚಕ);
    ಹಿಂತಿರುಗಿ uhash_geti(g_ತಿಳಿದ_Canonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_ಕೊನೆ

// Function for testing.
U_EXPORT ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ*
ulocimp_get_ತಿಳಿದ_Canonicalized_Locale_For_Test(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(ತಿಳಿದ_CANONICALIZED);
    ಹಿಂತಿರುಗಿ ತಿಳಿದ_CANONICALIZED;
}

// Function for testing.
U_EXPORT ತಾರ್ಕಿಕ
ulocimp_isCanonicalizedLocaleForTest(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    CharString ತಾತ್ಕಾಲಿಕ;
    ಹಿಂತಿರುಗಿ !canonicalizeLocale(l, ತಾತ್ಕಾಲಿಕ, ಸ್ಥಿತಿ_2) && U_SUCCESS(ಸ್ಥಿತಿ_2);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    ಹಿಂತಿರುಗಿ localeID == ಶೂನ್ಯಸೂಚಕ ? *ಇದು = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    ಮಾಡು {
        ಅಕ್ಷರ *separator;
        ಅಕ್ಷರ *ಕ್ಷೇತ್ರ_2[5] = {ಶೂನ್ಯಸೂಚಕ};
        int32_t ಕ್ಷೇತ್ರ_Len[5] = {0};
        int32_t ಕ್ಷೇತ್ರ_Idx;
        int32_t variant_ಕ್ಷೇತ್ರ;
        int32_t length;
        UError_ಸಂಕೇತ err;

        ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ parse = [canonicalize](std::string_view localeID,
                                          ಅಕ್ಷರ* name,
                                          int32_t name_ಸಾಮರ್ಥ್ಯ,
                                          UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
                name, name_ಸಾಮರ್ಥ್ಯ,
                [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
                    ಆದರೆ (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, ಸ್ಥಿತಿ_2);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ulocimp_getName(localeID, sink, ಸ್ಥಿತಿ_2);
                    }
                },
                ಸ್ಥಿತಿ_2);
        };

        // "canonicalize" the locale ID to ICU/Java format
        ಅಕ್ಷರ* ತುಂಬಿದ_Name = nest.ತಳ_Name;
        err = U_ZERO_ERROR;
        length = parse(localeID, ತುಂಬಿದ_Name, ಗಾತ್ರ Nest::ತಳ_Name, err);

        FixedString ತುಂಬಿದ_Name_ಬಫರ್;
        ಆದರೆ (err == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR || length >= static_cast<int32_t>(ಗಾತ್ರ Nest::ತಳ_Name)) {
            /*Go to heap for the fullName if necessary*/
            ಆದರೆ (!ತುಂಬಿದ_Name_ಬಫರ್.reserve(length + 1)) {
                ನಿಲ್ಲಿಸು; // error: out of memory
            }
            ತುಂಬಿದ_Name = ತುಂಬಿದ_Name_ಬಫರ್.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, ತುಂಬಿದ_Name, length + 1, err);
        }
        ಆದರೆ(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            ನಿಲ್ಲಿಸು;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = ಕ್ಷೇತ್ರ_2[0] = ತುಂಬಿದ_Name;
        ಕ್ಷೇತ್ರ_Idx = 1;
        ಅಕ್ಷರ* at = uprv_strchr(ತುಂಬಿದ_Name, '@');
        ಆಗ ((separator = uprv_strchr(ಕ್ಷೇತ್ರ_2[ಕ್ಷೇತ್ರ_Idx-1], SEP_CHAR)) != ಶೂನ್ಯಸೂಚಕ &&
               ಕ್ಷೇತ್ರ_Idx < UPRV_LENGTHOF(ಕ್ಷೇತ್ರ_2)-1 &&
               (at == ಶೂನ್ಯಸೂಚಕ || separator < at)) {
            ಕ್ಷೇತ್ರ_2[ಕ್ಷೇತ್ರ_Idx] = separator + 1;
            ಕ್ಷೇತ್ರ_Len[ಕ್ಷೇತ್ರ_Idx - 1] = static_cast<int32_t>(separator - ಕ್ಷೇತ್ರ_2[ಕ್ಷೇತ್ರ_Idx - 1]);
            ಕ್ಷೇತ್ರ_Idx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(ಕ್ಷೇತ್ರ_2[ಕ್ಷೇತ್ರ_Idx-1], '@');
        ಅಕ್ಷರ* sep2 = uprv_strchr(ಕ್ಷೇತ್ರ_2[ಕ್ಷೇತ್ರ_Idx-1], '.');
        ಆದರೆ (separator!=ಶೂನ್ಯಸೂಚಕ || sep2!=ಶೂನ್ಯಸೂಚಕ) {
            ಆದರೆ (separator==ಶೂನ್ಯಸೂಚಕ || (sep2!=ಶೂನ್ಯಸೂಚಕ && separator > sep2)) {
                separator = sep2;
            }
            ಕ್ಷೇತ್ರ_Len[ಕ್ಷೇತ್ರ_Idx - 1] = static_cast<int32_t>(separator - ಕ್ಷೇತ್ರ_2[ಕ್ಷೇತ್ರ_Idx - 1]);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಕ್ಷೇತ್ರ_Len[ಕ್ಷೇತ್ರ_Idx - 1] = length - static_cast<int32_t>(ಕ್ಷೇತ್ರ_2[ಕ್ಷೇತ್ರ_Idx - 1] - ತುಂಬಿದ_Name);
        }
        ತಾರ್ಕಿಕ hasKeywords = at != ಶೂನ್ಯಸೂಚಕ && uprv_strchr(at + 1, '=') != ಶೂನ್ಯಸೂಚಕ;

        ಆದರೆ (ಕ್ಷೇತ್ರ_Len[0] >= ULOC_LANG_ಸಾಮರ್ಥ್ಯ)
        {
            ನಿಲ್ಲಿಸು; // error: the language field is too long
        }

        variant_ಕ್ಷೇತ್ರ = 1; /* Usually the 2nd one, except when a script or country is also used. */
        ಆದರೆ (ಕ್ಷೇತ್ರ_Len[0] > 0) {
            /* We have a language */
            language = {ತುಂಬಿದ_Name, static_cast<std::string_view::size_type>(ಕ್ಷೇತ್ರ_Len[0])};
        }
        ಆದರೆ (ಕ್ಷೇತ್ರ_Len[1] == 4 && uprv_isASCIILetter(ಕ್ಷೇತ್ರ_2[1][0]) &&
                uprv_isASCIILetter(ಕ್ಷೇತ್ರ_2[1][1]) && uprv_isASCIILetter(ಕ್ಷೇತ್ರ_2[1][2]) &&
                uprv_isASCIILetter(ಕ್ಷೇತ್ರ_2[1][3])) {
            /* We have at least a script */
            script = {ಕ್ಷೇತ್ರ_2[1], static_cast<std::string_view::size_type>(ಕ್ಷೇತ್ರ_Len[1])};
            variant_ಕ್ಷೇತ್ರ++;
        }

        ಆದರೆ (ಕ್ಷೇತ್ರ_Len[variant_ಕ್ಷೇತ್ರ] == 2 || ಕ್ಷೇತ್ರ_Len[variant_ಕ್ಷೇತ್ರ] == 3) {
            /* We have a country */
            region = {ಕ್ಷೇತ್ರ_2[variant_ಕ್ಷೇತ್ರ], static_cast<std::string_view::size_type>(ಕ್ಷೇತ್ರ_Len[variant_ಕ್ಷೇತ್ರ])};
            variant_ಕ್ಷೇತ್ರ++;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕ್ಷೇತ್ರ_Len[variant_ಕ್ಷೇತ್ರ] == 0) {
            variant_ಕ್ಷೇತ್ರ++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        ಆದರೆ (ಕ್ಷೇತ್ರ_Len[variant_ಕ್ಷೇತ್ರ] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(ಕ್ಷೇತ್ರ_2[variant_ಕ್ಷೇತ್ರ] - ತುಂಬಿದ_Name);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - ತುಂಬಿದ_Name);
        }

        ಆದರೆ (!hasKeywords && Nest::ಹೊಂದುತ್ತದೆ(length, language, script, region)) {
            U_ASSERT(ತುಂಬಿದ_Name == nest.ತಳ_Name);
            U_ASSERT(ತುಂಬಿದ_Name_ಬಫರ್.is_ಖಾಲಿಗಳು());
            nest.init(language, script, region, variantBegin);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (ತುಂಬಿದ_Name == nest.ತಳ_Name) {
                U_ASSERT(ತುಂಬಿದ_Name_ಬಫರ್.is_ಖಾಲಿಗಳು());
                ತುಂಬಿದ_Name_ಬಫರ್ = {ತುಂಬಿದ_Name, static_cast<std::string_view::size_type>(length)};
                ಆದರೆ (ತುಂಬಿದ_Name_ಬಫರ್.is_ಖಾಲಿಗಳು()) {
                    ನಿಲ್ಲಿಸು; // error: out of memory
                }
                ಆದರೆ (!language.empty()) {
                    language = {ತುಂಬಿದ_Name_ಬಫರ್.ದತ್ತಾಂಶ(), language.size()};
                }
                ಆದರೆ (!script.empty()) {
                    script = {ತುಂಬಿದ_Name_ಬಫರ್.ದತ್ತಾಂಶ() + (script.ದತ್ತಾಂಶ() - ತುಂಬಿದ_Name), script.size()};
                }
                ಆದರೆ (!region.empty()) {
                    region = {ತುಂಬಿದ_Name_ಬಫರ್.ದತ್ತಾಂಶ() + (region.ದತ್ತಾಂಶ() - ತುಂಬಿದ_Name), region.size()};
                }
            }
            ರಾಶಿ_2& ರಾಶಿ_3 = payload.emplace<ರಾಶಿ_2>(language, script, region, variantBegin);
            ಆದರೆ (isBogus()) {
                ನಿಲ್ಲಿಸು; // error: out of memory
            }
            U_ASSERT(!ತುಂಬಿದ_Name_ಬಫರ್.is_ಖಾಲಿಗಳು());
            ರಾಶಿ_3.ptr->ತುಂಬಿದ_Name = std::move(ತುಂಬಿದ_Name_ಬಫರ್);
            ಆದರೆ (hasKeywords) {
                ಆದರೆ (std::string_view::size_type ತಳ_Name_ಉದ್ದಳತೆ = at - ತುಂಬಿದ_Name; ತಳ_Name_ಉದ್ದಳತೆ > 0) {
                    ರಾಶಿ_3.ptr->ತಳ_Name = {ರಾಶಿ_3.ptr->ತುಂಬಿದ_Name.ದತ್ತಾಂಶ(), ತಳ_Name_ಉದ್ದಳತೆ};
                    ಆದರೆ (ರಾಶಿ_3.ptr->ತಳ_Name.is_ಖಾಲಿಗಳು()) {
                        ನಿಲ್ಲಿಸು; // error: out of memory
                    }
                }
            }
        }

        ಆದರೆ (canonicalize) {
            ಆದರೆ (!is_ತಿಳಿದ_Canonicalized_Locale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                ಆದರೆ (canonicalizeLocale(*ಇದು, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.ದತ್ತಾಂಶ(), ಸುಳ್ಳು);
                }
                ಆದರೆ (U_FAILURE(err)) {
                    ನಿಲ್ಲಿಸು;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        ಹಿಂತಿರುಗಿ *ಇದು;
    } ಆಗ(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    ಗಣ_ಗಮ್ಯ_Bogus();

    ಹಿಂತಿರುಗಿ *ಇದು;
}

int32_t
Locale::hash_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

ಶೂನ್ಯ
Locale::ಗಣ_ಗಮ್ಯ_Bogus() {
    /* Free our current storage */
    payload.ಗಣ_ಗಮ್ಯ_Bogus();
}

ಸ್ಥಿರಾಂಕ Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex ಬೀಗ(&gDefaultLocaleMutex);
        ಆದರೆ (gDefaultLocale != ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ *gDefaultLocale;
        }
    }
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ಹಿಂತಿರುಗಿ *locale_ಗಣ_default_ಆಂತರಿಕ(ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
}



ಶೂನ್ಯ U_EXPORT2
Locale::ಗಣ_Default( ಸ್ಥಿರಾಂಕ   Locale&     newLocale,
                            UError_ಸಂಕೇತ&  ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *localeID = newLocale.getName();
    locale_ಗಣ_default_ಆಂತರಿಕ(localeID, ಸ್ಥಿತಿ_2);
}

ಶೂನ್ಯ
Locale::ಕೂಡಿಸು_Likely_Subtags(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    CharString maximizedLocaleID = ulocimp_ಕೂಡಿಸು_Likely_Subtags(getName(), ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (ಸ್ಥಿತಿ_2 == U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR) {
            ಗಣ_ಗಮ್ಯ_Bogus();
        }
        ಹಿಂತಿರುಗಿ;
    }

    init(maximizedLocaleID.ದತ್ತಾಂಶ(), /*canonicalize=*/ಸುಳ್ಳು);
    ಆದರೆ (isBogus()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    }
}

ಶೂನ್ಯ
Locale::minimizeSubtags(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    Locale::minimizeSubtags(ಸುಳ್ಳು, ಸ್ಥಿತಿ_2);
}
ಶೂನ್ಯ
Locale::minimizeSubtags(ತಾರ್ಕಿಕ favorScript, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (ಸ್ಥಿತಿ_2 == U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR) {
            ಗಣ_ಗಮ್ಯ_Bogus();
        }
        ಹಿಂತಿರುಗಿ;
    }

    init(minimizedLocaleID.ದತ್ತಾಂಶ(), /*canonicalize=*/ಸುಳ್ಳು);
    ಆದರೆ (isBogus()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    }
}

ಶೂನ್ಯ
Locale::canonicalize(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (isBogus()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    CharString uncanonicalized(getName(), ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (ಸ್ಥಿತಿ_2 == U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR) {
            ಗಣ_ಗಮ್ಯ_Bogus();
        }
        ಹಿಂತಿರುಗಿ;
    }
    init(uncanonicalized.ದತ್ತಾಂಶ(), /*canonicalize=*/ನಿಜ);
    ಆದರೆ (isBogus()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    }
}

Locale U_EXPORT2
Locale::for_Language_ಟ್ಯಾಗ್(StringPiece ಟ್ಯಾಗ್, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    Locale ಫಲಿತಾಂಶ(Locale::eBOGUS);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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

    int32_t parsed_ಉದ್ದಳತೆ;
    CharString localeID = ulocimp_for_Language_ಟ್ಯಾಗ್(
            ಟ್ಯಾಗ್.ದತ್ತಾಂಶ(),
            ಟ್ಯಾಗ್.length(),
            &parsed_ಉದ್ದಳತೆ,
            ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಆದರೆ (parsed_ಉದ್ದಳತೆ != ಟ್ಯಾಗ್.size()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಫಲಿತಾಂಶ.init(localeID.ದತ್ತಾಂಶ(), /*canonicalize=*/ಸುಳ್ಳು);
    ಆದರೆ (ಫಲಿತಾಂಶ.isBogus()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ
Locale::ಗಮ್ಯ_Language_ಟ್ಯಾಗ್(ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (isBogus()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ulocimp_ಗಮ್ಯ_Language_ಟ್ಯಾಗ್(getName(), sink, /*strict=*/ಸುಳ್ಳು, ಸ್ಥಿತಿ_2);
}

Locale U_EXPORT2
Locale::createFromName (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
    ಆದರೆ (name) {
        Locale l("");
        l.init(name, ಸುಳ್ಳು);
        ಹಿಂತಿರುಗಿ l;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, ಸುಳ್ಳು);
    ಹಿಂತಿರುಗಿ loc;
}

Locale U_EXPORT2
Locale::createCanonical(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name) {
    Locale loc("");
    loc.init(name, ನಿಜ);
    ಹಿಂತಿರುಗಿ loc;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
Locale::getISO3Language() ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ uloc_getISO3Language(getName());
}


ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
Locale::getISO3Country() ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ uloc_getLCID(getName());
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ* U_EXPORT2 Locale::getISOCountries()
{
    ಹಿಂತಿರುಗಿ uloc_getISOCountries();
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ* U_EXPORT2 Locale::getISOLanguages()
{
    ಹಿಂತಿರುಗಿ uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
ಶೂನ್ಯ Locale::ಗಣ_From_POSIXID(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *posixID)
{
    init(posixID, ನಿಜ);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::get_ಬೇರು()
{
    ಹಿಂತಿರುಗಿ getLocale(e_ಬೇರು);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getEnglish()
{
    ಹಿಂತಿರುಗಿ getLocale(eENGLISH);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getFrench()
{
    ಹಿಂತಿರುಗಿ getLocale(eFRENCH);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getGerman()
{
    ಹಿಂತಿರುಗಿ getLocale(eGERMAN);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getItalian()
{
    ಹಿಂತಿರುಗಿ getLocale(eITALIAN);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getJapanese()
{
    ಹಿಂತಿರುಗಿ getLocale(eJAPANESE);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getKorean()
{
    ಹಿಂತಿರುಗಿ getLocale(eKOREAN);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getChinese()
{
    ಹಿಂತಿರುಗಿ getLocale(eCHINESE);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    ಹಿಂತಿರುಗಿ getLocale(eCHINA);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    ಹಿಂತಿರುಗಿ getLocale(eTAIWAN);
}


ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getFrance()
{
    ಹಿಂತಿರುಗಿ getLocale(eFRANCE);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getGermany()
{
    ಹಿಂತಿರುಗಿ getLocale(eGERMANY);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getItaly()
{
    ಹಿಂತಿರುಗಿ getLocale(eITALY);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getJapan()
{
    ಹಿಂತಿರುಗಿ getLocale(eJAPAN);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getKorea()
{
    ಹಿಂತಿರುಗಿ getLocale(eKOREA);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getChina()
{
    ಹಿಂತಿರುಗಿ getLocale(eCHINA);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getPRC()
{
    ಹಿಂತಿರುಗಿ getLocale(eCHINA);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getTaiwan()
{
    ಹಿಂತಿರುಗಿ getLocale(eTAIWAN);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getUK()
{
    ಹಿಂತಿರುಗಿ getLocale(eUK);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getUS()
{
    ಹಿಂತಿರುಗಿ getLocale(eUS);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getCanada()
{
    ಹಿಂತಿರುಗಿ getLocale(eCANADA);
}

ಸ್ಥಿರಾಂಕ Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    ಹಿಂತಿರುಗಿ getLocale(eCANADA_FRENCH);
}

ಸ್ಥಿರಾಂಕ Locale &
Locale::getLocale(ಪೂರ್ಣಾಂಕ locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    ಆದರೆ (localeCache == ಶೂನ್ಯಸೂಚಕ) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    ಹಿಂತಿರುಗಿ localeCache[locid]; /*operating on nullptr*/
}

/*
This function is defined this way in order to get around static
initialization and static destruction.
 */
Locale *
Locale::getLocaleCache()
{
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    umtx_initOnce(gLocaleCacheInitOnce, locale_init, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ gLocaleCache;
}

ವರ್ಗ KeywordEnumeration : ಸಾರ್ವಜನಿಕ StringEnumeration {
ಸಂರಕ್ಷಿತ:
    FixedString keywords;
ಖಾಸಗಿ:
    int32_t length;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *current;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ fgClassID;

ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID() { ಹಿಂತಿರುಗಿ (UClassID)&fgClassID; }
    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ getStaticClassID(); }
ಸಾರ್ವಜನಿಕ:
    KeywordEnumeration(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿಗಳು, int32_t keywordLen, int32_t current_ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
        : keywords(), length(keywordLen), current(ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2) && keywordLen != 0) {
            ಆದರೆ(ಕೀಲಿಗಳು == ಶೂನ್ಯಸೂಚಕ || keywordLen < 0) {
                ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            } ಇಲ್ಲದಿದ್ದರೆ {
                keywords = {ಕೀಲಿಗಳು, static_cast<std::string_view::size_type>(length)};
                ಆದರೆ (keywords.is_ಖಾಲಿಗಳು()) {
                    ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    current = keywords.ದತ್ತಾಂಶ() + current_ಸೂಚ್ಯಂಕ;
                }
            }
        }
    }

    ವಾಸ್ತವಿಕ ~KeywordEnumeration();

    ವಾಸ್ತವಿಕ StringEnumeration * clone() ಸ್ಥಿರಾಂಕ override
    {
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        ಹಿಂತಿರುಗಿ ಹೊಸ KeywordEnumeration(
                keywords.ದತ್ತಾಂಶ(), length,
                static_cast<int32_t>(current - keywords.ದತ್ತಾಂಶ()), ಸ್ಥಿತಿ_2);
    }

    ವಾಸ್ತವಿಕ int32_t count(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ 0; }
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *kw = keywords.ದತ್ತಾಂಶ();
        int32_t ಫಲಿತಾಂಶ = 0;
        ಆಗ(*kw) {
            ಫಲಿತಾಂಶ++;
            kw += uprv_strlen(kw)+1;
        }
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* next(int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND override {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಫಲಿತಾಂಶ;
        int32_t len;
        ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2) && *current != 0) {
            ಫಲಿತಾಂಶ = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            ಆದರೆ(ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) {
                *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = len;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) {
                *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = 0;
            }
            ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
        }
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ UnicodeString* snext(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND override {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
        int32_t ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = 0;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = next(&ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ ಗಣ_Chars(s, ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
    }

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ reset(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) override {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
        current = keywords.ದತ್ತಾಂಶ();
    }
};

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = ಪೂರ್ವನಿಯೋಜಿತ;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
ವರ್ಗ UnicodeKeywordEnumeration : ಸಾರ್ವಜನಿಕ KeywordEnumeration {
ಸಾರ್ವಜನಿಕ:
    ಬಳಸಿ KeywordEnumeration::KeywordEnumeration;
    ವಾಸ್ತವಿಕ ~UnicodeKeywordEnumeration();

    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* next(int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND override {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* legacy_ಕೀಲಿ = KeywordEnumeration::next(ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
        ಆಗ (U_SUCCESS(ಸ್ಥಿತಿ_2) && legacy_ಕೀಲಿ != ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ = uloc_ಗಮ್ಯ_Unicode_Locale_ಕೀಲಿ(legacy_ಕೀಲಿ);
            ಆದರೆ (ಕೀಲಿ != ಶೂನ್ಯಸೂಚಕ) {
                ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) {
                    *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(ಕೀಲಿ));
                }
                ಹಿಂತಿರುಗಿ ಕೀಲಿ;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_ಕೀಲಿ = KeywordEnumeration::next(ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
        }
        ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = 0;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ವಾಸ್ತವಿಕ int32_t count(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ 0; }
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *kw = keywords.ದತ್ತಾಂಶ();
        int32_t ಫಲಿತಾಂಶ = 0;
        ಆಗ(*kw) {
            ಆದರೆ (uloc_ಗಮ್ಯ_Unicode_Locale_ಕೀಲಿ(kw) != ಶೂನ್ಯಸೂಚಕ) {
                ಫಲಿತಾಂಶ++;
            }
            kw += uprv_strlen(kw)+1;
        }
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = ಪೂರ್ವನಿಯೋಜಿತ;

StringEnumeration *
Locale::createKeywords(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    StringEnumeration *ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant_ಆರಂಭ = uprv_strchr(getName(), '@');
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* assignment = uprv_strchr(getName(), '=');
    ಆದರೆ(variant_ಆರಂಭ) {
        ಆದರೆ(assignment > variant_ಆರಂಭ) {
            CharString keywords = ulocimp_getKeywords(variant_ಆರಂಭ + 1, '@', ಸುಳ್ಳು, ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && !keywords.is_ಖಾಲಿಗಳು()) {
                ಫಲಿತಾಂಶ = ಹೊಸ KeywordEnumeration(keywords.ದತ್ತಾಂಶ(), keywords.length(), 0, ಸ್ಥಿತಿ_2);
                ಆದರೆ (!ಫಲಿತಾಂಶ) {
                    ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

StringEnumeration *
Locale::createUnicodeKeywords(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    StringEnumeration *ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* variant_ಆರಂಭ = uprv_strchr(getName(), '@');
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* assignment = uprv_strchr(getName(), '=');
    ಆದರೆ(variant_ಆರಂಭ) {
        ಆದರೆ(assignment > variant_ಆರಂಭ) {
            CharString keywords = ulocimp_getKeywords(variant_ಆರಂಭ + 1, '@', ಸುಳ್ಳು, ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && !keywords.is_ಖಾಲಿಗಳು()) {
                ಫಲಿತಾಂಶ = ಹೊಸ UnicodeKeywordEnumeration(keywords.ದತ್ತಾಂಶ(), keywords.length(), 0, ಸ್ಥಿತಿ_2);
                ಆದರೆ (!ಫಲಿತಾಂಶ) {
                    ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

int32_t
Locale::get_Keyword_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* keywordName, ಅಕ್ಷರ *ಬಫರ್, int32_t bufLen, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ uloc_get_Keyword_ಮೌಲ್ಯ(getName(), keywordName, ಬಫರ್, bufLen, &ಸ್ಥಿತಿ_2);
}

ಶೂನ್ಯ
Locale::get_Keyword_ಮೌಲ್ಯ(StringPiece keywordName, ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (isBogus()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ulocimp_get_Keyword_ಮೌಲ್ಯ(getName(), keywordName, sink, ಸ್ಥಿತಿ_2);
}

ಶೂನ್ಯ
Locale::get_Unicode_Keyword_ಮೌಲ್ಯ(StringPiece keywordName,
                               ByteSink& sink,
                               UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    std::optional<std::string_view> legacy_ಕೀಲಿ = ulocimp_ಗಮ್ಯ_Legacy_ಕೀಲಿ_With_Fallback(keywordName);
    ಆದರೆ (!legacy_ಕೀಲಿ.has_value()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ವಯಂಚಾಲಿತ legacy_ಮೌಲ್ಯ = get_Keyword_ಮೌಲ್ಯ<CharString>(*legacy_ಕೀಲಿ, ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    std::optional<std::string_view> unicode_ಮೌಲ್ಯ =
        ulocimp_ಗಮ್ಯ_Bcp_Type_With_Fallback(keywordName, legacy_ಮೌಲ್ಯ.ಗಮ್ಯ_String_Piece());
    ಆದರೆ (!unicode_ಮೌಲ್ಯ.has_value()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    sink.Append(unicode_ಮೌಲ್ಯ->ದತ್ತಾಂಶ(), static_cast<int32_t>(unicode_ಮೌಲ್ಯ->size()));
}

ಶೂನ್ಯ
Locale::ಗಣ_Keyword_ಮೌಲ್ಯ(StringPiece keywordName,
                        StringPiece keyword_ಮೌಲ್ಯ,
                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
    ಆದರೆ (keywordName.empty()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಸ್ಥಿತಿ_2 == U_STRING_NOT_TERMINATED_WARNING) {
        ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, ಸ್ಥಿತಿ_2);
    ulocimp_ಗಣ_Keyword_ಮೌಲ್ಯ(keywordName, keyword_ಮೌಲ್ಯ, localeID, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (ಸ್ಥಿತಿ_2 == U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR) {
            ಗಣ_ಗಮ್ಯ_Bogus();
        }
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* at = locale_get_Keywords_ಆರಂಭ(localeID.ಗಮ್ಯ_String_Piece());
    ತಾರ್ಕಿಕ hasKeywords = at != ಶೂನ್ಯಸೂಚಕ && uprv_strchr(at + 1, '=') != ಶೂನ್ಯಸೂಚಕ;

    Nest* nest = payload.get<Nest>();
    ಆದರೆ (!hasKeywords) {
        ಆದರೆ (nest == ಶೂನ್ಯಸೂಚಕ) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            ರಾಶಿ_2* ರಾಶಿ_3 = payload.get<ರಾಶಿ_2>();
            U_ASSERT(ರಾಶಿ_3 != ಶೂನ್ಯಸೂಚಕ);
            ಆದರೆ (Nest::ಹೊಂದುತ್ತದೆ(localeID.length(), ರಾಶಿ_3->language, ರಾಶಿ_3->script, ರಾಶಿ_3->region)) {
                int32_t variantBegin = ರಾಶಿ_3->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < ಗಾತ್ರ Nest::ತಳ_Name);
                nest = &payload.emplace<Nest>(std::move(*ರಾಶಿ_3), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->ತಳ_Name, ಗಾತ್ರ Nest::ತಳ_Name, ಸ್ಥಿತಿ_2);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ರಾಶಿ_3->ptr->ತಳ_Name.clear();
                ರಾಶಿ_3->ptr->ತುಂಬಿದ_Name = localeID.ಗಮ್ಯ_String_Piece();
                ಆದರೆ (ರಾಶಿ_3->ptr->ತುಂಬಿದ_Name.is_ಖಾಲಿಗಳು()) {
                    ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಗಣ_ಗಮ್ಯ_Bogus();
                    ಹಿಂತಿರುಗಿ;
                }
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ರಾಶಿ_2* ರಾಶಿ_3 = ಶೂನ್ಯಸೂಚಕ;
        ಆದರೆ (nest != ಶೂನ್ಯಸೂಚಕ) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            ರಾಶಿ_3 = &payload.emplace<ರಾಶಿ_2>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            ಆದರೆ (isBogus()) {
                ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ರಾಶಿ_3 = payload.get<ರಾಶಿ_2>();
        }
        U_ASSERT(ರಾಶಿ_3 != ಶೂನ್ಯಸೂಚಕ);
        ರಾಶಿ_3->ptr->ತುಂಬಿದ_Name = localeID.ಗಮ್ಯ_String_Piece();
        ಆದರೆ (ರಾಶಿ_3->ptr->ತುಂಬಿದ_Name.is_ಖಾಲಿಗಳು()) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಗಣ_ಗಮ್ಯ_Bogus();
            ಹಿಂತಿರುಗಿ;
        }

        ಆದರೆ (ರಾಶಿ_3->ptr->ತಳ_Name.is_ಖಾಲಿಗಳು()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            ಆದರೆ (std::string_view::size_type ತಳ_Name_ಉದ್ದಳತೆ = at - localeID.ದತ್ತಾಂಶ(); ತಳ_Name_ಉದ್ದಳತೆ > 0) {
                ರಾಶಿ_3->ptr->ತಳ_Name = {ರಾಶಿ_3->ptr->ತುಂಬಿದ_Name.ದತ್ತಾಂಶ(), ತಳ_Name_ಉದ್ದಳತೆ};
                ಆದರೆ (ರಾಶಿ_3->ptr->ತಳ_Name.is_ಖಾಲಿಗಳು()) {
                    ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಗಣ_ಗಮ್ಯ_Bogus();
                    ಹಿಂತಿರುಗಿ;
                }
            }
        }
    }
}

ಶೂನ್ಯ
Locale::ಗಣ_Unicode_Keyword_ಮೌಲ್ಯ(StringPiece keywordName,
                               StringPiece keyword_ಮೌಲ್ಯ,
                               UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    std::optional<std::string_view> legacy_ಕೀಲಿ = ulocimp_ಗಮ್ಯ_Legacy_ಕೀಲಿ_With_Fallback(keywordName);
    ಆದರೆ (!legacy_ಕೀಲಿ.has_value()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    std::string_view value;

    ಆದರೆ (!keyword_ಮೌಲ್ಯ.empty()) {
        std::optional<std::string_view> legacy_ಮೌಲ್ಯ =
            ulocimp_ಗಮ್ಯ_Legacy_Type_With_Fallback(keywordName, keyword_ಮೌಲ್ಯ);
        ಆದರೆ (!legacy_ಮೌಲ್ಯ.has_value()) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        value = *legacy_ಮೌಲ್ಯ;
    }

    ಗಣ_Keyword_ಮೌಲ್ಯ(*legacy_ಕೀಲಿ, value, ಸ್ಥಿತಿ_2);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
Locale::getCountry() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ get_ಕ್ಷೇತ್ರ<&Nest::getRegion, &ರಾಶಿ_2::getRegion>();
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
Locale::getLanguage() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ get_ಕ್ಷೇತ್ರ<&Nest::getLanguage, &ರಾಶಿ_2::getLanguage>();
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
Locale::getScript() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ get_ಕ್ಷೇತ್ರ<&Nest::getScript, &ರಾಶಿ_2::getScript>();
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
Locale::getVariant() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ get_ಕ್ಷೇತ್ರ<&Nest::getVariant, &ರಾಶಿ_2::getVariant>();
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
Locale::getName() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ get_ಕ್ಷೇತ್ರ<&Nest::get_ತಳ_Name, &ರಾಶಿ_2::get_ತುಂಬಿದ_Name>();
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
Locale::get_ತಳ_Name() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ get_ಕ್ಷೇತ್ರ<&Nest::get_ತಳ_Name, &ರಾಶಿ_2::get_ತಳ_Name>();
}

ಮಾದರಿ <ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* (Locale::Nest::*ಸ್ಥಿರಾಂಕ NEST)() ಸ್ಥಿರಾಂಕ,
          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* (Locale::ರಾಶಿ_2::*ಸ್ಥಿರಾಂಕ ರಾಶಿ_4)() ಸ್ಥಿರಾಂಕ>
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* Locale::get_ಕ್ಷೇತ್ರ() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ payload.ಭೇಟಿ([] { ಹಿಂತಿರುಗಿ ""; },
                         [](ಸ್ಥಿರಾಂಕ Nest& nest) { ಹಿಂತಿರುಗಿ (nest.*NEST)(); },
                         [](ಸ್ಥಿರಾಂಕ ರಾಶಿ_2& ರಾಶಿ_3) { ಹಿಂತಿರುಗಿ (ರಾಶಿ_3.*ರಾಶಿ_4)(); });
}

Locale::Iterator::~Iterator() = ಪೂರ್ವನಿಯೋಜಿತ;

//eof
U_NAMESPACE_ಕೊನೆ
