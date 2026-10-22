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

#ضمن <cstddef>
#ضمن <optional>
#ضمن <string_view>
#ضمن <type_traits>
#ضمن <utility>

#ضمن "unicode/bytestream.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/localebuilder.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/strenum.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/uloc.h"
#ضمن "unicode/ures.h"

#ضمن "bytesinkutil.h"
#ضمن "charstr.h"
#ضمن "charstrmap.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "fixedstring.h"
#ضمن "mutex.h"
#ضمن "putilimp.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"
#ضمن "uhash.h"
#ضمن "ulocimp.h"
#ضمن "umutex.h"
#ضمن "uniquecharstr.h"
#ضمن "ustr_imp.h"
#ضمن "uvector.h"

#إذا UPRV_HAS_FEATURE(يادگيري_sanitizer)
#ضمن <sanitizer/msan_interface.h>
#جيڪڏهن_پورو

U_NAMESPACE_BEGIN

ساكن Locale   *gLocaleCache = خالي_اشارو;
ساكن UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
ساكن UMutex gDefaultLocaleMutex;
ساكن UHashtable *gDefaultLocalesHashT = خالي_اشارو;
ساكن Locale *gDefaultLocale = خالي_اشارو;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#وصف_ڪريو ULOC_STRING_حد_بندي 357913941

U_NAMESPACE_پڇاڙي

تعريفنوع تعداد ELocalePos {
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
    e_پاڙ,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

نطاق {

//
// Deleter function for Locales owned by the default Locale hash table/
//
فراغ U_CALLCONV
deleteLocale(فراغ *obj) {
    ميٽيو static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    ميٽيو [] gLocaleCache;
    gLocaleCache = خالي_اشارو;
    gLocaleCacheInitOnce.reset();

    إذا (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = خالي_اشارو;
    }
    gDefaultLocale = خالي_اشارو;
    أرجع سچ;
}

فراغ U_CALLCONV locale_init(UError_ڪوڊ &حالت) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == خالي_اشارو);
    gLocaleCache = جديد Locale[static_cast<صحيح>(eMAX_LOCALES)];
    إذا (gLocaleCache == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_پاڙ]          = Locale("");
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

Locale *locale_سيٽ_default_اندروني(ثابت حرف *id, UError_ڪوڊ& حالت) {
    // Synchronize this entire function.
    Mutex تالو(&gDefaultLocaleMutex);

    UBool canonicalize = خطأ;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    إذا (id == خالي_اشارو) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = سچ; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, حالت) : ulocimp_getName(id, حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع gDefaultLocale;
    }

    إذا (gDefaultLocalesHashT == خالي_اشارو) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, خالي_اشارو, &حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع gDefaultLocale;
        }
        uhash_سيٽ_قدر_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_رجسٽر_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.معلومات()));
    إذا (newDefault == خالي_اشارو) {
        newDefault = جديد Locale(Locale::eBOGUS);
        إذا (newDefault == خالي_اشارو) {
            حالت = U_يادگيري_ورهاست_ERROR;
            أرجع gDefaultLocale;
        }
        newDefault->init(localeNameBuf.معلومات(), خطأ);
        uhash_put(gDefaultLocalesHashT, const_cast<حرف*>(newDefault->getName()), newDefault, &حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    أرجع gDefaultLocale;
}

U_NAMESPACE_پڇاڙي

/* sfb 07/21/99 */
U_CFUNC فراغ
locale_سيٽ_default(ثابت حرف *id)
{
    U_NAMESPACE_USE
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    locale_سيٽ_default_اندروني(id, حالت);
}
/*پڇاڙي*/

U_CFUNC ثابت حرف *
locale_get_default()
{
    U_NAMESPACE_USE
    أرجع Locale::getDefault().getName();
}

نطاق {

سانچو <خودڪار ميدان, قسم_نالو T>
فراغ copy_منزل_Array(std::string_view sv, T* that) {
    خودڪار& ميدان_2 = that->*ميدان;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(ميدان_2)>>;
    static_assert(capacity > 0);
    إذا (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(ميدان_2, sv.معلومات(), sv.size());
    }
    ميدان_2[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

فراغ Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copy_منزل_Array<&Nest::language>(language, هذا);
    copy_منزل_Array<&Nest::script>(script, هذا);
    copy_منزل_Array<&Nest::region>(region, هذا);
    هذا->variantBegin = variantBegin;
}

Locale::Nest::Nest(ڍير_2&& ڍير_3, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(ڍير_2, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(ڍير_2, region));
    U_ASSERT(هذا == reinterpret_cast<Nest*>(&ڍير_3));
#إذا UPRV_HAS_FEATURE(يادگيري_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&ڍير_3, حجم(ڍير_2));
#جيڪڏهن_پورو
    copy_منزل_Array<&Nest::script>(ڍير_3.script, هذا);
    copy_منزل_Array<&Nest::region>(ڍير_3.region, هذا);
    هذا->variantBegin = variantBegin;
    *هذا->بنياد_Name = '\0';
}

بنية Locale::ڍير_2::Alloc : عام UMemory {
    FixedString ڀريل_Name;
    FixedString بنياد_Name;
    int32_t variantBegin;

    ثابت حرف* getVariant() ثابت U_LIFETIME_BOUND {
        أرجع variantBegin == 0 ? "" : get_بنياد_Name() + variantBegin;
    }
    ثابت حرف* get_ڀريل_Name() ثابت U_LIFETIME_BOUND { أرجع ڀريل_Name.معلومات(); }
    ثابت حرف* get_بنياد_Name() ثابت U_LIFETIME_BOUND {
        إذا (بنياد_Name.is_خالي_جايون()) {
            إذا (ثابت حرف* name = ڀريل_Name.معلومات(); *name != '@') {
                أرجع name;
            }
        }
        أرجع بنياد_Name.معلومات();
    }

    Alloc(int32_t variantBegin) : ڀريل_Name(), بنياد_Name(), variantBegin(variantBegin) {}

    Alloc(ثابت Alloc& other, UError_ڪوڊ& حالت)
        : ڀريل_Name(), بنياد_Name(), variantBegin(other.variantBegin) {
        إذا (U_SUCCESS(حالت)) {
            إذا (!other.ڀريل_Name.is_خالي_جايون()) {
                ڀريل_Name = other.ڀريل_Name;
                إذا (ڀريل_Name.is_خالي_جايون()) {
                    حالت = U_يادگيري_ورهاست_ERROR;
                } وإلا {
                    إذا (!other.بنياد_Name.is_خالي_جايون()) {
                        بنياد_Name = other.بنياد_Name;
                        إذا (بنياد_Name.is_خالي_جايون()) {
                            حالت = U_يادگيري_ورهاست_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = ميٽيو;

    ~Alloc() = افتراضي;
};

ثابت حرف* Locale::ڍير_2::getVariant() ثابت U_LIFETIME_BOUND { أرجع ptr->getVariant(); }
ثابت حرف* Locale::ڍير_2::get_ڀريل_Name() ثابت U_LIFETIME_BOUND { أرجع ptr->get_ڀريل_Name(); }
ثابت حرف* Locale::ڍير_2::get_بنياد_Name() ثابت U_LIFETIME_BOUND { أرجع ptr->get_بنياد_Name(); }

Locale::ڍير_2::ڍير_2(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = جديد Alloc(variantBegin);
    إذا (ptr == خالي_اشارو) {
        type = eBOGUS;
    } وإلا {
        type = e_ڍير;
        copy_منزل_Array<&ڍير_2::language>(language, هذا);
        copy_منزل_Array<&ڍير_2::script>(script, هذا);
        copy_منزل_Array<&ڍير_2::region>(region, هذا);
    }
}

Locale::ڍير_2::~ڍير_2() {
    U_ASSERT(type == e_ڍير);
    ميٽيو ptr;
}

Locale::ڍير_2& Locale::ڍير_2::عامل=(ثابت ڍير_2& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    ptr = جديد Alloc(*other.ptr, حالت);
    إذا (ptr == خالي_اشارو || U_FAILURE(حالت)) {
        ميٽيو ptr;
    } وإلا {
        type = e_ڍير;
        uprv_memcpy(language, other.language, حجم language);
        uprv_memcpy(script, other.script, حجم script);
        uprv_memcpy(region, other.region, حجم region);
    }
    أرجع *هذا;
}

Locale::ڍير_2& Locale::ڍير_2::عامل=(ڍير_2&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = e_ڍير;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, حجم language);
    uprv_memcpy(script, other.script, حجم script);
    uprv_memcpy(region, other.region, حجم region);
    أرجع *هذا;
}

سانچو <قسم_نالو BogusFn, قسم_نالو NestFn, قسم_نالو ڍير_Fn, قسم_نالو... دليلون>
خودڪار Locale::Payload::گهمو(BogusFn bogusFn, NestFn nestFn, ڍير_Fn ڍير_Fn_2, دليلون... دليلون_2) ثابت {
    بدل (type) {
        حالة eBOGUS:
            أرجع bogusFn(دليلون_2...);
        حالة eNEST:
            أرجع nestFn(nest, دليلون_2...);
        حالة e_ڍير:
            أرجع ڍير_Fn_2(ڍير_3, دليلون_2...);
        افتراضي:
            UPRV_UNREACHABLE_EXIT;
    };
}

فراغ Locale::Payload::copy(ثابت Payload& other) {
    other.گهمو([](Payload*) {},
                [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ثابت ڍير_2& ڍير_3, Payload* dst) { dst->ڍير_3 = ڍير_3; },
                هذا);
}

فراغ Locale::Payload::move(Payload&& other) noexcept {
    other.گهمو(
        [](Payload*) {},
        [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ثابت ڍير_2& ڍير_3, Payload* dst) { dst->ڍير_3 = std::move(const_cast<ڍير_2&>(ڍير_3)); },
        هذا);
}

Locale::Payload::~Payload() {
    إذا (type == e_ڍير) { ڍير_3.~ڍير_2(); }
}

Locale::Payload::Payload(ثابت Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::عامل=(ثابت Payload& other) U_LIFETIME_BOUND {
    إذا (هذا != &other) {
        سيٽ_منزل_Bogus();
        copy(other);
    }
    أرجع *هذا;
}

Locale::Payload& Locale::Payload::عامل=(Payload&& other) noexcept U_LIFETIME_BOUND {
    إذا (هذا != &other) {
        سيٽ_منزل_Bogus();
        move(std::move(other));
    }
    أرجع *هذا;
}

فراغ Locale::Payload::سيٽ_منزل_Bogus() {
    هذا->~Payload();
    type = eBOGUS;
}

سانچو <قسم_نالو T, قسم_نالو... دليلون> T& Locale::Payload::emplace(دليلون&&... دليلون_2) U_LIFETIME_BOUND {
    إذا constexpr (std::is_same_v<T, Nest>) {
        هذا->~Payload();
        ::جديد (&nest) Nest(std::forward<دليلون>(دليلون_2)...);
        أرجع nest;
    }
    إذا constexpr (std::is_same_v<T, ڍير_2>) {
        U_ASSERT(type != e_ڍير);
        ::جديد (&ڍير_3) ڍير_2(std::forward<دليلون>(دليلون_2)...);
        أرجع ڍير_3;
    }
}

سانچو <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    أرجع type == eNEST ? &nest : خالي_اشارو;
}
سانچو <> Locale::ڍير_2* Locale::Payload::get() U_LIFETIME_BOUND {
    أرجع type == e_ڍير ? &ڍير_3 : خالي_اشارو;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#وصف_ڪريو SEP_CHAR '_'
#وصف_ڪريو NULL_CHAR '\0'

Locale::~Locale() = افتراضي;

Locale::Locale()
    : UObject(), payload()
{
    init(خالي_اشارو, خطأ);
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


Locale::Locale( ثابت   حرف * newLanguage,
                ثابت   حرف * newCountry,
                ثابت   حرف * newVariant,
                ثابت   حرف * newKeywords)
    : UObject(), payload()
{
    إذا( (newLanguage==خالي_اشارو) && (newCountry == خالي_اشارو) && (newVariant == خالي_اشارو) )
    {
        init(خالي_اشارو, خطأ); /* shortcut */
    }
    وإلا
    {
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        إذا ( newLanguage != خالي_اشارو )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            إذا ( lsize < 0 || lsize > ULOC_STRING_حد_بندي ) { // int32 wrap
                أرجع;
            }
        }

        CharString togo(newLanguage, lsize, حالت); // start with newLanguage

        // _Country
        إذا ( newCountry != خالي_اشارو )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            إذا ( csize < 0 || csize > ULOC_STRING_حد_بندي ) { // int32 wrap
                أرجع;
            }
        }

        // _Variant
        إذا ( newVariant != خالي_اشارو )
        {
            // remove leading _'s
            طالما(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            إذا ( vsize < 0 || vsize > ULOC_STRING_حد_بندي ) { // int32 wrap
                أرجع;
            }
            طالما( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        إذا ( newKeywords != خالي_اشارو)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            إذا ( ksize < 0 || ksize > ULOC_STRING_حد_بندي ) {
              أرجع;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        إذا ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, حالت);
        }

        إذا ( csize != 0 )
        {
            togo.append(newCountry, حالت);
        }

        إذا ( vsize != 0)
        {
            togo.append(SEP_CHAR, حالت)
                .append(newVariant, vsize, حالت);
        }

        إذا ( ksize != 0)
        {
            إذا (uprv_strchr(newKeywords, '=')) {
                togo.append('@', حالت); /* keyword parsing */
            }
            وإلا {
                togo.append('_', حالت); /* Variant parsing with a script */
                إذا ( vsize == 0) {
                    togo.append('_', حالت); /* No country found */
                }
            }
            togo.append(newKeywords, حالت);
        }

        إذا (U_FAILURE(حالت)) {
            // Something went wrong with appending, etc.
            أرجع;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.معلومات(), خطأ);
    }
}

Locale::Locale(ثابت Locale&) = افتراضي;
Locale::Locale(Locale&&) noexcept = افتراضي;

Locale& Locale::عامل=(ثابت Locale&) U_LIFETIME_BOUND = افتراضي;
Locale& Locale::عامل=(Locale&&) noexcept U_LIFETIME_BOUND = افتراضي;

Locale *
Locale::clone() ثابت {
    أرجع جديد Locale(*هذا);
}

منطقي
Locale::عامل==( ثابت   Locale& other) ثابت
{
    أرجع uprv_strcmp(other.getName(), getName()) == 0;
}

نطاق {

UInitOnce g_معلوم_Canonicalized_Init_Once {};
UHashtable *g_معلوم_Canonicalized = خالي_اشارو;

constexpr ثابت حرف* معلوم_CANONICALIZED[] = {
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

UBool U_CALLCONV cleanup_معلوم_Canonicalized() {
    g_معلوم_Canonicalized_Init_Once.reset();
    إذا (g_معلوم_Canonicalized) { uhash_close(g_معلوم_Canonicalized); }
    أرجع سچ;
}

فراغ U_CALLCONV ڀريو_معلوم_Canonicalized(UError_ڪوڊ &حالت) {
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_LOCALE_معلوم_CANONICALIZED,
                                cleanup_معلوم_Canonicalized);
    Local_UHashtable_اشارو new_معلوم_Canonicalized_Map(
        uhash_open(uhash_hashChars, uhash_compareChars, خالي_اشارو, &حالت));
    لكل (int32_t i = 0;
            U_SUCCESS(حالت) && i < UPRV_LENGTHOF(معلوم_CANONICALIZED);
            i++) {
        uhash_puti(new_معلوم_Canonicalized_Map.getAlias(),
                   (فراغ*)معلوم_CANONICALIZED[i],
                   1, &حالت);
    }
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    g_معلوم_Canonicalized = new_معلوم_Canonicalized_Map.orphan();
}

صنف Alias_معلومات;

/**
 * A Builder class to build the alias data.
 */
صنف Alias_معلومات_Builder {
عام:
    Alias_معلومات_Builder() {
    }

    // Build the AliasData from resource.
    Alias_معلومات* build(UError_ڪوڊ &حالت);

خاص:
    فراغ readAlias(UResourceBundle* alias,
                   Unique_Char_لڙيون* لڙيون,
                   Local_يادگيري<ثابت حرف*>& types,
                   Local_يادگيري<int32_t>& replacementIndexes,
                   int32_t &length,
                   فراغ (*checkType)(ثابت حرف* type),
                   فراغ (*checkReplacement)(ثابت UChar* replacement),
                   UError_ڪوڊ &حالت);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readLanguageAlias(UResourceBundle* alias,
                           Unique_Char_لڙيون* لڙيون,
                           Local_يادگيري<ثابت حرف*>& types,
                           Local_يادگيري<int32_t>& replacementIndexes,
                           int32_t &length,
                           UError_ڪوڊ &حالت);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readScriptAlias(UResourceBundle* alias,
                         Unique_Char_لڙيون* لڙيون,
                         Local_يادگيري<ثابت حرف*>& types,
                         Local_يادگيري<int32_t>& replacementIndexes,
                         int32_t &length, UError_ڪوڊ &حالت);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readTerritoryAlias(UResourceBundle* alias,
                            Unique_Char_لڙيون* لڙيون,
                            Local_يادگيري<ثابت حرف*>& types,
                            Local_يادگيري<int32_t>& replacementIndexes,
                            int32_t &length, UError_ڪوڊ &حالت);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    فراغ readVariantAlias(UResourceBundle* alias,
                          Unique_Char_لڙيون* لڙيون,
                          Local_يادگيري<ثابت حرف*>& types,
                          Local_يادگيري<int32_t>& replacementIndexes,
                          int32_t &length, UError_ڪوڊ &حالت);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    فراغ readSubdivisionAlias(UResourceBundle* alias,
                          Unique_Char_لڙيون* لڙيون,
                          Local_يادگيري<ثابت حرف*>& types,
                          Local_يادگيري<int32_t>& replacementIndexes,
                          int32_t &length, UError_ڪوڊ &حالت);
};

/**
 * A class to hold the Alias Data.
 */
صنف Alias_معلومات : عام UMemory {
عام:
    ساكن ثابت Alias_معلومات* singleton(UError_ڪوڊ& حالت) {
        إذا (U_FAILURE(حالت)) {
            // Do not get into loadData if the status already has error.
            أرجع خالي_اشارو;
        }
        umtx_initOnce(Alias_معلومات::gInitOnce, &Alias_معلومات::ڀريو_معلومات, حالت);
        أرجع gSingleton;
    }

    ثابت CharStringMap& languageMap() ثابت U_LIFETIME_BOUND { أرجع language; }
    ثابت CharStringMap& scriptMap() ثابت U_LIFETIME_BOUND { أرجع script; }
    ثابت CharStringMap& territoryMap() ثابت U_LIFETIME_BOUND { أرجع territory; }
    ثابت CharStringMap& variantMap() ثابت U_LIFETIME_BOUND { أرجع variant; }
    ثابت CharStringMap& subdivisionMap() ثابت U_LIFETIME_BOUND { أرجع subdivision; }

    ساكن فراغ U_CALLCONV ڀريو_معلومات(UError_ڪوڊ &حالت);
    ساكن UBool U_CALLCONV cleanup();

    ساكن UInitOnce gInitOnce;

خاص:
    Alias_معلومات(CharStringMap languageMap,
              CharStringMap scriptMap,
              CharStringMap territoryMap,
              CharStringMap variantMap,
              CharStringMap subdivisionMap,
              CharString* لڙيون)
        : language(std::move(languageMap)),
          script(std::move(scriptMap)),
          territory(std::move(territoryMap)),
          variant(std::move(variantMap)),
          subdivision(std::move(subdivisionMap)),
          لڙيون(لڙيون) {
    }

    ~Alias_معلومات() {
        ميٽيو لڙيون;
    }

    ساكن ثابت Alias_معلومات* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* لڙيون;

    دوست صنف Alias_معلومات_Builder;
};


ثابت Alias_معلومات* Alias_معلومات::gSingleton = خالي_اشارو;
UInitOnce Alias_معلومات::gInitOnce {};

UBool U_CALLCONV
Alias_معلومات::cleanup()
{
    gInitOnce.reset();
    ميٽيو gSingleton;
    أرجع سچ;
}

فراغ
Alias_معلومات_Builder::readAlias(
        UResourceBundle* alias,
        Unique_Char_لڙيون* لڙيون,
        Local_يادگيري<ثابت حرف*>& types,
        Local_يادگيري<int32_t>& replacementIndexes,
        int32_t &length,
        فراغ (*checkType)(ثابت حرف* type),
        فراغ (*checkReplacement)(ثابت UChar* replacement),
        UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    length = ures_get_سائيز(alias);
    ثابت حرف** rawTypes = types.allocateInsteadAndCopy(length);
    إذا (rawTypes == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    إذا (rawIndexes == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    لكل (صحيح i = 0; U_SUCCESS(حالت) && ures_has_ايندڙ(alias); i++) {
        Local_UResource_Bundle_اشارو res(
            ures_get_ايندڙ_Resource(alias, خالي_اشارو, &حالت));
        ثابت حرف* aliasFrom = ures_get_ڪنجي(res.getAlias());
        ثابت UChar* alias_منزل =
            ures_get_String_By_ڪنجي(res.getAlias(), "replacement", خالي_اشارو, &حالت);
        إذا (U_FAILURE(حالت)) أرجع;

        checkType(aliasFrom);
        checkReplacement(alias_منزل);

        rawTypes[i] = aliasFrom;
        rawIndexes[i] = لڙيون->جوڙيو(alias_منزل, حالت);
    }
}

/**
 * Read the languageAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement language.
 */
فراغ
Alias_معلومات_Builder::readLanguageAlias(
        UResourceBundle* alias,
        Unique_Char_لڙيون* لڙيون,
        Local_يادگيري<ثابت حرف*>& types,
        Local_يادگيري<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ڪوڊ &حالت)
{
    أرجع readAlias(
        alias, لڙيون, types, replacementIndexes, length,
#إذا U_DEBUG
        [](ثابت حرف* type) {
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
#وإلا
        [](ثابت حرف*) {},
#جيڪڏهن_پورو
        [](ثابت UChar*) {}, حالت);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
فراغ
Alias_معلومات_Builder::readScriptAlias(
        UResourceBundle* alias,
        Unique_Char_لڙيون* لڙيون,
        Local_يادگيري<ثابت حرف*>& types,
        Local_يادگيري<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ڪوڊ &حالت)
{
    أرجع readAlias(
        alias, لڙيون, types, replacementIndexes, length,
#إذا U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ثابت UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#وإلا
        [](ثابت حرف*) {},
        [](ثابت UChar*) { },
#جيڪڏهن_پورو
        حالت);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
فراغ
Alias_معلومات_Builder::readTerritoryAlias(
        UResourceBundle* alias,
        Unique_Char_لڙيون* لڙيون,
        Local_يادگيري<ثابت حرف*>& types,
        Local_يادگيري<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ڪوڊ &حالت)
{
    أرجع readAlias(
        alias, لڙيون, types, replacementIndexes, length,
#إذا U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#وإلا
        [](ثابت حرف*) {},
#جيڪڏهن_پورو
        [](ثابت UChar*) { },
        حالت);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
فراغ
Alias_معلومات_Builder::readVariantAlias(
        UResourceBundle* alias,
        Unique_Char_لڙيون* لڙيون,
        Local_يادگيري<ثابت حرف*>& types,
        Local_يادگيري<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ڪوڊ &حالت)
{
    أرجع readAlias(
        alias, لڙيون, types, replacementIndexes, length,
#إذا U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](ثابت UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#وإلا
        [](ثابت حرف*) {},
        [](ثابت UChar*) { },
#جيڪڏهن_پورو
        حالت);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
فراغ
Alias_معلومات_Builder::readSubdivisionAlias(
        UResourceBundle* alias,
        Unique_Char_لڙيون* لڙيون,
        Local_يادگيري<ثابت حرف*>& types,
        Local_يادگيري<int32_t>& replacementIndexes,
        int32_t &length,
        UError_ڪوڊ &حالت)
{
    أرجع readAlias(
        alias, لڙيون, types, replacementIndexes, length,
#إذا U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#وإلا
        [](ثابت حرف*) {},
#جيڪڏهن_پورو
        [](ثابت UChar*) { },
        حالت);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
فراغ U_CALLCONV
Alias_معلومات::ڀريو_معلومات(UError_ڪوڊ &حالت)
{
#جيڪڏهن_وصف_ٿيل LOCALE_CANONICALIZATION_DEBUG
    UDate شروعات = uprv_getRawUTCtime();
#جيڪڏهن_پورو  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    Alias_معلومات_Builder builder;
    gSingleton = builder.build(حالت);
#جيڪڏهن_وصف_ٿيل LOCALE_CANONICALIZATION_DEBUG
    UDate پڇاڙي = uprv_getRawUTCtime();
    لکو("AliasData::loadData took total %f ms\n", پڇاڙي - شروعات);
#جيڪڏهن_پورو  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
Alias_معلومات*
Alias_معلومات_Builder::build(UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع خالي_اشارو; }

    Local_UResource_Bundle_اشارو metadata(
        ures_open_سڌو(خالي_اشارو, "metadata", &حالت));
    Local_UResource_Bundle_اشارو metadataAlias(
        ures_get_By_ڪنجي(metadata.getAlias(), "alias", خالي_اشارو, &حالت));
    Local_UResource_Bundle_اشارو languageAlias(
        ures_get_By_ڪنجي(metadataAlias.getAlias(), "language", خالي_اشارو, &حالت));
    Local_UResource_Bundle_اشارو scriptAlias(
        ures_get_By_ڪنجي(metadataAlias.getAlias(), "script", خالي_اشارو, &حالت));
    Local_UResource_Bundle_اشارو territoryAlias(
        ures_get_By_ڪنجي(metadataAlias.getAlias(), "territory", خالي_اشارو, &حالت));
    Local_UResource_Bundle_اشارو variantAlias(
        ures_get_By_ڪنجي(metadataAlias.getAlias(), "variant", خالي_اشارو, &حالت));
    Local_UResource_Bundle_اشارو subdivisionAlias(
        ures_get_By_ڪنجي(metadataAlias.getAlias(), "subdivision", خالي_اشارو, &حالت));

    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }
    int32_t languages_ڊيگهائي = 0, script_ڊيگهائي = 0, territory_ڊيگهائي = 0,
            variant_ڊيگهائي = 0, subdivision_ڊيگهائي = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    Unique_Char_لڙيون لڙيون(حالت);
    Local_يادگيري<ثابت حرف*> languageTypes;
    Local_يادگيري<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &لڙيون,
                      languageTypes,
                      languageReplacementIndexes,
                      languages_ڊيگهائي,
                      حالت);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    Local_يادگيري<ثابت حرف*> scriptTypes;
    Local_يادگيري<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &لڙيون,
                    scriptTypes,
                    scriptReplacementIndexes,
                    script_ڊيگهائي,
                    حالت);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    Local_يادگيري<ثابت حرف*> territoryTypes;
    Local_يادگيري<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &لڙيون,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territory_ڊيگهائي, حالت);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    Local_يادگيري<ثابت حرف*> variantTypes;
    Local_يادگيري<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &لڙيون,
                     variantTypes,
                     variantReplacementIndexes,
                     variant_ڊيگهائي, حالت);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    Local_يادگيري<ثابت حرف*> subdivisionTypes;
    Local_يادگيري<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &لڙيون,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivision_ڊيگهائي, حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }

    // We can only use strings after freeze it.
    لڙيون.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < languages_ڊيگهائي; i++) {
        languageMap.put(languageTypes[i],
                        لڙيون.get(languageReplacementIndexes[i]),
                        حالت);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < script_ڊيگهائي; i++) {
        scriptMap.put(scriptTypes[i],
                      لڙيون.get(scriptReplacementIndexes[i]),
                      حالت);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < territory_ڊيگهائي; i++) {
        territoryMap.put(territoryTypes[i],
                         لڙيون.get(territoryReplacementIndexes[i]),
                         حالت);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < variant_ڊيگهائي; i++) {
        variantMap.put(variantTypes[i],
                       لڙيون.get(variantReplacementIndexes[i]),
                       حالت);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < subdivision_ڊيگهائي; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       لڙيون.get(subdivisionReplacementIndexes[i]),
                       حالت);
    }

    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }

    // copy hashtables
    خودڪار *معلومات = جديد Alias_معلومات(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        لڙيون.orphan_Char_لڙيون());

    إذا (معلومات == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
    }
    أرجع معلومات;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
صنف AliasReplacer {
عام:
    AliasReplacer(UError_ڪوڊ& حالت) :
            language(خالي_اشارو), script(خالي_اشارو), region(خالي_اشارو),
            extensions(خالي_اشارو),
            // store value in variants only once
            variants(خالي_اشارو,
                     ([](UElement e1, UElement e2) -> UBool {
                       أرجع 0==uprv_strcmp((ثابت حرف*)e1.اشارو,
                                             (ثابت حرف*)e2.اشارو);}),
                     حالت),
            معلومات(خالي_اشارو) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    منطقي replace(
        ثابت Locale& locale, CharString& out, UError_ڪوڊ& حالت);

خاص:
    ثابت حرف* language;
    ثابت حرف* script;
    ثابت حرف* region;
    ثابت حرف* extensions;
    UVector variants;

    ثابت Alias_معلومات* معلومات;

    مضمن منطقي not_خالي_جايون(ثابت حرف* str) {
        أرجع str && str[0] != NULL_CHAR;
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
    مضمن ثابت حرف* deleteOrReplace(ثابت حرف* input U_LIFETIME_BOUND,
                                       ثابت حرف* type,
                                       ثابت حرف* replacement U_LIFETIME_BOUND) {
        أرجع not_خالي_جايون(replacement) ?
            ((input == خالي_اشارو) ?  replacement : input) :
            ((type == خالي_اشارو) ? input  : خالي_اشارو);
    }

    مضمن منطقي same(ثابت حرف* a, ثابت حرف* b) {
        إذا (a == خالي_اشارو && b == خالي_اشارو) {
            أرجع سچ;
        }
        إذا ((a == خالي_اشارو && b != خالي_اشارو) ||
            (a != خالي_اشارو && b == خالي_اشارو)) {
          أرجع خطأ;
        }
        أرجع uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& output_منزل_String(CharString& out U_LIFETIME_BOUND, UError_ڪوڊ& حالت);

    // Generate the lookup key.
    CharString& generate_ڪنجي(ثابت حرف* language, ثابت حرف* region,
                            ثابت حرف* variant, CharString& out U_LIFETIME_BOUND,
                            UError_ڪوڊ& حالت);

    فراغ parseLanguageReplacement(ثابت حرف* replacement,
                                  ثابت حرف*& replaceLanguage,
                                  ثابت حرف*& replaceScript,
                                  ثابت حرف*& replaceRegion,
                                  ثابت حرف*& replaceVariant,
                                  ثابت حرف*& replaceExtensions,
                                  UVector& منزل_Be_Freed,
                                  UError_ڪوڊ& حالت);

    // Replace by using languageAlias.
    منطقي replaceLanguage(منطقي checkLanguage, منطقي checkRegion,
                         منطقي checkVariants, UVector& منزل_Be_Freed,
                         UError_ڪوڊ& حالت);

    // Replace by using territoryAlias.
    منطقي replaceTerritory(UVector& منزل_Be_Freed, UError_ڪوڊ& حالت);

    // Replace by using scriptAlias.
    منطقي replaceScript(UError_ڪوڊ& حالت);

    // Replace by using variantAlias.
    منطقي replaceVariant(UError_ڪوڊ& حالت);

    // Replace by using subdivisionAlias.
    منطقي replaceSubdivision(StringPiece subdivision,
                            CharString& output, UError_ڪوڊ& حالت);

    // Replace transformed extensions.
    منطقي replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UError_ڪوڊ& حالت);
};

CharString&
AliasReplacer::generate_ڪنجي(
        ثابت حرف* language, ثابت حرف* region, ثابت حرف* variant,
        CharString& out U_LIFETIME_BOUND, UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع out; }
    out.append(language, حالت);
    إذا (not_خالي_جايون(region)) {
        out.append(SEP_CHAR, حالت)
            .append(region, حالت);
    }
    إذا (not_خالي_جايون(variant)) {
       out.append(SEP_CHAR, حالت)
           .append(variant, حالت);
    }
    أرجع out;
}

فراغ
AliasReplacer::parseLanguageReplacement(
    ثابت حرف* replacement,
    ثابت حرف*& replacedLanguage,
    ثابت حرف*& replacedScript,
    ثابت حرف*& replacedRegion,
    ثابت حرف*& replacedVariant,
    ثابت حرف*& replacedExtensions,
    UVector& منزل_Be_Freed,
    UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = خالي_اشارو;
    إذا (uprv_strchr(replacement, '_') == خالي_اشارو) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        أرجع;
    }
    // We have multiple field so we have to allocate and parse
    خودڪار str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), حالت, حالت);
    حرف* معلومات = str->معلومات();
    منزل_Be_Freed.adoptElement(str.release(), حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    replacedLanguage = (ثابت حرف*) معلومات;
    حرف* پڇاڙي_جو_ميدان = uprv_strchr(معلومات, '_');
    *پڇاڙي_جو_ميدان = '\0'; // null terminiate it.
    پڇاڙي_جو_ميدان++;
    ثابت حرف* شروعات = پڇاڙي_جو_ميدان;
    پڇاڙي_جو_ميدان = const_cast<حرف*>(uprv_strchr(شروعات, '_'));
    size_t len = 0;
    إذا (پڇاڙي_جو_ميدان == خالي_اشارو) {
        len = uprv_strlen(شروعات);
    } وإلا {
        len = پڇاڙي_جو_ميدان - شروعات;
        *پڇاڙي_جو_ميدان = '\0'; // null terminiate it.
    }
    إذا (len == 4 && uprv_isASCIILetter(*شروعات)) {
        // Got a script
        replacedScript = شروعات;
        إذا (پڇاڙي_جو_ميدان == خالي_اشارو) {
            أرجع;
        }
        شروعات = پڇاڙي_جو_ميدان++;
        پڇاڙي_جو_ميدان = const_cast<حرف*>(uprv_strchr(شروعات, '_'));
        إذا (پڇاڙي_جو_ميدان == خالي_اشارو) {
            len = uprv_strlen(شروعات);
        } وإلا {
            len = پڇاڙي_جو_ميدان - شروعات;
            *پڇاڙي_جو_ميدان = '\0'; // null terminiate it.
        }
    }
    إذا (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = شروعات;
        إذا (پڇاڙي_جو_ميدان == خالي_اشارو) {
            أرجع;
        }
        شروعات = پڇاڙي_جو_ميدان++;
        پڇاڙي_جو_ميدان = const_cast<حرف*>(uprv_strchr(شروعات, '_'));
        إذا (پڇاڙي_جو_ميدان == خالي_اشارو) {
            len = uprv_strlen(شروعات);
        } وإلا {
            len = پڇاڙي_جو_ميدان - شروعات;
            *پڇاڙي_جو_ميدان = '\0'; // null terminiate it.
        }
    }
    إذا (len >= 4) {
        // Got a variant
        replacedVariant = شروعات;
        إذا (پڇاڙي_جو_ميدان == خالي_اشارو) {
            أرجع;
        }
        شروعات = پڇاڙي_جو_ميدان++;
    }
    replacedExtensions = شروعات;
}

منطقي
AliasReplacer::replaceLanguage(
        منطقي checkLanguage, منطقي checkRegion,
        منطقي checkVariants, UVector& منزل_Be_Freed, UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (    (checkRegion && region == خالي_اشارو) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        أرجع خطأ;
    }
    int32_t variant_سائيز = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ثابت حرف* ڳولا_Language = checkLanguage ? language : "und";
    ثابت حرف* ڳولا_Region = checkRegion ? region : خالي_اشارو;
    ثابت حرف* ڳولا_Variant = خالي_اشارو;
    لكل (int32_t variant_اشاريو = 0;
            variant_اشاريو < variant_سائيز;
            variant_اشاريو++) {
        إذا (checkVariants) {
            U_ASSERT(variant_اشاريو < variant_سائيز);
            ڳولا_Variant = static_cast<ثابت حرف*>(variants.elementAt(variant_اشاريو));
        }

        إذا (ڳولا_Variant != خالي_اشارو && uprv_strlen(ڳولا_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            ڳولا_Variant = خالي_اشارو;
        }
        CharString type_ڪنجي;
        generate_ڪنجي(ڳولا_Language, ڳولا_Region, ڳولا_Variant, type_ڪنجي,
                    حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خطأ;
        }
        ثابت حرف *replacement = معلومات->languageMap().get(type_ڪنجي.معلومات());
        إذا (replacement == خالي_اشارو) {
            // Found no replacement data.
            استمر;
        }

        ثابت حرف* replacedLanguage = خالي_اشارو;
        ثابت حرف* replacedScript = خالي_اشارو;
        ثابت حرف* replacedRegion = خالي_اشارو;
        ثابت حرف* replacedVariant = خالي_اشارو;
        ثابت حرف* replacedExtensions = خالي_اشارو;
        parseLanguageReplacement(replacement,
                                 replacedLanguage,
                                 replacedScript,
                                 replacedRegion,
                                 replacedVariant,
                                 replacedExtensions,
                                 منزل_Be_Freed,
                                 حالت);
        replacedLanguage =
            (replacedLanguage != خالي_اشارو && uprv_strcmp(replacedLanguage, "und") == 0) ?
            language : replacedLanguage;
        replacedScript = deleteOrReplace(script, خالي_اشارو, replacedScript);
        replacedRegion = deleteOrReplace(region, ڳولا_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            ڳولا_Variant, ڳولا_Variant, replacedVariant);

        إذا (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(ڳولا_Variant, replacedVariant) &&
                replacedExtensions == خالي_اشارو) {
            // Replacement produce no changes.
            استمر;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        إذا (ڳولا_Variant != خالي_اشارو) {
            إذا (not_خالي_جايون(replacedVariant)) {
                variants.سيٽ_Element_At((فراغ*)replacedVariant, variant_اشاريو);
            } وإلا {
                variants.removeElementAt(variant_اشاريو);
            }
        }
        إذا (replacedExtensions != خالي_اشارو) {
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
        أرجع سچ;
    }
    // Nothing changed by language alias data.
    أرجع خطأ;
}

منطقي
AliasReplacer::replaceTerritory(UVector& منزل_Be_Freed, UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (region == خالي_اشارو) {
        // No region to search.
        أرجع خطأ;
    }
    ثابت حرف *replacement = معلومات->territoryMap().get(region);
    إذا (replacement == خالي_اشارو) {
        // Found no replacement data for this region.
        أرجع خطأ;
    }
    ثابت حرف* replacedRegion = replacement;
    ثابت حرف* پهريون_Space = uprv_strchr(replacement, ' ');
    إذا (پهريون_Space != خالي_اشارو) {
        // If there are are more than one region in the replacement.
        // We need to check which one match based on the language.
        // Cannot use nullptr for language because that will construct
        // the default locale, in that case, use "und" to get the correct
        // locale.
        Locale l = LocaleBuilder()
            .سيٽ_Language(language == خالي_اشارو ? "und" : language)
            .سيٽ_Script(script)
            .build(حالت);
        l.جوڙيو_Likely_Subtags(حالت);
        ثابت حرف* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> شي;
        إذا (likelyRegion != خالي_اشارو && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ثابت حرف* مليو_اندريون_Replacement = uprv_strstr(replacement,
                                                         likelyRegion);
            إذا (مليو_اندريون_Replacement != خالي_اشارو) {
                // Assuming the case there are no three letter region code in
                // the replacement of territoryAlias
                U_ASSERT(مليو_اندريون_Replacement == replacement ||
                         *(مليو_اندريون_Replacement-1) == ' ');
                U_ASSERT(مليو_اندريون_Replacement[len] == ' ' ||
                         مليو_اندريون_Replacement[len] == '\0');
                شي = prv::make_unique<CharString>(
                    مليو_اندريون_Replacement, static_cast<int32_t>(len), حالت, حالت);
            }
        }
        إذا (!شي && U_SUCCESS(حالت)) {
            شي = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(پهريون_Space - replacement), حالت, حالت);
        }
        إذا (U_FAILURE(حالت)) { أرجع خطأ; }
        replacedRegion = شي->معلومات();
        منزل_Be_Freed.adoptElement(شي.release(), حالت);
        إذا (U_FAILURE(حالت)) { أرجع خطأ; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    أرجع سچ;
}

منطقي
AliasReplacer::replaceScript(UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (script == خالي_اشارو) {
        // No script to search.
        أرجع خطأ;
    }
    ثابت حرف *replacement = معلومات->scriptMap().get(script);
    إذا (replacement == خالي_اشارو) {
        // Found no replacement data for this script.
        أرجع خطأ;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    أرجع سچ;
}

منطقي
AliasReplacer::replaceVariant(UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    // Since we may have more than one variant, we need to loop through them.
    لكل (int32_t i = 0; i < variants.size(); i++) {
        ثابت حرف* variant = static_cast<ثابت حرف*>(variants.elementAt(i));
        ثابت حرف *replacement = معلومات->variantMap().get(variant);
        إذا (replacement == خالي_اشارو) {
            // Found no replacement data for this variant.
            استمر;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        إذا (!same(variant, replacement)) {
            variants.سيٽ_Element_At((فراغ*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            إذا (uprv_strcmp(variant, "heploc") == 0) {
                لكل (int32_t j = 0; j < variants.size(); j++) {
                     إذا (uprv_strcmp((ثابت حرف*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            أرجع سچ;
        }
    }
    أرجع خطأ;
}

منطقي
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    ثابت حرف *replacement = معلومات->subdivisionMap().get(subdivision.معلومات());
    إذا (replacement != خالي_اشارو) {
        ثابت حرف* پهريون_Space = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (پهريون_Space != خالي_اشارو) ?
            (پهريون_Space - replacement) : uprv_strlen(replacement);
        إذا (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), حالت);
            إذا (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, حالت);
            }
        }
        أرجع سچ;
    }
    أرجع خطأ;
}

منطقي
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UError_ڪوڊ& حالت)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    int32_t len = transformedExtensions.length();
    ثابت حرف* str = transformedExtensions.معلومات();
    ثابت حرف* tkey = ultag_get_TKey_شروعات(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == خالي_اشارو) ? len : static_cast<int32_t>((tkey - str - 1)));
    إذا (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .سيٽ_Language_ٽيگ(StringPiece(str, tlangLen))
            .build(حالت);
        tlang.canonicalize(حالت);
        output = tlang.منزل_Language_ٽيگ<CharString>(حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خطأ;
        }
        T_CString_منزل_هيٺاهون_Case(output.معلومات());
    }
    إذا (tkey != خالي_اشارو) {
        // We need to sort the tfields by tkey
        UVector tfields(حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خطأ;
        }
        افعل {
            ثابت حرف* tvalue = uprv_strchr(tkey, '-');
            إذا (tvalue == خالي_اشارو) {
                حالت = U_ILLEGAL_دليل_ERROR;
                أرجع خطأ;
            }
            ثابت حرف* ايندڙ_TKey = ultag_get_TKey_شروعات(tvalue);
            إذا (ايندڙ_TKey != خالي_اشارو) {
                *const_cast<حرف*>(ايندڙ_TKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.وجهو_Element_At((فراغ*)tkey, tfields.size(), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع خطأ;
            }
            tkey = ايندڙ_TKey;
        } طالما (tkey != خالي_اشارو);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            أرجع uprv_strcmp((ثابت حرف*)e1.اشارو, (ثابت حرف*)e2.اشارو);
        }, حالت);
        لكل (int32_t i = 0; i < tfields.size(); i++) {
             إذا (output.length() > 0) {
                 output.append('-', حالت);
             }
             ثابت حرف* tfield = static_cast<ثابت حرف*>(tfields.elementAt(i));
             ثابت حرف* tvalue = uprv_strchr(tfield, '-');
             إذا (tvalue == خالي_اشارو) {
                 حالت = U_ILLEGAL_دليل_ERROR;
                 أرجع خطأ;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<حرف*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, حالت).append('-', حالت);
             std::optional<std::string_view> bcpTValue = ulocimp_منزل_Bcp_Type(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, حالت);
        }
    }
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    أرجع سچ;
}

CharString&
AliasReplacer::output_منزل_String(
    CharString& out U_LIFETIME_BOUND, UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع out; }
    out.append(language, حالت);
    إذا (not_خالي_جايون(script)) {
        out.append(SEP_CHAR, حالت)
            .append(script, حالت);
    }
    إذا (not_خالي_جايون(region)) {
        out.append(SEP_CHAR, حالت)
            .append(region, حالت);
    }
    إذا (variants.size() > 0) {
        إذا (!not_خالي_جايون(script) && !not_خالي_جايون(region)) {
          out.append(SEP_CHAR, حالت);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            أرجع uprv_strcmp((ثابت حرف*)e1.اشارو, (ثابت حرف*)e2.اشارو);
        }, حالت);
        int32_t variants_شروعات = out.length();
        لكل (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, حالت)
                 .append(static_cast<ثابت حرف*>(variants.elementAt(i)),
                         حالت);
        }
        T_CString_منزل_مٿانهون_Case(out.معلومات() + variants_شروعات);
    }
    إذا (not_خالي_جايون(extensions)) {
        CharString tmp("und_", حالت);
        tmp.append(extensions, حالت);
        Locale tmpLocale(tmp.معلومات());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, حالت);
    }
    أرجع out;
}

منطقي
AliasReplacer::replace(ثابت Locale& locale, CharString& out, UError_ڪوڊ& حالت)
{
    معلومات = Alias_معلومات::singleton(حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    U_ASSERT(معلومات != خالي_اشارو);
    out.clear();
    language = locale.getLanguage();
    إذا (!not_خالي_جايون(language)) {
        language = خالي_اشارو;
    }
    script = locale.getScript();
    إذا (!not_خالي_جايون(script)) {
        script = خالي_اشارو;
    }
    region = locale.getCountry();
    إذا (!not_خالي_جايون(region)) {
        region = خالي_اشارو;
    }
    ثابت حرف* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, حالت);
    إذا (!variantsBuff.is_خالي_جايون()) {
        إذا (U_FAILURE(حالت)) { أرجع خطأ; }
        حرف* شروعات = variantsBuff.معلومات();
        T_CString_منزل_هيٺاهون_Case(شروعات);
        حرف* پڇاڙي;
        طالما ((پڇاڙي = uprv_strchr(شروعات, SEP_CHAR)) != خالي_اشارو &&
               U_SUCCESS(حالت)) {
            *پڇاڙي = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            إذا (*شروعات && !variants.شامل_آهي(شروعات)) {
                variants.جوڙيو_Element(شروعات, حالت);
            }
            شروعات = پڇاڙي + 1;
        }
        // do not add "" or duplicate data to variants
        إذا (*شروعات && !variants.شامل_آهي(شروعات)) {
            variants.جوڙيو_Element(شروعات, حالت);
        }
    }
    إذا (U_FAILURE(حالت)) { أرجع خطأ; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        أرجع uprv_strcmp((ثابت حرف*)e1.اشارو, (ثابت حرف*)e2.اشارو);
    }, حالت);

    // A changed count to assert when loop too many times.
    int32_t بدليو = 0;
    ثابت int32_t max_بدليو = 4 + variants.size();
    (فراغ)max_بدليو; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector لڙيون_منزل_Be_Freed([](فراغ *obj) { ميٽيو static_cast<CharString*>(obj); },
                             خالي_اشارو, 10, حالت);
    طالما (U_SUCCESS(حالت)) {
        // Something wrong with the data cause looping here more than maxChanged
        // times already.
        U_ASSERT(بدليو <= max_بدليو);
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
        إذا (    replaceLanguage(سچ, سچ,  سچ,  لڙيون_منزل_Be_Freed, حالت) ||
                replaceLanguage(سچ, سچ,  خطأ, لڙيون_منزل_Be_Freed, حالت) ||
                replaceLanguage(سچ, خطأ, سچ,  لڙيون_منزل_Be_Freed, حالت) ||
                replaceLanguage(سچ, خطأ, خطأ, لڙيون_منزل_Be_Freed, حالت) ||
                replaceLanguage(خطأ,خطأ, سچ,  لڙيون_منزل_Be_Freed, حالت) ||
                replaceTerritory(لڙيون_منزل_Be_Freed, حالت) ||
                replaceScript(حالت) ||
                replaceVariant(حالت)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            بدليو++;
            استمر;
        }
        // Nothing changed. Break out.
        اكسر;
    }  // while(1)

    إذا (U_FAILURE(حالت)) { أرجع خطأ; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ثابت حرف* extensionsStr = locale_get_Keywords_شروعات(locale.getName());
    إذا (بدليو == 0 && variants.size() <= 1 && extensionsStr == خالي_اشارو) {
        أرجع خطأ;
    }
    output_منزل_String(out, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (extensionsStr != خالي_اشارو) {
        بدليو = 0;
        Locale عارضي(locale);
        Local_اشارو<icu::StringEnumeration> iter(locale.createKeywords(حالت));
        إذا (U_SUCCESS(حالت) && !iter.isNull()) {
            ثابت حرف* ڪنجي;
            طالما ((ڪنجي = iter->next(خالي_اشارو, حالت)) != خالي_اشارو) {
                إذا (uprv_strcmp("sd", ڪنجي) == 0 || uprv_strcmp("rg", ڪنجي) == 0 ||
                        uprv_strcmp("t", ڪنجي) == 0) {
                    خودڪار value = locale.get_Keyword_قدر<CharString>(ڪنجي, حالت);
                    إذا (U_FAILURE(حالت)) {
                        حالت = U_ZERO_ERROR;
                        استمر;
                    }
                    CharString replacement;
                    إذا (uprv_strlen(ڪنجي) == 2) {
                        إذا (replaceSubdivision(value.منزل_String_Piece(), replacement, حالت)) {
                            بدليو++;
                            عارضي.سيٽ_Keyword_قدر(ڪنجي, replacement.معلومات(), حالت);
                        }
                    } وإلا {
                        U_ASSERT(uprv_strcmp(ڪنجي, "t") == 0);
                        إذا (replaceTransformedExtensions(value, replacement, حالت)) {
                            بدليو++;
                            عارضي.سيٽ_Keyword_قدر(ڪنجي, replacement.معلومات(), حالت);
                        }
                    }
                    إذا (U_FAILURE(حالت)) {
                        أرجع خطأ;
                    }
                }
            }
        }
        إذا (بدليو != 0) {
            extensionsStr = locale_get_Keywords_شروعات(عارضي.getName());
        }
        out.append(extensionsStr, حالت);
    }
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    // If the tag is not changed, return.
    إذا (uprv_strcmp(out.معلومات(), locale.getName()) == 0) {
        out.clear();
        أرجع خطأ;
    }
    أرجع سچ;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
منطقي
canonicalizeLocale(ثابت Locale& locale, CharString& out, UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع خطأ; }
    AliasReplacer replacer(حالت);
    أرجع replacer.replace(locale, out, حالت);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
منطقي
is_معلوم_Canonicalized_Locale(ثابت حرف* locale, UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع خطأ; }

    إذا (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        أرجع سچ;
    }

    // common well-known Canonicalized.
    umtx_initOnce(g_معلوم_Canonicalized_Init_Once,
                  &ڀريو_معلوم_Canonicalized, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    U_ASSERT(g_معلوم_Canonicalized != خالي_اشارو);
    أرجع uhash_geti(g_معلوم_Canonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_پڇاڙي

// Function for testing.
U_EXPORT ثابت حرف* ثابت*
ulocimp_get_معلوم_Canonicalized_Locale_For_Test(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(معلوم_CANONICALIZED);
    أرجع معلوم_CANONICALIZED;
}

// Function for testing.
U_EXPORT منطقي
ulocimp_isCanonicalizedLocaleForTest(ثابت حرف* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    CharString عارضي;
    أرجع !canonicalizeLocale(l, عارضي, حالت) && U_SUCCESS(حالت);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ثابت حرف* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    أرجع localeID == خالي_اشارو ? *هذا = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    افعل {
        حرف *separator;
        حرف *ميدان_2[5] = {خالي_اشارو};
        int32_t ميدان_Len[5] = {0};
        int32_t ميدان_Idx;
        int32_t variant_ميدان;
        int32_t length;
        UError_ڪوڊ err;

        ثابت خودڪار parse = [canonicalize](std::string_view localeID,
                                          حرف* name,
                                          int32_t name_صلاحيت,
                                          UError_ڪوڊ& حالت) {
            أرجع ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
                name, name_صلاحيت,
                [&](ByteSink& sink, UError_ڪوڊ& حالت) {
                    إذا (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, حالت);
                    } وإلا {
                        ulocimp_getName(localeID, sink, حالت);
                    }
                },
                حالت);
        };

        // "canonicalize" the locale ID to ICU/Java format
        حرف* ڀريل_Name = nest.بنياد_Name;
        err = U_ZERO_ERROR;
        length = parse(localeID, ڀريل_Name, حجم Nest::بنياد_Name, err);

        FixedString ڀريل_Name_بفر;
        إذا (err == U_بفر_حد_کان_مٿي_ERROR || length >= static_cast<int32_t>(حجم Nest::بنياد_Name)) {
            /*Go to heap for the fullName if necessary*/
            إذا (!ڀريل_Name_بفر.reserve(length + 1)) {
                اكسر; // error: out of memory
            }
            ڀريل_Name = ڀريل_Name_بفر.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, ڀريل_Name, length + 1, err);
        }
        إذا(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            اكسر;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = ميدان_2[0] = ڀريل_Name;
        ميدان_Idx = 1;
        حرف* at = uprv_strchr(ڀريل_Name, '@');
        طالما ((separator = uprv_strchr(ميدان_2[ميدان_Idx-1], SEP_CHAR)) != خالي_اشارو &&
               ميدان_Idx < UPRV_LENGTHOF(ميدان_2)-1 &&
               (at == خالي_اشارو || separator < at)) {
            ميدان_2[ميدان_Idx] = separator + 1;
            ميدان_Len[ميدان_Idx - 1] = static_cast<int32_t>(separator - ميدان_2[ميدان_Idx - 1]);
            ميدان_Idx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(ميدان_2[ميدان_Idx-1], '@');
        حرف* sep2 = uprv_strchr(ميدان_2[ميدان_Idx-1], '.');
        إذا (separator!=خالي_اشارو || sep2!=خالي_اشارو) {
            إذا (separator==خالي_اشارو || (sep2!=خالي_اشارو && separator > sep2)) {
                separator = sep2;
            }
            ميدان_Len[ميدان_Idx - 1] = static_cast<int32_t>(separator - ميدان_2[ميدان_Idx - 1]);
        } وإلا {
            ميدان_Len[ميدان_Idx - 1] = length - static_cast<int32_t>(ميدان_2[ميدان_Idx - 1] - ڀريل_Name);
        }
        منطقي hasKeywords = at != خالي_اشارو && uprv_strchr(at + 1, '=') != خالي_اشارو;

        إذا (ميدان_Len[0] >= ULOC_LANG_صلاحيت)
        {
            اكسر; // error: the language field is too long
        }

        variant_ميدان = 1; /* Usually the 2nd one, except when a script or country is also used. */
        إذا (ميدان_Len[0] > 0) {
            /* We have a language */
            language = {ڀريل_Name, static_cast<std::string_view::size_type>(ميدان_Len[0])};
        }
        إذا (ميدان_Len[1] == 4 && uprv_isASCIILetter(ميدان_2[1][0]) &&
                uprv_isASCIILetter(ميدان_2[1][1]) && uprv_isASCIILetter(ميدان_2[1][2]) &&
                uprv_isASCIILetter(ميدان_2[1][3])) {
            /* We have at least a script */
            script = {ميدان_2[1], static_cast<std::string_view::size_type>(ميدان_Len[1])};
            variant_ميدان++;
        }

        إذا (ميدان_Len[variant_ميدان] == 2 || ميدان_Len[variant_ميدان] == 3) {
            /* We have a country */
            region = {ميدان_2[variant_ميدان], static_cast<std::string_view::size_type>(ميدان_Len[variant_ميدان])};
            variant_ميدان++;
        } وإلا إذا (ميدان_Len[variant_ميدان] == 0) {
            variant_ميدان++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        إذا (ميدان_Len[variant_ميدان] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(ميدان_2[variant_ميدان] - ڀريل_Name);
        } وإلا إذا (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - ڀريل_Name);
        }

        إذا (!hasKeywords && Nest::سمائي(length, language, script, region)) {
            U_ASSERT(ڀريل_Name == nest.بنياد_Name);
            U_ASSERT(ڀريل_Name_بفر.is_خالي_جايون());
            nest.init(language, script, region, variantBegin);
        } وإلا {
            إذا (ڀريل_Name == nest.بنياد_Name) {
                U_ASSERT(ڀريل_Name_بفر.is_خالي_جايون());
                ڀريل_Name_بفر = {ڀريل_Name, static_cast<std::string_view::size_type>(length)};
                إذا (ڀريل_Name_بفر.is_خالي_جايون()) {
                    اكسر; // error: out of memory
                }
                إذا (!language.empty()) {
                    language = {ڀريل_Name_بفر.معلومات(), language.size()};
                }
                إذا (!script.empty()) {
                    script = {ڀريل_Name_بفر.معلومات() + (script.معلومات() - ڀريل_Name), script.size()};
                }
                إذا (!region.empty()) {
                    region = {ڀريل_Name_بفر.معلومات() + (region.معلومات() - ڀريل_Name), region.size()};
                }
            }
            ڍير_2& ڍير_3 = payload.emplace<ڍير_2>(language, script, region, variantBegin);
            إذا (isBogus()) {
                اكسر; // error: out of memory
            }
            U_ASSERT(!ڀريل_Name_بفر.is_خالي_جايون());
            ڍير_3.ptr->ڀريل_Name = std::move(ڀريل_Name_بفر);
            إذا (hasKeywords) {
                إذا (std::string_view::size_type بنياد_Name_ڊيگهائي = at - ڀريل_Name; بنياد_Name_ڊيگهائي > 0) {
                    ڍير_3.ptr->بنياد_Name = {ڍير_3.ptr->ڀريل_Name.معلومات(), بنياد_Name_ڊيگهائي};
                    إذا (ڍير_3.ptr->بنياد_Name.is_خالي_جايون()) {
                        اكسر; // error: out of memory
                    }
                }
            }
        }

        إذا (canonicalize) {
            إذا (!is_معلوم_Canonicalized_Locale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                إذا (canonicalizeLocale(*هذا, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.معلومات(), خطأ);
                }
                إذا (U_FAILURE(err)) {
                    اكسر;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        أرجع *هذا;
    } طالما(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    سيٽ_منزل_Bogus();

    أرجع *هذا;
}

int32_t
Locale::hash_ڪوڊ() ثابت
{
    أرجع ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

فراغ
Locale::سيٽ_منزل_Bogus() {
    /* Free our current storage */
    payload.سيٽ_منزل_Bogus();
}

ثابت Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex تالو(&gDefaultLocaleMutex);
        إذا (gDefaultLocale != خالي_اشارو) {
            أرجع *gDefaultLocale;
        }
    }
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    أرجع *locale_سيٽ_default_اندروني(خالي_اشارو, حالت);
}



فراغ U_EXPORT2
Locale::سيٽ_Default( ثابت   Locale&     newLocale,
                            UError_ڪوڊ&  حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ثابت حرف *localeID = newLocale.getName();
    locale_سيٽ_default_اندروني(localeID, حالت);
}

فراغ
Locale::جوڙيو_Likely_Subtags(UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    CharString maximizedLocaleID = ulocimp_جوڙيو_Likely_Subtags(getName(), حالت);

    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_يادگيري_ورهاست_ERROR) {
            سيٽ_منزل_Bogus();
        }
        أرجع;
    }

    init(maximizedLocaleID.معلومات(), /*canonicalize=*/خطأ);
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دليل_ERROR;
    }
}

فراغ
Locale::minimizeSubtags(UError_ڪوڊ& حالت) {
    Locale::minimizeSubtags(خطأ, حالت);
}
فراغ
Locale::minimizeSubtags(منطقي favorScript, UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, حالت);

    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_يادگيري_ورهاست_ERROR) {
            سيٽ_منزل_Bogus();
        }
        أرجع;
    }

    init(minimizedLocaleID.معلومات(), /*canonicalize=*/خطأ);
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دليل_ERROR;
    }
}

فراغ
Locale::canonicalize(UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    CharString uncanonicalized(getName(), حالت);
    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_يادگيري_ورهاست_ERROR) {
            سيٽ_منزل_Bogus();
        }
        أرجع;
    }
    init(uncanonicalized.معلومات(), /*canonicalize=*/سچ);
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دليل_ERROR;
    }
}

Locale U_EXPORT2
Locale::for_Language_ٽيگ(StringPiece ٽيگ, UError_ڪوڊ& حالت)
{
    Locale نتيجو(Locale::eBOGUS);

    إذا (U_FAILURE(حالت)) {
        أرجع نتيجو;
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

    int32_t parsed_ڊيگهائي;
    CharString localeID = ulocimp_for_Language_ٽيگ(
            ٽيگ.معلومات(),
            ٽيگ.length(),
            &parsed_ڊيگهائي,
            حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع نتيجو;
    }

    إذا (parsed_ڊيگهائي != ٽيگ.size()) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع نتيجو;
    }

    نتيجو.init(localeID.معلومات(), /*canonicalize=*/خطأ);
    إذا (نتيجو.isBogus()) {
        حالت = U_ILLEGAL_دليل_ERROR;
    }
    أرجع نتيجو;
}

فراغ
Locale::منزل_Language_ٽيگ(ByteSink& sink, UError_ڪوڊ& حالت) ثابت
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    إذا (isBogus()) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    ulocimp_منزل_Language_ٽيگ(getName(), sink, /*strict=*/خطأ, حالت);
}

Locale U_EXPORT2
Locale::createFromName (ثابت حرف *name)
{
    إذا (name) {
        Locale l("");
        l.init(name, خطأ);
        أرجع l;
    }
    وإلا {
        أرجع getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, خطأ);
    أرجع loc;
}

Locale U_EXPORT2
Locale::createCanonical(ثابت حرف* name) {
    Locale loc("");
    loc.init(name, سچ);
    أرجع loc;
}

ثابت حرف *
Locale::getISO3Language() ثابت
{
    أرجع uloc_getISO3Language(getName());
}


ثابت حرف *
Locale::getISO3Country() ثابت
{
    أرجع uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() ثابت
{
    أرجع uloc_getLCID(getName());
}

ثابت حرف* ثابت* U_EXPORT2 Locale::getISOCountries()
{
    أرجع uloc_getISOCountries();
}

ثابت حرف* ثابت* U_EXPORT2 Locale::getISOLanguages()
{
    أرجع uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
فراغ Locale::سيٽ_From_POSIXID(ثابت حرف *posixID)
{
    init(posixID, سچ);
}

ثابت Locale & U_EXPORT2
Locale::get_پاڙ()
{
    أرجع getLocale(e_پاڙ);
}

ثابت Locale & U_EXPORT2
Locale::getEnglish()
{
    أرجع getLocale(eENGLISH);
}

ثابت Locale & U_EXPORT2
Locale::getFrench()
{
    أرجع getLocale(eFRENCH);
}

ثابت Locale & U_EXPORT2
Locale::getGerman()
{
    أرجع getLocale(eGERMAN);
}

ثابت Locale & U_EXPORT2
Locale::getItalian()
{
    أرجع getLocale(eITALIAN);
}

ثابت Locale & U_EXPORT2
Locale::getJapanese()
{
    أرجع getLocale(eJAPANESE);
}

ثابت Locale & U_EXPORT2
Locale::getKorean()
{
    أرجع getLocale(eKOREAN);
}

ثابت Locale & U_EXPORT2
Locale::getChinese()
{
    أرجع getLocale(eCHINESE);
}

ثابت Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    أرجع getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    أرجع getLocale(eTAIWAN);
}


ثابت Locale & U_EXPORT2
Locale::getFrance()
{
    أرجع getLocale(eFRANCE);
}

ثابت Locale & U_EXPORT2
Locale::getGermany()
{
    أرجع getLocale(eGERMANY);
}

ثابت Locale & U_EXPORT2
Locale::getItaly()
{
    أرجع getLocale(eITALY);
}

ثابت Locale & U_EXPORT2
Locale::getJapan()
{
    أرجع getLocale(eJAPAN);
}

ثابت Locale & U_EXPORT2
Locale::getKorea()
{
    أرجع getLocale(eKOREA);
}

ثابت Locale & U_EXPORT2
Locale::getChina()
{
    أرجع getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getPRC()
{
    أرجع getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getTaiwan()
{
    أرجع getLocale(eTAIWAN);
}

ثابت Locale & U_EXPORT2
Locale::getUK()
{
    أرجع getLocale(eUK);
}

ثابت Locale & U_EXPORT2
Locale::getUS()
{
    أرجع getLocale(eUS);
}

ثابت Locale & U_EXPORT2
Locale::getCanada()
{
    أرجع getLocale(eCANADA);
}

ثابت Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    أرجع getLocale(eCANADA_FRENCH);
}

ثابت Locale &
Locale::getLocale(صحيح locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    إذا (localeCache == خالي_اشارو) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    أرجع localeCache[locid]; /*operating on nullptr*/
}

/*
This function is defined this way in order to get around static
initialization and static destruction.
 */
Locale *
Locale::getLocaleCache()
{
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    umtx_initOnce(gLocaleCacheInitOnce, locale_init, حالت);
    أرجع gLocaleCache;
}

صنف KeywordEnumeration : عام StringEnumeration {
محمي:
    FixedString keywords;
خاص:
    int32_t length;
    ثابت حرف *current;
    ساكن ثابت حرف fgClassID;

عام:
    ساكن UClassID U_EXPORT2 getStaticClassID() { أرجع (UClassID)&fgClassID; }
    مجازي UClassID getDynamicClassID() ثابت override { أرجع getStaticClassID(); }
عام:
    KeywordEnumeration(ثابت حرف *ڪنجيون, int32_t keywordLen, int32_t current_اشاريو, UError_ڪوڊ &حالت)
        : keywords(), length(keywordLen), current(خالي_اشارو) {
        إذا(U_SUCCESS(حالت) && keywordLen != 0) {
            إذا(ڪنجيون == خالي_اشارو || keywordLen < 0) {
                حالت = U_ILLEGAL_دليل_ERROR;
            } وإلا {
                keywords = {ڪنجيون, static_cast<std::string_view::size_type>(length)};
                إذا (keywords.is_خالي_جايون()) {
                    حالت = U_يادگيري_ورهاست_ERROR;
                } وإلا {
                    current = keywords.معلومات() + current_اشاريو;
                }
            }
        }
    }

    مجازي ~KeywordEnumeration();

    مجازي StringEnumeration * clone() ثابت override
    {
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        أرجع جديد KeywordEnumeration(
                keywords.معلومات(), length,
                static_cast<int32_t>(current - keywords.معلومات()), حالت);
    }

    مجازي int32_t count(UError_ڪوڊ& حالت) ثابت override {
        إذا (U_FAILURE(حالت)) { أرجع 0; }
        ثابت حرف *kw = keywords.معلومات();
        int32_t نتيجو = 0;
        طالما(*kw) {
            نتيجو++;
            kw += uprv_strlen(kw)+1;
        }
        أرجع نتيجو;
    }

    مجازي ثابت حرف* next(int32_t* نتيجو_ڊيگهائي, UError_ڪوڊ& حالت) U_LIFETIME_BOUND override {
        ثابت حرف* نتيجو;
        int32_t len;
        إذا(U_SUCCESS(حالت) && *current != 0) {
            نتيجو = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            إذا(نتيجو_ڊيگهائي != خالي_اشارو) {
                *نتيجو_ڊيگهائي = len;
            }
        } وإلا {
            إذا(نتيجو_ڊيگهائي != خالي_اشارو) {
                *نتيجو_ڊيگهائي = 0;
            }
            نتيجو = خالي_اشارو;
        }
        أرجع نتيجو;
    }

    مجازي ثابت UnicodeString* snext(UError_ڪوڊ& حالت) U_LIFETIME_BOUND override {
        إذا (U_FAILURE(حالت)) { أرجع خالي_اشارو; }
        int32_t نتيجو_ڊيگهائي = 0;
        ثابت حرف *s = next(&نتيجو_ڊيگهائي, حالت);
        أرجع سيٽ_Chars(s, نتيجو_ڊيگهائي, حالت);
    }

    مجازي فراغ reset(UError_ڪوڊ& حالت) override {
        إذا (U_FAILURE(حالت)) { أرجع; }
        current = keywords.معلومات();
    }
};

ثابت حرف KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = افتراضي;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
صنف UnicodeKeywordEnumeration : عام KeywordEnumeration {
عام:
    استعمال KeywordEnumeration::KeywordEnumeration;
    مجازي ~UnicodeKeywordEnumeration();

    مجازي ثابت حرف* next(int32_t* نتيجو_ڊيگهائي, UError_ڪوڊ& حالت) U_LIFETIME_BOUND override {
        ثابت حرف* legacy_ڪنجي = KeywordEnumeration::next(خالي_اشارو, حالت);
        طالما (U_SUCCESS(حالت) && legacy_ڪنجي != خالي_اشارو) {
            ثابت حرف* ڪنجي = uloc_منزل_Unicode_Locale_ڪنجي(legacy_ڪنجي);
            إذا (ڪنجي != خالي_اشارو) {
                إذا (نتيجو_ڊيگهائي != خالي_اشارو) {
                    *نتيجو_ڊيگهائي = static_cast<int32_t>(uprv_strlen(ڪنجي));
                }
                أرجع ڪنجي;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_ڪنجي = KeywordEnumeration::next(خالي_اشارو, حالت);
        }
        إذا (نتيجو_ڊيگهائي != خالي_اشارو) *نتيجو_ڊيگهائي = 0;
        أرجع خالي_اشارو;
    }
    مجازي int32_t count(UError_ڪوڊ& حالت) ثابت override {
        إذا (U_FAILURE(حالت)) { أرجع 0; }
        ثابت حرف *kw = keywords.معلومات();
        int32_t نتيجو = 0;
        طالما(*kw) {
            إذا (uloc_منزل_Unicode_Locale_ڪنجي(kw) != خالي_اشارو) {
                نتيجو++;
            }
            kw += uprv_strlen(kw)+1;
        }
        أرجع نتيجو;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = افتراضي;

StringEnumeration *
Locale::createKeywords(UError_ڪوڊ &حالت) ثابت
{
    StringEnumeration *نتيجو = خالي_اشارو;

    إذا (U_FAILURE(حالت)) {
        أرجع نتيجو;
    }

    ثابت حرف* variant_شروعات = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    إذا(variant_شروعات) {
        إذا(assignment > variant_شروعات) {
            CharString keywords = ulocimp_getKeywords(variant_شروعات + 1, '@', خطأ, حالت);
            إذا (U_SUCCESS(حالت) && !keywords.is_خالي_جايون()) {
                نتيجو = جديد KeywordEnumeration(keywords.معلومات(), keywords.length(), 0, حالت);
                إذا (!نتيجو) {
                    حالت = U_يادگيري_ورهاست_ERROR;
                }
            }
        } وإلا {
            حالت = U_INVALID_FORMAT_ERROR;
        }
    }
    أرجع نتيجو;
}

StringEnumeration *
Locale::createUnicodeKeywords(UError_ڪوڊ &حالت) ثابت
{
    StringEnumeration *نتيجو = خالي_اشارو;

    إذا (U_FAILURE(حالت)) {
        أرجع نتيجو;
    }

    ثابت حرف* variant_شروعات = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    إذا(variant_شروعات) {
        إذا(assignment > variant_شروعات) {
            CharString keywords = ulocimp_getKeywords(variant_شروعات + 1, '@', خطأ, حالت);
            إذا (U_SUCCESS(حالت) && !keywords.is_خالي_جايون()) {
                نتيجو = جديد UnicodeKeywordEnumeration(keywords.معلومات(), keywords.length(), 0, حالت);
                إذا (!نتيجو) {
                    حالت = U_يادگيري_ورهاست_ERROR;
                }
            }
        } وإلا {
            حالت = U_INVALID_FORMAT_ERROR;
        }
    }
    أرجع نتيجو;
}

int32_t
Locale::get_Keyword_قدر(ثابت حرف* keywordName, حرف *بفر, int32_t bufLen, UError_ڪوڊ &حالت) ثابت
{
    أرجع uloc_get_Keyword_قدر(getName(), keywordName, بفر, bufLen, &حالت);
}

فراغ
Locale::get_Keyword_قدر(StringPiece keywordName, ByteSink& sink, UError_ڪوڊ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    إذا (isBogus()) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    ulocimp_get_Keyword_قدر(getName(), keywordName, sink, حالت);
}

فراغ
Locale::get_Unicode_Keyword_قدر(StringPiece keywordName,
                               ByteSink& sink,
                               UError_ڪوڊ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    std::optional<std::string_view> legacy_ڪنجي = ulocimp_منزل_Legacy_ڪنجي_With_Fallback(keywordName);
    إذا (!legacy_ڪنجي.has_value()) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    خودڪار legacy_قدر = get_Keyword_قدر<CharString>(*legacy_ڪنجي, حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    std::optional<std::string_view> unicode_قدر =
        ulocimp_منزل_Bcp_Type_With_Fallback(keywordName, legacy_قدر.منزل_String_Piece());
    إذا (!unicode_قدر.has_value()) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    sink.Append(unicode_قدر->معلومات(), static_cast<int32_t>(unicode_قدر->size()));
}

فراغ
Locale::سيٽ_Keyword_قدر(StringPiece keywordName,
                        StringPiece keyword_قدر,
                        UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }
    إذا (keywordName.empty()) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    إذا (حالت == U_STRING_NOT_TERMINATED_WARNING) {
        حالت = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, حالت);
    ulocimp_سيٽ_Keyword_قدر(keywordName, keyword_قدر, localeID, حالت);
    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_يادگيري_ورهاست_ERROR) {
            سيٽ_منزل_Bogus();
        }
        أرجع;
    }

    ثابت حرف* at = locale_get_Keywords_شروعات(localeID.منزل_String_Piece());
    منطقي hasKeywords = at != خالي_اشارو && uprv_strchr(at + 1, '=') != خالي_اشارو;

    Nest* nest = payload.get<Nest>();
    إذا (!hasKeywords) {
        إذا (nest == خالي_اشارو) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            ڍير_2* ڍير_3 = payload.get<ڍير_2>();
            U_ASSERT(ڍير_3 != خالي_اشارو);
            إذا (Nest::سمائي(localeID.length(), ڍير_3->language, ڍير_3->script, ڍير_3->region)) {
                int32_t variantBegin = ڍير_3->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < حجم Nest::بنياد_Name);
                nest = &payload.emplace<Nest>(std::move(*ڍير_3), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->بنياد_Name, حجم Nest::بنياد_Name, حالت);
            } وإلا {
                ڍير_3->ptr->بنياد_Name.clear();
                ڍير_3->ptr->ڀريل_Name = localeID.منزل_String_Piece();
                إذا (ڍير_3->ptr->ڀريل_Name.is_خالي_جايون()) {
                    حالت = U_يادگيري_ورهاست_ERROR;
                    سيٽ_منزل_Bogus();
                    أرجع;
                }
            }
        }
    } وإلا {
        ڍير_2* ڍير_3 = خالي_اشارو;
        إذا (nest != خالي_اشارو) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            ڍير_3 = &payload.emplace<ڍير_2>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            إذا (isBogus()) {
                حالت = U_يادگيري_ورهاست_ERROR;
                أرجع;
            }
        } وإلا {
            ڍير_3 = payload.get<ڍير_2>();
        }
        U_ASSERT(ڍير_3 != خالي_اشارو);
        ڍير_3->ptr->ڀريل_Name = localeID.منزل_String_Piece();
        إذا (ڍير_3->ptr->ڀريل_Name.is_خالي_جايون()) {
            حالت = U_يادگيري_ورهاست_ERROR;
            سيٽ_منزل_Bogus();
            أرجع;
        }

        إذا (ڍير_3->ptr->بنياد_Name.is_خالي_جايون()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            إذا (std::string_view::size_type بنياد_Name_ڊيگهائي = at - localeID.معلومات(); بنياد_Name_ڊيگهائي > 0) {
                ڍير_3->ptr->بنياد_Name = {ڍير_3->ptr->ڀريل_Name.معلومات(), بنياد_Name_ڊيگهائي};
                إذا (ڍير_3->ptr->بنياد_Name.is_خالي_جايون()) {
                    حالت = U_يادگيري_ورهاست_ERROR;
                    سيٽ_منزل_Bogus();
                    أرجع;
                }
            }
        }
    }
}

فراغ
Locale::سيٽ_Unicode_Keyword_قدر(StringPiece keywordName,
                               StringPiece keyword_قدر,
                               UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    std::optional<std::string_view> legacy_ڪنجي = ulocimp_منزل_Legacy_ڪنجي_With_Fallback(keywordName);
    إذا (!legacy_ڪنجي.has_value()) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    std::string_view value;

    إذا (!keyword_قدر.empty()) {
        std::optional<std::string_view> legacy_قدر =
            ulocimp_منزل_Legacy_Type_With_Fallback(keywordName, keyword_قدر);
        إذا (!legacy_قدر.has_value()) {
            حالت = U_ILLEGAL_دليل_ERROR;
            أرجع;
        }
        value = *legacy_قدر;
    }

    سيٽ_Keyword_قدر(*legacy_ڪنجي, value, حالت);
}

ثابت حرف*
Locale::getCountry() ثابت U_LIFETIME_BOUND {
    أرجع get_ميدان<&Nest::getRegion, &ڍير_2::getRegion>();
}

ثابت حرف*
Locale::getLanguage() ثابت U_LIFETIME_BOUND {
    أرجع get_ميدان<&Nest::getLanguage, &ڍير_2::getLanguage>();
}

ثابت حرف*
Locale::getScript() ثابت U_LIFETIME_BOUND {
    أرجع get_ميدان<&Nest::getScript, &ڍير_2::getScript>();
}

ثابت حرف*
Locale::getVariant() ثابت U_LIFETIME_BOUND {
    أرجع get_ميدان<&Nest::getVariant, &ڍير_2::getVariant>();
}

ثابت حرف*
Locale::getName() ثابت U_LIFETIME_BOUND {
    أرجع get_ميدان<&Nest::get_بنياد_Name, &ڍير_2::get_ڀريل_Name>();
}

ثابت حرف*
Locale::get_بنياد_Name() ثابت U_LIFETIME_BOUND {
    أرجع get_ميدان<&Nest::get_بنياد_Name, &ڍير_2::get_بنياد_Name>();
}

سانچو <ثابت حرف* (Locale::Nest::*ثابت NEST)() ثابت,
          ثابت حرف* (Locale::ڍير_2::*ثابت ڍير_4)() ثابت>
ثابت حرف* Locale::get_ميدان() ثابت U_LIFETIME_BOUND {
    أرجع payload.گهمو([] { أرجع ""; },
                         [](ثابت Nest& nest) { أرجع (nest.*NEST)(); },
                         [](ثابت ڍير_2& ڍير_3) { أرجع (ڍير_3.*ڍير_4)(); });
}

Locale::Iterator::~Iterator() = افتراضي;

//eof
U_NAMESPACE_پڇاڙي
