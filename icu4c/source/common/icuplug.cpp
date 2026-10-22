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


#nếu UCONFIG_ENABLE_PLUGINS


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

#nếu UPLUG_TRACE
#include <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

struct UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  void *lib;              /**< plugin library, or nullptr */
  char libName[UPLUG_NAME_MAX];   /**< library name */
  char sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  char config[UPLUG_NAME_MAX];     /**< configuration data */
  void *context;          /**< user context data */
  char name[UPLUG_NAME_MAX];   /**< name of plugin */
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
  nếu(listSize<1) {
    trả_về listSize;
  }
    
  /* is there anything to move? */
  nếu(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  trả_về listSize-1;
}




#nếu U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
struct UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
typedef struct UPlugLibrary {
  void *lib;                           /**< library ptr */
  char name[UPLUG_NAME_MAX]; /**< library name */
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
static int32_t searchForLibraryName(const char *libName) {
  int32_t i;
    
  cho(i=0;i<libraryCount;i++) {
    nếu(!uprv_strcmp(libName, libraryList[i].name)) {
      trả_về i;
    }
  }
  trả_về -1;
}

static int32_t searchForLibrary(void *lib) {
  int32_t i;
    
  cho(i=0;i<libraryCount;i++) {
    nếu(lib==libraryList[i].lib) {
      trả_về i;
    }
  }
  trả_về -1;
}

U_CAPI char * U_EXPORT2
uplug_findLibrary(void *lib, UErrorCode *status) {
  int32_t libEnt;
  char *ret = nullptr;
  nếu(U_FAILURE(*status)) {
    trả_về nullptr;
  }
  libEnt = searchForLibrary(lib);
  nếu(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } không_thì {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  trả_về ret;
}

U_CAPI void * U_EXPORT2
uplug_openLibrary(const char *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  void *lib = nullptr;
    
  nếu(U_FAILURE(*status)) trả_về nullptr;

  libEntry = searchForLibraryName(libName);
  nếu(libEntry == -1) {
    libEntry = libraryCount++;
    nếu(libraryCount >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#nếu UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      trả_về nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#nếu UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    nếu(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#nếu UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      libraryCount--;
    } không_thì { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } không_thì {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  trả_về lib;
}

U_CAPI void U_EXPORT2
uplug_closeLibrary(void *lib, UErrorCode *status) {
  int32_t i;
    
#nếu UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (void*)libraryList));
#endif
  nếu(U_FAILURE(*status)) trả_về;
    
  cho(i=0;i<libraryCount;i++) {
    nếu(lib==libraryList[i].lib) {
      nếu(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        libraryCount = uplug_removeEntryAt(libraryList, libraryCount, sizeof(*libraryList), i);
      }
      trả_về;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

static UPlugData pluginList[UPLUG_PLUGIN_INITIAL_COUNT];
static int32_t pluginCount = 0;



  
static int32_t uplug_pluginNumber(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[pluginCount];
  nếu(d<=pluginList) {
    trả_về 0;
  } không_thì nếu(d>=pastPlug) {
    trả_về pluginCount;
  } không_thì {
    trả_về (d-pluginList)/sizeof(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  nếu(prior==nullptr) {
    trả_về pluginList;
  } không_thì {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[pluginCount];
    
    nếu(nextPlug>=pastPlug) {
      trả_về nullptr;
    } không_thì {
      trả_về nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
static void uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  nếu(plug==nullptr||U_FAILURE(*status)) {
    trả_về;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  nếu(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


static void uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  nếu(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    trả_về; 
  }
  nếu(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

static void uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  nếu(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    trả_về; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  nếu(U_SUCCESS(*status)) { 
    nếu(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = false;
    }
  } không_thì {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = false;
  }
}


static void uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  nếu(U_FAILURE(*status)) {
    trả_về;
  }
  nếu(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_LOW) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    trả_về;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = false;
  nếu(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

static UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  nếu(U_FAILURE(*status)) {
    trả_về nullptr;
  }

  nếu(pluginCount == UPLUG_PLUGIN_INITIAL_COUNT) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    trả_về nullptr;
  }

  plug = &pluginList[pluginCount++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = sizeof(UPlugData);
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


  trả_về plug;
}

static UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, const char *config, void *lib, const char *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  nếu(U_FAILURE(*status)) {
    trả_về nullptr;
  }

  nếu(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } không_thì {
    plug->config[0] = 0;
  }
    
  nếu(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } không_thì {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  trả_về plug;
}

static void uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  nếu(!plug->dontUnload) {
#nếu U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  nếu(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  nếu(U_SUCCESS(*status)) {
    /* all ok- remove. */
    pluginCount = uplug_removeEntryAt(pluginList, pluginCount, sizeof(plug[0]), uplug_pluginNumber(plug));
  } không_thì {
    /* not ok- leave as a message. */
    plug->awaitingLoad=false;
    plug->entrypoint=0;
    plug->dontUnload=true;
  }
}

static void uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  nếu(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI void U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  nếu(U_FAILURE(*status)) trả_về;
    
  cho(cursor=pluginList;cursor!=nullptr;) {
    nếu(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } không_thì {
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
  trả_về data->level;
}


U_CAPI void U_EXPORT2
uplug_setPlugName(UPlugData *data, const char *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI const char* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  trả_về data->name;
}


U_CAPI const char* U_EXPORT2
uplug_getSymbolName(UPlugData* data U_LIFETIME_BOUND) {
  trả_về data->sym;
}

U_CAPI const char* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  nếu(data->libName[0]) {
    trả_về data->libName;
  } không_thì {
#nếu U_ENABLE_DYLOAD
    trả_về uplug_findLibrary(data->lib, status);
#không_thì
    trả_về nullptr;
#endif
  }
}

U_CAPI void * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  trả_về data->lib;
}

U_CAPI void * U_EXPORT2
uplug_getContext(UPlugData *data) {
  trả_về data->context;
}


U_CAPI void U_EXPORT2
uplug_setContext(UPlugData *data, void *context) {
  data->context = context;
}

U_CAPI const char* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  trả_về data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  nếu(n <0 || n >= pluginCount) {
    trả_về nullptr;
  } không_thì { 
    trả_về &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  trả_về plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
static UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, const char *config, void *lib, const char *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  nếu(U_SUCCESS(*status)) {
    trả_về plug;
  } không_thì {
    uplug_deallocatePlug(plug, status);
    trả_về nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, const char *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  trả_về plug;
}

#nếu U_ENABLE_DYLOAD

static UPlugData* 
uplug_initErrorPlug(const char *libName, const char *sym, const char *config, const char *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  nếu(U_FAILURE(*status)) trả_về nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = false; /* Won't load. */
  plug->dontUnload = true; /* cannot unload. */

  nếu(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  nếu(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  nếu(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  nếu(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  trả_về plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
static UPlugData* 
uplug_initPlugFromLibrary(const char *libName, const char *sym, const char *config, UErrorCode *status) {
  void *lib = nullptr;
  UPlugData *plug = nullptr;
  nếu(U_FAILURE(*status)) { trả_về nullptr; }
  lib = uplug_openLibrary(libName, status);
  nếu(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    nếu(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      nếu(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } không_thì {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    nếu(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } không_thì {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  trả_về plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(const char *libName, const char *sym, const char *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  nếu(U_FAILURE(*status)) { trả_về nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  trả_về plug;
}

#endif

static UPlugLevel gCurrentLevel = UPLUG_LEVEL_LOW;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  trả_về gCurrentLevel;
}

static UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  cho(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_LOW;
  trả_về true;
}

#nếu U_ENABLE_DYLOAD

static void uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  nếu(U_FAILURE(*status)) {
    trả_về;
  }
#nếu UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  cho(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    nếu(pluginToLoad->awaitingLoad) {
      nếu(pluginToLoad->level == UPLUG_LEVEL_LOW) {
        nếu(currentLevel > UPLUG_LEVEL_LOW) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_HIGH;
        } không_thì {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          nếu(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = false;
      } 
    }
  }    
  cho(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    nếu(pluginToLoad->awaitingLoad) {
      nếu(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } không_thì nếu(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } không_thì {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = false;
    }
  }
    
#nếu UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
static char plugin_file[2048] = "";
#endif

U_CAPI const char* U_EXPORT2
uplug_getPluginFile() {
#nếu U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  trả_về plugin_file;
#không_thì
  trả_về nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI void U_EXPORT2
uplug_init(UErrorCode *status) {
#nếu !U_ENABLE_DYLOAD
  (void)status; /* unused */
#elif !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  const char *env = getenv("ICU_PLUGINS");

  nếu(U_FAILURE(*status)) trả_về;
  nếu(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  nếu(U_FAILURE(*status)) trả_về;

#nếu defined(DEFAULT_ICU_PLUGINS) 
  nếu(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#nếu UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  nếu(!plugin_dir.isEmpty()) {
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
#không_thì
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#nếu UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    nếu(U_FAILURE(*status)) {
      trả_về;
    }
    nếu((size_t)pluginFile.length() > (sizeof(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#nếu UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      trả_về;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, sizeof(plugin_file), *status);

#nếu UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (số_nguyên)strlen(plugin_file), (số_nguyên)sizeof(plugin_file)));
#endif
        
#ifdef __MVS__
    nếu (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    không_thì
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    nếu(f != nullptr) {
      char linebuf[1024];
      char *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t line = 0;
            
            
      trong_khi(fgets(linebuf,1023,f)) {
        line++;

        nếu(!*linebuf || *linebuf=='#') {
          continue;
        } không_thì {
          p = linebuf;
          trong_khi(*p&&isspace((số_nguyên)*p))
            p++;
          nếu(!*p || *p=='#') continue;
          libName = p;
          trong_khi(*p&&!isspace((số_nguyên)*p)) {
            p++;
          }
          nếu(!*p || *p=='#') continue; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          trong_khi(*p&&isspace((số_nguyên)*p)) {
            p++;
          }
          nếu(!*p||*p=='#') continue; /* no symname after libname +tab */
          symName = p;
          trong_khi(*p&&!isspace((số_nguyên)*p)) {
            p++;
          }
                    
          nếu(*p) { /* has config */
            *p=0;
            ++p;
            trong_khi(*p&&isspace((số_nguyên)*p)) {
              p++;
            }
            nếu(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          nếu(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            trong_khi(p>config&&isspace((số_nguyên)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            nếu(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#nếu UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (void*)plug, u_errorName(subStatus)));
#không_thì
            (void)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } không_thì {
#nếu UPLUG_TRACE
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


