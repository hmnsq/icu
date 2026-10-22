// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/ubrk.h"

#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/uchriter.h"
#అన్తర్భావ "unicode/rbbi.h"
#అన్తర్భావ "rbbirb.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      స్థిరరాశి అక్షరం *locale,
      స్థిరరాశి char16_t *పాఠ్యం,
      int32_t పాఠ్యం_నిడివి,
      UError_సంకేతం *స్థితి_2)
{

  ఒకవేళ (U_FAILURE(*స్థితి_2)) తిరిగి శూన్యసూచిక;

  BreakIterator *ఫలితం = శూన్యసూచిక;

  ఎంపిక(type) {

  స్థితి UBRK_CHARACTER:
    ఫలితం = BreakIterator::createCharacterInstance(Locale(locale), *స్థితి_2);
    ఆపు;

  స్థితి UBRK_మాట:
    ఫలితం = BreakIterator::create_మాట_Instance(Locale(locale), *స్థితి_2);
    ఆపు;

  స్థితి UBRK_పంక్తి:
    ఫలితం = BreakIterator::create_పంక్తి_Instance(Locale(locale), *స్థితి_2);
    ఆపు;

  స్థితి UBRK_SENTENCE:
    ఫలితం = BreakIterator::createSentenceInstance(Locale(locale), *స్థితి_2);
    ఆపు;

  స్థితి UBRK_TITLE:
    ఫలితం = BreakIterator::createTitleInstance(Locale(locale), *స్థితి_2);
    ఆపు;

  అప్రమేయ:
    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
  }

  // check for allocation error
  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }
  ఒకవేళ (ఫలితం == శూన్యసూచిక) {
    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి శూన్యసూచిక;
  }


  UBreakIterator *uBI = (UBreakIterator *)ఫలితం;
  ఒకవేళ (పాఠ్యం != శూన్యసూచిక) {
      ubrk_సమితి_పాఠ్యం(uBI, పాఠ్యం, పాఠ్యం_నిడివి, స్థితి_2);
  }
  తిరిగి uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_నియమాలు(  స్థిరరాశి char16_t     *నియమాలు,
                       int32_t       నియమాలు_నిడివి,
                 స్థిరరాశి char16_t     *పాఠ్యం,
                       int32_t       పాఠ్యం_నిడివి,
                       UParseError  *parseErr,
                       UError_సంకేతం   *స్థితి_2)  {

    ఒకవేళ (స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి శూన్యసూచిక;
    }

    BreakIterator *ఫలితం = శూన్యసూచిక;
    UnicodeString నియమం_String(నియమాలు, నియమాలు_నిడివి);
    ఫలితం = RBBIRuleBuilder::create_నియమం_Based_Break_Iterator(నియమం_String, parseErr, *స్థితి_2);
    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    UBreakIterator *uBI = (UBreakIterator *)ఫలితం;
    ఒకవేళ (పాఠ్యం != శూన్యసూచిక) {
        ubrk_సమితి_పాఠ్యం(uBI, పాఠ్యం, పాఠ్యం_నిడివి, స్థితి_2);
    }
    తిరిగి uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_ద్విమాన_నియమాలు(స్థిరరాశి uint8_t *ద్విమాన_నియమాలు, int32_t నియమాలు_నిడివి,
                     స్థిరరాశి char16_t *  పాఠ్యం, int32_t పాఠ్యం_నిడివి,
                     UError_సంకేతం *   స్థితి_2)
{
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (నియమాలు_నిడివి < 0) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    Local_సూచిక<నియమం_Based_Break_Iterator> lpRBBI(కొత్త నియమం_Based_Break_Iterator(ద్విమాన_నియమాలు, నియమాలు_నిడివి, *స్థితి_2), *స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    ఒకవేళ (పాఠ్యం != శూన్యసూచిక) {
        ubrk_సమితి_పాఠ్యం(uBI, పాఠ్యం, పాఠ్యం_నిడివి, స్థితి_2);
    }
    తిరిగి uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_సురక్షిత_Clone(
          స్థిరరాశి UBreakIterator *bi,
          శూన్య * /*stackBuffer*/,
          int32_t *p_బఫరు_కొలత,
          UError_సంకేతం *స్థితి_2)
{
    ఒకవేళ (స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (bi == శూన్యసూచిక) {
       *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (p_బఫరు_కొలత != శూన్యసూచిక) {
        int32_t ఆదానం_కొలత = *p_బఫరు_కొలత;
        *p_బఫరు_కొలత = 1;
        ఒకవేళ (ఆదానం_కొలత == 0) {
            తిరిగి శూన్యసూచిక;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    ఒకవేళ (newBI == శూన్యసూచిక) {
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    } లేకపోతే ఒకవేళ (p_బఫరు_కొలత != శూన్యసూచిక) {
        *స్థితి_2 = U_SAFECLONE_ALLOCATED_WARNING;
    }
    తిరిగి (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(స్థిరరాశి UBreakIterator *bi, UError_సంకేతం *స్థితి_2) {
    తిరిగి ubrk_సురక్షిత_Clone(bi, శూన్యసూచిక, శూన్యసూచిక, స్థితి_2);
}


U_CAPI శూన్య U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    తొలగించు (BreakIterator *)bi;
}

U_CAPI శూన్య U_EXPORT2
ubrk_సమితి_పాఠ్యం(UBreakIterator* bi,
             స్థిరరాశి char16_t*    పాఠ్యం,
             int32_t         పాఠ్యం_నిడివి,
             UError_సంకేతం*     స్థితి_2)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, పాఠ్యం, పాఠ్యం_నిడివి, స్థితి_2);
    ((BreakIterator*)bi)->సమితి_పాఠ్యం(&ut, *స్థితి_2);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI శూన్య U_EXPORT2
ubrk_సమితి_UText(UBreakIterator *bi,
             UText          *పాఠ్యం,
             UError_సంకేతం     *స్థితి_2)
{
  ((BreakIterator*)bi)->సమితి_పాఠ్యం(పాఠ్యం, *స్థితి_2);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(స్థిరరాశి UBreakIterator *bi)
{

  తిరిగి ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_తదుపరి(UBreakIterator *bi)
{

  తిరిగి ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_మునుపటి(UBreakIterator *bi)
{

  తిరిగి ((BreakIterator*)bi)->మునుపటి();
}

U_CAPI int32_t U_EXPORT2
ubrk_మొదటి(UBreakIterator *bi)
{

  తిరిగి ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_చివరి(UBreakIterator *bi)
{

  తిరిగి ((BreakIterator*)bi)->చివరి();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t స్థానభ్రంశం)
{

  తిరిగి ((BreakIterator*)bi)->preceding(స్థానభ్రంశం);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t స్థానభ్రంశం)
{

  తిరిగి ((BreakIterator*)bi)->following(స్థానభ్రంశం);
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ubrk_get_అందుబాటు(int32_t సూచీ)
{

  తిరిగి uloc_get_అందుబాటు(సూచీ);
}

U_CAPI int32_t U_EXPORT2
ubrk_లెక్క_అందుబాటు()
{

  తిరిగి uloc_లెక్క_అందుబాటు();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t స్థానభ్రంశం)
{
    తిరిగి ((BreakIterator*)bi)->isBoundary(స్థానభ్రంశం);
}


U_CAPI  int32_t U_EXPORT2
ubrk_get_నియమం_స్థితి(UBreakIterator *bi)
{
    తిరిగి ((BreakIterator*)bi)->get_నియమం_స్థితి();
}

U_CAPI  int32_t U_EXPORT2
ubrk_get_నియమం_స్థితి_Vec(UBreakIterator *bi, int32_t *fill_లోపల_Vec, int32_t capacity, UError_సంకేతం *స్థితి_2)
{
    తిరిగి ((BreakIterator*)bi)->get_నియమం_స్థితి_Vec(fill_లోపల_Vec, capacity, *స్థితి_2);
}


U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ubrk_getLocaleByType(స్థిరరాశి UBreakIterator* bi U_LIFETIME_BOUND,
                     ULoc_దత్తాంశం_Locale_Type type,
                     UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ (bi == శూన్యసూచిక) {
        ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
            *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        }
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి ((BreakIterator*)bi)->getLocaleID(type, *స్థితి_2);
}


U_CAPI శూన్య U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *పాఠ్యం,
                       UError_సంకేతం     *స్థితి_2)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_ఆదానం_పాఠ్యం(పాఠ్యం, *స్థితి_2);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_ద్విమాన_నియమాలు(UBreakIterator *bi,
                    uint8_t *       ద్విమాన_నియమాలు, int32_t నియమాలు_సామర్థ్యం,
                    UError_సంకేతం *    స్థితి_2)
{
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి 0;
    }
    ఒకవేళ ((ద్విమాన_నియమాలు == శూన్యసూచిక && నియమాలు_సామర్థ్యం > 0) || నియమాలు_సామర్థ్యం < 0) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    నియమం_Based_Break_Iterator* rbbi;
    ఒకవేళ ((rbbi = dynamic_cast<నియమం_Based_Break_Iterator*>(reinterpret_cast<BreakIterator*>(bi))) == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    uint32_t నియమాలు_నిడివి;
    స్థిరరాశి uint8_t * returned_నియమాలు = rbbi->get_ద్విమాన_నియమాలు(నియమాలు_నిడివి);
    ఒకవేళ (నియమాలు_నిడివి > INT32_MAX) {
        *స్థితి_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }
    ఒకవేళ (ద్విమాన_నియమాలు != శూన్యసూచిక) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        ఒకవేళ ((int32_t)నియమాలు_నిడివి > నియమాలు_సామర్థ్యం) {
            *స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
        } లేకపోతే {
            uprv_memcpy(ద్విమాన_నియమాలు, returned_నియమాలు, నియమాలు_నిడివి);
        }
    }
    తిరిగి (int32_t)నియమాలు_నిడివి;
}


#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */
