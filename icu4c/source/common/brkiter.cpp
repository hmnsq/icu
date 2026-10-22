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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাৱ "unicode/rbbi.h"
#অন্তর্ভাৱ "unicode/brkiter.h"
#অন্তর্ভাৱ "unicode/udata.h"
#অন্তর্ভাৱ "unicode/uloc.h"
#অন্তর্ভাৱ "unicode/ures.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/filteredbrk.h"
#অন্তর্ভাৱ "bytesinkutil.h"
#অন্তর্ভাৱ "ucln_cmn.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "umutex.h"
#অন্তর্ভাৱ "servloc.h"
#অন্তর্ভাৱ "locbased.h"
#অন্তর্ভাৱ "uresimp.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "ubrkimpl.h"
#অন্তর্ভাৱ "utracimp.h"
#অন্তর্ভাৱ "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(ধ্রুৱ Locale& loc, ধ্রুৱ ৱর্ণ *type, UErrorCode &status)
{
    ৱর্ণ fnbuff[256];
    ৱর্ণ ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ধ্রুৱ char16_t* brkfname = nullptr;
    UResourceBundle brk_Rules_স্তূপ;
    UResourceBundle brk_Name_স্তূপ;
    UResourceBundle *brkRules = &brk_Rules_স্তূপ;
    UResourceBundle *brkName  = &brk_Name_স্তূপ;
    RuleBasedBreakIterator *result = nullptr;

    যদি (U_FAILURE(status))
        ঘূৰাই nullptr;

    ures_init_স্তূপ_Object(brkRules);
    ures_init_স্তূপ_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    যদি (U_SUCCESS(status)) {
        brkRules = ures_get_By_চাবি_With_Fallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_get_By_চাবি_With_Fallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((size_t)size<পরিমাণ(fnbuff));
        যদি (static_cast<size_t>(size) >= পরিমাণ(fnbuff)) {
            size=0;
            যদি (U_SUCCESS(status)) {
                status = U_BUFFER_OVERFLOW_ERROR;
            }
        }

        // Use the string if we found it
        যদি (U_SUCCESS(status) && brkfname) {
            actual.append(ures_getLocaleInternal(brkName, &status), -1, status);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            পূর্ণাঙ্ক len = 0;
            যদি (extStart != nullptr){
                len = static_cast<পূর্ণাঙ্ক>(extStart - brkfname);
                u_UCharsToChars(extStart+1, ext, পরিমাণ(ext)); // nul terminates the buff
                u_UCharsToChars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brkRules);
    ures_close(brkName);

    UDataMemory* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &status);
    যদি (U_FAILURE(status)) {
        ures_close(b);
        ঘূৰাই nullptr;
    }

    // Create a RuleBasedBreakIterator
    result = নূতন RuleBasedBreakIterator(file, uprv_strstr(type, "phrase") != nullptr, status);

    // If there is a result, set the valid locale and actual locale, and the kind
    যদি (U_SUCCESS(status) && result != nullptr) {
        result->actualLocale = Locale(actual.data());
        result->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &status));
        result->requestLocale = loc;
    }

    ures_close(b);

    যদি (U_FAILURE(status) && result != nullptr) {  // Sometimes redundant check, but simple
        delete result;
        ঘূৰাই nullptr;
    }

    যদি (result == nullptr) {
        udata_close(file);
        যদি (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    ঘূৰাই result;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(ধ্রুৱ Locale& চাবি, UErrorCode& status)
{
    ঘূৰাই createInstance(চাবি, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_সাৰি_Instance(ধ্রুৱ Locale& চাবি, UErrorCode& status)
{
    ঘূৰাই createInstance(চাবি, UBRK_সাৰি, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ধ্রুৱ Locale& চাবি, UErrorCode& status)
{
    ঘূৰাই createInstance(চাবি, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ধ্রুৱ Locale& চাবি, UErrorCode& status)
{
    ঘূৰাই createInstance(চাবি, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ধ্রুৱ Locale& চাবি, UErrorCode& status)
{
    ঘূৰাই createInstance(চাবি, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ধ্রুৱ Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    ঘূৰাই Locale::getAvailableLocales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_মূল()), validLocale(Locale::get_মূল()), requestLocale(Locale::get_মূল())
{
}

BreakIterator::BreakIterator(ধ্রুৱ BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::operator=(ধ্রুৱ BreakIterator& other) U_LIFETIME_BOUND {
    যদি (এতত্ != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    ঘূৰাই *এতত্;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#যদি !UCONFIG_NO_SERVICE

// -------------------------------------

শ্ৰেণী ICUBreakIteratorFactory : সার্ৱজনিক ICUResourceBundleFactory {
সার্ৱজনিক:
    আভাস ~ICUBreakIteratorFactory();
রক্ষিত:
    আভাস UObject* handleCreate(ধ্রুৱ Locale& loc, int32_t kind, ধ্রুৱ ICUService* /*service*/, UErrorCode& status) ধ্রুৱ override {
        ঘূৰাই BreakIterator::makeInstance(loc, kind, status);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

শ্ৰেণী ICUBreakIteratorService : সার্ৱজনিক ICULocaleService {
সার্ৱজনিক:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UErrorCode status = U_ZERO_ERROR;
        registerFactory(নূতন ICUBreakIteratorFactory(), status);
    }

    আভাস ~ICUBreakIteratorService();

    আভাস UObject* cloneInstance(UObject* instance) ধ্রুৱ override {
        ঘূৰাই ((BreakIterator*)instance)->clone();
    }

    আভাস UObject* handleDefault(ধ্রুৱ ICUService_চাবি& চাবি, UnicodeString* /*actualID*/, UErrorCode& status) ধ্রুৱ override {
        Locale_চাবি& lkey = static_cast<Locale_চাবি&>(const_cast<ICUService_চাবি&>(চাবি));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        ঘূৰাই BreakIterator::makeInstance(loc, kind, status);
    }

    আভাস UBool isDefault() ধ্রুৱ override {
        ঘূৰাই গুনতি_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_END

স্থির icu::UInitOnce gInitOnceBrkiter {};
স্থির icu::ICULocaleService* gService = nullptr;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
স্থির UBool U_CALLCONV breakiterator_cleanup() {
#যদি !UCONFIG_NO_SERVICE
    যদি (gService) {
        delete gService;
        gService = nullptr;
    }
    gInitOnceBrkiter.reset();
#endif
    ঘূৰাই সঁচা;
}
U_CDECL_END
U_NAMESPACE_BEGIN

স্থির শূন্য U_CALLCONV 
initService() {
    gService = নূতন ICUBreakIteratorService();
    ucln_common_registerCleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

স্থির ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    ঘূৰাই gService;
}


// -------------------------------------

স্থির অন্তঃস্থ UBool
hasService()
{
    ঘূৰাই !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistry_চাবি U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, ধ্রুৱ Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    যদি (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }
    ঘূৰাই service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_চাবি চাবি, UErrorCode& status)
{
    যদি (U_SUCCESS(status)) {
        যদি (hasService()) {
            ঘূৰাই gService->unregister(চাবি, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ঘূৰাই মিছা;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    যদি (service == nullptr) {
        ঘূৰাই nullptr;
    }
    ঘূৰাই service->getAvailableLocales();
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ধ্রুৱ Locale& loc, int32_t kind, UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ঘূৰাই nullptr;
    }

#যদি !UCONFIG_NO_SERVICE
    যদি (hasService()) {
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
        যদি (U_SUCCESS(status) && (result != nullptr) && *actualLoc.getName() != 0) {
            result->actualLocale = actualLoc;
            result->validLocale = actualLoc;
        }
        ঘূৰাই result;
    }
    অন্যথা
#endif
    {
        ঘূৰাই makeInstance(loc, kind, status);
    }
}

// -------------------------------------
গণনা { k_চাবি_মান_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ধ্রুৱ Locale& loc, int32_t kind, UErrorCode& status)
{

    যদি (U_FAILURE(status)) {
        ঘূৰাই nullptr;
    }

    BreakIterator *result = nullptr;
    ৱিকল্প (kind) {
    পক্ষ UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            result = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        ভঙ্গ;
    পক্ষ UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            result = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        ভঙ্গ;
    পক্ষ UBRK_সাৰি:
        {
            ৱর্ণ lb_lw[k_চাবি_মান_Len_Max];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_সাৰি);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            auto value = loc.get_Keyword_মান<CharString>("lb", kvStatus);
            যদি (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.data());
            }
            // lw=phrase is only supported in Japanese and Korean
            যদি (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_মান<CharString>("lw", kvStatus);
                যদি (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.data());
                }
            }
            result = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        ভঙ্গ;
    পক্ষ UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            result = BreakIterator::buildInstance(loc, "sentence", status);
#যদি !UCONFIG_NO_FILTERED_BREAK_ITERATION
            ৱর্ণ ss_চাবি_মান[k_চাবি_মান_Len_Max] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_মান("ss", ss_চাবি_মান, k_চাবি_মান_Len_Max, kvStatus);
            যদি (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ss_চাবি_মান,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                যদি (U_SUCCESS(kvStatus)) {
                    result = fbiBuilder->build(result, status);
                    delete fbiBuilder;
                }
            }
#endif
            UTRACE_EXIT_STATUS(status);
        }
        ভঙ্গ;
    পক্ষ UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            result = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        ভঙ্গ;
    স্ৱভাৱ:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    যদি (U_FAILURE(status)) {
        delete result;
        ঘূৰাই nullptr;
    }

    ঘূৰাই result;
}

Locale
BreakIterator::getLocale(ULocDataLocaleType type, UErrorCode& status) ধ্রুৱ {
    যদি (U_FAILURE(status)) {
        ঘূৰাই Locale::get_মূল();
    }
    যদি (type == ULOC_REQUESTED_LOCALE) {
        ঘূৰাই requestLocale;
    }
    ঘূৰাই LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

ধ্রুৱ ৱর্ণ*
BreakIterator::getLocaleID(ULocDataLocaleType type, UErrorCode& status) ধ্রুৱ U_LIFETIME_BOUND {
    যদি (U_FAILURE(status)) {
        ঘূৰাই nullptr;
    }
    যদি (type == ULOC_REQUESTED_LOCALE) {
        ঘূৰাই requestLocale.getName();
    }
    ঘূৰাই LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() ধ্রুৱ {
    ঘূৰাই 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই 0;
    }
    যদি (capacity < 1) {
        status = U_BUFFER_OVERFLOW_ERROR;
        ঘূৰাই 1;
    }
    *fillInVec = 0;
    ঘূৰাই 1;
}

BreakIterator::BreakIterator(ধ্রুৱ Locale& valid, ধ্রুৱ Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_মূল()) {
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
