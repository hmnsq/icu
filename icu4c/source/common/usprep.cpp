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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_IDNA

#شامل "unicode/usprep.h"

#شامل "unicode/normalizer2.h"
#شامل "unicode/ustring.h"
#شامل "unicode/uchar.h"
#شامل "unicode/uversion.h"
#شامل "umutex.h"
#شامل "cmemory.h"
#شامل "sprpimpl.h"
#شامل "ustr_imp.h"
#شامل "uhash.h"
#شامل "cstring.h"
#شامل "udataswp.h"
#شامل "ucln_cmn.h"
#شامل "ubidi_props.h"
#شامل "uprops.h"

U_NAMESPACE_USE

U_CDECL_BEGIN

/*
Static cache for already opened StringPrep profiles
*/
ساکن UHashtable *SHARED_ڈیٹا_HASHTABLE = خالی_اشاریہ;
ساکن icu::UInitOnce g_Shared_ڈیٹا_Init_Once {};

ساکن UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
ساکن UVersionInfo ڈیٹا_نسخہ={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
ساکن ثابت حرف * ثابت PROFILE_نام[] = {
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

ساکن UBool U_CALLCONV
isSPrepAcceptable(خالی * /* context */,
             ثابت حرف * /* type */, 
             ثابت حرف * /* name */,
             ثابت UDataInfo *pInfo) {
    اگر(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ڈیٹا_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->ڈیٹا_Format[1]==0x50 &&
        pInfo->ڈیٹا_Format[2]==0x52 &&
        pInfo->ڈیٹا_Format[3]==0x50 &&
        pInfo->format_نسخہ[0]==3 &&
        pInfo->format_نسخہ[2]==UTRIE_سرکاؤ &&
        pInfo->format_نسخہ[3]==UTRIE_اشاریہ_سرکاؤ
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(ڈیٹا_نسخہ, pInfo->ڈیٹا_نسخہ, 4);
        واپس سچ;
    } ورنہ {
        واپس جھوٹ;
    }
}

ساکن int32_t U_CALLCONV
get_SPrep_Folding_انحراف(uint32_t ڈیٹا) {

    واپس (int32_t)ڈیٹا;

}

/* hashes an entry  */
ساکن int32_t U_CALLCONV 
hash_اندراج(ثابت UHashTok parm) {
    UString_Prep_کلید *b = (UString_Prep_کلید *)parm.اشارہ_گر;
    UHashTok namekey, pathkey;
    namekey.اشارہ_گر = b->name;
    pathkey.اشارہ_گر = b->راستہ;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    واپس static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
ساکن UBool U_CALLCONV 
compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UString_Prep_کلید *b1 = (UString_Prep_کلید *)p1.اشارہ_گر;
    UString_Prep_کلید *b2 = (UString_Prep_کلید *)p2.اشارہ_گر;
    UHashTok name1, name2, path1, path2;
    name1.اشارہ_گر = b1->name;
    name2.اشارہ_گر = b2->name;
    path1.اشارہ_گر = b1->راستہ;
    path2.اشارہ_گر = b2->راستہ;
    واپس uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

ساکن خالی 
usprep_unload(UStringPrepProfile* ڈیٹا){
    udata_close(ڈیٹا->sprep_ڈیٹا);
}

ساکن int32_t 
usprep_اندرونی_flush_Cache(UBool no_Ref_گنتی){
    UStringPrepProfile *profile = خالی_اشاریہ;
    UString_Prep_کلید  *کلید  = خالی_اشاریہ;
    int32_t pos = UHASH_پہلا;
    int32_t deleted_عدد = 0;
    ثابت UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_تالا(&usprepMutex);
    اگر (SHARED_ڈیٹا_HASHTABLE == خالی_اشاریہ) {
        umtx_unlock(&usprepMutex);
        واپس 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    جب ((e = uhash_اگلا_Element(SHARED_ڈیٹا_HASHTABLE, &pos)) != خالی_اشاریہ)
    {
        profile = (UStringPrepProfile *) e->value.اشارہ_گر;
        کلید  = (UString_Prep_کلید *) e->کلید.اشارہ_گر;

        اگر ((no_Ref_گنتی== جھوٹ && profile->ref_گنتی == 0) || 
             no_Ref_گنتی) {
            deleted_عدد++;
            uhash_removeElement(SHARED_ڈیٹا_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            اگر(کلید->name != خالی_اشاریہ) {
                uprv_free(کلید->name);
                کلید->name=خالی_اشاریہ;
            }
            اگر(کلید->راستہ != خالی_اشاریہ) {
                uprv_free(کلید->راستہ);
                کلید->راستہ=خالی_اشاریہ;
            }
            uprv_free(profile);
            uprv_free(کلید);
        }
       
    }
    umtx_unlock(&usprepMutex);

    واپس deleted_عدد;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

ساکن UBool U_CALLCONV usprep_cleanup(){
    اگر (SHARED_ڈیٹا_HASHTABLE != خالی_اشاریہ) {
        usprep_اندرونی_flush_Cache(سچ);
        اگر (SHARED_ڈیٹا_HASHTABLE != خالی_اشاریہ && uhash_گنتی(SHARED_ڈیٹا_HASHTABLE) == 0) {
            uhash_close(SHARED_ڈیٹا_HASHTABLE);
            SHARED_ڈیٹا_HASHTABLE = خالی_اشاریہ;
        }
    }
    g_Shared_ڈیٹا_Init_Once.reset();
    واپس (SHARED_ڈیٹا_HASHTABLE == خالی_اشاریہ);
}
U_CDECL_انتہا


/** Initializes the cache for resources */
ساکن خالی U_CALLCONV
createCache(UError_رمز &حالت_2) {
    SHARED_ڈیٹا_HASHTABLE = uhash_open(hash_اندراج, compareEntries, خالی_اشاریہ, &حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        SHARED_ڈیٹا_HASHTABLE = خالی_اشاریہ;
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

ساکن خالی 
initCache(UError_رمز *حالت_2) {
    umtx_initOnce(g_Shared_ڈیٹا_Init_Once, &createCache, *حالت_2);
}

ساکن UBool U_CALLCONV
بھرو_ڈیٹا(UStringPrepProfile* profile, 
         ثابت حرف* راستہ, 
         ثابت حرف* name, 
         ثابت حرف* type, 
         UError_رمز* error_رمز) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, 0, 0, 0, 0};
    UData_حافظہ *ڈیٹا_حافظہ;
    ثابت int32_t *p=خالی_اشاریہ;
    ثابت uint8_t *pb;
    UVersionInfo norm_Unicode_نسخہ;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    اگر(error_رمز==خالی_اشاریہ || U_FAILURE(*error_رمز)) {
        واپس 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    ڈیٹا_حافظہ=udata_openChoice(راستہ, type, name, isSPrepAcceptable, خالی_اشاریہ, error_رمز);
    اگر(U_FAILURE(*error_رمز)) {
        واپس جھوٹ;
    }

    p = static_cast<ثابت int32_t*>(udata_get_حافظہ(ڈیٹا_حافظہ));
    pb = reinterpret_cast<ثابت uint8_t*>(p + SPREP_اشاریہ_چوٹی);
    utrie_unserialize(&_sprepTrie, pb, p[SPREP_اشاریہ_TRIE_سائز], error_رمز);
    _sprepTrie.get_Folding_انحراف=get_SPrep_Folding_انحراف;


    اگر(U_FAILURE(*error_رمز)) {
        udata_close(ڈیٹا_حافظہ);
        واپس جھوٹ;
    }

    /* in the mutex block, set the data for this process */
    umtx_تالا(&usprepMutex);
    اگر(profile->sprep_ڈیٹا==خالی_اشاریہ) {
        profile->sprep_ڈیٹا=ڈیٹا_حافظہ;
        ڈیٹا_حافظہ=خالی_اشاریہ;
        uprv_memcpy(&profile->indexes, p, حجم(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, حجم(UTrie));
    } ورنہ {
        p = static_cast<ثابت int32_t*>(udata_get_حافظہ(profile->sprep_ڈیٹا));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mapping_ڈیٹا = reinterpret_cast<ثابت uint16_t*>(reinterpret_cast<ثابت uint8_t*>(p + SPREP_اشاریہ_چوٹی) + profile->indexes[SPREP_اشاریہ_TRIE_سائز]);
    
    u_get_Unicode_نسخہ(norm_Unicode_نسخہ);
    normUniVer = (norm_Unicode_نسخہ[0] << 24) + (norm_Unicode_نسخہ[1] << 16) + 
                 (norm_Unicode_نسخہ[2] << 8 ) + (norm_Unicode_نسخہ[3]);
    sprepUniVer = (ڈیٹا_نسخہ[0] << 24) + (ڈیٹا_نسخہ[1] << 16) + 
                  (ڈیٹا_نسخہ[2] << 8 ) + (ڈیٹا_نسخہ[3]);
    normCorrVer = profile->indexes[SPREP_NORM_CORRECTNS_آخری_UNI_نسخہ];
    
    اگر(U_FAILURE(*error_رمز)){
        udata_close(ڈیٹا_حافظہ);
        واپس جھوٹ;
    }
    اگر( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *error_رمز = U_INVALID_FORMAT_ERROR;
        udata_close(ڈیٹا_حافظہ);
        واپس جھوٹ;
    }
    profile->is_ڈیٹا_Loaded = سچ;

    /* if a different thread set it first, then close the extra data */
    اگر(ڈیٹا_حافظہ!=خالی_اشاریہ) {
        udata_close(ڈیٹا_حافظہ); /* nullptr if it was set correctly */
    }


    واپس profile->is_ڈیٹا_Loaded;
}

ساکن UStringPrepProfile* 
usprep_getProfile(ثابت حرف* راستہ, 
                  ثابت حرف* name,
                  UError_رمز *حالت_2){

    UStringPrepProfile* profile = خالی_اشاریہ;

    initCache(حالت_2);

    اگر(U_FAILURE(*حالت_2)){
        واپس خالی_اشاریہ;
    }

    UString_Prep_کلید ڈھیر_کلید;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    ڈھیر_کلید.name = const_cast<حرف*>(name);
    ڈھیر_کلید.راستہ = const_cast<حرف*>(راستہ);

    /* fetch the data from the cache */
    umtx_تالا(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_ڈیٹا_HASHTABLE, &ڈھیر_کلید));
    اگر(profile != خالی_اشاریہ) {
        profile->ref_گنتی++;
    }
    umtx_unlock(&usprepMutex);
    
    اگر(profile == خالی_اشاریہ) {
        /* else load the data and put the data in the cache */
        Local_حافظہ<UStringPrepProfile> newProfile;
        اگر(newProfile.allocateInsteadAndReset() == خالی_اشاریہ) {
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس خالی_اشاریہ;
        }

        /* load the data */
        اگر(!بھرو_ڈیٹا(newProfile.getAlias(), راستہ, name, SPREP_ڈیٹا_TYPE, حالت_2) || U_FAILURE(*حالت_2) ){
            واپس خالی_اشاریہ;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        Local_حافظہ<UString_Prep_کلید> کلید;
        Local_حافظہ<حرف> کلید_Name;
        Local_حافظہ<حرف> کلید_راستہ;
        اگر( کلید.allocateInsteadAndReset() == خالی_اشاریہ ||
            کلید_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == خالی_اشاریہ ||
            (راستہ != خالی_اشاریہ &&
             کلید_راستہ.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(راستہ)+1)) == خالی_اشاریہ)
         ) {
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            usprep_unload(newProfile.getAlias());
            واپس خالی_اشاریہ;
        }

        umtx_تالا(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_ڈیٹا_HASHTABLE, &ڈھیر_کلید));
        اگر(profile != خالی_اشاریہ) {
            profile->ref_گنتی++;
            usprep_unload(newProfile.getAlias());
        }
        ورنہ {
            /* initialize the key members */
            کلید->name = کلید_Name.orphan();
            uprv_strcpy(کلید->name, name);
            اگر(راستہ != خالی_اشاریہ){
                کلید->راستہ = کلید_راستہ.orphan();
                uprv_strcpy(کلید->راستہ, راستہ);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_گنتی = 1;
            uhash_put(SHARED_ڈیٹا_HASHTABLE, کلید.orphan(), profile, حالت_2);
        }
        umtx_unlock(&usprepMutex);
    }

    واپس profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(ثابت حرف* راستہ, 
            ثابت حرف* name,
            UError_رمز* حالت_2){

    اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس خالی_اشاریہ;
    }
       
    /* initialize the profile struct members */
    واپس usprep_getProfile(راستہ,name,حالت_2);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UError_رمز* حالت_2) {
    اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس خالی_اشاریہ;
    }
    int32_t اشاریہ = (int32_t)type;
    اگر (اشاریہ < 0 || اشاریہ >= UPRV_LENGTHOF(PROFILE_نام)) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    واپس usprep_open(خالی_اشاریہ, PROFILE_نام[اشاریہ], حالت_2);
}

U_CAPI خالی U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    اگر(profile==خالی_اشاریہ){
        واپس;
    }

    umtx_تالا(&usprepMutex);
    /* decrement the ref count*/
    اگر(profile->ref_گنتی > 0){
        profile->ref_گنتی--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC خالی 
uprv_syntaxError(ثابت char16_t* قواعد,
                 int32_t pos,
                 int32_t قواعد_Len,
                 UParseError* parseError){
    اگر(parseError == خالی_اشاریہ){
        واپس;
    }
    parseError->انحراف = pos;
    parseError->سطر = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t آغاز = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t حد_بندی = pos;
    
    u_memcpy(parseError->preContext,قواعد+آغاز,حد_بندی-آغاز);
    //null terminate the buffer
    parseError->preContext[حد_بندی-آغاز] = 0;
    
    // for post-context; include error rules[pos]
    آغاز = pos;
    حد_بندی = آغاز + (U_PARSE_CONTEXT_LEN-1);
    اگر (حد_بندی > قواعد_Len) {
        حد_بندی = قواعد_Len;
    }
    اگر (آغاز < قواعد_Len) {
        u_memcpy(parseError->postContext,قواعد+آغاز,حد_بندی-آغاز);
    }
    //null terminate the buffer
    parseError->postContext[حد_بندی-آغاز]= 0;
}


ساکن اندرونی UStringPrepType
get_اقدار(uint16_t trie_لفظ, int16_t& value, UBool& is_اشاریہ){

    UStringPrepType type;
    اگر(trie_لفظ == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_حد_بندی;
        is_اشاریہ =جھوٹ;
        value = 0;
    }ورنہ اگر(trie_لفظ >= SPREP_TYPE_حد){
        type = static_cast<UStringPrepType>(trie_لفظ - SPREP_TYPE_حد);
        is_اشاریہ =جھوٹ;
        value = 0;
    }ورنہ{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        اگر(trie_لفظ & 0x02){
            is_اشاریہ = سچ;
            value = trie_لفظ  >> 2; //mask off the lower 2 bits and shift
        }ورنہ{
            is_اشاریہ = جھوٹ;
            value = static_cast<int16_t>(trie_لفظ);
            value =  (value >> 2);
        }
 
        اگر((trie_لفظ>>2) == SPREP_MAX_اشاریہ_قدر){
            type = USPREP_DELETE;
            is_اشاریہ =جھوٹ;
            value = 0;
        }
    }
    واپس type;
}

// TODO: change to writing to UnicodeString not char16_t *
ساکن int32_t 
usprep_map(  ثابت UStringPrepProfile* profile, 
             ثابت char16_t* src, int32_t src_لمبائی,
             char16_t* dest, int32_t dest_استعداد,
             int32_t options,
             UParseError* parseError,
             UError_رمز* حالت_2 ){
    
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

    برائے(src_اشاریہ=0;src_اشاریہ<src_لمبائی;){
        UChar32 ch;

        U16_اگلا(src,src_اشاریہ,src_لمبائی,ch);
        
        نتیجہ=0;

        UTRIE_GET16(&profile->sprepTrie,ch,نتیجہ);
        
        type = get_اقدار(نتیجہ, value, is_اشاریہ);

        // check if the source codepoint is unassigned
        اگر(type == USPREP_UNASSIGNED && allowUnassigned == جھوٹ){

            uprv_syntaxError(src,src_اشاریہ-U16_لمبائی(ch), src_لمبائی,parseError);
            *حالت_2 = U_STRINGPREP_UNASSIGNED_ERROR;
            واپس 0;
            
        }ورنہ اگر(type == USPREP_MAP){
            
            int32_t اشاریہ, length;

            اگر(is_اشاریہ){
                اشاریہ = value;
                اگر(اشاریہ >= indexes[SPREP_ONE_UCHAR_MAPPING_اشاریہ_آغاز] &&
                         اشاریہ < indexes[SPREP_TWO_UCHARS_MAPPING_اشاریہ_آغاز]){
                    length = 1;
                }ورنہ اگر(اشاریہ >= indexes[SPREP_TWO_UCHARS_MAPPING_اشاریہ_آغاز] &&
                         اشاریہ < indexes[SPREP_THREE_UCHARS_MAPPING_اشاریہ_آغاز]){
                    length = 2;
                }ورنہ اگر(اشاریہ >= indexes[SPREP_THREE_UCHARS_MAPPING_اشاریہ_آغاز] &&
                         اشاریہ < indexes[SPREP_FOUR_UCHARS_MAPPING_اشاریہ_آغاز]){
                    length = 3;
                }ورنہ{
                    length = profile->mapping_ڈیٹا[اشاریہ++];
         
                }

                /* copy mapping to destination */
                برائے(int32_t i=0; i< length; i++){
                    اگر(dest_اشاریہ < dest_استعداد  ){
                        dest[dest_اشاریہ] = profile->mapping_ڈیٹا[اشاریہ+i];
                    }
                    dest_اشاریہ++; /* for pre-flighting */
                }  
                جاری;
            }ورنہ{
                // subtract the delta to arrive at the code point
                ch -= value;
            }

        }ورنہ اگر(type==USPREP_DELETE){
             // just consume the codepoint and continue
            جاری;
        }
        //copy the code point into destination
        اگر(ch <= 0xFFFF){
            اگر(dest_اشاریہ < dest_استعداد ){
                dest[dest_اشاریہ] = static_cast<char16_t>(ch);
            }
            dest_اشاریہ++;
        }ورنہ{
            اگر(dest_اشاریہ+1 < dest_استعداد ){
                dest[dest_اشاریہ]   = U16_LEAD(ch);
                dest[dest_اشاریہ+1] = U16_TRAIL(ch);
            }
            dest_اشاریہ +=2;
        }
       
    }
        
    واپس u_terminateUChars(dest, dest_استعداد, dest_اشاریہ, حالت_2);
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
                  ثابت char16_t* src, int32_t src_لمبائی,
                  char16_t* dest, int32_t dest_استعداد,
                  int32_t options,
                  UParseError* parseError,
                  UError_رمز* حالت_2 ){

    // check error status
    اگر(U_FAILURE(*حالت_2)){
        واپس 0;
    }

    //check arguments
    اگر(profile==خالی_اشاریہ ||
            (src==خالی_اشاریہ ? src_لمبائی!=0 : src_لمبائی<-1) ||
            (dest==خالی_اشاریہ ? dest_استعداد!=0 : dest_استعداد<0)) {
        *حالت_2=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    //get the string length
    اگر(src_لمبائی < 0){
        src_لمبائی = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.get_بفر(src_لمبائی);
    اگر(b1==خالی_اشاریہ){
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس 0;
    }
    UError_رمز بفر_حالت = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, src_لمبائی,
                               b1, s1.get_استعداد(), options, parseError, &بفر_حالت);
    s1.release_بفر(U_SUCCESS(بفر_حالت) ? b1Len : 0);

    اگر(بفر_حالت == U_بفر_حد_سے_تجاوز_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.get_بفر(b1Len);
        اگر(b1==خالی_اشاریہ){
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس 0;
        }

        بفر_حالت = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, src_لمبائی,
                           b1, s1.get_استعداد(), options, parseError, &بفر_حالت);
        s1.release_بفر(U_SUCCESS(بفر_حالت) ? b1Len : 0);
    }
    اگر(U_FAILURE(بفر_حالت)){
        *حالت_2 = بفر_حالت;
        واپس 0;
    }

    // normalize
    UnicodeString s2;
    اگر(profile->doNFKC){
        ثابت Normalizer2 *n2 = Normalizer2::getNFKCInstance(*حالت_2);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*حالت_2));
        اگر(U_FAILURE(*حالت_2)){
            واپس 0;
        }
        fn2.normalize(s1, s2, *حالت_2);
    }ورنہ{
        s2.fastCopyFrom(s1);
    }
    اگر(U_FAILURE(*حالت_2)){
        واپس 0;
    }

    // Prohibit and checkBiDi in one pass
    ثابت char16_t *b2 = s2.get_بفر();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_گنتی, پہلا_Char_Dir=U_CHAR_DIRECTION_گنتی;
    UBool بایاں_مقصد_دایاں=جھوٹ, دایاں_مقصد_بایاں=جھوٹ;
    int32_t rtlPos =-1, ltrPos =-1;

    برائے(int32_t b2_اشاریہ=0; b2_اشاریہ<b2Len;){
        UChar32 ch = 0;
        U16_اگلا(b2, b2_اشاریہ, b2Len, ch);

        uint16_t نتیجہ;
        UTRIE_GET16(&profile->sprepTrie,ch,نتیجہ);

        int16_t value;
        UBool is_اشاریہ;
        UStringPrepType type = get_اقدار(نتیجہ, value, is_اشاریہ);

        اگر( type == USPREP_PROHIBITED || 
            ((نتیجہ < SPREP_TYPE_حد) && (نتیجہ & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *حالت_2 = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2_اشاریہ-U16_لمبائی(ch), b2Len, parseError);
            واپس 0;
        }

        اگر(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            اگر(پہلا_Char_Dir == U_CHAR_DIRECTION_گنتی){
                پہلا_Char_Dir = direction;
            }
            اگر(direction == U_بایاں_مقصد_دایاں){
                بایاں_مقصد_دایاں = سچ;
                ltrPos = b2_اشاریہ-1;
            }
            اگر(direction == U_دایاں_مقصد_بایاں || direction == U_دایاں_مقصد_بایاں_ARABIC){
                دایاں_مقصد_بایاں = سچ;
                rtlPos = b2_اشاریہ-1;
            }
        }
    }
    اگر(profile->checkBiDi){
        // satisfy 2
        اگر( بایاں_مقصد_دایاں && دایاں_مقصد_بایاں){
            *حالت_2 = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            واپس 0;
        }

        //satisfy 3
        اگر( دایاں_مقصد_بایاں && 
            !((پہلا_Char_Dir == U_دایاں_مقصد_بایاں || پہلا_Char_Dir == U_دایاں_مقصد_بایاں_ARABIC) &&
              (direction == U_دایاں_مقصد_بایاں || direction == U_دایاں_مقصد_بایاں_ARABIC))
           ){
            *حالت_2 = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            واپس جھوٹ;
        }
    }
    واپس s2.extract(dest, dest_استعداد, *حالت_2);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_تبادلہ(ثابت UDataSwapper *ds,
            ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
            UError_رمز *p_Error_رمز) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_سائز;

    ثابت uint8_t *اندر_بائٹیں;
    uint8_t *برآمد_بائٹیں;

    ثابت int32_t *اندر_Indexes;
    int32_t indexes[16];

    int32_t i, انحراف, count, size;

    /* udata_swapDataHeader checks the arguments */
    سرخی_سائز=udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندر_ڈیٹا+4);
    اگر(!(
        pInfo->ڈیٹا_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->ڈیٹا_Format[1]==0x50 &&
        pInfo->ڈیٹا_Format[2]==0x52 &&
        pInfo->ڈیٹا_Format[3]==0x50 &&
        pInfo->format_نسخہ[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->ڈیٹا_Format[0], pInfo->ڈیٹا_Format[1],
                         pInfo->ڈیٹا_Format[2], pInfo->ڈیٹا_Format[3],
                         pInfo->format_نسخہ[0]);
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    اندر_بائٹیں=(ثابت uint8_t *)اندر_ڈیٹا+سرخی_سائز;
    برآمد_بائٹیں= (برآمد_ڈیٹا == خالی_اشاریہ ) ? خالی_اشاریہ : (uint8_t *)برآمد_ڈیٹا+سرخی_سائز;

    اندر_Indexes=(ثابت int32_t *)اندر_بائٹیں;

    اگر(length>=0) {
        length-=سرخی_سائز;
        اگر(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    برائے(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, اندر_Indexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[SPREP_اشاریہ_TRIE_سائز]+
        indexes[SPREP_اشاریہ_MAPPING_ڈیٹا_سائز];

    اگر(length>=0) {
        اگر(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }

        /* copy the data for inaccessible bytes */
        اگر(اندر_بائٹیں!=برآمد_بائٹیں) {
            uprv_memcpy(برآمد_بائٹیں, اندر_بائٹیں, size);
        }

        انحراف=0;

        /* swap the int32_t indexes[] */
        count=16*4;
        ds->تبادلہ_Array32(ds, اندر_بائٹیں, count, برآمد_بائٹیں, p_Error_رمز);
        انحراف+=count;

        /* swap the UTrie */
        count=indexes[SPREP_اشاریہ_TRIE_سائز];
        utrie_تبادلہ(ds, اندر_بائٹیں+انحراف, count, برآمد_بائٹیں+انحراف, p_Error_رمز);
        انحراف+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[SPREP_اشاریہ_MAPPING_ڈیٹا_سائز];
        ds->تبادلہ_Array16(ds, اندر_بائٹیں+انحراف, count, برآمد_بائٹیں+انحراف, p_Error_رمز);
        //offset+=count;
    }

    واپس سرخی_سائز+size;
}

#اگر_ختم /* #if !UCONFIG_NO_IDNA */
