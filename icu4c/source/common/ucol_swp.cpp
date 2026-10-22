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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h" /* UDataInfo */
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucol_data.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucol_swp.h"

/* swapping ----------------------------------------------------------------- */

#ਜੇ !UCONFIG_NO_COLLATION

U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_ਦੁਆਧਾਰੀ(ਅਚਲ UDataSwapper *ds,
                              ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length) {
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || length<-1) {
        ਵਾਪਸ ਝੂਠ;
    }

    // First check for format version 4+ which has a standard data header.
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    (ਖਾਲੀ)udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, -1, ਖਾਲੀ_ਸੂਚਕ, &error_ਕੋਡ);
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        ਅਚਲ UDataInfo &info=*(ਅਚਲ UDataInfo *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+4);
        ਜੇ(info.ਅੰਕੜੇ_Format[0]==0x55 &&   // dataFormat="UCol"
                info.ਅੰਕੜੇ_Format[1]==0x43 &&
                info.ਅੰਕੜੇ_Format[2]==0x6f &&
                info.ਅੰਕੜੇ_Format[3]==0x6c) {
            ਵਾਪਸ ਸੱਚ;
        }
    }

    // Else check for format version 3.
    ਅਚਲ UCATable_ਸਿਰਲੇਖ *ਵਿੱਚ_ਸਿਰਲੇਖ=(ਅਚਲ UCATable_ਸਿਰਲੇਖ *)ਵਿੱਚ_ਅੰਕੜੇ;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATable_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;
    uprv_memset(&ਸਿਰਲੇਖ, 0, ਆਕਾਰ(ਸਿਰਲੇਖ));
    ਜੇ(length<0) {
        ਸਿਰਲੇਖ.size=udata_readInt32(ds, ਵਿੱਚ_ਸਿਰਲੇਖ->size);
    } ਨਹੀਂਤਾਂ ਜੇ((length<(42*4) || length<(ਸਿਰਲੇਖ.size=udata_readInt32(ds, ਵਿੱਚ_ਸਿਰਲੇਖ->size)))) {
        ਵਾਪਸ ਝੂਠ;
    }

    ਸਿਰਲੇਖ.magic=ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->magic);
    ਜੇ(!(
        ਸਿਰਲੇਖ.magic==UCOL_ਸਿਰਲੇਖ_MAGIC &&
        ਵਿੱਚ_ਸਿਰਲੇਖ->format_ਸੰਸਕਰਣ[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        ਵਾਪਸ ਝੂਠ;
    }

    ਜੇ(ਵਿੱਚ_ਸਿਰਲੇਖ->isBigEndian!=ds->ਵਿੱਚ_Is_Big_Endian || ਵਿੱਚ_ਸਿਰਲੇਖ->char_ਸੈੱਟ_Family!=ds->ਵਿੱਚ_Charset) {
        ਵਾਪਸ ਝੂਠ;
    }

    ਵਾਪਸ ਸੱਚ;
}

ਨਾਮਾਕਾਸ਼ {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
ਅਦਲਾ_ਬਦਲੀ_Format_Version3(ਅਚਲ UDataSwapper *ds,
                   ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ uint8_t *ਵਿੱਚ_ਬਾਈਟਾਂ;
    uint8_t *ਬਾਹਰਲਾ_ਬਾਈਟਾਂ;

    ਅਚਲ UCATable_ਸਿਰਲੇਖ *ਵਿੱਚ_ਸਿਰਲੇਖ;
    UCATable_ਸਿਰਲੇਖ *ਬਾਹਰਲਾ_ਸਿਰਲੇਖ;
    UCATable_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || length<-1 || (length>0 && ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_ਬਾਈਟਾਂ = static_cast<ਅਚਲ uint8_t*>(ਵਿੱਚ_ਅੰਕੜੇ);
    ਬਾਹਰਲਾ_ਬਾਈਟਾਂ = static_cast<uint8_t*>(ਬਾਹਰਲਾ_ਅੰਕੜੇ);

    ਵਿੱਚ_ਸਿਰਲੇਖ = static_cast<ਅਚਲ UCATable_ਸਿਰਲੇਖ*>(ਵਿੱਚ_ਅੰਕੜੇ);
    ਬਾਹਰਲਾ_ਸਿਰਲੇਖ = static_cast<UCATable_ਸਿਰਲੇਖ*>(ਬਾਹਰਲਾ_ਅੰਕੜੇ);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&ਸਿਰਲੇਖ, 0, ਆਕਾਰ(ਸਿਰਲੇਖ));
    ਜੇ(length<0) {
        ਸਿਰਲੇਖ.size=udata_readInt32(ds, ਵਿੱਚ_ਸਿਰਲੇਖ->size);
    } ਨਹੀਂਤਾਂ ਜੇ((length<(42*4) || length<(ਸਿਰਲੇਖ.size=udata_readInt32(ds, ਵਿੱਚ_ਸਿਰਲੇਖ->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    ਸਿਰਲੇਖ.magic=ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->magic);
    ਜੇ(!(
        ਸਿਰਲੇਖ.magic==UCOL_ਸਿਰਲੇਖ_MAGIC &&
        ਵਿੱਚ_ਸਿਰਲੇਖ->format_ਸੰਸਕਰਣ[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         ਸਿਰਲੇਖ.magic,
                         ਵਿੱਚ_ਸਿਰਲੇਖ->format_ਸੰਸਕਰਣ[0], ਵਿੱਚ_ਸਿਰਲੇਖ->format_ਸੰਸਕਰਣ[1]);
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(ਵਿੱਚ_ਸਿਰਲੇਖ->isBigEndian!=ds->ਵਿੱਚ_Is_Big_Endian || ਵਿੱਚ_ਸਿਰਲੇਖ->char_ਸੈੱਟ_Family!=ds->ਵਿੱਚ_Charset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         ਵਿੱਚ_ਸਿਰਲੇਖ->isBigEndian, ਵਿੱਚ_ਸਿਰਲੇਖ->char_ਸੈੱਟ_Family);
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        ਜੇ(ਵਿੱਚ_ਬਾਈਟਾਂ!=ਬਾਹਰਲਾ_ਬਾਈਟਾਂ) {
            uprv_memcpy(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, ਵਿੱਚ_ਬਾਈਟਾਂ, ਸਿਰਲੇਖ.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        ਸਿਰਲੇਖ.options=                 ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->options);
        ਸਿਰਲੇਖ.UCAConsts=               ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->UCAConsts);
        ਸਿਰਲੇਖ.contractionUCACombos=    ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->contractionUCACombos);
        ਸਿਰਲੇਖ.mappingPosition=         ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->mappingPosition);
        ਸਿਰਲੇਖ.expansion=               ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->expansion);
        ਸਿਰਲੇਖ.contraction_ਸੂਚਕਾਂਕ=        ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->contraction_ਸੂਚਕਾਂਕ);
        ਸਿਰਲੇਖ.contractionCEs=          ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->contractionCEs);
        ਸਿਰਲੇਖ.contraction_ਮਾਪ=         ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->contraction_ਮਾਪ);
        ਸਿਰਲੇਖ.ਸਿਰਾ_Expansion_CE=          ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->ਸਿਰਾ_Expansion_CE);
        ਸਿਰਲੇਖ.expansionCESize=         ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->expansionCESize);
        ਸਿਰਲੇਖ.ਸਿਰਾ_Expansion_CECount=     udata_readInt32(ds, ਵਿੱਚ_ਸਿਰਲੇਖ->ਸਿਰਾ_Expansion_CECount);
        ਸਿਰਲੇਖ.contraction_UCACombos_ਮਾਪ=udata_readInt32(ds, ਵਿੱਚ_ਸਿਰਲੇਖ->contraction_UCACombos_ਮਾਪ);
        ਸਿਰਲੇਖ.script_ਮੰਜ਼ਿਲ_Lead_Byte=        ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->script_ਮੰਜ਼ਿਲ_Lead_Byte);
        ਸਿਰਲੇਖ.lead_Byte_ਮੰਜ਼ਿਲ_Script=        ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->lead_Byte_ਮੰਜ਼ਿਲ_Script);
        
        /* swap the 32-bit integers in the header */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਸਿਰਲੇਖ, static_cast<int32_t>(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(&ਵਿੱਚ_ਸਿਰਲੇਖ->jamoSpecial) - reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਵਿੱਚ_ਸਿਰਲੇਖ)),
                           ਬਾਹਰਲਾ_ਸਿਰਲੇਖ, p_Error_ਕੋਡ);
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, &(ਵਿੱਚ_ਸਿਰਲੇਖ->script_ਮੰਜ਼ਿਲ_Lead_Byte), ਆਕਾਰ(ਸਿਰਲੇਖ.script_ਮੰਜ਼ਿਲ_Lead_Byte) + ਆਕਾਰ(ਸਿਰਲੇਖ.lead_Byte_ਮੰਜ਼ਿਲ_Script),
                           &(ਬਾਹਰਲਾ_ਸਿਰਲੇਖ->script_ਮੰਜ਼ਿਲ_Lead_Byte), p_Error_ਕੋਡ);
        /* set the output platform properties */
        ਬਾਹਰਲਾ_ਸਿਰਲੇਖ->isBigEndian=ds->ਬਾਹਰਲਾ_Is_Big_Endian;
        ਬਾਹਰਲਾ_ਸਿਰਲੇਖ->char_ਸੈੱਟ_Family=ds->ਬਾਹਰਲਾ_Charset;

        /* swap the options */
        ਜੇ(ਸਿਰਲੇਖ.options!=0) {
            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.options, ਸਿਰਲੇਖ.expansion-ਸਿਰਲੇਖ.options,
                               ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.options, p_Error_ਕੋਡ);
        }

        /* swap the expansions */
        ਜੇ(ਸਿਰਲੇਖ.mappingPosition!=0 && ਸਿਰਲੇਖ.expansion!=0) {
            ਜੇ(ਸਿਰਲੇਖ.contraction_ਸੂਚਕਾਂਕ!=0) {
                /* expansions bounded by contractions */
                count=ਸਿਰਲੇਖ.contraction_ਸੂਚਕਾਂਕ-ਸਿਰਲੇਖ.expansion;
            } ਨਹੀਂਤਾਂ {
                /* no contractions: expansions bounded by the main trie */
                count=ਸਿਰਲੇਖ.mappingPosition-ਸਿਰਲੇਖ.expansion;
            }
            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਸਿਰਲੇਖ.expansion, static_cast<int32_t>(count),
                               ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.expansion, p_Error_ਕੋਡ);
        }

        /* swap the contractions */
        ਜੇ(ਸਿਰਲੇਖ.contraction_ਮਾਪ!=0) {
            /* contractionIndex: char16_t[] */
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.contraction_ਸੂਚਕਾਂਕ, ਸਿਰਲੇਖ.contraction_ਮਾਪ*2,
                               ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.contraction_ਸੂਚਕਾਂਕ, p_Error_ਕੋਡ);

            /* contractionCEs: CEs[] */
            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.contractionCEs, ਸਿਰਲੇਖ.contraction_ਮਾਪ*4,
                               ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.contractionCEs, p_Error_ਕੋਡ);
        }

        /* swap the main trie */
        ਜੇ(ਸਿਰਲੇਖ.mappingPosition!=0) {
            count=ਸਿਰਲੇਖ.ਸਿਰਾ_Expansion_CE-ਸਿਰਲੇਖ.mappingPosition;
            utrie_ਅਦਲਾ_ਬਦਲੀ(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਸਿਰਲੇਖ.mappingPosition, static_cast<int32_t>(count),
                          ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.mappingPosition, p_Error_ਕੋਡ);
        }

        /* swap the max expansion table */
        ਜੇ(ਸਿਰਲੇਖ.ਸਿਰਾ_Expansion_CECount!=0) {
            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.ਸਿਰਾ_Expansion_CE, ਸਿਰਲੇਖ.ਸਿਰਾ_Expansion_CECount*4,
                               ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.ਸਿਰਾ_Expansion_CE, p_Error_ਕੋਡ);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        ਜੇ(ਸਿਰਲੇਖ.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.UCAConsts, ਸਿਰਲੇਖ.contractionUCACombos-ਸਿਰਲੇਖ.UCAConsts,
                               ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.UCAConsts, p_Error_ਕੋਡ);
        }

        /* swap UCA contractions */
        ਜੇ(ਸਿਰਲੇਖ.contraction_UCACombos_ਮਾਪ!=0) {
            count=ਸਿਰਲੇਖ.contraction_UCACombos_ਮਾਪ*ਵਿੱਚ_ਸਿਰਲੇਖ->contraction_UCACombos_ਚੌੜਾਈ*U_SIZEOF_UCHAR;
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਸਿਰਲੇਖ.contractionUCACombos, static_cast<int32_t>(count),
                               ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.contractionUCACombos, p_Error_ਕੋਡ);
        }
        
        /* swap the script to lead bytes */
        ਜੇ(ਸਿਰਲੇਖ.script_ਮੰਜ਼ਿਲ_Lead_Byte!=0) {
            ਪੂਰਨ ਸੂਚਕਾਂਕ_ਗਿਣਤੀ = ds->readUInt16(*((uint16_t*)(ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.script_ਮੰਜ਼ਿਲ_Lead_Byte))); // each entry = 2 * uint16
            ਪੂਰਨ ਅੰਕੜੇ_ਗਿਣਤੀ = ds->readUInt16(*((uint16_t*)(ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.script_ਮੰਜ਼ਿਲ_Lead_Byte + 2))); // each entry = uint16
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.script_ਮੰਜ਼ਿਲ_Lead_Byte, 
                                4 + (4 * ਸੂਚਕਾਂਕ_ਗਿਣਤੀ) + (2 * ਅੰਕੜੇ_ਗਿਣਤੀ),
                                ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.script_ਮੰਜ਼ਿਲ_Lead_Byte, p_Error_ਕੋਡ);
        }
        
        /* swap the lead byte to scripts */
        ਜੇ(ਸਿਰਲੇਖ.lead_Byte_ਮੰਜ਼ਿਲ_Script!=0) {
            ਪੂਰਨ ਸੂਚਕਾਂਕ_ਗਿਣਤੀ = ds->readUInt16(*((uint16_t*)(ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.lead_Byte_ਮੰਜ਼ਿਲ_Script))); // each entry = uint16
            ਪੂਰਨ ਅੰਕੜੇ_ਗਿਣਤੀ = ds->readUInt16(*((uint16_t*)(ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.lead_Byte_ਮੰਜ਼ਿਲ_Script + 2))); // each entry = uint16
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.lead_Byte_ਮੰਜ਼ਿਲ_Script, 
                                4 + (2 * ਸੂਚਕਾਂਕ_ਗਿਣਤੀ) + (2 * ਅੰਕੜੇ_ਗਿਣਤੀ),
                                ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.lead_Byte_ਮੰਜ਼ਿਲ_Script, p_Error_ਕੋਡ);
        }
    }

    ਵਾਪਸ ਸਿਰਲੇਖ.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

ਗਿਣਤੀ {
    IX_INDEXES_ਲੰਮਾਈ,  // 0
    IX_OPTIONS,
    IX_RESERVED2,
    IX_RESERVED3,

    IX_JAMO_CE32_S_ਸ਼ੁਰੂ,  // 4
    IX_REORDER_CODES_ਵਿੱਥ,
    IX_REORDER_ਸਾਰਣੀ_ਵਿੱਥ,
    IX_TRIE_ਵਿੱਥ,

    IX_RESERVED8_ਵਿੱਥ,  // 8
    IX_CES_ਵਿੱਥ,
    IX_RESERVED10_ਵਿੱਥ,
    IX_CE32_S_ਵਿੱਥ,

    IX_ਜੜ੍ਹ_ELEMENTS_ਵਿੱਥ,  // 12
    IX_CONTEXTS_ਵਿੱਥ,
    IX_UNSAFE_BWD_ਵਿੱਥ,
    IX_FAST_LATIN_ਸਾਰਣੀ_ਵਿੱਥ,

    IX_SCRIPTS_ਵਿੱਥ,  // 16
    IX_COMPRESSIBLE_ਬਾਈਟਾਂ_ਵਿੱਥ,
    IX_RESERVED18_ਵਿੱਥ,
    IX_ਕੁੱਲ_ਮਾਪ
};

int32_t
ਅਦਲਾ_ਬਦਲੀ_Format_Version4(ਅਚਲ UDataSwapper *ds,
                   ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                   UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ 0; }

    ਅਚਲ uint8_t* ਵਿੱਚ_ਬਾਈਟਾਂ = static_cast<ਅਚਲ uint8_t*>(ਵਿੱਚ_ਅੰਕੜੇ);
    uint8_t* ਬਾਹਰਲਾ_ਬਾਈਟਾਂ = static_cast<uint8_t*>(ਬਾਹਰਲਾ_ਅੰਕੜੇ);

    ਅਚਲ int32_t* ਵਿੱਚ_Indexes = reinterpret_cast<ਅਚਲ int32_t*>(ਵਿੱਚ_ਬਾਈਟਾਂ);
    int32_t indexes[IX_ਕੁੱਲ_ਮਾਪ+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    ਜੇ(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    int32_t indexes_ਲੰਮਾਈ=indexes[0]=udata_readInt32(ds, ਵਿੱਚ_Indexes[0]);
    ਜੇ(0<=length && length<(indexes_ਲੰਮਾਈ*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    ਲਈ(int32_t i=1; i<=IX_ਕੁੱਲ_ਮਾਪ && i<indexes_ਲੰਮਾਈ; ++i) {
        indexes[i]=udata_readInt32(ds, ਵਿੱਚ_Indexes[i]);
    }
    ਲਈ(int32_t i=indexes_ਲੰਮਾਈ; i<=IX_ਕੁੱਲ_ਮਾਪ; ++i) {
        indexes[i]=-1;
    }
    ਵਿੱਚ_Indexes=ਖਾਲੀ_ਸੂਚਕ;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    ਜੇ(indexes_ਲੰਮਾਈ>IX_ਕੁੱਲ_ਮਾਪ) {
        size=indexes[IX_ਕੁੱਲ_ਮਾਪ];
    } ਨਹੀਂਤਾਂ ਜੇ(indexes_ਲੰਮਾਈ>IX_REORDER_CODES_ਵਿੱਥ) {
        size=indexes[indexes_ਲੰਮਾਈ-1];
    } ਨਹੀਂਤਾਂ {
        size=indexes_ਲੰਮਾਈ*4;
    }
    ਜੇ(length<0) { ਵਾਪਸ size; }

    ਜੇ(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    ਜੇ(ਵਿੱਚ_ਬਾਈਟਾਂ!=ਬਾਹਰਲਾ_ਬਾਈਟਾਂ) {
        uprv_memcpy(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, ਵਿੱਚ_ਬਾਈਟਾਂ, size);
    }

    // Swap the int32_t indexes[].
    ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ, indexes_ਲੰਮਾਈ * 4, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, &error_ਕੋਡ);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t ਸੂਚਕਾਂਕ;  // one of the indexes[] slots
    int32_t ਵਿੱਥ;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    ਸੂਚਕਾਂਕ = IX_REORDER_CODES_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, length, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, &error_ਕੋਡ);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    ਸੂਚਕਾਂਕ = IX_TRIE_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        utrie2_ਅਦਲਾ_ਬਦਲੀ(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, length, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, &error_ਕੋਡ);
    }

    ਸੂਚਕਾਂਕ = IX_RESERVED8_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        error_ਕੋਡ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਸੂਚਕਾਂਕ = IX_CES_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        ds->ਅਦਲਾ_ਬਦਲੀ_Array64(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, length, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, &error_ਕੋਡ);
    }

    ਸੂਚਕਾਂਕ = IX_RESERVED10_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        error_ਕੋਡ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਸੂਚਕਾਂਕ = IX_CE32_S_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, length, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, &error_ਕੋਡ);
    }

    ਸੂਚਕਾਂਕ = IX_ਜੜ੍ਹ_ELEMENTS_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, length, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, &error_ਕੋਡ);
    }

    ਸੂਚਕਾਂਕ = IX_CONTEXTS_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, length, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, &error_ਕੋਡ);
    }

    ਸੂਚਕਾਂਕ = IX_UNSAFE_BWD_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, length, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, &error_ਕੋਡ);
    }

    ਸੂਚਕਾਂਕ = IX_FAST_LATIN_ਸਾਰਣੀ_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, length, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, &error_ਕੋਡ);
    }

    ਸੂਚਕਾਂਕ = IX_SCRIPTS_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, length, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, &error_ਕੋਡ);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    ਸੂਚਕਾਂਕ = IX_RESERVED18_ਵਿੱਥ;
    ਵਿੱਥ = indexes[ਸੂਚਕਾਂਕ];
    length = indexes[ਸੂਚਕਾਂਕ + 1] - ਵਿੱਥ;
    ਜੇ(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        error_ਕੋਡ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਾਪਸ size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
          ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) { ਵਾਪਸ 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t ਸਿਰਲੇਖ_ਮਾਪ=udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        // Try to swap the old format version which did not have a standard data header.
        *p_Error_ਕੋਡ=U_ZERO_ERROR;
        ਵਾਪਸ ਅਦਲਾ_ਬਦਲੀ_Format_Version3(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    }

    /* check data format and format version */
    ਅਚਲ UDataInfo &info=*(ਅਚਲ UDataInfo *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+4);
    ਜੇ(!(
        info.ਅੰਕੜੇ_Format[0]==0x55 &&   // dataFormat="UCol"
        info.ਅੰਕੜੇ_Format[1]==0x43 &&
        info.ਅੰਕੜੇ_Format[2]==0x6f &&
        info.ਅੰਕੜੇ_Format[3]==0x6c &&
        (3<=info.format_ਸੰਸਕਰਣ[0] && info.format_ਸੰਸਕਰਣ[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.ਅੰਕੜੇ_Format[0], info.ਅੰਕੜੇ_Format[1],
                         info.ਅੰਕੜੇ_Format[2], info.ਅੰਕੜੇ_Format[3],
                         info.format_ਸੰਸਕਰਣ[0], info.format_ਸੰਸਕਰਣ[1]);
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_ਅੰਕੜੇ=(ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;
    ਜੇ(length>=0) { length-=ਸਿਰਲੇਖ_ਮਾਪ; }
    ਬਾਹਰਲਾ_ਅੰਕੜੇ=(ਬਾਹਰਲਾ_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) ? ਖਾਲੀ_ਸੂਚਕ : (ਅੱਖਰ *)ਬਾਹਰਲਾ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;
    int32_t collation_ਮਾਪ;
    ਜੇ(info.format_ਸੰਸਕਰਣ[0]>=4) {
        collation_ਮਾਪ=ਅਦਲਾ_ਬਦਲੀ_Format_Version4(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, *p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        collation_ਮਾਪ=ਅਦਲਾ_ਬਦਲੀ_Format_Version3(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    }
    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਸਿਰਲੇਖ_ਮਾਪ+collation_ਮਾਪ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_ਅਦਲਾ_ਬਦਲੀ_Inverse_UCA(ਅਚਲ UDataSwapper *ds,
                    ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                    UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UDataInfo *pInfo;
    int32_t ਸਿਰਲੇਖ_ਮਾਪ;

    ਅਚਲ uint8_t *ਵਿੱਚ_ਬਾਈਟਾਂ;
    uint8_t *ਬਾਹਰਲਾ_ਬਾਈਟਾਂ;

    ਅਚਲ Inverse_UCATable_ਸਿਰਲੇਖ *ਵਿੱਚ_ਸਿਰਲੇਖ;
    Inverse_UCATable_ਸਿਰਲੇਖ *ਬਾਹਰਲਾ_ਸਿਰਲੇਖ;
    Inverse_UCATable_ਸਿਰਲੇਖ ਸਿਰਲੇਖ={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    ਸਿਰਲੇਖ_ਮਾਪ=udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* check data format and format version */
    pInfo=(ਅਚਲ UDataInfo *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+4);
    ਜੇ(!(
        pInfo->ਅੰਕੜੇ_Format[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x6e &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x76 &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x43 &&
        pInfo->format_ਸੰਸਕਰਣ[0]==2 &&
        pInfo->format_ਸੰਸਕਰਣ[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->ਅੰਕੜੇ_Format[0], pInfo->ਅੰਕੜੇ_Format[1],
                         pInfo->ਅੰਕੜੇ_Format[2], pInfo->ਅੰਕੜੇ_Format[3],
                         pInfo->format_ਸੰਸਕਰਣ[0], pInfo->format_ਸੰਸਕਰਣ[1]);
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_ਬਾਈਟਾਂ=(ਅਚਲ uint8_t *)ਵਿੱਚ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;
    ਬਾਹਰਲਾ_ਬਾਈਟਾਂ=(uint8_t *)ਬਾਹਰਲਾ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;

    ਵਿੱਚ_ਸਿਰਲੇਖ=(ਅਚਲ Inverse_UCATable_ਸਿਰਲੇਖ *)ਵਿੱਚ_ਬਾਈਟਾਂ;
    ਬਾਹਰਲਾ_ਸਿਰਲੇਖ=(Inverse_UCATable_ਸਿਰਲੇਖ *)ਬਾਹਰਲਾ_ਬਾਈਟਾਂ;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    ਜੇ(length<0) {
        ਸਿਰਲੇਖ.byte_ਮਾਪ=udata_readInt32(ds, ਵਿੱਚ_ਸਿਰਲੇਖ->byte_ਮਾਪ);
    } ਨਹੀਂਤਾਂ ਜੇ(
        ((length-ਸਿਰਲੇਖ_ਮਾਪ)<(8*4) ||
         (uint32_t)(length-ਸਿਰਲੇਖ_ਮਾਪ)<(ਸਿਰਲੇਖ.byte_ਮਾਪ=udata_readInt32(ds, ਵਿੱਚ_ਸਿਰਲੇਖ->byte_ਮਾਪ)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        ਜੇ(ਵਿੱਚ_ਬਾਈਟਾਂ!=ਬਾਹਰਲਾ_ਬਾਈਟਾਂ) {
            uprv_memcpy(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, ਵਿੱਚ_ਬਾਈਟਾਂ, ਸਿਰਲੇਖ.byte_ਮਾਪ);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        ਸਿਰਲੇਖ.ਸਾਰਣੀ_ਮਾਪ=   ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->ਸਾਰਣੀ_ਮਾਪ);
        ਸਿਰਲੇਖ.conts_ਮਾਪ=   ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->conts_ਮਾਪ);
        ਸਿਰਲੇਖ.ਸਾਰਣੀ=       ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->ਸਾਰਣੀ);
        ਸਿਰਲੇਖ.conts=       ds->readUInt32(ਵਿੱਚ_ਸਿਰਲੇਖ->conts);

        /* swap the 32-bit integers in the header */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਸਿਰਲੇਖ, 5*4, ਬਾਹਰਲਾ_ਸਿਰਲੇਖ, p_Error_ਕੋਡ);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.ਸਾਰਣੀ, ਸਿਰਲੇਖ.ਸਾਰਣੀ_ਮਾਪ*3*4,
                           ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.ਸਾਰਣੀ, p_Error_ਕੋਡ);

        /* swap the continuation table; contsSize counts UChars */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.conts, ਸਿਰਲੇਖ.conts_ਮਾਪ*U_SIZEOF_UCHAR,
                           ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਿਰਲੇਖ.conts, p_Error_ਕੋਡ);
    }

    ਵਾਪਸ ਸਿਰਲੇਖ_ਮਾਪ+ਸਿਰਲੇਖ.byte_ਮਾਪ;
}

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_COLLATION */
