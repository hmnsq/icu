// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucol_swp.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003sep10
*   created by: Markus W. Scherer
*
*   Swap collation binaries.
*/

#ضمن "unicode/udata.h" /* UDataInfo */
#ضمن "utrie.h"
#ضمن "utrie2.h"
#ضمن "udataswp.h"
#ضمن "cmemory.h"
#ضمن "ucol_data.h"
#ضمن "ucol_swp.h"

/* swapping ----------------------------------------------------------------- */

#إذا !UCONFIG_NO_COLLATION

U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_ٻٽو(ثابت UDataSwapper *ds,
                              ثابت فراغ *اندريون_معلومات, int32_t length) {
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || length<-1) {
        أرجع خطأ;
    }

    // First check for format version 4+ which has a standard data header.
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    (فراغ)udata_مٽا_سٽا_معلومات_سرو(ds, اندريون_معلومات, -1, خالي_اشارو, &error_ڪوڊ);
    إذا(U_SUCCESS(error_ڪوڊ)) {
        ثابت UDataInfo &info=*(ثابت UDataInfo *)((ثابت حرف *)اندريون_معلومات+4);
        إذا(info.معلومات_Format[0]==0x55 &&   // dataFormat="UCol"
                info.معلومات_Format[1]==0x43 &&
                info.معلومات_Format[2]==0x6f &&
                info.معلومات_Format[3]==0x6c) {
            أرجع سچ;
        }
    }

    // Else check for format version 3.
    ثابت UCATable_سرو *اندريون_سرو=(ثابت UCATable_سرو *)اندريون_معلومات;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATable_سرو سرو;
    uprv_memset(&سرو, 0, حجم(سرو));
    إذا(length<0) {
        سرو.size=udata_readInt32(ds, اندريون_سرو->size);
    } وإلا إذا((length<(42*4) || length<(سرو.size=udata_readInt32(ds, اندريون_سرو->size)))) {
        أرجع خطأ;
    }

    سرو.magic=ds->readUInt32(اندريون_سرو->magic);
    إذا(!(
        سرو.magic==UCOL_سرو_MAGIC &&
        اندريون_سرو->format_نسخو[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        أرجع خطأ;
    }

    إذا(اندريون_سرو->isBigEndian!=ds->اندريون_Is_Big_Endian || اندريون_سرو->char_سيٽ_Family!=ds->اندريون_Charset) {
        أرجع خطأ;
    }

    أرجع سچ;
}

نطاق {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
مٽا_سٽا_Format_Version3(ثابت UDataSwapper *ds,
                   ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت uint8_t *اندريون_بائيٽون;
    uint8_t *ٻاهريون_بائيٽون;

    ثابت UCATable_سرو *اندريون_سرو;
    UCATable_سرو *ٻاهريون_سرو;
    UCATable_سرو سرو;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || length<-1 || (length>0 && ٻاهريون_معلومات==خالي_اشارو)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    اندريون_بائيٽون = static_cast<ثابت uint8_t*>(اندريون_معلومات);
    ٻاهريون_بائيٽون = static_cast<uint8_t*>(ٻاهريون_معلومات);

    اندريون_سرو = static_cast<ثابت UCATable_سرو*>(اندريون_معلومات);
    ٻاهريون_سرو = static_cast<UCATable_سرو*>(ٻاهريون_معلومات);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&سرو, 0, حجم(سرو));
    إذا(length<0) {
        سرو.size=udata_readInt32(ds, اندريون_سرو->size);
    } وإلا إذا((length<(42*4) || length<(سرو.size=udata_readInt32(ds, اندريون_سرو->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    سرو.magic=ds->readUInt32(اندريون_سرو->magic);
    إذا(!(
        سرو.magic==UCOL_سرو_MAGIC &&
        اندريون_سرو->format_نسخو[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         سرو.magic,
                         اندريون_سرو->format_نسخو[0], اندريون_سرو->format_نسخو[1]);
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    إذا(اندريون_سرو->isBigEndian!=ds->اندريون_Is_Big_Endian || اندريون_سرو->char_سيٽ_Family!=ds->اندريون_Charset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         اندريون_سرو->isBigEndian, اندريون_سرو->char_سيٽ_Family);
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        إذا(اندريون_بائيٽون!=ٻاهريون_بائيٽون) {
            uprv_memcpy(ٻاهريون_بائيٽون, اندريون_بائيٽون, سرو.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        سرو.options=                 ds->readUInt32(اندريون_سرو->options);
        سرو.UCAConsts=               ds->readUInt32(اندريون_سرو->UCAConsts);
        سرو.contractionUCACombos=    ds->readUInt32(اندريون_سرو->contractionUCACombos);
        سرو.mappingPosition=         ds->readUInt32(اندريون_سرو->mappingPosition);
        سرو.expansion=               ds->readUInt32(اندريون_سرو->expansion);
        سرو.contraction_اشاريو=        ds->readUInt32(اندريون_سرو->contraction_اشاريو);
        سرو.contractionCEs=          ds->readUInt32(اندريون_سرو->contractionCEs);
        سرو.contraction_سائيز=         ds->readUInt32(اندريون_سرو->contraction_سائيز);
        سرو.پڇاڙي_Expansion_CE=          ds->readUInt32(اندريون_سرو->پڇاڙي_Expansion_CE);
        سرو.expansionCESize=         ds->readUInt32(اندريون_سرو->expansionCESize);
        سرو.پڇاڙي_Expansion_CECount=     udata_readInt32(ds, اندريون_سرو->پڇاڙي_Expansion_CECount);
        سرو.contraction_UCACombos_سائيز=udata_readInt32(ds, اندريون_سرو->contraction_UCACombos_سائيز);
        سرو.script_منزل_Lead_Byte=        ds->readUInt32(اندريون_سرو->script_منزل_Lead_Byte);
        سرو.lead_Byte_منزل_Script=        ds->readUInt32(اندريون_سرو->lead_Byte_منزل_Script);
        
        /* swap the 32-bit integers in the header */
        ds->مٽا_سٽا_Array32(ds, اندريون_سرو, static_cast<int32_t>(reinterpret_cast<ثابت حرف*>(&اندريون_سرو->jamoSpecial) - reinterpret_cast<ثابت حرف*>(اندريون_سرو)),
                           ٻاهريون_سرو, p_Error_ڪوڊ);
        ds->مٽا_سٽا_Array32(ds, &(اندريون_سرو->script_منزل_Lead_Byte), حجم(سرو.script_منزل_Lead_Byte) + حجم(سرو.lead_Byte_منزل_Script),
                           &(ٻاهريون_سرو->script_منزل_Lead_Byte), p_Error_ڪوڊ);
        /* set the output platform properties */
        ٻاهريون_سرو->isBigEndian=ds->ٻاهريون_Is_Big_Endian;
        ٻاهريون_سرو->char_سيٽ_Family=ds->ٻاهريون_Charset;

        /* swap the options */
        إذا(سرو.options!=0) {
            ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+سرو.options, سرو.expansion-سرو.options,
                               ٻاهريون_بائيٽون+سرو.options, p_Error_ڪوڊ);
        }

        /* swap the expansions */
        إذا(سرو.mappingPosition!=0 && سرو.expansion!=0) {
            إذا(سرو.contraction_اشاريو!=0) {
                /* expansions bounded by contractions */
                count=سرو.contraction_اشاريو-سرو.expansion;
            } وإلا {
                /* no contractions: expansions bounded by the main trie */
                count=سرو.mappingPosition-سرو.expansion;
            }
            ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون + سرو.expansion, static_cast<int32_t>(count),
                               ٻاهريون_بائيٽون+سرو.expansion, p_Error_ڪوڊ);
        }

        /* swap the contractions */
        إذا(سرو.contraction_سائيز!=0) {
            /* contractionIndex: char16_t[] */
            ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+سرو.contraction_اشاريو, سرو.contraction_سائيز*2,
                               ٻاهريون_بائيٽون+سرو.contraction_اشاريو, p_Error_ڪوڊ);

            /* contractionCEs: CEs[] */
            ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+سرو.contractionCEs, سرو.contraction_سائيز*4,
                               ٻاهريون_بائيٽون+سرو.contractionCEs, p_Error_ڪوڊ);
        }

        /* swap the main trie */
        إذا(سرو.mappingPosition!=0) {
            count=سرو.پڇاڙي_Expansion_CE-سرو.mappingPosition;
            utrie_مٽا_سٽا(ds, اندريون_بائيٽون + سرو.mappingPosition, static_cast<int32_t>(count),
                          ٻاهريون_بائيٽون+سرو.mappingPosition, p_Error_ڪوڊ);
        }

        /* swap the max expansion table */
        إذا(سرو.پڇاڙي_Expansion_CECount!=0) {
            ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+سرو.پڇاڙي_Expansion_CE, سرو.پڇاڙي_Expansion_CECount*4,
                               ٻاهريون_بائيٽون+سرو.پڇاڙي_Expansion_CE, p_Error_ڪوڊ);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        إذا(سرو.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+سرو.UCAConsts, سرو.contractionUCACombos-سرو.UCAConsts,
                               ٻاهريون_بائيٽون+سرو.UCAConsts, p_Error_ڪوڊ);
        }

        /* swap UCA contractions */
        إذا(سرو.contraction_UCACombos_سائيز!=0) {
            count=سرو.contraction_UCACombos_سائيز*اندريون_سرو->contraction_UCACombos_ويڪر*U_SIZEOF_UCHAR;
            ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون + سرو.contractionUCACombos, static_cast<int32_t>(count),
                               ٻاهريون_بائيٽون+سرو.contractionUCACombos, p_Error_ڪوڊ);
        }
        
        /* swap the script to lead bytes */
        إذا(سرو.script_منزل_Lead_Byte!=0) {
            صحيح اشاريو_ڳڻپ = ds->readUInt16(*((uint16_t*)(اندريون_بائيٽون+سرو.script_منزل_Lead_Byte))); // each entry = 2 * uint16
            صحيح معلومات_ڳڻپ = ds->readUInt16(*((uint16_t*)(اندريون_بائيٽون+سرو.script_منزل_Lead_Byte + 2))); // each entry = uint16
            ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+سرو.script_منزل_Lead_Byte, 
                                4 + (4 * اشاريو_ڳڻپ) + (2 * معلومات_ڳڻپ),
                                ٻاهريون_بائيٽون+سرو.script_منزل_Lead_Byte, p_Error_ڪوڊ);
        }
        
        /* swap the lead byte to scripts */
        إذا(سرو.lead_Byte_منزل_Script!=0) {
            صحيح اشاريو_ڳڻپ = ds->readUInt16(*((uint16_t*)(اندريون_بائيٽون+سرو.lead_Byte_منزل_Script))); // each entry = uint16
            صحيح معلومات_ڳڻپ = ds->readUInt16(*((uint16_t*)(اندريون_بائيٽون+سرو.lead_Byte_منزل_Script + 2))); // each entry = uint16
            ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+سرو.lead_Byte_منزل_Script, 
                                4 + (2 * اشاريو_ڳڻپ) + (2 * معلومات_ڳڻپ),
                                ٻاهريون_بائيٽون+سرو.lead_Byte_منزل_Script, p_Error_ڪوڊ);
        }
    }

    أرجع سرو.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

تعداد {
    IX_INDEXES_ڊيگهائي,  // 0
    IX_OPTIONS,
    IX_RESERVED2,
    IX_RESERVED3,

    IX_JAMO_CE32_S_شروعات,  // 4
    IX_REORDER_CODES_وٿي,
    IX_REORDER_جدول_وٿي,
    IX_TRIE_وٿي,

    IX_RESERVED8_وٿي,  // 8
    IX_CES_وٿي,
    IX_RESERVED10_وٿي,
    IX_CE32_S_وٿي,

    IX_پاڙ_ELEMENTS_وٿي,  // 12
    IX_CONTEXTS_وٿي,
    IX_UNSAFE_BWD_وٿي,
    IX_FAST_LATIN_جدول_وٿي,

    IX_SCRIPTS_وٿي,  // 16
    IX_COMPRESSIBLE_بائيٽون_وٿي,
    IX_RESERVED18_وٿي,
    IX_ڪل_سائيز
};

int32_t
مٽا_سٽا_Format_Version4(ثابت UDataSwapper *ds,
                   ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                   UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) { أرجع 0; }

    ثابت uint8_t* اندريون_بائيٽون = static_cast<ثابت uint8_t*>(اندريون_معلومات);
    uint8_t* ٻاهريون_بائيٽون = static_cast<uint8_t*>(ٻاهريون_معلومات);

    ثابت int32_t* اندريون_Indexes = reinterpret_cast<ثابت int32_t*>(اندريون_بائيٽون);
    int32_t indexes[IX_ڪل_سائيز+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    إذا(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    int32_t indexes_ڊيگهائي=indexes[0]=udata_readInt32(ds, اندريون_Indexes[0]);
    إذا(0<=length && length<(indexes_ڊيگهائي*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    لكل(int32_t i=1; i<=IX_ڪل_سائيز && i<indexes_ڊيگهائي; ++i) {
        indexes[i]=udata_readInt32(ds, اندريون_Indexes[i]);
    }
    لكل(int32_t i=indexes_ڊيگهائي; i<=IX_ڪل_سائيز; ++i) {
        indexes[i]=-1;
    }
    اندريون_Indexes=خالي_اشارو;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    إذا(indexes_ڊيگهائي>IX_ڪل_سائيز) {
        size=indexes[IX_ڪل_سائيز];
    } وإلا إذا(indexes_ڊيگهائي>IX_REORDER_CODES_وٿي) {
        size=indexes[indexes_ڊيگهائي-1];
    } وإلا {
        size=indexes_ڊيگهائي*4;
    }
    إذا(length<0) { أرجع size; }

    إذا(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    إذا(اندريون_بائيٽون!=ٻاهريون_بائيٽون) {
        uprv_memcpy(ٻاهريون_بائيٽون, اندريون_بائيٽون, size);
    }

    // Swap the int32_t indexes[].
    ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون, indexes_ڊيگهائي * 4, ٻاهريون_بائيٽون, &error_ڪوڊ);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t اشاريو;  // one of the indexes[] slots
    int32_t وٿي;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    اشاريو = IX_REORDER_CODES_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون + وٿي, length, ٻاهريون_بائيٽون + وٿي, &error_ڪوڊ);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    اشاريو = IX_TRIE_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        utrie2_مٽا_سٽا(ds, اندريون_بائيٽون + وٿي, length, ٻاهريون_بائيٽون + وٿي, &error_ڪوڊ);
    }

    اشاريو = IX_RESERVED8_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        error_ڪوڊ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    اشاريو = IX_CES_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        ds->مٽا_سٽا_Array64(ds, اندريون_بائيٽون + وٿي, length, ٻاهريون_بائيٽون + وٿي, &error_ڪوڊ);
    }

    اشاريو = IX_RESERVED10_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        error_ڪوڊ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    اشاريو = IX_CE32_S_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون + وٿي, length, ٻاهريون_بائيٽون + وٿي, &error_ڪوڊ);
    }

    اشاريو = IX_پاڙ_ELEMENTS_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون + وٿي, length, ٻاهريون_بائيٽون + وٿي, &error_ڪوڊ);
    }

    اشاريو = IX_CONTEXTS_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون + وٿي, length, ٻاهريون_بائيٽون + وٿي, &error_ڪوڊ);
    }

    اشاريو = IX_UNSAFE_BWD_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون + وٿي, length, ٻاهريون_بائيٽون + وٿي, &error_ڪوڊ);
    }

    اشاريو = IX_FAST_LATIN_جدول_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون + وٿي, length, ٻاهريون_بائيٽون + وٿي, &error_ڪوڊ);
    }

    اشاريو = IX_SCRIPTS_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون + وٿي, length, ٻاهريون_بائيٽون + وٿي, &error_ڪوڊ);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    اشاريو = IX_RESERVED18_وٿي;
    وٿي = indexes[اشاريو];
    length = indexes[اشاريو + 1] - وٿي;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        error_ڪوڊ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    أرجع size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_مٽا_سٽا(ثابت UDataSwapper *ds,
          ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
          UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(U_FAILURE(*p_Error_ڪوڊ)) { أرجع 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t سرو_سائيز=udata_مٽا_سٽا_معلومات_سرو(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        // Try to swap the old format version which did not have a standard data header.
        *p_Error_ڪوڊ=U_ZERO_ERROR;
        أرجع مٽا_سٽا_Format_Version3(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    }

    /* check data format and format version */
    ثابت UDataInfo &info=*(ثابت UDataInfo *)((ثابت حرف *)اندريون_معلومات+4);
    إذا(!(
        info.معلومات_Format[0]==0x55 &&   // dataFormat="UCol"
        info.معلومات_Format[1]==0x43 &&
        info.معلومات_Format[2]==0x6f &&
        info.معلومات_Format[3]==0x6c &&
        (3<=info.format_نسخو[0] && info.format_نسخو[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.معلومات_Format[0], info.معلومات_Format[1],
                         info.معلومات_Format[2], info.معلومات_Format[3],
                         info.format_نسخو[0], info.format_نسخو[1]);
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    اندريون_معلومات=(ثابت حرف *)اندريون_معلومات+سرو_سائيز;
    إذا(length>=0) { length-=سرو_سائيز; }
    ٻاهريون_معلومات=(ٻاهريون_معلومات == خالي_اشارو) ? خالي_اشارو : (حرف *)ٻاهريون_معلومات+سرو_سائيز;
    int32_t collation_سائيز;
    إذا(info.format_نسخو[0]>=4) {
        collation_سائيز=مٽا_سٽا_Format_Version4(ds, اندريون_معلومات, length, ٻاهريون_معلومات, *p_Error_ڪوڊ);
    } وإلا {
        collation_سائيز=مٽا_سٽا_Format_Version3(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    }
    إذا(U_SUCCESS(*p_Error_ڪوڊ)) {
        أرجع سرو_سائيز+collation_سائيز;
    } وإلا {
        أرجع 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_مٽا_سٽا_Inverse_UCA(ثابت UDataSwapper *ds,
                    ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                    UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UDataInfo *pInfo;
    int32_t سرو_سائيز;

    ثابت uint8_t *اندريون_بائيٽون;
    uint8_t *ٻاهريون_بائيٽون;

    ثابت Inverse_UCATable_سرو *اندريون_سرو;
    Inverse_UCATable_سرو *ٻاهريون_سرو;
    Inverse_UCATable_سرو سرو={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    سرو_سائيز=udata_مٽا_سٽا_معلومات_سرو(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندريون_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->معلومات_Format[1]==0x6e &&
        pInfo->معلومات_Format[2]==0x76 &&
        pInfo->معلومات_Format[3]==0x43 &&
        pInfo->format_نسخو[0]==2 &&
        pInfo->format_نسخو[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخو[0], pInfo->format_نسخو[1]);
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    اندريون_بائيٽون=(ثابت uint8_t *)اندريون_معلومات+سرو_سائيز;
    ٻاهريون_بائيٽون=(uint8_t *)ٻاهريون_معلومات+سرو_سائيز;

    اندريون_سرو=(ثابت Inverse_UCATable_سرو *)اندريون_بائيٽون;
    ٻاهريون_سرو=(Inverse_UCATable_سرو *)ٻاهريون_بائيٽون;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    إذا(length<0) {
        سرو.byte_سائيز=udata_readInt32(ds, اندريون_سرو->byte_سائيز);
    } وإلا إذا(
        ((length-سرو_سائيز)<(8*4) ||
         (uint32_t)(length-سرو_سائيز)<(سرو.byte_سائيز=udata_readInt32(ds, اندريون_سرو->byte_سائيز)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        إذا(اندريون_بائيٽون!=ٻاهريون_بائيٽون) {
            uprv_memcpy(ٻاهريون_بائيٽون, اندريون_بائيٽون, سرو.byte_سائيز);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        سرو.جدول_سائيز=   ds->readUInt32(اندريون_سرو->جدول_سائيز);
        سرو.conts_سائيز=   ds->readUInt32(اندريون_سرو->conts_سائيز);
        سرو.جدول=       ds->readUInt32(اندريون_سرو->جدول);
        سرو.conts=       ds->readUInt32(اندريون_سرو->conts);

        /* swap the 32-bit integers in the header */
        ds->مٽا_سٽا_Array32(ds, اندريون_سرو, 5*4, ٻاهريون_سرو, p_Error_ڪوڊ);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+سرو.جدول, سرو.جدول_سائيز*3*4,
                           ٻاهريون_بائيٽون+سرو.جدول, p_Error_ڪوڊ);

        /* swap the continuation table; contsSize counts UChars */
        ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+سرو.conts, سرو.conts_سائيز*U_SIZEOF_UCHAR,
                           ٻاهريون_بائيٽون+سرو.conts, p_Error_ڪوڊ);
    }

    أرجع سرو_سائيز+سرو.byte_سائيز;
}

#جيڪڏهن_پورو /* #if !UCONFIG_NO_COLLATION */
