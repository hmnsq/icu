// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1999-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  unistr_case.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:2
*
*   created on: 2004aug19
*   created by: Markus W. Scherer
*
*   Case-mapping functions moved here from unistr.cpp
*/

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/casemap.h"
#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucasemap_imp.h"
#अन्तर्भाव "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t start,
                             int32_t length,
                             ध्रुव char16_t *srcChars,
                             int32_t srcStart,
                             int32_t src_दीर्घता,
                             uint32_t options) ध्रुव
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  यदि(isBogus()) {
    प्रत्यागम -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  यदि(srcChars == शून्यसूचकः) {
    srcStart = src_दीर्घता = 0;
  }

  // get the correct pointer
  ध्रुव char16_t *chars = getArrayStart();

  chars += start;
  यदि(srcStart!=0) {
    srcChars += srcStart;
  }

  यदि(chars != srcChars) {
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    int32_t परिणाम=u_strcmpFold(chars, length, srcChars, src_दीर्घता,
                                options|U_COMPARE_IGNORE_CASE, &error_संहिता);
    यदि(परिणाम!=0) {
      प्रत्यागम static_cast<int8_t>(परिणाम >> 24 | 1);
    }
  } अन्यथा {
    // get the srcLength if necessary
    यदि(src_दीर्घता < 0) {
      src_दीर्घता = u_strlen(srcChars + srcStart);
    }
    यदि(length != src_दीर्घता) {
      प्रत्यागम static_cast<int8_t>((length - src_दीर्घता) >> 24 | 1);
    }
  }
  प्रत्यागम 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  यदि(is_रिक्तानि() || !isWritable()) {
    // nothing to do
    प्रत्यागम *एतत्;
  }

  char16_t old_बफर[2 * US_STACKBUF_आकार];
  char16_t *oldArray;
  int32_t old_दीर्घता = length();
  int32_t new_दीर्घता;
  UBool writable = is_बफर_Writable();
  UError_संहिता error_संहिता = U_ZERO_ERROR;

#यदि !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#यदि_अन्तः

  // Try to avoid heap-allocating a new character array for this string.
  यदि (writable ? old_दीर्घता <= UPRV_LENGTHOF(old_बफर) : old_दीर्घता < US_STACKBUF_आकार) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *बफर = getArrayStart();
    int32_t capacity;
    oldArray = old_बफर;
    u_memcpy(old_बफर, बफर, old_दीर्घता);
    यदि (writable) {
      capacity = get_क्षमता();
    } अन्यथा {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      यदि (!cloneArrayIfNeeded(US_STACKBUF_आकार, US_STACKBUF_आकार, /* doCopyArray= */ असत्य)) {
        प्रत्यागम *एतत्;
      }
      U_ASSERT(fUnion.fFields.f_दीर्घता_And_Flags & k_Using_राशिः_बफर);
      बफर = fUnion.f_राशिः_Fields.f_बफर;
      capacity = US_STACKBUF_आकार;
    }
#यदि !UCONFIG_NO_BREAK_ITERATION
    यदि (iter != शून्यसूचकः) {
      oldString.समुच्चयः_गन्तव्यम्(असत्य, oldArray, old_दीर्घता);
      iter->समुच्चयः_Text(oldString);
    }
#यदि_अन्तः
    new_दीर्घता = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 बफर, capacity,
                                 oldArray, old_दीर्घता, शून्यसूचकः, error_संहिता);
    यदि (U_SUCCESS(error_संहिता)) {
      समुच्चयः_दीर्घता(new_दीर्घता);
      प्रत्यागम *एतत्;
    } अन्यथा यदि (error_संहिता == U_बफर_अतिप्रवाहः_ERROR) {
      // common overflow handling below
    } अन्यथा {
      समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम *एतत्;
    }
  } अन्यथा {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#यदि !UCONFIG_NO_BREAK_ITERATION
    यदि (iter != शून्यसूचकः) {
      oldString.समुच्चयः_गन्तव्यम्(असत्य, oldArray, old_दीर्घता);
      iter->समुच्चयः_Text(oldString);
    }
#यदि_अन्तः
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, old_दीर्घता, &edits, error_संहिता);
    यदि (U_SUCCESS(error_संहिता)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      new_दीर्घता = old_दीर्घता + edits.दीर्घता_Delta();
      यदि (new_दीर्घता > old_दीर्घता && !cloneArrayIfNeeded(new_दीर्घता, new_दीर्घता)) {
        प्रत्यागम *एतत्;
      }
      प्रति (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(error_संहिता);) {
        doReplace(ei.destination_सूचकांक(), ei.old_दीर्घता(),
                  replacementChars, ei.replacement_सूचकांक(), ei.new_दीर्घता());
      }
      यदि (U_FAILURE(error_संहिता)) {
        समुच्चयः_गन्तव्यम्_Bogus();
      }
      प्रत्यागम *एतत्;
    } अन्यथा यदि (error_संहिता == U_बफर_अतिप्रवाहः_ERROR) {
      // common overflow handling below
      new_दीर्घता = old_दीर्घता + edits.दीर्घता_Delta();
    } अन्यथा {
      समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम *एतत्;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *बफर_गन्तव्यम्_Delete = शून्यसूचकः;
  यदि (!cloneArrayIfNeeded(new_दीर्घता, new_दीर्घता, असत्य, &बफर_गन्तव्यम्_Delete, सत्य)) {
    प्रत्यागम *एतत्;
  }
  error_संहिता = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  new_दीर्घता = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), get_क्षमता(),
                               oldArray, old_दीर्घता, शून्यसूचकः, error_संहिता);
  यदि (बफर_गन्तव्यम्_Delete) {
    uprv_free(बफर_गन्तव्यम्_Delete);
  }
  यदि (U_SUCCESS(error_संहिता)) {
    समुच्चयः_दीर्घता(new_दीर्घता);
  } अन्यथा {
    समुच्चयः_गन्तव्यम्_Bogus();
  }
  प्रत्यागम *एतत्;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  प्रत्यागम caseMap(UCASE_LOC_मूलम्, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_आन्तरिकम्_Fold);
}

U_NAMESPACE_अन्तः

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ध्रुव UElement कुञ्चिका) {
    U_NAMESPACE_USE
    ध्रुव UnicodeString *str = (ध्रुव UnicodeString*) कुञ्चिका.pointer;
    यदि (str == शून्यसूचकः) {
        प्रत्यागम 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    प्रत्यागम copy.foldCase().hash_संहिता();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ध्रुव UElement key1, ध्रुव UElement key2) {
    U_NAMESPACE_USE
    ध्रुव UnicodeString *str1 = (ध्रुव UnicodeString*) key1.pointer;
    ध्रुव UnicodeString *str2 = (ध्रुव UnicodeString*) key2.pointer;
    यदि (str1 == str2) {
        प्रत्यागम सत्य;
    }
    यदि (str1 == शून्यसूचकः || str2 == शून्यसूचकः) {
        प्रत्यागम असत्य;
    }
    प्रत्यागम str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
