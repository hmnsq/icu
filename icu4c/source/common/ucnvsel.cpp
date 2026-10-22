// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2008-2011, International Business Machines
*   Corporation, Google and others.  All Rights Reserved.
*
*******************************************************************************
*/
// Author : eldawy@google.com (Mohamed Eldawy)
// ucnvsel.cpp
//
// Purpose: To generate a list of encodings capable of handling
// a given Unicode text
//
// Started 09-April-2008

/**
 * \file
 *
 * This is an implementation of an encoding selector.
 * The goal is, given a unicode string, find the encodings
 * this string can be mapped to. To make processing faster
 * a trie is built when you call ucnvsel_open() that
 * stores all encodings a codepoint can map to
 */

#include "unicode/ucnvsel.h"

#གལ་ཏེ !UCONFIG_NO_CONVERSION

#include <string.h>

#include "unicode/uchar.h"
#include "unicode/uniset.h"
#include "unicode/ucnv.h"
#include "unicode/ustring.h"
#include "unicode/uchriter.h"
#include "utrie2.h"
#include "propsvec.h"
#include "uassert.h"
#include "ucmndata.h"
#include "udataswp.h"
#include "uenumimp.h"
#include "cmemory.h"
#include "cstring.h"

U_NAMESPACE_USE

sa_nracanaa UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pvCount;
  var_na** encodings;          // which encodings did user ask to use?
  int32_t encodingsCount;
  int32_t encodingStrLength;
  uint8_t* swapped;
  UBool ownPv, ownEncodingStrings;
};

static void generateSelectorData(UConverterSelector* result,
                                 UPropsVectors *upvec,
                                 const USet* excludedCodePoints,
                                 const UConverterUnicodeSet whichSet,
                                 UErrorCode* status) {
  གལ་ཏེ (U_FAILURE(*status)) {
    ཕྱིར;
  }

  int32_t columns = (result->encodingsCount+31)/32;

  // set errorValue to all-ones
  ཆེད་དུ (int32_t col = 0; col < columns; col++) {
    upvec_setValue(upvec, UPVEC_ERROR_VALUE_CP, UPVEC_ERROR_VALUE_CP,
                   col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), status);
  }

  ཆེད་དུ (int32_t i = 0; i < result->encodingsCount; ++i) {
    uint32_t mask;
    uint32_t column;
    int32_t item_count;
    int32_t j;
    UConverter* test_converter = ucnv_open(result->encodings[i], status);
    གལ་ཏེ (U_FAILURE(*status)) {
      ཕྱིར;
    }
    USet* unicode_point_set;
    unicode_point_set = uset_open(1, 0);  // empty set

    ucnv_getUnicodeSet(test_converter, unicode_point_set,
                       whichSet, status);
    གལ་ཏེ (U_FAILURE(*status)) {
      ucnv_close(test_converter);
      ཕྱིར;
    }

    column = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    item_count = uset_getItemCount(unicode_point_set);

    ཆེད་དུ (j = 0; j < item_count; ++j) {
      UChar32 start_char;
      UChar32 end_char;
      UErrorCode smallStatus = U_ZERO_ERROR;
      uset_getItem(unicode_point_set, j, &start_char, &end_char, nullptr, 0,
                   &smallStatus);
      གལ་ཏེ (U_FAILURE(smallStatus)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } དེ་མིན {
        upvec_setValue(upvec, start_char, end_char, column, static_cast<uint32_t>(~0), mask,
                       status);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_point_set);
    གལ་ཏེ (U_FAILURE(*status)) {
      ཕྱིར;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  གལ་ཏེ (excludedCodePoints) {
    int32_t item_count = uset_getItemCount(excludedCodePoints);
    ཆེད་དུ (int32_t j = 0; j < item_count; ++j) {
      UChar32 start_char;
      UChar32 end_char;

      uset_getItem(excludedCodePoints, j, &start_char, &end_char, nullptr, 0,
                   status);
      ཆེད་དུ (int32_t col = 0; col < columns; col++) {
        upvec_setValue(upvec, start_char, end_char, col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      status);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  result->trie = upvec_compactToUTrie2WithRowIndexes(upvec, status);
  result->pv = upvec_cloneArray(upvec, &result->pvCount, nullptr, status);
  result->pvCount *= columns;  // number of uint32_t = rows * columns
  result->ownPv = true;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(const var_na* const*  converterList, int32_t converterListSize,
             const USet* excludedCodePoints,
             const UConverterUnicodeSet whichSet, UErrorCode* status) {
  // check if already failed
  གལ་ཏེ (U_FAILURE(*status)) {
    ཕྱིར nullptr;
  }
  // ensure args make sense!
  གལ་ཏེ (converterListSize < 0 || (converterList == nullptr && converterListSize != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ཕྱིར nullptr;
  }

  // allocate a new converter
  LocalUConverterSelectorPointer newSelector(
    (UConverterSelector*)uprv_malloc(parimaa_na(UConverterSelector)));
  གལ་ཏེ (newSelector.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ཕྱིར nullptr;
  }
  uprv_memset(newSelector.getAlias(), 0, parimaa_na(UConverterSelector));

  གལ་ཏེ (converterListSize == 0) {
    converterList = nullptr;
    converterListSize = ucnv_countAvailable();
  }
  newSelector->encodings =
    (var_na**)uprv_malloc(converterListSize * parimaa_na(var_na*));
  གལ་ཏེ (!newSelector->encodings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ཕྱིར nullptr;
  }
  newSelector->encodings[0] = nullptr;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t totalSize = 0;
  int32_t i;
  ཆེད་དུ (i = 0; i < converterListSize; i++) {
    totalSize +=
      (int32_t)uprv_strlen(converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = totalSize & 3;
  གལ་ཏེ (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encodingStrLength = totalSize += encodingStrPadding;
  var_na* allStrings = (var_na*) uprv_malloc(totalSize);
  གལ་ཏེ (!allStrings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ཕྱིར nullptr;
  }

  ཆེད་དུ (i = 0; i < converterListSize; i++) {
    newSelector->encodings[i] = allStrings;
    uprv_strcpy(newSelector->encodings[i],
                converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i));
    allStrings += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  སྐབས (encodingStrPadding > 0) {
    *allStrings++ = 0;
    --encodingStrPadding;
  }

  newSelector->ownEncodingStrings = true;
  newSelector->encodingsCount = converterListSize;
  UPropsVectors *upvec = upvec_open((converterListSize+31)/32, status);
  generateSelectorData(newSelector.getAlias(), upvec, excludedCodePoints, whichSet, status);
  upvec_close(upvec);

  གལ་ཏེ (U_FAILURE(*status)) {
    ཕྱིར nullptr;
  }

  ཕྱིར newSelector.orphan();
}

/* close opened selector */
U_CAPI void U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  གལ་ཏེ (!sel) {
    ཕྱིར;
  }
  གལ་ཏེ (sel->ownEncodingStrings) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  གལ་ཏེ (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

static const UDataInfo dataInfo = {
  parimaa_na(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

ga_nanaa {
  UCNVSEL_INDEX_TRIE_SIZE,      // trie size in bytes
  UCNVSEL_INDEX_PV_COUNT,       // number of uint32_t in the bit vectors
  UCNVSEL_INDEX_NAMES_COUNT,    // number of encoding names
  UCNVSEL_INDEX_NAMES_LENGTH,   // number of encoding name bytes including padding
  UCNVSEL_INDEX_SIZE = 15,      // bytes following the DataHeader
  UCNVSEL_INDEX_COUNT = 16
};

/*
 * Serialized form of a UConverterSelector, formatVersion 1:
 *
 * The serialized form begins with a standard ICU DataHeader with a UDataInfo
 * as the template above.
 * This is followed by:
 *   int32_t indexes[UCNVSEL_INDEX_COUNT];          // see index entry constants above
 *   serialized UTrie2;                             // indexes[UCNVSEL_INDEX_TRIE_SIZE] bytes
 *   uint32_t pv[indexes[UCNVSEL_INDEX_PV_COUNT]];  // bit vectors
 *   char* encodingNames[indexes[UCNVSEL_INDEX_NAMES_LENGTH]];  // NUL-terminated strings + padding
 */

/* serialize a selector */
U_CAPI int32_t U_EXPORT2
ucnvsel_serialize(const UConverterSelector* sel,
                  void* buffer, int32_t bufferCapacity, UErrorCode* status) {
  // check if already failed
  གལ་ཏེ (U_FAILURE(*status)) {
    ཕྱིར 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)buffer;
  གལ་ཏེ (bufferCapacity < 0 ||
      (bufferCapacity > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ཕྱིར 0;
  }
  // add up the size of the serialized form
  int32_t serializedTrieSize = utrie2_serialize(sel->trie, nullptr, 0, status);
  གལ་ཏེ (*status != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(*status)) {
    ཕྱིར 0;
  }
  *status = U_ZERO_ERROR;

  DataHeader header;
  uprv_memset(&header, 0, parimaa_na(header));
  header.dataHeader.headerSize = (uint16_t)((parimaa_na(header) + 15) & ~15);
  header.dataHeader.magic1 = 0xda;
  header.dataHeader.magic2 = 0x27;
  uprv_memcpy(&header.info, &dataInfo, parimaa_na(dataInfo));

  int32_t indexes[UCNVSEL_INDEX_COUNT] = {
    serializedTrieSize,
    sel->pvCount,
    sel->encodingsCount,
    sel->encodingStrLength
  };

  int32_t totalSize =
    header.dataHeader.headerSize +
    (int32_t)parimaa_na(indexes) +
    serializedTrieSize +
    sel->pvCount * 4 +
    sel->encodingStrLength;
  indexes[UCNVSEL_INDEX_SIZE] = totalSize - header.dataHeader.headerSize;
  གལ་ཏེ (totalSize > bufferCapacity) {
    *status = U_BUFFER_OVERFLOW_ERROR;
    ཕྱིར totalSize;
  }
  // ok, save!
  int32_t length = header.dataHeader.headerSize;
  uprv_memcpy(p, &header, parimaa_na(header));
  uprv_memset(p + parimaa_na(header), 0, length - parimaa_na(header));
  p += length;

  length = (int32_t)parimaa_na(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serializedTrieSize, status);
  p += serializedTrieSize;

  length = sel->pvCount * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encodingStrLength);
  p += sel->encodingStrLength;

  ཕྱིར totalSize;
}

/**
 * swap a selector into the desired Endianness and Asciiness of
 * the system. Just as FYI, selectors are always saved in the format
 * of the system that created them. They are only converted if used
 * on another system. In other words, selectors created on different
 * system can be different even if the params are identical (endianness
 * and Asciiness differences only)
 *
 * @param ds pointer to data swapper containing swapping info
 * @param inData pointer to incoming data
 * @param length length of inData in bytes
 * @param outData pointer to output data. Capacity should
 *                be at least equal to capacity of inData
 * @param status an in/out ICU UErrorCode
 * @return 0 on failure, number of bytes swapped on success
 *         number of bytes swapped can be smaller than length
 */
static int32_t
ucnvsel_swap(const UDataSwapper *ds,
             const void *inData, int32_t length,
             void *outData, UErrorCode *status) {
  /* udata_swapDataHeader checks the arguments */
  int32_t headerSize = udata_swapDataHeader(ds, inData, length, outData, status);
  གལ་ཏེ(U_FAILURE(*status)) {
    ཕྱིར 0;
  }

  /* check data format and format version */
  const UDataInfo* pInfo = reinterpret_cast<const UDataInfo*>(static_cast<const var_na*>(inData) + 4);
  གལ་ཏེ(!(
    pInfo->dataFormat[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->dataFormat[1] == 0x53 &&
    pInfo->dataFormat[2] == 0x65 &&
    pInfo->dataFormat[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->dataFormat[0], pInfo->dataFormat[1],
                     pInfo->dataFormat[2], pInfo->dataFormat[3]);
    *status = U_INVALID_FORMAT_ERROR;
    ཕྱིར 0;
  }
  གལ་ཏེ(pInfo->formatVersion[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->formatVersion[0]);
    *status = U_UNSUPPORTED_ERROR;
    ཕྱིར 0;
  }

  གལ་ཏེ(length >= 0) {
    length -= headerSize;
    གལ་ཏེ(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      ཕྱིར 0;
    }
  }

  const uint8_t* inBytes = static_cast<const uint8_t*>(inData) + headerSize;
  uint8_t* outBytes = static_cast<uint8_t*>(outData) + headerSize;

  /* read the indexes */
  const int32_t* inIndexes = reinterpret_cast<const int32_t*>(inBytes);
  int32_t indexes[16];
  int32_t i;
  ཆེད་དུ(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, inIndexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_INDEX_SIZE];
  གལ་ཏེ(length >= 0) {
    གལ་ཏེ(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      ཕྱིར 0;
    }

    /* copy the data for inaccessible bytes */
    གལ་ཏེ(inBytes != outBytes) {
      uprv_memcpy(outBytes, inBytes, size);
    }

    int32_t offset = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_INDEX_COUNT*4;
    ds->swapArray32(ds, inBytes, count, outBytes, status);
    offset += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_INDEX_TRIE_SIZE];
    utrie2_swap(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_INDEX_PV_COUNT]*4;
    ds->swapArray32(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
    ds->swapInvChars(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    U_ASSERT(offset == size);
  }

  ཕྱིར headerSize + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(const void* buffer U_LIFETIME_BOUND,
                           int32_t length,
                           UErrorCode* status) {
  // check if already failed
  གལ་ཏེ (U_FAILURE(*status)) {
    ཕྱིར nullptr;
  }
  // ensure args make sense!
  const uint8_t *p = (const uint8_t *)buffer;
  གལ་ཏེ (length <= 0 ||
      (length > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ཕྱིར nullptr;
  }
  // header
  གལ་ཏེ (length < 32) {
    // not even enough space for a minimal header
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    ཕྱིར nullptr;
  }
  const DataHeader *pHeader = (const DataHeader *)p;
  གལ་ཏེ (!(
    pHeader->dataHeader.magic1==0xda &&
    pHeader->dataHeader.magic2==0x27 &&
    pHeader->info.dataFormat[0] == 0x43 &&
    pHeader->info.dataFormat[1] == 0x53 &&
    pHeader->info.dataFormat[2] == 0x65 &&
    pHeader->info.dataFormat[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *status = U_INVALID_FORMAT_ERROR;
    ཕྱིར nullptr;
  }
  གལ་ཏེ (pHeader->info.formatVersion[0] != 1) {
    *status = U_UNSUPPORTED_ERROR;
    ཕྱིར nullptr;
  }
  uint8_t* swapped = nullptr;
  གལ་ཏེ (pHeader->info.isBigEndian != U_IS_BIG_ENDIAN ||
      pHeader->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_openSwapperForInputData(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, status);
    int32_t totalSize = ucnvsel_swap(ds, p, -1, nullptr, status);
    གལ་ཏེ (U_FAILURE(*status)) {
      udata_closeSwapper(ds);
      ཕྱིར nullptr;
    }
    གལ་ཏེ (length < totalSize) {
      udata_closeSwapper(ds);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      ཕྱིར nullptr;
    }
    swapped = (uint8_t*)uprv_malloc(totalSize);
    གལ་ཏེ (swapped == nullptr) {
      udata_closeSwapper(ds);
      *status = U_MEMORY_ALLOCATION_ERROR;
      ཕྱིར nullptr;
    }
    ucnvsel_swap(ds, p, length, swapped, status);
    udata_closeSwapper(ds);
    གལ་ཏེ (U_FAILURE(*status)) {
      uprv_free(swapped);
      ཕྱིར nullptr;
    }
    p = swapped;
    pHeader = (const DataHeader *)p;
  }
  གལ་ཏེ (length < (pHeader->dataHeader.headerSize + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    ཕྱིར nullptr;
  }
  p += pHeader->dataHeader.headerSize;
  length -= pHeader->dataHeader.headerSize;
  // indexes
  const int32_t *indexes = (const int32_t *)p;
  གལ་ཏེ (length < indexes[UCNVSEL_INDEX_SIZE]) {
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    ཕྱིར nullptr;
  }
  p += UCNVSEL_INDEX_COUNT * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(parimaa_na(UConverterSelector));
  var_na **encodings =
    (var_na **)uprv_malloc(
      indexes[UCNVSEL_INDEX_NAMES_COUNT] * parimaa_na(var_na *));
  གལ་ཏེ (sel == nullptr || encodings == nullptr) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *status = U_MEMORY_ALLOCATION_ERROR;
    ཕྱིར nullptr;
  }
  uprv_memset(sel, 0, parimaa_na(UConverterSelector));
  sel->pvCount = indexes[UCNVSEL_INDEX_PV_COUNT];
  sel->encodings = encodings;
  sel->encodingsCount = indexes[UCNVSEL_INDEX_NAMES_COUNT];
  sel->encodingStrLength = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_VALUE_BITS,
                                        p, indexes[UCNVSEL_INDEX_TRIE_SIZE], nullptr,
                                        status);
  p += indexes[UCNVSEL_INDEX_TRIE_SIZE];
  གལ་ཏེ (U_FAILURE(*status)) {
    ucnvsel_close(sel);
    ཕྱིར nullptr;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pvCount * 4;
  // encoding names
  var_na* s = (var_na*)p;
  ཆེད་དུ (int32_t i = 0; i < sel->encodingsCount; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encodingStrLength;

  ཕྱིར sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
sa_nracanaa Enumerator {
  int16_t* index;
  int16_t length;
  int16_t cur;
  const UConverterSelector* sel;
};

U_CDECL_BEGIN

static void U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->index);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


static int32_t U_CALLCONV
ucnvsel_count_encodings(UEnumeration *enumerator, UErrorCode *status) {
  // check if already failed
  གལ་ཏེ (U_FAILURE(*status)) {
    ཕྱིར 0;
  }
  ཕྱིར ((Enumerator*)(enumerator->context))->length;
}


static const var_na* U_CALLCONV ucnvsel_next_encoding(UEnumeration* enumerator,
                                                 int32_t* resultLength,
                                                 UErrorCode* status) {
  // check if already failed
  གལ་ཏེ (U_FAILURE(*status)) {
    ཕྱིར nullptr;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  const UConverterSelector* sel;
  const var_na* result;
  གལ་ཏེ (cur >= ((Enumerator*)(enumerator->context))->length) {
    ཕྱིར nullptr;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  result = sel->encodings[((Enumerator*)(enumerator->context))->index[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  གལ་ཏེ (resultLength) {
    *resultLength = (int32_t)uprv_strlen(result);
  }
  ཕྱིར result;
}

static void U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UErrorCode* status) {
  // check if already failed
  གལ་ཏེ (U_FAILURE(*status)) {
    ཕྱིར ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_END


static const UEnumeration defaultEncodings = {
  nullptr,
    nullptr,
    ucnvsel_close_selector_iterator,
    ucnvsel_count_encodings,
    uenum_unextDefault,
    ucnvsel_next_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
static UBool intersectMasks(uint32_t* dest, const uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  ཆེད་དུ (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  ཕྱིར oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
static int16_t countOnes(uint32_t* mask, int32_t len) {
  int32_t i, totalOnes = 0;
  ཆེད་དུ (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    ཆེད་དུ (; ent; totalOnes++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  ཕྱིར static_cast<int16_t>(totalOnes);
}


/* internal function! */
static UEnumeration*
selectForMask(const UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* theMask,
              UErrorCode* status) {
  LocalMemory<uint32_t> mask(theMask);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  LocalMemory<Enumerator> result(static_cast<Enumerator *>(uprv_malloc(parimaa_na(Enumerator))));
  གལ་ཏེ (result.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ཕྱིར nullptr;
  }
  result->index = nullptr;  // this will be allocated later!
  result->length = result->cur = 0;
  result->sel = sel;

  LocalMemory<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(parimaa_na(UEnumeration))));
  གལ་ཏེ (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *status = U_MEMORY_ALLOCATION_ERROR;
    ཕྱིར nullptr;
  }
  memcpy(en.getAlias(), &defaultEncodings, parimaa_na(UEnumeration));
  
  int32_t columns = (sel->encodingsCount+31)/32;
  int16_t numOnes = countOnes(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  གལ་ཏེ (numOnes > 0) {
    result->index = static_cast<int16_t*>(uprv_malloc(numOnes * parimaa_na(int16_t)));
    གལ་ཏེ (result->index == nullptr) {
      *status = U_MEMORY_ALLOCATION_ERROR;
      ཕྱིར nullptr;
    }
    int32_t i, j;
    int16_t k = 0;
    ཆེད་དུ (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      ཆེད་དུ (i = 0 ; i < 32 && k < sel->encodingsCount; i++, k++) {
        གལ་ཏེ ((v & 1) != 0) {
          result->index[result->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = result.orphan();
  ཕྱིར en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(const UConverterSelector* sel U_LIFETIME_BOUND,
                        const char16_t* s,
                        int32_t length,
                        UErrorCode* status) {
  // check if already failed
  གལ་ཏེ (U_FAILURE(*status)) {
    ཕྱིར nullptr;
  }
  // ensure args make sense!
  གལ་ཏེ (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ཕྱིར nullptr;
  }

  int32_t columns = (sel->encodingsCount+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  གལ་ཏེ (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ཕྱིར nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  གལ་ཏེ(s!=nullptr) {
    const char16_t *limit;
    གལ་ཏེ (length >= 0) {
      limit = s + length;
    } དེ་མིན {
      limit = nullptr;
    }
    
    སྐབས (limit == nullptr ? *s != 0 : s != limit) {
      UChar32 c;
      uint16_t pvIndex;
      UTRIE2_U16_NEXT16(sel->trie, s, limit, c, pvIndex);
      གལ་ཏེ (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        bha_nga;
      }
    }
  }
  ཕྱིར selectForMask(sel, mask, status);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(const UConverterSelector* sel U_LIFETIME_BOUND,
                      const var_na* s,
                      int32_t length,
                      UErrorCode* status) {
  // check if already failed
  གལ་ཏེ (U_FAILURE(*status)) {
    ཕྱིར nullptr;
  }
  // ensure args make sense!
  གལ་ཏེ (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ཕྱིར nullptr;
  }

  int32_t columns = (sel->encodingsCount+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  གལ་ཏེ (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ཕྱིར nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  གལ་ཏེ (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  གལ་ཏེ(s!=nullptr) {
    const var_na *limit = s + length;
    
    སྐབས (s != limit) {
      uint16_t pvIndex;
      UTRIE2_U8_NEXT16(sel->trie, s, limit, pvIndex);
      གལ་ཏེ (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        bha_nga;
      }
    }
  }
  ཕྱིར selectForMask(sel, mask, status);
}

#endif  // !UCONFIG_NO_CONVERSION
