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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/casemap.h"
#അന്തര്ഭാവ "unicode/chariter.h"
#അന്തര്ഭാവ "unicode/localpointer.h"
#അന്തര്ഭാവ "unicode/ubrk.h"
#അന്തര്ഭാവ "unicode/ucasemap.h"
#അന്തര്ഭാവ "unicode/utext.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucase.h"
#അന്തര്ഭാവ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
വർഗ്ഗം WholeStringBreakIterator : പൊതു BreakIterator {
പൊതു:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    തർക്കം operator==(സ്ഥിരം BreakIterator&) സ്ഥിരം override;
    WholeStringBreakIterator *clone() സ്ഥിരം override;
    സ്ഥിതം UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() സ്ഥിരം override;
    CharacterIterator& getText() സ്ഥിരം U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) സ്ഥിരം override;
    ശൂന്യം  setText(സ്ഥിരം UnicodeString &text) override;
    ശൂന്യം  setText(UText *text, UErrorCode &errorCode) override;
    ശൂന്യം  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() സ്ഥിരം override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *createBufferClone(ശൂന്യം *അട്ടി_Buffer, int32_t &BufferSize,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refresh_നിവേശം_Text(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

സ്വകാര്യ:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
തർക്കം WholeStringBreakIterator::operator==(സ്ഥിരം BreakIterator&) സ്ഥിരം { തിരികെ അസത്യം; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() സ്ഥിരം { തിരികെ nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() സ്ഥിരം U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    തിരികെ nullptr;
}

ശൂന്യം  WholeStringBreakIterator::setText(സ്ഥിരം UnicodeString &text) {
    length = text.length();
}
ശൂന്യം  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    എങ്കിൽ (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_nativeLength(text);
        എങ്കിൽ (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } അല്ലെങ്കിൽ {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        }
    }
}
ശൂന്യം  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { തിരികെ 0; }
int32_t WholeStringBreakIterator::last() { തിരികെ length; }
int32_t WholeStringBreakIterator::previous() { തിരികെ 0; }
int32_t WholeStringBreakIterator::next() { തിരികെ length; }
int32_t WholeStringBreakIterator::current() സ്ഥിരം { തിരികെ 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { തിരികെ length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { തിരികെ 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { തിരികെ അസത്യം; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { തിരികെ length; }

WholeStringBreakIterator *WholeStringBreakIterator::createBufferClone(
        ശൂന്യം * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    എങ്കിൽ (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    തിരികെ nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refresh_നിവേശം_Text(UText* /*നിവേശം*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    എങ്കിൽ (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    തിരികെ *ഇത്;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(സ്ഥിരം Locale* locale,
                                              സ്ഥിരം അക്ഷരം* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    എങ്കിൽ (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    എങ്കിൽ (iter == nullptr) {
        തിരഞ്ഞെടുപ്പ് (options) {
        അവസ്ഥ 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            നിർത്തുക;
        അവസ്ഥ U_TITLECASE_WHOLE_STRING:
            iter = പുതിയ WholeStringBreakIterator();
            എങ്കിൽ (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            നിർത്തുക;
        അവസ്ഥ U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            നിർത്തുക;
        }
        ownedIter.adoptInstead(iter);
    }
    തിരികെ iter;
}

int32_t CaseMap::toTitle(
        സ്ഥിരം അക്ഷരം *locale, uint32_t options, BreakIterator *iter,
        സ്ഥിരം char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    എങ്കിൽ(iter==nullptr) {
        തിരികെ 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    തിരികെ ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, edits, errorCode);
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_strToTitle(char16_t *dest, int32_t destCapacity,
             സ്ഥിരം char16_t *src, int32_t srcLength,
             UBreakIterator *titleIter,
             സ്ഥിരം അക്ഷരം *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    എങ്കിൽ (iter == nullptr) {
        തിരികെ 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    തിരികെ ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_toTitle(UCaseMap *csm,
                 char16_t *dest, int32_t destCapacity,
                 സ്ഥിരം char16_t *src, int32_t srcLength,
                 UErrorCode *pErrorCode) {
    എങ്കിൽ (U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    എങ്കിൽ (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        എങ്കിൽ (iter == nullptr) {
            തിരികെ 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(srcLength<0, src, srcLength);
    impl->iter->setText(s);
    തിരികെ ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
