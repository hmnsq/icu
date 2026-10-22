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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_IDNA

#அந்தர்பாவ "unicode/usprep.h"

#அந்தர்பாவ "unicode/normalizer2.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/uversion.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "sprpimpl.h"
#அந்தர்பாவ "ustr_imp.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "udataswp.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "ubidi_props.h"
#அந்தர்பாவ "uprops.h"

U_NAMESPACE_USE

U_CDECL_BEGIN

/*
Static cache for already opened StringPrep profiles
*/
நிலையான UHashtable *SHARED_DATA_HASHTABLE = nullptr;
நிலையான icu::UInitOnce gSharedDataInitOnce {};

நிலையான UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
நிலையான UVersionInfo dataVersion={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
நிலையான மாறிலி எழுத்து * மாறிலி PROFILE_NAMES[] = {
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

நிலையான UBool U_CALLCONV
isSPrepAcceptable(வெறுமை * /* context */,
             மாறிலி எழுத்து * /* type */, 
             மாறிலி எழுத்து * /* name */,
             மாறிலி UDataInfo *pInfo) {
    எனில்(
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
        திரும்பு உண்மை;
    } இல்லையெனில் {
        திரும்பு பொய்;
    }
}

நிலையான int32_t U_CALLCONV
getSPrepFoldingOffset(uint32_t data) {

    திரும்பு (int32_t)data;

}

/* hashes an entry  */
நிலையான int32_t U_CALLCONV 
hashEntry(மாறிலி UHashTok parm) {
    UString_Prep_சாவி *b = (UString_Prep_சாவி *)parm.pointer;
    UHashTok namekey, pathkey;
    namekey.pointer = b->name;
    pathkey.pointer = b->path;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    திரும்பு static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
நிலையான UBool U_CALLCONV 
compareEntries(மாறிலி UHashTok p1, மாறிலி UHashTok p2) {
    UString_Prep_சாவி *b1 = (UString_Prep_சாவி *)p1.pointer;
    UString_Prep_சாவி *b2 = (UString_Prep_சாவி *)p2.pointer;
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->name;
    name2.pointer = b2->name;
    path1.pointer = b1->path;
    path2.pointer = b2->path;
    திரும்பு uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

நிலையான வெறுமை 
usprep_unload(UStringPrepProfile* data){
    udata_close(data->sprepData);
}

நிலையான int32_t 
usprep_internal_flushCache(UBool no_Ref_மொத்தம்){
    UStringPrepProfile *profile = nullptr;
    UString_Prep_சாவி  *சாவி  = nullptr;
    int32_t pos = UHASH_FIRST;
    int32_t deleted_இலக்கம் = 0;
    மாறிலி UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_lock(&usprepMutex);
    எனில் (SHARED_DATA_HASHTABLE == nullptr) {
        umtx_unlock(&usprepMutex);
        திரும்பு 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    போது ((e = uhash_nextElement(SHARED_DATA_HASHTABLE, &pos)) != nullptr)
    {
        profile = (UStringPrepProfile *) e->value.pointer;
        சாவி  = (UString_Prep_சாவி *) e->சாவி.pointer;

        எனில் ((no_Ref_மொத்தம்== பொய் && profile->ref_மொத்தம் == 0) || 
             no_Ref_மொத்தம்) {
            deleted_இலக்கம்++;
            uhash_removeElement(SHARED_DATA_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            எனில்(சாவி->name != nullptr) {
                uprv_free(சாவி->name);
                சாவி->name=nullptr;
            }
            எனில்(சாவி->path != nullptr) {
                uprv_free(சாவி->path);
                சாவி->path=nullptr;
            }
            uprv_free(profile);
            uprv_free(சாவி);
        }
       
    }
    umtx_unlock(&usprepMutex);

    திரும்பு deleted_இலக்கம்;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

நிலையான UBool U_CALLCONV usprep_cleanup(){
    எனில் (SHARED_DATA_HASHTABLE != nullptr) {
        usprep_internal_flushCache(உண்மை);
        எனில் (SHARED_DATA_HASHTABLE != nullptr && uhash_மொத்தம்(SHARED_DATA_HASHTABLE) == 0) {
            uhash_close(SHARED_DATA_HASHTABLE);
            SHARED_DATA_HASHTABLE = nullptr;
        }
    }
    gSharedDataInitOnce.reset();
    திரும்பு (SHARED_DATA_HASHTABLE == nullptr);
}
U_CDECL_END


/** Initializes the cache for resources */
நிலையான வெறுமை U_CALLCONV
createCache(UErrorCode &status) {
    SHARED_DATA_HASHTABLE = uhash_open(hashEntry, compareEntries, nullptr, &status);
    எனில் (U_FAILURE(status)) {
        SHARED_DATA_HASHTABLE = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

நிலையான வெறுமை 
initCache(UErrorCode *status) {
    umtx_initOnce(gSharedDataInitOnce, &createCache, *status);
}

நிலையான UBool U_CALLCONV
loadData(UStringPrepProfile* profile, 
         மாறிலி எழுத்து* path, 
         மாறிலி எழுத்து* name, 
         மாறிலி எழுத்து* type, 
         UErrorCode* errorCode) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {nullptr, nullptr, nullptr, 0, 0, 0, 0};
    UDataMemory *dataMemory;
    மாறிலி int32_t *p=nullptr;
    மாறிலி uint8_t *pb;
    UVersionInfo normUnicodeVersion;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    எனில்(errorCode==nullptr || U_FAILURE(*errorCode)) {
        திரும்பு 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    dataMemory=udata_openChoice(path, type, name, isSPrepAcceptable, nullptr, errorCode);
    எனில்(U_FAILURE(*errorCode)) {
        திரும்பு பொய்;
    }

    p = static_cast<மாறிலி int32_t*>(udata_getMemory(dataMemory));
    pb = reinterpret_cast<மாறிலி uint8_t*>(p + SPREP_INDEX_உச்சி);
    utrie_unserialize(&_sprepTrie, pb, p[_SPREP_INDEX_TRIE_SIZE], errorCode);
    _sprepTrie.getFoldingOffset=getSPrepFoldingOffset;


    எனில்(U_FAILURE(*errorCode)) {
        udata_close(dataMemory);
        திரும்பு பொய்;
    }

    /* in the mutex block, set the data for this process */
    umtx_lock(&usprepMutex);
    எனில்(profile->sprepData==nullptr) {
        profile->sprepData=dataMemory;
        dataMemory=nullptr;
        uprv_memcpy(&profile->indexes, p, அளவு(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, அளவு(UTrie));
    } இல்லையெனில் {
        p = static_cast<மாறிலி int32_t*>(udata_getMemory(profile->sprepData));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mappingData = reinterpret_cast<மாறிலி uint16_t*>(reinterpret_cast<மாறிலி uint8_t*>(p + SPREP_INDEX_உச்சி) + profile->indexes[_SPREP_INDEX_TRIE_SIZE]);
    
    u_getUnicodeVersion(normUnicodeVersion);
    normUniVer = (normUnicodeVersion[0] << 24) + (normUnicodeVersion[1] << 16) + 
                 (normUnicodeVersion[2] << 8 ) + (normUnicodeVersion[3]);
    sprepUniVer = (dataVersion[0] << 24) + (dataVersion[1] << 16) + 
                  (dataVersion[2] << 8 ) + (dataVersion[3]);
    normCorrVer = profile->indexes[_SPREP_NORM_CORRECTNS_LAST_UNI_VERSION];
    
    எனில்(U_FAILURE(*errorCode)){
        udata_close(dataMemory);
        திரும்பு பொய்;
    }
    எனில்( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *errorCode = U_INVALID_FORMAT_ERROR;
        udata_close(dataMemory);
        திரும்பு பொய்;
    }
    profile->isDataLoaded = உண்மை;

    /* if a different thread set it first, then close the extra data */
    எனில்(dataMemory!=nullptr) {
        udata_close(dataMemory); /* nullptr if it was set correctly */
    }


    திரும்பு profile->isDataLoaded;
}

நிலையான UStringPrepProfile* 
usprep_getProfile(மாறிலி எழுத்து* path, 
                  மாறிலி எழுத்து* name,
                  UErrorCode *status){

    UStringPrepProfile* profile = nullptr;

    initCache(status);

    எனில்(U_FAILURE(*status)){
        திரும்பு nullptr;
    }

    UString_Prep_சாவி அடுக்கு_சாவி;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    அடுக்கு_சாவி.name = const_cast<எழுத்து*>(name);
    அடுக்கு_சாவி.path = const_cast<எழுத்து*>(path);

    /* fetch the data from the cache */
    umtx_lock(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_DATA_HASHTABLE, &அடுக்கு_சாவி));
    எனில்(profile != nullptr) {
        profile->ref_மொத்தம்++;
    }
    umtx_unlock(&usprepMutex);
    
    எனில்(profile == nullptr) {
        /* else load the data and put the data in the cache */
        LocalMemory<UStringPrepProfile> newProfile;
        எனில்(newProfile.allocateInsteadAndReset() == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு nullptr;
        }

        /* load the data */
        எனில்(!loadData(newProfile.getAlias(), path, name, _SPREP_DATA_TYPE, status) || U_FAILURE(*status) ){
            திரும்பு nullptr;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        LocalMemory<UString_Prep_சாவி> சாவி;
        LocalMemory<எழுத்து> சாவி_Name;
        LocalMemory<எழுத்து> சாவி_Path;
        எனில்( சாவி.allocateInsteadAndReset() == nullptr ||
            சாவி_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == nullptr ||
            (path != nullptr &&
             சாவி_Path.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(path)+1)) == nullptr)
         ) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            usprep_unload(newProfile.getAlias());
            திரும்பு nullptr;
        }

        umtx_lock(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_DATA_HASHTABLE, &அடுக்கு_சாவி));
        எனில்(profile != nullptr) {
            profile->ref_மொத்தம்++;
            usprep_unload(newProfile.getAlias());
        }
        இல்லையெனில் {
            /* initialize the key members */
            சாவி->name = சாவி_Name.orphan();
            uprv_strcpy(சாவி->name, name);
            எனில்(path != nullptr){
                சாவி->path = சாவி_Path.orphan();
                uprv_strcpy(சாவி->path, path);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_மொத்தம் = 1;
            uhash_put(SHARED_DATA_HASHTABLE, சாவி.orphan(), profile, status);
        }
        umtx_unlock(&usprepMutex);
    }

    திரும்பு profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(மாறிலி எழுத்து* path, 
            மாறிலி எழுத்து* name,
            UErrorCode* status){

    எனில்(status == nullptr || U_FAILURE(*status)){
        திரும்பு nullptr;
    }
       
    /* initialize the profile struct members */
    திரும்பு usprep_getProfile(path,name,status);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UErrorCode* status) {
    எனில்(status == nullptr || U_FAILURE(*status)){
        திரும்பு nullptr;
    }
    int32_t index = (int32_t)type;
    எனில் (index < 0 || index >= UPRV_LENGTHOF(PROFILE_NAMES)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    திரும்பு usprep_open(nullptr, PROFILE_NAMES[index], status);
}

U_CAPI வெறுமை U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    எனில்(profile==nullptr){
        திரும்பு;
    }

    umtx_lock(&usprepMutex);
    /* decrement the ref count*/
    எனில்(profile->ref_மொத்தம் > 0){
        profile->ref_மொத்தம்--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC வெறுமை 
uprv_syntaxError(மாறிலி char16_t* rules,
                 int32_t pos,
                 int32_t rulesLen,
                 UParseError* parseError){
    எனில்(parseError == nullptr){
        திரும்பு;
    }
    parseError->offset = pos;
    parseError->வரி = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t start = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t limit = pos;
    
    u_memcpy(parseError->preContext,rules+start,limit-start);
    //null terminate the buffer
    parseError->preContext[limit-start] = 0;
    
    // for post-context; include error rules[pos]
    start = pos;
    limit = start + (U_PARSE_CONTEXT_LEN-1);
    எனில் (limit > rulesLen) {
        limit = rulesLen;
    }
    எனில் (start < rulesLen) {
        u_memcpy(parseError->postContext,rules+start,limit-start);
    }
    //null terminate the buffer
    parseError->postContext[limit-start]= 0;
}


நிலையான உள்வரி UStringPrepType
get_மதிப்புகள்(uint16_t trieWord, int16_t& value, UBool& isIndex){

    UStringPrepType type;
    எனில்(trieWord == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_LIMIT;
        isIndex =பொய்;
        value = 0;
    }இல்லையெனில் எனில்(trieWord >= _SPREP_TYPE_THRESHOLD){
        type = static_cast<UStringPrepType>(trieWord - _SPREP_TYPE_THRESHOLD);
        isIndex =பொய்;
        value = 0;
    }இல்லையெனில்{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        எனில்(trieWord & 0x02){
            isIndex = உண்மை;
            value = trieWord  >> 2; //mask off the lower 2 bits and shift
        }இல்லையெனில்{
            isIndex = பொய்;
            value = static_cast<int16_t>(trieWord);
            value =  (value >> 2);
        }
 
        எனில்((trieWord>>2) == SPREP_MAX_INDEX_மதிப்பு){
            type = USPREP_DELETE;
            isIndex =பொய்;
            value = 0;
        }
    }
    திரும்பு type;
}

// TODO: change to writing to UnicodeString not char16_t *
நிலையான int32_t 
usprep_map(  மாறிலி UStringPrepProfile* profile, 
             மாறிலி char16_t* src, int32_t srcLength,
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
    மாறிலி int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    க்கு(srcIndex=0;srcIndex<srcLength;){
        UChar32 ch;

        U16_NEXT(src,srcIndex,srcLength,ch);
        
        result=0;

        UTRIE_GET16(&profile->sprepTrie,ch,result);
        
        type = get_மதிப்புகள்(result, value, isIndex);

        // check if the source codepoint is unassigned
        எனில்(type == USPREP_UNASSIGNED && allowUnassigned == பொய்){

            uprv_syntaxError(src,srcIndex-U16_LENGTH(ch), srcLength,parseError);
            *status = U_STRINGPREP_UNASSIGNED_ERROR;
            திரும்பு 0;
            
        }இல்லையெனில் எனில்(type == USPREP_MAP){
            
            int32_t index, length;

            எனில்(isIndex){
                index = value;
                எனில்(index >= indexes[_SPREP_ONE_UCHAR_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_TWO_UCHARS_MAPPING_INDEX_START]){
                    length = 1;
                }இல்லையெனில் எனில்(index >= indexes[_SPREP_TWO_UCHARS_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_THREE_UCHARS_MAPPING_INDEX_START]){
                    length = 2;
                }இல்லையெனில் எனில்(index >= indexes[_SPREP_THREE_UCHARS_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_FOUR_UCHARS_MAPPING_INDEX_START]){
                    length = 3;
                }இல்லையெனில்{
                    length = profile->mappingData[index++];
         
                }

                /* copy mapping to destination */
                க்கு(int32_t i=0; i< length; i++){
                    எனில்(destIndex < destCapacity  ){
                        dest[destIndex] = profile->mappingData[index+i];
                    }
                    destIndex++; /* for pre-flighting */
                }  
                தொடர்;
            }இல்லையெனில்{
                // subtract the delta to arrive at the code point
                ch -= value;
            }

        }இல்லையெனில் எனில்(type==USPREP_DELETE){
             // just consume the codepoint and continue
            தொடர்;
        }
        //copy the code point into destination
        எனில்(ch <= 0xFFFF){
            எனில்(destIndex < destCapacity ){
                dest[destIndex] = static_cast<char16_t>(ch);
            }
            destIndex++;
        }இல்லையெனில்{
            எனில்(destIndex+1 < destCapacity ){
                dest[destIndex]   = U16_LEAD(ch);
                dest[destIndex+1] = U16_TRAIL(ch);
            }
            destIndex +=2;
        }
       
    }
        
    திரும்பு u_terminateUChars(dest, destCapacity, destIndex, status);
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
usprep_prepare(   மாறிலி UStringPrepProfile* profile,
                  மாறிலி char16_t* src, int32_t srcLength,
                  char16_t* dest, int32_t destCapacity,
                  int32_t options,
                  UParseError* parseError,
                  UErrorCode* status ){

    // check error status
    எனில்(U_FAILURE(*status)){
        திரும்பு 0;
    }

    //check arguments
    எனில்(profile==nullptr ||
            (src==nullptr ? srcLength!=0 : srcLength<-1) ||
            (dest==nullptr ? destCapacity!=0 : destCapacity<0)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    //get the string length
    எனில்(srcLength < 0){
        srcLength = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.getBuffer(srcLength);
    எனில்(b1==nullptr){
        *status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு 0;
    }
    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, srcLength,
                               b1, s1.getCapacity(), options, parseError, &bufferStatus);
    s1.releaseBuffer(U_SUCCESS(bufferStatus) ? b1Len : 0);

    எனில்(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.getBuffer(b1Len);
        எனில்(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு 0;
        }

        bufferStatus = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, srcLength,
                           b1, s1.getCapacity(), options, parseError, &bufferStatus);
        s1.releaseBuffer(U_SUCCESS(bufferStatus) ? b1Len : 0);
    }
    எனில்(U_FAILURE(bufferStatus)){
        *status = bufferStatus;
        திரும்பு 0;
    }

    // normalize
    UnicodeString s2;
    எனில்(profile->doNFKC){
        மாறிலி Normalizer2 *n2 = Normalizer2::getNFKCInstance(*status);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*status));
        எனில்(U_FAILURE(*status)){
            திரும்பு 0;
        }
        fn2.normalize(s1, s2, *status);
    }இல்லையெனில்{
        s2.fastCopyFrom(s1);
    }
    எனில்(U_FAILURE(*status)){
        திரும்பு 0;
    }

    // Prohibit and checkBiDi in one pass
    மாறிலி char16_t *b2 = s2.getBuffer();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_மொத்தம், firstCharDir=U_CHAR_DIRECTION_மொத்தம்;
    UBool இடது_To_வலது=பொய், வலது_To_இடது=பொய்;
    int32_t rtlPos =-1, ltrPos =-1;

    க்கு(int32_t b2Index=0; b2Index<b2Len;){
        UChar32 ch = 0;
        U16_NEXT(b2, b2Index, b2Len, ch);

        uint16_t result;
        UTRIE_GET16(&profile->sprepTrie,ch,result);

        int16_t value;
        UBool isIndex;
        UStringPrepType type = get_மதிப்புகள்(result, value, isIndex);

        எனில்( type == USPREP_PROHIBITED || 
            ((result < _SPREP_TYPE_THRESHOLD) && (result & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *status = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2Index-U16_LENGTH(ch), b2Len, parseError);
            திரும்பு 0;
        }

        எனில்(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            எனில்(firstCharDir == U_CHAR_DIRECTION_மொத்தம்){
                firstCharDir = direction;
            }
            எனில்(direction == U_இடது_TO_வலது){
                இடது_To_வலது = உண்மை;
                ltrPos = b2Index-1;
            }
            எனில்(direction == U_வலது_TO_இடது || direction == U_வலது_TO_இடது_ARABIC){
                வலது_To_இடது = உண்மை;
                rtlPos = b2Index-1;
            }
        }
    }
    எனில்(profile->checkBiDi){
        // satisfy 2
        எனில்( இடது_To_வலது && வலது_To_இடது){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            திரும்பு 0;
        }

        //satisfy 3
        எனில்( வலது_To_இடது && 
            !((firstCharDir == U_வலது_TO_இடது || firstCharDir == U_வலது_TO_இடது_ARABIC) &&
              (direction == U_வலது_TO_இடது || direction == U_வலது_TO_இடது_ARABIC))
           ){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            திரும்பு பொய்;
        }
    }
    திரும்பு s2.extract(dest, destCapacity, *status);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_swap(மாறிலி UDataSwapper *ds,
            மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
            UErrorCode *pErrorCode) {
    மாறிலி UDataInfo *pInfo;
    int32_t headerSize;

    மாறிலி uint8_t *inBytes;
    uint8_t *outBytes;

    மாறிலி int32_t *inIndexes;
    int32_t indexes[16];

    int32_t i, offset, count, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }

    /* check data format and format version */
    pInfo=(மாறிலி UDataInfo *)((மாறிலி எழுத்து *)inData+4);
    எனில்(!(
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
        திரும்பு 0;
    }

    inBytes=(மாறிலி uint8_t *)inData+headerSize;
    outBytes= (outData == nullptr ) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(மாறிலி int32_t *)inBytes;

    எனில்(length>=0) {
        length-=headerSize;
        எனில்(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    க்கு(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[_SPREP_INDEX_TRIE_SIZE]+
        indexes[_SPREP_INDEX_MAPPING_DATA_SIZE];

    எனில்(length>=0) {
        எனில்(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }

        /* copy the data for inaccessible bytes */
        எனில்(inBytes!=outBytes) {
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

    திரும்பு headerSize+size;
}

#endif /* #if !UCONFIG_NO_IDNA */
