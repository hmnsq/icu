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

#അന്തര്ഭാവ "unicode/ucnvsel.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/uchriter.h"
#അന്തര്ഭാവ "utrie2.h"
#അന്തര്ഭാവ "propsvec.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucmndata.h"
#അന്തര്ഭാവ "udataswp.h"
#അന്തര്ഭാവ "uenumimp.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"

U_NAMESPACE_USE

ഘടന UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_എണ്ണം;
  അക്ഷരം** encodings;          // which encodings did user ask to use?
  int32_t encodings_എണ്ണം;
  int32_t encodingStrLength;
  uint8_t* swapped;
  UBool ownPv, ownEncodingStrings;
};

സ്ഥിതം ശൂന്യം generateSelectorData(UConverterSelector* result,
                                 UPropsVectors *upvec,
                                 സ്ഥിരം USet* excludedCodePoints,
                                 സ്ഥിരം UConverterUnicodeSet whichSet,
                                 UErrorCode* status) {
  എങ്കിൽ (U_FAILURE(*status)) {
    തിരികെ;
  }

  int32_t columns = (result->encodings_എണ്ണം+31)/32;

  // set errorValue to all-ones
  വേണ്ടി (int32_t col = 0; col < columns; col++) {
    upvec_set_മൂല്യം(upvec, UPVEC_ERROR_മൂല്യം_CP, UPVEC_ERROR_മൂല്യം_CP,
                   col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), status);
  }

  വേണ്ടി (int32_t i = 0; i < result->encodings_എണ്ണം; ++i) {
    uint32_t mask;
    uint32_t column;
    int32_t item_എണ്ണം;
    int32_t j;
    UConverter* test_converter = ucnv_open(result->encodings[i], status);
    എങ്കിൽ (U_FAILURE(*status)) {
      തിരികെ;
    }
    USet* unicode_point_set;
    unicode_point_set = uset_open(1, 0);  // empty set

    ucnv_getUnicodeSet(test_converter, unicode_point_set,
                       whichSet, status);
    എങ്കിൽ (U_FAILURE(*status)) {
      ucnv_close(test_converter);
      തിരികെ;
    }

    column = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    item_എണ്ണം = uset_get_Item_എണ്ണം(unicode_point_set);

    വേണ്ടി (j = 0; j < item_എണ്ണം; ++j) {
      UChar32 start_char;
      UChar32 end_char;
      UErrorCode smallStatus = U_ZERO_ERROR;
      uset_getItem(unicode_point_set, j, &start_char, &end_char, nullptr, 0,
                   &smallStatus);
      എങ്കിൽ (U_FAILURE(smallStatus)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } അല്ലെങ്കിൽ {
        upvec_set_മൂല്യം(upvec, start_char, end_char, column, static_cast<uint32_t>(~0), mask,
                       status);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_point_set);
    എങ്കിൽ (U_FAILURE(*status)) {
      തിരികെ;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  എങ്കിൽ (excludedCodePoints) {
    int32_t item_എണ്ണം = uset_get_Item_എണ്ണം(excludedCodePoints);
    വേണ്ടി (int32_t j = 0; j < item_എണ്ണം; ++j) {
      UChar32 start_char;
      UChar32 end_char;

      uset_getItem(excludedCodePoints, j, &start_char, &end_char, nullptr, 0,
                   status);
      വേണ്ടി (int32_t col = 0; col < columns; col++) {
        upvec_set_മൂല്യം(upvec, start_char, end_char, col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      status);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  result->trie = upvec_compactToUTrie2WithRowIndexes(upvec, status);
  result->pv = upvec_cloneArray(upvec, &result->pv_എണ്ണം, nullptr, status);
  result->pv_എണ്ണം *= columns;  // number of uint32_t = rows * columns
  result->ownPv = സത്യം;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(സ്ഥിരം അക്ഷരം* സ്ഥിരം*  converterList, int32_t converterListSize,
             സ്ഥിരം USet* excludedCodePoints,
             സ്ഥിരം UConverterUnicodeSet whichSet, UErrorCode* status) {
  // check if already failed
  എങ്കിൽ (U_FAILURE(*status)) {
    തിരികെ nullptr;
  }
  // ensure args make sense!
  എങ്കിൽ (converterListSize < 0 || (converterList == nullptr && converterListSize != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ nullptr;
  }

  // allocate a new converter
  LocalUConverterSelectorPointer newSelector(
    (UConverterSelector*)uprv_malloc(വലുപ്പം(UConverterSelector)));
  എങ്കിൽ (newSelector.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    തിരികെ nullptr;
  }
  uprv_memset(newSelector.getAlias(), 0, വലുപ്പം(UConverterSelector));

  എങ്കിൽ (converterListSize == 0) {
    converterList = nullptr;
    converterListSize = ucnv_എണ്ണം_Available();
  }
  newSelector->encodings =
    (അക്ഷരം**)uprv_malloc(converterListSize * വലുപ്പം(അക്ഷരം*));
  എങ്കിൽ (!newSelector->encodings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    തിരികെ nullptr;
  }
  newSelector->encodings[0] = nullptr;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t totalSize = 0;
  int32_t i;
  വേണ്ടി (i = 0; i < converterListSize; i++) {
    totalSize +=
      (int32_t)uprv_strlen(converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = totalSize & 3;
  എങ്കിൽ (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encodingStrLength = totalSize += encodingStrPadding;
  അക്ഷരം* allStrings = (അക്ഷരം*) uprv_malloc(totalSize);
  എങ്കിൽ (!allStrings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    തിരികെ nullptr;
  }

  വേണ്ടി (i = 0; i < converterListSize; i++) {
    newSelector->encodings[i] = allStrings;
    uprv_strcpy(newSelector->encodings[i],
                converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i));
    allStrings += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  ആകുമ്പോൾ (encodingStrPadding > 0) {
    *allStrings++ = 0;
    --encodingStrPadding;
  }

  newSelector->ownEncodingStrings = സത്യം;
  newSelector->encodings_എണ്ണം = converterListSize;
  UPropsVectors *upvec = upvec_open((converterListSize+31)/32, status);
  generateSelectorData(newSelector.getAlias(), upvec, excludedCodePoints, whichSet, status);
  upvec_close(upvec);

  എങ്കിൽ (U_FAILURE(*status)) {
    തിരികെ nullptr;
  }

  തിരികെ newSelector.orphan();
}

/* close opened selector */
U_CAPI ശൂന്യം U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  എങ്കിൽ (!sel) {
    തിരികെ;
  }
  എങ്കിൽ (sel->ownEncodingStrings) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  എങ്കിൽ (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

സ്ഥിതം സ്ഥിരം UDataInfo dataInfo = {
  വലുപ്പം(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

എണ്ണൽ {
  UCNVSEL_INDEX_TRIE_SIZE,      // trie size in bytes
  UCNVSEL_INDEX_PV_എണ്ണം,       // number of uint32_t in the bit vectors
  UCNVSEL_INDEX_NAMES_എണ്ണം,    // number of encoding names
  UCNVSEL_INDEX_NAMES_LENGTH,   // number of encoding name bytes including padding
  UCNVSEL_INDEX_SIZE = 15,      // bytes following the DataHeader
  UCNVSEL_INDEX_എണ്ണം = 16
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
ucnvsel_serialize(സ്ഥിരം UConverterSelector* sel,
                  ശൂന്യം* buffer, int32_t bufferCapacity, UErrorCode* status) {
  // check if already failed
  എങ്കിൽ (U_FAILURE(*status)) {
    തിരികെ 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)buffer;
  എങ്കിൽ (bufferCapacity < 0 ||
      (bufferCapacity > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ 0;
  }
  // add up the size of the serialized form
  int32_t serializedTrieSize = utrie2_serialize(sel->trie, nullptr, 0, status);
  എങ്കിൽ (*status != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(*status)) {
    തിരികെ 0;
  }
  *status = U_ZERO_ERROR;

  DataHeader header;
  uprv_memset(&header, 0, വലുപ്പം(header));
  header.dataHeader.headerSize = (uint16_t)((വലുപ്പം(header) + 15) & ~15);
  header.dataHeader.magic1 = 0xda;
  header.dataHeader.magic2 = 0x27;
  uprv_memcpy(&header.info, &dataInfo, വലുപ്പം(dataInfo));

  int32_t indexes[UCNVSEL_INDEX_എണ്ണം] = {
    serializedTrieSize,
    sel->pv_എണ്ണം,
    sel->encodings_എണ്ണം,
    sel->encodingStrLength
  };

  int32_t totalSize =
    header.dataHeader.headerSize +
    (int32_t)വലുപ്പം(indexes) +
    serializedTrieSize +
    sel->pv_എണ്ണം * 4 +
    sel->encodingStrLength;
  indexes[UCNVSEL_INDEX_SIZE] = totalSize - header.dataHeader.headerSize;
  എങ്കിൽ (totalSize > bufferCapacity) {
    *status = U_BUFFER_OVERFLOW_ERROR;
    തിരികെ totalSize;
  }
  // ok, save!
  int32_t length = header.dataHeader.headerSize;
  uprv_memcpy(p, &header, വലുപ്പം(header));
  uprv_memset(p + വലുപ്പം(header), 0, length - വലുപ്പം(header));
  p += length;

  length = (int32_t)വലുപ്പം(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serializedTrieSize, status);
  p += serializedTrieSize;

  length = sel->pv_എണ്ണം * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encodingStrLength);
  p += sel->encodingStrLength;

  തിരികെ totalSize;
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
സ്ഥിതം int32_t
ucnvsel_swap(സ്ഥിരം UDataSwapper *ds,
             സ്ഥിരം ശൂന്യം *inData, int32_t length,
             ശൂന്യം *outData, UErrorCode *status) {
  /* udata_swapDataHeader checks the arguments */
  int32_t headerSize = udata_swapDataHeader(ds, inData, length, outData, status);
  എങ്കിൽ(U_FAILURE(*status)) {
    തിരികെ 0;
  }

  /* check data format and format version */
  സ്ഥിരം UDataInfo* pInfo = reinterpret_cast<സ്ഥിരം UDataInfo*>(static_cast<സ്ഥിരം അക്ഷരം*>(inData) + 4);
  എങ്കിൽ(!(
    pInfo->dataFormat[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->dataFormat[1] == 0x53 &&
    pInfo->dataFormat[2] == 0x65 &&
    pInfo->dataFormat[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->dataFormat[0], pInfo->dataFormat[1],
                     pInfo->dataFormat[2], pInfo->dataFormat[3]);
    *status = U_INVALID_FORMAT_ERROR;
    തിരികെ 0;
  }
  എങ്കിൽ(pInfo->formatVersion[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->formatVersion[0]);
    *status = U_UNSUPPORTED_ERROR;
    തിരികെ 0;
  }

  എങ്കിൽ(length >= 0) {
    length -= headerSize;
    എങ്കിൽ(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      തിരികെ 0;
    }
  }

  സ്ഥിരം uint8_t* inBytes = static_cast<സ്ഥിരം uint8_t*>(inData) + headerSize;
  uint8_t* outBytes = static_cast<uint8_t*>(outData) + headerSize;

  /* read the indexes */
  സ്ഥിരം int32_t* inIndexes = reinterpret_cast<സ്ഥിരം int32_t*>(inBytes);
  int32_t indexes[16];
  int32_t i;
  വേണ്ടി(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, inIndexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_INDEX_SIZE];
  എങ്കിൽ(length >= 0) {
    എങ്കിൽ(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      തിരികെ 0;
    }

    /* copy the data for inaccessible bytes */
    എങ്കിൽ(inBytes != outBytes) {
      uprv_memcpy(outBytes, inBytes, size);
    }

    int32_t offset = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_INDEX_എണ്ണം*4;
    ds->swapArray32(ds, inBytes, count, outBytes, status);
    offset += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_INDEX_TRIE_SIZE];
    utrie2_swap(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_INDEX_PV_എണ്ണം]*4;
    ds->swapArray32(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
    ds->swapInvChars(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    U_ASSERT(offset == size);
  }

  തിരികെ headerSize + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(സ്ഥിരം ശൂന്യം* buffer U_LIFETIME_BOUND,
                           int32_t length,
                           UErrorCode* status) {
  // check if already failed
  എങ്കിൽ (U_FAILURE(*status)) {
    തിരികെ nullptr;
  }
  // ensure args make sense!
  സ്ഥിരം uint8_t *p = (സ്ഥിരം uint8_t *)buffer;
  എങ്കിൽ (length <= 0 ||
      (length > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ nullptr;
  }
  // header
  എങ്കിൽ (length < 32) {
    // not even enough space for a minimal header
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    തിരികെ nullptr;
  }
  സ്ഥിരം DataHeader *pHeader = (സ്ഥിരം DataHeader *)p;
  എങ്കിൽ (!(
    pHeader->dataHeader.magic1==0xda &&
    pHeader->dataHeader.magic2==0x27 &&
    pHeader->info.dataFormat[0] == 0x43 &&
    pHeader->info.dataFormat[1] == 0x53 &&
    pHeader->info.dataFormat[2] == 0x65 &&
    pHeader->info.dataFormat[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *status = U_INVALID_FORMAT_ERROR;
    തിരികെ nullptr;
  }
  എങ്കിൽ (pHeader->info.formatVersion[0] != 1) {
    *status = U_UNSUPPORTED_ERROR;
    തിരികെ nullptr;
  }
  uint8_t* swapped = nullptr;
  എങ്കിൽ (pHeader->info.isBigEndian != U_IS_BIG_ENDIAN ||
      pHeader->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_നിവേശം_Data(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, status);
    int32_t totalSize = ucnvsel_swap(ds, p, -1, nullptr, status);
    എങ്കിൽ (U_FAILURE(*status)) {
      udata_closeSwapper(ds);
      തിരികെ nullptr;
    }
    എങ്കിൽ (length < totalSize) {
      udata_closeSwapper(ds);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      തിരികെ nullptr;
    }
    swapped = (uint8_t*)uprv_malloc(totalSize);
    എങ്കിൽ (swapped == nullptr) {
      udata_closeSwapper(ds);
      *status = U_MEMORY_ALLOCATION_ERROR;
      തിരികെ nullptr;
    }
    ucnvsel_swap(ds, p, length, swapped, status);
    udata_closeSwapper(ds);
    എങ്കിൽ (U_FAILURE(*status)) {
      uprv_free(swapped);
      തിരികെ nullptr;
    }
    p = swapped;
    pHeader = (സ്ഥിരം DataHeader *)p;
  }
  എങ്കിൽ (length < (pHeader->dataHeader.headerSize + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    തിരികെ nullptr;
  }
  p += pHeader->dataHeader.headerSize;
  length -= pHeader->dataHeader.headerSize;
  // indexes
  സ്ഥിരം int32_t *indexes = (സ്ഥിരം int32_t *)p;
  എങ്കിൽ (length < indexes[UCNVSEL_INDEX_SIZE]) {
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    തിരികെ nullptr;
  }
  p += UCNVSEL_INDEX_എണ്ണം * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(വലുപ്പം(UConverterSelector));
  അക്ഷരം **encodings =
    (അക്ഷരം **)uprv_malloc(
      indexes[UCNVSEL_INDEX_NAMES_എണ്ണം] * വലുപ്പം(അക്ഷരം *));
  എങ്കിൽ (sel == nullptr || encodings == nullptr) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *status = U_MEMORY_ALLOCATION_ERROR;
    തിരികെ nullptr;
  }
  uprv_memset(sel, 0, വലുപ്പം(UConverterSelector));
  sel->pv_എണ്ണം = indexes[UCNVSEL_INDEX_PV_എണ്ണം];
  sel->encodings = encodings;
  sel->encodings_എണ്ണം = indexes[UCNVSEL_INDEX_NAMES_എണ്ണം];
  sel->encodingStrLength = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_മൂല്യം_BITS,
                                        p, indexes[UCNVSEL_INDEX_TRIE_SIZE], nullptr,
                                        status);
  p += indexes[UCNVSEL_INDEX_TRIE_SIZE];
  എങ്കിൽ (U_FAILURE(*status)) {
    ucnvsel_close(sel);
    തിരികെ nullptr;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_എണ്ണം * 4;
  // encoding names
  അക്ഷരം* s = (അക്ഷരം*)p;
  വേണ്ടി (int32_t i = 0; i < sel->encodings_എണ്ണം; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encodingStrLength;

  തിരികെ sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
ഘടന Enumerator {
  int16_t* index;
  int16_t length;
  int16_t cur;
  സ്ഥിരം UConverterSelector* sel;
};

U_CDECL_BEGIN

സ്ഥിതം ശൂന്യം U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->index);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


സ്ഥിതം int32_t U_CALLCONV
ucnvsel_എണ്ണം_encodings(UEnumeration *enumerator, UErrorCode *status) {
  // check if already failed
  എങ്കിൽ (U_FAILURE(*status)) {
    തിരികെ 0;
  }
  തിരികെ ((Enumerator*)(enumerator->context))->length;
}


സ്ഥിതം സ്ഥിരം അക്ഷരം* U_CALLCONV ucnvsel_next_encoding(UEnumeration* enumerator,
                                                 int32_t* resultLength,
                                                 UErrorCode* status) {
  // check if already failed
  എങ്കിൽ (U_FAILURE(*status)) {
    തിരികെ nullptr;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  സ്ഥിരം UConverterSelector* sel;
  സ്ഥിരം അക്ഷരം* result;
  എങ്കിൽ (cur >= ((Enumerator*)(enumerator->context))->length) {
    തിരികെ nullptr;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  result = sel->encodings[((Enumerator*)(enumerator->context))->index[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  എങ്കിൽ (resultLength) {
    *resultLength = (int32_t)uprv_strlen(result);
  }
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UErrorCode* status) {
  // check if already failed
  എങ്കിൽ (U_FAILURE(*status)) {
    തിരികെ ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_END


സ്ഥിതം സ്ഥിരം UEnumeration defaultEncodings = {
  nullptr,
    nullptr,
    ucnvsel_close_selector_iterator,
    ucnvsel_എണ്ണം_encodings,
    uenum_unextDefault,
    ucnvsel_next_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
സ്ഥിതം UBool intersectMasks(uint32_t* dest, സ്ഥിരം uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  വേണ്ടി (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  തിരികെ oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
സ്ഥിതം int16_t എണ്ണം_Ones(uint32_t* mask, int32_t len) {
  int32_t i, totalOnes = 0;
  വേണ്ടി (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    വേണ്ടി (; ent; totalOnes++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  തിരികെ static_cast<int16_t>(totalOnes);
}


/* internal function! */
സ്ഥിതം UEnumeration*
selectForMask(സ്ഥിരം UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* theMask,
              UErrorCode* status) {
  LocalMemory<uint32_t> mask(theMask);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  LocalMemory<Enumerator> result(static_cast<Enumerator *>(uprv_malloc(വലുപ്പം(Enumerator))));
  എങ്കിൽ (result.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    തിരികെ nullptr;
  }
  result->index = nullptr;  // this will be allocated later!
  result->length = result->cur = 0;
  result->sel = sel;

  LocalMemory<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(വലുപ്പം(UEnumeration))));
  എങ്കിൽ (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *status = U_MEMORY_ALLOCATION_ERROR;
    തിരികെ nullptr;
  }
  memcpy(en.getAlias(), &defaultEncodings, വലുപ്പം(UEnumeration));
  
  int32_t columns = (sel->encodings_എണ്ണം+31)/32;
  int16_t അക്കം_Ones = എണ്ണം_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  എങ്കിൽ (അക്കം_Ones > 0) {
    result->index = static_cast<int16_t*>(uprv_malloc(അക്കം_Ones * വലുപ്പം(int16_t)));
    എങ്കിൽ (result->index == nullptr) {
      *status = U_MEMORY_ALLOCATION_ERROR;
      തിരികെ nullptr;
    }
    int32_t i, j;
    int16_t k = 0;
    വേണ്ടി (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      വേണ്ടി (i = 0 ; i < 32 && k < sel->encodings_എണ്ണം; i++, k++) {
        എങ്കിൽ ((v & 1) != 0) {
          result->index[result->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = result.orphan();
  തിരികെ en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(സ്ഥിരം UConverterSelector* sel U_LIFETIME_BOUND,
                        സ്ഥിരം char16_t* s,
                        int32_t length,
                        UErrorCode* status) {
  // check if already failed
  എങ്കിൽ (U_FAILURE(*status)) {
    തിരികെ nullptr;
  }
  // ensure args make sense!
  എങ്കിൽ (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ nullptr;
  }

  int32_t columns = (sel->encodings_എണ്ണം+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  എങ്കിൽ (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    തിരികെ nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  എങ്കിൽ(s!=nullptr) {
    സ്ഥിരം char16_t *limit;
    എങ്കിൽ (length >= 0) {
      limit = s + length;
    } അല്ലെങ്കിൽ {
      limit = nullptr;
    }
    
    ആകുമ്പോൾ (limit == nullptr ? *s != 0 : s != limit) {
      UChar32 c;
      uint16_t pvIndex;
      UTRIE2_U16_NEXT16(sel->trie, s, limit, c, pvIndex);
      എങ്കിൽ (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        നിർത്തുക;
      }
    }
  }
  തിരികെ selectForMask(sel, mask, status);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(സ്ഥിരം UConverterSelector* sel U_LIFETIME_BOUND,
                      സ്ഥിരം അക്ഷരം* s,
                      int32_t length,
                      UErrorCode* status) {
  // check if already failed
  എങ്കിൽ (U_FAILURE(*status)) {
    തിരികെ nullptr;
  }
  // ensure args make sense!
  എങ്കിൽ (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    തിരികെ nullptr;
  }

  int32_t columns = (sel->encodings_എണ്ണം+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  എങ്കിൽ (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    തിരികെ nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  എങ്കിൽ (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  എങ്കിൽ(s!=nullptr) {
    സ്ഥിരം അക്ഷരം *limit = s + length;
    
    ആകുമ്പോൾ (s != limit) {
      uint16_t pvIndex;
      UTRIE2_U8_NEXT16(sel->trie, s, limit, pvIndex);
      എങ്കിൽ (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        നിർത്തുക;
      }
    }
  }
  തിരികെ selectForMask(sel, mask, status);
}

#endif  // !UCONFIG_NO_CONVERSION
