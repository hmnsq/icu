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
ساكن UHashtable *SHARED_انکڑے_HASHTABLE = خالی_سوچک;
ساكن icu::UInitOnce g_Shared_انکڑے_Init_Once {};

ساكن UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
ساكن UVersionInfo انکڑے_نسخہ={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
ساكن ثابت حرف * ثابت PROFILE_ناں[] = {
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
        pInfo->انکڑے_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->انکڑے_Format[1]==0x50 &&
        pInfo->انکڑے_Format[2]==0x52 &&
        pInfo->انکڑے_Format[3]==0x50 &&
        pInfo->format_نسخہ[0]==3 &&
        pInfo->format_نسخہ[2]==UTRIE_کھسکاؤ &&
        pInfo->format_نسخہ[3]==UTRIE_اشاریہ_کھسکاؤ
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(انکڑے_نسخہ, pInfo->انکڑے_نسخہ, 4);
        أرجع سچ;
    } وإلا {
        أرجع خطأ;
    }
}

ساكن int32_t U_CALLCONV
get_SPrep_Folding_وتھ(uint32_t انکڑے) {

    أرجع (int32_t)انکڑے;

}

/* hashes an entry  */
ساكن int32_t U_CALLCONV 
hash_اندراج(ثابت UHashTok parm) {
    UString_Prep_کلید *b = (UString_Prep_کلید *)parm.اشارہ_گر;
    UHashTok namekey, pathkey;
    namekey.اشارہ_گر = b->name;
    pathkey.اشارہ_گر = b->رستہ;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    أرجع static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
ساكن UBool U_CALLCONV 
compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UString_Prep_کلید *b1 = (UString_Prep_کلید *)p1.اشارہ_گر;
    UString_Prep_کلید *b2 = (UString_Prep_کلید *)p2.اشارہ_گر;
    UHashTok name1, name2, path1, path2;
    name1.اشارہ_گر = b1->name;
    name2.اشارہ_گر = b2->name;
    path1.اشارہ_گر = b1->رستہ;
    path2.اشارہ_گر = b2->رستہ;
    أرجع uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

ساكن فراغ 
usprep_unload(UStringPrepProfile* انکڑے){
    udata_close(انکڑے->sprep_انکڑے);
}

ساكن int32_t 
usprep_اندرونی_flush_Cache(UBool no_Ref_گنتی){
    UStringPrepProfile *profile = خالی_سوچک;
    UString_Prep_کلید  *کلید  = خالی_سوچک;
    int32_t pos = UHASH_پہلا;
    int32_t deleted_عدد = 0;
    ثابت UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_جندرا(&usprepMutex);
    إذا (SHARED_انکڑے_HASHTABLE == خالی_سوچک) {
        umtx_unlock(&usprepMutex);
        أرجع 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    طالما ((e = uhash_اگلا_Element(SHARED_انکڑے_HASHTABLE, &pos)) != خالی_سوچک)
    {
        profile = (UStringPrepProfile *) e->value.اشارہ_گر;
        کلید  = (UString_Prep_کلید *) e->کلید.اشارہ_گر;

        إذا ((no_Ref_گنتی== خطأ && profile->ref_گنتی == 0) || 
             no_Ref_گنتی) {
            deleted_عدد++;
            uhash_removeElement(SHARED_انکڑے_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            إذا(کلید->name != خالی_سوچک) {
                uprv_free(کلید->name);
                کلید->name=خالی_سوچک;
            }
            إذا(کلید->رستہ != خالی_سوچک) {
                uprv_free(کلید->رستہ);
                کلید->رستہ=خالی_سوچک;
            }
            uprv_free(profile);
            uprv_free(کلید);
        }
       
    }
    umtx_unlock(&usprepMutex);

    أرجع deleted_عدد;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

ساكن UBool U_CALLCONV usprep_cleanup(){
    إذا (SHARED_انکڑے_HASHTABLE != خالی_سوچک) {
        usprep_اندرونی_flush_Cache(سچ);
        إذا (SHARED_انکڑے_HASHTABLE != خالی_سوچک && uhash_گنتی(SHARED_انکڑے_HASHTABLE) == 0) {
            uhash_close(SHARED_انکڑے_HASHTABLE);
            SHARED_انکڑے_HASHTABLE = خالی_سوچک;
        }
    }
    g_Shared_انکڑے_Init_Once.reset();
    أرجع (SHARED_انکڑے_HASHTABLE == خالی_سوچک);
}
U_CDECL_سرا


/** Initializes the cache for resources */
ساكن فراغ U_CALLCONV
createCache(UError_کوڈ &حالت) {
    SHARED_انکڑے_HASHTABLE = uhash_open(hash_اندراج, compareEntries, خالی_سوچک, &حالت);
    إذا (U_FAILURE(حالت)) {
        SHARED_انکڑے_HASHTABLE = خالی_سوچک;
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

ساكن فراغ 
initCache(UError_کوڈ *حالت) {
    umtx_initOnce(g_Shared_انکڑے_Init_Once, &createCache, *حالت);
}

ساكن UBool U_CALLCONV
لدو_انکڑے(UStringPrepProfile* profile, 
         ثابت حرف* رستہ, 
         ثابت حرف* name, 
         ثابت حرف* type, 
         UError_کوڈ* error_کوڈ) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {خالی_سوچک, خالی_سوچک, خالی_سوچک, 0, 0, 0, 0};
    UData_حافظہ *انکڑے_حافظہ;
    ثابت int32_t *p=خالی_سوچک;
    ثابت uint8_t *pb;
    UVersionInfo norm_Unicode_نسخہ;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    إذا(error_کوڈ==خالی_سوچک || U_FAILURE(*error_کوڈ)) {
        أرجع 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    انکڑے_حافظہ=udata_openChoice(رستہ, type, name, isSPrepAcceptable, خالی_سوچک, error_کوڈ);
    إذا(U_FAILURE(*error_کوڈ)) {
        أرجع خطأ;
    }

    p = static_cast<ثابت int32_t*>(udata_get_حافظہ(انکڑے_حافظہ));
    pb = reinterpret_cast<ثابت uint8_t*>(p + SPREP_اشاریہ_چوٹی);
    utrie_unserialize(&_sprepTrie, pb, p[SPREP_اشاریہ_TRIE_ناپ], error_کوڈ);
    _sprepTrie.get_Folding_وتھ=get_SPrep_Folding_وتھ;


    إذا(U_FAILURE(*error_کوڈ)) {
        udata_close(انکڑے_حافظہ);
        أرجع خطأ;
    }

    /* in the mutex block, set the data for this process */
    umtx_جندرا(&usprepMutex);
    إذا(profile->sprep_انکڑے==خالی_سوچک) {
        profile->sprep_انکڑے=انکڑے_حافظہ;
        انکڑے_حافظہ=خالی_سوچک;
        uprv_memcpy(&profile->indexes, p, حجم(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, حجم(UTrie));
    } وإلا {
        p = static_cast<ثابت int32_t*>(udata_get_حافظہ(profile->sprep_انکڑے));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mapping_انکڑے = reinterpret_cast<ثابت uint16_t*>(reinterpret_cast<ثابت uint8_t*>(p + SPREP_اشاریہ_چوٹی) + profile->indexes[SPREP_اشاریہ_TRIE_ناپ]);
    
    u_get_Unicode_نسخہ(norm_Unicode_نسخہ);
    normUniVer = (norm_Unicode_نسخہ[0] << 24) + (norm_Unicode_نسخہ[1] << 16) + 
                 (norm_Unicode_نسخہ[2] << 8 ) + (norm_Unicode_نسخہ[3]);
    sprepUniVer = (انکڑے_نسخہ[0] << 24) + (انکڑے_نسخہ[1] << 16) + 
                  (انکڑے_نسخہ[2] << 8 ) + (انکڑے_نسخہ[3]);
    normCorrVer = profile->indexes[SPREP_NORM_CORRECTNS_آخری_UNI_نسخہ];
    
    إذا(U_FAILURE(*error_کوڈ)){
        udata_close(انکڑے_حافظہ);
        أرجع خطأ;
    }
    إذا( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *error_کوڈ = U_INVALID_FORMAT_ERROR;
        udata_close(انکڑے_حافظہ);
        أرجع خطأ;
    }
    profile->is_انکڑے_Loaded = سچ;

    /* if a different thread set it first, then close the extra data */
    إذا(انکڑے_حافظہ!=خالی_سوچک) {
        udata_close(انکڑے_حافظہ); /* nullptr if it was set correctly */
    }


    أرجع profile->is_انکڑے_Loaded;
}

ساكن UStringPrepProfile* 
usprep_getProfile(ثابت حرف* رستہ, 
                  ثابت حرف* name,
                  UError_کوڈ *حالت){

    UStringPrepProfile* profile = خالی_سوچک;

    initCache(حالت);

    إذا(U_FAILURE(*حالت)){
        أرجع خالی_سوچک;
    }

    UString_Prep_کلید ڈھیر_کلید;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    ڈھیر_کلید.name = const_cast<حرف*>(name);
    ڈھیر_کلید.رستہ = const_cast<حرف*>(رستہ);

    /* fetch the data from the cache */
    umtx_جندرا(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_انکڑے_HASHTABLE, &ڈھیر_کلید));
    إذا(profile != خالی_سوچک) {
        profile->ref_گنتی++;
    }
    umtx_unlock(&usprepMutex);
    
    إذا(profile == خالی_سوچک) {
        /* else load the data and put the data in the cache */
        Local_حافظہ<UStringPrepProfile> newProfile;
        إذا(newProfile.allocateInsteadAndReset() == خالی_سوچک) {
            *حالت = U_حافظہ_ونڈ_ERROR;
            أرجع خالی_سوچک;
        }

        /* load the data */
        إذا(!لدو_انکڑے(newProfile.getAlias(), رستہ, name, SPREP_انکڑے_TYPE, حالت) || U_FAILURE(*حالت) ){
            أرجع خالی_سوچک;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        Local_حافظہ<UString_Prep_کلید> کلید;
        Local_حافظہ<حرف> کلید_Name;
        Local_حافظہ<حرف> کلید_رستہ;
        إذا( کلید.allocateInsteadAndReset() == خالی_سوچک ||
            کلید_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == خالی_سوچک ||
            (رستہ != خالی_سوچک &&
             کلید_رستہ.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(رستہ)+1)) == خالی_سوچک)
         ) {
            *حالت = U_حافظہ_ونڈ_ERROR;
            usprep_unload(newProfile.getAlias());
            أرجع خالی_سوچک;
        }

        umtx_جندرا(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_انکڑے_HASHTABLE, &ڈھیر_کلید));
        إذا(profile != خالی_سوچک) {
            profile->ref_گنتی++;
            usprep_unload(newProfile.getAlias());
        }
        وإلا {
            /* initialize the key members */
            کلید->name = کلید_Name.orphan();
            uprv_strcpy(کلید->name, name);
            إذا(رستہ != خالی_سوچک){
                کلید->رستہ = کلید_رستہ.orphan();
                uprv_strcpy(کلید->رستہ, رستہ);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_گنتی = 1;
            uhash_put(SHARED_انکڑے_HASHTABLE, کلید.orphan(), profile, حالت);
        }
        umtx_unlock(&usprepMutex);
    }

    أرجع profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(ثابت حرف* رستہ, 
            ثابت حرف* name,
            UError_کوڈ* حالت){

    إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع خالی_سوچک;
    }
       
    /* initialize the profile struct members */
    أرجع usprep_getProfile(رستہ,name,حالت);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UError_کوڈ* حالت) {
    إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع خالی_سوچک;
    }
    int32_t اشاریہ = (int32_t)type;
    إذا (اشاریہ < 0 || اشاریہ >= UPRV_LENGTHOF(PROFILE_ناں)) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    أرجع usprep_open(خالی_سوچک, PROFILE_ناں[اشاریہ], حالت);
}

U_CAPI فراغ U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    إذا(profile==خالی_سوچک){
        أرجع;
    }

    umtx_جندرا(&usprepMutex);
    /* decrement the ref count*/
    إذا(profile->ref_گنتی > 0){
        profile->ref_گنتی--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC فراغ 
uprv_syntaxError(ثابت char16_t* قاعدے,
                 int32_t pos,
                 int32_t قاعدے_Len,
                 UParseError* parseError){
    إذا(parseError == خالی_سوچک){
        أرجع;
    }
    parseError->وتھ = pos;
    parseError->سطر = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t شروع = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t حد_بندی = pos;
    
    u_memcpy(parseError->preContext,قاعدے+شروع,حد_بندی-شروع);
    //null terminate the buffer
    parseError->preContext[حد_بندی-شروع] = 0;
    
    // for post-context; include error rules[pos]
    شروع = pos;
    حد_بندی = شروع + (U_PARSE_CONTEXT_LEN-1);
    إذا (حد_بندی > قاعدے_Len) {
        حد_بندی = قاعدے_Len;
    }
    إذا (شروع < قاعدے_Len) {
        u_memcpy(parseError->postContext,قاعدے+شروع,حد_بندی-شروع);
    }
    //null terminate the buffer
    parseError->postContext[حد_بندی-شروع]= 0;
}


ساكن مضمن UStringPrepType
get_اقدار(uint16_t trie_لفظ, int16_t& value, UBool& is_اشاریہ){

    UStringPrepType type;
    إذا(trie_لفظ == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_حد_بندی;
        is_اشاریہ =خطأ;
        value = 0;
    }وإلا إذا(trie_لفظ >= SPREP_TYPE_حد){
        type = static_cast<UStringPrepType>(trie_لفظ - SPREP_TYPE_حد);
        is_اشاریہ =خطأ;
        value = 0;
    }وإلا{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        إذا(trie_لفظ & 0x02){
            is_اشاریہ = سچ;
            value = trie_لفظ  >> 2; //mask off the lower 2 bits and shift
        }وإلا{
            is_اشاریہ = خطأ;
            value = static_cast<int16_t>(trie_لفظ);
            value =  (value >> 2);
        }
 
        إذا((trie_لفظ>>2) == SPREP_MAX_اشاریہ_قدر){
            type = USPREP_DELETE;
            is_اشاریہ =خطأ;
            value = 0;
        }
    }
    أرجع type;
}

// TODO: change to writing to UnicodeString not char16_t *
ساكن int32_t 
usprep_map(  ثابت UStringPrepProfile* profile, 
             ثابت char16_t* src, int32_t src_لمائی,
             char16_t* dest, int32_t dest_سمرتھا,
             int32_t options,
             UParseError* parseError,
             UError_کوڈ* حالت ){
    
    uint16_t نتیجہ;
    int32_t dest_اشاریہ=0;
    int32_t src_اشاریہ;
    UBool allowUnassigned = static_cast<UBool>((options & USPREP_ALLOW_UNASSIGNED) > 0);
    UStringPrepType type;
    int16_t value;
    UBool is_اشاریہ;
    ثابت int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    لكل(src_اشاریہ=0;src_اشاریہ<src_لمائی;){
        UChar32 ch;

        U16_اگلا(src,src_اشاریہ,src_لمائی,ch);
        
        نتیجہ=0;

        UTRIE_GET16(&profile->sprepTrie,ch,نتیجہ);
        
        type = get_اقدار(نتیجہ, value, is_اشاریہ);

        // check if the source codepoint is unassigned
        إذا(type == USPREP_UNASSIGNED && allowUnassigned == خطأ){

            uprv_syntaxError(src,src_اشاریہ-U16_لمائی(ch), src_لمائی,parseError);
            *حالت = U_STRINGPREP_UNASSIGNED_ERROR;
            أرجع 0;
            
        }وإلا إذا(type == USPREP_MAP){
            
            int32_t اشاریہ, length;

            إذا(is_اشاریہ){
                اشاریہ = value;
                إذا(اشاریہ >= indexes[SPREP_ONE_UCHAR_MAPPING_اشاریہ_شروع] &&
                         اشاریہ < indexes[SPREP_TWO_UCHARS_MAPPING_اشاریہ_شروع]){
                    length = 1;
                }وإلا إذا(اشاریہ >= indexes[SPREP_TWO_UCHARS_MAPPING_اشاریہ_شروع] &&
                         اشاریہ < indexes[SPREP_THREE_UCHARS_MAPPING_اشاریہ_شروع]){
                    length = 2;
                }وإلا إذا(اشاریہ >= indexes[SPREP_THREE_UCHARS_MAPPING_اشاریہ_شروع] &&
                         اشاریہ < indexes[SPREP_FOUR_UCHARS_MAPPING_اشاریہ_شروع]){
                    length = 3;
                }وإلا{
                    length = profile->mapping_انکڑے[اشاریہ++];
         
                }

                /* copy mapping to destination */
                لكل(int32_t i=0; i< length; i++){
                    إذا(dest_اشاریہ < dest_سمرتھا  ){
                        dest[dest_اشاریہ] = profile->mapping_انکڑے[اشاریہ+i];
                    }
                    dest_اشاریہ++; /* for pre-flighting */
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
            إذا(dest_اشاریہ < dest_سمرتھا ){
                dest[dest_اشاریہ] = static_cast<char16_t>(ch);
            }
            dest_اشاریہ++;
        }وإلا{
            إذا(dest_اشاریہ+1 < dest_سمرتھا ){
                dest[dest_اشاریہ]   = U16_LEAD(ch);
                dest[dest_اشاریہ+1] = U16_TRAIL(ch);
            }
            dest_اشاریہ +=2;
        }
       
    }
        
    أرجع u_terminateUChars(dest, dest_سمرتھا, dest_اشاریہ, حالت);
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
                  ثابت char16_t* src, int32_t src_لمائی,
                  char16_t* dest, int32_t dest_سمرتھا,
                  int32_t options,
                  UParseError* parseError,
                  UError_کوڈ* حالت ){

    // check error status
    إذا(U_FAILURE(*حالت)){
        أرجع 0;
    }

    //check arguments
    إذا(profile==خالی_سوچک ||
            (src==خالی_سوچک ? src_لمائی!=0 : src_لمائی<-1) ||
            (dest==خالی_سوچک ? dest_سمرتھا!=0 : dest_سمرتھا<0)) {
        *حالت=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    //get the string length
    إذا(src_لمائی < 0){
        src_لمائی = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.get_بفر(src_لمائی);
    إذا(b1==خالی_سوچک){
        *حالت = U_حافظہ_ونڈ_ERROR;
        أرجع 0;
    }
    UError_کوڈ بفر_حالت = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, src_لمائی,
                               b1, s1.get_سمرتھا(), options, parseError, &بفر_حالت);
    s1.release_بفر(U_SUCCESS(بفر_حالت) ? b1Len : 0);

    إذا(بفر_حالت == U_بفر_حد_توں_ودھ_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.get_بفر(b1Len);
        إذا(b1==خالی_سوچک){
            *حالت = U_حافظہ_ونڈ_ERROR;
            أرجع 0;
        }

        بفر_حالت = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, src_لمائی,
                           b1, s1.get_سمرتھا(), options, parseError, &بفر_حالت);
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
    UCharDirection direction=U_CHAR_DIRECTION_گنتی, پہلا_Char_Dir=U_CHAR_DIRECTION_گنتی;
    UBool بایاں_منزل_دایاں=خطأ, دایاں_منزل_بایاں=خطأ;
    int32_t rtlPos =-1, ltrPos =-1;

    لكل(int32_t b2_اشاریہ=0; b2_اشاریہ<b2Len;){
        UChar32 ch = 0;
        U16_اگلا(b2, b2_اشاریہ, b2Len, ch);

        uint16_t نتیجہ;
        UTRIE_GET16(&profile->sprepTrie,ch,نتیجہ);

        int16_t value;
        UBool is_اشاریہ;
        UStringPrepType type = get_اقدار(نتیجہ, value, is_اشاریہ);

        إذا( type == USPREP_PROHIBITED || 
            ((نتیجہ < SPREP_TYPE_حد) && (نتیجہ & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *حالت = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2_اشاریہ-U16_لمائی(ch), b2Len, parseError);
            أرجع 0;
        }

        إذا(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            إذا(پہلا_Char_Dir == U_CHAR_DIRECTION_گنتی){
                پہلا_Char_Dir = direction;
            }
            إذا(direction == U_بایاں_منزل_دایاں){
                بایاں_منزل_دایاں = سچ;
                ltrPos = b2_اشاریہ-1;
            }
            إذا(direction == U_دایاں_منزل_بایاں || direction == U_دایاں_منزل_بایاں_ARABIC){
                دایاں_منزل_بایاں = سچ;
                rtlPos = b2_اشاریہ-1;
            }
        }
    }
    إذا(profile->checkBiDi){
        // satisfy 2
        إذا( بایاں_منزل_دایاں && دایاں_منزل_بایاں){
            *حالت = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            أرجع 0;
        }

        //satisfy 3
        إذا( دایاں_منزل_بایاں && 
            !((پہلا_Char_Dir == U_دایاں_منزل_بایاں || پہلا_Char_Dir == U_دایاں_منزل_بایاں_ARABIC) &&
              (direction == U_دایاں_منزل_بایاں || direction == U_دایاں_منزل_بایاں_ARABIC))
           ){
            *حالت = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            أرجع خطأ;
        }
    }
    أرجع s2.extract(dest, dest_سمرتھا, *حالت);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_وٹاندرا(ثابت UDataSwapper *ds,
            ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
            UError_کوڈ *p_Error_کوڈ) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_ناپ;

    ثابت uint8_t *وچ_بائٹاں;
    uint8_t *باہرلا_بائٹاں;

    ثابت int32_t *وچ_Indexes;
    int32_t indexes[16];

    int32_t i, وتھ, count, size;

    /* udata_swapDataHeader checks the arguments */
    سرخی_ناپ=udata_وٹاندرا_انکڑے_سرخی(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)وچ_انکڑے+4);
    إذا(!(
        pInfo->انکڑے_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->انکڑے_Format[1]==0x50 &&
        pInfo->انکڑے_Format[2]==0x52 &&
        pInfo->انکڑے_Format[3]==0x50 &&
        pInfo->format_نسخہ[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->انکڑے_Format[0], pInfo->انکڑے_Format[1],
                         pInfo->انکڑے_Format[2], pInfo->انکڑے_Format[3],
                         pInfo->format_نسخہ[0]);
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    وچ_بائٹاں=(ثابت uint8_t *)وچ_انکڑے+سرخی_ناپ;
    باہرلا_بائٹاں= (باہرلا_انکڑے == خالی_سوچک ) ? خالی_سوچک : (uint8_t *)باہرلا_انکڑے+سرخی_ناپ;

    وچ_Indexes=(ثابت int32_t *)وچ_بائٹاں;

    إذا(length>=0) {
        length-=سرخی_ناپ;
        إذا(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    لكل(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, وچ_Indexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[SPREP_اشاریہ_TRIE_ناپ]+
        indexes[SPREP_اشاریہ_MAPPING_انکڑے_ناپ];

    إذا(length>=0) {
        إذا(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        /* copy the data for inaccessible bytes */
        إذا(وچ_بائٹاں!=باہرلا_بائٹاں) {
            uprv_memcpy(باہرلا_بائٹاں, وچ_بائٹاں, size);
        }

        وتھ=0;

        /* swap the int32_t indexes[] */
        count=16*4;
        ds->وٹاندرا_Array32(ds, وچ_بائٹاں, count, باہرلا_بائٹاں, p_Error_کوڈ);
        وتھ+=count;

        /* swap the UTrie */
        count=indexes[SPREP_اشاریہ_TRIE_ناپ];
        utrie_وٹاندرا(ds, وچ_بائٹاں+وتھ, count, باہرلا_بائٹاں+وتھ, p_Error_کوڈ);
        وتھ+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[SPREP_اشاریہ_MAPPING_انکڑے_ناپ];
        ds->وٹاندرا_Array16(ds, وچ_بائٹاں+وتھ, count, باہرلا_بائٹاں+وتھ, p_Error_کوڈ);
        //offset+=count;
    }

    أرجع سرخی_ناپ+size;
}

#جے_انت /* #if !UCONFIG_NO_IDNA */
