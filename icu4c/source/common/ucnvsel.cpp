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

#समावेश "unicode/ucnvsel.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश <string.h>

#समावेश "unicode/uchar.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/ucnv.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/uchriter.h"
#समावेश "utrie2.h"
#समावेश "propsvec.h"
#समावेश "uassert.h"
#समावेश "ucmndata.h"
#समावेश "udataswp.h"
#समावेश "uenumimp.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"

U_NAMESPACE_USE

काष्ठा UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_गिनती;
  अक्षर** encodings;          // which encodings did user ask to use?
  int32_t encodings_गिनती;
  int32_t encoding_Str_लंबाई;
  uint8_t* swapped;
  UBool ownPv, ownEncodingStrings;
};

अटल व्योम generate_Selector_तथ्य(UConverterSelector* परिणाम,
                                 UPropsVectors *upvec,
                                 स्थिर USet* excludedCodePoints,
                                 स्थिर UConverterUnicodeSet whichSet,
                                 UErrorCode* status) {
  अगर (U_FAILURE(*status)) {
    वापस;
  }

  int32_t columns = (परिणाम->encodings_गिनती+31)/32;

  // set errorValue to all-ones
  क्रम (int32_t col = 0; col < columns; col++) {
    upvec_set_मूल्य(upvec, UPVEC_ERROR_मूल्य_CP, UPVEC_ERROR_मूल्य_CP,
                   col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), status);
  }

  क्रम (int32_t i = 0; i < परिणाम->encodings_गिनती; ++i) {
    uint32_t mask;
    uint32_t column;
    int32_t item_गिनती;
    int32_t j;
    UConverter* test_converter = ucnv_open(परिणाम->encodings[i], status);
    अगर (U_FAILURE(*status)) {
      वापस;
    }
    USet* unicode_point_set;
    unicode_point_set = uset_open(1, 0);  // empty set

    ucnv_getUnicodeSet(test_converter, unicode_point_set,
                       whichSet, status);
    अगर (U_FAILURE(*status)) {
      ucnv_close(test_converter);
      वापस;
    }

    column = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    item_गिनती = uset_get_Item_गिनती(unicode_point_set);

    क्रम (j = 0; j < item_गिनती; ++j) {
      UChar32 start_char;
      UChar32 end_char;
      UErrorCode smallStatus = U_ZERO_ERROR;
      uset_getItem(unicode_point_set, j, &start_char, &end_char, nullptr, 0,
                   &smallStatus);
      अगर (U_FAILURE(smallStatus)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } अन्यथा {
        upvec_set_मूल्य(upvec, start_char, end_char, column, static_cast<uint32_t>(~0), mask,
                       status);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_point_set);
    अगर (U_FAILURE(*status)) {
      वापस;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  अगर (excludedCodePoints) {
    int32_t item_गिनती = uset_get_Item_गिनती(excludedCodePoints);
    क्रम (int32_t j = 0; j < item_गिनती; ++j) {
      UChar32 start_char;
      UChar32 end_char;

      uset_getItem(excludedCodePoints, j, &start_char, &end_char, nullptr, 0,
                   status);
      क्रम (int32_t col = 0; col < columns; col++) {
        upvec_set_मूल्य(upvec, start_char, end_char, col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      status);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  परिणाम->trie = upvec_compactToUTrie2WithRowIndexes(upvec, status);
  परिणाम->pv = upvec_cloneArray(upvec, &परिणाम->pv_गिनती, nullptr, status);
  परिणाम->pv_गिनती *= columns;  // number of uint32_t = rows * columns
  परिणाम->ownPv = true;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(स्थिर अक्षर* स्थिर*  converterList, int32_t converter_List_आकार,
             स्थिर USet* excludedCodePoints,
             स्थिर UConverterUnicodeSet whichSet, UErrorCode* status) {
  // check if already failed
  अगर (U_FAILURE(*status)) {
    वापस nullptr;
  }
  // ensure args make sense!
  अगर (converter_List_आकार < 0 || (converterList == nullptr && converter_List_आकार != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    वापस nullptr;
  }

  // allocate a new converter
  LocalUConverterSelectorPointer newSelector(
    (UConverterSelector*)uprv_malloc(माप(UConverterSelector)));
  अगर (newSelector.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    वापस nullptr;
  }
  uprv_memset(newSelector.getAlias(), 0, माप(UConverterSelector));

  अगर (converter_List_आकार == 0) {
    converterList = nullptr;
    converter_List_आकार = ucnv_गिनती_Available();
  }
  newSelector->encodings =
    (अक्षर**)uprv_malloc(converter_List_आकार * माप(अक्षर*));
  अगर (!newSelector->encodings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    वापस nullptr;
  }
  newSelector->encodings[0] = nullptr;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t योग_आकार = 0;
  int32_t i;
  क्रम (i = 0; i < converter_List_आकार; i++) {
    योग_आकार +=
      (int32_t)uprv_strlen(converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = योग_आकार & 3;
  अगर (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encoding_Str_लंबाई = योग_आकार += encodingStrPadding;
  अक्षर* allStrings = (अक्षर*) uprv_malloc(योग_आकार);
  अगर (!allStrings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    वापस nullptr;
  }

  क्रम (i = 0; i < converter_List_आकार; i++) {
    newSelector->encodings[i] = allStrings;
    uprv_strcpy(newSelector->encodings[i],
                converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i));
    allStrings += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  जबतक (encodingStrPadding > 0) {
    *allStrings++ = 0;
    --encodingStrPadding;
  }

  newSelector->ownEncodingStrings = true;
  newSelector->encodings_गिनती = converter_List_आकार;
  UPropsVectors *upvec = upvec_open((converter_List_आकार+31)/32, status);
  generate_Selector_तथ्य(newSelector.getAlias(), upvec, excludedCodePoints, whichSet, status);
  upvec_close(upvec);

  अगर (U_FAILURE(*status)) {
    वापस nullptr;
  }

  वापस newSelector.orphan();
}

/* close opened selector */
U_CAPI व्योम U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  अगर (!sel) {
    वापस;
  }
  अगर (sel->ownEncodingStrings) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  अगर (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

अटल स्थिर UDataInfo तथ्य_Info = {
  माप(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

क्रमागत {
  UCNVSEL_सूचकांक_TRIE_आकार,      // trie size in bytes
  UCNVSEL_सूचकांक_PV_गिनती,       // number of uint32_t in the bit vectors
  UCNVSEL_सूचकांक_NAMES_गिनती,    // number of encoding names
  UCNVSEL_सूचकांक_NAMES_लंबाई,   // number of encoding name bytes including padding
  UCNVSEL_सूचकांक_आकार = 15,      // bytes following the DataHeader
  UCNVSEL_सूचकांक_गिनती = 16
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
ucnvsel_serialize(स्थिर UConverterSelector* sel,
                  व्योम* बफर, int32_t बफर_Capacity, UErrorCode* status) {
  // check if already failed
  अगर (U_FAILURE(*status)) {
    वापस 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)बफर;
  अगर (बफर_Capacity < 0 ||
      (बफर_Capacity > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    वापस 0;
  }
  // add up the size of the serialized form
  int32_t serialized_Trie_आकार = utrie2_serialize(sel->trie, nullptr, 0, status);
  अगर (*status != U_बफर_OVERFLOW_ERROR && U_FAILURE(*status)) {
    वापस 0;
  }
  *status = U_ZERO_ERROR;

  तथ्य_Header header;
  uprv_memset(&header, 0, माप(header));
  header.तथ्य_Header_2.header_आकार = (uint16_t)((माप(header) + 15) & ~15);
  header.तथ्य_Header_2.magic1 = 0xda;
  header.तथ्य_Header_2.magic2 = 0x27;
  uprv_memcpy(&header.info, &तथ्य_Info, माप(तथ्य_Info));

  int32_t indexes[UCNVSEL_सूचकांक_गिनती] = {
    serialized_Trie_आकार,
    sel->pv_गिनती,
    sel->encodings_गिनती,
    sel->encoding_Str_लंबाई
  };

  int32_t योग_आकार =
    header.तथ्य_Header_2.header_आकार +
    (int32_t)माप(indexes) +
    serialized_Trie_आकार +
    sel->pv_गिनती * 4 +
    sel->encoding_Str_लंबाई;
  indexes[UCNVSEL_सूचकांक_आकार] = योग_आकार - header.तथ्य_Header_2.header_आकार;
  अगर (योग_आकार > बफर_Capacity) {
    *status = U_बफर_OVERFLOW_ERROR;
    वापस योग_आकार;
  }
  // ok, save!
  int32_t length = header.तथ्य_Header_2.header_आकार;
  uprv_memcpy(p, &header, माप(header));
  uprv_memset(p + माप(header), 0, length - माप(header));
  p += length;

  length = (int32_t)माप(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serialized_Trie_आकार, status);
  p += serialized_Trie_आकार;

  length = sel->pv_गिनती * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encoding_Str_लंबाई);
  p += sel->encoding_Str_लंबाई;

  वापस योग_आकार;
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
अटल int32_t
ucnvsel_swap(स्थिर UDataSwapper *ds,
             स्थिर व्योम *in_तथ्य, int32_t length,
             व्योम *out_तथ्य, UErrorCode *status) {
  /* udata_swapDataHeader checks the arguments */
  int32_t header_आकार = udata_swap_तथ्य_Header(ds, in_तथ्य, length, out_तथ्य, status);
  अगर(U_FAILURE(*status)) {
    वापस 0;
  }

  /* check data format and format version */
  स्थिर UDataInfo* pInfo = reinterpret_cast<स्थिर UDataInfo*>(static_cast<स्थिर अक्षर*>(in_तथ्य) + 4);
  अगर(!(
    pInfo->तथ्य_Format[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->तथ्य_Format[1] == 0x53 &&
    pInfo->तथ्य_Format[2] == 0x65 &&
    pInfo->तथ्य_Format[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->तथ्य_Format[0], pInfo->तथ्य_Format[1],
                     pInfo->तथ्य_Format[2], pInfo->तथ्य_Format[3]);
    *status = U_INVALID_FORMAT_ERROR;
    वापस 0;
  }
  अगर(pInfo->formatVersion[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->formatVersion[0]);
    *status = U_UNSUPPORTED_ERROR;
    वापस 0;
  }

  अगर(length >= 0) {
    length -= header_आकार;
    अगर(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *status = U_सूचकांक_OUTOFBOUNDS_ERROR;
      वापस 0;
    }
  }

  स्थिर uint8_t* inBytes = static_cast<स्थिर uint8_t*>(in_तथ्य) + header_आकार;
  uint8_t* outBytes = static_cast<uint8_t*>(out_तथ्य) + header_आकार;

  /* read the indexes */
  स्थिर int32_t* inIndexes = reinterpret_cast<स्थिर int32_t*>(inBytes);
  int32_t indexes[16];
  int32_t i;
  क्रम(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, inIndexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_सूचकांक_आकार];
  अगर(length >= 0) {
    अगर(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *status = U_सूचकांक_OUTOFBOUNDS_ERROR;
      वापस 0;
    }

    /* copy the data for inaccessible bytes */
    अगर(inBytes != outBytes) {
      uprv_memcpy(outBytes, inBytes, size);
    }

    int32_t offset = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_सूचकांक_गिनती*4;
    ds->swapArray32(ds, inBytes, count, outBytes, status);
    offset += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_सूचकांक_TRIE_आकार];
    utrie2_swap(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_सूचकांक_PV_गिनती]*4;
    ds->swapArray32(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_सूचकांक_NAMES_लंबाई];
    ds->swapInvChars(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    U_ASSERT(offset == size);
  }

  वापस header_आकार + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(स्थिर व्योम* बफर U_LIFETIME_BOUND,
                           int32_t length,
                           UErrorCode* status) {
  // check if already failed
  अगर (U_FAILURE(*status)) {
    वापस nullptr;
  }
  // ensure args make sense!
  स्थिर uint8_t *p = (स्थिर uint8_t *)बफर;
  अगर (length <= 0 ||
      (length > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    वापस nullptr;
  }
  // header
  अगर (length < 32) {
    // not even enough space for a minimal header
    *status = U_सूचकांक_OUTOFBOUNDS_ERROR;
    वापस nullptr;
  }
  स्थिर तथ्य_Header *pHeader = (स्थिर तथ्य_Header *)p;
  अगर (!(
    pHeader->तथ्य_Header_2.magic1==0xda &&
    pHeader->तथ्य_Header_2.magic2==0x27 &&
    pHeader->info.तथ्य_Format[0] == 0x43 &&
    pHeader->info.तथ्य_Format[1] == 0x53 &&
    pHeader->info.तथ्य_Format[2] == 0x65 &&
    pHeader->info.तथ्य_Format[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *status = U_INVALID_FORMAT_ERROR;
    वापस nullptr;
  }
  अगर (pHeader->info.formatVersion[0] != 1) {
    *status = U_UNSUPPORTED_ERROR;
    वापस nullptr;
  }
  uint8_t* swapped = nullptr;
  अगर (pHeader->info.isBigEndian != U_IS_BIG_ENDIAN ||
      pHeader->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_निवेश_तथ्य(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, status);
    int32_t योग_आकार = ucnvsel_swap(ds, p, -1, nullptr, status);
    अगर (U_FAILURE(*status)) {
      udata_closeSwapper(ds);
      वापस nullptr;
    }
    अगर (length < योग_आकार) {
      udata_closeSwapper(ds);
      *status = U_सूचकांक_OUTOFBOUNDS_ERROR;
      वापस nullptr;
    }
    swapped = (uint8_t*)uprv_malloc(योग_आकार);
    अगर (swapped == nullptr) {
      udata_closeSwapper(ds);
      *status = U_MEMORY_ALLOCATION_ERROR;
      वापस nullptr;
    }
    ucnvsel_swap(ds, p, length, swapped, status);
    udata_closeSwapper(ds);
    अगर (U_FAILURE(*status)) {
      uprv_free(swapped);
      वापस nullptr;
    }
    p = swapped;
    pHeader = (स्थिर तथ्य_Header *)p;
  }
  अगर (length < (pHeader->तथ्य_Header_2.header_आकार + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *status = U_सूचकांक_OUTOFBOUNDS_ERROR;
    वापस nullptr;
  }
  p += pHeader->तथ्य_Header_2.header_आकार;
  length -= pHeader->तथ्य_Header_2.header_आकार;
  // indexes
  स्थिर int32_t *indexes = (स्थिर int32_t *)p;
  अगर (length < indexes[UCNVSEL_सूचकांक_आकार]) {
    uprv_free(swapped);
    *status = U_सूचकांक_OUTOFBOUNDS_ERROR;
    वापस nullptr;
  }
  p += UCNVSEL_सूचकांक_गिनती * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(माप(UConverterSelector));
  अक्षर **encodings =
    (अक्षर **)uprv_malloc(
      indexes[UCNVSEL_सूचकांक_NAMES_गिनती] * माप(अक्षर *));
  अगर (sel == nullptr || encodings == nullptr) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *status = U_MEMORY_ALLOCATION_ERROR;
    वापस nullptr;
  }
  uprv_memset(sel, 0, माप(UConverterSelector));
  sel->pv_गिनती = indexes[UCNVSEL_सूचकांक_PV_गिनती];
  sel->encodings = encodings;
  sel->encodings_गिनती = indexes[UCNVSEL_सूचकांक_NAMES_गिनती];
  sel->encoding_Str_लंबाई = indexes[UCNVSEL_सूचकांक_NAMES_लंबाई];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_मूल्य_BITS,
                                        p, indexes[UCNVSEL_सूचकांक_TRIE_आकार], nullptr,
                                        status);
  p += indexes[UCNVSEL_सूचकांक_TRIE_आकार];
  अगर (U_FAILURE(*status)) {
    ucnvsel_close(sel);
    वापस nullptr;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_गिनती * 4;
  // encoding names
  अक्षर* s = (अक्षर*)p;
  क्रम (int32_t i = 0; i < sel->encodings_गिनती; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encoding_Str_लंबाई;

  वापस sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
काष्ठा Enumerator {
  int16_t* सूचकांक;
  int16_t length;
  int16_t cur;
  स्थिर UConverterSelector* sel;
};

U_CDECL_BEGIN

अटल व्योम U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->सूचकांक);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


अटल int32_t U_CALLCONV
ucnvsel_गिनती_encodings(UEnumeration *enumerator, UErrorCode *status) {
  // check if already failed
  अगर (U_FAILURE(*status)) {
    वापस 0;
  }
  वापस ((Enumerator*)(enumerator->context))->length;
}


अटल स्थिर अक्षर* U_CALLCONV ucnvsel_अग्र_encoding(UEnumeration* enumerator,
                                                 int32_t* परिणाम_लंबाई,
                                                 UErrorCode* status) {
  // check if already failed
  अगर (U_FAILURE(*status)) {
    वापस nullptr;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  स्थिर UConverterSelector* sel;
  स्थिर अक्षर* परिणाम;
  अगर (cur >= ((Enumerator*)(enumerator->context))->length) {
    वापस nullptr;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  परिणाम = sel->encodings[((Enumerator*)(enumerator->context))->सूचकांक[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  अगर (परिणाम_लंबाई) {
    *परिणाम_लंबाई = (int32_t)uprv_strlen(परिणाम);
  }
  वापस परिणाम;
}

अटल व्योम U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UErrorCode* status) {
  // check if already failed
  अगर (U_FAILURE(*status)) {
    वापस ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_END


अटल स्थिर UEnumeration defaultEncodings = {
  nullptr,
    nullptr,
    ucnvsel_close_selector_iterator,
    ucnvsel_गिनती_encodings,
    uenum_unextDefault,
    ucnvsel_अग्र_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
अटल UBool intersectMasks(uint32_t* dest, स्थिर uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  क्रम (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  वापस oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
अटल int16_t गिनती_Ones(uint32_t* mask, int32_t len) {
  int32_t i, योग_Ones = 0;
  क्रम (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    क्रम (; ent; योग_Ones++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  वापस static_cast<int16_t>(योग_Ones);
}


/* internal function! */
अटल UEnumeration*
selectForMask(स्थिर UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* theMask,
              UErrorCode* status) {
  LocalMemory<uint32_t> mask(theMask);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  LocalMemory<Enumerator> परिणाम(static_cast<Enumerator *>(uprv_malloc(माप(Enumerator))));
  अगर (परिणाम.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    वापस nullptr;
  }
  परिणाम->सूचकांक = nullptr;  // this will be allocated later!
  परिणाम->length = परिणाम->cur = 0;
  परिणाम->sel = sel;

  LocalMemory<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(माप(UEnumeration))));
  अगर (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *status = U_MEMORY_ALLOCATION_ERROR;
    वापस nullptr;
  }
  स_नकल(en.getAlias(), &defaultEncodings, माप(UEnumeration));
  
  int32_t columns = (sel->encodings_गिनती+31)/32;
  int16_t अंक_Ones = गिनती_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  अगर (अंक_Ones > 0) {
    परिणाम->सूचकांक = static_cast<int16_t*>(uprv_malloc(अंक_Ones * माप(int16_t)));
    अगर (परिणाम->सूचकांक == nullptr) {
      *status = U_MEMORY_ALLOCATION_ERROR;
      वापस nullptr;
    }
    int32_t i, j;
    int16_t k = 0;
    क्रम (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      क्रम (i = 0 ; i < 32 && k < sel->encodings_गिनती; i++, k++) {
        अगर ((v & 1) != 0) {
          परिणाम->सूचकांक[परिणाम->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = परिणाम.orphan();
  वापस en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(स्थिर UConverterSelector* sel U_LIFETIME_BOUND,
                        स्थिर char16_t* s,
                        int32_t length,
                        UErrorCode* status) {
  // check if already failed
  अगर (U_FAILURE(*status)) {
    वापस nullptr;
  }
  // ensure args make sense!
  अगर (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    वापस nullptr;
  }

  int32_t columns = (sel->encodings_गिनती+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  अगर (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    वापस nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  अगर(s!=nullptr) {
    स्थिर char16_t *limit;
    अगर (length >= 0) {
      limit = s + length;
    } अन्यथा {
      limit = nullptr;
    }
    
    जबतक (limit == nullptr ? *s != 0 : s != limit) {
      UChar32 c;
      uint16_t pv_सूचकांक;
      UTRIE2_U16_NEXT16(sel->trie, s, limit, c, pv_सूचकांक);
      अगर (intersectMasks(mask, sel->pv+pv_सूचकांक, columns)) {
        अवरोध;
      }
    }
  }
  वापस selectForMask(sel, mask, status);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(स्थिर UConverterSelector* sel U_LIFETIME_BOUND,
                      स्थिर अक्षर* s,
                      int32_t length,
                      UErrorCode* status) {
  // check if already failed
  अगर (U_FAILURE(*status)) {
    वापस nullptr;
  }
  // ensure args make sense!
  अगर (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    वापस nullptr;
  }

  int32_t columns = (sel->encodings_गिनती+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  अगर (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    वापस nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  अगर (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  अगर(s!=nullptr) {
    स्थिर अक्षर *limit = s + length;
    
    जबतक (s != limit) {
      uint16_t pv_सूचकांक;
      UTRIE2_U8_NEXT16(sel->trie, s, limit, pv_सूचकांक);
      अगर (intersectMasks(mask, sel->pv+pv_सूचकांक, columns)) {
        अवरोध;
      }
    }
  }
  वापस selectForMask(sel, mask, status);
}

#पूर्ण_अगर  // !UCONFIG_NO_CONVERSION
