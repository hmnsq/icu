// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/ubrk.h"

#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uchriter.h"
#अन्तर्भाव "unicode/rbbi.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      स्थिरांक अक्षर *locale,
      स्थिरांक char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  जर (U_FAILURE(*status)) परत nullptr;

  BreakIterator *result = nullptr;

  निवड(type) {

  स्थिती UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    थांबा;

  स्थिती UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    थांबा;

  स्थिती UBRK_ओळ:
    result = BreakIterator::create_ओळ_Instance(Locale(locale), *status);
    थांबा;

  स्थिती UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    थांबा;

  स्थिती UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    थांबा;

  पूर्वनिर्धारित:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  जर (U_FAILURE(*status)) {
    परत nullptr;
  }
  जर (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    परत nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  जर (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  परत uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  स्थिरांक char16_t     *rules,
                       int32_t       rulesLength,
                 स्थिरांक char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    जर (status == nullptr || U_FAILURE(*status)){
        परत nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    जर(U_FAILURE(*status)) {
        परत nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    जर (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    परत uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_द्विमान_Rules(स्थिरांक uint8_t *द्विमान_Rules, int32_t rulesLength,
                     स्थिरांक char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    जर (U_FAILURE(*status)) {
        परत nullptr;
    }
    जर (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(नवीन RuleBasedBreakIterator(द्विमान_Rules, rulesLength, *status), *status);
    जर (U_FAILURE(*status)) {
        परत nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    जर (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    परत uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          स्थिरांक UBreakIterator *bi,
          शून्य * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    जर (status == nullptr || U_FAILURE(*status)){
        परत nullptr;
    }
    जर (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    जर (pBufferSize != nullptr) {
        int32_t आदान_Size = *pBufferSize;
        *pBufferSize = 1;
        जर (आदान_Size == 0) {
            परत nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    जर (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा जर (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    परत (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(स्थिरांक UBreakIterator *bi, UErrorCode *status) {
    परत ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI शून्य U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI शून्य U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             स्थिरांक char16_t*    text,
             int32_t         textLength,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, textLength, status);
    ((BreakIterator*)bi)->setText(&ut, *status);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI शून्य U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(स्थिरांक UBreakIterator *bi)
{

  परत ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  परत ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  परत ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  परत ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  परत ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  परत ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  परत ((BreakIterator*)bi)->following(offset);
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  परत uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_मोजणी_Available()
{

  परत uloc_मोजणी_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    परत ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    परत ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    परत ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI स्थिरांक अक्षर* U_EXPORT2
ubrk_getLocaleByType(स्थिरांक UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    जर (bi == nullptr) {
        जर (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        परत nullptr;
    }
    परत ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI शून्य U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_आदान_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_द्विमान_Rules(UBreakIterator *bi,
                    uint8_t *       द्विमान_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    जर (U_FAILURE(*status)) {
        परत 0;
    }
    जर ((द्विमान_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }
    RuleBasedBreakIterator* rbbi;
    जर ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }
    uint32_t rulesLength;
    स्थिरांक uint8_t * returnedRules = rbbi->get_द्विमान_Rules(rulesLength);
    जर (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        परत 0;
    }
    जर (द्विमान_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        जर ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } अन्यथा {
            uprv_memcpy(द्विमान_Rules, returnedRules, rulesLength);
        }
    }
    परत (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
