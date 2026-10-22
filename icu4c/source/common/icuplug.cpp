// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2009-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*  FILE NAME : icuplug.c
*
*   Date         Name        Description
*   10/29/2009   sl          New.
******************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/icuplug.h"


#ਜੇ UCONFIG_ENABLE_PLUGINS


#ਅਨ੍ਤਰ੍ਭਾਵ "icuplugimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MVS__  /* defined by z/OS compiler */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _POSIX_SOURCE
#ਅਨ੍ਤਰ੍ਭਾਵ <cics.h> /* 12 Nov 2011 JAM iscics() function */
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ icu;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UPLUG_TRACE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPLUG_TRACE 0
#ਜੇ_ਅੰਤ

#ਜੇ UPLUG_TRACE
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#ਜੇ_ਅੰਤ

/**
 * Internal structure of an ICU plugin. 
 */

ਬਣਤਰ UPlug_ਅੰਕੜੇ {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t struct_ਮਾਪ;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  ਖਾਲੀ *lib;              /**< plugin library, or nullptr */
  ਅੱਖਰ libName[UPLUG_NAME_MAX];   /**< library name */
  ਅੱਖਰ sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  ਅੱਖਰ config[UPLUG_NAME_MAX];     /**< configuration data */
  ਖਾਲੀ *context;          /**< user context data */
  ਅੱਖਰ name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaiting_ਲੱਦੋ; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UError_ਕੋਡ plugin_ਸਥਿਤੀ; /**< status code of plugin */
};



#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPLUG_LIBRARY_INITIAL_ਗਿਣਤੀ 8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UPLUG_PLUGIN_INITIAL_ਗਿਣਤੀ 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
ਸਥਿਰ int32_t uplug_remove_ਇੰਦਰਾਜ_At(ਖਾਲੀ *list, int32_t list_ਮਾਪ, int32_t member_ਮਾਪ, int32_t ਵਸਤੂ_ਮੰਜ਼ਿਲ_Remove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  ਜੇ(list_ਮਾਪ<1) {
    ਵਾਪਸ list_ਮਾਪ;
  }
    
  /* is there anything to move? */
  ਜੇ(list_ਮਾਪ > ਵਸਤੂ_ਮੰਜ਼ਿਲ_Remove+1) {
    memmove(bytePtr+(ਵਸਤੂ_ਮੰਜ਼ਿਲ_Remove*member_ਮਾਪ), bytePtr+((ਵਸਤੂ_ਮੰਜ਼ਿਲ_Remove+1)*member_ਮਾਪ), member_ਮਾਪ);
  }
    
  ਵਾਪਸ list_ਮਾਪ-1;
}




#ਜੇ U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
ਬਣਤਰ UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UPlugLibrary {
  ਖਾਲੀ *lib;                           /**< library ptr */
  ਅੱਖਰ name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

ਸਥਿਰ UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_ਗਿਣਤੀ];
ਸਥਿਰ UPlugLibrary * libraryList = staticLibraryList;
ਸਥਿਰ int32_t library_ਗਿਣਤੀ = 0;
ਸਥਿਰ int32_t libraryMax = UPLUG_LIBRARY_INITIAL_ਗਿਣਤੀ;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ਸਥਿਰ int32_t ਖੋਜ_For_Library_Name(ਅਚਲ ਅੱਖਰ *libName) {
  int32_t i;
    
  ਲਈ(i=0;i<library_ਗਿਣਤੀ;i++) {
    ਜੇ(!uprv_strcmp(libName, libraryList[i].name)) {
      ਵਾਪਸ i;
    }
  }
  ਵਾਪਸ -1;
}

ਸਥਿਰ int32_t ਖੋਜ_For_Library(ਖਾਲੀ *lib) {
  int32_t i;
    
  ਲਈ(i=0;i<library_ਗਿਣਤੀ;i++) {
    ਜੇ(lib==libraryList[i].lib) {
      ਵਾਪਸ i;
    }
  }
  ਵਾਪਸ -1;
}

U_CAPI ਅੱਖਰ * U_EXPORT2
uplug_ਲੱਭੋ_Library(ਖਾਲੀ *lib, UError_ਕੋਡ *ਸਥਿਤੀ) {
  int32_t libEnt;
  ਅੱਖਰ *ret = ਖਾਲੀ_ਸੂਚਕ;
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  libEnt = ਖੋਜ_For_Library(lib);
  ਜੇ(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } ਨਹੀਂਤਾਂ {
    *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
  }
  ਵਾਪਸ ret;
}

U_CAPI ਖਾਲੀ * U_EXPORT2
uplug_openLibrary(ਅਚਲ ਅੱਖਰ *libName, UError_ਕੋਡ *ਸਥਿਤੀ) {
  int32_t lib_ਇੰਦਰਾਜ = -1;
  ਖਾਲੀ *lib = ਖਾਲੀ_ਸੂਚਕ;
    
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

  lib_ਇੰਦਰਾਜ = ਖੋਜ_For_Library_Name(libName);
  ਜੇ(lib_ਇੰਦਰਾਜ == -1) {
    lib_ਇੰਦਰਾਜ = library_ਗਿਣਤੀ++;
    ਜੇ(library_ਗਿਣਤੀ >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
#ਜੇ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#ਜੇ_ਅੰਤ
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[lib_ਇੰਦਰਾਜ].lib = uprv_dl_open(libName, ਸਥਿਤੀ);
#ਜੇ UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*ਸਥਿਤੀ), lib_ਇੰਦਰਾਜ, lib));
#ਜੇ_ਅੰਤ
        
    ਜੇ(libraryList[lib_ਇੰਦਰਾਜ].lib == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
      /* cleanup. */
      libraryList[lib_ਇੰਦਰਾਜ].lib = ਖਾਲੀ_ਸੂਚਕ; /* failure with open */
      libraryList[lib_ਇੰਦਰਾਜ].name[0] = 0;
#ਜੇ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*ਸਥਿਤੀ), lib_ਇੰਦਰਾਜ, lib));
#ਜੇ_ਅੰਤ
      /* no need to free - just won't increase the count. */
      library_ਗਿਣਤੀ--;
    } ਨਹੀਂਤਾਂ { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[lib_ਇੰਦਰਾਜ].name,libName,UPLUG_NAME_MAX);
      libraryList[lib_ਇੰਦਰਾਜ].ref=1;
      lib = libraryList[lib_ਇੰਦਰਾਜ].lib;
    }

  } ਨਹੀਂਤਾਂ {
    lib = libraryList[lib_ਇੰਦਰਾਜ].lib;
    libraryList[lib_ਇੰਦਰਾਜ].ref++;
  }
  ਵਾਪਸ lib;
}

U_CAPI ਖਾਲੀ U_EXPORT2
uplug_closeLibrary(ਖਾਲੀ *lib, UError_ਕੋਡ *ਸਥਿਤੀ) {
  int32_t i;
    
#ਜੇ UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*ਸਥਿਤੀ), (ਖਾਲੀ*)libraryList));
#ਜੇ_ਅੰਤ
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ;
    
  ਲਈ(i=0;i<library_ਗਿਣਤੀ;i++) {
    ਜੇ(lib==libraryList[i].lib) {
      ਜੇ(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, ਸਥਿਤੀ);
        library_ਗਿਣਤੀ = uplug_remove_ਇੰਦਰਾਜ_At(libraryList, library_ਗਿਣਤੀ, ਆਕਾਰ(*libraryList), i);
      }
      ਵਾਪਸ;
    }
  }
  *ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR; /* could not find the entry! */
}

#ਜੇ_ਅੰਤ

ਸਥਿਰ UPlug_ਅੰਕੜੇ pluginList[UPLUG_PLUGIN_INITIAL_ਗਿਣਤੀ];
ਸਥਿਰ int32_t plugin_ਗਿਣਤੀ = 0;



  
ਸਥਿਰ int32_t uplug_plugin_ਸੰਖਿਆ(UPlug_ਅੰਕੜੇ* d) {
  UPlug_ਅੰਕੜੇ *pastPlug = &pluginList[plugin_ਗਿਣਤੀ];
  ਜੇ(d<=pluginList) {
    ਵਾਪਸ 0;
  } ਨਹੀਂਤਾਂ ਜੇ(d>=pastPlug) {
    ਵਾਪਸ plugin_ਗਿਣਤੀ;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ (d-pluginList)/ਆਕਾਰ(pluginList[0]);
  }
}


U_CAPI UPlug_ਅੰਕੜੇ * U_EXPORT2
uplug_ਅਗਲਾ_Plug(UPlug_ਅੰਕੜੇ *ਪੂਰਵ) {
  ਜੇ(ਪੂਰਵ==ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ pluginList;
  } ਨਹੀਂਤਾਂ {
    UPlug_ਅੰਕੜੇ *ਅਗਲਾ_Plug = &ਪੂਰਵ[1];
    UPlug_ਅੰਕੜੇ *pastPlug = &pluginList[plugin_ਗਿਣਤੀ];
    
    ਜੇ(ਅਗਲਾ_Plug>=pastPlug) {
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
      ਵਾਪਸ ਅਗਲਾ_Plug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ਸਥਿਰ ਖਾਲੀ uplug_callPlug(UPlug_ਅੰਕੜੇ *plug, UPlugReason reason, UError_ਕੋਡ *ਸਥਿਤੀ) {
  UPlugTokenReturn token;
  ਜੇ(plug==ਖਾਲੀ_ਸੂਚਕ||U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ;
  }
  token = (*(plug->entrypoint))(plug, reason, ਸਥਿਤੀ);
  ਜੇ(token!=UPLUG_TOKEN) {
    *ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
  }
}


ਸਥਿਰ ਖਾਲੀ uplug_unloadPlug(UPlug_ਅੰਕੜੇ *plug, UError_ਕੋਡ *ਸਥਿਤੀ) {
  ਜੇ(plug->awaiting_ਲੱਦੋ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
    ਵਾਪਸ; 
  }
  ਜੇ(U_SUCCESS(plug->plugin_ਸਥਿਤੀ)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, ਸਥਿਤੀ);
  }
}

ਸਥਿਰ ਖਾਲੀ uplug_queryPlug(UPlug_ਅੰਕੜੇ *plug, UError_ਕੋਡ *ਸਥਿਤੀ) {
  ਜੇ(!plug->awaiting_ਲੱਦੋ || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
    ਵਾਪਸ; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, ਸਥਿਤੀ);
  ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) { 
    ਜੇ(plug->level == UPLUG_LEVEL_INVALID) {
      plug->plugin_ਸਥਿਤੀ = U_PLUGIN_DIDNT_ਸੈੱਟ_LEVEL;
      plug->awaiting_ਲੱਦੋ = ਝੂਠ;
    }
  } ਨਹੀਂਤਾਂ {
    plug->plugin_ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
    plug->awaiting_ਲੱਦੋ = ਝੂਠ;
  }
}


ਸਥਿਰ ਖਾਲੀ uplug_ਲੱਦੋ_Plug(UPlug_ਅੰਕੜੇ *plug, UError_ਕੋਡ *ਸਥਿਤੀ) {
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ;
  }
  ਜੇ(!plug->awaiting_ਲੱਦੋ || (plug->level < UPLUG_LEVEL_ਨੀਵਾਂ) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
    ਵਾਪਸ;
  }
  uplug_callPlug(plug, UPLUG_REASON_ਲੱਦੋ, ਸਥਿਤੀ);
  plug->awaiting_ਲੱਦੋ = ਝੂਠ;
  ਜੇ(!U_SUCCESS(*ਸਥਿਤੀ)) {
    plug->plugin_ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
  }
}

ਸਥਿਰ UPlug_ਅੰਕੜੇ *uplug_allocate_ਖਾਲੀ_ਥਾਵਾਂ_Plug(UError_ਕੋਡ *ਸਥਿਤੀ)
{
  UPlug_ਅੰਕੜੇ *plug = ਖਾਲੀ_ਸੂਚਕ;

  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  ਜੇ(plugin_ਗਿਣਤੀ == UPLUG_PLUGIN_INITIAL_ਗਿਣਤੀ) {
    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  plug = &pluginList[plugin_ਗਿਣਤੀ++];

  plug->token = UPLUG_TOKEN;
  plug->struct_ਮਾਪ = ਆਕਾਰ(UPlug_ਅੰਕੜੇ);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaiting_ਲੱਦੋ = ਸੱਚ;
  plug->dontUnload = ਝੂਠ;
  plug->plugin_ਸਥਿਤੀ = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=ਖਾਲੀ_ਸੂਚਕ;
  plug->entrypoint=ਖਾਲੀ_ਸੂਚਕ;


  ਵਾਪਸ plug;
}

ਸਥਿਰ UPlug_ਅੰਕੜੇ *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ਅਚਲ ਅੱਖਰ *config, ਖਾਲੀ *lib, ਅਚਲ ਅੱਖਰ *symName,
                                     UError_ਕੋਡ *ਸਥਿਤੀ) {
  UPlug_ਅੰਕੜੇ *plug = uplug_allocate_ਖਾਲੀ_ਥਾਵਾਂ_Plug(ਸਥਿਤੀ);
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }

  ਜੇ(config!=ਖਾਲੀ_ਸੂਚਕ) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } ਨਹੀਂਤਾਂ {
    plug->config[0] = 0;
  }
    
  ਜੇ(symName!=ਖਾਲੀ_ਸੂਚਕ) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } ਨਹੀਂਤਾਂ {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, ਸਥਿਤੀ);
    
  ਵਾਪਸ plug;
}

ਸਥਿਰ ਖਾਲੀ uplug_deallocatePlug(UPlug_ਅੰਕੜੇ *plug, UError_ਕੋਡ *ਸਥਿਤੀ) {
  UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR;
  ਜੇ(!plug->dontUnload) {
#ਜੇ U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &sub_ਸਥਿਤੀ);
#ਜੇ_ਅੰਤ
  }
  plug->lib = ਖਾਲੀ_ਸੂਚਕ;
  ਜੇ(U_SUCCESS(*ਸਥਿਤੀ) && U_FAILURE(sub_ਸਥਿਤੀ)) {
    *ਸਥਿਤੀ = sub_ਸਥਿਤੀ;
  }
  /* shift plugins up and decrement count. */
  ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
    /* all ok- remove. */
    plugin_ਗਿਣਤੀ = uplug_remove_ਇੰਦਰਾਜ_At(pluginList, plugin_ਗਿਣਤੀ, ਆਕਾਰ(plug[0]), uplug_plugin_ਸੰਖਿਆ(plug));
  } ਨਹੀਂਤਾਂ {
    /* not ok- leave as a message. */
    plug->awaiting_ਲੱਦੋ=ਝੂਠ;
    plug->entrypoint=0;
    plug->dontUnload=ਸੱਚ;
  }
}

ਸਥਿਰ ਖਾਲੀ uplug_doUnloadPlug(UPlug_ਅੰਕੜੇ *plug_ਮੰਜ਼ਿਲ_Remove, UError_ਕੋਡ *ਸਥਿਤੀ) {
  ਜੇ(plug_ਮੰਜ਼ਿਲ_Remove != ਖਾਲੀ_ਸੂਚਕ) {
    uplug_unloadPlug(plug_ਮੰਜ਼ਿਲ_Remove, ਸਥਿਤੀ);
    uplug_deallocatePlug(plug_ਮੰਜ਼ਿਲ_Remove, ਸਥਿਤੀ);
  }
}

U_CAPI ਖਾਲੀ U_EXPORT2
uplug_removePlug(UPlug_ਅੰਕੜੇ *plug, UError_ਕੋਡ *ਸਥਿਤੀ)  {
  UPlug_ਅੰਕੜੇ *cursor = ਖਾਲੀ_ਸੂਚਕ;
  UPlug_ਅੰਕੜੇ *plug_ਮੰਜ਼ਿਲ_Remove = ਖਾਲੀ_ਸੂਚਕ;
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ;
    
  ਲਈ(cursor=pluginList;cursor!=ਖਾਲੀ_ਸੂਚਕ;) {
    ਜੇ(cursor==plug) {
      plug_ਮੰਜ਼ਿਲ_Remove = plug;
      cursor=ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
      cursor = uplug_ਅਗਲਾ_Plug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plug_ਮੰਜ਼ਿਲ_Remove, ਸਥਿਤੀ);
}




U_CAPI ਖਾਲੀ U_EXPORT2 
uplug_ਸੈੱਟ_Plug_No_Unload(UPlug_ਅੰਕੜੇ *ਅੰਕੜੇ, UBool dontUnload)
{
  ਅੰਕੜੇ->dontUnload = dontUnload;
}


U_CAPI ਖਾਲੀ U_EXPORT2
uplug_ਸੈੱਟ_Plug_Level(UPlug_ਅੰਕੜੇ *ਅੰਕੜੇ, UPlugLevel level) {
  ਅੰਕੜੇ->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlug_ਅੰਕੜੇ *ਅੰਕੜੇ) {
  ਵਾਪਸ ਅੰਕੜੇ->level;
}


U_CAPI ਖਾਲੀ U_EXPORT2
uplug_ਸੈੱਟ_Plug_Name(UPlug_ਅੰਕੜੇ *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *name) {
  uprv_strncpy(ਅੰਕੜੇ->name, name, UPLUG_NAME_MAX);
}


U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uplug_getPlugName(UPlug_ਅੰਕੜੇ* ਅੰਕੜੇ U_LIFETIME_BOUND) {
  ਵਾਪਸ ਅੰਕੜੇ->name;
}


U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uplug_get_ਚਿੰਨ੍ਹ_Name(UPlug_ਅੰਕੜੇ* ਅੰਕੜੇ U_LIFETIME_BOUND) {
  ਵਾਪਸ ਅੰਕੜੇ->sym;
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uplug_getLibraryName(UPlug_ਅੰਕੜੇ* ਅੰਕੜੇ U_LIFETIME_BOUND, UError_ਕੋਡ* ਸਥਿਤੀ) {
  ਜੇ(ਅੰਕੜੇ->libName[0]) {
    ਵਾਪਸ ਅੰਕੜੇ->libName;
  } ਨਹੀਂਤਾਂ {
#ਜੇ U_ENABLE_DYLOAD
    ਵਾਪਸ uplug_ਲੱਭੋ_Library(ਅੰਕੜੇ->lib, ਸਥਿਤੀ);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
#ਜੇ_ਅੰਤ
  }
}

U_CAPI ਖਾਲੀ * U_EXPORT2
uplug_getLibrary(UPlug_ਅੰਕੜੇ *ਅੰਕੜੇ) {
  ਵਾਪਸ ਅੰਕੜੇ->lib;
}

U_CAPI ਖਾਲੀ * U_EXPORT2
uplug_getContext(UPlug_ਅੰਕੜੇ *ਅੰਕੜੇ) {
  ਵਾਪਸ ਅੰਕੜੇ->context;
}


U_CAPI ਖਾਲੀ U_EXPORT2
uplug_ਸੈੱਟ_Context(UPlug_ਅੰਕੜੇ *ਅੰਕੜੇ, ਖਾਲੀ *context) {
  ਅੰਕੜੇ->context = context;
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uplug_getConfiguration(UPlug_ਅੰਕੜੇ* ਅੰਕੜੇ U_LIFETIME_BOUND) {
  ਵਾਪਸ ਅੰਕੜੇ->config;
}

U_CAPI UPlug_ਅੰਕੜੇ* U_EXPORT2
uplug_get_Plug_ਅੰਦਰੂਨੀ(int32_t n) { 
  ਜੇ(n <0 || n >= plugin_ਗਿਣਤੀ) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  } ਨਹੀਂਤਾਂ { 
    ਵਾਪਸ &(pluginList[n]);
  }
}


U_CAPI UError_ਕੋਡ U_EXPORT2
uplug_get_Plug_ਲੱਦੋ_ਸਥਿਤੀ(UPlug_ਅੰਕੜੇ *plug) {
  ਵਾਪਸ plug->plugin_ਸਥਿਤੀ;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ਸਥਿਰ UPlug_ਅੰਕੜੇ* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ਅਚਲ ਅੱਖਰ *config, ਖਾਲੀ *lib, ਅਚਲ ਅੱਖਰ *sym,
                                                         UError_ਕੋਡ *ਸਥਿਤੀ) {
  UPlug_ਅੰਕੜੇ *plug = ਖਾਲੀ_ਸੂਚਕ;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, ਸਥਿਤੀ);

  ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
    ਵਾਪਸ plug;
  } ਨਹੀਂਤਾਂ {
    uplug_deallocatePlug(plug, ਸਥਿਤੀ);
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
}

U_CAPI UPlug_ਅੰਕੜੇ* U_EXPORT2
uplug_ਲੱਦੋ_Plug_From_Entrypoint(UPlugEntrypoint *entrypoint, ਅਚਲ ਅੱਖਰ *config, UError_ਕੋਡ *ਸਥਿਤੀ) {
  UPlug_ਅੰਕੜੇ* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
  uplug_ਲੱਦੋ_Plug(plug, ਸਥਿਤੀ);
  ਵਾਪਸ plug;
}

#ਜੇ U_ENABLE_DYLOAD

ਸਥਿਰ UPlug_ਅੰਕੜੇ* 
uplug_initErrorPlug(ਅਚਲ ਅੱਖਰ *libName, ਅਚਲ ਅੱਖਰ *sym, ਅਚਲ ਅੱਖਰ *config, ਅਚਲ ਅੱਖਰ *nameOrError, UError_ਕੋਡ ਲੱਦੋ_ਸਥਿਤੀ, UError_ਕੋਡ *ਸਥਿਤੀ)
{
  UPlug_ਅੰਕੜੇ *plug = uplug_allocate_ਖਾਲੀ_ਥਾਵਾਂ_Plug(ਸਥਿਤੀ);
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

  plug->plugin_ਸਥਿਤੀ = ਲੱਦੋ_ਸਥਿਤੀ;
  plug->awaiting_ਲੱਦੋ = ਝੂਠ; /* Won't load. */
  plug->dontUnload = ਸੱਚ; /* cannot unload. */

  ਜੇ(sym!=ਖਾਲੀ_ਸੂਚਕ) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  ਜੇ(libName!=ਖਾਲੀ_ਸੂਚਕ) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  ਜੇ(nameOrError!=ਖਾਲੀ_ਸੂਚਕ) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  ਜੇ(config!=ਖਾਲੀ_ਸੂਚਕ) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  ਵਾਪਸ plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ਸਥਿਰ UPlug_ਅੰਕੜੇ* 
uplug_initPlugFromLibrary(ਅਚਲ ਅੱਖਰ *libName, ਅਚਲ ਅੱਖਰ *sym, ਅਚਲ ਅੱਖਰ *config, UError_ਕੋਡ *ਸਥਿਤੀ) {
  ਖਾਲੀ *lib = ਖਾਲੀ_ਸੂਚਕ;
  UPlug_ਅੰਕੜੇ *plug = ਖਾਲੀ_ਸੂਚਕ;
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
  lib = uplug_openLibrary(libName, ਸਥਿਤੀ);
  ਜੇ(lib!=ਖਾਲੀ_ਸੂਚਕ && U_SUCCESS(*ਸਥਿਤੀ)) {
    UPlugEntrypoint *entrypoint = ਖਾਲੀ_ਸੂਚਕ;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, ਸਥਿਤੀ);

    ਜੇ(entrypoint!=ਖਾਲੀ_ਸੂਚਕ&&U_SUCCESS(*ਸਥਿਤੀ)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, ਸਥਿਤੀ);
      ਜੇ(plug!=ਖਾਲੀ_ਸੂਚਕ&&U_SUCCESS(*ਸਥਿਤੀ)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = ਖਾਲੀ_ਸੂਚਕ; /* library is now owned by plugin. */
      }
    } ਨਹੀਂਤਾਂ {
      UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==ਖਾਲੀ_ਸੂਚਕ)?U_MISSING_RESOURCE_ERROR:*ਸਥਿਤੀ,&sub_ਸਥਿਤੀ);
    }
    ਜੇ(lib!=ਖਾਲੀ_ਸੂਚਕ) { /* still need to close the lib */
      UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &sub_ਸਥਿਤੀ); /* don't care here */
    }
  } ਨਹੀਂਤਾਂ {
    UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==ਖਾਲੀ_ਸੂਚਕ)?U_MISSING_RESOURCE_ERROR:*ਸਥਿਤੀ,&sub_ਸਥਿਤੀ);
  }
  ਵਾਪਸ plug;
}

U_CAPI UPlug_ਅੰਕੜੇ* U_EXPORT2
uplug_ਲੱਦੋ_Plug_From_Library(ਅਚਲ ਅੱਖਰ *libName, ਅਚਲ ਅੱਖਰ *sym, ਅਚਲ ਅੱਖਰ *config, UError_ਕੋਡ *ਸਥਿਤੀ) { 
  UPlug_ਅੰਕੜੇ *plug = ਖਾਲੀ_ਸੂਚਕ;
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, ਸਥਿਤੀ);
  uplug_ਲੱਦੋ_Plug(plug, ਸਥਿਤੀ);

  ਵਾਪਸ plug;
}

#ਜੇ_ਅੰਤ

ਸਥਿਰ UPlugLevel gCurrentLevel = UPLUG_LEVEL_ਨੀਵਾਂ;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  ਵਾਪਸ gCurrentLevel;
}

ਸਥਿਰ UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlug_ਅੰਕੜੇ *plugin_ਮੰਜ਼ਿਲ_Remove;
  /* cleanup plugs */
  ਲਈ(i=0;i<plugin_ਗਿਣਤੀ;i++) {
    UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR;
    plugin_ਮੰਜ਼ਿਲ_Remove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(plugin_ਮੰਜ਼ਿਲ_Remove, &sub_ਸਥਿਤੀ);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_ਨੀਵਾਂ;
  ਵਾਪਸ ਸੱਚ;
}

#ਜੇ U_ENABLE_DYLOAD

ਸਥਿਰ ਖਾਲੀ uplug_ਲੱਦੋ_ਉਡੀਕ_Plugs(UError_ਕੋਡ *ਸਥਿਤੀ) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
    ਵਾਪਸ;
  }
#ਜੇ UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#ਜੇ_ਅੰਤ
  /* pass #1: low level plugs */
  ਲਈ(i=0;i<plugin_ਗਿਣਤੀ;i++) {
    UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR;
    UPlug_ਅੰਕੜੇ *plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ = &pluginList[i];
    ਜੇ(plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ->awaiting_ਲੱਦੋ) {
      ਜੇ(plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ->level == UPLUG_LEVEL_ਨੀਵਾਂ) {
        ਜੇ(currentLevel > UPLUG_LEVEL_ਨੀਵਾਂ) {
          plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ->plugin_ਸਥਿਤੀ = U_PLUGIN_TOO_ਉੱਚਾ;
        } ਨਹੀਂਤਾਂ {
          UPlugLevel newLevel;
          uplug_ਲੱਦੋ_Plug(plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ, &sub_ਸਥਿਤੀ);
          newLevel = uplug_getCurrentLevel();
          ਜੇ(newLevel > currentLevel) {
            plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ->plugin_ਸਥਿਤੀ = U_PLUGIN_ਬਦਲਿਆ_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ->awaiting_ਲੱਦੋ = ਝੂਠ;
      } 
    }
  }    
  ਲਈ(i=0;i<plugin_ਗਿਣਤੀ;i++) {
    UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR;
    UPlug_ਅੰਕੜੇ *plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ = &pluginList[i];
        
    ਜੇ(plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ->awaiting_ਲੱਦੋ) {
      ਜੇ(plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ->level == UPLUG_LEVEL_INVALID) { 
        plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ->plugin_ਸਥਿਤੀ = U_PLUGIN_DIDNT_ਸੈੱਟ_LEVEL;
      } ਨਹੀਂਤਾਂ ਜੇ(plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ->level == UPLUG_LEVEL_UNKNOWN) {
        plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ->plugin_ਸਥਿਤੀ = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
      } ਨਹੀਂਤਾਂ {
        uplug_ਲੱਦੋ_Plug(plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ, &sub_ਸਥਿਤੀ);
      }
      plugin_ਮੰਜ਼ਿਲ_ਲੱਦੋ->awaiting_ਲੱਦੋ = ਝੂਠ;
    }
  }
    
#ਜੇ UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#ਜੇ_ਅੰਤ
}

/* Name of the plugin config file */
ਸਥਿਰ ਅੱਖਰ plugin_file[2048] = "";
#ਜੇ_ਅੰਤ

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uplug_getPluginFile() {
#ਜੇ U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  ਵਾਪਸ plugin_file;
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
#ਜੇ_ਅੰਤ
}


//  uplug_init()  is called first thing from u_init().

U_CAPI ਖਾਲੀ U_EXPORT2
uplug_init(UError_ਕੋਡ *ਸਥਿਤੀ) {
#ਜੇ !U_ENABLE_DYLOAD
  (ਖਾਲੀ)ਸਥਿਤੀ; /*ਅਣਵਰਤਿਆ*/
#ਜਾਂਜੇ !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ਅਚਲ ਅੱਖਰ *env = getenv("ICU_PLUGINS");

  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ;
  ਜੇ(env != ਖਾਲੀ_ਸੂਚਕ) {
    plugin_dir.append(env, -1, *ਸਥਿਤੀ);
  }
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ;

#ਜੇ defined(DEFAULT_ICU_PLUGINS) 
  ਜੇ(plugin_dir.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *ਸਥਿਤੀ);
  }
#ਜੇ_ਅੰਤ

#ਜੇ UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.ਅੰਕੜੇ()));
#ਜੇ_ਅੰਤ

  ਜੇ(!plugin_dir.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
    FILE *f;
        
    CharString pluginFile;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ICU_PLUGINS_DD
/* There are potentially a lot of ways to implement a plugin directory on OS390/zOS  */
/* Keeping in mind that unauthorized file access is logged, monitored, and enforced  */
/* I've chosen to open a DDNAME if BATCH and leave it alone for (presumably) UNIX    */
/* System Services.  Alternative techniques might be allocating a member in          */
/* SYS1.PARMLIB or setting an environment variable "ICU_PLUGIN_PATH" (?).  The       */
/* DDNAME can be connected to a file in the HFS if need be.                          */

    pluginFile.append("//DD:ICUPLUG", -1, *ਸਥਿਤੀ);        /* JAM 20 Oct 2011 */
#ਨਹੀਂਤਾਂ
    pluginFile.append(plugin_dir, *ਸਥਿਤੀ);
    pluginFile.append(U_FILE_SEP_STRING, -1, *ਸਥਿਤੀ);
    pluginFile.append("icuplugins", -1, *ਸਥਿਤੀ);
    pluginFile.append(U_ICU_ਸੰਸਕਰਣ_SHORT, -1, *ਸਥਿਤੀ);
    pluginFile.append(".txt", -1, *ਸਥਿਤੀ);
#ਜੇ_ਅੰਤ

#ਜੇ UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*ਸਥਿਤੀ)));
#ਜੇ_ਅੰਤ

    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
      ਵਾਪਸ;
    }
    ਜੇ((size_t)pluginFile.length() > (ਆਕਾਰ(plugin_file)-1)) {
      *ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
#ਜੇ UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*ਸਥਿਤੀ)));
#ਜੇ_ਅੰਤ
      ਵਾਪਸ;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, ਆਕਾਰ(plugin_file), *ਸਥਿਤੀ);

#ਜੇ UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(plugin_file), (ਪੂਰਨ)ਆਕਾਰ(plugin_file)));
#ਜੇ_ਅੰਤ
        
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MVS__
    ਜੇ (iscics()) /* 12 Nov 2011 JAM */
    {
        f = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
        f = fopen(pluginFile.ਅੰਕੜੇ(), "r");
    }

    ਜੇ(f != ਖਾਲੀ_ਸੂਚਕ) {
      ਅੱਖਰ linebuf[1024];
      ਅੱਖਰ *p, *libName=ਖਾਲੀ_ਸੂਚਕ, *symName=ਖਾਲੀ_ਸੂਚਕ, *config=ਖਾਲੀ_ਸੂਚਕ;
      int32_t ਸਤਰ = 0;
            
            
      ਜਦੋਂ(fgets(linebuf,1023,f)) {
        ਸਤਰ++;

        ਜੇ(!*linebuf || *linebuf=='#') {
          ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ {
          p = linebuf;
          ਜਦੋਂ(*p&&ਖਾਲੀ_ਥਾਂ_ਹੈ((ਪੂਰਨ)*p))
            p++;
          ਜੇ(!*p || *p=='#') ਜਾਰੀ;
          libName = p;
          ਜਦੋਂ(*p&&!ਖਾਲੀ_ਥਾਂ_ਹੈ((ਪੂਰਨ)*p)) {
            p++;
          }
          ਜੇ(!*p || *p=='#') ਜਾਰੀ; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          ਜਦੋਂ(*p&&ਖਾਲੀ_ਥਾਂ_ਹੈ((ਪੂਰਨ)*p)) {
            p++;
          }
          ਜੇ(!*p||*p=='#') ਜਾਰੀ; /* no symname after libname +tab */
          symName = p;
          ਜਦੋਂ(*p&&!ਖਾਲੀ_ਥਾਂ_ਹੈ((ਪੂਰਨ)*p)) {
            p++;
          }
                    
          ਜੇ(*p) { /* has config */
            *p=0;
            ++p;
            ਜਦੋਂ(*p&&ਖਾਲੀ_ਥਾਂ_ਹੈ((ਪੂਰਨ)*p)) {
              p++;
            }
            ਜੇ(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          ਜੇ(config!=ਖਾਲੀ_ਸੂਚਕ&&*config!=0) {
            p = config+ਲੜੀ_ਲੰਬਾਈ(config);
            ਜਦੋਂ(p>config&&ਖਾਲੀ_ਥਾਂ_ਹੈ((ਪੂਰਨ)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UError_ਕੋਡ sub_ਸਥਿਤੀ = U_ZERO_ERROR;
            UPlug_ਅੰਕੜੇ *plug = uplug_initPlugFromLibrary(libName, symName, config, &sub_ਸਥਿਤੀ);
            ਜੇ(U_FAILURE(sub_ਸਥਿਤੀ) && U_SUCCESS(*ਸਥਿਤੀ)) {
              *ਸਥਿਤੀ = sub_ਸਥਿਤੀ;
            }
#ਜੇ UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (ਖਾਲੀ*)plug, u_errorName(sub_ਸਥਿਤੀ)));
#ਨਹੀਂਤਾਂ
            (ਖਾਲੀ)plug; /*ਅਣਵਰਤਿਆ*/
#ਜੇ_ਅੰਤ
          }
        }
      }
      fclose(f);
    } ਨਹੀਂਤਾਂ {
#ਜੇ UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#ਜੇ_ਅੰਤ
    }
  }
  uplug_ਲੱਦੋ_ਉਡੀਕ_Plugs(ਸਥਿਤੀ);
#ਜੇ_ਅੰਤ /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_ਉੱਚਾ;
  ucln_ਰਜਿਸਟਰ_Cleanup(UCLN_UPLUG, uplug_cleanup);
}

#ਜੇ_ਅੰਤ


