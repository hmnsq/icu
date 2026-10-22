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

#அந்தர்பாவ <cstddef>
#அந்தர்பாவ <optional>
#அந்தர்பாவ <string_view>
#அந்தர்பாவ <type_traits>
#அந்தர்பாவ <utility>

#அந்தர்பாவ "unicode/bytestream.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/localebuilder.h"
#அந்தர்பாவ "unicode/localpointer.h"
#அந்தர்பாவ "unicode/strenum.h"
#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "unicode/uloc.h"
#அந்தர்பாவ "unicode/ures.h"

#அந்தர்பாவ "bytesinkutil.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "charstrmap.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "fixedstring.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "putilimp.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "ulocimp.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "uniquecharstr.h"
#அந்தர்பாவ "ustr_imp.h"
#அந்தர்பாவ "uvector.h"

#எனில் UPRV_HAS_FEATURE(memory_sanitizer)
#அந்தர்பாவ <sanitizer/msan_interface.h>
#endif

U_NAMESPACE_BEGIN

நிலையான Locale   *gLocaleCache = nullptr;
நிலையான UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
நிலையான UMutex gDefaultLocaleMutex;
நிலையான UHashtable *gDefaultLocalesHashT = nullptr;
நிலையான Locale *gDefaultLocale = nullptr;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#define ULOC_STRING_LIMIT 357913941

U_NAMESPACE_END

வகைவரை எண்ணிக்கை ELocalePos {
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
    e_வேர்,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

நாமாகாஶ {

//
// Deleter function for Locales owned by the default Locale hash table/
//
வெறுமை U_CALLCONV
deleteLocale(வெறுமை *obj) {
    delete static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    delete [] gLocaleCache;
    gLocaleCache = nullptr;
    gLocaleCacheInitOnce.reset();

    எனில் (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    திரும்பு உண்மை;
}

வெறுமை U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = புதிய Locale[static_cast<முழு>(eMAX_LOCALES)];
    எனில் (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_வேர்]          = Locale("");
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

Locale *locale_set_default_internal(மாறிலி எழுத்து *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = பொய்;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    எனில் (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = உண்மை; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    எனில் (U_FAILURE(status)) {
        திரும்பு gDefaultLocale;
    }

    எனில் (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        எனில் (U_FAILURE(status)) {
            திரும்பு gDefaultLocale;
        }
        uhash_set_மதிப்பு_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    எனில் (newDefault == nullptr) {
        newDefault = புதிய Locale(Locale::eBOGUS);
        எனில் (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), பொய்);
        uhash_put(gDefaultLocalesHashT, const_cast<எழுத்து*>(newDefault->getName()), newDefault, &status);
        எனில் (U_FAILURE(status)) {
            திரும்பு gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    திரும்பு gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC வெறுமை
locale_set_default(மாறிலி எழுத்து *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC மாறிலி எழுத்து *
locale_get_default()
{
    U_NAMESPACE_USE
    திரும்பு Locale::getDefault().getName();
}

நாமாகாஶ {

template <auto FIELD, typename T>
வெறுமை copyToArray(std::string_view sv, T* that) {
    auto& field = that->*FIELD;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    எனில் (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.data(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

வெறுமை Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, இது);
    copyToArray<&Nest::script>(script, இது);
    copyToArray<&Nest::region>(region, இது);
    இது->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(Heap, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(Heap, region));
    U_ASSERT(இது == reinterpret_cast<Nest*>(&heap));
#எனில் UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, அளவு(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, இது);
    copyToArray<&Nest::region>(heap.region, இது);
    இது->variantBegin = variantBegin;
    *இது->baseName = '\0';
}

கட்டமைப்பு Locale::Heap::Alloc : பொது UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    மாறிலி எழுத்து* getVariant() மாறிலி U_LIFETIME_BOUND {
        திரும்பு variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    மாறிலி எழுத்து* getFullName() மாறிலி U_LIFETIME_BOUND { திரும்பு fullName.data(); }
    மாறிலி எழுத்து* getBaseName() மாறிலி U_LIFETIME_BOUND {
        எனில் (baseName.isEmpty()) {
            எனில் (மாறிலி எழுத்து* name = fullName.data(); *name != '@') {
                திரும்பு name;
            }
        }
        திரும்பு baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(மாறிலி Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        எனில் (U_SUCCESS(status)) {
            எனில் (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                எனில் (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } இல்லையெனில் {
                    எனில் (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        எனில் (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = delete;

    ~Alloc() = இயல்பு;
};

மாறிலி எழுத்து* Locale::Heap::getVariant() மாறிலி U_LIFETIME_BOUND { திரும்பு ptr->getVariant(); }
மாறிலி எழுத்து* Locale::Heap::getFullName() மாறிலி U_LIFETIME_BOUND { திரும்பு ptr->getFullName(); }
மாறிலி எழுத்து* Locale::Heap::getBaseName() மாறிலி U_LIFETIME_BOUND { திரும்பு ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = புதிய Alloc(variantBegin);
    எனில் (ptr == nullptr) {
        type = eBOGUS;
    } இல்லையெனில் {
        type = eHEAP;
        copyToArray<&Heap::language>(language, இது);
        copyToArray<&Heap::script>(script, இது);
        copyToArray<&Heap::region>(region, இது);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::operator=(மாறிலி Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = புதிய Alloc(*other.ptr, status);
    எனில் (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } இல்லையெனில் {
        type = eHEAP;
        uprv_memcpy(language, other.language, அளவு language);
        uprv_memcpy(script, other.script, அளவு script);
        uprv_memcpy(region, other.region, அளவு region);
    }
    திரும்பு *இது;
}

Locale::Heap& Locale::Heap::operator=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, அளவு language);
    uprv_memcpy(script, other.script, அளவு script);
    uprv_memcpy(region, other.region, அளவு region);
    திரும்பு *இது;
}

template <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
auto Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) மாறிலி {
    தேர்வு (type) {
        நிலை eBOGUS:
            திரும்பு bogusFn(args...);
        நிலை eNEST:
            திரும்பு nestFn(nest, args...);
        நிலை eHEAP:
            திரும்பு heapFn(heap, args...);
        இயல்பு:
            UPRV_UNREACHABLE_EXIT;
    };
}

வெறுமை Locale::Payload::copy(மாறிலி Payload& other) {
    other.visit([](Payload*) {},
                [](மாறிலி Nest& nest, Payload* dst) { dst->nest = nest; },
                [](மாறிலி Heap& heap, Payload* dst) { dst->heap = heap; },
                இது);
}

வெறுமை Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](மாறிலி Nest& nest, Payload* dst) { dst->nest = nest; },
        [](மாறிலி Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        இது);
}

Locale::Payload::~Payload() {
    எனில் (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(மாறிலி Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::operator=(மாறிலி Payload& other) U_LIFETIME_BOUND {
    எனில் (இது != &other) {
        setToBogus();
        copy(other);
    }
    திரும்பு *இது;
}

Locale::Payload& Locale::Payload::operator=(Payload&& other) noexcept U_LIFETIME_BOUND {
    எனில் (இது != &other) {
        setToBogus();
        move(std::move(other));
    }
    திரும்பு *இது;
}

வெறுமை Locale::Payload::setToBogus() {
    இது->~Payload();
    type = eBOGUS;
}

template <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    எனில் constexpr (std::is_same_v<T, Nest>) {
        இது->~Payload();
        ::புதிய (&nest) Nest(std::forward<Args>(args)...);
        திரும்பு nest;
    }
    எனில் constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::புதிய (&heap) Heap(std::forward<Args>(args)...);
        திரும்பு heap;
    }
}

template <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    திரும்பு type == eNEST ? &nest : nullptr;
}
template <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    திரும்பு type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = இயல்பு;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, பொய்);
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


Locale::Locale( மாறிலி   எழுத்து * newLanguage,
                மாறிலி   எழுத்து * newCountry,
                மாறிலி   எழுத்து * newVariant,
                மாறிலி   எழுத்து * newKeywords)
    : UObject(), payload()
{
    எனில்( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, பொய்); /* shortcut */
    }
    இல்லையெனில்
    {
        UErrorCode status = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        எனில் ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            எனில் ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                திரும்பு;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        எனில் ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            எனில் ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                திரும்பு;
            }
        }

        // _Variant
        எனில் ( newVariant != nullptr )
        {
            // remove leading _'s
            போது(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            எனில் ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                திரும்பு;
            }
            போது( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        எனில் ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            எனில் ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              திரும்பு;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        எனில் ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        எனில் ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        எனில் ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        எனில் ( ksize != 0)
        {
            எனில் (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            இல்லையெனில் {
                togo.append('_', status); /* Variant parsing with a script */
                எனில் ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        எனில் (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            திரும்பு;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), பொய்);
    }
}

Locale::Locale(மாறிலி Locale&) = இயல்பு;
Locale::Locale(Locale&&) noexcept = இயல்பு;

Locale& Locale::operator=(மாறிலி Locale&) U_LIFETIME_BOUND = இயல்பு;
Locale& Locale::operator=(Locale&&) noexcept U_LIFETIME_BOUND = இயல்பு;

Locale *
Locale::clone() மாறிலி {
    திரும்பு புதிய Locale(*இது);
}

தருக்க
Locale::operator==( மாறிலி   Locale& other) மாறிலி
{
    திரும்பு uprv_strcmp(other.getName(), getName()) == 0;
}

நாமாகாஶ {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr மாறிலி எழுத்து* KNOWN_CANONICALIZED[] = {
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
    எனில் (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    திரும்பு உண்மை;
}

வெறுமை U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    க்கு (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (வெறுமை*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

வகுப்பு AliasData;

/**
 * A Builder class to build the alias data.
 */
வகுப்பு AliasDataBuilder {
பொது:
    AliasDataBuilder() {
    }

    // Build the AliasData from resource.
    AliasData* build(UErrorCode &status);

தனிப்பட்ட:
    வெறுமை readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<மாறிலி எழுத்து*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   வெறுமை (*checkType)(மாறிலி எழுத்து* type),
                   வெறுமை (*checkReplacement)(மாறிலி UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    வெறுமை readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<மாறிலி எழுத்து*>& types,
                           LocalMemory<int32_t>& replacementIndexes,
                           int32_t &length,
                           UErrorCode &status);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    வெறுமை readScriptAlias(UResourceBundle* alias,
                         UniqueCharStrings* strings,
                         LocalMemory<மாறிலி எழுத்து*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    வெறுமை readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<மாறிலி எழுத்து*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    வெறுமை readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<மாறிலி எழுத்து*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    வெறுமை readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<மாறிலி எழுத்து*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
வகுப்பு AliasData : பொது UMemory {
பொது:
    நிலையான மாறிலி AliasData* singleton(UErrorCode& status) {
        எனில் (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            திரும்பு nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        திரும்பு gSingleton;
    }

    மாறிலி CharStringMap& languageMap() மாறிலி U_LIFETIME_BOUND { திரும்பு language; }
    மாறிலி CharStringMap& scriptMap() மாறிலி U_LIFETIME_BOUND { திரும்பு script; }
    மாறிலி CharStringMap& territoryMap() மாறிலி U_LIFETIME_BOUND { திரும்பு territory; }
    மாறிலி CharStringMap& variantMap() மாறிலி U_LIFETIME_BOUND { திரும்பு variant; }
    மாறிலி CharStringMap& subdivisionMap() மாறிலி U_LIFETIME_BOUND { திரும்பு subdivision; }

    நிலையான வெறுமை U_CALLCONV loadData(UErrorCode &status);
    நிலையான UBool U_CALLCONV cleanup();

    நிலையான UInitOnce gInitOnce;

தனிப்பட்ட:
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

    நிலையான மாறிலி AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    friend வகுப்பு AliasDataBuilder;
};


மாறிலி AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    திரும்பு உண்மை;
}

வெறுமை
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<மாறிலி எழுத்து*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        வெறுமை (*checkType)(மாறிலி எழுத்து* type),
        வெறுமை (*checkReplacement)(மாறிலி UChar* replacement),
        UErrorCode &status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    length = ures_getSize(alias);
    மாறிலி எழுத்து** rawTypes = types.allocateInsteadAndCopy(length);
    எனில் (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    எனில் (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    க்கு (முழு i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        மாறிலி எழுத்து* aliasFrom = ures_get_சாவி(res.getAlias());
        மாறிலி UChar* aliasTo =
            ures_get_String_By_சாவி(res.getAlias(), "replacement", nullptr, &status);
        எனில் (U_FAILURE(status)) திரும்பு;

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
வெறுமை
AliasDataBuilder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<மாறிலி எழுத்து*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    திரும்பு readAlias(
        alias, strings, types, replacementIndexes, length,
#எனில் U_DEBUG
        [](மாறிலி எழுத்து* type) {
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
#இல்லையெனில்
        [](மாறிலி எழுத்து*) {},
#endif
        [](மாறிலி UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
வெறுமை
AliasDataBuilder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<மாறிலி எழுத்து*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    திரும்பு readAlias(
        alias, strings, types, replacementIndexes, length,
#எனில் U_DEBUG
        [](மாறிலி எழுத்து* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](மாறிலி UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#இல்லையெனில்
        [](மாறிலி எழுத்து*) {},
        [](மாறிலி UChar*) { },
#endif
        status);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
வெறுமை
AliasDataBuilder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<மாறிலி எழுத்து*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    திரும்பு readAlias(
        alias, strings, types, replacementIndexes, length,
#எனில் U_DEBUG
        [](மாறிலி எழுத்து* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#இல்லையெனில்
        [](மாறிலி எழுத்து*) {},
#endif
        [](மாறிலி UChar*) { },
        status);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
வெறுமை
AliasDataBuilder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<மாறிலி எழுத்து*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    திரும்பு readAlias(
        alias, strings, types, replacementIndexes, length,
#எனில் U_DEBUG
        [](மாறிலி எழுத்து* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](மாறிலி UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#இல்லையெனில்
        [](மாறிலி எழுத்து*) {},
        [](மாறிலி UChar*) { },
#endif
        status);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
வெறுமை
AliasDataBuilder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<மாறிலி எழுத்து*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    திரும்பு readAlias(
        alias, strings, types, replacementIndexes, length,
#எனில் U_DEBUG
        [](மாறிலி எழுத்து* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#இல்லையெனில்
        [](மாறிலி எழுத்து*) {},
#endif
        [](மாறிலி UChar*) { },
        status);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
வெறுமை U_CALLCONV
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
    எனில் (U_FAILURE(status)) { திரும்பு nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_சாவி(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_சாவி(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_சாவி(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_சாவி(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_சாவி(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_சாவி(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<மாறிலி எழுத்து*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<மாறிலி எழுத்து*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<மாறிலி எழுத்து*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<மாறிலி எழுத்து*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<மாறிலி எழுத்து*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    க்கு (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    க்கு (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    க்கு (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    க்கு (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    க்கு (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }

    // copy hashtables
    auto *data = புதிய AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    எனில் (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    திரும்பு data;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
வகுப்பு AliasReplacer {
பொது:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       திரும்பு 0==uprv_strcmp((மாறிலி எழுத்து*)e1.pointer,
                                             (மாறிலி எழுத்து*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    தருக்க replace(
        மாறிலி Locale& locale, CharString& out, UErrorCode& status);

தனிப்பட்ட:
    மாறிலி எழுத்து* language;
    மாறிலி எழுத்து* script;
    மாறிலி எழுத்து* region;
    மாறிலி எழுத்து* extensions;
    UVector variants;

    மாறிலி AliasData* data;

    உள்வரி தருக்க notEmpty(மாறிலி எழுத்து* str) {
        திரும்பு str && str[0] != NULL_CHAR;
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
    உள்வரி மாறிலி எழுத்து* deleteOrReplace(மாறிலி எழுத்து* input U_LIFETIME_BOUND,
                                       மாறிலி எழுத்து* type,
                                       மாறிலி எழுத்து* replacement U_LIFETIME_BOUND) {
        திரும்பு notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    உள்வரி தருக்க same(மாறிலி எழுத்து* a, மாறிலி எழுத்து* b) {
        எனில் (a == nullptr && b == nullptr) {
            திரும்பு உண்மை;
        }
        எனில் ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          திரும்பு பொய்;
        }
        திரும்பு uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_சாவி(மாறிலி எழுத்து* language, மாறிலி எழுத்து* region,
                            மாறிலி எழுத்து* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    வெறுமை parseLanguageReplacement(மாறிலி எழுத்து* replacement,
                                  மாறிலி எழுத்து*& replaceLanguage,
                                  மாறிலி எழுத்து*& replaceScript,
                                  மாறிலி எழுத்து*& replaceRegion,
                                  மாறிலி எழுத்து*& replaceVariant,
                                  மாறிலி எழுத்து*& replaceExtensions,
                                  UVector& toBeFreed,
                                  UErrorCode& status);

    // Replace by using languageAlias.
    தருக்க replaceLanguage(தருக்க checkLanguage, தருக்க checkRegion,
                         தருக்க checkVariants, UVector& toBeFreed,
                         UErrorCode& status);

    // Replace by using territoryAlias.
    தருக்க replaceTerritory(UVector& toBeFreed, UErrorCode& status);

    // Replace by using scriptAlias.
    தருக்க replaceScript(UErrorCode& status);

    // Replace by using variantAlias.
    தருக்க replaceVariant(UErrorCode& status);

    // Replace by using subdivisionAlias.
    தருக்க replaceSubdivision(StringPiece subdivision,
                            CharString& output, UErrorCode& status);

    // Replace transformed extensions.
    தருக்க replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UErrorCode& status);
};

CharString&
AliasReplacer::generate_சாவி(
        மாறிலி எழுத்து* language, மாறிலி எழுத்து* region, மாறிலி எழுத்து* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    எனில் (U_FAILURE(status)) { திரும்பு out; }
    out.append(language, status);
    எனில் (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    எனில் (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    திரும்பு out;
}

வெறுமை
AliasReplacer::parseLanguageReplacement(
    மாறிலி எழுத்து* replacement,
    மாறிலி எழுத்து*& replacedLanguage,
    மாறிலி எழுத்து*& replacedScript,
    மாறிலி எழுத்து*& replacedRegion,
    மாறிலி எழுத்து*& replacedVariant,
    மாறிலி எழுத்து*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    எனில் (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        திரும்பு;
    }
    // We have multiple field so we have to allocate and parse
    auto str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    எழுத்து* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    replacedLanguage = (மாறிலி எழுத்து*) data;
    எழுத்து* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    மாறிலி எழுத்து* start = endOfField;
    endOfField = const_cast<எழுத்து*>(uprv_strchr(start, '_'));
    size_t len = 0;
    எனில் (endOfField == nullptr) {
        len = uprv_strlen(start);
    } இல்லையெனில் {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    எனில் (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        எனில் (endOfField == nullptr) {
            திரும்பு;
        }
        start = endOfField++;
        endOfField = const_cast<எழுத்து*>(uprv_strchr(start, '_'));
        எனில் (endOfField == nullptr) {
            len = uprv_strlen(start);
        } இல்லையெனில் {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    எனில் (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        எனில் (endOfField == nullptr) {
            திரும்பு;
        }
        start = endOfField++;
        endOfField = const_cast<எழுத்து*>(uprv_strchr(start, '_'));
        எனில் (endOfField == nullptr) {
            len = uprv_strlen(start);
        } இல்லையெனில் {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    எனில் (len >= 4) {
        // Got a variant
        replacedVariant = start;
        எனில் (endOfField == nullptr) {
            திரும்பு;
        }
        start = endOfField++;
    }
    replacedExtensions = start;
}

தருக்க
AliasReplacer::replaceLanguage(
        தருக்க checkLanguage, தருக்க checkRegion,
        தருக்க checkVariants, UVector& toBeFreed, UErrorCode& status)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    எனில் (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        திரும்பு பொய்;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    மாறிலி எழுத்து* தேடல்_Language = checkLanguage ? language : "und";
    மாறிலி எழுத்து* தேடல்_Region = checkRegion ? region : nullptr;
    மாறிலி எழுத்து* தேடல்_Variant = nullptr;
    க்கு (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        எனில் (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            தேடல்_Variant = static_cast<மாறிலி எழுத்து*>(variants.elementAt(variant_index));
        }

        எனில் (தேடல்_Variant != nullptr && uprv_strlen(தேடல்_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            தேடல்_Variant = nullptr;
        }
        CharString type_சாவி;
        generate_சாவி(தேடல்_Language, தேடல்_Region, தேடல்_Variant, type_சாவி,
                    status);
        எனில் (U_FAILURE(status)) {
            திரும்பு பொய்;
        }
        மாறிலி எழுத்து *replacement = data->languageMap().get(type_சாவி.data());
        எனில் (replacement == nullptr) {
            // Found no replacement data.
            தொடர்;
        }

        மாறிலி எழுத்து* replacedLanguage = nullptr;
        மாறிலி எழுத்து* replacedScript = nullptr;
        மாறிலி எழுத்து* replacedRegion = nullptr;
        மாறிலி எழுத்து* replacedVariant = nullptr;
        மாறிலி எழுத்து* replacedExtensions = nullptr;
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
        replacedRegion = deleteOrReplace(region, தேடல்_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            தேடல்_Variant, தேடல்_Variant, replacedVariant);

        எனில் (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(தேடல்_Variant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            தொடர்;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        எனில் (தேடல்_Variant != nullptr) {
            எனில் (notEmpty(replacedVariant)) {
                variants.setElementAt((வெறுமை*)replacedVariant, variant_index);
            } இல்லையெனில் {
                variants.removeElementAt(variant_index);
            }
        }
        எனில் (replacedExtensions != nullptr) {
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
        திரும்பு உண்மை;
    }
    // Nothing changed by language alias data.
    திரும்பு பொய்;
}

தருக்க
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    எனில் (region == nullptr) {
        // No region to search.
        திரும்பு பொய்;
    }
    மாறிலி எழுத்து *replacement = data->territoryMap().get(region);
    எனில் (replacement == nullptr) {
        // Found no replacement data for this region.
        திரும்பு பொய்;
    }
    மாறிலி எழுத்து* replacedRegion = replacement;
    மாறிலி எழுத்து* firstSpace = uprv_strchr(replacement, ' ');
    எனில் (firstSpace != nullptr) {
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
        மாறிலி எழுத்து* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        எனில் (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            மாறிலி எழுத்து* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            எனில் (foundInReplacement != nullptr) {
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
        எனில் (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        எனில் (U_FAILURE(status)) { திரும்பு பொய்; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        எனில் (U_FAILURE(status)) { திரும்பு பொய்; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    திரும்பு உண்மை;
}

தருக்க
AliasReplacer::replaceScript(UErrorCode& status)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    எனில் (script == nullptr) {
        // No script to search.
        திரும்பு பொய்;
    }
    மாறிலி எழுத்து *replacement = data->scriptMap().get(script);
    எனில் (replacement == nullptr) {
        // Found no replacement data for this script.
        திரும்பு பொய்;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    திரும்பு உண்மை;
}

தருக்க
AliasReplacer::replaceVariant(UErrorCode& status)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    // Since we may have more than one variant, we need to loop through them.
    க்கு (int32_t i = 0; i < variants.size(); i++) {
        மாறிலி எழுத்து* variant = static_cast<மாறிலி எழுத்து*>(variants.elementAt(i));
        மாறிலி எழுத்து *replacement = data->variantMap().get(variant);
        எனில் (replacement == nullptr) {
            // Found no replacement data for this variant.
            தொடர்;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        எனில் (!same(variant, replacement)) {
            variants.setElementAt((வெறுமை*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            எனில் (uprv_strcmp(variant, "heploc") == 0) {
                க்கு (int32_t j = 0; j < variants.size(); j++) {
                     எனில் (uprv_strcmp((மாறிலி எழுத்து*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            திரும்பு உண்மை;
        }
    }
    திரும்பு பொய்;
}

தருக்க
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    மாறிலி எழுத்து *replacement = data->subdivisionMap().get(subdivision.data());
    எனில் (replacement != nullptr) {
        மாறிலி எழுத்து* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        எனில் (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            எனில் (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        திரும்பு உண்மை;
    }
    திரும்பு பொய்;
}

தருக்க
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    int32_t len = transformedExtensions.length();
    மாறிலி எழுத்து* str = transformedExtensions.data();
    மாறிலி எழுத்து* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    எனில் (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        எனில் (U_FAILURE(status)) {
            திரும்பு பொய்;
        }
        T_CString_toLowerCase(output.data());
    }
    எனில் (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        எனில் (U_FAILURE(status)) {
            திரும்பு பொய்;
        }
        செய் {
            மாறிலி எழுத்து* tvalue = uprv_strchr(tkey, '-');
            எனில் (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                திரும்பு பொய்;
            }
            மாறிலி எழுத்து* nextTKey = ultag_getTKeyStart(tvalue);
            எனில் (nextTKey != nullptr) {
                *const_cast<எழுத்து*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.செருகு_Element_At((வெறுமை*)tkey, tfields.size(), status);
            எனில் (U_FAILURE(status)) {
                திரும்பு பொய்;
            }
            tkey = nextTKey;
        } போது (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            திரும்பு uprv_strcmp((மாறிலி எழுத்து*)e1.pointer, (மாறிலி எழுத்து*)e2.pointer);
        }, status);
        க்கு (int32_t i = 0; i < tfields.size(); i++) {
             எனில் (output.length() > 0) {
                 output.append('-', status);
             }
             மாறிலி எழுத்து* tfield = static_cast<மாறிலி எழுத்து*>(tfields.elementAt(i));
             மாறிலி எழுத்து* tvalue = uprv_strchr(tfield, '-');
             எனில் (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 திரும்பு பொய்;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<எழுத்து*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    திரும்பு உண்மை;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    எனில் (U_FAILURE(status)) { திரும்பு out; }
    out.append(language, status);
    எனில் (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    எனில் (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    எனில் (variants.size() > 0) {
        எனில் (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            திரும்பு uprv_strcmp((மாறிலி எழுத்து*)e1.pointer, (மாறிலி எழுத்து*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        க்கு (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<மாறிலி எழுத்து*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    எனில் (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    திரும்பு out;
}

தருக்க
AliasReplacer::replace(மாறிலி Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    U_ASSERT(data != nullptr);
    out.clear();
    language = locale.getLanguage();
    எனில் (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    எனில் (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    எனில் (!notEmpty(region)) {
        region = nullptr;
    }
    மாறிலி எழுத்து* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    எனில் (!variantsBuff.isEmpty()) {
        எனில் (U_FAILURE(status)) { திரும்பு பொய்; }
        எழுத்து* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        எழுத்து* end;
        போது ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            எனில் (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        எனில் (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    எனில் (U_FAILURE(status)) { திரும்பு பொய்; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        திரும்பு uprv_strcmp((மாறிலி எழுத்து*)e1.pointer, (மாறிலி எழுத்து*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    மாறிலி int32_t maxChanged = 4 + variants.size();
    (வெறுமை)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](வெறுமை *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    போது (U_SUCCESS(status)) {
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
        எனில் (    replaceLanguage(உண்மை, உண்மை,  உண்மை,  stringsToBeFreed, status) ||
                replaceLanguage(உண்மை, உண்மை,  பொய், stringsToBeFreed, status) ||
                replaceLanguage(உண்மை, பொய், உண்மை,  stringsToBeFreed, status) ||
                replaceLanguage(உண்மை, பொய், பொய், stringsToBeFreed, status) ||
                replaceLanguage(பொய்,பொய், உண்மை,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            தொடர்;
        }
        // Nothing changed. Break out.
        நிறுத்து;
    }  // while(1)

    எனில் (U_FAILURE(status)) { திரும்பு பொய்; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    மாறிலி எழுத்து* extensionsStr = locale_getKeywordsStart(locale.getName());
    எனில் (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        திரும்பு பொய்;
    }
    outputToString(out, status);
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    எனில் (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        எனில் (U_SUCCESS(status) && !iter.isNull()) {
            மாறிலி எழுத்து* சாவி;
            போது ((சாவி = iter->next(nullptr, status)) != nullptr) {
                எனில் (uprv_strcmp("sd", சாவி) == 0 || uprv_strcmp("rg", சாவி) == 0 ||
                        uprv_strcmp("t", சாவி) == 0) {
                    auto value = locale.get_Keyword_மதிப்பு<CharString>(சாவி, status);
                    எனில் (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        தொடர்;
                    }
                    CharString replacement;
                    எனில் (uprv_strlen(சாவி) == 2) {
                        எனில் (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.set_Keyword_மதிப்பு(சாவி, replacement.data(), status);
                        }
                    } இல்லையெனில் {
                        U_ASSERT(uprv_strcmp(சாவி, "t") == 0);
                        எனில் (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.set_Keyword_மதிப்பு(சாவி, replacement.data(), status);
                        }
                    }
                    எனில் (U_FAILURE(status)) {
                        திரும்பு பொய்;
                    }
                }
            }
        }
        எனில் (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    // If the tag is not changed, return.
    எனில் (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        திரும்பு பொய்;
    }
    திரும்பு உண்மை;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
தருக்க
canonicalizeLocale(மாறிலி Locale& locale, CharString& out, UErrorCode& status)
{
    எனில் (U_FAILURE(status)) { திரும்பு பொய்; }
    AliasReplacer replacer(status);
    திரும்பு replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
தருக்க
isKnownCanonicalizedLocale(மாறிலி எழுத்து* locale, UErrorCode& status)
{
    எனில் (U_FAILURE(status)) { திரும்பு பொய்; }

    எனில் (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        திரும்பு உண்மை;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    திரும்பு uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT மாறிலி எழுத்து* மாறிலி*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    திரும்பு KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT தருக்க
ulocimp_isCanonicalizedLocaleForTest(மாறிலி எழுத்து* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    திரும்பு !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(மாறிலி எழுத்து* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    திரும்பு localeID == nullptr ? *இது = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    செய் {
        எழுத்து *separator;
        எழுத்து *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        மாறிலி auto parse = [canonicalize](std::string_view localeID,
                                          எழுத்து* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            திரும்பு ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    எனில் (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } இல்லையெனில் {
                        ulocimp_getName(localeID, sink, status);
                    }
                },
                status);
        };

        // "canonicalize" the locale ID to ICU/Java format
        எழுத்து* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, அளவு Nest::baseName, err);

        FixedString fullNameBuffer;
        எனில் (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(அளவு Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            எனில் (!fullNameBuffer.reserve(length + 1)) {
                நிறுத்து; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        எனில்(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            நிறுத்து;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        எழுத்து* at = uprv_strchr(fullName, '@');
        போது ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        எழுத்து* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        எனில் (separator!=nullptr || sep2!=nullptr) {
            எனில் (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } இல்லையெனில் {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        தருக்க hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        எனில் (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            நிறுத்து; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        எனில் (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        எனில் (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        எனில் (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } இல்லையெனில் எனில் (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        எனில் (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } இல்லையெனில் எனில் (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        எனில் (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } இல்லையெனில் {
            எனில் (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                எனில் (fullNameBuffer.isEmpty()) {
                    நிறுத்து; // error: out of memory
                }
                எனில் (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                எனில் (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                எனில் (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            எனில் (isBogus()) {
                நிறுத்து; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            எனில் (hasKeywords) {
                எனில் (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    எனில் (heap.ptr->baseName.isEmpty()) {
                        நிறுத்து; // error: out of memory
                    }
                }
            }
        }

        எனில் (canonicalize) {
            எனில் (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                எனில் (canonicalizeLocale(*இது, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), பொய்);
                }
                எனில் (U_FAILURE(err)) {
                    நிறுத்து;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        திரும்பு *இது;
    } போது(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    திரும்பு *இது;
}

int32_t
Locale::hashCode() மாறிலி
{
    திரும்பு ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

வெறுமை
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

மாறிலி Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        எனில் (gDefaultLocale != nullptr) {
            திரும்பு *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    திரும்பு *locale_set_default_internal(nullptr, status);
}



வெறுமை U_EXPORT2
Locale::setDefault( மாறிலி   Locale&     newLocale,
                            UErrorCode&  status)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    மாறிலி எழுத்து *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

வெறுமை
Locale::addLikelySubtags(UErrorCode& status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    எனில் (U_FAILURE(status)) {
        எனில் (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        திரும்பு;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/பொய்);
    எனில் (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

வெறுமை
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(பொய், status);
}
வெறுமை
Locale::minimizeSubtags(தருக்க favorScript, UErrorCode& status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    எனில் (U_FAILURE(status)) {
        எனில் (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        திரும்பு;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/பொய்);
    எனில் (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

வெறுமை
Locale::canonicalize(UErrorCode& status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    எனில் (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    CharString uncanonicalized(getName(), status);
    எனில் (U_FAILURE(status)) {
        எனில் (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        திரும்பு;
    }
    init(uncanonicalized.data(), /*canonicalize=*/உண்மை);
    எனில் (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    எனில் (U_FAILURE(status)) {
        திரும்பு result;
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

    எனில் (U_FAILURE(status)) {
        திரும்பு result;
    }

    எனில் (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு result;
    }

    result.init(localeID.data(), /*canonicalize=*/பொய்);
    எனில் (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    திரும்பு result;
}

வெறுமை
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) மாறிலி
{
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    எனில் (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/பொய், status);
}

Locale U_EXPORT2
Locale::createFromName (மாறிலி எழுத்து *name)
{
    எனில் (name) {
        Locale l("");
        l.init(name, பொய்);
        திரும்பு l;
    }
    இல்லையெனில் {
        திரும்பு getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, பொய்);
    திரும்பு loc;
}

Locale U_EXPORT2
Locale::createCanonical(மாறிலி எழுத்து* name) {
    Locale loc("");
    loc.init(name, உண்மை);
    திரும்பு loc;
}

மாறிலி எழுத்து *
Locale::getISO3Language() மாறிலி
{
    திரும்பு uloc_getISO3Language(getName());
}


மாறிலி எழுத்து *
Locale::getISO3Country() மாறிலி
{
    திரும்பு uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() மாறிலி
{
    திரும்பு uloc_getLCID(getName());
}

மாறிலி எழுத்து* மாறிலி* U_EXPORT2 Locale::getISOCountries()
{
    திரும்பு uloc_getISOCountries();
}

மாறிலி எழுத்து* மாறிலி* U_EXPORT2 Locale::getISOLanguages()
{
    திரும்பு uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
வெறுமை Locale::setFromPOSIXID(மாறிலி எழுத்து *posixID)
{
    init(posixID, உண்மை);
}

மாறிலி Locale & U_EXPORT2
Locale::get_வேர்()
{
    திரும்பு getLocale(e_வேர்);
}

மாறிலி Locale & U_EXPORT2
Locale::getEnglish()
{
    திரும்பு getLocale(eENGLISH);
}

மாறிலி Locale & U_EXPORT2
Locale::getFrench()
{
    திரும்பு getLocale(eFRENCH);
}

மாறிலி Locale & U_EXPORT2
Locale::getGerman()
{
    திரும்பு getLocale(eGERMAN);
}

மாறிலி Locale & U_EXPORT2
Locale::getItalian()
{
    திரும்பு getLocale(eITALIAN);
}

மாறிலி Locale & U_EXPORT2
Locale::getJapanese()
{
    திரும்பு getLocale(eJAPANESE);
}

மாறிலி Locale & U_EXPORT2
Locale::getKorean()
{
    திரும்பு getLocale(eKOREAN);
}

மாறிலி Locale & U_EXPORT2
Locale::getChinese()
{
    திரும்பு getLocale(eCHINESE);
}

மாறிலி Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    திரும்பு getLocale(eCHINA);
}

மாறிலி Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    திரும்பு getLocale(eTAIWAN);
}


மாறிலி Locale & U_EXPORT2
Locale::getFrance()
{
    திரும்பு getLocale(eFRANCE);
}

மாறிலி Locale & U_EXPORT2
Locale::getGermany()
{
    திரும்பு getLocale(eGERMANY);
}

மாறிலி Locale & U_EXPORT2
Locale::getItaly()
{
    திரும்பு getLocale(eITALY);
}

மாறிலி Locale & U_EXPORT2
Locale::getJapan()
{
    திரும்பு getLocale(eJAPAN);
}

மாறிலி Locale & U_EXPORT2
Locale::getKorea()
{
    திரும்பு getLocale(eKOREA);
}

மாறிலி Locale & U_EXPORT2
Locale::getChina()
{
    திரும்பு getLocale(eCHINA);
}

மாறிலி Locale & U_EXPORT2
Locale::getPRC()
{
    திரும்பு getLocale(eCHINA);
}

மாறிலி Locale & U_EXPORT2
Locale::getTaiwan()
{
    திரும்பு getLocale(eTAIWAN);
}

மாறிலி Locale & U_EXPORT2
Locale::getUK()
{
    திரும்பு getLocale(eUK);
}

மாறிலி Locale & U_EXPORT2
Locale::getUS()
{
    திரும்பு getLocale(eUS);
}

மாறிலி Locale & U_EXPORT2
Locale::getCanada()
{
    திரும்பு getLocale(eCANADA);
}

மாறிலி Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    திரும்பு getLocale(eCANADA_FRENCH);
}

மாறிலி Locale &
Locale::getLocale(முழு locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    எனில் (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    திரும்பு localeCache[locid]; /*operating on nullptr*/
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
    திரும்பு gLocaleCache;
}

வகுப்பு KeywordEnumeration : பொது StringEnumeration {
பாதுகாக்கப்பட்ட:
    FixedString keywords;
தனிப்பட்ட:
    int32_t length;
    மாறிலி எழுத்து *current;
    நிலையான மாறிலி எழுத்து fgClassID;

பொது:
    நிலையான UClassID U_EXPORT2 getStaticClassID() { திரும்பு (UClassID)&fgClassID; }
    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override { திரும்பு getStaticClassID(); }
பொது:
    KeywordEnumeration(மாறிலி எழுத்து *சாவிகள், int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        எனில்(U_SUCCESS(status) && keywordLen != 0) {
            எனில்(சாவிகள் == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } இல்லையெனில் {
                keywords = {சாவிகள், static_cast<std::string_view::size_type>(length)};
                எனில் (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } இல்லையெனில் {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    மெய்நிகர் ~KeywordEnumeration();

    மெய்நிகர் StringEnumeration * clone() மாறிலி override
    {
        UErrorCode status = U_ZERO_ERROR;
        திரும்பு புதிய KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    மெய்நிகர் int32_t count(UErrorCode& status) மாறிலி override {
        எனில் (U_FAILURE(status)) { திரும்பு 0; }
        மாறிலி எழுத்து *kw = keywords.data();
        int32_t result = 0;
        போது(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        திரும்பு result;
    }

    மெய்நிகர் மாறிலி எழுத்து* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        மாறிலி எழுத்து* result;
        int32_t len;
        எனில்(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            எனில்(resultLength != nullptr) {
                *resultLength = len;
            }
        } இல்லையெனில் {
            எனில்(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        திரும்பு result;
    }

    மெய்நிகர் மாறிலி UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        எனில் (U_FAILURE(status)) { திரும்பு nullptr; }
        int32_t resultLength = 0;
        மாறிலி எழுத்து *s = next(&resultLength, status);
        திரும்பு setChars(s, resultLength, status);
    }

    மெய்நிகர் வெறுமை reset(UErrorCode& status) override {
        எனில் (U_FAILURE(status)) { திரும்பு; }
        current = keywords.data();
    }
};

மாறிலி எழுத்து KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = இயல்பு;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
வகுப்பு UnicodeKeywordEnumeration : பொது KeywordEnumeration {
பொது:
    using KeywordEnumeration::KeywordEnumeration;
    மெய்நிகர் ~UnicodeKeywordEnumeration();

    மெய்நிகர் மாறிலி எழுத்து* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        மாறிலி எழுத்து* legacy_சாவி = KeywordEnumeration::next(nullptr, status);
        போது (U_SUCCESS(status) && legacy_சாவி != nullptr) {
            மாறிலி எழுத்து* சாவி = uloc_to_Unicode_Locale_சாவி(legacy_சாவி);
            எனில் (சாவி != nullptr) {
                எனில் (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(சாவி));
                }
                திரும்பு சாவி;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_சாவி = KeywordEnumeration::next(nullptr, status);
        }
        எனில் (resultLength != nullptr) *resultLength = 0;
        திரும்பு nullptr;
    }
    மெய்நிகர் int32_t count(UErrorCode& status) மாறிலி override {
        எனில் (U_FAILURE(status)) { திரும்பு 0; }
        மாறிலி எழுத்து *kw = keywords.data();
        int32_t result = 0;
        போது(*kw) {
            எனில் (uloc_to_Unicode_Locale_சாவி(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        திரும்பு result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = இயல்பு;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) மாறிலி
{
    StringEnumeration *result = nullptr;

    எனில் (U_FAILURE(status)) {
        திரும்பு result;
    }

    மாறிலி எழுத்து* variantStart = uprv_strchr(getName(), '@');
    மாறிலி எழுத்து* assignment = uprv_strchr(getName(), '=');
    எனில்(variantStart) {
        எனில்(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', பொய், status);
            எனில் (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = புதிய KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                எனில் (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } இல்லையெனில் {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    திரும்பு result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) மாறிலி
{
    StringEnumeration *result = nullptr;

    எனில் (U_FAILURE(status)) {
        திரும்பு result;
    }

    மாறிலி எழுத்து* variantStart = uprv_strchr(getName(), '@');
    மாறிலி எழுத்து* assignment = uprv_strchr(getName(), '=');
    எனில்(variantStart) {
        எனில்(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', பொய், status);
            எனில் (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = புதிய UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                எனில் (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } இல்லையெனில் {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    திரும்பு result;
}

int32_t
Locale::get_Keyword_மதிப்பு(மாறிலி எழுத்து* keywordName, எழுத்து *buffer, int32_t bufLen, UErrorCode &status) மாறிலி
{
    திரும்பு uloc_get_Keyword_மதிப்பு(getName(), keywordName, buffer, bufLen, &status);
}

வெறுமை
Locale::get_Keyword_மதிப்பு(StringPiece keywordName, ByteSink& sink, UErrorCode& status) மாறிலி {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    எனில் (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    ulocimp_get_Keyword_மதிப்பு(getName(), keywordName, sink, status);
}

வெறுமை
Locale::get_Unicode_Keyword_மதிப்பு(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) மாறிலி {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    std::optional<std::string_view> legacy_சாவி = ulocimp_to_Legacy_சாவி_With_Fallback(keywordName);
    எனில் (!legacy_சாவி.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    auto legacy_மதிப்பு = get_Keyword_மதிப்பு<CharString>(*legacy_சாவி, status);

    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    std::optional<std::string_view> unicode_மதிப்பு =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_மதிப்பு.toStringPiece());
    எனில் (!unicode_மதிப்பு.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    sink.Append(unicode_மதிப்பு->data(), static_cast<int32_t>(unicode_மதிப்பு->size()));
}

வெறுமை
Locale::set_Keyword_மதிப்பு(StringPiece keywordName,
                        StringPiece keyword_மதிப்பு,
                        UErrorCode& status) {
    எனில் (U_FAILURE(status)) { திரும்பு; }
    எனில் (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    எனில் (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_set_Keyword_மதிப்பு(keywordName, keyword_மதிப்பு, localeID, status);
    எனில் (U_FAILURE(status)) {
        எனில் (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        திரும்பு;
    }

    மாறிலி எழுத்து* at = locale_getKeywordsStart(localeID.toStringPiece());
    தருக்க hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    எனில் (!hasKeywords) {
        எனில் (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            எனில் (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < அளவு Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, அளவு Nest::baseName, status);
            } இல்லையெனில் {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                எனில் (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    திரும்பு;
                }
            }
        }
    } இல்லையெனில் {
        Heap* heap = nullptr;
        எனில் (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            எனில் (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு;
            }
        } இல்லையெனில் {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        எனில் (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            திரும்பு;
        }

        எனில் (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            எனில் (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                எனில் (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    திரும்பு;
                }
            }
        }
    }
}

வெறுமை
Locale::set_Unicode_Keyword_மதிப்பு(StringPiece keywordName,
                               StringPiece keyword_மதிப்பு,
                               UErrorCode& status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    std::optional<std::string_view> legacy_சாவி = ulocimp_to_Legacy_சாவி_With_Fallback(keywordName);
    எனில் (!legacy_சாவி.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    std::string_view value;

    எனில் (!keyword_மதிப்பு.empty()) {
        std::optional<std::string_view> legacy_மதிப்பு =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_மதிப்பு);
        எனில் (!legacy_மதிப்பு.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு;
        }
        value = *legacy_மதிப்பு;
    }

    set_Keyword_மதிப்பு(*legacy_சாவி, value, status);
}

மாறிலி எழுத்து*
Locale::getCountry() மாறிலி U_LIFETIME_BOUND {
    திரும்பு getField<&Nest::getRegion, &Heap::getRegion>();
}

மாறிலி எழுத்து*
Locale::getLanguage() மாறிலி U_LIFETIME_BOUND {
    திரும்பு getField<&Nest::getLanguage, &Heap::getLanguage>();
}

மாறிலி எழுத்து*
Locale::getScript() மாறிலி U_LIFETIME_BOUND {
    திரும்பு getField<&Nest::getScript, &Heap::getScript>();
}

மாறிலி எழுத்து*
Locale::getVariant() மாறிலி U_LIFETIME_BOUND {
    திரும்பு getField<&Nest::getVariant, &Heap::getVariant>();
}

மாறிலி எழுத்து*
Locale::getName() மாறிலி U_LIFETIME_BOUND {
    திரும்பு getField<&Nest::getBaseName, &Heap::getFullName>();
}

மாறிலி எழுத்து*
Locale::getBaseName() மாறிலி U_LIFETIME_BOUND {
    திரும்பு getField<&Nest::getBaseName, &Heap::getBaseName>();
}

template <மாறிலி எழுத்து* (Locale::Nest::*மாறிலி NEST)() மாறிலி,
          மாறிலி எழுத்து* (Locale::Heap::*மாறிலி HEAP)() மாறிலி>
மாறிலி எழுத்து* Locale::getField() மாறிலி U_LIFETIME_BOUND {
    திரும்பு payload.visit([] { திரும்பு ""; },
                         [](மாறிலி Nest& nest) { திரும்பு (nest.*NEST)(); },
                         [](மாறிலி Heap& heap) { திரும்பு (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = இயல்பு;

//eof
U_NAMESPACE_END
