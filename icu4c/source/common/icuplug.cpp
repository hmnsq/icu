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

#അന്തര്ഭാവ "unicode/icuplug.h"


#എങ്കിൽ UCONFIG_ENABLE_PLUGINS


#അന്തര്ഭാവ "icuplugimp.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "ucln.h"
#അന്തര്ഭാവ <stdio.h>
#ifdef __MVS__  /* defined by z/OS compiler */
#define _POSIX_SOURCE
#അന്തര്ഭാവ <cics.h> /* 12 Nov 2011 JAM iscics() function */
#endif
#അന്തര്ഭാവ "charstr.h"

using നാമാകാശ icu;

#ifndef UPLUG_TRACE
#define UPLUG_TRACE 0
#endif

#എങ്കിൽ UPLUG_TRACE
#അന്തര്ഭാവ <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

ഘടന UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  ശൂന്യം *lib;              /**< plugin library, or nullptr */
  അക്ഷരം libName[UPLUG_NAME_MAX];   /**< library name */
  അക്ഷരം sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  അക്ഷരം config[UPLUG_NAME_MAX];     /**< configuration data */
  ശൂന്യം *context;          /**< user context data */
  അക്ഷരം name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaitingLoad; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UErrorCode pluginStatus; /**< status code of plugin */
};



#define UPLUG_LIBRARY_INITIAL_എണ്ണം 8
#define UPLUG_PLUGIN_INITIAL_എണ്ണം 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
സ്ഥിതം int32_t uplug_removeEntryAt(ശൂന്യം *list, int32_t listSize, int32_t memberSize, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  എങ്കിൽ(listSize<1) {
    തിരികെ listSize;
  }
    
  /* is there anything to move? */
  എങ്കിൽ(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  തിരികെ listSize-1;
}




#എങ്കിൽ U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
ഘടന UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
തരനിർവചനം ഘടന UPlugLibrary {
  ശൂന്യം *lib;                           /**< library ptr */
  അക്ഷരം name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

സ്ഥിതം UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_എണ്ണം];
സ്ഥിതം UPlugLibrary * libraryList = staticLibraryList;
സ്ഥിതം int32_t library_എണ്ണം = 0;
സ്ഥിതം int32_t libraryMax = UPLUG_LIBRARY_INITIAL_എണ്ണം;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
സ്ഥിതം int32_t തിരയൽ_For_Library_Name(സ്ഥിരം അക്ഷരം *libName) {
  int32_t i;
    
  വേണ്ടി(i=0;i<library_എണ്ണം;i++) {
    എങ്കിൽ(!uprv_strcmp(libName, libraryList[i].name)) {
      തിരികെ i;
    }
  }
  തിരികെ -1;
}

സ്ഥിതം int32_t തിരയൽ_For_Library(ശൂന്യം *lib) {
  int32_t i;
    
  വേണ്ടി(i=0;i<library_എണ്ണം;i++) {
    എങ്കിൽ(lib==libraryList[i].lib) {
      തിരികെ i;
    }
  }
  തിരികെ -1;
}

U_CAPI അക്ഷരം * U_EXPORT2
uplug_findLibrary(ശൂന്യം *lib, UErrorCode *status) {
  int32_t libEnt;
  അക്ഷരം *ret = nullptr;
  എങ്കിൽ(U_FAILURE(*status)) {
    തിരികെ nullptr;
  }
  libEnt = തിരയൽ_For_Library(lib);
  എങ്കിൽ(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } അല്ലെങ്കിൽ {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  തിരികെ ret;
}

U_CAPI ശൂന്യം * U_EXPORT2
uplug_openLibrary(സ്ഥിരം അക്ഷരം *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  ശൂന്യം *lib = nullptr;
    
  എങ്കിൽ(U_FAILURE(*status)) തിരികെ nullptr;

  libEntry = തിരയൽ_For_Library_Name(libName);
  എങ്കിൽ(libEntry == -1) {
    libEntry = library_എണ്ണം++;
    എങ്കിൽ(library_എണ്ണം >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#എങ്കിൽ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      തിരികെ nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#എങ്കിൽ UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    എങ്കിൽ(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#എങ്കിൽ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      library_എണ്ണം--;
    } അല്ലെങ്കിൽ { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } അല്ലെങ്കിൽ {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  തിരികെ lib;
}

U_CAPI ശൂന്യം U_EXPORT2
uplug_closeLibrary(ശൂന്യം *lib, UErrorCode *status) {
  int32_t i;
    
#എങ്കിൽ UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (ശൂന്യം*)libraryList));
#endif
  എങ്കിൽ(U_FAILURE(*status)) തിരികെ;
    
  വേണ്ടി(i=0;i<library_എണ്ണം;i++) {
    എങ്കിൽ(lib==libraryList[i].lib) {
      എങ്കിൽ(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_എണ്ണം = uplug_removeEntryAt(libraryList, library_എണ്ണം, വലുപ്പം(*libraryList), i);
      }
      തിരികെ;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

സ്ഥിതം UPlugData pluginList[UPLUG_PLUGIN_INITIAL_എണ്ണം];
സ്ഥിതം int32_t plugin_എണ്ണം = 0;



  
സ്ഥിതം int32_t uplug_plugin_സംഖ്യ(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[plugin_എണ്ണം];
  എങ്കിൽ(d<=pluginList) {
    തിരികെ 0;
  } അല്ലെങ്കിൽ എങ്കിൽ(d>=pastPlug) {
    തിരികെ plugin_എണ്ണം;
  } അല്ലെങ്കിൽ {
    തിരികെ (d-pluginList)/വലുപ്പം(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  എങ്കിൽ(prior==nullptr) {
    തിരികെ pluginList;
  } അല്ലെങ്കിൽ {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[plugin_എണ്ണം];
    
    എങ്കിൽ(nextPlug>=pastPlug) {
      തിരികെ nullptr;
    } അല്ലെങ്കിൽ {
      തിരികെ nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
സ്ഥിതം ശൂന്യം uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  എങ്കിൽ(plug==nullptr||U_FAILURE(*status)) {
    തിരികെ;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  എങ്കിൽ(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


സ്ഥിതം ശൂന്യം uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  എങ്കിൽ(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    തിരികെ; 
  }
  എങ്കിൽ(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

സ്ഥിതം ശൂന്യം uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  എങ്കിൽ(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    തിരികെ; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  എങ്കിൽ(U_SUCCESS(*status)) { 
    എങ്കിൽ(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = അസത്യം;
    }
  } അല്ലെങ്കിൽ {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = അസത്യം;
  }
}


സ്ഥിതം ശൂന്യം uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  എങ്കിൽ(U_FAILURE(*status)) {
    തിരികെ;
  }
  എങ്കിൽ(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_താഴെയുള്ള) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    തിരികെ;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = അസത്യം;
  എങ്കിൽ(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

സ്ഥിതം UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  എങ്കിൽ(U_FAILURE(*status)) {
    തിരികെ nullptr;
  }

  എങ്കിൽ(plugin_എണ്ണം == UPLUG_PLUGIN_INITIAL_എണ്ണം) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    തിരികെ nullptr;
  }

  plug = &pluginList[plugin_എണ്ണം++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = വലുപ്പം(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = സത്യം;
  plug->dontUnload = അസത്യം;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  തിരികെ plug;
}

സ്ഥിതം UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, സ്ഥിരം അക്ഷരം *config, ശൂന്യം *lib, സ്ഥിരം അക്ഷരം *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  എങ്കിൽ(U_FAILURE(*status)) {
    തിരികെ nullptr;
  }

  എങ്കിൽ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } അല്ലെങ്കിൽ {
    plug->config[0] = 0;
  }
    
  എങ്കിൽ(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } അല്ലെങ്കിൽ {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  തിരികെ plug;
}

സ്ഥിതം ശൂന്യം uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  എങ്കിൽ(!plug->dontUnload) {
#എങ്കിൽ U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  എങ്കിൽ(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  എങ്കിൽ(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_എണ്ണം = uplug_removeEntryAt(pluginList, plugin_എണ്ണം, വലുപ്പം(plug[0]), uplug_plugin_സംഖ്യ(plug));
  } അല്ലെങ്കിൽ {
    /* not ok- leave as a message. */
    plug->awaitingLoad=അസത്യം;
    plug->entrypoint=0;
    plug->dontUnload=സത്യം;
  }
}

സ്ഥിതം ശൂന്യം uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  എങ്കിൽ(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI ശൂന്യം U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  എങ്കിൽ(U_FAILURE(*status)) തിരികെ;
    
  വേണ്ടി(cursor=pluginList;cursor!=nullptr;) {
    എങ്കിൽ(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } അല്ലെങ്കിൽ {
      cursor = uplug_nextPlug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI ശൂന്യം U_EXPORT2 
uplug_setPlugNoUnload(UPlugData *data, UBool dontUnload)
{
  data->dontUnload = dontUnload;
}


U_CAPI ശൂന്യം U_EXPORT2
uplug_setPlugLevel(UPlugData *data, UPlugLevel level) {
  data->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlugData *data) {
  തിരികെ data->level;
}


U_CAPI ശൂന്യം U_EXPORT2
uplug_setPlugName(UPlugData *data, സ്ഥിരം അക്ഷരം *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  തിരികെ data->name;
}


U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
uplug_get_ചിഹ്നം_Name(UPlugData* data U_LIFETIME_BOUND) {
  തിരികെ data->sym;
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  എങ്കിൽ(data->libName[0]) {
    തിരികെ data->libName;
  } അല്ലെങ്കിൽ {
#എങ്കിൽ U_ENABLE_DYLOAD
    തിരികെ uplug_findLibrary(data->lib, status);
#അല്ലെങ്കിൽ
    തിരികെ nullptr;
#endif
  }
}

U_CAPI ശൂന്യം * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  തിരികെ data->lib;
}

U_CAPI ശൂന്യം * U_EXPORT2
uplug_getContext(UPlugData *data) {
  തിരികെ data->context;
}


U_CAPI ശൂന്യം U_EXPORT2
uplug_setContext(UPlugData *data, ശൂന്യം *context) {
  data->context = context;
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  തിരികെ data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  എങ്കിൽ(n <0 || n >= plugin_എണ്ണം) {
    തിരികെ nullptr;
  } അല്ലെങ്കിൽ { 
    തിരികെ &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  തിരികെ plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
സ്ഥിതം UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, സ്ഥിരം അക്ഷരം *config, ശൂന്യം *lib, സ്ഥിരം അക്ഷരം *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  എങ്കിൽ(U_SUCCESS(*status)) {
    തിരികെ plug;
  } അല്ലെങ്കിൽ {
    uplug_deallocatePlug(plug, status);
    തിരികെ nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, സ്ഥിരം അക്ഷരം *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  തിരികെ plug;
}

#എങ്കിൽ U_ENABLE_DYLOAD

സ്ഥിതം UPlugData* 
uplug_initErrorPlug(സ്ഥിരം അക്ഷരം *libName, സ്ഥിരം അക്ഷരം *sym, സ്ഥിരം അക്ഷരം *config, സ്ഥിരം അക്ഷരം *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  എങ്കിൽ(U_FAILURE(*status)) തിരികെ nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = അസത്യം; /* Won't load. */
  plug->dontUnload = സത്യം; /* cannot unload. */

  എങ്കിൽ(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  എങ്കിൽ(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  എങ്കിൽ(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  എങ്കിൽ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  തിരികെ plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
സ്ഥിതം UPlugData* 
uplug_initPlugFromLibrary(സ്ഥിരം അക്ഷരം *libName, സ്ഥിരം അക്ഷരം *sym, സ്ഥിരം അക്ഷരം *config, UErrorCode *status) {
  ശൂന്യം *lib = nullptr;
  UPlugData *plug = nullptr;
  എങ്കിൽ(U_FAILURE(*status)) { തിരികെ nullptr; }
  lib = uplug_openLibrary(libName, status);
  എങ്കിൽ(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    എങ്കിൽ(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      എങ്കിൽ(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } അല്ലെങ്കിൽ {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    എങ്കിൽ(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } അല്ലെങ്കിൽ {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  തിരികെ plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(സ്ഥിരം അക്ഷരം *libName, സ്ഥിരം അക്ഷരം *sym, സ്ഥിരം അക്ഷരം *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  എങ്കിൽ(U_FAILURE(*status)) { തിരികെ nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  തിരികെ plug;
}

#endif

സ്ഥിതം UPlugLevel gCurrentLevel = UPLUG_LEVEL_താഴെയുള്ള;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  തിരികെ gCurrentLevel;
}

സ്ഥിതം UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  വേണ്ടി(i=0;i<plugin_എണ്ണം;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_താഴെയുള്ള;
  തിരികെ സത്യം;
}

#എങ്കിൽ U_ENABLE_DYLOAD

സ്ഥിതം ശൂന്യം uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  എങ്കിൽ(U_FAILURE(*status)) {
    തിരികെ;
  }
#എങ്കിൽ UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  വേണ്ടി(i=0;i<plugin_എണ്ണം;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    എങ്കിൽ(pluginToLoad->awaitingLoad) {
      എങ്കിൽ(pluginToLoad->level == UPLUG_LEVEL_താഴെയുള്ള) {
        എങ്കിൽ(currentLevel > UPLUG_LEVEL_താഴെയുള്ള) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_മുകളിലെ;
        } അല്ലെങ്കിൽ {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          എങ്കിൽ(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = അസത്യം;
      } 
    }
  }    
  വേണ്ടി(i=0;i<plugin_എണ്ണം;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    എങ്കിൽ(pluginToLoad->awaitingLoad) {
      എങ്കിൽ(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } അല്ലെങ്കിൽ എങ്കിൽ(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } അല്ലെങ്കിൽ {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = അസത്യം;
    }
  }
    
#എങ്കിൽ UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
സ്ഥിതം അക്ഷരം plugin_file[2048] = "";
#endif

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
uplug_getPluginFile() {
#എങ്കിൽ U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  തിരികെ plugin_file;
#അല്ലെങ്കിൽ
  തിരികെ nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI ശൂന്യം U_EXPORT2
uplug_init(UErrorCode *status) {
#എങ്കിൽ !U_ENABLE_DYLOAD
  (ശൂന്യം)status; /* unused */
#അല്ലെങ്കിലെങ്കിൽ !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  സ്ഥിരം അക്ഷരം *env = getenv("ICU_PLUGINS");

  എങ്കിൽ(U_FAILURE(*status)) തിരികെ;
  എങ്കിൽ(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  എങ്കിൽ(U_FAILURE(*status)) തിരികെ;

#എങ്കിൽ defined(DEFAULT_ICU_PLUGINS) 
  എങ്കിൽ(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#എങ്കിൽ UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  എങ്കിൽ(!plugin_dir.isEmpty()) {
    FILE *f;
        
    CharString pluginFile;
#ifdef ICU_PLUGINS_DD
/* There are potentially a lot of ways to implement a plugin directory on OS390/zOS  */
/* Keeping in mind that unauthorized file access is logged, monitored, and enforced  */
/* I've chosen to open a DDNAME if BATCH and leave it alone for (presumably) UNIX    */
/* System Services.  Alternative techniques might be allocating a member in          */
/* SYS1.PARMLIB or setting an environment variable "ICU_PLUGIN_PATH" (?).  The       */
/* DDNAME can be connected to a file in the HFS if need be.                          */

    pluginFile.append("//DD:ICUPLUG", -1, *status);        /* JAM 20 Oct 2011 */
#അല്ലെങ്കിൽ
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#എങ്കിൽ UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    എങ്കിൽ(U_FAILURE(*status)) {
      തിരികെ;
    }
    എങ്കിൽ((size_t)pluginFile.length() > (വലുപ്പം(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#എങ്കിൽ UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      തിരികെ;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, വലുപ്പം(plugin_file), *status);

#എങ്കിൽ UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (പൂർണ്ണസംഖ്യ)strlen(plugin_file), (പൂർണ്ണസംഖ്യ)വലുപ്പം(plugin_file)));
#endif
        
#ifdef __MVS__
    എങ്കിൽ (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    അല്ലെങ്കിൽ
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    എങ്കിൽ(f != nullptr) {
      അക്ഷരം linebuf[1024];
      അക്ഷരം *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t വരി = 0;
            
            
      ആകുമ്പോൾ(fgets(linebuf,1023,f)) {
        വരി++;

        എങ്കിൽ(!*linebuf || *linebuf=='#') {
          തുടരുക;
        } അല്ലെങ്കിൽ {
          p = linebuf;
          ആകുമ്പോൾ(*p&&isspace((പൂർണ്ണസംഖ്യ)*p))
            p++;
          എങ്കിൽ(!*p || *p=='#') തുടരുക;
          libName = p;
          ആകുമ്പോൾ(*p&&!isspace((പൂർണ്ണസംഖ്യ)*p)) {
            p++;
          }
          എങ്കിൽ(!*p || *p=='#') തുടരുക; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          ആകുമ്പോൾ(*p&&isspace((പൂർണ്ണസംഖ്യ)*p)) {
            p++;
          }
          എങ്കിൽ(!*p||*p=='#') തുടരുക; /* no symname after libname +tab */
          symName = p;
          ആകുമ്പോൾ(*p&&!isspace((പൂർണ്ണസംഖ്യ)*p)) {
            p++;
          }
                    
          എങ്കിൽ(*p) { /* has config */
            *p=0;
            ++p;
            ആകുമ്പോൾ(*p&&isspace((പൂർണ്ണസംഖ്യ)*p)) {
              p++;
            }
            എങ്കിൽ(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          എങ്കിൽ(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            ആകുമ്പോൾ(p>config&&isspace((പൂർണ്ണസംഖ്യ)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            എങ്കിൽ(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#എങ്കിൽ UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (ശൂന്യം*)plug, u_errorName(subStatus)));
#അല്ലെങ്കിൽ
            (ശൂന്യം)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } അല്ലെങ്കിൽ {
#എങ്കിൽ UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_മുകളിലെ;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


