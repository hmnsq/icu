// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

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

  إذا (U_FAILURE(*status)) أرجع nullptr;

  BreakIterator *result = nullptr;

  بدل(type) {

  حالة UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    اكسر;

  حالة UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    اكسر;

  حالة UBRK_سطر:
    result = BreakIterator::create_سطر_Instance(Locale(locale), *status);
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
  إذا (U_FAILURE(*status)) {
    أرجع nullptr;
  }
  إذا (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    أرجع nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  إذا (text != nullptr) {
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

    إذا (status == nullptr || U_FAILURE(*status)){
        أرجع nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    إذا(U_FAILURE(*status)) {
        أرجع nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    إذا (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    أرجع uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ثنائي_Rules(ثابت uint8_t *ثنائي_Rules, int32_t rulesLength,
                     ثابت char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    إذا (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    إذا (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(جديد RuleBasedBreakIterator(ثنائي_Rules, rulesLength, *status), *status);
    إذا (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    إذا (text != nullptr) {
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
    إذا (status == nullptr || U_FAILURE(*status)){
        أرجع nullptr;
    }
    إذا (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    إذا (pBufferSize != nullptr) {
        int32_t مدخل_Size = *pBufferSize;
        *pBufferSize = 1;
        إذا (مدخل_Size == 0) {
            أرجع nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    إذا (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } وإلا إذا (pBufferSize != nullptr) {
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
ubrk_عدد_Available()
{

  أرجع uloc_عدد_Available();
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
    إذا (bi == nullptr) {
        إذا (U_SUCCESS(*status)) {
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
    bii->refresh_مدخل_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_ثنائي_Rules(UBreakIterator *bi,
                    uint8_t *       ثنائي_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    إذا (U_FAILURE(*status)) {
        أرجع 0;
    }
    إذا ((ثنائي_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    RuleBasedBreakIterator* rbbi;
    إذا ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    uint32_t rulesLength;
    ثابت uint8_t * returnedRules = rbbi->get_ثنائي_Rules(rulesLength);
    إذا (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    إذا (ثنائي_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        إذا ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } وإلا {
            uprv_memcpy(ثنائي_Rules, returnedRules, rulesLength);
        }
    }
    أرجع (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
