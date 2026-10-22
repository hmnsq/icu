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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/rbbi.h"
#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/filteredbrk.h"
#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "servloc.h"
#అన్తర్భావ "locbased.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ubrkimpl.h"
#అన్తర్భావ "utracimp.h"
#అన్తర్భావ "charstr.h"

// *****************************************************************************
// class BreakIterator
// This class implements methods for finding the location of boundaries in text.
// Instances of BreakIterator maintain a current position and scan over text
// returning the index of characters where boundaries occur.
// *****************************************************************************

U_NAMESPACE_BEGIN

// -------------------------------------

BreakIterator*
BreakIterator::buildInstance(స్థిరరాశి Locale& loc, స్థిరరాశి అక్షరం *type, UError_సంకేతం &స్థితి_2)
{
    అక్షరం fnbuff[256];
    అక్షరం ext[4]={'\0'};
    CharString actual;
    int32_t size;
    స్థిరరాశి char16_t* brkfname = శూన్యసూచిక;
    UResourceBundle brk_నియమాలు_దొంతర;
    UResourceBundle brk_Name_దొంతర;
    UResourceBundle *brk_నియమాలు = &brk_నియమాలు_దొంతర;
    UResourceBundle *brkName  = &brk_Name_దొంతర;
    నియమం_Based_Break_Iterator *ఫలితం = శూన్యసూచిక;

    ఒకవేళ (U_FAILURE(స్థితి_2))
        తిరిగి శూన్యసూచిక;

    ures_init_దొంతర_Object(brk_నియమాలు);
    ures_init_దొంతర_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &స్థితి_2);

    // Get the "boundaries" array.
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        brk_నియమాలు = ures_get_By_కీలకం_With_Fallback(b, "boundaries", brk_నియమాలు, &స్థితి_2);
        // Get the string object naming the rules file
        brkName = ures_get_By_కీలకం_With_Fallback(brk_నియమాలు, type, brkName, &స్థితి_2);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &స్థితి_2);
        U_ASSERT((size_t)size<పరిమాణం(fnbuff));
        ఒకవేళ (static_cast<size_t>(size) >= పరిమాణం(fnbuff)) {
            size=0;
            ఒకవేళ (U_SUCCESS(స్థితి_2)) {
                స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
            }
        }

        // Use the string if we found it
        ఒకవేళ (U_SUCCESS(స్థితి_2) && brkfname) {
            actual.append(ures_get_Locale_అంతర్గత(brkName, &స్థితి_2), -1, స్థితి_2);

            char16_t* ext_ప్రారంభం=u_strchr(brkfname, 0x002e);
            పూర్ణాంకం len = 0;
            ఒకవేళ (ext_ప్రారంభం != శూన్యసూచిక){
                len = static_cast<పూర్ణాంకం>(ext_ప్రారంభం - brkfname);
                u_UChars_గమ్యం_Chars(ext_ప్రారంభం+1, ext, పరిమాణం(ext)); // nul terminates the buff
                u_UChars_గమ్యం_Chars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brk_నియమాలు);
    ures_close(brkName);

    UData_జ్ఞాపకం* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        ures_close(b);
        తిరిగి శూన్యసూచిక;
    }

    // Create a RuleBasedBreakIterator
    ఫలితం = కొత్త నియమం_Based_Break_Iterator(file, uprv_strstr(type, "phrase") != శూన్యసూచిక, స్థితి_2);

    // If there is a result, set the valid locale and actual locale, and the kind
    ఒకవేళ (U_SUCCESS(స్థితి_2) && ఫలితం != శూన్యసూచిక) {
        ఫలితం->actualLocale = Locale(actual.దత్తాంశం());
        ఫలితం->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &స్థితి_2));
        ఫలితం->requestLocale = loc;
    }

    ures_close(b);

    ఒకవేళ (U_FAILURE(స్థితి_2) && ఫలితం != శూన్యసూచిక) {  // Sometimes redundant check, but simple
        తొలగించు ఫలితం;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (ఫలితం == శూన్యసూచిక) {
        udata_close(file);
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
    }

    తిరిగి ఫలితం;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_మాట_Instance(స్థిరరాశి Locale& కీలకం, UError_సంకేతం& స్థితి_2)
{
    తిరిగి createInstance(కీలకం, UBRK_మాట, స్థితి_2);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_పంక్తి_Instance(స్థిరరాశి Locale& కీలకం, UError_సంకేతం& స్థితి_2)
{
    తిరిగి createInstance(కీలకం, UBRK_పంక్తి, స్థితి_2);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(స్థిరరాశి Locale& కీలకం, UError_సంకేతం& స్థితి_2)
{
    తిరిగి createInstance(కీలకం, UBRK_CHARACTER, స్థితి_2);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(స్థిరరాశి Locale& కీలకం, UError_సంకేతం& స్థితి_2)
{
    తిరిగి createInstance(కీలకం, UBRK_SENTENCE, స్థితి_2);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(స్థిరరాశి Locale& కీలకం, UError_సంకేతం& స్థితి_2)
{
    తిరిగి createInstance(కీలకం, UBRK_TITLE, స్థితి_2);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
స్థిరరాశి Locale* U_EXPORT2
BreakIterator::get_అందుబాటు_Locales(int32_t& count)
{
    తిరిగి Locale::get_అందుబాటు_Locales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_మూలం()), validLocale(Locale::get_మూలం()), requestLocale(Locale::get_మూలం())
{
}

BreakIterator::BreakIterator(స్థిరరాశి BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::క్రియాకారకం=(స్థిరరాశి BreakIterator& other) U_LIFETIME_BOUND {
    ఒకవేళ (ఇది != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    తిరిగి *ఇది;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#ఒకవేళ !UCONFIG_NO_SERVICE

// -------------------------------------

తరగతి ICUBreakIteratorFactory : బహిరంగ ICUResourceBundleFactory {
బహిరంగ:
    మాయా ~ICUBreakIteratorFactory();
రక్షిత:
    మాయా UObject* handleCreate(స్థిరరాశి Locale& loc, int32_t రకం, స్థిరరాశి ICUService* /*service*/, UError_సంకేతం& స్థితి_2) స్థిరరాశి override {
        తిరిగి BreakIterator::makeInstance(loc, రకం, స్థితి_2);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

తరగతి ICUBreakIteratorService : బహిరంగ ICULocaleService {
బహిరంగ:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        రిజిస్టరు_Factory(కొత్త ICUBreakIteratorFactory(), స్థితి_2);
    }

    మాయా ~ICUBreakIteratorService();

    మాయా UObject* cloneInstance(UObject* instance) స్థిరరాశి override {
        తిరిగి ((BreakIterator*)instance)->clone();
    }

    మాయా UObject* handleDefault(స్థిరరాశి ICUService_కీలకం& కీలకం, UnicodeString* /*actualID*/, UError_సంకేతం& స్థితి_2) స్థిరరాశి override {
        Locale_కీలకం& lkey = static_cast<Locale_కీలకం&>(const_cast<ICUService_కీలకం&>(కీలకం));
        int32_t రకం = lkey.రకం();
        Locale loc;
        lkey.currentLocale(loc);
        తిరిగి BreakIterator::makeInstance(loc, రకం, స్థితి_2);
    }

    మాయా UBool isDefault() స్థిరరాశి override {
        తిరిగి లెక్క_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_చివర

స్థిర icu::UInitOnce gInitOnceBrkiter {};
స్థిర icu::ICULocaleService* gService = శూన్యసూచిక;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
స్థిర UBool U_CALLCONV breakiterator_cleanup() {
#ఒకవేళ !UCONFIG_NO_SERVICE
    ఒకవేళ (gService) {
        తొలగించు gService;
        gService = శూన్యసూచిక;
    }
    gInitOnceBrkiter.reset();
#అయితే_ముగింపు
    తిరిగి నిజం;
}
U_CDECL_చివర
U_NAMESPACE_BEGIN

స్థిర శూన్య U_CALLCONV 
initService() {
    gService = కొత్త ICUBreakIteratorService();
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

స్థిర ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    తిరిగి gService;
}


// -------------------------------------

స్థిర అంతఃవరుస UBool
hasService()
{
    తిరిగి !gInitOnceBrkiter.isReset() && getService() != శూన్యసూచిక;
}

// -------------------------------------

URegistry_కీలకం U_EXPORT2
BreakIterator::రిజిస్టరు_Instance(BreakIterator* గమ్యం_Adopt, స్థిరరాశి Locale& locale, UBreakIteratorType రకం, UError_సంకేతం& స్థితి_2)
{
    ICULocaleService *service = getService();
    ఒకవేళ (service == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి service->రిజిస్టరు_Instance(గమ్యం_Adopt, locale, రకం, స్థితి_2);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_కీలకం కీలకం, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        ఒకవేళ (hasService()) {
            తిరిగి gService->unregister(కీలకం, స్థితి_2);
        }
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    తిరిగి అబద్ధం;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::get_అందుబాటు_Locales()
{
    ICULocaleService *service = getService();
    ఒకవేళ (service == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి service->get_అందుబాటు_Locales();
}
#అయితే_ముగింపు /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(స్థిరరాశి Locale& loc, int32_t రకం, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

#ఒకవేళ !UCONFIG_NO_SERVICE
    ఒకవేళ (hasService()) {
        Locale actualLoc("");
        BreakIterator *ఫలితం = (BreakIterator*)gService->get(loc, రకం, &actualLoc, స్థితి_2);
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
        ఒకవేళ (U_SUCCESS(స్థితి_2) && (ఫలితం != శూన్యసూచిక) && *actualLoc.getName() != 0) {
            ఫలితం->actualLocale = actualLoc;
            ఫలితం->validLocale = actualLoc;
        }
        తిరిగి ఫలితం;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
        తిరిగి makeInstance(loc, రకం, స్థితి_2);
    }
}

// -------------------------------------
గణన { k_కీలకం_విలువ_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(స్థిరరాశి Locale& loc, int32_t రకం, UError_సంకేతం& స్థితి_2)
{

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    BreakIterator *ఫలితం = శూన్యసూచిక;
    ఎంపిక (రకం) {
    స్థితి UBRK_CHARACTER:
        {
            UTRACE_నమోదు(UTRACE_UBRK_CREATE_CHARACTER);
            ఫలితం = BreakIterator::buildInstance(loc, "grapheme", స్థితి_2);
            UTRACE_EXIT_స్థితి(స్థితి_2);
        }
        ఆపు;
    స్థితి UBRK_మాట:
        {
            UTRACE_నమోదు(UTRACE_UBRK_CREATE_మాట);
            ఫలితం = BreakIterator::buildInstance(loc, "word", స్థితి_2);
            UTRACE_EXIT_స్థితి(స్థితి_2);
        }
        ఆపు;
    స్థితి UBRK_పంక్తి:
        {
            అక్షరం lb_lw[k_కీలకం_విలువ_Len_Max];
            UTRACE_నమోదు(UTRACE_UBRK_CREATE_పంక్తి);
            uprv_strcpy(lb_lw, "line");
            UError_సంకేతం kv_స్థితి = U_ZERO_ERROR;
            స్వయంచాలక value = loc.get_Keyword_విలువ<CharString>("lb", kv_స్థితి);
            ఒకవేళ (U_SUCCESS(kv_స్థితి) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.దత్తాంశం());
            }
            // lw=phrase is only supported in Japanese and Korean
            ఒకవేళ (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_విలువ<CharString>("lw", kv_స్థితి);
                ఒకవేళ (U_SUCCESS(kv_స్థితి) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.దత్తాంశం());
                }
            }
            ఫలితం = BreakIterator::buildInstance(loc, lb_lw, స్థితి_2);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_స్థితి(స్థితి_2);
        }
        ఆపు;
    స్థితి UBRK_SENTENCE:
        {
            UTRACE_నమోదు(UTRACE_UBRK_CREATE_SENTENCE);
            ఫలితం = BreakIterator::buildInstance(loc, "sentence", స్థితి_2);
#ఒకవేళ !UCONFIG_NO_FILTERED_BREAK_ITERATION
            అక్షరం ss_కీలకం_విలువ[k_కీలకం_విలువ_Len_Max] = {0};
            UError_సంకేతం kv_స్థితి = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_విలువ("ss", ss_కీలకం_విలువ, k_కీలకం_విలువ_Len_Max, kv_స్థితి);
            ఒకవేళ (U_SUCCESS(kv_స్థితి) && kLen > 0 && uprv_strcmp(ss_కీలకం_విలువ,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kv_స్థితి);
                ఒకవేళ (U_SUCCESS(kv_స్థితి)) {
                    ఫలితం = fbiBuilder->build(ఫలితం, స్థితి_2);
                    తొలగించు fbiBuilder;
                }
            }
#అయితే_ముగింపు
            UTRACE_EXIT_స్థితి(స్థితి_2);
        }
        ఆపు;
    స్థితి UBRK_TITLE:
        {
            UTRACE_నమోదు(UTRACE_UBRK_CREATE_TITLE);
            ఫలితం = BreakIterator::buildInstance(loc, "title", స్థితి_2);
            UTRACE_EXIT_స్థితి(స్థితి_2);
        }
        ఆపు;
    అప్రమేయ:
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    }

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తొలగించు ఫలితం;
        తిరిగి శూన్యసూచిక;
    }

    తిరిగి ఫలితం;
}

Locale
BreakIterator::getLocale(ULoc_దత్తాంశం_Locale_Type type, UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి Locale::get_మూలం();
    }
    ఒకవేళ (type == ULOC_REQUESTED_LOCALE) {
        తిరిగి requestLocale;
    }
    తిరిగి LocaleBased::getLocale(validLocale, actualLocale, type, స్థితి_2);
}

స్థిరరాశి అక్షరం*
BreakIterator::getLocaleID(ULoc_దత్తాంశం_Locale_Type type, UError_సంకేతం& స్థితి_2) స్థిరరాశి U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (type == ULOC_REQUESTED_LOCALE) {
        తిరిగి requestLocale.getName();
    }
    తిరిగి LocaleBased::getLocaleID(validLocale, actualLocale, type, స్థితి_2);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_నియమం_స్థితి() స్థిరరాశి {
    తిరిగి 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_నియమం_స్థితి_Vec(int32_t *fill_లోపల_Vec, int32_t capacity, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి 0;
    }
    ఒకవేళ (capacity < 1) {
        స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి 1;
    }
    *fill_లోపల_Vec = 0;
    తిరిగి 1;
}

BreakIterator::BreakIterator(స్థిరరాశి Locale& valid, స్థిరరాశి Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_మూలం()) {
}

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
