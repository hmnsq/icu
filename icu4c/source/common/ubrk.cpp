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
      int32_t متن_درازی,
      UError_کد *وضعیت)
{

  إذا (U_FAILURE(*وضعیت)) أرجع اشاره_تهی;

  BreakIterator *نتیجه = اشاره_تهی;

  بدل(type) {

  حالة UBRK_CHARACTER:
    نتیجه = BreakIterator::createCharacterInstance(Locale(locale), *وضعیت);
    اكسر;

  حالة UBRK_کلمه:
    نتیجه = BreakIterator::create_کلمه_Instance(Locale(locale), *وضعیت);
    اكسر;

  حالة UBRK_سطر:
    نتیجه = BreakIterator::create_سطر_Instance(Locale(locale), *وضعیت);
    اكسر;

  حالة UBRK_SENTENCE:
    نتیجه = BreakIterator::createSentenceInstance(Locale(locale), *وضعیت);
    اكسر;

  حالة UBRK_TITLE:
    نتیجه = BreakIterator::createTitleInstance(Locale(locale), *وضعیت);
    اكسر;

  افتراضي:
    *وضعیت = U_ILLEGAL_آرگومان_ERROR;
  }

  // check for allocation error
  إذا (U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }
  إذا (نتیجه == اشاره_تهی) {
    *وضعیت = U_حافظه_تخصیص_ERROR;
    أرجع اشاره_تهی;
  }


  UBreakIterator *uBI = (UBreakIterator *)نتیجه;
  إذا (متن != اشاره_تهی) {
      ubrk_مجموعه_متن(uBI, متن, متن_درازی, وضعیت);
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
                       int32_t       قواعد_درازی,
                 ثابت char16_t     *متن,
                       int32_t       متن_درازی,
                       UParseError  *parseErr,
                       UError_کد   *وضعیت)  {

    إذا (وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)){
        أرجع اشاره_تهی;
    }

    BreakIterator *نتیجه = اشاره_تهی;
    UnicodeString قاعده_String(قواعد, قواعد_درازی);
    نتیجه = RBBIRuleBuilder::create_قاعده_Based_Break_Iterator(قاعده_String, parseErr, *وضعیت);
    إذا(U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }

    UBreakIterator *uBI = (UBreakIterator *)نتیجه;
    إذا (متن != اشاره_تهی) {
        ubrk_مجموعه_متن(uBI, متن, متن_درازی, وضعیت);
    }
    أرجع uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_دودویی_قواعد(ثابت uint8_t *دودویی_قواعد, int32_t قواعد_درازی,
                     ثابت char16_t *  متن, int32_t متن_درازی,
                     UError_کد *   وضعیت)
{
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا (قواعد_درازی < 0) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    Local_اشاره_گر<قاعده_Based_Break_Iterator> lpRBBI(جديد قاعده_Based_Break_Iterator(دودویی_قواعد, قواعد_درازی, *وضعیت), *وضعیت);
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    إذا (متن != اشاره_تهی) {
        ubrk_مجموعه_متن(uBI, متن, متن_درازی, وضعیت);
    }
    أرجع uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_امن_Clone(
          ثابت UBreakIterator *bi,
          فراغ * /*stackBuffer*/,
          int32_t *p_میانگیر_اندازه,
          UError_کد *وضعیت)
{
    إذا (وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)){
        أرجع اشاره_تهی;
    }
    إذا (bi == اشاره_تهی) {
       *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    إذا (p_میانگیر_اندازه != اشاره_تهی) {
        int32_t درون_داد_اندازه = *p_میانگیر_اندازه;
        *p_میانگیر_اندازه = 1;
        إذا (درون_داد_اندازه == 0) {
            أرجع اشاره_تهی;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    إذا (newBI == اشاره_تهی) {
        *وضعیت = U_حافظه_تخصیص_ERROR;
    } وإلا إذا (p_میانگیر_اندازه != اشاره_تهی) {
        *وضعیت = U_SAFECLONE_ALLOCATED_WARNING;
    }
    أرجع (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ثابت UBreakIterator *bi, UError_کد *وضعیت) {
    أرجع ubrk_امن_Clone(bi, اشاره_تهی, اشاره_تهی, وضعیت);
}


U_CAPI فراغ U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    حذف (BreakIterator *)bi;
}

U_CAPI فراغ U_EXPORT2
ubrk_مجموعه_متن(UBreakIterator* bi,
             ثابت char16_t*    متن,
             int32_t         متن_درازی,
             UError_کد*     وضعیت)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, متن, متن_درازی, وضعیت);
    ((BreakIterator*)bi)->مجموعه_متن(&ut, *وضعیت);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI فراغ U_EXPORT2
ubrk_مجموعه_UText(UBreakIterator *bi,
             UText          *متن,
             UError_کد     *وضعیت)
{
  ((BreakIterator*)bi)->مجموعه_متن(متن, *وضعیت);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ثابت UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_بعدی(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_قبلی(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->قبلی();
}

U_CAPI int32_t U_EXPORT2
ubrk_اولی(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_آخرین(UBreakIterator *bi)
{

  أرجع ((BreakIterator*)bi)->آخرین();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t جابه_جایی)
{

  أرجع ((BreakIterator*)bi)->preceding(جابه_جایی);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t جابه_جایی)
{

  أرجع ((BreakIterator*)bi)->following(جابه_جایی);
}

U_CAPI ثابت حرف* U_EXPORT2
ubrk_get_موجود(int32_t شاخص)
{

  أرجع uloc_get_موجود(شاخص);
}

U_CAPI int32_t U_EXPORT2
ubrk_شمار_موجود()
{

  أرجع uloc_شمار_موجود();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t جابه_جایی)
{
    أرجع ((BreakIterator*)bi)->isBoundary(جابه_جایی);
}


U_CAPI  int32_t U_EXPORT2
ubrk_get_قاعده_وضعیت(UBreakIterator *bi)
{
    أرجع ((BreakIterator*)bi)->get_قاعده_وضعیت();
}

U_CAPI  int32_t U_EXPORT2
ubrk_get_قاعده_وضعیت_Vec(UBreakIterator *bi, int32_t *fill_درون_Vec, int32_t capacity, UError_کد *وضعیت)
{
    أرجع ((BreakIterator*)bi)->get_قاعده_وضعیت_Vec(fill_درون_Vec, capacity, *وضعیت);
}


U_CAPI ثابت حرف* U_EXPORT2
ubrk_getLocaleByType(ثابت UBreakIterator* bi U_LIFETIME_BOUND,
                     ULoc_داده_Locale_Type type,
                     UError_کد* وضعیت)
{
    إذا (bi == اشاره_تهی) {
        إذا (U_SUCCESS(*وضعیت)) {
            *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        }
        أرجع اشاره_تهی;
    }
    أرجع ((BreakIterator*)bi)->getLocaleID(type, *وضعیت);
}


U_CAPI فراغ U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *متن,
                       UError_کد     *وضعیت)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_درون_داد_متن(متن, *وضعیت);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_دودویی_قواعد(UBreakIterator *bi,
                    uint8_t *       دودویی_قواعد, int32_t قواعد_ظرفیت,
                    UError_کد *    وضعیت)
{
    إذا (U_FAILURE(*وضعیت)) {
        أرجع 0;
    }
    إذا ((دودویی_قواعد == اشاره_تهی && قواعد_ظرفیت > 0) || قواعد_ظرفیت < 0) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    قاعده_Based_Break_Iterator* rbbi;
    إذا ((rbbi = dynamic_cast<قاعده_Based_Break_Iterator*>(reinterpret_cast<BreakIterator*>(bi))) == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    uint32_t قواعد_درازی;
    ثابت uint8_t * returned_قواعد = rbbi->get_دودویی_قواعد(قواعد_درازی);
    إذا (قواعد_درازی > INT32_MAX) {
        *وضعیت = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    إذا (دودویی_قواعد != اشاره_تهی) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        إذا ((int32_t)قواعد_درازی > قواعد_ظرفیت) {
            *وضعیت = U_میانگیر_سرریز_ERROR;
        } وإلا {
            uprv_memcpy(دودویی_قواعد, returned_قواعد, قواعد_درازی);
        }
    }
    أرجع (int32_t)قواعد_درازی;
}


#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */
