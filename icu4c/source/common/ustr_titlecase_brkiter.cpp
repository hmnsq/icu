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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/casemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/chariter.h"
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucasemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/utext.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucase.h"
#ಅನ್ತರ್ಭಾವ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
ವರ್ಗ WholeStringBreakIterator : ಸಾರ್ವಜನಿಕ BreakIterator {
ಸಾರ್ವಜನಿಕ:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    ತಾರ್ಕಿಕ operator==(ಧ್ರುವ BreakIterator&) ಧ್ರುವ override;
    WholeStringBreakIterator *clone() ಧ್ರುವ override;
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ಧ್ರುವ override;
    CharacterIterator& getText() ಧ್ರುವ U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) ಧ್ರುವ override;
    ಶೂನ್ಯ  setText(ಧ್ರುವ UnicodeString &text) override;
    ಶೂನ್ಯ  setText(UText *text, UErrorCode &errorCode) override;
    ಶೂನ್ಯ  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() ಧ್ರುವ override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *createBufferClone(ಶೂನ್ಯ *ರಾಶಿ_Buffer, int32_t &BufferSize,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refresh_ಒಳಹರಿವು_Text(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

ಗುಪ್ತ:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
ತಾರ್ಕಿಕ WholeStringBreakIterator::operator==(ಧ್ರುವ BreakIterator&) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() ಧ್ರುವ U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಶೂನ್ಯ  WholeStringBreakIterator::setText(ಧ್ರುವ UnicodeString &text) {
    length = text.length();
}
ಶೂನ್ಯ  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    ಯದಿ (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_nativeLength(text);
        ಯದಿ (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } ಅನ್ಯಥಾ {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        }
    }
}
ಶೂನ್ಯ  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { ಪ್ರತ್ಯಾಗಮ 0; }
int32_t WholeStringBreakIterator::last() { ಪ್ರತ್ಯಾಗಮ length; }
int32_t WholeStringBreakIterator::previous() { ಪ್ರತ್ಯಾಗಮ 0; }
int32_t WholeStringBreakIterator::next() { ಪ್ರತ್ಯಾಗಮ length; }
int32_t WholeStringBreakIterator::current() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { ಪ್ರತ್ಯಾಗಮ length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { ಪ್ರತ್ಯಾಗಮ 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { ಪ್ರತ್ಯಾಗಮ length; }

WholeStringBreakIterator *WholeStringBreakIterator::createBufferClone(
        ಶೂನ್ಯ * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    ಯದಿ (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refresh_ಒಳಹರಿವು_Text(UText* /*ಒಳಹರಿವು*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    ಯದಿ (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ಧ್ರುವ Locale* locale,
                                              ಧ್ರುವ ವರ್ಣ* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    ಯದಿ (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (iter == nullptr) {
        ವಿಕಲ್ಪ (options) {
        ಪಕ್ಷ 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            ಭಙ್ಗ;
        ಪಕ್ಷ U_TITLECASE_WHOLE_STRING:
            iter = ನೂತನ WholeStringBreakIterator();
            ಯದಿ (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            ಭಙ್ಗ;
        ಪಕ್ಷ U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ಭಙ್ಗ;
        }
        ownedIter.adoptInstead(iter);
    }
    ಪ್ರತ್ಯಾಗಮ iter;
}

int32_t CaseMap::toTitle(
        ಧ್ರುವ ವರ್ಣ *locale, uint32_t options, BreakIterator *iter,
        ಧ್ರುವ char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    ಯದಿ(iter==nullptr) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    ಪ್ರತ್ಯಾಗಮ ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, edits, errorCode);
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_strToTitle(char16_t *dest, int32_t destCapacity,
             ಧ್ರುವ char16_t *src, int32_t srcLength,
             UBreakIterator *titleIter,
             ಧ್ರುವ ವರ್ಣ *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    ಯದಿ (iter == nullptr) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    ಪ್ರತ್ಯಾಗಮ ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_toTitle(UCaseMap *csm,
                 char16_t *dest, int32_t destCapacity,
                 ಧ್ರುವ char16_t *src, int32_t srcLength,
                 UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ಯದಿ (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        ಯದಿ (iter == nullptr) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(srcLength<0, src, srcLength);
    impl->iter->setText(s);
    ಪ್ರತ್ಯಾಗಮ ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
