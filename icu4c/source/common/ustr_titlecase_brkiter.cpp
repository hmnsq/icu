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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/casemap.h"
#அந்தர்பாவ "unicode/chariter.h"
#அந்தர்பாவ "unicode/localpointer.h"
#அந்தர்பாவ "unicode/ubrk.h"
#அந்தர்பாவ "unicode/ucasemap.h"
#அந்தர்பாவ "unicode/utext.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucase.h"
#அந்தர்பாவ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
வகுப்பு WholeStringBreakIterator : பொது BreakIterator {
பொது:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    தருக்க operator==(மாறிலி BreakIterator&) மாறிலி override;
    WholeStringBreakIterator *clone() மாறிலி override;
    நிலையான UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() மாறிலி override;
    CharacterIterator& getText() மாறிலி U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) மாறிலி override;
    வெறுமை  setText(மாறிலி UnicodeString &text) override;
    வெறுமை  setText(UText *text, UErrorCode &errorCode) override;
    வெறுமை  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() மாறிலி override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *createBufferClone(வெறுமை *அடுக்கு_Buffer, int32_t &BufferSize,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refresh_உள்ளீடு_Text(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

தனிப்பட்ட:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
தருக்க WholeStringBreakIterator::operator==(மாறிலி BreakIterator&) மாறிலி { திரும்பு பொய்; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() மாறிலி { திரும்பு nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() மாறிலி U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) மாறிலி {
    எனில் (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    திரும்பு nullptr;
}

வெறுமை  WholeStringBreakIterator::setText(மாறிலி UnicodeString &text) {
    length = text.length();
}
வெறுமை  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    எனில் (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_nativeLength(text);
        எனில் (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } இல்லையெனில் {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        }
    }
}
வெறுமை  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { திரும்பு 0; }
int32_t WholeStringBreakIterator::last() { திரும்பு length; }
int32_t WholeStringBreakIterator::previous() { திரும்பு 0; }
int32_t WholeStringBreakIterator::next() { திரும்பு length; }
int32_t WholeStringBreakIterator::current() மாறிலி { திரும்பு 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { திரும்பு length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { திரும்பு 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { திரும்பு பொய்; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { திரும்பு length; }

WholeStringBreakIterator *WholeStringBreakIterator::createBufferClone(
        வெறுமை * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    எனில் (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    திரும்பு nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refresh_உள்ளீடு_Text(UText* /*உள்ளீடு*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    எனில் (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    திரும்பு *இது;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(மாறிலி Locale* locale,
                                              மாறிலி எழுத்து* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    எனில் (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    எனில் (iter == nullptr) {
        தேர்வு (options) {
        நிலை 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            நிறுத்து;
        நிலை U_TITLECASE_WHOLE_STRING:
            iter = புதிய WholeStringBreakIterator();
            எனில் (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            நிறுத்து;
        நிலை U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            நிறுத்து;
        இயல்பு:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            நிறுத்து;
        }
        ownedIter.adoptInstead(iter);
    }
    திரும்பு iter;
}

int32_t CaseMap::toTitle(
        மாறிலி எழுத்து *locale, uint32_t options, BreakIterator *iter,
        மாறிலி char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    எனில்(iter==nullptr) {
        திரும்பு 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    திரும்பு ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, edits, errorCode);
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_strToTitle(char16_t *dest, int32_t destCapacity,
             மாறிலி char16_t *src, int32_t srcLength,
             UBreakIterator *titleIter,
             மாறிலி எழுத்து *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    எனில் (iter == nullptr) {
        திரும்பு 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    திரும்பு ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_toTitle(UCaseMap *csm,
                 char16_t *dest, int32_t destCapacity,
                 மாறிலி char16_t *src, int32_t srcLength,
                 UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    எனில் (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        எனில் (iter == nullptr) {
            திரும்பு 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(srcLength<0, src, srcLength);
    impl->iter->setText(s);
    திரும்பு ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
