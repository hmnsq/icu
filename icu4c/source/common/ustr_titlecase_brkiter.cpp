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
صنف سارا_String_Break_Iterator : عام BreakIterator {
عام:
    سارا_String_Break_Iterator() : BreakIterator(), length(0) {}
    ~سارا_String_Break_Iterator() override;
    منطقي عامل==(ثابت BreakIterator&) ثابت override;
    سارا_String_Break_Iterator *clone() ثابت override;
    ساكن UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ثابت override;
    CharacterIterator& get_متن() ثابت U_LIFETIME_BOUND override;
    UText* getUText(UText* fill_وچ U_LIFETIME_BOUND, UError_کوڈ& error_کوڈ) ثابت override;
    فراغ  سیٹ_متن(ثابت UnicodeString &متن) override;
    فراغ  سیٹ_متن(UText *متن, UError_کوڈ &error_کوڈ) override;
    فراغ  adopt_متن(CharacterIterator* it) override;
    int32_t first() override;
    int32_t آخری() override;
    int32_t پچھلا() override;
    int32_t next() override;
    int32_t current() ثابت override;
    int32_t following(int32_t وتھ) override;
    int32_t preceding(int32_t وتھ) override;
    UBool isBoundary(int32_t وتھ) override;
    int32_t next(int32_t n) override;
    سارا_String_Break_Iterator *create_بفر_Clone(فراغ *ڈھیر_بفر, int32_t &بفر_ناپ_2,
                                                UError_کوڈ &error_کوڈ) override;
    سارا_String_Break_Iterator& refresh_داخلہ_متن(UText* input,
                                               UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND override;

خاص:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(سارا_String_Break_Iterator)

سارا_String_Break_Iterator::~سارا_String_Break_Iterator() {}
منطقي سارا_String_Break_Iterator::عامل==(ثابت BreakIterator&) ثابت { أرجع خطأ; }
سارا_String_Break_Iterator *سارا_String_Break_Iterator::clone() ثابت { أرجع خالی_سوچک; }

CharacterIterator& سارا_String_Break_Iterator::get_متن() ثابت U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *سارا_String_Break_Iterator::getUText(UText * /*fillIn*/, UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_SUCCESS(error_کوڈ)) {
        error_کوڈ = U_UNSUPPORTED_ERROR;
    }
    أرجع خالی_سوچک;
}

فراغ  سارا_String_Break_Iterator::سیٹ_متن(ثابت UnicodeString &متن) {
    length = متن.length();
}
فراغ  سارا_String_Break_Iterator::سیٹ_متن(UText *متن, UError_کوڈ &error_کوڈ) {
    إذا (U_SUCCESS(error_کوڈ)) {
        int64_t length64 = utext_native_لمائی(متن);
        إذا (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } وإلا {
            error_کوڈ = U_اشاریہ_OUTOFBOUNDS_ERROR;
        }
    }
}
فراغ  سارا_String_Break_Iterator::adopt_متن(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t سارا_String_Break_Iterator::first() { أرجع 0; }
int32_t سارا_String_Break_Iterator::آخری() { أرجع length; }
int32_t سارا_String_Break_Iterator::پچھلا() { أرجع 0; }
int32_t سارا_String_Break_Iterator::next() { أرجع length; }
int32_t سارا_String_Break_Iterator::current() ثابت { أرجع 0; }
int32_t سارا_String_Break_Iterator::following(int32_t /*offset*/) { أرجع length; }
int32_t سارا_String_Break_Iterator::preceding(int32_t /*offset*/) { أرجع 0; }
UBool سارا_String_Break_Iterator::isBoundary(int32_t /*offset*/) { أرجع خطأ; }
int32_t سارا_String_Break_Iterator::next(int32_t /*n*/) { أرجع length; }

سارا_String_Break_Iterator *سارا_String_Break_Iterator::create_بفر_Clone(
        فراغ * /*stackBuffer*/, int32_t & /*BufferSize*/, UError_کوڈ &error_کوڈ) {
    إذا (U_SUCCESS(error_کوڈ)) {
        error_کوڈ = U_UNSUPPORTED_ERROR;
    }
    أرجع خالی_سوچک;
}
سارا_String_Break_Iterator&
سارا_String_Break_Iterator::refresh_داخلہ_متن(UText* /*داخلہ*/,
                                           UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    إذا (U_SUCCESS(error_کوڈ)) {
        error_کوڈ = U_UNSUPPORTED_ERROR;
    }
    أرجع *هذا;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ثابت Locale* locale,
                                              ثابت حرف* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              Local_اشارہ_گر<BreakIterator>& ownedIter,
                                              UError_کوڈ& error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    options &= U_TITLECASE_ITERATOR_نقاب;
    إذا (options != 0 && iter != خالی_سوچک) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    إذا (iter == خالی_سوچک) {
        بدل (options) {
        حالة 0:
            iter = BreakIterator::create_لفظ_Instance(
                locale != خالی_سوچک ? *locale : Locale(locID), error_کوڈ);
            اكسر;
        حالة U_TITLECASE_سارا_STRING:
            iter = جديد سارا_String_Break_Iterator();
            إذا (iter == خالی_سوچک) {
                error_کوڈ = U_حافظہ_ونڈ_ERROR;
            }
            اكسر;
        حالة U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != خالی_سوچک ? *locale : Locale(locID), error_کوڈ);
            اكسر;
        افتراضي:
            error_کوڈ = U_ILLEGAL_دلیل_ERROR;
            اكسر;
        }
        ownedIter.adoptInstead(iter);
    }
    أرجع iter;
}

int32_t CaseMap::منزل_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت char16_t *src, int32_t src_لمائی,
        char16_t *dest, int32_t dest_سمرتھا, Edits *edits,
        UError_کوڈ &error_کوڈ) {
    Local_اشارہ_گر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(خالی_سوچک, locale, options, iter, ownedIter, error_کوڈ);
    إذا(iter==خالی_سوچک) {
        أرجع 0;
    }
    UnicodeString s(src_لمائی<0, src, src_لمائی);
    iter->سیٹ_متن(s);
    أرجع ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_سمرتھا,
        src, src_لمائی,
        ustrcase_اندرونی_منزل_Title, edits, error_کوڈ);
}

U_NAMESPACE_سرا

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_str_منزل_Title(char16_t *dest, int32_t dest_سمرتھا,
             ثابت char16_t *src, int32_t src_لمائی,
             UBreakIterator *titleIter,
             ثابت حرف *locale,
             UError_کوڈ *p_Error_کوڈ) {
    Local_اشارہ_گر<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        خالی_سوچک, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *p_Error_کوڈ);
    إذا (iter == خالی_سوچک) {
        أرجع 0;
    }
    UnicodeString s(src_لمائی<0, src, src_لمائی);
    iter->سیٹ_متن(s);
    أرجع ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, dest_سمرتھا,
        src, src_لمائی,
        ustrcase_اندرونی_منزل_Title, *p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_منزل_Title(UCaseMap *csm,
                 char16_t *dest, int32_t dest_سمرتھا,
                 ثابت char16_t *src, int32_t src_لمائی,
                 UError_کوڈ *p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا (impl->iter == خالی_سوچک) {
        Local_اشارہ_گر<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            خالی_سوچک, impl->locale, impl->options, خالی_سوچک, ownedIter, *p_Error_کوڈ);
        إذا (iter == خالی_سوچک) {
            أرجع 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(src_لمائی<0, src, src_لمائی);
    impl->iter->سیٹ_متن(s);
    أرجع ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, dest_سمرتھا,
        src, src_لمائی,
        ustrcase_اندرونی_منزل_Title, خالی_سوچک, *p_Error_کوڈ);
}

#جے_انت  // !UCONFIG_NO_BREAK_ITERATION
