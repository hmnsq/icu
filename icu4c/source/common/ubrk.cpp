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
      int32_t متن_ڊيگهائي,
      UError_ڪوڊ *حالت)
{

  إذا (U_FAILURE(*حالت)) أرجع خالي_اشارو;

  BreakIterator *نتيجو = خالي_اشارو;

  بدل(type) {

  حالة UBRK_CHARACTER:
    نتيجو = BreakIterator::createCharacterInstance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_لفظ:
    نتيجو = BreakIterator::create_لفظ_Instance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_سٽ:
    نتيجو = BreakIterator::create_سٽ_Instance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_SENTENCE:
    نتيجو = BreakIterator::createSentenceInstance(Locale(locale), *حالت);
    اكسر;

  حالة UBRK_TITLE:
    نتيجو = BreakIterator::createTitleInstance(Locale(locale), *حالت);
    اكسر;

  افتراضي:
    *حالت = U_ILLEGAL_دليل_ERROR;
  }

  // check for allocation error
  إذا (U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }
  إذا (نتيجو == خالي_اشارو) {
    *حالت = U_يادگيري_ورهاست_ERROR;
    أرجع خالي_اشارو;
  }


  UBreakIterator *uBI = (UBreakIterator *)نتيجو;
  إذا (متن != خالي_اشارو) {
      ubrk_سيٽ_متن(uBI, متن, متن_ڊيگهائي, حالت);
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
ubrk_open_قاعدا(  ثابت char16_t     *قاعدا,
                       int32_t       قاعدا_ڊيگهائي,
                 ثابت char16_t     *متن,
                       int32_t       متن_ڊيگهائي,
                       UParseError  *parseErr,
                       UError_ڪوڊ   *حالت)  {

    إذا (حالت == خالي_اشارو || U_FAILURE(*حالت)){
        أرجع خالي_اشارو;
    }

    BreakIterator *نتيجو = خالي_اشارو;
    UnicodeString قاعدو_String(قاعدا, قاعدا_ڊيگهائي);
    نتيجو = RBBIRuleBuilder::create_قاعدو_Based_Break_Iterator(قاعدو_String, parseErr, *حالت);
    إذا(U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }

    UBreakIterator *uBI = (UBreakIterator *)نتيجو;
    إذا (متن != خالي_اشارو) {
        ubrk_سيٽ_متن(uBI, متن, متن_ڊيگهائي, حالت);
    }
    أرجع uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ٻٽو_قاعدا(ثابت uint8_t *ٻٽو_قاعدا, int32_t قاعدا_ڊيگهائي,
                     ثابت char16_t *  متن, int32_t متن_ڊيگهائي,
                     UError_ڪوڊ *   حالت)
{
    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا (قاعدا_ڊيگهائي < 0) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    Local_اشارو<قاعدو_Based_Break_Iterator> lpRBBI(جديد قاعدو_Based_Break_Iterator(ٻٽو_قاعدا, قاعدا_ڊيگهائي, *حالت), *حالت);
    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    إذا (متن != خالي_اشارو) {
        ubrk_سيٽ_متن(uBI, متن, متن_ڊيگهائي, حالت);
    }
    أرجع uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_محفوظ_Clone(
          ثابت UBreakIterator *bi,
          فراغ * /*stackBuffer*/,
          int32_t *p_بفر_سائيز,
          UError_ڪوڊ *حالت)
{
    إذا (حالت == خالي_اشارو || U_FAILURE(*حالت)){
        أرجع خالي_اشارو;
    }
    إذا (bi == خالي_اشارو) {
       *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    إذا (p_بفر_سائيز != خالي_اشارو) {
        int32_t داخلا_سائيز = *p_بفر_سائيز;
        *p_بفر_سائيز = 1;
        إذا (داخلا_سائيز == 0) {
            أرجع خالي_اشارو;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    إذا (newBI == خالي_اشارو) {
        *حالت = U_يادگيري_ورهاست_ERROR;
    } وإلا إذا (p_بفر_سائيز != خالي_اشارو) {
        *حالت = U_SAFECLONE_ALLOCATED_WARNING;
    }
    أرجع (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ثابت UBreakIterator *bi, UError_ڪوڊ *حالت) {
    أرجع ubrk_محفوظ_Clone(bi, خالي_اشارو, خالي_اشارو, حالت);
}


U_CAPI فراغ U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    ميٽيو (BreakIterator *)bi;
}

U_CAPI فراغ U_EXPORT2
ubrk_سيٽ_متن(UBreakIterator* bi,
             ثابت char16_t*    متن,
             int32_t         متن_ڊيگهائي,
             UError_ڪوڊ*     حالت)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, متن, متن_ڊيگهائي, حالت);
    ((BreakIterator*)bi)->سيٽ_متن(&ut, *حالت);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI فراغ U_EXPORT2
ubrk_سيٽ_UText(UBreakIterator *bi,
             UText          *متن,
             UError_ڪوڊ     *حالت)
{
  ((BreakIterator*)bi)->سيٽ_متن(متن, *حالت);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ثابت UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_ايندڙ(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_اڳيون(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->اڳيون();
}

U_CAPI int32_t U_EXPORT2
ubrk_پهريون(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_آخري(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->آخري();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t وٿي)
{

  أرجع ((BreakIterator*)bi)->preceding(وٿي);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t وٿي)
{

  أرجع ((BreakIterator*)bi)->following(وٿي);
}

U_CAPI ثابت حرف* U_EXPORT2
ubrk_get_موجود(int32_t اشاريو)
{

  أرجع uloc_get_موجود(اشاريو);
}

U_CAPI int32_t U_EXPORT2
ubrk_ڳڻپ_موجود()
{

  أرجع uloc_ڳڻپ_موجود();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t وٿي)
{
    أرجع ((BreakIterator*)bi)->isBoundary(وٿي);
}


U_CAPI  int32_t U_EXPORT2
ubrk_get_قاعدو_حالت(UBreakIterator *bi)
{
    أرجع ((BreakIterator*)bi)->get_قاعدو_حالت();
}

U_CAPI  int32_t U_EXPORT2
ubrk_get_قاعدو_حالت_Vec(UBreakIterator *bi, int32_t *fill_اندريون_Vec, int32_t capacity, UError_ڪوڊ *حالت)
{
    أرجع ((BreakIterator*)bi)->get_قاعدو_حالت_Vec(fill_اندريون_Vec, capacity, *حالت);
}


U_CAPI ثابت حرف* U_EXPORT2
ubrk_getLocaleByType(ثابت UBreakIterator* bi U_LIFETIME_BOUND,
                     ULoc_معلومات_Locale_Type type,
                     UError_ڪوڊ* حالت)
{
    إذا (bi == خالي_اشارو) {
        إذا (U_SUCCESS(*حالت)) {
            *حالت = U_ILLEGAL_دليل_ERROR;
        }
        أرجع خالي_اشارو;
    }
    أرجع ((BreakIterator*)bi)->getLocaleID(type, *حالت);
}


U_CAPI فراغ U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *متن,
                       UError_ڪوڊ     *حالت)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_داخلا_متن(متن, *حالت);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_ٻٽو_قاعدا(UBreakIterator *bi,
                    uint8_t *       ٻٽو_قاعدا, int32_t قاعدا_صلاحيت,
                    UError_ڪوڊ *    حالت)
{
    إذا (U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا ((ٻٽو_قاعدا == خالي_اشارو && قاعدا_صلاحيت > 0) || قاعدا_صلاحيت < 0) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    قاعدو_Based_Break_Iterator* rbbi;
    إذا ((rbbi = dynamic_cast<قاعدو_Based_Break_Iterator*>(reinterpret_cast<BreakIterator*>(bi))) == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    uint32_t قاعدا_ڊيگهائي;
    ثابت uint8_t * returned_قاعدا = rbbi->get_ٻٽو_قاعدا(قاعدا_ڊيگهائي);
    إذا (قاعدا_ڊيگهائي > INT32_MAX) {
        *حالت = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    إذا (ٻٽو_قاعدا != خالي_اشارو) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        إذا ((int32_t)قاعدا_ڊيگهائي > قاعدا_صلاحيت) {
            *حالت = U_بفر_حد_کان_مٿي_ERROR;
        } وإلا {
            uprv_memcpy(ٻٽو_قاعدا, returned_قاعدا, قاعدا_ڊيگهائي);
        }
    }
    أرجع (int32_t)قاعدا_ڊيگهائي;
}


#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */
