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
                             int32_t srcLength,
                             uint32_t options) ध्रुव
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  सचे(isBogus()) {
    पटिनिवत्त -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  सचे(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  ध्रुव char16_t *chars = getArrayStart();

  chars += start;
  सचे(srcStart!=0) {
    srcChars += srcStart;
  }

  सचे(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    सचे(result!=0) {
      पटिनिवत्त static_cast<int8_t>(result >> 24 | 1);
    }
  } अञ्ञथा {
    // get the srcLength if necessary
    सचे(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    सचे(length != srcLength) {
      पटिनिवत्त static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  पटिनिवत्त 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  सचे(isEmpty() || !isWritable()) {
    // nothing to do
    पटिनिवत्त *एतत्;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#सचे !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  सचे (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    सचे (writable) {
      capacity = getCapacity();
    } अञ्ञथा {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      सचे (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ मिच्छा)) {
        पटिनिवत्त *एतत्;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & k_Using_राशिः_Buffer);
      buffer = fUnion.f_राशिः_Fields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#सचे !UCONFIG_NO_BREAK_ITERATION
    सचे (iter != nullptr) {
      oldString.setTo(मिच्छा, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    सचे (U_SUCCESS(errorCode)) {
      setLength(newLength);
      पटिनिवत्त *एतत्;
    } अञ्ञथा सचे (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } अञ्ञथा {
      setToBogus();
      पटिनिवत्त *एतत्;
    }
  } अञ्ञथा {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#सचे !UCONFIG_NO_BREAK_ITERATION
    सचे (iter != nullptr) {
      oldString.setTo(मिच्छा, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    सचे (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      सचे (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        पटिनिवत्त *एतत्;
      }
      अत्थाय (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      सचे (U_FAILURE(errorCode)) {
        setToBogus();
      }
      पटिनिवत्त *एतत्;
    } अञ्ञथा सचे (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } अञ्ञथा {
      setToBogus();
      पटिनिवत्त *एतत्;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  सचे (!cloneArrayIfNeeded(newLength, newLength, मिच्छा, &bufferToDelete, सच्च)) {
    पटिनिवत्त *एतत्;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  सचे (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  सचे (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } अञ्ञथा {
    setToBogus();
  }
  पटिनिवत्त *एतत्;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  पटिनिवत्त caseMap(UCASE_LOC_मूलम्, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ध्रुव UElement कुञ्चिका) {
    U_NAMESPACE_USE
    ध्रुव UnicodeString *str = (ध्रुव UnicodeString*) कुञ्चिका.pointer;
    सचे (str == nullptr) {
        पटिनिवत्त 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    पटिनिवत्त copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ध्रुव UElement key1, ध्रुव UElement key2) {
    U_NAMESPACE_USE
    ध्रुव UnicodeString *str1 = (ध्रुव UnicodeString*) key1.pointer;
    ध्रुव UnicodeString *str2 = (ध्रुव UnicodeString*) key2.pointer;
    सचे (str1 == str2) {
        पटिनिवत्त सच्च;
    }
    सचे (str1 == nullptr || str2 == nullptr) {
        पटिनिवत्त मिच्छा;
    }
    पटिनिवत्त str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
