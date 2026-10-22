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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/brkiter.h"
#شامل "unicode/casemap.h"
#شامل "unicode/chariter.h"
#شامل "unicode/localpointer.h"
#شامل "unicode/ubrk.h"
#شامل "unicode/ucasemap.h"
#شامل "unicode/utext.h"
#شامل "cmemory.h"
#شامل "uassert.h"
#شامل "ucase.h"
#شامل "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
جماعت پورا_String_Break_Iterator : عوامی BreakIterator {
عوامی:
    پورا_String_Break_Iterator() : BreakIterator(), length(0) {}
    ~پورا_String_Break_Iterator() override;
    منطقی عامل==(ثابت BreakIterator&) ثابت override;
    پورا_String_Break_Iterator *clone() ثابت override;
    ساکن UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ثابت override;
    CharacterIterator& get_متن() ثابت U_LIFETIME_BOUND override;
    UText* getUText(UText* fill_اندر U_LIFETIME_BOUND, UError_رمز& error_رمز) ثابت override;
    خالی  سیٹ_متن(ثابت UnicodeString &متن) override;
    خالی  سیٹ_متن(UText *متن, UError_رمز &error_رمز) override;
    خالی  adopt_متن(CharacterIterator* it) override;
    int32_t first() override;
    int32_t آخری() override;
    int32_t پچھلا_2() override;
    int32_t next() override;
    int32_t current() ثابت override;
    int32_t following(int32_t انحراف) override;
    int32_t preceding(int32_t انحراف) override;
    UBool isBoundary(int32_t انحراف) override;
    int32_t next(int32_t n) override;
    پورا_String_Break_Iterator *create_بفر_Clone(خالی *ڈھیر_بفر, int32_t &بفر_سائز_2,
                                                UError_رمز &error_رمز) override;
    پورا_String_Break_Iterator& refresh_داخلہ_متن(UText* input,
                                               UError_رمز& error_رمز) U_LIFETIME_BOUND override;

نجی:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(پورا_String_Break_Iterator)

پورا_String_Break_Iterator::~پورا_String_Break_Iterator() {}
منطقی پورا_String_Break_Iterator::عامل==(ثابت BreakIterator&) ثابت { واپس جھوٹ; }
پورا_String_Break_Iterator *پورا_String_Break_Iterator::clone() ثابت { واپس خالی_اشاریہ; }

CharacterIterator& پورا_String_Break_Iterator::get_متن() ثابت U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *پورا_String_Break_Iterator::getUText(UText * /*fillIn*/, UError_رمز &error_رمز) ثابت {
    اگر (U_SUCCESS(error_رمز)) {
        error_رمز = U_UNSUPPORTED_ERROR;
    }
    واپس خالی_اشاریہ;
}

خالی  پورا_String_Break_Iterator::سیٹ_متن(ثابت UnicodeString &متن) {
    length = متن.length();
}
خالی  پورا_String_Break_Iterator::سیٹ_متن(UText *متن, UError_رمز &error_رمز) {
    اگر (U_SUCCESS(error_رمز)) {
        int64_t length64 = utext_native_لمبائی(متن);
        اگر (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } ورنہ {
            error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
        }
    }
}
خالی  پورا_String_Break_Iterator::adopt_متن(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t پورا_String_Break_Iterator::first() { واپس 0; }
int32_t پورا_String_Break_Iterator::آخری() { واپس length; }
int32_t پورا_String_Break_Iterator::پچھلا_2() { واپس 0; }
int32_t پورا_String_Break_Iterator::next() { واپس length; }
int32_t پورا_String_Break_Iterator::current() ثابت { واپس 0; }
int32_t پورا_String_Break_Iterator::following(int32_t /*offset*/) { واپس length; }
int32_t پورا_String_Break_Iterator::preceding(int32_t /*offset*/) { واپس 0; }
UBool پورا_String_Break_Iterator::isBoundary(int32_t /*offset*/) { واپس جھوٹ; }
int32_t پورا_String_Break_Iterator::next(int32_t /*n*/) { واپس length; }

پورا_String_Break_Iterator *پورا_String_Break_Iterator::create_بفر_Clone(
        خالی * /*stackBuffer*/, int32_t & /*BufferSize*/, UError_رمز &error_رمز) {
    اگر (U_SUCCESS(error_رمز)) {
        error_رمز = U_UNSUPPORTED_ERROR;
    }
    واپس خالی_اشاریہ;
}
پورا_String_Break_Iterator&
پورا_String_Break_Iterator::refresh_داخلہ_متن(UText* /*داخلہ*/,
                                           UError_رمز& error_رمز) U_LIFETIME_BOUND {
    اگر (U_SUCCESS(error_رمز)) {
        error_رمز = U_UNSUPPORTED_ERROR;
    }
    واپس *یہ;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ثابت Locale* locale,
                                              ثابت حرف* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              Local_اشارہ_گر<BreakIterator>& ownedIter,
                                              UError_رمز& error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    options &= U_TITLECASE_ITERATOR_نقاب;
    اگر (options != 0 && iter != خالی_اشاریہ) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    اگر (iter == خالی_اشاریہ) {
        انتخاب (options) {
        حالت 0:
            iter = BreakIterator::create_لفظ_Instance(
                locale != خالی_اشاریہ ? *locale : Locale(locID), error_رمز);
            توڑو;
        حالت U_TITLECASE_پورا_STRING:
            iter = نیا پورا_String_Break_Iterator();
            اگر (iter == خالی_اشاریہ) {
                error_رمز = U_حافظہ_تخصیص_ERROR;
            }
            توڑو;
        حالت U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != خالی_اشاریہ ? *locale : Locale(locID), error_رمز);
            توڑو;
        طےشدہ:
            error_رمز = U_ILLEGAL_دلیل_ERROR;
            توڑو;
        }
        ownedIter.adoptInstead(iter);
    }
    واپس iter;
}

int32_t CaseMap::مقصد_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت char16_t *src, int32_t src_لمبائی,
        char16_t *dest, int32_t dest_استعداد, Edits *edits,
        UError_رمز &error_رمز) {
    Local_اشارہ_گر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(خالی_اشاریہ, locale, options, iter, ownedIter, error_رمز);
    اگر(iter==خالی_اشاریہ) {
        واپس 0;
    }
    UnicodeString s(src_لمبائی<0, src, src_لمبائی);
    iter->سیٹ_متن(s);
    واپس ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_استعداد,
        src, src_لمبائی,
        ustrcase_اندرونی_مقصد_Title, edits, error_رمز);
}

U_NAMESPACE_انتہا

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_str_مقصد_Title(char16_t *dest, int32_t dest_استعداد,
             ثابت char16_t *src, int32_t src_لمبائی,
             UBreakIterator *titleIter,
             ثابت حرف *locale,
             UError_رمز *p_Error_رمز) {
    Local_اشارہ_گر<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        خالی_اشاریہ, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *p_Error_رمز);
    اگر (iter == خالی_اشاریہ) {
        واپس 0;
    }
    UnicodeString s(src_لمبائی<0, src, src_لمبائی);
    iter->سیٹ_متن(s);
    واپس ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, dest_استعداد,
        src, src_لمبائی,
        ustrcase_اندرونی_مقصد_Title, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
ucasemap_مقصد_Title(UCaseMap *csm,
                 char16_t *dest, int32_t dest_استعداد,
                 ثابت char16_t *src, int32_t src_لمبائی,
                 UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    اگر (impl->iter == خالی_اشاریہ) {
        Local_اشارہ_گر<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            خالی_اشاریہ, impl->locale, impl->options, خالی_اشاریہ, ownedIter, *p_Error_رمز);
        اگر (iter == خالی_اشاریہ) {
            واپس 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(src_لمبائی<0, src, src_لمبائی);
    impl->iter->سیٹ_متن(s);
    واپس ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, dest_استعداد,
        src, src_لمبائی,
        ustrcase_اندرونی_مقصد_Title, خالی_اشاریہ, *p_Error_رمز);
}

#اگر_ختم  // !UCONFIG_NO_BREAK_ITERATION
