// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাৱ "unicode/ubrk.h"

#অন্তর্ভাৱ "unicode/brkiter.h"
#অন্তর্ভাৱ "unicode/uloc.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/uchriter.h"
#অন্তর্ভাৱ "unicode/rbbi.h"
#অন্তর্ভাৱ "rbbirb.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      ধ্রুৱ ৱর্ণ *locale,
      ধ্রুৱ char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  যদি (U_FAILURE(*status)) ঘূৰাই nullptr;

  BreakIterator *result = nullptr;

  ৱিকল্প(type) {

  পক্ষ UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    ভঙ্গ;

  পক্ষ UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    ভঙ্গ;

  পক্ষ UBRK_সাৰি:
    result = BreakIterator::create_সাৰি_Instance(Locale(locale), *status);
    ভঙ্গ;

  পক্ষ UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    ভঙ্গ;

  পক্ষ UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    ভঙ্গ;

  স্ৱভাৱ:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  যদি (U_FAILURE(*status)) {
    ঘূৰাই nullptr;
  }
  যদি (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ঘূৰাই nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  যদি (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  ঘূৰাই uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  ধ্রুৱ char16_t     *rules,
                       int32_t       rulesLength,
                 ধ্রুৱ char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    যদি (status == nullptr || U_FAILURE(*status)){
        ঘূৰাই nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    যদি(U_FAILURE(*status)) {
        ঘূৰাই nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    যদি (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ঘূৰাই uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_দ্বিমিক_Rules(ধ্রুৱ uint8_t *দ্বিমিক_Rules, int32_t rulesLength,
                     ধ্রুৱ char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    যদি (U_FAILURE(*status)) {
        ঘূৰাই nullptr;
    }
    যদি (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(নূতন RuleBasedBreakIterator(দ্বিমিক_Rules, rulesLength, *status), *status);
    যদি (U_FAILURE(*status)) {
        ঘূৰাই nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    যদি (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ঘূৰাই uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ধ্রুৱ UBreakIterator *bi,
          শূন্য * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    যদি (status == nullptr || U_FAILURE(*status)){
        ঘূৰাই nullptr;
    }
    যদি (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }
    যদি (pBufferSize != nullptr) {
        int32_t প্ৰবেশ_Size = *pBufferSize;
        *pBufferSize = 1;
        যদি (প্ৰবেশ_Size == 0) {
            ঘূৰাই nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    যদি (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা যদি (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ঘূৰাই (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ধ্রুৱ UBreakIterator *bi, UErrorCode *status) {
    ঘূৰাই ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI শূন্য U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI শূন্য U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             ধ্রুৱ char16_t*    text,
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
ubrk_current(ধ্রুৱ UBreakIterator *bi)
{

  ঘূৰাই ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  ঘূৰাই ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  ঘূৰাই ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  ঘূৰাই ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  ঘূৰাই ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  ঘূৰাই ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  ঘূৰাই ((BreakIterator*)bi)->following(offset);
}

U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  ঘূৰাই uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_গুনতি_Available()
{

  ঘূৰাই uloc_গুনতি_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    ঘূৰাই ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    ঘূৰাই ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    ঘূৰাই ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
ubrk_getLocaleByType(ধ্রুৱ UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    যদি (bi == nullptr) {
        যদি (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ঘূৰাই nullptr;
    }
    ঘূৰাই ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI শূন্য U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_প্ৰবেশ_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_দ্বিমিক_Rules(UBreakIterator *bi,
                    uint8_t *       দ্বিমিক_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    যদি (U_FAILURE(*status)) {
        ঘূৰাই 0;
    }
    যদি ((দ্বিমিক_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }
    RuleBasedBreakIterator* rbbi;
    যদি ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }
    uint32_t rulesLength;
    ধ্রুৱ uint8_t * returnedRules = rbbi->get_দ্বিমিক_Rules(rulesLength);
    যদি (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        ঘূৰাই 0;
    }
    যদি (দ্বিমিক_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        যদি ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } অন্যথা {
            uprv_memcpy(দ্বিমিক_Rules, returnedRules, rulesLength);
        }
    }
    ঘূৰাই (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
