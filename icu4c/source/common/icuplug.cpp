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

#ಅನ್ತರ್ಭಾವ "unicode/icuplug.h"


#ಯದಿ UCONFIG_ENABLE_PLUGINS


#ಅನ್ತರ್ಭಾವ "icuplugimp.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "ucln.h"
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ifdef __MVS__  /* defined by z/OS compiler */
#define _POSIX_SOURCE
#ಅನ್ತರ್ಭಾವ <cics.h> /* 12 Nov 2011 JAM iscics() function */
#endif
#ಅನ್ತರ್ಭಾವ "charstr.h"

using ನಾಮಾಕಾಶ icu;

#ifndef UPLUG_TRACE
#define UPLUG_TRACE 0
#endif

#ಯದಿ UPLUG_TRACE
#ಅನ್ತರ್ಭಾವ <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

ಸಂರಚನಾ UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  ಶೂನ್ಯ *lib;              /**< plugin library, or nullptr */
  ವರ್ಣ libName[UPLUG_NAME_MAX];   /**< library name */
  ವರ್ಣ sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  ವರ್ಣ config[UPLUG_NAME_MAX];     /**< configuration data */
  ಶೂನ್ಯ *context;          /**< user context data */
  ವರ್ಣ name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaitingLoad; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UErrorCode pluginStatus; /**< status code of plugin */
};



#define UPLUG_LIBRARY_INITIAL_ಎಣಿಕೆ 8
#define UPLUG_PLUGIN_INITIAL_ಎಣಿಕೆ 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
ಸ್ಥಿರ int32_t uplug_removeEntryAt(ಶೂನ್ಯ *list, int32_t listSize, int32_t memberSize, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  ಯದಿ(listSize<1) {
    ಪ್ರತ್ಯಾಗಮ listSize;
  }
    
  /* is there anything to move? */
  ಯದಿ(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  ಪ್ರತ್ಯಾಗಮ listSize-1;
}




#ಯದಿ U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
ಸಂರಚನಾ UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ UPlugLibrary {
  ಶೂನ್ಯ *lib;                           /**< library ptr */
  ವರ್ಣ name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

ಸ್ಥಿರ UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_ಎಣಿಕೆ];
ಸ್ಥಿರ UPlugLibrary * libraryList = staticLibraryList;
ಸ್ಥಿರ int32_t library_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ int32_t libraryMax = UPLUG_LIBRARY_INITIAL_ಎಣಿಕೆ;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ಸ್ಥಿರ int32_t ಹುಡುಕು_For_Library_Name(ಧ್ರುವ ವರ್ಣ *libName) {
  int32_t i;
    
  ಪ್ರತಿ(i=0;i<library_ಎಣಿಕೆ;i++) {
    ಯದಿ(!uprv_strcmp(libName, libraryList[i].name)) {
      ಪ್ರತ್ಯಾಗಮ i;
    }
  }
  ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ int32_t ಹುಡುಕು_For_Library(ಶೂನ್ಯ *lib) {
  int32_t i;
    
  ಪ್ರತಿ(i=0;i<library_ಎಣಿಕೆ;i++) {
    ಯದಿ(lib==libraryList[i].lib) {
      ಪ್ರತ್ಯಾಗಮ i;
    }
  }
  ಪ್ರತ್ಯಾಗಮ -1;
}

U_CAPI ವರ್ಣ * U_EXPORT2
uplug_findLibrary(ಶೂನ್ಯ *lib, UErrorCode *status) {
  int32_t libEnt;
  ವರ್ಣ *ret = nullptr;
  ಯದಿ(U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  libEnt = ಹುಡುಕು_For_Library(lib);
  ಯದಿ(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } ಅನ್ಯಥಾ {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  ಪ್ರತ್ಯಾಗಮ ret;
}

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uplug_openLibrary(ಧ್ರುವ ವರ್ಣ *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  ಶೂನ್ಯ *lib = nullptr;
    
  ಯದಿ(U_FAILURE(*status)) ಪ್ರತ್ಯಾಗಮ nullptr;

  libEntry = ಹುಡುಕು_For_Library_Name(libName);
  ಯದಿ(libEntry == -1) {
    libEntry = library_ಎಣಿಕೆ++;
    ಯದಿ(library_ಎಣಿಕೆ >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#ಯದಿ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#ಯದಿ UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    ಯದಿ(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#ಯದಿ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      library_ಎಣಿಕೆ--;
    } ಅನ್ಯಥಾ { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } ಅನ್ಯಥಾ {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  ಪ್ರತ್ಯಾಗಮ lib;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_closeLibrary(ಶೂನ್ಯ *lib, UErrorCode *status) {
  int32_t i;
    
#ಯದಿ UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (ಶೂನ್ಯ*)libraryList));
#endif
  ಯದಿ(U_FAILURE(*status)) ಪ್ರತ್ಯಾಗಮ;
    
  ಪ್ರತಿ(i=0;i<library_ಎಣಿಕೆ;i++) {
    ಯದಿ(lib==libraryList[i].lib) {
      ಯದಿ(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_ಎಣಿಕೆ = uplug_removeEntryAt(libraryList, library_ಎಣಿಕೆ, ಪರಿಮಾಣ(*libraryList), i);
      }
      ಪ್ರತ್ಯಾಗಮ;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

ಸ್ಥಿರ UPlugData pluginList[UPLUG_PLUGIN_INITIAL_ಎಣಿಕೆ];
ಸ್ಥಿರ int32_t plugin_ಎಣಿಕೆ = 0;



  
ಸ್ಥಿರ int32_t uplug_plugin_ಸಂಖ್ಯೆ(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[plugin_ಎಣಿಕೆ];
  ಯದಿ(d<=pluginList) {
    ಪ್ರತ್ಯಾಗಮ 0;
  } ಅನ್ಯಥಾ ಯದಿ(d>=pastPlug) {
    ಪ್ರತ್ಯಾಗಮ plugin_ಎಣಿಕೆ;
  } ಅನ್ಯಥಾ {
    ಪ್ರತ್ಯಾಗಮ (d-pluginList)/ಪರಿಮಾಣ(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  ಯದಿ(prior==nullptr) {
    ಪ್ರತ್ಯಾಗಮ pluginList;
  } ಅನ್ಯಥಾ {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[plugin_ಎಣಿಕೆ];
    
    ಯದಿ(nextPlug>=pastPlug) {
      ಪ್ರತ್ಯಾಗಮ nullptr;
    } ಅನ್ಯಥಾ {
      ಪ್ರತ್ಯಾಗಮ nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ಸ್ಥಿರ ಶೂನ್ಯ uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  ಯದಿ(plug==nullptr||U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  ಯದಿ(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


ಸ್ಥಿರ ಶೂನ್ಯ uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  ಯದಿ(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ಪ್ರತ್ಯಾಗಮ; 
  }
  ಯದಿ(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  ಯದಿ(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ಪ್ರತ್ಯಾಗಮ; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  ಯದಿ(U_SUCCESS(*status)) { 
    ಯದಿ(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = ಅಸತ್ಯ;
    }
  } ಅನ್ಯಥಾ {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = ಅಸತ್ಯ;
  }
}


ಸ್ಥಿರ ಶೂನ್ಯ uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  ಯದಿ(U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ;
  }
  ಯದಿ(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_ಕೆಳಗಿನ) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ಪ್ರತ್ಯಾಗಮ;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = ಅಸತ್ಯ;
  ಯದಿ(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

ಸ್ಥಿರ UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  ಯದಿ(U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  ಯದಿ(plugin_ಎಣಿಕೆ == UPLUG_PLUGIN_INITIAL_ಎಣಿಕೆ) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  plug = &pluginList[plugin_ಎಣಿಕೆ++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = ಪರಿಮಾಣ(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = ಸತ್ಯ;
  plug->dontUnload = ಅಸತ್ಯ;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  ಪ್ರತ್ಯಾಗಮ plug;
}

ಸ್ಥಿರ UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ಧ್ರುವ ವರ್ಣ *config, ಶೂನ್ಯ *lib, ಧ್ರುವ ವರ್ಣ *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  ಯದಿ(U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }

  ಯದಿ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } ಅನ್ಯಥಾ {
    plug->config[0] = 0;
  }
    
  ಯದಿ(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } ಅನ್ಯಥಾ {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  ಪ್ರತ್ಯಾಗಮ plug;
}

ಸ್ಥಿರ ಶೂನ್ಯ uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  ಯದಿ(!plug->dontUnload) {
#ಯದಿ U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  ಯದಿ(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  ಯದಿ(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_ಎಣಿಕೆ = uplug_removeEntryAt(pluginList, plugin_ಎಣಿಕೆ, ಪರಿಮಾಣ(plug[0]), uplug_plugin_ಸಂಖ್ಯೆ(plug));
  } ಅನ್ಯಥಾ {
    /* not ok- leave as a message. */
    plug->awaitingLoad=ಅಸತ್ಯ;
    plug->entrypoint=0;
    plug->dontUnload=ಸತ್ಯ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  ಯದಿ(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  ಯದಿ(U_FAILURE(*status)) ಪ್ರತ್ಯಾಗಮ;
    
  ಪ್ರತಿ(cursor=pluginList;cursor!=nullptr;) {
    ಯದಿ(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } ಅನ್ಯಥಾ {
      cursor = uplug_nextPlug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI ಶೂನ್ಯ U_EXPORT2 
uplug_setPlugNoUnload(UPlugData *data, UBool dontUnload)
{
  data->dontUnload = dontUnload;
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_setPlugLevel(UPlugData *data, UPlugLevel level) {
  data->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlugData *data) {
  ಪ್ರತ್ಯಾಗಮ data->level;
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_setPlugName(UPlugData *data, ಧ್ರುವ ವರ್ಣ *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  ಪ್ರತ್ಯಾಗಮ data->name;
}


U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uplug_get_ಚಿಹ್ನೆ_Name(UPlugData* data U_LIFETIME_BOUND) {
  ಪ್ರತ್ಯಾಗಮ data->sym;
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  ಯದಿ(data->libName[0]) {
    ಪ್ರತ್ಯಾಗಮ data->libName;
  } ಅನ್ಯಥಾ {
#ಯದಿ U_ENABLE_DYLOAD
    ಪ್ರತ್ಯಾಗಮ uplug_findLibrary(data->lib, status);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ nullptr;
#endif
  }
}

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  ಪ್ರತ್ಯಾಗಮ data->lib;
}

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uplug_getContext(UPlugData *data) {
  ಪ್ರತ್ಯಾಗಮ data->context;
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_setContext(UPlugData *data, ಶೂನ್ಯ *context) {
  data->context = context;
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  ಪ್ರತ್ಯಾಗಮ data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  ಯದಿ(n <0 || n >= plugin_ಎಣಿಕೆ) {
    ಪ್ರತ್ಯಾಗಮ nullptr;
  } ಅನ್ಯಥಾ { 
    ಪ್ರತ್ಯಾಗಮ &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  ಪ್ರತ್ಯಾಗಮ plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ಸ್ಥಿರ UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ಧ್ರುವ ವರ್ಣ *config, ಶೂನ್ಯ *lib, ಧ್ರುವ ವರ್ಣ *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  ಯದಿ(U_SUCCESS(*status)) {
    ಪ್ರತ್ಯಾಗಮ plug;
  } ಅನ್ಯಥಾ {
    uplug_deallocatePlug(plug, status);
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, ಧ್ರುವ ವರ್ಣ *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  ಪ್ರತ್ಯಾಗಮ plug;
}

#ಯದಿ U_ENABLE_DYLOAD

ಸ್ಥಿರ UPlugData* 
uplug_initErrorPlug(ಧ್ರುವ ವರ್ಣ *libName, ಧ್ರುವ ವರ್ಣ *sym, ಧ್ರುವ ವರ್ಣ *config, ಧ್ರುವ ವರ್ಣ *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  ಯದಿ(U_FAILURE(*status)) ಪ್ರತ್ಯಾಗಮ nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = ಅಸತ್ಯ; /* Won't load. */
  plug->dontUnload = ಸತ್ಯ; /* cannot unload. */

  ಯದಿ(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  ಯದಿ(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  ಯದಿ(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  ಯದಿ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  ಪ್ರತ್ಯಾಗಮ plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ಸ್ಥಿರ UPlugData* 
uplug_initPlugFromLibrary(ಧ್ರುವ ವರ್ಣ *libName, ಧ್ರುವ ವರ್ಣ *sym, ಧ್ರುವ ವರ್ಣ *config, UErrorCode *status) {
  ಶೂನ್ಯ *lib = nullptr;
  UPlugData *plug = nullptr;
  ಯದಿ(U_FAILURE(*status)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
  lib = uplug_openLibrary(libName, status);
  ಯದಿ(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    ಯದಿ(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      ಯದಿ(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } ಅನ್ಯಥಾ {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    ಯದಿ(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } ಅನ್ಯಥಾ {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  ಪ್ರತ್ಯಾಗಮ plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(ಧ್ರುವ ವರ್ಣ *libName, ಧ್ರುವ ವರ್ಣ *sym, ಧ್ರುವ ವರ್ಣ *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  ಯದಿ(U_FAILURE(*status)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  ಪ್ರತ್ಯಾಗಮ plug;
}

#endif

ಸ್ಥಿರ UPlugLevel gCurrentLevel = UPLUG_LEVEL_ಕೆಳಗಿನ;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  ಪ್ರತ್ಯಾಗಮ gCurrentLevel;
}

ಸ್ಥಿರ UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  ಪ್ರತಿ(i=0;i<plugin_ಎಣಿಕೆ;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_ಕೆಳಗಿನ;
  ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

#ಯದಿ U_ENABLE_DYLOAD

ಸ್ಥಿರ ಶೂನ್ಯ uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  ಯದಿ(U_FAILURE(*status)) {
    ಪ್ರತ್ಯಾಗಮ;
  }
#ಯದಿ UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  ಪ್ರತಿ(i=0;i<plugin_ಎಣಿಕೆ;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    ಯದಿ(pluginToLoad->awaitingLoad) {
      ಯದಿ(pluginToLoad->level == UPLUG_LEVEL_ಕೆಳಗಿನ) {
        ಯದಿ(currentLevel > UPLUG_LEVEL_ಕೆಳಗಿನ) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_ಮೇಲಿನ;
        } ಅನ್ಯಥಾ {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          ಯದಿ(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = ಅಸತ್ಯ;
      } 
    }
  }    
  ಪ್ರತಿ(i=0;i<plugin_ಎಣಿಕೆ;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    ಯದಿ(pluginToLoad->awaitingLoad) {
      ಯದಿ(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } ಅನ್ಯಥಾ ಯದಿ(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } ಅನ್ಯಥಾ {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = ಅಸತ್ಯ;
    }
  }
    
#ಯದಿ UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
ಸ್ಥಿರ ವರ್ಣ plugin_file[2048] = "";
#endif

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uplug_getPluginFile() {
#ಯದಿ U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  ಪ್ರತ್ಯಾಗಮ plugin_file;
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_init(UErrorCode *status) {
#ಯದಿ !U_ENABLE_DYLOAD
  (ಶೂನ್ಯ)status; /* unused */
#ನೋಚೇದ್ಯದಿ !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ಧ್ರುವ ವರ್ಣ *env = getenv("ICU_PLUGINS");

  ಯದಿ(U_FAILURE(*status)) ಪ್ರತ್ಯಾಗಮ;
  ಯದಿ(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  ಯದಿ(U_FAILURE(*status)) ಪ್ರತ್ಯಾಗಮ;

#ಯದಿ defined(DEFAULT_ICU_PLUGINS) 
  ಯದಿ(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#ಯದಿ UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  ಯದಿ(!plugin_dir.isEmpty()) {
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
#ಅನ್ಯಥಾ
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#ಯದಿ UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    ಯದಿ(U_FAILURE(*status)) {
      ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ((size_t)pluginFile.length() > (ಪರಿಮಾಣ(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#ಯದಿ UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      ಪ್ರತ್ಯಾಗಮ;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, ಪರಿಮಾಣ(plugin_file), *status);

#ಯದಿ UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (ಪೂರ್ಣಾಙ್ಕ)strlen(plugin_file), (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(plugin_file)));
#endif
        
#ifdef __MVS__
    ಯದಿ (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    ಅನ್ಯಥಾ
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    ಯದಿ(f != nullptr) {
      ವರ್ಣ linebuf[1024];
      ವರ್ಣ *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t ಸಾಲು = 0;
            
            
      ಯಾವತ್(fgets(linebuf,1023,f)) {
        ಸಾಲು++;

        ಯದಿ(!*linebuf || *linebuf=='#') {
          ಅನುವರ್ತ;
        } ಅನ್ಯಥಾ {
          p = linebuf;
          ಯಾವತ್(*p&&isspace((ಪೂರ್ಣಾಙ್ಕ)*p))
            p++;
          ಯದಿ(!*p || *p=='#') ಅನುವರ್ತ;
          libName = p;
          ಯಾವತ್(*p&&!isspace((ಪೂರ್ಣಾಙ್ಕ)*p)) {
            p++;
          }
          ಯದಿ(!*p || *p=='#') ಅನುವರ್ತ; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          ಯಾವತ್(*p&&isspace((ಪೂರ್ಣಾಙ್ಕ)*p)) {
            p++;
          }
          ಯದಿ(!*p||*p=='#') ಅನುವರ್ತ; /* no symname after libname +tab */
          symName = p;
          ಯಾವತ್(*p&&!isspace((ಪೂರ್ಣಾಙ್ಕ)*p)) {
            p++;
          }
                    
          ಯದಿ(*p) { /* has config */
            *p=0;
            ++p;
            ಯಾವತ್(*p&&isspace((ಪೂರ್ಣಾಙ್ಕ)*p)) {
              p++;
            }
            ಯದಿ(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          ಯದಿ(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            ಯಾವತ್(p>config&&isspace((ಪೂರ್ಣಾಙ್ಕ)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            ಯದಿ(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#ಯದಿ UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (ಶೂನ್ಯ*)plug, u_errorName(subStatus)));
#ಅನ್ಯಥಾ
            (ಶೂನ್ಯ)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } ಅನ್ಯಥಾ {
#ಯದಿ UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_ಮೇಲಿನ;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


