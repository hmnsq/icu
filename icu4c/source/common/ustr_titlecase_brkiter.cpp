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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/casemap.h"
#অন্তর্ভাব "unicode/chariter.h"
#অন্তর্ভাব "unicode/localpointer.h"
#অন্তর্ভাব "unicode/ubrk.h"
#অন্তর্ভাব "unicode/ucasemap.h"
#অন্তর্ভাব "unicode/utext.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucase.h"
#অন্তর্ভাব "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
শ্রেণী WholeStringBreakIterator : সর্বজনীন BreakIterator {
সর্বজনীন:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    যৌক্তিক operator==(ধ্রুবক BreakIterator&) ধ্রুবক override;
    WholeStringBreakIterator *clone() ধ্রুবক override;
    স্থির UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ধ্রুবক override;
    CharacterIterator& getText() ধ্রুবক U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) ধ্রুবক override;
    শূন্য  setText(ধ্রুবক UnicodeString &text) override;
    শূন্য  setText(UText *text, UErrorCode &errorCode) override;
    শূন্য  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() ধ্রুবক override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *create_বাফার_Clone(শূন্য *স্তূপ_বাফার, int32_t &বাফার_আকার_2,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refresh_প্রবেশ_Text(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

ব্যক্তিগত:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
যৌক্তিক WholeStringBreakIterator::operator==(ধ্রুবক BreakIterator&) ধ্রুবক { ফেরত মিথ্যা; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() ধ্রুবক { ফেরত nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() ধ্রুবক U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) ধ্রুবক {
    যদি (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ফেরত nullptr;
}

শূন্য  WholeStringBreakIterator::setText(ধ্রুবক UnicodeString &text) {
    length = text.length();
}
শূন্য  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    যদি (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_native_দৈর্ঘ্য(text);
        যদি (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } অন্যথা {
            errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
        }
    }
}
শূন্য  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { ফেরত 0; }
int32_t WholeStringBreakIterator::last() { ফেরত length; }
int32_t WholeStringBreakIterator::previous() { ফেরত 0; }
int32_t WholeStringBreakIterator::next() { ফেরত length; }
int32_t WholeStringBreakIterator::current() ধ্রুবক { ফেরত 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { ফেরত length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { ফেরত 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { ফেরত মিথ্যা; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { ফেরত length; }

WholeStringBreakIterator *WholeStringBreakIterator::create_বাফার_Clone(
        শূন্য * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    যদি (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ফেরত nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refresh_প্রবেশ_Text(UText* /*প্রবেশ*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    ফেরত *এই;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ধ্রুবক Locale* locale,
                                              ধ্রুবক অক্ষর* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    যদি (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    যদি (iter == nullptr) {
        নির্বাচন (options) {
        ক্ষেত্র 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            বিরতি;
        ক্ষেত্র U_TITLECASE_WHOLE_STRING:
            iter = নতুন WholeStringBreakIterator();
            যদি (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            বিরতি;
        ক্ষেত্র U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            বিরতি;
        পূর্বনির্ধারিত:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            বিরতি;
        }
        ownedIter.adoptInstead(iter);
    }
    ফেরত iter;
}

int32_t CaseMap::toTitle(
        ধ্রুবক অক্ষর *locale, uint32_t options, BreakIterator *iter,
        ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    যদি(iter==nullptr) {
        ফেরত 0;
    }
    UnicodeString s(src_দৈর্ঘ্য<0, src, src_দৈর্ঘ্য);
    iter->setText(s);
    ফেরত ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ustrcase_internalToTitle, edits, errorCode);
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_strToTitle(char16_t *dest, int32_t destCapacity,
             ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
             UBreakIterator *titleIter,
             ধ্রুবক অক্ষর *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    যদি (iter == nullptr) {
        ফেরত 0;
    }
    UnicodeString s(src_দৈর্ঘ্য<0, src, src_দৈর্ঘ্য);
    iter->setText(s);
    ফেরত ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ustrcase_internalToTitle, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_toTitle(UCaseMap *csm,
                 char16_t *dest, int32_t destCapacity,
                 ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                 UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    যদি (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        যদি (iter == nullptr) {
            ফেরত 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(src_দৈর্ঘ্য<0, src, src_দৈর্ঘ্য);
    impl->iter->setText(s);
    ফেরত ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
