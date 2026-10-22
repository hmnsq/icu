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
صنف ټولیز_String_Break_Iterator : عام BreakIterator {
عام:
    ټولیز_String_Break_Iterator() : BreakIterator(), length(0) {}
    ~ټولیز_String_Break_Iterator() override;
    منطقي چلوونکی==(ثابت BreakIterator&) ثابت override;
    ټولیز_String_Break_Iterator *clone() ثابت override;
    ساكن UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ثابت override;
    CharacterIterator& get_متن() ثابت U_LIFETIME_BOUND override;
    UText* getUText(UText* fill_دننه U_LIFETIME_BOUND, UError_کوډ& error_کوډ) ثابت override;
    فراغ  سېټ_متن(ثابت UnicodeString &متن) override;
    فراغ  سېټ_متن(UText *متن, UError_کوډ &error_کوډ) override;
    فراغ  adopt_متن(CharacterIterator* it) override;
    int32_t first() override;
    int32_t وروستی() override;
    int32_t پخوانی() override;
    int32_t next() override;
    int32_t current() ثابت override;
    int32_t following(int32_t دننه_واټن) override;
    int32_t preceding(int32_t دننه_واټن) override;
    UBool isBoundary(int32_t دننه_واټن) override;
    int32_t next(int32_t n) override;
    ټولیز_String_Break_Iterator *create_بفر_Clone(فراغ *ډېری_بفر, int32_t &بفر_اندازه_2,
                                                UError_کوډ &error_کوډ) override;
    ټولیز_String_Break_Iterator& refresh_ننوت_متن(UText* input,
                                               UError_کوډ& error_کوډ) U_LIFETIME_BOUND override;

خاص:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ټولیز_String_Break_Iterator)

ټولیز_String_Break_Iterator::~ټولیز_String_Break_Iterator() {}
منطقي ټولیز_String_Break_Iterator::چلوونکی==(ثابت BreakIterator&) ثابت { أرجع خطأ; }
ټولیز_String_Break_Iterator *ټولیز_String_Break_Iterator::clone() ثابت { أرجع تش_اشاره; }

CharacterIterator& ټولیز_String_Break_Iterator::get_متن() ثابت U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *ټولیز_String_Break_Iterator::getUText(UText * /*fillIn*/, UError_کوډ &error_کوډ) ثابت {
    إذا (U_SUCCESS(error_کوډ)) {
        error_کوډ = U_UNSUPPORTED_ERROR;
    }
    أرجع تش_اشاره;
}

فراغ  ټولیز_String_Break_Iterator::سېټ_متن(ثابت UnicodeString &متن) {
    length = متن.length();
}
فراغ  ټولیز_String_Break_Iterator::سېټ_متن(UText *متن, UError_کوډ &error_کوډ) {
    إذا (U_SUCCESS(error_کوډ)) {
        int64_t length64 = utext_native_اوږدتیا(متن);
        إذا (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } وإلا {
            error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
        }
    }
}
فراغ  ټولیز_String_Break_Iterator::adopt_متن(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t ټولیز_String_Break_Iterator::first() { أرجع 0; }
int32_t ټولیز_String_Break_Iterator::وروستی() { أرجع length; }
int32_t ټولیز_String_Break_Iterator::پخوانی() { أرجع 0; }
int32_t ټولیز_String_Break_Iterator::next() { أرجع length; }
int32_t ټولیز_String_Break_Iterator::current() ثابت { أرجع 0; }
int32_t ټولیز_String_Break_Iterator::following(int32_t /*offset*/) { أرجع length; }
int32_t ټولیز_String_Break_Iterator::preceding(int32_t /*offset*/) { أرجع 0; }
UBool ټولیز_String_Break_Iterator::isBoundary(int32_t /*offset*/) { أرجع خطأ; }
int32_t ټولیز_String_Break_Iterator::next(int32_t /*n*/) { أرجع length; }

ټولیز_String_Break_Iterator *ټولیز_String_Break_Iterator::create_بفر_Clone(
        فراغ * /*stackBuffer*/, int32_t & /*BufferSize*/, UError_کوډ &error_کوډ) {
    إذا (U_SUCCESS(error_کوډ)) {
        error_کوډ = U_UNSUPPORTED_ERROR;
    }
    أرجع تش_اشاره;
}
ټولیز_String_Break_Iterator&
ټولیز_String_Break_Iterator::refresh_ننوت_متن(UText* /*ننوت*/,
                                           UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    إذا (U_SUCCESS(error_کوډ)) {
        error_کوډ = U_UNSUPPORTED_ERROR;
    }
    أرجع *هذا;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ثابت Locale* locale,
                                              ثابت حرف* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              Local_اشاره_ګر<BreakIterator>& ownedIter,
                                              UError_کوډ& error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    options &= U_TITLECASE_ITERATOR_ماسک;
    إذا (options != 0 && iter != تش_اشاره) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    إذا (iter == تش_اشاره) {
        بدل (options) {
        حالة 0:
            iter = BreakIterator::create_کلمه_Instance(
                locale != تش_اشاره ? *locale : Locale(locID), error_کوډ);
            اكسر;
        حالة U_TITLECASE_ټولیز_STRING:
            iter = جديد ټولیز_String_Break_Iterator();
            إذا (iter == تش_اشاره) {
                error_کوډ = U_حافظه_ویش_ERROR;
            }
            اكسر;
        حالة U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != تش_اشاره ? *locale : Locale(locID), error_کوډ);
            اكسر;
        افتراضي:
            error_کوډ = U_ILLEGAL_دلیل_ERROR;
            اكسر;
        }
        ownedIter.adoptInstead(iter);
    }
    أرجع iter;
}

int32_t CaseMap::منزل_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت char16_t *src, int32_t src_اوږدتیا,
        char16_t *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کوډ &error_کوډ) {
    Local_اشاره_ګر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(تش_اشاره, locale, options, iter, ownedIter, error_کوډ);
    إذا(iter==تش_اشاره) {
        أرجع 0;
    }
    UnicodeString s(src_اوږدتیا<0, src, src_اوږدتیا);
    iter->سېټ_متن(s);
    أرجع ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ustrcase_دننی_منزل_Title, edits, error_کوډ);
}

U_NAMESPACE_پای_ټکی

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_str_منزل_Title(char16_t *dest, int32_t dest_ظرفیت,
             ثابت char16_t *src, int32_t src_اوږدتیا,
             UBreakIterator *titleIter,
             ثابت حرف *locale,
             UError_کوډ *p_Error_کوډ) {
    Local_اشاره_ګر<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        تش_اشاره, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *p_Error_کوډ);
    إذا (iter == تش_اشاره) {
        أرجع 0;
    }
    UnicodeString s(src_اوږدتیا<0, src, src_اوږدتیا);
    iter->سېټ_متن(s);
    أرجع ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ustrcase_دننی_منزل_Title, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_منزل_Title(UCaseMap *csm,
                 char16_t *dest, int32_t dest_ظرفیت,
                 ثابت char16_t *src, int32_t src_اوږدتیا,
                 UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    خپلکاری *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا (impl->iter == تش_اشاره) {
        Local_اشاره_ګر<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            تش_اشاره, impl->locale, impl->options, تش_اشاره, ownedIter, *p_Error_کوډ);
        إذا (iter == تش_اشاره) {
            أرجع 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(src_اوږدتیا<0, src, src_اوږدتیا);
    impl->iter->سېټ_متن(s);
    أرجع ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ustrcase_دننی_منزل_Title, تش_اشاره, *p_Error_کوډ);
}

#د_که_پای  // !UCONFIG_NO_BREAK_ITERATION
