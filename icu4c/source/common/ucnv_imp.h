// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*
*
*  ucnv_imp.h:
*  Contains all internal and external data structure definitions
* Created & Maintained by Bertrand A. Damiba
*
*
*
* ATTENTION:
* ---------
* Although the data structures in this file are open and stack allocatable
* we reserve the right to hide them in further releases.
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UCNV_IMP_H
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_IMP_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"

/*
 * Fast check for whether a charset name is "UTF-8".
 * This does not recognize all of the variations that ucnv_open()
 * and other functions recognize, but it covers most cases.
 * @param name const char * charset name
 * @return
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_FAST_IS_UTF8(name) \
    (((name[0]=='U' ? \
      (                name[1]=='T' && name[2]=='F') : \
      (name[0]=='u' && name[1]=='t' && name[2]=='f'))) \
  && (name[3]=='-' ? \
     (name[4]=='8' && name[5]==0) : \
     (name[3]=='8' && name[4]==0)))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ cnvName[UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ];
    ಅಕ್ಷರ locale[ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ];
    uint32_t options;
} UConverterNamePieces;

U_CFUNC UBool
ucnv_canCreateConverter(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *converterName, UError_ಸಂಕೇತ *err);

/* figures out if we need to go to file to read in the data tables.
 * @param converterName The name of the converter
 * @param err The error code
 * @return the newly created converter
 */
U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* converterName,
                     UError_ಸಂಕೇತ* err);

/*
 * Open a purely algorithmic converter, specified by a type constant.
 * @param myUConverter  NULL, or pre-allocated UConverter structure to avoid
 *                      a memory allocation
 * @param type          requested converter type
 * @param locale        locale parameter, or ""
 * @param options       converter options bit set (default 0)
 * @param err           ICU error code, not tested for U_FAILURE on input
 *                      because this is an internal function
 * @internal
 */
U_CFUNC UConverter*
ucnv_createAlgorithmicConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                                UConverterType type,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale,
                                uint32_t options,
                                UError_ಸಂಕೇತ* err);

/*
 * Creates a converter from shared data.
 * Adopts mySharedConverterData: No matter what happens, the caller must not
 * unload mySharedConverterData, except via ucnv_close(return value)
 * if this function is successful.
 */
U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_ದತ್ತಾಂಶ(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_ದತ್ತಾಂಶ* my_Shared_Converter_ದತ್ತಾಂಶ,
                                   UConverter_ತುಂಬು_ವಾದಗಳು* p_ವಾದಗಳು,
                                   UError_ಸಂಕೇತ* err);

U_CFUNC UConverter *
ucnv_createConverterFromPackage(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *packageName, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *converterName, UError_ಸಂಕೇತ *err);

/**
 * Load a converter but do not create a UConverter object.
 * Simply return the UConverterSharedData.
 * Performs alias lookup etc.
 * The UConverterNamePieces need not be initialized
 * before calling this function.
 * The UConverterLoadArgs must be initialized
 * before calling this function.
 * If the args are passed in, then the pieces must be passed in too.
 * In other words, the following combinations are allowed:
 * - pieces==NULL && args==NULL
 * - pieces!=NULL && args==NULL
 * - pieces!=NULL && args!=NULL
 * @internal
 */
U_CFUNC UConverter_Shared_ದತ್ತಾಂಶ *
ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *converterName,
                    UConverterNamePieces *pieces,
                    UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
                    UError_ಸಂಕೇತ * err);

/**
 * This may unload the shared data in a thread safe manner.
 * This will only unload the data if no other converters are sharing it.
 */
U_CFUNC ಶೂನ್ಯ
ucnv_unload_Shared_ದತ್ತಾಂಶ_If_Ready(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ);

/**
 * This is a thread safe way to increment the reference count.
 */
U_CFUNC ಶೂನ್ಯ
ucnv_increment_Ref_ಎಣಿಕೆ(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ);

/**
 * These are the default error handling callbacks for the charset conversion framework.
 * For performance reasons, they are only called to handle an error (not normally called for a reset or close).
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_ಗಮ್ಯ_U_DEFAULT_CALLBACK ((UConverter_ಗಮ್ಯ_UCallback) UCNV_ಗಮ್ಯ_U_CALLBACK_SUBSTITUTE)
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_FROM_U_DEFAULT_CALLBACK ((UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE)

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* _UCNV_IMP */
