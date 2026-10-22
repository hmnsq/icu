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

#समावेश <cstddef>
#समावेश <optional>
#समावेश <string_view>
#समावेश <type_traits>
#समावेश <utility>

#समावेश "unicode/bytestream.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/localebuilder.h"
#समावेश "unicode/localpointer.h"
#समावेश "unicode/strenum.h"
#समावेश "unicode/stringpiece.h"
#समावेश "unicode/uloc.h"
#समावेश "unicode/ures.h"

#समावेश "bytesinkutil.h"
#समावेश "charstr.h"
#समावेश "charstrmap.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "fixedstring.h"
#समावेश "mutex.h"
#समावेश "putilimp.h"
#समावेश "uassert.h"
#समावेश "ucln_cmn.h"
#समावेश "uhash.h"
#समावेश "ulocimp.h"
#समावेश "umutex.h"
#समावेश "uniquecharstr.h"
#समावेश "ustr_imp.h"
#समावेश "uvector.h"

#अगर UPRV_HAS_FEATURE(memory_sanitizer)
#समावेश <sanitizer/msan_interface.h>
#पूर्ण_अगर

U_NAMESPACE_BEGIN

अटल Locale   *gLocaleCache = nullptr;
अटल UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
अटल UMutex gDefaultLocaleMutex;
अटल UHashtable *gDefaultLocalesHashT = nullptr;
अटल Locale *gDefaultLocale = nullptr;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#घोषणा ULOC_STRING_LIMIT 357913941

U_NAMESPACE_END

प्रकार क्रमागत ELocalePos {
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
    e_मूल,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

namespace {

//
// Deleter function for Locales owned by the default Locale hash table/
//
व्योम U_CALLCONV
deleteLocale(व्योम *obj) {
    मिटाओ static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    मिटाओ [] gLocaleCache;
    gLocaleCache = nullptr;
    gLocaleCacheInitOnce.reset();

    अगर (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    वापस true;
}

व्योम U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = नया Locale[static_cast<पूर्णांक>(eMAX_LOCALES)];
    अगर (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_मूल]          = Locale("");
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

Locale *locale_set_default_internal(स्थिर अक्षर *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = false;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    अगर (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = true; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    अगर (U_FAILURE(status)) {
        वापस gDefaultLocale;
    }

    अगर (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        अगर (U_FAILURE(status)) {
            वापस gDefaultLocale;
        }
        uhash_set_मूल्य_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.तथ्य()));
    अगर (newDefault == nullptr) {
        newDefault = नया Locale(Locale::eBOGUS);
        अगर (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            वापस gDefaultLocale;
        }
        newDefault->init(localeNameBuf.तथ्य(), false);
        uhash_put(gDefaultLocalesHashT, const_cast<अक्षर*>(newDefault->getName()), newDefault, &status);
        अगर (U_FAILURE(status)) {
            वापस gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    वापस gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC व्योम
locale_set_default(स्थिर अक्षर *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC स्थिर अक्षर *
locale_get_default()
{
    U_NAMESPACE_USE
    वापस Locale::getDefault().getName();
}

namespace {

ढाँचा <स्वतः FIELD, typename T>
व्योम copyToArray(std::string_view sv, T* that) {
    स्वतः& field = that->*FIELD;
    constexpr माप_प्रकार capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    अगर (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.तथ्य(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

व्योम Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, यह);
    copyToArray<&Nest::script>(script, यह);
    copyToArray<&Nest::region>(region, यह);
    यह->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(दुरत्व(Nest, region) <= दुरत्व(Heap, script));
    static_assert(दुरत्व(Nest, variantBegin) <= दुरत्व(Heap, region));
    U_ASSERT(यह == reinterpret_cast<Nest*>(&heap));
#अगर UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, माप(Heap));
#पूर्ण_अगर
    copyToArray<&Nest::script>(heap.script, यह);
    copyToArray<&Nest::region>(heap.region, यह);
    यह->variantBegin = variantBegin;
    *यह->baseName = '\0';
}

काष्ठा Locale::Heap::Alloc : खुला UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    स्थिर अक्षर* getVariant() स्थिर U_LIFETIME_BOUND {
        वापस variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    स्थिर अक्षर* getFullName() स्थिर U_LIFETIME_BOUND { वापस fullName.तथ्य(); }
    स्थिर अक्षर* getBaseName() स्थिर U_LIFETIME_BOUND {
        अगर (baseName.isEmpty()) {
            अगर (स्थिर अक्षर* name = fullName.तथ्य(); *name != '@') {
                वापस name;
            }
        }
        वापस baseName.तथ्य();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(स्थिर Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        अगर (U_SUCCESS(status)) {
            अगर (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                अगर (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अन्यथा {
                    अगर (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        अगर (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = मिटाओ;

    ~Alloc() = शेष;
};

स्थिर अक्षर* Locale::Heap::getVariant() स्थिर U_LIFETIME_BOUND { वापस ptr->getVariant(); }
स्थिर अक्षर* Locale::Heap::getFullName() स्थिर U_LIFETIME_BOUND { वापस ptr->getFullName(); }
स्थिर अक्षर* Locale::Heap::getBaseName() स्थिर U_LIFETIME_BOUND { वापस ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = नया Alloc(variantBegin);
    अगर (ptr == nullptr) {
        type = eBOGUS;
    } अन्यथा {
        type = eHEAP;
        copyToArray<&Heap::language>(language, यह);
        copyToArray<&Heap::script>(script, यह);
        copyToArray<&Heap::region>(region, यह);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    मिटाओ ptr;
}

Locale::Heap& Locale::Heap::चालक=(स्थिर Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = नया Alloc(*other.ptr, status);
    अगर (ptr == nullptr || U_FAILURE(status)) {
        मिटाओ ptr;
    } अन्यथा {
        type = eHEAP;
        uprv_memcpy(language, other.language, माप language);
        uprv_memcpy(script, other.script, माप script);
        uprv_memcpy(region, other.region, माप region);
    }
    वापस *यह;
}

Locale::Heap& Locale::Heap::चालक=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, माप language);
    uprv_memcpy(script, other.script, माप script);
    uprv_memcpy(region, other.region, माप region);
    वापस *यह;
}

ढाँचा <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
स्वतः Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) स्थिर {
    चयन (type) {
        हाल eBOGUS:
            वापस bogusFn(args...);
        हाल eNEST:
            वापस nestFn(nest, args...);
        हाल eHEAP:
            वापस heapFn(heap, args...);
        शेष:
            UPRV_UNREACHABLE_EXIT;
    };
}

व्योम Locale::Payload::copy(स्थिर Payload& other) {
    other.visit([](Payload*) {},
                [](स्थिर Nest& nest, Payload* dst) { dst->nest = nest; },
                [](स्थिर Heap& heap, Payload* dst) { dst->heap = heap; },
                यह);
}

व्योम Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](स्थिर Nest& nest, Payload* dst) { dst->nest = nest; },
        [](स्थिर Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        यह);
}

Locale::Payload::~Payload() {
    अगर (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(स्थिर Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::चालक=(स्थिर Payload& other) U_LIFETIME_BOUND {
    अगर (यह != &other) {
        setToBogus();
        copy(other);
    }
    वापस *यह;
}

Locale::Payload& Locale::Payload::चालक=(Payload&& other) noexcept U_LIFETIME_BOUND {
    अगर (यह != &other) {
        setToBogus();
        move(std::move(other));
    }
    वापस *यह;
}

व्योम Locale::Payload::setToBogus() {
    यह->~Payload();
    type = eBOGUS;
}

ढाँचा <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    अगर constexpr (std::is_same_v<T, Nest>) {
        यह->~Payload();
        ::नया (&nest) Nest(std::forward<Args>(args)...);
        वापस nest;
    }
    अगर constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::नया (&heap) Heap(std::forward<Args>(args)...);
        वापस heap;
    }
}

ढाँचा <> Locale::Nest* Locale::Payload::लो() U_LIFETIME_BOUND {
    वापस type == eNEST ? &nest : nullptr;
}
ढाँचा <> Locale::Heap* Locale::Payload::लो() U_LIFETIME_BOUND {
    वापस type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#घोषणा SEP_CHAR '_'
#घोषणा NULL_CHAR '\0'

Locale::~Locale() = शेष;

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


Locale::Locale( स्थिर   अक्षर * newLanguage,
                स्थिर   अक्षर * newCountry,
                स्थिर   अक्षर * newVariant,
                स्थिर   अक्षर * newKeywords)
    : UObject(), payload()
{
    अगर( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, false); /* shortcut */
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
        अगर ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            अगर ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                वापस;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        अगर ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            अगर ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                वापस;
            }
        }

        // _Variant
        अगर ( newVariant != nullptr )
        {
            // remove leading _'s
            जबतक(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            अगर ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                वापस;
            }
            जबतक( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        अगर ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            अगर ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              वापस;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        अगर ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        अगर ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        अगर ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        अगर ( ksize != 0)
        {
            अगर (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            अन्यथा {
                togo.append('_', status); /* Variant parsing with a script */
                अगर ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        अगर (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            वापस;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.तथ्य(), false);
    }
}

Locale::Locale(स्थिर Locale&) = शेष;
Locale::Locale(Locale&&) noexcept = शेष;

Locale& Locale::चालक=(स्थिर Locale&) U_LIFETIME_BOUND = शेष;
Locale& Locale::चालक=(Locale&&) noexcept U_LIFETIME_BOUND = शेष;

Locale *
Locale::clone() स्थिर {
    वापस नया Locale(*यह);
}

bool
Locale::चालक==( स्थिर   Locale& other) स्थिर
{
    वापस uprv_strcmp(other.getName(), getName()) == 0;
}

namespace {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr स्थिर अक्षर* KNOWN_CANONICALIZED[] = {
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
    अगर (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    वापस true;
}

व्योम U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    क्रम (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (व्योम*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    अगर (U_FAILURE(status)) {
        वापस;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

श्रेणी Alias_तथ्य;

/**
 * A Builder class to build the alias data.
 */
श्रेणी Alias_तथ्य_Builder {
खुला:
    Alias_तथ्य_Builder() {
    }

    // Build the AliasData from resource.
    Alias_तथ्य* build(UErrorCode &status);

निजी:
    व्योम readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<स्थिर अक्षर*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   व्योम (*checkType)(स्थिर अक्षर* type),
                   व्योम (*checkReplacement)(स्थिर UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    व्योम readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<स्थिर अक्षर*>& types,
                           LocalMemory<int32_t>& replacementIndexes,
                           int32_t &length,
                           UErrorCode &status);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    व्योम readScriptAlias(UResourceBundle* alias,
                         UniqueCharStrings* strings,
                         LocalMemory<स्थिर अक्षर*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    व्योम readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<स्थिर अक्षर*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    व्योम readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<स्थिर अक्षर*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    व्योम readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<स्थिर अक्षर*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
श्रेणी Alias_तथ्य : खुला UMemory {
खुला:
    अटल स्थिर Alias_तथ्य* singleton(UErrorCode& status) {
        अगर (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            वापस nullptr;
        }
        umtx_initOnce(Alias_तथ्य::gInitOnce, &Alias_तथ्य::load_तथ्य, status);
        वापस gSingleton;
    }

    स्थिर CharStringMap& languageMap() स्थिर U_LIFETIME_BOUND { वापस language; }
    स्थिर CharStringMap& scriptMap() स्थिर U_LIFETIME_BOUND { वापस script; }
    स्थिर CharStringMap& territoryMap() स्थिर U_LIFETIME_BOUND { वापस territory; }
    स्थिर CharStringMap& variantMap() स्थिर U_LIFETIME_BOUND { वापस variant; }
    स्थिर CharStringMap& subdivisionMap() स्थिर U_LIFETIME_BOUND { वापस subdivision; }

    अटल व्योम U_CALLCONV load_तथ्य(UErrorCode &status);
    अटल UBool U_CALLCONV cleanup();

    अटल UInitOnce gInitOnce;

निजी:
    Alias_तथ्य(CharStringMap languageMap,
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

    ~Alias_तथ्य() {
        मिटाओ strings;
    }

    अटल स्थिर Alias_तथ्य* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    मित्र श्रेणी Alias_तथ्य_Builder;
};


स्थिर Alias_तथ्य* Alias_तथ्य::gSingleton = nullptr;
UInitOnce Alias_तथ्य::gInitOnce {};

UBool U_CALLCONV
Alias_तथ्य::cleanup()
{
    gInitOnce.reset();
    मिटाओ gSingleton;
    वापस true;
}

व्योम
Alias_तथ्य_Builder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<स्थिर अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        व्योम (*checkType)(स्थिर अक्षर* type),
        व्योम (*checkReplacement)(स्थिर UChar* replacement),
        UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    length = ures_get_आकार(alias);
    स्थिर अक्षर** rawTypes = types.allocateInsteadAndCopy(length);
    अगर (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    अगर (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    क्रम (पूर्णांक i = 0; U_SUCCESS(status) && ures_has_अग्र(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_get_अग्र_Resource(alias, nullptr, &status));
        स्थिर अक्षर* aliasFrom = ures_get_कुंजी(res.getAlias());
        स्थिर UChar* aliasTo =
            ures_get_String_By_कुंजी(res.getAlias(), "replacement", nullptr, &status);
        अगर (U_FAILURE(status)) वापस;

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
व्योम
Alias_तथ्य_Builder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<स्थिर अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    वापस readAlias(
        alias, strings, types, replacementIndexes, length,
#अगर U_DEBUG
        [](स्थिर अक्षर* type) {
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
        [](स्थिर अक्षर*) {},
#पूर्ण_अगर
        [](स्थिर UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
व्योम
Alias_तथ्य_Builder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<स्थिर अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    वापस readAlias(
        alias, strings, types, replacementIndexes, length,
#अगर U_DEBUG
        [](स्थिर अक्षर* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](स्थिर UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#अन्यथा
        [](स्थिर अक्षर*) {},
        [](स्थिर UChar*) { },
#पूर्ण_अगर
        status);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
व्योम
Alias_तथ्य_Builder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<स्थिर अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    वापस readAlias(
        alias, strings, types, replacementIndexes, length,
#अगर U_DEBUG
        [](स्थिर अक्षर* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#अन्यथा
        [](स्थिर अक्षर*) {},
#पूर्ण_अगर
        [](स्थिर UChar*) { },
        status);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
व्योम
Alias_तथ्य_Builder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<स्थिर अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    वापस readAlias(
        alias, strings, types, replacementIndexes, length,
#अगर U_DEBUG
        [](स्थिर अक्षर* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](स्थिर UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#अन्यथा
        [](स्थिर अक्षर*) {},
        [](स्थिर UChar*) { },
#पूर्ण_अगर
        status);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
व्योम
Alias_तथ्य_Builder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<स्थिर अक्षर*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    वापस readAlias(
        alias, strings, types, replacementIndexes, length,
#अगर U_DEBUG
        [](स्थिर अक्षर* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#अन्यथा
        [](स्थिर अक्षर*) {},
#पूर्ण_अगर
        [](स्थिर UChar*) { },
        status);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
व्योम U_CALLCONV
Alias_तथ्य::load_तथ्य(UErrorCode &status)
{
#अगर_घोषित LOCALE_CANONICALIZATION_DEBUG
    UDate start = uprv_getRawUTCtime();
#पूर्ण_अगर  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    Alias_तथ्य_Builder builder;
    gSingleton = builder.build(status);
#अगर_घोषित LOCALE_CANONICALIZATION_DEBUG
    UDate end = uprv_getRawUTCtime();
    म_लिखो("AliasData::loadData took total %f ms\n", end - start);
#पूर्ण_अगर  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
Alias_तथ्य*
Alias_तथ्य_Builder::build(UErrorCode &status) {
    अगर (U_FAILURE(status)) { वापस nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_कुंजी(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_कुंजी(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_कुंजी(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_कुंजी(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_कुंजी(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_कुंजी(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }
    int32_t languages_लंबाई = 0, script_लंबाई = 0, territory_लंबाई = 0,
            variant_लंबाई = 0, subdivision_लंबाई = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<स्थिर अक्षर*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languages_लंबाई,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<स्थिर अक्षर*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    script_लंबाई,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<स्थिर अक्षर*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territory_लंबाई, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<स्थिर अक्षर*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variant_लंबाई, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<स्थिर अक्षर*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivision_लंबाई, status);

    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    क्रम (int32_t i = 0; U_SUCCESS(status) && i < languages_लंबाई; i++) {
        languageMap.दो(languageTypes[i],
                        strings.लो(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    क्रम (int32_t i = 0; U_SUCCESS(status) && i < script_लंबाई; i++) {
        scriptMap.दो(scriptTypes[i],
                      strings.लो(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    क्रम (int32_t i = 0; U_SUCCESS(status) && i < territory_लंबाई; i++) {
        territoryMap.दो(territoryTypes[i],
                         strings.लो(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    क्रम (int32_t i = 0; U_SUCCESS(status) && i < variant_लंबाई; i++) {
        variantMap.दो(variantTypes[i],
                       strings.लो(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    क्रम (int32_t i = 0; U_SUCCESS(status) && i < subdivision_लंबाई; i++) {
        subdivisionMap.दो(subdivisionTypes[i],
                       strings.लो(subdivisionReplacementIndexes[i]),
                       status);
    }

    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    // copy hashtables
    स्वतः *तथ्य = नया Alias_तथ्य(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    अगर (तथ्य == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    वापस तथ्य;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
श्रेणी AliasReplacer {
खुला:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       वापस 0==uprv_strcmp((स्थिर अक्षर*)e1.pointer,
                                             (स्थिर अक्षर*)e2.pointer);}),
                     status),
            तथ्य(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    bool replace(
        स्थिर Locale& locale, CharString& out, UErrorCode& status);

निजी:
    स्थिर अक्षर* language;
    स्थिर अक्षर* script;
    स्थिर अक्षर* region;
    स्थिर अक्षर* extensions;
    UVector variants;

    स्थिर Alias_तथ्य* तथ्य;

    अंतरभूत bool notEmpty(स्थिर अक्षर* str) {
        वापस str && str[0] != NULL_CHAR;
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
    अंतरभूत स्थिर अक्षर* deleteOrReplace(स्थिर अक्षर* input U_LIFETIME_BOUND,
                                       स्थिर अक्षर* type,
                                       स्थिर अक्षर* replacement U_LIFETIME_BOUND) {
        वापस notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    अंतरभूत bool same(स्थिर अक्षर* a, स्थिर अक्षर* b) {
        अगर (a == nullptr && b == nullptr) {
            वापस true;
        }
        अगर ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          वापस false;
        }
        वापस uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_कुंजी(स्थिर अक्षर* language, स्थिर अक्षर* region,
                            स्थिर अक्षर* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    व्योम parseLanguageReplacement(स्थिर अक्षर* replacement,
                                  स्थिर अक्षर*& replaceLanguage,
                                  स्थिर अक्षर*& replaceScript,
                                  स्थिर अक्षर*& replaceRegion,
                                  स्थिर अक्षर*& replaceVariant,
                                  स्थिर अक्षर*& replaceExtensions,
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
AliasReplacer::generate_कुंजी(
        स्थिर अक्षर* language, स्थिर अक्षर* region, स्थिर अक्षर* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    अगर (U_FAILURE(status)) { वापस out; }
    out.append(language, status);
    अगर (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    अगर (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    वापस out;
}

व्योम
AliasReplacer::parseLanguageReplacement(
    स्थिर अक्षर* replacement,
    स्थिर अक्षर*& replacedLanguage,
    स्थिर अक्षर*& replacedScript,
    स्थिर अक्षर*& replacedRegion,
    स्थिर अक्षर*& replacedVariant,
    स्थिर अक्षर*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    अगर (U_FAILURE(status)) {
        वापस;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    अगर (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        वापस;
    }
    // We have multiple field so we have to allocate and parse
    स्वतः str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    अक्षर* तथ्य = str->तथ्य();
    toBeFreed.adoptElement(str.release(), status);
    अगर (U_FAILURE(status)) {
        वापस;
    }
    replacedLanguage = (स्थिर अक्षर*) तथ्य;
    अक्षर* endOfField = uprv_strchr(तथ्य, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    स्थिर अक्षर* start = endOfField;
    endOfField = const_cast<अक्षर*>(uprv_strchr(start, '_'));
    माप_प्रकार len = 0;
    अगर (endOfField == nullptr) {
        len = uprv_strlen(start);
    } अन्यथा {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    अगर (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        अगर (endOfField == nullptr) {
            वापस;
        }
        start = endOfField++;
        endOfField = const_cast<अक्षर*>(uprv_strchr(start, '_'));
        अगर (endOfField == nullptr) {
            len = uprv_strlen(start);
        } अन्यथा {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    अगर (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        अगर (endOfField == nullptr) {
            वापस;
        }
        start = endOfField++;
        endOfField = const_cast<अक्षर*>(uprv_strchr(start, '_'));
        अगर (endOfField == nullptr) {
            len = uprv_strlen(start);
        } अन्यथा {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    अगर (len >= 4) {
        // Got a variant
        replacedVariant = start;
        अगर (endOfField == nullptr) {
            वापस;
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
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    अगर (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        वापस false;
    }
    int32_t variant_आकार = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    स्थिर अक्षर* खोज_Language = checkLanguage ? language : "und";
    स्थिर अक्षर* खोज_Region = checkRegion ? region : nullptr;
    स्थिर अक्षर* खोज_Variant = nullptr;
    क्रम (int32_t variant_सूचकांक = 0;
            variant_सूचकांक < variant_आकार;
            variant_सूचकांक++) {
        अगर (checkVariants) {
            U_ASSERT(variant_सूचकांक < variant_आकार);
            खोज_Variant = static_cast<स्थिर अक्षर*>(variants.elementAt(variant_सूचकांक));
        }

        अगर (खोज_Variant != nullptr && uprv_strlen(खोज_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            खोज_Variant = nullptr;
        }
        CharString type_कुंजी;
        generate_कुंजी(खोज_Language, खोज_Region, खोज_Variant, type_कुंजी,
                    status);
        अगर (U_FAILURE(status)) {
            वापस false;
        }
        स्थिर अक्षर *replacement = तथ्य->languageMap().लो(type_कुंजी.तथ्य());
        अगर (replacement == nullptr) {
            // Found no replacement data.
            जारी;
        }

        स्थिर अक्षर* replacedLanguage = nullptr;
        स्थिर अक्षर* replacedScript = nullptr;
        स्थिर अक्षर* replacedRegion = nullptr;
        स्थिर अक्षर* replacedVariant = nullptr;
        स्थिर अक्षर* replacedExtensions = nullptr;
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

        अगर (    same(language, replacedLanguage) &&
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
        अगर (खोज_Variant != nullptr) {
            अगर (notEmpty(replacedVariant)) {
                variants.setElementAt((व्योम*)replacedVariant, variant_सूचकांक);
            } अन्यथा {
                variants.removeElementAt(variant_सूचकांक);
            }
        }
        अगर (replacedExtensions != nullptr) {
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
        वापस true;
    }
    // Nothing changed by language alias data.
    वापस false;
}

bool
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    अगर (region == nullptr) {
        // No region to search.
        वापस false;
    }
    स्थिर अक्षर *replacement = तथ्य->territoryMap().लो(region);
    अगर (replacement == nullptr) {
        // Found no replacement data for this region.
        वापस false;
    }
    स्थिर अक्षर* replacedRegion = replacement;
    स्थिर अक्षर* firstSpace = uprv_strchr(replacement, ' ');
    अगर (firstSpace != nullptr) {
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
        स्थिर अक्षर* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        अगर (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            माप_प्रकार len = uprv_strlen(likelyRegion);
            स्थिर अक्षर* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            अगर (foundInReplacement != nullptr) {
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
        अगर (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        अगर (U_FAILURE(status)) { वापस false; }
        replacedRegion = item->तथ्य();
        toBeFreed.adoptElement(item.release(), status);
        अगर (U_FAILURE(status)) { वापस false; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    वापस true;
}

bool
AliasReplacer::replaceScript(UErrorCode& status)
{
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    अगर (script == nullptr) {
        // No script to search.
        वापस false;
    }
    स्थिर अक्षर *replacement = तथ्य->scriptMap().लो(script);
    अगर (replacement == nullptr) {
        // Found no replacement data for this script.
        वापस false;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    वापस true;
}

bool
AliasReplacer::replaceVariant(UErrorCode& status)
{
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    // Since we may have more than one variant, we need to loop through them.
    क्रम (int32_t i = 0; i < variants.size(); i++) {
        स्थिर अक्षर* variant = static_cast<स्थिर अक्षर*>(variants.elementAt(i));
        स्थिर अक्षर *replacement = तथ्य->variantMap().लो(variant);
        अगर (replacement == nullptr) {
            // Found no replacement data for this variant.
            जारी;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        अगर (!same(variant, replacement)) {
            variants.setElementAt((व्योम*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            अगर (uprv_strcmp(variant, "heploc") == 0) {
                क्रम (int32_t j = 0; j < variants.size(); j++) {
                     अगर (uprv_strcmp((स्थिर अक्षर*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            वापस true;
        }
    }
    वापस false;
}

bool
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    स्थिर अक्षर *replacement = तथ्य->subdivisionMap().लो(subdivision.तथ्य());
    अगर (replacement != nullptr) {
        स्थिर अक्षर* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        माप_प्रकार len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        अगर (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            अगर (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        वापस true;
    }
    वापस false;
}

bool
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    int32_t len = transformedExtensions.length();
    स्थिर अक्षर* str = transformedExtensions.तथ्य();
    स्थिर अक्षर* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    अगर (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        अगर (U_FAILURE(status)) {
            वापस false;
        }
        T_CString_toLowerCase(output.तथ्य());
    }
    अगर (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        अगर (U_FAILURE(status)) {
            वापस false;
        }
        करो {
            स्थिर अक्षर* tvalue = uprv_strchr(tkey, '-');
            अगर (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                वापस false;
            }
            स्थिर अक्षर* अग्र_TKey = ultag_getTKeyStart(tvalue);
            अगर (अग्र_TKey != nullptr) {
                *const_cast<अक्षर*>(अग्र_TKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.जोड़ो_Element_At((व्योम*)tkey, tfields.size(), status);
            अगर (U_FAILURE(status)) {
                वापस false;
            }
            tkey = अग्र_TKey;
        } जबतक (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            वापस uprv_strcmp((स्थिर अक्षर*)e1.pointer, (स्थिर अक्षर*)e2.pointer);
        }, status);
        क्रम (int32_t i = 0; i < tfields.size(); i++) {
             अगर (output.length() > 0) {
                 output.append('-', status);
             }
             स्थिर अक्षर* tfield = static_cast<स्थिर अक्षर*>(tfields.elementAt(i));
             स्थिर अक्षर* tvalue = uprv_strchr(tfield, '-');
             अगर (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 वापस false;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<अक्षर*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    वापस true;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    अगर (U_FAILURE(status)) { वापस out; }
    out.append(language, status);
    अगर (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    अगर (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    अगर (variants.size() > 0) {
        अगर (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            वापस uprv_strcmp((स्थिर अक्षर*)e1.pointer, (स्थिर अक्षर*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        क्रम (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<स्थिर अक्षर*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.तथ्य() + variantsStart);
    }
    अगर (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.तथ्य());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    वापस out;
}

bool
AliasReplacer::replace(स्थिर Locale& locale, CharString& out, UErrorCode& status)
{
    तथ्य = Alias_तथ्य::singleton(status);
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    U_ASSERT(तथ्य != nullptr);
    out.सफा();
    language = locale.getLanguage();
    अगर (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    अगर (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    अगर (!notEmpty(region)) {
        region = nullptr;
    }
    स्थिर अक्षर* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    अगर (!variantsBuff.isEmpty()) {
        अगर (U_FAILURE(status)) { वापस false; }
        अक्षर* start = variantsBuff.तथ्य();
        T_CString_toLowerCase(start);
        अक्षर* end;
        जबतक ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            अगर (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        अगर (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    अगर (U_FAILURE(status)) { वापस false; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        वापस uprv_strcmp((स्थिर अक्षर*)e1.pointer, (स्थिर अक्षर*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    स्थिर int32_t maxChanged = 4 + variants.size();
    (व्योम)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](व्योम *obj) { मिटाओ static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    जबतक (U_SUCCESS(status)) {
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
        अगर (    replaceLanguage(true, true,  true,  stringsToBeFreed, status) ||
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
            जारी;
        }
        // Nothing changed. Break out.
        अवरोध;
    }  // while(1)

    अगर (U_FAILURE(status)) { वापस false; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    स्थिर अक्षर* extensionsStr = locale_getKeywordsStart(locale.getName());
    अगर (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        वापस false;
    }
    outputToString(out, status);
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    अगर (extensionsStr != nullptr) {
        changed = 0;
        Locale अस्थायी(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        अगर (U_SUCCESS(status) && !iter.isNull()) {
            स्थिर अक्षर* कुंजी;
            जबतक ((कुंजी = iter->next(nullptr, status)) != nullptr) {
                अगर (uprv_strcmp("sd", कुंजी) == 0 || uprv_strcmp("rg", कुंजी) == 0 ||
                        uprv_strcmp("t", कुंजी) == 0) {
                    स्वतः value = locale.get_Keyword_मूल्य<CharString>(कुंजी, status);
                    अगर (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        जारी;
                    }
                    CharString replacement;
                    अगर (uprv_strlen(कुंजी) == 2) {
                        अगर (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            अस्थायी.set_Keyword_मूल्य(कुंजी, replacement.तथ्य(), status);
                        }
                    } अन्यथा {
                        U_ASSERT(uprv_strcmp(कुंजी, "t") == 0);
                        अगर (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            अस्थायी.set_Keyword_मूल्य(कुंजी, replacement.तथ्य(), status);
                        }
                    }
                    अगर (U_FAILURE(status)) {
                        वापस false;
                    }
                }
            }
        }
        अगर (changed != 0) {
            extensionsStr = locale_getKeywordsStart(अस्थायी.getName());
        }
        out.append(extensionsStr, status);
    }
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    // If the tag is not changed, return.
    अगर (uprv_strcmp(out.तथ्य(), locale.getName()) == 0) {
        out.सफा();
        वापस false;
    }
    वापस true;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
bool
canonicalizeLocale(स्थिर Locale& locale, CharString& out, UErrorCode& status)
{
    अगर (U_FAILURE(status)) { वापस false; }
    AliasReplacer replacer(status);
    वापस replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
bool
isKnownCanonicalizedLocale(स्थिर अक्षर* locale, UErrorCode& status)
{
    अगर (U_FAILURE(status)) { वापस false; }

    अगर (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        वापस true;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    वापस uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT स्थिर अक्षर* स्थिर*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    वापस KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT bool
ulocimp_isCanonicalizedLocaleForTest(स्थिर अक्षर* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString अस्थायी;
    वापस !canonicalizeLocale(l, अस्थायी, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(स्थिर अक्षर* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    वापस localeID == nullptr ? *यह = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    करो {
        अक्षर *separator;
        अक्षर *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        स्थिर स्वतः parse = [canonicalize](std::string_view localeID,
                                          अक्षर* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    अगर (canonicalize) {
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
        length = parse(localeID, fullName, माप Nest::baseName, err);

        FixedString full_Name_बफर;
        अगर (err == U_बफर_OVERFLOW_ERROR || length >= static_cast<int32_t>(माप Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            अगर (!full_Name_बफर.reserve(length + 1)) {
                अवरोध; // error: out of memory
            }
            fullName = full_Name_बफर.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        अगर(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            अवरोध;
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
        जबतक ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        अक्षर* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        अगर (separator!=nullptr || sep2!=nullptr) {
            अगर (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } अन्यथा {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        bool hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        अगर (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            अवरोध; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        अगर (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        अगर (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        अगर (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } अन्यथा अगर (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        अगर (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } अन्यथा अगर (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        अगर (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(full_Name_बफर.isEmpty());
            nest.init(language, script, region, variantBegin);
        } अन्यथा {
            अगर (fullName == nest.baseName) {
                U_ASSERT(full_Name_बफर.isEmpty());
                full_Name_बफर = {fullName, static_cast<std::string_view::size_type>(length)};
                अगर (full_Name_बफर.isEmpty()) {
                    अवरोध; // error: out of memory
                }
                अगर (!language.empty()) {
                    language = {full_Name_बफर.तथ्य(), language.size()};
                }
                अगर (!script.empty()) {
                    script = {full_Name_बफर.तथ्य() + (script.तथ्य() - fullName), script.size()};
                }
                अगर (!region.empty()) {
                    region = {full_Name_बफर.तथ्य() + (region.तथ्य() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            अगर (isBogus()) {
                अवरोध; // error: out of memory
            }
            U_ASSERT(!full_Name_बफर.isEmpty());
            heap.ptr->fullName = std::move(full_Name_बफर);
            अगर (hasKeywords) {
                अगर (std::string_view::size_type base_Name_लंबाई = at - fullName; base_Name_लंबाई > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.तथ्य(), base_Name_लंबाई};
                    अगर (heap.ptr->baseName.isEmpty()) {
                        अवरोध; // error: out of memory
                    }
                }
            }
        }

        अगर (canonicalize) {
            अगर (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                अगर (canonicalizeLocale(*यह, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.तथ्य(), false);
                }
                अगर (U_FAILURE(err)) {
                    अवरोध;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        वापस *यह;
    } जबतक(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    वापस *यह;
}

int32_t
Locale::hashCode() स्थिर
{
    वापस ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

व्योम
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

स्थिर Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        अगर (gDefaultLocale != nullptr) {
            वापस *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    वापस *locale_set_default_internal(nullptr, status);
}



व्योम U_EXPORT2
Locale::setDefault( स्थिर   Locale&     newLocale,
                            UErrorCode&  status)
{
    अगर (U_FAILURE(status)) {
        वापस;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    स्थिर अक्षर *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

व्योम
Locale::addLikelySubtags(UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    अगर (U_FAILURE(status)) {
        अगर (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        वापस;
    }

    init(maximizedLocaleID.तथ्य(), /*canonicalize=*/false);
    अगर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

व्योम
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(false, status);
}
व्योम
Locale::minimizeSubtags(bool favorScript, UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    अगर (U_FAILURE(status)) {
        अगर (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        वापस;
    }

    init(minimizedLocaleID.तथ्य(), /*canonicalize=*/false);
    अगर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

व्योम
Locale::canonicalize(UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    अगर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    CharString uncanonicalized(getName(), status);
    अगर (U_FAILURE(status)) {
        अगर (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        वापस;
    }
    init(uncanonicalized.तथ्य(), /*canonicalize=*/true);
    अगर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale परिणाम(Locale::eBOGUS);

    अगर (U_FAILURE(status)) {
        वापस परिणाम;
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

    int32_t parsed_लंबाई;
    CharString localeID = ulocimp_forLanguageTag(
            tag.तथ्य(),
            tag.length(),
            &parsed_लंबाई,
            status);

    अगर (U_FAILURE(status)) {
        वापस परिणाम;
    }

    अगर (parsed_लंबाई != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस परिणाम;
    }

    परिणाम.init(localeID.तथ्य(), /*canonicalize=*/false);
    अगर (परिणाम.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    वापस परिणाम;
}

व्योम
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) स्थिर
{
    अगर (U_FAILURE(status)) {
        वापस;
    }

    अगर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/false, status);
}

Locale U_EXPORT2
Locale::createFromName (स्थिर अक्षर *name)
{
    अगर (name) {
        Locale l("");
        l.init(name, false);
        वापस l;
    }
    अन्यथा {
        वापस getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, false);
    वापस loc;
}

Locale U_EXPORT2
Locale::createCanonical(स्थिर अक्षर* name) {
    Locale loc("");
    loc.init(name, true);
    वापस loc;
}

स्थिर अक्षर *
Locale::getISO3Language() स्थिर
{
    वापस uloc_getISO3Language(getName());
}


स्थिर अक्षर *
Locale::getISO3Country() स्थिर
{
    वापस uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() स्थिर
{
    वापस uloc_getLCID(getName());
}

स्थिर अक्षर* स्थिर* U_EXPORT2 Locale::getISOCountries()
{
    वापस uloc_getISOCountries();
}

स्थिर अक्षर* स्थिर* U_EXPORT2 Locale::getISOLanguages()
{
    वापस uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
व्योम Locale::setFromPOSIXID(स्थिर अक्षर *posixID)
{
    init(posixID, true);
}

स्थिर Locale & U_EXPORT2
Locale::get_मूल()
{
    वापस getLocale(e_मूल);
}

स्थिर Locale & U_EXPORT2
Locale::getEnglish()
{
    वापस getLocale(eENGLISH);
}

स्थिर Locale & U_EXPORT2
Locale::getFrench()
{
    वापस getLocale(eFRENCH);
}

स्थिर Locale & U_EXPORT2
Locale::getGerman()
{
    वापस getLocale(eGERMAN);
}

स्थिर Locale & U_EXPORT2
Locale::getItalian()
{
    वापस getLocale(eITALIAN);
}

स्थिर Locale & U_EXPORT2
Locale::getJapanese()
{
    वापस getLocale(eJAPANESE);
}

स्थिर Locale & U_EXPORT2
Locale::getKorean()
{
    वापस getLocale(eKOREAN);
}

स्थिर Locale & U_EXPORT2
Locale::getChinese()
{
    वापस getLocale(eCHINESE);
}

स्थिर Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    वापस getLocale(eCHINA);
}

स्थिर Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    वापस getLocale(eTAIWAN);
}


स्थिर Locale & U_EXPORT2
Locale::getFrance()
{
    वापस getLocale(eFRANCE);
}

स्थिर Locale & U_EXPORT2
Locale::getGermany()
{
    वापस getLocale(eGERMANY);
}

स्थिर Locale & U_EXPORT2
Locale::getItaly()
{
    वापस getLocale(eITALY);
}

स्थिर Locale & U_EXPORT2
Locale::getJapan()
{
    वापस getLocale(eJAPAN);
}

स्थिर Locale & U_EXPORT2
Locale::getKorea()
{
    वापस getLocale(eKOREA);
}

स्थिर Locale & U_EXPORT2
Locale::getChina()
{
    वापस getLocale(eCHINA);
}

स्थिर Locale & U_EXPORT2
Locale::getPRC()
{
    वापस getLocale(eCHINA);
}

स्थिर Locale & U_EXPORT2
Locale::getTaiwan()
{
    वापस getLocale(eTAIWAN);
}

स्थिर Locale & U_EXPORT2
Locale::getUK()
{
    वापस getLocale(eUK);
}

स्थिर Locale & U_EXPORT2
Locale::getUS()
{
    वापस getLocale(eUS);
}

स्थिर Locale & U_EXPORT2
Locale::getCanada()
{
    वापस getLocale(eCANADA);
}

स्थिर Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    वापस getLocale(eCANADA_FRENCH);
}

स्थिर Locale &
Locale::getLocale(पूर्णांक locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    अगर (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    वापस localeCache[locid]; /*operating on nullptr*/
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
    वापस gLocaleCache;
}

श्रेणी KeywordEnumeration : खुला StringEnumeration {
रक्षित:
    FixedString keywords;
निजी:
    int32_t length;
    स्थिर अक्षर *current;
    अटल स्थिर अक्षर fgClassID;

खुला:
    अटल UClassID U_EXPORT2 getStaticClassID() { वापस (UClassID)&fgClassID; }
    भव UClassID getDynamicClassID() स्थिर override { वापस getStaticClassID(); }
खुला:
    KeywordEnumeration(स्थिर अक्षर *कुंजियाँ, int32_t keywordLen, int32_t current_सूचकांक, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        अगर(U_SUCCESS(status) && keywordLen != 0) {
            अगर(कुंजियाँ == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } अन्यथा {
                keywords = {कुंजियाँ, static_cast<std::string_view::size_type>(length)};
                अगर (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अन्यथा {
                    current = keywords.तथ्य() + current_सूचकांक;
                }
            }
        }
    }

    भव ~KeywordEnumeration();

    भव StringEnumeration * clone() स्थिर override
    {
        UErrorCode status = U_ZERO_ERROR;
        वापस नया KeywordEnumeration(
                keywords.तथ्य(), length,
                static_cast<int32_t>(current - keywords.तथ्य()), status);
    }

    भव int32_t count(UErrorCode& status) स्थिर override {
        अगर (U_FAILURE(status)) { वापस 0; }
        स्थिर अक्षर *kw = keywords.तथ्य();
        int32_t परिणाम = 0;
        जबतक(*kw) {
            परिणाम++;
            kw += uprv_strlen(kw)+1;
        }
        वापस परिणाम;
    }

    भव स्थिर अक्षर* next(int32_t* परिणाम_लंबाई, UErrorCode& status) U_LIFETIME_BOUND override {
        स्थिर अक्षर* परिणाम;
        int32_t len;
        अगर(U_SUCCESS(status) && *current != 0) {
            परिणाम = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            अगर(परिणाम_लंबाई != nullptr) {
                *परिणाम_लंबाई = len;
            }
        } अन्यथा {
            अगर(परिणाम_लंबाई != nullptr) {
                *परिणाम_लंबाई = 0;
            }
            परिणाम = nullptr;
        }
        वापस परिणाम;
    }

    भव स्थिर UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        अगर (U_FAILURE(status)) { वापस nullptr; }
        int32_t परिणाम_लंबाई = 0;
        स्थिर अक्षर *s = next(&परिणाम_लंबाई, status);
        वापस setChars(s, परिणाम_लंबाई, status);
    }

    भव व्योम reset(UErrorCode& status) override {
        अगर (U_FAILURE(status)) { वापस; }
        current = keywords.तथ्य();
    }
};

स्थिर अक्षर KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = शेष;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
श्रेणी UnicodeKeywordEnumeration : खुला KeywordEnumeration {
खुला:
    using KeywordEnumeration::KeywordEnumeration;
    भव ~UnicodeKeywordEnumeration();

    भव स्थिर अक्षर* next(int32_t* परिणाम_लंबाई, UErrorCode& status) U_LIFETIME_BOUND override {
        स्थिर अक्षर* legacy_कुंजी = KeywordEnumeration::next(nullptr, status);
        जबतक (U_SUCCESS(status) && legacy_कुंजी != nullptr) {
            स्थिर अक्षर* कुंजी = uloc_to_Unicode_Locale_कुंजी(legacy_कुंजी);
            अगर (कुंजी != nullptr) {
                अगर (परिणाम_लंबाई != nullptr) {
                    *परिणाम_लंबाई = static_cast<int32_t>(uprv_strlen(कुंजी));
                }
                वापस कुंजी;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_कुंजी = KeywordEnumeration::next(nullptr, status);
        }
        अगर (परिणाम_लंबाई != nullptr) *परिणाम_लंबाई = 0;
        वापस nullptr;
    }
    भव int32_t count(UErrorCode& status) स्थिर override {
        अगर (U_FAILURE(status)) { वापस 0; }
        स्थिर अक्षर *kw = keywords.तथ्य();
        int32_t परिणाम = 0;
        जबतक(*kw) {
            अगर (uloc_to_Unicode_Locale_कुंजी(kw) != nullptr) {
                परिणाम++;
            }
            kw += uprv_strlen(kw)+1;
        }
        वापस परिणाम;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = शेष;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) स्थिर
{
    StringEnumeration *परिणाम = nullptr;

    अगर (U_FAILURE(status)) {
        वापस परिणाम;
    }

    स्थिर अक्षर* variantStart = uprv_strchr(getName(), '@');
    स्थिर अक्षर* assignment = uprv_strchr(getName(), '=');
    अगर(variantStart) {
        अगर(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', false, status);
            अगर (U_SUCCESS(status) && !keywords.isEmpty()) {
                परिणाम = नया KeywordEnumeration(keywords.तथ्य(), keywords.length(), 0, status);
                अगर (!परिणाम) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } अन्यथा {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    वापस परिणाम;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) स्थिर
{
    StringEnumeration *परिणाम = nullptr;

    अगर (U_FAILURE(status)) {
        वापस परिणाम;
    }

    स्थिर अक्षर* variantStart = uprv_strchr(getName(), '@');
    स्थिर अक्षर* assignment = uprv_strchr(getName(), '=');
    अगर(variantStart) {
        अगर(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', false, status);
            अगर (U_SUCCESS(status) && !keywords.isEmpty()) {
                परिणाम = नया UnicodeKeywordEnumeration(keywords.तथ्य(), keywords.length(), 0, status);
                अगर (!परिणाम) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } अन्यथा {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    वापस परिणाम;
}

int32_t
Locale::get_Keyword_मूल्य(स्थिर अक्षर* keywordName, अक्षर *बफर, int32_t bufLen, UErrorCode &status) स्थिर
{
    वापस uloc_get_Keyword_मूल्य(getName(), keywordName, बफर, bufLen, &status);
}

व्योम
Locale::get_Keyword_मूल्य(StringPiece keywordName, ByteSink& sink, UErrorCode& status) स्थिर {
    अगर (U_FAILURE(status)) {
        वापस;
    }

    अगर (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    ulocimp_get_Keyword_मूल्य(getName(), keywordName, sink, status);
}

व्योम
Locale::get_Unicode_Keyword_मूल्य(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) स्थिर {
    अगर (U_FAILURE(status)) {
        वापस;
    }

    std::optional<std::string_view> legacy_कुंजी = ulocimp_to_Legacy_कुंजी_With_Fallback(keywordName);
    अगर (!legacy_कुंजी.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    स्वतः legacy_मूल्य = get_Keyword_मूल्य<CharString>(*legacy_कुंजी, status);

    अगर (U_FAILURE(status)) {
        वापस;
    }

    std::optional<std::string_view> unicode_मूल्य =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_मूल्य.toStringPiece());
    अगर (!unicode_मूल्य.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    sink.Append(unicode_मूल्य->तथ्य(), static_cast<int32_t>(unicode_मूल्य->size()));
}

व्योम
Locale::set_Keyword_मूल्य(StringPiece keywordName,
                        StringPiece keyword_मूल्य,
                        UErrorCode& status) {
    अगर (U_FAILURE(status)) { वापस; }
    अगर (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    अगर (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_set_Keyword_मूल्य(keywordName, keyword_मूल्य, localeID, status);
    अगर (U_FAILURE(status)) {
        अगर (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        वापस;
    }

    स्थिर अक्षर* at = locale_getKeywordsStart(localeID.toStringPiece());
    bool hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.लो<Nest>();
    अगर (!hasKeywords) {
        अगर (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.लो<Heap>();
            U_ASSERT(heap != nullptr);
            अगर (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<माप_प्रकार>(variantBegin) < माप Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, माप Nest::baseName, status);
            } अन्यथा {
                heap->ptr->baseName.सफा();
                heap->ptr->fullName = localeID.toStringPiece();
                अगर (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    वापस;
                }
            }
        }
    } अन्यथा {
        Heap* heap = nullptr;
        अगर (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            अगर (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
        } अन्यथा {
            heap = payload.लो<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        अगर (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            वापस;
        }

        अगर (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            अगर (std::string_view::size_type base_Name_लंबाई = at - localeID.तथ्य(); base_Name_लंबाई > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.तथ्य(), base_Name_लंबाई};
                अगर (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    वापस;
                }
            }
        }
    }
}

व्योम
Locale::set_Unicode_Keyword_मूल्य(StringPiece keywordName,
                               StringPiece keyword_मूल्य,
                               UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }

    std::optional<std::string_view> legacy_कुंजी = ulocimp_to_Legacy_कुंजी_With_Fallback(keywordName);
    अगर (!legacy_कुंजी.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    std::string_view value;

    अगर (!keyword_मूल्य.empty()) {
        std::optional<std::string_view> legacy_मूल्य =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_मूल्य);
        अगर (!legacy_मूल्य.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }
        value = *legacy_मूल्य;
    }

    set_Keyword_मूल्य(*legacy_कुंजी, value, status);
}

स्थिर अक्षर*
Locale::getCountry() स्थिर U_LIFETIME_BOUND {
    वापस getField<&Nest::getRegion, &Heap::getRegion>();
}

स्थिर अक्षर*
Locale::getLanguage() स्थिर U_LIFETIME_BOUND {
    वापस getField<&Nest::getLanguage, &Heap::getLanguage>();
}

स्थिर अक्षर*
Locale::getScript() स्थिर U_LIFETIME_BOUND {
    वापस getField<&Nest::getScript, &Heap::getScript>();
}

स्थिर अक्षर*
Locale::getVariant() स्थिर U_LIFETIME_BOUND {
    वापस getField<&Nest::getVariant, &Heap::getVariant>();
}

स्थिर अक्षर*
Locale::getName() स्थिर U_LIFETIME_BOUND {
    वापस getField<&Nest::getBaseName, &Heap::getFullName>();
}

स्थिर अक्षर*
Locale::getBaseName() स्थिर U_LIFETIME_BOUND {
    वापस getField<&Nest::getBaseName, &Heap::getBaseName>();
}

ढाँचा <स्थिर अक्षर* (Locale::Nest::*स्थिर NEST)() स्थिर,
          स्थिर अक्षर* (Locale::Heap::*स्थिर HEAP)() स्थिर>
स्थिर अक्षर* Locale::getField() स्थिर U_LIFETIME_BOUND {
    वापस payload.visit([] { वापस ""; },
                         [](स्थिर Nest& nest) { वापस (nest.*NEST)(); },
                         [](स्थिर Heap& heap) { वापस (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = शेष;

//eof
U_NAMESPACE_END
