// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

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
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
      ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ,
      int32_t ಪಠ್ಯ_ಉದ್ದಳತೆ,
      UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{

  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

  BreakIterator *ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;

  ಆಯ್ಕೆ(type) {

  ಸ್ಥಿತಿ UBRK_CHARACTER:
    ಫಲಿತಾಂಶ = BreakIterator::createCharacterInstance(Locale(locale), *ಸ್ಥಿತಿ_2);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ UBRK_ಶಬ್ದ:
    ಫಲಿತಾಂಶ = BreakIterator::create_ಶಬ್ದ_Instance(Locale(locale), *ಸ್ಥಿತಿ_2);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ UBRK_ಸಾಲು:
    ಫಲಿತಾಂಶ = BreakIterator::create_ಸಾಲು_Instance(Locale(locale), *ಸ್ಥಿತಿ_2);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ UBRK_SENTENCE:
    ಫಲಿತಾಂಶ = BreakIterator::createSentenceInstance(Locale(locale), *ಸ್ಥಿತಿ_2);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ UBRK_TITLE:
    ಫಲಿತಾಂಶ = BreakIterator::createTitleInstance(Locale(locale), *ಸ್ಥಿತಿ_2);
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
  }

  // check for allocation error
  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }


  UBreakIterator *uBI = (UBreakIterator *)ಫಲಿತಾಂಶ;
  ಆದರೆ (ಪಠ್ಯ != ಶೂನ್ಯಸೂಚಕ) {
      ubrk_ಗಣ_ಪಠ್ಯ(uBI, ಪಠ್ಯ, ಪಠ್ಯ_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
  }
  ಹಿಂತಿರುಗಿ uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ನಿಯಮಗಳು(  ಸ್ಥಿರಾಂಕ char16_t     *ನಿಯಮಗಳು,
                       int32_t       ನಿಯಮಗಳು_ಉದ್ದಳತೆ,
                 ಸ್ಥಿರಾಂಕ char16_t     *ಪಠ್ಯ,
                       int32_t       ಪಠ್ಯ_ಉದ್ದಳತೆ,
                       UParseError  *parseErr,
                       UError_ಸಂಕೇತ   *ಸ್ಥಿತಿ_2)  {

    ಆದರೆ (ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    BreakIterator *ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    UnicodeString ನಿಯಮ_String(ನಿಯಮಗಳು, ನಿಯಮಗಳು_ಉದ್ದಳತೆ);
    ಫಲಿತಾಂಶ = RBBIRuleBuilder::create_ನಿಯಮ_Based_Break_Iterator(ನಿಯಮ_String, parseErr, *ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    UBreakIterator *uBI = (UBreakIterator *)ಫಲಿತಾಂಶ;
    ಆದರೆ (ಪಠ್ಯ != ಶೂನ್ಯಸೂಚಕ) {
        ubrk_ಗಣ_ಪಠ್ಯ(uBI, ಪಠ್ಯ, ಪಠ್ಯ_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ದ್ವಿಮಾನ_ನಿಯಮಗಳು(ಸ್ಥಿರಾಂಕ uint8_t *ದ್ವಿಮಾನ_ನಿಯಮಗಳು, int32_t ನಿಯಮಗಳು_ಉದ್ದಳತೆ,
                     ಸ್ಥಿರಾಂಕ char16_t *  ಪಠ್ಯ, int32_t ಪಠ್ಯ_ಉದ್ದಳತೆ,
                     UError_ಸಂಕೇತ *   ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (ನಿಯಮಗಳು_ಉದ್ದಳತೆ < 0) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    Local_ಸೂಚಕ<ನಿಯಮ_Based_Break_Iterator> lpRBBI(ಹೊಸ ನಿಯಮ_Based_Break_Iterator(ದ್ವಿಮಾನ_ನಿಯಮಗಳು, ನಿಯಮಗಳು_ಉದ್ದಳತೆ, *ಸ್ಥಿತಿ_2), *ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    ಆದರೆ (ಪಠ್ಯ != ಶೂನ್ಯಸೂಚಕ) {
        ubrk_ಗಣ_ಪಠ್ಯ(uBI, ಪಠ್ಯ, ಪಠ್ಯ_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_ಸುರಕ್ಷಿತ_Clone(
          ಸ್ಥಿರಾಂಕ UBreakIterator *bi,
          ಶೂನ್ಯ * /*stackBuffer*/,
          int32_t *p_ಬಫರ್_ಗಾತ್ರ,
          UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (bi == ಶೂನ್ಯಸೂಚಕ) {
       *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (p_ಬಫರ್_ಗಾತ್ರ != ಶೂನ್ಯಸೂಚಕ) {
        int32_t ಒಳಹರಿವು_ಗಾತ್ರ = *p_ಬಫರ್_ಗಾತ್ರ;
        *p_ಬಫರ್_ಗಾತ್ರ = 1;
        ಆದರೆ (ಒಳಹರಿವು_ಗಾತ್ರ == 0) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    ಆದರೆ (newBI == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_ಬಫರ್_ಗಾತ್ರ != ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_SAFECLONE_ALLOCATED_WARNING;
    }
    ಹಿಂತಿರುಗಿ (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ಸ್ಥಿರಾಂಕ UBreakIterator *bi, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ ubrk_ಸುರಕ್ಷಿತ_Clone(bi, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    ಅಳಿಸು (BreakIterator *)bi;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubrk_ಗಣ_ಪಠ್ಯ(UBreakIterator* bi,
             ಸ್ಥಿರಾಂಕ char16_t*    ಪಠ್ಯ,
             int32_t         ಪಠ್ಯ_ಉದ್ದಳತೆ,
             UError_ಸಂಕೇತ*     ಸ್ಥಿತಿ_2)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, ಪಠ್ಯ, ಪಠ್ಯ_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
    ((BreakIterator*)bi)->ಗಣ_ಪಠ್ಯ(&ut, *ಸ್ಥಿತಿ_2);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI ಶೂನ್ಯ U_EXPORT2
ubrk_ಗಣ_UText(UBreakIterator *bi,
             UText          *ಪಠ್ಯ,
             UError_ಸಂಕೇತ     *ಸ್ಥಿತಿ_2)
{
  ((BreakIterator*)bi)->ಗಣ_ಪಠ್ಯ(ಪಠ್ಯ, *ಸ್ಥಿತಿ_2);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ಸ್ಥಿರಾಂಕ UBreakIterator *bi)
{

  ಹಿಂತಿರುಗಿ ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_ಮುಂದಿನ(UBreakIterator *bi)
{

  ಹಿಂತಿರುಗಿ ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_ಹಿಂದಿನ(UBreakIterator *bi)
{

  ಹಿಂತಿರುಗಿ ((BreakIterator*)bi)->ಹಿಂದಿನ();
}

U_CAPI int32_t U_EXPORT2
ubrk_ಮೊದಲ(UBreakIterator *bi)
{

  ಹಿಂತಿರುಗಿ ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_ಕೊನೆಯ(UBreakIterator *bi)
{

  ಹಿಂತಿರುಗಿ ((BreakIterator*)bi)->ಕೊನೆಯ();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t ಸ್ಥಾನಾಂತರ)
{

  ಹಿಂತಿರುಗಿ ((BreakIterator*)bi)->preceding(ಸ್ಥಾನಾಂತರ);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t ಸ್ಥಾನಾಂತರ)
{

  ಹಿಂತಿರುಗಿ ((BreakIterator*)bi)->following(ಸ್ಥಾನಾಂತರ);
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ubrk_get_ಲಭ್ಯ(int32_t ಸೂಚ್ಯಂಕ)
{

  ಹಿಂತಿರುಗಿ uloc_get_ಲಭ್ಯ(ಸೂಚ್ಯಂಕ);
}

U_CAPI int32_t U_EXPORT2
ubrk_ಎಣಿಕೆ_ಲಭ್ಯ()
{

  ಹಿಂತಿರುಗಿ uloc_ಎಣಿಕೆ_ಲಭ್ಯ();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t ಸ್ಥಾನಾಂತರ)
{
    ಹಿಂತಿರುಗಿ ((BreakIterator*)bi)->isBoundary(ಸ್ಥಾನಾಂತರ);
}


U_CAPI  int32_t U_EXPORT2
ubrk_get_ನಿಯಮ_ಸ್ಥಿತಿ(UBreakIterator *bi)
{
    ಹಿಂತಿರುಗಿ ((BreakIterator*)bi)->get_ನಿಯಮ_ಸ್ಥಿತಿ();
}

U_CAPI  int32_t U_EXPORT2
ubrk_get_ನಿಯಮ_ಸ್ಥಿತಿ_Vec(UBreakIterator *bi, int32_t *fill_ಒಳ_Vec, int32_t capacity, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ ((BreakIterator*)bi)->get_ನಿಯಮ_ಸ್ಥಿತಿ_Vec(fill_ಒಳ_Vec, capacity, *ಸ್ಥಿತಿ_2);
}


U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ubrk_getLocaleByType(ಸ್ಥಿರಾಂಕ UBreakIterator* bi U_LIFETIME_BOUND,
                     ULoc_ದತ್ತಾಂಶ_Locale_Type type,
                     UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (bi == ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
            *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        }
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ((BreakIterator*)bi)->getLocaleID(type, *ಸ್ಥಿತಿ_2);
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *ಪಠ್ಯ,
                       UError_ಸಂಕೇತ     *ಸ್ಥಿತಿ_2)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_ಒಳಹರಿವು_ಪಠ್ಯ(ಪಠ್ಯ, *ಸ್ಥಿತಿ_2);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_ದ್ವಿಮಾನ_ನಿಯಮಗಳು(UBreakIterator *bi,
                    uint8_t *       ದ್ವಿಮಾನ_ನಿಯಮಗಳು, int32_t ನಿಯಮಗಳು_ಸಾಮರ್ಥ್ಯ,
                    UError_ಸಂಕೇತ *    ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ ((ದ್ವಿಮಾನ_ನಿಯಮಗಳು == ಶೂನ್ಯಸೂಚಕ && ನಿಯಮಗಳು_ಸಾಮರ್ಥ್ಯ > 0) || ನಿಯಮಗಳು_ಸಾಮರ್ಥ್ಯ < 0) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ನಿಯಮ_Based_Break_Iterator* rbbi;
    ಆದರೆ ((rbbi = dynamic_cast<ನಿಯಮ_Based_Break_Iterator*>(reinterpret_cast<BreakIterator*>(bi))) == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    uint32_t ನಿಯಮಗಳು_ಉದ್ದಳತೆ;
    ಸ್ಥಿರಾಂಕ uint8_t * returned_ನಿಯಮಗಳು = rbbi->get_ದ್ವಿಮಾನ_ನಿಯಮಗಳು(ನಿಯಮಗಳು_ಉದ್ದಳತೆ);
    ಆದರೆ (ನಿಯಮಗಳು_ಉದ್ದಳತೆ > INT32_MAX) {
        *ಸ್ಥಿತಿ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (ದ್ವಿಮಾನ_ನಿಯಮಗಳು != ಶೂನ್ಯಸೂಚಕ) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        ಆದರೆ ((int32_t)ನಿಯಮಗಳು_ಉದ್ದಳತೆ > ನಿಯಮಗಳು_ಸಾಮರ್ಥ್ಯ) {
            *ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            uprv_memcpy(ದ್ವಿಮಾನ_ನಿಯಮಗಳು, returned_ನಿಯಮಗಳು, ನಿಯಮಗಳು_ಉದ್ದಳತೆ);
        }
    }
    ಹಿಂತಿರುಗಿ (int32_t)ನಿಯಮಗಳು_ಉದ್ದಳತೆ;
}


#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */
