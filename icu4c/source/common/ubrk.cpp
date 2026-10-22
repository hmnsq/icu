// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_BREAK_ITERATION

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
      ध्रुव वर्ण *locale,
      ध्रुव char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  सचे (U_FAILURE(*status)) पटिनिवत्त nullptr;

  BreakIterator *result = nullptr;

  विकल्प(type) {

  पक्ष UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    भङ्ग;

  पक्ष UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    भङ्ग;

  पक्ष UBRK_पङ्क्तिः:
    result = BreakIterator::create_पङ्क्तिः_Instance(Locale(locale), *status);
    भङ्ग;

  पक्ष UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    भङ्ग;

  पक्ष UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    भङ्ग;

  स्वभाव:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  सचे (U_FAILURE(*status)) {
    पटिनिवत्त nullptr;
  }
  सचे (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    पटिनिवत्त nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  सचे (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  पटिनिवत्त uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  ध्रुव char16_t     *rules,
                       int32_t       rulesLength,
                 ध्रुव char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    सचे (status == nullptr || U_FAILURE(*status)){
        पटिनिवत्त nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    सचे(U_FAILURE(*status)) {
        पटिनिवत्त nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    सचे (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    पटिनिवत्त uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_द्विमान_Rules(ध्रुव uint8_t *द्विमान_Rules, int32_t rulesLength,
                     ध्रुव char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    सचे (U_FAILURE(*status)) {
        पटिनिवत्त nullptr;
    }
    सचे (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(नूतन RuleBasedBreakIterator(द्विमान_Rules, rulesLength, *status), *status);
    सचे (U_FAILURE(*status)) {
        पटिनिवत्त nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    सचे (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    पटिनिवत्त uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ध्रुव UBreakIterator *bi,
          शून्य * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    सचे (status == nullptr || U_FAILURE(*status)){
        पटिनिवत्त nullptr;
    }
    सचे (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    सचे (pBufferSize != nullptr) {
        int32_t निवेशः_Size = *pBufferSize;
        *pBufferSize = 1;
        सचे (निवेशः_Size == 0) {
            पटिनिवत्त nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    सचे (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } अञ्ञथा सचे (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    पटिनिवत्त (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ध्रुव UBreakIterator *bi, UErrorCode *status) {
    पटिनिवत्त ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI शून्य U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI शून्य U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             ध्रुव char16_t*    text,
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
ubrk_current(ध्रुव UBreakIterator *bi)
{

  पटिनिवत्त ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  पटिनिवत्त ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  पटिनिवत्त ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  पटिनिवत्त ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  पटिनिवत्त ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  पटिनिवत्त ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  पटिनिवत्त ((BreakIterator*)bi)->following(offset);
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  पटिनिवत्त uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_गणनम्_Available()
{

  पटिनिवत्त uloc_गणनम्_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    पटिनिवत्त ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    पटिनिवत्त ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    पटिनिवत्त ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI ध्रुव वर्ण* U_EXPORT2
ubrk_getLocaleByType(ध्रुव UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    सचे (bi == nullptr) {
        सचे (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI शून्य U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_निवेशः_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_द्विमान_Rules(UBreakIterator *bi,
                    uint8_t *       द्विमान_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    सचे (U_FAILURE(*status)) {
        पटिनिवत्त 0;
    }
    सचे ((द्विमान_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }
    RuleBasedBreakIterator* rbbi;
    सचे ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }
    uint32_t rulesLength;
    ध्रुव uint8_t * returnedRules = rbbi->get_द्विमान_Rules(rulesLength);
    सचे (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त 0;
    }
    सचे (द्विमान_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        सचे ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } अञ्ञथा {
            uprv_memcpy(द्विमान_Rules, returnedRules, rulesLength);
        }
    }
    पटिनिवत्त (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
