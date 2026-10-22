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
स्थिर UHashtable *SHARED_DATA_HASHTABLE = nullptr;
स्थिर icu::UInitOnce gSharedDataInitOnce {};

स्थिर UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
स्थिर UVersionInfo dataVersion={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
स्थिर ध्रुव वर्ण * ध्रुव PROFILE_NAMES[] = {
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
        प्रत्यागम सत्य;
    } अन्यथा {
        प्रत्यागम असत्य;
    }
}

स्थिर int32_t U_CALLCONV
getSPrepFoldingOffset(uint32_t data) {

    प्रत्यागम (int32_t)data;

}

/* hashes an entry  */
स्थिर int32_t U_CALLCONV 
hashEntry(ध्रुव UHashTok parm) {
    UString_Prep_कुंजी *b = (UString_Prep_कुंजी *)parm.pointer;
    UHashTok namekey, pathkey;
    namekey.pointer = b->name;
    pathkey.pointer = b->path;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    प्रत्यागम static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
स्थिर UBool U_CALLCONV 
compareEntries(ध्रुव UHashTok p1, ध्रुव UHashTok p2) {
    UString_Prep_कुंजी *b1 = (UString_Prep_कुंजी *)p1.pointer;
    UString_Prep_कुंजी *b2 = (UString_Prep_कुंजी *)p2.pointer;
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->name;
    name2.pointer = b2->name;
    path1.pointer = b1->path;
    path2.pointer = b2->path;
    प्रत्यागम uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

स्थिर शून्य 
usprep_unload(UStringPrepProfile* data){
    udata_close(data->sprepData);
}

स्थिर int32_t 
usprep_internal_flushCache(UBool no_Ref_गिनती){
    UStringPrepProfile *profile = nullptr;
    UString_Prep_कुंजी  *कुंजी  = nullptr;
    int32_t pos = UHASH_FIRST;
    int32_t deleted_अंक = 0;
    ध्रुव UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_lock(&usprepMutex);
    यदि (SHARED_DATA_HASHTABLE == nullptr) {
        umtx_unlock(&usprepMutex);
        प्रत्यागम 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    यावत् ((e = uhash_nextElement(SHARED_DATA_HASHTABLE, &pos)) != nullptr)
    {
        profile = (UStringPrepProfile *) e->value.pointer;
        कुंजी  = (UString_Prep_कुंजी *) e->कुंजी.pointer;

        यदि ((no_Ref_गिनती== असत्य && profile->ref_गिनती == 0) || 
             no_Ref_गिनती) {
            deleted_अंक++;
            uhash_removeElement(SHARED_DATA_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            यदि(कुंजी->name != nullptr) {
                uprv_free(कुंजी->name);
                कुंजी->name=nullptr;
            }
            यदि(कुंजी->path != nullptr) {
                uprv_free(कुंजी->path);
                कुंजी->path=nullptr;
            }
            uprv_free(profile);
            uprv_free(कुंजी);
        }
       
    }
    umtx_unlock(&usprepMutex);

    प्रत्यागम deleted_अंक;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

स्थिर UBool U_CALLCONV usprep_cleanup(){
    यदि (SHARED_DATA_HASHTABLE != nullptr) {
        usprep_internal_flushCache(सत्य);
        यदि (SHARED_DATA_HASHTABLE != nullptr && uhash_गिनती(SHARED_DATA_HASHTABLE) == 0) {
            uhash_close(SHARED_DATA_HASHTABLE);
            SHARED_DATA_HASHTABLE = nullptr;
        }
    }
    gSharedDataInitOnce.reset();
    प्रत्यागम (SHARED_DATA_HASHTABLE == nullptr);
}
U_CDECL_END


/** Initializes the cache for resources */
स्थिर शून्य U_CALLCONV
createCache(UErrorCode &status) {
    SHARED_DATA_HASHTABLE = uhash_open(hashEntry, compareEntries, nullptr, &status);
    यदि (U_FAILURE(status)) {
        SHARED_DATA_HASHTABLE = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

स्थिर शून्य 
initCache(UErrorCode *status) {
    umtx_initOnce(gSharedDataInitOnce, &createCache, *status);
}

स्थिर UBool U_CALLCONV
loadData(UStringPrepProfile* profile, 
         ध्रुव वर्ण* path, 
         ध्रुव वर्ण* name, 
         ध्रुव वर्ण* type, 
         UErrorCode* errorCode) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {nullptr, nullptr, nullptr, 0, 0, 0, 0};
    UDataMemory *dataMemory;
    ध्रुव int32_t *p=nullptr;
    ध्रुव uint8_t *pb;
    UVersionInfo normUnicodeVersion;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    यदि(errorCode==nullptr || U_FAILURE(*errorCode)) {
        प्रत्यागम 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    dataMemory=udata_openChoice(path, type, name, isSPrepAcceptable, nullptr, errorCode);
    यदि(U_FAILURE(*errorCode)) {
        प्रत्यागम असत्य;
    }

    p = static_cast<ध्रुव int32_t*>(udata_getMemory(dataMemory));
    pb = reinterpret_cast<ध्रुव uint8_t*>(p + SPREP_INDEX_शीर्ष);
    utrie_unserialize(&_sprepTrie, pb, p[_SPREP_INDEX_TRIE_SIZE], errorCode);
    _sprepTrie.getFoldingOffset=getSPrepFoldingOffset;


    यदि(U_FAILURE(*errorCode)) {
        udata_close(dataMemory);
        प्रत्यागम असत्य;
    }

    /* in the mutex block, set the data for this process */
    umtx_lock(&usprepMutex);
    यदि(profile->sprepData==nullptr) {
        profile->sprepData=dataMemory;
        dataMemory=nullptr;
        uprv_memcpy(&profile->indexes, p, परिमाण(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, परिमाण(UTrie));
    } अन्यथा {
        p = static_cast<ध्रुव int32_t*>(udata_getMemory(profile->sprepData));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mappingData = reinterpret_cast<ध्रुव uint16_t*>(reinterpret_cast<ध्रुव uint8_t*>(p + SPREP_INDEX_शीर्ष) + profile->indexes[_SPREP_INDEX_TRIE_SIZE]);
    
    u_getUnicodeVersion(normUnicodeVersion);
    normUniVer = (normUnicodeVersion[0] << 24) + (normUnicodeVersion[1] << 16) + 
                 (normUnicodeVersion[2] << 8 ) + (normUnicodeVersion[3]);
    sprepUniVer = (dataVersion[0] << 24) + (dataVersion[1] << 16) + 
                  (dataVersion[2] << 8 ) + (dataVersion[3]);
    normCorrVer = profile->indexes[_SPREP_NORM_CORRECTNS_LAST_UNI_VERSION];
    
    यदि(U_FAILURE(*errorCode)){
        udata_close(dataMemory);
        प्रत्यागम असत्य;
    }
    यदि( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *errorCode = U_INVALID_FORMAT_ERROR;
        udata_close(dataMemory);
        प्रत्यागम असत्य;
    }
    profile->isDataLoaded = सत्य;

    /* if a different thread set it first, then close the extra data */
    यदि(dataMemory!=nullptr) {
        udata_close(dataMemory); /* nullptr if it was set correctly */
    }


    प्रत्यागम profile->isDataLoaded;
}

स्थिर UStringPrepProfile* 
usprep_getProfile(ध्रुव वर्ण* path, 
                  ध्रुव वर्ण* name,
                  UErrorCode *status){

    UStringPrepProfile* profile = nullptr;

    initCache(status);

    यदि(U_FAILURE(*status)){
        प्रत्यागम nullptr;
    }

    UString_Prep_कुंजी ढेर_कुंजी;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    ढेर_कुंजी.name = const_cast<वर्ण*>(name);
    ढेर_कुंजी.path = const_cast<वर्ण*>(path);

    /* fetch the data from the cache */
    umtx_lock(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_DATA_HASHTABLE, &ढेर_कुंजी));
    यदि(profile != nullptr) {
        profile->ref_गिनती++;
    }
    umtx_unlock(&usprepMutex);
    
    यदि(profile == nullptr) {
        /* else load the data and put the data in the cache */
        LocalMemory<UStringPrepProfile> newProfile;
        यदि(newProfile.allocateInsteadAndReset() == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम nullptr;
        }

        /* load the data */
        यदि(!loadData(newProfile.getAlias(), path, name, _SPREP_DATA_TYPE, status) || U_FAILURE(*status) ){
            प्रत्यागम nullptr;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        LocalMemory<UString_Prep_कुंजी> कुंजी;
        LocalMemory<वर्ण> कुंजी_Name;
        LocalMemory<वर्ण> कुंजी_Path;
        यदि( कुंजी.allocateInsteadAndReset() == nullptr ||
            कुंजी_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == nullptr ||
            (path != nullptr &&
             कुंजी_Path.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(path)+1)) == nullptr)
         ) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            usprep_unload(newProfile.getAlias());
            प्रत्यागम nullptr;
        }

        umtx_lock(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_DATA_HASHTABLE, &ढेर_कुंजी));
        यदि(profile != nullptr) {
            profile->ref_गिनती++;
            usprep_unload(newProfile.getAlias());
        }
        अन्यथा {
            /* initialize the key members */
            कुंजी->name = कुंजी_Name.orphan();
            uprv_strcpy(कुंजी->name, name);
            यदि(path != nullptr){
                कुंजी->path = कुंजी_Path.orphan();
                uprv_strcpy(कुंजी->path, path);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_गिनती = 1;
            uhash_put(SHARED_DATA_HASHTABLE, कुंजी.orphan(), profile, status);
        }
        umtx_unlock(&usprepMutex);
    }

    प्रत्यागम profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(ध्रुव वर्ण* path, 
            ध्रुव वर्ण* name,
            UErrorCode* status){

    यदि(status == nullptr || U_FAILURE(*status)){
        प्रत्यागम nullptr;
    }
       
    /* initialize the profile struct members */
    प्रत्यागम usprep_getProfile(path,name,status);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UErrorCode* status) {
    यदि(status == nullptr || U_FAILURE(*status)){
        प्रत्यागम nullptr;
    }
    int32_t index = (int32_t)type;
    यदि (index < 0 || index >= UPRV_LENGTHOF(PROFILE_NAMES)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    प्रत्यागम usprep_open(nullptr, PROFILE_NAMES[index], status);
}

U_CAPI शून्य U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    यदि(profile==nullptr){
        प्रत्यागम;
    }

    umtx_lock(&usprepMutex);
    /* decrement the ref count*/
    यदि(profile->ref_गिनती > 0){
        profile->ref_गिनती--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC शून्य 
uprv_syntaxError(ध्रुव char16_t* rules,
                 int32_t pos,
                 int32_t rulesLen,
                 UParseError* parseError){
    यदि(parseError == nullptr){
        प्रत्यागम;
    }
    parseError->offset = pos;
    parseError->पंक्ति = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t start = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t limit = pos;
    
    u_memcpy(parseError->preContext,rules+start,limit-start);
    //null terminate the buffer
    parseError->preContext[limit-start] = 0;
    
    // for post-context; include error rules[pos]
    start = pos;
    limit = start + (U_PARSE_CONTEXT_LEN-1);
    यदि (limit > rulesLen) {
        limit = rulesLen;
    }
    यदि (start < rulesLen) {
        u_memcpy(parseError->postContext,rules+start,limit-start);
    }
    //null terminate the buffer
    parseError->postContext[limit-start]= 0;
}


स्थिर अन्तःस्थ UStringPrepType
get_मूल्यावली(uint16_t trieWord, int16_t& value, UBool& isIndex){

    UStringPrepType type;
    यदि(trieWord == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_LIMIT;
        isIndex =असत्य;
        value = 0;
    }अन्यथा यदि(trieWord >= _SPREP_TYPE_THRESHOLD){
        type = static_cast<UStringPrepType>(trieWord - _SPREP_TYPE_THRESHOLD);
        isIndex =असत्य;
        value = 0;
    }अन्यथा{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        यदि(trieWord & 0x02){
            isIndex = सत्य;
            value = trieWord  >> 2; //mask off the lower 2 bits and shift
        }अन्यथा{
            isIndex = असत्य;
            value = static_cast<int16_t>(trieWord);
            value =  (value >> 2);
        }
 
        यदि((trieWord>>2) == SPREP_MAX_INDEX_मूल्य){
            type = USPREP_DELETE;
            isIndex =असत्य;
            value = 0;
        }
    }
    प्रत्यागम type;
}

// TODO: change to writing to UnicodeString not char16_t *
स्थिर int32_t 
usprep_map(  ध्रुव UStringPrepProfile* profile, 
             ध्रुव char16_t* src, int32_t srcLength,
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
    ध्रुव int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    प्रति(srcIndex=0;srcIndex<srcLength;){
        UChar32 ch;

        U16_NEXT(src,srcIndex,srcLength,ch);
        
        result=0;

        UTRIE_GET16(&profile->sprepTrie,ch,result);
        
        type = get_मूल्यावली(result, value, isIndex);

        // check if the source codepoint is unassigned
        यदि(type == USPREP_UNASSIGNED && allowUnassigned == असत्य){

            uprv_syntaxError(src,srcIndex-U16_LENGTH(ch), srcLength,parseError);
            *status = U_STRINGPREP_UNASSIGNED_ERROR;
            प्रत्यागम 0;
            
        }अन्यथा यदि(type == USPREP_MAP){
            
            int32_t index, length;

            यदि(isIndex){
                index = value;
                यदि(index >= indexes[_SPREP_ONE_UCHAR_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_TWO_UCHARS_MAPPING_INDEX_START]){
                    length = 1;
                }अन्यथा यदि(index >= indexes[_SPREP_TWO_UCHARS_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_THREE_UCHARS_MAPPING_INDEX_START]){
                    length = 2;
                }अन्यथा यदि(index >= indexes[_SPREP_THREE_UCHARS_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_FOUR_UCHARS_MAPPING_INDEX_START]){
                    length = 3;
                }अन्यथा{
                    length = profile->mappingData[index++];
         
                }

                /* copy mapping to destination */
                प्रति(int32_t i=0; i< length; i++){
                    यदि(destIndex < destCapacity  ){
                        dest[destIndex] = profile->mappingData[index+i];
                    }
                    destIndex++; /* for pre-flighting */
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
            यदि(destIndex < destCapacity ){
                dest[destIndex] = static_cast<char16_t>(ch);
            }
            destIndex++;
        }अन्यथा{
            यदि(destIndex+1 < destCapacity ){
                dest[destIndex]   = U16_LEAD(ch);
                dest[destIndex+1] = U16_TRAIL(ch);
            }
            destIndex +=2;
        }
       
    }
        
    प्रत्यागम u_terminateUChars(dest, destCapacity, destIndex, status);
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
                  ध्रुव char16_t* src, int32_t srcLength,
                  char16_t* dest, int32_t destCapacity,
                  int32_t options,
                  UParseError* parseError,
                  UErrorCode* status ){

    // check error status
    यदि(U_FAILURE(*status)){
        प्रत्यागम 0;
    }

    //check arguments
    यदि(profile==nullptr ||
            (src==nullptr ? srcLength!=0 : srcLength<-1) ||
            (dest==nullptr ? destCapacity!=0 : destCapacity<0)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    //get the string length
    यदि(srcLength < 0){
        srcLength = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.getBuffer(srcLength);
    यदि(b1==nullptr){
        *status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम 0;
    }
    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, srcLength,
                               b1, s1.getCapacity(), options, parseError, &bufferStatus);
    s1.releaseBuffer(U_SUCCESS(bufferStatus) ? b1Len : 0);

    यदि(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.getBuffer(b1Len);
        यदि(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम 0;
        }

        bufferStatus = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, srcLength,
                           b1, s1.getCapacity(), options, parseError, &bufferStatus);
        s1.releaseBuffer(U_SUCCESS(bufferStatus) ? b1Len : 0);
    }
    यदि(U_FAILURE(bufferStatus)){
        *status = bufferStatus;
        प्रत्यागम 0;
    }

    // normalize
    UnicodeString s2;
    यदि(profile->doNFKC){
        ध्रुव Normalizer2 *n2 = Normalizer2::getNFKCInstance(*status);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*status));
        यदि(U_FAILURE(*status)){
            प्रत्यागम 0;
        }
        fn2.normalize(s1, s2, *status);
    }अन्यथा{
        s2.fastCopyFrom(s1);
    }
    यदि(U_FAILURE(*status)){
        प्रत्यागम 0;
    }

    // Prohibit and checkBiDi in one pass
    ध्रुव char16_t *b2 = s2.getBuffer();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_गिनती, firstCharDir=U_CHAR_DIRECTION_गिनती;
    UBool बायाँ_To_दायाँ=असत्य, दायाँ_To_बायाँ=असत्य;
    int32_t rtlPos =-1, ltrPos =-1;

    प्रति(int32_t b2Index=0; b2Index<b2Len;){
        UChar32 ch = 0;
        U16_NEXT(b2, b2Index, b2Len, ch);

        uint16_t result;
        UTRIE_GET16(&profile->sprepTrie,ch,result);

        int16_t value;
        UBool isIndex;
        UStringPrepType type = get_मूल्यावली(result, value, isIndex);

        यदि( type == USPREP_PROHIBITED || 
            ((result < _SPREP_TYPE_THRESHOLD) && (result & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *status = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2Index-U16_LENGTH(ch), b2Len, parseError);
            प्रत्यागम 0;
        }

        यदि(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            यदि(firstCharDir == U_CHAR_DIRECTION_गिनती){
                firstCharDir = direction;
            }
            यदि(direction == U_बायाँ_TO_दायाँ){
                बायाँ_To_दायाँ = सत्य;
                ltrPos = b2Index-1;
            }
            यदि(direction == U_दायाँ_TO_बायाँ || direction == U_दायाँ_TO_बायाँ_ARABIC){
                दायाँ_To_बायाँ = सत्य;
                rtlPos = b2Index-1;
            }
        }
    }
    यदि(profile->checkBiDi){
        // satisfy 2
        यदि( बायाँ_To_दायाँ && दायाँ_To_बायाँ){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            प्रत्यागम 0;
        }

        //satisfy 3
        यदि( दायाँ_To_बायाँ && 
            !((firstCharDir == U_दायाँ_TO_बायाँ || firstCharDir == U_दायाँ_TO_बायाँ_ARABIC) &&
              (direction == U_दायाँ_TO_बायाँ || direction == U_दायाँ_TO_बायाँ_ARABIC))
           ){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम s2.extract(dest, destCapacity, *status);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_swap(ध्रुव UDataSwapper *ds,
            ध्रुव शून्य *inData, int32_t length, शून्य *outData,
            UErrorCode *pErrorCode) {
    ध्रुव UDataInfo *pInfo;
    int32_t headerSize;

    ध्रुव uint8_t *inBytes;
    uint8_t *outBytes;

    ध्रुव int32_t *inIndexes;
    int32_t indexes[16];

    int32_t i, offset, count, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)inData+4);
    यदि(!(
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
        प्रत्यागम 0;
    }

    inBytes=(ध्रुव uint8_t *)inData+headerSize;
    outBytes= (outData == nullptr ) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(ध्रुव int32_t *)inBytes;

    यदि(length>=0) {
        length-=headerSize;
        यदि(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    प्रति(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[_SPREP_INDEX_TRIE_SIZE]+
        indexes[_SPREP_INDEX_MAPPING_DATA_SIZE];

    यदि(length>=0) {
        यदि(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        /* copy the data for inaccessible bytes */
        यदि(inBytes!=outBytes) {
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

    प्रत्यागम headerSize+size;
}

#endif /* #if !UCONFIG_NO_IDNA */
