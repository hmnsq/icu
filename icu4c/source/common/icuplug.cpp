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

#ضمن "unicode/icuplug.h"


#ܐܢ UCONFIG_ENABLE_PLUGINS


#ضمن "icuplugimp.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "putilimp.h"
#ضمن "ucln.h"
#ضمن <stdio.h>
#ifdef __MVS__  /* defined by z/OS compiler */
#define _POSIX_SOURCE
#ضمن <cics.h> /* 12 Nov 2011 JAM iscics() function */
#endif
#ضمن "charstr.h"

using نطاق icu;

#ifndef UPLUG_TRACE
#define UPLUG_TRACE 0
#endif

#ܐܢ UPLUG_TRACE
#ضمن <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

بنية UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  فراغ *lib;              /**< plugin library, or nullptr */
  حرف libName[UPLUG_NAME_MAX];   /**< library name */
  حرف sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  حرف config[UPLUG_NAME_MAX];     /**< configuration data */
  فراغ *context;          /**< user context data */
  حرف name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaitingLoad; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UErrorCode pluginStatus; /**< status code of plugin */
};



#define UPLUG_LIBRARY_INITIAL_COUNT 8
#define UPLUG_PLUGIN_INITIAL_COUNT 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
ساكن int32_t uplug_removeEntryAt(فراغ *list, int32_t listSize, int32_t memberSize, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  ܐܢ(listSize<1) {
    ܦܢܐ listSize;
  }
    
  /* is there anything to move? */
  ܐܢ(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  ܦܢܐ listSize-1;
}




#ܐܢ U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
بنية UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
تعريفنوع بنية UPlugLibrary {
  فراغ *lib;                           /**< library ptr */
  حرف name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

ساكن UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_COUNT];
ساكن UPlugLibrary * libraryList = staticLibraryList;
ساكن int32_t libraryCount = 0;
ساكن int32_t libraryMax = UPLUG_LIBRARY_INITIAL_COUNT;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ساكن int32_t searchForLibraryName(ثابت حرف *libName) {
  int32_t i;
    
  لكل(i=0;i<libraryCount;i++) {
    ܐܢ(!uprv_strcmp(libName, libraryList[i].name)) {
      ܦܢܐ i;
    }
  }
  ܦܢܐ -1;
}

ساكن int32_t searchForLibrary(فراغ *lib) {
  int32_t i;
    
  لكل(i=0;i<libraryCount;i++) {
    ܐܢ(lib==libraryList[i].lib) {
      ܦܢܐ i;
    }
  }
  ܦܢܐ -1;
}

U_CAPI حرف * U_EXPORT2
uplug_findLibrary(فراغ *lib, UErrorCode *status) {
  int32_t libEnt;
  حرف *ret = nullptr;
  ܐܢ(U_FAILURE(*status)) {
    ܦܢܐ nullptr;
  }
  libEnt = searchForLibrary(lib);
  ܐܢ(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } ܐܚܪܢܐ {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  ܦܢܐ ret;
}

U_CAPI فراغ * U_EXPORT2
uplug_openLibrary(ثابت حرف *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  فراغ *lib = nullptr;
    
  ܐܢ(U_FAILURE(*status)) ܦܢܐ nullptr;

  libEntry = searchForLibraryName(libName);
  ܐܢ(libEntry == -1) {
    libEntry = libraryCount++;
    ܐܢ(libraryCount >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#ܐܢ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      ܦܢܐ nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#ܐܢ UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    ܐܢ(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#ܐܢ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      libraryCount--;
    } ܐܚܪܢܐ { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } ܐܚܪܢܐ {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  ܦܢܐ lib;
}

U_CAPI فراغ U_EXPORT2
uplug_closeLibrary(فراغ *lib, UErrorCode *status) {
  int32_t i;
    
#ܐܢ UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (فراغ*)libraryList));
#endif
  ܐܢ(U_FAILURE(*status)) ܦܢܐ;
    
  لكل(i=0;i<libraryCount;i++) {
    ܐܢ(lib==libraryList[i].lib) {
      ܐܢ(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        libraryCount = uplug_removeEntryAt(libraryList, libraryCount, حجم(*libraryList), i);
      }
      ܦܢܐ;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

ساكن UPlugData pluginList[UPLUG_PLUGIN_INITIAL_COUNT];
ساكن int32_t pluginCount = 0;



  
ساكن int32_t uplug_pluginNumber(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[pluginCount];
  ܐܢ(d<=pluginList) {
    ܦܢܐ 0;
  } ܐܚܪܢܐ ܐܢ(d>=pastPlug) {
    ܦܢܐ pluginCount;
  } ܐܚܪܢܐ {
    ܦܢܐ (d-pluginList)/حجم(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  ܐܢ(prior==nullptr) {
    ܦܢܐ pluginList;
  } ܐܚܪܢܐ {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[pluginCount];
    
    ܐܢ(nextPlug>=pastPlug) {
      ܦܢܐ nullptr;
    } ܐܚܪܢܐ {
      ܦܢܐ nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ساكن فراغ uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  ܐܢ(plug==nullptr||U_FAILURE(*status)) {
    ܦܢܐ;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  ܐܢ(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


ساكن فراغ uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  ܐܢ(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ܦܢܐ; 
  }
  ܐܢ(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

ساكن فراغ uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  ܐܢ(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ܦܢܐ; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  ܐܢ(U_SUCCESS(*status)) { 
    ܐܢ(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = ܕܓܠܐ;
    }
  } ܐܚܪܢܐ {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = ܕܓܠܐ;
  }
}


ساكن فراغ uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  ܐܢ(U_FAILURE(*status)) {
    ܦܢܐ;
  }
  ܐܢ(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_LOW) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ܦܢܐ;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = ܕܓܠܐ;
  ܐܢ(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

ساكن UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  ܐܢ(U_FAILURE(*status)) {
    ܦܢܐ nullptr;
  }

  ܐܢ(pluginCount == UPLUG_PLUGIN_INITIAL_COUNT) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ܦܢܐ nullptr;
  }

  plug = &pluginList[pluginCount++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = حجم(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = ܫܪܪܐ;
  plug->dontUnload = ܕܓܠܐ;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  ܦܢܐ plug;
}

ساكن UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  ܐܢ(U_FAILURE(*status)) {
    ܦܢܐ nullptr;
  }

  ܐܢ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } ܐܚܪܢܐ {
    plug->config[0] = 0;
  }
    
  ܐܢ(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } ܐܚܪܢܐ {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  ܦܢܐ plug;
}

ساكن فراغ uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  ܐܢ(!plug->dontUnload) {
#ܐܢ U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  ܐܢ(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  ܐܢ(U_SUCCESS(*status)) {
    /* all ok- remove. */
    pluginCount = uplug_removeEntryAt(pluginList, pluginCount, حجم(plug[0]), uplug_pluginNumber(plug));
  } ܐܚܪܢܐ {
    /* not ok- leave as a message. */
    plug->awaitingLoad=ܕܓܠܐ;
    plug->entrypoint=0;
    plug->dontUnload=ܫܪܪܐ;
  }
}

ساكن فراغ uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  ܐܢ(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI فراغ U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  ܐܢ(U_FAILURE(*status)) ܦܢܐ;
    
  لكل(cursor=pluginList;cursor!=nullptr;) {
    ܐܢ(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } ܐܚܪܢܐ {
      cursor = uplug_nextPlug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI فراغ U_EXPORT2 
uplug_setPlugNoUnload(UPlugData *data, UBool dontUnload)
{
  data->dontUnload = dontUnload;
}


U_CAPI فراغ U_EXPORT2
uplug_setPlugLevel(UPlugData *data, UPlugLevel level) {
  data->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlugData *data) {
  ܦܢܐ data->level;
}


U_CAPI فراغ U_EXPORT2
uplug_setPlugName(UPlugData *data, ثابت حرف *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  ܦܢܐ data->name;
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_getSymbolName(UPlugData* data U_LIFETIME_BOUND) {
  ܦܢܐ data->sym;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  ܐܢ(data->libName[0]) {
    ܦܢܐ data->libName;
  } ܐܚܪܢܐ {
#ܐܢ U_ENABLE_DYLOAD
    ܦܢܐ uplug_findLibrary(data->lib, status);
#ܐܚܪܢܐ
    ܦܢܐ nullptr;
#endif
  }
}

U_CAPI فراغ * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  ܦܢܐ data->lib;
}

U_CAPI فراغ * U_EXPORT2
uplug_getContext(UPlugData *data) {
  ܦܢܐ data->context;
}


U_CAPI فراغ U_EXPORT2
uplug_setContext(UPlugData *data, فراغ *context) {
  data->context = context;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  ܦܢܐ data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  ܐܢ(n <0 || n >= pluginCount) {
    ܦܢܐ nullptr;
  } ܐܚܪܢܐ { 
    ܦܢܐ &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  ܦܢܐ plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ساكن UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  ܐܢ(U_SUCCESS(*status)) {
    ܦܢܐ plug;
  } ܐܚܪܢܐ {
    uplug_deallocatePlug(plug, status);
    ܦܢܐ nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, ثابت حرف *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  ܦܢܐ plug;
}

#ܐܢ U_ENABLE_DYLOAD

ساكن UPlugData* 
uplug_initErrorPlug(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, ثابت حرف *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  ܐܢ(U_FAILURE(*status)) ܦܢܐ nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = ܕܓܠܐ; /* Won't load. */
  plug->dontUnload = ܫܪܪܐ; /* cannot unload. */

  ܐܢ(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  ܐܢ(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  ܐܢ(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  ܐܢ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  ܦܢܐ plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ساكن UPlugData* 
uplug_initPlugFromLibrary(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UErrorCode *status) {
  فراغ *lib = nullptr;
  UPlugData *plug = nullptr;
  ܐܢ(U_FAILURE(*status)) { ܦܢܐ nullptr; }
  lib = uplug_openLibrary(libName, status);
  ܐܢ(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    ܐܢ(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      ܐܢ(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } ܐܚܪܢܐ {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    ܐܢ(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } ܐܚܪܢܐ {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  ܦܢܐ plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  ܐܢ(U_FAILURE(*status)) { ܦܢܐ nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  ܦܢܐ plug;
}

#endif

ساكن UPlugLevel gCurrentLevel = UPLUG_LEVEL_LOW;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  ܦܢܐ gCurrentLevel;
}

ساكن UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  لكل(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_LOW;
  ܦܢܐ ܫܪܪܐ;
}

#ܐܢ U_ENABLE_DYLOAD

ساكن فراغ uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  ܐܢ(U_FAILURE(*status)) {
    ܦܢܐ;
  }
#ܐܢ UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  لكل(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    ܐܢ(pluginToLoad->awaitingLoad) {
      ܐܢ(pluginToLoad->level == UPLUG_LEVEL_LOW) {
        ܐܢ(currentLevel > UPLUG_LEVEL_LOW) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_HIGH;
        } ܐܚܪܢܐ {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          ܐܢ(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = ܕܓܠܐ;
      } 
    }
  }    
  لكل(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    ܐܢ(pluginToLoad->awaitingLoad) {
      ܐܢ(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } ܐܚܪܢܐ ܐܢ(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } ܐܚܪܢܐ {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = ܕܓܠܐ;
    }
  }
    
#ܐܢ UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
ساكن حرف plugin_file[2048] = "";
#endif

U_CAPI ثابت حرف* U_EXPORT2
uplug_getPluginFile() {
#ܐܢ U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  ܦܢܐ plugin_file;
#ܐܚܪܢܐ
  ܦܢܐ nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI فراغ U_EXPORT2
uplug_init(UErrorCode *status) {
#ܐܢ !U_ENABLE_DYLOAD
  (فراغ)status; /* unused */
#وإلاإذا !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ثابت حرف *env = getenv("ICU_PLUGINS");

  ܐܢ(U_FAILURE(*status)) ܦܢܐ;
  ܐܢ(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  ܐܢ(U_FAILURE(*status)) ܦܢܐ;

#ܐܢ defined(DEFAULT_ICU_PLUGINS) 
  ܐܢ(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#ܐܢ UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  ܐܢ(!plugin_dir.isEmpty()) {
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
#ܐܚܪܢܐ
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#ܐܢ UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    ܐܢ(U_FAILURE(*status)) {
      ܦܢܐ;
    }
    ܐܢ((size_t)pluginFile.length() > (حجم(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#ܐܢ UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      ܦܢܐ;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, حجم(plugin_file), *status);

#ܐܢ UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (صحيح)strlen(plugin_file), (صحيح)حجم(plugin_file)));
#endif
        
#ifdef __MVS__
    ܐܢ (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    ܐܚܪܢܐ
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    ܐܢ(f != nullptr) {
      حرف linebuf[1024];
      حرف *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t line = 0;
            
            
      ܟܡܐ(fgets(linebuf,1023,f)) {
        line++;

        ܐܢ(!*linebuf || *linebuf=='#') {
          استمر;
        } ܐܚܪܢܐ {
          p = linebuf;
          ܟܡܐ(*p&&isspace((صحيح)*p))
            p++;
          ܐܢ(!*p || *p=='#') استمر;
          libName = p;
          ܟܡܐ(*p&&!isspace((صحيح)*p)) {
            p++;
          }
          ܐܢ(!*p || *p=='#') استمر; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          ܟܡܐ(*p&&isspace((صحيح)*p)) {
            p++;
          }
          ܐܢ(!*p||*p=='#') استمر; /* no symname after libname +tab */
          symName = p;
          ܟܡܐ(*p&&!isspace((صحيح)*p)) {
            p++;
          }
                    
          ܐܢ(*p) { /* has config */
            *p=0;
            ++p;
            ܟܡܐ(*p&&isspace((صحيح)*p)) {
              p++;
            }
            ܐܢ(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          ܐܢ(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            ܟܡܐ(p>config&&isspace((صحيح)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            ܐܢ(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#ܐܢ UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (فراغ*)plug, u_errorName(subStatus)));
#ܐܚܪܢܐ
            (فراغ)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } ܐܚܪܢܐ {
#ܐܢ UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_HIGH;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


