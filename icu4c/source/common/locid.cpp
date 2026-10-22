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

#شامل <cstddef>
#شامل <optional>
#شامل <string_view>
#شامل <type_traits>
#شامل <utility>

#شامل "unicode/bytestream.h"
#شامل "unicode/locid.h"
#شامل "unicode/localebuilder.h"
#شامل "unicode/localpointer.h"
#شامل "unicode/strenum.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/uloc.h"
#شامل "unicode/ures.h"

#شامل "bytesinkutil.h"
#شامل "charstr.h"
#شامل "charstrmap.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "fixedstring.h"
#شامل "mutex.h"
#شامل "putilimp.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"
#شامل "uhash.h"
#شامل "ulocimp.h"
#شامل "umutex.h"
#شامل "uniquecharstr.h"
#شامل "ustr_imp.h"
#شامل "uvector.h"

#اگر UPRV_HAS_FEATURE(حافظہ_sanitizer)
#شامل <sanitizer/msan_interface.h>
#اگر_ختم

U_NAMESPACE_BEGIN

ساکن Locale   *gLocaleCache = خالی_اشاریہ;
ساکن UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
ساکن UMutex gDefaultLocaleMutex;
ساکن UHashtable *gDefaultLocalesHashT = خالی_اشاریہ;
ساکن Locale *gDefaultLocale = خالی_اشاریہ;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#تعریف_کرو ULOC_STRING_حد_بندی 357913941

U_NAMESPACE_انتہا

قسمتعریف شمار ELocalePos {
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

نامخلا {

//
// Deleter function for Locales owned by the default Locale hash table/
//
خالی U_CALLCONV
deleteLocale(خالی *obj) {
    حذف static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    حذف [] gLocaleCache;
    gLocaleCache = خالی_اشاریہ;
    gLocaleCacheInitOnce.reset();

    اگر (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = خالی_اشاریہ;
    }
    gDefaultLocale = خالی_اشاریہ;
    واپس سچ;
}

خالی U_CALLCONV locale_init(UError_رمز &حالت_2) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == خالی_اشاریہ);
    gLocaleCache = نیا Locale[static_cast<صحیح>(eMAX_LOCALES)];
    اگر (gLocaleCache == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
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

Locale *locale_سیٹ_default_اندرونی(ثابت حرف *id, UError_رمز& حالت_2) {
    // Synchronize this entire function.
    Mutex تالا(&gDefaultLocaleMutex);

    UBool canonicalize = جھوٹ;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    اگر (id == خالی_اشاریہ) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = سچ; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, حالت_2) : ulocimp_getName(id, حالت_2);

    اگر (U_FAILURE(حالت_2)) {
        واپس gDefaultLocale;
    }

    اگر (gDefaultLocalesHashT == خالی_اشاریہ) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, خالی_اشاریہ, &حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            واپس gDefaultLocale;
        }
        uhash_سیٹ_قدر_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.ڈیٹا()));
    اگر (newDefault == خالی_اشاریہ) {
        newDefault = نیا Locale(Locale::eBOGUS);
        اگر (newDefault == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس gDefaultLocale;
        }
        newDefault->init(localeNameBuf.ڈیٹا(), جھوٹ);
        uhash_put(gDefaultLocalesHashT, const_cast<حرف*>(newDefault->getName()), newDefault, &حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            واپس gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    واپس gDefaultLocale;
}

U_NAMESPACE_انتہا

/* sfb 07/21/99 */
U_CFUNC خالی
locale_سیٹ_default(ثابت حرف *id)
{
    U_NAMESPACE_USE
    UError_رمز حالت_2 = U_ZERO_ERROR;
    locale_سیٹ_default_اندرونی(id, حالت_2);
}
/*انتہا*/

U_CFUNC ثابت حرف *
locale_get_default()
{
    U_NAMESPACE_USE
    واپس Locale::getDefault().getName();
}

نامخلا {

سانچہ <خودکار میدان, قسم_نام T>
خالی copy_مقصد_Array(std::string_view sv, T* that) {
    خودکار& میدان_2 = that->*میدان;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(میدان_2)>>;
    static_assert(capacity > 0);
    اگر (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(میدان_2, sv.ڈیٹا(), sv.size());
    }
    میدان_2[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

خالی Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copy_مقصد_Array<&Nest::language>(language, یہ);
    copy_مقصد_Array<&Nest::script>(script, یہ);
    copy_مقصد_Array<&Nest::region>(region, یہ);
    یہ->variantBegin = variantBegin;
}

Locale::Nest::Nest(ڈھیر_2&& ڈھیر_3, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(ڈھیر_2, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(ڈھیر_2, region));
    U_ASSERT(یہ == reinterpret_cast<Nest*>(&ڈھیر_3));
#اگر UPRV_HAS_FEATURE(حافظہ_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&ڈھیر_3, حجم(ڈھیر_2));
#اگر_ختم
    copy_مقصد_Array<&Nest::script>(ڈھیر_3.script, یہ);
    copy_مقصد_Array<&Nest::region>(ڈھیر_3.region, یہ);
    یہ->variantBegin = variantBegin;
    *یہ->بنیاد_Name = '\0';
}

ساخت Locale::ڈھیر_2::Alloc : عوامی UMemory {
    FixedString بھرا_Name;
    FixedString بنیاد_Name;
    int32_t variantBegin;

    ثابت حرف* getVariant() ثابت U_LIFETIME_BOUND {
        واپس variantBegin == 0 ? "" : get_بنیاد_Name() + variantBegin;
    }
    ثابت حرف* get_بھرا_Name() ثابت U_LIFETIME_BOUND { واپس بھرا_Name.ڈیٹا(); }
    ثابت حرف* get_بنیاد_Name() ثابت U_LIFETIME_BOUND {
        اگر (بنیاد_Name.is_خالی_خانے()) {
            اگر (ثابت حرف* name = بھرا_Name.ڈیٹا(); *name != '@') {
                واپس name;
            }
        }
        واپس بنیاد_Name.ڈیٹا();
    }

    Alloc(int32_t variantBegin) : بھرا_Name(), بنیاد_Name(), variantBegin(variantBegin) {}

    Alloc(ثابت Alloc& other, UError_رمز& حالت_2)
        : بھرا_Name(), بنیاد_Name(), variantBegin(other.variantBegin) {
        اگر (U_SUCCESS(حالت_2)) {
            اگر (!other.بھرا_Name.is_خالی_خانے()) {
                بھرا_Name = other.بھرا_Name;
                اگر (بھرا_Name.is_خالی_خانے()) {
                    حالت_2 = U_حافظہ_تخصیص_ERROR;
                } ورنہ {
                    اگر (!other.بنیاد_Name.is_خالی_خانے()) {
                        بنیاد_Name = other.بنیاد_Name;
                        اگر (بنیاد_Name.is_خالی_خانے()) {
                            حالت_2 = U_حافظہ_تخصیص_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = حذف;

    ~Alloc() = طےشدہ;
};

ثابت حرف* Locale::ڈھیر_2::getVariant() ثابت U_LIFETIME_BOUND { واپس ptr->getVariant(); }
ثابت حرف* Locale::ڈھیر_2::get_بھرا_Name() ثابت U_LIFETIME_BOUND { واپس ptr->get_بھرا_Name(); }
ثابت حرف* Locale::ڈھیر_2::get_بنیاد_Name() ثابت U_LIFETIME_BOUND { واپس ptr->get_بنیاد_Name(); }

Locale::ڈھیر_2::ڈھیر_2(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = نیا Alloc(variantBegin);
    اگر (ptr == خالی_اشاریہ) {
        type = eBOGUS;
    } ورنہ {
        type = e_ڈھیر;
        copy_مقصد_Array<&ڈھیر_2::language>(language, یہ);
        copy_مقصد_Array<&ڈھیر_2::script>(script, یہ);
        copy_مقصد_Array<&ڈھیر_2::region>(region, یہ);
    }
}

Locale::ڈھیر_2::~ڈھیر_2() {
    U_ASSERT(type == e_ڈھیر);
    حذف ptr;
}

Locale::ڈھیر_2& Locale::ڈھیر_2::عامل=(ثابت ڈھیر_2& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UError_رمز حالت_2 = U_ZERO_ERROR;
    ptr = نیا Alloc(*other.ptr, حالت_2);
    اگر (ptr == خالی_اشاریہ || U_FAILURE(حالت_2)) {
        حذف ptr;
    } ورنہ {
        type = e_ڈھیر;
        uprv_memcpy(language, other.language, حجم language);
        uprv_memcpy(script, other.script, حجم script);
        uprv_memcpy(region, other.region, حجم region);
    }
    واپس *یہ;
}

Locale::ڈھیر_2& Locale::ڈھیر_2::عامل=(ڈھیر_2&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = e_ڈھیر;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, حجم language);
    uprv_memcpy(script, other.script, حجم script);
    uprv_memcpy(region, other.region, حجم region);
    واپس *یہ;
}

سانچہ <قسم_نام BogusFn, قسم_نام NestFn, قسم_نام ڈھیر_Fn, قسم_نام... دلائل>
خودکار Locale::Payload::دورہ(BogusFn bogusFn, NestFn nestFn, ڈھیر_Fn ڈھیر_Fn_2, دلائل... دلائل_2) ثابت {
    انتخاب (type) {
        حالت eBOGUS:
            واپس bogusFn(دلائل_2...);
        حالت eNEST:
            واپس nestFn(nest, دلائل_2...);
        حالت e_ڈھیر:
            واپس ڈھیر_Fn_2(ڈھیر_3, دلائل_2...);
        طےشدہ:
            UPRV_UNREACHABLE_EXIT;
    };
}

خالی Locale::Payload::copy(ثابت Payload& other) {
    other.دورہ([](Payload*) {},
                [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ثابت ڈھیر_2& ڈھیر_3, Payload* dst) { dst->ڈھیر_3 = ڈھیر_3; },
                یہ);
}

خالی Locale::Payload::move(Payload&& other) noexcept {
    other.دورہ(
        [](Payload*) {},
        [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ثابت ڈھیر_2& ڈھیر_3, Payload* dst) { dst->ڈھیر_3 = std::move(const_cast<ڈھیر_2&>(ڈھیر_3)); },
        یہ);
}

Locale::Payload::~Payload() {
    اگر (type == e_ڈھیر) { ڈھیر_3.~ڈھیر_2(); }
}

Locale::Payload::Payload(ثابت Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::عامل=(ثابت Payload& other) U_LIFETIME_BOUND {
    اگر (یہ != &other) {
        سیٹ_مقصد_Bogus();
        copy(other);
    }
    واپس *یہ;
}

Locale::Payload& Locale::Payload::عامل=(Payload&& other) noexcept U_LIFETIME_BOUND {
    اگر (یہ != &other) {
        سیٹ_مقصد_Bogus();
        move(std::move(other));
    }
    واپس *یہ;
}

خالی Locale::Payload::سیٹ_مقصد_Bogus() {
    یہ->~Payload();
    type = eBOGUS;
}

سانچہ <قسم_نام T, قسم_نام... دلائل> T& Locale::Payload::emplace(دلائل&&... دلائل_2) U_LIFETIME_BOUND {
    اگر constexpr (std::is_same_v<T, Nest>) {
        یہ->~Payload();
        ::نیا (&nest) Nest(std::forward<دلائل>(دلائل_2)...);
        واپس nest;
    }
    اگر constexpr (std::is_same_v<T, ڈھیر_2>) {
        U_ASSERT(type != e_ڈھیر);
        ::نیا (&ڈھیر_3) ڈھیر_2(std::forward<دلائل>(دلائل_2)...);
        واپس ڈھیر_3;
    }
}

سانچہ <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    واپس type == eNEST ? &nest : خالی_اشاریہ;
}
سانچہ <> Locale::ڈھیر_2* Locale::Payload::get() U_LIFETIME_BOUND {
    واپس type == e_ڈھیر ? &ڈھیر_3 : خالی_اشاریہ;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#تعریف_کرو SEP_CHAR '_'
#تعریف_کرو NULL_CHAR '\0'

Locale::~Locale() = طےشدہ;

Locale::Locale()
    : UObject(), payload()
{
    init(خالی_اشاریہ, جھوٹ);
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
    اگر( (newLanguage==خالی_اشاریہ) && (newCountry == خالی_اشاریہ) && (newVariant == خالی_اشاریہ) )
    {
        init(خالی_اشاریہ, جھوٹ); /* shortcut */
    }
    ورنہ
    {
        UError_رمز حالت_2 = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        اگر ( newLanguage != خالی_اشاریہ )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            اگر ( lsize < 0 || lsize > ULOC_STRING_حد_بندی ) { // int32 wrap
                واپس;
            }
        }

        CharString togo(newLanguage, lsize, حالت_2); // start with newLanguage

        // _Country
        اگر ( newCountry != خالی_اشاریہ )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            اگر ( csize < 0 || csize > ULOC_STRING_حد_بندی ) { // int32 wrap
                واپس;
            }
        }

        // _Variant
        اگر ( newVariant != خالی_اشاریہ )
        {
            // remove leading _'s
            جب(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            اگر ( vsize < 0 || vsize > ULOC_STRING_حد_بندی ) { // int32 wrap
                واپس;
            }
            جب( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        اگر ( newKeywords != خالی_اشاریہ)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            اگر ( ksize < 0 || ksize > ULOC_STRING_حد_بندی ) {
              واپس;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        اگر ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, حالت_2);
        }

        اگر ( csize != 0 )
        {
            togo.append(newCountry, حالت_2);
        }

        اگر ( vsize != 0)
        {
            togo.append(SEP_CHAR, حالت_2)
                .append(newVariant, vsize, حالت_2);
        }

        اگر ( ksize != 0)
        {
            اگر (uprv_strchr(newKeywords, '=')) {
                togo.append('@', حالت_2); /* keyword parsing */
            }
            ورنہ {
                togo.append('_', حالت_2); /* Variant parsing with a script */
                اگر ( vsize == 0) {
                    togo.append('_', حالت_2); /* No country found */
                }
            }
            togo.append(newKeywords, حالت_2);
        }

        اگر (U_FAILURE(حالت_2)) {
            // Something went wrong with appending, etc.
            واپس;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.ڈیٹا(), جھوٹ);
    }
}

Locale::Locale(ثابت Locale&) = طےشدہ;
Locale::Locale(Locale&&) noexcept = طےشدہ;

Locale& Locale::عامل=(ثابت Locale&) U_LIFETIME_BOUND = طےشدہ;
Locale& Locale::عامل=(Locale&&) noexcept U_LIFETIME_BOUND = طےشدہ;

Locale *
Locale::clone() ثابت {
    واپس نیا Locale(*یہ);
}

منطقی
Locale::عامل==( ثابت   Locale& other) ثابت
{
    واپس uprv_strcmp(other.getName(), getName()) == 0;
}

نامخلا {

UInitOnce g_معلوم_Canonicalized_Init_Once {};
UHashtable *g_معلوم_Canonicalized = خالی_اشاریہ;

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
    اگر (g_معلوم_Canonicalized) { uhash_close(g_معلوم_Canonicalized); }
    واپس سچ;
}

خالی U_CALLCONV بھرو_معلوم_Canonicalized(UError_رمز &حالت_2) {
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE_معلوم_CANONICALIZED,
                                cleanup_معلوم_Canonicalized);
    Local_UHashtable_اشارہ_گر new_معلوم_Canonicalized_Map(
        uhash_open(uhash_hashChars, uhash_compareChars, خالی_اشاریہ, &حالت_2));
    برائے (int32_t i = 0;
            U_SUCCESS(حالت_2) && i < UPRV_LENGTHOF(معلوم_CANONICALIZED);
            i++) {
        uhash_puti(new_معلوم_Canonicalized_Map.getAlias(),
                   (خالی*)معلوم_CANONICALIZED[i],
                   1, &حالت_2);
    }
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    g_معلوم_Canonicalized = new_معلوم_Canonicalized_Map.orphan();
}

جماعت Alias_ڈیٹا;

/**
 * A Builder class to build the alias data.
 */
جماعت Alias_ڈیٹا_Builder {
عوامی:
    Alias_ڈیٹا_Builder() {
    }

    // Build the AliasData from resource.
    Alias_ڈیٹا* build(UError_رمز &حالت_2);

نجی:
    خالی readAlias(UResourceBundle* alias,
                   Unique_Char_لڑیاں* لڑیاں,
                   Local_حافظہ<ثابت حرف*>& types,
                   Local_حافظہ<int32_t>& replacementIndexes,
                   int32_t &length,
                   خالی (*checkType)(ثابت حرف* type),
                   خالی (*checkReplacement)(ثابت UChar* replacement),
                   UError_رمز &حالت_2);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    خالی readLanguageAlias(UResourceBundle* alias,
                           Unique_Char_لڑیاں* لڑیاں,
                           Local_حافظہ<ثابت حرف*>& types,
                           Local_حافظہ<int32_t>& replacementIndexes,
                           int32_t &length,
                           UError_رمز &حالت_2);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    خالی readScriptAlias(UResourceBundle* alias,
                         Unique_Char_لڑیاں* لڑیاں,
                         Local_حافظہ<ثابت حرف*>& types,
                         Local_حافظہ<int32_t>& replacementIndexes,
                         int32_t &length, UError_رمز &حالت_2);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    خالی readTerritoryAlias(UResourceBundle* alias,
                            Unique_Char_لڑیاں* لڑیاں,
                            Local_حافظہ<ثابت حرف*>& types,
                            Local_حافظہ<int32_t>& replacementIndexes,
                            int32_t &length, UError_رمز &حالت_2);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    خالی readVariantAlias(UResourceBundle* alias,
                          Unique_Char_لڑیاں* لڑیاں,
                          Local_حافظہ<ثابت حرف*>& types,
                          Local_حافظہ<int32_t>& replacementIndexes,
                          int32_t &length, UError_رمز &حالت_2);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    خالی readSubdivisionAlias(UResourceBundle* alias,
                          Unique_Char_لڑیاں* لڑیاں,
                          Local_حافظہ<ثابت حرف*>& types,
                          Local_حافظہ<int32_t>& replacementIndexes,
                          int32_t &length, UError_رمز &حالت_2);
};

/**
 * A class to hold the Alias Data.
 */
جماعت Alias_ڈیٹا : عوامی UMemory {
عوامی:
    ساکن ثابت Alias_ڈیٹا* singleton(UError_رمز& حالت_2) {
        اگر (U_FAILURE(حالت_2)) {
            // Do not get into loadData if the status already has error.
            واپس خالی_اشاریہ;
        }
        umtx_initOnce(Alias_ڈیٹا::gInitOnce, &Alias_ڈیٹا::بھرو_ڈیٹا, حالت_2);
        واپس gSingleton;
    }

    ثابت CharStringMap& languageMap() ثابت U_LIFETIME_BOUND { واپس language; }
    ثابت CharStringMap& scriptMap() ثابت U_LIFETIME_BOUND { واپس script; }
    ثابت CharStringMap& territoryMap() ثابت U_LIFETIME_BOUND { واپس territory; }
    ثابت CharStringMap& variantMap() ثابت U_LIFETIME_BOUND { واپس variant; }
    ثابت CharStringMap& subdivisionMap() ثابت U_LIFETIME_BOUND { واپس subdivision; }

    ساکن خالی U_CALLCONV بھرو_ڈیٹا(UError_رمز &حالت_2);
    ساکن UBool U_CALLCONV cleanup();

    ساکن UInitOnce gInitOnce;

نجی:
    Alias_ڈیٹا(CharStringMap languageMap,
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

    ~Alias_ڈیٹا() {
        حذف لڑیاں;
    }

    ساکن ثابت Alias_ڈیٹا* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* لڑیاں;

    دوست جماعت Alias_ڈیٹا_Builder;
};


ثابت Alias_ڈیٹا* Alias_ڈیٹا::gSingleton = خالی_اشاریہ;
UInitOnce Alias_ڈیٹا::gInitOnce {};

UBool U_CALLCONV
Alias_ڈیٹا::cleanup()
{
    gInitOnce.reset();
    حذف gSingleton;
    واپس سچ;
}

خالی
Alias_ڈیٹا_Builder::readAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        خالی (*checkType)(ثابت حرف* type),
        خالی (*checkReplacement)(ثابت UChar* replacement),
        UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    length = ures_get_سائز(alias);
    ثابت حرف** rawTypes = types.allocateInsteadAndCopy(length);
    اگر (rawTypes == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    اگر (rawIndexes == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    برائے (صحیح i = 0; U_SUCCESS(حالت_2) && ures_has_اگلا(alias); i++) {
        Local_UResource_Bundle_اشارہ_گر res(
            ures_get_اگلا_Resource(alias, خالی_اشاریہ, &حالت_2));
        ثابت حرف* aliasFrom = ures_get_کلید(res.getAlias());
        ثابت UChar* alias_مقصد =
            ures_get_String_By_کلید(res.getAlias(), "replacement", خالی_اشاریہ, &حالت_2);
        اگر (U_FAILURE(حالت_2)) واپس;

        checkType(aliasFrom);
        checkReplacement(alias_مقصد);

        rawTypes[i] = aliasFrom;
        rawIndexes[i] = لڑیاں->جوڑو(alias_مقصد, حالت_2);
    }
}

/**
 * Read the languageAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement language.
 */
خالی
Alias_ڈیٹا_Builder::readLanguageAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_رمز &حالت_2)
{
    واپس readAlias(
        alias, لڑیاں, types, replacementIndexes, length,
#اگر U_DEBUG
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
#ورنہ
        [](ثابت حرف*) {},
#اگر_ختم
        [](ثابت UChar*) {}, حالت_2);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
خالی
Alias_ڈیٹا_Builder::readScriptAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_رمز &حالت_2)
{
    واپس readAlias(
        alias, لڑیاں, types, replacementIndexes, length,
#اگر U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ثابت UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#ورنہ
        [](ثابت حرف*) {},
        [](ثابت UChar*) { },
#اگر_ختم
        حالت_2);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
خالی
Alias_ڈیٹا_Builder::readTerritoryAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_رمز &حالت_2)
{
    واپس readAlias(
        alias, لڑیاں, types, replacementIndexes, length,
#اگر U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#ورنہ
        [](ثابت حرف*) {},
#اگر_ختم
        [](ثابت UChar*) { },
        حالت_2);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
خالی
Alias_ڈیٹا_Builder::readVariantAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_رمز &حالت_2)
{
    واپس readAlias(
        alias, لڑیاں, types, replacementIndexes, length,
#اگر U_DEBUG
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
#ورنہ
        [](ثابت حرف*) {},
        [](ثابت UChar*) { },
#اگر_ختم
        حالت_2);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
خالی
Alias_ڈیٹا_Builder::readSubdivisionAlias(
        UResourceBundle* alias,
        Unique_Char_لڑیاں* لڑیاں,
        Local_حافظہ<ثابت حرف*>& types,
        Local_حافظہ<int32_t>& replacementIndexes,
        int32_t &length,
        UError_رمز &حالت_2)
{
    واپس readAlias(
        alias, لڑیاں, types, replacementIndexes, length,
#اگر U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#ورنہ
        [](ثابت حرف*) {},
#اگر_ختم
        [](ثابت UChar*) { },
        حالت_2);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
خالی U_CALLCONV
Alias_ڈیٹا::بھرو_ڈیٹا(UError_رمز &حالت_2)
{
#اگر_تعریف_شدہ LOCALE_CANONICALIZATION_DEBUG
    UDate آغاز = uprv_getRawUTCtime();
#اگر_ختم  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    Alias_ڈیٹا_Builder builder;
    gSingleton = builder.build(حالت_2);
#اگر_تعریف_شدہ LOCALE_CANONICALIZATION_DEBUG
    UDate انتہا = uprv_getRawUTCtime();
    لکھو("AliasData::loadData took total %f ms\n", انتہا - آغاز);
#اگر_ختم  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
Alias_ڈیٹا*
Alias_ڈیٹا_Builder::build(UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) { واپس خالی_اشاریہ; }

    Local_UResource_Bundle_اشارہ_گر metadata(
        ures_open_براہ_راست(خالی_اشاریہ, "metadata", &حالت_2));
    Local_UResource_Bundle_اشارہ_گر metadataAlias(
        ures_get_By_کلید(metadata.getAlias(), "alias", خالی_اشاریہ, &حالت_2));
    Local_UResource_Bundle_اشارہ_گر languageAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "language", خالی_اشاریہ, &حالت_2));
    Local_UResource_Bundle_اشارہ_گر scriptAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "script", خالی_اشاریہ, &حالت_2));
    Local_UResource_Bundle_اشارہ_گر territoryAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "territory", خالی_اشاریہ, &حالت_2));
    Local_UResource_Bundle_اشارہ_گر variantAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "variant", خالی_اشاریہ, &حالت_2));
    Local_UResource_Bundle_اشارہ_گر subdivisionAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "subdivision", خالی_اشاریہ, &حالت_2));

    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    int32_t languages_لمبائی = 0, script_لمبائی = 0, territory_لمبائی = 0,
            variant_لمبائی = 0, subdivision_لمبائی = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    Unique_Char_لڑیاں لڑیاں(حالت_2);
    Local_حافظہ<ثابت حرف*> languageTypes;
    Local_حافظہ<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &لڑیاں,
                      languageTypes,
                      languageReplacementIndexes,
                      languages_لمبائی,
                      حالت_2);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    Local_حافظہ<ثابت حرف*> scriptTypes;
    Local_حافظہ<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &لڑیاں,
                    scriptTypes,
                    scriptReplacementIndexes,
                    script_لمبائی,
                    حالت_2);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    Local_حافظہ<ثابت حرف*> territoryTypes;
    Local_حافظہ<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &لڑیاں,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territory_لمبائی, حالت_2);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    Local_حافظہ<ثابت حرف*> variantTypes;
    Local_حافظہ<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &لڑیاں,
                     variantTypes,
                     variantReplacementIndexes,
                     variant_لمبائی, حالت_2);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    Local_حافظہ<ثابت حرف*> subdivisionTypes;
    Local_حافظہ<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &لڑیاں,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivision_لمبائی, حالت_2);

    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    // We can only use strings after freeze it.
    لڑیاں.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, حالت_2);
    برائے (int32_t i = 0; U_SUCCESS(حالت_2) && i < languages_لمبائی; i++) {
        languageMap.put(languageTypes[i],
                        لڑیاں.get(languageReplacementIndexes[i]),
                        حالت_2);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, حالت_2);
    برائے (int32_t i = 0; U_SUCCESS(حالت_2) && i < script_لمبائی; i++) {
        scriptMap.put(scriptTypes[i],
                      لڑیاں.get(scriptReplacementIndexes[i]),
                      حالت_2);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, حالت_2);
    برائے (int32_t i = 0; U_SUCCESS(حالت_2) && i < territory_لمبائی; i++) {
        territoryMap.put(territoryTypes[i],
                         لڑیاں.get(territoryReplacementIndexes[i]),
                         حالت_2);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, حالت_2);
    برائے (int32_t i = 0; U_SUCCESS(حالت_2) && i < variant_لمبائی; i++) {
        variantMap.put(variantTypes[i],
                       لڑیاں.get(variantReplacementIndexes[i]),
                       حالت_2);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, حالت_2);
    برائے (int32_t i = 0; U_SUCCESS(حالت_2) && i < subdivision_لمبائی; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       لڑیاں.get(subdivisionReplacementIndexes[i]),
                       حالت_2);
    }

    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    // copy hashtables
    خودکار *ڈیٹا = نیا Alias_ڈیٹا(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        لڑیاں.orphan_Char_لڑیاں());

    اگر (ڈیٹا == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    }
    واپس ڈیٹا;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
جماعت AliasReplacer {
عوامی:
    AliasReplacer(UError_رمز& حالت_2) :
            language(خالی_اشاریہ), script(خالی_اشاریہ), region(خالی_اشاریہ),
            extensions(خالی_اشاریہ),
            // store value in variants only once
            variants(خالی_اشاریہ,
                     ([](UElement e1, UElement e2) -> UBool {
                       واپس 0==uprv_strcmp((ثابت حرف*)e1.اشارہ_گر,
                                             (ثابت حرف*)e2.اشارہ_گر);}),
                     حالت_2),
            ڈیٹا(خالی_اشاریہ) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    منطقی replace(
        ثابت Locale& locale, CharString& out, UError_رمز& حالت_2);

نجی:
    ثابت حرف* language;
    ثابت حرف* script;
    ثابت حرف* region;
    ثابت حرف* extensions;
    UVector variants;

    ثابت Alias_ڈیٹا* ڈیٹا;

    اندرونی منطقی not_خالی_خانے(ثابت حرف* str) {
        واپس str && str[0] != NULL_CHAR;
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
    اندرونی ثابت حرف* deleteOrReplace(ثابت حرف* input U_LIFETIME_BOUND,
                                       ثابت حرف* type,
                                       ثابت حرف* replacement U_LIFETIME_BOUND) {
        واپس not_خالی_خانے(replacement) ?
            ((input == خالی_اشاریہ) ?  replacement : input) :
            ((type == خالی_اشاریہ) ? input  : خالی_اشاریہ);
    }

    اندرونی منطقی same(ثابت حرف* a, ثابت حرف* b) {
        اگر (a == خالی_اشاریہ && b == خالی_اشاریہ) {
            واپس سچ;
        }
        اگر ((a == خالی_اشاریہ && b != خالی_اشاریہ) ||
            (a != خالی_اشاریہ && b == خالی_اشاریہ)) {
          واپس جھوٹ;
        }
        واپس uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& output_مقصد_String(CharString& out U_LIFETIME_BOUND, UError_رمز& حالت_2);

    // Generate the lookup key.
    CharString& generate_کلید(ثابت حرف* language, ثابت حرف* region,
                            ثابت حرف* variant, CharString& out U_LIFETIME_BOUND,
                            UError_رمز& حالت_2);

    خالی parseLanguageReplacement(ثابت حرف* replacement,
                                  ثابت حرف*& replaceLanguage,
                                  ثابت حرف*& replaceScript,
                                  ثابت حرف*& replaceRegion,
                                  ثابت حرف*& replaceVariant,
                                  ثابت حرف*& replaceExtensions,
                                  UVector& مقصد_Be_Freed,
                                  UError_رمز& حالت_2);

    // Replace by using languageAlias.
    منطقی replaceLanguage(منطقی checkLanguage, منطقی checkRegion,
                         منطقی checkVariants, UVector& مقصد_Be_Freed,
                         UError_رمز& حالت_2);

    // Replace by using territoryAlias.
    منطقی replaceTerritory(UVector& مقصد_Be_Freed, UError_رمز& حالت_2);

    // Replace by using scriptAlias.
    منطقی replaceScript(UError_رمز& حالت_2);

    // Replace by using variantAlias.
    منطقی replaceVariant(UError_رمز& حالت_2);

    // Replace by using subdivisionAlias.
    منطقی replaceSubdivision(StringPiece subdivision,
                            CharString& output, UError_رمز& حالت_2);

    // Replace transformed extensions.
    منطقی replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UError_رمز& حالت_2);
};

CharString&
AliasReplacer::generate_کلید(
        ثابت حرف* language, ثابت حرف* region, ثابت حرف* variant,
        CharString& out U_LIFETIME_BOUND, UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) { واپس out; }
    out.append(language, حالت_2);
    اگر (not_خالی_خانے(region)) {
        out.append(SEP_CHAR, حالت_2)
            .append(region, حالت_2);
    }
    اگر (not_خالی_خانے(variant)) {
       out.append(SEP_CHAR, حالت_2)
           .append(variant, حالت_2);
    }
    واپس out;
}

خالی
AliasReplacer::parseLanguageReplacement(
    ثابت حرف* replacement,
    ثابت حرف*& replacedLanguage,
    ثابت حرف*& replacedScript,
    ثابت حرف*& replacedRegion,
    ثابت حرف*& replacedVariant,
    ثابت حرف*& replacedExtensions,
    UVector& مقصد_Be_Freed,
    UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = خالی_اشاریہ;
    اگر (uprv_strchr(replacement, '_') == خالی_اشاریہ) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        واپس;
    }
    // We have multiple field so we have to allocate and parse
    خودکار str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), حالت_2, حالت_2);
    حرف* ڈیٹا = str->ڈیٹا();
    مقصد_Be_Freed.adoptElement(str.release(), حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    replacedLanguage = (ثابت حرف*) ڈیٹا;
    حرف* انتہا_کا_میدان = uprv_strchr(ڈیٹا, '_');
    *انتہا_کا_میدان = '\0'; // null terminiate it.
    انتہا_کا_میدان++;
    ثابت حرف* آغاز = انتہا_کا_میدان;
    انتہا_کا_میدان = const_cast<حرف*>(uprv_strchr(آغاز, '_'));
    size_t len = 0;
    اگر (انتہا_کا_میدان == خالی_اشاریہ) {
        len = uprv_strlen(آغاز);
    } ورنہ {
        len = انتہا_کا_میدان - آغاز;
        *انتہا_کا_میدان = '\0'; // null terminiate it.
    }
    اگر (len == 4 && uprv_isASCIILetter(*آغاز)) {
        // Got a script
        replacedScript = آغاز;
        اگر (انتہا_کا_میدان == خالی_اشاریہ) {
            واپس;
        }
        آغاز = انتہا_کا_میدان++;
        انتہا_کا_میدان = const_cast<حرف*>(uprv_strchr(آغاز, '_'));
        اگر (انتہا_کا_میدان == خالی_اشاریہ) {
            len = uprv_strlen(آغاز);
        } ورنہ {
            len = انتہا_کا_میدان - آغاز;
            *انتہا_کا_میدان = '\0'; // null terminiate it.
        }
    }
    اگر (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = آغاز;
        اگر (انتہا_کا_میدان == خالی_اشاریہ) {
            واپس;
        }
        آغاز = انتہا_کا_میدان++;
        انتہا_کا_میدان = const_cast<حرف*>(uprv_strchr(آغاز, '_'));
        اگر (انتہا_کا_میدان == خالی_اشاریہ) {
            len = uprv_strlen(آغاز);
        } ورنہ {
            len = انتہا_کا_میدان - آغاز;
            *انتہا_کا_میدان = '\0'; // null terminiate it.
        }
    }
    اگر (len >= 4) {
        // Got a variant
        replacedVariant = آغاز;
        اگر (انتہا_کا_میدان == خالی_اشاریہ) {
            واپس;
        }
        آغاز = انتہا_کا_میدان++;
    }
    replacedExtensions = آغاز;
}

منطقی
AliasReplacer::replaceLanguage(
        منطقی checkLanguage, منطقی checkRegion,
        منطقی checkVariants, UVector& مقصد_Be_Freed, UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    اگر (    (checkRegion && region == خالی_اشاریہ) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        واپس جھوٹ;
    }
    int32_t variant_سائز = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ثابت حرف* تلاش_Language = checkLanguage ? language : "und";
    ثابت حرف* تلاش_Region = checkRegion ? region : خالی_اشاریہ;
    ثابت حرف* تلاش_Variant = خالی_اشاریہ;
    برائے (int32_t variant_اشاریہ = 0;
            variant_اشاریہ < variant_سائز;
            variant_اشاریہ++) {
        اگر (checkVariants) {
            U_ASSERT(variant_اشاریہ < variant_سائز);
            تلاش_Variant = static_cast<ثابت حرف*>(variants.elementAt(variant_اشاریہ));
        }

        اگر (تلاش_Variant != خالی_اشاریہ && uprv_strlen(تلاش_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            تلاش_Variant = خالی_اشاریہ;
        }
        CharString type_کلید;
        generate_کلید(تلاش_Language, تلاش_Region, تلاش_Variant, type_کلید,
                    حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            واپس جھوٹ;
        }
        ثابت حرف *replacement = ڈیٹا->languageMap().get(type_کلید.ڈیٹا());
        اگر (replacement == خالی_اشاریہ) {
            // Found no replacement data.
            جاری;
        }

        ثابت حرف* replacedLanguage = خالی_اشاریہ;
        ثابت حرف* replacedScript = خالی_اشاریہ;
        ثابت حرف* replacedRegion = خالی_اشاریہ;
        ثابت حرف* replacedVariant = خالی_اشاریہ;
        ثابت حرف* replacedExtensions = خالی_اشاریہ;
        parseLanguageReplacement(replacement,
                                 replacedLanguage,
                                 replacedScript,
                                 replacedRegion,
                                 replacedVariant,
                                 replacedExtensions,
                                 مقصد_Be_Freed,
                                 حالت_2);
        replacedLanguage =
            (replacedLanguage != خالی_اشاریہ && uprv_strcmp(replacedLanguage, "und") == 0) ?
            language : replacedLanguage;
        replacedScript = deleteOrReplace(script, خالی_اشاریہ, replacedScript);
        replacedRegion = deleteOrReplace(region, تلاش_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            تلاش_Variant, تلاش_Variant, replacedVariant);

        اگر (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(تلاش_Variant, replacedVariant) &&
                replacedExtensions == خالی_اشاریہ) {
            // Replacement produce no changes.
            جاری;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        اگر (تلاش_Variant != خالی_اشاریہ) {
            اگر (not_خالی_خانے(replacedVariant)) {
                variants.سیٹ_Element_At((خالی*)replacedVariant, variant_اشاریہ);
            } ورنہ {
                variants.removeElementAt(variant_اشاریہ);
            }
        }
        اگر (replacedExtensions != خالی_اشاریہ) {
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
        واپس سچ;
    }
    // Nothing changed by language alias data.
    واپس جھوٹ;
}

منطقی
AliasReplacer::replaceTerritory(UVector& مقصد_Be_Freed, UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    اگر (region == خالی_اشاریہ) {
        // No region to search.
        واپس جھوٹ;
    }
    ثابت حرف *replacement = ڈیٹا->territoryMap().get(region);
    اگر (replacement == خالی_اشاریہ) {
        // Found no replacement data for this region.
        واپس جھوٹ;
    }
    ثابت حرف* replacedRegion = replacement;
    ثابت حرف* پہلا_Space = uprv_strchr(replacement, ' ');
    اگر (پہلا_Space != خالی_اشاریہ) {
        // If there are are more than one region in the replacement.
        // We need to check which one match based on the language.
        // Cannot use nullptr for language because that will construct
        // the default locale, in that case, use "und" to get the correct
        // locale.
        Locale l = LocaleBuilder()
            .سیٹ_Language(language == خالی_اشاریہ ? "und" : language)
            .سیٹ_Script(script)
            .build(حالت_2);
        l.جوڑو_Likely_Subtags(حالت_2);
        ثابت حرف* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> شے;
        اگر (likelyRegion != خالی_اشاریہ && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ثابت حرف* ملا_اندر_Replacement = uprv_strstr(replacement,
                                                         likelyRegion);
            اگر (ملا_اندر_Replacement != خالی_اشاریہ) {
                // Assuming the case there are no three letter region code in
                // the replacement of territoryAlias
                U_ASSERT(ملا_اندر_Replacement == replacement ||
                         *(ملا_اندر_Replacement-1) == ' ');
                U_ASSERT(ملا_اندر_Replacement[len] == ' ' ||
                         ملا_اندر_Replacement[len] == '\0');
                شے = prv::make_unique<CharString>(
                    ملا_اندر_Replacement, static_cast<int32_t>(len), حالت_2, حالت_2);
            }
        }
        اگر (!شے && U_SUCCESS(حالت_2)) {
            شے = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(پہلا_Space - replacement), حالت_2, حالت_2);
        }
        اگر (U_FAILURE(حالت_2)) { واپس جھوٹ; }
        replacedRegion = شے->ڈیٹا();
        مقصد_Be_Freed.adoptElement(شے.release(), حالت_2);
        اگر (U_FAILURE(حالت_2)) { واپس جھوٹ; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    واپس سچ;
}

منطقی
AliasReplacer::replaceScript(UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    اگر (script == خالی_اشاریہ) {
        // No script to search.
        واپس جھوٹ;
    }
    ثابت حرف *replacement = ڈیٹا->scriptMap().get(script);
    اگر (replacement == خالی_اشاریہ) {
        // Found no replacement data for this script.
        واپس جھوٹ;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    واپس سچ;
}

منطقی
AliasReplacer::replaceVariant(UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    // Since we may have more than one variant, we need to loop through them.
    برائے (int32_t i = 0; i < variants.size(); i++) {
        ثابت حرف* variant = static_cast<ثابت حرف*>(variants.elementAt(i));
        ثابت حرف *replacement = ڈیٹا->variantMap().get(variant);
        اگر (replacement == خالی_اشاریہ) {
            // Found no replacement data for this variant.
            جاری;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        اگر (!same(variant, replacement)) {
            variants.سیٹ_Element_At((خالی*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            اگر (uprv_strcmp(variant, "heploc") == 0) {
                برائے (int32_t j = 0; j < variants.size(); j++) {
                     اگر (uprv_strcmp((ثابت حرف*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            واپس سچ;
        }
    }
    واپس جھوٹ;
}

منطقی
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    ثابت حرف *replacement = ڈیٹا->subdivisionMap().get(subdivision.ڈیٹا());
    اگر (replacement != خالی_اشاریہ) {
        ثابت حرف* پہلا_Space = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (پہلا_Space != خالی_اشاریہ) ?
            (پہلا_Space - replacement) : uprv_strlen(replacement);
        اگر (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), حالت_2);
            اگر (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, حالت_2);
            }
        }
        واپس سچ;
    }
    واپس جھوٹ;
}

منطقی
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UError_رمز& حالت_2)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    int32_t len = transformedExtensions.length();
    ثابت حرف* str = transformedExtensions.ڈیٹا();
    ثابت حرف* tkey = ultag_get_TKey_آغاز(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == خالی_اشاریہ) ? len : static_cast<int32_t>((tkey - str - 1)));
    اگر (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .سیٹ_Language_لیبل(StringPiece(str, tlangLen))
            .build(حالت_2);
        tlang.canonicalize(حالت_2);
        output = tlang.مقصد_Language_لیبل<CharString>(حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            واپس جھوٹ;
        }
        T_CString_مقصد_زیریں_Case(output.ڈیٹا());
    }
    اگر (tkey != خالی_اشاریہ) {
        // We need to sort the tfields by tkey
        UVector tfields(حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            واپس جھوٹ;
        }
        کرو {
            ثابت حرف* tvalue = uprv_strchr(tkey, '-');
            اگر (tvalue == خالی_اشاریہ) {
                حالت_2 = U_ILLEGAL_دلیل_ERROR;
                واپس جھوٹ;
            }
            ثابت حرف* اگلا_TKey = ultag_get_TKey_آغاز(tvalue);
            اگر (اگلا_TKey != خالی_اشاریہ) {
                *const_cast<حرف*>(اگلا_TKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.داخل_کرو_Element_At((خالی*)tkey, tfields.size(), حالت_2);
            اگر (U_FAILURE(حالت_2)) {
                واپس جھوٹ;
            }
            tkey = اگلا_TKey;
        } جب (tkey != خالی_اشاریہ);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            واپس uprv_strcmp((ثابت حرف*)e1.اشارہ_گر, (ثابت حرف*)e2.اشارہ_گر);
        }, حالت_2);
        برائے (int32_t i = 0; i < tfields.size(); i++) {
             اگر (output.length() > 0) {
                 output.append('-', حالت_2);
             }
             ثابت حرف* tfield = static_cast<ثابت حرف*>(tfields.elementAt(i));
             ثابت حرف* tvalue = uprv_strchr(tfield, '-');
             اگر (tvalue == خالی_اشاریہ) {
                 حالت_2 = U_ILLEGAL_دلیل_ERROR;
                 واپس جھوٹ;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<حرف*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, حالت_2).append('-', حالت_2);
             std::optional<std::string_view> bcpTValue = ulocimp_مقصد_Bcp_Type(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, حالت_2);
        }
    }
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    واپس سچ;
}

CharString&
AliasReplacer::output_مقصد_String(
    CharString& out U_LIFETIME_BOUND, UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) { واپس out; }
    out.append(language, حالت_2);
    اگر (not_خالی_خانے(script)) {
        out.append(SEP_CHAR, حالت_2)
            .append(script, حالت_2);
    }
    اگر (not_خالی_خانے(region)) {
        out.append(SEP_CHAR, حالت_2)
            .append(region, حالت_2);
    }
    اگر (variants.size() > 0) {
        اگر (!not_خالی_خانے(script) && !not_خالی_خانے(region)) {
          out.append(SEP_CHAR, حالت_2);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            واپس uprv_strcmp((ثابت حرف*)e1.اشارہ_گر, (ثابت حرف*)e2.اشارہ_گر);
        }, حالت_2);
        int32_t variants_آغاز = out.length();
        برائے (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, حالت_2)
                 .append(static_cast<ثابت حرف*>(variants.elementAt(i)),
                         حالت_2);
        }
        T_CString_مقصد_بالائی_Case(out.ڈیٹا() + variants_آغاز);
    }
    اگر (not_خالی_خانے(extensions)) {
        CharString tmp("und_", حالت_2);
        tmp.append(extensions, حالت_2);
        Locale tmpLocale(tmp.ڈیٹا());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, حالت_2);
    }
    واپس out;
}

منطقی
AliasReplacer::replace(ثابت Locale& locale, CharString& out, UError_رمز& حالت_2)
{
    ڈیٹا = Alias_ڈیٹا::singleton(حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    U_ASSERT(ڈیٹا != خالی_اشاریہ);
    out.clear();
    language = locale.getLanguage();
    اگر (!not_خالی_خانے(language)) {
        language = خالی_اشاریہ;
    }
    script = locale.getScript();
    اگر (!not_خالی_خانے(script)) {
        script = خالی_اشاریہ;
    }
    region = locale.getCountry();
    اگر (!not_خالی_خانے(region)) {
        region = خالی_اشاریہ;
    }
    ثابت حرف* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, حالت_2);
    اگر (!variantsBuff.is_خالی_خانے()) {
        اگر (U_FAILURE(حالت_2)) { واپس جھوٹ; }
        حرف* آغاز = variantsBuff.ڈیٹا();
        T_CString_مقصد_زیریں_Case(آغاز);
        حرف* انتہا;
        جب ((انتہا = uprv_strchr(آغاز, SEP_CHAR)) != خالی_اشاریہ &&
               U_SUCCESS(حالت_2)) {
            *انتہا = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            اگر (*آغاز && !variants.شامل_ہے(آغاز)) {
                variants.جوڑو_Element(آغاز, حالت_2);
            }
            آغاز = انتہا + 1;
        }
        // do not add "" or duplicate data to variants
        اگر (*آغاز && !variants.شامل_ہے(آغاز)) {
            variants.جوڑو_Element(آغاز, حالت_2);
        }
    }
    اگر (U_FAILURE(حالت_2)) { واپس جھوٹ; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        واپس uprv_strcmp((ثابت حرف*)e1.اشارہ_گر, (ثابت حرف*)e2.اشارہ_گر);
    }, حالت_2);

    // A changed count to assert when loop too many times.
    int32_t بدلا = 0;
    ثابت int32_t max_بدلا = 4 + variants.size();
    (خالی)max_بدلا; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector لڑیاں_مقصد_Be_Freed([](خالی *obj) { حذف static_cast<CharString*>(obj); },
                             خالی_اشاریہ, 10, حالت_2);
    جب (U_SUCCESS(حالت_2)) {
        // Something wrong with the data cause looping here more than maxChanged
        // times already.
        U_ASSERT(بدلا <= max_بدلا);
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
        اگر (    replaceLanguage(سچ, سچ,  سچ,  لڑیاں_مقصد_Be_Freed, حالت_2) ||
                replaceLanguage(سچ, سچ,  جھوٹ, لڑیاں_مقصد_Be_Freed, حالت_2) ||
                replaceLanguage(سچ, جھوٹ, سچ,  لڑیاں_مقصد_Be_Freed, حالت_2) ||
                replaceLanguage(سچ, جھوٹ, جھوٹ, لڑیاں_مقصد_Be_Freed, حالت_2) ||
                replaceLanguage(جھوٹ,جھوٹ, سچ,  لڑیاں_مقصد_Be_Freed, حالت_2) ||
                replaceTerritory(لڑیاں_مقصد_Be_Freed, حالت_2) ||
                replaceScript(حالت_2) ||
                replaceVariant(حالت_2)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            بدلا++;
            جاری;
        }
        // Nothing changed. Break out.
        توڑو;
    }  // while(1)

    اگر (U_FAILURE(حالت_2)) { واپس جھوٹ; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ثابت حرف* extensionsStr = locale_get_Keywords_آغاز(locale.getName());
    اگر (بدلا == 0 && variants.size() <= 1 && extensionsStr == خالی_اشاریہ) {
        واپس جھوٹ;
    }
    output_مقصد_String(out, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    اگر (extensionsStr != خالی_اشاریہ) {
        بدلا = 0;
        Locale عارضی(locale);
        Local_اشارہ_گر<icu::StringEnumeration> iter(locale.createKeywords(حالت_2));
        اگر (U_SUCCESS(حالت_2) && !iter.isNull()) {
            ثابت حرف* کلید;
            جب ((کلید = iter->next(خالی_اشاریہ, حالت_2)) != خالی_اشاریہ) {
                اگر (uprv_strcmp("sd", کلید) == 0 || uprv_strcmp("rg", کلید) == 0 ||
                        uprv_strcmp("t", کلید) == 0) {
                    خودکار value = locale.get_Keyword_قدر<CharString>(کلید, حالت_2);
                    اگر (U_FAILURE(حالت_2)) {
                        حالت_2 = U_ZERO_ERROR;
                        جاری;
                    }
                    CharString replacement;
                    اگر (uprv_strlen(کلید) == 2) {
                        اگر (replaceSubdivision(value.مقصد_String_Piece(), replacement, حالت_2)) {
                            بدلا++;
                            عارضی.سیٹ_Keyword_قدر(کلید, replacement.ڈیٹا(), حالت_2);
                        }
                    } ورنہ {
                        U_ASSERT(uprv_strcmp(کلید, "t") == 0);
                        اگر (replaceTransformedExtensions(value, replacement, حالت_2)) {
                            بدلا++;
                            عارضی.سیٹ_Keyword_قدر(کلید, replacement.ڈیٹا(), حالت_2);
                        }
                    }
                    اگر (U_FAILURE(حالت_2)) {
                        واپس جھوٹ;
                    }
                }
            }
        }
        اگر (بدلا != 0) {
            extensionsStr = locale_get_Keywords_آغاز(عارضی.getName());
        }
        out.append(extensionsStr, حالت_2);
    }
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    // If the tag is not changed, return.
    اگر (uprv_strcmp(out.ڈیٹا(), locale.getName()) == 0) {
        out.clear();
        واپس جھوٹ;
    }
    واپس سچ;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
منطقی
canonicalizeLocale(ثابت Locale& locale, CharString& out, UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) { واپس جھوٹ; }
    AliasReplacer replacer(حالت_2);
    واپس replacer.replace(locale, out, حالت_2);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
منطقی
is_معلوم_Canonicalized_Locale(ثابت حرف* locale, UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) { واپس جھوٹ; }

    اگر (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        واپس سچ;
    }

    // common well-known Canonicalized.
    umtx_initOnce(g_معلوم_Canonicalized_Init_Once,
                  &بھرو_معلوم_Canonicalized, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    U_ASSERT(g_معلوم_Canonicalized != خالی_اشاریہ);
    واپس uhash_geti(g_معلوم_Canonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_انتہا

// Function for testing.
U_EXPORT ثابت حرف* ثابت*
ulocimp_get_معلوم_Canonicalized_Locale_For_Test(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(معلوم_CANONICALIZED);
    واپس معلوم_CANONICALIZED;
}

// Function for testing.
U_EXPORT منطقی
ulocimp_isCanonicalizedLocaleForTest(ثابت حرف* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UError_رمز حالت_2 = U_ZERO_ERROR;
    CharString عارضی;
    واپس !canonicalizeLocale(l, عارضی, حالت_2) && U_SUCCESS(حالت_2);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ثابت حرف* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    واپس localeID == خالی_اشاریہ ? *یہ = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    کرو {
        حرف *separator;
        حرف *میدان_2[5] = {خالی_اشاریہ};
        int32_t میدان_Len[5] = {0};
        int32_t میدان_Idx;
        int32_t variant_میدان;
        int32_t length;
        UError_رمز err;

        ثابت خودکار parse = [canonicalize](std::string_view localeID,
                                          حرف* name,
                                          int32_t name_استعداد,
                                          UError_رمز& حالت_2) {
            واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
                name, name_استعداد,
                [&](ByteSink& sink, UError_رمز& حالت_2) {
                    اگر (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, حالت_2);
                    } ورنہ {
                        ulocimp_getName(localeID, sink, حالت_2);
                    }
                },
                حالت_2);
        };

        // "canonicalize" the locale ID to ICU/Java format
        حرف* بھرا_Name = nest.بنیاد_Name;
        err = U_ZERO_ERROR;
        length = parse(localeID, بھرا_Name, حجم Nest::بنیاد_Name, err);

        FixedString بھرا_Name_بفر;
        اگر (err == U_بفر_حد_سے_تجاوز_ERROR || length >= static_cast<int32_t>(حجم Nest::بنیاد_Name)) {
            /*Go to heap for the fullName if necessary*/
            اگر (!بھرا_Name_بفر.reserve(length + 1)) {
                توڑو; // error: out of memory
            }
            بھرا_Name = بھرا_Name_بفر.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, بھرا_Name, length + 1, err);
        }
        اگر(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            توڑو;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = میدان_2[0] = بھرا_Name;
        میدان_Idx = 1;
        حرف* at = uprv_strchr(بھرا_Name, '@');
        جب ((separator = uprv_strchr(میدان_2[میدان_Idx-1], SEP_CHAR)) != خالی_اشاریہ &&
               میدان_Idx < UPRV_LENGTHOF(میدان_2)-1 &&
               (at == خالی_اشاریہ || separator < at)) {
            میدان_2[میدان_Idx] = separator + 1;
            میدان_Len[میدان_Idx - 1] = static_cast<int32_t>(separator - میدان_2[میدان_Idx - 1]);
            میدان_Idx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(میدان_2[میدان_Idx-1], '@');
        حرف* sep2 = uprv_strchr(میدان_2[میدان_Idx-1], '.');
        اگر (separator!=خالی_اشاریہ || sep2!=خالی_اشاریہ) {
            اگر (separator==خالی_اشاریہ || (sep2!=خالی_اشاریہ && separator > sep2)) {
                separator = sep2;
            }
            میدان_Len[میدان_Idx - 1] = static_cast<int32_t>(separator - میدان_2[میدان_Idx - 1]);
        } ورنہ {
            میدان_Len[میدان_Idx - 1] = length - static_cast<int32_t>(میدان_2[میدان_Idx - 1] - بھرا_Name);
        }
        منطقی hasKeywords = at != خالی_اشاریہ && uprv_strchr(at + 1, '=') != خالی_اشاریہ;

        اگر (میدان_Len[0] >= ULOC_LANG_استعداد)
        {
            توڑو; // error: the language field is too long
        }

        variant_میدان = 1; /* Usually the 2nd one, except when a script or country is also used. */
        اگر (میدان_Len[0] > 0) {
            /* We have a language */
            language = {بھرا_Name, static_cast<std::string_view::size_type>(میدان_Len[0])};
        }
        اگر (میدان_Len[1] == 4 && uprv_isASCIILetter(میدان_2[1][0]) &&
                uprv_isASCIILetter(میدان_2[1][1]) && uprv_isASCIILetter(میدان_2[1][2]) &&
                uprv_isASCIILetter(میدان_2[1][3])) {
            /* We have at least a script */
            script = {میدان_2[1], static_cast<std::string_view::size_type>(میدان_Len[1])};
            variant_میدان++;
        }

        اگر (میدان_Len[variant_میدان] == 2 || میدان_Len[variant_میدان] == 3) {
            /* We have a country */
            region = {میدان_2[variant_میدان], static_cast<std::string_view::size_type>(میدان_Len[variant_میدان])};
            variant_میدان++;
        } ورنہ اگر (میدان_Len[variant_میدان] == 0) {
            variant_میدان++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        اگر (میدان_Len[variant_میدان] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(میدان_2[variant_میدان] - بھرا_Name);
        } ورنہ اگر (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - بھرا_Name);
        }

        اگر (!hasKeywords && Nest::سماتا(length, language, script, region)) {
            U_ASSERT(بھرا_Name == nest.بنیاد_Name);
            U_ASSERT(بھرا_Name_بفر.is_خالی_خانے());
            nest.init(language, script, region, variantBegin);
        } ورنہ {
            اگر (بھرا_Name == nest.بنیاد_Name) {
                U_ASSERT(بھرا_Name_بفر.is_خالی_خانے());
                بھرا_Name_بفر = {بھرا_Name, static_cast<std::string_view::size_type>(length)};
                اگر (بھرا_Name_بفر.is_خالی_خانے()) {
                    توڑو; // error: out of memory
                }
                اگر (!language.empty()) {
                    language = {بھرا_Name_بفر.ڈیٹا(), language.size()};
                }
                اگر (!script.empty()) {
                    script = {بھرا_Name_بفر.ڈیٹا() + (script.ڈیٹا() - بھرا_Name), script.size()};
                }
                اگر (!region.empty()) {
                    region = {بھرا_Name_بفر.ڈیٹا() + (region.ڈیٹا() - بھرا_Name), region.size()};
                }
            }
            ڈھیر_2& ڈھیر_3 = payload.emplace<ڈھیر_2>(language, script, region, variantBegin);
            اگر (isBogus()) {
                توڑو; // error: out of memory
            }
            U_ASSERT(!بھرا_Name_بفر.is_خالی_خانے());
            ڈھیر_3.ptr->بھرا_Name = std::move(بھرا_Name_بفر);
            اگر (hasKeywords) {
                اگر (std::string_view::size_type بنیاد_Name_لمبائی = at - بھرا_Name; بنیاد_Name_لمبائی > 0) {
                    ڈھیر_3.ptr->بنیاد_Name = {ڈھیر_3.ptr->بھرا_Name.ڈیٹا(), بنیاد_Name_لمبائی};
                    اگر (ڈھیر_3.ptr->بنیاد_Name.is_خالی_خانے()) {
                        توڑو; // error: out of memory
                    }
                }
            }
        }

        اگر (canonicalize) {
            اگر (!is_معلوم_Canonicalized_Locale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                اگر (canonicalizeLocale(*یہ, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.ڈیٹا(), جھوٹ);
                }
                اگر (U_FAILURE(err)) {
                    توڑو;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        واپس *یہ;
    } جب(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    سیٹ_مقصد_Bogus();

    واپس *یہ;
}

int32_t
Locale::hash_رمز() ثابت
{
    واپس ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

خالی
Locale::سیٹ_مقصد_Bogus() {
    /* Free our current storage */
    payload.سیٹ_مقصد_Bogus();
}

ثابت Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex تالا(&gDefaultLocaleMutex);
        اگر (gDefaultLocale != خالی_اشاریہ) {
            واپس *gDefaultLocale;
        }
    }
    UError_رمز حالت_2 = U_ZERO_ERROR;
    واپس *locale_سیٹ_default_اندرونی(خالی_اشاریہ, حالت_2);
}



خالی U_EXPORT2
Locale::سیٹ_Default( ثابت   Locale&     newLocale,
                            UError_رمز&  حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ثابت حرف *localeID = newLocale.getName();
    locale_سیٹ_default_اندرونی(localeID, حالت_2);
}

خالی
Locale::جوڑو_Likely_Subtags(UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    CharString maximizedLocaleID = ulocimp_جوڑو_Likely_Subtags(getName(), حالت_2);

    اگر (U_FAILURE(حالت_2)) {
        اگر (حالت_2 == U_حافظہ_تخصیص_ERROR) {
            سیٹ_مقصد_Bogus();
        }
        واپس;
    }

    init(maximizedLocaleID.ڈیٹا(), /*canonicalize=*/جھوٹ);
    اگر (isBogus()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
    }
}

خالی
Locale::minimizeSubtags(UError_رمز& حالت_2) {
    Locale::minimizeSubtags(جھوٹ, حالت_2);
}
خالی
Locale::minimizeSubtags(منطقی favorScript, UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, حالت_2);

    اگر (U_FAILURE(حالت_2)) {
        اگر (حالت_2 == U_حافظہ_تخصیص_ERROR) {
            سیٹ_مقصد_Bogus();
        }
        واپس;
    }

    init(minimizedLocaleID.ڈیٹا(), /*canonicalize=*/جھوٹ);
    اگر (isBogus()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
    }
}

خالی
Locale::canonicalize(UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    اگر (isBogus()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    CharString uncanonicalized(getName(), حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        اگر (حالت_2 == U_حافظہ_تخصیص_ERROR) {
            سیٹ_مقصد_Bogus();
        }
        واپس;
    }
    init(uncanonicalized.ڈیٹا(), /*canonicalize=*/سچ);
    اگر (isBogus()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
    }
}

Locale U_EXPORT2
Locale::for_Language_لیبل(StringPiece لیبل, UError_رمز& حالت_2)
{
    Locale نتیجہ(Locale::eBOGUS);

    اگر (U_FAILURE(حالت_2)) {
        واپس نتیجہ;
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

    int32_t parsed_لمبائی;
    CharString localeID = ulocimp_for_Language_لیبل(
            لیبل.ڈیٹا(),
            لیبل.length(),
            &parsed_لمبائی,
            حالت_2);

    اگر (U_FAILURE(حالت_2)) {
        واپس نتیجہ;
    }

    اگر (parsed_لمبائی != لیبل.size()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس نتیجہ;
    }

    نتیجہ.init(localeID.ڈیٹا(), /*canonicalize=*/جھوٹ);
    اگر (نتیجہ.isBogus()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
    }
    واپس نتیجہ;
}

خالی
Locale::مقصد_Language_لیبل(ByteSink& sink, UError_رمز& حالت_2) ثابت
{
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    اگر (isBogus()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    ulocimp_مقصد_Language_لیبل(getName(), sink, /*strict=*/جھوٹ, حالت_2);
}

Locale U_EXPORT2
Locale::createFromName (ثابت حرف *name)
{
    اگر (name) {
        Locale l("");
        l.init(name, جھوٹ);
        واپس l;
    }
    ورنہ {
        واپس getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, جھوٹ);
    واپس loc;
}

Locale U_EXPORT2
Locale::createCanonical(ثابت حرف* name) {
    Locale loc("");
    loc.init(name, سچ);
    واپس loc;
}

ثابت حرف *
Locale::getISO3Language() ثابت
{
    واپس uloc_getISO3Language(getName());
}


ثابت حرف *
Locale::getISO3Country() ثابت
{
    واپس uloc_getISO3Country(getName());
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
    واپس uloc_getLCID(getName());
}

ثابت حرف* ثابت* U_EXPORT2 Locale::getISOCountries()
{
    واپس uloc_getISOCountries();
}

ثابت حرف* ثابت* U_EXPORT2 Locale::getISOLanguages()
{
    واپس uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
خالی Locale::سیٹ_From_POSIXID(ثابت حرف *posixID)
{
    init(posixID, سچ);
}

ثابت Locale & U_EXPORT2
Locale::get_جڑ()
{
    واپس getLocale(e_جڑ);
}

ثابت Locale & U_EXPORT2
Locale::getEnglish()
{
    واپس getLocale(eENGLISH);
}

ثابت Locale & U_EXPORT2
Locale::getFrench()
{
    واپس getLocale(eFRENCH);
}

ثابت Locale & U_EXPORT2
Locale::getGerman()
{
    واپس getLocale(eGERMAN);
}

ثابت Locale & U_EXPORT2
Locale::getItalian()
{
    واپس getLocale(eITALIAN);
}

ثابت Locale & U_EXPORT2
Locale::getJapanese()
{
    واپس getLocale(eJAPANESE);
}

ثابت Locale & U_EXPORT2
Locale::getKorean()
{
    واپس getLocale(eKOREAN);
}

ثابت Locale & U_EXPORT2
Locale::getChinese()
{
    واپس getLocale(eCHINESE);
}

ثابت Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    واپس getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    واپس getLocale(eTAIWAN);
}


ثابت Locale & U_EXPORT2
Locale::getFrance()
{
    واپس getLocale(eFRANCE);
}

ثابت Locale & U_EXPORT2
Locale::getGermany()
{
    واپس getLocale(eGERMANY);
}

ثابت Locale & U_EXPORT2
Locale::getItaly()
{
    واپس getLocale(eITALY);
}

ثابت Locale & U_EXPORT2
Locale::getJapan()
{
    واپس getLocale(eJAPAN);
}

ثابت Locale & U_EXPORT2
Locale::getKorea()
{
    واپس getLocale(eKOREA);
}

ثابت Locale & U_EXPORT2
Locale::getChina()
{
    واپس getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getPRC()
{
    واپس getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getTaiwan()
{
    واپس getLocale(eTAIWAN);
}

ثابت Locale & U_EXPORT2
Locale::getUK()
{
    واپس getLocale(eUK);
}

ثابت Locale & U_EXPORT2
Locale::getUS()
{
    واپس getLocale(eUS);
}

ثابت Locale & U_EXPORT2
Locale::getCanada()
{
    واپس getLocale(eCANADA);
}

ثابت Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    واپس getLocale(eCANADA_FRENCH);
}

ثابت Locale &
Locale::getLocale(صحیح locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    اگر (localeCache == خالی_اشاریہ) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    واپس localeCache[locid]; /*operating on nullptr*/
}

/*
This function is defined this way in order to get around static
initialization and static destruction.
 */
Locale *
Locale::getLocaleCache()
{
    UError_رمز حالت_2 = U_ZERO_ERROR;
    umtx_initOnce(gLocaleCacheInitOnce, locale_init, حالت_2);
    واپس gLocaleCache;
}

جماعت KeywordEnumeration : عوامی StringEnumeration {
محفوظ:
    FixedString keywords;
نجی:
    int32_t length;
    ثابت حرف *current;
    ساکن ثابت حرف fgClassID;

عوامی:
    ساکن UClassID U_EXPORT2 getStaticClassID() { واپس (UClassID)&fgClassID; }
    مجازی UClassID getDynamicClassID() ثابت override { واپس getStaticClassID(); }
عوامی:
    KeywordEnumeration(ثابت حرف *کلیدیں, int32_t keywordLen, int32_t current_اشاریہ, UError_رمز &حالت_2)
        : keywords(), length(keywordLen), current(خالی_اشاریہ) {
        اگر(U_SUCCESS(حالت_2) && keywordLen != 0) {
            اگر(کلیدیں == خالی_اشاریہ || keywordLen < 0) {
                حالت_2 = U_ILLEGAL_دلیل_ERROR;
            } ورنہ {
                keywords = {کلیدیں, static_cast<std::string_view::size_type>(length)};
                اگر (keywords.is_خالی_خانے()) {
                    حالت_2 = U_حافظہ_تخصیص_ERROR;
                } ورنہ {
                    current = keywords.ڈیٹا() + current_اشاریہ;
                }
            }
        }
    }

    مجازی ~KeywordEnumeration();

    مجازی StringEnumeration * clone() ثابت override
    {
        UError_رمز حالت_2 = U_ZERO_ERROR;
        واپس نیا KeywordEnumeration(
                keywords.ڈیٹا(), length,
                static_cast<int32_t>(current - keywords.ڈیٹا()), حالت_2);
    }

    مجازی int32_t count(UError_رمز& حالت_2) ثابت override {
        اگر (U_FAILURE(حالت_2)) { واپس 0; }
        ثابت حرف *kw = keywords.ڈیٹا();
        int32_t نتیجہ = 0;
        جب(*kw) {
            نتیجہ++;
            kw += uprv_strlen(kw)+1;
        }
        واپس نتیجہ;
    }

    مجازی ثابت حرف* next(int32_t* نتیجہ_لمبائی, UError_رمز& حالت_2) U_LIFETIME_BOUND override {
        ثابت حرف* نتیجہ;
        int32_t len;
        اگر(U_SUCCESS(حالت_2) && *current != 0) {
            نتیجہ = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            اگر(نتیجہ_لمبائی != خالی_اشاریہ) {
                *نتیجہ_لمبائی = len;
            }
        } ورنہ {
            اگر(نتیجہ_لمبائی != خالی_اشاریہ) {
                *نتیجہ_لمبائی = 0;
            }
            نتیجہ = خالی_اشاریہ;
        }
        واپس نتیجہ;
    }

    مجازی ثابت UnicodeString* snext(UError_رمز& حالت_2) U_LIFETIME_BOUND override {
        اگر (U_FAILURE(حالت_2)) { واپس خالی_اشاریہ; }
        int32_t نتیجہ_لمبائی = 0;
        ثابت حرف *s = next(&نتیجہ_لمبائی, حالت_2);
        واپس سیٹ_Chars(s, نتیجہ_لمبائی, حالت_2);
    }

    مجازی خالی reset(UError_رمز& حالت_2) override {
        اگر (U_FAILURE(حالت_2)) { واپس; }
        current = keywords.ڈیٹا();
    }
};

ثابت حرف KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = طےشدہ;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
جماعت UnicodeKeywordEnumeration : عوامی KeywordEnumeration {
عوامی:
    استعمال KeywordEnumeration::KeywordEnumeration;
    مجازی ~UnicodeKeywordEnumeration();

    مجازی ثابت حرف* next(int32_t* نتیجہ_لمبائی, UError_رمز& حالت_2) U_LIFETIME_BOUND override {
        ثابت حرف* legacy_کلید = KeywordEnumeration::next(خالی_اشاریہ, حالت_2);
        جب (U_SUCCESS(حالت_2) && legacy_کلید != خالی_اشاریہ) {
            ثابت حرف* کلید = uloc_مقصد_Unicode_Locale_کلید(legacy_کلید);
            اگر (کلید != خالی_اشاریہ) {
                اگر (نتیجہ_لمبائی != خالی_اشاریہ) {
                    *نتیجہ_لمبائی = static_cast<int32_t>(uprv_strlen(کلید));
                }
                واپس کلید;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_کلید = KeywordEnumeration::next(خالی_اشاریہ, حالت_2);
        }
        اگر (نتیجہ_لمبائی != خالی_اشاریہ) *نتیجہ_لمبائی = 0;
        واپس خالی_اشاریہ;
    }
    مجازی int32_t count(UError_رمز& حالت_2) ثابت override {
        اگر (U_FAILURE(حالت_2)) { واپس 0; }
        ثابت حرف *kw = keywords.ڈیٹا();
        int32_t نتیجہ = 0;
        جب(*kw) {
            اگر (uloc_مقصد_Unicode_Locale_کلید(kw) != خالی_اشاریہ) {
                نتیجہ++;
            }
            kw += uprv_strlen(kw)+1;
        }
        واپس نتیجہ;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = طےشدہ;

StringEnumeration *
Locale::createKeywords(UError_رمز &حالت_2) ثابت
{
    StringEnumeration *نتیجہ = خالی_اشاریہ;

    اگر (U_FAILURE(حالت_2)) {
        واپس نتیجہ;
    }

    ثابت حرف* variant_آغاز = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    اگر(variant_آغاز) {
        اگر(assignment > variant_آغاز) {
            CharString keywords = ulocimp_getKeywords(variant_آغاز + 1, '@', جھوٹ, حالت_2);
            اگر (U_SUCCESS(حالت_2) && !keywords.is_خالی_خانے()) {
                نتیجہ = نیا KeywordEnumeration(keywords.ڈیٹا(), keywords.length(), 0, حالت_2);
                اگر (!نتیجہ) {
                    حالت_2 = U_حافظہ_تخصیص_ERROR;
                }
            }
        } ورنہ {
            حالت_2 = U_INVALID_FORMAT_ERROR;
        }
    }
    واپس نتیجہ;
}

StringEnumeration *
Locale::createUnicodeKeywords(UError_رمز &حالت_2) ثابت
{
    StringEnumeration *نتیجہ = خالی_اشاریہ;

    اگر (U_FAILURE(حالت_2)) {
        واپس نتیجہ;
    }

    ثابت حرف* variant_آغاز = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    اگر(variant_آغاز) {
        اگر(assignment > variant_آغاز) {
            CharString keywords = ulocimp_getKeywords(variant_آغاز + 1, '@', جھوٹ, حالت_2);
            اگر (U_SUCCESS(حالت_2) && !keywords.is_خالی_خانے()) {
                نتیجہ = نیا UnicodeKeywordEnumeration(keywords.ڈیٹا(), keywords.length(), 0, حالت_2);
                اگر (!نتیجہ) {
                    حالت_2 = U_حافظہ_تخصیص_ERROR;
                }
            }
        } ورنہ {
            حالت_2 = U_INVALID_FORMAT_ERROR;
        }
    }
    واپس نتیجہ;
}

int32_t
Locale::get_Keyword_قدر(ثابت حرف* keywordName, حرف *بفر, int32_t bufLen, UError_رمز &حالت_2) ثابت
{
    واپس uloc_get_Keyword_قدر(getName(), keywordName, بفر, bufLen, &حالت_2);
}

خالی
Locale::get_Keyword_قدر(StringPiece keywordName, ByteSink& sink, UError_رمز& حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    اگر (isBogus()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    ulocimp_get_Keyword_قدر(getName(), keywordName, sink, حالت_2);
}

خالی
Locale::get_Unicode_Keyword_قدر(StringPiece keywordName,
                               ByteSink& sink,
                               UError_رمز& حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    std::optional<std::string_view> legacy_کلید = ulocimp_مقصد_Legacy_کلید_With_Fallback(keywordName);
    اگر (!legacy_کلید.has_value()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    خودکار legacy_قدر = get_Keyword_قدر<CharString>(*legacy_کلید, حالت_2);

    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    std::optional<std::string_view> unicode_قدر =
        ulocimp_مقصد_Bcp_Type_With_Fallback(keywordName, legacy_قدر.مقصد_String_Piece());
    اگر (!unicode_قدر.has_value()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    sink.Append(unicode_قدر->ڈیٹا(), static_cast<int32_t>(unicode_قدر->size()));
}

خالی
Locale::سیٹ_Keyword_قدر(StringPiece keywordName,
                        StringPiece keyword_قدر,
                        UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) { واپس; }
    اگر (keywordName.empty()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    اگر (حالت_2 == U_STRING_NOT_TERMINATED_WARNING) {
        حالت_2 = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, حالت_2);
    ulocimp_سیٹ_Keyword_قدر(keywordName, keyword_قدر, localeID, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        اگر (حالت_2 == U_حافظہ_تخصیص_ERROR) {
            سیٹ_مقصد_Bogus();
        }
        واپس;
    }

    ثابت حرف* at = locale_get_Keywords_آغاز(localeID.مقصد_String_Piece());
    منطقی hasKeywords = at != خالی_اشاریہ && uprv_strchr(at + 1, '=') != خالی_اشاریہ;

    Nest* nest = payload.get<Nest>();
    اگر (!hasKeywords) {
        اگر (nest == خالی_اشاریہ) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            ڈھیر_2* ڈھیر_3 = payload.get<ڈھیر_2>();
            U_ASSERT(ڈھیر_3 != خالی_اشاریہ);
            اگر (Nest::سماتا(localeID.length(), ڈھیر_3->language, ڈھیر_3->script, ڈھیر_3->region)) {
                int32_t variantBegin = ڈھیر_3->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < حجم Nest::بنیاد_Name);
                nest = &payload.emplace<Nest>(std::move(*ڈھیر_3), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->بنیاد_Name, حجم Nest::بنیاد_Name, حالت_2);
            } ورنہ {
                ڈھیر_3->ptr->بنیاد_Name.clear();
                ڈھیر_3->ptr->بھرا_Name = localeID.مقصد_String_Piece();
                اگر (ڈھیر_3->ptr->بھرا_Name.is_خالی_خانے()) {
                    حالت_2 = U_حافظہ_تخصیص_ERROR;
                    سیٹ_مقصد_Bogus();
                    واپس;
                }
            }
        }
    } ورنہ {
        ڈھیر_2* ڈھیر_3 = خالی_اشاریہ;
        اگر (nest != خالی_اشاریہ) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            ڈھیر_3 = &payload.emplace<ڈھیر_2>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            اگر (isBogus()) {
                حالت_2 = U_حافظہ_تخصیص_ERROR;
                واپس;
            }
        } ورنہ {
            ڈھیر_3 = payload.get<ڈھیر_2>();
        }
        U_ASSERT(ڈھیر_3 != خالی_اشاریہ);
        ڈھیر_3->ptr->بھرا_Name = localeID.مقصد_String_Piece();
        اگر (ڈھیر_3->ptr->بھرا_Name.is_خالی_خانے()) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            سیٹ_مقصد_Bogus();
            واپس;
        }

        اگر (ڈھیر_3->ptr->بنیاد_Name.is_خالی_خانے()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            اگر (std::string_view::size_type بنیاد_Name_لمبائی = at - localeID.ڈیٹا(); بنیاد_Name_لمبائی > 0) {
                ڈھیر_3->ptr->بنیاد_Name = {ڈھیر_3->ptr->بھرا_Name.ڈیٹا(), بنیاد_Name_لمبائی};
                اگر (ڈھیر_3->ptr->بنیاد_Name.is_خالی_خانے()) {
                    حالت_2 = U_حافظہ_تخصیص_ERROR;
                    سیٹ_مقصد_Bogus();
                    واپس;
                }
            }
        }
    }
}

خالی
Locale::سیٹ_Unicode_Keyword_قدر(StringPiece keywordName,
                               StringPiece keyword_قدر,
                               UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    std::optional<std::string_view> legacy_کلید = ulocimp_مقصد_Legacy_کلید_With_Fallback(keywordName);
    اگر (!legacy_کلید.has_value()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    std::string_view value;

    اگر (!keyword_قدر.empty()) {
        std::optional<std::string_view> legacy_قدر =
            ulocimp_مقصد_Legacy_Type_With_Fallback(keywordName, keyword_قدر);
        اگر (!legacy_قدر.has_value()) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR;
            واپس;
        }
        value = *legacy_قدر;
    }

    سیٹ_Keyword_قدر(*legacy_کلید, value, حالت_2);
}

ثابت حرف*
Locale::getCountry() ثابت U_LIFETIME_BOUND {
    واپس get_میدان<&Nest::getRegion, &ڈھیر_2::getRegion>();
}

ثابت حرف*
Locale::getLanguage() ثابت U_LIFETIME_BOUND {
    واپس get_میدان<&Nest::getLanguage, &ڈھیر_2::getLanguage>();
}

ثابت حرف*
Locale::getScript() ثابت U_LIFETIME_BOUND {
    واپس get_میدان<&Nest::getScript, &ڈھیر_2::getScript>();
}

ثابت حرف*
Locale::getVariant() ثابت U_LIFETIME_BOUND {
    واپس get_میدان<&Nest::getVariant, &ڈھیر_2::getVariant>();
}

ثابت حرف*
Locale::getName() ثابت U_LIFETIME_BOUND {
    واپس get_میدان<&Nest::get_بنیاد_Name, &ڈھیر_2::get_بھرا_Name>();
}

ثابت حرف*
Locale::get_بنیاد_Name() ثابت U_LIFETIME_BOUND {
    واپس get_میدان<&Nest::get_بنیاد_Name, &ڈھیر_2::get_بنیاد_Name>();
}

سانچہ <ثابت حرف* (Locale::Nest::*ثابت NEST)() ثابت,
          ثابت حرف* (Locale::ڈھیر_2::*ثابت ڈھیر_4)() ثابت>
ثابت حرف* Locale::get_میدان() ثابت U_LIFETIME_BOUND {
    واپس payload.دورہ([] { واپس ""; },
                         [](ثابت Nest& nest) { واپس (nest.*NEST)(); },
                         [](ثابت ڈھیر_2& ڈھیر_3) { واپس (ڈھیر_3.*ڈھیر_4)(); });
}

Locale::Iterator::~Iterator() = طےشدہ;

//eof
U_NAMESPACE_انتہا
