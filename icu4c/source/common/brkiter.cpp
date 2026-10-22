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
BreakIterator::buildInstance(ثابت Locale& loc, ثابت حرف *type, UError_کوڈ &حالت)
{
    حرف fnbuff[256];
    حرف ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ثابت char16_t* brkfname = خالی_سوچک;
    UResourceBundle brk_قاعدے_ڈھیر;
    UResourceBundle brk_Name_ڈھیر;
    UResourceBundle *brk_قاعدے = &brk_قاعدے_ڈھیر;
    UResourceBundle *brkName  = &brk_Name_ڈھیر;
    قاعدہ_Based_Break_Iterator *نتیجہ = خالی_سوچک;

    إذا (U_FAILURE(حالت))
        أرجع خالی_سوچک;

    ures_init_ڈھیر_Object(brk_قاعدے);
    ures_init_ڈھیر_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &حالت);

    // Get the "boundaries" array.
    إذا (U_SUCCESS(حالت)) {
        brk_قاعدے = ures_get_By_کلید_With_Fallback(b, "boundaries", brk_قاعدے, &حالت);
        // Get the string object naming the rules file
        brkName = ures_get_By_کلید_With_Fallback(brk_قاعدے, type, brkName, &حالت);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &حالت);
        U_ASSERT((size_t)size<حجم(fnbuff));
        إذا (static_cast<size_t>(size) >= حجم(fnbuff)) {
            size=0;
            إذا (U_SUCCESS(حالت)) {
                حالت = U_بفر_حد_توں_ودھ_ERROR;
            }
        }

        // Use the string if we found it
        إذا (U_SUCCESS(حالت) && brkfname) {
            actual.append(ures_get_Locale_اندرونی(brkName, &حالت), -1, حالت);

            char16_t* ext_شروع=u_strchr(brkfname, 0x002e);
            صحيح len = 0;
            إذا (ext_شروع != خالی_سوچک){
                len = static_cast<صحيح>(ext_شروع - brkfname);
                u_UChars_منزل_Chars(ext_شروع+1, ext, حجم(ext)); // nul terminates the buff
                u_UChars_منزل_Chars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brk_قاعدے);
    ures_close(brkName);

    UData_حافظہ* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &حالت);
    إذا (U_FAILURE(حالت)) {
        ures_close(b);
        أرجع خالی_سوچک;
    }

    // Create a RuleBasedBreakIterator
    نتیجہ = جديد قاعدہ_Based_Break_Iterator(file, uprv_strstr(type, "phrase") != خالی_سوچک, حالت);

    // If there is a result, set the valid locale and actual locale, and the kind
    إذا (U_SUCCESS(حالت) && نتیجہ != خالی_سوچک) {
        نتیجہ->actualLocale = Locale(actual.انکڑے());
        نتیجہ->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &حالت));
        نتیجہ->requestLocale = loc;
    }

    ures_close(b);

    إذا (U_FAILURE(حالت) && نتیجہ != خالی_سوچک) {  // Sometimes redundant check, but simple
        مٹاؤ نتیجہ;
        أرجع خالی_سوچک;
    }

    إذا (نتیجہ == خالی_سوچک) {
        udata_close(file);
        إذا (U_SUCCESS(حالت)) {
            حالت = U_حافظہ_ونڈ_ERROR;
        }
    }

    أرجع نتیجہ;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_لفظ_Instance(ثابت Locale& کلید, UError_کوڈ& حالت)
{
    أرجع createInstance(کلید, UBRK_لفظ, حالت);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_سطر_Instance(ثابت Locale& کلید, UError_کوڈ& حالت)
{
    أرجع createInstance(کلید, UBRK_سطر, حالت);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ثابت Locale& کلید, UError_کوڈ& حالت)
{
    أرجع createInstance(کلید, UBRK_CHARACTER, حالت);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ثابت Locale& کلید, UError_کوڈ& حالت)
{
    أرجع createInstance(کلید, UBRK_SENTENCE, حالت);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ثابت Locale& کلید, UError_کوڈ& حالت)
{
    أرجع createInstance(کلید, UBRK_TITLE, حالت);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ثابت Locale* U_EXPORT2
BreakIterator::get_دستیاب_Locales(int32_t& count)
{
    أرجع Locale::get_دستیاب_Locales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_جڑ()), validLocale(Locale::get_جڑ()), requestLocale(Locale::get_جڑ())
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
    مجازی ~ICUBreakIteratorFactory();
محمي:
    مجازی UObject* handleCreate(ثابت Locale& loc, int32_t قسم, ثابت ICUService* /*service*/, UError_کوڈ& حالت) ثابت override {
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
        UError_کوڈ حالت = U_ZERO_ERROR;
        رجسٹر_Factory(جديد ICUBreakIteratorFactory(), حالت);
    }

    مجازی ~ICUBreakIteratorService();

    مجازی UObject* cloneInstance(UObject* instance) ثابت override {
        أرجع ((BreakIterator*)instance)->clone();
    }

    مجازی UObject* handleDefault(ثابت ICUService_کلید& کلید, UnicodeString* /*actualID*/, UError_کوڈ& حالت) ثابت override {
        Locale_کلید& lkey = static_cast<Locale_کلید&>(const_cast<ICUService_کلید&>(کلید));
        int32_t قسم = lkey.قسم();
        Locale loc;
        lkey.currentLocale(loc);
        أرجع BreakIterator::makeInstance(loc, قسم, حالت);
    }

    مجازی UBool isDefault() ثابت override {
        أرجع گنتی_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_سرا

ساكن icu::UInitOnce gInitOnceBrkiter {};
ساكن icu::ICULocaleService* gService = خالی_سوچک;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
ساكن UBool U_CALLCONV breakiterator_cleanup() {
#إذا !UCONFIG_NO_SERVICE
    إذا (gService) {
        مٹاؤ gService;
        gService = خالی_سوچک;
    }
    gInitOnceBrkiter.reset();
#جے_انت
    أرجع سچ;
}
U_CDECL_سرا
U_NAMESPACE_BEGIN

ساكن فراغ U_CALLCONV 
initService() {
    gService = جديد ICUBreakIteratorService();
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
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
    أرجع !gInitOnceBrkiter.isReset() && getService() != خالی_سوچک;
}

// -------------------------------------

URegistry_کلید U_EXPORT2
BreakIterator::رجسٹر_Instance(BreakIterator* منزل_Adopt, ثابت Locale& locale, UBreakIteratorType قسم, UError_کوڈ& حالت)
{
    ICULocaleService *service = getService();
    إذا (service == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    أرجع service->رجسٹر_Instance(منزل_Adopt, locale, قسم, حالت);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_کلید کلید, UError_کوڈ& حالت)
{
    إذا (U_SUCCESS(حالت)) {
        إذا (hasService()) {
            أرجع gService->unregister(کلید, حالت);
        }
        حالت = U_حافظہ_ونڈ_ERROR;
    }
    أرجع خطأ;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::get_دستیاب_Locales()
{
    ICULocaleService *service = getService();
    إذا (service == خالی_سوچک) {
        أرجع خالی_سوچک;
    }
    أرجع service->get_دستیاب_Locales();
}
#جے_انت /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ثابت Locale& loc, int32_t قسم, UError_کوڈ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

#إذا !UCONFIG_NO_SERVICE
    إذا (hasService()) {
        Locale actualLoc("");
        BreakIterator *نتیجہ = (BreakIterator*)gService->get(loc, قسم, &actualLoc, حالت);
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
        إذا (U_SUCCESS(حالت) && (نتیجہ != خالی_سوچک) && *actualLoc.getName() != 0) {
            نتیجہ->actualLocale = actualLoc;
            نتیجہ->validLocale = actualLoc;
        }
        أرجع نتیجہ;
    }
    وإلا
#جے_انت
    {
        أرجع makeInstance(loc, قسم, حالت);
    }
}

// -------------------------------------
تعداد { k_کلید_قدر_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ثابت Locale& loc, int32_t قسم, UError_کوڈ& حالت)
{

    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    BreakIterator *نتیجہ = خالی_سوچک;
    بدل (قسم) {
    حالة UBRK_CHARACTER:
        {
            UTRACE_اندراج(UTRACE_UBRK_CREATE_CHARACTER);
            نتیجہ = BreakIterator::buildInstance(loc, "grapheme", حالت);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_لفظ:
        {
            UTRACE_اندراج(UTRACE_UBRK_CREATE_لفظ);
            نتیجہ = BreakIterator::buildInstance(loc, "word", حالت);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_سطر:
        {
            حرف lb_lw[k_کلید_قدر_Len_Max];
            UTRACE_اندراج(UTRACE_UBRK_CREATE_سطر);
            uprv_strcpy(lb_lw, "line");
            UError_کوڈ kv_حالت = U_ZERO_ERROR;
            خودکار value = loc.get_Keyword_قدر<CharString>("lb", kv_حالت);
            إذا (U_SUCCESS(kv_حالت) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.انکڑے());
            }
            // lw=phrase is only supported in Japanese and Korean
            إذا (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_قدر<CharString>("lw", kv_حالت);
                إذا (U_SUCCESS(kv_حالت) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.انکڑے());
                }
            }
            نتیجہ = BreakIterator::buildInstance(loc, lb_lw, حالت);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_SENTENCE:
        {
            UTRACE_اندراج(UTRACE_UBRK_CREATE_SENTENCE);
            نتیجہ = BreakIterator::buildInstance(loc, "sentence", حالت);
#إذا !UCONFIG_NO_FILTERED_BREAK_ITERATION
            حرف ss_کلید_قدر[k_کلید_قدر_Len_Max] = {0};
            UError_کوڈ kv_حالت = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_قدر("ss", ss_کلید_قدر, k_کلید_قدر_Len_Max, kv_حالت);
            إذا (U_SUCCESS(kv_حالت) && kLen > 0 && uprv_strcmp(ss_کلید_قدر,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kv_حالت);
                إذا (U_SUCCESS(kv_حالت)) {
                    نتیجہ = fbiBuilder->build(نتیجہ, حالت);
                    مٹاؤ fbiBuilder;
                }
            }
#جے_انت
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_TITLE:
        {
            UTRACE_اندراج(UTRACE_UBRK_CREATE_TITLE);
            نتیجہ = BreakIterator::buildInstance(loc, "title", حالت);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    افتراضي:
        حالت = U_ILLEGAL_دلیل_ERROR;
    }

    إذا (U_FAILURE(حالت)) {
        مٹاؤ نتیجہ;
        أرجع خالی_سوچک;
    }

    أرجع نتیجہ;
}

Locale
BreakIterator::getLocale(ULoc_انکڑے_Locale_Type type, UError_کوڈ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع Locale::get_جڑ();
    }
    إذا (type == ULOC_REQUESTED_LOCALE) {
        أرجع requestLocale;
    }
    أرجع LocaleBased::getLocale(validLocale, actualLocale, type, حالت);
}

ثابت حرف*
BreakIterator::getLocaleID(ULoc_انکڑے_Locale_Type type, UError_کوڈ& حالت) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    إذا (type == ULOC_REQUESTED_LOCALE) {
        أرجع requestLocale.getName();
    }
    أرجع LocaleBased::getLocaleID(validLocale, actualLocale, type, حالت);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_قاعدہ_حالت() ثابت {
    أرجع 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_قاعدہ_حالت_Vec(int32_t *fill_وچ_Vec, int32_t capacity, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    إذا (capacity < 1) {
        حالت = U_بفر_حد_توں_ودھ_ERROR;
        أرجع 1;
    }
    *fill_وچ_Vec = 0;
    أرجع 1;
}

BreakIterator::BreakIterator(ثابت Locale& valid, ثابت Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_جڑ()) {
}

U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
