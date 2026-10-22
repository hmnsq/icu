// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/ubrk.h"

#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uchriter.h"
#अन्तर्भाव "unicode/rbbi.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_USE

//------------------------------------------------------------------------------
//
//    ubrk_open      Create a canned type of break iterator based on type (word, line, etc.)
//                   and locale.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open(UBreakIteratorType type,
      ध्रुव वर्ण *locale,
      ध्रुव char16_t *text,
      int32_t text_दीर्घता,
      UError_संहिता *स्थितिः)
{

  यदि (U_FAILURE(*स्थितिः)) प्रत्यागम शून्यसूचकः;

  BreakIterator *परिणाम = शून्यसूचकः;

  विकल्प(type) {

  पक्ष UBRK_CHARACTER:
    परिणाम = BreakIterator::createCharacterInstance(Locale(locale), *स्थितिः);
    भङ्ग;

  पक्ष UBRK_WORD:
    परिणाम = BreakIterator::createWordInstance(Locale(locale), *स्थितिः);
    भङ्ग;

  पक्ष UBRK_पङ्क्तिः:
    परिणाम = BreakIterator::create_पङ्क्तिः_Instance(Locale(locale), *स्थितिः);
    भङ्ग;

  पक्ष UBRK_SENTENCE:
    परिणाम = BreakIterator::createSentenceInstance(Locale(locale), *स्थितिः);
    भङ्ग;

  पक्ष UBRK_TITLE:
    परिणाम = BreakIterator::createTitleInstance(Locale(locale), *स्थितिः);
    भङ्ग;

  स्वभाव:
    *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
  }

  // check for allocation error
  यदि (U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }
  यदि (परिणाम == शून्यसूचकः) {
    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम शून्यसूचकः;
  }


  UBreakIterator *uBI = (UBreakIterator *)परिणाम;
  यदि (text != शून्यसूचकः) {
      ubrk_समुच्चयः_Text(uBI, text, text_दीर्घता, स्थितिः);
  }
  प्रत्यागम uBI;
}



//------------------------------------------------------------------------------
//
//   ubrk_openRules      open a break iterator from a set of break rules.
//                       Invokes the rule builder.
//
//------------------------------------------------------------------------------
U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_नियमाः(  ध्रुव char16_t     *नियमाः,
                       int32_t       नियमाः_दीर्घता,
                 ध्रुव char16_t     *text,
                       int32_t       text_दीर्घता,
                       UParseError  *parseErr,
                       UError_संहिता   *स्थितिः)  {

    यदि (स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम शून्यसूचकः;
    }

    BreakIterator *परिणाम = शून्यसूचकः;
    UnicodeString नियमः_String(नियमाः, नियमाः_दीर्घता);
    परिणाम = RBBIRuleBuilder::create_नियमः_Based_Break_Iterator(नियमः_String, parseErr, *स्थितिः);
    यदि(U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    UBreakIterator *uBI = (UBreakIterator *)परिणाम;
    यदि (text != शून्यसूचकः) {
        ubrk_समुच्चयः_Text(uBI, text, text_दीर्घता, स्थितिः);
    }
    प्रत्यागम uBI;
}


U_CAPI UBreakIterator* U_EXPORT2
ubrk_open_द्विमान_नियमाः(ध्रुव uint8_t *द्विमान_नियमाः, int32_t नियमाः_दीर्घता,
                     ध्रुव char16_t *  text, int32_t text_दीर्घता,
                     UError_संहिता *   स्थितिः)
{
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (नियमाः_दीर्घता < 0) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    LocalPointer<नियमः_Based_Break_Iterator> lpRBBI(नूतन नियमः_Based_Break_Iterator(द्विमान_नियमाः, नियमाः_दीर्घता, *स्थितिः), *स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    UBreakIterator *uBI = reinterpret_cast<UBreakIterator *>(lpRBBI.orphan());
    यदि (text != शून्यसूचकः) {
        ubrk_समुच्चयः_Text(uBI, text, text_दीर्घता, स्थितिः);
    }
    प्रत्यागम uBI;
}


U_CAPI UBreakIterator * U_EXPORT2
ubrk_safeClone(
          ध्रुव UBreakIterator *bi,
          शून्य * /*stackBuffer*/,
          int32_t *p_बफर_आकार,
          UError_संहिता *स्थितिः)
{
    यदि (स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम शून्यसूचकः;
    }
    यदि (bi == शून्यसूचकः) {
       *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    यदि (p_बफर_आकार != शून्यसूचकः) {
        int32_t निवेशः_आकार = *p_बफर_आकार;
        *p_बफर_आकार = 1;
        यदि (निवेशः_आकार == 0) {
            प्रत्यागम शून्यसूचकः;  // preflighting for deprecated functionality
        }
    }
    BreakIterator *newBI = ((BreakIterator *)bi)->clone();
    यदि (newBI == शून्यसूचकः) {
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    } अन्यथा यदि (p_बफर_आकार != शून्यसूचकः) {
        *स्थितिः = U_SAFECLONE_ALLOCATED_WARNING;
    }
    प्रत्यागम (UBreakIterator *)newBI;
}

U_CAPI UBreakIterator * U_EXPORT2
ubrk_clone(ध्रुव UBreakIterator *bi, UError_संहिता *स्थितिः) {
    प्रत्यागम ubrk_safeClone(bi, शून्यसूचकः, शून्यसूचकः, स्थितिः);
}


U_CAPI शून्य U_EXPORT2
ubrk_close(UBreakIterator *bi)
{
    विलोपय (BreakIterator *)bi;
}

U_CAPI शून्य U_EXPORT2
ubrk_समुच्चयः_Text(UBreakIterator* bi,
             ध्रुव char16_t*    text,
             int32_t         text_दीर्घता,
             UError_संहिता*     स्थितिः)
{
    UText  ut = UTEXT_INITIALIZER;
    utext_openUChars(&ut, text, text_दीर्घता, स्थितिः);
    ((BreakIterator*)bi)->समुच्चयः_Text(&ut, *स्थितिः);
    // A stack allocated UText wrapping a char16_t * string
    //   can be dumped without explicitly closing it.
}



U_CAPI शून्य U_EXPORT2
ubrk_समुच्चयः_UText(UBreakIterator *bi,
             UText          *text,
             UError_संहिता     *स्थितिः)
{
  ((BreakIterator*)bi)->समुच्चयः_Text(text, *स्थितिः);
}





U_CAPI int32_t U_EXPORT2
ubrk_current(ध्रुव UBreakIterator *bi)
{

  प्रत्यागम ((BreakIterator*)bi)->current();
}

U_CAPI int32_t U_EXPORT2
ubrk_अग्र(UBreakIterator *bi)
{

  प्रत्यागम ((BreakIterator*)bi)->next();
}

U_CAPI int32_t U_EXPORT2
ubrk_previous(UBreakIterator *bi)
{

  प्रत्यागम ((BreakIterator*)bi)->previous();
}

U_CAPI int32_t U_EXPORT2
ubrk_first(UBreakIterator *bi)
{

  प्रत्यागम ((BreakIterator*)bi)->first();
}

U_CAPI int32_t U_EXPORT2
ubrk_चरमः(UBreakIterator *bi)
{

  प्रत्यागम ((BreakIterator*)bi)->चरमः();
}

U_CAPI int32_t U_EXPORT2
ubrk_preceding(UBreakIterator *bi,
           int32_t विस्थापनम्)
{

  प्रत्यागम ((BreakIterator*)bi)->preceding(विस्थापनम्);
}

U_CAPI int32_t U_EXPORT2
ubrk_following(UBreakIterator *bi,
           int32_t विस्थापनम्)
{

  प्रत्यागम ((BreakIterator*)bi)->following(विस्थापनम्);
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ubrk_get_उपलब्धम्(int32_t सूचकांक)
{

  प्रत्यागम uloc_get_उपलब्धम्(सूचकांक);
}

U_CAPI int32_t U_EXPORT2
ubrk_गणनम्_उपलब्धम्()
{

  प्रत्यागम uloc_गणनम्_उपलब्धम्();
}


U_CAPI  UBool U_EXPORT2
ubrk_isBoundary(UBreakIterator *bi, int32_t विस्थापनम्)
{
    प्रत्यागम ((BreakIterator*)bi)->isBoundary(विस्थापनम्);
}


U_CAPI  int32_t U_EXPORT2
ubrk_get_नियमः_स्थितिः(UBreakIterator *bi)
{
    प्रत्यागम ((BreakIterator*)bi)->get_नियमः_स्थितिः();
}

U_CAPI  int32_t U_EXPORT2
ubrk_get_नियमः_स्थितिः_Vec(UBreakIterator *bi, int32_t *fill_अन्तर्_Vec, int32_t capacity, UError_संहिता *स्थितिः)
{
    प्रत्यागम ((BreakIterator*)bi)->get_नियमः_स्थितिः_Vec(fill_अन्तर्_Vec, capacity, *स्थितिः);
}


U_CAPI ध्रुव वर्ण* U_EXPORT2
ubrk_getLocaleByType(ध्रुव UBreakIterator* bi U_LIFETIME_BOUND,
                     ULoc_दत्त_Locale_Type type,
                     UError_संहिता* स्थितिः)
{
    यदि (bi == शून्यसूचकः) {
        यदि (U_SUCCESS(*स्थितिः)) {
            *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        }
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम ((BreakIterator*)bi)->getLocaleID(type, *स्थितिः);
}


U_CAPI शून्य U_EXPORT2
ubrk_refreshUText(UBreakIterator *bi,
                       UText          *text,
                       UError_संहिता     *स्थितिः)
{
    BreakIterator *bii = reinterpret_cast<BreakIterator *>(bi);
    bii->refresh_निवेशः_Text(text, *स्थितिः);
}

U_CAPI int32_t U_EXPORT2
ubrk_get_द्विमान_नियमाः(UBreakIterator *bi,
                    uint8_t *       द्विमान_नियमाः, int32_t नियमाः_क्षमता,
                    UError_संहिता *    स्थितिः)
{
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम 0;
    }
    यदि ((द्विमान_नियमाः == शून्यसूचकः && नियमाः_क्षमता > 0) || नियमाः_क्षमता < 0) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    नियमः_Based_Break_Iterator* rbbi;
    यदि ((rbbi = dynamic_cast<नियमः_Based_Break_Iterator*>(reinterpret_cast<BreakIterator*>(bi))) == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    uint32_t नियमाः_दीर्घता;
    ध्रुव uint8_t * returned_नियमाः = rbbi->get_द्विमान_नियमाः(नियमाः_दीर्घता);
    यदि (नियमाः_दीर्घता > INT32_MAX) {
        *स्थितिः = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }
    यदि (द्विमान_नियमाः != शून्यसूचकः) { // if not preflighting
        // Here we know rulesLength <= INT32_MAX and rulesCapacity >= 0, can cast safely
        यदि ((int32_t)नियमाः_दीर्घता > नियमाः_क्षमता) {
            *स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
        } अन्यथा {
            uprv_memcpy(द्विमान_नियमाः, returned_नियमाः, नियमाः_दीर्घता);
        }
    }
    प्रत्यागम (int32_t)नियमाः_दीर्घता;
}


#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */
