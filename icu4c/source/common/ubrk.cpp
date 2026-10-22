// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/ubrk.h"

#समावेश "unicode/brkiter.h"
#समावेश "unicode/uloc.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/uchriter.h"
#समावेश "unicode/rbbi.h"
#समावेश "rbbirb.h"
#समावेश "uassert.h"
#समावेश "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      स्थिर अक्षर *locale,
      स्थिर char16_t *text,
      int32_t text_लंबाई,
      UErrorCode *status)
{

  अगर (U_FAILURE(*status)) वापस nullptr;

  BreakIterator *परिणाम = nullptr;

  चयन(type) {

  हाल UBRK_CHARACTER:
    परिणाम = BreakIterator::createCharacterInstance(Locale(locale), *status);
    अवरोध;

  हाल UBRK_WORD:
    परिणाम = BreakIterator::createWordInstance(Locale(locale), *status);
    अवरोध;

  हाल UBRK_पंक्ति:
    परिणाम = BreakIterator::create_पंक्ति_Instance(Locale(locale), *status);
    अवरोध;

  हाल UBRK_SENTENCE:
    परिणाम = BreakIterator::createSentenceInstance(Locale(locale), *status);
    अवरोध;

  हाल UBRK_TITLE:
    परिणाम = BreakIterator::createTitleInstance(Locale(locale), *status);
    अवरोध;

  शेष:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  अगर (U_FAILURE(*status)) {
    वापस nullptr;
  }
  अगर (परिणाम == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    वापस nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)परिणाम;
  अगर (text != nullptr) {
      ubrk_setText(uBI, text, text_लंबाई, status);
  }
  वापस uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  स्थिर char16_t     *rules,
                       int32_t       rules_लंबाई,
                 स्थिर char16_t     *text,
                       int32_t       text_लंबाई,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    अगर (status == nullptr || U_FAILURE(*status)){
        वापस nullptr;
    }

    BreakIterator *परिणाम = nullptr;
    UnicodeString ruleString(rules, rules_लंबाई);
    परिणाम = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    अगर(U_FAILURE(*status)) {
        वापस nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)परिणाम;
    अगर (text != nullptr) {
        ubrk_setText(uBI, text, text_लंबाई, status);
    }
    वापस uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_द्विआधारी_Rules(स्थिर uint8_t *द्विआधारी_Rules, int32_t rules_लंबाई,
                     स्थिर char16_t *  text, int32_t text_लंबाई,
                     UErrorCode *   status)
{
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर (rules_लंबाई < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(नया RuleBasedBreakIterator(द्विआधारी_Rules, rules_लंबाई, *status), *status);
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    अगर (text != nullptr) {
        ubrk_setText(uBI, text, text_लंबाई, status);
    }
    वापस uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          स्थिर UBreakIterator *bi,
          व्योम * /*stackBuffer*/,
          int32_t *p_बफर_आकार,
          UErrorCode *status)
{
    अगर (status == nullptr || U_FAILURE(*status)){
        वापस nullptr;
    }
    अगर (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    अगर (p_बफर_आकार != nullptr) {
        int32_t निवेश_आकार = *p_बफर_आकार;
        *p_बफर_आकार = 1;
        अगर (निवेश_आकार == 0) {
            वापस nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    अगर (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा अगर (p_बफर_आकार != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    वापस (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(स्थिर UBreakIterator *bi, UErrorCode *status) {
    वापस ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI व्योम U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    मिटाओ (BreakIterator *)bi;
}

U_CAPI व्योम U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             स्थिर char16_t*    text,
             int32_t         text_लंबाई,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, text_लंबाई, status);
    ((BreakIterator*)bi)->setText(&ut, *status);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI व्योम U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(स्थिर UBreakIterator *bi)
{

  वापस ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_अग्र(UBreakIterator *bi)
{

  वापस ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  वापस ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  वापस ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  वापस ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  वापस ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  वापस ((BreakIterator*)bi)->following(offset);
}

U_CAPI स्थिर अक्षर* U_EXPORT2
ubrk_getAvailable(int32_t सूचकांक)
{

  वापस uloc_getAvailable(सूचकांक);
}

U_CAPI int32_t U_EXPORT2
ubrk_गिनती_Available()
{

  वापस uloc_गिनती_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    वापस ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    वापस ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    वापस ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI स्थिर अक्षर* U_EXPORT2
ubrk_getLocaleByType(स्थिर UBreakIterator* bi U_LIFETIME_BOUND,
                     ULoc_तथ्य_Locale_Type type,
                     UErrorCode* status)
{
    अगर (bi == nullptr) {
        अगर (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        वापस nullptr;
    }
    वापस ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI व्योम U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_निवेश_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_द्विआधारी_Rules(UBreakIterator *bi,
                    uint8_t *       द्विआधारी_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    अगर (U_FAILURE(*status)) {
        वापस 0;
    }
    अगर ((द्विआधारी_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    RuleBasedBreakIterator* rbbi;
    अगर ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    uint32_t rules_लंबाई;
    स्थिर uint8_t * returnedRules = rbbi->get_द्विआधारी_Rules(rules_लंबाई);
    अगर (rules_लंबाई > INT32_MAX) {
        *status = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }
    अगर (द्विआधारी_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        अगर ((int32_t)rules_लंबाई > rulesCapacity) {
            *status = U_बफर_OVERFLOW_ERROR;
        } अन्यथा {
            uprv_memcpy(द्विआधारी_Rules, returnedRules, rules_लंबाई);
        }
    }
    वापस (int32_t)rules_लंबाई;
}


#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */
