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

#अन्तर्भाव "unicode/icuplug.h"


#जइ UCONFIG_ENABLE_PLUGINS


#अन्तर्भाव "icuplugimp.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "ucln.h"
#अन्तर्भाव <stdio.h>
#ifdef __MVS__  /* defined by z/OS compiler */
#define _POSIX_SOURCE
#अन्तर्भाव <cics.h> /* 12 Nov 2011 JAM iscics() function */
#endif
#अन्तर्भाव "charstr.h"

using नामाकाश icu;

#ifndef UPLUG_TRACE
#define UPLUG_TRACE 0
#endif

#जइ UPLUG_TRACE
#अन्तर्भाव <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

संरचना UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  शून्य *lib;              /**< plugin library, or nullptr */
  वर्ण libName[UPLUG_NAME_MAX];   /**< library name */
  वर्ण sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  वर्ण config[UPLUG_NAME_MAX];     /**< configuration data */
  शून्य *context;          /**< user context data */
  वर्ण name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaitingLoad; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UErrorCode pluginStatus; /**< status code of plugin */
};



#define UPLUG_LIBRARY_INITIAL_गणनम् 8
#define UPLUG_PLUGIN_INITIAL_गणनम् 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
स्थिर int32_t uplug_removeEntryAt(शून्य *list, int32_t listSize, int32_t memberSize, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  जइ(listSize<1) {
    प्रत्यागम listSize;
  }
    
  /* is there anything to move? */
  जइ(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  प्रत्यागम listSize-1;
}




#जइ U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
संरचना UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
प्रकारनिर्देश संरचना UPlugLibrary {
  शून्य *lib;                           /**< library ptr */
  वर्ण name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

स्थिर UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_गणनम्];
स्थिर UPlugLibrary * libraryList = staticLibraryList;
स्थिर int32_t library_गणनम् = 0;
स्थिर int32_t libraryMax = UPLUG_LIBRARY_INITIAL_गणनम्;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
स्थिर int32_t अन्वेषणम्_For_Library_Name(ध्रुव वर्ण *libName) {
  int32_t i;
    
  प्रति(i=0;i<library_गणनम्;i++) {
    जइ(!uprv_strcmp(libName, libraryList[i].name)) {
      प्रत्यागम i;
    }
  }
  प्रत्यागम -1;
}

स्थिर int32_t अन्वेषणम्_For_Library(शून्य *lib) {
  int32_t i;
    
  प्रति(i=0;i<library_गणनम्;i++) {
    जइ(lib==libraryList[i].lib) {
      प्रत्यागम i;
    }
  }
  प्रत्यागम -1;
}

U_CAPI वर्ण * U_EXPORT2
uplug_findLibrary(शून्य *lib, UErrorCode *status) {
  int32_t libEnt;
  वर्ण *ret = nullptr;
  जइ(U_FAILURE(*status)) {
    प्रत्यागम nullptr;
  }
  libEnt = अन्वेषणम्_For_Library(lib);
  जइ(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } अण्णहा {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  प्रत्यागम ret;
}

U_CAPI शून्य * U_EXPORT2
uplug_openLibrary(ध्रुव वर्ण *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  शून्य *lib = nullptr;
    
  जइ(U_FAILURE(*status)) प्रत्यागम nullptr;

  libEntry = अन्वेषणम्_For_Library_Name(libName);
  जइ(libEntry == -1) {
    libEntry = library_गणनम्++;
    जइ(library_गणनम् >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#जइ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      प्रत्यागम nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#जइ UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    जइ(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#जइ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      library_गणनम्--;
    } अण्णहा { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } अण्णहा {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  प्रत्यागम lib;
}

U_CAPI शून्य U_EXPORT2
uplug_closeLibrary(शून्य *lib, UErrorCode *status) {
  int32_t i;
    
#जइ UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (शून्य*)libraryList));
#endif
  जइ(U_FAILURE(*status)) प्रत्यागम;
    
  प्रति(i=0;i<library_गणनम्;i++) {
    जइ(lib==libraryList[i].lib) {
      जइ(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_गणनम् = uplug_removeEntryAt(libraryList, library_गणनम्, परिमाण(*libraryList), i);
      }
      प्रत्यागम;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

स्थिर UPlugData pluginList[UPLUG_PLUGIN_INITIAL_गणनम्];
स्थिर int32_t plugin_गणनम् = 0;



  
स्थिर int32_t uplug_plugin_सङ्ख्या(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[plugin_गणनम्];
  जइ(d<=pluginList) {
    प्रत्यागम 0;
  } अण्णहा जइ(d>=pastPlug) {
    प्रत्यागम plugin_गणनम्;
  } अण्णहा {
    प्रत्यागम (d-pluginList)/परिमाण(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  जइ(prior==nullptr) {
    प्रत्यागम pluginList;
  } अण्णहा {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[plugin_गणनम्];
    
    जइ(nextPlug>=pastPlug) {
      प्रत्यागम nullptr;
    } अण्णहा {
      प्रत्यागम nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
स्थिर शून्य uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  जइ(plug==nullptr||U_FAILURE(*status)) {
    प्रत्यागम;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  जइ(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


स्थिर शून्य uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  जइ(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    प्रत्यागम; 
  }
  जइ(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

स्थिर शून्य uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  जइ(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    प्रत्यागम; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  जइ(U_SUCCESS(*status)) { 
    जइ(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = मिच्छ;
    }
  } अण्णहा {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = मिच्छ;
  }
}


स्थिर शून्य uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  जइ(U_FAILURE(*status)) {
    प्रत्यागम;
  }
  जइ(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_नीचः) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    प्रत्यागम;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = मिच्छ;
  जइ(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

स्थिर UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  जइ(U_FAILURE(*status)) {
    प्रत्यागम nullptr;
  }

  जइ(plugin_गणनम् == UPLUG_PLUGIN_INITIAL_गणनम्) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    प्रत्यागम nullptr;
  }

  plug = &pluginList[plugin_गणनम्++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = परिमाण(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = सच्च;
  plug->dontUnload = मिच्छ;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  प्रत्यागम plug;
}

स्थिर UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ध्रुव वर्ण *config, शून्य *lib, ध्रुव वर्ण *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  जइ(U_FAILURE(*status)) {
    प्रत्यागम nullptr;
  }

  जइ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } अण्णहा {
    plug->config[0] = 0;
  }
    
  जइ(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } अण्णहा {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  प्रत्यागम plug;
}

स्थिर शून्य uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  जइ(!plug->dontUnload) {
#जइ U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  जइ(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  जइ(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_गणनम् = uplug_removeEntryAt(pluginList, plugin_गणनम्, परिमाण(plug[0]), uplug_plugin_सङ्ख्या(plug));
  } अण्णहा {
    /* not ok- leave as a message. */
    plug->awaitingLoad=मिच्छ;
    plug->entrypoint=0;
    plug->dontUnload=सच्च;
  }
}

स्थिर शून्य uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  जइ(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI शून्य U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  जइ(U_FAILURE(*status)) प्रत्यागम;
    
  प्रति(cursor=pluginList;cursor!=nullptr;) {
    जइ(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } अण्णहा {
      cursor = uplug_nextPlug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI शून्य U_EXPORT2 
uplug_setPlugNoUnload(UPlugData *data, UBool dontUnload)
{
  data->dontUnload = dontUnload;
}


U_CAPI शून्य U_EXPORT2
uplug_setPlugLevel(UPlugData *data, UPlugLevel level) {
  data->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlugData *data) {
  प्रत्यागम data->level;
}


U_CAPI शून्य U_EXPORT2
uplug_setPlugName(UPlugData *data, ध्रुव वर्ण *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI ध्रुव वर्ण* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  प्रत्यागम data->name;
}


U_CAPI ध्रुव वर्ण* U_EXPORT2
uplug_get_चिह्नम्_Name(UPlugData* data U_LIFETIME_BOUND) {
  प्रत्यागम data->sym;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  जइ(data->libName[0]) {
    प्रत्यागम data->libName;
  } अण्णहा {
#जइ U_ENABLE_DYLOAD
    प्रत्यागम uplug_findLibrary(data->lib, status);
#अण्णहा
    प्रत्यागम nullptr;
#endif
  }
}

U_CAPI शून्य * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  प्रत्यागम data->lib;
}

U_CAPI शून्य * U_EXPORT2
uplug_getContext(UPlugData *data) {
  प्रत्यागम data->context;
}


U_CAPI शून्य U_EXPORT2
uplug_setContext(UPlugData *data, शून्य *context) {
  data->context = context;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  प्रत्यागम data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  जइ(n <0 || n >= plugin_गणनम्) {
    प्रत्यागम nullptr;
  } अण्णहा { 
    प्रत्यागम &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  प्रत्यागम plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
स्थिर UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ध्रुव वर्ण *config, शून्य *lib, ध्रुव वर्ण *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  जइ(U_SUCCESS(*status)) {
    प्रत्यागम plug;
  } अण्णहा {
    uplug_deallocatePlug(plug, status);
    प्रत्यागम nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, ध्रुव वर्ण *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  प्रत्यागम plug;
}

#जइ U_ENABLE_DYLOAD

स्थिर UPlugData* 
uplug_initErrorPlug(ध्रुव वर्ण *libName, ध्रुव वर्ण *sym, ध्रुव वर्ण *config, ध्रुव वर्ण *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  जइ(U_FAILURE(*status)) प्रत्यागम nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = मिच्छ; /* Won't load. */
  plug->dontUnload = सच्च; /* cannot unload. */

  जइ(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  जइ(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  जइ(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  जइ(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  प्रत्यागम plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
स्थिर UPlugData* 
uplug_initPlugFromLibrary(ध्रुव वर्ण *libName, ध्रुव वर्ण *sym, ध्रुव वर्ण *config, UErrorCode *status) {
  शून्य *lib = nullptr;
  UPlugData *plug = nullptr;
  जइ(U_FAILURE(*status)) { प्रत्यागम nullptr; }
  lib = uplug_openLibrary(libName, status);
  जइ(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    जइ(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      जइ(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } अण्णहा {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    जइ(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } अण्णहा {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  प्रत्यागम plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(ध्रुव वर्ण *libName, ध्रुव वर्ण *sym, ध्रुव वर्ण *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  जइ(U_FAILURE(*status)) { प्रत्यागम nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  प्रत्यागम plug;
}

#endif

स्थिर UPlugLevel gCurrentLevel = UPLUG_LEVEL_नीचः;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  प्रत्यागम gCurrentLevel;
}

स्थिर UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  प्रति(i=0;i<plugin_गणनम्;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_नीचः;
  प्रत्यागम सच्च;
}

#जइ U_ENABLE_DYLOAD

स्थिर शून्य uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  जइ(U_FAILURE(*status)) {
    प्रत्यागम;
  }
#जइ UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  प्रति(i=0;i<plugin_गणनम्;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    जइ(pluginToLoad->awaitingLoad) {
      जइ(pluginToLoad->level == UPLUG_LEVEL_नीचः) {
        जइ(currentLevel > UPLUG_LEVEL_नीचः) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_उच्चः;
        } अण्णहा {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          जइ(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = मिच्छ;
      } 
    }
  }    
  प्रति(i=0;i<plugin_गणनम्;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    जइ(pluginToLoad->awaitingLoad) {
      जइ(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } अण्णहा जइ(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } अण्णहा {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = मिच्छ;
    }
  }
    
#जइ UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
स्थिर वर्ण plugin_file[2048] = "";
#endif

U_CAPI ध्रुव वर्ण* U_EXPORT2
uplug_getPluginFile() {
#जइ U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  प्रत्यागम plugin_file;
#अण्णहा
  प्रत्यागम nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI शून्य U_EXPORT2
uplug_init(UErrorCode *status) {
#जइ !U_ENABLE_DYLOAD
  (शून्य)status; /* unused */
#नोचेद्यदि !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ध्रुव वर्ण *env = getenv("ICU_PLUGINS");

  जइ(U_FAILURE(*status)) प्रत्यागम;
  जइ(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  जइ(U_FAILURE(*status)) प्रत्यागम;

#जइ defined(DEFAULT_ICU_PLUGINS) 
  जइ(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#जइ UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  जइ(!plugin_dir.isEmpty()) {
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
#अण्णहा
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#जइ UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    जइ(U_FAILURE(*status)) {
      प्रत्यागम;
    }
    जइ((size_t)pluginFile.length() > (परिमाण(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#जइ UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      प्रत्यागम;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, परिमाण(plugin_file), *status);

#जइ UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (पूर्णाङ्क)strlen(plugin_file), (पूर्णाङ्क)परिमाण(plugin_file)));
#endif
        
#ifdef __MVS__
    जइ (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    अण्णहा
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    जइ(f != nullptr) {
      वर्ण linebuf[1024];
      वर्ण *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t पङ्क्तिः = 0;
            
            
      जाव(fgets(linebuf,1023,f)) {
        पङ्क्तिः++;

        जइ(!*linebuf || *linebuf=='#') {
          अनुवर्त;
        } अण्णहा {
          p = linebuf;
          जाव(*p&&isspace((पूर्णाङ्क)*p))
            p++;
          जइ(!*p || *p=='#') अनुवर्त;
          libName = p;
          जाव(*p&&!isspace((पूर्णाङ्क)*p)) {
            p++;
          }
          जइ(!*p || *p=='#') अनुवर्त; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          जाव(*p&&isspace((पूर्णाङ्क)*p)) {
            p++;
          }
          जइ(!*p||*p=='#') अनुवर्त; /* no symname after libname +tab */
          symName = p;
          जाव(*p&&!isspace((पूर्णाङ्क)*p)) {
            p++;
          }
                    
          जइ(*p) { /* has config */
            *p=0;
            ++p;
            जाव(*p&&isspace((पूर्णाङ्क)*p)) {
              p++;
            }
            जइ(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          जइ(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            जाव(p>config&&isspace((पूर्णाङ्क)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            जइ(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#जइ UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (शून्य*)plug, u_errorName(subStatus)));
#अण्णहा
            (शून्य)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } अण्णहा {
#जइ UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_उच्चः;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


