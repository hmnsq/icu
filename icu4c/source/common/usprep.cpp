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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_IDNA

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/usprep.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/normalizer2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uversion.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sprpimpl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubidi_props.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uprops.h"

U_NAMESPACE_USE

U_CDECL_BEGIN

/*
Static cache for already opened StringPrep profiles
*/
ਸਥਿਰ UHashtable *SHARED_ਅੰਕੜੇ_HASHTABLE = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::UInitOnce g_Shared_ਅੰਕੜੇ_Init_Once {};

ਸਥਿਰ UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
ਸਥਿਰ UVersionInfo ਅੰਕੜੇ_ਸੰਸਕਰਣ={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ PROFILE_ਨਾਂ[] = {
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

ਸਥਿਰ UBool U_CALLCONV
isSPrepAcceptable(ਖਾਲੀ * /* context */,
             ਅਚਲ ਅੱਖਰ * /* type */, 
             ਅਚਲ ਅੱਖਰ * /* name */,
             ਅਚਲ UDataInfo *pInfo) {
    ਜੇ(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ਅੰਕੜੇ_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x50 &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x52 &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x50 &&
        pInfo->format_ਸੰਸਕਰਣ[0]==3 &&
        pInfo->format_ਸੰਸਕਰਣ[2]==UTRIE_ਖਿਸਕਾਓ &&
        pInfo->format_ਸੰਸਕਰਣ[3]==UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(ਅੰਕੜੇ_ਸੰਸਕਰਣ, pInfo->ਅੰਕੜੇ_ਸੰਸਕਰਣ, 4);
        ਵਾਪਸ ਸੱਚ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਝੂਠ;
    }
}

ਸਥਿਰ int32_t U_CALLCONV
get_SPrep_Folding_ਵਿੱਥ(uint32_t ਅੰਕੜੇ) {

    ਵਾਪਸ (int32_t)ਅੰਕੜੇ;

}

/* hashes an entry  */
ਸਥਿਰ int32_t U_CALLCONV 
hash_ਇੰਦਰਾਜ(ਅਚਲ UHashTok parm) {
    UString_Prep_ਕੁੰਜੀ *b = (UString_Prep_ਕੁੰਜੀ *)parm.ਸੂਚਕ;
    UHashTok namekey, pathkey;
    namekey.ਸੂਚਕ = b->name;
    pathkey.ਸੂਚਕ = b->ਰਸਤਾ;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    ਵਾਪਸ static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
ਸਥਿਰ UBool U_CALLCONV 
compareEntries(ਅਚਲ UHashTok p1, ਅਚਲ UHashTok p2) {
    UString_Prep_ਕੁੰਜੀ *b1 = (UString_Prep_ਕੁੰਜੀ *)p1.ਸੂਚਕ;
    UString_Prep_ਕੁੰਜੀ *b2 = (UString_Prep_ਕੁੰਜੀ *)p2.ਸੂਚਕ;
    UHashTok name1, name2, path1, path2;
    name1.ਸੂਚਕ = b1->name;
    name2.ਸੂਚਕ = b2->name;
    path1.ਸੂਚਕ = b1->ਰਸਤਾ;
    path2.ਸੂਚਕ = b2->ਰਸਤਾ;
    ਵਾਪਸ uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

ਸਥਿਰ ਖਾਲੀ 
usprep_unload(UStringPrepProfile* ਅੰਕੜੇ){
    udata_close(ਅੰਕੜੇ->sprep_ਅੰਕੜੇ);
}

ਸਥਿਰ int32_t 
usprep_ਅੰਦਰੂਨੀ_flush_Cache(UBool no_Ref_ਗਿਣਤੀ){
    UStringPrepProfile *profile = ਖਾਲੀ_ਸੂਚਕ;
    UString_Prep_ਕੁੰਜੀ  *ਕੁੰਜੀ  = ਖਾਲੀ_ਸੂਚਕ;
    int32_t pos = UHASH_ਪਹਿਲਾ;
    int32_t deleted_ਅੰਕ = 0;
    ਅਚਲ UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_ਤਾਲਾ(&usprepMutex);
    ਜੇ (SHARED_ਅੰਕੜੇ_HASHTABLE == ਖਾਲੀ_ਸੂਚਕ) {
        umtx_unlock(&usprepMutex);
        ਵਾਪਸ 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    ਜਦੋਂ ((e = uhash_ਅਗਲਾ_Element(SHARED_ਅੰਕੜੇ_HASHTABLE, &pos)) != ਖਾਲੀ_ਸੂਚਕ)
    {
        profile = (UStringPrepProfile *) e->value.ਸੂਚਕ;
        ਕੁੰਜੀ  = (UString_Prep_ਕੁੰਜੀ *) e->ਕੁੰਜੀ.ਸੂਚਕ;

        ਜੇ ((no_Ref_ਗਿਣਤੀ== ਝੂਠ && profile->ref_ਗਿਣਤੀ == 0) || 
             no_Ref_ਗਿਣਤੀ) {
            deleted_ਅੰਕ++;
            uhash_removeElement(SHARED_ਅੰਕੜੇ_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            ਜੇ(ਕੁੰਜੀ->name != ਖਾਲੀ_ਸੂਚਕ) {
                uprv_free(ਕੁੰਜੀ->name);
                ਕੁੰਜੀ->name=ਖਾਲੀ_ਸੂਚਕ;
            }
            ਜੇ(ਕੁੰਜੀ->ਰਸਤਾ != ਖਾਲੀ_ਸੂਚਕ) {
                uprv_free(ਕੁੰਜੀ->ਰਸਤਾ);
                ਕੁੰਜੀ->ਰਸਤਾ=ਖਾਲੀ_ਸੂਚਕ;
            }
            uprv_free(profile);
            uprv_free(ਕੁੰਜੀ);
        }
       
    }
    umtx_unlock(&usprepMutex);

    ਵਾਪਸ deleted_ਅੰਕ;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

ਸਥਿਰ UBool U_CALLCONV usprep_cleanup(){
    ਜੇ (SHARED_ਅੰਕੜੇ_HASHTABLE != ਖਾਲੀ_ਸੂਚਕ) {
        usprep_ਅੰਦਰੂਨੀ_flush_Cache(ਸੱਚ);
        ਜੇ (SHARED_ਅੰਕੜੇ_HASHTABLE != ਖਾਲੀ_ਸੂਚਕ && uhash_ਗਿਣਤੀ(SHARED_ਅੰਕੜੇ_HASHTABLE) == 0) {
            uhash_close(SHARED_ਅੰਕੜੇ_HASHTABLE);
            SHARED_ਅੰਕੜੇ_HASHTABLE = ਖਾਲੀ_ਸੂਚਕ;
        }
    }
    g_Shared_ਅੰਕੜੇ_Init_Once.reset();
    ਵਾਪਸ (SHARED_ਅੰਕੜੇ_HASHTABLE == ਖਾਲੀ_ਸੂਚਕ);
}
U_CDECL_ਸਿਰਾ


/** Initializes the cache for resources */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
createCache(UError_ਕੋਡ &ਸਥਿਤੀ) {
    SHARED_ਅੰਕੜੇ_HASHTABLE = uhash_open(hash_ਇੰਦਰਾਜ, compareEntries, ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        SHARED_ਅੰਕੜੇ_HASHTABLE = ਖਾਲੀ_ਸੂਚਕ;
    }
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

ਸਥਿਰ ਖਾਲੀ 
initCache(UError_ਕੋਡ *ਸਥਿਤੀ) {
    umtx_initOnce(g_Shared_ਅੰਕੜੇ_Init_Once, &createCache, *ਸਥਿਤੀ);
}

ਸਥਿਰ UBool U_CALLCONV
ਲੱਦੋ_ਅੰਕੜੇ(UStringPrepProfile* profile, 
         ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, 
         ਅਚਲ ਅੱਖਰ* name, 
         ਅਚਲ ਅੱਖਰ* type, 
         UError_ਕੋਡ* error_ਕੋਡ) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, 0, 0, 0, 0};
    UData_ਸਮ੍ਰਿਤੀ *ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ;
    ਅਚਲ int32_t *p=ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ uint8_t *pb;
    UVersionInfo norm_Unicode_ਸੰਸਕਰਣ;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    ਜੇ(error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ=udata_openChoice(ਰਸਤਾ, type, name, isSPrepAcceptable, ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ);
    ਜੇ(U_FAILURE(*error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }

    p = static_cast<ਅਚਲ int32_t*>(udata_get_ਸਮ੍ਰਿਤੀ(ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ));
    pb = reinterpret_cast<ਅਚਲ uint8_t*>(p + SPREP_ਸੂਚਕਾਂਕ_ਸਿਖਰ);
    utrie_unserialize(&_sprepTrie, pb, p[SPREP_ਸੂਚਕਾਂਕ_TRIE_ਮਾਪ], error_ਕੋਡ);
    _sprepTrie.get_Folding_ਵਿੱਥ=get_SPrep_Folding_ਵਿੱਥ;


    ਜੇ(U_FAILURE(*error_ਕੋਡ)) {
        udata_close(ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ);
        ਵਾਪਸ ਝੂਠ;
    }

    /* in the mutex block, set the data for this process */
    umtx_ਤਾਲਾ(&usprepMutex);
    ਜੇ(profile->sprep_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        profile->sprep_ਅੰਕੜੇ=ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ;
        ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ=ਖਾਲੀ_ਸੂਚਕ;
        uprv_memcpy(&profile->indexes, p, ਆਕਾਰ(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, ਆਕਾਰ(UTrie));
    } ਨਹੀਂਤਾਂ {
        p = static_cast<ਅਚਲ int32_t*>(udata_get_ਸਮ੍ਰਿਤੀ(profile->sprep_ਅੰਕੜੇ));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mapping_ਅੰਕੜੇ = reinterpret_cast<ਅਚਲ uint16_t*>(reinterpret_cast<ਅਚਲ uint8_t*>(p + SPREP_ਸੂਚਕਾਂਕ_ਸਿਖਰ) + profile->indexes[SPREP_ਸੂਚਕਾਂਕ_TRIE_ਮਾਪ]);
    
    u_get_Unicode_ਸੰਸਕਰਣ(norm_Unicode_ਸੰਸਕਰਣ);
    normUniVer = (norm_Unicode_ਸੰਸਕਰਣ[0] << 24) + (norm_Unicode_ਸੰਸਕਰਣ[1] << 16) + 
                 (norm_Unicode_ਸੰਸਕਰਣ[2] << 8 ) + (norm_Unicode_ਸੰਸਕਰਣ[3]);
    sprepUniVer = (ਅੰਕੜੇ_ਸੰਸਕਰਣ[0] << 24) + (ਅੰਕੜੇ_ਸੰਸਕਰਣ[1] << 16) + 
                  (ਅੰਕੜੇ_ਸੰਸਕਰਣ[2] << 8 ) + (ਅੰਕੜੇ_ਸੰਸਕਰਣ[3]);
    normCorrVer = profile->indexes[SPREP_NORM_CORRECTNS_ਆਖਰੀ_UNI_ਸੰਸਕਰਣ];
    
    ਜੇ(U_FAILURE(*error_ਕੋਡ)){
        udata_close(ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ);
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *error_ਕੋਡ = U_INVALID_FORMAT_ERROR;
        udata_close(ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ);
        ਵਾਪਸ ਝੂਠ;
    }
    profile->is_ਅੰਕੜੇ_Loaded = ਸੱਚ;

    /* if a different thread set it first, then close the extra data */
    ਜੇ(ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ!=ਖਾਲੀ_ਸੂਚਕ) {
        udata_close(ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ); /* nullptr if it was set correctly */
    }


    ਵਾਪਸ profile->is_ਅੰਕੜੇ_Loaded;
}

ਸਥਿਰ UStringPrepProfile* 
usprep_getProfile(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, 
                  ਅਚਲ ਅੱਖਰ* name,
                  UError_ਕੋਡ *ਸਥਿਤੀ){

    UStringPrepProfile* profile = ਖਾਲੀ_ਸੂਚਕ;

    initCache(ਸਥਿਤੀ);

    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    UString_Prep_ਕੁੰਜੀ ਢੇਰ_ਕੁੰਜੀ;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    ਢੇਰ_ਕੁੰਜੀ.name = const_cast<ਅੱਖਰ*>(name);
    ਢੇਰ_ਕੁੰਜੀ.ਰਸਤਾ = const_cast<ਅੱਖਰ*>(ਰਸਤਾ);

    /* fetch the data from the cache */
    umtx_ਤਾਲਾ(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_ਅੰਕੜੇ_HASHTABLE, &ਢੇਰ_ਕੁੰਜੀ));
    ਜੇ(profile != ਖਾਲੀ_ਸੂਚਕ) {
        profile->ref_ਗਿਣਤੀ++;
    }
    umtx_unlock(&usprepMutex);
    
    ਜੇ(profile == ਖਾਲੀ_ਸੂਚਕ) {
        /* else load the data and put the data in the cache */
        Local_ਸਮ੍ਰਿਤੀ<UStringPrepProfile> newProfile;
        ਜੇ(newProfile.allocateInsteadAndReset() == ਖਾਲੀ_ਸੂਚਕ) {
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }

        /* load the data */
        ਜੇ(!ਲੱਦੋ_ਅੰਕੜੇ(newProfile.getAlias(), ਰਸਤਾ, name, SPREP_ਅੰਕੜੇ_TYPE, ਸਥਿਤੀ) || U_FAILURE(*ਸਥਿਤੀ) ){
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        Local_ਸਮ੍ਰਿਤੀ<UString_Prep_ਕੁੰਜੀ> ਕੁੰਜੀ;
        Local_ਸਮ੍ਰਿਤੀ<ਅੱਖਰ> ਕੁੰਜੀ_Name;
        Local_ਸਮ੍ਰਿਤੀ<ਅੱਖਰ> ਕੁੰਜੀ_ਰਸਤਾ;
        ਜੇ( ਕੁੰਜੀ.allocateInsteadAndReset() == ਖਾਲੀ_ਸੂਚਕ ||
            ਕੁੰਜੀ_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == ਖਾਲੀ_ਸੂਚਕ ||
            (ਰਸਤਾ != ਖਾਲੀ_ਸੂਚਕ &&
             ਕੁੰਜੀ_ਰਸਤਾ.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(ਰਸਤਾ)+1)) == ਖਾਲੀ_ਸੂਚਕ)
         ) {
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            usprep_unload(newProfile.getAlias());
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }

        umtx_ਤਾਲਾ(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_ਅੰਕੜੇ_HASHTABLE, &ਢੇਰ_ਕੁੰਜੀ));
        ਜੇ(profile != ਖਾਲੀ_ਸੂਚਕ) {
            profile->ref_ਗਿਣਤੀ++;
            usprep_unload(newProfile.getAlias());
        }
        ਨਹੀਂਤਾਂ {
            /* initialize the key members */
            ਕੁੰਜੀ->name = ਕੁੰਜੀ_Name.orphan();
            uprv_strcpy(ਕੁੰਜੀ->name, name);
            ਜੇ(ਰਸਤਾ != ਖਾਲੀ_ਸੂਚਕ){
                ਕੁੰਜੀ->ਰਸਤਾ = ਕੁੰਜੀ_ਰਸਤਾ.orphan();
                uprv_strcpy(ਕੁੰਜੀ->ਰਸਤਾ, ਰਸਤਾ);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_ਗਿਣਤੀ = 1;
            uhash_put(SHARED_ਅੰਕੜੇ_HASHTABLE, ਕੁੰਜੀ.orphan(), profile, ਸਥਿਤੀ);
        }
        umtx_unlock(&usprepMutex);
    }

    ਵਾਪਸ profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, 
            ਅਚਲ ਅੱਖਰ* name,
            UError_ਕੋਡ* ਸਥਿਤੀ){

    ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
       
    /* initialize the profile struct members */
    ਵਾਪਸ usprep_getProfile(ਰਸਤਾ,name,ਸਥਿਤੀ);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UError_ਕੋਡ* ਸਥਿਤੀ) {
    ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    int32_t ਸੂਚਕਾਂਕ = (int32_t)type;
    ਜੇ (ਸੂਚਕਾਂਕ < 0 || ਸੂਚਕਾਂਕ >= UPRV_LENGTHOF(PROFILE_ਨਾਂ)) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ usprep_open(ਖਾਲੀ_ਸੂਚਕ, PROFILE_ਨਾਂ[ਸੂਚਕਾਂਕ], ਸਥਿਤੀ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    ਜੇ(profile==ਖਾਲੀ_ਸੂਚਕ){
        ਵਾਪਸ;
    }

    umtx_ਤਾਲਾ(&usprepMutex);
    /* decrement the ref count*/
    ਜੇ(profile->ref_ਗਿਣਤੀ > 0){
        profile->ref_ਗਿਣਤੀ--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC ਖਾਲੀ 
uprv_syntaxError(ਅਚਲ char16_t* ਨਿਯਮਾਂ,
                 int32_t pos,
                 int32_t ਨਿਯਮਾਂ_Len,
                 UParseError* parseError){
    ਜੇ(parseError == ਖਾਲੀ_ਸੂਚਕ){
        ਵਾਪਸ;
    }
    parseError->ਵਿੱਥ = pos;
    parseError->ਸਤਰ = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t ਸ਼ੁਰੂ = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t ਹੱਦਬੰਦੀ = pos;
    
    u_memcpy(parseError->preContext,ਨਿਯਮਾਂ+ਸ਼ੁਰੂ,ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ);
    //null terminate the buffer
    parseError->preContext[ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ] = 0;
    
    // for post-context; include error rules[pos]
    ਸ਼ੁਰੂ = pos;
    ਹੱਦਬੰਦੀ = ਸ਼ੁਰੂ + (U_PARSE_CONTEXT_LEN-1);
    ਜੇ (ਹੱਦਬੰਦੀ > ਨਿਯਮਾਂ_Len) {
        ਹੱਦਬੰਦੀ = ਨਿਯਮਾਂ_Len;
    }
    ਜੇ (ਸ਼ੁਰੂ < ਨਿਯਮਾਂ_Len) {
        u_memcpy(parseError->postContext,ਨਿਯਮਾਂ+ਸ਼ੁਰੂ,ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ);
    }
    //null terminate the buffer
    parseError->postContext[ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ]= 0;
}


ਸਥਿਰ ਸਤਰਅੰਦਰ UStringPrepType
get_ਮੁੱਲਾਂ(uint16_t trie_ਸ਼ਬਦ, int16_t& value, UBool& is_ਸੂਚਕਾਂਕ){

    UStringPrepType type;
    ਜੇ(trie_ਸ਼ਬਦ == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_ਹੱਦਬੰਦੀ;
        is_ਸੂਚਕਾਂਕ =ਝੂਠ;
        value = 0;
    }ਨਹੀਂਤਾਂ ਜੇ(trie_ਸ਼ਬਦ >= SPREP_TYPE_ਹੱਦ){
        type = static_cast<UStringPrepType>(trie_ਸ਼ਬਦ - SPREP_TYPE_ਹੱਦ);
        is_ਸੂਚਕਾਂਕ =ਝੂਠ;
        value = 0;
    }ਨਹੀਂਤਾਂ{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        ਜੇ(trie_ਸ਼ਬਦ & 0x02){
            is_ਸੂਚਕਾਂਕ = ਸੱਚ;
            value = trie_ਸ਼ਬਦ  >> 2; //mask off the lower 2 bits and shift
        }ਨਹੀਂਤਾਂ{
            is_ਸੂਚਕਾਂਕ = ਝੂਠ;
            value = static_cast<int16_t>(trie_ਸ਼ਬਦ);
            value =  (value >> 2);
        }
 
        ਜੇ((trie_ਸ਼ਬਦ>>2) == SPREP_MAX_ਸੂਚਕਾਂਕ_ਮੁੱਲ){
            type = USPREP_DELETE;
            is_ਸੂਚਕਾਂਕ =ਝੂਠ;
            value = 0;
        }
    }
    ਵਾਪਸ type;
}

// TODO: change to writing to UnicodeString not char16_t *
ਸਥਿਰ int32_t 
usprep_map(  ਅਚਲ UStringPrepProfile* profile, 
             ਅਚਲ char16_t* src, int32_t src_ਲੰਮਾਈ,
             char16_t* dest, int32_t dest_ਸਮਰੱਥਾ,
             int32_t options,
             UParseError* parseError,
             UError_ਕੋਡ* ਸਥਿਤੀ ){
    
    uint16_t ਨਤੀਜਾ;
    int32_t dest_ਸੂਚਕਾਂਕ=0;
    int32_t src_ਸੂਚਕਾਂਕ;
    UBool allowUnassigned = static_cast<UBool>((options & USPREP_ALLOW_UNASSIGNED) > 0);
    UStringPrepType type;
    int16_t value;
    UBool is_ਸੂਚਕਾਂਕ;
    ਅਚਲ int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    ਲਈ(src_ਸੂਚਕਾਂਕ=0;src_ਸੂਚਕਾਂਕ<src_ਲੰਮਾਈ;){
        UChar32 ch;

        U16_ਅਗਲਾ(src,src_ਸੂਚਕਾਂਕ,src_ਲੰਮਾਈ,ch);
        
        ਨਤੀਜਾ=0;

        UTRIE_GET16(&profile->sprepTrie,ch,ਨਤੀਜਾ);
        
        type = get_ਮੁੱਲਾਂ(ਨਤੀਜਾ, value, is_ਸੂਚਕਾਂਕ);

        // check if the source codepoint is unassigned
        ਜੇ(type == USPREP_UNASSIGNED && allowUnassigned == ਝੂਠ){

            uprv_syntaxError(src,src_ਸੂਚਕਾਂਕ-U16_ਲੰਮਾਈ(ch), src_ਲੰਮਾਈ,parseError);
            *ਸਥਿਤੀ = U_STRINGPREP_UNASSIGNED_ERROR;
            ਵਾਪਸ 0;
            
        }ਨਹੀਂਤਾਂ ਜੇ(type == USPREP_MAP){
            
            int32_t ਸੂਚਕਾਂਕ, length;

            ਜੇ(is_ਸੂਚਕਾਂਕ){
                ਸੂਚਕਾਂਕ = value;
                ਜੇ(ਸੂਚਕਾਂਕ >= indexes[SPREP_ONE_UCHAR_MAPPING_ਸੂਚਕਾਂਕ_ਸ਼ੁਰੂ] &&
                         ਸੂਚਕਾਂਕ < indexes[SPREP_TWO_UCHARS_MAPPING_ਸੂਚਕਾਂਕ_ਸ਼ੁਰੂ]){
                    length = 1;
                }ਨਹੀਂਤਾਂ ਜੇ(ਸੂਚਕਾਂਕ >= indexes[SPREP_TWO_UCHARS_MAPPING_ਸੂਚਕਾਂਕ_ਸ਼ੁਰੂ] &&
                         ਸੂਚਕਾਂਕ < indexes[SPREP_THREE_UCHARS_MAPPING_ਸੂਚਕਾਂਕ_ਸ਼ੁਰੂ]){
                    length = 2;
                }ਨਹੀਂਤਾਂ ਜੇ(ਸੂਚਕਾਂਕ >= indexes[SPREP_THREE_UCHARS_MAPPING_ਸੂਚਕਾਂਕ_ਸ਼ੁਰੂ] &&
                         ਸੂਚਕਾਂਕ < indexes[SPREP_FOUR_UCHARS_MAPPING_ਸੂਚਕਾਂਕ_ਸ਼ੁਰੂ]){
                    length = 3;
                }ਨਹੀਂਤਾਂ{
                    length = profile->mapping_ਅੰਕੜੇ[ਸੂਚਕਾਂਕ++];
         
                }

                /* copy mapping to destination */
                ਲਈ(int32_t i=0; i< length; i++){
                    ਜੇ(dest_ਸੂਚਕਾਂਕ < dest_ਸਮਰੱਥਾ  ){
                        dest[dest_ਸੂਚਕਾਂਕ] = profile->mapping_ਅੰਕੜੇ[ਸੂਚਕਾਂਕ+i];
                    }
                    dest_ਸੂਚਕਾਂਕ++; /* for pre-flighting */
                }  
                ਜਾਰੀ;
            }ਨਹੀਂਤਾਂ{
                // subtract the delta to arrive at the code point
                ch -= value;
            }

        }ਨਹੀਂਤਾਂ ਜੇ(type==USPREP_DELETE){
             // just consume the codepoint and continue
            ਜਾਰੀ;
        }
        //copy the code point into destination
        ਜੇ(ch <= 0xFFFF){
            ਜੇ(dest_ਸੂਚਕਾਂਕ < dest_ਸਮਰੱਥਾ ){
                dest[dest_ਸੂਚਕਾਂਕ] = static_cast<char16_t>(ch);
            }
            dest_ਸੂਚਕਾਂਕ++;
        }ਨਹੀਂਤਾਂ{
            ਜੇ(dest_ਸੂਚਕਾਂਕ+1 < dest_ਸਮਰੱਥਾ ){
                dest[dest_ਸੂਚਕਾਂਕ]   = U16_LEAD(ch);
                dest[dest_ਸੂਚਕਾਂਕ+1] = U16_TRAIL(ch);
            }
            dest_ਸੂਚਕਾਂਕ +=2;
        }
       
    }
        
    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, dest_ਸੂਚਕਾਂਕ, ਸਥਿਤੀ);
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
usprep_prepare(   ਅਚਲ UStringPrepProfile* profile,
                  ਅਚਲ char16_t* src, int32_t src_ਲੰਮਾਈ,
                  char16_t* dest, int32_t dest_ਸਮਰੱਥਾ,
                  int32_t options,
                  UParseError* parseError,
                  UError_ਕੋਡ* ਸਥਿਤੀ ){

    // check error status
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ 0;
    }

    //check arguments
    ਜੇ(profile==ਖਾਲੀ_ਸੂਚਕ ||
            (src==ਖਾਲੀ_ਸੂਚਕ ? src_ਲੰਮਾਈ!=0 : src_ਲੰਮਾਈ<-1) ||
            (dest==ਖਾਲੀ_ਸੂਚਕ ? dest_ਸਮਰੱਥਾ!=0 : dest_ਸਮਰੱਥਾ<0)) {
        *ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    //get the string length
    ਜੇ(src_ਲੰਮਾਈ < 0){
        src_ਲੰਮਾਈ = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.get_ਬਫਰ(src_ਲੰਮਾਈ);
    ਜੇ(b1==ਖਾਲੀ_ਸੂਚਕ){
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ 0;
    }
    UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, src_ਲੰਮਾਈ,
                               b1, s1.get_ਸਮਰੱਥਾ(), options, parseError, &ਬਫਰ_ਸਥਿਤੀ);
    s1.release_ਬਫਰ(U_SUCCESS(ਬਫਰ_ਸਥਿਤੀ) ? b1Len : 0);

    ਜੇ(ਬਫਰ_ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.get_ਬਫਰ(b1Len);
        ਜੇ(b1==ਖਾਲੀ_ਸੂਚਕ){
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ 0;
        }

        ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, src_ਲੰਮਾਈ,
                           b1, s1.get_ਸਮਰੱਥਾ(), options, parseError, &ਬਫਰ_ਸਥਿਤੀ);
        s1.release_ਬਫਰ(U_SUCCESS(ਬਫਰ_ਸਥਿਤੀ) ? b1Len : 0);
    }
    ਜੇ(U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)){
        *ਸਥਿਤੀ = ਬਫਰ_ਸਥਿਤੀ;
        ਵਾਪਸ 0;
    }

    // normalize
    UnicodeString s2;
    ਜੇ(profile->doNFKC){
        ਅਚਲ Normalizer2 *n2 = Normalizer2::getNFKCInstance(*ਸਥਿਤੀ);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*ਸਥਿਤੀ));
        ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
            ਵਾਪਸ 0;
        }
        fn2.normalize(s1, s2, *ਸਥਿਤੀ);
    }ਨਹੀਂਤਾਂ{
        s2.fastCopyFrom(s1);
    }
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ 0;
    }

    // Prohibit and checkBiDi in one pass
    ਅਚਲ char16_t *b2 = s2.get_ਬਫਰ();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_ਗਿਣਤੀ, ਪਹਿਲਾ_Char_Dir=U_CHAR_DIRECTION_ਗਿਣਤੀ;
    UBool ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ=ਝੂਠ, ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ=ਝੂਠ;
    int32_t rtlPos =-1, ltrPos =-1;

    ਲਈ(int32_t b2_ਸੂਚਕਾਂਕ=0; b2_ਸੂਚਕਾਂਕ<b2Len;){
        UChar32 ch = 0;
        U16_ਅਗਲਾ(b2, b2_ਸੂਚਕਾਂਕ, b2Len, ch);

        uint16_t ਨਤੀਜਾ;
        UTRIE_GET16(&profile->sprepTrie,ch,ਨਤੀਜਾ);

        int16_t value;
        UBool is_ਸੂਚਕਾਂਕ;
        UStringPrepType type = get_ਮੁੱਲਾਂ(ਨਤੀਜਾ, value, is_ਸੂਚਕਾਂਕ);

        ਜੇ( type == USPREP_PROHIBITED || 
            ((ਨਤੀਜਾ < SPREP_TYPE_ਹੱਦ) && (ਨਤੀਜਾ & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *ਸਥਿਤੀ = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2_ਸੂਚਕਾਂਕ-U16_ਲੰਮਾਈ(ch), b2Len, parseError);
            ਵਾਪਸ 0;
        }

        ਜੇ(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            ਜੇ(ਪਹਿਲਾ_Char_Dir == U_CHAR_DIRECTION_ਗਿਣਤੀ){
                ਪਹਿਲਾ_Char_Dir = direction;
            }
            ਜੇ(direction == U_ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ){
                ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ = ਸੱਚ;
                ltrPos = b2_ਸੂਚਕਾਂਕ-1;
            }
            ਜੇ(direction == U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ || direction == U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ_ARABIC){
                ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ = ਸੱਚ;
                rtlPos = b2_ਸੂਚਕਾਂਕ-1;
            }
        }
    }
    ਜੇ(profile->checkBiDi){
        // satisfy 2
        ਜੇ( ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ && ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ){
            *ਸਥਿਤੀ = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            ਵਾਪਸ 0;
        }

        //satisfy 3
        ਜੇ( ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ && 
            !((ਪਹਿਲਾ_Char_Dir == U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ || ਪਹਿਲਾ_Char_Dir == U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ_ARABIC) &&
              (direction == U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ || direction == U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ_ARABIC))
           ){
            *ਸਥਿਤੀ = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ s2.extract(dest, dest_ਸਮਰੱਥਾ, *ਸਥਿਤੀ);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
            ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UDataInfo *pInfo;
    int32_t ਸਿਰਲੇਖ_ਮਾਪ;

    ਅਚਲ uint8_t *ਵਿੱਚ_ਬਾਈਟਾਂ;
    uint8_t *ਬਾਹਰਲਾ_ਬਾਈਟਾਂ;

    ਅਚਲ int32_t *ਵਿੱਚ_Indexes;
    int32_t indexes[16];

    int32_t i, ਵਿੱਥ, count, size;

    /* udata_swapDataHeader checks the arguments */
    ਸਿਰਲੇਖ_ਮਾਪ=udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* check data format and format version */
    pInfo=(ਅਚਲ UDataInfo *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+4);
    ਜੇ(!(
        pInfo->ਅੰਕੜੇ_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x50 &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x52 &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x50 &&
        pInfo->format_ਸੰਸਕਰਣ[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->ਅੰਕੜੇ_Format[0], pInfo->ਅੰਕੜੇ_Format[1],
                         pInfo->ਅੰਕੜੇ_Format[2], pInfo->ਅੰਕੜੇ_Format[3],
                         pInfo->format_ਸੰਸਕਰਣ[0]);
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_ਬਾਈਟਾਂ=(ਅਚਲ uint8_t *)ਵਿੱਚ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;
    ਬਾਹਰਲਾ_ਬਾਈਟਾਂ= (ਬਾਹਰਲਾ_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ ) ? ਖਾਲੀ_ਸੂਚਕ : (uint8_t *)ਬਾਹਰਲਾ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;

    ਵਿੱਚ_Indexes=(ਅਚਲ int32_t *)ਵਿੱਚ_ਬਾਈਟਾਂ;

    ਜੇ(length>=0) {
        length-=ਸਿਰਲੇਖ_ਮਾਪ;
        ਜੇ(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    ਲਈ(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, ਵਿੱਚ_Indexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[SPREP_ਸੂਚਕਾਂਕ_TRIE_ਮਾਪ]+
        indexes[SPREP_ਸੂਚਕਾਂਕ_MAPPING_ਅੰਕੜੇ_ਮਾਪ];

    ਜੇ(length>=0) {
        ਜੇ(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }

        /* copy the data for inaccessible bytes */
        ਜੇ(ਵਿੱਚ_ਬਾਈਟਾਂ!=ਬਾਹਰਲਾ_ਬਾਈਟਾਂ) {
            uprv_memcpy(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, ਵਿੱਚ_ਬਾਈਟਾਂ, size);
        }

        ਵਿੱਥ=0;

        /* swap the int32_t indexes[] */
        count=16*4;
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ, count, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, p_Error_ਕੋਡ);
        ਵਿੱਥ+=count;

        /* swap the UTrie */
        count=indexes[SPREP_ਸੂਚਕਾਂਕ_TRIE_ਮਾਪ];
        utrie_ਅਦਲਾ_ਬਦਲੀ(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, count, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);
        ਵਿੱਥ+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[SPREP_ਸੂਚਕਾਂਕ_MAPPING_ਅੰਕੜੇ_ਮਾਪ];
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, count, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);
        //offset+=count;
    }

    ਵਾਪਸ ਸਿਰਲੇਖ_ਮਾਪ+size;
}

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_IDNA */
