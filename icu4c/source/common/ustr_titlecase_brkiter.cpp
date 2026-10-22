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

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

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
ವರ್ಗ ಸಂಪೂರ್ಣ_String_Break_Iterator : ಸಾರ್ವಜನಿಕ BreakIterator {
ಸಾರ್ವಜನಿಕ:
    ಸಂಪೂರ್ಣ_String_Break_Iterator() : BreakIterator(), length(0) {}
    ~ಸಂಪೂರ್ಣ_String_Break_Iterator() override;
    ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ BreakIterator&) ಸ್ಥಿರಾಂಕ override;
    ಸಂಪೂರ್ಣ_String_Break_Iterator *clone() ಸ್ಥಿರಾಂಕ override;
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;
    CharacterIterator& get_ಪಠ್ಯ() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND override;
    UText* getUText(UText* fill_ಒಳ U_LIFETIME_BOUND, UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;
    ಶೂನ್ಯ  ಗಣ_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ UnicodeString &ಪಠ್ಯ) override;
    ಶೂನ್ಯ  ಗಣ_ಪಠ್ಯ(UText *ಪಠ್ಯ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) override;
    ಶೂನ್ಯ  adopt_ಪಠ್ಯ(CharacterIterator* it) override;
    int32_t first() override;
    int32_t ಕೊನೆಯ() override;
    int32_t ಹಿಂದಿನ() override;
    int32_t next() override;
    int32_t current() ಸ್ಥಿರಾಂಕ override;
    int32_t following(int32_t ಸ್ಥಾನಾಂತರ) override;
    int32_t preceding(int32_t ಸ್ಥಾನಾಂತರ) override;
    UBool isBoundary(int32_t ಸ್ಥಾನಾಂತರ) override;
    int32_t next(int32_t n) override;
    ಸಂಪೂರ್ಣ_String_Break_Iterator *create_ಬಫರ್_Clone(ಶೂನ್ಯ *ರಾಶಿ_ಬಫರ್, int32_t &ಬಫರ್_ಗಾತ್ರ_2,
                                                UError_ಸಂಕೇತ &error_ಸಂಕೇತ) override;
    ಸಂಪೂರ್ಣ_String_Break_Iterator& refresh_ಒಳಹರಿವು_ಪಠ್ಯ(UText* input,
                                               UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND override;

ಖಾಸಗಿ:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ಸಂಪೂರ್ಣ_String_Break_Iterator)

ಸಂಪೂರ್ಣ_String_Break_Iterator::~ಸಂಪೂರ್ಣ_String_Break_Iterator() {}
ತಾರ್ಕಿಕ ಸಂಪೂರ್ಣ_String_Break_Iterator::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ BreakIterator&) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
ಸಂಪೂರ್ಣ_String_Break_Iterator *ಸಂಪೂರ್ಣ_String_Break_Iterator::clone() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }

CharacterIterator& ಸಂಪೂರ್ಣ_String_Break_Iterator::get_ಪಠ್ಯ() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *ಸಂಪೂರ್ಣ_String_Break_Iterator::getUText(UText * /*fillIn*/, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
        error_ಸಂಕೇತ = U_UNSUPPORTED_ERROR;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಶೂನ್ಯ  ಸಂಪೂರ್ಣ_String_Break_Iterator::ಗಣ_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ UnicodeString &ಪಠ್ಯ) {
    length = ಪಠ್ಯ.length();
}
ಶೂನ್ಯ  ಸಂಪೂರ್ಣ_String_Break_Iterator::ಗಣ_ಪಠ್ಯ(UText *ಪಠ್ಯ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
        int64_t length64 = utext_native_ಉದ್ದಳತೆ(ಪಠ್ಯ);
        ಆದರೆ (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } ಇಲ್ಲದಿದ್ದರೆ {
            error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        }
    }
}
ಶೂನ್ಯ  ಸಂಪೂರ್ಣ_String_Break_Iterator::adopt_ಪಠ್ಯ(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t ಸಂಪೂರ್ಣ_String_Break_Iterator::first() { ಹಿಂತಿರುಗಿ 0; }
int32_t ಸಂಪೂರ್ಣ_String_Break_Iterator::ಕೊನೆಯ() { ಹಿಂತಿರುಗಿ length; }
int32_t ಸಂಪೂರ್ಣ_String_Break_Iterator::ಹಿಂದಿನ() { ಹಿಂತಿರುಗಿ 0; }
int32_t ಸಂಪೂರ್ಣ_String_Break_Iterator::next() { ಹಿಂತಿರುಗಿ length; }
int32_t ಸಂಪೂರ್ಣ_String_Break_Iterator::current() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ 0; }
int32_t ಸಂಪೂರ್ಣ_String_Break_Iterator::following(int32_t /*offset*/) { ಹಿಂತಿರುಗಿ length; }
int32_t ಸಂಪೂರ್ಣ_String_Break_Iterator::preceding(int32_t /*offset*/) { ಹಿಂತಿರುಗಿ 0; }
UBool ಸಂಪೂರ್ಣ_String_Break_Iterator::isBoundary(int32_t /*offset*/) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
int32_t ಸಂಪೂರ್ಣ_String_Break_Iterator::next(int32_t /*n*/) { ಹಿಂತಿರುಗಿ length; }

ಸಂಪೂರ್ಣ_String_Break_Iterator *ಸಂಪೂರ್ಣ_String_Break_Iterator::create_ಬಫರ್_Clone(
        ಶೂನ್ಯ * /*stackBuffer*/, int32_t & /*BufferSize*/, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
        error_ಸಂಕೇತ = U_UNSUPPORTED_ERROR;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}
ಸಂಪೂರ್ಣ_String_Break_Iterator&
ಸಂಪೂರ್ಣ_String_Break_Iterator::refresh_ಒಳಹರಿವು_ಪಠ್ಯ(UText* /*ಒಳಹರಿವು*/,
                                           UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
        error_ಸಂಕೇತ = U_UNSUPPORTED_ERROR;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ಸ್ಥಿರಾಂಕ Locale* locale,
                                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              Local_ಸೂಚಕ<BreakIterator>& ownedIter,
                                              UError_ಸಂಕೇತ& error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    options &= U_TITLECASE_ITERATOR_ಮುಖವಾಡ;
    ಆದರೆ (options != 0 && iter != ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (iter == ಶೂನ್ಯಸೂಚಕ) {
        ಆಯ್ಕೆ (options) {
        ಸ್ಥಿತಿ 0:
            iter = BreakIterator::create_ಶಬ್ದ_Instance(
                locale != ಶೂನ್ಯಸೂಚಕ ? *locale : Locale(locID), error_ಸಂಕೇತ);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ U_TITLECASE_ಸಂಪೂರ್ಣ_STRING:
            iter = ಹೊಸ ಸಂಪೂರ್ಣ_String_Break_Iterator();
            ಆದರೆ (iter == ಶೂನ್ಯಸೂಚಕ) {
                error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != ಶೂನ್ಯಸೂಚಕ ? *locale : Locale(locID), error_ಸಂಕೇತ);
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
        ownedIter.adoptInstead(iter);
    }
    ಹಿಂತಿರುಗಿ iter;
}

int32_t CaseMap::ಗಮ್ಯ_Title(
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, uint32_t options, BreakIterator *iter,
        ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
        char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    Local_ಸೂಚಕ<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(ಶೂನ್ಯಸೂಚಕ, locale, options, iter, ownedIter, error_ಸಂಕೇತ);
    ಆದರೆ(iter==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    UnicodeString s(src_ಉದ್ದಳತೆ<0, src, src_ಉದ್ದಳತೆ);
    iter->ಗಣ_ಪಠ್ಯ(s);
    ಹಿಂತಿರುಗಿ ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_Title, edits, error_ಸಂಕೇತ);
}

U_NAMESPACE_ಕೊನೆ

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_str_ಗಮ್ಯ_Title(char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
             ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
             UBreakIterator *titleIter,
             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    Local_ಸೂಚಕ<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        ಶೂನ್ಯಸೂಚಕ, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *p_Error_ಸಂಕೇತ);
    ಆದರೆ (iter == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    UnicodeString s(src_ಉದ್ದಳತೆ<0, src, src_ಉದ್ದಳತೆ);
    iter->ಗಣ_ಪಠ್ಯ(s);
    ಹಿಂತಿರುಗಿ ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_Title, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_ಗಮ್ಯ_Title(UCaseMap *csm,
                 char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                 ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಸ್ವಯಂಚಾಲಿತ *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ಆದರೆ (impl->iter == ಶೂನ್ಯಸೂಚಕ) {
        Local_ಸೂಚಕ<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            ಶೂನ್ಯಸೂಚಕ, impl->locale, impl->options, ಶೂನ್ಯಸೂಚಕ, ownedIter, *p_Error_ಸಂಕೇತ);
        ಆದರೆ (iter == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(src_ಉದ್ದಳತೆ<0, src, src_ಉದ್ದಳತೆ);
    impl->iter->ಗಣ_ಪಠ್ಯ(s);
    ಹಿಂತಿರುಗಿ ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_Title, ಶೂನ್ಯಸೂಚಕ, *p_Error_ಸಂಕೇತ);
}

#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_NO_BREAK_ITERATION
