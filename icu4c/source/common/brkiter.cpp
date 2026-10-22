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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"
#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/filteredbrk.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "servloc.h"
#ಅನ್ತರ್ಭಾವ "locbased.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ubrkimpl.h"
#ಅನ್ತರ್ಭಾವ "utracimp.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(ಧ್ರುವ Locale& loc, ಧ್ರುವ ವರ್ಣ *type, UErrorCode &status)
{
    ವರ್ಣ fnbuff[256];
    ವರ್ಣ ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ಧ್ರುವ char16_t* brkfname = nullptr;
    UResourceBundle brk_Rules_ರಾಶಿ;
    UResourceBundle brk_Name_ರಾಶಿ;
    UResourceBundle *brkRules = &brk_Rules_ರಾಶಿ;
    UResourceBundle *brkName  = &brk_Name_ರಾಶಿ;
    RuleBasedBreakIterator *result = nullptr;

    ಯದಿ (U_FAILURE(status))
        ಪ್ರತ್ಯಾಗಮ nullptr;

    ures_init_ರಾಶಿ_Object(brkRules);
    ures_init_ರಾಶಿ_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    ಯದಿ (U_SUCCESS(status)) {
        brkRules = ures_get_By_ಕೀಲಿ_With_Fallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_get_By_ಕೀಲಿ_With_Fallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((size_t)size<ಪರಿಮಾಣ(fnbuff));
        ಯದಿ (static_cast<size_t>(size) >= ಪರಿಮಾಣ(fnbuff)) {
            size=0;
            ಯದಿ (U_SUCCESS(status)) {
                status = U_BUFFER_OVERFLOW_ERROR;
            }
        }

        // Use the string if we found it
        ಯದಿ (U_SUCCESS(status) && brkfname) {
            actual.append(ures_getLocaleInternal(brkName, &status), -1, status);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            ಪೂರ್ಣಾಙ್ಕ len = 0;
            ಯದಿ (extStart != nullptr){
                len = static_cast<ಪೂರ್ಣಾಙ್ಕ>(extStart - brkfname);
                u_UCharsToChars(extStart+1, ext, ಪರಿಮಾಣ(ext)); // nul terminates the buff
                u_UCharsToChars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brkRules);
    ures_close(brkName);

    UDataMemory* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &status);
    ಯದಿ (U_FAILURE(status)) {
        ures_close(b);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // Create a RuleBasedBreakIterator
    result = ನೂತನ RuleBasedBreakIterator(file, uprv_strstr(type, "phrase") != nullptr, status);

    // If there is a result, set the valid locale and actual locale, and the kind
    ಯದಿ (U_SUCCESS(status) && result != nullptr) {
        result->actualLocale = Locale(actual.data());
        result->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &status));
        result->requestLocale = loc;
    }

    ures_close(b);

    ಯದಿ (U_FAILURE(status) && result != nullptr) {  // Sometimes redundant check, but simple
        delete result;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ (result == nullptr) {
        udata_close(file);
        ಯದಿ (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(ಧ್ರುವ Locale& ಕೀಲಿ, UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ createInstance(ಕೀಲಿ, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_ಸಾಲು_Instance(ಧ್ರುವ Locale& ಕೀಲಿ, UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ createInstance(ಕೀಲಿ, UBRK_ಸಾಲು, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ಧ್ರುವ Locale& ಕೀಲಿ, UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ createInstance(ಕೀಲಿ, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ಧ್ರುವ Locale& ಕೀಲಿ, UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ createInstance(ಕೀಲಿ, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ಧ್ರುವ Locale& ಕೀಲಿ, UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ createInstance(ಕೀಲಿ, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ಧ್ರುವ Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    ಪ್ರತ್ಯಾಗಮ Locale::getAvailableLocales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_ಬೇರು()), validLocale(Locale::get_ಬೇರು()), requestLocale(Locale::get_ಬೇರು())
{
}

BreakIterator::BreakIterator(ಧ್ರುವ BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::operator=(ಧ್ರುವ BreakIterator& other) U_LIFETIME_BOUND {
    ಯದಿ (ಏತತ್ != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#ಯದಿ !UCONFIG_NO_SERVICE

// -------------------------------------

ವರ್ಗ ICUBreakIteratorFactory : ಸಾರ್ವಜನಿಕ ICUResourceBundleFactory {
ಸಾರ್ವಜನಿಕ:
    ಆಭಾಸ ~ICUBreakIteratorFactory();
ರಕ್ಷಿತ:
    ಆಭಾಸ UObject* handleCreate(ಧ್ರುವ Locale& loc, int32_t kind, ಧ್ರುವ ICUService* /*service*/, UErrorCode& status) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ BreakIterator::makeInstance(loc, kind, status);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

ವರ್ಗ ICUBreakIteratorService : ಸಾರ್ವಜನಿಕ ICULocaleService {
ಸಾರ್ವಜನಿಕ:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UErrorCode status = U_ZERO_ERROR;
        registerFactory(ನೂತನ ICUBreakIteratorFactory(), status);
    }

    ಆಭಾಸ ~ICUBreakIteratorService();

    ಆಭಾಸ UObject* cloneInstance(UObject* instance) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)instance)->clone();
    }

    ಆಭಾಸ UObject* handleDefault(ಧ್ರುವ ICUService_ಕೀಲಿ& ಕೀಲಿ, UnicodeString* /*actualID*/, UErrorCode& status) ಧ್ರುವ override {
        Locale_ಕೀಲಿ& lkey = static_cast<Locale_ಕೀಲಿ&>(const_cast<ICUService_ಕೀಲಿ&>(ಕೀಲಿ));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        ಪ್ರತ್ಯಾಗಮ BreakIterator::makeInstance(loc, kind, status);
    }

    ಆಭಾಸ UBool isDefault() ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_END

ಸ್ಥಿರ icu::UInitOnce gInitOnceBrkiter {};
ಸ್ಥಿರ icu::ICULocaleService* gService = nullptr;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV breakiterator_cleanup() {
#ಯದಿ !UCONFIG_NO_SERVICE
    ಯದಿ (gService) {
        delete gService;
        gService = nullptr;
    }
    gInitOnceBrkiter.reset();
#endif
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}
U_CDECL_END
U_NAMESPACE_BEGIN

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV 
initService() {
    gService = ನೂತನ ICUBreakIteratorService();
    ucln_common_registerCleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

ಸ್ಥಿರ ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    ಪ್ರತ್ಯಾಗಮ gService;
}


// -------------------------------------

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ UBool
hasService()
{
    ಪ್ರತ್ಯಾಗಮ !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistry_ಕೀಲಿ U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, ಧ್ರುವ Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    ಯದಿ (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_ಕೀಲಿ ಕೀಲಿ, UErrorCode& status)
{
    ಯದಿ (U_SUCCESS(status)) {
        ಯದಿ (hasService()) {
            ಪ್ರತ್ಯಾಗಮ gService->unregister(ಕೀಲಿ, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    ಯದಿ (service == nullptr) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ service->getAvailableLocales();
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ಧ್ರುವ Locale& loc, int32_t kind, UErrorCode& status)
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

#ಯದಿ !UCONFIG_NO_SERVICE
    ಯದಿ (hasService()) {
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
        ಯದಿ (U_SUCCESS(status) && (result != nullptr) && *actualLoc.getName() != 0) {
            result->actualLocale = actualLoc;
            result->validLocale = actualLoc;
        }
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಅನ್ಯಥಾ
#endif
    {
        ಪ್ರತ್ಯಾಗಮ makeInstance(loc, kind, status);
    }
}

// -------------------------------------
ಗಣನಾ { k_ಕೀಲಿ_ಮೌಲ್ಯ_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ಧ್ರುವ Locale& loc, int32_t kind, UErrorCode& status)
{

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    BreakIterator *result = nullptr;
    ವಿಕಲ್ಪ (kind) {
    ಪಕ್ಷ UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            result = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            result = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ UBRK_ಸಾಲು:
        {
            ವರ್ಣ lb_lw[k_ಕೀಲಿ_ಮೌಲ್ಯ_Len_Max];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_ಸಾಲು);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            auto value = loc.get_Keyword_ಮೌಲ್ಯ<CharString>("lb", kvStatus);
            ಯದಿ (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.data());
            }
            // lw=phrase is only supported in Japanese and Korean
            ಯದಿ (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_ಮೌಲ್ಯ<CharString>("lw", kvStatus);
                ಯದಿ (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.data());
                }
            }
            result = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            result = BreakIterator::buildInstance(loc, "sentence", status);
#ಯದಿ !UCONFIG_NO_FILTERED_BREAK_ITERATION
            ವರ್ಣ ss_ಕೀಲಿ_ಮೌಲ್ಯ[k_ಕೀಲಿ_ಮೌಲ್ಯ_Len_Max] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_ಮೌಲ್ಯ("ss", ss_ಕೀಲಿ_ಮೌಲ್ಯ, k_ಕೀಲಿ_ಮೌಲ್ಯ_Len_Max, kvStatus);
            ಯದಿ (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ss_ಕೀಲಿ_ಮೌಲ್ಯ,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                ಯದಿ (U_SUCCESS(kvStatus)) {
                    result = fbiBuilder->build(result, status);
                    delete fbiBuilder;
                }
            }
#endif
            UTRACE_EXIT_STATUS(status);
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            result = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    ಯದಿ (U_FAILURE(status)) {
        delete result;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

Locale
BreakIterator::getLocale(ULocDataLocaleType type, UErrorCode& status) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ Locale::get_ಬೇರು();
    }
    ಯದಿ (type == ULOC_REQUESTED_LOCALE) {
        ಪ್ರತ್ಯಾಗಮ requestLocale;
    }
    ಪ್ರತ್ಯಾಗಮ LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

ಧ್ರುವ ವರ್ಣ*
BreakIterator::getLocaleID(ULocDataLocaleType type, UErrorCode& status) ಧ್ರುವ U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (type == ULOC_REQUESTED_LOCALE) {
        ಪ್ರತ್ಯಾಗಮ requestLocale.getName();
    }
    ಪ್ರತ್ಯಾಗಮ LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (capacity < 1) {
        status = U_BUFFER_OVERFLOW_ERROR;
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    *fillInVec = 0;
    ಪ್ರತ್ಯಾಗಮ 1;
}

BreakIterator::BreakIterator(ಧ್ರುವ Locale& valid, ಧ್ರುವ Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_ಬೇರು()) {
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
