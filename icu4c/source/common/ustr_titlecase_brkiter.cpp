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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/brkiter.h"
#شامل "unicode/casemap.h"
#شامل "unicode/chariter.h"
#شامل "unicode/localpointer.h"
#شامل "unicode/ubrk.h"
#شامل "unicode/ucasemap.h"
#شامل "unicode/utext.h"
#شامل "cmemory.h"
#شامل "uassert.h"
#شامل "ucase.h"
#شامل "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
رده WholeStringBreakIterator : عمومی BreakIterator {
عمومی:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    منطقی operator==(ثابت BreakIterator&) ثابت override;
    WholeStringBreakIterator *clone() ثابت override;
    ایستا UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ثابت override;
    CharacterIterator& getText() ثابت U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) ثابت override;
    تهی  setText(ثابت UnicodeString &text) override;
    تهی  setText(UText *text, UErrorCode &errorCode) override;
    تهی  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() ثابت override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *createBufferClone(تهی *پشته_Buffer, int32_t &BufferSize,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refresh_درون_داد_Text(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

خصوصی:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
منطقی WholeStringBreakIterator::operator==(ثابت BreakIterator&) ثابت { بازگشت نادرست; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() ثابت { بازگشت nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() ثابت U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) ثابت {
    اگر (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    بازگشت nullptr;
}

تهی  WholeStringBreakIterator::setText(ثابت UnicodeString &text) {
    length = text.length();
}
تهی  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    اگر (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_nativeLength(text);
        اگر (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } وگرنه {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        }
    }
}
تهی  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { بازگشت 0; }
int32_t WholeStringBreakIterator::last() { بازگشت length; }
int32_t WholeStringBreakIterator::previous() { بازگشت 0; }
int32_t WholeStringBreakIterator::next() { بازگشت length; }
int32_t WholeStringBreakIterator::current() ثابت { بازگشت 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { بازگشت length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { بازگشت 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { بازگشت نادرست; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { بازگشت length; }

WholeStringBreakIterator *WholeStringBreakIterator::createBufferClone(
        تهی * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    اگر (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    بازگشت nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refresh_درون_داد_Text(UText* /*درون داد*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    اگر (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    بازگشت *این;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ثابت Locale* locale,
                                              ثابت نویسه* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    اگر (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    اگر (iter == nullptr) {
        انتخاب (options) {
        حالت 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            بشکن;
        حالت U_TITLECASE_WHOLE_STRING:
            iter = جدید WholeStringBreakIterator();
            اگر (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            بشکن;
        حالت U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            بشکن;
        پیشفرض:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            بشکن;
        }
        ownedIter.adoptInstead(iter);
    }
    بازگشت iter;
}

int32_t CaseMap::toTitle(
        ثابت نویسه *locale, uint32_t options, BreakIterator *iter,
        ثابت char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    اگر(iter==nullptr) {
        بازگشت 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    بازگشت ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, edits, errorCode);
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_strToTitle(char16_t *dest, int32_t destCapacity,
             ثابت char16_t *src, int32_t srcLength,
             UBreakIterator *titleIter,
             ثابت نویسه *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    اگر (iter == nullptr) {
        بازگشت 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    بازگشت ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_toTitle(UCaseMap *csm,
                 char16_t *dest, int32_t destCapacity,
                 ثابت char16_t *src, int32_t srcLength,
                 UErrorCode *pErrorCode) {
    اگر (U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    اگر (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        اگر (iter == nullptr) {
            بازگشت 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(srcLength<0, src, srcLength);
    impl->iter->setText(s);
    بازگشت ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
