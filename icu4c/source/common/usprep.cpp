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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_IDNA

#अन्तर्भाव "unicode/usprep.h"

#अन्तर्भाव "unicode/normalizer2.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/uversion.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "sprpimpl.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "ubidi_props.h"
#अन्तर्भाव "uprops.h"

U_NAMESPACE_USE

U_CDECL_BEGIN

/*
Static cache for already opened StringPrep profiles
*/
स्थिर UHashtable *SHARED_दत्त_HASHTABLE = शून्यसूचकः;
स्थिर icu::UInitOnce g_Shared_दत्त_Init_Once {};

स्थिर UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
स्थिर UVersionInfo दत्त_संस्करणम्={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
स्थिर ध्रुव वर्ण * ध्रुव PROFILE_नामानि[] = {
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

स्थिर UBool U_CALLCONV
isSPrepAcceptable(शून्य * /* context */,
             ध्रुव वर्ण * /* type */, 
             ध्रुव वर्ण * /* name */,
             ध्रुव UDataInfo *pInfo) {
    यदि(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->दत्त_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->दत्त_Format[1]==0x50 &&
        pInfo->दत्त_Format[2]==0x52 &&
        pInfo->दत्त_Format[3]==0x50 &&
        pInfo->format_संस्करणम्[0]==3 &&
        pInfo->format_संस्करणम्[2]==UTRIE_सरणम् &&
        pInfo->format_संस्करणम्[3]==UTRIE_सूचकांक_सरणम्
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(दत्त_संस्करणम्, pInfo->दत्त_संस्करणम्, 4);
        प्रत्यागम सत्य;
    } अन्यथा {
        प्रत्यागम असत्य;
    }
}

स्थिर int32_t U_CALLCONV
get_SPrep_Folding_विस्थापनम्(uint32_t दत्त) {

    प्रत्यागम (int32_t)दत्त;

}

/* hashes an entry  */
स्थिर int32_t U_CALLCONV 
hash_प्रविष्टिः(ध्रुव UHashTok parm) {
    UString_Prep_कुञ्चिका *b = (UString_Prep_कुञ्चिका *)parm.pointer;
    UHashTok namekey, pathkey;
    namekey.pointer = b->name;
    pathkey.pointer = b->मार्गः;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    प्रत्यागम static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
स्थिर UBool U_CALLCONV 
compareEntries(ध्रुव UHashTok p1, ध्रुव UHashTok p2) {
    UString_Prep_कुञ्चिका *b1 = (UString_Prep_कुञ्चिका *)p1.pointer;
    UString_Prep_कुञ्चिका *b2 = (UString_Prep_कुञ्चिका *)p2.pointer;
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->name;
    name2.pointer = b2->name;
    path1.pointer = b1->मार्गः;
    path2.pointer = b2->मार्गः;
    प्रत्यागम uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

स्थिर शून्य 
usprep_unload(UStringPrepProfile* दत्त){
    udata_close(दत्त->sprep_दत्त);
}

स्थिर int32_t 
usprep_आन्तरिकम्_flush_Cache(UBool no_Ref_गणनम्){
    UStringPrepProfile *profile = शून्यसूचकः;
    UString_Prep_कुञ्चिका  *कुञ्चिका  = शून्यसूचकः;
    int32_t pos = UHASH_FIRST;
    int32_t deleted_अङ्कः = 0;
    ध्रुव UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_तालकम्(&usprepMutex);
    यदि (SHARED_दत्त_HASHTABLE == शून्यसूचकः) {
        umtx_unlock(&usprepMutex);
        प्रत्यागम 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    यावत् ((e = uhash_अग्र_Element(SHARED_दत्त_HASHTABLE, &pos)) != शून्यसूचकः)
    {
        profile = (UStringPrepProfile *) e->value.pointer;
        कुञ्चिका  = (UString_Prep_कुञ्चिका *) e->कुञ्चिका.pointer;

        यदि ((no_Ref_गणनम्== असत्य && profile->ref_गणनम् == 0) || 
             no_Ref_गणनम्) {
            deleted_अङ्कः++;
            uhash_removeElement(SHARED_दत्त_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            यदि(कुञ्चिका->name != शून्यसूचकः) {
                uprv_free(कुञ्चिका->name);
                कुञ्चिका->name=शून्यसूचकः;
            }
            यदि(कुञ्चिका->मार्गः != शून्यसूचकः) {
                uprv_free(कुञ्चिका->मार्गः);
                कुञ्चिका->मार्गः=शून्यसूचकः;
            }
            uprv_free(profile);
            uprv_free(कुञ्चिका);
        }
       
    }
    umtx_unlock(&usprepMutex);

    प्रत्यागम deleted_अङ्कः;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

स्थिर UBool U_CALLCONV usprep_cleanup(){
    यदि (SHARED_दत्त_HASHTABLE != शून्यसूचकः) {
        usprep_आन्तरिकम्_flush_Cache(सत्य);
        यदि (SHARED_दत्त_HASHTABLE != शून्यसूचकः && uhash_गणनम्(SHARED_दत्त_HASHTABLE) == 0) {
            uhash_close(SHARED_दत्त_HASHTABLE);
            SHARED_दत्त_HASHTABLE = शून्यसूचकः;
        }
    }
    g_Shared_दत्त_Init_Once.reset();
    प्रत्यागम (SHARED_दत्त_HASHTABLE == शून्यसूचकः);
}
U_CDECL_अन्तः


/** Initializes the cache for resources */
स्थिर शून्य U_CALLCONV
createCache(UError_संहिता &स्थितिः) {
    SHARED_दत्त_HASHTABLE = uhash_open(hash_प्रविष्टिः, compareEntries, शून्यसूचकः, &स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        SHARED_दत्त_HASHTABLE = शून्यसूचकः;
    }
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

स्थिर शून्य 
initCache(UError_संहिता *स्थितिः) {
    umtx_initOnce(g_Shared_दत्त_Init_Once, &createCache, *स्थितिः);
}

स्थिर UBool U_CALLCONV
आरोपय_दत्त(UStringPrepProfile* profile, 
         ध्रुव वर्ण* मार्गः, 
         ध्रुव वर्ण* name, 
         ध्रुव वर्ण* type, 
         UError_संहिता* error_संहिता) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, 0, 0, 0, 0};
    UData_स्मृतिः *दत्त_स्मृतिः;
    ध्रुव int32_t *p=शून्यसूचकः;
    ध्रुव uint8_t *pb;
    UVersionInfo norm_Unicode_संस्करणम्;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    यदि(error_संहिता==शून्यसूचकः || U_FAILURE(*error_संहिता)) {
        प्रत्यागम 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    दत्त_स्मृतिः=udata_openChoice(मार्गः, type, name, isSPrepAcceptable, शून्यसूचकः, error_संहिता);
    यदि(U_FAILURE(*error_संहिता)) {
        प्रत्यागम असत्य;
    }

    p = static_cast<ध्रुव int32_t*>(udata_get_स्मृतिः(दत्त_स्मृतिः));
    pb = reinterpret_cast<ध्रुव uint8_t*>(p + SPREP_सूचकांक_शिखरम्);
    utrie_unserialize(&_sprepTrie, pb, p[SPREP_सूचकांक_TRIE_आकार], error_संहिता);
    _sprepTrie.get_Folding_विस्थापनम्=get_SPrep_Folding_विस्थापनम्;


    यदि(U_FAILURE(*error_संहिता)) {
        udata_close(दत्त_स्मृतिः);
        प्रत्यागम असत्य;
    }

    /* in the mutex block, set the data for this process */
    umtx_तालकम्(&usprepMutex);
    यदि(profile->sprep_दत्त==शून्यसूचकः) {
        profile->sprep_दत्त=दत्त_स्मृतिः;
        दत्त_स्मृतिः=शून्यसूचकः;
        uprv_memcpy(&profile->indexes, p, परिमाण(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, परिमाण(UTrie));
    } अन्यथा {
        p = static_cast<ध्रुव int32_t*>(udata_get_स्मृतिः(profile->sprep_दत्त));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mapping_दत्त = reinterpret_cast<ध्रुव uint16_t*>(reinterpret_cast<ध्रुव uint8_t*>(p + SPREP_सूचकांक_शिखरम्) + profile->indexes[SPREP_सूचकांक_TRIE_आकार]);
    
    u_get_Unicode_संस्करणम्(norm_Unicode_संस्करणम्);
    normUniVer = (norm_Unicode_संस्करणम्[0] << 24) + (norm_Unicode_संस्करणम्[1] << 16) + 
                 (norm_Unicode_संस्करणम्[2] << 8 ) + (norm_Unicode_संस्करणम्[3]);
    sprepUniVer = (दत्त_संस्करणम्[0] << 24) + (दत्त_संस्करणम्[1] << 16) + 
                  (दत्त_संस्करणम्[2] << 8 ) + (दत्त_संस्करणम्[3]);
    normCorrVer = profile->indexes[SPREP_NORM_CORRECTNS_चरमः_UNI_संस्करणम्];
    
    यदि(U_FAILURE(*error_संहिता)){
        udata_close(दत्त_स्मृतिः);
        प्रत्यागम असत्य;
    }
    यदि( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *error_संहिता = U_INVALID_FORMAT_ERROR;
        udata_close(दत्त_स्मृतिः);
        प्रत्यागम असत्य;
    }
    profile->is_दत्त_Loaded = सत्य;

    /* if a different thread set it first, then close the extra data */
    यदि(दत्त_स्मृतिः!=शून्यसूचकः) {
        udata_close(दत्त_स्मृतिः); /* nullptr if it was set correctly */
    }


    प्रत्यागम profile->is_दत्त_Loaded;
}

स्थिर UStringPrepProfile* 
usprep_getProfile(ध्रुव वर्ण* मार्गः, 
                  ध्रुव वर्ण* name,
                  UError_संहिता *स्थितिः){

    UStringPrepProfile* profile = शून्यसूचकः;

    initCache(स्थितिः);

    यदि(U_FAILURE(*स्थितिः)){
        प्रत्यागम शून्यसूचकः;
    }

    UString_Prep_कुञ्चिका राशिः_कुञ्चिका;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    राशिः_कुञ्चिका.name = const_cast<वर्ण*>(name);
    राशिः_कुञ्चिका.मार्गः = const_cast<वर्ण*>(मार्गः);

    /* fetch the data from the cache */
    umtx_तालकम्(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_दत्त_HASHTABLE, &राशिः_कुञ्चिका));
    यदि(profile != शून्यसूचकः) {
        profile->ref_गणनम्++;
    }
    umtx_unlock(&usprepMutex);
    
    यदि(profile == शून्यसूचकः) {
        /* else load the data and put the data in the cache */
        Local_स्मृतिः<UStringPrepProfile> newProfile;
        यदि(newProfile.allocateInsteadAndReset() == शून्यसूचकः) {
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम शून्यसूचकः;
        }

        /* load the data */
        यदि(!आरोपय_दत्त(newProfile.getAlias(), मार्गः, name, SPREP_दत्त_TYPE, स्थितिः) || U_FAILURE(*स्थितिः) ){
            प्रत्यागम शून्यसूचकः;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        Local_स्मृतिः<UString_Prep_कुञ्चिका> कुञ्चिका;
        Local_स्मृतिः<वर्ण> कुञ्चिका_Name;
        Local_स्मृतिः<वर्ण> कुञ्चिका_मार्गः;
        यदि( कुञ्चिका.allocateInsteadAndReset() == शून्यसूचकः ||
            कुञ्चिका_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == शून्यसूचकः ||
            (मार्गः != शून्यसूचकः &&
             कुञ्चिका_मार्गः.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(मार्गः)+1)) == शून्यसूचकः)
         ) {
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            usprep_unload(newProfile.getAlias());
            प्रत्यागम शून्यसूचकः;
        }

        umtx_तालकम्(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_दत्त_HASHTABLE, &राशिः_कुञ्चिका));
        यदि(profile != शून्यसूचकः) {
            profile->ref_गणनम्++;
            usprep_unload(newProfile.getAlias());
        }
        अन्यथा {
            /* initialize the key members */
            कुञ्चिका->name = कुञ्चिका_Name.orphan();
            uprv_strcpy(कुञ्चिका->name, name);
            यदि(मार्गः != शून्यसूचकः){
                कुञ्चिका->मार्गः = कुञ्चिका_मार्गः.orphan();
                uprv_strcpy(कुञ्चिका->मार्गः, मार्गः);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_गणनम् = 1;
            uhash_put(SHARED_दत्त_HASHTABLE, कुञ्चिका.orphan(), profile, स्थितिः);
        }
        umtx_unlock(&usprepMutex);
    }

    प्रत्यागम profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(ध्रुव वर्ण* मार्गः, 
            ध्रुव वर्ण* name,
            UError_संहिता* स्थितिः){

    यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम शून्यसूचकः;
    }
       
    /* initialize the profile struct members */
    प्रत्यागम usprep_getProfile(मार्गः,name,स्थितिः);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UError_संहिता* स्थितिः) {
    यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम शून्यसूचकः;
    }
    int32_t सूचकांक = (int32_t)type;
    यदि (सूचकांक < 0 || सूचकांक >= UPRV_LENGTHOF(PROFILE_नामानि)) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम usprep_open(शून्यसूचकः, PROFILE_नामानि[सूचकांक], स्थितिः);
}

U_CAPI शून्य U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    यदि(profile==शून्यसूचकः){
        प्रत्यागम;
    }

    umtx_तालकम्(&usprepMutex);
    /* decrement the ref count*/
    यदि(profile->ref_गणनम् > 0){
        profile->ref_गणनम्--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC शून्य 
uprv_syntaxError(ध्रुव char16_t* नियमाः,
                 int32_t pos,
                 int32_t नियमाः_Len,
                 UParseError* parseError){
    यदि(parseError == शून्यसूचकः){
        प्रत्यागम;
    }
    parseError->विस्थापनम् = pos;
    parseError->पङ्क्तिः = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t start = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t सीमा = pos;
    
    u_memcpy(parseError->preContext,नियमाः+start,सीमा-start);
    //null terminate the buffer
    parseError->preContext[सीमा-start] = 0;
    
    // for post-context; include error rules[pos]
    start = pos;
    सीमा = start + (U_PARSE_CONTEXT_LEN-1);
    यदि (सीमा > नियमाः_Len) {
        सीमा = नियमाः_Len;
    }
    यदि (start < नियमाः_Len) {
        u_memcpy(parseError->postContext,नियमाः+start,सीमा-start);
    }
    //null terminate the buffer
    parseError->postContext[सीमा-start]= 0;
}


स्थिर अन्तःस्थ UStringPrepType
get_मानानि(uint16_t trieWord, int16_t& value, UBool& is_सूचकांक){

    UStringPrepType type;
    यदि(trieWord == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_सीमा;
        is_सूचकांक =असत्य;
        value = 0;
    }अन्यथा यदि(trieWord >= SPREP_TYPE_देहली){
        type = static_cast<UStringPrepType>(trieWord - SPREP_TYPE_देहली);
        is_सूचकांक =असत्य;
        value = 0;
    }अन्यथा{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        यदि(trieWord & 0x02){
            is_सूचकांक = सत्य;
            value = trieWord  >> 2; //mask off the lower 2 bits and shift
        }अन्यथा{
            is_सूचकांक = असत्य;
            value = static_cast<int16_t>(trieWord);
            value =  (value >> 2);
        }
 
        यदि((trieWord>>2) == SPREP_MAX_सूचकांक_मानम्){
            type = USPREP_DELETE;
            is_सूचकांक =असत्य;
            value = 0;
        }
    }
    प्रत्यागम type;
}

// TODO: change to writing to UnicodeString not char16_t *
स्थिर int32_t 
usprep_map(  ध्रुव UStringPrepProfile* profile, 
             ध्रुव char16_t* src, int32_t src_दीर्घता,
             char16_t* dest, int32_t dest_क्षमता,
             int32_t options,
             UParseError* parseError,
             UError_संहिता* स्थितिः ){
    
    uint16_t परिणाम;
    int32_t dest_सूचकांक=0;
    int32_t src_सूचकांक;
    UBool allowUnassigned = static_cast<UBool>((options & USPREP_ALLOW_UNASSIGNED) > 0);
    UStringPrepType type;
    int16_t value;
    UBool is_सूचकांक;
    ध्रुव int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    प्रति(src_सूचकांक=0;src_सूचकांक<src_दीर्घता;){
        UChar32 ch;

        U16_अग्र(src,src_सूचकांक,src_दीर्घता,ch);
        
        परिणाम=0;

        UTRIE_GET16(&profile->sprepTrie,ch,परिणाम);
        
        type = get_मानानि(परिणाम, value, is_सूचकांक);

        // check if the source codepoint is unassigned
        यदि(type == USPREP_UNASSIGNED && allowUnassigned == असत्य){

            uprv_syntaxError(src,src_सूचकांक-U16_दीर्घता(ch), src_दीर्घता,parseError);
            *स्थितिः = U_STRINGPREP_UNASSIGNED_ERROR;
            प्रत्यागम 0;
            
        }अन्यथा यदि(type == USPREP_MAP){
            
            int32_t सूचकांक, length;

            यदि(is_सूचकांक){
                सूचकांक = value;
                यदि(सूचकांक >= indexes[SPREP_ONE_UCHAR_MAPPING_सूचकांक_START] &&
                         सूचकांक < indexes[SPREP_TWO_UCHARS_MAPPING_सूचकांक_START]){
                    length = 1;
                }अन्यथा यदि(सूचकांक >= indexes[SPREP_TWO_UCHARS_MAPPING_सूचकांक_START] &&
                         सूचकांक < indexes[SPREP_THREE_UCHARS_MAPPING_सूचकांक_START]){
                    length = 2;
                }अन्यथा यदि(सूचकांक >= indexes[SPREP_THREE_UCHARS_MAPPING_सूचकांक_START] &&
                         सूचकांक < indexes[SPREP_FOUR_UCHARS_MAPPING_सूचकांक_START]){
                    length = 3;
                }अन्यथा{
                    length = profile->mapping_दत्त[सूचकांक++];
         
                }

                /* copy mapping to destination */
                प्रति(int32_t i=0; i< length; i++){
                    यदि(dest_सूचकांक < dest_क्षमता  ){
                        dest[dest_सूचकांक] = profile->mapping_दत्त[सूचकांक+i];
                    }
                    dest_सूचकांक++; /* for pre-flighting */
                }  
                अनुवर्त;
            }अन्यथा{
                // subtract the delta to arrive at the code point
                ch -= value;
            }

        }अन्यथा यदि(type==USPREP_DELETE){
             // just consume the codepoint and continue
            अनुवर्त;
        }
        //copy the code point into destination
        यदि(ch <= 0xFFFF){
            यदि(dest_सूचकांक < dest_क्षमता ){
                dest[dest_सूचकांक] = static_cast<char16_t>(ch);
            }
            dest_सूचकांक++;
        }अन्यथा{
            यदि(dest_सूचकांक+1 < dest_क्षमता ){
                dest[dest_सूचकांक]   = U16_LEAD(ch);
                dest[dest_सूचकांक+1] = U16_TRAIL(ch);
            }
            dest_सूचकांक +=2;
        }
       
    }
        
    प्रत्यागम u_terminateUChars(dest, dest_क्षमता, dest_सूचकांक, स्थितिः);
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
usprep_prepare(   ध्रुव UStringPrepProfile* profile,
                  ध्रुव char16_t* src, int32_t src_दीर्घता,
                  char16_t* dest, int32_t dest_क्षमता,
                  int32_t options,
                  UParseError* parseError,
                  UError_संहिता* स्थितिः ){

    // check error status
    यदि(U_FAILURE(*स्थितिः)){
        प्रत्यागम 0;
    }

    //check arguments
    यदि(profile==शून्यसूचकः ||
            (src==शून्यसूचकः ? src_दीर्घता!=0 : src_दीर्घता<-1) ||
            (dest==शून्यसूचकः ? dest_क्षमता!=0 : dest_क्षमता<0)) {
        *स्थितिः=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    //get the string length
    यदि(src_दीर्घता < 0){
        src_दीर्घता = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.get_बफर(src_दीर्घता);
    यदि(b1==शून्यसूचकः){
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम 0;
    }
    UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, src_दीर्घता,
                               b1, s1.get_क्षमता(), options, parseError, &बफर_स्थितिः);
    s1.release_बफर(U_SUCCESS(बफर_स्थितिः) ? b1Len : 0);

    यदि(बफर_स्थितिः == U_बफर_अतिप्रवाहः_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.get_बफर(b1Len);
        यदि(b1==शून्यसूचकः){
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम 0;
        }

        बफर_स्थितिः = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, src_दीर्घता,
                           b1, s1.get_क्षमता(), options, parseError, &बफर_स्थितिः);
        s1.release_बफर(U_SUCCESS(बफर_स्थितिः) ? b1Len : 0);
    }
    यदि(U_FAILURE(बफर_स्थितिः)){
        *स्थितिः = बफर_स्थितिः;
        प्रत्यागम 0;
    }

    // normalize
    UnicodeString s2;
    यदि(profile->doNFKC){
        ध्रुव Normalizer2 *n2 = Normalizer2::getNFKCInstance(*स्थितिः);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*स्थितिः));
        यदि(U_FAILURE(*स्थितिः)){
            प्रत्यागम 0;
        }
        fn2.normalize(s1, s2, *स्थितिः);
    }अन्यथा{
        s2.fastCopyFrom(s1);
    }
    यदि(U_FAILURE(*स्थितिः)){
        प्रत्यागम 0;
    }

    // Prohibit and checkBiDi in one pass
    ध्रुव char16_t *b2 = s2.get_बफर();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_गणनम्, firstCharDir=U_CHAR_DIRECTION_गणनम्;
    UBool वामः_गन्तव्यम्_दक्षिणः=असत्य, दक्षिणः_गन्तव्यम्_वामः=असत्य;
    int32_t rtlPos =-1, ltrPos =-1;

    प्रति(int32_t b2_सूचकांक=0; b2_सूचकांक<b2Len;){
        UChar32 ch = 0;
        U16_अग्र(b2, b2_सूचकांक, b2Len, ch);

        uint16_t परिणाम;
        UTRIE_GET16(&profile->sprepTrie,ch,परिणाम);

        int16_t value;
        UBool is_सूचकांक;
        UStringPrepType type = get_मानानि(परिणाम, value, is_सूचकांक);

        यदि( type == USPREP_PROHIBITED || 
            ((परिणाम < SPREP_TYPE_देहली) && (परिणाम & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *स्थितिः = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2_सूचकांक-U16_दीर्घता(ch), b2Len, parseError);
            प्रत्यागम 0;
        }

        यदि(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            यदि(firstCharDir == U_CHAR_DIRECTION_गणनम्){
                firstCharDir = direction;
            }
            यदि(direction == U_वामः_गन्तव्यम्_दक्षिणः){
                वामः_गन्तव्यम्_दक्षिणः = सत्य;
                ltrPos = b2_सूचकांक-1;
            }
            यदि(direction == U_दक्षिणः_गन्तव्यम्_वामः || direction == U_दक्षिणः_गन्तव्यम्_वामः_ARABIC){
                दक्षिणः_गन्तव्यम्_वामः = सत्य;
                rtlPos = b2_सूचकांक-1;
            }
        }
    }
    यदि(profile->checkBiDi){
        // satisfy 2
        यदि( वामः_गन्तव्यम्_दक्षिणः && दक्षिणः_गन्तव्यम्_वामः){
            *स्थितिः = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            प्रत्यागम 0;
        }

        //satisfy 3
        यदि( दक्षिणः_गन्तव्यम्_वामः && 
            !((firstCharDir == U_दक्षिणः_गन्तव्यम्_वामः || firstCharDir == U_दक्षिणः_गन्तव्यम्_वामः_ARABIC) &&
              (direction == U_दक्षिणः_गन्तव्यम्_वामः || direction == U_दक्षिणः_गन्तव्यम्_वामः_ARABIC))
           ){
            *स्थितिः = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम s2.extract(dest, dest_क्षमता, *स्थितिः);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_swap(ध्रुव UDataSwapper *ds,
            ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
            UError_संहिता *p_Error_संहिता) {
    ध्रुव UDataInfo *pInfo;
    int32_t शीर्षकम्_आकार;

    ध्रुव uint8_t *अन्तर्_अष्टकानि;
    uint8_t *बहिर्गमनम्_अष्टकानि;

    ध्रुव int32_t *अन्तर्_Indexes;
    int32_t indexes[16];

    int32_t i, विस्थापनम्, count, size;

    /* udata_swapDataHeader checks the arguments */
    शीर्षकम्_आकार=udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)अन्तर्_दत्त+4);
    यदि(!(
        pInfo->दत्त_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->दत्त_Format[1]==0x50 &&
        pInfo->दत्त_Format[2]==0x52 &&
        pInfo->दत्त_Format[3]==0x50 &&
        pInfo->format_संस्करणम्[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->दत्त_Format[0], pInfo->दत्त_Format[1],
                         pInfo->दत्त_Format[2], pInfo->दत्त_Format[3],
                         pInfo->format_संस्करणम्[0]);
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_अष्टकानि=(ध्रुव uint8_t *)अन्तर्_दत्त+शीर्षकम्_आकार;
    बहिर्गमनम्_अष्टकानि= (बहिर्गमनम्_दत्त == शून्यसूचकः ) ? शून्यसूचकः : (uint8_t *)बहिर्गमनम्_दत्त+शीर्षकम्_आकार;

    अन्तर्_Indexes=(ध्रुव int32_t *)अन्तर्_अष्टकानि;

    यदि(length>=0) {
        length-=शीर्षकम्_आकार;
        यदि(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    प्रति(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, अन्तर्_Indexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[SPREP_सूचकांक_TRIE_आकार]+
        indexes[SPREP_सूचकांक_MAPPING_दत्त_आकार];

    यदि(length>=0) {
        यदि(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        /* copy the data for inaccessible bytes */
        यदि(अन्तर्_अष्टकानि!=बहिर्गमनम्_अष्टकानि) {
            uprv_memcpy(बहिर्गमनम्_अष्टकानि, अन्तर्_अष्टकानि, size);
        }

        विस्थापनम्=0;

        /* swap the int32_t indexes[] */
        count=16*4;
        ds->swapArray32(ds, अन्तर्_अष्टकानि, count, बहिर्गमनम्_अष्टकानि, p_Error_संहिता);
        विस्थापनम्+=count;

        /* swap the UTrie */
        count=indexes[SPREP_सूचकांक_TRIE_आकार];
        utrie_swap(ds, अन्तर्_अष्टकानि+विस्थापनम्, count, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);
        विस्थापनम्+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[SPREP_सूचकांक_MAPPING_दत्त_आकार];
        ds->swapArray16(ds, अन्तर्_अष्टकानि+विस्थापनम्, count, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);
        //offset+=count;
    }

    प्रत्यागम शीर्षकम्_आकार+size;
}

#यदि_अन्तः /* #if !UCONFIG_NO_IDNA */
