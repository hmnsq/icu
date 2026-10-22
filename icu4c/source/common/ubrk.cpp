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
      int32_t متن_لمائی,
      UError_کوڈ *حالت)
{

  إذا (U_FAILURE(*حالت)) أرجع خالی_سوچک;

  BreakIterator *نتیجہ = خالی_سوچک;

  بدل(type) {

  حالة UBRK_CHARACTER:
    نتیجہ = BreakIterator::createCharacterInstance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_لفظ:
    نتیجہ = BreakIterator::create_لفظ_Instance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_سطر:
    نتیجہ = BreakIterator::create_سطر_Instance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_SENTENCE:
    نتیجہ = BreakIterator::createSentenceInstance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_TITLE:
    نتیجہ = BreakIterator::createTitleInstance(Locale(locale), *حالت);
    اكسر;

  افتراضي:
    *حالت = U_ILLEGAL_دلیل_ERROR;
  }

  // check for allocation error
  إذا (U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }
  إذا (نتیجہ == خالی_سوچک) {
    *حالت = U_حافظہ_ونڈ_ERROR;
    أرجع خالی_سوچک;
  }


  UBreakIterator *uBI = (UBreakIterator *)نتیجہ;
  إذا (متن != خالی_سوچک) {
      ubrk_سیٹ_متن(uBI, متن, متن_لمائی, حالت);
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
ubrk_open_قاعدے(  ثابت char16_t     *قاعدے,
                       int32_t       قاعدے_لمائی,
                 ثابت char16_t     *متن,
                       int32_t       متن_لمائی,
                       UParseError  *parseErr,
                       UError_کوڈ   *حالت)  {

    إذا (حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع خالی_سوچک;
    }

    BreakIterator *نتیجہ = خالی_سوچک;
    UnicodeString قاعدہ_String(قاعدے, قاعدے_لمائی);
    نتیجہ = RBBIRuleBuilder::create_قاعدہ_Based_Break_Iterator(قاعدہ_String, parseErr, *حالت);
    إذا(U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }

    UBreakIterator *uBI = (UBreakIterator *)نتیجہ;
    إذا (متن != خالی_سوچک) {
        ubrk_سیٹ_متن(uBI, متن, متن_لمائی, حالت);
    }
    أرجع uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ثنائی_قاعدے(ثابت uint8_t *ثنائی_قاعدے, int32_t قاعدے_لمائی,
                     ثابت char16_t *  متن, int32_t متن_لمائی,
                     UError_کوڈ *   حالت)
{
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا (قاعدے_لمائی < 0) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    Local_اشارہ_گر<قاعدہ_Based_Break_Iterator> lpRBBI(جديد قاعدہ_Based_Break_Iterator(ثنائی_قاعدے, قاعدے_لمائی, *حالت), *حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    إذا (متن != خالی_سوچک) {
        ubrk_سیٹ_متن(uBI, متن, متن_لمائی, حالت);
    }
    أرجع uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_محفوظ_Clone(
          ثابت UBreakIterator *bi,
          فراغ * /*stackBuffer*/,
          int32_t *p_بفر_ناپ,
          UError_کوڈ *حالت)
{
    إذا (حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع خالی_سوچک;
    }
    إذا (bi == خالی_سوچک) {
       *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    إذا (p_بفر_ناپ != خالی_سوچک) {
        int32_t داخلہ_ناپ = *p_بفر_ناپ;
        *p_بفر_ناپ = 1;
        إذا (داخلہ_ناپ == 0) {
            أرجع خالی_سوچک;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    إذا (newBI == خالی_سوچک) {
        *حالت = U_حافظہ_ونڈ_ERROR;
    } وإلا إذا (p_بفر_ناپ != خالی_سوچک) {
        *حالت = U_SAFECLONE_ALLOCATED_WARNING;
    }
    أرجع (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ثابت UBreakIterator *bi, UError_کوڈ *حالت) {
    أرجع ubrk_محفوظ_Clone(bi, خالی_سوچک, خالی_سوچک, حالت);
}


U_CAPI فراغ U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    مٹاؤ (BreakIterator *)bi;
}

U_CAPI فراغ U_EXPORT2
ubrk_سیٹ_متن(UBreakIterator* bi,
             ثابت char16_t*    متن,
             int32_t         متن_لمائی,
             UError_کوڈ*     حالت)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, متن, متن_لمائی, حالت);
    ((BreakIterator*)bi)->سیٹ_متن(&ut, *حالت);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI فراغ U_EXPORT2
ubrk_سیٹ_UText(UBreakIterator *bi,
             UText          *متن,
             UError_کوڈ     *حالت)
{
  ((BreakIterator*)bi)->سیٹ_متن(متن, *حالت);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ثابت UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_اگلا(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_پچھلا(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->پچھلا();
}

U_CAPI int32_t U_EXPORT2
ubrk_پہلا(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_آخری(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->آخری();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t وتھ)
{

  أرجع ((BreakIterator*)bi)->preceding(وتھ);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t وتھ)
{

  أرجع ((BreakIterator*)bi)->following(وتھ);
}

U_CAPI ثابت حرف* U_EXPORT2
ubrk_get_دستیاب(int32_t اشاریہ)
{

  أرجع uloc_get_دستیاب(اشاریہ);
}

U_CAPI int32_t U_EXPORT2
ubrk_گنتی_دستیاب()
{

  أرجع uloc_گنتی_دستیاب();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t وتھ)
{
    أرجع ((BreakIterator*)bi)->isBoundary(وتھ);
}


U_CAPI  int32_t U_EXPORT2
ubrk_get_قاعدہ_حالت(UBreakIterator *bi)
{
    أرجع ((BreakIterator*)bi)->get_قاعدہ_حالت();
}

U_CAPI  int32_t U_EXPORT2
ubrk_get_قاعدہ_حالت_Vec(UBreakIterator *bi, int32_t *fill_وچ_Vec, int32_t capacity, UError_کوڈ *حالت)
{
    أرجع ((BreakIterator*)bi)->get_قاعدہ_حالت_Vec(fill_وچ_Vec, capacity, *حالت);
}


U_CAPI ثابت حرف* U_EXPORT2
ubrk_getLocaleByType(ثابت UBreakIterator* bi U_LIFETIME_BOUND,
                     ULoc_انکڑے_Locale_Type type,
                     UError_کوڈ* حالت)
{
    إذا (bi == خالی_سوچک) {
        إذا (U_SUCCESS(*حالت)) {
            *حالت = U_ILLEGAL_دلیل_ERROR;
        }
        أرجع خالی_سوچک;
    }
    أرجع ((BreakIterator*)bi)->getLocaleID(type, *حالت);
}


U_CAPI فراغ U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *متن,
                       UError_کوڈ     *حالت)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_داخلہ_متن(متن, *حالت);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_ثنائی_قاعدے(UBreakIterator *bi,
                    uint8_t *       ثنائی_قاعدے, int32_t قاعدے_سمرتھا,
                    UError_کوڈ *    حالت)
{
    إذا (U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا ((ثنائی_قاعدے == خالی_سوچک && قاعدے_سمرتھا > 0) || قاعدے_سمرتھا < 0) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    قاعدہ_Based_Break_Iterator* rbbi;
    إذا ((rbbi = dynamic_cast<قاعدہ_Based_Break_Iterator*>(reinterpret_cast<BreakIterator*>(bi))) == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    uint32_t قاعدے_لمائی;
    ثابت uint8_t * returned_قاعدے = rbbi->get_ثنائی_قاعدے(قاعدے_لمائی);
    إذا (قاعدے_لمائی > INT32_MAX) {
        *حالت = U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    إذا (ثنائی_قاعدے != خالی_سوچک) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        إذا ((int32_t)قاعدے_لمائی > قاعدے_سمرتھا) {
            *حالت = U_بفر_حد_توں_ودھ_ERROR;
        } وإلا {
            uprv_memcpy(ثنائی_قاعدے, returned_قاعدے, قاعدے_لمائی);
        }
    }
    أرجع (int32_t)قاعدے_لمائی;
}


#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */
