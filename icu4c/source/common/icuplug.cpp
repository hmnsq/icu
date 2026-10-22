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


#если UCONFIG_ENABLE_PLUGINS


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

#если UPLUG_TRACE
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
  если(listSize<1) {
    вернуть listSize;
  }
    
  /* is there anything to move? */
  если(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  вернуть listSize-1;
}




#если U_ENABLE_DYLOAD
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
    
  для(i=0;i<libraryCount;i++) {
    если(!uprv_strcmp(libName, libraryList[i].name)) {
      вернуть i;
    }
  }
  вернуть -1;
}

static int32_t searchForLibrary(void *lib) {
  int32_t i;
    
  для(i=0;i<libraryCount;i++) {
    если(lib==libraryList[i].lib) {
      вернуть i;
    }
  }
  вернуть -1;
}

U_CAPI char * U_EXPORT2
uplug_findLibrary(void *lib, UErrorCode *status) {
  int32_t libEnt;
  char *ret = nullptr;
  если(U_FAILURE(*status)) {
    вернуть nullptr;
  }
  libEnt = searchForLibrary(lib);
  если(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } иначе {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  вернуть ret;
}

U_CAPI void * U_EXPORT2
uplug_openLibrary(const char *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  void *lib = nullptr;
    
  если(U_FAILURE(*status)) вернуть nullptr;

  libEntry = searchForLibraryName(libName);
  если(libEntry == -1) {
    libEntry = libraryCount++;
    если(libraryCount >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#если UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      вернуть nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#если UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    если(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#если UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      libraryCount--;
    } иначе { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } иначе {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  вернуть lib;
}

U_CAPI void U_EXPORT2
uplug_closeLibrary(void *lib, UErrorCode *status) {
  int32_t i;
    
#если UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (void*)libraryList));
#endif
  если(U_FAILURE(*status)) вернуть;
    
  для(i=0;i<libraryCount;i++) {
    если(lib==libraryList[i].lib) {
      если(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        libraryCount = uplug_removeEntryAt(libraryList, libraryCount, sizeof(*libraryList), i);
      }
      вернуть;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

static UPlugData pluginList[UPLUG_PLUGIN_INITIAL_COUNT];
static int32_t pluginCount = 0;



  
static int32_t uplug_pluginNumber(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[pluginCount];
  если(d<=pluginList) {
    вернуть 0;
  } иначе если(d>=pastPlug) {
    вернуть pluginCount;
  } иначе {
    вернуть (d-pluginList)/sizeof(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  если(prior==nullptr) {
    вернуть pluginList;
  } иначе {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[pluginCount];
    
    если(nextPlug>=pastPlug) {
      вернуть nullptr;
    } иначе {
      вернуть nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
static void uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  если(plug==nullptr||U_FAILURE(*status)) {
    вернуть;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  если(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


static void uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  если(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    вернуть; 
  }
  если(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

static void uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  если(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    вернуть; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  если(U_SUCCESS(*status)) { 
    если(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = false;
    }
  } иначе {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = false;
  }
}


static void uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  если(U_FAILURE(*status)) {
    вернуть;
  }
  если(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_LOW) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    вернуть;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = false;
  если(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

static UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  если(U_FAILURE(*status)) {
    вернуть nullptr;
  }

  если(pluginCount == UPLUG_PLUGIN_INITIAL_COUNT) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    вернуть nullptr;
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


  вернуть plug;
}

static UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, const char *config, void *lib, const char *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  если(U_FAILURE(*status)) {
    вернуть nullptr;
  }

  если(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } иначе {
    plug->config[0] = 0;
  }
    
  если(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } иначе {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  вернуть plug;
}

static void uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  если(!plug->dontUnload) {
#если U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  если(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  если(U_SUCCESS(*status)) {
    /* all ok- remove. */
    pluginCount = uplug_removeEntryAt(pluginList, pluginCount, sizeof(plug[0]), uplug_pluginNumber(plug));
  } иначе {
    /* not ok- leave as a message. */
    plug->awaitingLoad=false;
    plug->entrypoint=0;
    plug->dontUnload=true;
  }
}

static void uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  если(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI void U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  если(U_FAILURE(*status)) вернуть;
    
  для(cursor=pluginList;cursor!=nullptr;) {
    если(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } иначе {
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
  вернуть data->level;
}


U_CAPI void U_EXPORT2
uplug_setPlugName(UPlugData *data, const char *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI const char* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  вернуть data->name;
}


U_CAPI const char* U_EXPORT2
uplug_getSymbolName(UPlugData* data U_LIFETIME_BOUND) {
  вернуть data->sym;
}

U_CAPI const char* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  если(data->libName[0]) {
    вернуть data->libName;
  } иначе {
#если U_ENABLE_DYLOAD
    вернуть uplug_findLibrary(data->lib, status);
#иначе
    вернуть nullptr;
#endif
  }
}

U_CAPI void * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  вернуть data->lib;
}

U_CAPI void * U_EXPORT2
uplug_getContext(UPlugData *data) {
  вернуть data->context;
}


U_CAPI void U_EXPORT2
uplug_setContext(UPlugData *data, void *context) {
  data->context = context;
}

U_CAPI const char* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  вернуть data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  если(n <0 || n >= pluginCount) {
    вернуть nullptr;
  } иначе { 
    вернуть &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  вернуть plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
static UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, const char *config, void *lib, const char *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  если(U_SUCCESS(*status)) {
    вернуть plug;
  } иначе {
    uplug_deallocatePlug(plug, status);
    вернуть nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, const char *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  вернуть plug;
}

#если U_ENABLE_DYLOAD

static UPlugData* 
uplug_initErrorPlug(const char *libName, const char *sym, const char *config, const char *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  если(U_FAILURE(*status)) вернуть nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = false; /* Won't load. */
  plug->dontUnload = true; /* cannot unload. */

  если(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  если(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  если(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  если(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  вернуть plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
static UPlugData* 
uplug_initPlugFromLibrary(const char *libName, const char *sym, const char *config, UErrorCode *status) {
  void *lib = nullptr;
  UPlugData *plug = nullptr;
  если(U_FAILURE(*status)) { вернуть nullptr; }
  lib = uplug_openLibrary(libName, status);
  если(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    если(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      если(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } иначе {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    если(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } иначе {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  вернуть plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(const char *libName, const char *sym, const char *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  если(U_FAILURE(*status)) { вернуть nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  вернуть plug;
}

#endif

static UPlugLevel gCurrentLevel = UPLUG_LEVEL_LOW;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  вернуть gCurrentLevel;
}

static UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  для(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_LOW;
  вернуть true;
}

#если U_ENABLE_DYLOAD

static void uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  если(U_FAILURE(*status)) {
    вернуть;
  }
#если UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  для(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    если(pluginToLoad->awaitingLoad) {
      если(pluginToLoad->level == UPLUG_LEVEL_LOW) {
        если(currentLevel > UPLUG_LEVEL_LOW) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_HIGH;
        } иначе {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          если(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = false;
      } 
    }
  }    
  для(i=0;i<pluginCount;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    если(pluginToLoad->awaitingLoad) {
      если(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } иначе если(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } иначе {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = false;
    }
  }
    
#если UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
static char plugin_file[2048] = "";
#endif

U_CAPI const char* U_EXPORT2
uplug_getPluginFile() {
#если U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  вернуть plugin_file;
#иначе
  вернуть nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI void U_EXPORT2
uplug_init(UErrorCode *status) {
#если !U_ENABLE_DYLOAD
  (void)status; /* unused */
#elif !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  const char *env = getenv("ICU_PLUGINS");

  если(U_FAILURE(*status)) вернуть;
  если(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  если(U_FAILURE(*status)) вернуть;

#если defined(DEFAULT_ICU_PLUGINS) 
  если(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#если UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  если(!plugin_dir.isEmpty()) {
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
#иначе
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#если UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    если(U_FAILURE(*status)) {
      вернуть;
    }
    если((size_t)pluginFile.length() > (sizeof(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#если UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      вернуть;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, sizeof(plugin_file), *status);

#если UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (целое)strlen(plugin_file), (целое)sizeof(plugin_file)));
#endif
        
#ifdef __MVS__
    если (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    иначе
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    если(f != nullptr) {
      char linebuf[1024];
      char *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t line = 0;
            
            
      пока(fgets(linebuf,1023,f)) {
        line++;

        если(!*linebuf || *linebuf=='#') {
          continue;
        } иначе {
          p = linebuf;
          пока(*p&&isspace((целое)*p))
            p++;
          если(!*p || *p=='#') continue;
          libName = p;
          пока(*p&&!isspace((целое)*p)) {
            p++;
          }
          если(!*p || *p=='#') continue; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          пока(*p&&isspace((целое)*p)) {
            p++;
          }
          если(!*p||*p=='#') continue; /* no symname after libname +tab */
          symName = p;
          пока(*p&&!isspace((целое)*p)) {
            p++;
          }
                    
          если(*p) { /* has config */
            *p=0;
            ++p;
            пока(*p&&isspace((целое)*p)) {
              p++;
            }
            если(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          если(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            пока(p>config&&isspace((целое)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            если(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#если UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (void*)plug, u_errorName(subStatus)));
#иначе
            (void)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } иначе {
#если UPLUG_TRACE
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


