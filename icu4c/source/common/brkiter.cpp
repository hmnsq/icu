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
BreakIterator::buildInstance(ثابت Locale& loc, ثابت حرف *type, UError_رمز &حالت_2)
{
    حرف fnbuff[256];
    حرف ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ثابت char16_t* brkfname = خالی_اشاریہ;
    UResourceBundle brk_قواعد_ڈھیر;
    UResourceBundle brk_Name_ڈھیر;
    UResourceBundle *brk_قواعد = &brk_قواعد_ڈھیر;
    UResourceBundle *brkName  = &brk_Name_ڈھیر;
    قاعدہ_Based_Break_Iterator *نتیجہ = خالی_اشاریہ;

    اگر (U_FAILURE(حالت_2))
        واپس خالی_اشاریہ;

    ures_init_ڈھیر_Object(brk_قواعد);
    ures_init_ڈھیر_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &حالت_2);

    // Get the "boundaries" array.
    اگر (U_SUCCESS(حالت_2)) {
        brk_قواعد = ures_get_By_کلید_With_Fallback(b, "boundaries", brk_قواعد, &حالت_2);
        // Get the string object naming the rules file
        brkName = ures_get_By_کلید_With_Fallback(brk_قواعد, type, brkName, &حالت_2);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &حالت_2);
        U_ASSERT((size_t)size<حجم(fnbuff));
        اگر (static_cast<size_t>(size) >= حجم(fnbuff)) {
            size=0;
            اگر (U_SUCCESS(حالت_2)) {
                حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
            }
        }

        // Use the string if we found it
        اگر (U_SUCCESS(حالت_2) && brkfname) {
            actual.append(ures_get_Locale_اندرونی(brkName, &حالت_2), -1, حالت_2);

            char16_t* ext_آغاز=u_strchr(brkfname, 0x002e);
            صحیح len = 0;
            اگر (ext_آغاز != خالی_اشاریہ){
                len = static_cast<صحیح>(ext_آغاز - brkfname);
                u_UChars_مقصد_Chars(ext_آغاز+1, ext, حجم(ext)); // nul terminates the buff
                u_UChars_مقصد_Chars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brk_قواعد);
    ures_close(brkName);

    UData_حافظہ* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        ures_close(b);
        واپس خالی_اشاریہ;
    }

    // Create a RuleBasedBreakIterator
    نتیجہ = نیا قاعدہ_Based_Break_Iterator(file, uprv_strstr(type, "phrase") != خالی_اشاریہ, حالت_2);

    // If there is a result, set the valid locale and actual locale, and the kind
    اگر (U_SUCCESS(حالت_2) && نتیجہ != خالی_اشاریہ) {
        نتیجہ->actualLocale = Locale(actual.ڈیٹا());
        نتیجہ->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &حالت_2));
        نتیجہ->requestLocale = loc;
    }

    ures_close(b);

    اگر (U_FAILURE(حالت_2) && نتیجہ != خالی_اشاریہ) {  // Sometimes redundant check, but simple
        حذف نتیجہ;
        واپس خالی_اشاریہ;
    }

    اگر (نتیجہ == خالی_اشاریہ) {
        udata_close(file);
        اگر (U_SUCCESS(حالت_2)) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
        }
    }

    واپس نتیجہ;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_لفظ_Instance(ثابت Locale& کلید, UError_رمز& حالت_2)
{
    واپس createInstance(کلید, UBRK_لفظ, حالت_2);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_سطر_Instance(ثابت Locale& کلید, UError_رمز& حالت_2)
{
    واپس createInstance(کلید, UBRK_سطر, حالت_2);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ثابت Locale& کلید, UError_رمز& حالت_2)
{
    واپس createInstance(کلید, UBRK_CHARACTER, حالت_2);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ثابت Locale& کلید, UError_رمز& حالت_2)
{
    واپس createInstance(کلید, UBRK_SENTENCE, حالت_2);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ثابت Locale& کلید, UError_رمز& حالت_2)
{
    واپس createInstance(کلید, UBRK_TITLE, حالت_2);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ثابت Locale* U_EXPORT2
BreakIterator::get_دستیاب_Locales(int32_t& count)
{
    واپس Locale::get_دستیاب_Locales(count);
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
    اگر (یہ != &other) {
        actualLocale = other.actualLocale;
        validLocale = other.validLocale;
        requestLocale = other.requestLocale;
    }
    واپس *یہ;
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

جماعت ICUBreakIteratorFactory : عوامی ICUResourceBundleFactory {
عوامی:
    مجازی ~ICUBreakIteratorFactory();
محفوظ:
    مجازی UObject* handleCreate(ثابت Locale& loc, int32_t قسم, ثابت ICUService* /*service*/, UError_رمز& حالت_2) ثابت override {
        واپس BreakIterator::makeInstance(loc, قسم, حالت_2);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

جماعت ICUBreakIteratorService : عوامی ICULocaleService {
عوامی:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UError_رمز حالت_2 = U_ZERO_ERROR;
        رجسٹر_Factory(نیا ICUBreakIteratorFactory(), حالت_2);
    }

    مجازی ~ICUBreakIteratorService();

    مجازی UObject* cloneInstance(UObject* instance) ثابت override {
        واپس ((BreakIterator*)instance)->clone();
    }

    مجازی UObject* handleDefault(ثابت ICUService_کلید& کلید, UnicodeString* /*actualID*/, UError_رمز& حالت_2) ثابت override {
        Locale_کلید& lkey = static_cast<Locale_کلید&>(const_cast<ICUService_کلید&>(کلید));
        int32_t قسم = lkey.قسم();
        Locale loc;
        lkey.currentLocale(loc);
        واپس BreakIterator::makeInstance(loc, قسم, حالت_2);
    }

    مجازی UBool isDefault() ثابت override {
        واپس گنتی_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_انتہا

ساکن icu::UInitOnce gInitOnceBrkiter {};
ساکن icu::ICULocaleService* gService = خالی_اشاریہ;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
ساکن UBool U_CALLCONV breakiterator_cleanup() {
#اگر !UCONFIG_NO_SERVICE
    اگر (gService) {
        حذف gService;
        gService = خالی_اشاریہ;
    }
    gInitOnceBrkiter.reset();
#اگر_ختم
    واپس سچ;
}
U_CDECL_انتہا
U_NAMESPACE_BEGIN

ساکن خالی U_CALLCONV 
initService() {
    gService = نیا ICUBreakIteratorService();
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
}

ساکن ICULocaleService*
getService()
{
    umtx_initOnce(gInitOnceBrkiter, &initService);
    واپس gService;
}


// -------------------------------------

ساکن اندرونی UBool
hasService()
{
    واپس !gInitOnceBrkiter.isReset() && getService() != خالی_اشاریہ;
}

// -------------------------------------

URegistry_کلید U_EXPORT2
BreakIterator::رجسٹر_Instance(BreakIterator* مقصد_Adopt, ثابت Locale& locale, UBreakIteratorType قسم, UError_رمز& حالت_2)
{
    ICULocaleService *service = getService();
    اگر (service == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    واپس service->رجسٹر_Instance(مقصد_Adopt, locale, قسم, حالت_2);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_کلید کلید, UError_رمز& حالت_2)
{
    اگر (U_SUCCESS(حالت_2)) {
        اگر (hasService()) {
            واپس gService->unregister(کلید, حالت_2);
        }
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    }
    واپس جھوٹ;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::get_دستیاب_Locales()
{
    ICULocaleService *service = getService();
    اگر (service == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }
    واپس service->get_دستیاب_Locales();
}
#اگر_ختم /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ثابت Locale& loc, int32_t قسم, UError_رمز& حالت_2)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

#اگر !UCONFIG_NO_SERVICE
    اگر (hasService()) {
        Locale actualLoc("");
        BreakIterator *نتیجہ = (BreakIterator*)gService->get(loc, قسم, &actualLoc, حالت_2);
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
        اگر (U_SUCCESS(حالت_2) && (نتیجہ != خالی_اشاریہ) && *actualLoc.getName() != 0) {
            نتیجہ->actualLocale = actualLoc;
            نتیجہ->validLocale = actualLoc;
        }
        واپس نتیجہ;
    }
    ورنہ
#اگر_ختم
    {
        واپس makeInstance(loc, قسم, حالت_2);
    }
}

// -------------------------------------
شمار { k_کلید_قدر_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ثابت Locale& loc, int32_t قسم, UError_رمز& حالت_2)
{

    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    BreakIterator *نتیجہ = خالی_اشاریہ;
    انتخاب (قسم) {
    حالت UBRK_CHARACTER:
        {
            UTRACE_اندراج(UTRACE_UBRK_CREATE_CHARACTER);
            نتیجہ = BreakIterator::buildInstance(loc, "grapheme", حالت_2);
            UTRACE_EXIT_حالت(حالت_2);
        }
        توڑو;
    حالت UBRK_لفظ:
        {
            UTRACE_اندراج(UTRACE_UBRK_CREATE_لفظ);
            نتیجہ = BreakIterator::buildInstance(loc, "word", حالت_2);
            UTRACE_EXIT_حالت(حالت_2);
        }
        توڑو;
    حالت UBRK_سطر:
        {
            حرف lb_lw[k_کلید_قدر_Len_Max];
            UTRACE_اندراج(UTRACE_UBRK_CREATE_سطر);
            uprv_strcpy(lb_lw, "line");
            UError_رمز kv_حالت = U_ZERO_ERROR;
            خودکار value = loc.get_Keyword_قدر<CharString>("lb", kv_حالت);
            اگر (U_SUCCESS(kv_حالت) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.ڈیٹا());
            }
            // lw=phrase is only supported in Japanese and Korean
            اگر (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_قدر<CharString>("lw", kv_حالت);
                اگر (U_SUCCESS(kv_حالت) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.ڈیٹا());
                }
            }
            نتیجہ = BreakIterator::buildInstance(loc, lb_lw, حالت_2);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_حالت(حالت_2);
        }
        توڑو;
    حالت UBRK_SENTENCE:
        {
            UTRACE_اندراج(UTRACE_UBRK_CREATE_SENTENCE);
            نتیجہ = BreakIterator::buildInstance(loc, "sentence", حالت_2);
#اگر !UCONFIG_NO_FILTERED_BREAK_ITERATION
            حرف ss_کلید_قدر[k_کلید_قدر_Len_Max] = {0};
            UError_رمز kv_حالت = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_قدر("ss", ss_کلید_قدر, k_کلید_قدر_Len_Max, kv_حالت);
            اگر (U_SUCCESS(kv_حالت) && kLen > 0 && uprv_strcmp(ss_کلید_قدر,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kv_حالت);
                اگر (U_SUCCESS(kv_حالت)) {
                    نتیجہ = fbiBuilder->build(نتیجہ, حالت_2);
                    حذف fbiBuilder;
                }
            }
#اگر_ختم
            UTRACE_EXIT_حالت(حالت_2);
        }
        توڑو;
    حالت UBRK_TITLE:
        {
            UTRACE_اندراج(UTRACE_UBRK_CREATE_TITLE);
            نتیجہ = BreakIterator::buildInstance(loc, "title", حالت_2);
            UTRACE_EXIT_حالت(حالت_2);
        }
        توڑو;
    طےشدہ:
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
    }

    اگر (U_FAILURE(حالت_2)) {
        حذف نتیجہ;
        واپس خالی_اشاریہ;
    }

    واپس نتیجہ;
}

Locale
BreakIterator::getLocale(ULoc_ڈیٹا_Locale_Type type, UError_رمز& حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) {
        واپس Locale::get_جڑ();
    }
    اگر (type == ULOC_REQUESTED_LOCALE) {
        واپس requestLocale;
    }
    واپس LocaleBased::getLocale(validLocale, actualLocale, type, حالت_2);
}

ثابت حرف*
BreakIterator::getLocaleID(ULoc_ڈیٹا_Locale_Type type, UError_رمز& حالت_2) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر (type == ULOC_REQUESTED_LOCALE) {
        واپس requestLocale.getName();
    }
    واپس LocaleBased::getLocaleID(validLocale, actualLocale, type, حالت_2);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_قاعدہ_حالت() ثابت {
    واپس 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_قاعدہ_حالت_Vec(int32_t *fill_اندر_Vec, int32_t capacity, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس 0;
    }
    اگر (capacity < 1) {
        حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
        واپس 1;
    }
    *fill_اندر_Vec = 0;
    واپس 1;
}

BreakIterator::BreakIterator(ثابت Locale& valid, ثابت Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_جڑ()) {
}

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
