// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 *
 *   Copyright (C) 2003-2016, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *
 *******************************************************************************
 *   file name:  usprep.cpp
 *   encoding:   UTF-8
 *   tab size:   8 (not used)
 *   indentation:4
 *
 *   created on: 2003jul2
 *   created by: Ram Viswanadha
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_IDNA

#ಅನ್ತರ್ಭಾವ "unicode/usprep.h"

#ಅನ್ತರ್ಭಾವ "unicode/normalizer2.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/uversion.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "sprpimpl.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "ubidi_props.h"
#ಅನ್ತರ್ಭಾವ "uprops.h"

U_NAMESPACE_USE

U_CDECL_BEGIN

/*
Static cache for already opened StringPrep profiles
*/
ಸ್ಥಿರ UHashtable *SHARED_ದತ್ತಾಂಶ_HASHTABLE = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::UInitOnce g_Shared_ದತ್ತಾಂಶ_Init_Once {};

ಸ್ಥಿರ UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
ಸ್ಥಿರ UVersionInfo ದತ್ತಾಂಶ_ಆವೃತ್ತಿ={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ PROFILE_ಹೆಸರುಗಳು[] = {
    "rfc3491",      /* USPREP_RFC3491_NAMEPREP */
    "rfc3530cs",    /* USPREP_RFC3530_NFS4_CS_PREP */
    "rfc3530csci",  /* USPREP_RFC3530_NFS4_CS_PREP_CI */
    "rfc3491",      /* USPREP_RFC3530_NSF4_CIS_PREP */
    "rfc3530mixp",  /* USPREP_RFC3530_NSF4_MIXED_PREP_PREFIX */
    "rfc3491",      /* USPREP_RFC3530_NSF4_MIXED_PREP_SUFFIX */
    "rfc3722",      /* USPREP_RFC3722_ISCSI */
    "rfc3920node",  /* USPREP_RFC3920_NODEPREP */
    "rfc3920res",   /* USPREP_RFC3920_RESOURCEPREP */
    "rfc4011",      /* USPREP_RFC4011_MIB */
    "rfc4013",      /* USPREP_RFC4013_SASLPREP */
    "rfc4505",      /* USPREP_RFC4505_TRACE */
    "rfc4518",      /* USPREP_RFC4518_LDAP */
    "rfc4518ci",    /* USPREP_RFC4518_LDAP_CI */
};

ಸ್ಥಿರ UBool U_CALLCONV
isSPrepAcceptable(ಶೂನ್ಯ * /* context */,
             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /* type */, 
             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /* name */,
             ಸ್ಥಿರಾಂಕ UDataInfo *pInfo) {
    ಆದರೆ(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x50 &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x52 &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x50 &&
        pInfo->format_ಆವೃತ್ತಿ[0]==3 &&
        pInfo->format_ಆವೃತ್ತಿ[2]==UTRIE_ಸರಿಸು &&
        pInfo->format_ಆವೃತ್ತಿ[3]==UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(ದತ್ತಾಂಶ_ಆವೃತ್ತಿ, pInfo->ದತ್ತಾಂಶ_ಆವೃತ್ತಿ, 4);
        ಹಿಂತಿರುಗಿ ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
}

ಸ್ಥಿರ int32_t U_CALLCONV
get_SPrep_Folding_ಸ್ಥಾನಾಂತರ(uint32_t ದತ್ತಾಂಶ) {

    ಹಿಂತಿರುಗಿ (int32_t)ದತ್ತಾಂಶ;

}

/* hashes an entry  */
ಸ್ಥಿರ int32_t U_CALLCONV 
hash_ನಮೂದು(ಸ್ಥಿರಾಂಕ UHashTok parm) {
    UString_Prep_ಕೀಲಿ *b = (UString_Prep_ಕೀಲಿ *)parm.ಸೂಚಕ;
    UHashTok namekey, pathkey;
    namekey.ಸೂಚಕ = b->name;
    pathkey.ಸೂಚಕ = b->ಮಾರ್ಗ;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
ಸ್ಥಿರ UBool U_CALLCONV 
compareEntries(ಸ್ಥಿರಾಂಕ UHashTok p1, ಸ್ಥಿರಾಂಕ UHashTok p2) {
    UString_Prep_ಕೀಲಿ *b1 = (UString_Prep_ಕೀಲಿ *)p1.ಸೂಚಕ;
    UString_Prep_ಕೀಲಿ *b2 = (UString_Prep_ಕೀಲಿ *)p2.ಸೂಚಕ;
    UHashTok name1, name2, path1, path2;
    name1.ಸೂಚಕ = b1->name;
    name2.ಸೂಚಕ = b2->name;
    path1.ಸೂಚಕ = b1->ಮಾರ್ಗ;
    path2.ಸೂಚಕ = b2->ಮಾರ್ಗ;
    ಹಿಂತಿರುಗಿ uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

ಸ್ಥಿರ ಶೂನ್ಯ 
usprep_unload(UStringPrepProfile* ದತ್ತಾಂಶ){
    udata_close(ದತ್ತಾಂಶ->sprep_ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ int32_t 
usprep_ಆಂತರಿಕ_flush_Cache(UBool no_Ref_ಎಣಿಕೆ){
    UStringPrepProfile *profile = ಶೂನ್ಯಸೂಚಕ;
    UString_Prep_ಕೀಲಿ  *ಕೀಲಿ  = ಶೂನ್ಯಸೂಚಕ;
    int32_t pos = UHASH_ಮೊದಲ;
    int32_t deleted_ಅಂಕೆ = 0;
    ಸ್ಥಿರಾಂಕ UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_ಬೀಗ(&usprepMutex);
    ಆದರೆ (SHARED_ದತ್ತಾಂಶ_HASHTABLE == ಶೂನ್ಯಸೂಚಕ) {
        umtx_unlock(&usprepMutex);
        ಹಿಂತಿರುಗಿ 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    ಆಗ ((e = uhash_ಮುಂದಿನ_Element(SHARED_ದತ್ತಾಂಶ_HASHTABLE, &pos)) != ಶೂನ್ಯಸೂಚಕ)
    {
        profile = (UStringPrepProfile *) e->value.ಸೂಚಕ;
        ಕೀಲಿ  = (UString_Prep_ಕೀಲಿ *) e->ಕೀಲಿ.ಸೂಚಕ;

        ಆದರೆ ((no_Ref_ಎಣಿಕೆ== ಸುಳ್ಳು && profile->ref_ಎಣಿಕೆ == 0) || 
             no_Ref_ಎಣಿಕೆ) {
            deleted_ಅಂಕೆ++;
            uhash_removeElement(SHARED_ದತ್ತಾಂಶ_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            ಆದರೆ(ಕೀಲಿ->name != ಶೂನ್ಯಸೂಚಕ) {
                uprv_free(ಕೀಲಿ->name);
                ಕೀಲಿ->name=ಶೂನ್ಯಸೂಚಕ;
            }
            ಆದರೆ(ಕೀಲಿ->ಮಾರ್ಗ != ಶೂನ್ಯಸೂಚಕ) {
                uprv_free(ಕೀಲಿ->ಮಾರ್ಗ);
                ಕೀಲಿ->ಮಾರ್ಗ=ಶೂನ್ಯಸೂಚಕ;
            }
            uprv_free(profile);
            uprv_free(ಕೀಲಿ);
        }
       
    }
    umtx_unlock(&usprepMutex);

    ಹಿಂತಿರುಗಿ deleted_ಅಂಕೆ;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

ಸ್ಥಿರ UBool U_CALLCONV usprep_cleanup(){
    ಆದರೆ (SHARED_ದತ್ತಾಂಶ_HASHTABLE != ಶೂನ್ಯಸೂಚಕ) {
        usprep_ಆಂತರಿಕ_flush_Cache(ನಿಜ);
        ಆದರೆ (SHARED_ದತ್ತಾಂಶ_HASHTABLE != ಶೂನ್ಯಸೂಚಕ && uhash_ಎಣಿಕೆ(SHARED_ದತ್ತಾಂಶ_HASHTABLE) == 0) {
            uhash_close(SHARED_ದತ್ತಾಂಶ_HASHTABLE);
            SHARED_ದತ್ತಾಂಶ_HASHTABLE = ಶೂನ್ಯಸೂಚಕ;
        }
    }
    g_Shared_ದತ್ತಾಂಶ_Init_Once.reset();
    ಹಿಂತಿರುಗಿ (SHARED_ದತ್ತಾಂಶ_HASHTABLE == ಶೂನ್ಯಸೂಚಕ);
}
U_CDECL_ಕೊನೆ


/** Initializes the cache for resources */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
createCache(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    SHARED_ದತ್ತಾಂಶ_HASHTABLE = uhash_open(hash_ನಮೂದು, compareEntries, ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        SHARED_ದತ್ತಾಂಶ_HASHTABLE = ಶೂನ್ಯಸೂಚಕ;
    }
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

ಸ್ಥಿರ ಶೂನ್ಯ 
initCache(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    umtx_initOnce(g_Shared_ದತ್ತಾಂಶ_Init_Once, &createCache, *ಸ್ಥಿತಿ_2);
}

ಸ್ಥಿರ UBool U_CALLCONV
ತುಂಬು_ದತ್ತಾಂಶ(UStringPrepProfile* profile, 
         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, 
         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name, 
         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type, 
         UError_ಸಂಕೇತ* error_ಸಂಕೇತ) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, 0, 0, 0, 0};
    UData_ಸ್ಮರಣೆ *ದತ್ತಾಂಶ_ಸ್ಮರಣೆ;
    ಸ್ಥಿರಾಂಕ int32_t *p=ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ uint8_t *pb;
    UVersionInfo norm_Unicode_ಆವೃತ್ತಿ;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    ಆದರೆ(error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    ದತ್ತಾಂಶ_ಸ್ಮರಣೆ=udata_openChoice(ಮಾರ್ಗ, type, name, isSPrepAcceptable, ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    p = static_cast<ಸ್ಥಿರಾಂಕ int32_t*>(udata_get_ಸ್ಮರಣೆ(ದತ್ತಾಂಶ_ಸ್ಮರಣೆ));
    pb = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p + SPREP_ಸೂಚ್ಯಂಕ_ತುದಿ);
    utrie_unserialize(&_sprepTrie, pb, p[SPREP_ಸೂಚ್ಯಂಕ_TRIE_ಗಾತ್ರ], error_ಸಂಕೇತ);
    _sprepTrie.get_Folding_ಸ್ಥಾನಾಂತರ=get_SPrep_Folding_ಸ್ಥಾನಾಂತರ;


    ಆದರೆ(U_FAILURE(*error_ಸಂಕೇತ)) {
        udata_close(ದತ್ತಾಂಶ_ಸ್ಮರಣೆ);
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    /* in the mutex block, set the data for this process */
    umtx_ಬೀಗ(&usprepMutex);
    ಆದರೆ(profile->sprep_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        profile->sprep_ದತ್ತಾಂಶ=ದತ್ತಾಂಶ_ಸ್ಮರಣೆ;
        ದತ್ತಾಂಶ_ಸ್ಮರಣೆ=ಶೂನ್ಯಸೂಚಕ;
        uprv_memcpy(&profile->indexes, p, ಗಾತ್ರ(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, ಗಾತ್ರ(UTrie));
    } ಇಲ್ಲದಿದ್ದರೆ {
        p = static_cast<ಸ್ಥಿರಾಂಕ int32_t*>(udata_get_ಸ್ಮರಣೆ(profile->sprep_ದತ್ತಾಂಶ));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mapping_ದತ್ತಾಂಶ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p + SPREP_ಸೂಚ್ಯಂಕ_ತುದಿ) + profile->indexes[SPREP_ಸೂಚ್ಯಂಕ_TRIE_ಗಾತ್ರ]);
    
    u_get_Unicode_ಆವೃತ್ತಿ(norm_Unicode_ಆವೃತ್ತಿ);
    normUniVer = (norm_Unicode_ಆವೃತ್ತಿ[0] << 24) + (norm_Unicode_ಆವೃತ್ತಿ[1] << 16) + 
                 (norm_Unicode_ಆವೃತ್ತಿ[2] << 8 ) + (norm_Unicode_ಆವೃತ್ತಿ[3]);
    sprepUniVer = (ದತ್ತಾಂಶ_ಆವೃತ್ತಿ[0] << 24) + (ದತ್ತಾಂಶ_ಆವೃತ್ತಿ[1] << 16) + 
                  (ದತ್ತಾಂಶ_ಆವೃತ್ತಿ[2] << 8 ) + (ದತ್ತಾಂಶ_ಆವೃತ್ತಿ[3]);
    normCorrVer = profile->indexes[SPREP_NORM_CORRECTNS_ಕೊನೆಯ_UNI_ಆವೃತ್ತಿ];
    
    ಆದರೆ(U_FAILURE(*error_ಸಂಕೇತ)){
        udata_close(ದತ್ತಾಂಶ_ಸ್ಮರಣೆ);
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
        udata_close(ದತ್ತಾಂಶ_ಸ್ಮರಣೆ);
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    profile->is_ದತ್ತಾಂಶ_Loaded = ನಿಜ;

    /* if a different thread set it first, then close the extra data */
    ಆದರೆ(ದತ್ತಾಂಶ_ಸ್ಮರಣೆ!=ಶೂನ್ಯಸೂಚಕ) {
        udata_close(ದತ್ತಾಂಶ_ಸ್ಮರಣೆ); /* nullptr if it was set correctly */
    }


    ಹಿಂತಿರುಗಿ profile->is_ದತ್ತಾಂಶ_Loaded;
}

ಸ್ಥಿರ UStringPrepProfile* 
usprep_getProfile(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, 
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name,
                  UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2){

    UStringPrepProfile* profile = ಶೂನ್ಯಸೂಚಕ;

    initCache(ಸ್ಥಿತಿ_2);

    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    UString_Prep_ಕೀಲಿ ರಾಶಿ_ಕೀಲಿ;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    ರಾಶಿ_ಕೀಲಿ.name = const_cast<ಅಕ್ಷರ*>(name);
    ರಾಶಿ_ಕೀಲಿ.ಮಾರ್ಗ = const_cast<ಅಕ್ಷರ*>(ಮಾರ್ಗ);

    /* fetch the data from the cache */
    umtx_ಬೀಗ(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_ದತ್ತಾಂಶ_HASHTABLE, &ರಾಶಿ_ಕೀಲಿ));
    ಆದರೆ(profile != ಶೂನ್ಯಸೂಚಕ) {
        profile->ref_ಎಣಿಕೆ++;
    }
    umtx_unlock(&usprepMutex);
    
    ಆದರೆ(profile == ಶೂನ್ಯಸೂಚಕ) {
        /* else load the data and put the data in the cache */
        Local_ಸ್ಮರಣೆ<UStringPrepProfile> newProfile;
        ಆದರೆ(newProfile.allocateInsteadAndReset() == ಶೂನ್ಯಸೂಚಕ) {
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }

        /* load the data */
        ಆದರೆ(!ತುಂಬು_ದತ್ತಾಂಶ(newProfile.getAlias(), ಮಾರ್ಗ, name, SPREP_ದತ್ತಾಂಶ_TYPE, ಸ್ಥಿತಿ_2) || U_FAILURE(*ಸ್ಥಿತಿ_2) ){
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        Local_ಸ್ಮರಣೆ<UString_Prep_ಕೀಲಿ> ಕೀಲಿ;
        Local_ಸ್ಮರಣೆ<ಅಕ್ಷರ> ಕೀಲಿ_Name;
        Local_ಸ್ಮರಣೆ<ಅಕ್ಷರ> ಕೀಲಿ_ಮಾರ್ಗ;
        ಆದರೆ( ಕೀಲಿ.allocateInsteadAndReset() == ಶೂನ್ಯಸೂಚಕ ||
            ಕೀಲಿ_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == ಶೂನ್ಯಸೂಚಕ ||
            (ಮಾರ್ಗ != ಶೂನ್ಯಸೂಚಕ &&
             ಕೀಲಿ_ಮಾರ್ಗ.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(ಮಾರ್ಗ)+1)) == ಶೂನ್ಯಸೂಚಕ)
         ) {
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            usprep_unload(newProfile.getAlias());
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }

        umtx_ಬೀಗ(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_ದತ್ತಾಂಶ_HASHTABLE, &ರಾಶಿ_ಕೀಲಿ));
        ಆದರೆ(profile != ಶೂನ್ಯಸೂಚಕ) {
            profile->ref_ಎಣಿಕೆ++;
            usprep_unload(newProfile.getAlias());
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            /* initialize the key members */
            ಕೀಲಿ->name = ಕೀಲಿ_Name.orphan();
            uprv_strcpy(ಕೀಲಿ->name, name);
            ಆದರೆ(ಮಾರ್ಗ != ಶೂನ್ಯಸೂಚಕ){
                ಕೀಲಿ->ಮಾರ್ಗ = ಕೀಲಿ_ಮಾರ್ಗ.orphan();
                uprv_strcpy(ಕೀಲಿ->ಮಾರ್ಗ, ಮಾರ್ಗ);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_ಎಣಿಕೆ = 1;
            uhash_put(SHARED_ದತ್ತಾಂಶ_HASHTABLE, ಕೀಲಿ.orphan(), profile, ಸ್ಥಿತಿ_2);
        }
        umtx_unlock(&usprepMutex);
    }

    ಹಿಂತಿರುಗಿ profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, 
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name,
            UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2){

    ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
       
    /* initialize the profile struct members */
    ಹಿಂತಿರುಗಿ usprep_getProfile(ಮಾರ್ಗ,name,ಸ್ಥಿತಿ_2);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    int32_t ಸೂಚ್ಯಂಕ = (int32_t)type;
    ಆದರೆ (ಸೂಚ್ಯಂಕ < 0 || ಸೂಚ್ಯಂಕ >= UPRV_LENGTHOF(PROFILE_ಹೆಸರುಗಳು)) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ usprep_open(ಶೂನ್ಯಸೂಚಕ, PROFILE_ಹೆಸರುಗಳು[ಸೂಚ್ಯಂಕ], ಸ್ಥಿತಿ_2);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    ಆದರೆ(profile==ಶೂನ್ಯಸೂಚಕ){
        ಹಿಂತಿರುಗಿ;
    }

    umtx_ಬೀಗ(&usprepMutex);
    /* decrement the ref count*/
    ಆದರೆ(profile->ref_ಎಣಿಕೆ > 0){
        profile->ref_ಎಣಿಕೆ--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC ಶೂನ್ಯ 
uprv_syntaxError(ಸ್ಥಿರಾಂಕ char16_t* ನಿಯಮಗಳು,
                 int32_t pos,
                 int32_t ನಿಯಮಗಳು_Len,
                 UParseError* parseError){
    ಆದರೆ(parseError == ಶೂನ್ಯಸೂಚಕ){
        ಹಿಂತಿರುಗಿ;
    }
    parseError->ಸ್ಥಾನಾಂತರ = pos;
    parseError->ಸಾಲು = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t ಆರಂಭ = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t ಎಲ್ಲೆ = pos;
    
    u_memcpy(parseError->preContext,ನಿಯಮಗಳು+ಆರಂಭ,ಎಲ್ಲೆ-ಆರಂಭ);
    //null terminate the buffer
    parseError->preContext[ಎಲ್ಲೆ-ಆರಂಭ] = 0;
    
    // for post-context; include error rules[pos]
    ಆರಂಭ = pos;
    ಎಲ್ಲೆ = ಆರಂಭ + (U_PARSE_CONTEXT_LEN-1);
    ಆದರೆ (ಎಲ್ಲೆ > ನಿಯಮಗಳು_Len) {
        ಎಲ್ಲೆ = ನಿಯಮಗಳು_Len;
    }
    ಆದರೆ (ಆರಂಭ < ನಿಯಮಗಳು_Len) {
        u_memcpy(parseError->postContext,ನಿಯಮಗಳು+ಆರಂಭ,ಎಲ್ಲೆ-ಆರಂಭ);
    }
    //null terminate the buffer
    parseError->postContext[ಎಲ್ಲೆ-ಆರಂಭ]= 0;
}


ಸ್ಥಿರ ಸಾಲೊಳಗೆ UStringPrepType
get_ಮೌಲ್ಯಗಳು(uint16_t trie_ಶಬ್ದ, int16_t& value, UBool& is_ಸೂಚ್ಯಂಕ){

    UStringPrepType type;
    ಆದರೆ(trie_ಶಬ್ದ == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_ಎಲ್ಲೆ;
        is_ಸೂಚ್ಯಂಕ =ಸುಳ್ಳು;
        value = 0;
    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(trie_ಶಬ್ದ >= SPREP_TYPE_ಮಿತಿ){
        type = static_cast<UStringPrepType>(trie_ಶಬ್ದ - SPREP_TYPE_ಮಿತಿ);
        is_ಸೂಚ್ಯಂಕ =ಸುಳ್ಳು;
        value = 0;
    }ಇಲ್ಲದಿದ್ದರೆ{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        ಆದರೆ(trie_ಶಬ್ದ & 0x02){
            is_ಸೂಚ್ಯಂಕ = ನಿಜ;
            value = trie_ಶಬ್ದ  >> 2; //mask off the lower 2 bits and shift
        }ಇಲ್ಲದಿದ್ದರೆ{
            is_ಸೂಚ್ಯಂಕ = ಸುಳ್ಳು;
            value = static_cast<int16_t>(trie_ಶಬ್ದ);
            value =  (value >> 2);
        }
 
        ಆದರೆ((trie_ಶಬ್ದ>>2) == SPREP_MAX_ಸೂಚ್ಯಂಕ_ಮೌಲ್ಯ){
            type = USPREP_DELETE;
            is_ಸೂಚ್ಯಂಕ =ಸುಳ್ಳು;
            value = 0;
        }
    }
    ಹಿಂತಿರುಗಿ type;
}

// TODO: change to writing to UnicodeString not char16_t *
ಸ್ಥಿರ int32_t 
usprep_map(  ಸ್ಥಿರಾಂಕ UStringPrepProfile* profile, 
             ಸ್ಥಿರಾಂಕ char16_t* src, int32_t src_ಉದ್ದಳತೆ,
             char16_t* dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
             int32_t options,
             UParseError* parseError,
             UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2 ){
    
    uint16_t ಫಲಿತಾಂಶ;
    int32_t dest_ಸೂಚ್ಯಂಕ=0;
    int32_t src_ಸೂಚ್ಯಂಕ;
    UBool allowUnassigned = static_cast<UBool>((options & USPREP_ALLOW_UNASSIGNED) > 0);
    UStringPrepType type;
    int16_t value;
    UBool is_ಸೂಚ್ಯಂಕ;
    ಸ್ಥಿರಾಂಕ int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    ಗಾಗಿ(src_ಸೂಚ್ಯಂಕ=0;src_ಸೂಚ್ಯಂಕ<src_ಉದ್ದಳತೆ;){
        UChar32 ch;

        U16_ಮುಂದಿನ(src,src_ಸೂಚ್ಯಂಕ,src_ಉದ್ದಳತೆ,ch);
        
        ಫಲಿತಾಂಶ=0;

        UTRIE_GET16(&profile->sprepTrie,ch,ಫಲಿತಾಂಶ);
        
        type = get_ಮೌಲ್ಯಗಳು(ಫಲಿತಾಂಶ, value, is_ಸೂಚ್ಯಂಕ);

        // check if the source codepoint is unassigned
        ಆದರೆ(type == USPREP_UNASSIGNED && allowUnassigned == ಸುಳ್ಳು){

            uprv_syntaxError(src,src_ಸೂಚ್ಯಂಕ-U16_ಉದ್ದಳತೆ(ch), src_ಉದ್ದಳತೆ,parseError);
            *ಸ್ಥಿತಿ_2 = U_STRINGPREP_UNASSIGNED_ERROR;
            ಹಿಂತಿರುಗಿ 0;
            
        }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(type == USPREP_MAP){
            
            int32_t ಸೂಚ್ಯಂಕ, length;

            ಆದರೆ(is_ಸೂಚ್ಯಂಕ){
                ಸೂಚ್ಯಂಕ = value;
                ಆದರೆ(ಸೂಚ್ಯಂಕ >= indexes[SPREP_ONE_UCHAR_MAPPING_ಸೂಚ್ಯಂಕ_ಆರಂಭ] &&
                         ಸೂಚ್ಯಂಕ < indexes[SPREP_TWO_UCHARS_MAPPING_ಸೂಚ್ಯಂಕ_ಆರಂಭ]){
                    length = 1;
                }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸೂಚ್ಯಂಕ >= indexes[SPREP_TWO_UCHARS_MAPPING_ಸೂಚ್ಯಂಕ_ಆರಂಭ] &&
                         ಸೂಚ್ಯಂಕ < indexes[SPREP_THREE_UCHARS_MAPPING_ಸೂಚ್ಯಂಕ_ಆರಂಭ]){
                    length = 2;
                }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸೂಚ್ಯಂಕ >= indexes[SPREP_THREE_UCHARS_MAPPING_ಸೂಚ್ಯಂಕ_ಆರಂಭ] &&
                         ಸೂಚ್ಯಂಕ < indexes[SPREP_FOUR_UCHARS_MAPPING_ಸೂಚ್ಯಂಕ_ಆರಂಭ]){
                    length = 3;
                }ಇಲ್ಲದಿದ್ದರೆ{
                    length = profile->mapping_ದತ್ತಾಂಶ[ಸೂಚ್ಯಂಕ++];
         
                }

                /* copy mapping to destination */
                ಗಾಗಿ(int32_t i=0; i< length; i++){
                    ಆದರೆ(dest_ಸೂಚ್ಯಂಕ < dest_ಸಾಮರ್ಥ್ಯ  ){
                        dest[dest_ಸೂಚ್ಯಂಕ] = profile->mapping_ದತ್ತಾಂಶ[ಸೂಚ್ಯಂಕ+i];
                    }
                    dest_ಸೂಚ್ಯಂಕ++; /* for pre-flighting */
                }  
                ಮುಂದುವರಿ;
            }ಇಲ್ಲದಿದ್ದರೆ{
                // subtract the delta to arrive at the code point
                ch -= value;
            }

        }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(type==USPREP_DELETE){
             // just consume the codepoint and continue
            ಮುಂದುವರಿ;
        }
        //copy the code point into destination
        ಆದರೆ(ch <= 0xFFFF){
            ಆದರೆ(dest_ಸೂಚ್ಯಂಕ < dest_ಸಾಮರ್ಥ್ಯ ){
                dest[dest_ಸೂಚ್ಯಂಕ] = static_cast<char16_t>(ch);
            }
            dest_ಸೂಚ್ಯಂಕ++;
        }ಇಲ್ಲದಿದ್ದರೆ{
            ಆದರೆ(dest_ಸೂಚ್ಯಂಕ+1 < dest_ಸಾಮರ್ಥ್ಯ ){
                dest[dest_ಸೂಚ್ಯಂಕ]   = U16_LEAD(ch);
                dest[dest_ಸೂಚ್ಯಂಕ+1] = U16_TRAIL(ch);
            }
            dest_ಸೂಚ್ಯಂಕ +=2;
        }
       
    }
        
    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, dest_ಸೂಚ್ಯಂಕ, ಸ್ಥಿತಿ_2);
}

/*
   1) Map -- For each character in the input, check if it has a mapping
      and, if so, replace it with its mapping.  

   2) Normalize -- Possibly normalize the result of step 1 using Unicode
      normalization. 

   3) Prohibit -- Check for any characters that are not allowed in the
      output.  If any are found, return an error.  

   4) Check bidi -- Possibly check for right-to-left characters, and if
      any are found, make sure that the whole string satisfies the
      requirements for bidirectional strings.  If the string does not
      satisfy the requirements for bidirectional strings, return an
      error.  
      [Unicode3.2] defines several bidirectional categories; each character
       has one bidirectional category assigned to it.  For the purposes of
       the requirements below, an "RandALCat character" is a character that
       has Unicode bidirectional categories "R" or "AL"; an "LCat character"
       is a character that has Unicode bidirectional category "L".  Note


       that there are many characters which fall in neither of the above
       definitions; Latin digits (<U+0030> through <U+0039>) are examples of
       this because they have bidirectional category "EN".

       In any profile that specifies bidirectional character handling, all
       three of the following requirements MUST be met:

       1) The characters in section 5.8 MUST be prohibited.

       2) If a string contains any RandALCat character, the string MUST NOT
          contain any LCat character.

       3) If a string contains any RandALCat character, a RandALCat
          character MUST be the first character of the string, and a
          RandALCat character MUST be the last character of the string.
*/
U_CAPI int32_t U_EXPORT2
usprep_prepare(   ಸ್ಥಿರಾಂಕ UStringPrepProfile* profile,
                  ಸ್ಥಿರಾಂಕ char16_t* src, int32_t src_ಉದ್ದಳತೆ,
                  char16_t* dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                  int32_t options,
                  UParseError* parseError,
                  UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2 ){

    // check error status
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ 0;
    }

    //check arguments
    ಆದರೆ(profile==ಶೂನ್ಯಸೂಚಕ ||
            (src==ಶೂನ್ಯಸೂಚಕ ? src_ಉದ್ದಳತೆ!=0 : src_ಉದ್ದಳತೆ<-1) ||
            (dest==ಶೂನ್ಯಸೂಚಕ ? dest_ಸಾಮರ್ಥ್ಯ!=0 : dest_ಸಾಮರ್ಥ್ಯ<0)) {
        *ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    //get the string length
    ಆದರೆ(src_ಉದ್ದಳತೆ < 0){
        src_ಉದ್ದಳತೆ = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.get_ಬಫರ್(src_ಉದ್ದಳತೆ);
    ಆದರೆ(b1==ಶೂನ್ಯಸೂಚಕ){
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, src_ಉದ್ದಳತೆ,
                               b1, s1.get_ಸಾಮರ್ಥ್ಯ(), options, parseError, &ಬಫರ್_ಸ್ಥಿತಿ);
    s1.release_ಬಫರ್(U_SUCCESS(ಬಫರ್_ಸ್ಥಿತಿ) ? b1Len : 0);

    ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.get_ಬಫರ್(b1Len);
        ಆದರೆ(b1==ಶೂನ್ಯಸೂಚಕ){
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, src_ಉದ್ದಳತೆ,
                           b1, s1.get_ಸಾಮರ್ಥ್ಯ(), options, parseError, &ಬಫರ್_ಸ್ಥಿತಿ);
        s1.release_ಬಫರ್(U_SUCCESS(ಬಫರ್_ಸ್ಥಿತಿ) ? b1Len : 0);
    }
    ಆದರೆ(U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)){
        *ಸ್ಥಿತಿ_2 = ಬಫರ್_ಸ್ಥಿತಿ;
        ಹಿಂತಿರುಗಿ 0;
    }

    // normalize
    UnicodeString s2;
    ಆದರೆ(profile->doNFKC){
        ಸ್ಥಿರಾಂಕ Normalizer2 *n2 = Normalizer2::getNFKCInstance(*ಸ್ಥಿತಿ_2);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*ಸ್ಥಿತಿ_2));
        ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
            ಹಿಂತಿರುಗಿ 0;
        }
        fn2.normalize(s1, s2, *ಸ್ಥಿತಿ_2);
    }ಇಲ್ಲದಿದ್ದರೆ{
        s2.fastCopyFrom(s1);
    }
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ 0;
    }

    // Prohibit and checkBiDi in one pass
    ಸ್ಥಿರಾಂಕ char16_t *b2 = s2.get_ಬಫರ್();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_ಎಣಿಕೆ, ಮೊದಲ_Char_Dir=U_CHAR_DIRECTION_ಎಣಿಕೆ;
    UBool ಎಡ_ಗಮ್ಯ_ಬಲ=ಸುಳ್ಳು, ಬಲ_ಗಮ್ಯ_ಎಡ=ಸುಳ್ಳು;
    int32_t rtlPos =-1, ltrPos =-1;

    ಗಾಗಿ(int32_t b2_ಸೂಚ್ಯಂಕ=0; b2_ಸೂಚ್ಯಂಕ<b2Len;){
        UChar32 ch = 0;
        U16_ಮುಂದಿನ(b2, b2_ಸೂಚ್ಯಂಕ, b2Len, ch);

        uint16_t ಫಲಿತಾಂಶ;
        UTRIE_GET16(&profile->sprepTrie,ch,ಫಲಿತಾಂಶ);

        int16_t value;
        UBool is_ಸೂಚ್ಯಂಕ;
        UStringPrepType type = get_ಮೌಲ್ಯಗಳು(ಫಲಿತಾಂಶ, value, is_ಸೂಚ್ಯಂಕ);

        ಆದರೆ( type == USPREP_PROHIBITED || 
            ((ಫಲಿತಾಂಶ < SPREP_TYPE_ಮಿತಿ) && (ಫಲಿತಾಂಶ & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *ಸ್ಥಿತಿ_2 = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2_ಸೂಚ್ಯಂಕ-U16_ಉದ್ದಳತೆ(ch), b2Len, parseError);
            ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            ಆದರೆ(ಮೊದಲ_Char_Dir == U_CHAR_DIRECTION_ಎಣಿಕೆ){
                ಮೊದಲ_Char_Dir = direction;
            }
            ಆದರೆ(direction == U_ಎಡ_ಗಮ್ಯ_ಬಲ){
                ಎಡ_ಗಮ್ಯ_ಬಲ = ನಿಜ;
                ltrPos = b2_ಸೂಚ್ಯಂಕ-1;
            }
            ಆದರೆ(direction == U_ಬಲ_ಗಮ್ಯ_ಎಡ || direction == U_ಬಲ_ಗಮ್ಯ_ಎಡ_ARABIC){
                ಬಲ_ಗಮ್ಯ_ಎಡ = ನಿಜ;
                rtlPos = b2_ಸೂಚ್ಯಂಕ-1;
            }
        }
    }
    ಆದರೆ(profile->checkBiDi){
        // satisfy 2
        ಆದರೆ( ಎಡ_ಗಮ್ಯ_ಬಲ && ಬಲ_ಗಮ್ಯ_ಎಡ){
            *ಸ್ಥಿತಿ_2 = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            ಹಿಂತಿರುಗಿ 0;
        }

        //satisfy 3
        ಆದರೆ( ಬಲ_ಗಮ್ಯ_ಎಡ && 
            !((ಮೊದಲ_Char_Dir == U_ಬಲ_ಗಮ್ಯ_ಎಡ || ಮೊದಲ_Char_Dir == U_ಬಲ_ಗಮ್ಯ_ಎಡ_ARABIC) &&
              (direction == U_ಬಲ_ಗಮ್ಯ_ಎಡ || direction == U_ಬಲ_ಗಮ್ಯ_ಎಡ_ARABIC))
           ){
            *ಸ್ಥಿತಿ_2 = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ s2.extract(dest, dest_ಸಾಮರ್ಥ್ಯ, *ಸ್ಥಿತಿ_2);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UDataInfo *pInfo;
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

    ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ_ಬೈಟುಗಳು;
    uint8_t *ಹೊರಹೋಗು_ಬೈಟುಗಳು;

    ಸ್ಥಿರಾಂಕ int32_t *ಒಳ_Indexes;
    int32_t indexes[16];

    int32_t i, ಸ್ಥಾನಾಂತರ, count, size;

    /* udata_swapDataHeader checks the arguments */
    ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* check data format and format version */
    pInfo=(ಸ್ಥಿರಾಂಕ UDataInfo *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+4);
    ಆದರೆ(!(
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x50 &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x52 &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x50 &&
        pInfo->format_ಆವೃತ್ತಿ[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->ದತ್ತಾಂಶ_Format[0], pInfo->ದತ್ತಾಂಶ_Format[1],
                         pInfo->ದತ್ತಾಂಶ_Format[2], pInfo->ದತ್ತಾಂಶ_Format[3],
                         pInfo->format_ಆವೃತ್ತಿ[0]);
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_ಬೈಟುಗಳು=(ಸ್ಥಿರಾಂಕ uint8_t *)ಒಳ_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ಹೊರಹೋಗು_ಬೈಟುಗಳು= (ಹೊರಹೋಗು_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ ) ? ಶೂನ್ಯಸೂಚಕ : (uint8_t *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

    ಒಳ_Indexes=(ಸ್ಥಿರಾಂಕ int32_t *)ಒಳ_ಬೈಟುಗಳು;

    ಆದರೆ(length>=0) {
        length-=ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
        ಆದರೆ(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    ಗಾಗಿ(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, ಒಳ_Indexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[SPREP_ಸೂಚ್ಯಂಕ_TRIE_ಗಾತ್ರ]+
        indexes[SPREP_ಸೂಚ್ಯಂಕ_MAPPING_ದತ್ತಾಂಶ_ಗಾತ್ರ];

    ಆದರೆ(length>=0) {
        ಆದರೆ(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        /* copy the data for inaccessible bytes */
        ಆದರೆ(ಒಳ_ಬೈಟುಗಳು!=ಹೊರಹೋಗು_ಬೈಟುಗಳು) {
            uprv_memcpy(ಹೊರಹೋಗು_ಬೈಟುಗಳು, ಒಳ_ಬೈಟುಗಳು, size);
        }

        ಸ್ಥಾನಾಂತರ=0;

        /* swap the int32_t indexes[] */
        count=16*4;
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು, count, ಹೊರಹೋಗು_ಬೈಟುಗಳು, p_Error_ಸಂಕೇತ);
        ಸ್ಥಾನಾಂತರ+=count;

        /* swap the UTrie */
        count=indexes[SPREP_ಸೂಚ್ಯಂಕ_TRIE_ಗಾತ್ರ];
        utrie_ಅದಲುಬದಲು(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, count, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);
        ಸ್ಥಾನಾಂತರ+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[SPREP_ಸೂಚ್ಯಂಕ_MAPPING_ದತ್ತಾಂಶ_ಗಾತ್ರ];
        ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, count, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);
        //offset+=count;
    }

    ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ+size;
}

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_IDNA */
