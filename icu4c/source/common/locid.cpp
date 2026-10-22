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
#پایان_اگر

U_NAMESPACE_BEGIN

ساكن Locale   *gLocaleCache = اشاره_تهی;
ساكن UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
ساكن UMutex gDefaultLocaleMutex;
ساكن UHashtable *gDefaultLocalesHashT = اشاره_تهی;
ساكن Locale *gDefaultLocale = اشاره_تهی;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#تعریف_کن ULOC_STRING_حد 357913941

U_NAMESPACE_انتها

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
    e_ریشه,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

نطاق {

//
// Deleter function for Locales owned by the default Locale hash table/
//
فراغ U_CALLCONV
deleteLocale(فراغ *obj) {
    حذف static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    حذف [] gLocaleCache;
    gLocaleCache = اشاره_تهی;
    gLocaleCacheInitOnce.reset();

    إذا (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = اشاره_تهی;
    }
    gDefaultLocale = اشاره_تهی;
    أرجع درست;
}

فراغ U_CALLCONV locale_init(UError_کد &وضعیت) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == اشاره_تهی);
    gLocaleCache = جديد Locale[static_cast<صحيح>(eMAX_LOCALES)];
    إذا (gLocaleCache == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    ucln_common_ثبات_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_ریشه]          = Locale("");
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

Locale *locale_مجموعه_default_داخلی(ثابت حرف *id, UError_کد& وضعیت) {
    // Synchronize this entire function.
    Mutex قفل(&gDefaultLocaleMutex);

    UBool canonicalize = خطأ;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    إذا (id == اشاره_تهی) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = درست; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, وضعیت) : ulocimp_getName(id, وضعیت);

    إذا (U_FAILURE(وضعیت)) {
        أرجع gDefaultLocale;
    }

    إذا (gDefaultLocalesHashT == اشاره_تهی) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, اشاره_تهی, &وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            أرجع gDefaultLocale;
        }
        uhash_مجموعه_مقدار_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_ثبات_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.داده()));
    إذا (newDefault == اشاره_تهی) {
        newDefault = جديد Locale(Locale::eBOGUS);
        إذا (newDefault == اشاره_تهی) {
            وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع gDefaultLocale;
        }
        newDefault->init(localeNameBuf.داده(), خطأ);
        uhash_put(gDefaultLocalesHashT, const_cast<حرف*>(newDefault->getName()), newDefault, &وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            أرجع gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    أرجع gDefaultLocale;
}

U_NAMESPACE_انتها

/* sfb 07/21/99 */
U_CFUNC فراغ
locale_مجموعه_default(ثابت حرف *id)
{
    U_NAMESPACE_USE
    UError_کد وضعیت = U_ZERO_ERROR;
    locale_مجموعه_default_داخلی(id, وضعیت);
}
/*انتها*/

U_CFUNC ثابت حرف *
locale_get_default()
{
    U_NAMESPACE_USE
    أرجع Locale::getDefault().getName();
}

نطاق {

قالب <خودکار فیلد, نام_نوع T>
فراغ copy_مقصد_Array(std::string_view sv, T* that) {
    خودکار& فیلد_2 = that->*فیلد;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(فیلد_2)>>;
    static_assert(capacity > 0);
    إذا (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(فیلد_2, sv.داده(), sv.size());
    }
    فیلد_2[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

فراغ Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copy_مقصد_Array<&Nest::language>(language, هذا);
    copy_مقصد_Array<&Nest::script>(script, هذا);
    copy_مقصد_Array<&Nest::region>(region, هذا);
    هذا->variantBegin = variantBegin;
}

Locale::Nest::Nest(انباشته&& انباشته_2, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(انباشته, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(انباشته, region));
    U_ASSERT(هذا == reinterpret_cast<Nest*>(&انباشته_2));
#إذا UPRV_HAS_FEATURE(حافظه_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&انباشته_2, حجم(انباشته));
#پایان_اگر
    copy_مقصد_Array<&Nest::script>(انباشته_2.script, هذا);
    copy_مقصد_Array<&Nest::region>(انباشته_2.region, هذا);
    هذا->variantBegin = variantBegin;
    *هذا->پایه_Name = '\0';
}

بنية Locale::انباشته::Alloc : عام UMemory {
    FixedString پر_Name;
    FixedString پایه_Name;
    int32_t variantBegin;

    ثابت حرف* getVariant() ثابت U_LIFETIME_BOUND {
        أرجع variantBegin == 0 ? "" : get_پایه_Name() + variantBegin;
    }
    ثابت حرف* get_پر_Name() ثابت U_LIFETIME_BOUND { أرجع پر_Name.داده(); }
    ثابت حرف* get_پایه_Name() ثابت U_LIFETIME_BOUND {
        إذا (پایه_Name.is_خالی()) {
            إذا (ثابت حرف* name = پر_Name.داده(); *name != '@') {
                أرجع name;
            }
        }
        أرجع پایه_Name.داده();
    }

    Alloc(int32_t variantBegin) : پر_Name(), پایه_Name(), variantBegin(variantBegin) {}

    Alloc(ثابت Alloc& other, UError_کد& وضعیت)
        : پر_Name(), پایه_Name(), variantBegin(other.variantBegin) {
        إذا (U_SUCCESS(وضعیت)) {
            إذا (!other.پر_Name.is_خالی()) {
                پر_Name = other.پر_Name;
                إذا (پر_Name.is_خالی()) {
                    وضعیت = U_حافظه_تخصیص_ERROR;
                } وإلا {
                    إذا (!other.پایه_Name.is_خالی()) {
                        پایه_Name = other.پایه_Name;
                        إذا (پایه_Name.is_خالی()) {
                            وضعیت = U_حافظه_تخصیص_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = حذف;

    ~Alloc() = افتراضي;
};

ثابت حرف* Locale::انباشته::getVariant() ثابت U_LIFETIME_BOUND { أرجع ptr->getVariant(); }
ثابت حرف* Locale::انباشته::get_پر_Name() ثابت U_LIFETIME_BOUND { أرجع ptr->get_پر_Name(); }
ثابت حرف* Locale::انباشته::get_پایه_Name() ثابت U_LIFETIME_BOUND { أرجع ptr->get_پایه_Name(); }

Locale::انباشته::انباشته(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = جديد Alloc(variantBegin);
    إذا (ptr == اشاره_تهی) {
        type = eBOGUS;
    } وإلا {
        type = e_انباشته;
        copy_مقصد_Array<&انباشته::language>(language, هذا);
        copy_مقصد_Array<&انباشته::script>(script, هذا);
        copy_مقصد_Array<&انباشته::region>(region, هذا);
    }
}

Locale::انباشته::~انباشته() {
    U_ASSERT(type == e_انباشته);
    حذف ptr;
}

Locale::انباشته& Locale::انباشته::عملگر=(ثابت انباشته& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UError_کد وضعیت = U_ZERO_ERROR;
    ptr = جديد Alloc(*other.ptr, وضعیت);
    إذا (ptr == اشاره_تهی || U_FAILURE(وضعیت)) {
        حذف ptr;
    } وإلا {
        type = e_انباشته;
        uprv_memcpy(language, other.language, حجم language);
        uprv_memcpy(script, other.script, حجم script);
        uprv_memcpy(region, other.region, حجم region);
    }
    أرجع *هذا;
}

Locale::انباشته& Locale::انباشته::عملگر=(انباشته&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = e_انباشته;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, حجم language);
    uprv_memcpy(script, other.script, حجم script);
    uprv_memcpy(region, other.region, حجم region);
    أرجع *هذا;
}

قالب <نام_نوع BogusFn, نام_نوع NestFn, نام_نوع انباشته_Fn, نام_نوع... آرگومانها>
خودکار Locale::Payload::بازدید(BogusFn bogusFn, NestFn nestFn, انباشته_Fn انباشته_Fn_2, آرگومانها... آرگومانها_2) ثابت {
    بدل (type) {
        حالة eBOGUS:
            أرجع bogusFn(آرگومانها_2...);
        حالة eNEST:
            أرجع nestFn(nest, آرگومانها_2...);
        حالة e_انباشته:
            أرجع انباشته_Fn_2(انباشته_2, آرگومانها_2...);
        افتراضي:
            UPRV_UNREACHABLE_EXIT;
    };
}

فراغ Locale::Payload::copy(ثابت Payload& other) {
    other.بازدید([](Payload*) {},
                [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ثابت انباشته& انباشته_2, Payload* dst) { dst->انباشته_2 = انباشته_2; },
                هذا);
}

فراغ Locale::Payload::move(Payload&& other) noexcept {
    other.بازدید(
        [](Payload*) {},
        [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ثابت انباشته& انباشته_2, Payload* dst) { dst->انباشته_2 = std::move(const_cast<انباشته&>(انباشته_2)); },
        هذا);
}

Locale::Payload::~Payload() {
    إذا (type == e_انباشته) { انباشته_2.~انباشته(); }
}

Locale::Payload::Payload(ثابت Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::عملگر=(ثابت Payload& other) U_LIFETIME_BOUND {
    إذا (هذا != &other) {
        مجموعه_مقصد_Bogus();
        copy(other);
    }
    أرجع *هذا;
}

Locale::Payload& Locale::Payload::عملگر=(Payload&& other) noexcept U_LIFETIME_BOUND {
    إذا (هذا != &other) {
        مجموعه_مقصد_Bogus();
        move(std::move(other));
    }
    أرجع *هذا;
}

فراغ Locale::Payload::مجموعه_مقصد_Bogus() {
    هذا->~Payload();
    type = eBOGUS;
}

قالب <نام_نوع T, نام_نوع... آرگومانها> T& Locale::Payload::emplace(آرگومانها&&... آرگومانها_2) U_LIFETIME_BOUND {
    إذا constexpr (std::is_same_v<T, Nest>) {
        هذا->~Payload();
        ::جديد (&nest) Nest(std::forward<آرگومانها>(آرگومانها_2)...);
        أرجع nest;
    }
    إذا constexpr (std::is_same_v<T, انباشته>) {
        U_ASSERT(type != e_انباشته);
        ::جديد (&انباشته_2) انباشته(std::forward<آرگومانها>(آرگومانها_2)...);
        أرجع انباشته_2;
    }
}

قالب <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    أرجع type == eNEST ? &nest : اشاره_تهی;
}
قالب <> Locale::انباشته* Locale::Payload::get() U_LIFETIME_BOUND {
    أرجع type == e_انباشته ? &انباشته_2 : اشاره_تهی;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#تعریف_کن SEP_CHAR '_'
#تعریف_کن NULL_CHAR '\0'

Locale::~Locale() = افتراضي;

Locale::Locale()
    : UObject(), payload()
{
    init(اشاره_تهی, خطأ);
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
    إذا( (newLanguage==اشاره_تهی) && (newCountry == اشاره_تهی) && (newVariant == اشاره_تهی) )
    {
        init(اشاره_تهی, خطأ); /* shortcut */
    }
    وإلا
    {
        UError_کد وضعیت = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        إذا ( newLanguage != اشاره_تهی )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            إذا ( lsize < 0 || lsize > ULOC_STRING_حد ) { // int32 wrap
                أرجع;
            }
        }

        CharString togo(newLanguage, lsize, وضعیت); // start with newLanguage

        // _Country
        إذا ( newCountry != اشاره_تهی )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            إذا ( csize < 0 || csize > ULOC_STRING_حد ) { // int32 wrap
                أرجع;
            }
        }

        // _Variant
        إذا ( newVariant != اشاره_تهی )
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

        إذا ( newKeywords != اشاره_تهی)
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
            togo.append(SEP_CHAR, وضعیت);
        }

        إذا ( csize != 0 )
        {
            togo.append(newCountry, وضعیت);
        }

        إذا ( vsize != 0)
        {
            togo.append(SEP_CHAR, وضعیت)
                .append(newVariant, vsize, وضعیت);
        }

        إذا ( ksize != 0)
        {
            إذا (uprv_strchr(newKeywords, '=')) {
                togo.append('@', وضعیت); /* keyword parsing */
            }
            وإلا {
                togo.append('_', وضعیت); /* Variant parsing with a script */
                إذا ( vsize == 0) {
                    togo.append('_', وضعیت); /* No country found */
                }
            }
            togo.append(newKeywords, وضعیت);
        }

        إذا (U_FAILURE(وضعیت)) {
            // Something went wrong with appending, etc.
            أرجع;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.داده(), خطأ);
    }
}

Locale::Locale(ثابت Locale&) = افتراضي;
Locale::Locale(Locale&&) noexcept = افتراضي;

Locale& Locale::عملگر=(ثابت Locale&) U_LIFETIME_BOUND = افتراضي;
Locale& Locale::عملگر=(Locale&&) noexcept U_LIFETIME_BOUND = افتراضي;

Locale *
Locale::clone() ثابت {
    أرجع جديد Locale(*هذا);
}

منطقي
Locale::عملگر==( ثابت   Locale& other) ثابت
{
    أرجع uprv_strcmp(other.getName(), getName()) == 0;
}

نطاق {

UInitOnce g_معلوم_Canonicalized_Init_Once {};
UHashtable *g_معلوم_Canonicalized = اشاره_تهی;

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
    أرجع درست;
}

فراغ U_CALLCONV بارگیری_معلوم_Canonicalized(UError_کد &وضعیت) {
    ucln_common_ثبات_Cleanup(UCLN_COMMON_LOCALE_معلوم_CANONICALIZED,
                                cleanup_معلوم_Canonicalized);
    Local_UHashtable_اشاره_گر new_معلوم_Canonicalized_Map(
        uhash_open(uhash_hashChars, uhash_compareChars, اشاره_تهی, &وضعیت));
    لكل (int32_t i = 0;
            U_SUCCESS(وضعیت) && i < UPRV_LENGTHOF(معلوم_CANONICALIZED);
            i++) {
        uhash_puti(new_معلوم_Canonicalized_Map.getAlias(),
                   (فراغ*)معلوم_CANONICALIZED[i],
                   1, &وضعیت);
    }
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    g_معلوم_Canonicalized = new_معلوم_Canonicalized_Map.orphan();
}

صنف Alias_داده;

/**
 * A Builder class to build the alias data.
 */
صنف Alias_داده_Builder {
عام:
    Alias_داده_Builder() {
    }

    // Build the AliasData from resource.
    Alias_داده* build(UError_کد &وضعیت);

خاص:
    فراغ readAlias(UResourceBundle* alias,
                   Unique_Char_رشتهها* رشتهها,
                   Local_حافظه<ثابت حرف*>& types,
                   Local_حافظه<int32_t>& replacementIndexes,
                   int32_t &length,
                   فراغ (*checkType)(ثابت حرف* type),
                   فراغ (*checkReplacement)(ثابت UChar* replacement),
                   UError_کد &وضعیت);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readLanguageAlias(UResourceBundle* alias,
                           Unique_Char_رشتهها* رشتهها,
                           Local_حافظه<ثابت حرف*>& types,
                           Local_حافظه<int32_t>& replacementIndexes,
                           int32_t &length,
                           UError_کد &وضعیت);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readScriptAlias(UResourceBundle* alias,
                         Unique_Char_رشتهها* رشتهها,
                         Local_حافظه<ثابت حرف*>& types,
                         Local_حافظه<int32_t>& replacementIndexes,
                         int32_t &length, UError_کد &وضعیت);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readTerritoryAlias(UResourceBundle* alias,
                            Unique_Char_رشتهها* رشتهها,
                            Local_حافظه<ثابت حرف*>& types,
                            Local_حافظه<int32_t>& replacementIndexes,
                            int32_t &length, UError_کد &وضعیت);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    فراغ readVariantAlias(UResourceBundle* alias,
                          Unique_Char_رشتهها* رشتهها,
                          Local_حافظه<ثابت حرف*>& types,
                          Local_حافظه<int32_t>& replacementIndexes,
                          int32_t &length, UError_کد &وضعیت);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    فراغ readSubdivisionAlias(UResourceBundle* alias,
                          Unique_Char_رشتهها* رشتهها,
                          Local_حافظه<ثابت حرف*>& types,
                          Local_حافظه<int32_t>& replacementIndexes,
                          int32_t &length, UError_کد &وضعیت);
};

/**
 * A class to hold the Alias Data.
 */
صنف Alias_داده : عام UMemory {
عام:
    ساكن ثابت Alias_داده* singleton(UError_کد& وضعیت) {
        إذا (U_FAILURE(وضعیت)) {
            // Do not get into loadData if the status already has error.
            أرجع اشاره_تهی;
        }
        umtx_initOnce(Alias_داده::gInitOnce, &Alias_داده::بارگیری_داده, وضعیت);
        أرجع gSingleton;
    }

    ثابت CharStringMap& languageMap() ثابت U_LIFETIME_BOUND { أرجع language; }
    ثابت CharStringMap& scriptMap() ثابت U_LIFETIME_BOUND { أرجع script; }
    ثابت CharStringMap& territoryMap() ثابت U_LIFETIME_BOUND { أرجع territory; }
    ثابت CharStringMap& variantMap() ثابت U_LIFETIME_BOUND { أرجع variant; }
    ثابت CharStringMap& subdivisionMap() ثابت U_LIFETIME_BOUND { أرجع subdivision; }

    ساكن فراغ U_CALLCONV بارگیری_داده(UError_کد &وضعیت);
    ساكن UBool U_CALLCONV cleanup();

    ساكن UInitOnce gInitOnce;

خاص:
    Alias_داده(CharStringMap languageMap,
              CharStringMap scriptMap,
              CharStringMap territoryMap,
              CharStringMap variantMap,
              CharStringMap subdivisionMap,
              CharString* رشتهها)
        : language(std::move(languageMap)),
          script(std::move(scriptMap)),
          territory(std::move(territoryMap)),
          variant(std::move(variantMap)),
          subdivision(std::move(subdivisionMap)),
          رشتهها(رشتهها) {
    }

    ~Alias_داده() {
        حذف رشتهها;
    }

    ساكن ثابت Alias_داده* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* رشتهها;

    دوست صنف Alias_داده_Builder;
};


ثابت Alias_داده* Alias_داده::gSingleton = اشاره_تهی;
UInitOnce Alias_داده::gInitOnce {};

UBool U_CALLCONV
Alias_داده::cleanup()
{
    gInitOnce.reset();
    حذف gSingleton;
    أرجع درست;
}

فراغ
Alias_داده_Builder::readAlias(
        UResourceBundle* alias,
        Unique_Char_رشتهها* رشتهها,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        فراغ (*checkType)(ثابت حرف* type),
        فراغ (*checkReplacement)(ثابت UChar* replacement),
        UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    length = ures_get_اندازه(alias);
    ثابت حرف** rawTypes = types.allocateInsteadAndCopy(length);
    إذا (rawTypes == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    إذا (rawIndexes == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    لكل (صحيح i = 0; U_SUCCESS(وضعیت) && ures_has_بعدی(alias); i++) {
        Local_UResource_Bundle_اشاره_گر res(
            ures_get_بعدی_Resource(alias, اشاره_تهی, &وضعیت));
        ثابت حرف* aliasFrom = ures_get_کلید(res.getAlias());
        ثابت UChar* alias_مقصد =
            ures_get_String_By_کلید(res.getAlias(), "replacement", اشاره_تهی, &وضعیت);
        إذا (U_FAILURE(وضعیت)) أرجع;

        checkType(aliasFrom);
        checkReplacement(alias_مقصد);

        rawTypes[i] = aliasFrom;
        rawIndexes[i] = رشتهها->جمع_کن(alias_مقصد, وضعیت);
    }
}

/**
 * Read the languageAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement language.
 */
فراغ
Alias_داده_Builder::readLanguageAlias(
        UResourceBundle* alias,
        Unique_Char_رشتهها* رشتهها,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کد &وضعیت)
{
    أرجع readAlias(
        alias, رشتهها, types, replacementIndexes, length,
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
#پایان_اگر
        [](ثابت UChar*) {}, وضعیت);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
فراغ
Alias_داده_Builder::readScriptAlias(
        UResourceBundle* alias,
        Unique_Char_رشتهها* رشتهها,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کد &وضعیت)
{
    أرجع readAlias(
        alias, رشتهها, types, replacementIndexes, length,
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
#پایان_اگر
        وضعیت);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
فراغ
Alias_داده_Builder::readTerritoryAlias(
        UResourceBundle* alias,
        Unique_Char_رشتهها* رشتهها,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کد &وضعیت)
{
    أرجع readAlias(
        alias, رشتهها, types, replacementIndexes, length,
#إذا U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#وإلا
        [](ثابت حرف*) {},
#پایان_اگر
        [](ثابت UChar*) { },
        وضعیت);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
فراغ
Alias_داده_Builder::readVariantAlias(
        UResourceBundle* alias,
        Unique_Char_رشتهها* رشتهها,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کد &وضعیت)
{
    أرجع readAlias(
        alias, رشتهها, types, replacementIndexes, length,
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
#پایان_اگر
        وضعیت);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
فراغ
Alias_داده_Builder::readSubdivisionAlias(
        UResourceBundle* alias,
        Unique_Char_رشتهها* رشتهها,
        Local_حافظه<ثابت حرف*>& types,
        Local_حافظه<int32_t>& replacementIndexes,
        int32_t &length,
        UError_کد &وضعیت)
{
    أرجع readAlias(
        alias, رشتهها, types, replacementIndexes, length,
#إذا U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#وإلا
        [](ثابت حرف*) {},
#پایان_اگر
        [](ثابت UChar*) { },
        وضعیت);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
فراغ U_CALLCONV
Alias_داده::بارگیری_داده(UError_کد &وضعیت)
{
#اگر_تعریف_شده LOCALE_CANONICALIZATION_DEBUG
    UDate شروع = uprv_getRawUTCtime();
#پایان_اگر  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_ثبات_Cleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    Alias_داده_Builder builder;
    gSingleton = builder.build(وضعیت);
#اگر_تعریف_شده LOCALE_CANONICALIZATION_DEBUG
    UDate انتها = uprv_getRawUTCtime();
    چاپ_کن("AliasData::loadData took total %f ms\n", انتها - شروع);
#پایان_اگر  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
Alias_داده*
Alias_داده_Builder::build(UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) { أرجع اشاره_تهی; }

    Local_UResource_Bundle_اشاره_گر metadata(
        ures_open_مستقیم(اشاره_تهی, "metadata", &وضعیت));
    Local_UResource_Bundle_اشاره_گر metadataAlias(
        ures_get_By_کلید(metadata.getAlias(), "alias", اشاره_تهی, &وضعیت));
    Local_UResource_Bundle_اشاره_گر languageAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "language", اشاره_تهی, &وضعیت));
    Local_UResource_Bundle_اشاره_گر scriptAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "script", اشاره_تهی, &وضعیت));
    Local_UResource_Bundle_اشاره_گر territoryAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "territory", اشاره_تهی, &وضعیت));
    Local_UResource_Bundle_اشاره_گر variantAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "variant", اشاره_تهی, &وضعیت));
    Local_UResource_Bundle_اشاره_گر subdivisionAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "subdivision", اشاره_تهی, &وضعیت));

    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }
    int32_t languages_درازی = 0, script_درازی = 0, territory_درازی = 0,
            variant_درازی = 0, subdivision_درازی = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    Unique_Char_رشتهها رشتهها(وضعیت);
    Local_حافظه<ثابت حرف*> languageTypes;
    Local_حافظه<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &رشتهها,
                      languageTypes,
                      languageReplacementIndexes,
                      languages_درازی,
                      وضعیت);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    Local_حافظه<ثابت حرف*> scriptTypes;
    Local_حافظه<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &رشتهها,
                    scriptTypes,
                    scriptReplacementIndexes,
                    script_درازی,
                    وضعیت);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    Local_حافظه<ثابت حرف*> territoryTypes;
    Local_حافظه<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &رشتهها,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territory_درازی, وضعیت);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    Local_حافظه<ثابت حرف*> variantTypes;
    Local_حافظه<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &رشتهها,
                     variantTypes,
                     variantReplacementIndexes,
                     variant_درازی, وضعیت);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    Local_حافظه<ثابت حرف*> subdivisionTypes;
    Local_حافظه<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &رشتهها,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivision_درازی, وضعیت);

    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }

    // We can only use strings after freeze it.
    رشتهها.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, وضعیت);
    لكل (int32_t i = 0; U_SUCCESS(وضعیت) && i < languages_درازی; i++) {
        languageMap.put(languageTypes[i],
                        رشتهها.get(languageReplacementIndexes[i]),
                        وضعیت);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, وضعیت);
    لكل (int32_t i = 0; U_SUCCESS(وضعیت) && i < script_درازی; i++) {
        scriptMap.put(scriptTypes[i],
                      رشتهها.get(scriptReplacementIndexes[i]),
                      وضعیت);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, وضعیت);
    لكل (int32_t i = 0; U_SUCCESS(وضعیت) && i < territory_درازی; i++) {
        territoryMap.put(territoryTypes[i],
                         رشتهها.get(territoryReplacementIndexes[i]),
                         وضعیت);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, وضعیت);
    لكل (int32_t i = 0; U_SUCCESS(وضعیت) && i < variant_درازی; i++) {
        variantMap.put(variantTypes[i],
                       رشتهها.get(variantReplacementIndexes[i]),
                       وضعیت);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, وضعیت);
    لكل (int32_t i = 0; U_SUCCESS(وضعیت) && i < subdivision_درازی; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       رشتهها.get(subdivisionReplacementIndexes[i]),
                       وضعیت);
    }

    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }

    // copy hashtables
    خودکار *داده = جديد Alias_داده(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        رشتهها.orphan_Char_رشتهها());

    إذا (داده == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
    }
    أرجع داده;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
صنف AliasReplacer {
عام:
    AliasReplacer(UError_کد& وضعیت) :
            language(اشاره_تهی), script(اشاره_تهی), region(اشاره_تهی),
            extensions(اشاره_تهی),
            // store value in variants only once
            variants(اشاره_تهی,
                     ([](UElement e1, UElement e2) -> UBool {
                       أرجع 0==uprv_strcmp((ثابت حرف*)e1.اشاره_گر,
                                             (ثابت حرف*)e2.اشاره_گر);}),
                     وضعیت),
            داده(اشاره_تهی) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    منطقي replace(
        ثابت Locale& locale, CharString& out, UError_کد& وضعیت);

خاص:
    ثابت حرف* language;
    ثابت حرف* script;
    ثابت حرف* region;
    ثابت حرف* extensions;
    UVector variants;

    ثابت Alias_داده* داده;

    مضمن منطقي not_خالی(ثابت حرف* str) {
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
        أرجع not_خالی(replacement) ?
            ((input == اشاره_تهی) ?  replacement : input) :
            ((type == اشاره_تهی) ? input  : اشاره_تهی);
    }

    مضمن منطقي same(ثابت حرف* a, ثابت حرف* b) {
        إذا (a == اشاره_تهی && b == اشاره_تهی) {
            أرجع درست;
        }
        إذا ((a == اشاره_تهی && b != اشاره_تهی) ||
            (a != اشاره_تهی && b == اشاره_تهی)) {
          أرجع خطأ;
        }
        أرجع uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& output_مقصد_String(CharString& out U_LIFETIME_BOUND, UError_کد& وضعیت);

    // Generate the lookup key.
    CharString& generate_کلید(ثابت حرف* language, ثابت حرف* region,
                            ثابت حرف* variant, CharString& out U_LIFETIME_BOUND,
                            UError_کد& وضعیت);

    فراغ parseLanguageReplacement(ثابت حرف* replacement,
                                  ثابت حرف*& replaceLanguage,
                                  ثابت حرف*& replaceScript,
                                  ثابت حرف*& replaceRegion,
                                  ثابت حرف*& replaceVariant,
                                  ثابت حرف*& replaceExtensions,
                                  UVector& مقصد_Be_Freed,
                                  UError_کد& وضعیت);

    // Replace by using languageAlias.
    منطقي replaceLanguage(منطقي checkLanguage, منطقي checkRegion,
                         منطقي checkVariants, UVector& مقصد_Be_Freed,
                         UError_کد& وضعیت);

    // Replace by using territoryAlias.
    منطقي replaceTerritory(UVector& مقصد_Be_Freed, UError_کد& وضعیت);

    // Replace by using scriptAlias.
    منطقي replaceScript(UError_کد& وضعیت);

    // Replace by using variantAlias.
    منطقي replaceVariant(UError_کد& وضعیت);

    // Replace by using subdivisionAlias.
    منطقي replaceSubdivision(StringPiece subdivision,
                            CharString& output, UError_کد& وضعیت);

    // Replace transformed extensions.
    منطقي replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UError_کد& وضعیت);
};

CharString&
AliasReplacer::generate_کلید(
        ثابت حرف* language, ثابت حرف* region, ثابت حرف* variant,
        CharString& out U_LIFETIME_BOUND, UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) { أرجع out; }
    out.append(language, وضعیت);
    إذا (not_خالی(region)) {
        out.append(SEP_CHAR, وضعیت)
            .append(region, وضعیت);
    }
    إذا (not_خالی(variant)) {
       out.append(SEP_CHAR, وضعیت)
           .append(variant, وضعیت);
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
    UVector& مقصد_Be_Freed,
    UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = اشاره_تهی;
    إذا (uprv_strchr(replacement, '_') == اشاره_تهی) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        أرجع;
    }
    // We have multiple field so we have to allocate and parse
    خودکار str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), وضعیت, وضعیت);
    حرف* داده = str->داده();
    مقصد_Be_Freed.adoptElement(str.release(), وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    replacedLanguage = (ثابت حرف*) داده;
    حرف* انتها_از_فیلد = uprv_strchr(داده, '_');
    *انتها_از_فیلد = '\0'; // null terminiate it.
    انتها_از_فیلد++;
    ثابت حرف* شروع = انتها_از_فیلد;
    انتها_از_فیلد = const_cast<حرف*>(uprv_strchr(شروع, '_'));
    size_t len = 0;
    إذا (انتها_از_فیلد == اشاره_تهی) {
        len = uprv_strlen(شروع);
    } وإلا {
        len = انتها_از_فیلد - شروع;
        *انتها_از_فیلد = '\0'; // null terminiate it.
    }
    إذا (len == 4 && uprv_isASCIILetter(*شروع)) {
        // Got a script
        replacedScript = شروع;
        إذا (انتها_از_فیلد == اشاره_تهی) {
            أرجع;
        }
        شروع = انتها_از_فیلد++;
        انتها_از_فیلد = const_cast<حرف*>(uprv_strchr(شروع, '_'));
        إذا (انتها_از_فیلد == اشاره_تهی) {
            len = uprv_strlen(شروع);
        } وإلا {
            len = انتها_از_فیلد - شروع;
            *انتها_از_فیلد = '\0'; // null terminiate it.
        }
    }
    إذا (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = شروع;
        إذا (انتها_از_فیلد == اشاره_تهی) {
            أرجع;
        }
        شروع = انتها_از_فیلد++;
        انتها_از_فیلد = const_cast<حرف*>(uprv_strchr(شروع, '_'));
        إذا (انتها_از_فیلد == اشاره_تهی) {
            len = uprv_strlen(شروع);
        } وإلا {
            len = انتها_از_فیلد - شروع;
            *انتها_از_فیلد = '\0'; // null terminiate it.
        }
    }
    إذا (len >= 4) {
        // Got a variant
        replacedVariant = شروع;
        إذا (انتها_از_فیلد == اشاره_تهی) {
            أرجع;
        }
        شروع = انتها_از_فیلد++;
    }
    replacedExtensions = شروع;
}

منطقي
AliasReplacer::replaceLanguage(
        منطقي checkLanguage, منطقي checkRegion,
        منطقي checkVariants, UVector& مقصد_Be_Freed, UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    إذا (    (checkRegion && region == اشاره_تهی) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        أرجع خطأ;
    }
    int32_t variant_اندازه = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ثابت حرف* جستجو_Language = checkLanguage ? language : "und";
    ثابت حرف* جستجو_Region = checkRegion ? region : اشاره_تهی;
    ثابت حرف* جستجو_Variant = اشاره_تهی;
    لكل (int32_t variant_شاخص = 0;
            variant_شاخص < variant_اندازه;
            variant_شاخص++) {
        إذا (checkVariants) {
            U_ASSERT(variant_شاخص < variant_اندازه);
            جستجو_Variant = static_cast<ثابت حرف*>(variants.elementAt(variant_شاخص));
        }

        إذا (جستجو_Variant != اشاره_تهی && uprv_strlen(جستجو_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            جستجو_Variant = اشاره_تهی;
        }
        CharString type_کلید;
        generate_کلید(جستجو_Language, جستجو_Region, جستجو_Variant, type_کلید,
                    وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            أرجع خطأ;
        }
        ثابت حرف *replacement = داده->languageMap().get(type_کلید.داده());
        إذا (replacement == اشاره_تهی) {
            // Found no replacement data.
            استمر;
        }

        ثابت حرف* replacedLanguage = اشاره_تهی;
        ثابت حرف* replacedScript = اشاره_تهی;
        ثابت حرف* replacedRegion = اشاره_تهی;
        ثابت حرف* replacedVariant = اشاره_تهی;
        ثابت حرف* replacedExtensions = اشاره_تهی;
        parseLanguageReplacement(replacement,
                                 replacedLanguage,
                                 replacedScript,
                                 replacedRegion,
                                 replacedVariant,
                                 replacedExtensions,
                                 مقصد_Be_Freed,
                                 وضعیت);
        replacedLanguage =
            (replacedLanguage != اشاره_تهی && uprv_strcmp(replacedLanguage, "und") == 0) ?
            language : replacedLanguage;
        replacedScript = deleteOrReplace(script, اشاره_تهی, replacedScript);
        replacedRegion = deleteOrReplace(region, جستجو_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            جستجو_Variant, جستجو_Variant, replacedVariant);

        إذا (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(جستجو_Variant, replacedVariant) &&
                replacedExtensions == اشاره_تهی) {
            // Replacement produce no changes.
            استمر;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        إذا (جستجو_Variant != اشاره_تهی) {
            إذا (not_خالی(replacedVariant)) {
                variants.مجموعه_Element_At((فراغ*)replacedVariant, variant_شاخص);
            } وإلا {
                variants.removeElementAt(variant_شاخص);
            }
        }
        إذا (replacedExtensions != اشاره_تهی) {
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
        أرجع درست;
    }
    // Nothing changed by language alias data.
    أرجع خطأ;
}

منطقي
AliasReplacer::replaceTerritory(UVector& مقصد_Be_Freed, UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    إذا (region == اشاره_تهی) {
        // No region to search.
        أرجع خطأ;
    }
    ثابت حرف *replacement = داده->territoryMap().get(region);
    إذا (replacement == اشاره_تهی) {
        // Found no replacement data for this region.
        أرجع خطأ;
    }
    ثابت حرف* replacedRegion = replacement;
    ثابت حرف* اولی_Space = uprv_strchr(replacement, ' ');
    إذا (اولی_Space != اشاره_تهی) {
        // If there are are more than one region in the replacement.
        // We need to check which one match based on the language.
        // Cannot use nullptr for language because that will construct
        // the default locale, in that case, use "und" to get the correct
        // locale.
        Locale l = LocaleBuilder()
            .مجموعه_Language(language == اشاره_تهی ? "und" : language)
            .مجموعه_Script(script)
            .build(وضعیت);
        l.جمع_کن_Likely_Subtags(وضعیت);
        ثابت حرف* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> قلم;
        إذا (likelyRegion != اشاره_تهی && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ثابت حرف* یافت_درون_Replacement = uprv_strstr(replacement,
                                                         likelyRegion);
            إذا (یافت_درون_Replacement != اشاره_تهی) {
                // Assuming the case there are no three letter region code in
                // the replacement of territoryAlias
                U_ASSERT(یافت_درون_Replacement == replacement ||
                         *(یافت_درون_Replacement-1) == ' ');
                U_ASSERT(یافت_درون_Replacement[len] == ' ' ||
                         یافت_درون_Replacement[len] == '\0');
                قلم = prv::make_unique<CharString>(
                    یافت_درون_Replacement, static_cast<int32_t>(len), وضعیت, وضعیت);
            }
        }
        إذا (!قلم && U_SUCCESS(وضعیت)) {
            قلم = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(اولی_Space - replacement), وضعیت, وضعیت);
        }
        إذا (U_FAILURE(وضعیت)) { أرجع خطأ; }
        replacedRegion = قلم->داده();
        مقصد_Be_Freed.adoptElement(قلم.release(), وضعیت);
        إذا (U_FAILURE(وضعیت)) { أرجع خطأ; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    أرجع درست;
}

منطقي
AliasReplacer::replaceScript(UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    إذا (script == اشاره_تهی) {
        // No script to search.
        أرجع خطأ;
    }
    ثابت حرف *replacement = داده->scriptMap().get(script);
    إذا (replacement == اشاره_تهی) {
        // Found no replacement data for this script.
        أرجع خطأ;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    أرجع درست;
}

منطقي
AliasReplacer::replaceVariant(UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    // Since we may have more than one variant, we need to loop through them.
    لكل (int32_t i = 0; i < variants.size(); i++) {
        ثابت حرف* variant = static_cast<ثابت حرف*>(variants.elementAt(i));
        ثابت حرف *replacement = داده->variantMap().get(variant);
        إذا (replacement == اشاره_تهی) {
            // Found no replacement data for this variant.
            استمر;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        إذا (!same(variant, replacement)) {
            variants.مجموعه_Element_At((فراغ*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            إذا (uprv_strcmp(variant, "heploc") == 0) {
                لكل (int32_t j = 0; j < variants.size(); j++) {
                     إذا (uprv_strcmp((ثابت حرف*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            أرجع درست;
        }
    }
    أرجع خطأ;
}

منطقي
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    ثابت حرف *replacement = داده->subdivisionMap().get(subdivision.داده());
    إذا (replacement != اشاره_تهی) {
        ثابت حرف* اولی_Space = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (اولی_Space != اشاره_تهی) ?
            (اولی_Space - replacement) : uprv_strlen(replacement);
        إذا (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), وضعیت);
            إذا (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, وضعیت);
            }
        }
        أرجع درست;
    }
    أرجع خطأ;
}

منطقي
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UError_کد& وضعیت)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    int32_t len = transformedExtensions.length();
    ثابت حرف* str = transformedExtensions.داده();
    ثابت حرف* tkey = ultag_get_TKey_شروع(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == اشاره_تهی) ? len : static_cast<int32_t>((tkey - str - 1)));
    إذا (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .مجموعه_Language_برچسب(StringPiece(str, tlangLen))
            .build(وضعیت);
        tlang.canonicalize(وضعیت);
        output = tlang.مقصد_Language_برچسب<CharString>(وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            أرجع خطأ;
        }
        T_CString_مقصد_پایینی_Case(output.داده());
    }
    إذا (tkey != اشاره_تهی) {
        // We need to sort the tfields by tkey
        UVector tfields(وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            أرجع خطأ;
        }
        افعل {
            ثابت حرف* tvalue = uprv_strchr(tkey, '-');
            إذا (tvalue == اشاره_تهی) {
                وضعیت = U_ILLEGAL_آرگومان_ERROR;
                أرجع خطأ;
            }
            ثابت حرف* بعدی_TKey = ultag_get_TKey_شروع(tvalue);
            إذا (بعدی_TKey != اشاره_تهی) {
                *const_cast<حرف*>(بعدی_TKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.درج_Element_At((فراغ*)tkey, tfields.size(), وضعیت);
            إذا (U_FAILURE(وضعیت)) {
                أرجع خطأ;
            }
            tkey = بعدی_TKey;
        } طالما (tkey != اشاره_تهی);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            أرجع uprv_strcmp((ثابت حرف*)e1.اشاره_گر, (ثابت حرف*)e2.اشاره_گر);
        }, وضعیت);
        لكل (int32_t i = 0; i < tfields.size(); i++) {
             إذا (output.length() > 0) {
                 output.append('-', وضعیت);
             }
             ثابت حرف* tfield = static_cast<ثابت حرف*>(tfields.elementAt(i));
             ثابت حرف* tvalue = uprv_strchr(tfield, '-');
             إذا (tvalue == اشاره_تهی) {
                 وضعیت = U_ILLEGAL_آرگومان_ERROR;
                 أرجع خطأ;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<حرف*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, وضعیت).append('-', وضعیت);
             std::optional<std::string_view> bcpTValue = ulocimp_مقصد_Bcp_Type(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, وضعیت);
        }
    }
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    أرجع درست;
}

CharString&
AliasReplacer::output_مقصد_String(
    CharString& out U_LIFETIME_BOUND, UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) { أرجع out; }
    out.append(language, وضعیت);
    إذا (not_خالی(script)) {
        out.append(SEP_CHAR, وضعیت)
            .append(script, وضعیت);
    }
    إذا (not_خالی(region)) {
        out.append(SEP_CHAR, وضعیت)
            .append(region, وضعیت);
    }
    إذا (variants.size() > 0) {
        إذا (!not_خالی(script) && !not_خالی(region)) {
          out.append(SEP_CHAR, وضعیت);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            أرجع uprv_strcmp((ثابت حرف*)e1.اشاره_گر, (ثابت حرف*)e2.اشاره_گر);
        }, وضعیت);
        int32_t variants_شروع = out.length();
        لكل (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, وضعیت)
                 .append(static_cast<ثابت حرف*>(variants.elementAt(i)),
                         وضعیت);
        }
        T_CString_مقصد_فوقانی_Case(out.داده() + variants_شروع);
    }
    إذا (not_خالی(extensions)) {
        CharString tmp("und_", وضعیت);
        tmp.append(extensions, وضعیت);
        Locale tmpLocale(tmp.داده());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, وضعیت);
    }
    أرجع out;
}

منطقي
AliasReplacer::replace(ثابت Locale& locale, CharString& out, UError_کد& وضعیت)
{
    داده = Alias_داده::singleton(وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    U_ASSERT(داده != اشاره_تهی);
    out.clear();
    language = locale.getLanguage();
    إذا (!not_خالی(language)) {
        language = اشاره_تهی;
    }
    script = locale.getScript();
    إذا (!not_خالی(script)) {
        script = اشاره_تهی;
    }
    region = locale.getCountry();
    إذا (!not_خالی(region)) {
        region = اشاره_تهی;
    }
    ثابت حرف* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, وضعیت);
    إذا (!variantsBuff.is_خالی()) {
        إذا (U_FAILURE(وضعیت)) { أرجع خطأ; }
        حرف* شروع = variantsBuff.داده();
        T_CString_مقصد_پایینی_Case(شروع);
        حرف* انتها;
        طالما ((انتها = uprv_strchr(شروع, SEP_CHAR)) != اشاره_تهی &&
               U_SUCCESS(وضعیت)) {
            *انتها = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            إذا (*شروع && !variants.شامل_است(شروع)) {
                variants.جمع_کن_Element(شروع, وضعیت);
            }
            شروع = انتها + 1;
        }
        // do not add "" or duplicate data to variants
        إذا (*شروع && !variants.شامل_است(شروع)) {
            variants.جمع_کن_Element(شروع, وضعیت);
        }
    }
    إذا (U_FAILURE(وضعیت)) { أرجع خطأ; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        أرجع uprv_strcmp((ثابت حرف*)e1.اشاره_گر, (ثابت حرف*)e2.اشاره_گر);
    }, وضعیت);

    // A changed count to assert when loop too many times.
    int32_t تغییریافته = 0;
    ثابت int32_t max_تغییریافته = 4 + variants.size();
    (فراغ)max_تغییریافته; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector رشتهها_مقصد_Be_Freed([](فراغ *obj) { حذف static_cast<CharString*>(obj); },
                             اشاره_تهی, 10, وضعیت);
    طالما (U_SUCCESS(وضعیت)) {
        // Something wrong with the data cause looping here more than maxChanged
        // times already.
        U_ASSERT(تغییریافته <= max_تغییریافته);
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
        إذا (    replaceLanguage(درست, درست,  درست,  رشتهها_مقصد_Be_Freed, وضعیت) ||
                replaceLanguage(درست, درست,  خطأ, رشتهها_مقصد_Be_Freed, وضعیت) ||
                replaceLanguage(درست, خطأ, درست,  رشتهها_مقصد_Be_Freed, وضعیت) ||
                replaceLanguage(درست, خطأ, خطأ, رشتهها_مقصد_Be_Freed, وضعیت) ||
                replaceLanguage(خطأ,خطأ, درست,  رشتهها_مقصد_Be_Freed, وضعیت) ||
                replaceTerritory(رشتهها_مقصد_Be_Freed, وضعیت) ||
                replaceScript(وضعیت) ||
                replaceVariant(وضعیت)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            تغییریافته++;
            استمر;
        }
        // Nothing changed. Break out.
        اكسر;
    }  // while(1)

    إذا (U_FAILURE(وضعیت)) { أرجع خطأ; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ثابت حرف* extensionsStr = locale_get_Keywords_شروع(locale.getName());
    إذا (تغییریافته == 0 && variants.size() <= 1 && extensionsStr == اشاره_تهی) {
        أرجع خطأ;
    }
    output_مقصد_String(out, وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    إذا (extensionsStr != اشاره_تهی) {
        تغییریافته = 0;
        Locale موقت(locale);
        Local_اشاره_گر<icu::StringEnumeration> iter(locale.createKeywords(وضعیت));
        إذا (U_SUCCESS(وضعیت) && !iter.isNull()) {
            ثابت حرف* کلید;
            طالما ((کلید = iter->next(اشاره_تهی, وضعیت)) != اشاره_تهی) {
                إذا (uprv_strcmp("sd", کلید) == 0 || uprv_strcmp("rg", کلید) == 0 ||
                        uprv_strcmp("t", کلید) == 0) {
                    خودکار value = locale.get_Keyword_مقدار<CharString>(کلید, وضعیت);
                    إذا (U_FAILURE(وضعیت)) {
                        وضعیت = U_ZERO_ERROR;
                        استمر;
                    }
                    CharString replacement;
                    إذا (uprv_strlen(کلید) == 2) {
                        إذا (replaceSubdivision(value.مقصد_String_Piece(), replacement, وضعیت)) {
                            تغییریافته++;
                            موقت.مجموعه_Keyword_مقدار(کلید, replacement.داده(), وضعیت);
                        }
                    } وإلا {
                        U_ASSERT(uprv_strcmp(کلید, "t") == 0);
                        إذا (replaceTransformedExtensions(value, replacement, وضعیت)) {
                            تغییریافته++;
                            موقت.مجموعه_Keyword_مقدار(کلید, replacement.داده(), وضعیت);
                        }
                    }
                    إذا (U_FAILURE(وضعیت)) {
                        أرجع خطأ;
                    }
                }
            }
        }
        إذا (تغییریافته != 0) {
            extensionsStr = locale_get_Keywords_شروع(موقت.getName());
        }
        out.append(extensionsStr, وضعیت);
    }
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    // If the tag is not changed, return.
    إذا (uprv_strcmp(out.داده(), locale.getName()) == 0) {
        out.clear();
        أرجع خطأ;
    }
    أرجع درست;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
منطقي
canonicalizeLocale(ثابت Locale& locale, CharString& out, UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) { أرجع خطأ; }
    AliasReplacer replacer(وضعیت);
    أرجع replacer.replace(locale, out, وضعیت);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
منطقي
is_معلوم_Canonicalized_Locale(ثابت حرف* locale, UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) { أرجع خطأ; }

    إذا (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        أرجع درست;
    }

    // common well-known Canonicalized.
    umtx_initOnce(g_معلوم_Canonicalized_Init_Once,
                  &بارگیری_معلوم_Canonicalized, وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    U_ASSERT(g_معلوم_Canonicalized != اشاره_تهی);
    أرجع uhash_geti(g_معلوم_Canonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_انتها

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
    UError_کد وضعیت = U_ZERO_ERROR;
    CharString موقت;
    أرجع !canonicalizeLocale(l, موقت, وضعیت) && U_SUCCESS(وضعیت);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ثابت حرف* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    أرجع localeID == اشاره_تهی ? *هذا = getDefault() : init(StringPiece{localeID}, canonicalize);
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
        حرف *فیلد_2[5] = {اشاره_تهی};
        int32_t فیلد_Len[5] = {0};
        int32_t فیلد_Idx;
        int32_t variant_فیلد;
        int32_t length;
        UError_کد err;

        ثابت خودکار parse = [canonicalize](std::string_view localeID,
                                          حرف* name,
                                          int32_t name_ظرفیت,
                                          UError_کد& وضعیت) {
            أرجع ByteSinkUtil::از_طریق_Byte_Sink_مقصد_Terminated_Chars(
                name, name_ظرفیت,
                [&](ByteSink& sink, UError_کد& وضعیت) {
                    إذا (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, وضعیت);
                    } وإلا {
                        ulocimp_getName(localeID, sink, وضعیت);
                    }
                },
                وضعیت);
        };

        // "canonicalize" the locale ID to ICU/Java format
        حرف* پر_Name = nest.پایه_Name;
        err = U_ZERO_ERROR;
        length = parse(localeID, پر_Name, حجم Nest::پایه_Name, err);

        FixedString پر_Name_میانگیر;
        إذا (err == U_میانگیر_سرریز_ERROR || length >= static_cast<int32_t>(حجم Nest::پایه_Name)) {
            /*Go to heap for the fullName if necessary*/
            إذا (!پر_Name_میانگیر.reserve(length + 1)) {
                اكسر; // error: out of memory
            }
            پر_Name = پر_Name_میانگیر.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, پر_Name, length + 1, err);
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
        separator = فیلد_2[0] = پر_Name;
        فیلد_Idx = 1;
        حرف* at = uprv_strchr(پر_Name, '@');
        طالما ((separator = uprv_strchr(فیلد_2[فیلد_Idx-1], SEP_CHAR)) != اشاره_تهی &&
               فیلد_Idx < UPRV_LENGTHOF(فیلد_2)-1 &&
               (at == اشاره_تهی || separator < at)) {
            فیلد_2[فیلد_Idx] = separator + 1;
            فیلد_Len[فیلد_Idx - 1] = static_cast<int32_t>(separator - فیلد_2[فیلد_Idx - 1]);
            فیلد_Idx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(فیلد_2[فیلد_Idx-1], '@');
        حرف* sep2 = uprv_strchr(فیلد_2[فیلد_Idx-1], '.');
        إذا (separator!=اشاره_تهی || sep2!=اشاره_تهی) {
            إذا (separator==اشاره_تهی || (sep2!=اشاره_تهی && separator > sep2)) {
                separator = sep2;
            }
            فیلد_Len[فیلد_Idx - 1] = static_cast<int32_t>(separator - فیلد_2[فیلد_Idx - 1]);
        } وإلا {
            فیلد_Len[فیلد_Idx - 1] = length - static_cast<int32_t>(فیلد_2[فیلد_Idx - 1] - پر_Name);
        }
        منطقي hasKeywords = at != اشاره_تهی && uprv_strchr(at + 1, '=') != اشاره_تهی;

        إذا (فیلد_Len[0] >= ULOC_LANG_ظرفیت)
        {
            اكسر; // error: the language field is too long
        }

        variant_فیلد = 1; /* Usually the 2nd one, except when a script or country is also used. */
        إذا (فیلد_Len[0] > 0) {
            /* We have a language */
            language = {پر_Name, static_cast<std::string_view::size_type>(فیلد_Len[0])};
        }
        إذا (فیلد_Len[1] == 4 && uprv_isASCIILetter(فیلد_2[1][0]) &&
                uprv_isASCIILetter(فیلد_2[1][1]) && uprv_isASCIILetter(فیلد_2[1][2]) &&
                uprv_isASCIILetter(فیلد_2[1][3])) {
            /* We have at least a script */
            script = {فیلد_2[1], static_cast<std::string_view::size_type>(فیلد_Len[1])};
            variant_فیلد++;
        }

        إذا (فیلد_Len[variant_فیلد] == 2 || فیلد_Len[variant_فیلد] == 3) {
            /* We have a country */
            region = {فیلد_2[variant_فیلد], static_cast<std::string_view::size_type>(فیلد_Len[variant_فیلد])};
            variant_فیلد++;
        } وإلا إذا (فیلد_Len[variant_فیلد] == 0) {
            variant_فیلد++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        إذا (فیلد_Len[variant_فیلد] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(فیلد_2[variant_فیلد] - پر_Name);
        } وإلا إذا (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - پر_Name);
        }

        إذا (!hasKeywords && Nest::میگنجد(length, language, script, region)) {
            U_ASSERT(پر_Name == nest.پایه_Name);
            U_ASSERT(پر_Name_میانگیر.is_خالی());
            nest.init(language, script, region, variantBegin);
        } وإلا {
            إذا (پر_Name == nest.پایه_Name) {
                U_ASSERT(پر_Name_میانگیر.is_خالی());
                پر_Name_میانگیر = {پر_Name, static_cast<std::string_view::size_type>(length)};
                إذا (پر_Name_میانگیر.is_خالی()) {
                    اكسر; // error: out of memory
                }
                إذا (!language.empty()) {
                    language = {پر_Name_میانگیر.داده(), language.size()};
                }
                إذا (!script.empty()) {
                    script = {پر_Name_میانگیر.داده() + (script.داده() - پر_Name), script.size()};
                }
                إذا (!region.empty()) {
                    region = {پر_Name_میانگیر.داده() + (region.داده() - پر_Name), region.size()};
                }
            }
            انباشته& انباشته_2 = payload.emplace<انباشته>(language, script, region, variantBegin);
            إذا (isBogus()) {
                اكسر; // error: out of memory
            }
            U_ASSERT(!پر_Name_میانگیر.is_خالی());
            انباشته_2.ptr->پر_Name = std::move(پر_Name_میانگیر);
            إذا (hasKeywords) {
                إذا (std::string_view::size_type پایه_Name_درازی = at - پر_Name; پایه_Name_درازی > 0) {
                    انباشته_2.ptr->پایه_Name = {انباشته_2.ptr->پر_Name.داده(), پایه_Name_درازی};
                    إذا (انباشته_2.ptr->پایه_Name.is_خالی()) {
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
                    init(replaced.داده(), خطأ);
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
    مجموعه_مقصد_Bogus();

    أرجع *هذا;
}

int32_t
Locale::hash_کد() ثابت
{
    أرجع ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

فراغ
Locale::مجموعه_مقصد_Bogus() {
    /* Free our current storage */
    payload.مجموعه_مقصد_Bogus();
}

ثابت Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex قفل(&gDefaultLocaleMutex);
        إذا (gDefaultLocale != اشاره_تهی) {
            أرجع *gDefaultLocale;
        }
    }
    UError_کد وضعیت = U_ZERO_ERROR;
    أرجع *locale_مجموعه_default_داخلی(اشاره_تهی, وضعیت);
}



فراغ U_EXPORT2
Locale::مجموعه_Default( ثابت   Locale&     newLocale,
                            UError_کد&  وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ثابت حرف *localeID = newLocale.getName();
    locale_مجموعه_default_داخلی(localeID, وضعیت);
}

فراغ
Locale::جمع_کن_Likely_Subtags(UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    CharString maximizedLocaleID = ulocimp_جمع_کن_Likely_Subtags(getName(), وضعیت);

    إذا (U_FAILURE(وضعیت)) {
        إذا (وضعیت == U_حافظه_تخصیص_ERROR) {
            مجموعه_مقصد_Bogus();
        }
        أرجع;
    }

    init(maximizedLocaleID.داده(), /*canonicalize=*/خطأ);
    إذا (isBogus()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
    }
}

فراغ
Locale::minimizeSubtags(UError_کد& وضعیت) {
    Locale::minimizeSubtags(خطأ, وضعیت);
}
فراغ
Locale::minimizeSubtags(منطقي favorScript, UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, وضعیت);

    إذا (U_FAILURE(وضعیت)) {
        إذا (وضعیت == U_حافظه_تخصیص_ERROR) {
            مجموعه_مقصد_Bogus();
        }
        أرجع;
    }

    init(minimizedLocaleID.داده(), /*canonicalize=*/خطأ);
    إذا (isBogus()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
    }
}

فراغ
Locale::canonicalize(UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    إذا (isBogus()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    CharString uncanonicalized(getName(), وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        إذا (وضعیت == U_حافظه_تخصیص_ERROR) {
            مجموعه_مقصد_Bogus();
        }
        أرجع;
    }
    init(uncanonicalized.داده(), /*canonicalize=*/درست);
    إذا (isBogus()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
    }
}

Locale U_EXPORT2
Locale::for_Language_برچسب(StringPiece برچسب, UError_کد& وضعیت)
{
    Locale نتیجه(Locale::eBOGUS);

    إذا (U_FAILURE(وضعیت)) {
        أرجع نتیجه;
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

    int32_t parsed_درازی;
    CharString localeID = ulocimp_for_Language_برچسب(
            برچسب.داده(),
            برچسب.length(),
            &parsed_درازی,
            وضعیت);

    إذا (U_FAILURE(وضعیت)) {
        أرجع نتیجه;
    }

    إذا (parsed_درازی != برچسب.size()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع نتیجه;
    }

    نتیجه.init(localeID.داده(), /*canonicalize=*/خطأ);
    إذا (نتیجه.isBogus()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
    }
    أرجع نتیجه;
}

فراغ
Locale::مقصد_Language_برچسب(ByteSink& sink, UError_کد& وضعیت) ثابت
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    إذا (isBogus()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    ulocimp_مقصد_Language_برچسب(getName(), sink, /*strict=*/خطأ, وضعیت);
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
    loc.init(name, درست);
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
فراغ Locale::مجموعه_From_POSIXID(ثابت حرف *posixID)
{
    init(posixID, درست);
}

ثابت Locale & U_EXPORT2
Locale::get_ریشه()
{
    أرجع getLocale(e_ریشه);
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
    إذا (localeCache == اشاره_تهی) {
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
    UError_کد وضعیت = U_ZERO_ERROR;
    umtx_initOnce(gLocaleCacheInitOnce, locale_init, وضعیت);
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
    KeywordEnumeration(ثابت حرف *کلیدها, int32_t keywordLen, int32_t current_شاخص, UError_کد &وضعیت)
        : keywords(), length(keywordLen), current(اشاره_تهی) {
        إذا(U_SUCCESS(وضعیت) && keywordLen != 0) {
            إذا(کلیدها == اشاره_تهی || keywordLen < 0) {
                وضعیت = U_ILLEGAL_آرگومان_ERROR;
            } وإلا {
                keywords = {کلیدها, static_cast<std::string_view::size_type>(length)};
                إذا (keywords.is_خالی()) {
                    وضعیت = U_حافظه_تخصیص_ERROR;
                } وإلا {
                    current = keywords.داده() + current_شاخص;
                }
            }
        }
    }

    مجازی ~KeywordEnumeration();

    مجازی StringEnumeration * clone() ثابت override
    {
        UError_کد وضعیت = U_ZERO_ERROR;
        أرجع جديد KeywordEnumeration(
                keywords.داده(), length,
                static_cast<int32_t>(current - keywords.داده()), وضعیت);
    }

    مجازی int32_t count(UError_کد& وضعیت) ثابت override {
        إذا (U_FAILURE(وضعیت)) { أرجع 0; }
        ثابت حرف *kw = keywords.داده();
        int32_t نتیجه = 0;
        طالما(*kw) {
            نتیجه++;
            kw += uprv_strlen(kw)+1;
        }
        أرجع نتیجه;
    }

    مجازی ثابت حرف* next(int32_t* نتیجه_درازی, UError_کد& وضعیت) U_LIFETIME_BOUND override {
        ثابت حرف* نتیجه;
        int32_t len;
        إذا(U_SUCCESS(وضعیت) && *current != 0) {
            نتیجه = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            إذا(نتیجه_درازی != اشاره_تهی) {
                *نتیجه_درازی = len;
            }
        } وإلا {
            إذا(نتیجه_درازی != اشاره_تهی) {
                *نتیجه_درازی = 0;
            }
            نتیجه = اشاره_تهی;
        }
        أرجع نتیجه;
    }

    مجازی ثابت UnicodeString* snext(UError_کد& وضعیت) U_LIFETIME_BOUND override {
        إذا (U_FAILURE(وضعیت)) { أرجع اشاره_تهی; }
        int32_t نتیجه_درازی = 0;
        ثابت حرف *s = next(&نتیجه_درازی, وضعیت);
        أرجع مجموعه_Chars(s, نتیجه_درازی, وضعیت);
    }

    مجازی فراغ reset(UError_کد& وضعیت) override {
        إذا (U_FAILURE(وضعیت)) { أرجع; }
        current = keywords.داده();
    }
};

ثابت حرف KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = افتراضي;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
صنف UnicodeKeywordEnumeration : عام KeywordEnumeration {
عام:
    استفاده KeywordEnumeration::KeywordEnumeration;
    مجازی ~UnicodeKeywordEnumeration();

    مجازی ثابت حرف* next(int32_t* نتیجه_درازی, UError_کد& وضعیت) U_LIFETIME_BOUND override {
        ثابت حرف* legacy_کلید = KeywordEnumeration::next(اشاره_تهی, وضعیت);
        طالما (U_SUCCESS(وضعیت) && legacy_کلید != اشاره_تهی) {
            ثابت حرف* کلید = uloc_مقصد_Unicode_Locale_کلید(legacy_کلید);
            إذا (کلید != اشاره_تهی) {
                إذا (نتیجه_درازی != اشاره_تهی) {
                    *نتیجه_درازی = static_cast<int32_t>(uprv_strlen(کلید));
                }
                أرجع کلید;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_کلید = KeywordEnumeration::next(اشاره_تهی, وضعیت);
        }
        إذا (نتیجه_درازی != اشاره_تهی) *نتیجه_درازی = 0;
        أرجع اشاره_تهی;
    }
    مجازی int32_t count(UError_کد& وضعیت) ثابت override {
        إذا (U_FAILURE(وضعیت)) { أرجع 0; }
        ثابت حرف *kw = keywords.داده();
        int32_t نتیجه = 0;
        طالما(*kw) {
            إذا (uloc_مقصد_Unicode_Locale_کلید(kw) != اشاره_تهی) {
                نتیجه++;
            }
            kw += uprv_strlen(kw)+1;
        }
        أرجع نتیجه;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = افتراضي;

StringEnumeration *
Locale::createKeywords(UError_کد &وضعیت) ثابت
{
    StringEnumeration *نتیجه = اشاره_تهی;

    إذا (U_FAILURE(وضعیت)) {
        أرجع نتیجه;
    }

    ثابت حرف* variant_شروع = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    إذا(variant_شروع) {
        إذا(assignment > variant_شروع) {
            CharString keywords = ulocimp_getKeywords(variant_شروع + 1, '@', خطأ, وضعیت);
            إذا (U_SUCCESS(وضعیت) && !keywords.is_خالی()) {
                نتیجه = جديد KeywordEnumeration(keywords.داده(), keywords.length(), 0, وضعیت);
                إذا (!نتیجه) {
                    وضعیت = U_حافظه_تخصیص_ERROR;
                }
            }
        } وإلا {
            وضعیت = U_INVALID_FORMAT_ERROR;
        }
    }
    أرجع نتیجه;
}

StringEnumeration *
Locale::createUnicodeKeywords(UError_کد &وضعیت) ثابت
{
    StringEnumeration *نتیجه = اشاره_تهی;

    إذا (U_FAILURE(وضعیت)) {
        أرجع نتیجه;
    }

    ثابت حرف* variant_شروع = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    إذا(variant_شروع) {
        إذا(assignment > variant_شروع) {
            CharString keywords = ulocimp_getKeywords(variant_شروع + 1, '@', خطأ, وضعیت);
            إذا (U_SUCCESS(وضعیت) && !keywords.is_خالی()) {
                نتیجه = جديد UnicodeKeywordEnumeration(keywords.داده(), keywords.length(), 0, وضعیت);
                إذا (!نتیجه) {
                    وضعیت = U_حافظه_تخصیص_ERROR;
                }
            }
        } وإلا {
            وضعیت = U_INVALID_FORMAT_ERROR;
        }
    }
    أرجع نتیجه;
}

int32_t
Locale::get_Keyword_مقدار(ثابت حرف* keywordName, حرف *میانگیر, int32_t bufLen, UError_کد &وضعیت) ثابت
{
    أرجع uloc_get_Keyword_مقدار(getName(), keywordName, میانگیر, bufLen, &وضعیت);
}

فراغ
Locale::get_Keyword_مقدار(StringPiece keywordName, ByteSink& sink, UError_کد& وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    إذا (isBogus()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    ulocimp_get_Keyword_مقدار(getName(), keywordName, sink, وضعیت);
}

فراغ
Locale::get_Unicode_Keyword_مقدار(StringPiece keywordName,
                               ByteSink& sink,
                               UError_کد& وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    std::optional<std::string_view> legacy_کلید = ulocimp_مقصد_Legacy_کلید_With_Fallback(keywordName);
    إذا (!legacy_کلید.has_value()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    خودکار legacy_مقدار = get_Keyword_مقدار<CharString>(*legacy_کلید, وضعیت);

    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    std::optional<std::string_view> unicode_مقدار =
        ulocimp_مقصد_Bcp_Type_With_Fallback(keywordName, legacy_مقدار.مقصد_String_Piece());
    إذا (!unicode_مقدار.has_value()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    sink.Append(unicode_مقدار->داده(), static_cast<int32_t>(unicode_مقدار->size()));
}

فراغ
Locale::مجموعه_Keyword_مقدار(StringPiece keywordName,
                        StringPiece keyword_مقدار,
                        UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) { أرجع; }
    إذا (keywordName.empty()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    إذا (وضعیت == U_STRING_NOT_TERMINATED_WARNING) {
        وضعیت = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, وضعیت);
    ulocimp_مجموعه_Keyword_مقدار(keywordName, keyword_مقدار, localeID, وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        إذا (وضعیت == U_حافظه_تخصیص_ERROR) {
            مجموعه_مقصد_Bogus();
        }
        أرجع;
    }

    ثابت حرف* at = locale_get_Keywords_شروع(localeID.مقصد_String_Piece());
    منطقي hasKeywords = at != اشاره_تهی && uprv_strchr(at + 1, '=') != اشاره_تهی;

    Nest* nest = payload.get<Nest>();
    إذا (!hasKeywords) {
        إذا (nest == اشاره_تهی) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            انباشته* انباشته_2 = payload.get<انباشته>();
            U_ASSERT(انباشته_2 != اشاره_تهی);
            إذا (Nest::میگنجد(localeID.length(), انباشته_2->language, انباشته_2->script, انباشته_2->region)) {
                int32_t variantBegin = انباشته_2->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < حجم Nest::پایه_Name);
                nest = &payload.emplace<Nest>(std::move(*انباشته_2), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->پایه_Name, حجم Nest::پایه_Name, وضعیت);
            } وإلا {
                انباشته_2->ptr->پایه_Name.clear();
                انباشته_2->ptr->پر_Name = localeID.مقصد_String_Piece();
                إذا (انباشته_2->ptr->پر_Name.is_خالی()) {
                    وضعیت = U_حافظه_تخصیص_ERROR;
                    مجموعه_مقصد_Bogus();
                    أرجع;
                }
            }
        }
    } وإلا {
        انباشته* انباشته_2 = اشاره_تهی;
        إذا (nest != اشاره_تهی) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            انباشته_2 = &payload.emplace<انباشته>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            إذا (isBogus()) {
                وضعیت = U_حافظه_تخصیص_ERROR;
                أرجع;
            }
        } وإلا {
            انباشته_2 = payload.get<انباشته>();
        }
        U_ASSERT(انباشته_2 != اشاره_تهی);
        انباشته_2->ptr->پر_Name = localeID.مقصد_String_Piece();
        إذا (انباشته_2->ptr->پر_Name.is_خالی()) {
            وضعیت = U_حافظه_تخصیص_ERROR;
            مجموعه_مقصد_Bogus();
            أرجع;
        }

        إذا (انباشته_2->ptr->پایه_Name.is_خالی()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            إذا (std::string_view::size_type پایه_Name_درازی = at - localeID.داده(); پایه_Name_درازی > 0) {
                انباشته_2->ptr->پایه_Name = {انباشته_2->ptr->پر_Name.داده(), پایه_Name_درازی};
                إذا (انباشته_2->ptr->پایه_Name.is_خالی()) {
                    وضعیت = U_حافظه_تخصیص_ERROR;
                    مجموعه_مقصد_Bogus();
                    أرجع;
                }
            }
        }
    }
}

فراغ
Locale::مجموعه_Unicode_Keyword_مقدار(StringPiece keywordName,
                               StringPiece keyword_مقدار,
                               UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    std::optional<std::string_view> legacy_کلید = ulocimp_مقصد_Legacy_کلید_With_Fallback(keywordName);
    إذا (!legacy_کلید.has_value()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    std::string_view value;

    إذا (!keyword_مقدار.empty()) {
        std::optional<std::string_view> legacy_مقدار =
            ulocimp_مقصد_Legacy_Type_With_Fallback(keywordName, keyword_مقدار);
        إذا (!legacy_مقدار.has_value()) {
            وضعیت = U_ILLEGAL_آرگومان_ERROR;
            أرجع;
        }
        value = *legacy_مقدار;
    }

    مجموعه_Keyword_مقدار(*legacy_کلید, value, وضعیت);
}

ثابت حرف*
Locale::getCountry() ثابت U_LIFETIME_BOUND {
    أرجع get_فیلد<&Nest::getRegion, &انباشته::getRegion>();
}

ثابت حرف*
Locale::getLanguage() ثابت U_LIFETIME_BOUND {
    أرجع get_فیلد<&Nest::getLanguage, &انباشته::getLanguage>();
}

ثابت حرف*
Locale::getScript() ثابت U_LIFETIME_BOUND {
    أرجع get_فیلد<&Nest::getScript, &انباشته::getScript>();
}

ثابت حرف*
Locale::getVariant() ثابت U_LIFETIME_BOUND {
    أرجع get_فیلد<&Nest::getVariant, &انباشته::getVariant>();
}

ثابت حرف*
Locale::getName() ثابت U_LIFETIME_BOUND {
    أرجع get_فیلد<&Nest::get_پایه_Name, &انباشته::get_پر_Name>();
}

ثابت حرف*
Locale::get_پایه_Name() ثابت U_LIFETIME_BOUND {
    أرجع get_فیلد<&Nest::get_پایه_Name, &انباشته::get_پایه_Name>();
}

قالب <ثابت حرف* (Locale::Nest::*ثابت NEST)() ثابت,
          ثابت حرف* (Locale::انباشته::*ثابت انباشته_3)() ثابت>
ثابت حرف* Locale::get_فیلد() ثابت U_LIFETIME_BOUND {
    أرجع payload.بازدید([] { أرجع ""; },
                         [](ثابت Nest& nest) { أرجع (nest.*NEST)(); },
                         [](ثابت انباشته& انباشته_2) { أرجع (انباشته_2.*انباشته_3)(); });
}

Locale::Iterator::~Iterator() = افتراضي;

//eof
U_NAMESPACE_انتها
