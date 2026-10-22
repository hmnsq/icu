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


#जर UCONFIG_ENABLE_PLUGINS


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

#जर UPLUG_TRACE
#अन्तर्भाव <stdio.h>
#define DBG(x) ख_लिखो(मानक_त्रुटि, "%s:%d: ",__FILE__,__LINE__); ख_लिखो x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

संरचना UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  शून्य *lib;              /**< plugin library, or nullptr */
  अक्षर libName[UPLUG_NAME_MAX];   /**< library name */
  अक्षर sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  अक्षर config[UPLUG_NAME_MAX];     /**< configuration data */
  शून्य *context;          /**< user context data */
  अक्षर name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaitingLoad; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UErrorCode pluginStatus; /**< status code of plugin */
};



#define UPLUG_LIBRARY_INITIAL_मोजणी 8
#define UPLUG_PLUGIN_INITIAL_मोजणी 12

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
  जर(listSize<1) {
    परत listSize;
  }
    
  /* is there anything to move? */
  जर(listSize > itemToRemove+1) {
    स_हटाओ(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  परत listSize-1;
}




#जर U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
संरचना UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
प्रकारव्याख्या संरचना UPlugLibrary {
  शून्य *lib;                           /**< library ptr */
  अक्षर name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

स्थिर UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_मोजणी];
स्थिर UPlugLibrary * libraryList = staticLibraryList;
स्थिर int32_t library_मोजणी = 0;
स्थिर int32_t libraryMax = UPLUG_LIBRARY_INITIAL_मोजणी;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
स्थिर int32_t शोध_For_Library_Name(स्थिरांक अक्षर *libName) {
  int32_t i;
    
  साठी(i=0;i<library_मोजणी;i++) {
    जर(!uprv_strcmp(libName, libraryList[i].name)) {
      परत i;
    }
  }
  परत -1;
}

स्थिर int32_t शोध_For_Library(शून्य *lib) {
  int32_t i;
    
  साठी(i=0;i<library_मोजणी;i++) {
    जर(lib==libraryList[i].lib) {
      परत i;
    }
  }
  परत -1;
}

U_CAPI अक्षर * U_EXPORT2
uplug_findLibrary(शून्य *lib, UErrorCode *status) {
  int32_t libEnt;
  अक्षर *ret = nullptr;
  जर(U_FAILURE(*status)) {
    परत nullptr;
  }
  libEnt = शोध_For_Library(lib);
  जर(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } अन्यथा {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  परत ret;
}

U_CAPI शून्य * U_EXPORT2
uplug_openLibrary(स्थिरांक अक्षर *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  शून्य *lib = nullptr;
    
  जर(U_FAILURE(*status)) परत nullptr;

  libEntry = शोध_For_Library_Name(libName);
  जर(libEntry == -1) {
    libEntry = library_मोजणी++;
    जर(library_मोजणी >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#जर UPLUG_TRACE
      DBG((मानक_त्रुटि, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      परत nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#जर UPLUG_TRACE
    DBG((मानक_त्रुटि, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    जर(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#जर UPLUG_TRACE
      DBG((मानक_त्रुटि, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      library_मोजणी--;
    } अन्यथा { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } अन्यथा {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  परत lib;
}

U_CAPI शून्य U_EXPORT2
uplug_closeLibrary(शून्य *lib, UErrorCode *status) {
  int32_t i;
    
#जर UPLUG_TRACE
  DBG((मानक_त्रुटि, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (शून्य*)libraryList));
#endif
  जर(U_FAILURE(*status)) परत;
    
  साठी(i=0;i<library_मोजणी;i++) {
    जर(lib==libraryList[i].lib) {
      जर(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_मोजणी = uplug_removeEntryAt(libraryList, library_मोजणी, आकार(*libraryList), i);
      }
      परत;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

स्थिर UPlugData pluginList[UPLUG_PLUGIN_INITIAL_मोजणी];
स्थिर int32_t plugin_मोजणी = 0;



  
स्थिर int32_t uplug_plugin_संख्या(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[plugin_मोजणी];
  जर(d<=pluginList) {
    परत 0;
  } अन्यथा जर(d>=pastPlug) {
    परत plugin_मोजणी;
  } अन्यथा {
    परत (d-pluginList)/आकार(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  जर(prior==nullptr) {
    परत pluginList;
  } अन्यथा {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[plugin_मोजणी];
    
    जर(nextPlug>=pastPlug) {
      परत nullptr;
    } अन्यथा {
      परत nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
स्थिर शून्य uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  जर(plug==nullptr||U_FAILURE(*status)) {
    परत;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  जर(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


स्थिर शून्य uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  जर(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    परत; 
  }
  जर(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

स्थिर शून्य uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  जर(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    परत; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  जर(U_SUCCESS(*status)) { 
    जर(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = असत्य;
    }
  } अन्यथा {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = असत्य;
  }
}


स्थिर शून्य uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  जर(U_FAILURE(*status)) {
    परत;
  }
  जर(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_खालचा) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    परत;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = असत्य;
  जर(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

स्थिर UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  जर(U_FAILURE(*status)) {
    परत nullptr;
  }

  जर(plugin_मोजणी == UPLUG_PLUGIN_INITIAL_मोजणी) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    परत nullptr;
  }

  plug = &pluginList[plugin_मोजणी++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = आकार(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = सत्य;
  plug->dontUnload = असत्य;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  परत plug;
}

स्थिर UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, स्थिरांक अक्षर *config, शून्य *lib, स्थिरांक अक्षर *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  जर(U_FAILURE(*status)) {
    परत nullptr;
  }

  जर(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } अन्यथा {
    plug->config[0] = 0;
  }
    
  जर(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } अन्यथा {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  परत plug;
}

स्थिर शून्य uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  जर(!plug->dontUnload) {
#जर U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  जर(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  जर(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_मोजणी = uplug_removeEntryAt(pluginList, plugin_मोजणी, आकार(plug[0]), uplug_plugin_संख्या(plug));
  } अन्यथा {
    /* not ok- leave as a message. */
    plug->awaitingLoad=असत्य;
    plug->entrypoint=0;
    plug->dontUnload=सत्य;
  }
}

स्थिर शून्य uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  जर(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI शून्य U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  जर(U_FAILURE(*status)) परत;
    
  साठी(cursor=pluginList;cursor!=nullptr;) {
    जर(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } अन्यथा {
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
  परत data->level;
}


U_CAPI शून्य U_EXPORT2
uplug_setPlugName(UPlugData *data, स्थिरांक अक्षर *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI स्थिरांक अक्षर* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  परत data->name;
}


U_CAPI स्थिरांक अक्षर* U_EXPORT2
uplug_get_चिन्ह_Name(UPlugData* data U_LIFETIME_BOUND) {
  परत data->sym;
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  जर(data->libName[0]) {
    परत data->libName;
  } अन्यथा {
#जर U_ENABLE_DYLOAD
    परत uplug_findLibrary(data->lib, status);
#अन्यथा
    परत nullptr;
#endif
  }
}

U_CAPI शून्य * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  परत data->lib;
}

U_CAPI शून्य * U_EXPORT2
uplug_getContext(UPlugData *data) {
  परत data->context;
}


U_CAPI शून्य U_EXPORT2
uplug_setContext(UPlugData *data, शून्य *context) {
  data->context = context;
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  परत data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  जर(n <0 || n >= plugin_मोजणी) {
    परत nullptr;
  } अन्यथा { 
    परत &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  परत plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
स्थिर UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, स्थिरांक अक्षर *config, शून्य *lib, स्थिरांक अक्षर *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  जर(U_SUCCESS(*status)) {
    परत plug;
  } अन्यथा {
    uplug_deallocatePlug(plug, status);
    परत nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, स्थिरांक अक्षर *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  परत plug;
}

#जर U_ENABLE_DYLOAD

स्थिर UPlugData* 
uplug_initErrorPlug(स्थिरांक अक्षर *libName, स्थिरांक अक्षर *sym, स्थिरांक अक्षर *config, स्थिरांक अक्षर *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  जर(U_FAILURE(*status)) परत nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = असत्य; /* Won't load. */
  plug->dontUnload = सत्य; /* cannot unload. */

  जर(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  जर(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  जर(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  जर(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  परत plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
स्थिर UPlugData* 
uplug_initPlugFromLibrary(स्थिरांक अक्षर *libName, स्थिरांक अक्षर *sym, स्थिरांक अक्षर *config, UErrorCode *status) {
  शून्य *lib = nullptr;
  UPlugData *plug = nullptr;
  जर(U_FAILURE(*status)) { परत nullptr; }
  lib = uplug_openLibrary(libName, status);
  जर(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    जर(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      जर(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } अन्यथा {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    जर(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } अन्यथा {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  परत plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(स्थिरांक अक्षर *libName, स्थिरांक अक्षर *sym, स्थिरांक अक्षर *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  जर(U_FAILURE(*status)) { परत nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  परत plug;
}

#endif

स्थिर UPlugLevel gCurrentLevel = UPLUG_LEVEL_खालचा;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  परत gCurrentLevel;
}

स्थिर UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  साठी(i=0;i<plugin_मोजणी;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_खालचा;
  परत सत्य;
}

#जर U_ENABLE_DYLOAD

स्थिर शून्य uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  जर(U_FAILURE(*status)) {
    परत;
  }
#जर UPLUG_TRACE
  DBG((मानक_त्रुटि,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  साठी(i=0;i<plugin_मोजणी;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    जर(pluginToLoad->awaitingLoad) {
      जर(pluginToLoad->level == UPLUG_LEVEL_खालचा) {
        जर(currentLevel > UPLUG_LEVEL_खालचा) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_वरचा;
        } अन्यथा {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          जर(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = असत्य;
      } 
    }
  }    
  साठी(i=0;i<plugin_मोजणी;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    जर(pluginToLoad->awaitingLoad) {
      जर(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } अन्यथा जर(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } अन्यथा {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = असत्य;
    }
  }
    
#जर UPLUG_TRACE
  DBG((मानक_त्रुटि,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
स्थिर अक्षर plugin_file[2048] = "";
#endif

U_CAPI स्थिरांक अक्षर* U_EXPORT2
uplug_getPluginFile() {
#जर U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  परत plugin_file;
#अन्यथा
  परत nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI शून्य U_EXPORT2
uplug_init(UErrorCode *status) {
#जर !U_ENABLE_DYLOAD
  (शून्य)status; /* unused */
#किंवाजर !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  स्थिरांक अक्षर *env = दो_पर्या("ICU_PLUGINS");

  जर(U_FAILURE(*status)) परत;
  जर(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  जर(U_FAILURE(*status)) परत;

#जर defined(DEFAULT_ICU_PLUGINS) 
  जर(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#जर UPLUG_TRACE
  DBG((मानक_त्रुटि, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  जर(!plugin_dir.isEmpty()) {
    खाता *f;
        
    CharString pluginFile;
#ifdef ICU_PLUGINS_DD
/* There are potentially a lot of ways to implement a plugin directory on OS390/zOS  */
/* Keeping in mind that unauthorized file access is logged, monitored, and enforced  */
/* I've chosen to open a DDNAME if BATCH and leave it alone for (presumably) UNIX    */
/* System Services.  Alternative techniques might be allocating a member in          */
/* SYS1.PARMLIB or setting an environment variable "ICU_PLUGIN_PATH" (?).  The       */
/* DDNAME can be connected to a file in the HFS if need be.                          */

    pluginFile.append("//DD:ICUPLUG", -1, *status);        /* JAM 20 Oct 2011 */
#अन्यथा
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#जर UPLUG_TRACE
    DBG((मानक_त्रुटि, "status=%s\n", u_errorName(*status)));
#endif

    जर(U_FAILURE(*status)) {
      परत;
    }
    जर((माप_प्रकार)pluginFile.length() > (आकार(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#जर UPLUG_TRACE
      DBG((मानक_त्रुटि, "status=%s\n", u_errorName(*status)));
#endif
      परत;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, आकार(plugin_file), *status);

#जर UPLUG_TRACE
    DBG((मानक_त्रुटि, "pluginfile= %s len %d/%d\n", plugin_file, (पूर्णांक)म_माप(plugin_file), (पूर्णांक)आकार(plugin_file)));
#endif
        
#ifdef __MVS__
    जर (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    अन्यथा
#endif
    {
        f = ख_खोलो(pluginFile.data(), "r");
    }

    जर(f != nullptr) {
      अक्षर linebuf[1024];
      अक्षर *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t ओळ = 0;
            
            
      पर्यंत(ख_माला_लो(linebuf,1023,f)) {
        ओळ++;

        जर(!*linebuf || *linebuf=='#') {
          सुरूठेवा;
        } अन्यथा {
          p = linebuf;
          पर्यंत(*p&&है_खाली((पूर्णांक)*p))
            p++;
          जर(!*p || *p=='#') सुरूठेवा;
          libName = p;
          पर्यंत(*p&&!है_खाली((पूर्णांक)*p)) {
            p++;
          }
          जर(!*p || *p=='#') सुरूठेवा; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          पर्यंत(*p&&है_खाली((पूर्णांक)*p)) {
            p++;
          }
          जर(!*p||*p=='#') सुरूठेवा; /* no symname after libname +tab */
          symName = p;
          पर्यंत(*p&&!है_खाली((पूर्णांक)*p)) {
            p++;
          }
                    
          जर(*p) { /* has config */
            *p=0;
            ++p;
            पर्यंत(*p&&है_खाली((पूर्णांक)*p)) {
              p++;
            }
            जर(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          जर(config!=nullptr&&*config!=0) {
            p = config+म_माप(config);
            पर्यंत(p>config&&है_खाली((पूर्णांक)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            जर(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#जर UPLUG_TRACE
            DBG((मानक_त्रुटि, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((मानक_त्रुटि, " -> %p, %s\n", (शून्य*)plug, u_errorName(subStatus)));
#अन्यथा
            (शून्य)plug; /* unused */
#endif
          }
        }
      }
      ख_बंद(f);
    } अन्यथा {
#जर UPLUG_TRACE
      DBG((मानक_त्रुटि, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_वरचा;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


