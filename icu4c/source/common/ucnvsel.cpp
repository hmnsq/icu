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

#ಅನ್ತರ್ಭಾವ "unicode/ucnvsel.h"

#ಯದಿ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchriter.h"
#ಅನ್ತರ್ಭಾವ "utrie2.h"
#ಅನ್ತರ್ಭಾವ "propsvec.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucmndata.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"

U_NAMESPACE_USE

ಸಂರಚನಾ UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_ಎಣಿಕೆ;
  ವರ್ಣ** encodings;          // which encodings did user ask to use?
  int32_t encodings_ಎಣಿಕೆ;
  int32_t encodingStrLength;
  uint8_t* swapped;
  UBool ownPv, ownEncodingStrings;
};

ಸ್ಥಿರ ಶೂನ್ಯ generateSelectorData(UConverterSelector* result,
                                 UPropsVectors *upvec,
                                 ಧ್ರುವ USet* excludedCodePoints,
                                 ಧ್ರುವ UConverterUnicodeSet whichSet,
                                 UErrorCode* status) {
  ಯದಿ (U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ;
  }

  int32_t columns = (result->encodings_ಎಣಿಕೆ+31)/32;

  // set errorValue to all-ones
  ಪ್ರತಿ (int32_t col = 0; col < columns; col++) {
    upvec_set_ಮೌಲ್ಯ(upvec, UPVEC_ERROR_ಮೌಲ್ಯ_CP, UPVEC_ERROR_ಮೌಲ್ಯ_CP,
                   col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), status);
  }

  ಪ್ರತಿ (int32_t i = 0; i < result->encodings_ಎಣಿಕೆ; ++i) {
    uint32_t mask;
    uint32_t column;
    int32_t item_ಎಣಿಕೆ;
    int32_t j;
    UConverter* test_converter = ucnv_open(result->encodings[i], status);
    ಯದಿ (U_FAILURE(*status)) {
      ಪ್ರತ್ಯಾಗಮ;
    }
    USet* unicode_point_set;
    unicode_point_set = uset_open(1, 0);  // empty set

    ucnv_getUnicodeSet(test_converter, unicode_point_set,
                       whichSet, status);
    ಯದಿ (U_FAILURE(*status)) {
      ucnv_close(test_converter);
      ಪ್ರತ್ಯಾಗಮ;
    }

    column = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    item_ಎಣಿಕೆ = uset_get_Item_ಎಣಿಕೆ(unicode_point_set);

    ಪ್ರತಿ (j = 0; j < item_ಎಣಿಕೆ; ++j) {
      UChar32 start_char;
      UChar32 end_char;
      UErrorCode smallStatus = U_ZERO_ERROR;
      uset_getItem(unicode_point_set, j, &start_char, &end_char, nullptr, 0,
                   &smallStatus);
      ಯದಿ (U_FAILURE(smallStatus)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } ಅನ್ಯಥಾ {
        upvec_set_ಮೌಲ್ಯ(upvec, start_char, end_char, column, static_cast<uint32_t>(~0), mask,
                       status);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_point_set);
    ಯದಿ (U_FAILURE(*status)) {
      ಪ್ರತ್ಯಾಗಮ;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  ಯದಿ (excludedCodePoints) {
    int32_t item_ಎಣಿಕೆ = uset_get_Item_ಎಣಿಕೆ(excludedCodePoints);
    ಪ್ರತಿ (int32_t j = 0; j < item_ಎಣಿಕೆ; ++j) {
      UChar32 start_char;
      UChar32 end_char;

      uset_getItem(excludedCodePoints, j, &start_char, &end_char, nullptr, 0,
                   status);
      ಪ್ರತಿ (int32_t col = 0; col < columns; col++) {
        upvec_set_ಮೌಲ್ಯ(upvec, start_char, end_char, col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      status);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  result->trie = upvec_compactToUTrie2WithRowIndexes(upvec, status);
  result->pv = upvec_cloneArray(upvec, &result->pv_ಎಣಿಕೆ, nullptr, status);
  result->pv_ಎಣಿಕೆ *= columns;  // number of uint32_t = rows * columns
  result->ownPv = ಸತ್ಯ;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ*  converterList, int32_t converterListSize,
             ಧ್ರುವ USet* excludedCodePoints,
             ಧ್ರುವ UConverterUnicodeSet whichSet, UErrorCode* status) {
  // check if already failed
  ಯದಿ (U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  // ensure args make sense!
  ಯದಿ (converterListSize < 0 || (converterList == nullptr && converterListSize != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  // allocate a new converter
  LocalUConverterSelectorPointer newSelector(
    (UConverterSelector*)uprv_malloc(ಪರಿಮಾಣ(UConverterSelector)));
  ಯದಿ (newSelector.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  uprv_memset(newSelector.getAlias(), 0, ಪರಿಮಾಣ(UConverterSelector));

  ಯದಿ (converterListSize == 0) {
    converterList = nullptr;
    converterListSize = ucnv_ಎಣಿಕೆ_Available();
  }
  newSelector->encodings =
    (ವರ್ಣ**)uprv_malloc(converterListSize * ಪರಿಮಾಣ(ವರ್ಣ*));
  ಯದಿ (!newSelector->encodings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  newSelector->encodings[0] = nullptr;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t totalSize = 0;
  int32_t i;
  ಪ್ರತಿ (i = 0; i < converterListSize; i++) {
    totalSize +=
      (int32_t)uprv_strlen(converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = totalSize & 3;
  ಯದಿ (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encodingStrLength = totalSize += encodingStrPadding;
  ವರ್ಣ* allStrings = (ವರ್ಣ*) uprv_malloc(totalSize);
  ಯದಿ (!allStrings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  ಪ್ರತಿ (i = 0; i < converterListSize; i++) {
    newSelector->encodings[i] = allStrings;
    uprv_strcpy(newSelector->encodings[i],
                converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i));
    allStrings += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  ಯಾವತ್ (encodingStrPadding > 0) {
    *allStrings++ = 0;
    --encodingStrPadding;
  }

  newSelector->ownEncodingStrings = ಸತ್ಯ;
  newSelector->encodings_ಎಣಿಕೆ = converterListSize;
  UPropsVectors *upvec = upvec_open((converterListSize+31)/32, status);
  generateSelectorData(newSelector.getAlias(), upvec, excludedCodePoints, whichSet, status);
  upvec_close(upvec);

  ಯದಿ (U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  ಪ್ರತ್ಯಾಗಮ newSelector.orphan();
}

/* close opened selector */
U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  ಯದಿ (!sel) {
    ಪ್ರತ್ಯಾಗಮ;
  }
  ಯದಿ (sel->ownEncodingStrings) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  ಯದಿ (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

ಸ್ಥಿರ ಧ್ರುವ UDataInfo dataInfo = {
  ಪರಿಮಾಣ(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

ಗಣನಾ {
  UCNVSEL_INDEX_TRIE_SIZE,      // trie size in bytes
  UCNVSEL_INDEX_PV_ಎಣಿಕೆ,       // number of uint32_t in the bit vectors
  UCNVSEL_INDEX_NAMES_ಎಣಿಕೆ,    // number of encoding names
  UCNVSEL_INDEX_NAMES_LENGTH,   // number of encoding name bytes including padding
  UCNVSEL_INDEX_SIZE = 15,      // bytes following the DataHeader
  UCNVSEL_INDEX_ಎಣಿಕೆ = 16
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
ucnvsel_serialize(ಧ್ರುವ UConverterSelector* sel,
                  ಶೂನ್ಯ* buffer, int32_t bufferCapacity, UErrorCode* status) {
  // check if already failed
  ಯದಿ (U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)buffer;
  ಯದಿ (bufferCapacity < 0 ||
      (bufferCapacity > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  // add up the size of the serialized form
  int32_t serializedTrieSize = utrie2_serialize(sel->trie, nullptr, 0, status);
  ಯದಿ (*status != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  *status = U_ZERO_ERROR;

  DataHeader header;
  uprv_memset(&header, 0, ಪರಿಮಾಣ(header));
  header.dataHeader.headerSize = (uint16_t)((ಪರಿಮಾಣ(header) + 15) & ~15);
  header.dataHeader.magic1 = 0xda;
  header.dataHeader.magic2 = 0x27;
  uprv_memcpy(&header.info, &dataInfo, ಪರಿಮಾಣ(dataInfo));

  int32_t indexes[UCNVSEL_INDEX_ಎಣಿಕೆ] = {
    serializedTrieSize,
    sel->pv_ಎಣಿಕೆ,
    sel->encodings_ಎಣಿಕೆ,
    sel->encodingStrLength
  };

  int32_t totalSize =
    header.dataHeader.headerSize +
    (int32_t)ಪರಿಮಾಣ(indexes) +
    serializedTrieSize +
    sel->pv_ಎಣಿಕೆ * 4 +
    sel->encodingStrLength;
  indexes[UCNVSEL_INDEX_SIZE] = totalSize - header.dataHeader.headerSize;
  ಯದಿ (totalSize > bufferCapacity) {
    *status = U_BUFFER_OVERFLOW_ERROR;
    ಪ್ರತ್ಯಾಗಮ totalSize;
  }
  // ok, save!
  int32_t length = header.dataHeader.headerSize;
  uprv_memcpy(p, &header, ಪರಿಮಾಣ(header));
  uprv_memset(p + ಪರಿಮಾಣ(header), 0, length - ಪರಿಮಾಣ(header));
  p += length;

  length = (int32_t)ಪರಿಮಾಣ(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serializedTrieSize, status);
  p += serializedTrieSize;

  length = sel->pv_ಎಣಿಕೆ * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encodingStrLength);
  p += sel->encodingStrLength;

  ಪ್ರತ್ಯಾಗಮ totalSize;
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
ಸ್ಥಿರ int32_t
ucnvsel_swap(ಧ್ರುವ UDataSwapper *ds,
             ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length,
             ಶೂನ್ಯ *outData, UErrorCode *status) {
  /* udata_swapDataHeader checks the arguments */
  int32_t headerSize = udata_swapDataHeader(ds, inData, length, outData, status);
  ಯದಿ(U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  /* check data format and format version */
  ಧ್ರುವ UDataInfo* pInfo = reinterpret_cast<ಧ್ರುವ UDataInfo*>(static_cast<ಧ್ರುವ ವರ್ಣ*>(inData) + 4);
  ಯದಿ(!(
    pInfo->dataFormat[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->dataFormat[1] == 0x53 &&
    pInfo->dataFormat[2] == 0x65 &&
    pInfo->dataFormat[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->dataFormat[0], pInfo->dataFormat[1],
                     pInfo->dataFormat[2], pInfo->dataFormat[3]);
    *status = U_INVALID_FORMAT_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ(pInfo->formatVersion[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->formatVersion[0]);
    *status = U_UNSUPPORTED_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(length >= 0) {
    length -= headerSize;
    ಯದಿ(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      ಪ್ರತ್ಯಾಗಮ 0;
    }
  }

  ಧ್ರುವ uint8_t* inBytes = static_cast<ಧ್ರುವ uint8_t*>(inData) + headerSize;
  uint8_t* outBytes = static_cast<uint8_t*>(outData) + headerSize;

  /* read the indexes */
  ಧ್ರುವ int32_t* inIndexes = reinterpret_cast<ಧ್ರುವ int32_t*>(inBytes);
  int32_t indexes[16];
  int32_t i;
  ಪ್ರತಿ(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, inIndexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_INDEX_SIZE];
  ಯದಿ(length >= 0) {
    ಯದಿ(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* copy the data for inaccessible bytes */
    ಯದಿ(inBytes != outBytes) {
      uprv_memcpy(outBytes, inBytes, size);
    }

    int32_t offset = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_INDEX_ಎಣಿಕೆ*4;
    ds->swapArray32(ds, inBytes, count, outBytes, status);
    offset += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_INDEX_TRIE_SIZE];
    utrie2_swap(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_INDEX_PV_ಎಣಿಕೆ]*4;
    ds->swapArray32(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
    ds->swapInvChars(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    U_ASSERT(offset == size);
  }

  ಪ್ರತ್ಯಾಗಮ headerSize + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ಧ್ರುವ ಶೂನ್ಯ* buffer U_LIFETIME_BOUND,
                           int32_t length,
                           UErrorCode* status) {
  // check if already failed
  ಯದಿ (U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  // ensure args make sense!
  ಧ್ರುವ uint8_t *p = (ಧ್ರುವ uint8_t *)buffer;
  ಯದಿ (length <= 0 ||
      (length > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  // header
  ಯದಿ (length < 32) {
    // not even enough space for a minimal header
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  ಧ್ರುವ DataHeader *pHeader = (ಧ್ರುವ DataHeader *)p;
  ಯದಿ (!(
    pHeader->dataHeader.magic1==0xda &&
    pHeader->dataHeader.magic2==0x27 &&
    pHeader->info.dataFormat[0] == 0x43 &&
    pHeader->info.dataFormat[1] == 0x53 &&
    pHeader->info.dataFormat[2] == 0x65 &&
    pHeader->info.dataFormat[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *status = U_INVALID_FORMAT_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  ಯದಿ (pHeader->info.formatVersion[0] != 1) {
    *status = U_UNSUPPORTED_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  uint8_t* swapped = nullptr;
  ಯದಿ (pHeader->info.isBigEndian != U_IS_BIG_ENDIAN ||
      pHeader->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_ಒಳಹರಿವು_Data(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, status);
    int32_t totalSize = ucnvsel_swap(ds, p, -1, nullptr, status);
    ಯದಿ (U_FAILURE(*status)) {
      udata_closeSwapper(ds);
      ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (length < totalSize) {
      udata_closeSwapper(ds);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    swapped = (uint8_t*)uprv_malloc(totalSize);
    ಯದಿ (swapped == nullptr) {
      udata_closeSwapper(ds);
      *status = U_MEMORY_ALLOCATION_ERROR;
      ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ucnvsel_swap(ds, p, length, swapped, status);
    udata_closeSwapper(ds);
    ಯದಿ (U_FAILURE(*status)) {
      uprv_free(swapped);
      ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    p = swapped;
    pHeader = (ಧ್ರುವ DataHeader *)p;
  }
  ಯದಿ (length < (pHeader->dataHeader.headerSize + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  p += pHeader->dataHeader.headerSize;
  length -= pHeader->dataHeader.headerSize;
  // indexes
  ಧ್ರುವ int32_t *indexes = (ಧ್ರುವ int32_t *)p;
  ಯದಿ (length < indexes[UCNVSEL_INDEX_SIZE]) {
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  p += UCNVSEL_INDEX_ಎಣಿಕೆ * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(ಪರಿಮಾಣ(UConverterSelector));
  ವರ್ಣ **encodings =
    (ವರ್ಣ **)uprv_malloc(
      indexes[UCNVSEL_INDEX_NAMES_ಎಣಿಕೆ] * ಪರಿಮಾಣ(ವರ್ಣ *));
  ಯದಿ (sel == nullptr || encodings == nullptr) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *status = U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  uprv_memset(sel, 0, ಪರಿಮಾಣ(UConverterSelector));
  sel->pv_ಎಣಿಕೆ = indexes[UCNVSEL_INDEX_PV_ಎಣಿಕೆ];
  sel->encodings = encodings;
  sel->encodings_ಎಣಿಕೆ = indexes[UCNVSEL_INDEX_NAMES_ಎಣಿಕೆ];
  sel->encodingStrLength = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_ಮೌಲ್ಯ_BITS,
                                        p, indexes[UCNVSEL_INDEX_TRIE_SIZE], nullptr,
                                        status);
  p += indexes[UCNVSEL_INDEX_TRIE_SIZE];
  ಯದಿ (U_FAILURE(*status)) {
    ucnvsel_close(sel);
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_ಎಣಿಕೆ * 4;
  // encoding names
  ವರ್ಣ* s = (ವರ್ಣ*)p;
  ಪ್ರತಿ (int32_t i = 0; i < sel->encodings_ಎಣಿಕೆ; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encodingStrLength;

  ಪ್ರತ್ಯಾಗಮ sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
ಸಂರಚನಾ Enumerator {
  int16_t* index;
  int16_t length;
  int16_t cur;
  ಧ್ರುವ UConverterSelector* sel;
};

U_CDECL_BEGIN

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->index);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


ಸ್ಥಿರ int32_t U_CALLCONV
ucnvsel_ಎಣಿಕೆ_encodings(UEnumeration *enumerator, UErrorCode *status) {
  // check if already failed
  ಯದಿ (U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ ((Enumerator*)(enumerator->context))->length;
}


ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* U_CALLCONV ucnvsel_next_encoding(UEnumeration* enumerator,
                                                 int32_t* resultLength,
                                                 UErrorCode* status) {
  // check if already failed
  ಯದಿ (U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ಧ್ರುವ UConverterSelector* sel;
  ಧ್ರುವ ವರ್ಣ* result;
  ಯದಿ (cur >= ((Enumerator*)(enumerator->context))->length) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  result = sel->encodings[((Enumerator*)(enumerator->context))->index[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  ಯದಿ (resultLength) {
    *resultLength = (int32_t)uprv_strlen(result);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UErrorCode* status) {
  // check if already failed
  ಯದಿ (U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_END


ಸ್ಥಿರ ಧ್ರುವ UEnumeration defaultEncodings = {
  nullptr,
    nullptr,
    ucnvsel_close_selector_iterator,
    ucnvsel_ಎಣಿಕೆ_encodings,
    uenum_unextDefault,
    ucnvsel_next_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
ಸ್ಥಿರ UBool intersectMasks(uint32_t* dest, ಧ್ರುವ uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  ಪ್ರತಿ (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  ಪ್ರತ್ಯಾಗಮ oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
ಸ್ಥಿರ int16_t ಎಣಿಕೆ_Ones(uint32_t* mask, int32_t len) {
  int32_t i, totalOnes = 0;
  ಪ್ರತಿ (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    ಪ್ರತಿ (; ent; totalOnes++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  ಪ್ರತ್ಯಾಗಮ static_cast<int16_t>(totalOnes);
}


/* internal function! */
ಸ್ಥಿರ UEnumeration*
selectForMask(ಧ್ರುವ UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* theMask,
              UErrorCode* status) {
  LocalMemory<uint32_t> mask(theMask);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  LocalMemory<Enumerator> result(static_cast<Enumerator *>(uprv_malloc(ಪರಿಮಾಣ(Enumerator))));
  ಯದಿ (result.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  result->index = nullptr;  // this will be allocated later!
  result->length = result->cur = 0;
  result->sel = sel;

  LocalMemory<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(ಪರಿಮಾಣ(UEnumeration))));
  ಯದಿ (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *status = U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  memcpy(en.getAlias(), &defaultEncodings, ಪರಿಮಾಣ(UEnumeration));
  
  int32_t columns = (sel->encodings_ಎಣಿಕೆ+31)/32;
  int16_t ಅಂಕೆ_Ones = ಎಣಿಕೆ_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  ಯದಿ (ಅಂಕೆ_Ones > 0) {
    result->index = static_cast<int16_t*>(uprv_malloc(ಅಂಕೆ_Ones * ಪರಿಮಾಣ(int16_t)));
    ಯದಿ (result->index == nullptr) {
      *status = U_MEMORY_ALLOCATION_ERROR;
      ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    int32_t i, j;
    int16_t k = 0;
    ಪ್ರತಿ (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      ಪ್ರತಿ (i = 0 ; i < 32 && k < sel->encodings_ಎಣಿಕೆ; i++, k++) {
        ಯದಿ ((v & 1) != 0) {
          result->index[result->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = result.orphan();
  ಪ್ರತ್ಯಾಗಮ en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ಧ್ರುವ UConverterSelector* sel U_LIFETIME_BOUND,
                        ಧ್ರುವ char16_t* s,
                        int32_t length,
                        UErrorCode* status) {
  // check if already failed
  ಯದಿ (U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  // ensure args make sense!
  ಯದಿ (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  int32_t columns = (sel->encodings_ಎಣಿಕೆ+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  ಯದಿ (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  ಯದಿ(s!=nullptr) {
    ಧ್ರುವ char16_t *limit;
    ಯದಿ (length >= 0) {
      limit = s + length;
    } ಅನ್ಯಥಾ {
      limit = nullptr;
    }
    
    ಯಾವತ್ (limit == nullptr ? *s != 0 : s != limit) {
      UChar32 c;
      uint16_t pvIndex;
      UTRIE2_U16_NEXT16(sel->trie, s, limit, c, pvIndex);
      ಯದಿ (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        ಭಙ್ಗ;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ selectForMask(sel, mask, status);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ಧ್ರುವ UConverterSelector* sel U_LIFETIME_BOUND,
                      ಧ್ರುವ ವರ್ಣ* s,
                      int32_t length,
                      UErrorCode* status) {
  // check if already failed
  ಯದಿ (U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  // ensure args make sense!
  ಯದಿ (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  int32_t columns = (sel->encodings_ಎಣಿಕೆ+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  ಯದಿ (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  ಯದಿ (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  ಯದಿ(s!=nullptr) {
    ಧ್ರುವ ವರ್ಣ *limit = s + length;
    
    ಯಾವತ್ (s != limit) {
      uint16_t pvIndex;
      UTRIE2_U8_NEXT16(sel->trie, s, limit, pvIndex);
      ಯದಿ (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        ಭಙ್ಗ;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ selectForMask(sel, mask, status);
}

#endif  // !UCONFIG_NO_CONVERSION
