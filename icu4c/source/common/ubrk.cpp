// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubrk.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchriter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/rbbi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbirb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      ਅਚਲ ਅੱਖਰ *locale,
      ਅਚਲ char16_t *ਪਾਠ,
      int32_t ਪਾਠ_ਲੰਮਾਈ,
      UError_ਕੋਡ *ਸਥਿਤੀ)
{

  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

  BreakIterator *ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;

  ਚੋਣ(type) {

  ਹਾਲਤ UBRK_CHARACTER:
    ਨਤੀਜਾ = BreakIterator::createCharacterInstance(Locale(locale), *ਸਥਿਤੀ);
    ਰੋਕੋ;

  ਹਾਲਤ UBRK_ਸ਼ਬਦ:
    ਨਤੀਜਾ = BreakIterator::create_ਸ਼ਬਦ_Instance(Locale(locale), *ਸਥਿਤੀ);
    ਰੋਕੋ;

  ਹਾਲਤ UBRK_ਸਤਰ:
    ਨਤੀਜਾ = BreakIterator::create_ਸਤਰ_Instance(Locale(locale), *ਸਥਿਤੀ);
    ਰੋਕੋ;

  ਹਾਲਤ UBRK_SENTENCE:
    ਨਤੀਜਾ = BreakIterator::createSentenceInstance(Locale(locale), *ਸਥਿਤੀ);
    ਰੋਕੋ;

  ਹਾਲਤ UBRK_TITLE:
    ਨਤੀਜਾ = BreakIterator::createTitleInstance(Locale(locale), *ਸਥਿਤੀ);
    ਰੋਕੋ;

  ਮੂਲ:
    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
  }

  // check for allocation error
  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }


  UBreakIterator *uBI = (UBreakIterator *)ਨਤੀਜਾ;
  ਜੇ (ਪਾਠ != ਖਾਲੀ_ਸੂਚਕ) {
      ubrk_ਸੈੱਟ_ਪਾਠ(uBI, ਪਾਠ, ਪਾਠ_ਲੰਮਾਈ, ਸਥਿਤੀ);
  }
  ਵਾਪਸ uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ਨਿਯਮਾਂ(  ਅਚਲ char16_t     *ਨਿਯਮਾਂ,
                       int32_t       ਨਿਯਮਾਂ_ਲੰਮਾਈ,
                 ਅਚਲ char16_t     *ਪਾਠ,
                       int32_t       ਪਾਠ_ਲੰਮਾਈ,
                       UParseError  *parseErr,
                       UError_ਕੋਡ   *ਸਥਿਤੀ)  {

    ਜੇ (ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    BreakIterator *ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    UnicodeString ਨਿਯਮ_String(ਨਿਯਮਾਂ, ਨਿਯਮਾਂ_ਲੰਮਾਈ);
    ਨਤੀਜਾ = RBBIRuleBuilder::create_ਨਿਯਮ_Based_Break_Iterator(ਨਿਯਮ_String, parseErr, *ਸਥਿਤੀ);
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    UBreakIterator *uBI = (UBreakIterator *)ਨਤੀਜਾ;
    ਜੇ (ਪਾਠ != ਖਾਲੀ_ਸੂਚਕ) {
        ubrk_ਸੈੱਟ_ਪਾਠ(uBI, ਪਾਠ, ਪਾਠ_ਲੰਮਾਈ, ਸਥਿਤੀ);
    }
    ਵਾਪਸ uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ਦੁਆਧਾਰੀ_ਨਿਯਮਾਂ(ਅਚਲ uint8_t *ਦੁਆਧਾਰੀ_ਨਿਯਮਾਂ, int32_t ਨਿਯਮਾਂ_ਲੰਮਾਈ,
                     ਅਚਲ char16_t *  ਪਾਠ, int32_t ਪਾਠ_ਲੰਮਾਈ,
                     UError_ਕੋਡ *   ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (ਨਿਯਮਾਂ_ਲੰਮਾਈ < 0) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    Local_ਸੂਚਕ<ਨਿਯਮ_Based_Break_Iterator> lpRBBI(ਨਵਾਂ ਨਿਯਮ_Based_Break_Iterator(ਦੁਆਧਾਰੀ_ਨਿਯਮਾਂ, ਨਿਯਮਾਂ_ਲੰਮਾਈ, *ਸਥਿਤੀ), *ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    ਜੇ (ਪਾਠ != ਖਾਲੀ_ਸੂਚਕ) {
        ubrk_ਸੈੱਟ_ਪਾਠ(uBI, ਪਾਠ, ਪਾਠ_ਲੰਮਾਈ, ਸਥਿਤੀ);
    }
    ਵਾਪਸ uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_ਸੁਰੱਖਿਅਤ_Clone(
          ਅਚਲ UBreakIterator *bi,
          ਖਾਲੀ * /*stackBuffer*/,
          int32_t *p_ਬਫਰ_ਮਾਪ,
          UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਜੇ (ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (bi == ਖਾਲੀ_ਸੂਚਕ) {
       *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (p_ਬਫਰ_ਮਾਪ != ਖਾਲੀ_ਸੂਚਕ) {
        int32_t ਨਿਵੇਸ਼_ਮਾਪ = *p_ਬਫਰ_ਮਾਪ;
        *p_ਬਫਰ_ਮਾਪ = 1;
        ਜੇ (ਨਿਵੇਸ਼_ਮਾਪ == 0) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    ਜੇ (newBI == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    } ਨਹੀਂਤਾਂ ਜੇ (p_ਬਫਰ_ਮਾਪ != ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ਵਾਪਸ (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ਅਚਲ UBreakIterator *bi, UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਵਾਪਸ ubrk_ਸੁਰੱਖਿਅਤ_Clone(bi, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
}


U_CAPI ਖਾਲੀ U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    ਮਿਟਾਓ (BreakIterator *)bi;
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubrk_ਸੈੱਟ_ਪਾਠ(UBreakIterator* bi,
             ਅਚਲ char16_t*    ਪਾਠ,
             int32_t         ਪਾਠ_ਲੰਮਾਈ,
             UError_ਕੋਡ*     ਸਥਿਤੀ)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, ਪਾਠ, ਪਾਠ_ਲੰਮਾਈ, ਸਥਿਤੀ);
    ((BreakIterator*)bi)->ਸੈੱਟ_ਪਾਠ(&ut, *ਸਥਿਤੀ);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI ਖਾਲੀ U_EXPORT2
ubrk_ਸੈੱਟ_UText(UBreakIterator *bi,
             UText          *ਪਾਠ,
             UError_ਕੋਡ     *ਸਥਿਤੀ)
{
  ((BreakIterator*)bi)->ਸੈੱਟ_ਪਾਠ(ਪਾਠ, *ਸਥਿਤੀ);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ਅਚਲ UBreakIterator *bi)
{

  ਵਾਪਸ ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_ਅਗਲਾ(UBreakIterator *bi)
{

  ਵਾਪਸ ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_ਪਿਛਲਾ(UBreakIterator *bi)
{

  ਵਾਪਸ ((BreakIterator*)bi)->ਪਿਛਲਾ();
}

U_CAPI int32_t U_EXPORT2
ubrk_ਪਹਿਲਾ(UBreakIterator *bi)
{

  ਵਾਪਸ ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_ਆਖਰੀ(UBreakIterator *bi)
{

  ਵਾਪਸ ((BreakIterator*)bi)->ਆਖਰੀ();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t ਵਿੱਥ)
{

  ਵਾਪਸ ((BreakIterator*)bi)->preceding(ਵਿੱਥ);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t ਵਿੱਥ)
{

  ਵਾਪਸ ((BreakIterator*)bi)->following(ਵਿੱਥ);
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ubrk_get_ਉਪਲਬਧ(int32_t ਸੂਚਕਾਂਕ)
{

  ਵਾਪਸ uloc_get_ਉਪਲਬਧ(ਸੂਚਕਾਂਕ);
}

U_CAPI int32_t U_EXPORT2
ubrk_ਗਿਣਤੀ_ਉਪਲਬਧ()
{

  ਵਾਪਸ uloc_ਗਿਣਤੀ_ਉਪਲਬਧ();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t ਵਿੱਥ)
{
    ਵਾਪਸ ((BreakIterator*)bi)->isBoundary(ਵਿੱਥ);
}


U_CAPI  int32_t U_EXPORT2
ubrk_get_ਨਿਯਮ_ਸਥਿਤੀ(UBreakIterator *bi)
{
    ਵਾਪਸ ((BreakIterator*)bi)->get_ਨਿਯਮ_ਸਥਿਤੀ();
}

U_CAPI  int32_t U_EXPORT2
ubrk_get_ਨਿਯਮ_ਸਥਿਤੀ_Vec(UBreakIterator *bi, int32_t *fill_ਵਿੱਚ_Vec, int32_t capacity, UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਵਾਪਸ ((BreakIterator*)bi)->get_ਨਿਯਮ_ਸਥਿਤੀ_Vec(fill_ਵਿੱਚ_Vec, capacity, *ਸਥਿਤੀ);
}


U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ubrk_getLocaleByType(ਅਚਲ UBreakIterator* bi U_LIFETIME_BOUND,
                     ULoc_ਅੰਕੜੇ_Locale_Type type,
                     UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ (bi == ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
            *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        }
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ((BreakIterator*)bi)->getLocaleID(type, *ਸਥਿਤੀ);
}


U_CAPI ਖਾਲੀ U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *ਪਾਠ,
                       UError_ਕੋਡ     *ਸਥਿਤੀ)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_ਨਿਵੇਸ਼_ਪਾਠ(ਪਾਠ, *ਸਥਿਤੀ);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_ਦੁਆਧਾਰੀ_ਨਿਯਮਾਂ(UBreakIterator *bi,
                    uint8_t *       ਦੁਆਧਾਰੀ_ਨਿਯਮਾਂ, int32_t ਨਿਯਮਾਂ_ਸਮਰੱਥਾ,
                    UError_ਕੋਡ *    ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ ((ਦੁਆਧਾਰੀ_ਨਿਯਮਾਂ == ਖਾਲੀ_ਸੂਚਕ && ਨਿਯਮਾਂ_ਸਮਰੱਥਾ > 0) || ਨਿਯਮਾਂ_ਸਮਰੱਥਾ < 0) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    ਨਿਯਮ_Based_Break_Iterator* rbbi;
    ਜੇ ((rbbi = dynamic_cast<ਨਿਯਮ_Based_Break_Iterator*>(reinterpret_cast<BreakIterator*>(bi))) == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    uint32_t ਨਿਯਮਾਂ_ਲੰਮਾਈ;
    ਅਚਲ uint8_t * returned_ਨਿਯਮਾਂ = rbbi->get_ਦੁਆਧਾਰੀ_ਨਿਯਮਾਂ(ਨਿਯਮਾਂ_ਲੰਮਾਈ);
    ਜੇ (ਨਿਯਮਾਂ_ਲੰਮਾਈ > INT32_MAX) {
        *ਸਥਿਤੀ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }
    ਜੇ (ਦੁਆਧਾਰੀ_ਨਿਯਮਾਂ != ਖਾਲੀ_ਸੂਚਕ) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        ਜੇ ((int32_t)ਨਿਯਮਾਂ_ਲੰਮਾਈ > ਨਿਯਮਾਂ_ਸਮਰੱਥਾ) {
            *ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        } ਨਹੀਂਤਾਂ {
            uprv_memcpy(ਦੁਆਧਾਰੀ_ਨਿਯਮਾਂ, returned_ਨਿਯਮਾਂ, ਨਿਯਮਾਂ_ਲੰਮਾਈ);
        }
    }
    ਵਾਪਸ (int32_t)ਨਿਯਮਾਂ_ਲੰਮਾਈ;
}


#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */
