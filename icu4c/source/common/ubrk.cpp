// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "unicode/ubrk.h"

#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/uloc.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/uchriter.h"
#அந்தர்பாவ "unicode/rbbi.h"
#அந்தர்பாவ "rbbirb.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      மாறிலி எழுத்து *locale,
      மாறிலி char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  எனில் (U_FAILURE(*status)) திரும்பு nullptr;

  BreakIterator *result = nullptr;

  தேர்வு(type) {

  நிலை UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    நிறுத்து;

  நிலை UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    நிறுத்து;

  நிலை UBRK_வரி:
    result = BreakIterator::create_வரி_Instance(Locale(locale), *status);
    நிறுத்து;

  நிலை UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    நிறுத்து;

  நிலை UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    நிறுத்து;

  இயல்பு:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  எனில் (U_FAILURE(*status)) {
    திரும்பு nullptr;
  }
  எனில் (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    திரும்பு nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  எனில் (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  திரும்பு uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  மாறிலி char16_t     *rules,
                       int32_t       rulesLength,
                 மாறிலி char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    எனில் (status == nullptr || U_FAILURE(*status)){
        திரும்பு nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    எனில்(U_FAILURE(*status)) {
        திரும்பு nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    எனில் (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    திரும்பு uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_இருமம்_Rules(மாறிலி uint8_t *இருமம்_Rules, int32_t rulesLength,
                     மாறிலி char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில் (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(புதிய RuleBasedBreakIterator(இருமம்_Rules, rulesLength, *status), *status);
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    எனில் (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    திரும்பு uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          மாறிலி UBreakIterator *bi,
          வெறுமை * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    எனில் (status == nullptr || U_FAILURE(*status)){
        திரும்பு nullptr;
    }
    எனில் (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    எனில் (pBufferSize != nullptr) {
        int32_t உள்ளீடு_Size = *pBufferSize;
        *pBufferSize = 1;
        எனில் (உள்ளீடு_Size == 0) {
            திரும்பு nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    எனில் (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } இல்லையெனில் எனில் (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    திரும்பு (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(மாறிலி UBreakIterator *bi, UErrorCode *status) {
    திரும்பு ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI வெறுமை U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI வெறுமை U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             மாறிலி char16_t*    text,
             int32_t         textLength,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, textLength, status);
    ((BreakIterator*)bi)->setText(&ut, *status);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI வெறுமை U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(மாறிலி UBreakIterator *bi)
{

  திரும்பு ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  திரும்பு ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  திரும்பு ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  திரும்பு ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  திரும்பு ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  திரும்பு ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  திரும்பு ((BreakIterator*)bi)->following(offset);
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  திரும்பு uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_மொத்தம்_Available()
{

  திரும்பு uloc_மொத்தம்_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    திரும்பு ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    திரும்பு ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    திரும்பு ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI மாறிலி எழுத்து* U_EXPORT2
ubrk_getLocaleByType(மாறிலி UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    எனில் (bi == nullptr) {
        எனில் (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        திரும்பு nullptr;
    }
    திரும்பு ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI வெறுமை U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_உள்ளீடு_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_இருமம்_Rules(UBreakIterator *bi,
                    uint8_t *       இருமம்_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    எனில் (U_FAILURE(*status)) {
        திரும்பு 0;
    }
    எனில் ((இருமம்_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    RuleBasedBreakIterator* rbbi;
    எனில் ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    uint32_t rulesLength;
    மாறிலி uint8_t * returnedRules = rbbi->get_இருமம்_Rules(rulesLength);
    எனில் (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }
    எனில் (இருமம்_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        எனில் ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } இல்லையெனில் {
            uprv_memcpy(இருமம்_Rules, returnedRules, rulesLength);
        }
    }
    திரும்பு (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
