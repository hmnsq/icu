// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 **********************************************************************
 *   Copyright (C) 1999-2006, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 **********************************************************************
 *
 *
 *  ucnv_io.h:
 *  defines  variables and functions pertaining to converter name resolution
 *  aspect of the conversion code
 */

#నిర్వచితం_కాకపోతే UCNV_IO_H
#నిర్వచించు UCNV_IO_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "udataswp.h"

#నిర్వచించు UCNV_AMBIGUOUS_ALIAS_MAP_బిట్టు 0x8000
#నిర్వచించు UCNV_కలిగియుంది_OPTION_బిట్టు 0x4000
#నిర్వచించు UCNV_CONVERTER_సూచీ_ముసుగు 0xFFF
#నిర్వచించు UCNV_అంకె_RESERVED_ట్యాగులు 2
#నిర్వచించు UCNV_అంకె_HIDDEN_ట్యాగులు 1

గణన {
    UCNV_IO_UNNORMALIZED,
    UCNV_IO_STD_NORMALIZED,
    UCNV_IO_NORM_TYPE_లెక్క
};

రకనిర్వచనం నిర్మాణం {
    uint16_t stringNormalizationType;
    uint16_t కలిగియుంది_Cnv_Option_Info;
} UConverterAliasOptions;

రకనిర్వచనం నిర్మాణం UConverterAlias {
    స్థిరరాశి uint16_t *converterList;
    స్థిరరాశి uint16_t *ట్యాగ్_List;
    స్థిరరాశి uint16_t *aliasList;
    స్థిరరాశి uint16_t *untaggedConvArray;
    స్థిరరాశి uint16_t *taggedAliasArray;
    స్థిరరాశి uint16_t *taggedAliasLists;
    స్థిరరాశి UConverterAliasOptions *option_పట్టిక;
    స్థిరరాశి uint16_t *string_పట్టిక;
    స్థిరరాశి uint16_t *normalized_String_పట్టిక;

    uint32_t converter_List_కొలత;
    uint32_t ట్యాగ్_List_కొలత;
    uint32_t alias_List_కొలత;
    uint32_t untagged_Conv_Array_కొలత;
    uint32_t tagged_Alias_Array_కొలత;
    uint32_t tagged_Alias_Lists_కొలత;
    uint32_t option_పట్టిక_కొలత;
    uint32_t string_పట్టిక_కొలత;
    uint32_t normalized_String_పట్టిక_కొలత;
} UConverterAlias;

/**
 * \var ucnv_io_stripForCompare
 * Remove the underscores, dashes and spaces from the name, and convert
 * the name to lower case.
 * @param dst The destination buffer, which is <= the buffer of name.
 * @param dst The destination buffer, which is <= the buffer of name.
 * @see ucnv_compareNames
 * @return the destination buffer.
 */
#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripASCIIForCompare
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripEBCDICForCompare
#లేకపోతే
#   error U_CHARSET_FAMILY is not valid
#అయితే_ముగింపు

U_CAPI అక్షరం* U_CALLCONV
ucnv_io_stripASCIIForCompare(అక్షరం* dst U_LIFETIME_BOUND, స్థిరరాశి అక్షరం* name);

U_CAPI అక్షరం* U_CALLCONV
ucnv_io_stripEBCDICForCompare(అక్షరం* dst U_LIFETIME_BOUND, స్థిరరాశి అక్షరం* name);

/**
 * Map a converter alias name to a canonical converter name.
 * The alias is searched for case-insensitively, the converter name
 * is returned in mixed-case.
 * Returns NULL if the alias is not found.
 * @param alias The alias name to be searched.
 * @param containsOption A return value stating whether the returned converter name contains an option (a comma)
 * @param pErrorCode The error code
 * @return the converter name in mixed-case, return NULL if the alias is not found.
 */
U_CAPI స్థిరరాశి అక్షరం *
ucnv_io_getConverterName(స్థిరరాశి అక్షరం *alias, UBool *కలిగియుంది_Option, UError_సంకేతం *p_Error_సంకేతం);

/**
 * Return the number of all known converter names (no aliases).
 * @param pErrorCode The error code
 * @return the number of all aliases
 */
U_CAPI uint16_t
ucnv_io_లెక్క_తెలిసిన_Converters(UError_సంకేతం *p_Error_సంకేతం);

/**
 * Swap an ICU converter alias table. See implementation for details.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_మార్పిడి_Aliases(స్థిరరాశి UDataSwapper *ds,
                 స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t నిడివి, శూన్య *బహిర్గతం_దత్తాంశం,
                 UError_సంకేతం *p_Error_సంకేతం);

#అయితే_ముగింపు

#అయితే_ముగింపు /* _UCNV_IO */

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
