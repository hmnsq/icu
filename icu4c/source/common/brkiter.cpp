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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "unicode/rbbi.h"
#અન્તર્ભાવ "unicode/brkiter.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/uloc.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/filteredbrk.h"
#અન્તર્ભાવ "bytesinkutil.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "servloc.h"
#અન્તર્ભાવ "locbased.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ubrkimpl.h"
#અન્તર્ભાવ "utracimp.h"
#અન્તર્ભાવ "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(અચલ Locale& loc, અચલ અક્ષર *type, UErrorCode &status)
{
    અક્ષર fnbuff[256];
    અક્ષર ext[4]={'\0'};
    CharString actual;
    int32_t size;
    અચલ char16_t* brkfname = nullptr;
    UResourceBundle brk_Rules_ઢગલો;
    UResourceBundle brk_Name_ઢગલો;
    UResourceBundle *brkRules = &brk_Rules_ઢગલો;
    UResourceBundle *brkName  = &brk_Name_ઢગલો;
    RuleBasedBreakIterator *result = nullptr;

    જો (U_FAILURE(status))
        પાછા nullptr;

    ures_init_ઢગલો_Object(brkRules);
    ures_init_ઢગલો_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    જો (U_SUCCESS(status)) {
        brkRules = ures_get_By_ચાવી_With_Fallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_get_By_ચાવી_With_Fallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((size_t)size<કદ(fnbuff));
        જો (static_cast<size_t>(size) >= કદ(fnbuff)) {
            size=0;
            જો (U_SUCCESS(status)) {
                status = U_BUFFER_OVERFLOW_ERROR;
            }
        }

        // Use the string if we found it
        જો (U_SUCCESS(status) && brkfname) {
            actual.append(ures_getLocaleInternal(brkName, &status), -1, status);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            પૂર્ણાંક len = 0;
            જો (extStart != nullptr){
                len = static_cast<પૂર્ણાંક>(extStart - brkfname);
                u_UCharsToChars(extStart+1, ext, કદ(ext)); // nul terminates the buff
                u_UCharsToChars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brkRules);
    ures_close(brkName);

    UDataMemory* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &status);
    જો (U_FAILURE(status)) {
        ures_close(b);
        પાછા nullptr;
    }

    // Create a RuleBasedBreakIterator
    result = નવું RuleBasedBreakIterator(file, uprv_strstr(type, "phrase") != nullptr, status);

    // If there is a result, set the valid locale and actual locale, and the kind
    જો (U_SUCCESS(status) && result != nullptr) {
        result->actualLocale = Locale(actual.data());
        result->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &status));
        result->requestLocale = loc;
    }

    ures_close(b);

    જો (U_FAILURE(status) && result != nullptr) {  // Sometimes redundant check, but simple
        delete result;
        પાછા nullptr;
    }

    જો (result == nullptr) {
        udata_close(file);
        જો (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    પાછા result;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(અચલ Locale& ચાવી, UErrorCode& status)
{
    પાછા createInstance(ચાવી, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_લીટી_Instance(અચલ Locale& ચાવી, UErrorCode& status)
{
    પાછા createInstance(ચાવી, UBRK_લીટી, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(અચલ Locale& ચાવી, UErrorCode& status)
{
    પાછા createInstance(ચાવી, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(અચલ Locale& ચાવી, UErrorCode& status)
{
    પાછા createInstance(ચાવી, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(અચલ Locale& ચાવી, UErrorCode& status)
{
    પાછા createInstance(ચાવી, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
અચલ Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    પાછા Locale::getAvailableLocales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_મૂળ()), validLocale(Locale::get_મૂળ()), requestLocale(Locale::get_મૂળ())
{
}

BreakIterator::BreakIterator(અચલ BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::operator=(અચલ BreakIterator& other) U_LIFETIME_BOUND {
    જો (આ != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    પાછા *આ;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#જો !UCONFIG_NO_SERVICE

// -------------------------------------

વર્ગ ICUBreakIteratorFactory : જાહેર ICUResourceBundleFactory {
જાહેર:
    આભાસી ~ICUBreakIteratorFactory();
સુરક્ષિત:
    આભાસી UObject* handleCreate(અચલ Locale& loc, int32_t kind, અચલ ICUService* /*service*/, UErrorCode& status) અચલ override {
        પાછા BreakIterator::makeInstance(loc, kind, status);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

વર્ગ ICUBreakIteratorService : જાહેર ICULocaleService {
જાહેર:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UErrorCode status = U_ZERO_ERROR;
        registerFactory(નવું ICUBreakIteratorFactory(), status);
    }

    આભાસી ~ICUBreakIteratorService();

    આભાસી UObject* cloneInstance(UObject* instance) અચલ override {
        પાછા ((BreakIterator*)instance)->clone();
    }

    આભાસી UObject* handleDefault(અચલ ICUService_ચાવી& ચાવી, UnicodeString* /*actualID*/, UErrorCode& status) અચલ override {
        Locale_ચાવી& lkey = static_cast<Locale_ચાવી&>(const_cast<ICUService_ચાવી&>(ચાવી));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        પાછા BreakIterator::makeInstance(loc, kind, status);
    }

    આભાસી UBool isDefault() અચલ override {
        પાછા ગણતરી_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_END

સ્થિર icu::UInitOnce gInitOnceBrkiter {};
સ્થિર icu::ICULocaleService* gService = nullptr;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
સ્થિર UBool U_CALLCONV breakiterator_cleanup() {
#જો !UCONFIG_NO_SERVICE
    જો (gService) {
        delete gService;
        gService = nullptr;
    }
    gInitOnceBrkiter.reset();
#endif
    પાછા સાચું;
}
U_CDECL_END
U_NAMESPACE_BEGIN

સ્થિર શૂન્ય U_CALLCONV 
initService() {
    gService = નવું ICUBreakIteratorService();
    ucln_common_registerCleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

સ્થિર ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    પાછા gService;
}


// -------------------------------------

સ્થિર અંતઃરેખ UBool
hasService()
{
    પાછા !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistry_ચાવી U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, અચલ Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    જો (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    પાછા service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_ચાવી ચાવી, UErrorCode& status)
{
    જો (U_SUCCESS(status)) {
        જો (hasService()) {
            પાછા gService->unregister(ચાવી, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    પાછા ખોટું;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    જો (service == nullptr) {
        પાછા nullptr;
    }
    પાછા service->getAvailableLocales();
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(અચલ Locale& loc, int32_t kind, UErrorCode& status)
{
    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }

#જો !UCONFIG_NO_SERVICE
    જો (hasService()) {
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
        જો (U_SUCCESS(status) && (result != nullptr) && *actualLoc.getName() != 0) {
            result->actualLocale = actualLoc;
            result->validLocale = actualLoc;
        }
        પાછા result;
    }
    નહીંતર
#endif
    {
        પાછા makeInstance(loc, kind, status);
    }
}

// -------------------------------------
ગણના { k_ચાવી_કિંમત_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(અચલ Locale& loc, int32_t kind, UErrorCode& status)
{

    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }

    BreakIterator *result = nullptr;
    પસંદગી (kind) {
    સ્થિતિ UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            result = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        તોડો;
    સ્થિતિ UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            result = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        તોડો;
    સ્થિતિ UBRK_લીટી:
        {
            અક્ષર lb_lw[k_ચાવી_કિંમત_Len_Max];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_લીટી);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            auto value = loc.get_Keyword_કિંમત<CharString>("lb", kvStatus);
            જો (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.data());
            }
            // lw=phrase is only supported in Japanese and Korean
            જો (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_કિંમત<CharString>("lw", kvStatus);
                જો (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.data());
                }
            }
            result = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        તોડો;
    સ્થિતિ UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            result = BreakIterator::buildInstance(loc, "sentence", status);
#જો !UCONFIG_NO_FILTERED_BREAK_ITERATION
            અક્ષર ss_ચાવી_કિંમત[k_ચાવી_કિંમત_Len_Max] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_કિંમત("ss", ss_ચાવી_કિંમત, k_ચાવી_કિંમત_Len_Max, kvStatus);
            જો (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ss_ચાવી_કિંમત,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                જો (U_SUCCESS(kvStatus)) {
                    result = fbiBuilder->build(result, status);
                    delete fbiBuilder;
                }
            }
#endif
            UTRACE_EXIT_STATUS(status);
        }
        તોડો;
    સ્થિતિ UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            result = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        તોડો;
    મૂળભૂત:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    જો (U_FAILURE(status)) {
        delete result;
        પાછા nullptr;
    }

    પાછા result;
}

Locale
BreakIterator::getLocale(ULocDataLocaleType type, UErrorCode& status) અચલ {
    જો (U_FAILURE(status)) {
        પાછા Locale::get_મૂળ();
    }
    જો (type == ULOC_REQUESTED_LOCALE) {
        પાછા requestLocale;
    }
    પાછા LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

અચલ અક્ષર*
BreakIterator::getLocaleID(ULocDataLocaleType type, UErrorCode& status) અચલ U_LIFETIME_BOUND {
    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }
    જો (type == ULOC_REQUESTED_LOCALE) {
        પાછા requestLocale.getName();
    }
    પાછા LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() અચલ {
    પાછા 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    જો (U_FAILURE(status)) {
        પાછા 0;
    }
    જો (capacity < 1) {
        status = U_BUFFER_OVERFLOW_ERROR;
        પાછા 1;
    }
    *fillInVec = 0;
    પાછા 1;
}

BreakIterator::BreakIterator(અચલ Locale& valid, અચલ Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_મૂળ()) {
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
