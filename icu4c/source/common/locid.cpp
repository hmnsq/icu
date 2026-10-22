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

#అన్తర్భావ <cstddef>
#అన్తర్భావ <optional>
#అన్తర్భావ <string_view>
#అన్తర్భావ <type_traits>
#అన్తర్భావ <utility>

#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/localebuilder.h"
#అన్తర్భావ "unicode/localpointer.h"
#అన్తర్భావ "unicode/strenum.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "unicode/ures.h"

#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "charstrmap.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "fixedstring.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "ulocimp.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "uniquecharstr.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "uvector.h"

#ఒకవేళ UPRV_HAS_FEATURE(జ్ఞాపకం_sanitizer)
#అన్తర్భావ <sanitizer/msan_interface.h>
#అయితే_ముగింపు

U_NAMESPACE_BEGIN

స్థిర Locale   *gLocaleCache = శూన్యసూచిక;
స్థిర UInitOnce gLocaleCacheInitOnce {};

// gDefaultLocaleMutex protects all access to gDefaultLocalesHashT and gDefaultLocale.
స్థిర UMutex gDefaultLocaleMutex;
స్థిర UHashtable *gDefaultLocalesHashT = శూన్యసూచిక;
స్థిర Locale *gDefaultLocale = శూన్యసూచిక;

/**
 * \def ULOC_STRING_LIMIT
 * strings beyond this value crash in CharString
 */
#నిర్వచించు ULOC_STRING_హద్దు 357913941

U_NAMESPACE_చివర

రకనిర్వచనం గణన ELocalePos {
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
    e_మూలం,


    //eDEFAULT,
    eMAX_LOCALES
} ELocalePos;

నామాకాశ {

//
// Deleter function for Locales owned by the default Locale hash table/
//
శూన్య U_CALLCONV
deleteLocale(శూన్య *obj) {
    తొలగించు static_cast<icu::Locale*>(obj);
}

UBool U_CALLCONV locale_cleanup()
{
    U_NAMESPACE_USE

    తొలగించు [] gLocaleCache;
    gLocaleCache = శూన్యసూచిక;
    gLocaleCacheInitOnce.reset();

    ఒకవేళ (gDefaultLocalesHashT) {
        uhash_close(gDefaultLocalesHashT);   // Automatically deletes all elements, using deleter func.
        gDefaultLocalesHashT = శూన్యసూచిక;
    }
    gDefaultLocale = శూన్యసూచిక;
    తిరిగి నిజం;
}

శూన్య U_CALLCONV locale_init(UError_సంకేతం &స్థితి_2) {
    U_NAMESPACE_USE

    U_ASSERT(gLocaleCache == శూన్యసూచిక);
    gLocaleCache = కొత్త Locale[static_cast<పూర్ణాంకం>(eMAX_LOCALES)];
    ఒకవేళ (gLocaleCache == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    gLocaleCache[e_మూలం]          = Locale("");
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

Locale *locale_సమితి_default_అంతర్గత(స్థిరరాశి అక్షరం *id, UError_సంకేతం& స్థితి_2) {
    // Synchronize this entire function.
    Mutex తాళం(&gDefaultLocaleMutex);

    UBool canonicalize = అబద్ధం;

    // If given a nullptr string for the locale id, grab the default
    //   name from the system.
    //   (Different from most other locale APIs, where a null name means use
    //    the current ICU default locale.)
    ఒకవేళ (id == శూన్యసూచిక) {
        id = uprv_getDefaultLocaleID();   // This function not thread safe? TODO: verify.
        canonicalize = నిజం; // always canonicalize host ID
    }

    CharString localeNameBuf =
        canonicalize ? ulocimp_canonicalize(id, స్థితి_2) : ulocimp_getName(id, స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి gDefaultLocale;
    }

    ఒకవేళ (gDefaultLocalesHashT == శూన్యసూచిక) {
        gDefaultLocalesHashT = uhash_open(uhash_hashChars, uhash_compareChars, శూన్యసూచిక, &స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి gDefaultLocale;
        }
        uhash_సమితి_విలువ_Deleter(gDefaultLocalesHashT, deleteLocale);
        ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_LOCALE, locale_cleanup);
    }

    Locale* newDefault = static_cast<Locale*>(uhash_get(gDefaultLocalesHashT, localeNameBuf.దత్తాంశం()));
    ఒకవేళ (newDefault == శూన్యసూచిక) {
        newDefault = కొత్త Locale(Locale::eBOGUS);
        ఒకవేళ (newDefault == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి gDefaultLocale;
        }
        newDefault->init(localeNameBuf.దత్తాంశం(), అబద్ధం);
        uhash_put(gDefaultLocalesHashT, const_cast<అక్షరం*>(newDefault->getName()), newDefault, &స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి gDefaultLocale;
        }
    }
    gDefaultLocale = newDefault;
    తిరిగి gDefaultLocale;
}

U_NAMESPACE_చివర

/* sfb 07/21/99 */
U_CFUNC శూన్య
locale_సమితి_default(స్థిరరాశి అక్షరం *id)
{
    U_NAMESPACE_USE
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    locale_సమితి_default_అంతర్గత(id, స్థితి_2);
}
/*చివర*/

U_CFUNC స్థిరరాశి అక్షరం *
locale_get_default()
{
    U_NAMESPACE_USE
    తిరిగి Locale::getDefault().getName();
}

నామాకాశ {

మూస <స్వయంచాలక క్షేత్రం, రకం_పేరు T>
శూన్య copy_గమ్యం_Array(std::string_view sv, T* that) {
    స్వయంచాలక& క్షేత్రం_2 = that->*క్షేత్రం;
    constexpr size_t capacity = std::extent_v<std::remove_reference_t<decltype(క్షేత్రం_2)>>;
    static_assert(capacity > 0);
    ఒకవేళ (!sv.empty()) {
        U_ASSERT(sv.size() < capacity);
        uprv_memcpy(క్షేత్రం_2, sv.దత్తాంశం(), sv.size());
    }
    క్షేత్రం_2[sv.size()] = '\0';
}

} // namespace

U_NAMESPACE_BEGIN

శూన్య Locale::Nest::init(std::string_view language,
                        std::string_view script,
                        std::string_view region,
                        uint8_t variantBegin) {
    copy_గమ్యం_Array<&Nest::language>(language, ఇది);
    copy_గమ్యం_Array<&Nest::script>(script, ఇది);
    copy_గమ్యం_Array<&Nest::region>(region, ఇది);
    ఇది->variantBegin = variantBegin;
}

Locale::Nest::Nest(కుప్ప&& కుప్ప_2, uint8_t variantBegin) {
    // When moving from Heap to Nest the language field can be left untouched
    // (as it has the same offset in both) and only the script and region fields
    // need to be copied to their new locations, which is safe to do because the
    // new locations come before the old locations in memory and don't overlap.
    static_assert(offsetof(Nest, region) <= offsetof(కుప్ప, script));
    static_assert(offsetof(Nest, variantBegin) <= offsetof(కుప్ప, region));
    U_ASSERT(ఇది == reinterpret_cast<Nest*>(&కుప్ప_2));
#ఒకవేళ UPRV_HAS_FEATURE(జ్ఞాపకం_sanitizer)
    // Tell the sanitizer that we know that this memory really is OK to access.
    __msan_unpoison(&కుప్ప_2, పరిమాణం(కుప్ప));
#అయితే_ముగింపు
    copy_గమ్యం_Array<&Nest::script>(కుప్ప_2.script, ఇది);
    copy_గమ్యం_Array<&Nest::region>(కుప్ప_2.region, ఇది);
    ఇది->variantBegin = variantBegin;
    *ఇది->పునాది_Name = '\0';
}

నిర్మాణం Locale::కుప్ప::Alloc : బహిరంగ UMemory {
    FixedString నిండిన_Name;
    FixedString పునాది_Name;
    int32_t variantBegin;

    స్థిరరాశి అక్షరం* getVariant() స్థిరరాశి U_LIFETIME_BOUND {
        తిరిగి variantBegin == 0 ? "" : get_పునాది_Name() + variantBegin;
    }
    స్థిరరాశి అక్షరం* get_నిండిన_Name() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి నిండిన_Name.దత్తాంశం(); }
    స్థిరరాశి అక్షరం* get_పునాది_Name() స్థిరరాశి U_LIFETIME_BOUND {
        ఒకవేళ (పునాది_Name.is_ఖాళీలు()) {
            ఒకవేళ (స్థిరరాశి అక్షరం* name = నిండిన_Name.దత్తాంశం(); *name != '@') {
                తిరిగి name;
            }
        }
        తిరిగి పునాది_Name.దత్తాంశం();
    }

    Alloc(int32_t variantBegin) : నిండిన_Name(), పునాది_Name(), variantBegin(variantBegin) {}

    Alloc(స్థిరరాశి Alloc& other, UError_సంకేతం& స్థితి_2)
        : నిండిన_Name(), పునాది_Name(), variantBegin(other.variantBegin) {
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            ఒకవేళ (!other.నిండిన_Name.is_ఖాళీలు()) {
                నిండిన_Name = other.నిండిన_Name;
                ఒకవేళ (నిండిన_Name.is_ఖాళీలు()) {
                    స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                } లేకపోతే {
                    ఒకవేళ (!other.పునాది_Name.is_ఖాళీలు()) {
                        పునాది_Name = other.పునాది_Name;
                        ఒకవేళ (పునాది_Name.is_ఖాళీలు()) {
                            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                        }
                    }
                }
            }
        }
    }

    // Move should be done on the owner of the pointer to this object.
    Alloc(Alloc&&) noexcept = తొలగించు;

    ~Alloc() = అప్రమేయ;
};

స్థిరరాశి అక్షరం* Locale::కుప్ప::getVariant() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి ptr->getVariant(); }
స్థిరరాశి అక్షరం* Locale::కుప్ప::get_నిండిన_Name() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి ptr->get_నిండిన_Name(); }
స్థిరరాశి అక్షరం* Locale::కుప్ప::get_పునాది_Name() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి ptr->get_పునాది_Name(); }

Locale::కుప్ప::కుప్ప(std::string_view language,
                   std::string_view script,
                   std::string_view region,
                   int32_t variantBegin) {
    ptr = కొత్త Alloc(variantBegin);
    ఒకవేళ (ptr == శూన్యసూచిక) {
        type = eBOGUS;
    } లేకపోతే {
        type = e_కుప్ప;
        copy_గమ్యం_Array<&కుప్ప::language>(language, ఇది);
        copy_గమ్యం_Array<&కుప్ప::script>(script, ఇది);
        copy_గమ్యం_Array<&కుప్ప::region>(region, ఇది);
    }
}

Locale::కుప్ప::~కుప్ప() {
    U_ASSERT(type == e_కుప్ప);
    తొలగించు ptr;
}

Locale::కుప్ప& Locale::కుప్ప::క్రియాకారకం=(స్థిరరాశి కుప్ప& other) U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    ptr = కొత్త Alloc(*other.ptr, స్థితి_2);
    ఒకవేళ (ptr == శూన్యసూచిక || U_FAILURE(స్థితి_2)) {
        తొలగించు ptr;
    } లేకపోతే {
        type = e_కుప్ప;
        uprv_memcpy(language, other.language, పరిమాణం language);
        uprv_memcpy(script, other.script, పరిమాణం script);
        uprv_memcpy(region, other.region, పరిమాణం region);
    }
    తిరిగి *ఇది;
}

Locale::కుప్ప& Locale::కుప్ప::క్రియాకారకం=(కుప్ప&& other) noexcept U_LIFETIME_BOUND {
    U_ASSERT(type == eBOGUS);
    ptr = other.ptr;
    type = e_కుప్ప;
    other.type = eBOGUS;
    uprv_memcpy(language, other.language, పరిమాణం language);
    uprv_memcpy(script, other.script, పరిమాణం script);
    uprv_memcpy(region, other.region, పరిమాణం region);
    తిరిగి *ఇది;
}

మూస <రకం_పేరు BogusFn, రకం_పేరు NestFn, రకం_పేరు కుప్ప_Fn, రకం_పేరు... వాదనలు>
స్వయంచాలక Locale::Payload::సందర్శించు(BogusFn bogusFn, NestFn nestFn, కుప్ప_Fn కుప్ప_Fn_2, వాదనలు... వాదనలు_2) స్థిరరాశి {
    ఎంపిక (type) {
        స్థితి eBOGUS:
            తిరిగి bogusFn(వాదనలు_2...);
        స్థితి eNEST:
            తిరిగి nestFn(nest, వాదనలు_2...);
        స్థితి e_కుప్ప:
            తిరిగి కుప్ప_Fn_2(కుప్ప_2, వాదనలు_2...);
        అప్రమేయ:
            UPRV_UNREACHABLE_EXIT;
    };
}

శూన్య Locale::Payload::copy(స్థిరరాశి Payload& other) {
    other.సందర్శించు([](Payload*) {},
                [](స్థిరరాశి Nest& nest, Payload* dst) { dst->nest = nest; },
                [](స్థిరరాశి కుప్ప& కుప్ప_2, Payload* dst) { dst->కుప్ప_2 = కుప్ప_2; },
                ఇది);
}

శూన్య Locale::Payload::move(Payload&& other) noexcept {
    other.సందర్శించు(
        [](Payload*) {},
        [](స్థిరరాశి Nest& nest, Payload* dst) { dst->nest = nest; },
        [](స్థిరరాశి కుప్ప& కుప్ప_2, Payload* dst) { dst->కుప్ప_2 = std::move(const_cast<కుప్ప&>(కుప్ప_2)); },
        ఇది);
}

Locale::Payload::~Payload() {
    ఒకవేళ (type == e_కుప్ప) { కుప్ప_2.~కుప్ప(); }
}

Locale::Payload::Payload(స్థిరరాశి Payload& other) : type{eBOGUS} { copy(other); }
Locale::Payload::Payload(Payload&& other) noexcept : type{eBOGUS} { move(std::move(other)); }

Locale::Payload& Locale::Payload::క్రియాకారకం=(స్థిరరాశి Payload& other) U_LIFETIME_BOUND {
    ఒకవేళ (ఇది != &other) {
        సమితి_గమ్యం_Bogus();
        copy(other);
    }
    తిరిగి *ఇది;
}

Locale::Payload& Locale::Payload::క్రియాకారకం=(Payload&& other) noexcept U_LIFETIME_BOUND {
    ఒకవేళ (ఇది != &other) {
        సమితి_గమ్యం_Bogus();
        move(std::move(other));
    }
    తిరిగి *ఇది;
}

శూన్య Locale::Payload::సమితి_గమ్యం_Bogus() {
    ఇది->~Payload();
    type = eBOGUS;
}

మూస <రకం_పేరు T, రకం_పేరు... వాదనలు> T& Locale::Payload::emplace(వాదనలు&&... వాదనలు_2) U_LIFETIME_BOUND {
    ఒకవేళ constexpr (std::is_same_v<T, Nest>) {
        ఇది->~Payload();
        ::కొత్త (&nest) Nest(std::forward<వాదనలు>(వాదనలు_2)...);
        తిరిగి nest;
    }
    ఒకవేళ constexpr (std::is_same_v<T, కుప్ప>) {
        U_ASSERT(type != e_కుప్ప);
        ::కొత్త (&కుప్ప_2) కుప్ప(std::forward<వాదనలు>(వాదనలు_2)...);
        తిరిగి కుప్ప_2;
    }
}

మూస <> Locale::Nest* Locale::Payload::get() U_LIFETIME_BOUND {
    తిరిగి type == eNEST ? &nest : శూన్యసూచిక;
}
మూస <> Locale::కుప్ప* Locale::Payload::get() U_LIFETIME_BOUND {
    తిరిగి type == e_కుప్ప ? &కుప్ప_2 : శూన్యసూచిక;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale)

/*Character separating the posix id fields*/
// '_'
// In the platform codepage.
#నిర్వచించు SEP_CHAR '_'
#నిర్వచించు NULL_CHAR '\0'

Locale::~Locale() = అప్రమేయ;

Locale::Locale()
    : UObject(), payload()
{
    init(శూన్యసూచిక, అబద్ధం);
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


Locale::Locale( స్థిరరాశి   అక్షరం * newLanguage,
                స్థిరరాశి   అక్షరం * newCountry,
                స్థిరరాశి   అక్షరం * newVariant,
                స్థిరరాశి   అక్షరం * newKeywords)
    : UObject(), payload()
{
    ఒకవేళ( (newLanguage==శూన్యసూచిక) && (newCountry == శూన్యసూచిక) && (newVariant == శూన్యసూచిక) )
    {
        init(శూన్యసూచిక, అబద్ధం); /* shortcut */
    }
    లేకపోతే
    {
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        int32_t lsize = 0;
        int32_t csize = 0;
        int32_t vsize = 0;
        int32_t ksize = 0;

        // Check the sizes of the input strings.

        // Language
        ఒకవేళ ( newLanguage != శూన్యసూచిక )
        {
            lsize = static_cast<int32_t>(uprv_strlen(newLanguage));
            ఒకవేళ ( lsize < 0 || lsize > ULOC_STRING_హద్దు ) { // int32 wrap
                తిరిగి;
            }
        }

        CharString togo(newLanguage, lsize, స్థితి_2); // start with newLanguage

        // _Country
        ఒకవేళ ( newCountry != శూన్యసూచిక )
        {
            csize = static_cast<int32_t>(uprv_strlen(newCountry));
            ఒకవేళ ( csize < 0 || csize > ULOC_STRING_హద్దు ) { // int32 wrap
                తిరిగి;
            }
        }

        // _Variant
        ఒకవేళ ( newVariant != శూన్యసూచిక )
        {
            // remove leading _'s
            అయితే(newVariant[0] == SEP_CHAR)
            {
                newVariant++;
            }

            // remove trailing _'s
            vsize = static_cast<int32_t>(uprv_strlen(newVariant));
            ఒకవేళ ( vsize < 0 || vsize > ULOC_STRING_హద్దు ) { // int32 wrap
                తిరిగి;
            }
            అయితే( (vsize>1) && (newVariant[vsize-1] == SEP_CHAR) )
            {
                vsize--;
            }
        }

        ఒకవేళ ( newKeywords != శూన్యసూచిక)
        {
            ksize = static_cast<int32_t>(uprv_strlen(newKeywords));
            ఒకవేళ ( ksize < 0 || ksize > ULOC_STRING_హద్దు ) {
              తిరిగి;
            }
        }

        // We've checked the input sizes, now build up the full locale string..

        // newLanguage is already copied

        ఒకవేళ ( ( vsize != 0 ) || (csize != 0) )  // at least:  __v
        {                                      //            ^
            togo.append(SEP_CHAR, స్థితి_2);
        }

        ఒకవేళ ( csize != 0 )
        {
            togo.append(newCountry, స్థితి_2);
        }

        ఒకవేళ ( vsize != 0)
        {
            togo.append(SEP_CHAR, స్థితి_2)
                .append(newVariant, vsize, స్థితి_2);
        }

        ఒకవేళ ( ksize != 0)
        {
            ఒకవేళ (uprv_strchr(newKeywords, '=')) {
                togo.append('@', స్థితి_2); /* keyword parsing */
            }
            లేకపోతే {
                togo.append('_', స్థితి_2); /* Variant parsing with a script */
                ఒకవేళ ( vsize == 0) {
                    togo.append('_', స్థితి_2); /* No country found */
                }
            }
            togo.append(newKeywords, స్థితి_2);
        }

        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            // Something went wrong with appending, etc.
            తిరిగి;
        }
        // Parse it, because for example 'language' might really be a complete
        // string.
        init(togo.దత్తాంశం(), అబద్ధం);
    }
}

Locale::Locale(స్థిరరాశి Locale&) = అప్రమేయ;
Locale::Locale(Locale&&) noexcept = అప్రమేయ;

Locale& Locale::క్రియాకారకం=(స్థిరరాశి Locale&) U_LIFETIME_BOUND = అప్రమేయ;
Locale& Locale::క్రియాకారకం=(Locale&&) noexcept U_LIFETIME_BOUND = అప్రమేయ;

Locale *
Locale::clone() స్థిరరాశి {
    తిరిగి కొత్త Locale(*ఇది);
}

తార్కిక
Locale::క్రియాకారకం==( స్థిరరాశి   Locale& other) స్థిరరాశి
{
    తిరిగి uprv_strcmp(other.getName(), getName()) == 0;
}

నామాకాశ {

UInitOnce g_తెలిసిన_Canonicalized_Init_Once {};
UHashtable *g_తెలిసిన_Canonicalized = శూన్యసూచిక;

constexpr స్థిరరాశి అక్షరం* తెలిసిన_CANONICALIZED[] = {
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

UBool U_CALLCONV cleanup_తెలిసిన_Canonicalized() {
    g_తెలిసిన_Canonicalized_Init_Once.reset();
    ఒకవేళ (g_తెలిసిన_Canonicalized) { uhash_close(g_తెలిసిన_Canonicalized); }
    తిరిగి నిజం;
}

శూన్య U_CALLCONV ఎక్కించు_తెలిసిన_Canonicalized(UError_సంకేతం &స్థితి_2) {
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_LOCALE_తెలిసిన_CANONICALIZED,
                                cleanup_తెలిసిన_Canonicalized);
    Local_UHashtable_సూచిక new_తెలిసిన_Canonicalized_Map(
        uhash_open(uhash_hashChars, uhash_compareChars, శూన్యసూచిక, &స్థితి_2));
    కొరకు (int32_t i = 0;
            U_SUCCESS(స్థితి_2) && i < UPRV_LENGTHOF(తెలిసిన_CANONICALIZED);
            i++) {
        uhash_puti(new_తెలిసిన_Canonicalized_Map.getAlias(),
                   (శూన్య*)తెలిసిన_CANONICALIZED[i],
                   1, &స్థితి_2);
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    g_తెలిసిన_Canonicalized = new_తెలిసిన_Canonicalized_Map.orphan();
}

తరగతి Alias_దత్తాంశం;

/**
 * A Builder class to build the alias data.
 */
తరగతి Alias_దత్తాంశం_Builder {
బహిరంగ:
    Alias_దత్తాంశం_Builder() {
    }

    // Build the AliasData from resource.
    Alias_దత్తాంశం* build(UError_సంకేతం &స్థితి_2);

అంతరంగ:
    శూన్య readAlias(UResourceBundle* alias,
                   Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
                   Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
                   Local_జ్ఞాపకం<int32_t>& replacementIndexes,
                   int32_t &length,
                   శూన్య (*checkType)(స్థిరరాశి అక్షరం* type),
                   శూన్య (*checkReplacement)(స్థిరరాశి UChar* replacement),
                   UError_సంకేతం &స్థితి_2);

    // Read the languageAlias data from alias to
    // strings+types+replacementIndexes
    // The number of record will be stored into length.
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    శూన్య readLanguageAlias(UResourceBundle* alias,
                           Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
                           Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
                           Local_జ్ఞాపకం<int32_t>& replacementIndexes,
                           int32_t &length,
                           UError_సంకేతం &స్థితి_2);

    // Read the scriptAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    శూన్య readScriptAlias(UResourceBundle* alias,
                         Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
                         Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
                         Local_జ్ఞాపకం<int32_t>& replacementIndexes,
                         int32_t &length, UError_సంకేతం &స్థితి_2);

    // Read the territoryAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement script.
    శూన్య readTerritoryAlias(UResourceBundle* alias,
                            Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
                            Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
                            Local_జ్ఞాపకం<int32_t>& replacementIndexes,
                            int32_t &length, UError_సంకేతం &స్థితి_2);

    // Read the variantAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    శూన్య readVariantAlias(UResourceBundle* alias,
                          Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
                          Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
                          Local_జ్ఞాపకం<int32_t>& replacementIndexes,
                          int32_t &length, UError_సంకేతం &స్థితి_2);

    // Read the subdivisionAlias data from alias to
    // strings+types+replacementIndexes
    // Allocate length items for types, to store the type field.
    // Allocate length items for replacementIndexes,
    // to store the index in the strings for the replacement variant.
    శూన్య readSubdivisionAlias(UResourceBundle* alias,
                          Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
                          Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
                          Local_జ్ఞాపకం<int32_t>& replacementIndexes,
                          int32_t &length, UError_సంకేతం &స్థితి_2);
};

/**
 * A class to hold the Alias Data.
 */
తరగతి Alias_దత్తాంశం : బహిరంగ UMemory {
బహిరంగ:
    స్థిర స్థిరరాశి Alias_దత్తాంశం* singleton(UError_సంకేతం& స్థితి_2) {
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            // Do not get into loadData if the status already has error.
            తిరిగి శూన్యసూచిక;
        }
        umtx_initOnce(Alias_దత్తాంశం::gInitOnce, &Alias_దత్తాంశం::ఎక్కించు_దత్తాంశం, స్థితి_2);
        తిరిగి gSingleton;
    }

    స్థిరరాశి CharStringMap& languageMap() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి language; }
    స్థిరరాశి CharStringMap& scriptMap() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి script; }
    స్థిరరాశి CharStringMap& territoryMap() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి territory; }
    స్థిరరాశి CharStringMap& variantMap() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి variant; }
    స్థిరరాశి CharStringMap& subdivisionMap() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి subdivision; }

    స్థిర శూన్య U_CALLCONV ఎక్కించు_దత్తాంశం(UError_సంకేతం &స్థితి_2);
    స్థిర UBool U_CALLCONV cleanup();

    స్థిర UInitOnce gInitOnce;

అంతరంగ:
    Alias_దత్తాంశం(CharStringMap languageMap,
              CharStringMap scriptMap,
              CharStringMap territoryMap,
              CharStringMap variantMap,
              CharStringMap subdivisionMap,
              CharString* అక్షరశ్రేణులు)
        : language(std::move(languageMap)),
          script(std::move(scriptMap)),
          territory(std::move(territoryMap)),
          variant(std::move(variantMap)),
          subdivision(std::move(subdivisionMap)),
          అక్షరశ్రేణులు(అక్షరశ్రేణులు) {
    }

    ~Alias_దత్తాంశం() {
        తొలగించు అక్షరశ్రేణులు;
    }

    స్థిర స్థిరరాశి Alias_దత్తాంశం* gSingleton;

    CharStringMap language;
    CharStringMap script;
    CharStringMap territory;
    CharStringMap variant;
    CharStringMap subdivision;
    CharString* అక్షరశ్రేణులు;

    మిత్ర తరగతి Alias_దత్తాంశం_Builder;
};


స్థిరరాశి Alias_దత్తాంశం* Alias_దత్తాంశం::gSingleton = శూన్యసూచిక;
UInitOnce Alias_దత్తాంశం::gInitOnce {};

UBool U_CALLCONV
Alias_దత్తాంశం::cleanup()
{
    gInitOnce.reset();
    తొలగించు gSingleton;
    తిరిగి నిజం;
}

శూన్య
Alias_దత్తాంశం_Builder::readAlias(
        UResourceBundle* alias,
        Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
        Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
        Local_జ్ఞాపకం<int32_t>& replacementIndexes,
        int32_t &length,
        శూన్య (*checkType)(స్థిరరాశి అక్షరం* type),
        శూన్య (*checkReplacement)(స్థిరరాశి UChar* replacement),
        UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    length = ures_get_కొలత(alias);
    స్థిరరాశి అక్షరం** rawTypes = types.allocateInsteadAndCopy(length);
    ఒకవేళ (rawTypes == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    int32_t* rawIndexes = replacementIndexes.allocateInsteadAndCopy(length);
    ఒకవేళ (rawIndexes == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    కొరకు (పూర్ణాంకం i = 0; U_SUCCESS(స్థితి_2) && ures_has_తదుపరి(alias); i++) {
        Local_UResource_Bundle_సూచిక res(
            ures_get_తదుపరి_Resource(alias, శూన్యసూచిక, &స్థితి_2));
        స్థిరరాశి అక్షరం* aliasFrom = ures_get_కీలకం(res.getAlias());
        స్థిరరాశి UChar* alias_గమ్యం =
            ures_get_String_By_కీలకం(res.getAlias(), "replacement", శూన్యసూచిక, &స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;

        checkType(aliasFrom);
        checkReplacement(alias_గమ్యం);

        rawTypes[i] = aliasFrom;
        rawIndexes[i] = అక్షరశ్రేణులు->కూడు(alias_గమ్యం, స్థితి_2);
    }
}

/**
 * Read the languageAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement language.
 */
శూన్య
Alias_దత్తాంశం_Builder::readLanguageAlias(
        UResourceBundle* alias,
        Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
        Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
        Local_జ్ఞాపకం<int32_t>& replacementIndexes,
        int32_t &length,
        UError_సంకేతం &స్థితి_2)
{
    తిరిగి readAlias(
        alias, అక్షరశ్రేణులు, types, replacementIndexes, length,
#ఒకవేళ U_DEBUG
        [](స్థిరరాశి అక్షరం* type) {
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
#లేకపోతే
        [](స్థిరరాశి అక్షరం*) {},
#అయితే_ముగింపు
        [](స్థిరరాశి UChar*) {}, స్థితి_2);
}

/**
 * Read the scriptAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement script.
 */
శూన్య
Alias_దత్తాంశం_Builder::readScriptAlias(
        UResourceBundle* alias,
        Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
        Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
        Local_జ్ఞాపకం<int32_t>& replacementIndexes,
        int32_t &length,
        UError_సంకేతం &స్థితి_2)
{
    తిరిగి readAlias(
        alias, అక్షరశ్రేణులు, types, replacementIndexes, length,
#ఒకవేళ U_DEBUG
        [](స్థిరరాశి అక్షరం* type) {
            U_ASSERT(uprv_strlen(type) == 4);
        },
        [](స్థిరరాశి UChar* replacement) {
            U_ASSERT(u_strlen(replacement) == 4);
        },
#లేకపోతే
        [](స్థిరరాశి అక్షరం*) {},
        [](స్థిరరాశి UChar*) { },
#అయితే_ముగింపు
        స్థితి_2);
}

/**
 * Read the territoryAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
శూన్య
Alias_దత్తాంశం_Builder::readTerritoryAlias(
        UResourceBundle* alias,
        Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
        Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
        Local_జ్ఞాపకం<int32_t>& replacementIndexes,
        int32_t &length,
        UError_సంకేతం &స్థితి_2)
{
    తిరిగి readAlias(
        alias, అక్షరశ్రేణులు, types, replacementIndexes, length,
#ఒకవేళ U_DEBUG
        [](స్థిరరాశి అక్షరం* type) {
            U_ASSERT(uprv_strlen(type) == 2 || uprv_strlen(type) == 3);
        },
#లేకపోతే
        [](స్థిరరాశి అక్షరం*) {},
#అయితే_ముగింపు
        [](స్థిరరాశి UChar*) { },
        స్థితి_2);
}

/**
 * Read the variantAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement variant.
 */
శూన్య
Alias_దత్తాంశం_Builder::readVariantAlias(
        UResourceBundle* alias,
        Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
        Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
        Local_జ్ఞాపకం<int32_t>& replacementIndexes,
        int32_t &length,
        UError_సంకేతం &స్థితి_2)
{
    తిరిగి readAlias(
        alias, అక్షరశ్రేణులు, types, replacementIndexes, length,
#ఒకవేళ U_DEBUG
        [](స్థిరరాశి అక్షరం* type) {
            U_ASSERT(uprv_strlen(type) >= 4 && uprv_strlen(type) <= 8);
            U_ASSERT(uprv_strlen(type) != 4 ||
                     (type[0] >= '0' && type[0] <= '9'));
        },
        [](స్థిరరాశి UChar* replacement) {
            int32_t len = u_strlen(replacement);
            U_ASSERT(len >= 4 && len <= 8);
            U_ASSERT(len != 4 ||
                     (*replacement >= u'0' &&
                      *replacement <= u'9'));
        },
#లేకపోతే
        [](స్థిరరాశి అక్షరం*) {},
        [](స్థిరరాశి UChar*) { },
#అయితే_ముగింపు
        స్థితి_2);
}

/**
 * Read the subdivisionAlias data from alias to strings+types+replacementIndexes.
 * Allocate length items for types, to store the type field. Allocate length
 * items for replacementIndexes, to store the index in the strings for the
 * replacement regions.
 */
శూన్య
Alias_దత్తాంశం_Builder::readSubdivisionAlias(
        UResourceBundle* alias,
        Unique_Char_అక్షరశ్రేణులు* అక్షరశ్రేణులు,
        Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*>& types,
        Local_జ్ఞాపకం<int32_t>& replacementIndexes,
        int32_t &length,
        UError_సంకేతం &స్థితి_2)
{
    తిరిగి readAlias(
        alias, అక్షరశ్రేణులు, types, replacementIndexes, length,
#ఒకవేళ U_DEBUG
        [](స్థిరరాశి అక్షరం* type) {
            U_ASSERT(uprv_strlen(type) >= 3 && uprv_strlen(type) <= 8);
        },
#లేకపోతే
        [](స్థిరరాశి అక్షరం*) {},
#అయితే_ముగింపు
        [](స్థిరరాశి UChar*) { },
        స్థితి_2);
}

/**
 * Initializes the alias data from the ICU resource bundles. The alias data
 * contains alias of language, country, script and variants.
 *
 * If the alias data has already loaded, then this method simply returns without
 * doing anything meaningful.
 */
శూన్య U_CALLCONV
Alias_దత్తాంశం::ఎక్కించు_దత్తాంశం(UError_సంకేతం &స్థితి_2)
{
#నిర్వచితమైతే LOCALE_CANONICALIZATION_DEBUG
    UDate ప్రారంభం = uprv_getRawUTCtime();
#అయితే_ముగింపు  // LOCALE_CANONICALIZATION_DEBUG
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_LOCALE_ALIAS, cleanup);
    Alias_దత్తాంశం_Builder builder;
    gSingleton = builder.build(స్థితి_2);
#నిర్వచితమైతే LOCALE_CANONICALIZATION_DEBUG
    UDate చివర = uprv_getRawUTCtime();
    రాయి("AliasData::loadData took total %f ms\n", చివర - ప్రారంభం);
#అయితే_ముగింపు  // LOCALE_CANONICALIZATION_DEBUG
}

/**
 * Build the alias data from resources.
 */
Alias_దత్తాంశం*
Alias_దత్తాంశం_Builder::build(UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి శూన్యసూచిక; }

    Local_UResource_Bundle_సూచిక metadata(
        ures_open_ప్రత్యక్ష(శూన్యసూచిక, "metadata", &స్థితి_2));
    Local_UResource_Bundle_సూచిక metadataAlias(
        ures_get_By_కీలకం(metadata.getAlias(), "alias", శూన్యసూచిక, &స్థితి_2));
    Local_UResource_Bundle_సూచిక languageAlias(
        ures_get_By_కీలకం(metadataAlias.getAlias(), "language", శూన్యసూచిక, &స్థితి_2));
    Local_UResource_Bundle_సూచిక scriptAlias(
        ures_get_By_కీలకం(metadataAlias.getAlias(), "script", శూన్యసూచిక, &స్థితి_2));
    Local_UResource_Bundle_సూచిక territoryAlias(
        ures_get_By_కీలకం(metadataAlias.getAlias(), "territory", శూన్యసూచిక, &స్థితి_2));
    Local_UResource_Bundle_సూచిక variantAlias(
        ures_get_By_కీలకం(metadataAlias.getAlias(), "variant", శూన్యసూచిక, &స్థితి_2));
    Local_UResource_Bundle_సూచిక subdivisionAlias(
        ures_get_By_కీలకం(metadataAlias.getAlias(), "subdivision", శూన్యసూచిక, &స్థితి_2));

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    int32_t languages_నిడివి = 0, script_నిడివి = 0, territory_నిడివి = 0,
            variant_నిడివి = 0, subdivision_నిడివి = 0;

    // Read the languageAlias into languageTypes, languageReplacementIndexes
    // and strings
    Unique_Char_అక్షరశ్రేణులు అక్షరశ్రేణులు(స్థితి_2);
    Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*> languageTypes;
    Local_జ్ఞాపకం<int32_t> languageReplacementIndexes;
    readLanguageAlias(languageAlias.getAlias(),
                      &అక్షరశ్రేణులు,
                      languageTypes,
                      languageReplacementIndexes,
                      languages_నిడివి,
                      స్థితి_2);

    // Read the scriptAlias into scriptTypes, scriptReplacementIndexes
    // and strings
    Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*> scriptTypes;
    Local_జ్ఞాపకం<int32_t> scriptReplacementIndexes;
    readScriptAlias(scriptAlias.getAlias(),
                    &అక్షరశ్రేణులు,
                    scriptTypes,
                    scriptReplacementIndexes,
                    script_నిడివి,
                    స్థితి_2);

    // Read the territoryAlias into territoryTypes, territoryReplacementIndexes
    // and strings
    Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*> territoryTypes;
    Local_జ్ఞాపకం<int32_t> territoryReplacementIndexes;
    readTerritoryAlias(territoryAlias.getAlias(),
                       &అక్షరశ్రేణులు,
                       territoryTypes,
                       territoryReplacementIndexes,
                       territory_నిడివి, స్థితి_2);

    // Read the variantAlias into variantTypes, variantReplacementIndexes
    // and strings
    Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*> variantTypes;
    Local_జ్ఞాపకం<int32_t> variantReplacementIndexes;
    readVariantAlias(variantAlias.getAlias(),
                     &అక్షరశ్రేణులు,
                     variantTypes,
                     variantReplacementIndexes,
                     variant_నిడివి, స్థితి_2);

    // Read the subdivisionAlias into subdivisionTypes, subdivisionReplacementIndexes
    // and strings
    Local_జ్ఞాపకం<స్థిరరాశి అక్షరం*> subdivisionTypes;
    Local_జ్ఞాపకం<int32_t> subdivisionReplacementIndexes;
    readSubdivisionAlias(subdivisionAlias.getAlias(),
                         &అక్షరశ్రేణులు,
                         subdivisionTypes,
                         subdivisionReplacementIndexes,
                         subdivision_నిడివి, స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    // We can only use strings after freeze it.
    అక్షరశ్రేణులు.freeze();

    // Build the languageMap from languageTypes & languageReplacementIndexes
    CharStringMap languageMap(490, స్థితి_2);
    కొరకు (int32_t i = 0; U_SUCCESS(స్థితి_2) && i < languages_నిడివి; i++) {
        languageMap.put(languageTypes[i],
                        అక్షరశ్రేణులు.get(languageReplacementIndexes[i]),
                        స్థితి_2);
    }

    // Build the scriptMap from scriptTypes & scriptReplacementIndexes
    CharStringMap scriptMap(1, స్థితి_2);
    కొరకు (int32_t i = 0; U_SUCCESS(స్థితి_2) && i < script_నిడివి; i++) {
        scriptMap.put(scriptTypes[i],
                      అక్షరశ్రేణులు.get(scriptReplacementIndexes[i]),
                      స్థితి_2);
    }

    // Build the territoryMap from territoryTypes & territoryReplacementIndexes
    CharStringMap territoryMap(650, స్థితి_2);
    కొరకు (int32_t i = 0; U_SUCCESS(స్థితి_2) && i < territory_నిడివి; i++) {
        territoryMap.put(territoryTypes[i],
                         అక్షరశ్రేణులు.get(territoryReplacementIndexes[i]),
                         స్థితి_2);
    }

    // Build the variantMap from variantTypes & variantReplacementIndexes.
    CharStringMap variantMap(2, స్థితి_2);
    కొరకు (int32_t i = 0; U_SUCCESS(స్థితి_2) && i < variant_నిడివి; i++) {
        variantMap.put(variantTypes[i],
                       అక్షరశ్రేణులు.get(variantReplacementIndexes[i]),
                       స్థితి_2);
    }

    // Build the subdivisionMap from subdivisionTypes & subdivisionReplacementIndexes.
    CharStringMap subdivisionMap(2, స్థితి_2);
    కొరకు (int32_t i = 0; U_SUCCESS(స్థితి_2) && i < subdivision_నిడివి; i++) {
        subdivisionMap.put(subdivisionTypes[i],
                       అక్షరశ్రేణులు.get(subdivisionReplacementIndexes[i]),
                       స్థితి_2);
    }

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    // copy hashtables
    స్వయంచాలక *దత్తాంశం = కొత్త Alias_దత్తాంశం(
        std::move(languageMap),
        std::move(scriptMap),
        std::move(territoryMap),
        std::move(variantMap),
        std::move(subdivisionMap),
        అక్షరశ్రేణులు.orphan_Char_అక్షరశ్రేణులు());

    ఒకవేళ (దత్తాంశం == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    తిరిగి దత్తాంశం;
}

/**
 * A class that find the replacement values of locale fields by using AliasData.
 */
తరగతి AliasReplacer {
బహిరంగ:
    AliasReplacer(UError_సంకేతం& స్థితి_2) :
            language(శూన్యసూచిక), script(శూన్యసూచిక), region(శూన్యసూచిక),
            extensions(శూన్యసూచిక),
            // store value in variants only once
            variants(శూన్యసూచిక,
                     ([](UElement e1, UElement e2) -> UBool {
                       తిరిగి 0==uprv_strcmp((స్థిరరాశి అక్షరం*)e1.సూచిక,
                                             (స్థిరరాశి అక్షరం*)e2.సూచిక);}),
                     స్థితి_2),
            దత్తాంశం(శూన్యసూచిక) {
    }
    ~AliasReplacer() {
    }

    // Check the fields inside locale, if need to replace fields,
    // place the replaced locale ID in out and return true.
    // Otherwise return false for no replacement or error.
    తార్కిక replace(
        స్థిరరాశి Locale& locale, CharString& out, UError_సంకేతం& స్థితి_2);

అంతరంగ:
    స్థిరరాశి అక్షరం* language;
    స్థిరరాశి అక్షరం* script;
    స్థిరరాశి అక్షరం* region;
    స్థిరరాశి అక్షరం* extensions;
    UVector variants;

    స్థిరరాశి Alias_దత్తాంశం* దత్తాంశం;

    అంతఃవరుస తార్కిక not_ఖాళీలు(స్థిరరాశి అక్షరం* str) {
        తిరిగి str && str[0] != NULL_CHAR;
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
    అంతఃవరుస స్థిరరాశి అక్షరం* deleteOrReplace(స్థిరరాశి అక్షరం* input U_LIFETIME_BOUND,
                                       స్థిరరాశి అక్షరం* type,
                                       స్థిరరాశి అక్షరం* replacement U_LIFETIME_BOUND) {
        తిరిగి not_ఖాళీలు(replacement) ?
            ((input == శూన్యసూచిక) ?  replacement : input) :
            ((type == శూన్యసూచిక) ? input  : శూన్యసూచిక);
    }

    అంతఃవరుస తార్కిక same(స్థిరరాశి అక్షరం* a, స్థిరరాశి అక్షరం* b) {
        ఒకవేళ (a == శూన్యసూచిక && b == శూన్యసూచిక) {
            తిరిగి నిజం;
        }
        ఒకవేళ ((a == శూన్యసూచిక && b != శూన్యసూచిక) ||
            (a != శూన్యసూచిక && b == శూన్యసూచిక)) {
          తిరిగి అబద్ధం;
        }
        తిరిగి uprv_strcmp(a, b) == 0;
    }

    // Gather fields and generate locale ID into out.
    CharString& output_గమ్యం_String(CharString& out U_LIFETIME_BOUND, UError_సంకేతం& స్థితి_2);

    // Generate the lookup key.
    CharString& generate_కీలకం(స్థిరరాశి అక్షరం* language, స్థిరరాశి అక్షరం* region,
                            స్థిరరాశి అక్షరం* variant, CharString& out U_LIFETIME_BOUND,
                            UError_సంకేతం& స్థితి_2);

    శూన్య parseLanguageReplacement(స్థిరరాశి అక్షరం* replacement,
                                  స్థిరరాశి అక్షరం*& replaceLanguage,
                                  స్థిరరాశి అక్షరం*& replaceScript,
                                  స్థిరరాశి అక్షరం*& replaceRegion,
                                  స్థిరరాశి అక్షరం*& replaceVariant,
                                  స్థిరరాశి అక్షరం*& replaceExtensions,
                                  UVector& గమ్యం_Be_Freed,
                                  UError_సంకేతం& స్థితి_2);

    // Replace by using languageAlias.
    తార్కిక replaceLanguage(తార్కిక checkLanguage, తార్కిక checkRegion,
                         తార్కిక checkVariants, UVector& గమ్యం_Be_Freed,
                         UError_సంకేతం& స్థితి_2);

    // Replace by using territoryAlias.
    తార్కిక replaceTerritory(UVector& గమ్యం_Be_Freed, UError_సంకేతం& స్థితి_2);

    // Replace by using scriptAlias.
    తార్కిక replaceScript(UError_సంకేతం& స్థితి_2);

    // Replace by using variantAlias.
    తార్కిక replaceVariant(UError_సంకేతం& స్థితి_2);

    // Replace by using subdivisionAlias.
    తార్కిక replaceSubdivision(StringPiece subdivision,
                            CharString& output, UError_సంకేతం& స్థితి_2);

    // Replace transformed extensions.
    తార్కిక replaceTransformedExtensions(
        CharString& transformedExtensions, CharString& output, UError_సంకేతం& స్థితి_2);
};

CharString&
AliasReplacer::generate_కీలకం(
        స్థిరరాశి అక్షరం* language, స్థిరరాశి అక్షరం* region, స్థిరరాశి అక్షరం* variant,
        CharString& out U_LIFETIME_BOUND, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి out; }
    out.append(language, స్థితి_2);
    ఒకవేళ (not_ఖాళీలు(region)) {
        out.append(SEP_CHAR, స్థితి_2)
            .append(region, స్థితి_2);
    }
    ఒకవేళ (not_ఖాళీలు(variant)) {
       out.append(SEP_CHAR, స్థితి_2)
           .append(variant, స్థితి_2);
    }
    తిరిగి out;
}

శూన్య
AliasReplacer::parseLanguageReplacement(
    స్థిరరాశి అక్షరం* replacement,
    స్థిరరాశి అక్షరం*& replacedLanguage,
    స్థిరరాశి అక్షరం*& replacedScript,
    స్థిరరాశి అక్షరం*& replacedRegion,
    స్థిరరాశి అక్షరం*& replacedVariant,
    స్థిరరాశి అక్షరం*& replacedExtensions,
    UVector& గమ్యం_Be_Freed,
    UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    replacedScript = replacedRegion = replacedVariant
        = replacedExtensions = శూన్యసూచిక;
    ఒకవేళ (uprv_strchr(replacement, '_') == శూన్యసూచిక) {
        replacedLanguage = replacement;
        // reach the end, just return it.
        తిరిగి;
    }
    // We have multiple field so we have to allocate and parse
    స్వయంచాలక str = prv::make_unique<CharString>(
        replacement, static_cast<int32_t>(uprv_strlen(replacement)), స్థితి_2, స్థితి_2);
    అక్షరం* దత్తాంశం = str->దత్తాంశం();
    గమ్యం_Be_Freed.adoptElement(str.release(), స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    replacedLanguage = (స్థిరరాశి అక్షరం*) దత్తాంశం;
    అక్షరం* చివర_యొక్క_క్షేత్రం = uprv_strchr(దత్తాంశం, '_');
    *చివర_యొక్క_క్షేత్రం = '\0'; // null terminiate it.
    చివర_యొక్క_క్షేత్రం++;
    స్థిరరాశి అక్షరం* ప్రారంభం = చివర_యొక్క_క్షేత్రం;
    చివర_యొక్క_క్షేత్రం = const_cast<అక్షరం*>(uprv_strchr(ప్రారంభం, '_'));
    size_t len = 0;
    ఒకవేళ (చివర_యొక్క_క్షేత్రం == శూన్యసూచిక) {
        len = uprv_strlen(ప్రారంభం);
    } లేకపోతే {
        len = చివర_యొక్క_క్షేత్రం - ప్రారంభం;
        *చివర_యొక్క_క్షేత్రం = '\0'; // null terminiate it.
    }
    ఒకవేళ (len == 4 && uprv_isASCIILetter(*ప్రారంభం)) {
        // Got a script
        replacedScript = ప్రారంభం;
        ఒకవేళ (చివర_యొక్క_క్షేత్రం == శూన్యసూచిక) {
            తిరిగి;
        }
        ప్రారంభం = చివర_యొక్క_క్షేత్రం++;
        చివర_యొక్క_క్షేత్రం = const_cast<అక్షరం*>(uprv_strchr(ప్రారంభం, '_'));
        ఒకవేళ (చివర_యొక్క_క్షేత్రం == శూన్యసూచిక) {
            len = uprv_strlen(ప్రారంభం);
        } లేకపోతే {
            len = చివర_యొక్క_క్షేత్రం - ప్రారంభం;
            *చివర_యొక్క_క్షేత్రం = '\0'; // null terminiate it.
        }
    }
    ఒకవేళ (len >= 2 && len <= 3) {
        // Got a region
        replacedRegion = ప్రారంభం;
        ఒకవేళ (చివర_యొక్క_క్షేత్రం == శూన్యసూచిక) {
            తిరిగి;
        }
        ప్రారంభం = చివర_యొక్క_క్షేత్రం++;
        చివర_యొక్క_క్షేత్రం = const_cast<అక్షరం*>(uprv_strchr(ప్రారంభం, '_'));
        ఒకవేళ (చివర_యొక్క_క్షేత్రం == శూన్యసూచిక) {
            len = uprv_strlen(ప్రారంభం);
        } లేకపోతే {
            len = చివర_యొక్క_క్షేత్రం - ప్రారంభం;
            *చివర_యొక్క_క్షేత్రం = '\0'; // null terminiate it.
        }
    }
    ఒకవేళ (len >= 4) {
        // Got a variant
        replacedVariant = ప్రారంభం;
        ఒకవేళ (చివర_యొక్క_క్షేత్రం == శూన్యసూచిక) {
            తిరిగి;
        }
        ప్రారంభం = చివర_యొక్క_క్షేత్రం++;
    }
    replacedExtensions = ప్రారంభం;
}

తార్కిక
AliasReplacer::replaceLanguage(
        తార్కిక checkLanguage, తార్కిక checkRegion,
        తార్కిక checkVariants, UVector& గమ్యం_Be_Freed, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (    (checkRegion && region == శూన్యసూచిక) ||
            (checkVariants && variants.size() == 0)) {
        // Nothing to search.
        తిరిగి అబద్ధం;
    }
    int32_t variant_కొలత = checkVariants ? variants.size() : 1;
    // Since we may have more than one variant, we need to loop through them.
    స్థిరరాశి అక్షరం* శోధన_Language = checkLanguage ? language : "und";
    స్థిరరాశి అక్షరం* శోధన_Region = checkRegion ? region : శూన్యసూచిక;
    స్థిరరాశి అక్షరం* శోధన_Variant = శూన్యసూచిక;
    కొరకు (int32_t variant_సూచీ = 0;
            variant_సూచీ < variant_కొలత;
            variant_సూచీ++) {
        ఒకవేళ (checkVariants) {
            U_ASSERT(variant_సూచీ < variant_కొలత);
            శోధన_Variant = static_cast<స్థిరరాశి అక్షరం*>(variants.elementAt(variant_సూచీ));
        }

        ఒకవేళ (శోధన_Variant != శూన్యసూచిక && uprv_strlen(శోధన_Variant) < 4) {
            // Do not consider  ill-formed variant subtag.
            శోధన_Variant = శూన్యసూచిక;
        }
        CharString type_కీలకం;
        generate_కీలకం(శోధన_Language, శోధన_Region, శోధన_Variant, type_కీలకం,
                    స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి అబద్ధం;
        }
        స్థిరరాశి అక్షరం *replacement = దత్తాంశం->languageMap().get(type_కీలకం.దత్తాంశం());
        ఒకవేళ (replacement == శూన్యసూచిక) {
            // Found no replacement data.
            కొనసాగు;
        }

        స్థిరరాశి అక్షరం* replacedLanguage = శూన్యసూచిక;
        స్థిరరాశి అక్షరం* replacedScript = శూన్యసూచిక;
        స్థిరరాశి అక్షరం* replacedRegion = శూన్యసూచిక;
        స్థిరరాశి అక్షరం* replacedVariant = శూన్యసూచిక;
        స్థిరరాశి అక్షరం* replacedExtensions = శూన్యసూచిక;
        parseLanguageReplacement(replacement,
                                 replacedLanguage,
                                 replacedScript,
                                 replacedRegion,
                                 replacedVariant,
                                 replacedExtensions,
                                 గమ్యం_Be_Freed,
                                 స్థితి_2);
        replacedLanguage =
            (replacedLanguage != శూన్యసూచిక && uprv_strcmp(replacedLanguage, "und") == 0) ?
            language : replacedLanguage;
        replacedScript = deleteOrReplace(script, శూన్యసూచిక, replacedScript);
        replacedRegion = deleteOrReplace(region, శోధన_Region, replacedRegion);
        replacedVariant = deleteOrReplace(
            శోధన_Variant, శోధన_Variant, replacedVariant);

        ఒకవేళ (    same(language, replacedLanguage) &&
                same(script, replacedScript) &&
                same(region, replacedRegion) &&
                same(శోధన_Variant, replacedVariant) &&
                replacedExtensions == శూన్యసూచిక) {
            // Replacement produce no changes.
            కొనసాగు;
        }

        language = replacedLanguage;
        region = replacedRegion;
        script = replacedScript;
        ఒకవేళ (శోధన_Variant != శూన్యసూచిక) {
            ఒకవేళ (not_ఖాళీలు(replacedVariant)) {
                variants.సమితి_Element_At((శూన్య*)replacedVariant, variant_సూచీ);
            } లేకపోతే {
                variants.removeElementAt(variant_సూచీ);
            }
        }
        ఒకవేళ (replacedExtensions != శూన్యసూచిక) {
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
        తిరిగి నిజం;
    }
    // Nothing changed by language alias data.
    తిరిగి అబద్ధం;
}

తార్కిక
AliasReplacer::replaceTerritory(UVector& గమ్యం_Be_Freed, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (region == శూన్యసూచిక) {
        // No region to search.
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి అక్షరం *replacement = దత్తాంశం->territoryMap().get(region);
    ఒకవేళ (replacement == శూన్యసూచిక) {
        // Found no replacement data for this region.
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి అక్షరం* replacedRegion = replacement;
    స్థిరరాశి అక్షరం* మొదటి_Space = uprv_strchr(replacement, ' ');
    ఒకవేళ (మొదటి_Space != శూన్యసూచిక) {
        // If there are are more than one region in the replacement.
        // We need to check which one match based on the language.
        // Cannot use nullptr for language because that will construct
        // the default locale, in that case, use "und" to get the correct
        // locale.
        Locale l = LocaleBuilder()
            .సమితి_Language(language == శూన్యసూచిక ? "und" : language)
            .సమితి_Script(script)
            .build(స్థితి_2);
        l.కూడు_Likely_Subtags(స్థితి_2);
        స్థిరరాశి అక్షరం* likelyRegion = l.getCountry();
        prv::unique_ptr<CharString> అంశం;
        ఒకవేళ (likelyRegion != శూన్యసూచిక && uprv_strlen(likelyRegion) > 0) {
            size_t len = uprv_strlen(likelyRegion);
            స్థిరరాశి అక్షరం* దొరికింది_లోపల_Replacement = uprv_strstr(replacement,
                                                         likelyRegion);
            ఒకవేళ (దొరికింది_లోపల_Replacement != శూన్యసూచిక) {
                // Assuming the case there are no three letter region code in
                // the replacement of territoryAlias
                U_ASSERT(దొరికింది_లోపల_Replacement == replacement ||
                         *(దొరికింది_లోపల_Replacement-1) == ' ');
                U_ASSERT(దొరికింది_లోపల_Replacement[len] == ' ' ||
                         దొరికింది_లోపల_Replacement[len] == '\0');
                అంశం = prv::make_unique<CharString>(
                    దొరికింది_లోపల_Replacement, static_cast<int32_t>(len), స్థితి_2, స్థితి_2);
            }
        }
        ఒకవేళ (!అంశం && U_SUCCESS(స్థితి_2)) {
            అంశం = prv::make_unique<CharString>(
                replacement, static_cast<int32_t>(మొదటి_Space - replacement), స్థితి_2, స్థితి_2);
        }
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి అబద్ధం; }
        replacedRegion = అంశం->దత్తాంశం();
        గమ్యం_Be_Freed.adoptElement(అంశం.release(), స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి అబద్ధం; }
    }
    U_ASSERT(!same(region, replacedRegion));
    region = replacedRegion;
    // The region is changed by data in territory alias.
    తిరిగి నిజం;
}

తార్కిక
AliasReplacer::replaceScript(UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (script == శూన్యసూచిక) {
        // No script to search.
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి అక్షరం *replacement = దత్తాంశం->scriptMap().get(script);
    ఒకవేళ (replacement == శూన్యసూచిక) {
        // Found no replacement data for this script.
        తిరిగి అబద్ధం;
    }
    U_ASSERT(!same(script, replacement));
    script = replacement;
    // The script is changed by data in script alias.
    తిరిగి నిజం;
}

తార్కిక
AliasReplacer::replaceVariant(UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    // Since we may have more than one variant, we need to loop through them.
    కొరకు (int32_t i = 0; i < variants.size(); i++) {
        స్థిరరాశి అక్షరం* variant = static_cast<స్థిరరాశి అక్షరం*>(variants.elementAt(i));
        స్థిరరాశి అక్షరం *replacement = దత్తాంశం->variantMap().get(variant);
        ఒకవేళ (replacement == శూన్యసూచిక) {
            // Found no replacement data for this variant.
            కొనసాగు;
        }
        U_ASSERT((uprv_strlen(replacement) >= 5  &&
                  uprv_strlen(replacement) <= 8) ||
                 (uprv_strlen(replacement) == 4 &&
                  replacement[0] >= '0' &&
                  replacement[0] <= '9'));
        ఒకవేళ (!same(variant, replacement)) {
            variants.సమితి_Element_At((శూన్య*)replacement, i);
            // Special hack to handle hepburn-heploc => alalc97
            ఒకవేళ (uprv_strcmp(variant, "heploc") == 0) {
                కొరకు (int32_t j = 0; j < variants.size(); j++) {
                     ఒకవేళ (uprv_strcmp((స్థిరరాశి అక్షరం*)(variants.elementAt(j)),
                                     "hepburn") == 0) {
                         variants.removeElementAt(j);
                     }
                }
            }
            తిరిగి నిజం;
        }
    }
    తిరిగి అబద్ధం;
}

తార్కిక
AliasReplacer::replaceSubdivision(
    StringPiece subdivision, CharString& output, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి అక్షరం *replacement = దత్తాంశం->subdivisionMap().get(subdivision.దత్తాంశం());
    ఒకవేళ (replacement != శూన్యసూచిక) {
        స్థిరరాశి అక్షరం* మొదటి_Space = uprv_strchr(replacement, ' ');
        // Found replacement data for this subdivision.
        size_t len = (మొదటి_Space != శూన్యసూచిక) ?
            (మొదటి_Space - replacement) : uprv_strlen(replacement);
        ఒకవేళ (2 <= len && len <= 8) {
            output.append(replacement, static_cast<int32_t>(len), స్థితి_2);
            ఒకవేళ (2 == len) {
                // Add 'zzzz' based on changes to UTS #35 for CLDR-14312.
                output.append("zzzz", 4, స్థితి_2);
            }
        }
        తిరిగి నిజం;
    }
    తిరిగి అబద్ధం;
}

తార్కిక
AliasReplacer::replaceTransformedExtensions(
    CharString& transformedExtensions, CharString& output, UError_సంకేతం& స్థితి_2)
{
    // The content of the transformedExtensions will be modified in this
    // function to NUL-terminating (tkey-tvalue) pairs.
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    int32_t len = transformedExtensions.length();
    స్థిరరాశి అక్షరం* str = transformedExtensions.దత్తాంశం();
    స్థిరరాశి అక్షరం* tkey = ultag_get_TKey_ప్రారంభం(str);
    int32_t tlangLen = (tkey == str) ? 0 :
        ((tkey == శూన్యసూచిక) ? len : static_cast<int32_t>((tkey - str - 1)));
    ఒకవేళ (tlangLen > 0) {
        Locale tlang = LocaleBuilder()
            .సమితి_Language_ట్యాగ్(StringPiece(str, tlangLen))
            .build(స్థితి_2);
        tlang.canonicalize(స్థితి_2);
        output = tlang.గమ్యం_Language_ట్యాగ్<CharString>(స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి అబద్ధం;
        }
        T_CString_గమ్యం_క్రింది_Case(output.దత్తాంశం());
    }
    ఒకవేళ (tkey != శూన్యసూచిక) {
        // We need to sort the tfields by tkey
        UVector tfields(స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి అబద్ధం;
        }
        చేయి {
            స్థిరరాశి అక్షరం* tvalue = uprv_strchr(tkey, '-');
            ఒకవేళ (tvalue == శూన్యసూచిక) {
                స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
                తిరిగి అబద్ధం;
            }
            స్థిరరాశి అక్షరం* తదుపరి_TKey = ultag_get_TKey_ప్రారంభం(tvalue);
            ఒకవేళ (తదుపరి_TKey != శూన్యసూచిక) {
                *const_cast<అక్షరం*>(తదుపరి_TKey - 1) = '\0'; // NUL terminate tvalue
            }
            tfields.చొప్పించు_Element_At((శూన్య*)tkey, tfields.size(), స్థితి_2);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తిరిగి అబద్ధం;
            }
            tkey = తదుపరి_TKey;
        } అయితే (tkey != శూన్యసూచిక);
        tfields.sort([](UElement e1, UElement e2) -> int32_t {
            తిరిగి uprv_strcmp((స్థిరరాశి అక్షరం*)e1.సూచిక, (స్థిరరాశి అక్షరం*)e2.సూచిక);
        }, స్థితి_2);
        కొరకు (int32_t i = 0; i < tfields.size(); i++) {
             ఒకవేళ (output.length() > 0) {
                 output.append('-', స్థితి_2);
             }
             స్థిరరాశి అక్షరం* tfield = static_cast<స్థిరరాశి అక్షరం*>(tfields.elementAt(i));
             స్థిరరాశి అక్షరం* tvalue = uprv_strchr(tfield, '-');
             ఒకవేళ (tvalue == శూన్యసూచిక) {
                 స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
                 తిరిగి అబద్ధం;
             }
             // Split the "tkey-tvalue" pair string so that we can canonicalize the tvalue.
             *const_cast<అక్షరం*>(tvalue++) = '\0'; // NUL terminate tkey
             output.append(tfield, స్థితి_2).append('-', స్థితి_2);
             std::optional<std::string_view> bcpTValue = ulocimp_గమ్యం_Bcp_Type(tfield, tvalue);
             output.append(bcpTValue.has_value() ? *bcpTValue : tvalue, స్థితి_2);
        }
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    తిరిగి నిజం;
}

CharString&
AliasReplacer::output_గమ్యం_String(
    CharString& out U_LIFETIME_BOUND, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి out; }
    out.append(language, స్థితి_2);
    ఒకవేళ (not_ఖాళీలు(script)) {
        out.append(SEP_CHAR, స్థితి_2)
            .append(script, స్థితి_2);
    }
    ఒకవేళ (not_ఖాళీలు(region)) {
        out.append(SEP_CHAR, స్థితి_2)
            .append(region, స్థితి_2);
    }
    ఒకవేళ (variants.size() > 0) {
        ఒకవేళ (!not_ఖాళీలు(script) && !not_ఖాళీలు(region)) {
          out.append(SEP_CHAR, స్థితి_2);
        }
        variants.sort([](UElement e1, UElement e2) -> int32_t {
            తిరిగి uprv_strcmp((స్థిరరాశి అక్షరం*)e1.సూచిక, (స్థిరరాశి అక్షరం*)e2.సూచిక);
        }, స్థితి_2);
        int32_t variants_ప్రారంభం = out.length();
        కొరకు (int32_t i = 0; i < variants.size(); i++) {
             out.append(SEP_CHAR, స్థితి_2)
                 .append(static_cast<స్థిరరాశి అక్షరం*>(variants.elementAt(i)),
                         స్థితి_2);
        }
        T_CString_గమ్యం_పై_Case(out.దత్తాంశం() + variants_ప్రారంభం);
    }
    ఒకవేళ (not_ఖాళీలు(extensions)) {
        CharString tmp("und_", స్థితి_2);
        tmp.append(extensions, స్థితి_2);
        Locale tmpLocale(tmp.దత్తాంశం());
        // only support x extension inside CLDR for now.
        U_ASSERT(extensions[0] == 'x');
        out.append(tmpLocale.getName() + 1, స్థితి_2);
    }
    తిరిగి out;
}

తార్కిక
AliasReplacer::replace(స్థిరరాశి Locale& locale, CharString& out, UError_సంకేతం& స్థితి_2)
{
    దత్తాంశం = Alias_దత్తాంశం::singleton(స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    U_ASSERT(దత్తాంశం != శూన్యసూచిక);
    out.clear();
    language = locale.getLanguage();
    ఒకవేళ (!not_ఖాళీలు(language)) {
        language = శూన్యసూచిక;
    }
    script = locale.getScript();
    ఒకవేళ (!not_ఖాళీలు(script)) {
        script = శూన్యసూచిక;
    }
    region = locale.getCountry();
    ఒకవేళ (!not_ఖాళీలు(region)) {
        region = శూన్యసూచిక;
    }
    స్థిరరాశి అక్షరం* variantsStr = locale.getVariant();
    CharString variantsBuff(variantsStr, -1, స్థితి_2);
    ఒకవేళ (!variantsBuff.is_ఖాళీలు()) {
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి అబద్ధం; }
        అక్షరం* ప్రారంభం = variantsBuff.దత్తాంశం();
        T_CString_గమ్యం_క్రింది_Case(ప్రారంభం);
        అక్షరం* చివర;
        అయితే ((చివర = uprv_strchr(ప్రారంభం, SEP_CHAR)) != శూన్యసూచిక &&
               U_SUCCESS(స్థితి_2)) {
            *చివర = NULL_CHAR;  // null terminate inside variantsBuff
            // do not add "" or duplicate data to variants
            ఒకవేళ (*ప్రారంభం && !variants.కలిగియుంది(ప్రారంభం)) {
                variants.కూడు_Element(ప్రారంభం, స్థితి_2);
            }
            ప్రారంభం = చివర + 1;
        }
        // do not add "" or duplicate data to variants
        ఒకవేళ (*ప్రారంభం && !variants.కలిగియుంది(ప్రారంభం)) {
            variants.కూడు_Element(ప్రారంభం, స్థితి_2);
        }
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి అబద్ధం; }

    // Sort the variants
    variants.sort([](UElement e1, UElement e2) -> int32_t {
        తిరిగి uprv_strcmp((స్థిరరాశి అక్షరం*)e1.సూచిక, (స్థిరరాశి అక్షరం*)e2.సూచిక);
    }, స్థితి_2);

    // A changed count to assert when loop too many times.
    int32_t మారింది = 0;
    స్థిరరాశి int32_t max_మారింది = 4 + variants.size();
    (శూన్య)max_మారింది; // avoid unused-variable warning when U_ASSERT is a no-op
    // A UVector to to hold CharString allocated by the replace* method
    // and freed when out of scope from his function.
    UVector అక్షరశ్రేణులు_గమ్యం_Be_Freed([](శూన్య *obj) { తొలగించు static_cast<CharString*>(obj); },
                             శూన్యసూచిక, 10, స్థితి_2);
    అయితే (U_SUCCESS(స్థితి_2)) {
        // Something wrong with the data cause looping here more than maxChanged
        // times already.
        U_ASSERT(మారింది <= max_మారింది);
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
        ఒకవేళ (    replaceLanguage(నిజం, నిజం,  నిజం,  అక్షరశ్రేణులు_గమ్యం_Be_Freed, స్థితి_2) ||
                replaceLanguage(నిజం, నిజం,  అబద్ధం, అక్షరశ్రేణులు_గమ్యం_Be_Freed, స్థితి_2) ||
                replaceLanguage(నిజం, అబద్ధం, నిజం,  అక్షరశ్రేణులు_గమ్యం_Be_Freed, స్థితి_2) ||
                replaceLanguage(నిజం, అబద్ధం, అబద్ధం, అక్షరశ్రేణులు_గమ్యం_Be_Freed, స్థితి_2) ||
                replaceLanguage(అబద్ధం,అబద్ధం, నిజం,  అక్షరశ్రేణులు_గమ్యం_Be_Freed, స్థితి_2) ||
                replaceTerritory(అక్షరశ్రేణులు_గమ్యం_Be_Freed, స్థితి_2) ||
                replaceScript(స్థితి_2) ||
                replaceVariant(స్థితి_2)) {
            // Some values in data is changed, try to match from the beginning
            // again.
            మారింది++;
            కొనసాగు;
        }
        // Nothing changed. Break out.
        ఆపు;
    }  // while(1)

    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి అబద్ధం; }
    // Nothing changed and we know the order of the variants are not change
    // because we have no variant or only one.
    స్థిరరాశి అక్షరం* extensionsStr = locale_get_Keywords_ప్రారంభం(locale.getName());
    ఒకవేళ (మారింది == 0 && variants.size() <= 1 && extensionsStr == శూన్యసూచిక) {
        తిరిగి అబద్ధం;
    }
    output_గమ్యం_String(out, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (extensionsStr != శూన్యసూచిక) {
        మారింది = 0;
        Locale తాత్కాలిక(locale);
        Local_సూచిక<icu::StringEnumeration> iter(locale.createKeywords(స్థితి_2));
        ఒకవేళ (U_SUCCESS(స్థితి_2) && !iter.isNull()) {
            స్థిరరాశి అక్షరం* కీలకం;
            అయితే ((కీలకం = iter->next(శూన్యసూచిక, స్థితి_2)) != శూన్యసూచిక) {
                ఒకవేళ (uprv_strcmp("sd", కీలకం) == 0 || uprv_strcmp("rg", కీలకం) == 0 ||
                        uprv_strcmp("t", కీలకం) == 0) {
                    స్వయంచాలక value = locale.get_Keyword_విలువ<CharString>(కీలకం, స్థితి_2);
                    ఒకవేళ (U_FAILURE(స్థితి_2)) {
                        స్థితి_2 = U_ZERO_ERROR;
                        కొనసాగు;
                    }
                    CharString replacement;
                    ఒకవేళ (uprv_strlen(కీలకం) == 2) {
                        ఒకవేళ (replaceSubdivision(value.గమ్యం_String_Piece(), replacement, స్థితి_2)) {
                            మారింది++;
                            తాత్కాలిక.సమితి_Keyword_విలువ(కీలకం, replacement.దత్తాంశం(), స్థితి_2);
                        }
                    } లేకపోతే {
                        U_ASSERT(uprv_strcmp(కీలకం, "t") == 0);
                        ఒకవేళ (replaceTransformedExtensions(value, replacement, స్థితి_2)) {
                            మారింది++;
                            తాత్కాలిక.సమితి_Keyword_విలువ(కీలకం, replacement.దత్తాంశం(), స్థితి_2);
                        }
                    }
                    ఒకవేళ (U_FAILURE(స్థితి_2)) {
                        తిరిగి అబద్ధం;
                    }
                }
            }
        }
        ఒకవేళ (మారింది != 0) {
            extensionsStr = locale_get_Keywords_ప్రారంభం(తాత్కాలిక.getName());
        }
        out.append(extensionsStr, స్థితి_2);
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    // If the tag is not changed, return.
    ఒకవేళ (uprv_strcmp(out.దత్తాంశం(), locale.getName()) == 0) {
        out.clear();
        తిరిగి అబద్ధం;
    }
    తిరిగి నిజం;
}

// Return true if the locale is changed during canonicalization.
// The replaced value then will be put into out.
తార్కిక
canonicalizeLocale(స్థిరరాశి Locale& locale, CharString& out, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి అబద్ధం; }
    AliasReplacer replacer(స్థితి_2);
    తిరిగి replacer.replace(locale, out, స్థితి_2);
}

// Function to optimize for known cases without so we can skip the loading
// of resources in the startup time until we really need it.
తార్కిక
is_తెలిసిన_Canonicalized_Locale(స్థిరరాశి అక్షరం* locale, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి అబద్ధం; }

    ఒకవేళ (    uprv_strcmp(locale, "c") == 0 ||
            uprv_strcmp(locale, "en") == 0 ||
            uprv_strcmp(locale, "en_US") == 0) {
        తిరిగి నిజం;
    }

    // common well-known Canonicalized.
    umtx_initOnce(g_తెలిసిన_Canonicalized_Init_Once,
                  &ఎక్కించు_తెలిసిన_Canonicalized, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    U_ASSERT(g_తెలిసిన_Canonicalized != శూన్యసూచిక);
    తిరిగి uhash_geti(g_తెలిసిన_Canonicalized, locale) != 0;
}

}  // namespace

U_NAMESPACE_చివర

// Function for testing.
U_EXPORT స్థిరరాశి అక్షరం* స్థిరరాశి*
ulocimp_get_తెలిసిన_Canonicalized_Locale_For_Test(int32_t& length)
{
    U_NAMESPACE_USE
    length = UPRV_LENGTHOF(తెలిసిన_CANONICALIZED);
    తిరిగి తెలిసిన_CANONICALIZED;
}

// Function for testing.
U_EXPORT తార్కిక
ulocimp_isCanonicalizedLocaleForTest(స్థిరరాశి అక్షరం* localeName)
{
    U_NAMESPACE_USE
    Locale l(localeName);
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    CharString తాత్కాలిక;
    తిరిగి !canonicalizeLocale(l, తాత్కాలిక, స్థితి_2) && U_SUCCESS(స్థితి_2);
}

U_NAMESPACE_BEGIN

Locale& Locale::init(స్థిరరాశి అక్షరం* localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    తిరిగి localeID == శూన్యసూచిక ? *ఇది = getDefault() : init(StringPiece{localeID}, canonicalize);
}

/*This function initializes a Locale from a C locale ID*/
Locale& Locale::init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND
{
    /* Free our current storage */
    Nest& nest = payload.emplace<Nest>();

    // not a loop:
    // just an easy way to have a common error-exit
    // without goto and without another function
    చేయి {
        అక్షరం *separator;
        అక్షరం *క్షేత్రం_2[5] = {శూన్యసూచిక};
        int32_t క్షేత్రం_Len[5] = {0};
        int32_t క్షేత్రం_Idx;
        int32_t variant_క్షేత్రం;
        int32_t length;
        UError_సంకేతం err;

        స్థిరరాశి స్వయంచాలక parse = [canonicalize](std::string_view localeID,
                                          అక్షరం* name,
                                          int32_t name_సామర్థ్యం,
                                          UError_సంకేతం& స్థితి_2) {
            తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
                name, name_సామర్థ్యం,
                [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
                    ఒకవేళ (canonicalize) {
                        ulocimp_canonicalize(localeID, sink, స్థితి_2);
                    } లేకపోతే {
                        ulocimp_getName(localeID, sink, స్థితి_2);
                    }
                },
                స్థితి_2);
        };

        // "canonicalize" the locale ID to ICU/Java format
        అక్షరం* నిండిన_Name = nest.పునాది_Name;
        err = U_ZERO_ERROR;
        length = parse(localeID, నిండిన_Name, పరిమాణం Nest::పునాది_Name, err);

        FixedString నిండిన_Name_బఫరు;
        ఒకవేళ (err == U_బఫరు_అధికప్రవాహం_ERROR || length >= static_cast<int32_t>(పరిమాణం Nest::పునాది_Name)) {
            /*Go to heap for the fullName if necessary*/
            ఒకవేళ (!నిండిన_Name_బఫరు.reserve(length + 1)) {
                ఆపు; // error: out of memory
            }
            నిండిన_Name = నిండిన_Name_బఫరు.getAlias();
            err = U_ZERO_ERROR;
            length = parse(localeID, నిండిన_Name, length + 1, err);
        }
        ఒకవేళ(U_FAILURE(err) || err == U_STRING_NOT_TERMINATED_WARNING) {
            /* should never occur */
            ఆపు;
        }

        std::string_view language;
        std::string_view script;
        std::string_view region;
        int32_t variantBegin = length;

        /* after uloc_getName/canonicalize() we know that only '_' are separators */
        /* But _ could also appeared in timezone such as "en@timezone=America/Los_Angeles" */
        separator = క్షేత్రం_2[0] = నిండిన_Name;
        క్షేత్రం_Idx = 1;
        అక్షరం* at = uprv_strchr(నిండిన_Name, '@');
        అయితే ((separator = uprv_strchr(క్షేత్రం_2[క్షేత్రం_Idx-1], SEP_CHAR)) != శూన్యసూచిక &&
               క్షేత్రం_Idx < UPRV_LENGTHOF(క్షేత్రం_2)-1 &&
               (at == శూన్యసూచిక || separator < at)) {
            క్షేత్రం_2[క్షేత్రం_Idx] = separator + 1;
            క్షేత్రం_Len[క్షేత్రం_Idx - 1] = static_cast<int32_t>(separator - క్షేత్రం_2[క్షేత్రం_Idx - 1]);
            క్షేత్రం_Idx++;
        }
        // variant may contain @foo or .foo POSIX cruft; remove it
        separator = uprv_strchr(క్షేత్రం_2[క్షేత్రం_Idx-1], '@');
        అక్షరం* sep2 = uprv_strchr(క్షేత్రం_2[క్షేత్రం_Idx-1], '.');
        ఒకవేళ (separator!=శూన్యసూచిక || sep2!=శూన్యసూచిక) {
            ఒకవేళ (separator==శూన్యసూచిక || (sep2!=శూన్యసూచిక && separator > sep2)) {
                separator = sep2;
            }
            క్షేత్రం_Len[క్షేత్రం_Idx - 1] = static_cast<int32_t>(separator - క్షేత్రం_2[క్షేత్రం_Idx - 1]);
        } లేకపోతే {
            క్షేత్రం_Len[క్షేత్రం_Idx - 1] = length - static_cast<int32_t>(క్షేత్రం_2[క్షేత్రం_Idx - 1] - నిండిన_Name);
        }
        తార్కిక hasKeywords = at != శూన్యసూచిక && uprv_strchr(at + 1, '=') != శూన్యసూచిక;

        ఒకవేళ (క్షేత్రం_Len[0] >= ULOC_LANG_సామర్థ్యం)
        {
            ఆపు; // error: the language field is too long
        }

        variant_క్షేత్రం = 1; /* Usually the 2nd one, except when a script or country is also used. */
        ఒకవేళ (క్షేత్రం_Len[0] > 0) {
            /* We have a language */
            language = {నిండిన_Name, static_cast<std::string_view::size_type>(క్షేత్రం_Len[0])};
        }
        ఒకవేళ (క్షేత్రం_Len[1] == 4 && uprv_isASCIILetter(క్షేత్రం_2[1][0]) &&
                uprv_isASCIILetter(క్షేత్రం_2[1][1]) && uprv_isASCIILetter(క్షేత్రం_2[1][2]) &&
                uprv_isASCIILetter(క్షేత్రం_2[1][3])) {
            /* We have at least a script */
            script = {క్షేత్రం_2[1], static_cast<std::string_view::size_type>(క్షేత్రం_Len[1])};
            variant_క్షేత్రం++;
        }

        ఒకవేళ (క్షేత్రం_Len[variant_క్షేత్రం] == 2 || క్షేత్రం_Len[variant_క్షేత్రం] == 3) {
            /* We have a country */
            region = {క్షేత్రం_2[variant_క్షేత్రం], static_cast<std::string_view::size_type>(క్షేత్రం_Len[variant_క్షేత్రం])};
            variant_క్షేత్రం++;
        } లేకపోతే ఒకవేళ (క్షేత్రం_Len[variant_క్షేత్రం] == 0) {
            variant_క్షేత్రం++; /* script or country empty but variant in next field (i.e. en__POSIX) */
        }

        ఒకవేళ (క్షేత్రం_Len[variant_క్షేత్రం] > 0) {
            /* We have a variant */
            variantBegin = static_cast<int32_t>(క్షేత్రం_2[variant_క్షేత్రం] - నిండిన_Name);
        } లేకపోతే ఒకవేళ (hasKeywords) {
            // The original computation of variantBegin leaves it equal to the length
            // of fullName if there is no variant.  It should instead be
            // the length of the baseName.
            variantBegin = static_cast<int32_t>(at - నిండిన_Name);
        }

        ఒకవేళ (!hasKeywords && Nest::సరిపోతుంది(length, language, script, region)) {
            U_ASSERT(నిండిన_Name == nest.పునాది_Name);
            U_ASSERT(నిండిన_Name_బఫరు.is_ఖాళీలు());
            nest.init(language, script, region, variantBegin);
        } లేకపోతే {
            ఒకవేళ (నిండిన_Name == nest.పునాది_Name) {
                U_ASSERT(నిండిన_Name_బఫరు.is_ఖాళీలు());
                నిండిన_Name_బఫరు = {నిండిన_Name, static_cast<std::string_view::size_type>(length)};
                ఒకవేళ (నిండిన_Name_బఫరు.is_ఖాళీలు()) {
                    ఆపు; // error: out of memory
                }
                ఒకవేళ (!language.empty()) {
                    language = {నిండిన_Name_బఫరు.దత్తాంశం(), language.size()};
                }
                ఒకవేళ (!script.empty()) {
                    script = {నిండిన_Name_బఫరు.దత్తాంశం() + (script.దత్తాంశం() - నిండిన_Name), script.size()};
                }
                ఒకవేళ (!region.empty()) {
                    region = {నిండిన_Name_బఫరు.దత్తాంశం() + (region.దత్తాంశం() - నిండిన_Name), region.size()};
                }
            }
            కుప్ప& కుప్ప_2 = payload.emplace<కుప్ప>(language, script, region, variantBegin);
            ఒకవేళ (isBogus()) {
                ఆపు; // error: out of memory
            }
            U_ASSERT(!నిండిన_Name_బఫరు.is_ఖాళీలు());
            కుప్ప_2.ptr->నిండిన_Name = std::move(నిండిన_Name_బఫరు);
            ఒకవేళ (hasKeywords) {
                ఒకవేళ (std::string_view::size_type పునాది_Name_నిడివి = at - నిండిన_Name; పునాది_Name_నిడివి > 0) {
                    కుప్ప_2.ptr->పునాది_Name = {కుప్ప_2.ptr->నిండిన_Name.దత్తాంశం(), పునాది_Name_నిడివి};
                    ఒకవేళ (కుప్ప_2.ptr->పునాది_Name.is_ఖాళీలు()) {
                        ఆపు; // error: out of memory
                    }
                }
            }
        }

        ఒకవేళ (canonicalize) {
            ఒకవేళ (!is_తెలిసిన_Canonicalized_Locale(getName(), err)) {
                CharString replaced;
                // Not sure it is already canonicalized
                ఒకవేళ (canonicalizeLocale(*ఇది, replaced, err)) {
                    U_ASSERT(U_SUCCESS(err));
                    // If need replacement, call init again.
                    init(replaced.దత్తాంశం(), అబద్ధం);
                }
                ఒకవేళ (U_FAILURE(err)) {
                    ఆపు;
                }
            }
        }   // if (canonicalize) {

        // successful end of init()
        తిరిగి *ఇది;
    } అయితే(0); /*loop doesn't iterate*/

    // when an error occurs, then set this object to "bogus" (there is no UErrorCode here)
    సమితి_గమ్యం_Bogus();

    తిరిగి *ఇది;
}

int32_t
Locale::hash_సంకేతం() స్థిరరాశి
{
    తిరిగి ustr_hashCharsN(getName(), static_cast<int32_t>(uprv_strlen(getName())));
}

శూన్య
Locale::సమితి_గమ్యం_Bogus() {
    /* Free our current storage */
    payload.సమితి_గమ్యం_Bogus();
}

స్థిరరాశి Locale& U_EXPORT2
Locale::getDefault()
{
    {
        Mutex తాళం(&gDefaultLocaleMutex);
        ఒకవేళ (gDefaultLocale != శూన్యసూచిక) {
            తిరిగి *gDefaultLocale;
        }
    }
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    తిరిగి *locale_సమితి_default_అంతర్గత(శూన్యసూచిక, స్థితి_2);
}



శూన్య U_EXPORT2
Locale::సమితి_Default( స్థిరరాశి   Locale&     newLocale,
                            UError_సంకేతం&  స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    /* Set the default from the full name string of the supplied locale.
     * This is a convenient way to access the default locale caching mechanisms.
     */
    స్థిరరాశి అక్షరం *localeID = newLocale.getName();
    locale_సమితి_default_అంతర్గత(localeID, స్థితి_2);
}

శూన్య
Locale::కూడు_Likely_Subtags(UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    CharString maximizedLocaleID = ulocimp_కూడు_Likely_Subtags(getName(), స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        ఒకవేళ (స్థితి_2 == U_జ్ఞాపకం_కేటాయింపు_ERROR) {
            సమితి_గమ్యం_Bogus();
        }
        తిరిగి;
    }

    init(maximizedLocaleID.దత్తాంశం(), /*canonicalize=*/అబద్ధం);
    ఒకవేళ (isBogus()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    }
}

శూన్య
Locale::minimizeSubtags(UError_సంకేతం& స్థితి_2) {
    Locale::minimizeSubtags(అబద్ధం, స్థితి_2);
}
శూన్య
Locale::minimizeSubtags(తార్కిక favorScript, UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    CharString minimizedLocaleID = ulocimp_minimizeSubtags(getName(), favorScript, స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        ఒకవేళ (స్థితి_2 == U_జ్ఞాపకం_కేటాయింపు_ERROR) {
            సమితి_గమ్యం_Bogus();
        }
        తిరిగి;
    }

    init(minimizedLocaleID.దత్తాంశం(), /*canonicalize=*/అబద్ధం);
    ఒకవేళ (isBogus()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    }
}

శూన్య
Locale::canonicalize(UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (isBogus()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    CharString uncanonicalized(getName(), స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        ఒకవేళ (స్థితి_2 == U_జ్ఞాపకం_కేటాయింపు_ERROR) {
            సమితి_గమ్యం_Bogus();
        }
        తిరిగి;
    }
    init(uncanonicalized.దత్తాంశం(), /*canonicalize=*/నిజం);
    ఒకవేళ (isBogus()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    }
}

Locale U_EXPORT2
Locale::for_Language_ట్యాగ్(StringPiece ట్యాగ్, UError_సంకేతం& స్థితి_2)
{
    Locale ఫలితం(Locale::eBOGUS);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి ఫలితం;
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

    int32_t parsed_నిడివి;
    CharString localeID = ulocimp_for_Language_ట్యాగ్(
            ట్యాగ్.దత్తాంశం(),
            ట్యాగ్.length(),
            &parsed_నిడివి,
            స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి ఫలితం;
    }

    ఒకవేళ (parsed_నిడివి != ట్యాగ్.size()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి ఫలితం;
    }

    ఫలితం.init(localeID.దత్తాంశం(), /*canonicalize=*/అబద్ధం);
    ఒకవేళ (ఫలితం.isBogus()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    }
    తిరిగి ఫలితం;
}

శూన్య
Locale::గమ్యం_Language_ట్యాగ్(ByteSink& sink, UError_సంకేతం& స్థితి_2) స్థిరరాశి
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    ఒకవేళ (isBogus()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    ulocimp_గమ్యం_Language_ట్యాగ్(getName(), sink, /*strict=*/అబద్ధం, స్థితి_2);
}

Locale U_EXPORT2
Locale::createFromName (స్థిరరాశి అక్షరం *name)
{
    ఒకవేళ (name) {
        Locale l("");
        l.init(name, అబద్ధం);
        తిరిగి l;
    }
    లేకపోతే {
        తిరిగి getDefault();
    }
}

Locale U_EXPORT2
Locale::createFromName(StringPiece name) {
    Locale loc("");
    loc.init(name, అబద్ధం);
    తిరిగి loc;
}

Locale U_EXPORT2
Locale::createCanonical(స్థిరరాశి అక్షరం* name) {
    Locale loc("");
    loc.init(name, నిజం);
    తిరిగి loc;
}

స్థిరరాశి అక్షరం *
Locale::getISO3Language() స్థిరరాశి
{
    తిరిగి uloc_getISO3Language(getName());
}


స్థిరరాశి అక్షరం *
Locale::getISO3Country() స్థిరరాశి
{
    తిరిగి uloc_getISO3Country(getName());
}

/**
 * Return the LCID value as specified in the "LocaleID" resource for this
 * locale.  The LocaleID must be expressed as a hexadecimal number, from
 * one to four digits.  If the LocaleID resource is not present, or is
 * in an incorrect format, 0 is returned.  The LocaleID is for use in
 * Windows (it is an LCID), but is available on all platforms.
 */
uint32_t
Locale::getLCID() స్థిరరాశి
{
    తిరిగి uloc_getLCID(getName());
}

స్థిరరాశి అక్షరం* స్థిరరాశి* U_EXPORT2 Locale::getISOCountries()
{
    తిరిగి uloc_getISOCountries();
}

స్థిరరాశి అక్షరం* స్థిరరాశి* U_EXPORT2 Locale::getISOLanguages()
{
    తిరిగి uloc_getISOLanguages();
}

// Set the locale's data based on a posix id.
శూన్య Locale::సమితి_From_POSIXID(స్థిరరాశి అక్షరం *posixID)
{
    init(posixID, నిజం);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::get_మూలం()
{
    తిరిగి getLocale(e_మూలం);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getEnglish()
{
    తిరిగి getLocale(eENGLISH);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getFrench()
{
    తిరిగి getLocale(eFRENCH);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getGerman()
{
    తిరిగి getLocale(eGERMAN);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getItalian()
{
    తిరిగి getLocale(eITALIAN);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getJapanese()
{
    తిరిగి getLocale(eJAPANESE);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getKorean()
{
    తిరిగి getLocale(eKOREAN);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getChinese()
{
    తిరిగి getLocale(eCHINESE);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getSimplifiedChinese()
{
    తిరిగి getLocale(eCHINA);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getTraditionalChinese()
{
    తిరిగి getLocale(eTAIWAN);
}


స్థిరరాశి Locale & U_EXPORT2
Locale::getFrance()
{
    తిరిగి getLocale(eFRANCE);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getGermany()
{
    తిరిగి getLocale(eGERMANY);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getItaly()
{
    తిరిగి getLocale(eITALY);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getJapan()
{
    తిరిగి getLocale(eJAPAN);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getKorea()
{
    తిరిగి getLocale(eKOREA);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getChina()
{
    తిరిగి getLocale(eCHINA);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getPRC()
{
    తిరిగి getLocale(eCHINA);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getTaiwan()
{
    తిరిగి getLocale(eTAIWAN);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getUK()
{
    తిరిగి getLocale(eUK);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getUS()
{
    తిరిగి getLocale(eUS);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getCanada()
{
    తిరిగి getLocale(eCANADA);
}

స్థిరరాశి Locale & U_EXPORT2
Locale::getCanadaFrench()
{
    తిరిగి getLocale(eCANADA_FRENCH);
}

స్థిరరాశి Locale &
Locale::getLocale(పూర్ణాంకం locid)
{
    Locale *localeCache = getLocaleCache();
    U_ASSERT((locid < eMAX_LOCALES)&&(locid>=0));
    ఒకవేళ (localeCache == శూన్యసూచిక) {
        // Failure allocating the locale cache.
        //   The best we can do is return a nullptr reference.
        locid = 0;
    }
    తిరిగి localeCache[locid]; /*operating on nullptr*/
}

/*
This function is defined this way in order to get around static
initialization and static destruction.
 */
Locale *
Locale::getLocaleCache()
{
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    umtx_initOnce(gLocaleCacheInitOnce, locale_init, స్థితి_2);
    తిరిగి gLocaleCache;
}

తరగతి KeywordEnumeration : బహిరంగ StringEnumeration {
రక్షిత:
    FixedString keywords;
అంతరంగ:
    int32_t length;
    స్థిరరాశి అక్షరం *current;
    స్థిర స్థిరరాశి అక్షరం fgClassID;

బహిరంగ:
    స్థిర UClassID U_EXPORT2 getStaticClassID() { తిరిగి (UClassID)&fgClassID; }
    మాయా UClassID getDynamicClassID() స్థిరరాశి override { తిరిగి getStaticClassID(); }
బహిరంగ:
    KeywordEnumeration(స్థిరరాశి అక్షరం *కీలకాలు, int32_t keywordLen, int32_t current_సూచీ, UError_సంకేతం &స్థితి_2)
        : keywords(), length(keywordLen), current(శూన్యసూచిక) {
        ఒకవేళ(U_SUCCESS(స్థితి_2) && keywordLen != 0) {
            ఒకవేళ(కీలకాలు == శూన్యసూచిక || keywordLen < 0) {
                స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            } లేకపోతే {
                keywords = {కీలకాలు, static_cast<std::string_view::size_type>(length)};
                ఒకవేళ (keywords.is_ఖాళీలు()) {
                    స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                } లేకపోతే {
                    current = keywords.దత్తాంశం() + current_సూచీ;
                }
            }
        }
    }

    మాయా ~KeywordEnumeration();

    మాయా StringEnumeration * clone() స్థిరరాశి override
    {
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        తిరిగి కొత్త KeywordEnumeration(
                keywords.దత్తాంశం(), length,
                static_cast<int32_t>(current - keywords.దత్తాంశం()), స్థితి_2);
    }

    మాయా int32_t count(UError_సంకేతం& స్థితి_2) స్థిరరాశి override {
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి 0; }
        స్థిరరాశి అక్షరం *kw = keywords.దత్తాంశం();
        int32_t ఫలితం = 0;
        అయితే(*kw) {
            ఫలితం++;
            kw += uprv_strlen(kw)+1;
        }
        తిరిగి ఫలితం;
    }

    మాయా స్థిరరాశి అక్షరం* next(int32_t* ఫలితం_నిడివి, UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND override {
        స్థిరరాశి అక్షరం* ఫలితం;
        int32_t len;
        ఒకవేళ(U_SUCCESS(స్థితి_2) && *current != 0) {
            ఫలితం = current;
            len = static_cast<int32_t>(uprv_strlen(current));
            current += len+1;
            ఒకవేళ(ఫలితం_నిడివి != శూన్యసూచిక) {
                *ఫలితం_నిడివి = len;
            }
        } లేకపోతే {
            ఒకవేళ(ఫలితం_నిడివి != శూన్యసూచిక) {
                *ఫలితం_నిడివి = 0;
            }
            ఫలితం = శూన్యసూచిక;
        }
        తిరిగి ఫలితం;
    }

    మాయా స్థిరరాశి UnicodeString* snext(UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND override {
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి శూన్యసూచిక; }
        int32_t ఫలితం_నిడివి = 0;
        స్థిరరాశి అక్షరం *s = next(&ఫలితం_నిడివి, స్థితి_2);
        తిరిగి సమితి_Chars(s, ఫలితం_నిడివి, స్థితి_2);
    }

    మాయా శూన్య reset(UError_సంకేతం& స్థితి_2) override {
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
        current = keywords.దత్తాంశం();
    }
};

స్థిరరాశి అక్షరం KeywordEnumeration::fgClassID = '\0';

// Out-of-line virtual destructor to serve as the "key function".
KeywordEnumeration::~KeywordEnumeration() = అప్రమేయ;

// A wrapper around KeywordEnumeration that calls uloc_toUnicodeLocaleKey() in
// the next() method for each keyword before returning it.
తరగతి UnicodeKeywordEnumeration : బహిరంగ KeywordEnumeration {
బహిరంగ:
    ఉపయోగించి KeywordEnumeration::KeywordEnumeration;
    మాయా ~UnicodeKeywordEnumeration();

    మాయా స్థిరరాశి అక్షరం* next(int32_t* ఫలితం_నిడివి, UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND override {
        స్థిరరాశి అక్షరం* legacy_కీలకం = KeywordEnumeration::next(శూన్యసూచిక, స్థితి_2);
        అయితే (U_SUCCESS(స్థితి_2) && legacy_కీలకం != శూన్యసూచిక) {
            స్థిరరాశి అక్షరం* కీలకం = uloc_గమ్యం_Unicode_Locale_కీలకం(legacy_కీలకం);
            ఒకవేళ (కీలకం != శూన్యసూచిక) {
                ఒకవేళ (ఫలితం_నిడివి != శూన్యసూచిక) {
                    *ఫలితం_నిడివి = static_cast<int32_t>(uprv_strlen(కీలకం));
                }
                తిరిగి కీలకం;
            }
            // Not a Unicode keyword, could be a t, x or other, continue to look at the next one.
            legacy_కీలకం = KeywordEnumeration::next(శూన్యసూచిక, స్థితి_2);
        }
        ఒకవేళ (ఫలితం_నిడివి != శూన్యసూచిక) *ఫలితం_నిడివి = 0;
        తిరిగి శూన్యసూచిక;
    }
    మాయా int32_t count(UError_సంకేతం& స్థితి_2) స్థిరరాశి override {
        ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి 0; }
        స్థిరరాశి అక్షరం *kw = keywords.దత్తాంశం();
        int32_t ఫలితం = 0;
        అయితే(*kw) {
            ఒకవేళ (uloc_గమ్యం_Unicode_Locale_కీలకం(kw) != శూన్యసూచిక) {
                ఫలితం++;
            }
            kw += uprv_strlen(kw)+1;
        }
        తిరిగి ఫలితం;
    }
};

// Out-of-line virtual destructor to serve as the "key function".
UnicodeKeywordEnumeration::~UnicodeKeywordEnumeration() = అప్రమేయ;

StringEnumeration *
Locale::createKeywords(UError_సంకేతం &స్థితి_2) స్థిరరాశి
{
    StringEnumeration *ఫలితం = శూన్యసూచిక;

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి ఫలితం;
    }

    స్థిరరాశి అక్షరం* variant_ప్రారంభం = uprv_strchr(getName(), '@');
    స్థిరరాశి అక్షరం* assignment = uprv_strchr(getName(), '=');
    ఒకవేళ(variant_ప్రారంభం) {
        ఒకవేళ(assignment > variant_ప్రారంభం) {
            CharString keywords = ulocimp_getKeywords(variant_ప్రారంభం + 1, '@', అబద్ధం, స్థితి_2);
            ఒకవేళ (U_SUCCESS(స్థితి_2) && !keywords.is_ఖాళీలు()) {
                ఫలితం = కొత్త KeywordEnumeration(keywords.దత్తాంశం(), keywords.length(), 0, స్థితి_2);
                ఒకవేళ (!ఫలితం) {
                    స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                }
            }
        } లేకపోతే {
            స్థితి_2 = U_INVALID_FORMAT_ERROR;
        }
    }
    తిరిగి ఫలితం;
}

StringEnumeration *
Locale::createUnicodeKeywords(UError_సంకేతం &స్థితి_2) స్థిరరాశి
{
    StringEnumeration *ఫలితం = శూన్యసూచిక;

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి ఫలితం;
    }

    స్థిరరాశి అక్షరం* variant_ప్రారంభం = uprv_strchr(getName(), '@');
    స్థిరరాశి అక్షరం* assignment = uprv_strchr(getName(), '=');
    ఒకవేళ(variant_ప్రారంభం) {
        ఒకవేళ(assignment > variant_ప్రారంభం) {
            CharString keywords = ulocimp_getKeywords(variant_ప్రారంభం + 1, '@', అబద్ధం, స్థితి_2);
            ఒకవేళ (U_SUCCESS(స్థితి_2) && !keywords.is_ఖాళీలు()) {
                ఫలితం = కొత్త UnicodeKeywordEnumeration(keywords.దత్తాంశం(), keywords.length(), 0, స్థితి_2);
                ఒకవేళ (!ఫలితం) {
                    స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                }
            }
        } లేకపోతే {
            స్థితి_2 = U_INVALID_FORMAT_ERROR;
        }
    }
    తిరిగి ఫలితం;
}

int32_t
Locale::get_Keyword_విలువ(స్థిరరాశి అక్షరం* keywordName, అక్షరం *బఫరు, int32_t bufLen, UError_సంకేతం &స్థితి_2) స్థిరరాశి
{
    తిరిగి uloc_get_Keyword_విలువ(getName(), keywordName, బఫరు, bufLen, &స్థితి_2);
}

శూన్య
Locale::get_Keyword_విలువ(StringPiece keywordName, ByteSink& sink, UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    ఒకవేళ (isBogus()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    ulocimp_get_Keyword_విలువ(getName(), keywordName, sink, స్థితి_2);
}

శూన్య
Locale::get_Unicode_Keyword_విలువ(StringPiece keywordName,
                               ByteSink& sink,
                               UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    std::optional<std::string_view> legacy_కీలకం = ulocimp_గమ్యం_Legacy_కీలకం_With_Fallback(keywordName);
    ఒకవేళ (!legacy_కీలకం.has_value()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    స్వయంచాలక legacy_విలువ = get_Keyword_విలువ<CharString>(*legacy_కీలకం, స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    std::optional<std::string_view> unicode_విలువ =
        ulocimp_గమ్యం_Bcp_Type_With_Fallback(keywordName, legacy_విలువ.గమ్యం_String_Piece());
    ఒకవేళ (!unicode_విలువ.has_value()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    sink.Append(unicode_విలువ->దత్తాంశం(), static_cast<int32_t>(unicode_విలువ->size()));
}

శూన్య
Locale::సమితి_Keyword_విలువ(StringPiece keywordName,
                        StringPiece keyword_విలువ,
                        UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
    ఒకవేళ (keywordName.empty()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ఒకవేళ (స్థితి_2 == U_STRING_NOT_TERMINATED_WARNING) {
        స్థితి_2 = U_ZERO_ERROR;
    }

    CharString localeID(getName(), -1, స్థితి_2);
    ulocimp_సమితి_Keyword_విలువ(keywordName, keyword_విలువ, localeID, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        ఒకవేళ (స్థితి_2 == U_జ్ఞాపకం_కేటాయింపు_ERROR) {
            సమితి_గమ్యం_Bogus();
        }
        తిరిగి;
    }

    స్థిరరాశి అక్షరం* at = locale_get_Keywords_ప్రారంభం(localeID.గమ్యం_String_Piece());
    తార్కిక hasKeywords = at != శూన్యసూచిక && uprv_strchr(at + 1, '=') != శూన్యసూచిక;

    Nest* nest = payload.get<Nest>();
    ఒకవేళ (!hasKeywords) {
        ఒకవేళ (nest == శూన్యసూచిక) {
            // There are no longer any keywords left, so it might now be
            // possible to move the payload from Heap to Nest.
            కుప్ప* కుప్ప_2 = payload.get<కుప్ప>();
            U_ASSERT(కుప్ప_2 != శూన్యసూచిక);
            ఒకవేళ (Nest::సరిపోతుంది(localeID.length(), కుప్ప_2->language, కుప్ప_2->script, కుప్ప_2->region)) {
                int32_t variantBegin = కుప్ప_2->ptr->variantBegin;
                U_ASSERT(variantBegin >= 0);
                U_ASSERT(static_cast<size_t>(variantBegin) < పరిమాణం Nest::పునాది_Name);
                nest = &payload.emplace<Nest>(std::move(*కుప్ప_2), static_cast<uint8_t>(variantBegin));
                localeID.extract(nest->పునాది_Name, పరిమాణం Nest::పునాది_Name, స్థితి_2);
            } లేకపోతే {
                కుప్ప_2->ptr->పునాది_Name.clear();
                కుప్ప_2->ptr->నిండిన_Name = localeID.గమ్యం_String_Piece();
                ఒకవేళ (కుప్ప_2->ptr->నిండిన_Name.is_ఖాళీలు()) {
                    స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    సమితి_గమ్యం_Bogus();
                    తిరిగి;
                }
            }
        }
    } లేకపోతే {
        కుప్ప* కుప్ప_2 = శూన్యసూచిక;
        ఒకవేళ (nest != శూన్యసూచిక) {
            // A keyword has been added, so the payload now needs to be moved
            // from Nest to Heap so that it can get a baseName.
            Nest copy(*nest);
            కుప్ప_2 = &payload.emplace<కుప్ప>(copy.language,
                                          copy.script,
                                          copy.region,
                                          copy.variantBegin);
            ఒకవేళ (isBogus()) {
                స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
        } లేకపోతే {
            కుప్ప_2 = payload.get<కుప్ప>();
        }
        U_ASSERT(కుప్ప_2 != శూన్యసూచిక);
        కుప్ప_2->ptr->నిండిన_Name = localeID.గమ్యం_String_Piece();
        ఒకవేళ (కుప్ప_2->ptr->నిండిన_Name.is_ఖాళీలు()) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            సమితి_గమ్యం_Bogus();
            తిరిగి;
        }

        ఒకవేళ (కుప్ప_2->ptr->పునాది_Name.is_ఖాళీలు()) {
            // Has added the first keyword, meaning that the fullName is no longer also the baseName.
            ఒకవేళ (std::string_view::size_type పునాది_Name_నిడివి = at - localeID.దత్తాంశం(); పునాది_Name_నిడివి > 0) {
                కుప్ప_2->ptr->పునాది_Name = {కుప్ప_2->ptr->నిండిన_Name.దత్తాంశం(), పునాది_Name_నిడివి};
                ఒకవేళ (కుప్ప_2->ptr->పునాది_Name.is_ఖాళీలు()) {
                    స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    సమితి_గమ్యం_Bogus();
                    తిరిగి;
                }
            }
        }
    }
}

శూన్య
Locale::సమితి_Unicode_Keyword_విలువ(StringPiece keywordName,
                               StringPiece keyword_విలువ,
                               UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    std::optional<std::string_view> legacy_కీలకం = ulocimp_గమ్యం_Legacy_కీలకం_With_Fallback(keywordName);
    ఒకవేళ (!legacy_కీలకం.has_value()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    std::string_view value;

    ఒకవేళ (!keyword_విలువ.empty()) {
        std::optional<std::string_view> legacy_విలువ =
            ulocimp_గమ్యం_Legacy_Type_With_Fallback(keywordName, keyword_విలువ);
        ఒకవేళ (!legacy_విలువ.has_value()) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }
        value = *legacy_విలువ;
    }

    సమితి_Keyword_విలువ(*legacy_కీలకం, value, స్థితి_2);
}

స్థిరరాశి అక్షరం*
Locale::getCountry() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి get_క్షేత్రం<&Nest::getRegion, &కుప్ప::getRegion>();
}

స్థిరరాశి అక్షరం*
Locale::getLanguage() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి get_క్షేత్రం<&Nest::getLanguage, &కుప్ప::getLanguage>();
}

స్థిరరాశి అక్షరం*
Locale::getScript() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి get_క్షేత్రం<&Nest::getScript, &కుప్ప::getScript>();
}

స్థిరరాశి అక్షరం*
Locale::getVariant() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి get_క్షేత్రం<&Nest::getVariant, &కుప్ప::getVariant>();
}

స్థిరరాశి అక్షరం*
Locale::getName() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి get_క్షేత్రం<&Nest::get_పునాది_Name, &కుప్ప::get_నిండిన_Name>();
}

స్థిరరాశి అక్షరం*
Locale::get_పునాది_Name() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి get_క్షేత్రం<&Nest::get_పునాది_Name, &కుప్ప::get_పునాది_Name>();
}

మూస <స్థిరరాశి అక్షరం* (Locale::Nest::*స్థిరరాశి NEST)() స్థిరరాశి,
          స్థిరరాశి అక్షరం* (Locale::కుప్ప::*స్థిరరాశి కుప్ప_3)() స్థిరరాశి>
స్థిరరాశి అక్షరం* Locale::get_క్షేత్రం() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి payload.సందర్శించు([] { తిరిగి ""; },
                         [](స్థిరరాశి Nest& nest) { తిరిగి (nest.*NEST)(); },
                         [](స్థిరరాశి కుప్ప& కుప్ప_2) { తిరిగి (కుప్ప_2.*కుప్ప_3)(); });
}

Locale::Iterator::~Iterator() = అప్రమేయ;

//eof
U_NAMESPACE_చివర
