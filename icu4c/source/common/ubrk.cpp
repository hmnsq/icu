// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#जइ !UCONFIG_NO_BREAK_ITERATION

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

  जइ (U_FAILURE(*status)) प्रत्यागम nullptr;

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
  जइ (U_FAILURE(*status)) {
    प्रत्यागम nullptr;
  }
  जइ (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    प्रत्यागम nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  जइ (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  प्रत्यागम uBI;
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

    जइ (status == nullptr || U_FAILURE(*status)){
        प्रत्यागम nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    जइ(U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    जइ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    प्रत्यागम uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_द्विमान_Rules(ध्रुव uint8_t *द्विमान_Rules, int32_t rulesLength,
                     ध्रुव char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    जइ (U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(नूतन RuleBasedBreakIterator(द्विमान_Rules, rulesLength, *status), *status);
    जइ (U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    जइ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    प्रत्यागम uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ध्रुव UBreakIterator *bi,
          शून्य * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    जइ (status == nullptr || U_FAILURE(*status)){
        प्रत्यागम nullptr;
    }
    जइ (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    जइ (pBufferSize != nullptr) {
        int32_t निवेशः_Size = *pBufferSize;
        *pBufferSize = 1;
        जइ (निवेशः_Size == 0) {
            प्रत्यागम nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    जइ (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } अण्णहा जइ (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    प्रत्यागम (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ध्रुव UBreakIterator *bi, UErrorCode *status) {
    प्रत्यागम ubrk_safeClone(bi, nullptr, nullptr, status);
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

  प्रत्यागम ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  प्रत्यागम ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  प्रत्यागम ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  प्रत्यागम ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  प्रत्यागम ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  प्रत्यागम ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  प्रत्यागम ((BreakIterator*)bi)->following(offset);
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  प्रत्यागम uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_गणनम्_Available()
{

  प्रत्यागम uloc_गणनम्_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    प्रत्यागम ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    प्रत्यागम ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    प्रत्यागम ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI ध्रुव वर्ण* U_EXPORT2
ubrk_getLocaleByType(ध्रुव UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    जइ (bi == nullptr) {
        जइ (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        प्रत्यागम nullptr;
    }
    प्रत्यागम ((BreakIterator*)bi)->getLocaleID(type, *status);
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
    जइ (U_FAILURE(*status)) {
        प्रत्यागम 0;
    }
    जइ ((द्विमान_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    RuleBasedBreakIterator* rbbi;
    जइ ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    uint32_t rulesLength;
    ध्रुव uint8_t * returnedRules = rbbi->get_द्विमान_Rules(rulesLength);
    जइ (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }
    जइ (द्विमान_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        जइ ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } अण्णहा {
            uprv_memcpy(द्विमान_Rules, returnedRules, rulesLength);
        }
    }
    प्रत्यागम (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
