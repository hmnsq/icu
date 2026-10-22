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
      ধ্রুবক অক্ষর *locale,
      ধ্রুবক char16_t *text,
      int32_t text_দৈর্ঘ্য,
      UErrorCode *status)
{

  যদি (U_FAILURE(*status)) ফেরত nullptr;

  BreakIterator *ফলাফল = nullptr;

  নির্বাচন(type) {

  ক্ষেত্র UBRK_CHARACTER:
    ফলাফল = BreakIterator::createCharacterInstance(Locale(locale), *status);
    বিরতি;

  ক্ষেত্র UBRK_WORD:
    ফলাফল = BreakIterator::createWordInstance(Locale(locale), *status);
    বিরতি;

  ক্ষেত্র UBRK_সারি:
    ফলাফল = BreakIterator::create_সারি_Instance(Locale(locale), *status);
    বিরতি;

  ক্ষেত্র UBRK_SENTENCE:
    ফলাফল = BreakIterator::createSentenceInstance(Locale(locale), *status);
    বিরতি;

  ক্ষেত্র UBRK_TITLE:
    ফলাফল = BreakIterator::createTitleInstance(Locale(locale), *status);
    বিরতি;

  পূর্বনির্ধারিত:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  যদি (U_FAILURE(*status)) {
    ফেরত nullptr;
  }
  যদি (ফলাফল == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ফেরত nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)ফলাফল;
  যদি (text != nullptr) {
      ubrk_setText(uBI, text, text_দৈর্ঘ্য, status);
  }
  ফেরত uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  ধ্রুবক char16_t     *rules,
                       int32_t       rules_দৈর্ঘ্য,
                 ধ্রুবক char16_t     *text,
                       int32_t       text_দৈর্ঘ্য,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    যদি (status == nullptr || U_FAILURE(*status)){
        ফেরত nullptr;
    }

    BreakIterator *ফলাফল = nullptr;
    UnicodeString ruleString(rules, rules_দৈর্ঘ্য);
    ফলাফল = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    যদি(U_FAILURE(*status)) {
        ফেরত nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)ফলাফল;
    যদি (text != nullptr) {
        ubrk_setText(uBI, text, text_দৈর্ঘ্য, status);
    }
    ফেরত uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_দ্বিমিক_Rules(ধ্রুবক uint8_t *দ্বিমিক_Rules, int32_t rules_দৈর্ঘ্য,
                     ধ্রুবক char16_t *  text, int32_t text_দৈর্ঘ্য,
                     UErrorCode *   status)
{
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি (rules_দৈর্ঘ্য < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(নতুন RuleBasedBreakIterator(দ্বিমিক_Rules, rules_দৈর্ঘ্য, *status), *status);
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    যদি (text != nullptr) {
        ubrk_setText(uBI, text, text_দৈর্ঘ্য, status);
    }
    ফেরত uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ধ্রুবক UBreakIterator *bi,
          শূন্য * /*stackBuffer*/,
          int32_t *p_বাফার_আকার,
          UErrorCode *status)
{
    যদি (status == nullptr || U_FAILURE(*status)){
        ফেরত nullptr;
    }
    যদি (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    যদি (p_বাফার_আকার != nullptr) {
        int32_t প্রবেশ_আকার = *p_বাফার_আকার;
        *p_বাফার_আকার = 1;
        যদি (প্রবেশ_আকার == 0) {
            ফেরত nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    যদি (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা যদি (p_বাফার_আকার != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ফেরত (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ধ্রুবক UBreakIterator *bi, UErrorCode *status) {
    ফেরত ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI শূন্য U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI শূন্য U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             ধ্রুবক char16_t*    text,
             int32_t         text_দৈর্ঘ্য,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, text_দৈর্ঘ্য, status);
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
ubrk_current(ধ্রুবক UBreakIterator *bi)
{

  ফেরত ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_পরবর্তী(UBreakIterator *bi)
{

  ফেরত ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  ফেরত ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  ফেরত ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  ফেরত ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  ফেরত ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  ফেরত ((BreakIterator*)bi)->following(offset);
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ubrk_getAvailable(int32_t সূচক)
{

  ফেরত uloc_getAvailable(সূচক);
}

U_CAPI int32_t U_EXPORT2
ubrk_গুনতি_Available()
{

  ফেরত uloc_গুনতি_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    ফেরত ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    ফেরত ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    ফেরত ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ubrk_getLocaleByType(ধ্রুবক UBreakIterator* bi U_LIFETIME_BOUND,
                     ULoc_তথ্য_Locale_Type type,
                     UErrorCode* status)
{
    যদি (bi == nullptr) {
        যদি (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ফেরত nullptr;
    }
    ফেরত ((BreakIterator*)bi)->getLocaleID(type, *status);
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
        ফেরত 0;
    }
    যদি ((দ্বিমিক_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    RuleBasedBreakIterator* rbbi;
    যদি ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    uint32_t rules_দৈর্ঘ্য;
    ধ্রুবক uint8_t * returnedRules = rbbi->get_দ্বিমিক_Rules(rules_দৈর্ঘ্য);
    যদি (rules_দৈর্ঘ্য > INT32_MAX) {
        *status = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }
    যদি (দ্বিমিক_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        যদি ((int32_t)rules_দৈর্ঘ্য > rulesCapacity) {
            *status = U_বাফার_OVERFLOW_ERROR;
        } অন্যথা {
            uprv_memcpy(দ্বিমিক_Rules, returnedRules, rules_দৈর্ঘ্য);
        }
    }
    ফেরত (int32_t)rules_দৈর্ঘ্য;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
