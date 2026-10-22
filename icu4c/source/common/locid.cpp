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

#অন্তর্ভাৱ <cstddef>
#অন্তর্ভাৱ <optional>
#অন্তর্ভাৱ <string_view>
#অন্তর্ভাৱ <type_traits>
#অন্তর্ভাৱ <utility>

#অন্তর্ভাৱ "unicode/bytestream.h"
#অন্তর্ভাৱ "unicode/locid.h"
#অন্তর্ভাৱ "unicode/localebuilder.h"
#অন্তর্ভাৱ "unicode/localpointer.h"
#অন্তর্ভাৱ "unicode/strenum.h"
#অন্তর্ভাৱ "unicode/stringpiece.h"
#অন্তর্ভাৱ "unicode/uloc.h"
#অন্তর্ভাৱ "unicode/ures.h"

#অন্তর্ভাৱ "bytesinkutil.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "charstrmap.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "fixedstring.h"
#অন্তর্ভাৱ "mutex.h"
#অন্তর্ভাৱ "putilimp.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "ucln_cmn.h"
#অন্তর্ভাৱ "uhash.h"
#অন্তর্ভাৱ "ulocimp.h"
#অন্তর্ভাৱ "umutex.h"
#অন্তর্ভাৱ "uniquecharstr.h"
#অন্তর্ভাৱ "ustr_imp.h"
#অন্তর্ভাৱ "uvector.h"

#যদি UPRV_HAS_FEATURE(memory_sanitizer)
#অন্তর্ভাৱ <sanitizer/msan_interface.h>
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

প্রকারনির্দেশ গণনা ELocalePos {
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
    ঘূৰাই সঁচা;
}

শূন্য U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = নূতন Locale[static_cast<পূর্ণাঙ্ক>(eMAX_LOCALES)];
    যদি (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
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

Locale *locale_set_default_internal(ধ্রুৱ ৱর্ণ *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = মিছা;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    যদি (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = সঁচা; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    যদি (U_FAILURE(status)) {
        ঘূৰাই gDefaultLocale;
    }

    যদি (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        যদি (U_FAILURE(status)) {
            ঘূৰাই gDefaultLocale;
        }
        uhash_set_মান_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    যদি (newDefault == nullptr) {
        newDefault = নূতন Locale(Locale::eBOGUS);
        যদি (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), মিছা);
        uhash_put(gDefaultLocalesHashT, const_cast<ৱর্ণ*>(newDefault->getName()), newDefault, &status);
        যদি (U_FAILURE(status)) {
            ঘূৰাই gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    ঘূৰাই gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC শূন্য
locale_set_default(ধ্রুৱ ৱর্ণ *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC ধ্রুৱ ৱর্ণ *
locale_get_default()
{
    U_NAMESPACE_USE
    ঘূৰাই Locale::getDefault().getName();
}

নামাকাশ {

template <auto FIELD, typename T>
শূন্য copyToArray(std::string_view sv, T* that) {
    auto& field = that->*FIELD;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    যদি (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.data(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

শূন্য Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, এতত্);
    copyToArray<&Nest::script>(script, এতত্);
    copyToArray<&Nest::region>(region, এতত্);
    এতত্->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(Heap, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(Heap, region));
    U_ASSERT(এতত্ == reinterpret_cast<Nest*>(&heap));
#যদি UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, পরিমাণ(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, এতত্);
    copyToArray<&Nest::region>(heap.region, এতত্);
    এতত্->variantBegin = variantBegin;
    *এতত্->baseName = '\0';
}

সংরচনা Locale::Heap::Alloc : সার্ৱজনিক UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    ধ্রুৱ ৱর্ণ* getVariant() ধ্রুৱ U_LIFETIME_BOUND {
        ঘূৰাই variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    ধ্রুৱ ৱর্ণ* getFullName() ধ্রুৱ U_LIFETIME_BOUND { ঘূৰাই fullName.data(); }
    ধ্রুৱ ৱর্ণ* getBaseName() ধ্রুৱ U_LIFETIME_BOUND {
        যদি (baseName.isEmpty()) {
            যদি (ধ্রুৱ ৱর্ণ* name = fullName.data(); *name != '@') {
                ঘূৰাই name;
            }
        }
        ঘূৰাই baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(ধ্রুৱ Alloc& other, UErrorCode& status)
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

    ~Alloc() = স্ৱভাৱ;
};

ধ্রুৱ ৱর্ণ* Locale::Heap::getVariant() ধ্রুৱ U_LIFETIME_BOUND { ঘূৰাই ptr->getVariant(); }
ধ্রুৱ ৱর্ণ* Locale::Heap::getFullName() ধ্রুৱ U_LIFETIME_BOUND { ঘূৰাই ptr->getFullName(); }
ধ্রুৱ ৱর্ণ* Locale::Heap::getBaseName() ধ্রুৱ U_LIFETIME_BOUND { ঘূৰাই ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = নূতন Alloc(variantBegin);
    যদি (ptr == nullptr) {
        type = eBOGUS;
    } অন্যথা {
        type = eHEAP;
        copyToArray<&Heap::language>(language, এতত্);
        copyToArray<&Heap::script>(script, এতত্);
        copyToArray<&Heap::region>(region, এতত্);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::operator=(ধ্রুৱ Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = নূতন Alloc(*other.ptr, status);
    যদি (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } অন্যথা {
        type = eHEAP;
        uprv_memcpy(language, other.language, পরিমাণ language);
        uprv_memcpy(script, other.script, পরিমাণ script);
        uprv_memcpy(region, other.region, পরিমাণ region);
    }
    ঘূৰাই *এতত্;
}

Locale::Heap& Locale::Heap::operator=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, পরিমাণ language);
    uprv_memcpy(script, other.script, পরিমাণ script);
    uprv_memcpy(region, other.region, পরিমাণ region);
    ঘূৰাই *এতত্;
}

template <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
auto Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) ধ্রুৱ {
    ৱিকল্প (type) {
        পক্ষ eBOGUS:
            ঘূৰাই bogusFn(args...);
        পক্ষ eNEST:
            ঘূৰাই nestFn(nest, args...);
        পক্ষ eHEAP:
            ঘূৰাই heapFn(heap, args...);
        স্ৱভাৱ:
            UPRV_UNREACHABLE_EXIT;
    };
}

শূন্য Locale::Payload::copy(ধ্রুৱ Payload& other) {
    other.visit([](Payload*) {},
                [](ধ্রুৱ Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ধ্রুৱ Heap& heap, Payload* dst) { dst->heap = heap; },
                এতত্);
}

শূন্য Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](ধ্রুৱ Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ধ্রুৱ Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        এতত্);
}

Locale::Payload::~Payload() {
    যদি (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(ধ্রুৱ Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::operator=(ধ্রুৱ Payload& other) U_LIFETIME_BOUND {
    যদি (এতত্ != &other) {
        setToBogus();
        copy(other);
    }
    ঘূৰাই *এতত্;
}

Locale::Payload& Locale::Payload::operator=(Payload&& other) noexcept U_LIFETIME_BOUND {
    যদি (এতত্ != &other) {
        setToBogus();
        move(std::move(other));
    }
    ঘূৰাই *এতত্;
}

শূন্য Locale::Payload::setToBogus() {
    এতত্->~Payload();
    type = eBOGUS;
}

template <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    যদি constexpr (std::is_same_v<T, Nest>) {
        এতত্->~Payload();
        ::নূতন (&nest) Nest(std::forward<Args>(args)...);
        ঘূৰাই nest;
    }
    যদি constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::নূতন (&heap) Heap(std::forward<Args>(args)...);
        ঘূৰাই heap;
    }
}

template <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    ঘূৰাই type == eNEST ? &nest : nullptr;
}
template <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    ঘূৰাই type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = স্ৱভাৱ;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, মিছা);
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


Locale::Locale( ধ্রুৱ   ৱর্ণ * newLanguage,
                ধ্রুৱ   ৱর্ণ * newCountry,
                ধ্রুৱ   ৱর্ণ * newVariant,
                ধ্রুৱ   ৱর্ণ * newKeywords)
    : UObject(), payload()
{
    যদি( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, মিছা); /* shortcut */
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
                ঘূৰাই;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        যদি ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            যদি ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                ঘূৰাই;
            }
        }

        // _Variant
        যদি ( newVariant != nullptr )
        {
            // remove leading _'s
            যেতিয়ালৈ(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            যদি ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                ঘূৰাই;
            }
            যেতিয়ালৈ( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        যদি ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            যদি ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              ঘূৰাই;
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
            ঘূৰাই;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), মিছা);
    }
}

Locale::Locale(ধ্রুৱ Locale&) = স্ৱভাৱ;
Locale::Locale(Locale&&) noexcept = স্ৱভাৱ;

Locale& Locale::operator=(ধ্রুৱ Locale&) U_LIFETIME_BOUND = স্ৱভাৱ;
Locale& Locale::operator=(Locale&&) noexcept U_LIFETIME_BOUND = স্ৱভাৱ;

Locale *
Locale::clone() ধ্রুৱ {
    ঘূৰাই নূতন Locale(*এতত্);
}

তার্কিক
Locale::operator==( ধ্রুৱ   Locale& other) ধ্রুৱ
{
    ঘূৰাই uprv_strcmp(other.getName(), getName()) == 0;
}

নামাকাশ {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr ধ্রুৱ ৱর্ণ* KNOWN_CANONICALIZED[] = {
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
    ঘূৰাই সঁচা;
}

শূন্য U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    বাবে (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (শূন্য*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

শ্ৰেণী AliasData;

/**
 * A Builder class to build the alias data.
 */
শ্ৰেণী AliasDataBuilder {
সার্ৱজনিক:
    AliasDataBuilder() {
    }

    // Build the AliasData from resource.
    AliasData* build(UErrorCode &status);

গুপ্ত:
    শূন্য readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   শূন্য (*checkType)(ধ্রুৱ ৱর্ণ* type),
                   শূন্য (*checkReplacement)(ধ্রুৱ UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    শূন্য readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
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
                         LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    শূন্য readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    শূন্য readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    শূন্য readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
শ্ৰেণী AliasData : সার্ৱজনিক UMemory {
সার্ৱজনিক:
    স্থির ধ্রুৱ AliasData* singleton(UErrorCode& status) {
        যদি (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            ঘূৰাই nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        ঘূৰাই gSingleton;
    }

    ধ্রুৱ CharStringMap& languageMap() ধ্রুৱ U_LIFETIME_BOUND { ঘূৰাই language; }
    ধ্রুৱ CharStringMap& scriptMap() ধ্রুৱ U_LIFETIME_BOUND { ঘূৰাই script; }
    ধ্রুৱ CharStringMap& territoryMap() ধ্রুৱ U_LIFETIME_BOUND { ঘূৰাই territory; }
    ধ্রুৱ CharStringMap& variantMap() ধ্রুৱ U_LIFETIME_BOUND { ঘূৰাই variant; }
    ধ্রুৱ CharStringMap& subdivisionMap() ধ্রুৱ U_LIFETIME_BOUND { ঘূৰাই subdivision; }

    স্থির শূন্য U_CALLCONV loadData(UErrorCode &status);
    স্থির UBool U_CALLCONV cleanup();

    স্থির UInitOnce gInitOnce;

গুপ্ত:
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

    স্থির ধ্রুৱ AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    friend শ্ৰেণী AliasDataBuilder;
};


ধ্রুৱ AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    ঘূৰাই সঁচা;
}

শূন্য
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        শূন্য (*checkType)(ধ্রুৱ ৱর্ণ* type),
        শূন্য (*checkReplacement)(ধ্রুৱ UChar* replacement),
        UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
    length = ures_getSize(alias);
    ধ্রুৱ ৱর্ণ** rawTypes = types.allocateInsteadAndCopy(length);
    যদি (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    যদি (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }
    বাবে (পূর্ণাঙ্ক i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        ধ্রুৱ ৱর্ণ* aliasFrom = ures_get_চাবি(res.getAlias());
        ধ্রুৱ UChar* aliasTo =
            ures_get_String_By_চাবি(res.getAlias(), "replacement", nullptr, &status);
        যদি (U_FAILURE(status)) ঘূৰাই;

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
AliasDataBuilder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ঘূৰাই readAlias(
        alias, strings, types, replacementIndexes, length,
#যদি U_DEBUG
        [](ধ্রুৱ ৱর্ণ* type) {
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
        [](ধ্রুৱ ৱর্ণ*) {},
#endif
        [](ধ্রুৱ UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
শূন্য
AliasDataBuilder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ঘূৰাই readAlias(
        alias, strings, types, replacementIndexes, length,
#যদি U_DEBUG
        [](ধ্রুৱ ৱর্ণ* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ধ্রুৱ UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#অন্যথা
        [](ধ্রুৱ ৱর্ণ*) {},
        [](ধ্রুৱ UChar*) { },
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
AliasDataBuilder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ঘূৰাই readAlias(
        alias, strings, types, replacementIndexes, length,
#যদি U_DEBUG
        [](ধ্রুৱ ৱর্ণ* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#অন্যথা
        [](ধ্রুৱ ৱর্ণ*) {},
#endif
        [](ধ্রুৱ UChar*) { },
        status);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
শূন্য
AliasDataBuilder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ঘূৰাই readAlias(
        alias, strings, types, replacementIndexes, length,
#যদি U_DEBUG
        [](ধ্রুৱ ৱর্ণ* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](ধ্রুৱ UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#অন্যথা
        [](ধ্রুৱ ৱর্ণ*) {},
        [](ধ্রুৱ UChar*) { },
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
AliasDataBuilder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ধ্রুৱ ৱর্ণ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ঘূৰাই readAlias(
        alias, strings, types, replacementIndexes, length,
#যদি U_DEBUG
        [](ধ্রুৱ ৱর্ণ* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#অন্যথা
        [](ধ্রুৱ ৱর্ণ*) {},
#endif
        [](ধ্রুৱ UChar*) { },
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
    যদি (U_FAILURE(status)) { ঘূৰাই nullptr; }

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
        ঘূৰাই nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<ধ্রুৱ ৱর্ণ*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<ধ্রুৱ ৱর্ণ*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<ধ্রুৱ ৱর্ণ*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<ধ্রুৱ ৱর্ণ*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<ধ্রুৱ ৱর্ণ*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    যদি (U_FAILURE(status)) {
        ঘূৰাই nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    বাবে (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    বাবে (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    বাবে (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    বাবে (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    বাবে (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    যদি (U_FAILURE(status)) {
        ঘূৰাই nullptr;
    }

    // copy hashtables
    auto *data = নূতন AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    যদি (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ঘূৰাই data;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
শ্ৰেণী AliasReplacer {
সার্ৱজনিক:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       ঘূৰাই 0==uprv_strcmp((ধ্রুৱ ৱর্ণ*)e1.pointer,
                                             (ধ্রুৱ ৱর্ণ*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    তার্কিক replace(
        ধ্রুৱ Locale& locale, CharString& out, UErrorCode& status);

গুপ্ত:
    ধ্রুৱ ৱর্ণ* language;
    ধ্রুৱ ৱর্ণ* script;
    ধ্রুৱ ৱর্ণ* region;
    ধ্রুৱ ৱর্ণ* extensions;
    UVector variants;

    ধ্রুৱ AliasData* data;

    অন্তঃস্থ তার্কিক notEmpty(ধ্রুৱ ৱর্ণ* str) {
        ঘূৰাই str && str[0] != NULL_CHAR;
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
    অন্তঃস্থ ধ্রুৱ ৱর্ণ* deleteOrReplace(ধ্রুৱ ৱর্ণ* input U_LIFETIME_BOUND,
                                       ধ্রুৱ ৱর্ণ* type,
                                       ধ্রুৱ ৱর্ণ* replacement U_LIFETIME_BOUND) {
        ঘূৰাই notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    অন্তঃস্থ তার্কিক same(ধ্রুৱ ৱর্ণ* a, ধ্রুৱ ৱর্ণ* b) {
        যদি (a == nullptr && b == nullptr) {
            ঘূৰাই সঁচা;
        }
        যদি ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          ঘূৰাই মিছা;
        }
        ঘূৰাই uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_চাবি(ধ্রুৱ ৱর্ণ* language, ধ্রুৱ ৱর্ণ* region,
                            ধ্রুৱ ৱর্ণ* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    শূন্য parseLanguageReplacement(ধ্রুৱ ৱর্ণ* replacement,
                                  ধ্রুৱ ৱর্ণ*& replaceLanguage,
                                  ধ্রুৱ ৱর্ণ*& replaceScript,
                                  ধ্রুৱ ৱর্ণ*& replaceRegion,
                                  ধ্রুৱ ৱর্ণ*& replaceVariant,
                                  ধ্রুৱ ৱর্ণ*& replaceExtensions,
                                  UVector& toBeFreed,
                                  UErrorCode& status);

    // Replace by using languageAlias.
    তার্কিক replaceLanguage(তার্কিক checkLanguage, তার্কিক checkRegion,
                         তার্কিক checkVariants, UVector& toBeFreed,
                         UErrorCode& status);

    // Replace by using territoryAlias.
    তার্কিক replaceTerritory(UVector& toBeFreed, UErrorCode& status);

    // Replace by using scriptAlias.
    তার্কিক replaceScript(UErrorCode& status);

    // Replace by using variantAlias.
    তার্কিক replaceVariant(UErrorCode& status);

    // Replace by using subdivisionAlias.
    তার্কিক replaceSubdivision(StringPiece subdivision,
                            CharString& output, UErrorCode& status);

    // Replace transformed extensions.
    তার্কিক replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UErrorCode& status);
};

CharString&
AliasReplacer::generate_চাবি(
        ধ্রুৱ ৱর্ণ* language, ধ্রুৱ ৱর্ণ* region, ধ্রুৱ ৱর্ণ* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    যদি (U_FAILURE(status)) { ঘূৰাই out; }
    out.append(language, status);
    যদি (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    যদি (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    ঘূৰাই out;
}

শূন্য
AliasReplacer::parseLanguageReplacement(
    ধ্রুৱ ৱর্ণ* replacement,
    ধ্রুৱ ৱর্ণ*& replacedLanguage,
    ধ্রুৱ ৱর্ণ*& replacedScript,
    ধ্রুৱ ৱর্ণ*& replacedRegion,
    ধ্রুৱ ৱর্ণ*& replacedVariant,
    ধ্রুৱ ৱর্ণ*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    যদি (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        ঘূৰাই;
    }
    // We have multiple field so we have to allocate and parse
    auto str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    ৱর্ণ* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
    replacedLanguage = (ধ্রুৱ ৱর্ণ*) data;
    ৱর্ণ* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    ধ্রুৱ ৱর্ণ* start = endOfField;
    endOfField = const_cast<ৱর্ণ*>(uprv_strchr(start, '_'));
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
            ঘূৰাই;
        }
        start = endOfField++;
        endOfField = const_cast<ৱর্ণ*>(uprv_strchr(start, '_'));
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
            ঘূৰাই;
        }
        start = endOfField++;
        endOfField = const_cast<ৱর্ণ*>(uprv_strchr(start, '_'));
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
            ঘূৰাই;
        }
        start = endOfField++;
    }
    replacedExtensions = start;
}

তার্কিক
AliasReplacer::replaceLanguage(
        তার্কিক checkLanguage, তার্কিক checkRegion,
        তার্কিক checkVariants, UVector& toBeFreed, UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    যদি (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        ঘূৰাই মিছা;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ধ্রুৱ ৱর্ণ* অনুসন্ধান_Language = checkLanguage ? language : "und";
    ধ্রুৱ ৱর্ণ* অনুসন্ধান_Region = checkRegion ? region : nullptr;
    ধ্রুৱ ৱর্ণ* অনুসন্ধান_Variant = nullptr;
    বাবে (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        যদি (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            অনুসন্ধান_Variant = static_cast<ধ্রুৱ ৱর্ণ*>(variants.elementAt(variant_index));
        }

        যদি (অনুসন্ধান_Variant != nullptr && uprv_strlen(অনুসন্ধান_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            অনুসন্ধান_Variant = nullptr;
        }
        CharString type_চাবি;
        generate_চাবি(অনুসন্ধান_Language, অনুসন্ধান_Region, অনুসন্ধান_Variant, type_চাবি,
                    status);
        যদি (U_FAILURE(status)) {
            ঘূৰাই মিছা;
        }
        ধ্রুৱ ৱর্ণ *replacement = data->languageMap().get(type_চাবি.data());
        যদি (replacement == nullptr) {
            // Found no replacement data.
            অনুৱর্ত;
        }

        ধ্রুৱ ৱর্ণ* replacedLanguage = nullptr;
        ধ্রুৱ ৱর্ণ* replacedScript = nullptr;
        ধ্রুৱ ৱর্ণ* replacedRegion = nullptr;
        ধ্রুৱ ৱর্ণ* replacedVariant = nullptr;
        ধ্রুৱ ৱর্ণ* replacedExtensions = nullptr;
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
            অনুৱর্ত;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        যদি (অনুসন্ধান_Variant != nullptr) {
            যদি (notEmpty(replacedVariant)) {
                variants.setElementAt((শূন্য*)replacedVariant, variant_index);
            } অন্যথা {
                variants.removeElementAt(variant_index);
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
        ঘূৰাই সঁচা;
    }
    // Nothing changed by language alias data.
    ঘূৰাই মিছা;
}

তার্কিক
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    যদি (region == nullptr) {
        // No region to search.
        ঘূৰাই মিছা;
    }
    ধ্রুৱ ৱর্ণ *replacement = data->territoryMap().get(region);
    যদি (replacement == nullptr) {
        // Found no replacement data for this region.
        ঘূৰাই মিছা;
    }
    ধ্রুৱ ৱর্ণ* replacedRegion = replacement;
    ধ্রুৱ ৱর্ণ* firstSpace = uprv_strchr(replacement, ' ');
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
        ধ্রুৱ ৱর্ণ* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        যদি (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ধ্রুৱ ৱর্ণ* foundInReplacement = uprv_strstr(replacement,
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
        যদি (U_FAILURE(status)) { ঘূৰাই মিছা; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        যদি (U_FAILURE(status)) { ঘূৰাই মিছা; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    ঘূৰাই সঁচা;
}

তার্কিক
AliasReplacer::replaceScript(UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    যদি (script == nullptr) {
        // No script to search.
        ঘূৰাই মিছা;
    }
    ধ্রুৱ ৱর্ণ *replacement = data->scriptMap().get(script);
    যদি (replacement == nullptr) {
        // Found no replacement data for this script.
        ঘূৰাই মিছা;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    ঘূৰাই সঁচা;
}

তার্কিক
AliasReplacer::replaceVariant(UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    // Since we may have more than one variant, we need to loop through them.
    বাবে (int32_t i = 0; i < variants.size(); i++) {
        ধ্রুৱ ৱর্ণ* variant = static_cast<ধ্রুৱ ৱর্ণ*>(variants.elementAt(i));
        ধ্রুৱ ৱর্ণ *replacement = data->variantMap().get(variant);
        যদি (replacement == nullptr) {
            // Found no replacement data for this variant.
            অনুৱর্ত;
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
                বাবে (int32_t j = 0; j < variants.size(); j++) {
                     যদি (uprv_strcmp((ধ্রুৱ ৱর্ণ*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            ঘূৰাই সঁচা;
        }
    }
    ঘূৰাই মিছা;
}

তার্কিক
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ ৱর্ণ *replacement = data->subdivisionMap().get(subdivision.data());
    যদি (replacement != nullptr) {
        ধ্রুৱ ৱর্ণ* firstSpace = uprv_strchr(replacement, ' ');
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
        ঘূৰাই সঁচা;
    }
    ঘূৰাই মিছা;
}

তার্কিক
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    int32_t len = transformedExtensions.length();
    ধ্রুৱ ৱর্ণ* str = transformedExtensions.data();
    ধ্রুৱ ৱর্ণ* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    যদি (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        যদি (U_FAILURE(status)) {
            ঘূৰাই মিছা;
        }
        T_CString_toLowerCase(output.data());
    }
    যদি (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        যদি (U_FAILURE(status)) {
            ঘূৰাই মিছা;
        }
        কৰা {
            ধ্রুৱ ৱর্ণ* tvalue = uprv_strchr(tkey, '-');
            যদি (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ঘূৰাই মিছা;
            }
            ধ্রুৱ ৱর্ণ* nextTKey = ultag_getTKeyStart(tvalue);
            যদি (nextTKey != nullptr) {
                *const_cast<ৱর্ণ*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.ঢোকাও_Element_At((শূন্য*)tkey, tfields.size(), status);
            যদি (U_FAILURE(status)) {
                ঘূৰাই মিছা;
            }
            tkey = nextTKey;
        } যেতিয়ালৈ (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            ঘূৰাই uprv_strcmp((ধ্রুৱ ৱর্ণ*)e1.pointer, (ধ্রুৱ ৱর্ণ*)e2.pointer);
        }, status);
        বাবে (int32_t i = 0; i < tfields.size(); i++) {
             যদি (output.length() > 0) {
                 output.append('-', status);
             }
             ধ্রুৱ ৱর্ণ* tfield = static_cast<ধ্রুৱ ৱর্ণ*>(tfields.elementAt(i));
             ধ্রুৱ ৱর্ণ* tvalue = uprv_strchr(tfield, '-');
             যদি (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 ঘূৰাই মিছা;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<ৱর্ণ*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    ঘূৰাই সঁচা;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    যদি (U_FAILURE(status)) { ঘূৰাই out; }
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
            ঘূৰাই uprv_strcmp((ধ্রুৱ ৱর্ণ*)e1.pointer, (ধ্রুৱ ৱর্ণ*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        বাবে (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<ধ্রুৱ ৱর্ণ*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    যদি (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    ঘূৰাই out;
}

তার্কিক
AliasReplacer::replace(ধ্রুৱ Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    U_ASSERT(data != nullptr);
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
    ধ্রুৱ ৱর্ণ* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    যদি (!variantsBuff.isEmpty()) {
        যদি (U_FAILURE(status)) { ঘূৰাই মিছা; }
        ৱর্ণ* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        ৱর্ণ* end;
        যেতিয়ালৈ ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
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
    যদি (U_FAILURE(status)) { ঘূৰাই মিছা; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        ঘূৰাই uprv_strcmp((ধ্রুৱ ৱর্ণ*)e1.pointer, (ধ্রুৱ ৱর্ণ*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    ধ্রুৱ int32_t maxChanged = 4 + variants.size();
    (শূন্য)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](শূন্য *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    যেতিয়ালৈ (U_SUCCESS(status)) {
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
        যদি (    replaceLanguage(সঁচা, সঁচা,  সঁচা,  stringsToBeFreed, status) ||
                replaceLanguage(সঁচা, সঁচা,  মিছা, stringsToBeFreed, status) ||
                replaceLanguage(সঁচা, মিছা, সঁচা,  stringsToBeFreed, status) ||
                replaceLanguage(সঁচা, মিছা, মিছা, stringsToBeFreed, status) ||
                replaceLanguage(মিছা,মিছা, সঁচা,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            অনুৱর্ত;
        }
        // Nothing changed. Break out.
        ভঙ্গ;
    }  // while(1)

    যদি (U_FAILURE(status)) { ঘূৰাই মিছা; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ধ্রুৱ ৱর্ণ* extensionsStr = locale_getKeywordsStart(locale.getName());
    যদি (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        ঘূৰাই মিছা;
    }
    outputToString(out, status);
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    যদি (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        যদি (U_SUCCESS(status) && !iter.isNull()) {
            ধ্রুৱ ৱর্ণ* চাবি;
            যেতিয়ালৈ ((চাবি = iter->next(nullptr, status)) != nullptr) {
                যদি (uprv_strcmp("sd", চাবি) == 0 || uprv_strcmp("rg", চাবি) == 0 ||
                        uprv_strcmp("t", চাবি) == 0) {
                    auto value = locale.get_Keyword_মান<CharString>(চাবি, status);
                    যদি (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        অনুৱর্ত;
                    }
                    CharString replacement;
                    যদি (uprv_strlen(চাবি) == 2) {
                        যদি (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.set_Keyword_মান(চাবি, replacement.data(), status);
                        }
                    } অন্যথা {
                        U_ASSERT(uprv_strcmp(চাবি, "t") == 0);
                        যদি (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.set_Keyword_মান(চাবি, replacement.data(), status);
                        }
                    }
                    যদি (U_FAILURE(status)) {
                        ঘূৰাই মিছা;
                    }
                }
            }
        }
        যদি (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    // If the tag is not changed, return.
    যদি (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        ঘূৰাই মিছা;
    }
    ঘূৰাই সঁচা;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
তার্কিক
canonicalizeLocale(ধ্রুৱ Locale& locale, CharString& out, UErrorCode& status)
{
    যদি (U_FAILURE(status)) { ঘূৰাই মিছা; }
    AliasReplacer replacer(status);
    ঘূৰাই replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
তার্কিক
isKnownCanonicalizedLocale(ধ্রুৱ ৱর্ণ* locale, UErrorCode& status)
{
    যদি (U_FAILURE(status)) { ঘূৰাই মিছা; }

    যদি (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        ঘূৰাই সঁচা;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    ঘূৰাই uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT ধ্রুৱ ৱর্ণ* ধ্রুৱ*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    ঘূৰাই KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT তার্কিক
ulocimp_isCanonicalizedLocaleForTest(ধ্রুৱ ৱর্ণ* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    ঘূৰাই !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ধ্রুৱ ৱর্ণ* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    ঘূৰাই localeID == nullptr ? *এতত্ = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    কৰা {
        ৱর্ণ *separator;
        ৱর্ণ *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        ধ্রুৱ auto parse = [canonicalize](std::string_view localeID,
                                          ৱর্ণ* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            ঘূৰাই ByteSinkUtil::viaByteSinkToTerminatedChars(
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
        ৱর্ণ* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, পরিমাণ Nest::baseName, err);

        FixedString fullNameBuffer;
        যদি (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(পরিমাণ Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            যদি (!fullNameBuffer.reserve(length + 1)) {
                ভঙ্গ; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        যদি(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            ভঙ্গ;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        ৱর্ণ* at = uprv_strchr(fullName, '@');
        যেতিয়ালৈ ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        ৱর্ণ* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        যদি (separator!=nullptr || sep2!=nullptr) {
            যদি (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } অন্যথা {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        তার্কিক hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        যদি (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            ভঙ্গ; // error: the language field is too long
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
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } অন্যথা {
            যদি (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                যদি (fullNameBuffer.isEmpty()) {
                    ভঙ্গ; // error: out of memory
                }
                যদি (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                যদি (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                যদি (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            যদি (isBogus()) {
                ভঙ্গ; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            যদি (hasKeywords) {
                যদি (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    যদি (heap.ptr->baseName.isEmpty()) {
                        ভঙ্গ; // error: out of memory
                    }
                }
            }
        }

        যদি (canonicalize) {
            যদি (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                যদি (canonicalizeLocale(*এতত্, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), মিছা);
                }
                যদি (U_FAILURE(err)) {
                    ভঙ্গ;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        ঘূৰাই *এতত্;
    } যেতিয়ালৈ(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    ঘূৰাই *এতত্;
}

int32_t
Locale::hashCode() ধ্রুৱ
{
    ঘূৰাই ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

শূন্য
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

ধ্রুৱ Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        যদি (gDefaultLocale != nullptr) {
            ঘূৰাই *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    ঘূৰাই *locale_set_default_internal(nullptr, status);
}



শূন্য U_EXPORT2
Locale::setDefault( ধ্রুৱ   Locale&     newLocale,
                            UErrorCode&  status)
{
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ধ্রুৱ ৱর্ণ *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

শূন্য
Locale::addLikelySubtags(UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    যদি (U_FAILURE(status)) {
        যদি (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ঘূৰাই;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/মিছা);
    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

শূন্য
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(মিছা, status);
}
শূন্য
Locale::minimizeSubtags(তার্কিক favorScript, UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    যদি (U_FAILURE(status)) {
        যদি (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ঘূৰাই;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/মিছা);
    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

শূন্য
Locale::canonicalize(UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }
    CharString uncanonicalized(getName(), status);
    যদি (U_FAILURE(status)) {
        যদি (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ঘূৰাই;
    }
    init(uncanonicalized.data(), /*canonicalize=*/সঁচা);
    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    যদি (U_FAILURE(status)) {
        ঘূৰাই result;
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

    যদি (U_FAILURE(status)) {
        ঘূৰাই result;
    }

    যদি (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই result;
    }

    result.init(localeID.data(), /*canonicalize=*/মিছা);
    যদি (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ঘূৰাই result;
}

শূন্য
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) ধ্রুৱ
{
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }

    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/মিছা, status);
}

Locale U_EXPORT2
Locale::createFromName (ধ্রুৱ ৱর্ণ *name)
{
    যদি (name) {
        Locale l("");
        l.init(name, মিছা);
        ঘূৰাই l;
    }
    অন্যথা {
        ঘূৰাই getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, মিছা);
    ঘূৰাই loc;
}

Locale U_EXPORT2
Locale::createCanonical(ধ্রুৱ ৱর্ণ* name) {
    Locale loc("");
    loc.init(name, সঁচা);
    ঘূৰাই loc;
}

ধ্রুৱ ৱর্ণ *
Locale::getISO3Language() ধ্রুৱ
{
    ঘূৰাই uloc_getISO3Language(getName());
}


ধ্রুৱ ৱর্ণ *
Locale::getISO3Country() ধ্রুৱ
{
    ঘূৰাই uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() ধ্রুৱ
{
    ঘূৰাই uloc_getLCID(getName());
}

ধ্রুৱ ৱর্ণ* ধ্রুৱ* U_EXPORT2 Locale::getISOCountries()
{
    ঘূৰাই uloc_getISOCountries();
}

ধ্রুৱ ৱর্ণ* ধ্রুৱ* U_EXPORT2 Locale::getISOLanguages()
{
    ঘূৰাই uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
শূন্য Locale::setFromPOSIXID(ধ্রুৱ ৱর্ণ *posixID)
{
    init(posixID, সঁচা);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::get_মূল()
{
    ঘূৰাই getLocale(e_মূল);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getEnglish()
{
    ঘূৰাই getLocale(eENGLISH);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getFrench()
{
    ঘূৰাই getLocale(eFRENCH);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getGerman()
{
    ঘূৰাই getLocale(eGERMAN);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getItalian()
{
    ঘূৰাই getLocale(eITALIAN);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getJapanese()
{
    ঘূৰাই getLocale(eJAPANESE);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getKorean()
{
    ঘূৰাই getLocale(eKOREAN);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getChinese()
{
    ঘূৰাই getLocale(eCHINESE);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    ঘূৰাই getLocale(eCHINA);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    ঘূৰাই getLocale(eTAIWAN);
}


ধ্রুৱ Locale & U_EXPORT2
Locale::getFrance()
{
    ঘূৰাই getLocale(eFRANCE);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getGermany()
{
    ঘূৰাই getLocale(eGERMANY);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getItaly()
{
    ঘূৰাই getLocale(eITALY);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getJapan()
{
    ঘূৰাই getLocale(eJAPAN);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getKorea()
{
    ঘূৰাই getLocale(eKOREA);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getChina()
{
    ঘূৰাই getLocale(eCHINA);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getPRC()
{
    ঘূৰাই getLocale(eCHINA);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getTaiwan()
{
    ঘূৰাই getLocale(eTAIWAN);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getUK()
{
    ঘূৰাই getLocale(eUK);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getUS()
{
    ঘূৰাই getLocale(eUS);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getCanada()
{
    ঘূৰাই getLocale(eCANADA);
}

ধ্রুৱ Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    ঘূৰাই getLocale(eCANADA_FRENCH);
}

ধ্রুৱ Locale &
Locale::getLocale(পূর্ণাঙ্ক locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    যদি (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    ঘূৰাই localeCache[locid]; /*operating on nullptr*/
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
    ঘূৰাই gLocaleCache;
}

শ্ৰেণী KeywordEnumeration : সার্ৱজনিক StringEnumeration {
রক্ষিত:
    FixedString keywords;
গুপ্ত:
    int32_t length;
    ধ্রুৱ ৱর্ণ *current;
    স্থির ধ্রুৱ ৱর্ণ fgClassID;

সার্ৱজনিক:
    স্থির UClassID U_EXPORT2 getStaticClassID() { ঘূৰাই (UClassID)&fgClassID; }
    আভাস UClassID getDynamicClassID() ধ্রুৱ override { ঘূৰাই getStaticClassID(); }
সার্ৱজনিক:
    KeywordEnumeration(ধ্রুৱ ৱর্ণ *চাবিগুলি, int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        যদি(U_SUCCESS(status) && keywordLen != 0) {
            যদি(চাবিগুলি == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } অন্যথা {
                keywords = {চাবিগুলি, static_cast<std::string_view::size_type>(length)};
                যদি (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } অন্যথা {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    আভাস ~KeywordEnumeration();

    আভাস StringEnumeration * clone() ধ্রুৱ override
    {
        UErrorCode status = U_ZERO_ERROR;
        ঘূৰাই নূতন KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    আভাস int32_t count(UErrorCode& status) ধ্রুৱ override {
        যদি (U_FAILURE(status)) { ঘূৰাই 0; }
        ধ্রুৱ ৱর্ণ *kw = keywords.data();
        int32_t result = 0;
        যেতিয়ালৈ(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        ঘূৰাই result;
    }

    আভাস ধ্রুৱ ৱর্ণ* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ধ্রুৱ ৱর্ণ* result;
        int32_t len;
        যদি(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            যদি(resultLength != nullptr) {
                *resultLength = len;
            }
        } অন্যথা {
            যদি(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        ঘূৰাই result;
    }

    আভাস ধ্রুৱ UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        যদি (U_FAILURE(status)) { ঘূৰাই nullptr; }
        int32_t resultLength = 0;
        ধ্রুৱ ৱর্ণ *s = next(&resultLength, status);
        ঘূৰাই setChars(s, resultLength, status);
    }

    আভাস শূন্য reset(UErrorCode& status) override {
        যদি (U_FAILURE(status)) { ঘূৰাই; }
        current = keywords.data();
    }
};

ধ্রুৱ ৱর্ণ KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = স্ৱভাৱ;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
শ্ৰেণী UnicodeKeywordEnumeration : সার্ৱজনিক KeywordEnumeration {
সার্ৱজনিক:
    using KeywordEnumeration::KeywordEnumeration;
    আভাস ~UnicodeKeywordEnumeration();

    আভাস ধ্রুৱ ৱর্ণ* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ধ্রুৱ ৱর্ণ* legacy_চাবি = KeywordEnumeration::next(nullptr, status);
        যেতিয়ালৈ (U_SUCCESS(status) && legacy_চাবি != nullptr) {
            ধ্রুৱ ৱর্ণ* চাবি = uloc_to_Unicode_Locale_চাবি(legacy_চাবি);
            যদি (চাবি != nullptr) {
                যদি (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(চাবি));
                }
                ঘূৰাই চাবি;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_চাবি = KeywordEnumeration::next(nullptr, status);
        }
        যদি (resultLength != nullptr) *resultLength = 0;
        ঘূৰাই nullptr;
    }
    আভাস int32_t count(UErrorCode& status) ধ্রুৱ override {
        যদি (U_FAILURE(status)) { ঘূৰাই 0; }
        ধ্রুৱ ৱর্ণ *kw = keywords.data();
        int32_t result = 0;
        যেতিয়ালৈ(*kw) {
            যদি (uloc_to_Unicode_Locale_চাবি(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        ঘূৰাই result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = স্ৱভাৱ;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) ধ্রুৱ
{
    StringEnumeration *result = nullptr;

    যদি (U_FAILURE(status)) {
        ঘূৰাই result;
    }

    ধ্রুৱ ৱর্ণ* variantStart = uprv_strchr(getName(), '@');
    ধ্রুৱ ৱর্ণ* assignment = uprv_strchr(getName(), '=');
    যদি(variantStart) {
        যদি(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', মিছা, status);
            যদি (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = নূতন KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                যদি (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } অন্যথা {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ঘূৰাই result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) ধ্রুৱ
{
    StringEnumeration *result = nullptr;

    যদি (U_FAILURE(status)) {
        ঘূৰাই result;
    }

    ধ্রুৱ ৱর্ণ* variantStart = uprv_strchr(getName(), '@');
    ধ্রুৱ ৱর্ণ* assignment = uprv_strchr(getName(), '=');
    যদি(variantStart) {
        যদি(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', মিছা, status);
            যদি (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = নূতন UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                যদি (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } অন্যথা {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ঘূৰাই result;
}

int32_t
Locale::get_Keyword_মান(ধ্রুৱ ৱর্ণ* keywordName, ৱর্ণ *buffer, int32_t bufLen, UErrorCode &status) ধ্রুৱ
{
    ঘূৰাই uloc_get_Keyword_মান(getName(), keywordName, buffer, bufLen, &status);
}

শূন্য
Locale::get_Keyword_মান(StringPiece keywordName, ByteSink& sink, UErrorCode& status) ধ্রুৱ {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }

    যদি (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    ulocimp_get_Keyword_মান(getName(), keywordName, sink, status);
}

শূন্য
Locale::get_Unicode_Keyword_মান(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) ধ্রুৱ {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }

    std::optional<std::string_view> legacy_চাবি = ulocimp_to_Legacy_চাবি_With_Fallback(keywordName);
    যদি (!legacy_চাবি.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    auto legacy_মান = get_Keyword_মান<CharString>(*legacy_চাবি, status);

    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }

    std::optional<std::string_view> unicode_মান =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_মান.toStringPiece());
    যদি (!unicode_মান.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    sink.Append(unicode_মান->data(), static_cast<int32_t>(unicode_মান->size()));
}

শূন্য
Locale::set_Keyword_মান(StringPiece keywordName,
                        StringPiece keyword_মান,
                        UErrorCode& status) {
    যদি (U_FAILURE(status)) { ঘূৰাই; }
    যদি (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
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
        ঘূৰাই;
    }

    ধ্রুৱ ৱর্ণ* at = locale_getKeywordsStart(localeID.toStringPiece());
    তার্কিক hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

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
                U_ASSERT(static_cast<size_t>(variantBegin) < পরিমাণ Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, পরিমাণ Nest::baseName, status);
            } অন্যথা {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                যদি (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ঘূৰাই;
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
                ঘূৰাই;
            }
        } অন্যথা {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        যদি (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            ঘূৰাই;
        }

        যদি (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            যদি (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                যদি (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ঘূৰাই;
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
        ঘূৰাই;
    }

    std::optional<std::string_view> legacy_চাবি = ulocimp_to_Legacy_চাবি_With_Fallback(keywordName);
    যদি (!legacy_চাবি.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    std::string_view value;

    যদি (!keyword_মান.empty()) {
        std::optional<std::string_view> legacy_মান =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_মান);
        যদি (!legacy_মান.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই;
        }
        value = *legacy_মান;
    }

    set_Keyword_মান(*legacy_চাবি, value, status);
}

ধ্রুৱ ৱর্ণ*
Locale::getCountry() ধ্রুৱ U_LIFETIME_BOUND {
    ঘূৰাই getField<&Nest::getRegion, &Heap::getRegion>();
}

ধ্রুৱ ৱর্ণ*
Locale::getLanguage() ধ্রুৱ U_LIFETIME_BOUND {
    ঘূৰাই getField<&Nest::getLanguage, &Heap::getLanguage>();
}

ধ্রুৱ ৱর্ণ*
Locale::getScript() ধ্রুৱ U_LIFETIME_BOUND {
    ঘূৰাই getField<&Nest::getScript, &Heap::getScript>();
}

ধ্রুৱ ৱর্ণ*
Locale::getVariant() ধ্রুৱ U_LIFETIME_BOUND {
    ঘূৰাই getField<&Nest::getVariant, &Heap::getVariant>();
}

ধ্রুৱ ৱর্ণ*
Locale::getName() ধ্রুৱ U_LIFETIME_BOUND {
    ঘূৰাই getField<&Nest::getBaseName, &Heap::getFullName>();
}

ধ্রুৱ ৱর্ণ*
Locale::getBaseName() ধ্রুৱ U_LIFETIME_BOUND {
    ঘূৰাই getField<&Nest::getBaseName, &Heap::getBaseName>();
}

template <ধ্রুৱ ৱর্ণ* (Locale::Nest::*ধ্রুৱ NEST)() ধ্রুৱ,
          ধ্রুৱ ৱর্ণ* (Locale::Heap::*ধ্রুৱ HEAP)() ধ্রুৱ>
ধ্রুৱ ৱর্ণ* Locale::getField() ধ্রুৱ U_LIFETIME_BOUND {
    ঘূৰাই payload.visit([] { ঘূৰাই ""; },
                         [](ধ্রুৱ Nest& nest) { ঘূৰাই (nest.*NEST)(); },
                         [](ধ্রুৱ Heap& heap) { ঘূৰাই (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = স্ৱভাৱ;

//eof
U_NAMESPACE_END
