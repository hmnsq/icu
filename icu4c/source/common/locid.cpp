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

#ܐܢ UPRV_HAS_FEATURE(memory_sanitizer)
#ضمن <sanitizer/msan_interface.h>
#endif

U_NAMESPACE_BEGIN

ساكن Locale   *gLocaleCache = nullptr;
ساكن UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
ساكن UMutex gDefaultLocaleMutex;
ساكن UHashtable *gDefaultLocalesHashT = nullptr;
ساكن Locale *gDefaultLocale = nullptr;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#define ULOC_STRING_LIMIT 357913941

U_NAMESPACE_END

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
    eROOT,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

نطاق {

//
// Deleter function for Locales owned by the default Locale hash table/
//
فراغ U_CALLCONV
deleteLocale(فراغ *obj) {
    delete static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    delete [] gLocaleCache;
    gLocaleCache = nullptr;
    gLocaleCacheInitOnce.reset();

    ܐܢ (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    ܦܢܐ ܫܪܪܐ;
}

فراغ U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = جديد Locale[static_cast<صحيح>(eMAX_LOCALES)];
    ܐܢ (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[eROOT]          = Locale("");
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

Locale *locale_set_default_internal(ثابت حرف *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = ܕܓܠܐ;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    ܐܢ (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = ܫܪܪܐ; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ gDefaultLocale;
    }

    ܐܢ (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        ܐܢ (U_FAILURE(status)) {
            ܦܢܐ gDefaultLocale;
        }
        uhash_setValueDeleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    ܐܢ (newDefault == nullptr) {
        newDefault = جديد Locale(Locale::eBOGUS);
        ܐܢ (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ܦܢܐ gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), ܕܓܠܐ);
        uhash_put(gDefaultLocalesHashT, const_cast<حرف*>(newDefault->getName()), newDefault, &status);
        ܐܢ (U_FAILURE(status)) {
            ܦܢܐ gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    ܦܢܐ gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC فراغ
locale_set_default(ثابت حرف *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC ثابت حرف *
locale_get_default()
{
    U_NAMESPACE_USE
    ܦܢܐ Locale::getDefault().getName();
}

نطاق {

template <auto FIELD, typename T>
فراغ copyToArray(std::string_view sv, T* that) {
    auto& field = that->*FIELD;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    ܐܢ (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.data(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

فراغ Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, هذا);
    copyToArray<&Nest::script>(script, هذا);
    copyToArray<&Nest::region>(region, هذا);
    هذا->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(Heap, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(Heap, region));
    U_ASSERT(هذا == reinterpret_cast<Nest*>(&heap));
#ܐܢ UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, حجم(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, هذا);
    copyToArray<&Nest::region>(heap.region, هذا);
    هذا->variantBegin = variantBegin;
    *هذا->baseName = '\0';
}

بنية Locale::Heap::Alloc : عام UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    ثابت حرف* getVariant() ثابت U_LIFETIME_BOUND {
        ܦܢܐ variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    ثابت حرف* getFullName() ثابت U_LIFETIME_BOUND { ܦܢܐ fullName.data(); }
    ثابت حرف* getBaseName() ثابت U_LIFETIME_BOUND {
        ܐܢ (baseName.isEmpty()) {
            ܐܢ (ثابت حرف* name = fullName.data(); *name != '@') {
                ܦܢܐ name;
            }
        }
        ܦܢܐ baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(ثابت Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        ܐܢ (U_SUCCESS(status)) {
            ܐܢ (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                ܐܢ (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ܐܚܪܢܐ {
                    ܐܢ (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        ܐܢ (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = delete;

    ~Alloc() = افتراضي;
};

ثابت حرف* Locale::Heap::getVariant() ثابت U_LIFETIME_BOUND { ܦܢܐ ptr->getVariant(); }
ثابت حرف* Locale::Heap::getFullName() ثابت U_LIFETIME_BOUND { ܦܢܐ ptr->getFullName(); }
ثابت حرف* Locale::Heap::getBaseName() ثابت U_LIFETIME_BOUND { ܦܢܐ ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = جديد Alloc(variantBegin);
    ܐܢ (ptr == nullptr) {
        type = eBOGUS;
    } ܐܚܪܢܐ {
        type = eHEAP;
        copyToArray<&Heap::language>(language, هذا);
        copyToArray<&Heap::script>(script, هذا);
        copyToArray<&Heap::region>(region, هذا);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::operator=(ثابت Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = جديد Alloc(*other.ptr, status);
    ܐܢ (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } ܐܚܪܢܐ {
        type = eHEAP;
        uprv_memcpy(language, other.language, حجم language);
        uprv_memcpy(script, other.script, حجم script);
        uprv_memcpy(region, other.region, حجم region);
    }
    ܦܢܐ *هذا;
}

Locale::Heap& Locale::Heap::operator=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, حجم language);
    uprv_memcpy(script, other.script, حجم script);
    uprv_memcpy(region, other.region, حجم region);
    ܦܢܐ *هذا;
}

template <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
auto Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) ثابت {
    بدل (type) {
        حالة eBOGUS:
            ܦܢܐ bogusFn(args...);
        حالة eNEST:
            ܦܢܐ nestFn(nest, args...);
        حالة eHEAP:
            ܦܢܐ heapFn(heap, args...);
        افتراضي:
            UPRV_UNREACHABLE_EXIT;
    };
}

فراغ Locale::Payload::copy(ثابت Payload& other) {
    other.visit([](Payload*) {},
                [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ثابت Heap& heap, Payload* dst) { dst->heap = heap; },
                هذا);
}

فراغ Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ثابت Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        هذا);
}

Locale::Payload::~Payload() {
    ܐܢ (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(ثابت Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::operator=(ثابت Payload& other) U_LIFETIME_BOUND {
    ܐܢ (هذا != &other) {
        setToBogus();
        copy(other);
    }
    ܦܢܐ *هذا;
}

Locale::Payload& Locale::Payload::operator=(Payload&& other) noexcept U_LIFETIME_BOUND {
    ܐܢ (هذا != &other) {
        setToBogus();
        move(std::move(other));
    }
    ܦܢܐ *هذا;
}

فراغ Locale::Payload::setToBogus() {
    هذا->~Payload();
    type = eBOGUS;
}

template <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    ܐܢ constexpr (std::is_same_v<T, Nest>) {
        هذا->~Payload();
        ::جديد (&nest) Nest(std::forward<Args>(args)...);
        ܦܢܐ nest;
    }
    ܐܢ constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::جديد (&heap) Heap(std::forward<Args>(args)...);
        ܦܢܐ heap;
    }
}

template <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    ܦܢܐ type == eNEST ? &nest : nullptr;
}
template <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    ܦܢܐ type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = افتراضي;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, ܕܓܠܐ);
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
    ܐܢ( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, ܕܓܠܐ); /* shortcut */
    }
    ܐܚܪܢܐ
    {
        UErrorCode status = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        ܐܢ ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            ܐܢ ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                ܦܢܐ;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        ܐܢ ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            ܐܢ ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                ܦܢܐ;
            }
        }

        // _Variant
        ܐܢ ( newVariant != nullptr )
        {
            // remove leading _'s
            ܟܡܐ(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            ܐܢ ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                ܦܢܐ;
            }
            ܟܡܐ( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        ܐܢ ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            ܐܢ ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              ܦܢܐ;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        ܐܢ ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        ܐܢ ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        ܐܢ ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        ܐܢ ( ksize != 0)
        {
            ܐܢ (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            ܐܚܪܢܐ {
                togo.append('_', status); /* Variant parsing with a script */
                ܐܢ ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        ܐܢ (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            ܦܢܐ;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), ܕܓܠܐ);
    }
}

Locale::Locale(ثابت Locale&) = افتراضي;
Locale::Locale(Locale&&) noexcept = افتراضي;

Locale& Locale::operator=(ثابت Locale&) U_LIFETIME_BOUND = افتراضي;
Locale& Locale::operator=(Locale&&) noexcept U_LIFETIME_BOUND = افتراضي;

Locale *
Locale::clone() ثابت {
    ܦܢܐ جديد Locale(*هذا);
}

منطقي
Locale::operator==( ثابت   Locale& other) ثابت
{
    ܦܢܐ uprv_strcmp(other.getName(), getName()) == 0;
}

نطاق {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr ثابت حرف* KNOWN_CANONICALIZED[] = {
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

UBool U_CALLCONV cleanupKnownCanonicalized() {
    gKnownCanonicalizedInitOnce.reset();
    ܐܢ (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    ܦܢܐ ܫܪܪܐ;
}

فراغ U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    لكل (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (فراغ*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

صنف AliasData;

/**
 * A Builder class to build the alias data.
 */
صنف AliasDataBuilder {
عام:
    AliasDataBuilder() {
    }

    // Build the AliasData from resource.
    AliasData* build(UErrorCode &status);

خاص:
    فراغ readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<ثابت حرف*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   فراغ (*checkType)(ثابت حرف* type),
                   فراغ (*checkReplacement)(ثابت UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<ثابت حرف*>& types,
                           LocalMemory<int32_t>& replacementIndexes,
                           int32_t &length,
                           UErrorCode &status);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readScriptAlias(UResourceBundle* alias,
                         UniqueCharStrings* strings,
                         LocalMemory<ثابت حرف*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    فراغ readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<ثابت حرف*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    فراغ readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ثابت حرف*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    فراغ readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ثابت حرف*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
صنف AliasData : عام UMemory {
عام:
    ساكن ثابت AliasData* singleton(UErrorCode& status) {
        ܐܢ (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            ܦܢܐ nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        ܦܢܐ gSingleton;
    }

    ثابت CharStringMap& languageMap() ثابت U_LIFETIME_BOUND { ܦܢܐ language; }
    ثابت CharStringMap& scriptMap() ثابت U_LIFETIME_BOUND { ܦܢܐ script; }
    ثابت CharStringMap& territoryMap() ثابت U_LIFETIME_BOUND { ܦܢܐ territory; }
    ثابت CharStringMap& variantMap() ثابت U_LIFETIME_BOUND { ܦܢܐ variant; }
    ثابت CharStringMap& subdivisionMap() ثابت U_LIFETIME_BOUND { ܦܢܐ subdivision; }

    ساكن فراغ U_CALLCONV loadData(UErrorCode &status);
    ساكن UBool U_CALLCONV cleanup();

    ساكن UInitOnce gInitOnce;

خاص:
    AliasData(CharStringMap languageMap,
              CharStringMap scriptMap,
              CharStringMap territoryMap,
              CharStringMap variantMap,
              CharStringMap subdivisionMap,
              CharString* strings)
        : language(std::move(languageMap)),
          script(std::move(scriptMap)),
          territory(std::move(territoryMap)),
          variant(std::move(variantMap)),
          subdivision(std::move(subdivisionMap)),
          strings(strings) {
    }

    ~AliasData() {
        delete strings;
    }

    ساكن ثابت AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    friend صنف AliasDataBuilder;
};


ثابت AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    ܦܢܐ ܫܪܪܐ;
}

فراغ
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت حرف*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        فراغ (*checkType)(ثابت حرف* type),
        فراغ (*checkReplacement)(ثابت UChar* replacement),
        UErrorCode &status) {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    length = ures_getSize(alias);
    ثابت حرف** rawTypes = types.allocateInsteadAndCopy(length);
    ܐܢ (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    ܐܢ (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    لكل (صحيح i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        ثابت حرف* aliasFrom = ures_getKey(res.getAlias());
        ثابت UChar* aliasTo =
            ures_getStringByKey(res.getAlias(), "replacement", nullptr, &status);
        ܐܢ (U_FAILURE(status)) ܦܢܐ;

        checkType(aliasFrom);
        checkReplacement(aliasTo);

        rawTypes[i] = aliasFrom;
        rawIndexes[i] = strings->add(aliasTo, status);
    }
}

/**
 * Read the languageAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement language.
 */
فراغ
AliasDataBuilder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت حرف*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ܦܢܐ readAlias(
        alias, strings, types, replacementIndexes, length,
#ܐܢ U_DEBUG
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
#ܐܚܪܢܐ
        [](ثابت حرف*) {},
#endif
        [](ثابت UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
فراغ
AliasDataBuilder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت حرف*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ܦܢܐ readAlias(
        alias, strings, types, replacementIndexes, length,
#ܐܢ U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ثابت UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#ܐܚܪܢܐ
        [](ثابت حرف*) {},
        [](ثابت UChar*) { },
#endif
        status);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
فراغ
AliasDataBuilder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت حرف*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ܦܢܐ readAlias(
        alias, strings, types, replacementIndexes, length,
#ܐܢ U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#ܐܚܪܢܐ
        [](ثابت حرف*) {},
#endif
        [](ثابت UChar*) { },
        status);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
فراغ
AliasDataBuilder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت حرف*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ܦܢܐ readAlias(
        alias, strings, types, replacementIndexes, length,
#ܐܢ U_DEBUG
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
#ܐܚܪܢܐ
        [](ثابت حرف*) {},
        [](ثابت UChar*) { },
#endif
        status);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
فراغ
AliasDataBuilder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت حرف*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ܦܢܐ readAlias(
        alias, strings, types, replacementIndexes, length,
#ܐܢ U_DEBUG
        [](ثابت حرف* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#ܐܚܪܢܐ
        [](ثابت حرف*) {},
#endif
        [](ثابت UChar*) { },
        status);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
فراغ U_CALLCONV
AliasData::loadData(UErrorCode &status)
{
#ifdef LOCALE_CANONICALIZATION_DEBUG
    UDate start = uprv_getRawUTCtime();
#endif  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    AliasDataBuilder builder;
    gSingleton = builder.build(status);
#ifdef LOCALE_CANONICALIZATION_DEBUG
    UDate end = uprv_getRawUTCtime();
    printf("AliasData::loadData took total %f ms\n", end - start);
#endif  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
AliasData*
AliasDataBuilder::build(UErrorCode &status) {
    ܐܢ (U_FAILURE(status)) { ܦܢܐ nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_getByKey(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_getByKey(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_getByKey(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_getByKey(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_getByKey(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_getByKey(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<ثابت حرف*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<ثابت حرف*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<ثابت حرف*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<ثابت حرف*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<ثابت حرف*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    لكل (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    لكل (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    لكل (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    لكل (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    لكل (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ nullptr;
    }

    // copy hashtables
    auto *data = جديد AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    ܐܢ (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ܦܢܐ data;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
صنف AliasReplacer {
عام:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       ܦܢܐ 0==uprv_strcmp((ثابت حرف*)e1.pointer,
                                             (ثابت حرف*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    منطقي replace(
        ثابت Locale& locale, CharString& out, UErrorCode& status);

خاص:
    ثابت حرف* language;
    ثابت حرف* script;
    ثابت حرف* region;
    ثابت حرف* extensions;
    UVector variants;

    ثابت AliasData* data;

    مضمن منطقي notEmpty(ثابت حرف* str) {
        ܦܢܐ str && str[0] != NULL_CHAR;
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
        ܦܢܐ notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    مضمن منطقي same(ثابت حرف* a, ثابت حرف* b) {
        ܐܢ (a == nullptr && b == nullptr) {
            ܦܢܐ ܫܪܪܐ;
        }
        ܐܢ ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          ܦܢܐ ܕܓܠܐ;
        }
        ܦܢܐ uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generateKey(ثابت حرف* language, ثابت حرف* region,
                            ثابت حرف* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    فراغ parseLanguageReplacement(ثابت حرف* replacement,
                                  ثابت حرف*& replaceLanguage,
                                  ثابت حرف*& replaceScript,
                                  ثابت حرف*& replaceRegion,
                                  ثابت حرف*& replaceVariant,
                                  ثابت حرف*& replaceExtensions,
                                  UVector& toBeFreed,
                                  UErrorCode& status);

    // Replace by using languageAlias.
    منطقي replaceLanguage(منطقي checkLanguage, منطقي checkRegion,
                         منطقي checkVariants, UVector& toBeFreed,
                         UErrorCode& status);

    // Replace by using territoryAlias.
    منطقي replaceTerritory(UVector& toBeFreed, UErrorCode& status);

    // Replace by using scriptAlias.
    منطقي replaceScript(UErrorCode& status);

    // Replace by using variantAlias.
    منطقي replaceVariant(UErrorCode& status);

    // Replace by using subdivisionAlias.
    منطقي replaceSubdivision(StringPiece subdivision,
                            CharString& output, UErrorCode& status);

    // Replace transformed extensions.
    منطقي replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UErrorCode& status);
};

CharString&
AliasReplacer::generateKey(
        ثابت حرف* language, ثابت حرف* region, ثابت حرف* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    ܐܢ (U_FAILURE(status)) { ܦܢܐ out; }
    out.append(language, status);
    ܐܢ (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    ܐܢ (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    ܦܢܐ out;
}

فراغ
AliasReplacer::parseLanguageReplacement(
    ثابت حرف* replacement,
    ثابت حرف*& replacedLanguage,
    ثابت حرف*& replacedScript,
    ثابت حرف*& replacedRegion,
    ثابت حرف*& replacedVariant,
    ثابت حرف*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    ܐܢ (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        ܦܢܐ;
    }
    // We have multiple field so we have to allocate and parse
    auto str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    حرف* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    replacedLanguage = (ثابت حرف*) data;
    حرف* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    ثابت حرف* start = endOfField;
    endOfField = const_cast<حرف*>(uprv_strchr(start, '_'));
    size_t len = 0;
    ܐܢ (endOfField == nullptr) {
        len = uprv_strlen(start);
    } ܐܚܪܢܐ {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    ܐܢ (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        ܐܢ (endOfField == nullptr) {
            ܦܢܐ;
        }
        start = endOfField++;
        endOfField = const_cast<حرف*>(uprv_strchr(start, '_'));
        ܐܢ (endOfField == nullptr) {
            len = uprv_strlen(start);
        } ܐܚܪܢܐ {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    ܐܢ (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        ܐܢ (endOfField == nullptr) {
            ܦܢܐ;
        }
        start = endOfField++;
        endOfField = const_cast<حرف*>(uprv_strchr(start, '_'));
        ܐܢ (endOfField == nullptr) {
            len = uprv_strlen(start);
        } ܐܚܪܢܐ {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    ܐܢ (len >= 4) {
        // Got a variant
        replacedVariant = start;
        ܐܢ (endOfField == nullptr) {
            ܦܢܐ;
        }
        start = endOfField++;
    }
    replacedExtensions = start;
}

منطقي
AliasReplacer::replaceLanguage(
        منطقي checkLanguage, منطقي checkRegion,
        منطقي checkVariants, UVector& toBeFreed, UErrorCode& status)
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        ܦܢܐ ܕܓܠܐ;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ثابت حرف* searchLanguage = checkLanguage ? language : "und";
    ثابت حرف* searchRegion = checkRegion ? region : nullptr;
    ثابت حرف* searchVariant = nullptr;
    لكل (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        ܐܢ (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            searchVariant = static_cast<ثابت حرف*>(variants.elementAt(variant_index));
        }

        ܐܢ (searchVariant != nullptr && uprv_strlen(searchVariant) < 4) {
            // Do not consider  ill-formed variant subtag.
            searchVariant = nullptr;
        }
        CharString typeKey;
        generateKey(searchLanguage, searchRegion, searchVariant, typeKey,
                    status);
        ܐܢ (U_FAILURE(status)) {
            ܦܢܐ ܕܓܠܐ;
        }
        ثابت حرف *replacement = data->languageMap().get(typeKey.data());
        ܐܢ (replacement == nullptr) {
            // Found no replacement data.
            استمر;
        }

        ثابت حرف* replacedLanguage = nullptr;
        ثابت حرف* replacedScript = nullptr;
        ثابت حرف* replacedRegion = nullptr;
        ثابت حرف* replacedVariant = nullptr;
        ثابت حرف* replacedExtensions = nullptr;
        parseLanguageReplacement(replacement,
                                 replacedLanguage,
                                 replacedScript,
                                 replacedRegion,
                                 replacedVariant,
                                 replacedExtensions,
                                 toBeFreed,
                                 status);
        replacedLanguage =
            (replacedLanguage != nullptr && uprv_strcmp(replacedLanguage, "und") == 0) ?
            language : replacedLanguage;
        replacedScript = deleteOrReplace(script, nullptr, replacedScript);
        replacedRegion = deleteOrReplace(region, searchRegion, replacedRegion);
        replacedVariant = deleteOrReplace(
            searchVariant, searchVariant, replacedVariant);

        ܐܢ (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(searchVariant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            استمر;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        ܐܢ (searchVariant != nullptr) {
            ܐܢ (notEmpty(replacedVariant)) {
                variants.setElementAt((فراغ*)replacedVariant, variant_index);
            } ܐܚܪܢܐ {
                variants.removeElementAt(variant_index);
            }
        }
        ܐܢ (replacedExtensions != nullptr) {
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
        ܦܢܐ ܫܪܪܐ;
    }
    // Nothing changed by language alias data.
    ܦܢܐ ܕܓܠܐ;
}

منطقي
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ (region == nullptr) {
        // No region to search.
        ܦܢܐ ܕܓܠܐ;
    }
    ثابت حرف *replacement = data->territoryMap().get(region);
    ܐܢ (replacement == nullptr) {
        // Found no replacement data for this region.
        ܦܢܐ ܕܓܠܐ;
    }
    ثابت حرف* replacedRegion = replacement;
    ثابت حرف* firstSpace = uprv_strchr(replacement, ' ');
    ܐܢ (firstSpace != nullptr) {
        // If there are are more than one region in the replacement.
        // We need to check which one match based on the language.
        // Cannot use nullptr for language because that will construct
        // the default locale, in that case, use "und" to get the correct
        // locale.
        Locale l = LocaleBuilder()
            .setLanguage(language == nullptr ? "und" : language)
            .setScript(script)
            .build(status);
        l.addLikelySubtags(status);
        ثابت حرف* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        ܐܢ (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ثابت حرف* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            ܐܢ (foundInReplacement != nullptr) {
                // Assuming the case there are no three letter region code in
                // the replacement of territoryAlias
                U_ASSERT(foundInReplacement == replacement ||
                         *(foundInReplacement-1) == ' ');
                U_ASSERT(foundInReplacement[len] == ' ' ||
                         foundInReplacement[len] == '\0');
                item = prv::make_unique<CharString>(
                    foundInReplacement, static_cast<int32_t>(len), status, status);
            }
        }
        ܐܢ (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        ܐܢ (U_FAILURE(status)) { ܦܢܐ ܕܓܠܐ; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        ܐܢ (U_FAILURE(status)) { ܦܢܐ ܕܓܠܐ; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    ܦܢܐ ܫܪܪܐ;
}

منطقي
AliasReplacer::replaceScript(UErrorCode& status)
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ (script == nullptr) {
        // No script to search.
        ܦܢܐ ܕܓܠܐ;
    }
    ثابت حرف *replacement = data->scriptMap().get(script);
    ܐܢ (replacement == nullptr) {
        // Found no replacement data for this script.
        ܦܢܐ ܕܓܠܐ;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    ܦܢܐ ܫܪܪܐ;
}

منطقي
AliasReplacer::replaceVariant(UErrorCode& status)
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    // Since we may have more than one variant, we need to loop through them.
    لكل (int32_t i = 0; i < variants.size(); i++) {
        ثابت حرف* variant = static_cast<ثابت حرف*>(variants.elementAt(i));
        ثابت حرف *replacement = data->variantMap().get(variant);
        ܐܢ (replacement == nullptr) {
            // Found no replacement data for this variant.
            استمر;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        ܐܢ (!same(variant, replacement)) {
            variants.setElementAt((فراغ*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            ܐܢ (uprv_strcmp(variant, "heploc") == 0) {
                لكل (int32_t j = 0; j < variants.size(); j++) {
                     ܐܢ (uprv_strcmp((ثابت حرف*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            ܦܢܐ ܫܪܪܐ;
        }
    }
    ܦܢܐ ܕܓܠܐ;
}

منطقي
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ثابت حرف *replacement = data->subdivisionMap().get(subdivision.data());
    ܐܢ (replacement != nullptr) {
        ثابت حرف* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        ܐܢ (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            ܐܢ (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        ܦܢܐ ܫܪܪܐ;
    }
    ܦܢܐ ܕܓܠܐ;
}

منطقي
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    int32_t len = transformedExtensions.length();
    ثابت حرف* str = transformedExtensions.data();
    ثابت حرف* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    ܐܢ (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        ܐܢ (U_FAILURE(status)) {
            ܦܢܐ ܕܓܠܐ;
        }
        T_CString_toLowerCase(output.data());
    }
    ܐܢ (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        ܐܢ (U_FAILURE(status)) {
            ܦܢܐ ܕܓܠܐ;
        }
        ܥܒܕ {
            ثابت حرف* tvalue = uprv_strchr(tkey, '-');
            ܐܢ (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ܦܢܐ ܕܓܠܐ;
            }
            ثابت حرف* nextTKey = ultag_getTKeyStart(tvalue);
            ܐܢ (nextTKey != nullptr) {
                *const_cast<حرف*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.insertElementAt((فراغ*)tkey, tfields.size(), status);
            ܐܢ (U_FAILURE(status)) {
                ܦܢܐ ܕܓܠܐ;
            }
            tkey = nextTKey;
        } ܟܡܐ (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            ܦܢܐ uprv_strcmp((ثابت حرف*)e1.pointer, (ثابت حرف*)e2.pointer);
        }, status);
        لكل (int32_t i = 0; i < tfields.size(); i++) {
             ܐܢ (output.length() > 0) {
                 output.append('-', status);
             }
             ثابت حرف* tfield = static_cast<ثابت حرف*>(tfields.elementAt(i));
             ثابت حرف* tvalue = uprv_strchr(tfield, '-');
             ܐܢ (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 ܦܢܐ ܕܓܠܐ;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<حرف*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ܦܢܐ ܫܪܪܐ;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    ܐܢ (U_FAILURE(status)) { ܦܢܐ out; }
    out.append(language, status);
    ܐܢ (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    ܐܢ (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    ܐܢ (variants.size() > 0) {
        ܐܢ (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            ܦܢܐ uprv_strcmp((ثابت حرف*)e1.pointer, (ثابت حرف*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        لكل (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<ثابت حرف*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    ܐܢ (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    ܦܢܐ out;
}

منطقي
AliasReplacer::replace(ثابت Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    U_ASSERT(data != nullptr);
    out.clear();
    language = locale.getLanguage();
    ܐܢ (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    ܐܢ (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    ܐܢ (!notEmpty(region)) {
        region = nullptr;
    }
    ثابت حرف* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    ܐܢ (!variantsBuff.isEmpty()) {
        ܐܢ (U_FAILURE(status)) { ܦܢܐ ܕܓܠܐ; }
        حرف* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        حرف* end;
        ܟܡܐ ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            ܐܢ (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        ܐܢ (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    ܐܢ (U_FAILURE(status)) { ܦܢܐ ܕܓܠܐ; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        ܦܢܐ uprv_strcmp((ثابت حرف*)e1.pointer, (ثابت حرف*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    ثابت int32_t maxChanged = 4 + variants.size();
    (فراغ)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](فراغ *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    ܟܡܐ (U_SUCCESS(status)) {
        // Something wrong with the data cause looping here more than maxChanged
        // times already.
        U_ASSERT(changed <= maxChanged);
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
        ܐܢ (    replaceLanguage(ܫܪܪܐ, ܫܪܪܐ,  ܫܪܪܐ,  stringsToBeFreed, status) ||
                replaceLanguage(ܫܪܪܐ, ܫܪܪܐ,  ܕܓܠܐ, stringsToBeFreed, status) ||
                replaceLanguage(ܫܪܪܐ, ܕܓܠܐ, ܫܪܪܐ,  stringsToBeFreed, status) ||
                replaceLanguage(ܫܪܪܐ, ܕܓܠܐ, ܕܓܠܐ, stringsToBeFreed, status) ||
                replaceLanguage(ܕܓܠܐ,ܕܓܠܐ, ܫܪܪܐ,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            استمر;
        }
        // Nothing changed. Break out.
        اكسر;
    }  // while(1)

    ܐܢ (U_FAILURE(status)) { ܦܢܐ ܕܓܠܐ; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ثابت حرف* extensionsStr = locale_getKeywordsStart(locale.getName());
    ܐܢ (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        ܦܢܐ ܕܓܠܐ;
    }
    outputToString(out, status);
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        ܐܢ (U_SUCCESS(status) && !iter.isNull()) {
            ثابت حرف* key;
            ܟܡܐ ((key = iter->next(nullptr, status)) != nullptr) {
                ܐܢ (uprv_strcmp("sd", key) == 0 || uprv_strcmp("rg", key) == 0 ||
                        uprv_strcmp("t", key) == 0) {
                    auto value = locale.getKeywordValue<CharString>(key, status);
                    ܐܢ (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        استمر;
                    }
                    CharString replacement;
                    ܐܢ (uprv_strlen(key) == 2) {
                        ܐܢ (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.setKeywordValue(key, replacement.data(), status);
                        }
                    } ܐܚܪܢܐ {
                        U_ASSERT(uprv_strcmp(key, "t") == 0);
                        ܐܢ (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.setKeywordValue(key, replacement.data(), status);
                        }
                    }
                    ܐܢ (U_FAILURE(status)) {
                        ܦܢܐ ܕܓܠܐ;
                    }
                }
            }
        }
        ܐܢ (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    // If the tag is not changed, return.
    ܐܢ (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        ܦܢܐ ܕܓܠܐ;
    }
    ܦܢܐ ܫܪܪܐ;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
منطقي
canonicalizeLocale(ثابت Locale& locale, CharString& out, UErrorCode& status)
{
    ܐܢ (U_FAILURE(status)) { ܦܢܐ ܕܓܠܐ; }
    AliasReplacer replacer(status);
    ܦܢܐ replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
منطقي
isKnownCanonicalizedLocale(ثابت حرف* locale, UErrorCode& status)
{
    ܐܢ (U_FAILURE(status)) { ܦܢܐ ܕܓܠܐ; }

    ܐܢ (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        ܦܢܐ ܫܪܪܐ;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    ܦܢܐ uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT ثابت حرف* ثابت*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    ܦܢܐ KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT منطقي
ulocimp_isCanonicalizedLocaleForTest(ثابت حرف* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    ܦܢܐ !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ثابت حرف* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    ܦܢܐ localeID == nullptr ? *هذا = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    ܥܒܕ {
        حرف *separator;
        حرف *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        ثابت auto parse = [canonicalize](std::string_view localeID,
                                          حرف* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            ܦܢܐ ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    ܐܢ (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } ܐܚܪܢܐ {
                        ulocimp_getName(localeID, sink, status);
                    }
                },
                status);
        };

        // "canonicalize" the locale ID to ICU/Java format
        حرف* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, حجم Nest::baseName, err);

        FixedString fullNameBuffer;
        ܐܢ (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(حجم Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            ܐܢ (!fullNameBuffer.reserve(length + 1)) {
                اكسر; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        ܐܢ(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            اكسر;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        حرف* at = uprv_strchr(fullName, '@');
        ܟܡܐ ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        حرف* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        ܐܢ (separator!=nullptr || sep2!=nullptr) {
            ܐܢ (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } ܐܚܪܢܐ {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        منطقي hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        ܐܢ (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            اكسر; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        ܐܢ (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        ܐܢ (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        ܐܢ (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } ܐܚܪܢܐ ܐܢ (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        ܐܢ (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } ܐܚܪܢܐ ܐܢ (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        ܐܢ (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } ܐܚܪܢܐ {
            ܐܢ (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                ܐܢ (fullNameBuffer.isEmpty()) {
                    اكسر; // error: out of memory
                }
                ܐܢ (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                ܐܢ (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                ܐܢ (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            ܐܢ (isBogus()) {
                اكسر; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            ܐܢ (hasKeywords) {
                ܐܢ (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    ܐܢ (heap.ptr->baseName.isEmpty()) {
                        اكسر; // error: out of memory
                    }
                }
            }
        }

        ܐܢ (canonicalize) {
            ܐܢ (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                ܐܢ (canonicalizeLocale(*هذا, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), ܕܓܠܐ);
                }
                ܐܢ (U_FAILURE(err)) {
                    اكسر;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        ܦܢܐ *هذا;
    } ܟܡܐ(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    ܦܢܐ *هذا;
}

int32_t
Locale::hashCode() ثابت
{
    ܦܢܐ ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

فراغ
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

ثابت Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        ܐܢ (gDefaultLocale != nullptr) {
            ܦܢܐ *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    ܦܢܐ *locale_set_default_internal(nullptr, status);
}



فراغ U_EXPORT2
Locale::setDefault( ثابت   Locale&     newLocale,
                            UErrorCode&  status)
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ثابت حرف *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

فراغ
Locale::addLikelySubtags(UErrorCode& status) {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    ܐܢ (U_FAILURE(status)) {
        ܐܢ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ܦܢܐ;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/ܕܓܠܐ);
    ܐܢ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

فراغ
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(ܕܓܠܐ, status);
}
فراغ
Locale::minimizeSubtags(منطقي favorScript, UErrorCode& status) {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    ܐܢ (U_FAILURE(status)) {
        ܐܢ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ܦܢܐ;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/ܕܓܠܐ);
    ܐܢ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

فراغ
Locale::canonicalize(UErrorCode& status) {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    ܐܢ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ;
    }
    CharString uncanonicalized(getName(), status);
    ܐܢ (U_FAILURE(status)) {
        ܐܢ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ܦܢܐ;
    }
    init(uncanonicalized.data(), /*canonicalize=*/ܫܪܪܐ);
    ܐܢ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ result;
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

    int32_t parsedLength;
    CharString localeID = ulocimp_forLanguageTag(
            tag.data(),
            tag.length(),
            &parsedLength,
            status);

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ result;
    }

    ܐܢ (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ result;
    }

    result.init(localeID.data(), /*canonicalize=*/ܕܓܠܐ);
    ܐܢ (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ܦܢܐ result;
}

فراغ
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) ثابت
{
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }

    ܐܢ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/ܕܓܠܐ, status);
}

Locale U_EXPORT2
Locale::createFromName (ثابت حرف *name)
{
    ܐܢ (name) {
        Locale l("");
        l.init(name, ܕܓܠܐ);
        ܦܢܐ l;
    }
    ܐܚܪܢܐ {
        ܦܢܐ getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, ܕܓܠܐ);
    ܦܢܐ loc;
}

Locale U_EXPORT2
Locale::createCanonical(ثابت حرف* name) {
    Locale loc("");
    loc.init(name, ܫܪܪܐ);
    ܦܢܐ loc;
}

ثابت حرف *
Locale::getISO3Language() ثابت
{
    ܦܢܐ uloc_getISO3Language(getName());
}


ثابت حرف *
Locale::getISO3Country() ثابت
{
    ܦܢܐ uloc_getISO3Country(getName());
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
    ܦܢܐ uloc_getLCID(getName());
}

ثابت حرف* ثابت* U_EXPORT2 Locale::getISOCountries()
{
    ܦܢܐ uloc_getISOCountries();
}

ثابت حرف* ثابت* U_EXPORT2 Locale::getISOLanguages()
{
    ܦܢܐ uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
فراغ Locale::setFromPOSIXID(ثابت حرف *posixID)
{
    init(posixID, ܫܪܪܐ);
}

ثابت Locale & U_EXPORT2
Locale::getRoot()
{
    ܦܢܐ getLocale(eROOT);
}

ثابت Locale & U_EXPORT2
Locale::getEnglish()
{
    ܦܢܐ getLocale(eENGLISH);
}

ثابت Locale & U_EXPORT2
Locale::getFrench()
{
    ܦܢܐ getLocale(eFRENCH);
}

ثابت Locale & U_EXPORT2
Locale::getGerman()
{
    ܦܢܐ getLocale(eGERMAN);
}

ثابت Locale & U_EXPORT2
Locale::getItalian()
{
    ܦܢܐ getLocale(eITALIAN);
}

ثابت Locale & U_EXPORT2
Locale::getJapanese()
{
    ܦܢܐ getLocale(eJAPANESE);
}

ثابت Locale & U_EXPORT2
Locale::getKorean()
{
    ܦܢܐ getLocale(eKOREAN);
}

ثابت Locale & U_EXPORT2
Locale::getChinese()
{
    ܦܢܐ getLocale(eCHINESE);
}

ثابت Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    ܦܢܐ getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    ܦܢܐ getLocale(eTAIWAN);
}


ثابت Locale & U_EXPORT2
Locale::getFrance()
{
    ܦܢܐ getLocale(eFRANCE);
}

ثابت Locale & U_EXPORT2
Locale::getGermany()
{
    ܦܢܐ getLocale(eGERMANY);
}

ثابت Locale & U_EXPORT2
Locale::getItaly()
{
    ܦܢܐ getLocale(eITALY);
}

ثابت Locale & U_EXPORT2
Locale::getJapan()
{
    ܦܢܐ getLocale(eJAPAN);
}

ثابت Locale & U_EXPORT2
Locale::getKorea()
{
    ܦܢܐ getLocale(eKOREA);
}

ثابت Locale & U_EXPORT2
Locale::getChina()
{
    ܦܢܐ getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getPRC()
{
    ܦܢܐ getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getTaiwan()
{
    ܦܢܐ getLocale(eTAIWAN);
}

ثابت Locale & U_EXPORT2
Locale::getUK()
{
    ܦܢܐ getLocale(eUK);
}

ثابت Locale & U_EXPORT2
Locale::getUS()
{
    ܦܢܐ getLocale(eUS);
}

ثابت Locale & U_EXPORT2
Locale::getCanada()
{
    ܦܢܐ getLocale(eCANADA);
}

ثابت Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    ܦܢܐ getLocale(eCANADA_FRENCH);
}

ثابت Locale &
Locale::getLocale(صحيح locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    ܐܢ (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    ܦܢܐ localeCache[locid]; /*operating on nullptr*/
}

/*
This function is defined this way in order to get around static
initialization and static destruction.
 */
Locale *
Locale::getLocaleCache()
{
    UErrorCode status = U_ZERO_ERROR;
    umtx_initOnce(gLocaleCacheInitOnce, locale_init, status);
    ܦܢܐ gLocaleCache;
}

صنف KeywordEnumeration : عام StringEnumeration {
محمي:
    FixedString keywords;
خاص:
    int32_t length;
    ثابت حرف *current;
    ساكن ثابت حرف fgClassID;

عام:
    ساكن UClassID U_EXPORT2 getStaticClassID() { ܦܢܐ (UClassID)&fgClassID; }
    virtual UClassID getDynamicClassID() ثابت override { ܦܢܐ getStaticClassID(); }
عام:
    KeywordEnumeration(ثابت حرف *keys, int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        ܐܢ(U_SUCCESS(status) && keywordLen != 0) {
            ܐܢ(keys == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } ܐܚܪܢܐ {
                keywords = {keys, static_cast<std::string_view::size_type>(length)};
                ܐܢ (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ܐܚܪܢܐ {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    virtual ~KeywordEnumeration();

    virtual StringEnumeration * clone() ثابت override
    {
        UErrorCode status = U_ZERO_ERROR;
        ܦܢܐ جديد KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    virtual int32_t count(UErrorCode& status) ثابت override {
        ܐܢ (U_FAILURE(status)) { ܦܢܐ 0; }
        ثابت حرف *kw = keywords.data();
        int32_t result = 0;
        ܟܡܐ(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        ܦܢܐ result;
    }

    virtual ثابت حرف* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ثابت حرف* result;
        int32_t len;
        ܐܢ(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            ܐܢ(resultLength != nullptr) {
                *resultLength = len;
            }
        } ܐܚܪܢܐ {
            ܐܢ(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        ܦܢܐ result;
    }

    virtual ثابت UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        ܐܢ (U_FAILURE(status)) { ܦܢܐ nullptr; }
        int32_t resultLength = 0;
        ثابت حرف *s = next(&resultLength, status);
        ܦܢܐ setChars(s, resultLength, status);
    }

    virtual فراغ reset(UErrorCode& status) override {
        ܐܢ (U_FAILURE(status)) { ܦܢܐ; }
        current = keywords.data();
    }
};

ثابت حرف KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = افتراضي;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
صنف UnicodeKeywordEnumeration : عام KeywordEnumeration {
عام:
    using KeywordEnumeration::KeywordEnumeration;
    virtual ~UnicodeKeywordEnumeration();

    virtual ثابت حرف* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ثابت حرف* legacy_key = KeywordEnumeration::next(nullptr, status);
        ܟܡܐ (U_SUCCESS(status) && legacy_key != nullptr) {
            ثابت حرف* key = uloc_toUnicodeLocaleKey(legacy_key);
            ܐܢ (key != nullptr) {
                ܐܢ (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(key));
                }
                ܦܢܐ key;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_key = KeywordEnumeration::next(nullptr, status);
        }
        ܐܢ (resultLength != nullptr) *resultLength = 0;
        ܦܢܐ nullptr;
    }
    virtual int32_t count(UErrorCode& status) ثابت override {
        ܐܢ (U_FAILURE(status)) { ܦܢܐ 0; }
        ثابت حرف *kw = keywords.data();
        int32_t result = 0;
        ܟܡܐ(*kw) {
            ܐܢ (uloc_toUnicodeLocaleKey(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        ܦܢܐ result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = افتراضي;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) ثابت
{
    StringEnumeration *result = nullptr;

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ result;
    }

    ثابت حرف* variantStart = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    ܐܢ(variantStart) {
        ܐܢ(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', ܕܓܠܐ, status);
            ܐܢ (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = جديد KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                ܐܢ (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } ܐܚܪܢܐ {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ܦܢܐ result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) ثابت
{
    StringEnumeration *result = nullptr;

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ result;
    }

    ثابت حرف* variantStart = uprv_strchr(getName(), '@');
    ثابت حرف* assignment = uprv_strchr(getName(), '=');
    ܐܢ(variantStart) {
        ܐܢ(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', ܕܓܠܐ, status);
            ܐܢ (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = جديد UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                ܐܢ (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } ܐܚܪܢܐ {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ܦܢܐ result;
}

int32_t
Locale::getKeywordValue(ثابت حرف* keywordName, حرف *buffer, int32_t bufLen, UErrorCode &status) ثابت
{
    ܦܢܐ uloc_getKeywordValue(getName(), keywordName, buffer, bufLen, &status);
}

فراغ
Locale::getKeywordValue(StringPiece keywordName, ByteSink& sink, UErrorCode& status) ثابت {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }

    ܐܢ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ;
    }

    ulocimp_getKeywordValue(getName(), keywordName, sink, status);
}

فراغ
Locale::getUnicodeKeywordValue(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) ثابت {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }

    std::optional<std::string_view> legacy_key = ulocimp_toLegacyKeyWithFallback(keywordName);
    ܐܢ (!legacy_key.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ;
    }

    auto legacy_value = getKeywordValue<CharString>(*legacy_key, status);

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }

    std::optional<std::string_view> unicode_value =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_value.toStringPiece());
    ܐܢ (!unicode_value.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ;
    }

    sink.Append(unicode_value->data(), static_cast<int32_t>(unicode_value->size()));
}

فراغ
Locale::setKeywordValue(StringPiece keywordName,
                        StringPiece keywordValue,
                        UErrorCode& status) {
    ܐܢ (U_FAILURE(status)) { ܦܢܐ; }
    ܐܢ (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ;
    }
    ܐܢ (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_setKeywordValue(keywordName, keywordValue, localeID, status);
    ܐܢ (U_FAILURE(status)) {
        ܐܢ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ܦܢܐ;
    }

    ثابت حرف* at = locale_getKeywordsStart(localeID.toStringPiece());
    منطقي hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    ܐܢ (!hasKeywords) {
        ܐܢ (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            ܐܢ (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < حجم Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, حجم Nest::baseName, status);
            } ܐܚܪܢܐ {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                ܐܢ (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ܦܢܐ;
                }
            }
        }
    } ܐܚܪܢܐ {
        Heap* heap = nullptr;
        ܐܢ (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            ܐܢ (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ܦܢܐ;
            }
        } ܐܚܪܢܐ {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        ܐܢ (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            ܦܢܐ;
        }

        ܐܢ (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            ܐܢ (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                ܐܢ (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ܦܢܐ;
                }
            }
        }
    }
}

فراغ
Locale::setUnicodeKeywordValue(StringPiece keywordName,
                               StringPiece keywordValue,
                               UErrorCode& status) {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }

    std::optional<std::string_view> legacy_key = ulocimp_toLegacyKeyWithFallback(keywordName);
    ܐܢ (!legacy_key.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ;
    }

    std::string_view value;

    ܐܢ (!keywordValue.empty()) {
        std::optional<std::string_view> legacy_value =
            ulocimp_toLegacyTypeWithFallback(keywordName, keywordValue);
        ܐܢ (!legacy_value.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ܦܢܐ;
        }
        value = *legacy_value;
    }

    setKeywordValue(*legacy_key, value, status);
}

ثابت حرف*
Locale::getCountry() ثابت U_LIFETIME_BOUND {
    ܦܢܐ getField<&Nest::getRegion, &Heap::getRegion>();
}

ثابت حرف*
Locale::getLanguage() ثابت U_LIFETIME_BOUND {
    ܦܢܐ getField<&Nest::getLanguage, &Heap::getLanguage>();
}

ثابت حرف*
Locale::getScript() ثابت U_LIFETIME_BOUND {
    ܦܢܐ getField<&Nest::getScript, &Heap::getScript>();
}

ثابت حرف*
Locale::getVariant() ثابت U_LIFETIME_BOUND {
    ܦܢܐ getField<&Nest::getVariant, &Heap::getVariant>();
}

ثابت حرف*
Locale::getName() ثابت U_LIFETIME_BOUND {
    ܦܢܐ getField<&Nest::getBaseName, &Heap::getFullName>();
}

ثابت حرف*
Locale::getBaseName() ثابت U_LIFETIME_BOUND {
    ܦܢܐ getField<&Nest::getBaseName, &Heap::getBaseName>();
}

template <ثابت حرف* (Locale::Nest::*ثابت NEST)() ثابت,
          ثابت حرف* (Locale::Heap::*ثابت HEAP)() ثابت>
ثابت حرف* Locale::getField() ثابت U_LIFETIME_BOUND {
    ܦܢܐ payload.visit([] { ܦܢܐ ""; },
                         [](ثابت Nest& nest) { ܦܢܐ (nest.*NEST)(); },
                         [](ثابت Heap& heap) { ܦܢܐ (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = افتراضي;

//eof
U_NAMESPACE_END
