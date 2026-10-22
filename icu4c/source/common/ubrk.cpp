// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#ضمن "unicode/utypes.h"

#ܐܢ !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/ubrk.h"

#ضمن "unicode/brkiter.h"
#ضمن "unicode/uloc.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/uchriter.h"
#ضمن "unicode/rbbi.h"
#ضمن "rbbirb.h"
#ضمن "uassert.h"
#ضمن "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      ثابت حرف *locale,
      ثابت char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  ܐܢ (U_FAILURE(*status)) ܦܢܐ nullptr;

  BreakIterator *result = nullptr;

  بدل(type) {

  حالة UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    اكسر;

  حالة UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    اكسر;

  حالة UBRK_LINE:
    result = BreakIterator::createLineInstance(Locale(locale), *status);
    اكسر;

  حالة UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    اكسر;

  حالة UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    اكسر;

  افتراضي:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  ܐܢ (U_FAILURE(*status)) {
    ܦܢܐ nullptr;
  }
  ܐܢ (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ܦܢܐ nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  ܐܢ (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  ܦܢܐ uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  ثابت char16_t     *rules,
                       int32_t       rulesLength,
                 ثابت char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    ܐܢ (status == nullptr || U_FAILURE(*status)){
        ܦܢܐ nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    ܐܢ(U_FAILURE(*status)) {
        ܦܢܐ nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    ܐܢ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ܦܢܐ uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_openBinaryRules(ثابت uint8_t *binaryRules, int32_t rulesLength,
                     ثابت char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    ܐܢ (U_FAILURE(*status)) {
        ܦܢܐ nullptr;
    }
    ܐܢ (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(جديد RuleBasedBreakIterator(binaryRules, rulesLength, *status), *status);
    ܐܢ (U_FAILURE(*status)) {
        ܦܢܐ nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    ܐܢ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ܦܢܐ uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ثابت UBreakIterator *bi,
          فراغ * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    ܐܢ (status == nullptr || U_FAILURE(*status)){
        ܦܢܐ nullptr;
    }
    ܐܢ (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ nullptr;
    }
    ܐܢ (pBufferSize != nullptr) {
        int32_t inputSize = *pBufferSize;
        *pBufferSize = 1;
        ܐܢ (inputSize == 0) {
            ܦܢܐ nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    ܐܢ (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } ܐܚܪܢܐ ܐܢ (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ܦܢܐ (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ثابت UBreakIterator *bi, UErrorCode *status) {
    ܦܢܐ ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI فراغ U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI فراغ U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             ثابت char16_t*    text,
             int32_t         textLength,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, textLength, status);
    ((BreakIterator*)bi)->setText(&ut, *status);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI فراغ U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ثابت UBreakIterator *bi)
{

  ܦܢܐ ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  ܦܢܐ ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  ܦܢܐ ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  ܦܢܐ ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  ܦܢܐ ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  ܦܢܐ ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  ܦܢܐ ((BreakIterator*)bi)->following(offset);
}

U_CAPI ثابت حرف* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  ܦܢܐ uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_countAvailable()
{

  ܦܢܐ uloc_countAvailable();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    ܦܢܐ ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    ܦܢܐ ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    ܦܢܐ ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI ثابت حرف* U_EXPORT2
ubrk_getLocaleByType(ثابت UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    ܐܢ (bi == nullptr) {
        ܐܢ (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ܦܢܐ nullptr;
    }
    ܦܢܐ ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI فراغ U_EXPORT2
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
    ܐܢ (U_FAILURE(*status)) {
        ܦܢܐ 0;
    }
    ܐܢ ((binaryRules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ 0;
    }
    RuleBasedBreakIterator* rbbi;
    ܐܢ ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ 0;
    }
    uint32_t rulesLength;
    ثابت uint8_t * returnedRules = rbbi->getBinaryRules(rulesLength);
    ܐܢ (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        ܦܢܐ 0;
    }
    ܐܢ (binaryRules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        ܐܢ ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } ܐܚܪܢܐ {
            uprv_memcpy(binaryRules, returnedRules, rulesLength);
        }
    }
    ܦܢܐ (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
