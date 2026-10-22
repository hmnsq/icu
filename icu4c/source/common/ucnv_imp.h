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

#جے_بنا_تعریف UCNV_IMP_H
#تعریف_کرو UCNV_IMP_H

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/uloc.h"
#ضمن "ucnv_bld.h"

/*
 * Fast check for whether a charset name is "UTF-8".
 * This does not recognize all of the variations that ucnv_open()
 * and other functions recognize, but it covers most cases.
 * @param name const char * charset name
 * @return
 */
#تعریف_کرو UCNV_FAST_IS_UTF8(name) \
    (((name[0]=='U' ? \
      (                name[1]=='T' && name[2]=='F') : \
      (name[0]=='u' && name[1]=='t' && name[2]=='f'))) \
  && (name[3]=='-' ? \
     (name[4]=='8' && name[5]==0) : \
     (name[3]=='8' && name[4]==0)))

تعريفنوع بنية {
    حرف cnvName[UCNV_MAX_CONVERTER_NAME_لمائی];
    حرف locale[ULOC_FULLNAME_سمرتھا];
    uint32_t options;
} UConverterNamePieces;

U_CFUNC UBool
ucnv_canCreateConverter(ثابت حرف *converterName, UError_کوڈ *err);

/* figures out if we need to go to file to read in the data tables.
 * @param converterName The name of the converter
 * @param err The error code
 * @return the newly created converter
 */
U_CAPI UConverter*
ucnv_createConverter(UConverter* myUConverter U_LIFETIME_BOUND,
                     ثابت حرف* converterName,
                     UError_کوڈ* err);

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
                                ثابت حرف* locale,
                                uint32_t options,
                                UError_کوڈ* err);

/*
 * Creates a converter from shared data.
 * Adopts mySharedConverterData: No matter what happens, the caller must not
 * unload mySharedConverterData, except via ucnv_close(return value)
 * if this function is successful.
 */
U_CFUNC UConverter*
ucnv_create_Converter_From_Shared_انکڑے(UConverter* myUConverter U_LIFETIME_BOUND,
                                   UConverter_Shared_انکڑے* my_Shared_Converter_انکڑے,
                                   UConverter_لدو_دلیلاں* p_دلیلاں,
                                   UError_کوڈ* err);

U_CFUNC UConverter *
ucnv_createConverterFromPackage(ثابت حرف *packageName, ثابت حرف *converterName, UError_کوڈ *err);

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
U_CFUNC UConverter_Shared_انکڑے *
ucnv_لدو_Shared_انکڑے(ثابت حرف *converterName,
                    UConverterNamePieces *pieces,
                    UConverter_لدو_دلیلاں *p_دلیلاں,
                    UError_کوڈ * err);

/**
 * This may unload the shared data in a thread safe manner.
 * This will only unload the data if no other converters are sharing it.
 */
U_CFUNC فراغ
ucnv_unload_Shared_انکڑے_If_Ready(UConverter_Shared_انکڑے *shared_انکڑے);

/**
 * This is a thread safe way to increment the reference count.
 */
U_CFUNC فراغ
ucnv_increment_Ref_گنتی(UConverter_Shared_انکڑے *shared_انکڑے);

/**
 * These are the default error handling callbacks for the charset conversion framework.
 * For performance reasons, they are only called to handle an error (not normally called for a reset or close).
 */
#تعریف_کرو UCNV_منزل_U_DEFAULT_CALLBACK ((UConverter_منزل_UCallback) UCNV_منزل_U_CALLBACK_SUBSTITUTE)
#تعریف_کرو UCNV_FROM_U_DEFAULT_CALLBACK ((UConverterFromUCallback) UCNV_FROM_U_CALLBACK_SUBSTITUTE)

#جے_انت

#جے_انت /* _UCNV_IMP */
