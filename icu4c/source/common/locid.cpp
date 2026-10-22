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

#यदि UPRV_HAS_FEATURE(स्मृतिः_sanitizer)
#अन्तर्भाव <sanitizer/msan_interface.h>
#यदि_अन्तः

U_NAMESPACE_BEGIN

स्थिर Locale   *gLocaleCache = शून्यसूचकः;
स्थिर UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
स्थिर UMutex gDefaultLocaleMutex;
स्थिर UHashtable *gDefaultLocalesHashT = शून्यसूचकः;
स्थिर Locale *gDefaultLocale = शून्यसूचकः;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#परिभाषय ULOC_STRING_सीमा 357913941

U_NAMESPACE_अन्तः

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
    विलोपय static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    विलोपय [] gLocaleCache;
    gLocaleCache = शून्यसूचकः;
    gLocaleCacheInitOnce.reset();

    यदि (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = शून्यसूचकः;
    }
    gDefaultLocale = शून्यसूचकः;
    प्रत्यागम सत्य;
}

शून्य U_CALLCONV locale_init(UError_संहिता &स्थितिः) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == शून्यसूचकः);
    gLocaleCache = नूतन Locale[static_cast<पूर्णाङ्क>(eMAX_LOCALES)];
    यदि (gLocaleCache == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
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

Locale *locale_समुच्चयः_default_आन्तरिकम्(ध्रुव वर्ण *id, UError_संहिता& स्थितिः) {
    // Synchronize this entire function.
    Mutex तालकम्(&gDefaultLocaleMutex);

    UBool canonicalize = असत्य;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    यदि (id == शून्यसूचकः) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = सत्य; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, स्थितिः) : ulocimp_getName(id, स्थितिः);

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम gDefaultLocale;
    }

    यदि (gDefaultLocalesHashT == शून्यसूचकः) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, शून्यसूचकः, &स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम gDefaultLocale;
        }
        uhash_समुच्चयः_मानम्_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.दत्त()));
    यदि (newDefault == शून्यसूचकः) {
        newDefault = नूतन Locale(Locale::eBOGUS);
        यदि (newDefault == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम gDefaultLocale;
        }
        newDefault->init(localeNameBuf.दत्त(), असत्य);
        uhash_put(gDefaultLocalesHashT, const_cast<वर्ण*>(newDefault->getName()), newDefault, &स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    प्रत्यागम gDefaultLocale;
}

U_NAMESPACE_अन्तः

/* sfb 07/21/99 */
U_CFUNC शून्य
locale_समुच्चयः_default(ध्रुव वर्ण *id)
{
    U_NAMESPACE_USE
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    locale_समुच्चयः_default_आन्तरिकम्(id, स्थितिः);
}
/*अन्तः*/

U_CFUNC ध्रुव वर्ण *
locale_get_default()
{
    U_NAMESPACE_USE
    प्रत्यागम Locale::getDefault().getName();
}

नामाकाश {

प्रतिरूपम् <स्वचालित क्षेत्रम्, प्रकारनाम T>
शून्य copy_गन्तव्यम्_Array(std::string_view sv, T* that) {
    स्वचालित& क्षेत्रम्_2 = that->*क्षेत्रम्;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(क्षेत्रम्_2)>>;
    static_assert(capacity > 0);
    यदि (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(क्षेत्रम्_2, sv.दत्त(), sv.size());
    }
    क्षेत्रम्_2[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

शून्य Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copy_गन्तव्यम्_Array<&Nest::language>(language, एतत्);
    copy_गन्तव्यम्_Array<&Nest::script>(script, एतत्);
    copy_गन्तव्यम्_Array<&Nest::region>(region, एतत्);
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
#यदि UPRV_HAS_FEATURE(स्मृतिः_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&heap, परिमाण(Heap));
#यदि_अन्तः
    copy_गन्तव्यम्_Array<&Nest::script>(heap.script, एतत्);
    copy_गन्तव्यम्_Array<&Nest::region>(heap.region, एतत्);
    एतत्->variantBegin = variantBegin;
    *एतत्->आधारः_Name = '\0';
}

संरचना Locale::Heap::Alloc : सार्वजनिक UMemory {
    FixedString पूर्णम्_Name;
    FixedString आधारः_Name;
    int32_t variantBegin;

    ध्रुव वर्ण* getVariant() ध्रुव U_LIFETIME_BOUND {
        प्रत्यागम variantBegin == 0 ? "" : get_आधारः_Name() + variantBegin;
    }
    ध्रुव वर्ण* get_पूर्णम्_Name() ध्रुव U_LIFETIME_BOUND { प्रत्यागम पूर्णम्_Name.दत्त(); }
    ध्रुव वर्ण* get_आधारः_Name() ध्रुव U_LIFETIME_BOUND {
        यदि (आधारः_Name.is_रिक्तानि()) {
            यदि (ध्रुव वर्ण* name = पूर्णम्_Name.दत्त(); *name != '@') {
                प्रत्यागम name;
            }
        }
        प्रत्यागम आधारः_Name.दत्त();
    }

    Alloc(int32_t variantBegin) : पूर्णम्_Name(), आधारः_Name(), variantBegin(variantBegin) {}

    Alloc(ध्रुव Alloc& other, UError_संहिता& स्थितिः)
        : पूर्णम्_Name(), आधारः_Name(), variantBegin(other.variantBegin) {
        यदि (U_SUCCESS(स्थितिः)) {
            यदि (!other.पूर्णम्_Name.is_रिक्तानि()) {
                पूर्णम्_Name = other.पूर्णम्_Name;
                यदि (पूर्णम्_Name.is_रिक्तानि()) {
                    स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                } अन्यथा {
                    यदि (!other.आधारः_Name.is_रिक्तानि()) {
                        आधारः_Name = other.आधारः_Name;
                        यदि (आधारः_Name.is_रिक्तानि()) {
                            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = विलोपय;

    ~Alloc() = स्वभाव;
};

ध्रुव वर्ण* Locale::Heap::getVariant() ध्रुव U_LIFETIME_BOUND { प्रत्यागम ptr->getVariant(); }
ध्रुव वर्ण* Locale::Heap::get_पूर्णम्_Name() ध्रुव U_LIFETIME_BOUND { प्रत्यागम ptr->get_पूर्णम्_Name(); }
ध्रुव वर्ण* Locale::Heap::get_आधारः_Name() ध्रुव U_LIFETIME_BOUND { प्रत्यागम ptr->get_आधारः_Name(); }

Locale::Heap::Heap(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = नूतन Alloc(variantBegin);
    यदि (ptr == शून्यसूचकः) {
        type = eBOGUS;
    } अन्यथा {
        type = eHEAP;
        copy_गन्तव्यम्_Array<&Heap::language>(language, एतत्);
        copy_गन्तव्यम्_Array<&Heap::script>(script, एतत्);
        copy_गन्तव्यम्_Array<&Heap::region>(region, एतत्);
    }
}

Locale::Heap::~Heap() {
    U_ASSERT(type == eHEAP);
    विलोपय ptr;
}

Locale::Heap& Locale::Heap::संक्रियकः=(ध्रुव Heap& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    ptr = नूतन Alloc(*other.ptr, स्थितिः);
    यदि (ptr == शून्यसूचकः || U_FAILURE(स्थितिः)) {
        विलोपय ptr;
    } अन्यथा {
        type = eHEAP;
        uprv_memcpy(language, other.language, परिमाण language);
        uprv_memcpy(script, other.script, परिमाण script);
        uprv_memcpy(region, other.region, परिमाण region);
    }
    प्रत्यागम *एतत्;
}

Locale::Heap& Locale::Heap::संक्रियकः=(Heap&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = eHEAP;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, परिमाण language);
    uprv_memcpy(script, other.script, परिमाण script);
    uprv_memcpy(region, other.region, परिमाण region);
    प्रत्यागम *एतत्;
}

प्रतिरूपम् <प्रकारनाम BogusFn, प्रकारनाम NestFn, प्रकारनाम HeapFn, प्रकारनाम... तर्काः>
स्वचालित Locale::Payload::दर्शय(BogusFn bogusFn, NestFn nestFn, HeapFn heapFn, तर्काः... तर्काः_2) ध्रुव {
    विकल्प (type) {
        पक्ष eBOGUS:
            प्रत्यागम bogusFn(तर्काः_2...);
        पक्ष eNEST:
            प्रत्यागम nestFn(nest, तर्काः_2...);
        पक्ष eHEAP:
            प्रत्यागम heapFn(heap, तर्काः_2...);
        स्वभाव:
            UPRV_UNREACHABLE_EXIT;
    };
}

शून्य Locale::Payload::copy(ध्रुव Payload& other) {
    other.दर्शय([](Payload*) {},
                [](ध्रुव Nest& nest, Payload* dst) { dst->nest = nest; },
                [](ध्रुव Heap& heap, Payload* dst) { dst->heap = heap; },
                एतत्);
}

शून्य Locale::Payload::move(Payload&& other) noexcept {
    other.दर्शय(
        [](Payload*) {},
        [](ध्रुव Nest& nest, Payload* dst) { dst->nest = nest; },
        [](ध्रुव Heap& heap, Payload* dst) { dst->heap = std::move(const_cast<Heap&>(heap)); },
        एतत्);
}

Locale::Payload::~Payload() {
    यदि (type == eHEAP) { heap.~Heap(); }
}

Locale::Payload::Payload(ध्रुव Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::संक्रियकः=(ध्रुव Payload& other) U_LIFETIME_BOUND {
    यदि (एतत् != &other) {
        समुच्चयः_गन्तव्यम्_Bogus();
        copy(other);
    }
    प्रत्यागम *एतत्;
}

Locale::Payload& Locale::Payload::संक्रियकः=(Payload&& other) noexcept U_LIFETIME_BOUND {
    यदि (एतत् != &other) {
        समुच्चयः_गन्तव्यम्_Bogus();
        move(std::move(other));
    }
    प्रत्यागम *एतत्;
}

शून्य Locale::Payload::समुच्चयः_गन्तव्यम्_Bogus() {
    एतत्->~Payload();
    type = eBOGUS;
}

प्रतिरूपम् <प्रकारनाम T, प्रकारनाम... तर्काः> T& Locale::Payload::emplace(तर्काः&&... तर्काः_2) U_LIFETIME_BOUND {
    यदि constexpr (std::is_same_v<T, Nest>) {
        एतत्->~Payload();
        ::नूतन (&nest) Nest(std::forward<तर्काः>(तर्काः_2)...);
        प्रत्यागम nest;
    }
    यदि constexpr (std::is_same_v<T, Heap>) {
        U_ASSERT(type != eHEAP);
        ::नूतन (&heap) Heap(std::forward<तर्काः>(तर्काः_2)...);
        प्रत्यागम heap;
    }
}

प्रतिरूपम् <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    प्रत्यागम type == eNEST ? &nest : शून्यसूचकः;
}
प्रतिरूपम् <> Locale::Heap* Locale::Payload::get() U_LIFETIME_BOUND {
    प्रत्यागम type == eHEAP ? &heap : शून्यसूचकः;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#परिभाषय SEP_CHAR '_'
#परिभाषय NULL_CHAR '\0'

Locale::~Locale() = स्वभाव;

Locale::Locale()
    : UObject(), payload()
{
    init(शून्यसूचकः, असत्य);
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
    यदि( (newLanguage==शून्यसूचकः) && (newCountry == शून्यसूचकः) && (newVariant == शून्यसूचकः) )
    {
        init(शून्यसूचकः, असत्य); /* shortcut */
    }
    अन्यथा
    {
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        यदि ( newLanguage != शून्यसूचकः )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            यदि ( lsize < 0 || lsize > ULOC_STRING_सीमा ) { // int32 wrap
                प्रत्यागम;
            }
        }

        CharString togo(newLanguage, lsize, स्थितिः); // start with newLanguage

        // _Country
        यदि ( newCountry != शून्यसूचकः )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            यदि ( csize < 0 || csize > ULOC_STRING_सीमा ) { // int32 wrap
                प्रत्यागम;
            }
        }

        // _Variant
        यदि ( newVariant != शून्यसूचकः )
        {
            // remove leading _'s
            यावत्(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            यदि ( vsize < 0 || vsize > ULOC_STRING_सीमा ) { // int32 wrap
                प्रत्यागम;
            }
            यावत्( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        यदि ( newKeywords != शून्यसूचकः)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            यदि ( ksize < 0 || ksize > ULOC_STRING_सीमा ) {
              प्रत्यागम;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        यदि ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, स्थितिः);
        }

        यदि ( csize != 0 )
        {
            togo.append(newCountry, स्थितिः);
        }

        यदि ( vsize != 0)
        {
            togo.append(SEP_CHAR, स्थितिः)
                .append(newVariant, vsize, स्थितिः);
        }

        यदि ( ksize != 0)
        {
            यदि (uprv_strchr(newKeywords, '=')) {
                togo.append('@', स्थितिः); /* keyword parsing */
            }
            अन्यथा {
                togo.append('_', स्थितिः); /* Variant parsing with a script */
                यदि ( vsize == 0) {
                    togo.append('_', स्थितिः); /* No country found */
                }
            }
            togo.append(newKeywords, स्थितिः);
        }

        यदि (U_FAILURE(स्थितिः)) {
            // Something went wrong with appending, etc.
            प्रत्यागम;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.दत्त(), असत्य);
    }
}

Locale::Locale(ध्रुव Locale&) = स्वभाव;
Locale::Locale(Locale&&) noexcept = स्वभाव;

Locale& Locale::संक्रियकः=(ध्रुव Locale&) U_LIFETIME_BOUND = स्वभाव;
Locale& Locale::संक्रियकः=(Locale&&) noexcept U_LIFETIME_BOUND = स्वभाव;

Locale *
Locale::clone() ध्रुव {
    प्रत्यागम नूतन Locale(*एतत्);
}

तार्किक
Locale::संक्रियकः==( ध्रुव   Locale& other) ध्रुव
{
    प्रत्यागम uprv_strcmp(other.getName(), getName()) == 0;
}

नामाकाश {

UInitOnce g_ज्ञातम्_Canonicalized_Init_Once {};
UHashtable *g_ज्ञातम्_Canonicalized = शून्यसूचकः;

constexpr ध्रुव वर्ण* ज्ञातम्_CANONICALIZED[] = {
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

UBool U_CALLCONV cleanup_ज्ञातम्_Canonicalized() {
    g_ज्ञातम्_Canonicalized_Init_Once.reset();
    यदि (g_ज्ञातम्_Canonicalized) { uhash_close(g_ज्ञातम्_Canonicalized); }
    प्रत्यागम सत्य;
}

शून्य U_CALLCONV आरोपय_ज्ञातम्_Canonicalized(UError_संहिता &स्थितिः) {
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_LOCALE_ज्ञातम्_CANONICALIZED,
                                cleanup_ज्ञातम्_Canonicalized);
    LocalUHashtablePointer new_ज्ञातम्_Canonicalized_Map(
        uhash_open(uhash_hashChars, uhash_compareChars, शून्यसूचकः, &स्थितिः));
    प्रति (int32_t i = 0;
            U_SUCCESS(स्थितिः) && i < UPRV_LENGTHOF(ज्ञातम्_CANONICALIZED);
            i++) {
        uhash_puti(new_ज्ञातम्_Canonicalized_Map.getAlias(),
                   (शून्य*)ज्ञातम्_CANONICALIZED[i],
                   1, &स्थितिः);
    }
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    g_ज्ञातम्_Canonicalized = new_ज्ञातम्_Canonicalized_Map.orphan();
}

वर्ग Alias_दत्त;

/**
 * A Builder class to build the alias data.
 */
वर्ग Alias_दत्त_Builder {
सार्वजनिक:
    Alias_दत्त_Builder() {
    }

    // Build the AliasData from resource.
    Alias_दत्त* build(UError_संहिता &स्थितिः);

गुप्त:
    शून्य readAlias(UResourceBundle* alias,
                   Unique_Char_वाक्यानि* वाक्यानि,
                   Local_स्मृतिः<ध्रुव वर्ण*>& types,
                   Local_स्मृतिः<int32_t>& replacementIndexes,
                   int32_t &length,
                   शून्य (*checkType)(ध्रुव वर्ण* type),
                   शून्य (*checkReplacement)(ध्रुव UChar* replacement),
                   UError_संहिता &स्थितिः);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    शून्य readLanguageAlias(UResourceBundle* alias,
                           Unique_Char_वाक्यानि* वाक्यानि,
                           Local_स्मृतिः<ध्रुव वर्ण*>& types,
                           Local_स्मृतिः<int32_t>& replacementIndexes,
                           int32_t &length,
                           UError_संहिता &स्थितिः);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    शून्य readScriptAlias(UResourceBundle* alias,
                         Unique_Char_वाक्यानि* वाक्यानि,
                         Local_स्मृतिः<ध्रुव वर्ण*>& types,
                         Local_स्मृतिः<int32_t>& replacementIndexes,
                         int32_t &length, UError_संहिता &स्थितिः);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    शून्य readTerritoryAlias(UResourceBundle* alias,
                            Unique_Char_वाक्यानि* वाक्यानि,
                            Local_स्मृतिः<ध्रुव वर्ण*>& types,
                            Local_स्मृतिः<int32_t>& replacementIndexes,
                            int32_t &length, UError_संहिता &स्थितिः);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    शून्य readVariantAlias(UResourceBundle* alias,
                          Unique_Char_वाक्यानि* वाक्यानि,
                          Local_स्मृतिः<ध्रुव वर्ण*>& types,
                          Local_स्मृतिः<int32_t>& replacementIndexes,
                          int32_t &length, UError_संहिता &स्थितिः);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    शून्य readSubdivisionAlias(UResourceBundle* alias,
                          Unique_Char_वाक्यानि* वाक्यानि,
                          Local_स्मृतिः<ध्रुव वर्ण*>& types,
                          Local_स्मृतिः<int32_t>& replacementIndexes,
                          int32_t &length, UError_संहिता &स्थितिः);
};

/**
 * A class to hold the Alias Data.
 */
वर्ग Alias_दत्त : सार्वजनिक UMemory {
सार्वजनिक:
    स्थिर ध्रुव Alias_दत्त* singleton(UError_संहिता& स्थितिः) {
        यदि (U_FAILURE(स्थितिः)) {
            // Do not get into loadData if the status already has error.
            प्रत्यागम शून्यसूचकः;
        }
        umtx_initOnce(Alias_दत्त::gInitOnce, &Alias_दत्त::आरोपय_दत्त, स्थितिः);
        प्रत्यागम gSingleton;
    }

    ध्रुव CharStringMap& languageMap() ध्रुव U_LIFETIME_BOUND { प्रत्यागम language; }
    ध्रुव CharStringMap& scriptMap() ध्रुव U_LIFETIME_BOUND { प्रत्यागम script; }
    ध्रुव CharStringMap& territoryMap() ध्रुव U_LIFETIME_BOUND { प्रत्यागम territory; }
    ध्रुव CharStringMap& variantMap() ध्रुव U_LIFETIME_BOUND { प्रत्यागम variant; }
    ध्रुव CharStringMap& subdivisionMap() ध्रुव U_LIFETIME_BOUND { प्रत्यागम subdivision; }

    स्थिर शून्य U_CALLCONV आरोपय_दत्त(UError_संहिता &स्थितिः);
    स्थिर UBool U_CALLCONV cleanup();

    स्थिर UInitOnce gInitOnce;

गुप्त:
    Alias_दत्त(CharStringMap languageMap,
              CharStringMap scriptMap,
              CharStringMap territoryMap,
              CharStringMap variantMap,
              CharStringMap subdivisionMap,
              CharString* वाक्यानि)
        : language(std::move(languageMap)),
          script(std::move(scriptMap)),
          territory(std::move(territoryMap)),
          variant(std::move(variantMap)),
          subdivision(std::move(subdivisionMap)),
          वाक्यानि(वाक्यानि) {
    }

    ~Alias_दत्त() {
        विलोपय वाक्यानि;
    }

    स्थिर ध्रुव Alias_दत्त* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* वाक्यानि;

    मित्रम् वर्ग Alias_दत्त_Builder;
};


ध्रुव Alias_दत्त* Alias_दत्त::gSingleton = शून्यसूचकः;
UInitOnce Alias_दत्त::gInitOnce {};

UBool U_CALLCONV
Alias_दत्त::cleanup()
{
    gInitOnce.reset();
    विलोपय gSingleton;
    प्रत्यागम सत्य;
}

शून्य
Alias_दत्त_Builder::readAlias(
        UResourceBundle* alias,
        Unique_Char_वाक्यानि* वाक्यानि,
        Local_स्मृतिः<ध्रुव वर्ण*>& types,
        Local_स्मृतिः<int32_t>& replacementIndexes,
        int32_t &length,
        शून्य (*checkType)(ध्रुव वर्ण* type),
        शून्य (*checkReplacement)(ध्रुव UChar* replacement),
        UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    length = ures_get_आकार(alias);
    ध्रुव वर्ण** rawTypes = types.allocateInsteadAndCopy(length);
    यदि (rawTypes == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    यदि (rawIndexes == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    प्रति (पूर्णाङ्क i = 0; U_SUCCESS(स्थितिः) && ures_has_अग्र(alias); i++) {
        LocalUResourceBundlePointer res(
            ures_get_अग्र_Resource(alias, शून्यसूचकः, &स्थितिः));
        ध्रुव वर्ण* aliasFrom = ures_get_कुञ्चिका(res.getAlias());
        ध्रुव UChar* alias_गन्तव्यम् =
            ures_get_String_By_कुञ्चिका(res.getAlias(), "replacement", शून्यसूचकः, &स्थितिः);
        यदि (U_FAILURE(स्थितिः)) प्रत्यागम;

        checkType(aliasFrom);
        checkReplacement(alias_गन्तव्यम्);

        rawTypes[i] = aliasFrom;
        rawIndexes[i] = वाक्यानि->सङ्कलय(alias_गन्तव्यम्, स्थितिः);
    }
}

/**
 * Read the languageAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement language.
 */
शून्य
Alias_दत्त_Builder::readLanguageAlias(
        UResourceBundle* alias,
        Unique_Char_वाक्यानि* वाक्यानि,
        Local_स्मृतिः<ध्रुव वर्ण*>& types,
        Local_स्मृतिः<int32_t>& replacementIndexes,
        int32_t &length,
        UError_संहिता &स्थितिः)
{
    प्रत्यागम readAlias(
        alias, वाक्यानि, types, replacementIndexes, length,
#यदि U_DEBUG
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
#अन्यथा
        [](ध्रुव वर्ण*) {},
#यदि_अन्तः
        [](ध्रुव UChar*) {}, स्थितिः);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
शून्य
Alias_दत्त_Builder::readScriptAlias(
        UResourceBundle* alias,
        Unique_Char_वाक्यानि* वाक्यानि,
        Local_स्मृतिः<ध्रुव वर्ण*>& types,
        Local_स्मृतिः<int32_t>& replacementIndexes,
        int32_t &length,
        UError_संहिता &स्थितिः)
{
    प्रत्यागम readAlias(
        alias, वाक्यानि, types, replacementIndexes, length,
#यदि U_DEBUG
        [](ध्रुव वर्ण* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](ध्रुव UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#अन्यथा
        [](ध्रुव वर्ण*) {},
        [](ध्रुव UChar*) { },
#यदि_अन्तः
        स्थितिः);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
शून्य
Alias_दत्त_Builder::readTerritoryAlias(
        UResourceBundle* alias,
        Unique_Char_वाक्यानि* वाक्यानि,
        Local_स्मृतिः<ध्रुव वर्ण*>& types,
        Local_स्मृतिः<int32_t>& replacementIndexes,
        int32_t &length,
        UError_संहिता &स्थितिः)
{
    प्रत्यागम readAlias(
        alias, वाक्यानि, types, replacementIndexes, length,
#यदि U_DEBUG
        [](ध्रुव वर्ण* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#अन्यथा
        [](ध्रुव वर्ण*) {},
#यदि_अन्तः
        [](ध्रुव UChar*) { },
        स्थितिः);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
शून्य
Alias_दत्त_Builder::readVariantAlias(
        UResourceBundle* alias,
        Unique_Char_वाक्यानि* वाक्यानि,
        Local_स्मृतिः<ध्रुव वर्ण*>& types,
        Local_स्मृतिः<int32_t>& replacementIndexes,
        int32_t &length,
        UError_संहिता &स्थितिः)
{
    प्रत्यागम readAlias(
        alias, वाक्यानि, types, replacementIndexes, length,
#यदि U_DEBUG
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
#अन्यथा
        [](ध्रुव वर्ण*) {},
        [](ध्रुव UChar*) { },
#यदि_अन्तः
        स्थितिः);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
शून्य
Alias_दत्त_Builder::readSubdivisionAlias(
        UResourceBundle* alias,
        Unique_Char_वाक्यानि* वाक्यानि,
        Local_स्मृतिः<ध्रुव वर्ण*>& types,
        Local_स्मृतिः<int32_t>& replacementIndexes,
        int32_t &length,
        UError_संहिता &स्थितिः)
{
    प्रत्यागम readAlias(
        alias, वाक्यानि, types, replacementIndexes, length,
#यदि U_DEBUG
        [](ध्रुव वर्ण* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#अन्यथा
        [](ध्रुव वर्ण*) {},
#यदि_अन्तः
        [](ध्रुव UChar*) { },
        स्थितिः);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
शून्य U_CALLCONV
Alias_दत्त::आरोपय_दत्त(UError_संहिता &स्थितिः)
{
#यदि_परिभाषितम् LOCALE_CANONICALIZATION_DEBUG
    UDate start = uprv_getRawUTCtime();
#यदि_अन्तः  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    Alias_दत्त_Builder builder;
    gSingleton = builder.build(स्थितिः);
#यदि_परिभाषितम् LOCALE_CANONICALIZATION_DEBUG
    UDate अन्तः = uprv_getRawUTCtime();
    प्रारूप_लिख("AliasData::loadData took total %f ms\n", अन्तः - start);
#यदि_अन्तः  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
Alias_दत्त*
Alias_दत्त_Builder::build(UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम शून्यसूचकः; }

    LocalUResourceBundlePointer metadata(
        ures_open_प्रत्यक्षम्(शून्यसूचकः, "metadata", &स्थितिः));
    LocalUResourceBundlePointer metadataAlias(
        ures_get_By_कुञ्चिका(metadata.getAlias(), "alias", शून्यसूचकः, &स्थितिः));
    LocalUResourceBundlePointer languageAlias(
        ures_get_By_कुञ्चिका(metadataAlias.getAlias(), "language", शून्यसूचकः, &स्थितिः));
    LocalUResourceBundlePointer scriptAlias(
        ures_get_By_कुञ्चिका(metadataAlias.getAlias(), "script", शून्यसूचकः, &स्थितिः));
    LocalUResourceBundlePointer territoryAlias(
        ures_get_By_कुञ्चिका(metadataAlias.getAlias(), "territory", शून्यसूचकः, &स्थितिः));
    LocalUResourceBundlePointer variantAlias(
        ures_get_By_कुञ्चिका(metadataAlias.getAlias(), "variant", शून्यसूचकः, &स्थितिः));
    LocalUResourceBundlePointer subdivisionAlias(
        ures_get_By_कुञ्चिका(metadataAlias.getAlias(), "subdivision", शून्यसूचकः, &स्थितिः));

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    int32_t languages_दीर्घता = 0, script_दीर्घता = 0, territory_दीर्घता = 0,
            variant_दीर्घता = 0, subdivision_दीर्घता = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    Unique_Char_वाक्यानि वाक्यानि(स्थितिः);
    Local_स्मृतिः<ध्रुव वर्ण*> languageTypes;
    Local_स्मृतिः<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &वाक्यानि,
                      languageTypes,
                      languageReplacementIndexes,
                      languages_दीर्घता,
                      स्थितिः);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    Local_स्मृतिः<ध्रुव वर्ण*> scriptTypes;
    Local_स्मृतिः<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &वाक्यानि,
                    scriptTypes,
                    scriptReplacementIndexes,
                    script_दीर्घता,
                    स्थितिः);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    Local_स्मृतिः<ध्रुव वर्ण*> territoryTypes;
    Local_स्मृतिः<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &वाक्यानि,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territory_दीर्घता, स्थितिः);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    Local_स्मृतिः<ध्रुव वर्ण*> variantTypes;
    Local_स्मृतिः<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &वाक्यानि,
                     variantTypes,
                     variantReplacementIndexes,
                     variant_दीर्घता, स्थितिः);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    Local_स्मृतिः<ध्रुव वर्ण*> subdivisionTypes;
    Local_स्मृतिः<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &वाक्यानि,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivision_दीर्घता, स्थितिः);

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    // We can only use strings after freeze it.
    वाक्यानि.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, स्थितिः);
    प्रति (int32_t i = 0; U_SUCCESS(स्थितिः) && i < languages_दीर्घता; i++) {
        languageMap.put(languageTypes[i],
                        वाक्यानि.get(languageReplacementIndexes[i]),
                        स्थितिः);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, स्थितिः);
    प्रति (int32_t i = 0; U_SUCCESS(स्थितिः) && i < script_दीर्घता; i++) {
        scriptMap.put(scriptTypes[i],
                      वाक्यानि.get(scriptReplacementIndexes[i]),
                      स्थितिः);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, स्थितिः);
    प्रति (int32_t i = 0; U_SUCCESS(स्थितिः) && i < territory_दीर्घता; i++) {
        territoryMap.put(territoryTypes[i],
                         वाक्यानि.get(territoryReplacementIndexes[i]),
                         स्थितिः);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, स्थितिः);
    प्रति (int32_t i = 0; U_SUCCESS(स्थितिः) && i < variant_दीर्घता; i++) {
        variantMap.put(variantTypes[i],
                       वाक्यानि.get(variantReplacementIndexes[i]),
                       स्थितिः);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, स्थितिः);
    प्रति (int32_t i = 0; U_SUCCESS(स्थितिः) && i < subdivision_दीर्घता; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       वाक्यानि.get(subdivisionReplacementIndexes[i]),
                       स्थितिः);
    }

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    // copy hashtables
    स्वचालित *दत्त = नूतन Alias_दत्त(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        वाक्यानि.orphan_Char_वाक्यानि());

    यदि (दत्त == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    प्रत्यागम दत्त;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
वर्ग AliasReplacer {
सार्वजनिक:
    AliasReplacer(UError_संहिता& स्थितिः) :
            language(शून्यसूचकः), script(शून्यसूचकः), region(शून्यसूचकः),
            extensions(शून्यसूचकः),
            // store value in variants only once
            variants(शून्यसूचकः,
                     ([](UElement e1, UElement e2) -> UBool {
                       प्रत्यागम 0==uprv_strcmp((ध्रुव वर्ण*)e1.pointer,
                                             (ध्रुव वर्ण*)e2.pointer);}),
                     स्थितिः),
            दत्त(शून्यसूचकः) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    तार्किक replace(
        ध्रुव Locale& locale, CharString& out, UError_संहिता& स्थितिः);

गुप्त:
    ध्रुव वर्ण* language;
    ध्रुव वर्ण* script;
    ध्रुव वर्ण* region;
    ध्रुव वर्ण* extensions;
    UVector variants;

    ध्रुव Alias_दत्त* दत्त;

    अन्तःस्थ तार्किक not_रिक्तानि(ध्रुव वर्ण* str) {
        प्रत्यागम str && str[0] != NULL_CHAR;
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
        प्रत्यागम not_रिक्तानि(replacement) ?
            ((input == शून्यसूचकः) ?  replacement : input) :
            ((type == शून्यसूचकः) ? input  : शून्यसूचकः);
    }

    अन्तःस्थ तार्किक same(ध्रुव वर्ण* a, ध्रुव वर्ण* b) {
        यदि (a == शून्यसूचकः && b == शून्यसूचकः) {
            प्रत्यागम सत्य;
        }
        यदि ((a == शून्यसूचकः && b != शून्यसूचकः) ||
            (a != शून्यसूचकः && b == शून्यसूचकः)) {
          प्रत्यागम असत्य;
        }
        प्रत्यागम uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& output_गन्तव्यम्_String(CharString& out U_LIFETIME_BOUND, UError_संहिता& स्थितिः);

    // Generate the lookup key.
    CharString& generate_कुञ्चिका(ध्रुव वर्ण* language, ध्रुव वर्ण* region,
                            ध्रुव वर्ण* variant, CharString& out U_LIFETIME_BOUND,
                            UError_संहिता& स्थितिः);

    शून्य parseLanguageReplacement(ध्रुव वर्ण* replacement,
                                  ध्रुव वर्ण*& replaceLanguage,
                                  ध्रुव वर्ण*& replaceScript,
                                  ध्रुव वर्ण*& replaceRegion,
                                  ध्रुव वर्ण*& replaceVariant,
                                  ध्रुव वर्ण*& replaceExtensions,
                                  UVector& गन्तव्यम्_Be_Freed,
                                  UError_संहिता& स्थितिः);

    // Replace by using languageAlias.
    तार्किक replaceLanguage(तार्किक checkLanguage, तार्किक checkRegion,
                         तार्किक checkVariants, UVector& गन्तव्यम्_Be_Freed,
                         UError_संहिता& स्थितिः);

    // Replace by using territoryAlias.
    तार्किक replaceTerritory(UVector& गन्तव्यम्_Be_Freed, UError_संहिता& स्थितिः);

    // Replace by using scriptAlias.
    तार्किक replaceScript(UError_संहिता& स्थितिः);

    // Replace by using variantAlias.
    तार्किक replaceVariant(UError_संहिता& स्थितिः);

    // Replace by using subdivisionAlias.
    तार्किक replaceSubdivision(StringPiece subdivision,
                            CharString& output, UError_संहिता& स्थितिः);

    // Replace transformed extensions.
    तार्किक replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UError_संहिता& स्थितिः);
};

CharString&
AliasReplacer::generate_कुञ्चिका(
        ध्रुव वर्ण* language, ध्रुव वर्ण* region, ध्रुव वर्ण* variant,
        CharString& out U_LIFETIME_BOUND, UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम out; }
    out.append(language, स्थितिः);
    यदि (not_रिक्तानि(region)) {
        out.append(SEP_CHAR, स्थितिः)
            .append(region, स्थितिः);
    }
    यदि (not_रिक्तानि(variant)) {
       out.append(SEP_CHAR, स्थितिः)
           .append(variant, स्थितिः);
    }
    प्रत्यागम out;
}

शून्य
AliasReplacer::parseLanguageReplacement(
    ध्रुव वर्ण* replacement,
    ध्रुव वर्ण*& replacedLanguage,
    ध्रुव वर्ण*& replacedScript,
    ध्रुव वर्ण*& replacedRegion,
    ध्रुव वर्ण*& replacedVariant,
    ध्रुव वर्ण*& replacedExtensions,
    UVector& गन्तव्यम्_Be_Freed,
    UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = शून्यसूचकः;
    यदि (uprv_strchr(replacement, '_') == शून्यसूचकः) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        प्रत्यागम;
    }
    // We have multiple field so we have to allocate and parse
    स्वचालित str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), स्थितिः, स्थितिः);
    वर्ण* दत्त = str->दत्त();
    गन्तव्यम्_Be_Freed.adoptElement(str.release(), स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    replacedLanguage = (ध्रुव वर्ण*) दत्त;
    वर्ण* अन्तः_सम्बन्धि_क्षेत्रम् = uprv_strchr(दत्त, '_');
    *अन्तः_सम्बन्धि_क्षेत्रम् = '\0'; // null terminiate it.
    अन्तः_सम्बन्धि_क्षेत्रम्++;
    ध्रुव वर्ण* start = अन्तः_सम्बन्धि_क्षेत्रम्;
    अन्तः_सम्बन्धि_क्षेत्रम् = const_cast<वर्ण*>(uprv_strchr(start, '_'));
    size_t len = 0;
    यदि (अन्तः_सम्बन्धि_क्षेत्रम् == शून्यसूचकः) {
        len = uprv_strlen(start);
    } अन्यथा {
        len = अन्तः_सम्बन्धि_क्षेत्रम् - start;
        *अन्तः_सम्बन्धि_क्षेत्रम् = '\0'; // null terminiate it.
    }
    यदि (len == 4 && uprv_isASCIILetter(*start)) {
        // Got a script
        replacedScript = start;
        यदि (अन्तः_सम्बन्धि_क्षेत्रम् == शून्यसूचकः) {
            प्रत्यागम;
        }
        start = अन्तः_सम्बन्धि_क्षेत्रम्++;
        अन्तः_सम्बन्धि_क्षेत्रम् = const_cast<वर्ण*>(uprv_strchr(start, '_'));
        यदि (अन्तः_सम्बन्धि_क्षेत्रम् == शून्यसूचकः) {
            len = uprv_strlen(start);
        } अन्यथा {
            len = अन्तः_सम्बन्धि_क्षेत्रम् - start;
            *अन्तः_सम्बन्धि_क्षेत्रम् = '\0'; // null terminiate it.
        }
    }
    यदि (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = start;
        यदि (अन्तः_सम्बन्धि_क्षेत्रम् == शून्यसूचकः) {
            प्रत्यागम;
        }
        start = अन्तः_सम्बन्धि_क्षेत्रम्++;
        अन्तः_सम्बन्धि_क्षेत्रम् = const_cast<वर्ण*>(uprv_strchr(start, '_'));
        यदि (अन्तः_सम्बन्धि_क्षेत्रम् == शून्यसूचकः) {
            len = uprv_strlen(start);
        } अन्यथा {
            len = अन्तः_सम्बन्धि_क्षेत्रम् - start;
            *अन्तः_सम्बन्धि_क्षेत्रम् = '\0'; // null terminiate it.
        }
    }
    यदि (len >= 4) {
        // Got a variant
        replacedVariant = start;
        यदि (अन्तः_सम्बन्धि_क्षेत्रम् == शून्यसूचकः) {
            प्रत्यागम;
        }
        start = अन्तः_सम्बन्धि_क्षेत्रम्++;
    }
    replacedExtensions = start;
}

तार्किक
AliasReplacer::replaceLanguage(
        तार्किक checkLanguage, तार्किक checkRegion,
        तार्किक checkVariants, UVector& गन्तव्यम्_Be_Freed, UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    यदि (    (checkRegion && region == शून्यसूचकः) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        प्रत्यागम असत्य;
    }
    int32_t variant_आकार = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    ध्रुव वर्ण* अन्वेषणम्_Language = checkLanguage ? language : "und";
    ध्रुव वर्ण* अन्वेषणम्_Region = checkRegion ? region : शून्यसूचकः;
    ध्रुव वर्ण* अन्वेषणम्_Variant = शून्यसूचकः;
    प्रति (int32_t variant_सूचकांक = 0;
            variant_सूचकांक < variant_आकार;
            variant_सूचकांक++) {
        यदि (checkVariants) {
            U_ASSERT(variant_सूचकांक < variant_आकार);
            अन्वेषणम्_Variant = static_cast<ध्रुव वर्ण*>(variants.elementAt(variant_सूचकांक));
        }

        यदि (अन्वेषणम्_Variant != शून्यसूचकः && uprv_strlen(अन्वेषणम्_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            अन्वेषणम्_Variant = शून्यसूचकः;
        }
        CharString type_कुञ्चिका;
        generate_कुञ्चिका(अन्वेषणम्_Language, अन्वेषणम्_Region, अन्वेषणम्_Variant, type_कुञ्चिका,
                    स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम असत्य;
        }
        ध्रुव वर्ण *replacement = दत्त->languageMap().get(type_कुञ्चिका.दत्त());
        यदि (replacement == शून्यसूचकः) {
            // Found no replacement data.
            अनुवर्त;
        }

        ध्रुव वर्ण* replacedLanguage = शून्यसूचकः;
        ध्रुव वर्ण* replacedScript = शून्यसूचकः;
        ध्रुव वर्ण* replacedRegion = शून्यसूचकः;
        ध्रुव वर्ण* replacedVariant = शून्यसूचकः;
        ध्रुव वर्ण* replacedExtensions = शून्यसूचकः;
        parseLanguageReplacement(replacement,
                                 replacedLanguage,
                                 replacedScript,
                                 replacedRegion,
                                 replacedVariant,
                                 replacedExtensions,
                                 गन्तव्यम्_Be_Freed,
                                 स्थितिः);
        replacedLanguage =
            (replacedLanguage != शून्यसूचकः && uprv_strcmp(replacedLanguage, "und") == 0) ?
            language : replacedLanguage;
        replacedScript = deleteOrReplace(script, शून्यसूचकः, replacedScript);
        replacedRegion = deleteOrReplace(region, अन्वेषणम्_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            अन्वेषणम्_Variant, अन्वेषणम्_Variant, replacedVariant);

        यदि (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(अन्वेषणम्_Variant, replacedVariant) &&
                replacedExtensions == शून्यसूचकः) {
            // Replacement produce no changes.
            अनुवर्त;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        यदि (अन्वेषणम्_Variant != शून्यसूचकः) {
            यदि (not_रिक्तानि(replacedVariant)) {
                variants.समुच्चयः_Element_At((शून्य*)replacedVariant, variant_सूचकांक);
            } अन्यथा {
                variants.removeElementAt(variant_सूचकांक);
            }
        }
        यदि (replacedExtensions != शून्यसूचकः) {
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
        प्रत्यागम सत्य;
    }
    // Nothing changed by language alias data.
    प्रत्यागम असत्य;
}

तार्किक
AliasReplacer::replaceTerritory(UVector& गन्तव्यम्_Be_Freed, UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    यदि (region == शून्यसूचकः) {
        // No region to search.
        प्रत्यागम असत्य;
    }
    ध्रुव वर्ण *replacement = दत्त->territoryMap().get(region);
    यदि (replacement == शून्यसूचकः) {
        // Found no replacement data for this region.
        प्रत्यागम असत्य;
    }
    ध्रुव वर्ण* replacedRegion = replacement;
    ध्रुव वर्ण* firstSpace = uprv_strchr(replacement, ' ');
    यदि (firstSpace != शून्यसूचकः) {
        // If there are are more than one region in the replacement.
        // We need to check which one match based on the language.
        // Cannot use nullptr for language because that will construct
        // the default locale, in that case, use "und" to get the correct
        // locale.
        Locale l = LocaleBuilder()
            .समुच्चयः_Language(language == शून्यसूचकः ? "und" : language)
            .समुच्चयः_Script(script)
            .build(स्थितिः);
        l.सङ्कलय_Likely_Subtags(स्थितिः);
        ध्रुव वर्ण* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> item;
        यदि (likelyRegion != शून्यसूचकः && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            ध्रुव वर्ण* प्राप्तम्_अन्तर्_Replacement = uprv_strstr(replacement,
                                                         likelyRegion);
            यदि (प्राप्तम्_अन्तर्_Replacement != शून्यसूचकः) {
                // Assuming the case there are no three letter region code in
                // the replacement of territoryAlias
                U_ASSERT(प्राप्तम्_अन्तर्_Replacement == replacement ||
                         *(प्राप्तम्_अन्तर्_Replacement-1) == ' ');
                U_ASSERT(प्राप्तम्_अन्तर्_Replacement[len] == ' ' ||
                         प्राप्तम्_अन्तर्_Replacement[len] == '\0');
                item = prv::make_unique<CharString>(
                    प्राप्तम्_अन्तर्_Replacement, static_cast<int32_t>(len), स्थितिः, स्थितिः);
            }
        }
        यदि (!item && U_SUCCESS(स्थितिः)) {
            item = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(firstSpace - replacement), स्थितिः, स्थितिः);
        }
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम असत्य; }
        replacedRegion = item->दत्त();
        गन्तव्यम्_Be_Freed.adoptElement(item.release(), स्थितिः);
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम असत्य; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    प्रत्यागम सत्य;
}

तार्किक
AliasReplacer::replaceScript(UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    यदि (script == शून्यसूचकः) {
        // No script to search.
        प्रत्यागम असत्य;
    }
    ध्रुव वर्ण *replacement = दत्त->scriptMap().get(script);
    यदि (replacement == शून्यसूचकः) {
        // Found no replacement data for this script.
        प्रत्यागम असत्य;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    प्रत्यागम सत्य;
}

तार्किक
AliasReplacer::replaceVariant(UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    // Since we may have more than one variant, we need to loop through them.
    प्रति (int32_t i = 0; i < variants.size(); i++) {
        ध्रुव वर्ण* variant = static_cast<ध्रुव वर्ण*>(variants.elementAt(i));
        ध्रुव वर्ण *replacement = दत्त->variantMap().get(variant);
        यदि (replacement == शून्यसूचकः) {
            // Found no replacement data for this variant.
            अनुवर्त;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        यदि (!same(variant, replacement)) {
            variants.समुच्चयः_Element_At((शून्य*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            यदि (uprv_strcmp(variant, "heploc") == 0) {
                प्रति (int32_t j = 0; j < variants.size(); j++) {
                     यदि (uprv_strcmp((ध्रुव वर्ण*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            प्रत्यागम सत्य;
        }
    }
    प्रत्यागम असत्य;
}

तार्किक
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    ध्रुव वर्ण *replacement = दत्त->subdivisionMap().get(subdivision.दत्त());
    यदि (replacement != शून्यसूचकः) {
        ध्रुव वर्ण* firstSpace = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (firstSpace != शून्यसूचकः) ?
            (firstSpace - replacement) : uprv_strlen(replacement);
        यदि (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), स्थितिः);
            यदि (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, स्थितिः);
            }
        }
        प्रत्यागम सत्य;
    }
    प्रत्यागम असत्य;
}

तार्किक
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UError_संहिता& स्थितिः)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    int32_t len = transformedExtensions.length();
    ध्रुव वर्ण* str = transformedExtensions.दत्त();
    ध्रुव वर्ण* tkey = ultag_getTKeyStart(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == शून्यसूचकः) ? len : static_cast<int32_t>((tkey - str - 1)));
    यदि (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .समुच्चयः_Language_चिह्नकम्(StringPiece(str, tlangLen))
            .build(स्थितिः);
        tlang.canonicalize(स्थितिः);
        output = tlang.गन्तव्यम्_Language_चिह्नकम्<CharString>(स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम असत्य;
        }
        T_CString_गन्तव्यम्_अधस्तनम्_Case(output.दत्त());
    }
    यदि (tkey != शून्यसूचकः) {
        // We need to sort the tfields by tkey
        UVector tfields(स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम असत्य;
        }
        कुरु {
            ध्रुव वर्ण* tvalue = uprv_strchr(tkey, '-');
            यदि (tvalue == शून्यसूचकः) {
                स्थितिः = U_ILLEGAL_तर्कः_ERROR;
                प्रत्यागम असत्य;
            }
            ध्रुव वर्ण* अग्र_TKey = ultag_getTKeyStart(tvalue);
            यदि (अग्र_TKey != शून्यसूचकः) {
                *const_cast<वर्ण*>(अग्र_TKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.निवेशय_Element_At((शून्य*)tkey, tfields.size(), स्थितिः);
            यदि (U_FAILURE(स्थितिः)) {
                प्रत्यागम असत्य;
            }
            tkey = अग्र_TKey;
        } यावत् (tkey != शून्यसूचकः);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            प्रत्यागम uprv_strcmp((ध्रुव वर्ण*)e1.pointer, (ध्रुव वर्ण*)e2.pointer);
        }, स्थितिः);
        प्रति (int32_t i = 0; i < tfields.size(); i++) {
             यदि (output.length() > 0) {
                 output.append('-', स्थितिः);
             }
             ध्रुव वर्ण* tfield = static_cast<ध्रुव वर्ण*>(tfields.elementAt(i));
             ध्रुव वर्ण* tvalue = uprv_strchr(tfield, '-');
             यदि (tvalue == शून्यसूचकः) {
                 स्थितिः = U_ILLEGAL_तर्कः_ERROR;
                 प्रत्यागम असत्य;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<वर्ण*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, स्थितिः).append('-', स्थितिः);
             std::optional<std::string_view> bcpTValue = ulocimp_गन्तव्यम्_Bcp_Type(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, स्थितिः);
        }
    }
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    प्रत्यागम सत्य;
}

CharString&
AliasReplacer::output_गन्तव्यम्_String(
    CharString& out U_LIFETIME_BOUND, UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम out; }
    out.append(language, स्थितिः);
    यदि (not_रिक्तानि(script)) {
        out.append(SEP_CHAR, स्थितिः)
            .append(script, स्थितिः);
    }
    यदि (not_रिक्तानि(region)) {
        out.append(SEP_CHAR, स्थितिः)
            .append(region, स्थितिः);
    }
    यदि (variants.size() > 0) {
        यदि (!not_रिक्तानि(script) && !not_रिक्तानि(region)) {
          out.append(SEP_CHAR, स्थितिः);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            प्रत्यागम uprv_strcmp((ध्रुव वर्ण*)e1.pointer, (ध्रुव वर्ण*)e2.pointer);
        }, स्थितिः);
        int32_t variantsStart = out.length();
        प्रति (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, स्थितिः)
                 .append(static_cast<ध्रुव वर्ण*>(variants.elementAt(i)),
                         स्थितिः);
        }
        T_CString_गन्तव्यम्_ऊर्ध्वस्थम्_Case(out.दत्त() + variantsStart);
    }
    यदि (not_रिक्तानि(extensions)) {
        CharString tmp("und_", स्थितिः);
        tmp.append(extensions, स्थितिः);
        Locale tmpLocale(tmp.दत्त());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, स्थितिः);
    }
    प्रत्यागम out;
}

तार्किक
AliasReplacer::replace(ध्रुव Locale& locale, CharString& out, UError_संहिता& स्थितिः)
{
    दत्त = Alias_दत्त::singleton(स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    U_ASSERT(दत्त != शून्यसूचकः);
    out.clear();
    language = locale.getLanguage();
    यदि (!not_रिक्तानि(language)) {
        language = शून्यसूचकः;
    }
    script = locale.getScript();
    यदि (!not_रिक्तानि(script)) {
        script = शून्यसूचकः;
    }
    region = locale.getCountry();
    यदि (!not_रिक्तानि(region)) {
        region = शून्यसूचकः;
    }
    ध्रुव वर्ण* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, स्थितिः);
    यदि (!variantsBuff.is_रिक्तानि()) {
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम असत्य; }
        वर्ण* start = variantsBuff.दत्त();
        T_CString_गन्तव्यम्_अधस्तनम्_Case(start);
        वर्ण* अन्तः;
        यावत् ((अन्तः = uprv_strchr(start, SEP_CHAR)) != शून्यसूचकः &&
               U_SUCCESS(स्थितिः)) {
            *अन्तः = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            यदि (*start && !variants.अन्तर्भवति(start)) {
                variants.सङ्कलय_Element(start, स्थितिः);
            }
            start = अन्तः + 1;
        }
        // do not add "" or duplicate data to variants
        यदि (*start && !variants.अन्तर्भवति(start)) {
            variants.सङ्कलय_Element(start, स्थितिः);
        }
    }
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम असत्य; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        प्रत्यागम uprv_strcmp((ध्रुव वर्ण*)e1.pointer, (ध्रुव वर्ण*)e2.pointer);
    }, स्थितिः);

    // A changed count to assert when loop too many times.
    int32_t परिवर्तितम् = 0;
    ध्रुव int32_t max_परिवर्तितम् = 4 + variants.size();
    (शून्य)max_परिवर्तितम्; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector वाक्यानि_गन्तव्यम्_Be_Freed([](शून्य *obj) { विलोपय static_cast<CharString*>(obj); },
                             शून्यसूचकः, 10, स्थितिः);
    यावत् (U_SUCCESS(स्थितिः)) {
        // Something wrong with the data cause looping here more than maxChanged
        // times already.
        U_ASSERT(परिवर्तितम् <= max_परिवर्तितम्);
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
        यदि (    replaceLanguage(सत्य, सत्य,  सत्य,  वाक्यानि_गन्तव्यम्_Be_Freed, स्थितिः) ||
                replaceLanguage(सत्य, सत्य,  असत्य, वाक्यानि_गन्तव्यम्_Be_Freed, स्थितिः) ||
                replaceLanguage(सत्य, असत्य, सत्य,  वाक्यानि_गन्तव्यम्_Be_Freed, स्थितिः) ||
                replaceLanguage(सत्य, असत्य, असत्य, वाक्यानि_गन्तव्यम्_Be_Freed, स्थितिः) ||
                replaceLanguage(असत्य,असत्य, सत्य,  वाक्यानि_गन्तव्यम्_Be_Freed, स्थितिः) ||
                replaceTerritory(वाक्यानि_गन्तव्यम्_Be_Freed, स्थितिः) ||
                replaceScript(स्थितिः) ||
                replaceVariant(स्थितिः)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            परिवर्तितम्++;
            अनुवर्त;
        }
        // Nothing changed. Break out.
        भङ्ग;
    }  // while(1)

    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम असत्य; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    ध्रुव वर्ण* extensionsStr = locale_getKeywordsStart(locale.getName());
    यदि (परिवर्तितम् == 0 && variants.size() <= 1 && extensionsStr == शून्यसूचकः) {
        प्रत्यागम असत्य;
    }
    output_गन्तव्यम्_String(out, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    यदि (extensionsStr != शून्यसूचकः) {
        परिवर्तितम् = 0;
        Locale अस्थायी(locale);
        LocalPointer<icu::StringEnumeration> iter(locale.createKeywords(स्थितिः));
        यदि (U_SUCCESS(स्थितिः) && !iter.isNull()) {
            ध्रुव वर्ण* कुञ्चिका;
            यावत् ((कुञ्चिका = iter->next(शून्यसूचकः, स्थितिः)) != शून्यसूचकः) {
                यदि (uprv_strcmp("sd", कुञ्चिका) == 0 || uprv_strcmp("rg", कुञ्चिका) == 0 ||
                        uprv_strcmp("t", कुञ्चिका) == 0) {
                    स्वचालित value = locale.get_Keyword_मानम्<CharString>(कुञ्चिका, स्थितिः);
                    यदि (U_FAILURE(स्थितिः)) {
                        स्थितिः = U_ZERO_ERROR;
                        अनुवर्त;
                    }
                    CharString replacement;
                    यदि (uprv_strlen(कुञ्चिका) == 2) {
                        यदि (replaceSubdivision(value.गन्तव्यम्_String_Piece(), replacement, स्थितिः)) {
                            परिवर्तितम्++;
                            अस्थायी.समुच्चयः_Keyword_मानम्(कुञ्चिका, replacement.दत्त(), स्थितिः);
                        }
                    } अन्यथा {
                        U_ASSERT(uprv_strcmp(कुञ्चिका, "t") == 0);
                        यदि (replaceTransformedExtensions(value, replacement, स्थितिः)) {
                            परिवर्तितम्++;
                            अस्थायी.समुच्चयः_Keyword_मानम्(कुञ्चिका, replacement.दत्त(), स्थितिः);
                        }
                    }
                    यदि (U_FAILURE(स्थितिः)) {
                        प्रत्यागम असत्य;
                    }
                }
            }
        }
        यदि (परिवर्तितम् != 0) {
            extensionsStr = locale_getKeywordsStart(अस्थायी.getName());
        }
        out.append(extensionsStr, स्थितिः);
    }
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    // If the tag is not changed, return.
    यदि (uprv_strcmp(out.दत्त(), locale.getName()) == 0) {
        out.clear();
        प्रत्यागम असत्य;
    }
    प्रत्यागम सत्य;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
तार्किक
canonicalizeLocale(ध्रुव Locale& locale, CharString& out, UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम असत्य; }
    AliasReplacer replacer(स्थितिः);
    प्रत्यागम replacer.replace(locale, out, स्थितिः);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
तार्किक
is_ज्ञातम्_Canonicalized_Locale(ध्रुव वर्ण* locale, UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम असत्य; }

    यदि (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        प्रत्यागम सत्य;
    }

    // common well-known Canonicalized.
    umtx_initOnce(g_ज्ञातम्_Canonicalized_Init_Once,
                  &आरोपय_ज्ञातम्_Canonicalized, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    U_ASSERT(g_ज्ञातम्_Canonicalized != शून्यसूचकः);
    प्रत्यागम uhash_geti(g_ज्ञातम्_Canonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_अन्तः

// Function for testing.
U_EXPORT ध्रुव वर्ण* ध्रुव*
ulocimp_get_ज्ञातम्_Canonicalized_Locale_For_Test(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(ज्ञातम्_CANONICALIZED);
    प्रत्यागम ज्ञातम्_CANONICALIZED;
}

// Function for testing.
U_EXPORT तार्किक
ulocimp_isCanonicalizedLocaleForTest(ध्रुव वर्ण* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    CharString अस्थायी;
    प्रत्यागम !canonicalizeLocale(l, अस्थायी, स्थितिः) && U_SUCCESS(स्थितिः);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(ध्रुव वर्ण* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    प्रत्यागम localeID == शून्यसूचकः ? *एतत् = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    कुरु {
        वर्ण *separator;
        वर्ण *क्षेत्रम्_2[5] = {शून्यसूचकः};
        int32_t क्षेत्रम्_Len[5] = {0};
        int32_t क्षेत्रम्_Idx;
        int32_t variant_क्षेत्रम्;
        int32_t length;
        UError_संहिता err;

        ध्रुव स्वचालित parse = [canonicalize](std::string_view localeID,
                                          वर्ण* name,
                                          int32_t name_क्षमता,
                                          UError_संहिता& स्थितिः) {
            प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
                name, name_क्षमता,
                [&](ByteSink& sink, UError_संहिता& स्थितिः) {
                    यदि (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, स्थितिः);
                    } अन्यथा {
                        ulocimp_getName(localeID, sink, स्थितिः);
                    }
                },
                स्थितिः);
        };

        // "canonicalize" the locale ID to ICU/Java format
        वर्ण* पूर्णम्_Name = nest.आधारः_Name;
        err = U_ZERO_ERROR;
        length = parse(localeID, पूर्णम्_Name, परिमाण Nest::आधारः_Name, err);

        FixedString पूर्णम्_Name_बफर;
        यदि (err == U_बफर_अतिप्रवाहः_ERROR || length >= static_cast<int32_t>(परिमाण Nest::आधारः_Name)) {
            /*Go to heap for the fullName if necessary*/
            यदि (!पूर्णम्_Name_बफर.reserve(length + 1)) {
                भङ्ग; // error: out of memory
            }
            पूर्णम्_Name = पूर्णम्_Name_बफर.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, पूर्णम्_Name, length + 1, err);
        }
        यदि(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            भङ्ग;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = क्षेत्रम्_2[0] = पूर्णम्_Name;
        क्षेत्रम्_Idx = 1;
        वर्ण* at = uprv_strchr(पूर्णम्_Name, '@');
        यावत् ((separator = uprv_strchr(क्षेत्रम्_2[क्षेत्रम्_Idx-1], SEP_CHAR)) != शून्यसूचकः &&
               क्षेत्रम्_Idx < UPRV_LENGTHOF(क्षेत्रम्_2)-1 &&
               (at == शून्यसूचकः || separator < at)) {
            क्षेत्रम्_2[क्षेत्रम्_Idx] = separator + 1;
            क्षेत्रम्_Len[क्षेत्रम्_Idx - 1] = static_cast<int32_t>(separator - क्षेत्रम्_2[क्षेत्रम्_Idx - 1]);
            क्षेत्रम्_Idx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(क्षेत्रम्_2[क्षेत्रम्_Idx-1], '@');
        वर्ण* sep2 = uprv_strchr(क्षेत्रम्_2[क्षेत्रम्_Idx-1], '.');
        यदि (separator!=शून्यसूचकः || sep2!=शून्यसूचकः) {
            यदि (separator==शून्यसूचकः || (sep2!=शून्यसूचकः && separator > sep2)) {
                separator = sep2;
            }
            क्षेत्रम्_Len[क्षेत्रम्_Idx - 1] = static_cast<int32_t>(separator - क्षेत्रम्_2[क्षेत्रम्_Idx - 1]);
        } अन्यथा {
            क्षेत्रम्_Len[क्षेत्रम्_Idx - 1] = length - static_cast<int32_t>(क्षेत्रम्_2[क्षेत्रम्_Idx - 1] - पूर्णम्_Name);
        }
        तार्किक hasKeywords = at != शून्यसूचकः && uprv_strchr(at + 1, '=') != शून्यसूचकः;

        यदि (क्षेत्रम्_Len[0] >= ULOC_LANG_क्षमता)
        {
            भङ्ग; // error: the language field is too long
        }

        variant_क्षेत्रम् = 1; /* Usually the 2nd one, except when a script or country is also used. */
        यदि (क्षेत्रम्_Len[0] > 0) {
            /* We have a language */
            language = {पूर्णम्_Name, static_cast<std::string_view::size_type>(क्षेत्रम्_Len[0])};
        }
        यदि (क्षेत्रम्_Len[1] == 4 && uprv_isASCIILetter(क्षेत्रम्_2[1][0]) &&
                uprv_isASCIILetter(क्षेत्रम्_2[1][1]) && uprv_isASCIILetter(क्षेत्रम्_2[1][2]) &&
                uprv_isASCIILetter(क्षेत्रम्_2[1][3])) {
            /* We have at least a script */
            script = {क्षेत्रम्_2[1], static_cast<std::string_view::size_type>(क्षेत्रम्_Len[1])};
            variant_क्षेत्रम्++;
        }

        यदि (क्षेत्रम्_Len[variant_क्षेत्रम्] == 2 || क्षेत्रम्_Len[variant_क्षेत्रम्] == 3) {
            /* We have a country */
            region = {क्षेत्रम्_2[variant_क्षेत्रम्], static_cast<std::string_view::size_type>(क्षेत्रम्_Len[variant_क्षेत्रम्])};
            variant_क्षेत्रम्++;
        } अन्यथा यदि (क्षेत्रम्_Len[variant_क्षेत्रम्] == 0) {
            variant_क्षेत्रम्++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        यदि (क्षेत्रम्_Len[variant_क्षेत्रम्] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(क्षेत्रम्_2[variant_क्षेत्रम्] - पूर्णम्_Name);
        } अन्यथा यदि (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - पूर्णम्_Name);
        }

        यदि (!hasKeywords && Nest::माति(length, language, script, region)) {
            U_ASSERT(पूर्णम्_Name == nest.आधारः_Name);
            U_ASSERT(पूर्णम्_Name_बफर.is_रिक्तानि());
            nest.init(language, script, region, variantBegin);
        } अन्यथा {
            यदि (पूर्णम्_Name == nest.आधारः_Name) {
                U_ASSERT(पूर्णम्_Name_बफर.is_रिक्तानि());
                पूर्णम्_Name_बफर = {पूर्णम्_Name, static_cast<std::string_view::size_type>(length)};
                यदि (पूर्णम्_Name_बफर.is_रिक्तानि()) {
                    भङ्ग; // error: out of memory
                }
                यदि (!language.empty()) {
                    language = {पूर्णम्_Name_बफर.दत्त(), language.size()};
                }
                यदि (!script.empty()) {
                    script = {पूर्णम्_Name_बफर.दत्त() + (script.दत्त() - पूर्णम्_Name), script.size()};
                }
                यदि (!region.empty()) {
                    region = {पूर्णम्_Name_बफर.दत्त() + (region.दत्त() - पूर्णम्_Name), region.size()};
                }
            }
            Heap& heap = payload.emplace<Heap>(language, script, region, variantBegin);
            यदि (isBogus()) {
                भङ्ग; // error: out of memory
            }
            U_ASSERT(!पूर्णम्_Name_बफर.is_रिक्तानि());
            heap.ptr->पूर्णम्_Name = std::move(पूर्णम्_Name_बफर);
            यदि (hasKeywords) {
                यदि (std::string_view::size_type आधारः_Name_दीर्घता = at - पूर्णम्_Name; आधारः_Name_दीर्घता > 0) {
                    heap.ptr->आधारः_Name = {heap.ptr->पूर्णम्_Name.दत्त(), आधारः_Name_दीर्घता};
                    यदि (heap.ptr->आधारः_Name.is_रिक्तानि()) {
                        भङ्ग; // error: out of memory
                    }
                }
            }
        }

        यदि (canonicalize) {
            यदि (!is_ज्ञातम्_Canonicalized_Locale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                यदि (canonicalizeLocale(*एतत्, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.दत्त(), असत्य);
                }
                यदि (U_FAILURE(err)) {
                    भङ्ग;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        प्रत्यागम *एतत्;
    } यावत्(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    समुच्चयः_गन्तव्यम्_Bogus();

    प्रत्यागम *एतत्;
}

int32_t
Locale::hash_संहिता() ध्रुव
{
    प्रत्यागम ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

शून्य
Locale::समुच्चयः_गन्तव्यम्_Bogus() {
    /* Free our current storage */
    payload.समुच्चयः_गन्तव्यम्_Bogus();
}

ध्रुव Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex तालकम्(&gDefaultLocaleMutex);
        यदि (gDefaultLocale != शून्यसूचकः) {
            प्रत्यागम *gDefaultLocale;
        }
    }
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    प्रत्यागम *locale_समुच्चयः_default_आन्तरिकम्(शून्यसूचकः, स्थितिः);
}



शून्य U_EXPORT2
Locale::समुच्चयः_Default( ध्रुव   Locale&     newLocale,
                            UError_संहिता&  स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    ध्रुव वर्ण *localeID = newLocale.getName();
    locale_समुच्चयः_default_आन्तरिकम्(localeID, स्थितिः);
}

शून्य
Locale::सङ्कलय_Likely_Subtags(UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    CharString maximizedLocaleID = ulocimp_सङ्कलय_Likely_Subtags(getName(), स्थितिः);

    यदि (U_FAILURE(स्थितिः)) {
        यदि (स्थितिः == U_स्मृतिः_आवण्टनम्_ERROR) {
            समुच्चयः_गन्तव्यम्_Bogus();
        }
        प्रत्यागम;
    }

    init(maximizedLocaleID.दत्त(), /*canonicalize=*/असत्य);
    यदि (isBogus()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    }
}

शून्य
Locale::minimizeSubtags(UError_संहिता& स्थितिः) {
    Locale::minimizeSubtags(असत्य, स्थितिः);
}
शून्य
Locale::minimizeSubtags(तार्किक favorScript, UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, स्थितिः);

    यदि (U_FAILURE(स्थितिः)) {
        यदि (स्थितिः == U_स्मृतिः_आवण्टनम्_ERROR) {
            समुच्चयः_गन्तव्यम्_Bogus();
        }
        प्रत्यागम;
    }

    init(minimizedLocaleID.दत्त(), /*canonicalize=*/असत्य);
    यदि (isBogus()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    }
}

शून्य
Locale::canonicalize(UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    यदि (isBogus()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    CharString uncanonicalized(getName(), स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        यदि (स्थितिः == U_स्मृतिः_आवण्टनम्_ERROR) {
            समुच्चयः_गन्तव्यम्_Bogus();
        }
        प्रत्यागम;
    }
    init(uncanonicalized.दत्त(), /*canonicalize=*/सत्य);
    यदि (isBogus()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    }
}

Locale U_EXPORT2
Locale::for_Language_चिह्नकम्(StringPiece चिह्नकम्, UError_संहिता& स्थितिः)
{
    Locale परिणाम(Locale::eBOGUS);

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम परिणाम;
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

    int32_t parsed_दीर्घता;
    CharString localeID = ulocimp_for_Language_चिह्नकम्(
            चिह्नकम्.दत्त(),
            चिह्नकम्.length(),
            &parsed_दीर्घता,
            स्थितिः);

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम परिणाम;
    }

    यदि (parsed_दीर्घता != चिह्नकम्.size()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम परिणाम;
    }

    परिणाम.init(localeID.दत्त(), /*canonicalize=*/असत्य);
    यदि (परिणाम.isBogus()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    }
    प्रत्यागम परिणाम;
}

शून्य
Locale::गन्तव्यम्_Language_चिह्नकम्(ByteSink& sink, UError_संहिता& स्थितिः) ध्रुव
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    यदि (isBogus()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    ulocimp_गन्तव्यम्_Language_चिह्नकम्(getName(), sink, /*strict=*/असत्य, स्थितिः);
}

Locale U_EXPORT2
Locale::createFromName (ध्रुव वर्ण *name)
{
    यदि (name) {
        Locale l("");
        l.init(name, असत्य);
        प्रत्यागम l;
    }
    अन्यथा {
        प्रत्यागम getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, असत्य);
    प्रत्यागम loc;
}

Locale U_EXPORT2
Locale::createCanonical(ध्रुव वर्ण* name) {
    Locale loc("");
    loc.init(name, सत्य);
    प्रत्यागम loc;
}

ध्रुव वर्ण *
Locale::getISO3Language() ध्रुव
{
    प्रत्यागम uloc_getISO3Language(getName());
}


ध्रुव वर्ण *
Locale::getISO3Country() ध्रुव
{
    प्रत्यागम uloc_getISO3Country(getName());
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
    प्रत्यागम uloc_getLCID(getName());
}

ध्रुव वर्ण* ध्रुव* U_EXPORT2 Locale::getISOCountries()
{
    प्रत्यागम uloc_getISOCountries();
}

ध्रुव वर्ण* ध्रुव* U_EXPORT2 Locale::getISOLanguages()
{
    प्रत्यागम uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
शून्य Locale::समुच्चयः_From_POSIXID(ध्रुव वर्ण *posixID)
{
    init(posixID, सत्य);
}

ध्रुव Locale & U_EXPORT2
Locale::get_मूलम्()
{
    प्रत्यागम getLocale(e_मूलम्);
}

ध्रुव Locale & U_EXPORT2
Locale::getEnglish()
{
    प्रत्यागम getLocale(eENGLISH);
}

ध्रुव Locale & U_EXPORT2
Locale::getFrench()
{
    प्रत्यागम getLocale(eFRENCH);
}

ध्रुव Locale & U_EXPORT2
Locale::getGerman()
{
    प्रत्यागम getLocale(eGERMAN);
}

ध्रुव Locale & U_EXPORT2
Locale::getItalian()
{
    प्रत्यागम getLocale(eITALIAN);
}

ध्रुव Locale & U_EXPORT2
Locale::getJapanese()
{
    प्रत्यागम getLocale(eJAPANESE);
}

ध्रुव Locale & U_EXPORT2
Locale::getKorean()
{
    प्रत्यागम getLocale(eKOREAN);
}

ध्रुव Locale & U_EXPORT2
Locale::getChinese()
{
    प्रत्यागम getLocale(eCHINESE);
}

ध्रुव Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    प्रत्यागम getLocale(eCHINA);
}

ध्रुव Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    प्रत्यागम getLocale(eTAIWAN);
}


ध्रुव Locale & U_EXPORT2
Locale::getFrance()
{
    प्रत्यागम getLocale(eFRANCE);
}

ध्रुव Locale & U_EXPORT2
Locale::getGermany()
{
    प्रत्यागम getLocale(eGERMANY);
}

ध्रुव Locale & U_EXPORT2
Locale::getItaly()
{
    प्रत्यागम getLocale(eITALY);
}

ध्रुव Locale & U_EXPORT2
Locale::getJapan()
{
    प्रत्यागम getLocale(eJAPAN);
}

ध्रुव Locale & U_EXPORT2
Locale::getKorea()
{
    प्रत्यागम getLocale(eKOREA);
}

ध्रुव Locale & U_EXPORT2
Locale::getChina()
{
    प्रत्यागम getLocale(eCHINA);
}

ध्रुव Locale & U_EXPORT2
Locale::getPRC()
{
    प्रत्यागम getLocale(eCHINA);
}

ध्रुव Locale & U_EXPORT2
Locale::getTaiwan()
{
    प्रत्यागम getLocale(eTAIWAN);
}

ध्रुव Locale & U_EXPORT2
Locale::getUK()
{
    प्रत्यागम getLocale(eUK);
}

ध्रुव Locale & U_EXPORT2
Locale::getUS()
{
    प्रत्यागम getLocale(eUS);
}

ध्रुव Locale & U_EXPORT2
Locale::getCanada()
{
    प्रत्यागम getLocale(eCANADA);
}

ध्रुव Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    प्रत्यागम getLocale(eCANADA_FRENCH);
}

ध्रुव Locale &
Locale::getLocale(पूर्णाङ्क locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    यदि (localeCache == शून्यसूचकः) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    प्रत्यागम localeCache[locid]; /*operating on nullptr*/
}

/*
This function is defined this way in order to get around static
initialization and static destruction.
 */
Locale *
Locale::getLocaleCache()
{
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    umtx_initOnce(gLocaleCacheInitOnce, locale_init, स्थितिः);
    प्रत्यागम gLocaleCache;
}

वर्ग KeywordEnumeration : सार्वजनिक StringEnumeration {
रक्षित:
    FixedString keywords;
गुप्त:
    int32_t length;
    ध्रुव वर्ण *current;
    स्थिर ध्रुव वर्ण fgClassID;

सार्वजनिक:
    स्थिर UClassID U_EXPORT2 getStaticClassID() { प्रत्यागम (UClassID)&fgClassID; }
    आभास UClassID getDynamicClassID() ध्रुव override { प्रत्यागम getStaticClassID(); }
सार्वजनिक:
    KeywordEnumeration(ध्रुव वर्ण *कुञ्चिकाः, int32_t keywordLen, int32_t current_सूचकांक, UError_संहिता &स्थितिः)
        : keywords(), length(keywordLen), current(शून्यसूचकः) {
        यदि(U_SUCCESS(स्थितिः) && keywordLen != 0) {
            यदि(कुञ्चिकाः == शून्यसूचकः || keywordLen < 0) {
                स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            } अन्यथा {
                keywords = {कुञ्चिकाः, static_cast<std::string_view::size_type>(length)};
                यदि (keywords.is_रिक्तानि()) {
                    स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                } अन्यथा {
                    current = keywords.दत्त() + current_सूचकांक;
                }
            }
        }
    }

    आभास ~KeywordEnumeration();

    आभास StringEnumeration * clone() ध्रुव override
    {
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        प्रत्यागम नूतन KeywordEnumeration(
                keywords.दत्त(), length,
                static_cast<int32_t>(current - keywords.दत्त()), स्थितिः);
    }

    आभास int32_t count(UError_संहिता& स्थितिः) ध्रुव override {
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम 0; }
        ध्रुव वर्ण *kw = keywords.दत्त();
        int32_t परिणाम = 0;
        यावत्(*kw) {
            परिणाम++;
            kw += uprv_strlen(kw)+1;
        }
        प्रत्यागम परिणाम;
    }

    आभास ध्रुव वर्ण* next(int32_t* परिणाम_दीर्घता, UError_संहिता& स्थितिः) U_LIFETIME_BOUND override {
        ध्रुव वर्ण* परिणाम;
        int32_t len;
        यदि(U_SUCCESS(स्थितिः) && *current != 0) {
            परिणाम = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            यदि(परिणाम_दीर्घता != शून्यसूचकः) {
                *परिणाम_दीर्घता = len;
            }
        } अन्यथा {
            यदि(परिणाम_दीर्घता != शून्यसूचकः) {
                *परिणाम_दीर्घता = 0;
            }
            परिणाम = शून्यसूचकः;
        }
        प्रत्यागम परिणाम;
    }

    आभास ध्रुव UnicodeString* snext(UError_संहिता& स्थितिः) U_LIFETIME_BOUND override {
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम शून्यसूचकः; }
        int32_t परिणाम_दीर्घता = 0;
        ध्रुव वर्ण *s = next(&परिणाम_दीर्घता, स्थितिः);
        प्रत्यागम समुच्चयः_Chars(s, परिणाम_दीर्घता, स्थितिः);
    }

    आभास शून्य reset(UError_संहिता& स्थितिः) override {
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
        current = keywords.दत्त();
    }
};

ध्रुव वर्ण KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = स्वभाव;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
वर्ग UnicodeKeywordEnumeration : सार्वजनिक KeywordEnumeration {
सार्वजनिक:
    उपयुज्य KeywordEnumeration::KeywordEnumeration;
    आभास ~UnicodeKeywordEnumeration();

    आभास ध्रुव वर्ण* next(int32_t* परिणाम_दीर्घता, UError_संहिता& स्थितिः) U_LIFETIME_BOUND override {
        ध्रुव वर्ण* legacy_कुञ्चिका = KeywordEnumeration::next(शून्यसूचकः, स्थितिः);
        यावत् (U_SUCCESS(स्थितिः) && legacy_कुञ्चिका != शून्यसूचकः) {
            ध्रुव वर्ण* कुञ्चिका = uloc_गन्तव्यम्_Unicode_Locale_कुञ्चिका(legacy_कुञ्चिका);
            यदि (कुञ्चिका != शून्यसूचकः) {
                यदि (परिणाम_दीर्घता != शून्यसूचकः) {
                    *परिणाम_दीर्घता = static_cast<int32_t>(uprv_strlen(कुञ्चिका));
                }
                प्रत्यागम कुञ्चिका;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_कुञ्चिका = KeywordEnumeration::next(शून्यसूचकः, स्थितिः);
        }
        यदि (परिणाम_दीर्घता != शून्यसूचकः) *परिणाम_दीर्घता = 0;
        प्रत्यागम शून्यसूचकः;
    }
    आभास int32_t count(UError_संहिता& स्थितिः) ध्रुव override {
        यदि (U_FAILURE(स्थितिः)) { प्रत्यागम 0; }
        ध्रुव वर्ण *kw = keywords.दत्त();
        int32_t परिणाम = 0;
        यावत्(*kw) {
            यदि (uloc_गन्तव्यम्_Unicode_Locale_कुञ्चिका(kw) != शून्यसूचकः) {
                परिणाम++;
            }
            kw += uprv_strlen(kw)+1;
        }
        प्रत्यागम परिणाम;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = स्वभाव;

StringEnumeration *
Locale::createKeywords(UError_संहिता &स्थितिः) ध्रुव
{
    StringEnumeration *परिणाम = शून्यसूचकः;

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम परिणाम;
    }

    ध्रुव वर्ण* variantStart = uprv_strchr(getName(), '@');
    ध्रुव वर्ण* assignment = uprv_strchr(getName(), '=');
    यदि(variantStart) {
        यदि(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', असत्य, स्थितिः);
            यदि (U_SUCCESS(स्थितिः) && !keywords.is_रिक्तानि()) {
                परिणाम = नूतन KeywordEnumeration(keywords.दत्त(), keywords.length(), 0, स्थितिः);
                यदि (!परिणाम) {
                    स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                }
            }
        } अन्यथा {
            स्थितिः = U_INVALID_FORMAT_ERROR;
        }
    }
    प्रत्यागम परिणाम;
}

StringEnumeration *
Locale::createUnicodeKeywords(UError_संहिता &स्थितिः) ध्रुव
{
    StringEnumeration *परिणाम = शून्यसूचकः;

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम परिणाम;
    }

    ध्रुव वर्ण* variantStart = uprv_strchr(getName(), '@');
    ध्रुव वर्ण* assignment = uprv_strchr(getName(), '=');
    यदि(variantStart) {
        यदि(assignment > variantStart) {
            CharString keywords = ulocimp_getKeywords(variantStart + 1, '@', असत्य, स्थितिः);
            यदि (U_SUCCESS(स्थितिः) && !keywords.is_रिक्तानि()) {
                परिणाम = नूतन UnicodeKeywordEnumeration(keywords.दत्त(), keywords.length(), 0, स्थितिः);
                यदि (!परिणाम) {
                    स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                }
            }
        } अन्यथा {
            स्थितिः = U_INVALID_FORMAT_ERROR;
        }
    }
    प्रत्यागम परिणाम;
}

int32_t
Locale::get_Keyword_मानम्(ध्रुव वर्ण* keywordName, वर्ण *बफर, int32_t bufLen, UError_संहिता &स्थितिः) ध्रुव
{
    प्रत्यागम uloc_get_Keyword_मानम्(getName(), keywordName, बफर, bufLen, &स्थितिः);
}

शून्य
Locale::get_Keyword_मानम्(StringPiece keywordName, ByteSink& sink, UError_संहिता& स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    यदि (isBogus()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    ulocimp_get_Keyword_मानम्(getName(), keywordName, sink, स्थितिः);
}

शून्य
Locale::get_Unicode_Keyword_मानम्(StringPiece keywordName,
                               ByteSink& sink,
                               UError_संहिता& स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    std::optional<std::string_view> legacy_कुञ्चिका = ulocimp_गन्तव्यम्_Legacy_कुञ्चिका_With_Fallback(keywordName);
    यदि (!legacy_कुञ्चिका.has_value()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    स्वचालित legacy_मानम् = get_Keyword_मानम्<CharString>(*legacy_कुञ्चिका, स्थितिः);

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    std::optional<std::string_view> unicode_मानम् =
        ulocimp_गन्तव्यम्_Bcp_Type_With_Fallback(keywordName, legacy_मानम्.गन्तव्यम्_String_Piece());
    यदि (!unicode_मानम्.has_value()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    sink.Append(unicode_मानम्->दत्त(), static_cast<int32_t>(unicode_मानम्->size()));
}

शून्य
Locale::समुच्चयः_Keyword_मानम्(StringPiece keywordName,
                        StringPiece keyword_मानम्,
                        UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
    यदि (keywordName.empty()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    यदि (स्थितिः == U_STRING_NOT_TERMINATED_WARNING) {
        स्थितिः = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, स्थितिः);
    ulocimp_समुच्चयः_Keyword_मानम्(keywordName, keyword_मानम्, localeID, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        यदि (स्थितिः == U_स्मृतिः_आवण्टनम्_ERROR) {
            समुच्चयः_गन्तव्यम्_Bogus();
        }
        प्रत्यागम;
    }

    ध्रुव वर्ण* at = locale_getKeywordsStart(localeID.गन्तव्यम्_String_Piece());
    तार्किक hasKeywords = at != शून्यसूचकः && uprv_strchr(at + 1, '=') != शून्यसूचकः;

    Nest* nest = payload.get<Nest>();
    यदि (!hasKeywords) {
        यदि (nest == शून्यसूचकः) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            Heap* heap = payload.get<Heap>();
            U_ASSERT(heap != शून्यसूचकः);
            यदि (Nest::माति(localeID.length(), heap->language, heap->script, heap->region)) {
                int32_t variantBegin = heap->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < परिमाण Nest::आधारः_Name);
                nest = &payload.emplace<Nest>(std::move(*heap), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->आधारः_Name, परिमाण Nest::आधारः_Name, स्थितिः);
            } अन्यथा {
                heap->ptr->आधारः_Name.clear();
                heap->ptr->पूर्णम्_Name = localeID.गन्तव्यम्_String_Piece();
                यदि (heap->ptr->पूर्णम्_Name.is_रिक्तानि()) {
                    स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                    समुच्चयः_गन्तव्यम्_Bogus();
                    प्रत्यागम;
                }
            }
        }
    } अन्यथा {
        Heap* heap = शून्यसूचकः;
        यदि (nest != शून्यसूचकः) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            heap = &payload.emplace<Heap>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            यदि (isBogus()) {
                स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
        } अन्यथा {
            heap = payload.get<Heap>();
        }
        U_ASSERT(heap != शून्यसूचकः);
        heap->ptr->पूर्णम्_Name = localeID.गन्तव्यम्_String_Piece();
        यदि (heap->ptr->पूर्णम्_Name.is_रिक्तानि()) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            समुच्चयः_गन्तव्यम्_Bogus();
            प्रत्यागम;
        }

        यदि (heap->ptr->आधारः_Name.is_रिक्तानि()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            यदि (std::string_view::size_type आधारः_Name_दीर्घता = at - localeID.दत्त(); आधारः_Name_दीर्घता > 0) {
                heap->ptr->आधारः_Name = {heap->ptr->पूर्णम्_Name.दत्त(), आधारः_Name_दीर्घता};
                यदि (heap->ptr->आधारः_Name.is_रिक्तानि()) {
                    स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                    समुच्चयः_गन्तव्यम्_Bogus();
                    प्रत्यागम;
                }
            }
        }
    }
}

शून्य
Locale::समुच्चयः_Unicode_Keyword_मानम्(StringPiece keywordName,
                               StringPiece keyword_मानम्,
                               UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    std::optional<std::string_view> legacy_कुञ्चिका = ulocimp_गन्तव्यम्_Legacy_कुञ्चिका_With_Fallback(keywordName);
    यदि (!legacy_कुञ्चिका.has_value()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    std::string_view value;

    यदि (!keyword_मानम्.empty()) {
        std::optional<std::string_view> legacy_मानम् =
            ulocimp_गन्तव्यम्_Legacy_Type_With_Fallback(keywordName, keyword_मानम्);
        यदि (!legacy_मानम्.has_value()) {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }
        value = *legacy_मानम्;
    }

    समुच्चयः_Keyword_मानम्(*legacy_कुञ्चिका, value, स्थितिः);
}

ध्रुव वर्ण*
Locale::getCountry() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम get_क्षेत्रम्<&Nest::getRegion, &Heap::getRegion>();
}

ध्रुव वर्ण*
Locale::getLanguage() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम get_क्षेत्रम्<&Nest::getLanguage, &Heap::getLanguage>();
}

ध्रुव वर्ण*
Locale::getScript() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम get_क्षेत्रम्<&Nest::getScript, &Heap::getScript>();
}

ध्रुव वर्ण*
Locale::getVariant() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम get_क्षेत्रम्<&Nest::getVariant, &Heap::getVariant>();
}

ध्रुव वर्ण*
Locale::getName() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम get_क्षेत्रम्<&Nest::get_आधारः_Name, &Heap::get_पूर्णम्_Name>();
}

ध्रुव वर्ण*
Locale::get_आधारः_Name() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम get_क्षेत्रम्<&Nest::get_आधारः_Name, &Heap::get_आधारः_Name>();
}

प्रतिरूपम् <ध्रुव वर्ण* (Locale::Nest::*ध्रुव NEST)() ध्रुव,
          ध्रुव वर्ण* (Locale::Heap::*ध्रुव HEAP)() ध्रुव>
ध्रुव वर्ण* Locale::get_क्षेत्रम्() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम payload.दर्शय([] { प्रत्यागम ""; },
                         [](ध्रुव Nest& nest) { प्रत्यागम (nest.*NEST)(); },
                         [](ध्रुव Heap& heap) { प्रत्यागम (heap.*HEAP)(); });
}

Locale::Iterator::~Iterator() = स्वभाव;

//eof
U_NAMESPACE_अन्तः
