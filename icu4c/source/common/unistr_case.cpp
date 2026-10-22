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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/brkiter.h"
#અન્તર્ભાવ "unicode/casemap.h"
#અન્તર્ભાવ "unicode/edits.h"
#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucasemap_imp.h"
#અન્તર્ભાવ "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t start,
                             int32_t length,
                             અચલ char16_t *srcChars,
                             int32_t srcStart,
                             int32_t srcLength,
                             uint32_t options) અચલ
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  જો(isBogus()) {
    પાછા -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  જો(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  અચલ char16_t *chars = getArrayStart();

  chars += start;
  જો(srcStart!=0) {
    srcChars += srcStart;
  }

  જો(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    જો(result!=0) {
      પાછા static_cast<int8_t>(result >> 24 | 1);
    }
  } નહીંતર {
    // get the srcLength if necessary
    જો(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    જો(length != srcLength) {
      પાછા static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  પાછા 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  જો(isEmpty() || !isWritable()) {
    // nothing to do
    પાછા *આ;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#જો !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  જો (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    જો (writable) {
      capacity = getCapacity();
    } નહીંતર {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      જો (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ ખોટું)) {
        પાછા *આ;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & k_Using_ઢગલો_Buffer);
      buffer = fUnion.f_ઢગલો_Fields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#જો !UCONFIG_NO_BREAK_ITERATION
    જો (iter != nullptr) {
      oldString.setTo(ખોટું, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    જો (U_SUCCESS(errorCode)) {
      setLength(newLength);
      પાછા *આ;
    } નહીંતર જો (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } નહીંતર {
      setToBogus();
      પાછા *આ;
    }
  } નહીંતર {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#જો !UCONFIG_NO_BREAK_ITERATION
    જો (iter != nullptr) {
      oldString.setTo(ખોટું, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    જો (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      જો (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        પાછા *આ;
      }
      માટે (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      જો (U_FAILURE(errorCode)) {
        setToBogus();
      }
      પાછા *આ;
    } નહીંતર જો (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } નહીંતર {
      setToBogus();
      પાછા *આ;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  જો (!cloneArrayIfNeeded(newLength, newLength, ખોટું, &bufferToDelete, સાચું)) {
    પાછા *આ;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  જો (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  જો (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } નહીંતર {
    setToBogus();
  }
  પાછા *આ;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  પાછા caseMap(UCASE_LOC_મૂળ, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(અચલ UElement ચાવી) {
    U_NAMESPACE_USE
    અચલ UnicodeString *str = (અચલ UnicodeString*) ચાવી.pointer;
    જો (str == nullptr) {
        પાછા 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    પાછા copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(અચલ UElement key1, અચલ UElement key2) {
    U_NAMESPACE_USE
    અચલ UnicodeString *str1 = (અચલ UnicodeString*) key1.pointer;
    અચલ UnicodeString *str2 = (અચલ UnicodeString*) key2.pointer;
    જો (str1 == str2) {
        પાછા સાચું;
    }
    જો (str1 == nullptr || str2 == nullptr) {
        પાછા ખોટું;
    }
    પાછા str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
