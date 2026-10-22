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
BreakIterator::buildInstance(ثابت Locale& loc, ثابت حرف *type, UError_کوډ &حالت)
{
    حرف fnbuff[256];
    حرف ext[4]={'\0'};
    CharString actual;
    int32_t size;
    ثابت char16_t* brkfname = تش_اشاره;
    UResourceBundle brk_قواعد_ډېری;
    UResourceBundle brk_Name_ډېری;
    UResourceBundle *brk_قواعد = &brk_قواعد_ډېری;
    UResourceBundle *brkName  = &brk_Name_ډېری;
    قاعده_Based_Break_Iterator *پایله = تش_اشاره;

    إذا (U_FAILURE(حالت))
        أرجع تش_اشاره;

    ures_init_ډېری_Object(brk_قواعد);
    ures_init_ډېری_Object(brkName);

    // Get the locale
    UResourceBundle *b = ures_openNoDefault(U_ICUDATA_BRKITR, loc.getName(), &حالت);

    // Get the "boundaries" array.
    إذا (U_SUCCESS(حالت)) {
        brk_قواعد = ures_get_By_کلی_With_Fallback(b, "boundaries", brk_قواعد, &حالت);
        // Get the string object naming the rules file
        brkName = ures_get_By_کلی_With_Fallback(brk_قواعد, type, brkName, &حالت);
        // Get the actual string
        brkfname = ures_getString(brkName, &size, &حالت);
        U_ASSERT((size_t)size<حجم(fnbuff));
        إذا (static_cast<size_t>(size) >= حجم(fnbuff)) {
            size=0;
            إذا (U_SUCCESS(حالت)) {
                حالت = U_بفر_ډکېدنه_ERROR;
            }
        }

        // Use the string if we found it
        إذا (U_SUCCESS(حالت) && brkfname) {
            actual.append(ures_get_Locale_دننی(brkName, &حالت), -1, حالت);

            char16_t* ext_پیل=u_strchr(brkfname, 0x002e);
            صحيح len = 0;
            إذا (ext_پیل != تش_اشاره){
                len = static_cast<صحيح>(ext_پیل - brkfname);
                u_UChars_منزل_Chars(ext_پیل+1, ext, حجم(ext)); // nul terminates the buff
                u_UChars_منزل_Chars(brkfname, fnbuff, len);
            }
            fnbuff[len]=0; // nul terminate
        }
    }

    ures_close(brk_قواعد);
    ures_close(brkName);

    UData_حافظه* file = udata_open(U_ICUDATA_BRKITR, ext, fnbuff, &حالت);
    إذا (U_FAILURE(حالت)) {
        ures_close(b);
        أرجع تش_اشاره;
    }

    // Create a RuleBasedBreakIterator
    پایله = جديد قاعده_Based_Break_Iterator(file, uprv_strstr(type, "phrase") != تش_اشاره, حالت);

    // If there is a result, set the valid locale and actual locale, and the kind
    إذا (U_SUCCESS(حالت) && پایله != تش_اشاره) {
        پایله->actualLocale = Locale(actual.معلومات());
        پایله->validLocale = Locale(ures_getLocaleByType(b, ULOC_VALID_LOCALE, &حالت));
        پایله->requestLocale = loc;
    }

    ures_close(b);

    إذا (U_FAILURE(حالت) && پایله != تش_اشاره) {  // Sometimes redundant check, but simple
        ړنګول پایله;
        أرجع تش_اشاره;
    }

    إذا (پایله == تش_اشاره) {
        udata_close(file);
        إذا (U_SUCCESS(حالت)) {
            حالت = U_حافظه_ویش_ERROR;
        }
    }

    أرجع پایله;
}

// Creates a break iterator for word breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_کلمه_Instance(ثابت Locale& کلی, UError_کوډ& حالت)
{
    أرجع createInstance(کلی, UBRK_کلمه, حالت);
}

// -------------------------------------

// Creates a break iterator  for line breaks.
BreakIterator* U_EXPORT2
BreakIterator::create_کرښه_Instance(ثابت Locale& کلی, UError_کوډ& حالت)
{
    أرجع createInstance(کلی, UBRK_کرښه, حالت);
}

// -------------------------------------

// Creates a break iterator  for character breaks.
BreakIterator* U_EXPORT2
BreakIterator::createCharacterInstance(ثابت Locale& کلی, UError_کوډ& حالت)
{
    أرجع createInstance(کلی, UBRK_CHARACTER, حالت);
}

// -------------------------------------

// Creates a break iterator  for sentence breaks.
BreakIterator* U_EXPORT2
BreakIterator::createSentenceInstance(ثابت Locale& کلی, UError_کوډ& حالت)
{
    أرجع createInstance(کلی, UBRK_SENTENCE, حالت);
}

// -------------------------------------

// Creates a break iterator for title casing breaks.
BreakIterator* U_EXPORT2
BreakIterator::createTitleInstance(ثابت Locale& کلی, UError_کوډ& حالت)
{
    أرجع createInstance(کلی, UBRK_TITLE, حالت);
}

// -------------------------------------

// Gets all the available locales that has localized text boundary data.
ثابت Locale* U_EXPORT2
BreakIterator::get_شته_Locales(int32_t& count)
{
    أرجع Locale::get_شته_Locales(count);
}

// ------------------------------------------
//
// Constructors, destructor and assignment operator
//
//-------------------------------------------

BreakIterator::BreakIterator()
    : actualLocale(Locale::get_ریښه()), validLocale(Locale::get_ریښه()), requestLocale(Locale::get_ریښه())
{
}

BreakIterator::BreakIterator(ثابت BreakIterator &other)
    : UObject(other),
      actualLocale(other.actualLocale),
      validLocale(other.validLocale),
      requestLocale(other.requestLocale) {
}

BreakIterator& BreakIterator::چلوونکی=(ثابت BreakIterator& other) U_LIFETIME_BOUND {
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
    مجازي UObject* handleCreate(ثابت Locale& loc, int32_t ډول, ثابت ICUService* /*service*/, UError_کوډ& حالت) ثابت override {
        أرجع BreakIterator::makeInstance(loc, ډول, حالت);
    }
};

ICUBreakIteratorFactory::~ICUBreakIteratorFactory() {}

// -------------------------------------

صنف ICUBreakIteratorService : عام ICULocaleService {
عام:
    ICUBreakIteratorService()
        : ICULocaleService(UNICODE_STRING("Break Iterator", 14))
    {
        UError_کوډ حالت = U_ZERO_ERROR;
        ثبتوونکی_Factory(جديد ICUBreakIteratorFactory(), حالت);
    }

    مجازي ~ICUBreakIteratorService();

    مجازي UObject* cloneInstance(UObject* instance) ثابت override {
        أرجع ((BreakIterator*)instance)->clone();
    }

    مجازي UObject* handleDefault(ثابت ICUService_کلی& کلی, UnicodeString* /*actualID*/, UError_کوډ& حالت) ثابت override {
        Locale_کلی& lkey = static_cast<Locale_کلی&>(const_cast<ICUService_کلی&>(کلی));
        int32_t ډول = lkey.ډول();
        Locale loc;
        lkey.currentLocale(loc);
        أرجع BreakIterator::makeInstance(loc, ډول, حالت);
    }

    مجازي UBool isDefault() ثابت override {
        أرجع شمېر_Factories() == 1;
    }
};

ICUBreakIteratorService::~ICUBreakIteratorService() {}

// -------------------------------------

// defined in ucln_cmn.h
U_NAMESPACE_پای_ټکی

ساكن icu::UInitOnce gInitOnceBrkiter {};
ساكن icu::ICULocaleService* gService = تش_اشاره;



/**
 * Release all static memory held by breakiterator.
 */
U_CDECL_BEGIN
ساكن UBool U_CALLCONV breakiterator_cleanup() {
#إذا !UCONFIG_NO_SERVICE
    إذا (gService) {
        ړنګول gService;
        gService = تش_اشاره;
    }
    gInitOnceBrkiter.reset();
#د_که_پای
    أرجع ریښتیا;
}
U_CDECL_پای_ټکی
U_NAMESPACE_BEGIN

ساكن فراغ U_CALLCONV 
initService() {
    gService = جديد ICUBreakIteratorService();
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_BREAKITERATOR, breakiterator_cleanup);
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
    أرجع !gInitOnceBrkiter.isReset() && getService() != تش_اشاره;
}

// -------------------------------------

URegistry_کلی U_EXPORT2
BreakIterator::ثبتوونکی_Instance(BreakIterator* منزل_Adopt, ثابت Locale& locale, UBreakIteratorType ډول, UError_کوډ& حالت)
{
    ICULocaleService *service = getService();
    إذا (service == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    أرجع service->ثبتوونکی_Instance(منزل_Adopt, locale, ډول, حالت);
}

// -------------------------------------

UBool U_EXPORT2
BreakIterator::unregister(URegistry_کلی کلی, UError_کوډ& حالت)
{
    إذا (U_SUCCESS(حالت)) {
        إذا (hasService()) {
            أرجع gService->unregister(کلی, حالت);
        }
        حالت = U_حافظه_ویش_ERROR;
    }
    أرجع خطأ;
}

// -------------------------------------

StringEnumeration* U_EXPORT2
BreakIterator::get_شته_Locales()
{
    ICULocaleService *service = getService();
    إذا (service == تش_اشاره) {
        أرجع تش_اشاره;
    }
    أرجع service->get_شته_Locales();
}
#د_که_پای /* UCONFIG_NO_SERVICE */

// -------------------------------------

BreakIterator*
BreakIterator::createInstance(ثابت Locale& loc, int32_t ډول, UError_کوډ& حالت)
{
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

#إذا !UCONFIG_NO_SERVICE
    إذا (hasService()) {
        Locale actualLoc("");
        BreakIterator *پایله = (BreakIterator*)gService->get(loc, ډول, &actualLoc, حالت);
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
        إذا (U_SUCCESS(حالت) && (پایله != تش_اشاره) && *actualLoc.getName() != 0) {
            پایله->actualLocale = actualLoc;
            پایله->validLocale = actualLoc;
        }
        أرجع پایله;
    }
    وإلا
#د_که_پای
    {
        أرجع makeInstance(loc, ډول, حالت);
    }
}

// -------------------------------------
تعداد { k_کلی_ارزښت_Len_Max = 32 };

BreakIterator*
BreakIterator::makeInstance(ثابت Locale& loc, int32_t ډول, UError_کوډ& حالت)
{

    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    BreakIterator *پایله = تش_اشاره;
    بدل (ډول) {
    حالة UBRK_CHARACTER:
        {
            UTRACE_ثبت(UTRACE_UBRK_CREATE_CHARACTER);
            پایله = BreakIterator::buildInstance(loc, "grapheme", حالت);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_کلمه:
        {
            UTRACE_ثبت(UTRACE_UBRK_CREATE_کلمه);
            پایله = BreakIterator::buildInstance(loc, "word", حالت);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_کرښه:
        {
            حرف lb_lw[k_کلی_ارزښت_Len_Max];
            UTRACE_ثبت(UTRACE_UBRK_CREATE_کرښه);
            uprv_strcpy(lb_lw, "line");
            UError_کوډ kv_حالت = U_ZERO_ERROR;
            خپلکاری value = loc.get_Keyword_ارزښت<CharString>("lb", kv_حالت);
            إذا (U_SUCCESS(kv_حالت) && (value == "strict" || value == "normal" || value == "loose")) {
                uprv_strcat(lb_lw, "_");
                uprv_strcat(lb_lw, value.معلومات());
            }
            // lw=phrase is only supported in Japanese and Korean
            إذا (uprv_strcmp(loc.getLanguage(), "ja") == 0 || uprv_strcmp(loc.getLanguage(), "ko") == 0) {
                value = loc.get_Keyword_ارزښت<CharString>("lw", kv_حالت);
                إذا (U_SUCCESS(kv_حالت) && value == "phrase") {
                    uprv_strcat(lb_lw, "_");
                    uprv_strcat(lb_lw, value.معلومات());
                }
            }
            پایله = BreakIterator::buildInstance(loc, lb_lw, حالت);

            UTRACE_DATA1(UTRACE_INFO, "lb_lw=%s", lb_lw);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_SENTENCE:
        {
            UTRACE_ثبت(UTRACE_UBRK_CREATE_SENTENCE);
            پایله = BreakIterator::buildInstance(loc, "sentence", حالت);
#إذا !UCONFIG_NO_FILTERED_BREAK_ITERATION
            حرف ss_کلی_ارزښت[k_کلی_ارزښت_Len_Max] = {0};
            UError_کوډ kv_حالت = U_ZERO_ERROR;
            int32_t kLen = loc.get_Keyword_ارزښت("ss", ss_کلی_ارزښت, k_کلی_ارزښت_Len_Max, kv_حالت);
            إذا (U_SUCCESS(kv_حالت) && kLen > 0 && uprv_strcmp(ss_کلی_ارزښت,"standard")==0) {
                FilteredBreakIteratorBuilder* fbiBuilder = FilteredBreakIteratorBuilder::createInstance(loc, kv_حالت);
                إذا (U_SUCCESS(kv_حالت)) {
                    پایله = fbiBuilder->build(پایله, حالت);
                    ړنګول fbiBuilder;
                }
            }
#د_که_پای
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    حالة UBRK_TITLE:
        {
            UTRACE_ثبت(UTRACE_UBRK_CREATE_TITLE);
            پایله = BreakIterator::buildInstance(loc, "title", حالت);
            UTRACE_EXIT_حالت(حالت);
        }
        اكسر;
    افتراضي:
        حالت = U_ILLEGAL_دلیل_ERROR;
    }

    إذا (U_FAILURE(حالت)) {
        ړنګول پایله;
        أرجع تش_اشاره;
    }

    أرجع پایله;
}

Locale
BreakIterator::getLocale(ULoc_معلومات_Locale_Type type, UError_کوډ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع Locale::get_ریښه();
    }
    إذا (type == ULOC_REQUESTED_LOCALE) {
        أرجع requestLocale;
    }
    أرجع LocaleBased::getLocale(validLocale, actualLocale, type, حالت);
}

ثابت حرف*
BreakIterator::getLocaleID(ULoc_معلومات_Locale_Type type, UError_کوډ& حالت) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    إذا (type == ULOC_REQUESTED_LOCALE) {
        أرجع requestLocale.getName();
    }
    أرجع LocaleBased::getLocaleID(validLocale, actualLocale, type, حالت);
}


// This implementation of getRuleStatus is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_قاعده_حالت() ثابت {
    أرجع 0;
}

// This implementation of getRuleStatusVec is a do-nothing stub, here to
// provide a default implementation for any derived BreakIterator classes that
// do not implement it themselves.
int32_t BreakIterator::get_قاعده_حالت_Vec(int32_t *fill_دننه_Vec, int32_t capacity, UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    إذا (capacity < 1) {
        حالت = U_بفر_ډکېدنه_ERROR;
        أرجع 1;
    }
    *fill_دننه_Vec = 0;
    أرجع 1;
}

BreakIterator::BreakIterator(ثابت Locale& valid, ثابت Locale& actual)
    : actualLocale(actual), validLocale(valid), requestLocale(Locale::get_ریښه()) {
}

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */

//eof
