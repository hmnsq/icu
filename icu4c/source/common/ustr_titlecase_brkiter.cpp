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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/casemap.h"
#ଅନ୍ତର୍ଭାଵ "unicode/chariter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localpointer.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ubrk.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucasemap.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utext.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucase.h"
#ଅନ୍ତର୍ଭାଵ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
ଶ୍ରେଣୀ WholeStringBreakIterator : ସର୍ବସାଧାରଣ BreakIterator {
ସର୍ବସାଧାରଣ:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    ତାର୍କିକ operator==(ଅଚଳ BreakIterator&) ଅଚଳ override;
    WholeStringBreakIterator *clone() ଅଚଳ override;
    ସ୍ଥିର UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ଅଚଳ override;
    CharacterIterator& getText() ଅଚଳ U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) ଅଚଳ override;
    ଶୂନ୍ୟ  setText(ଅଚଳ UnicodeString &text) override;
    ଶୂନ୍ୟ  setText(UText *text, UErrorCode &errorCode) override;
    ଶୂନ୍ୟ  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() ଅଚଳ override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *createBufferClone(ଶୂନ୍ୟ *ଗଦା_Buffer, int32_t &BufferSize,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refresh_ନିବେଶ_Text(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

ବ୍ୟକ୍ତିଗତ:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
ତାର୍କିକ WholeStringBreakIterator::operator==(ଅଚଳ BreakIterator&) ଅଚଳ { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() ଅଚଳ { ଫେରନ୍ତୁ nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() ଅଚଳ U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ଫେରନ୍ତୁ nullptr;
}

ଶୂନ୍ୟ  WholeStringBreakIterator::setText(ଅଚଳ UnicodeString &text) {
    length = text.length();
}
ଶୂନ୍ୟ  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    ଯଦି (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_nativeLength(text);
        ଯଦି (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } ଅନ୍ୟଥା {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        }
    }
}
ଶୂନ୍ୟ  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { ଫେରନ୍ତୁ 0; }
int32_t WholeStringBreakIterator::last() { ଫେରନ୍ତୁ length; }
int32_t WholeStringBreakIterator::previous() { ଫେରନ୍ତୁ 0; }
int32_t WholeStringBreakIterator::next() { ଫେରନ୍ତୁ length; }
int32_t WholeStringBreakIterator::current() ଅଚଳ { ଫେରନ୍ତୁ 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { ଫେରନ୍ତୁ length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { ଫେରନ୍ତୁ 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { ଫେରନ୍ତୁ length; }

WholeStringBreakIterator *WholeStringBreakIterator::createBufferClone(
        ଶୂନ୍ୟ * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    ଯଦି (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ଫେରନ୍ତୁ nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refresh_ନିବେଶ_Text(UText* /*ନିବେଶ*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    ଯଦି (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ଅଚଳ Locale* locale,
                                              ଅଚଳ ଅକ୍ଷର* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    ଯଦି (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (iter == nullptr) {
        ବାଛ (options) {
        ସ୍ଥିତି 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି U_TITLECASE_WHOLE_STRING:
            iter = ନୂତନ WholeStringBreakIterator();
            ଯଦି (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            ଭାଙ୍ଗ;
        ସ୍ଥିତି U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ଭାଙ୍ଗ;
        }
        ownedIter.adoptInstead(iter);
    }
    ଫେରନ୍ତୁ iter;
}

int32_t CaseMap::toTitle(
        ଅଚଳ ଅକ୍ଷର *locale, uint32_t options, BreakIterator *iter,
        ଅଚଳ char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    ଯଦି(iter==nullptr) {
        ଫେରନ୍ତୁ 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    ଫେରନ୍ତୁ ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, edits, errorCode);
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_strToTitle(char16_t *dest, int32_t destCapacity,
             ଅଚଳ char16_t *src, int32_t srcLength,
             UBreakIterator *titleIter,
             ଅଚଳ ଅକ୍ଷର *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    ଯଦି (iter == nullptr) {
        ଫେରନ୍ତୁ 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    ଫେରନ୍ତୁ ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_toTitle(UCaseMap *csm,
                 char16_t *dest, int32_t destCapacity,
                 ଅଚଳ char16_t *src, int32_t srcLength,
                 UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ଯଦି (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        ଯଦି (iter == nullptr) {
            ଫେରନ୍ତୁ 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(srcLength<0, src, srcLength);
    impl->iter->setText(s);
    ଫେରନ୍ତୁ ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
