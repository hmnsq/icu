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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/casemap.h"
#అన్తర్భావ "unicode/edits.h"
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucasemap_imp.h"
#అన్తర్భావ "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t ప్రారంభం,
                             int32_t length,
                             స్థిరరాశి char16_t *srcChars,
                             int32_t src_ప్రారంభం,
                             int32_t src_నిడివి,
                             uint32_t options) స్థిరరాశి
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  ఒకవేళ(isBogus()) {
    తిరిగి -1;
  }

  // pin indices to legal values
  pinIndices(ప్రారంభం, length);

  ఒకవేళ(srcChars == శూన్యసూచిక) {
    src_ప్రారంభం = src_నిడివి = 0;
  }

  // get the correct pointer
  స్థిరరాశి char16_t *chars = get_Array_ప్రారంభం();

  chars += ప్రారంభం;
  ఒకవేళ(src_ప్రారంభం!=0) {
    srcChars += src_ప్రారంభం;
  }

  ఒకవేళ(chars != srcChars) {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    int32_t ఫలితం=u_strcmpFold(chars, length, srcChars, src_నిడివి,
                                options|U_COMPARE_IGNORE_CASE, &error_సంకేతం);
    ఒకవేళ(ఫలితం!=0) {
      తిరిగి static_cast<int8_t>(ఫలితం >> 24 | 1);
    }
  } లేకపోతే {
    // get the srcLength if necessary
    ఒకవేళ(src_నిడివి < 0) {
      src_నిడివి = u_strlen(srcChars + src_ప్రారంభం);
    }
    ఒకవేళ(length != src_నిడివి) {
      తిరిగి static_cast<int8_t>((length - src_నిడివి) >> 24 | 1);
    }
  }
  తిరిగి 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  ఒకవేళ(is_ఖాళీలు() || !isWritable()) {
    // nothing to do
    తిరిగి *ఇది;
  }

  char16_t old_బఫరు[2 * US_STACKBUF_కొలత];
  char16_t *oldArray;
  int32_t old_నిడివి = length();
  int32_t new_నిడివి;
  UBool writable = is_బఫరు_Writable();
  UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#అయితే_ముగింపు

  // Try to avoid heap-allocating a new character array for this string.
  ఒకవేళ (writable ? old_నిడివి <= UPRV_LENGTHOF(old_బఫరు) : old_నిడివి < US_STACKBUF_కొలత) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *బఫరు = get_Array_ప్రారంభం();
    int32_t capacity;
    oldArray = old_బఫరు;
    u_memcpy(old_బఫరు, బఫరు, old_నిడివి);
    ఒకవేళ (writable) {
      capacity = get_సామర్థ్యం();
    } లేకపోతే {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      ఒకవేళ (!cloneArrayIfNeeded(US_STACKBUF_కొలత, US_STACKBUF_కొలత, /* doCopyArray= */ అబద్ధం)) {
        తిరిగి *ఇది;
      }
      U_ASSERT(fUnion.fFields.f_నిడివి_And_Flags & k_Using_దొంతర_బఫరు);
      బఫరు = fUnion.f_దొంతర_Fields.f_బఫరు;
      capacity = US_STACKBUF_కొలత;
    }
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
    ఒకవేళ (iter != శూన్యసూచిక) {
      oldString.సమితి_గమ్యం(అబద్ధం, oldArray, old_నిడివి);
      iter->సమితి_పాఠ్యం(oldString);
    }
#అయితే_ముగింపు
    new_నిడివి = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 బఫరు, capacity,
                                 oldArray, old_నిడివి, శూన్యసూచిక, error_సంకేతం);
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
      సమితి_నిడివి(new_నిడివి);
      తిరిగి *ఇది;
    } లేకపోతే ఒకవేళ (error_సంకేతం == U_బఫరు_అధికప్రవాహం_ERROR) {
      // common overflow handling below
    } లేకపోతే {
      సమితి_గమ్యం_Bogus();
      తిరిగి *ఇది;
    }
  } లేకపోతే {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = get_Array_ప్రారంభం();
    Edits edits;
    char16_t replacementChars[200];
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
    ఒకవేళ (iter != శూన్యసూచిక) {
      oldString.సమితి_గమ్యం(అబద్ధం, oldArray, old_నిడివి);
      iter->సమితి_పాఠ్యం(oldString);
    }
#అయితే_ముగింపు
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_పాఠ్యం, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, old_నిడివి, &edits, error_సంకేతం);
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      new_నిడివి = old_నిడివి + edits.నిడివి_Delta();
      ఒకవేళ (new_నిడివి > old_నిడివి && !cloneArrayIfNeeded(new_నిడివి, new_నిడివి)) {
        తిరిగి *ఇది;
      }
      కొరకు (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(error_సంకేతం);) {
        doReplace(ei.destination_సూచీ(), ei.old_నిడివి(),
                  replacementChars, ei.replacement_సూచీ(), ei.new_నిడివి());
      }
      ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        సమితి_గమ్యం_Bogus();
      }
      తిరిగి *ఇది;
    } లేకపోతే ఒకవేళ (error_సంకేతం == U_బఫరు_అధికప్రవాహం_ERROR) {
      // common overflow handling below
      new_నిడివి = old_నిడివి + edits.నిడివి_Delta();
    } లేకపోతే {
      సమితి_గమ్యం_Bogus();
      తిరిగి *ఇది;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *బఫరు_గమ్యం_Delete = శూన్యసూచిక;
  ఒకవేళ (!cloneArrayIfNeeded(new_నిడివి, new_నిడివి, అబద్ధం, &బఫరు_గమ్యం_Delete, నిజం)) {
    తిరిగి *ఇది;
  }
  error_సంకేతం = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  new_నిడివి = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               get_Array_ప్రారంభం(), get_సామర్థ్యం(),
                               oldArray, old_నిడివి, శూన్యసూచిక, error_సంకేతం);
  ఒకవేళ (బఫరు_గమ్యం_Delete) {
    uprv_free(బఫరు_గమ్యం_Delete);
  }
  ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
    సమితి_నిడివి(new_నిడివి);
  } లేకపోతే {
    సమితి_గమ్యం_Bogus();
  }
  తిరిగి *ఇది;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  తిరిగి caseMap(UCASE_LOC_మూలం, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_అంతర్గత_Fold);
}

U_NAMESPACE_చివర

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(స్థిరరాశి UElement కీలకం) {
    U_NAMESPACE_USE
    స్థిరరాశి UnicodeString *str = (స్థిరరాశి UnicodeString*) కీలకం.సూచిక;
    ఒకవేళ (str == శూన్యసూచిక) {
        తిరిగి 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    తిరిగి copy.foldCase().hash_సంకేతం();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(స్థిరరాశి UElement key1, స్థిరరాశి UElement key2) {
    U_NAMESPACE_USE
    స్థిరరాశి UnicodeString *str1 = (స్థిరరాశి UnicodeString*) key1.సూచిక;
    స్థిరరాశి UnicodeString *str2 = (స్థిరరాశి UnicodeString*) key2.సూచిక;
    ఒకవేళ (str1 == str2) {
        తిరిగి నిజం;
    }
    ఒకవేళ (str1 == శూన్యసూచిక || str2 == శూన్యసూచిక) {
        తిరిగి అబద్ధం;
    }
    తిరిగి str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
