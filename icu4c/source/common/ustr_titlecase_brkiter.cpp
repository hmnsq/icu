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
वर्ग सम्पूर्णम्_String_Break_Iterator : सार्वजनिक BreakIterator {
सार्वजनिक:
    सम्पूर्णम्_String_Break_Iterator() : BreakIterator(), length(0) {}
    ~सम्पूर्णम्_String_Break_Iterator() override;
    तार्किक संक्रियकः==(ध्रुव BreakIterator&) ध्रुव override;
    सम्पूर्णम्_String_Break_Iterator *clone() ध्रुव override;
    स्थिर UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() ध्रुव override;
    CharacterIterator& getText() ध्रुव U_LIFETIME_BOUND override;
    UText* getUText(UText* fill_अन्तर् U_LIFETIME_BOUND, UError_संहिता& error_संहिता) ध्रुव override;
    शून्य  समुच्चयः_Text(ध्रुव UnicodeString &text) override;
    शून्य  समुच्चयः_Text(UText *text, UError_संहिता &error_संहिता) override;
    शून्य  adoptText(CharacterIterator* it) override;
    int32_t first() override;
    int32_t चरमः() override;
    int32_t previous() override;
    int32_t next() override;
    int32_t current() ध्रुव override;
    int32_t following(int32_t विस्थापनम्) override;
    int32_t preceding(int32_t विस्थापनम्) override;
    UBool isBoundary(int32_t विस्थापनम्) override;
    int32_t next(int32_t n) override;
    सम्पूर्णम्_String_Break_Iterator *create_बफर_Clone(शून्य *राशिः_बफर, int32_t &बफर_आकार_2,
                                                UError_संहिता &error_संहिता) override;
    सम्पूर्णम्_String_Break_Iterator& refresh_निवेशः_Text(UText* input,
                                               UError_संहिता& error_संहिता) U_LIFETIME_BOUND override;

गुप्त:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(सम्पूर्णम्_String_Break_Iterator)

सम्पूर्णम्_String_Break_Iterator::~सम्पूर्णम्_String_Break_Iterator() {}
तार्किक सम्पूर्णम्_String_Break_Iterator::संक्रियकः==(ध्रुव BreakIterator&) ध्रुव { प्रत्यागम असत्य; }
सम्पूर्णम्_String_Break_Iterator *सम्पूर्णम्_String_Break_Iterator::clone() ध्रुव { प्रत्यागम शून्यसूचकः; }

CharacterIterator& सम्पूर्णम्_String_Break_Iterator::getText() ध्रुव U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *सम्पूर्णम्_String_Break_Iterator::getUText(UText * /*fillIn*/, UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_SUCCESS(error_संहिता)) {
        error_संहिता = U_UNSUPPORTED_ERROR;
    }
    प्रत्यागम शून्यसूचकः;
}

शून्य  सम्पूर्णम्_String_Break_Iterator::समुच्चयः_Text(ध्रुव UnicodeString &text) {
    length = text.length();
}
शून्य  सम्पूर्णम्_String_Break_Iterator::समुच्चयः_Text(UText *text, UError_संहिता &error_संहिता) {
    यदि (U_SUCCESS(error_संहिता)) {
        int64_t length64 = utext_native_दीर्घता(text);
        यदि (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } अन्यथा {
            error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
        }
    }
}
शून्य  सम्पूर्णम्_String_Break_Iterator::adoptText(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t सम्पूर्णम्_String_Break_Iterator::first() { प्रत्यागम 0; }
int32_t सम्पूर्णम्_String_Break_Iterator::चरमः() { प्रत्यागम length; }
int32_t सम्पूर्णम्_String_Break_Iterator::previous() { प्रत्यागम 0; }
int32_t सम्पूर्णम्_String_Break_Iterator::next() { प्रत्यागम length; }
int32_t सम्पूर्णम्_String_Break_Iterator::current() ध्रुव { प्रत्यागम 0; }
int32_t सम्पूर्णम्_String_Break_Iterator::following(int32_t /*offset*/) { प्रत्यागम length; }
int32_t सम्पूर्णम्_String_Break_Iterator::preceding(int32_t /*offset*/) { प्रत्यागम 0; }
UBool सम्पूर्णम्_String_Break_Iterator::isBoundary(int32_t /*offset*/) { प्रत्यागम असत्य; }
int32_t सम्पूर्णम्_String_Break_Iterator::next(int32_t /*n*/) { प्रत्यागम length; }

सम्पूर्णम्_String_Break_Iterator *सम्पूर्णम्_String_Break_Iterator::create_बफर_Clone(
        शून्य * /*stackBuffer*/, int32_t & /*BufferSize*/, UError_संहिता &error_संहिता) {
    यदि (U_SUCCESS(error_संहिता)) {
        error_संहिता = U_UNSUPPORTED_ERROR;
    }
    प्रत्यागम शून्यसूचकः;
}
सम्पूर्णम्_String_Break_Iterator&
सम्पूर्णम्_String_Break_Iterator::refresh_निवेशः_Text(UText* /*निवेशः*/,
                                           UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    यदि (U_SUCCESS(error_संहिता)) {
        error_संहिता = U_UNSUPPORTED_ERROR;
    }
    प्रत्यागम *एतत्;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ध्रुव Locale* locale,
                                              ध्रुव वर्ण* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UError_संहिता& error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    options &= U_TITLECASE_ITERATOR_आवरणम्;
    यदि (options != 0 && iter != शून्यसूचकः) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    यदि (iter == शून्यसूचकः) {
        विकल्प (options) {
        पक्ष 0:
            iter = BreakIterator::createWordInstance(
                locale != शून्यसूचकः ? *locale : Locale(locID), error_संहिता);
            भङ्ग;
        पक्ष U_TITLECASE_सम्पूर्णम्_STRING:
            iter = नूतन सम्पूर्णम्_String_Break_Iterator();
            यदि (iter == शून्यसूचकः) {
                error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            }
            भङ्ग;
        पक्ष U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != शून्यसूचकः ? *locale : Locale(locID), error_संहिता);
            भङ्ग;
        स्वभाव:
            error_संहिता = U_ILLEGAL_तर्कः_ERROR;
            भङ्ग;
        }
        ownedIter.adoptInstead(iter);
    }
    प्रत्यागम iter;
}

int32_t CaseMap::गन्तव्यम्_Title(
        ध्रुव वर्ण *locale, uint32_t options, BreakIterator *iter,
        ध्रुव char16_t *src, int32_t src_दीर्घता,
        char16_t *dest, int32_t dest_क्षमता, Edits *edits,
        UError_संहिता &error_संहिता) {
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(शून्यसूचकः, locale, options, iter, ownedIter, error_संहिता);
    यदि(iter==शून्यसूचकः) {
        प्रत्यागम 0;
    }
    UnicodeString s(src_दीर्घता<0, src, src_दीर्घता);
    iter->समुच्चयः_Text(s);
    प्रत्यागम ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ustrcase_आन्तरिकम्_गन्तव्यम्_Title, edits, error_संहिता);
}

U_NAMESPACE_अन्तः

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_str_गन्तव्यम्_Title(char16_t *dest, int32_t dest_क्षमता,
             ध्रुव char16_t *src, int32_t src_दीर्घता,
             UBreakIterator *titleIter,
             ध्रुव वर्ण *locale,
             UError_संहिता *p_Error_संहिता) {
    LocalPointer<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        शून्यसूचकः, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *p_Error_संहिता);
    यदि (iter == शून्यसूचकः) {
        प्रत्यागम 0;
    }
    UnicodeString s(src_दीर्घता<0, src, src_दीर्घता);
    iter->समुच्चयः_Text(s);
    प्रत्यागम ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ustrcase_आन्तरिकम्_गन्तव्यम्_Title, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
ucasemap_गन्तव्यम्_Title(UCaseMap *csm,
                 char16_t *dest, int32_t dest_क्षमता,
                 ध्रुव char16_t *src, int32_t src_दीर्घता,
                 UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    स्वचालित *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    यदि (impl->iter == शून्यसूचकः) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            शून्यसूचकः, impl->locale, impl->options, शून्यसूचकः, ownedIter, *p_Error_संहिता);
        यदि (iter == शून्यसूचकः) {
            प्रत्यागम 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(src_दीर्घता<0, src, src_दीर्घता);
    impl->iter->समुच्चयः_Text(s);
    प्रत्यागम ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ustrcase_आन्तरिकम्_गन्तव्यम्_Title, शून्यसूचकः, *p_Error_संहिता);
}

#यदि_अन्तः  // !UCONFIG_NO_BREAK_ITERATION
