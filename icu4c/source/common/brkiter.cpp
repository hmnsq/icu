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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/rbbi.h"
#समावेश "unicode/brkiter.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/uloc.h"
#समावेश "unicode/ures.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/filteredbrk.h"
#समावेश "bytesinkutil.h"
#समावेश "ucln_cmn.h"
#समावेश "cstring.h"
#समावेश "umutex.h"
#समावेश "servloc.h"
#समावेश "locbased.h"
#समावेश "uresimp.h"
#समावेश "uassert.h"
#समावेश "ubrkimpl.h"
#समावेश "utracimp.h"
#समावेश "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(स्थिर Locale& loc, स्थिर अक्षर *type, UErrorCode &status)
{
    अक्षर fnbuff[256];
    अक्षर ext[4]={'\0'};
    CharString actual;
    int32_t size;
    स्थिर char16_t* brkfname = nullptr;
    UResourceBundle brk_Rules_ढेर;
    UResourceBundle brk_Name_ढेर;
    UResourceBundle *brkRules = &brk_Rules_ढेर;
    UResourceBundle *brkName  = &brk_Name_ढेर;
    RuleBasedBreakIterator *परिणाम = nullptr;

    अगर (U_FAILURE(status))
        वापस nullptr;

    ures_init_ढेर_Object(brkRules);
    ures_init_ढेर_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    अगर (U_SUCCESS(status)) {
        brkRules = ures_get_By_कुंजी_With_Fallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_get_By_कुंजी_With_Fallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((माप_प्रकार)size<माप(fnbuff));
        अगर (static_cast<माप_प्रकार>(size) >= माप(fnbuff)) {
            size=0;
            अगर (U_SUCCESS(status)) {
                status = U_बफर_OVERFLOW_ERROR;
            }
        }

        // Use the string if we found it
        अगर (U_SUCCESS(status) && brkfname) {
            actual.append(ures_getLocaleInternal(brkName, &status), -1, status);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            पूर्णांक len = 0;
            अगर (extStart != nullptr){
                len = static_cast<पूर्णांक>(extStart - brkfname);
                u_UCharsToChars(extStart+1, ext, माप(ext)); // nul terminates the buff
                u_UCharsToChars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brkRules);
    ures_close(brkName);

    UDataMemory* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &status);
    अगर (U_FAILURE(status)) {
        ures_close(b);
        वापस nullptr;
    }

    // Create a RuleBasedBreakIterator
    परिणाम = नया RuleBasedBreakIterator(file, uprv_strstr(type, "phrase") != nullptr, status);

    // If there is a result, set the valid locale and actual locale, and the kind
    अगर (U_SUCCESS(status) && परिणाम != nullptr) {
        परिणाम->actualLocale = Locale(actual.तथ्य());
        परिणाम->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &status));
        परिणाम->requestLocale = loc;
    }

    ures_close(b);

    अगर (U_FAILURE(status) && परिणाम != nullptr) {  // Sometimes redundant check, but simple
        मिटाओ परिणाम;
        वापस nullptr;
    }

    अगर (परिणाम == nullptr) {
        udata_close(file);
        अगर (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    वापस परिणाम;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(स्थिर Locale& कुंजी, UErrorCode& status)
{
    वापस createInstance(कुंजी, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_पंक्ति_Instance(स्थिर Locale& कुंजी, UErrorCode& status)
{
    वापस createInstance(कुंजी, UBRK_पंक्ति, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(स्थिर Locale& कुंजी, UErrorCode& status)
{
    वापस createInstance(कुंजी, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(स्थिर Locale& कुंजी, UErrorCode& status)
{
    वापस createInstance(कुंजी, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(स्थिर Locale& कुंजी, UErrorCode& status)
{
    वापस createInstance(कुंजी, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
स्थिर Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    वापस Locale::getAvailableLocales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_मूल()), validLocale(Locale::get_मूल()), requestLocale(Locale::get_मूल())
{
}

BreakIterator::BreakIterator(स्थिर BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::चालक=(स्थिर BreakIterator& other) U_LIFETIME_BOUND {
    अगर (यह != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    वापस *यह;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#अगर !UCONFIG_NO_SERVICE

// -------------------------------------

श्रेणी ICUBreakIteratorFactory : खुला ICUResourceBundleFactory {
खुला:
    भव ~ICUBreakIteratorFactory();
रक्षित:
    भव UObject* handleCreate(स्थिर Locale& loc, int32_t kind, स्थिर ICUService* /*service*/, UErrorCode& status) स्थिर override {
        वापस BreakIterator::makeInstance(loc, kind, status);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

श्रेणी ICUBreakIteratorService : खुला ICULocaleService {
खुला:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UErrorCode status = U_ZERO_ERROR;
        registerFactory(नया ICUBreakIteratorFactory(), status);
    }

    भव ~ICUBreakIteratorService();

    भव UObject* cloneInstance(UObject* instance) स्थिर override {
        वापस ((BreakIterator*)instance)->clone();
    }

    भव UObject* handleDefault(स्थिर ICUService_कुंजी& कुंजी, UnicodeString* /*actualID*/, UErrorCode& status) स्थिर override {
        Locale_कुंजी& lkey = static_cast<Locale_कुंजी&>(const_cast<ICUService_कुंजी&>(कुंजी));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        वापस BreakIterator::makeInstance(loc, kind, status);
    }

    भव UBool isDefault() स्थिर override {
        वापस गिनती_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_END

अटल icu::UInitOnce gInitOnceBrkiter {};
अटल icu::ICULocaleService* gService = nullptr;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
अटल UBool U_CALLCONV breakiterator_cleanup() {
#अगर !UCONFIG_NO_SERVICE
    अगर (gService) {
        मिटाओ gService;
        gService = nullptr;
    }
    gInitOnceBrkiter.reset();
#पूर्ण_अगर
    वापस true;
}
U_CDECL_END
U_NAMESPACE_BEGIN

अटल व्योम U_CALLCONV 
initService() {
    gService = नया ICUBreakIteratorService();
    ucln_common_registerCleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

अटल ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    वापस gService;
}


// -------------------------------------

अटल अंतरभूत UBool
hasService()
{
    वापस !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistry_कुंजी U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, स्थिर Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    अगर (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    वापस service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_कुंजी कुंजी, UErrorCode& status)
{
    अगर (U_SUCCESS(status)) {
        अगर (hasService()) {
            वापस gService->unregister(कुंजी, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    वापस false;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    अगर (service == nullptr) {
        वापस nullptr;
    }
    वापस service->getAvailableLocales();
}
#पूर्ण_अगर /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(स्थिर Locale& loc, int32_t kind, UErrorCode& status)
{
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

#अगर !UCONFIG_NO_SERVICE
    अगर (hasService()) {
        Locale actualLoc("");
        BreakIterator *परिणाम = (BreakIterator*)gService->लो(loc, kind, &actualLoc, status);
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
        अगर (U_SUCCESS(status) && (परिणाम != nullptr) && *actualLoc.getName() != 0) {
            परिणाम->actualLocale = actualLoc;
            परिणाम->validLocale = actualLoc;
        }
        वापस परिणाम;
    }
    अन्यथा
#पूर्ण_अगर
    {
        वापस makeInstance(loc, kind, status);
    }
}

// -------------------------------------
क्रमागत { k_कुंजी_मूल्य_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(स्थिर Locale& loc, int32_t kind, UErrorCode& status)
{

    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    BreakIterator *परिणाम = nullptr;
    चयन (kind) {
    हाल UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            परिणाम = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        अवरोध;
    हाल UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            परिणाम = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        अवरोध;
    हाल UBRK_पंक्ति:
        {
            अक्षर lb_lw[k_कुंजी_मूल्य_Len_Max];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_पंक्ति);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            स्वतः value = loc.get_Keyword_मूल्य<CharString>("lb", kvStatus);
            अगर (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.तथ्य());
            }
            // lw=phrase is only supported in Japanese and Korean
            अगर (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_मूल्य<CharString>("lw", kvStatus);
                अगर (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.तथ्य());
                }
            }
            परिणाम = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        अवरोध;
    हाल UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            परिणाम = BreakIterator::buildInstance(loc, "sentence", status);
#अगर !UCONFIG_NO_FILTERED_BREAK_ITERATION
            अक्षर ss_कुंजी_मूल्य[k_कुंजी_मूल्य_Len_Max] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_मूल्य("ss", ss_कुंजी_मूल्य, k_कुंजी_मूल्य_Len_Max, kvStatus);
            अगर (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ss_कुंजी_मूल्य,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                अगर (U_SUCCESS(kvStatus)) {
                    परिणाम = fbiBuilder->build(परिणाम, status);
                    मिटाओ fbiBuilder;
                }
            }
#पूर्ण_अगर
            UTRACE_EXIT_STATUS(status);
        }
        अवरोध;
    हाल UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            परिणाम = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        अवरोध;
    शेष:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    अगर (U_FAILURE(status)) {
        मिटाओ परिणाम;
        वापस nullptr;
    }

    वापस परिणाम;
}

Locale
BreakIterator::getLocale(ULoc_तथ्य_Locale_Type type, UErrorCode& status) स्थिर {
    अगर (U_FAILURE(status)) {
        वापस Locale::get_मूल();
    }
    अगर (type == ULOC_REQUESTED_LOCALE) {
        वापस requestLocale;
    }
    वापस LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

स्थिर अक्षर*
BreakIterator::getLocaleID(ULoc_तथ्य_Locale_Type type, UErrorCode& status) स्थिर U_LIFETIME_BOUND {
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }
    अगर (type == ULOC_REQUESTED_LOCALE) {
        वापस requestLocale.getName();
    }
    वापस LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() स्थिर {
    वापस 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस 0;
    }
    अगर (capacity < 1) {
        status = U_बफर_OVERFLOW_ERROR;
        वापस 1;
    }
    *fillInVec = 0;
    वापस 1;
}

BreakIterator::BreakIterator(स्थिर Locale& valid, स्थिर Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_मूल()) {
}

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
