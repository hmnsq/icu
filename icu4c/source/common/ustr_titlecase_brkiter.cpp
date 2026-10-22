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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "unicode/brkiter.h"
#અન્તર્ભાવ "unicode/casemap.h"
#અન્તર્ભાવ "unicode/chariter.h"
#અન્તર્ભાવ "unicode/localpointer.h"
#અન્તર્ભાવ "unicode/ubrk.h"
#અન્તર્ભાવ "unicode/ucasemap.h"
#અન્તર્ભાવ "unicode/utext.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucase.h"
#અન્તર્ભાવ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
વર્ગ WholeStringBreakIterator : જાહેર BreakIterator {
જાહેર:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    તાર્કિક operator==(અચલ BreakIterator&) અચલ override;
    WholeStringBreakIterator *clone() અચલ override;
    સ્થિર UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() અચલ override;
    CharacterIterator& getText() અચલ U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) અચલ override;
    શૂન્ય  setText(અચલ UnicodeString &text) override;
    શૂન્ય  setText(UText *text, UErrorCode &errorCode) override;
    શૂન્ય  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() અચલ override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *createBufferClone(શૂન્ય *ઢગલો_Buffer, int32_t &BufferSize,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refresh_આગત_Text(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

ખાનગી:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
તાર્કિક WholeStringBreakIterator::operator==(અચલ BreakIterator&) અચલ { પાછા ખોટું; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() અચલ { પાછા nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() અચલ U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) અચલ {
    જો (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    પાછા nullptr;
}

શૂન્ય  WholeStringBreakIterator::setText(અચલ UnicodeString &text) {
    length = text.length();
}
શૂન્ય  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    જો (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_nativeLength(text);
        જો (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } નહીંતર {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        }
    }
}
શૂન્ય  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { પાછા 0; }
int32_t WholeStringBreakIterator::last() { પાછા length; }
int32_t WholeStringBreakIterator::previous() { પાછા 0; }
int32_t WholeStringBreakIterator::next() { પાછા length; }
int32_t WholeStringBreakIterator::current() અચલ { પાછા 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { પાછા length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { પાછા 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { પાછા ખોટું; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { પાછા length; }

WholeStringBreakIterator *WholeStringBreakIterator::createBufferClone(
        શૂન્ય * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    જો (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    પાછા nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refresh_આગત_Text(UText* /*આગત*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    જો (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    પાછા *આ;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(અચલ Locale* locale,
                                              અચલ અક્ષર* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    જો (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    જો (iter == nullptr) {
        પસંદગી (options) {
        સ્થિતિ 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            તોડો;
        સ્થિતિ U_TITLECASE_WHOLE_STRING:
            iter = નવું WholeStringBreakIterator();
            જો (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            તોડો;
        સ્થિતિ U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            તોડો;
        મૂળભૂત:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            તોડો;
        }
        ownedIter.adoptInstead(iter);
    }
    પાછા iter;
}

int32_t CaseMap::toTitle(
        અચલ અક્ષર *locale, uint32_t options, BreakIterator *iter,
        અચલ char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    જો(iter==nullptr) {
        પાછા 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    પાછા ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, edits, errorCode);
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_strToTitle(char16_t *dest, int32_t destCapacity,
             અચલ char16_t *src, int32_t srcLength,
             UBreakIterator *titleIter,
             અચલ અક્ષર *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    જો (iter == nullptr) {
        પાછા 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    પાછા ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_toTitle(UCaseMap *csm,
                 char16_t *dest, int32_t destCapacity,
                 અચલ char16_t *src, int32_t srcLength,
                 UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    જો (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        જો (iter == nullptr) {
            પાછા 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(srcLength<0, src, srcLength);
    impl->iter->setText(s);
    પાછા ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
