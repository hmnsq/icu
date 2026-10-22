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

#إذا UPRV_HAS_FEATURE(حافظہ_sanitizer)
#ضمن <sanitizer/msan_interface.h>
#جے_انت

U_NAMESPACE_BEGIN

ساكن Locale   *gLocaleCache = خالی_سوچک;
ساكن UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
ساكن UMutex gDefaultLocaleMutex;
ساكن UHashtable *gDefaultLocalesHashT = خالی_سوچک;
ساكن Locale *gDefaultLocale = خالی_سوچک;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#تعریف_کرو ULOC_STRING_حد_بندی 357913941

U_NAMESPACE_سرا

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
    e_جڑ,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

نطاق {

//
// Deleter function for Locales owned by the default Locale hash table/
//
فراغ U_CALLCONV
deleteLocale(فراغ *obj) {
    مٹاؤ static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    مٹاؤ [] gLocaleCache;
    gLocaleCache = خالی_سوچک;
    gLocaleCacheInitOnce.reset();

    إذا (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = خالی_سوچک;
    }
    gDefaultLocale = خالی_سوچک;
    أرجع سچ;
}

فراغ U_CALLCONV locale_init(UError_کوڈ &حالت) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == خالی_سوچک);
    gLocaleCache = جديد Locale[static_cast<صحيح>(eMAX_LOCALES)];
    إذا (gLocaleCache == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_جڑ]          = Locale("");
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

Locale *locale_سیٹ_default_اندرونی(ثابت حرف *id, UError_کوڈ& حالت) {
    // Synchronize this entire function.
    Mutex جندرا(&gDefaultLocaleMutex);

    UBool canonicalize = خطأ;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    إذا (id == خالی_سوچک) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = سچ; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, حالت) : ulocimp_getName(id, حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع gDefaultLocale;
    }

    إذا (gDefaultLocalesHashT == خالی_سوچک) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, خالی_سوچک, &حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع gDefaultLocale;
        }
        uhash_سیٹ_قدر_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.انکڑے()));
    إذا (newDefault == خالی_سوچک) {
        newDefault = جديد Locale(Locale::eBOGUS);
        إذا (newDefault == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
            أرجع gDefaultLocale;
        }
        newDefault->init(localeNameBuf.انکڑے(), خطأ);
        uhash_put(gDefaultLocalesHashT, const_cast<حرف*>(newDefault->getName()), newDefault, &حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    أرجع gDefaultLocale;
}

U_NAMESPACE_سرا

/* sfb 07/21/99 */
U_CFUNC فراغ
locale_سیٹ_default(ثابت حرف *id)
{
    U_NAMESPACE_USE
    UError_کوڈ حالت = U_ZERO_ERROR;
    locale_سیٹ_default_اندرونی(id, حالت);
}
/*سرا*/

U_CFUNC ثابت حرف *
locale_get_default()
{
    U_NAMESPACE_USE
    أرجع Locale::getDefault().getName();
}

نطاق {

سانچا <خودکار میدان, قسم_ناں T>
فراغ copy_منزل_Array(std::string_view sv, T* that) {
    خودکار& میدان_2 = that->*میدان;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(میدان_2)>>;
    static_assert(capacity > 0);
    إذا (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(میدان_2, sv.انکڑے(), sv.size());
    }
    میدان_2[sv.size()] = '\0';
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

Locale::Nest::Nest(ڈھیر_2&& ڈھیر_3, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(ڈھیر_2, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(ڈھیر_2, region));
    U_ASSERT(هذا == reinterpret_cast<Nest*>(&ڈھیر_3));
#إذا UPRV_HAS_FEATURE(حافظہ_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&ڈھیر_3, حجم(ڈھیر_2));
#جے_انت
    copy_منزل_Array<&Nest::script>(ڈھیر_3.script, هذا);
    copy_منزل_Array<&Nest::region>(ڈھیر_3.region, هذا);
    هذا->variantBegin = variantBegin;
    *هذا->ادھار_Name = '\0';
}

بنية Locale::ڈھیر_2::Alloc : عام UMemory {
    FixedString بھریا_Name;
    FixedString ادھار_Name;
    int32_t variantBegin;

    ثابت حرف* getVariant() ثابت U_LIFETIME_BOUND {
        أرجع variantBegin == 0 ? "" : get_ادھار_Name() + variantBegin;
    }
    ثابت حرف* get_بھریا_Name() ثابت U_LIFETIME_BOUND { أرجع بھریا_Name.انکڑے(); }
    ثابت حرف* get_ادھار_Name() ثابت U_LIFETIME_BOUND {
        إذا (ادھار_Name.is_خالی_تھاواں()) {
            إذا (ثابت حرف* name = بھریا_Name.انکڑے(); *name != '@') {
                أرجع name;
            }
        }
        أرجع ادھار_Name.انکڑے();
    }

    Alloc(int32_t variantBegin) : بھریا_Name(), ادھار_Name(), variantBegin(variantBegin) {}

    Alloc(ثابت Alloc& other, UError_کوڈ& حالت)
        : بھریا_Name(), ادھار_Name(), variantBegin(other.variantBegin) {
        إذا (U_SUCCESS(حالت)) {
            إذا (!other.بھریا_Name.is_خالی_تھاواں()) {
                بھریا_Name = other.بھریا_Name;
                إذا (بھریا_Name.is_خالی_تھاواں()) {
                    حالت = U_حافظہ_ونڈ_ERROR;
                } وإلا {
                    إذا (!other.ادھار_Name.is_خالی_تھاواں()) {
                        ادھار_Name = other.ادھار_Name;
                        إذا (ادھار_Name.is_خالی_تھاواں()) {
                            حالت = U_حافظہ_ونڈ_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = مٹاؤ;

    ~Alloc() = افتراضي;
};

ثابت حرف* Locale::ڈھیر_2::getVariant() ثابت U_LIFETIME_BOUND { أرجع ptr->getVariant(); }
ثابت حرف* Locale::ڈھیر_2::get_بھریا_Name() ثابت U_LIFETIME_BOUND { أرجع ptr->get_بھریا_Name(); }
ثابت حرف* Locale::ڈھیر_2::get_ادھار_Name() ثابت U_LIFETIME_BOUND { أرجع ptr->get_ادھار_Name(); }

Locale::ڈھیر_2::ڈھیر_2(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = جديد Alloc(variantBegin);
    إذا (ptr == خالی_سوچک) {
        type = eBOGUS;
    } وإلا {
        type = e_ڈھیر;
        copy_منزل_Array<&ڈھیر_2::language>(language, هذا);
        copy_منزل_Array<&ڈھیر_2::script>(script, هذا);
        copy_منزل_Array<&ڈھیر_2::region>(region, هذا);
    }
}

Locale::ڈھیر_2::~ڈھیر_2() {
    U_ASSERT(type == e_ڈھیر);
    مٹاؤ ptr;
}

Locale::ڈھیر_2& Locale::ڈھیر_2::عامل=(ثابت ڈھیر_2& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UError_کوڈ حالت = U_ZERO_ERROR;
    ptr = جديد Alloc(*other.ptr, حالت);
    إذا (ptr == خالی_سوچک || U_FAILURE(حالت)) {
        مٹاؤ ptr;
    } وإلا {
        type = e_ڈھیر;
        uprv_memcpy(language, other.language, حجم language);
        uprv_memcpy(script, other.script, حجم script);
        uprv_memcpy(region, other.region, حجم region);
    }
    أرجع *هذا;
}

Locale::ڈھیر_2& Locale::ڈھیر_2::عامل=(ڈھیر_2&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = e_ڈھیر;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, حجم language);
    uprv_memcpy(script, other.script, حجم script);
    uprv_memcpy(region, other.region, حجم region);
    أرجع *هذا;
}

سانچا <قسم_ناں BogusFn, قسم_ناں NestFn, قسم_ناں ڈھیر_Fn, قسم_ناں... دلیلاں>
خودکار Locale::Payload::ویکھو(BogusFn bogusFn, NestFn nestFn, ڈھیر_Fn ڈھیر_Fn_2, دلیلاں... دلیلاں_2) ثابت {
    بدل (type) {
        حالة eBOGUS:
            أرجع bogusFn(دلیلاں_2...);
        حالة eNEST:
            أرجع nestFn(nest, دلیلاں_2...);
        حالة e_ڈھیر:
            أرجع ڈھیر_Fn_2(ڈھیر_3, دلیلاں_2...);
        افتراضي:
            UPRV_UNREACHABLE_EXIT;
    };
}

فراغ Locale::Payload::copy(ثابت Payload& other) {
    other.ویکھو([](Payload*) {},
                [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ثابت ڈھیر_2& ڈھیر_3, Payload* dst) { dst->ڈھیر_3 = ڈھیر_3; },
                هذا);
}

فراغ Locale::Payload::move(Payload&& other) noexcept {
    other.ویکھو(
        [](Payload*) {},
        [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ثابت ڈھیر_2& ڈھیر_3, Payload* dst) { dst->ڈھیر_3 = std::move(const_cast<ڈھیر_2&>(ڈھیر_3)); },
        هذا);
}

Locale::Payload::~Payload() {
    إذا (type == e_ڈھیر) { ڈھیر_3.~ڈھیر_2(); }
}

Locale::Payload::Payload(ثابت Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::عامل=(ثابت Payload& other) U_LIFETIME_BOUND {
    إذا (هذا != &other) {
        سیٹ_منزل_Bogus();
        copy(other);
    }
    أرجع *هذا;
}

Locale::Payload& Locale::Payload::عامل=(Payload&& other) noexcept U_LIFETIME_BOUND {
    إذا (هذا != &other) {
        سیٹ_منزل_Bogus();
        move(std::move(other));
    }
    أرجع *هذا;
}

فراغ Locale::Payload::سیٹ_منزل_Bogus() {
    هذا->~Payload();
    type = eBOGUS;
}

سانچا <قسم_ناں T, قسم_ناں... دلیلاں> T& Locale::Payload::emplace(دلیلاں&&... دلیلاں_2) U_LIFETIME_BOUND {
    إذا constexpr (std::is_same_v<T, Nest>) {
        هذا->~Payload();
        ::جديد (&nest) Nest(std::forward<دلیلاں>(دلیلاں_2)...);
        أرجع nest;
    }
    إذا constexpr (std::is_same_v<T, ڈھیر_2>) {
        U_ASSERT(type != e_ڈھیر);
        ::جديد (&ڈھیر_3) ڈھیر_2(std::forward<دلیلاں>(دلیلاں_2)...);
        أرجع ڈھیر_3;
    }
}

سانچا <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    أرجع type == eNEST ? &nest : خالی_سوچک;
}
سانچا <> Locale::ڈھیر_2* Locale::Payload::get() U_LIFETIME_BOUND {
    أرجع type == e_ڈھیر ? &ڈھیر_3 : خالی_سوچک;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#تعریف_کرو SEP_CHAR '_'
#تعریف_کرو NULL_CHAR '\0'

Locale::~Locale() = افتراضي;

Locale::Locale()
    : UObject(), payload()
{
    init(خالی_سوچک, خطأ);
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
    إذا( (newLanguage==خالی_سوچک) && (newCountry == خالی_سوچک) && (newVariant == خالی_سوچک) )
    {
        init(خالی_سوچک, خطأ); /* shortcut */
    }
    وإلا
    {
        UError_کوڈ حالت = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        إذا ( newLanguage != خالی_سوچک )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            إذا ( lsize < 0 || lsize > ULOC_STRING_حد_بندی ) { // int32 wrap
                أرجع;
            }
        }

        CharString togo(newLanguage, lsize, حالت); // start with newLanguage

        // _Country
        إذا ( newCountry != خالی_سوچک )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            إذا ( csize < 0 || csize > ULOC_STRING_حد_بندی ) { // int32 wrap
                أرجع;
            }
        }

        // _Variant
        إذا ( newVariant != خالی_سوچک )
        {
            // remove leading _'s
            طالما(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            إذا ( vsize < 0 || vsize > ULOC_STRING_حد_بندی ) { // int32 wrap
                أرجع;
            }
            طالما( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        إذا ( newKeywords != خالی_سوچک)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            إذا ( ksize < 0 || ksize > ULOC_STRING_حد_بندی ) {
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
        init(togo.انکڑے(), خطأ);
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
UHashtable *g_معلوم_Canonicalized = خالی_سوچک;

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

فراغ U_CALLCONV لدو_معلوم_Canonicalized(UError_کوڈ &حالت) {
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE_معلوم_CANONICALIZED,
                                cleanup_معلوم_Canonicalized);
    Local_UHashtable_اشارہ_گر new_معلوم_Canonicalized_Map(
        uhash_open(uhash_hashChars, uhash_compareChars, خالی_سوچک, &حالت));
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

صنف Alias_انکڑے;

/**
 * A Builder class to build the alias data.
 */
صنف Alias_انکڑے_Builder {
عام:
    Alias_انکڑے_Builder() {
    }

    // Build the AliasData from resource.
    Alias_انکڑے* build(UError_کوڈ &حالت);

خاص:
    فراغ readAlias(UResourceBundle* alias,
                   Unique_Char_لڑیاں* لڑیاں,
                   Local_حافظہ<ثابت حرف*>& types,
                   Local_حافظہ<int32_t>& replacementIndexes,
                   int32_t &length,
                   فراغ (*checkType)(ثابت حرف* type),
                   فراغ (*checkReplacement)(ثابت UChar* replacement),
                   UError_کوڈ &حالت);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readLanguageAlias(UResourceBundle* alias,
                           Unique_Char_لڑیاں* لڑیاں,
                           Local_حافظہ<ثابت حرف*>& types,
                           Local_حافظہ<int32_t>& replacementIndexes,
                           int32_t &length,
                           UError_کوڈ &حالت);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readScriptAlias(UResourceBundle* alias,
                         Unique_Char_لڑیاں* لڑیاں,
                         Local_حافظہ<ثابت حرف*>& types,
                         Local_حافظہ<int32_t>& replacementIndexes,
                         int32_t &length, UError_کوڈ &حالت);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readTerritoryAlias(UResourceBundle* alias,
                            Unique_Char_لڑیاں* لڑیاں,
                            Local_حافظہ<ثابت حرف*>& types,
                            Local_حافظہ<int32_t>& replacementIndexes,
                            int32_t &length, UError_کوڈ &حالت);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    فراغ readVariantAlias(UResourceBundle* alias,
                          Unique_Char_لڑیاں* لڑیاں,
                          Local_حافظہ<ثابت حرف*>& types,
                          Local_حافظہ<int32_t>& replacementIndexes,
                          int32_t &length, UError_کوڈ &حالت);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    فراغ readSubdivisionAlias(UResourceBundle* alias,
                          Unique_Char_لڑیاں* لڑیاں,
                          Local_حافظہ<ثابت حرف*>& types,
                          Local_حافظہ<int32_t>& replacementIndexes,
                          int32_t &length, UError_کوڈ &حالت);
};

/**
 * A class to hold the Alias Data.
 */
صنف Alias_انکڑے : عام UMemory {
عام:
    ساكن ثابت Alias_انکڑے* singleton(UError_کوڈ& حالت) {
        إذا (U_FAILURE(حالت)) {
            // Do not get into loadData if the status already has error.
            أرجع خالی_سوچک;
        }
        umtx_initOnce(Alias_انکڑے::gInitOnce, &Alias_انکڑے::لدو_انکڑے, حالت);
        أرجع gSingleton;
    }

    ثابت CharStringMap& languageMap() ثابت U_LIFETIME_BOUND { أرجع language; }
    ثابت CharStringMap& scriptMap() ثابت U_LIFETIME_BOUND { أرجع script; }
    ثابت CharStringMap& territoryMap() ثابت U_LIFETIME_BOUND { أرجع territory; }
    ثابت CharStringMap& variantMap() ثابت U_LIFETIME_BOUND { أرجع variant; }
    ثابت CharStringMap& subdivisionMap() ثابت U_LIFETIME_BOUND { أرجع subdivision; }

    ساكن فراغ U_CALLCONV لدو_انکڑے(UError_کوڈ &حالت);
    ساكن UBool U_CALLCONV cleanup();

    ساكن UInitOnce gInitOnce;

خاص:
    Alias_انکڑے(CharStringMap languageMap,
              CharStringMap scriptMap,
              CharStringMap territoryMap,
              CharStringMap variantMap,
              CharStringMap subdivisionMap,
              CharString* لڑیاں)
        : language(std::move(languageMap)),
          script(std::move(scriptMap)),
          territory(std::move(territoryMap)),
          variant(std::move(variantMap)),
          subdivision(std::move(subdivisionMap)),
          لڑیاں(لڑیاں) {
    }

    ~Alias_انکڑے() {
        مٹاؤ لڑیاں;
    }

    ساكن ثابت Alias_انکڑے* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* لڑیاں;

    یار صنف Alias_انکڑے_Builder;
};


ثابت Alias_انکڑے* Alias_انکڑے::gSingleton = خالی_سوچک;
UInitOnce Alias_انکڑے::gInitOnce {};

UBool U_CALLCONV
Alias_انکڑے::cleanup()
{
    gInitOnce.reset();
    مٹاؤ gSingleton;
    أرجع سچ;
}

فراغ
Alias_انکڑے_Builder::readAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        فراغ (*checkType)(ثابت حرف* type),
        فراغ (*checkReplacement)(ثابت UChar* replacement),
        UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    length = ures_get_ناپ(alias);
    ثابت حرف** rawTypes = types.allocateInsteadAndCopy(length);
    إذا (rawTypes == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    إذا (rawIndexes == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    لكل (صحيح i = 0; U_SUCCESS(حالت) && ures_has_اگلا(alias); i++) {
        Local_UResource_Bundle_اشارہ_گر res(
            ures_get_اگلا_Resource(alias, خالی_سوچک, &حالت));
        ثابت حرف* aliasFrom = ures_get_کلید(res.getAlias());
        ثابت UChar* alias_منزل =
            ures_get_String_By_کلید(res.getAlias(), "replacement", خالی_سوچک, &حالت);
        إذا (U_FAILURE(حالت)) أرجع;

        checkType(aliasFrom);
        checkReplacement(alias_منزل);

        rawTypes[i] = aliasFrom;
        rawIndexes[i] = لڑیاں->جوڑو(alias_منزل, حالت);
    }
}

/**
 * Read the languageAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement language.
 */
فراغ
Alias_انکڑے_Builder::readLanguageAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کوڈ &حالت)
{
    أرجع readAlias(
        alias, لڑیاں, types, replacementIndexes, length,
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
#جے_انت
        [](ثابت UChar*) {}, حالت);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
فراغ
Alias_انکڑے_Builder::readScriptAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کوڈ &حالت)
{
    أرجع readAlias(
        alias, لڑیاں, types, replacementIndexes, length,
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
#جے_انت
        حالت);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
فراغ
Alias_انکڑے_Builder::readTerritoryAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کوڈ &حالت)
{
    أرجع readAlias(
        alias, لڑیاں, types, replacementIndexes, length,
#إذا U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#وإلا
        [](ثابت حرف*) {},
#جے_انت
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
Alias_انکڑے_Builder::readVariantAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کوڈ &حالت)
{
    أرجع readAlias(
        alias, لڑیاں, types, replacementIndexes, length,
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
#جے_انت
        حالت);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
فراغ
Alias_انکڑے_Builder::readSubdivisionAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کوڈ &حالت)
{
    أرجع readAlias(
        alias, لڑیاں, types, replacementIndexes, length,
#إذا U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#وإلا
        [](ثابت حرف*) {},
#جے_انت
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
Alias_انکڑے::لدو_انکڑے(UError_کوڈ &حالت)
{
#جے_تعریف_شدہ LOCALE_CANONICALIZATION_DEBUG
    UDate شروع = uprv_getRawUTCtime();
#جے_انت  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    Alias_انکڑے_Builder builder;
    gSingleton = builder.build(حالت);
#جے_تعریف_شدہ LOCALE_CANONICALIZATION_DEBUG
    UDate سرا = uprv_getRawUTCtime();
    لکھو("AliasData::loadData took total %f ms\n", سرا - شروع);
#جے_انت  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
Alias_انکڑے*
Alias_انکڑے_Builder::build(UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع خالی_سوچک; }

    Local_UResource_Bundle_اشارہ_گر metadata(
        ures_open_سدھا(خالی_سوچک, "metadata", &حالت));
    Local_UResource_Bundle_اشارہ_گر metadataAlias(
        ures_get_By_کلید(metadata.getAlias(), "alias", خالی_سوچک, &حالت));
    Local_UResource_Bundle_اشارہ_گر languageAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "language", خالی_سوچک, &حالت));
    Local_UResource_Bundle_اشارہ_گر scriptAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "script", خالی_سوچک, &حالت));
    Local_UResource_Bundle_اشارہ_گر territoryAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "territory", خالی_سوچک, &حالت));
    Local_UResource_Bundle_اشارہ_گر variantAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "variant", خالی_سوچک, &حالت));
    Local_UResource_Bundle_اشارہ_گر subdivisionAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "subdivision", خالی_سوچک, &حالت));

    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    int32_t languages_لمائی = 0, script_لمائی = 0, territory_لمائی = 0,
            variant_لمائی = 0, subdivision_لمائی = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    Unique_Char_لڑیاں لڑیاں(حالت);
    Local_حافظہ<ثابت حرف*> languageTypes;
    Local_حافظہ<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &لڑیاں,
                      languageTypes,
                      languageReplacementIndexes,
                      languages_لمائی,
                      حالت);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    Local_حافظہ<ثابت حرف*> scriptTypes;
    Local_حافظہ<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &لڑیاں,
                    scriptTypes,
                    scriptReplacementIndexes,
                    script_لمائی,
                    حالت);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    Local_حافظہ<ثابت حرف*> territoryTypes;
    Local_حافظہ<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &لڑیاں,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territory_لمائی, حالت);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    Local_حافظہ<ثابت حرف*> variantTypes;
    Local_حافظہ<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &لڑیاں,
                     variantTypes,
                     variantReplacementIndexes,
                     variant_لمائی, حالت);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    Local_حافظہ<ثابت حرف*> subdivisionTypes;
    Local_حافظہ<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &لڑیاں,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivision_لمائی, حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    // We can only use strings after freeze it.
    لڑیاں.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < languages_لمائی; i++) {
        languageMap.put(languageTypes[i],
                        لڑیاں.get(languageReplacementIndexes[i]),
                        حالت);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < script_لمائی; i++) {
        scriptMap.put(scriptTypes[i],
                      لڑیاں.get(scriptReplacementIndexes[i]),
                      حالت);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < territory_لمائی; i++) {
        territoryMap.put(territoryTypes[i],
                         لڑیاں.get(territoryReplacementIndexes[i]),
                         حالت);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < variant_لمائی; i++) {
        variantMap.put(variantTypes[i],
                       لڑیاں.get(variantReplacementIndexes[i]),
                       حالت);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, حالت);
    لكل (int32_t i = 0; U_SUCCESS(حالت) && i < subdivision_لمائی; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       لڑیاں.get(subdivisionReplacementIndexes[i]),
                       حالت);
    }

    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    // copy hashtables
    خودکار *انکڑے = جديد Alias_انکڑے(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        لڑیاں.orphan_Char_لڑیاں());

    إذا (انکڑے == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
    }
    أرجع انکڑے;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
صنف AliasReplacer {
عام:
    AliasReplacer(UError_کوڈ& حالت) :
            language(خالی_سوچک), script(خالی_سوچک), region(خالی_سوچک),
            extensions(خالی_سوچک),
            // store value in variants only once
            variants(خالی_سوچک,
                     ([](UElement e1, UElement e2) -> UBool {
                       أرجع 0==uprv_strcmp((ثابت حرف*)e1.اشارہ_گر,
                                             (ثابت حرف*)e2.اشارہ_گر);}),
                     حالت),
            انکڑے(خالی_سوچک) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    منطقي replace(
        ثابت Locale& locale, CharString& out, UError_کوڈ& حالت);

خاص:
    ثابت حرف* language;
    ثابت حرف* script;
    ثابت حرف* region;
    ثابت حرف* extensions;
    UVector variants;

    ثابت Alias_انکڑے* انکڑے;

    مضمن منطقي not_خالی_تھاواں(ثابت حرف* str) {
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
        أرجع not_خالی_تھاواں(replacement) ?
            ((input == خالی_سوچک) ?  replacement : input) :
            ((type == خالی_سوچک) ? input  : خالی_سوچک);
    }

    مضمن منطقي same(ثابت حرف* a, ثابت حرف* b) {
        إذا (a == خالی_سوچک && b == خالی_سوچک) {
            أرجع سچ;
        }
        إذا ((a == خالی_سوچک && b != خالی_سوچک) ||
            (a != خالی_سوچک && b == خالی_سوچک)) {
          أرجع خطأ;
        }
        أرجع uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& output_منزل_String(CharString& out U_LIFETIME_BOUND, UError_کوڈ& حالت);

    // Generate the lookup key.
    CharString& generate_کلید(ثابت حرف* language, ثابت حرف* region,
                            ثابت حرف* variant, CharString& out U_LIFETIME_BOUND,
                            UError_کوڈ& حالت);

    فراغ parseLanguageReplacement(ثابت حرف* replacement,
                                  ثابت حرف*& replaceLanguage,
                                  ثابت حرف*& replaceScript,
                                  ثابت حرف*& replaceRegion,
                                  ثابت حرف*& replaceVariant,
                                  ثابت حرف*& replaceExtensions,
                                  UVector& منزل_Be_Freed,
                                  UError_کوڈ& حالت);

    // Replace by using languageAlias.
    منطقي replaceLanguage(منطقي checkLanguage, منطقي checkRegion,
                         منطقي checkVariants, UVector& منزل_Be_Freed,
                         UError_کوڈ& حالت);

    // Replace by using territoryAlias.
    منطقي replaceTerritory(UVector& منزل_Be_Freed, UError_کوڈ& حالت);

    // Replace by using scriptAlias.
    منطقي replaceScript(UError_کوڈ& حالت);

    // Replace by using variantAlias.
    منطقي replaceVariant(UError_کوڈ& حالت);

    // Replace by using subdivisionAlias.
    منطقي replaceSubdivision(StringPiece subdivision,
                            CharString& output, UError_کوڈ& حالت);

    // Replace transformed extensions.
    منطقي replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UError_کوڈ& حالت);
};

CharString&
AliasReplacer::generate_کلید(
        ثابت حرف* language, ثابت حرف* region, ثابت حرف* variant,
        CharString& out U_LIFETIME_BOUND, UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع out; }
    out.append(language, حالت);
    إذا (not_خالی_تھاواں(region)) {
        out.append(SEP_CHAR, حالت)
            .append(region, حالت);
    }
    إذا (not_خالی_تھاواں(variant)) {
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
    UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = خالی_سوچک;
    إذا (uprv_strchr(replacement, '_') == خالی_سوچک) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        أرجع;
    }
    // We have multiple field so we have to allocate and parse
    خودکار str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), حالت, حالت);
    حرف* انکڑے = str->انکڑے();
    منزل_Be_Freed.adoptElement(str.release(), حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    replacedLanguage = (ثابت حرف*) انکڑے;
    حرف* سرا_دا_میدان = uprv_strchr(انکڑے, '_');
    *سرا_دا_میدان = '\0'; // null terminiate it.
    سرا_دا_میدان++;
    ثابت حرف* شروع = سرا_دا_میدان;
    سرا_دا_میدان = const_cast<حرف*>(uprv_strchr(شروع, '_'));
    size_t len = 0;
    إذا (سرا_دا_میدان == خالی_سوچک) {
        len = uprv_strlen(شروع);
    } وإلا {
        len = سرا_دا_میدان - شروع;
        *سرا_دا_میدان = '\0'; // null terminiate it.
    }
    إذا (len == 4 && uprv_isASCIILetter(*شروع)) {
        // Got a script
        replacedScript = شروع;
        إذا (سرا_دا_میدان == خالی_سوچک) {
            أرجع;
        }
        شروع = سرا_دا_میدان++;
        سرا_دا_میدان = const_cast<حرف*>(uprv_strchr(شروع, '_'));
        إذا (سرا_دا_میدان == خالی_سوچک) {
            len = uprv_strlen(شروع);
        } وإلا {
            len = سرا_دا_میدان - شروع;
            *سرا_دا_میدان = '\0'; // null terminiate it.
        }
    }
    إذا (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = شروع;
        إذا (سرا_دا_میدان == خالی_سوچک) {
            أرجع;
        }
        شروع = سرا_دا_میدان++;
        سرا_دا_میدان = const_cast<حرف*>(uprv_strchr(شروع, '_'));
        إذا (سرا_دا_میدان == خالی_سوچک) {
            len = uprv_strlen(شروع);
        } وإلا {
            len = سرا_دا_میدان - شروع;
            *سرا_دا_میدان = '\0'; // null terminiate it.
        }
    }
    إذا (len >= 4) {
        // Got a variant
        replacedVariant = شروع;
        إذا (سرا_دا_میدان == خالی_سوچک) {
            أرجع;
        }
        شروع = سرا_دا_میدان++;
    }
    replacedExtensions = شروع;
}

منطقي
AliasReplacer::replaceLanguage(
        منطقي checkLanguage, منطقي checkRegion,
        منطقي checkVariants, UVector& منزل_Be_Freed, UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (    (checkRegion && region == خالی_سوچک) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        أرجع خطأ;
    }
    int32_t variant_ناپ = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ثابت حرف* تلاش_Language = checkLanguage ? language : "und";
    ثابت حرف* تلاش_Region = checkRegion ? region : خالی_سوچک;
    ثابت حرف* تلاش_Variant = خالی_سوچک;
    لكل (int32_t variant_اشاریہ = 0;
            variant_اشاریہ < variant_ناپ;
            variant_اشاریہ++) {
        إذا (checkVariants) {
            U_ASSERT(variant_اشاریہ < variant_ناپ);
            تلاش_Variant = static_cast<ثابت حرف*>(variants.elementAt(variant_اشاریہ));
        }

        إذا (تلاش_Variant != خالی_سوچک && uprv_strlen(تلاش_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            تلاش_Variant = خالی_سوچک;
        }
        CharString type_کلید;
        generate_کلید(تلاش_Language, تلاش_Region, تلاش_Variant, type_کلید,
                    حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خطأ;
        }
        ثابت حرف *replacement = انکڑے->languageMap().get(type_کلید.انکڑے());
        إذا (replacement == خالی_سوچک) {
            // Found no replacement data.
            استمر;
        }

        ثابت حرف* replacedLanguage = خالی_سوچک;
        ثابت حرف* replacedScript = خالی_سوچک;
        ثابت حرف* replacedRegion = خالی_سوچک;
        ثابت حرف* replacedVariant = خالی_سوچک;
        ثابت حرف* replacedExtensions = خالی_سوچک;
        parseLanguageReplacement(replacement,
                                 replacedLanguage,
                                 replacedScript,
                                 replacedRegion,
                                 replacedVariant,
                                 replacedExtensions,
                                 منزل_Be_Freed,
                                 حالت);
        replacedLanguage =
            (replacedLanguage != خالی_سوچک && uprv_strcmp(replacedLanguage, "und") == 0) ?
            language : replacedLanguage;
        replacedScript = deleteOrReplace(script, خالی_سوچک, replacedScript);
        replacedRegion = deleteOrReplace(region, تلاش_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            تلاش_Variant, تلاش_Variant, replacedVariant);

        إذا (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(تلاش_Variant, replacedVariant) &&
                replacedExtensions == خالی_سوچک) {
            // Replacement produce no changes.
            استمر;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        إذا (تلاش_Variant != خالی_سوچک) {
            إذا (not_خالی_تھاواں(replacedVariant)) {
                variants.سیٹ_Element_At((فراغ*)replacedVariant, variant_اشاریہ);
            } وإلا {
                variants.removeElementAt(variant_اشاریہ);
            }
        }
        إذا (replacedExtensions != خالی_سوچک) {
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
AliasReplacer::replaceTerritory(UVector& منزل_Be_Freed, UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (region == خالی_سوچک) {
        // No region to search.
        أرجع خطأ;
    }
    ثابت حرف *replacement = انکڑے->territoryMap().get(region);
    إذا (replacement == خالی_سوچک) {
        // Found no replacement data for this region.
        أرجع خطأ;
    }
    ثابت حرف* replacedRegion = replacement;
    ثابت حرف* پہلا_Space = uprv_strchr(replacement, ' ');
    إذا (پہلا_Space != خالی_سوچک) {
        // If there are are more than one region in the replacement.
        // We need to check which one match based on the language.
        // Cannot use nullptr for language because that will construct
        // the default locale, in that case, use "und" to get the correct
        // locale.
        Locale l = LocaleBuilder()
            .سیٹ_Language(language == خالی_سوچک ? "und" : language)
            .سیٹ_Script(script)
            .build(حالت);
        l.جوڑو_Likely_Subtags(حالت);
        ثابت حرف* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> شے;
        إذا (likelyRegion != خالی_سوچک && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ثابت حرف* لبھیا_وچ_Replacement = uprv_strstr(replacement,
                                                         likelyRegion);
            إذا (لبھیا_وچ_Replacement != خالی_سوچک) {
                // Assuming the case there are no three letter region code in
                // the replacement of territoryAlias
                U_ASSERT(لبھیا_وچ_Replacement == replacement ||
                         *(لبھیا_وچ_Replacement-1) == ' ');
                U_ASSERT(لبھیا_وچ_Replacement[len] == ' ' ||
                         لبھیا_وچ_Replacement[len] == '\0');
                شے = prv::make_unique<CharString>(
                    لبھیا_وچ_Replacement, static_cast<int32_t>(len), حالت, حالت);
            }
        }
        إذا (!شے && U_SUCCESS(حالت)) {
            شے = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(پہلا_Space - replacement), حالت, حالت);
        }
        إذا (U_FAILURE(حالت)) { أرجع خطأ; }
        replacedRegion = شے->انکڑے();
        منزل_Be_Freed.adoptElement(شے.release(), حالت);
        إذا (U_FAILURE(حالت)) { أرجع خطأ; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    أرجع سچ;
}

منطقي
AliasReplacer::replaceScript(UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (script == خالی_سوچک) {
        // No script to search.
        أرجع خطأ;
    }
    ثابت حرف *replacement = انکڑے->scriptMap().get(script);
    إذا (replacement == خالی_سوچک) {
        // Found no replacement data for this script.
        أرجع خطأ;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    أرجع سچ;
}

منطقي
AliasReplacer::replaceVariant(UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    // Since we may have more than one variant, we need to loop through them.
    لكل (int32_t i = 0; i < variants.size(); i++) {
        ثابت حرف* variant = static_cast<ثابت حرف*>(variants.elementAt(i));
        ثابت حرف *replacement = انکڑے->variantMap().get(variant);
        إذا (replacement == خالی_سوچک) {
            // Found no replacement data for this variant.
            استمر;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        إذا (!same(variant, replacement)) {
            variants.سیٹ_Element_At((فراغ*)replacement, i);
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
    StringPiece subdivision, CharString& output, UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    ثابت حرف *replacement = انکڑے->subdivisionMap().get(subdivision.انکڑے());
    إذا (replacement != خالی_سوچک) {
        ثابت حرف* پہلا_Space = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (پہلا_Space != خالی_سوچک) ?
            (پہلا_Space - replacement) : uprv_strlen(replacement);
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
    CharString& transformedExtensions, CharString& output, UError_کوڈ& حالت)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    int32_t len = transformedExtensions.length();
    ثابت حرف* str = transformedExtensions.انکڑے();
    ثابت حرف* tkey = ultag_get_TKey_شروع(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == خالی_سوچک) ? len : static_cast<int32_t>((tkey - str - 1)));
    إذا (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .سیٹ_Language_ٹیگ(StringPiece(str, tlangLen))
            .build(حالت);
        tlang.canonicalize(حالت);
        output = tlang.منزل_Language_ٹیگ<CharString>(حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خطأ;
        }
        T_CString_منزل_ہیٹھلا_Case(output.انکڑے());
    }
    إذا (tkey != خالی_سوچک) {
        // We need to sort the tfields by tkey
        UVector tfields(حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خطأ;
        }
        افعل {
            ثابت حرف* tvalue = uprv_strchr(tkey, '-');
            إذا (tvalue == خالی_سوچک) {
                حالت = U_ILLEGAL_دلیل_ERROR;
                أرجع خطأ;
            }
            ثابت حرف* اگلا_TKey = ultag_get_TKey_شروع(tvalue);
            إذا (اگلا_TKey != خالی_سوچک) {
                *const_cast<حرف*>(اگلا_TKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.داخل_کرو_Element_At((فراغ*)tkey, tfields.size(), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع خطأ;
            }
            tkey = اگلا_TKey;
        } طالما (tkey != خالی_سوچک);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            أرجع uprv_strcmp((ثابت حرف*)e1.اشارہ_گر, (ثابت حرف*)e2.اشارہ_گر);
        }, حالت);
        لكل (int32_t i = 0; i < tfields.size(); i++) {
             إذا (output.length() > 0) {
                 output.append('-', حالت);
             }
             ثابت حرف* tfield = static_cast<ثابت حرف*>(tfields.elementAt(i));
             ثابت حرف* tvalue = uprv_strchr(tfield, '-');
             إذا (tvalue == خالی_سوچک) {
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
    أرجع سچ;
}

CharString&
AliasReplacer::output_منزل_String(
    CharString& out U_LIFETIME_BOUND, UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع out; }
    out.append(language, حالت);
    إذا (not_خالی_تھاواں(script)) {
        out.append(SEP_CHAR, حالت)
            .append(script, حالت);
    }
    إذا (not_خالی_تھاواں(region)) {
        out.append(SEP_CHAR, حالت)
            .append(region, حالت);
    }
    إذا (variants.size() > 0) {
        إذا (!not_خالی_تھاواں(script) && !not_خالی_تھاواں(region)) {
          out.append(SEP_CHAR, حالت);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            أرجع uprv_strcmp((ثابت حرف*)e1.اشارہ_گر, (ثابت حرف*)e2.اشارہ_گر);
        }, حالت);
        int32_t variants_شروع = out.length();
        لكل (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, حالت)
                 .append(static_cast<ثابت حرف*>(variants.elementAt(i)),
                         حالت);
        }
        T_CString_منزل_اتلا_Case(out.انکڑے() + variants_شروع);
    }
    إذا (not_خالی_تھاواں(extensions)) {
        CharString tmp("und_", حالت);
        tmp.append(extensions, حالت);
        Locale tmpLocale(tmp.انکڑے());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, حالت);
    }
    أرجع out;
}

منطقي
AliasReplacer::replace(ثابت Locale& locale, CharString& out, UError_کوڈ& حالت)
{
    انکڑے = Alias_انکڑے::singleton(حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    U_ASSERT(انکڑے != خالی_سوچک);
    out.clear();
    language = locale.getLanguage();
    إذا (!not_خالی_تھاواں(language)) {
        language = خالی_سوچک;
    }
    script = locale.getScript();
    إذا (!not_خالی_تھاواں(script)) {
        script = خالی_سوچک;
    }
    region = locale.getCountry();
    إذا (!not_خالی_تھاواں(region)) {
        region = خالی_سوچک;
    }
    ثابت حرف* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, حالت);
    إذا (!variantsBuff.is_خالی_تھاواں()) {
        إذا (U_FAILURE(حالت)) { أرجع خطأ; }
        حرف* شروع = variantsBuff.انکڑے();
        T_CString_منزل_ہیٹھلا_Case(شروع);
        حرف* سرا;
        طالما ((سرا = uprv_strchr(شروع, SEP_CHAR)) != خالی_سوچک &&
               U_SUCCESS(حالت)) {
            *سرا = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            إذا (*شروع && !variants.شامل_اے(شروع)) {
                variants.جوڑو_Element(شروع, حالت);
            }
            شروع = سرا + 1;
        }
        // do not add "" or duplicate data to variants
        إذا (*شروع && !variants.شامل_اے(شروع)) {
            variants.جوڑو_Element(شروع, حالت);
        }
    }
    إذا (U_FAILURE(حالت)) { أرجع خطأ; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        أرجع uprv_strcmp((ثابت حرف*)e1.اشارہ_گر, (ثابت حرف*)e2.اشارہ_گر);
    }, حالت);

    // A changed count to assert when loop too many times.
    int32_t بدلیا = 0;
    ثابت int32_t max_بدلیا = 4 + variants.size();
    (فراغ)max_بدلیا; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector لڑیاں_منزل_Be_Freed([](فراغ *obj) { مٹاؤ static_cast<CharString*>(obj); },
                             خالی_سوچک, 10, حالت);
    طالما (U_SUCCESS(حالت)) {
        // Something wrong with the data cause looping here more than maxChanged
        // times already.
        U_ASSERT(بدلیا <= max_بدلیا);
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
        إذا (    replaceLanguage(سچ, سچ,  سچ,  لڑیاں_منزل_Be_Freed, حالت) ||
                replaceLanguage(سچ, سچ,  خطأ, لڑیاں_منزل_Be_Freed, حالت) ||
                replaceLanguage(سچ, خطأ, سچ,  لڑیاں_منزل_Be_Freed, حالت) ||
                replaceLanguage(سچ, خطأ, خطأ, لڑیاں_منزل_Be_Freed, حالت) ||
                replaceLanguage(خطأ,خطأ, سچ,  لڑیاں_منزل_Be_Freed, حالت) ||
                replaceTerritory(لڑیاں_منزل_Be_Freed, حالت) ||
                replaceScript(حالت) ||
                replaceVariant(حالت)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            بدلیا++;
            استمر;
        }
        // Nothing changed. Break out.
        اكسر;
    }  // while(1)

    إذا (U_FAILURE(حالت)) { أرجع خطأ; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ثابت حرف* extensionsStr = locale_get_Keywords_شروع(locale.getName());
    إذا (بدلیا == 0 && variants.size() <= 1 && extensionsStr == خالی_سوچک) {
        أرجع خطأ;
    }
    output_منزل_String(out, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (extensionsStr != خالی_سوچک) {
        بدلیا = 0;
        Locale عارضی(locale);
        Local_اشارہ_گر<icu::StringEnumeration> iter(locale.createKeywords(حالت));
        إذا (U_SUCCESS(حالت) && !iter.isNull()) {
            ثابت حرف* کلید;
            طالما ((کلید = iter->next(خالی_سوچک, حالت)) != خالی_سوچک) {
                إذا (uprv_strcmp("sd", کلید) == 0 || uprv_strcmp("rg", کلید) == 0 ||
                        uprv_strcmp("t", کلید) == 0) {
                    خودکار value = locale.get_Keyword_قدر<CharString>(کلید, حالت);
                    إذا (U_FAILURE(حالت)) {
                        حالت = U_ZERO_ERROR;
                        استمر;
                    }
                    CharString replacement;
                    إذا (uprv_strlen(کلید) == 2) {
                        إذا (replaceSubdivision(value.منزل_String_Piece(), replacement, حالت)) {
                            بدلیا++;
                            عارضی.سیٹ_Keyword_قدر(کلید, replacement.انکڑے(), حالت);
                        }
                    } وإلا {
                        U_ASSERT(uprv_strcmp(کلید, "t") == 0);
                        إذا (replaceTransformedExtensions(value, replacement, حالت)) {
                            بدلیا++;
                            عارضی.سیٹ_Keyword_قدر(کلید, replacement.انکڑے(), حالت);
                        }
                    }
                    إذا (U_FAILURE(حالت)) {
                        أرجع خطأ;
                    }
                }
            }
        }
        إذا (بدلیا != 0) {
            extensionsStr = locale_get_Keywords_شروع(عارضی.getName());
        }
        out.append(extensionsStr, حالت);
    }
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    // If the tag is not changed, return.
    إذا (uprv_strcmp(out.انکڑے(), locale.getName()) == 0) {
        out.clear();
        أرجع خطأ;
    }
    أرجع سچ;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
منطقي
canonicalizeLocale(ثابت Locale& locale, CharString& out, UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع خطأ; }
    AliasReplacer replacer(حالت);
    أرجع replacer.replace(locale, out, حالت);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
منطقي
is_معلوم_Canonicalized_Locale(ثابت حرف* locale, UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) { أرجع خطأ; }

    إذا (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        أرجع سچ;
    }

    // common well-known Canonicalized.
    umtx_initOnce(g_معلوم_Canonicalized_Init_Once,
                  &لدو_معلوم_Canonicalized, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    U_ASSERT(g_معلوم_Canonicalized != خالی_سوچک);
    أرجع uhash_geti(g_معلوم_Canonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_سرا

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
    UError_کوڈ حالت = U_ZERO_ERROR;
    CharString عارضی;
    أرجع !canonicalizeLocale(l, عارضی, حالت) && U_SUCCESS(حالت);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ثابت حرف* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    أرجع localeID == خالی_سوچک ? *هذا = getDefault() : init(StringPiece{localeID}, canonicalize);
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
        حرف *میدان_2[5] = {خالی_سوچک};
        int32_t میدان_Len[5] = {0};
        int32_t میدان_Idx;
        int32_t variant_میدان;
        int32_t length;
        UError_کوڈ err;

        ثابت خودکار parse = [canonicalize](std::string_view localeID,
                                          حرف* name,
                                          int32_t name_سمرتھا,
                                          UError_کوڈ& حالت) {
            أرجع ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
                name, name_سمرتھا,
                [&](ByteSink& sink, UError_کوڈ& حالت) {
                    إذا (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, حالت);
                    } وإلا {
                        ulocimp_getName(localeID, sink, حالت);
                    }
                },
                حالت);
        };

        // "canonicalize" the locale ID to ICU/Java format
        حرف* بھریا_Name = nest.ادھار_Name;
        err = U_ZERO_ERROR;
        length = parse(localeID, بھریا_Name, حجم Nest::ادھار_Name, err);

        FixedString بھریا_Name_بفر;
        إذا (err == U_بفر_حد_توں_ودھ_ERROR || length >= static_cast<int32_t>(حجم Nest::ادھار_Name)) {
            /*Go to heap for the fullName if necessary*/
            إذا (!بھریا_Name_بفر.reserve(length + 1)) {
                اكسر; // error: out of memory
            }
            بھریا_Name = بھریا_Name_بفر.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, بھریا_Name, length + 1, err);
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
        separator = میدان_2[0] = بھریا_Name;
        میدان_Idx = 1;
        حرف* at = uprv_strchr(بھریا_Name, '@');
        طالما ((separator = uprv_strchr(میدان_2[میدان_Idx-1], SEP_CHAR)) != خالی_سوچک &&
               میدان_Idx < UPRV_LENGTHOF(میدان_2)-1 &&
               (at == خالی_سوچک || separator < at)) {
            میدان_2[میدان_Idx] = separator + 1;
            میدان_Len[میدان_Idx - 1] = static_cast<int32_t>(separator - میدان_2[میدان_Idx - 1]);
            میدان_Idx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(میدان_2[میدان_Idx-1], '@');
        حرف* sep2 = uprv_strchr(میدان_2[میدان_Idx-1], '.');
        إذا (separator!=خالی_سوچک || sep2!=خالی_سوچک) {
            إذا (separator==خالی_سوچک || (sep2!=خالی_سوچک && separator > sep2)) {
                separator = sep2;
            }
            میدان_Len[میدان_Idx - 1] = static_cast<int32_t>(separator - میدان_2[میدان_Idx - 1]);
        } وإلا {
            میدان_Len[میدان_Idx - 1] = length - static_cast<int32_t>(میدان_2[میدان_Idx - 1] - بھریا_Name);
        }
        منطقي hasKeywords = at != خالی_سوچک && uprv_strchr(at + 1, '=') != خالی_سوچک;

        إذا (میدان_Len[0] >= ULOC_LANG_سمرتھا)
        {
            اكسر; // error: the language field is too long
        }

        variant_میدان = 1; /* Usually the 2nd one, except when a script or country is also used. */
        إذا (میدان_Len[0] > 0) {
            /* We have a language */
            language = {بھریا_Name, static_cast<std::string_view::size_type>(میدان_Len[0])};
        }
        إذا (میدان_Len[1] == 4 && uprv_isASCIILetter(میدان_2[1][0]) &&
                uprv_isASCIILetter(میدان_2[1][1]) && uprv_isASCIILetter(میدان_2[1][2]) &&
                uprv_isASCIILetter(میدان_2[1][3])) {
            /* We have at least a script */
            script = {میدان_2[1], static_cast<std::string_view::size_type>(میدان_Len[1])};
            variant_میدان++;
        }

        إذا (میدان_Len[variant_میدان] == 2 || میدان_Len[variant_میدان] == 3) {
            /* We have a country */
            region = {میدان_2[variant_میدان], static_cast<std::string_view::size_type>(میدان_Len[variant_میدان])};
            variant_میدان++;
        } وإلا إذا (میدان_Len[variant_میدان] == 0) {
            variant_میدان++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        إذا (میدان_Len[variant_میدان] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(میدان_2[variant_میدان] - بھریا_Name);
        } وإلا إذا (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - بھریا_Name);
        }

        إذا (!hasKeywords && Nest::سماندا(length, language, script, region)) {
            U_ASSERT(بھریا_Name == nest.ادھار_Name);
            U_ASSERT(بھریا_Name_بفر.is_خالی_تھاواں());
            nest.init(language, script, region, variantBegin);
        } وإلا {
            إذا (بھریا_Name == nest.ادھار_Name) {
                U_ASSERT(بھریا_Name_بفر.is_خالی_تھاواں());
                بھریا_Name_بفر = {بھریا_Name, static_cast<std::string_view::size_type>(length)};
                إذا (بھریا_Name_بفر.is_خالی_تھاواں()) {
                    اكسر; // error: out of memory
                }
                إذا (!language.empty()) {
                    language = {بھریا_Name_بفر.انکڑے(), language.size()};
                }
                إذا (!script.empty()) {
                    script = {بھریا_Name_بفر.انکڑے() + (script.انکڑے() - بھریا_Name), script.size()};
                }
                إذا (!region.empty()) {
                    region = {بھریا_Name_بفر.انکڑے() + (region.انکڑے() - بھریا_Name), region.size()};
                }
            }
            ڈھیر_2& ڈھیر_3 = payload.emplace<ڈھیر_2>(language, script, region, variantBegin);
            إذا (isBogus()) {
                اكسر; // error: out of memory
            }
            U_ASSERT(!بھریا_Name_بفر.is_خالی_تھاواں());
            ڈھیر_3.ptr->بھریا_Name = std::move(بھریا_Name_بفر);
            إذا (hasKeywords) {
                إذا (std::string_view::size_type ادھار_Name_لمائی = at - بھریا_Name; ادھار_Name_لمائی > 0) {
                    ڈھیر_3.ptr->ادھار_Name = {ڈھیر_3.ptr->بھریا_Name.انکڑے(), ادھار_Name_لمائی};
                    إذا (ڈھیر_3.ptr->ادھار_Name.is_خالی_تھاواں()) {
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
                    init(replaced.انکڑے(), خطأ);
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
    سیٹ_منزل_Bogus();

    أرجع *هذا;
}

int32_t
Locale::hash_کوڈ() ثابت
{
    أرجع ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

فراغ
Locale::سیٹ_منزل_Bogus() {
    /* Free our current storage */
    payload.سیٹ_منزل_Bogus();
}

ثابت Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex جندرا(&gDefaultLocaleMutex);
        إذا (gDefaultLocale != خالی_سوچک) {
            أرجع *gDefaultLocale;
        }
    }
    UError_کوڈ حالت = U_ZERO_ERROR;
    أرجع *locale_سیٹ_default_اندرونی(خالی_سوچک, حالت);
}



فراغ U_EXPORT2
Locale::سیٹ_Default( ثابت   Locale&     newLocale,
                            UError_کوڈ&  حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ثابت حرف *localeID = newLocale.getName();
    locale_سیٹ_default_اندرونی(localeID, حالت);
}

فراغ
Locale::جوڑو_Likely_Subtags(UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    CharString maximizedLocaleID = ulocimp_جوڑو_Likely_Subtags(getName(), حالت);

    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_حافظہ_ونڈ_ERROR) {
            سیٹ_منزل_Bogus();
        }
        أرجع;
    }

    init(maximizedLocaleID.انکڑے(), /*canonicalize=*/خطأ);
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
}

فراغ
Locale::minimizeSubtags(UError_کوڈ& حالت) {
    Locale::minimizeSubtags(خطأ, حالت);
}
فراغ
Locale::minimizeSubtags(منطقي favorScript, UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, حالت);

    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_حافظہ_ونڈ_ERROR) {
            سیٹ_منزل_Bogus();
        }
        أرجع;
    }

    init(minimizedLocaleID.انکڑے(), /*canonicalize=*/خطأ);
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
}

فراغ
Locale::canonicalize(UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    CharString uncanonicalized(getName(), حالت);
    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_حافظہ_ونڈ_ERROR) {
            سیٹ_منزل_Bogus();
        }
        أرجع;
    }
    init(uncanonicalized.انکڑے(), /*canonicalize=*/سچ);
    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
}

Locale U_EXPORT2
Locale::for_Language_ٹیگ(StringPiece ٹیگ, UError_کوڈ& حالت)
{
    Locale نتیجہ(Locale::eBOGUS);

    إذا (U_FAILURE(حالت)) {
        أرجع نتیجہ;
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

    int32_t parsed_لمائی;
    CharString localeID = ulocimp_for_Language_ٹیگ(
            ٹیگ.انکڑے(),
            ٹیگ.length(),
            &parsed_لمائی,
            حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع نتیجہ;
    }

    إذا (parsed_لمائی != ٹیگ.size()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع نتیجہ;
    }

    نتیجہ.init(localeID.انکڑے(), /*canonicalize=*/خطأ);
    إذا (نتیجہ.isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
    أرجع نتیجہ;
}

فراغ
Locale::منزل_Language_ٹیگ(ByteSink& sink, UError_کوڈ& حالت) ثابت
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    ulocimp_منزل_Language_ٹیگ(getName(), sink, /*strict=*/خطأ, حالت);
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
فراغ Locale::سیٹ_From_POSIXID(ثابت حرف *posixID)
{
    init(posixID, سچ);
}

ثابت Locale & U_EXPORT2
Locale::get_جڑ()
{
    أرجع getLocale(e_جڑ);
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
    إذا (localeCache == خالی_سوچک) {
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
    UError_کوڈ حالت = U_ZERO_ERROR;
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
    مجازی UClassID getDynamicClassID() ثابت override { أرجع getStaticClassID(); }
عام:
    KeywordEnumeration(ثابت حرف *کلیدیں, int32_t keywordLen, int32_t current_اشاریہ, UError_کوڈ &حالت)
        : keywords(), length(keywordLen), current(خالی_سوچک) {
        إذا(U_SUCCESS(حالت) && keywordLen != 0) {
            إذا(کلیدیں == خالی_سوچک || keywordLen < 0) {
                حالت = U_ILLEGAL_دلیل_ERROR;
            } وإلا {
                keywords = {کلیدیں, static_cast<std::string_view::size_type>(length)};
                إذا (keywords.is_خالی_تھاواں()) {
                    حالت = U_حافظہ_ونڈ_ERROR;
                } وإلا {
                    current = keywords.انکڑے() + current_اشاریہ;
                }
            }
        }
    }

    مجازی ~KeywordEnumeration();

    مجازی StringEnumeration * clone() ثابت override
    {
        UError_کوڈ حالت = U_ZERO_ERROR;
        أرجع جديد KeywordEnumeration(
                keywords.انکڑے(), length,
                static_cast<int32_t>(current - keywords.انکڑے()), حالت);
    }

    مجازی int32_t count(UError_کوڈ& حالت) ثابت override {
        إذا (U_FAILURE(حالت)) { أرجع 0; }
        ثابت حرف *kw = keywords.انکڑے();
        int32_t نتیجہ = 0;
        طالما(*kw) {
            نتیجہ++;
            kw += uprv_strlen(kw)+1;
        }
        أرجع نتیجہ;
    }

    مجازی ثابت حرف* next(int32_t* نتیجہ_لمائی, UError_کوڈ& حالت) U_LIFETIME_BOUND override {
        ثابت حرف* نتیجہ;
        int32_t len;
        إذا(U_SUCCESS(حالت) && *current != 0) {
            نتیجہ = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            إذا(نتیجہ_لمائی != خالی_سوچک) {
                *نتیجہ_لمائی = len;
            }
        } وإلا {
            إذا(نتیجہ_لمائی != خالی_سوچک) {
                *نتیجہ_لمائی = 0;
            }
            نتیجہ = خالی_سوچک;
        }
        أرجع نتیجہ;
    }

    مجازی ثابت UnicodeString* snext(UError_کوڈ& حالت) U_LIFETIME_BOUND override {
        إذا (U_FAILURE(حالت)) { أرجع خالی_سوچک; }
        int32_t نتیجہ_لمائی = 0;
        ثابت حرف *s = next(&نتیجہ_لمائی, حالت);
        أرجع سیٹ_Chars(s, نتیجہ_لمائی, حالت);
    }

    مجازی فراغ reset(UError_کوڈ& حالت) override {
        إذا (U_FAILURE(حالت)) { أرجع; }
        current = keywords.انکڑے();
    }
};

ثابت حرف KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = افتراضي;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
صنف UnicodeKeywordEnumeration : عام KeywordEnumeration {
عام:
    ورتوں KeywordEnumeration::KeywordEnumeration;
    مجازی ~UnicodeKeywordEnumeration();

    مجازی ثابت حرف* next(int32_t* نتیجہ_لمائی, UError_کوڈ& حالت) U_LIFETIME_BOUND override {
        ثابت حرف* legacy_کلید = KeywordEnumeration::next(خالی_سوچک, حالت);
        طالما (U_SUCCESS(حالت) && legacy_کلید != خالی_سوچک) {
            ثابت حرف* کلید = uloc_منزل_Unicode_Locale_کلید(legacy_کلید);
            إذا (کلید != خالی_سوچک) {
                إذا (نتیجہ_لمائی != خالی_سوچک) {
                    *نتیجہ_لمائی = static_cast<int32_t>(uprv_strlen(کلید));
                }
                أرجع کلید;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_کلید = KeywordEnumeration::next(خالی_سوچک, حالت);
        }
        إذا (نتیجہ_لمائی != خالی_سوچک) *نتیجہ_لمائی = 0;
        أرجع خالی_سوچک;
    }
    مجازی int32_t count(UError_کوڈ& حالت) ثابت override {
        إذا (U_FAILURE(حالت)) { أرجع 0; }
        ثابت حرف *kw = keywords.انکڑے();
        int32_t نتیجہ = 0;
        طالما(*kw) {
            إذا (uloc_منزل_Unicode_Locale_کلید(kw) != خالی_سوچک) {
                نتیجہ++;
            }
            kw += uprv_strlen(kw)+1;
        }
        أرجع نتیجہ;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = افتراضي;

StringEnumeration *
Locale::createKeywords(UError_کوڈ &حالت) ثابت
{
    StringEnumeration *نتیجہ = خالی_سوچک;

    إذا (U_FAILURE(حالت)) {
        أرجع نتیجہ;
    }

    ثابت حرف* variant_شروع = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    إذا(variant_شروع) {
        إذا(assignment > variant_شروع) {
            CharString keywords = ulocimp_getKeywords(variant_شروع + 1, '@', خطأ, حالت);
            إذا (U_SUCCESS(حالت) && !keywords.is_خالی_تھاواں()) {
                نتیجہ = جديد KeywordEnumeration(keywords.انکڑے(), keywords.length(), 0, حالت);
                إذا (!نتیجہ) {
                    حالت = U_حافظہ_ونڈ_ERROR;
                }
            }
        } وإلا {
            حالت = U_INVALID_FORMAT_ERROR;
        }
    }
    أرجع نتیجہ;
}

StringEnumeration *
Locale::createUnicodeKeywords(UError_کوڈ &حالت) ثابت
{
    StringEnumeration *نتیجہ = خالی_سوچک;

    إذا (U_FAILURE(حالت)) {
        أرجع نتیجہ;
    }

    ثابت حرف* variant_شروع = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    إذا(variant_شروع) {
        إذا(assignment > variant_شروع) {
            CharString keywords = ulocimp_getKeywords(variant_شروع + 1, '@', خطأ, حالت);
            إذا (U_SUCCESS(حالت) && !keywords.is_خالی_تھاواں()) {
                نتیجہ = جديد UnicodeKeywordEnumeration(keywords.انکڑے(), keywords.length(), 0, حالت);
                إذا (!نتیجہ) {
                    حالت = U_حافظہ_ونڈ_ERROR;
                }
            }
        } وإلا {
            حالت = U_INVALID_FORMAT_ERROR;
        }
    }
    أرجع نتیجہ;
}

int32_t
Locale::get_Keyword_قدر(ثابت حرف* keywordName, حرف *بفر, int32_t bufLen, UError_کوڈ &حالت) ثابت
{
    أرجع uloc_get_Keyword_قدر(getName(), keywordName, بفر, bufLen, &حالت);
}

فراغ
Locale::get_Keyword_قدر(StringPiece keywordName, ByteSink& sink, UError_کوڈ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    إذا (isBogus()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    ulocimp_get_Keyword_قدر(getName(), keywordName, sink, حالت);
}

فراغ
Locale::get_Unicode_Keyword_قدر(StringPiece keywordName,
                               ByteSink& sink,
                               UError_کوڈ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    std::optional<std::string_view> legacy_کلید = ulocimp_منزل_Legacy_کلید_With_Fallback(keywordName);
    إذا (!legacy_کلید.has_value()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    خودکار legacy_قدر = get_Keyword_قدر<CharString>(*legacy_کلید, حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    std::optional<std::string_view> unicode_قدر =
        ulocimp_منزل_Bcp_Type_With_Fallback(keywordName, legacy_قدر.منزل_String_Piece());
    إذا (!unicode_قدر.has_value()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    sink.Append(unicode_قدر->انکڑے(), static_cast<int32_t>(unicode_قدر->size()));
}

فراغ
Locale::سیٹ_Keyword_قدر(StringPiece keywordName,
                        StringPiece keyword_قدر,
                        UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }
    إذا (keywordName.empty()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا (حالت == U_STRING_NOT_TERMINATED_WARNING) {
        حالت = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, حالت);
    ulocimp_سیٹ_Keyword_قدر(keywordName, keyword_قدر, localeID, حالت);
    إذا (U_FAILURE(حالت)) {
        إذا (حالت == U_حافظہ_ونڈ_ERROR) {
            سیٹ_منزل_Bogus();
        }
        أرجع;
    }

    ثابت حرف* at = locale_get_Keywords_شروع(localeID.منزل_String_Piece());
    منطقي hasKeywords = at != خالی_سوچک && uprv_strchr(at + 1, '=') != خالی_سوچک;

    Nest* nest = payload.get<Nest>();
    إذا (!hasKeywords) {
        إذا (nest == خالی_سوچک) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            ڈھیر_2* ڈھیر_3 = payload.get<ڈھیر_2>();
            U_ASSERT(ڈھیر_3 != خالی_سوچک);
            إذا (Nest::سماندا(localeID.length(), ڈھیر_3->language, ڈھیر_3->script, ڈھیر_3->region)) {
                int32_t variantBegin = ڈھیر_3->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < حجم Nest::ادھار_Name);
                nest = &payload.emplace<Nest>(std::move(*ڈھیر_3), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->ادھار_Name, حجم Nest::ادھار_Name, حالت);
            } وإلا {
                ڈھیر_3->ptr->ادھار_Name.clear();
                ڈھیر_3->ptr->بھریا_Name = localeID.منزل_String_Piece();
                إذا (ڈھیر_3->ptr->بھریا_Name.is_خالی_تھاواں()) {
                    حالت = U_حافظہ_ونڈ_ERROR;
                    سیٹ_منزل_Bogus();
                    أرجع;
                }
            }
        }
    } وإلا {
        ڈھیر_2* ڈھیر_3 = خالی_سوچک;
        إذا (nest != خالی_سوچک) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            ڈھیر_3 = &payload.emplace<ڈھیر_2>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            إذا (isBogus()) {
                حالت = U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
        } وإلا {
            ڈھیر_3 = payload.get<ڈھیر_2>();
        }
        U_ASSERT(ڈھیر_3 != خالی_سوچک);
        ڈھیر_3->ptr->بھریا_Name = localeID.منزل_String_Piece();
        إذا (ڈھیر_3->ptr->بھریا_Name.is_خالی_تھاواں()) {
            حالت = U_حافظہ_ونڈ_ERROR;
            سیٹ_منزل_Bogus();
            أرجع;
        }

        إذا (ڈھیر_3->ptr->ادھار_Name.is_خالی_تھاواں()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            إذا (std::string_view::size_type ادھار_Name_لمائی = at - localeID.انکڑے(); ادھار_Name_لمائی > 0) {
                ڈھیر_3->ptr->ادھار_Name = {ڈھیر_3->ptr->بھریا_Name.انکڑے(), ادھار_Name_لمائی};
                إذا (ڈھیر_3->ptr->ادھار_Name.is_خالی_تھاواں()) {
                    حالت = U_حافظہ_ونڈ_ERROR;
                    سیٹ_منزل_Bogus();
                    أرجع;
                }
            }
        }
    }
}

فراغ
Locale::سیٹ_Unicode_Keyword_قدر(StringPiece keywordName,
                               StringPiece keyword_قدر,
                               UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    std::optional<std::string_view> legacy_کلید = ulocimp_منزل_Legacy_کلید_With_Fallback(keywordName);
    إذا (!legacy_کلید.has_value()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    std::string_view value;

    إذا (!keyword_قدر.empty()) {
        std::optional<std::string_view> legacy_قدر =
            ulocimp_منزل_Legacy_Type_With_Fallback(keywordName, keyword_قدر);
        إذا (!legacy_قدر.has_value()) {
            حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        value = *legacy_قدر;
    }

    سیٹ_Keyword_قدر(*legacy_کلید, value, حالت);
}

ثابت حرف*
Locale::getCountry() ثابت U_LIFETIME_BOUND {
    أرجع get_میدان<&Nest::getRegion, &ڈھیر_2::getRegion>();
}

ثابت حرف*
Locale::getLanguage() ثابت U_LIFETIME_BOUND {
    أرجع get_میدان<&Nest::getLanguage, &ڈھیر_2::getLanguage>();
}

ثابت حرف*
Locale::getScript() ثابت U_LIFETIME_BOUND {
    أرجع get_میدان<&Nest::getScript, &ڈھیر_2::getScript>();
}

ثابت حرف*
Locale::getVariant() ثابت U_LIFETIME_BOUND {
    أرجع get_میدان<&Nest::getVariant, &ڈھیر_2::getVariant>();
}

ثابت حرف*
Locale::getName() ثابت U_LIFETIME_BOUND {
    أرجع get_میدان<&Nest::get_ادھار_Name, &ڈھیر_2::get_بھریا_Name>();
}

ثابت حرف*
Locale::get_ادھار_Name() ثابت U_LIFETIME_BOUND {
    أرجع get_میدان<&Nest::get_ادھار_Name, &ڈھیر_2::get_ادھار_Name>();
}

سانچا <ثابت حرف* (Locale::Nest::*ثابت NEST)() ثابت,
          ثابت حرف* (Locale::ڈھیر_2::*ثابت ڈھیر_4)() ثابت>
ثابت حرف* Locale::get_میدان() ثابت U_LIFETIME_BOUND {
    أرجع payload.ویکھو([] { أرجع ""; },
                         [](ثابت Nest& nest) { أرجع (nest.*NEST)(); },
                         [](ثابت ڈھیر_2& ڈھیر_3) { أرجع (ڈھیر_3.*ڈھیر_4)(); });
}

Locale::Iterator::~Iterator() = افتراضي;

//eof
U_NAMESPACE_سرا
