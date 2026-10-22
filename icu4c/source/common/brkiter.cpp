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

#ضمن "unicode/utypes.h"

#𐤀𐤌 !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/rbbi.h"
#ضمن "unicode/brkiter.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/uloc.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/filteredbrk.h"
#ضمن "bytesinkutil.h"
#ضمن "ucln_cmn.h"
#ضمن "cstring.h"
#ضمن "umutex.h"
#ضمن "servloc.h"
#ضمن "locbased.h"
#ضمن "uresimp.h"
#ضمن "uassert.h"
#ضمن "ubrkimpl.h"
#ضمن "utracimp.h"
#ضمن "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(ثابت Locale& loc, ثابت حرف *type, UErrorCode &status)
{
    حرف fnbuff[256];
    حرف ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ثابت char16_t* brkfname = nullptr;
    UResourceBundle brkRulesStack;
    UResourceBundle brkNameStack;
    UResourceBundle *brkRules = &brkRulesStack;
    UResourceBundle *brkName  = &brkNameStack;
    RuleBasedBreakIterator *result = nullptr;

    𐤀𐤌 (U_FAILURE(status))
        أرجع nullptr;

    ures_initStackObject(brkRules);
    ures_initStackObject(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    𐤀𐤌 (U_SUCCESS(status)) {
        brkRules = ures_getByKeyWithFallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_getByKeyWithFallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((size_t)size<حجم(fnbuff));
        𐤀𐤌 (static_cast<size_t>(size) >= حجم(fnbuff)) {
            size=0;
            𐤀𐤌 (U_SUCCESS(status)) {
                status = U_BUFFER_OVERFLOW_ERROR;
            }
        }

        // Use the string if we found it
        𐤀𐤌 (U_SUCCESS(status) && brkfname) {
            actual.append(ures_getLocaleInternal(brkName, &status), -1, status);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            صحيح len = 0;
            𐤀𐤌 (extStart != nullptr){
                len = static_cast<صحيح>(extStart - brkfname);
                u_UCharsToChars(extStart+1, ext, حجم(ext)); // nul terminates the buff
                u_UCharsToChars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brkRules);
    ures_close(brkName);

    UDataMemory* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &status);
    𐤀𐤌 (U_FAILURE(status)) {
        ures_close(b);
        أرجع nullptr;
    }

    // Create a RuleBasedBreakIterator
    result = جديد RuleBasedBreakIterator(file, uprv_strstr(type, "phrase") != nullptr, status);

    // If there is a result, set the valid locale and actual locale, and the kind
    𐤀𐤌 (U_SUCCESS(status) && result != nullptr) {
        result->actualLocale = Locale(actual.data());
        result->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &status));
        result->requestLocale = loc;
    }

    ures_close(b);

    𐤀𐤌 (U_FAILURE(status) && result != nullptr) {  // Sometimes redundant check, but simple
        delete result;
        أرجع nullptr;
    }

    𐤀𐤌 (result == nullptr) {
        udata_close(file);
        𐤀𐤌 (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    أرجع result;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(ثابت Locale& key, UErrorCode& status)
{
    أرجع createInstance(key, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::createLineInstance(ثابت Locale& key, UErrorCode& status)
{
    أرجع createInstance(key, UBRK_LINE, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ثابت Locale& key, UErrorCode& status)
{
    أرجع createInstance(key, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ثابت Locale& key, UErrorCode& status)
{
    أرجع createInstance(key, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ثابت Locale& key, UErrorCode& status)
{
    أرجع createInstance(key, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ثابت Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    أرجع Locale::getAvailableLocales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::getRoot()), validLocale(Locale::getRoot()), requestLocale(Locale::getRoot())
{
}

BreakIterator::BreakIterator(ثابت BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::operator=(ثابت BreakIterator& other) U_LIFETIME_BOUND {
    𐤀𐤌 (هذا != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    أرجع *هذا;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#𐤀𐤌 !UCONFIG_NO_SERVICE

// -------------------------------------

صنف ICUBreakIteratorFactory : عام ICUResourceBundleFactory {
عام:
    virtual ~ICUBreakIteratorFactory();
محمي:
    virtual UObject* handleCreate(ثابت Locale& loc, int32_t kind, ثابت ICUService* /*service*/, UErrorCode& status) ثابت override {
        أرجع BreakIterator::makeInstance(loc, kind, status);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

صنف ICUBreakIteratorService : عام ICULocaleService {
عام:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UErrorCode status = U_ZERO_ERROR;
        registerFactory(جديد ICUBreakIteratorFactory(), status);
    }

    virtual ~ICUBreakIteratorService();

    virtual UObject* cloneInstance(UObject* instance) ثابت override {
        أرجع ((BreakIterator*)instance)->clone();
    }

    virtual UObject* handleDefault(ثابت ICUServiceKey& key, UnicodeString* /*actualID*/, UErrorCode& status) ثابت override {
        LocaleKey& lkey = static_cast<LocaleKey&>(const_cast<ICUServiceKey&>(key));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        أرجع BreakIterator::makeInstance(loc, kind, status);
    }

    virtual UBool isDefault() ثابت override {
        أرجع countFactories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_END

ساكن icu::UInitOnce gInitOnceBrkiter {};
ساكن icu::ICULocaleService* gService = nullptr;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
ساكن UBool U_CALLCONV breakiterator_cleanup() {
#𐤀𐤌 !UCONFIG_NO_SERVICE
    𐤀𐤌 (gService) {
        delete gService;
        gService = nullptr;
    }
    gInitOnceBrkiter.reset();
#endif
    أرجع true;
}
U_CDECL_END
U_NAMESPACE_BEGIN

ساكن فراغ U_CALLCONV 
initService() {
    gService = جديد ICUBreakIteratorService();
    ucln_common_registerCleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

ساكن ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    أرجع gService;
}


// -------------------------------------

ساكن مضمن UBool
hasService()
{
    أرجع !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistryKey U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, ثابت Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    𐤀𐤌 (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    أرجع service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistryKey key, UErrorCode& status)
{
    𐤀𐤌 (U_SUCCESS(status)) {
        𐤀𐤌 (hasService()) {
            أرجع gService->unregister(key, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    أرجع خطأ;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    𐤀𐤌 (service == nullptr) {
        أرجع nullptr;
    }
    أرجع service->getAvailableLocales();
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ثابت Locale& loc, int32_t kind, UErrorCode& status)
{
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع nullptr;
    }

#𐤀𐤌 !UCONFIG_NO_SERVICE
    𐤀𐤌 (hasService()) {
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
        𐤀𐤌 (U_SUCCESS(status) && (result != nullptr) && *actualLoc.getName() != 0) {
            result->actualLocale = actualLoc;
            result->validLocale = actualLoc;
        }
        أرجع result;
    }
    وإلا
#endif
    {
        أرجع makeInstance(loc, kind, status);
    }
}

// -------------------------------------
تعداد { kKeyValueLenMax = 32 };

BreakIterator*
BreakIterator::makeInstance(ثابت Locale& loc, int32_t kind, UErrorCode& status)
{

    𐤀𐤌 (U_FAILURE(status)) {
        أرجع nullptr;
    }

    BreakIterator *result = nullptr;
    بدل (kind) {
    حالة UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            result = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        اكسر;
    حالة UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            result = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        اكسر;
    حالة UBRK_LINE:
        {
            حرف lb_lw[kKeyValueLenMax];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_LINE);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            auto value = loc.getKeywordValue<CharString>("lb", kvStatus);
            𐤀𐤌 (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.data());
            }
            // lw=phrase is only supported in Japanese and Korean
            𐤀𐤌 (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.getKeywordValue<CharString>("lw", kvStatus);
                𐤀𐤌 (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.data());
                }
            }
            result = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        اكسر;
    حالة UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            result = BreakIterator::buildInstance(loc, "sentence", status);
#𐤀𐤌 !UCONFIG_NO_FILTERED_BREAK_ITERATION
            حرف ssKeyValue[kKeyValueLenMax] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.getKeywordValue("ss", ssKeyValue, kKeyValueLenMax, kvStatus);
            𐤀𐤌 (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ssKeyValue,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                𐤀𐤌 (U_SUCCESS(kvStatus)) {
                    result = fbiBuilder->build(result, status);
                    delete fbiBuilder;
                }
            }
#endif
            UTRACE_EXIT_STATUS(status);
        }
        اكسر;
    حالة UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            result = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        اكسر;
    افتراضي:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    𐤀𐤌 (U_FAILURE(status)) {
        delete result;
        أرجع nullptr;
    }

    أرجع result;
}

Locale
BreakIterator::getLocale(ULocDataLocaleType type, UErrorCode& status) ثابت {
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع Locale::getRoot();
    }
    𐤀𐤌 (type == ULOC_REQUESTED_LOCALE) {
        أرجع requestLocale;
    }
    أرجع LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

ثابت حرف*
BreakIterator::getLocaleID(ULocDataLocaleType type, UErrorCode& status) ثابت U_LIFETIME_BOUND {
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع nullptr;
    }
    𐤀𐤌 (type == ULOC_REQUESTED_LOCALE) {
        أرجع requestLocale.getName();
    }
    أرجع LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() ثابت {
    أرجع 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع 0;
    }
    𐤀𐤌 (capacity < 1) {
        status = U_BUFFER_OVERFLOW_ERROR;
        أرجع 1;
    }
    *fillInVec = 0;
    أرجع 1;
}

BreakIterator::BreakIterator(ثابت Locale& valid, ثابت Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::getRoot()) {
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
