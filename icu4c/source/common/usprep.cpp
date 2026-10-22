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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_IDNA

#అన్తర్భావ "unicode/usprep.h"

#అన్తర్భావ "unicode/normalizer2.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/uversion.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "sprpimpl.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "udataswp.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "ubidi_props.h"
#అన్తర్భావ "uprops.h"

U_NAMESPACE_USE

U_CDECL_BEGIN

/*
Static cache for already opened StringPrep profiles
*/
స్థిర UHashtable *SHARED_దత్తాంశం_HASHTABLE = శూన్యసూచిక;
స్థిర icu::UInitOnce g_Shared_దత్తాంశం_Init_Once {};

స్థిర UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
స్థిర UVersionInfo దత్తాంశం_సంస్కరణ={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి PROFILE_పేర్లు[] = {
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

స్థిర UBool U_CALLCONV
isSPrepAcceptable(శూన్య * /* context */,
             స్థిరరాశి అక్షరం * /* type */, 
             స్థిరరాశి అక్షరం * /* name */,
             స్థిరరాశి UDataInfo *pInfo) {
    ఒకవేళ(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->దత్తాంశం_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->దత్తాంశం_Format[1]==0x50 &&
        pInfo->దత్తాంశం_Format[2]==0x52 &&
        pInfo->దత్తాంశం_Format[3]==0x50 &&
        pInfo->format_సంస్కరణ[0]==3 &&
        pInfo->format_సంస్కరణ[2]==UTRIE_జరుపు &&
        pInfo->format_సంస్కరణ[3]==UTRIE_సూచీ_జరుపు
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(దత్తాంశం_సంస్కరణ, pInfo->దత్తాంశం_సంస్కరణ, 4);
        తిరిగి నిజం;
    } లేకపోతే {
        తిరిగి అబద్ధం;
    }
}

స్థిర int32_t U_CALLCONV
get_SPrep_Folding_స్థానభ్రంశం(uint32_t దత్తాంశం) {

    తిరిగి (int32_t)దత్తాంశం;

}

/* hashes an entry  */
స్థిర int32_t U_CALLCONV 
hash_నమోదు(స్థిరరాశి UHashTok parm) {
    UString_Prep_కీలకం *b = (UString_Prep_కీలకం *)parm.సూచిక;
    UHashTok namekey, pathkey;
    namekey.సూచిక = b->name;
    pathkey.సూచిక = b->మార్గం;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    తిరిగి static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
స్థిర UBool U_CALLCONV 
compareEntries(స్థిరరాశి UHashTok p1, స్థిరరాశి UHashTok p2) {
    UString_Prep_కీలకం *b1 = (UString_Prep_కీలకం *)p1.సూచిక;
    UString_Prep_కీలకం *b2 = (UString_Prep_కీలకం *)p2.సూచిక;
    UHashTok name1, name2, path1, path2;
    name1.సూచిక = b1->name;
    name2.సూచిక = b2->name;
    path1.సూచిక = b1->మార్గం;
    path2.సూచిక = b2->మార్గం;
    తిరిగి uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

స్థిర శూన్య 
usprep_unload(UStringPrepProfile* దత్తాంశం){
    udata_close(దత్తాంశం->sprep_దత్తాంశం);
}

స్థిర int32_t 
usprep_అంతర్గత_flush_Cache(UBool no_Ref_లెక్క){
    UStringPrepProfile *profile = శూన్యసూచిక;
    UString_Prep_కీలకం  *కీలకం  = శూన్యసూచిక;
    int32_t pos = UHASH_మొదటి;
    int32_t deleted_అంకె = 0;
    స్థిరరాశి UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_తాళం(&usprepMutex);
    ఒకవేళ (SHARED_దత్తాంశం_HASHTABLE == శూన్యసూచిక) {
        umtx_unlock(&usprepMutex);
        తిరిగి 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    అయితే ((e = uhash_తదుపరి_Element(SHARED_దత్తాంశం_HASHTABLE, &pos)) != శూన్యసూచిక)
    {
        profile = (UStringPrepProfile *) e->value.సూచిక;
        కీలకం  = (UString_Prep_కీలకం *) e->కీలకం.సూచిక;

        ఒకవేళ ((no_Ref_లెక్క== అబద్ధం && profile->ref_లెక్క == 0) || 
             no_Ref_లెక్క) {
            deleted_అంకె++;
            uhash_removeElement(SHARED_దత్తాంశం_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            ఒకవేళ(కీలకం->name != శూన్యసూచిక) {
                uprv_free(కీలకం->name);
                కీలకం->name=శూన్యసూచిక;
            }
            ఒకవేళ(కీలకం->మార్గం != శూన్యసూచిక) {
                uprv_free(కీలకం->మార్గం);
                కీలకం->మార్గం=శూన్యసూచిక;
            }
            uprv_free(profile);
            uprv_free(కీలకం);
        }
       
    }
    umtx_unlock(&usprepMutex);

    తిరిగి deleted_అంకె;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

స్థిర UBool U_CALLCONV usprep_cleanup(){
    ఒకవేళ (SHARED_దత్తాంశం_HASHTABLE != శూన్యసూచిక) {
        usprep_అంతర్గత_flush_Cache(నిజం);
        ఒకవేళ (SHARED_దత్తాంశం_HASHTABLE != శూన్యసూచిక && uhash_లెక్క(SHARED_దత్తాంశం_HASHTABLE) == 0) {
            uhash_close(SHARED_దత్తాంశం_HASHTABLE);
            SHARED_దత్తాంశం_HASHTABLE = శూన్యసూచిక;
        }
    }
    g_Shared_దత్తాంశం_Init_Once.reset();
    తిరిగి (SHARED_దత్తాంశం_HASHTABLE == శూన్యసూచిక);
}
U_CDECL_చివర


/** Initializes the cache for resources */
స్థిర శూన్య U_CALLCONV
createCache(UError_సంకేతం &స్థితి_2) {
    SHARED_దత్తాంశం_HASHTABLE = uhash_open(hash_నమోదు, compareEntries, శూన్యసూచిక, &స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        SHARED_దత్తాంశం_HASHTABLE = శూన్యసూచిక;
    }
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

స్థిర శూన్య 
initCache(UError_సంకేతం *స్థితి_2) {
    umtx_initOnce(g_Shared_దత్తాంశం_Init_Once, &createCache, *స్థితి_2);
}

స్థిర UBool U_CALLCONV
ఎక్కించు_దత్తాంశం(UStringPrepProfile* profile, 
         స్థిరరాశి అక్షరం* మార్గం, 
         స్థిరరాశి అక్షరం* name, 
         స్థిరరాశి అక్షరం* type, 
         UError_సంకేతం* error_సంకేతం) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, 0, 0, 0, 0};
    UData_జ్ఞాపకం *దత్తాంశం_జ్ఞాపకం;
    స్థిరరాశి int32_t *p=శూన్యసూచిక;
    స్థిరరాశి uint8_t *pb;
    UVersionInfo norm_Unicode_సంస్కరణ;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    ఒకవేళ(error_సంకేతం==శూన్యసూచిక || U_FAILURE(*error_సంకేతం)) {
        తిరిగి 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    దత్తాంశం_జ్ఞాపకం=udata_openChoice(మార్గం, type, name, isSPrepAcceptable, శూన్యసూచిక, error_సంకేతం);
    ఒకవేళ(U_FAILURE(*error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }

    p = static_cast<స్థిరరాశి int32_t*>(udata_get_జ్ఞాపకం(దత్తాంశం_జ్ఞాపకం));
    pb = reinterpret_cast<స్థిరరాశి uint8_t*>(p + SPREP_సూచీ_శిఖరం);
    utrie_unserialize(&_sprepTrie, pb, p[SPREP_సూచీ_TRIE_కొలత], error_సంకేతం);
    _sprepTrie.get_Folding_స్థానభ్రంశం=get_SPrep_Folding_స్థానభ్రంశం;


    ఒకవేళ(U_FAILURE(*error_సంకేతం)) {
        udata_close(దత్తాంశం_జ్ఞాపకం);
        తిరిగి అబద్ధం;
    }

    /* in the mutex block, set the data for this process */
    umtx_తాళం(&usprepMutex);
    ఒకవేళ(profile->sprep_దత్తాంశం==శూన్యసూచిక) {
        profile->sprep_దత్తాంశం=దత్తాంశం_జ్ఞాపకం;
        దత్తాంశం_జ్ఞాపకం=శూన్యసూచిక;
        uprv_memcpy(&profile->indexes, p, పరిమాణం(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, పరిమాణం(UTrie));
    } లేకపోతే {
        p = static_cast<స్థిరరాశి int32_t*>(udata_get_జ్ఞాపకం(profile->sprep_దత్తాంశం));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mapping_దత్తాంశం = reinterpret_cast<స్థిరరాశి uint16_t*>(reinterpret_cast<స్థిరరాశి uint8_t*>(p + SPREP_సూచీ_శిఖరం) + profile->indexes[SPREP_సూచీ_TRIE_కొలత]);
    
    u_get_Unicode_సంస్కరణ(norm_Unicode_సంస్కరణ);
    normUniVer = (norm_Unicode_సంస్కరణ[0] << 24) + (norm_Unicode_సంస్కరణ[1] << 16) + 
                 (norm_Unicode_సంస్కరణ[2] << 8 ) + (norm_Unicode_సంస్కరణ[3]);
    sprepUniVer = (దత్తాంశం_సంస్కరణ[0] << 24) + (దత్తాంశం_సంస్కరణ[1] << 16) + 
                  (దత్తాంశం_సంస్కరణ[2] << 8 ) + (దత్తాంశం_సంస్కరణ[3]);
    normCorrVer = profile->indexes[SPREP_NORM_CORRECTNS_చివరి_UNI_సంస్కరణ];
    
    ఒకవేళ(U_FAILURE(*error_సంకేతం)){
        udata_close(దత్తాంశం_జ్ఞాపకం);
        తిరిగి అబద్ధం;
    }
    ఒకవేళ( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *error_సంకేతం = U_INVALID_FORMAT_ERROR;
        udata_close(దత్తాంశం_జ్ఞాపకం);
        తిరిగి అబద్ధం;
    }
    profile->is_దత్తాంశం_Loaded = నిజం;

    /* if a different thread set it first, then close the extra data */
    ఒకవేళ(దత్తాంశం_జ్ఞాపకం!=శూన్యసూచిక) {
        udata_close(దత్తాంశం_జ్ఞాపకం); /* nullptr if it was set correctly */
    }


    తిరిగి profile->is_దత్తాంశం_Loaded;
}

స్థిర UStringPrepProfile* 
usprep_getProfile(స్థిరరాశి అక్షరం* మార్గం, 
                  స్థిరరాశి అక్షరం* name,
                  UError_సంకేతం *స్థితి_2){

    UStringPrepProfile* profile = శూన్యసూచిక;

    initCache(స్థితి_2);

    ఒకవేళ(U_FAILURE(*స్థితి_2)){
        తిరిగి శూన్యసూచిక;
    }

    UString_Prep_కీలకం దొంతర_కీలకం;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    దొంతర_కీలకం.name = const_cast<అక్షరం*>(name);
    దొంతర_కీలకం.మార్గం = const_cast<అక్షరం*>(మార్గం);

    /* fetch the data from the cache */
    umtx_తాళం(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_దత్తాంశం_HASHTABLE, &దొంతర_కీలకం));
    ఒకవేళ(profile != శూన్యసూచిక) {
        profile->ref_లెక్క++;
    }
    umtx_unlock(&usprepMutex);
    
    ఒకవేళ(profile == శూన్యసూచిక) {
        /* else load the data and put the data in the cache */
        Local_జ్ఞాపకం<UStringPrepProfile> newProfile;
        ఒకవేళ(newProfile.allocateInsteadAndReset() == శూన్యసూచిక) {
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి శూన్యసూచిక;
        }

        /* load the data */
        ఒకవేళ(!ఎక్కించు_దత్తాంశం(newProfile.getAlias(), మార్గం, name, SPREP_దత్తాంశం_TYPE, స్థితి_2) || U_FAILURE(*స్థితి_2) ){
            తిరిగి శూన్యసూచిక;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        Local_జ్ఞాపకం<UString_Prep_కీలకం> కీలకం;
        Local_జ్ఞాపకం<అక్షరం> కీలకం_Name;
        Local_జ్ఞాపకం<అక్షరం> కీలకం_మార్గం;
        ఒకవేళ( కీలకం.allocateInsteadAndReset() == శూన్యసూచిక ||
            కీలకం_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == శూన్యసూచిక ||
            (మార్గం != శూన్యసూచిక &&
             కీలకం_మార్గం.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(మార్గం)+1)) == శూన్యసూచిక)
         ) {
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            usprep_unload(newProfile.getAlias());
            తిరిగి శూన్యసూచిక;
        }

        umtx_తాళం(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_దత్తాంశం_HASHTABLE, &దొంతర_కీలకం));
        ఒకవేళ(profile != శూన్యసూచిక) {
            profile->ref_లెక్క++;
            usprep_unload(newProfile.getAlias());
        }
        లేకపోతే {
            /* initialize the key members */
            కీలకం->name = కీలకం_Name.orphan();
            uprv_strcpy(కీలకం->name, name);
            ఒకవేళ(మార్గం != శూన్యసూచిక){
                కీలకం->మార్గం = కీలకం_మార్గం.orphan();
                uprv_strcpy(కీలకం->మార్గం, మార్గం);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_లెక్క = 1;
            uhash_put(SHARED_దత్తాంశం_HASHTABLE, కీలకం.orphan(), profile, స్థితి_2);
        }
        umtx_unlock(&usprepMutex);
    }

    తిరిగి profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(స్థిరరాశి అక్షరం* మార్గం, 
            స్థిరరాశి అక్షరం* name,
            UError_సంకేతం* స్థితి_2){

    ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి శూన్యసూచిక;
    }
       
    /* initialize the profile struct members */
    తిరిగి usprep_getProfile(మార్గం,name,స్థితి_2);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UError_సంకేతం* స్థితి_2) {
    ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి శూన్యసూచిక;
    }
    int32_t సూచీ = (int32_t)type;
    ఒకవేళ (సూచీ < 0 || సూచీ >= UPRV_LENGTHOF(PROFILE_పేర్లు)) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి usprep_open(శూన్యసూచిక, PROFILE_పేర్లు[సూచీ], స్థితి_2);
}

U_CAPI శూన్య U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    ఒకవేళ(profile==శూన్యసూచిక){
        తిరిగి;
    }

    umtx_తాళం(&usprepMutex);
    /* decrement the ref count*/
    ఒకవేళ(profile->ref_లెక్క > 0){
        profile->ref_లెక్క--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC శూన్య 
uprv_syntaxError(స్థిరరాశి char16_t* నియమాలు,
                 int32_t pos,
                 int32_t నియమాలు_Len,
                 UParseError* parseError){
    ఒకవేళ(parseError == శూన్యసూచిక){
        తిరిగి;
    }
    parseError->స్థానభ్రంశం = pos;
    parseError->పంక్తి = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t ప్రారంభం = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t హద్దు = pos;
    
    u_memcpy(parseError->preContext,నియమాలు+ప్రారంభం,హద్దు-ప్రారంభం);
    //null terminate the buffer
    parseError->preContext[హద్దు-ప్రారంభం] = 0;
    
    // for post-context; include error rules[pos]
    ప్రారంభం = pos;
    హద్దు = ప్రారంభం + (U_PARSE_CONTEXT_LEN-1);
    ఒకవేళ (హద్దు > నియమాలు_Len) {
        హద్దు = నియమాలు_Len;
    }
    ఒకవేళ (ప్రారంభం < నియమాలు_Len) {
        u_memcpy(parseError->postContext,నియమాలు+ప్రారంభం,హద్దు-ప్రారంభం);
    }
    //null terminate the buffer
    parseError->postContext[హద్దు-ప్రారంభం]= 0;
}


స్థిర అంతఃవరుస UStringPrepType
get_విలువలు(uint16_t trie_మాట, int16_t& value, UBool& is_సూచీ){

    UStringPrepType type;
    ఒకవేళ(trie_మాట == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_హద్దు;
        is_సూచీ =అబద్ధం;
        value = 0;
    }లేకపోతే ఒకవేళ(trie_మాట >= SPREP_TYPE_పరిమితి){
        type = static_cast<UStringPrepType>(trie_మాట - SPREP_TYPE_పరిమితి);
        is_సూచీ =అబద్ధం;
        value = 0;
    }లేకపోతే{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        ఒకవేళ(trie_మాట & 0x02){
            is_సూచీ = నిజం;
            value = trie_మాట  >> 2; //mask off the lower 2 bits and shift
        }లేకపోతే{
            is_సూచీ = అబద్ధం;
            value = static_cast<int16_t>(trie_మాట);
            value =  (value >> 2);
        }
 
        ఒకవేళ((trie_మాట>>2) == SPREP_MAX_సూచీ_విలువ){
            type = USPREP_DELETE;
            is_సూచీ =అబద్ధం;
            value = 0;
        }
    }
    తిరిగి type;
}

// TODO: change to writing to UnicodeString not char16_t *
స్థిర int32_t 
usprep_map(  స్థిరరాశి UStringPrepProfile* profile, 
             స్థిరరాశి char16_t* src, int32_t src_నిడివి,
             char16_t* dest, int32_t dest_సామర్థ్యం,
             int32_t options,
             UParseError* parseError,
             UError_సంకేతం* స్థితి_2 ){
    
    uint16_t ఫలితం;
    int32_t dest_సూచీ=0;
    int32_t src_సూచీ;
    UBool allowUnassigned = static_cast<UBool>((options & USPREP_ALLOW_UNASSIGNED) > 0);
    UStringPrepType type;
    int16_t value;
    UBool is_సూచీ;
    స్థిరరాశి int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    కొరకు(src_సూచీ=0;src_సూచీ<src_నిడివి;){
        UChar32 ch;

        U16_తదుపరి(src,src_సూచీ,src_నిడివి,ch);
        
        ఫలితం=0;

        UTRIE_GET16(&profile->sprepTrie,ch,ఫలితం);
        
        type = get_విలువలు(ఫలితం, value, is_సూచీ);

        // check if the source codepoint is unassigned
        ఒకవేళ(type == USPREP_UNASSIGNED && allowUnassigned == అబద్ధం){

            uprv_syntaxError(src,src_సూచీ-U16_నిడివి(ch), src_నిడివి,parseError);
            *స్థితి_2 = U_STRINGPREP_UNASSIGNED_ERROR;
            తిరిగి 0;
            
        }లేకపోతే ఒకవేళ(type == USPREP_MAP){
            
            int32_t సూచీ, length;

            ఒకవేళ(is_సూచీ){
                సూచీ = value;
                ఒకవేళ(సూచీ >= indexes[SPREP_ONE_UCHAR_MAPPING_సూచీ_ప్రారంభం] &&
                         సూచీ < indexes[SPREP_TWO_UCHARS_MAPPING_సూచీ_ప్రారంభం]){
                    length = 1;
                }లేకపోతే ఒకవేళ(సూచీ >= indexes[SPREP_TWO_UCHARS_MAPPING_సూచీ_ప్రారంభం] &&
                         సూచీ < indexes[SPREP_THREE_UCHARS_MAPPING_సూచీ_ప్రారంభం]){
                    length = 2;
                }లేకపోతే ఒకవేళ(సూచీ >= indexes[SPREP_THREE_UCHARS_MAPPING_సూచీ_ప్రారంభం] &&
                         సూచీ < indexes[SPREP_FOUR_UCHARS_MAPPING_సూచీ_ప్రారంభం]){
                    length = 3;
                }లేకపోతే{
                    length = profile->mapping_దత్తాంశం[సూచీ++];
         
                }

                /* copy mapping to destination */
                కొరకు(int32_t i=0; i< length; i++){
                    ఒకవేళ(dest_సూచీ < dest_సామర్థ్యం  ){
                        dest[dest_సూచీ] = profile->mapping_దత్తాంశం[సూచీ+i];
                    }
                    dest_సూచీ++; /* for pre-flighting */
                }  
                కొనసాగు;
            }లేకపోతే{
                // subtract the delta to arrive at the code point
                ch -= value;
            }

        }లేకపోతే ఒకవేళ(type==USPREP_DELETE){
             // just consume the codepoint and continue
            కొనసాగు;
        }
        //copy the code point into destination
        ఒకవేళ(ch <= 0xFFFF){
            ఒకవేళ(dest_సూచీ < dest_సామర్థ్యం ){
                dest[dest_సూచీ] = static_cast<char16_t>(ch);
            }
            dest_సూచీ++;
        }లేకపోతే{
            ఒకవేళ(dest_సూచీ+1 < dest_సామర్థ్యం ){
                dest[dest_సూచీ]   = U16_LEAD(ch);
                dest[dest_సూచీ+1] = U16_TRAIL(ch);
            }
            dest_సూచీ +=2;
        }
       
    }
        
    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, dest_సూచీ, స్థితి_2);
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
usprep_prepare(   స్థిరరాశి UStringPrepProfile* profile,
                  స్థిరరాశి char16_t* src, int32_t src_నిడివి,
                  char16_t* dest, int32_t dest_సామర్థ్యం,
                  int32_t options,
                  UParseError* parseError,
                  UError_సంకేతం* స్థితి_2 ){

    // check error status
    ఒకవేళ(U_FAILURE(*స్థితి_2)){
        తిరిగి 0;
    }

    //check arguments
    ఒకవేళ(profile==శూన్యసూచిక ||
            (src==శూన్యసూచిక ? src_నిడివి!=0 : src_నిడివి<-1) ||
            (dest==శూన్యసూచిక ? dest_సామర్థ్యం!=0 : dest_సామర్థ్యం<0)) {
        *స్థితి_2=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    //get the string length
    ఒకవేళ(src_నిడివి < 0){
        src_నిడివి = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.get_బఫరు(src_నిడివి);
    ఒకవేళ(b1==శూన్యసూచిక){
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి 0;
    }
    UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, src_నిడివి,
                               b1, s1.get_సామర్థ్యం(), options, parseError, &బఫరు_స్థితి);
    s1.release_బఫరు(U_SUCCESS(బఫరు_స్థితి) ? b1Len : 0);

    ఒకవేళ(బఫరు_స్థితి == U_బఫరు_అధికప్రవాహం_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.get_బఫరు(b1Len);
        ఒకవేళ(b1==శూన్యసూచిక){
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి 0;
        }

        బఫరు_స్థితి = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, src_నిడివి,
                           b1, s1.get_సామర్థ్యం(), options, parseError, &బఫరు_స్థితి);
        s1.release_బఫరు(U_SUCCESS(బఫరు_స్థితి) ? b1Len : 0);
    }
    ఒకవేళ(U_FAILURE(బఫరు_స్థితి)){
        *స్థితి_2 = బఫరు_స్థితి;
        తిరిగి 0;
    }

    // normalize
    UnicodeString s2;
    ఒకవేళ(profile->doNFKC){
        స్థిరరాశి Normalizer2 *n2 = Normalizer2::getNFKCInstance(*స్థితి_2);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*స్థితి_2));
        ఒకవేళ(U_FAILURE(*స్థితి_2)){
            తిరిగి 0;
        }
        fn2.normalize(s1, s2, *స్థితి_2);
    }లేకపోతే{
        s2.fastCopyFrom(s1);
    }
    ఒకవేళ(U_FAILURE(*స్థితి_2)){
        తిరిగి 0;
    }

    // Prohibit and checkBiDi in one pass
    స్థిరరాశి char16_t *b2 = s2.get_బఫరు();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_లెక్క, మొదటి_Char_Dir=U_CHAR_DIRECTION_లెక్క;
    UBool ఎడమ_గమ్యం_కుడి=అబద్ధం, కుడి_గమ్యం_ఎడమ=అబద్ధం;
    int32_t rtlPos =-1, ltrPos =-1;

    కొరకు(int32_t b2_సూచీ=0; b2_సూచీ<b2Len;){
        UChar32 ch = 0;
        U16_తదుపరి(b2, b2_సూచీ, b2Len, ch);

        uint16_t ఫలితం;
        UTRIE_GET16(&profile->sprepTrie,ch,ఫలితం);

        int16_t value;
        UBool is_సూచీ;
        UStringPrepType type = get_విలువలు(ఫలితం, value, is_సూచీ);

        ఒకవేళ( type == USPREP_PROHIBITED || 
            ((ఫలితం < SPREP_TYPE_పరిమితి) && (ఫలితం & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *స్థితి_2 = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2_సూచీ-U16_నిడివి(ch), b2Len, parseError);
            తిరిగి 0;
        }

        ఒకవేళ(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            ఒకవేళ(మొదటి_Char_Dir == U_CHAR_DIRECTION_లెక్క){
                మొదటి_Char_Dir = direction;
            }
            ఒకవేళ(direction == U_ఎడమ_గమ్యం_కుడి){
                ఎడమ_గమ్యం_కుడి = నిజం;
                ltrPos = b2_సూచీ-1;
            }
            ఒకవేళ(direction == U_కుడి_గమ్యం_ఎడమ || direction == U_కుడి_గమ్యం_ఎడమ_ARABIC){
                కుడి_గమ్యం_ఎడమ = నిజం;
                rtlPos = b2_సూచీ-1;
            }
        }
    }
    ఒకవేళ(profile->checkBiDi){
        // satisfy 2
        ఒకవేళ( ఎడమ_గమ్యం_కుడి && కుడి_గమ్యం_ఎడమ){
            *స్థితి_2 = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            తిరిగి 0;
        }

        //satisfy 3
        ఒకవేళ( కుడి_గమ్యం_ఎడమ && 
            !((మొదటి_Char_Dir == U_కుడి_గమ్యం_ఎడమ || మొదటి_Char_Dir == U_కుడి_గమ్యం_ఎడమ_ARABIC) &&
              (direction == U_కుడి_గమ్యం_ఎడమ || direction == U_కుడి_గమ్యం_ఎడమ_ARABIC))
           ){
            *స్థితి_2 = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి s2.extract(dest, dest_సామర్థ్యం, *స్థితి_2);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
            స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
            UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UDataInfo *pInfo;
    int32_t శీర్షిక_కొలత;

    స్థిరరాశి uint8_t *లోపల_బైట్లు;
    uint8_t *బహిర్గతం_బైట్లు;

    స్థిరరాశి int32_t *లోపల_Indexes;
    int32_t indexes[16];

    int32_t i, స్థానభ్రంశం, count, size;

    /* udata_swapDataHeader checks the arguments */
    శీర్షిక_కొలత=udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* check data format and format version */
    pInfo=(స్థిరరాశి UDataInfo *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+4);
    ఒకవేళ(!(
        pInfo->దత్తాంశం_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->దత్తాంశం_Format[1]==0x50 &&
        pInfo->దత్తాంశం_Format[2]==0x52 &&
        pInfo->దత్తాంశం_Format[3]==0x50 &&
        pInfo->format_సంస్కరణ[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->దత్తాంశం_Format[0], pInfo->దత్తాంశం_Format[1],
                         pInfo->దత్తాంశం_Format[2], pInfo->దత్తాంశం_Format[3],
                         pInfo->format_సంస్కరణ[0]);
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    లోపల_బైట్లు=(స్థిరరాశి uint8_t *)లోపల_దత్తాంశం+శీర్షిక_కొలత;
    బహిర్గతం_బైట్లు= (బహిర్గతం_దత్తాంశం == శూన్యసూచిక ) ? శూన్యసూచిక : (uint8_t *)బహిర్గతం_దత్తాంశం+శీర్షిక_కొలత;

    లోపల_Indexes=(స్థిరరాశి int32_t *)లోపల_బైట్లు;

    ఒకవేళ(length>=0) {
        length-=శీర్షిక_కొలత;
        ఒకవేళ(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    కొరకు(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, లోపల_Indexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[SPREP_సూచీ_TRIE_కొలత]+
        indexes[SPREP_సూచీ_MAPPING_దత్తాంశం_కొలత];

    ఒకవేళ(length>=0) {
        ఒకవేళ(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }

        /* copy the data for inaccessible bytes */
        ఒకవేళ(లోపల_బైట్లు!=బహిర్గతం_బైట్లు) {
            uprv_memcpy(బహిర్గతం_బైట్లు, లోపల_బైట్లు, size);
        }

        స్థానభ్రంశం=0;

        /* swap the int32_t indexes[] */
        count=16*4;
        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు, count, బహిర్గతం_బైట్లు, p_Error_సంకేతం);
        స్థానభ్రంశం+=count;

        /* swap the UTrie */
        count=indexes[SPREP_సూచీ_TRIE_కొలత];
        utrie_మార్పిడి(ds, లోపల_బైట్లు+స్థానభ్రంశం, count, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);
        స్థానభ్రంశం+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[SPREP_సూచీ_MAPPING_దత్తాంశం_కొలత];
        ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+స్థానభ్రంశం, count, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);
        //offset+=count;
    }

    తిరిగి శీర్షిక_కొలత+size;
}

#అయితే_ముగింపు /* #if !UCONFIG_NO_IDNA */
