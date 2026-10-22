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

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/rbbi.h"
#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/filteredbrk.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "servloc.h"
#अन्तर्भाव "locbased.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ubrkimpl.h"
#अन्तर्भाव "utracimp.h"
#अन्तर्भाव "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(स्थिरांक Locale& loc, स्थिरांक अक्षर *type, UErrorCode &status)
{
    अक्षर fnbuff[256];
    अक्षर ext[4]={'\0'};
    CharString actual;
    int32_t size;
    स्थिरांक char16_t* brkfname = nullptr;
    UResourceBundle brk_Rules_चळत;
    UResourceBundle brk_Name_चळत;
    UResourceBundle *brkRules = &brk_Rules_चळत;
    UResourceBundle *brkName  = &brk_Name_चळत;
    RuleBasedBreakIterator *result = nullptr;

    जर (U_FAILURE(status))
        परत nullptr;

    ures_init_चळत_Object(brkRules);
    ures_init_चळत_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    जर (U_SUCCESS(status)) {
        brkRules = ures_get_By_किल्ली_With_Fallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_get_By_किल्ली_With_Fallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((माप_प्रकार)size<आकार(fnbuff));
        जर (static_cast<माप_प्रकार>(size) >= आकार(fnbuff)) {
            size=0;
            जर (U_SUCCESS(status)) {
                status = U_BUFFER_OVERFLOW_ERROR;
            }
        }

        // Use the string if we found it
        जर (U_SUCCESS(status) && brkfname) {
            actual.append(ures_getLocaleInternal(brkName, &status), -1, status);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            पूर्णांक len = 0;
            जर (extStart != nullptr){
                len = static_cast<पूर्णांक>(extStart - brkfname);
                u_UCharsToChars(extStart+1, ext, आकार(ext)); // nul terminates the buff
                u_UCharsToChars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brkRules);
    ures_close(brkName);

    UDataMemory* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &status);
    जर (U_FAILURE(status)) {
        ures_close(b);
        परत nullptr;
    }

    // Create a RuleBasedBreakIterator
    result = नवीन RuleBasedBreakIterator(file, uprv_strstr(type, "phrase") != nullptr, status);

    // If there is a result, set the valid locale and actual locale, and the kind
    जर (U_SUCCESS(status) && result != nullptr) {
        result->actualLocale = Locale(actual.data());
        result->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &status));
        result->requestLocale = loc;
    }

    ures_close(b);

    जर (U_FAILURE(status) && result != nullptr) {  // Sometimes redundant check, but simple
        delete result;
        परत nullptr;
    }

    जर (result == nullptr) {
        udata_close(file);
        जर (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    परत result;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(स्थिरांक Locale& किल्ली, UErrorCode& status)
{
    परत createInstance(किल्ली, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_ओळ_Instance(स्थिरांक Locale& किल्ली, UErrorCode& status)
{
    परत createInstance(किल्ली, UBRK_ओळ, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(स्थिरांक Locale& किल्ली, UErrorCode& status)
{
    परत createInstance(किल्ली, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(स्थिरांक Locale& किल्ली, UErrorCode& status)
{
    परत createInstance(किल्ली, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(स्थिरांक Locale& किल्ली, UErrorCode& status)
{
    परत createInstance(किल्ली, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
स्थिरांक Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    परत Locale::getAvailableLocales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_मूळ()), validLocale(Locale::get_मूळ()), requestLocale(Locale::get_मूळ())
{
}

BreakIterator::BreakIterator(स्थिरांक BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::चालक=(स्थिरांक BreakIterator& other) U_LIFETIME_BOUND {
    जर (हे != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    परत *हे;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#जर !UCONFIG_NO_SERVICE

// -------------------------------------

वर्ग ICUBreakIteratorFactory : सार्वजनिक ICUResourceBundleFactory {
सार्वजनिक:
    आभासी ~ICUBreakIteratorFactory();
संरक्षित:
    आभासी UObject* handleCreate(स्थिरांक Locale& loc, int32_t kind, स्थिरांक ICUService* /*service*/, UErrorCode& status) स्थिरांक override {
        परत BreakIterator::makeInstance(loc, kind, status);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

वर्ग ICUBreakIteratorService : सार्वजनिक ICULocaleService {
सार्वजनिक:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UErrorCode status = U_ZERO_ERROR;
        registerFactory(नवीन ICUBreakIteratorFactory(), status);
    }

    आभासी ~ICUBreakIteratorService();

    आभासी UObject* cloneInstance(UObject* instance) स्थिरांक override {
        परत ((BreakIterator*)instance)->clone();
    }

    आभासी UObject* handleDefault(स्थिरांक ICUService_किल्ली& किल्ली, UnicodeString* /*actualID*/, UErrorCode& status) स्थिरांक override {
        Locale_किल्ली& lkey = static_cast<Locale_किल्ली&>(const_cast<ICUService_किल्ली&>(किल्ली));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        परत BreakIterator::makeInstance(loc, kind, status);
    }

    आभासी UBool isDefault() स्थिरांक override {
        परत मोजणी_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_END

स्थिर icu::UInitOnce gInitOnceBrkiter {};
स्थिर icu::ICULocaleService* gService = nullptr;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
स्थिर UBool U_CALLCONV breakiterator_cleanup() {
#जर !UCONFIG_NO_SERVICE
    जर (gService) {
        delete gService;
        gService = nullptr;
    }
    gInitOnceBrkiter.reset();
#endif
    परत सत्य;
}
U_CDECL_END
U_NAMESPACE_BEGIN

स्थिर शून्य U_CALLCONV 
initService() {
    gService = नवीन ICUBreakIteratorService();
    ucln_common_registerCleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

स्थिर ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    परत gService;
}


// -------------------------------------

स्थिर ओळीत UBool
hasService()
{
    परत !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistry_किल्ली U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, स्थिरांक Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    जर (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    परत service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_किल्ली किल्ली, UErrorCode& status)
{
    जर (U_SUCCESS(status)) {
        जर (hasService()) {
            परत gService->unregister(किल्ली, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    परत असत्य;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    जर (service == nullptr) {
        परत nullptr;
    }
    परत service->getAvailableLocales();
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(स्थिरांक Locale& loc, int32_t kind, UErrorCode& status)
{
    जर (U_FAILURE(status)) {
        परत nullptr;
    }

#जर !UCONFIG_NO_SERVICE
    जर (hasService()) {
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
        जर (U_SUCCESS(status) && (result != nullptr) && *actualLoc.getName() != 0) {
            result->actualLocale = actualLoc;
            result->validLocale = actualLoc;
        }
        परत result;
    }
    अन्यथा
#endif
    {
        परत makeInstance(loc, kind, status);
    }
}

// -------------------------------------
गणना { k_किल्ली_मूल्य_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(स्थिरांक Locale& loc, int32_t kind, UErrorCode& status)
{

    जर (U_FAILURE(status)) {
        परत nullptr;
    }

    BreakIterator *result = nullptr;
    निवड (kind) {
    स्थिती UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            result = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        थांबा;
    स्थिती UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            result = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        थांबा;
    स्थिती UBRK_ओळ:
        {
            अक्षर lb_lw[k_किल्ली_मूल्य_Len_Max];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_ओळ);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            स्वतः value = loc.get_Keyword_मूल्य<CharString>("lb", kvStatus);
            जर (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.data());
            }
            // lw=phrase is only supported in Japanese and Korean
            जर (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_मूल्य<CharString>("lw", kvStatus);
                जर (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.data());
                }
            }
            result = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        थांबा;
    स्थिती UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            result = BreakIterator::buildInstance(loc, "sentence", status);
#जर !UCONFIG_NO_FILTERED_BREAK_ITERATION
            अक्षर ss_किल्ली_मूल्य[k_किल्ली_मूल्य_Len_Max] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_मूल्य("ss", ss_किल्ली_मूल्य, k_किल्ली_मूल्य_Len_Max, kvStatus);
            जर (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ss_किल्ली_मूल्य,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                जर (U_SUCCESS(kvStatus)) {
                    result = fbiBuilder->build(result, status);
                    delete fbiBuilder;
                }
            }
#endif
            UTRACE_EXIT_STATUS(status);
        }
        थांबा;
    स्थिती UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            result = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        थांबा;
    पूर्वनिर्धारित:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    जर (U_FAILURE(status)) {
        delete result;
        परत nullptr;
    }

    परत result;
}

Locale
BreakIterator::getLocale(ULocDataLocaleType type, UErrorCode& status) स्थिरांक {
    जर (U_FAILURE(status)) {
        परत Locale::get_मूळ();
    }
    जर (type == ULOC_REQUESTED_LOCALE) {
        परत requestLocale;
    }
    परत LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

स्थिरांक अक्षर*
BreakIterator::getLocaleID(ULocDataLocaleType type, UErrorCode& status) स्थिरांक U_LIFETIME_BOUND {
    जर (U_FAILURE(status)) {
        परत nullptr;
    }
    जर (type == ULOC_REQUESTED_LOCALE) {
        परत requestLocale.getName();
    }
    परत LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() स्थिरांक {
    परत 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    जर (U_FAILURE(status)) {
        परत 0;
    }
    जर (capacity < 1) {
        status = U_BUFFER_OVERFLOW_ERROR;
        परत 1;
    }
    *fillInVec = 0;
    परत 1;
}

BreakIterator::BreakIterator(स्थिरांक Locale& valid, स्थिरांक Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_मूळ()) {
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
