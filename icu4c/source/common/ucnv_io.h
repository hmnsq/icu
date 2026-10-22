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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UCNV_IO_H
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_IO_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "udataswp.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_AMBIGUOUS_ALIAS_MAP_ಬಿಟ್ 0x8000
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_ಒಳಗೊಂಡಿದೆ_OPTION_ಬಿಟ್ 0x4000
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_CONVERTER_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ 0xFFF
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_ಅಂಕೆ_RESERVED_ಟ್ಯಾಗುಗಳು 2
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_ಅಂಕೆ_HIDDEN_ಟ್ಯಾಗುಗಳು 1

ಎಣಿಕೆ {
    UCNV_IO_UNNORMALIZED,
    UCNV_IO_STD_NORMALIZED,
    UCNV_IO_NORM_TYPE_ಎಣಿಕೆ
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    uint16_t stringNormalizationType;
    uint16_t ಒಳಗೊಂಡಿದೆ_Cnv_Option_Info;
} UConverterAliasOptions;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UConverterAlias {
    ಸ್ಥಿರಾಂಕ uint16_t *converterList;
    ಸ್ಥಿರಾಂಕ uint16_t *ಟ್ಯಾಗ್_List;
    ಸ್ಥಿರಾಂಕ uint16_t *aliasList;
    ಸ್ಥಿರಾಂಕ uint16_t *untaggedConvArray;
    ಸ್ಥಿರಾಂಕ uint16_t *taggedAliasArray;
    ಸ್ಥಿರಾಂಕ uint16_t *taggedAliasLists;
    ಸ್ಥಿರಾಂಕ UConverterAliasOptions *option_ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ uint16_t *string_ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ uint16_t *normalized_String_ಕೋಷ್ಟಕ;

    uint32_t converter_List_ಗಾತ್ರ;
    uint32_t ಟ್ಯಾಗ್_List_ಗಾತ್ರ;
    uint32_t alias_List_ಗಾತ್ರ;
    uint32_t untagged_Conv_Array_ಗಾತ್ರ;
    uint32_t tagged_Alias_Array_ಗಾತ್ರ;
    uint32_t tagged_Alias_Lists_ಗಾತ್ರ;
    uint32_t option_ಕೋಷ್ಟಕ_ಗಾತ್ರ;
    uint32_t string_ಕೋಷ್ಟಕ_ಗಾತ್ರ;
    uint32_t normalized_String_ಕೋಷ್ಟಕ_ಗಾತ್ರ;
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
#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripASCIIForCompare
#ಅಥವಾಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripEBCDICForCompare
#ಇಲ್ಲದಿದ್ದರೆ
#   error U_CHARSET_FAMILY is not valid
#ಆದರೆ_ಅಂತ್ಯ

U_CAPI ಅಕ್ಷರ* U_CALLCONV
ucnv_io_stripASCIIForCompare(ಅಕ್ಷರ* dst U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name);

U_CAPI ಅಕ್ಷರ* U_CALLCONV
ucnv_io_stripEBCDICForCompare(ಅಕ್ಷರ* dst U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name);

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
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
ucnv_io_getConverterName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, UBool *ಒಳಗೊಂಡಿದೆ_Option, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/**
 * Return the number of all known converter names (no aliases).
 * @param pErrorCode The error code
 * @return the number of all aliases
 */
U_CAPI uint16_t
ucnv_io_ಎಣಿಕೆ_ತಿಳಿದ_Converters(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/**
 * Swap an ICU converter alias table. See implementation for details.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_ಅದಲುಬದಲು_Aliases(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t ಉದ್ದಳತೆ, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* _UCNV_IO */

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
