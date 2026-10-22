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

#include "unicode/icuplug.h"


#ᰀᰧ UCONFIG_ENABLE_PLUGINS


#include "icuplugimp.h"
#include "cstring.h"
#include "cmemory.h"
#include "putilimp.h"
#include "ucln.h"
#include <stdio.h>
#ifdef __MVS__  /* defined by z/OS compiler */
#define _POSIX_SOURCE
#include <cics.h> /* 12 Nov 2011 JAM iscics() function */
#endif
#include "charstr.h"

using namespace icu;

#ifndef UPLUG_TRACE
#define UPLUG_TRACE 0
#endif

#ᰀᰧ UPLUG_TRACE
#include <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

sa_nracanaa UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  void *lib;              /**< plugin library, or nullptr */
  var_na libName[UPLUG_NAME_MAX];   /**< library name */
  var_na sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  var_na config[UPLUG_NAME_MAX];     /**< configuration data */
  void *context;          /**< user context data */
  var_na name[UPLUG_NAME_MAX];   /**< name of plugin */
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
static int32_t uplug_removeEntryAt(void *list, int32_t listSize, int32_t memberSize, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  ᰀᰧ(listSize<1) {
    ᰀᰧᰌ listSize;
  }
    
  /* is there anything to move? */
  ᰀᰧ(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  ᰀᰧᰌ listSize-1;
}




#ᰀᰧ U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
sa_nracanaa UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
typedef sa_nracanaa UPlugLibrary {
  void *lib;                           /**< library ptr */
  var_na name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

static UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_COUNT];
static UPlugLibrary * libraryList = staticLibraryList;
static int32_t libraryCount = 0;
static int32_t libraryMax = UPLUG_LIBRARY_INITIAL_COUNT;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
static int32_t searchForLibraryName(const var_na *libName) {
  int32_t i;
    
  for(i=0;i<libraryCount;i++) {
    ᰀᰧ(!uprv_strcmp(libName, libraryList[i].name)) {
      ᰀᰧᰌ i;
    }
  }
  ᰀᰧᰌ -1;
}

static int32_t searchForLibrary(void *lib) {
  int32_t i;
    
  for(i=0;i<libraryCount;i++) {
    ᰀᰧ(lib==libraryList[i].lib) {
      ᰀᰧᰌ i;
    }
  }
  ᰀᰧᰌ -1;
}

U_CAPI var_na * U_EXPORT2
uplug_findLibrary(void *lib, UErrorCode *status) {
  int32_t libEnt;
  var_na *ret = nullptr;
  ᰀᰧ(U_FAILURE(*status)) {
    ᰀᰧᰌ nullptr;
  }
  libEnt = searchForLibrary(lib);
  ᰀᰧ(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } ᰀᰧᰀ {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  ᰀᰧᰌ ret;
}

U_CAPI void * U_EXPORT2
uplug_openLibrary(const var_na *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  void *lib = nullptr;
    
  ᰀᰧ(U_FAILURE(*status)) ᰀᰧᰌ nullptr;

  libEntry = searchForLibraryName(libName);
  ᰀᰧ(libEntry == -1) {
    libEntry = libraryCount++;
    ᰀᰧ(libraryCount >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#ᰀᰧ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      ᰀᰧᰌ nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#ᰀᰧ UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    ᰀᰧ(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#ᰀᰧ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      libraryCount--;
    } ᰀᰧᰀ { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } ᰀᰧᰀ {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  ᰀᰧᰌ lib;
}

U_CAPI void U_EXPORT2
uplug_closeLibrary(void *lib, UErrorCode *status) {
  int32_t i;
    
#ᰀᰧ UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (void*)libraryList));
#endif
  ᰀᰧ(U_FAILURE(*status)) ᰀᰧᰌ;
    
  for(i=0;i<libraryCount;i++) {
    ᰀᰧ(lib==libraryList[i].lib) {
      ᰀᰧ(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        libraryCount = uplug_removeEntryAt(libraryList, libraryCount, parimaa_na(*libraryList), i);
      }
      ᰀᰧᰌ;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

static UPlugData pluginList[UPLUG_PLUGIN_INITIAL_COUNT];
static int32_t pluginCount = 0;



  
static int32_t uplug_pluginNumber(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[pluginCount];
  ᰀᰧ(d<=pluginList) {
    ᰀᰧᰌ 0;
  } ᰀᰧᰀ ᰀᰧ(d>=pastPlug) {
    ᰀᰧᰌ pluginCount;
  } ᰀᰧᰀ {
    ᰀᰧᰌ (d-pluginList)/parimaa_na(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  ᰀᰧ(prior==nullptr) {
    ᰀᰧᰌ pluginList;
  } ᰀᰧᰀ {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[pluginCount];
    
    ᰀᰧ(nextPlug>=pastPlug) {
      ᰀᰧᰌ nullptr;
    } ᰀᰧᰀ {
      ᰀᰧᰌ nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
static void uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  ᰀᰧ(plug==nullptr||U_FAILURE(*status)) {
    ᰀᰧᰌ;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  ᰀᰧ(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


static void uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  ᰀᰧ(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ᰀᰧᰌ; 
  }
  ᰀᰧ(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

static void uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  ᰀᰧ(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ᰀᰧᰌ; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  ᰀᰧ(U_SUCCESS(*status)) { 
    ᰀᰧ(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = false;
    }
  } ᰀᰧᰀ {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = false;
  }
}


static void uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  ᰀᰧ(U_FAILURE(*status)) {
    ᰀᰧᰌ;
  }
  ᰀᰧ(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_LOW) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ᰀᰧᰌ;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = false;
  ᰀᰧ(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

static UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  ᰀᰧ(U_FAILURE(*status)) {
    ᰀᰧᰌ nullptr;
  }

  ᰀᰧ(pluginCount == UPLUG_PLUGIN_INITIAL_COUNT) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ᰀᰧᰌ nullptr;
  }

  plug = &pluginList[pluginCount++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = parimaa_na(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = true;
  plug->dontUnload = false;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  ᰀᰧᰌ plug;
}

static UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, const var_na *config, void *lib, const var_na *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  ᰀᰧ(U_FAILURE(*status)) {
    ᰀᰧᰌ nullptr;
  }

  ᰀᰧ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } ᰀᰧᰀ {
    plug->config[0] = 0;
  }
    
  ᰀᰧ(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } ᰀᰧᰀ {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  ᰀᰧᰌ plug;
}

static void uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  ᰀᰧ(!plug->dontUnload) {
#ᰀᰧ U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  ᰀᰧ(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  ᰀᰧ(U_SUCCESS(*status)) {
    /* all ok- remove. */
    pluginCount = uplug_removeEntryAt(pluginList, pluginCount, parimaa_na(plug[0]), uplug_pluginNumber(plug));
  } ᰀᰧᰀ {
    /* not ok- leave as a message. */
    plug->awaitingLoad=false;
    plug->entrypoint=0;
    plug->dontUnload=true;
  }
}

static void uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  ᰀᰧ(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI void U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  ᰀᰧ(U_FAILURE(*status)) ᰀᰧᰌ;
    
  for(cursor=pluginList;cursor!=nullptr;) {
    ᰀᰧ(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } ᰀᰧᰀ {
      cursor = uplug_nextPlug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI void U_EXPORT2 
uplug_setPlugNoUnload(UPlugData *data, UBool dontUnload)
{
  data->dontUnload = dontUnload;
}


U_CAPI void U_EXPORT2
uplug_setPlugLevel(UPlugData *data, UPlugLevel level) {
  data->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlugData *data) {
  ᰀᰧᰌ data->level;
}


U_CAPI void U_EXPORT2
uplug_setPlugName(UPlugData *data, const var_na *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI const var_na* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  ᰀᰧᰌ data->name;
}


U_CAPI const var_na* U_EXPORT2
uplug_getSymbolName(UPlugData* data U_LIFETIME_BOUND) {
  ᰀᰧᰌ data->sym;
}

U_CAPI const var_na* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  ᰀᰧ(data->libName[0]) {
    ᰀᰧᰌ data->libName;
  } ᰀᰧᰀ {
#ᰀᰧ U_ENABLE_DYLOAD
    ᰀᰧᰌ uplug_findLibrary(data->lib, status);
#ᰀᰧᰀ
    ᰀᰧᰌ nullptr;
#endif
  }
}

U_CAPI void * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  ᰀᰧᰌ data->lib;
}

U_CAPI void * U_EXPORT2
uplug_getContext(UPlugData *data) {
  ᰀᰧᰌ data->context;
}


U_CAPI void U_EXPORT2
uplug_setContext(UPlugData *data, void *context) {
  data->context = context;
}

U_CAPI const var_na* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  ᰀᰧᰌ data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  ᰀᰧ(n <0 || n >= pluginCount) {
    ᰀᰧᰌ nullptr;
  } ᰀᰧᰀ { 
    ᰀᰧᰌ &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  ᰀᰧᰌ plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
static UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, const var_na *config, void *lib, const var_na *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  ᰀᰧ(U_SUCCESS(*status)) {
    ᰀᰧᰌ plug;
  } ᰀᰧᰀ {
    uplug_deallocatePlug(plug, status);
    ᰀᰧᰌ nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, const var_na *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  ᰀᰧᰌ plug;
}

#ᰀᰧ U_ENABLE_DYLOAD

static UPlugData* 
uplug_initErrorPlug(const var_na *libName, const var_na *sym, const var_na *config, const var_na *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  ᰀᰧ(U_FAILURE(*status)) ᰀᰧᰌ nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = false; /* Won't load. */
  plug->dontUnload = true; /* cannot unload. */

  ᰀᰧ(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  ᰀᰧ(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  ᰀᰧ(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  ᰀᰧ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  ᰀᰧᰌ plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
static UPlugData* 
uplug_initPlugFromLibrary(const var_na *libName, const var_na *sym, const var_na *config, UErrorCode *status) {
  void *lib = nullptr;
  UPlugData *plug = nullptr;
  ᰀᰧ(U_FAILURE(*status)) { ᰀᰧᰌ nullptr; }
  lib = uplug_openLibrary(libName, status);
  ᰀᰧ(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    ᰀᰧ(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      ᰀᰧ(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } ᰀᰧᰀ {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    ᰀᰧ(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } ᰀᰧᰀ {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  ᰀᰧᰌ plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(const var_na *libName, const var_na *sym, const var_na *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  ᰀᰧ(U_FAILURE(*status)) { ᰀᰧᰌ nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  ᰀᰧᰌ plug;
}

#endif

static UPlugLevel gCurrentLevel = UPLUG_LEVEL_LOW;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  ᰀᰧᰌ gCurrentLevel;
}

static UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  for(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_LOW;
  ᰀᰧᰌ true;
}

#ᰀᰧ U_ENABLE_DYLOAD

static void uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  ᰀᰧ(U_FAILURE(*status)) {
    ᰀᰧᰌ;
  }
#ᰀᰧ UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  for(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    ᰀᰧ(pluginToLoad->awaitingLoad) {
      ᰀᰧ(pluginToLoad->level == UPLUG_LEVEL_LOW) {
        ᰀᰧ(currentLevel > UPLUG_LEVEL_LOW) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_HIGH;
        } ᰀᰧᰀ {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          ᰀᰧ(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = false;
      } 
    }
  }    
  for(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    ᰀᰧ(pluginToLoad->awaitingLoad) {
      ᰀᰧ(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } ᰀᰧᰀ ᰀᰧ(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } ᰀᰧᰀ {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = false;
    }
  }
    
#ᰀᰧ UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
static var_na plugin_file[2048] = "";
#endif

U_CAPI const var_na* U_EXPORT2
uplug_getPluginFile() {
#ᰀᰧ U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  ᰀᰧᰌ plugin_file;
#ᰀᰧᰀ
  ᰀᰧᰌ nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI void U_EXPORT2
uplug_init(UErrorCode *status) {
#ᰀᰧ !U_ENABLE_DYLOAD
  (void)status; /* unused */
#elif !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  const var_na *env = getenv("ICU_PLUGINS");

  ᰀᰧ(U_FAILURE(*status)) ᰀᰧᰌ;
  ᰀᰧ(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  ᰀᰧ(U_FAILURE(*status)) ᰀᰧᰌ;

#ᰀᰧ defined(DEFAULT_ICU_PLUGINS) 
  ᰀᰧ(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#ᰀᰧ UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  ᰀᰧ(!plugin_dir.isEmpty()) {
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
#ᰀᰧᰀ
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#ᰀᰧ UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    ᰀᰧ(U_FAILURE(*status)) {
      ᰀᰧᰌ;
    }
    ᰀᰧ((size_t)pluginFile.length() > (parimaa_na(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#ᰀᰧ UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      ᰀᰧᰌ;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, parimaa_na(plugin_file), *status);

#ᰀᰧ UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (puur_naa_nka)strlen(plugin_file), (puur_naa_nka)parimaa_na(plugin_file)));
#endif
        
#ifdef __MVS__
    ᰀᰧ (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    ᰀᰧᰀ
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    ᰀᰧ(f != nullptr) {
      var_na linebuf[1024];
      var_na *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t line = 0;
            
            
      ᰀᰬ(fgets(linebuf,1023,f)) {
        line++;

        ᰀᰧ(!*linebuf || *linebuf=='#') {
          continue;
        } ᰀᰧᰀ {
          p = linebuf;
          ᰀᰬ(*p&&isspace((puur_naa_nka)*p))
            p++;
          ᰀᰧ(!*p || *p=='#') continue;
          libName = p;
          ᰀᰬ(*p&&!isspace((puur_naa_nka)*p)) {
            p++;
          }
          ᰀᰧ(!*p || *p=='#') continue; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          ᰀᰬ(*p&&isspace((puur_naa_nka)*p)) {
            p++;
          }
          ᰀᰧ(!*p||*p=='#') continue; /* no symname after libname +tab */
          symName = p;
          ᰀᰬ(*p&&!isspace((puur_naa_nka)*p)) {
            p++;
          }
                    
          ᰀᰧ(*p) { /* has config */
            *p=0;
            ++p;
            ᰀᰬ(*p&&isspace((puur_naa_nka)*p)) {
              p++;
            }
            ᰀᰧ(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          ᰀᰧ(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            ᰀᰬ(p>config&&isspace((puur_naa_nka)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            ᰀᰧ(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#ᰀᰧ UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (void*)plug, u_errorName(subStatus)));
#ᰀᰧᰀ
            (void)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } ᰀᰧᰀ {
#ᰀᰧ UPLUG_TRACE
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


