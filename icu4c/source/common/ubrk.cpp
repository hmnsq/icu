// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#include "unicode/utypes.h"

#ប្រសិនបើ !UCONFIG_NO_BREAK_ITERATION

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

  ប្រសិនបើ (U_FAILURE(*status)) ត្រឡប់ nullptr;

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
  ប្រសិនបើ (U_FAILURE(*status)) {
    ត្រឡប់ nullptr;
  }
  ប្រសិនបើ (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ត្រឡប់ nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  ប្រសិនបើ (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  ត្រឡប់ uBI;
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

    ប្រសិនបើ (status == nullptr || U_FAILURE(*status)){
        ត្រឡប់ nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    ប្រសិនបើ(U_FAILURE(*status)) {
        ត្រឡប់ nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    ប្រសិនបើ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ត្រឡប់ uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_openBinaryRules(const uint8_t *binaryRules, int32_t rulesLength,
                     const char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    ប្រសិនបើ (U_FAILURE(*status)) {
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(new RuleBasedBreakIterator(binaryRules, rulesLength, *status), *status);
    ប្រសិនបើ (U_FAILURE(*status)) {
        ត្រឡប់ nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    ប្រសិនបើ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ត្រឡប់ uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          const UBreakIterator *bi,
          void * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    ប្រសិនបើ (status == nullptr || U_FAILURE(*status)){
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ (pBufferSize != nullptr) {
        int32_t inputSize = *pBufferSize;
        *pBufferSize = 1;
        ប្រសិនបើ (inputSize == 0) {
            ត្រឡប់ nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    ប្រសិនបើ (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } បើមិនដូច្នេះ ប្រសិនបើ (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ត្រឡប់ (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(const UBreakIterator *bi, UErrorCode *status) {
    ត្រឡប់ ubrk_safeClone(bi, nullptr, nullptr, status);
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

  ត្រឡប់ ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  ត្រឡប់ ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  ត្រឡប់ ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  ត្រឡប់ ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  ត្រឡប់ ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  ត្រឡប់ ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  ត្រឡប់ ((BreakIterator*)bi)->following(offset);
}

U_CAPI const var_na* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  ត្រឡប់ uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_countAvailable()
{

  ត្រឡប់ uloc_countAvailable();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    ត្រឡប់ ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    ត្រឡប់ ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    ត្រឡប់ ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI const var_na* U_EXPORT2
ubrk_getLocaleByType(const UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    ប្រសិនបើ (bi == nullptr) {
        ប្រសិនបើ (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ត្រឡប់ nullptr;
    }
    ត្រឡប់ ((BreakIterator*)bi)->getLocaleID(type, *status);
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
    ប្រសិនបើ (U_FAILURE(*status)) {
        ត្រឡប់ 0;
    }
    ប្រសិនបើ ((binaryRules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ 0;
    }
    RuleBasedBreakIterator* rbbi;
    ប្រសិនបើ ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ 0;
    }
    uint32_t rulesLength;
    const uint8_t * returnedRules = rbbi->getBinaryRules(rulesLength);
    ប្រសិនបើ (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        ត្រឡប់ 0;
    }
    ប្រសិនបើ (binaryRules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        ប្រសិនបើ ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } បើមិនដូច្នេះ {
            uprv_memcpy(binaryRules, returnedRules, rulesLength);
        }
    }
    ត្រឡប់ (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
