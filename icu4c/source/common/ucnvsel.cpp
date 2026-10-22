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

#अन्तर्भाव "unicode/ucnvsel.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव <string.h>

#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uchriter.h"
#अन्तर्भाव "utrie2.h"
#अन्तर्भाव "propsvec.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucmndata.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "uenumimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"

U_NAMESPACE_USE

संरचना UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_गणनम्;
  वर्ण** encodings;          // which encodings did user ask to use?
  int32_t encodings_गणनम्;
  int32_t encoding_Str_दीर्घता;
  uint8_t* swapped;
  UBool ownPv, own_Encoding_वाक्यानि;
};

स्थिर शून्य generate_Selector_दत्त(UConverterSelector* परिणाम,
                                 UPropsVectors *upvec,
                                 ध्रुव USet* excluded_संहिता_Points,
                                 ध्रुव UConverter_Unicode_समुच्चयः which_समुच्चयः,
                                 UError_संहिता* स्थितिः) {
  यदि (U_FAILURE(*स्थितिः)) {
    प्रत्यागम;
  }

  int32_t columns = (परिणाम->encodings_गणनम्+31)/32;

  // set errorValue to all-ones
  प्रति (int32_t col = 0; col < columns; col++) {
    upvec_समुच्चयः_मानम्(upvec, UPVEC_ERROR_मानम्_CP, UPVEC_ERROR_मानम्_CP,
                   col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), स्थितिः);
  }

  प्रति (int32_t i = 0; i < परिणाम->encodings_गणनम्; ++i) {
    uint32_t mask;
    uint32_t column;
    int32_t item_गणनम्;
    int32_t j;
    UConverter* test_converter = ucnv_open(परिणाम->encodings[i], स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
      प्रत्यागम;
    }
    USet* unicode_point_समुच्चयः;
    unicode_point_समुच्चयः = uset_open(1, 0);  // empty set

    ucnv_get_Unicode_समुच्चयः(test_converter, unicode_point_समुच्चयः,
                       which_समुच्चयः, स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
      ucnv_close(test_converter);
      प्रत्यागम;
    }

    column = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    item_गणनम् = uset_get_Item_गणनम्(unicode_point_समुच्चयः);

    प्रति (j = 0; j < item_गणनम्; ++j) {
      UChar32 start_char;
      UChar32 अन्तः_char;
      UError_संहिता small_स्थितिः = U_ZERO_ERROR;
      uset_getItem(unicode_point_समुच्चयः, j, &start_char, &अन्तः_char, शून्यसूचकः, 0,
                   &small_स्थितिः);
      यदि (U_FAILURE(small_स्थितिः)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } अन्यथा {
        upvec_समुच्चयः_मानम्(upvec, start_char, अन्तः_char, column, static_cast<uint32_t>(~0), mask,
                       स्थितिः);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_point_समुच्चयः);
    यदि (U_FAILURE(*स्थितिः)) {
      प्रत्यागम;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  यदि (excluded_संहिता_Points) {
    int32_t item_गणनम् = uset_get_Item_गणनम्(excluded_संहिता_Points);
    प्रति (int32_t j = 0; j < item_गणनम्; ++j) {
      UChar32 start_char;
      UChar32 अन्तः_char;

      uset_getItem(excluded_संहिता_Points, j, &start_char, &अन्तः_char, शून्यसूचकः, 0,
                   स्थितिः);
      प्रति (int32_t col = 0; col < columns; col++) {
        upvec_समुच्चयः_मानम्(upvec, start_char, अन्तः_char, col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      स्थितिः);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  परिणाम->trie = upvec_compact_गन्तव्यम्_UTrie2_With_Row_Indexes(upvec, स्थितिः);
  परिणाम->pv = upvec_cloneArray(upvec, &परिणाम->pv_गणनम्, शून्यसूचकः, स्थितिः);
  परिणाम->pv_गणनम् *= columns;  // number of uint32_t = rows * columns
  परिणाम->ownPv = सत्य;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ध्रुव वर्ण* ध्रुव*  converterList, int32_t converter_List_आकार,
             ध्रुव USet* excluded_संहिता_Points,
             ध्रुव UConverter_Unicode_समुच्चयः which_समुच्चयः, UError_संहिता* स्थितिः) {
  // check if already failed
  यदि (U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }
  // ensure args make sense!
  यदि (converter_List_आकार < 0 || (converterList == शून्यसूचकः && converter_List_आकार != 0)) {
    *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम शून्यसूचकः;
  }

  // allocate a new converter
  LocalUConverterSelectorPointer newSelector(
    (UConverterSelector*)uprv_malloc(परिमाण(UConverterSelector)));
  यदि (newSelector.isNull()) {
    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  uprv_memset(newSelector.getAlias(), 0, परिमाण(UConverterSelector));

  यदि (converter_List_आकार == 0) {
    converterList = शून्यसूचकः;
    converter_List_आकार = ucnv_गणनम्_उपलब्धम्();
  }
  newSelector->encodings =
    (वर्ण**)uprv_malloc(converter_List_आकार * परिमाण(वर्ण*));
  यदि (!newSelector->encodings) {
    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  newSelector->encodings[0] = शून्यसूचकः;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t योग_आकार = 0;
  int32_t i;
  प्रति (i = 0; i < converter_List_आकार; i++) {
    योग_आकार +=
      (int32_t)uprv_strlen(converterList != शून्यसूचकः ? converterList[i] : ucnv_get_उपलब्धम्_Name(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = योग_आकार & 3;
  यदि (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encoding_Str_दीर्घता = योग_आकार += encodingStrPadding;
  वर्ण* all_वाक्यानि = (वर्ण*) uprv_malloc(योग_आकार);
  यदि (!all_वाक्यानि) {
    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम शून्यसूचकः;
  }

  प्रति (i = 0; i < converter_List_आकार; i++) {
    newSelector->encodings[i] = all_वाक्यानि;
    uprv_strcpy(newSelector->encodings[i],
                converterList != शून्यसूचकः ? converterList[i] : ucnv_get_उपलब्धम्_Name(i));
    all_वाक्यानि += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  यावत् (encodingStrPadding > 0) {
    *all_वाक्यानि++ = 0;
    --encodingStrPadding;
  }

  newSelector->own_Encoding_वाक्यानि = सत्य;
  newSelector->encodings_गणनम् = converter_List_आकार;
  UPropsVectors *upvec = upvec_open((converter_List_आकार+31)/32, स्थितिः);
  generate_Selector_दत्त(newSelector.getAlias(), upvec, excluded_संहिता_Points, which_समुच्चयः, स्थितिः);
  upvec_close(upvec);

  यदि (U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }

  प्रत्यागम newSelector.orphan();
}

/* close opened selector */
U_CAPI शून्य U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  यदि (!sel) {
    प्रत्यागम;
  }
  यदि (sel->own_Encoding_वाक्यानि) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  यदि (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

स्थिर ध्रुव UDataInfo दत्त_Info = {
  परिमाण(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

गणना {
  UCNVSEL_सूचकांक_TRIE_आकार,      // trie size in bytes
  UCNVSEL_सूचकांक_PV_गणनम्,       // number of uint32_t in the bit vectors
  UCNVSEL_सूचकांक_नामानि_गणनम्,    // number of encoding names
  UCNVSEL_सूचकांक_नामानि_दीर्घता,   // number of encoding name bytes including padding
  UCNVSEL_सूचकांक_आकार = 15,      // bytes following the DataHeader
  UCNVSEL_सूचकांक_गणनम् = 16
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
ucnvsel_serialize(ध्रुव UConverterSelector* sel,
                  शून्य* बफर, int32_t बफर_क्षमता, UError_संहिता* स्थितिः) {
  // check if already failed
  यदि (U_FAILURE(*स्थितिः)) {
    प्रत्यागम 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)बफर;
  यदि (बफर_क्षमता < 0 ||
      (बफर_क्षमता > 0 && (p == शून्यसूचकः || (U_POINTER_आवरणम्_LSB(p, 3) != 0)))
  ) {
    *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम 0;
  }
  // add up the size of the serialized form
  int32_t serialized_Trie_आकार = utrie2_serialize(sel->trie, शून्यसूचकः, 0, स्थितिः);
  यदि (*स्थितिः != U_बफर_अतिप्रवाहः_ERROR && U_FAILURE(*स्थितिः)) {
    प्रत्यागम 0;
  }
  *स्थितिः = U_ZERO_ERROR;

  दत्त_शीर्षकम् शीर्षकम्;
  uprv_memset(&शीर्षकम्, 0, परिमाण(शीर्षकम्));
  शीर्षकम्.दत्त_शीर्षकम्_2.शीर्षकम्_आकार = (uint16_t)((परिमाण(शीर्षकम्) + 15) & ~15);
  शीर्षकम्.दत्त_शीर्षकम्_2.magic1 = 0xda;
  शीर्षकम्.दत्त_शीर्षकम्_2.magic2 = 0x27;
  uprv_memcpy(&शीर्षकम्.info, &दत्त_Info, परिमाण(दत्त_Info));

  int32_t indexes[UCNVSEL_सूचकांक_गणनम्] = {
    serialized_Trie_आकार,
    sel->pv_गणनम्,
    sel->encodings_गणनम्,
    sel->encoding_Str_दीर्घता
  };

  int32_t योग_आकार =
    शीर्षकम्.दत्त_शीर्षकम्_2.शीर्षकम्_आकार +
    (int32_t)परिमाण(indexes) +
    serialized_Trie_आकार +
    sel->pv_गणनम् * 4 +
    sel->encoding_Str_दीर्घता;
  indexes[UCNVSEL_सूचकांक_आकार] = योग_आकार - शीर्षकम्.दत्त_शीर्षकम्_2.शीर्षकम्_आकार;
  यदि (योग_आकार > बफर_क्षमता) {
    *स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
    प्रत्यागम योग_आकार;
  }
  // ok, save!
  int32_t length = शीर्षकम्.दत्त_शीर्षकम्_2.शीर्षकम्_आकार;
  uprv_memcpy(p, &शीर्षकम्, परिमाण(शीर्षकम्));
  uprv_memset(p + परिमाण(शीर्षकम्), 0, length - परिमाण(शीर्षकम्));
  p += length;

  length = (int32_t)परिमाण(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serialized_Trie_आकार, स्थितिः);
  p += serialized_Trie_आकार;

  length = sel->pv_गणनम् * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encoding_Str_दीर्घता);
  p += sel->encoding_Str_दीर्घता;

  प्रत्यागम योग_आकार;
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
स्थिर int32_t
ucnvsel_swap(ध्रुव UDataSwapper *ds,
             ध्रुव शून्य *अन्तर्_दत्त, int32_t length,
             शून्य *बहिर्गमनम्_दत्त, UError_संहिता *स्थितिः) {
  /* udata_swapDataHeader checks the arguments */
  int32_t शीर्षकम्_आकार = udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, स्थितिः);
  यदि(U_FAILURE(*स्थितिः)) {
    प्रत्यागम 0;
  }

  /* check data format and format version */
  ध्रुव UDataInfo* pInfo = reinterpret_cast<ध्रुव UDataInfo*>(static_cast<ध्रुव वर्ण*>(अन्तर्_दत्त) + 4);
  यदि(!(
    pInfo->दत्त_Format[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->दत्त_Format[1] == 0x53 &&
    pInfo->दत्त_Format[2] == 0x65 &&
    pInfo->दत्त_Format[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->दत्त_Format[0], pInfo->दत्त_Format[1],
                     pInfo->दत्त_Format[2], pInfo->दत्त_Format[3]);
    *स्थितिः = U_INVALID_FORMAT_ERROR;
    प्रत्यागम 0;
  }
  यदि(pInfo->format_संस्करणम्[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->format_संस्करणम्[0]);
    *स्थितिः = U_UNSUPPORTED_ERROR;
    प्रत्यागम 0;
  }

  यदि(length >= 0) {
    length -= शीर्षकम्_आकार;
    यदि(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *स्थितिः = U_सूचकांक_OUTOFBOUNDS_ERROR;
      प्रत्यागम 0;
    }
  }

  ध्रुव uint8_t* अन्तर्_अष्टकानि = static_cast<ध्रुव uint8_t*>(अन्तर्_दत्त) + शीर्षकम्_आकार;
  uint8_t* बहिर्गमनम्_अष्टकानि = static_cast<uint8_t*>(बहिर्गमनम्_दत्त) + शीर्षकम्_आकार;

  /* read the indexes */
  ध्रुव int32_t* अन्तर्_Indexes = reinterpret_cast<ध्रुव int32_t*>(अन्तर्_अष्टकानि);
  int32_t indexes[16];
  int32_t i;
  प्रति(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, अन्तर्_Indexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_सूचकांक_आकार];
  यदि(length >= 0) {
    यदि(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *स्थितिः = U_सूचकांक_OUTOFBOUNDS_ERROR;
      प्रत्यागम 0;
    }

    /* copy the data for inaccessible bytes */
    यदि(अन्तर्_अष्टकानि != बहिर्गमनम्_अष्टकानि) {
      uprv_memcpy(बहिर्गमनम्_अष्टकानि, अन्तर्_अष्टकानि, size);
    }

    int32_t विस्थापनम् = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_सूचकांक_गणनम्*4;
    ds->swapArray32(ds, अन्तर्_अष्टकानि, count, बहिर्गमनम्_अष्टकानि, स्थितिः);
    विस्थापनम् += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_सूचकांक_TRIE_आकार];
    utrie2_swap(ds, अन्तर्_अष्टकानि + विस्थापनम्, count, बहिर्गमनम्_अष्टकानि + विस्थापनम्, स्थितिः);
    विस्थापनम् += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_सूचकांक_PV_गणनम्]*4;
    ds->swapArray32(ds, अन्तर्_अष्टकानि + विस्थापनम्, count, बहिर्गमनम्_अष्टकानि + विस्थापनम्, स्थितिः);
    विस्थापनम् += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_सूचकांक_नामानि_दीर्घता];
    ds->swapInvChars(ds, अन्तर्_अष्टकानि + विस्थापनम्, count, बहिर्गमनम्_अष्टकानि + विस्थापनम्, स्थितिः);
    विस्थापनम् += count;

    U_ASSERT(विस्थापनम् == size);
  }

  प्रत्यागम शीर्षकम्_आकार + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ध्रुव शून्य* बफर U_LIFETIME_BOUND,
                           int32_t length,
                           UError_संहिता* स्थितिः) {
  // check if already failed
  यदि (U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }
  // ensure args make sense!
  ध्रुव uint8_t *p = (ध्रुव uint8_t *)बफर;
  यदि (length <= 0 ||
      (length > 0 && (p == शून्यसूचकः || (U_POINTER_आवरणम्_LSB(p, 3) != 0)))
  ) {
    *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  //शीर्षकम्
  यदि (length < 32) {
    // not even enough space for a minimal header
    *स्थितिः = U_सूचकांक_OUTOFBOUNDS_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  ध्रुव दत्त_शीर्षकम् *p_शीर्षकम् = (ध्रुव दत्त_शीर्षकम् *)p;
  यदि (!(
    p_शीर्षकम्->दत्त_शीर्षकम्_2.magic1==0xda &&
    p_शीर्षकम्->दत्त_शीर्षकम्_2.magic2==0x27 &&
    p_शीर्षकम्->info.दत्त_Format[0] == 0x43 &&
    p_शीर्षकम्->info.दत्त_Format[1] == 0x53 &&
    p_शीर्षकम्->info.दत्त_Format[2] == 0x65 &&
    p_शीर्षकम्->info.दत्त_Format[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *स्थितिः = U_INVALID_FORMAT_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  यदि (p_शीर्षकम्->info.format_संस्करणम्[0] != 1) {
    *स्थितिः = U_UNSUPPORTED_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  uint8_t* swapped = शून्यसूचकः;
  यदि (p_शीर्षकम्->info.isBigEndian != U_IS_BIG_ENDIAN ||
      p_शीर्षकम्->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_निवेशः_दत्त(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, स्थितिः);
    int32_t योग_आकार = ucnvsel_swap(ds, p, -1, शून्यसूचकः, स्थितिः);
    यदि (U_FAILURE(*स्थितिः)) {
      udata_closeSwapper(ds);
      प्रत्यागम शून्यसूचकः;
    }
    यदि (length < योग_आकार) {
      udata_closeSwapper(ds);
      *स्थितिः = U_सूचकांक_OUTOFBOUNDS_ERROR;
      प्रत्यागम शून्यसूचकः;
    }
    swapped = (uint8_t*)uprv_malloc(योग_आकार);
    यदि (swapped == शून्यसूचकः) {
      udata_closeSwapper(ds);
      *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
      प्रत्यागम शून्यसूचकः;
    }
    ucnvsel_swap(ds, p, length, swapped, स्थितिः);
    udata_closeSwapper(ds);
    यदि (U_FAILURE(*स्थितिः)) {
      uprv_free(swapped);
      प्रत्यागम शून्यसूचकः;
    }
    p = swapped;
    p_शीर्षकम् = (ध्रुव दत्त_शीर्षकम् *)p;
  }
  यदि (length < (p_शीर्षकम्->दत्त_शीर्षकम्_2.शीर्षकम्_आकार + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *स्थितिः = U_सूचकांक_OUTOFBOUNDS_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  p += p_शीर्षकम्->दत्त_शीर्षकम्_2.शीर्षकम्_आकार;
  length -= p_शीर्षकम्->दत्त_शीर्षकम्_2.शीर्षकम्_आकार;
  // indexes
  ध्रुव int32_t *indexes = (ध्रुव int32_t *)p;
  यदि (length < indexes[UCNVSEL_सूचकांक_आकार]) {
    uprv_free(swapped);
    *स्थितिः = U_सूचकांक_OUTOFBOUNDS_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  p += UCNVSEL_सूचकांक_गणनम् * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(परिमाण(UConverterSelector));
  वर्ण **encodings =
    (वर्ण **)uprv_malloc(
      indexes[UCNVSEL_सूचकांक_नामानि_गणनम्] * परिमाण(वर्ण *));
  यदि (sel == शून्यसूचकः || encodings == शून्यसूचकः) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  uprv_memset(sel, 0, परिमाण(UConverterSelector));
  sel->pv_गणनम् = indexes[UCNVSEL_सूचकांक_PV_गणनम्];
  sel->encodings = encodings;
  sel->encodings_गणनम् = indexes[UCNVSEL_सूचकांक_नामानि_गणनम्];
  sel->encoding_Str_दीर्घता = indexes[UCNVSEL_सूचकांक_नामानि_दीर्घता];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_मानम्_द्वयङ्काः,
                                        p, indexes[UCNVSEL_सूचकांक_TRIE_आकार], शून्यसूचकः,
                                        स्थितिः);
  p += indexes[UCNVSEL_सूचकांक_TRIE_आकार];
  यदि (U_FAILURE(*स्थितिः)) {
    ucnvsel_close(sel);
    प्रत्यागम शून्यसूचकः;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_गणनम् * 4;
  // encoding names
  वर्ण* s = (वर्ण*)p;
  प्रति (int32_t i = 0; i < sel->encodings_गणनम्; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encoding_Str_दीर्घता;

  प्रत्यागम sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
संरचना Enumerator {
  int16_t* सूचकांक;
  int16_t length;
  int16_t cur;
  ध्रुव UConverterSelector* sel;
};

U_CDECL_BEGIN

स्थिर शून्य U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->सूचकांक);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


स्थिर int32_t U_CALLCONV
ucnvsel_गणनम्_encodings(UEnumeration *enumerator, UError_संहिता *स्थितिः) {
  // check if already failed
  यदि (U_FAILURE(*स्थितिः)) {
    प्रत्यागम 0;
  }
  प्रत्यागम ((Enumerator*)(enumerator->context))->length;
}


स्थिर ध्रुव वर्ण* U_CALLCONV ucnvsel_अग्र_encoding(UEnumeration* enumerator,
                                                 int32_t* परिणाम_दीर्घता,
                                                 UError_संहिता* स्थितिः) {
  // check if already failed
  यदि (U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ध्रुव UConverterSelector* sel;
  ध्रुव वर्ण* परिणाम;
  यदि (cur >= ((Enumerator*)(enumerator->context))->length) {
    प्रत्यागम शून्यसूचकः;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  परिणाम = sel->encodings[((Enumerator*)(enumerator->context))->सूचकांक[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  यदि (परिणाम_दीर्घता) {
    *परिणाम_दीर्घता = (int32_t)uprv_strlen(परिणाम);
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UError_संहिता* स्थितिः) {
  // check if already failed
  यदि (U_FAILURE(*स्थितिः)) {
    प्रत्यागम ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_अन्तः


स्थिर ध्रुव UEnumeration defaultEncodings = {
  शून्यसूचकः,
    शून्यसूचकः,
    ucnvsel_close_selector_iterator,
    ucnvsel_गणनम्_encodings,
    uenum_unextDefault,
    ucnvsel_अग्र_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
स्थिर UBool intersectMasks(uint32_t* dest, ध्रुव uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  प्रति (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  प्रत्यागम oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
स्थिर int16_t गणनम्_Ones(uint32_t* mask, int32_t len) {
  int32_t i, योग_Ones = 0;
  प्रति (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    प्रति (; ent; योग_Ones++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  प्रत्यागम static_cast<int16_t>(योग_Ones);
}


/* internal function! */
स्थिर UEnumeration*
select_For_आवरणम्(ध्रुव UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* the_आवरणम्,
              UError_संहिता* स्थितिः) {
  Local_स्मृतिः<uint32_t> mask(the_आवरणम्);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  Local_स्मृतिः<Enumerator> परिणाम(static_cast<Enumerator *>(uprv_malloc(परिमाण(Enumerator))));
  यदि (परिणाम.isNull()) {
    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  परिणाम->सूचकांक = शून्यसूचकः;  // this will be allocated later!
  परिणाम->length = परिणाम->cur = 0;
  परिणाम->sel = sel;

  Local_स्मृतिः<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(परिमाण(UEnumeration))));
  यदि (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  स्मृतिं_प्रतिलिख(en.getAlias(), &defaultEncodings, परिमाण(UEnumeration));
  
  int32_t columns = (sel->encodings_गणनम्+31)/32;
  int16_t अङ्कः_Ones = गणनम्_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  यदि (अङ्कः_Ones > 0) {
    परिणाम->सूचकांक = static_cast<int16_t*>(uprv_malloc(अङ्कः_Ones * परिमाण(int16_t)));
    यदि (परिणाम->सूचकांक == शून्यसूचकः) {
      *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
      प्रत्यागम शून्यसूचकः;
    }
    int32_t i, j;
    int16_t k = 0;
    प्रति (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      प्रति (i = 0 ; i < 32 && k < sel->encodings_गणनम्; i++, k++) {
        यदि ((v & 1) != 0) {
          परिणाम->सूचकांक[परिणाम->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = परिणाम.orphan();
  प्रत्यागम en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ध्रुव UConverterSelector* sel U_LIFETIME_BOUND,
                        ध्रुव char16_t* s,
                        int32_t length,
                        UError_संहिता* स्थितिः) {
  // check if already failed
  यदि (U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }
  // ensure args make sense!
  यदि (sel == शून्यसूचकः || (s == शून्यसूचकः && length != 0)) {
    *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम शून्यसूचकः;
  }

  int32_t columns = (sel->encodings_गणनम्+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  यदि (mask == शून्यसूचकः) {
    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  uprv_memset(mask, ~0, columns *4);

  यदि(s!=शून्यसूचकः) {
    ध्रुव char16_t *सीमा;
    यदि (length >= 0) {
      सीमा = s + length;
    } अन्यथा {
      सीमा = शून्यसूचकः;
    }
    
    यावत् (सीमा == शून्यसूचकः ? *s != 0 : s != सीमा) {
      UChar32 c;
      uint16_t pv_सूचकांक;
      UTRIE2_U16_NEXT16(sel->trie, s, सीमा, c, pv_सूचकांक);
      यदि (intersectMasks(mask, sel->pv+pv_सूचकांक, columns)) {
        भङ्ग;
      }
    }
  }
  प्रत्यागम select_For_आवरणम्(sel, mask, स्थितिः);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ध्रुव UConverterSelector* sel U_LIFETIME_BOUND,
                      ध्रुव वर्ण* s,
                      int32_t length,
                      UError_संहिता* स्थितिः) {
  // check if already failed
  यदि (U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }
  // ensure args make sense!
  यदि (sel == शून्यसूचकः || (s == शून्यसूचकः && length != 0)) {
    *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    प्रत्यागम शून्यसूचकः;
  }

  int32_t columns = (sel->encodings_गणनम्+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  यदि (mask == शून्यसूचकः) {
    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम शून्यसूचकः;
  }
  uprv_memset(mask, ~0, columns *4);

  यदि (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  यदि(s!=शून्यसूचकः) {
    ध्रुव वर्ण *सीमा = s + length;
    
    यावत् (s != सीमा) {
      uint16_t pv_सूचकांक;
      UTRIE2_U8_NEXT16(sel->trie, s, सीमा, pv_सूचकांक);
      यदि (intersectMasks(mask, sel->pv+pv_सूचकांक, columns)) {
        भङ्ग;
      }
    }
  }
  प्रत्यागम select_For_आवरणम्(sel, mask, स्थितिः);
}

#यदि_अन्तः  // !UCONFIG_NO_CONVERSION
