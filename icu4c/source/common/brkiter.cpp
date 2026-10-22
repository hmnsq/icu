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
BreakIterator::buildInstance(ثابت Locale& loc, ثابت حرف *type, UError_کد &وضعیت)
{
    حرف fnbuff[256];
    حرف ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ثابت char16_t* brkfname = اشاره_تهی;
    UResourceBundle brk_قواعد_پشته;
    UResourceBundle brk_Name_پشته;
    UResourceBundle *brk_قواعد = &brk_قواعد_پشته;
    UResourceBundle *brkName  = &brk_Name_پشته;
    قاعده_Based_Break_Iterator *نتیجه = اشاره_تهی;

    إذا (U_FAILURE(وضعیت))
        أرجع اشاره_تهی;

    ures_init_پشته_Object(brk_قواعد);
    ures_init_پشته_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &وضعیت);

    // Get the "boundaries" array.
    إذا (U_SUCCESS(وضعیت)) {
        brk_قواعد = ures_get_By_کلید_With_Fallback(b, "boundaries", brk_قواعد, &وضعیت);
        // Get the string object naming the rules file
        brkName = ures_get_By_کلید_With_Fallback(brk_قواعد, type, brkName, &وضعیت);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &وضعیت);
        U_ASSERT((size_t)size<حجم(fnbuff));
        إذا (static_cast<size_t>(size) >= حجم(fnbuff)) {
            size=0;
            إذا (U_SUCCESS(وضعیت)) {
                وضعیت = U_میانگیر_سرریز_ERROR;
            }
        }

        // Use the string if we found it
        إذا (U_SUCCESS(وضعیت) && brkfname) {
            actual.append(ures_get_Locale_داخلی(brkName, &وضعیت), -1, وضعیت);

            char16_t* ext_شروع=u_strchr(brkfname, 0x002e);
            صحيح len = 0;
            إذا (ext_شروع != اشاره_تهی){
                len = static_cast<صحيح>(ext_شروع - brkfname);
                u_UChars_مقصد_Chars(ext_شروع+1, ext, حجم(ext)); // nul terminates the buff
                u_UChars_مقصد_Chars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brk_قواعد);
    ures_close(brkName);

    UData_حافظه* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        ures_close(b);
        أرجع اشاره_تهی;
    }

    // Create a RuleBasedBreakIterator
    نتیجه = جديد قاعده_Based_Break_Iterator(file, uprv_strstr(type, "phrase") != اشاره_تهی, وضعیت);

    // If there is a result, set the valid locale and actual locale, and the kind
    إذا (U_SUCCESS(وضعیت) && نتیجه != اشاره_تهی) {
        نتیجه->actualLocale = Locale(actual.داده());
        نتیجه->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &وضعیت));
        نتیجه->requestLocale = loc;
    }

    ures_close(b);

    إذا (U_FAILURE(وضعیت) && نتیجه != اشاره_تهی) {  // Sometimes redundant check, but simple
        حذف نتیجه;
        أرجع اشاره_تهی;
    }

    إذا (نتیجه == اشاره_تهی) {
        udata_close(file);
        إذا (U_SUCCESS(وضعیت)) {
            وضعیت = U_حافظه_تخصیص_ERROR;
        }
    }

    أرجع نتیجه;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_کلمه_Instance(ثابت Locale& کلید, UError_کد& وضعیت)
{
    أرجع createInstance(کلید, UBRK_کلمه, وضعیت);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_سطر_Instance(ثابت Locale& کلید, UError_کد& وضعیت)
{
    أرجع createInstance(کلید, UBRK_سطر, وضعیت);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ثابت Locale& کلید, UError_کد& وضعیت)
{
    أرجع createInstance(کلید, UBRK_CHARACTER, وضعیت);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ثابت Locale& کلید, UError_کد& وضعیت)
{
    أرجع createInstance(کلید, UBRK_SENTENCE, وضعیت);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ثابت Locale& کلید, UError_کد& وضعیت)
{
    أرجع createInstance(کلید, UBRK_TITLE, وضعیت);
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
    : actualLocale(Locale::get_ریشه()), validLocale(Locale::get_ریشه()), requestLocale(Locale::get_ریشه())
{
}

BreakIterator::BreakIterator(ثابت BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::عملگر=(ثابت BreakIterator& other) U_LIFETIME_BOUND {
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
    مجازی UObject* handleCreate(ثابت Locale& loc, int32_t نوع, ثابت ICUService* /*service*/, UError_کد& وضعیت) ثابت override {
        أرجع BreakIterator::makeInstance(loc, نوع, وضعیت);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

صنف ICUBreakIteratorService : عام ICULocaleService {
عام:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UError_کد وضعیت = U_ZERO_ERROR;
        ثبات_Factory(جديد ICUBreakIteratorFactory(), وضعیت);
    }

    مجازی ~ICUBreakIteratorService();

    مجازی UObject* cloneInstance(UObject* instance) ثابت override {
        أرجع ((BreakIterator*)instance)->clone();
    }

    مجازی UObject* handleDefault(ثابت ICUService_کلید& کلید, UnicodeString* /*actualID*/, UError_کد& وضعیت) ثابت override {
        Locale_کلید& lkey = static_cast<Locale_کلید&>(const_cast<ICUService_کلید&>(کلید));
        int32_t نوع = lkey.نوع();
        Locale loc;
        lkey.currentLocale(loc);
        أرجع BreakIterator::makeInstance(loc, نوع, وضعیت);
    }

    مجازی UBool isDefault() ثابت override {
        أرجع شمار_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_انتها

ساكن icu::UInitOnce gInitOnceBrkiter {};
ساكن icu::ICULocaleService* gService = اشاره_تهی;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
ساكن UBool U_CALLCONV breakiterator_cleanup() {
#إذا !UCONFIG_NO_SERVICE
    إذا (gService) {
        حذف gService;
        gService = اشاره_تهی;
    }
    gInitOnceBrkiter.reset();
#پایان_اگر
    أرجع درست;
}
U_CDECL_انتها
U_NAMESPACE_BEGIN

ساكن فراغ U_CALLCONV 
initService() {
    gService = جديد ICUBreakIteratorService();
    ucln_common_ثبات_Cleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
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
    أرجع !gInitOnceBrkiter.isReset() && getService() != اشاره_تهی;
}

// -------------------------------------

URegistry_کلید U_EXPORT2
BreakIterator::ثبات_Instance(BreakIterator* مقصد_Adopt, ثابت Locale& locale, UBreakIteratorType نوع, UError_کد& وضعیت)
{
    ICULocaleService *service = getService();
    إذا (service == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    أرجع service->ثبات_Instance(مقصد_Adopt, locale, نوع, وضعیت);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_کلید کلید, UError_کد& وضعیت)
{
    إذا (U_SUCCESS(وضعیت)) {
        إذا (hasService()) {
            أرجع gService->unregister(کلید, وضعیت);
        }
        وضعیت = U_حافظه_تخصیص_ERROR;
    }
    أرجع خطأ;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::get_موجود_Locales()
{
    ICULocaleService *service = getService();
    إذا (service == اشاره_تهی) {
        أرجع اشاره_تهی;
    }
    أرجع service->get_موجود_Locales();
}
#پایان_اگر /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ثابت Locale& loc, int32_t نوع, UError_کد& وضعیت)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }

#إذا !UCONFIG_NO_SERVICE
    إذا (hasService()) {
        Locale actualLoc("");
        BreakIterator *نتیجه = (BreakIterator*)gService->get(loc, نوع, &actualLoc, وضعیت);
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
        إذا (U_SUCCESS(وضعیت) && (نتیجه != اشاره_تهی) && *actualLoc.getName() != 0) {
            نتیجه->actualLocale = actualLoc;
            نتیجه->validLocale = actualLoc;
        }
        أرجع نتیجه;
    }
    وإلا
#پایان_اگر
    {
        أرجع makeInstance(loc, نوع, وضعیت);
    }
}

// -------------------------------------
تعداد { k_کلید_مقدار_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ثابت Locale& loc, int32_t نوع, UError_کد& وضعیت)
{

    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }

    BreakIterator *نتیجه = اشاره_تهی;
    بدل (نوع) {
    حالة UBRK_CHARACTER:
        {
            UTRACE_مدخل(UTRACE_UBRK_CREATE_CHARACTER);
            نتیجه = BreakIterator::buildInstance(loc, "grapheme", وضعیت);
            UTRACE_EXIT_وضعیت(وضعیت);
        }
        اكسر;
    حالة UBRK_کلمه:
        {
            UTRACE_مدخل(UTRACE_UBRK_CREATE_کلمه);
            نتیجه = BreakIterator::buildInstance(loc, "word", وضعیت);
            UTRACE_EXIT_وضعیت(وضعیت);
        }
        اكسر;
    حالة UBRK_سطر:
        {
            حرف lb_lw[k_کلید_مقدار_Len_Max];
            UTRACE_مدخل(UTRACE_UBRK_CREATE_سطر);
            uprv_strcpy(lb_lw, "line");
            UError_کد kv_وضعیت = U_ZERO_ERROR;
            خودکار value = loc.get_Keyword_مقدار<CharString>("lb", kv_وضعیت);
            إذا (U_SUCCESS(kv_وضعیت) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.داده());
            }
            // lw=phrase is only supported in Japanese and Korean
            إذا (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_مقدار<CharString>("lw", kv_وضعیت);
                إذا (U_SUCCESS(kv_وضعیت) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.داده());
                }
            }
            نتیجه = BreakIterator::buildInstance(loc, lb_lw, وضعیت);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_وضعیت(وضعیت);
        }
        اكسر;
    حالة UBRK_SENTENCE:
        {
            UTRACE_مدخل(UTRACE_UBRK_CREATE_SENTENCE);
            نتیجه = BreakIterator::buildInstance(loc, "sentence", وضعیت);
#إذا !UCONFIG_NO_FILTERED_BREAK_ITERATION
            حرف ss_کلید_مقدار[k_کلید_مقدار_Len_Max] = {0};
            UError_کد kv_وضعیت = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_مقدار("ss", ss_کلید_مقدار, k_کلید_مقدار_Len_Max, kv_وضعیت);
            إذا (U_SUCCESS(kv_وضعیت) && kLen > 0 && uprv_strcmp(ss_کلید_مقدار,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kv_وضعیت);
                إذا (U_SUCCESS(kv_وضعیت)) {
                    نتیجه = fbiBuilder->build(نتیجه, وضعیت);
                    حذف fbiBuilder;
                }
            }
#پایان_اگر
            UTRACE_EXIT_وضعیت(وضعیت);
        }
        اكسر;
    حالة UBRK_TITLE:
        {
            UTRACE_مدخل(UTRACE_UBRK_CREATE_TITLE);
            نتیجه = BreakIterator::buildInstance(loc, "title", وضعیت);
            UTRACE_EXIT_وضعیت(وضعیت);
        }
        اكسر;
    افتراضي:
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
    }

    إذا (U_FAILURE(وضعیت)) {
        حذف نتیجه;
        أرجع اشاره_تهی;
    }

    أرجع نتیجه;
}

Locale
BreakIterator::getLocale(ULoc_داده_Locale_Type type, UError_کد& وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) {
        أرجع Locale::get_ریشه();
    }
    إذا (type == ULOC_REQUESTED_LOCALE) {
        أرجع requestLocale;
    }
    أرجع LocaleBased::getLocale(validLocale, actualLocale, type, وضعیت);
}

ثابت حرف*
BreakIterator::getLocaleID(ULoc_داده_Locale_Type type, UError_کد& وضعیت) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا (type == ULOC_REQUESTED_LOCALE) {
        أرجع requestLocale.getName();
    }
    أرجع LocaleBased::getLocaleID(validLocale, actualLocale, type, وضعیت);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_قاعده_وضعیت() ثابت {
    أرجع 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_قاعده_وضعیت_Vec(int32_t *fill_درون_Vec, int32_t capacity, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع 0;
    }
    إذا (capacity < 1) {
        وضعیت = U_میانگیر_سرریز_ERROR;
        أرجع 1;
    }
    *fill_درون_Vec = 0;
    أرجع 1;
}

BreakIterator::BreakIterator(ثابت Locale& valid, ثابت Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_ریشه()) {
}

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
