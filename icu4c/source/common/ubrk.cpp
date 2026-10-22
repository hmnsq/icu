// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#include "unicode/utypes.h"

#nếu !UCONFIG_NO_BREAK_ITERATION

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
      const char *locale,
      const char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  nếu (U_FAILURE(*status)) trả_về nullptr;

  BreakIterator *result = nullptr;

  switch(type) {

  case UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    break;

  case UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    break;

  case UBRK_LINE:
    result = BreakIterator::createLineInstance(Locale(locale), *status);
    break;

  case UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    break;

  case UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    break;

  default:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  nếu (U_FAILURE(*status)) {
    trả_về nullptr;
  }
  nếu (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    trả_về nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  nếu (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  trả_về uBI;
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

    nếu (status == nullptr || U_FAILURE(*status)){
        trả_về nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    nếu(U_FAILURE(*status)) {
        trả_về nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    nếu (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    trả_về uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_openBinaryRules(const uint8_t *binaryRules, int32_t rulesLength,
                     const char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    nếu (U_FAILURE(*status)) {
        trả_về nullptr;
    }
    nếu (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(new RuleBasedBreakIterator(binaryRules, rulesLength, *status), *status);
    nếu (U_FAILURE(*status)) {
        trả_về nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    nếu (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    trả_về uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          const UBreakIterator *bi,
          void * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    nếu (status == nullptr || U_FAILURE(*status)){
        trả_về nullptr;
    }
    nếu (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về nullptr;
    }
    nếu (pBufferSize != nullptr) {
        int32_t inputSize = *pBufferSize;
        *pBufferSize = 1;
        nếu (inputSize == 0) {
            trả_về nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    nếu (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } không_thì nếu (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    trả_về (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(const UBreakIterator *bi, UErrorCode *status) {
    trả_về ubrk_safeClone(bi, nullptr, nullptr, status);
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

  trả_về ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  trả_về ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  trả_về ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  trả_về ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  trả_về ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  trả_về ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  trả_về ((BreakIterator*)bi)->following(offset);
}

U_CAPI const char* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  trả_về uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_countAvailable()
{

  trả_về uloc_countAvailable();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    trả_về ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    trả_về ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    trả_về ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI const char* U_EXPORT2
ubrk_getLocaleByType(const UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    nếu (bi == nullptr) {
        nếu (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        trả_về nullptr;
    }
    trả_về ((BreakIterator*)bi)->getLocaleID(type, *status);
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
    nếu (U_FAILURE(*status)) {
        trả_về 0;
    }
    nếu ((binaryRules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về 0;
    }
    RuleBasedBreakIterator* rbbi;
    nếu ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về 0;
    }
    uint32_t rulesLength;
    const uint8_t * returnedRules = rbbi->getBinaryRules(rulesLength);
    nếu (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        trả_về 0;
    }
    nếu (binaryRules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        nếu ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } không_thì {
            uprv_memcpy(binaryRules, returnedRules, rulesLength);
        }
    }
    trả_về (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
