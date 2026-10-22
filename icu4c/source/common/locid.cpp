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

#અન્તર્ભાવ <cstddef>
#અન્તર્ભાવ <optional>
#અન્તર્ભાવ <string_view>
#અન્તર્ભાવ <type_traits>
#અન્તર્ભાવ <utility>

#અન્તર્ભાવ "unicode/bytestream.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/localebuilder.h"
#અન્તર્ભાવ "unicode/localpointer.h"
#અન્તર્ભાવ "unicode/strenum.h"
#અન્તર્ભાવ "unicode/stringpiece.h"
#અન્તર્ભાવ "unicode/uloc.h"
#અન્તર્ભાવ "unicode/ures.h"

#અન્તર્ભાવ "bytesinkutil.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "charstrmap.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "fixedstring.h"
#અન્તર્ભાવ "mutex.h"
#અન્તર્ભાવ "putilimp.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "ulocimp.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "uniquecharstr.h"
#અન્તર્ભાવ "ustr_imp.h"
#અન્તર્ભાવ "uvector.h"

#જો UPRV_HAS_FEATURE(memory_sanitizer)
#અન્તર્ભાવ <sanitizer/msan_interface.h>
#endif

U_NAMESPACE_BEGIN

સ્થિર Locale   *gLocaleCache = nullptr;
સ્થિર UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
સ્થિર UMutex gDefaultLocaleMutex;
સ્થિર UHashtable *gDefaultLocalesHashT = nullptr;
સ્થિર Locale *gDefaultLocale = nullptr;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#define ULOC_STRING_LIMIT 357913941

U_NAMESPACE_END

પ્રકારવ્યાખ્યા ગણના ELocalePos {
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
    e_મૂળ,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

નામાકાશ {

//
// Deleter function for Locales owned by the default Locale hash table/
//
શૂન્ય U_CALLCONV
deleteLocale(શૂન્ય *obj) {
    delete static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    delete [] gLocaleCache;
    gLocaleCache = nullptr;
    gLocaleCacheInitOnce.reset();

    જો (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    પાછા સાચું;
}

શૂન્ય U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = નવું Locale[static_cast<પૂર્ણાંક>(eMAX_LOCALES)];
    જો (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_મૂળ]          = Locale("");
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

Locale *locale_set_default_internal(અચલ અક્ષર *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = ખોટું;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    જો (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = સાચું; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    જો (U_FAILURE(status)) {
        પાછા gDefaultLocale;
    }

    જો (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        જો (U_FAILURE(status)) {
            પાછા gDefaultLocale;
        }
        uhash_set_કિંમત_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    જો (newDefault == nullptr) {
        newDefault = નવું Locale(Locale::eBOGUS);
        જો (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            પાછા gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), ખોટું);
        uhash_put(gDefaultLocalesHashT, const_cast<અક્ષર*>(newDefault->getName()), newDefault, &status);
        જો (U_FAILURE(status)) {
            પાછા gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    પાછા gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC શૂન્ય
locale_set_default(અચલ અક્ષર *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC અચલ અક્ષર *
locale_get_default()
{
    U_NAMESPACE_USE
    પાછા Locale::getDefault().getName();
}

નામાકાશ {

template <auto FIELD, typename T>
શૂન્ય copyToArray(std::string_view sv, T* that) {
    auto& field = that->*FIELD;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    જો (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.data(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

શૂન્ય Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, આ);
    copyToArray<&Nest::script>(script, આ);
    copyToArray<&Nest::region>(region, આ);
    આ->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(Heap, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(Heap, region));
    U_ASSERT(આ == reinterpret_cast<Nest*>(&heap));
#જો UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, કદ(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, આ);
    copyToArray<&Nest::region>(heap.region, આ);
    આ->variantBegin = variantBegin;
    *આ->baseName = '\0';
}

સંરચના Locale::Heap::Alloc : જાહેર UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    અચલ અક્ષર* getVariant() અચલ U_LIFETIME_BOUND {
        પાછા variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    અચલ અક્ષર* getFullName() અચલ U_LIFETIME_BOUND { પાછા fullName.data(); }
    અચલ અક્ષર* getBaseName() અચલ U_LIFETIME_BOUND {
        જો (baseName.isEmpty()) {
            જો (અચલ અક્ષર* name = fullName.data(); *name != '@') {
                પાછા name;
            }
        }
        પાછા baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(અચલ Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        જો (U_SUCCESS(status)) {
            જો (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                જો (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } નહીંતર {
                    જો (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        જો (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = delete;

    ~Alloc() = મૂળભૂત;
};

અચલ અક્ષર* Locale::Heap::getVariant() અચલ U_LIFETIME_BOUND { પાછા ptr->getVariant(); }
અચલ અક્ષર* Locale::Heap::getFullName() અચલ U_LIFETIME_BOUND { પાછા ptr->getFullName(); }
અચલ અક્ષર* Locale::Heap::getBaseName() અચલ U_LIFETIME_BOUND { પાછા ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = નવું Alloc(variantBegin);
    જો (ptr == nullptr) {
        type = eBOGUS;
    } નહીંતર {
        type = eHEAP;
        copyToArray<&Heap::language>(language, આ);
        copyToArray<&Heap::script>(script, આ);
        copyToArray<&Heap::region>(region, આ);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::operator=(અચલ Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = નવું Alloc(*other.ptr, status);
    જો (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } નહીંતર {
        type = eHEAP;
        uprv_memcpy(language, other.language, કદ language);
        uprv_memcpy(script, other.script, કદ script);
        uprv_memcpy(region, other.region, કદ region);
    }
    પાછા *આ;
}

Locale::Heap& Locale::Heap::operator=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, કદ language);
    uprv_memcpy(script, other.script, કદ script);
    uprv_memcpy(region, other.region, કદ region);
    પાછા *આ;
}

template <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
auto Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) અચલ {
    પસંદગી (type) {
        સ્થિતિ eBOGUS:
            પાછા bogusFn(args...);
        સ્થિતિ eNEST:
            પાછા nestFn(nest, args...);
        સ્થિતિ eHEAP:
            પાછા heapFn(heap, args...);
        મૂળભૂત:
            UPRV_UNREACHABLE_EXIT;
    };
}

શૂન્ય Locale::Payload::copy(અચલ Payload& other) {
    other.visit([](Payload*) {},
                [](અચલ Nest& nest, Payload* dst) { dst->nest = nest; },
                [](અચલ Heap& heap, Payload* dst) { dst->heap = heap; },
                આ);
}

શૂન્ય Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](અચલ Nest& nest, Payload* dst) { dst->nest = nest; },
        [](અચલ Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        આ);
}

Locale::Payload::~Payload() {
    જો (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(અચલ Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::operator=(અચલ Payload& other) U_LIFETIME_BOUND {
    જો (આ != &other) {
        setToBogus();
        copy(other);
    }
    પાછા *આ;
}

Locale::Payload& Locale::Payload::operator=(Payload&& other) noexcept U_LIFETIME_BOUND {
    જો (આ != &other) {
        setToBogus();
        move(std::move(other));
    }
    પાછા *આ;
}

શૂન્ય Locale::Payload::setToBogus() {
    આ->~Payload();
    type = eBOGUS;
}

template <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    જો constexpr (std::is_same_v<T, Nest>) {
        આ->~Payload();
        ::નવું (&nest) Nest(std::forward<Args>(args)...);
        પાછા nest;
    }
    જો constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::નવું (&heap) Heap(std::forward<Args>(args)...);
        પાછા heap;
    }
}

template <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    પાછા type == eNEST ? &nest : nullptr;
}
template <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    પાછા type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = મૂળભૂત;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, ખોટું);
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


Locale::Locale( અચલ   અક્ષર * newLanguage,
                અચલ   અક્ષર * newCountry,
                અચલ   અક્ષર * newVariant,
                અચલ   અક્ષર * newKeywords)
    : UObject(), payload()
{
    જો( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, ખોટું); /* shortcut */
    }
    નહીંતર
    {
        UErrorCode status = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        જો ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            જો ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                પાછા;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        જો ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            જો ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                પાછા;
            }
        }

        // _Variant
        જો ( newVariant != nullptr )
        {
            // remove leading _'s
            જ્યારે(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            જો ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                પાછા;
            }
            જ્યારે( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        જો ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            જો ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              પાછા;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        જો ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        જો ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        જો ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        જો ( ksize != 0)
        {
            જો (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            નહીંતર {
                togo.append('_', status); /* Variant parsing with a script */
                જો ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        જો (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            પાછા;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), ખોટું);
    }
}

Locale::Locale(અચલ Locale&) = મૂળભૂત;
Locale::Locale(Locale&&) noexcept = મૂળભૂત;

Locale& Locale::operator=(અચલ Locale&) U_LIFETIME_BOUND = મૂળભૂત;
Locale& Locale::operator=(Locale&&) noexcept U_LIFETIME_BOUND = મૂળભૂત;

Locale *
Locale::clone() અચલ {
    પાછા નવું Locale(*આ);
}

તાર્કિક
Locale::operator==( અચલ   Locale& other) અચલ
{
    પાછા uprv_strcmp(other.getName(), getName()) == 0;
}

નામાકાશ {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr અચલ અક્ષર* KNOWN_CANONICALIZED[] = {
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
    જો (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    પાછા સાચું;
}

શૂન્ય U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    માટે (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (શૂન્ય*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    જો (U_FAILURE(status)) {
        પાછા;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

વર્ગ AliasData;

/**
 * A Builder class to build the alias data.
 */
વર્ગ AliasDataBuilder {
જાહેર:
    AliasDataBuilder() {
    }

    // Build the AliasData from resource.
    AliasData* build(UErrorCode &status);

ખાનગી:
    શૂન્ય readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<અચલ અક્ષર*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   શૂન્ય (*checkType)(અચલ અક્ષર* type),
                   શૂન્ય (*checkReplacement)(અચલ UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    શૂન્ય readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<અચલ અક્ષર*>& types,
                           LocalMemory<int32_t>& replacementIndexes,
                           int32_t &length,
                           UErrorCode &status);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    શૂન્ય readScriptAlias(UResourceBundle* alias,
                         UniqueCharStrings* strings,
                         LocalMemory<અચલ અક્ષર*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    શૂન્ય readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<અચલ અક્ષર*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    શૂન્ય readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<અચલ અક્ષર*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    શૂન્ય readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<અચલ અક્ષર*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
વર્ગ AliasData : જાહેર UMemory {
જાહેર:
    સ્થિર અચલ AliasData* singleton(UErrorCode& status) {
        જો (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            પાછા nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        પાછા gSingleton;
    }

    અચલ CharStringMap& languageMap() અચલ U_LIFETIME_BOUND { પાછા language; }
    અચલ CharStringMap& scriptMap() અચલ U_LIFETIME_BOUND { પાછા script; }
    અચલ CharStringMap& territoryMap() અચલ U_LIFETIME_BOUND { પાછા territory; }
    અચલ CharStringMap& variantMap() અચલ U_LIFETIME_BOUND { પાછા variant; }
    અચલ CharStringMap& subdivisionMap() અચલ U_LIFETIME_BOUND { પાછા subdivision; }

    સ્થિર શૂન્ય U_CALLCONV loadData(UErrorCode &status);
    સ્થિર UBool U_CALLCONV cleanup();

    સ્થિર UInitOnce gInitOnce;

ખાનગી:
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

    સ્થિર અચલ AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    friend વર્ગ AliasDataBuilder;
};


અચલ AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    પાછા સાચું;
}

શૂન્ય
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<અચલ અક્ષર*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        શૂન્ય (*checkType)(અચલ અક્ષર* type),
        શૂન્ય (*checkReplacement)(અચલ UChar* replacement),
        UErrorCode &status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    length = ures_getSize(alias);
    અચલ અક્ષર** rawTypes = types.allocateInsteadAndCopy(length);
    જો (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    જો (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    માટે (પૂર્ણાંક i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        અચલ અક્ષર* aliasFrom = ures_get_ચાવી(res.getAlias());
        અચલ UChar* aliasTo =
            ures_get_String_By_ચાવી(res.getAlias(), "replacement", nullptr, &status);
        જો (U_FAILURE(status)) પાછા;

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
શૂન્ય
AliasDataBuilder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<અચલ અક્ષર*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    પાછા readAlias(
        alias, strings, types, replacementIndexes, length,
#જો U_DEBUG
        [](અચલ અક્ષર* type) {
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
#નહીંતર
        [](અચલ અક્ષર*) {},
#endif
        [](અચલ UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
શૂન્ય
AliasDataBuilder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<અચલ અક્ષર*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    પાછા readAlias(
        alias, strings, types, replacementIndexes, length,
#જો U_DEBUG
        [](અચલ અક્ષર* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](અચલ UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#નહીંતર
        [](અચલ અક્ષર*) {},
        [](અચલ UChar*) { },
#endif
        status);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
શૂન્ય
AliasDataBuilder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<અચલ અક્ષર*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    પાછા readAlias(
        alias, strings, types, replacementIndexes, length,
#જો U_DEBUG
        [](અચલ અક્ષર* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#નહીંતર
        [](અચલ અક્ષર*) {},
#endif
        [](અચલ UChar*) { },
        status);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
શૂન્ય
AliasDataBuilder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<અચલ અક્ષર*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    પાછા readAlias(
        alias, strings, types, replacementIndexes, length,
#જો U_DEBUG
        [](અચલ અક્ષર* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](અચલ UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#નહીંતર
        [](અચલ અક્ષર*) {},
        [](અચલ UChar*) { },
#endif
        status);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
શૂન્ય
AliasDataBuilder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<અચલ અક્ષર*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    પાછા readAlias(
        alias, strings, types, replacementIndexes, length,
#જો U_DEBUG
        [](અચલ અક્ષર* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#નહીંતર
        [](અચલ અક્ષર*) {},
#endif
        [](અચલ UChar*) { },
        status);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
શૂન્ય U_CALLCONV
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
    જો (U_FAILURE(status)) { પાછા nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_ચાવી(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_ચાવી(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_ચાવી(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_ચાવી(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_ચાવી(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_ચાવી(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<અચલ અક્ષર*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<અચલ અક્ષર*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<અચલ અક્ષર*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<અચલ અક્ષર*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<અચલ અક્ષર*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    માટે (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    માટે (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    માટે (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    માટે (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    માટે (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }

    // copy hashtables
    auto *data = નવું AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    જો (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    પાછા data;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
વર્ગ AliasReplacer {
જાહેર:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       પાછા 0==uprv_strcmp((અચલ અક્ષર*)e1.pointer,
                                             (અચલ અક્ષર*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    તાર્કિક replace(
        અચલ Locale& locale, CharString& out, UErrorCode& status);

ખાનગી:
    અચલ અક્ષર* language;
    અચલ અક્ષર* script;
    અચલ અક્ષર* region;
    અચલ અક્ષર* extensions;
    UVector variants;

    અચલ AliasData* data;

    અંતઃરેખ તાર્કિક notEmpty(અચલ અક્ષર* str) {
        પાછા str && str[0] != NULL_CHAR;
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
    અંતઃરેખ અચલ અક્ષર* deleteOrReplace(અચલ અક્ષર* input U_LIFETIME_BOUND,
                                       અચલ અક્ષર* type,
                                       અચલ અક્ષર* replacement U_LIFETIME_BOUND) {
        પાછા notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    અંતઃરેખ તાર્કિક same(અચલ અક્ષર* a, અચલ અક્ષર* b) {
        જો (a == nullptr && b == nullptr) {
            પાછા સાચું;
        }
        જો ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          પાછા ખોટું;
        }
        પાછા uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_ચાવી(અચલ અક્ષર* language, અચલ અક્ષર* region,
                            અચલ અક્ષર* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    શૂન્ય parseLanguageReplacement(અચલ અક્ષર* replacement,
                                  અચલ અક્ષર*& replaceLanguage,
                                  અચલ અક્ષર*& replaceScript,
                                  અચલ અક્ષર*& replaceRegion,
                                  અચલ અક્ષર*& replaceVariant,
                                  અચલ અક્ષર*& replaceExtensions,
                                  UVector& toBeFreed,
                                  UErrorCode& status);

    // Replace by using languageAlias.
    તાર્કિક replaceLanguage(તાર્કિક checkLanguage, તાર્કિક checkRegion,
                         તાર્કિક checkVariants, UVector& toBeFreed,
                         UErrorCode& status);

    // Replace by using territoryAlias.
    તાર્કિક replaceTerritory(UVector& toBeFreed, UErrorCode& status);

    // Replace by using scriptAlias.
    તાર્કિક replaceScript(UErrorCode& status);

    // Replace by using variantAlias.
    તાર્કિક replaceVariant(UErrorCode& status);

    // Replace by using subdivisionAlias.
    તાર્કિક replaceSubdivision(StringPiece subdivision,
                            CharString& output, UErrorCode& status);

    // Replace transformed extensions.
    તાર્કિક replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UErrorCode& status);
};

CharString&
AliasReplacer::generate_ચાવી(
        અચલ અક્ષર* language, અચલ અક્ષર* region, અચલ અક્ષર* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    જો (U_FAILURE(status)) { પાછા out; }
    out.append(language, status);
    જો (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    જો (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    પાછા out;
}

શૂન્ય
AliasReplacer::parseLanguageReplacement(
    અચલ અક્ષર* replacement,
    અચલ અક્ષર*& replacedLanguage,
    અચલ અક્ષર*& replacedScript,
    અચલ અક્ષર*& replacedRegion,
    અચલ અક્ષર*& replacedVariant,
    અચલ અક્ષર*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    જો (U_FAILURE(status)) {
        પાછા;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    જો (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        પાછા;
    }
    // We have multiple field so we have to allocate and parse
    auto str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    અક્ષર* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    જો (U_FAILURE(status)) {
        પાછા;
    }
    replacedLanguage = (અચલ અક્ષર*) data;
    અક્ષર* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    અચલ અક્ષર* start = endOfField;
    endOfField = const_cast<અક્ષર*>(uprv_strchr(start, '_'));
    size_t len = 0;
    જો (endOfField == nullptr) {
        len = uprv_strlen(start);
    } નહીંતર {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    જો (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        જો (endOfField == nullptr) {
            પાછા;
        }
        start = endOfField++;
        endOfField = const_cast<અક્ષર*>(uprv_strchr(start, '_'));
        જો (endOfField == nullptr) {
            len = uprv_strlen(start);
        } નહીંતર {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    જો (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        જો (endOfField == nullptr) {
            પાછા;
        }
        start = endOfField++;
        endOfField = const_cast<અક્ષર*>(uprv_strchr(start, '_'));
        જો (endOfField == nullptr) {
            len = uprv_strlen(start);
        } નહીંતર {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    જો (len >= 4) {
        // Got a variant
        replacedVariant = start;
        જો (endOfField == nullptr) {
            પાછા;
        }
        start = endOfField++;
    }
    replacedExtensions = start;
}

તાર્કિક
AliasReplacer::replaceLanguage(
        તાર્કિક checkLanguage, તાર્કિક checkRegion,
        તાર્કિક checkVariants, UVector& toBeFreed, UErrorCode& status)
{
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    જો (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        પાછા ખોટું;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    અચલ અક્ષર* શોધ_Language = checkLanguage ? language : "und";
    અચલ અક્ષર* શોધ_Region = checkRegion ? region : nullptr;
    અચલ અક્ષર* શોધ_Variant = nullptr;
    માટે (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        જો (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            શોધ_Variant = static_cast<અચલ અક્ષર*>(variants.elementAt(variant_index));
        }

        જો (શોધ_Variant != nullptr && uprv_strlen(શોધ_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            શોધ_Variant = nullptr;
        }
        CharString type_ચાવી;
        generate_ચાવી(શોધ_Language, શોધ_Region, શોધ_Variant, type_ચાવી,
                    status);
        જો (U_FAILURE(status)) {
            પાછા ખોટું;
        }
        અચલ અક્ષર *replacement = data->languageMap().get(type_ચાવી.data());
        જો (replacement == nullptr) {
            // Found no replacement data.
            ચાલુ;
        }

        અચલ અક્ષર* replacedLanguage = nullptr;
        અચલ અક્ષર* replacedScript = nullptr;
        અચલ અક્ષર* replacedRegion = nullptr;
        અચલ અક્ષર* replacedVariant = nullptr;
        અચલ અક્ષર* replacedExtensions = nullptr;
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
        replacedRegion = deleteOrReplace(region, શોધ_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            શોધ_Variant, શોધ_Variant, replacedVariant);

        જો (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(શોધ_Variant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            ચાલુ;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        જો (શોધ_Variant != nullptr) {
            જો (notEmpty(replacedVariant)) {
                variants.setElementAt((શૂન્ય*)replacedVariant, variant_index);
            } નહીંતર {
                variants.removeElementAt(variant_index);
            }
        }
        જો (replacedExtensions != nullptr) {
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
        પાછા સાચું;
    }
    // Nothing changed by language alias data.
    પાછા ખોટું;
}

તાર્કિક
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    જો (region == nullptr) {
        // No region to search.
        પાછા ખોટું;
    }
    અચલ અક્ષર *replacement = data->territoryMap().get(region);
    જો (replacement == nullptr) {
        // Found no replacement data for this region.
        પાછા ખોટું;
    }
    અચલ અક્ષર* replacedRegion = replacement;
    અચલ અક્ષર* firstSpace = uprv_strchr(replacement, ' ');
    જો (firstSpace != nullptr) {
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
        અચલ અક્ષર* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        જો (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            અચલ અક્ષર* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            જો (foundInReplacement != nullptr) {
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
        જો (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        જો (U_FAILURE(status)) { પાછા ખોટું; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        જો (U_FAILURE(status)) { પાછા ખોટું; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    પાછા સાચું;
}

તાર્કિક
AliasReplacer::replaceScript(UErrorCode& status)
{
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    જો (script == nullptr) {
        // No script to search.
        પાછા ખોટું;
    }
    અચલ અક્ષર *replacement = data->scriptMap().get(script);
    જો (replacement == nullptr) {
        // Found no replacement data for this script.
        પાછા ખોટું;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    પાછા સાચું;
}

તાર્કિક
AliasReplacer::replaceVariant(UErrorCode& status)
{
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    // Since we may have more than one variant, we need to loop through them.
    માટે (int32_t i = 0; i < variants.size(); i++) {
        અચલ અક્ષર* variant = static_cast<અચલ અક્ષર*>(variants.elementAt(i));
        અચલ અક્ષર *replacement = data->variantMap().get(variant);
        જો (replacement == nullptr) {
            // Found no replacement data for this variant.
            ચાલુ;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        જો (!same(variant, replacement)) {
            variants.setElementAt((શૂન્ય*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            જો (uprv_strcmp(variant, "heploc") == 0) {
                માટે (int32_t j = 0; j < variants.size(); j++) {
                     જો (uprv_strcmp((અચલ અક્ષર*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            પાછા સાચું;
        }
    }
    પાછા ખોટું;
}

તાર્કિક
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    અચલ અક્ષર *replacement = data->subdivisionMap().get(subdivision.data());
    જો (replacement != nullptr) {
        અચલ અક્ષર* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        જો (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            જો (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        પાછા સાચું;
    }
    પાછા ખોટું;
}

તાર્કિક
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    int32_t len = transformedExtensions.length();
    અચલ અક્ષર* str = transformedExtensions.data();
    અચલ અક્ષર* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    જો (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        જો (U_FAILURE(status)) {
            પાછા ખોટું;
        }
        T_CString_toLowerCase(output.data());
    }
    જો (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        જો (U_FAILURE(status)) {
            પાછા ખોટું;
        }
        કરો {
            અચલ અક્ષર* tvalue = uprv_strchr(tkey, '-');
            જો (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                પાછા ખોટું;
            }
            અચલ અક્ષર* nextTKey = ultag_getTKeyStart(tvalue);
            જો (nextTKey != nullptr) {
                *const_cast<અક્ષર*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.ઉમેરો_Element_At((શૂન્ય*)tkey, tfields.size(), status);
            જો (U_FAILURE(status)) {
                પાછા ખોટું;
            }
            tkey = nextTKey;
        } જ્યારે (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            પાછા uprv_strcmp((અચલ અક્ષર*)e1.pointer, (અચલ અક્ષર*)e2.pointer);
        }, status);
        માટે (int32_t i = 0; i < tfields.size(); i++) {
             જો (output.length() > 0) {
                 output.append('-', status);
             }
             અચલ અક્ષર* tfield = static_cast<અચલ અક્ષર*>(tfields.elementAt(i));
             અચલ અક્ષર* tvalue = uprv_strchr(tfield, '-');
             જો (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 પાછા ખોટું;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<અક્ષર*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    પાછા સાચું;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    જો (U_FAILURE(status)) { પાછા out; }
    out.append(language, status);
    જો (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    જો (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    જો (variants.size() > 0) {
        જો (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            પાછા uprv_strcmp((અચલ અક્ષર*)e1.pointer, (અચલ અક્ષર*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        માટે (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<અચલ અક્ષર*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    જો (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    પાછા out;
}

તાર્કિક
AliasReplacer::replace(અચલ Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    U_ASSERT(data != nullptr);
    out.clear();
    language = locale.getLanguage();
    જો (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    જો (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    જો (!notEmpty(region)) {
        region = nullptr;
    }
    અચલ અક્ષર* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    જો (!variantsBuff.isEmpty()) {
        જો (U_FAILURE(status)) { પાછા ખોટું; }
        અક્ષર* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        અક્ષર* end;
        જ્યારે ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            જો (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        જો (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    જો (U_FAILURE(status)) { પાછા ખોટું; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        પાછા uprv_strcmp((અચલ અક્ષર*)e1.pointer, (અચલ અક્ષર*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    અચલ int32_t maxChanged = 4 + variants.size();
    (શૂન્ય)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](શૂન્ય *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    જ્યારે (U_SUCCESS(status)) {
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
        જો (    replaceLanguage(સાચું, સાચું,  સાચું,  stringsToBeFreed, status) ||
                replaceLanguage(સાચું, સાચું,  ખોટું, stringsToBeFreed, status) ||
                replaceLanguage(સાચું, ખોટું, સાચું,  stringsToBeFreed, status) ||
                replaceLanguage(સાચું, ખોટું, ખોટું, stringsToBeFreed, status) ||
                replaceLanguage(ખોટું,ખોટું, સાચું,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            ચાલુ;
        }
        // Nothing changed. Break out.
        તોડો;
    }  // while(1)

    જો (U_FAILURE(status)) { પાછા ખોટું; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    અચલ અક્ષર* extensionsStr = locale_getKeywordsStart(locale.getName());
    જો (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        પાછા ખોટું;
    }
    outputToString(out, status);
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    જો (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        જો (U_SUCCESS(status) && !iter.isNull()) {
            અચલ અક્ષર* ચાવી;
            જ્યારે ((ચાવી = iter->next(nullptr, status)) != nullptr) {
                જો (uprv_strcmp("sd", ચાવી) == 0 || uprv_strcmp("rg", ચાવી) == 0 ||
                        uprv_strcmp("t", ચાવી) == 0) {
                    auto value = locale.get_Keyword_કિંમત<CharString>(ચાવી, status);
                    જો (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        ચાલુ;
                    }
                    CharString replacement;
                    જો (uprv_strlen(ચાવી) == 2) {
                        જો (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.set_Keyword_કિંમત(ચાવી, replacement.data(), status);
                        }
                    } નહીંતર {
                        U_ASSERT(uprv_strcmp(ચાવી, "t") == 0);
                        જો (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.set_Keyword_કિંમત(ચાવી, replacement.data(), status);
                        }
                    }
                    જો (U_FAILURE(status)) {
                        પાછા ખોટું;
                    }
                }
            }
        }
        જો (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    // If the tag is not changed, return.
    જો (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        પાછા ખોટું;
    }
    પાછા સાચું;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
તાર્કિક
canonicalizeLocale(અચલ Locale& locale, CharString& out, UErrorCode& status)
{
    જો (U_FAILURE(status)) { પાછા ખોટું; }
    AliasReplacer replacer(status);
    પાછા replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
તાર્કિક
isKnownCanonicalizedLocale(અચલ અક્ષર* locale, UErrorCode& status)
{
    જો (U_FAILURE(status)) { પાછા ખોટું; }

    જો (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        પાછા સાચું;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    પાછા uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT અચલ અક્ષર* અચલ*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    પાછા KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT તાર્કિક
ulocimp_isCanonicalizedLocaleForTest(અચલ અક્ષર* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    પાછા !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(અચલ અક્ષર* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    પાછા localeID == nullptr ? *આ = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    કરો {
        અક્ષર *separator;
        અક્ષર *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        અચલ auto parse = [canonicalize](std::string_view localeID,
                                          અક્ષર* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            પાછા ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    જો (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } નહીંતર {
                        ulocimp_getName(localeID, sink, status);
                    }
                },
                status);
        };

        // "canonicalize" the locale ID to ICU/Java format
        અક્ષર* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, કદ Nest::baseName, err);

        FixedString fullNameBuffer;
        જો (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(કદ Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            જો (!fullNameBuffer.reserve(length + 1)) {
                તોડો; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        જો(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            તોડો;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        અક્ષર* at = uprv_strchr(fullName, '@');
        જ્યારે ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        અક્ષર* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        જો (separator!=nullptr || sep2!=nullptr) {
            જો (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } નહીંતર {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        તાર્કિક hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        જો (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            તોડો; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        જો (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        જો (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        જો (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } નહીંતર જો (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        જો (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } નહીંતર જો (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        જો (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } નહીંતર {
            જો (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                જો (fullNameBuffer.isEmpty()) {
                    તોડો; // error: out of memory
                }
                જો (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                જો (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                જો (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            જો (isBogus()) {
                તોડો; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            જો (hasKeywords) {
                જો (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    જો (heap.ptr->baseName.isEmpty()) {
                        તોડો; // error: out of memory
                    }
                }
            }
        }

        જો (canonicalize) {
            જો (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                જો (canonicalizeLocale(*આ, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), ખોટું);
                }
                જો (U_FAILURE(err)) {
                    તોડો;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        પાછા *આ;
    } જ્યારે(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    પાછા *આ;
}

int32_t
Locale::hashCode() અચલ
{
    પાછા ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

શૂન્ય
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

અચલ Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        જો (gDefaultLocale != nullptr) {
            પાછા *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    પાછા *locale_set_default_internal(nullptr, status);
}



શૂન્ય U_EXPORT2
Locale::setDefault( અચલ   Locale&     newLocale,
                            UErrorCode&  status)
{
    જો (U_FAILURE(status)) {
        પાછા;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    અચલ અક્ષર *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

શૂન્ય
Locale::addLikelySubtags(UErrorCode& status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    જો (U_FAILURE(status)) {
        જો (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        પાછા;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/ખોટું);
    જો (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

શૂન્ય
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(ખોટું, status);
}
શૂન્ય
Locale::minimizeSubtags(તાર્કિક favorScript, UErrorCode& status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    જો (U_FAILURE(status)) {
        જો (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        પાછા;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/ખોટું);
    જો (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

શૂન્ય
Locale::canonicalize(UErrorCode& status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    જો (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    CharString uncanonicalized(getName(), status);
    જો (U_FAILURE(status)) {
        જો (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        પાછા;
    }
    init(uncanonicalized.data(), /*canonicalize=*/સાચું);
    જો (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    જો (U_FAILURE(status)) {
        પાછા result;
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

    જો (U_FAILURE(status)) {
        પાછા result;
    }

    જો (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા result;
    }

    result.init(localeID.data(), /*canonicalize=*/ખોટું);
    જો (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    પાછા result;
}

શૂન્ય
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) અચલ
{
    જો (U_FAILURE(status)) {
        પાછા;
    }

    જો (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/ખોટું, status);
}

Locale U_EXPORT2
Locale::createFromName (અચલ અક્ષર *name)
{
    જો (name) {
        Locale l("");
        l.init(name, ખોટું);
        પાછા l;
    }
    નહીંતર {
        પાછા getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, ખોટું);
    પાછા loc;
}

Locale U_EXPORT2
Locale::createCanonical(અચલ અક્ષર* name) {
    Locale loc("");
    loc.init(name, સાચું);
    પાછા loc;
}

અચલ અક્ષર *
Locale::getISO3Language() અચલ
{
    પાછા uloc_getISO3Language(getName());
}


અચલ અક્ષર *
Locale::getISO3Country() અચલ
{
    પાછા uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() અચલ
{
    પાછા uloc_getLCID(getName());
}

અચલ અક્ષર* અચલ* U_EXPORT2 Locale::getISOCountries()
{
    પાછા uloc_getISOCountries();
}

અચલ અક્ષર* અચલ* U_EXPORT2 Locale::getISOLanguages()
{
    પાછા uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
શૂન્ય Locale::setFromPOSIXID(અચલ અક્ષર *posixID)
{
    init(posixID, સાચું);
}

અચલ Locale & U_EXPORT2
Locale::get_મૂળ()
{
    પાછા getLocale(e_મૂળ);
}

અચલ Locale & U_EXPORT2
Locale::getEnglish()
{
    પાછા getLocale(eENGLISH);
}

અચલ Locale & U_EXPORT2
Locale::getFrench()
{
    પાછા getLocale(eFRENCH);
}

અચલ Locale & U_EXPORT2
Locale::getGerman()
{
    પાછા getLocale(eGERMAN);
}

અચલ Locale & U_EXPORT2
Locale::getItalian()
{
    પાછા getLocale(eITALIAN);
}

અચલ Locale & U_EXPORT2
Locale::getJapanese()
{
    પાછા getLocale(eJAPANESE);
}

અચલ Locale & U_EXPORT2
Locale::getKorean()
{
    પાછા getLocale(eKOREAN);
}

અચલ Locale & U_EXPORT2
Locale::getChinese()
{
    પાછા getLocale(eCHINESE);
}

અચલ Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    પાછા getLocale(eCHINA);
}

અચલ Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    પાછા getLocale(eTAIWAN);
}


અચલ Locale & U_EXPORT2
Locale::getFrance()
{
    પાછા getLocale(eFRANCE);
}

અચલ Locale & U_EXPORT2
Locale::getGermany()
{
    પાછા getLocale(eGERMANY);
}

અચલ Locale & U_EXPORT2
Locale::getItaly()
{
    પાછા getLocale(eITALY);
}

અચલ Locale & U_EXPORT2
Locale::getJapan()
{
    પાછા getLocale(eJAPAN);
}

અચલ Locale & U_EXPORT2
Locale::getKorea()
{
    પાછા getLocale(eKOREA);
}

અચલ Locale & U_EXPORT2
Locale::getChina()
{
    પાછા getLocale(eCHINA);
}

અચલ Locale & U_EXPORT2
Locale::getPRC()
{
    પાછા getLocale(eCHINA);
}

અચલ Locale & U_EXPORT2
Locale::getTaiwan()
{
    પાછા getLocale(eTAIWAN);
}

અચલ Locale & U_EXPORT2
Locale::getUK()
{
    પાછા getLocale(eUK);
}

અચલ Locale & U_EXPORT2
Locale::getUS()
{
    પાછા getLocale(eUS);
}

અચલ Locale & U_EXPORT2
Locale::getCanada()
{
    પાછા getLocale(eCANADA);
}

અચલ Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    પાછા getLocale(eCANADA_FRENCH);
}

અચલ Locale &
Locale::getLocale(પૂર્ણાંક locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    જો (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    પાછા localeCache[locid]; /*operating on nullptr*/
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
    પાછા gLocaleCache;
}

વર્ગ KeywordEnumeration : જાહેર StringEnumeration {
સુરક્ષિત:
    FixedString keywords;
ખાનગી:
    int32_t length;
    અચલ અક્ષર *current;
    સ્થિર અચલ અક્ષર fgClassID;

જાહેર:
    સ્થિર UClassID U_EXPORT2 getStaticClassID() { પાછા (UClassID)&fgClassID; }
    આભાસી UClassID getDynamicClassID() અચલ override { પાછા getStaticClassID(); }
જાહેર:
    KeywordEnumeration(અચલ અક્ષર *ચાવીઓ, int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        જો(U_SUCCESS(status) && keywordLen != 0) {
            જો(ચાવીઓ == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } નહીંતર {
                keywords = {ચાવીઓ, static_cast<std::string_view::size_type>(length)};
                જો (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } નહીંતર {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    આભાસી ~KeywordEnumeration();

    આભાસી StringEnumeration * clone() અચલ override
    {
        UErrorCode status = U_ZERO_ERROR;
        પાછા નવું KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    આભાસી int32_t count(UErrorCode& status) અચલ override {
        જો (U_FAILURE(status)) { પાછા 0; }
        અચલ અક્ષર *kw = keywords.data();
        int32_t result = 0;
        જ્યારે(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        પાછા result;
    }

    આભાસી અચલ અક્ષર* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        અચલ અક્ષર* result;
        int32_t len;
        જો(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            જો(resultLength != nullptr) {
                *resultLength = len;
            }
        } નહીંતર {
            જો(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        પાછા result;
    }

    આભાસી અચલ UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        જો (U_FAILURE(status)) { પાછા nullptr; }
        int32_t resultLength = 0;
        અચલ અક્ષર *s = next(&resultLength, status);
        પાછા setChars(s, resultLength, status);
    }

    આભાસી શૂન્ય reset(UErrorCode& status) override {
        જો (U_FAILURE(status)) { પાછા; }
        current = keywords.data();
    }
};

અચલ અક્ષર KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = મૂળભૂત;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
વર્ગ UnicodeKeywordEnumeration : જાહેર KeywordEnumeration {
જાહેર:
    using KeywordEnumeration::KeywordEnumeration;
    આભાસી ~UnicodeKeywordEnumeration();

    આભાસી અચલ અક્ષર* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        અચલ અક્ષર* legacy_ચાવી = KeywordEnumeration::next(nullptr, status);
        જ્યારે (U_SUCCESS(status) && legacy_ચાવી != nullptr) {
            અચલ અક્ષર* ચાવી = uloc_to_Unicode_Locale_ચાવી(legacy_ચાવી);
            જો (ચાવી != nullptr) {
                જો (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(ચાવી));
                }
                પાછા ચાવી;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_ચાવી = KeywordEnumeration::next(nullptr, status);
        }
        જો (resultLength != nullptr) *resultLength = 0;
        પાછા nullptr;
    }
    આભાસી int32_t count(UErrorCode& status) અચલ override {
        જો (U_FAILURE(status)) { પાછા 0; }
        અચલ અક્ષર *kw = keywords.data();
        int32_t result = 0;
        જ્યારે(*kw) {
            જો (uloc_to_Unicode_Locale_ચાવી(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        પાછા result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = મૂળભૂત;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) અચલ
{
    StringEnumeration *result = nullptr;

    જો (U_FAILURE(status)) {
        પાછા result;
    }

    અચલ અક્ષર* variantStart = uprv_strchr(getName(), '@');
    અચલ અક્ષર* assignment = uprv_strchr(getName(), '=');
    જો(variantStart) {
        જો(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', ખોટું, status);
            જો (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = નવું KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                જો (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } નહીંતર {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    પાછા result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) અચલ
{
    StringEnumeration *result = nullptr;

    જો (U_FAILURE(status)) {
        પાછા result;
    }

    અચલ અક્ષર* variantStart = uprv_strchr(getName(), '@');
    અચલ અક્ષર* assignment = uprv_strchr(getName(), '=');
    જો(variantStart) {
        જો(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', ખોટું, status);
            જો (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = નવું UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                જો (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } નહીંતર {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    પાછા result;
}

int32_t
Locale::get_Keyword_કિંમત(અચલ અક્ષર* keywordName, અક્ષર *buffer, int32_t bufLen, UErrorCode &status) અચલ
{
    પાછા uloc_get_Keyword_કિંમત(getName(), keywordName, buffer, bufLen, &status);
}

શૂન્ય
Locale::get_Keyword_કિંમત(StringPiece keywordName, ByteSink& sink, UErrorCode& status) અચલ {
    જો (U_FAILURE(status)) {
        પાછા;
    }

    જો (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    ulocimp_get_Keyword_કિંમત(getName(), keywordName, sink, status);
}

શૂન્ય
Locale::get_Unicode_Keyword_કિંમત(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) અચલ {
    જો (U_FAILURE(status)) {
        પાછા;
    }

    std::optional<std::string_view> legacy_ચાવી = ulocimp_to_Legacy_ચાવી_With_Fallback(keywordName);
    જો (!legacy_ચાવી.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    auto legacy_કિંમત = get_Keyword_કિંમત<CharString>(*legacy_ચાવી, status);

    જો (U_FAILURE(status)) {
        પાછા;
    }

    std::optional<std::string_view> unicode_કિંમત =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_કિંમત.toStringPiece());
    જો (!unicode_કિંમત.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    sink.Append(unicode_કિંમત->data(), static_cast<int32_t>(unicode_કિંમત->size()));
}

શૂન્ય
Locale::set_Keyword_કિંમત(StringPiece keywordName,
                        StringPiece keyword_કિંમત,
                        UErrorCode& status) {
    જો (U_FAILURE(status)) { પાછા; }
    જો (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    જો (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_set_Keyword_કિંમત(keywordName, keyword_કિંમત, localeID, status);
    જો (U_FAILURE(status)) {
        જો (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        પાછા;
    }

    અચલ અક્ષર* at = locale_getKeywordsStart(localeID.toStringPiece());
    તાર્કિક hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    જો (!hasKeywords) {
        જો (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            જો (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < કદ Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, કદ Nest::baseName, status);
            } નહીંતર {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                જો (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    પાછા;
                }
            }
        }
    } નહીંતર {
        Heap* heap = nullptr;
        જો (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            જો (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                પાછા;
            }
        } નહીંતર {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        જો (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            પાછા;
        }

        જો (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            જો (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                જો (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    પાછા;
                }
            }
        }
    }
}

શૂન્ય
Locale::set_Unicode_Keyword_કિંમત(StringPiece keywordName,
                               StringPiece keyword_કિંમત,
                               UErrorCode& status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }

    std::optional<std::string_view> legacy_ચાવી = ulocimp_to_Legacy_ચાવી_With_Fallback(keywordName);
    જો (!legacy_ચાવી.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    std::string_view value;

    જો (!keyword_કિંમત.empty()) {
        std::optional<std::string_view> legacy_કિંમત =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_કિંમત);
        જો (!legacy_કિંમત.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા;
        }
        value = *legacy_કિંમત;
    }

    set_Keyword_કિંમત(*legacy_ચાવી, value, status);
}

અચલ અક્ષર*
Locale::getCountry() અચલ U_LIFETIME_BOUND {
    પાછા getField<&Nest::getRegion, &Heap::getRegion>();
}

અચલ અક્ષર*
Locale::getLanguage() અચલ U_LIFETIME_BOUND {
    પાછા getField<&Nest::getLanguage, &Heap::getLanguage>();
}

અચલ અક્ષર*
Locale::getScript() અચલ U_LIFETIME_BOUND {
    પાછા getField<&Nest::getScript, &Heap::getScript>();
}

અચલ અક્ષર*
Locale::getVariant() અચલ U_LIFETIME_BOUND {
    પાછા getField<&Nest::getVariant, &Heap::getVariant>();
}

અચલ અક્ષર*
Locale::getName() અચલ U_LIFETIME_BOUND {
    પાછા getField<&Nest::getBaseName, &Heap::getFullName>();
}

અચલ અક્ષર*
Locale::getBaseName() અચલ U_LIFETIME_BOUND {
    પાછા getField<&Nest::getBaseName, &Heap::getBaseName>();
}

template <અચલ અક્ષર* (Locale::Nest::*અચલ NEST)() અચલ,
          અચલ અક્ષર* (Locale::Heap::*અચલ HEAP)() અચલ>
અચલ અક્ષર* Locale::getField() અચલ U_LIFETIME_BOUND {
    પાછા payload.visit([] { પાછા ""; },
                         [](અચલ Nest& nest) { પાછા (nest.*NEST)(); },
                         [](અચલ Heap& heap) { પાછા (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = મૂળભૂત;

//eof
U_NAMESPACE_END
