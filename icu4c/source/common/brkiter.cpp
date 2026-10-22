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
BreakIterator::buildInstance(ध्रुव Locale& loc, ध्रुव वर्ण *type, UError_संहिता &स्थितिः)
{
    वर्ण fnbuff[256];
    वर्ण ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ध्रुव char16_t* brkfname = शून्यसूचकः;
    UResourceBundle brk_नियमाः_राशिः;
    UResourceBundle brk_Name_राशिः;
    UResourceBundle *brk_नियमाः = &brk_नियमाः_राशिः;
    UResourceBundle *brkName  = &brk_Name_राशिः;
    नियमः_Based_Break_Iterator *परिणाम = शून्यसूचकः;

    यदि (U_FAILURE(स्थितिः))
        प्रत्यागम शून्यसूचकः;

    ures_init_राशिः_Object(brk_नियमाः);
    ures_init_राशिः_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &स्थितिः);

    // Get the "boundaries" array.
    यदि (U_SUCCESS(स्थितिः)) {
        brk_नियमाः = ures_get_By_कुञ्चिका_With_Fallback(b, "boundaries", brk_नियमाः, &स्थितिः);
        // Get the string object naming the rules file
        brkName = ures_get_By_कुञ्चिका_With_Fallback(brk_नियमाः, type, brkName, &स्थितिः);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &स्थितिः);
        U_ASSERT((size_t)size<परिमाण(fnbuff));
        यदि (static_cast<size_t>(size) >= परिमाण(fnbuff)) {
            size=0;
            यदि (U_SUCCESS(स्थितिः)) {
                स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
            }
        }

        // Use the string if we found it
        यदि (U_SUCCESS(स्थितिः) && brkfname) {
            actual.append(ures_get_Locale_आन्तरिकम्(brkName, &स्थितिः), -1, स्थितिः);

            char16_t* extStart=u_strchr(brkfname, 0x002e);
            पूर्णाङ्क len = 0;
            यदि (extStart != शून्यसूचकः){
                len = static_cast<पूर्णाङ्क>(extStart - brkfname);
                u_UChars_गन्तव्यम्_Chars(extStart+1, ext, परिमाण(ext)); // nul terminates the buff
                u_UChars_गन्तव्यम्_Chars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brk_नियमाः);
    ures_close(brkName);

    UData_स्मृतिः* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        ures_close(b);
        प्रत्यागम शून्यसूचकः;
    }

    // Create a RuleBasedBreakIterator
    परिणाम = नूतन नियमः_Based_Break_Iterator(file, uprv_strstr(type, "phrase") != शून्यसूचकः, स्थितिः);

    // If there is a result, set the valid locale and actual locale, and the kind
    यदि (U_SUCCESS(स्थितिः) && परिणाम != शून्यसूचकः) {
        परिणाम->actualLocale = Locale(actual.दत्त());
        परिणाम->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &स्थितिः));
        परिणाम->requestLocale = loc;
    }

    ures_close(b);

    यदि (U_FAILURE(स्थितिः) && परिणाम != शून्यसूचकः) {  // Sometimes redundant check, but simple
        विलोपय परिणाम;
        प्रत्यागम शून्यसूचकः;
    }

    यदि (परिणाम == शून्यसूचकः) {
        udata_close(file);
        यदि (U_SUCCESS(स्थितिः)) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        }
    }

    प्रत्यागम परिणाम;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::createWordInstance(ध्रुव Locale& कुञ्चिका, UError_संहिता& स्थितिः)
{
    प्रत्यागम createInstance(कुञ्चिका, UBRK_WORD, स्थितिः);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_पङ्क्तिः_Instance(ध्रुव Locale& कुञ्चिका, UError_संहिता& स्थितिः)
{
    प्रत्यागम createInstance(कुञ्चिका, UBRK_पङ्क्तिः, स्थितिः);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ध्रुव Locale& कुञ्चिका, UError_संहिता& स्थितिः)
{
    प्रत्यागम createInstance(कुञ्चिका, UBRK_CHARACTER, स्थितिः);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ध्रुव Locale& कुञ्चिका, UError_संहिता& स्थितिः)
{
    प्रत्यागम createInstance(कुञ्चिका, UBRK_SENTENCE, स्थितिः);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ध्रुव Locale& कुञ्चिका, UError_संहिता& स्थितिः)
{
    प्रत्यागम createInstance(कुञ्चिका, UBRK_TITLE, स्थितिः);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ध्रुव Locale* U_EXPORT2
BreakIterator::get_उपलब्धम्_Locales(int32_t& count)
{
    प्रत्यागम Locale::get_उपलब्धम्_Locales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_मूलम्()), validLocale(Locale::get_मूलम्()), requestLocale(Locale::get_मूलम्())
{
}

BreakIterator::BreakIterator(ध्रुव BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::संक्रियकः=(ध्रुव BreakIterator& other) U_LIFETIME_BOUND {
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
    आभास UObject* handleCreate(ध्रुव Locale& loc, int32_t विधा, ध्रुव ICUService* /*service*/, UError_संहिता& स्थितिः) ध्रुव override {
        प्रत्यागम BreakIterator::makeInstance(loc, विधा, स्थितिः);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

वर्ग ICUBreakIteratorService : सार्वजनिक ICULocaleService {
सार्वजनिक:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        पञ्जिकाकोशः_Factory(नूतन ICUBreakIteratorFactory(), स्थितिः);
    }

    आभास ~ICUBreakIteratorService();

    आभास UObject* cloneInstance(UObject* instance) ध्रुव override {
        प्रत्यागम ((BreakIterator*)instance)->clone();
    }

    आभास UObject* handleDefault(ध्रुव ICUService_कुञ्चिका& कुञ्चिका, UnicodeString* /*actualID*/, UError_संहिता& स्थितिः) ध्रुव override {
        Locale_कुञ्चिका& lkey = static_cast<Locale_कुञ्चिका&>(const_cast<ICUService_कुञ्चिका&>(कुञ्चिका));
        int32_t विधा = lkey.विधा();
        Locale loc;
        lkey.currentLocale(loc);
        प्रत्यागम BreakIterator::makeInstance(loc, विधा, स्थितिः);
    }

    आभास UBool isDefault() ध्रुव override {
        प्रत्यागम गणनम्_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_अन्तः

स्थिर icu::UInitOnce gInitOnceBrkiter {};
स्थिर icu::ICULocaleService* gService = शून्यसूचकः;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
स्थिर UBool U_CALLCONV breakiterator_cleanup() {
#यदि !UCONFIG_NO_SERVICE
    यदि (gService) {
        विलोपय gService;
        gService = शून्यसूचकः;
    }
    gInitOnceBrkiter.reset();
#यदि_अन्तः
    प्रत्यागम सत्य;
}
U_CDECL_अन्तः
U_NAMESPACE_BEGIN

स्थिर शून्य U_CALLCONV 
initService() {
    gService = नूतन ICUBreakIteratorService();
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
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
    प्रत्यागम !gInitOnceBrkiter.isReset() && getService() != शून्यसूचकः;
}

// -------------------------------------

URegistry_कुञ्चिका U_EXPORT2
BreakIterator::पञ्जिकाकोशः_Instance(BreakIterator* गन्तव्यम्_Adopt, ध्रुव Locale& locale, UBreakIteratorType विधा, UError_संहिता& स्थितिः)
{
    ICULocaleService *service = getService();
    यदि (service == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम service->पञ्जिकाकोशः_Instance(गन्तव्यम्_Adopt, locale, विधा, स्थितिः);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_कुञ्चिका कुञ्चिका, UError_संहिता& स्थितिः)
{
    यदि (U_SUCCESS(स्थितिः)) {
        यदि (hasService()) {
            प्रत्यागम gService->unregister(कुञ्चिका, स्थितिः);
        }
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    प्रत्यागम असत्य;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::get_उपलब्धम्_Locales()
{
    ICULocaleService *service = getService();
    यदि (service == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम service->get_उपलब्धम्_Locales();
}
#यदि_अन्तः /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ध्रुव Locale& loc, int32_t विधा, UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

#यदि !UCONFIG_NO_SERVICE
    यदि (hasService()) {
        Locale actualLoc("");
        BreakIterator *परिणाम = (BreakIterator*)gService->get(loc, विधा, &actualLoc, स्थितिः);
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
        यदि (U_SUCCESS(स्थितिः) && (परिणाम != शून्यसूचकः) && *actualLoc.getName() != 0) {
            परिणाम->actualLocale = actualLoc;
            परिणाम->validLocale = actualLoc;
        }
        प्रत्यागम परिणाम;
    }
    अन्यथा
#यदि_अन्तः
    {
        प्रत्यागम makeInstance(loc, विधा, स्थितिः);
    }
}

// -------------------------------------
गणना { k_कुञ्चिका_मानम्_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ध्रुव Locale& loc, int32_t विधा, UError_संहिता& स्थितिः)
{

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    BreakIterator *परिणाम = शून्यसूचकः;
    विकल्प (विधा) {
    पक्ष UBRK_CHARACTER:
        {
            UTRACE_प्रविष्टिः(UTRACE_UBRK_CREATE_CHARACTER);
            परिणाम = BreakIterator::buildInstance(loc, "grapheme", स्थितिः);
            UTRACE_EXIT_स्थितिः(स्थितिः);
        }
        भङ्ग;
    पक्ष UBRK_WORD:
        {
            UTRACE_प्रविष्टिः(UTRACE_UBRK_CREATE_WORD);
            परिणाम = BreakIterator::buildInstance(loc, "word", स्थितिः);
            UTRACE_EXIT_स्थितिः(स्थितिः);
        }
        भङ्ग;
    पक्ष UBRK_पङ्क्तिः:
        {
            वर्ण lb_lw[k_कुञ्चिका_मानम्_Len_Max];
            UTRACE_प्रविष्टिः(UTRACE_UBRK_CREATE_पङ्क्तिः);
            uprv_strcpy(lb_lw, "line");
            UError_संहिता kv_स्थितिः = U_ZERO_ERROR;
            स्वचालित value = loc.get_Keyword_मानम्<CharString>("lb", kv_स्थितिः);
            यदि (U_SUCCESS(kv_स्थितिः) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.दत्त());
            }
            // lw=phrase is only supported in Japanese and Korean
            यदि (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_मानम्<CharString>("lw", kv_स्थितिः);
                यदि (U_SUCCESS(kv_स्थितिः) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.दत्त());
                }
            }
            परिणाम = BreakIterator::buildInstance(loc, lb_lw, स्थितिः);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_स्थितिः(स्थितिः);
        }
        भङ्ग;
    पक्ष UBRK_SENTENCE:
        {
            UTRACE_प्रविष्टिः(UTRACE_UBRK_CREATE_SENTENCE);
            परिणाम = BreakIterator::buildInstance(loc, "sentence", स्थितिः);
#यदि !UCONFIG_NO_FILTERED_BREAK_ITERATION
            वर्ण ss_कुञ्चिका_मानम्[k_कुञ्चिका_मानम्_Len_Max] = {0};
            UError_संहिता kv_स्थितिः = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_मानम्("ss", ss_कुञ्चिका_मानम्, k_कुञ्चिका_मानम्_Len_Max, kv_स्थितिः);
            यदि (U_SUCCESS(kv_स्थितिः) && kLen > 0 && uprv_strcmp(ss_कुञ्चिका_मानम्,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kv_स्थितिः);
                यदि (U_SUCCESS(kv_स्थितिः)) {
                    परिणाम = fbiBuilder->build(परिणाम, स्थितिः);
                    विलोपय fbiBuilder;
                }
            }
#यदि_अन्तः
            UTRACE_EXIT_स्थितिः(स्थितिः);
        }
        भङ्ग;
    पक्ष UBRK_TITLE:
        {
            UTRACE_प्रविष्टिः(UTRACE_UBRK_CREATE_TITLE);
            परिणाम = BreakIterator::buildInstance(loc, "title", स्थितिः);
            UTRACE_EXIT_स्थितिः(स्थितिः);
        }
        भङ्ग;
    स्वभाव:
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    }

    यदि (U_FAILURE(स्थितिः)) {
        विलोपय परिणाम;
        प्रत्यागम शून्यसूचकः;
    }

    प्रत्यागम परिणाम;
}

Locale
BreakIterator::getLocale(ULoc_दत्त_Locale_Type type, UError_संहिता& स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम Locale::get_मूलम्();
    }
    यदि (type == ULOC_REQUESTED_LOCALE) {
        प्रत्यागम requestLocale;
    }
    प्रत्यागम LocaleBased::getLocale(validLocale, actualLocale, type, स्थितिः);
}

ध्रुव वर्ण*
BreakIterator::getLocaleID(ULoc_दत्त_Locale_Type type, UError_संहिता& स्थितिः) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (type == ULOC_REQUESTED_LOCALE) {
        प्रत्यागम requestLocale.getName();
    }
    प्रत्यागम LocaleBased::getLocaleID(validLocale, actualLocale, type, स्थितिः);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_नियमः_स्थितिः() ध्रुव {
    प्रत्यागम 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_नियमः_स्थितिः_Vec(int32_t *fill_अन्तर्_Vec, int32_t capacity, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम 0;
    }
    यदि (capacity < 1) {
        स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम 1;
    }
    *fill_अन्तर्_Vec = 0;
    प्रत्यागम 1;
}

BreakIterator::BreakIterator(ध्रुव Locale& valid, ध्रुव Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_मूलम्()) {
}

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
