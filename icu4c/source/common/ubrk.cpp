// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#include "unicode/utypes.h"

#ပေႃး !UCONFIG_NO_BREAK_ITERATION

#include "unicode/ubrk.h"

#include "unicode/brkiter.h"
#include "unicode/uloc.h"
#include "unicode/ustring.h"
#include "unicode/uchriter.h"
#include "unicode/rbbi.h"
#include "rbbirb.h"
#include "uassert.h"
#include "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      const var_na *locale,
      const char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  ပေႃး (U_FAILURE(*status)) ပွၵ်ႈ nullptr;

  BreakIterator *result = nullptr;

  switch(type) {

  pak_sa UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    bha_nga;

  pak_sa UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    bha_nga;

  pak_sa UBRK_LINE:
    result = BreakIterator::createLineInstance(Locale(locale), *status);
    bha_nga;

  pak_sa UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    bha_nga;

  pak_sa UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    bha_nga;

  default:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  ပေႃး (U_FAILURE(*status)) {
    ပွၵ်ႈ nullptr;
  }
  ပေႃး (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ပွၵ်ႈ nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  ပေႃး (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  ပွၵ်ႈ uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  const char16_t     *rules,
                       int32_t       rulesLength,
                 const char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    ပေႃး (status == nullptr || U_FAILURE(*status)){
        ပွၵ်ႈ nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    ပေႃး(U_FAILURE(*status)) {
        ပွၵ်ႈ nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    ပေႃး (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ပွၵ်ႈ uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_openBinaryRules(const uint8_t *binaryRules, int32_t rulesLength,
                     const char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    ပေႃး (U_FAILURE(*status)) {
        ပွၵ်ႈ nullptr;
    }
    ပေႃး (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(new RuleBasedBreakIterator(binaryRules, rulesLength, *status), *status);
    ပေႃး (U_FAILURE(*status)) {
        ပွၵ်ႈ nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    ပေႃး (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ပွၵ်ႈ uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          const UBreakIterator *bi,
          void * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    ပေႃး (status == nullptr || U_FAILURE(*status)){
        ပွၵ်ႈ nullptr;
    }
    ပေႃး (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ nullptr;
    }
    ပေႃး (pBufferSize != nullptr) {
        int32_t inputSize = *pBufferSize;
        *pBufferSize = 1;
        ပေႃး (inputSize == 0) {
            ပွၵ်ႈ nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    ပေႃး (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } ပေႃးဢမ်ႇ ပေႃး (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ပွၵ်ႈ (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(const UBreakIterator *bi, UErrorCode *status) {
    ပွၵ်ႈ ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI void U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI void U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             const char16_t*    text,
             int32_t         textLength,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, textLength, status);
    ((BreakIterator*)bi)->setText(&ut, *status);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI void U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(const UBreakIterator *bi)
{

  ပွၵ်ႈ ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  ပွၵ်ႈ ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  ပွၵ်ႈ ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  ပွၵ်ႈ ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  ပွၵ်ႈ ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  ပွၵ်ႈ ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  ပွၵ်ႈ ((BreakIterator*)bi)->following(offset);
}

U_CAPI const var_na* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  ပွၵ်ႈ uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_countAvailable()
{

  ပွၵ်ႈ uloc_countAvailable();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    ပွၵ်ႈ ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    ပွၵ်ႈ ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    ပွၵ်ႈ ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI const var_na* U_EXPORT2
ubrk_getLocaleByType(const UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    ပေႃး (bi == nullptr) {
        ပေႃး (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ပွၵ်ႈ nullptr;
    }
    ပွၵ်ႈ ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI void U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refreshInputText(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_getBinaryRules(UBreakIterator *bi,
                    uint8_t *       binaryRules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    ပေႃး (U_FAILURE(*status)) {
        ပွၵ်ႈ 0;
    }
    ပေႃး ((binaryRules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ 0;
    }
    RuleBasedBreakIterator* rbbi;
    ပေႃး ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ 0;
    }
    uint32_t rulesLength;
    const uint8_t * returnedRules = rbbi->getBinaryRules(rulesLength);
    ပေႃး (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        ပွၵ်ႈ 0;
    }
    ပေႃး (binaryRules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        ပေႃး ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } ပေႃးဢမ်ႇ {
            uprv_memcpy(binaryRules, returnedRules, rulesLength);
        }
    }
    ပွၵ်ႈ (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
