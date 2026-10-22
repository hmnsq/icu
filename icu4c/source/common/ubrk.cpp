// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/ubrk.h"

#ضمن "unicode/brkiter.h"
#ضمن "unicode/uloc.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/uchriter.h"
#ضمن "unicode/rbbi.h"
#ضمن "rbbirb.h"
#ضمن "uassert.h"
#ضمن "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      ثابت حرف *locale,
      ثابت char16_t *متن,
      int32_t متن_اوږدتیا,
      UError_کوډ *حالت)
{

  إذا (U_FAILURE(*حالت)) أرجع تش_اشاره;

  BreakIterator *پایله = تش_اشاره;

  بدل(type) {

  حالة UBRK_CHARACTER:
    پایله = BreakIterator::createCharacterInstance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_کلمه:
    پایله = BreakIterator::create_کلمه_Instance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_کرښه:
    پایله = BreakIterator::create_کرښه_Instance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_SENTENCE:
    پایله = BreakIterator::createSentenceInstance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_TITLE:
    پایله = BreakIterator::createTitleInstance(Locale(locale), *حالت);
    اكسر;

  افتراضي:
    *حالت = U_ILLEGAL_دلیل_ERROR;
  }

  // check for allocation error
  إذا (U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }
  إذا (پایله == تش_اشاره) {
    *حالت = U_حافظه_ویش_ERROR;
    أرجع تش_اشاره;
  }


  UBreakIterator *uBI = (UBreakIterator *)پایله;
  إذا (متن != تش_اشاره) {
      ubrk_سېټ_متن(uBI, متن, متن_اوږدتیا, حالت);
  }
  أرجع uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_قواعد(  ثابت char16_t     *قواعد,
                       int32_t       قواعد_اوږدتیا,
                 ثابت char16_t     *متن,
                       int32_t       متن_اوږدتیا,
                       UParseError  *parseErr,
                       UError_کوډ   *حالت)  {

    إذا (حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع تش_اشاره;
    }

    BreakIterator *پایله = تش_اشاره;
    UnicodeString قاعده_String(قواعد, قواعد_اوږدتیا);
    پایله = RBBIRuleBuilder::create_قاعده_Based_Break_Iterator(قاعده_String, parseErr, *حالت);
    إذا(U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }

    UBreakIterator *uBI = (UBreakIterator *)پایله;
    إذا (متن != تش_اشاره) {
        ubrk_سېټ_متن(uBI, متن, متن_اوږدتیا, حالت);
    }
    أرجع uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_دوګونی_قواعد(ثابت uint8_t *دوګونی_قواعد, int32_t قواعد_اوږدتیا,
                     ثابت char16_t *  متن, int32_t متن_اوږدتیا,
                     UError_کوډ *   حالت)
{
    إذا (U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا (قواعد_اوږدتیا < 0) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    Local_اشاره_ګر<قاعده_Based_Break_Iterator> lpRBBI(جديد قاعده_Based_Break_Iterator(دوګونی_قواعد, قواعد_اوږدتیا, *حالت), *حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    إذا (متن != تش_اشاره) {
        ubrk_سېټ_متن(uBI, متن, متن_اوږدتیا, حالت);
    }
    أرجع uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_خوندي_Clone(
          ثابت UBreakIterator *bi,
          فراغ * /*stackBuffer*/,
          int32_t *p_بفر_اندازه,
          UError_کوډ *حالت)
{
    إذا (حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع تش_اشاره;
    }
    إذا (bi == تش_اشاره) {
       *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    إذا (p_بفر_اندازه != تش_اشاره) {
        int32_t ننوت_اندازه = *p_بفر_اندازه;
        *p_بفر_اندازه = 1;
        إذا (ننوت_اندازه == 0) {
            أرجع تش_اشاره;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    إذا (newBI == تش_اشاره) {
        *حالت = U_حافظه_ویش_ERROR;
    } وإلا إذا (p_بفر_اندازه != تش_اشاره) {
        *حالت = U_SAFECLONE_ALLOCATED_WARNING;
    }
    أرجع (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ثابت UBreakIterator *bi, UError_کوډ *حالت) {
    أرجع ubrk_خوندي_Clone(bi, تش_اشاره, تش_اشاره, حالت);
}


U_CAPI فراغ U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    ړنګول (BreakIterator *)bi;
}

U_CAPI فراغ U_EXPORT2
ubrk_سېټ_متن(UBreakIterator* bi,
             ثابت char16_t*    متن,
             int32_t         متن_اوږدتیا,
             UError_کوډ*     حالت)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, متن, متن_اوږدتیا, حالت);
    ((BreakIterator*)bi)->سېټ_متن(&ut, *حالت);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI فراغ U_EXPORT2
ubrk_سېټ_UText(UBreakIterator *bi,
             UText          *متن,
             UError_کوډ     *حالت)
{
  ((BreakIterator*)bi)->سېټ_متن(متن, *حالت);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ثابت UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_بل(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_پخوانی(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->پخوانی();
}

U_CAPI int32_t U_EXPORT2
ubrk_لومړی(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_وروستی(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->وروستی();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t دننه_واټن)
{

  أرجع ((BreakIterator*)bi)->preceding(دننه_واټن);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t دننه_واټن)
{

  أرجع ((BreakIterator*)bi)->following(دننه_واټن);
}

U_CAPI ثابت حرف* U_EXPORT2
ubrk_get_شته(int32_t شاخص)
{

  أرجع uloc_get_شته(شاخص);
}

U_CAPI int32_t U_EXPORT2
ubrk_شمېر_شته()
{

  أرجع uloc_شمېر_شته();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t دننه_واټن)
{
    أرجع ((BreakIterator*)bi)->isBoundary(دننه_واټن);
}


U_CAPI  int32_t U_EXPORT2
ubrk_get_قاعده_حالت(UBreakIterator *bi)
{
    أرجع ((BreakIterator*)bi)->get_قاعده_حالت();
}

U_CAPI  int32_t U_EXPORT2
ubrk_get_قاعده_حالت_Vec(UBreakIterator *bi, int32_t *fill_دننه_Vec, int32_t capacity, UError_کوډ *حالت)
{
    أرجع ((BreakIterator*)bi)->get_قاعده_حالت_Vec(fill_دننه_Vec, capacity, *حالت);
}


U_CAPI ثابت حرف* U_EXPORT2
ubrk_getLocaleByType(ثابت UBreakIterator* bi U_LIFETIME_BOUND,
                     ULoc_معلومات_Locale_Type type,
                     UError_کوډ* حالت)
{
    إذا (bi == تش_اشاره) {
        إذا (U_SUCCESS(*حالت)) {
            *حالت = U_ILLEGAL_دلیل_ERROR;
        }
        أرجع تش_اشاره;
    }
    أرجع ((BreakIterator*)bi)->getLocaleID(type, *حالت);
}


U_CAPI فراغ U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *متن,
                       UError_کوډ     *حالت)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_ننوت_متن(متن, *حالت);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_دوګونی_قواعد(UBreakIterator *bi,
                    uint8_t *       دوګونی_قواعد, int32_t قواعد_ظرفیت,
                    UError_کوډ *    حالت)
{
    إذا (U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا ((دوګونی_قواعد == تش_اشاره && قواعد_ظرفیت > 0) || قواعد_ظرفیت < 0) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    قاعده_Based_Break_Iterator* rbbi;
    إذا ((rbbi = dynamic_cast<قاعده_Based_Break_Iterator*>(reinterpret_cast<BreakIterator*>(bi))) == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    uint32_t قواعد_اوږدتیا;
    ثابت uint8_t * returned_قواعد = rbbi->get_دوګونی_قواعد(قواعد_اوږدتیا);
    إذا (قواعد_اوږدتیا > INT32_MAX) {
        *حالت = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    إذا (دوګونی_قواعد != تش_اشاره) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        إذا ((int32_t)قواعد_اوږدتیا > قواعد_ظرفیت) {
            *حالت = U_بفر_ډکېدنه_ERROR;
        } وإلا {
            uprv_memcpy(دوګونی_قواعد, returned_قواعد, قواعد_اوږدتیا);
        }
    }
    أرجع (int32_t)قواعد_اوږدتیا;
}


#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */
