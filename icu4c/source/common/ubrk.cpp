// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#ضمن "unicode/utypes.h"

#𐤀𐤌 !UCONFIG_NO_BREAK_ITERATION

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

  𐤀𐤌 (U_FAILURE(*status)) أرجع nullptr;

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
  𐤀𐤌 (U_FAILURE(*status)) {
    أرجع nullptr;
  }
  𐤀𐤌 (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    أرجع nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  𐤀𐤌 (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  أرجع uBI;
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

    𐤀𐤌 (status == nullptr || U_FAILURE(*status)){
        أرجع nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    𐤀𐤌(U_FAILURE(*status)) {
        أرجع nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    𐤀𐤌 (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    أرجع uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_openBinaryRules(ثابت uint8_t *binaryRules, int32_t rulesLength,
                     ثابت char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    𐤀𐤌 (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    𐤀𐤌 (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(جديد RuleBasedBreakIterator(binaryRules, rulesLength, *status), *status);
    𐤀𐤌 (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    𐤀𐤌 (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    أرجع uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ثابت UBreakIterator *bi,
          فراغ * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    𐤀𐤌 (status == nullptr || U_FAILURE(*status)){
        أرجع nullptr;
    }
    𐤀𐤌 (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    𐤀𐤌 (pBufferSize != nullptr) {
        int32_t inputSize = *pBufferSize;
        *pBufferSize = 1;
        𐤀𐤌 (inputSize == 0) {
            أرجع nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    𐤀𐤌 (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } وإلا 𐤀𐤌 (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    أرجع (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ثابت UBreakIterator *bi, UErrorCode *status) {
    أرجع ubrk_safeClone(bi, nullptr, nullptr, status);
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

  أرجع ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  أرجع ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  أرجع ((BreakIterator*)bi)->following(offset);
}

U_CAPI ثابت حرف* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  أرجع uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_countAvailable()
{

  أرجع uloc_countAvailable();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    أرجع ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    أرجع ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    أرجع ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI ثابت حرف* U_EXPORT2
ubrk_getLocaleByType(ثابت UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    𐤀𐤌 (bi == nullptr) {
        𐤀𐤌 (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        أرجع nullptr;
    }
    أرجع ((BreakIterator*)bi)->getLocaleID(type, *status);
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
    𐤀𐤌 (U_FAILURE(*status)) {
        أرجع 0;
    }
    𐤀𐤌 ((binaryRules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    RuleBasedBreakIterator* rbbi;
    𐤀𐤌 ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    uint32_t rulesLength;
    ثابت uint8_t * returnedRules = rbbi->getBinaryRules(rulesLength);
    𐤀𐤌 (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    𐤀𐤌 (binaryRules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        𐤀𐤌 ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } وإلا {
            uprv_memcpy(binaryRules, returnedRules, rulesLength);
        }
    }
    أرجع (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
