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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_IDNA

#ضمن "unicode/usprep.h"

#ضمن "unicode/normalizer2.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/uversion.h"
#ضمن "umutex.h"
#ضمن "cmemory.h"
#ضمن "sprpimpl.h"
#ضمن "ustr_imp.h"
#ضمن "uhash.h"
#ضمن "cstring.h"
#ضمن "udataswp.h"
#ضمن "ucln_cmn.h"
#ضمن "ubidi_props.h"
#ضمن "uprops.h"

U_NAMESPACE_USE

U_CDECL_BEGIN

/*
Static cache for already opened StringPrep profiles
*/
ساكن UHashtable *SHARED_معلومات_HASHTABLE = خالي_اشارو;
ساكن icu::UInitOnce g_Shared_معلومات_Init_Once {};

ساكن UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
ساكن UVersionInfo معلومات_نسخو={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
ساكن ثابت حرف * ثابت PROFILE_نالا[] = {
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

ساكن UBool U_CALLCONV
isSPrepAcceptable(فراغ * /* context */,
             ثابت حرف * /* type */, 
             ثابت حرف * /* name */,
             ثابت UDataInfo *pInfo) {
    إذا(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->معلومات_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->معلومات_Format[1]==0x50 &&
        pInfo->معلومات_Format[2]==0x52 &&
        pInfo->معلومات_Format[3]==0x50 &&
        pInfo->format_نسخو[0]==3 &&
        pInfo->format_نسخو[2]==UTRIE_سرڪايو &&
        pInfo->format_نسخو[3]==UTRIE_اشاريو_سرڪايو
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(معلومات_نسخو, pInfo->معلومات_نسخو, 4);
        أرجع سچ;
    } وإلا {
        أرجع خطأ;
    }
}

ساكن int32_t U_CALLCONV
get_SPrep_Folding_وٿي(uint32_t معلومات) {

    أرجع (int32_t)معلومات;

}

/* hashes an entry  */
ساكن int32_t U_CALLCONV 
hash_داخلا(ثابت UHashTok parm) {
    UString_Prep_ڪنجي *b = (UString_Prep_ڪنجي *)parm.اشارو;
    UHashTok namekey, pathkey;
    namekey.اشارو = b->name;
    pathkey.اشارو = b->رستو;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    أرجع static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
ساكن UBool U_CALLCONV 
compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UString_Prep_ڪنجي *b1 = (UString_Prep_ڪنجي *)p1.اشارو;
    UString_Prep_ڪنجي *b2 = (UString_Prep_ڪنجي *)p2.اشارو;
    UHashTok name1, name2, path1, path2;
    name1.اشارو = b1->name;
    name2.اشارو = b2->name;
    path1.اشارو = b1->رستو;
    path2.اشارو = b2->رستو;
    أرجع uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

ساكن فراغ 
usprep_unload(UStringPrepProfile* معلومات){
    udata_close(معلومات->sprep_معلومات);
}

ساكن int32_t 
usprep_اندروني_flush_Cache(UBool no_Ref_ڳڻپ){
    UStringPrepProfile *profile = خالي_اشارو;
    UString_Prep_ڪنجي  *ڪنجي  = خالي_اشارو;
    int32_t pos = UHASH_پهريون;
    int32_t deleted_انگ = 0;
    ثابت UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_تالو(&usprepMutex);
    إذا (SHARED_معلومات_HASHTABLE == خالي_اشارو) {
        umtx_unlock(&usprepMutex);
        أرجع 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    طالما ((e = uhash_ايندڙ_Element(SHARED_معلومات_HASHTABLE, &pos)) != خالي_اشارو)
    {
        profile = (UStringPrepProfile *) e->value.اشارو;
        ڪنجي  = (UString_Prep_ڪنجي *) e->ڪنجي.اشارو;

        إذا ((no_Ref_ڳڻپ== خطأ && profile->ref_ڳڻپ == 0) || 
             no_Ref_ڳڻپ) {
            deleted_انگ++;
            uhash_removeElement(SHARED_معلومات_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            إذا(ڪنجي->name != خالي_اشارو) {
                uprv_free(ڪنجي->name);
                ڪنجي->name=خالي_اشارو;
            }
            إذا(ڪنجي->رستو != خالي_اشارو) {
                uprv_free(ڪنجي->رستو);
                ڪنجي->رستو=خالي_اشارو;
            }
            uprv_free(profile);
            uprv_free(ڪنجي);
        }
       
    }
    umtx_unlock(&usprepMutex);

    أرجع deleted_انگ;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

ساكن UBool U_CALLCONV usprep_cleanup(){
    إذا (SHARED_معلومات_HASHTABLE != خالي_اشارو) {
        usprep_اندروني_flush_Cache(سچ);
        إذا (SHARED_معلومات_HASHTABLE != خالي_اشارو && uhash_ڳڻپ(SHARED_معلومات_HASHTABLE) == 0) {
            uhash_close(SHARED_معلومات_HASHTABLE);
            SHARED_معلومات_HASHTABLE = خالي_اشارو;
        }
    }
    g_Shared_معلومات_Init_Once.reset();
    أرجع (SHARED_معلومات_HASHTABLE == خالي_اشارو);
}
U_CDECL_پڇاڙي


/** Initializes the cache for resources */
ساكن فراغ U_CALLCONV
createCache(UError_ڪوڊ &حالت) {
    SHARED_معلومات_HASHTABLE = uhash_open(hash_داخلا, compareEntries, خالي_اشارو, &حالت);
    إذا (U_FAILURE(حالت)) {
        SHARED_معلومات_HASHTABLE = خالي_اشارو;
    }
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

ساكن فراغ 
initCache(UError_ڪوڊ *حالت) {
    umtx_initOnce(g_Shared_معلومات_Init_Once, &createCache, *حالت);
}

ساكن UBool U_CALLCONV
ڀريو_معلومات(UStringPrepProfile* profile, 
         ثابت حرف* رستو, 
         ثابت حرف* name, 
         ثابت حرف* type, 
         UError_ڪوڊ* error_ڪوڊ) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {خالي_اشارو, خالي_اشارو, خالي_اشارو, 0, 0, 0, 0};
    UData_يادگيري *معلومات_يادگيري;
    ثابت int32_t *p=خالي_اشارو;
    ثابت uint8_t *pb;
    UVersionInfo norm_Unicode_نسخو;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    إذا(error_ڪوڊ==خالي_اشارو || U_FAILURE(*error_ڪوڊ)) {
        أرجع 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    معلومات_يادگيري=udata_openChoice(رستو, type, name, isSPrepAcceptable, خالي_اشارو, error_ڪوڊ);
    إذا(U_FAILURE(*error_ڪوڊ)) {
        أرجع خطأ;
    }

    p = static_cast<ثابت int32_t*>(udata_get_يادگيري(معلومات_يادگيري));
    pb = reinterpret_cast<ثابت uint8_t*>(p + SPREP_اشاريو_چوٽي);
    utrie_unserialize(&_sprepTrie, pb, p[SPREP_اشاريو_TRIE_سائيز], error_ڪوڊ);
    _sprepTrie.get_Folding_وٿي=get_SPrep_Folding_وٿي;


    إذا(U_FAILURE(*error_ڪوڊ)) {
        udata_close(معلومات_يادگيري);
        أرجع خطأ;
    }

    /* in the mutex block, set the data for this process */
    umtx_تالو(&usprepMutex);
    إذا(profile->sprep_معلومات==خالي_اشارو) {
        profile->sprep_معلومات=معلومات_يادگيري;
        معلومات_يادگيري=خالي_اشارو;
        uprv_memcpy(&profile->indexes, p, حجم(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, حجم(UTrie));
    } وإلا {
        p = static_cast<ثابت int32_t*>(udata_get_يادگيري(profile->sprep_معلومات));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mapping_معلومات = reinterpret_cast<ثابت uint16_t*>(reinterpret_cast<ثابت uint8_t*>(p + SPREP_اشاريو_چوٽي) + profile->indexes[SPREP_اشاريو_TRIE_سائيز]);
    
    u_get_Unicode_نسخو(norm_Unicode_نسخو);
    normUniVer = (norm_Unicode_نسخو[0] << 24) + (norm_Unicode_نسخو[1] << 16) + 
                 (norm_Unicode_نسخو[2] << 8 ) + (norm_Unicode_نسخو[3]);
    sprepUniVer = (معلومات_نسخو[0] << 24) + (معلومات_نسخو[1] << 16) + 
                  (معلومات_نسخو[2] << 8 ) + (معلومات_نسخو[3]);
    normCorrVer = profile->indexes[SPREP_NORM_CORRECTNS_آخري_UNI_نسخو];
    
    إذا(U_FAILURE(*error_ڪوڊ)){
        udata_close(معلومات_يادگيري);
        أرجع خطأ;
    }
    إذا( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *error_ڪوڊ = U_INVALID_FORMAT_ERROR;
        udata_close(معلومات_يادگيري);
        أرجع خطأ;
    }
    profile->is_معلومات_Loaded = سچ;

    /* if a different thread set it first, then close the extra data */
    إذا(معلومات_يادگيري!=خالي_اشارو) {
        udata_close(معلومات_يادگيري); /* nullptr if it was set correctly */
    }


    أرجع profile->is_معلومات_Loaded;
}

ساكن UStringPrepProfile* 
usprep_getProfile(ثابت حرف* رستو, 
                  ثابت حرف* name,
                  UError_ڪوڊ *حالت){

    UStringPrepProfile* profile = خالي_اشارو;

    initCache(حالت);

    إذا(U_FAILURE(*حالت)){
        أرجع خالي_اشارو;
    }

    UString_Prep_ڪنجي ڍير_ڪنجي;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    ڍير_ڪنجي.name = const_cast<حرف*>(name);
    ڍير_ڪنجي.رستو = const_cast<حرف*>(رستو);

    /* fetch the data from the cache */
    umtx_تالو(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_معلومات_HASHTABLE, &ڍير_ڪنجي));
    إذا(profile != خالي_اشارو) {
        profile->ref_ڳڻپ++;
    }
    umtx_unlock(&usprepMutex);
    
    إذا(profile == خالي_اشارو) {
        /* else load the data and put the data in the cache */
        Local_يادگيري<UStringPrepProfile> newProfile;
        إذا(newProfile.allocateInsteadAndReset() == خالي_اشارو) {
            *حالت = U_يادگيري_ورهاست_ERROR;
            أرجع خالي_اشارو;
        }

        /* load the data */
        إذا(!ڀريو_معلومات(newProfile.getAlias(), رستو, name, SPREP_معلومات_TYPE, حالت) || U_FAILURE(*حالت) ){
            أرجع خالي_اشارو;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        Local_يادگيري<UString_Prep_ڪنجي> ڪنجي;
        Local_يادگيري<حرف> ڪنجي_Name;
        Local_يادگيري<حرف> ڪنجي_رستو;
        إذا( ڪنجي.allocateInsteadAndReset() == خالي_اشارو ||
            ڪنجي_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == خالي_اشارو ||
            (رستو != خالي_اشارو &&
             ڪنجي_رستو.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(رستو)+1)) == خالي_اشارو)
         ) {
            *حالت = U_يادگيري_ورهاست_ERROR;
            usprep_unload(newProfile.getAlias());
            أرجع خالي_اشارو;
        }

        umtx_تالو(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_معلومات_HASHTABLE, &ڍير_ڪنجي));
        إذا(profile != خالي_اشارو) {
            profile->ref_ڳڻپ++;
            usprep_unload(newProfile.getAlias());
        }
        وإلا {
            /* initialize the key members */
            ڪنجي->name = ڪنجي_Name.orphan();
            uprv_strcpy(ڪنجي->name, name);
            إذا(رستو != خالي_اشارو){
                ڪنجي->رستو = ڪنجي_رستو.orphan();
                uprv_strcpy(ڪنجي->رستو, رستو);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_ڳڻپ = 1;
            uhash_put(SHARED_معلومات_HASHTABLE, ڪنجي.orphan(), profile, حالت);
        }
        umtx_unlock(&usprepMutex);
    }

    أرجع profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(ثابت حرف* رستو, 
            ثابت حرف* name,
            UError_ڪوڊ* حالت){

    إذا(حالت == خالي_اشارو || U_FAILURE(*حالت)){
        أرجع خالي_اشارو;
    }
       
    /* initialize the profile struct members */
    أرجع usprep_getProfile(رستو,name,حالت);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UError_ڪوڊ* حالت) {
    إذا(حالت == خالي_اشارو || U_FAILURE(*حالت)){
        أرجع خالي_اشارو;
    }
    int32_t اشاريو = (int32_t)type;
    إذا (اشاريو < 0 || اشاريو >= UPRV_LENGTHOF(PROFILE_نالا)) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    أرجع usprep_open(خالي_اشارو, PROFILE_نالا[اشاريو], حالت);
}

U_CAPI فراغ U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    إذا(profile==خالي_اشارو){
        أرجع;
    }

    umtx_تالو(&usprepMutex);
    /* decrement the ref count*/
    إذا(profile->ref_ڳڻپ > 0){
        profile->ref_ڳڻپ--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC فراغ 
uprv_syntaxError(ثابت char16_t* قاعدا,
                 int32_t pos,
                 int32_t قاعدا_Len,
                 UParseError* parseError){
    إذا(parseError == خالي_اشارو){
        أرجع;
    }
    parseError->وٿي = pos;
    parseError->سٽ = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t شروعات = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t حد_بندي = pos;
    
    u_memcpy(parseError->preContext,قاعدا+شروعات,حد_بندي-شروعات);
    //null terminate the buffer
    parseError->preContext[حد_بندي-شروعات] = 0;
    
    // for post-context; include error rules[pos]
    شروعات = pos;
    حد_بندي = شروعات + (U_PARSE_CONTEXT_LEN-1);
    إذا (حد_بندي > قاعدا_Len) {
        حد_بندي = قاعدا_Len;
    }
    إذا (شروعات < قاعدا_Len) {
        u_memcpy(parseError->postContext,قاعدا+شروعات,حد_بندي-شروعات);
    }
    //null terminate the buffer
    parseError->postContext[حد_بندي-شروعات]= 0;
}


ساكن مضمن UStringPrepType
get_قدرون(uint16_t trie_لفظ, int16_t& value, UBool& is_اشاريو){

    UStringPrepType type;
    إذا(trie_لفظ == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_حد_بندي;
        is_اشاريو =خطأ;
        value = 0;
    }وإلا إذا(trie_لفظ >= SPREP_TYPE_حد){
        type = static_cast<UStringPrepType>(trie_لفظ - SPREP_TYPE_حد);
        is_اشاريو =خطأ;
        value = 0;
    }وإلا{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        إذا(trie_لفظ & 0x02){
            is_اشاريو = سچ;
            value = trie_لفظ  >> 2; //mask off the lower 2 bits and shift
        }وإلا{
            is_اشاريو = خطأ;
            value = static_cast<int16_t>(trie_لفظ);
            value =  (value >> 2);
        }
 
        إذا((trie_لفظ>>2) == SPREP_MAX_اشاريو_قدر){
            type = USPREP_DELETE;
            is_اشاريو =خطأ;
            value = 0;
        }
    }
    أرجع type;
}

// TODO: change to writing to UnicodeString not char16_t *
ساكن int32_t 
usprep_map(  ثابت UStringPrepProfile* profile, 
             ثابت char16_t* src, int32_t src_ڊيگهائي,
             char16_t* dest, int32_t dest_صلاحيت,
             int32_t options,
             UParseError* parseError,
             UError_ڪوڊ* حالت ){
    
    uint16_t نتيجو;
    int32_t dest_اشاريو=0;
    int32_t src_اشاريو;
    UBool allowUnassigned = static_cast<UBool>((options & USPREP_ALLOW_UNASSIGNED) > 0);
    UStringPrepType type;
    int16_t value;
    UBool is_اشاريو;
    ثابت int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    لكل(src_اشاريو=0;src_اشاريو<src_ڊيگهائي;){
        UChar32 ch;

        U16_ايندڙ(src,src_اشاريو,src_ڊيگهائي,ch);
        
        نتيجو=0;

        UTRIE_GET16(&profile->sprepTrie,ch,نتيجو);
        
        type = get_قدرون(نتيجو, value, is_اشاريو);

        // check if the source codepoint is unassigned
        إذا(type == USPREP_UNASSIGNED && allowUnassigned == خطأ){

            uprv_syntaxError(src,src_اشاريو-U16_ڊيگهائي(ch), src_ڊيگهائي,parseError);
            *حالت = U_STRINGPREP_UNASSIGNED_ERROR;
            أرجع 0;
            
        }وإلا إذا(type == USPREP_MAP){
            
            int32_t اشاريو, length;

            إذا(is_اشاريو){
                اشاريو = value;
                إذا(اشاريو >= indexes[SPREP_ONE_UCHAR_MAPPING_اشاريو_شروعات] &&
                         اشاريو < indexes[SPREP_TWO_UCHARS_MAPPING_اشاريو_شروعات]){
                    length = 1;
                }وإلا إذا(اشاريو >= indexes[SPREP_TWO_UCHARS_MAPPING_اشاريو_شروعات] &&
                         اشاريو < indexes[SPREP_THREE_UCHARS_MAPPING_اشاريو_شروعات]){
                    length = 2;
                }وإلا إذا(اشاريو >= indexes[SPREP_THREE_UCHARS_MAPPING_اشاريو_شروعات] &&
                         اشاريو < indexes[SPREP_FOUR_UCHARS_MAPPING_اشاريو_شروعات]){
                    length = 3;
                }وإلا{
                    length = profile->mapping_معلومات[اشاريو++];
         
                }

                /* copy mapping to destination */
                لكل(int32_t i=0; i< length; i++){
                    إذا(dest_اشاريو < dest_صلاحيت  ){
                        dest[dest_اشاريو] = profile->mapping_معلومات[اشاريو+i];
                    }
                    dest_اشاريو++; /* for pre-flighting */
                }  
                استمر;
            }وإلا{
                // subtract the delta to arrive at the code point
                ch -= value;
            }

        }وإلا إذا(type==USPREP_DELETE){
             // just consume the codepoint and continue
            استمر;
        }
        //copy the code point into destination
        إذا(ch <= 0xFFFF){
            إذا(dest_اشاريو < dest_صلاحيت ){
                dest[dest_اشاريو] = static_cast<char16_t>(ch);
            }
            dest_اشاريو++;
        }وإلا{
            إذا(dest_اشاريو+1 < dest_صلاحيت ){
                dest[dest_اشاريو]   = U16_LEAD(ch);
                dest[dest_اشاريو+1] = U16_TRAIL(ch);
            }
            dest_اشاريو +=2;
        }
       
    }
        
    أرجع u_terminateUChars(dest, dest_صلاحيت, dest_اشاريو, حالت);
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
usprep_prepare(   ثابت UStringPrepProfile* profile,
                  ثابت char16_t* src, int32_t src_ڊيگهائي,
                  char16_t* dest, int32_t dest_صلاحيت,
                  int32_t options,
                  UParseError* parseError,
                  UError_ڪوڊ* حالت ){

    // check error status
    إذا(U_FAILURE(*حالت)){
        أرجع 0;
    }

    //check arguments
    إذا(profile==خالي_اشارو ||
            (src==خالي_اشارو ? src_ڊيگهائي!=0 : src_ڊيگهائي<-1) ||
            (dest==خالي_اشارو ? dest_صلاحيت!=0 : dest_صلاحيت<0)) {
        *حالت=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    //get the string length
    إذا(src_ڊيگهائي < 0){
        src_ڊيگهائي = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.get_بفر(src_ڊيگهائي);
    إذا(b1==خالي_اشارو){
        *حالت = U_يادگيري_ورهاست_ERROR;
        أرجع 0;
    }
    UError_ڪوڊ بفر_حالت = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, src_ڊيگهائي,
                               b1, s1.get_صلاحيت(), options, parseError, &بفر_حالت);
    s1.release_بفر(U_SUCCESS(بفر_حالت) ? b1Len : 0);

    إذا(بفر_حالت == U_بفر_حد_کان_مٿي_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.get_بفر(b1Len);
        إذا(b1==خالي_اشارو){
            *حالت = U_يادگيري_ورهاست_ERROR;
            أرجع 0;
        }

        بفر_حالت = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, src_ڊيگهائي,
                           b1, s1.get_صلاحيت(), options, parseError, &بفر_حالت);
        s1.release_بفر(U_SUCCESS(بفر_حالت) ? b1Len : 0);
    }
    إذا(U_FAILURE(بفر_حالت)){
        *حالت = بفر_حالت;
        أرجع 0;
    }

    // normalize
    UnicodeString s2;
    إذا(profile->doNFKC){
        ثابت Normalizer2 *n2 = Normalizer2::getNFKCInstance(*حالت);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*حالت));
        إذا(U_FAILURE(*حالت)){
            أرجع 0;
        }
        fn2.normalize(s1, s2, *حالت);
    }وإلا{
        s2.fastCopyFrom(s1);
    }
    إذا(U_FAILURE(*حالت)){
        أرجع 0;
    }

    // Prohibit and checkBiDi in one pass
    ثابت char16_t *b2 = s2.get_بفر();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_ڳڻپ, پهريون_Char_Dir=U_CHAR_DIRECTION_ڳڻپ;
    UBool کاٻو_منزل_ساڄو=خطأ, ساڄو_منزل_کاٻو=خطأ;
    int32_t rtlPos =-1, ltrPos =-1;

    لكل(int32_t b2_اشاريو=0; b2_اشاريو<b2Len;){
        UChar32 ch = 0;
        U16_ايندڙ(b2, b2_اشاريو, b2Len, ch);

        uint16_t نتيجو;
        UTRIE_GET16(&profile->sprepTrie,ch,نتيجو);

        int16_t value;
        UBool is_اشاريو;
        UStringPrepType type = get_قدرون(نتيجو, value, is_اشاريو);

        إذا( type == USPREP_PROHIBITED || 
            ((نتيجو < SPREP_TYPE_حد) && (نتيجو & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *حالت = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2_اشاريو-U16_ڊيگهائي(ch), b2Len, parseError);
            أرجع 0;
        }

        إذا(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            إذا(پهريون_Char_Dir == U_CHAR_DIRECTION_ڳڻپ){
                پهريون_Char_Dir = direction;
            }
            إذا(direction == U_کاٻو_منزل_ساڄو){
                کاٻو_منزل_ساڄو = سچ;
                ltrPos = b2_اشاريو-1;
            }
            إذا(direction == U_ساڄو_منزل_کاٻو || direction == U_ساڄو_منزل_کاٻو_ARABIC){
                ساڄو_منزل_کاٻو = سچ;
                rtlPos = b2_اشاريو-1;
            }
        }
    }
    إذا(profile->checkBiDi){
        // satisfy 2
        إذا( کاٻو_منزل_ساڄو && ساڄو_منزل_کاٻو){
            *حالت = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            أرجع 0;
        }

        //satisfy 3
        إذا( ساڄو_منزل_کاٻو && 
            !((پهريون_Char_Dir == U_ساڄو_منزل_کاٻو || پهريون_Char_Dir == U_ساڄو_منزل_کاٻو_ARABIC) &&
              (direction == U_ساڄو_منزل_کاٻو || direction == U_ساڄو_منزل_کاٻو_ARABIC))
           ){
            *حالت = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            أرجع خطأ;
        }
    }
    أرجع s2.extract(dest, dest_صلاحيت, *حالت);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_مٽا_سٽا(ثابت UDataSwapper *ds,
            ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
            UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UDataInfo *pInfo;
    int32_t سرو_سائيز;

    ثابت uint8_t *اندريون_بائيٽون;
    uint8_t *ٻاهريون_بائيٽون;

    ثابت int32_t *اندريون_Indexes;
    int32_t indexes[16];

    int32_t i, وٿي, count, size;

    /* udata_swapDataHeader checks the arguments */
    سرو_سائيز=udata_مٽا_سٽا_معلومات_سرو(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندريون_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->معلومات_Format[1]==0x50 &&
        pInfo->معلومات_Format[2]==0x52 &&
        pInfo->معلومات_Format[3]==0x50 &&
        pInfo->format_نسخو[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخو[0]);
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    اندريون_بائيٽون=(ثابت uint8_t *)اندريون_معلومات+سرو_سائيز;
    ٻاهريون_بائيٽون= (ٻاهريون_معلومات == خالي_اشارو ) ? خالي_اشارو : (uint8_t *)ٻاهريون_معلومات+سرو_سائيز;

    اندريون_Indexes=(ثابت int32_t *)اندريون_بائيٽون;

    إذا(length>=0) {
        length-=سرو_سائيز;
        إذا(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    لكل(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, اندريون_Indexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[SPREP_اشاريو_TRIE_سائيز]+
        indexes[SPREP_اشاريو_MAPPING_معلومات_سائيز];

    إذا(length>=0) {
        إذا(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        /* copy the data for inaccessible bytes */
        إذا(اندريون_بائيٽون!=ٻاهريون_بائيٽون) {
            uprv_memcpy(ٻاهريون_بائيٽون, اندريون_بائيٽون, size);
        }

        وٿي=0;

        /* swap the int32_t indexes[] */
        count=16*4;
        ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون, count, ٻاهريون_بائيٽون, p_Error_ڪوڊ);
        وٿي+=count;

        /* swap the UTrie */
        count=indexes[SPREP_اشاريو_TRIE_سائيز];
        utrie_مٽا_سٽا(ds, اندريون_بائيٽون+وٿي, count, ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);
        وٿي+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[SPREP_اشاريو_MAPPING_معلومات_سائيز];
        ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+وٿي, count, ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);
        //offset+=count;
    }

    أرجع سرو_سائيز+size;
}

#جيڪڏهن_پورو /* #if !UCONFIG_NO_IDNA */
