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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnvsel.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchriter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "propsvec.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucmndata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uenumimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"

U_NAMESPACE_USE

ਬਣਤਰ UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_ਗਿਣਤੀ;
  ਅੱਖਰ** encodings;          // which encodings did user ask to use?
  int32_t encodings_ਗਿਣਤੀ;
  int32_t encoding_Str_ਲੰਮਾਈ;
  uint8_t* swapped;
  UBool ownPv, own_Encoding_ਲੜੀਆਂ;
};

ਸਥਿਰ ਖਾਲੀ generate_Selector_ਅੰਕੜੇ(UConverterSelector* ਨਤੀਜਾ,
                                 UPropsVectors *upvec,
                                 ਅਚਲ USet* excluded_ਕੋਡ_Points,
                                 ਅਚਲ UConverter_Unicode_ਸੈੱਟ which_ਸੈੱਟ,
                                 UError_ਕੋਡ* ਸਥਿਤੀ) {
  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ;
  }

  int32_t columns = (ਨਤੀਜਾ->encodings_ਗਿਣਤੀ+31)/32;

  // set errorValue to all-ones
  ਲਈ (int32_t ਖਾਨਾ = 0; ਖਾਨਾ < columns; ਖਾਨਾ++) {
    upvec_ਸੈੱਟ_ਮੁੱਲ(upvec, UPVEC_ERROR_ਮੁੱਲ_CP, UPVEC_ERROR_ਮੁੱਲ_CP,
                   ਖਾਨਾ, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), ਸਥਿਤੀ);
  }

  ਲਈ (int32_t i = 0; i < ਨਤੀਜਾ->encodings_ਗਿਣਤੀ; ++i) {
    uint32_t mask;
    uint32_t ਕਾਲਮ;
    int32_t ਵਸਤੂ_ਗਿਣਤੀ;
    int32_t j;
    UConverter* test_converter = ucnv_open(ਨਤੀਜਾ->encodings[i], ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
      ਵਾਪਸ;
    }
    USet* unicode_ਬਿੰਦੂ_ਸੈੱਟ;
    unicode_ਬਿੰਦੂ_ਸੈੱਟ = uset_open(1, 0);  // empty set

    ucnv_get_Unicode_ਸੈੱਟ(test_converter, unicode_ਬਿੰਦੂ_ਸੈੱਟ,
                       which_ਸੈੱਟ, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
      ucnv_close(test_converter);
      ਵਾਪਸ;
    }

    ਕਾਲਮ = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    ਵਸਤੂ_ਗਿਣਤੀ = uset_get_ਵਸਤੂ_ਗਿਣਤੀ(unicode_ਬਿੰਦੂ_ਸੈੱਟ);

    ਲਈ (j = 0; j < ਵਸਤੂ_ਗਿਣਤੀ; ++j) {
      UChar32 ਸ਼ੁਰੂ_char;
      UChar32 ਸਿਰਾ_char;
      UError_ਕੋਡ small_ਸਥਿਤੀ = U_ZERO_ERROR;
      uset_get_ਵਸਤੂ(unicode_ਬਿੰਦੂ_ਸੈੱਟ, j, &ਸ਼ੁਰੂ_char, &ਸਿਰਾ_char, ਖਾਲੀ_ਸੂਚਕ, 0,
                   &small_ਸਥਿਤੀ);
      ਜੇ (U_FAILURE(small_ਸਥਿਤੀ)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } ਨਹੀਂਤਾਂ {
        upvec_ਸੈੱਟ_ਮੁੱਲ(upvec, ਸ਼ੁਰੂ_char, ਸਿਰਾ_char, ਕਾਲਮ, static_cast<uint32_t>(~0), mask,
                       ਸਥਿਤੀ);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_ਬਿੰਦੂ_ਸੈੱਟ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
      ਵਾਪਸ;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  ਜੇ (excluded_ਕੋਡ_Points) {
    int32_t ਵਸਤੂ_ਗਿਣਤੀ = uset_get_ਵਸਤੂ_ਗਿਣਤੀ(excluded_ਕੋਡ_Points);
    ਲਈ (int32_t j = 0; j < ਵਸਤੂ_ਗਿਣਤੀ; ++j) {
      UChar32 ਸ਼ੁਰੂ_char;
      UChar32 ਸਿਰਾ_char;

      uset_get_ਵਸਤੂ(excluded_ਕੋਡ_Points, j, &ਸ਼ੁਰੂ_char, &ਸਿਰਾ_char, ਖਾਲੀ_ਸੂਚਕ, 0,
                   ਸਥਿਤੀ);
      ਲਈ (int32_t ਖਾਨਾ = 0; ਖਾਨਾ < columns; ਖਾਨਾ++) {
        upvec_ਸੈੱਟ_ਮੁੱਲ(upvec, ਸ਼ੁਰੂ_char, ਸਿਰਾ_char, ਖਾਨਾ, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      ਸਥਿਤੀ);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  ਨਤੀਜਾ->trie = upvec_compact_ਮੰਜ਼ਿਲ_UTrie2_With_ਪੰਕਤੀ_Indexes(upvec, ਸਥਿਤੀ);
  ਨਤੀਜਾ->pv = upvec_cloneArray(upvec, &ਨਤੀਜਾ->pv_ਗਿਣਤੀ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
  ਨਤੀਜਾ->pv_ਗਿਣਤੀ *= columns;  // number of uint32_t = rows * columns
  ਨਤੀਜਾ->ownPv = ਸੱਚ;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ਅਚਲ ਅੱਖਰ* ਅਚਲ*  converterList, int32_t converter_List_ਮਾਪ,
             ਅਚਲ USet* excluded_ਕੋਡ_Points,
             ਅਚਲ UConverter_Unicode_ਸੈੱਟ which_ਸੈੱਟ, UError_ਕੋਡ* ਸਥਿਤੀ) {
  // check if already failed
  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  // ensure args make sense!
  ਜੇ (converter_List_ਮਾਪ < 0 || (converterList == ਖਾਲੀ_ਸੂਚਕ && converter_List_ਮਾਪ != 0)) {
    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  // allocate a new converter
  Local_UConverter_Selector_ਸੂਚਕ newSelector(
    (UConverterSelector*)uprv_malloc(ਆਕਾਰ(UConverterSelector)));
  ਜੇ (newSelector.isNull()) {
    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  uprv_memset(newSelector.getAlias(), 0, ਆਕਾਰ(UConverterSelector));

  ਜੇ (converter_List_ਮਾਪ == 0) {
    converterList = ਖਾਲੀ_ਸੂਚਕ;
    converter_List_ਮਾਪ = ucnv_ਗਿਣਤੀ_ਉਪਲਬਧ();
  }
  newSelector->encodings =
    (ਅੱਖਰ**)uprv_malloc(converter_List_ਮਾਪ * ਆਕਾਰ(ਅੱਖਰ*));
  ਜੇ (!newSelector->encodings) {
    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  newSelector->encodings[0] = ਖਾਲੀ_ਸੂਚਕ;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t ਕੁੱਲ_ਮਾਪ = 0;
  int32_t i;
  ਲਈ (i = 0; i < converter_List_ਮਾਪ; i++) {
    ਕੁੱਲ_ਮਾਪ +=
      (int32_t)uprv_strlen(converterList != ਖਾਲੀ_ਸੂਚਕ ? converterList[i] : ucnv_get_ਉਪਲਬਧ_Name(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = ਕੁੱਲ_ਮਾਪ & 3;
  ਜੇ (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encoding_Str_ਲੰਮਾਈ = ਕੁੱਲ_ਮਾਪ += encodingStrPadding;
  ਅੱਖਰ* all_ਲੜੀਆਂ = (ਅੱਖਰ*) uprv_malloc(ਕੁੱਲ_ਮਾਪ);
  ਜੇ (!all_ਲੜੀਆਂ) {
    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  ਲਈ (i = 0; i < converter_List_ਮਾਪ; i++) {
    newSelector->encodings[i] = all_ਲੜੀਆਂ;
    uprv_strcpy(newSelector->encodings[i],
                converterList != ਖਾਲੀ_ਸੂਚਕ ? converterList[i] : ucnv_get_ਉਪਲਬਧ_Name(i));
    all_ਲੜੀਆਂ += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  ਜਦੋਂ (encodingStrPadding > 0) {
    *all_ਲੜੀਆਂ++ = 0;
    --encodingStrPadding;
  }

  newSelector->own_Encoding_ਲੜੀਆਂ = ਸੱਚ;
  newSelector->encodings_ਗਿਣਤੀ = converter_List_ਮਾਪ;
  UPropsVectors *upvec = upvec_open((converter_List_ਮਾਪ+31)/32, ਸਥਿਤੀ);
  generate_Selector_ਅੰਕੜੇ(newSelector.getAlias(), upvec, excluded_ਕੋਡ_Points, which_ਸੈੱਟ, ਸਥਿਤੀ);
  upvec_close(upvec);

  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  ਵਾਪਸ newSelector.orphan();
}

/* close opened selector */
U_CAPI ਖਾਲੀ U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  ਜੇ (!sel) {
    ਵਾਪਸ;
  }
  ਜੇ (sel->own_Encoding_ਲੜੀਆਂ) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  ਜੇ (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

ਸਥਿਰ ਅਚਲ UDataInfo ਅੰਕੜੇ_Info = {
  ਆਕਾਰ(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

ਗਿਣਤੀ {
  UCNVSEL_ਸੂਚਕਾਂਕ_TRIE_ਮਾਪ,      // trie size in bytes
  UCNVSEL_ਸੂਚਕਾਂਕ_PV_ਗਿਣਤੀ,       // number of uint32_t in the bit vectors
  UCNVSEL_ਸੂਚਕਾਂਕ_ਨਾਂ_ਗਿਣਤੀ,    // number of encoding names
  UCNVSEL_ਸੂਚਕਾਂਕ_ਨਾਂ_ਲੰਮਾਈ,   // number of encoding name bytes including padding
  UCNVSEL_ਸੂਚਕਾਂਕ_ਮਾਪ = 15,      // bytes following the DataHeader
  UCNVSEL_ਸੂਚਕਾਂਕ_ਗਿਣਤੀ = 16
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
ucnvsel_serialize(ਅਚਲ UConverterSelector* sel,
                  ਖਾਲੀ* ਬਫਰ, int32_t ਬਫਰ_ਸਮਰੱਥਾ, UError_ਕੋਡ* ਸਥਿਤੀ) {
  // check if already failed
  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)ਬਫਰ;
  ਜੇ (ਬਫਰ_ਸਮਰੱਥਾ < 0 ||
      (ਬਫਰ_ਸਮਰੱਥਾ > 0 && (p == ਖਾਲੀ_ਸੂਚਕ || (U_ਸੂਚਕ_ਨਕਾਬ_LSB(p, 3) != 0)))
  ) {
    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ 0;
  }
  // add up the size of the serialized form
  int32_t serialized_Trie_ਮਾਪ = utrie2_serialize(sel->trie, ਖਾਲੀ_ਸੂਚਕ, 0, ਸਥਿਤੀ);
  ਜੇ (*ਸਥਿਤੀ != U_ਬਫਰ_ਉਛਾਲ_ERROR && U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ 0;
  }
  *ਸਥਿਤੀ = U_ZERO_ERROR;

  ਅੰਕੜੇ_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;
  uprv_memset(&ਸਿਰਲੇਖ, 0, ਆਕਾਰ(ਸਿਰਲੇਖ));
  ਸਿਰਲੇਖ.ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ = (uint16_t)((ਆਕਾਰ(ਸਿਰਲੇਖ) + 15) & ~15);
  ਸਿਰਲੇਖ.ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic1 = 0xda;
  ਸਿਰਲੇਖ.ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic2 = 0x27;
  uprv_memcpy(&ਸਿਰਲੇਖ.info, &ਅੰਕੜੇ_Info, ਆਕਾਰ(ਅੰਕੜੇ_Info));

  int32_t indexes[UCNVSEL_ਸੂਚਕਾਂਕ_ਗਿਣਤੀ] = {
    serialized_Trie_ਮਾਪ,
    sel->pv_ਗਿਣਤੀ,
    sel->encodings_ਗਿਣਤੀ,
    sel->encoding_Str_ਲੰਮਾਈ
  };

  int32_t ਕੁੱਲ_ਮਾਪ =
    ਸਿਰਲੇਖ.ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ +
    (int32_t)ਆਕਾਰ(indexes) +
    serialized_Trie_ਮਾਪ +
    sel->pv_ਗਿਣਤੀ * 4 +
    sel->encoding_Str_ਲੰਮਾਈ;
  indexes[UCNVSEL_ਸੂਚਕਾਂਕ_ਮਾਪ] = ਕੁੱਲ_ਮਾਪ - ਸਿਰਲੇਖ.ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ;
  ਜੇ (ਕੁੱਲ_ਮਾਪ > ਬਫਰ_ਸਮਰੱਥਾ) {
    *ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
    ਵਾਪਸ ਕੁੱਲ_ਮਾਪ;
  }
  // ok, save!
  int32_t length = ਸਿਰਲੇਖ.ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ;
  uprv_memcpy(p, &ਸਿਰਲੇਖ, ਆਕਾਰ(ਸਿਰਲੇਖ));
  uprv_memset(p + ਆਕਾਰ(ਸਿਰਲੇਖ), 0, length - ਆਕਾਰ(ਸਿਰਲੇਖ));
  p += length;

  length = (int32_t)ਆਕਾਰ(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serialized_Trie_ਮਾਪ, ਸਥਿਤੀ);
  p += serialized_Trie_ਮਾਪ;

  length = sel->pv_ਗਿਣਤੀ * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encoding_Str_ਲੰਮਾਈ);
  p += sel->encoding_Str_ਲੰਮਾਈ;

  ਵਾਪਸ ਕੁੱਲ_ਮਾਪ;
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
ਸਥਿਰ int32_t
ucnvsel_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
             ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length,
             ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ, UError_ਕੋਡ *ਸਥਿਤੀ) {
  /* udata_swapDataHeader checks the arguments */
  int32_t ਸਿਰਲੇਖ_ਮਾਪ = udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, ਸਥਿਤੀ);
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ 0;
  }

  /* check data format and format version */
  ਅਚਲ UDataInfo* pInfo = reinterpret_cast<ਅਚਲ UDataInfo*>(static_cast<ਅਚਲ ਅੱਖਰ*>(ਵਿੱਚ_ਅੰਕੜੇ) + 4);
  ਜੇ(!(
    pInfo->ਅੰਕੜੇ_Format[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->ਅੰਕੜੇ_Format[1] == 0x53 &&
    pInfo->ਅੰਕੜੇ_Format[2] == 0x65 &&
    pInfo->ਅੰਕੜੇ_Format[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->ਅੰਕੜੇ_Format[0], pInfo->ਅੰਕੜੇ_Format[1],
                     pInfo->ਅੰਕੜੇ_Format[2], pInfo->ਅੰਕੜੇ_Format[3]);
    *ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
    ਵਾਪਸ 0;
  }
  ਜੇ(pInfo->format_ਸੰਸਕਰਣ[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->format_ਸੰਸਕਰਣ[0]);
    *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
    ਵਾਪਸ 0;
  }

  ਜੇ(length >= 0) {
    length -= ਸਿਰਲੇਖ_ਮਾਪ;
    ਜੇ(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *ਸਥਿਤੀ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
      ਵਾਪਸ 0;
    }
  }

  ਅਚਲ uint8_t* ਵਿੱਚ_ਬਾਈਟਾਂ = static_cast<ਅਚਲ uint8_t*>(ਵਿੱਚ_ਅੰਕੜੇ) + ਸਿਰਲੇਖ_ਮਾਪ;
  uint8_t* ਬਾਹਰਲਾ_ਬਾਈਟਾਂ = static_cast<uint8_t*>(ਬਾਹਰਲਾ_ਅੰਕੜੇ) + ਸਿਰਲੇਖ_ਮਾਪ;

  /* read the indexes */
  ਅਚਲ int32_t* ਵਿੱਚ_Indexes = reinterpret_cast<ਅਚਲ int32_t*>(ਵਿੱਚ_ਬਾਈਟਾਂ);
  int32_t indexes[16];
  int32_t i;
  ਲਈ(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, ਵਿੱਚ_Indexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_ਸੂਚਕਾਂਕ_ਮਾਪ];
  ਜੇ(length >= 0) {
    ਜੇ(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *ਸਥਿਤੀ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
      ਵਾਪਸ 0;
    }

    /* copy the data for inaccessible bytes */
    ਜੇ(ਵਿੱਚ_ਬਾਈਟਾਂ != ਬਾਹਰਲਾ_ਬਾਈਟਾਂ) {
      uprv_memcpy(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, ਵਿੱਚ_ਬਾਈਟਾਂ, size);
    }

    int32_t ਵਿੱਥ = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_ਸੂਚਕਾਂਕ_ਗਿਣਤੀ*4;
    ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ, count, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, ਸਥਿਤੀ);
    ਵਿੱਥ += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_ਸੂਚਕਾਂਕ_TRIE_ਮਾਪ];
    utrie2_ਅਦਲਾ_ਬਦਲੀ(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, count, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, ਸਥਿਤੀ);
    ਵਿੱਥ += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_ਸੂਚਕਾਂਕ_PV_ਗਿਣਤੀ]*4;
    ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, count, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, ਸਥਿਤੀ);
    ਵਿੱਥ += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_ਸੂਚਕਾਂਕ_ਨਾਂ_ਲੰਮਾਈ];
    ds->ਅਦਲਾ_ਬਦਲੀ_Inv_Chars(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, count, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, ਸਥਿਤੀ);
    ਵਿੱਥ += count;

    U_ASSERT(ਵਿੱਥ == size);
  }

  ਵਾਪਸ ਸਿਰਲੇਖ_ਮਾਪ + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ਅਚਲ ਖਾਲੀ* ਬਫਰ U_LIFETIME_BOUND,
                           int32_t length,
                           UError_ਕੋਡ* ਸਥਿਤੀ) {
  // check if already failed
  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  // ensure args make sense!
  ਅਚਲ uint8_t *p = (ਅਚਲ uint8_t *)ਬਫਰ;
  ਜੇ (length <= 0 ||
      (length > 0 && (p == ਖਾਲੀ_ਸੂਚਕ || (U_ਸੂਚਕ_ਨਕਾਬ_LSB(p, 3) != 0)))
  ) {
    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  //ਸਿਰਲੇਖ
  ਜੇ (length < 32) {
    // not even enough space for a minimal header
    *ਸਥਿਤੀ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *p_ਸਿਰਲੇਖ = (ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *)p;
  ਜੇ (!(
    p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic1==0xda &&
    p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic2==0x27 &&
    p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[0] == 0x43 &&
    p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[1] == 0x53 &&
    p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[2] == 0x65 &&
    p_ਸਿਰਲੇਖ->info.ਅੰਕੜੇ_Format[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  ਜੇ (p_ਸਿਰਲੇਖ->info.format_ਸੰਸਕਰਣ[0] != 1) {
    *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  uint8_t* swapped = ਖਾਲੀ_ਸੂਚਕ;
  ਜੇ (p_ਸਿਰਲੇਖ->info.isBigEndian != U_IS_BIG_ENDIAN ||
      p_ਸਿਰਲੇਖ->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_ਨਿਵੇਸ਼_ਅੰਕੜੇ(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, ਸਥਿਤੀ);
    int32_t ਕੁੱਲ_ਮਾਪ = ucnvsel_ਅਦਲਾ_ਬਦਲੀ(ds, p, -1, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
      udata_closeSwapper(ds);
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (length < ਕੁੱਲ_ਮਾਪ) {
      udata_closeSwapper(ds);
      *ਸਥਿਤੀ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    swapped = (uint8_t*)uprv_malloc(ਕੁੱਲ_ਮਾਪ);
    ਜੇ (swapped == ਖਾਲੀ_ਸੂਚਕ) {
      udata_closeSwapper(ds);
      *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ucnvsel_ਅਦਲਾ_ਬਦਲੀ(ds, p, length, swapped, ਸਥਿਤੀ);
    udata_closeSwapper(ds);
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
      uprv_free(swapped);
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    p = swapped;
    p_ਸਿਰਲੇਖ = (ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *)p;
  }
  ਜੇ (length < (p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *ਸਥਿਤੀ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  p += p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ;
  length -= p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ;
  // indexes
  ਅਚਲ int32_t *indexes = (ਅਚਲ int32_t *)p;
  ਜੇ (length < indexes[UCNVSEL_ਸੂਚਕਾਂਕ_ਮਾਪ]) {
    uprv_free(swapped);
    *ਸਥਿਤੀ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  p += UCNVSEL_ਸੂਚਕਾਂਕ_ਗਿਣਤੀ * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(ਆਕਾਰ(UConverterSelector));
  ਅੱਖਰ **encodings =
    (ਅੱਖਰ **)uprv_malloc(
      indexes[UCNVSEL_ਸੂਚਕਾਂਕ_ਨਾਂ_ਗਿਣਤੀ] * ਆਕਾਰ(ਅੱਖਰ *));
  ਜੇ (sel == ਖਾਲੀ_ਸੂਚਕ || encodings == ਖਾਲੀ_ਸੂਚਕ) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  uprv_memset(sel, 0, ਆਕਾਰ(UConverterSelector));
  sel->pv_ਗਿਣਤੀ = indexes[UCNVSEL_ਸੂਚਕਾਂਕ_PV_ਗਿਣਤੀ];
  sel->encodings = encodings;
  sel->encodings_ਗਿਣਤੀ = indexes[UCNVSEL_ਸੂਚਕਾਂਕ_ਨਾਂ_ਗਿਣਤੀ];
  sel->encoding_Str_ਲੰਮਾਈ = indexes[UCNVSEL_ਸੂਚਕਾਂਕ_ਨਾਂ_ਲੰਮਾਈ];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ,
                                        p, indexes[UCNVSEL_ਸੂਚਕਾਂਕ_TRIE_ਮਾਪ], ਖਾਲੀ_ਸੂਚਕ,
                                        ਸਥਿਤੀ);
  p += indexes[UCNVSEL_ਸੂਚਕਾਂਕ_TRIE_ਮਾਪ];
  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ucnvsel_close(sel);
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_ਗਿਣਤੀ * 4;
  // encoding names
  ਅੱਖਰ* s = (ਅੱਖਰ*)p;
  ਲਈ (int32_t i = 0; i < sel->encodings_ਗਿਣਤੀ; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encoding_Str_ਲੰਮਾਈ;

  ਵਾਪਸ sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
ਬਣਤਰ Enumerator {
  int16_t* ਸੂਚਕਾਂਕ;
  int16_t length;
  int16_t cur;
  ਅਚਲ UConverterSelector* sel;
};

U_CDECL_BEGIN

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->ਸੂਚਕਾਂਕ);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


ਸਥਿਰ int32_t U_CALLCONV
ucnvsel_ਗਿਣਤੀ_encodings(UEnumeration *enumerator, UError_ਕੋਡ *ਸਥਿਤੀ) {
  // check if already failed
  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ ((Enumerator*)(enumerator->context))->length;
}


ਸਥਿਰ ਅਚਲ ਅੱਖਰ* U_CALLCONV ucnvsel_ਅਗਲਾ_encoding(UEnumeration* enumerator,
                                                 int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
                                                 UError_ਕੋਡ* ਸਥਿਤੀ) {
  // check if already failed
  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ਅਚਲ UConverterSelector* sel;
  ਅਚਲ ਅੱਖਰ* ਨਤੀਜਾ;
  ਜੇ (cur >= ((Enumerator*)(enumerator->context))->length) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  ਨਤੀਜਾ = sel->encodings[((Enumerator*)(enumerator->context))->ਸੂਚਕਾਂਕ[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
    *ਨਤੀਜਾ_ਲੰਮਾਈ = (int32_t)uprv_strlen(ਨਤੀਜਾ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UError_ਕੋਡ* ਸਥਿਤੀ) {
  // check if already failed
  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_ਸਿਰਾ


ਸਥਿਰ ਅਚਲ UEnumeration defaultEncodings = {
  ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnvsel_close_selector_iterator,
    ucnvsel_ਗਿਣਤੀ_encodings,
    uenum_unextDefault,
    ucnvsel_ਅਗਲਾ_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
ਸਥਿਰ UBool intersectMasks(uint32_t* dest, ਅਚਲ uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  ਲਈ (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  ਵਾਪਸ oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
ਸਥਿਰ int16_t ਗਿਣਤੀ_Ones(uint32_t* mask, int32_t len) {
  int32_t i, ਕੁੱਲ_Ones = 0;
  ਲਈ (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    ਲਈ (; ent; ਕੁੱਲ_Ones++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  ਵਾਪਸ static_cast<int16_t>(ਕੁੱਲ_Ones);
}


/* internal function! */
ਸਥਿਰ UEnumeration*
select_For_ਨਕਾਬ(ਅਚਲ UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* the_ਨਕਾਬ,
              UError_ਕੋਡ* ਸਥਿਤੀ) {
  Local_ਸਮ੍ਰਿਤੀ<uint32_t> mask(the_ਨਕਾਬ);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  Local_ਸਮ੍ਰਿਤੀ<Enumerator> ਨਤੀਜਾ(static_cast<Enumerator *>(uprv_malloc(ਆਕਾਰ(Enumerator))));
  ਜੇ (ਨਤੀਜਾ.isNull()) {
    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  ਨਤੀਜਾ->ਸੂਚਕਾਂਕ = ਖਾਲੀ_ਸੂਚਕ;  // this will be allocated later!
  ਨਤੀਜਾ->length = ਨਤੀਜਾ->cur = 0;
  ਨਤੀਜਾ->sel = sel;

  Local_ਸਮ੍ਰਿਤੀ<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(ਆਕਾਰ(UEnumeration))));
  ਜੇ (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(en.getAlias(), &defaultEncodings, ਆਕਾਰ(UEnumeration));
  
  int32_t columns = (sel->encodings_ਗਿਣਤੀ+31)/32;
  int16_t ਅੰਕ_Ones = ਗਿਣਤੀ_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  ਜੇ (ਅੰਕ_Ones > 0) {
    ਨਤੀਜਾ->ਸੂਚਕਾਂਕ = static_cast<int16_t*>(uprv_malloc(ਅੰਕ_Ones * ਆਕਾਰ(int16_t)));
    ਜੇ (ਨਤੀਜਾ->ਸੂਚਕਾਂਕ == ਖਾਲੀ_ਸੂਚਕ) {
      *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    int32_t i, j;
    int16_t k = 0;
    ਲਈ (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      ਲਈ (i = 0 ; i < 32 && k < sel->encodings_ਗਿਣਤੀ; i++, k++) {
        ਜੇ ((v & 1) != 0) {
          ਨਤੀਜਾ->ਸੂਚਕਾਂਕ[ਨਤੀਜਾ->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = ਨਤੀਜਾ.orphan();
  ਵਾਪਸ en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ਅਚਲ UConverterSelector* sel U_LIFETIME_BOUND,
                        ਅਚਲ char16_t* s,
                        int32_t length,
                        UError_ਕੋਡ* ਸਥਿਤੀ) {
  // check if already failed
  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  // ensure args make sense!
  ਜੇ (sel == ਖਾਲੀ_ਸੂਚਕ || (s == ਖਾਲੀ_ਸੂਚਕ && length != 0)) {
    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  int32_t columns = (sel->encodings_ਗਿਣਤੀ+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  ਜੇ (mask == ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  uprv_memset(mask, ~0, columns *4);

  ਜੇ(s!=ਖਾਲੀ_ਸੂਚਕ) {
    ਅਚਲ char16_t *ਹੱਦਬੰਦੀ;
    ਜੇ (length >= 0) {
      ਹੱਦਬੰਦੀ = s + length;
    } ਨਹੀਂਤਾਂ {
      ਹੱਦਬੰਦੀ = ਖਾਲੀ_ਸੂਚਕ;
    }
    
    ਜਦੋਂ (ਹੱਦਬੰਦੀ == ਖਾਲੀ_ਸੂਚਕ ? *s != 0 : s != ਹੱਦਬੰਦੀ) {
      UChar32 c;
      uint16_t pv_ਸੂਚਕਾਂਕ;
      UTRIE2_U16_NEXT16(sel->trie, s, ਹੱਦਬੰਦੀ, c, pv_ਸੂਚਕਾਂਕ);
      ਜੇ (intersectMasks(mask, sel->pv+pv_ਸੂਚਕਾਂਕ, columns)) {
        ਰੋਕੋ;
      }
    }
  }
  ਵਾਪਸ select_For_ਨਕਾਬ(sel, mask, ਸਥਿਤੀ);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ਅਚਲ UConverterSelector* sel U_LIFETIME_BOUND,
                      ਅਚਲ ਅੱਖਰ* s,
                      int32_t length,
                      UError_ਕੋਡ* ਸਥਿਤੀ) {
  // check if already failed
  ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  // ensure args make sense!
  ਜੇ (sel == ਖਾਲੀ_ਸੂਚਕ || (s == ਖਾਲੀ_ਸੂਚਕ && length != 0)) {
    *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  int32_t columns = (sel->encodings_ਗਿਣਤੀ+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  ਜੇ (mask == ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  uprv_memset(mask, ~0, columns *4);

  ਜੇ (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  ਜੇ(s!=ਖਾਲੀ_ਸੂਚਕ) {
    ਅਚਲ ਅੱਖਰ *ਹੱਦਬੰਦੀ = s + length;
    
    ਜਦੋਂ (s != ਹੱਦਬੰਦੀ) {
      uint16_t pv_ਸੂਚਕਾਂਕ;
      UTRIE2_U8_NEXT16(sel->trie, s, ਹੱਦਬੰਦੀ, pv_ਸੂਚਕਾਂਕ);
      ਜੇ (intersectMasks(mask, sel->pv+pv_ਸੂਚਕਾਂਕ, columns)) {
        ਰੋਕੋ;
      }
    }
  }
  ਵਾਪਸ select_For_ਨਕਾਬ(sel, mask, ਸਥਿਤੀ);
}

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_CONVERSION
