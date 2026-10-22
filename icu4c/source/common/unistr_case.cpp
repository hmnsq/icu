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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/brkiter.h"
#অন্তর্ভাৱ "unicode/casemap.h"
#অন্তর্ভাৱ "unicode/edits.h"
#অন্তর্ভাৱ "unicode/putil.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "unicode/uchar.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "ucasemap_imp.h"
#অন্তর্ভাৱ "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t start,
                             int32_t length,
                             ধ্রুৱ char16_t *srcChars,
                             int32_t srcStart,
                             int32_t srcLength,
                             uint32_t options) ধ্রুৱ
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  যদি(isBogus()) {
    ঘূৰাই -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  যদি(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  ধ্রুৱ char16_t *chars = getArrayStart();

  chars += start;
  যদি(srcStart!=0) {
    srcChars += srcStart;
  }

  যদি(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    যদি(result!=0) {
      ঘূৰাই static_cast<int8_t>(result >> 24 | 1);
    }
  } অন্যথা {
    // get the srcLength if necessary
    যদি(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    যদি(length != srcLength) {
      ঘূৰাই static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  ঘূৰাই 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  যদি(isEmpty() || !isWritable()) {
    // nothing to do
    ঘূৰাই *এতত্;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#যদি !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  যদি (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    যদি (writable) {
      capacity = getCapacity();
    } অন্যথা {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      যদি (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ মিছা)) {
        ঘূৰাই *এতত্;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & k_Using_স্তূপ_Buffer);
      buffer = fUnion.f_স্তূপ_Fields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#যদি !UCONFIG_NO_BREAK_ITERATION
    যদি (iter != nullptr) {
      oldString.setTo(মিছা, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    যদি (U_SUCCESS(errorCode)) {
      setLength(newLength);
      ঘূৰাই *এতত্;
    } অন্যথা যদি (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } অন্যথা {
      setToBogus();
      ঘূৰাই *এতত্;
    }
  } অন্যথা {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#যদি !UCONFIG_NO_BREAK_ITERATION
    যদি (iter != nullptr) {
      oldString.setTo(মিছা, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    যদি (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      যদি (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        ঘূৰাই *এতত্;
      }
      বাবে (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      যদি (U_FAILURE(errorCode)) {
        setToBogus();
      }
      ঘূৰাই *এতত্;
    } অন্যথা যদি (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } অন্যথা {
      setToBogus();
      ঘূৰাই *এতত্;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  যদি (!cloneArrayIfNeeded(newLength, newLength, মিছা, &bufferToDelete, সঁচা)) {
    ঘূৰাই *এতত্;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  যদি (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  যদি (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } অন্যথা {
    setToBogus();
  }
  ঘূৰাই *এতত্;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  ঘূৰাই caseMap(UCASE_LOC_মূল, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ধ্রুৱ UElement চাবি) {
    U_NAMESPACE_USE
    ধ্রুৱ UnicodeString *str = (ধ্রুৱ UnicodeString*) চাবি.pointer;
    যদি (str == nullptr) {
        ঘূৰাই 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    ঘূৰাই copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ধ্রুৱ UElement key1, ধ্রুৱ UElement key2) {
    U_NAMESPACE_USE
    ধ্রুৱ UnicodeString *str1 = (ধ্রুৱ UnicodeString*) key1.pointer;
    ধ্রুৱ UnicodeString *str2 = (ধ্রুৱ UnicodeString*) key2.pointer;
    যদি (str1 == str2) {
        ঘূৰাই সঁচা;
    }
    যদি (str1 == nullptr || str2 == nullptr) {
        ঘূৰাই মিছা;
    }
    ঘূৰাই str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
