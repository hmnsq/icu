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

#यदि !UCONFIG_NO_BREAK_ITERATION

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
BreakIterator::buildInstance(अचर Locale& loc, अचर वर्ण *type, UErrorCode &status)
{
    वर्ण fnbuff[256];
    वर्ण ext[4]={'\0'};
    CharString actual;
    int32_t size;
    अचर char16_t* brkfname = nullptr;
    UResourceBundle brk_Rules_थाक;
    UResourceBundle brk_Name_थाक;
    UResourceBundle *brkRules = &brk_Rules_थाक;
    UResourceBundle *brkName  = &brk_Name_थाक;
    RuleBasedBreakIterator *result = nullptr;

    यदि (U_FAILURE(status))
        फर्काऊ nullptr;

    ures_init_थाक_Object(brkRules);
    ures_init_थाक_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    यदि (U_SUCCESS(status)) {
        brkRules = ures_get_By_साँचो_With_Fallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_get_By_साँचो_With_Fallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((माप_प्रकार)size<परिमाण(fnbuff));
        यदि (static_cast<माप_प्रकार>(size) >= परिमाण(fnbuff)) {
            size=0;
            यदि (U_SUCCESS(status)) {
                status = U_BUFFER_OVERFLOW_ERROR;
            }
        }

        // Use the string if we found it
        यदि (U_SUCCESS(status) && brkfname) {
            actual.append(ures_getLocaleInternal(brkName, &status), -1, status);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            पूर्णाङ्क len = 0;
            यदि (extStart != nullptr){
                len = static_cast<पूर्णाङ्क>(extStart - brkfname);
                u_UCharsToChars(extStart+1, ext, परिमाण(ext)); // nul terminates the buff
                u_UCharsToChars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brkRules);
    ures_close(brkName);

    UDataMemory* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &status);
    यदि (U_FAILURE(status)) {
        ures_close(b);
        फर्काऊ nullptr;
    }

    // Create a RuleBasedBreakIterator
    result = नूतन RuleBasedBreakIterator(file, uprv_strstr(type, "phrase") != nullptr, status);

    // If there is a result, set the valid locale and actual locale, and the kind
    यदि (U_SUCCESS(status) && result != nullptr) {
        result->actualLocale = Locale(actual.data());
        result->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &status));
        result->requestLocale = loc;
    }

    ures_close(b);

    यदि (U_FAILURE(status) && result != nullptr) {  // Sometimes redundant check, but simple
        delete result;
        फर्काऊ nullptr;
    }

    यदि (result == nullptr) {
        udata_close(file);
        यदि (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    फर्काऊ result;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(अचर Locale& साँचो, UErrorCode& status)
{
    फर्काऊ createInstance(साँचो, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_पङ्क्ति_Instance(अचर Locale& साँचो, UErrorCode& status)
{
    फर्काऊ createInstance(साँचो, UBRK_पङ्क्ति, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(अचर Locale& साँचो, UErrorCode& status)
{
    फर्काऊ createInstance(साँचो, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(अचर Locale& साँचो, UErrorCode& status)
{
    फर्काऊ createInstance(साँचो, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(अचर Locale& साँचो, UErrorCode& status)
{
    फर्काऊ createInstance(साँचो, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
अचर Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    फर्काऊ Locale::getAvailableLocales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_जरा()), validLocale(Locale::get_जरा()), requestLocale(Locale::get_जरा())
{
}

BreakIterator::BreakIterator(अचर BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::चालक=(अचर BreakIterator& other) U_LIFETIME_BOUND {
    यदि (एतत् != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    फर्काऊ *एतत्;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#यदि !UCONFIG_NO_SERVICE

// -------------------------------------

वर्ग ICUBreakIteratorFactory : सार्वजनिक ICUResourceBundleFactory {
सार्वजनिक:
    आभास ~ICUBreakIteratorFactory();
रक्षित:
    आभास UObject* handleCreate(अचर Locale& loc, int32_t kind, अचर ICUService* /*service*/, UErrorCode& status) अचर override {
        फर्काऊ BreakIterator::makeInstance(loc, kind, status);
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
        registerFactory(नूतन ICUBreakIteratorFactory(), status);
    }

    आभास ~ICUBreakIteratorService();

    आभास UObject* cloneInstance(UObject* instance) अचर override {
        फर्काऊ ((BreakIterator*)instance)->clone();
    }

    आभास UObject* handleDefault(अचर ICUService_साँचो& साँचो, UnicodeString* /*actualID*/, UErrorCode& status) अचर override {
        Locale_साँचो& lkey = static_cast<Locale_साँचो&>(const_cast<ICUService_साँचो&>(साँचो));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        फर्काऊ BreakIterator::makeInstance(loc, kind, status);
    }

    आभास UBool isDefault() अचर override {
        फर्काऊ गन्ती_Factories() == 1;
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
#यदि !UCONFIG_NO_SERVICE
    यदि (gService) {
        delete gService;
        gService = nullptr;
    }
    gInitOnceBrkiter.reset();
#endif
    फर्काऊ सत्य;
}
U_CDECL_END
U_NAMESPACE_BEGIN

स्थिर शून्य U_CALLCONV 
initService() {
    gService = नूतन ICUBreakIteratorService();
    ucln_common_registerCleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

स्थिर ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    फर्काऊ gService;
}


// -------------------------------------

स्थिर अन्तःस्थ UBool
hasService()
{
    फर्काऊ !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistry_साँचो U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, अचर Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    यदि (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    फर्काऊ service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_साँचो साँचो, UErrorCode& status)
{
    यदि (U_SUCCESS(status)) {
        यदि (hasService()) {
            फर्काऊ gService->unregister(साँचो, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    फर्काऊ असत्य;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    यदि (service == nullptr) {
        फर्काऊ nullptr;
    }
    फर्काऊ service->getAvailableLocales();
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(अचर Locale& loc, int32_t kind, UErrorCode& status)
{
    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }

#यदि !UCONFIG_NO_SERVICE
    यदि (hasService()) {
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
        यदि (U_SUCCESS(status) && (result != nullptr) && *actualLoc.getName() != 0) {
            result->actualLocale = actualLoc;
            result->validLocale = actualLoc;
        }
        फर्काऊ result;
    }
    अन्यथा
#endif
    {
        फर्काऊ makeInstance(loc, kind, status);
    }
}

// -------------------------------------
गणना { k_साँचो_मान_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(अचर Locale& loc, int32_t kind, UErrorCode& status)
{

    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }

    BreakIterator *result = nullptr;
    विकल्प (kind) {
    पक्ष UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            result = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        रोक;
    पक्ष UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            result = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        रोक;
    पक्ष UBRK_पङ्क्ति:
        {
            वर्ण lb_lw[k_साँचो_मान_Len_Max];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_पङ्क्ति);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            स्वतः value = loc.get_Keyword_मान<CharString>("lb", kvStatus);
            यदि (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.data());
            }
            // lw=phrase is only supported in Japanese and Korean
            यदि (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_मान<CharString>("lw", kvStatus);
                यदि (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.data());
                }
            }
            result = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        रोक;
    पक्ष UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            result = BreakIterator::buildInstance(loc, "sentence", status);
#यदि !UCONFIG_NO_FILTERED_BREAK_ITERATION
            वर्ण ss_साँचो_मान[k_साँचो_मान_Len_Max] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_मान("ss", ss_साँचो_मान, k_साँचो_मान_Len_Max, kvStatus);
            यदि (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ss_साँचो_मान,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                यदि (U_SUCCESS(kvStatus)) {
                    result = fbiBuilder->build(result, status);
                    delete fbiBuilder;
                }
            }
#endif
            UTRACE_EXIT_STATUS(status);
        }
        रोक;
    पक्ष UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            result = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        रोक;
    स्वभाव:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    यदि (U_FAILURE(status)) {
        delete result;
        फर्काऊ nullptr;
    }

    फर्काऊ result;
}

Locale
BreakIterator::getLocale(ULocDataLocaleType type, UErrorCode& status) अचर {
    यदि (U_FAILURE(status)) {
        फर्काऊ Locale::get_जरा();
    }
    यदि (type == ULOC_REQUESTED_LOCALE) {
        फर्काऊ requestLocale;
    }
    फर्काऊ LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

अचर वर्ण*
BreakIterator::getLocaleID(ULocDataLocaleType type, UErrorCode& status) अचर U_LIFETIME_BOUND {
    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }
    यदि (type == ULOC_REQUESTED_LOCALE) {
        फर्काऊ requestLocale.getName();
    }
    फर्काऊ LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() अचर {
    फर्काऊ 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ 0;
    }
    यदि (capacity < 1) {
        status = U_BUFFER_OVERFLOW_ERROR;
        फर्काऊ 1;
    }
    *fillInVec = 0;
    फर्काऊ 1;
}

BreakIterator::BreakIterator(अचर Locale& valid, अचर Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_जरा()) {
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
