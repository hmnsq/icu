// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "unicode/ubrk.h"

#અન્તર્ભાવ "unicode/brkiter.h"
#અન્તર્ભાવ "unicode/uloc.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/uchriter.h"
#અન્તર્ભાવ "unicode/rbbi.h"
#અન્તર્ભાવ "rbbirb.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      અચલ અક્ષર *locale,
      અચલ char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  જો (U_FAILURE(*status)) પાછા nullptr;

  BreakIterator *result = nullptr;

  પસંદગી(type) {

  સ્થિતિ UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    તોડો;

  સ્થિતિ UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    તોડો;

  સ્થિતિ UBRK_લીટી:
    result = BreakIterator::create_લીટી_Instance(Locale(locale), *status);
    તોડો;

  સ્થિતિ UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    તોડો;

  સ્થિતિ UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    તોડો;

  મૂળભૂત:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  જો (U_FAILURE(*status)) {
    પાછા nullptr;
  }
  જો (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    પાછા nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  જો (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  પાછા uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  અચલ char16_t     *rules,
                       int32_t       rulesLength,
                 અચલ char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    જો (status == nullptr || U_FAILURE(*status)){
        પાછા nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    જો(U_FAILURE(*status)) {
        પાછા nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    જો (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    પાછા uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_દ્વિઅંકી_Rules(અચલ uint8_t *દ્વિઅંકી_Rules, int32_t rulesLength,
                     અચલ char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    જો (U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(નવું RuleBasedBreakIterator(દ્વિઅંકી_Rules, rulesLength, *status), *status);
    જો (U_FAILURE(*status)) {
        પાછા nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    જો (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    પાછા uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          અચલ UBreakIterator *bi,
          શૂન્ય * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    જો (status == nullptr || U_FAILURE(*status)){
        પાછા nullptr;
    }
    જો (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    જો (pBufferSize != nullptr) {
        int32_t આગત_Size = *pBufferSize;
        *pBufferSize = 1;
        જો (આગત_Size == 0) {
            પાછા nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    જો (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } નહીંતર જો (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    પાછા (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(અચલ UBreakIterator *bi, UErrorCode *status) {
    પાછા ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI શૂન્ય U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI શૂન્ય U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             અચલ char16_t*    text,
             int32_t         textLength,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, textLength, status);
    ((BreakIterator*)bi)->setText(&ut, *status);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI શૂન્ય U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(અચલ UBreakIterator *bi)
{

  પાછા ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  પાછા ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  પાછા ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  પાછા ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  પાછા ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  પાછા ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  પાછા ((BreakIterator*)bi)->following(offset);
}

U_CAPI અચલ અક્ષર* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  પાછા uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_ગણતરી_Available()
{

  પાછા uloc_ગણતરી_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    પાછા ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    પાછા ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    પાછા ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI અચલ અક્ષર* U_EXPORT2
ubrk_getLocaleByType(અચલ UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    જો (bi == nullptr) {
        જો (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        પાછા nullptr;
    }
    પાછા ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI શૂન્ય U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_આગત_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_દ્વિઅંકી_Rules(UBreakIterator *bi,
                    uint8_t *       દ્વિઅંકી_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    જો (U_FAILURE(*status)) {
        પાછા 0;
    }
    જો ((દ્વિઅંકી_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }
    RuleBasedBreakIterator* rbbi;
    જો ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }
    uint32_t rulesLength;
    અચલ uint8_t * returnedRules = rbbi->get_દ્વિઅંકી_Rules(rulesLength);
    જો (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }
    જો (દ્વિઅંકી_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        જો ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } નહીંતર {
            uprv_memcpy(દ્વિઅંકી_Rules, returnedRules, rulesLength);
        }
    }
    પાછા (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
