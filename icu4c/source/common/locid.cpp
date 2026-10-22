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

#यदि UPRV_HAS_FEATURE(memory_sanitizer)
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
    e_जरा,


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

    यदि (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    फर्काऊ सत्य;
}

शून्य U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = नूतन Locale[static_cast<पूर्णाङ्क>(eMAX_LOCALES)];
    यदि (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_जरा]          = Locale("");
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

Locale *locale_set_default_internal(अचर वर्ण *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = असत्य;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    यदि (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = सत्य; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    यदि (U_FAILURE(status)) {
        फर्काऊ gDefaultLocale;
    }

    यदि (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        यदि (U_FAILURE(status)) {
            फर्काऊ gDefaultLocale;
        }
        uhash_set_मान_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    यदि (newDefault == nullptr) {
        newDefault = नूतन Locale(Locale::eBOGUS);
        यदि (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), असत्य);
        uhash_put(gDefaultLocalesHashT, const_cast<वर्ण*>(newDefault->getName()), newDefault, &status);
        यदि (U_FAILURE(status)) {
            फर्काऊ gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    फर्काऊ gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC शून्य
locale_set_default(अचर वर्ण *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC अचर वर्ण *
locale_get_default()
{
    U_NAMESPACE_USE
    फर्काऊ Locale::getDefault().getName();
}

नामाकाश {

ढाँचा <स्वतः FIELD, typename T>
शून्य copyToArray(std::string_view sv, T* that) {
    स्वतः& field = that->*FIELD;
    constexpr माप_प्रकार capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    यदि (!sv.empty()) {
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
    static_assert(दुरत्व(Nest, region) <= दुरत्व(Heap, script));
    static_assert(दुरत्व(Nest, variantBegin) <= दुरत्व(Heap, region));
    U_ASSERT(एतत् == reinterpret_cast<Nest*>(&heap));
#यदि UPRV_HAS_FEATURE(memory_sanitizer)
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

    अचर वर्ण* getVariant() अचर U_LIFETIME_BOUND {
        फर्काऊ variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    अचर वर्ण* getFullName() अचर U_LIFETIME_BOUND { फर्काऊ fullName.data(); }
    अचर वर्ण* getBaseName() अचर U_LIFETIME_BOUND {
        यदि (baseName.isEmpty()) {
            यदि (अचर वर्ण* name = fullName.data(); *name != '@') {
                फर्काऊ name;
            }
        }
        फर्काऊ baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(अचर Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        यदि (U_SUCCESS(status)) {
            यदि (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                यदि (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अन्यथा {
                    यदि (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        यदि (baseName.isEmpty()) {
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

अचर वर्ण* Locale::Heap::getVariant() अचर U_LIFETIME_BOUND { फर्काऊ ptr->getVariant(); }
अचर वर्ण* Locale::Heap::getFullName() अचर U_LIFETIME_BOUND { फर्काऊ ptr->getFullName(); }
अचर वर्ण* Locale::Heap::getBaseName() अचर U_LIFETIME_BOUND { फर्काऊ ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = नूतन Alloc(variantBegin);
    यदि (ptr == nullptr) {
        type = eBOGUS;
    } अन्यथा {
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

Locale::Heap& Locale::Heap::चालक=(अचर Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = नूतन Alloc(*other.ptr, status);
    यदि (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } अन्यथा {
        type = eHEAP;
        uprv_memcpy(language, other.language, परिमाण language);
        uprv_memcpy(script, other.script, परिमाण script);
        uprv_memcpy(region, other.region, परिमाण region);
    }
    फर्काऊ *एतत्;
}

Locale::Heap& Locale::Heap::चालक=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, परिमाण language);
    uprv_memcpy(script, other.script, परिमाण script);
    uprv_memcpy(region, other.region, परिमाण region);
    फर्काऊ *एतत्;
}

ढाँचा <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
स्वतः Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) अचर {
    विकल्प (type) {
        पक्ष eBOGUS:
            फर्काऊ bogusFn(args...);
        पक्ष eNEST:
            फर्काऊ nestFn(nest, args...);
        पक्ष eHEAP:
            फर्काऊ heapFn(heap, args...);
        स्वभाव:
            UPRV_UNREACHABLE_EXIT;
    };
}

शून्य Locale::Payload::copy(अचर Payload& other) {
    other.visit([](Payload*) {},
                [](अचर Nest& nest, Payload* dst) { dst->nest = nest; },
                [](अचर Heap& heap, Payload* dst) { dst->heap = heap; },
                एतत्);
}

शून्य Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](अचर Nest& nest, Payload* dst) { dst->nest = nest; },
        [](अचर Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        एतत्);
}

Locale::Payload::~Payload() {
    यदि (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(अचर Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::चालक=(अचर Payload& other) U_LIFETIME_BOUND {
    यदि (एतत् != &other) {
        setToBogus();
        copy(other);
    }
    फर्काऊ *एतत्;
}

Locale::Payload& Locale::Payload::चालक=(Payload&& other) noexcept U_LIFETIME_BOUND {
    यदि (एतत् != &other) {
        setToBogus();
        move(std::move(other));
    }
    फर्काऊ *एतत्;
}

शून्य Locale::Payload::setToBogus() {
    एतत्->~Payload();
    type = eBOGUS;
}

ढाँचा <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    यदि constexpr (std::is_same_v<T, Nest>) {
        एतत्->~Payload();
        ::नूतन (&nest) Nest(std::forward<Args>(args)...);
        फर्काऊ nest;
    }
    यदि constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::नूतन (&heap) Heap(std::forward<Args>(args)...);
        फर्काऊ heap;
    }
}

ढाँचा <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    फर्काऊ type == eNEST ? &nest : nullptr;
}
ढाँचा <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    फर्काऊ type == eHEAP ? &heap : nullptr;
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


Locale::Locale( अचर   वर्ण * newLanguage,
                अचर   वर्ण * newCountry,
                अचर   वर्ण * newVariant,
                अचर   वर्ण * newKeywords)
    : UObject(), payload()
{
    यदि( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
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
        यदि ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            यदि ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                फर्काऊ;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        यदि ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            यदि ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                फर्काऊ;
            }
        }

        // _Variant
        यदि ( newVariant != nullptr )
        {
            // remove leading _'s
            जबसम्म(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            यदि ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                फर्काऊ;
            }
            जबसम्म( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        यदि ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            यदि ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              फर्काऊ;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        यदि ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        यदि ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        यदि ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        यदि ( ksize != 0)
        {
            यदि (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            अन्यथा {
                togo.append('_', status); /* Variant parsing with a script */
                यदि ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        यदि (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            फर्काऊ;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), असत्य);
    }
}

Locale::Locale(अचर Locale&) = स्वभाव;
Locale::Locale(Locale&&) noexcept = स्वभाव;

Locale& Locale::चालक=(अचर Locale&) U_LIFETIME_BOUND = स्वभाव;
Locale& Locale::चालक=(Locale&&) noexcept U_LIFETIME_BOUND = स्वभाव;

Locale *
Locale::clone() अचर {
    फर्काऊ नूतन Locale(*एतत्);
}

तार्किक
Locale::चालक==( अचर   Locale& other) अचर
{
    फर्काऊ uprv_strcmp(other.getName(), getName()) == 0;
}

नामाकाश {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr अचर वर्ण* KNOWN_CANONICALIZED[] = {
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
    यदि (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    फर्काऊ सत्य;
}

शून्य U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    लागि (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (शून्य*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    यदि (U_FAILURE(status)) {
        फर्काऊ;
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

गुप्त:
    शून्य readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<अचर वर्ण*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   शून्य (*checkType)(अचर वर्ण* type),
                   शून्य (*checkReplacement)(अचर UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    शून्य readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<अचर वर्ण*>& types,
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
                         LocalMemory<अचर वर्ण*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    शून्य readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<अचर वर्ण*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    शून्य readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<अचर वर्ण*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    शून्य readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<अचर वर्ण*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
वर्ग AliasData : सार्वजनिक UMemory {
सार्वजनिक:
    स्थिर अचर AliasData* singleton(UErrorCode& status) {
        यदि (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            फर्काऊ nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        फर्काऊ gSingleton;
    }

    अचर CharStringMap& languageMap() अचर U_LIFETIME_BOUND { फर्काऊ language; }
    अचर CharStringMap& scriptMap() अचर U_LIFETIME_BOUND { फर्काऊ script; }
    अचर CharStringMap& territoryMap() अचर U_LIFETIME_BOUND { फर्काऊ territory; }
    अचर CharStringMap& variantMap() अचर U_LIFETIME_BOUND { फर्काऊ variant; }
    अचर CharStringMap& subdivisionMap() अचर U_LIFETIME_BOUND { फर्काऊ subdivision; }

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

    स्थिर अचर AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    मित्र वर्ग AliasDataBuilder;
};


अचर AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    फर्काऊ सत्य;
}

शून्य
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<अचर वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        शून्य (*checkType)(अचर वर्ण* type),
        शून्य (*checkReplacement)(अचर UChar* replacement),
        UErrorCode &status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    length = ures_getSize(alias);
    अचर वर्ण** rawTypes = types.allocateInsteadAndCopy(length);
    यदि (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    यदि (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    लागि (पूर्णाङ्क i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        अचर वर्ण* aliasFrom = ures_get_साँचो(res.getAlias());
        अचर UChar* aliasTo =
            ures_get_String_By_साँचो(res.getAlias(), "replacement", nullptr, &status);
        यदि (U_FAILURE(status)) फर्काऊ;

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
        LocalMemory<अचर वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    फर्काऊ readAlias(
        alias, strings, types, replacementIndexes, length,
#यदि U_DEBUG
        [](अचर वर्ण* type) {
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
        [](अचर वर्ण*) {},
#endif
        [](अचर UChar*) {}, status);
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
        LocalMemory<अचर वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    फर्काऊ readAlias(
        alias, strings, types, replacementIndexes, length,
#यदि U_DEBUG
        [](अचर वर्ण* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](अचर UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#अन्यथा
        [](अचर वर्ण*) {},
        [](अचर UChar*) { },
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
        LocalMemory<अचर वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    फर्काऊ readAlias(
        alias, strings, types, replacementIndexes, length,
#यदि U_DEBUG
        [](अचर वर्ण* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#अन्यथा
        [](अचर वर्ण*) {},
#endif
        [](अचर UChar*) { },
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
        LocalMemory<अचर वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    फर्काऊ readAlias(
        alias, strings, types, replacementIndexes, length,
#यदि U_DEBUG
        [](अचर वर्ण* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](अचर UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#अन्यथा
        [](अचर वर्ण*) {},
        [](अचर UChar*) { },
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
        LocalMemory<अचर वर्ण*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    फर्काऊ readAlias(
        alias, strings, types, replacementIndexes, length,
#यदि U_DEBUG
        [](अचर वर्ण* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#अन्यथा
        [](अचर वर्ण*) {},
#endif
        [](अचर UChar*) { },
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
    यदि (U_FAILURE(status)) { फर्काऊ nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_साँचो(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_साँचो(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_साँचो(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_साँचो(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_साँचो(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_साँचो(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<अचर वर्ण*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<अचर वर्ण*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<अचर वर्ण*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<अचर वर्ण*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<अचर वर्ण*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    लागि (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    लागि (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    लागि (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    लागि (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    लागि (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }

    // copy hashtables
    स्वतः *data = नूतन AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    यदि (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    फर्काऊ data;
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
                       फर्काऊ 0==uprv_strcmp((अचर वर्ण*)e1.pointer,
                                             (अचर वर्ण*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    तार्किक replace(
        अचर Locale& locale, CharString& out, UErrorCode& status);

गुप्त:
    अचर वर्ण* language;
    अचर वर्ण* script;
    अचर वर्ण* region;
    अचर वर्ण* extensions;
    UVector variants;

    अचर AliasData* data;

    अन्तःस्थ तार्किक notEmpty(अचर वर्ण* str) {
        फर्काऊ str && str[0] != NULL_CHAR;
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
    अन्तःस्थ अचर वर्ण* deleteOrReplace(अचर वर्ण* input U_LIFETIME_BOUND,
                                       अचर वर्ण* type,
                                       अचर वर्ण* replacement U_LIFETIME_BOUND) {
        फर्काऊ notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    अन्तःस्थ तार्किक same(अचर वर्ण* a, अचर वर्ण* b) {
        यदि (a == nullptr && b == nullptr) {
            फर्काऊ सत्य;
        }
        यदि ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          फर्काऊ असत्य;
        }
        फर्काऊ uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_साँचो(अचर वर्ण* language, अचर वर्ण* region,
                            अचर वर्ण* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    शून्य parseLanguageReplacement(अचर वर्ण* replacement,
                                  अचर वर्ण*& replaceLanguage,
                                  अचर वर्ण*& replaceScript,
                                  अचर वर्ण*& replaceRegion,
                                  अचर वर्ण*& replaceVariant,
                                  अचर वर्ण*& replaceExtensions,
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
AliasReplacer::generate_साँचो(
        अचर वर्ण* language, अचर वर्ण* region, अचर वर्ण* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    यदि (U_FAILURE(status)) { फर्काऊ out; }
    out.append(language, status);
    यदि (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    यदि (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    फर्काऊ out;
}

शून्य
AliasReplacer::parseLanguageReplacement(
    अचर वर्ण* replacement,
    अचर वर्ण*& replacedLanguage,
    अचर वर्ण*& replacedScript,
    अचर वर्ण*& replacedRegion,
    अचर वर्ण*& replacedVariant,
    अचर वर्ण*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    यदि (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        फर्काऊ;
    }
    // We have multiple field so we have to allocate and parse
    स्वतः str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    वर्ण* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    replacedLanguage = (अचर वर्ण*) data;
    वर्ण* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    अचर वर्ण* start = endOfField;
    endOfField = const_cast<वर्ण*>(uprv_strchr(start, '_'));
    माप_प्रकार len = 0;
    यदि (endOfField == nullptr) {
        len = uprv_strlen(start);
    } अन्यथा {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    यदि (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        यदि (endOfField == nullptr) {
            फर्काऊ;
        }
        start = endOfField++;
        endOfField = const_cast<वर्ण*>(uprv_strchr(start, '_'));
        यदि (endOfField == nullptr) {
            len = uprv_strlen(start);
        } अन्यथा {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    यदि (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        यदि (endOfField == nullptr) {
            फर्काऊ;
        }
        start = endOfField++;
        endOfField = const_cast<वर्ण*>(uprv_strchr(start, '_'));
        यदि (endOfField == nullptr) {
            len = uprv_strlen(start);
        } अन्यथा {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    यदि (len >= 4) {
        // Got a variant
        replacedVariant = start;
        यदि (endOfField == nullptr) {
            फर्काऊ;
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
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    यदि (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        फर्काऊ असत्य;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    अचर वर्ण* खोज_Language = checkLanguage ? language : "und";
    अचर वर्ण* खोज_Region = checkRegion ? region : nullptr;
    अचर वर्ण* खोज_Variant = nullptr;
    लागि (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        यदि (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            खोज_Variant = static_cast<अचर वर्ण*>(variants.elementAt(variant_index));
        }

        यदि (खोज_Variant != nullptr && uprv_strlen(खोज_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            खोज_Variant = nullptr;
        }
        CharString type_साँचो;
        generate_साँचो(खोज_Language, खोज_Region, खोज_Variant, type_साँचो,
                    status);
        यदि (U_FAILURE(status)) {
            फर्काऊ असत्य;
        }
        अचर वर्ण *replacement = data->languageMap().get(type_साँचो.data());
        यदि (replacement == nullptr) {
            // Found no replacement data.
            जारी;
        }

        अचर वर्ण* replacedLanguage = nullptr;
        अचर वर्ण* replacedScript = nullptr;
        अचर वर्ण* replacedRegion = nullptr;
        अचर वर्ण* replacedVariant = nullptr;
        अचर वर्ण* replacedExtensions = nullptr;
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
        replacedRegion = deleteOrReplace(region, खोज_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            खोज_Variant, खोज_Variant, replacedVariant);

        यदि (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(खोज_Variant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            जारी;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        यदि (खोज_Variant != nullptr) {
            यदि (notEmpty(replacedVariant)) {
                variants.setElementAt((शून्य*)replacedVariant, variant_index);
            } अन्यथा {
                variants.removeElementAt(variant_index);
            }
        }
        यदि (replacedExtensions != nullptr) {
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
        फर्काऊ सत्य;
    }
    // Nothing changed by language alias data.
    फर्काऊ असत्य;
}

तार्किक
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    यदि (region == nullptr) {
        // No region to search.
        फर्काऊ असत्य;
    }
    अचर वर्ण *replacement = data->territoryMap().get(region);
    यदि (replacement == nullptr) {
        // Found no replacement data for this region.
        फर्काऊ असत्य;
    }
    अचर वर्ण* replacedRegion = replacement;
    अचर वर्ण* firstSpace = uprv_strchr(replacement, ' ');
    यदि (firstSpace != nullptr) {
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
        अचर वर्ण* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        यदि (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            माप_प्रकार len = uprv_strlen(likelyRegion);
            अचर वर्ण* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            यदि (foundInReplacement != nullptr) {
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
        यदि (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        यदि (U_FAILURE(status)) { फर्काऊ असत्य; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        यदि (U_FAILURE(status)) { फर्काऊ असत्य; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    फर्काऊ सत्य;
}

तार्किक
AliasReplacer::replaceScript(UErrorCode& status)
{
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    यदि (script == nullptr) {
        // No script to search.
        फर्काऊ असत्य;
    }
    अचर वर्ण *replacement = data->scriptMap().get(script);
    यदि (replacement == nullptr) {
        // Found no replacement data for this script.
        फर्काऊ असत्य;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    फर्काऊ सत्य;
}

तार्किक
AliasReplacer::replaceVariant(UErrorCode& status)
{
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    // Since we may have more than one variant, we need to loop through them.
    लागि (int32_t i = 0; i < variants.size(); i++) {
        अचर वर्ण* variant = static_cast<अचर वर्ण*>(variants.elementAt(i));
        अचर वर्ण *replacement = data->variantMap().get(variant);
        यदि (replacement == nullptr) {
            // Found no replacement data for this variant.
            जारी;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        यदि (!same(variant, replacement)) {
            variants.setElementAt((शून्य*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            यदि (uprv_strcmp(variant, "heploc") == 0) {
                लागि (int32_t j = 0; j < variants.size(); j++) {
                     यदि (uprv_strcmp((अचर वर्ण*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            फर्काऊ सत्य;
        }
    }
    फर्काऊ असत्य;
}

तार्किक
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    अचर वर्ण *replacement = data->subdivisionMap().get(subdivision.data());
    यदि (replacement != nullptr) {
        अचर वर्ण* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        माप_प्रकार len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        यदि (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            यदि (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        फर्काऊ सत्य;
    }
    फर्काऊ असत्य;
}

तार्किक
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    int32_t len = transformedExtensions.length();
    अचर वर्ण* str = transformedExtensions.data();
    अचर वर्ण* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    यदि (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        यदि (U_FAILURE(status)) {
            फर्काऊ असत्य;
        }
        T_CString_toLowerCase(output.data());
    }
    यदि (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        यदि (U_FAILURE(status)) {
            फर्काऊ असत्य;
        }
        गर {
            अचर वर्ण* tvalue = uprv_strchr(tkey, '-');
            यदि (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                फर्काऊ असत्य;
            }
            अचर वर्ण* nextTKey = ultag_getTKeyStart(tvalue);
            यदि (nextTKey != nullptr) {
                *const_cast<वर्ण*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.घुसाउ_Element_At((शून्य*)tkey, tfields.size(), status);
            यदि (U_FAILURE(status)) {
                फर्काऊ असत्य;
            }
            tkey = nextTKey;
        } जबसम्म (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            फर्काऊ uprv_strcmp((अचर वर्ण*)e1.pointer, (अचर वर्ण*)e2.pointer);
        }, status);
        लागि (int32_t i = 0; i < tfields.size(); i++) {
             यदि (output.length() > 0) {
                 output.append('-', status);
             }
             अचर वर्ण* tfield = static_cast<अचर वर्ण*>(tfields.elementAt(i));
             अचर वर्ण* tvalue = uprv_strchr(tfield, '-');
             यदि (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 फर्काऊ असत्य;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<वर्ण*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    फर्काऊ सत्य;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    यदि (U_FAILURE(status)) { फर्काऊ out; }
    out.append(language, status);
    यदि (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    यदि (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    यदि (variants.size() > 0) {
        यदि (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            फर्काऊ uprv_strcmp((अचर वर्ण*)e1.pointer, (अचर वर्ण*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        लागि (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<अचर वर्ण*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    यदि (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    फर्काऊ out;
}

तार्किक
AliasReplacer::replace(अचर Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    U_ASSERT(data != nullptr);
    out.clear();
    language = locale.getLanguage();
    यदि (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    यदि (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    यदि (!notEmpty(region)) {
        region = nullptr;
    }
    अचर वर्ण* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    यदि (!variantsBuff.isEmpty()) {
        यदि (U_FAILURE(status)) { फर्काऊ असत्य; }
        वर्ण* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        वर्ण* end;
        जबसम्म ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            यदि (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        यदि (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    यदि (U_FAILURE(status)) { फर्काऊ असत्य; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        फर्काऊ uprv_strcmp((अचर वर्ण*)e1.pointer, (अचर वर्ण*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    अचर int32_t maxChanged = 4 + variants.size();
    (शून्य)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](शून्य *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    जबसम्म (U_SUCCESS(status)) {
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
        यदि (    replaceLanguage(सत्य, सत्य,  सत्य,  stringsToBeFreed, status) ||
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
            जारी;
        }
        // Nothing changed. Break out.
        रोक;
    }  // while(1)

    यदि (U_FAILURE(status)) { फर्काऊ असत्य; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    अचर वर्ण* extensionsStr = locale_getKeywordsStart(locale.getName());
    यदि (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        फर्काऊ असत्य;
    }
    outputToString(out, status);
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    यदि (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        यदि (U_SUCCESS(status) && !iter.isNull()) {
            अचर वर्ण* साँचो;
            जबसम्म ((साँचो = iter->next(nullptr, status)) != nullptr) {
                यदि (uprv_strcmp("sd", साँचो) == 0 || uprv_strcmp("rg", साँचो) == 0 ||
                        uprv_strcmp("t", साँचो) == 0) {
                    स्वतः value = locale.get_Keyword_मान<CharString>(साँचो, status);
                    यदि (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        जारी;
                    }
                    CharString replacement;
                    यदि (uprv_strlen(साँचो) == 2) {
                        यदि (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.set_Keyword_मान(साँचो, replacement.data(), status);
                        }
                    } अन्यथा {
                        U_ASSERT(uprv_strcmp(साँचो, "t") == 0);
                        यदि (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.set_Keyword_मान(साँचो, replacement.data(), status);
                        }
                    }
                    यदि (U_FAILURE(status)) {
                        फर्काऊ असत्य;
                    }
                }
            }
        }
        यदि (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    // If the tag is not changed, return.
    यदि (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        फर्काऊ असत्य;
    }
    फर्काऊ सत्य;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
तार्किक
canonicalizeLocale(अचर Locale& locale, CharString& out, UErrorCode& status)
{
    यदि (U_FAILURE(status)) { फर्काऊ असत्य; }
    AliasReplacer replacer(status);
    फर्काऊ replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
तार्किक
isKnownCanonicalizedLocale(अचर वर्ण* locale, UErrorCode& status)
{
    यदि (U_FAILURE(status)) { फर्काऊ असत्य; }

    यदि (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        फर्काऊ सत्य;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    फर्काऊ uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT अचर वर्ण* अचर*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    फर्काऊ KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT तार्किक
ulocimp_isCanonicalizedLocaleForTest(अचर वर्ण* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    फर्काऊ !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(अचर वर्ण* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    फर्काऊ localeID == nullptr ? *एतत् = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    गर {
        वर्ण *separator;
        वर्ण *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        अचर स्वतः parse = [canonicalize](std::string_view localeID,
                                          वर्ण* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            फर्काऊ ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    यदि (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } अन्यथा {
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
        यदि (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(परिमाण Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            यदि (!fullNameBuffer.reserve(length + 1)) {
                रोक; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        यदि(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            रोक;
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
        जबसम्म ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        वर्ण* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        यदि (separator!=nullptr || sep2!=nullptr) {
            यदि (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } अन्यथा {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        तार्किक hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        यदि (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            रोक; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        यदि (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        यदि (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        यदि (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } अन्यथा यदि (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        यदि (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } अन्यथा यदि (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        यदि (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } अन्यथा {
            यदि (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                यदि (fullNameBuffer.isEmpty()) {
                    रोक; // error: out of memory
                }
                यदि (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                यदि (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                यदि (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            यदि (isBogus()) {
                रोक; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            यदि (hasKeywords) {
                यदि (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    यदि (heap.ptr->baseName.isEmpty()) {
                        रोक; // error: out of memory
                    }
                }
            }
        }

        यदि (canonicalize) {
            यदि (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                यदि (canonicalizeLocale(*एतत्, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), असत्य);
                }
                यदि (U_FAILURE(err)) {
                    रोक;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        फर्काऊ *एतत्;
    } जबसम्म(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    फर्काऊ *एतत्;
}

int32_t
Locale::hashCode() अचर
{
    फर्काऊ ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

शून्य
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

अचर Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        यदि (gDefaultLocale != nullptr) {
            फर्काऊ *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    फर्काऊ *locale_set_default_internal(nullptr, status);
}



शून्य U_EXPORT2
Locale::setDefault( अचर   Locale&     newLocale,
                            UErrorCode&  status)
{
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    अचर वर्ण *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

शून्य
Locale::addLikelySubtags(UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    यदि (U_FAILURE(status)) {
        यदि (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        फर्काऊ;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/असत्य);
    यदि (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

शून्य
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(असत्य, status);
}
शून्य
Locale::minimizeSubtags(तार्किक favorScript, UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    यदि (U_FAILURE(status)) {
        यदि (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        फर्काऊ;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/असत्य);
    यदि (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

शून्य
Locale::canonicalize(UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    यदि (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }
    CharString uncanonicalized(getName(), status);
    यदि (U_FAILURE(status)) {
        यदि (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        फर्काऊ;
    }
    init(uncanonicalized.data(), /*canonicalize=*/सत्य);
    यदि (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    यदि (U_FAILURE(status)) {
        फर्काऊ result;
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

    यदि (U_FAILURE(status)) {
        फर्काऊ result;
    }

    यदि (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ result;
    }

    result.init(localeID.data(), /*canonicalize=*/असत्य);
    यदि (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    फर्काऊ result;
}

शून्य
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) अचर
{
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }

    यदि (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/असत्य, status);
}

Locale U_EXPORT2
Locale::createFromName (अचर वर्ण *name)
{
    यदि (name) {
        Locale l("");
        l.init(name, असत्य);
        फर्काऊ l;
    }
    अन्यथा {
        फर्काऊ getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, असत्य);
    फर्काऊ loc;
}

Locale U_EXPORT2
Locale::createCanonical(अचर वर्ण* name) {
    Locale loc("");
    loc.init(name, सत्य);
    फर्काऊ loc;
}

अचर वर्ण *
Locale::getISO3Language() अचर
{
    फर्काऊ uloc_getISO3Language(getName());
}


अचर वर्ण *
Locale::getISO3Country() अचर
{
    फर्काऊ uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() अचर
{
    फर्काऊ uloc_getLCID(getName());
}

अचर वर्ण* अचर* U_EXPORT2 Locale::getISOCountries()
{
    फर्काऊ uloc_getISOCountries();
}

अचर वर्ण* अचर* U_EXPORT2 Locale::getISOLanguages()
{
    फर्काऊ uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
शून्य Locale::setFromPOSIXID(अचर वर्ण *posixID)
{
    init(posixID, सत्य);
}

अचर Locale & U_EXPORT2
Locale::get_जरा()
{
    फर्काऊ getLocale(e_जरा);
}

अचर Locale & U_EXPORT2
Locale::getEnglish()
{
    फर्काऊ getLocale(eENGLISH);
}

अचर Locale & U_EXPORT2
Locale::getFrench()
{
    फर्काऊ getLocale(eFRENCH);
}

अचर Locale & U_EXPORT2
Locale::getGerman()
{
    फर्काऊ getLocale(eGERMAN);
}

अचर Locale & U_EXPORT2
Locale::getItalian()
{
    फर्काऊ getLocale(eITALIAN);
}

अचर Locale & U_EXPORT2
Locale::getJapanese()
{
    फर्काऊ getLocale(eJAPANESE);
}

अचर Locale & U_EXPORT2
Locale::getKorean()
{
    फर्काऊ getLocale(eKOREAN);
}

अचर Locale & U_EXPORT2
Locale::getChinese()
{
    फर्काऊ getLocale(eCHINESE);
}

अचर Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    फर्काऊ getLocale(eCHINA);
}

अचर Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    फर्काऊ getLocale(eTAIWAN);
}


अचर Locale & U_EXPORT2
Locale::getFrance()
{
    फर्काऊ getLocale(eFRANCE);
}

अचर Locale & U_EXPORT2
Locale::getGermany()
{
    फर्काऊ getLocale(eGERMANY);
}

अचर Locale & U_EXPORT2
Locale::getItaly()
{
    फर्काऊ getLocale(eITALY);
}

अचर Locale & U_EXPORT2
Locale::getJapan()
{
    फर्काऊ getLocale(eJAPAN);
}

अचर Locale & U_EXPORT2
Locale::getKorea()
{
    फर्काऊ getLocale(eKOREA);
}

अचर Locale & U_EXPORT2
Locale::getChina()
{
    फर्काऊ getLocale(eCHINA);
}

अचर Locale & U_EXPORT2
Locale::getPRC()
{
    फर्काऊ getLocale(eCHINA);
}

अचर Locale & U_EXPORT2
Locale::getTaiwan()
{
    फर्काऊ getLocale(eTAIWAN);
}

अचर Locale & U_EXPORT2
Locale::getUK()
{
    फर्काऊ getLocale(eUK);
}

अचर Locale & U_EXPORT2
Locale::getUS()
{
    फर्काऊ getLocale(eUS);
}

अचर Locale & U_EXPORT2
Locale::getCanada()
{
    फर्काऊ getLocale(eCANADA);
}

अचर Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    फर्काऊ getLocale(eCANADA_FRENCH);
}

अचर Locale &
Locale::getLocale(पूर्णाङ्क locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    यदि (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    फर्काऊ localeCache[locid]; /*operating on nullptr*/
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
    फर्काऊ gLocaleCache;
}

वर्ग KeywordEnumeration : सार्वजनिक StringEnumeration {
रक्षित:
    FixedString keywords;
गुप्त:
    int32_t length;
    अचर वर्ण *current;
    स्थिर अचर वर्ण fgClassID;

सार्वजनिक:
    स्थिर UClassID U_EXPORT2 getStaticClassID() { फर्काऊ (UClassID)&fgClassID; }
    आभास UClassID getDynamicClassID() अचर override { फर्काऊ getStaticClassID(); }
सार्वजनिक:
    KeywordEnumeration(अचर वर्ण *साँचोहरू, int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        यदि(U_SUCCESS(status) && keywordLen != 0) {
            यदि(साँचोहरू == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } अन्यथा {
                keywords = {साँचोहरू, static_cast<std::string_view::size_type>(length)};
                यदि (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अन्यथा {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    आभास ~KeywordEnumeration();

    आभास StringEnumeration * clone() अचर override
    {
        UErrorCode status = U_ZERO_ERROR;
        फर्काऊ नूतन KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    आभास int32_t count(UErrorCode& status) अचर override {
        यदि (U_FAILURE(status)) { फर्काऊ 0; }
        अचर वर्ण *kw = keywords.data();
        int32_t result = 0;
        जबसम्म(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        फर्काऊ result;
    }

    आभास अचर वर्ण* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        अचर वर्ण* result;
        int32_t len;
        यदि(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            यदि(resultLength != nullptr) {
                *resultLength = len;
            }
        } अन्यथा {
            यदि(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        फर्काऊ result;
    }

    आभास अचर UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        यदि (U_FAILURE(status)) { फर्काऊ nullptr; }
        int32_t resultLength = 0;
        अचर वर्ण *s = next(&resultLength, status);
        फर्काऊ setChars(s, resultLength, status);
    }

    आभास शून्य reset(UErrorCode& status) override {
        यदि (U_FAILURE(status)) { फर्काऊ; }
        current = keywords.data();
    }
};

अचर वर्ण KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = स्वभाव;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
वर्ग UnicodeKeywordEnumeration : सार्वजनिक KeywordEnumeration {
सार्वजनिक:
    using KeywordEnumeration::KeywordEnumeration;
    आभास ~UnicodeKeywordEnumeration();

    आभास अचर वर्ण* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        अचर वर्ण* legacy_साँचो = KeywordEnumeration::next(nullptr, status);
        जबसम्म (U_SUCCESS(status) && legacy_साँचो != nullptr) {
            अचर वर्ण* साँचो = uloc_to_Unicode_Locale_साँचो(legacy_साँचो);
            यदि (साँचो != nullptr) {
                यदि (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(साँचो));
                }
                फर्काऊ साँचो;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_साँचो = KeywordEnumeration::next(nullptr, status);
        }
        यदि (resultLength != nullptr) *resultLength = 0;
        फर्काऊ nullptr;
    }
    आभास int32_t count(UErrorCode& status) अचर override {
        यदि (U_FAILURE(status)) { फर्काऊ 0; }
        अचर वर्ण *kw = keywords.data();
        int32_t result = 0;
        जबसम्म(*kw) {
            यदि (uloc_to_Unicode_Locale_साँचो(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        फर्काऊ result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = स्वभाव;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) अचर
{
    StringEnumeration *result = nullptr;

    यदि (U_FAILURE(status)) {
        फर्काऊ result;
    }

    अचर वर्ण* variantStart = uprv_strchr(getName(), '@');
    अचर वर्ण* assignment = uprv_strchr(getName(), '=');
    यदि(variantStart) {
        यदि(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', असत्य, status);
            यदि (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = नूतन KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                यदि (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } अन्यथा {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    फर्काऊ result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) अचर
{
    StringEnumeration *result = nullptr;

    यदि (U_FAILURE(status)) {
        फर्काऊ result;
    }

    अचर वर्ण* variantStart = uprv_strchr(getName(), '@');
    अचर वर्ण* assignment = uprv_strchr(getName(), '=');
    यदि(variantStart) {
        यदि(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', असत्य, status);
            यदि (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = नूतन UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                यदि (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } अन्यथा {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    फर्काऊ result;
}

int32_t
Locale::get_Keyword_मान(अचर वर्ण* keywordName, वर्ण *buffer, int32_t bufLen, UErrorCode &status) अचर
{
    फर्काऊ uloc_get_Keyword_मान(getName(), keywordName, buffer, bufLen, &status);
}

शून्य
Locale::get_Keyword_मान(StringPiece keywordName, ByteSink& sink, UErrorCode& status) अचर {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }

    यदि (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }

    ulocimp_get_Keyword_मान(getName(), keywordName, sink, status);
}

शून्य
Locale::get_Unicode_Keyword_मान(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) अचर {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }

    std::optional<std::string_view> legacy_साँचो = ulocimp_to_Legacy_साँचो_With_Fallback(keywordName);
    यदि (!legacy_साँचो.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }

    स्वतः legacy_मान = get_Keyword_मान<CharString>(*legacy_साँचो, status);

    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }

    std::optional<std::string_view> unicode_मान =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_मान.toStringPiece());
    यदि (!unicode_मान.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }

    sink.Append(unicode_मान->data(), static_cast<int32_t>(unicode_मान->size()));
}

शून्य
Locale::set_Keyword_मान(StringPiece keywordName,
                        StringPiece keyword_मान,
                        UErrorCode& status) {
    यदि (U_FAILURE(status)) { फर्काऊ; }
    यदि (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }
    यदि (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_set_Keyword_मान(keywordName, keyword_मान, localeID, status);
    यदि (U_FAILURE(status)) {
        यदि (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        फर्काऊ;
    }

    अचर वर्ण* at = locale_getKeywordsStart(localeID.toStringPiece());
    तार्किक hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    यदि (!hasKeywords) {
        यदि (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            यदि (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<माप_प्रकार>(variantBegin) < परिमाण Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, परिमाण Nest::baseName, status);
            } अन्यथा {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                यदि (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    फर्काऊ;
                }
            }
        }
    } अन्यथा {
        Heap* heap = nullptr;
        यदि (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            यदि (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ;
            }
        } अन्यथा {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        यदि (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            फर्काऊ;
        }

        यदि (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            यदि (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                यदि (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    फर्काऊ;
                }
            }
        }
    }
}

शून्य
Locale::set_Unicode_Keyword_मान(StringPiece keywordName,
                               StringPiece keyword_मान,
                               UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }

    std::optional<std::string_view> legacy_साँचो = ulocimp_to_Legacy_साँचो_With_Fallback(keywordName);
    यदि (!legacy_साँचो.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }

    std::string_view value;

    यदि (!keyword_मान.empty()) {
        std::optional<std::string_view> legacy_मान =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_मान);
        यदि (!legacy_मान.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ;
        }
        value = *legacy_मान;
    }

    set_Keyword_मान(*legacy_साँचो, value, status);
}

अचर वर्ण*
Locale::getCountry() अचर U_LIFETIME_BOUND {
    फर्काऊ getField<&Nest::getRegion, &Heap::getRegion>();
}

अचर वर्ण*
Locale::getLanguage() अचर U_LIFETIME_BOUND {
    फर्काऊ getField<&Nest::getLanguage, &Heap::getLanguage>();
}

अचर वर्ण*
Locale::getScript() अचर U_LIFETIME_BOUND {
    फर्काऊ getField<&Nest::getScript, &Heap::getScript>();
}

अचर वर्ण*
Locale::getVariant() अचर U_LIFETIME_BOUND {
    फर्काऊ getField<&Nest::getVariant, &Heap::getVariant>();
}

अचर वर्ण*
Locale::getName() अचर U_LIFETIME_BOUND {
    फर्काऊ getField<&Nest::getBaseName, &Heap::getFullName>();
}

अचर वर्ण*
Locale::getBaseName() अचर U_LIFETIME_BOUND {
    फर्काऊ getField<&Nest::getBaseName, &Heap::getBaseName>();
}

ढाँचा <अचर वर्ण* (Locale::Nest::*अचर NEST)() अचर,
          अचर वर्ण* (Locale::Heap::*अचर HEAP)() अचर>
अचर वर्ण* Locale::getField() अचर U_LIFETIME_BOUND {
    फर्काऊ payload.visit([] { फर्काऊ ""; },
                         [](अचर Nest& nest) { फर्काऊ (nest.*NEST)(); },
                         [](अचर Heap& heap) { फर्काऊ (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = स्वभाव;

//eof
U_NAMESPACE_END
