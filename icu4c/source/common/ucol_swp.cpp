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

#ಅನ್ತರ್ಭಾವ "unicode/udata.h" /* UDataInfo */
#ಅನ್ತರ್ಭಾವ "utrie.h"
#ಅನ್ತರ್ಭಾವ "utrie2.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ucol_data.h"
#ಅನ್ತರ್ಭಾವ "ucol_swp.h"

/* swapping ----------------------------------------------------------------- */

#ಆದರೆ !UCONFIG_NO_COLLATION

U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_ದ್ವಿಮಾನ(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                              ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length) {
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || length<-1) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    // First check for format version 4+ which has a standard data header.
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    (ಶೂನ್ಯ)udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, -1, ಶೂನ್ಯಸೂಚಕ, &error_ಸಂಕೇತ);
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        ಸ್ಥಿರಾಂಕ UDataInfo &info=*(ಸ್ಥಿರಾಂಕ UDataInfo *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+4);
        ಆದರೆ(info.ದತ್ತಾಂಶ_Format[0]==0x55 &&   // dataFormat="UCol"
                info.ದತ್ತಾಂಶ_Format[1]==0x43 &&
                info.ದತ್ತಾಂಶ_Format[2]==0x6f &&
                info.ದತ್ತಾಂಶ_Format[3]==0x6c) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }

    // Else check for format version 3.
    ಸ್ಥಿರಾಂಕ UCATable_ಶೀರ್ಷಿಕೆ *ಒಳ_ಶೀರ್ಷಿಕೆ=(ಸ್ಥಿರಾಂಕ UCATable_ಶೀರ್ಷಿಕೆ *)ಒಳ_ದತ್ತಾಂಶ;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATable_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;
    uprv_memset(&ಶೀರ್ಷಿಕೆ, 0, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ));
    ಆದರೆ(length<0) {
        ಶೀರ್ಷಿಕೆ.size=udata_readInt32(ds, ಒಳ_ಶೀರ್ಷಿಕೆ->size);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((length<(42*4) || length<(ಶೀರ್ಷಿಕೆ.size=udata_readInt32(ds, ಒಳ_ಶೀರ್ಷಿಕೆ->size)))) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಶೀರ್ಷಿಕೆ.magic=ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->magic);
    ಆದರೆ(!(
        ಶೀರ್ಷಿಕೆ.magic==UCOL_ಶೀರ್ಷಿಕೆ_MAGIC &&
        ಒಳ_ಶೀರ್ಷಿಕೆ->format_ಆವೃತ್ತಿ[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆದರೆ(ಒಳ_ಶೀರ್ಷಿಕೆ->isBigEndian!=ds->ಒಳ_Is_Big_Endian || ಒಳ_ಶೀರ್ಷಿಕೆ->char_ಗಣ_Family!=ds->ಒಳ_Charset) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ನಾಮಾಕಾಶ {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
ಅದಲುಬದಲು_Format_Version3(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                   ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ_ಬೈಟುಗಳು;
    uint8_t *ಹೊರಹೋಗು_ಬೈಟುಗಳು;

    ಸ್ಥಿರಾಂಕ UCATable_ಶೀರ್ಷಿಕೆ *ಒಳ_ಶೀರ್ಷಿಕೆ;
    UCATable_ಶೀರ್ಷಿಕೆ *ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ;
    UCATable_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || length<-1 || (length>0 && ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_ಬೈಟುಗಳು = static_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(ಒಳ_ದತ್ತಾಂಶ);
    ಹೊರಹೋಗು_ಬೈಟುಗಳು = static_cast<uint8_t*>(ಹೊರಹೋಗು_ದತ್ತಾಂಶ);

    ಒಳ_ಶೀರ್ಷಿಕೆ = static_cast<ಸ್ಥಿರಾಂಕ UCATable_ಶೀರ್ಷಿಕೆ*>(ಒಳ_ದತ್ತಾಂಶ);
    ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ = static_cast<UCATable_ಶೀರ್ಷಿಕೆ*>(ಹೊರಹೋಗು_ದತ್ತಾಂಶ);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&ಶೀರ್ಷಿಕೆ, 0, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ));
    ಆದರೆ(length<0) {
        ಶೀರ್ಷಿಕೆ.size=udata_readInt32(ds, ಒಳ_ಶೀರ್ಷಿಕೆ->size);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((length<(42*4) || length<(ಶೀರ್ಷಿಕೆ.size=udata_readInt32(ds, ಒಳ_ಶೀರ್ಷಿಕೆ->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಶೀರ್ಷಿಕೆ.magic=ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->magic);
    ಆದರೆ(!(
        ಶೀರ್ಷಿಕೆ.magic==UCOL_ಶೀರ್ಷಿಕೆ_MAGIC &&
        ಒಳ_ಶೀರ್ಷಿಕೆ->format_ಆವೃತ್ತಿ[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         ಶೀರ್ಷಿಕೆ.magic,
                         ಒಳ_ಶೀರ್ಷಿಕೆ->format_ಆವೃತ್ತಿ[0], ಒಳ_ಶೀರ್ಷಿಕೆ->format_ಆವೃತ್ತಿ[1]);
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(ಒಳ_ಶೀರ್ಷಿಕೆ->isBigEndian!=ds->ಒಳ_Is_Big_Endian || ಒಳ_ಶೀರ್ಷಿಕೆ->char_ಗಣ_Family!=ds->ಒಳ_Charset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         ಒಳ_ಶೀರ್ಷಿಕೆ->isBigEndian, ಒಳ_ಶೀರ್ಷಿಕೆ->char_ಗಣ_Family);
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        ಆದರೆ(ಒಳ_ಬೈಟುಗಳು!=ಹೊರಹೋಗು_ಬೈಟುಗಳು) {
            uprv_memcpy(ಹೊರಹೋಗು_ಬೈಟುಗಳು, ಒಳ_ಬೈಟುಗಳು, ಶೀರ್ಷಿಕೆ.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        ಶೀರ್ಷಿಕೆ.options=                 ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->options);
        ಶೀರ್ಷಿಕೆ.UCAConsts=               ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->UCAConsts);
        ಶೀರ್ಷಿಕೆ.contractionUCACombos=    ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->contractionUCACombos);
        ಶೀರ್ಷಿಕೆ.mappingPosition=         ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->mappingPosition);
        ಶೀರ್ಷಿಕೆ.expansion=               ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->expansion);
        ಶೀರ್ಷಿಕೆ.contraction_ಸೂಚ್ಯಂಕ=        ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->contraction_ಸೂಚ್ಯಂಕ);
        ಶೀರ್ಷಿಕೆ.contractionCEs=          ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->contractionCEs);
        ಶೀರ್ಷಿಕೆ.contraction_ಗಾತ್ರ=         ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->contraction_ಗಾತ್ರ);
        ಶೀರ್ಷಿಕೆ.ಕೊನೆ_Expansion_CE=          ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->ಕೊನೆ_Expansion_CE);
        ಶೀರ್ಷಿಕೆ.expansionCESize=         ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->expansionCESize);
        ಶೀರ್ಷಿಕೆ.ಕೊನೆ_Expansion_CECount=     udata_readInt32(ds, ಒಳ_ಶೀರ್ಷಿಕೆ->ಕೊನೆ_Expansion_CECount);
        ಶೀರ್ಷಿಕೆ.contraction_UCACombos_ಗಾತ್ರ=udata_readInt32(ds, ಒಳ_ಶೀರ್ಷಿಕೆ->contraction_UCACombos_ಗಾತ್ರ);
        ಶೀರ್ಷಿಕೆ.script_ಗಮ್ಯ_Lead_Byte=        ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->script_ಗಮ್ಯ_Lead_Byte);
        ಶೀರ್ಷಿಕೆ.lead_Byte_ಗಮ್ಯ_Script=        ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->lead_Byte_ಗಮ್ಯ_Script);
        
        /* swap the 32-bit integers in the header */
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಶೀರ್ಷಿಕೆ, static_cast<int32_t>(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(&ಒಳ_ಶೀರ್ಷಿಕೆ->jamoSpecial) - reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಒಳ_ಶೀರ್ಷಿಕೆ)),
                           ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ, p_Error_ಸಂಕೇತ);
        ds->ಅದಲುಬದಲು_Array32(ds, &(ಒಳ_ಶೀರ್ಷಿಕೆ->script_ಗಮ್ಯ_Lead_Byte), ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ.script_ಗಮ್ಯ_Lead_Byte) + ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ.lead_Byte_ಗಮ್ಯ_Script),
                           &(ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ->script_ಗಮ್ಯ_Lead_Byte), p_Error_ಸಂಕೇತ);
        /* set the output platform properties */
        ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ->isBigEndian=ds->ಹೊರಹೋಗು_Is_Big_Endian;
        ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ->char_ಗಣ_Family=ds->ಹೊರಹೋಗು_Charset;

        /* swap the options */
        ಆದರೆ(ಶೀರ್ಷಿಕೆ.options!=0) {
            ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.options, ಶೀರ್ಷಿಕೆ.expansion-ಶೀರ್ಷಿಕೆ.options,
                               ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.options, p_Error_ಸಂಕೇತ);
        }

        /* swap the expansions */
        ಆದರೆ(ಶೀರ್ಷಿಕೆ.mappingPosition!=0 && ಶೀರ್ಷಿಕೆ.expansion!=0) {
            ಆದರೆ(ಶೀರ್ಷಿಕೆ.contraction_ಸೂಚ್ಯಂಕ!=0) {
                /* expansions bounded by contractions */
                count=ಶೀರ್ಷಿಕೆ.contraction_ಸೂಚ್ಯಂಕ-ಶೀರ್ಷಿಕೆ.expansion;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* no contractions: expansions bounded by the main trie */
                count=ಶೀರ್ಷಿಕೆ.mappingPosition-ಶೀರ್ಷಿಕೆ.expansion;
            }
            ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು + ಶೀರ್ಷಿಕೆ.expansion, static_cast<int32_t>(count),
                               ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.expansion, p_Error_ಸಂಕೇತ);
        }

        /* swap the contractions */
        ಆದರೆ(ಶೀರ್ಷಿಕೆ.contraction_ಗಾತ್ರ!=0) {
            /* contractionIndex: char16_t[] */
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.contraction_ಸೂಚ್ಯಂಕ, ಶೀರ್ಷಿಕೆ.contraction_ಗಾತ್ರ*2,
                               ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.contraction_ಸೂಚ್ಯಂಕ, p_Error_ಸಂಕೇತ);

            /* contractionCEs: CEs[] */
            ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.contractionCEs, ಶೀರ್ಷಿಕೆ.contraction_ಗಾತ್ರ*4,
                               ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.contractionCEs, p_Error_ಸಂಕೇತ);
        }

        /* swap the main trie */
        ಆದರೆ(ಶೀರ್ಷಿಕೆ.mappingPosition!=0) {
            count=ಶೀರ್ಷಿಕೆ.ಕೊನೆ_Expansion_CE-ಶೀರ್ಷಿಕೆ.mappingPosition;
            utrie_ಅದಲುಬದಲು(ds, ಒಳ_ಬೈಟುಗಳು + ಶೀರ್ಷಿಕೆ.mappingPosition, static_cast<int32_t>(count),
                          ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.mappingPosition, p_Error_ಸಂಕೇತ);
        }

        /* swap the max expansion table */
        ಆದರೆ(ಶೀರ್ಷಿಕೆ.ಕೊನೆ_Expansion_CECount!=0) {
            ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.ಕೊನೆ_Expansion_CE, ಶೀರ್ಷಿಕೆ.ಕೊನೆ_Expansion_CECount*4,
                               ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.ಕೊನೆ_Expansion_CE, p_Error_ಸಂಕೇತ);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        ಆದರೆ(ಶೀರ್ಷಿಕೆ.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.UCAConsts, ಶೀರ್ಷಿಕೆ.contractionUCACombos-ಶೀರ್ಷಿಕೆ.UCAConsts,
                               ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.UCAConsts, p_Error_ಸಂಕೇತ);
        }

        /* swap UCA contractions */
        ಆದರೆ(ಶೀರ್ಷಿಕೆ.contraction_UCACombos_ಗಾತ್ರ!=0) {
            count=ಶೀರ್ಷಿಕೆ.contraction_UCACombos_ಗಾತ್ರ*ಒಳ_ಶೀರ್ಷಿಕೆ->contraction_UCACombos_ಅಗಲ*U_SIZEOF_UCHAR;
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು + ಶೀರ್ಷಿಕೆ.contractionUCACombos, static_cast<int32_t>(count),
                               ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.contractionUCACombos, p_Error_ಸಂಕೇತ);
        }
        
        /* swap the script to lead bytes */
        ಆದರೆ(ಶೀರ್ಷಿಕೆ.script_ಗಮ್ಯ_Lead_Byte!=0) {
            ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_ಎಣಿಕೆ = ds->readUInt16(*((uint16_t*)(ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.script_ಗಮ್ಯ_Lead_Byte))); // each entry = 2 * uint16
            ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_ಎಣಿಕೆ = ds->readUInt16(*((uint16_t*)(ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.script_ಗಮ್ಯ_Lead_Byte + 2))); // each entry = uint16
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.script_ಗಮ್ಯ_Lead_Byte, 
                                4 + (4 * ಸೂಚ್ಯಂಕ_ಎಣಿಕೆ) + (2 * ದತ್ತಾಂಶ_ಎಣಿಕೆ),
                                ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.script_ಗಮ್ಯ_Lead_Byte, p_Error_ಸಂಕೇತ);
        }
        
        /* swap the lead byte to scripts */
        ಆದರೆ(ಶೀರ್ಷಿಕೆ.lead_Byte_ಗಮ್ಯ_Script!=0) {
            ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_ಎಣಿಕೆ = ds->readUInt16(*((uint16_t*)(ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.lead_Byte_ಗಮ್ಯ_Script))); // each entry = uint16
            ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_ಎಣಿಕೆ = ds->readUInt16(*((uint16_t*)(ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.lead_Byte_ಗಮ್ಯ_Script + 2))); // each entry = uint16
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.lead_Byte_ಗಮ್ಯ_Script, 
                                4 + (2 * ಸೂಚ್ಯಂಕ_ಎಣಿಕೆ) + (2 * ದತ್ತಾಂಶ_ಎಣಿಕೆ),
                                ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.lead_Byte_ಗಮ್ಯ_Script, p_Error_ಸಂಕೇತ);
        }
    }

    ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

ಎಣಿಕೆ {
    IX_INDEXES_ಉದ್ದಳತೆ,  // 0
    IX_OPTIONS,
    IX_RESERVED2,
    IX_RESERVED3,

    IX_JAMO_CE32_S_ಆರಂಭ,  // 4
    IX_REORDER_CODES_ಸ್ಥಾನಾಂತರ,
    IX_REORDER_ಕೋಷ್ಟಕ_ಸ್ಥಾನಾಂತರ,
    IX_TRIE_ಸ್ಥಾನಾಂತರ,

    IX_RESERVED8_ಸ್ಥಾನಾಂತರ,  // 8
    IX_CES_ಸ್ಥಾನಾಂತರ,
    IX_RESERVED10_ಸ್ಥಾನಾಂತರ,
    IX_CE32_S_ಸ್ಥಾನಾಂತರ,

    IX_ಬೇರು_ELEMENTS_ಸ್ಥಾನಾಂತರ,  // 12
    IX_CONTEXTS_ಸ್ಥಾನಾಂತರ,
    IX_UNSAFE_BWD_ಸ್ಥಾನಾಂತರ,
    IX_FAST_LATIN_ಕೋಷ್ಟಕ_ಸ್ಥಾನಾಂತರ,

    IX_SCRIPTS_ಸ್ಥಾನಾಂತರ,  // 16
    IX_COMPRESSIBLE_ಬೈಟುಗಳು_ಸ್ಥಾನಾಂತರ,
    IX_RESERVED18_ಸ್ಥಾನಾಂತರ,
    IX_ಒಟ್ಟು_ಗಾತ್ರ
};

int32_t
ಅದಲುಬದಲು_Format_Version4(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                   ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                   UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0; }

    ಸ್ಥಿರಾಂಕ uint8_t* ಒಳ_ಬೈಟುಗಳು = static_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(ಒಳ_ದತ್ತಾಂಶ);
    uint8_t* ಹೊರಹೋಗು_ಬೈಟುಗಳು = static_cast<uint8_t*>(ಹೊರಹೋಗು_ದತ್ತಾಂಶ);

    ಸ್ಥಿರಾಂಕ int32_t* ಒಳ_Indexes = reinterpret_cast<ಸ್ಥಿರಾಂಕ int32_t*>(ಒಳ_ಬೈಟುಗಳು);
    int32_t indexes[IX_ಒಟ್ಟು_ಗಾತ್ರ+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    ಆದರೆ(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    int32_t indexes_ಉದ್ದಳತೆ=indexes[0]=udata_readInt32(ds, ಒಳ_Indexes[0]);
    ಆದರೆ(0<=length && length<(indexes_ಉದ್ದಳತೆ*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಗಾಗಿ(int32_t i=1; i<=IX_ಒಟ್ಟು_ಗಾತ್ರ && i<indexes_ಉದ್ದಳತೆ; ++i) {
        indexes[i]=udata_readInt32(ds, ಒಳ_Indexes[i]);
    }
    ಗಾಗಿ(int32_t i=indexes_ಉದ್ದಳತೆ; i<=IX_ಒಟ್ಟು_ಗಾತ್ರ; ++i) {
        indexes[i]=-1;
    }
    ಒಳ_Indexes=ಶೂನ್ಯಸೂಚಕ;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    ಆದರೆ(indexes_ಉದ್ದಳತೆ>IX_ಒಟ್ಟು_ಗಾತ್ರ) {
        size=indexes[IX_ಒಟ್ಟು_ಗಾತ್ರ];
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(indexes_ಉದ್ದಳತೆ>IX_REORDER_CODES_ಸ್ಥಾನಾಂತರ) {
        size=indexes[indexes_ಉದ್ದಳತೆ-1];
    } ಇಲ್ಲದಿದ್ದರೆ {
        size=indexes_ಉದ್ದಳತೆ*4;
    }
    ಆದರೆ(length<0) { ಹಿಂತಿರುಗಿ size; }

    ಆದರೆ(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    ಆದರೆ(ಒಳ_ಬೈಟುಗಳು!=ಹೊರಹೋಗು_ಬೈಟುಗಳು) {
        uprv_memcpy(ಹೊರಹೋಗು_ಬೈಟುಗಳು, ಒಳ_ಬೈಟುಗಳು, size);
    }

    // Swap the int32_t indexes[].
    ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು, indexes_ಉದ್ದಳತೆ * 4, ಹೊರಹೋಗು_ಬೈಟುಗಳು, &error_ಸಂಕೇತ);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t ಸೂಚ್ಯಂಕ;  // one of the indexes[] slots
    int32_t ಸ್ಥಾನಾಂತರ;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    ಸೂಚ್ಯಂಕ = IX_REORDER_CODES_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, length, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, &error_ಸಂಕೇತ);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    ಸೂಚ್ಯಂಕ = IX_TRIE_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        utrie2_ಅದಲುಬದಲು(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, length, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, &error_ಸಂಕೇತ);
    }

    ಸೂಚ್ಯಂಕ = IX_RESERVED8_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        error_ಸಂಕೇತ = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಸೂಚ್ಯಂಕ = IX_CES_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        ds->ಅದಲುಬದಲು_Array64(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, length, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, &error_ಸಂಕೇತ);
    }

    ಸೂಚ್ಯಂಕ = IX_RESERVED10_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        error_ಸಂಕೇತ = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಸೂಚ್ಯಂಕ = IX_CE32_S_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, length, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, &error_ಸಂಕೇತ);
    }

    ಸೂಚ್ಯಂಕ = IX_ಬೇರು_ELEMENTS_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, length, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, &error_ಸಂಕೇತ);
    }

    ಸೂಚ್ಯಂಕ = IX_CONTEXTS_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, length, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, &error_ಸಂಕೇತ);
    }

    ಸೂಚ್ಯಂಕ = IX_UNSAFE_BWD_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, length, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, &error_ಸಂಕೇತ);
    }

    ಸೂಚ್ಯಂಕ = IX_FAST_LATIN_ಕೋಷ್ಟಕ_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, length, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, &error_ಸಂಕೇತ);
    }

    ಸೂಚ್ಯಂಕ = IX_SCRIPTS_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, length, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, &error_ಸಂಕೇತ);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    ಸೂಚ್ಯಂಕ = IX_RESERVED18_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಾನಾಂತರ = indexes[ಸೂಚ್ಯಂಕ];
    length = indexes[ಸೂಚ್ಯಂಕ + 1] - ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        error_ಸಂಕೇತ = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಹಿಂತಿರುಗಿ size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        // Try to swap the old format version which did not have a standard data header.
        *p_Error_ಸಂಕೇತ=U_ZERO_ERROR;
        ಹಿಂತಿರುಗಿ ಅದಲುಬದಲು_Format_Version3(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    }

    /* check data format and format version */
    ಸ್ಥಿರಾಂಕ UDataInfo &info=*(ಸ್ಥಿರಾಂಕ UDataInfo *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+4);
    ಆದರೆ(!(
        info.ದತ್ತಾಂಶ_Format[0]==0x55 &&   // dataFormat="UCol"
        info.ದತ್ತಾಂಶ_Format[1]==0x43 &&
        info.ದತ್ತಾಂಶ_Format[2]==0x6f &&
        info.ದತ್ತಾಂಶ_Format[3]==0x6c &&
        (3<=info.format_ಆವೃತ್ತಿ[0] && info.format_ಆವೃತ್ತಿ[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.ದತ್ತಾಂಶ_Format[0], info.ದತ್ತಾಂಶ_Format[1],
                         info.ದತ್ತಾಂಶ_Format[2], info.ದತ್ತಾಂಶ_Format[3],
                         info.format_ಆವೃತ್ತಿ[0], info.format_ಆವೃತ್ತಿ[1]);
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_ದತ್ತಾಂಶ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ಆದರೆ(length>=0) { length-=ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ; }
    ಹೊರಹೋಗು_ದತ್ತಾಂಶ=(ಹೊರಹೋಗು_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) ? ಶೂನ್ಯಸೂಚಕ : (ಅಕ್ಷರ *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    int32_t collation_ಗಾತ್ರ;
    ಆದರೆ(info.format_ಆವೃತ್ತಿ[0]>=4) {
        collation_ಗಾತ್ರ=ಅದಲುಬದಲು_Format_Version4(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, *p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        collation_ಗಾತ್ರ=ಅದಲುಬದಲು_Format_Version3(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    }
    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ+collation_ಗಾತ್ರ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_ಅದಲುಬದಲು_Inverse_UCA(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UDataInfo *pInfo;
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

    ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ_ಬೈಟುಗಳು;
    uint8_t *ಹೊರಹೋಗು_ಬೈಟುಗಳು;

    ಸ್ಥಿರಾಂಕ Inverse_UCATable_ಶೀರ್ಷಿಕೆ *ಒಳ_ಶೀರ್ಷಿಕೆ;
    Inverse_UCATable_ಶೀರ್ಷಿಕೆ *ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ;
    Inverse_UCATable_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* check data format and format version */
    pInfo=(ಸ್ಥಿರಾಂಕ UDataInfo *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+4);
    ಆದರೆ(!(
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x6e &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x76 &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x43 &&
        pInfo->format_ಆವೃತ್ತಿ[0]==2 &&
        pInfo->format_ಆವೃತ್ತಿ[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->ದತ್ತಾಂಶ_Format[0], pInfo->ದತ್ತಾಂಶ_Format[1],
                         pInfo->ದತ್ತಾಂಶ_Format[2], pInfo->ದತ್ತಾಂಶ_Format[3],
                         pInfo->format_ಆವೃತ್ತಿ[0], pInfo->format_ಆವೃತ್ತಿ[1]);
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_ಬೈಟುಗಳು=(ಸ್ಥಿರಾಂಕ uint8_t *)ಒಳ_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ಹೊರಹೋಗು_ಬೈಟುಗಳು=(uint8_t *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

    ಒಳ_ಶೀರ್ಷಿಕೆ=(ಸ್ಥಿರಾಂಕ Inverse_UCATable_ಶೀರ್ಷಿಕೆ *)ಒಳ_ಬೈಟುಗಳು;
    ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ=(Inverse_UCATable_ಶೀರ್ಷಿಕೆ *)ಹೊರಹೋಗು_ಬೈಟುಗಳು;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    ಆದರೆ(length<0) {
        ಶೀರ್ಷಿಕೆ.byte_ಗಾತ್ರ=udata_readInt32(ds, ಒಳ_ಶೀರ್ಷಿಕೆ->byte_ಗಾತ್ರ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(
        ((length-ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ)<(8*4) ||
         (uint32_t)(length-ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ)<(ಶೀರ್ಷಿಕೆ.byte_ಗಾತ್ರ=udata_readInt32(ds, ಒಳ_ಶೀರ್ಷಿಕೆ->byte_ಗಾತ್ರ)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        ಆದರೆ(ಒಳ_ಬೈಟುಗಳು!=ಹೊರಹೋಗು_ಬೈಟುಗಳು) {
            uprv_memcpy(ಹೊರಹೋಗು_ಬೈಟುಗಳು, ಒಳ_ಬೈಟುಗಳು, ಶೀರ್ಷಿಕೆ.byte_ಗಾತ್ರ);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        ಶೀರ್ಷಿಕೆ.ಕೋಷ್ಟಕ_ಗಾತ್ರ=   ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->ಕೋಷ್ಟಕ_ಗಾತ್ರ);
        ಶೀರ್ಷಿಕೆ.conts_ಗಾತ್ರ=   ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->conts_ಗಾತ್ರ);
        ಶೀರ್ಷಿಕೆ.ಕೋಷ್ಟಕ=       ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->ಕೋಷ್ಟಕ);
        ಶೀರ್ಷಿಕೆ.conts=       ds->readUInt32(ಒಳ_ಶೀರ್ಷಿಕೆ->conts);

        /* swap the 32-bit integers in the header */
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಶೀರ್ಷಿಕೆ, 5*4, ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ, p_Error_ಸಂಕೇತ);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.ಕೋಷ್ಟಕ, ಶೀರ್ಷಿಕೆ.ಕೋಷ್ಟಕ_ಗಾತ್ರ*3*4,
                           ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.ಕೋಷ್ಟಕ, p_Error_ಸಂಕೇತ);

        /* swap the continuation table; contsSize counts UChars */
        ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.conts, ಶೀರ್ಷಿಕೆ.conts_ಗಾತ್ರ*U_SIZEOF_UCHAR,
                           ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಶೀರ್ಷಿಕೆ.conts, p_Error_ಸಂಕೇತ);
    }

    ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ+ಶೀರ್ಷಿಕೆ.byte_ಗಾತ್ರ;
}

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_COLLATION */
