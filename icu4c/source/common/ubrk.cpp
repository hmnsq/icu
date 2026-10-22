// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/ubrk.h"

#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/uchriter.h"
#അന്തര്ഭാവ "unicode/rbbi.h"
#അന്തര്ഭാവ "rbbirb.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      സ്ഥിരം അക്ഷരം *locale,
      സ്ഥിരം char16_t *text,
      int32_t textLength,
      UErrorCode *status)
{

  എങ്കിൽ (U_FAILURE(*status)) തിരികെ nullptr;

  BreakIterator *result = nullptr;

  തിരഞ്ഞെടുപ്പ്(type) {

  അവസ്ഥ UBRK_CHARACTER:
    result = BreakIterator::createCharacterInstance(Locale(locale), *status);
    നിർത്തുക;

  അവസ്ഥ UBRK_WORD:
    result = BreakIterator::createWordInstance(Locale(locale), *status);
    നിർത്തുക;

  അവസ്ഥ UBRK_വരി:
    result = BreakIterator::create_വരി_Instance(Locale(locale), *status);
    നിർത്തുക;

  അവസ്ഥ UBRK_SENTENCE:
    result = BreakIterator::createSentenceInstance(Locale(locale), *status);
    നിർത്തുക;

  അവസ്ഥ UBRK_TITLE:
    result = BreakIterator::createTitleInstance(Locale(locale), *status);
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    *status = U_ILLEGAL_ARGUMENT_ERROR;
  }

  // check for allocation error
  എങ്കിൽ (U_FAILURE(*status)) {
    തിരികെ nullptr;
  }
  എങ്കിൽ (result == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    തിരികെ nullptr;
  }


  UBreakIterator *uBI = (UBreakIterator *)result;
  എങ്കിൽ (text != nullptr) {
      ubrk_setText(uBI, text, textLength, status);
  }
  തിരികെ uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_openRules(  സ്ഥിരം char16_t     *rules,
                       int32_t       rulesLength,
                 സ്ഥിരം char16_t     *text,
                       int32_t       textLength,
                       UParseError  *parseErr,
                       UErrorCode   *status)  {

    എങ്കിൽ (status == nullptr || U_FAILURE(*status)){
        തിരികെ nullptr;
    }

    BreakIterator *result = nullptr;
    UnicodeString ruleString(rules, rulesLength);
    result = RBBIRuleBuilder::createRuleBasedBreakIterator(ruleString, parseErr, *status);
    എങ്കിൽ(U_FAILURE(*status)) {
        തിരികെ nullptr;
    }

    UBreakIterator *uBI = (UBreakIterator *)result;
    എങ്കിൽ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    തിരികെ uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ദ്വിമാന_Rules(സ്ഥിരം uint8_t *ദ്വിമാന_Rules, int32_t rulesLength,
                     സ്ഥിരം char16_t *  text, int32_t textLength,
                     UErrorCode *   status)
{
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (rulesLength < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    LocalPointer<RuleBasedBreakIterator> lpRBBI(പുതിയ RuleBasedBreakIterator(ദ്വിമാന_Rules, rulesLength, *status), *status);
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    എങ്കിൽ (text != nullptr) {
        ubrk_setText(uBI, text, textLength, status);
    }
    തിരികെ uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          സ്ഥിരം UBreakIterator *bi,
          ശൂന്യം * /*stackBuffer*/,
          int32_t *pBufferSize,
          UErrorCode *status)
{
    എങ്കിൽ (status == nullptr || U_FAILURE(*status)){
        തിരികെ nullptr;
    }
    എങ്കിൽ (bi == nullptr) {
       *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    എങ്കിൽ (pBufferSize != nullptr) {
        int32_t നിവേശം_Size = *pBufferSize;
        *pBufferSize = 1;
        എങ്കിൽ (നിവേശം_Size == 0) {
            തിരികെ nullptr;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    എങ്കിൽ (newBI == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
    } അല്ലെങ്കിൽ എങ്കിൽ (pBufferSize != nullptr) {
        *status = U_SAFECLONE_ALLOCATED_WARNING;
    }
    തിരികെ (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(സ്ഥിരം UBreakIterator *bi, UErrorCode *status) {
    തിരികെ ubrk_safeClone(bi, nullptr, nullptr, status);
}


U_CAPI ശൂന്യം U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    delete (BreakIterator *)bi;
}

U_CAPI ശൂന്യം U_EXPORT2
ubrk_setText(UBreakIterator* bi,
             സ്ഥിരം char16_t*    text,
             int32_t         textLength,
             UErrorCode*     status)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, textLength, status);
    ((BreakIterator*)bi)->setText(&ut, *status);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI ശൂന്യം U_EXPORT2
ubrk_setUText(UBreakIterator *bi,
             UText          *text,
             UErrorCode     *status)
{
  ((BreakIterator*)bi)->setText(text, *status);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(സ്ഥിരം UBreakIterator *bi)
{

  തിരികെ ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_next(UBreakIterator *bi)
{

  തിരികെ ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  തിരികെ ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  തിരികെ ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_last(UBreakIterator *bi)
{

  തിരികെ ((BreakIterator*)bi)->last();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t offset)
{

  തിരികെ ((BreakIterator*)bi)->preceding(offset);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t offset)
{

  തിരികെ ((BreakIterator*)bi)->following(offset);
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ubrk_getAvailable(int32_t index)
{

  തിരികെ uloc_getAvailable(index);
}

U_CAPI int32_t U_EXPORT2
ubrk_എണ്ണം_Available()
{

  തിരികെ uloc_എണ്ണം_Available();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t offset)
{
    തിരികെ ((BreakIterator*)bi)->isBoundary(offset);
}


U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatus(UBreakIterator *bi)
{
    തിരികെ ((BreakIterator*)bi)->getRuleStatus();
}

U_CAPI  int32_t U_EXPORT2
ubrk_getRuleStatusVec(UBreakIterator *bi, int32_t *fillInVec, int32_t capacity, UErrorCode *status)
{
    തിരികെ ((BreakIterator*)bi)->getRuleStatusVec(fillInVec, capacity, *status);
}


U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ubrk_getLocaleByType(സ്ഥിരം UBreakIterator* bi U_LIFETIME_BOUND,
                     ULocDataLocaleType type,
                     UErrorCode* status)
{
    എങ്കിൽ (bi == nullptr) {
        എങ്കിൽ (U_SUCCESS(*status)) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        }
        തിരികെ nullptr;
    }
    തിരികെ ((BreakIterator*)bi)->getLocaleID(type, *status);
}


U_CAPI ശൂന്യം U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UErrorCode     *status)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_നിവേശം_Text(text, *status);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_ദ്വിമാന_Rules(UBreakIterator *bi,
                    uint8_t *       ദ്വിമാന_Rules, int32_t rulesCapacity,
                    UErrorCode *    status)
{
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ 0;
    }
    എങ്കിൽ ((ദ്വിമാന_Rules == nullptr && rulesCapacity > 0) || rulesCapacity < 0) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    RuleBasedBreakIterator* rbbi;
    എങ്കിൽ ((rbbi = dynamic_cast<RuleBasedBreakIterator*>(reinterpret_cast<BreakIterator*>(bi))) == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    uint32_t rulesLength;
    സ്ഥിരം uint8_t * returnedRules = rbbi->get_ദ്വിമാന_Rules(rulesLength);
    എങ്കിൽ (rulesLength > INT32_MAX) {
        *status = U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }
    എങ്കിൽ (ദ്വിമാന_Rules != nullptr) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        എങ്കിൽ ((int32_t)rulesLength > rulesCapacity) {
            *status = U_BUFFER_OVERFLOW_ERROR;
        } അല്ലെങ്കിൽ {
            uprv_memcpy(ദ്വിമാന_Rules, returnedRules, rulesLength);
        }
    }
    തിരികെ (int32_t)rulesLength;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
