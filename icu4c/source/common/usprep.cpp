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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_IDNA

#অন্তর্ভাৱ "unicode/usprep.h"

#অন্তর্ভাৱ "unicode/normalizer2.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/uchar.h"
#অন্তর্ভাৱ "unicode/uversion.h"
#অন্তর্ভাৱ "umutex.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "sprpimpl.h"
#অন্তর্ভাৱ "ustr_imp.h"
#অন্তর্ভাৱ "uhash.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "udataswp.h"
#অন্তর্ভাৱ "ucln_cmn.h"
#অন্তর্ভাৱ "ubidi_props.h"
#অন্তর্ভাৱ "uprops.h"

U_NAMESPACE_USE

U_CDECL_BEGIN

/*
Static cache for already opened StringPrep profiles
*/
স্থির UHashtable *SHARED_DATA_HASHTABLE = nullptr;
স্থির icu::UInitOnce gSharedDataInitOnce {};

স্থির UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
স্থির UVersionInfo dataVersion={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ PROFILE_NAMES[] = {
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

স্থির UBool U_CALLCONV
isSPrepAcceptable(শূন্য * /* context */,
             ধ্রুৱ ৱর্ণ * /* type */, 
             ধ্রুৱ ৱর্ণ * /* name */,
             ধ্রুৱ UDataInfo *pInfo) {
    যদি(
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
        ঘূৰাই সঁচা;
    } অন্যথা {
        ঘূৰাই মিছা;
    }
}

স্থির int32_t U_CALLCONV
getSPrepFoldingOffset(uint32_t data) {

    ঘূৰাই (int32_t)data;

}

/* hashes an entry  */
স্থির int32_t U_CALLCONV 
hashEntry(ধ্রুৱ UHashTok parm) {
    UString_Prep_চাবি *b = (UString_Prep_চাবি *)parm.pointer;
    UHashTok namekey, pathkey;
    namekey.pointer = b->name;
    pathkey.pointer = b->path;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    ঘূৰাই static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
স্থির UBool U_CALLCONV 
compareEntries(ধ্রুৱ UHashTok p1, ধ্রুৱ UHashTok p2) {
    UString_Prep_চাবি *b1 = (UString_Prep_চাবি *)p1.pointer;
    UString_Prep_চাবি *b2 = (UString_Prep_চাবি *)p2.pointer;
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->name;
    name2.pointer = b2->name;
    path1.pointer = b1->path;
    path2.pointer = b2->path;
    ঘূৰাই uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

স্থির শূন্য 
usprep_unload(UStringPrepProfile* data){
    udata_close(data->sprepData);
}

স্থির int32_t 
usprep_internal_flushCache(UBool no_Ref_গুনতি){
    UStringPrepProfile *profile = nullptr;
    UString_Prep_চাবি  *চাবি  = nullptr;
    int32_t pos = UHASH_FIRST;
    int32_t deleted_অঙ্ক = 0;
    ধ্রুৱ UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_lock(&usprepMutex);
    যদি (SHARED_DATA_HASHTABLE == nullptr) {
        umtx_unlock(&usprepMutex);
        ঘূৰাই 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    যেতিয়ালৈ ((e = uhash_nextElement(SHARED_DATA_HASHTABLE, &pos)) != nullptr)
    {
        profile = (UStringPrepProfile *) e->value.pointer;
        চাবি  = (UString_Prep_চাবি *) e->চাবি.pointer;

        যদি ((no_Ref_গুনতি== মিছা && profile->ref_গুনতি == 0) || 
             no_Ref_গুনতি) {
            deleted_অঙ্ক++;
            uhash_removeElement(SHARED_DATA_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            যদি(চাবি->name != nullptr) {
                uprv_free(চাবি->name);
                চাবি->name=nullptr;
            }
            যদি(চাবি->path != nullptr) {
                uprv_free(চাবি->path);
                চাবি->path=nullptr;
            }
            uprv_free(profile);
            uprv_free(চাবি);
        }
       
    }
    umtx_unlock(&usprepMutex);

    ঘূৰাই deleted_অঙ্ক;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

স্থির UBool U_CALLCONV usprep_cleanup(){
    যদি (SHARED_DATA_HASHTABLE != nullptr) {
        usprep_internal_flushCache(সঁচা);
        যদি (SHARED_DATA_HASHTABLE != nullptr && uhash_গুনতি(SHARED_DATA_HASHTABLE) == 0) {
            uhash_close(SHARED_DATA_HASHTABLE);
            SHARED_DATA_HASHTABLE = nullptr;
        }
    }
    gSharedDataInitOnce.reset();
    ঘূৰাই (SHARED_DATA_HASHTABLE == nullptr);
}
U_CDECL_END


/** Initializes the cache for resources */
স্থির শূন্য U_CALLCONV
createCache(UErrorCode &status) {
    SHARED_DATA_HASHTABLE = uhash_open(hashEntry, compareEntries, nullptr, &status);
    যদি (U_FAILURE(status)) {
        SHARED_DATA_HASHTABLE = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

স্থির শূন্য 
initCache(UErrorCode *status) {
    umtx_initOnce(gSharedDataInitOnce, &createCache, *status);
}

স্থির UBool U_CALLCONV
loadData(UStringPrepProfile* profile, 
         ধ্রুৱ ৱর্ণ* path, 
         ধ্রুৱ ৱর্ণ* name, 
         ধ্রুৱ ৱর্ণ* type, 
         UErrorCode* errorCode) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {nullptr, nullptr, nullptr, 0, 0, 0, 0};
    UDataMemory *dataMemory;
    ধ্রুৱ int32_t *p=nullptr;
    ধ্রুৱ uint8_t *pb;
    UVersionInfo normUnicodeVersion;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    যদি(errorCode==nullptr || U_FAILURE(*errorCode)) {
        ঘূৰাই 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    dataMemory=udata_openChoice(path, type, name, isSPrepAcceptable, nullptr, errorCode);
    যদি(U_FAILURE(*errorCode)) {
        ঘূৰাই মিছা;
    }

    p = static_cast<ধ্রুৱ int32_t*>(udata_getMemory(dataMemory));
    pb = reinterpret_cast<ধ্রুৱ uint8_t*>(p + SPREP_INDEX_শীৰ্ষ);
    utrie_unserialize(&_sprepTrie, pb, p[_SPREP_INDEX_TRIE_SIZE], errorCode);
    _sprepTrie.getFoldingOffset=getSPrepFoldingOffset;


    যদি(U_FAILURE(*errorCode)) {
        udata_close(dataMemory);
        ঘূৰাই মিছা;
    }

    /* in the mutex block, set the data for this process */
    umtx_lock(&usprepMutex);
    যদি(profile->sprepData==nullptr) {
        profile->sprepData=dataMemory;
        dataMemory=nullptr;
        uprv_memcpy(&profile->indexes, p, পরিমাণ(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, পরিমাণ(UTrie));
    } অন্যথা {
        p = static_cast<ধ্রুৱ int32_t*>(udata_getMemory(profile->sprepData));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mappingData = reinterpret_cast<ধ্রুৱ uint16_t*>(reinterpret_cast<ধ্রুৱ uint8_t*>(p + SPREP_INDEX_শীৰ্ষ) + profile->indexes[_SPREP_INDEX_TRIE_SIZE]);
    
    u_getUnicodeVersion(normUnicodeVersion);
    normUniVer = (normUnicodeVersion[0] << 24) + (normUnicodeVersion[1] << 16) + 
                 (normUnicodeVersion[2] << 8 ) + (normUnicodeVersion[3]);
    sprepUniVer = (dataVersion[0] << 24) + (dataVersion[1] << 16) + 
                  (dataVersion[2] << 8 ) + (dataVersion[3]);
    normCorrVer = profile->indexes[_SPREP_NORM_CORRECTNS_LAST_UNI_VERSION];
    
    যদি(U_FAILURE(*errorCode)){
        udata_close(dataMemory);
        ঘূৰাই মিছা;
    }
    যদি( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *errorCode = U_INVALID_FORMAT_ERROR;
        udata_close(dataMemory);
        ঘূৰাই মিছা;
    }
    profile->isDataLoaded = সঁচা;

    /* if a different thread set it first, then close the extra data */
    যদি(dataMemory!=nullptr) {
        udata_close(dataMemory); /* nullptr if it was set correctly */
    }


    ঘূৰাই profile->isDataLoaded;
}

স্থির UStringPrepProfile* 
usprep_getProfile(ধ্রুৱ ৱর্ণ* path, 
                  ধ্রুৱ ৱর্ণ* name,
                  UErrorCode *status){

    UStringPrepProfile* profile = nullptr;

    initCache(status);

    যদি(U_FAILURE(*status)){
        ঘূৰাই nullptr;
    }

    UString_Prep_চাবি স্তূপ_চাবি;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    স্তূপ_চাবি.name = const_cast<ৱর্ণ*>(name);
    স্তূপ_চাবি.path = const_cast<ৱর্ণ*>(path);

    /* fetch the data from the cache */
    umtx_lock(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_DATA_HASHTABLE, &স্তূপ_চাবি));
    যদি(profile != nullptr) {
        profile->ref_গুনতি++;
    }
    umtx_unlock(&usprepMutex);
    
    যদি(profile == nullptr) {
        /* else load the data and put the data in the cache */
        LocalMemory<UStringPrepProfile> newProfile;
        যদি(newProfile.allocateInsteadAndReset() == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই nullptr;
        }

        /* load the data */
        যদি(!loadData(newProfile.getAlias(), path, name, _SPREP_DATA_TYPE, status) || U_FAILURE(*status) ){
            ঘূৰাই nullptr;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        LocalMemory<UString_Prep_চাবি> চাবি;
        LocalMemory<ৱর্ণ> চাবি_Name;
        LocalMemory<ৱর্ণ> চাবি_Path;
        যদি( চাবি.allocateInsteadAndReset() == nullptr ||
            চাবি_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == nullptr ||
            (path != nullptr &&
             চাবি_Path.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(path)+1)) == nullptr)
         ) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            usprep_unload(newProfile.getAlias());
            ঘূৰাই nullptr;
        }

        umtx_lock(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_DATA_HASHTABLE, &স্তূপ_চাবি));
        যদি(profile != nullptr) {
            profile->ref_গুনতি++;
            usprep_unload(newProfile.getAlias());
        }
        অন্যথা {
            /* initialize the key members */
            চাবি->name = চাবি_Name.orphan();
            uprv_strcpy(চাবি->name, name);
            যদি(path != nullptr){
                চাবি->path = চাবি_Path.orphan();
                uprv_strcpy(চাবি->path, path);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_গুনতি = 1;
            uhash_put(SHARED_DATA_HASHTABLE, চাবি.orphan(), profile, status);
        }
        umtx_unlock(&usprepMutex);
    }

    ঘূৰাই profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(ধ্রুৱ ৱর্ণ* path, 
            ধ্রুৱ ৱর্ণ* name,
            UErrorCode* status){

    যদি(status == nullptr || U_FAILURE(*status)){
        ঘূৰাই nullptr;
    }
       
    /* initialize the profile struct members */
    ঘূৰাই usprep_getProfile(path,name,status);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UErrorCode* status) {
    যদি(status == nullptr || U_FAILURE(*status)){
        ঘূৰাই nullptr;
    }
    int32_t index = (int32_t)type;
    যদি (index < 0 || index >= UPRV_LENGTHOF(PROFILE_NAMES)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }
    ঘূৰাই usprep_open(nullptr, PROFILE_NAMES[index], status);
}

U_CAPI শূন্য U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    যদি(profile==nullptr){
        ঘূৰাই;
    }

    umtx_lock(&usprepMutex);
    /* decrement the ref count*/
    যদি(profile->ref_গুনতি > 0){
        profile->ref_গুনতি--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC শূন্য 
uprv_syntaxError(ধ্রুৱ char16_t* rules,
                 int32_t pos,
                 int32_t rulesLen,
                 UParseError* parseError){
    যদি(parseError == nullptr){
        ঘূৰাই;
    }
    parseError->offset = pos;
    parseError->সাৰি = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t start = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t limit = pos;
    
    u_memcpy(parseError->preContext,rules+start,limit-start);
    //null terminate the buffer
    parseError->preContext[limit-start] = 0;
    
    // for post-context; include error rules[pos]
    start = pos;
    limit = start + (U_PARSE_CONTEXT_LEN-1);
    যদি (limit > rulesLen) {
        limit = rulesLen;
    }
    যদি (start < rulesLen) {
        u_memcpy(parseError->postContext,rules+start,limit-start);
    }
    //null terminate the buffer
    parseError->postContext[limit-start]= 0;
}


স্থির অন্তঃস্থ UStringPrepType
get_মানসমূহ(uint16_t trieWord, int16_t& value, UBool& isIndex){

    UStringPrepType type;
    যদি(trieWord == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_LIMIT;
        isIndex =মিছা;
        value = 0;
    }অন্যথা যদি(trieWord >= _SPREP_TYPE_THRESHOLD){
        type = static_cast<UStringPrepType>(trieWord - _SPREP_TYPE_THRESHOLD);
        isIndex =মিছা;
        value = 0;
    }অন্যথা{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        যদি(trieWord & 0x02){
            isIndex = সঁচা;
            value = trieWord  >> 2; //mask off the lower 2 bits and shift
        }অন্যথা{
            isIndex = মিছা;
            value = static_cast<int16_t>(trieWord);
            value =  (value >> 2);
        }
 
        যদি((trieWord>>2) == SPREP_MAX_INDEX_মান){
            type = USPREP_DELETE;
            isIndex =মিছা;
            value = 0;
        }
    }
    ঘূৰাই type;
}

// TODO: change to writing to UnicodeString not char16_t *
স্থির int32_t 
usprep_map(  ধ্রুৱ UStringPrepProfile* profile, 
             ধ্রুৱ char16_t* src, int32_t srcLength,
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
    ধ্রুৱ int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    বাবে(srcIndex=0;srcIndex<srcLength;){
        UChar32 ch;

        U16_NEXT(src,srcIndex,srcLength,ch);
        
        result=0;

        UTRIE_GET16(&profile->sprepTrie,ch,result);
        
        type = get_মানসমূহ(result, value, isIndex);

        // check if the source codepoint is unassigned
        যদি(type == USPREP_UNASSIGNED && allowUnassigned == মিছা){

            uprv_syntaxError(src,srcIndex-U16_LENGTH(ch), srcLength,parseError);
            *status = U_STRINGPREP_UNASSIGNED_ERROR;
            ঘূৰাই 0;
            
        }অন্যথা যদি(type == USPREP_MAP){
            
            int32_t index, length;

            যদি(isIndex){
                index = value;
                যদি(index >= indexes[_SPREP_ONE_UCHAR_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_TWO_UCHARS_MAPPING_INDEX_START]){
                    length = 1;
                }অন্যথা যদি(index >= indexes[_SPREP_TWO_UCHARS_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_THREE_UCHARS_MAPPING_INDEX_START]){
                    length = 2;
                }অন্যথা যদি(index >= indexes[_SPREP_THREE_UCHARS_MAPPING_INDEX_START] &&
                         index < indexes[_SPREP_FOUR_UCHARS_MAPPING_INDEX_START]){
                    length = 3;
                }অন্যথা{
                    length = profile->mappingData[index++];
         
                }

                /* copy mapping to destination */
                বাবে(int32_t i=0; i< length; i++){
                    যদি(destIndex < destCapacity  ){
                        dest[destIndex] = profile->mappingData[index+i];
                    }
                    destIndex++; /* for pre-flighting */
                }  
                অনুৱর্ত;
            }অন্যথা{
                // subtract the delta to arrive at the code point
                ch -= value;
            }

        }অন্যথা যদি(type==USPREP_DELETE){
             // just consume the codepoint and continue
            অনুৱর্ত;
        }
        //copy the code point into destination
        যদি(ch <= 0xFFFF){
            যদি(destIndex < destCapacity ){
                dest[destIndex] = static_cast<char16_t>(ch);
            }
            destIndex++;
        }অন্যথা{
            যদি(destIndex+1 < destCapacity ){
                dest[destIndex]   = U16_LEAD(ch);
                dest[destIndex+1] = U16_TRAIL(ch);
            }
            destIndex +=2;
        }
       
    }
        
    ঘূৰাই u_terminateUChars(dest, destCapacity, destIndex, status);
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
usprep_prepare(   ধ্রুৱ UStringPrepProfile* profile,
                  ধ্রুৱ char16_t* src, int32_t srcLength,
                  char16_t* dest, int32_t destCapacity,
                  int32_t options,
                  UParseError* parseError,
                  UErrorCode* status ){

    // check error status
    যদি(U_FAILURE(*status)){
        ঘূৰাই 0;
    }

    //check arguments
    যদি(profile==nullptr ||
            (src==nullptr ? srcLength!=0 : srcLength<-1) ||
            (dest==nullptr ? destCapacity!=0 : destCapacity<0)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    //get the string length
    যদি(srcLength < 0){
        srcLength = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.getBuffer(srcLength);
    যদি(b1==nullptr){
        *status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই 0;
    }
    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, srcLength,
                               b1, s1.getCapacity(), options, parseError, &bufferStatus);
    s1.releaseBuffer(U_SUCCESS(bufferStatus) ? b1Len : 0);

    যদি(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.getBuffer(b1Len);
        যদি(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই 0;
        }

        bufferStatus = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, srcLength,
                           b1, s1.getCapacity(), options, parseError, &bufferStatus);
        s1.releaseBuffer(U_SUCCESS(bufferStatus) ? b1Len : 0);
    }
    যদি(U_FAILURE(bufferStatus)){
        *status = bufferStatus;
        ঘূৰাই 0;
    }

    // normalize
    UnicodeString s2;
    যদি(profile->doNFKC){
        ধ্রুৱ Normalizer2 *n2 = Normalizer2::getNFKCInstance(*status);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*status));
        যদি(U_FAILURE(*status)){
            ঘূৰাই 0;
        }
        fn2.normalize(s1, s2, *status);
    }অন্যথা{
        s2.fastCopyFrom(s1);
    }
    যদি(U_FAILURE(*status)){
        ঘূৰাই 0;
    }

    // Prohibit and checkBiDi in one pass
    ধ্রুৱ char16_t *b2 = s2.getBuffer();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_গুনতি, firstCharDir=U_CHAR_DIRECTION_গুনতি;
    UBool বাম_To_ডান=মিছা, ডান_To_বাম=মিছা;
    int32_t rtlPos =-1, ltrPos =-1;

    বাবে(int32_t b2Index=0; b2Index<b2Len;){
        UChar32 ch = 0;
        U16_NEXT(b2, b2Index, b2Len, ch);

        uint16_t result;
        UTRIE_GET16(&profile->sprepTrie,ch,result);

        int16_t value;
        UBool isIndex;
        UStringPrepType type = get_মানসমূহ(result, value, isIndex);

        যদি( type == USPREP_PROHIBITED || 
            ((result < _SPREP_TYPE_THRESHOLD) && (result & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *status = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2Index-U16_LENGTH(ch), b2Len, parseError);
            ঘূৰাই 0;
        }

        যদি(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            যদি(firstCharDir == U_CHAR_DIRECTION_গুনতি){
                firstCharDir = direction;
            }
            যদি(direction == U_বাম_TO_ডান){
                বাম_To_ডান = সঁচা;
                ltrPos = b2Index-1;
            }
            যদি(direction == U_ডান_TO_বাম || direction == U_ডান_TO_বাম_ARABIC){
                ডান_To_বাম = সঁচা;
                rtlPos = b2Index-1;
            }
        }
    }
    যদি(profile->checkBiDi){
        // satisfy 2
        যদি( বাম_To_ডান && ডান_To_বাম){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            ঘূৰাই 0;
        }

        //satisfy 3
        যদি( ডান_To_বাম && 
            !((firstCharDir == U_ডান_TO_বাম || firstCharDir == U_ডান_TO_বাম_ARABIC) &&
              (direction == U_ডান_TO_বাম || direction == U_ডান_TO_বাম_ARABIC))
           ){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            ঘূৰাই মিছা;
        }
    }
    ঘূৰাই s2.extract(dest, destCapacity, *status);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_swap(ধ্রুৱ UDataSwapper *ds,
            ধ্রুৱ শূন্য *inData, int32_t length, শূন্য *outData,
            UErrorCode *pErrorCode) {
    ধ্রুৱ UDataInfo *pInfo;
    int32_t headerSize;

    ধ্রুৱ uint8_t *inBytes;
    uint8_t *outBytes;

    ধ্রুৱ int32_t *inIndexes;
    int32_t indexes[16];

    int32_t i, offset, count, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }

    /* check data format and format version */
    pInfo=(ধ্রুৱ UDataInfo *)((ধ্রুৱ ৱর্ণ *)inData+4);
    যদি(!(
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
        ঘূৰাই 0;
    }

    inBytes=(ধ্রুৱ uint8_t *)inData+headerSize;
    outBytes= (outData == nullptr ) ? nullptr : (uint8_t *)outData+headerSize;

    inIndexes=(ধ্রুৱ int32_t *)inBytes;

    যদি(length>=0) {
        length-=headerSize;
        যদি(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ঘূৰাই 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    বাবে(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[_SPREP_INDEX_TRIE_SIZE]+
        indexes[_SPREP_INDEX_MAPPING_DATA_SIZE];

    যদি(length>=0) {
        যদি(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ঘূৰাই 0;
        }

        /* copy the data for inaccessible bytes */
        যদি(inBytes!=outBytes) {
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

    ঘূৰাই headerSize+size;
}

#endif /* #if !UCONFIG_NO_IDNA */
