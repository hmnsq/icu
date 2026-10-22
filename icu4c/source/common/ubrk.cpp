// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "unicode/ubrk.h"

#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/uloc.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/uchriter.h"
#অন্তর্ভাব "unicode/rbbi.h"
#অন্তর্ভাব "rbbirb.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      ধ্রুব বর্ণ *locale,
      ধ্রুব char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  যদি (U_FAILURE(*status)) প্রত্যাগম nullptr;

  BreakIterator *result = nullptr;

  বিকল্প(type) {

  পক্ষ UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    ভঙ্গ;

  পক্ষ UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    ভঙ্গ;

  পক্ষ UBRK_সারি:
    result = BreakIterator::create_সারি_Instance(Locale(locale), *status);
    ভঙ্গ;

  পক্ষ UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    ভঙ্গ;

  পক্ষ UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    ভঙ্গ;

  স্বভাব:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  যদি (U_FAILURE(*status)) {
    প্রত্যাগম nullptr;
  }
  যদি (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    প্রত্যাগম nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  যদি (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  প্রত্যাগম uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  ধ্রুব char16_t     *rules,
                       int32_t       rulesLength,
                 ধ্রুব char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    যদি (status == nullptr || U_FAILURE(*status)){
        প্রত্যাগম nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    যদি(U_FAILURE(*status)) {
        প্রত্যাগম nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    যদি (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    প্রত্যাগম uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_দ্বিমিক_Rules(ধ্রুব uint8_t *দ্বিমিক_Rules, int32_t rulesLength,
                     ধ্রুব char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    যদি (U_FAILURE(*status)) {
        প্রত্যাগম nullptr;
    }
    যদি (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(নূতন RuleBasedBreakIterator(দ্বিমিক_Rules, rulesLength, *status), *status);
    যদি (U_FAILURE(*status)) {
        প্রত্যাগম nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    যদি (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    প্রত্যাগম uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ধ্রুব UBreakIterator *bi,
          শূন্য * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    যদি (status == nullptr || U_FAILURE(*status)){
        প্রত্যাগম nullptr;
    }
    যদি (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }
    যদি (pBufferSize != nullptr) {
        int32_t প্রবেশ_Size = *pBufferSize;
        *pBufferSize = 1;
        যদি (প্রবেশ_Size == 0) {
            প্রত্যাগম nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    যদি (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা যদি (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    প্রত্যাগম (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ধ্রুব UBreakIterator *bi, UErrorCode *status) {
    প্রত্যাগম ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI শূন্য U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI শূন্য U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             ধ্রুব char16_t*    text,
             int32_t         textLength,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, textLength, status);
    ((BreakIterator*)bi)->setText(&ut, *status);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI শূন্য U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ধ্রুব UBreakIterator *bi)
{

  প্রত্যাগম ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  প্রত্যাগম ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  প্রত্যাগম ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  প্রত্যাগম ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  প্রত্যাগম ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  প্রত্যাগম ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  প্রত্যাগম ((BreakIterator*)bi)->following(offset);
}

U_CAPI ধ্রুব বর্ণ* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  প্রত্যাগম uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_গুনতি_Available()
{

  প্রত্যাগম uloc_গুনতি_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    প্রত্যাগম ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    প্রত্যাগম ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    প্রত্যাগম ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI ধ্রুব বর্ণ* U_EXPORT2
ubrk_getLocaleByType(ধ্রুব UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    যদি (bi == nullptr) {
        যদি (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        প্রত্যাগম nullptr;
    }
    প্রত্যাগম ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI শূন্য U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_প্রবেশ_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_দ্বিমিক_Rules(UBreakIterator *bi,
                    uint8_t *       দ্বিমিক_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    যদি (U_FAILURE(*status)) {
        প্রত্যাগম 0;
    }
    যদি ((দ্বিমিক_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }
    RuleBasedBreakIterator* rbbi;
    যদি ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }
    uint32_t rulesLength;
    ধ্রুব uint8_t * returnedRules = rbbi->get_দ্বিমিক_Rules(rulesLength);
    যদি (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0;
    }
    যদি (দ্বিমিক_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        যদি ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } অন্যথা {
            uprv_memcpy(দ্বিমিক_Rules, returnedRules, rulesLength);
        }
    }
    প্রত্যাগম (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
