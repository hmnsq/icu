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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/casemap.h"
#അന്തര്ഭാവ "unicode/edits.h"
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucasemap_imp.h"
#അന്തര്ഭാവ "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t start,
                             int32_t length,
                             സ്ഥിരം char16_t *srcChars,
                             int32_t srcStart,
                             int32_t srcLength,
                             uint32_t options) സ്ഥിരം
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  എങ്കിൽ(isBogus()) {
    തിരികെ -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  എങ്കിൽ(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  സ്ഥിരം char16_t *chars = getArrayStart();

  chars += start;
  എങ്കിൽ(srcStart!=0) {
    srcChars += srcStart;
  }

  എങ്കിൽ(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    എങ്കിൽ(result!=0) {
      തിരികെ static_cast<int8_t>(result >> 24 | 1);
    }
  } അല്ലെങ്കിൽ {
    // get the srcLength if necessary
    എങ്കിൽ(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    എങ്കിൽ(length != srcLength) {
      തിരികെ static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  തിരികെ 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  എങ്കിൽ(isEmpty() || !isWritable()) {
    // nothing to do
    തിരികെ *ഇത്;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  എങ്കിൽ (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    എങ്കിൽ (writable) {
      capacity = getCapacity();
    } അല്ലെങ്കിൽ {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      എങ്കിൽ (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ അസത്യം)) {
        തിരികെ *ഇത്;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & k_Using_അട്ടി_Buffer);
      buffer = fUnion.f_അട്ടി_Fields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION
    എങ്കിൽ (iter != nullptr) {
      oldString.setTo(അസത്യം, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    എങ്കിൽ (U_SUCCESS(errorCode)) {
      setLength(newLength);
      തിരികെ *ഇത്;
    } അല്ലെങ്കിൽ എങ്കിൽ (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } അല്ലെങ്കിൽ {
      setToBogus();
      തിരികെ *ഇത്;
    }
  } അല്ലെങ്കിൽ {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION
    എങ്കിൽ (iter != nullptr) {
      oldString.setTo(അസത്യം, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    എങ്കിൽ (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      എങ്കിൽ (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        തിരികെ *ഇത്;
      }
      വേണ്ടി (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      എങ്കിൽ (U_FAILURE(errorCode)) {
        setToBogus();
      }
      തിരികെ *ഇത്;
    } അല്ലെങ്കിൽ എങ്കിൽ (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } അല്ലെങ്കിൽ {
      setToBogus();
      തിരികെ *ഇത്;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  എങ്കിൽ (!cloneArrayIfNeeded(newLength, newLength, അസത്യം, &bufferToDelete, സത്യം)) {
    തിരികെ *ഇത്;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  എങ്കിൽ (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  എങ്കിൽ (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } അല്ലെങ്കിൽ {
    setToBogus();
  }
  തിരികെ *ഇത്;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  തിരികെ caseMap(UCASE_LOC_വേര്, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(സ്ഥിരം UElement താക്കോൽ) {
    U_NAMESPACE_USE
    സ്ഥിരം UnicodeString *str = (സ്ഥിരം UnicodeString*) താക്കോൽ.pointer;
    എങ്കിൽ (str == nullptr) {
        തിരികെ 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    തിരികെ copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(സ്ഥിരം UElement key1, സ്ഥിരം UElement key2) {
    U_NAMESPACE_USE
    സ്ഥിരം UnicodeString *str1 = (സ്ഥിരം UnicodeString*) key1.pointer;
    സ്ഥിരം UnicodeString *str2 = (സ്ഥിരം UnicodeString*) key2.pointer;
    എങ്കിൽ (str1 == str2) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (str1 == nullptr || str2 == nullptr) {
        തിരികെ അസത്യം;
    }
    തിരികെ str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
