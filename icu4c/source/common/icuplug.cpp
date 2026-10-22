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

#অন্তর্ভাৱ "unicode/icuplug.h"


#যদি UCONFIG_ENABLE_PLUGINS


#অন্তর্ভাৱ "icuplugimp.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "putilimp.h"
#অন্তর্ভাৱ "ucln.h"
#অন্তর্ভাৱ <stdio.h>
#ifdef __MVS__  /* defined by z/OS compiler */
#define _POSIX_SOURCE
#অন্তর্ভাৱ <cics.h> /* 12 Nov 2011 JAM iscics() function */
#endif
#অন্তর্ভাৱ "charstr.h"

using নামাকাশ icu;

#ifndef UPLUG_TRACE
#define UPLUG_TRACE 0
#endif

#যদি UPLUG_TRACE
#অন্তর্ভাৱ <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

সংরচনা UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  শূন্য *lib;              /**< plugin library, or nullptr */
  ৱর্ণ libName[UPLUG_NAME_MAX];   /**< library name */
  ৱর্ণ sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  ৱর্ণ config[UPLUG_NAME_MAX];     /**< configuration data */
  শূন্য *context;          /**< user context data */
  ৱর্ণ name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaitingLoad; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UErrorCode pluginStatus; /**< status code of plugin */
};



#define UPLUG_LIBRARY_INITIAL_গুনতি 8
#define UPLUG_PLUGIN_INITIAL_গুনতি 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
স্থির int32_t uplug_removeEntryAt(শূন্য *list, int32_t listSize, int32_t memberSize, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  যদি(listSize<1) {
    ঘূৰাই listSize;
  }
    
  /* is there anything to move? */
  যদি(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  ঘূৰাই listSize-1;
}




#যদি U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
সংরচনা UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
প্রকারনির্দেশ সংরচনা UPlugLibrary {
  শূন্য *lib;                           /**< library ptr */
  ৱর্ণ name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

স্থির UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_গুনতি];
স্থির UPlugLibrary * libraryList = staticLibraryList;
স্থির int32_t library_গুনতি = 0;
স্থির int32_t libraryMax = UPLUG_LIBRARY_INITIAL_গুনতি;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
স্থির int32_t অনুসন্ধান_For_Library_Name(ধ্রুৱ ৱর্ণ *libName) {
  int32_t i;
    
  বাবে(i=0;i<library_গুনতি;i++) {
    যদি(!uprv_strcmp(libName, libraryList[i].name)) {
      ঘূৰাই i;
    }
  }
  ঘূৰাই -1;
}

স্থির int32_t অনুসন্ধান_For_Library(শূন্য *lib) {
  int32_t i;
    
  বাবে(i=0;i<library_গুনতি;i++) {
    যদি(lib==libraryList[i].lib) {
      ঘূৰাই i;
    }
  }
  ঘূৰাই -1;
}

U_CAPI ৱর্ণ * U_EXPORT2
uplug_findLibrary(শূন্য *lib, UErrorCode *status) {
  int32_t libEnt;
  ৱর্ণ *ret = nullptr;
  যদি(U_FAILURE(*status)) {
    ঘূৰাই nullptr;
  }
  libEnt = অনুসন্ধান_For_Library(lib);
  যদি(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } অন্যথা {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  ঘূৰাই ret;
}

U_CAPI শূন্য * U_EXPORT2
uplug_openLibrary(ধ্রুৱ ৱর্ণ *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  শূন্য *lib = nullptr;
    
  যদি(U_FAILURE(*status)) ঘূৰাই nullptr;

  libEntry = অনুসন্ধান_For_Library_Name(libName);
  যদি(libEntry == -1) {
    libEntry = library_গুনতি++;
    যদি(library_গুনতি >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#যদি UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      ঘূৰাই nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#যদি UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    যদি(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#যদি UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      library_গুনতি--;
    } অন্যথা { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } অন্যথা {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  ঘূৰাই lib;
}

U_CAPI শূন্য U_EXPORT2
uplug_closeLibrary(শূন্য *lib, UErrorCode *status) {
  int32_t i;
    
#যদি UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (শূন্য*)libraryList));
#endif
  যদি(U_FAILURE(*status)) ঘূৰাই;
    
  বাবে(i=0;i<library_গুনতি;i++) {
    যদি(lib==libraryList[i].lib) {
      যদি(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_গুনতি = uplug_removeEntryAt(libraryList, library_গুনতি, পরিমাণ(*libraryList), i);
      }
      ঘূৰাই;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

স্থির UPlugData pluginList[UPLUG_PLUGIN_INITIAL_গুনতি];
স্থির int32_t plugin_গুনতি = 0;



  
স্থির int32_t uplug_plugin_সংখ্যা(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[plugin_গুনতি];
  যদি(d<=pluginList) {
    ঘূৰাই 0;
  } অন্যথা যদি(d>=pastPlug) {
    ঘূৰাই plugin_গুনতি;
  } অন্যথা {
    ঘূৰাই (d-pluginList)/পরিমাণ(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  যদি(prior==nullptr) {
    ঘূৰাই pluginList;
  } অন্যথা {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[plugin_গুনতি];
    
    যদি(nextPlug>=pastPlug) {
      ঘূৰাই nullptr;
    } অন্যথা {
      ঘূৰাই nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
স্থির শূন্য uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  যদি(plug==nullptr||U_FAILURE(*status)) {
    ঘূৰাই;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  যদি(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


স্থির শূন্য uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  যদি(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ঘূৰাই; 
  }
  যদি(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

স্থির শূন্য uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  যদি(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ঘূৰাই; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  যদি(U_SUCCESS(*status)) { 
    যদি(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = মিছা;
    }
  } অন্যথা {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = মিছা;
  }
}


স্থির শূন্য uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  যদি(U_FAILURE(*status)) {
    ঘূৰাই;
  }
  যদি(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_নিচেৰ) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ঘূৰাই;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = মিছা;
  যদি(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

স্থির UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  যদি(U_FAILURE(*status)) {
    ঘূৰাই nullptr;
  }

  যদি(plugin_গুনতি == UPLUG_PLUGIN_INITIAL_গুনতি) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ঘূৰাই nullptr;
  }

  plug = &pluginList[plugin_গুনতি++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = পরিমাণ(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = সঁচা;
  plug->dontUnload = মিছা;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  ঘূৰাই plug;
}

স্থির UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ধ্রুৱ ৱর্ণ *config, শূন্য *lib, ধ্রুৱ ৱর্ণ *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  যদি(U_FAILURE(*status)) {
    ঘূৰাই nullptr;
  }

  যদি(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } অন্যথা {
    plug->config[0] = 0;
  }
    
  যদি(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } অন্যথা {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  ঘূৰাই plug;
}

স্থির শূন্য uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  যদি(!plug->dontUnload) {
#যদি U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  যদি(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  যদি(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_গুনতি = uplug_removeEntryAt(pluginList, plugin_গুনতি, পরিমাণ(plug[0]), uplug_plugin_সংখ্যা(plug));
  } অন্যথা {
    /* not ok- leave as a message. */
    plug->awaitingLoad=মিছা;
    plug->entrypoint=0;
    plug->dontUnload=সঁচা;
  }
}

স্থির শূন্য uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  যদি(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI শূন্য U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  যদি(U_FAILURE(*status)) ঘূৰাই;
    
  বাবে(cursor=pluginList;cursor!=nullptr;) {
    যদি(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } অন্যথা {
      cursor = uplug_nextPlug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI শূন্য U_EXPORT2 
uplug_setPlugNoUnload(UPlugData *data, UBool dontUnload)
{
  data->dontUnload = dontUnload;
}


U_CAPI শূন্য U_EXPORT2
uplug_setPlugLevel(UPlugData *data, UPlugLevel level) {
  data->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlugData *data) {
  ঘূৰাই data->level;
}


U_CAPI শূন্য U_EXPORT2
uplug_setPlugName(UPlugData *data, ধ্রুৱ ৱর্ণ *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  ঘূৰাই data->name;
}


U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
uplug_get_প্ৰতীক_Name(UPlugData* data U_LIFETIME_BOUND) {
  ঘূৰাই data->sym;
}

U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  যদি(data->libName[0]) {
    ঘূৰাই data->libName;
  } অন্যথা {
#যদি U_ENABLE_DYLOAD
    ঘূৰাই uplug_findLibrary(data->lib, status);
#অন্যথা
    ঘূৰাই nullptr;
#endif
  }
}

U_CAPI শূন্য * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  ঘূৰাই data->lib;
}

U_CAPI শূন্য * U_EXPORT2
uplug_getContext(UPlugData *data) {
  ঘূৰাই data->context;
}


U_CAPI শূন্য U_EXPORT2
uplug_setContext(UPlugData *data, শূন্য *context) {
  data->context = context;
}

U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  ঘূৰাই data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  যদি(n <0 || n >= plugin_গুনতি) {
    ঘূৰাই nullptr;
  } অন্যথা { 
    ঘূৰাই &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  ঘূৰাই plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
স্থির UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ধ্রুৱ ৱর্ণ *config, শূন্য *lib, ধ্রুৱ ৱর্ণ *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  যদি(U_SUCCESS(*status)) {
    ঘূৰাই plug;
  } অন্যথা {
    uplug_deallocatePlug(plug, status);
    ঘূৰাই nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, ধ্রুৱ ৱর্ণ *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  ঘূৰাই plug;
}

#যদি U_ENABLE_DYLOAD

স্থির UPlugData* 
uplug_initErrorPlug(ধ্রুৱ ৱর্ণ *libName, ধ্রুৱ ৱর্ণ *sym, ধ্রুৱ ৱর্ণ *config, ধ্রুৱ ৱর্ণ *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  যদি(U_FAILURE(*status)) ঘূৰাই nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = মিছা; /* Won't load. */
  plug->dontUnload = সঁচা; /* cannot unload. */

  যদি(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  যদি(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  যদি(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  যদি(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  ঘূৰাই plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
স্থির UPlugData* 
uplug_initPlugFromLibrary(ধ্রুৱ ৱর্ণ *libName, ধ্রুৱ ৱর্ণ *sym, ধ্রুৱ ৱর্ণ *config, UErrorCode *status) {
  শূন্য *lib = nullptr;
  UPlugData *plug = nullptr;
  যদি(U_FAILURE(*status)) { ঘূৰাই nullptr; }
  lib = uplug_openLibrary(libName, status);
  যদি(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    যদি(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      যদি(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } অন্যথা {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    যদি(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } অন্যথা {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  ঘূৰাই plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(ধ্রুৱ ৱর্ণ *libName, ধ্রুৱ ৱর্ণ *sym, ধ্রুৱ ৱর্ণ *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  যদি(U_FAILURE(*status)) { ঘূৰাই nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  ঘূৰাই plug;
}

#endif

স্থির UPlugLevel gCurrentLevel = UPLUG_LEVEL_নিচেৰ;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  ঘূৰাই gCurrentLevel;
}

স্থির UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  বাবে(i=0;i<plugin_গুনতি;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_নিচেৰ;
  ঘূৰাই সঁচা;
}

#যদি U_ENABLE_DYLOAD

স্থির শূন্য uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  যদি(U_FAILURE(*status)) {
    ঘূৰাই;
  }
#যদি UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  বাবে(i=0;i<plugin_গুনতি;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    যদি(pluginToLoad->awaitingLoad) {
      যদি(pluginToLoad->level == UPLUG_LEVEL_নিচেৰ) {
        যদি(currentLevel > UPLUG_LEVEL_নিচেৰ) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_উপৰেৰ;
        } অন্যথা {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          যদি(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = মিছা;
      } 
    }
  }    
  বাবে(i=0;i<plugin_গুনতি;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    যদি(pluginToLoad->awaitingLoad) {
      যদি(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } অন্যথা যদি(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } অন্যথা {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = মিছা;
    }
  }
    
#যদি UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
স্থির ৱর্ণ plugin_file[2048] = "";
#endif

U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
uplug_getPluginFile() {
#যদি U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  ঘূৰাই plugin_file;
#অন্যথা
  ঘূৰাই nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI শূন্য U_EXPORT2
uplug_init(UErrorCode *status) {
#যদি !U_ENABLE_DYLOAD
  (শূন্য)status; /* unused */
#নোচেদ্যদি !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ধ্রুৱ ৱর্ণ *env = getenv("ICU_PLUGINS");

  যদি(U_FAILURE(*status)) ঘূৰাই;
  যদি(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  যদি(U_FAILURE(*status)) ঘূৰাই;

#যদি defined(DEFAULT_ICU_PLUGINS) 
  যদি(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#যদি UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  যদি(!plugin_dir.isEmpty()) {
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
#অন্যথা
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#যদি UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    যদি(U_FAILURE(*status)) {
      ঘূৰাই;
    }
    যদি((size_t)pluginFile.length() > (পরিমাণ(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#যদি UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      ঘূৰাই;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, পরিমাণ(plugin_file), *status);

#যদি UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (পূর্ণাঙ্ক)strlen(plugin_file), (পূর্ণাঙ্ক)পরিমাণ(plugin_file)));
#endif
        
#ifdef __MVS__
    যদি (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    অন্যথা
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    যদি(f != nullptr) {
      ৱর্ণ linebuf[1024];
      ৱর্ণ *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t সাৰি = 0;
            
            
      যেতিয়ালৈ(fgets(linebuf,1023,f)) {
        সাৰি++;

        যদি(!*linebuf || *linebuf=='#') {
          অনুৱর্ত;
        } অন্যথা {
          p = linebuf;
          যেতিয়ালৈ(*p&&isspace((পূর্ণাঙ্ক)*p))
            p++;
          যদি(!*p || *p=='#') অনুৱর্ত;
          libName = p;
          যেতিয়ালৈ(*p&&!isspace((পূর্ণাঙ্ক)*p)) {
            p++;
          }
          যদি(!*p || *p=='#') অনুৱর্ত; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          যেতিয়ালৈ(*p&&isspace((পূর্ণাঙ্ক)*p)) {
            p++;
          }
          যদি(!*p||*p=='#') অনুৱর্ত; /* no symname after libname +tab */
          symName = p;
          যেতিয়ালৈ(*p&&!isspace((পূর্ণাঙ্ক)*p)) {
            p++;
          }
                    
          যদি(*p) { /* has config */
            *p=0;
            ++p;
            যেতিয়ালৈ(*p&&isspace((পূর্ণাঙ্ক)*p)) {
              p++;
            }
            যদি(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          যদি(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            যেতিয়ালৈ(p>config&&isspace((পূর্ণাঙ্ক)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            যদি(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#যদি UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (শূন্য*)plug, u_errorName(subStatus)));
#অন্যথা
            (শূন্য)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } অন্যথা {
#যদি UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_উপৰেৰ;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


