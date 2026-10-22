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


#ຖ້າ UCONFIG_ENABLE_PLUGINS


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

#ຖ້າ UPLUG_TRACE
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
  ຖ້າ(listSize<1) {
    ສົ່ງຄືນ listSize;
  }
    
  /* is there anything to move? */
  ຖ້າ(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  ສົ່ງຄືນ listSize-1;
}




#ຖ້າ U_ENABLE_DYLOAD
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
    
  ສຳລັບ(i=0;i<libraryCount;i++) {
    ຖ້າ(!uprv_strcmp(libName, libraryList[i].name)) {
      ສົ່ງຄືນ i;
    }
  }
  ສົ່ງຄືນ -1;
}

static int32_t searchForLibrary(void *lib) {
  int32_t i;
    
  ສຳລັບ(i=0;i<libraryCount;i++) {
    ຖ້າ(lib==libraryList[i].lib) {
      ສົ່ງຄືນ i;
    }
  }
  ສົ່ງຄືນ -1;
}

U_CAPI var_na * U_EXPORT2
uplug_findLibrary(void *lib, UErrorCode *status) {
  int32_t libEnt;
  var_na *ret = nullptr;
  ຖ້າ(U_FAILURE(*status)) {
    ສົ່ງຄືນ nullptr;
  }
  libEnt = searchForLibrary(lib);
  ຖ້າ(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } ບໍ່ດັ່ງນັ້ນ {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  ສົ່ງຄືນ ret;
}

U_CAPI void * U_EXPORT2
uplug_openLibrary(const var_na *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  void *lib = nullptr;
    
  ຖ້າ(U_FAILURE(*status)) ສົ່ງຄືນ nullptr;

  libEntry = searchForLibraryName(libName);
  ຖ້າ(libEntry == -1) {
    libEntry = libraryCount++;
    ຖ້າ(libraryCount >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#ຖ້າ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      ສົ່ງຄືນ nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#ຖ້າ UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    ຖ້າ(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#ຖ້າ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      libraryCount--;
    } ບໍ່ດັ່ງນັ້ນ { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } ບໍ່ດັ່ງນັ້ນ {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  ສົ່ງຄືນ lib;
}

U_CAPI void U_EXPORT2
uplug_closeLibrary(void *lib, UErrorCode *status) {
  int32_t i;
    
#ຖ້າ UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (void*)libraryList));
#endif
  ຖ້າ(U_FAILURE(*status)) ສົ່ງຄືນ;
    
  ສຳລັບ(i=0;i<libraryCount;i++) {
    ຖ້າ(lib==libraryList[i].lib) {
      ຖ້າ(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        libraryCount = uplug_removeEntryAt(libraryList, libraryCount, parimaa_na(*libraryList), i);
      }
      ສົ່ງຄືນ;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

static UPlugData pluginList[UPLUG_PLUGIN_INITIAL_COUNT];
static int32_t pluginCount = 0;



  
static int32_t uplug_pluginNumber(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[pluginCount];
  ຖ້າ(d<=pluginList) {
    ສົ່ງຄືນ 0;
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ(d>=pastPlug) {
    ສົ່ງຄືນ pluginCount;
  } ບໍ່ດັ່ງນັ້ນ {
    ສົ່ງຄືນ (d-pluginList)/parimaa_na(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  ຖ້າ(prior==nullptr) {
    ສົ່ງຄືນ pluginList;
  } ບໍ່ດັ່ງນັ້ນ {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[pluginCount];
    
    ຖ້າ(nextPlug>=pastPlug) {
      ສົ່ງຄືນ nullptr;
    } ບໍ່ດັ່ງນັ້ນ {
      ສົ່ງຄືນ nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
static void uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  ຖ້າ(plug==nullptr||U_FAILURE(*status)) {
    ສົ່ງຄືນ;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  ຖ້າ(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


static void uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  ຖ້າ(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ສົ່ງຄືນ; 
  }
  ຖ້າ(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

static void uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  ຖ້າ(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ສົ່ງຄືນ; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  ຖ້າ(U_SUCCESS(*status)) { 
    ຖ້າ(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = false;
    }
  } ບໍ່ດັ່ງນັ້ນ {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = false;
  }
}


static void uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  ຖ້າ(U_FAILURE(*status)) {
    ສົ່ງຄືນ;
  }
  ຖ້າ(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_LOW) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ສົ່ງຄືນ;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = false;
  ຖ້າ(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

static UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  ຖ້າ(U_FAILURE(*status)) {
    ສົ່ງຄືນ nullptr;
  }

  ຖ້າ(pluginCount == UPLUG_PLUGIN_INITIAL_COUNT) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ສົ່ງຄືນ nullptr;
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


  ສົ່ງຄືນ plug;
}

static UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, const var_na *config, void *lib, const var_na *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  ຖ້າ(U_FAILURE(*status)) {
    ສົ່ງຄືນ nullptr;
  }

  ຖ້າ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } ບໍ່ດັ່ງນັ້ນ {
    plug->config[0] = 0;
  }
    
  ຖ້າ(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } ບໍ່ດັ່ງນັ້ນ {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  ສົ່ງຄືນ plug;
}

static void uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  ຖ້າ(!plug->dontUnload) {
#ຖ້າ U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  ຖ້າ(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  ຖ້າ(U_SUCCESS(*status)) {
    /* all ok- remove. */
    pluginCount = uplug_removeEntryAt(pluginList, pluginCount, parimaa_na(plug[0]), uplug_pluginNumber(plug));
  } ບໍ່ດັ່ງນັ້ນ {
    /* not ok- leave as a message. */
    plug->awaitingLoad=false;
    plug->entrypoint=0;
    plug->dontUnload=true;
  }
}

static void uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  ຖ້າ(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI void U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  ຖ້າ(U_FAILURE(*status)) ສົ່ງຄືນ;
    
  ສຳລັບ(cursor=pluginList;cursor!=nullptr;) {
    ຖ້າ(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } ບໍ່ດັ່ງນັ້ນ {
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
  ສົ່ງຄືນ data->level;
}


U_CAPI void U_EXPORT2
uplug_setPlugName(UPlugData *data, const var_na *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI const var_na* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  ສົ່ງຄືນ data->name;
}


U_CAPI const var_na* U_EXPORT2
uplug_getSymbolName(UPlugData* data U_LIFETIME_BOUND) {
  ສົ່ງຄືນ data->sym;
}

U_CAPI const var_na* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  ຖ້າ(data->libName[0]) {
    ສົ່ງຄືນ data->libName;
  } ບໍ່ດັ່ງນັ້ນ {
#ຖ້າ U_ENABLE_DYLOAD
    ສົ່ງຄືນ uplug_findLibrary(data->lib, status);
#ບໍ່ດັ່ງນັ້ນ
    ສົ່ງຄືນ nullptr;
#endif
  }
}

U_CAPI void * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  ສົ່ງຄືນ data->lib;
}

U_CAPI void * U_EXPORT2
uplug_getContext(UPlugData *data) {
  ສົ່ງຄືນ data->context;
}


U_CAPI void U_EXPORT2
uplug_setContext(UPlugData *data, void *context) {
  data->context = context;
}

U_CAPI const var_na* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  ສົ່ງຄືນ data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  ຖ້າ(n <0 || n >= pluginCount) {
    ສົ່ງຄືນ nullptr;
  } ບໍ່ດັ່ງນັ້ນ { 
    ສົ່ງຄືນ &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  ສົ່ງຄືນ plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
static UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, const var_na *config, void *lib, const var_na *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  ຖ້າ(U_SUCCESS(*status)) {
    ສົ່ງຄືນ plug;
  } ບໍ່ດັ່ງນັ້ນ {
    uplug_deallocatePlug(plug, status);
    ສົ່ງຄືນ nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, const var_na *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  ສົ່ງຄືນ plug;
}

#ຖ້າ U_ENABLE_DYLOAD

static UPlugData* 
uplug_initErrorPlug(const var_na *libName, const var_na *sym, const var_na *config, const var_na *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  ຖ້າ(U_FAILURE(*status)) ສົ່ງຄືນ nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = false; /* Won't load. */
  plug->dontUnload = true; /* cannot unload. */

  ຖ້າ(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  ຖ້າ(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  ຖ້າ(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  ຖ້າ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  ສົ່ງຄືນ plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
static UPlugData* 
uplug_initPlugFromLibrary(const var_na *libName, const var_na *sym, const var_na *config, UErrorCode *status) {
  void *lib = nullptr;
  UPlugData *plug = nullptr;
  ຖ້າ(U_FAILURE(*status)) { ສົ່ງຄືນ nullptr; }
  lib = uplug_openLibrary(libName, status);
  ຖ້າ(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    ຖ້າ(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      ຖ້າ(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } ບໍ່ດັ່ງນັ້ນ {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    ຖ້າ(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } ບໍ່ດັ່ງນັ້ນ {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  ສົ່ງຄືນ plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(const var_na *libName, const var_na *sym, const var_na *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  ຖ້າ(U_FAILURE(*status)) { ສົ່ງຄືນ nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  ສົ່ງຄືນ plug;
}

#endif

static UPlugLevel gCurrentLevel = UPLUG_LEVEL_LOW;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  ສົ່ງຄືນ gCurrentLevel;
}

static UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  ສຳລັບ(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_LOW;
  ສົ່ງຄືນ true;
}

#ຖ້າ U_ENABLE_DYLOAD

static void uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  ຖ້າ(U_FAILURE(*status)) {
    ສົ່ງຄືນ;
  }
#ຖ້າ UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  ສຳລັບ(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    ຖ້າ(pluginToLoad->awaitingLoad) {
      ຖ້າ(pluginToLoad->level == UPLUG_LEVEL_LOW) {
        ຖ້າ(currentLevel > UPLUG_LEVEL_LOW) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_HIGH;
        } ບໍ່ດັ່ງນັ້ນ {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          ຖ້າ(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = false;
      } 
    }
  }    
  ສຳລັບ(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    ຖ້າ(pluginToLoad->awaitingLoad) {
      ຖ້າ(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } ບໍ່ດັ່ງນັ້ນ {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = false;
    }
  }
    
#ຖ້າ UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
static var_na plugin_file[2048] = "";
#endif

U_CAPI const var_na* U_EXPORT2
uplug_getPluginFile() {
#ຖ້າ U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  ສົ່ງຄືນ plugin_file;
#ບໍ່ດັ່ງນັ້ນ
  ສົ່ງຄືນ nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI void U_EXPORT2
uplug_init(UErrorCode *status) {
#ຖ້າ !U_ENABLE_DYLOAD
  (void)status; /* unused */
#elif !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  const var_na *env = getenv("ICU_PLUGINS");

  ຖ້າ(U_FAILURE(*status)) ສົ່ງຄືນ;
  ຖ້າ(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  ຖ້າ(U_FAILURE(*status)) ສົ່ງຄືນ;

#ຖ້າ defined(DEFAULT_ICU_PLUGINS) 
  ຖ້າ(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#ຖ້າ UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  ຖ້າ(!plugin_dir.isEmpty()) {
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
#ບໍ່ດັ່ງນັ້ນ
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#ຖ້າ UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    ຖ້າ(U_FAILURE(*status)) {
      ສົ່ງຄືນ;
    }
    ຖ້າ((size_t)pluginFile.length() > (parimaa_na(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#ຖ້າ UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      ສົ່ງຄືນ;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, parimaa_na(plugin_file), *status);

#ຖ້າ UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (ຈຳນວນເຕັມ)strlen(plugin_file), (ຈຳນວນເຕັມ)parimaa_na(plugin_file)));
#endif
        
#ifdef __MVS__
    ຖ້າ (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    ບໍ່ດັ່ງນັ້ນ
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    ຖ້າ(f != nullptr) {
      var_na linebuf[1024];
      var_na *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t line = 0;
            
            
      ໃນຂະນະທີ່(fgets(linebuf,1023,f)) {
        line++;

        ຖ້າ(!*linebuf || *linebuf=='#') {
          continue;
        } ບໍ່ດັ່ງນັ້ນ {
          p = linebuf;
          ໃນຂະນະທີ່(*p&&isspace((ຈຳນວນເຕັມ)*p))
            p++;
          ຖ້າ(!*p || *p=='#') continue;
          libName = p;
          ໃນຂະນະທີ່(*p&&!isspace((ຈຳນວນເຕັມ)*p)) {
            p++;
          }
          ຖ້າ(!*p || *p=='#') continue; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          ໃນຂະນະທີ່(*p&&isspace((ຈຳນວນເຕັມ)*p)) {
            p++;
          }
          ຖ້າ(!*p||*p=='#') continue; /* no symname after libname +tab */
          symName = p;
          ໃນຂະນະທີ່(*p&&!isspace((ຈຳນວນເຕັມ)*p)) {
            p++;
          }
                    
          ຖ້າ(*p) { /* has config */
            *p=0;
            ++p;
            ໃນຂະນະທີ່(*p&&isspace((ຈຳນວນເຕັມ)*p)) {
              p++;
            }
            ຖ້າ(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          ຖ້າ(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            ໃນຂະນະທີ່(p>config&&isspace((ຈຳນວນເຕັມ)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            ຖ້າ(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#ຖ້າ UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (void*)plug, u_errorName(subStatus)));
#ບໍ່ດັ່ງນັ້ນ
            (void)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } ບໍ່ດັ່ງນັ້ນ {
#ຖ້າ UPLUG_TRACE
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


