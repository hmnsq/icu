// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ustr_titlecase_brkiter.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011may30
*   created by: Markus W. Scherer
*
*   Titlecasing functions that are based on BreakIterator
*   were moved here to break dependency cycles among parts of the common library.
*/

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/brkiter.h"
#ضمن "unicode/casemap.h"
#ضمن "unicode/chariter.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/ubrk.h"
#ضمن "unicode/ucasemap.h"
#ضمن "unicode/utext.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ucase.h"
#ضمن "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
صنف کامل_String_Break_Iterator : عام BreakIterator {
عام:
    کامل_String_Break_Iterator() : BreakIterator(), length(0) {}
    ~کامل_String_Break_Iterator() override;
    منطقي عملگر==(ثابت BreakIterator&) ثابت override;
    کامل_String_Break_Iterator *clone() ثابت override;
    ساكن UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ثابت override;
    CharacterIterator& get_متن() ثابت U_LIFETIME_BOUND override;
    UText* getUText(UText* fill_درون U_LIFETIME_BOUND, UError_کد& error_کد) ثابت override;
    فراغ  مجموعه_متن(ثابت UnicodeString &متن) override;
    فراغ  مجموعه_متن(UText *متن, UError_کد &error_کد) override;
    فراغ  adopt_متن(CharacterIterator* it) override;
    int32_t first() override;
    int32_t آخرین() override;
    int32_t قبلی() override;
    int32_t next() override;
    int32_t current() ثابت override;
    int32_t following(int32_t جابه_جایی) override;
    int32_t preceding(int32_t جابه_جایی) override;
    UBool isBoundary(int32_t جابه_جایی) override;
    int32_t next(int32_t n) override;
    کامل_String_Break_Iterator *create_میانگیر_Clone(فراغ *پشته_میانگیر, int32_t &میانگیر_اندازه_2,
                                                UError_کد &error_کد) override;
    کامل_String_Break_Iterator& refresh_درون_داد_متن(UText* input,
                                               UError_کد& error_کد) U_LIFETIME_BOUND override;

خاص:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(کامل_String_Break_Iterator)

کامل_String_Break_Iterator::~کامل_String_Break_Iterator() {}
منطقي کامل_String_Break_Iterator::عملگر==(ثابت BreakIterator&) ثابت { أرجع خطأ; }
کامل_String_Break_Iterator *کامل_String_Break_Iterator::clone() ثابت { أرجع اشاره_تهی; }

CharacterIterator& کامل_String_Break_Iterator::get_متن() ثابت U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *کامل_String_Break_Iterator::getUText(UText * /*fillIn*/, UError_کد &error_کد) ثابت {
    إذا (U_SUCCESS(error_کد)) {
        error_کد = U_UNSUPPORTED_ERROR;
    }
    أرجع اشاره_تهی;
}

فراغ  کامل_String_Break_Iterator::مجموعه_متن(ثابت UnicodeString &متن) {
    length = متن.length();
}
فراغ  کامل_String_Break_Iterator::مجموعه_متن(UText *متن, UError_کد &error_کد) {
    إذا (U_SUCCESS(error_کد)) {
        int64_t length64 = utext_native_درازی(متن);
        إذا (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } وإلا {
            error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
        }
    }
}
فراغ  کامل_String_Break_Iterator::adopt_متن(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t کامل_String_Break_Iterator::first() { أرجع 0; }
int32_t کامل_String_Break_Iterator::آخرین() { أرجع length; }
int32_t کامل_String_Break_Iterator::قبلی() { أرجع 0; }
int32_t کامل_String_Break_Iterator::next() { أرجع length; }
int32_t کامل_String_Break_Iterator::current() ثابت { أرجع 0; }
int32_t کامل_String_Break_Iterator::following(int32_t /*offset*/) { أرجع length; }
int32_t کامل_String_Break_Iterator::preceding(int32_t /*offset*/) { أرجع 0; }
UBool کامل_String_Break_Iterator::isBoundary(int32_t /*offset*/) { أرجع خطأ; }
int32_t کامل_String_Break_Iterator::next(int32_t /*n*/) { أرجع length; }

کامل_String_Break_Iterator *کامل_String_Break_Iterator::create_میانگیر_Clone(
        فراغ * /*stackBuffer*/, int32_t & /*BufferSize*/, UError_کد &error_کد) {
    إذا (U_SUCCESS(error_کد)) {
        error_کد = U_UNSUPPORTED_ERROR;
    }
    أرجع اشاره_تهی;
}
کامل_String_Break_Iterator&
کامل_String_Break_Iterator::refresh_درون_داد_متن(UText* /*درون داد*/,
                                           UError_کد& error_کد) U_LIFETIME_BOUND {
    إذا (U_SUCCESS(error_کد)) {
        error_کد = U_UNSUPPORTED_ERROR;
    }
    أرجع *هذا;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ثابت Locale* locale,
                                              ثابت حرف* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              Local_اشاره_گر<BreakIterator>& ownedIter,
                                              UError_کد& error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    options &= U_TITLECASE_ITERATOR_ماسک;
    إذا (options != 0 && iter != اشاره_تهی) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    إذا (iter == اشاره_تهی) {
        بدل (options) {
        حالة 0:
            iter = BreakIterator::create_کلمه_Instance(
                locale != اشاره_تهی ? *locale : Locale(locID), error_کد);
            اكسر;
        حالة U_TITLECASE_کامل_STRING:
            iter = جديد کامل_String_Break_Iterator();
            إذا (iter == اشاره_تهی) {
                error_کد = U_حافظه_تخصیص_ERROR;
            }
            اكسر;
        حالة U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != اشاره_تهی ? *locale : Locale(locID), error_کد);
            اكسر;
        افتراضي:
            error_کد = U_ILLEGAL_آرگومان_ERROR;
            اكسر;
        }
        ownedIter.adoptInstead(iter);
    }
    أرجع iter;
}

int32_t CaseMap::مقصد_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت char16_t *src, int32_t src_درازی,
        char16_t *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کد &error_کد) {
    Local_اشاره_گر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(اشاره_تهی, locale, options, iter, ownedIter, error_کد);
    إذا(iter==اشاره_تهی) {
        أرجع 0;
    }
    UnicodeString s(src_درازی<0, src, src_درازی);
    iter->مجموعه_متن(s);
    أرجع ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_ظرفیت,
        src, src_درازی,
        ustrcase_داخلی_مقصد_Title, edits, error_کد);
}

U_NAMESPACE_انتها

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_str_مقصد_Title(char16_t *dest, int32_t dest_ظرفیت,
             ثابت char16_t *src, int32_t src_درازی,
             UBreakIterator *titleIter,
             ثابت حرف *locale,
             UError_کد *p_Error_کد) {
    Local_اشاره_گر<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        اشاره_تهی, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *p_Error_کد);
    إذا (iter == اشاره_تهی) {
        أرجع 0;
    }
    UnicodeString s(src_درازی<0, src, src_درازی);
    iter->مجموعه_متن(s);
    أرجع ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, dest_ظرفیت,
        src, src_درازی,
        ustrcase_داخلی_مقصد_Title, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
ucasemap_مقصد_Title(UCaseMap *csm,
                 char16_t *dest, int32_t dest_ظرفیت,
                 ثابت char16_t *src, int32_t src_درازی,
                 UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا (impl->iter == اشاره_تهی) {
        Local_اشاره_گر<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            اشاره_تهی, impl->locale, impl->options, اشاره_تهی, ownedIter, *p_Error_کد);
        إذا (iter == اشاره_تهی) {
            أرجع 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(src_درازی<0, src, src_درازی);
    impl->iter->مجموعه_متن(s);
    أرجع ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, dest_ظرفیت,
        src, src_درازی,
        ustrcase_داخلی_مقصد_Title, اشاره_تهی, *p_Error_کد);
}

#پایان_اگر  // !UCONFIG_NO_BREAK_ITERATION
