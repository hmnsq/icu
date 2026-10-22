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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাৱ "unicode/brkiter.h"
#অন্তর্ভাৱ "unicode/casemap.h"
#অন্তর্ভাৱ "unicode/chariter.h"
#অন্তর্ভাৱ "unicode/localpointer.h"
#অন্তর্ভাৱ "unicode/ubrk.h"
#অন্তর্ভাৱ "unicode/ucasemap.h"
#অন্তর্ভাৱ "unicode/utext.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "ucase.h"
#অন্তর্ভাৱ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
শ্ৰেণী WholeStringBreakIterator : সার্ৱজনিক BreakIterator {
সার্ৱজনিক:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    তার্কিক operator==(ধ্রুৱ BreakIterator&) ধ্রুৱ override;
    WholeStringBreakIterator *clone() ধ্রুৱ override;
    স্থির UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ধ্রুৱ override;
    CharacterIterator& getText() ধ্রুৱ U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) ধ্রুৱ override;
    শূন্য  setText(ধ্রুৱ UnicodeString &text) override;
    শূন্য  setText(UText *text, UErrorCode &errorCode) override;
    শূন্য  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() ধ্রুৱ override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *createBufferClone(শূন্য *স্তূপ_Buffer, int32_t &BufferSize,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refresh_প্ৰবেশ_Text(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

গুপ্ত:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
তার্কিক WholeStringBreakIterator::operator==(ধ্রুৱ BreakIterator&) ধ্রুৱ { ঘূৰাই মিছা; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() ধ্রুৱ { ঘূৰাই nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() ধ্রুৱ U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) ধ্রুৱ {
    যদি (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ঘূৰাই nullptr;
}

শূন্য  WholeStringBreakIterator::setText(ধ্রুৱ UnicodeString &text) {
    length = text.length();
}
শূন্য  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    যদি (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_nativeLength(text);
        যদি (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } অন্যথা {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        }
    }
}
শূন্য  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { ঘূৰাই 0; }
int32_t WholeStringBreakIterator::last() { ঘূৰাই length; }
int32_t WholeStringBreakIterator::previous() { ঘূৰাই 0; }
int32_t WholeStringBreakIterator::next() { ঘূৰাই length; }
int32_t WholeStringBreakIterator::current() ধ্রুৱ { ঘূৰাই 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { ঘূৰাই length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { ঘূৰাই 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { ঘূৰাই মিছা; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { ঘূৰাই length; }

WholeStringBreakIterator *WholeStringBreakIterator::createBufferClone(
        শূন্য * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    যদি (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ঘূৰাই nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refresh_প্ৰবেশ_Text(UText* /*প্ৰবেশ*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ঘূৰাই *এতত্;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ধ্রুৱ Locale* locale,
                                              ধ্রুৱ ৱর্ণ* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    যদি (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }
    যদি (iter == nullptr) {
        ৱিকল্প (options) {
        পক্ষ 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            ভঙ্গ;
        পক্ষ U_TITLECASE_WHOLE_STRING:
            iter = নূতন WholeStringBreakIterator();
            যদি (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            ভঙ্গ;
        পক্ষ U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            ভঙ্গ;
        স্ৱভাৱ:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ভঙ্গ;
        }
        ownedIter.adoptInstead(iter);
    }
    ঘূৰাই iter;
}

int32_t CaseMap::toTitle(
        ধ্রুৱ ৱর্ণ *locale, uint32_t options, BreakIterator *iter,
        ধ্রুৱ char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    যদি(iter==nullptr) {
        ঘূৰাই 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    ঘূৰাই ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, edits, errorCode);
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_strToTitle(char16_t *dest, int32_t destCapacity,
             ধ্রুৱ char16_t *src, int32_t srcLength,
             UBreakIterator *titleIter,
             ধ্রুৱ ৱর্ণ *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    যদি (iter == nullptr) {
        ঘূৰাই 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    ঘূৰাই ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_toTitle(UCaseMap *csm,
                 char16_t *dest, int32_t destCapacity,
                 ধ্রুৱ char16_t *src, int32_t srcLength,
                 UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    যদি (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        যদি (iter == nullptr) {
            ঘূৰাই 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(srcLength<0, src, srcLength);
    impl->iter->setText(s);
    ঘূৰাই ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
