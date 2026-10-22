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

#అన్తర్భావ "unicode/ucnvsel.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ <string.h>

#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/uchriter.h"
#అన్తర్భావ "utrie2.h"
#అన్తర్భావ "propsvec.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucmndata.h"
#అన్తర్భావ "udataswp.h"
#అన్తర్భావ "uenumimp.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"

U_NAMESPACE_USE

నిర్మాణం UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_లెక్క;
  అక్షరం** encodings;          // which encodings did user ask to use?
  int32_t encodings_లెక్క;
  int32_t encoding_Str_నిడివి;
  uint8_t* swapped;
  UBool ownPv, own_Encoding_అక్షరశ్రేణులు;
};

స్థిర శూన్య generate_Selector_దత్తాంశం(UConverterSelector* ఫలితం,
                                 UPropsVectors *upvec,
                                 స్థిరరాశి USet* excluded_సంకేతం_Points,
                                 స్థిరరాశి UConverter_Unicode_సమితి which_సమితి,
                                 UError_సంకేతం* స్థితి_2) {
  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    తిరిగి;
  }

  int32_t columns = (ఫలితం->encodings_లెక్క+31)/32;

  // set errorValue to all-ones
  కొరకు (int32_t నిలువు = 0; నిలువు < columns; నిలువు++) {
    upvec_సమితి_విలువ(upvec, UPVEC_ERROR_విలువ_CP, UPVEC_ERROR_విలువ_CP,
                   నిలువు, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), స్థితి_2);
  }

  కొరకు (int32_t i = 0; i < ఫలితం->encodings_లెక్క; ++i) {
    uint32_t mask;
    uint32_t స్తంభం;
    int32_t అంశం_లెక్క;
    int32_t j;
    UConverter* test_converter = ucnv_open(ఫలితం->encodings[i], స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
      తిరిగి;
    }
    USet* unicode_బిందువు_సమితి;
    unicode_బిందువు_సమితి = uset_open(1, 0);  // empty set

    ucnv_get_Unicode_సమితి(test_converter, unicode_బిందువు_సమితి,
                       which_సమితి, స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
      ucnv_close(test_converter);
      తిరిగి;
    }

    స్తంభం = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    అంశం_లెక్క = uset_get_అంశం_లెక్క(unicode_బిందువు_సమితి);

    కొరకు (j = 0; j < అంశం_లెక్క; ++j) {
      UChar32 ప్రారంభం_char;
      UChar32 చివర_char;
      UError_సంకేతం small_స్థితి = U_ZERO_ERROR;
      uset_get_అంశం(unicode_బిందువు_సమితి, j, &ప్రారంభం_char, &చివర_char, శూన్యసూచిక, 0,
                   &small_స్థితి);
      ఒకవేళ (U_FAILURE(small_స్థితి)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } లేకపోతే {
        upvec_సమితి_విలువ(upvec, ప్రారంభం_char, చివర_char, స్తంభం, static_cast<uint32_t>(~0), mask,
                       స్థితి_2);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_బిందువు_సమితి);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
      తిరిగి;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  ఒకవేళ (excluded_సంకేతం_Points) {
    int32_t అంశం_లెక్క = uset_get_అంశం_లెక్క(excluded_సంకేతం_Points);
    కొరకు (int32_t j = 0; j < అంశం_లెక్క; ++j) {
      UChar32 ప్రారంభం_char;
      UChar32 చివర_char;

      uset_get_అంశం(excluded_సంకేతం_Points, j, &ప్రారంభం_char, &చివర_char, శూన్యసూచిక, 0,
                   స్థితి_2);
      కొరకు (int32_t నిలువు = 0; నిలువు < columns; నిలువు++) {
        upvec_సమితి_విలువ(upvec, ప్రారంభం_char, చివర_char, నిలువు, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      స్థితి_2);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  ఫలితం->trie = upvec_compact_గమ్యం_UTrie2_With_అడ్డువరుస_Indexes(upvec, స్థితి_2);
  ఫలితం->pv = upvec_cloneArray(upvec, &ఫలితం->pv_లెక్క, శూన్యసూచిక, స్థితి_2);
  ఫలితం->pv_లెక్క *= columns;  // number of uint32_t = rows * columns
  ఫలితం->ownPv = నిజం;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(స్థిరరాశి అక్షరం* స్థిరరాశి*  converterList, int32_t converter_List_కొలత,
             స్థిరరాశి USet* excluded_సంకేతం_Points,
             స్థిరరాశి UConverter_Unicode_సమితి which_సమితి, UError_సంకేతం* స్థితి_2) {
  // check if already failed
  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }
  // ensure args make sense!
  ఒకవేళ (converter_List_కొలత < 0 || (converterList == శూన్యసూచిక && converter_List_కొలత != 0)) {
    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    తిరిగి శూన్యసూచిక;
  }

  // allocate a new converter
  Local_UConverter_Selector_సూచిక newSelector(
    (UConverterSelector*)uprv_malloc(పరిమాణం(UConverterSelector)));
  ఒకవేళ (newSelector.isNull()) {
    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  uprv_memset(newSelector.getAlias(), 0, పరిమాణం(UConverterSelector));

  ఒకవేళ (converter_List_కొలత == 0) {
    converterList = శూన్యసూచిక;
    converter_List_కొలత = ucnv_లెక్క_అందుబాటు();
  }
  newSelector->encodings =
    (అక్షరం**)uprv_malloc(converter_List_కొలత * పరిమాణం(అక్షరం*));
  ఒకవేళ (!newSelector->encodings) {
    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  newSelector->encodings[0] = శూన్యసూచిక;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t మొత్తం_కొలత = 0;
  int32_t i;
  కొరకు (i = 0; i < converter_List_కొలత; i++) {
    మొత్తం_కొలత +=
      (int32_t)uprv_strlen(converterList != శూన్యసూచిక ? converterList[i] : ucnv_get_అందుబాటు_Name(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = మొత్తం_కొలత & 3;
  ఒకవేళ (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encoding_Str_నిడివి = మొత్తం_కొలత += encodingStrPadding;
  అక్షరం* all_అక్షరశ్రేణులు = (అక్షరం*) uprv_malloc(మొత్తం_కొలత);
  ఒకవేళ (!all_అక్షరశ్రేణులు) {
    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి శూన్యసూచిక;
  }

  కొరకు (i = 0; i < converter_List_కొలత; i++) {
    newSelector->encodings[i] = all_అక్షరశ్రేణులు;
    uprv_strcpy(newSelector->encodings[i],
                converterList != శూన్యసూచిక ? converterList[i] : ucnv_get_అందుబాటు_Name(i));
    all_అక్షరశ్రేణులు += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  అయితే (encodingStrPadding > 0) {
    *all_అక్షరశ్రేణులు++ = 0;
    --encodingStrPadding;
  }

  newSelector->own_Encoding_అక్షరశ్రేణులు = నిజం;
  newSelector->encodings_లెక్క = converter_List_కొలత;
  UPropsVectors *upvec = upvec_open((converter_List_కొలత+31)/32, స్థితి_2);
  generate_Selector_దత్తాంశం(newSelector.getAlias(), upvec, excluded_సంకేతం_Points, which_సమితి, స్థితి_2);
  upvec_close(upvec);

  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }

  తిరిగి newSelector.orphan();
}

/* close opened selector */
U_CAPI శూన్య U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  ఒకవేళ (!sel) {
    తిరిగి;
  }
  ఒకవేళ (sel->own_Encoding_అక్షరశ్రేణులు) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  ఒకవేళ (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

స్థిర స్థిరరాశి UDataInfo దత్తాంశం_Info = {
  పరిమాణం(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

గణన {
  UCNVSEL_సూచీ_TRIE_కొలత,      // trie size in bytes
  UCNVSEL_సూచీ_PV_లెక్క,       // number of uint32_t in the bit vectors
  UCNVSEL_సూచీ_పేర్లు_లెక్క,    // number of encoding names
  UCNVSEL_సూచీ_పేర్లు_నిడివి,   // number of encoding name bytes including padding
  UCNVSEL_సూచీ_కొలత = 15,      // bytes following the DataHeader
  UCNVSEL_సూచీ_లెక్క = 16
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
ucnvsel_serialize(స్థిరరాశి UConverterSelector* sel,
                  శూన్య* బఫరు, int32_t బఫరు_సామర్థ్యం, UError_సంకేతం* స్థితి_2) {
  // check if already failed
  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    తిరిగి 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)బఫరు;
  ఒకవేళ (బఫరు_సామర్థ్యం < 0 ||
      (బఫరు_సామర్థ్యం > 0 && (p == శూన్యసూచిక || (U_సూచిక_ముసుగు_LSB(p, 3) != 0)))
  ) {
    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    తిరిగి 0;
  }
  // add up the size of the serialized form
  int32_t serialized_Trie_కొలత = utrie2_serialize(sel->trie, శూన్యసూచిక, 0, స్థితి_2);
  ఒకవేళ (*స్థితి_2 != U_బఫరు_అధికప్రవాహం_ERROR && U_FAILURE(*స్థితి_2)) {
    తిరిగి 0;
  }
  *స్థితి_2 = U_ZERO_ERROR;

  దత్తాంశం_శీర్షిక శీర్షిక;
  uprv_memset(&శీర్షిక, 0, పరిమాణం(శీర్షిక));
  శీర్షిక.దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత = (uint16_t)((పరిమాణం(శీర్షిక) + 15) & ~15);
  శీర్షిక.దత్తాంశం_శీర్షిక_2.magic1 = 0xda;
  శీర్షిక.దత్తాంశం_శీర్షిక_2.magic2 = 0x27;
  uprv_memcpy(&శీర్షిక.info, &దత్తాంశం_Info, పరిమాణం(దత్తాంశం_Info));

  int32_t indexes[UCNVSEL_సూచీ_లెక్క] = {
    serialized_Trie_కొలత,
    sel->pv_లెక్క,
    sel->encodings_లెక్క,
    sel->encoding_Str_నిడివి
  };

  int32_t మొత్తం_కొలత =
    శీర్షిక.దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత +
    (int32_t)పరిమాణం(indexes) +
    serialized_Trie_కొలత +
    sel->pv_లెక్క * 4 +
    sel->encoding_Str_నిడివి;
  indexes[UCNVSEL_సూచీ_కొలత] = మొత్తం_కొలత - శీర్షిక.దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత;
  ఒకవేళ (మొత్తం_కొలత > బఫరు_సామర్థ్యం) {
    *స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
    తిరిగి మొత్తం_కొలత;
  }
  // ok, save!
  int32_t length = శీర్షిక.దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత;
  uprv_memcpy(p, &శీర్షిక, పరిమాణం(శీర్షిక));
  uprv_memset(p + పరిమాణం(శీర్షిక), 0, length - పరిమాణం(శీర్షిక));
  p += length;

  length = (int32_t)పరిమాణం(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serialized_Trie_కొలత, స్థితి_2);
  p += serialized_Trie_కొలత;

  length = sel->pv_లెక్క * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encoding_Str_నిడివి);
  p += sel->encoding_Str_నిడివి;

  తిరిగి మొత్తం_కొలత;
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
స్థిర int32_t
ucnvsel_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
             స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length,
             శూన్య *బహిర్గతం_దత్తాంశం, UError_సంకేతం *స్థితి_2) {
  /* udata_swapDataHeader checks the arguments */
  int32_t శీర్షిక_కొలత = udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, స్థితి_2);
  ఒకవేళ(U_FAILURE(*స్థితి_2)) {
    తిరిగి 0;
  }

  /* check data format and format version */
  స్థిరరాశి UDataInfo* pInfo = reinterpret_cast<స్థిరరాశి UDataInfo*>(static_cast<స్థిరరాశి అక్షరం*>(లోపల_దత్తాంశం) + 4);
  ఒకవేళ(!(
    pInfo->దత్తాంశం_Format[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->దత్తాంశం_Format[1] == 0x53 &&
    pInfo->దత్తాంశం_Format[2] == 0x65 &&
    pInfo->దత్తాంశం_Format[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->దత్తాంశం_Format[0], pInfo->దత్తాంశం_Format[1],
                     pInfo->దత్తాంశం_Format[2], pInfo->దత్తాంశం_Format[3]);
    *స్థితి_2 = U_INVALID_FORMAT_ERROR;
    తిరిగి 0;
  }
  ఒకవేళ(pInfo->format_సంస్కరణ[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->format_సంస్కరణ[0]);
    *స్థితి_2 = U_UNSUPPORTED_ERROR;
    తిరిగి 0;
  }

  ఒకవేళ(length >= 0) {
    length -= శీర్షిక_కొలత;
    ఒకవేళ(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *స్థితి_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
      తిరిగి 0;
    }
  }

  స్థిరరాశి uint8_t* లోపల_బైట్లు = static_cast<స్థిరరాశి uint8_t*>(లోపల_దత్తాంశం) + శీర్షిక_కొలత;
  uint8_t* బహిర్గతం_బైట్లు = static_cast<uint8_t*>(బహిర్గతం_దత్తాంశం) + శీర్షిక_కొలత;

  /* read the indexes */
  స్థిరరాశి int32_t* లోపల_Indexes = reinterpret_cast<స్థిరరాశి int32_t*>(లోపల_బైట్లు);
  int32_t indexes[16];
  int32_t i;
  కొరకు(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, లోపల_Indexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_సూచీ_కొలత];
  ఒకవేళ(length >= 0) {
    ఒకవేళ(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *స్థితి_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
      తిరిగి 0;
    }

    /* copy the data for inaccessible bytes */
    ఒకవేళ(లోపల_బైట్లు != బహిర్గతం_బైట్లు) {
      uprv_memcpy(బహిర్గతం_బైట్లు, లోపల_బైట్లు, size);
    }

    int32_t స్థానభ్రంశం = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_సూచీ_లెక్క*4;
    ds->మార్పిడి_Array32(ds, లోపల_బైట్లు, count, బహిర్గతం_బైట్లు, స్థితి_2);
    స్థానభ్రంశం += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_సూచీ_TRIE_కొలత];
    utrie2_మార్పిడి(ds, లోపల_బైట్లు + స్థానభ్రంశం, count, బహిర్గతం_బైట్లు + స్థానభ్రంశం, స్థితి_2);
    స్థానభ్రంశం += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_సూచీ_PV_లెక్క]*4;
    ds->మార్పిడి_Array32(ds, లోపల_బైట్లు + స్థానభ్రంశం, count, బహిర్గతం_బైట్లు + స్థానభ్రంశం, స్థితి_2);
    స్థానభ్రంశం += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_సూచీ_పేర్లు_నిడివి];
    ds->మార్పిడి_Inv_Chars(ds, లోపల_బైట్లు + స్థానభ్రంశం, count, బహిర్గతం_బైట్లు + స్థానభ్రంశం, స్థితి_2);
    స్థానభ్రంశం += count;

    U_ASSERT(స్థానభ్రంశం == size);
  }

  తిరిగి శీర్షిక_కొలత + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(స్థిరరాశి శూన్య* బఫరు U_LIFETIME_BOUND,
                           int32_t length,
                           UError_సంకేతం* స్థితి_2) {
  // check if already failed
  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }
  // ensure args make sense!
  స్థిరరాశి uint8_t *p = (స్థిరరాశి uint8_t *)బఫరు;
  ఒకవేళ (length <= 0 ||
      (length > 0 && (p == శూన్యసూచిక || (U_సూచిక_ముసుగు_LSB(p, 3) != 0)))
  ) {
    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  //శీర్షిక
  ఒకవేళ (length < 32) {
    // not even enough space for a minimal header
    *స్థితి_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  స్థిరరాశి దత్తాంశం_శీర్షిక *p_శీర్షిక = (స్థిరరాశి దత్తాంశం_శీర్షిక *)p;
  ఒకవేళ (!(
    p_శీర్షిక->దత్తాంశం_శీర్షిక_2.magic1==0xda &&
    p_శీర్షిక->దత్తాంశం_శీర్షిక_2.magic2==0x27 &&
    p_శీర్షిక->info.దత్తాంశం_Format[0] == 0x43 &&
    p_శీర్షిక->info.దత్తాంశం_Format[1] == 0x53 &&
    p_శీర్షిక->info.దత్తాంశం_Format[2] == 0x65 &&
    p_శీర్షిక->info.దత్తాంశం_Format[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *స్థితి_2 = U_INVALID_FORMAT_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  ఒకవేళ (p_శీర్షిక->info.format_సంస్కరణ[0] != 1) {
    *స్థితి_2 = U_UNSUPPORTED_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  uint8_t* swapped = శూన్యసూచిక;
  ఒకవేళ (p_శీర్షిక->info.isBigEndian != U_IS_BIG_ENDIAN ||
      p_శీర్షిక->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_ఆదానం_దత్తాంశం(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, స్థితి_2);
    int32_t మొత్తం_కొలత = ucnvsel_మార్పిడి(ds, p, -1, శూన్యసూచిక, స్థితి_2);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
      udata_closeSwapper(ds);
      తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (length < మొత్తం_కొలత) {
      udata_closeSwapper(ds);
      *స్థితి_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
      తిరిగి శూన్యసూచిక;
    }
    swapped = (uint8_t*)uprv_malloc(మొత్తం_కొలత);
    ఒకవేళ (swapped == శూన్యసూచిక) {
      udata_closeSwapper(ds);
      *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
      తిరిగి శూన్యసూచిక;
    }
    ucnvsel_మార్పిడి(ds, p, length, swapped, స్థితి_2);
    udata_closeSwapper(ds);
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
      uprv_free(swapped);
      తిరిగి శూన్యసూచిక;
    }
    p = swapped;
    p_శీర్షిక = (స్థిరరాశి దత్తాంశం_శీర్షిక *)p;
  }
  ఒకవేళ (length < (p_శీర్షిక->దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *స్థితి_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  p += p_శీర్షిక->దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత;
  length -= p_శీర్షిక->దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత;
  // indexes
  స్థిరరాశి int32_t *indexes = (స్థిరరాశి int32_t *)p;
  ఒకవేళ (length < indexes[UCNVSEL_సూచీ_కొలత]) {
    uprv_free(swapped);
    *స్థితి_2 = U_సూచీ_OUTOFBOUNDS_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  p += UCNVSEL_సూచీ_లెక్క * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(పరిమాణం(UConverterSelector));
  అక్షరం **encodings =
    (అక్షరం **)uprv_malloc(
      indexes[UCNVSEL_సూచీ_పేర్లు_లెక్క] * పరిమాణం(అక్షరం *));
  ఒకవేళ (sel == శూన్యసూచిక || encodings == శూన్యసూచిక) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  uprv_memset(sel, 0, పరిమాణం(UConverterSelector));
  sel->pv_లెక్క = indexes[UCNVSEL_సూచీ_PV_లెక్క];
  sel->encodings = encodings;
  sel->encodings_లెక్క = indexes[UCNVSEL_సూచీ_పేర్లు_లెక్క];
  sel->encoding_Str_నిడివి = indexes[UCNVSEL_సూచీ_పేర్లు_నిడివి];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_విలువ_బిట్లు,
                                        p, indexes[UCNVSEL_సూచీ_TRIE_కొలత], శూన్యసూచిక,
                                        స్థితి_2);
  p += indexes[UCNVSEL_సూచీ_TRIE_కొలత];
  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    ucnvsel_close(sel);
    తిరిగి శూన్యసూచిక;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_లెక్క * 4;
  // encoding names
  అక్షరం* s = (అక్షరం*)p;
  కొరకు (int32_t i = 0; i < sel->encodings_లెక్క; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encoding_Str_నిడివి;

  తిరిగి sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
నిర్మాణం Enumerator {
  int16_t* సూచీ;
  int16_t length;
  int16_t cur;
  స్థిరరాశి UConverterSelector* sel;
};

U_CDECL_BEGIN

స్థిర శూన్య U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->సూచీ);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


స్థిర int32_t U_CALLCONV
ucnvsel_లెక్క_encodings(UEnumeration *enumerator, UError_సంకేతం *స్థితి_2) {
  // check if already failed
  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    తిరిగి 0;
  }
  తిరిగి ((Enumerator*)(enumerator->context))->length;
}


స్థిర స్థిరరాశి అక్షరం* U_CALLCONV ucnvsel_తదుపరి_encoding(UEnumeration* enumerator,
                                                 int32_t* ఫలితం_నిడివి,
                                                 UError_సంకేతం* స్థితి_2) {
  // check if already failed
  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  స్థిరరాశి UConverterSelector* sel;
  స్థిరరాశి అక్షరం* ఫలితం;
  ఒకవేళ (cur >= ((Enumerator*)(enumerator->context))->length) {
    తిరిగి శూన్యసూచిక;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  ఫలితం = sel->encodings[((Enumerator*)(enumerator->context))->సూచీ[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  ఒకవేళ (ఫలితం_నిడివి) {
    *ఫలితం_నిడివి = (int32_t)uprv_strlen(ఫలితం);
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UError_సంకేతం* స్థితి_2) {
  // check if already failed
  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    తిరిగి ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_చివర


స్థిర స్థిరరాశి UEnumeration defaultEncodings = {
  శూన్యసూచిక,
    శూన్యసూచిక,
    ucnvsel_close_selector_iterator,
    ucnvsel_లెక్క_encodings,
    uenum_unextDefault,
    ucnvsel_తదుపరి_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
స్థిర UBool intersectMasks(uint32_t* dest, స్థిరరాశి uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  కొరకు (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  తిరిగి oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
స్థిర int16_t లెక్క_Ones(uint32_t* mask, int32_t len) {
  int32_t i, మొత్తం_Ones = 0;
  కొరకు (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    కొరకు (; ent; మొత్తం_Ones++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  తిరిగి static_cast<int16_t>(మొత్తం_Ones);
}


/* internal function! */
స్థిర UEnumeration*
select_For_ముసుగు(స్థిరరాశి UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* the_ముసుగు,
              UError_సంకేతం* స్థితి_2) {
  Local_జ్ఞాపకం<uint32_t> mask(the_ముసుగు);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  Local_జ్ఞాపకం<Enumerator> ఫలితం(static_cast<Enumerator *>(uprv_malloc(పరిమాణం(Enumerator))));
  ఒకవేళ (ఫలితం.isNull()) {
    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  ఫలితం->సూచీ = శూన్యసూచిక;  // this will be allocated later!
  ఫలితం->length = ఫలితం->cur = 0;
  ఫలితం->sel = sel;

  Local_జ్ఞాపకం<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(పరిమాణం(UEnumeration))));
  ఒకవేళ (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  జ్ఞాపకం_నకలు(en.getAlias(), &defaultEncodings, పరిమాణం(UEnumeration));
  
  int32_t columns = (sel->encodings_లెక్క+31)/32;
  int16_t అంకె_Ones = లెక్క_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  ఒకవేళ (అంకె_Ones > 0) {
    ఫలితం->సూచీ = static_cast<int16_t*>(uprv_malloc(అంకె_Ones * పరిమాణం(int16_t)));
    ఒకవేళ (ఫలితం->సూచీ == శూన్యసూచిక) {
      *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
      తిరిగి శూన్యసూచిక;
    }
    int32_t i, j;
    int16_t k = 0;
    కొరకు (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      కొరకు (i = 0 ; i < 32 && k < sel->encodings_లెక్క; i++, k++) {
        ఒకవేళ ((v & 1) != 0) {
          ఫలితం->సూచీ[ఫలితం->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = ఫలితం.orphan();
  తిరిగి en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(స్థిరరాశి UConverterSelector* sel U_LIFETIME_BOUND,
                        స్థిరరాశి char16_t* s,
                        int32_t length,
                        UError_సంకేతం* స్థితి_2) {
  // check if already failed
  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }
  // ensure args make sense!
  ఒకవేళ (sel == శూన్యసూచిక || (s == శూన్యసూచిక && length != 0)) {
    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    తిరిగి శూన్యసూచిక;
  }

  int32_t columns = (sel->encodings_లెక్క+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  ఒకవేళ (mask == శూన్యసూచిక) {
    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  uprv_memset(mask, ~0, columns *4);

  ఒకవేళ(s!=శూన్యసూచిక) {
    స్థిరరాశి char16_t *హద్దు;
    ఒకవేళ (length >= 0) {
      హద్దు = s + length;
    } లేకపోతే {
      హద్దు = శూన్యసూచిక;
    }
    
    అయితే (హద్దు == శూన్యసూచిక ? *s != 0 : s != హద్దు) {
      UChar32 c;
      uint16_t pv_సూచీ;
      UTRIE2_U16_NEXT16(sel->trie, s, హద్దు, c, pv_సూచీ);
      ఒకవేళ (intersectMasks(mask, sel->pv+pv_సూచీ, columns)) {
        ఆపు;
      }
    }
  }
  తిరిగి select_For_ముసుగు(sel, mask, స్థితి_2);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(స్థిరరాశి UConverterSelector* sel U_LIFETIME_BOUND,
                      స్థిరరాశి అక్షరం* s,
                      int32_t length,
                      UError_సంకేతం* స్థితి_2) {
  // check if already failed
  ఒకవేళ (U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }
  // ensure args make sense!
  ఒకవేళ (sel == శూన్యసూచిక || (s == శూన్యసూచిక && length != 0)) {
    *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    తిరిగి శూన్యసూచిక;
  }

  int32_t columns = (sel->encodings_లెక్క+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  ఒకవేళ (mask == శూన్యసూచిక) {
    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి శూన్యసూచిక;
  }
  uprv_memset(mask, ~0, columns *4);

  ఒకవేళ (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  ఒకవేళ(s!=శూన్యసూచిక) {
    స్థిరరాశి అక్షరం *హద్దు = s + length;
    
    అయితే (s != హద్దు) {
      uint16_t pv_సూచీ;
      UTRIE2_U8_NEXT16(sel->trie, s, హద్దు, pv_సూచీ);
      ఒకవేళ (intersectMasks(mask, sel->pv+pv_సూచీ, columns)) {
        ఆపు;
      }
    }
  }
  తిరిగి select_For_ముసుగు(sel, mask, స్థితి_2);
}

#అయితే_ముగింపు  // !UCONFIG_NO_CONVERSION
