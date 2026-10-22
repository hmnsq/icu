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

#ಅನ್ತರ್ಭಾವ <cstddef>
#ಅನ್ತರ್ಭಾವ <optional>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <type_traits>
#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/localebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಅನ್ತರ್ಭಾವ "unicode/strenum.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"

#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "charstrmap.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "fixedstring.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uniquecharstr.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

#ಯದಿ UPRV_HAS_FEATURE(memory_sanitizer)
#ಅನ್ತರ್ಭಾವ <sanitizer/msan_interface.h>
#endif

U_NAMESPACE_BEGIN

ಸ್ಥಿರ Locale   *gLocaleCache = nullptr;
ಸ್ಥಿರ UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
ಸ್ಥಿರ UMutex gDefaultLocaleMutex;
ಸ್ಥಿರ UHashtable *gDefaultLocalesHashT = nullptr;
ಸ್ಥಿರ Locale *gDefaultLocale = nullptr;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#define ULOC_STRING_LIMIT 357913941

U_NAMESPACE_END

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ELocalePos {
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
    e_ಬೇರು,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

ನಾಮಾಕಾಶ {

//
// Deleter function for Locales owned by the default Locale hash table/
//
ಶೂನ್ಯ U_CALLCONV
deleteLocale(ಶೂನ್ಯ *obj) {
    delete static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    delete [] gLocaleCache;
    gLocaleCache = nullptr;
    gLocaleCacheInitOnce.reset();

    ಯದಿ (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = ನೂತನ Locale[static_cast<ಪೂರ್ಣಾಙ್ಕ>(eMAX_LOCALES)];
    ಯದಿ (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_ಬೇರು]          = Locale("");
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

Locale *locale_set_default_internal(ಧ್ರುವ ವರ್ಣ *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = ಅಸತ್ಯ;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    ಯದಿ (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = ಸತ್ಯ; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ gDefaultLocale;
    }

    ಯದಿ (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ gDefaultLocale;
        }
        uhash_set_ಮೌಲ್ಯ_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    ಯದಿ (newDefault == nullptr) {
        newDefault = ನೂತನ Locale(Locale::eBOGUS);
        ಯದಿ (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), ಅಸತ್ಯ);
        uhash_put(gDefaultLocalesHashT, const_cast<ವರ್ಣ*>(newDefault->getName()), newDefault, &status);
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    ಪ್ರತ್ಯಾಗಮ gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC ಶೂನ್ಯ
locale_set_default(ಧ್ರುವ ವರ್ಣ *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC ಧ್ರುವ ವರ್ಣ *
locale_get_default()
{
    U_NAMESPACE_USE
    ಪ್ರತ್ಯಾಗಮ Locale::getDefault().getName();
}

ನಾಮಾಕಾಶ {

template <auto FIELD, typename T>
ಶೂನ್ಯ copyToArray(std::string_view sv, T* that) {
    auto& field = that->*FIELD;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    ಯದಿ (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.data(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

ಶೂನ್ಯ Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, ಏತತ್);
    copyToArray<&Nest::script>(script, ಏತತ್);
    copyToArray<&Nest::region>(region, ಏತತ್);
    ಏತತ್->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(Heap, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(Heap, region));
    U_ASSERT(ಏತತ್ == reinterpret_cast<Nest*>(&heap));
#ಯದಿ UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, ಪರಿಮಾಣ(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, ಏತತ್);
    copyToArray<&Nest::region>(heap.region, ಏತತ್);
    ಏತತ್->variantBegin = variantBegin;
    *ಏತತ್->baseName = '\0';
}

ಸಂರಚನಾ Locale::Heap::Alloc : ಸಾರ್ವಜನಿಕ UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    ಧ್ರುವ ವರ್ಣ* getVariant() ಧ್ರುವ U_LIFETIME_BOUND {
        ಪ್ರತ್ಯಾಗಮ variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    ಧ್ರುವ ವರ್ಣ* getFullName() ಧ್ರುವ U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ fullName.data(); }
    ಧ್ರುವ ವರ್ಣ* getBaseName() ಧ್ರುವ U_LIFETIME_BOUND {
        ಯದಿ (baseName.isEmpty()) {
            ಯದಿ (ಧ್ರುವ ವರ್ಣ* name = fullName.data(); *name != '@') {
                ಪ್ರತ್ಯಾಗಮ name;
            }
        }
        ಪ್ರತ್ಯಾಗಮ baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(ಧ್ರುವ Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        ಯದಿ (U_SUCCESS(status)) {
            ಯದಿ (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                ಯದಿ (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ಅನ್ಯಥಾ {
                    ಯದಿ (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        ಯದಿ (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = delete;

    ~Alloc() = ಸ್ವಭಾವ;
};

ಧ್ರುವ ವರ್ಣ* Locale::Heap::getVariant() ಧ್ರುವ U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ ptr->getVariant(); }
ಧ್ರುವ ವರ್ಣ* Locale::Heap::getFullName() ಧ್ರುವ U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ ptr->getFullName(); }
ಧ್ರುವ ವರ್ಣ* Locale::Heap::getBaseName() ಧ್ರುವ U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = ನೂತನ Alloc(variantBegin);
    ಯದಿ (ptr == nullptr) {
        type = eBOGUS;
    } ಅನ್ಯಥಾ {
        type = eHEAP;
        copyToArray<&Heap::language>(language, ಏತತ್);
        copyToArray<&Heap::script>(script, ಏತತ್);
        copyToArray<&Heap::region>(region, ಏತತ್);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::operator=(ಧ್ರುವ Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = ನೂತನ Alloc(*other.ptr, status);
    ಯದಿ (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } ಅನ್ಯಥಾ {
        type = eHEAP;
        uprv_memcpy(language, other.language, ಪರಿಮಾಣ language);
        uprv_memcpy(script, other.script, ಪರಿಮಾಣ script);
        uprv_memcpy(region, other.region, ಪರಿಮಾಣ region);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

Locale::Heap& Locale::Heap::operator=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, ಪರಿಮಾಣ language);
    uprv_memcpy(script, other.script, ಪರಿಮಾಣ script);
    uprv_memcpy(region, other.region, ಪರಿಮಾಣ region);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

template <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
auto Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) ಧ್ರುವ {
    ವಿಕಲ್ಪ (type) {
        ಪಕ್ಷ eBOGUS:
            ಪ್ರತ್ಯಾಗಮ bogusFn(args...);
        ಪಕ್ಷ eNEST:
            ಪ್ರತ್ಯಾಗಮ nestFn(nest, args...);
        ಪಕ್ಷ eHEAP:
            ಪ್ರತ್ಯಾಗಮ heapFn(heap, args...);
        ಸ್ವಭಾವ:
            UPRV_UNREACHABLE_EXIT;
    };
}

ಶೂನ್ಯ Locale::Payload::copy(ಧ್ರುವ Payload& other) {
    other.visit([](Payload*) {},
                [](ಧ್ರುವ Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ಧ್ರುವ Heap& heap, Payload* dst) { dst->heap = heap; },
                ಏತತ್);
}

ಶೂನ್ಯ Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](ಧ್ರುವ Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ಧ್ರುವ Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        ಏತತ್);
}

Locale::Payload::~Payload() {
    ಯದಿ (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(ಧ್ರುವ Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::operator=(ಧ್ರುವ Payload& other) U_LIFETIME_BOUND {
    ಯದಿ (ಏತತ್ != &other) {
        setToBogus();
        copy(other);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

Locale::Payload& Locale::Payload::operator=(Payload&& other) noexcept U_LIFETIME_BOUND {
    ಯದಿ (ಏತತ್ != &other) {
        setToBogus();
        move(std::move(other));
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ಶೂನ್ಯ Locale::Payload::setToBogus() {
    ಏತತ್->~Payload();
    type = eBOGUS;
}

template <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    ಯದಿ constexpr (std::is_same_v<T, Nest>) {
        ಏತತ್->~Payload();
        ::ನೂತನ (&nest) Nest(std::forward<Args>(args)...);
        ಪ್ರತ್ಯಾಗಮ nest;
    }
    ಯದಿ constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::ನೂತನ (&heap) Heap(std::forward<Args>(args)...);
        ಪ್ರತ್ಯಾಗಮ heap;
    }
}

template <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ type == eNEST ? &nest : nullptr;
}
template <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = ಸ್ವಭಾವ;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, ಅಸತ್ಯ);
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


Locale::Locale( ಧ್ರುವ   ವರ್ಣ * newLanguage,
                ಧ್ರುವ   ವರ್ಣ * newCountry,
                ಧ್ರುವ   ವರ್ಣ * newVariant,
                ಧ್ರುವ   ವರ್ಣ * newKeywords)
    : UObject(), payload()
{
    ಯದಿ( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, ಅಸತ್ಯ); /* shortcut */
    }
    ಅನ್ಯಥಾ
    {
        UErrorCode status = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        ಯದಿ ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            ಯದಿ ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                ಪ್ರತ್ಯಾಗಮ;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        ಯದಿ ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            ಯದಿ ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                ಪ್ರತ್ಯಾಗಮ;
            }
        }

        // _Variant
        ಯದಿ ( newVariant != nullptr )
        {
            // remove leading _'s
            ಯಾವತ್(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            ಯದಿ ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                ಪ್ರತ್ಯಾಗಮ;
            }
            ಯಾವತ್( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        ಯದಿ ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            ಯದಿ ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              ಪ್ರತ್ಯಾಗಮ;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        ಯದಿ ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        ಯದಿ ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        ಯದಿ ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        ಯದಿ ( ksize != 0)
        {
            ಯದಿ (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            ಅನ್ಯಥಾ {
                togo.append('_', status); /* Variant parsing with a script */
                ಯದಿ ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        ಯದಿ (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            ಪ್ರತ್ಯಾಗಮ;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), ಅಸತ್ಯ);
    }
}

Locale::Locale(ಧ್ರುವ Locale&) = ಸ್ವಭಾವ;
Locale::Locale(Locale&&) noexcept = ಸ್ವಭಾವ;

Locale& Locale::operator=(ಧ್ರುವ Locale&) U_LIFETIME_BOUND = ಸ್ವಭಾವ;
Locale& Locale::operator=(Locale&&) noexcept U_LIFETIME_BOUND = ಸ್ವಭಾವ;

Locale *
Locale::clone() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ನೂತನ Locale(*ಏತತ್);
}

ತಾರ್ಕಿಕ
Locale::operator==( ಧ್ರುವ   Locale& other) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ uprv_strcmp(other.getName(), getName()) == 0;
}

ನಾಮಾಕಾಶ {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr ಧ್ರುವ ವರ್ಣ* KNOWN_CANONICALIZED[] = {
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
    ಯದಿ (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    ಪ್ರತಿ (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (ಶೂನ್ಯ*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

ವರ್ಗ AliasData;

/**
 * A Builder class to build the alias data.
 */
ವರ್ಗ AliasDataBuilder {
ಸಾರ್ವಜನಿಕ:
    AliasDataBuilder() {
    }

    // Build the AliasData from resource.
    AliasData* build(UErrorCode &status);

ಗುಪ್ತ:
    ಶೂನ್ಯ readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   ಶೂನ್ಯ (*checkType)(ಧ್ರುವ ವರ್ಣ* type),
                   ಶೂನ್ಯ (*checkReplacement)(ಧ್ರುವ UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ಶೂನ್ಯ readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
                           LocalMemory<int32_t>& replacementIndexes,
                           int32_t &length,
                           UErrorCode &status);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ಶೂನ್ಯ readScriptAlias(UResourceBundle* alias,
                         UniqueCharStrings* strings,
                         LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ಶೂನ್ಯ readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    ಶೂನ್ಯ readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    ಶೂನ್ಯ readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
ವರ್ಗ AliasData : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ಧ್ರುವ AliasData* singleton(UErrorCode& status) {
        ಯದಿ (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        ಪ್ರತ್ಯಾಗಮ gSingleton;
    }

    ಧ್ರುವ CharStringMap& languageMap() ಧ್ರುವ U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ language; }
    ಧ್ರುವ CharStringMap& scriptMap() ಧ್ರುವ U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ script; }
    ಧ್ರುವ CharStringMap& territoryMap() ಧ್ರುವ U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ territory; }
    ಧ್ರುವ CharStringMap& variantMap() ಧ್ರುವ U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ variant; }
    ಧ್ರುವ CharStringMap& subdivisionMap() ಧ್ರುವ U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ subdivision; }

    ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV loadData(UErrorCode &status);
    ಸ್ಥಿರ UBool U_CALLCONV cleanup();

    ಸ್ಥಿರ UInitOnce gInitOnce;

ಗುಪ್ತ:
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

    ಸ್ಥಿರ ಧ್ರುವ AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    friend ವರ್ಗ AliasDataBuilder;
};


ಧ್ರುವ AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        ಶೂನ್ಯ (*checkType)(ಧ್ರುವ ವರ್ಣ* type),
        ಶೂನ್ಯ (*checkReplacement)(ಧ್ರುವ UChar* replacement),
        UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    length = ures_getSize(alias);
    ಧ್ರುವ ವರ್ಣ** rawTypes = types.allocateInsteadAndCopy(length);
    ಯದಿ (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    ಯದಿ (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        ಧ್ರುವ ವರ್ಣ* aliasFrom = ures_get_ಕೀಲಿ(res.getAlias());
        ಧ್ರುವ UChar* aliasTo =
            ures_get_String_By_ಕೀಲಿ(res.getAlias(), "replacement", nullptr, &status);
        ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;

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
ಶೂನ್ಯ
AliasDataBuilder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ಪ್ರತ್ಯಾಗಮ readAlias(
        alias, strings, types, replacementIndexes, length,
#ಯದಿ U_DEBUG
        [](ಧ್ರುವ ವರ್ಣ* type) {
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
#ಅನ್ಯಥಾ
        [](ಧ್ರುವ ವರ್ಣ*) {},
#endif
        [](ಧ್ರುವ UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
ಶೂನ್ಯ
AliasDataBuilder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ಪ್ರತ್ಯಾಗಮ readAlias(
        alias, strings, types, replacementIndexes, length,
#ಯದಿ U_DEBUG
        [](ಧ್ರುವ ವರ್ಣ* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ಧ್ರುವ UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#ಅನ್ಯಥಾ
        [](ಧ್ರುವ ವರ್ಣ*) {},
        [](ಧ್ರುವ UChar*) { },
#endif
        status);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
ಶೂನ್ಯ
AliasDataBuilder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ಪ್ರತ್ಯಾಗಮ readAlias(
        alias, strings, types, replacementIndexes, length,
#ಯದಿ U_DEBUG
        [](ಧ್ರುವ ವರ್ಣ* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#ಅನ್ಯಥಾ
        [](ಧ್ರುವ ವರ್ಣ*) {},
#endif
        [](ಧ್ರುವ UChar*) { },
        status);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
ಶೂನ್ಯ
AliasDataBuilder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ಪ್ರತ್ಯಾಗಮ readAlias(
        alias, strings, types, replacementIndexes, length,
#ಯದಿ U_DEBUG
        [](ಧ್ರುವ ವರ್ಣ* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](ಧ್ರುವ UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#ಅನ್ಯಥಾ
        [](ಧ್ರುವ ವರ್ಣ*) {},
        [](ಧ್ರುವ UChar*) { },
#endif
        status);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
ಶೂನ್ಯ
AliasDataBuilder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ಧ್ರುವ ವರ್ಣ*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ಪ್ರತ್ಯಾಗಮ readAlias(
        alias, strings, types, replacementIndexes, length,
#ಯದಿ U_DEBUG
        [](ಧ್ರುವ ವರ್ಣ* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#ಅನ್ಯಥಾ
        [](ಧ್ರುವ ವರ್ಣ*) {},
#endif
        [](ಧ್ರುವ UChar*) { },
        status);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
ಶೂನ್ಯ U_CALLCONV
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
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_ಕೀಲಿ(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_ಕೀಲಿ(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_ಕೀಲಿ(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_ಕೀಲಿ(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_ಕೀಲಿ(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_ಕೀಲಿ(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<ಧ್ರುವ ವರ್ಣ*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<ಧ್ರುವ ವರ್ಣ*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<ಧ್ರುವ ವರ್ಣ*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<ಧ್ರುವ ವರ್ಣ*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<ಧ್ರುವ ವರ್ಣ*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    ಪ್ರತಿ (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    ಪ್ರತಿ (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    ಪ್ರತಿ (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    ಪ್ರತಿ (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    ಪ್ರತಿ (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // copy hashtables
    auto *data = ನೂತನ AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    ಯದಿ (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ data;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
ವರ್ಗ AliasReplacer {
ಸಾರ್ವಜನಿಕ:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       ಪ್ರತ್ಯಾಗಮ 0==uprv_strcmp((ಧ್ರುವ ವರ್ಣ*)e1.pointer,
                                             (ಧ್ರುವ ವರ್ಣ*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    ತಾರ್ಕಿಕ replace(
        ಧ್ರುವ Locale& locale, CharString& out, UErrorCode& status);

ಗುಪ್ತ:
    ಧ್ರುವ ವರ್ಣ* language;
    ಧ್ರುವ ವರ್ಣ* script;
    ಧ್ರುವ ವರ್ಣ* region;
    ಧ್ರುವ ವರ್ಣ* extensions;
    UVector variants;

    ಧ್ರುವ AliasData* data;

    ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ notEmpty(ಧ್ರುವ ವರ್ಣ* str) {
        ಪ್ರತ್ಯಾಗಮ str && str[0] != NULL_CHAR;
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
    ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* deleteOrReplace(ಧ್ರುವ ವರ್ಣ* input U_LIFETIME_BOUND,
                                       ಧ್ರುವ ವರ್ಣ* type,
                                       ಧ್ರುವ ವರ್ಣ* replacement U_LIFETIME_BOUND) {
        ಪ್ರತ್ಯಾಗಮ notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ same(ಧ್ರುವ ವರ್ಣ* a, ಧ್ರುವ ವರ್ಣ* b) {
        ಯದಿ (a == nullptr && b == nullptr) {
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
        ಯದಿ ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        ಪ್ರತ್ಯಾಗಮ uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_ಕೀಲಿ(ಧ್ರುವ ವರ್ಣ* language, ಧ್ರುವ ವರ್ಣ* region,
                            ಧ್ರುವ ವರ್ಣ* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    ಶೂನ್ಯ parseLanguageReplacement(ಧ್ರುವ ವರ್ಣ* replacement,
                                  ಧ್ರುವ ವರ್ಣ*& replaceLanguage,
                                  ಧ್ರುವ ವರ್ಣ*& replaceScript,
                                  ಧ್ರುವ ವರ್ಣ*& replaceRegion,
                                  ಧ್ರುವ ವರ್ಣ*& replaceVariant,
                                  ಧ್ರುವ ವರ್ಣ*& replaceExtensions,
                                  UVector& toBeFreed,
                                  UErrorCode& status);

    // Replace by using languageAlias.
    ತಾರ್ಕಿಕ replaceLanguage(ತಾರ್ಕಿಕ checkLanguage, ತಾರ್ಕಿಕ checkRegion,
                         ತಾರ್ಕಿಕ checkVariants, UVector& toBeFreed,
                         UErrorCode& status);

    // Replace by using territoryAlias.
    ತಾರ್ಕಿಕ replaceTerritory(UVector& toBeFreed, UErrorCode& status);

    // Replace by using scriptAlias.
    ತಾರ್ಕಿಕ replaceScript(UErrorCode& status);

    // Replace by using variantAlias.
    ತಾರ್ಕಿಕ replaceVariant(UErrorCode& status);

    // Replace by using subdivisionAlias.
    ತಾರ್ಕಿಕ replaceSubdivision(StringPiece subdivision,
                            CharString& output, UErrorCode& status);

    // Replace transformed extensions.
    ತಾರ್ಕಿಕ replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UErrorCode& status);
};

CharString&
AliasReplacer::generate_ಕೀಲಿ(
        ಧ್ರುವ ವರ್ಣ* language, ಧ್ರುವ ವರ್ಣ* region, ಧ್ರುವ ವರ್ಣ* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ out; }
    out.append(language, status);
    ಯದಿ (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    ಯದಿ (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    ಪ್ರತ್ಯಾಗಮ out;
}

ಶೂನ್ಯ
AliasReplacer::parseLanguageReplacement(
    ಧ್ರುವ ವರ್ಣ* replacement,
    ಧ್ರುವ ವರ್ಣ*& replacedLanguage,
    ಧ್ರುವ ವರ್ಣ*& replacedScript,
    ಧ್ರುವ ವರ್ಣ*& replacedRegion,
    ಧ್ರುವ ವರ್ಣ*& replacedVariant,
    ಧ್ರುವ ವರ್ಣ*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    ಯದಿ (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        ಪ್ರತ್ಯಾಗಮ;
    }
    // We have multiple field so we have to allocate and parse
    auto str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    ವರ್ಣ* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    replacedLanguage = (ಧ್ರುವ ವರ್ಣ*) data;
    ವರ್ಣ* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    ಧ್ರುವ ವರ್ಣ* start = endOfField;
    endOfField = const_cast<ವರ್ಣ*>(uprv_strchr(start, '_'));
    size_t len = 0;
    ಯದಿ (endOfField == nullptr) {
        len = uprv_strlen(start);
    } ಅನ್ಯಥಾ {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    ಯದಿ (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        ಯದಿ (endOfField == nullptr) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        start = endOfField++;
        endOfField = const_cast<ವರ್ಣ*>(uprv_strchr(start, '_'));
        ಯದಿ (endOfField == nullptr) {
            len = uprv_strlen(start);
        } ಅನ್ಯಥಾ {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    ಯದಿ (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        ಯದಿ (endOfField == nullptr) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        start = endOfField++;
        endOfField = const_cast<ವರ್ಣ*>(uprv_strchr(start, '_'));
        ಯದಿ (endOfField == nullptr) {
            len = uprv_strlen(start);
        } ಅನ್ಯಥಾ {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    ಯದಿ (len >= 4) {
        // Got a variant
        replacedVariant = start;
        ಯದಿ (endOfField == nullptr) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        start = endOfField++;
    }
    replacedExtensions = start;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceLanguage(
        ತಾರ್ಕಿಕ checkLanguage, ತಾರ್ಕಿಕ checkRegion,
        ತಾರ್ಕಿಕ checkVariants, UVector& toBeFreed, UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ಧ್ರುವ ವರ್ಣ* ಹುಡುಕು_Language = checkLanguage ? language : "und";
    ಧ್ರುವ ವರ್ಣ* ಹುಡುಕು_Region = checkRegion ? region : nullptr;
    ಧ್ರುವ ವರ್ಣ* ಹುಡುಕು_Variant = nullptr;
    ಪ್ರತಿ (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        ಯದಿ (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            ಹುಡುಕು_Variant = static_cast<ಧ್ರುವ ವರ್ಣ*>(variants.elementAt(variant_index));
        }

        ಯದಿ (ಹುಡುಕು_Variant != nullptr && uprv_strlen(ಹುಡುಕು_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            ಹುಡುಕು_Variant = nullptr;
        }
        CharString type_ಕೀಲಿ;
        generate_ಕೀಲಿ(ಹುಡುಕು_Language, ಹುಡುಕು_Region, ಹುಡುಕು_Variant, type_ಕೀಲಿ,
                    status);
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        ಧ್ರುವ ವರ್ಣ *replacement = data->languageMap().get(type_ಕೀಲಿ.data());
        ಯದಿ (replacement == nullptr) {
            // Found no replacement data.
            ಅನುವರ್ತ;
        }

        ಧ್ರುವ ವರ್ಣ* replacedLanguage = nullptr;
        ಧ್ರುವ ವರ್ಣ* replacedScript = nullptr;
        ಧ್ರುವ ವರ್ಣ* replacedRegion = nullptr;
        ಧ್ರುವ ವರ್ಣ* replacedVariant = nullptr;
        ಧ್ರುವ ವರ್ಣ* replacedExtensions = nullptr;
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
        replacedRegion = deleteOrReplace(region, ಹುಡುಕು_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            ಹುಡುಕು_Variant, ಹುಡುಕು_Variant, replacedVariant);

        ಯದಿ (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(ಹುಡುಕು_Variant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            ಅನುವರ್ತ;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        ಯದಿ (ಹುಡುಕು_Variant != nullptr) {
            ಯದಿ (notEmpty(replacedVariant)) {
                variants.setElementAt((ಶೂನ್ಯ*)replacedVariant, variant_index);
            } ಅನ್ಯಥಾ {
                variants.removeElementAt(variant_index);
            }
        }
        ಯದಿ (replacedExtensions != nullptr) {
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
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    // Nothing changed by language alias data.
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (region == nullptr) {
        // No region to search.
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ ವರ್ಣ *replacement = data->territoryMap().get(region);
    ಯದಿ (replacement == nullptr) {
        // Found no replacement data for this region.
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ ವರ್ಣ* replacedRegion = replacement;
    ಧ್ರುವ ವರ್ಣ* firstSpace = uprv_strchr(replacement, ' ');
    ಯದಿ (firstSpace != nullptr) {
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
        ಧ್ರುವ ವರ್ಣ* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        ಯದಿ (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ಧ್ರುವ ವರ್ಣ* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            ಯದಿ (foundInReplacement != nullptr) {
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
        ಯದಿ (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceScript(UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (script == nullptr) {
        // No script to search.
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ ವರ್ಣ *replacement = data->scriptMap().get(script);
    ಯದಿ (replacement == nullptr) {
        // Found no replacement data for this script.
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceVariant(UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    // Since we may have more than one variant, we need to loop through them.
    ಪ್ರತಿ (int32_t i = 0; i < variants.size(); i++) {
        ಧ್ರುವ ವರ್ಣ* variant = static_cast<ಧ್ರುವ ವರ್ಣ*>(variants.elementAt(i));
        ಧ್ರುವ ವರ್ಣ *replacement = data->variantMap().get(variant);
        ಯದಿ (replacement == nullptr) {
            // Found no replacement data for this variant.
            ಅನುವರ್ತ;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        ಯದಿ (!same(variant, replacement)) {
            variants.setElementAt((ಶೂನ್ಯ*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            ಯದಿ (uprv_strcmp(variant, "heploc") == 0) {
                ಪ್ರತಿ (int32_t j = 0; j < variants.size(); j++) {
                     ಯದಿ (uprv_strcmp((ಧ್ರುವ ವರ್ಣ*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ ವರ್ಣ *replacement = data->subdivisionMap().get(subdivision.data());
    ಯದಿ (replacement != nullptr) {
        ಧ್ರುವ ವರ್ಣ* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        ಯದಿ (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            ಯದಿ (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    int32_t len = transformedExtensions.length();
    ಧ್ರುವ ವರ್ಣ* str = transformedExtensions.data();
    ಧ್ರುವ ವರ್ಣ* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    ಯದಿ (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        T_CString_toLowerCase(output.data());
    }
    ಯದಿ (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        ಕುರು {
            ಧ್ರುವ ವರ್ಣ* tvalue = uprv_strchr(tkey, '-');
            ಯದಿ (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            ಧ್ರುವ ವರ್ಣ* nextTKey = ultag_getTKeyStart(tvalue);
            ಯದಿ (nextTKey != nullptr) {
                *const_cast<ವರ್ಣ*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.ಸೇರಿಸು_Element_At((ಶೂನ್ಯ*)tkey, tfields.size(), status);
            ಯದಿ (U_FAILURE(status)) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            tkey = nextTKey;
        } ಯಾವತ್ (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            ಪ್ರತ್ಯಾಗಮ uprv_strcmp((ಧ್ರುವ ವರ್ಣ*)e1.pointer, (ಧ್ರುವ ವರ್ಣ*)e2.pointer);
        }, status);
        ಪ್ರತಿ (int32_t i = 0; i < tfields.size(); i++) {
             ಯದಿ (output.length() > 0) {
                 output.append('-', status);
             }
             ಧ್ರುವ ವರ್ಣ* tfield = static_cast<ಧ್ರುವ ವರ್ಣ*>(tfields.elementAt(i));
             ಧ್ರುವ ವರ್ಣ* tvalue = uprv_strchr(tfield, '-');
             ಯದಿ (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<ವರ್ಣ*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ out; }
    out.append(language, status);
    ಯದಿ (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    ಯದಿ (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    ಯದಿ (variants.size() > 0) {
        ಯದಿ (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            ಪ್ರತ್ಯಾಗಮ uprv_strcmp((ಧ್ರುವ ವರ್ಣ*)e1.pointer, (ಧ್ರುವ ವರ್ಣ*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        ಪ್ರತಿ (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<ಧ್ರುವ ವರ್ಣ*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    ಯದಿ (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    ಪ್ರತ್ಯಾಗಮ out;
}

ತಾರ್ಕಿಕ
AliasReplacer::replace(ಧ್ರುವ Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    U_ASSERT(data != nullptr);
    out.clear();
    language = locale.getLanguage();
    ಯದಿ (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    ಯದಿ (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    ಯದಿ (!notEmpty(region)) {
        region = nullptr;
    }
    ಧ್ರುವ ವರ್ಣ* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    ಯದಿ (!variantsBuff.isEmpty()) {
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
        ವರ್ಣ* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        ವರ್ಣ* end;
        ಯಾವತ್ ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            ಯದಿ (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        ಯದಿ (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        ಪ್ರತ್ಯಾಗಮ uprv_strcmp((ಧ್ರುವ ವರ್ಣ*)e1.pointer, (ಧ್ರುವ ವರ್ಣ*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    ಧ್ರುವ int32_t maxChanged = 4 + variants.size();
    (ಶೂನ್ಯ)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](ಶೂನ್ಯ *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    ಯಾವತ್ (U_SUCCESS(status)) {
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
        ಯದಿ (    replaceLanguage(ಸತ್ಯ, ಸತ್ಯ,  ಸತ್ಯ,  stringsToBeFreed, status) ||
                replaceLanguage(ಸತ್ಯ, ಸತ್ಯ,  ಅಸತ್ಯ, stringsToBeFreed, status) ||
                replaceLanguage(ಸತ್ಯ, ಅಸತ್ಯ, ಸತ್ಯ,  stringsToBeFreed, status) ||
                replaceLanguage(ಸತ್ಯ, ಅಸತ್ಯ, ಅಸತ್ಯ, stringsToBeFreed, status) ||
                replaceLanguage(ಅಸತ್ಯ,ಅಸತ್ಯ, ಸತ್ಯ,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            ಅನುವರ್ತ;
        }
        // Nothing changed. Break out.
        ಭಙ್ಗ;
    }  // while(1)

    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ಧ್ರುವ ವರ್ಣ* extensionsStr = locale_getKeywordsStart(locale.getName());
    ಯದಿ (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    outputToString(out, status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        ಯದಿ (U_SUCCESS(status) && !iter.isNull()) {
            ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ;
            ಯಾವತ್ ((ಕೀಲಿ = iter->next(nullptr, status)) != nullptr) {
                ಯದಿ (uprv_strcmp("sd", ಕೀಲಿ) == 0 || uprv_strcmp("rg", ಕೀಲಿ) == 0 ||
                        uprv_strcmp("t", ಕೀಲಿ) == 0) {
                    auto value = locale.get_Keyword_ಮೌಲ್ಯ<CharString>(ಕೀಲಿ, status);
                    ಯದಿ (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        ಅನುವರ್ತ;
                    }
                    CharString replacement;
                    ಯದಿ (uprv_strlen(ಕೀಲಿ) == 2) {
                        ಯದಿ (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.set_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, replacement.data(), status);
                        }
                    } ಅನ್ಯಥಾ {
                        U_ASSERT(uprv_strcmp(ಕೀಲಿ, "t") == 0);
                        ಯದಿ (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.set_Keyword_ಮೌಲ್ಯ(ಕೀಲಿ, replacement.data(), status);
                        }
                    }
                    ಯದಿ (U_FAILURE(status)) {
                        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                    }
                }
            }
        }
        ಯದಿ (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    // If the tag is not changed, return.
    ಯದಿ (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
ತಾರ್ಕಿಕ
canonicalizeLocale(ಧ್ರುವ Locale& locale, CharString& out, UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    AliasReplacer replacer(status);
    ಪ್ರತ್ಯಾಗಮ replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
ತಾರ್ಕಿಕ
isKnownCanonicalizedLocale(ಧ್ರುವ ವರ್ಣ* locale, UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }

    ಯದಿ (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    ಪ್ರತ್ಯಾಗಮ uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    ಪ್ರತ್ಯಾಗಮ KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT ತಾರ್ಕಿಕ
ulocimp_isCanonicalizedLocaleForTest(ಧ್ರುವ ವರ್ಣ* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    ಪ್ರತ್ಯಾಗಮ !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ಧ್ರುವ ವರ್ಣ* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    ಪ್ರತ್ಯಾಗಮ localeID == nullptr ? *ಏತತ್ = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    ಕುರು {
        ವರ್ಣ *separator;
        ವರ್ಣ *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        ಧ್ರುವ auto parse = [canonicalize](std::string_view localeID,
                                          ವರ್ಣ* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    ಯದಿ (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } ಅನ್ಯಥಾ {
                        ulocimp_getName(localeID, sink, status);
                    }
                },
                status);
        };

        // "canonicalize" the locale ID to ICU/Java format
        ವರ್ಣ* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, ಪರಿಮಾಣ Nest::baseName, err);

        FixedString fullNameBuffer;
        ಯದಿ (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(ಪರಿಮಾಣ Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            ಯದಿ (!fullNameBuffer.reserve(length + 1)) {
                ಭಙ್ಗ; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        ಯದಿ(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            ಭಙ್ಗ;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        ವರ್ಣ* at = uprv_strchr(fullName, '@');
        ಯಾವತ್ ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        ವರ್ಣ* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        ಯದಿ (separator!=nullptr || sep2!=nullptr) {
            ಯದಿ (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } ಅನ್ಯಥಾ {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        ತಾರ್ಕಿಕ hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        ಯದಿ (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            ಭಙ್ಗ; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        ಯದಿ (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        ಯದಿ (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        ಯದಿ (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } ಅನ್ಯಥಾ ಯದಿ (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        ಯದಿ (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } ಅನ್ಯಥಾ ಯದಿ (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        ಯದಿ (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } ಅನ್ಯಥಾ {
            ಯದಿ (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                ಯದಿ (fullNameBuffer.isEmpty()) {
                    ಭಙ್ಗ; // error: out of memory
                }
                ಯದಿ (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                ಯದಿ (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                ಯದಿ (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            ಯದಿ (isBogus()) {
                ಭಙ್ಗ; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            ಯದಿ (hasKeywords) {
                ಯದಿ (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    ಯದಿ (heap.ptr->baseName.isEmpty()) {
                        ಭಙ್ಗ; // error: out of memory
                    }
                }
            }
        }

        ಯದಿ (canonicalize) {
            ಯದಿ (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                ಯದಿ (canonicalizeLocale(*ಏತತ್, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), ಅಸತ್ಯ);
                }
                ಯದಿ (U_FAILURE(err)) {
                    ಭಙ್ಗ;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    } ಯಾವತ್(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

int32_t
Locale::hashCode() ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

ಶೂನ್ಯ
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

ಧ್ರುವ Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        ಯದಿ (gDefaultLocale != nullptr) {
            ಪ್ರತ್ಯಾಗಮ *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    ಪ್ರತ್ಯಾಗಮ *locale_set_default_internal(nullptr, status);
}



ಶೂನ್ಯ U_EXPORT2
Locale::setDefault( ಧ್ರುವ   Locale&     newLocale,
                            UErrorCode&  status)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ಧ್ರುವ ವರ್ಣ *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

ಶೂನ್ಯ
Locale::addLikelySubtags(UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    ಯದಿ (U_FAILURE(status)) {
        ಯದಿ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ಪ್ರತ್ಯಾಗಮ;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/ಅಸತ್ಯ);
    ಯದಿ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ಶೂನ್ಯ
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(ಅಸತ್ಯ, status);
}
ಶೂನ್ಯ
Locale::minimizeSubtags(ತಾರ್ಕಿಕ favorScript, UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    ಯದಿ (U_FAILURE(status)) {
        ಯದಿ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ಪ್ರತ್ಯಾಗಮ;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/ಅಸತ್ಯ);
    ಯದಿ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ಶೂನ್ಯ
Locale::canonicalize(UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    CharString uncanonicalized(getName(), status);
    ಯದಿ (U_FAILURE(status)) {
        ಯದಿ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ಪ್ರತ್ಯಾಗಮ;
    }
    init(uncanonicalized.data(), /*canonicalize=*/ಸತ್ಯ);
    ಯದಿ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ result;
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

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ result;
    }

    ಯದಿ (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ result;
    }

    result.init(localeID.data(), /*canonicalize=*/ಅಸತ್ಯ);
    ಯದಿ (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) ಧ್ರುವ
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/ಅಸತ್ಯ, status);
}

Locale U_EXPORT2
Locale::createFromName (ಧ್ರುವ ವರ್ಣ *name)
{
    ಯದಿ (name) {
        Locale l("");
        l.init(name, ಅಸತ್ಯ);
        ಪ್ರತ್ಯಾಗಮ l;
    }
    ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, ಅಸತ್ಯ);
    ಪ್ರತ್ಯಾಗಮ loc;
}

Locale U_EXPORT2
Locale::createCanonical(ಧ್ರುವ ವರ್ಣ* name) {
    Locale loc("");
    loc.init(name, ಸತ್ಯ);
    ಪ್ರತ್ಯಾಗಮ loc;
}

ಧ್ರುವ ವರ್ಣ *
Locale::getISO3Language() ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ uloc_getISO3Language(getName());
}


ಧ್ರುವ ವರ್ಣ *
Locale::getISO3Country() ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ uloc_getLCID(getName());
}

ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ* U_EXPORT2 Locale::getISOCountries()
{
    ಪ್ರತ್ಯಾಗಮ uloc_getISOCountries();
}

ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ* U_EXPORT2 Locale::getISOLanguages()
{
    ಪ್ರತ್ಯಾಗಮ uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
ಶೂನ್ಯ Locale::setFromPOSIXID(ಧ್ರುವ ವರ್ಣ *posixID)
{
    init(posixID, ಸತ್ಯ);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::get_ಬೇರು()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(e_ಬೇರು);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getEnglish()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eENGLISH);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getFrench()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eFRENCH);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getGerman()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eGERMAN);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getItalian()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eITALIAN);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getJapanese()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eJAPANESE);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getKorean()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eKOREAN);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getChinese()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eCHINESE);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eCHINA);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eTAIWAN);
}


ಧ್ರುವ Locale & U_EXPORT2
Locale::getFrance()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eFRANCE);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getGermany()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eGERMANY);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getItaly()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eITALY);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getJapan()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eJAPAN);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getKorea()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eKOREA);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getChina()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eCHINA);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getPRC()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eCHINA);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getTaiwan()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eTAIWAN);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getUK()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eUK);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getUS()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eUS);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getCanada()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eCANADA);
}

ಧ್ರುವ Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    ಪ್ರತ್ಯಾಗಮ getLocale(eCANADA_FRENCH);
}

ಧ್ರುವ Locale &
Locale::getLocale(ಪೂರ್ಣಾಙ್ಕ locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    ಯದಿ (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    ಪ್ರತ್ಯಾಗಮ localeCache[locid]; /*operating on nullptr*/
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
    ಪ್ರತ್ಯಾಗಮ gLocaleCache;
}

ವರ್ಗ KeywordEnumeration : ಸಾರ್ವಜನಿಕ StringEnumeration {
ರಕ್ಷಿತ:
    FixedString keywords;
ಗುಪ್ತ:
    int32_t length;
    ಧ್ರುವ ವರ್ಣ *current;
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ fgClassID;

ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID() { ಪ್ರತ್ಯಾಗಮ (UClassID)&fgClassID; }
    ಆಭಾಸ UClassID getDynamicClassID() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ getStaticClassID(); }
ಸಾರ್ವಜನಿಕ:
    KeywordEnumeration(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿಗಳು, int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        ಯದಿ(U_SUCCESS(status) && keywordLen != 0) {
            ಯದಿ(ಕೀಲಿಗಳು == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } ಅನ್ಯಥಾ {
                keywords = {ಕೀಲಿಗಳು, static_cast<std::string_view::size_type>(length)};
                ಯದಿ (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ಅನ್ಯಥಾ {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    ಆಭಾಸ ~KeywordEnumeration();

    ಆಭಾಸ StringEnumeration * clone() ಧ್ರುವ override
    {
        UErrorCode status = U_ZERO_ERROR;
        ಪ್ರತ್ಯಾಗಮ ನೂತನ KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    ಆಭಾಸ int32_t count(UErrorCode& status) ಧ್ರುವ override {
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ 0; }
        ಧ್ರುವ ವರ್ಣ *kw = keywords.data();
        int32_t result = 0;
        ಯಾವತ್(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        ಪ್ರತ್ಯಾಗಮ result;
    }

    ಆಭಾಸ ಧ್ರುವ ವರ್ಣ* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ಧ್ರುವ ವರ್ಣ* result;
        int32_t len;
        ಯದಿ(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            ಯದಿ(resultLength != nullptr) {
                *resultLength = len;
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        ಪ್ರತ್ಯಾಗಮ result;
    }

    ಆಭಾಸ ಧ್ರುವ UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
        int32_t resultLength = 0;
        ಧ್ರುವ ವರ್ಣ *s = next(&resultLength, status);
        ಪ್ರತ್ಯಾಗಮ setChars(s, resultLength, status);
    }

    ಆಭಾಸ ಶೂನ್ಯ reset(UErrorCode& status) override {
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
        current = keywords.data();
    }
};

ಧ್ರುವ ವರ್ಣ KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = ಸ್ವಭಾವ;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
ವರ್ಗ UnicodeKeywordEnumeration : ಸಾರ್ವಜನಿಕ KeywordEnumeration {
ಸಾರ್ವಜನಿಕ:
    using KeywordEnumeration::KeywordEnumeration;
    ಆಭಾಸ ~UnicodeKeywordEnumeration();

    ಆಭಾಸ ಧ್ರುವ ವರ್ಣ* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ಧ್ರುವ ವರ್ಣ* legacy_ಕೀಲಿ = KeywordEnumeration::next(nullptr, status);
        ಯಾವತ್ (U_SUCCESS(status) && legacy_ಕೀಲಿ != nullptr) {
            ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ = uloc_to_Unicode_Locale_ಕೀಲಿ(legacy_ಕೀಲಿ);
            ಯದಿ (ಕೀಲಿ != nullptr) {
                ಯದಿ (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(ಕೀಲಿ));
                }
                ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_ಕೀಲಿ = KeywordEnumeration::next(nullptr, status);
        }
        ಯದಿ (resultLength != nullptr) *resultLength = 0;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಆಭಾಸ int32_t count(UErrorCode& status) ಧ್ರುವ override {
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ 0; }
        ಧ್ರುವ ವರ್ಣ *kw = keywords.data();
        int32_t result = 0;
        ಯಾವತ್(*kw) {
            ಯದಿ (uloc_to_Unicode_Locale_ಕೀಲಿ(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        ಪ್ರತ್ಯಾಗಮ result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = ಸ್ವಭಾವ;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) ಧ್ರುವ
{
    StringEnumeration *result = nullptr;

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ result;
    }

    ಧ್ರುವ ವರ್ಣ* variantStart = uprv_strchr(getName(), '@');
    ಧ್ರುವ ವರ್ಣ* assignment = uprv_strchr(getName(), '=');
    ಯದಿ(variantStart) {
        ಯದಿ(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', ಅಸತ್ಯ, status);
            ಯದಿ (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = ನೂತನ KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                ಯದಿ (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } ಅನ್ಯಥಾ {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) ಧ್ರುವ
{
    StringEnumeration *result = nullptr;

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ result;
    }

    ಧ್ರುವ ವರ್ಣ* variantStart = uprv_strchr(getName(), '@');
    ಧ್ರುವ ವರ್ಣ* assignment = uprv_strchr(getName(), '=');
    ಯದಿ(variantStart) {
        ಯದಿ(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', ಅಸತ್ಯ, status);
            ಯದಿ (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = ನೂತನ UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                ಯದಿ (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } ಅನ್ಯಥಾ {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

int32_t
Locale::get_Keyword_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ* keywordName, ವರ್ಣ *buffer, int32_t bufLen, UErrorCode &status) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ uloc_get_Keyword_ಮೌಲ್ಯ(getName(), keywordName, buffer, bufLen, &status);
}

ಶೂನ್ಯ
Locale::get_Keyword_ಮೌಲ್ಯ(StringPiece keywordName, ByteSink& sink, UErrorCode& status) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ulocimp_get_Keyword_ಮೌಲ್ಯ(getName(), keywordName, sink, status);
}

ಶೂನ್ಯ
Locale::get_Unicode_Keyword_ಮೌಲ್ಯ(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    std::optional<std::string_view> legacy_ಕೀಲಿ = ulocimp_to_Legacy_ಕೀಲಿ_With_Fallback(keywordName);
    ಯದಿ (!legacy_ಕೀಲಿ.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    auto legacy_ಮೌಲ್ಯ = get_Keyword_ಮೌಲ್ಯ<CharString>(*legacy_ಕೀಲಿ, status);

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    std::optional<std::string_view> unicode_ಮೌಲ್ಯ =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_ಮೌಲ್ಯ.toStringPiece());
    ಯದಿ (!unicode_ಮೌಲ್ಯ.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    sink.Append(unicode_ಮೌಲ್ಯ->data(), static_cast<int32_t>(unicode_ಮೌಲ್ಯ->size()));
}

ಶೂನ್ಯ
Locale::set_Keyword_ಮೌಲ್ಯ(StringPiece keywordName,
                        StringPiece keyword_ಮೌಲ್ಯ,
                        UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
    ಯದಿ (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_set_Keyword_ಮೌಲ್ಯ(keywordName, keyword_ಮೌಲ್ಯ, localeID, status);
    ಯದಿ (U_FAILURE(status)) {
        ಯದಿ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಧ್ರುವ ವರ್ಣ* at = locale_getKeywordsStart(localeID.toStringPiece());
    ತಾರ್ಕಿಕ hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    ಯದಿ (!hasKeywords) {
        ಯದಿ (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            ಯದಿ (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < ಪರಿಮಾಣ Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, ಪರಿಮಾಣ Nest::baseName, status);
            } ಅನ್ಯಥಾ {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                ಯದಿ (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ಪ್ರತ್ಯಾಗಮ;
                }
            }
        }
    } ಅನ್ಯಥಾ {
        Heap* heap = nullptr;
        ಯದಿ (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            ಯದಿ (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
        } ಅನ್ಯಥಾ {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        ಯದಿ (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            ಪ್ರತ್ಯಾಗಮ;
        }

        ಯದಿ (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            ಯದಿ (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                ಯದಿ (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ಪ್ರತ್ಯಾಗಮ;
                }
            }
        }
    }
}

ಶೂನ್ಯ
Locale::set_Unicode_Keyword_ಮೌಲ್ಯ(StringPiece keywordName,
                               StringPiece keyword_ಮೌಲ್ಯ,
                               UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    std::optional<std::string_view> legacy_ಕೀಲಿ = ulocimp_to_Legacy_ಕೀಲಿ_With_Fallback(keywordName);
    ಯದಿ (!legacy_ಕೀಲಿ.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    std::string_view value;

    ಯದಿ (!keyword_ಮೌಲ್ಯ.empty()) {
        std::optional<std::string_view> legacy_ಮೌಲ್ಯ =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_ಮೌಲ್ಯ);
        ಯದಿ (!legacy_ಮೌಲ್ಯ.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        value = *legacy_ಮೌಲ್ಯ;
    }

    set_Keyword_ಮೌಲ್ಯ(*legacy_ಕೀಲಿ, value, status);
}

ಧ್ರುವ ವರ್ಣ*
Locale::getCountry() ಧ್ರುವ U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ getField<&Nest::getRegion, &Heap::getRegion>();
}

ಧ್ರುವ ವರ್ಣ*
Locale::getLanguage() ಧ್ರುವ U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ getField<&Nest::getLanguage, &Heap::getLanguage>();
}

ಧ್ರುವ ವರ್ಣ*
Locale::getScript() ಧ್ರುವ U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ getField<&Nest::getScript, &Heap::getScript>();
}

ಧ್ರುವ ವರ್ಣ*
Locale::getVariant() ಧ್ರುವ U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ getField<&Nest::getVariant, &Heap::getVariant>();
}

ಧ್ರುವ ವರ್ಣ*
Locale::getName() ಧ್ರುವ U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ getField<&Nest::getBaseName, &Heap::getFullName>();
}

ಧ್ರುವ ವರ್ಣ*
Locale::getBaseName() ಧ್ರುವ U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ getField<&Nest::getBaseName, &Heap::getBaseName>();
}

template <ಧ್ರುವ ವರ್ಣ* (Locale::Nest::*ಧ್ರುವ NEST)() ಧ್ರುವ,
          ಧ್ರುವ ವರ್ಣ* (Locale::Heap::*ಧ್ರುವ HEAP)() ಧ್ರುವ>
ಧ್ರುವ ವರ್ಣ* Locale::getField() ಧ್ರುವ U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ payload.visit([] { ಪ್ರತ್ಯಾಗಮ ""; },
                         [](ಧ್ರುವ Nest& nest) { ಪ್ರತ್ಯಾಗಮ (nest.*NEST)(); },
                         [](ಧ್ರುವ Heap& heap) { ಪ್ರತ್ಯಾಗಮ (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = ಸ್ವಭಾವ;

//eof
U_NAMESPACE_END
