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

#जर UPRV_HAS_FEATURE(memory_sanitizer)
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

प्रकारव्याख्या गणना ELocalePos {
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
    e_मूळ,


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

    जर (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    परत सत्य;
}

शून्य U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = नवीन Locale[static_cast<पूर्णांक>(eMAX_LOCALES)];
    जर (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_मूळ]          = Locale("");
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

Locale *locale_set_default_internal(स्थिरांक अक्षर *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = असत्य;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    जर (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = सत्य; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    जर (U_FAILURE(status)) {
        परत gDefaultLocale;
    }

    जर (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        जर (U_FAILURE(status)) {
            परत gDefaultLocale;
        }
        uhash_set_मूल्य_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    जर (newDefault == nullptr) {
        newDefault = नवीन Locale(Locale::eBOGUS);
        जर (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            परत gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), असत्य);
        uhash_put(gDefaultLocalesHashT, const_cast<अक्षर*>(newDefault->getName()), newDefault, &status);
        जर (U_FAILURE(status)) {
            परत gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    परत gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC शून्य
locale_set_default(स्थिरांक अक्षर *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC स्थिरांक अक्षर *
locale_get_default()
{
    U_NAMESPACE_USE
    परत Locale::getDefault().getName();
}

नामाकाश {

ढाँचा <स्वतः FIELD, typename T>
शून्य copyToArray(std::string_view sv, T* that) {
    स्वतः& field = that->*FIELD;
    constexpr माप_प्रकार capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    जर (!sv.empty()) {
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
    copyToArray<&Nest::language>(language, हे);
    copyToArray<&Nest::script>(script, हे);
    copyToArray<&Nest::region>(region, हे);
    हे->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(दुरत्व(Nest, region) <= दुरत्व(Heap, script));
    static_assert(दुरत्व(Nest, variantBegin) <= दुरत्व(Heap, region));
    U_ASSERT(हे == reinterpret_cast<Nest*>(&heap));
#जर UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, आकार(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, हे);
    copyToArray<&Nest::region>(heap.region, हे);
    हे->variantBegin = variantBegin;
    *हे->baseName = '\0';
}

संरचना Locale::Heap::Alloc : सार्वजनिक UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    स्थिरांक अक्षर* getVariant() स्थिरांक U_LIFETIME_BOUND {
        परत variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    स्थिरांक अक्षर* getFullName() स्थिरांक U_LIFETIME_BOUND { परत fullName.data(); }
    स्थिरांक अक्षर* getBaseName() स्थिरांक U_LIFETIME_BOUND {
        जर (baseName.isEmpty()) {
            जर (स्थिरांक अक्षर* name = fullName.data(); *name != '@') {
                परत name;
            }
        }
        परत baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(स्थिरांक Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        जर (U_SUCCESS(status)) {
            जर (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                जर (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अन्यथा {
                    जर (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        जर (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = delete;

    ~Alloc() = पूर्वनिर्धारित;
};

स्थिरांक अक्षर* Locale::Heap::getVariant() स्थिरांक U_LIFETIME_BOUND { परत ptr->getVariant(); }
स्थिरांक अक्षर* Locale::Heap::getFullName() स्थिरांक U_LIFETIME_BOUND { परत ptr->getFullName(); }
स्थिरांक अक्षर* Locale::Heap::getBaseName() स्थिरांक U_LIFETIME_BOUND { परत ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = नवीन Alloc(variantBegin);
    जर (ptr == nullptr) {
        type = eBOGUS;
    } अन्यथा {
        type = eHEAP;
        copyToArray<&Heap::language>(language, हे);
        copyToArray<&Heap::script>(script, हे);
        copyToArray<&Heap::region>(region, हे);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::चालक=(स्थिरांक Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = नवीन Alloc(*other.ptr, status);
    जर (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } अन्यथा {
        type = eHEAP;
        uprv_memcpy(language, other.language, आकार language);
        uprv_memcpy(script, other.script, आकार script);
        uprv_memcpy(region, other.region, आकार region);
    }
    परत *हे;
}

Locale::Heap& Locale::Heap::चालक=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, आकार language);
    uprv_memcpy(script, other.script, आकार script);
    uprv_memcpy(region, other.region, आकार region);
    परत *हे;
}

ढाँचा <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
स्वतः Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) स्थिरांक {
    निवड (type) {
        स्थिती eBOGUS:
            परत bogusFn(args...);
        स्थिती eNEST:
            परत nestFn(nest, args...);
        स्थिती eHEAP:
            परत heapFn(heap, args...);
        पूर्वनिर्धारित:
            UPRV_UNREACHABLE_EXIT;
    };
}

शून्य Locale::Payload::copy(स्थिरांक Payload& other) {
    other.visit([](Payload*) {},
                [](स्थिरांक Nest& nest, Payload* dst) { dst->nest = nest; },
                [](स्थिरांक Heap& heap, Payload* dst) { dst->heap = heap; },
                हे);
}

शून्य Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](स्थिरांक Nest& nest, Payload* dst) { dst->nest = nest; },
        [](स्थिरांक Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        हे);
}

Locale::Payload::~Payload() {
    जर (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(स्थिरांक Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::चालक=(स्थिरांक Payload& other) U_LIFETIME_BOUND {
    जर (हे != &other) {
        setToBogus();
        copy(other);
    }
    परत *हे;
}

Locale::Payload& Locale::Payload::चालक=(Payload&& other) noexcept U_LIFETIME_BOUND {
    जर (हे != &other) {
        setToBogus();
        move(std::move(other));
    }
    परत *हे;
}

शून्य Locale::Payload::setToBogus() {
    हे->~Payload();
    type = eBOGUS;
}

ढाँचा <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    जर constexpr (std::is_same_v<T, Nest>) {
        हे->~Payload();
        ::नवीन (&nest) Nest(std::forward<Args>(args)...);
        परत nest;
    }
    जर constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::नवीन (&heap) Heap(std::forward<Args>(args)...);
        परत heap;
    }
}

ढाँचा <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    परत type == eNEST ? &nest : nullptr;
}
ढाँचा <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    परत type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = पूर्वनिर्धारित;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, असत्य);
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


Locale::Locale( स्थिरांक   अक्षर * newLanguage,
                स्थिरांक   अक्षर * newCountry,
                स्थिरांक   अक्षर * newVariant,
                स्थिरांक   अक्षर * newKeywords)
    : UObject(), payload()
{
    जर( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, असत्य); /* shortcut */
    }
    अन्यथा
    {
        UErrorCode status = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        जर ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            जर ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                परत;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        जर ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            जर ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                परत;
            }
        }

        // _Variant
        जर ( newVariant != nullptr )
        {
            // remove leading _'s
            पर्यंत(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            जर ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                परत;
            }
            पर्यंत( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        जर ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            जर ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              परत;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        जर ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        जर ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        जर ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        जर ( ksize != 0)
        {
            जर (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            अन्यथा {
                togo.append('_', status); /* Variant parsing with a script */
                जर ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        जर (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            परत;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), असत्य);
    }
}

Locale::Locale(स्थिरांक Locale&) = पूर्वनिर्धारित;
Locale::Locale(Locale&&) noexcept = पूर्वनिर्धारित;

Locale& Locale::चालक=(स्थिरांक Locale&) U_LIFETIME_BOUND = पूर्वनिर्धारित;
Locale& Locale::चालक=(Locale&&) noexcept U_LIFETIME_BOUND = पूर्वनिर्धारित;

Locale *
Locale::clone() स्थिरांक {
    परत नवीन Locale(*हे);
}

तार्किक
Locale::चालक==( स्थिरांक   Locale& other) स्थिरांक
{
    परत uprv_strcmp(other.getName(), getName()) == 0;
}

नामाकाश {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr स्थिरांक अक्षर* KNOWN_CANONICALIZED[] = {
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
    जर (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    परत सत्य;
}

शून्य U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    साठी (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (शून्य*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    जर (U_FAILURE(status)) {
        परत;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

वर्ग AliasData;

/**
 * A Builder class to build the alias data.
 */
वर्ग AliasDataBuilder {
सार्वजनिक:
    AliasDataBuilder() {
    }

    // Build the AliasData from resource.
    AliasData* build(UErrorCode &status);

खाजगी:
    शून्य readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<स्थिरांक अक्षर*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   शून्य (*checkType)(स्थिरांक अक्षर* type),
                   शून्य (*checkReplacement)(स्थिरांक UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    शून्य readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<स्थिरांक अक्षर*>& types,
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
                         LocalMemory<स्थिरांक अक्षर*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    शून्य readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<स्थिरांक अक्षर*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    शून्य readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<स्थिरांक अक्षर*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    शून्य readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<स्थिरांक अक्षर*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
वर्ग AliasData : सार्वजनिक UMemory {
सार्वजनिक:
    स्थिर स्थिरांक AliasData* singleton(UErrorCode& status) {
        जर (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            परत nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        परत gSingleton;
    }

    स्थिरांक CharStringMap& languageMap() स्थिरांक U_LIFETIME_BOUND { परत language; }
    स्थिरांक CharStringMap& scriptMap() स्थिरांक U_LIFETIME_BOUND { परत script; }
    स्थिरांक CharStringMap& territoryMap() स्थिरांक U_LIFETIME_BOUND { परत territory; }
    स्थिरांक CharStringMap& variantMap() स्थिरांक U_LIFETIME_BOUND { परत variant; }
    स्थिरांक CharStringMap& subdivisionMap() स्थिरांक U_LIFETIME_BOUND { परत subdivision; }

    स्थिर शून्य U_CALLCONV loadData(UErrorCode &status);
    स्थिर UBool U_CALLCONV cleanup();

    स्थिर UInitOnce gInitOnce;

खाजगी:
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

    स्थिर स्थिरांक AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    मित्र वर्ग AliasDataBuilder;
};


स्थिरांक AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    परत सत्य;
}

शून्य
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<स्थिरांक अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        शून्य (*checkType)(स्थिरांक अक्षर* type),
        शून्य (*checkReplacement)(स्थिरांक UChar* replacement),
        UErrorCode &status) {
    जर (U_FAILURE(status)) {
        परत;
    }
    length = ures_getSize(alias);
    स्थिरांक अक्षर** rawTypes = types.allocateInsteadAndCopy(length);
    जर (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    जर (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    साठी (पूर्णांक i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        स्थिरांक अक्षर* aliasFrom = ures_get_किल्ली(res.getAlias());
        स्थिरांक UChar* aliasTo =
            ures_get_String_By_किल्ली(res.getAlias(), "replacement", nullptr, &status);
        जर (U_FAILURE(status)) परत;

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
        LocalMemory<स्थिरांक अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    परत readAlias(
        alias, strings, types, replacementIndexes, length,
#जर U_DEBUG
        [](स्थिरांक अक्षर* type) {
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
#अन्यथा
        [](स्थिरांक अक्षर*) {},
#endif
        [](स्थिरांक UChar*) {}, status);
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
        LocalMemory<स्थिरांक अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    परत readAlias(
        alias, strings, types, replacementIndexes, length,
#जर U_DEBUG
        [](स्थिरांक अक्षर* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](स्थिरांक UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#अन्यथा
        [](स्थिरांक अक्षर*) {},
        [](स्थिरांक UChar*) { },
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
        LocalMemory<स्थिरांक अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    परत readAlias(
        alias, strings, types, replacementIndexes, length,
#जर U_DEBUG
        [](स्थिरांक अक्षर* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#अन्यथा
        [](स्थिरांक अक्षर*) {},
#endif
        [](स्थिरांक UChar*) { },
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
        LocalMemory<स्थिरांक अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    परत readAlias(
        alias, strings, types, replacementIndexes, length,
#जर U_DEBUG
        [](स्थिरांक अक्षर* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](स्थिरांक UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#अन्यथा
        [](स्थिरांक अक्षर*) {},
        [](स्थिरांक UChar*) { },
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
        LocalMemory<स्थिरांक अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    परत readAlias(
        alias, strings, types, replacementIndexes, length,
#जर U_DEBUG
        [](स्थिरांक अक्षर* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#अन्यथा
        [](स्थिरांक अक्षर*) {},
#endif
        [](स्थिरांक UChar*) { },
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
    म_लिखो("AliasData::loadData took total %f ms\n", end - start);
#endif  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
AliasData*
AliasDataBuilder::build(UErrorCode &status) {
    जर (U_FAILURE(status)) { परत nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_किल्ली(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_किल्ली(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_किल्ली(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_किल्ली(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_किल्ली(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_किल्ली(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    जर (U_FAILURE(status)) {
        परत nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<स्थिरांक अक्षर*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<स्थिरांक अक्षर*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<स्थिरांक अक्षर*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<स्थिरांक अक्षर*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<स्थिरांक अक्षर*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    जर (U_FAILURE(status)) {
        परत nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    साठी (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    साठी (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    साठी (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    साठी (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    साठी (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    जर (U_FAILURE(status)) {
        परत nullptr;
    }

    // copy hashtables
    स्वतः *data = नवीन AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    जर (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    परत data;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
वर्ग AliasReplacer {
सार्वजनिक:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       परत 0==uprv_strcmp((स्थिरांक अक्षर*)e1.pointer,
                                             (स्थिरांक अक्षर*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    तार्किक replace(
        स्थिरांक Locale& locale, CharString& out, UErrorCode& status);

खाजगी:
    स्थिरांक अक्षर* language;
    स्थिरांक अक्षर* script;
    स्थिरांक अक्षर* region;
    स्थिरांक अक्षर* extensions;
    UVector variants;

    स्थिरांक AliasData* data;

    ओळीत तार्किक notEmpty(स्थिरांक अक्षर* str) {
        परत str && str[0] != NULL_CHAR;
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
    ओळीत स्थिरांक अक्षर* deleteOrReplace(स्थिरांक अक्षर* input U_LIFETIME_BOUND,
                                       स्थिरांक अक्षर* type,
                                       स्थिरांक अक्षर* replacement U_LIFETIME_BOUND) {
        परत notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    ओळीत तार्किक same(स्थिरांक अक्षर* a, स्थिरांक अक्षर* b) {
        जर (a == nullptr && b == nullptr) {
            परत सत्य;
        }
        जर ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          परत असत्य;
        }
        परत uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_किल्ली(स्थिरांक अक्षर* language, स्थिरांक अक्षर* region,
                            स्थिरांक अक्षर* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    शून्य parseLanguageReplacement(स्थिरांक अक्षर* replacement,
                                  स्थिरांक अक्षर*& replaceLanguage,
                                  स्थिरांक अक्षर*& replaceScript,
                                  स्थिरांक अक्षर*& replaceRegion,
                                  स्थिरांक अक्षर*& replaceVariant,
                                  स्थिरांक अक्षर*& replaceExtensions,
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
AliasReplacer::generate_किल्ली(
        स्थिरांक अक्षर* language, स्थिरांक अक्षर* region, स्थिरांक अक्षर* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    जर (U_FAILURE(status)) { परत out; }
    out.append(language, status);
    जर (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    जर (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    परत out;
}

शून्य
AliasReplacer::parseLanguageReplacement(
    स्थिरांक अक्षर* replacement,
    स्थिरांक अक्षर*& replacedLanguage,
    स्थिरांक अक्षर*& replacedScript,
    स्थिरांक अक्षर*& replacedRegion,
    स्थिरांक अक्षर*& replacedVariant,
    स्थिरांक अक्षर*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    जर (U_FAILURE(status)) {
        परत;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    जर (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        परत;
    }
    // We have multiple field so we have to allocate and parse
    स्वतः str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    अक्षर* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    जर (U_FAILURE(status)) {
        परत;
    }
    replacedLanguage = (स्थिरांक अक्षर*) data;
    अक्षर* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    स्थिरांक अक्षर* start = endOfField;
    endOfField = const_cast<अक्षर*>(uprv_strchr(start, '_'));
    माप_प्रकार len = 0;
    जर (endOfField == nullptr) {
        len = uprv_strlen(start);
    } अन्यथा {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    जर (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        जर (endOfField == nullptr) {
            परत;
        }
        start = endOfField++;
        endOfField = const_cast<अक्षर*>(uprv_strchr(start, '_'));
        जर (endOfField == nullptr) {
            len = uprv_strlen(start);
        } अन्यथा {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    जर (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        जर (endOfField == nullptr) {
            परत;
        }
        start = endOfField++;
        endOfField = const_cast<अक्षर*>(uprv_strchr(start, '_'));
        जर (endOfField == nullptr) {
            len = uprv_strlen(start);
        } अन्यथा {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    जर (len >= 4) {
        // Got a variant
        replacedVariant = start;
        जर (endOfField == nullptr) {
            परत;
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
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    जर (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        परत असत्य;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    स्थिरांक अक्षर* शोध_Language = checkLanguage ? language : "und";
    स्थिरांक अक्षर* शोध_Region = checkRegion ? region : nullptr;
    स्थिरांक अक्षर* शोध_Variant = nullptr;
    साठी (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        जर (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            शोध_Variant = static_cast<स्थिरांक अक्षर*>(variants.elementAt(variant_index));
        }

        जर (शोध_Variant != nullptr && uprv_strlen(शोध_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            शोध_Variant = nullptr;
        }
        CharString type_किल्ली;
        generate_किल्ली(शोध_Language, शोध_Region, शोध_Variant, type_किल्ली,
                    status);
        जर (U_FAILURE(status)) {
            परत असत्य;
        }
        स्थिरांक अक्षर *replacement = data->languageMap().get(type_किल्ली.data());
        जर (replacement == nullptr) {
            // Found no replacement data.
            सुरूठेवा;
        }

        स्थिरांक अक्षर* replacedLanguage = nullptr;
        स्थिरांक अक्षर* replacedScript = nullptr;
        स्थिरांक अक्षर* replacedRegion = nullptr;
        स्थिरांक अक्षर* replacedVariant = nullptr;
        स्थिरांक अक्षर* replacedExtensions = nullptr;
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
        replacedRegion = deleteOrReplace(region, शोध_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            शोध_Variant, शोध_Variant, replacedVariant);

        जर (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(शोध_Variant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            सुरूठेवा;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        जर (शोध_Variant != nullptr) {
            जर (notEmpty(replacedVariant)) {
                variants.setElementAt((शून्य*)replacedVariant, variant_index);
            } अन्यथा {
                variants.removeElementAt(variant_index);
            }
        }
        जर (replacedExtensions != nullptr) {
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
        परत सत्य;
    }
    // Nothing changed by language alias data.
    परत असत्य;
}

तार्किक
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    जर (region == nullptr) {
        // No region to search.
        परत असत्य;
    }
    स्थिरांक अक्षर *replacement = data->territoryMap().get(region);
    जर (replacement == nullptr) {
        // Found no replacement data for this region.
        परत असत्य;
    }
    स्थिरांक अक्षर* replacedRegion = replacement;
    स्थिरांक अक्षर* firstSpace = uprv_strchr(replacement, ' ');
    जर (firstSpace != nullptr) {
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
        स्थिरांक अक्षर* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        जर (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            माप_प्रकार len = uprv_strlen(likelyRegion);
            स्थिरांक अक्षर* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            जर (foundInReplacement != nullptr) {
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
        जर (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        जर (U_FAILURE(status)) { परत असत्य; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        जर (U_FAILURE(status)) { परत असत्य; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    परत सत्य;
}

तार्किक
AliasReplacer::replaceScript(UErrorCode& status)
{
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    जर (script == nullptr) {
        // No script to search.
        परत असत्य;
    }
    स्थिरांक अक्षर *replacement = data->scriptMap().get(script);
    जर (replacement == nullptr) {
        // Found no replacement data for this script.
        परत असत्य;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    परत सत्य;
}

तार्किक
AliasReplacer::replaceVariant(UErrorCode& status)
{
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    // Since we may have more than one variant, we need to loop through them.
    साठी (int32_t i = 0; i < variants.size(); i++) {
        स्थिरांक अक्षर* variant = static_cast<स्थिरांक अक्षर*>(variants.elementAt(i));
        स्थिरांक अक्षर *replacement = data->variantMap().get(variant);
        जर (replacement == nullptr) {
            // Found no replacement data for this variant.
            सुरूठेवा;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        जर (!same(variant, replacement)) {
            variants.setElementAt((शून्य*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            जर (uprv_strcmp(variant, "heploc") == 0) {
                साठी (int32_t j = 0; j < variants.size(); j++) {
                     जर (uprv_strcmp((स्थिरांक अक्षर*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            परत सत्य;
        }
    }
    परत असत्य;
}

तार्किक
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    स्थिरांक अक्षर *replacement = data->subdivisionMap().get(subdivision.data());
    जर (replacement != nullptr) {
        स्थिरांक अक्षर* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        माप_प्रकार len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        जर (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            जर (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        परत सत्य;
    }
    परत असत्य;
}

तार्किक
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    int32_t len = transformedExtensions.length();
    स्थिरांक अक्षर* str = transformedExtensions.data();
    स्थिरांक अक्षर* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    जर (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        जर (U_FAILURE(status)) {
            परत असत्य;
        }
        T_CString_toLowerCase(output.data());
    }
    जर (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        जर (U_FAILURE(status)) {
            परत असत्य;
        }
        करा {
            स्थिरांक अक्षर* tvalue = uprv_strchr(tkey, '-');
            जर (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                परत असत्य;
            }
            स्थिरांक अक्षर* nextTKey = ultag_getTKeyStart(tvalue);
            जर (nextTKey != nullptr) {
                *const_cast<अक्षर*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.घाला_Element_At((शून्य*)tkey, tfields.size(), status);
            जर (U_FAILURE(status)) {
                परत असत्य;
            }
            tkey = nextTKey;
        } पर्यंत (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            परत uprv_strcmp((स्थिरांक अक्षर*)e1.pointer, (स्थिरांक अक्षर*)e2.pointer);
        }, status);
        साठी (int32_t i = 0; i < tfields.size(); i++) {
             जर (output.length() > 0) {
                 output.append('-', status);
             }
             स्थिरांक अक्षर* tfield = static_cast<स्थिरांक अक्षर*>(tfields.elementAt(i));
             स्थिरांक अक्षर* tvalue = uprv_strchr(tfield, '-');
             जर (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 परत असत्य;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<अक्षर*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    परत सत्य;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    जर (U_FAILURE(status)) { परत out; }
    out.append(language, status);
    जर (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    जर (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    जर (variants.size() > 0) {
        जर (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            परत uprv_strcmp((स्थिरांक अक्षर*)e1.pointer, (स्थिरांक अक्षर*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        साठी (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<स्थिरांक अक्षर*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    जर (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    परत out;
}

तार्किक
AliasReplacer::replace(स्थिरांक Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    U_ASSERT(data != nullptr);
    out.clear();
    language = locale.getLanguage();
    जर (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    जर (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    जर (!notEmpty(region)) {
        region = nullptr;
    }
    स्थिरांक अक्षर* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    जर (!variantsBuff.isEmpty()) {
        जर (U_FAILURE(status)) { परत असत्य; }
        अक्षर* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        अक्षर* end;
        पर्यंत ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            जर (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        जर (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    जर (U_FAILURE(status)) { परत असत्य; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        परत uprv_strcmp((स्थिरांक अक्षर*)e1.pointer, (स्थिरांक अक्षर*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    स्थिरांक int32_t maxChanged = 4 + variants.size();
    (शून्य)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](शून्य *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    पर्यंत (U_SUCCESS(status)) {
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
        जर (    replaceLanguage(सत्य, सत्य,  सत्य,  stringsToBeFreed, status) ||
                replaceLanguage(सत्य, सत्य,  असत्य, stringsToBeFreed, status) ||
                replaceLanguage(सत्य, असत्य, सत्य,  stringsToBeFreed, status) ||
                replaceLanguage(सत्य, असत्य, असत्य, stringsToBeFreed, status) ||
                replaceLanguage(असत्य,असत्य, सत्य,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            सुरूठेवा;
        }
        // Nothing changed. Break out.
        थांबा;
    }  // while(1)

    जर (U_FAILURE(status)) { परत असत्य; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    स्थिरांक अक्षर* extensionsStr = locale_getKeywordsStart(locale.getName());
    जर (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        परत असत्य;
    }
    outputToString(out, status);
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    जर (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        जर (U_SUCCESS(status) && !iter.isNull()) {
            स्थिरांक अक्षर* किल्ली;
            पर्यंत ((किल्ली = iter->next(nullptr, status)) != nullptr) {
                जर (uprv_strcmp("sd", किल्ली) == 0 || uprv_strcmp("rg", किल्ली) == 0 ||
                        uprv_strcmp("t", किल्ली) == 0) {
                    स्वतः value = locale.get_Keyword_मूल्य<CharString>(किल्ली, status);
                    जर (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        सुरूठेवा;
                    }
                    CharString replacement;
                    जर (uprv_strlen(किल्ली) == 2) {
                        जर (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.set_Keyword_मूल्य(किल्ली, replacement.data(), status);
                        }
                    } अन्यथा {
                        U_ASSERT(uprv_strcmp(किल्ली, "t") == 0);
                        जर (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.set_Keyword_मूल्य(किल्ली, replacement.data(), status);
                        }
                    }
                    जर (U_FAILURE(status)) {
                        परत असत्य;
                    }
                }
            }
        }
        जर (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    // If the tag is not changed, return.
    जर (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        परत असत्य;
    }
    परत सत्य;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
तार्किक
canonicalizeLocale(स्थिरांक Locale& locale, CharString& out, UErrorCode& status)
{
    जर (U_FAILURE(status)) { परत असत्य; }
    AliasReplacer replacer(status);
    परत replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
तार्किक
isKnownCanonicalizedLocale(स्थिरांक अक्षर* locale, UErrorCode& status)
{
    जर (U_FAILURE(status)) { परत असत्य; }

    जर (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        परत सत्य;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    परत uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT स्थिरांक अक्षर* स्थिरांक*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    परत KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT तार्किक
ulocimp_isCanonicalizedLocaleForTest(स्थिरांक अक्षर* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    परत !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(स्थिरांक अक्षर* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    परत localeID == nullptr ? *हे = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    करा {
        अक्षर *separator;
        अक्षर *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        स्थिरांक स्वतः parse = [canonicalize](std::string_view localeID,
                                          अक्षर* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            परत ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    जर (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } अन्यथा {
                        ulocimp_getName(localeID, sink, status);
                    }
                },
                status);
        };

        // "canonicalize" the locale ID to ICU/Java format
        अक्षर* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, आकार Nest::baseName, err);

        FixedString fullNameBuffer;
        जर (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(आकार Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            जर (!fullNameBuffer.reserve(length + 1)) {
                थांबा; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        जर(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            थांबा;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        अक्षर* at = uprv_strchr(fullName, '@');
        पर्यंत ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        अक्षर* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        जर (separator!=nullptr || sep2!=nullptr) {
            जर (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } अन्यथा {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        तार्किक hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        जर (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            थांबा; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        जर (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        जर (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        जर (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } अन्यथा जर (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        जर (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } अन्यथा जर (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        जर (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } अन्यथा {
            जर (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                जर (fullNameBuffer.isEmpty()) {
                    थांबा; // error: out of memory
                }
                जर (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                जर (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                जर (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            जर (isBogus()) {
                थांबा; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            जर (hasKeywords) {
                जर (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    जर (heap.ptr->baseName.isEmpty()) {
                        थांबा; // error: out of memory
                    }
                }
            }
        }

        जर (canonicalize) {
            जर (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                जर (canonicalizeLocale(*हे, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), असत्य);
                }
                जर (U_FAILURE(err)) {
                    थांबा;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        परत *हे;
    } पर्यंत(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    परत *हे;
}

int32_t
Locale::hashCode() स्थिरांक
{
    परत ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

शून्य
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

स्थिरांक Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        जर (gDefaultLocale != nullptr) {
            परत *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    परत *locale_set_default_internal(nullptr, status);
}



शून्य U_EXPORT2
Locale::setDefault( स्थिरांक   Locale&     newLocale,
                            UErrorCode&  status)
{
    जर (U_FAILURE(status)) {
        परत;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    स्थिरांक अक्षर *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

शून्य
Locale::addLikelySubtags(UErrorCode& status) {
    जर (U_FAILURE(status)) {
        परत;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    जर (U_FAILURE(status)) {
        जर (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        परत;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/असत्य);
    जर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

शून्य
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(असत्य, status);
}
शून्य
Locale::minimizeSubtags(तार्किक favorScript, UErrorCode& status) {
    जर (U_FAILURE(status)) {
        परत;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    जर (U_FAILURE(status)) {
        जर (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        परत;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/असत्य);
    जर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

शून्य
Locale::canonicalize(UErrorCode& status) {
    जर (U_FAILURE(status)) {
        परत;
    }
    जर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    CharString uncanonicalized(getName(), status);
    जर (U_FAILURE(status)) {
        जर (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        परत;
    }
    init(uncanonicalized.data(), /*canonicalize=*/सत्य);
    जर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    जर (U_FAILURE(status)) {
        परत result;
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

    जर (U_FAILURE(status)) {
        परत result;
    }

    जर (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        परत result;
    }

    result.init(localeID.data(), /*canonicalize=*/असत्य);
    जर (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    परत result;
}

शून्य
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) स्थिरांक
{
    जर (U_FAILURE(status)) {
        परत;
    }

    जर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/असत्य, status);
}

Locale U_EXPORT2
Locale::createFromName (स्थिरांक अक्षर *name)
{
    जर (name) {
        Locale l("");
        l.init(name, असत्य);
        परत l;
    }
    अन्यथा {
        परत getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, असत्य);
    परत loc;
}

Locale U_EXPORT2
Locale::createCanonical(स्थिरांक अक्षर* name) {
    Locale loc("");
    loc.init(name, सत्य);
    परत loc;
}

स्थिरांक अक्षर *
Locale::getISO3Language() स्थिरांक
{
    परत uloc_getISO3Language(getName());
}


स्थिरांक अक्षर *
Locale::getISO3Country() स्थिरांक
{
    परत uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() स्थिरांक
{
    परत uloc_getLCID(getName());
}

स्थिरांक अक्षर* स्थिरांक* U_EXPORT2 Locale::getISOCountries()
{
    परत uloc_getISOCountries();
}

स्थिरांक अक्षर* स्थिरांक* U_EXPORT2 Locale::getISOLanguages()
{
    परत uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
शून्य Locale::setFromPOSIXID(स्थिरांक अक्षर *posixID)
{
    init(posixID, सत्य);
}

स्थिरांक Locale & U_EXPORT2
Locale::get_मूळ()
{
    परत getLocale(e_मूळ);
}

स्थिरांक Locale & U_EXPORT2
Locale::getEnglish()
{
    परत getLocale(eENGLISH);
}

स्थिरांक Locale & U_EXPORT2
Locale::getFrench()
{
    परत getLocale(eFRENCH);
}

स्थिरांक Locale & U_EXPORT2
Locale::getGerman()
{
    परत getLocale(eGERMAN);
}

स्थिरांक Locale & U_EXPORT2
Locale::getItalian()
{
    परत getLocale(eITALIAN);
}

स्थिरांक Locale & U_EXPORT2
Locale::getJapanese()
{
    परत getLocale(eJAPANESE);
}

स्थिरांक Locale & U_EXPORT2
Locale::getKorean()
{
    परत getLocale(eKOREAN);
}

स्थिरांक Locale & U_EXPORT2
Locale::getChinese()
{
    परत getLocale(eCHINESE);
}

स्थिरांक Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    परत getLocale(eCHINA);
}

स्थिरांक Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    परत getLocale(eTAIWAN);
}


स्थिरांक Locale & U_EXPORT2
Locale::getFrance()
{
    परत getLocale(eFRANCE);
}

स्थिरांक Locale & U_EXPORT2
Locale::getGermany()
{
    परत getLocale(eGERMANY);
}

स्थिरांक Locale & U_EXPORT2
Locale::getItaly()
{
    परत getLocale(eITALY);
}

स्थिरांक Locale & U_EXPORT2
Locale::getJapan()
{
    परत getLocale(eJAPAN);
}

स्थिरांक Locale & U_EXPORT2
Locale::getKorea()
{
    परत getLocale(eKOREA);
}

स्थिरांक Locale & U_EXPORT2
Locale::getChina()
{
    परत getLocale(eCHINA);
}

स्थिरांक Locale & U_EXPORT2
Locale::getPRC()
{
    परत getLocale(eCHINA);
}

स्थिरांक Locale & U_EXPORT2
Locale::getTaiwan()
{
    परत getLocale(eTAIWAN);
}

स्थिरांक Locale & U_EXPORT2
Locale::getUK()
{
    परत getLocale(eUK);
}

स्थिरांक Locale & U_EXPORT2
Locale::getUS()
{
    परत getLocale(eUS);
}

स्थिरांक Locale & U_EXPORT2
Locale::getCanada()
{
    परत getLocale(eCANADA);
}

स्थिरांक Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    परत getLocale(eCANADA_FRENCH);
}

स्थिरांक Locale &
Locale::getLocale(पूर्णांक locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    जर (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    परत localeCache[locid]; /*operating on nullptr*/
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
    परत gLocaleCache;
}

वर्ग KeywordEnumeration : सार्वजनिक StringEnumeration {
संरक्षित:
    FixedString keywords;
खाजगी:
    int32_t length;
    स्थिरांक अक्षर *current;
    स्थिर स्थिरांक अक्षर fgClassID;

सार्वजनिक:
    स्थिर UClassID U_EXPORT2 getStaticClassID() { परत (UClassID)&fgClassID; }
    आभासी UClassID getDynamicClassID() स्थिरांक override { परत getStaticClassID(); }
सार्वजनिक:
    KeywordEnumeration(स्थिरांक अक्षर *किल्ल्या, int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        जर(U_SUCCESS(status) && keywordLen != 0) {
            जर(किल्ल्या == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } अन्यथा {
                keywords = {किल्ल्या, static_cast<std::string_view::size_type>(length)};
                जर (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अन्यथा {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    आभासी ~KeywordEnumeration();

    आभासी StringEnumeration * clone() स्थिरांक override
    {
        UErrorCode status = U_ZERO_ERROR;
        परत नवीन KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    आभासी int32_t count(UErrorCode& status) स्थिरांक override {
        जर (U_FAILURE(status)) { परत 0; }
        स्थिरांक अक्षर *kw = keywords.data();
        int32_t result = 0;
        पर्यंत(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        परत result;
    }

    आभासी स्थिरांक अक्षर* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        स्थिरांक अक्षर* result;
        int32_t len;
        जर(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            जर(resultLength != nullptr) {
                *resultLength = len;
            }
        } अन्यथा {
            जर(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        परत result;
    }

    आभासी स्थिरांक UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        जर (U_FAILURE(status)) { परत nullptr; }
        int32_t resultLength = 0;
        स्थिरांक अक्षर *s = next(&resultLength, status);
        परत setChars(s, resultLength, status);
    }

    आभासी शून्य reset(UErrorCode& status) override {
        जर (U_FAILURE(status)) { परत; }
        current = keywords.data();
    }
};

स्थिरांक अक्षर KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = पूर्वनिर्धारित;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
वर्ग UnicodeKeywordEnumeration : सार्वजनिक KeywordEnumeration {
सार्वजनिक:
    using KeywordEnumeration::KeywordEnumeration;
    आभासी ~UnicodeKeywordEnumeration();

    आभासी स्थिरांक अक्षर* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        स्थिरांक अक्षर* legacy_किल्ली = KeywordEnumeration::next(nullptr, status);
        पर्यंत (U_SUCCESS(status) && legacy_किल्ली != nullptr) {
            स्थिरांक अक्षर* किल्ली = uloc_to_Unicode_Locale_किल्ली(legacy_किल्ली);
            जर (किल्ली != nullptr) {
                जर (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(किल्ली));
                }
                परत किल्ली;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_किल्ली = KeywordEnumeration::next(nullptr, status);
        }
        जर (resultLength != nullptr) *resultLength = 0;
        परत nullptr;
    }
    आभासी int32_t count(UErrorCode& status) स्थिरांक override {
        जर (U_FAILURE(status)) { परत 0; }
        स्थिरांक अक्षर *kw = keywords.data();
        int32_t result = 0;
        पर्यंत(*kw) {
            जर (uloc_to_Unicode_Locale_किल्ली(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        परत result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = पूर्वनिर्धारित;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) स्थिरांक
{
    StringEnumeration *result = nullptr;

    जर (U_FAILURE(status)) {
        परत result;
    }

    स्थिरांक अक्षर* variantStart = uprv_strchr(getName(), '@');
    स्थिरांक अक्षर* assignment = uprv_strchr(getName(), '=');
    जर(variantStart) {
        जर(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', असत्य, status);
            जर (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = नवीन KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                जर (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } अन्यथा {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    परत result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) स्थिरांक
{
    StringEnumeration *result = nullptr;

    जर (U_FAILURE(status)) {
        परत result;
    }

    स्थिरांक अक्षर* variantStart = uprv_strchr(getName(), '@');
    स्थिरांक अक्षर* assignment = uprv_strchr(getName(), '=');
    जर(variantStart) {
        जर(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', असत्य, status);
            जर (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = नवीन UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                जर (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } अन्यथा {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    परत result;
}

int32_t
Locale::get_Keyword_मूल्य(स्थिरांक अक्षर* keywordName, अक्षर *buffer, int32_t bufLen, UErrorCode &status) स्थिरांक
{
    परत uloc_get_Keyword_मूल्य(getName(), keywordName, buffer, bufLen, &status);
}

शून्य
Locale::get_Keyword_मूल्य(StringPiece keywordName, ByteSink& sink, UErrorCode& status) स्थिरांक {
    जर (U_FAILURE(status)) {
        परत;
    }

    जर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }

    ulocimp_get_Keyword_मूल्य(getName(), keywordName, sink, status);
}

शून्य
Locale::get_Unicode_Keyword_मूल्य(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) स्थिरांक {
    जर (U_FAILURE(status)) {
        परत;
    }

    std::optional<std::string_view> legacy_किल्ली = ulocimp_to_Legacy_किल्ली_With_Fallback(keywordName);
    जर (!legacy_किल्ली.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }

    स्वतः legacy_मूल्य = get_Keyword_मूल्य<CharString>(*legacy_किल्ली, status);

    जर (U_FAILURE(status)) {
        परत;
    }

    std::optional<std::string_view> unicode_मूल्य =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_मूल्य.toStringPiece());
    जर (!unicode_मूल्य.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }

    sink.Append(unicode_मूल्य->data(), static_cast<int32_t>(unicode_मूल्य->size()));
}

शून्य
Locale::set_Keyword_मूल्य(StringPiece keywordName,
                        StringPiece keyword_मूल्य,
                        UErrorCode& status) {
    जर (U_FAILURE(status)) { परत; }
    जर (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    जर (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_set_Keyword_मूल्य(keywordName, keyword_मूल्य, localeID, status);
    जर (U_FAILURE(status)) {
        जर (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        परत;
    }

    स्थिरांक अक्षर* at = locale_getKeywordsStart(localeID.toStringPiece());
    तार्किक hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    जर (!hasKeywords) {
        जर (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            जर (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<माप_प्रकार>(variantBegin) < आकार Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, आकार Nest::baseName, status);
            } अन्यथा {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                जर (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    परत;
                }
            }
        }
    } अन्यथा {
        Heap* heap = nullptr;
        जर (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            जर (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                परत;
            }
        } अन्यथा {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        जर (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            परत;
        }

        जर (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            जर (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                जर (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    परत;
                }
            }
        }
    }
}

शून्य
Locale::set_Unicode_Keyword_मूल्य(StringPiece keywordName,
                               StringPiece keyword_मूल्य,
                               UErrorCode& status) {
    जर (U_FAILURE(status)) {
        परत;
    }

    std::optional<std::string_view> legacy_किल्ली = ulocimp_to_Legacy_किल्ली_With_Fallback(keywordName);
    जर (!legacy_किल्ली.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }

    std::string_view value;

    जर (!keyword_मूल्य.empty()) {
        std::optional<std::string_view> legacy_मूल्य =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_मूल्य);
        जर (!legacy_मूल्य.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            परत;
        }
        value = *legacy_मूल्य;
    }

    set_Keyword_मूल्य(*legacy_किल्ली, value, status);
}

स्थिरांक अक्षर*
Locale::getCountry() स्थिरांक U_LIFETIME_BOUND {
    परत getField<&Nest::getRegion, &Heap::getRegion>();
}

स्थिरांक अक्षर*
Locale::getLanguage() स्थिरांक U_LIFETIME_BOUND {
    परत getField<&Nest::getLanguage, &Heap::getLanguage>();
}

स्थिरांक अक्षर*
Locale::getScript() स्थिरांक U_LIFETIME_BOUND {
    परत getField<&Nest::getScript, &Heap::getScript>();
}

स्थिरांक अक्षर*
Locale::getVariant() स्थिरांक U_LIFETIME_BOUND {
    परत getField<&Nest::getVariant, &Heap::getVariant>();
}

स्थिरांक अक्षर*
Locale::getName() स्थिरांक U_LIFETIME_BOUND {
    परत getField<&Nest::getBaseName, &Heap::getFullName>();
}

स्थिरांक अक्षर*
Locale::getBaseName() स्थिरांक U_LIFETIME_BOUND {
    परत getField<&Nest::getBaseName, &Heap::getBaseName>();
}

ढाँचा <स्थिरांक अक्षर* (Locale::Nest::*स्थिरांक NEST)() स्थिरांक,
          स्थिरांक अक्षर* (Locale::Heap::*स्थिरांक HEAP)() स्थिरांक>
स्थिरांक अक्षर* Locale::getField() स्थिरांक U_LIFETIME_BOUND {
    परत payload.visit([] { परत ""; },
                         [](स्थिरांक Nest& nest) { परत (nest.*NEST)(); },
                         [](स्थिरांक Heap& heap) { परत (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = पूर्वनिर्धारित;

//eof
U_NAMESPACE_END
