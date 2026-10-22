// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#ضمن "unicode/utypes.h"

#אם !UCONFIG_NO_BREAK_ITERATION

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

  אם (U_FAILURE(*status)) החזר nullptr;

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
  אם (U_FAILURE(*status)) {
    החזר nullptr;
  }
  אם (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    החזר nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  אם (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  החזר uBI;
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

    אם (status == nullptr || U_FAILURE(*status)){
        החזר nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    אם(U_FAILURE(*status)) {
        החזר nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    אם (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    החזר uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_openBinaryRules(ثابت uint8_t *binaryRules, int32_t rulesLength,
                     ثابت char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    אם (U_FAILURE(*status)) {
        החזר nullptr;
    }
    אם (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        החזר nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(جديد RuleBasedBreakIterator(binaryRules, rulesLength, *status), *status);
    אם (U_FAILURE(*status)) {
        החזר nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    אם (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    החזר uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ثابت UBreakIterator *bi,
          فراغ * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    אם (status == nullptr || U_FAILURE(*status)){
        החזר nullptr;
    }
    אם (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        החזר nullptr;
    }
    אם (pBufferSize != nullptr) {
        int32_t inputSize = *pBufferSize;
        *pBufferSize = 1;
        אם (inputSize == 0) {
            החזר nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    אם (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } אחרת אם (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    החזר (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ثابت UBreakIterator *bi, UErrorCode *status) {
    החזר ubrk_safeClone(bi, nullptr, nullptr, status);
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

  החזר ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  החזר ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  החזר ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  החזר ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  החזר ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  החזר ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  החזר ((BreakIterator*)bi)->following(offset);
}

U_CAPI ثابت حرف* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  החזר uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_countAvailable()
{

  החזר uloc_countAvailable();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    החזר ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    החזר ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    החזר ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI ثابت حرف* U_EXPORT2
ubrk_getLocaleByType(ثابت UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    אם (bi == nullptr) {
        אם (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        החזר nullptr;
    }
    החזר ((BreakIterator*)bi)->getLocaleID(type, *status);
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
    אם (U_FAILURE(*status)) {
        החזר 0;
    }
    אם ((binaryRules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        החזר 0;
    }
    RuleBasedBreakIterator* rbbi;
    אם ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        החזר 0;
    }
    uint32_t rulesLength;
    ثابت uint8_t * returnedRules = rbbi->getBinaryRules(rulesLength);
    אם (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        החזר 0;
    }
    אם (binaryRules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        אם ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } אחרת {
            uprv_memcpy(binaryRules, returnedRules, rulesLength);
        }
    }
    החזר (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
