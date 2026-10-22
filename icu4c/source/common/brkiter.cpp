// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 1997-2015, International Business Machines Corporation and
* others. All Rights Reserved.
*******************************************************************************
*
* File brkiter.cpp
*
* Modification History:
*
*   Date        Name        Description
*   02/18/97    aliu        Converted from OpenClass.  Added DONE.
*   01/13/2000  helena      Added UErrorCode parameter to createXXXInstance methods.
*****************************************************************************************
*/

// *****************************************************************************
// This file was generated from the java source file BreakIterator.java
// *****************************************************************************

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/rbbi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/filteredbrk.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "servloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locbased.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubrkimpl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utracimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(ਅਚਲ Locale& loc, ਅਚਲ ਅੱਖਰ *type, UError_ਕੋਡ &ਸਥਿਤੀ)
{
    ਅੱਖਰ fnbuff[256];
    ਅੱਖਰ ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ਅਚਲ char16_t* brkfname = ਖਾਲੀ_ਸੂਚਕ;
    UResourceBundle brk_ਨਿਯਮਾਂ_ਢੇਰ;
    UResourceBundle brk_Name_ਢੇਰ;
    UResourceBundle *brk_ਨਿਯਮਾਂ = &brk_ਨਿਯਮਾਂ_ਢੇਰ;
    UResourceBundle *brkName  = &brk_Name_ਢੇਰ;
    ਨਿਯਮ_Based_Break_Iterator *ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ (U_FAILURE(ਸਥਿਤੀ))
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

    ures_init_ਢੇਰ_Object(brk_ਨਿਯਮਾਂ);
    ures_init_ਢੇਰ_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &ਸਥਿਤੀ);

    // Get the "boundaries" array.
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        brk_ਨਿਯਮਾਂ = ures_get_By_ਕੁੰਜੀ_With_Fallback(b, "boundaries", brk_ਨਿਯਮਾਂ, &ਸਥਿਤੀ);
        // Get the string object naming the rules file
        brkName = ures_get_By_ਕੁੰਜੀ_With_Fallback(brk_ਨਿਯਮਾਂ, type, brkName, &ਸਥਿਤੀ);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &ਸਥਿਤੀ);
        U_ASSERT((size_t)size<ਆਕਾਰ(fnbuff));
        ਜੇ (static_cast<size_t>(size) >= ਆਕਾਰ(fnbuff)) {
            size=0;
            ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
                ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            }
        }

        // Use the string if we found it
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && brkfname) {
            actual.append(ures_get_Locale_ਅੰਦਰੂਨੀ(brkName, &ਸਥਿਤੀ), -1, ਸਥਿਤੀ);

            char16_t* ext_ਸ਼ੁਰੂ=u_strchr(brkfname, 0x002e);
            ਪੂਰਨ len = 0;
            ਜੇ (ext_ਸ਼ੁਰੂ != ਖਾਲੀ_ਸੂਚਕ){
                len = static_cast<ਪੂਰਨ>(ext_ਸ਼ੁਰੂ - brkfname);
                u_UChars_ਮੰਜ਼ਿਲ_Chars(ext_ਸ਼ੁਰੂ+1, ext, ਆਕਾਰ(ext)); // nul terminates the buff
                u_UChars_ਮੰਜ਼ਿਲ_Chars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brk_ਨਿਯਮਾਂ);
    ures_close(brkName);

    UData_ਸਮ੍ਰਿਤੀ* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ures_close(b);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // Create a RuleBasedBreakIterator
    ਨਤੀਜਾ = ਨਵਾਂ ਨਿਯਮ_Based_Break_Iterator(file, uprv_strstr(type, "phrase") != ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);

    // If there is a result, set the valid locale and actual locale, and the kind
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && ਨਤੀਜਾ != ਖਾਲੀ_ਸੂਚਕ) {
        ਨਤੀਜਾ->actualLocale = Locale(actual.ਅੰਕੜੇ());
        ਨਤੀਜਾ->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &ਸਥਿਤੀ));
        ਨਤੀਜਾ->requestLocale = loc;
    }

    ures_close(b);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ) && ਨਤੀਜਾ != ਖਾਲੀ_ਸੂਚਕ) {  // Sometimes redundant check, but simple
        ਮਿਟਾਓ ਨਤੀਜਾ;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        udata_close(file);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_ਸ਼ਬਦ_Instance(ਅਚਲ Locale& ਕੁੰਜੀ, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ createInstance(ਕੁੰਜੀ, UBRK_ਸ਼ਬਦ, ਸਥਿਤੀ);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_ਸਤਰ_Instance(ਅਚਲ Locale& ਕੁੰਜੀ, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ createInstance(ਕੁੰਜੀ, UBRK_ਸਤਰ, ਸਥਿਤੀ);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ਅਚਲ Locale& ਕੁੰਜੀ, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ createInstance(ਕੁੰਜੀ, UBRK_CHARACTER, ਸਥਿਤੀ);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ਅਚਲ Locale& ਕੁੰਜੀ, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ createInstance(ਕੁੰਜੀ, UBRK_SENTENCE, ਸਥਿਤੀ);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ਅਚਲ Locale& ਕੁੰਜੀ, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ createInstance(ਕੁੰਜੀ, UBRK_TITLE, ਸਥਿਤੀ);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ਅਚਲ Locale* U_EXPORT2
BreakIterator::get_ਉਪਲਬਧ_Locales(int32_t& count)
{
    ਵਾਪਸ Locale::get_ਉਪਲਬਧ_Locales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_ਜੜ੍ਹ()), validLocale(Locale::get_ਜੜ੍ਹ()), requestLocale(Locale::get_ਜੜ੍ਹ())
{
}

BreakIterator::BreakIterator(ਅਚਲ BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::ਸੰਚਾਲਕ=(ਅਚਲ BreakIterator& other) U_LIFETIME_BOUND {
    ਜੇ (ਇਹ != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    ਵਾਪਸ *ਇਹ;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#ਜੇ !UCONFIG_NO_SERVICE

// -------------------------------------

ਵਰਗ ICUBreakIteratorFactory : ਜਨਤਕ ICUResourceBundleFactory {
ਜਨਤਕ:
    ਆਭਾਸੀ ~ICUBreakIteratorFactory();
ਸੁਰੱਖਿਅਤ:
    ਆਭਾਸੀ UObject* handleCreate(ਅਚਲ Locale& loc, int32_t ਕਿਸਮ, ਅਚਲ ICUService* /*service*/, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override {
        ਵਾਪਸ BreakIterator::makeInstance(loc, ਕਿਸਮ, ਸਥਿਤੀ);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

ਵਰਗ ICUBreakIteratorService : ਜਨਤਕ ICULocaleService {
ਜਨਤਕ:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        ਰਜਿਸਟਰ_Factory(ਨਵਾਂ ICUBreakIteratorFactory(), ਸਥਿਤੀ);
    }

    ਆਭਾਸੀ ~ICUBreakIteratorService();

    ਆਭਾਸੀ UObject* cloneInstance(UObject* instance) ਅਚਲ override {
        ਵਾਪਸ ((BreakIterator*)instance)->clone();
    }

    ਆਭਾਸੀ UObject* handleDefault(ਅਚਲ ICUService_ਕੁੰਜੀ& ਕੁੰਜੀ, UnicodeString* /*actualID*/, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override {
        Locale_ਕੁੰਜੀ& lkey = static_cast<Locale_ਕੁੰਜੀ&>(const_cast<ICUService_ਕੁੰਜੀ&>(ਕੁੰਜੀ));
        int32_t ਕਿਸਮ = lkey.ਕਿਸਮ();
        Locale loc;
        lkey.currentLocale(loc);
        ਵਾਪਸ BreakIterator::makeInstance(loc, ਕਿਸਮ, ਸਥਿਤੀ);
    }

    ਆਭਾਸੀ UBool isDefault() ਅਚਲ override {
        ਵਾਪਸ ਗਿਣਤੀ_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_ਸਿਰਾ

ਸਥਿਰ icu::UInitOnce gInitOnceBrkiter {};
ਸਥਿਰ icu::ICULocaleService* gService = ਖਾਲੀ_ਸੂਚਕ;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
ਸਥਿਰ UBool U_CALLCONV breakiterator_cleanup() {
#ਜੇ !UCONFIG_NO_SERVICE
    ਜੇ (gService) {
        ਮਿਟਾਓ gService;
        gService = ਖਾਲੀ_ਸੂਚਕ;
    }
    gInitOnceBrkiter.reset();
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਸੱਚ;
}
U_CDECL_ਸਿਰਾ
U_NAMESPACE_BEGIN

ਸਥਿਰ ਖਾਲੀ U_CALLCONV 
initService() {
    gService = ਨਵਾਂ ICUBreakIteratorService();
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

ਸਥਿਰ ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    ਵਾਪਸ gService;
}


// -------------------------------------

ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
hasService()
{
    ਵਾਪਸ !gInitOnceBrkiter.isReset() && getService() != ਖਾਲੀ_ਸੂਚਕ;
}

// -------------------------------------

URegistry_ਕੁੰਜੀ U_EXPORT2
BreakIterator::ਰਜਿਸਟਰ_Instance(BreakIterator* ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ Locale& locale, UBreakIteratorType ਕਿਸਮ, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ICULocaleService *service = getService();
    ਜੇ (service == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ service->ਰਜਿਸਟਰ_Instance(ਮੰਜ਼ਿਲ_Adopt, locale, ਕਿਸਮ, ਸਥਿਤੀ);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_ਕੁੰਜੀ ਕੁੰਜੀ, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਜੇ (hasService()) {
            ਵਾਪਸ gService->unregister(ਕੁੰਜੀ, ਸਥਿਤੀ);
        }
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਵਾਪਸ ਝੂਠ;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::get_ਉਪਲਬਧ_Locales()
{
    ICULocaleService *service = getService();
    ਜੇ (service == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ service->get_ਉਪਲਬਧ_Locales();
}
#ਜੇ_ਅੰਤ /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ਅਚਲ Locale& loc, int32_t ਕਿਸਮ, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

#ਜੇ !UCONFIG_NO_SERVICE
    ਜੇ (hasService()) {
        Locale actualLoc("");
        BreakIterator *ਨਤੀਜਾ = (BreakIterator*)gService->get(loc, ਕਿਸਮ, &actualLoc, ਸਥਿਤੀ);
        // TODO: The way the service code works in ICU 2.8 is that if
        // there is a real registered break iterator, the actualLoc
        // will be populated, but if the handleDefault path is taken
        // (because nothing is registered that can handle the
        // requested locale) then the actualLoc comes back empty.  In
        // that case, the returned object already has its actual/valid
        // locale data populated (by makeInstance, which is what
        // handleDefault calls), so we don't touch it.  YES, A COMMENT
        // THIS LONG is a sign of bad code -- so the action item is to
        // revisit this in ICU 3.0 and clean it up/fix it/remove it.
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && (ਨਤੀਜਾ != ਖਾਲੀ_ਸੂਚਕ) && *actualLoc.getName() != 0) {
            ਨਤੀਜਾ->actualLocale = actualLoc;
            ਨਤੀਜਾ->validLocale = actualLoc;
        }
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
        ਵਾਪਸ makeInstance(loc, ਕਿਸਮ, ਸਥਿਤੀ);
    }
}

// -------------------------------------
ਗਿਣਤੀ { k_ਕੁੰਜੀ_ਮੁੱਲ_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ਅਚਲ Locale& loc, int32_t ਕਿਸਮ, UError_ਕੋਡ& ਸਥਿਤੀ)
{

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    BreakIterator *ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    ਚੋਣ (ਕਿਸਮ) {
    ਹਾਲਤ UBRK_CHARACTER:
        {
            UTRACE_ਇੰਦਰਾਜ(UTRACE_UBRK_CREATE_CHARACTER);
            ਨਤੀਜਾ = BreakIterator::buildInstance(loc, "grapheme", ਸਥਿਤੀ);
            UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
        }
        ਰੋਕੋ;
    ਹਾਲਤ UBRK_ਸ਼ਬਦ:
        {
            UTRACE_ਇੰਦਰਾਜ(UTRACE_UBRK_CREATE_ਸ਼ਬਦ);
            ਨਤੀਜਾ = BreakIterator::buildInstance(loc, "word", ਸਥਿਤੀ);
            UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
        }
        ਰੋਕੋ;
    ਹਾਲਤ UBRK_ਸਤਰ:
        {
            ਅੱਖਰ lb_lw[k_ਕੁੰਜੀ_ਮੁੱਲ_Len_Max];
            UTRACE_ਇੰਦਰਾਜ(UTRACE_UBRK_CREATE_ਸਤਰ);
            uprv_strcpy(lb_lw, "line");
            UError_ਕੋਡ kv_ਸਥਿਤੀ = U_ZERO_ERROR;
            ਸਵੈਚਾਲਿਤ value = loc.get_Keyword_ਮੁੱਲ<CharString>("lb", kv_ਸਥਿਤੀ);
            ਜੇ (U_SUCCESS(kv_ਸਥਿਤੀ) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.ਅੰਕੜੇ());
            }
            // lw=phrase is only supported in Japanese and Korean
            ਜੇ (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_ਮੁੱਲ<CharString>("lw", kv_ਸਥਿਤੀ);
                ਜੇ (U_SUCCESS(kv_ਸਥਿਤੀ) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.ਅੰਕੜੇ());
                }
            }
            ਨਤੀਜਾ = BreakIterator::buildInstance(loc, lb_lw, ਸਥਿਤੀ);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
        }
        ਰੋਕੋ;
    ਹਾਲਤ UBRK_SENTENCE:
        {
            UTRACE_ਇੰਦਰਾਜ(UTRACE_UBRK_CREATE_SENTENCE);
            ਨਤੀਜਾ = BreakIterator::buildInstance(loc, "sentence", ਸਥਿਤੀ);
#ਜੇ !UCONFIG_NO_FILTERED_BREAK_ITERATION
            ਅੱਖਰ ss_ਕੁੰਜੀ_ਮੁੱਲ[k_ਕੁੰਜੀ_ਮੁੱਲ_Len_Max] = {0};
            UError_ਕੋਡ kv_ਸਥਿਤੀ = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_ਮੁੱਲ("ss", ss_ਕੁੰਜੀ_ਮੁੱਲ, k_ਕੁੰਜੀ_ਮੁੱਲ_Len_Max, kv_ਸਥਿਤੀ);
            ਜੇ (U_SUCCESS(kv_ਸਥਿਤੀ) && kLen > 0 && uprv_strcmp(ss_ਕੁੰਜੀ_ਮੁੱਲ,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kv_ਸਥਿਤੀ);
                ਜੇ (U_SUCCESS(kv_ਸਥਿਤੀ)) {
                    ਨਤੀਜਾ = fbiBuilder->build(ਨਤੀਜਾ, ਸਥਿਤੀ);
                    ਮਿਟਾਓ fbiBuilder;
                }
            }
#ਜੇ_ਅੰਤ
            UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
        }
        ਰੋਕੋ;
    ਹਾਲਤ UBRK_TITLE:
        {
            UTRACE_ਇੰਦਰਾਜ(UTRACE_UBRK_CREATE_TITLE);
            ਨਤੀਜਾ = BreakIterator::buildInstance(loc, "title", ਸਥਿਤੀ);
            UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
        }
        ਰੋਕੋ;
    ਮੂਲ:
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਮਿਟਾਓ ਨਤੀਜਾ;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

Locale
BreakIterator::getLocale(ULoc_ਅੰਕੜੇ_Locale_Type type, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ Locale::get_ਜੜ੍ਹ();
    }
    ਜੇ (type == ULOC_REQUESTED_LOCALE) {
        ਵਾਪਸ requestLocale;
    }
    ਵਾਪਸ LocaleBased::getLocale(validLocale, actualLocale, type, ਸਥਿਤੀ);
}

ਅਚਲ ਅੱਖਰ*
BreakIterator::getLocaleID(ULoc_ਅੰਕੜੇ_Locale_Type type, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (type == ULOC_REQUESTED_LOCALE) {
        ਵਾਪਸ requestLocale.getName();
    }
    ਵਾਪਸ LocaleBased::getLocaleID(validLocale, actualLocale, type, ਸਥਿਤੀ);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_ਨਿਯਮ_ਸਥਿਤੀ() ਅਚਲ {
    ਵਾਪਸ 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_ਨਿਯਮ_ਸਥਿਤੀ_Vec(int32_t *fill_ਵਿੱਚ_Vec, int32_t capacity, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ (capacity < 1) {
        ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ 1;
    }
    *fill_ਵਿੱਚ_Vec = 0;
    ਵਾਪਸ 1;
}

BreakIterator::BreakIterator(ਅਚਲ Locale& valid, ਅਚਲ Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_ਜੜ੍ਹ()) {
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
