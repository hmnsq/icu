// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#include "unicode/utypes.h"

#ᰀᰧ !UCONFIG_NO_BREAK_ITERATION

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

  ᰀᰧ (U_FAILURE(*status)) ᰀᰧᰌ nullptr;

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
  ᰀᰧ (U_FAILURE(*status)) {
    ᰀᰧᰌ nullptr;
  }
  ᰀᰧ (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ᰀᰧᰌ nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  ᰀᰧ (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  ᰀᰧᰌ uBI;
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

    ᰀᰧ (status == nullptr || U_FAILURE(*status)){
        ᰀᰧᰌ nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    ᰀᰧ(U_FAILURE(*status)) {
        ᰀᰧᰌ nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    ᰀᰧ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ᰀᰧᰌ uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_openBinaryRules(const uint8_t *binaryRules, int32_t rulesLength,
                     const char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    ᰀᰧ (U_FAILURE(*status)) {
        ᰀᰧᰌ nullptr;
    }
    ᰀᰧ (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(new RuleBasedBreakIterator(binaryRules, rulesLength, *status), *status);
    ᰀᰧ (U_FAILURE(*status)) {
        ᰀᰧᰌ nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    ᰀᰧ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ᰀᰧᰌ uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          const UBreakIterator *bi,
          void * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    ᰀᰧ (status == nullptr || U_FAILURE(*status)){
        ᰀᰧᰌ nullptr;
    }
    ᰀᰧ (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ nullptr;
    }
    ᰀᰧ (pBufferSize != nullptr) {
        int32_t inputSize = *pBufferSize;
        *pBufferSize = 1;
        ᰀᰧ (inputSize == 0) {
            ᰀᰧᰌ nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    ᰀᰧ (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } ᰀᰧᰀ ᰀᰧ (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ᰀᰧᰌ (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(const UBreakIterator *bi, UErrorCode *status) {
    ᰀᰧᰌ ubrk_safeClone(bi, nullptr, nullptr, status);
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

  ᰀᰧᰌ ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  ᰀᰧᰌ ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  ᰀᰧᰌ ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  ᰀᰧᰌ ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  ᰀᰧᰌ ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  ᰀᰧᰌ ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  ᰀᰧᰌ ((BreakIterator*)bi)->following(offset);
}

U_CAPI const var_na* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  ᰀᰧᰌ uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_countAvailable()
{

  ᰀᰧᰌ uloc_countAvailable();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    ᰀᰧᰌ ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    ᰀᰧᰌ ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    ᰀᰧᰌ ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI const var_na* U_EXPORT2
ubrk_getLocaleByType(const UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    ᰀᰧ (bi == nullptr) {
        ᰀᰧ (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ᰀᰧᰌ nullptr;
    }
    ᰀᰧᰌ ((BreakIterator*)bi)->getLocaleID(type, *status);
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
    ᰀᰧ (U_FAILURE(*status)) {
        ᰀᰧᰌ 0;
    }
    ᰀᰧ ((binaryRules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ 0;
    }
    RuleBasedBreakIterator* rbbi;
    ᰀᰧ ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ 0;
    }
    uint32_t rulesLength;
    const uint8_t * returnedRules = rbbi->getBinaryRules(rulesLength);
    ᰀᰧ (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        ᰀᰧᰌ 0;
    }
    ᰀᰧ (binaryRules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        ᰀᰧ ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } ᰀᰧᰀ {
            uprv_memcpy(binaryRules, returnedRules, rulesLength);
        }
    }
    ᰀᰧᰌ (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
