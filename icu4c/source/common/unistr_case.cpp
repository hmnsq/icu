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
                             स्थिरांक char16_t *srcChars,
                             int32_t srcStart,
                             int32_t srcLength,
                             uint32_t options) स्थिरांक
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  जर(isBogus()) {
    परत -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  जर(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  स्थिरांक char16_t *chars = getArrayStart();

  chars += start;
  जर(srcStart!=0) {
    srcChars += srcStart;
  }

  जर(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    जर(result!=0) {
      परत static_cast<int8_t>(result >> 24 | 1);
    }
  } अन्यथा {
    // get the srcLength if necessary
    जर(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    जर(length != srcLength) {
      परत static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  परत 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  जर(isEmpty() || !isWritable()) {
    // nothing to do
    परत *हे;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#जर !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  जर (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    जर (writable) {
      capacity = getCapacity();
    } अन्यथा {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      जर (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ असत्य)) {
        परत *हे;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & k_Using_चळत_Buffer);
      buffer = fUnion.f_चळत_Fields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#जर !UCONFIG_NO_BREAK_ITERATION
    जर (iter != nullptr) {
      oldString.setTo(असत्य, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    जर (U_SUCCESS(errorCode)) {
      setLength(newLength);
      परत *हे;
    } अन्यथा जर (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } अन्यथा {
      setToBogus();
      परत *हे;
    }
  } अन्यथा {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#जर !UCONFIG_NO_BREAK_ITERATION
    जर (iter != nullptr) {
      oldString.setTo(असत्य, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    जर (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      जर (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        परत *हे;
      }
      साठी (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      जर (U_FAILURE(errorCode)) {
        setToBogus();
      }
      परत *हे;
    } अन्यथा जर (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } अन्यथा {
      setToBogus();
      परत *हे;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  जर (!cloneArrayIfNeeded(newLength, newLength, असत्य, &bufferToDelete, सत्य)) {
    परत *हे;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  जर (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  जर (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } अन्यथा {
    setToBogus();
  }
  परत *हे;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  परत caseMap(UCASE_LOC_मूळ, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(स्थिरांक UElement किल्ली) {
    U_NAMESPACE_USE
    स्थिरांक UnicodeString *str = (स्थिरांक UnicodeString*) किल्ली.pointer;
    जर (str == nullptr) {
        परत 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    परत copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(स्थिरांक UElement key1, स्थिरांक UElement key2) {
    U_NAMESPACE_USE
    स्थिरांक UnicodeString *str1 = (स्थिरांक UnicodeString*) key1.pointer;
    स्थिरांक UnicodeString *str2 = (स्थिरांक UnicodeString*) key2.pointer;
    जर (str1 == str2) {
        परत सत्य;
    }
    जर (str1 == nullptr || str2 == nullptr) {
        परत असत्य;
    }
    परत str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
