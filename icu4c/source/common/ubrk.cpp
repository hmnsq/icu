// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#include "unicode/utypes.h"

#если !UCONFIG_NO_BREAK_ITERATION

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

  если (U_FAILURE(*status)) вернуть nullptr;

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
  если (U_FAILURE(*status)) {
    вернуть nullptr;
  }
  если (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    вернуть nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  если (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  вернуть uBI;
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

    если (status == nullptr || U_FAILURE(*status)){
        вернуть nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    если(U_FAILURE(*status)) {
        вернуть nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    если (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    вернуть uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_openBinaryRules(const uint8_t *binaryRules, int32_t rulesLength,
                     const char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    если (U_FAILURE(*status)) {
        вернуть nullptr;
    }
    если (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        вернуть nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(new RuleBasedBreakIterator(binaryRules, rulesLength, *status), *status);
    если (U_FAILURE(*status)) {
        вернуть nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    если (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    вернуть uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          const UBreakIterator *bi,
          void * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    если (status == nullptr || U_FAILURE(*status)){
        вернуть nullptr;
    }
    если (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        вернуть nullptr;
    }
    если (pBufferSize != nullptr) {
        int32_t inputSize = *pBufferSize;
        *pBufferSize = 1;
        если (inputSize == 0) {
            вернуть nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    если (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } иначе если (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    вернуть (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(const UBreakIterator *bi, UErrorCode *status) {
    вернуть ubrk_safeClone(bi, nullptr, nullptr, status);
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

  вернуть ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  вернуть ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  вернуть ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  вернуть ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  вернуть ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  вернуть ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  вернуть ((BreakIterator*)bi)->following(offset);
}

U_CAPI const char* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  вернуть uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_countAvailable()
{

  вернуть uloc_countAvailable();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    вернуть ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    вернуть ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    вернуть ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI const char* U_EXPORT2
ubrk_getLocaleByType(const UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    если (bi == nullptr) {
        если (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        вернуть nullptr;
    }
    вернуть ((BreakIterator*)bi)->getLocaleID(type, *status);
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
    если (U_FAILURE(*status)) {
        вернуть 0;
    }
    если ((binaryRules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        вернуть 0;
    }
    RuleBasedBreakIterator* rbbi;
    если ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        вернуть 0;
    }
    uint32_t rulesLength;
    const uint8_t * returnedRules = rbbi->getBinaryRules(rulesLength);
    если (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        вернуть 0;
    }
    если (binaryRules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        если ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } иначе {
            uprv_memcpy(binaryRules, returnedRules, rulesLength);
        }
    }
    вернуть (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
