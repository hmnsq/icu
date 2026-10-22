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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_IDNA

#समावेश "unicode/usprep.h"

#समावेश "unicode/normalizer2.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/uversion.h"
#समावेश "umutex.h"
#समावेश "cmemory.h"
#समावेश "sprpimpl.h"
#समावेश "ustr_imp.h"
#समावेश "uhash.h"
#समावेश "cstring.h"
#समावेश "udataswp.h"
#समावेश "ucln_cmn.h"
#समावेश "ubidi_props.h"
#समावेश "uprops.h"

U_NAMESPACE_USE

U_CDECL_BEGIN

/*
Static cache for already opened StringPrep profiles
*/
अटल UHashtable *SHARED_तथ्य_HASHTABLE = nullptr;
अटल icu::UInitOnce g_Shared_तथ्य_Init_Once {};

अटल UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
अटल UVersionInfo तथ्य_Version={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
अटल स्थिर अक्षर * स्थिर PROFILE_NAMES[] = {
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

अटल UBool U_CALLCONV
isSPrepAcceptable(व्योम * /* context */,
             स्थिर अक्षर * /* type */, 
             स्थिर अक्षर * /* name */,
             स्थिर UDataInfo *pInfo) {
    अगर(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->तथ्य_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->तथ्य_Format[1]==0x50 &&
        pInfo->तथ्य_Format[2]==0x52 &&
        pInfo->तथ्य_Format[3]==0x50 &&
        pInfo->formatVersion[0]==3 &&
        pInfo->formatVersion[2]==UTRIE_SHIFT &&
        pInfo->formatVersion[3]==UTRIE_सूचकांक_SHIFT
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(तथ्य_Version, pInfo->तथ्य_Version, 4);
        वापस true;
    } अन्यथा {
        वापस false;
    }
}

अटल int32_t U_CALLCONV
getSPrepFoldingOffset(uint32_t तथ्य) {

    वापस (int32_t)तथ्य;

}

/* hashes an entry  */
अटल int32_t U_CALLCONV 
hashEntry(स्थिर UHashTok parm) {
    UString_Prep_कुंजी *b = (UString_Prep_कुंजी *)parm.pointer;
    UHashTok namekey, pathkey;
    namekey.pointer = b->name;
    pathkey.pointer = b->path;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    वापस static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
अटल UBool U_CALLCONV 
compareEntries(स्थिर UHashTok p1, स्थिर UHashTok p2) {
    UString_Prep_कुंजी *b1 = (UString_Prep_कुंजी *)p1.pointer;
    UString_Prep_कुंजी *b2 = (UString_Prep_कुंजी *)p2.pointer;
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->name;
    name2.pointer = b2->name;
    path1.pointer = b1->path;
    path2.pointer = b2->path;
    वापस uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

अटल व्योम 
usprep_unload(UStringPrepProfile* तथ्य){
    udata_close(तथ्य->sprep_तथ्य);
}

अटल int32_t 
usprep_internal_flushCache(UBool no_Ref_गिनती){
    UStringPrepProfile *profile = nullptr;
    UString_Prep_कुंजी  *कुंजी  = nullptr;
    int32_t pos = UHASH_FIRST;
    int32_t deleted_अंक = 0;
    स्थिर UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_lock(&usprepMutex);
    अगर (SHARED_तथ्य_HASHTABLE == nullptr) {
        umtx_unlock(&usprepMutex);
        वापस 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    जबतक ((e = uhash_अग्र_Element(SHARED_तथ्य_HASHTABLE, &pos)) != nullptr)
    {
        profile = (UStringPrepProfile *) e->value.pointer;
        कुंजी  = (UString_Prep_कुंजी *) e->कुंजी.pointer;

        अगर ((no_Ref_गिनती== false && profile->ref_गिनती == 0) || 
             no_Ref_गिनती) {
            deleted_अंक++;
            uhash_removeElement(SHARED_तथ्य_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            अगर(कुंजी->name != nullptr) {
                uprv_free(कुंजी->name);
                कुंजी->name=nullptr;
            }
            अगर(कुंजी->path != nullptr) {
                uprv_free(कुंजी->path);
                कुंजी->path=nullptr;
            }
            uprv_free(profile);
            uprv_free(कुंजी);
        }
       
    }
    umtx_unlock(&usprepMutex);

    वापस deleted_अंक;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

अटल UBool U_CALLCONV usprep_cleanup(){
    अगर (SHARED_तथ्य_HASHTABLE != nullptr) {
        usprep_internal_flushCache(true);
        अगर (SHARED_तथ्य_HASHTABLE != nullptr && uhash_गिनती(SHARED_तथ्य_HASHTABLE) == 0) {
            uhash_close(SHARED_तथ्य_HASHTABLE);
            SHARED_तथ्य_HASHTABLE = nullptr;
        }
    }
    g_Shared_तथ्य_Init_Once.reset();
    वापस (SHARED_तथ्य_HASHTABLE == nullptr);
}
U_CDECL_END


/** Initializes the cache for resources */
अटल व्योम U_CALLCONV
createCache(UErrorCode &status) {
    SHARED_तथ्य_HASHTABLE = uhash_open(hashEntry, compareEntries, nullptr, &status);
    अगर (U_FAILURE(status)) {
        SHARED_तथ्य_HASHTABLE = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

अटल व्योम 
initCache(UErrorCode *status) {
    umtx_initOnce(g_Shared_तथ्य_Init_Once, &createCache, *status);
}

अटल UBool U_CALLCONV
load_तथ्य(UStringPrepProfile* profile, 
         स्थिर अक्षर* path, 
         स्थिर अक्षर* name, 
         स्थिर अक्षर* type, 
         UErrorCode* errorCode) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {nullptr, nullptr, nullptr, 0, 0, 0, 0};
    UDataMemory *तथ्य_Memory;
    स्थिर int32_t *p=nullptr;
    स्थिर uint8_t *pb;
    UVersionInfo normUnicodeVersion;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    अगर(errorCode==nullptr || U_FAILURE(*errorCode)) {
        वापस 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    तथ्य_Memory=udata_openChoice(path, type, name, isSPrepAcceptable, nullptr, errorCode);
    अगर(U_FAILURE(*errorCode)) {
        वापस false;
    }

    p = static_cast<स्थिर int32_t*>(udata_getMemory(तथ्य_Memory));
    pb = reinterpret_cast<स्थिर uint8_t*>(p + SPREP_सूचकांक_शीर्ष);
    utrie_unserialize(&_sprepTrie, pb, p[SPREP_सूचकांक_TRIE_आकार], errorCode);
    _sprepTrie.getFoldingOffset=getSPrepFoldingOffset;


    अगर(U_FAILURE(*errorCode)) {
        udata_close(तथ्य_Memory);
        वापस false;
    }

    /* in the mutex block, set the data for this process */
    umtx_lock(&usprepMutex);
    अगर(profile->sprep_तथ्य==nullptr) {
        profile->sprep_तथ्य=तथ्य_Memory;
        तथ्य_Memory=nullptr;
        uprv_memcpy(&profile->indexes, p, माप(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, माप(UTrie));
    } अन्यथा {
        p = static_cast<स्थिर int32_t*>(udata_getMemory(profile->sprep_तथ्य));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mapping_तथ्य = reinterpret_cast<स्थिर uint16_t*>(reinterpret_cast<स्थिर uint8_t*>(p + SPREP_सूचकांक_शीर्ष) + profile->indexes[SPREP_सूचकांक_TRIE_आकार]);
    
    u_getUnicodeVersion(normUnicodeVersion);
    normUniVer = (normUnicodeVersion[0] << 24) + (normUnicodeVersion[1] << 16) + 
                 (normUnicodeVersion[2] << 8 ) + (normUnicodeVersion[3]);
    sprepUniVer = (तथ्य_Version[0] << 24) + (तथ्य_Version[1] << 16) + 
                  (तथ्य_Version[2] << 8 ) + (तथ्य_Version[3]);
    normCorrVer = profile->indexes[_SPREP_NORM_CORRECTNS_LAST_UNI_VERSION];
    
    अगर(U_FAILURE(*errorCode)){
        udata_close(तथ्य_Memory);
        वापस false;
    }
    अगर( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *errorCode = U_INVALID_FORMAT_ERROR;
        udata_close(तथ्य_Memory);
        वापस false;
    }
    profile->is_तथ्य_Loaded = true;

    /* if a different thread set it first, then close the extra data */
    अगर(तथ्य_Memory!=nullptr) {
        udata_close(तथ्य_Memory); /* nullptr if it was set correctly */
    }


    वापस profile->is_तथ्य_Loaded;
}

अटल UStringPrepProfile* 
usprep_getProfile(स्थिर अक्षर* path, 
                  स्थिर अक्षर* name,
                  UErrorCode *status){

    UStringPrepProfile* profile = nullptr;

    initCache(status);

    अगर(U_FAILURE(*status)){
        वापस nullptr;
    }

    UString_Prep_कुंजी ढेर_कुंजी;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    ढेर_कुंजी.name = const_cast<अक्षर*>(name);
    ढेर_कुंजी.path = const_cast<अक्षर*>(path);

    /* fetch the data from the cache */
    umtx_lock(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_तथ्य_HASHTABLE, &ढेर_कुंजी));
    अगर(profile != nullptr) {
        profile->ref_गिनती++;
    }
    umtx_unlock(&usprepMutex);
    
    अगर(profile == nullptr) {
        /* else load the data and put the data in the cache */
        LocalMemory<UStringPrepProfile> newProfile;
        अगर(newProfile.allocateInsteadAndReset() == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            वापस nullptr;
        }

        /* load the data */
        अगर(!load_तथ्य(newProfile.getAlias(), path, name, SPREP_तथ्य_TYPE, status) || U_FAILURE(*status) ){
            वापस nullptr;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        LocalMemory<UString_Prep_कुंजी> कुंजी;
        LocalMemory<अक्षर> कुंजी_Name;
        LocalMemory<अक्षर> कुंजी_Path;
        अगर( कुंजी.allocateInsteadAndReset() == nullptr ||
            कुंजी_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == nullptr ||
            (path != nullptr &&
             कुंजी_Path.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(path)+1)) == nullptr)
         ) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            usprep_unload(newProfile.getAlias());
            वापस nullptr;
        }

        umtx_lock(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_तथ्य_HASHTABLE, &ढेर_कुंजी));
        अगर(profile != nullptr) {
            profile->ref_गिनती++;
            usprep_unload(newProfile.getAlias());
        }
        अन्यथा {
            /* initialize the key members */
            कुंजी->name = कुंजी_Name.orphan();
            uprv_strcpy(कुंजी->name, name);
            अगर(path != nullptr){
                कुंजी->path = कुंजी_Path.orphan();
                uprv_strcpy(कुंजी->path, path);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_गिनती = 1;
            uhash_put(SHARED_तथ्य_HASHTABLE, कुंजी.orphan(), profile, status);
        }
        umtx_unlock(&usprepMutex);
    }

    वापस profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(स्थिर अक्षर* path, 
            स्थिर अक्षर* name,
            UErrorCode* status){

    अगर(status == nullptr || U_FAILURE(*status)){
        वापस nullptr;
    }
       
    /* initialize the profile struct members */
    वापस usprep_getProfile(path,name,status);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UErrorCode* status) {
    अगर(status == nullptr || U_FAILURE(*status)){
        वापस nullptr;
    }
    int32_t सूचकांक = (int32_t)type;
    अगर (सूचकांक < 0 || सूचकांक >= UPRV_LENGTHOF(PROFILE_NAMES)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    वापस usprep_open(nullptr, PROFILE_NAMES[सूचकांक], status);
}

U_CAPI व्योम U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    अगर(profile==nullptr){
        वापस;
    }

    umtx_lock(&usprepMutex);
    /* decrement the ref count*/
    अगर(profile->ref_गिनती > 0){
        profile->ref_गिनती--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC व्योम 
uprv_syntaxError(स्थिर char16_t* rules,
                 int32_t pos,
                 int32_t rulesLen,
                 UParseError* parseError){
    अगर(parseError == nullptr){
        वापस;
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
    अगर (limit > rulesLen) {
        limit = rulesLen;
    }
    अगर (start < rulesLen) {
        u_memcpy(parseError->postContext,rules+start,limit-start);
    }
    //null terminate the buffer
    parseError->postContext[limit-start]= 0;
}


अटल अंतरभूत UStringPrepType
get_मूल्यावली(uint16_t trieWord, int16_t& value, UBool& is_सूचकांक){

    UStringPrepType type;
    अगर(trieWord == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_LIMIT;
        is_सूचकांक =false;
        value = 0;
    }अन्यथा अगर(trieWord >= _SPREP_TYPE_THRESHOLD){
        type = static_cast<UStringPrepType>(trieWord - _SPREP_TYPE_THRESHOLD);
        is_सूचकांक =false;
        value = 0;
    }अन्यथा{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        अगर(trieWord & 0x02){
            is_सूचकांक = true;
            value = trieWord  >> 2; //mask off the lower 2 bits and shift
        }अन्यथा{
            is_सूचकांक = false;
            value = static_cast<int16_t>(trieWord);
            value =  (value >> 2);
        }
 
        अगर((trieWord>>2) == SPREP_MAX_सूचकांक_मूल्य){
            type = USPREP_DELETE;
            is_सूचकांक =false;
            value = 0;
        }
    }
    वापस type;
}

// TODO: change to writing to UnicodeString not char16_t *
अटल int32_t 
usprep_map(  स्थिर UStringPrepProfile* profile, 
             स्थिर char16_t* src, int32_t src_लंबाई,
             char16_t* dest, int32_t destCapacity,
             int32_t options,
             UParseError* parseError,
             UErrorCode* status ){
    
    uint16_t परिणाम;
    int32_t dest_सूचकांक=0;
    int32_t src_सूचकांक;
    UBool allowUnassigned = static_cast<UBool>((options & USPREP_ALLOW_UNASSIGNED) > 0);
    UStringPrepType type;
    int16_t value;
    UBool is_सूचकांक;
    स्थिर int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    क्रम(src_सूचकांक=0;src_सूचकांक<src_लंबाई;){
        UChar32 ch;

        U16_अग्र(src,src_सूचकांक,src_लंबाई,ch);
        
        परिणाम=0;

        UTRIE_GET16(&profile->sprepTrie,ch,परिणाम);
        
        type = get_मूल्यावली(परिणाम, value, is_सूचकांक);

        // check if the source codepoint is unassigned
        अगर(type == USPREP_UNASSIGNED && allowUnassigned == false){

            uprv_syntaxError(src,src_सूचकांक-U16_लंबाई(ch), src_लंबाई,parseError);
            *status = U_STRINGPREP_UNASSIGNED_ERROR;
            वापस 0;
            
        }अन्यथा अगर(type == USPREP_MAP){
            
            int32_t सूचकांक, length;

            अगर(is_सूचकांक){
                सूचकांक = value;
                अगर(सूचकांक >= indexes[SPREP_ONE_UCHAR_MAPPING_सूचकांक_START] &&
                         सूचकांक < indexes[SPREP_TWO_UCHARS_MAPPING_सूचकांक_START]){
                    length = 1;
                }अन्यथा अगर(सूचकांक >= indexes[SPREP_TWO_UCHARS_MAPPING_सूचकांक_START] &&
                         सूचकांक < indexes[SPREP_THREE_UCHARS_MAPPING_सूचकांक_START]){
                    length = 2;
                }अन्यथा अगर(सूचकांक >= indexes[SPREP_THREE_UCHARS_MAPPING_सूचकांक_START] &&
                         सूचकांक < indexes[SPREP_FOUR_UCHARS_MAPPING_सूचकांक_START]){
                    length = 3;
                }अन्यथा{
                    length = profile->mapping_तथ्य[सूचकांक++];
         
                }

                /* copy mapping to destination */
                क्रम(int32_t i=0; i< length; i++){
                    अगर(dest_सूचकांक < destCapacity  ){
                        dest[dest_सूचकांक] = profile->mapping_तथ्य[सूचकांक+i];
                    }
                    dest_सूचकांक++; /* for pre-flighting */
                }  
                जारी;
            }अन्यथा{
                // subtract the delta to arrive at the code point
                ch -= value;
            }

        }अन्यथा अगर(type==USPREP_DELETE){
             // just consume the codepoint and continue
            जारी;
        }
        //copy the code point into destination
        अगर(ch <= 0xFFFF){
            अगर(dest_सूचकांक < destCapacity ){
                dest[dest_सूचकांक] = static_cast<char16_t>(ch);
            }
            dest_सूचकांक++;
        }अन्यथा{
            अगर(dest_सूचकांक+1 < destCapacity ){
                dest[dest_सूचकांक]   = U16_LEAD(ch);
                dest[dest_सूचकांक+1] = U16_TRAIL(ch);
            }
            dest_सूचकांक +=2;
        }
       
    }
        
    वापस u_terminateUChars(dest, destCapacity, dest_सूचकांक, status);
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
usprep_prepare(   स्थिर UStringPrepProfile* profile,
                  स्थिर char16_t* src, int32_t src_लंबाई,
                  char16_t* dest, int32_t destCapacity,
                  int32_t options,
                  UParseError* parseError,
                  UErrorCode* status ){

    // check error status
    अगर(U_FAILURE(*status)){
        वापस 0;
    }

    //check arguments
    अगर(profile==nullptr ||
            (src==nullptr ? src_लंबाई!=0 : src_लंबाई<-1) ||
            (dest==nullptr ? destCapacity!=0 : destCapacity<0)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    //get the string length
    अगर(src_लंबाई < 0){
        src_लंबाई = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.get_बफर(src_लंबाई);
    अगर(b1==nullptr){
        *status = U_MEMORY_ALLOCATION_ERROR;
        वापस 0;
    }
    UErrorCode बफर_Status = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, src_लंबाई,
                               b1, s1.getCapacity(), options, parseError, &बफर_Status);
    s1.release_बफर(U_SUCCESS(बफर_Status) ? b1Len : 0);

    अगर(बफर_Status == U_बफर_OVERFLOW_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.get_बफर(b1Len);
        अगर(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            वापस 0;
        }

        बफर_Status = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, src_लंबाई,
                           b1, s1.getCapacity(), options, parseError, &बफर_Status);
        s1.release_बफर(U_SUCCESS(बफर_Status) ? b1Len : 0);
    }
    अगर(U_FAILURE(बफर_Status)){
        *status = बफर_Status;
        वापस 0;
    }

    // normalize
    UnicodeString s2;
    अगर(profile->doNFKC){
        स्थिर Normalizer2 *n2 = Normalizer2::getNFKCInstance(*status);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*status));
        अगर(U_FAILURE(*status)){
            वापस 0;
        }
        fn2.normalize(s1, s2, *status);
    }अन्यथा{
        s2.fastCopyFrom(s1);
    }
    अगर(U_FAILURE(*status)){
        वापस 0;
    }

    // Prohibit and checkBiDi in one pass
    स्थिर char16_t *b2 = s2.get_बफर();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_गिनती, firstCharDir=U_CHAR_DIRECTION_गिनती;
    UBool बायाँ_To_दायाँ=false, दायाँ_To_बायाँ=false;
    int32_t rtlPos =-1, ltrPos =-1;

    क्रम(int32_t b2_सूचकांक=0; b2_सूचकांक<b2Len;){
        UChar32 ch = 0;
        U16_अग्र(b2, b2_सूचकांक, b2Len, ch);

        uint16_t परिणाम;
        UTRIE_GET16(&profile->sprepTrie,ch,परिणाम);

        int16_t value;
        UBool is_सूचकांक;
        UStringPrepType type = get_मूल्यावली(परिणाम, value, is_सूचकांक);

        अगर( type == USPREP_PROHIBITED || 
            ((परिणाम < _SPREP_TYPE_THRESHOLD) && (परिणाम & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *status = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2_सूचकांक-U16_लंबाई(ch), b2Len, parseError);
            वापस 0;
        }

        अगर(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            अगर(firstCharDir == U_CHAR_DIRECTION_गिनती){
                firstCharDir = direction;
            }
            अगर(direction == U_बायाँ_TO_दायाँ){
                बायाँ_To_दायाँ = true;
                ltrPos = b2_सूचकांक-1;
            }
            अगर(direction == U_दायाँ_TO_बायाँ || direction == U_दायाँ_TO_बायाँ_ARABIC){
                दायाँ_To_बायाँ = true;
                rtlPos = b2_सूचकांक-1;
            }
        }
    }
    अगर(profile->checkBiDi){
        // satisfy 2
        अगर( बायाँ_To_दायाँ && दायाँ_To_बायाँ){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            वापस 0;
        }

        //satisfy 3
        अगर( दायाँ_To_बायाँ && 
            !((firstCharDir == U_दायाँ_TO_बायाँ || firstCharDir == U_दायाँ_TO_बायाँ_ARABIC) &&
              (direction == U_दायाँ_TO_बायाँ || direction == U_दायाँ_TO_बायाँ_ARABIC))
           ){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            वापस false;
        }
    }
    वापस s2.extract(dest, destCapacity, *status);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_swap(स्थिर UDataSwapper *ds,
            स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
            UErrorCode *pErrorCode) {
    स्थिर UDataInfo *pInfo;
    int32_t header_आकार;

    स्थिर uint8_t *inBytes;
    uint8_t *outBytes;

    स्थिर int32_t *inIndexes;
    int32_t indexes[16];

    int32_t i, offset, count, size;

    /* udata_swapDataHeader checks the arguments */
    header_आकार=udata_swap_तथ्य_Header(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* check data format and format version */
    pInfo=(स्थिर UDataInfo *)((स्थिर अक्षर *)in_तथ्य+4);
    अगर(!(
        pInfo->तथ्य_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->तथ्य_Format[1]==0x50 &&
        pInfo->तथ्य_Format[2]==0x52 &&
        pInfo->तथ्य_Format[3]==0x50 &&
        pInfo->formatVersion[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->तथ्य_Format[0], pInfo->तथ्य_Format[1],
                         pInfo->तथ्य_Format[2], pInfo->तथ्य_Format[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    inBytes=(स्थिर uint8_t *)in_तथ्य+header_आकार;
    outBytes= (out_तथ्य == nullptr ) ? nullptr : (uint8_t *)out_तथ्य+header_आकार;

    inIndexes=(स्थिर int32_t *)inBytes;

    अगर(length>=0) {
        length-=header_आकार;
        अगर(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    क्रम(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[SPREP_सूचकांक_TRIE_आकार]+
        indexes[SPREP_सूचकांक_MAPPING_तथ्य_आकार];

    अगर(length>=0) {
        अगर(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }

        /* copy the data for inaccessible bytes */
        अगर(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset=0;

        /* swap the int32_t indexes[] */
        count=16*4;
        ds->swapArray32(ds, inBytes, count, outBytes, pErrorCode);
        offset+=count;

        /* swap the UTrie */
        count=indexes[SPREP_सूचकांक_TRIE_आकार];
        utrie_swap(ds, inBytes+offset, count, outBytes+offset, pErrorCode);
        offset+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[SPREP_सूचकांक_MAPPING_तथ्य_आकार];
        ds->swapArray16(ds, inBytes+offset, count, outBytes+offset, pErrorCode);
        //offset+=count;
    }

    वापस header_आकार+size;
}

#पूर्ण_अगर /* #if !UCONFIG_NO_IDNA */
