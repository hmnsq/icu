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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/brkiter.h"
#समावेश "unicode/casemap.h"
#समावेश "unicode/chariter.h"
#समावेश "unicode/localpointer.h"
#समावेश "unicode/ubrk.h"
#समावेश "unicode/ucasemap.h"
#समावेश "unicode/utext.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"
#समावेश "ucase.h"
#समावेश "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
श्रेणी WholeStringBreakIterator : खुला BreakIterator {
खुला:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    bool चालक==(स्थिर BreakIterator&) स्थिर override;
    WholeStringBreakIterator *clone() स्थिर override;
    अटल UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() स्थिर override;
    CharacterIterator& getText() स्थिर U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) स्थिर override;
    व्योम  setText(स्थिर UnicodeString &text) override;
    व्योम  setText(UText *text, UErrorCode &errorCode) override;
    व्योम  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() स्थिर override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *create_बफर_Clone(व्योम *ढेर_बफर, int32_t &बफर_आकार_2,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refresh_निवेश_Text(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

निजी:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
bool WholeStringBreakIterator::चालक==(स्थिर BreakIterator&) स्थिर { वापस false; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() स्थिर { वापस nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() स्थिर U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) स्थिर {
    अगर (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    वापस nullptr;
}

व्योम  WholeStringBreakIterator::setText(स्थिर UnicodeString &text) {
    length = text.length();
}
व्योम  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    अगर (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_native_लंबाई(text);
        अगर (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } अन्यथा {
            errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
        }
    }
}
व्योम  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { वापस 0; }
int32_t WholeStringBreakIterator::last() { वापस length; }
int32_t WholeStringBreakIterator::previous() { वापस 0; }
int32_t WholeStringBreakIterator::next() { वापस length; }
int32_t WholeStringBreakIterator::current() स्थिर { वापस 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { वापस length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { वापस 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { वापस false; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { वापस length; }

WholeStringBreakIterator *WholeStringBreakIterator::create_बफर_Clone(
        व्योम * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    अगर (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    वापस nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refresh_निवेश_Text(UText* /*निवेश*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    अगर (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    वापस *यह;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(स्थिर Locale* locale,
                                              स्थिर अक्षर* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    अगर (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    अगर (iter == nullptr) {
        चयन (options) {
        हाल 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            अवरोध;
        हाल U_TITLECASE_WHOLE_STRING:
            iter = नया WholeStringBreakIterator();
            अगर (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            अवरोध;
        हाल U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            अवरोध;
        शेष:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            अवरोध;
        }
        ownedIter.adoptInstead(iter);
    }
    वापस iter;
}

int32_t CaseMap::toTitle(
        स्थिर अक्षर *locale, uint32_t options, BreakIterator *iter,
        स्थिर char16_t *src, int32_t src_लंबाई,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    अगर(iter==nullptr) {
        वापस 0;
    }
    UnicodeString s(src_लंबाई<0, src, src_लंबाई);
    iter->setText(s);
    वापस ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, src_लंबाई,
        ustrcase_internalToTitle, edits, errorCode);
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_strToTitle(char16_t *dest, int32_t destCapacity,
             स्थिर char16_t *src, int32_t src_लंबाई,
             UBreakIterator *titleIter,
             स्थिर अक्षर *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    अगर (iter == nullptr) {
        वापस 0;
    }
    UnicodeString s(src_लंबाई<0, src, src_लंबाई);
    iter->setText(s);
    वापस ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, destCapacity,
        src, src_लंबाई,
        ustrcase_internalToTitle, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_toTitle(UCaseMap *csm,
                 char16_t *dest, int32_t destCapacity,
                 स्थिर char16_t *src, int32_t src_लंबाई,
                 UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    स्वतः *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    अगर (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        अगर (iter == nullptr) {
            वापस 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(src_लंबाई<0, src, src_लंबाई);
    impl->iter->setText(s);
    वापस ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, src_लंबाई,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#पूर्ण_अगर  // !UCONFIG_NO_BREAK_ITERATION
