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
صنف سڄو_String_Break_Iterator : عام BreakIterator {
عام:
    سڄو_String_Break_Iterator() : BreakIterator(), length(0) {}
    ~سڄو_String_Break_Iterator() override;
    منطقي عامل==(ثابت BreakIterator&) ثابت override;
    سڄو_String_Break_Iterator *clone() ثابت override;
    ساكن UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ثابت override;
    CharacterIterator& get_متن() ثابت U_LIFETIME_BOUND override;
    UText* getUText(UText* fill_اندريون U_LIFETIME_BOUND, UError_ڪوڊ& error_ڪوڊ) ثابت override;
    فراغ  سيٽ_متن(ثابت UnicodeString &متن) override;
    فراغ  سيٽ_متن(UText *متن, UError_ڪوڊ &error_ڪوڊ) override;
    فراغ  adopt_متن(CharacterIterator* it) override;
    int32_t first() override;
    int32_t آخري() override;
    int32_t اڳيون() override;
    int32_t next() override;
    int32_t current() ثابت override;
    int32_t following(int32_t وٿي) override;
    int32_t preceding(int32_t وٿي) override;
    UBool isBoundary(int32_t وٿي) override;
    int32_t next(int32_t n) override;
    سڄو_String_Break_Iterator *create_بفر_Clone(فراغ *ڍير_بفر, int32_t &بفر_سائيز_2,
                                                UError_ڪوڊ &error_ڪوڊ) override;
    سڄو_String_Break_Iterator& refresh_داخلا_متن(UText* input,
                                               UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND override;

خاص:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(سڄو_String_Break_Iterator)

سڄو_String_Break_Iterator::~سڄو_String_Break_Iterator() {}
منطقي سڄو_String_Break_Iterator::عامل==(ثابت BreakIterator&) ثابت { أرجع خطأ; }
سڄو_String_Break_Iterator *سڄو_String_Break_Iterator::clone() ثابت { أرجع خالي_اشارو; }

CharacterIterator& سڄو_String_Break_Iterator::get_متن() ثابت U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *سڄو_String_Break_Iterator::getUText(UText * /*fillIn*/, UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_SUCCESS(error_ڪوڊ)) {
        error_ڪوڊ = U_UNSUPPORTED_ERROR;
    }
    أرجع خالي_اشارو;
}

فراغ  سڄو_String_Break_Iterator::سيٽ_متن(ثابت UnicodeString &متن) {
    length = متن.length();
}
فراغ  سڄو_String_Break_Iterator::سيٽ_متن(UText *متن, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_SUCCESS(error_ڪوڊ)) {
        int64_t length64 = utext_native_ڊيگهائي(متن);
        إذا (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } وإلا {
            error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
        }
    }
}
فراغ  سڄو_String_Break_Iterator::adopt_متن(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t سڄو_String_Break_Iterator::first() { أرجع 0; }
int32_t سڄو_String_Break_Iterator::آخري() { أرجع length; }
int32_t سڄو_String_Break_Iterator::اڳيون() { أرجع 0; }
int32_t سڄو_String_Break_Iterator::next() { أرجع length; }
int32_t سڄو_String_Break_Iterator::current() ثابت { أرجع 0; }
int32_t سڄو_String_Break_Iterator::following(int32_t /*offset*/) { أرجع length; }
int32_t سڄو_String_Break_Iterator::preceding(int32_t /*offset*/) { أرجع 0; }
UBool سڄو_String_Break_Iterator::isBoundary(int32_t /*offset*/) { أرجع خطأ; }
int32_t سڄو_String_Break_Iterator::next(int32_t /*n*/) { أرجع length; }

سڄو_String_Break_Iterator *سڄو_String_Break_Iterator::create_بفر_Clone(
        فراغ * /*stackBuffer*/, int32_t & /*BufferSize*/, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_SUCCESS(error_ڪوڊ)) {
        error_ڪوڊ = U_UNSUPPORTED_ERROR;
    }
    أرجع خالي_اشارو;
}
سڄو_String_Break_Iterator&
سڄو_String_Break_Iterator::refresh_داخلا_متن(UText* /*داخلا*/,
                                           UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    إذا (U_SUCCESS(error_ڪوڊ)) {
        error_ڪوڊ = U_UNSUPPORTED_ERROR;
    }
    أرجع *هذا;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ثابت Locale* locale,
                                              ثابت حرف* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              Local_اشارو<BreakIterator>& ownedIter,
                                              UError_ڪوڊ& error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    options &= U_TITLECASE_ITERATOR_نقاب;
    إذا (options != 0 && iter != خالي_اشارو) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    إذا (iter == خالي_اشارو) {
        بدل (options) {
        حالة 0:
            iter = BreakIterator::create_لفظ_Instance(
                locale != خالي_اشارو ? *locale : Locale(locID), error_ڪوڊ);
            اكسر;
        حالة U_TITLECASE_سڄو_STRING:
            iter = جديد سڄو_String_Break_Iterator();
            إذا (iter == خالي_اشارو) {
                error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            }
            اكسر;
        حالة U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != خالي_اشارو ? *locale : Locale(locID), error_ڪوڊ);
            اكسر;
        افتراضي:
            error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
            اكسر;
        }
        ownedIter.adoptInstead(iter);
    }
    أرجع iter;
}

int32_t CaseMap::منزل_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت char16_t *src, int32_t src_ڊيگهائي,
        char16_t *dest, int32_t dest_صلاحيت, Edits *edits,
        UError_ڪوڊ &error_ڪوڊ) {
    Local_اشارو<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(خالي_اشارو, locale, options, iter, ownedIter, error_ڪوڊ);
    إذا(iter==خالي_اشارو) {
        أرجع 0;
    }
    UnicodeString s(src_ڊيگهائي<0, src, src_ڊيگهائي);
    iter->سيٽ_متن(s);
    أرجع ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ustrcase_اندروني_منزل_Title, edits, error_ڪوڊ);
}

U_NAMESPACE_پڇاڙي

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_str_منزل_Title(char16_t *dest, int32_t dest_صلاحيت,
             ثابت char16_t *src, int32_t src_ڊيگهائي,
             UBreakIterator *titleIter,
             ثابت حرف *locale,
             UError_ڪوڊ *p_Error_ڪوڊ) {
    Local_اشارو<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        خالي_اشارو, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *p_Error_ڪوڊ);
    إذا (iter == خالي_اشارو) {
        أرجع 0;
    }
    UnicodeString s(src_ڊيگهائي<0, src, src_ڊيگهائي);
    iter->سيٽ_متن(s);
    أرجع ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ustrcase_اندروني_منزل_Title, *p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_منزل_Title(UCaseMap *csm,
                 char16_t *dest, int32_t dest_صلاحيت,
                 ثابت char16_t *src, int32_t src_ڊيگهائي,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    خودڪار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا (impl->iter == خالي_اشارو) {
        Local_اشارو<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            خالي_اشارو, impl->locale, impl->options, خالي_اشارو, ownedIter, *p_Error_ڪوڊ);
        إذا (iter == خالي_اشارو) {
            أرجع 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(src_ڊيگهائي<0, src, src_ڊيگهائي);
    impl->iter->سيٽ_متن(s);
    أرجع ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ustrcase_اندروني_منزل_Title, خالي_اشارو, *p_Error_ڪوڊ);
}

#جيڪڏهن_پورو  // !UCONFIG_NO_BREAK_ITERATION
