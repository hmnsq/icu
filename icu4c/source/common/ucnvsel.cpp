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

#ضمن "unicode/ucnvsel.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن <string.h>

#ضمن "unicode/uchar.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/ucnv.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/uchriter.h"
#ضمن "utrie2.h"
#ضمن "propsvec.h"
#ضمن "uassert.h"
#ضمن "ucmndata.h"
#ضمن "udataswp.h"
#ضمن "uenumimp.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"

U_NAMESPACE_USE

بنية UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_شمېر;
  حرف** encodings;          // which encodings did user ask to use?
  int32_t encodings_شمېر;
  int32_t encoding_Str_اوږدتیا;
  uint8_t* swapped;
  UBool ownPv, own_Encoding_مزي_ګانې;
};

ساكن فراغ generate_Selector_معلومات(UConverterSelector* پایله,
                                 UPropsVectors *upvec,
                                 ثابت USet* excluded_کوډ_Points,
                                 ثابت UConverter_Unicode_سېټ which_سېټ,
                                 UError_کوډ* حالت) {
  إذا (U_FAILURE(*حالت)) {
    أرجع;
  }

  int32_t columns = (پایله->encodings_شمېر+31)/32;

  // set errorValue to all-ones
  لكل (int32_t ستون = 0; ستون < columns; ستون++) {
    upvec_سېټ_ارزښت(upvec, UPVEC_ERROR_ارزښت_CP, UPVEC_ERROR_ارزښت_CP,
                   ستون, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), حالت);
  }

  لكل (int32_t i = 0; i < پایله->encodings_شمېر; ++i) {
    uint32_t mask;
    uint32_t ستنه;
    int32_t توکی_شمېر;
    int32_t j;
    UConverter* test_converter = ucnv_open(پایله->encodings[i], حالت);
    إذا (U_FAILURE(*حالت)) {
      أرجع;
    }
    USet* unicode_ټکی_سېټ;
    unicode_ټکی_سېټ = uset_open(1, 0);  // empty set

    ucnv_get_Unicode_سېټ(test_converter, unicode_ټکی_سېټ,
                       which_سېټ, حالت);
    إذا (U_FAILURE(*حالت)) {
      ucnv_close(test_converter);
      أرجع;
    }

    ستنه = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    توکی_شمېر = uset_get_توکی_شمېر(unicode_ټکی_سېټ);

    لكل (j = 0; j < توکی_شمېر; ++j) {
      UChar32 پیل_char;
      UChar32 پای_ټکی_char;
      UError_کوډ small_حالت = U_ZERO_ERROR;
      uset_get_توکی(unicode_ټکی_سېټ, j, &پیل_char, &پای_ټکی_char, تش_اشاره, 0,
                   &small_حالت);
      إذا (U_FAILURE(small_حالت)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } وإلا {
        upvec_سېټ_ارزښت(upvec, پیل_char, پای_ټکی_char, ستنه, static_cast<uint32_t>(~0), mask,
                       حالت);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_ټکی_سېټ);
    إذا (U_FAILURE(*حالت)) {
      أرجع;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  إذا (excluded_کوډ_Points) {
    int32_t توکی_شمېر = uset_get_توکی_شمېر(excluded_کوډ_Points);
    لكل (int32_t j = 0; j < توکی_شمېر; ++j) {
      UChar32 پیل_char;
      UChar32 پای_ټکی_char;

      uset_get_توکی(excluded_کوډ_Points, j, &پیل_char, &پای_ټکی_char, تش_اشاره, 0,
                   حالت);
      لكل (int32_t ستون = 0; ستون < columns; ستون++) {
        upvec_سېټ_ارزښت(upvec, پیل_char, پای_ټکی_char, ستون, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      حالت);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  پایله->trie = upvec_compact_منزل_UTrie2_With_لیکه_Indexes(upvec, حالت);
  پایله->pv = upvec_cloneArray(upvec, &پایله->pv_شمېر, تش_اشاره, حالت);
  پایله->pv_شمېر *= columns;  // number of uint32_t = rows * columns
  پایله->ownPv = ریښتیا;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ثابت حرف* ثابت*  converterList, int32_t converter_List_اندازه,
             ثابت USet* excluded_کوډ_Points,
             ثابت UConverter_Unicode_سېټ which_سېټ, UError_کوډ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }
  // ensure args make sense!
  إذا (converter_List_اندازه < 0 || (converterList == تش_اشاره && converter_List_اندازه != 0)) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع تش_اشاره;
  }

  // allocate a new converter
  Local_UConverter_Selector_اشاره_ګر newSelector(
    (UConverterSelector*)uprv_malloc(حجم(UConverterSelector)));
  إذا (newSelector.isNull()) {
    *حالت = U_حافظه_ویش_ERROR;
    أرجع تش_اشاره;
  }
  uprv_memset(newSelector.getAlias(), 0, حجم(UConverterSelector));

  إذا (converter_List_اندازه == 0) {
    converterList = تش_اشاره;
    converter_List_اندازه = ucnv_شمېر_شته();
  }
  newSelector->encodings =
    (حرف**)uprv_malloc(converter_List_اندازه * حجم(حرف*));
  إذا (!newSelector->encodings) {
    *حالت = U_حافظه_ویش_ERROR;
    أرجع تش_اشاره;
  }
  newSelector->encodings[0] = تش_اشاره;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t ټول_اندازه = 0;
  int32_t i;
  لكل (i = 0; i < converter_List_اندازه; i++) {
    ټول_اندازه +=
      (int32_t)uprv_strlen(converterList != تش_اشاره ? converterList[i] : ucnv_get_شته_Name(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = ټول_اندازه & 3;
  إذا (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encoding_Str_اوږدتیا = ټول_اندازه += encodingStrPadding;
  حرف* all_مزي_ګانې = (حرف*) uprv_malloc(ټول_اندازه);
  إذا (!all_مزي_ګانې) {
    *حالت = U_حافظه_ویش_ERROR;
    أرجع تش_اشاره;
  }

  لكل (i = 0; i < converter_List_اندازه; i++) {
    newSelector->encodings[i] = all_مزي_ګانې;
    uprv_strcpy(newSelector->encodings[i],
                converterList != تش_اشاره ? converterList[i] : ucnv_get_شته_Name(i));
    all_مزي_ګانې += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  طالما (encodingStrPadding > 0) {
    *all_مزي_ګانې++ = 0;
    --encodingStrPadding;
  }

  newSelector->own_Encoding_مزي_ګانې = ریښتیا;
  newSelector->encodings_شمېر = converter_List_اندازه;
  UPropsVectors *upvec = upvec_open((converter_List_اندازه+31)/32, حالت);
  generate_Selector_معلومات(newSelector.getAlias(), upvec, excluded_کوډ_Points, which_سېټ, حالت);
  upvec_close(upvec);

  إذا (U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }

  أرجع newSelector.orphan();
}

/* close opened selector */
U_CAPI فراغ U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  إذا (!sel) {
    أرجع;
  }
  إذا (sel->own_Encoding_مزي_ګانې) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  إذا (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

ساكن ثابت UDataInfo معلومات_Info = {
  حجم(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

تعداد {
  UCNVSEL_شاخص_TRIE_اندازه,      // trie size in bytes
  UCNVSEL_شاخص_PV_شمېر,       // number of uint32_t in the bit vectors
  UCNVSEL_شاخص_نومونه_شمېر,    // number of encoding names
  UCNVSEL_شاخص_نومونه_اوږدتیا,   // number of encoding name bytes including padding
  UCNVSEL_شاخص_اندازه = 15,      // bytes following the DataHeader
  UCNVSEL_شاخص_شمېر = 16
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
ucnvsel_serialize(ثابت UConverterSelector* sel,
                  فراغ* بفر, int32_t بفر_ظرفیت, UError_کوډ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)بفر;
  إذا (بفر_ظرفیت < 0 ||
      (بفر_ظرفیت > 0 && (p == تش_اشاره || (U_اشاره_ګر_ماسک_LSB(p, 3) != 0)))
  ) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  // add up the size of the serialized form
  int32_t serialized_Trie_اندازه = utrie2_serialize(sel->trie, تش_اشاره, 0, حالت);
  إذا (*حالت != U_بفر_ډکېدنه_ERROR && U_FAILURE(*حالت)) {
    أرجع 0;
  }
  *حالت = U_ZERO_ERROR;

  معلومات_سرلیک سرلیک;
  uprv_memset(&سرلیک, 0, حجم(سرلیک));
  سرلیک.معلومات_سرلیک_2.سرلیک_اندازه = (uint16_t)((حجم(سرلیک) + 15) & ~15);
  سرلیک.معلومات_سرلیک_2.magic1 = 0xda;
  سرلیک.معلومات_سرلیک_2.magic2 = 0x27;
  uprv_memcpy(&سرلیک.info, &معلومات_Info, حجم(معلومات_Info));

  int32_t indexes[UCNVSEL_شاخص_شمېر] = {
    serialized_Trie_اندازه,
    sel->pv_شمېر,
    sel->encodings_شمېر,
    sel->encoding_Str_اوږدتیا
  };

  int32_t ټول_اندازه =
    سرلیک.معلومات_سرلیک_2.سرلیک_اندازه +
    (int32_t)حجم(indexes) +
    serialized_Trie_اندازه +
    sel->pv_شمېر * 4 +
    sel->encoding_Str_اوږدتیا;
  indexes[UCNVSEL_شاخص_اندازه] = ټول_اندازه - سرلیک.معلومات_سرلیک_2.سرلیک_اندازه;
  إذا (ټول_اندازه > بفر_ظرفیت) {
    *حالت = U_بفر_ډکېدنه_ERROR;
    أرجع ټول_اندازه;
  }
  // ok, save!
  int32_t length = سرلیک.معلومات_سرلیک_2.سرلیک_اندازه;
  uprv_memcpy(p, &سرلیک, حجم(سرلیک));
  uprv_memset(p + حجم(سرلیک), 0, length - حجم(سرلیک));
  p += length;

  length = (int32_t)حجم(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serialized_Trie_اندازه, حالت);
  p += serialized_Trie_اندازه;

  length = sel->pv_شمېر * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encoding_Str_اوږدتیا);
  p += sel->encoding_Str_اوږدتیا;

  أرجع ټول_اندازه;
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
ساكن int32_t
ucnvsel_بدلون(ثابت UDataSwapper *ds,
             ثابت فراغ *دننه_معلومات, int32_t length,
             فراغ *بهر_معلومات, UError_کوډ *حالت) {
  /* udata_swapDataHeader checks the arguments */
  int32_t سرلیک_اندازه = udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, length, بهر_معلومات, حالت);
  إذا(U_FAILURE(*حالت)) {
    أرجع 0;
  }

  /* check data format and format version */
  ثابت UDataInfo* pInfo = reinterpret_cast<ثابت UDataInfo*>(static_cast<ثابت حرف*>(دننه_معلومات) + 4);
  إذا(!(
    pInfo->معلومات_Format[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->معلومات_Format[1] == 0x53 &&
    pInfo->معلومات_Format[2] == 0x65 &&
    pInfo->معلومات_Format[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                     pInfo->معلومات_Format[2], pInfo->معلومات_Format[3]);
    *حالت = U_INVALID_FORMAT_ERROR;
    أرجع 0;
  }
  إذا(pInfo->format_نسخه[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->format_نسخه[0]);
    *حالت = U_UNSUPPORTED_ERROR;
    أرجع 0;
  }

  إذا(length >= 0) {
    length -= سرلیک_اندازه;
    إذا(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *حالت = U_شاخص_OUTOFBOUNDS_ERROR;
      أرجع 0;
    }
  }

  ثابت uint8_t* دننه_بایټونه = static_cast<ثابت uint8_t*>(دننه_معلومات) + سرلیک_اندازه;
  uint8_t* بهر_بایټونه = static_cast<uint8_t*>(بهر_معلومات) + سرلیک_اندازه;

  /* read the indexes */
  ثابت int32_t* دننه_Indexes = reinterpret_cast<ثابت int32_t*>(دننه_بایټونه);
  int32_t indexes[16];
  int32_t i;
  لكل(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, دننه_Indexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_شاخص_اندازه];
  إذا(length >= 0) {
    إذا(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *حالت = U_شاخص_OUTOFBOUNDS_ERROR;
      أرجع 0;
    }

    /* copy the data for inaccessible bytes */
    إذا(دننه_بایټونه != بهر_بایټونه) {
      uprv_memcpy(بهر_بایټونه, دننه_بایټونه, size);
    }

    int32_t دننه_واټن = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_شاخص_شمېر*4;
    ds->بدلون_Array32(ds, دننه_بایټونه, count, بهر_بایټونه, حالت);
    دننه_واټن += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_شاخص_TRIE_اندازه];
    utrie2_بدلون(ds, دننه_بایټونه + دننه_واټن, count, بهر_بایټونه + دننه_واټن, حالت);
    دننه_واټن += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_شاخص_PV_شمېر]*4;
    ds->بدلون_Array32(ds, دننه_بایټونه + دننه_واټن, count, بهر_بایټونه + دننه_واټن, حالت);
    دننه_واټن += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_شاخص_نومونه_اوږدتیا];
    ds->بدلون_Inv_Chars(ds, دننه_بایټونه + دننه_واټن, count, بهر_بایټونه + دننه_واټن, حالت);
    دننه_واټن += count;

    U_ASSERT(دننه_واټن == size);
  }

  أرجع سرلیک_اندازه + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ثابت فراغ* بفر U_LIFETIME_BOUND,
                           int32_t length,
                           UError_کوډ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }
  // ensure args make sense!
  ثابت uint8_t *p = (ثابت uint8_t *)بفر;
  إذا (length <= 0 ||
      (length > 0 && (p == تش_اشاره || (U_اشاره_ګر_ماسک_LSB(p, 3) != 0)))
  ) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع تش_اشاره;
  }
  //سرلیک
  إذا (length < 32) {
    // not even enough space for a minimal header
    *حالت = U_شاخص_OUTOFBOUNDS_ERROR;
    أرجع تش_اشاره;
  }
  ثابت معلومات_سرلیک *p_سرلیک = (ثابت معلومات_سرلیک *)p;
  إذا (!(
    p_سرلیک->معلومات_سرلیک_2.magic1==0xda &&
    p_سرلیک->معلومات_سرلیک_2.magic2==0x27 &&
    p_سرلیک->info.معلومات_Format[0] == 0x43 &&
    p_سرلیک->info.معلومات_Format[1] == 0x53 &&
    p_سرلیک->info.معلومات_Format[2] == 0x65 &&
    p_سرلیک->info.معلومات_Format[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *حالت = U_INVALID_FORMAT_ERROR;
    أرجع تش_اشاره;
  }
  إذا (p_سرلیک->info.format_نسخه[0] != 1) {
    *حالت = U_UNSUPPORTED_ERROR;
    أرجع تش_اشاره;
  }
  uint8_t* swapped = تش_اشاره;
  إذا (p_سرلیک->info.isBigEndian != U_IS_BIG_ENDIAN ||
      p_سرلیک->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_ننوت_معلومات(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, حالت);
    int32_t ټول_اندازه = ucnvsel_بدلون(ds, p, -1, تش_اشاره, حالت);
    إذا (U_FAILURE(*حالت)) {
      udata_closeSwapper(ds);
      أرجع تش_اشاره;
    }
    إذا (length < ټول_اندازه) {
      udata_closeSwapper(ds);
      *حالت = U_شاخص_OUTOFBOUNDS_ERROR;
      أرجع تش_اشاره;
    }
    swapped = (uint8_t*)uprv_malloc(ټول_اندازه);
    إذا (swapped == تش_اشاره) {
      udata_closeSwapper(ds);
      *حالت = U_حافظه_ویش_ERROR;
      أرجع تش_اشاره;
    }
    ucnvsel_بدلون(ds, p, length, swapped, حالت);
    udata_closeSwapper(ds);
    إذا (U_FAILURE(*حالت)) {
      uprv_free(swapped);
      أرجع تش_اشاره;
    }
    p = swapped;
    p_سرلیک = (ثابت معلومات_سرلیک *)p;
  }
  إذا (length < (p_سرلیک->معلومات_سرلیک_2.سرلیک_اندازه + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *حالت = U_شاخص_OUTOFBOUNDS_ERROR;
    أرجع تش_اشاره;
  }
  p += p_سرلیک->معلومات_سرلیک_2.سرلیک_اندازه;
  length -= p_سرلیک->معلومات_سرلیک_2.سرلیک_اندازه;
  // indexes
  ثابت int32_t *indexes = (ثابت int32_t *)p;
  إذا (length < indexes[UCNVSEL_شاخص_اندازه]) {
    uprv_free(swapped);
    *حالت = U_شاخص_OUTOFBOUNDS_ERROR;
    أرجع تش_اشاره;
  }
  p += UCNVSEL_شاخص_شمېر * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(حجم(UConverterSelector));
  حرف **encodings =
    (حرف **)uprv_malloc(
      indexes[UCNVSEL_شاخص_نومونه_شمېر] * حجم(حرف *));
  إذا (sel == تش_اشاره || encodings == تش_اشاره) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *حالت = U_حافظه_ویش_ERROR;
    أرجع تش_اشاره;
  }
  uprv_memset(sel, 0, حجم(UConverterSelector));
  sel->pv_شمېر = indexes[UCNVSEL_شاخص_PV_شمېر];
  sel->encodings = encodings;
  sel->encodings_شمېر = indexes[UCNVSEL_شاخص_نومونه_شمېر];
  sel->encoding_Str_اوږدتیا = indexes[UCNVSEL_شاخص_نومونه_اوږدتیا];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_ارزښت_بټونه,
                                        p, indexes[UCNVSEL_شاخص_TRIE_اندازه], تش_اشاره,
                                        حالت);
  p += indexes[UCNVSEL_شاخص_TRIE_اندازه];
  إذا (U_FAILURE(*حالت)) {
    ucnvsel_close(sel);
    أرجع تش_اشاره;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_شمېر * 4;
  // encoding names
  حرف* s = (حرف*)p;
  لكل (int32_t i = 0; i < sel->encodings_شمېر; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encoding_Str_اوږدتیا;

  أرجع sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
بنية Enumerator {
  int16_t* شاخص;
  int16_t length;
  int16_t cur;
  ثابت UConverterSelector* sel;
};

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->شاخص);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


ساكن int32_t U_CALLCONV
ucnvsel_شمېر_encodings(UEnumeration *enumerator, UError_کوډ *حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع 0;
  }
  أرجع ((Enumerator*)(enumerator->context))->length;
}


ساكن ثابت حرف* U_CALLCONV ucnvsel_بل_encoding(UEnumeration* enumerator,
                                                 int32_t* پایله_اوږدتیا,
                                                 UError_کوډ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ثابت UConverterSelector* sel;
  ثابت حرف* پایله;
  إذا (cur >= ((Enumerator*)(enumerator->context))->length) {
    أرجع تش_اشاره;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  پایله = sel->encodings[((Enumerator*)(enumerator->context))->شاخص[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  إذا (پایله_اوږدتیا) {
    *پایله_اوږدتیا = (int32_t)uprv_strlen(پایله);
  }
  أرجع پایله;
}

ساكن فراغ U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UError_کوډ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_پای_ټکی


ساكن ثابت UEnumeration defaultEncodings = {
  تش_اشاره,
    تش_اشاره,
    ucnvsel_close_selector_iterator,
    ucnvsel_شمېر_encodings,
    uenum_unextDefault,
    ucnvsel_بل_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
ساكن UBool intersectMasks(uint32_t* dest, ثابت uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  لكل (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  أرجع oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
ساكن int16_t شمېر_Ones(uint32_t* mask, int32_t len) {
  int32_t i, ټول_Ones = 0;
  لكل (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    لكل (; ent; ټول_Ones++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  أرجع static_cast<int16_t>(ټول_Ones);
}


/* internal function! */
ساكن UEnumeration*
select_For_ماسک(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* the_ماسک,
              UError_کوډ* حالت) {
  Local_حافظه<uint32_t> mask(the_ماسک);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  Local_حافظه<Enumerator> پایله(static_cast<Enumerator *>(uprv_malloc(حجم(Enumerator))));
  إذا (پایله.isNull()) {
    *حالت = U_حافظه_ویش_ERROR;
    أرجع تش_اشاره;
  }
  پایله->شاخص = تش_اشاره;  // this will be allocated later!
  پایله->length = پایله->cur = 0;
  پایله->sel = sel;

  Local_حافظه<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration))));
  إذا (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *حالت = U_حافظه_ویش_ERROR;
    أرجع تش_اشاره;
  }
  حافظه_نقل(en.getAlias(), &defaultEncodings, حجم(UEnumeration));
  
  int32_t columns = (sel->encodings_شمېر+31)/32;
  int16_t عدد_Ones = شمېر_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  إذا (عدد_Ones > 0) {
    پایله->شاخص = static_cast<int16_t*>(uprv_malloc(عدد_Ones * حجم(int16_t)));
    إذا (پایله->شاخص == تش_اشاره) {
      *حالت = U_حافظه_ویش_ERROR;
      أرجع تش_اشاره;
    }
    int32_t i, j;
    int16_t k = 0;
    لكل (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      لكل (i = 0 ; i < 32 && k < sel->encodings_شمېر; i++, k++) {
        إذا ((v & 1) != 0) {
          پایله->شاخص[پایله->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = پایله.orphan();
  أرجع en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
                        ثابت char16_t* s,
                        int32_t length,
                        UError_کوډ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }
  // ensure args make sense!
  إذا (sel == تش_اشاره || (s == تش_اشاره && length != 0)) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع تش_اشاره;
  }

  int32_t columns = (sel->encodings_شمېر+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  إذا (mask == تش_اشاره) {
    *حالت = U_حافظه_ویش_ERROR;
    أرجع تش_اشاره;
  }
  uprv_memset(mask, ~0, columns *4);

  إذا(s!=تش_اشاره) {
    ثابت char16_t *حد;
    إذا (length >= 0) {
      حد = s + length;
    } وإلا {
      حد = تش_اشاره;
    }
    
    طالما (حد == تش_اشاره ? *s != 0 : s != حد) {
      UChar32 c;
      uint16_t pv_شاخص;
      UTRIE2_U16_NEXT16(sel->trie, s, حد, c, pv_شاخص);
      إذا (intersectMasks(mask, sel->pv+pv_شاخص, columns)) {
        اكسر;
      }
    }
  }
  أرجع select_For_ماسک(sel, mask, حالت);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
                      ثابت حرف* s,
                      int32_t length,
                      UError_کوډ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }
  // ensure args make sense!
  إذا (sel == تش_اشاره || (s == تش_اشاره && length != 0)) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع تش_اشاره;
  }

  int32_t columns = (sel->encodings_شمېر+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  إذا (mask == تش_اشاره) {
    *حالت = U_حافظه_ویش_ERROR;
    أرجع تش_اشاره;
  }
  uprv_memset(mask, ~0, columns *4);

  إذا (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  إذا(s!=تش_اشاره) {
    ثابت حرف *حد = s + length;
    
    طالما (s != حد) {
      uint16_t pv_شاخص;
      UTRIE2_U8_NEXT16(sel->trie, s, حد, pv_شاخص);
      إذا (intersectMasks(mask, sel->pv+pv_شاخص, columns)) {
        اكسر;
      }
    }
  }
  أرجع select_For_ماسک(sel, mask, حالت);
}

#د_که_پای  // !UCONFIG_NO_CONVERSION
