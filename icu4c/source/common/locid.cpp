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

#ଅନ୍ତର୍ଭାଵ <cstddef>
#ଅନ୍ତର୍ଭାଵ <optional>
#ଅନ୍ତର୍ଭାଵ <string_view>
#ଅନ୍ତର୍ଭାଵ <type_traits>
#ଅନ୍ତର୍ଭାଵ <utility>

#ଅନ୍ତର୍ଭାଵ "unicode/bytestream.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localebuilder.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localpointer.h"
#ଅନ୍ତର୍ଭାଵ "unicode/strenum.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"

#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "charstrmap.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "fixedstring.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "ulocimp.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "uniquecharstr.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"

#ଯଦି UPRV_HAS_FEATURE(memory_sanitizer)
#ଅନ୍ତର୍ଭାଵ <sanitizer/msan_interface.h>
#endif

U_NAMESPACE_BEGIN

ସ୍ଥିର Locale   *gLocaleCache = nullptr;
ସ୍ଥିର UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
ସ୍ଥିର UMutex gDefaultLocaleMutex;
ସ୍ଥିର UHashtable *gDefaultLocalesHashT = nullptr;
ସ୍ଥିର Locale *gDefaultLocale = nullptr;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#define ULOC_STRING_LIMIT 357913941

U_NAMESPACE_END

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ELocalePos {
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
    e_ମୂଳ,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

ନାମାକାଶ {

//
// Deleter function for Locales owned by the default Locale hash table/
//
ଶୂନ୍ୟ U_CALLCONV
deleteLocale(ଶୂନ୍ୟ *obj) {
    delete static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    delete [] gLocaleCache;
    gLocaleCache = nullptr;
    gLocaleCacheInitOnce.reset();

    ଯଦି (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = ନୂତନ Locale[static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(eMAX_LOCALES)];
    ଯଦି (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_ମୂଳ]          = Locale("");
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

Locale *locale_set_default_internal(ଅଚଳ ଅକ୍ଷର *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = ମିଥ୍ୟା;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    ଯଦି (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = ସତ୍ୟ; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ gDefaultLocale;
    }

    ଯଦି (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ gDefaultLocale;
        }
        uhash_set_ମୂଲ୍ୟ_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    ଯଦି (newDefault == nullptr) {
        newDefault = ନୂତନ Locale(Locale::eBOGUS);
        ଯଦି (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), ମିଥ୍ୟା);
        uhash_put(gDefaultLocalesHashT, const_cast<ଅକ୍ଷର*>(newDefault->getName()), newDefault, &status);
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    ଫେରନ୍ତୁ gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC ଶୂନ୍ୟ
locale_set_default(ଅଚଳ ଅକ୍ଷର *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC ଅଚଳ ଅକ୍ଷର *
locale_get_default()
{
    U_NAMESPACE_USE
    ଫେରନ୍ତୁ Locale::getDefault().getName();
}

ନାମାକାଶ {

template <auto FIELD, typename T>
ଶୂନ୍ୟ copyToArray(std::string_view sv, T* that) {
    auto& field = that->*FIELD;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    ଯଦି (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.data(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

ଶୂନ୍ୟ Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, ଏହି);
    copyToArray<&Nest::script>(script, ଏହି);
    copyToArray<&Nest::region>(region, ଏହି);
    ଏହି->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(Heap, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(Heap, region));
    U_ASSERT(ଏହି == reinterpret_cast<Nest*>(&heap));
#ଯଦି UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, ଆକାର(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, ଏହି);
    copyToArray<&Nest::region>(heap.region, ଏହି);
    ଏହି->variantBegin = variantBegin;
    *ଏହି->baseName = '\0';
}

ସଂରଚନା Locale::Heap::Alloc : ସର୍ବସାଧାରଣ UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    ଅଚଳ ଅକ୍ଷର* getVariant() ଅଚଳ U_LIFETIME_BOUND {
        ଫେରନ୍ତୁ variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    ଅଚଳ ଅକ୍ଷର* getFullName() ଅଚଳ U_LIFETIME_BOUND { ଫେରନ୍ତୁ fullName.data(); }
    ଅଚଳ ଅକ୍ଷର* getBaseName() ଅଚଳ U_LIFETIME_BOUND {
        ଯଦି (baseName.isEmpty()) {
            ଯଦି (ଅଚଳ ଅକ୍ଷର* name = fullName.data(); *name != '@') {
                ଫେରନ୍ତୁ name;
            }
        }
        ଫେରନ୍ତୁ baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(ଅଚଳ Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        ଯଦି (U_SUCCESS(status)) {
            ଯଦି (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                ଯଦି (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ଅନ୍ୟଥା {
                    ଯଦି (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        ଯଦି (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = delete;

    ~Alloc() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
};

ଅଚଳ ଅକ୍ଷର* Locale::Heap::getVariant() ଅଚଳ U_LIFETIME_BOUND { ଫେରନ୍ତୁ ptr->getVariant(); }
ଅଚଳ ଅକ୍ଷର* Locale::Heap::getFullName() ଅଚଳ U_LIFETIME_BOUND { ଫେରନ୍ତୁ ptr->getFullName(); }
ଅଚଳ ଅକ୍ଷର* Locale::Heap::getBaseName() ଅଚଳ U_LIFETIME_BOUND { ଫେରନ୍ତୁ ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = ନୂତନ Alloc(variantBegin);
    ଯଦି (ptr == nullptr) {
        type = eBOGUS;
    } ଅନ୍ୟଥା {
        type = eHEAP;
        copyToArray<&Heap::language>(language, ଏହି);
        copyToArray<&Heap::script>(script, ଏହି);
        copyToArray<&Heap::region>(region, ଏହି);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::operator=(ଅଚଳ Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = ନୂତନ Alloc(*other.ptr, status);
    ଯଦି (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } ଅନ୍ୟଥା {
        type = eHEAP;
        uprv_memcpy(language, other.language, ଆକାର language);
        uprv_memcpy(script, other.script, ଆକାର script);
        uprv_memcpy(region, other.region, ଆକାର region);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

Locale::Heap& Locale::Heap::operator=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, ଆକାର language);
    uprv_memcpy(script, other.script, ଆକାର script);
    uprv_memcpy(region, other.region, ଆକାର region);
    ଫେରନ୍ତୁ *ଏହି;
}

template <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
auto Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) ଅଚଳ {
    ବାଛ (type) {
        ସ୍ଥିତି eBOGUS:
            ଫେରନ୍ତୁ bogusFn(args...);
        ସ୍ଥିତି eNEST:
            ଫେରନ୍ତୁ nestFn(nest, args...);
        ସ୍ଥିତି eHEAP:
            ଫେରନ୍ତୁ heapFn(heap, args...);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            UPRV_UNREACHABLE_EXIT;
    };
}

ଶୂନ୍ୟ Locale::Payload::copy(ଅଚଳ Payload& other) {
    other.visit([](Payload*) {},
                [](ଅଚଳ Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ଅଚଳ Heap& heap, Payload* dst) { dst->heap = heap; },
                ଏହି);
}

ଶୂନ୍ୟ Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](ଅଚଳ Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ଅଚଳ Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        ଏହି);
}

Locale::Payload::~Payload() {
    ଯଦି (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(ଅଚଳ Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::operator=(ଅଚଳ Payload& other) U_LIFETIME_BOUND {
    ଯଦି (ଏହି != &other) {
        setToBogus();
        copy(other);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

Locale::Payload& Locale::Payload::operator=(Payload&& other) noexcept U_LIFETIME_BOUND {
    ଯଦି (ଏହି != &other) {
        setToBogus();
        move(std::move(other));
    }
    ଫେରନ୍ତୁ *ଏହି;
}

ଶୂନ୍ୟ Locale::Payload::setToBogus() {
    ଏହି->~Payload();
    type = eBOGUS;
}

template <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    ଯଦି constexpr (std::is_same_v<T, Nest>) {
        ଏହି->~Payload();
        ::ନୂତନ (&nest) Nest(std::forward<Args>(args)...);
        ଫେରନ୍ତୁ nest;
    }
    ଯଦି constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::ନୂତନ (&heap) Heap(std::forward<Args>(args)...);
        ଫେରନ୍ତୁ heap;
    }
}

template <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ type == eNEST ? &nest : nullptr;
}
template <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, ମିଥ୍ୟା);
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


Locale::Locale( ଅଚଳ   ଅକ୍ଷର * newLanguage,
                ଅଚଳ   ଅକ୍ଷର * newCountry,
                ଅଚଳ   ଅକ୍ଷର * newVariant,
                ଅଚଳ   ଅକ୍ଷର * newKeywords)
    : UObject(), payload()
{
    ଯଦି( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, ମିଥ୍ୟା); /* shortcut */
    }
    ଅନ୍ୟଥା
    {
        UErrorCode status = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        ଯଦି ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            ଯଦି ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                ଫେରନ୍ତୁ;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        ଯଦି ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            ଯଦି ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                ଫେରନ୍ତୁ;
            }
        }

        // _Variant
        ଯଦି ( newVariant != nullptr )
        {
            // remove leading _'s
            ଯେତେବେଳେ(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            ଯଦି ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                ଫେରନ୍ତୁ;
            }
            ଯେତେବେଳେ( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        ଯଦି ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            ଯଦି ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              ଫେରନ୍ତୁ;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        ଯଦି ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        ଯଦି ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        ଯଦି ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        ଯଦି ( ksize != 0)
        {
            ଯଦି (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            ଅନ୍ୟଥା {
                togo.append('_', status); /* Variant parsing with a script */
                ଯଦି ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        ଯଦି (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            ଫେରନ୍ତୁ;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), ମିଥ୍ୟା);
    }
}

Locale::Locale(ଅଚଳ Locale&) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
Locale::Locale(Locale&&) noexcept = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

Locale& Locale::operator=(ଅଚଳ Locale&) U_LIFETIME_BOUND = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
Locale& Locale::operator=(Locale&&) noexcept U_LIFETIME_BOUND = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

Locale *
Locale::clone() ଅଚଳ {
    ଫେରନ୍ତୁ ନୂତନ Locale(*ଏହି);
}

ତାର୍କିକ
Locale::operator==( ଅଚଳ   Locale& other) ଅଚଳ
{
    ଫେରନ୍ତୁ uprv_strcmp(other.getName(), getName()) == 0;
}

ନାମାକାଶ {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr ଅଚଳ ଅକ୍ଷର* KNOWN_CANONICALIZED[] = {
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
    ଯଦି (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    ପାଇଁ (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (ଶୂନ୍ୟ*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

ଶ୍ରେଣୀ AliasData;

/**
 * A Builder class to build the alias data.
 */
ଶ୍ରେଣୀ AliasDataBuilder {
ସର୍ବସାଧାରଣ:
    AliasDataBuilder() {
    }

    // Build the AliasData from resource.
    AliasData* build(UErrorCode &status);

ବ୍ୟକ୍ତିଗତ:
    ଶୂନ୍ୟ readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   ଶୂନ୍ୟ (*checkType)(ଅଚଳ ଅକ୍ଷର* type),
                   ଶୂନ୍ୟ (*checkReplacement)(ଅଚଳ UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ଶୂନ୍ୟ readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
                           LocalMemory<int32_t>& replacementIndexes,
                           int32_t &length,
                           UErrorCode &status);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ଶୂନ୍ୟ readScriptAlias(UResourceBundle* alias,
                         UniqueCharStrings* strings,
                         LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ଶୂନ୍ୟ readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    ଶୂନ୍ୟ readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    ଶୂନ୍ୟ readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
ଶ୍ରେଣୀ AliasData : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    ସ୍ଥିର ଅଚଳ AliasData* singleton(UErrorCode& status) {
        ଯଦି (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            ଫେରନ୍ତୁ nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        ଫେରନ୍ତୁ gSingleton;
    }

    ଅଚଳ CharStringMap& languageMap() ଅଚଳ U_LIFETIME_BOUND { ଫେରନ୍ତୁ language; }
    ଅଚଳ CharStringMap& scriptMap() ଅଚଳ U_LIFETIME_BOUND { ଫେରନ୍ତୁ script; }
    ଅଚଳ CharStringMap& territoryMap() ଅଚଳ U_LIFETIME_BOUND { ଫେରନ୍ତୁ territory; }
    ଅଚଳ CharStringMap& variantMap() ଅଚଳ U_LIFETIME_BOUND { ଫେରନ୍ତୁ variant; }
    ଅଚଳ CharStringMap& subdivisionMap() ଅଚଳ U_LIFETIME_BOUND { ଫେରନ୍ତୁ subdivision; }

    ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV loadData(UErrorCode &status);
    ସ୍ଥିର UBool U_CALLCONV cleanup();

    ସ୍ଥିର UInitOnce gInitOnce;

ବ୍ୟକ୍ତିଗତ:
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

    ସ୍ଥିର ଅଚଳ AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    friend ଶ୍ରେଣୀ AliasDataBuilder;
};


ଅଚଳ AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        ଶୂନ୍ୟ (*checkType)(ଅଚଳ ଅକ୍ଷର* type),
        ଶୂନ୍ୟ (*checkReplacement)(ଅଚଳ UChar* replacement),
        UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    length = ures_getSize(alias);
    ଅଚଳ ଅକ୍ଷର** rawTypes = types.allocateInsteadAndCopy(length);
    ଯଦି (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    ଯଦି (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        ଅଚଳ ଅକ୍ଷର* aliasFrom = ures_get_ଚାବି(res.getAlias());
        ଅଚଳ UChar* aliasTo =
            ures_get_String_By_ଚାବି(res.getAlias(), "replacement", nullptr, &status);
        ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;

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
ଶୂନ୍ୟ
AliasDataBuilder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ଫେରନ୍ତୁ readAlias(
        alias, strings, types, replacementIndexes, length,
#ଯଦି U_DEBUG
        [](ଅଚଳ ଅକ୍ଷର* type) {
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
#ଅନ୍ୟଥା
        [](ଅଚଳ ଅକ୍ଷର*) {},
#endif
        [](ଅଚଳ UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
ଶୂନ୍ୟ
AliasDataBuilder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ଫେରନ୍ତୁ readAlias(
        alias, strings, types, replacementIndexes, length,
#ଯଦି U_DEBUG
        [](ଅଚଳ ଅକ୍ଷର* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ଅଚଳ UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#ଅନ୍ୟଥା
        [](ଅଚଳ ଅକ୍ଷର*) {},
        [](ଅଚଳ UChar*) { },
#endif
        status);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
ଶୂନ୍ୟ
AliasDataBuilder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ଫେରନ୍ତୁ readAlias(
        alias, strings, types, replacementIndexes, length,
#ଯଦି U_DEBUG
        [](ଅଚଳ ଅକ୍ଷର* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#ଅନ୍ୟଥା
        [](ଅଚଳ ଅକ୍ଷର*) {},
#endif
        [](ଅଚଳ UChar*) { },
        status);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
ଶୂନ୍ୟ
AliasDataBuilder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ଫେରନ୍ତୁ readAlias(
        alias, strings, types, replacementIndexes, length,
#ଯଦି U_DEBUG
        [](ଅଚଳ ଅକ୍ଷର* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](ଅଚଳ UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#ଅନ୍ୟଥା
        [](ଅଚଳ ଅକ୍ଷର*) {},
        [](ଅଚଳ UChar*) { },
#endif
        status);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
ଶୂନ୍ୟ
AliasDataBuilder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<ଅଚଳ ଅକ୍ଷର*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    ଫେରନ୍ତୁ readAlias(
        alias, strings, types, replacementIndexes, length,
#ଯଦି U_DEBUG
        [](ଅଚଳ ଅକ୍ଷର* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#ଅନ୍ୟଥା
        [](ଅଚଳ ଅକ୍ଷର*) {},
#endif
        [](ଅଚଳ UChar*) { },
        status);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
ଶୂନ୍ୟ U_CALLCONV
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
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_ଚାବି(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_ଚାବି(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_ଚାବି(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_ଚାବି(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_ଚାବି(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_ଚାବି(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<ଅଚଳ ଅକ୍ଷର*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<ଅଚଳ ଅକ୍ଷର*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<ଅଚଳ ଅକ୍ଷର*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<ଅଚଳ ଅକ୍ଷର*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<ଅଚଳ ଅକ୍ଷର*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    ପାଇଁ (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    ପାଇଁ (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    ପାଇଁ (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    ପାଇଁ (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    ପାଇଁ (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    // copy hashtables
    auto *data = ନୂତନ AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    ଯଦି (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ଫେରନ୍ତୁ data;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
ଶ୍ରେଣୀ AliasReplacer {
ସର୍ବସାଧାରଣ:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       ଫେରନ୍ତୁ 0==uprv_strcmp((ଅଚଳ ଅକ୍ଷର*)e1.pointer,
                                             (ଅଚଳ ଅକ୍ଷର*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    ତାର୍କିକ replace(
        ଅଚଳ Locale& locale, CharString& out, UErrorCode& status);

ବ୍ୟକ୍ତିଗତ:
    ଅଚଳ ଅକ୍ଷର* language;
    ଅଚଳ ଅକ୍ଷର* script;
    ଅଚଳ ଅକ୍ଷର* region;
    ଅଚଳ ଅକ୍ଷର* extensions;
    UVector variants;

    ଅଚଳ AliasData* data;

    ଧାଡ଼ିଭିତରେ ତାର୍କିକ notEmpty(ଅଚଳ ଅକ୍ଷର* str) {
        ଫେରନ୍ତୁ str && str[0] != NULL_CHAR;
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
    ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* deleteOrReplace(ଅଚଳ ଅକ୍ଷର* input U_LIFETIME_BOUND,
                                       ଅଚଳ ଅକ୍ଷର* type,
                                       ଅଚଳ ଅକ୍ଷର* replacement U_LIFETIME_BOUND) {
        ଫେରନ୍ତୁ notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    ଧାଡ଼ିଭିତରେ ତାର୍କିକ same(ଅଚଳ ଅକ୍ଷର* a, ଅଚଳ ଅକ୍ଷର* b) {
        ଯଦି (a == nullptr && b == nullptr) {
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
        ଯଦି ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        ଫେରନ୍ତୁ uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_ଚାବି(ଅଚଳ ଅକ୍ଷର* language, ଅଚଳ ଅକ୍ଷର* region,
                            ଅଚଳ ଅକ୍ଷର* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    ଶୂନ୍ୟ parseLanguageReplacement(ଅଚଳ ଅକ୍ଷର* replacement,
                                  ଅଚଳ ଅକ୍ଷର*& replaceLanguage,
                                  ଅଚଳ ଅକ୍ଷର*& replaceScript,
                                  ଅଚଳ ଅକ୍ଷର*& replaceRegion,
                                  ଅଚଳ ଅକ୍ଷର*& replaceVariant,
                                  ଅଚଳ ଅକ୍ଷର*& replaceExtensions,
                                  UVector& toBeFreed,
                                  UErrorCode& status);

    // Replace by using languageAlias.
    ତାର୍କିକ replaceLanguage(ତାର୍କିକ checkLanguage, ତାର୍କିକ checkRegion,
                         ତାର୍କିକ checkVariants, UVector& toBeFreed,
                         UErrorCode& status);

    // Replace by using territoryAlias.
    ତାର୍କିକ replaceTerritory(UVector& toBeFreed, UErrorCode& status);

    // Replace by using scriptAlias.
    ତାର୍କିକ replaceScript(UErrorCode& status);

    // Replace by using variantAlias.
    ତାର୍କିକ replaceVariant(UErrorCode& status);

    // Replace by using subdivisionAlias.
    ତାର୍କିକ replaceSubdivision(StringPiece subdivision,
                            CharString& output, UErrorCode& status);

    // Replace transformed extensions.
    ତାର୍କିକ replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UErrorCode& status);
};

CharString&
AliasReplacer::generate_ଚାବି(
        ଅଚଳ ଅକ୍ଷର* language, ଅଚଳ ଅକ୍ଷର* region, ଅଚଳ ଅକ୍ଷର* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ out; }
    out.append(language, status);
    ଯଦି (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    ଯଦି (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    ଫେରନ୍ତୁ out;
}

ଶୂନ୍ୟ
AliasReplacer::parseLanguageReplacement(
    ଅଚଳ ଅକ୍ଷର* replacement,
    ଅଚଳ ଅକ୍ଷର*& replacedLanguage,
    ଅଚଳ ଅକ୍ଷର*& replacedScript,
    ଅଚଳ ଅକ୍ଷର*& replacedRegion,
    ଅଚଳ ଅକ୍ଷର*& replacedVariant,
    ଅଚଳ ଅକ୍ଷର*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    ଯଦି (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        ଫେରନ୍ତୁ;
    }
    // We have multiple field so we have to allocate and parse
    auto str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    ଅକ୍ଷର* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    replacedLanguage = (ଅଚଳ ଅକ୍ଷର*) data;
    ଅକ୍ଷର* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    ଅଚଳ ଅକ୍ଷର* start = endOfField;
    endOfField = const_cast<ଅକ୍ଷର*>(uprv_strchr(start, '_'));
    size_t len = 0;
    ଯଦି (endOfField == nullptr) {
        len = uprv_strlen(start);
    } ଅନ୍ୟଥା {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    ଯଦି (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        ଯଦି (endOfField == nullptr) {
            ଫେରନ୍ତୁ;
        }
        start = endOfField++;
        endOfField = const_cast<ଅକ୍ଷର*>(uprv_strchr(start, '_'));
        ଯଦି (endOfField == nullptr) {
            len = uprv_strlen(start);
        } ଅନ୍ୟଥା {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    ଯଦି (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        ଯଦି (endOfField == nullptr) {
            ଫେରନ୍ତୁ;
        }
        start = endOfField++;
        endOfField = const_cast<ଅକ୍ଷର*>(uprv_strchr(start, '_'));
        ଯଦି (endOfField == nullptr) {
            len = uprv_strlen(start);
        } ଅନ୍ୟଥା {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    ଯଦି (len >= 4) {
        // Got a variant
        replacedVariant = start;
        ଯଦି (endOfField == nullptr) {
            ଫେରନ୍ତୁ;
        }
        start = endOfField++;
    }
    replacedExtensions = start;
}

ତାର୍କିକ
AliasReplacer::replaceLanguage(
        ତାର୍କିକ checkLanguage, ତାର୍କିକ checkRegion,
        ତାର୍କିକ checkVariants, UVector& toBeFreed, UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ଅଚଳ ଅକ୍ଷର* ସନ୍ଧାନ_Language = checkLanguage ? language : "und";
    ଅଚଳ ଅକ୍ଷର* ସନ୍ଧାନ_Region = checkRegion ? region : nullptr;
    ଅଚଳ ଅକ୍ଷର* ସନ୍ଧାନ_Variant = nullptr;
    ପାଇଁ (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        ଯଦି (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            ସନ୍ଧାନ_Variant = static_cast<ଅଚଳ ଅକ୍ଷର*>(variants.elementAt(variant_index));
        }

        ଯଦି (ସନ୍ଧାନ_Variant != nullptr && uprv_strlen(ସନ୍ଧାନ_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            ସନ୍ଧାନ_Variant = nullptr;
        }
        CharString type_ଚାବି;
        generate_ଚାବି(ସନ୍ଧାନ_Language, ସନ୍ଧାନ_Region, ସନ୍ଧାନ_Variant, type_ଚାବି,
                    status);
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        ଅଚଳ ଅକ୍ଷର *replacement = data->languageMap().get(type_ଚାବି.data());
        ଯଦି (replacement == nullptr) {
            // Found no replacement data.
            ଜାରି;
        }

        ଅଚଳ ଅକ୍ଷର* replacedLanguage = nullptr;
        ଅଚଳ ଅକ୍ଷର* replacedScript = nullptr;
        ଅଚଳ ଅକ୍ଷର* replacedRegion = nullptr;
        ଅଚଳ ଅକ୍ଷର* replacedVariant = nullptr;
        ଅଚଳ ଅକ୍ଷର* replacedExtensions = nullptr;
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
        replacedRegion = deleteOrReplace(region, ସନ୍ଧାନ_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            ସନ୍ଧାନ_Variant, ସନ୍ଧାନ_Variant, replacedVariant);

        ଯଦି (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(ସନ୍ଧାନ_Variant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            ଜାରି;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        ଯଦି (ସନ୍ଧାନ_Variant != nullptr) {
            ଯଦି (notEmpty(replacedVariant)) {
                variants.setElementAt((ଶୂନ୍ୟ*)replacedVariant, variant_index);
            } ଅନ୍ୟଥା {
                variants.removeElementAt(variant_index);
            }
        }
        ଯଦି (replacedExtensions != nullptr) {
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
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    // Nothing changed by language alias data.
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (region == nullptr) {
        // No region to search.
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ ଅକ୍ଷର *replacement = data->territoryMap().get(region);
    ଯଦି (replacement == nullptr) {
        // Found no replacement data for this region.
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ ଅକ୍ଷର* replacedRegion = replacement;
    ଅଚଳ ଅକ୍ଷର* firstSpace = uprv_strchr(replacement, ' ');
    ଯଦି (firstSpace != nullptr) {
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
        ଅଚଳ ଅକ୍ଷର* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        ଯଦି (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ଅଚଳ ଅକ୍ଷର* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            ଯଦି (foundInReplacement != nullptr) {
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
        ଯଦି (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ
AliasReplacer::replaceScript(UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (script == nullptr) {
        // No script to search.
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ ଅକ୍ଷର *replacement = data->scriptMap().get(script);
    ଯଦି (replacement == nullptr) {
        // Found no replacement data for this script.
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ
AliasReplacer::replaceVariant(UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    // Since we may have more than one variant, we need to loop through them.
    ପାଇଁ (int32_t i = 0; i < variants.size(); i++) {
        ଅଚଳ ଅକ୍ଷର* variant = static_cast<ଅଚଳ ଅକ୍ଷର*>(variants.elementAt(i));
        ଅଚଳ ଅକ୍ଷର *replacement = data->variantMap().get(variant);
        ଯଦି (replacement == nullptr) {
            // Found no replacement data for this variant.
            ଜାରି;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        ଯଦି (!same(variant, replacement)) {
            variants.setElementAt((ଶୂନ୍ୟ*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            ଯଦି (uprv_strcmp(variant, "heploc") == 0) {
                ପାଇଁ (int32_t j = 0; j < variants.size(); j++) {
                     ଯଦି (uprv_strcmp((ଅଚଳ ଅକ୍ଷର*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ ଅକ୍ଷର *replacement = data->subdivisionMap().get(subdivision.data());
    ଯଦି (replacement != nullptr) {
        ଅଚଳ ଅକ୍ଷର* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        ଯଦି (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            ଯଦି (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    int32_t len = transformedExtensions.length();
    ଅଚଳ ଅକ୍ଷର* str = transformedExtensions.data();
    ଅଚଳ ଅକ୍ଷର* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    ଯଦି (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        T_CString_toLowerCase(output.data());
    }
    ଯଦି (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        କର {
            ଅଚଳ ଅକ୍ଷର* tvalue = uprv_strchr(tkey, '-');
            ଯଦି (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            ଅଚଳ ଅକ୍ଷର* nextTKey = ultag_getTKeyStart(tvalue);
            ଯଦି (nextTKey != nullptr) {
                *const_cast<ଅକ୍ଷର*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.ଭର୍ତ୍ତି_Element_At((ଶୂନ୍ୟ*)tkey, tfields.size(), status);
            ଯଦି (U_FAILURE(status)) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            tkey = nextTKey;
        } ଯେତେବେଳେ (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            ଫେରନ୍ତୁ uprv_strcmp((ଅଚଳ ଅକ୍ଷର*)e1.pointer, (ଅଚଳ ଅକ୍ଷର*)e2.pointer);
        }, status);
        ପାଇଁ (int32_t i = 0; i < tfields.size(); i++) {
             ଯଦି (output.length() > 0) {
                 output.append('-', status);
             }
             ଅଚଳ ଅକ୍ଷର* tfield = static_cast<ଅଚଳ ଅକ୍ଷର*>(tfields.elementAt(i));
             ଅଚଳ ଅକ୍ଷର* tvalue = uprv_strchr(tfield, '-');
             ଯଦି (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 ଫେରନ୍ତୁ ମିଥ୍ୟା;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<ଅକ୍ଷର*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ out; }
    out.append(language, status);
    ଯଦି (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    ଯଦି (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    ଯଦି (variants.size() > 0) {
        ଯଦି (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            ଫେରନ୍ତୁ uprv_strcmp((ଅଚଳ ଅକ୍ଷର*)e1.pointer, (ଅଚଳ ଅକ୍ଷର*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        ପାଇଁ (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<ଅଚଳ ଅକ୍ଷର*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    ଯଦି (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    ଫେରନ୍ତୁ out;
}

ତାର୍କିକ
AliasReplacer::replace(ଅଚଳ Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    U_ASSERT(data != nullptr);
    out.clear();
    language = locale.getLanguage();
    ଯଦି (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    ଯଦି (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    ଯଦି (!notEmpty(region)) {
        region = nullptr;
    }
    ଅଚଳ ଅକ୍ଷର* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    ଯଦି (!variantsBuff.isEmpty()) {
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
        ଅକ୍ଷର* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        ଅକ୍ଷର* end;
        ଯେତେବେଳେ ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            ଯଦି (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        ଯଦି (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        ଫେରନ୍ତୁ uprv_strcmp((ଅଚଳ ଅକ୍ଷର*)e1.pointer, (ଅଚଳ ଅକ୍ଷର*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    ଅଚଳ int32_t maxChanged = 4 + variants.size();
    (ଶୂନ୍ୟ)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](ଶୂନ୍ୟ *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    ଯେତେବେଳେ (U_SUCCESS(status)) {
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
        ଯଦି (    replaceLanguage(ସତ୍ୟ, ସତ୍ୟ,  ସତ୍ୟ,  stringsToBeFreed, status) ||
                replaceLanguage(ସତ୍ୟ, ସତ୍ୟ,  ମିଥ୍ୟା, stringsToBeFreed, status) ||
                replaceLanguage(ସତ୍ୟ, ମିଥ୍ୟା, ସତ୍ୟ,  stringsToBeFreed, status) ||
                replaceLanguage(ସତ୍ୟ, ମିଥ୍ୟା, ମିଥ୍ୟା, stringsToBeFreed, status) ||
                replaceLanguage(ମିଥ୍ୟା,ମିଥ୍ୟା, ସତ୍ୟ,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            ଜାରି;
        }
        // Nothing changed. Break out.
        ଭାଙ୍ଗ;
    }  // while(1)

    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ଅଚଳ ଅକ୍ଷର* extensionsStr = locale_getKeywordsStart(locale.getName());
    ଯଦି (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    outputToString(out, status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        ଯଦି (U_SUCCESS(status) && !iter.isNull()) {
            ଅଚଳ ଅକ୍ଷର* ଚାବି;
            ଯେତେବେଳେ ((ଚାବି = iter->next(nullptr, status)) != nullptr) {
                ଯଦି (uprv_strcmp("sd", ଚାବି) == 0 || uprv_strcmp("rg", ଚାବି) == 0 ||
                        uprv_strcmp("t", ଚାବି) == 0) {
                    auto value = locale.get_Keyword_ମୂଲ୍ୟ<CharString>(ଚାବି, status);
                    ଯଦି (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        ଜାରି;
                    }
                    CharString replacement;
                    ଯଦି (uprv_strlen(ଚାବି) == 2) {
                        ଯଦି (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.set_Keyword_ମୂଲ୍ୟ(ଚାବି, replacement.data(), status);
                        }
                    } ଅନ୍ୟଥା {
                        U_ASSERT(uprv_strcmp(ଚାବି, "t") == 0);
                        ଯଦି (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.set_Keyword_ମୂଲ୍ୟ(ଚାବି, replacement.data(), status);
                        }
                    }
                    ଯଦି (U_FAILURE(status)) {
                        ଫେରନ୍ତୁ ମିଥ୍ୟା;
                    }
                }
            }
        }
        ଯଦି (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    // If the tag is not changed, return.
    ଯଦି (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
ତାର୍କିକ
canonicalizeLocale(ଅଚଳ Locale& locale, CharString& out, UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    AliasReplacer replacer(status);
    ଫେରନ୍ତୁ replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
ତାର୍କିକ
isKnownCanonicalizedLocale(ଅଚଳ ଅକ୍ଷର* locale, UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }

    ଯଦି (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    ଫେରନ୍ତୁ uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT ଅଚଳ ଅକ୍ଷର* ଅଚଳ*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    ଫେରନ୍ତୁ KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT ତାର୍କିକ
ulocimp_isCanonicalizedLocaleForTest(ଅଚଳ ଅକ୍ଷର* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    ଫେରନ୍ତୁ !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ଅଚଳ ଅକ୍ଷର* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    ଫେରନ୍ତୁ localeID == nullptr ? *ଏହି = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    କର {
        ଅକ୍ଷର *separator;
        ଅକ୍ଷର *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        ଅଚଳ auto parse = [canonicalize](std::string_view localeID,
                                          ଅକ୍ଷର* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    ଯଦି (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } ଅନ୍ୟଥା {
                        ulocimp_getName(localeID, sink, status);
                    }
                },
                status);
        };

        // "canonicalize" the locale ID to ICU/Java format
        ଅକ୍ଷର* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, ଆକାର Nest::baseName, err);

        FixedString fullNameBuffer;
        ଯଦି (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(ଆକାର Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            ଯଦି (!fullNameBuffer.reserve(length + 1)) {
                ଭାଙ୍ଗ; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        ଯଦି(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            ଭାଙ୍ଗ;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        ଅକ୍ଷର* at = uprv_strchr(fullName, '@');
        ଯେତେବେଳେ ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        ଅକ୍ଷର* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        ଯଦି (separator!=nullptr || sep2!=nullptr) {
            ଯଦି (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } ଅନ୍ୟଥା {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        ତାର୍କିକ hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        ଯଦି (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            ଭାଙ୍ଗ; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        ଯଦି (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        ଯଦି (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        ଯଦି (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } ଅନ୍ୟଥା ଯଦି (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        ଯଦି (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } ଅନ୍ୟଥା ଯଦି (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        ଯଦି (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } ଅନ୍ୟଥା {
            ଯଦି (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                ଯଦି (fullNameBuffer.isEmpty()) {
                    ଭାଙ୍ଗ; // error: out of memory
                }
                ଯଦି (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                ଯଦି (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                ଯଦି (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            ଯଦି (isBogus()) {
                ଭାଙ୍ଗ; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            ଯଦି (hasKeywords) {
                ଯଦି (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    ଯଦି (heap.ptr->baseName.isEmpty()) {
                        ଭାଙ୍ଗ; // error: out of memory
                    }
                }
            }
        }

        ଯଦି (canonicalize) {
            ଯଦି (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                ଯଦି (canonicalizeLocale(*ଏହି, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), ମିଥ୍ୟା);
                }
                ଯଦି (U_FAILURE(err)) {
                    ଭାଙ୍ଗ;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        ଫେରନ୍ତୁ *ଏହି;
    } ଯେତେବେଳେ(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    ଫେରନ୍ତୁ *ଏହି;
}

int32_t
Locale::hashCode() ଅଚଳ
{
    ଫେରନ୍ତୁ ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

ଶୂନ୍ୟ
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

ଅଚଳ Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        ଯଦି (gDefaultLocale != nullptr) {
            ଫେରନ୍ତୁ *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    ଫେରନ୍ତୁ *locale_set_default_internal(nullptr, status);
}



ଶୂନ୍ୟ U_EXPORT2
Locale::setDefault( ଅଚଳ   Locale&     newLocale,
                            UErrorCode&  status)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ଅଚଳ ଅକ୍ଷର *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

ଶୂନ୍ୟ
Locale::addLikelySubtags(UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    ଯଦି (U_FAILURE(status)) {
        ଯଦି (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ଫେରନ୍ତୁ;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/ମିଥ୍ୟା);
    ଯଦି (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ଶୂନ୍ୟ
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(ମିଥ୍ୟା, status);
}
ଶୂନ୍ୟ
Locale::minimizeSubtags(ତାର୍କିକ favorScript, UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    ଯଦି (U_FAILURE(status)) {
        ଯଦି (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ଫେରନ୍ତୁ;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/ମିଥ୍ୟା);
    ଯଦି (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ଶୂନ୍ୟ
Locale::canonicalize(UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    CharString uncanonicalized(getName(), status);
    ଯଦି (U_FAILURE(status)) {
        ଯଦି (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ଫେରନ୍ତୁ;
    }
    init(uncanonicalized.data(), /*canonicalize=*/ସତ୍ୟ);
    ଯଦି (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ result;
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

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ result;
    }

    ଯଦି (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ result;
    }

    result.init(localeID.data(), /*canonicalize=*/ମିଥ୍ୟା);
    ଯଦି (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) ଅଚଳ
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/ମିଥ୍ୟା, status);
}

Locale U_EXPORT2
Locale::createFromName (ଅଚଳ ଅକ୍ଷର *name)
{
    ଯଦି (name) {
        Locale l("");
        l.init(name, ମିଥ୍ୟା);
        ଫେରନ୍ତୁ l;
    }
    ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, ମିଥ୍ୟା);
    ଫେରନ୍ତୁ loc;
}

Locale U_EXPORT2
Locale::createCanonical(ଅଚଳ ଅକ୍ଷର* name) {
    Locale loc("");
    loc.init(name, ସତ୍ୟ);
    ଫେରନ୍ତୁ loc;
}

ଅଚଳ ଅକ୍ଷର *
Locale::getISO3Language() ଅଚଳ
{
    ଫେରନ୍ତୁ uloc_getISO3Language(getName());
}


ଅଚଳ ଅକ୍ଷର *
Locale::getISO3Country() ଅଚଳ
{
    ଫେରନ୍ତୁ uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() ଅଚଳ
{
    ଫେରନ୍ତୁ uloc_getLCID(getName());
}

ଅଚଳ ଅକ୍ଷର* ଅଚଳ* U_EXPORT2 Locale::getISOCountries()
{
    ଫେରନ୍ତୁ uloc_getISOCountries();
}

ଅଚଳ ଅକ୍ଷର* ଅଚଳ* U_EXPORT2 Locale::getISOLanguages()
{
    ଫେରନ୍ତୁ uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
ଶୂନ୍ୟ Locale::setFromPOSIXID(ଅଚଳ ଅକ୍ଷର *posixID)
{
    init(posixID, ସତ୍ୟ);
}

ଅଚଳ Locale & U_EXPORT2
Locale::get_ମୂଳ()
{
    ଫେରନ୍ତୁ getLocale(e_ମୂଳ);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getEnglish()
{
    ଫେରନ୍ତୁ getLocale(eENGLISH);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getFrench()
{
    ଫେରନ୍ତୁ getLocale(eFRENCH);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getGerman()
{
    ଫେରନ୍ତୁ getLocale(eGERMAN);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getItalian()
{
    ଫେରନ୍ତୁ getLocale(eITALIAN);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getJapanese()
{
    ଫେରନ୍ତୁ getLocale(eJAPANESE);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getKorean()
{
    ଫେରନ୍ତୁ getLocale(eKOREAN);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getChinese()
{
    ଫେରନ୍ତୁ getLocale(eCHINESE);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    ଫେରନ୍ତୁ getLocale(eCHINA);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    ଫେରନ୍ତୁ getLocale(eTAIWAN);
}


ଅଚଳ Locale & U_EXPORT2
Locale::getFrance()
{
    ଫେରନ୍ତୁ getLocale(eFRANCE);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getGermany()
{
    ଫେରନ୍ତୁ getLocale(eGERMANY);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getItaly()
{
    ଫେରନ୍ତୁ getLocale(eITALY);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getJapan()
{
    ଫେରନ୍ତୁ getLocale(eJAPAN);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getKorea()
{
    ଫେରନ୍ତୁ getLocale(eKOREA);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getChina()
{
    ଫେରନ୍ତୁ getLocale(eCHINA);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getPRC()
{
    ଫେରନ୍ତୁ getLocale(eCHINA);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getTaiwan()
{
    ଫେରନ୍ତୁ getLocale(eTAIWAN);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getUK()
{
    ଫେରନ୍ତୁ getLocale(eUK);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getUS()
{
    ଫେରନ୍ତୁ getLocale(eUS);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getCanada()
{
    ଫେରନ୍ତୁ getLocale(eCANADA);
}

ଅଚଳ Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    ଫେରନ୍ତୁ getLocale(eCANADA_FRENCH);
}

ଅଚଳ Locale &
Locale::getLocale(ପୂର୍ଣ୍ଣସଂଖ୍ୟା locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    ଯଦି (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    ଫେରନ୍ତୁ localeCache[locid]; /*operating on nullptr*/
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
    ଫେରନ୍ତୁ gLocaleCache;
}

ଶ୍ରେଣୀ KeywordEnumeration : ସର୍ବସାଧାରଣ StringEnumeration {
ସୁରକ୍ଷିତ:
    FixedString keywords;
ବ୍ୟକ୍ତିଗତ:
    int32_t length;
    ଅଚଳ ଅକ୍ଷର *current;
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର fgClassID;

ସର୍ବସାଧାରଣ:
    ସ୍ଥିର UClassID U_EXPORT2 getStaticClassID() { ଫେରନ୍ତୁ (UClassID)&fgClassID; }
    ଆଭାସୀ UClassID getDynamicClassID() ଅଚଳ override { ଫେରନ୍ତୁ getStaticClassID(); }
ସର୍ବସାଧାରଣ:
    KeywordEnumeration(ଅଚଳ ଅକ୍ଷର *ଚାବିଗୁଡ଼ିକ, int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        ଯଦି(U_SUCCESS(status) && keywordLen != 0) {
            ଯଦି(ଚାବିଗୁଡ଼ିକ == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } ଅନ୍ୟଥା {
                keywords = {ଚାବିଗୁଡ଼ିକ, static_cast<std::string_view::size_type>(length)};
                ଯଦି (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ଅନ୍ୟଥା {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    ଆଭାସୀ ~KeywordEnumeration();

    ଆଭାସୀ StringEnumeration * clone() ଅଚଳ override
    {
        UErrorCode status = U_ZERO_ERROR;
        ଫେରନ୍ତୁ ନୂତନ KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    ଆଭାସୀ int32_t count(UErrorCode& status) ଅଚଳ override {
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ 0; }
        ଅଚଳ ଅକ୍ଷର *kw = keywords.data();
        int32_t result = 0;
        ଯେତେବେଳେ(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        ଫେରନ୍ତୁ result;
    }

    ଆଭାସୀ ଅଚଳ ଅକ୍ଷର* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ଅଚଳ ଅକ୍ଷର* result;
        int32_t len;
        ଯଦି(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            ଯଦି(resultLength != nullptr) {
                *resultLength = len;
            }
        } ଅନ୍ୟଥା {
            ଯଦି(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        ଫେରନ୍ତୁ result;
    }

    ଆଭାସୀ ଅଚଳ UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ nullptr; }
        int32_t resultLength = 0;
        ଅଚଳ ଅକ୍ଷର *s = next(&resultLength, status);
        ଫେରନ୍ତୁ setChars(s, resultLength, status);
    }

    ଆଭାସୀ ଶୂନ୍ୟ reset(UErrorCode& status) override {
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
        current = keywords.data();
    }
};

ଅଚଳ ଅକ୍ଷର KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
ଶ୍ରେଣୀ UnicodeKeywordEnumeration : ସର୍ବସାଧାରଣ KeywordEnumeration {
ସର୍ବସାଧାରଣ:
    using KeywordEnumeration::KeywordEnumeration;
    ଆଭାସୀ ~UnicodeKeywordEnumeration();

    ଆଭାସୀ ଅଚଳ ଅକ୍ଷର* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        ଅଚଳ ଅକ୍ଷର* legacy_ଚାବି = KeywordEnumeration::next(nullptr, status);
        ଯେତେବେଳେ (U_SUCCESS(status) && legacy_ଚାବି != nullptr) {
            ଅଚଳ ଅକ୍ଷର* ଚାବି = uloc_to_Unicode_Locale_ଚାବି(legacy_ଚାବି);
            ଯଦି (ଚାବି != nullptr) {
                ଯଦି (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(ଚାବି));
                }
                ଫେରନ୍ତୁ ଚାବି;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_ଚାବି = KeywordEnumeration::next(nullptr, status);
        }
        ଯଦି (resultLength != nullptr) *resultLength = 0;
        ଫେରନ୍ତୁ nullptr;
    }
    ଆଭାସୀ int32_t count(UErrorCode& status) ଅଚଳ override {
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ 0; }
        ଅଚଳ ଅକ୍ଷର *kw = keywords.data();
        int32_t result = 0;
        ଯେତେବେଳେ(*kw) {
            ଯଦି (uloc_to_Unicode_Locale_ଚାବି(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        ଫେରନ୍ତୁ result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) ଅଚଳ
{
    StringEnumeration *result = nullptr;

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ result;
    }

    ଅଚଳ ଅକ୍ଷର* variantStart = uprv_strchr(getName(), '@');
    ଅଚଳ ଅକ୍ଷର* assignment = uprv_strchr(getName(), '=');
    ଯଦି(variantStart) {
        ଯଦି(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', ମିଥ୍ୟା, status);
            ଯଦି (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = ନୂତନ KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                ଯଦି (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } ଅନ୍ୟଥା {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ଫେରନ୍ତୁ result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) ଅଚଳ
{
    StringEnumeration *result = nullptr;

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ result;
    }

    ଅଚଳ ଅକ୍ଷର* variantStart = uprv_strchr(getName(), '@');
    ଅଚଳ ଅକ୍ଷର* assignment = uprv_strchr(getName(), '=');
    ଯଦି(variantStart) {
        ଯଦି(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', ମିଥ୍ୟା, status);
            ଯଦି (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = ନୂତନ UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                ଯଦି (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } ଅନ୍ୟଥା {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    ଫେରନ୍ତୁ result;
}

int32_t
Locale::get_Keyword_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର* keywordName, ଅକ୍ଷର *buffer, int32_t bufLen, UErrorCode &status) ଅଚଳ
{
    ଫେରନ୍ତୁ uloc_get_Keyword_ମୂଲ୍ୟ(getName(), keywordName, buffer, bufLen, &status);
}

ଶୂନ୍ୟ
Locale::get_Keyword_ମୂଲ୍ୟ(StringPiece keywordName, ByteSink& sink, UErrorCode& status) ଅଚଳ {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    ଯଦି (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    ulocimp_get_Keyword_ମୂଲ୍ୟ(getName(), keywordName, sink, status);
}

ଶୂନ୍ୟ
Locale::get_Unicode_Keyword_ମୂଲ୍ୟ(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) ଅଚଳ {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    std::optional<std::string_view> legacy_ଚାବି = ulocimp_to_Legacy_ଚାବି_With_Fallback(keywordName);
    ଯଦି (!legacy_ଚାବି.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    auto legacy_ମୂଲ୍ୟ = get_Keyword_ମୂଲ୍ୟ<CharString>(*legacy_ଚାବି, status);

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    std::optional<std::string_view> unicode_ମୂଲ୍ୟ =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_ମୂଲ୍ୟ.toStringPiece());
    ଯଦି (!unicode_ମୂଲ୍ୟ.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    sink.Append(unicode_ମୂଲ୍ୟ->data(), static_cast<int32_t>(unicode_ମୂଲ୍ୟ->size()));
}

ଶୂନ୍ୟ
Locale::set_Keyword_ମୂଲ୍ୟ(StringPiece keywordName,
                        StringPiece keyword_ମୂଲ୍ୟ,
                        UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
    ଯଦି (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    ଯଦି (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_set_Keyword_ମୂଲ୍ୟ(keywordName, keyword_ମୂଲ୍ୟ, localeID, status);
    ଯଦି (U_FAILURE(status)) {
        ଯଦି (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        ଫେରନ୍ତୁ;
    }

    ଅଚଳ ଅକ୍ଷର* at = locale_getKeywordsStart(localeID.toStringPiece());
    ତାର୍କିକ hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    ଯଦି (!hasKeywords) {
        ଯଦି (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            ଯଦି (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < ଆକାର Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, ଆକାର Nest::baseName, status);
            } ଅନ୍ୟଥା {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                ଯଦି (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ଫେରନ୍ତୁ;
                }
            }
        }
    } ଅନ୍ୟଥା {
        Heap* heap = nullptr;
        ଯଦି (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            ଯଦି (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ;
            }
        } ଅନ୍ୟଥା {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        ଯଦି (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            ଫେରନ୍ତୁ;
        }

        ଯଦି (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            ଯଦି (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                ଯଦି (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    ଫେରନ୍ତୁ;
                }
            }
        }
    }
}

ଶୂନ୍ୟ
Locale::set_Unicode_Keyword_ମୂଲ୍ୟ(StringPiece keywordName,
                               StringPiece keyword_ମୂଲ୍ୟ,
                               UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    std::optional<std::string_view> legacy_ଚାବି = ulocimp_to_Legacy_ଚାବି_With_Fallback(keywordName);
    ଯଦି (!legacy_ଚାବି.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    std::string_view value;

    ଯଦି (!keyword_ମୂଲ୍ୟ.empty()) {
        std::optional<std::string_view> legacy_ମୂଲ୍ୟ =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_ମୂଲ୍ୟ);
        ଯଦି (!legacy_ମୂଲ୍ୟ.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ;
        }
        value = *legacy_ମୂଲ୍ୟ;
    }

    set_Keyword_ମୂଲ୍ୟ(*legacy_ଚାବି, value, status);
}

ଅଚଳ ଅକ୍ଷର*
Locale::getCountry() ଅଚଳ U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ getField<&Nest::getRegion, &Heap::getRegion>();
}

ଅଚଳ ଅକ୍ଷର*
Locale::getLanguage() ଅଚଳ U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ getField<&Nest::getLanguage, &Heap::getLanguage>();
}

ଅଚଳ ଅକ୍ଷର*
Locale::getScript() ଅଚଳ U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ getField<&Nest::getScript, &Heap::getScript>();
}

ଅଚଳ ଅକ୍ଷର*
Locale::getVariant() ଅଚଳ U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ getField<&Nest::getVariant, &Heap::getVariant>();
}

ଅଚଳ ଅକ୍ଷର*
Locale::getName() ଅଚଳ U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ getField<&Nest::getBaseName, &Heap::getFullName>();
}

ଅଚଳ ଅକ୍ଷର*
Locale::getBaseName() ଅଚଳ U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ getField<&Nest::getBaseName, &Heap::getBaseName>();
}

template <ଅଚଳ ଅକ୍ଷର* (Locale::Nest::*ଅଚଳ NEST)() ଅଚଳ,
          ଅଚଳ ଅକ୍ଷର* (Locale::Heap::*ଅଚଳ HEAP)() ଅଚଳ>
ଅଚଳ ଅକ୍ଷର* Locale::getField() ଅଚଳ U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ payload.visit([] { ଫେରନ୍ତୁ ""; },
                         [](ଅଚଳ Nest& nest) { ଫେରନ୍ତୁ (nest.*NEST)(); },
                         [](ଅଚଳ Heap& heap) { ଫେରନ୍ତୁ (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

//eof
U_NAMESPACE_END
