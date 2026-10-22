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

#അന്തര്ഭാവ <cstddef>
#അന്തര്ഭാവ <optional>
#അന്തര്ഭാവ <string_view>
#അന്തര്ഭാവ <type_traits>
#അന്തര്ഭാവ <utility>

#അന്തര്ഭാവ "unicode/bytestream.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/localebuilder.h"
#അന്തര്ഭാവ "unicode/localpointer.h"
#അന്തര്ഭാവ "unicode/strenum.h"
#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "unicode/ures.h"

#അന്തര്ഭാവ "bytesinkutil.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "charstrmap.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "fixedstring.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "ulocimp.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "uniquecharstr.h"
#അന്തര്ഭാവ "ustr_imp.h"
#അന്തര്ഭാവ "uvector.h"

#എങ്കിൽ UPRV_HAS_FEATURE(memory_sanitizer)
#അന്തര്ഭാവ <sanitizer/msan_interface.h>
#endif

U_NAMESPACE_BEGIN

സ്ഥിതം Locale   *gLocaleCache = nullptr;
സ്ഥിതം UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
സ്ഥിതം UMutex gDefaultLocaleMutex;
സ്ഥിതം UHashtable *gDefaultLocalesHashT = nullptr;
സ്ഥിതം Locale *gDefaultLocale = nullptr;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#define ULOC_STRING_LIMIT 357913941

U_NAMESPACE_END

തരനിർവചനം എണ്ണൽ ELocalePos {
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
    e_വേര്,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

നാമാകാശ {

//
// Deleter function for Locales owned by the default Locale hash table/
//
ശൂന്യം U_CALLCONV
deleteLocale(ശൂന്യം *obj) {
    delete static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    delete [] gLocaleCache;
    gLocaleCache = nullptr;
    gLocaleCacheInitOnce.reset();

    എങ്കിൽ (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = nullptr;
    }
    gDefaultLocale = nullptr;
    തിരികെ സത്യം;
}

ശൂന്യം U_CALLCONV locale_init(UErrorCode &status) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == nullptr);
    gLocaleCache = പുതിയ Locale[static_cast<പൂർണ്ണസംഖ്യ>(eMAX_LOCALES)];
    എങ്കിൽ (gLocaleCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_വേര്]          = Locale("");
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

Locale *locale_set_default_internal(സ്ഥിരം അക്ഷരം *id, UErrorCode& status) {
    // Synchronize this entire function.
    Mutex lock(&gDefaultLocaleMutex);

    UBool canonicalize = അസത്യം;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    എങ്കിൽ (id == nullptr) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = സത്യം; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, status) : ulocimp_getName(id, status);

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ gDefaultLocale;
    }

    എങ്കിൽ (gDefaultLocalesHashT == nullptr) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status);
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ gDefaultLocale;
        }
        uhash_set_മൂല്യം_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_registerCleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.data()));
    എങ്കിൽ (newDefault == nullptr) {
        newDefault = പുതിയ Locale(Locale::eBOGUS);
        എങ്കിൽ (newDefault == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ gDefaultLocale;
        }
        newDefault->init(localeNameBuf.data(), അസത്യം);
        uhash_put(gDefaultLocalesHashT, const_cast<അക്ഷരം*>(newDefault->getName()), newDefault, &status);
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    തിരികെ gDefaultLocale;
}

U_NAMESPACE_END

/* sfb 07/21/99 */
U_CFUNC ശൂന്യം
locale_set_default(സ്ഥിരം അക്ഷരം *id)
{
    U_NAMESPACE_USE
    UErrorCode status = U_ZERO_ERROR;
    locale_set_default_internal(id, status);
}
/* end */

U_CFUNC സ്ഥിരം അക്ഷരം *
locale_get_default()
{
    U_NAMESPACE_USE
    തിരികെ Locale::getDefault().getName();
}

നാമാകാശ {

template <auto FIELD, typename T>
ശൂന്യം copyToArray(std::string_view sv, T* that) {
    auto& field = that->*FIELD;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(field)>>;
    static_assert(capacity > 0);
    എങ്കിൽ (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(field, sv.data(), sv.size());
    }
    field[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

ശൂന്യം Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copyToArray<&Nest::language>(language, ഇത്);
    copyToArray<&Nest::script>(script, ഇത്);
    copyToArray<&Nest::region>(region, ഇത്);
    ഇത്->variantBegin = variantBegin;
}

Locale::Nest::Nest(Heap&& heap, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(Heap, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(Heap, region));
    U_ASSERT(ഇത് == reinterpret_cast<Nest*>(&heap));
#എങ്കിൽ UPRV_HAS_FEATURE(memory_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, വലുപ്പം(Heap));
#endif
    copyToArray<&Nest::script>(heap.script, ഇത്);
    copyToArray<&Nest::region>(heap.region, ഇത്);
    ഇത്->variantBegin = variantBegin;
    *ഇത്->baseName = '\0';
}

ഘടന Locale::Heap::Alloc : പൊതു UMemory {
    FixedString fullName;
    FixedString baseName;
    int32_t variantBegin;

    സ്ഥിരം അക്ഷരം* getVariant() സ്ഥിരം U_LIFETIME_BOUND {
        തിരികെ variantBegin == 0 ? "" : getBaseName() + variantBegin;
    }
    സ്ഥിരം അക്ഷരം* getFullName() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ fullName.data(); }
    സ്ഥിരം അക്ഷരം* getBaseName() സ്ഥിരം U_LIFETIME_BOUND {
        എങ്കിൽ (baseName.isEmpty()) {
            എങ്കിൽ (സ്ഥിരം അക്ഷരം* name = fullName.data(); *name != '@') {
                തിരികെ name;
            }
        }
        തിരികെ baseName.data();
    }

    Alloc(int32_t variantBegin) : fullName(), baseName(), variantBegin(variantBegin) {}

    Alloc(സ്ഥിരം Alloc& other, UErrorCode& status)
        : fullName(), baseName(), variantBegin(other.variantBegin) {
        എങ്കിൽ (U_SUCCESS(status)) {
            എങ്കിൽ (!other.fullName.isEmpty()) {
                fullName = other.fullName;
                എങ്കിൽ (fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } അല്ലെങ്കിൽ {
                    എങ്കിൽ (!other.baseName.isEmpty()) {
                        baseName = other.baseName;
                        എങ്കിൽ (baseName.isEmpty()) {
                            status = U_MEMORY_ALLOCATION_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = delete;

    ~Alloc() = സ്ഥിരസ്ഥിതി;
};

സ്ഥിരം അക്ഷരം* Locale::Heap::getVariant() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ ptr->getVariant(); }
സ്ഥിരം അക്ഷരം* Locale::Heap::getFullName() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ ptr->getFullName(); }
സ്ഥിരം അക്ഷരം* Locale::Heap::getBaseName() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ ptr->getBaseName(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = പുതിയ Alloc(variantBegin);
    എങ്കിൽ (ptr == nullptr) {
        type = eBOGUS;
    } അല്ലെങ്കിൽ {
        type = eHEAP;
        copyToArray<&Heap::language>(language, ഇത്);
        copyToArray<&Heap::script>(script, ഇത്);
        copyToArray<&Heap::region>(region, ഇത്);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    delete ptr;
}

Locale::Heap& Locale::Heap::operator=(സ്ഥിരം Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UErrorCode status = U_ZERO_ERROR;
    ptr = പുതിയ Alloc(*other.ptr, status);
    എങ്കിൽ (ptr == nullptr || U_FAILURE(status)) {
        delete ptr;
    } അല്ലെങ്കിൽ {
        type = eHEAP;
        uprv_memcpy(language, other.language, വലുപ്പം language);
        uprv_memcpy(script, other.script, വലുപ്പം script);
        uprv_memcpy(region, other.region, വലുപ്പം region);
    }
    തിരികെ *ഇത്;
}

Locale::Heap& Locale::Heap::operator=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, വലുപ്പം language);
    uprv_memcpy(script, other.script, വലുപ്പം script);
    uprv_memcpy(region, other.region, വലുപ്പം region);
    തിരികെ *ഇത്;
}

template <typename BogusFn, typename NestFn, typename HeapFn, typename... Args>
auto Locale::Payload::visit(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, Args... args) സ്ഥിരം {
    തിരഞ്ഞെടുപ്പ് (type) {
        അവസ്ഥ eBOGUS:
            തിരികെ bogusFn(args...);
        അവസ്ഥ eNEST:
            തിരികെ nestFn(nest, args...);
        അവസ്ഥ eHEAP:
            തിരികെ heapFn(heap, args...);
        സ്ഥിരസ്ഥിതി:
            UPRV_UNREACHABLE_EXIT;
    };
}

ശൂന്യം Locale::Payload::copy(സ്ഥിരം Payload& other) {
    other.visit([](Payload*) {},
                [](സ്ഥിരം Nest& nest, Payload* dst) { dst->nest = nest; },
                [](സ്ഥിരം Heap& heap, Payload* dst) { dst->heap = heap; },
                ഇത്);
}

ശൂന്യം Locale::Payload::move(Payload&& other) noexcept {
    other.visit(
        [](Payload*) {},
        [](സ്ഥിരം Nest& nest, Payload* dst) { dst->nest = nest; },
        [](സ്ഥിരം Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        ഇത്);
}

Locale::Payload::~Payload() {
    എങ്കിൽ (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(സ്ഥിരം Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::operator=(സ്ഥിരം Payload& other) U_LIFETIME_BOUND {
    എങ്കിൽ (ഇത് != &other) {
        setToBogus();
        copy(other);
    }
    തിരികെ *ഇത്;
}

Locale::Payload& Locale::Payload::operator=(Payload&& other) noexcept U_LIFETIME_BOUND {
    എങ്കിൽ (ഇത് != &other) {
        setToBogus();
        move(std::move(other));
    }
    തിരികെ *ഇത്;
}

ശൂന്യം Locale::Payload::setToBogus() {
    ഇത്->~Payload();
    type = eBOGUS;
}

template <typename T, typename... Args> T& Locale::Payload::emplace(Args&&... args) U_LIFETIME_BOUND {
    എങ്കിൽ constexpr (std::is_same_v<T, Nest>) {
        ഇത്->~Payload();
        ::പുതിയ (&nest) Nest(std::forward<Args>(args)...);
        തിരികെ nest;
    }
    എങ്കിൽ constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::പുതിയ (&heap) Heap(std::forward<Args>(args)...);
        തിരികെ heap;
    }
}

template <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    തിരികെ type == eNEST ? &nest : nullptr;
}
template <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    തിരികെ type == eHEAP ? &heap : nullptr;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#define SEP_CHAR '_'
#define NULL_CHAR '\0'

Locale::~Locale() = സ്ഥിരസ്ഥിതി;

Locale::Locale()
    : UObject(), payload()
{
    init(nullptr, അസത്യം);
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


Locale::Locale( സ്ഥിരം   അക്ഷരം * newLanguage,
                സ്ഥിരം   അക്ഷരം * newCountry,
                സ്ഥിരം   അക്ഷരം * newVariant,
                സ്ഥിരം   അക്ഷരം * newKeywords)
    : UObject(), payload()
{
    എങ്കിൽ( (newLanguage==nullptr) && (newCountry == nullptr) && (newVariant == nullptr) )
    {
        init(nullptr, അസത്യം); /* shortcut */
    }
    അല്ലെങ്കിൽ
    {
        UErrorCode status = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        എങ്കിൽ ( newLanguage != nullptr )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            എങ്കിൽ ( lsize < 0 || lsize > ULOC_STRING_LIMIT ) { // int32 wrap
                തിരികെ;
            }
        }

        CharString togo(newLanguage, lsize, status); // start with newLanguage

        // _Country
        എങ്കിൽ ( newCountry != nullptr )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            എങ്കിൽ ( csize < 0 || csize > ULOC_STRING_LIMIT ) { // int32 wrap
                തിരികെ;
            }
        }

        // _Variant
        എങ്കിൽ ( newVariant != nullptr )
        {
            // remove leading _'s
            ആകുമ്പോൾ(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            എങ്കിൽ ( vsize < 0 || vsize > ULOC_STRING_LIMIT ) { // int32 wrap
                തിരികെ;
            }
            ആകുമ്പോൾ( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        എങ്കിൽ ( newKeywords != nullptr)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            എങ്കിൽ ( ksize < 0 || ksize > ULOC_STRING_LIMIT ) {
              തിരികെ;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        എങ്കിൽ ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, status);
        }

        എങ്കിൽ ( csize != 0 )
        {
            togo.append(newCountry, status);
        }

        എങ്കിൽ ( vsize != 0)
        {
            togo.append(SEP_CHAR, status)
                .append(newVariant, vsize, status);
        }

        എങ്കിൽ ( ksize != 0)
        {
            എങ്കിൽ (uprv_strchr(newKeywords, '=')) {
                togo.append('@', status); /* keyword parsing */
            }
            അല്ലെങ്കിൽ {
                togo.append('_', status); /* Variant parsing with a script */
                എങ്കിൽ ( vsize == 0) {
                    togo.append('_', status); /* No country found */
                }
            }
            togo.append(newKeywords, status);
        }

        എങ്കിൽ (U_FAILURE(status)) {
            // Something went wrong with appending, etc.
            തിരികെ;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.data(), അസത്യം);
    }
}

Locale::Locale(സ്ഥിരം Locale&) = സ്ഥിരസ്ഥിതി;
Locale::Locale(Locale&&) noexcept = സ്ഥിരസ്ഥിതി;

Locale& Locale::operator=(സ്ഥിരം Locale&) U_LIFETIME_BOUND = സ്ഥിരസ്ഥിതി;
Locale& Locale::operator=(Locale&&) noexcept U_LIFETIME_BOUND = സ്ഥിരസ്ഥിതി;

Locale *
Locale::clone() സ്ഥിരം {
    തിരികെ പുതിയ Locale(*ഇത്);
}

തർക്കം
Locale::operator==( സ്ഥിരം   Locale& other) സ്ഥിരം
{
    തിരികെ uprv_strcmp(other.getName(), getName()) == 0;
}

നാമാകാശ {

UInitOnce gKnownCanonicalizedInitOnce {};
UHashtable *gKnownCanonicalized = nullptr;

constexpr സ്ഥിരം അക്ഷരം* KNOWN_CANONICALIZED[] = {
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
    എങ്കിൽ (gKnownCanonicalized) { uhash_close(gKnownCanonicalized); }
    തിരികെ സത്യം;
}

ശൂന്യം U_CALLCONV loadKnownCanonicalized(UErrorCode &status) {
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KNOWN_CANONICALIZED,
                                cleanupKnownCanonicalized);
    LocalUHashtablePointer newKnownCanonicalizedMap(
        uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &status));
    വേണ്ടി (int32_t i = 0;
            U_SUCCESS(status) && i < UPRV_LENGTHOF(KNOWN_CANONICALIZED);
            i++) {
        uhash_puti(newKnownCanonicalizedMap.getAlias(),
                   (ശൂന്യം*)KNOWN_CANONICALIZED[i],
                   1, &status);
    }
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    gKnownCanonicalized = newKnownCanonicalizedMap.orphan();
}

വർഗ്ഗം AliasData;

/**
 * A Builder class to build the alias data.
 */
വർഗ്ഗം AliasDataBuilder {
പൊതു:
    AliasDataBuilder() {
    }

    // Build the AliasData from resource.
    AliasData* build(UErrorCode &status);

സ്വകാര്യ:
    ശൂന്യം readAlias(UResourceBundle* alias,
                   UniqueCharStrings* strings,
                   LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
                   LocalMemory<int32_t>& replacementIndexes,
                   int32_t &length,
                   ശൂന്യം (*checkType)(സ്ഥിരം അക്ഷരം* type),
                   ശൂന്യം (*checkReplacement)(സ്ഥിരം UChar* replacement),
                   UErrorCode &status);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ശൂന്യം readLanguageAlias(UResourceBundle* alias,
                           UniqueCharStrings* strings,
                           LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
                           LocalMemory<int32_t>& replacementIndexes,
                           int32_t &length,
                           UErrorCode &status);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ശൂന്യം readScriptAlias(UResourceBundle* alias,
                         UniqueCharStrings* strings,
                         LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
                         LocalMemory<int32_t>& replacementIndexes,
                         int32_t &length, UErrorCode &status);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    ശൂന്യം readTerritoryAlias(UResourceBundle* alias,
                            UniqueCharStrings* strings,
                            LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
                            LocalMemory<int32_t>& replacementIndexes,
                            int32_t &length, UErrorCode &status);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    ശൂന്യം readVariantAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    ശൂന്യം readSubdivisionAlias(UResourceBundle* alias,
                          UniqueCharStrings* strings,
                          LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
                          LocalMemory<int32_t>& replacementIndexes,
                          int32_t &length, UErrorCode &status);
};

/**
 * A class to hold the Alias Data.
 */
വർഗ്ഗം AliasData : പൊതു UMemory {
പൊതു:
    സ്ഥിതം സ്ഥിരം AliasData* singleton(UErrorCode& status) {
        എങ്കിൽ (U_FAILURE(status)) {
            // Do not get into loadData if the status already has error.
            തിരികെ nullptr;
        }
        umtx_initOnce(AliasData::gInitOnce, &AliasData::loadData, status);
        തിരികെ gSingleton;
    }

    സ്ഥിരം CharStringMap& languageMap() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ language; }
    സ്ഥിരം CharStringMap& scriptMap() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ script; }
    സ്ഥിരം CharStringMap& territoryMap() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ territory; }
    സ്ഥിരം CharStringMap& variantMap() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ variant; }
    സ്ഥിരം CharStringMap& subdivisionMap() സ്ഥിരം U_LIFETIME_BOUND { തിരികെ subdivision; }

    സ്ഥിതം ശൂന്യം U_CALLCONV loadData(UErrorCode &status);
    സ്ഥിതം UBool U_CALLCONV cleanup();

    സ്ഥിതം UInitOnce gInitOnce;

സ്വകാര്യ:
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

    സ്ഥിതം സ്ഥിരം AliasData* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* strings;

    friend വർഗ്ഗം AliasDataBuilder;
};


സ്ഥിരം AliasData* AliasData::gSingleton = nullptr;
UInitOnce AliasData::gInitOnce {};

UBool U_CALLCONV
AliasData::cleanup()
{
    gInitOnce.reset();
    delete gSingleton;
    തിരികെ സത്യം;
}

ശൂന്യം
AliasDataBuilder::readAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        ശൂന്യം (*checkType)(സ്ഥിരം അക്ഷരം* type),
        ശൂന്യം (*checkReplacement)(സ്ഥിരം UChar* replacement),
        UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    length = ures_getSize(alias);
    സ്ഥിരം അക്ഷരം** rawTypes = types.allocateInsteadAndCopy(length);
    എങ്കിൽ (rawTypes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    എങ്കിൽ (rawIndexes == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; U_SUCCESS(status) && ures_hasNext(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_getNextResource(alias, nullptr, &status));
        സ്ഥിരം അക്ഷരം* aliasFrom = ures_get_താക്കോൽ(res.getAlias());
        സ്ഥിരം UChar* aliasTo =
            ures_get_String_By_താക്കോൽ(res.getAlias(), "replacement", nullptr, &status);
        എങ്കിൽ (U_FAILURE(status)) തിരികെ;

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
ശൂന്യം
AliasDataBuilder::readLanguageAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    തിരികെ readAlias(
        alias, strings, types, replacementIndexes, length,
#എങ്കിൽ U_DEBUG
        [](സ്ഥിരം അക്ഷരം* type) {
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
#അല്ലെങ്കിൽ
        [](സ്ഥിരം അക്ഷരം*) {},
#endif
        [](സ്ഥിരം UChar*) {}, status);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
ശൂന്യം
AliasDataBuilder::readScriptAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    തിരികെ readAlias(
        alias, strings, types, replacementIndexes, length,
#എങ്കിൽ U_DEBUG
        [](സ്ഥിരം അക്ഷരം* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](സ്ഥിരം UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#അല്ലെങ്കിൽ
        [](സ്ഥിരം അക്ഷരം*) {},
        [](സ്ഥിരം UChar*) { },
#endif
        status);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
ശൂന്യം
AliasDataBuilder::readTerritoryAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    തിരികെ readAlias(
        alias, strings, types, replacementIndexes, length,
#എങ്കിൽ U_DEBUG
        [](സ്ഥിരം അക്ഷരം* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#അല്ലെങ്കിൽ
        [](സ്ഥിരം അക്ഷരം*) {},
#endif
        [](സ്ഥിരം UChar*) { },
        status);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
ശൂന്യം
AliasDataBuilder::readVariantAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    തിരികെ readAlias(
        alias, strings, types, replacementIndexes, length,
#എങ്കിൽ U_DEBUG
        [](സ്ഥിരം അക്ഷരം* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](സ്ഥിരം UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#അല്ലെങ്കിൽ
        [](സ്ഥിരം അക്ഷരം*) {},
        [](സ്ഥിരം UChar*) { },
#endif
        status);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
ശൂന്യം
AliasDataBuilder::readSubdivisionAlias(
        UResourceBundle* alias,
        UniqueCharStrings* strings,
        LocalMemory<സ്ഥിരം അക്ഷരം*>& types,
        LocalMemory<int32_t>& replacementIndexes,
        int32_t &length,
        UErrorCode &status)
{
    തിരികെ readAlias(
        alias, strings, types, replacementIndexes, length,
#എങ്കിൽ U_DEBUG
        [](സ്ഥിരം അക്ഷരം* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#അല്ലെങ്കിൽ
        [](സ്ഥിരം അക്ഷരം*) {},
#endif
        [](സ്ഥിരം UChar*) { },
        status);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
ശൂന്യം U_CALLCONV
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
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ nullptr; }

    LocalUResourceBundlePointer metadata(
        ures_openDirect(nullptr, "metadata", &status));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_താക്കോൽ(metadata.getAlias(), "alias", nullptr, &status));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_താക്കോൽ(metadataAlias.getAlias(), "language", nullptr, &status));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_താക്കോൽ(metadataAlias.getAlias(), "script", nullptr, &status));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_താക്കോൽ(metadataAlias.getAlias(), "territory", nullptr, &status));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_താക്കോൽ(metadataAlias.getAlias(), "variant", nullptr, &status));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_താക്കോൽ(metadataAlias.getAlias(), "subdivision", nullptr, &status));

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    int32_t languagesLength = 0, scriptLength = 0, territoryLength = 0,
            variantLength = 0, subdivisionLength = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    UniqueCharStrings strings(status);
    LocalMemory<സ്ഥിരം അക്ഷരം*> languageTypes;
    LocalMemory<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &strings,
                      languageTypes,
                      languageReplacementIndexes,
                      languagesLength,
                      status);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    LocalMemory<സ്ഥിരം അക്ഷരം*> scriptTypes;
    LocalMemory<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &strings,
                    scriptTypes,
                    scriptReplacementIndexes,
                    scriptLength,
                    status);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    LocalMemory<സ്ഥിരം അക്ഷരം*> territoryTypes;
    LocalMemory<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &strings,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territoryLength, status);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    LocalMemory<സ്ഥിരം അക്ഷരം*> variantTypes;
    LocalMemory<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &strings,
                     variantTypes,
                     variantReplacementIndexes,
                     variantLength, status);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    LocalMemory<സ്ഥിരം അക്ഷരം*> subdivisionTypes;
    LocalMemory<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &strings,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivisionLength, status);

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    // We can only use strings after freeze it.
    strings.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, status);
    വേണ്ടി (int32_t i = 0; U_SUCCESS(status) && i < languagesLength; i++) {
        languageMap.put(languageTypes[i],
                        strings.get(languageReplacementIndexes[i]),
                        status);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, status);
    വേണ്ടി (int32_t i = 0; U_SUCCESS(status) && i < scriptLength; i++) {
        scriptMap.put(scriptTypes[i],
                      strings.get(scriptReplacementIndexes[i]),
                      status);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, status);
    വേണ്ടി (int32_t i = 0; U_SUCCESS(status) && i < territoryLength; i++) {
        territoryMap.put(territoryTypes[i],
                         strings.get(territoryReplacementIndexes[i]),
                         status);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, status);
    വേണ്ടി (int32_t i = 0; U_SUCCESS(status) && i < variantLength; i++) {
        variantMap.put(variantTypes[i],
                       strings.get(variantReplacementIndexes[i]),
                       status);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, status);
    വേണ്ടി (int32_t i = 0; U_SUCCESS(status) && i < subdivisionLength; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       strings.get(subdivisionReplacementIndexes[i]),
                       status);
    }

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    // copy hashtables
    auto *data = പുതിയ AliasData(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        strings.orphanCharStrings());

    എങ്കിൽ (data == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    തിരികെ data;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
വർഗ്ഗം AliasReplacer {
പൊതു:
    AliasReplacer(UErrorCode& status) :
            language(nullptr), script(nullptr), region(nullptr),
            extensions(nullptr),
            // store value in variants only once
            variants(nullptr,
                     ([](UElement e1, UElement e2) -> UBool {
                       തിരികെ 0==uprv_strcmp((സ്ഥിരം അക്ഷരം*)e1.pointer,
                                             (സ്ഥിരം അക്ഷരം*)e2.pointer);}),
                     status),
            data(nullptr) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    തർക്കം replace(
        സ്ഥിരം Locale& locale, CharString& out, UErrorCode& status);

സ്വകാര്യ:
    സ്ഥിരം അക്ഷരം* language;
    സ്ഥിരം അക്ഷരം* script;
    സ്ഥിരം അക്ഷരം* region;
    സ്ഥിരം അക്ഷരം* extensions;
    UVector variants;

    സ്ഥിരം AliasData* data;

    വരിയിൽ തർക്കം notEmpty(സ്ഥിരം അക്ഷരം* str) {
        തിരികെ str && str[0] != NULL_CHAR;
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
    വരിയിൽ സ്ഥിരം അക്ഷരം* deleteOrReplace(സ്ഥിരം അക്ഷരം* input U_LIFETIME_BOUND,
                                       സ്ഥിരം അക്ഷരം* type,
                                       സ്ഥിരം അക്ഷരം* replacement U_LIFETIME_BOUND) {
        തിരികെ notEmpty(replacement) ?
            ((input == nullptr) ?  replacement : input) :
            ((type == nullptr) ? input  : nullptr);
    }

    വരിയിൽ തർക്കം same(സ്ഥിരം അക്ഷരം* a, സ്ഥിരം അക്ഷരം* b) {
        എങ്കിൽ (a == nullptr && b == nullptr) {
            തിരികെ സത്യം;
        }
        എങ്കിൽ ((a == nullptr && b != nullptr) ||
            (a != nullptr && b == nullptr)) {
          തിരികെ അസത്യം;
        }
        തിരികെ uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& outputToString(CharString& out U_LIFETIME_BOUND, UErrorCode& status);

    // Generate the lookup key.
    CharString& generate_താക്കോൽ(സ്ഥിരം അക്ഷരം* language, സ്ഥിരം അക്ഷരം* region,
                            സ്ഥിരം അക്ഷരം* variant, CharString& out U_LIFETIME_BOUND,
                            UErrorCode& status);

    ശൂന്യം parseLanguageReplacement(സ്ഥിരം അക്ഷരം* replacement,
                                  സ്ഥിരം അക്ഷരം*& replaceLanguage,
                                  സ്ഥിരം അക്ഷരം*& replaceScript,
                                  സ്ഥിരം അക്ഷരം*& replaceRegion,
                                  സ്ഥിരം അക്ഷരം*& replaceVariant,
                                  സ്ഥിരം അക്ഷരം*& replaceExtensions,
                                  UVector& toBeFreed,
                                  UErrorCode& status);

    // Replace by using languageAlias.
    തർക്കം replaceLanguage(തർക്കം checkLanguage, തർക്കം checkRegion,
                         തർക്കം checkVariants, UVector& toBeFreed,
                         UErrorCode& status);

    // Replace by using territoryAlias.
    തർക്കം replaceTerritory(UVector& toBeFreed, UErrorCode& status);

    // Replace by using scriptAlias.
    തർക്കം replaceScript(UErrorCode& status);

    // Replace by using variantAlias.
    തർക്കം replaceVariant(UErrorCode& status);

    // Replace by using subdivisionAlias.
    തർക്കം replaceSubdivision(StringPiece subdivision,
                            CharString& output, UErrorCode& status);

    // Replace transformed extensions.
    തർക്കം replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UErrorCode& status);
};

CharString&
AliasReplacer::generate_താക്കോൽ(
        സ്ഥിരം അക്ഷരം* language, സ്ഥിരം അക്ഷരം* region, സ്ഥിരം അക്ഷരം* variant,
        CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ out; }
    out.append(language, status);
    എങ്കിൽ (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    എങ്കിൽ (notEmpty(variant)) {
       out.append(SEP_CHAR, status)
           .append(variant, status);
    }
    തിരികെ out;
}

ശൂന്യം
AliasReplacer::parseLanguageReplacement(
    സ്ഥിരം അക്ഷരം* replacement,
    സ്ഥിരം അക്ഷരം*& replacedLanguage,
    സ്ഥിരം അക്ഷരം*& replacedScript,
    സ്ഥിരം അക്ഷരം*& replacedRegion,
    സ്ഥിരം അക്ഷരം*& replacedVariant,
    സ്ഥിരം അക്ഷരം*& replacedExtensions,
    UVector& toBeFreed,
    UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = nullptr;
    എങ്കിൽ (uprv_strchr(replacement, '_') == nullptr) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        തിരികെ;
    }
    // We have multiple field so we have to allocate and parse
    auto str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), status, status);
    അക്ഷരം* data = str->data();
    toBeFreed.adoptElement(str.release(), status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    replacedLanguage = (സ്ഥിരം അക്ഷരം*) data;
    അക്ഷരം* endOfField = uprv_strchr(data, '_');
    *endOfField = '\0'; // null terminiate it.
    endOfField++;
    സ്ഥിരം അക്ഷരം* start = endOfField;
    endOfField = const_cast<അക്ഷരം*>(uprv_strchr(start, '_'));
    size_t len = 0;
    എങ്കിൽ (endOfField == nullptr) {
        len = uprv_strlen(start);
    } അല്ലെങ്കിൽ {
        len = endOfField - start;
        *endOfField = '\0'; // null terminiate it.
    }
    എങ്കിൽ (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        എങ്കിൽ (endOfField == nullptr) {
            തിരികെ;
        }
        start = endOfField++;
        endOfField = const_cast<അക്ഷരം*>(uprv_strchr(start, '_'));
        എങ്കിൽ (endOfField == nullptr) {
            len = uprv_strlen(start);
        } അല്ലെങ്കിൽ {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    എങ്കിൽ (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        എങ്കിൽ (endOfField == nullptr) {
            തിരികെ;
        }
        start = endOfField++;
        endOfField = const_cast<അക്ഷരം*>(uprv_strchr(start, '_'));
        എങ്കിൽ (endOfField == nullptr) {
            len = uprv_strlen(start);
        } അല്ലെങ്കിൽ {
            len = endOfField - start;
            *endOfField = '\0'; // null terminiate it.
        }
    }
    എങ്കിൽ (len >= 4) {
        // Got a variant
        replacedVariant = start;
        എങ്കിൽ (endOfField == nullptr) {
            തിരികെ;
        }
        start = endOfField++;
    }
    replacedExtensions = start;
}

തർക്കം
AliasReplacer::replaceLanguage(
        തർക്കം checkLanguage, തർക്കം checkRegion,
        തർക്കം checkVariants, UVector& toBeFreed, UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (    (checkRegion && region == nullptr) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        തിരികെ അസത്യം;
    }
    int32_t variant_size = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    സ്ഥിരം അക്ഷരം* തിരയൽ_Language = checkLanguage ? language : "und";
    സ്ഥിരം അക്ഷരം* തിരയൽ_Region = checkRegion ? region : nullptr;
    സ്ഥിരം അക്ഷരം* തിരയൽ_Variant = nullptr;
    വേണ്ടി (int32_t variant_index = 0;
            variant_index < variant_size;
            variant_index++) {
        എങ്കിൽ (checkVariants) {
            U_ASSERT(variant_index < variant_size);
            തിരയൽ_Variant = static_cast<സ്ഥിരം അക്ഷരം*>(variants.elementAt(variant_index));
        }

        എങ്കിൽ (തിരയൽ_Variant != nullptr && uprv_strlen(തിരയൽ_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            തിരയൽ_Variant = nullptr;
        }
        CharString type_താക്കോൽ;
        generate_താക്കോൽ(തിരയൽ_Language, തിരയൽ_Region, തിരയൽ_Variant, type_താക്കോൽ,
                    status);
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ അസത്യം;
        }
        സ്ഥിരം അക്ഷരം *replacement = data->languageMap().get(type_താക്കോൽ.data());
        എങ്കിൽ (replacement == nullptr) {
            // Found no replacement data.
            തുടരുക;
        }

        സ്ഥിരം അക്ഷരം* replacedLanguage = nullptr;
        സ്ഥിരം അക്ഷരം* replacedScript = nullptr;
        സ്ഥിരം അക്ഷരം* replacedRegion = nullptr;
        സ്ഥിരം അക്ഷരം* replacedVariant = nullptr;
        സ്ഥിരം അക്ഷരം* replacedExtensions = nullptr;
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
        replacedRegion = deleteOrReplace(region, തിരയൽ_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            തിരയൽ_Variant, തിരയൽ_Variant, replacedVariant);

        എങ്കിൽ (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(തിരയൽ_Variant, replacedVariant) &&
                replacedExtensions == nullptr) {
            // Replacement produce no changes.
            തുടരുക;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        എങ്കിൽ (തിരയൽ_Variant != nullptr) {
            എങ്കിൽ (notEmpty(replacedVariant)) {
                variants.setElementAt((ശൂന്യം*)replacedVariant, variant_index);
            } അല്ലെങ്കിൽ {
                variants.removeElementAt(variant_index);
            }
        }
        എങ്കിൽ (replacedExtensions != nullptr) {
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
        തിരികെ സത്യം;
    }
    // Nothing changed by language alias data.
    തിരികെ അസത്യം;
}

തർക്കം
AliasReplacer::replaceTerritory(UVector& toBeFreed, UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (region == nullptr) {
        // No region to search.
        തിരികെ അസത്യം;
    }
    സ്ഥിരം അക്ഷരം *replacement = data->territoryMap().get(region);
    എങ്കിൽ (replacement == nullptr) {
        // Found no replacement data for this region.
        തിരികെ അസത്യം;
    }
    സ്ഥിരം അക്ഷരം* replacedRegion = replacement;
    സ്ഥിരം അക്ഷരം* firstSpace = uprv_strchr(replacement, ' ');
    എങ്കിൽ (firstSpace != nullptr) {
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
        സ്ഥിരം അക്ഷരം* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        എങ്കിൽ (likelyRegion != nullptr && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            സ്ഥിരം അക്ഷരം* foundInReplacement = uprv_strstr(replacement,
                                                         likelyRegion);
            എങ്കിൽ (foundInReplacement != nullptr) {
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
        എങ്കിൽ (!item && U_SUCCESS(status)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), status, status);
        }
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ അസത്യം; }
        replacedRegion = item->data();
        toBeFreed.adoptElement(item.release(), status);
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ അസത്യം; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    തിരികെ സത്യം;
}

തർക്കം
AliasReplacer::replaceScript(UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (script == nullptr) {
        // No script to search.
        തിരികെ അസത്യം;
    }
    സ്ഥിരം അക്ഷരം *replacement = data->scriptMap().get(script);
    എങ്കിൽ (replacement == nullptr) {
        // Found no replacement data for this script.
        തിരികെ അസത്യം;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    തിരികെ സത്യം;
}

തർക്കം
AliasReplacer::replaceVariant(UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    // Since we may have more than one variant, we need to loop through them.
    വേണ്ടി (int32_t i = 0; i < variants.size(); i++) {
        സ്ഥിരം അക്ഷരം* variant = static_cast<സ്ഥിരം അക്ഷരം*>(variants.elementAt(i));
        സ്ഥിരം അക്ഷരം *replacement = data->variantMap().get(variant);
        എങ്കിൽ (replacement == nullptr) {
            // Found no replacement data for this variant.
            തുടരുക;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        എങ്കിൽ (!same(variant, replacement)) {
            variants.setElementAt((ശൂന്യം*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            എങ്കിൽ (uprv_strcmp(variant, "heploc") == 0) {
                വേണ്ടി (int32_t j = 0; j < variants.size(); j++) {
                     എങ്കിൽ (uprv_strcmp((സ്ഥിരം അക്ഷരം*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            തിരികെ സത്യം;
        }
    }
    തിരികെ അസത്യം;
}

തർക്കം
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം അക്ഷരം *replacement = data->subdivisionMap().get(subdivision.data());
    എങ്കിൽ (replacement != nullptr) {
        സ്ഥിരം അക്ഷരം* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (firstSpace != nullptr) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        എങ്കിൽ (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), status);
            എങ്കിൽ (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, status);
            }
        }
        തിരികെ സത്യം;
    }
    തിരികെ അസത്യം;
}

തർക്കം
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UErrorCode& status)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    int32_t len = transformedExtensions.length();
    സ്ഥിരം അക്ഷരം* str = transformedExtensions.data();
    സ്ഥിരം അക്ഷരം* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == nullptr) ? len : static_cast<int32_t>((tkey - str - 1)));
    എങ്കിൽ (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .setLanguageTag(StringPiece(str, tlangLen))
            .build(status);
        tlang.canonicalize(status);
        output = tlang.toLanguageTag<CharString>(status);
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ അസത്യം;
        }
        T_CString_toLowerCase(output.data());
    }
    എങ്കിൽ (tkey != nullptr) {
        // We need to sort the tfields by tkey
        UVector tfields(status);
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ അസത്യം;
        }
        ചെയ്യുക {
            സ്ഥിരം അക്ഷരം* tvalue = uprv_strchr(tkey, '-');
            എങ്കിൽ (tvalue == nullptr) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
                തിരികെ അസത്യം;
            }
            സ്ഥിരം അക്ഷരം* nextTKey = ultag_getTKeyStart(tvalue);
            എങ്കിൽ (nextTKey != nullptr) {
                *const_cast<അക്ഷരം*>(nextTKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.തിരുകുക_Element_At((ശൂന്യം*)tkey, tfields.size(), status);
            എങ്കിൽ (U_FAILURE(status)) {
                തിരികെ അസത്യം;
            }
            tkey = nextTKey;
        } ആകുമ്പോൾ (tkey != nullptr);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            തിരികെ uprv_strcmp((സ്ഥിരം അക്ഷരം*)e1.pointer, (സ്ഥിരം അക്ഷരം*)e2.pointer);
        }, status);
        വേണ്ടി (int32_t i = 0; i < tfields.size(); i++) {
             എങ്കിൽ (output.length() > 0) {
                 output.append('-', status);
             }
             സ്ഥിരം അക്ഷരം* tfield = static_cast<സ്ഥിരം അക്ഷരം*>(tfields.elementAt(i));
             സ്ഥിരം അക്ഷരം* tvalue = uprv_strchr(tfield, '-');
             എങ്കിൽ (tvalue == nullptr) {
                 status = U_ILLEGAL_ARGUMENT_ERROR;
                 തിരികെ അസത്യം;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<അക്ഷരം*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, status).append('-', status);
             std::optional<std::string_view> bcpTValue = ulocimp_toBcpType(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, status);
        }
    }
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    തിരികെ സത്യം;
}

CharString&
AliasReplacer::outputToString(
    CharString& out U_LIFETIME_BOUND, UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ out; }
    out.append(language, status);
    എങ്കിൽ (notEmpty(script)) {
        out.append(SEP_CHAR, status)
            .append(script, status);
    }
    എങ്കിൽ (notEmpty(region)) {
        out.append(SEP_CHAR, status)
            .append(region, status);
    }
    എങ്കിൽ (variants.size() > 0) {
        എങ്കിൽ (!notEmpty(script) && !notEmpty(region)) {
          out.append(SEP_CHAR, status);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            തിരികെ uprv_strcmp((സ്ഥിരം അക്ഷരം*)e1.pointer, (സ്ഥിരം അക്ഷരം*)e2.pointer);
        }, status);
        int32_t variantsStart = out.length();
        വേണ്ടി (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, status)
                 .append(static_cast<സ്ഥിരം അക്ഷരം*>(variants.elementAt(i)),
                         status);
        }
        T_CString_toUpperCase(out.data() + variantsStart);
    }
    എങ്കിൽ (notEmpty(extensions)) {
        CharString tmp("und_", status);
        tmp.append(extensions, status);
        Locale tmpLocale(tmp.data());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, status);
    }
    തിരികെ out;
}

തർക്കം
AliasReplacer::replace(സ്ഥിരം Locale& locale, CharString& out, UErrorCode& status)
{
    data = AliasData::singleton(status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    U_ASSERT(data != nullptr);
    out.clear();
    language = locale.getLanguage();
    എങ്കിൽ (!notEmpty(language)) {
        language = nullptr;
    }
    script = locale.getScript();
    എങ്കിൽ (!notEmpty(script)) {
        script = nullptr;
    }
    region = locale.getCountry();
    എങ്കിൽ (!notEmpty(region)) {
        region = nullptr;
    }
    സ്ഥിരം അക്ഷരം* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, status);
    എങ്കിൽ (!variantsBuff.isEmpty()) {
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ അസത്യം; }
        അക്ഷരം* start = variantsBuff.data();
        T_CString_toLowerCase(start);
        അക്ഷരം* end;
        ആകുമ്പോൾ ((end = uprv_strchr(start, SEP_CHAR)) != nullptr &&
               U_SUCCESS(status)) {
            *end = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            എങ്കിൽ (*start && !variants.contains(start)) {
                variants.addElement(start, status);
            }
            start = end + 1;
        }
        // do not add "" or duplicate data to variants
        എങ്കിൽ (*start && !variants.contains(start)) {
            variants.addElement(start, status);
        }
    }
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ അസത്യം; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        തിരികെ uprv_strcmp((സ്ഥിരം അക്ഷരം*)e1.pointer, (സ്ഥിരം അക്ഷരം*)e2.pointer);
    }, status);

    // A changed count to assert when loop too many times.
    int32_t changed = 0;
    സ്ഥിരം int32_t maxChanged = 4 + variants.size();
    (ശൂന്യം)maxChanged; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector stringsToBeFreed([](ശൂന്യം *obj) { delete static_cast<CharString*>(obj); },
                             nullptr, 10, status);
    ആകുമ്പോൾ (U_SUCCESS(status)) {
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
        എങ്കിൽ (    replaceLanguage(സത്യം, സത്യം,  സത്യം,  stringsToBeFreed, status) ||
                replaceLanguage(സത്യം, സത്യം,  അസത്യം, stringsToBeFreed, status) ||
                replaceLanguage(സത്യം, അസത്യം, സത്യം,  stringsToBeFreed, status) ||
                replaceLanguage(സത്യം, അസത്യം, അസത്യം, stringsToBeFreed, status) ||
                replaceLanguage(അസത്യം,അസത്യം, സത്യം,  stringsToBeFreed, status) ||
                replaceTerritory(stringsToBeFreed, status) ||
                replaceScript(status) ||
                replaceVariant(status)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            changed++;
            തുടരുക;
        }
        // Nothing changed. Break out.
        നിർത്തുക;
    }  // while(1)

    എങ്കിൽ (U_FAILURE(status)) { തിരികെ അസത്യം; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    സ്ഥിരം അക്ഷരം* extensionsStr = locale_getKeywordsStart(locale.getName());
    എങ്കിൽ (changed == 0 && variants.size() <= 1 && extensionsStr == nullptr) {
        തിരികെ അസത്യം;
    }
    outputToString(out, status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (extensionsStr != nullptr) {
        changed = 0;
        Locale temp(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(status));
        എങ്കിൽ (U_SUCCESS(status) && !iter.isNull()) {
            സ്ഥിരം അക്ഷരം* താക്കോൽ;
            ആകുമ്പോൾ ((താക്കോൽ = iter->next(nullptr, status)) != nullptr) {
                എങ്കിൽ (uprv_strcmp("sd", താക്കോൽ) == 0 || uprv_strcmp("rg", താക്കോൽ) == 0 ||
                        uprv_strcmp("t", താക്കോൽ) == 0) {
                    auto value = locale.get_Keyword_മൂല്യം<CharString>(താക്കോൽ, status);
                    എങ്കിൽ (U_FAILURE(status)) {
                        status = U_ZERO_ERROR;
                        തുടരുക;
                    }
                    CharString replacement;
                    എങ്കിൽ (uprv_strlen(താക്കോൽ) == 2) {
                        എങ്കിൽ (replaceSubdivision(value.toStringPiece(), replacement, status)) {
                            changed++;
                            temp.set_Keyword_മൂല്യം(താക്കോൽ, replacement.data(), status);
                        }
                    } അല്ലെങ്കിൽ {
                        U_ASSERT(uprv_strcmp(താക്കോൽ, "t") == 0);
                        എങ്കിൽ (replaceTransformedExtensions(value, replacement, status)) {
                            changed++;
                            temp.set_Keyword_മൂല്യം(താക്കോൽ, replacement.data(), status);
                        }
                    }
                    എങ്കിൽ (U_FAILURE(status)) {
                        തിരികെ അസത്യം;
                    }
                }
            }
        }
        എങ്കിൽ (changed != 0) {
            extensionsStr = locale_getKeywordsStart(temp.getName());
        }
        out.append(extensionsStr, status);
    }
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    // If the tag is not changed, return.
    എങ്കിൽ (uprv_strcmp(out.data(), locale.getName()) == 0) {
        out.clear();
        തിരികെ അസത്യം;
    }
    തിരികെ സത്യം;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
തർക്കം
canonicalizeLocale(സ്ഥിരം Locale& locale, CharString& out, UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ അസത്യം; }
    AliasReplacer replacer(status);
    തിരികെ replacer.replace(locale, out, status);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
തർക്കം
isKnownCanonicalizedLocale(സ്ഥിരം അക്ഷരം* locale, UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ അസത്യം; }

    എങ്കിൽ (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        തിരികെ സത്യം;
    }

    // common well-known Canonicalized.
    umtx_initOnce(gKnownCanonicalizedInitOnce,
                  &loadKnownCanonicalized, status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    U_ASSERT(gKnownCanonicalized != nullptr);
    തിരികെ uhash_geti(gKnownCanonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_END

// Function for testing.
U_EXPORT സ്ഥിരം അക്ഷരം* സ്ഥിരം*
ulocimp_getKnownCanonicalizedLocaleForTest(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(KNOWN_CANONICALIZED);
    തിരികെ KNOWN_CANONICALIZED;
}

// Function for testing.
U_EXPORT തർക്കം
ulocimp_isCanonicalizedLocaleForTest(സ്ഥിരം അക്ഷരം* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UErrorCode status = U_ZERO_ERROR;
    CharString temp;
    തിരികെ !canonicalizeLocale(l, temp, status) && U_SUCCESS(status);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(സ്ഥിരം അക്ഷരം* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    തിരികെ localeID == nullptr ? *ഇത് = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    ചെയ്യുക {
        അക്ഷരം *separator;
        അക്ഷരം *field[5] = {nullptr};
        int32_t fieldLen[5] = {0};
        int32_t fieldIdx;
        int32_t variantField;
        int32_t length;
        UErrorCode err;

        സ്ഥിരം auto parse = [canonicalize](std::string_view localeID,
                                          അക്ഷരം* name,
                                          int32_t nameCapacity,
                                          UErrorCode& status) {
            തിരികെ ByteSinkUtil::viaByteSinkToTerminatedChars(
                name, nameCapacity,
                [&](ByteSink& sink, UErrorCode& status) {
                    എങ്കിൽ (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, status);
                    } അല്ലെങ്കിൽ {
                        ulocimp_getName(localeID, sink, status);
                    }
                },
                status);
        };

        // "canonicalize" the locale ID to ICU/Java format
        അക്ഷരം* fullName = nest.baseName;
        err = U_ZERO_ERROR;
        length = parse(localeID, fullName, വലുപ്പം Nest::baseName, err);

        FixedString fullNameBuffer;
        എങ്കിൽ (err == U_BUFFER_OVERFLOW_ERROR || length >= static_cast<int32_t>(വലുപ്പം Nest::baseName)) {
            /*Go to heap for the fullName if necessary*/
            എങ്കിൽ (!fullNameBuffer.reserve(length + 1)) {
                നിർത്തുക; // error: out of memory
            }
            fullName = fullNameBuffer.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, fullName, length + 1, err);
        }
        എങ്കിൽ(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            നിർത്തുക;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = field[0] = fullName;
        fieldIdx = 1;
        അക്ഷരം* at = uprv_strchr(fullName, '@');
        ആകുമ്പോൾ ((separator = uprv_strchr(field[fieldIdx-1], SEP_CHAR)) != nullptr &&
               fieldIdx < UPRV_LENGTHOF(field)-1 &&
               (at == nullptr || separator < at)) {
            field[fieldIdx] = separator + 1;
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
            fieldIdx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(field[fieldIdx-1], '@');
        അക്ഷരം* sep2 = uprv_strchr(field[fieldIdx-1], '.');
        എങ്കിൽ (separator!=nullptr || sep2!=nullptr) {
            എങ്കിൽ (separator==nullptr || (sep2!=nullptr && separator > sep2)) {
                separator = sep2;
            }
            fieldLen[fieldIdx - 1] = static_cast<int32_t>(separator - field[fieldIdx - 1]);
        } അല്ലെങ്കിൽ {
            fieldLen[fieldIdx - 1] = length - static_cast<int32_t>(field[fieldIdx - 1] - fullName);
        }
        തർക്കം hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

        എങ്കിൽ (fieldLen[0] >= ULOC_LANG_CAPACITY)
        {
            നിർത്തുക; // error: the language field is too long
        }

        variantField = 1; /* Usually the 2nd one, except when a script or country is also used. */
        എങ്കിൽ (fieldLen[0] > 0) {
            /* We have a language */
            language = {fullName, static_cast<std::string_view::size_type>(fieldLen[0])};
        }
        എങ്കിൽ (fieldLen[1] == 4 && uprv_isASCIILetter(field[1][0]) &&
                uprv_isASCIILetter(field[1][1]) && uprv_isASCIILetter(field[1][2]) &&
                uprv_isASCIILetter(field[1][3])) {
            /* We have at least a script */
            script = {field[1], static_cast<std::string_view::size_type>(fieldLen[1])};
            variantField++;
        }

        എങ്കിൽ (fieldLen[variantField] == 2 || fieldLen[variantField] == 3) {
            /* We have a country */
            region = {field[variantField], static_cast<std::string_view::size_type>(fieldLen[variantField])};
            variantField++;
        } അല്ലെങ്കിൽ എങ്കിൽ (fieldLen[variantField] == 0) {
            variantField++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        എങ്കിൽ (fieldLen[variantField] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(field[variantField] - fullName);
        } അല്ലെങ്കിൽ എങ്കിൽ (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - fullName);
        }

        എങ്കിൽ (!hasKeywords && Nest::fits(length, language, script, region)) {
            U_ASSERT(fullName == nest.baseName);
            U_ASSERT(fullNameBuffer.isEmpty());
            nest.init(language, script, region, variantBegin);
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (fullName == nest.baseName) {
                U_ASSERT(fullNameBuffer.isEmpty());
                fullNameBuffer = {fullName, static_cast<std::string_view::size_type>(length)};
                എങ്കിൽ (fullNameBuffer.isEmpty()) {
                    നിർത്തുക; // error: out of memory
                }
                എങ്കിൽ (!language.empty()) {
                    language = {fullNameBuffer.data(), language.size()};
                }
                എങ്കിൽ (!script.empty()) {
                    script = {fullNameBuffer.data() + (script.data() - fullName), script.size()};
                }
                എങ്കിൽ (!region.empty()) {
                    region = {fullNameBuffer.data() + (region.data() - fullName), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            എങ്കിൽ (isBogus()) {
                നിർത്തുക; // error: out of memory
            }
            U_ASSERT(!fullNameBuffer.isEmpty());
            heap.ptr->fullName = std::move(fullNameBuffer);
            എങ്കിൽ (hasKeywords) {
                എങ്കിൽ (std::string_view::size_type baseNameLength = at - fullName; baseNameLength > 0) {
                    heap.ptr->baseName = {heap.ptr->fullName.data(), baseNameLength};
                    എങ്കിൽ (heap.ptr->baseName.isEmpty()) {
                        നിർത്തുക; // error: out of memory
                    }
                }
            }
        }

        എങ്കിൽ (canonicalize) {
            എങ്കിൽ (!isKnownCanonicalizedLocale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                എങ്കിൽ (canonicalizeLocale(*ഇത്, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.data(), അസത്യം);
                }
                എങ്കിൽ (U_FAILURE(err)) {
                    നിർത്തുക;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        തിരികെ *ഇത്;
    } ആകുമ്പോൾ(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    setToBogus();

    തിരികെ *ഇത്;
}

int32_t
Locale::hashCode() സ്ഥിരം
{
    തിരികെ ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

ശൂന്യം
Locale::setToBogus() {
    /* Free our current storage */
    payload.setToBogus();
}

സ്ഥിരം Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex lock(&gDefaultLocaleMutex);
        എങ്കിൽ (gDefaultLocale != nullptr) {
            തിരികെ *gDefaultLocale;
        }
    }
    UErrorCode status = U_ZERO_ERROR;
    തിരികെ *locale_set_default_internal(nullptr, status);
}



ശൂന്യം U_EXPORT2
Locale::setDefault( സ്ഥിരം   Locale&     newLocale,
                            UErrorCode&  status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    സ്ഥിരം അക്ഷരം *localeID = newLocale.getName();
    locale_set_default_internal(localeID, status);
}

ശൂന്യം
Locale::addLikelySubtags(UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    CharString maximizedLocaleID = ulocimp_addLikelySubtags(getName(), status);

    എങ്കിൽ (U_FAILURE(status)) {
        എങ്കിൽ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        തിരികെ;
    }

    init(maximizedLocaleID.data(), /*canonicalize=*/അസത്യം);
    എങ്കിൽ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ശൂന്യം
Locale::minimizeSubtags(UErrorCode& status) {
    Locale::minimizeSubtags(അസത്യം, status);
}
ശൂന്യം
Locale::minimizeSubtags(തർക്കം favorScript, UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, status);

    എങ്കിൽ (U_FAILURE(status)) {
        എങ്കിൽ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        തിരികെ;
    }

    init(minimizedLocaleID.data(), /*canonicalize=*/അസത്യം);
    എങ്കിൽ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ശൂന്യം
Locale::canonicalize(UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    എങ്കിൽ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    CharString uncanonicalized(getName(), status);
    എങ്കിൽ (U_FAILURE(status)) {
        എങ്കിൽ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        തിരികെ;
    }
    init(uncanonicalized.data(), /*canonicalize=*/സത്യം);
    എങ്കിൽ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
}

Locale U_EXPORT2
Locale::forLanguageTag(StringPiece tag, UErrorCode& status)
{
    Locale result(Locale::eBOGUS);

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ result;
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

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ result;
    }

    എങ്കിൽ (parsedLength != tag.size()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ result;
    }

    result.init(localeID.data(), /*canonicalize=*/അസത്യം);
    എങ്കിൽ (result.isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    തിരികെ result;
}

ശൂന്യം
Locale::toLanguageTag(ByteSink& sink, UErrorCode& status) സ്ഥിരം
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    എങ്കിൽ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    ulocimp_toLanguageTag(getName(), sink, /*strict=*/അസത്യം, status);
}

Locale U_EXPORT2
Locale::createFromName (സ്ഥിരം അക്ഷരം *name)
{
    എങ്കിൽ (name) {
        Locale l("");
        l.init(name, അസത്യം);
        തിരികെ l;
    }
    അല്ലെങ്കിൽ {
        തിരികെ getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, അസത്യം);
    തിരികെ loc;
}

Locale U_EXPORT2
Locale::createCanonical(സ്ഥിരം അക്ഷരം* name) {
    Locale loc("");
    loc.init(name, സത്യം);
    തിരികെ loc;
}

സ്ഥിരം അക്ഷരം *
Locale::getISO3Language() സ്ഥിരം
{
    തിരികെ uloc_getISO3Language(getName());
}


സ്ഥിരം അക്ഷരം *
Locale::getISO3Country() സ്ഥിരം
{
    തിരികെ uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() സ്ഥിരം
{
    തിരികെ uloc_getLCID(getName());
}

സ്ഥിരം അക്ഷരം* സ്ഥിരം* U_EXPORT2 Locale::getISOCountries()
{
    തിരികെ uloc_getISOCountries();
}

സ്ഥിരം അക്ഷരം* സ്ഥിരം* U_EXPORT2 Locale::getISOLanguages()
{
    തിരികെ uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
ശൂന്യം Locale::setFromPOSIXID(സ്ഥിരം അക്ഷരം *posixID)
{
    init(posixID, സത്യം);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::get_വേര്()
{
    തിരികെ getLocale(e_വേര്);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getEnglish()
{
    തിരികെ getLocale(eENGLISH);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getFrench()
{
    തിരികെ getLocale(eFRENCH);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getGerman()
{
    തിരികെ getLocale(eGERMAN);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getItalian()
{
    തിരികെ getLocale(eITALIAN);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getJapanese()
{
    തിരികെ getLocale(eJAPANESE);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getKorean()
{
    തിരികെ getLocale(eKOREAN);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getChinese()
{
    തിരികെ getLocale(eCHINESE);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    തിരികെ getLocale(eCHINA);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    തിരികെ getLocale(eTAIWAN);
}


സ്ഥിരം Locale & U_EXPORT2
Locale::getFrance()
{
    തിരികെ getLocale(eFRANCE);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getGermany()
{
    തിരികെ getLocale(eGERMANY);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getItaly()
{
    തിരികെ getLocale(eITALY);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getJapan()
{
    തിരികെ getLocale(eJAPAN);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getKorea()
{
    തിരികെ getLocale(eKOREA);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getChina()
{
    തിരികെ getLocale(eCHINA);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getPRC()
{
    തിരികെ getLocale(eCHINA);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getTaiwan()
{
    തിരികെ getLocale(eTAIWAN);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getUK()
{
    തിരികെ getLocale(eUK);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getUS()
{
    തിരികെ getLocale(eUS);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getCanada()
{
    തിരികെ getLocale(eCANADA);
}

സ്ഥിരം Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    തിരികെ getLocale(eCANADA_FRENCH);
}

സ്ഥിരം Locale &
Locale::getLocale(പൂർണ്ണസംഖ്യ locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    എങ്കിൽ (localeCache == nullptr) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    തിരികെ localeCache[locid]; /*operating on nullptr*/
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
    തിരികെ gLocaleCache;
}

വർഗ്ഗം KeywordEnumeration : പൊതു StringEnumeration {
സംരക്ഷിത:
    FixedString keywords;
സ്വകാര്യ:
    int32_t length;
    സ്ഥിരം അക്ഷരം *current;
    സ്ഥിതം സ്ഥിരം അക്ഷരം fgClassID;

പൊതു:
    സ്ഥിതം UClassID U_EXPORT2 getStaticClassID() { തിരികെ (UClassID)&fgClassID; }
    മായാ UClassID getDynamicClassID() സ്ഥിരം override { തിരികെ getStaticClassID(); }
പൊതു:
    KeywordEnumeration(സ്ഥിരം അക്ഷരം *താക്കോലുകൾ, int32_t keywordLen, int32_t currentIndex, UErrorCode &status)
        : keywords(), length(keywordLen), current(nullptr) {
        എങ്കിൽ(U_SUCCESS(status) && keywordLen != 0) {
            എങ്കിൽ(താക്കോലുകൾ == nullptr || keywordLen < 0) {
                status = U_ILLEGAL_ARGUMENT_ERROR;
            } അല്ലെങ്കിൽ {
                keywords = {താക്കോലുകൾ, static_cast<std::string_view::size_type>(length)};
                എങ്കിൽ (keywords.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } അല്ലെങ്കിൽ {
                    current = keywords.data() + currentIndex;
                }
            }
        }
    }

    മായാ ~KeywordEnumeration();

    മായാ StringEnumeration * clone() സ്ഥിരം override
    {
        UErrorCode status = U_ZERO_ERROR;
        തിരികെ പുതിയ KeywordEnumeration(
                keywords.data(), length,
                static_cast<int32_t>(current - keywords.data()), status);
    }

    മായാ int32_t count(UErrorCode& status) സ്ഥിരം override {
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ 0; }
        സ്ഥിരം അക്ഷരം *kw = keywords.data();
        int32_t result = 0;
        ആകുമ്പോൾ(*kw) {
            result++;
            kw += uprv_strlen(kw)+1;
        }
        തിരികെ result;
    }

    മായാ സ്ഥിരം അക്ഷരം* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        സ്ഥിരം അക്ഷരം* result;
        int32_t len;
        എങ്കിൽ(U_SUCCESS(status) && *current != 0) {
            result = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            എങ്കിൽ(resultLength != nullptr) {
                *resultLength = len;
            }
        } അല്ലെങ്കിൽ {
            എങ്കിൽ(resultLength != nullptr) {
                *resultLength = 0;
            }
            result = nullptr;
        }
        തിരികെ result;
    }

    മായാ സ്ഥിരം UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ nullptr; }
        int32_t resultLength = 0;
        സ്ഥിരം അക്ഷരം *s = next(&resultLength, status);
        തിരികെ setChars(s, resultLength, status);
    }

    മായാ ശൂന്യം reset(UErrorCode& status) override {
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
        current = keywords.data();
    }
};

സ്ഥിരം അക്ഷരം KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = സ്ഥിരസ്ഥിതി;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
വർഗ്ഗം UnicodeKeywordEnumeration : പൊതു KeywordEnumeration {
പൊതു:
    using KeywordEnumeration::KeywordEnumeration;
    മായാ ~UnicodeKeywordEnumeration();

    മായാ സ്ഥിരം അക്ഷരം* next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND override {
        സ്ഥിരം അക്ഷരം* legacy_താക്കോൽ = KeywordEnumeration::next(nullptr, status);
        ആകുമ്പോൾ (U_SUCCESS(status) && legacy_താക്കോൽ != nullptr) {
            സ്ഥിരം അക്ഷരം* താക്കോൽ = uloc_to_Unicode_Locale_താക്കോൽ(legacy_താക്കോൽ);
            എങ്കിൽ (താക്കോൽ != nullptr) {
                എങ്കിൽ (resultLength != nullptr) {
                    *resultLength = static_cast<int32_t>(uprv_strlen(താക്കോൽ));
                }
                തിരികെ താക്കോൽ;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_താക്കോൽ = KeywordEnumeration::next(nullptr, status);
        }
        എങ്കിൽ (resultLength != nullptr) *resultLength = 0;
        തിരികെ nullptr;
    }
    മായാ int32_t count(UErrorCode& status) സ്ഥിരം override {
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ 0; }
        സ്ഥിരം അക്ഷരം *kw = keywords.data();
        int32_t result = 0;
        ആകുമ്പോൾ(*kw) {
            എങ്കിൽ (uloc_to_Unicode_Locale_താക്കോൽ(kw) != nullptr) {
                result++;
            }
            kw += uprv_strlen(kw)+1;
        }
        തിരികെ result;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = സ്ഥിരസ്ഥിതി;

StringEnumeration *
Locale::createKeywords(UErrorCode &status) സ്ഥിരം
{
    StringEnumeration *result = nullptr;

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ result;
    }

    സ്ഥിരം അക്ഷരം* variantStart = uprv_strchr(getName(), '@');
    സ്ഥിരം അക്ഷരം* assignment = uprv_strchr(getName(), '=');
    എങ്കിൽ(variantStart) {
        എങ്കിൽ(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', അസത്യം, status);
            എങ്കിൽ (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = പുതിയ KeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                എങ്കിൽ (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } അല്ലെങ്കിൽ {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    തിരികെ result;
}

StringEnumeration *
Locale::createUnicodeKeywords(UErrorCode &status) സ്ഥിരം
{
    StringEnumeration *result = nullptr;

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ result;
    }

    സ്ഥിരം അക്ഷരം* variantStart = uprv_strchr(getName(), '@');
    സ്ഥിരം അക്ഷരം* assignment = uprv_strchr(getName(), '=');
    എങ്കിൽ(variantStart) {
        എങ്കിൽ(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', അസത്യം, status);
            എങ്കിൽ (U_SUCCESS(status) && !keywords.isEmpty()) {
                result = പുതിയ UnicodeKeywordEnumeration(keywords.data(), keywords.length(), 0, status);
                എങ്കിൽ (!result) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                }
            }
        } അല്ലെങ്കിൽ {
            status = U_INVALID_FORMAT_ERROR;
        }
    }
    തിരികെ result;
}

int32_t
Locale::get_Keyword_മൂല്യം(സ്ഥിരം അക്ഷരം* keywordName, അക്ഷരം *buffer, int32_t bufLen, UErrorCode &status) സ്ഥിരം
{
    തിരികെ uloc_get_Keyword_മൂല്യം(getName(), keywordName, buffer, bufLen, &status);
}

ശൂന്യം
Locale::get_Keyword_മൂല്യം(StringPiece keywordName, ByteSink& sink, UErrorCode& status) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    എങ്കിൽ (isBogus()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    ulocimp_get_Keyword_മൂല്യം(getName(), keywordName, sink, status);
}

ശൂന്യം
Locale::get_Unicode_Keyword_മൂല്യം(StringPiece keywordName,
                               ByteSink& sink,
                               UErrorCode& status) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    std::optional<std::string_view> legacy_താക്കോൽ = ulocimp_to_Legacy_താക്കോൽ_With_Fallback(keywordName);
    എങ്കിൽ (!legacy_താക്കോൽ.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    auto legacy_മൂല്യം = get_Keyword_മൂല്യം<CharString>(*legacy_താക്കോൽ, status);

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    std::optional<std::string_view> unicode_മൂല്യം =
        ulocimp_toBcpTypeWithFallback(keywordName, legacy_മൂല്യം.toStringPiece());
    എങ്കിൽ (!unicode_മൂല്യം.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    sink.Append(unicode_മൂല്യം->data(), static_cast<int32_t>(unicode_മൂല്യം->size()));
}

ശൂന്യം
Locale::set_Keyword_മൂല്യം(StringPiece keywordName,
                        StringPiece keyword_മൂല്യം,
                        UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
    എങ്കിൽ (keywordName.empty()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    എങ്കിൽ (status == U_STRING_NOT_TERMINATED_WARNING) {
        status = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, status);
    ulocimp_set_Keyword_മൂല്യം(keywordName, keyword_മൂല്യം, localeID, status);
    എങ്കിൽ (U_FAILURE(status)) {
        എങ്കിൽ (status == U_MEMORY_ALLOCATION_ERROR) {
            setToBogus();
        }
        തിരികെ;
    }

    സ്ഥിരം അക്ഷരം* at = locale_getKeywordsStart(localeID.toStringPiece());
    തർക്കം hasKeywords = at != nullptr && uprv_strchr(at + 1, '=') != nullptr;

    Nest* nest = payload.get<Nest>();
    എങ്കിൽ (!hasKeywords) {
        എങ്കിൽ (nest == nullptr) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != nullptr);
            എങ്കിൽ (Nest::fits(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < വലുപ്പം Nest::baseName);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->baseName, വലുപ്പം Nest::baseName, status);
            } അല്ലെങ്കിൽ {
                heap->ptr->baseName.clear();
                heap->ptr->fullName = localeID.toStringPiece();
                എങ്കിൽ (heap->ptr->fullName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    തിരികെ;
                }
            }
        }
    } അല്ലെങ്കിൽ {
        Heap* heap = nullptr;
        എങ്കിൽ (nest != nullptr) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            എങ്കിൽ (isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ;
            }
        } അല്ലെങ്കിൽ {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != nullptr);
        heap->ptr->fullName = localeID.toStringPiece();
        എങ്കിൽ (heap->ptr->fullName.isEmpty()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            setToBogus();
            തിരികെ;
        }

        എങ്കിൽ (heap->ptr->baseName.isEmpty()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            എങ്കിൽ (std::string_view::size_type baseNameLength = at - localeID.data(); baseNameLength > 0) {
                heap->ptr->baseName = {heap->ptr->fullName.data(), baseNameLength};
                എങ്കിൽ (heap->ptr->baseName.isEmpty()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    setToBogus();
                    തിരികെ;
                }
            }
        }
    }
}

ശൂന്യം
Locale::set_Unicode_Keyword_മൂല്യം(StringPiece keywordName,
                               StringPiece keyword_മൂല്യം,
                               UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    std::optional<std::string_view> legacy_താക്കോൽ = ulocimp_to_Legacy_താക്കോൽ_With_Fallback(keywordName);
    എങ്കിൽ (!legacy_താക്കോൽ.has_value()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    std::string_view value;

    എങ്കിൽ (!keyword_മൂല്യം.empty()) {
        std::optional<std::string_view> legacy_മൂല്യം =
            ulocimp_toLegacyTypeWithFallback(keywordName, keyword_മൂല്യം);
        എങ്കിൽ (!legacy_മൂല്യം.has_value()) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ;
        }
        value = *legacy_മൂല്യം;
    }

    set_Keyword_മൂല്യം(*legacy_താക്കോൽ, value, status);
}

സ്ഥിരം അക്ഷരം*
Locale::getCountry() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ getField<&Nest::getRegion, &Heap::getRegion>();
}

സ്ഥിരം അക്ഷരം*
Locale::getLanguage() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ getField<&Nest::getLanguage, &Heap::getLanguage>();
}

സ്ഥിരം അക്ഷരം*
Locale::getScript() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ getField<&Nest::getScript, &Heap::getScript>();
}

സ്ഥിരം അക്ഷരം*
Locale::getVariant() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ getField<&Nest::getVariant, &Heap::getVariant>();
}

സ്ഥിരം അക്ഷരം*
Locale::getName() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ getField<&Nest::getBaseName, &Heap::getFullName>();
}

സ്ഥിരം അക്ഷരം*
Locale::getBaseName() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ getField<&Nest::getBaseName, &Heap::getBaseName>();
}

template <സ്ഥിരം അക്ഷരം* (Locale::Nest::*സ്ഥിരം NEST)() സ്ഥിരം,
          സ്ഥിരം അക്ഷരം* (Locale::Heap::*സ്ഥിരം HEAP)() സ്ഥിരം>
സ്ഥിരം അക്ഷരം* Locale::getField() സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ payload.visit([] { തിരികെ ""; },
                         [](സ്ഥിരം Nest& nest) { തിരികെ (nest.*NEST)(); },
                         [](സ്ഥിരം Heap& heap) { തിരികെ (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = സ്ഥിരസ്ഥിതി;

//eof
U_NAMESPACE_END
