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
ساكن UHashtable *SHARED_معلومات_HASHTABLE = تش_اشاره;
ساكن icu::UInitOnce g_Shared_معلومات_Init_Once {};

ساكن UMutex usprepMutex;
/* format version of spp file */
//static uint8_t formatVersion[4]={ 0, 0, 0, 0 };

/* the Unicode version of the sprep data */
ساكن UVersionInfo معلومات_نسخه={ 0, 0, 0, 0 };

/* Profile names must be aligned to UStringPrepProfileType */
ساكن ثابت حرف * ثابت PROFILE_نومونه[] = {
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
        pInfo->format_نسخه[0]==3 &&
        pInfo->format_نسخه[2]==UTRIE_ښويدنه &&
        pInfo->format_نسخه[3]==UTRIE_شاخص_ښويدنه
    ) {
        //uprv_memcpy(formatVersion, pInfo->formatVersion, 4);
        uprv_memcpy(معلومات_نسخه, pInfo->معلومات_نسخه, 4);
        أرجع ریښتیا;
    } وإلا {
        أرجع خطأ;
    }
}

ساكن int32_t U_CALLCONV
get_SPrep_Folding_دننه_واټن(uint32_t معلومات) {

    أرجع (int32_t)معلومات;

}

/* hashes an entry  */
ساكن int32_t U_CALLCONV 
hash_ثبت(ثابت UHashTok parm) {
    UString_Prep_کلی *b = (UString_Prep_کلی *)parm.اشاره_ګر;
    UHashTok namekey, pathkey;
    namekey.اشاره_ګر = b->name;
    pathkey.اشاره_ګر = b->لار;
    uint32_t unsignedHash = static_cast<uint32_t>(uhash_hashChars(namekey)) +
            37u * static_cast<uint32_t>(uhash_hashChars(pathkey));
    أرجع static_cast<int32_t>(unsignedHash);
}

/* compares two entries */
ساكن UBool U_CALLCONV 
compareEntries(ثابت UHashTok p1, ثابت UHashTok p2) {
    UString_Prep_کلی *b1 = (UString_Prep_کلی *)p1.اشاره_ګر;
    UString_Prep_کلی *b2 = (UString_Prep_کلی *)p2.اشاره_ګر;
    UHashTok name1, name2, path1, path2;
    name1.اشاره_ګر = b1->name;
    name2.اشاره_ګر = b2->name;
    path1.اشاره_ګر = b1->لار;
    path2.اشاره_ګر = b2->لار;
    أرجع uhash_compareChars(name1, name2) && uhash_compareChars(path1, path2);
}

ساكن فراغ 
usprep_unload(UStringPrepProfile* معلومات){
    udata_close(معلومات->sprep_معلومات);
}

ساكن int32_t 
usprep_دننی_flush_Cache(UBool no_Ref_شمېر){
    UStringPrepProfile *profile = تش_اشاره;
    UString_Prep_کلی  *کلی  = تش_اشاره;
    int32_t pos = UHASH_لومړی;
    int32_t deleted_عدد = 0;
    ثابت UHashElement *e;

    /*
     * if shared data hasn't even been lazy evaluated yet
     * return 0
     */
    umtx_قلف(&usprepMutex);
    إذا (SHARED_معلومات_HASHTABLE == تش_اشاره) {
        umtx_unlock(&usprepMutex);
        أرجع 0;
    }

    /*creates an enumeration to iterate through every element in the table */
    طالما ((e = uhash_بل_Element(SHARED_معلومات_HASHTABLE, &pos)) != تش_اشاره)
    {
        profile = (UStringPrepProfile *) e->value.اشاره_ګر;
        کلی  = (UString_Prep_کلی *) e->کلی.اشاره_ګر;

        إذا ((no_Ref_شمېر== خطأ && profile->ref_شمېر == 0) || 
             no_Ref_شمېر) {
            deleted_عدد++;
            uhash_removeElement(SHARED_معلومات_HASHTABLE, e);

            /* unload the data */
            usprep_unload(profile);

            إذا(کلی->name != تش_اشاره) {
                uprv_free(کلی->name);
                کلی->name=تش_اشاره;
            }
            إذا(کلی->لار != تش_اشاره) {
                uprv_free(کلی->لار);
                کلی->لار=تش_اشاره;
            }
            uprv_free(profile);
            uprv_free(کلی);
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
    إذا (SHARED_معلومات_HASHTABLE != تش_اشاره) {
        usprep_دننی_flush_Cache(ریښتیا);
        إذا (SHARED_معلومات_HASHTABLE != تش_اشاره && uhash_شمېر(SHARED_معلومات_HASHTABLE) == 0) {
            uhash_close(SHARED_معلومات_HASHTABLE);
            SHARED_معلومات_HASHTABLE = تش_اشاره;
        }
    }
    g_Shared_معلومات_Init_Once.reset();
    أرجع (SHARED_معلومات_HASHTABLE == تش_اشاره);
}
U_CDECL_پای_ټکی


/** Initializes the cache for resources */
ساكن فراغ U_CALLCONV
createCache(UError_کوډ &حالت) {
    SHARED_معلومات_HASHTABLE = uhash_open(hash_ثبت, compareEntries, تش_اشاره, &حالت);
    إذا (U_FAILURE(حالت)) {
        SHARED_معلومات_HASHTABLE = تش_اشاره;
    }
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_USPREP, usprep_cleanup);
}

ساكن فراغ 
initCache(UError_کوډ *حالت) {
    umtx_initOnce(g_Shared_معلومات_Init_Once, &createCache, *حالت);
}

ساكن UBool U_CALLCONV
بارول_معلومات(UStringPrepProfile* profile, 
         ثابت حرف* لار, 
         ثابت حرف* name, 
         ثابت حرف* type, 
         UError_کوډ* error_کوډ) {
    /* load Unicode SPREP data from file */    
    UTrie _sprepTrie = {تش_اشاره, تش_اشاره, تش_اشاره, 0, 0, 0, 0};
    UData_حافظه *معلومات_حافظه;
    ثابت int32_t *p=تش_اشاره;
    ثابت uint8_t *pb;
    UVersionInfo norm_Unicode_نسخه;
    int32_t normUniVer, sprepUniVer, normCorrVer;

    إذا(error_کوډ==تش_اشاره || U_FAILURE(*error_کوډ)) {
        أرجع 0;
    }

    /* open the data outside the mutex block */
    //TODO: change the path
    معلومات_حافظه=udata_openChoice(لار, type, name, isSPrepAcceptable, تش_اشاره, error_کوډ);
    إذا(U_FAILURE(*error_کوډ)) {
        أرجع خطأ;
    }

    p = static_cast<ثابت int32_t*>(udata_get_حافظه(معلومات_حافظه));
    pb = reinterpret_cast<ثابت uint8_t*>(p + SPREP_شاخص_سر);
    utrie_unserialize(&_sprepTrie, pb, p[SPREP_شاخص_TRIE_اندازه], error_کوډ);
    _sprepTrie.get_Folding_دننه_واټن=get_SPrep_Folding_دننه_واټن;


    إذا(U_FAILURE(*error_کوډ)) {
        udata_close(معلومات_حافظه);
        أرجع خطأ;
    }

    /* in the mutex block, set the data for this process */
    umtx_قلف(&usprepMutex);
    إذا(profile->sprep_معلومات==تش_اشاره) {
        profile->sprep_معلومات=معلومات_حافظه;
        معلومات_حافظه=تش_اشاره;
        uprv_memcpy(&profile->indexes, p, حجم(profile->indexes));
        uprv_memcpy(&profile->sprepTrie, &_sprepTrie, حجم(UTrie));
    } وإلا {
        p = static_cast<ثابت int32_t*>(udata_get_حافظه(profile->sprep_معلومات));
    }
    umtx_unlock(&usprepMutex);
    /* initialize some variables */
    profile->mapping_معلومات = reinterpret_cast<ثابت uint16_t*>(reinterpret_cast<ثابت uint8_t*>(p + SPREP_شاخص_سر) + profile->indexes[SPREP_شاخص_TRIE_اندازه]);
    
    u_get_Unicode_نسخه(norm_Unicode_نسخه);
    normUniVer = (norm_Unicode_نسخه[0] << 24) + (norm_Unicode_نسخه[1] << 16) + 
                 (norm_Unicode_نسخه[2] << 8 ) + (norm_Unicode_نسخه[3]);
    sprepUniVer = (معلومات_نسخه[0] << 24) + (معلومات_نسخه[1] << 16) + 
                  (معلومات_نسخه[2] << 8 ) + (معلومات_نسخه[3]);
    normCorrVer = profile->indexes[SPREP_NORM_CORRECTNS_وروستی_UNI_نسخه];
    
    إذا(U_FAILURE(*error_کوډ)){
        udata_close(معلومات_حافظه);
        أرجع خطأ;
    }
    إذا( normUniVer < sprepUniVer && /* the Unicode version of SPREP file must be less than the Unicode Version of the normalization data */
        normUniVer < normCorrVer && /* the Unicode version of the NormalizationCorrections.txt file should be less than the Unicode Version of the normalization data */
        ((profile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0) /* normalization turned on*/
      ){
        *error_کوډ = U_INVALID_FORMAT_ERROR;
        udata_close(معلومات_حافظه);
        أرجع خطأ;
    }
    profile->is_معلومات_Loaded = ریښتیا;

    /* if a different thread set it first, then close the extra data */
    إذا(معلومات_حافظه!=تش_اشاره) {
        udata_close(معلومات_حافظه); /* nullptr if it was set correctly */
    }


    أرجع profile->is_معلومات_Loaded;
}

ساكن UStringPrepProfile* 
usprep_getProfile(ثابت حرف* لار, 
                  ثابت حرف* name,
                  UError_کوډ *حالت){

    UStringPrepProfile* profile = تش_اشاره;

    initCache(حالت);

    إذا(U_FAILURE(*حالت)){
        أرجع تش_اشاره;
    }

    UString_Prep_کلی ډېری_کلی;
    /* 
     * const is cast way to save malloc, strcpy and free calls 
     * we use the passed in pointers for fetching the data from the 
     * hash table which is safe
     */
    ډېری_کلی.name = const_cast<حرف*>(name);
    ډېری_کلی.لار = const_cast<حرف*>(لار);

    /* fetch the data from the cache */
    umtx_قلف(&usprepMutex);
    profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_معلومات_HASHTABLE, &ډېری_کلی));
    إذا(profile != تش_اشاره) {
        profile->ref_شمېر++;
    }
    umtx_unlock(&usprepMutex);
    
    إذا(profile == تش_اشاره) {
        /* else load the data and put the data in the cache */
        Local_حافظه<UStringPrepProfile> newProfile;
        إذا(newProfile.allocateInsteadAndReset() == تش_اشاره) {
            *حالت = U_حافظه_ویش_ERROR;
            أرجع تش_اشاره;
        }

        /* load the data */
        إذا(!بارول_معلومات(newProfile.getAlias(), لار, name, SPREP_معلومات_TYPE, حالت) || U_FAILURE(*حالت) ){
            أرجع تش_اشاره;
        }

        /* get the options */
        newProfile->doNFKC = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_NORMALIZATION_ON) > 0);
        newProfile->checkBiDi = static_cast<UBool>((newProfile->indexes[_SPREP_OPTIONS] & _SPREP_CHECK_BIDI_ON) > 0);

        Local_حافظه<UString_Prep_کلی> کلی;
        Local_حافظه<حرف> کلی_Name;
        Local_حافظه<حرف> کلی_لار;
        إذا( کلی.allocateInsteadAndReset() == تش_اشاره ||
            کلی_Name.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(name)+1)) == تش_اشاره ||
            (لار != تش_اشاره &&
             کلی_لار.allocateInsteadAndCopy(static_cast<int32_t>(uprv_strlen(لار)+1)) == تش_اشاره)
         ) {
            *حالت = U_حافظه_ویش_ERROR;
            usprep_unload(newProfile.getAlias());
            أرجع تش_اشاره;
        }

        umtx_قلف(&usprepMutex);
        // If another thread already inserted the same key/value, refcount and cleanup our thread data
        profile = static_cast<UStringPrepProfile*>(uhash_get(SHARED_معلومات_HASHTABLE, &ډېری_کلی));
        إذا(profile != تش_اشاره) {
            profile->ref_شمېر++;
            usprep_unload(newProfile.getAlias());
        }
        وإلا {
            /* initialize the key members */
            کلی->name = کلی_Name.orphan();
            uprv_strcpy(کلی->name, name);
            إذا(لار != تش_اشاره){
                کلی->لار = کلی_لار.orphan();
                uprv_strcpy(کلی->لار, لار);
            }        
            profile = newProfile.orphan();
    
            /* add the data object to the cache */
            profile->ref_شمېر = 1;
            uhash_put(SHARED_معلومات_HASHTABLE, کلی.orphan(), profile, حالت);
        }
        umtx_unlock(&usprepMutex);
    }

    أرجع profile;
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_open(ثابت حرف* لار, 
            ثابت حرف* name,
            UError_کوډ* حالت){

    إذا(حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع تش_اشاره;
    }
       
    /* initialize the profile struct members */
    أرجع usprep_getProfile(لار,name,حالت);
}

U_CAPI UStringPrepProfile* U_EXPORT2
usprep_openByType(UStringPrepProfileType type,
				  UError_کوډ* حالت) {
    إذا(حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع تش_اشاره;
    }
    int32_t شاخص = (int32_t)type;
    إذا (شاخص < 0 || شاخص >= UPRV_LENGTHOF(PROFILE_نومونه)) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    أرجع usprep_open(تش_اشاره, PROFILE_نومونه[شاخص], حالت);
}

U_CAPI فراغ U_EXPORT2
usprep_close(UStringPrepProfile* profile){
    إذا(profile==تش_اشاره){
        أرجع;
    }

    umtx_قلف(&usprepMutex);
    /* decrement the ref count*/
    إذا(profile->ref_شمېر > 0){
        profile->ref_شمېر--;
    }
    umtx_unlock(&usprepMutex);
    
}

U_CFUNC فراغ 
uprv_syntaxError(ثابت char16_t* قواعد,
                 int32_t pos,
                 int32_t قواعد_Len,
                 UParseError* parseError){
    إذا(parseError == تش_اشاره){
        أرجع;
    }
    parseError->دننه_واټن = pos;
    parseError->کرښه = 0 ; // we are not using line numbers 
    
    // for pre-context
    int32_t پیل = (pos < U_PARSE_CONTEXT_LEN)? 0 : (pos - (U_PARSE_CONTEXT_LEN-1));
    int32_t حد = pos;
    
    u_memcpy(parseError->preContext,قواعد+پیل,حد-پیل);
    //null terminate the buffer
    parseError->preContext[حد-پیل] = 0;
    
    // for post-context; include error rules[pos]
    پیل = pos;
    حد = پیل + (U_PARSE_CONTEXT_LEN-1);
    إذا (حد > قواعد_Len) {
        حد = قواعد_Len;
    }
    إذا (پیل < قواعد_Len) {
        u_memcpy(parseError->postContext,قواعد+پیل,حد-پیل);
    }
    //null terminate the buffer
    parseError->postContext[حد-پیل]= 0;
}


ساكن مضمن UStringPrepType
get_ارزښتونه(uint16_t trie_کلمه, int16_t& value, UBool& is_شاخص){

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
    }وإلا إذا(trie_کلمه >= SPREP_TYPE_پوله){
        type = static_cast<UStringPrepType>(trie_کلمه - SPREP_TYPE_پوله);
        is_شاخص =خطأ;
        value = 0;
    }وإلا{
        /* get the type */
        type = USPREP_MAP;
        /* ascertain if the value is index or delta */
        إذا(trie_کلمه & 0x02){
            is_شاخص = ریښتیا;
            value = trie_کلمه  >> 2; //mask off the lower 2 bits and shift
        }وإلا{
            is_شاخص = خطأ;
            value = static_cast<int16_t>(trie_کلمه);
            value =  (value >> 2);
        }
 
        إذا((trie_کلمه>>2) == SPREP_MAX_شاخص_ارزښت){
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
             ثابت char16_t* src, int32_t src_اوږدتیا,
             char16_t* dest, int32_t dest_ظرفیت,
             int32_t options,
             UParseError* parseError,
             UError_کوډ* حالت ){
    
    uint16_t پایله;
    int32_t dest_شاخص=0;
    int32_t src_شاخص;
    UBool allowUnassigned = static_cast<UBool>((options & USPREP_ALLOW_UNASSIGNED) > 0);
    UStringPrepType type;
    int16_t value;
    UBool is_شاخص;
    ثابت int32_t* indexes = profile->indexes;

    // no error checking the caller check for error and arguments
    // no string length check the caller finds out the string length

    لكل(src_شاخص=0;src_شاخص<src_اوږدتیا;){
        UChar32 ch;

        U16_بل(src,src_شاخص,src_اوږدتیا,ch);
        
        پایله=0;

        UTRIE_GET16(&profile->sprepTrie,ch,پایله);
        
        type = get_ارزښتونه(پایله, value, is_شاخص);

        // check if the source codepoint is unassigned
        إذا(type == USPREP_UNASSIGNED && allowUnassigned == خطأ){

            uprv_syntaxError(src,src_شاخص-U16_اوږدتیا(ch), src_اوږدتیا,parseError);
            *حالت = U_STRINGPREP_UNASSIGNED_ERROR;
            أرجع 0;
            
        }وإلا إذا(type == USPREP_MAP){
            
            int32_t شاخص, length;

            إذا(is_شاخص){
                شاخص = value;
                إذا(شاخص >= indexes[SPREP_ONE_UCHAR_MAPPING_شاخص_پیل] &&
                         شاخص < indexes[SPREP_TWO_UCHARS_MAPPING_شاخص_پیل]){
                    length = 1;
                }وإلا إذا(شاخص >= indexes[SPREP_TWO_UCHARS_MAPPING_شاخص_پیل] &&
                         شاخص < indexes[SPREP_THREE_UCHARS_MAPPING_شاخص_پیل]){
                    length = 2;
                }وإلا إذا(شاخص >= indexes[SPREP_THREE_UCHARS_MAPPING_شاخص_پیل] &&
                         شاخص < indexes[SPREP_FOUR_UCHARS_MAPPING_شاخص_پیل]){
                    length = 3;
                }وإلا{
                    length = profile->mapping_معلومات[شاخص++];
         
                }

                /* copy mapping to destination */
                لكل(int32_t i=0; i< length; i++){
                    إذا(dest_شاخص < dest_ظرفیت  ){
                        dest[dest_شاخص] = profile->mapping_معلومات[شاخص+i];
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
        
    أرجع u_terminateUChars(dest, dest_ظرفیت, dest_شاخص, حالت);
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
                  ثابت char16_t* src, int32_t src_اوږدتیا,
                  char16_t* dest, int32_t dest_ظرفیت,
                  int32_t options,
                  UParseError* parseError,
                  UError_کوډ* حالت ){

    // check error status
    إذا(U_FAILURE(*حالت)){
        أرجع 0;
    }

    //check arguments
    إذا(profile==تش_اشاره ||
            (src==تش_اشاره ? src_اوږدتیا!=0 : src_اوږدتیا<-1) ||
            (dest==تش_اشاره ? dest_ظرفیت!=0 : dest_ظرفیت<0)) {
        *حالت=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    //get the string length
    إذا(src_اوږدتیا < 0){
        src_اوږدتیا = u_strlen(src);
    }
    // map
    UnicodeString s1;
    char16_t *b1 = s1.get_بفر(src_اوږدتیا);
    إذا(b1==تش_اشاره){
        *حالت = U_حافظه_ویش_ERROR;
        أرجع 0;
    }
    UError_کوډ بفر_حالت = U_ZERO_ERROR;
    int32_t b1Len = usprep_map(profile, src, src_اوږدتیا,
                               b1, s1.get_ظرفیت(), options, parseError, &بفر_حالت);
    s1.release_بفر(U_SUCCESS(بفر_حالت) ? b1Len : 0);

    إذا(بفر_حالت == U_بفر_ډکېدنه_ERROR){
        // redo processing of string
        /* we do not have enough room so grow the buffer*/
        b1 = s1.get_بفر(b1Len);
        إذا(b1==تش_اشاره){
            *حالت = U_حافظه_ویش_ERROR;
            أرجع 0;
        }

        بفر_حالت = U_ZERO_ERROR; // reset error
        b1Len = usprep_map(profile, src, src_اوږدتیا,
                           b1, s1.get_ظرفیت(), options, parseError, &بفر_حالت);
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
    UCharDirection direction=U_CHAR_DIRECTION_شمېر, لومړی_Char_Dir=U_CHAR_DIRECTION_شمېر;
    UBool کیڼ_منزل_ښی=خطأ, ښی_منزل_کیڼ=خطأ;
    int32_t rtlPos =-1, ltrPos =-1;

    لكل(int32_t b2_شاخص=0; b2_شاخص<b2Len;){
        UChar32 ch = 0;
        U16_بل(b2, b2_شاخص, b2Len, ch);

        uint16_t پایله;
        UTRIE_GET16(&profile->sprepTrie,ch,پایله);

        int16_t value;
        UBool is_شاخص;
        UStringPrepType type = get_ارزښتونه(پایله, value, is_شاخص);

        إذا( type == USPREP_PROHIBITED || 
            ((پایله < SPREP_TYPE_پوله) && (پایله & 0x01) /* first bit says it the code point is prohibited*/)
           ){
            *حالت = U_STRINGPREP_PROHIBITED_ERROR;
            uprv_syntaxError(b2, b2_شاخص-U16_اوږدتیا(ch), b2Len, parseError);
            أرجع 0;
        }

        إذا(profile->checkBiDi) {
            direction = ubidi_getClass(ch);
            إذا(لومړی_Char_Dir == U_CHAR_DIRECTION_شمېر){
                لومړی_Char_Dir = direction;
            }
            إذا(direction == U_کیڼ_منزل_ښی){
                کیڼ_منزل_ښی = ریښتیا;
                ltrPos = b2_شاخص-1;
            }
            إذا(direction == U_ښی_منزل_کیڼ || direction == U_ښی_منزل_کیڼ_ARABIC){
                ښی_منزل_کیڼ = ریښتیا;
                rtlPos = b2_شاخص-1;
            }
        }
    }
    إذا(profile->checkBiDi){
        // satisfy 2
        إذا( کیڼ_منزل_ښی && ښی_منزل_کیڼ){
            *حالت = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2,(rtlPos>ltrPos) ? rtlPos : ltrPos, b2Len, parseError);
            أرجع 0;
        }

        //satisfy 3
        إذا( ښی_منزل_کیڼ && 
            !((لومړی_Char_Dir == U_ښی_منزل_کیڼ || لومړی_Char_Dir == U_ښی_منزل_کیڼ_ARABIC) &&
              (direction == U_ښی_منزل_کیڼ || direction == U_ښی_منزل_کیڼ_ARABIC))
           ){
            *حالت = U_STRINGPREP_CHECK_BIDI_ERROR;
            uprv_syntaxError(b2, rtlPos, b2Len, parseError);
            أرجع خطأ;
        }
    }
    أرجع s2.extract(dest, dest_ظرفیت, *حالت);
}


/* data swapping ------------------------------------------------------------ */

U_CAPI int32_t U_EXPORT2
usprep_بدلون(ثابت UDataSwapper *ds,
            ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
            UError_کوډ *p_Error_کوډ) {
    ثابت UDataInfo *pInfo;
    int32_t سرلیک_اندازه;

    ثابت uint8_t *دننه_بایټونه;
    uint8_t *بهر_بایټونه;

    ثابت int32_t *دننه_Indexes;
    int32_t indexes[16];

    int32_t i, دننه_واټن, count, size;

    /* udata_swapDataHeader checks the arguments */
    سرلیک_اندازه=udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)دننه_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x53 &&   /* dataFormat="SPRP" */
        pInfo->معلومات_Format[1]==0x50 &&
        pInfo->معلومات_Format[2]==0x52 &&
        pInfo->معلومات_Format[3]==0x50 &&
        pInfo->format_نسخه[0]==3
    )) {
        udata_printError(ds, "usprep_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as StringPrep .spp data\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخه[0]);
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    دننه_بایټونه=(ثابت uint8_t *)دننه_معلومات+سرلیک_اندازه;
    بهر_بایټونه= (بهر_معلومات == تش_اشاره ) ? تش_اشاره : (uint8_t *)بهر_معلومات+سرلیک_اندازه;

    دننه_Indexes=(ثابت int32_t *)دننه_بایټونه;

    إذا(length>=0) {
        length-=سرلیک_اندازه;
        إذا(length<16*4) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for StringPrep .spp data\n",
                             length);
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    /* read the first 16 indexes (ICU 2.8/format version 3: _SPREP_INDEX_TOP==16, might grow) */
    لكل(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, دننه_Indexes[i]);
    }

    /* calculate the total length of the data */
    size=
        16*4+ /* size of indexes[] */
        indexes[SPREP_شاخص_TRIE_اندازه]+
        indexes[SPREP_شاخص_MAPPING_معلومات_اندازه];

    إذا(length>=0) {
        إذا(length<size) {
            udata_printError(ds, "usprep_swap(): too few bytes (%d after header) for all of StringPrep .spp data\n",
                             length);
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        /* copy the data for inaccessible bytes */
        إذا(دننه_بایټونه!=بهر_بایټونه) {
            uprv_memcpy(بهر_بایټونه, دننه_بایټونه, size);
        }

        دننه_واټن=0;

        /* swap the int32_t indexes[] */
        count=16*4;
        ds->بدلون_Array32(ds, دننه_بایټونه, count, بهر_بایټونه, p_Error_کوډ);
        دننه_واټن+=count;

        /* swap the UTrie */
        count=indexes[SPREP_شاخص_TRIE_اندازه];
        utrie_بدلون(ds, دننه_بایټونه+دننه_واټن, count, بهر_بایټونه+دننه_واټن, p_Error_کوډ);
        دننه_واټن+=count;

        /* swap the uint16_t mappingTable[] */
        count=indexes[SPREP_شاخص_MAPPING_معلومات_اندازه];
        ds->بدلون_Array16(ds, دننه_بایټونه+دننه_واټن, count, بهر_بایټونه+دننه_واټن, p_Error_کوډ);
        //offset+=count;
    }

    أرجع سرلیک_اندازه+size;
}

#د_که_پای /* #if !UCONFIG_NO_IDNA */
