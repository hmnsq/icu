// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/ubrk.h"

#شامل "unicode/brkiter.h"
#شامل "unicode/uloc.h"
#شامل "unicode/ustring.h"
#شامل "unicode/uchriter.h"
#شامل "unicode/rbbi.h"
#شامل "rbbirb.h"
#شامل "uassert.h"
#شامل "cmemory.h"

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
      int32_t متن_لمبائی,
      UError_رمز *حالت_2)
{

  اگر (U_FAILURE(*حالت_2)) واپس خالی_اشاریہ;

  BreakIterator *نتیجہ = خالی_اشاریہ;

  انتخاب(type) {

  حالت UBRK_CHARACTER:
    نتیجہ = BreakIterator::createCharacterInstance(Locale(locale), *حالت_2);
    توڑو;

  حالت UBRK_لفظ:
    نتیجہ = BreakIterator::create_لفظ_Instance(Locale(locale), *حالت_2);
    توڑو;

  حالت UBRK_سطر:
    نتیجہ = BreakIterator::create_سطر_Instance(Locale(locale), *حالت_2);
    توڑو;

  حالت UBRK_SENTENCE:
    نتیجہ = BreakIterator::createSentenceInstance(Locale(locale), *حالت_2);
    توڑو;

  حالت UBRK_TITLE:
    نتیجہ = BreakIterator::createTitleInstance(Locale(locale), *حالت_2);
    توڑو;

  طےشدہ:
    *حالت_2 = U_ILLEGAL_دلیل_ERROR;
  }

  // check for allocation error
  اگر (U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }
  اگر (نتیجہ == خالی_اشاریہ) {
    *حالت_2 = U_حافظہ_تخصیص_ERROR;
    واپس خالی_اشاریہ;
  }


  UBreakIterator *uBI = (UBreakIterator *)نتیجہ;
  اگر (متن != خالی_اشاریہ) {
      ubrk_سیٹ_متن(uBI, متن, متن_لمبائی, حالت_2);
  }
  واپس uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_قواعد(  ثابت char16_t     *قواعد,
                       int32_t       قواعد_لمبائی,
                 ثابت char16_t     *متن,
                       int32_t       متن_لمبائی,
                       UParseError  *parseErr,
                       UError_رمز   *حالت_2)  {

    اگر (حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس خالی_اشاریہ;
    }

    BreakIterator *نتیجہ = خالی_اشاریہ;
    UnicodeString قاعدہ_String(قواعد, قواعد_لمبائی);
    نتیجہ = RBBIRuleBuilder::create_قاعدہ_Based_Break_Iterator(قاعدہ_String, parseErr, *حالت_2);
    اگر(U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }

    UBreakIterator *uBI = (UBreakIterator *)نتیجہ;
    اگر (متن != خالی_اشاریہ) {
        ubrk_سیٹ_متن(uBI, متن, متن_لمبائی, حالت_2);
    }
    واپس uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ثنائی_قواعد(ثابت uint8_t *ثنائی_قواعد, int32_t قواعد_لمبائی,
                     ثابت char16_t *  متن, int32_t متن_لمبائی,
                     UError_رمز *   حالت_2)
{
    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر (قواعد_لمبائی < 0) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    Local_اشارہ_گر<قاعدہ_Based_Break_Iterator> lpRBBI(نیا قاعدہ_Based_Break_Iterator(ثنائی_قواعد, قواعد_لمبائی, *حالت_2), *حالت_2);
    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    اگر (متن != خالی_اشاریہ) {
        ubrk_سیٹ_متن(uBI, متن, متن_لمبائی, حالت_2);
    }
    واپس uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_محفوظ_Clone(
          ثابت UBreakIterator *bi,
          خالی * /*stackBuffer*/,
          int32_t *p_بفر_سائز,
          UError_رمز *حالت_2)
{
    اگر (حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس خالی_اشاریہ;
    }
    اگر (bi == خالی_اشاریہ) {
       *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    اگر (p_بفر_سائز != خالی_اشاریہ) {
        int32_t داخلہ_سائز = *p_بفر_سائز;
        *p_بفر_سائز = 1;
        اگر (داخلہ_سائز == 0) {
            واپس خالی_اشاریہ;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    اگر (newBI == خالی_اشاریہ) {
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
    } ورنہ اگر (p_بفر_سائز != خالی_اشاریہ) {
        *حالت_2 = U_SAFECLONE_ALLOCATED_WARNING;
    }
    واپس (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ثابت UBreakIterator *bi, UError_رمز *حالت_2) {
    واپس ubrk_محفوظ_Clone(bi, خالی_اشاریہ, خالی_اشاریہ, حالت_2);
}


U_CAPI خالی U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    حذف (BreakIterator *)bi;
}

U_CAPI خالی U_EXPORT2
ubrk_سیٹ_متن(UBreakIterator* bi,
             ثابت char16_t*    متن,
             int32_t         متن_لمبائی,
             UError_رمز*     حالت_2)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, متن, متن_لمبائی, حالت_2);
    ((BreakIterator*)bi)->سیٹ_متن(&ut, *حالت_2);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI خالی U_EXPORT2
ubrk_سیٹ_UText(UBreakIterator *bi,
             UText          *متن,
             UError_رمز     *حالت_2)
{
  ((BreakIterator*)bi)->سیٹ_متن(متن, *حالت_2);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ثابت UBreakIterator *bi)
{

  واپس ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_اگلا(UBreakIterator *bi)
{

  واپس ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_پچھلا(UBreakIterator *bi)
{

  واپس ((BreakIterator*)bi)->پچھلا_2();
}

U_CAPI int32_t U_EXPORT2
ubrk_پہلا(UBreakIterator *bi)
{

  واپس ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_آخری(UBreakIterator *bi)
{

  واپس ((BreakIterator*)bi)->آخری();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t انحراف)
{

  واپس ((BreakIterator*)bi)->preceding(انحراف);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t انحراف)
{

  واپس ((BreakIterator*)bi)->following(انحراف);
}

U_CAPI ثابت حرف* U_EXPORT2
ubrk_get_دستیاب(int32_t اشاریہ)
{

  واپس uloc_get_دستیاب(اشاریہ);
}

U_CAPI int32_t U_EXPORT2
ubrk_گنتی_دستیاب()
{

  واپس uloc_گنتی_دستیاب();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t انحراف)
{
    واپس ((BreakIterator*)bi)->isBoundary(انحراف);
}


U_CAPI  int32_t U_EXPORT2
ubrk_get_قاعدہ_حالت(UBreakIterator *bi)
{
    واپس ((BreakIterator*)bi)->get_قاعدہ_حالت();
}

U_CAPI  int32_t U_EXPORT2
ubrk_get_قاعدہ_حالت_Vec(UBreakIterator *bi, int32_t *fill_اندر_Vec, int32_t capacity, UError_رمز *حالت_2)
{
    واپس ((BreakIterator*)bi)->get_قاعدہ_حالت_Vec(fill_اندر_Vec, capacity, *حالت_2);
}


U_CAPI ثابت حرف* U_EXPORT2
ubrk_getLocaleByType(ثابت UBreakIterator* bi U_LIFETIME_BOUND,
                     ULoc_ڈیٹا_Locale_Type type,
                     UError_رمز* حالت_2)
{
    اگر (bi == خالی_اشاریہ) {
        اگر (U_SUCCESS(*حالت_2)) {
            *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        }
        واپس خالی_اشاریہ;
    }
    واپس ((BreakIterator*)bi)->getLocaleID(type, *حالت_2);
}


U_CAPI خالی U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *متن,
                       UError_رمز     *حالت_2)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_داخلہ_متن(متن, *حالت_2);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_ثنائی_قواعد(UBreakIterator *bi,
                    uint8_t *       ثنائی_قواعد, int32_t قواعد_استعداد,
                    UError_رمز *    حالت_2)
{
    اگر (U_FAILURE(*حالت_2)) {
        واپس 0;
    }
    اگر ((ثنائی_قواعد == خالی_اشاریہ && قواعد_استعداد > 0) || قواعد_استعداد < 0) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    قاعدہ_Based_Break_Iterator* rbbi;
    اگر ((rbbi = dynamic_cast<قاعدہ_Based_Break_Iterator*>(reinterpret_cast<BreakIterator*>(bi))) == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    uint32_t قواعد_لمبائی;
    ثابت uint8_t * returned_قواعد = rbbi->get_ثنائی_قواعد(قواعد_لمبائی);
    اگر (قواعد_لمبائی > INT32_MAX) {
        *حالت_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }
    اگر (ثنائی_قواعد != خالی_اشاریہ) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        اگر ((int32_t)قواعد_لمبائی > قواعد_استعداد) {
            *حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
        } ورنہ {
            uprv_memcpy(ثنائی_قواعد, returned_قواعد, قواعد_لمبائی);
        }
    }
    واپس (int32_t)قواعد_لمبائی;
}


#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */
