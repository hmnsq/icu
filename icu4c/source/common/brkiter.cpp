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
BreakIterator::buildInstance(ध्रुव Locale& loc, ध्रुव वर्ण *type, UErrorCode &status)
{
    वर्ण fnbuff[256];
    वर्ण ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ध्रुव char16_t* brkfname = nullptr;
    UResourceBundle brk_Rules_चळत;
    UResourceBundle brk_Name_चळत;
    UResourceBundle *brkRules = &brk_Rules_चळत;
    UResourceBundle *brkName  = &brk_Name_चळत;
    RuleBasedBreakIterator *result = nullptr;

    यदि (U_FAILURE(status))
        प्रत्यागम nullptr;

    ures_init_चळत_Object(brkRules);
    ures_init_चळत_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &status);

    // Get the "boundaries" array.
    यदि (U_SUCCESS(status)) {
        brkRules = ures_get_By_किल्ली_With_Fallback(b, "boundaries", brkRules, &status);
        // Get the string object naming the rules file
        brkName = ures_get_By_किल्ली_With_Fallback(brkRules, type, brkName, &status);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &status);
        U_ASSERT((size_t)size<परिमाण(fnbuff));
        यदि (static_cast<size_t>(size) >= परिमाण(fnbuff)) {
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
        प्रत्यागम nullptr;
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
        प्रत्यागम nullptr;
    }

    यदि (result == nullptr) {
        udata_close(file);
        यदि (U_SUCCESS(status)) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
    }

    प्रत्यागम result;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(ध्रुव Locale& किल्ली, UErrorCode& status)
{
    प्रत्यागम createInstance(किल्ली, UBRK_WORD, status);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_ओळ_Instance(ध्रुव Locale& किल्ली, UErrorCode& status)
{
    प्रत्यागम createInstance(किल्ली, UBRK_ओळ, status);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ध्रुव Locale& किल्ली, UErrorCode& status)
{
    प्रत्यागम createInstance(किल्ली, UBRK_CHARACTER, status);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ध्रुव Locale& किल्ली, UErrorCode& status)
{
    प्रत्यागम createInstance(किल्ली, UBRK_SENTENCE, status);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ध्रुव Locale& किल्ली, UErrorCode& status)
{
    प्रत्यागम createInstance(किल्ली, UBRK_TITLE, status);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ध्रुव Locale* U_EXPORT2
BreakIterator::getAvailableLocales(int32_t& count)
{
    प्रत्यागम Locale::getAvailableLocales(count);
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

BreakIterator::BreakIterator(ध्रुव BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::operator=(ध्रुव BreakIterator& other) U_LIFETIME_BOUND {
    यदि (एतत् != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    प्रत्यागम *एतत्;
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
    आभास UObject* handleCreate(ध्रुव Locale& loc, int32_t kind, ध्रुव ICUService* /*service*/, UErrorCode& status) ध्रुव override {
        प्रत्यागम BreakIterator::makeInstance(loc, kind, status);
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

    आभास UObject* cloneInstance(UObject* instance) ध्रुव override {
        प्रत्यागम ((BreakIterator*)instance)->clone();
    }

    आभास UObject* handleDefault(ध्रुव ICUService_किल्ली& किल्ली, UnicodeString* /*actualID*/, UErrorCode& status) ध्रुव override {
        Locale_किल्ली& lkey = static_cast<Locale_किल्ली&>(const_cast<ICUService_किल्ली&>(किल्ली));
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);
        प्रत्यागम BreakIterator::makeInstance(loc, kind, status);
    }

    आभास UBool isDefault() ध्रुव override {
        प्रत्यागम मोजणी_Factories() == 1;
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
    प्रत्यागम सत्य;
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
    प्रत्यागम gService;
}


// -------------------------------------

स्थिर अन्तःस्थ UBool
hasService()
{
    प्रत्यागम !gInitOnceBrkiter.isReset() && getService() != nullptr;
}

// -------------------------------------

URegistry_किल्ली U_EXPORT2
BreakIterator::registerInstance(BreakIterator* toAdopt, ध्रुव Locale& locale, UBreakIteratorType kind, UErrorCode& status)
{
    ICULocaleService *service = getService();
    यदि (service == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    }
    प्रत्यागम service->registerInstance(toAdopt, locale, kind, status);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_किल्ली किल्ली, UErrorCode& status)
{
    यदि (U_SUCCESS(status)) {
        यदि (hasService()) {
            प्रत्यागम gService->unregister(किल्ली, status);
        }
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    प्रत्यागम असत्य;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::getAvailableLocales()
{
    ICULocaleService *service = getService();
    यदि (service == nullptr) {
        प्रत्यागम nullptr;
    }
    प्रत्यागम service->getAvailableLocales();
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ध्रुव Locale& loc, int32_t kind, UErrorCode& status)
{
    यदि (U_FAILURE(status)) {
        प्रत्यागम nullptr;
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
        प्रत्यागम result;
    }
    अन्यथा
#endif
    {
        प्रत्यागम makeInstance(loc, kind, status);
    }
}

// -------------------------------------
गणना { k_किल्ली_मूल्य_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ध्रुव Locale& loc, int32_t kind, UErrorCode& status)
{

    यदि (U_FAILURE(status)) {
        प्रत्यागम nullptr;
    }

    BreakIterator *result = nullptr;
    विकल्प (kind) {
    पक्ष UBRK_CHARACTER:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_CHARACTER);
            result = BreakIterator::buildInstance(loc, "grapheme", status);
            UTRACE_EXIT_STATUS(status);
        }
        भङ्ग;
    पक्ष UBRK_WORD:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_WORD);
            result = BreakIterator::buildInstance(loc, "word", status);
            UTRACE_EXIT_STATUS(status);
        }
        भङ्ग;
    पक्ष UBRK_ओळ:
        {
            वर्ण lb_lw[k_किल्ली_मूल्य_Len_Max];
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_ओळ);
            uprv_strcpy(lb_lw, "line");
            UErrorCode kvStatus = U_ZERO_ERROR;
            auto value = loc.get_Keyword_मूल्य<CharString>("lb", kvStatus);
            यदि (U_SUCCESS(kvStatus) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.data());
            }
            // lw=phrase is only supported in Japanese and Korean
            यदि (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_मूल्य<CharString>("lw", kvStatus);
                यदि (U_SUCCESS(kvStatus) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.data());
                }
            }
            result = BreakIterator::buildInstance(loc, lb_lw, status);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_STATUS(status);
        }
        भङ्ग;
    पक्ष UBRK_SENTENCE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_SENTENCE);
            result = BreakIterator::buildInstance(loc, "sentence", status);
#यदि !UCONFIG_NO_FILTERED_BREAK_ITERATION
            वर्ण ss_किल्ली_मूल्य[k_किल्ली_मूल्य_Len_Max] = {0};
            UErrorCode kvStatus = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_मूल्य("ss", ss_किल्ली_मूल्य, k_किल्ली_मूल्य_Len_Max, kvStatus);
            यदि (U_SUCCESS(kvStatus) && kLen > 0 && uprv_strcmp(ss_किल्ली_मूल्य,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kvStatus);
                यदि (U_SUCCESS(kvStatus)) {
                    result = fbiBuilder->build(result, status);
                    delete fbiBuilder;
                }
            }
#endif
            UTRACE_EXIT_STATUS(status);
        }
        भङ्ग;
    पक्ष UBRK_TITLE:
        {
            UTRACE_ENTRY(UTRACE_UBRK_CREATE_TITLE);
            result = BreakIterator::buildInstance(loc, "title", status);
            UTRACE_EXIT_STATUS(status);
        }
        भङ्ग;
    स्वभाव:
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }

    यदि (U_FAILURE(status)) {
        delete result;
        प्रत्यागम nullptr;
    }

    प्रत्यागम result;
}

Locale
BreakIterator::getLocale(ULocDataLocaleType type, UErrorCode& status) ध्रुव {
    यदि (U_FAILURE(status)) {
        प्रत्यागम Locale::get_मूळ();
    }
    यदि (type == ULOC_REQUESTED_LOCALE) {
        प्रत्यागम requestLocale;
    }
    प्रत्यागम LocaleBased::getLocale(validLocale, actualLocale, type, status);
}

ध्रुव वर्ण*
BreakIterator::getLocaleID(ULocDataLocaleType type, UErrorCode& status) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(status)) {
        प्रत्यागम nullptr;
    }
    यदि (type == ULOC_REQUESTED_LOCALE) {
        प्रत्यागम requestLocale.getName();
    }
    प्रत्यागम LocaleBased::getLocaleID(validLocale, actualLocale, type, status);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatus() ध्रुव {
    प्रत्यागम 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::getRuleStatusVec(int32_t *fillInVec, int32_t capacity, UErrorCode &status) {
    यदि (U_FAILURE(status)) {
        प्रत्यागम 0;
    }
    यदि (capacity < 1) {
        status = U_BUFFER_OVERFLOW_ERROR;
        प्रत्यागम 1;
    }
    *fillInVec = 0;
    प्रत्यागम 1;
}

BreakIterator::BreakIterator(ध्रुव Locale& valid, ध्रुव Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_मूळ()) {
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
