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


#إذا UCONFIG_ENABLE_PLUGINS


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

#إذا UPLUG_TRACE
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



#define UPLUG_LIBRARY_INITIAL_ژماردن 8
#define UPLUG_PLUGIN_INITIAL_ژماردن 12

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
  إذا(listSize<1) {
    أرجع listSize;
  }
    
  /* is there anything to move? */
  إذا(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  أرجع listSize-1;
}




#إذا U_ENABLE_DYLOAD
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

ساكن UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_ژماردن];
ساكن UPlugLibrary * libraryList = staticLibraryList;
ساكن int32_t library_ژماردن = 0;
ساكن int32_t libraryMax = UPLUG_LIBRARY_INITIAL_ژماردن;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ساكن int32_t گەڕان_For_Library_Name(ثابت حرف *libName) {
  int32_t i;
    
  لكل(i=0;i<library_ژماردن;i++) {
    إذا(!uprv_strcmp(libName, libraryList[i].name)) {
      أرجع i;
    }
  }
  أرجع -1;
}

ساكن int32_t گەڕان_For_Library(فراغ *lib) {
  int32_t i;
    
  لكل(i=0;i<library_ژماردن;i++) {
    إذا(lib==libraryList[i].lib) {
      أرجع i;
    }
  }
  أرجع -1;
}

U_CAPI حرف * U_EXPORT2
uplug_findLibrary(فراغ *lib, UErrorCode *status) {
  int32_t libEnt;
  حرف *ret = nullptr;
  إذا(U_FAILURE(*status)) {
    أرجع nullptr;
  }
  libEnt = گەڕان_For_Library(lib);
  إذا(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } وإلا {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  أرجع ret;
}

U_CAPI فراغ * U_EXPORT2
uplug_openLibrary(ثابت حرف *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  فراغ *lib = nullptr;
    
  إذا(U_FAILURE(*status)) أرجع nullptr;

  libEntry = گەڕان_For_Library_Name(libName);
  إذا(libEntry == -1) {
    libEntry = library_ژماردن++;
    إذا(library_ژماردن >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#إذا UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      أرجع nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#إذا UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    إذا(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#إذا UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      library_ژماردن--;
    } وإلا { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } وإلا {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  أرجع lib;
}

U_CAPI فراغ U_EXPORT2
uplug_closeLibrary(فراغ *lib, UErrorCode *status) {
  int32_t i;
    
#إذا UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (فراغ*)libraryList));
#endif
  إذا(U_FAILURE(*status)) أرجع;
    
  لكل(i=0;i<library_ژماردن;i++) {
    إذا(lib==libraryList[i].lib) {
      إذا(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_ژماردن = uplug_removeEntryAt(libraryList, library_ژماردن, حجم(*libraryList), i);
      }
      أرجع;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

ساكن UPlugData pluginList[UPLUG_PLUGIN_INITIAL_ژماردن];
ساكن int32_t plugin_ژماردن = 0;



  
ساكن int32_t uplug_plugin_ژمار(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[plugin_ژماردن];
  إذا(d<=pluginList) {
    أرجع 0;
  } وإلا إذا(d>=pastPlug) {
    أرجع plugin_ژماردن;
  } وإلا {
    أرجع (d-pluginList)/حجم(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  إذا(prior==nullptr) {
    أرجع pluginList;
  } وإلا {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[plugin_ژماردن];
    
    إذا(nextPlug>=pastPlug) {
      أرجع nullptr;
    } وإلا {
      أرجع nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ساكن فراغ uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  إذا(plug==nullptr||U_FAILURE(*status)) {
    أرجع;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  إذا(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


ساكن فراغ uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  إذا(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    أرجع; 
  }
  إذا(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

ساكن فراغ uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  إذا(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    أرجع; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  إذا(U_SUCCESS(*status)) { 
    إذا(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = خطأ;
    }
  } وإلا {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = خطأ;
  }
}


ساكن فراغ uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  إذا(U_FAILURE(*status)) {
    أرجع;
  }
  إذا(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_نزم) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    أرجع;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = خطأ;
  إذا(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

ساكن UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  إذا(U_FAILURE(*status)) {
    أرجع nullptr;
  }

  إذا(plugin_ژماردن == UPLUG_PLUGIN_INITIAL_ژماردن) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    أرجع nullptr;
  }

  plug = &pluginList[plugin_ژماردن++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = حجم(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = true;
  plug->dontUnload = خطأ;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  أرجع plug;
}

ساكن UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  إذا(U_FAILURE(*status)) {
    أرجع nullptr;
  }

  إذا(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } وإلا {
    plug->config[0] = 0;
  }
    
  إذا(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } وإلا {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  أرجع plug;
}

ساكن فراغ uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  إذا(!plug->dontUnload) {
#إذا U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  إذا(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  إذا(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_ژماردن = uplug_removeEntryAt(pluginList, plugin_ژماردن, حجم(plug[0]), uplug_plugin_ژمار(plug));
  } وإلا {
    /* not ok- leave as a message. */
    plug->awaitingLoad=خطأ;
    plug->entrypoint=0;
    plug->dontUnload=true;
  }
}

ساكن فراغ uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  إذا(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI فراغ U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  إذا(U_FAILURE(*status)) أرجع;
    
  لكل(cursor=pluginList;cursor!=nullptr;) {
    إذا(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } وإلا {
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
  أرجع data->level;
}


U_CAPI فراغ U_EXPORT2
uplug_setPlugName(UPlugData *data, ثابت حرف *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  أرجع data->name;
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_get_هێما_Name(UPlugData* data U_LIFETIME_BOUND) {
  أرجع data->sym;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  إذا(data->libName[0]) {
    أرجع data->libName;
  } وإلا {
#إذا U_ENABLE_DYLOAD
    أرجع uplug_findLibrary(data->lib, status);
#وإلا
    أرجع nullptr;
#endif
  }
}

U_CAPI فراغ * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  أرجع data->lib;
}

U_CAPI فراغ * U_EXPORT2
uplug_getContext(UPlugData *data) {
  أرجع data->context;
}


U_CAPI فراغ U_EXPORT2
uplug_setContext(UPlugData *data, فراغ *context) {
  data->context = context;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  أرجع data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  إذا(n <0 || n >= plugin_ژماردن) {
    أرجع nullptr;
  } وإلا { 
    أرجع &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  أرجع plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ساكن UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  إذا(U_SUCCESS(*status)) {
    أرجع plug;
  } وإلا {
    uplug_deallocatePlug(plug, status);
    أرجع nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, ثابت حرف *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  أرجع plug;
}

#إذا U_ENABLE_DYLOAD

ساكن UPlugData* 
uplug_initErrorPlug(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, ثابت حرف *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  إذا(U_FAILURE(*status)) أرجع nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = خطأ; /* Won't load. */
  plug->dontUnload = true; /* cannot unload. */

  إذا(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  إذا(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  إذا(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  إذا(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  أرجع plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ساكن UPlugData* 
uplug_initPlugFromLibrary(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UErrorCode *status) {
  فراغ *lib = nullptr;
  UPlugData *plug = nullptr;
  إذا(U_FAILURE(*status)) { أرجع nullptr; }
  lib = uplug_openLibrary(libName, status);
  إذا(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    إذا(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      إذا(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } وإلا {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    إذا(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } وإلا {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  أرجع plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  إذا(U_FAILURE(*status)) { أرجع nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  أرجع plug;
}

#endif

ساكن UPlugLevel gCurrentLevel = UPLUG_LEVEL_نزم;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  أرجع gCurrentLevel;
}

ساكن UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  لكل(i=0;i<plugin_ژماردن;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_نزم;
  أرجع true;
}

#إذا U_ENABLE_DYLOAD

ساكن فراغ uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  إذا(U_FAILURE(*status)) {
    أرجع;
  }
#إذا UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  لكل(i=0;i<plugin_ژماردن;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    إذا(pluginToLoad->awaitingLoad) {
      إذا(pluginToLoad->level == UPLUG_LEVEL_نزم) {
        إذا(currentLevel > UPLUG_LEVEL_نزم) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_بەرز;
        } وإلا {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          إذا(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = خطأ;
      } 
    }
  }    
  لكل(i=0;i<plugin_ژماردن;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    إذا(pluginToLoad->awaitingLoad) {
      إذا(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } وإلا إذا(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } وإلا {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = خطأ;
    }
  }
    
#إذا UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
ساكن حرف plugin_file[2048] = "";
#endif

U_CAPI ثابت حرف* U_EXPORT2
uplug_getPluginFile() {
#إذا U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  أرجع plugin_file;
#وإلا
  أرجع nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI فراغ U_EXPORT2
uplug_init(UErrorCode *status) {
#إذا !U_ENABLE_DYLOAD
  (فراغ)status; /* unused */
#وإلاإذا !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ثابت حرف *env = getenv("ICU_PLUGINS");

  إذا(U_FAILURE(*status)) أرجع;
  إذا(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  إذا(U_FAILURE(*status)) أرجع;

#إذا defined(DEFAULT_ICU_PLUGINS) 
  إذا(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#إذا UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  إذا(!plugin_dir.isEmpty()) {
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
#وإلا
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#إذا UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    إذا(U_FAILURE(*status)) {
      أرجع;
    }
    إذا((size_t)pluginFile.length() > (حجم(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#إذا UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      أرجع;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, حجم(plugin_file), *status);

#إذا UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (صحيح)strlen(plugin_file), (صحيح)حجم(plugin_file)));
#endif
        
#ifdef __MVS__
    إذا (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    وإلا
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    إذا(f != nullptr) {
      حرف linebuf[1024];
      حرف *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t هێڵ = 0;
            
            
      طالما(fgets(linebuf,1023,f)) {
        هێڵ++;

        إذا(!*linebuf || *linebuf=='#') {
          استمر;
        } وإلا {
          p = linebuf;
          طالما(*p&&isspace((صحيح)*p))
            p++;
          إذا(!*p || *p=='#') استمر;
          libName = p;
          طالما(*p&&!isspace((صحيح)*p)) {
            p++;
          }
          إذا(!*p || *p=='#') استمر; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          طالما(*p&&isspace((صحيح)*p)) {
            p++;
          }
          إذا(!*p||*p=='#') استمر; /* no symname after libname +tab */
          symName = p;
          طالما(*p&&!isspace((صحيح)*p)) {
            p++;
          }
                    
          إذا(*p) { /* has config */
            *p=0;
            ++p;
            طالما(*p&&isspace((صحيح)*p)) {
              p++;
            }
            إذا(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          إذا(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            طالما(p>config&&isspace((صحيح)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            إذا(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#إذا UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (فراغ*)plug, u_errorName(subStatus)));
#وإلا
            (فراغ)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } وإلا {
#إذا UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_بەرز;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


