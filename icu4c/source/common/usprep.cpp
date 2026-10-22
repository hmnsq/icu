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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_IDNA

#অন্তর্ভাব "unicode/usprep.h"

#অন্তর্ভাব "unicode/normalizer2.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/uversion.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "sprpimpl.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "udataswp.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "ubidi_props.h"
#অন্তর্ভাব "uprops.h"

U_NAMESPACE_USE

U_CDECL_BEGIN

/*
Static cache for already opened StringPrep profiles
*/
স্থির UHashtable *SHARED_তথ্য_HASHTABLE = nullptr;
স্থির icu::UInitOnce g_Shared_তথ্য_Init_Once {};

স্থির UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
স্থির UVersionInfo তথ্য_Version={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
স্থির ধ্রুবক অক্ষর * ধ্রুবক PROFILE_NAMES[] = {
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
             ধ্রুবক অক্ষর * /* type */, 
             ধ্রুবক অক্ষর * /* name */,
             ধ্রুবক UDataInfo *pInfo) {
    যদি(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->তথ্য_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->তথ্য_Format[1]==0x50 &&
        pInfo->তথ্য_Format[2]==0x52 &&
        pInfo->তথ্য_Format[3]==0x50 &&
        pInfo->formatVersion[0]==3 &&
        pInfo->formatVersion[2]==UTRIE_SHIFT &&
        pInfo->formatVersion[3]==UTRIE_সূচক_SHIFT
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(তথ্য_Version, pInfo->তথ্য_Version, 4);
        ফেরত সত্য;
    } অন্যথা {
        ফেরত মিথ্যা;
    }
}

স্থির int32_t U_CALLCONV
getSPrepFoldingOffset(uint32_t তথ্য) {

    ফেরত (int32_t)তথ্য;

}

/* hashes an entry  */
স্থির int32_t U_CALLCONV 
hashEntry(ধ্রুবক UHashTok parm) {
    UString_Prep_চাবি *b = (UString_Prep_চাবি *)parm.pointer;
    UHashTok namekey, pathkey;
    namekey.pointer = b->name;
    pathkey.pointer = b->path;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    ফেরত static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
স্থির UBool U_CALLCONV 
compareEntries(ধ্রুবক UHashTok p1, ধ্রুবক UHashTok p2) {
    UString_Prep_চাবি *b1 = (UString_Prep_চাবি *)p1.pointer;
    UString_Prep_চাবি *b2 = (UString_Prep_চাবি *)p2.pointer;
    UHashTok name1, name2, path1, path2;
    name1.pointer = b1->name;
    name2.pointer = b2->name;
    path1.pointer = b1->path;
    path2.pointer = b2->path;
    ফেরত uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

স্থির শূন্য 
usprep_unload(UStringPrepProfile* তথ্য){
    udata_close(তথ্য->sprep_তথ্য);
}

স্থির int32_t 
usprep_internal_flushCache(UBool no_Ref_গুনতি){
    UStringPrepProfile *profile = nullptr;
    UString_Prep_চাবি  *চাবি  = nullptr;
    int32_t pos = UHASH_FIRST;
    int32_t deleted_অঙ্ক = 0;
    ধ্রুবক UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_lock(&usprepMutex);
    যদি (SHARED_তথ্য_HASHTABLE == nullptr) {
        umtx_unlock(&usprepMutex);
        ফেরত 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    যখন ((e = uhash_পরবর্তী_Element(SHARED_তথ্য_HASHTABLE, &pos)) != nullptr)
    {
        profile = (UStringPrepProfile *) e->value.pointer;
        চাবি  = (UString_Prep_চাবি *) e->চাবি.pointer;

        যদি ((no_Ref_গুনতি== মিথ্যা && profile->ref_গুনতি == 0) || 
             no_Ref_গুনতি) {
            deleted_অঙ্ক++;
            uhash_removeElement(SHARED_তথ্য_HASHTABLE, e);

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

    ফেরত deleted_অঙ্ক;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

স্থির UBool U_CALLCONV usprep_cleanup(){
    যদি (SHARED_তথ্য_HASHTABLE != nullptr) {
        usprep_internal_flushCache(সত্য);
        যদি (SHARED_তথ্য_HASHTABLE != nullptr && uhash_গুনতি(SHARED_তথ্য_HASHTABLE) == 0) {
            uhash_close(SHARED_তথ্য_HASHTABLE);
            SHARED_তথ্য_HASHTABLE = nullptr;
        }
    }
    g_Shared_তথ্য_Init_Once.reset();
    ফেরত (SHARED_তথ্য_HASHTABLE == nullptr);
}
U_CDECL_END


/** Initializes the cache for resources */
স্থির শূন্য U_CALLCONV
createCache(UErrorCode &status) {
    SHARED_তথ্য_HASHTABLE = uhash_open(hashEntry, compareEntries, nullptr, &status);
    যদি (U_FAILURE(status)) {
        SHARED_তথ্য_HASHTABLE = nullptr;
    }
    ucln_common_registerCleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

স্থির শূন্য 
initCache(UErrorCode *status) {
    umtx_initOnce(g_Shared_তথ্য_Init_Once, &createCache, *status);
}

স্থির UBool U_CALLCONV
load_তথ্য(UStringPrepProfile* profile, 
         ধ্রুবক অক্ষর* path, 
         ধ্রুবক অক্ষর* name, 
         ধ্রুবক অক্ষর* type, 
         UErrorCode* errorCode) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {nullptr, nullptr, nullptr, 0, 0, 0, 0};
    UDataMemory *তথ্য_Memory;
    ধ্রুবক int32_t *p=nullptr;
    ধ্রুবক uint8_t *pb;
    UVersionInfo normUnicodeVersion;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    যদি(errorCode==nullptr || U_FAILURE(*errorCode)) {
        ফেরত 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    তথ্য_Memory=udata_openChoice(path, type, name, isSPrepAcceptable, nullptr, errorCode);
    যদি(U_FAILURE(*errorCode)) {
        ফেরত মিথ্যা;
    }

    p = static_cast<ধ্রুবক int32_t*>(udata_getMemory(তথ্য_Memory));
    pb = reinterpret_cast<ধ্রুবক uint8_t*>(p + SPREP_সূচক_শীর্ষ);
    utrie_unserialize(&_sprepTrie, pb, p[SPREP_সূচক_TRIE_আকার], errorCode);
    _sprepTrie.getFoldingOffset=getSPrepFoldingOffset;


    যদি(U_FAILURE(*errorCode)) {
        udata_close(তথ্য_Memory);
        ফেরত মিথ্যা;
    }

    /* in the mutex block, set the data for this process */
    umtx_lock(&usprepMutex);
    যদি(profile->sprep_তথ্য==nullptr) {
        profile->sprep_তথ্য=তথ্য_Memory;
        তথ্য_Memory=nullptr;
        uprv_memcpy(&profile->indexes, p, আকার(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, আকার(UTrie));
    } অন্যথা {
        p = static_cast<ধ্রুবক int32_t*>(udata_getMemory(profile->sprep_তথ্য));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mapping_তথ্য = reinterpret_cast<ধ্রুবক uint16_t*>(reinterpret_cast<ধ্রুবক uint8_t*>(p + SPREP_সূচক_শীর্ষ) + profile->indexes[SPREP_সূচক_TRIE_আকার]);
    
    u_getUnicodeVersion(normUnicodeVersion);
    normUniVer = (normUnicodeVersion[0] << 24) + (normUnicodeVersion[1] << 16) + 
                 (normUnicodeVersion[2] << 8 ) + (normUnicodeVersion[3]);
    sprepUniVer = (তথ্য_Version[0] << 24) + (তথ্য_Version[1] << 16) + 
                  (তথ্য_Version[2] << 8 ) + (তথ্য_Version[3]);
    normCorrVer = profile->indexes[_SPREP_NORM_CORRECTNS_LAST_UNI_VERSION];
    
    যদি(U_FAILURE(*errorCode)){
        udata_close(তথ্য_Memory);
        ফেরত মিথ্যা;
    }
    যদি( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *errorCode = U_INVALID_FORMAT_ERROR;
        udata_close(তথ্য_Memory);
        ফেরত মিথ্যা;
    }
    profile->is_তথ্য_Loaded = সত্য;

    /* if a different thread set it first, then close the extra data */
    যদি(তথ্য_Memory!=nullptr) {
        udata_close(তথ্য_Memory); /* nullptr if it was set correctly */
    }


    ফেরত profile->is_তথ্য_Loaded;
}

স্থির UStringPrepProfile* 
usprep_getProfile(ধ্রুবক অক্ষর* path, 
                  ধ্রুবক অক্ষর* name,
                  UErrorCode *status){

    UStringPrepProfile* profile = nullptr;

    initCache(status);

    যদি(U_FAILURE(*status)){
        ফেরত nullptr;
    }

    UString_Prep_চাবি স্তূপ_চাবি;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    স্তূপ_চাবি.name = const_cast<অক্ষর*>(name);
    স্তূপ_চাবি.path = const_cast<অক্ষর*>(path);

    /* fetch the data from the cache */
    umtx_lock(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_তথ্য_HASHTABLE, &স্তূপ_চাবি));
    যদি(profile != nullptr) {
        profile->ref_গুনতি++;
    }
    umtx_unlock(&usprepMutex);
    
    যদি(profile == nullptr) {
        /* else load the data and put the data in the cache */
        LocalMemory<UStringPrepProfile> newProfile;
        যদি(newProfile.allocateInsteadAndReset() == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত nullptr;
        }

        /* load the data */
        যদি(!load_তথ্য(newProfile.getAlias(), path, name, SPREP_তথ্য_TYPE, status) || U_FAILURE(*status) ){
            ফেরত nullptr;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        LocalMemory<UString_Prep_চাবি> চাবি;
        LocalMemory<অক্ষর> চাবি_Name;
        LocalMemory<অক্ষর> চাবি_Path;
        যদি( চাবি.allocateInsteadAndReset() == nullptr ||
            চাবি_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == nullptr ||
            (path != nullptr &&
             চাবি_Path.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(path)+1)) == nullptr)
         ) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            usprep_unload(newProfile.getAlias());
            ফেরত nullptr;
        }

        umtx_lock(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_তথ্য_HASHTABLE, &স্তূপ_চাবি));
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
            uhash_put(SHARED_তথ্য_HASHTABLE, চাবি.orphan(), profile, status);
        }
        umtx_unlock(&usprepMutex);
    }

    ফেরত profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(ধ্রুবক অক্ষর* path, 
            ধ্রুবক অক্ষর* name,
            UErrorCode* status){

    যদি(status == nullptr || U_FAILURE(*status)){
        ফেরত nullptr;
    }
       
    /* initialize the profile struct members */
    ফেরত usprep_getProfile(path,name,status);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UErrorCode* status) {
    যদি(status == nullptr || U_FAILURE(*status)){
        ফেরত nullptr;
    }
    int32_t সূচক = (int32_t)type;
    যদি (সূচক < 0 || সূচক >= UPRV_LENGTHOF(PROFILE_NAMES)) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    ফেরত usprep_open(nullptr, PROFILE_NAMES[সূচক], status);
}

U_CAPI শূন্য U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    যদি(profile==nullptr){
        ফেরত;
    }

    umtx_lock(&usprepMutex);
    /* decrement the ref count*/
    যদি(profile->ref_গুনতি > 0){
        profile->ref_গুনতি--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC শূন্য 
uprv_syntaxError(ধ্রুবক char16_t* rules,
                 int32_t pos,
                 int32_t rulesLen,
                 UParseError* parseError){
    যদি(parseError == nullptr){
        ফেরত;
    }
    parseError->offset = pos;
    parseError->সারি = 0 ; // we are not using line numbers 
    
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


স্থির অন্তঃরেখ UStringPrepType
get_মানসমূহ(uint16_t trieWord, int16_t& value, UBool& is_সূচক){

    UStringPrepType type;
    যদি(trieWord == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_LIMIT;
        is_সূচক =মিথ্যা;
        value = 0;
    }অন্যথা যদি(trieWord >= _SPREP_TYPE_THRESHOLD){
        type = static_cast<UStringPrepType>(trieWord - _SPREP_TYPE_THRESHOLD);
        is_সূচক =মিথ্যা;
        value = 0;
    }অন্যথা{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        যদি(trieWord & 0x02){
            is_সূচক = সত্য;
            value = trieWord  >> 2; //mask off the lower 2 bits and shift
        }অন্যথা{
            is_সূচক = মিথ্যা;
            value = static_cast<int16_t>(trieWord);
            value =  (value >> 2);
        }
 
        যদি((trieWord>>2) == SPREP_MAX_সূচক_মান){
            type = USPREP_DELETE;
            is_সূচক =মিথ্যা;
            value = 0;
        }
    }
    ফেরত type;
}

// TODO: change to writing to UnicodeString not char16_t *
স্থির int32_t 
usprep_map(  ধ্রুবক UStringPrepProfile* profile, 
             ধ্রুবক char16_t* src, int32_t src_দৈর্ঘ্য,
             char16_t* dest, int32_t destCapacity,
             int32_t options,
             UParseError* parseError,
             UErrorCode* status ){
    
    uint16_t ফলাফল;
    int32_t dest_সূচক=0;
    int32_t src_সূচক;
    UBool allowUnassigned = static_cast<UBool>((options & USPREP_ALLOW_UNASSIGNED) > 0);
    UStringPrepType type;
    int16_t value;
    UBool is_সূচক;
    ধ্রুবক int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    জন্য(src_সূচক=0;src_সূচক<src_দৈর্ঘ্য;){
        UChar32 ch;

        U16_পরবর্তী(src,src_সূচক,src_দৈর্ঘ্য,ch);
        
        ফলাফল=0;

        UTRIE_GET16(&profile->sprepTrie,ch,ফলাফল);
        
        type = get_মানসমূহ(ফলাফল, value, is_সূচক);

        // check if the source codepoint is unassigned
        যদি(type == USPREP_UNASSIGNED && allowUnassigned == মিথ্যা){

            uprv_syntaxError(src,src_সূচক-U16_দৈর্ঘ্য(ch), src_দৈর্ঘ্য,parseError);
            *status = U_STRINGPREP_UNASSIGNED_ERROR;
            ফেরত 0;
            
        }অন্যথা যদি(type == USPREP_MAP){
            
            int32_t সূচক, length;

            যদি(is_সূচক){
                সূচক = value;
                যদি(সূচক >= indexes[SPREP_ONE_UCHAR_MAPPING_সূচক_START] &&
                         সূচক < indexes[SPREP_TWO_UCHARS_MAPPING_সূচক_START]){
                    length = 1;
                }অন্যথা যদি(সূচক >= indexes[SPREP_TWO_UCHARS_MAPPING_সূচক_START] &&
                         সূচক < indexes[SPREP_THREE_UCHARS_MAPPING_সূচক_START]){
                    length = 2;
                }অন্যথা যদি(সূচক >= indexes[SPREP_THREE_UCHARS_MAPPING_সূচক_START] &&
                         সূচক < indexes[SPREP_FOUR_UCHARS_MAPPING_সূচক_START]){
                    length = 3;
                }অন্যথা{
                    length = profile->mapping_তথ্য[সূচক++];
         
                }

                /* copy mapping to destination */
                জন্য(int32_t i=0; i< length; i++){
                    যদি(dest_সূচক < destCapacity  ){
                        dest[dest_সূচক] = profile->mapping_তথ্য[সূচক+i];
                    }
                    dest_সূচক++; /* for pre-flighting */
                }  
                চালু;
            }অন্যথা{
                // subtract the delta to arrive at the code point
                ch -= value;
            }

        }অন্যথা যদি(type==USPREP_DELETE){
             // just consume the codepoint and continue
            চালু;
        }
        //copy the code point into destination
        যদি(ch <= 0xFFFF){
            যদি(dest_সূচক < destCapacity ){
                dest[dest_সূচক] = static_cast<char16_t>(ch);
            }
            dest_সূচক++;
        }অন্যথা{
            যদি(dest_সূচক+1 < destCapacity ){
                dest[dest_সূচক]   = U16_LEAD(ch);
                dest[dest_সূচক+1] = U16_TRAIL(ch);
            }
            dest_সূচক +=2;
        }
       
    }
        
    ফেরত u_terminateUChars(dest, destCapacity, dest_সূচক, status);
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
usprep_prepare(   ধ্রুবক UStringPrepProfile* profile,
                  ধ্রুবক char16_t* src, int32_t src_দৈর্ঘ্য,
                  char16_t* dest, int32_t destCapacity,
                  int32_t options,
                  UParseError* parseError,
                  UErrorCode* status ){

    // check error status
    যদি(U_FAILURE(*status)){
        ফেরত 0;
    }

    //check arguments
    যদি(profile==nullptr ||
            (src==nullptr ? src_দৈর্ঘ্য!=0 : src_দৈর্ঘ্য<-1) ||
            (dest==nullptr ? destCapacity!=0 : destCapacity<0)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    //get the string length
    যদি(src_দৈর্ঘ্য < 0){
        src_দৈর্ঘ্য = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.get_বাফার(src_দৈর্ঘ্য);
    যদি(b1==nullptr){
        *status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত 0;
    }
    UErrorCode বাফার_Status = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, src_দৈর্ঘ্য,
                               b1, s1.getCapacity(), options, parseError, &বাফার_Status);
    s1.release_বাফার(U_SUCCESS(বাফার_Status) ? b1Len : 0);

    যদি(বাফার_Status == U_বাফার_OVERFLOW_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.get_বাফার(b1Len);
        যদি(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত 0;
        }

        বাফার_Status = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, src_দৈর্ঘ্য,
                           b1, s1.getCapacity(), options, parseError, &বাফার_Status);
        s1.release_বাফার(U_SUCCESS(বাফার_Status) ? b1Len : 0);
    }
    যদি(U_FAILURE(বাফার_Status)){
        *status = বাফার_Status;
        ফেরত 0;
    }

    // normalize
    UnicodeString s2;
    যদি(profile->doNFKC){
        ধ্রুবক Normalizer2 *n2 = Normalizer2::getNFKCInstance(*status);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*status));
        যদি(U_FAILURE(*status)){
            ফেরত 0;
        }
        fn2.normalize(s1, s2, *status);
    }অন্যথা{
        s2.fastCopyFrom(s1);
    }
    যদি(U_FAILURE(*status)){
        ফেরত 0;
    }

    // Prohibit and checkBiDi in one pass
    ধ্রুবক char16_t *b2 = s2.get_বাফার();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_গুনতি, firstCharDir=U_CHAR_DIRECTION_গুনতি;
    UBool বাম_To_ডান=মিথ্যা, ডান_To_বাম=মিথ্যা;
    int32_t rtlPos =-1, ltrPos =-1;

    জন্য(int32_t b2_সূচক=0; b2_সূচক<b2Len;){
        UChar32 ch = 0;
        U16_পরবর্তী(b2, b2_সূচক, b2Len, ch);

        uint16_t ফলাফল;
        UTRIE_GET16(&profile->sprepTrie,ch,ফলাফল);

        int16_t value;
        UBool is_সূচক;
        UStringPrepType type = get_মানসমূহ(ফলাফল, value, is_সূচক);

        যদি( type == USPREP_PROHIBITED || 
            ((ফলাফল < _SPREP_TYPE_THRESHOLD) && (ফলাফল & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *status = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2_সূচক-U16_দৈর্ঘ্য(ch), b2Len, parseError);
            ফেরত 0;
        }

        যদি(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            যদি(firstCharDir == U_CHAR_DIRECTION_গুনতি){
                firstCharDir = direction;
            }
            যদি(direction == U_বাম_TO_ডান){
                বাম_To_ডান = সত্য;
                ltrPos = b2_সূচক-1;
            }
            যদি(direction == U_ডান_TO_বাম || direction == U_ডান_TO_বাম_ARABIC){
                ডান_To_বাম = সত্য;
                rtlPos = b2_সূচক-1;
            }
        }
    }
    যদি(profile->checkBiDi){
        // satisfy 2
        যদি( বাম_To_ডান && ডান_To_বাম){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            ফেরত 0;
        }

        //satisfy 3
        যদি( ডান_To_বাম && 
            !((firstCharDir == U_ডান_TO_বাম || firstCharDir == U_ডান_TO_বাম_ARABIC) &&
              (direction == U_ডান_TO_বাম || direction == U_ডান_TO_বাম_ARABIC))
           ){
            *status = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            ফেরত মিথ্যা;
        }
    }
    ফেরত s2.extract(dest, destCapacity, *status);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_swap(ধ্রুবক UDataSwapper *ds,
            ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
            UErrorCode *pErrorCode) {
    ধ্রুবক UDataInfo *pInfo;
    int32_t header_আকার;

    ধ্রুবক uint8_t *inBytes;
    uint8_t *outBytes;

    ধ্রুবক int32_t *inIndexes;
    int32_t indexes[16];

    int32_t i, offset, count, size;

    /* udata_swapDataHeader checks the arguments */
    header_আকার=udata_swap_তথ্য_Header(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* check data format and format version */
    pInfo=(ধ্রুবক UDataInfo *)((ধ্রুবক অক্ষর *)in_তথ্য+4);
    যদি(!(
        pInfo->তথ্য_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->তথ্য_Format[1]==0x50 &&
        pInfo->তথ্য_Format[2]==0x52 &&
        pInfo->তথ্য_Format[3]==0x50 &&
        pInfo->formatVersion[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->তথ্য_Format[0], pInfo->তথ্য_Format[1],
                         pInfo->তথ্য_Format[2], pInfo->তথ্য_Format[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    inBytes=(ধ্রুবক uint8_t *)in_তথ্য+header_আকার;
    outBytes= (out_তথ্য == nullptr ) ? nullptr : (uint8_t *)out_তথ্য+header_আকার;

    inIndexes=(ধ্রুবক int32_t *)inBytes;

    যদি(length>=0) {
        length-=header_আকার;
        যদি(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    জন্য(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[SPREP_সূচক_TRIE_আকার]+
        indexes[SPREP_সূচক_MAPPING_তথ্য_আকার];

    যদি(length>=0) {
        যদি(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
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
        count=indexes[SPREP_সূচক_TRIE_আকার];
        utrie_swap(ds, inBytes+offset, count, outBytes+offset, pErrorCode);
        offset+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[SPREP_সূচক_MAPPING_তথ্য_আকার];
        ds->swapArray16(ds, inBytes+offset, count, outBytes+offset, pErrorCode);
        //offset+=count;
    }

    ফেরত header_আকার+size;
}

#endif /* #if !UCONFIG_NO_IDNA */
