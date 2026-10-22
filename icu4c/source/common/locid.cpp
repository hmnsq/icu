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

#إذا UPRV_HAS_FEATURE(حافظه_sanitizer)
#ضمن <sanitizer/msan_interface.h>
#د_که_پای

U_NAMESPACE_BEGIN

ساكن Locale   *gLocaleCache = تش_اشاره;
ساكن UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
ساكن UMutex gDefaultLocaleMutex;
ساكن UHashtable *gDefaultLocalesHashT = تش_اشاره;
ساكن Locale *gDefaultLocale = تش_اشاره;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#تعریف_کړه ULOC_STRING_حد 357913941

U_NAMESPACE_پای_ټکی

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
    e_ریښه,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

نطاق {

//
// Deleter function for Locales owned by the default Locale hash table/
//
فراغ U_CALLCONV
deleteLocale(فراغ *obj) {
    ړنګول static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    ړنګول [] gLocaleCache;
    gLocaleCache = تش_اشاره;
    gLocaleCacheInitOnce.reset();

    إذا (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = تش_اشاره;
    }
    gDefaultLocale = تش_اشاره;
    أرجع ریښتیا;
}

فراغ U_CALLCONV locale_init(UError_کوډ &حالت) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == تش_اشاره);
    gLocaleCache = جديد Locale[static_cast<صحيح>(eMAX_LOCALES)];
    إذا (gLocaleCache == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_ریښه]          = Locale("");
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

Locale *locale_سېټ_default_دننی(ثابت حرف *id, UError_کوډ& حالت) {
    // Synchronize this entire function.
    Mutex قلف(&gDefaultLocaleMutex);

    UBool canonicalize = خطأ;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    إذا (id == تش_اشاره) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = ریښتیا; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, حالت) : ulocimp_getName(id, حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع gDefaultLocale;
    }

    إذا (gDefaultLocalesHashT == تش_اشاره) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, تش_اشاره, &حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع gDefaultLocale;
        }
        uhash_سېټ_ارزښت_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.معلومات()));
    إذا (newDefault == تش_اشاره) {
        newDefault = جديد Locale(Locale::eBOGUS);
        إذا (newDefault == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
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

U_NAMESPACE_پای_ټکی

/* sfb 07/21/99 */
U_CFUNC فراغ
locale_سېټ_default(ثابت حرف *id)
{
    U_NAMESPACE_USE
    UError_کوډ حالت = U_ZERO_ERROR;
    locale_سېټ_default_دننی(id, حالت);
}
/*پای_ټکی*/

U_CFUNC ثابت حرف *
locale_get_default()
{
    U_NAMESPACE_USE
    أرجع Locale::getDefault().getName();
}

نطاق {

کينډۍ <خپلکاری ډګر, ډول_نوم T>
فراغ copy_منزل_Array(std::string_view sv, T* that) {
    خپلکاری& ډګر_2 = that->*ډګر;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(ډګر_2)>>;
    static_assert(capacity > 0);
    إذا (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(ډګر_2, sv.معلومات(), sv.size());
    }
    ډګر_2[sv.size()] = '\0';
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

Locale::Nest::Nest(ډېرۍ&& ډېرۍ_2, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(ډېرۍ, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(ډېرۍ, region));
    U_ASSERT(هذا == reinterpret_cast<Nest*>(&ډېرۍ_2));
#إذا UPRV_HAS_FEATURE(حافظه_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&ډېرۍ_2, حجم(ډېرۍ));
#د_که_پای
    copy_منزل_Array<&Nest::script>(ډېرۍ_2.script, هذا);
    copy_منزل_Array<&Nest::region>(ډېرۍ_2.region, هذا);
    هذا->variantBegin = variantBegin;
    *هذا->بنسټ_Name = '\0';
}

بنية Locale::ډېرۍ::Alloc : عام UMemory {
    FixedString ډک_Name;
    FixedString بنسټ_Name;
    int32_t variantBegin;

    ثابت حرف* getVariant() ثابت U_LIFETIME_BOUND {
        أرجع variantBegin == 0 ? "" : get_بنسټ_Name() + variantBegin;
    }
    ثابت حرف* get_ډک_Name() ثابت U_LIFETIME_BOUND { أرجع ډک_Name.معلومات(); }
    ثابت حرف* get_بنسټ_Name() ثابت U_LIFETIME_BOUND {
        إذا (بنسټ_Name.is_تشه()) {
            إذا (ثابت حرف* name = ډک_Name.معلومات(); *name != '@') {
                أرجع name;
            }
        }
        أرجع بنسټ_Name.معلومات();
    }

    Alloc(int32_t variantBegin) : ډک_Name(), بنسټ_Name(), variantBegin(variantBegin) {}

    Alloc(ثابت Alloc& other, UError_کوډ& حالت)
        : ډک_Name(), بنسټ_Name(), variantBegin(other.variantBegin) {
        إذا (U_SUCCESS(حالت)) {
            إذا (!other.ډک_Name.is_تشه()) {
                ډک_Name = other.ډک_Name;
                إذا (ډک_Name.is_تشه()) {
                    حالت = U_حافظه_ویش_ERROR;
                } وإلا {
                    إذا (!other.بنسټ_Name.is_تشه()) {
                        بنسټ_Name = other.بنسټ_Name;
                        إذا (بنسټ_Name.is_تشه()) {
                            حالت = U_حافظه_ویش_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = ړنګول;

    ~Alloc() = افتراضي;
};

ثابت حرف* Locale::ډېرۍ::getVariant() ثابت U_LIFETIME_BOUND { أرجع ptr->getVariant(); }
ثابت حرف* Locale::ډېرۍ::get_ډک_Name() ثابت U_LIFETIME_BOUND { أرجع ptr->get_ډک_Name(); }
ثابت حرف* Locale::ډېرۍ::get_بنسټ_Name() ثابت U_LIFETIME_BOUND { أرجع ptr->get_بنسټ_Name(); }

Locale::ډېرۍ::ډېرۍ(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = جديد Alloc(variantBegin);
    إذا (ptr == تش_اشاره) {
        type = eBOGUS;
    } وإلا {
        type = e_ډېرۍ;
        copy_منزل_Array<&ډېرۍ::language>(language, هذا);
        copy_منزل_Array<&ډېرۍ::script>(script, هذا);
        copy_منزل_Array<&ډېرۍ::region>(region, هذا);
    }
}

Locale::ډېرۍ::~ډېرۍ() {
    U_ASSERT(type == e_ډېرۍ);
    ړنګول ptr;
}

Locale::ډېرۍ& Locale::ډېرۍ::چلوونکی=(ثابت ډېرۍ& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UError_کوډ حالت = U_ZERO_ERROR;
    ptr = جديد Alloc(*other.ptr, حالت);
    إذا (ptr == تش_اشاره || U_FAILURE(حالت)) {
        ړنګول ptr;
    } وإلا {
        type = e_ډېرۍ;
        uprv_memcpy(language, other.language, حجم language);
        uprv_memcpy(script, other.script, حجم script);
        uprv_memcpy(region, other.region, حجم region);
    }
    أرجع *هذا;
}

Locale::ډېرۍ& Locale::ډېرۍ::چلوونکی=(ډېرۍ&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = e_ډېرۍ;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, حجم language);
    uprv_memcpy(script, other.script, حجم script);
    uprv_memcpy(region, other.region, حجم region);
    أرجع *هذا;
}

کينډۍ <ډول_نوم BogusFn, ډول_نوم NestFn, ډول_نوم ډېرۍ_Fn, ډول_نوم... دلیلونه>
خپلکاری Locale::Payload::لیدنه(BogusFn bogusFn, NestFn nestFn, ډېرۍ_Fn ډېرۍ_Fn_2, دلیلونه... دلیلونه_2) ثابت {
    بدل (type) {
        حالة eBOGUS:
            أرجع bogusFn(دلیلونه_2...);
        حالة eNEST:
            أرجع nestFn(nest, دلیلونه_2...);
        حالة e_ډېرۍ:
            أرجع ډېرۍ_Fn_2(ډېرۍ_2, دلیلونه_2...);
        افتراضي:
            UPRV_UNREACHABLE_EXIT;
    };
}

فراغ Locale::Payload::copy(ثابت Payload& other) {
    other.لیدنه([](Payload*) {},
                [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ثابت ډېرۍ& ډېرۍ_2, Payload* dst) { dst->ډېرۍ_2 = ډېرۍ_2; },
                هذا);
}

فراغ Locale::Payload::move(Payload&& other) noexcept {
    other.لیدنه(
        [](Payload*) {},
        [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ثابت ډېرۍ& ډېرۍ_2, Payload* dst) { dst->ډېرۍ_2 = std::move(const_cast<ډېرۍ&>(ډېرۍ_2)); },
        هذا);
}

Locale::Payload::~Payload() {
    إذا (type == e_ډېرۍ) { ډېرۍ_2.~ډېرۍ(); }
}

Locale::Payload::Payload(ثابت Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::چلوونکی=(ثابت Payload& other) U_LIFETIME_BOUND {
    إذا (هذا != &other) {
        سېټ_منزل_Bogus();
        copy(other);
    }
    أرجع *هذا;
}

Locale::Payload& Locale::Payload::چلوونکی=(Payload&& other) noexcept U_LIFETIME_BOUND {
    إذا (هذا != &other) {
        سېټ_منزل_Bogus();
        move(std::move(other));
    }
    أرجع *هذا;
}

فراغ Locale::Payload::سېټ_منزل_Bogus() {
    هذا->~Payload();
    type = eBOGUS;
}

کينډۍ <ډول_نوم T, ډول_نوم... دلیلونه> T& Locale::Payload::emplace(دلیلونه&&... دلیلونه_2) U_LIFETIME_BOUND {
    إذا constexpr (std::is_same_v<T, Nest>) {
        هذا->~Payload();
        ::جديد (&nest) Nest(std::forward<دلیلونه>(دلیلونه_2)...);
        أرجع nest;
    }
    إذا constexpr (std::is_same_v<T, ډېرۍ>) {
        U_ASSERT(type != e_ډېرۍ);
        ::جديد (&ډېرۍ_2) ډېرۍ(std::forward<دلیلونه>(دلیلونه_2)...);
        أرجع ډېرۍ_2;
    }
}

کينډۍ <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    أرجع type == eNEST ? &nest : تش_اشاره;
}
کينډۍ <> Locale::ډېرۍ* Locale::Payload::get() U_LIFETIME_BOUND {
    أرجع type == e_ډېرۍ ? &ډېرۍ_2 : تش_اشاره;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#تعریف_کړه SEP_CHAR '_'
#تعریف_کړه NULL_CHAR '\0'

Locale::~Locale() = افتراضي;

Locale::Locale()
    : UObject(), payload()
{
    init(تش_اشاره, خطأ);
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
    إذا( (newLanguage==تش_اشاره) && (newCountry == تش_اشاره) && (newVariant == تش_اشاره) )
    {
        init(تش_اشاره, خطأ); /* shortcut */
    }
    وإلا
    {
        UError_کوډ حالت = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        إذا ( newLanguage != تش_اشاره )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            إذا ( lsize < 0 || lsize > ULOC_STRING_حد ) { // int32 wrap
                أرجع;
            }
        }

        CharString togo(newLanguage, lsize, حالت); // start with newLanguage

        // _Country
        إذا ( newCountry != تش_اشاره )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            إذا ( csize < 0 || csize > ULOC_STRING_حد ) { // int32 wrap
                أرجع;
            }
        }

        // _Variant
        إذا ( newVariant != تش_اشاره )
        {
            // remove leading _'s
            طالما(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            إذا ( vsize < 0 || vsize > ULOC_STRING_حد ) { // int32 wrap
                أرجع;
            }
            طالما( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        إذا ( newKeywords != تش_اشاره)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            إذا ( ksize < 0 || ksize > ULOC_STRING_حد ) {
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

Locale& Locale::چلوونکی=(ثابت Locale&) U_LIFETIME_BOUND = افتراضي;
Locale& Locale::چلوونکی=(Locale&&) noexcept U_LIFETIME_BOUND = افتراضي;

Locale *
Locale::clone() ثابت {
    أرجع جديد Locale(*هذا);
}

منطقي
Locale::چلوونکی==( ثابت   Locale& other) ثابت
{
    أرجع uprv_strcmp(other.getName(), getName()) == 0;
}

نطاق {

UInitOnce g_معلوم_Canonicalized_Init_Once {};
UHashtable *g_معلوم_Canonicalized = تش_اشاره;

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
    أرجع ریښتیا;
}

فراغ U_CALLCONV بارول_معلوم_Canonicalized(UError_کوډ &حالت) {
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_LOCALE_معلوم_CANONICALIZED,
                                cleanup_معلوم_Canonicalized);
    Local_UHashtable_اشاره_ګر new_معلوم_Canonicalized_Map(
        uhash_open(uhash_hashChars, uhash_compareChars, تش_اشاره, &حالت));
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
    Alias_معلومات* build(UError_کوډ &حالت);

خاص:
    فراغ readAlias(UResourceBundle* alias,
                   Unique_Char_مزي_ګانې* مزي_ګانې,
                   Local_حافظه<ثابت حرف*>& types,
                   Local_حافظه<int32_t>& replacementIndexes,
                   int32_t &length,
                   فراغ (*checkType)(ثابت حرف* type),
                   فراغ (*checkReplacement)(ثابت UChar* replacement),
                   UError_کوډ &حالت);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readLanguageAlias(UResourceBundle* alias,
                           Unique_Char_مزي_ګانې* مزي_ګانې,
                           Local_حافظه<ثابت حرف*>& types,
                           Local_حافظه<int32_t>& replacementIndexes,
                           int32_t &length,
                           UError_کوډ &حالت);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readScriptAlias(UResourceBundle* alias,
                         Unique_Char_مزي_ګانې* مزي_ګانې,
                         Local_حافظه<ثابت حرف*>& types,
                         Local_حافظه<int32_t>& replacementIndexes,
                         int32_t &length, UError_کوډ &حالت);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readTerritoryAlias(UResourceBundle* alias,
                            Unique_Char_مزي_ګانې* مزي_ګانې,
                            Local_حافظه<ثابت حرف*>& types,
                            Local_حافظه<int32_t>& replacementIndexes,
                            int32_t &length, UError_کوډ &حالت);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    فراغ readVariantAlias(UResourceBundle* alias,
                          Unique_Char_مزي_ګانې* مزي_ګانې,
                          Local_حافظه<ثابت حرف*>& types,
                          Local_حافظه<int32_t>& replacementIndexes,
                          int32_t &length, UError_کوډ &حالت);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    فراغ readSubdivisionAlias(UResourceBundle* alias,
                          Unique_Char_مزي_ګانې* مزي_ګانې,
                          Local_حافظه<ثابت حرف*>& types,
                          Local_حافظه<int32_t>& replacementIndexes,
                          int32_t &length, UError_کوډ &حالت);
};

/**
 * A class to hold the Alias Data.
 */
صنف Alias_معلومات : عام UMemory {
عام:
    ساكن ثابت Alias_معلومات* singleton(UError_کوډ& حالت) {
        إذا (U_FAILURE(حالت)) {
            // Do not get into loadData if the status already has error.
            أرجع تش_اشاره;
        }
        umtx_initOnce(Alias_معلومات::gInitOnce, &Alias_معلومات::بارول_معلومات, حالت);
        أرجع gSingleton;
    }

    ثابت CharStringMap& languageMap() ثابت U_LIFETIME_BOUND { أرجع language; }
    ثابت CharStringMap& scriptMap() ثابت U_LIFETIME_BOUND { أرجع script; }
    ثابت CharStringMap& territoryMap() ثابت U_LIFETIME_BOUND { أرجع territory; }
    ثابت CharStringMap& variantMap() ثابت U_LIFETIME_BOUND { أرجع variant; }
    ثابت CharStringMap& subdivisionMap() ثابت U_LIFETIME_BOUND { أرجع subdivision; }

    ساكن فراغ U_CALLCONV بارول_معلومات(UError_کوډ &حالت);
    ساكن UBool U_CALLCONV cleanup();

    ساكن UInitOnce gInitOnce;

خاص:
    Alias_معلومات(CharStringMap languageMap,
              CharStringMap scriptMap,
              CharStringMap territoryMap,
              CharStringMap variantMap,
              CharStringMap subdivisionMap,
              CharString* مزي_ګانې)
        : language(std::move(languageMap)),
          script(std::move(scriptMap)),
          territory(std::move(territoryMap)),
          variant(std::move(variantMap)),
          subdivision(std::move(subdivisionMap)),
          مزي_ګانې(مزي_ګانې) {
    }

    ~Alias_معلومات() {
        ړنګول مزي_ګانې;
    }

    ساكن ثابت Alias_معلومات* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* مزي_ګانې;

    ملګری صنف Alias_معلومات_Builder;
};


ثابت Alias_معلومات* Alias_معلومات::gSingleton = تش_اشاره;
UInitOnce Alias_معلومات::gInitOnce {};

UBool U_CALLCONV
Alias_معلومات::cleanup()
{
    gInitOnce.reset();
    ړنګول gSingleton;
    أرجع ریښتیا;
}

فراغ
Alias_معلومات_Builder::readAlias(
        UResourceBundle* alias,
        Unique_Char_مزي_ګانې* مزي_ګانې,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        فراغ (*checkType)(ثابت حرف* type),
        فراغ (*checkReplacement)(ثابت UChar* replacement),
        UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    length = ures_get_اندازه(alias);
    ثابت حرف** rawTypes = types.allocateInsteadAndCopy(length);
    إذا (rawTypes == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    إذا (rawIndexes == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    لكل (صحيح i = 0; U_SUCCESS(حالت) && ures_has_بل(alias); i++) {
        Local_UResource_Bundle_اشاره_ګر res(
            ures_get_بل_Resource(alias, تش_اشاره, &حالت));
        ثابت حرف* aliasFrom = ures_get_کلی(res.getAlias());
        ثابت UChar* alias_منزل =
            ures_get_String_By_کلی(res.getAlias(), "replacement", تش_اشاره, &حالت);
        إذا (U_FAILURE(حالت)) أرجع;

        checkType(aliasFrom);
        checkReplacement(alias_منزل);

        rawTypes[i] = aliasFrom;
        rawIndexes[i] = مزي_ګانې->جمع_کړه(alias_منزل, حالت);
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
        Unique_Char_مزي_ګانې* مزي_ګانې,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کوډ &حالت)
{
    أرجع readAlias(
        alias, مزي_ګانې, types, replacementIndexes, length,
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
#د_که_پای
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
        Unique_Char_مزي_ګانې* مزي_ګانې,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کوډ &حالت)
{
    أرجع readAlias(
        alias, مزي_ګانې, types, replacementIndexes, length,
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
#د_که_پای
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
        Unique_Char_مزي_ګانې* مزي_ګانې,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کوډ &حالت)
{
    أرجع readAlias(
        alias, مزي_ګانې, types, replacementIndexes, length,
#إذا U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#وإلا
        [](ثابت حرف*) {},
#د_که_پای
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
        Unique_Char_مزي_ګانې* مزي_ګانې,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کوډ &حالت)
{
    أرجع readAlias(
        alias, مزي_ګانې, types, replacementIndexes, length,
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
#د_که_پای
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
        Unique_Char_مزي_ګانې* مزي_ګانې,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کوډ &حالت)
{
    أرجع readAlias(
        alias, مزي_ګانې, types, replacementIndexes, length,
#إذا U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#وإلا
        [](ثابت حرف*) {},
#د_که_پای
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
Alias_معلومات::بارول_معلومات(UError_کوډ &حالت)
{
#که_تعریف_شوی LOCALE_CANONICALIZATION_DEBUG
    UDate پیل = uprv_getRawUTCtime();
#د_که_پای  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    Alias_معلومات_Builder builder;
    gSingleton = builder.build(حالت);
#که_تعریف_شوی LOCALE_CANONICALIZATION_DEBUG
    UDate پای_ټکی = uprv_getRawUTCtime();
    چاپ_کړه("AliasData::loadData took total %f ms\n", پای_ټکی - پیل);
#د_که_پای  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
Alias_معلومات*
Alias_معلومات_Builder::build(UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع تش_اشاره; }

    Local_UResource_Bundle_اشاره_ګر metadata(
        ures_open_مستقیم(تش_اشاره, "metadata", &حالت));
    Local_UResource_Bundle_اشاره_ګر metadataAlias(
        ures_get_By_کلی(metadata.getAlias(), "alias", تش_اشاره, &حالت));
    Local_UResource_Bundle_اشاره_ګر languageAlias(
        ures_get_By_کلی(metadataAlias.getAlias(), "language", تش_اشاره, &حالت));
    Local_UResource_Bundle_اشاره_ګر scriptAlias(
        ures_get_By_کلی(metadataAlias.getAlias(), "script", تش_اشاره, &حالت));
    Local_UResource_Bundle_اشاره_ګر territoryAlias(
        ures_get_By_کلی(metadataAlias.getAlias(), "territory", تش_اشاره, &حالت));
    Local_UResource_Bundle_اشاره_ګر variantAlias(
        ures_get_By_کلی(metadataAlias.getAlias(), "variant", تش_اشاره, &حالت));
    Local_UResource_Bundle_اشاره_ګر subdivisionAlias(
        ures_get_By_کلی(metadataAlias.getAlias(), "subdivision", تش_اشاره, &حالت));

    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    int32_t languages_اوږدتیا = 0, script_اوږدتیا = 0, territory_اوږدتیا = 0,
            variant_اوږدتیا = 0, subdivision_اوږدتیا = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    Unique_Char_مزي_ګانې مزي_ګانې(حالت);
    Local_حافظه<ثابت حرف*> languageTypes;
    Local_حافظه<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &مزي_ګانې,
                      languageTypes,
                      languageReplacementIndexes,
                      languages_اوږدتیا,
                      حالت);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    Local_حافظه<ثابت حرف*> scriptTypes;
    Local_حافظه<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &مزي_ګانې,
                    scriptTypes,
                    scriptReplacementIndexes,
                    script_اوږدتیا,
                    حالت);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    Local_حافظه<ثابت حرف*> territoryTypes;
    Local_حافظه<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &مزي_ګانې,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territory_اوږدتیا, حالت);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    Local_حافظه<ثابت حرف*> variantTypes;
    Local_حافظه<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &مزي_ګانې,
                     variantTypes,
                     variantReplacementIndexes,
                     variant_اوږدتیا, حالت);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    Local_حافظه<ثابت حرف*> subdivisionTypes;
    Local_حافظه<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &مزي_ګانې,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivision_اوږدتیا, حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    // We can only use strings after freeze it.
    مزي_ګانې.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < languages_اوږدتیا; i++) {
        languageMap.put(languageTypes[i],
                        مزي_ګانې.get(languageReplacementIndexes[i]),
                        حالت);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < script_اوږدتیا; i++) {
        scriptMap.put(scriptTypes[i],
                      مزي_ګانې.get(scriptReplacementIndexes[i]),
                      حالت);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < territory_اوږدتیا; i++) {
        territoryMap.put(territoryTypes[i],
                         مزي_ګانې.get(territoryReplacementIndexes[i]),
                         حالت);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < variant_اوږدتیا; i++) {
        variantMap.put(variantTypes[i],
                       مزي_ګانې.get(variantReplacementIndexes[i]),
                       حالت);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < subdivision_اوږدتیا; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       مزي_ګانې.get(subdivisionReplacementIndexes[i]),
                       حالت);
    }

    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    // copy hashtables
    خپلکاری *معلومات = جديد Alias_معلومات(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        مزي_ګانې.orphan_Char_مزي_ګانې());

    إذا (معلومات == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
    }
    أرجع معلومات;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
صنف AliasReplacer {
عام:
    AliasReplacer(UError_کوډ& حالت) :
            language(تش_اشاره), script(تش_اشاره), region(تش_اشاره),
            extensions(تش_اشاره),
            // store value in variants only once
            variants(تش_اشاره,
                     ([](UElement e1, UElement e2) -> UBool {
                       أرجع 0==uprv_strcmp((ثابت حرف*)e1.اشاره_ګر,
                                             (ثابت حرف*)e2.اشاره_ګر);}),
                     حالت),
            معلومات(تش_اشاره) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    منطقي replace(
        ثابت Locale& locale, CharString& out, UError_کوډ& حالت);

خاص:
    ثابت حرف* language;
    ثابت حرف* script;
    ثابت حرف* region;
    ثابت حرف* extensions;
    UVector variants;

    ثابت Alias_معلومات* معلومات;

    مضمن منطقي not_تشه(ثابت حرف* str) {
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
        أرجع not_تشه(replacement) ?
            ((input == تش_اشاره) ?  replacement : input) :
            ((type == تش_اشاره) ? input  : تش_اشاره);
    }

    مضمن منطقي same(ثابت حرف* a, ثابت حرف* b) {
        إذا (a == تش_اشاره && b == تش_اشاره) {
            أرجع ریښتیا;
        }
        إذا ((a == تش_اشاره && b != تش_اشاره) ||
            (a != تش_اشاره && b == تش_اشاره)) {
          أرجع خطأ;
        }
        أرجع uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& output_منزل_String(CharString& out U_LIFETIME_BOUND, UError_کوډ& حالت);

    // Generate the lookup key.
    CharString& generate_کلی(ثابت حرف* language, ثابت حرف* region,
                            ثابت حرف* variant, CharString& out U_LIFETIME_BOUND,
                            UError_کوډ& حالت);

    فراغ parseLanguageReplacement(ثابت حرف* replacement,
                                  ثابت حرف*& replaceLanguage,
                                  ثابت حرف*& replaceScript,
                                  ثابت حرف*& replaceRegion,
                                  ثابت حرف*& replaceVariant,
                                  ثابت حرف*& replaceExtensions,
                                  UVector& منزل_Be_Freed,
                                  UError_کوډ& حالت);

    // Replace by using languageAlias.
    منطقي replaceLanguage(منطقي checkLanguage, منطقي checkRegion,
                         منطقي checkVariants, UVector& منزل_Be_Freed,
                         UError_کوډ& حالت);

    // Replace by using territoryAlias.
    منطقي replaceTerritory(UVector& منزل_Be_Freed, UError_کوډ& حالت);

    // Replace by using scriptAlias.
    منطقي replaceScript(UError_کوډ& حالت);

    // Replace by using variantAlias.
    منطقي replaceVariant(UError_کوډ& حالت);

    // Replace by using subdivisionAlias.
    منطقي replaceSubdivision(StringPiece subdivision,
                            CharString& output, UError_کوډ& حالت);

    // Replace transformed extensions.
    منطقي replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UError_کوډ& حالت);
};

CharString&
AliasReplacer::generate_کلی(
        ثابت حرف* language, ثابت حرف* region, ثابت حرف* variant,
        CharString& out U_LIFETIME_BOUND, UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع out; }
    out.append(language, حالت);
    إذا (not_تشه(region)) {
        out.append(SEP_CHAR, حالت)
            .append(region, حالت);
    }
    إذا (not_تشه(variant)) {
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
    UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = تش_اشاره;
    إذا (uprv_strchr(replacement, '_') == تش_اشاره) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        أرجع;
    }
    // We have multiple field so we have to allocate and parse
    خپلکاری str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), حالت, حالت);
    حرف* معلومات = str->معلومات();
    منزل_Be_Freed.adoptElement(str.release(), حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    replacedLanguage = (ثابت حرف*) معلومات;
    حرف* پای_ټکی_د_ډګر = uprv_strchr(معلومات, '_');
    *پای_ټکی_د_ډګر = '\0'; // null terminiate it.
    پای_ټکی_د_ډګر++;
    ثابت حرف* پیل = پای_ټکی_د_ډګر;
    پای_ټکی_د_ډګر = const_cast<حرف*>(uprv_strchr(پیل, '_'));
    size_t len = 0;
    إذا (پای_ټکی_د_ډګر == تش_اشاره) {
        len = uprv_strlen(پیل);
    } وإلا {
        len = پای_ټکی_د_ډګر - پیل;
        *پای_ټکی_د_ډګر = '\0'; // null terminiate it.
    }
    إذا (len == 4 && uprv_isASCIILetter(*پیل)) {
        // Got a script
        replacedScript = پیل;
        إذا (پای_ټکی_د_ډګر == تش_اشاره) {
            أرجع;
        }
        پیل = پای_ټکی_د_ډګر++;
        پای_ټکی_د_ډګر = const_cast<حرف*>(uprv_strchr(پیل, '_'));
        إذا (پای_ټکی_د_ډګر == تش_اشاره) {
            len = uprv_strlen(پیل);
        } وإلا {
            len = پای_ټکی_د_ډګر - پیل;
            *پای_ټکی_د_ډګر = '\0'; // null terminiate it.
        }
    }
    إذا (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = پیل;
        إذا (پای_ټکی_د_ډګر == تش_اشاره) {
            أرجع;
        }
        پیل = پای_ټکی_د_ډګر++;
        پای_ټکی_د_ډګر = const_cast<حرف*>(uprv_strchr(پیل, '_'));
        إذا (پای_ټکی_د_ډګر == تش_اشاره) {
            len = uprv_strlen(پیل);
        } وإلا {
            len = پای_ټکی_د_ډګر - پیل;
            *پای_ټکی_د_ډګر = '\0'; // null terminiate it.
        }
    }
    إذا (len >= 4) {
        // Got a variant
        replacedVariant = پیل;
        إذا (پای_ټکی_د_ډګر == تش_اشاره) {
            أرجع;
        }
        پیل = پای_ټکی_د_ډګر++;
    }
    replacedExtensions = پیل;
}

منطقي
AliasReplacer::replaceLanguage(
        منطقي checkLanguage, منطقي checkRegion,
        منطقي checkVariants, UVector& منزل_Be_Freed, UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (    (checkRegion && region == تش_اشاره) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        أرجع خطأ;
    }
    int32_t variant_اندازه = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ثابت حرف* پلټنه_Language = checkLanguage ? language : "und";
    ثابت حرف* پلټنه_Region = checkRegion ? region : تش_اشاره;
    ثابت حرف* پلټنه_Variant = تش_اشاره;
    لكل (int32_t variant_شاخص = 0;
            variant_شاخص < variant_اندازه;
            variant_شاخص++) {
        إذا (checkVariants) {
            U_ASSERT(variant_شاخص < variant_اندازه);
            پلټنه_Variant = static_cast<ثابت حرف*>(variants.elementAt(variant_شاخص));
        }

        إذا (پلټنه_Variant != تش_اشاره && uprv_strlen(پلټنه_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            پلټنه_Variant = تش_اشاره;
        }
        CharString type_کلی;
        generate_کلی(پلټنه_Language, پلټنه_Region, پلټنه_Variant, type_کلی,
                    حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خطأ;
        }
        ثابت حرف *replacement = معلومات->languageMap().get(type_کلی.معلومات());
        إذا (replacement == تش_اشاره) {
            // Found no replacement data.
            استمر;
        }

        ثابت حرف* replacedLanguage = تش_اشاره;
        ثابت حرف* replacedScript = تش_اشاره;
        ثابت حرف* replacedRegion = تش_اشاره;
        ثابت حرف* replacedVariant = تش_اشاره;
        ثابت حرف* replacedExtensions = تش_اشاره;
        parseLanguageReplacement(replacement,
                                 replacedLanguage,
                                 replacedScript,
                                 replacedRegion,
                                 replacedVariant,
                                 replacedExtensions,
                                 منزل_Be_Freed,
                                 حالت);
        replacedLanguage =
            (replacedLanguage != تش_اشاره && uprv_strcmp(replacedLanguage, "und") == 0) ?
            language : replacedLanguage;
        replacedScript = deleteOrReplace(script, تش_اشاره, replacedScript);
        replacedRegion = deleteOrReplace(region, پلټنه_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            پلټنه_Variant, پلټنه_Variant, replacedVariant);

        إذا (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(پلټنه_Variant, replacedVariant) &&
                replacedExtensions == تش_اشاره) {
            // Replacement produce no changes.
            استمر;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        إذا (پلټنه_Variant != تش_اشاره) {
            إذا (not_تشه(replacedVariant)) {
                variants.سېټ_Element_At((فراغ*)replacedVariant, variant_شاخص);
            } وإلا {
                variants.removeElementAt(variant_شاخص);
            }
        }
        إذا (replacedExtensions != تش_اشاره) {
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
        أرجع ریښتیا;
    }
    // Nothing changed by language alias data.
    أرجع خطأ;
}

منطقي
AliasReplacer::replaceTerritory(UVector& منزل_Be_Freed, UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (region == تش_اشاره) {
        // No region to search.
        أرجع خطأ;
    }
    ثابت حرف *replacement = معلومات->territoryMap().get(region);
    إذا (replacement == تش_اشاره) {
        // Found no replacement data for this region.
        أرجع خطأ;
    }
    ثابت حرف* replacedRegion = replacement;
    ثابت حرف* لومړی_Space = uprv_strchr(replacement, ' ');
    إذا (لومړی_Space != تش_اشاره) {
        // If there are are more than one region in the replacement.
        // We need to check which one match based on the language.
        // Cannot use nullptr for language because that will construct
        // the default locale, in that case, use "und" to get the correct
        // locale.
        Locale l = LocaleBuilder()
            .سېټ_Language(language == تش_اشاره ? "und" : language)
            .سېټ_Script(script)
            .build(حالت);
        l.جمع_کړه_Likely_Subtags(حالت);
        ثابت حرف* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> توکی;
        إذا (likelyRegion != تش_اشاره && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ثابت حرف* وموندل_دننه_Replacement = uprv_strstr(replacement,
                                                         likelyRegion);
            إذا (وموندل_دننه_Replacement != تش_اشاره) {
                // Assuming the case there are no three letter region code in
                // the replacement of territoryAlias
                U_ASSERT(وموندل_دننه_Replacement == replacement ||
                         *(وموندل_دننه_Replacement-1) == ' ');
                U_ASSERT(وموندل_دننه_Replacement[len] == ' ' ||
                         وموندل_دننه_Replacement[len] == '\0');
                توکی = prv::make_unique<CharString>(
                    وموندل_دننه_Replacement, static_cast<int32_t>(len), حالت, حالت);
            }
        }
        إذا (!توکی && U_SUCCESS(حالت)) {
            توکی = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(لومړی_Space - replacement), حالت, حالت);
        }
        إذا (U_FAILURE(حالت)) { أرجع خطأ; }
        replacedRegion = توکی->معلومات();
        منزل_Be_Freed.adoptElement(توکی.release(), حالت);
        إذا (U_FAILURE(حالت)) { أرجع خطأ; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    أرجع ریښتیا;
}

منطقي
AliasReplacer::replaceScript(UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (script == تش_اشاره) {
        // No script to search.
        أرجع خطأ;
    }
    ثابت حرف *replacement = معلومات->scriptMap().get(script);
    إذا (replacement == تش_اشاره) {
        // Found no replacement data for this script.
        أرجع خطأ;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    أرجع ریښتیا;
}

منطقي
AliasReplacer::replaceVariant(UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    // Since we may have more than one variant, we need to loop through them.
    لكل (int32_t i = 0; i < variants.size(); i++) {
        ثابت حرف* variant = static_cast<ثابت حرف*>(variants.elementAt(i));
        ثابت حرف *replacement = معلومات->variantMap().get(variant);
        إذا (replacement == تش_اشاره) {
            // Found no replacement data for this variant.
            استمر;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        إذا (!same(variant, replacement)) {
            variants.سېټ_Element_At((فراغ*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            إذا (uprv_strcmp(variant, "heploc") == 0) {
                لكل (int32_t j = 0; j < variants.size(); j++) {
                     إذا (uprv_strcmp((ثابت حرف*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            أرجع ریښتیا;
        }
    }
    أرجع خطأ;
}

منطقي
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    ثابت حرف *replacement = معلومات->subdivisionMap().get(subdivision.معلومات());
    إذا (replacement != تش_اشاره) {
        ثابت حرف* لومړی_Space = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (لومړی_Space != تش_اشاره) ?
            (لومړی_Space - replacement) : uprv_strlen(replacement);
        إذا (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), حالت);
            إذا (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, حالت);
            }
        }
        أرجع ریښتیا;
    }
    أرجع خطأ;
}

منطقي
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UError_کوډ& حالت)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    int32_t len = transformedExtensions.length();
    ثابت حرف* str = transformedExtensions.معلومات();
    ثابت حرف* tkey = ultag_get_TKey_پیل(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == تش_اشاره) ? len : static_cast<int32_t>((tkey - str - 1)));
    إذا (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .سېټ_Language_نښان(StringPiece(str, tlangLen))
            .build(حالت);
        tlang.canonicalize(حالت);
        output = tlang.منزل_Language_نښان<CharString>(حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خطأ;
        }
        T_CString_منزل_ښکتنی_Case(output.معلومات());
    }
    إذا (tkey != تش_اشاره) {
        // We need to sort the tfields by tkey
        UVector tfields(حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خطأ;
        }
        افعل {
            ثابت حرف* tvalue = uprv_strchr(tkey, '-');
            إذا (tvalue == تش_اشاره) {
                حالت = U_ILLEGAL_دلیل_ERROR;
                أرجع خطأ;
            }
            ثابت حرف* بل_TKey = ultag_get_TKey_پیل(tvalue);
            إذا (بل_TKey != تش_اشاره) {
                *const_cast<حرف*>(بل_TKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.ورننباسه_Element_At((فراغ*)tkey, tfields.size(), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع خطأ;
            }
            tkey = بل_TKey;
        } طالما (tkey != تش_اشاره);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            أرجع uprv_strcmp((ثابت حرف*)e1.اشاره_ګر, (ثابت حرف*)e2.اشاره_ګر);
        }, حالت);
        لكل (int32_t i = 0; i < tfields.size(); i++) {
             إذا (output.length() > 0) {
                 output.append('-', حالت);
             }
             ثابت حرف* tfield = static_cast<ثابت حرف*>(tfields.elementAt(i));
             ثابت حرف* tvalue = uprv_strchr(tfield, '-');
             إذا (tvalue == تش_اشاره) {
                 حالت = U_ILLEGAL_دلیل_ERROR;
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
    أرجع ریښتیا;
}

CharString&
AliasReplacer::output_منزل_String(
    CharString& out U_LIFETIME_BOUND, UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع out; }
    out.append(language, حالت);
    إذا (not_تشه(script)) {
        out.append(SEP_CHAR, حالت)
            .append(script, حالت);
    }
    إذا (not_تشه(region)) {
        out.append(SEP_CHAR, حالت)
            .append(region, حالت);
    }
    إذا (variants.size() > 0) {
        إذا (!not_تشه(script) && !not_تشه(region)) {
          out.append(SEP_CHAR, حالت);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            أرجع uprv_strcmp((ثابت حرف*)e1.اشاره_ګر, (ثابت حرف*)e2.اشاره_ګر);
        }, حالت);
        int32_t variants_پیل = out.length();
        لكل (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, حالت)
                 .append(static_cast<ثابت حرف*>(variants.elementAt(i)),
                         حالت);
        }
        T_CString_منزل_پاسنی_Case(out.معلومات() + variants_پیل);
    }
    إذا (not_تشه(extensions)) {
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
AliasReplacer::replace(ثابت Locale& locale, CharString& out, UError_کوډ& حالت)
{
    معلومات = Alias_معلومات::singleton(حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    U_ASSERT(معلومات != تش_اشاره);
    out.clear();
    language = locale.getLanguage();
    إذا (!not_تشه(language)) {
        language = تش_اشاره;
    }
    script = locale.getScript();
    إذا (!not_تشه(script)) {
        script = تش_اشاره;
    }
    region = locale.getCountry();
    إذا (!not_تشه(region)) {
        region = تش_اشاره;
    }
    ثابت حرف* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, حالت);
    إذا (!variantsBuff.is_تشه()) {
        إذا (U_FAILURE(حالت)) { أرجع خطأ; }
        حرف* پیل = variantsBuff.معلومات();
        T_CString_منزل_ښکتنی_Case(پیل);
        حرف* پای_ټکی;
        طالما ((پای_ټکی = uprv_strchr(پیل, SEP_CHAR)) != تش_اشاره &&
               U_SUCCESS(حالت)) {
            *پای_ټکی = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            إذا (*پیل && !variants.لري(پیل)) {
                variants.جمع_کړه_Element(پیل, حالت);
            }
            پیل = پای_ټکی + 1;
        }
        // do not add "" or duplicate data to variants
        إذا (*پیل && !variants.لري(پیل)) {
            variants.جمع_کړه_Element(پیل, حالت);
        }
    }
    إذا (U_FAILURE(حالت)) { أرجع خطأ; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        أرجع uprv_strcmp((ثابت حرف*)e1.اشاره_ګر, (ثابت حرف*)e2.اشاره_ګر);
    }, حالت);

    // A changed count to assert when loop too many times.
    int32_t بدل_شوی = 0;
    ثابت int32_t max_بدل_شوی = 4 + variants.size();
    (فراغ)max_بدل_شوی; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector مزي_ګانې_منزل_Be_Freed([](فراغ *obj) { ړنګول static_cast<CharString*>(obj); },
                             تش_اشاره, 10, حالت);
    طالما (U_SUCCESS(حالت)) {
        // Something wrong with the data cause looping here more than maxChanged
        // times already.
        U_ASSERT(بدل_شوی <= max_بدل_شوی);
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
        إذا (    replaceLanguage(ریښتیا, ریښتیا,  ریښتیا,  مزي_ګانې_منزل_Be_Freed, حالت) ||
                replaceLanguage(ریښتیا, ریښتیا,  خطأ, مزي_ګانې_منزل_Be_Freed, حالت) ||
                replaceLanguage(ریښتیا, خطأ, ریښتیا,  مزي_ګانې_منزل_Be_Freed, حالت) ||
                replaceLanguage(ریښتیا, خطأ, خطأ, مزي_ګانې_منزل_Be_Freed, حالت) ||
                replaceLanguage(خطأ,خطأ, ریښتیا,  مزي_ګانې_منزل_Be_Freed, حالت) ||
                replaceTerritory(مزي_ګانې_منزل_Be_Freed, حالت) ||
                replaceScript(حالت) ||
                replaceVariant(حالت)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            بدل_شوی++;
            استمر;
        }
        // Nothing changed. Break out.
        اكسر;
    }  // while(1)

    إذا (U_FAILURE(حالت)) { أرجع خطأ; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ثابت حرف* extensionsStr = locale_get_Keywords_پیل(locale.getName());
    إذا (بدل_شوی == 0 && variants.size() <= 1 && extensionsStr == تش_اشاره) {
        أرجع خطأ;
    }
    output_منزل_String(out, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (extensionsStr != تش_اشاره) {
        بدل_شوی = 0;
        Locale لنډمهاله(locale);
        Local_اشاره_ګر<icu::StringEnumeration> iter(locale.createKeywords(حالت));
        إذا (U_SUCCESS(حالت) && !iter.isNull()) {
            ثابت حرف* کلی;
            طالما ((کلی = iter->next(تش_اشاره, حالت)) != تش_اشاره) {
                إذا (uprv_strcmp("sd", کلی) == 0 || uprv_strcmp("rg", کلی) == 0 ||
                        uprv_strcmp("t", کلی) == 0) {
                    خپلکاری value = locale.get_Keyword_ارزښت<CharString>(کلی, حالت);
                    إذا (U_FAILURE(حالت)) {
                        حالت = U_ZERO_ERROR;
                        استمر;
                    }
                    CharString replacement;
                    إذا (uprv_strlen(کلی) == 2) {
                        إذا (replaceSubdivision(value.منزل_String_Piece(), replacement, حالت)) {
                            بدل_شوی++;
                            لنډمهاله.سېټ_Keyword_ارزښت(کلی, replacement.معلومات(), حالت);
                        }
                    } وإلا {
                        U_ASSERT(uprv_strcmp(کلی, "t") == 0);
                        إذا (replaceTransformedExtensions(value, replacement, حالت)) {
                            بدل_شوی++;
                            لنډمهاله.سېټ_Keyword_ارزښت(کلی, replacement.معلومات(), حالت);
                        }
                    }
                    إذا (U_FAILURE(حالت)) {
                        أرجع خطأ;
                    }
                }
            }
        }
        إذا (بدل_شوی != 0) {
            extensionsStr = locale_get_Keywords_پیل(لنډمهاله.getName());
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
    أرجع ریښتیا;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
منطقي
canonicalizeLocale(ثابت Locale& locale, CharString& out, UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع خطأ; }
    AliasReplacer replacer(حالت);
    أرجع replacer.replace(locale, out, حالت);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
منطقي
is_معلوم_Canonicalized_Locale(ثابت حرف* locale, UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع خطأ; }

    إذا (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        أرجع ریښتیا;
    }

    // common well-known Canonicalized.
    umtx_initOnce(g_معلوم_Canonicalized_Init_Once,
                  &بارول_معلوم_Canonicalized, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    U_ASSERT(g_معلوم_Canonicalized != تش_اشاره);
    أرجع uhash_geti(g_معلوم_Canonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_پای_ټکی

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
    UError_کوډ حالت = U_ZERO_ERROR;
    CharString لنډمهاله;
    أرجع !canonicalizeLocale(l, لنډمهاله, حالت) && U_SUCCESS(حالت);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ثابت حرف* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    أرجع localeID == تش_اشاره ? *هذا = getDefault() : init(StringPiece{localeID}, canonicalize);
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
        حرف *ډګر_2[5] = {تش_اشاره};
        int32_t ډګر_Len[5] = {0};
        int32_t ډګر_Idx;
        int32_t variant_ډګر;
        int32_t length;
        UError_کوډ err;

        ثابت خپلکاری parse = [canonicalize](std::string_view localeID,
                                          حرف* name,
                                          int32_t name_ظرفیت,
                                          UError_کوډ& حالت) {
            أرجع ByteSinkUtil::له_لارې_Byte_Sink_منزل_Terminated_Chars(
                name, name_ظرفیت,
                [&](ByteSink& sink, UError_کوډ& حالت) {
                    إذا (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, حالت);
                    } وإلا {
                        ulocimp_getName(localeID, sink, حالت);
                    }
                },
                حالت);
        };

        // "canonicalize" the locale ID to ICU/Java format
        حرف* ډک_Name = nest.بنسټ_Name;
        err = U_ZERO_ERROR;
        length = parse(localeID, ډک_Name, حجم Nest::بنسټ_Name, err);

        FixedString ډک_Name_بفر;
        إذا (err == U_بفر_ډکېدنه_ERROR || length >= static_cast<int32_t>(حجم Nest::بنسټ_Name)) {
            /*Go to heap for the fullName if necessary*/
            إذا (!ډک_Name_بفر.reserve(length + 1)) {
                اكسر; // error: out of memory
            }
            ډک_Name = ډک_Name_بفر.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, ډک_Name, length + 1, err);
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
        separator = ډګر_2[0] = ډک_Name;
        ډګر_Idx = 1;
        حرف* at = uprv_strchr(ډک_Name, '@');
        طالما ((separator = uprv_strchr(ډګر_2[ډګر_Idx-1], SEP_CHAR)) != تش_اشاره &&
               ډګر_Idx < UPRV_LENGTHOF(ډګر_2)-1 &&
               (at == تش_اشاره || separator < at)) {
            ډګر_2[ډګر_Idx] = separator + 1;
            ډګر_Len[ډګر_Idx - 1] = static_cast<int32_t>(separator - ډګر_2[ډګر_Idx - 1]);
            ډګر_Idx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(ډګر_2[ډګر_Idx-1], '@');
        حرف* sep2 = uprv_strchr(ډګر_2[ډګر_Idx-1], '.');
        إذا (separator!=تش_اشاره || sep2!=تش_اشاره) {
            إذا (separator==تش_اشاره || (sep2!=تش_اشاره && separator > sep2)) {
                separator = sep2;
            }
            ډګر_Len[ډګر_Idx - 1] = static_cast<int32_t>(separator - ډګر_2[ډګر_Idx - 1]);
        } وإلا {
            ډګر_Len[ډګر_Idx - 1] = length - static_cast<int32_t>(ډګر_2[ډګر_Idx - 1] - ډک_Name);
        }
        منطقي hasKeywords = at != تش_اشاره && uprv_strchr(at + 1, '=') != تش_اشاره;

        إذا (ډګر_Len[0] >= ULOC_LANG_ظرفیت)
        {
            اكسر; // error: the language field is too long
        }

        variant_ډګر = 1; /* Usually the 2nd one, except when a script or country is also used. */
        إذا (ډګر_Len[0] > 0) {
            /* We have a language */
            language = {ډک_Name, static_cast<std::string_view::size_type>(ډګر_Len[0])};
        }
        إذا (ډګر_Len[1] == 4 && uprv_isASCIILetter(ډګر_2[1][0]) &&
                uprv_isASCIILetter(ډګر_2[1][1]) && uprv_isASCIILetter(ډګر_2[1][2]) &&
                uprv_isASCIILetter(ډګر_2[1][3])) {
            /* We have at least a script */
            script = {ډګر_2[1], static_cast<std::string_view::size_type>(ډګر_Len[1])};
            variant_ډګر++;
        }

        إذا (ډګر_Len[variant_ډګر] == 2 || ډګر_Len[variant_ډګر] == 3) {
            /* We have a country */
            region = {ډګر_2[variant_ډګر], static_cast<std::string_view::size_type>(ډګر_Len[variant_ډګر])};
            variant_ډګر++;
        } وإلا إذا (ډګر_Len[variant_ډګر] == 0) {
            variant_ډګر++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        إذا (ډګر_Len[variant_ډګر] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(ډګر_2[variant_ډګر] - ډک_Name);
        } وإلا إذا (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - ډک_Name);
        }

        إذا (!hasKeywords && Nest::ځاییږي(length, language, script, region)) {
            U_ASSERT(ډک_Name == nest.بنسټ_Name);
            U_ASSERT(ډک_Name_بفر.is_تشه());
            nest.init(language, script, region, variantBegin);
        } وإلا {
            إذا (ډک_Name == nest.بنسټ_Name) {
                U_ASSERT(ډک_Name_بفر.is_تشه());
                ډک_Name_بفر = {ډک_Name, static_cast<std::string_view::size_type>(length)};
                إذا (ډک_Name_بفر.is_تشه()) {
                    اكسر; // error: out of memory
                }
                إذا (!language.empty()) {
                    language = {ډک_Name_بفر.معلومات(), language.size()};
                }
                إذا (!script.empty()) {
                    script = {ډک_Name_بفر.معلومات() + (script.معلومات() - ډک_Name), script.size()};
                }
                إذا (!region.empty()) {
                    region = {ډک_Name_بفر.معلومات() + (region.معلومات() - ډک_Name), region.size()};
                }
            }
            ډېرۍ& ډېرۍ_2 = payload.emplace<ډېرۍ>(language, script, region, variantBegin);
            إذا (isBogus()) {
                اكسر; // error: out of memory
            }
            U_ASSERT(!ډک_Name_بفر.is_تشه());
            ډېرۍ_2.ptr->ډک_Name = std::move(ډک_Name_بفر);
            إذا (hasKeywords) {
                إذا (std::string_view::size_type بنسټ_Name_اوږدتیا = at - ډک_Name; بنسټ_Name_اوږدتیا > 0) {
                    ډېرۍ_2.ptr->بنسټ_Name = {ډېرۍ_2.ptr->ډک_Name.معلومات(), بنسټ_Name_اوږدتیا};
                    إذا (ډېرۍ_2.ptr->بنسټ_Name.is_تشه()) {
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
    سېټ_منزل_Bogus();

    أرجع *هذا;
}

int32_t
Locale::hash_کوډ() ثابت
{
    أرجع ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

فراغ
Locale::سېټ_منزل_Bogus() {
    /* Free our current storage */
    payload.سېټ_منزل_Bogus();
}

ثابت Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex قلف(&gDefaultLocaleMutex);
        إذا (gDefaultLocale != تش_اشاره) {
            أرجع *gDefaultLocale;
        }
    }
    UError_کوډ حالت = U_ZERO_ERROR;
    أرجع *locale_سېټ_default_دننی(تش_اشاره, حالت);
}



فراغ U_EXPORT2
Locale::سېټ_Default( ثابت   Locale&     newLocale,
                            UError_کوډ&  حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ثابت حرف *localeID = newLocale.getName();
    locale_سېټ_default_دننی(localeID, حالت);
}

فراغ
Locale::جمع_کړه_Likely_Subtags(UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    CharString maximizedLocaleID = ulocimp_جمع_کړه_Likely_Subtags(getName(), حالت);

    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_حافظه_ویش_ERROR) {
            سېټ_منزل_Bogus();
        }
        أرجع;
    }

    init(maximizedLocaleID.معلومات(), /*canonicalize=*/خطأ);
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
}

فراغ
Locale::minimizeSubtags(UError_کوډ& حالت) {
    Locale::minimizeSubtags(خطأ, حالت);
}
فراغ
Locale::minimizeSubtags(منطقي favorScript, UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, حالت);

    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_حافظه_ویش_ERROR) {
            سېټ_منزل_Bogus();
        }
        أرجع;
    }

    init(minimizedLocaleID.معلومات(), /*canonicalize=*/خطأ);
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
}

فراغ
Locale::canonicalize(UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    CharString uncanonicalized(getName(), حالت);
    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_حافظه_ویش_ERROR) {
            سېټ_منزل_Bogus();
        }
        أرجع;
    }
    init(uncanonicalized.معلومات(), /*canonicalize=*/ریښتیا);
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
}

Locale U_EXPORT2
Locale::for_Language_نښان(StringPiece نښان, UError_کوډ& حالت)
{
    Locale پایله(Locale::eBOGUS);

    إذا (U_FAILURE(حالت)) {
        أرجع پایله;
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

    int32_t parsed_اوږدتیا;
    CharString localeID = ulocimp_for_Language_نښان(
            نښان.معلومات(),
            نښان.length(),
            &parsed_اوږدتیا,
            حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع پایله;
    }

    إذا (parsed_اوږدتیا != نښان.size()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع پایله;
    }

    پایله.init(localeID.معلومات(), /*canonicalize=*/خطأ);
    إذا (پایله.isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
    أرجع پایله;
}

فراغ
Locale::منزل_Language_نښان(ByteSink& sink, UError_کوډ& حالت) ثابت
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    ulocimp_منزل_Language_نښان(getName(), sink, /*strict=*/خطأ, حالت);
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
    loc.init(name, ریښتیا);
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
فراغ Locale::سېټ_From_POSIXID(ثابت حرف *posixID)
{
    init(posixID, ریښتیا);
}

ثابت Locale & U_EXPORT2
Locale::get_ریښه()
{
    أرجع getLocale(e_ریښه);
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
    إذا (localeCache == تش_اشاره) {
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
    UError_کوډ حالت = U_ZERO_ERROR;
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
    KeywordEnumeration(ثابت حرف *کلیګانې, int32_t keywordLen, int32_t current_شاخص, UError_کوډ &حالت)
        : keywords(), length(keywordLen), current(تش_اشاره) {
        إذا(U_SUCCESS(حالت) && keywordLen != 0) {
            إذا(کلیګانې == تش_اشاره || keywordLen < 0) {
                حالت = U_ILLEGAL_دلیل_ERROR;
            } وإلا {
                keywords = {کلیګانې, static_cast<std::string_view::size_type>(length)};
                إذا (keywords.is_تشه()) {
                    حالت = U_حافظه_ویش_ERROR;
                } وإلا {
                    current = keywords.معلومات() + current_شاخص;
                }
            }
        }
    }

    مجازي ~KeywordEnumeration();

    مجازي StringEnumeration * clone() ثابت override
    {
        UError_کوډ حالت = U_ZERO_ERROR;
        أرجع جديد KeywordEnumeration(
                keywords.معلومات(), length,
                static_cast<int32_t>(current - keywords.معلومات()), حالت);
    }

    مجازي int32_t count(UError_کوډ& حالت) ثابت override {
        إذا (U_FAILURE(حالت)) { أرجع 0; }
        ثابت حرف *kw = keywords.معلومات();
        int32_t پایله = 0;
        طالما(*kw) {
            پایله++;
            kw += uprv_strlen(kw)+1;
        }
        أرجع پایله;
    }

    مجازي ثابت حرف* next(int32_t* پایله_اوږدتیا, UError_کوډ& حالت) U_LIFETIME_BOUND override {
        ثابت حرف* پایله;
        int32_t len;
        إذا(U_SUCCESS(حالت) && *current != 0) {
            پایله = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            إذا(پایله_اوږدتیا != تش_اشاره) {
                *پایله_اوږدتیا = len;
            }
        } وإلا {
            إذا(پایله_اوږدتیا != تش_اشاره) {
                *پایله_اوږدتیا = 0;
            }
            پایله = تش_اشاره;
        }
        أرجع پایله;
    }

    مجازي ثابت UnicodeString* snext(UError_کوډ& حالت) U_LIFETIME_BOUND override {
        إذا (U_FAILURE(حالت)) { أرجع تش_اشاره; }
        int32_t پایله_اوږدتیا = 0;
        ثابت حرف *s = next(&پایله_اوږدتیا, حالت);
        أرجع سېټ_Chars(s, پایله_اوږدتیا, حالت);
    }

    مجازي فراغ reset(UError_کوډ& حالت) override {
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
    کارول KeywordEnumeration::KeywordEnumeration;
    مجازي ~UnicodeKeywordEnumeration();

    مجازي ثابت حرف* next(int32_t* پایله_اوږدتیا, UError_کوډ& حالت) U_LIFETIME_BOUND override {
        ثابت حرف* legacy_کلی = KeywordEnumeration::next(تش_اشاره, حالت);
        طالما (U_SUCCESS(حالت) && legacy_کلی != تش_اشاره) {
            ثابت حرف* کلی = uloc_منزل_Unicode_Locale_کلی(legacy_کلی);
            إذا (کلی != تش_اشاره) {
                إذا (پایله_اوږدتیا != تش_اشاره) {
                    *پایله_اوږدتیا = static_cast<int32_t>(uprv_strlen(کلی));
                }
                أرجع کلی;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_کلی = KeywordEnumeration::next(تش_اشاره, حالت);
        }
        إذا (پایله_اوږدتیا != تش_اشاره) *پایله_اوږدتیا = 0;
        أرجع تش_اشاره;
    }
    مجازي int32_t count(UError_کوډ& حالت) ثابت override {
        إذا (U_FAILURE(حالت)) { أرجع 0; }
        ثابت حرف *kw = keywords.معلومات();
        int32_t پایله = 0;
        طالما(*kw) {
            إذا (uloc_منزل_Unicode_Locale_کلی(kw) != تش_اشاره) {
                پایله++;
            }
            kw += uprv_strlen(kw)+1;
        }
        أرجع پایله;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = افتراضي;

StringEnumeration *
Locale::createKeywords(UError_کوډ &حالت) ثابت
{
    StringEnumeration *پایله = تش_اشاره;

    إذا (U_FAILURE(حالت)) {
        أرجع پایله;
    }

    ثابت حرف* variant_پیل = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    إذا(variant_پیل) {
        إذا(assignment > variant_پیل) {
            CharString keywords = ulocimp_getKeywords(variant_پیل + 1, '@', خطأ, حالت);
            إذا (U_SUCCESS(حالت) && !keywords.is_تشه()) {
                پایله = جديد KeywordEnumeration(keywords.معلومات(), keywords.length(), 0, حالت);
                إذا (!پایله) {
                    حالت = U_حافظه_ویش_ERROR;
                }
            }
        } وإلا {
            حالت = U_INVALID_FORMAT_ERROR;
        }
    }
    أرجع پایله;
}

StringEnumeration *
Locale::createUnicodeKeywords(UError_کوډ &حالت) ثابت
{
    StringEnumeration *پایله = تش_اشاره;

    إذا (U_FAILURE(حالت)) {
        أرجع پایله;
    }

    ثابت حرف* variant_پیل = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    إذا(variant_پیل) {
        إذا(assignment > variant_پیل) {
            CharString keywords = ulocimp_getKeywords(variant_پیل + 1, '@', خطأ, حالت);
            إذا (U_SUCCESS(حالت) && !keywords.is_تشه()) {
                پایله = جديد UnicodeKeywordEnumeration(keywords.معلومات(), keywords.length(), 0, حالت);
                إذا (!پایله) {
                    حالت = U_حافظه_ویش_ERROR;
                }
            }
        } وإلا {
            حالت = U_INVALID_FORMAT_ERROR;
        }
    }
    أرجع پایله;
}

int32_t
Locale::get_Keyword_ارزښت(ثابت حرف* keywordName, حرف *بفر, int32_t bufLen, UError_کوډ &حالت) ثابت
{
    أرجع uloc_get_Keyword_ارزښت(getName(), keywordName, بفر, bufLen, &حالت);
}

فراغ
Locale::get_Keyword_ارزښت(StringPiece keywordName, ByteSink& sink, UError_کوډ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    ulocimp_get_Keyword_ارزښت(getName(), keywordName, sink, حالت);
}

فراغ
Locale::get_Unicode_Keyword_ارزښت(StringPiece keywordName,
                               ByteSink& sink,
                               UError_کوډ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    std::optional<std::string_view> legacy_کلی = ulocimp_منزل_Legacy_کلی_With_Fallback(keywordName);
    إذا (!legacy_کلی.has_value()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    خپلکاری legacy_ارزښت = get_Keyword_ارزښت<CharString>(*legacy_کلی, حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    std::optional<std::string_view> unicode_ارزښت =
        ulocimp_منزل_Bcp_Type_With_Fallback(keywordName, legacy_ارزښت.منزل_String_Piece());
    إذا (!unicode_ارزښت.has_value()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    sink.Append(unicode_ارزښت->معلومات(), static_cast<int32_t>(unicode_ارزښت->size()));
}

فراغ
Locale::سېټ_Keyword_ارزښت(StringPiece keywordName,
                        StringPiece keyword_ارزښت,
                        UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }
    إذا (keywordName.empty()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا (حالت == U_STRING_NOT_TERMINATED_WARNING) {
        حالت = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, حالت);
    ulocimp_سېټ_Keyword_ارزښت(keywordName, keyword_ارزښت, localeID, حالت);
    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_حافظه_ویش_ERROR) {
            سېټ_منزل_Bogus();
        }
        أرجع;
    }

    ثابت حرف* at = locale_get_Keywords_پیل(localeID.منزل_String_Piece());
    منطقي hasKeywords = at != تش_اشاره && uprv_strchr(at + 1, '=') != تش_اشاره;

    Nest* nest = payload.get<Nest>();
    إذا (!hasKeywords) {
        إذا (nest == تش_اشاره) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            ډېرۍ* ډېرۍ_2 = payload.get<ډېرۍ>();
            U_ASSERT(ډېرۍ_2 != تش_اشاره);
            إذا (Nest::ځاییږي(localeID.length(), ډېرۍ_2->language, ډېرۍ_2->script, ډېرۍ_2->region)) {
                int32_t variantBegin = ډېرۍ_2->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < حجم Nest::بنسټ_Name);
                nest = &payload.emplace<Nest>(std::move(*ډېرۍ_2), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->بنسټ_Name, حجم Nest::بنسټ_Name, حالت);
            } وإلا {
                ډېرۍ_2->ptr->بنسټ_Name.clear();
                ډېرۍ_2->ptr->ډک_Name = localeID.منزل_String_Piece();
                إذا (ډېرۍ_2->ptr->ډک_Name.is_تشه()) {
                    حالت = U_حافظه_ویش_ERROR;
                    سېټ_منزل_Bogus();
                    أرجع;
                }
            }
        }
    } وإلا {
        ډېرۍ* ډېرۍ_2 = تش_اشاره;
        إذا (nest != تش_اشاره) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            ډېرۍ_2 = &payload.emplace<ډېرۍ>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            إذا (isBogus()) {
                حالت = U_حافظه_ویش_ERROR;
                أرجع;
            }
        } وإلا {
            ډېرۍ_2 = payload.get<ډېرۍ>();
        }
        U_ASSERT(ډېرۍ_2 != تش_اشاره);
        ډېرۍ_2->ptr->ډک_Name = localeID.منزل_String_Piece();
        إذا (ډېرۍ_2->ptr->ډک_Name.is_تشه()) {
            حالت = U_حافظه_ویش_ERROR;
            سېټ_منزل_Bogus();
            أرجع;
        }

        إذا (ډېرۍ_2->ptr->بنسټ_Name.is_تشه()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            إذا (std::string_view::size_type بنسټ_Name_اوږدتیا = at - localeID.معلومات(); بنسټ_Name_اوږدتیا > 0) {
                ډېرۍ_2->ptr->بنسټ_Name = {ډېرۍ_2->ptr->ډک_Name.معلومات(), بنسټ_Name_اوږدتیا};
                إذا (ډېرۍ_2->ptr->بنسټ_Name.is_تشه()) {
                    حالت = U_حافظه_ویش_ERROR;
                    سېټ_منزل_Bogus();
                    أرجع;
                }
            }
        }
    }
}

فراغ
Locale::سېټ_Unicode_Keyword_ارزښت(StringPiece keywordName,
                               StringPiece keyword_ارزښت,
                               UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    std::optional<std::string_view> legacy_کلی = ulocimp_منزل_Legacy_کلی_With_Fallback(keywordName);
    إذا (!legacy_کلی.has_value()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    std::string_view value;

    إذا (!keyword_ارزښت.empty()) {
        std::optional<std::string_view> legacy_ارزښت =
            ulocimp_منزل_Legacy_Type_With_Fallback(keywordName, keyword_ارزښت);
        إذا (!legacy_ارزښت.has_value()) {
            حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        value = *legacy_ارزښت;
    }

    سېټ_Keyword_ارزښت(*legacy_کلی, value, حالت);
}

ثابت حرف*
Locale::getCountry() ثابت U_LIFETIME_BOUND {
    أرجع get_ډګر<&Nest::getRegion, &ډېرۍ::getRegion>();
}

ثابت حرف*
Locale::getLanguage() ثابت U_LIFETIME_BOUND {
    أرجع get_ډګر<&Nest::getLanguage, &ډېرۍ::getLanguage>();
}

ثابت حرف*
Locale::getScript() ثابت U_LIFETIME_BOUND {
    أرجع get_ډګر<&Nest::getScript, &ډېرۍ::getScript>();
}

ثابت حرف*
Locale::getVariant() ثابت U_LIFETIME_BOUND {
    أرجع get_ډګر<&Nest::getVariant, &ډېرۍ::getVariant>();
}

ثابت حرف*
Locale::getName() ثابت U_LIFETIME_BOUND {
    أرجع get_ډګر<&Nest::get_بنسټ_Name, &ډېرۍ::get_ډک_Name>();
}

ثابت حرف*
Locale::get_بنسټ_Name() ثابت U_LIFETIME_BOUND {
    أرجع get_ډګر<&Nest::get_بنسټ_Name, &ډېرۍ::get_بنسټ_Name>();
}

کينډۍ <ثابت حرف* (Locale::Nest::*ثابت NEST)() ثابت,
          ثابت حرف* (Locale::ډېرۍ::*ثابت ډېرۍ_3)() ثابت>
ثابت حرف* Locale::get_ډګر() ثابت U_LIFETIME_BOUND {
    أرجع payload.لیدنه([] { أرجع ""; },
                         [](ثابت Nest& nest) { أرجع (nest.*NEST)(); },
                         [](ثابت ډېرۍ& ډېرۍ_2) { أرجع (ډېرۍ_2.*ډېرۍ_3)(); });
}

Locale::Iterator::~Iterator() = افتراضي;

//eof
U_NAMESPACE_پای_ټکی
