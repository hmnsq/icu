// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/ubrk.h"

#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uchriter.h"
#अन्तर्भाव "unicode/rbbi.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      अचर वर्ण *locale,
      अचर char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  यदि (U_FAILURE(*status)) फर्काऊ nullptr;

  BreakIterator *result = nullptr;

  विकल्प(type) {

  पक्ष UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    रोक;

  पक्ष UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    रोक;

  पक्ष UBRK_पङ्क्ति:
    result = BreakIterator::create_पङ्क्ति_Instance(Locale(locale), *status);
    रोक;

  पक्ष UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    रोक;

  पक्ष UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    रोक;

  स्वभाव:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  यदि (U_FAILURE(*status)) {
    फर्काऊ nullptr;
  }
  यदि (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    फर्काऊ nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  यदि (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  फर्काऊ uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  अचर char16_t     *rules,
                       int32_t       rulesLength,
                 अचर char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    यदि (status == nullptr || U_FAILURE(*status)){
        फर्काऊ nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    यदि(U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    यदि (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    फर्काऊ uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_द्विआधारी_Rules(अचर uint8_t *द्विआधारी_Rules, int32_t rulesLength,
                     अचर char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    यदि (U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(नूतन RuleBasedBreakIterator(द्विआधारी_Rules, rulesLength, *status), *status);
    यदि (U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    यदि (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    फर्काऊ uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          अचर UBreakIterator *bi,
          शून्य * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    यदि (status == nullptr || U_FAILURE(*status)){
        फर्काऊ nullptr;
    }
    यदि (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    यदि (pBufferSize != nullptr) {
        int32_t आगत_Size = *pBufferSize;
        *pBufferSize = 1;
        यदि (आगत_Size == 0) {
            फर्काऊ nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    यदि (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा यदि (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    फर्काऊ (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(अचर UBreakIterator *bi, UErrorCode *status) {
    फर्काऊ ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI शून्य U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI शून्य U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             अचर char16_t*    text,
             int32_t         textLength,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, textLength, status);
    ((BreakIterator*)bi)->setText(&ut, *status);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI शून्य U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(अचर UBreakIterator *bi)
{

  फर्काऊ ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  फर्काऊ ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  फर्काऊ ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  फर्काऊ ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  फर्काऊ ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  फर्काऊ ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  फर्काऊ ((BreakIterator*)bi)->following(offset);
}

U_CAPI अचर वर्ण* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  फर्काऊ uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_गन्ती_Available()
{

  फर्काऊ uloc_गन्ती_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    फर्काऊ ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    फर्काऊ ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    फर्काऊ ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI अचर वर्ण* U_EXPORT2
ubrk_getLocaleByType(अचर UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    यदि (bi == nullptr) {
        यदि (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        फर्काऊ nullptr;
    }
    फर्काऊ ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI शून्य U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_आगत_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_द्विआधारी_Rules(UBreakIterator *bi,
                    uint8_t *       द्विआधारी_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    यदि (U_FAILURE(*status)) {
        फर्काऊ 0;
    }
    यदि ((द्विआधारी_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }
    RuleBasedBreakIterator* rbbi;
    यदि ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }
    uint32_t rulesLength;
    अचर uint8_t * returnedRules = rbbi->get_द्विआधारी_Rules(rulesLength);
    यदि (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        फर्काऊ 0;
    }
    यदि (द्विआधारी_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        यदि ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } अन्यथा {
            uprv_memcpy(द्विआधारी_Rules, returnedRules, rulesLength);
        }
    }
    फर्काऊ (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
