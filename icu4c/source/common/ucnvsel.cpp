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

#ಆದರೆ !UCONFIG_NO_CONVERSION

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

ರಚನೆ UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_ಎಣಿಕೆ;
  ಅಕ್ಷರ** encodings;          // which encodings did user ask to use?
  int32_t encodings_ಎಣಿಕೆ;
  int32_t encoding_Str_ಉದ್ದಳತೆ;
  uint8_t* swapped;
  UBool ownPv, own_Encoding_ಅಕ್ಷರಸರಣಿಗಳು;
};

ಸ್ಥಿರ ಶೂನ್ಯ generate_Selector_ದತ್ತಾಂಶ(UConverterSelector* ಫಲಿತಾಂಶ,
                                 UPropsVectors *upvec,
                                 ಸ್ಥಿರಾಂಕ USet* excluded_ಸಂಕೇತ_Points,
                                 ಸ್ಥಿರಾಂಕ UConverter_Unicode_ಗಣ which_ಗಣ,
                                 UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ;
  }

  int32_t columns = (ಫಲಿತಾಂಶ->encodings_ಎಣಿಕೆ+31)/32;

  // set errorValue to all-ones
  ಗಾಗಿ (int32_t ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < columns; ಲಂಬಸಾಲು++) {
    upvec_ಗಣ_ಮೌಲ್ಯ(upvec, UPVEC_ERROR_ಮೌಲ್ಯ_CP, UPVEC_ERROR_ಮೌಲ್ಯ_CP,
                   ಲಂಬಸಾಲು, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), ಸ್ಥಿತಿ_2);
  }

  ಗಾಗಿ (int32_t i = 0; i < ಫಲಿತಾಂಶ->encodings_ಎಣಿಕೆ; ++i) {
    uint32_t mask;
    uint32_t ಸ್ತಂಭ;
    int32_t ಅಂಶ_ಎಣಿಕೆ;
    int32_t j;
    UConverter* test_converter = ucnv_open(ಫಲಿತಾಂಶ->encodings[i], ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
      ಹಿಂತಿರುಗಿ;
    }
    USet* unicode_ಬಿಂದು_ಗಣ;
    unicode_ಬಿಂದು_ಗಣ = uset_open(1, 0);  // empty set

    ucnv_get_Unicode_ಗಣ(test_converter, unicode_ಬಿಂದು_ಗಣ,
                       which_ಗಣ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
      ucnv_close(test_converter);
      ಹಿಂತಿರುಗಿ;
    }

    ಸ್ತಂಭ = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    ಅಂಶ_ಎಣಿಕೆ = uset_get_ಅಂಶ_ಎಣಿಕೆ(unicode_ಬಿಂದು_ಗಣ);

    ಗಾಗಿ (j = 0; j < ಅಂಶ_ಎಣಿಕೆ; ++j) {
      UChar32 ಆರಂಭ_char;
      UChar32 ಕೊನೆ_char;
      UError_ಸಂಕೇತ small_ಸ್ಥಿತಿ = U_ZERO_ERROR;
      uset_get_ಅಂಶ(unicode_ಬಿಂದು_ಗಣ, j, &ಆರಂಭ_char, &ಕೊನೆ_char, ಶೂನ್ಯಸೂಚಕ, 0,
                   &small_ಸ್ಥಿತಿ);
      ಆದರೆ (U_FAILURE(small_ಸ್ಥಿತಿ)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } ಇಲ್ಲದಿದ್ದರೆ {
        upvec_ಗಣ_ಮೌಲ್ಯ(upvec, ಆರಂಭ_char, ಕೊನೆ_char, ಸ್ತಂಭ, static_cast<uint32_t>(~0), mask,
                       ಸ್ಥಿತಿ_2);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_ಬಿಂದು_ಗಣ);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
      ಹಿಂತಿರುಗಿ;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  ಆದರೆ (excluded_ಸಂಕೇತ_Points) {
    int32_t ಅಂಶ_ಎಣಿಕೆ = uset_get_ಅಂಶ_ಎಣಿಕೆ(excluded_ಸಂಕೇತ_Points);
    ಗಾಗಿ (int32_t j = 0; j < ಅಂಶ_ಎಣಿಕೆ; ++j) {
      UChar32 ಆರಂಭ_char;
      UChar32 ಕೊನೆ_char;

      uset_get_ಅಂಶ(excluded_ಸಂಕೇತ_Points, j, &ಆರಂಭ_char, &ಕೊನೆ_char, ಶೂನ್ಯಸೂಚಕ, 0,
                   ಸ್ಥಿತಿ_2);
      ಗಾಗಿ (int32_t ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < columns; ಲಂಬಸಾಲು++) {
        upvec_ಗಣ_ಮೌಲ್ಯ(upvec, ಆರಂಭ_char, ಕೊನೆ_char, ಲಂಬಸಾಲು, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      ಸ್ಥಿತಿ_2);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  ಫಲಿತಾಂಶ->trie = upvec_compact_ಗಮ್ಯ_UTrie2_With_ಅಡ್ಡಸಾಲು_Indexes(upvec, ಸ್ಥಿತಿ_2);
  ಫಲಿತಾಂಶ->pv = upvec_cloneArray(upvec, &ಫಲಿತಾಂಶ->pv_ಎಣಿಕೆ, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
  ಫಲಿತಾಂಶ->pv_ಎಣಿಕೆ *= columns;  // number of uint32_t = rows * columns
  ಫಲಿತಾಂಶ->ownPv = ನಿಜ;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ*  converterList, int32_t converter_List_ಗಾತ್ರ,
             ಸ್ಥಿರಾಂಕ USet* excluded_ಸಂಕೇತ_Points,
             ಸ್ಥಿರಾಂಕ UConverter_Unicode_ಗಣ which_ಗಣ, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  // check if already failed
  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  // ensure args make sense!
  ಆದರೆ (converter_List_ಗಾತ್ರ < 0 || (converterList == ಶೂನ್ಯಸೂಚಕ && converter_List_ಗಾತ್ರ != 0)) {
    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  // allocate a new converter
  Local_UConverter_Selector_ಸೂಚಕ newSelector(
    (UConverterSelector*)uprv_malloc(ಗಾತ್ರ(UConverterSelector)));
  ಆದರೆ (newSelector.isNull()) {
    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  uprv_memset(newSelector.getAlias(), 0, ಗಾತ್ರ(UConverterSelector));

  ಆದರೆ (converter_List_ಗಾತ್ರ == 0) {
    converterList = ಶೂನ್ಯಸೂಚಕ;
    converter_List_ಗಾತ್ರ = ucnv_ಎಣಿಕೆ_ಲಭ್ಯ();
  }
  newSelector->encodings =
    (ಅಕ್ಷರ**)uprv_malloc(converter_List_ಗಾತ್ರ * ಗಾತ್ರ(ಅಕ್ಷರ*));
  ಆದರೆ (!newSelector->encodings) {
    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  newSelector->encodings[0] = ಶೂನ್ಯಸೂಚಕ;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t ಒಟ್ಟು_ಗಾತ್ರ = 0;
  int32_t i;
  ಗಾಗಿ (i = 0; i < converter_List_ಗಾತ್ರ; i++) {
    ಒಟ್ಟು_ಗಾತ್ರ +=
      (int32_t)uprv_strlen(converterList != ಶೂನ್ಯಸೂಚಕ ? converterList[i] : ucnv_get_ಲಭ್ಯ_Name(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = ಒಟ್ಟು_ಗಾತ್ರ & 3;
  ಆದರೆ (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encoding_Str_ಉದ್ದಳತೆ = ಒಟ್ಟು_ಗಾತ್ರ += encodingStrPadding;
  ಅಕ್ಷರ* all_ಅಕ್ಷರಸರಣಿಗಳು = (ಅಕ್ಷರ*) uprv_malloc(ಒಟ್ಟು_ಗಾತ್ರ);
  ಆದರೆ (!all_ಅಕ್ಷರಸರಣಿಗಳು) {
    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  ಗಾಗಿ (i = 0; i < converter_List_ಗಾತ್ರ; i++) {
    newSelector->encodings[i] = all_ಅಕ್ಷರಸರಣಿಗಳು;
    uprv_strcpy(newSelector->encodings[i],
                converterList != ಶೂನ್ಯಸೂಚಕ ? converterList[i] : ucnv_get_ಲಭ್ಯ_Name(i));
    all_ಅಕ್ಷರಸರಣಿಗಳು += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  ಆಗ (encodingStrPadding > 0) {
    *all_ಅಕ್ಷರಸರಣಿಗಳು++ = 0;
    --encodingStrPadding;
  }

  newSelector->own_Encoding_ಅಕ್ಷರಸರಣಿಗಳು = ನಿಜ;
  newSelector->encodings_ಎಣಿಕೆ = converter_List_ಗಾತ್ರ;
  UPropsVectors *upvec = upvec_open((converter_List_ಗಾತ್ರ+31)/32, ಸ್ಥಿತಿ_2);
  generate_Selector_ದತ್ತಾಂಶ(newSelector.getAlias(), upvec, excluded_ಸಂಕೇತ_Points, which_ಗಣ, ಸ್ಥಿತಿ_2);
  upvec_close(upvec);

  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  ಹಿಂತಿರುಗಿ newSelector.orphan();
}

/* close opened selector */
U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  ಆದರೆ (!sel) {
    ಹಿಂತಿರುಗಿ;
  }
  ಆದರೆ (sel->own_Encoding_ಅಕ್ಷರಸರಣಿಗಳು) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  ಆದರೆ (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UDataInfo ದತ್ತಾಂಶ_Info = {
  ಗಾತ್ರ(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

ಎಣಿಕೆ {
  UCNVSEL_ಸೂಚ್ಯಂಕ_TRIE_ಗಾತ್ರ,      // trie size in bytes
  UCNVSEL_ಸೂಚ್ಯಂಕ_PV_ಎಣಿಕೆ,       // number of uint32_t in the bit vectors
  UCNVSEL_ಸೂಚ್ಯಂಕ_ಹೆಸರುಗಳು_ಎಣಿಕೆ,    // number of encoding names
  UCNVSEL_ಸೂಚ್ಯಂಕ_ಹೆಸರುಗಳು_ಉದ್ದಳತೆ,   // number of encoding name bytes including padding
  UCNVSEL_ಸೂಚ್ಯಂಕ_ಗಾತ್ರ = 15,      // bytes following the DataHeader
  UCNVSEL_ಸೂಚ್ಯಂಕ_ಎಣಿಕೆ = 16
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
ucnvsel_serialize(ಸ್ಥಿರಾಂಕ UConverterSelector* sel,
                  ಶೂನ್ಯ* ಬಫರ್, int32_t ಬಫರ್_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  // check if already failed
  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)ಬಫರ್;
  ಆದರೆ (ಬಫರ್_ಸಾಮರ್ಥ್ಯ < 0 ||
      (ಬಫರ್_ಸಾಮರ್ಥ್ಯ > 0 && (p == ಶೂನ್ಯಸೂಚಕ || (U_ಸೂಚಕ_ಮುಖವಾಡ_LSB(p, 3) != 0)))
  ) {
    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  // add up the size of the serialized form
  int32_t serialized_Trie_ಗಾತ್ರ = utrie2_serialize(sel->trie, ಶೂನ್ಯಸೂಚಕ, 0, ಸ್ಥಿತಿ_2);
  ಆದರೆ (*ಸ್ಥಿತಿ_2 != U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR && U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ 0;
  }
  *ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

  ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;
  uprv_memset(&ಶೀರ್ಷಿಕೆ, 0, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ));
  ಶೀರ್ಷಿಕೆ.ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ = (uint16_t)((ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ) + 15) & ~15);
  ಶೀರ್ಷಿಕೆ.ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic1 = 0xda;
  ಶೀರ್ಷಿಕೆ.ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic2 = 0x27;
  uprv_memcpy(&ಶೀರ್ಷಿಕೆ.info, &ದತ್ತಾಂಶ_Info, ಗಾತ್ರ(ದತ್ತಾಂಶ_Info));

  int32_t indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_ಎಣಿಕೆ] = {
    serialized_Trie_ಗಾತ್ರ,
    sel->pv_ಎಣಿಕೆ,
    sel->encodings_ಎಣಿಕೆ,
    sel->encoding_Str_ಉದ್ದಳತೆ
  };

  int32_t ಒಟ್ಟು_ಗಾತ್ರ =
    ಶೀರ್ಷಿಕೆ.ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ +
    (int32_t)ಗಾತ್ರ(indexes) +
    serialized_Trie_ಗಾತ್ರ +
    sel->pv_ಎಣಿಕೆ * 4 +
    sel->encoding_Str_ಉದ್ದಳತೆ;
  indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_ಗಾತ್ರ] = ಒಟ್ಟು_ಗಾತ್ರ - ಶೀರ್ಷಿಕೆ.ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
  ಆದರೆ (ಒಟ್ಟು_ಗಾತ್ರ > ಬಫರ್_ಸಾಮರ್ಥ್ಯ) {
    *ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    ಹಿಂತಿರುಗಿ ಒಟ್ಟು_ಗಾತ್ರ;
  }
  // ok, save!
  int32_t length = ಶೀರ್ಷಿಕೆ.ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
  uprv_memcpy(p, &ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ));
  uprv_memset(p + ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ), 0, length - ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ));
  p += length;

  length = (int32_t)ಗಾತ್ರ(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serialized_Trie_ಗಾತ್ರ, ಸ್ಥಿತಿ_2);
  p += serialized_Trie_ಗಾತ್ರ;

  length = sel->pv_ಎಣಿಕೆ * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encoding_Str_ಉದ್ದಳತೆ);
  p += sel->encoding_Str_ಉದ್ದಳತೆ;

  ಹಿಂತಿರುಗಿ ಒಟ್ಟು_ಗಾತ್ರ;
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
ucnvsel_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
             ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length,
             ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  /* udata_swapDataHeader checks the arguments */
  int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ = udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2);
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ 0;
  }

  /* check data format and format version */
  ಸ್ಥಿರಾಂಕ UDataInfo* pInfo = reinterpret_cast<ಸ್ಥಿರಾಂಕ UDataInfo*>(static_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಒಳ_ದತ್ತಾಂಶ) + 4);
  ಆದರೆ(!(
    pInfo->ದತ್ತಾಂಶ_Format[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->ದತ್ತಾಂಶ_Format[1] == 0x53 &&
    pInfo->ದತ್ತಾಂಶ_Format[2] == 0x65 &&
    pInfo->ದತ್ತಾಂಶ_Format[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->ದತ್ತಾಂಶ_Format[0], pInfo->ದತ್ತಾಂಶ_Format[1],
                     pInfo->ದತ್ತಾಂಶ_Format[2], pInfo->ದತ್ತಾಂಶ_Format[3]);
    *ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ(pInfo->format_ಆವೃತ್ತಿ[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->format_ಆವೃತ್ತಿ[0]);
    *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(length >= 0) {
    length -= ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ಆದರೆ(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *ಸ್ಥಿತಿ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
      ಹಿಂತಿರುಗಿ 0;
    }
  }

  ಸ್ಥಿರಾಂಕ uint8_t* ಒಳ_ಬೈಟುಗಳು = static_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(ಒಳ_ದತ್ತಾಂಶ) + ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
  uint8_t* ಹೊರಹೋಗು_ಬೈಟುಗಳು = static_cast<uint8_t*>(ಹೊರಹೋಗು_ದತ್ತಾಂಶ) + ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

  /* read the indexes */
  ಸ್ಥಿರಾಂಕ int32_t* ಒಳ_Indexes = reinterpret_cast<ಸ್ಥಿರಾಂಕ int32_t*>(ಒಳ_ಬೈಟುಗಳು);
  int32_t indexes[16];
  int32_t i;
  ಗಾಗಿ(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, ಒಳ_Indexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_ಗಾತ್ರ];
  ಆದರೆ(length >= 0) {
    ಆದರೆ(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *ಸ್ಥಿತಿ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
      ಹಿಂತಿರುಗಿ 0;
    }

    /* copy the data for inaccessible bytes */
    ಆದರೆ(ಒಳ_ಬೈಟುಗಳು != ಹೊರಹೋಗು_ಬೈಟುಗಳು) {
      uprv_memcpy(ಹೊರಹೋಗು_ಬೈಟುಗಳು, ಒಳ_ಬೈಟುಗಳು, size);
    }

    int32_t ಸ್ಥಾನಾಂತರ = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_ಸೂಚ್ಯಂಕ_ಎಣಿಕೆ*4;
    ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು, count, ಹೊರಹೋಗು_ಬೈಟುಗಳು, ಸ್ಥಿತಿ_2);
    ಸ್ಥಾನಾಂತರ += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_TRIE_ಗಾತ್ರ];
    utrie2_ಅದಲುಬದಲು(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, count, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, ಸ್ಥಿತಿ_2);
    ಸ್ಥಾನಾಂತರ += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_PV_ಎಣಿಕೆ]*4;
    ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, count, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, ಸ್ಥಿತಿ_2);
    ಸ್ಥಾನಾಂತರ += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_ಹೆಸರುಗಳು_ಉದ್ದಳತೆ];
    ds->ಅದಲುಬದಲು_Inv_Chars(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, count, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, ಸ್ಥಿತಿ_2);
    ಸ್ಥಾನಾಂತರ += count;

    U_ASSERT(ಸ್ಥಾನಾಂತರ == size);
  }

  ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* ಬಫರ್ U_LIFETIME_BOUND,
                           int32_t length,
                           UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  // check if already failed
  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  // ensure args make sense!
  ಸ್ಥಿರಾಂಕ uint8_t *p = (ಸ್ಥಿರಾಂಕ uint8_t *)ಬಫರ್;
  ಆದರೆ (length <= 0 ||
      (length > 0 && (p == ಶೂನ್ಯಸೂಚಕ || (U_ಸೂಚಕ_ಮುಖವಾಡ_LSB(p, 3) != 0)))
  ) {
    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  //ಶೀರ್ಷಿಕೆ
  ಆದರೆ (length < 32) {
    // not even enough space for a minimal header
    *ಸ್ಥಿತಿ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *p_ಶೀರ್ಷಿಕೆ = (ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *)p;
  ಆದರೆ (!(
    p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic1==0xda &&
    p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic2==0x27 &&
    p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[0] == 0x43 &&
    p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[1] == 0x53 &&
    p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[2] == 0x65 &&
    p_ಶೀರ್ಷಿಕೆ->info.ದತ್ತಾಂಶ_Format[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  ಆದರೆ (p_ಶೀರ್ಷಿಕೆ->info.format_ಆವೃತ್ತಿ[0] != 1) {
    *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  uint8_t* swapped = ಶೂನ್ಯಸೂಚಕ;
  ಆದರೆ (p_ಶೀರ್ಷಿಕೆ->info.isBigEndian != U_IS_BIG_ENDIAN ||
      p_ಶೀರ್ಷಿಕೆ->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_ಒಳಹರಿವು_ದತ್ತಾಂಶ(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, ಸ್ಥಿತಿ_2);
    int32_t ಒಟ್ಟು_ಗಾತ್ರ = ucnvsel_ಅದಲುಬದಲು(ds, p, -1, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
      udata_closeSwapper(ds);
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (length < ಒಟ್ಟು_ಗಾತ್ರ) {
      udata_closeSwapper(ds);
      *ಸ್ಥಿತಿ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    swapped = (uint8_t*)uprv_malloc(ಒಟ್ಟು_ಗಾತ್ರ);
    ಆದರೆ (swapped == ಶೂನ್ಯಸೂಚಕ) {
      udata_closeSwapper(ds);
      *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ucnvsel_ಅದಲುಬದಲು(ds, p, length, swapped, ಸ್ಥಿತಿ_2);
    udata_closeSwapper(ds);
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
      uprv_free(swapped);
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    p = swapped;
    p_ಶೀರ್ಷಿಕೆ = (ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *)p;
  }
  ಆದರೆ (length < (p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *ಸ್ಥಿತಿ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  p += p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
  length -= p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
  // indexes
  ಸ್ಥಿರಾಂಕ int32_t *indexes = (ಸ್ಥಿರಾಂಕ int32_t *)p;
  ಆದರೆ (length < indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_ಗಾತ್ರ]) {
    uprv_free(swapped);
    *ಸ್ಥಿತಿ_2 = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  p += UCNVSEL_ಸೂಚ್ಯಂಕ_ಎಣಿಕೆ * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(ಗಾತ್ರ(UConverterSelector));
  ಅಕ್ಷರ **encodings =
    (ಅಕ್ಷರ **)uprv_malloc(
      indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_ಹೆಸರುಗಳು_ಎಣಿಕೆ] * ಗಾತ್ರ(ಅಕ್ಷರ *));
  ಆದರೆ (sel == ಶೂನ್ಯಸೂಚಕ || encodings == ಶೂನ್ಯಸೂಚಕ) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  uprv_memset(sel, 0, ಗಾತ್ರ(UConverterSelector));
  sel->pv_ಎಣಿಕೆ = indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_PV_ಎಣಿಕೆ];
  sel->encodings = encodings;
  sel->encodings_ಎಣಿಕೆ = indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_ಹೆಸರುಗಳು_ಎಣಿಕೆ];
  sel->encoding_Str_ಉದ್ದಳತೆ = indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_ಹೆಸರುಗಳು_ಉದ್ದಳತೆ];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು,
                                        p, indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_TRIE_ಗಾತ್ರ], ಶೂನ್ಯಸೂಚಕ,
                                        ಸ್ಥಿತಿ_2);
  p += indexes[UCNVSEL_ಸೂಚ್ಯಂಕ_TRIE_ಗಾತ್ರ];
  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ucnvsel_close(sel);
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_ಎಣಿಕೆ * 4;
  // encoding names
  ಅಕ್ಷರ* s = (ಅಕ್ಷರ*)p;
  ಗಾಗಿ (int32_t i = 0; i < sel->encodings_ಎಣಿಕೆ; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encoding_Str_ಉದ್ದಳತೆ;

  ಹಿಂತಿರುಗಿ sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
ರಚನೆ Enumerator {
  int16_t* ಸೂಚ್ಯಂಕ;
  int16_t length;
  int16_t cur;
  ಸ್ಥಿರಾಂಕ UConverterSelector* sel;
};

U_CDECL_BEGIN

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->ಸೂಚ್ಯಂಕ);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


ಸ್ಥಿರ int32_t U_CALLCONV
ucnvsel_ಎಣಿಕೆ_encodings(UEnumeration *enumerator, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  // check if already failed
  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ ((Enumerator*)(enumerator->context))->length;
}


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_CALLCONV ucnvsel_ಮುಂದಿನ_encoding(UEnumeration* enumerator,
                                                 int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
                                                 UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  // check if already failed
  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ಸ್ಥಿರಾಂಕ UConverterSelector* sel;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಫಲಿತಾಂಶ;
  ಆದರೆ (cur >= ((Enumerator*)(enumerator->context))->length) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  ಫಲಿತಾಂಶ = sel->encodings[((Enumerator*)(enumerator->context))->ಸೂಚ್ಯಂಕ[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
    *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = (int32_t)uprv_strlen(ಫಲಿತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  // check if already failed
  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_ಕೊನೆ


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UEnumeration defaultEncodings = {
  ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnvsel_close_selector_iterator,
    ucnvsel_ಎಣಿಕೆ_encodings,
    uenum_unextDefault,
    ucnvsel_ಮುಂದಿನ_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
ಸ್ಥಿರ UBool intersectMasks(uint32_t* dest, ಸ್ಥಿರಾಂಕ uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  ಗಾಗಿ (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  ಹಿಂತಿರುಗಿ oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
ಸ್ಥಿರ int16_t ಎಣಿಕೆ_Ones(uint32_t* mask, int32_t len) {
  int32_t i, ಒಟ್ಟು_Ones = 0;
  ಗಾಗಿ (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    ಗಾಗಿ (; ent; ಒಟ್ಟು_Ones++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  ಹಿಂತಿರುಗಿ static_cast<int16_t>(ಒಟ್ಟು_Ones);
}


/* internal function! */
ಸ್ಥಿರ UEnumeration*
select_For_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* the_ಮುಖವಾಡ,
              UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  Local_ಸ್ಮರಣೆ<uint32_t> mask(the_ಮುಖವಾಡ);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  Local_ಸ್ಮರಣೆ<Enumerator> ಫಲಿತಾಂಶ(static_cast<Enumerator *>(uprv_malloc(ಗಾತ್ರ(Enumerator))));
  ಆದರೆ (ಫಲಿತಾಂಶ.isNull()) {
    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  ಫಲಿತಾಂಶ->ಸೂಚ್ಯಂಕ = ಶೂನ್ಯಸೂಚಕ;  // this will be allocated later!
  ಫಲಿತಾಂಶ->length = ಫಲಿತಾಂಶ->cur = 0;
  ಫಲಿತಾಂಶ->sel = sel;

  Local_ಸ್ಮರಣೆ<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(ಗಾತ್ರ(UEnumeration))));
  ಆದರೆ (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  ಸ್ಮರಣೆ_ನಕಲು(en.getAlias(), &defaultEncodings, ಗಾತ್ರ(UEnumeration));
  
  int32_t columns = (sel->encodings_ಎಣಿಕೆ+31)/32;
  int16_t ಅಂಕೆ_Ones = ಎಣಿಕೆ_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  ಆದರೆ (ಅಂಕೆ_Ones > 0) {
    ಫಲಿತಾಂಶ->ಸೂಚ್ಯಂಕ = static_cast<int16_t*>(uprv_malloc(ಅಂಕೆ_Ones * ಗಾತ್ರ(int16_t)));
    ಆದರೆ (ಫಲಿತಾಂಶ->ಸೂಚ್ಯಂಕ == ಶೂನ್ಯಸೂಚಕ) {
      *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    int32_t i, j;
    int16_t k = 0;
    ಗಾಗಿ (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      ಗಾಗಿ (i = 0 ; i < 32 && k < sel->encodings_ಎಣಿಕೆ; i++, k++) {
        ಆದರೆ ((v & 1) != 0) {
          ಫಲಿತಾಂಶ->ಸೂಚ್ಯಂಕ[ಫಲಿತಾಂಶ->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = ಫಲಿತಾಂಶ.orphan();
  ಹಿಂತಿರುಗಿ en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ಸ್ಥಿರಾಂಕ UConverterSelector* sel U_LIFETIME_BOUND,
                        ಸ್ಥಿರಾಂಕ char16_t* s,
                        int32_t length,
                        UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  // check if already failed
  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  // ensure args make sense!
  ಆದರೆ (sel == ಶೂನ್ಯಸೂಚಕ || (s == ಶೂನ್ಯಸೂಚಕ && length != 0)) {
    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  int32_t columns = (sel->encodings_ಎಣಿಕೆ+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  ಆದರೆ (mask == ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  uprv_memset(mask, ~0, columns *4);

  ಆದರೆ(s!=ಶೂನ್ಯಸೂಚಕ) {
    ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ;
    ಆದರೆ (length >= 0) {
      ಎಲ್ಲೆ = s + length;
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಎಲ್ಲೆ = ಶೂನ್ಯಸೂಚಕ;
    }
    
    ಆಗ (ಎಲ್ಲೆ == ಶೂನ್ಯಸೂಚಕ ? *s != 0 : s != ಎಲ್ಲೆ) {
      UChar32 c;
      uint16_t pv_ಸೂಚ್ಯಂಕ;
      UTRIE2_U16_NEXT16(sel->trie, s, ಎಲ್ಲೆ, c, pv_ಸೂಚ್ಯಂಕ);
      ಆದರೆ (intersectMasks(mask, sel->pv+pv_ಸೂಚ್ಯಂಕ, columns)) {
        ನಿಲ್ಲಿಸು;
      }
    }
  }
  ಹಿಂತಿರುಗಿ select_For_ಮುಖವಾಡ(sel, mask, ಸ್ಥಿತಿ_2);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ಸ್ಥಿರಾಂಕ UConverterSelector* sel U_LIFETIME_BOUND,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s,
                      int32_t length,
                      UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  // check if already failed
  ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  // ensure args make sense!
  ಆದರೆ (sel == ಶೂನ್ಯಸೂಚಕ || (s == ಶೂನ್ಯಸೂಚಕ && length != 0)) {
    *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  int32_t columns = (sel->encodings_ಎಣಿಕೆ+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  ಆದರೆ (mask == ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  uprv_memset(mask, ~0, columns *4);

  ಆದರೆ (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  ಆದರೆ(s!=ಶೂನ್ಯಸೂಚಕ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಎಲ್ಲೆ = s + length;
    
    ಆಗ (s != ಎಲ್ಲೆ) {
      uint16_t pv_ಸೂಚ್ಯಂಕ;
      UTRIE2_U8_NEXT16(sel->trie, s, ಎಲ್ಲೆ, pv_ಸೂಚ್ಯಂಕ);
      ಆದರೆ (intersectMasks(mask, sel->pv+pv_ಸೂಚ್ಯಂಕ, columns)) {
        ನಿಲ್ಲಿಸು;
      }
    }
  }
  ಹಿಂತಿರುಗಿ select_For_ಮುಖವಾಡ(sel, mask, ಸ್ಥಿತಿ_2);
}

#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_NO_CONVERSION
