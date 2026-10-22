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

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

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
BreakIterator::buildInstance(ಸ್ಥಿರಾಂಕ Locale& loc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
{
    ಅಕ್ಷರ fnbuff[256];
    ಅಕ್ಷರ ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ಸ್ಥಿರಾಂಕ char16_t* brkfname = ಶೂನ್ಯಸೂಚಕ;
    UResourceBundle brk_ನಿಯಮಗಳು_ರಾಶಿ;
    UResourceBundle brk_Name_ರಾಶಿ;
    UResourceBundle *brk_ನಿಯಮಗಳು = &brk_ನಿಯಮಗಳು_ರಾಶಿ;
    UResourceBundle *brkName  = &brk_Name_ರಾಶಿ;
    ನಿಯಮ_Based_Break_Iterator *ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2))
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

    ures_init_ರಾಶಿ_Object(brk_ನಿಯಮಗಳು);
    ures_init_ರಾಶಿ_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &ಸ್ಥಿತಿ_2);

    // Get the "boundaries" array.
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        brk_ನಿಯಮಗಳು = ures_get_By_ಕೀಲಿ_With_Fallback(b, "boundaries", brk_ನಿಯಮಗಳು, &ಸ್ಥಿತಿ_2);
        // Get the string object naming the rules file
        brkName = ures_get_By_ಕೀಲಿ_With_Fallback(brk_ನಿಯಮಗಳು, type, brkName, &ಸ್ಥಿತಿ_2);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &ಸ್ಥಿತಿ_2);
        U_ASSERT((size_t)size<ಗಾತ್ರ(fnbuff));
        ಆದರೆ (static_cast<size_t>(size) >= ಗಾತ್ರ(fnbuff)) {
            size=0;
            ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
                ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            }
        }

        // Use the string if we found it
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && brkfname) {
            actual.append(ures_get_Locale_ಆಂತರಿಕ(brkName, &ಸ್ಥಿತಿ_2), -1, ಸ್ಥಿತಿ_2);

            char16_t* ext_ಆರಂಭ=u_strchr(brkfname, 0x002e);
            ಪೂರ್ಣಾಂಕ len = 0;
            ಆದರೆ (ext_ಆರಂಭ != ಶೂನ್ಯಸೂಚಕ){
                len = static_cast<ಪೂರ್ಣಾಂಕ>(ext_ಆರಂಭ - brkfname);
                u_UChars_ಗಮ್ಯ_Chars(ext_ಆರಂಭ+1, ext, ಗಾತ್ರ(ext)); // nul terminates the buff
                u_UChars_ಗಮ್ಯ_Chars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brk_ನಿಯಮಗಳು);
    ures_close(brkName);

    UData_ಸ್ಮರಣೆ* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ures_close(b);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // Create a RuleBasedBreakIterator
    ಫಲಿತಾಂಶ = ಹೊಸ ನಿಯಮ_Based_Break_Iterator(file, uprv_strstr(type, "phrase") != ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);

    // If there is a result, set the valid locale and actual locale, and the kind
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && ಫಲಿತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        ಫಲಿತಾಂಶ->actualLocale = Locale(actual.ದತ್ತಾಂಶ());
        ಫಲಿತಾಂಶ->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &ಸ್ಥಿತಿ_2));
        ಫಲಿತಾಂಶ->requestLocale = loc;
    }

    ures_close(b);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2) && ಫಲಿತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {  // Sometimes redundant check, but simple
        ಅಳಿಸು ಫಲಿತಾಂಶ;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        udata_close(file);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_ಶಬ್ದ_Instance(ಸ್ಥಿರಾಂಕ Locale& ಕೀಲಿ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ createInstance(ಕೀಲಿ, UBRK_ಶಬ್ದ, ಸ್ಥಿತಿ_2);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_ಸಾಲು_Instance(ಸ್ಥಿರಾಂಕ Locale& ಕೀಲಿ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ createInstance(ಕೀಲಿ, UBRK_ಸಾಲು, ಸ್ಥಿತಿ_2);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ಸ್ಥಿರಾಂಕ Locale& ಕೀಲಿ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ createInstance(ಕೀಲಿ, UBRK_CHARACTER, ಸ್ಥಿತಿ_2);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ಸ್ಥಿರಾಂಕ Locale& ಕೀಲಿ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ createInstance(ಕೀಲಿ, UBRK_SENTENCE, ಸ್ಥಿತಿ_2);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ಸ್ಥಿರಾಂಕ Locale& ಕೀಲಿ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ createInstance(ಕೀಲಿ, UBRK_TITLE, ಸ್ಥಿತಿ_2);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ಸ್ಥಿರಾಂಕ Locale* U_EXPORT2
BreakIterator::get_ಲಭ್ಯ_Locales(int32_t& count)
{
    ಹಿಂತಿರುಗಿ Locale::get_ಲಭ್ಯ_Locales(count);
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

BreakIterator::BreakIterator(ಸ್ಥಿರಾಂಕ BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ BreakIterator& other) U_LIFETIME_BOUND {
    ಆದರೆ (ಇದು != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

BreakIterator::~BreakIterator()
{
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------
#ಆದರೆ !UCONFIG_NO_SERVICE

// -------------------------------------

ವರ್ಗ ICUBreakIteratorFactory : ಸಾರ್ವಜನಿಕ ICUResourceBundleFactory {
ಸಾರ್ವಜನಿಕ:
    ವಾಸ್ತವಿಕ ~ICUBreakIteratorFactory();
ಸಂರಕ್ಷಿತ:
    ವಾಸ್ತವಿಕ UObject* handleCreate(ಸ್ಥಿರಾಂಕ Locale& loc, int32_t ಬಗೆ, ಸ್ಥಿರಾಂಕ ICUService* /*service*/, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ BreakIterator::makeInstance(loc, ಬಗೆ, ಸ್ಥಿತಿ_2);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

ವರ್ಗ ICUBreakIteratorService : ಸಾರ್ವಜನಿಕ ICULocaleService {
ಸಾರ್ವಜನಿಕ:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        ರಿಜಿಸ್ಟರ್_Factory(ಹೊಸ ICUBreakIteratorFactory(), ಸ್ಥಿತಿ_2);
    }

    ವಾಸ್ತವಿಕ ~ICUBreakIteratorService();

    ವಾಸ್ತವಿಕ UObject* cloneInstance(UObject* instance) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ ((BreakIterator*)instance)->clone();
    }

    ವಾಸ್ತವಿಕ UObject* handleDefault(ಸ್ಥಿರಾಂಕ ICUService_ಕೀಲಿ& ಕೀಲಿ, UnicodeString* /*actualID*/, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override {
        Locale_ಕೀಲಿ& lkey = static_cast<Locale_ಕೀಲಿ&>(const_cast<ICUService_ಕೀಲಿ&>(ಕೀಲಿ));
        int32_t ಬಗೆ = lkey.ಬಗೆ();
        Locale loc;
        lkey.currentLocale(loc);
        ಹಿಂತಿರುಗಿ BreakIterator::makeInstance(loc, ಬಗೆ, ಸ್ಥಿತಿ_2);
    }

    ವಾಸ್ತವಿಕ UBool isDefault() ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_ಕೊನೆ

ಸ್ಥಿರ icu::UInitOnce gInitOnceBrkiter {};
ಸ್ಥಿರ icu::ICULocaleService* gService = ಶೂನ್ಯಸೂಚಕ;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV breakiterator_cleanup() {
#ಆದರೆ !UCONFIG_NO_SERVICE
    ಆದರೆ (gService) {
        ಅಳಿಸು gService;
        gService = ಶೂನ್ಯಸೂಚಕ;
    }
    gInitOnceBrkiter.reset();
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ನಿಜ;
}
U_CDECL_ಕೊನೆ
U_NAMESPACE_BEGIN

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV 
initService() {
    gService = ಹೊಸ ICUBreakIteratorService();
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

ಸ್ಥಿರ ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    ಹಿಂತಿರುಗಿ gService;
}


// -------------------------------------

ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
hasService()
{
    ಹಿಂತಿರುಗಿ !gInitOnceBrkiter.isReset() && getService() != ಶೂನ್ಯಸೂಚಕ;
}

// -------------------------------------

URegistry_ಕೀಲಿ U_EXPORT2
BreakIterator::ರಿಜಿಸ್ಟರ್_Instance(BreakIterator* ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ Locale& locale, UBreakIteratorType ಬಗೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ICULocaleService *service = getService();
    ಆದರೆ (service == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ service->ರಿಜಿಸ್ಟರ್_Instance(ಗಮ್ಯ_Adopt, locale, ಬಗೆ, ಸ್ಥಿತಿ_2);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_ಕೀಲಿ ಕೀಲಿ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (hasService()) {
            ಹಿಂತಿರುಗಿ gService->unregister(ಕೀಲಿ, ಸ್ಥಿತಿ_2);
        }
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::get_ಲಭ್ಯ_Locales()
{
    ICULocaleService *service = getService();
    ಆದರೆ (service == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ service->get_ಲಭ್ಯ_Locales();
}
#ಆದರೆ_ಅಂತ್ಯ /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ಸ್ಥಿರಾಂಕ Locale& loc, int32_t ಬಗೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

#ಆದರೆ !UCONFIG_NO_SERVICE
    ಆದರೆ (hasService()) {
        Locale actualLoc("");
        BreakIterator *ಫಲಿತಾಂಶ = (BreakIterator*)gService->get(loc, ಬಗೆ, &actualLoc, ಸ್ಥಿತಿ_2);
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
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && (ಫಲಿತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) && *actualLoc.getName() != 0) {
            ಫಲಿತಾಂಶ->actualLocale = actualLoc;
            ಫಲಿತಾಂಶ->validLocale = actualLoc;
        }
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
        ಹಿಂತಿರುಗಿ makeInstance(loc, ಬಗೆ, ಸ್ಥಿತಿ_2);
    }
}

// -------------------------------------
ಎಣಿಕೆ { k_ಕೀಲಿ_ಮೌಲ್ಯ_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ಸ್ಥಿರಾಂಕ Locale& loc, int32_t ಬಗೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    BreakIterator *ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    ಆಯ್ಕೆ (ಬಗೆ) {
    ಸ್ಥಿತಿ UBRK_CHARACTER:
        {
            UTRACE_ನಮೂದು(UTRACE_UBRK_CREATE_CHARACTER);
            ಫಲಿತಾಂಶ = BreakIterator::buildInstance(loc, "grapheme", ಸ್ಥಿತಿ_2);
            UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UBRK_ಶಬ್ದ:
        {
            UTRACE_ನಮೂದು(UTRACE_UBRK_CREATE_ಶಬ್ದ);
            ಫಲಿತಾಂಶ = BreakIterator::buildInstance(loc, "word", ಸ್ಥಿತಿ_2);
            UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UBRK_ಸಾಲು:
        {
            ಅಕ್ಷರ lb_lw[k_ಕೀಲಿ_ಮೌಲ್ಯ_Len_Max];
            UTRACE_ನಮೂದು(UTRACE_UBRK_CREATE_ಸಾಲು);
            uprv_strcpy(lb_lw, "line");
            UError_ಸಂಕೇತ kv_ಸ್ಥಿತಿ = U_ZERO_ERROR;
            ಸ್ವಯಂಚಾಲಿತ value = loc.get_Keyword_ಮೌಲ್ಯ<CharString>("lb", kv_ಸ್ಥಿತಿ);
            ಆದರೆ (U_SUCCESS(kv_ಸ್ಥಿತಿ) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.ದತ್ತಾಂಶ());
            }
            // lw=phrase is only supported in Japanese and Korean
            ಆದರೆ (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_ಮೌಲ್ಯ<CharString>("lw", kv_ಸ್ಥಿತಿ);
                ಆದರೆ (U_SUCCESS(kv_ಸ್ಥಿತಿ) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.ದತ್ತಾಂಶ());
                }
            }
            ಫಲಿತಾಂಶ = BreakIterator::buildInstance(loc, lb_lw, ಸ್ಥಿತಿ_2);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UBRK_SENTENCE:
        {
            UTRACE_ನಮೂದು(UTRACE_UBRK_CREATE_SENTENCE);
            ಫಲಿತಾಂಶ = BreakIterator::buildInstance(loc, "sentence", ಸ್ಥಿತಿ_2);
#ಆದರೆ !UCONFIG_NO_FILTERED_BREAK_ITERATION
            ಅಕ್ಷರ ss_ಕೀಲಿ_ಮೌಲ್ಯ[k_ಕೀಲಿ_ಮೌಲ್ಯ_Len_Max] = {0};
            UError_ಸಂಕೇತ kv_ಸ್ಥಿತಿ = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_ಮೌಲ್ಯ("ss", ss_ಕೀಲಿ_ಮೌಲ್ಯ, k_ಕೀಲಿ_ಮೌಲ್ಯ_Len_Max, kv_ಸ್ಥಿತಿ);
            ಆದರೆ (U_SUCCESS(kv_ಸ್ಥಿತಿ) && kLen > 0 && uprv_strcmp(ss_ಕೀಲಿ_ಮೌಲ್ಯ,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kv_ಸ್ಥಿತಿ);
                ಆದರೆ (U_SUCCESS(kv_ಸ್ಥಿತಿ)) {
                    ಫಲಿತಾಂಶ = fbiBuilder->build(ಫಲಿತಾಂಶ, ಸ್ಥಿತಿ_2);
                    ಅಳಿಸು fbiBuilder;
                }
            }
#ಆದರೆ_ಅಂತ್ಯ
            UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UBRK_TITLE:
        {
            UTRACE_ನಮೂದು(UTRACE_UBRK_CREATE_TITLE);
            ಫಲಿತಾಂಶ = BreakIterator::buildInstance(loc, "title", ಸ್ಥಿತಿ_2);
            UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    }

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಅಳಿಸು ಫಲಿತಾಂಶ;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

Locale
BreakIterator::getLocale(ULoc_ದತ್ತಾಂಶ_Locale_Type type, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ Locale::get_ಬೇರು();
    }
    ಆದರೆ (type == ULOC_REQUESTED_LOCALE) {
        ಹಿಂತಿರುಗಿ requestLocale;
    }
    ಹಿಂತಿರುಗಿ LocaleBased::getLocale(validLocale, actualLocale, type, ಸ್ಥಿತಿ_2);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
BreakIterator::getLocaleID(ULoc_ದತ್ತಾಂಶ_Locale_Type type, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (type == ULOC_REQUESTED_LOCALE) {
        ಹಿಂತಿರುಗಿ requestLocale.getName();
    }
    ಹಿಂತಿರುಗಿ LocaleBased::getLocaleID(validLocale, actualLocale, type, ಸ್ಥಿತಿ_2);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_ನಿಯಮ_ಸ್ಥಿತಿ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_ನಿಯಮ_ಸ್ಥಿತಿ_Vec(int32_t *fill_ಒಳ_Vec, int32_t capacity, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (capacity < 1) {
        ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ 1;
    }
    *fill_ಒಳ_Vec = 0;
    ಹಿಂತಿರುಗಿ 1;
}

BreakIterator::BreakIterator(ಸ್ಥಿರಾಂಕ Locale& valid, ಸ್ಥಿರಾಂಕ Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_ಬೇರು()) {
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
