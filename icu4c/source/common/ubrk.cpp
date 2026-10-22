// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/ubrk.h"

#شامل "unicode/brkiter.h"
#شامل "unicode/uloc.h"
#شامل "unicode/ustring.h"
#شامل "unicode/uchriter.h"
#شامل "unicode/rbbi.h"
#شامل "rbbirb.h"
#شامل "uassert.h"
#شامل "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      ثابت نویسه *locale,
      ثابت char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  اگر (U_FAILURE(*status)) بازگشت nullptr;

  BreakIterator *result = nullptr;

  انتخاب(type) {

  حالت UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    بشکن;

  حالت UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    بشکن;

  حالت UBRK_سطر:
    result = BreakIterator::create_سطر_Instance(Locale(locale), *status);
    بشکن;

  حالت UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    بشکن;

  حالت UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    بشکن;

  پیشفرض:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  اگر (U_FAILURE(*status)) {
    بازگشت nullptr;
  }
  اگر (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    بازگشت nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  اگر (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  بازگشت uBI;
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

    اگر (status == nullptr || U_FAILURE(*status)){
        بازگشت nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    اگر(U_FAILURE(*status)) {
        بازگشت nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    اگر (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    بازگشت uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_دودویی_Rules(ثابت uint8_t *دودویی_Rules, int32_t rulesLength,
                     ثابت char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(جدید RuleBasedBreakIterator(دودویی_Rules, rulesLength, *status), *status);
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    اگر (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    بازگشت uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ثابت UBreakIterator *bi,
          تهی * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    اگر (status == nullptr || U_FAILURE(*status)){
        بازگشت nullptr;
    }
    اگر (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    اگر (pBufferSize != nullptr) {
        int32_t درون_داد_Size = *pBufferSize;
        *pBufferSize = 1;
        اگر (درون_داد_Size == 0) {
            بازگشت nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    اگر (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } وگرنه اگر (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    بازگشت (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ثابت UBreakIterator *bi, UErrorCode *status) {
    بازگشت ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI تهی U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI تهی U_EXPORT2
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



U_CAPI تهی U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ثابت UBreakIterator *bi)
{

  بازگشت ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  بازگشت ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  بازگشت ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  بازگشت ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  بازگشت ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  بازگشت ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  بازگشت ((BreakIterator*)bi)->following(offset);
}

U_CAPI ثابت نویسه* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  بازگشت uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_شمار_Available()
{

  بازگشت uloc_شمار_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    بازگشت ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    بازگشت ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    بازگشت ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI ثابت نویسه* U_EXPORT2
ubrk_getLocaleByType(ثابت UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    اگر (bi == nullptr) {
        اگر (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        بازگشت nullptr;
    }
    بازگشت ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI تهی U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_درون_داد_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_دودویی_Rules(UBreakIterator *bi,
                    uint8_t *       دودویی_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    اگر (U_FAILURE(*status)) {
        بازگشت 0;
    }
    اگر ((دودویی_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    RuleBasedBreakIterator* rbbi;
    اگر ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    uint32_t rulesLength;
    ثابت uint8_t * returnedRules = rbbi->get_دودویی_Rules(rulesLength);
    اگر (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0;
    }
    اگر (دودویی_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        اگر ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } وگرنه {
            uprv_memcpy(دودویی_Rules, returnedRules, rulesLength);
        }
    }
    بازگشت (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
