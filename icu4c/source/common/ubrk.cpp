// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"

#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchriter.h"
#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"
#ಅನ್ತರ್ಭಾವ "rbbirb.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      ಧ್ರುವ ವರ್ಣ *locale,
      ಧ್ರುವ char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  ಯದಿ (U_FAILURE(*status)) ಪ್ರತ್ಯಾಗಮ nullptr;

  BreakIterator *result = nullptr;

  ವಿಕಲ್ಪ(type) {

  ಪಕ್ಷ UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    ಭಙ್ಗ;

  ಪಕ್ಷ UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    ಭಙ್ಗ;

  ಪಕ್ಷ UBRK_ಸಾಲು:
    result = BreakIterator::create_ಸಾಲು_Instance(Locale(locale), *status);
    ಭಙ್ಗ;

  ಪಕ್ಷ UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    ಭಙ್ಗ;

  ಪಕ್ಷ UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  ಯದಿ (U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  ಯದಿ (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  ಯದಿ (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  ಪ್ರತ್ಯಾಗಮ uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  ಧ್ರುವ char16_t     *rules,
                       int32_t       rulesLength,
                 ಧ್ರುವ char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    ಯದಿ (status == nullptr || U_FAILURE(*status)){
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    ಯದಿ(U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    ಯದಿ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ಪ್ರತ್ಯಾಗಮ uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ದ್ವಿಮಾನ_Rules(ಧ್ರುವ uint8_t *ದ್ವಿಮಾನ_Rules, int32_t rulesLength,
                     ಧ್ರುವ char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(ನೂತನ RuleBasedBreakIterator(ದ್ವಿಮಾನ_Rules, rulesLength, *status), *status);
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    ಯದಿ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    ಪ್ರತ್ಯಾಗಮ uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ಧ್ರುವ UBreakIterator *bi,
          ಶೂನ್ಯ * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    ಯದಿ (status == nullptr || U_FAILURE(*status)){
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (pBufferSize != nullptr) {
        int32_t ಒಳಹರಿವು_Size = *pBufferSize;
        *pBufferSize = 1;
        ಯದಿ (ಒಳಹರಿವು_Size == 0) {
            ಪ್ರತ್ಯಾಗಮ nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    ಯದಿ (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } ಅನ್ಯಥಾ ಯದಿ (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ಪ್ರತ್ಯಾಗಮ (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ಧ್ರುವ UBreakIterator *bi, UErrorCode *status) {
    ಪ್ರತ್ಯಾಗಮ ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             ಧ್ರುವ char16_t*    text,
             int32_t         textLength,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, textLength, status);
    ((BreakIterator*)bi)->setText(&ut, *status);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI ಶೂನ್ಯ U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ಧ್ರುವ UBreakIterator *bi)
{

  ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)bi)->following(offset);
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  ಪ್ರತ್ಯಾಗಮ uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_ಎಣಿಕೆ_Available()
{

  ಪ್ರತ್ಯಾಗಮ uloc_ಎಣಿಕೆ_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ubrk_getLocaleByType(ಧ್ರುವ UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    ಯದಿ (bi == nullptr) {
        ಯದಿ (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_ಒಳಹರಿವು_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_ದ್ವಿಮಾನ_Rules(UBreakIterator *bi,
                    uint8_t *       ದ್ವಿಮಾನ_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ ((ದ್ವಿಮಾನ_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    RuleBasedBreakIterator* rbbi;
    ಯದಿ ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    uint32_t rulesLength;
    ಧ್ರುವ uint8_t * returnedRules = rbbi->get_ದ್ವಿಮಾನ_Rules(rulesLength);
    ಯದಿ (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (ದ್ವಿಮಾನ_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        ಯದಿ ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } ಅನ್ಯಥಾ {
            uprv_memcpy(ದ್ವಿಮಾನ_Rules, returnedRules, rulesLength);
        }
    }
    ಪ್ರತ್ಯಾಗಮ (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
