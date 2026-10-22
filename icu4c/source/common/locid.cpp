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

#अन्तर्भाव <cstddef>
#अन्तर्भाव <optional>
#अन्तर्भाव <string_view>
#अन्तर्भाव <type_traits>
#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/localebuilder.h"
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "unicode/strenum.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ures.h"

#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "charstrmap.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "fixedstring.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "ulocimp.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uniquecharstr.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "uvector.h"

#सचे UPRV_HAS_FEATURE(memory_sanitizer)
#अन्तर्भाव <sanitizer/msan_interface.h>
#endif

U_NAMESPACE_BEGIN

स्थिर Locale   *gLocaleCache = nullptr;
स्थिर UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
स्थिर UMutex gDefaultLocaleMutex;
स्थिर UHashtable *gDefaultLocalesHashT = nullptr;
स्थिर Locale *gDefaultLocale = nullptr;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#define ULOC_STRING_LIMIT 357913941

U_NAMESPACE_END

प्रकारनिर्देश गणना ELocalePos {
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
    e_मूलम्,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

नामाकाश {

//
// Deleter function for Locales owned by the default Locale hash table/
//
शून्य U_CALLCONV
deleteLocale(शून्य *obj) {
    delete static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    delete [] gLocaleCache;
    gLocaleCache = nullptr;
    gLocaleCacheInitOnce.reset();

    सचे (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    पटिनिवत्त सच्च;
}

शून्य U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = नूतन Locale[static_cast<पूर्णाङ्क>(eMAX_LOCALES)];
    सचे (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_मूलम्]          = Locale("");
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

Locale *locale_set_default_internal(ध्रुव वर्ण *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = मिच्छा;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    सचे (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = सच्च; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    सचे (U_FAILURE(status)) {
        पटिनिवत्त gDefaultLocale;
    }

    सचे (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        सचे (U_FAILURE(status)) {
            पटिनिवत्त gDefaultLocale;
        }
        uhash_set_मानम्_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    सचे (newDefault == nullptr) {
        newDefault = नूतन Locale(Locale::eBOGUS);
        सचे (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), मिच्छा);
        uhash_put(gDefaultLocalesHashT, const_cast<वर्ण*>(newDefault->getName()), newDefault, &status);
        सचे (U_FAILURE(status)) {
            पटिनिवत्त gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    पटिनिवत्त gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC शून्य
locale_set_default(ध्रुव वर्ण *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC ध्रुव वर्ण *
locale_get_default()
{
    U_NAMESPACE_USE
    पटिनिवत्त Locale::getDefault().getName();
}

नामाकाश {

template <auto FIELD, typename T>
शून्य copyToArray(std::string_view sv, T* that) {
    auto& field = that->*FIELD;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    सचे (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.data(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

शून्य Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, एतत्);
    copyToArray<&Nest::script>(script, एतत्);
    copyToArray<&Nest::region>(region, एतत्);
    एतत्->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(Heap, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(Heap, region));
    U_ASSERT(एतत् == reinterpret_cast<Nest*>(&heap));
#सचे UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, परिमाण(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, एतत्);
    copyToArray<&Nest::region>(heap.region, एतत्);
    एतत्->variantBegin = variantBegin;
    *एतत्->baseName = '\0';
}

संरचना Locale::Heap::Alloc : सार्वजनिक UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    ध्रुव वर्ण* getVariant() ध्रुव U_LIFETIME_BOUND {
        पटिनिवत्त variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    ध्रुव वर्ण* getFullName() ध्रुव U_LIFETIME_BOUND { पटिनिवत्त fullName.data(); }
    ध्रुव वर्ण* getBaseName() ध्रुव U_LIFETIME_BOUND {
        सचे (baseName.isEmpty()) {
            सचे (ध्रुव वर्ण* name = fullName.data(); *name != '@') {
                पटिनिवत्त name;
            }
        }
        पटिनिवत्त baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(ध्रुव Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        सचे (U_SUCCESS(status)) {
            सचे (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                सचे (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अञ्ञथा {
                    सचे (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        सचे (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = delete;

    ~Alloc() = स्वभाव;
};

ध्रुव वर्ण* Locale::Heap::getVariant() ध्रुव U_LIFETIME_BOUND { पटिनिवत्त ptr->getVariant(); }
ध्रुव वर्ण* Locale::Heap::getFullName() ध्रुव U_LIFETIME_BOUND { पटिनिवत्त ptr->getFullName(); }
ध्रुव वर्ण* Locale::Heap::getBaseName() ध्रुव U_LIFETIME_BOUND { पटिनिवत्त ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = नूतन Alloc(variantBegin);
    सचे (ptr == nullptr) {
        type = eBOGUS;
    } अञ्ञथा {
        type = eHEAP;
        copyToArray<&Heap::language>(language, एतत्);
        copyToArray<&Heap::script>(script, एतत्);
        copyToArray<&Heap::region>(region, एतत्);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::operator=(ध्रुव Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = नूतन Alloc(*other.ptr, status);
    सचे (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } अञ्ञथा {
        type = eHEAP;
        uprv_memcpy(language, other.language, परिमाण language);
        uprv_memcpy(script, other.script, परिमाण script);
        uprv_memcpy(region, other.region, परिमाण region);
    }
    पटिनिवत्त *एतत्;
}

Locale::Heap& Locale::Heap::operator=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, परिमाण language);
    uprv_memcpy(script, other.script, परिमाण script);
    uprv_memcpy(region, other.region, परिमाण region);
    पटिनिवत्त *एतत्;
}

template <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
auto Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) ध्रुव {
    विकल्प (type) {
        पक्ष eBOGUS:
            पटिनिवत्त bogusFn(args...);
        पक्ष eNEST:
            पटिनिवत्त nestFn(nest, args...);
        पक्ष eHEAP:
            पटिनिवत्त heapFn(heap, args...);
        स्वभाव:
            UPRV_UNREACHABLE_EXIT;
    };
}

शून्य Locale::Payload::copy(ध्रुव Payload& other) {
    other.visit([](Payload*) {},
                [](ध्रुव Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ध्रुव Heap& heap, Payload* dst) { dst->heap = heap; },
                एतत्);
}

शून्य Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](ध्रुव Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ध्रुव Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        एतत्);
}

Locale::Payload::~Payload() {
    सचे (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(ध्रुव Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::operator=(ध्रुव Payload& other) U_LIFETIME_BOUND {
    सचे (एतत् != &other) {
        setToBogus();
        copy(other);
    }
    पटिनिवत्त *एतत्;
}

Locale::Payload& Locale::Payload::operator=(Payload&& other) noexcept U_LIFETIME_BOUND {
    सचे (एतत् != &other) {
        setToBogus();
        move(std::move(other));
    }
    पटिनिवत्त *एतत्;
}

शून्य Locale::Payload::setToBogus() {
    एतत्->~Payload();
    type = eBOGUS;
}

template <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    सचे constexpr (std::is_same_v<T, Nest>) {
        एतत्->~Payload();
        ::नूतन (&nest) Nest(std::forward<Args>(args)...);
        पटिनिवत्त nest;
    }
    सचे constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::नूतन (&heap) Heap(std::forward<Args>(args)...);
        पटिनिवत्त heap;
    }
}

template <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    पटिनिवत्त type == eNEST ? &nest : nullptr;
}
template <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    पटिनिवत्त type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = स्वभाव;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, मिच्छा);
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


Locale::Locale( ध्रुव   वर्ण * newLanguage,
                ध्रुव   वर्ण * newCountry,
                ध्रुव   वर्ण * newVariant,
                ध्रुव   वर्ण * newKeywords)
    : UObject(), payload()
{
    सचे( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, मिच्छा); /* shortcut */
    }
    अञ्ञथा
    {
        UErrorCode status = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        सचे ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            सचे ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                पटिनिवत्त;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        सचे ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            सचे ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                पटिनिवत्त;
            }
        }

        // _Variant
        सचे ( newVariant != nullptr )
        {
            // remove leading _'s
            याव(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            सचे ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                पटिनिवत्त;
            }
            याव( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        सचे ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            सचे ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              पटिनिवत्त;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        सचे ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        सचे ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        सचे ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        सचे ( ksize != 0)
        {
            सचे (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            अञ्ञथा {
                togo.append('_', status); /* Variant parsing with a script */
                सचे ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        सचे (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            पटिनिवत्त;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), मिच्छा);
    }
}

Locale::Locale(ध्रुव Locale&) = स्वभाव;
Locale::Locale(Locale&&) noexcept = स्वभाव;

Locale& Locale::operator=(ध्रुव Locale&) U_LIFETIME_BOUND = स्वभाव;
Locale& Locale::operator=(Locale&&) noexcept U_LIFETIME_BOUND = स्वभाव;

Locale *
Locale::clone() ध्रुव {
    पटिनिवत्त नूतन Locale(*एतत्);
}

तार्किक
Locale::operator==( ध्रुव   Locale& other) ध्रुव
{
    पटिनिवत्त uprv_strcmp(other.getName(), getName()) == 0;
}

नामाकाश {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr ध्रुव वर्ण* KNOWN_CANONICALIZED[] = {
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
    सचे (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    पटिनिवत्त सच्च;
}

शून्य U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    अत्थाय (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (शून्य*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

वग्ग AliasData;

/**
 * A Builder class to build the alias data.
 */
वग्ग AliasDataBuilder {
सार्वजनिक:
    AliasDataBuilder() {
    }

    // Build the AliasData from resource.
    AliasData* build(UErrorCode &status);

गुप्त:
    शून्य readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<ध्रुव वर्ण*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   शून्य (*checkType)(ध्रुव वर्ण* type),
                   शून्य (*checkReplacement)(ध्रुव UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    शून्य readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<ध्रुव वर्ण*>& types,
                           LocalMemory<int32_t>& replacementIndexes,
                           int32_t &length,
                           UErrorCode &status);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    शून्य readScriptAlias(UResourceBundle* alias,
                         UniqueCharStrings* strings,
                         LocalMemory<ध्रुव वर्ण*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    शून्य readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<ध्रुव वर्ण*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    शून्य readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ध्रुव वर्ण*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    शून्य readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ध्रुव वर्ण*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
वग्ग AliasData : सार्वजनिक UMemory {
सार्वजनिक:
    स्थिर ध्रुव AliasData* singleton(UErrorCode& status) {
        सचे (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            पटिनिवत्त nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        पटिनिवत्त gSingleton;
    }

    ध्रुव CharStringMap& languageMap() ध्रुव U_LIFETIME_BOUND { पटिनिवत्त language; }
    ध्रुव CharStringMap& scriptMap() ध्रुव U_LIFETIME_BOUND { पटिनिवत्त script; }
    ध्रुव CharStringMap& territoryMap() ध्रुव U_LIFETIME_BOUND { पटिनिवत्त territory; }
    ध्रुव CharStringMap& variantMap() ध्रुव U_LIFETIME_BOUND { पटिनिवत्त variant; }
    ध्रुव CharStringMap& subdivisionMap() ध्रुव U_LIFETIME_BOUND { पटिनिवत्त subdivision; }

    स्थिर शून्य U_CALLCONV loadData(UErrorCode &status);
    स्थिर UBool U_CALLCONV cleanup();

    स्थिर UInitOnce gInitOnce;

गुप्त:
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

    स्थिर ध्रुव AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    friend वग्ग AliasDataBuilder;
};


ध्रुव AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    पटिनिवत्त सच्च;
}

शून्य
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ध्रुव वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        शून्य (*checkType)(ध्रुव वर्ण* type),
        शून्य (*checkReplacement)(ध्रुव UChar* replacement),
        UErrorCode &status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
    length = ures_getSize(alias);
    ध्रुव वर्ण** rawTypes = types.allocateInsteadAndCopy(length);
    सचे (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    सचे (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    अत्थाय (पूर्णाङ्क i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        ध्रुव वर्ण* aliasFrom = ures_get_कुञ्चिका(res.getAlias());
        ध्रुव UChar* aliasTo =
            ures_get_String_By_कुञ्चिका(res.getAlias(), "replacement", nullptr, &status);
        सचे (U_FAILURE(status)) पटिनिवत्त;

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
शून्य
AliasDataBuilder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ध्रुव वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    पटिनिवत्त readAlias(
        alias, strings, types, replacementIndexes, length,
#सचे U_DEBUG
        [](ध्रुव वर्ण* type) {
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
#अञ्ञथा
        [](ध्रुव वर्ण*) {},
#endif
        [](ध्रुव UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
शून्य
AliasDataBuilder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ध्रुव वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    पटिनिवत्त readAlias(
        alias, strings, types, replacementIndexes, length,
#सचे U_DEBUG
        [](ध्रुव वर्ण* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ध्रुव UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#अञ्ञथा
        [](ध्रुव वर्ण*) {},
        [](ध्रुव UChar*) { },
#endif
        status);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
शून्य
AliasDataBuilder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ध्रुव वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    पटिनिवत्त readAlias(
        alias, strings, types, replacementIndexes, length,
#सचे U_DEBUG
        [](ध्रुव वर्ण* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#अञ्ञथा
        [](ध्रुव वर्ण*) {},
#endif
        [](ध्रुव UChar*) { },
        status);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
शून्य
AliasDataBuilder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ध्रुव वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    पटिनिवत्त readAlias(
        alias, strings, types, replacementIndexes, length,
#सचे U_DEBUG
        [](ध्रुव वर्ण* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](ध्रुव UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#अञ्ञथा
        [](ध्रुव वर्ण*) {},
        [](ध्रुव UChar*) { },
#endif
        status);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
शून्य
AliasDataBuilder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ध्रुव वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    पटिनिवत्त readAlias(
        alias, strings, types, replacementIndexes, length,
#सचे U_DEBUG
        [](ध्रुव वर्ण* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#अञ्ञथा
        [](ध्रुव वर्ण*) {},
#endif
        [](ध्रुव UChar*) { },
        status);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
शून्य U_CALLCONV
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
    सचे (U_FAILURE(status)) { पटिनिवत्त nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_कुञ्चिका(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_कुञ्चिका(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_कुञ्चिका(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_कुञ्चिका(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_कुञ्चिका(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_कुञ्चिका(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    सचे (U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<ध्रुव वर्ण*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<ध्रुव वर्ण*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<ध्रुव वर्ण*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<ध्रुव वर्ण*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<ध्रुव वर्ण*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    सचे (U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    अत्थाय (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    अत्थाय (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    अत्थाय (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    अत्थाय (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    अत्थाय (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    सचे (U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }

    // copy hashtables
    auto *data = नूतन AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    सचे (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    पटिनिवत्त data;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
वग्ग AliasReplacer {
सार्वजनिक:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       पटिनिवत्त 0==uprv_strcmp((ध्रुव वर्ण*)e1.pointer,
                                             (ध्रुव वर्ण*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    तार्किक replace(
        ध्रुव Locale& locale, CharString& out, UErrorCode& status);

गुप्त:
    ध्रुव वर्ण* language;
    ध्रुव वर्ण* script;
    ध्रुव वर्ण* region;
    ध्रुव वर्ण* extensions;
    UVector variants;

    ध्रुव AliasData* data;

    अन्तःस्थ तार्किक notEmpty(ध्रुव वर्ण* str) {
        पटिनिवत्त str && str[0] != NULL_CHAR;
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
    अन्तःस्थ ध्रुव वर्ण* deleteOrReplace(ध्रुव वर्ण* input U_LIFETIME_BOUND,
                                       ध्रुव वर्ण* type,
                                       ध्रुव वर्ण* replacement U_LIFETIME_BOUND) {
        पटिनिवत्त notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    अन्तःस्थ तार्किक same(ध्रुव वर्ण* a, ध्रुव वर्ण* b) {
        सचे (a == nullptr && b == nullptr) {
            पटिनिवत्त सच्च;
        }
        सचे ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          पटिनिवत्त मिच्छा;
        }
        पटिनिवत्त uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_कुञ्चिका(ध्रुव वर्ण* language, ध्रुव वर्ण* region,
                            ध्रुव वर्ण* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    शून्य parseLanguageReplacement(ध्रुव वर्ण* replacement,
                                  ध्रुव वर्ण*& replaceLanguage,
                                  ध्रुव वर्ण*& replaceScript,
                                  ध्रुव वर्ण*& replaceRegion,
                                  ध्रुव वर्ण*& replaceVariant,
                                  ध्रुव वर्ण*& replaceExtensions,
                                  UVector& toBeFreed,
                                  UErrorCode& status);

    // Replace by using languageAlias.
    तार्किक replaceLanguage(तार्किक checkLanguage, तार्किक checkRegion,
                         तार्किक checkVariants, UVector& toBeFreed,
                         UErrorCode& status);

    // Replace by using territoryAlias.
    तार्किक replaceTerritory(UVector& toBeFreed, UErrorCode& status);

    // Replace by using scriptAlias.
    तार्किक replaceScript(UErrorCode& status);

    // Replace by using variantAlias.
    तार्किक replaceVariant(UErrorCode& status);

    // Replace by using subdivisionAlias.
    तार्किक replaceSubdivision(StringPiece subdivision,
                            CharString& output, UErrorCode& status);

    // Replace transformed extensions.
    तार्किक replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UErrorCode& status);
};

CharString&
AliasReplacer::generate_कुञ्चिका(
        ध्रुव वर्ण* language, ध्रुव वर्ण* region, ध्रुव वर्ण* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    सचे (U_FAILURE(status)) { पटिनिवत्त out; }
    out.append(language, status);
    सचे (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    सचे (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    पटिनिवत्त out;
}

शून्य
AliasReplacer::parseLanguageReplacement(
    ध्रुव वर्ण* replacement,
    ध्रुव वर्ण*& replacedLanguage,
    ध्रुव वर्ण*& replacedScript,
    ध्रुव वर्ण*& replacedRegion,
    ध्रुव वर्ण*& replacedVariant,
    ध्रुव वर्ण*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    सचे (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        पटिनिवत्त;
    }
    // We have multiple field so we have to allocate and parse
    auto str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    वर्ण* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
    replacedLanguage = (ध्रुव वर्ण*) data;
    वर्ण* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    ध्रुव वर्ण* start = endOfField;
    endOfField = const_cast<वर्ण*>(uprv_strchr(start, '_'));
    size_t len = 0;
    सचे (endOfField == nullptr) {
        len = uprv_strlen(start);
    } अञ्ञथा {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    सचे (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        सचे (endOfField == nullptr) {
            पटिनिवत्त;
        }
        start = endOfField++;
        endOfField = const_cast<वर्ण*>(uprv_strchr(start, '_'));
        सचे (endOfField == nullptr) {
            len = uprv_strlen(start);
        } अञ्ञथा {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    सचे (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        सचे (endOfField == nullptr) {
            पटिनिवत्त;
        }
        start = endOfField++;
        endOfField = const_cast<वर्ण*>(uprv_strchr(start, '_'));
        सचे (endOfField == nullptr) {
            len = uprv_strlen(start);
        } अञ्ञथा {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    सचे (len >= 4) {
        // Got a variant
        replacedVariant = start;
        सचे (endOfField == nullptr) {
            पटिनिवत्त;
        }
        start = endOfField++;
    }
    replacedExtensions = start;
}

तार्किक
AliasReplacer::replaceLanguage(
        तार्किक checkLanguage, तार्किक checkRegion,
        तार्किक checkVariants, UVector& toBeFreed, UErrorCode& status)
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    सचे (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        पटिनिवत्त मिच्छा;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ध्रुव वर्ण* अन्वेषणम्_Language = checkLanguage ? language : "und";
    ध्रुव वर्ण* अन्वेषणम्_Region = checkRegion ? region : nullptr;
    ध्रुव वर्ण* अन्वेषणम्_Variant = nullptr;
    अत्थाय (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        सचे (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            अन्वेषणम्_Variant = static_cast<ध्रुव वर्ण*>(variants.elementAt(variant_index));
        }

        सचे (अन्वेषणम्_Variant != nullptr && uprv_strlen(अन्वेषणम्_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            अन्वेषणम्_Variant = nullptr;
        }
        CharString type_कुञ्चिका;
        generate_कुञ्चिका(अन्वेषणम्_Language, अन्वेषणम्_Region, अन्वेषणम्_Variant, type_कुञ्चिका,
                    status);
        सचे (U_FAILURE(status)) {
            पटिनिवत्त मिच्छा;
        }
        ध्रुव वर्ण *replacement = data->languageMap().get(type_कुञ्चिका.data());
        सचे (replacement == nullptr) {
            // Found no replacement data.
            अनुवर्त;
        }

        ध्रुव वर्ण* replacedLanguage = nullptr;
        ध्रुव वर्ण* replacedScript = nullptr;
        ध्रुव वर्ण* replacedRegion = nullptr;
        ध्रुव वर्ण* replacedVariant = nullptr;
        ध्रुव वर्ण* replacedExtensions = nullptr;
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
        replacedRegion = deleteOrReplace(region, अन्वेषणम्_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            अन्वेषणम्_Variant, अन्वेषणम्_Variant, replacedVariant);

        सचे (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(अन्वेषणम्_Variant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            अनुवर्त;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        सचे (अन्वेषणम्_Variant != nullptr) {
            सचे (notEmpty(replacedVariant)) {
                variants.setElementAt((शून्य*)replacedVariant, variant_index);
            } अञ्ञथा {
                variants.removeElementAt(variant_index);
            }
        }
        सचे (replacedExtensions != nullptr) {
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
        पटिनिवत्त सच्च;
    }
    // Nothing changed by language alias data.
    पटिनिवत्त मिच्छा;
}

तार्किक
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    सचे (region == nullptr) {
        // No region to search.
        पटिनिवत्त मिच्छा;
    }
    ध्रुव वर्ण *replacement = data->territoryMap().get(region);
    सचे (replacement == nullptr) {
        // Found no replacement data for this region.
        पटिनिवत्त मिच्छा;
    }
    ध्रुव वर्ण* replacedRegion = replacement;
    ध्रुव वर्ण* firstSpace = uprv_strchr(replacement, ' ');
    सचे (firstSpace != nullptr) {
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
        ध्रुव वर्ण* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        सचे (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ध्रुव वर्ण* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            सचे (foundInReplacement != nullptr) {
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
        सचे (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        सचे (U_FAILURE(status)) { पटिनिवत्त मिच्छा; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        सचे (U_FAILURE(status)) { पटिनिवत्त मिच्छा; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    पटिनिवत्त सच्च;
}

तार्किक
AliasReplacer::replaceScript(UErrorCode& status)
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    सचे (script == nullptr) {
        // No script to search.
        पटिनिवत्त मिच्छा;
    }
    ध्रुव वर्ण *replacement = data->scriptMap().get(script);
    सचे (replacement == nullptr) {
        // Found no replacement data for this script.
        पटिनिवत्त मिच्छा;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    पटिनिवत्त सच्च;
}

तार्किक
AliasReplacer::replaceVariant(UErrorCode& status)
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    // Since we may have more than one variant, we need to loop through them.
    अत्थाय (int32_t i = 0; i < variants.size(); i++) {
        ध्रुव वर्ण* variant = static_cast<ध्रुव वर्ण*>(variants.elementAt(i));
        ध्रुव वर्ण *replacement = data->variantMap().get(variant);
        सचे (replacement == nullptr) {
            // Found no replacement data for this variant.
            अनुवर्त;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        सचे (!same(variant, replacement)) {
            variants.setElementAt((शून्य*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            सचे (uprv_strcmp(variant, "heploc") == 0) {
                अत्थाय (int32_t j = 0; j < variants.size(); j++) {
                     सचे (uprv_strcmp((ध्रुव वर्ण*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            पटिनिवत्त सच्च;
        }
    }
    पटिनिवत्त मिच्छा;
}

तार्किक
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव वर्ण *replacement = data->subdivisionMap().get(subdivision.data());
    सचे (replacement != nullptr) {
        ध्रुव वर्ण* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        सचे (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            सचे (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        पटिनिवत्त सच्च;
    }
    पटिनिवत्त मिच्छा;
}

तार्किक
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    int32_t len = transformedExtensions.length();
    ध्रुव वर्ण* str = transformedExtensions.data();
    ध्रुव वर्ण* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    सचे (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        सचे (U_FAILURE(status)) {
            पटिनिवत्त मिच्छा;
        }
        T_CString_toLowerCase(output.data());
    }
    सचे (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        सचे (U_FAILURE(status)) {
            पटिनिवत्त मिच्छा;
        }
        करोहि {
            ध्रुव वर्ण* tvalue = uprv_strchr(tkey, '-');
            सचे (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                पटिनिवत्त मिच्छा;
            }
            ध्रुव वर्ण* nextTKey = ultag_getTKeyStart(tvalue);
            सचे (nextTKey != nullptr) {
                *const_cast<वर्ण*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.निवेशय_Element_At((शून्य*)tkey, tfields.size(), status);
            सचे (U_FAILURE(status)) {
                पटिनिवत्त मिच्छा;
            }
            tkey = nextTKey;
        } याव (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            पटिनिवत्त uprv_strcmp((ध्रुव वर्ण*)e1.pointer, (ध्रुव वर्ण*)e2.pointer);
        }, status);
        अत्थाय (int32_t i = 0; i < tfields.size(); i++) {
             सचे (output.length() > 0) {
                 output.append('-', status);
             }
             ध्रुव वर्ण* tfield = static_cast<ध्रुव वर्ण*>(tfields.elementAt(i));
             ध्रुव वर्ण* tvalue = uprv_strchr(tfield, '-');
             सचे (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 पटिनिवत्त मिच्छा;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<वर्ण*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    पटिनिवत्त सच्च;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    सचे (U_FAILURE(status)) { पटिनिवत्त out; }
    out.append(language, status);
    सचे (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    सचे (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    सचे (variants.size() > 0) {
        सचे (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            पटिनिवत्त uprv_strcmp((ध्रुव वर्ण*)e1.pointer, (ध्रुव वर्ण*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        अत्थाय (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<ध्रुव वर्ण*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    सचे (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    पटिनिवत्त out;
}

तार्किक
AliasReplacer::replace(ध्रुव Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    U_ASSERT(data != nullptr);
    out.clear();
    language = locale.getLanguage();
    सचे (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    सचे (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    सचे (!notEmpty(region)) {
        region = nullptr;
    }
    ध्रुव वर्ण* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    सचे (!variantsBuff.isEmpty()) {
        सचे (U_FAILURE(status)) { पटिनिवत्त मिच्छा; }
        वर्ण* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        वर्ण* end;
        याव ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            सचे (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        सचे (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    सचे (U_FAILURE(status)) { पटिनिवत्त मिच्छा; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        पटिनिवत्त uprv_strcmp((ध्रुव वर्ण*)e1.pointer, (ध्रुव वर्ण*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    ध्रुव int32_t maxChanged = 4 + variants.size();
    (शून्य)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](शून्य *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    याव (U_SUCCESS(status)) {
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
        सचे (    replaceLanguage(सच्च, सच्च,  सच्च,  stringsToBeFreed, status) ||
                replaceLanguage(सच्च, सच्च,  मिच्छा, stringsToBeFreed, status) ||
                replaceLanguage(सच्च, मिच्छा, सच्च,  stringsToBeFreed, status) ||
                replaceLanguage(सच्च, मिच्छा, मिच्छा, stringsToBeFreed, status) ||
                replaceLanguage(मिच्छा,मिच्छा, सच्च,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            अनुवर्त;
        }
        // Nothing changed. Break out.
        भङ्ग;
    }  // while(1)

    सचे (U_FAILURE(status)) { पटिनिवत्त मिच्छा; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ध्रुव वर्ण* extensionsStr = locale_getKeywordsStart(locale.getName());
    सचे (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        पटिनिवत्त मिच्छा;
    }
    outputToString(out, status);
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    सचे (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        सचे (U_SUCCESS(status) && !iter.isNull()) {
            ध्रुव वर्ण* कुञ्चिका;
            याव ((कुञ्चिका = iter->next(nullptr, status)) != nullptr) {
                सचे (uprv_strcmp("sd", कुञ्चिका) == 0 || uprv_strcmp("rg", कुञ्चिका) == 0 ||
                        uprv_strcmp("t", कुञ्चिका) == 0) {
                    auto value = locale.get_Keyword_मानम्<CharString>(कुञ्चिका, status);
                    सचे (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        अनुवर्त;
                    }
                    CharString replacement;
                    सचे (uprv_strlen(कुञ्चिका) == 2) {
                        सचे (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.set_Keyword_मानम्(कुञ्चिका, replacement.data(), status);
                        }
                    } अञ्ञथा {
                        U_ASSERT(uprv_strcmp(कुञ्चिका, "t") == 0);
                        सचे (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.set_Keyword_मानम्(कुञ्चिका, replacement.data(), status);
                        }
                    }
                    सचे (U_FAILURE(status)) {
                        पटिनिवत्त मिच्छा;
                    }
                }
            }
        }
        सचे (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    // If the tag is not changed, return.
    सचे (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        पटिनिवत्त मिच्छा;
    }
    पटिनिवत्त सच्च;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
तार्किक
canonicalizeLocale(ध्रुव Locale& locale, CharString& out, UErrorCode& status)
{
    सचे (U_FAILURE(status)) { पटिनिवत्त मिच्छा; }
    AliasReplacer replacer(status);
    पटिनिवत्त replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
तार्किक
isKnownCanonicalizedLocale(ध्रुव वर्ण* locale, UErrorCode& status)
{
    सचे (U_FAILURE(status)) { पटिनिवत्त मिच्छा; }

    सचे (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        पटिनिवत्त सच्च;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    पटिनिवत्त uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT ध्रुव वर्ण* ध्रुव*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    पटिनिवत्त KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT तार्किक
ulocimp_isCanonicalizedLocaleForTest(ध्रुव वर्ण* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    पटिनिवत्त !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ध्रुव वर्ण* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    पटिनिवत्त localeID == nullptr ? *एतत् = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    करोहि {
        वर्ण *separator;
        वर्ण *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        ध्रुव auto parse = [canonicalize](std::string_view localeID,
                                          वर्ण* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            पटिनिवत्त ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    सचे (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } अञ्ञथा {
                        ulocimp_getName(localeID, sink, status);
                    }
                },
                status);
        };

        // "canonicalize" the locale ID to ICU/Java format
        वर्ण* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, परिमाण Nest::baseName, err);

        FixedString fullNameBuffer;
        सचे (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(परिमाण Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            सचे (!fullNameBuffer.reserve(length + 1)) {
                भङ्ग; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        सचे(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            भङ्ग;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        वर्ण* at = uprv_strchr(fullName, '@');
        याव ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        वर्ण* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        सचे (separator!=nullptr || sep2!=nullptr) {
            सचे (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } अञ्ञथा {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        तार्किक hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        सचे (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            भङ्ग; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        सचे (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        सचे (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        सचे (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } अञ्ञथा सचे (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        सचे (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } अञ्ञथा सचे (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        सचे (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } अञ्ञथा {
            सचे (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                सचे (fullNameBuffer.isEmpty()) {
                    भङ्ग; // error: out of memory
                }
                सचे (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                सचे (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                सचे (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            सचे (isBogus()) {
                भङ्ग; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            सचे (hasKeywords) {
                सचे (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    सचे (heap.ptr->baseName.isEmpty()) {
                        भङ्ग; // error: out of memory
                    }
                }
            }
        }

        सचे (canonicalize) {
            सचे (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                सचे (canonicalizeLocale(*एतत्, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), मिच्छा);
                }
                सचे (U_FAILURE(err)) {
                    भङ्ग;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        पटिनिवत्त *एतत्;
    } याव(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    पटिनिवत्त *एतत्;
}

int32_t
Locale::hashCode() ध्रुव
{
    पटिनिवत्त ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

शून्य
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

ध्रुव Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        सचे (gDefaultLocale != nullptr) {
            पटिनिवत्त *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    पटिनिवत्त *locale_set_default_internal(nullptr, status);
}



शून्य U_EXPORT2
Locale::setDefault( ध्रुव   Locale&     newLocale,
                            UErrorCode&  status)
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ध्रुव वर्ण *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

शून्य
Locale::addLikelySubtags(UErrorCode& status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    सचे (U_FAILURE(status)) {
        सचे (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        पटिनिवत्त;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/मिच्छा);
    सचे (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

शून्य
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(मिच्छा, status);
}
शून्य
Locale::minimizeSubtags(तार्किक favorScript, UErrorCode& status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    सचे (U_FAILURE(status)) {
        सचे (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        पटिनिवत्त;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/मिच्छा);
    सचे (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

शून्य
Locale::canonicalize(UErrorCode& status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
    सचे (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }
    CharString uncanonicalized(getName(), status);
    सचे (U_FAILURE(status)) {
        सचे (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        पटिनिवत्त;
    }
    init(uncanonicalized.data(), /*canonicalize=*/सच्च);
    सचे (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    सचे (U_FAILURE(status)) {
        पटिनिवत्त result;
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

    सचे (U_FAILURE(status)) {
        पटिनिवत्त result;
    }

    सचे (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त result;
    }

    result.init(localeID.data(), /*canonicalize=*/मिच्छा);
    सचे (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    पटिनिवत्त result;
}

शून्य
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) ध्रुव
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }

    सचे (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/मिच्छा, status);
}

Locale U_EXPORT2
Locale::createFromName (ध्रुव वर्ण *name)
{
    सचे (name) {
        Locale l("");
        l.init(name, मिच्छा);
        पटिनिवत्त l;
    }
    अञ्ञथा {
        पटिनिवत्त getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, मिच्छा);
    पटिनिवत्त loc;
}

Locale U_EXPORT2
Locale::createCanonical(ध्रुव वर्ण* name) {
    Locale loc("");
    loc.init(name, सच्च);
    पटिनिवत्त loc;
}

ध्रुव वर्ण *
Locale::getISO3Language() ध्रुव
{
    पटिनिवत्त uloc_getISO3Language(getName());
}


ध्रुव वर्ण *
Locale::getISO3Country() ध्रुव
{
    पटिनिवत्त uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() ध्रुव
{
    पटिनिवत्त uloc_getLCID(getName());
}

ध्रुव वर्ण* ध्रुव* U_EXPORT2 Locale::getISOCountries()
{
    पटिनिवत्त uloc_getISOCountries();
}

ध्रुव वर्ण* ध्रुव* U_EXPORT2 Locale::getISOLanguages()
{
    पटिनिवत्त uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
शून्य Locale::setFromPOSIXID(ध्रुव वर्ण *posixID)
{
    init(posixID, सच्च);
}

ध्रुव Locale & U_EXPORT2
Locale::get_मूलम्()
{
    पटिनिवत्त getLocale(e_मूलम्);
}

ध्रुव Locale & U_EXPORT2
Locale::getEnglish()
{
    पटिनिवत्त getLocale(eENGLISH);
}

ध्रुव Locale & U_EXPORT2
Locale::getFrench()
{
    पटिनिवत्त getLocale(eFRENCH);
}

ध्रुव Locale & U_EXPORT2
Locale::getGerman()
{
    पटिनिवत्त getLocale(eGERMAN);
}

ध्रुव Locale & U_EXPORT2
Locale::getItalian()
{
    पटिनिवत्त getLocale(eITALIAN);
}

ध्रुव Locale & U_EXPORT2
Locale::getJapanese()
{
    पटिनिवत्त getLocale(eJAPANESE);
}

ध्रुव Locale & U_EXPORT2
Locale::getKorean()
{
    पटिनिवत्त getLocale(eKOREAN);
}

ध्रुव Locale & U_EXPORT2
Locale::getChinese()
{
    पटिनिवत्त getLocale(eCHINESE);
}

ध्रुव Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    पटिनिवत्त getLocale(eCHINA);
}

ध्रुव Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    पटिनिवत्त getLocale(eTAIWAN);
}


ध्रुव Locale & U_EXPORT2
Locale::getFrance()
{
    पटिनिवत्त getLocale(eFRANCE);
}

ध्रुव Locale & U_EXPORT2
Locale::getGermany()
{
    पटिनिवत्त getLocale(eGERMANY);
}

ध्रुव Locale & U_EXPORT2
Locale::getItaly()
{
    पटिनिवत्त getLocale(eITALY);
}

ध्रुव Locale & U_EXPORT2
Locale::getJapan()
{
    पटिनिवत्त getLocale(eJAPAN);
}

ध्रुव Locale & U_EXPORT2
Locale::getKorea()
{
    पटिनिवत्त getLocale(eKOREA);
}

ध्रुव Locale & U_EXPORT2
Locale::getChina()
{
    पटिनिवत्त getLocale(eCHINA);
}

ध्रुव Locale & U_EXPORT2
Locale::getPRC()
{
    पटिनिवत्त getLocale(eCHINA);
}

ध्रुव Locale & U_EXPORT2
Locale::getTaiwan()
{
    पटिनिवत्त getLocale(eTAIWAN);
}

ध्रुव Locale & U_EXPORT2
Locale::getUK()
{
    पटिनिवत्त getLocale(eUK);
}

ध्रुव Locale & U_EXPORT2
Locale::getUS()
{
    पटिनिवत्त getLocale(eUS);
}

ध्रुव Locale & U_EXPORT2
Locale::getCanada()
{
    पटिनिवत्त getLocale(eCANADA);
}

ध्रुव Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    पटिनिवत्त getLocale(eCANADA_FRENCH);
}

ध्रुव Locale &
Locale::getLocale(पूर्णाङ्क locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    सचे (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    पटिनिवत्त localeCache[locid]; /*operating on nullptr*/
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
    पटिनिवत्त gLocaleCache;
}

वग्ग KeywordEnumeration : सार्वजनिक StringEnumeration {
रक्षित:
    FixedString keywords;
गुप्त:
    int32_t length;
    ध्रुव वर्ण *current;
    स्थिर ध्रुव वर्ण fgClassID;

सार्वजनिक:
    स्थिर UClassID U_EXPORT2 getStaticClassID() { पटिनिवत्त (UClassID)&fgClassID; }
    आभास UClassID getDynamicClassID() ध्रुव override { पटिनिवत्त getStaticClassID(); }
सार्वजनिक:
    KeywordEnumeration(ध्रुव वर्ण *कुञ्चिकाः, int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        सचे(U_SUCCESS(status) && keywordLen != 0) {
            सचे(कुञ्चिकाः == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } अञ्ञथा {
                keywords = {कुञ्चिकाः, static_cast<std::string_view::size_type>(length)};
                सचे (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अञ्ञथा {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    आभास ~KeywordEnumeration();

    आभास StringEnumeration * clone() ध्रुव override
    {
        UErrorCode status = U_ZERO_ERROR;
        पटिनिवत्त नूतन KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    आभास int32_t count(UErrorCode& status) ध्रुव override {
        सचे (U_FAILURE(status)) { पटिनिवत्त 0; }
        ध्रुव वर्ण *kw = keywords.data();
        int32_t result = 0;
        याव(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        पटिनिवत्त result;
    }

    आभास ध्रुव वर्ण* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ध्रुव वर्ण* result;
        int32_t len;
        सचे(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            सचे(resultLength != nullptr) {
                *resultLength = len;
            }
        } अञ्ञथा {
            सचे(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        पटिनिवत्त result;
    }

    आभास ध्रुव UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        सचे (U_FAILURE(status)) { पटिनिवत्त nullptr; }
        int32_t resultLength = 0;
        ध्रुव वर्ण *s = next(&resultLength, status);
        पटिनिवत्त setChars(s, resultLength, status);
    }

    आभास शून्य reset(UErrorCode& status) override {
        सचे (U_FAILURE(status)) { पटिनिवत्त; }
        current = keywords.data();
    }
};

ध्रुव वर्ण KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = स्वभाव;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
वग्ग UnicodeKeywordEnumeration : सार्वजनिक KeywordEnumeration {
सार्वजनिक:
    using KeywordEnumeration::KeywordEnumeration;
    आभास ~UnicodeKeywordEnumeration();

    आभास ध्रुव वर्ण* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ध्रुव वर्ण* legacy_कुञ्चिका = KeywordEnumeration::next(nullptr, status);
        याव (U_SUCCESS(status) && legacy_कुञ्चिका != nullptr) {
            ध्रुव वर्ण* कुञ्चिका = uloc_to_Unicode_Locale_कुञ्चिका(legacy_कुञ्चिका);
            सचे (कुञ्चिका != nullptr) {
                सचे (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(कुञ्चिका));
                }
                पटिनिवत्त कुञ्चिका;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_कुञ्चिका = KeywordEnumeration::next(nullptr, status);
        }
        सचे (resultLength != nullptr) *resultLength = 0;
        पटिनिवत्त nullptr;
    }
    आभास int32_t count(UErrorCode& status) ध्रुव override {
        सचे (U_FAILURE(status)) { पटिनिवत्त 0; }
        ध्रुव वर्ण *kw = keywords.data();
        int32_t result = 0;
        याव(*kw) {
            सचे (uloc_to_Unicode_Locale_कुञ्चिका(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        पटिनिवत्त result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = स्वभाव;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) ध्रुव
{
    StringEnumeration *result = nullptr;

    सचे (U_FAILURE(status)) {
        पटिनिवत्त result;
    }

    ध्रुव वर्ण* variantStart = uprv_strchr(getName(), '@');
    ध्रुव वर्ण* assignment = uprv_strchr(getName(), '=');
    सचे(variantStart) {
        सचे(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', मिच्छा, status);
            सचे (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = नूतन KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                सचे (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } अञ्ञथा {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    पटिनिवत्त result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) ध्रुव
{
    StringEnumeration *result = nullptr;

    सचे (U_FAILURE(status)) {
        पटिनिवत्त result;
    }

    ध्रुव वर्ण* variantStart = uprv_strchr(getName(), '@');
    ध्रुव वर्ण* assignment = uprv_strchr(getName(), '=');
    सचे(variantStart) {
        सचे(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', मिच्छा, status);
            सचे (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = नूतन UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                सचे (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } अञ्ञथा {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    पटिनिवत्त result;
}

int32_t
Locale::get_Keyword_मानम्(ध्रुव वर्ण* keywordName, वर्ण *buffer, int32_t bufLen, UErrorCode &status) ध्रुव
{
    पटिनिवत्त uloc_get_Keyword_मानम्(getName(), keywordName, buffer, bufLen, &status);
}

शून्य
Locale::get_Keyword_मानम्(StringPiece keywordName, ByteSink& sink, UErrorCode& status) ध्रुव {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }

    सचे (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }

    ulocimp_get_Keyword_मानम्(getName(), keywordName, sink, status);
}

शून्य
Locale::get_Unicode_Keyword_मानम्(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) ध्रुव {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }

    std::optional<std::string_view> legacy_कुञ्चिका = ulocimp_to_Legacy_कुञ्चिका_With_Fallback(keywordName);
    सचे (!legacy_कुञ्चिका.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }

    auto legacy_मानम् = get_Keyword_मानम्<CharString>(*legacy_कुञ्चिका, status);

    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }

    std::optional<std::string_view> unicode_मानम् =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_मानम्.toStringPiece());
    सचे (!unicode_मानम्.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }

    sink.Append(unicode_मानम्->data(), static_cast<int32_t>(unicode_मानम्->size()));
}

शून्य
Locale::set_Keyword_मानम्(StringPiece keywordName,
                        StringPiece keyword_मानम्,
                        UErrorCode& status) {
    सचे (U_FAILURE(status)) { पटिनिवत्त; }
    सचे (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }
    सचे (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_set_Keyword_मानम्(keywordName, keyword_मानम्, localeID, status);
    सचे (U_FAILURE(status)) {
        सचे (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        पटिनिवत्त;
    }

    ध्रुव वर्ण* at = locale_getKeywordsStart(localeID.toStringPiece());
    तार्किक hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    सचे (!hasKeywords) {
        सचे (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            सचे (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < परिमाण Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, परिमाण Nest::baseName, status);
            } अञ्ञथा {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                सचे (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    पटिनिवत्त;
                }
            }
        }
    } अञ्ञथा {
        Heap* heap = nullptr;
        सचे (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            सचे (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त;
            }
        } अञ्ञथा {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        सचे (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            पटिनिवत्त;
        }

        सचे (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            सचे (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                सचे (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    पटिनिवत्त;
                }
            }
        }
    }
}

शून्य
Locale::set_Unicode_Keyword_मानम्(StringPiece keywordName,
                               StringPiece keyword_मानम्,
                               UErrorCode& status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }

    std::optional<std::string_view> legacy_कुञ्चिका = ulocimp_to_Legacy_कुञ्चिका_With_Fallback(keywordName);
    सचे (!legacy_कुञ्चिका.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }

    std::string_view value;

    सचे (!keyword_मानम्.empty()) {
        std::optional<std::string_view> legacy_मानम् =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_मानम्);
        सचे (!legacy_मानम्.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त;
        }
        value = *legacy_मानम्;
    }

    set_Keyword_मानम्(*legacy_कुञ्चिका, value, status);
}

ध्रुव वर्ण*
Locale::getCountry() ध्रुव U_LIFETIME_BOUND {
    पटिनिवत्त getField<&Nest::getRegion, &Heap::getRegion>();
}

ध्रुव वर्ण*
Locale::getLanguage() ध्रुव U_LIFETIME_BOUND {
    पटिनिवत्त getField<&Nest::getLanguage, &Heap::getLanguage>();
}

ध्रुव वर्ण*
Locale::getScript() ध्रुव U_LIFETIME_BOUND {
    पटिनिवत्त getField<&Nest::getScript, &Heap::getScript>();
}

ध्रुव वर्ण*
Locale::getVariant() ध्रुव U_LIFETIME_BOUND {
    पटिनिवत्त getField<&Nest::getVariant, &Heap::getVariant>();
}

ध्रुव वर्ण*
Locale::getName() ध्रुव U_LIFETIME_BOUND {
    पटिनिवत्त getField<&Nest::getBaseName, &Heap::getFullName>();
}

ध्रुव वर्ण*
Locale::getBaseName() ध्रुव U_LIFETIME_BOUND {
    पटिनिवत्त getField<&Nest::getBaseName, &Heap::getBaseName>();
}

template <ध्रुव वर्ण* (Locale::Nest::*ध्रुव NEST)() ध्रुव,
          ध्रुव वर्ण* (Locale::Heap::*ध्रुव HEAP)() ध्रुव>
ध्रुव वर्ण* Locale::getField() ध्रुव U_LIFETIME_BOUND {
    पटिनिवत्त payload.visit([] { पटिनिवत्त ""; },
                         [](ध्रुव Nest& nest) { पटिनिवत्त (nest.*NEST)(); },
                         [](ध्रुव Heap& heap) { पटिनिवत्त (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = स्वभाव;

//eof
U_NAMESPACE_END
