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

#شامل "unicode/icuplug.h"


#اگر UCONFIG_ENABLE_PLUGINS


#شامل "icuplugimp.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "putilimp.h"
#شامل "ucln.h"
#شامل <stdio.h>
#ifdef __MVS__  /* defined by z/OS compiler */
#define _POSIX_SOURCE
#شامل <cics.h> /* 12 Nov 2011 JAM iscics() function */
#endif
#شامل "charstr.h"

using فضاینام icu;

#ifndef UPLUG_TRACE
#define UPLUG_TRACE 0
#endif

#اگر UPLUG_TRACE
#شامل <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

ساختار UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  تهی *lib;              /**< plugin library, or nullptr */
  نویسه libName[UPLUG_NAME_MAX];   /**< library name */
  نویسه sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  نویسه config[UPLUG_NAME_MAX];     /**< configuration data */
  تهی *context;          /**< user context data */
  نویسه name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaitingLoad; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UErrorCode pluginStatus; /**< status code of plugin */
};



#define UPLUG_LIBRARY_INITIAL_شمار 8
#define UPLUG_PLUGIN_INITIAL_شمار 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
ایستا int32_t uplug_removeEntryAt(تهی *list, int32_t listSize, int32_t memberSize, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  اگر(listSize<1) {
    بازگشت listSize;
  }
    
  /* is there anything to move? */
  اگر(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  بازگشت listSize-1;
}




#اگر U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
ساختار UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
تعریفنوع ساختار UPlugLibrary {
  تهی *lib;                           /**< library ptr */
  نویسه name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

ایستا UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_شمار];
ایستا UPlugLibrary * libraryList = staticLibraryList;
ایستا int32_t library_شمار = 0;
ایستا int32_t libraryMax = UPLUG_LIBRARY_INITIAL_شمار;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ایستا int32_t جستجو_For_Library_Name(ثابت نویسه *libName) {
  int32_t i;
    
  برای(i=0;i<library_شمار;i++) {
    اگر(!uprv_strcmp(libName, libraryList[i].name)) {
      بازگشت i;
    }
  }
  بازگشت -1;
}

ایستا int32_t جستجو_For_Library(تهی *lib) {
  int32_t i;
    
  برای(i=0;i<library_شمار;i++) {
    اگر(lib==libraryList[i].lib) {
      بازگشت i;
    }
  }
  بازگشت -1;
}

U_CAPI نویسه * U_EXPORT2
uplug_findLibrary(تهی *lib, UErrorCode *status) {
  int32_t libEnt;
  نویسه *ret = nullptr;
  اگر(U_FAILURE(*status)) {
    بازگشت nullptr;
  }
  libEnt = جستجو_For_Library(lib);
  اگر(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } وگرنه {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  بازگشت ret;
}

U_CAPI تهی * U_EXPORT2
uplug_openLibrary(ثابت نویسه *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  تهی *lib = nullptr;
    
  اگر(U_FAILURE(*status)) بازگشت nullptr;

  libEntry = جستجو_For_Library_Name(libName);
  اگر(libEntry == -1) {
    libEntry = library_شمار++;
    اگر(library_شمار >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#اگر UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      بازگشت nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#اگر UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    اگر(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#اگر UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      library_شمار--;
    } وگرنه { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } وگرنه {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  بازگشت lib;
}

U_CAPI تهی U_EXPORT2
uplug_closeLibrary(تهی *lib, UErrorCode *status) {
  int32_t i;
    
#اگر UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (تهی*)libraryList));
#endif
  اگر(U_FAILURE(*status)) بازگشت;
    
  برای(i=0;i<library_شمار;i++) {
    اگر(lib==libraryList[i].lib) {
      اگر(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_شمار = uplug_removeEntryAt(libraryList, library_شمار, اندازه(*libraryList), i);
      }
      بازگشت;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

ایستا UPlugData pluginList[UPLUG_PLUGIN_INITIAL_شمار];
ایستا int32_t plugin_شمار = 0;



  
ایستا int32_t uplug_plugin_عدد(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[plugin_شمار];
  اگر(d<=pluginList) {
    بازگشت 0;
  } وگرنه اگر(d>=pastPlug) {
    بازگشت plugin_شمار;
  } وگرنه {
    بازگشت (d-pluginList)/اندازه(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  اگر(prior==nullptr) {
    بازگشت pluginList;
  } وگرنه {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[plugin_شمار];
    
    اگر(nextPlug>=pastPlug) {
      بازگشت nullptr;
    } وگرنه {
      بازگشت nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ایستا تهی uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  اگر(plug==nullptr||U_FAILURE(*status)) {
    بازگشت;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  اگر(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


ایستا تهی uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  اگر(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    بازگشت; 
  }
  اگر(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

ایستا تهی uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  اگر(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    بازگشت; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  اگر(U_SUCCESS(*status)) { 
    اگر(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = نادرست;
    }
  } وگرنه {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = نادرست;
  }
}


ایستا تهی uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  اگر(U_FAILURE(*status)) {
    بازگشت;
  }
  اگر(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_کم) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    بازگشت;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = نادرست;
  اگر(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

ایستا UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  اگر(U_FAILURE(*status)) {
    بازگشت nullptr;
  }

  اگر(plugin_شمار == UPLUG_PLUGIN_INITIAL_شمار) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    بازگشت nullptr;
  }

  plug = &pluginList[plugin_شمار++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = اندازه(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = درست;
  plug->dontUnload = نادرست;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  بازگشت plug;
}

ایستا UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ثابت نویسه *config, تهی *lib, ثابت نویسه *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  اگر(U_FAILURE(*status)) {
    بازگشت nullptr;
  }

  اگر(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } وگرنه {
    plug->config[0] = 0;
  }
    
  اگر(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } وگرنه {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  بازگشت plug;
}

ایستا تهی uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  اگر(!plug->dontUnload) {
#اگر U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  اگر(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  اگر(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_شمار = uplug_removeEntryAt(pluginList, plugin_شمار, اندازه(plug[0]), uplug_plugin_عدد(plug));
  } وگرنه {
    /* not ok- leave as a message. */
    plug->awaitingLoad=نادرست;
    plug->entrypoint=0;
    plug->dontUnload=درست;
  }
}

ایستا تهی uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  اگر(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI تهی U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  اگر(U_FAILURE(*status)) بازگشت;
    
  برای(cursor=pluginList;cursor!=nullptr;) {
    اگر(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } وگرنه {
      cursor = uplug_nextPlug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI تهی U_EXPORT2 
uplug_setPlugNoUnload(UPlugData *data, UBool dontUnload)
{
  data->dontUnload = dontUnload;
}


U_CAPI تهی U_EXPORT2
uplug_setPlugLevel(UPlugData *data, UPlugLevel level) {
  data->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlugData *data) {
  بازگشت data->level;
}


U_CAPI تهی U_EXPORT2
uplug_setPlugName(UPlugData *data, ثابت نویسه *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI ثابت نویسه* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  بازگشت data->name;
}


U_CAPI ثابت نویسه* U_EXPORT2
uplug_get_نماد_Name(UPlugData* data U_LIFETIME_BOUND) {
  بازگشت data->sym;
}

U_CAPI ثابت نویسه* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  اگر(data->libName[0]) {
    بازگشت data->libName;
  } وگرنه {
#اگر U_ENABLE_DYLOAD
    بازگشت uplug_findLibrary(data->lib, status);
#وگرنه
    بازگشت nullptr;
#endif
  }
}

U_CAPI تهی * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  بازگشت data->lib;
}

U_CAPI تهی * U_EXPORT2
uplug_getContext(UPlugData *data) {
  بازگشت data->context;
}


U_CAPI تهی U_EXPORT2
uplug_setContext(UPlugData *data, تهی *context) {
  data->context = context;
}

U_CAPI ثابت نویسه* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  بازگشت data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  اگر(n <0 || n >= plugin_شمار) {
    بازگشت nullptr;
  } وگرنه { 
    بازگشت &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  بازگشت plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ایستا UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ثابت نویسه *config, تهی *lib, ثابت نویسه *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  اگر(U_SUCCESS(*status)) {
    بازگشت plug;
  } وگرنه {
    uplug_deallocatePlug(plug, status);
    بازگشت nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, ثابت نویسه *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  بازگشت plug;
}

#اگر U_ENABLE_DYLOAD

ایستا UPlugData* 
uplug_initErrorPlug(ثابت نویسه *libName, ثابت نویسه *sym, ثابت نویسه *config, ثابت نویسه *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  اگر(U_FAILURE(*status)) بازگشت nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = نادرست; /* Won't load. */
  plug->dontUnload = درست; /* cannot unload. */

  اگر(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  اگر(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  اگر(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  اگر(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  بازگشت plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ایستا UPlugData* 
uplug_initPlugFromLibrary(ثابت نویسه *libName, ثابت نویسه *sym, ثابت نویسه *config, UErrorCode *status) {
  تهی *lib = nullptr;
  UPlugData *plug = nullptr;
  اگر(U_FAILURE(*status)) { بازگشت nullptr; }
  lib = uplug_openLibrary(libName, status);
  اگر(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    اگر(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      اگر(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } وگرنه {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    اگر(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } وگرنه {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  بازگشت plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(ثابت نویسه *libName, ثابت نویسه *sym, ثابت نویسه *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  اگر(U_FAILURE(*status)) { بازگشت nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  بازگشت plug;
}

#endif

ایستا UPlugLevel gCurrentLevel = UPLUG_LEVEL_کم;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  بازگشت gCurrentLevel;
}

ایستا UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  برای(i=0;i<plugin_شمار;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_کم;
  بازگشت درست;
}

#اگر U_ENABLE_DYLOAD

ایستا تهی uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  اگر(U_FAILURE(*status)) {
    بازگشت;
  }
#اگر UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  برای(i=0;i<plugin_شمار;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    اگر(pluginToLoad->awaitingLoad) {
      اگر(pluginToLoad->level == UPLUG_LEVEL_کم) {
        اگر(currentLevel > UPLUG_LEVEL_کم) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_بالا;
        } وگرنه {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          اگر(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = نادرست;
      } 
    }
  }    
  برای(i=0;i<plugin_شمار;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    اگر(pluginToLoad->awaitingLoad) {
      اگر(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } وگرنه اگر(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } وگرنه {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = نادرست;
    }
  }
    
#اگر UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
ایستا نویسه plugin_file[2048] = "";
#endif

U_CAPI ثابت نویسه* U_EXPORT2
uplug_getPluginFile() {
#اگر U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  بازگشت plugin_file;
#وگرنه
  بازگشت nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI تهی U_EXPORT2
uplug_init(UErrorCode *status) {
#اگر !U_ENABLE_DYLOAD
  (تهی)status; /* unused */
#وگرنهاگر !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ثابت نویسه *env = getenv("ICU_PLUGINS");

  اگر(U_FAILURE(*status)) بازگشت;
  اگر(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  اگر(U_FAILURE(*status)) بازگشت;

#اگر defined(DEFAULT_ICU_PLUGINS) 
  اگر(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#اگر UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  اگر(!plugin_dir.isEmpty()) {
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
#وگرنه
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#اگر UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    اگر(U_FAILURE(*status)) {
      بازگشت;
    }
    اگر((size_t)pluginFile.length() > (اندازه(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#اگر UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      بازگشت;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, اندازه(plugin_file), *status);

#اگر UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (صحیح)strlen(plugin_file), (صحیح)اندازه(plugin_file)));
#endif
        
#ifdef __MVS__
    اگر (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    وگرنه
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    اگر(f != nullptr) {
      نویسه linebuf[1024];
      نویسه *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t سطر = 0;
            
            
      وقتی(fgets(linebuf,1023,f)) {
        سطر++;

        اگر(!*linebuf || *linebuf=='#') {
          ادامه;
        } وگرنه {
          p = linebuf;
          وقتی(*p&&isspace((صحیح)*p))
            p++;
          اگر(!*p || *p=='#') ادامه;
          libName = p;
          وقتی(*p&&!isspace((صحیح)*p)) {
            p++;
          }
          اگر(!*p || *p=='#') ادامه; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          وقتی(*p&&isspace((صحیح)*p)) {
            p++;
          }
          اگر(!*p||*p=='#') ادامه; /* no symname after libname +tab */
          symName = p;
          وقتی(*p&&!isspace((صحیح)*p)) {
            p++;
          }
                    
          اگر(*p) { /* has config */
            *p=0;
            ++p;
            وقتی(*p&&isspace((صحیح)*p)) {
              p++;
            }
            اگر(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          اگر(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            وقتی(p>config&&isspace((صحیح)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            اگر(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#اگر UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (تهی*)plug, u_errorName(subStatus)));
#وگرنه
            (تهی)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } وگرنه {
#اگر UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_بالا;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


