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

#include "unicode/utypes.h"

#якщо !UCONFIG_NO_BREAK_ITERATION

#include "unicode/brkiter.h"
#include "unicode/casemap.h"
#include "unicode/chariter.h"
#include "unicode/localpointer.h"
#include "unicode/ubrk.h"
#include "unicode/ucasemap.h"
#include "unicode/utext.h"
#include "cmemory.h"
#include "uassert.h"
#include "ucase.h"
#include "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
class WholeStringBreakIterator : public BreakIterator {
public:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    bool operator==(const BreakIterator&) const override;
    WholeStringBreakIterator *clone() const override;
    static UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() const override;
    CharacterIterator& getText() const U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) const override;
    void  setText(const UnicodeString &text) override;
    void  setText(UText *text, UErrorCode &errorCode) override;
    void  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() const override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *createBufferClone(void *stackBuffer, int32_t &BufferSize,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refreshInputText(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

private:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
bool WholeStringBreakIterator::operator==(const BreakIterator&) const { повернути false; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() const { повернути nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() const U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) const {
    якщо (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    повернути nullptr;
}

void  WholeStringBreakIterator::setText(const UnicodeString &text) {
    length = text.length();
}
void  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    якщо (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_nativeLength(text);
        якщо (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } інакше {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        }
    }
}
void  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { повернути 0; }
int32_t WholeStringBreakIterator::last() { повернути length; }
int32_t WholeStringBreakIterator::previous() { повернути 0; }
int32_t WholeStringBreakIterator::next() { повернути length; }
int32_t WholeStringBreakIterator::current() const { повернути 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { повернути length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { повернути 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { повернути false; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { повернути length; }

WholeStringBreakIterator *WholeStringBreakIterator::createBufferClone(
        void * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    якщо (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    повернути nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refreshInputText(UText* /*input*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    якщо (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    повернути *this;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(const Locale* locale,
                                              const char* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    якщо (U_FAILURE(errorCode)) { повернути nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    якщо (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        повернути nullptr;
    }
    якщо (iter == nullptr) {
        switch (options) {
        case 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            break;
        case U_TITLECASE_WHOLE_STRING:
            iter = new WholeStringBreakIterator();
            якщо (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            break;
        case U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            break;
        default:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            break;
        }
        ownedIter.adoptInstead(iter);
    }
    повернути iter;
}

int32_t CaseMap::toTitle(
        const char *locale, uint32_t options, BreakIterator *iter,
        const char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    якщо(iter==nullptr) {
        повернути 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    повернути ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, edits, errorCode);
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_strToTitle(char16_t *dest, int32_t destCapacity,
             const char16_t *src, int32_t srcLength,
             UBreakIterator *titleIter,
             const char *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    якщо (iter == nullptr) {
        повернути 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    повернути ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_toTitle(UCaseMap *csm,
                 char16_t *dest, int32_t destCapacity,
                 const char16_t *src, int32_t srcLength,
                 UErrorCode *pErrorCode) {
    якщо (U_FAILURE(*pErrorCode)) {
        повернути 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    якщо (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        якщо (iter == nullptr) {
            повернути 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(srcLength<0, src, srcLength);
    impl->iter->setText(s);
    повернути ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
