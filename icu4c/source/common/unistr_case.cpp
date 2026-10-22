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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/casemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/edits.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucasemap_imp.h"
#ಅನ್ತರ್ಭಾವ "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t start,
                             int32_t length,
                             ಧ್ರುವ char16_t *srcChars,
                             int32_t srcStart,
                             int32_t srcLength,
                             uint32_t options) ಧ್ರುವ
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  ಯದಿ(isBogus()) {
    ಪ್ರತ್ಯಾಗಮ -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  ಯದಿ(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  ಧ್ರುವ char16_t *chars = getArrayStart();

  chars += start;
  ಯದಿ(srcStart!=0) {
    srcChars += srcStart;
  }

  ಯದಿ(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    ಯದಿ(result!=0) {
      ಪ್ರತ್ಯಾಗಮ static_cast<int8_t>(result >> 24 | 1);
    }
  } ಅನ್ಯಥಾ {
    // get the srcLength if necessary
    ಯದಿ(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    ಯದಿ(length != srcLength) {
      ಪ್ರತ್ಯಾಗಮ static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  ಯದಿ(isEmpty() || !isWritable()) {
    // nothing to do
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  ಯದಿ (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    ಯದಿ (writable) {
      capacity = getCapacity();
    } ಅನ್ಯಥಾ {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      ಯದಿ (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ ಅಸತ್ಯ)) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & k_Using_ರಾಶಿ_Buffer);
      buffer = fUnion.f_ರಾಶಿ_Fields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#ಯದಿ !UCONFIG_NO_BREAK_ITERATION
    ಯದಿ (iter != nullptr) {
      oldString.setTo(ಅಸತ್ಯ, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    ಯದಿ (U_SUCCESS(errorCode)) {
      setLength(newLength);
      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    } ಅನ್ಯಥಾ ಯದಿ (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } ಅನ್ಯಥಾ {
      setToBogus();
      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
  } ಅನ್ಯಥಾ {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#ಯದಿ !UCONFIG_NO_BREAK_ITERATION
    ಯದಿ (iter != nullptr) {
      oldString.setTo(ಅಸತ್ಯ, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    ಯದಿ (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      ಯದಿ (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
      }
      ಪ್ರತಿ (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      ಯದಿ (U_FAILURE(errorCode)) {
        setToBogus();
      }
      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    } ಅನ್ಯಥಾ ಯದಿ (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } ಅನ್ಯಥಾ {
      setToBogus();
      ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  ಯದಿ (!cloneArrayIfNeeded(newLength, newLength, ಅಸತ್ಯ, &bufferToDelete, ಸತ್ಯ)) {
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  ಯದಿ (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  ಯದಿ (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } ಅನ್ಯಥಾ {
    setToBogus();
  }
  ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  ಪ್ರತ್ಯಾಗಮ caseMap(UCASE_LOC_ಬೇರು, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ಧ್ರುವ UElement ಕೀಲಿ) {
    U_NAMESPACE_USE
    ಧ್ರುವ UnicodeString *str = (ಧ್ರುವ UnicodeString*) ಕೀಲಿ.pointer;
    ಯದಿ (str == nullptr) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    ಪ್ರತ್ಯಾಗಮ copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ಧ್ರುವ UElement key1, ಧ್ರುವ UElement key2) {
    U_NAMESPACE_USE
    ಧ್ರುವ UnicodeString *str1 = (ಧ್ರುವ UnicodeString*) key1.pointer;
    ಧ್ರುವ UnicodeString *str2 = (ಧ್ರುವ UnicodeString*) key2.pointer;
    ಯದಿ (str1 == str2) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (str1 == nullptr || str2 == nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
