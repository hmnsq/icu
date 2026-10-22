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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/rbbi.h"
#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/filteredbrk.h"
#അന്തര്ഭാവ "bytesinkutil.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "servloc.h"
#അന്തര്ഭാവ "locbased.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ubrkimpl.h"
#അന്തര്ഭാവ "utracimp.h"
#അന്തര്ഭാവ "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(സ്ഥിരം Locale& loc, സ്ഥിരം അക്ഷരം *type, UErrorCode &status)
{
    അക്ഷരം fnbuff[256];
    അക്ഷരം ext[4]={'\0'};
    CharString actual;
    int32_t size;
    സ്ഥിരം char16_t* brkfname = nullptr;
    UResourceBundle brk_Rules_അട്ടി;
    UResourceBundle brk_Name_അട്ടി;
    UResourceBundle *brkRules = &brk_Rules_അട്ടി;
    UResourceBundle *brkName  = &brk_Name_അട്ടി;
    RuleBasedBreakIterator *result = nullptr;

    എങ്കിൽ (U_FAILURE(status))
        തിരികെ nullptr;

    ures_init_അട്ടി_Object(brkRules);
    ures_init_അട്ടി_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    എങ്കിൽ (U_SUCCESS(status)) {
        brkRules = ures_get_By_താക്കോൽ_With_Fallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_get_By_താക്കോൽ_With_Fallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((size_t)size<വലുപ്പം(fnbuff));
        എങ്കിൽ (static_cast<size_t>(size) >= വലുപ്പം(fnbuff)) {
            size=0;
            എങ്കിൽ (U_SUCCESS(status)) {
                status = U_BUFFER_OVERFLOW_ERROR;
            }
        }

        // Use the string if we found it
        എങ്കിൽ (U_SUCCESS(status) && brkfname) {
            actual.append(ures_getLocaleInternal(brkName, &status), -1, status);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            പൂർണ്ണസംഖ്യ len = 0;
            എങ്കിൽ (extStart != nullptr){
                len = static_cast<പൂർണ്ണസംഖ്യ>(extStart - brkfname);
                u_UCharsToChars(extStart+1, ext, വലുപ്പം(ext)); // nul terminates the buff
                u_UCharsToChars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brkRules);
    ures_close(brkName);

    UDataMemory* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &status);
    എങ്കിൽ (U_FAILURE(status)) {
        ures_close(b);
        തിരികെ nullptr;
    }

    // Create a RuleBasedBreakIterator
    result = പുതിയ RuleBasedBreakIterator(file, uprv_strstr(type, "phrase") != nullptr, status);

    // If there is a result, set the valid locale and actual locale, and the kind
    എങ്കിൽ (U_SUCCESS(status) && result != nullptr) {
        result->actualLocale = Locale(actual.data());
        result->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &status));
        result->requestLocale = loc;
    }

    ures_close(b);

    എങ്കിൽ (U_FAILURE(status) && result != nullptr) {  // Sometimes redundant check, but simple
        delete result;
        തിരികെ nullptr;
    }

    എങ്കിൽ (result == nullptr) {
        udata_close(file);
        എങ്കിൽ (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    തിരികെ result;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(സ്ഥിരം Locale& താക്കോൽ, UErrorCode& status)
{
    തിരികെ createInstance(താക്കോൽ, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_വരി_Instance(സ്ഥിരം Locale& താക്കോൽ, UErrorCode& status)
{
    തിരികെ createInstance(താക്കോൽ, UBRK_വരി, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(സ്ഥിരം Locale& താക്കോൽ, UErrorCode& status)
{
    തിരികെ createInstance(താക്കോൽ, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(സ്ഥിരം Locale& താക്കോൽ, UErrorCode& status)
{
    തിരികെ createInstance(താക്കോൽ, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(സ്ഥിരം Locale& താക്കോൽ, UErrorCode& status)
{
    തിരികെ createInstance(താക്കോൽ, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
സ്ഥിരം Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    തിരികെ Locale::getAvailableLocales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_വേര്()), validLocale(Locale::get_വേര്()), requestLocale(Locale::get_വേര്())
{
}

BreakIterator::BreakIterator(സ്ഥിരം BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::operator=(സ്ഥിരം BreakIterator& other) U_LIFETIME_BOUND {
    എങ്കിൽ (ഇത് != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    തിരികെ *ഇത്;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#എങ്കിൽ !UCONFIG_NO_SERVICE

// -------------------------------------

വർഗ്ഗം ICUBreakIteratorFactory : പൊതു ICUResourceBundleFactory {
പൊതു:
    മായാ ~ICUBreakIteratorFactory();
സംരക്ഷിത:
    മായാ UObject* handleCreate(സ്ഥിരം Locale& loc, int32_t kind, സ്ഥിരം ICUService* /*service*/, UErrorCode& status) സ്ഥിരം override {
        തിരികെ BreakIterator::makeInstance(loc, kind, status);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

വർഗ്ഗം ICUBreakIteratorService : പൊതു ICULocaleService {
പൊതു:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UErrorCode status = U_ZERO_ERROR;
        registerFactory(പുതിയ ICUBreakIteratorFactory(), status);
    }

    മായാ ~ICUBreakIteratorService();

    മായാ UObject* cloneInstance(UObject* instance) സ്ഥിരം override {
        തിരികെ ((BreakIterator*)instance)->clone();
    }

    മായാ UObject* handleDefault(സ്ഥിരം ICUService_താക്കോൽ& താക്കോൽ, UnicodeString* /*actualID*/, UErrorCode& status) സ്ഥിരം override {
        Locale_താക്കോൽ& lkey = static_cast<Locale_താക്കോൽ&>(const_cast<ICUService_താക്കോൽ&>(താക്കോൽ));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        തിരികെ BreakIterator::makeInstance(loc, kind, status);
    }

    മായാ UBool isDefault() സ്ഥിരം override {
        തിരികെ എണ്ണം_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_END

സ്ഥിതം icu::UInitOnce gInitOnceBrkiter {};
സ്ഥിതം icu::ICULocaleService* gService = nullptr;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
സ്ഥിതം UBool U_CALLCONV breakiterator_cleanup() {
#എങ്കിൽ !UCONFIG_NO_SERVICE
    എങ്കിൽ (gService) {
        delete gService;
        gService = nullptr;
    }
    gInitOnceBrkiter.reset();
#endif
    തിരികെ സത്യം;
}
U_CDECL_END
U_NAMESPACE_BEGIN

സ്ഥിതം ശൂന്യം U_CALLCONV 
initService() {
    gService = പുതിയ ICUBreakIteratorService();
    ucln_common_registerCleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

സ്ഥിതം ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    തിരികെ gService;
}


// -------------------------------------

സ്ഥിതം വരിയിൽ UBool
hasService()
{
    തിരികെ !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistry_താക്കോൽ U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, സ്ഥിരം Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    എങ്കിൽ (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    തിരികെ service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_താക്കോൽ താക്കോൽ, UErrorCode& status)
{
    എങ്കിൽ (U_SUCCESS(status)) {
        എങ്കിൽ (hasService()) {
            തിരികെ gService->unregister(താക്കോൽ, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    തിരികെ അസത്യം;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    എങ്കിൽ (service == nullptr) {
        തിരികെ nullptr;
    }
    തിരികെ service->getAvailableLocales();
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(സ്ഥിരം Locale& loc, int32_t kind, UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

#എങ്കിൽ !UCONFIG_NO_SERVICE
    എങ്കിൽ (hasService()) {
        Locale actualLoc("");
        BreakIterator *result = (BreakIterator*)gService->get(loc, kind, &actualLoc, status);
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
        എങ്കിൽ (U_SUCCESS(status) && (result != nullptr) && *actualLoc.getName() != 0) {
            result->actualLocale = actualLoc;
            result->validLocale = actualLoc;
        }
        തിരികെ result;
    }
    അല്ലെങ്കിൽ
#endif
    {
        തിരികെ makeInstance(loc, kind, status);
    }
}

// -------------------------------------
എണ്ണൽ { k_താക്കോൽ_മൂല്യം_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(സ്ഥിരം Locale& loc, int32_t kind, UErrorCode& status)
{

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    BreakIterator *result = nullptr;
    തിരഞ്ഞെടുപ്പ് (kind) {
    അവസ്ഥ UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            result = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        നിർത്തുക;
    അവസ്ഥ UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            result = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        നിർത്തുക;
    അവസ്ഥ UBRK_വരി:
        {
            അക്ഷരം lb_lw[k_താക്കോൽ_മൂല്യം_Len_Max];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_വരി);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            auto value = loc.get_Keyword_മൂല്യം<CharString>("lb", kvStatus);
            എങ്കിൽ (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.data());
            }
            // lw=phrase is only supported in Japanese and Korean
            എങ്കിൽ (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_മൂല്യം<CharString>("lw", kvStatus);
                എങ്കിൽ (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.data());
                }
            }
            result = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        നിർത്തുക;
    അവസ്ഥ UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            result = BreakIterator::buildInstance(loc, "sentence", status);
#എങ്കിൽ !UCONFIG_NO_FILTERED_BREAK_ITERATION
            അക്ഷരം ss_താക്കോൽ_മൂല്യം[k_താക്കോൽ_മൂല്യം_Len_Max] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_മൂല്യം("ss", ss_താക്കോൽ_മൂല്യം, k_താക്കോൽ_മൂല്യം_Len_Max, kvStatus);
            എങ്കിൽ (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ss_താക്കോൽ_മൂല്യം,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                എങ്കിൽ (U_SUCCESS(kvStatus)) {
                    result = fbiBuilder->build(result, status);
                    delete fbiBuilder;
                }
            }
#endif
            UTRACE_EXIT_STATUS(status);
        }
        നിർത്തുക;
    അവസ്ഥ UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            result = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    എങ്കിൽ (U_FAILURE(status)) {
        delete result;
        തിരികെ nullptr;
    }

    തിരികെ result;
}

Locale
BreakIterator::getLocale(ULocDataLocaleType type, UErrorCode& status) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ Locale::get_വേര്();
    }
    എങ്കിൽ (type == ULOC_REQUESTED_LOCALE) {
        തിരികെ requestLocale;
    }
    തിരികെ LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

സ്ഥിരം അക്ഷരം*
BreakIterator::getLocaleID(ULocDataLocaleType type, UErrorCode& status) സ്ഥിരം U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (type == ULOC_REQUESTED_LOCALE) {
        തിരികെ requestLocale.getName();
    }
    തിരികെ LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() സ്ഥിരം {
    തിരികെ 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ 0;
    }
    എങ്കിൽ (capacity < 1) {
        status = U_BUFFER_OVERFLOW_ERROR;
        തിരികെ 1;
    }
    *fillInVec = 0;
    തിരികെ 1;
}

BreakIterator::BreakIterator(സ്ഥിരം Locale& valid, സ്ഥിരം Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_വേര്()) {
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
