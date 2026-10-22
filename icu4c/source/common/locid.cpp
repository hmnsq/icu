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

#اگر UPRV_HAS_FEATURE(memory_sanitizer)
#شامل <sanitizer/msan_interface.h>
#endif

U_NAMESPACE_BEGIN

ایستا Locale   *gLocaleCache = nullptr;
ایستا UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
ایستا UMutex gDefaultLocaleMutex;
ایستا UHashtable *gDefaultLocalesHashT = nullptr;
ایستا Locale *gDefaultLocale = nullptr;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#define ULOC_STRING_LIMIT 357913941

U_NAMESPACE_END

تعریفنوع شمارش ELocalePos {
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

فضاینام {

//
// Deleter function for Locales owned by the default Locale hash table/
//
تهی U_CALLCONV
deleteLocale(تهی *obj) {
    delete static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    delete [] gLocaleCache;
    gLocaleCache = nullptr;
    gLocaleCacheInitOnce.reset();

    اگر (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    بازگشت درست;
}

تهی U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = جدید Locale[static_cast<صحیح>(eMAX_LOCALES)];
    اگر (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
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

Locale *locale_set_default_internal(ثابت نویسه *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = نادرست;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    اگر (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = درست; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    اگر (U_FAILURE(status)) {
        بازگشت gDefaultLocale;
    }

    اگر (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        اگر (U_FAILURE(status)) {
            بازگشت gDefaultLocale;
        }
        uhash_set_مقدار_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    اگر (newDefault == nullptr) {
        newDefault = جدید Locale(Locale::eBOGUS);
        اگر (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), نادرست);
        uhash_put(gDefaultLocalesHashT, const_cast<نویسه*>(newDefault->getName()), newDefault, &status);
        اگر (U_FAILURE(status)) {
            بازگشت gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    بازگشت gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC تهی
locale_set_default(ثابت نویسه *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC ثابت نویسه *
locale_get_default()
{
    U_NAMESPACE_USE
    بازگشت Locale::getDefault().getName();
}

فضاینام {

template <auto FIELD, typename T>
تهی copyToArray(std::string_view sv, T* that) {
    auto& field = that->*FIELD;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    اگر (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.data(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

تهی Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, این);
    copyToArray<&Nest::script>(script, این);
    copyToArray<&Nest::region>(region, این);
    این->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(Heap, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(Heap, region));
    U_ASSERT(این == reinterpret_cast<Nest*>(&heap));
#اگر UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, اندازه(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, این);
    copyToArray<&Nest::region>(heap.region, این);
    این->variantBegin = variantBegin;
    *این->baseName = '\0';
}

ساختار Locale::Heap::Alloc : عمومی UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    ثابت نویسه* getVariant() ثابت U_LIFETIME_BOUND {
        بازگشت variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    ثابت نویسه* getFullName() ثابت U_LIFETIME_BOUND { بازگشت fullName.data(); }
    ثابت نویسه* getBaseName() ثابت U_LIFETIME_BOUND {
        اگر (baseName.isEmpty()) {
            اگر (ثابت نویسه* name = fullName.data(); *name != '@') {
                بازگشت name;
            }
        }
        بازگشت baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(ثابت Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        اگر (U_SUCCESS(status)) {
            اگر (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                اگر (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } وگرنه {
                    اگر (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        اگر (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = delete;

    ~Alloc() = پیشفرض;
};

ثابت نویسه* Locale::Heap::getVariant() ثابت U_LIFETIME_BOUND { بازگشت ptr->getVariant(); }
ثابت نویسه* Locale::Heap::getFullName() ثابت U_LIFETIME_BOUND { بازگشت ptr->getFullName(); }
ثابت نویسه* Locale::Heap::getBaseName() ثابت U_LIFETIME_BOUND { بازگشت ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = جدید Alloc(variantBegin);
    اگر (ptr == nullptr) {
        type = eBOGUS;
    } وگرنه {
        type = eHEAP;
        copyToArray<&Heap::language>(language, این);
        copyToArray<&Heap::script>(script, این);
        copyToArray<&Heap::region>(region, این);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::operator=(ثابت Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = جدید Alloc(*other.ptr, status);
    اگر (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } وگرنه {
        type = eHEAP;
        uprv_memcpy(language, other.language, اندازه language);
        uprv_memcpy(script, other.script, اندازه script);
        uprv_memcpy(region, other.region, اندازه region);
    }
    بازگشت *این;
}

Locale::Heap& Locale::Heap::operator=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, اندازه language);
    uprv_memcpy(script, other.script, اندازه script);
    uprv_memcpy(region, other.region, اندازه region);
    بازگشت *این;
}

template <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
auto Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) ثابت {
    انتخاب (type) {
        حالت eBOGUS:
            بازگشت bogusFn(args...);
        حالت eNEST:
            بازگشت nestFn(nest, args...);
        حالت eHEAP:
            بازگشت heapFn(heap, args...);
        پیشفرض:
            UPRV_UNREACHABLE_EXIT;
    };
}

تهی Locale::Payload::copy(ثابت Payload& other) {
    other.visit([](Payload*) {},
                [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ثابت Heap& heap, Payload* dst) { dst->heap = heap; },
                این);
}

تهی Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](ثابت Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ثابت Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        این);
}

Locale::Payload::~Payload() {
    اگر (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(ثابت Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::operator=(ثابت Payload& other) U_LIFETIME_BOUND {
    اگر (این != &other) {
        setToBogus();
        copy(other);
    }
    بازگشت *این;
}

Locale::Payload& Locale::Payload::operator=(Payload&& other) noexcept U_LIFETIME_BOUND {
    اگر (این != &other) {
        setToBogus();
        move(std::move(other));
    }
    بازگشت *این;
}

تهی Locale::Payload::setToBogus() {
    این->~Payload();
    type = eBOGUS;
}

template <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    اگر constexpr (std::is_same_v<T, Nest>) {
        این->~Payload();
        ::جدید (&nest) Nest(std::forward<Args>(args)...);
        بازگشت nest;
    }
    اگر constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::جدید (&heap) Heap(std::forward<Args>(args)...);
        بازگشت heap;
    }
}

template <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    بازگشت type == eNEST ? &nest : nullptr;
}
template <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    بازگشت type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = پیشفرض;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, نادرست);
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


Locale::Locale( ثابت   نویسه * newLanguage,
                ثابت   نویسه * newCountry,
                ثابت   نویسه * newVariant,
                ثابت   نویسه * newKeywords)
    : UObject(), payload()
{
    اگر( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, نادرست); /* shortcut */
    }
    وگرنه
    {
        UErrorCode status = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        اگر ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            اگر ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                بازگشت;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        اگر ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            اگر ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                بازگشت;
            }
        }

        // _Variant
        اگر ( newVariant != nullptr )
        {
            // remove leading _'s
            وقتی(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            اگر ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                بازگشت;
            }
            وقتی( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        اگر ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            اگر ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              بازگشت;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        اگر ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        اگر ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        اگر ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        اگر ( ksize != 0)
        {
            اگر (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            وگرنه {
                togo.append('_', status); /* Variant parsing with a script */
                اگر ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        اگر (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            بازگشت;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), نادرست);
    }
}

Locale::Locale(ثابت Locale&) = پیشفرض;
Locale::Locale(Locale&&) noexcept = پیشفرض;

Locale& Locale::operator=(ثابت Locale&) U_LIFETIME_BOUND = پیشفرض;
Locale& Locale::operator=(Locale&&) noexcept U_LIFETIME_BOUND = پیشفرض;

Locale *
Locale::clone() ثابت {
    بازگشت جدید Locale(*این);
}

منطقی
Locale::operator==( ثابت   Locale& other) ثابت
{
    بازگشت uprv_strcmp(other.getName(), getName()) == 0;
}

فضاینام {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr ثابت نویسه* KNOWN_CANONICALIZED[] = {
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
    اگر (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    بازگشت درست;
}

تهی U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    برای (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (تهی*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

رده AliasData;

/**
 * A Builder class to build the alias data.
 */
رده AliasDataBuilder {
عمومی:
    AliasDataBuilder() {
    }

    // Build the AliasData from resource.
    AliasData* build(UErrorCode &status);

خصوصی:
    تهی readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<ثابت نویسه*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   تهی (*checkType)(ثابت نویسه* type),
                   تهی (*checkReplacement)(ثابت UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    تهی readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<ثابت نویسه*>& types,
                           LocalMemory<int32_t>& replacementIndexes,
                           int32_t &length,
                           UErrorCode &status);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    تهی readScriptAlias(UResourceBundle* alias,
                         UniqueCharStrings* strings,
                         LocalMemory<ثابت نویسه*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    تهی readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<ثابت نویسه*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    تهی readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ثابت نویسه*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    تهی readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ثابت نویسه*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
رده AliasData : عمومی UMemory {
عمومی:
    ایستا ثابت AliasData* singleton(UErrorCode& status) {
        اگر (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            بازگشت nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        بازگشت gSingleton;
    }

    ثابت CharStringMap& languageMap() ثابت U_LIFETIME_BOUND { بازگشت language; }
    ثابت CharStringMap& scriptMap() ثابت U_LIFETIME_BOUND { بازگشت script; }
    ثابت CharStringMap& territoryMap() ثابت U_LIFETIME_BOUND { بازگشت territory; }
    ثابت CharStringMap& variantMap() ثابت U_LIFETIME_BOUND { بازگشت variant; }
    ثابت CharStringMap& subdivisionMap() ثابت U_LIFETIME_BOUND { بازگشت subdivision; }

    ایستا تهی U_CALLCONV loadData(UErrorCode &status);
    ایستا UBool U_CALLCONV cleanup();

    ایستا UInitOnce gInitOnce;

خصوصی:
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

    ایستا ثابت AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    friend رده AliasDataBuilder;
};


ثابت AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    بازگشت درست;
}

تهی
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت نویسه*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        تهی (*checkType)(ثابت نویسه* type),
        تهی (*checkReplacement)(ثابت UChar* replacement),
        UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    length = ures_getSize(alias);
    ثابت نویسه** rawTypes = types.allocateInsteadAndCopy(length);
    اگر (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    اگر (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    برای (صحیح i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        ثابت نویسه* aliasFrom = ures_get_کلید(res.getAlias());
        ثابت UChar* aliasTo =
            ures_get_String_By_کلید(res.getAlias(), "replacement", nullptr, &status);
        اگر (U_FAILURE(status)) بازگشت;

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
تهی
AliasDataBuilder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت نویسه*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    بازگشت readAlias(
        alias, strings, types, replacementIndexes, length,
#اگر U_DEBUG
        [](ثابت نویسه* type) {
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
#وگرنه
        [](ثابت نویسه*) {},
#endif
        [](ثابت UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
تهی
AliasDataBuilder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت نویسه*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    بازگشت readAlias(
        alias, strings, types, replacementIndexes, length,
#اگر U_DEBUG
        [](ثابت نویسه* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ثابت UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#وگرنه
        [](ثابت نویسه*) {},
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
تهی
AliasDataBuilder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت نویسه*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    بازگشت readAlias(
        alias, strings, types, replacementIndexes, length,
#اگر U_DEBUG
        [](ثابت نویسه* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#وگرنه
        [](ثابت نویسه*) {},
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
تهی
AliasDataBuilder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت نویسه*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    بازگشت readAlias(
        alias, strings, types, replacementIndexes, length,
#اگر U_DEBUG
        [](ثابت نویسه* type) {
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
#وگرنه
        [](ثابت نویسه*) {},
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
تهی
AliasDataBuilder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ثابت نویسه*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    بازگشت readAlias(
        alias, strings, types, replacementIndexes, length,
#اگر U_DEBUG
        [](ثابت نویسه* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#وگرنه
        [](ثابت نویسه*) {},
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
تهی U_CALLCONV
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
    اگر (U_FAILURE(status)) { بازگشت nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_کلید(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_کلید(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<ثابت نویسه*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<ثابت نویسه*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<ثابت نویسه*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<ثابت نویسه*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<ثابت نویسه*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    برای (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    برای (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    برای (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    برای (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    برای (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }

    // copy hashtables
    auto *data = جدید AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    اگر (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    بازگشت data;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
رده AliasReplacer {
عمومی:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       بازگشت 0==uprv_strcmp((ثابت نویسه*)e1.pointer,
                                             (ثابت نویسه*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    منطقی replace(
        ثابت Locale& locale, CharString& out, UErrorCode& status);

خصوصی:
    ثابت نویسه* language;
    ثابت نویسه* script;
    ثابت نویسه* region;
    ثابت نویسه* extensions;
    UVector variants;

    ثابت AliasData* data;

    درونخطی منطقی notEmpty(ثابت نویسه* str) {
        بازگشت str && str[0] != NULL_CHAR;
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
    درونخطی ثابت نویسه* deleteOrReplace(ثابت نویسه* input U_LIFETIME_BOUND,
                                       ثابت نویسه* type,
                                       ثابت نویسه* replacement U_LIFETIME_BOUND) {
        بازگشت notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    درونخطی منطقی same(ثابت نویسه* a, ثابت نویسه* b) {
        اگر (a == nullptr && b == nullptr) {
            بازگشت درست;
        }
        اگر ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          بازگشت نادرست;
        }
        بازگشت uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_کلید(ثابت نویسه* language, ثابت نویسه* region,
                            ثابت نویسه* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    تهی parseLanguageReplacement(ثابت نویسه* replacement,
                                  ثابت نویسه*& replaceLanguage,
                                  ثابت نویسه*& replaceScript,
                                  ثابت نویسه*& replaceRegion,
                                  ثابت نویسه*& replaceVariant,
                                  ثابت نویسه*& replaceExtensions,
                                  UVector& toBeFreed,
                                  UErrorCode& status);

    // Replace by using languageAlias.
    منطقی replaceLanguage(منطقی checkLanguage, منطقی checkRegion,
                         منطقی checkVariants, UVector& toBeFreed,
                         UErrorCode& status);

    // Replace by using territoryAlias.
    منطقی replaceTerritory(UVector& toBeFreed, UErrorCode& status);

    // Replace by using scriptAlias.
    منطقی replaceScript(UErrorCode& status);

    // Replace by using variantAlias.
    منطقی replaceVariant(UErrorCode& status);

    // Replace by using subdivisionAlias.
    منطقی replaceSubdivision(StringPiece subdivision,
                            CharString& output, UErrorCode& status);

    // Replace transformed extensions.
    منطقی replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UErrorCode& status);
};

CharString&
AliasReplacer::generate_کلید(
        ثابت نویسه* language, ثابت نویسه* region, ثابت نویسه* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    اگر (U_FAILURE(status)) { بازگشت out; }
    out.append(language, status);
    اگر (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    اگر (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    بازگشت out;
}

تهی
AliasReplacer::parseLanguageReplacement(
    ثابت نویسه* replacement,
    ثابت نویسه*& replacedLanguage,
    ثابت نویسه*& replacedScript,
    ثابت نویسه*& replacedRegion,
    ثابت نویسه*& replacedVariant,
    ثابت نویسه*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    اگر (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        بازگشت;
    }
    // We have multiple field so we have to allocate and parse
    auto str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    نویسه* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    replacedLanguage = (ثابت نویسه*) data;
    نویسه* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    ثابت نویسه* start = endOfField;
    endOfField = const_cast<نویسه*>(uprv_strchr(start, '_'));
    size_t len = 0;
    اگر (endOfField == nullptr) {
        len = uprv_strlen(start);
    } وگرنه {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    اگر (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        اگر (endOfField == nullptr) {
            بازگشت;
        }
        start = endOfField++;
        endOfField = const_cast<نویسه*>(uprv_strchr(start, '_'));
        اگر (endOfField == nullptr) {
            len = uprv_strlen(start);
        } وگرنه {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    اگر (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        اگر (endOfField == nullptr) {
            بازگشت;
        }
        start = endOfField++;
        endOfField = const_cast<نویسه*>(uprv_strchr(start, '_'));
        اگر (endOfField == nullptr) {
            len = uprv_strlen(start);
        } وگرنه {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    اگر (len >= 4) {
        // Got a variant
        replacedVariant = start;
        اگر (endOfField == nullptr) {
            بازگشت;
        }
        start = endOfField++;
    }
    replacedExtensions = start;
}

منطقی
AliasReplacer::replaceLanguage(
        منطقی checkLanguage, منطقی checkRegion,
        منطقی checkVariants, UVector& toBeFreed, UErrorCode& status)
{
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    اگر (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        بازگشت نادرست;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ثابت نویسه* جستجو_Language = checkLanguage ? language : "und";
    ثابت نویسه* جستجو_Region = checkRegion ? region : nullptr;
    ثابت نویسه* جستجو_Variant = nullptr;
    برای (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        اگر (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            جستجو_Variant = static_cast<ثابت نویسه*>(variants.elementAt(variant_index));
        }

        اگر (جستجو_Variant != nullptr && uprv_strlen(جستجو_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            جستجو_Variant = nullptr;
        }
        CharString type_کلید;
        generate_کلید(جستجو_Language, جستجو_Region, جستجو_Variant, type_کلید,
                    status);
        اگر (U_FAILURE(status)) {
            بازگشت نادرست;
        }
        ثابت نویسه *replacement = data->languageMap().get(type_کلید.data());
        اگر (replacement == nullptr) {
            // Found no replacement data.
            ادامه;
        }

        ثابت نویسه* replacedLanguage = nullptr;
        ثابت نویسه* replacedScript = nullptr;
        ثابت نویسه* replacedRegion = nullptr;
        ثابت نویسه* replacedVariant = nullptr;
        ثابت نویسه* replacedExtensions = nullptr;
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
        replacedRegion = deleteOrReplace(region, جستجو_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            جستجو_Variant, جستجو_Variant, replacedVariant);

        اگر (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(جستجو_Variant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            ادامه;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        اگر (جستجو_Variant != nullptr) {
            اگر (notEmpty(replacedVariant)) {
                variants.setElementAt((تهی*)replacedVariant, variant_index);
            } وگرنه {
                variants.removeElementAt(variant_index);
            }
        }
        اگر (replacedExtensions != nullptr) {
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
        بازگشت درست;
    }
    // Nothing changed by language alias data.
    بازگشت نادرست;
}

منطقی
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    اگر (region == nullptr) {
        // No region to search.
        بازگشت نادرست;
    }
    ثابت نویسه *replacement = data->territoryMap().get(region);
    اگر (replacement == nullptr) {
        // Found no replacement data for this region.
        بازگشت نادرست;
    }
    ثابت نویسه* replacedRegion = replacement;
    ثابت نویسه* firstSpace = uprv_strchr(replacement, ' ');
    اگر (firstSpace != nullptr) {
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
        ثابت نویسه* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        اگر (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ثابت نویسه* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            اگر (foundInReplacement != nullptr) {
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
        اگر (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        اگر (U_FAILURE(status)) { بازگشت نادرست; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        اگر (U_FAILURE(status)) { بازگشت نادرست; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    بازگشت درست;
}

منطقی
AliasReplacer::replaceScript(UErrorCode& status)
{
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    اگر (script == nullptr) {
        // No script to search.
        بازگشت نادرست;
    }
    ثابت نویسه *replacement = data->scriptMap().get(script);
    اگر (replacement == nullptr) {
        // Found no replacement data for this script.
        بازگشت نادرست;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    بازگشت درست;
}

منطقی
AliasReplacer::replaceVariant(UErrorCode& status)
{
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    // Since we may have more than one variant, we need to loop through them.
    برای (int32_t i = 0; i < variants.size(); i++) {
        ثابت نویسه* variant = static_cast<ثابت نویسه*>(variants.elementAt(i));
        ثابت نویسه *replacement = data->variantMap().get(variant);
        اگر (replacement == nullptr) {
            // Found no replacement data for this variant.
            ادامه;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        اگر (!same(variant, replacement)) {
            variants.setElementAt((تهی*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            اگر (uprv_strcmp(variant, "heploc") == 0) {
                برای (int32_t j = 0; j < variants.size(); j++) {
                     اگر (uprv_strcmp((ثابت نویسه*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            بازگشت درست;
        }
    }
    بازگشت نادرست;
}

منطقی
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    ثابت نویسه *replacement = data->subdivisionMap().get(subdivision.data());
    اگر (replacement != nullptr) {
        ثابت نویسه* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        اگر (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            اگر (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        بازگشت درست;
    }
    بازگشت نادرست;
}

منطقی
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    int32_t len = transformedExtensions.length();
    ثابت نویسه* str = transformedExtensions.data();
    ثابت نویسه* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    اگر (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        اگر (U_FAILURE(status)) {
            بازگشت نادرست;
        }
        T_CString_toLowerCase(output.data());
    }
    اگر (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        اگر (U_FAILURE(status)) {
            بازگشت نادرست;
        }
        انجامبده {
            ثابت نویسه* tvalue = uprv_strchr(tkey, '-');
            اگر (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                بازگشت نادرست;
            }
            ثابت نویسه* nextTKey = ultag_getTKeyStart(tvalue);
            اگر (nextTKey != nullptr) {
                *const_cast<نویسه*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.درج_Element_At((تهی*)tkey, tfields.size(), status);
            اگر (U_FAILURE(status)) {
                بازگشت نادرست;
            }
            tkey = nextTKey;
        } وقتی (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            بازگشت uprv_strcmp((ثابت نویسه*)e1.pointer, (ثابت نویسه*)e2.pointer);
        }, status);
        برای (int32_t i = 0; i < tfields.size(); i++) {
             اگر (output.length() > 0) {
                 output.append('-', status);
             }
             ثابت نویسه* tfield = static_cast<ثابت نویسه*>(tfields.elementAt(i));
             ثابت نویسه* tvalue = uprv_strchr(tfield, '-');
             اگر (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 بازگشت نادرست;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<نویسه*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    بازگشت درست;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    اگر (U_FAILURE(status)) { بازگشت out; }
    out.append(language, status);
    اگر (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    اگر (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    اگر (variants.size() > 0) {
        اگر (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            بازگشت uprv_strcmp((ثابت نویسه*)e1.pointer, (ثابت نویسه*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        برای (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<ثابت نویسه*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    اگر (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    بازگشت out;
}

منطقی
AliasReplacer::replace(ثابت Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    U_ASSERT(data != nullptr);
    out.clear();
    language = locale.getLanguage();
    اگر (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    اگر (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    اگر (!notEmpty(region)) {
        region = nullptr;
    }
    ثابت نویسه* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    اگر (!variantsBuff.isEmpty()) {
        اگر (U_FAILURE(status)) { بازگشت نادرست; }
        نویسه* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        نویسه* end;
        وقتی ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            اگر (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        اگر (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    اگر (U_FAILURE(status)) { بازگشت نادرست; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        بازگشت uprv_strcmp((ثابت نویسه*)e1.pointer, (ثابت نویسه*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    ثابت int32_t maxChanged = 4 + variants.size();
    (تهی)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](تهی *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    وقتی (U_SUCCESS(status)) {
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
        اگر (    replaceLanguage(درست, درست,  درست,  stringsToBeFreed, status) ||
                replaceLanguage(درست, درست,  نادرست, stringsToBeFreed, status) ||
                replaceLanguage(درست, نادرست, درست,  stringsToBeFreed, status) ||
                replaceLanguage(درست, نادرست, نادرست, stringsToBeFreed, status) ||
                replaceLanguage(نادرست,نادرست, درست,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            ادامه;
        }
        // Nothing changed. Break out.
        بشکن;
    }  // while(1)

    اگر (U_FAILURE(status)) { بازگشت نادرست; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ثابت نویسه* extensionsStr = locale_getKeywordsStart(locale.getName());
    اگر (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        بازگشت نادرست;
    }
    outputToString(out, status);
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    اگر (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        اگر (U_SUCCESS(status) && !iter.isNull()) {
            ثابت نویسه* کلید;
            وقتی ((کلید = iter->next(nullptr, status)) != nullptr) {
                اگر (uprv_strcmp("sd", کلید) == 0 || uprv_strcmp("rg", کلید) == 0 ||
                        uprv_strcmp("t", کلید) == 0) {
                    auto value = locale.get_Keyword_مقدار<CharString>(کلید, status);
                    اگر (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        ادامه;
                    }
                    CharString replacement;
                    اگر (uprv_strlen(کلید) == 2) {
                        اگر (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.set_Keyword_مقدار(کلید, replacement.data(), status);
                        }
                    } وگرنه {
                        U_ASSERT(uprv_strcmp(کلید, "t") == 0);
                        اگر (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.set_Keyword_مقدار(کلید, replacement.data(), status);
                        }
                    }
                    اگر (U_FAILURE(status)) {
                        بازگشت نادرست;
                    }
                }
            }
        }
        اگر (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    // If the tag is not changed, return.
    اگر (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        بازگشت نادرست;
    }
    بازگشت درست;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
منطقی
canonicalizeLocale(ثابت Locale& locale, CharString& out, UErrorCode& status)
{
    اگر (U_FAILURE(status)) { بازگشت نادرست; }
    AliasReplacer replacer(status);
    بازگشت replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
منطقی
isKnownCanonicalizedLocale(ثابت نویسه* locale, UErrorCode& status)
{
    اگر (U_FAILURE(status)) { بازگشت نادرست; }

    اگر (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        بازگشت درست;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    بازگشت uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT ثابت نویسه* ثابت*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    بازگشت KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT منطقی
ulocimp_isCanonicalizedLocaleForTest(ثابت نویسه* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    بازگشت !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ثابت نویسه* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    بازگشت localeID == nullptr ? *این = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    انجامبده {
        نویسه *separator;
        نویسه *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        ثابت auto parse = [canonicalize](std::string_view localeID,
                                          نویسه* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            بازگشت ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    اگر (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } وگرنه {
                        ulocimp_getName(localeID, sink, status);
                    }
                },
                status);
        };

        // "canonicalize" the locale ID to ICU/Java format
        نویسه* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, اندازه Nest::baseName, err);

        FixedString fullNameBuffer;
        اگر (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(اندازه Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            اگر (!fullNameBuffer.reserve(length + 1)) {
                بشکن; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        اگر(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            بشکن;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        نویسه* at = uprv_strchr(fullName, '@');
        وقتی ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        نویسه* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        اگر (separator!=nullptr || sep2!=nullptr) {
            اگر (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } وگرنه {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        منطقی hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        اگر (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            بشکن; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        اگر (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        اگر (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        اگر (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } وگرنه اگر (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        اگر (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } وگرنه اگر (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        اگر (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } وگرنه {
            اگر (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                اگر (fullNameBuffer.isEmpty()) {
                    بشکن; // error: out of memory
                }
                اگر (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                اگر (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                اگر (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            اگر (isBogus()) {
                بشکن; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            اگر (hasKeywords) {
                اگر (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    اگر (heap.ptr->baseName.isEmpty()) {
                        بشکن; // error: out of memory
                    }
                }
            }
        }

        اگر (canonicalize) {
            اگر (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                اگر (canonicalizeLocale(*این, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), نادرست);
                }
                اگر (U_FAILURE(err)) {
                    بشکن;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        بازگشت *این;
    } وقتی(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    بازگشت *این;
}

int32_t
Locale::hashCode() ثابت
{
    بازگشت ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

تهی
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

ثابت Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        اگر (gDefaultLocale != nullptr) {
            بازگشت *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    بازگشت *locale_set_default_internal(nullptr, status);
}



تهی U_EXPORT2
Locale::setDefault( ثابت   Locale&     newLocale,
                            UErrorCode&  status)
{
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ثابت نویسه *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

تهی
Locale::addLikelySubtags(UErrorCode& status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    اگر (U_FAILURE(status)) {
        اگر (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        بازگشت;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/نادرست);
    اگر (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

تهی
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(نادرست, status);
}
تهی
Locale::minimizeSubtags(منطقی favorScript, UErrorCode& status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    اگر (U_FAILURE(status)) {
        اگر (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        بازگشت;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/نادرست);
    اگر (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

تهی
Locale::canonicalize(UErrorCode& status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    اگر (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    CharString uncanonicalized(getName(), status);
    اگر (U_FAILURE(status)) {
        اگر (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        بازگشت;
    }
    init(uncanonicalized.data(), /*canonicalize=*/درست);
    اگر (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    اگر (U_FAILURE(status)) {
        بازگشت result;
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

    اگر (U_FAILURE(status)) {
        بازگشت result;
    }

    اگر (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت result;
    }

    result.init(localeID.data(), /*canonicalize=*/نادرست);
    اگر (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    بازگشت result;
}

تهی
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) ثابت
{
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    اگر (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/نادرست, status);
}

Locale U_EXPORT2
Locale::createFromName (ثابت نویسه *name)
{
    اگر (name) {
        Locale l("");
        l.init(name, نادرست);
        بازگشت l;
    }
    وگرنه {
        بازگشت getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, نادرست);
    بازگشت loc;
}

Locale U_EXPORT2
Locale::createCanonical(ثابت نویسه* name) {
    Locale loc("");
    loc.init(name, درست);
    بازگشت loc;
}

ثابت نویسه *
Locale::getISO3Language() ثابت
{
    بازگشت uloc_getISO3Language(getName());
}


ثابت نویسه *
Locale::getISO3Country() ثابت
{
    بازگشت uloc_getISO3Country(getName());
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
    بازگشت uloc_getLCID(getName());
}

ثابت نویسه* ثابت* U_EXPORT2 Locale::getISOCountries()
{
    بازگشت uloc_getISOCountries();
}

ثابت نویسه* ثابت* U_EXPORT2 Locale::getISOLanguages()
{
    بازگشت uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
تهی Locale::setFromPOSIXID(ثابت نویسه *posixID)
{
    init(posixID, درست);
}

ثابت Locale & U_EXPORT2
Locale::get_ریشه()
{
    بازگشت getLocale(e_ریشه);
}

ثابت Locale & U_EXPORT2
Locale::getEnglish()
{
    بازگشت getLocale(eENGLISH);
}

ثابت Locale & U_EXPORT2
Locale::getFrench()
{
    بازگشت getLocale(eFRENCH);
}

ثابت Locale & U_EXPORT2
Locale::getGerman()
{
    بازگشت getLocale(eGERMAN);
}

ثابت Locale & U_EXPORT2
Locale::getItalian()
{
    بازگشت getLocale(eITALIAN);
}

ثابت Locale & U_EXPORT2
Locale::getJapanese()
{
    بازگشت getLocale(eJAPANESE);
}

ثابت Locale & U_EXPORT2
Locale::getKorean()
{
    بازگشت getLocale(eKOREAN);
}

ثابت Locale & U_EXPORT2
Locale::getChinese()
{
    بازگشت getLocale(eCHINESE);
}

ثابت Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    بازگشت getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    بازگشت getLocale(eTAIWAN);
}


ثابت Locale & U_EXPORT2
Locale::getFrance()
{
    بازگشت getLocale(eFRANCE);
}

ثابت Locale & U_EXPORT2
Locale::getGermany()
{
    بازگشت getLocale(eGERMANY);
}

ثابت Locale & U_EXPORT2
Locale::getItaly()
{
    بازگشت getLocale(eITALY);
}

ثابت Locale & U_EXPORT2
Locale::getJapan()
{
    بازگشت getLocale(eJAPAN);
}

ثابت Locale & U_EXPORT2
Locale::getKorea()
{
    بازگشت getLocale(eKOREA);
}

ثابت Locale & U_EXPORT2
Locale::getChina()
{
    بازگشت getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getPRC()
{
    بازگشت getLocale(eCHINA);
}

ثابت Locale & U_EXPORT2
Locale::getTaiwan()
{
    بازگشت getLocale(eTAIWAN);
}

ثابت Locale & U_EXPORT2
Locale::getUK()
{
    بازگشت getLocale(eUK);
}

ثابت Locale & U_EXPORT2
Locale::getUS()
{
    بازگشت getLocale(eUS);
}

ثابت Locale & U_EXPORT2
Locale::getCanada()
{
    بازگشت getLocale(eCANADA);
}

ثابت Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    بازگشت getLocale(eCANADA_FRENCH);
}

ثابت Locale &
Locale::getLocale(صحیح locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    اگر (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    بازگشت localeCache[locid]; /*operating on nullptr*/
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
    بازگشت gLocaleCache;
}

رده KeywordEnumeration : عمومی StringEnumeration {
محافظتشده:
    FixedString keywords;
خصوصی:
    int32_t length;
    ثابت نویسه *current;
    ایستا ثابت نویسه fgClassID;

عمومی:
    ایستا UClassID U_EXPORT2 getStaticClassID() { بازگشت (UClassID)&fgClassID; }
    مجازی UClassID getDynamicClassID() ثابت override { بازگشت getStaticClassID(); }
عمومی:
    KeywordEnumeration(ثابت نویسه *کلیدها, int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        اگر(U_SUCCESS(status) && keywordLen != 0) {
            اگر(کلیدها == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } وگرنه {
                keywords = {کلیدها, static_cast<std::string_view::size_type>(length)};
                اگر (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } وگرنه {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    مجازی ~KeywordEnumeration();

    مجازی StringEnumeration * clone() ثابت override
    {
        UErrorCode status = U_ZERO_ERROR;
        بازگشت جدید KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    مجازی int32_t count(UErrorCode& status) ثابت override {
        اگر (U_FAILURE(status)) { بازگشت 0; }
        ثابت نویسه *kw = keywords.data();
        int32_t result = 0;
        وقتی(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        بازگشت result;
    }

    مجازی ثابت نویسه* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ثابت نویسه* result;
        int32_t len;
        اگر(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            اگر(resultLength != nullptr) {
                *resultLength = len;
            }
        } وگرنه {
            اگر(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        بازگشت result;
    }

    مجازی ثابت UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        اگر (U_FAILURE(status)) { بازگشت nullptr; }
        int32_t resultLength = 0;
        ثابت نویسه *s = next(&resultLength, status);
        بازگشت setChars(s, resultLength, status);
    }

    مجازی تهی reset(UErrorCode& status) override {
        اگر (U_FAILURE(status)) { بازگشت; }
        current = keywords.data();
    }
};

ثابت نویسه KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = پیشفرض;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
رده UnicodeKeywordEnumeration : عمومی KeywordEnumeration {
عمومی:
    using KeywordEnumeration::KeywordEnumeration;
    مجازی ~UnicodeKeywordEnumeration();

    مجازی ثابت نویسه* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ثابت نویسه* legacy_کلید = KeywordEnumeration::next(nullptr, status);
        وقتی (U_SUCCESS(status) && legacy_کلید != nullptr) {
            ثابت نویسه* کلید = uloc_to_Unicode_Locale_کلید(legacy_کلید);
            اگر (کلید != nullptr) {
                اگر (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(کلید));
                }
                بازگشت کلید;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_کلید = KeywordEnumeration::next(nullptr, status);
        }
        اگر (resultLength != nullptr) *resultLength = 0;
        بازگشت nullptr;
    }
    مجازی int32_t count(UErrorCode& status) ثابت override {
        اگر (U_FAILURE(status)) { بازگشت 0; }
        ثابت نویسه *kw = keywords.data();
        int32_t result = 0;
        وقتی(*kw) {
            اگر (uloc_to_Unicode_Locale_کلید(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        بازگشت result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = پیشفرض;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) ثابت
{
    StringEnumeration *result = nullptr;

    اگر (U_FAILURE(status)) {
        بازگشت result;
    }

    ثابت نویسه* variantStart = uprv_strchr(getName(), '@');
    ثابت نویسه* assignment = uprv_strchr(getName(), '=');
    اگر(variantStart) {
        اگر(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', نادرست, status);
            اگر (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = جدید KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                اگر (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } وگرنه {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    بازگشت result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) ثابت
{
    StringEnumeration *result = nullptr;

    اگر (U_FAILURE(status)) {
        بازگشت result;
    }

    ثابت نویسه* variantStart = uprv_strchr(getName(), '@');
    ثابت نویسه* assignment = uprv_strchr(getName(), '=');
    اگر(variantStart) {
        اگر(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', نادرست, status);
            اگر (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = جدید UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                اگر (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } وگرنه {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    بازگشت result;
}

int32_t
Locale::get_Keyword_مقدار(ثابت نویسه* keywordName, نویسه *buffer, int32_t bufLen, UErrorCode &status) ثابت
{
    بازگشت uloc_get_Keyword_مقدار(getName(), keywordName, buffer, bufLen, &status);
}

تهی
Locale::get_Keyword_مقدار(StringPiece keywordName, ByteSink& sink, UErrorCode& status) ثابت {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    اگر (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    ulocimp_get_Keyword_مقدار(getName(), keywordName, sink, status);
}

تهی
Locale::get_Unicode_Keyword_مقدار(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) ثابت {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    std::optional<std::string_view> legacy_کلید = ulocimp_to_Legacy_کلید_With_Fallback(keywordName);
    اگر (!legacy_کلید.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    auto legacy_مقدار = get_Keyword_مقدار<CharString>(*legacy_کلید, status);

    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    std::optional<std::string_view> unicode_مقدار =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_مقدار.toStringPiece());
    اگر (!unicode_مقدار.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    sink.Append(unicode_مقدار->data(), static_cast<int32_t>(unicode_مقدار->size()));
}

تهی
Locale::set_Keyword_مقدار(StringPiece keywordName,
                        StringPiece keyword_مقدار,
                        UErrorCode& status) {
    اگر (U_FAILURE(status)) { بازگشت; }
    اگر (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    اگر (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_set_Keyword_مقدار(keywordName, keyword_مقدار, localeID, status);
    اگر (U_FAILURE(status)) {
        اگر (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        بازگشت;
    }

    ثابت نویسه* at = locale_getKeywordsStart(localeID.toStringPiece());
    منطقی hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    اگر (!hasKeywords) {
        اگر (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            اگر (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < اندازه Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, اندازه Nest::baseName, status);
            } وگرنه {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                اگر (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    بازگشت;
                }
            }
        }
    } وگرنه {
        Heap* heap = nullptr;
        اگر (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            اگر (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                بازگشت;
            }
        } وگرنه {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        اگر (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            بازگشت;
        }

        اگر (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            اگر (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                اگر (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    بازگشت;
                }
            }
        }
    }
}

تهی
Locale::set_Unicode_Keyword_مقدار(StringPiece keywordName,
                               StringPiece keyword_مقدار,
                               UErrorCode& status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    std::optional<std::string_view> legacy_کلید = ulocimp_to_Legacy_کلید_With_Fallback(keywordName);
    اگر (!legacy_کلید.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    std::string_view value;

    اگر (!keyword_مقدار.empty()) {
        std::optional<std::string_view> legacy_مقدار =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_مقدار);
        اگر (!legacy_مقدار.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت;
        }
        value = *legacy_مقدار;
    }

    set_Keyword_مقدار(*legacy_کلید, value, status);
}

ثابت نویسه*
Locale::getCountry() ثابت U_LIFETIME_BOUND {
    بازگشت getField<&Nest::getRegion, &Heap::getRegion>();
}

ثابت نویسه*
Locale::getLanguage() ثابت U_LIFETIME_BOUND {
    بازگشت getField<&Nest::getLanguage, &Heap::getLanguage>();
}

ثابت نویسه*
Locale::getScript() ثابت U_LIFETIME_BOUND {
    بازگشت getField<&Nest::getScript, &Heap::getScript>();
}

ثابت نویسه*
Locale::getVariant() ثابت U_LIFETIME_BOUND {
    بازگشت getField<&Nest::getVariant, &Heap::getVariant>();
}

ثابت نویسه*
Locale::getName() ثابت U_LIFETIME_BOUND {
    بازگشت getField<&Nest::getBaseName, &Heap::getFullName>();
}

ثابت نویسه*
Locale::getBaseName() ثابت U_LIFETIME_BOUND {
    بازگشت getField<&Nest::getBaseName, &Heap::getBaseName>();
}

template <ثابت نویسه* (Locale::Nest::*ثابت NEST)() ثابت,
          ثابت نویسه* (Locale::Heap::*ثابت HEAP)() ثابت>
ثابت نویسه* Locale::getField() ثابت U_LIFETIME_BOUND {
    بازگشت payload.visit([] { بازگشت ""; },
                         [](ثابت Nest& nest) { بازگشت (nest.*NEST)(); },
                         [](ثابت Heap& heap) { بازگشت (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = پیشفرض;

//eof
U_NAMESPACE_END
