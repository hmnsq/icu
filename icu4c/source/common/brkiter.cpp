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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "unicode/rbbi.h"
#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/uloc.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/filteredbrk.h"
#அந்தர்பாவ "bytesinkutil.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "servloc.h"
#அந்தர்பாவ "locbased.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ubrkimpl.h"
#அந்தர்பாவ "utracimp.h"
#அந்தர்பாவ "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(மாறிலி Locale& loc, மாறிலி எழுத்து *type, UErrorCode &status)
{
    எழுத்து fnbuff[256];
    எழுத்து ext[4]={'\0'};
    CharString actual;
    int32_t size;
    மாறிலி char16_t* brkfname = nullptr;
    UResourceBundle brk_Rules_அடுக்கு;
    UResourceBundle brk_Name_அடுக்கு;
    UResourceBundle *brkRules = &brk_Rules_அடுக்கு;
    UResourceBundle *brkName  = &brk_Name_அடுக்கு;
    RuleBasedBreakIterator *result = nullptr;

    எனில் (U_FAILURE(status))
        திரும்பு nullptr;

    ures_init_அடுக்கு_Object(brkRules);
    ures_init_அடுக்கு_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    எனில் (U_SUCCESS(status)) {
        brkRules = ures_get_By_சாவி_With_Fallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_get_By_சாவி_With_Fallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((size_t)size<அளவு(fnbuff));
        எனில் (static_cast<size_t>(size) >= அளவு(fnbuff)) {
            size=0;
            எனில் (U_SUCCESS(status)) {
                status = U_BUFFER_OVERFLOW_ERROR;
            }
        }

        // Use the string if we found it
        எனில் (U_SUCCESS(status) && brkfname) {
            actual.append(ures_getLocaleInternal(brkName, &status), -1, status);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            முழு len = 0;
            எனில் (extStart != nullptr){
                len = static_cast<முழு>(extStart - brkfname);
                u_UCharsToChars(extStart+1, ext, அளவு(ext)); // nul terminates the buff
                u_UCharsToChars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brkRules);
    ures_close(brkName);

    UDataMemory* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &status);
    எனில் (U_FAILURE(status)) {
        ures_close(b);
        திரும்பு nullptr;
    }

    // Create a RuleBasedBreakIterator
    result = புதிய RuleBasedBreakIterator(file, uprv_strstr(type, "phrase") != nullptr, status);

    // If there is a result, set the valid locale and actual locale, and the kind
    எனில் (U_SUCCESS(status) && result != nullptr) {
        result->actualLocale = Locale(actual.data());
        result->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &status));
        result->requestLocale = loc;
    }

    ures_close(b);

    எனில் (U_FAILURE(status) && result != nullptr) {  // Sometimes redundant check, but simple
        delete result;
        திரும்பு nullptr;
    }

    எனில் (result == nullptr) {
        udata_close(file);
        எனில் (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    திரும்பு result;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(மாறிலி Locale& சாவி, UErrorCode& status)
{
    திரும்பு createInstance(சாவி, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_வரி_Instance(மாறிலி Locale& சாவி, UErrorCode& status)
{
    திரும்பு createInstance(சாவி, UBRK_வரி, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(மாறிலி Locale& சாவி, UErrorCode& status)
{
    திரும்பு createInstance(சாவி, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(மாறிலி Locale& சாவி, UErrorCode& status)
{
    திரும்பு createInstance(சாவி, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(மாறிலி Locale& சாவி, UErrorCode& status)
{
    திரும்பு createInstance(சாவி, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
மாறிலி Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    திரும்பு Locale::getAvailableLocales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_வேர்()), validLocale(Locale::get_வேர்()), requestLocale(Locale::get_வேர்())
{
}

BreakIterator::BreakIterator(மாறிலி BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::operator=(மாறிலி BreakIterator& other) U_LIFETIME_BOUND {
    எனில் (இது != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    திரும்பு *இது;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#எனில் !UCONFIG_NO_SERVICE

// -------------------------------------

வகுப்பு ICUBreakIteratorFactory : பொது ICUResourceBundleFactory {
பொது:
    மெய்நிகர் ~ICUBreakIteratorFactory();
பாதுகாக்கப்பட்ட:
    மெய்நிகர் UObject* handleCreate(மாறிலி Locale& loc, int32_t kind, மாறிலி ICUService* /*service*/, UErrorCode& status) மாறிலி override {
        திரும்பு BreakIterator::makeInstance(loc, kind, status);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

வகுப்பு ICUBreakIteratorService : பொது ICULocaleService {
பொது:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UErrorCode status = U_ZERO_ERROR;
        registerFactory(புதிய ICUBreakIteratorFactory(), status);
    }

    மெய்நிகர் ~ICUBreakIteratorService();

    மெய்நிகர் UObject* cloneInstance(UObject* instance) மாறிலி override {
        திரும்பு ((BreakIterator*)instance)->clone();
    }

    மெய்நிகர் UObject* handleDefault(மாறிலி ICUService_சாவி& சாவி, UnicodeString* /*actualID*/, UErrorCode& status) மாறிலி override {
        Locale_சாவி& lkey = static_cast<Locale_சாவி&>(const_cast<ICUService_சாவி&>(சாவி));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        திரும்பு BreakIterator::makeInstance(loc, kind, status);
    }

    மெய்நிகர் UBool isDefault() மாறிலி override {
        திரும்பு மொத்தம்_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_END

நிலையான icu::UInitOnce gInitOnceBrkiter {};
நிலையான icu::ICULocaleService* gService = nullptr;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
நிலையான UBool U_CALLCONV breakiterator_cleanup() {
#எனில் !UCONFIG_NO_SERVICE
    எனில் (gService) {
        delete gService;
        gService = nullptr;
    }
    gInitOnceBrkiter.reset();
#endif
    திரும்பு உண்மை;
}
U_CDECL_END
U_NAMESPACE_BEGIN

நிலையான வெறுமை U_CALLCONV 
initService() {
    gService = புதிய ICUBreakIteratorService();
    ucln_common_registerCleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

நிலையான ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    திரும்பு gService;
}


// -------------------------------------

நிலையான உள்வரி UBool
hasService()
{
    திரும்பு !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistry_சாவி U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, மாறிலி Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    எனில் (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    திரும்பு service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_சாவி சாவி, UErrorCode& status)
{
    எனில் (U_SUCCESS(status)) {
        எனில் (hasService()) {
            திரும்பு gService->unregister(சாவி, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    திரும்பு பொய்;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    எனில் (service == nullptr) {
        திரும்பு nullptr;
    }
    திரும்பு service->getAvailableLocales();
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(மாறிலி Locale& loc, int32_t kind, UErrorCode& status)
{
    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }

#எனில் !UCONFIG_NO_SERVICE
    எனில் (hasService()) {
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
        எனில் (U_SUCCESS(status) && (result != nullptr) && *actualLoc.getName() != 0) {
            result->actualLocale = actualLoc;
            result->validLocale = actualLoc;
        }
        திரும்பு result;
    }
    இல்லையெனில்
#endif
    {
        திரும்பு makeInstance(loc, kind, status);
    }
}

// -------------------------------------
எண்ணிக்கை { k_சாவி_மதிப்பு_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(மாறிலி Locale& loc, int32_t kind, UErrorCode& status)
{

    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }

    BreakIterator *result = nullptr;
    தேர்வு (kind) {
    நிலை UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            result = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        நிறுத்து;
    நிலை UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            result = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        நிறுத்து;
    நிலை UBRK_வரி:
        {
            எழுத்து lb_lw[k_சாவி_மதிப்பு_Len_Max];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_வரி);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            auto value = loc.get_Keyword_மதிப்பு<CharString>("lb", kvStatus);
            எனில் (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.data());
            }
            // lw=phrase is only supported in Japanese and Korean
            எனில் (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_மதிப்பு<CharString>("lw", kvStatus);
                எனில் (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.data());
                }
            }
            result = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        நிறுத்து;
    நிலை UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            result = BreakIterator::buildInstance(loc, "sentence", status);
#எனில் !UCONFIG_NO_FILTERED_BREAK_ITERATION
            எழுத்து ss_சாவி_மதிப்பு[k_சாவி_மதிப்பு_Len_Max] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_மதிப்பு("ss", ss_சாவி_மதிப்பு, k_சாவி_மதிப்பு_Len_Max, kvStatus);
            எனில் (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ss_சாவி_மதிப்பு,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                எனில் (U_SUCCESS(kvStatus)) {
                    result = fbiBuilder->build(result, status);
                    delete fbiBuilder;
                }
            }
#endif
            UTRACE_EXIT_STATUS(status);
        }
        நிறுத்து;
    நிலை UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            result = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        நிறுத்து;
    இயல்பு:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    எனில் (U_FAILURE(status)) {
        delete result;
        திரும்பு nullptr;
    }

    திரும்பு result;
}

Locale
BreakIterator::getLocale(ULocDataLocaleType type, UErrorCode& status) மாறிலி {
    எனில் (U_FAILURE(status)) {
        திரும்பு Locale::get_வேர்();
    }
    எனில் (type == ULOC_REQUESTED_LOCALE) {
        திரும்பு requestLocale;
    }
    திரும்பு LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

மாறிலி எழுத்து*
BreakIterator::getLocaleID(ULocDataLocaleType type, UErrorCode& status) மாறிலி U_LIFETIME_BOUND {
    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }
    எனில் (type == ULOC_REQUESTED_LOCALE) {
        திரும்பு requestLocale.getName();
    }
    திரும்பு LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() மாறிலி {
    திரும்பு 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு 0;
    }
    எனில் (capacity < 1) {
        status = U_BUFFER_OVERFLOW_ERROR;
        திரும்பு 1;
    }
    *fillInVec = 0;
    திரும்பு 1;
}

BreakIterator::BreakIterator(மாறிலி Locale& valid, மாறிலி Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_வேர்()) {
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
