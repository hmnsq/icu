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
ucol_looks_Like_Collation_دودویی(ثابت UDataSwapper *ds,
                              ثابت فراغ *درون_داده, int32_t length) {
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || length<-1) {
        أرجع خطأ;
    }

    // First check for format version 4+ which has a standard data header.
    UError_کد error_کد=U_ZERO_ERROR;
    (فراغ)udata_جابجا_داده_سرآیند(ds, درون_داده, -1, اشاره_تهی, &error_کد);
    إذا(U_SUCCESS(error_کد)) {
        ثابت UDataInfo &info=*(ثابت UDataInfo *)((ثابت حرف *)درون_داده+4);
        إذا(info.داده_Format[0]==0x55 &&   // dataFormat="UCol"
                info.داده_Format[1]==0x43 &&
                info.داده_Format[2]==0x6f &&
                info.داده_Format[3]==0x6c) {
            أرجع درست;
        }
    }

    // Else check for format version 3.
    ثابت UCATable_سرآیند *درون_سرآیند=(ثابت UCATable_سرآیند *)درون_داده;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATable_سرآیند سرآیند;
    uprv_memset(&سرآیند, 0, حجم(سرآیند));
    إذا(length<0) {
        سرآیند.size=udata_readInt32(ds, درون_سرآیند->size);
    } وإلا إذا((length<(42*4) || length<(سرآیند.size=udata_readInt32(ds, درون_سرآیند->size)))) {
        أرجع خطأ;
    }

    سرآیند.magic=ds->readUInt32(درون_سرآیند->magic);
    إذا(!(
        سرآیند.magic==UCOL_سرآیند_MAGIC &&
        درون_سرآیند->format_نسخه[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        أرجع خطأ;
    }

    إذا(درون_سرآیند->isBigEndian!=ds->درون_Is_Big_Endian || درون_سرآیند->char_مجموعه_Family!=ds->درون_Charset) {
        أرجع خطأ;
    }

    أرجع درست;
}

نطاق {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
جابجا_Format_Version3(ثابت UDataSwapper *ds,
                   ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                   UError_کد *p_Error_کد) {
    ثابت uint8_t *درون_بایتها;
    uint8_t *بیرون_بایتها;

    ثابت UCATable_سرآیند *درون_سرآیند;
    UCATable_سرآیند *بیرون_سرآیند;
    UCATable_سرآیند سرآیند;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || length<-1 || (length>0 && بیرون_داده==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    درون_بایتها = static_cast<ثابت uint8_t*>(درون_داده);
    بیرون_بایتها = static_cast<uint8_t*>(بیرون_داده);

    درون_سرآیند = static_cast<ثابت UCATable_سرآیند*>(درون_داده);
    بیرون_سرآیند = static_cast<UCATable_سرآیند*>(بیرون_داده);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&سرآیند, 0, حجم(سرآیند));
    إذا(length<0) {
        سرآیند.size=udata_readInt32(ds, درون_سرآیند->size);
    } وإلا إذا((length<(42*4) || length<(سرآیند.size=udata_readInt32(ds, درون_سرآیند->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    سرآیند.magic=ds->readUInt32(درون_سرآیند->magic);
    إذا(!(
        سرآیند.magic==UCOL_سرآیند_MAGIC &&
        درون_سرآیند->format_نسخه[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         سرآیند.magic,
                         درون_سرآیند->format_نسخه[0], درون_سرآیند->format_نسخه[1]);
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    إذا(درون_سرآیند->isBigEndian!=ds->درون_Is_Big_Endian || درون_سرآیند->char_مجموعه_Family!=ds->درون_Charset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         درون_سرآیند->isBigEndian, درون_سرآیند->char_مجموعه_Family);
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        إذا(درون_بایتها!=بیرون_بایتها) {
            uprv_memcpy(بیرون_بایتها, درون_بایتها, سرآیند.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        سرآیند.options=                 ds->readUInt32(درون_سرآیند->options);
        سرآیند.UCAConsts=               ds->readUInt32(درون_سرآیند->UCAConsts);
        سرآیند.contractionUCACombos=    ds->readUInt32(درون_سرآیند->contractionUCACombos);
        سرآیند.mappingPosition=         ds->readUInt32(درون_سرآیند->mappingPosition);
        سرآیند.expansion=               ds->readUInt32(درون_سرآیند->expansion);
        سرآیند.contraction_شاخص=        ds->readUInt32(درون_سرآیند->contraction_شاخص);
        سرآیند.contractionCEs=          ds->readUInt32(درون_سرآیند->contractionCEs);
        سرآیند.contraction_اندازه=         ds->readUInt32(درون_سرآیند->contraction_اندازه);
        سرآیند.انتها_Expansion_CE=          ds->readUInt32(درون_سرآیند->انتها_Expansion_CE);
        سرآیند.expansionCESize=         ds->readUInt32(درون_سرآیند->expansionCESize);
        سرآیند.انتها_Expansion_CECount=     udata_readInt32(ds, درون_سرآیند->انتها_Expansion_CECount);
        سرآیند.contraction_UCACombos_اندازه=udata_readInt32(ds, درون_سرآیند->contraction_UCACombos_اندازه);
        سرآیند.script_مقصد_Lead_Byte=        ds->readUInt32(درون_سرآیند->script_مقصد_Lead_Byte);
        سرآیند.lead_Byte_مقصد_Script=        ds->readUInt32(درون_سرآیند->lead_Byte_مقصد_Script);
        
        /* swap the 32-bit integers in the header */
        ds->جابجا_Array32(ds, درون_سرآیند, static_cast<int32_t>(reinterpret_cast<ثابت حرف*>(&درون_سرآیند->jamoSpecial) - reinterpret_cast<ثابت حرف*>(درون_سرآیند)),
                           بیرون_سرآیند, p_Error_کد);
        ds->جابجا_Array32(ds, &(درون_سرآیند->script_مقصد_Lead_Byte), حجم(سرآیند.script_مقصد_Lead_Byte) + حجم(سرآیند.lead_Byte_مقصد_Script),
                           &(بیرون_سرآیند->script_مقصد_Lead_Byte), p_Error_کد);
        /* set the output platform properties */
        بیرون_سرآیند->isBigEndian=ds->بیرون_Is_Big_Endian;
        بیرون_سرآیند->char_مجموعه_Family=ds->بیرون_Charset;

        /* swap the options */
        إذا(سرآیند.options!=0) {
            ds->جابجا_Array32(ds, درون_بایتها+سرآیند.options, سرآیند.expansion-سرآیند.options,
                               بیرون_بایتها+سرآیند.options, p_Error_کد);
        }

        /* swap the expansions */
        إذا(سرآیند.mappingPosition!=0 && سرآیند.expansion!=0) {
            إذا(سرآیند.contraction_شاخص!=0) {
                /* expansions bounded by contractions */
                count=سرآیند.contraction_شاخص-سرآیند.expansion;
            } وإلا {
                /* no contractions: expansions bounded by the main trie */
                count=سرآیند.mappingPosition-سرآیند.expansion;
            }
            ds->جابجا_Array32(ds, درون_بایتها + سرآیند.expansion, static_cast<int32_t>(count),
                               بیرون_بایتها+سرآیند.expansion, p_Error_کد);
        }

        /* swap the contractions */
        إذا(سرآیند.contraction_اندازه!=0) {
            /* contractionIndex: char16_t[] */
            ds->جابجا_Array16(ds, درون_بایتها+سرآیند.contraction_شاخص, سرآیند.contraction_اندازه*2,
                               بیرون_بایتها+سرآیند.contraction_شاخص, p_Error_کد);

            /* contractionCEs: CEs[] */
            ds->جابجا_Array32(ds, درون_بایتها+سرآیند.contractionCEs, سرآیند.contraction_اندازه*4,
                               بیرون_بایتها+سرآیند.contractionCEs, p_Error_کد);
        }

        /* swap the main trie */
        إذا(سرآیند.mappingPosition!=0) {
            count=سرآیند.انتها_Expansion_CE-سرآیند.mappingPosition;
            utrie_جابجا(ds, درون_بایتها + سرآیند.mappingPosition, static_cast<int32_t>(count),
                          بیرون_بایتها+سرآیند.mappingPosition, p_Error_کد);
        }

        /* swap the max expansion table */
        إذا(سرآیند.انتها_Expansion_CECount!=0) {
            ds->جابجا_Array32(ds, درون_بایتها+سرآیند.انتها_Expansion_CE, سرآیند.انتها_Expansion_CECount*4,
                               بیرون_بایتها+سرآیند.انتها_Expansion_CE, p_Error_کد);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        إذا(سرآیند.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->جابجا_Array32(ds, درون_بایتها+سرآیند.UCAConsts, سرآیند.contractionUCACombos-سرآیند.UCAConsts,
                               بیرون_بایتها+سرآیند.UCAConsts, p_Error_کد);
        }

        /* swap UCA contractions */
        إذا(سرآیند.contraction_UCACombos_اندازه!=0) {
            count=سرآیند.contraction_UCACombos_اندازه*درون_سرآیند->contraction_UCACombos_عرض*U_SIZEOF_UCHAR;
            ds->جابجا_Array16(ds, درون_بایتها + سرآیند.contractionUCACombos, static_cast<int32_t>(count),
                               بیرون_بایتها+سرآیند.contractionUCACombos, p_Error_کد);
        }
        
        /* swap the script to lead bytes */
        إذا(سرآیند.script_مقصد_Lead_Byte!=0) {
            صحيح شاخص_شمار = ds->readUInt16(*((uint16_t*)(درون_بایتها+سرآیند.script_مقصد_Lead_Byte))); // each entry = 2 * uint16
            صحيح داده_شمار = ds->readUInt16(*((uint16_t*)(درون_بایتها+سرآیند.script_مقصد_Lead_Byte + 2))); // each entry = uint16
            ds->جابجا_Array16(ds, درون_بایتها+سرآیند.script_مقصد_Lead_Byte, 
                                4 + (4 * شاخص_شمار) + (2 * داده_شمار),
                                بیرون_بایتها+سرآیند.script_مقصد_Lead_Byte, p_Error_کد);
        }
        
        /* swap the lead byte to scripts */
        إذا(سرآیند.lead_Byte_مقصد_Script!=0) {
            صحيح شاخص_شمار = ds->readUInt16(*((uint16_t*)(درون_بایتها+سرآیند.lead_Byte_مقصد_Script))); // each entry = uint16
            صحيح داده_شمار = ds->readUInt16(*((uint16_t*)(درون_بایتها+سرآیند.lead_Byte_مقصد_Script + 2))); // each entry = uint16
            ds->جابجا_Array16(ds, درون_بایتها+سرآیند.lead_Byte_مقصد_Script, 
                                4 + (2 * شاخص_شمار) + (2 * داده_شمار),
                                بیرون_بایتها+سرآیند.lead_Byte_مقصد_Script, p_Error_کد);
        }
    }

    أرجع سرآیند.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

تعداد {
    IX_INDEXES_درازی,  // 0
    IX_OPTIONS,
    IX_RESERVED2,
    IX_RESERVED3,

    IX_JAMO_CE32_S_شروع,  // 4
    IX_REORDER_CODES_جابه_جایی,
    IX_REORDER_جدول_جابه_جایی,
    IX_TRIE_جابه_جایی,

    IX_RESERVED8_جابه_جایی,  // 8
    IX_CES_جابه_جایی,
    IX_RESERVED10_جابه_جایی,
    IX_CE32_S_جابه_جایی,

    IX_ریشه_ELEMENTS_جابه_جایی,  // 12
    IX_CONTEXTS_جابه_جایی,
    IX_UNSAFE_BWD_جابه_جایی,
    IX_FAST_LATIN_جدول_جابه_جایی,

    IX_SCRIPTS_جابه_جایی,  // 16
    IX_COMPRESSIBLE_بایتها_جابه_جایی,
    IX_RESERVED18_جابه_جایی,
    IX_مجموع_اندازه
};

int32_t
جابجا_Format_Version4(ثابت UDataSwapper *ds,
                   ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                   UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) { أرجع 0; }

    ثابت uint8_t* درون_بایتها = static_cast<ثابت uint8_t*>(درون_داده);
    uint8_t* بیرون_بایتها = static_cast<uint8_t*>(بیرون_داده);

    ثابت int32_t* درون_Indexes = reinterpret_cast<ثابت int32_t*>(درون_بایتها);
    int32_t indexes[IX_مجموع_اندازه+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    إذا(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    int32_t indexes_درازی=indexes[0]=udata_readInt32(ds, درون_Indexes[0]);
    إذا(0<=length && length<(indexes_درازی*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    لكل(int32_t i=1; i<=IX_مجموع_اندازه && i<indexes_درازی; ++i) {
        indexes[i]=udata_readInt32(ds, درون_Indexes[i]);
    }
    لكل(int32_t i=indexes_درازی; i<=IX_مجموع_اندازه; ++i) {
        indexes[i]=-1;
    }
    درون_Indexes=اشاره_تهی;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    إذا(indexes_درازی>IX_مجموع_اندازه) {
        size=indexes[IX_مجموع_اندازه];
    } وإلا إذا(indexes_درازی>IX_REORDER_CODES_جابه_جایی) {
        size=indexes[indexes_درازی-1];
    } وإلا {
        size=indexes_درازی*4;
    }
    إذا(length<0) { أرجع size; }

    إذا(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    إذا(درون_بایتها!=بیرون_بایتها) {
        uprv_memcpy(بیرون_بایتها, درون_بایتها, size);
    }

    // Swap the int32_t indexes[].
    ds->جابجا_Array32(ds, درون_بایتها, indexes_درازی * 4, بیرون_بایتها, &error_کد);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t شاخص;  // one of the indexes[] slots
    int32_t جابه_جایی;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    شاخص = IX_REORDER_CODES_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        ds->جابجا_Array32(ds, درون_بایتها + جابه_جایی, length, بیرون_بایتها + جابه_جایی, &error_کد);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    شاخص = IX_TRIE_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        utrie2_جابجا(ds, درون_بایتها + جابه_جایی, length, بیرون_بایتها + جابه_جایی, &error_کد);
    }

    شاخص = IX_RESERVED8_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        error_کد = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    شاخص = IX_CES_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        ds->جابجا_Array64(ds, درون_بایتها + جابه_جایی, length, بیرون_بایتها + جابه_جایی, &error_کد);
    }

    شاخص = IX_RESERVED10_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        error_کد = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    شاخص = IX_CE32_S_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        ds->جابجا_Array32(ds, درون_بایتها + جابه_جایی, length, بیرون_بایتها + جابه_جایی, &error_کد);
    }

    شاخص = IX_ریشه_ELEMENTS_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        ds->جابجا_Array32(ds, درون_بایتها + جابه_جایی, length, بیرون_بایتها + جابه_جایی, &error_کد);
    }

    شاخص = IX_CONTEXTS_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        ds->جابجا_Array16(ds, درون_بایتها + جابه_جایی, length, بیرون_بایتها + جابه_جایی, &error_کد);
    }

    شاخص = IX_UNSAFE_BWD_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        ds->جابجا_Array16(ds, درون_بایتها + جابه_جایی, length, بیرون_بایتها + جابه_جایی, &error_کد);
    }

    شاخص = IX_FAST_LATIN_جدول_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        ds->جابجا_Array16(ds, درون_بایتها + جابه_جایی, length, بیرون_بایتها + جابه_جایی, &error_کد);
    }

    شاخص = IX_SCRIPTS_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        ds->جابجا_Array16(ds, درون_بایتها + جابه_جایی, length, بیرون_بایتها + جابه_جایی, &error_کد);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    شاخص = IX_RESERVED18_جابه_جایی;
    جابه_جایی = indexes[شاخص];
    length = indexes[شاخص + 1] - جابه_جایی;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        error_کد = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    أرجع size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_جابجا(ثابت UDataSwapper *ds,
          ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
          UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) { أرجع 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t سرآیند_اندازه=udata_جابجا_داده_سرآیند(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    إذا(U_FAILURE(*p_Error_کد)) {
        // Try to swap the old format version which did not have a standard data header.
        *p_Error_کد=U_ZERO_ERROR;
        أرجع جابجا_Format_Version3(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    }

    /* check data format and format version */
    ثابت UDataInfo &info=*(ثابت UDataInfo *)((ثابت حرف *)درون_داده+4);
    إذا(!(
        info.داده_Format[0]==0x55 &&   // dataFormat="UCol"
        info.داده_Format[1]==0x43 &&
        info.داده_Format[2]==0x6f &&
        info.داده_Format[3]==0x6c &&
        (3<=info.format_نسخه[0] && info.format_نسخه[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.داده_Format[0], info.داده_Format[1],
                         info.داده_Format[2], info.داده_Format[3],
                         info.format_نسخه[0], info.format_نسخه[1]);
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    درون_داده=(ثابت حرف *)درون_داده+سرآیند_اندازه;
    إذا(length>=0) { length-=سرآیند_اندازه; }
    بیرون_داده=(بیرون_داده == اشاره_تهی) ? اشاره_تهی : (حرف *)بیرون_داده+سرآیند_اندازه;
    int32_t collation_اندازه;
    إذا(info.format_نسخه[0]>=4) {
        collation_اندازه=جابجا_Format_Version4(ds, درون_داده, length, بیرون_داده, *p_Error_کد);
    } وإلا {
        collation_اندازه=جابجا_Format_Version3(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    }
    إذا(U_SUCCESS(*p_Error_کد)) {
        أرجع سرآیند_اندازه+collation_اندازه;
    } وإلا {
        أرجع 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_جابجا_Inverse_UCA(ثابت UDataSwapper *ds,
                    ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                    UError_کد *p_Error_کد) {
    ثابت UDataInfo *pInfo;
    int32_t سرآیند_اندازه;

    ثابت uint8_t *درون_بایتها;
    uint8_t *بیرون_بایتها;

    ثابت Inverse_UCATable_سرآیند *درون_سرآیند;
    Inverse_UCATable_سرآیند *بیرون_سرآیند;
    Inverse_UCATable_سرآیند سرآیند={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    سرآیند_اندازه=udata_جابجا_داده_سرآیند(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)درون_داده+4);
    إذا(!(
        pInfo->داده_Format[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->داده_Format[1]==0x6e &&
        pInfo->داده_Format[2]==0x76 &&
        pInfo->داده_Format[3]==0x43 &&
        pInfo->format_نسخه[0]==2 &&
        pInfo->format_نسخه[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->داده_Format[0], pInfo->داده_Format[1],
                         pInfo->داده_Format[2], pInfo->داده_Format[3],
                         pInfo->format_نسخه[0], pInfo->format_نسخه[1]);
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    درون_بایتها=(ثابت uint8_t *)درون_داده+سرآیند_اندازه;
    بیرون_بایتها=(uint8_t *)بیرون_داده+سرآیند_اندازه;

    درون_سرآیند=(ثابت Inverse_UCATable_سرآیند *)درون_بایتها;
    بیرون_سرآیند=(Inverse_UCATable_سرآیند *)بیرون_بایتها;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    إذا(length<0) {
        سرآیند.byte_اندازه=udata_readInt32(ds, درون_سرآیند->byte_اندازه);
    } وإلا إذا(
        ((length-سرآیند_اندازه)<(8*4) ||
         (uint32_t)(length-سرآیند_اندازه)<(سرآیند.byte_اندازه=udata_readInt32(ds, درون_سرآیند->byte_اندازه)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        إذا(درون_بایتها!=بیرون_بایتها) {
            uprv_memcpy(بیرون_بایتها, درون_بایتها, سرآیند.byte_اندازه);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        سرآیند.جدول_اندازه=   ds->readUInt32(درون_سرآیند->جدول_اندازه);
        سرآیند.conts_اندازه=   ds->readUInt32(درون_سرآیند->conts_اندازه);
        سرآیند.جدول=       ds->readUInt32(درون_سرآیند->جدول);
        سرآیند.conts=       ds->readUInt32(درون_سرآیند->conts);

        /* swap the 32-bit integers in the header */
        ds->جابجا_Array32(ds, درون_سرآیند, 5*4, بیرون_سرآیند, p_Error_کد);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->جابجا_Array32(ds, درون_بایتها+سرآیند.جدول, سرآیند.جدول_اندازه*3*4,
                           بیرون_بایتها+سرآیند.جدول, p_Error_کد);

        /* swap the continuation table; contsSize counts UChars */
        ds->جابجا_Array16(ds, درون_بایتها+سرآیند.conts, سرآیند.conts_اندازه*U_SIZEOF_UCHAR,
                           بیرون_بایتها+سرآیند.conts, p_Error_کد);
    }

    أرجع سرآیند_اندازه+سرآیند.byte_اندازه;
}

#پایان_اگر /* #if !UCONFIG_NO_COLLATION */
