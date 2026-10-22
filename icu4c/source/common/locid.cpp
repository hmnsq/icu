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

#include <cstddef>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

#include "unicode/bytestream.h"
#include "unicode/locid.h"
#include "unicode/localebuilder.h"
#include "unicode/localpointer.h"
#include "unicode/strenum.h"
#include "unicode/stringpiece.h"
#include "unicode/uloc.h"
#include "unicode/ures.h"

#include "bytesinkutil.h"
#include "charstr.h"
#include "charstrmap.h"
#include "cmemory.h"
#include "cstring.h"
#include "fixedstring.h"
#include "mutex.h"
#include "putilimp.h"
#include "uassert.h"
#include "ucln_cmn.h"
#include "uhash.h"
#include "ulocimp.h"
#include "umutex.h"
#include "uniquecharstr.h"
#include "ustr_imp.h"
#include "uvector.h"

#ຖ້າ UPRV_HAS_FEATURE(memory_sanitizer)
#include <sanitizer/msan_interface.h>
#endif

U_NAMESPACE_BEGIN

static Locale   *gLocaleCache = nullptr;
static UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
static UMutex gDefaultLocaleMutex;
static UHashtable *gDefaultLocalesHashT = nullptr;
static Locale *gDefaultLocale = nullptr;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#define ULOC_STRING_LIMIT 357913941

U_NAMESPACE_END

typedef ga_nanaa ELocalePos {
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

namespace {

//
// Deleter function for Locales owned by the default Locale hash table/
//
void U_CALLCONV
deleteLocale(void *obj) {
    delete static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    delete [] gLocaleCache;
    gLocaleCache = nullptr;
    gLocaleCacheInitOnce.reset();

    ຖ້າ (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    ສົ່ງຄືນ true;
}

void U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = new Locale[static_cast<ຈຳນວນເຕັມ>(eMAX_LOCALES)];
    ຖ້າ (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ;
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

Locale *locale_set_default_internal(const var_na *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = false;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    ຖ້າ (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = true; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ gDefaultLocale;
    }

    ຖ້າ (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        ຖ້າ (U_FAILURE(status)) {
            ສົ່ງຄືນ gDefaultLocale;
        }
        uhash_setValueDeleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    ຖ້າ (newDefault == nullptr) {
        newDefault = new Locale(Locale::eBOGUS);
        ຖ້າ (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), false);
        uhash_put(gDefaultLocalesHashT, const_cast<var_na*>(newDefault->getName()), newDefault, &status);
        ຖ້າ (U_FAILURE(status)) {
            ສົ່ງຄືນ gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    ສົ່ງຄືນ gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC void
locale_set_default(const var_na *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC const var_na *
locale_get_default()
{
    U_NAMESPACE_USE
    ສົ່ງຄືນ Locale::getDefault().getName();
}

namespace {

template <auto FIELD, typename T>
void copyToArray(std::string_view sv, T* that) {
    auto& field = that->*FIELD;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    ຖ້າ (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.data(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

void Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, this);
    copyToArray<&Nest::script>(script, this);
    copyToArray<&Nest::region>(region, this);
    this->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(Heap, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(Heap, region));
    U_ASSERT(this == reinterpret_cast<Nest*>(&heap));
#ຖ້າ UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, parimaa_na(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, this);
    copyToArray<&Nest::region>(heap.region, this);
    this->variantBegin = variantBegin;
    *this->baseName = '\0';
}

sa_nracanaa Locale::Heap::Alloc : public UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    const var_na* getVariant() const U_LIFETIME_BOUND {
        ສົ່ງຄືນ variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    const var_na* getFullName() const U_LIFETIME_BOUND { ສົ່ງຄືນ fullName.data(); }
    const var_na* getBaseName() const U_LIFETIME_BOUND {
        ຖ້າ (baseName.isEmpty()) {
            ຖ້າ (const var_na* name = fullName.data(); *name != '@') {
                ສົ່ງຄືນ name;
            }
        }
        ສົ່ງຄືນ baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(const Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        ຖ້າ (U_SUCCESS(status)) {
            ຖ້າ (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                ຖ້າ (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ບໍ່ດັ່ງນັ້ນ {
                    ຖ້າ (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        ຖ້າ (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = delete;

    ~Alloc() = default;
};

const var_na* Locale::Heap::getVariant() const U_LIFETIME_BOUND { ສົ່ງຄືນ ptr->getVariant(); }
const var_na* Locale::Heap::getFullName() const U_LIFETIME_BOUND { ສົ່ງຄືນ ptr->getFullName(); }
const var_na* Locale::Heap::getBaseName() const U_LIFETIME_BOUND { ສົ່ງຄືນ ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = new Alloc(variantBegin);
    ຖ້າ (ptr == nullptr) {
        type = eBOGUS;
    } ບໍ່ດັ່ງນັ້ນ {
        type = eHEAP;
        copyToArray<&Heap::language>(language, this);
        copyToArray<&Heap::script>(script, this);
        copyToArray<&Heap::region>(region, this);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::operator=(const Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = new Alloc(*other.ptr, status);
    ຖ້າ (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } ບໍ່ດັ່ງນັ້ນ {
        type = eHEAP;
        uprv_memcpy(language, other.language, parimaa_na language);
        uprv_memcpy(script, other.script, parimaa_na script);
        uprv_memcpy(region, other.region, parimaa_na region);
    }
    ສົ່ງຄືນ *this;
}

Locale::Heap& Locale::Heap::operator=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, parimaa_na language);
    uprv_memcpy(script, other.script, parimaa_na script);
    uprv_memcpy(region, other.region, parimaa_na region);
    ສົ່ງຄືນ *this;
}

template <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
auto Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) const {
    switch (type) {
        pak_sa eBOGUS:
            ສົ່ງຄືນ bogusFn(args...);
        pak_sa eNEST:
            ສົ່ງຄືນ nestFn(nest, args...);
        pak_sa eHEAP:
            ສົ່ງຄືນ heapFn(heap, args...);
        default:
            UPRV_UNREACHABLE_EXIT;
    };
}

void Locale::Payload::copy(const Payload& other) {
    other.visit([](Payload*) {},
                [](const Nest& nest, Payload* dst) { dst->nest = nest; },
                [](const Heap& heap, Payload* dst) { dst->heap = heap; },
                this);
}

void Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](const Nest& nest, Payload* dst) { dst->nest = nest; },
        [](const Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        this);
}

Locale::Payload::~Payload() {
    ຖ້າ (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(const Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::operator=(const Payload& other) U_LIFETIME_BOUND {
    ຖ້າ (this != &other) {
        setToBogus();
        copy(other);
    }
    ສົ່ງຄືນ *this;
}

Locale::Payload& Locale::Payload::operator=(Payload&& other) noexcept U_LIFETIME_BOUND {
    ຖ້າ (this != &other) {
        setToBogus();
        move(std::move(other));
    }
    ສົ່ງຄືນ *this;
}

void Locale::Payload::setToBogus() {
    this->~Payload();
    type = eBOGUS;
}

template <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    ຖ້າ constexpr (std::is_same_v<T, Nest>) {
        this->~Payload();
        ::new (&nest) Nest(std::forward<Args>(args)...);
        ສົ່ງຄືນ nest;
    }
    ຖ້າ constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::new (&heap) Heap(std::forward<Args>(args)...);
        ສົ່ງຄືນ heap;
    }
}

template <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    ສົ່ງຄືນ type == eNEST ? &nest : nullptr;
}
template <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    ສົ່ງຄືນ type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = default;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, false);
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


Locale::Locale( const   var_na * newLanguage,
                const   var_na * newCountry,
                const   var_na * newVariant,
                const   var_na * newKeywords)
    : UObject(), payload()
{
    ຖ້າ( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, false); /* shortcut */
    }
    ບໍ່ດັ່ງນັ້ນ
    {
        UErrorCode status = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        ຖ້າ ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            ຖ້າ ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                ສົ່ງຄືນ;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        ຖ້າ ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            ຖ້າ ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                ສົ່ງຄືນ;
            }
        }

        // _Variant
        ຖ້າ ( newVariant != nullptr )
        {
            // remove leading _'s
            ໃນຂະນະທີ່(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            ຖ້າ ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                ສົ່ງຄືນ;
            }
            ໃນຂະນະທີ່( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        ຖ້າ ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            ຖ້າ ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              ສົ່ງຄືນ;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        ຖ້າ ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        ຖ້າ ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        ຖ້າ ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        ຖ້າ ( ksize != 0)
        {
            ຖ້າ (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            ບໍ່ດັ່ງນັ້ນ {
                togo.append('_', status); /* Variant parsing with a script */
                ຖ້າ ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        ຖ້າ (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            ສົ່ງຄືນ;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), false);
    }
}

Locale::Locale(const Locale&) = default;
Locale::Locale(Locale&&) noexcept = default;

Locale& Locale::operator=(const Locale&) U_LIFETIME_BOUND = default;
Locale& Locale::operator=(Locale&&) noexcept U_LIFETIME_BOUND = default;

Locale *
Locale::clone() const {
    ສົ່ງຄືນ new Locale(*this);
}

bool
Locale::operator==( const   Locale& other) const
{
    ສົ່ງຄືນ uprv_strcmp(other.getName(), getName()) == 0;
}

namespace {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr const var_na* KNOWN_CANONICALIZED[] = {
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
    ຖ້າ (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    ສົ່ງຄືນ true;
}

void U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    ສຳລັບ (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (void*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

class AliasData;

/**
 * A Builder class to build the alias data.
 */
class AliasDataBuilder {
public:
    AliasDataBuilder() {
    }

    // Build the AliasData from resource.
    AliasData* build(UErrorCode &status);

private:
    void readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<const var_na*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   void (*checkType)(const var_na* type),
                   void (*checkReplacement)(const UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    void readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<const var_na*>& types,
                           LocalMemory<int32_t>& replacementIndexes,
                           int32_t &length,
                           UErrorCode &status);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    void readScriptAlias(UResourceBundle* alias,
                         UniqueCharStrings* strings,
                         LocalMemory<const var_na*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    void readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<const var_na*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    void readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<const var_na*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    void readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<const var_na*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
class AliasData : public UMemory {
public:
    static const AliasData* singleton(UErrorCode& status) {
        ຖ້າ (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            ສົ່ງຄືນ nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        ສົ່ງຄືນ gSingleton;
    }

    const CharStringMap& languageMap() const U_LIFETIME_BOUND { ສົ່ງຄືນ language; }
    const CharStringMap& scriptMap() const U_LIFETIME_BOUND { ສົ່ງຄືນ script; }
    const CharStringMap& territoryMap() const U_LIFETIME_BOUND { ສົ່ງຄືນ territory; }
    const CharStringMap& variantMap() const U_LIFETIME_BOUND { ສົ່ງຄືນ variant; }
    const CharStringMap& subdivisionMap() const U_LIFETIME_BOUND { ສົ່ງຄືນ subdivision; }

    static void U_CALLCONV loadData(UErrorCode &status);
    static UBool U_CALLCONV cleanup();

    static UInitOnce gInitOnce;

private:
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

    static const AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    friend class AliasDataBuilder;
};


const AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    ສົ່ງຄືນ true;
}

void
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<const var_na*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        void (*checkType)(const var_na* type),
        void (*checkReplacement)(const UChar* replacement),
        UErrorCode &status) {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }
    length = ures_getSize(alias);
    const var_na** rawTypes = types.allocateInsteadAndCopy(length);
    ຖ້າ (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    ຖ້າ (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ;
    }
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        const var_na* aliasFrom = ures_getKey(res.getAlias());
        const UChar* aliasTo =
            ures_getStringByKey(res.getAlias(), "replacement", nullptr, &status);
        ຖ້າ (U_FAILURE(status)) ສົ່ງຄືນ;

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
void
AliasDataBuilder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<const var_na*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ສົ່ງຄືນ readAlias(
        alias, strings, types, replacementIndexes, length,
#ຖ້າ U_DEBUG
        [](const var_na* type) {
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
#ບໍ່ດັ່ງນັ້ນ
        [](const var_na*) {},
#endif
        [](const UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
void
AliasDataBuilder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<const var_na*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ສົ່ງຄືນ readAlias(
        alias, strings, types, replacementIndexes, length,
#ຖ້າ U_DEBUG
        [](const var_na* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](const UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#ບໍ່ດັ່ງນັ້ນ
        [](const var_na*) {},
        [](const UChar*) { },
#endif
        status);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
void
AliasDataBuilder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<const var_na*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ສົ່ງຄືນ readAlias(
        alias, strings, types, replacementIndexes, length,
#ຖ້າ U_DEBUG
        [](const var_na* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#ບໍ່ດັ່ງນັ້ນ
        [](const var_na*) {},
#endif
        [](const UChar*) { },
        status);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
void
AliasDataBuilder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<const var_na*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ສົ່ງຄືນ readAlias(
        alias, strings, types, replacementIndexes, length,
#ຖ້າ U_DEBUG
        [](const var_na* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](const UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#ບໍ່ດັ່ງນັ້ນ
        [](const var_na*) {},
        [](const UChar*) { },
#endif
        status);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
void
AliasDataBuilder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<const var_na*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ສົ່ງຄືນ readAlias(
        alias, strings, types, replacementIndexes, length,
#ຖ້າ U_DEBUG
        [](const var_na* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#ບໍ່ດັ່ງນັ້ນ
        [](const var_na*) {},
#endif
        [](const UChar*) { },
        status);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
void U_CALLCONV
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
    ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ nullptr; }

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

    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<const var_na*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<const var_na*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<const var_na*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<const var_na*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<const var_na*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    ສຳລັບ (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    ສຳລັບ (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    ສຳລັບ (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    ສຳລັບ (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    ສຳລັບ (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ nullptr;
    }

    // copy hashtables
    auto *data = new AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    ຖ້າ (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ສົ່ງຄືນ data;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
class AliasReplacer {
public:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       ສົ່ງຄືນ 0==uprv_strcmp((const var_na*)e1.pointer,
                                             (const var_na*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    bool replace(
        const Locale& locale, CharString& out, UErrorCode& status);

private:
    const var_na* language;
    const var_na* script;
    const var_na* region;
    const var_na* extensions;
    UVector variants;

    const AliasData* data;

    anta_hstha bool notEmpty(const var_na* str) {
        ສົ່ງຄືນ str && str[0] != NULL_CHAR;
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
    anta_hstha const var_na* deleteOrReplace(const var_na* input U_LIFETIME_BOUND,
                                       const var_na* type,
                                       const var_na* replacement U_LIFETIME_BOUND) {
        ສົ່ງຄືນ notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    anta_hstha bool same(const var_na* a, const var_na* b) {
        ຖ້າ (a == nullptr && b == nullptr) {
            ສົ່ງຄືນ true;
        }
        ຖ້າ ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          ສົ່ງຄືນ false;
        }
        ສົ່ງຄືນ uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generateKey(const var_na* language, const var_na* region,
                            const var_na* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    void parseLanguageReplacement(const var_na* replacement,
                                  const var_na*& replaceLanguage,
                                  const var_na*& replaceScript,
                                  const var_na*& replaceRegion,
                                  const var_na*& replaceVariant,
                                  const var_na*& replaceExtensions,
                                  UVector& toBeFreed,
                                  UErrorCode& status);

    // Replace by using languageAlias.
    bool replaceLanguage(bool checkLanguage, bool checkRegion,
                         bool checkVariants, UVector& toBeFreed,
                         UErrorCode& status);

    // Replace by using territoryAlias.
    bool replaceTerritory(UVector& toBeFreed, UErrorCode& status);

    // Replace by using scriptAlias.
    bool replaceScript(UErrorCode& status);

    // Replace by using variantAlias.
    bool replaceVariant(UErrorCode& status);

    // Replace by using subdivisionAlias.
    bool replaceSubdivision(StringPiece subdivision,
                            CharString& output, UErrorCode& status);

    // Replace transformed extensions.
    bool replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UErrorCode& status);
};

CharString&
AliasReplacer::generateKey(
        const var_na* language, const var_na* region, const var_na* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ out; }
    out.append(language, status);
    ຖ້າ (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    ຖ້າ (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    ສົ່ງຄືນ out;
}

void
AliasReplacer::parseLanguageReplacement(
    const var_na* replacement,
    const var_na*& replacedLanguage,
    const var_na*& replacedScript,
    const var_na*& replacedRegion,
    const var_na*& replacedVariant,
    const var_na*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    ຖ້າ (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        ສົ່ງຄືນ;
    }
    // We have multiple field so we have to allocate and parse
    auto str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    var_na* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }
    replacedLanguage = (const var_na*) data;
    var_na* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    const var_na* start = endOfField;
    endOfField = const_cast<var_na*>(uprv_strchr(start, '_'));
    size_t len = 0;
    ຖ້າ (endOfField == nullptr) {
        len = uprv_strlen(start);
    } ບໍ່ດັ່ງນັ້ນ {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    ຖ້າ (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        ຖ້າ (endOfField == nullptr) {
            ສົ່ງຄືນ;
        }
        start = endOfField++;
        endOfField = const_cast<var_na*>(uprv_strchr(start, '_'));
        ຖ້າ (endOfField == nullptr) {
            len = uprv_strlen(start);
        } ບໍ່ດັ່ງນັ້ນ {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    ຖ້າ (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        ຖ້າ (endOfField == nullptr) {
            ສົ່ງຄືນ;
        }
        start = endOfField++;
        endOfField = const_cast<var_na*>(uprv_strchr(start, '_'));
        ຖ້າ (endOfField == nullptr) {
            len = uprv_strlen(start);
        } ບໍ່ດັ່ງນັ້ນ {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    ຖ້າ (len >= 4) {
        // Got a variant
        replacedVariant = start;
        ຖ້າ (endOfField == nullptr) {
            ສົ່ງຄືນ;
        }
        start = endOfField++;
    }
    replacedExtensions = start;
}

bool
AliasReplacer::replaceLanguage(
        bool checkLanguage, bool checkRegion,
        bool checkVariants, UVector& toBeFreed, UErrorCode& status)
{
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ false;
    }
    ຖ້າ (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        ສົ່ງຄືນ false;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    const var_na* searchLanguage = checkLanguage ? language : "und";
    const var_na* searchRegion = checkRegion ? region : nullptr;
    const var_na* searchVariant = nullptr;
    ສຳລັບ (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        ຖ້າ (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            searchVariant = static_cast<const var_na*>(variants.elementAt(variant_index));
        }

        ຖ້າ (searchVariant != nullptr && uprv_strlen(searchVariant) < 4) {
            // Do not consider  ill-formed variant subtag.
            searchVariant = nullptr;
        }
        CharString typeKey;
        generateKey(searchLanguage, searchRegion, searchVariant, typeKey,
                    status);
        ຖ້າ (U_FAILURE(status)) {
            ສົ່ງຄືນ false;
        }
        const var_na *replacement = data->languageMap().get(typeKey.data());
        ຖ້າ (replacement == nullptr) {
            // Found no replacement data.
            continue;
        }

        const var_na* replacedLanguage = nullptr;
        const var_na* replacedScript = nullptr;
        const var_na* replacedRegion = nullptr;
        const var_na* replacedVariant = nullptr;
        const var_na* replacedExtensions = nullptr;
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

        ຖ້າ (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(searchVariant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            continue;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        ຖ້າ (searchVariant != nullptr) {
            ຖ້າ (notEmpty(replacedVariant)) {
                variants.setElementAt((void*)replacedVariant, variant_index);
            } ບໍ່ດັ່ງນັ້ນ {
                variants.removeElementAt(variant_index);
            }
        }
        ຖ້າ (replacedExtensions != nullptr) {
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
        ສົ່ງຄືນ true;
    }
    // Nothing changed by language alias data.
    ສົ່ງຄືນ false;
}

bool
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ false;
    }
    ຖ້າ (region == nullptr) {
        // No region to search.
        ສົ່ງຄືນ false;
    }
    const var_na *replacement = data->territoryMap().get(region);
    ຖ້າ (replacement == nullptr) {
        // Found no replacement data for this region.
        ສົ່ງຄືນ false;
    }
    const var_na* replacedRegion = replacement;
    const var_na* firstSpace = uprv_strchr(replacement, ' ');
    ຖ້າ (firstSpace != nullptr) {
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
        const var_na* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        ຖ້າ (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            const var_na* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            ຖ້າ (foundInReplacement != nullptr) {
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
        ຖ້າ (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ false; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ false; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    ສົ່ງຄືນ true;
}

bool
AliasReplacer::replaceScript(UErrorCode& status)
{
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ false;
    }
    ຖ້າ (script == nullptr) {
        // No script to search.
        ສົ່ງຄືນ false;
    }
    const var_na *replacement = data->scriptMap().get(script);
    ຖ້າ (replacement == nullptr) {
        // Found no replacement data for this script.
        ສົ່ງຄືນ false;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    ສົ່ງຄືນ true;
}

bool
AliasReplacer::replaceVariant(UErrorCode& status)
{
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ false;
    }
    // Since we may have more than one variant, we need to loop through them.
    ສຳລັບ (int32_t i = 0; i < variants.size(); i++) {
        const var_na* variant = static_cast<const var_na*>(variants.elementAt(i));
        const var_na *replacement = data->variantMap().get(variant);
        ຖ້າ (replacement == nullptr) {
            // Found no replacement data for this variant.
            continue;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        ຖ້າ (!same(variant, replacement)) {
            variants.setElementAt((void*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            ຖ້າ (uprv_strcmp(variant, "heploc") == 0) {
                ສຳລັບ (int32_t j = 0; j < variants.size(); j++) {
                     ຖ້າ (uprv_strcmp((const var_na*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            ສົ່ງຄືນ true;
        }
    }
    ສົ່ງຄືນ false;
}

bool
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ false;
    }
    const var_na *replacement = data->subdivisionMap().get(subdivision.data());
    ຖ້າ (replacement != nullptr) {
        const var_na* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        ຖ້າ (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            ຖ້າ (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        ສົ່ງຄືນ true;
    }
    ສົ່ງຄືນ false;
}

bool
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ false;
    }
    int32_t len = transformedExtensions.length();
    const var_na* str = transformedExtensions.data();
    const var_na* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    ຖ້າ (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        ຖ້າ (U_FAILURE(status)) {
            ສົ່ງຄືນ false;
        }
        T_CString_toLowerCase(output.data());
    }
    ຖ້າ (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        ຖ້າ (U_FAILURE(status)) {
            ສົ່ງຄືນ false;
        }
        do {
            const var_na* tvalue = uprv_strchr(tkey, '-');
            ຖ້າ (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ສົ່ງຄືນ false;
            }
            const var_na* nextTKey = ultag_getTKeyStart(tvalue);
            ຖ້າ (nextTKey != nullptr) {
                *const_cast<var_na*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.insertElementAt((void*)tkey, tfields.size(), status);
            ຖ້າ (U_FAILURE(status)) {
                ສົ່ງຄືນ false;
            }
            tkey = nextTKey;
        } ໃນຂະນະທີ່ (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            ສົ່ງຄືນ uprv_strcmp((const var_na*)e1.pointer, (const var_na*)e2.pointer);
        }, status);
        ສຳລັບ (int32_t i = 0; i < tfields.size(); i++) {
             ຖ້າ (output.length() > 0) {
                 output.append('-', status);
             }
             const var_na* tfield = static_cast<const var_na*>(tfields.elementAt(i));
             const var_na* tvalue = uprv_strchr(tfield, '-');
             ຖ້າ (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 ສົ່ງຄືນ false;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<var_na*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ false;
    }
    ສົ່ງຄືນ true;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ out; }
    out.append(language, status);
    ຖ້າ (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    ຖ້າ (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    ຖ້າ (variants.size() > 0) {
        ຖ້າ (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            ສົ່ງຄືນ uprv_strcmp((const var_na*)e1.pointer, (const var_na*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        ສຳລັບ (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<const var_na*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    ຖ້າ (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    ສົ່ງຄືນ out;
}

bool
AliasReplacer::replace(const Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ false;
    }
    U_ASSERT(data != nullptr);
    out.clear();
    language = locale.getLanguage();
    ຖ້າ (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    ຖ້າ (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    ຖ້າ (!notEmpty(region)) {
        region = nullptr;
    }
    const var_na* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    ຖ້າ (!variantsBuff.isEmpty()) {
        ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ false; }
        var_na* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        var_na* end;
        ໃນຂະນະທີ່ ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            ຖ້າ (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        ຖ້າ (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ false; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        ສົ່ງຄືນ uprv_strcmp((const var_na*)e1.pointer, (const var_na*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    const int32_t maxChanged = 4 + variants.size();
    (void)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](void *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    ໃນຂະນະທີ່ (U_SUCCESS(status)) {
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
        ຖ້າ (    replaceLanguage(true, true,  true,  stringsToBeFreed, status) ||
                replaceLanguage(true, true,  false, stringsToBeFreed, status) ||
                replaceLanguage(true, false, true,  stringsToBeFreed, status) ||
                replaceLanguage(true, false, false, stringsToBeFreed, status) ||
                replaceLanguage(false,false, true,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            continue;
        }
        // Nothing changed. Break out.
        bha_nga;
    }  // while(1)

    ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ false; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    const var_na* extensionsStr = locale_getKeywordsStart(locale.getName());
    ຖ້າ (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        ສົ່ງຄືນ false;
    }
    outputToString(out, status);
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ false;
    }
    ຖ້າ (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        ຖ້າ (U_SUCCESS(status) && !iter.isNull()) {
            const var_na* key;
            ໃນຂະນະທີ່ ((key = iter->next(nullptr, status)) != nullptr) {
                ຖ້າ (uprv_strcmp("sd", key) == 0 || uprv_strcmp("rg", key) == 0 ||
                        uprv_strcmp("t", key) == 0) {
                    auto value = locale.getKeywordValue<CharString>(key, status);
                    ຖ້າ (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        continue;
                    }
                    CharString replacement;
                    ຖ້າ (uprv_strlen(key) == 2) {
                        ຖ້າ (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.setKeywordValue(key, replacement.data(), status);
                        }
                    } ບໍ່ດັ່ງນັ້ນ {
                        U_ASSERT(uprv_strcmp(key, "t") == 0);
                        ຖ້າ (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.setKeywordValue(key, replacement.data(), status);
                        }
                    }
                    ຖ້າ (U_FAILURE(status)) {
                        ສົ່ງຄືນ false;
                    }
                }
            }
        }
        ຖ້າ (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ false;
    }
    // If the tag is not changed, return.
    ຖ້າ (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        ສົ່ງຄືນ false;
    }
    ສົ່ງຄືນ true;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
bool
canonicalizeLocale(const Locale& locale, CharString& out, UErrorCode& status)
{
    ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ false; }
    AliasReplacer replacer(status);
    ສົ່ງຄືນ replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
bool
isKnownCanonicalizedLocale(const var_na* locale, UErrorCode& status)
{
    ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ false; }

    ຖ້າ (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        ສົ່ງຄືນ true;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ false;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    ສົ່ງຄືນ uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT const var_na* const*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    ສົ່ງຄືນ KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT bool
ulocimp_isCanonicalizedLocaleForTest(const var_na* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    ສົ່ງຄືນ !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(const var_na* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    ສົ່ງຄືນ localeID == nullptr ? *this = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    do {
        var_na *separator;
        var_na *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        const auto parse = [canonicalize](std::string_view localeID,
                                          var_na* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            ສົ່ງຄືນ ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    ຖ້າ (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } ບໍ່ດັ່ງນັ້ນ {
                        ulocimp_getName(localeID, sink, status);
                    }
                },
                status);
        };

        // "canonicalize" the locale ID to ICU/Java format
        var_na* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, parimaa_na Nest::baseName, err);

        FixedString fullNameBuffer;
        ຖ້າ (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(parimaa_na Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            ຖ້າ (!fullNameBuffer.reserve(length + 1)) {
                bha_nga; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        ຖ້າ(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            bha_nga;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        var_na* at = uprv_strchr(fullName, '@');
        ໃນຂະນະທີ່ ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        var_na* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        ຖ້າ (separator!=nullptr || sep2!=nullptr) {
            ຖ້າ (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } ບໍ່ດັ່ງນັ້ນ {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        bool hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        ຖ້າ (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            bha_nga; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        ຖ້າ (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        ຖ້າ (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        ຖ້າ (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        ຖ້າ (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        ຖ້າ (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } ບໍ່ດັ່ງນັ້ນ {
            ຖ້າ (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                ຖ້າ (fullNameBuffer.isEmpty()) {
                    bha_nga; // error: out of memory
                }
                ຖ້າ (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                ຖ້າ (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                ຖ້າ (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            ຖ້າ (isBogus()) {
                bha_nga; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            ຖ້າ (hasKeywords) {
                ຖ້າ (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    ຖ້າ (heap.ptr->baseName.isEmpty()) {
                        bha_nga; // error: out of memory
                    }
                }
            }
        }

        ຖ້າ (canonicalize) {
            ຖ້າ (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                ຖ້າ (canonicalizeLocale(*this, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), false);
                }
                ຖ້າ (U_FAILURE(err)) {
                    bha_nga;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        ສົ່ງຄືນ *this;
    } ໃນຂະນະທີ່(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    ສົ່ງຄືນ *this;
}

int32_t
Locale::hashCode() const
{
    ສົ່ງຄືນ ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

void
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

const Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        ຖ້າ (gDefaultLocale != nullptr) {
            ສົ່ງຄືນ *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    ສົ່ງຄືນ *locale_set_default_internal(nullptr, status);
}



void U_EXPORT2
Locale::setDefault( const   Locale&     newLocale,
                            UErrorCode&  status)
{
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    const var_na *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

void
Locale::addLikelySubtags(UErrorCode& status) {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    ຖ້າ (U_FAILURE(status)) {
        ຖ້າ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ສົ່ງຄືນ;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/false);
    ຖ້າ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

void
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(false, status);
}
void
Locale::minimizeSubtags(bool favorScript, UErrorCode& status) {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    ຖ້າ (U_FAILURE(status)) {
        ຖ້າ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ສົ່ງຄືນ;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/false);
    ຖ້າ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

void
Locale::canonicalize(UErrorCode& status) {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }
    ຖ້າ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }
    CharString uncanonicalized(getName(), status);
    ຖ້າ (U_FAILURE(status)) {
        ຖ້າ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ສົ່ງຄືນ;
    }
    init(uncanonicalized.data(), /*canonicalize=*/true);
    ຖ້າ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ result;
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

    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ result;
    }

    ຖ້າ (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ result;
    }

    result.init(localeID.data(), /*canonicalize=*/false);
    ຖ້າ (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ສົ່ງຄືນ result;
}

void
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) const
{
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }

    ຖ້າ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/false, status);
}

Locale U_EXPORT2
Locale::createFromName (const var_na *name)
{
    ຖ້າ (name) {
        Locale l("");
        l.init(name, false);
        ສົ່ງຄືນ l;
    }
    ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, false);
    ສົ່ງຄືນ loc;
}

Locale U_EXPORT2
Locale::createCanonical(const var_na* name) {
    Locale loc("");
    loc.init(name, true);
    ສົ່ງຄືນ loc;
}

const var_na *
Locale::getISO3Language() const
{
    ສົ່ງຄືນ uloc_getISO3Language(getName());
}


const var_na *
Locale::getISO3Country() const
{
    ສົ່ງຄືນ uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() const
{
    ສົ່ງຄືນ uloc_getLCID(getName());
}

const var_na* const* U_EXPORT2 Locale::getISOCountries()
{
    ສົ່ງຄືນ uloc_getISOCountries();
}

const var_na* const* U_EXPORT2 Locale::getISOLanguages()
{
    ສົ່ງຄືນ uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
void Locale::setFromPOSIXID(const var_na *posixID)
{
    init(posixID, true);
}

const Locale & U_EXPORT2
Locale::getRoot()
{
    ສົ່ງຄືນ getLocale(eROOT);
}

const Locale & U_EXPORT2
Locale::getEnglish()
{
    ສົ່ງຄືນ getLocale(eENGLISH);
}

const Locale & U_EXPORT2
Locale::getFrench()
{
    ສົ່ງຄືນ getLocale(eFRENCH);
}

const Locale & U_EXPORT2
Locale::getGerman()
{
    ສົ່ງຄືນ getLocale(eGERMAN);
}

const Locale & U_EXPORT2
Locale::getItalian()
{
    ສົ່ງຄືນ getLocale(eITALIAN);
}

const Locale & U_EXPORT2
Locale::getJapanese()
{
    ສົ່ງຄືນ getLocale(eJAPANESE);
}

const Locale & U_EXPORT2
Locale::getKorean()
{
    ສົ່ງຄືນ getLocale(eKOREAN);
}

const Locale & U_EXPORT2
Locale::getChinese()
{
    ສົ່ງຄືນ getLocale(eCHINESE);
}

const Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    ສົ່ງຄືນ getLocale(eCHINA);
}

const Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    ສົ່ງຄືນ getLocale(eTAIWAN);
}


const Locale & U_EXPORT2
Locale::getFrance()
{
    ສົ່ງຄືນ getLocale(eFRANCE);
}

const Locale & U_EXPORT2
Locale::getGermany()
{
    ສົ່ງຄືນ getLocale(eGERMANY);
}

const Locale & U_EXPORT2
Locale::getItaly()
{
    ສົ່ງຄືນ getLocale(eITALY);
}

const Locale & U_EXPORT2
Locale::getJapan()
{
    ສົ່ງຄືນ getLocale(eJAPAN);
}

const Locale & U_EXPORT2
Locale::getKorea()
{
    ສົ່ງຄືນ getLocale(eKOREA);
}

const Locale & U_EXPORT2
Locale::getChina()
{
    ສົ່ງຄືນ getLocale(eCHINA);
}

const Locale & U_EXPORT2
Locale::getPRC()
{
    ສົ່ງຄືນ getLocale(eCHINA);
}

const Locale & U_EXPORT2
Locale::getTaiwan()
{
    ສົ່ງຄືນ getLocale(eTAIWAN);
}

const Locale & U_EXPORT2
Locale::getUK()
{
    ສົ່ງຄືນ getLocale(eUK);
}

const Locale & U_EXPORT2
Locale::getUS()
{
    ສົ່ງຄືນ getLocale(eUS);
}

const Locale & U_EXPORT2
Locale::getCanada()
{
    ສົ່ງຄືນ getLocale(eCANADA);
}

const Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    ສົ່ງຄືນ getLocale(eCANADA_FRENCH);
}

const Locale &
Locale::getLocale(ຈຳນວນເຕັມ locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    ຖ້າ (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    ສົ່ງຄືນ localeCache[locid]; /*operating on nullptr*/
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
    ສົ່ງຄືນ gLocaleCache;
}

class KeywordEnumeration : public StringEnumeration {
rak_sita:
    FixedString keywords;
private:
    int32_t length;
    const var_na *current;
    static const var_na fgClassID;

public:
    static UClassID U_EXPORT2 getStaticClassID() { ສົ່ງຄືນ (UClassID)&fgClassID; }
    virtual UClassID getDynamicClassID() const override { ສົ່ງຄືນ getStaticClassID(); }
public:
    KeywordEnumeration(const var_na *keys, int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        ຖ້າ(U_SUCCESS(status) && keywordLen != 0) {
            ຖ້າ(keys == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } ບໍ່ດັ່ງນັ້ນ {
                keywords = {keys, static_cast<std::string_view::size_type>(length)};
                ຖ້າ (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ບໍ່ດັ່ງນັ້ນ {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    virtual ~KeywordEnumeration();

    virtual StringEnumeration * clone() const override
    {
        UErrorCode status = U_ZERO_ERROR;
        ສົ່ງຄືນ new KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    virtual int32_t count(UErrorCode& status) const override {
        ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ 0; }
        const var_na *kw = keywords.data();
        int32_t result = 0;
        ໃນຂະນະທີ່(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        ສົ່ງຄືນ result;
    }

    virtual const var_na* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        const var_na* result;
        int32_t len;
        ຖ້າ(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            ຖ້າ(resultLength != nullptr) {
                *resultLength = len;
            }
        } ບໍ່ດັ່ງນັ້ນ {
            ຖ້າ(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        ສົ່ງຄືນ result;
    }

    virtual const UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ nullptr; }
        int32_t resultLength = 0;
        const var_na *s = next(&resultLength, status);
        ສົ່ງຄືນ setChars(s, resultLength, status);
    }

    virtual void reset(UErrorCode& status) override {
        ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ; }
        current = keywords.data();
    }
};

const var_na KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = default;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
class UnicodeKeywordEnumeration : public KeywordEnumeration {
public:
    using KeywordEnumeration::KeywordEnumeration;
    virtual ~UnicodeKeywordEnumeration();

    virtual const var_na* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        const var_na* legacy_key = KeywordEnumeration::next(nullptr, status);
        ໃນຂະນະທີ່ (U_SUCCESS(status) && legacy_key != nullptr) {
            const var_na* key = uloc_toUnicodeLocaleKey(legacy_key);
            ຖ້າ (key != nullptr) {
                ຖ້າ (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(key));
                }
                ສົ່ງຄືນ key;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_key = KeywordEnumeration::next(nullptr, status);
        }
        ຖ້າ (resultLength != nullptr) *resultLength = 0;
        ສົ່ງຄືນ nullptr;
    }
    virtual int32_t count(UErrorCode& status) const override {
        ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ 0; }
        const var_na *kw = keywords.data();
        int32_t result = 0;
        ໃນຂະນະທີ່(*kw) {
            ຖ້າ (uloc_toUnicodeLocaleKey(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        ສົ່ງຄືນ result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = default;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) const
{
    StringEnumeration *result = nullptr;

    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ result;
    }

    const var_na* variantStart = uprv_strchr(getName(), '@');
    const var_na* assignment = uprv_strchr(getName(), '=');
    ຖ້າ(variantStart) {
        ຖ້າ(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', false, status);
            ຖ້າ (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = new KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                ຖ້າ (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } ບໍ່ດັ່ງນັ້ນ {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ສົ່ງຄືນ result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) const
{
    StringEnumeration *result = nullptr;

    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ result;
    }

    const var_na* variantStart = uprv_strchr(getName(), '@');
    const var_na* assignment = uprv_strchr(getName(), '=');
    ຖ້າ(variantStart) {
        ຖ້າ(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', false, status);
            ຖ້າ (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = new UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                ຖ້າ (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } ບໍ່ດັ່ງນັ້ນ {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ສົ່ງຄືນ result;
}

int32_t
Locale::getKeywordValue(const var_na* keywordName, var_na *buffer, int32_t bufLen, UErrorCode &status) const
{
    ສົ່ງຄືນ uloc_getKeywordValue(getName(), keywordName, buffer, bufLen, &status);
}

void
Locale::getKeywordValue(StringPiece keywordName, ByteSink& sink, UErrorCode& status) const {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }

    ຖ້າ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }

    ulocimp_getKeywordValue(getName(), keywordName, sink, status);
}

void
Locale::getUnicodeKeywordValue(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) const {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }

    std::optional<std::string_view> legacy_key = ulocimp_toLegacyKeyWithFallback(keywordName);
    ຖ້າ (!legacy_key.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }

    auto legacy_value = getKeywordValue<CharString>(*legacy_key, status);

    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }

    std::optional<std::string_view> unicode_value =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_value.toStringPiece());
    ຖ້າ (!unicode_value.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }

    sink.Append(unicode_value->data(), static_cast<int32_t>(unicode_value->size()));
}

void
Locale::setKeywordValue(StringPiece keywordName,
                        StringPiece keywordValue,
                        UErrorCode& status) {
    ຖ້າ (U_FAILURE(status)) { ສົ່ງຄືນ; }
    ຖ້າ (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }
    ຖ້າ (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_setKeywordValue(keywordName, keywordValue, localeID, status);
    ຖ້າ (U_FAILURE(status)) {
        ຖ້າ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ສົ່ງຄືນ;
    }

    const var_na* at = locale_getKeywordsStart(localeID.toStringPiece());
    bool hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    ຖ້າ (!hasKeywords) {
        ຖ້າ (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            ຖ້າ (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < parimaa_na Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, parimaa_na Nest::baseName, status);
            } ບໍ່ດັ່ງນັ້ນ {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                ຖ້າ (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ສົ່ງຄືນ;
                }
            }
        }
    } ບໍ່ດັ່ງນັ້ນ {
        Heap* heap = nullptr;
        ຖ້າ (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            ຖ້າ (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ສົ່ງຄືນ;
            }
        } ບໍ່ດັ່ງນັ້ນ {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        ຖ້າ (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            ສົ່ງຄືນ;
        }

        ຖ້າ (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            ຖ້າ (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                ຖ້າ (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ສົ່ງຄືນ;
                }
            }
        }
    }
}

void
Locale::setUnicodeKeywordValue(StringPiece keywordName,
                               StringPiece keywordValue,
                               UErrorCode& status) {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }

    std::optional<std::string_view> legacy_key = ulocimp_toLegacyKeyWithFallback(keywordName);
    ຖ້າ (!legacy_key.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
    }

    std::string_view value;

    ຖ້າ (!keywordValue.empty()) {
        std::optional<std::string_view> legacy_value =
            ulocimp_toLegacyTypeWithFallback(keywordName, keywordValue);
        ຖ້າ (!legacy_value.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ສົ່ງຄືນ;
        }
        value = *legacy_value;
    }

    setKeywordValue(*legacy_key, value, status);
}

const var_na*
Locale::getCountry() const U_LIFETIME_BOUND {
    ສົ່ງຄືນ getField<&Nest::getRegion, &Heap::getRegion>();
}

const var_na*
Locale::getLanguage() const U_LIFETIME_BOUND {
    ສົ່ງຄືນ getField<&Nest::getLanguage, &Heap::getLanguage>();
}

const var_na*
Locale::getScript() const U_LIFETIME_BOUND {
    ສົ່ງຄືນ getField<&Nest::getScript, &Heap::getScript>();
}

const var_na*
Locale::getVariant() const U_LIFETIME_BOUND {
    ສົ່ງຄືນ getField<&Nest::getVariant, &Heap::getVariant>();
}

const var_na*
Locale::getName() const U_LIFETIME_BOUND {
    ສົ່ງຄືນ getField<&Nest::getBaseName, &Heap::getFullName>();
}

const var_na*
Locale::getBaseName() const U_LIFETIME_BOUND {
    ສົ່ງຄືນ getField<&Nest::getBaseName, &Heap::getBaseName>();
}

template <const var_na* (Locale::Nest::*const NEST)() const,
          const var_na* (Locale::Heap::*const HEAP)() const>
const var_na* Locale::getField() const U_LIFETIME_BOUND {
    ສົ່ງຄືນ payload.visit([] { ສົ່ງຄືນ ""; },
                         [](const Nest& nest) { ສົ່ງຄືນ (nest.*NEST)(); },
                         [](const Heap& heap) { ສົ່ງຄືນ (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = default;

//eof
U_NAMESPACE_END
