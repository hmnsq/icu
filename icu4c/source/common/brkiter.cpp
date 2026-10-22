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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/rbbi.h"
#شامل "unicode/brkiter.h"
#شامل "unicode/udata.h"
#شامل "unicode/uloc.h"
#شامل "unicode/ures.h"
#شامل "unicode/ustring.h"
#شامل "unicode/filteredbrk.h"
#شامل "bytesinkutil.h"
#شامل "ucln_cmn.h"
#شامل "cstring.h"
#شامل "umutex.h"
#شامل "servloc.h"
#شامل "locbased.h"
#شامل "uresimp.h"
#شامل "uassert.h"
#شامل "ubrkimpl.h"
#شامل "utracimp.h"
#شامل "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(ثابت Locale& loc, ثابت نویسه *type, UErrorCode &status)
{
    نویسه fnbuff[256];
    نویسه ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ثابت char16_t* brkfname = nullptr;
    UResourceBundle brk_Rules_پشته;
    UResourceBundle brk_Name_پشته;
    UResourceBundle *brkRules = &brk_Rules_پشته;
    UResourceBundle *brkName  = &brk_Name_پشته;
    RuleBasedBreakIterator *result = nullptr;

    اگر (U_FAILURE(status))
        بازگشت nullptr;

    ures_init_پشته_Object(brkRules);
    ures_init_پشته_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    اگر (U_SUCCESS(status)) {
        brkRules = ures_get_By_کلید_With_Fallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_get_By_کلید_With_Fallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((size_t)size<اندازه(fnbuff));
        اگر (static_cast<size_t>(size) >= اندازه(fnbuff)) {
            size=0;
            اگر (U_SUCCESS(status)) {
                status = U_BUFFER_OVERFLOW_ERROR;
            }
        }

        // Use the string if we found it
        اگر (U_SUCCESS(status) && brkfname) {
            actual.append(ures_getLocaleInternal(brkName, &status), -1, status);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            صحیح len = 0;
            اگر (extStart != nullptr){
                len = static_cast<صحیح>(extStart - brkfname);
                u_UCharsToChars(extStart+1, ext, اندازه(ext)); // nul terminates the buff
                u_UCharsToChars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brkRules);
    ures_close(brkName);

    UDataMemory* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &status);
    اگر (U_FAILURE(status)) {
        ures_close(b);
        بازگشت nullptr;
    }

    // Create a RuleBasedBreakIterator
    result = جدید RuleBasedBreakIterator(file, uprv_strstr(type, "phrase") != nullptr, status);

    // If there is a result, set the valid locale and actual locale, and the kind
    اگر (U_SUCCESS(status) && result != nullptr) {
        result->actualLocale = Locale(actual.data());
        result->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &status));
        result->requestLocale = loc;
    }

    ures_close(b);

    اگر (U_FAILURE(status) && result != nullptr) {  // Sometimes redundant check, but simple
        delete result;
        بازگشت nullptr;
    }

    اگر (result == nullptr) {
        udata_close(file);
        اگر (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    بازگشت result;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(ثابت Locale& کلید, UErrorCode& status)
{
    بازگشت createInstance(کلید, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_سطر_Instance(ثابت Locale& کلید, UErrorCode& status)
{
    بازگشت createInstance(کلید, UBRK_سطر, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ثابت Locale& کلید, UErrorCode& status)
{
    بازگشت createInstance(کلید, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ثابت Locale& کلید, UErrorCode& status)
{
    بازگشت createInstance(کلید, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ثابت Locale& کلید, UErrorCode& status)
{
    بازگشت createInstance(کلید, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ثابت Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    بازگشت Locale::getAvailableLocales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_ریشه()), validLocale(Locale::get_ریشه()), requestLocale(Locale::get_ریشه())
{
}

BreakIterator::BreakIterator(ثابت BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::operator=(ثابت BreakIterator& other) U_LIFETIME_BOUND {
    اگر (این != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    بازگشت *این;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#اگر !UCONFIG_NO_SERVICE

// -------------------------------------

رده ICUBreakIteratorFactory : عمومی ICUResourceBundleFactory {
عمومی:
    مجازی ~ICUBreakIteratorFactory();
محافظتشده:
    مجازی UObject* handleCreate(ثابت Locale& loc, int32_t kind, ثابت ICUService* /*service*/, UErrorCode& status) ثابت override {
        بازگشت BreakIterator::makeInstance(loc, kind, status);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

رده ICUBreakIteratorService : عمومی ICULocaleService {
عمومی:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UErrorCode status = U_ZERO_ERROR;
        registerFactory(جدید ICUBreakIteratorFactory(), status);
    }

    مجازی ~ICUBreakIteratorService();

    مجازی UObject* cloneInstance(UObject* instance) ثابت override {
        بازگشت ((BreakIterator*)instance)->clone();
    }

    مجازی UObject* handleDefault(ثابت ICUService_کلید& کلید, UnicodeString* /*actualID*/, UErrorCode& status) ثابت override {
        Locale_کلید& lkey = static_cast<Locale_کلید&>(const_cast<ICUService_کلید&>(کلید));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        بازگشت BreakIterator::makeInstance(loc, kind, status);
    }

    مجازی UBool isDefault() ثابت override {
        بازگشت شمار_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_END

ایستا icu::UInitOnce gInitOnceBrkiter {};
ایستا icu::ICULocaleService* gService = nullptr;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
ایستا UBool U_CALLCONV breakiterator_cleanup() {
#اگر !UCONFIG_NO_SERVICE
    اگر (gService) {
        delete gService;
        gService = nullptr;
    }
    gInitOnceBrkiter.reset();
#endif
    بازگشت درست;
}
U_CDECL_END
U_NAMESPACE_BEGIN

ایستا تهی U_CALLCONV 
initService() {
    gService = جدید ICUBreakIteratorService();
    ucln_common_registerCleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

ایستا ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    بازگشت gService;
}


// -------------------------------------

ایستا درونخطی UBool
hasService()
{
    بازگشت !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistry_کلید U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, ثابت Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    اگر (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    بازگشت service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_کلید کلید, UErrorCode& status)
{
    اگر (U_SUCCESS(status)) {
        اگر (hasService()) {
            بازگشت gService->unregister(کلید, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    بازگشت نادرست;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    اگر (service == nullptr) {
        بازگشت nullptr;
    }
    بازگشت service->getAvailableLocales();
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ثابت Locale& loc, int32_t kind, UErrorCode& status)
{
    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }

#اگر !UCONFIG_NO_SERVICE
    اگر (hasService()) {
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
        اگر (U_SUCCESS(status) && (result != nullptr) && *actualLoc.getName() != 0) {
            result->actualLocale = actualLoc;
            result->validLocale = actualLoc;
        }
        بازگشت result;
    }
    وگرنه
#endif
    {
        بازگشت makeInstance(loc, kind, status);
    }
}

// -------------------------------------
شمارش { k_کلید_مقدار_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ثابت Locale& loc, int32_t kind, UErrorCode& status)
{

    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }

    BreakIterator *result = nullptr;
    انتخاب (kind) {
    حالت UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            result = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        بشکن;
    حالت UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            result = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        بشکن;
    حالت UBRK_سطر:
        {
            نویسه lb_lw[k_کلید_مقدار_Len_Max];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_سطر);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            auto value = loc.get_Keyword_مقدار<CharString>("lb", kvStatus);
            اگر (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.data());
            }
            // lw=phrase is only supported in Japanese and Korean
            اگر (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_مقدار<CharString>("lw", kvStatus);
                اگر (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.data());
                }
            }
            result = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        بشکن;
    حالت UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            result = BreakIterator::buildInstance(loc, "sentence", status);
#اگر !UCONFIG_NO_FILTERED_BREAK_ITERATION
            نویسه ss_کلید_مقدار[k_کلید_مقدار_Len_Max] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_مقدار("ss", ss_کلید_مقدار, k_کلید_مقدار_Len_Max, kvStatus);
            اگر (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ss_کلید_مقدار,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                اگر (U_SUCCESS(kvStatus)) {
                    result = fbiBuilder->build(result, status);
                    delete fbiBuilder;
                }
            }
#endif
            UTRACE_EXIT_STATUS(status);
        }
        بشکن;
    حالت UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            result = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        بشکن;
    پیشفرض:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    اگر (U_FAILURE(status)) {
        delete result;
        بازگشت nullptr;
    }

    بازگشت result;
}

Locale
BreakIterator::getLocale(ULocDataLocaleType type, UErrorCode& status) ثابت {
    اگر (U_FAILURE(status)) {
        بازگشت Locale::get_ریشه();
    }
    اگر (type == ULOC_REQUESTED_LOCALE) {
        بازگشت requestLocale;
    }
    بازگشت LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

ثابت نویسه*
BreakIterator::getLocaleID(ULocDataLocaleType type, UErrorCode& status) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }
    اگر (type == ULOC_REQUESTED_LOCALE) {
        بازگشت requestLocale.getName();
    }
    بازگشت LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() ثابت {
    بازگشت 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        بازگشت 0;
    }
    اگر (capacity < 1) {
        status = U_BUFFER_OVERFLOW_ERROR;
        بازگشت 1;
    }
    *fillInVec = 0;
    بازگشت 1;
}

BreakIterator::BreakIterator(ثابت Locale& valid, ثابت Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_ریشه()) {
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
