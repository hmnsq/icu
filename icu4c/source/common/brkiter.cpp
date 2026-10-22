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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/rbbi.h"
#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/filteredbrk.h"
#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "servloc.h"
#ଅନ୍ତର୍ଭାଵ "locbased.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ubrkimpl.h"
#ଅନ୍ତର୍ଭାଵ "utracimp.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(ଅଚଳ Locale& loc, ଅଚଳ ଅକ୍ଷର *type, UErrorCode &status)
{
    ଅକ୍ଷର fnbuff[256];
    ଅକ୍ଷର ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ଅଚଳ char16_t* brkfname = nullptr;
    UResourceBundle brk_Rules_ଗଦା;
    UResourceBundle brk_Name_ଗଦା;
    UResourceBundle *brkRules = &brk_Rules_ଗଦା;
    UResourceBundle *brkName  = &brk_Name_ଗଦା;
    RuleBasedBreakIterator *result = nullptr;

    ଯଦି (U_FAILURE(status))
        ଫେରନ୍ତୁ nullptr;

    ures_init_ଗଦା_Object(brkRules);
    ures_init_ଗଦା_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    ଯଦି (U_SUCCESS(status)) {
        brkRules = ures_get_By_ଚାବି_With_Fallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_get_By_ଚାବି_With_Fallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((size_t)size<ଆକାର(fnbuff));
        ଯଦି (static_cast<size_t>(size) >= ଆକାର(fnbuff)) {
            size=0;
            ଯଦି (U_SUCCESS(status)) {
                status = U_BUFFER_OVERFLOW_ERROR;
            }
        }

        // Use the string if we found it
        ଯଦି (U_SUCCESS(status) && brkfname) {
            actual.append(ures_getLocaleInternal(brkName, &status), -1, status);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = 0;
            ଯଦି (extStart != nullptr){
                len = static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(extStart - brkfname);
                u_UCharsToChars(extStart+1, ext, ଆକାର(ext)); // nul terminates the buff
                u_UCharsToChars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brkRules);
    ures_close(brkName);

    UDataMemory* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &status);
    ଯଦି (U_FAILURE(status)) {
        ures_close(b);
        ଫେରନ୍ତୁ nullptr;
    }

    // Create a RuleBasedBreakIterator
    result = ନୂତନ RuleBasedBreakIterator(file, uprv_strstr(type, "phrase") != nullptr, status);

    // If there is a result, set the valid locale and actual locale, and the kind
    ଯଦି (U_SUCCESS(status) && result != nullptr) {
        result->actualLocale = Locale(actual.data());
        result->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &status));
        result->requestLocale = loc;
    }

    ures_close(b);

    ଯଦି (U_FAILURE(status) && result != nullptr) {  // Sometimes redundant check, but simple
        delete result;
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି (result == nullptr) {
        udata_close(file);
        ଯଦି (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    ଫେରନ୍ତୁ result;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(ଅଚଳ Locale& ଚାବି, UErrorCode& status)
{
    ଫେରନ୍ତୁ createInstance(ଚାବି, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_ଧାଡ଼ି_Instance(ଅଚଳ Locale& ଚାବି, UErrorCode& status)
{
    ଫେରନ୍ତୁ createInstance(ଚାବି, UBRK_ଧାଡ଼ି, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ଅଚଳ Locale& ଚାବି, UErrorCode& status)
{
    ଫେରନ୍ତୁ createInstance(ଚାବି, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ଅଚଳ Locale& ଚାବି, UErrorCode& status)
{
    ଫେରନ୍ତୁ createInstance(ଚାବି, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ଅଚଳ Locale& ଚାବି, UErrorCode& status)
{
    ଫେରନ୍ତୁ createInstance(ଚାବି, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ଅଚଳ Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    ଫେରନ୍ତୁ Locale::getAvailableLocales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_ମୂଳ()), validLocale(Locale::get_ମୂଳ()), requestLocale(Locale::get_ମୂଳ())
{
}

BreakIterator::BreakIterator(ଅଚଳ BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::operator=(ଅଚଳ BreakIterator& other) U_LIFETIME_BOUND {
    ଯଦି (ଏହି != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#ଯଦି !UCONFIG_NO_SERVICE

// -------------------------------------

ଶ୍ରେଣୀ ICUBreakIteratorFactory : ସର୍ବସାଧାରଣ ICUResourceBundleFactory {
ସର୍ବସାଧାରଣ:
    ଆଭାସୀ ~ICUBreakIteratorFactory();
ସୁରକ୍ଷିତ:
    ଆଭାସୀ UObject* handleCreate(ଅଚଳ Locale& loc, int32_t kind, ଅଚଳ ICUService* /*service*/, UErrorCode& status) ଅଚଳ override {
        ଫେରନ୍ତୁ BreakIterator::makeInstance(loc, kind, status);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

ଶ୍ରେଣୀ ICUBreakIteratorService : ସର୍ବସାଧାରଣ ICULocaleService {
ସର୍ବସାଧାରଣ:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UErrorCode status = U_ZERO_ERROR;
        registerFactory(ନୂତନ ICUBreakIteratorFactory(), status);
    }

    ଆଭାସୀ ~ICUBreakIteratorService();

    ଆଭାସୀ UObject* cloneInstance(UObject* instance) ଅଚଳ override {
        ଫେରନ୍ତୁ ((BreakIterator*)instance)->clone();
    }

    ଆଭାସୀ UObject* handleDefault(ଅଚଳ ICUService_ଚାବି& ଚାବି, UnicodeString* /*actualID*/, UErrorCode& status) ଅଚଳ override {
        Locale_ଚାବି& lkey = static_cast<Locale_ଚାବି&>(const_cast<ICUService_ଚାବି&>(ଚାବି));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        ଫେରନ୍ତୁ BreakIterator::makeInstance(loc, kind, status);
    }

    ଆଭାସୀ UBool isDefault() ଅଚଳ override {
        ଫେରନ୍ତୁ ଗଣନା_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_END

ସ୍ଥିର icu::UInitOnce gInitOnceBrkiter {};
ସ୍ଥିର icu::ICULocaleService* gService = nullptr;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
ସ୍ଥିର UBool U_CALLCONV breakiterator_cleanup() {
#ଯଦି !UCONFIG_NO_SERVICE
    ଯଦି (gService) {
        delete gService;
        gService = nullptr;
    }
    gInitOnceBrkiter.reset();
#endif
    ଫେରନ୍ତୁ ସତ୍ୟ;
}
U_CDECL_END
U_NAMESPACE_BEGIN

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV 
initService() {
    gService = ନୂତନ ICUBreakIteratorService();
    ucln_common_registerCleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

ସ୍ଥିର ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    ଫେରନ୍ତୁ gService;
}


// -------------------------------------

ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
hasService()
{
    ଫେରନ୍ତୁ !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistry_ଚାବି U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, ଅଚଳ Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    ଯଦି (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_ଚାବି ଚାବି, UErrorCode& status)
{
    ଯଦି (U_SUCCESS(status)) {
        ଯଦି (hasService()) {
            ଫେରନ୍ତୁ gService->unregister(ଚାବି, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    ଯଦି (service == nullptr) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ service->getAvailableLocales();
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ଅଚଳ Locale& loc, int32_t kind, UErrorCode& status)
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }

#ଯଦି !UCONFIG_NO_SERVICE
    ଯଦି (hasService()) {
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
        ଯଦି (U_SUCCESS(status) && (result != nullptr) && *actualLoc.getName() != 0) {
            result->actualLocale = actualLoc;
            result->validLocale = actualLoc;
        }
        ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା
#endif
    {
        ଫେରନ୍ତୁ makeInstance(loc, kind, status);
    }
}

// -------------------------------------
ଗଣନା { k_ଚାବି_ମୂଲ୍ୟ_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ଅଚଳ Locale& loc, int32_t kind, UErrorCode& status)
{

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    BreakIterator *result = nullptr;
    ବାଛ (kind) {
    ସ୍ଥିତି UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            result = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            result = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UBRK_ଧାଡ଼ି:
        {
            ଅକ୍ଷର lb_lw[k_ଚାବି_ମୂଲ୍ୟ_Len_Max];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_ଧାଡ଼ି);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            auto value = loc.get_Keyword_ମୂଲ୍ୟ<CharString>("lb", kvStatus);
            ଯଦି (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.data());
            }
            // lw=phrase is only supported in Japanese and Korean
            ଯଦି (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_ମୂଲ୍ୟ<CharString>("lw", kvStatus);
                ଯଦି (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.data());
                }
            }
            result = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            result = BreakIterator::buildInstance(loc, "sentence", status);
#ଯଦି !UCONFIG_NO_FILTERED_BREAK_ITERATION
            ଅକ୍ଷର ss_ଚାବି_ମୂଲ୍ୟ[k_ଚାବି_ମୂଲ୍ୟ_Len_Max] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_ମୂଲ୍ୟ("ss", ss_ଚାବି_ମୂଲ୍ୟ, k_ଚାବି_ମୂଲ୍ୟ_Len_Max, kvStatus);
            ଯଦି (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ss_ଚାବି_ମୂଲ୍ୟ,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                ଯଦି (U_SUCCESS(kvStatus)) {
                    result = fbiBuilder->build(result, status);
                    delete fbiBuilder;
                }
            }
#endif
            UTRACE_EXIT_STATUS(status);
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            result = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    ଯଦି (U_FAILURE(status)) {
        delete result;
        ଫେରନ୍ତୁ nullptr;
    }

    ଫେରନ୍ତୁ result;
}

Locale
BreakIterator::getLocale(ULocDataLocaleType type, UErrorCode& status) ଅଚଳ {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ Locale::get_ମୂଳ();
    }
    ଯଦି (type == ULOC_REQUESTED_LOCALE) {
        ଫେରନ୍ତୁ requestLocale;
    }
    ଫେରନ୍ତୁ LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

ଅଚଳ ଅକ୍ଷର*
BreakIterator::getLocaleID(ULocDataLocaleType type, UErrorCode& status) ଅଚଳ U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (type == ULOC_REQUESTED_LOCALE) {
        ଫେରନ୍ତୁ requestLocale.getName();
    }
    ଫେରନ୍ତୁ LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() ଅଚଳ {
    ଫେରନ୍ତୁ 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (capacity < 1) {
        status = U_BUFFER_OVERFLOW_ERROR;
        ଫେରନ୍ତୁ 1;
    }
    *fillInVec = 0;
    ଫେରନ୍ତୁ 1;
}

BreakIterator::BreakIterator(ଅଚଳ Locale& valid, ଅଚଳ Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_ମୂଳ()) {
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
