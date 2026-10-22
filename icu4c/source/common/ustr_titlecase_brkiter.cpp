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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/casemap.h"
#अन्तर्भाव "unicode/chariter.h"
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "unicode/ubrk.h"
#अन्तर्भाव "unicode/ucasemap.h"
#अन्तर्भाव "unicode/utext.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucase.h"
#अन्तर्भाव "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
वर्ग WholeStringBreakIterator : सार्वजनिक BreakIterator {
सार्वजनिक:
    WholeStringBreakIterator() : BreakIterator(), length(0) {}
    ~WholeStringBreakIterator() override;
    तार्किक operator==(ध्रुव BreakIterator&) ध्रुव override;
    WholeStringBreakIterator *clone() ध्रुव override;
    स्थिर UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ध्रुव override;
    CharacterIterator& getText() ध्रुव U_LIFETIME_BOUND override;
    UText* getUText(UText* fillIn U_LIFETIME_BOUND, UErrorCode& errorCode) ध्रुव override;
    शून्य  setText(ध्रुव UnicodeString &text) override;
    शून्य  setText(UText *text, UErrorCode &errorCode) override;
    शून्य  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t last() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() ध्रुव override;
    int32_t following(int32_t offset) override;
    int32_t preceding(int32_t offset) override;
    UBool isBoundary(int32_t offset) override;
    int32_t next(int32_t n) override;
    WholeStringBreakIterator *createBufferClone(शून्य *चळत_Buffer, int32_t &BufferSize,
                                                UErrorCode &errorCode) override;
    WholeStringBreakIterator& refresh_आदान_Text(UText* input,
                                               UErrorCode& errorCode) U_LIFETIME_BOUND override;

गुप्त:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(WholeStringBreakIterator)

WholeStringBreakIterator::~WholeStringBreakIterator() {}
तार्किक WholeStringBreakIterator::operator==(ध्रुव BreakIterator&) ध्रुव { प्रत्यागम असत्य; }
WholeStringBreakIterator *WholeStringBreakIterator::clone() ध्रुव { प्रत्यागम nullptr; }

CharacterIterator& WholeStringBreakIterator::getText() ध्रुव U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *WholeStringBreakIterator::getUText(UText * /*fillIn*/, UErrorCode &errorCode) ध्रुव {
    यदि (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    प्रत्यागम nullptr;
}

शून्य  WholeStringBreakIterator::setText(ध्रुव UnicodeString &text) {
    length = text.length();
}
शून्य  WholeStringBreakIterator::setText(UText *text, UErrorCode &errorCode) {
    यदि (U_SUCCESS(errorCode)) {
        int64_t length64 = utext_nativeLength(text);
        यदि (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } अन्यथा {
            errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        }
    }
}
शून्य  WholeStringBreakIterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t WholeStringBreakIterator::first() { प्रत्यागम 0; }
int32_t WholeStringBreakIterator::last() { प्रत्यागम length; }
int32_t WholeStringBreakIterator::previous() { प्रत्यागम 0; }
int32_t WholeStringBreakIterator::next() { प्रत्यागम length; }
int32_t WholeStringBreakIterator::current() ध्रुव { प्रत्यागम 0; }
int32_t WholeStringBreakIterator::following(int32_t /*offset*/) { प्रत्यागम length; }
int32_t WholeStringBreakIterator::preceding(int32_t /*offset*/) { प्रत्यागम 0; }
UBool WholeStringBreakIterator::isBoundary(int32_t /*offset*/) { प्रत्यागम असत्य; }
int32_t WholeStringBreakIterator::next(int32_t /*n*/) { प्रत्यागम length; }

WholeStringBreakIterator *WholeStringBreakIterator::createBufferClone(
        शून्य * /*stackBuffer*/, int32_t & /*BufferSize*/, UErrorCode &errorCode) {
    यदि (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    प्रत्यागम nullptr;
}
WholeStringBreakIterator&
WholeStringBreakIterator::refresh_आदान_Text(UText* /*आदान*/,
                                           UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि (U_SUCCESS(errorCode)) {
        errorCode = U_UNSUPPORTED_ERROR;
    }
    प्रत्यागम *एतत्;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ध्रुव Locale* locale,
                                              ध्रुव वर्ण* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode) {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
    options &= U_TITLECASE_ITERATOR_MASK;
    यदि (options != 0 && iter != nullptr) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    यदि (iter == nullptr) {
        विकल्प (options) {
        पक्ष 0:
            iter = BreakIterator::createWordInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            भङ्ग;
        पक्ष U_TITLECASE_WHOLE_STRING:
            iter = नूतन WholeStringBreakIterator();
            यदि (iter == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
            भङ्ग;
        पक्ष U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != nullptr ? *locale : Locale(locID), errorCode);
            भङ्ग;
        स्वभाव:
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            भङ्ग;
        }
        ownedIter.adoptInstead(iter);
    }
    प्रत्यागम iter;
}

int32_t CaseMap::toTitle(
        ध्रुव वर्ण *locale, uint32_t options, BreakIterator *iter,
        ध्रुव char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    यदि(iter==nullptr) {
        प्रत्यागम 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    प्रत्यागम ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, edits, errorCode);
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_strToTitle(char16_t *dest, int32_t destCapacity,
             ध्रुव char16_t *src, int32_t srcLength,
             UBreakIterator *titleIter,
             ध्रुव वर्ण *locale,
             UErrorCode *pErrorCode) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        nullptr, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *pErrorCode);
    यदि (iter == nullptr) {
        प्रत्यागम 0;
    }
    UnicodeString s(srcLength<0, src, srcLength);
    iter->setText(s);
    प्रत्यागम ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_toTitle(UCaseMap *csm,
                 char16_t *dest, int32_t destCapacity,
                 ध्रुव char16_t *src, int32_t srcLength,
                 UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    यदि (impl->iter == nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        यदि (iter == nullptr) {
            प्रत्यागम 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(srcLength<0, src, srcLength);
    impl->iter->setText(s);
    प्रत्यागम ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToTitle, nullptr, *pErrorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
