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

#إذا !UCONFIG_NO_BREAK_ITERATION

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
BreakIterator::buildInstance(ثابت Locale& loc, ثابت حرف *type, UError_ڪوڊ &حالت)
{
    حرف fnbuff[256];
    حرف ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ثابت char16_t* brkfname = خالي_اشارو;
    UResourceBundle brk_قاعدا_ڍير;
    UResourceBundle brk_Name_ڍير;
    UResourceBundle *brk_قاعدا = &brk_قاعدا_ڍير;
    UResourceBundle *brkName  = &brk_Name_ڍير;
    قاعدو_Based_Break_Iterator *نتيجو = خالي_اشارو;

    إذا (U_FAILURE(حالت))
        أرجع خالي_اشارو;

    ures_init_ڍير_Object(brk_قاعدا);
    ures_init_ڍير_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &حالت);

    // Get the "boundaries" array.
    إذا (U_SUCCESS(حالت)) {
        brk_قاعدا = ures_get_By_ڪنجي_With_Fallback(b, "boundaries", brk_قاعدا, &حالت);
        // Get the string object naming the rules file
        brkName = ures_get_By_ڪنجي_With_Fallback(brk_قاعدا, type, brkName, &حالت);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &حالت);
        U_ASSERT((size_t)size<حجم(fnbuff));
        إذا (static_cast<size_t>(size) >= حجم(fnbuff)) {
            size=0;
            إذا (U_SUCCESS(حالت)) {
                حالت = U_بفر_حد_کان_مٿي_ERROR;
            }
        }

        // Use the string if we found it
        إذا (U_SUCCESS(حالت) && brkfname) {
            actual.append(ures_get_Locale_اندروني(brkName, &حالت), -1, حالت);

            char16_t* ext_شروعات=u_strchr(brkfname, 0x002e);
            صحيح len = 0;
            إذا (ext_شروعات != خالي_اشارو){
                len = static_cast<صحيح>(ext_شروعات - brkfname);
                u_UChars_منزل_Chars(ext_شروعات+1, ext, حجم(ext)); // nul terminates the buff
                u_UChars_منزل_Chars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brk_قاعدا);
    ures_close(brkName);

    UData_يادگيري* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &حالت);
    إذا (U_FAILURE(حالت)) {
        ures_close(b);
        أرجع خالي_اشارو;
    }

    // Create a RuleBasedBreakIterator
    نتيجو = جديد قاعدو_Based_Break_Iterator(file, uprv_strstr(type, "phrase") != خالي_اشارو, حالت);

    // If there is a result, set the valid locale and actual locale, and the kind
    إذا (U_SUCCESS(حالت) && نتيجو != خالي_اشارو) {
        نتيجو->actualLocale = Locale(actual.معلومات());
        نتيجو->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &حالت));
        نتيجو->requestLocale = loc;
    }

    ures_close(b);

    إذا (U_FAILURE(حالت) && نتيجو != خالي_اشارو) {  // Sometimes redundant check, but simple
        ميٽيو نتيجو;
        أرجع خالي_اشارو;
    }

    إذا (نتيجو == خالي_اشارو) {
        udata_close(file);
        إذا (U_SUCCESS(حالت)) {
            حالت = U_يادگيري_ورهاست_ERROR;
        }
    }

    أرجع نتيجو;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_لفظ_Instance(ثابت Locale& ڪنجي, UError_ڪوڊ& حالت)
{
    أرجع createInstance(ڪنجي, UBRK_لفظ, حالت);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_سٽ_Instance(ثابت Locale& ڪنجي, UError_ڪوڊ& حالت)
{
    أرجع createInstance(ڪنجي, UBRK_سٽ, حالت);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ثابت Locale& ڪنجي, UError_ڪوڊ& حالت)
{
    أرجع createInstance(ڪنجي, UBRK_CHARACTER, حالت);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ثابت Locale& ڪنجي, UError_ڪوڊ& حالت)
{
    أرجع createInstance(ڪنجي, UBRK_SENTENCE, حالت);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ثابت Locale& ڪنجي, UError_ڪوڊ& حالت)
{
    أرجع createInstance(ڪنجي, UBRK_TITLE, حالت);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ثابت Locale* U_EXPORT2
BreakIterator::get_موجود_Locales(int32_t& count)
{
    أرجع Locale::get_موجود_Locales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_پاڙ()), validLocale(Locale::get_پاڙ()), requestLocale(Locale::get_پاڙ())
{
}

BreakIterator::BreakIterator(ثابت BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::عامل=(ثابت BreakIterator& other) U_LIFETIME_BOUND {
    إذا (هذا != &other) {
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
#إذا !UCONFIG_NO_SERVICE

// -------------------------------------

صنف ICUBreakIteratorFactory : عام ICUResourceBundleFactory {
عام:
    مجازي ~ICUBreakIteratorFactory();
محمي:
    مجازي UObject* handleCreate(ثابت Locale& loc, int32_t قسم, ثابت ICUService* /*service*/, UError_ڪوڊ& حالت) ثابت override {
        أرجع BreakIterator::makeInstance(loc, قسم, حالت);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

صنف ICUBreakIteratorService : عام ICULocaleService {
عام:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        رجسٽر_Factory(جديد ICUBreakIteratorFactory(), حالت);
    }

    مجازي ~ICUBreakIteratorService();

    مجازي UObject* cloneInstance(UObject* instance) ثابت override {
        أرجع ((BreakIterator*)instance)->clone();
    }

    مجازي UObject* handleDefault(ثابت ICUService_ڪنجي& ڪنجي, UnicodeString* /*actualID*/, UError_ڪوڊ& حالت) ثابت override {
        Locale_ڪنجي& lkey = static_cast<Locale_ڪنجي&>(const_cast<ICUService_ڪنجي&>(ڪنجي));
        int32_t قسم = lkey.قسم();
        Locale loc;
        lkey.currentLocale(loc);
        أرجع BreakIterator::makeInstance(loc, قسم, حالت);
    }

    مجازي UBool isDefault() ثابت override {
        أرجع ڳڻپ_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_پڇاڙي

ساكن icu::UInitOnce gInitOnceBrkiter {};
ساكن icu::ICULocaleService* gService = خالي_اشارو;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
ساكن UBool U_CALLCONV breakiterator_cleanup() {
#إذا !UCONFIG_NO_SERVICE
    إذا (gService) {
        ميٽيو gService;
        gService = خالي_اشارو;
    }
    gInitOnceBrkiter.reset();
#جيڪڏهن_پورو
    أرجع سچ;
}
U_CDECL_پڇاڙي
U_NAMESPACE_BEGIN

ساكن فراغ U_CALLCONV 
initService() {
    gService = جديد ICUBreakIteratorService();
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
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
    أرجع !gInitOnceBrkiter.isReset() && getService() != خالي_اشارو;
}

// -------------------------------------

URegistry_ڪنجي U_EXPORT2
BreakIterator::رجسٽر_Instance(BreakIterator* منزل_Adopt, ثابت Locale& locale, UBreakIteratorType قسم, UError_ڪوڊ& حالت)
{
    ICULocaleService *service = getService();
    إذا (service == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    أرجع service->رجسٽر_Instance(منزل_Adopt, locale, قسم, حالت);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_ڪنجي ڪنجي, UError_ڪوڊ& حالت)
{
    إذا (U_SUCCESS(حالت)) {
        إذا (hasService()) {
            أرجع gService->unregister(ڪنجي, حالت);
        }
        حالت = U_يادگيري_ورهاست_ERROR;
    }
    أرجع خطأ;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::get_موجود_Locales()
{
    ICULocaleService *service = getService();
    إذا (service == خالي_اشارو) {
        أرجع خالي_اشارو;
    }
    أرجع service->get_موجود_Locales();
}
#جيڪڏهن_پورو /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ثابت Locale& loc, int32_t قسم, UError_ڪوڊ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }

#إذا !UCONFIG_NO_SERVICE
    إذا (hasService()) {
        Locale actualLoc("");
        BreakIterator *نتيجو = (BreakIterator*)gService->get(loc, قسم, &actualLoc, حالت);
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
        إذا (U_SUCCESS(حالت) && (نتيجو != خالي_اشارو) && *actualLoc.getName() != 0) {
            نتيجو->actualLocale = actualLoc;
            نتيجو->validLocale = actualLoc;
        }
        أرجع نتيجو;
    }
    وإلا
#جيڪڏهن_پورو
    {
        أرجع makeInstance(loc, قسم, حالت);
    }
}

// -------------------------------------
تعداد { k_ڪنجي_قدر_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ثابت Locale& loc, int32_t قسم, UError_ڪوڊ& حالت)
{

    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }

    BreakIterator *نتيجو = خالي_اشارو;
    بدل (قسم) {
    حالة UBRK_CHARACTER:
        {
            UTRACE_داخلا(UTRACE_UBRK_CREATE_CHARACTER);
            نتيجو = BreakIterator::buildInstance(loc, "grapheme", حالت);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_لفظ:
        {
            UTRACE_داخلا(UTRACE_UBRK_CREATE_لفظ);
            نتيجو = BreakIterator::buildInstance(loc, "word", حالت);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_سٽ:
        {
            حرف lb_lw[k_ڪنجي_قدر_Len_Max];
            UTRACE_داخلا(UTRACE_UBRK_CREATE_سٽ);
            uprv_strcpy(lb_lw, "line");
            UError_ڪوڊ kv_حالت = U_ZERO_ERROR;
            خودڪار value = loc.get_Keyword_قدر<CharString>("lb", kv_حالت);
            إذا (U_SUCCESS(kv_حالت) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.معلومات());
            }
            // lw=phrase is only supported in Japanese and Korean
            إذا (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_قدر<CharString>("lw", kv_حالت);
                إذا (U_SUCCESS(kv_حالت) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.معلومات());
                }
            }
            نتيجو = BreakIterator::buildInstance(loc, lb_lw, حالت);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_SENTENCE:
        {
            UTRACE_داخلا(UTRACE_UBRK_CREATE_SENTENCE);
            نتيجو = BreakIterator::buildInstance(loc, "sentence", حالت);
#إذا !UCONFIG_NO_FILTERED_BREAK_ITERATION
            حرف ss_ڪنجي_قدر[k_ڪنجي_قدر_Len_Max] = {0};
            UError_ڪوڊ kv_حالت = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_قدر("ss", ss_ڪنجي_قدر, k_ڪنجي_قدر_Len_Max, kv_حالت);
            إذا (U_SUCCESS(kv_حالت) && kLen > 0 && uprv_strcmp(ss_ڪنجي_قدر,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kv_حالت);
                إذا (U_SUCCESS(kv_حالت)) {
                    نتيجو = fbiBuilder->build(نتيجو, حالت);
                    ميٽيو fbiBuilder;
                }
            }
#جيڪڏهن_پورو
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_TITLE:
        {
            UTRACE_داخلا(UTRACE_UBRK_CREATE_TITLE);
            نتيجو = BreakIterator::buildInstance(loc, "title", حالت);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    افتراضي:
        حالت = U_ILLEGAL_دليل_ERROR;
    }

    إذا (U_FAILURE(حالت)) {
        ميٽيو نتيجو;
        أرجع خالي_اشارو;
    }

    أرجع نتيجو;
}

Locale
BreakIterator::getLocale(ULoc_معلومات_Locale_Type type, UError_ڪوڊ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع Locale::get_پاڙ();
    }
    إذا (type == ULOC_REQUESTED_LOCALE) {
        أرجع requestLocale;
    }
    أرجع LocaleBased::getLocale(validLocale, actualLocale, type, حالت);
}

ثابت حرف*
BreakIterator::getLocaleID(ULoc_معلومات_Locale_Type type, UError_ڪوڊ& حالت) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }
    إذا (type == ULOC_REQUESTED_LOCALE) {
        أرجع requestLocale.getName();
    }
    أرجع LocaleBased::getLocaleID(validLocale, actualLocale, type, حالت);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_قاعدو_حالت() ثابت {
    أرجع 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_قاعدو_حالت_Vec(int32_t *fill_اندريون_Vec, int32_t capacity, UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    إذا (capacity < 1) {
        حالت = U_بفر_حد_کان_مٿي_ERROR;
        أرجع 1;
    }
    *fill_اندريون_Vec = 0;
    أرجع 1;
}

BreakIterator::BreakIterator(ثابت Locale& valid, ثابت Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_پاڙ()) {
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
