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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/casemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/chariter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localpointer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubrk.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucasemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
ਵਰਗ ਸਾਰਾ_String_Break_Iterator : ਜਨਤਕ BreakIterator {
ਜਨਤਕ:
    ਸਾਰਾ_String_Break_Iterator() : BreakIterator(), length(0) {}
    ~ਸਾਰਾ_String_Break_Iterator() override;
    ਤਾਰਕਿਕ ਸੰਚਾਲਕ==(ਅਚਲ BreakIterator&) ਅਚਲ override;
    ਸਾਰਾ_String_Break_Iterator *clone() ਅਚਲ override;
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ਅਚਲ override;
    CharacterIterator& get_ਪਾਠ() ਅਚਲ U_LIFETIME_BOUND override;
    UText* getUText(UText* fill_ਵਿੱਚ U_LIFETIME_BOUND, UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ override;
    ਖਾਲੀ  ਸੈੱਟ_ਪਾਠ(ਅਚਲ UnicodeString &ਪਾਠ) override;
    ਖਾਲੀ  ਸੈੱਟ_ਪਾਠ(UText *ਪਾਠ, UError_ਕੋਡ &error_ਕੋਡ) override;
    ਖਾਲੀ  adopt_ਪਾਠ(CharacterIterator* it) override;
    int32_t first() override;
    int32_t ਆਖਰੀ() override;
    int32_t ਪਿਛਲਾ() override;
    int32_t next() override;
    int32_t current() ਅਚਲ override;
    int32_t following(int32_t ਵਿੱਥ) override;
    int32_t preceding(int32_t ਵਿੱਥ) override;
    UBool isBoundary(int32_t ਵਿੱਥ) override;
    int32_t next(int32_t n) override;
    ਸਾਰਾ_String_Break_Iterator *create_ਬਫਰ_Clone(ਖਾਲੀ *ਢੇਰ_ਬਫਰ, int32_t &ਬਫਰ_ਮਾਪ_2,
                                                UError_ਕੋਡ &error_ਕੋਡ) override;
    ਸਾਰਾ_String_Break_Iterator& refresh_ਨਿਵੇਸ਼_ਪਾਠ(UText* input,
                                               UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND override;

ਨਿਜੀ:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ਸਾਰਾ_String_Break_Iterator)

ਸਾਰਾ_String_Break_Iterator::~ਸਾਰਾ_String_Break_Iterator() {}
ਤਾਰਕਿਕ ਸਾਰਾ_String_Break_Iterator::ਸੰਚਾਲਕ==(ਅਚਲ BreakIterator&) ਅਚਲ { ਵਾਪਸ ਝੂਠ; }
ਸਾਰਾ_String_Break_Iterator *ਸਾਰਾ_String_Break_Iterator::clone() ਅਚਲ { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }

CharacterIterator& ਸਾਰਾ_String_Break_Iterator::get_ਪਾਠ() ਅਚਲ U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *ਸਾਰਾ_String_Break_Iterator::getUText(UText * /*fillIn*/, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
        error_ਕੋਡ = U_UNSUPPORTED_ERROR;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਖਾਲੀ  ਸਾਰਾ_String_Break_Iterator::ਸੈੱਟ_ਪਾਠ(ਅਚਲ UnicodeString &ਪਾਠ) {
    length = ਪਾਠ.length();
}
ਖਾਲੀ  ਸਾਰਾ_String_Break_Iterator::ਸੈੱਟ_ਪਾਠ(UText *ਪਾਠ, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
        int64_t length64 = utext_native_ਲੰਮਾਈ(ਪਾਠ);
        ਜੇ (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } ਨਹੀਂਤਾਂ {
            error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        }
    }
}
ਖਾਲੀ  ਸਾਰਾ_String_Break_Iterator::adopt_ਪਾਠ(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t ਸਾਰਾ_String_Break_Iterator::first() { ਵਾਪਸ 0; }
int32_t ਸਾਰਾ_String_Break_Iterator::ਆਖਰੀ() { ਵਾਪਸ length; }
int32_t ਸਾਰਾ_String_Break_Iterator::ਪਿਛਲਾ() { ਵਾਪਸ 0; }
int32_t ਸਾਰਾ_String_Break_Iterator::next() { ਵਾਪਸ length; }
int32_t ਸਾਰਾ_String_Break_Iterator::current() ਅਚਲ { ਵਾਪਸ 0; }
int32_t ਸਾਰਾ_String_Break_Iterator::following(int32_t /*offset*/) { ਵਾਪਸ length; }
int32_t ਸਾਰਾ_String_Break_Iterator::preceding(int32_t /*offset*/) { ਵਾਪਸ 0; }
UBool ਸਾਰਾ_String_Break_Iterator::isBoundary(int32_t /*offset*/) { ਵਾਪਸ ਝੂਠ; }
int32_t ਸਾਰਾ_String_Break_Iterator::next(int32_t /*n*/) { ਵਾਪਸ length; }

ਸਾਰਾ_String_Break_Iterator *ਸਾਰਾ_String_Break_Iterator::create_ਬਫਰ_Clone(
        ਖਾਲੀ * /*stackBuffer*/, int32_t & /*BufferSize*/, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
        error_ਕੋਡ = U_UNSUPPORTED_ERROR;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}
ਸਾਰਾ_String_Break_Iterator&
ਸਾਰਾ_String_Break_Iterator::refresh_ਨਿਵੇਸ਼_ਪਾਠ(UText* /*ਨਿਵੇਸ਼*/,
                                           UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
        error_ਕੋਡ = U_UNSUPPORTED_ERROR;
    }
    ਵਾਪਸ *ਇਹ;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ਅਚਲ Locale* locale,
                                              ਅਚਲ ਅੱਖਰ* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              Local_ਸੂਚਕ<BreakIterator>& ownedIter,
                                              UError_ਕੋਡ& error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    options &= U_TITLECASE_ITERATOR_ਨਕਾਬ;
    ਜੇ (options != 0 && iter != ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (iter == ਖਾਲੀ_ਸੂਚਕ) {
        ਚੋਣ (options) {
        ਹਾਲਤ 0:
            iter = BreakIterator::create_ਸ਼ਬਦ_Instance(
                locale != ਖਾਲੀ_ਸੂਚਕ ? *locale : Locale(locID), error_ਕੋਡ);
            ਰੋਕੋ;
        ਹਾਲਤ U_TITLECASE_ਸਾਰਾ_STRING:
            iter = ਨਵਾਂ ਸਾਰਾ_String_Break_Iterator();
            ਜੇ (iter == ਖਾਲੀ_ਸੂਚਕ) {
                error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            }
            ਰੋਕੋ;
        ਹਾਲਤ U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != ਖਾਲੀ_ਸੂਚਕ ? *locale : Locale(locID), error_ਕੋਡ);
            ਰੋਕੋ;
        ਮੂਲ:
            error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਰੋਕੋ;
        }
        ownedIter.adoptInstead(iter);
    }
    ਵਾਪਸ iter;
}

int32_t CaseMap::ਮੰਜ਼ਿਲ_Title(
        ਅਚਲ ਅੱਖਰ *locale, uint32_t options, BreakIterator *iter,
        ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
        char16_t *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    Local_ਸੂਚਕ<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(ਖਾਲੀ_ਸੂਚਕ, locale, options, iter, ownedIter, error_ਕੋਡ);
    ਜੇ(iter==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0;
    }
    UnicodeString s(src_ਲੰਮਾਈ<0, src, src_ਲੰਮਾਈ);
    iter->ਸੈੱਟ_ਪਾਠ(s);
    ਵਾਪਸ ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_Title, edits, error_ਕੋਡ);
}

U_NAMESPACE_ਸਿਰਾ

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_str_ਮੰਜ਼ਿਲ_Title(char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
             ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
             UBreakIterator *titleIter,
             ਅਚਲ ਅੱਖਰ *locale,
             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    Local_ਸੂਚਕ<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        ਖਾਲੀ_ਸੂਚਕ, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *p_Error_ਕੋਡ);
    ਜੇ (iter == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0;
    }
    UnicodeString s(src_ਲੰਮਾਈ<0, src, src_ਲੰਮਾਈ);
    iter->ਸੈੱਟ_ਪਾਠ(s);
    ਵਾਪਸ ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_Title, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_ਮੰਜ਼ਿਲ_Title(UCaseMap *csm,
                 char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                 ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਸਵੈਚਾਲਿਤ *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ਜੇ (impl->iter == ਖਾਲੀ_ਸੂਚਕ) {
        Local_ਸੂਚਕ<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            ਖਾਲੀ_ਸੂਚਕ, impl->locale, impl->options, ਖਾਲੀ_ਸੂਚਕ, ownedIter, *p_Error_ਕੋਡ);
        ਜੇ (iter == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(src_ਲੰਮਾਈ<0, src, src_ਲੰਮਾਈ);
    impl->iter->ਸੈੱਟ_ਪਾਠ(s);
    ਵਾਪਸ ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_Title, ਖਾਲੀ_ਸੂਚਕ, *p_Error_ਕੋਡ);
}

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_BREAK_ITERATION
