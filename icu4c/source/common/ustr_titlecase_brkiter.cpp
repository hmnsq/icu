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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/brkiter.h"
#ضمن "unicode/casemap.h"
#ضمن "unicode/chariter.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/ubrk.h"
#ضمن "unicode/ucasemap.h"
#ضمن "unicode/utext.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ucase.h"
#ضمن "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
صنف WholeStringBreakIterator : عام BreakIterator {
عام:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    منطقي operator==(ثابت BreakIterator&) ثابت override;
    WholeStringBreakIterator *clone() ثابت override;
    ساكن UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ثابت override;
    CharacterIterator& getText() ثابت U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) ثابت override;
    فراغ  setText(ثابت UnicodeString &text) override;
    فراغ  setText(UText *text, UErrorCode &errorCode) override;
    فراغ  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() ثابت override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *createBufferClone(فراغ *مكدس_Buffer, int32_t &BufferSize,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refresh_مدخل_Text(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

خاص:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
منطقي WholeStringBreakIterator::operator==(ثابت BreakIterator&) ثابت { أرجع خطأ; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() ثابت { أرجع nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() ثابت U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) ثابت {
    إذا (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    أرجع nullptr;
}

فراغ  WholeStringBreakIterator::setText(ثابت UnicodeString &text) {
    length = text.length();
}
فراغ  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    إذا (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_nativeLength(text);
        إذا (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } وإلا {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        }
    }
}
فراغ  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { أرجع 0; }
int32_t WholeStringBreakIterator::last() { أرجع length; }
int32_t WholeStringBreakIterator::previous() { أرجع 0; }
int32_t WholeStringBreakIterator::next() { أرجع length; }
int32_t WholeStringBreakIterator::current() ثابت { أرجع 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { أرجع length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { أرجع 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { أرجع خطأ; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { أرجع length; }

WholeStringBreakIterator *WholeStringBreakIterator::createBufferClone(
        فراغ * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    إذا (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    أرجع nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refresh_مدخل_Text(UText* /*مدخل*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    أرجع *هذا;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ثابت Locale* locale,
                                              ثابت حرف* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    إذا (U_FAILURE(errorCode)) { أرجع nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    إذا (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    إذا (iter == nullptr) {
        بدل (options) {
        حالة 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            اكسر;
        حالة U_TITLECASE_WHOLE_STRING:
            iter = جديد WholeStringBreakIterator();
            إذا (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            اكسر;
        حالة U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            اكسر;
        افتراضي:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            اكسر;
        }
        ownedIter.adoptInstead(iter);
    }
    أرجع iter;
}

int32_t CaseMap::toTitle(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    إذا(iter==nullptr) {
        أرجع 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    أرجع ustrcase_map(
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
             ثابت حرف *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    إذا (iter == nullptr) {
        أرجع 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    أرجع ustrcase_mapWithOverlap(
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
    إذا (U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        إذا (iter == nullptr) {
            أرجع 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(srcLength<0, src, srcLength);
    impl->iter->setText(s);
    أرجع ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
