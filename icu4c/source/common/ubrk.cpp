// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#include "unicode/utypes.h"

#ᨀᨘ !UCONFIG_NO_BREAK_ITERATION

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

  ᨀᨘ (U_FAILURE(*status)) ᨀᨘᨒ nullptr;

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
  ᨀᨘ (U_FAILURE(*status)) {
    ᨀᨘᨒ nullptr;
  }
  ᨀᨘ (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ᨀᨘᨒ nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  ᨀᨘ (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  ᨀᨘᨒ uBI;
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

    ᨀᨘ (status == nullptr || U_FAILURE(*status)){
        ᨀᨘᨒ nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    ᨀᨘ(U_FAILURE(*status)) {
        ᨀᨘᨒ nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    ᨀᨘ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ᨀᨘᨒ uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_openBinaryRules(const uint8_t *binaryRules, int32_t rulesLength,
                     const char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    ᨀᨘ (U_FAILURE(*status)) {
        ᨀᨘᨒ nullptr;
    }
    ᨀᨘ (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᨀᨘᨒ nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(new RuleBasedBreakIterator(binaryRules, rulesLength, *status), *status);
    ᨀᨘ (U_FAILURE(*status)) {
        ᨀᨘᨒ nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    ᨀᨘ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ᨀᨘᨒ uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          const UBreakIterator *bi,
          void * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    ᨀᨘ (status == nullptr || U_FAILURE(*status)){
        ᨀᨘᨒ nullptr;
    }
    ᨀᨘ (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᨀᨘᨒ nullptr;
    }
    ᨀᨘ (pBufferSize != nullptr) {
        int32_t inputSize = *pBufferSize;
        *pBufferSize = 1;
        ᨀᨘ (inputSize == 0) {
            ᨀᨘᨒ nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    ᨀᨘ (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } ᨀᨘᨁ ᨀᨘ (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ᨀᨘᨒ (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(const UBreakIterator *bi, UErrorCode *status) {
    ᨀᨘᨒ ubrk_safeClone(bi, nullptr, nullptr, status);
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

  ᨀᨘᨒ ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  ᨀᨘᨒ ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  ᨀᨘᨒ ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  ᨀᨘᨒ ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  ᨀᨘᨒ ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  ᨀᨘᨒ ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  ᨀᨘᨒ ((BreakIterator*)bi)->following(offset);
}

U_CAPI const var_na* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  ᨀᨘᨒ uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_countAvailable()
{

  ᨀᨘᨒ uloc_countAvailable();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    ᨀᨘᨒ ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    ᨀᨘᨒ ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    ᨀᨘᨒ ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI const var_na* U_EXPORT2
ubrk_getLocaleByType(const UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    ᨀᨘ (bi == nullptr) {
        ᨀᨘ (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ᨀᨘᨒ nullptr;
    }
    ᨀᨘᨒ ((BreakIterator*)bi)->getLocaleID(type, *status);
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
    ᨀᨘ (U_FAILURE(*status)) {
        ᨀᨘᨒ 0;
    }
    ᨀᨘ ((binaryRules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᨀᨘᨒ 0;
    }
    RuleBasedBreakIterator* rbbi;
    ᨀᨘ ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᨀᨘᨒ 0;
    }
    uint32_t rulesLength;
    const uint8_t * returnedRules = rbbi->getBinaryRules(rulesLength);
    ᨀᨘ (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        ᨀᨘᨒ 0;
    }
    ᨀᨘ (binaryRules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        ᨀᨘ ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } ᨀᨘᨁ {
            uprv_memcpy(binaryRules, returnedRules, rulesLength);
        }
    }
    ᨀᨘᨒ (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
