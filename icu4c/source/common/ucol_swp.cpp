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

#अन्तर्भाव "unicode/udata.h" /* UDataInfo */
#अन्तर्भाव "utrie.h"
#अन्तर्भाव "utrie2.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ucol_data.h"
#अन्तर्भाव "ucol_swp.h"

/* swapping ----------------------------------------------------------------- */

#यदि !UCONFIG_NO_COLLATION

U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_द्विमान(ध्रुव UDataSwapper *ds,
                              ध्रुव शून्य *अन्तर्_दत्त, int32_t length) {
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || length<-1) {
        प्रत्यागम असत्य;
    }

    // First check for format version 4+ which has a standard data header.
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    (शून्य)udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, -1, शून्यसूचकः, &error_संहिता);
    यदि(U_SUCCESS(error_संहिता)) {
        ध्रुव UDataInfo &info=*(ध्रुव UDataInfo *)((ध्रुव वर्ण *)अन्तर्_दत्त+4);
        यदि(info.दत्त_Format[0]==0x55 &&   // dataFormat="UCol"
                info.दत्त_Format[1]==0x43 &&
                info.दत्त_Format[2]==0x6f &&
                info.दत्त_Format[3]==0x6c) {
            प्रत्यागम सत्य;
        }
    }

    // Else check for format version 3.
    ध्रुव UCATable_शीर्षकम् *अन्तर्_शीर्षकम्=(ध्रुव UCATable_शीर्षकम् *)अन्तर्_दत्त;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATable_शीर्षकम् शीर्षकम्;
    uprv_memset(&शीर्षकम्, 0, परिमाण(शीर्षकम्));
    यदि(length<0) {
        शीर्षकम्.size=udata_readInt32(ds, अन्तर्_शीर्षकम्->size);
    } अन्यथा यदि((length<(42*4) || length<(शीर्षकम्.size=udata_readInt32(ds, अन्तर्_शीर्षकम्->size)))) {
        प्रत्यागम असत्य;
    }

    शीर्षकम्.magic=ds->readUInt32(अन्तर्_शीर्षकम्->magic);
    यदि(!(
        शीर्षकम्.magic==UCOL_शीर्षकम्_MAGIC &&
        अन्तर्_शीर्षकम्->format_संस्करणम्[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        प्रत्यागम असत्य;
    }

    यदि(अन्तर्_शीर्षकम्->isBigEndian!=ds->अन्तर्_Is_Big_Endian || अन्तर्_शीर्षकम्->char_समुच्चयः_Family!=ds->अन्तर्_Charset) {
        प्रत्यागम असत्य;
    }

    प्रत्यागम सत्य;
}

नामाकाश {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
swapFormatVersion3(ध्रुव UDataSwapper *ds,
                   ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                   UError_संहिता *p_Error_संहिता) {
    ध्रुव uint8_t *अन्तर्_अष्टकानि;
    uint8_t *बहिर्गमनम्_अष्टकानि;

    ध्रुव UCATable_शीर्षकम् *अन्तर्_शीर्षकम्;
    UCATable_शीर्षकम् *बहिर्गमनम्_शीर्षकम्;
    UCATable_शीर्षकम् शीर्षकम्;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || length<-1 || (length>0 && बहिर्गमनम्_दत्त==शून्यसूचकः)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_अष्टकानि = static_cast<ध्रुव uint8_t*>(अन्तर्_दत्त);
    बहिर्गमनम्_अष्टकानि = static_cast<uint8_t*>(बहिर्गमनम्_दत्त);

    अन्तर्_शीर्षकम् = static_cast<ध्रुव UCATable_शीर्षकम्*>(अन्तर्_दत्त);
    बहिर्गमनम्_शीर्षकम् = static_cast<UCATable_शीर्षकम्*>(बहिर्गमनम्_दत्त);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&शीर्षकम्, 0, परिमाण(शीर्षकम्));
    यदि(length<0) {
        शीर्षकम्.size=udata_readInt32(ds, अन्तर्_शीर्षकम्->size);
    } अन्यथा यदि((length<(42*4) || length<(शीर्षकम्.size=udata_readInt32(ds, अन्तर्_शीर्षकम्->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    शीर्षकम्.magic=ds->readUInt32(अन्तर्_शीर्षकम्->magic);
    यदि(!(
        शीर्षकम्.magic==UCOL_शीर्षकम्_MAGIC &&
        अन्तर्_शीर्षकम्->format_संस्करणम्[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         शीर्षकम्.magic,
                         अन्तर्_शीर्षकम्->format_संस्करणम्[0], अन्तर्_शीर्षकम्->format_संस्करणम्[1]);
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    यदि(अन्तर्_शीर्षकम्->isBigEndian!=ds->अन्तर्_Is_Big_Endian || अन्तर्_शीर्षकम्->char_समुच्चयः_Family!=ds->अन्तर्_Charset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         अन्तर्_शीर्षकम्->isBigEndian, अन्तर्_शीर्षकम्->char_समुच्चयः_Family);
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम 0;
    }

    यदि(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        यदि(अन्तर्_अष्टकानि!=बहिर्गमनम्_अष्टकानि) {
            uprv_memcpy(बहिर्गमनम्_अष्टकानि, अन्तर्_अष्टकानि, शीर्षकम्.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        शीर्षकम्.options=                 ds->readUInt32(अन्तर्_शीर्षकम्->options);
        शीर्षकम्.UCAConsts=               ds->readUInt32(अन्तर्_शीर्षकम्->UCAConsts);
        शीर्षकम्.contractionUCACombos=    ds->readUInt32(अन्तर्_शीर्षकम्->contractionUCACombos);
        शीर्षकम्.mappingPosition=         ds->readUInt32(अन्तर्_शीर्षकम्->mappingPosition);
        शीर्षकम्.expansion=               ds->readUInt32(अन्तर्_शीर्षकम्->expansion);
        शीर्षकम्.contraction_सूचकांक=        ds->readUInt32(अन्तर्_शीर्षकम्->contraction_सूचकांक);
        शीर्षकम्.contractionCEs=          ds->readUInt32(अन्तर्_शीर्षकम्->contractionCEs);
        शीर्षकम्.contraction_आकार=         ds->readUInt32(अन्तर्_शीर्षकम्->contraction_आकार);
        शीर्षकम्.अन्तः_Expansion_CE=          ds->readUInt32(अन्तर्_शीर्षकम्->अन्तः_Expansion_CE);
        शीर्षकम्.expansionCESize=         ds->readUInt32(अन्तर्_शीर्षकम्->expansionCESize);
        शीर्षकम्.अन्तः_Expansion_CECount=     udata_readInt32(ds, अन्तर्_शीर्षकम्->अन्तः_Expansion_CECount);
        शीर्षकम्.contraction_UCACombos_आकार=udata_readInt32(ds, अन्तर्_शीर्षकम्->contraction_UCACombos_आकार);
        शीर्षकम्.script_गन्तव्यम्_Lead_Byte=        ds->readUInt32(अन्तर्_शीर्षकम्->script_गन्तव्यम्_Lead_Byte);
        शीर्षकम्.lead_Byte_गन्तव्यम्_Script=        ds->readUInt32(अन्तर्_शीर्षकम्->lead_Byte_गन्तव्यम्_Script);
        
        /* swap the 32-bit integers in the header */
        ds->swapArray32(ds, अन्तर्_शीर्षकम्, static_cast<int32_t>(reinterpret_cast<ध्रुव वर्ण*>(&अन्तर्_शीर्षकम्->jamoSpecial) - reinterpret_cast<ध्रुव वर्ण*>(अन्तर्_शीर्षकम्)),
                           बहिर्गमनम्_शीर्षकम्, p_Error_संहिता);
        ds->swapArray32(ds, &(अन्तर्_शीर्षकम्->script_गन्तव्यम्_Lead_Byte), परिमाण(शीर्षकम्.script_गन्तव्यम्_Lead_Byte) + परिमाण(शीर्षकम्.lead_Byte_गन्तव्यम्_Script),
                           &(बहिर्गमनम्_शीर्षकम्->script_गन्तव्यम्_Lead_Byte), p_Error_संहिता);
        /* set the output platform properties */
        बहिर्गमनम्_शीर्षकम्->isBigEndian=ds->बहिर्गमनम्_Is_Big_Endian;
        बहिर्गमनम्_शीर्षकम्->char_समुच्चयः_Family=ds->बहिर्गमनम्_Charset;

        /* swap the options */
        यदि(शीर्षकम्.options!=0) {
            ds->swapArray32(ds, अन्तर्_अष्टकानि+शीर्षकम्.options, शीर्षकम्.expansion-शीर्षकम्.options,
                               बहिर्गमनम्_अष्टकानि+शीर्षकम्.options, p_Error_संहिता);
        }

        /* swap the expansions */
        यदि(शीर्षकम्.mappingPosition!=0 && शीर्षकम्.expansion!=0) {
            यदि(शीर्षकम्.contraction_सूचकांक!=0) {
                /* expansions bounded by contractions */
                count=शीर्षकम्.contraction_सूचकांक-शीर्षकम्.expansion;
            } अन्यथा {
                /* no contractions: expansions bounded by the main trie */
                count=शीर्षकम्.mappingPosition-शीर्षकम्.expansion;
            }
            ds->swapArray32(ds, अन्तर्_अष्टकानि + शीर्षकम्.expansion, static_cast<int32_t>(count),
                               बहिर्गमनम्_अष्टकानि+शीर्षकम्.expansion, p_Error_संहिता);
        }

        /* swap the contractions */
        यदि(शीर्षकम्.contraction_आकार!=0) {
            /* contractionIndex: char16_t[] */
            ds->swapArray16(ds, अन्तर्_अष्टकानि+शीर्षकम्.contraction_सूचकांक, शीर्षकम्.contraction_आकार*2,
                               बहिर्गमनम्_अष्टकानि+शीर्षकम्.contraction_सूचकांक, p_Error_संहिता);

            /* contractionCEs: CEs[] */
            ds->swapArray32(ds, अन्तर्_अष्टकानि+शीर्षकम्.contractionCEs, शीर्षकम्.contraction_आकार*4,
                               बहिर्गमनम्_अष्टकानि+शीर्षकम्.contractionCEs, p_Error_संहिता);
        }

        /* swap the main trie */
        यदि(शीर्षकम्.mappingPosition!=0) {
            count=शीर्षकम्.अन्तः_Expansion_CE-शीर्षकम्.mappingPosition;
            utrie_swap(ds, अन्तर्_अष्टकानि + शीर्षकम्.mappingPosition, static_cast<int32_t>(count),
                          बहिर्गमनम्_अष्टकानि+शीर्षकम्.mappingPosition, p_Error_संहिता);
        }

        /* swap the max expansion table */
        यदि(शीर्षकम्.अन्तः_Expansion_CECount!=0) {
            ds->swapArray32(ds, अन्तर्_अष्टकानि+शीर्षकम्.अन्तः_Expansion_CE, शीर्षकम्.अन्तः_Expansion_CECount*4,
                               बहिर्गमनम्_अष्टकानि+शीर्षकम्.अन्तः_Expansion_CE, p_Error_संहिता);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        यदि(शीर्षकम्.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->swapArray32(ds, अन्तर्_अष्टकानि+शीर्षकम्.UCAConsts, शीर्षकम्.contractionUCACombos-शीर्षकम्.UCAConsts,
                               बहिर्गमनम्_अष्टकानि+शीर्षकम्.UCAConsts, p_Error_संहिता);
        }

        /* swap UCA contractions */
        यदि(शीर्षकम्.contraction_UCACombos_आकार!=0) {
            count=शीर्षकम्.contraction_UCACombos_आकार*अन्तर्_शीर्षकम्->contraction_UCACombos_विस्तारः*U_SIZEOF_UCHAR;
            ds->swapArray16(ds, अन्तर्_अष्टकानि + शीर्षकम्.contractionUCACombos, static_cast<int32_t>(count),
                               बहिर्गमनम्_अष्टकानि+शीर्षकम्.contractionUCACombos, p_Error_संहिता);
        }
        
        /* swap the script to lead bytes */
        यदि(शीर्षकम्.script_गन्तव्यम्_Lead_Byte!=0) {
            पूर्णाङ्क सूचकांक_गणनम् = ds->readUInt16(*((uint16_t*)(अन्तर्_अष्टकानि+शीर्षकम्.script_गन्तव्यम्_Lead_Byte))); // each entry = 2 * uint16
            पूर्णाङ्क दत्त_गणनम् = ds->readUInt16(*((uint16_t*)(अन्तर्_अष्टकानि+शीर्षकम्.script_गन्तव्यम्_Lead_Byte + 2))); // each entry = uint16
            ds->swapArray16(ds, अन्तर्_अष्टकानि+शीर्षकम्.script_गन्तव्यम्_Lead_Byte, 
                                4 + (4 * सूचकांक_गणनम्) + (2 * दत्त_गणनम्),
                                बहिर्गमनम्_अष्टकानि+शीर्षकम्.script_गन्तव्यम्_Lead_Byte, p_Error_संहिता);
        }
        
        /* swap the lead byte to scripts */
        यदि(शीर्षकम्.lead_Byte_गन्तव्यम्_Script!=0) {
            पूर्णाङ्क सूचकांक_गणनम् = ds->readUInt16(*((uint16_t*)(अन्तर्_अष्टकानि+शीर्षकम्.lead_Byte_गन्तव्यम्_Script))); // each entry = uint16
            पूर्णाङ्क दत्त_गणनम् = ds->readUInt16(*((uint16_t*)(अन्तर्_अष्टकानि+शीर्षकम्.lead_Byte_गन्तव्यम्_Script + 2))); // each entry = uint16
            ds->swapArray16(ds, अन्तर्_अष्टकानि+शीर्षकम्.lead_Byte_गन्तव्यम्_Script, 
                                4 + (2 * सूचकांक_गणनम्) + (2 * दत्त_गणनम्),
                                बहिर्गमनम्_अष्टकानि+शीर्षकम्.lead_Byte_गन्तव्यम्_Script, p_Error_संहिता);
        }
    }

    प्रत्यागम शीर्षकम्.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

गणना {
    IX_INDEXES_दीर्घता,  // 0
    IX_OPTIONS,
    IX_RESERVED2,
    IX_RESERVED3,

    IX_JAMO_CE32S_START,  // 4
    IX_REORDER_CODES_विस्थापनम्,
    IX_REORDER_सारणी_विस्थापनम्,
    IX_TRIE_विस्थापनम्,

    IX_RESERVED8_विस्थापनम्,  // 8
    IX_CES_विस्थापनम्,
    IX_RESERVED10_विस्थापनम्,
    IX_CE32_S_विस्थापनम्,

    IX_मूलम्_ELEMENTS_विस्थापनम्,  // 12
    IX_CONTEXTS_विस्थापनम्,
    IX_UNSAFE_BWD_विस्थापनम्,
    IX_FAST_LATIN_सारणी_विस्थापनम्,

    IX_SCRIPTS_विस्थापनम्,  // 16
    IX_COMPRESSIBLE_अष्टकानि_विस्थापनम्,
    IX_RESERVED18_विस्थापनम्,
    IX_योग_आकार
};

int32_t
swapFormatVersion4(ध्रुव UDataSwapper *ds,
                   ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                   UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) { प्रत्यागम 0; }

    ध्रुव uint8_t* अन्तर्_अष्टकानि = static_cast<ध्रुव uint8_t*>(अन्तर्_दत्त);
    uint8_t* बहिर्गमनम्_अष्टकानि = static_cast<uint8_t*>(बहिर्गमनम्_दत्त);

    ध्रुव int32_t* अन्तर्_Indexes = reinterpret_cast<ध्रुव int32_t*>(अन्तर्_अष्टकानि);
    int32_t indexes[IX_योग_आकार+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    यदि(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    int32_t indexes_दीर्घता=indexes[0]=udata_readInt32(ds, अन्तर्_Indexes[0]);
    यदि(0<=length && length<(indexes_दीर्घता*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    प्रति(int32_t i=1; i<=IX_योग_आकार && i<indexes_दीर्घता; ++i) {
        indexes[i]=udata_readInt32(ds, अन्तर्_Indexes[i]);
    }
    प्रति(int32_t i=indexes_दीर्घता; i<=IX_योग_आकार; ++i) {
        indexes[i]=-1;
    }
    अन्तर्_Indexes=शून्यसूचकः;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    यदि(indexes_दीर्घता>IX_योग_आकार) {
        size=indexes[IX_योग_आकार];
    } अन्यथा यदि(indexes_दीर्घता>IX_REORDER_CODES_विस्थापनम्) {
        size=indexes[indexes_दीर्घता-1];
    } अन्यथा {
        size=indexes_दीर्घता*4;
    }
    यदि(length<0) { प्रत्यागम size; }

    यदि(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    यदि(अन्तर्_अष्टकानि!=बहिर्गमनम्_अष्टकानि) {
        uprv_memcpy(बहिर्गमनम्_अष्टकानि, अन्तर्_अष्टकानि, size);
    }

    // Swap the int32_t indexes[].
    ds->swapArray32(ds, अन्तर्_अष्टकानि, indexes_दीर्घता * 4, बहिर्गमनम्_अष्टकानि, &error_संहिता);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t सूचकांक;  // one of the indexes[] slots
    int32_t विस्थापनम्;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    सूचकांक = IX_REORDER_CODES_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        ds->swapArray32(ds, अन्तर्_अष्टकानि + विस्थापनम्, length, बहिर्गमनम्_अष्टकानि + विस्थापनम्, &error_संहिता);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    सूचकांक = IX_TRIE_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        utrie2_swap(ds, अन्तर्_अष्टकानि + विस्थापनम्, length, बहिर्गमनम्_अष्टकानि + विस्थापनम्, &error_संहिता);
    }

    सूचकांक = IX_RESERVED8_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        error_संहिता = U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    सूचकांक = IX_CES_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        ds->swapArray64(ds, अन्तर्_अष्टकानि + विस्थापनम्, length, बहिर्गमनम्_अष्टकानि + विस्थापनम्, &error_संहिता);
    }

    सूचकांक = IX_RESERVED10_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        error_संहिता = U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    सूचकांक = IX_CE32_S_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        ds->swapArray32(ds, अन्तर्_अष्टकानि + विस्थापनम्, length, बहिर्गमनम्_अष्टकानि + विस्थापनम्, &error_संहिता);
    }

    सूचकांक = IX_मूलम्_ELEMENTS_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        ds->swapArray32(ds, अन्तर्_अष्टकानि + विस्थापनम्, length, बहिर्गमनम्_अष्टकानि + विस्थापनम्, &error_संहिता);
    }

    सूचकांक = IX_CONTEXTS_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        ds->swapArray16(ds, अन्तर्_अष्टकानि + विस्थापनम्, length, बहिर्गमनम्_अष्टकानि + विस्थापनम्, &error_संहिता);
    }

    सूचकांक = IX_UNSAFE_BWD_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        ds->swapArray16(ds, अन्तर्_अष्टकानि + विस्थापनम्, length, बहिर्गमनम्_अष्टकानि + विस्थापनम्, &error_संहिता);
    }

    सूचकांक = IX_FAST_LATIN_सारणी_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        ds->swapArray16(ds, अन्तर्_अष्टकानि + विस्थापनम्, length, बहिर्गमनम्_अष्टकानि + विस्थापनम्, &error_संहिता);
    }

    सूचकांक = IX_SCRIPTS_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        ds->swapArray16(ds, अन्तर्_अष्टकानि + विस्थापनम्, length, बहिर्गमनम्_अष्टकानि + विस्थापनम्, &error_संहिता);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    सूचकांक = IX_RESERVED18_विस्थापनम्;
    विस्थापनम् = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - विस्थापनम्;
    यदि(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        error_संहिता = U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    प्रत्यागम size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_swap(ध्रुव UDataSwapper *ds,
          ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
          UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) { प्रत्यागम 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t शीर्षकम्_आकार=udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        // Try to swap the old format version which did not have a standard data header.
        *p_Error_संहिता=U_ZERO_ERROR;
        प्रत्यागम swapFormatVersion3(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    }

    /* check data format and format version */
    ध्रुव UDataInfo &info=*(ध्रुव UDataInfo *)((ध्रुव वर्ण *)अन्तर्_दत्त+4);
    यदि(!(
        info.दत्त_Format[0]==0x55 &&   // dataFormat="UCol"
        info.दत्त_Format[1]==0x43 &&
        info.दत्त_Format[2]==0x6f &&
        info.दत्त_Format[3]==0x6c &&
        (3<=info.format_संस्करणम्[0] && info.format_संस्करणम्[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.दत्त_Format[0], info.दत्त_Format[1],
                         info.दत्त_Format[2], info.दत्त_Format[3],
                         info.format_संस्करणम्[0], info.format_संस्करणम्[1]);
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_दत्त=(ध्रुव वर्ण *)अन्तर्_दत्त+शीर्षकम्_आकार;
    यदि(length>=0) { length-=शीर्षकम्_आकार; }
    बहिर्गमनम्_दत्त=(बहिर्गमनम्_दत्त == शून्यसूचकः) ? शून्यसूचकः : (वर्ण *)बहिर्गमनम्_दत्त+शीर्षकम्_आकार;
    int32_t collation_आकार;
    यदि(info.format_संस्करणम्[0]>=4) {
        collation_आकार=swapFormatVersion4(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, *p_Error_संहिता);
    } अन्यथा {
        collation_आकार=swapFormatVersion3(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    }
    यदि(U_SUCCESS(*p_Error_संहिता)) {
        प्रत्यागम शीर्षकम्_आकार+collation_आकार;
    } अन्यथा {
        प्रत्यागम 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_swapInverseUCA(ध्रुव UDataSwapper *ds,
                    ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                    UError_संहिता *p_Error_संहिता) {
    ध्रुव UDataInfo *pInfo;
    int32_t शीर्षकम्_आकार;

    ध्रुव uint8_t *अन्तर्_अष्टकानि;
    uint8_t *बहिर्गमनम्_अष्टकानि;

    ध्रुव Inverse_UCATable_शीर्षकम् *अन्तर्_शीर्षकम्;
    Inverse_UCATable_शीर्षकम् *बहिर्गमनम्_शीर्षकम्;
    Inverse_UCATable_शीर्षकम् शीर्षकम्={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    शीर्षकम्_आकार=udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)अन्तर्_दत्त+4);
    यदि(!(
        pInfo->दत्त_Format[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->दत्त_Format[1]==0x6e &&
        pInfo->दत्त_Format[2]==0x76 &&
        pInfo->दत्त_Format[3]==0x43 &&
        pInfo->format_संस्करणम्[0]==2 &&
        pInfo->format_संस्करणम्[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->दत्त_Format[0], pInfo->दत्त_Format[1],
                         pInfo->दत्त_Format[2], pInfo->दत्त_Format[3],
                         pInfo->format_संस्करणम्[0], pInfo->format_संस्करणम्[1]);
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_अष्टकानि=(ध्रुव uint8_t *)अन्तर्_दत्त+शीर्षकम्_आकार;
    बहिर्गमनम्_अष्टकानि=(uint8_t *)बहिर्गमनम्_दत्त+शीर्षकम्_आकार;

    अन्तर्_शीर्षकम्=(ध्रुव Inverse_UCATable_शीर्षकम् *)अन्तर्_अष्टकानि;
    बहिर्गमनम्_शीर्षकम्=(Inverse_UCATable_शीर्षकम् *)बहिर्गमनम्_अष्टकानि;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    यदि(length<0) {
        शीर्षकम्.byte_आकार=udata_readInt32(ds, अन्तर्_शीर्षकम्->byte_आकार);
    } अन्यथा यदि(
        ((length-शीर्षकम्_आकार)<(8*4) ||
         (uint32_t)(length-शीर्षकम्_आकार)<(शीर्षकम्.byte_आकार=udata_readInt32(ds, अन्तर्_शीर्षकम्->byte_आकार)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    यदि(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        यदि(अन्तर्_अष्टकानि!=बहिर्गमनम्_अष्टकानि) {
            uprv_memcpy(बहिर्गमनम्_अष्टकानि, अन्तर्_अष्टकानि, शीर्षकम्.byte_आकार);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        शीर्षकम्.सारणी_आकार=   ds->readUInt32(अन्तर्_शीर्षकम्->सारणी_आकार);
        शीर्षकम्.conts_आकार=   ds->readUInt32(अन्तर्_शीर्षकम्->conts_आकार);
        शीर्षकम्.सारणी=       ds->readUInt32(अन्तर्_शीर्षकम्->सारणी);
        शीर्षकम्.conts=       ds->readUInt32(अन्तर्_शीर्षकम्->conts);

        /* swap the 32-bit integers in the header */
        ds->swapArray32(ds, अन्तर्_शीर्षकम्, 5*4, बहिर्गमनम्_शीर्षकम्, p_Error_संहिता);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->swapArray32(ds, अन्तर्_अष्टकानि+शीर्षकम्.सारणी, शीर्षकम्.सारणी_आकार*3*4,
                           बहिर्गमनम्_अष्टकानि+शीर्षकम्.सारणी, p_Error_संहिता);

        /* swap the continuation table; contsSize counts UChars */
        ds->swapArray16(ds, अन्तर्_अष्टकानि+शीर्षकम्.conts, शीर्षकम्.conts_आकार*U_SIZEOF_UCHAR,
                           बहिर्गमनम्_अष्टकानि+शीर्षकम्.conts, p_Error_संहिता);
    }

    प्रत्यागम शीर्षकम्_आकार+शीर्षकम्.byte_आकार;
}

#यदि_अन्तः /* #if !UCONFIG_NO_COLLATION */
