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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_IDNA

#അന്തര്ഭാവ "unicode/usprep.h"

#അന്തര്ഭാവ "unicode/normalizer2.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/uversion.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "sprpimpl.h"
#അന്തര്ഭാവ "ustr_imp.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "udataswp.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "ubidi_props.h"
#അന്തര്ഭാവ "uprops.h"

U_NAMESPACE_USE

U_CDECL_BEGIN

/*
Static cache for already opened StringPrep profiles
*/
സ്ഥിതം UHashtable *SHARED_DATA_HASHTABLE = nullptr;
സ്ഥിതം icu::UInitOnce gSharedDataInitOnce {};

സ്ഥിതം UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
സ്ഥിതം UVersionInfo dataVersion={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം PROFILE_NAMES[] = {
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

സ്ഥിതം UBool U_CALLCONV
isSPrepAcceptable(ശൂന്യം * /* context */,
             സ്ഥിരം അക്ഷരം * /* type */, 
             സ്ഥിരം അക്ഷരം * /* name */,
             സ്ഥിരം UDataInfo *pInfo) {
    എങ്കിൽ(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->dataFormat[1]==0x50 &&
        pInfo->dataFormat[2]==0x52 &&
        pInfo->dataFormat[3]==0x50 &&
        pInfo->formatVersion[0]==3 &&
        pInfo->formatVersion[2]==UTRIE_SHIFT &&
        pInfo->formatVersion[3]==UTRIE_INDEX_SHIFT
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(dataVersion, pInfo->dataVersion, 4);
        തിരികെ സത്യം;
    } അല്ലെങ്കിൽ {
        തിരികെ അസത്യം;
    }
}

സ്ഥിതം int32_t U_CALLCONV
getSPrepFoldingOffset(uint32_t data) {

    തിരികെ (int32_t)data;

}

/* hashes an entry  */
സ്ഥിതം int32_t U_CALLCONV 
hashEntry(സ്ഥിരം UHashTok parm) {
    UString_Prep_താക്കോൽ *b = (UString_Prep_താക്കോൽ *)parm.pointer;
    UHashTok namekey, pathkey;
    namekey.pointer = b->name;
    pathkey.pointer = b->path;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    തിരികെ static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
സ്ഥിതം UBool U_CALLCONV 
compareEntries(സ്ഥിരം UHashTok p1, സ്ഥിരം UHashTok p2) {
    UString_Prep_താക്കോൽ *b1 = (UString_Prep_താക്കോൽ *)p1.pointer;
    UString_Prep_താക്കോൽ *b2 = (UString_Prep_താക്കോൽ *)p2.pointer;
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->name;
    name2.pointer = b2->name;
    path1.pointer = b1->path;
    path2.pointer = b2->path;
    തിരികെ uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

സ്ഥിതം ശൂന്യം 
usprep_unload(UStringPrepProfile* data){
    udata_close(data->sprepData);
}

സ്ഥിതം int32_t 
usprep_internal_flushCache(UBool no_Ref_എണ്ണം){
    UStringPrepProfile *profile = nullptr;
    UString_Prep_താക്കോൽ  *താക്കോൽ  = nullptr;
    int32_t pos = UHASH_FIRST;
    int32_t deleted_അക്കം = 0;
    സ്ഥിരം UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_lock(&usprepMutex);
    എങ്കിൽ (SHARED_DATA_HASHTABLE == nullptr) {
        umtx_unlock(&usprepMutex);
        തിരികെ 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    ആകുമ്പോൾ ((e = uhash_nextElement(SHARED_DATA_HASHTABLE, &pos)) != nullptr)
    {
        profile = (UStringPrepProfile *) e->value.pointer;
        താക്കോൽ  = (UString_Prep_താക്കോൽ *) e->താക്കോൽ.pointer;

        എങ്കിൽ ((no_Ref_എണ്ണം== അസത്യം && profile->ref_എണ്ണം == 0) || 
             no_Ref_എണ്ണം) {
            deleted_അക്കം++;
            uhash_removeElement(SHARED_DATA_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            എങ്കിൽ(താക്കോൽ->name != nullptr) {
                uprv_free(താക്കോൽ->name);
                താക്കോൽ->name=nullptr;
            }
            എങ്കിൽ(താക്കോൽ->path != nullptr) {
                uprv_free(താക്കോൽ->path);
                താക്കോൽ->path=nullptr;
            }
            uprv_free(profile);
            uprv_free(താക്കോൽ);
        }
       
    }
    umtx_unlock(&usprepMutex);

    തിരികെ deleted_അക്കം;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

സ്ഥിതം UBool U_CALLCONV usprep_cleanup(){
    എങ്കിൽ (SHARED_DATA_HASHTABLE != nullptr) {
        usprep_internal_flushCache(സത്യം);
        എങ്കിൽ (SHARED_DATA_HASHTABLE != nullptr && uhash_എണ്ണം(SHARED_DATA_HASHTABLE) == 0) {
            uhash_close(SHARED_DATA_HASHTABLE);
            SHARED_DATA_HASHTABLE = nullptr;
        }
    }
    gSharedDataInitOnce.reset();
    തിരികെ (SHARED_DATA_HASHTABLE == nullptr);
}
U_CDECL_END


/** Initializes the cache for resources */
സ്ഥിതം ശൂന്യം U_CALLCONV
createCache(UErrorCode &status) {
    SHARED_DATA_HASHTABLE = uhash_open(hashEntry, compareEntries, nullptr, &status);
    എങ്കിൽ (U_FAILURE(status)) {
        SHARED_DATA_HASHTABLE = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

സ്ഥിതം ശൂന്യം 
initCache(UErrorCode *status) {
    umtx_initOnce(gSharedDataInitOnce, &createCache, *status);
}

സ്ഥിതം UBool U_CALLCONV
loadData(UStringPrepProfile* profile, 
         സ്ഥിരം അക്ഷരം* path, 
         സ്ഥിരം അക്ഷരം* name, 
         സ്ഥിരം അക്ഷരം* type, 
         UErrorCode* errorCode) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {nullptr, nullptr, nullptr, 0, 0, 0, 0};
    UDataMemory *dataMemory;
    സ്ഥിരം int32_t *p=nullptr;
    സ്ഥിരം uint8_t *pb;
    UVersionInfo normUnicodeVersion;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    എങ്കിൽ(errorCode==nullptr || U_FAILURE(*errorCode)) {
        തിരികെ 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    dataMemory=udata_openChoice(path, type, name, isSPrepAcceptable, nullptr, errorCode);
    എങ്കിൽ(U_FAILURE(*errorCode)) {
        തിരികെ അസത്യം;
    }

    p = static_cast<സ്ഥിരം int32_t*>(udata_getMemory(dataMemory));
    pb = reinterpret_cast<സ്ഥിരം uint8_t*>(p + SPREP_INDEX_മുകൾ);
    utrie_unserialize(&_sprepTrie, pb, p[_SPREP_INDEX_TRIE_SIZE], errorCode);
    _sprepTrie.getFoldingOffset=getSPrepFoldingOffset;


    എങ്കിൽ(U_FAILURE(*errorCode)) {
        udata_close(dataMemory);
        തിരികെ അസത്യം;
    }

    /* in the mutex block, set the data for this process */
    umtx_lock(&usprepMutex);
    എങ്കിൽ(profile->sprepData==nullptr) {
        profile->sprepData=dataMemory;
        dataMemory=nullptr;
        uprv_memcpy(&profile->indexes, p, വലുപ്പം(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, വലുപ്പം(UTrie));
    } അല്ലെങ്കിൽ {
        p = static_cast<സ്ഥിരം int32_t*>(udata_getMemory(profile->sprepData));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mappingData = reinterpret_cast<സ്ഥിരം uint16_t*>(reinterpret_cast<സ്ഥിരം uint8_t*>(p + SPREP_INDEX_മുകൾ) + profile->indexes[_SPREP_INDEX_TRIE_SIZE]);
    
    u_getUnicodeVersion(normUnicodeVersion);
    normUniVer = (normUnicodeVersion[0] << 24) + (normUnicodeVersion[1] << 16) + 
                 (normUnicodeVersion[2] << 8 ) + (normUnicodeVersion[3]);
    sprepUniVer = (dataVersion[0] << 24) + (dataVersion[1] << 16) + 
                  (dataVersion[2] << 8 ) + (dataVersion[3]);
    normCorrVer = profile->indexes[_SPREP_NORM_CORRECTNS_LAST_UNI_VERSION];
    
    എങ്കിൽ(U_FAILURE(*errorCode)){
        udata_close(dataMemory);
        തിരികെ അസത്യം;
    }
    എങ്കിൽ( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *errorCode = U_INVALID_FORMAT_ERROR;
        udata_close(dataMemory);
        തിരികെ അസത്യം;
    }
    profile->isDataLoaded = സത്യം;

    /* if a different thread set it first, then close the extra data */
    എങ്കിൽ(dataMemory!=nullptr) {
        udata_close(dataMemory); /* nullptr if it was set correctly */
    }


    തിരികെ profile->isDataLoaded;
}

സ്ഥിതം UStringPrepProfile* 
usprep_getProfile(സ്ഥിരം അക്ഷരം* path, 
                  സ്ഥിരം അക്ഷരം* name,
                  UErrorCode *status){

    UStringPrepProfile* profile = nullptr;

    initCache(status);

    എങ്കിൽ(U_FAILURE(*status)){
        തിരികെ nullptr;
    }

    UString_Prep_താക്കോൽ അട്ടി_താക്കോൽ;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    അട്ടി_താക്കോൽ.name = const_cast<അക്ഷരം*>(name);
    അട്ടി_താക്കോൽ.path = const_cast<അക്ഷരം*>(path);

    /* fetch the data from the cache */
    umtx_lock(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_DATA_HASHTABLE, &അട്ടി_താക്കോൽ));
    എങ്കിൽ(profile != nullptr) {
        profile->ref_എണ്ണം++;
    }
    umtx_unlock(&usprepMutex);
    
    എങ്കിൽ(profile == nullptr) {
        /* else load the data and put the data in the cache */
        LocalMemory<UStringPrepProfile> newProfile;
        എങ്കിൽ(newProfile.allocateInsteadAndReset() == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ nullptr;
        }

        /* load the data */
        എങ്കിൽ(!loadData(newProfile.getAlias(), path, name, _SPREP_DATA_TYPE, status) || U_FAILURE(*status) ){
            തിരികെ nullptr;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        LocalMemory<UString_Prep_താക്കോൽ> താക്കോൽ;
        LocalMemory<അക്ഷരം> താക്കോൽ_Name;
        LocalMemory<അക്ഷരം> താക്കോൽ_Path;
        എങ്കിൽ( താക്കോൽ.allocateInsteadAndReset() == nullptr ||
            താക്കോൽ_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == nullptr ||
            (path != nullptr &&
             താക്കോൽ_Path.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(path)+1)) == nullptr)
         ) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            usprep_unload(newProfile.getAlias());
            തിരികെ nullptr;
        }

        umtx_lock(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_DATA_HASHTABLE, &അട്ടി_താക്കോൽ));
        എങ്കിൽ(profile != nullptr) {
            profile->ref_എണ്ണം++;
            usprep_unload(newProfile.getAlias());
        }
        അല്ലെങ്കിൽ {
            /* initialize the key members */
            താക്കോൽ->name = താക്കോൽ_Name.orphan();
            uprv_strcpy(താക്കോൽ->name, name);
            എങ്കിൽ(path != nullptr){
                താക്കോൽ->path = താക്കോൽ_Path.orphan();
                uprv_strcpy(താക്കോൽ->path, path);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_എണ്ണം = 1;
            uhash_put(SHARED_DATA_HASHTABLE, താക്കോൽ.orphan(), profile, status);
        }
        umtx_unlock(&usprepMutex);
    }

    തിരികെ profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(സ്ഥിരം അക്ഷരം* path, 
            സ്ഥിരം അക്ഷരം* name,
            UErrorCode* status){

    എങ്കിൽ(status == nullptr || U_FAILURE(*status)){
        തിരികെ nullptr;
    }
       
    /* initialize the profile struct members */
    തിരികെ usprep_getProfile(path,name,status);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UErrorCode* status) {
    എങ്കിൽ(status == nullptr || U_FAILURE(*status)){
        തിരികെ nullptr;
    }
    int32_t index = (int32_t)type;
    എങ്കിൽ (index < 0 || index >= UPRV_LENGTHOF(PROFILE_NAMES)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    തിരികെ usprep_open(nullptr, PROFILE_NAMES[index], status);
}

U_CAPI ശൂന്യം U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    എങ്കിൽ(profile==nullptr){
        തിരികെ;
    }

    umtx_lock(&usprepMutex);
    /* decrement the ref count*/
    എങ്കിൽ(profile->ref_എണ്ണം > 0){
        profile->ref_എണ്ണം--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC ശൂന്യം 
uprv_syntaxError(സ്ഥിരം char16_t* rules,
                 int32_t pos,
                 int32_t rulesLen,
                 UParseError* parseError){
    എങ്കിൽ(parseError == nullptr){
        തിരികെ;
    }
    parseError->offset = pos;
    parseError->വരി = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t start = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t limit = pos;
    
    u_memcpy(parseError->preContext,rules+start,limit-start);
    //null terminate the buffer
    parseError->preContext[limit-start] = 0;
    
    // for post-context; include error rules[pos]
    start = pos;
    limit = start + (U_PARSE_CONTEXT_LEN-1);
    എങ്കിൽ (limit > rulesLen) {
        limit = rulesLen;
    }
    എങ്കിൽ (start < rulesLen) {
        u_memcpy(parseError->postContext,rules+start,limit-start);
    }
    //null terminate the buffer
    parseError->postContext[limit-start]= 0;
}


സ്ഥിതം വരിയിൽ UStringPrepType
get_മൂല്യങ്ങൾ(uint16_t trieWord, int16_t& value, UBool& isIndex){

    UStringPrepType type;
    എങ്കിൽ(trieWord == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_LIMIT;
        isIndex =അസത്യം;
        value = 0;
    }അല്ലെങ്കിൽ എങ്കിൽ(trieWord >= _SPREP_TYPE_THRESHOLD){
        type = static_cast<UStringPrepType>(trieWord - _SPREP_TYPE_THRESHOLD);
        isIndex =അസത്യം;
        value = 0;
    }അല്ലെങ്കിൽ{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        എങ്കിൽ(trieWord & 0x02){
            isIndex = സത്യം;
            value = trieWord  >> 2; //mask off the lower 2 bits and shift
        }അല്ലെങ്കിൽ{
            isIndex = അസത്യം;
            value = static_cast<int16_t>(trieWord);
            value =  (value >> 2);
        }
 
        എങ്കിൽ((trieWord>>2) == SPREP_MAX_INDEX_മൂല്യം){
            type = USPREP_DELETE;
            isIndex =അസത്യം;
            value = 0;
        }
    }
    തിരികെ type;
}

// TODO: change to writing to UnicodeString not char16_t *
സ്ഥിതം int32_t 
usprep_map(  സ്ഥിരം UStringPrepProfile* profile, 
             സ്ഥിരം char16_t* src, int32_t srcLength,
             char16_t* dest, int32_t destCapacity,
             int32_t options,
             UParseError* parseError,
             UErrorCode* status ){
    
    uint16_t result;
    int32_t destIndex=0;
    int32_t srcIndex;
    UBool allowUnassigned = static_cast<UBool>((options & USPREP_ALLOW_UNASSIGNED) > 0);
    UStringPrepType type;
    int16_t value;
    UBool isIndex;
    സ്ഥിരം int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    വേണ്ടി(srcIndex=0;srcIndex<srcLength;){
        UChar32 ch;

        U16_NEXT(src,srcIndex,srcLength,ch);
        
        result=0;

        UTRIE_GET16(&profile->sprepTrie,ch,result);
        
        type = get_മൂല്യങ്ങൾ(result, value, isIndex);

        // check if the source codepoint is unassigned
        എങ്കിൽ(type == USPREP_UNASSIGNED && allowUnassigned == അസത്യം){

            uprv_syntaxError(src,srcIndex-U16_LENGTH(ch), srcLength,parseError);
            *status = U_STRINGPREP_UNASSIGNED_ERROR;
            തിരികെ 0;
            
        }അല്ലെങ്കിൽ എങ്കിൽ(type == USPREP_MAP){
            
            int32_t index, length;

            എങ്കിൽ(isIndex){
                index = value;
                എങ്കിൽ(index >= indexes[_SPREP_ONE_UCHAR_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_TWO_UCHARS_MAPPING_INDEX_START]){
                    length = 1;
                }അല്ലെങ്കിൽ എങ്കിൽ(index >= indexes[_SPREP_TWO_UCHARS_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_THREE_UCHARS_MAPPING_INDEX_START]){
                    length = 2;
                }അല്ലെങ്കിൽ എങ്കിൽ(index >= indexes[_SPREP_THREE_UCHARS_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_FOUR_UCHARS_MAPPING_INDEX_START]){
                    length = 3;
                }അല്ലെങ്കിൽ{
                    length = profile->mappingData[index++];
         
                }

                /* copy mapping to destination */
                വേണ്ടി(int32_t i=0; i< length; i++){
                    എങ്കിൽ(destIndex < destCapacity  ){
                        dest[destIndex] = profile->mappingData[index+i];
                    }
                    destIndex++; /* for pre-flighting */
                }  
                തുടരുക;
            }അല്ലെങ്കിൽ{
                // subtract the delta to arrive at the code point
                ch -= value;
            }

        }അല്ലെങ്കിൽ എങ്കിൽ(type==USPREP_DELETE){
             // just consume the codepoint and continue
            തുടരുക;
        }
        //copy the code point into destination
        എങ്കിൽ(ch <= 0xFFFF){
            എങ്കിൽ(destIndex < destCapacity ){
                dest[destIndex] = static_cast<char16_t>(ch);
            }
            destIndex++;
        }അല്ലെങ്കിൽ{
            എങ്കിൽ(destIndex+1 < destCapacity ){
                dest[destIndex]   = U16_LEAD(ch);
                dest[destIndex+1] = U16_TRAIL(ch);
            }
            destIndex +=2;
        }
       
    }
        
    തിരികെ u_terminateUChars(dest, destCapacity, destIndex, status);
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
usprep_prepare(   സ്ഥിരം UStringPrepProfile* profile,
                  സ്ഥിരം char16_t* src, int32_t srcLength,
                  char16_t* dest, int32_t destCapacity,
                  int32_t options,
                  UParseError* parseError,
                  UErrorCode* status ){

    // check error status
    എങ്കിൽ(U_FAILURE(*status)){
        തിരികെ 0;
    }

    //check arguments
    എങ്കിൽ(profile==nullptr ||
            (src==nullptr ? srcLength!=0 : srcLength<-1) ||
            (dest==nullptr ? destCapacity!=0 : destCapacity<0)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    //get the string length
    എങ്കിൽ(srcLength < 0){
        srcLength = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.getBuffer(srcLength);
    എങ്കിൽ(b1==nullptr){
        *status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ 0;
    }
    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, srcLength,
                               b1, s1.getCapacity(), options, parseError, &bufferStatus);
    s1.releaseBuffer(U_SUCCESS(bufferStatus) ? b1Len : 0);

    എങ്കിൽ(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.getBuffer(b1Len);
        എങ്കിൽ(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ 0;
        }

        bufferStatus = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, srcLength,
                           b1, s1.getCapacity(), options, parseError, &bufferStatus);
        s1.releaseBuffer(U_SUCCESS(bufferStatus) ? b1Len : 0);
    }
    എങ്കിൽ(U_FAILURE(bufferStatus)){
        *status = bufferStatus;
        തിരികെ 0;
    }

    // normalize
    UnicodeString s2;
    എങ്കിൽ(profile->doNFKC){
        സ്ഥിരം Normalizer2 *n2 = Normalizer2::getNFKCInstance(*status);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*status));
        എങ്കിൽ(U_FAILURE(*status)){
            തിരികെ 0;
        }
        fn2.normalize(s1, s2, *status);
    }അല്ലെങ്കിൽ{
        s2.fastCopyFrom(s1);
    }
    എങ്കിൽ(U_FAILURE(*status)){
        തിരികെ 0;
    }

    // Prohibit and checkBiDi in one pass
    സ്ഥിരം char16_t *b2 = s2.getBuffer();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_എണ്ണം, firstCharDir=U_CHAR_DIRECTION_എണ്ണം;
    UBool ഇടത്_To_വലത്=അസത്യം, വലത്_To_ഇടത്=അസത്യം;
    int32_t rtlPos =-1, ltrPos =-1;

    വേണ്ടി(int32_t b2Index=0; b2Index<b2Len;){
        UChar32 ch = 0;
        U16_NEXT(b2, b2Index, b2Len, ch);

        uint16_t result;
        UTRIE_GET16(&profile->sprepTrie,ch,result);

        int16_t value;
        UBool isIndex;
        UStringPrepType type = get_മൂല്യങ്ങൾ(result, value, isIndex);

        എങ്കിൽ( type == USPREP_PROHIBITED || 
            ((result < _SPREP_TYPE_THRESHOLD) && (result & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *status = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2Index-U16_LENGTH(ch), b2Len, parseError);
            തിരികെ 0;
        }

        എങ്കിൽ(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            എങ്കിൽ(firstCharDir == U_CHAR_DIRECTION_എണ്ണം){
                firstCharDir = direction;
            }
            എങ്കിൽ(direction == U_ഇടത്_TO_വലത്){
                ഇടത്_To_വലത് = സത്യം;
                ltrPos = b2Index-1;
            }
            എങ്കിൽ(direction == U_വലത്_TO_ഇടത് || direction == U_വലത്_TO_ഇടത്_ARABIC){
                വലത്_To_ഇടത് = സത്യം;
                rtlPos = b2Index-1;
            }
        }
    }
    എങ്കിൽ(profile->checkBiDi){
        // satisfy 2
        എങ്കിൽ( ഇടത്_To_വലത് && വലത്_To_ഇടത്){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            തിരികെ 0;
        }

        //satisfy 3
        എങ്കിൽ( വലത്_To_ഇടത് && 
            !((firstCharDir == U_വലത്_TO_ഇടത് || firstCharDir == U_വലത്_TO_ഇടത്_ARABIC) &&
              (direction == U_വലത്_TO_ഇടത് || direction == U_വലത്_TO_ഇടത്_ARABIC))
           ){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            തിരികെ അസത്യം;
        }
    }
    തിരികെ s2.extract(dest, destCapacity, *status);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_swap(സ്ഥിരം UDataSwapper *ds,
            സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
            UErrorCode *pErrorCode) {
    സ്ഥിരം UDataInfo *pInfo;
    int32_t headerSize;

    സ്ഥിരം uint8_t *inBytes;
    uint8_t *outBytes;

    സ്ഥിരം int32_t *inIndexes;
    int32_t indexes[16];

    int32_t i, offset, count, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    /* check data format and format version */
    pInfo=(സ്ഥിരം UDataInfo *)((സ്ഥിരം അക്ഷരം *)inData+4);
    എങ്കിൽ(!(
        pInfo->dataFormat[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->dataFormat[1]==0x50 &&
        pInfo->dataFormat[2]==0x52 &&
        pInfo->dataFormat[3]==0x50 &&
        pInfo->formatVersion[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        തിരികെ 0;
    }

    inBytes=(സ്ഥിരം uint8_t *)inData+headerSize;
    outBytes= (outData == nullptr ) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(സ്ഥിരം int32_t *)inBytes;

    എങ്കിൽ(length>=0) {
        length-=headerSize;
        എങ്കിൽ(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    വേണ്ടി(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[_SPREP_INDEX_TRIE_SIZE]+
        indexes[_SPREP_INDEX_MAPPING_DATA_SIZE];

    എങ്കിൽ(length>=0) {
        എങ്കിൽ(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }

        /* copy the data for inaccessible bytes */
        എങ്കിൽ(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset=0;

        /* swap the int32_t indexes[] */
        count=16*4;
        ds->swapArray32(ds, inBytes, count, outBytes, pErrorCode);
        offset+=count;

        /* swap the UTrie */
        count=indexes[_SPREP_INDEX_TRIE_SIZE];
        utrie_swap(ds, inBytes+offset, count, outBytes+offset, pErrorCode);
        offset+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[_SPREP_INDEX_MAPPING_DATA_SIZE];
        ds->swapArray16(ds, inBytes+offset, count, outBytes+offset, pErrorCode);
        //offset+=count;
    }

    തിരികെ headerSize+size;
}

#endif /* #if !UCONFIG_NO_IDNA */
