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

#অন্তর্ভাব <cstddef>
#অন্তর্ভাব <optional>
#অন্তর্ভাব <string_view>
#অন্তর্ভাব <type_traits>
#অন্তর্ভাব <utility>

#অন্তর্ভাব "unicode/bytestream.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/localebuilder.h"
#অন্তর্ভাব "unicode/localpointer.h"
#অন্তর্ভাব "unicode/strenum.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "unicode/uloc.h"
#অন্তর্ভাব "unicode/ures.h"

#অন্তর্ভাব "bytesinkutil.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "charstrmap.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "fixedstring.h"
#অন্তর্ভাব "mutex.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "ulocimp.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "uniquecharstr.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "uvector.h"

#যদি UPRV_HAS_FEATURE(memory_sanitizer)
#অন্তর্ভাব <sanitizer/msan_interface.h>
#endif

U_NAMESPACE_BEGIN

স্থির Locale   *gLocaleCache = nullptr;
স্থির UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
স্থির UMutex gDefaultLocaleMutex;
স্থির UHashtable *gDefaultLocalesHashT = nullptr;
স্থির Locale *gDefaultLocale = nullptr;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#define ULOC_STRING_LIMIT 357913941

U_NAMESPACE_END

প্রকারলেখ গণনা ELocalePos {
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
    e_মূল,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

নামাকাশ {

//
// Deleter function for Locales owned by the default Locale hash table/
//
শূন্য U_CALLCONV
deleteLocale(শূন্য *obj) {
    delete static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    delete [] gLocaleCache;
    gLocaleCache = nullptr;
    gLocaleCacheInitOnce.reset();

    যদি (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    ফেরত সত্য;
}

শূন্য U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = নতুন Locale[static_cast<পূর্ণসংখ্যা>(eMAX_LOCALES)];
    যদি (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_মূল]          = Locale("");
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

Locale *locale_set_default_internal(ধ্রুবক অক্ষর *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = মিথ্যা;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    যদি (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = সত্য; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    যদি (U_FAILURE(status)) {
        ফেরত gDefaultLocale;
    }

    যদি (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        যদি (U_FAILURE(status)) {
            ফেরত gDefaultLocale;
        }
        uhash_set_মান_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.তথ্য()));
    যদি (newDefault == nullptr) {
        newDefault = নতুন Locale(Locale::eBOGUS);
        যদি (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত gDefaultLocale;
        }
        newDefault->init(localeNameBuf.তথ্য(), মিথ্যা);
        uhash_put(gDefaultLocalesHashT, const_cast<অক্ষর*>(newDefault->getName()), newDefault, &status);
        যদি (U_FAILURE(status)) {
            ফেরত gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    ফেরত gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC শূন্য
locale_set_default(ধ্রুবক অক্ষর *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC ধ্রুবক অক্ষর *
locale_get_default()
{
    U_NAMESPACE_USE
    ফেরত Locale::getDefault().getName();
}

নামাকাশ {

template <auto FIELD, typename T>
শূন্য copyToArray(std::string_view sv, T* that) {
    auto& field = that->*FIELD;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    যদি (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.তথ্য(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

শূন্য Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, এই);
    copyToArray<&Nest::script>(script, এই);
    copyToArray<&Nest::region>(region, এই);
    এই->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(Heap, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(Heap, region));
    U_ASSERT(এই == reinterpret_cast<Nest*>(&heap));
#যদি UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, আকার(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, এই);
    copyToArray<&Nest::region>(heap.region, এই);
    এই->variantBegin = variantBegin;
    *এই->baseName = '\0';
}

কাঠামো Locale::Heap::Alloc : সর্বজনীন UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    ধ্রুবক অক্ষর* getVariant() ধ্রুবক U_LIFETIME_BOUND {
        ফেরত variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    ধ্রুবক অক্ষর* getFullName() ধ্রুবক U_LIFETIME_BOUND { ফেরত fullName.তথ্য(); }
    ধ্রুবক অক্ষর* getBaseName() ধ্রুবক U_LIFETIME_BOUND {
        যদি (baseName.isEmpty()) {
            যদি (ধ্রুবক অক্ষর* name = fullName.তথ্য(); *name != '@') {
                ফেরত name;
            }
        }
        ফেরত baseName.তথ্য();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(ধ্রুবক Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        যদি (U_SUCCESS(status)) {
            যদি (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                যদি (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } অন্যথা {
                    যদি (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        যদি (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = delete;

    ~Alloc() = পূর্বনির্ধারিত;
};

ধ্রুবক অক্ষর* Locale::Heap::getVariant() ধ্রুবক U_LIFETIME_BOUND { ফেরত ptr->getVariant(); }
ধ্রুবক অক্ষর* Locale::Heap::getFullName() ধ্রুবক U_LIFETIME_BOUND { ফেরত ptr->getFullName(); }
ধ্রুবক অক্ষর* Locale::Heap::getBaseName() ধ্রুবক U_LIFETIME_BOUND { ফেরত ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = নতুন Alloc(variantBegin);
    যদি (ptr == nullptr) {
        type = eBOGUS;
    } অন্যথা {
        type = eHEAP;
        copyToArray<&Heap::language>(language, এই);
        copyToArray<&Heap::script>(script, এই);
        copyToArray<&Heap::region>(region, এই);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::operator=(ধ্রুবক Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = নতুন Alloc(*other.ptr, status);
    যদি (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } অন্যথা {
        type = eHEAP;
        uprv_memcpy(language, other.language, আকার language);
        uprv_memcpy(script, other.script, আকার script);
        uprv_memcpy(region, other.region, আকার region);
    }
    ফেরত *এই;
}

Locale::Heap& Locale::Heap::operator=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, আকার language);
    uprv_memcpy(script, other.script, আকার script);
    uprv_memcpy(region, other.region, আকার region);
    ফেরত *এই;
}

template <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
auto Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) ধ্রুবক {
    নির্বাচন (type) {
        ক্ষেত্র eBOGUS:
            ফেরত bogusFn(args...);
        ক্ষেত্র eNEST:
            ফেরত nestFn(nest, args...);
        ক্ষেত্র eHEAP:
            ফেরত heapFn(heap, args...);
        পূর্বনির্ধারিত:
            UPRV_UNREACHABLE_EXIT;
    };
}

শূন্য Locale::Payload::copy(ধ্রুবক Payload& other) {
    other.visit([](Payload*) {},
                [](ধ্রুবক Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ধ্রুবক Heap& heap, Payload* dst) { dst->heap = heap; },
                এই);
}

শূন্য Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](ধ্রুবক Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ধ্রুবক Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        এই);
}

Locale::Payload::~Payload() {
    যদি (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(ধ্রুবক Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::operator=(ধ্রুবক Payload& other) U_LIFETIME_BOUND {
    যদি (এই != &other) {
        setToBogus();
        copy(other);
    }
    ফেরত *এই;
}

Locale::Payload& Locale::Payload::operator=(Payload&& other) noexcept U_LIFETIME_BOUND {
    যদি (এই != &other) {
        setToBogus();
        move(std::move(other));
    }
    ফেরত *এই;
}

শূন্য Locale::Payload::setToBogus() {
    এই->~Payload();
    type = eBOGUS;
}

template <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    যদি constexpr (std::is_same_v<T, Nest>) {
        এই->~Payload();
        ::নতুন (&nest) Nest(std::forward<Args>(args)...);
        ফেরত nest;
    }
    যদি constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::নতুন (&heap) Heap(std::forward<Args>(args)...);
        ফেরত heap;
    }
}

template <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    ফেরত type == eNEST ? &nest : nullptr;
}
template <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    ফেরত type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = পূর্বনির্ধারিত;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, মিথ্যা);
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


Locale::Locale( ধ্রুবক   অক্ষর * newLanguage,
                ধ্রুবক   অক্ষর * newCountry,
                ধ্রুবক   অক্ষর * newVariant,
                ধ্রুবক   অক্ষর * newKeywords)
    : UObject(), payload()
{
    যদি( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, মিথ্যা); /* shortcut */
    }
    অন্যথা
    {
        UErrorCode status = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        যদি ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            যদি ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                ফেরত;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        যদি ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            যদি ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                ফেরত;
            }
        }

        // _Variant
        যদি ( newVariant != nullptr )
        {
            // remove leading _'s
            যখন(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            যদি ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                ফেরত;
            }
            যখন( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        যদি ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            যদি ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              ফেরত;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        যদি ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        যদি ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        যদি ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        যদি ( ksize != 0)
        {
            যদি (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            অন্যথা {
                togo.append('_', status); /* Variant parsing with a script */
                যদি ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        যদি (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            ফেরত;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.তথ্য(), মিথ্যা);
    }
}

Locale::Locale(ধ্রুবক Locale&) = পূর্বনির্ধারিত;
Locale::Locale(Locale&&) noexcept = পূর্বনির্ধারিত;

Locale& Locale::operator=(ধ্রুবক Locale&) U_LIFETIME_BOUND = পূর্বনির্ধারিত;
Locale& Locale::operator=(Locale&&) noexcept U_LIFETIME_BOUND = পূর্বনির্ধারিত;

Locale *
Locale::clone() ধ্রুবক {
    ফেরত নতুন Locale(*এই);
}

যৌক্তিক
Locale::operator==( ধ্রুবক   Locale& other) ধ্রুবক
{
    ফেরত uprv_strcmp(other.getName(), getName()) == 0;
}

নামাকাশ {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr ধ্রুবক অক্ষর* KNOWN_CANONICALIZED[] = {
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
    যদি (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    ফেরত সত্য;
}

শূন্য U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    জন্য (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (শূন্য*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

শ্রেণী Alias_তথ্য;

/**
 * A Builder class to build the alias data.
 */
শ্রেণী Alias_তথ্য_Builder {
সর্বজনীন:
    Alias_তথ্য_Builder() {
    }

    // Build the AliasData from resource.
    Alias_তথ্য* build(UErrorCode &status);

ব্যক্তিগত:
    শূন্য readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<ধ্রুবক অক্ষর*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   শূন্য (*checkType)(ধ্রুবক অক্ষর* type),
                   শূন্য (*checkReplacement)(ধ্রুবক UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    শূন্য readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<ধ্রুবক অক্ষর*>& types,
                           LocalMemory<int32_t>& replacementIndexes,
                           int32_t &length,
                           UErrorCode &status);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    শূন্য readScriptAlias(UResourceBundle* alias,
                         UniqueCharStrings* strings,
                         LocalMemory<ধ্রুবক অক্ষর*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    শূন্য readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<ধ্রুবক অক্ষর*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    শূন্য readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ধ্রুবক অক্ষর*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    শূন্য readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ধ্রুবক অক্ষর*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
শ্রেণী Alias_তথ্য : সর্বজনীন UMemory {
সর্বজনীন:
    স্থির ধ্রুবক Alias_তথ্য* singleton(UErrorCode& status) {
        যদি (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            ফেরত nullptr;
        }
        umtx_initOnce(Alias_তথ্য::gInitOnce, &Alias_তথ্য::load_তথ্য, status);
        ফেরত gSingleton;
    }

    ধ্রুবক CharStringMap& languageMap() ধ্রুবক U_LIFETIME_BOUND { ফেরত language; }
    ধ্রুবক CharStringMap& scriptMap() ধ্রুবক U_LIFETIME_BOUND { ফেরত script; }
    ধ্রুবক CharStringMap& territoryMap() ধ্রুবক U_LIFETIME_BOUND { ফেরত territory; }
    ধ্রুবক CharStringMap& variantMap() ধ্রুবক U_LIFETIME_BOUND { ফেরত variant; }
    ধ্রুবক CharStringMap& subdivisionMap() ধ্রুবক U_LIFETIME_BOUND { ফেরত subdivision; }

    স্থির শূন্য U_CALLCONV load_তথ্য(UErrorCode &status);
    স্থির UBool U_CALLCONV cleanup();

    স্থির UInitOnce gInitOnce;

ব্যক্তিগত:
    Alias_তথ্য(CharStringMap languageMap,
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

    ~Alias_তথ্য() {
        delete strings;
    }

    স্থির ধ্রুবক Alias_তথ্য* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    friend শ্রেণী Alias_তথ্য_Builder;
};


ধ্রুবক Alias_তথ্য* Alias_তথ্য::gSingleton = nullptr;
UInitOnce Alias_তথ্য::gInitOnce {};

UBool U_CALLCONV
Alias_তথ্য::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    ফেরত সত্য;
}

শূন্য
Alias_তথ্য_Builder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুবক অক্ষর*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        শূন্য (*checkType)(ধ্রুবক অক্ষর* type),
        শূন্য (*checkReplacement)(ধ্রুবক UChar* replacement),
        UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    length = ures_get_আকার(alias);
    ধ্রুবক অক্ষর** rawTypes = types.allocateInsteadAndCopy(length);
    যদি (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    যদি (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    জন্য (পূর্ণসংখ্যা i = 0; U_SUCCESS(status) && ures_has_পরবর্তী(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_get_পরবর্তী_Resource(alias, nullptr, &status));
        ধ্রুবক অক্ষর* aliasFrom = ures_get_চাবি(res.getAlias());
        ধ্রুবক UChar* aliasTo =
            ures_get_String_By_চাবি(res.getAlias(), "replacement", nullptr, &status);
        যদি (U_FAILURE(status)) ফেরত;

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
শূন্য
Alias_তথ্য_Builder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুবক অক্ষর*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ফেরত readAlias(
        alias, strings, types, replacementIndexes, length,
#যদি U_DEBUG
        [](ধ্রুবক অক্ষর* type) {
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
#অন্যথা
        [](ধ্রুবক অক্ষর*) {},
#endif
        [](ধ্রুবক UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
শূন্য
Alias_তথ্য_Builder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুবক অক্ষর*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ফেরত readAlias(
        alias, strings, types, replacementIndexes, length,
#যদি U_DEBUG
        [](ধ্রুবক অক্ষর* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ধ্রুবক UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#অন্যথা
        [](ধ্রুবক অক্ষর*) {},
        [](ধ্রুবক UChar*) { },
#endif
        status);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
শূন্য
Alias_তথ্য_Builder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুবক অক্ষর*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ফেরত readAlias(
        alias, strings, types, replacementIndexes, length,
#যদি U_DEBUG
        [](ধ্রুবক অক্ষর* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#অন্যথা
        [](ধ্রুবক অক্ষর*) {},
#endif
        [](ধ্রুবক UChar*) { },
        status);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
শূন্য
Alias_তথ্য_Builder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুবক অক্ষর*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ফেরত readAlias(
        alias, strings, types, replacementIndexes, length,
#যদি U_DEBUG
        [](ধ্রুবক অক্ষর* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](ধ্রুবক UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#অন্যথা
        [](ধ্রুবক অক্ষর*) {},
        [](ধ্রুবক UChar*) { },
#endif
        status);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
শূন্য
Alias_তথ্য_Builder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুবক অক্ষর*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ফেরত readAlias(
        alias, strings, types, replacementIndexes, length,
#যদি U_DEBUG
        [](ধ্রুবক অক্ষর* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#অন্যথা
        [](ধ্রুবক অক্ষর*) {},
#endif
        [](ধ্রুবক UChar*) { },
        status);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
শূন্য U_CALLCONV
Alias_তথ্য::load_তথ্য(UErrorCode &status)
{
#ifdef LOCALE_CANONICALIZATION_DEBUG
    UDate start = uprv_getRawUTCtime();
#endif  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    Alias_তথ্য_Builder builder;
    gSingleton = builder.build(status);
#ifdef LOCALE_CANONICALIZATION_DEBUG
    UDate end = uprv_getRawUTCtime();
    printf("AliasData::loadData took total %f ms\n", end - start);
#endif  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
Alias_তথ্য*
Alias_তথ্য_Builder::build(UErrorCode &status) {
    যদি (U_FAILURE(status)) { ফেরত nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_চাবি(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_চাবি(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_চাবি(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_চাবি(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_চাবি(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_চাবি(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }
    int32_t languages_দৈর্ঘ্য = 0, script_দৈর্ঘ্য = 0, territory_দৈর্ঘ্য = 0,
            variant_দৈর্ঘ্য = 0, subdivision_দৈর্ঘ্য = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<ধ্রুবক অক্ষর*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languages_দৈর্ঘ্য,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<ধ্রুবক অক্ষর*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    script_দৈর্ঘ্য,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<ধ্রুবক অক্ষর*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territory_দৈর্ঘ্য, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<ধ্রুবক অক্ষর*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variant_দৈর্ঘ্য, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<ধ্রুবক অক্ষর*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivision_দৈর্ঘ্য, status);

    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    জন্য (int32_t i = 0; U_SUCCESS(status) && i < languages_দৈর্ঘ্য; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    জন্য (int32_t i = 0; U_SUCCESS(status) && i < script_দৈর্ঘ্য; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    জন্য (int32_t i = 0; U_SUCCESS(status) && i < territory_দৈর্ঘ্য; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    জন্য (int32_t i = 0; U_SUCCESS(status) && i < variant_দৈর্ঘ্য; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    জন্য (int32_t i = 0; U_SUCCESS(status) && i < subdivision_দৈর্ঘ্য; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    // copy hashtables
    auto *তথ্য = নতুন Alias_তথ্য(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    যদি (তথ্য == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ফেরত তথ্য;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
শ্রেণী AliasReplacer {
সর্বজনীন:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       ফেরত 0==uprv_strcmp((ধ্রুবক অক্ষর*)e1.pointer,
                                             (ধ্রুবক অক্ষর*)e2.pointer);}),
                     status),
            তথ্য(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    যৌক্তিক replace(
        ধ্রুবক Locale& locale, CharString& out, UErrorCode& status);

ব্যক্তিগত:
    ধ্রুবক অক্ষর* language;
    ধ্রুবক অক্ষর* script;
    ধ্রুবক অক্ষর* region;
    ধ্রুবক অক্ষর* extensions;
    UVector variants;

    ধ্রুবক Alias_তথ্য* তথ্য;

    অন্তঃরেখ যৌক্তিক notEmpty(ধ্রুবক অক্ষর* str) {
        ফেরত str && str[0] != NULL_CHAR;
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
    অন্তঃরেখ ধ্রুবক অক্ষর* deleteOrReplace(ধ্রুবক অক্ষর* input U_LIFETIME_BOUND,
                                       ধ্রুবক অক্ষর* type,
                                       ধ্রুবক অক্ষর* replacement U_LIFETIME_BOUND) {
        ফেরত notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    অন্তঃরেখ যৌক্তিক same(ধ্রুবক অক্ষর* a, ধ্রুবক অক্ষর* b) {
        যদি (a == nullptr && b == nullptr) {
            ফেরত সত্য;
        }
        যদি ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          ফেরত মিথ্যা;
        }
        ফেরত uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_চাবি(ধ্রুবক অক্ষর* language, ধ্রুবক অক্ষর* region,
                            ধ্রুবক অক্ষর* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    শূন্য parseLanguageReplacement(ধ্রুবক অক্ষর* replacement,
                                  ধ্রুবক অক্ষর*& replaceLanguage,
                                  ধ্রুবক অক্ষর*& replaceScript,
                                  ধ্রুবক অক্ষর*& replaceRegion,
                                  ধ্রুবক অক্ষর*& replaceVariant,
                                  ধ্রুবক অক্ষর*& replaceExtensions,
                                  UVector& toBeFreed,
                                  UErrorCode& status);

    // Replace by using languageAlias.
    যৌক্তিক replaceLanguage(যৌক্তিক checkLanguage, যৌক্তিক checkRegion,
                         যৌক্তিক checkVariants, UVector& toBeFreed,
                         UErrorCode& status);

    // Replace by using territoryAlias.
    যৌক্তিক replaceTerritory(UVector& toBeFreed, UErrorCode& status);

    // Replace by using scriptAlias.
    যৌক্তিক replaceScript(UErrorCode& status);

    // Replace by using variantAlias.
    যৌক্তিক replaceVariant(UErrorCode& status);

    // Replace by using subdivisionAlias.
    যৌক্তিক replaceSubdivision(StringPiece subdivision,
                            CharString& output, UErrorCode& status);

    // Replace transformed extensions.
    যৌক্তিক replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UErrorCode& status);
};

CharString&
AliasReplacer::generate_চাবি(
        ধ্রুবক অক্ষর* language, ধ্রুবক অক্ষর* region, ধ্রুবক অক্ষর* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    যদি (U_FAILURE(status)) { ফেরত out; }
    out.append(language, status);
    যদি (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    যদি (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    ফেরত out;
}

শূন্য
AliasReplacer::parseLanguageReplacement(
    ধ্রুবক অক্ষর* replacement,
    ধ্রুবক অক্ষর*& replacedLanguage,
    ধ্রুবক অক্ষর*& replacedScript,
    ধ্রুবক অক্ষর*& replacedRegion,
    ধ্রুবক অক্ষর*& replacedVariant,
    ধ্রুবক অক্ষর*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    যদি (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        ফেরত;
    }
    // We have multiple field so we have to allocate and parse
    auto str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    অক্ষর* তথ্য = str->তথ্য();
    toBeFreed.adoptElement(str.release(), status);
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    replacedLanguage = (ধ্রুবক অক্ষর*) তথ্য;
    অক্ষর* endOfField = uprv_strchr(তথ্য, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    ধ্রুবক অক্ষর* start = endOfField;
    endOfField = const_cast<অক্ষর*>(uprv_strchr(start, '_'));
    size_t len = 0;
    যদি (endOfField == nullptr) {
        len = uprv_strlen(start);
    } অন্যথা {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    যদি (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        যদি (endOfField == nullptr) {
            ফেরত;
        }
        start = endOfField++;
        endOfField = const_cast<অক্ষর*>(uprv_strchr(start, '_'));
        যদি (endOfField == nullptr) {
            len = uprv_strlen(start);
        } অন্যথা {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    যদি (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        যদি (endOfField == nullptr) {
            ফেরত;
        }
        start = endOfField++;
        endOfField = const_cast<অক্ষর*>(uprv_strchr(start, '_'));
        যদি (endOfField == nullptr) {
            len = uprv_strlen(start);
        } অন্যথা {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    যদি (len >= 4) {
        // Got a variant
        replacedVariant = start;
        যদি (endOfField == nullptr) {
            ফেরত;
        }
        start = endOfField++;
    }
    replacedExtensions = start;
}

যৌক্তিক
AliasReplacer::replaceLanguage(
        যৌক্তিক checkLanguage, যৌক্তিক checkRegion,
        যৌক্তিক checkVariants, UVector& toBeFreed, UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    যদি (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        ফেরত মিথ্যা;
    }
    int32_t variant_আকার = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ধ্রুবক অক্ষর* অনুসন্ধান_Language = checkLanguage ? language : "und";
    ধ্রুবক অক্ষর* অনুসন্ধান_Region = checkRegion ? region : nullptr;
    ধ্রুবক অক্ষর* অনুসন্ধান_Variant = nullptr;
    জন্য (int32_t variant_সূচক = 0;
            variant_সূচক < variant_আকার;
            variant_সূচক++) {
        যদি (checkVariants) {
            U_ASSERT(variant_সূচক < variant_আকার);
            অনুসন্ধান_Variant = static_cast<ধ্রুবক অক্ষর*>(variants.elementAt(variant_সূচক));
        }

        যদি (অনুসন্ধান_Variant != nullptr && uprv_strlen(অনুসন্ধান_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            অনুসন্ধান_Variant = nullptr;
        }
        CharString type_চাবি;
        generate_চাবি(অনুসন্ধান_Language, অনুসন্ধান_Region, অনুসন্ধান_Variant, type_চাবি,
                    status);
        যদি (U_FAILURE(status)) {
            ফেরত মিথ্যা;
        }
        ধ্রুবক অক্ষর *replacement = তথ্য->languageMap().get(type_চাবি.তথ্য());
        যদি (replacement == nullptr) {
            // Found no replacement data.
            চালু;
        }

        ধ্রুবক অক্ষর* replacedLanguage = nullptr;
        ধ্রুবক অক্ষর* replacedScript = nullptr;
        ধ্রুবক অক্ষর* replacedRegion = nullptr;
        ধ্রুবক অক্ষর* replacedVariant = nullptr;
        ধ্রুবক অক্ষর* replacedExtensions = nullptr;
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
        replacedRegion = deleteOrReplace(region, অনুসন্ধান_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            অনুসন্ধান_Variant, অনুসন্ধান_Variant, replacedVariant);

        যদি (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(অনুসন্ধান_Variant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            চালু;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        যদি (অনুসন্ধান_Variant != nullptr) {
            যদি (notEmpty(replacedVariant)) {
                variants.setElementAt((শূন্য*)replacedVariant, variant_সূচক);
            } অন্যথা {
                variants.removeElementAt(variant_সূচক);
            }
        }
        যদি (replacedExtensions != nullptr) {
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
        ফেরত সত্য;
    }
    // Nothing changed by language alias data.
    ফেরত মিথ্যা;
}

যৌক্তিক
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    যদি (region == nullptr) {
        // No region to search.
        ফেরত মিথ্যা;
    }
    ধ্রুবক অক্ষর *replacement = তথ্য->territoryMap().get(region);
    যদি (replacement == nullptr) {
        // Found no replacement data for this region.
        ফেরত মিথ্যা;
    }
    ধ্রুবক অক্ষর* replacedRegion = replacement;
    ধ্রুবক অক্ষর* firstSpace = uprv_strchr(replacement, ' ');
    যদি (firstSpace != nullptr) {
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
        ধ্রুবক অক্ষর* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        যদি (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ধ্রুবক অক্ষর* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            যদি (foundInReplacement != nullptr) {
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
        যদি (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        যদি (U_FAILURE(status)) { ফেরত মিথ্যা; }
        replacedRegion = item->তথ্য();
        toBeFreed.adoptElement(item.release(), status);
        যদি (U_FAILURE(status)) { ফেরত মিথ্যা; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    ফেরত সত্য;
}

যৌক্তিক
AliasReplacer::replaceScript(UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    যদি (script == nullptr) {
        // No script to search.
        ফেরত মিথ্যা;
    }
    ধ্রুবক অক্ষর *replacement = তথ্য->scriptMap().get(script);
    যদি (replacement == nullptr) {
        // Found no replacement data for this script.
        ফেরত মিথ্যা;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    ফেরত সত্য;
}

যৌক্তিক
AliasReplacer::replaceVariant(UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    // Since we may have more than one variant, we need to loop through them.
    জন্য (int32_t i = 0; i < variants.size(); i++) {
        ধ্রুবক অক্ষর* variant = static_cast<ধ্রুবক অক্ষর*>(variants.elementAt(i));
        ধ্রুবক অক্ষর *replacement = তথ্য->variantMap().get(variant);
        যদি (replacement == nullptr) {
            // Found no replacement data for this variant.
            চালু;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        যদি (!same(variant, replacement)) {
            variants.setElementAt((শূন্য*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            যদি (uprv_strcmp(variant, "heploc") == 0) {
                জন্য (int32_t j = 0; j < variants.size(); j++) {
                     যদি (uprv_strcmp((ধ্রুবক অক্ষর*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            ফেরত সত্য;
        }
    }
    ফেরত মিথ্যা;
}

যৌক্তিক
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক অক্ষর *replacement = তথ্য->subdivisionMap().get(subdivision.তথ্য());
    যদি (replacement != nullptr) {
        ধ্রুবক অক্ষর* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        যদি (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            যদি (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        ফেরত সত্য;
    }
    ফেরত মিথ্যা;
}

যৌক্তিক
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    int32_t len = transformedExtensions.length();
    ধ্রুবক অক্ষর* str = transformedExtensions.তথ্য();
    ধ্রুবক অক্ষর* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    যদি (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        যদি (U_FAILURE(status)) {
            ফেরত মিথ্যা;
        }
        T_CString_toLowerCase(output.তথ্য());
    }
    যদি (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        যদি (U_FAILURE(status)) {
            ফেরত মিথ্যা;
        }
        করো {
            ধ্রুবক অক্ষর* tvalue = uprv_strchr(tkey, '-');
            যদি (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ফেরত মিথ্যা;
            }
            ধ্রুবক অক্ষর* পরবর্তী_TKey = ultag_getTKeyStart(tvalue);
            যদি (পরবর্তী_TKey != nullptr) {
                *const_cast<অক্ষর*>(পরবর্তী_TKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.ঢোকাও_Element_At((শূন্য*)tkey, tfields.size(), status);
            যদি (U_FAILURE(status)) {
                ফেরত মিথ্যা;
            }
            tkey = পরবর্তী_TKey;
        } যখন (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            ফেরত uprv_strcmp((ধ্রুবক অক্ষর*)e1.pointer, (ধ্রুবক অক্ষর*)e2.pointer);
        }, status);
        জন্য (int32_t i = 0; i < tfields.size(); i++) {
             যদি (output.length() > 0) {
                 output.append('-', status);
             }
             ধ্রুবক অক্ষর* tfield = static_cast<ধ্রুবক অক্ষর*>(tfields.elementAt(i));
             ধ্রুবক অক্ষর* tvalue = uprv_strchr(tfield, '-');
             যদি (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 ফেরত মিথ্যা;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<অক্ষর*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    ফেরত সত্য;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    যদি (U_FAILURE(status)) { ফেরত out; }
    out.append(language, status);
    যদি (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    যদি (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    যদি (variants.size() > 0) {
        যদি (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            ফেরত uprv_strcmp((ধ্রুবক অক্ষর*)e1.pointer, (ধ্রুবক অক্ষর*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        জন্য (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<ধ্রুবক অক্ষর*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.তথ্য() + variantsStart);
    }
    যদি (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.তথ্য());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    ফেরত out;
}

যৌক্তিক
AliasReplacer::replace(ধ্রুবক Locale& locale, CharString& out, UErrorCode& status)
{
    তথ্য = Alias_তথ্য::singleton(status);
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    U_ASSERT(তথ্য != nullptr);
    out.clear();
    language = locale.getLanguage();
    যদি (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    যদি (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    যদি (!notEmpty(region)) {
        region = nullptr;
    }
    ধ্রুবক অক্ষর* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    যদি (!variantsBuff.isEmpty()) {
        যদি (U_FAILURE(status)) { ফেরত মিথ্যা; }
        অক্ষর* start = variantsBuff.তথ্য();
        T_CString_toLowerCase(start);
        অক্ষর* end;
        যখন ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            যদি (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        যদি (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    যদি (U_FAILURE(status)) { ফেরত মিথ্যা; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        ফেরত uprv_strcmp((ধ্রুবক অক্ষর*)e1.pointer, (ধ্রুবক অক্ষর*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    ধ্রুবক int32_t maxChanged = 4 + variants.size();
    (শূন্য)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](শূন্য *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    যখন (U_SUCCESS(status)) {
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
        যদি (    replaceLanguage(সত্য, সত্য,  সত্য,  stringsToBeFreed, status) ||
                replaceLanguage(সত্য, সত্য,  মিথ্যা, stringsToBeFreed, status) ||
                replaceLanguage(সত্য, মিথ্যা, সত্য,  stringsToBeFreed, status) ||
                replaceLanguage(সত্য, মিথ্যা, মিথ্যা, stringsToBeFreed, status) ||
                replaceLanguage(মিথ্যা,মিথ্যা, সত্য,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            চালু;
        }
        // Nothing changed. Break out.
        বিরতি;
    }  // while(1)

    যদি (U_FAILURE(status)) { ফেরত মিথ্যা; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ধ্রুবক অক্ষর* extensionsStr = locale_getKeywordsStart(locale.getName());
    যদি (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        ফেরত মিথ্যা;
    }
    outputToString(out, status);
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    যদি (extensionsStr != nullptr) {
        changed = 0;
        Locale অস্থায়ী(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        যদি (U_SUCCESS(status) && !iter.isNull()) {
            ধ্রুবক অক্ষর* চাবি;
            যখন ((চাবি = iter->next(nullptr, status)) != nullptr) {
                যদি (uprv_strcmp("sd", চাবি) == 0 || uprv_strcmp("rg", চাবি) == 0 ||
                        uprv_strcmp("t", চাবি) == 0) {
                    auto value = locale.get_Keyword_মান<CharString>(চাবি, status);
                    যদি (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        চালু;
                    }
                    CharString replacement;
                    যদি (uprv_strlen(চাবি) == 2) {
                        যদি (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            অস্থায়ী.set_Keyword_মান(চাবি, replacement.তথ্য(), status);
                        }
                    } অন্যথা {
                        U_ASSERT(uprv_strcmp(চাবি, "t") == 0);
                        যদি (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            অস্থায়ী.set_Keyword_মান(চাবি, replacement.তথ্য(), status);
                        }
                    }
                    যদি (U_FAILURE(status)) {
                        ফেরত মিথ্যা;
                    }
                }
            }
        }
        যদি (changed != 0) {
            extensionsStr = locale_getKeywordsStart(অস্থায়ী.getName());
        }
        out.append(extensionsStr, status);
    }
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    // If the tag is not changed, return.
    যদি (uprv_strcmp(out.তথ্য(), locale.getName()) == 0) {
        out.clear();
        ফেরত মিথ্যা;
    }
    ফেরত সত্য;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
যৌক্তিক
canonicalizeLocale(ধ্রুবক Locale& locale, CharString& out, UErrorCode& status)
{
    যদি (U_FAILURE(status)) { ফেরত মিথ্যা; }
    AliasReplacer replacer(status);
    ফেরত replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
যৌক্তিক
isKnownCanonicalizedLocale(ধ্রুবক অক্ষর* locale, UErrorCode& status)
{
    যদি (U_FAILURE(status)) { ফেরত মিথ্যা; }

    যদি (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        ফেরত সত্য;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    ফেরত uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT ধ্রুবক অক্ষর* ধ্রুবক*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    ফেরত KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT যৌক্তিক
ulocimp_isCanonicalizedLocaleForTest(ধ্রুবক অক্ষর* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString অস্থায়ী;
    ফেরত !canonicalizeLocale(l, অস্থায়ী, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ধ্রুবক অক্ষর* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    ফেরত localeID == nullptr ? *এই = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    করো {
        অক্ষর *separator;
        অক্ষর *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        ধ্রুবক auto parse = [canonicalize](std::string_view localeID,
                                          অক্ষর* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            ফেরত ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    যদি (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } অন্যথা {
                        ulocimp_getName(localeID, sink, status);
                    }
                },
                status);
        };

        // "canonicalize" the locale ID to ICU/Java format
        অক্ষর* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, আকার Nest::baseName, err);

        FixedString full_Name_বাফার;
        যদি (err == U_বাফার_OVERFLOW_ERROR || length >= static_cast<int32_t>(আকার Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            যদি (!full_Name_বাফার.reserve(length + 1)) {
                বিরতি; // error: out of memory
            }
            fullName = full_Name_বাফার.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        যদি(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            বিরতি;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        অক্ষর* at = uprv_strchr(fullName, '@');
        যখন ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        অক্ষর* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        যদি (separator!=nullptr || sep2!=nullptr) {
            যদি (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } অন্যথা {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        যৌক্তিক hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        যদি (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            বিরতি; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        যদি (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        যদি (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        যদি (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } অন্যথা যদি (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        যদি (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } অন্যথা যদি (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        যদি (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(full_Name_বাফার.isEmpty());
            nest.init(language, script, region, variantBegin);
        } অন্যথা {
            যদি (fullName == nest.baseName) {
                U_ASSERT(full_Name_বাফার.isEmpty());
                full_Name_বাফার = {fullName, static_cast<std::string_view::size_type>(length)};
                যদি (full_Name_বাফার.isEmpty()) {
                    বিরতি; // error: out of memory
                }
                যদি (!language.empty()) {
                    language = {full_Name_বাফার.তথ্য(), language.size()};
                }
                যদি (!script.empty()) {
                    script = {full_Name_বাফার.তথ্য() + (script.তথ্য() - fullName), script.size()};
                }
                যদি (!region.empty()) {
                    region = {full_Name_বাফার.তথ্য() + (region.তথ্য() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            যদি (isBogus()) {
                বিরতি; // error: out of memory
            }
            U_ASSERT(!full_Name_বাফার.isEmpty());
            heap.ptr->fullName = std::move(full_Name_বাফার);
            যদি (hasKeywords) {
                যদি (std::string_view::size_type base_Name_দৈর্ঘ্য = at - fullName; base_Name_দৈর্ঘ্য > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.তথ্য(), base_Name_দৈর্ঘ্য};
                    যদি (heap.ptr->baseName.isEmpty()) {
                        বিরতি; // error: out of memory
                    }
                }
            }
        }

        যদি (canonicalize) {
            যদি (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                যদি (canonicalizeLocale(*এই, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.তথ্য(), মিথ্যা);
                }
                যদি (U_FAILURE(err)) {
                    বিরতি;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        ফেরত *এই;
    } যখন(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    ফেরত *এই;
}

int32_t
Locale::hashCode() ধ্রুবক
{
    ফেরত ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

শূন্য
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

ধ্রুবক Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        যদি (gDefaultLocale != nullptr) {
            ফেরত *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    ফেরত *locale_set_default_internal(nullptr, status);
}



শূন্য U_EXPORT2
Locale::setDefault( ধ্রুবক   Locale&     newLocale,
                            UErrorCode&  status)
{
    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ধ্রুবক অক্ষর *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

শূন্য
Locale::addLikelySubtags(UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    যদি (U_FAILURE(status)) {
        যদি (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ফেরত;
    }

    init(maximizedLocaleID.তথ্য(), /*canonicalize=*/মিথ্যা);
    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

শূন্য
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(মিথ্যা, status);
}
শূন্য
Locale::minimizeSubtags(যৌক্তিক favorScript, UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    যদি (U_FAILURE(status)) {
        যদি (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ফেরত;
    }

    init(minimizedLocaleID.তথ্য(), /*canonicalize=*/মিথ্যা);
    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

শূন্য
Locale::canonicalize(UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    CharString uncanonicalized(getName(), status);
    যদি (U_FAILURE(status)) {
        যদি (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ফেরত;
    }
    init(uncanonicalized.তথ্য(), /*canonicalize=*/সত্য);
    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale ফলাফল(Locale::eBOGUS);

    যদি (U_FAILURE(status)) {
        ফেরত ফলাফল;
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

    int32_t parsed_দৈর্ঘ্য;
    CharString localeID = ulocimp_forLanguageTag(
            tag.তথ্য(),
            tag.length(),
            &parsed_দৈর্ঘ্য,
            status);

    যদি (U_FAILURE(status)) {
        ফেরত ফলাফল;
    }

    যদি (parsed_দৈর্ঘ্য != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত ফলাফল;
    }

    ফলাফল.init(localeID.তথ্য(), /*canonicalize=*/মিথ্যা);
    যদি (ফলাফল.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ফেরত ফলাফল;
}

শূন্য
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) ধ্রুবক
{
    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/মিথ্যা, status);
}

Locale U_EXPORT2
Locale::createFromName (ধ্রুবক অক্ষর *name)
{
    যদি (name) {
        Locale l("");
        l.init(name, মিথ্যা);
        ফেরত l;
    }
    অন্যথা {
        ফেরত getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, মিথ্যা);
    ফেরত loc;
}

Locale U_EXPORT2
Locale::createCanonical(ধ্রুবক অক্ষর* name) {
    Locale loc("");
    loc.init(name, সত্য);
    ফেরত loc;
}

ধ্রুবক অক্ষর *
Locale::getISO3Language() ধ্রুবক
{
    ফেরত uloc_getISO3Language(getName());
}


ধ্রুবক অক্ষর *
Locale::getISO3Country() ধ্রুবক
{
    ফেরত uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() ধ্রুবক
{
    ফেরত uloc_getLCID(getName());
}

ধ্রুবক অক্ষর* ধ্রুবক* U_EXPORT2 Locale::getISOCountries()
{
    ফেরত uloc_getISOCountries();
}

ধ্রুবক অক্ষর* ধ্রুবক* U_EXPORT2 Locale::getISOLanguages()
{
    ফেরত uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
শূন্য Locale::setFromPOSIXID(ধ্রুবক অক্ষর *posixID)
{
    init(posixID, সত্য);
}

ধ্রুবক Locale & U_EXPORT2
Locale::get_মূল()
{
    ফেরত getLocale(e_মূল);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getEnglish()
{
    ফেরত getLocale(eENGLISH);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getFrench()
{
    ফেরত getLocale(eFRENCH);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getGerman()
{
    ফেরত getLocale(eGERMAN);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getItalian()
{
    ফেরত getLocale(eITALIAN);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getJapanese()
{
    ফেরত getLocale(eJAPANESE);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getKorean()
{
    ফেরত getLocale(eKOREAN);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getChinese()
{
    ফেরত getLocale(eCHINESE);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    ফেরত getLocale(eCHINA);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    ফেরত getLocale(eTAIWAN);
}


ধ্রুবক Locale & U_EXPORT2
Locale::getFrance()
{
    ফেরত getLocale(eFRANCE);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getGermany()
{
    ফেরত getLocale(eGERMANY);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getItaly()
{
    ফেরত getLocale(eITALY);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getJapan()
{
    ফেরত getLocale(eJAPAN);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getKorea()
{
    ফেরত getLocale(eKOREA);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getChina()
{
    ফেরত getLocale(eCHINA);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getPRC()
{
    ফেরত getLocale(eCHINA);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getTaiwan()
{
    ফেরত getLocale(eTAIWAN);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getUK()
{
    ফেরত getLocale(eUK);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getUS()
{
    ফেরত getLocale(eUS);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getCanada()
{
    ফেরত getLocale(eCANADA);
}

ধ্রুবক Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    ফেরত getLocale(eCANADA_FRENCH);
}

ধ্রুবক Locale &
Locale::getLocale(পূর্ণসংখ্যা locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    যদি (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    ফেরত localeCache[locid]; /*operating on nullptr*/
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
    ফেরত gLocaleCache;
}

শ্রেণী KeywordEnumeration : সর্বজনীন StringEnumeration {
সুরক্ষিত:
    FixedString keywords;
ব্যক্তিগত:
    int32_t length;
    ধ্রুবক অক্ষর *current;
    স্থির ধ্রুবক অক্ষর fgClassID;

সর্বজনীন:
    স্থির UClassID U_EXPORT2 getStaticClassID() { ফেরত (UClassID)&fgClassID; }
    আভাসী UClassID getDynamicClassID() ধ্রুবক override { ফেরত getStaticClassID(); }
সর্বজনীন:
    KeywordEnumeration(ধ্রুবক অক্ষর *চাবিগুলি, int32_t keywordLen, int32_t current_সূচক, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        যদি(U_SUCCESS(status) && keywordLen != 0) {
            যদি(চাবিগুলি == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } অন্যথা {
                keywords = {চাবিগুলি, static_cast<std::string_view::size_type>(length)};
                যদি (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } অন্যথা {
                    current = keywords.তথ্য() + current_সূচক;
                }
            }
        }
    }

    আভাসী ~KeywordEnumeration();

    আভাসী StringEnumeration * clone() ধ্রুবক override
    {
        UErrorCode status = U_ZERO_ERROR;
        ফেরত নতুন KeywordEnumeration(
                keywords.তথ্য(), length,
                static_cast<int32_t>(current - keywords.তথ্য()), status);
    }

    আভাসী int32_t count(UErrorCode& status) ধ্রুবক override {
        যদি (U_FAILURE(status)) { ফেরত 0; }
        ধ্রুবক অক্ষর *kw = keywords.তথ্য();
        int32_t ফলাফল = 0;
        যখন(*kw) {
            ফলাফল++;
            kw += uprv_strlen(kw)+1;
        }
        ফেরত ফলাফল;
    }

    আভাসী ধ্রুবক অক্ষর* next(int32_t* ফলাফল_দৈর্ঘ্য, UErrorCode& status) U_LIFETIME_BOUND override {
        ধ্রুবক অক্ষর* ফলাফল;
        int32_t len;
        যদি(U_SUCCESS(status) && *current != 0) {
            ফলাফল = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            যদি(ফলাফল_দৈর্ঘ্য != nullptr) {
                *ফলাফল_দৈর্ঘ্য = len;
            }
        } অন্যথা {
            যদি(ফলাফল_দৈর্ঘ্য != nullptr) {
                *ফলাফল_দৈর্ঘ্য = 0;
            }
            ফলাফল = nullptr;
        }
        ফেরত ফলাফল;
    }

    আভাসী ধ্রুবক UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        যদি (U_FAILURE(status)) { ফেরত nullptr; }
        int32_t ফলাফল_দৈর্ঘ্য = 0;
        ধ্রুবক অক্ষর *s = next(&ফলাফল_দৈর্ঘ্য, status);
        ফেরত setChars(s, ফলাফল_দৈর্ঘ্য, status);
    }

    আভাসী শূন্য reset(UErrorCode& status) override {
        যদি (U_FAILURE(status)) { ফেরত; }
        current = keywords.তথ্য();
    }
};

ধ্রুবক অক্ষর KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = পূর্বনির্ধারিত;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
শ্রেণী UnicodeKeywordEnumeration : সর্বজনীন KeywordEnumeration {
সর্বজনীন:
    using KeywordEnumeration::KeywordEnumeration;
    আভাসী ~UnicodeKeywordEnumeration();

    আভাসী ধ্রুবক অক্ষর* next(int32_t* ফলাফল_দৈর্ঘ্য, UErrorCode& status) U_LIFETIME_BOUND override {
        ধ্রুবক অক্ষর* legacy_চাবি = KeywordEnumeration::next(nullptr, status);
        যখন (U_SUCCESS(status) && legacy_চাবি != nullptr) {
            ধ্রুবক অক্ষর* চাবি = uloc_to_Unicode_Locale_চাবি(legacy_চাবি);
            যদি (চাবি != nullptr) {
                যদি (ফলাফল_দৈর্ঘ্য != nullptr) {
                    *ফলাফল_দৈর্ঘ্য = static_cast<int32_t>(uprv_strlen(চাবি));
                }
                ফেরত চাবি;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_চাবি = KeywordEnumeration::next(nullptr, status);
        }
        যদি (ফলাফল_দৈর্ঘ্য != nullptr) *ফলাফল_দৈর্ঘ্য = 0;
        ফেরত nullptr;
    }
    আভাসী int32_t count(UErrorCode& status) ধ্রুবক override {
        যদি (U_FAILURE(status)) { ফেরত 0; }
        ধ্রুবক অক্ষর *kw = keywords.তথ্য();
        int32_t ফলাফল = 0;
        যখন(*kw) {
            যদি (uloc_to_Unicode_Locale_চাবি(kw) != nullptr) {
                ফলাফল++;
            }
            kw += uprv_strlen(kw)+1;
        }
        ফেরত ফলাফল;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = পূর্বনির্ধারিত;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) ধ্রুবক
{
    StringEnumeration *ফলাফল = nullptr;

    যদি (U_FAILURE(status)) {
        ফেরত ফলাফল;
    }

    ধ্রুবক অক্ষর* variantStart = uprv_strchr(getName(), '@');
    ধ্রুবক অক্ষর* assignment = uprv_strchr(getName(), '=');
    যদি(variantStart) {
        যদি(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', মিথ্যা, status);
            যদি (U_SUCCESS(status) && !keywords.isEmpty()) {
                ফলাফল = নতুন KeywordEnumeration(keywords.তথ্য(), keywords.length(), 0, status);
                যদি (!ফলাফল) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } অন্যথা {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ফেরত ফলাফল;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) ধ্রুবক
{
    StringEnumeration *ফলাফল = nullptr;

    যদি (U_FAILURE(status)) {
        ফেরত ফলাফল;
    }

    ধ্রুবক অক্ষর* variantStart = uprv_strchr(getName(), '@');
    ধ্রুবক অক্ষর* assignment = uprv_strchr(getName(), '=');
    যদি(variantStart) {
        যদি(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', মিথ্যা, status);
            যদি (U_SUCCESS(status) && !keywords.isEmpty()) {
                ফলাফল = নতুন UnicodeKeywordEnumeration(keywords.তথ্য(), keywords.length(), 0, status);
                যদি (!ফলাফল) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } অন্যথা {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ফেরত ফলাফল;
}

int32_t
Locale::get_Keyword_মান(ধ্রুবক অক্ষর* keywordName, অক্ষর *বাফার, int32_t bufLen, UErrorCode &status) ধ্রুবক
{
    ফেরত uloc_get_Keyword_মান(getName(), keywordName, বাফার, bufLen, &status);
}

শূন্য
Locale::get_Keyword_মান(StringPiece keywordName, ByteSink& sink, UErrorCode& status) ধ্রুবক {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    ulocimp_get_Keyword_মান(getName(), keywordName, sink, status);
}

শূন্য
Locale::get_Unicode_Keyword_মান(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) ধ্রুবক {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    std::optional<std::string_view> legacy_চাবি = ulocimp_to_Legacy_চাবি_With_Fallback(keywordName);
    যদি (!legacy_চাবি.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    auto legacy_মান = get_Keyword_মান<CharString>(*legacy_চাবি, status);

    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    std::optional<std::string_view> unicode_মান =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_মান.toStringPiece());
    যদি (!unicode_মান.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    sink.Append(unicode_মান->তথ্য(), static_cast<int32_t>(unicode_মান->size()));
}

শূন্য
Locale::set_Keyword_মান(StringPiece keywordName,
                        StringPiece keyword_মান,
                        UErrorCode& status) {
    যদি (U_FAILURE(status)) { ফেরত; }
    যদি (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    যদি (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_set_Keyword_মান(keywordName, keyword_মান, localeID, status);
    যদি (U_FAILURE(status)) {
        যদি (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ফেরত;
    }

    ধ্রুবক অক্ষর* at = locale_getKeywordsStart(localeID.toStringPiece());
    যৌক্তিক hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    যদি (!hasKeywords) {
        যদি (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            যদি (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < আকার Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, আকার Nest::baseName, status);
            } অন্যথা {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                যদি (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ফেরত;
                }
            }
        }
    } অন্যথা {
        Heap* heap = nullptr;
        যদি (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            যদি (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
        } অন্যথা {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        যদি (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            ফেরত;
        }

        যদি (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            যদি (std::string_view::size_type base_Name_দৈর্ঘ্য = at - localeID.তথ্য(); base_Name_দৈর্ঘ্য > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.তথ্য(), base_Name_দৈর্ঘ্য};
                যদি (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ফেরত;
                }
            }
        }
    }
}

শূন্য
Locale::set_Unicode_Keyword_মান(StringPiece keywordName,
                               StringPiece keyword_মান,
                               UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    std::optional<std::string_view> legacy_চাবি = ulocimp_to_Legacy_চাবি_With_Fallback(keywordName);
    যদি (!legacy_চাবি.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    std::string_view value;

    যদি (!keyword_মান.empty()) {
        std::optional<std::string_view> legacy_মান =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_মান);
        যদি (!legacy_মান.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত;
        }
        value = *legacy_মান;
    }

    set_Keyword_মান(*legacy_চাবি, value, status);
}

ধ্রুবক অক্ষর*
Locale::getCountry() ধ্রুবক U_LIFETIME_BOUND {
    ফেরত getField<&Nest::getRegion, &Heap::getRegion>();
}

ধ্রুবক অক্ষর*
Locale::getLanguage() ধ্রুবক U_LIFETIME_BOUND {
    ফেরত getField<&Nest::getLanguage, &Heap::getLanguage>();
}

ধ্রুবক অক্ষর*
Locale::getScript() ধ্রুবক U_LIFETIME_BOUND {
    ফেরত getField<&Nest::getScript, &Heap::getScript>();
}

ধ্রুবক অক্ষর*
Locale::getVariant() ধ্রুবক U_LIFETIME_BOUND {
    ফেরত getField<&Nest::getVariant, &Heap::getVariant>();
}

ধ্রুবক অক্ষর*
Locale::getName() ধ্রুবক U_LIFETIME_BOUND {
    ফেরত getField<&Nest::getBaseName, &Heap::getFullName>();
}

ধ্রুবক অক্ষর*
Locale::getBaseName() ধ্রুবক U_LIFETIME_BOUND {
    ফেরত getField<&Nest::getBaseName, &Heap::getBaseName>();
}

template <ধ্রুবক অক্ষর* (Locale::Nest::*ধ্রুবক NEST)() ধ্রুবক,
          ধ্রুবক অক্ষর* (Locale::Heap::*ধ্রুবক HEAP)() ধ্রুবক>
ধ্রুবক অক্ষর* Locale::getField() ধ্রুবক U_LIFETIME_BOUND {
    ফেরত payload.visit([] { ফেরত ""; },
                         [](ধ্রুবক Nest& nest) { ফেরত (nest.*NEST)(); },
                         [](ধ্রুবক Heap& heap) { ফেরত (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = পূর্বনির্ধারিত;

//eof
U_NAMESPACE_END
