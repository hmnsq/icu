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

#அந்தர்பாவ "unicode/icuplug.h"


#எனில் UCONFIG_ENABLE_PLUGINS


#அந்தர்பாவ "icuplugimp.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "putilimp.h"
#அந்தர்பாவ "ucln.h"
#அந்தர்பாவ <stdio.h>
#ifdef __MVS__  /* defined by z/OS compiler */
#define _POSIX_SOURCE
#அந்தர்பாவ <cics.h> /* 12 Nov 2011 JAM iscics() function */
#endif
#அந்தர்பாவ "charstr.h"

using நாமாகாஶ icu;

#ifndef UPLUG_TRACE
#define UPLUG_TRACE 0
#endif

#எனில் UPLUG_TRACE
#அந்தர்பாவ <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

கட்டமைப்பு UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  வெறுமை *lib;              /**< plugin library, or nullptr */
  எழுத்து libName[UPLUG_NAME_MAX];   /**< library name */
  எழுத்து sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  எழுத்து config[UPLUG_NAME_MAX];     /**< configuration data */
  வெறுமை *context;          /**< user context data */
  எழுத்து name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaitingLoad; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UErrorCode pluginStatus; /**< status code of plugin */
};



#define UPLUG_LIBRARY_INITIAL_மொத்தம் 8
#define UPLUG_PLUGIN_INITIAL_மொத்தம் 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
நிலையான int32_t uplug_removeEntryAt(வெறுமை *list, int32_t listSize, int32_t memberSize, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  எனில்(listSize<1) {
    திரும்பு listSize;
  }
    
  /* is there anything to move? */
  எனில்(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  திரும்பு listSize-1;
}




#எனில் U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
கட்டமைப்பு UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
வகைவரை கட்டமைப்பு UPlugLibrary {
  வெறுமை *lib;                           /**< library ptr */
  எழுத்து name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

நிலையான UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_மொத்தம்];
நிலையான UPlugLibrary * libraryList = staticLibraryList;
நிலையான int32_t library_மொத்தம் = 0;
நிலையான int32_t libraryMax = UPLUG_LIBRARY_INITIAL_மொத்தம்;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
நிலையான int32_t தேடல்_For_Library_Name(மாறிலி எழுத்து *libName) {
  int32_t i;
    
  க்கு(i=0;i<library_மொத்தம்;i++) {
    எனில்(!uprv_strcmp(libName, libraryList[i].name)) {
      திரும்பு i;
    }
  }
  திரும்பு -1;
}

நிலையான int32_t தேடல்_For_Library(வெறுமை *lib) {
  int32_t i;
    
  க்கு(i=0;i<library_மொத்தம்;i++) {
    எனில்(lib==libraryList[i].lib) {
      திரும்பு i;
    }
  }
  திரும்பு -1;
}

U_CAPI எழுத்து * U_EXPORT2
uplug_findLibrary(வெறுமை *lib, UErrorCode *status) {
  int32_t libEnt;
  எழுத்து *ret = nullptr;
  எனில்(U_FAILURE(*status)) {
    திரும்பு nullptr;
  }
  libEnt = தேடல்_For_Library(lib);
  எனில்(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } இல்லையெனில் {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  திரும்பு ret;
}

U_CAPI வெறுமை * U_EXPORT2
uplug_openLibrary(மாறிலி எழுத்து *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  வெறுமை *lib = nullptr;
    
  எனில்(U_FAILURE(*status)) திரும்பு nullptr;

  libEntry = தேடல்_For_Library_Name(libName);
  எனில்(libEntry == -1) {
    libEntry = library_மொத்தம்++;
    எனில்(library_மொத்தம் >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#எனில் UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      திரும்பு nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#எனில் UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    எனில்(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#எனில் UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      library_மொத்தம்--;
    } இல்லையெனில் { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } இல்லையெனில் {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  திரும்பு lib;
}

U_CAPI வெறுமை U_EXPORT2
uplug_closeLibrary(வெறுமை *lib, UErrorCode *status) {
  int32_t i;
    
#எனில் UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (வெறுமை*)libraryList));
#endif
  எனில்(U_FAILURE(*status)) திரும்பு;
    
  க்கு(i=0;i<library_மொத்தம்;i++) {
    எனில்(lib==libraryList[i].lib) {
      எனில்(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_மொத்தம் = uplug_removeEntryAt(libraryList, library_மொத்தம், அளவு(*libraryList), i);
      }
      திரும்பு;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

நிலையான UPlugData pluginList[UPLUG_PLUGIN_INITIAL_மொத்தம்];
நிலையான int32_t plugin_மொத்தம் = 0;



  
நிலையான int32_t uplug_plugin_எண்(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[plugin_மொத்தம்];
  எனில்(d<=pluginList) {
    திரும்பு 0;
  } இல்லையெனில் எனில்(d>=pastPlug) {
    திரும்பு plugin_மொத்தம்;
  } இல்லையெனில் {
    திரும்பு (d-pluginList)/அளவு(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  எனில்(prior==nullptr) {
    திரும்பு pluginList;
  } இல்லையெனில் {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[plugin_மொத்தம்];
    
    எனில்(nextPlug>=pastPlug) {
      திரும்பு nullptr;
    } இல்லையெனில் {
      திரும்பு nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
நிலையான வெறுமை uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  எனில்(plug==nullptr||U_FAILURE(*status)) {
    திரும்பு;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  எனில்(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


நிலையான வெறுமை uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  எனில்(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    திரும்பு; 
  }
  எனில்(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

நிலையான வெறுமை uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  எனில்(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    திரும்பு; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  எனில்(U_SUCCESS(*status)) { 
    எனில்(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = பொய்;
    }
  } இல்லையெனில் {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = பொய்;
  }
}


நிலையான வெறுமை uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  எனில்(U_FAILURE(*status)) {
    திரும்பு;
  }
  எனில்(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_கீழ்) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    திரும்பு;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = பொய்;
  எனில்(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

நிலையான UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  எனில்(U_FAILURE(*status)) {
    திரும்பு nullptr;
  }

  எனில்(plugin_மொத்தம் == UPLUG_PLUGIN_INITIAL_மொத்தம்) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    திரும்பு nullptr;
  }

  plug = &pluginList[plugin_மொத்தம்++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = அளவு(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = உண்மை;
  plug->dontUnload = பொய்;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  திரும்பு plug;
}

நிலையான UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, மாறிலி எழுத்து *config, வெறுமை *lib, மாறிலி எழுத்து *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  எனில்(U_FAILURE(*status)) {
    திரும்பு nullptr;
  }

  எனில்(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } இல்லையெனில் {
    plug->config[0] = 0;
  }
    
  எனில்(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } இல்லையெனில் {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  திரும்பு plug;
}

நிலையான வெறுமை uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  எனில்(!plug->dontUnload) {
#எனில் U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  எனில்(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  எனில்(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_மொத்தம் = uplug_removeEntryAt(pluginList, plugin_மொத்தம், அளவு(plug[0]), uplug_plugin_எண்(plug));
  } இல்லையெனில் {
    /* not ok- leave as a message. */
    plug->awaitingLoad=பொய்;
    plug->entrypoint=0;
    plug->dontUnload=உண்மை;
  }
}

நிலையான வெறுமை uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  எனில்(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI வெறுமை U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  எனில்(U_FAILURE(*status)) திரும்பு;
    
  க்கு(cursor=pluginList;cursor!=nullptr;) {
    எனில்(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } இல்லையெனில் {
      cursor = uplug_nextPlug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI வெறுமை U_EXPORT2 
uplug_setPlugNoUnload(UPlugData *data, UBool dontUnload)
{
  data->dontUnload = dontUnload;
}


U_CAPI வெறுமை U_EXPORT2
uplug_setPlugLevel(UPlugData *data, UPlugLevel level) {
  data->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlugData *data) {
  திரும்பு data->level;
}


U_CAPI வெறுமை U_EXPORT2
uplug_setPlugName(UPlugData *data, மாறிலி எழுத்து *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI மாறிலி எழுத்து* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  திரும்பு data->name;
}


U_CAPI மாறிலி எழுத்து* U_EXPORT2
uplug_get_குறியீடு_Name(UPlugData* data U_LIFETIME_BOUND) {
  திரும்பு data->sym;
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  எனில்(data->libName[0]) {
    திரும்பு data->libName;
  } இல்லையெனில் {
#எனில் U_ENABLE_DYLOAD
    திரும்பு uplug_findLibrary(data->lib, status);
#இல்லையெனில்
    திரும்பு nullptr;
#endif
  }
}

U_CAPI வெறுமை * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  திரும்பு data->lib;
}

U_CAPI வெறுமை * U_EXPORT2
uplug_getContext(UPlugData *data) {
  திரும்பு data->context;
}


U_CAPI வெறுமை U_EXPORT2
uplug_setContext(UPlugData *data, வெறுமை *context) {
  data->context = context;
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  திரும்பு data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  எனில்(n <0 || n >= plugin_மொத்தம்) {
    திரும்பு nullptr;
  } இல்லையெனில் { 
    திரும்பு &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  திரும்பு plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
நிலையான UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, மாறிலி எழுத்து *config, வெறுமை *lib, மாறிலி எழுத்து *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  எனில்(U_SUCCESS(*status)) {
    திரும்பு plug;
  } இல்லையெனில் {
    uplug_deallocatePlug(plug, status);
    திரும்பு nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, மாறிலி எழுத்து *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  திரும்பு plug;
}

#எனில் U_ENABLE_DYLOAD

நிலையான UPlugData* 
uplug_initErrorPlug(மாறிலி எழுத்து *libName, மாறிலி எழுத்து *sym, மாறிலி எழுத்து *config, மாறிலி எழுத்து *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  எனில்(U_FAILURE(*status)) திரும்பு nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = பொய்; /* Won't load. */
  plug->dontUnload = உண்மை; /* cannot unload. */

  எனில்(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  எனில்(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  எனில்(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  எனில்(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  திரும்பு plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
நிலையான UPlugData* 
uplug_initPlugFromLibrary(மாறிலி எழுத்து *libName, மாறிலி எழுத்து *sym, மாறிலி எழுத்து *config, UErrorCode *status) {
  வெறுமை *lib = nullptr;
  UPlugData *plug = nullptr;
  எனில்(U_FAILURE(*status)) { திரும்பு nullptr; }
  lib = uplug_openLibrary(libName, status);
  எனில்(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    எனில்(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      எனில்(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } இல்லையெனில் {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    எனில்(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } இல்லையெனில் {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  திரும்பு plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(மாறிலி எழுத்து *libName, மாறிலி எழுத்து *sym, மாறிலி எழுத்து *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  எனில்(U_FAILURE(*status)) { திரும்பு nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  திரும்பு plug;
}

#endif

நிலையான UPlugLevel gCurrentLevel = UPLUG_LEVEL_கீழ்;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  திரும்பு gCurrentLevel;
}

நிலையான UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  க்கு(i=0;i<plugin_மொத்தம்;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_கீழ்;
  திரும்பு உண்மை;
}

#எனில் U_ENABLE_DYLOAD

நிலையான வெறுமை uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  எனில்(U_FAILURE(*status)) {
    திரும்பு;
  }
#எனில் UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  க்கு(i=0;i<plugin_மொத்தம்;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    எனில்(pluginToLoad->awaitingLoad) {
      எனில்(pluginToLoad->level == UPLUG_LEVEL_கீழ்) {
        எனில்(currentLevel > UPLUG_LEVEL_கீழ்) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_உயர்வு;
        } இல்லையெனில் {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          எனில்(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = பொய்;
      } 
    }
  }    
  க்கு(i=0;i<plugin_மொத்தம்;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    எனில்(pluginToLoad->awaitingLoad) {
      எனில்(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } இல்லையெனில் எனில்(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } இல்லையெனில் {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = பொய்;
    }
  }
    
#எனில் UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
நிலையான எழுத்து plugin_file[2048] = "";
#endif

U_CAPI மாறிலி எழுத்து* U_EXPORT2
uplug_getPluginFile() {
#எனில் U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  திரும்பு plugin_file;
#இல்லையெனில்
  திரும்பு nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI வெறுமை U_EXPORT2
uplug_init(UErrorCode *status) {
#எனில் !U_ENABLE_DYLOAD
  (வெறுமை)status; /* unused */
#அல்லதெனில் !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  மாறிலி எழுத்து *env = getenv("ICU_PLUGINS");

  எனில்(U_FAILURE(*status)) திரும்பு;
  எனில்(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  எனில்(U_FAILURE(*status)) திரும்பு;

#எனில் defined(DEFAULT_ICU_PLUGINS) 
  எனில்(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#எனில் UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  எனில்(!plugin_dir.isEmpty()) {
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
#இல்லையெனில்
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#எனில் UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    எனில்(U_FAILURE(*status)) {
      திரும்பு;
    }
    எனில்((size_t)pluginFile.length() > (அளவு(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#எனில் UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      திரும்பு;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, அளவு(plugin_file), *status);

#எனில் UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (முழு)strlen(plugin_file), (முழு)அளவு(plugin_file)));
#endif
        
#ifdef __MVS__
    எனில் (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    இல்லையெனில்
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    எனில்(f != nullptr) {
      எழுத்து linebuf[1024];
      எழுத்து *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t வரி = 0;
            
            
      போது(fgets(linebuf,1023,f)) {
        வரி++;

        எனில்(!*linebuf || *linebuf=='#') {
          தொடர்;
        } இல்லையெனில் {
          p = linebuf;
          போது(*p&&isspace((முழு)*p))
            p++;
          எனில்(!*p || *p=='#') தொடர்;
          libName = p;
          போது(*p&&!isspace((முழு)*p)) {
            p++;
          }
          எனில்(!*p || *p=='#') தொடர்; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          போது(*p&&isspace((முழு)*p)) {
            p++;
          }
          எனில்(!*p||*p=='#') தொடர்; /* no symname after libname +tab */
          symName = p;
          போது(*p&&!isspace((முழு)*p)) {
            p++;
          }
                    
          எனில்(*p) { /* has config */
            *p=0;
            ++p;
            போது(*p&&isspace((முழு)*p)) {
              p++;
            }
            எனில்(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          எனில்(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            போது(p>config&&isspace((முழு)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            எனில்(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#எனில் UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (வெறுமை*)plug, u_errorName(subStatus)));
#இல்லையெனில்
            (வெறுமை)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } இல்லையெனில் {
#எனில் UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_உயர்வு;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


