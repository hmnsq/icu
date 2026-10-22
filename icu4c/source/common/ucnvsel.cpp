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

#ଅନ୍ତର୍ଭାଵ "unicode/ucnvsel.h"

#ଯଦି !UCONFIG_NO_CONVERSION

#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchriter.h"
#ଅନ୍ତର୍ଭାଵ "utrie2.h"
#ଅନ୍ତର୍ଭାଵ "propsvec.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucmndata.h"
#ଅନ୍ତର୍ଭାଵ "udataswp.h"
#ଅନ୍ତର୍ଭାଵ "uenumimp.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"

U_NAMESPACE_USE

ସଂରଚନା UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_ଗଣନା;
  ଅକ୍ଷର** encodings;          // which encodings did user ask to use?
  int32_t encodings_ଗଣନା;
  int32_t encodingStrLength;
  uint8_t* swapped;
  UBool ownPv, ownEncodingStrings;
};

ସ୍ଥିର ଶୂନ୍ୟ generateSelectorData(UConverterSelector* result,
                                 UPropsVectors *upvec,
                                 ଅଚଳ USet* excludedCodePoints,
                                 ଅଚଳ UConverterUnicodeSet whichSet,
                                 UErrorCode* status) {
  ଯଦି (U_FAILURE(*status)) {
    ଫେରନ୍ତୁ;
  }

  int32_t columns = (result->encodings_ଗଣନା+31)/32;

  // set errorValue to all-ones
  ପାଇଁ (int32_t col = 0; col < columns; col++) {
    upvec_set_ମୂଲ୍ୟ(upvec, UPVEC_ERROR_ମୂଲ୍ୟ_CP, UPVEC_ERROR_ମୂଲ୍ୟ_CP,
                   col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), status);
  }

  ପାଇଁ (int32_t i = 0; i < result->encodings_ଗଣନା; ++i) {
    uint32_t mask;
    uint32_t column;
    int32_t item_ଗଣନା;
    int32_t j;
    UConverter* test_converter = ucnv_open(result->encodings[i], status);
    ଯଦି (U_FAILURE(*status)) {
      ଫେରନ୍ତୁ;
    }
    USet* unicode_point_set;
    unicode_point_set = uset_open(1, 0);  // empty set

    ucnv_getUnicodeSet(test_converter, unicode_point_set,
                       whichSet, status);
    ଯଦି (U_FAILURE(*status)) {
      ucnv_close(test_converter);
      ଫେରନ୍ତୁ;
    }

    column = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    item_ଗଣନା = uset_get_Item_ଗଣନା(unicode_point_set);

    ପାଇଁ (j = 0; j < item_ଗଣନା; ++j) {
      UChar32 start_char;
      UChar32 end_char;
      UErrorCode smallStatus = U_ZERO_ERROR;
      uset_getItem(unicode_point_set, j, &start_char, &end_char, nullptr, 0,
                   &smallStatus);
      ଯଦି (U_FAILURE(smallStatus)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } ଅନ୍ୟଥା {
        upvec_set_ମୂଲ୍ୟ(upvec, start_char, end_char, column, static_cast<uint32_t>(~0), mask,
                       status);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_point_set);
    ଯଦି (U_FAILURE(*status)) {
      ଫେରନ୍ତୁ;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  ଯଦି (excludedCodePoints) {
    int32_t item_ଗଣନା = uset_get_Item_ଗଣନା(excludedCodePoints);
    ପାଇଁ (int32_t j = 0; j < item_ଗଣନା; ++j) {
      UChar32 start_char;
      UChar32 end_char;

      uset_getItem(excludedCodePoints, j, &start_char, &end_char, nullptr, 0,
                   status);
      ପାଇଁ (int32_t col = 0; col < columns; col++) {
        upvec_set_ମୂଲ୍ୟ(upvec, start_char, end_char, col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      status);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  result->trie = upvec_compactToUTrie2WithRowIndexes(upvec, status);
  result->pv = upvec_cloneArray(upvec, &result->pv_ଗଣନା, nullptr, status);
  result->pv_ଗଣନା *= columns;  // number of uint32_t = rows * columns
  result->ownPv = ସତ୍ୟ;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ଅଚଳ ଅକ୍ଷର* ଅଚଳ*  converterList, int32_t converterListSize,
             ଅଚଳ USet* excludedCodePoints,
             ଅଚଳ UConverterUnicodeSet whichSet, UErrorCode* status) {
  // check if already failed
  ଯଦି (U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }
  // ensure args make sense!
  ଯଦି (converterListSize < 0 || (converterList == nullptr && converterListSize != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }

  // allocate a new converter
  LocalUConverterSelectorPointer newSelector(
    (UConverterSelector*)uprv_malloc(ଆକାର(UConverterSelector)));
  ଯଦି (newSelector.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  uprv_memset(newSelector.getAlias(), 0, ଆକାର(UConverterSelector));

  ଯଦି (converterListSize == 0) {
    converterList = nullptr;
    converterListSize = ucnv_ଗଣନା_Available();
  }
  newSelector->encodings =
    (ଅକ୍ଷର**)uprv_malloc(converterListSize * ଆକାର(ଅକ୍ଷର*));
  ଯଦି (!newSelector->encodings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  newSelector->encodings[0] = nullptr;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t totalSize = 0;
  int32_t i;
  ପାଇଁ (i = 0; i < converterListSize; i++) {
    totalSize +=
      (int32_t)uprv_strlen(converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = totalSize & 3;
  ଯଦି (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encodingStrLength = totalSize += encodingStrPadding;
  ଅକ୍ଷର* allStrings = (ଅକ୍ଷର*) uprv_malloc(totalSize);
  ଯଦି (!allStrings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }

  ପାଇଁ (i = 0; i < converterListSize; i++) {
    newSelector->encodings[i] = allStrings;
    uprv_strcpy(newSelector->encodings[i],
                converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i));
    allStrings += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  ଯେତେବେଳେ (encodingStrPadding > 0) {
    *allStrings++ = 0;
    --encodingStrPadding;
  }

  newSelector->ownEncodingStrings = ସତ୍ୟ;
  newSelector->encodings_ଗଣନା = converterListSize;
  UPropsVectors *upvec = upvec_open((converterListSize+31)/32, status);
  generateSelectorData(newSelector.getAlias(), upvec, excludedCodePoints, whichSet, status);
  upvec_close(upvec);

  ଯଦି (U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }

  ଫେରନ୍ତୁ newSelector.orphan();
}

/* close opened selector */
U_CAPI ଶୂନ୍ୟ U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  ଯଦି (!sel) {
    ଫେରନ୍ତୁ;
  }
  ଯଦି (sel->ownEncodingStrings) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  ଯଦି (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

ସ୍ଥିର ଅଚଳ UDataInfo dataInfo = {
  ଆକାର(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

ଗଣନା {
  UCNVSEL_INDEX_TRIE_SIZE,      // trie size in bytes
  UCNVSEL_INDEX_PV_ଗଣନା,       // number of uint32_t in the bit vectors
  UCNVSEL_INDEX_NAMES_ଗଣନା,    // number of encoding names
  UCNVSEL_INDEX_NAMES_LENGTH,   // number of encoding name bytes including padding
  UCNVSEL_INDEX_SIZE = 15,      // bytes following the DataHeader
  UCNVSEL_INDEX_ଗଣନା = 16
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
ucnvsel_serialize(ଅଚଳ UConverterSelector* sel,
                  ଶୂନ୍ୟ* buffer, int32_t bufferCapacity, UErrorCode* status) {
  // check if already failed
  ଯଦି (U_FAILURE(*status)) {
    ଫେରନ୍ତୁ 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)buffer;
  ଯଦି (bufferCapacity < 0 ||
      (bufferCapacity > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  // add up the size of the serialized form
  int32_t serializedTrieSize = utrie2_serialize(sel->trie, nullptr, 0, status);
  ଯଦି (*status != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(*status)) {
    ଫେରନ୍ତୁ 0;
  }
  *status = U_ZERO_ERROR;

  DataHeader header;
  uprv_memset(&header, 0, ଆକାର(header));
  header.dataHeader.headerSize = (uint16_t)((ଆକାର(header) + 15) & ~15);
  header.dataHeader.magic1 = 0xda;
  header.dataHeader.magic2 = 0x27;
  uprv_memcpy(&header.info, &dataInfo, ଆକାର(dataInfo));

  int32_t indexes[UCNVSEL_INDEX_ଗଣନା] = {
    serializedTrieSize,
    sel->pv_ଗଣନା,
    sel->encodings_ଗଣନା,
    sel->encodingStrLength
  };

  int32_t totalSize =
    header.dataHeader.headerSize +
    (int32_t)ଆକାର(indexes) +
    serializedTrieSize +
    sel->pv_ଗଣନା * 4 +
    sel->encodingStrLength;
  indexes[UCNVSEL_INDEX_SIZE] = totalSize - header.dataHeader.headerSize;
  ଯଦି (totalSize > bufferCapacity) {
    *status = U_BUFFER_OVERFLOW_ERROR;
    ଫେରନ୍ତୁ totalSize;
  }
  // ok, save!
  int32_t length = header.dataHeader.headerSize;
  uprv_memcpy(p, &header, ଆକାର(header));
  uprv_memset(p + ଆକାର(header), 0, length - ଆକାର(header));
  p += length;

  length = (int32_t)ଆକାର(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serializedTrieSize, status);
  p += serializedTrieSize;

  length = sel->pv_ଗଣନା * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encodingStrLength);
  p += sel->encodingStrLength;

  ଫେରନ୍ତୁ totalSize;
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
ସ୍ଥିର int32_t
ucnvsel_swap(ଅଚଳ UDataSwapper *ds,
             ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length,
             ଶୂନ୍ୟ *outData, UErrorCode *status) {
  /* udata_swapDataHeader checks the arguments */
  int32_t headerSize = udata_swapDataHeader(ds, inData, length, outData, status);
  ଯଦି(U_FAILURE(*status)) {
    ଫେରନ୍ତୁ 0;
  }

  /* check data format and format version */
  ଅଚଳ UDataInfo* pInfo = reinterpret_cast<ଅଚଳ UDataInfo*>(static_cast<ଅଚଳ ଅକ୍ଷର*>(inData) + 4);
  ଯଦି(!(
    pInfo->dataFormat[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->dataFormat[1] == 0x53 &&
    pInfo->dataFormat[2] == 0x65 &&
    pInfo->dataFormat[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->dataFormat[0], pInfo->dataFormat[1],
                     pInfo->dataFormat[2], pInfo->dataFormat[3]);
    *status = U_INVALID_FORMAT_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି(pInfo->formatVersion[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->formatVersion[0]);
    *status = U_UNSUPPORTED_ERROR;
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(length >= 0) {
    length -= headerSize;
    ଯଦି(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      ଫେରନ୍ତୁ 0;
    }
  }

  ଅଚଳ uint8_t* inBytes = static_cast<ଅଚଳ uint8_t*>(inData) + headerSize;
  uint8_t* outBytes = static_cast<uint8_t*>(outData) + headerSize;

  /* read the indexes */
  ଅଚଳ int32_t* inIndexes = reinterpret_cast<ଅଚଳ int32_t*>(inBytes);
  int32_t indexes[16];
  int32_t i;
  ପାଇଁ(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, inIndexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_INDEX_SIZE];
  ଯଦି(length >= 0) {
    ଯଦି(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      ଫେରନ୍ତୁ 0;
    }

    /* copy the data for inaccessible bytes */
    ଯଦି(inBytes != outBytes) {
      uprv_memcpy(outBytes, inBytes, size);
    }

    int32_t offset = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_INDEX_ଗଣନା*4;
    ds->swapArray32(ds, inBytes, count, outBytes, status);
    offset += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_INDEX_TRIE_SIZE];
    utrie2_swap(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_INDEX_PV_ଗଣନା]*4;
    ds->swapArray32(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
    ds->swapInvChars(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    U_ASSERT(offset == size);
  }

  ଫେରନ୍ତୁ headerSize + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ଅଚଳ ଶୂନ୍ୟ* buffer U_LIFETIME_BOUND,
                           int32_t length,
                           UErrorCode* status) {
  // check if already failed
  ଯଦି (U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }
  // ensure args make sense!
  ଅଚଳ uint8_t *p = (ଅଚଳ uint8_t *)buffer;
  ଯଦି (length <= 0 ||
      (length > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  // header
  ଯଦି (length < 32) {
    // not even enough space for a minimal header
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  ଅଚଳ DataHeader *pHeader = (ଅଚଳ DataHeader *)p;
  ଯଦି (!(
    pHeader->dataHeader.magic1==0xda &&
    pHeader->dataHeader.magic2==0x27 &&
    pHeader->info.dataFormat[0] == 0x43 &&
    pHeader->info.dataFormat[1] == 0x53 &&
    pHeader->info.dataFormat[2] == 0x65 &&
    pHeader->info.dataFormat[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *status = U_INVALID_FORMAT_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  ଯଦି (pHeader->info.formatVersion[0] != 1) {
    *status = U_UNSUPPORTED_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  uint8_t* swapped = nullptr;
  ଯଦି (pHeader->info.isBigEndian != U_IS_BIG_ENDIAN ||
      pHeader->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_ନିବେଶ_Data(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, status);
    int32_t totalSize = ucnvsel_swap(ds, p, -1, nullptr, status);
    ଯଦି (U_FAILURE(*status)) {
      udata_closeSwapper(ds);
      ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (length < totalSize) {
      udata_closeSwapper(ds);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      ଫେରନ୍ତୁ nullptr;
    }
    swapped = (uint8_t*)uprv_malloc(totalSize);
    ଯଦି (swapped == nullptr) {
      udata_closeSwapper(ds);
      *status = U_MEMORY_ALLOCATION_ERROR;
      ଫେରନ୍ତୁ nullptr;
    }
    ucnvsel_swap(ds, p, length, swapped, status);
    udata_closeSwapper(ds);
    ଯଦି (U_FAILURE(*status)) {
      uprv_free(swapped);
      ଫେରନ୍ତୁ nullptr;
    }
    p = swapped;
    pHeader = (ଅଚଳ DataHeader *)p;
  }
  ଯଦି (length < (pHeader->dataHeader.headerSize + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  p += pHeader->dataHeader.headerSize;
  length -= pHeader->dataHeader.headerSize;
  // indexes
  ଅଚଳ int32_t *indexes = (ଅଚଳ int32_t *)p;
  ଯଦି (length < indexes[UCNVSEL_INDEX_SIZE]) {
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  p += UCNVSEL_INDEX_ଗଣନା * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(ଆକାର(UConverterSelector));
  ଅକ୍ଷର **encodings =
    (ଅକ୍ଷର **)uprv_malloc(
      indexes[UCNVSEL_INDEX_NAMES_ଗଣନା] * ଆକାର(ଅକ୍ଷର *));
  ଯଦି (sel == nullptr || encodings == nullptr) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *status = U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  uprv_memset(sel, 0, ଆକାର(UConverterSelector));
  sel->pv_ଗଣନା = indexes[UCNVSEL_INDEX_PV_ଗଣନା];
  sel->encodings = encodings;
  sel->encodings_ଗଣନା = indexes[UCNVSEL_INDEX_NAMES_ଗଣନା];
  sel->encodingStrLength = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_ମୂଲ୍ୟ_BITS,
                                        p, indexes[UCNVSEL_INDEX_TRIE_SIZE], nullptr,
                                        status);
  p += indexes[UCNVSEL_INDEX_TRIE_SIZE];
  ଯଦି (U_FAILURE(*status)) {
    ucnvsel_close(sel);
    ଫେରନ୍ତୁ nullptr;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_ଗଣନା * 4;
  // encoding names
  ଅକ୍ଷର* s = (ଅକ୍ଷର*)p;
  ପାଇଁ (int32_t i = 0; i < sel->encodings_ଗଣନା; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encodingStrLength;

  ଫେରନ୍ତୁ sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
ସଂରଚନା Enumerator {
  int16_t* index;
  int16_t length;
  int16_t cur;
  ଅଚଳ UConverterSelector* sel;
};

U_CDECL_BEGIN

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->index);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


ସ୍ଥିର int32_t U_CALLCONV
ucnvsel_ଗଣନା_encodings(UEnumeration *enumerator, UErrorCode *status) {
  // check if already failed
  ଯଦି (U_FAILURE(*status)) {
    ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ ((Enumerator*)(enumerator->context))->length;
}


ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* U_CALLCONV ucnvsel_next_encoding(UEnumeration* enumerator,
                                                 int32_t* resultLength,
                                                 UErrorCode* status) {
  // check if already failed
  ଯଦି (U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ଅଚଳ UConverterSelector* sel;
  ଅଚଳ ଅକ୍ଷର* result;
  ଯଦି (cur >= ((Enumerator*)(enumerator->context))->length) {
    ଫେରନ୍ତୁ nullptr;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  result = sel->encodings[((Enumerator*)(enumerator->context))->index[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  ଯଦି (resultLength) {
    *resultLength = (int32_t)uprv_strlen(result);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UErrorCode* status) {
  // check if already failed
  ଯଦି (U_FAILURE(*status)) {
    ଫେରନ୍ତୁ ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_END


ସ୍ଥିର ଅଚଳ UEnumeration defaultEncodings = {
  nullptr,
    nullptr,
    ucnvsel_close_selector_iterator,
    ucnvsel_ଗଣନା_encodings,
    uenum_unextDefault,
    ucnvsel_next_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
ସ୍ଥିର UBool intersectMasks(uint32_t* dest, ଅଚଳ uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  ପାଇଁ (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  ଫେରନ୍ତୁ oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
ସ୍ଥିର int16_t ଗଣନା_Ones(uint32_t* mask, int32_t len) {
  int32_t i, totalOnes = 0;
  ପାଇଁ (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    ପାଇଁ (; ent; totalOnes++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  ଫେରନ୍ତୁ static_cast<int16_t>(totalOnes);
}


/* internal function! */
ସ୍ଥିର UEnumeration*
selectForMask(ଅଚଳ UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* theMask,
              UErrorCode* status) {
  LocalMemory<uint32_t> mask(theMask);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  LocalMemory<Enumerator> result(static_cast<Enumerator *>(uprv_malloc(ଆକାର(Enumerator))));
  ଯଦି (result.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  result->index = nullptr;  // this will be allocated later!
  result->length = result->cur = 0;
  result->sel = sel;

  LocalMemory<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(ଆକାର(UEnumeration))));
  ଯଦି (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *status = U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  memcpy(en.getAlias(), &defaultEncodings, ଆକାର(UEnumeration));
  
  int32_t columns = (sel->encodings_ଗଣନା+31)/32;
  int16_t ଅଙ୍କ_Ones = ଗଣନା_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  ଯଦି (ଅଙ୍କ_Ones > 0) {
    result->index = static_cast<int16_t*>(uprv_malloc(ଅଙ୍କ_Ones * ଆକାର(int16_t)));
    ଯଦି (result->index == nullptr) {
      *status = U_MEMORY_ALLOCATION_ERROR;
      ଫେରନ୍ତୁ nullptr;
    }
    int32_t i, j;
    int16_t k = 0;
    ପାଇଁ (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      ପାଇଁ (i = 0 ; i < 32 && k < sel->encodings_ଗଣନା; i++, k++) {
        ଯଦି ((v & 1) != 0) {
          result->index[result->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = result.orphan();
  ଫେରନ୍ତୁ en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ଅଚଳ UConverterSelector* sel U_LIFETIME_BOUND,
                        ଅଚଳ char16_t* s,
                        int32_t length,
                        UErrorCode* status) {
  // check if already failed
  ଯଦି (U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }
  // ensure args make sense!
  ଯଦି (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }

  int32_t columns = (sel->encodings_ଗଣନା+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  ଯଦି (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  ଯଦି(s!=nullptr) {
    ଅଚଳ char16_t *limit;
    ଯଦି (length >= 0) {
      limit = s + length;
    } ଅନ୍ୟଥା {
      limit = nullptr;
    }
    
    ଯେତେବେଳେ (limit == nullptr ? *s != 0 : s != limit) {
      UChar32 c;
      uint16_t pvIndex;
      UTRIE2_U16_NEXT16(sel->trie, s, limit, c, pvIndex);
      ଯଦି (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        ଭାଙ୍ଗ;
      }
    }
  }
  ଫେରନ୍ତୁ selectForMask(sel, mask, status);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ଅଚଳ UConverterSelector* sel U_LIFETIME_BOUND,
                      ଅଚଳ ଅକ୍ଷର* s,
                      int32_t length,
                      UErrorCode* status) {
  // check if already failed
  ଯଦି (U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }
  // ensure args make sense!
  ଯଦି (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }

  int32_t columns = (sel->encodings_ଗଣନା+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  ଯଦି (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  ଯଦି (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  ଯଦି(s!=nullptr) {
    ଅଚଳ ଅକ୍ଷର *limit = s + length;
    
    ଯେତେବେଳେ (s != limit) {
      uint16_t pvIndex;
      UTRIE2_U8_NEXT16(sel->trie, s, limit, pvIndex);
      ଯଦି (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        ଭାଙ୍ଗ;
      }
    }
  }
  ଫେରନ୍ତୁ selectForMask(sel, mask, status);
}

#endif  // !UCONFIG_NO_CONVERSION
