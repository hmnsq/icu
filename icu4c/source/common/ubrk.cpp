// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/ubrk.h"

#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchriter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/rbbi.h"
#ଅନ୍ତର୍ଭାଵ "rbbirb.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      ଅଚଳ ଅକ୍ଷର *locale,
      ଅଚଳ char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  ଯଦି (U_FAILURE(*status)) ଫେରନ୍ତୁ nullptr;

  BreakIterator *result = nullptr;

  ବାଛ(type) {

  ସ୍ଥିତି UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି UBRK_ଧାଡ଼ି:
    result = BreakIterator::create_ଧାଡ଼ି_Instance(Locale(locale), *status);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  ଯଦି (U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }
  ଯଦି (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  ଯଦି (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  ଫେରନ୍ତୁ uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  ଅଚଳ char16_t     *rules,
                       int32_t       rulesLength,
                 ଅଚଳ char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    ଯଦି (status == nullptr || U_FAILURE(*status)){
        ଫେରନ୍ତୁ nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    ଯଦି(U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    ଯଦି (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ଫେରନ୍ତୁ uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ଦ୍ୱିମିକ_Rules(ଅଚଳ uint8_t *ଦ୍ୱିମିକ_Rules, int32_t rulesLength,
                     ଅଚଳ char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(ନୂତନ RuleBasedBreakIterator(ଦ୍ୱିମିକ_Rules, rulesLength, *status), *status);
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    ଯଦି (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ଫେରନ୍ତୁ uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ଅଚଳ UBreakIterator *bi,
          ଶୂନ୍ୟ * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    ଯଦି (status == nullptr || U_FAILURE(*status)){
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (pBufferSize != nullptr) {
        int32_t ନିବେଶ_Size = *pBufferSize;
        *pBufferSize = 1;
        ଯଦି (ନିବେଶ_Size == 0) {
            ଫେରନ୍ତୁ nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    ଯଦି (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } ଅନ୍ୟଥା ଯଦି (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ଫେରନ୍ତୁ (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ଅଚଳ UBreakIterator *bi, UErrorCode *status) {
    ଫେରନ୍ତୁ ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI ଶୂନ୍ୟ U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             ଅଚଳ char16_t*    text,
             int32_t         textLength,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, textLength, status);
    ((BreakIterator*)bi)->setText(&ut, *status);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI ଶୂନ୍ୟ U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ଅଚଳ UBreakIterator *bi)
{

  ଫେରନ୍ତୁ ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  ଫେରନ୍ତୁ ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  ଫେରନ୍ତୁ ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  ଫେରନ୍ତୁ ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  ଫେରନ୍ତୁ ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  ଫେରନ୍ତୁ ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  ଫେରନ୍ତୁ ((BreakIterator*)bi)->following(offset);
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  ଫେରନ୍ତୁ uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_ଗଣନା_Available()
{

  ଫେରନ୍ତୁ uloc_ଗଣନା_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    ଫେରନ୍ତୁ ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    ଫେରନ୍ତୁ ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    ଫେରନ୍ତୁ ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ubrk_getLocaleByType(ଅଚଳ UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    ଯଦି (bi == nullptr) {
        ଯଦି (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI ଶୂନ୍ୟ U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_ନିବେଶ_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_ଦ୍ୱିମିକ_Rules(UBreakIterator *bi,
                    uint8_t *       ଦ୍ୱିମିକ_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି ((ଦ୍ୱିମିକ_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    RuleBasedBreakIterator* rbbi;
    ଯଦି ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    uint32_t rulesLength;
    ଅଚଳ uint8_t * returnedRules = rbbi->get_ଦ୍ୱିମିକ_Rules(rulesLength);
    ଯଦି (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (ଦ୍ୱିମିକ_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        ଯଦି ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } ଅନ୍ୟଥା {
            uprv_memcpy(ଦ୍ୱିମିକ_Rules, returnedRules, rulesLength);
        }
    }
    ଫେରନ୍ତୁ (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
