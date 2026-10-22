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
ساكن UHashtable *SHARED_داده_HASHTABLE = اشاره_تهی;
ساكن icu::UInitOnce g_Shared_داده_Init_Once {};

ساكن UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
ساكن UVersionInfo داده_نسخه={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
ساكن ثابت حرف * ثابت PROFILE_نامها[] = {
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
        pInfo->داده_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->داده_Format[1]==0x50 &&
        pInfo->داده_Format[2]==0x52 &&
        pInfo->داده_Format[3]==0x50 &&
        pInfo->format_نسخه[0]==3 &&
        pInfo->format_نسخه[2]==UTRIE_لغزش &&
        pInfo->format_نسخه[3]==UTRIE_شاخص_لغزش
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(داده_نسخه, pInfo->داده_نسخه, 4);
        أرجع درست;
    } وإلا {
        أرجع خطأ;
    }
}

ساكن int32_t U_CALLCONV
get_SPrep_Folding_جابه_جایی(uint32_t داده) {

    أرجع (int32_t)داده;

}

/* hashes an entry  */
ساكن int32_t U_CALLCONV 
hash_مدخل(ثابت UHashTok parm) {
    UString_Prep_کلید *b = (UString_Prep_کلید *)parm.اشاره_گر;
    UHashTok namekey, pathkey;
    namekey.اشاره_گر = b->name;
    pathkey.اشاره_گر = b->مسیر;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    أرجع static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
ساكن UBool U_CALLCONV 
compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UString_Prep_کلید *b1 = (UString_Prep_کلید *)p1.اشاره_گر;
    UString_Prep_کلید *b2 = (UString_Prep_کلید *)p2.اشاره_گر;
    UHashTok name1, name2, path1, path2;
    name1.اشاره_گر = b1->name;
    name2.اشاره_گر = b2->name;
    path1.اشاره_گر = b1->مسیر;
    path2.اشاره_گر = b2->مسیر;
    أرجع uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

ساكن فراغ 
usprep_unload(UStringPrepProfile* داده){
    udata_close(داده->sprep_داده);
}

ساكن int32_t 
usprep_داخلی_flush_Cache(UBool no_Ref_شمار){
    UStringPrepProfile *profile = اشاره_تهی;
    UString_Prep_کلید  *کلید  = اشاره_تهی;
    int32_t pos = UHASH_اولی;
    int32_t deleted_رقم = 0;
    ثابت UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_قفل(&usprepMutex);
    إذا (SHARED_داده_HASHTABLE == اشاره_تهی) {
        umtx_unlock(&usprepMutex);
        أرجع 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    طالما ((e = uhash_بعدی_Element(SHARED_داده_HASHTABLE, &pos)) != اشاره_تهی)
    {
        profile = (UStringPrepProfile *) e->value.اشاره_گر;
        کلید  = (UString_Prep_کلید *) e->کلید.اشاره_گر;

        إذا ((no_Ref_شمار== خطأ && profile->ref_شمار == 0) || 
             no_Ref_شمار) {
            deleted_رقم++;
            uhash_removeElement(SHARED_داده_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            إذا(کلید->name != اشاره_تهی) {
                uprv_free(کلید->name);
                کلید->name=اشاره_تهی;
            }
            إذا(کلید->مسیر != اشاره_تهی) {
                uprv_free(کلید->مسیر);
                کلید->مسیر=اشاره_تهی;
            }
            uprv_free(profile);
            uprv_free(کلید);
        }
       
    }
    umtx_unlock(&usprepMutex);

    أرجع deleted_رقم;
}

/* Works just like ucnv_flushCache() 
static int32_t 
usprep_flushCache(){
    return usprep_internal_flushCache(false);
}
*/

ساكن UBool U_CALLCONV usprep_cleanup(){
    إذا (SHARED_داده_HASHTABLE != اشاره_تهی) {
        usprep_داخلی_flush_Cache(درست);
        إذا (SHARED_داده_HASHTABLE != اشاره_تهی && uhash_شمار(SHARED_داده_HASHTABLE) == 0) {
            uhash_close(SHARED_داده_HASHTABLE);
            SHARED_داده_HASHTABLE = اشاره_تهی;
        }
    }
    g_Shared_داده_Init_Once.reset();
    أرجع (SHARED_داده_HASHTABLE == اشاره_تهی);
}
U_CDECL_انتها


/** Initializes the cache for resources */
ساكن فراغ U_CALLCONV
createCache(UError_کد &وضعیت) {
    SHARED_داده_HASHTABLE = uhash_open(hash_مدخل, compareEntries, اشاره_تهی, &وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        SHARED_داده_HASHTABLE = اشاره_تهی;
    }
    ucln_common_ثبات_Cleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

ساكن فراغ 
initCache(UError_کد *وضعیت) {
    umtx_initOnce(g_Shared_داده_Init_Once, &createCache, *وضعیت);
}

ساكن UBool U_CALLCONV
بارگیری_داده(UStringPrepProfile* profile, 
         ثابت حرف* مسیر, 
         ثابت حرف* name, 
         ثابت حرف* type, 
         UError_کد* error_کد) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {اشاره_تهی, اشاره_تهی, اشاره_تهی, 0, 0, 0, 0};
    UData_حافظه *داده_حافظه;
    ثابت int32_t *p=اشاره_تهی;
    ثابت uint8_t *pb;
    UVersionInfo norm_Unicode_نسخه;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    إذا(error_کد==اشاره_تهی || U_FAILURE(*error_کد)) {
        أرجع 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    داده_حافظه=udata_openChoice(مسیر, type, name, isSPrepAcceptable, اشاره_تهی, error_کد);
    إذا(U_FAILURE(*error_کد)) {
        أرجع خطأ;
    }

    p = static_cast<ثابت int32_t*>(udata_get_حافظه(داده_حافظه));
    pb = reinterpret_cast<ثابت uint8_t*>(p + SPREP_شاخص_بالاترین);
    utrie_unserialize(&_sprepTrie, pb, p[SPREP_شاخص_TRIE_اندازه], error_کد);
    _sprepTrie.get_Folding_جابه_جایی=get_SPrep_Folding_جابه_جایی;


    إذا(U_FAILURE(*error_کد)) {
        udata_close(داده_حافظه);
        أرجع خطأ;
    }

    /* in the mutex block, set the data for this process */
    umtx_قفل(&usprepMutex);
    إذا(profile->sprep_داده==اشاره_تهی) {
        profile->sprep_داده=داده_حافظه;
        داده_حافظه=اشاره_تهی;
        uprv_memcpy(&profile->indexes, p, حجم(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, حجم(UTrie));
    } وإلا {
        p = static_cast<ثابت int32_t*>(udata_get_حافظه(profile->sprep_داده));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mapping_داده = reinterpret_cast<ثابت uint16_t*>(reinterpret_cast<ثابت uint8_t*>(p + SPREP_شاخص_بالاترین) + profile->indexes[SPREP_شاخص_TRIE_اندازه]);
    
    u_get_Unicode_نسخه(norm_Unicode_نسخه);
    normUniVer = (norm_Unicode_نسخه[0] << 24) + (norm_Unicode_نسخه[1] << 16) + 
                 (norm_Unicode_نسخه[2] << 8 ) + (norm_Unicode_نسخه[3]);
    sprepUniVer = (داده_نسخه[0] << 24) + (داده_نسخه[1] << 16) + 
                  (داده_نسخه[2] << 8 ) + (داده_نسخه[3]);
    normCorrVer = profile->indexes[SPREP_NORM_CORRECTNS_آخرین_UNI_نسخه];
    
    إذا(U_FAILURE(*error_کد)){
        udata_close(داده_حافظه);
        أرجع خطأ;
    }
    إذا( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *error_کد = U_INVALID_FORMAT_ERROR;
        udata_close(داده_حافظه);
        أرجع خطأ;
    }
    profile->is_داده_Loaded = درست;

    /* if a different thread set it first, then close the extra data */
    إذا(داده_حافظه!=اشاره_تهی) {
        udata_close(داده_حافظه); /* nullptr if it was set correctly */
    }


    أرجع profile->is_داده_Loaded;
}

ساكن UStringPrepProfile* 
usprep_getProfile(ثابت حرف* مسیر, 
                  ثابت حرف* name,
                  UError_کد *وضعیت){

    UStringPrepProfile* profile = اشاره_تهی;

    initCache(وضعیت);

    إذا(U_FAILURE(*وضعیت)){
        أرجع اشاره_تهی;
    }

    UString_Prep_کلید پشته_کلید;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    پشته_کلید.name = const_cast<حرف*>(name);
    پشته_کلید.مسیر = const_cast<حرف*>(مسیر);

    /* fetch the data from the cache */
    umtx_قفل(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_داده_HASHTABLE, &پشته_کلید));
    إذا(profile != اشاره_تهی) {
        profile->ref_شمار++;
    }
    umtx_unlock(&usprepMutex);
    
    إذا(profile == اشاره_تهی) {
        /* else load the data and put the data in the cache */
        Local_حافظه<UStringPrepProfile> newProfile;
        إذا(newProfile.allocateInsteadAndReset() == اشاره_تهی) {
            *وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع اشاره_تهی;
        }

        /* load the data */
        إذا(!بارگیری_داده(newProfile.getAlias(), مسیر, name, SPREP_داده_TYPE, وضعیت) || U_FAILURE(*وضعیت) ){
            أرجع اشاره_تهی;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        Local_حافظه<UString_Prep_کلید> کلید;
        Local_حافظه<حرف> کلید_Name;
        Local_حافظه<حرف> کلید_مسیر;
        إذا( کلید.allocateInsteadAndReset() == اشاره_تهی ||
            کلید_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == اشاره_تهی ||
            (مسیر != اشاره_تهی &&
             کلید_مسیر.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(مسیر)+1)) == اشاره_تهی)
         ) {
            *وضعیت = U_حافظه_تخصیص_ERROR;
            usprep_unload(newProfile.getAlias());
            أرجع اشاره_تهی;
        }

        umtx_قفل(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_داده_HASHTABLE, &پشته_کلید));
        إذا(profile != اشاره_تهی) {
            profile->ref_شمار++;
            usprep_unload(newProfile.getAlias());
        }
        وإلا {
            /* initialize the key members */
            کلید->name = کلید_Name.orphan();
            uprv_strcpy(کلید->name, name);
            إذا(مسیر != اشاره_تهی){
                کلید->مسیر = کلید_مسیر.orphan();
                uprv_strcpy(کلید->مسیر, مسیر);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_شمار = 1;
            uhash_put(SHARED_داده_HASHTABLE, کلید.orphan(), profile, وضعیت);
        }
        umtx_unlock(&usprepMutex);
    }

    أرجع profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(ثابت حرف* مسیر, 
            ثابت حرف* name,
            UError_کد* وضعیت){

    إذا(وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)){
        أرجع اشاره_تهی;
    }
       
    /* initialize the profile struct members */
    أرجع usprep_getProfile(مسیر,name,وضعیت);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UError_کد* وضعیت) {
    إذا(وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)){
        أرجع اشاره_تهی;
    }
    int32_t شاخص = (int32_t)type;
    إذا (شاخص < 0 || شاخص >= UPRV_LENGTHOF(PROFILE_نامها)) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    أرجع usprep_open(اشاره_تهی, PROFILE_نامها[شاخص], وضعیت);
}

U_CAPI فراغ U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    إذا(profile==اشاره_تهی){
        أرجع;
    }

    umtx_قفل(&usprepMutex);
    /* decrement the ref count*/
    إذا(profile->ref_شمار > 0){
        profile->ref_شمار--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC فراغ 
uprv_syntaxError(ثابت char16_t* قواعد,
                 int32_t pos,
                 int32_t قواعد_Len,
                 UParseError* parseError){
    إذا(parseError == اشاره_تهی){
        أرجع;
    }
    parseError->جابه_جایی = pos;
    parseError->سطر = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t شروع = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t حد = pos;
    
    u_memcpy(parseError->preContext,قواعد+شروع,حد-شروع);
    //null terminate the buffer
    parseError->preContext[حد-شروع] = 0;
    
    // for post-context; include error rules[pos]
    شروع = pos;
    حد = شروع + (U_PARSE_CONTEXT_LEN-1);
    إذا (حد > قواعد_Len) {
        حد = قواعد_Len;
    }
    إذا (شروع < قواعد_Len) {
        u_memcpy(parseError->postContext,قواعد+شروع,حد-شروع);
    }
    //null terminate the buffer
    parseError->postContext[حد-شروع]= 0;
}


ساكن مضمن UStringPrepType
get_مقادیر(uint16_t trie_کلمه, int16_t& value, UBool& is_شاخص){

    UStringPrepType type;
    إذا(trie_کلمه == 0){
        /* 
         * Initial value stored in the mapping table 
         * just return USPREP_TYPE_LIMIT .. so that
         * the source codepoint is copied to the destination
         */
        type = USPREP_TYPE_حد;
        is_شاخص =خطأ;
        value = 0;
    }وإلا إذا(trie_کلمه >= SPREP_TYPE_آستانه){
        type = static_cast<UStringPrepType>(trie_کلمه - SPREP_TYPE_آستانه);
        is_شاخص =خطأ;
        value = 0;
    }وإلا{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        إذا(trie_کلمه & 0x02){
            is_شاخص = درست;
            value = trie_کلمه  >> 2; //mask off the lower 2 bits and shift
        }وإلا{
            is_شاخص = خطأ;
            value = static_cast<int16_t>(trie_کلمه);
            value =  (value >> 2);
        }
 
        إذا((trie_کلمه>>2) == SPREP_MAX_شاخص_مقدار){
            type = USPREP_DELETE;
            is_شاخص =خطأ;
            value = 0;
        }
    }
    أرجع type;
}

// TODO: change to writing to UnicodeString not char16_t *
ساكن int32_t 
usprep_map(  ثابت UStringPrepProfile* profile, 
             ثابت char16_t* src, int32_t src_درازی,
             char16_t* dest, int32_t dest_ظرفیت,
             int32_t options,
             UParseError* parseError,
             UError_کد* وضعیت ){
    
    uint16_t نتیجه;
    int32_t dest_شاخص=0;
    int32_t src_شاخص;
    UBool allowUnassigned = static_cast<UBool>((options & USPREP_ALLOW_UNASSIGNED) > 0);
    UStringPrepType type;
    int16_t value;
    UBool is_شاخص;
    ثابت int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    لكل(src_شاخص=0;src_شاخص<src_درازی;){
        UChar32 ch;

        U16_بعدی(src,src_شاخص,src_درازی,ch);
        
        نتیجه=0;

        UTRIE_GET16(&profile->sprepTrie,ch,نتیجه);
        
        type = get_مقادیر(نتیجه, value, is_شاخص);

        // check if the source codepoint is unassigned
        إذا(type == USPREP_UNASSIGNED && allowUnassigned == خطأ){

            uprv_syntaxError(src,src_شاخص-U16_درازی(ch), src_درازی,parseError);
            *وضعیت = U_STRINGPREP_UNASSIGNED_ERROR;
            أرجع 0;
            
        }وإلا إذا(type == USPREP_MAP){
            
            int32_t شاخص, length;

            إذا(is_شاخص){
                شاخص = value;
                إذا(شاخص >= indexes[SPREP_ONE_UCHAR_MAPPING_شاخص_شروع] &&
                         شاخص < indexes[SPREP_TWO_UCHARS_MAPPING_شاخص_شروع]){
                    length = 1;
                }وإلا إذا(شاخص >= indexes[SPREP_TWO_UCHARS_MAPPING_شاخص_شروع] &&
                         شاخص < indexes[SPREP_THREE_UCHARS_MAPPING_شاخص_شروع]){
                    length = 2;
                }وإلا إذا(شاخص >= indexes[SPREP_THREE_UCHARS_MAPPING_شاخص_شروع] &&
                         شاخص < indexes[SPREP_FOUR_UCHARS_MAPPING_شاخص_شروع]){
                    length = 3;
                }وإلا{
                    length = profile->mapping_داده[شاخص++];
         
                }

                /* copy mapping to destination */
                لكل(int32_t i=0; i< length; i++){
                    إذا(dest_شاخص < dest_ظرفیت  ){
                        dest[dest_شاخص] = profile->mapping_داده[شاخص+i];
                    }
                    dest_شاخص++; /* for pre-flighting */
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
            إذا(dest_شاخص < dest_ظرفیت ){
                dest[dest_شاخص] = static_cast<char16_t>(ch);
            }
            dest_شاخص++;
        }وإلا{
            إذا(dest_شاخص+1 < dest_ظرفیت ){
                dest[dest_شاخص]   = U16_LEAD(ch);
                dest[dest_شاخص+1] = U16_TRAIL(ch);
            }
            dest_شاخص +=2;
        }
       
    }
        
    أرجع u_terminateUChars(dest, dest_ظرفیت, dest_شاخص, وضعیت);
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
                  ثابت char16_t* src, int32_t src_درازی,
                  char16_t* dest, int32_t dest_ظرفیت,
                  int32_t options,
                  UParseError* parseError,
                  UError_کد* وضعیت ){

    // check error status
    إذا(U_FAILURE(*وضعیت)){
        أرجع 0;
    }

    //check arguments
    إذا(profile==اشاره_تهی ||
            (src==اشاره_تهی ? src_درازی!=0 : src_درازی<-1) ||
            (dest==اشاره_تهی ? dest_ظرفیت!=0 : dest_ظرفیت<0)) {
        *وضعیت=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    //get the string length
    إذا(src_درازی < 0){
        src_درازی = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.get_میانگیر(src_درازی);
    إذا(b1==اشاره_تهی){
        *وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع 0;
    }
    UError_کد میانگیر_وضعیت = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, src_درازی,
                               b1, s1.get_ظرفیت(), options, parseError, &میانگیر_وضعیت);
    s1.release_میانگیر(U_SUCCESS(میانگیر_وضعیت) ? b1Len : 0);

    إذا(میانگیر_وضعیت == U_میانگیر_سرریز_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.get_میانگیر(b1Len);
        إذا(b1==اشاره_تهی){
            *وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع 0;
        }

        میانگیر_وضعیت = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, src_درازی,
                           b1, s1.get_ظرفیت(), options, parseError, &میانگیر_وضعیت);
        s1.release_میانگیر(U_SUCCESS(میانگیر_وضعیت) ? b1Len : 0);
    }
    إذا(U_FAILURE(میانگیر_وضعیت)){
        *وضعیت = میانگیر_وضعیت;
        أرجع 0;
    }

    // normalize
    UnicodeString s2;
    إذا(profile->doNFKC){
        ثابت Normalizer2 *n2 = Normalizer2::getNFKCInstance(*وضعیت);
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*وضعیت));
        إذا(U_FAILURE(*وضعیت)){
            أرجع 0;
        }
        fn2.normalize(s1, s2, *وضعیت);
    }وإلا{
        s2.fastCopyFrom(s1);
    }
    إذا(U_FAILURE(*وضعیت)){
        أرجع 0;
    }

    // Prohibit and checkBiDi in one pass
    ثابت char16_t *b2 = s2.get_میانگیر();
    int32_t b2Len = s2.length();
    UCharDirection direction=U_CHAR_DIRECTION_شمار, اولی_Char_Dir=U_CHAR_DIRECTION_شمار;
    UBool چپ_مقصد_راست=خطأ, راست_مقصد_چپ=خطأ;
    int32_t rtlPos =-1, ltrPos =-1;

    لكل(int32_t b2_شاخص=0; b2_شاخص<b2Len;){
        UChar32 ch = 0;
        U16_بعدی(b2, b2_شاخص, b2Len, ch);

        uint16_t نتیجه;
        UTRIE_GET16(&profile->sprepTrie,ch,نتیجه);

        int16_t value;
        UBool is_شاخص;
        UStringPrepType type = get_مقادیر(نتیجه, value, is_شاخص);

        إذا( type == USPREP_PROHIBITED || 
            ((نتیجه < SPREP_TYPE_آستانه) && (نتیجه & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *وضعیت = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2_شاخص-U16_درازی(ch), b2Len, parseError);
            أرجع 0;
        }

        إذا(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            إذا(اولی_Char_Dir == U_CHAR_DIRECTION_شمار){
                اولی_Char_Dir = direction;
            }
            إذا(direction == U_چپ_مقصد_راست){
                چپ_مقصد_راست = درست;
                ltrPos = b2_شاخص-1;
            }
            إذا(direction == U_راست_مقصد_چپ || direction == U_راست_مقصد_چپ_ARABIC){
                راست_مقصد_چپ = درست;
                rtlPos = b2_شاخص-1;
            }
        }
    }
    إذا(profile->checkBiDi){
        // satisfy 2
        إذا( چپ_مقصد_راست && راست_مقصد_چپ){
            *وضعیت = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            أرجع 0;
        }

        //satisfy 3
        إذا( راست_مقصد_چپ && 
            !((اولی_Char_Dir == U_راست_مقصد_چپ || اولی_Char_Dir == U_راست_مقصد_چپ_ARABIC) &&
              (direction == U_راست_مقصد_چپ || direction == U_راست_مقصد_چپ_ARABIC))
           ){
            *وضعیت = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            أرجع خطأ;
        }
    }
    أرجع s2.extract(dest, dest_ظرفیت, *وضعیت);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_جابجا(ثابت UDataSwapper *ds,
            ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
            UError_کد *p_Error_کد) {
    ثابت UDataInfo *pInfo;
    int32_t سرآیند_اندازه;

    ثابت uint8_t *درون_بایتها;
    uint8_t *بیرون_بایتها;

    ثابت int32_t *درون_Indexes;
    int32_t indexes[16];

    int32_t i, جابه_جایی, count, size;

    /* udata_swapDataHeader checks the arguments */
    سرآیند_اندازه=udata_جابجا_داده_سرآیند(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)درون_داده+4);
    إذا(!(
        pInfo->داده_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->داده_Format[1]==0x50 &&
        pInfo->داده_Format[2]==0x52 &&
        pInfo->داده_Format[3]==0x50 &&
        pInfo->format_نسخه[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->داده_Format[0], pInfo->داده_Format[1],
                         pInfo->داده_Format[2], pInfo->داده_Format[3],
                         pInfo->format_نسخه[0]);
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    درون_بایتها=(ثابت uint8_t *)درون_داده+سرآیند_اندازه;
    بیرون_بایتها= (بیرون_داده == اشاره_تهی ) ? اشاره_تهی : (uint8_t *)بیرون_داده+سرآیند_اندازه;

    درون_Indexes=(ثابت int32_t *)درون_بایتها;

    إذا(length>=0) {
        length-=سرآیند_اندازه;
        إذا(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    لكل(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, درون_Indexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[SPREP_شاخص_TRIE_اندازه]+
        indexes[SPREP_شاخص_MAPPING_داده_اندازه];

    إذا(length>=0) {
        إذا(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        /* copy the data for inaccessible bytes */
        إذا(درون_بایتها!=بیرون_بایتها) {
            uprv_memcpy(بیرون_بایتها, درون_بایتها, size);
        }

        جابه_جایی=0;

        /* swap the int32_t indexes[] */
        count=16*4;
        ds->جابجا_Array32(ds, درون_بایتها, count, بیرون_بایتها, p_Error_کد);
        جابه_جایی+=count;

        /* swap the UTrie */
        count=indexes[SPREP_شاخص_TRIE_اندازه];
        utrie_جابجا(ds, درون_بایتها+جابه_جایی, count, بیرون_بایتها+جابه_جایی, p_Error_کد);
        جابه_جایی+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[SPREP_شاخص_MAPPING_داده_اندازه];
        ds->جابجا_Array16(ds, درون_بایتها+جابه_جایی, count, بیرون_بایتها+جابه_جایی, p_Error_کد);
        //offset+=count;
    }

    أرجع سرآیند_اندازه+size;
}

#پایان_اگر /* #if !UCONFIG_NO_IDNA */
