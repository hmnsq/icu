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


#यदि UCONFIG_ENABLE_PLUGINS


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

#यदि UPLUG_TRACE
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



#define UPLUG_LIBRARY_INITIAL_गन्ती 8
#define UPLUG_PLUGIN_INITIAL_गन्ती 12

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
  यदि(listSize<1) {
    फर्काऊ listSize;
  }
    
  /* is there anything to move? */
  यदि(listSize > itemToRemove+1) {
    स_हटाओ(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  फर्काऊ listSize-1;
}




#यदि U_ENABLE_DYLOAD
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

स्थिर UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_गन्ती];
स्थिर UPlugLibrary * libraryList = staticLibraryList;
स्थिर int32_t library_गन्ती = 0;
स्थिर int32_t libraryMax = UPLUG_LIBRARY_INITIAL_गन्ती;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
स्थिर int32_t खोज_For_Library_Name(अचर वर्ण *libName) {
  int32_t i;
    
  लागि(i=0;i<library_गन्ती;i++) {
    यदि(!uprv_strcmp(libName, libraryList[i].name)) {
      फर्काऊ i;
    }
  }
  फर्काऊ -1;
}

स्थिर int32_t खोज_For_Library(शून्य *lib) {
  int32_t i;
    
  लागि(i=0;i<library_गन्ती;i++) {
    यदि(lib==libraryList[i].lib) {
      फर्काऊ i;
    }
  }
  फर्काऊ -1;
}

U_CAPI वर्ण * U_EXPORT2
uplug_findLibrary(शून्य *lib, UErrorCode *status) {
  int32_t libEnt;
  वर्ण *ret = nullptr;
  यदि(U_FAILURE(*status)) {
    फर्काऊ nullptr;
  }
  libEnt = खोज_For_Library(lib);
  यदि(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } अन्यथा {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  फर्काऊ ret;
}

U_CAPI शून्य * U_EXPORT2
uplug_openLibrary(अचर वर्ण *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  शून्य *lib = nullptr;
    
  यदि(U_FAILURE(*status)) फर्काऊ nullptr;

  libEntry = खोज_For_Library_Name(libName);
  यदि(libEntry == -1) {
    libEntry = library_गन्ती++;
    यदि(library_गन्ती >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#यदि UPLUG_TRACE
      DBG((मानक_त्रुटि, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      फर्काऊ nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#यदि UPLUG_TRACE
    DBG((मानक_त्रुटि, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    यदि(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#यदि UPLUG_TRACE
      DBG((मानक_त्रुटि, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      library_गन्ती--;
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
  फर्काऊ lib;
}

U_CAPI शून्य U_EXPORT2
uplug_closeLibrary(शून्य *lib, UErrorCode *status) {
  int32_t i;
    
#यदि UPLUG_TRACE
  DBG((मानक_त्रुटि, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (शून्य*)libraryList));
#endif
  यदि(U_FAILURE(*status)) फर्काऊ;
    
  लागि(i=0;i<library_गन्ती;i++) {
    यदि(lib==libraryList[i].lib) {
      यदि(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_गन्ती = uplug_removeEntryAt(libraryList, library_गन्ती, परिमाण(*libraryList), i);
      }
      फर्काऊ;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

स्थिर UPlugData pluginList[UPLUG_PLUGIN_INITIAL_गन्ती];
स्थिर int32_t plugin_गन्ती = 0;



  
स्थिर int32_t uplug_plugin_सङ्ख्या(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[plugin_गन्ती];
  यदि(d<=pluginList) {
    फर्काऊ 0;
  } अन्यथा यदि(d>=pastPlug) {
    फर्काऊ plugin_गन्ती;
  } अन्यथा {
    फर्काऊ (d-pluginList)/परिमाण(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  यदि(prior==nullptr) {
    फर्काऊ pluginList;
  } अन्यथा {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[plugin_गन्ती];
    
    यदि(nextPlug>=pastPlug) {
      फर्काऊ nullptr;
    } अन्यथा {
      फर्काऊ nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
स्थिर शून्य uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  यदि(plug==nullptr||U_FAILURE(*status)) {
    फर्काऊ;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  यदि(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


स्थिर शून्य uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  यदि(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    फर्काऊ; 
  }
  यदि(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

स्थिर शून्य uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  यदि(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    फर्काऊ; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  यदि(U_SUCCESS(*status)) { 
    यदि(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = असत्य;
    }
  } अन्यथा {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = असत्य;
  }
}


स्थिर शून्य uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  यदि(U_FAILURE(*status)) {
    फर्काऊ;
  }
  यदि(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_तल्लो) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    फर्काऊ;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = असत्य;
  यदि(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

स्थिर UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  यदि(U_FAILURE(*status)) {
    फर्काऊ nullptr;
  }

  यदि(plugin_गन्ती == UPLUG_PLUGIN_INITIAL_गन्ती) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    फर्काऊ nullptr;
  }

  plug = &pluginList[plugin_गन्ती++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = परिमाण(UPlugData);
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


  फर्काऊ plug;
}

स्थिर UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, अचर वर्ण *config, शून्य *lib, अचर वर्ण *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  यदि(U_FAILURE(*status)) {
    फर्काऊ nullptr;
  }

  यदि(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } अन्यथा {
    plug->config[0] = 0;
  }
    
  यदि(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } अन्यथा {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  फर्काऊ plug;
}

स्थिर शून्य uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  यदि(!plug->dontUnload) {
#यदि U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  यदि(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  यदि(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_गन्ती = uplug_removeEntryAt(pluginList, plugin_गन्ती, परिमाण(plug[0]), uplug_plugin_सङ्ख्या(plug));
  } अन्यथा {
    /* not ok- leave as a message. */
    plug->awaitingLoad=असत्य;
    plug->entrypoint=0;
    plug->dontUnload=सत्य;
  }
}

स्थिर शून्य uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  यदि(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI शून्य U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  यदि(U_FAILURE(*status)) फर्काऊ;
    
  लागि(cursor=pluginList;cursor!=nullptr;) {
    यदि(cursor==plug) {
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
  फर्काऊ data->level;
}


U_CAPI शून्य U_EXPORT2
uplug_setPlugName(UPlugData *data, अचर वर्ण *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI अचर वर्ण* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  फर्काऊ data->name;
}


U_CAPI अचर वर्ण* U_EXPORT2
uplug_get_चिह्न_Name(UPlugData* data U_LIFETIME_BOUND) {
  फर्काऊ data->sym;
}

U_CAPI अचर वर्ण* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  यदि(data->libName[0]) {
    फर्काऊ data->libName;
  } अन्यथा {
#यदि U_ENABLE_DYLOAD
    फर्काऊ uplug_findLibrary(data->lib, status);
#अन्यथा
    फर्काऊ nullptr;
#endif
  }
}

U_CAPI शून्य * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  फर्काऊ data->lib;
}

U_CAPI शून्य * U_EXPORT2
uplug_getContext(UPlugData *data) {
  फर्काऊ data->context;
}


U_CAPI शून्य U_EXPORT2
uplug_setContext(UPlugData *data, शून्य *context) {
  data->context = context;
}

U_CAPI अचर वर्ण* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  फर्काऊ data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  यदि(n <0 || n >= plugin_गन्ती) {
    फर्काऊ nullptr;
  } अन्यथा { 
    फर्काऊ &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  फर्काऊ plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
स्थिर UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, अचर वर्ण *config, शून्य *lib, अचर वर्ण *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  यदि(U_SUCCESS(*status)) {
    फर्काऊ plug;
  } अन्यथा {
    uplug_deallocatePlug(plug, status);
    फर्काऊ nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, अचर वर्ण *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  फर्काऊ plug;
}

#यदि U_ENABLE_DYLOAD

स्थिर UPlugData* 
uplug_initErrorPlug(अचर वर्ण *libName, अचर वर्ण *sym, अचर वर्ण *config, अचर वर्ण *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  यदि(U_FAILURE(*status)) फर्काऊ nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = असत्य; /* Won't load. */
  plug->dontUnload = सत्य; /* cannot unload. */

  यदि(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  यदि(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  यदि(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  यदि(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  फर्काऊ plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
स्थिर UPlugData* 
uplug_initPlugFromLibrary(अचर वर्ण *libName, अचर वर्ण *sym, अचर वर्ण *config, UErrorCode *status) {
  शून्य *lib = nullptr;
  UPlugData *plug = nullptr;
  यदि(U_FAILURE(*status)) { फर्काऊ nullptr; }
  lib = uplug_openLibrary(libName, status);
  यदि(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    यदि(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      यदि(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } अन्यथा {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    यदि(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } अन्यथा {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  फर्काऊ plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(अचर वर्ण *libName, अचर वर्ण *sym, अचर वर्ण *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  यदि(U_FAILURE(*status)) { फर्काऊ nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  फर्काऊ plug;
}

#endif

स्थिर UPlugLevel gCurrentLevel = UPLUG_LEVEL_तल्लो;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  फर्काऊ gCurrentLevel;
}

स्थिर UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  लागि(i=0;i<plugin_गन्ती;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_तल्लो;
  फर्काऊ सत्य;
}

#यदि U_ENABLE_DYLOAD

स्थिर शून्य uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  यदि(U_FAILURE(*status)) {
    फर्काऊ;
  }
#यदि UPLUG_TRACE
  DBG((मानक_त्रुटि,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  लागि(i=0;i<plugin_गन्ती;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    यदि(pluginToLoad->awaitingLoad) {
      यदि(pluginToLoad->level == UPLUG_LEVEL_तल्लो) {
        यदि(currentLevel > UPLUG_LEVEL_तल्लो) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_माथिल्लो;
        } अन्यथा {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          यदि(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = असत्य;
      } 
    }
  }    
  लागि(i=0;i<plugin_गन्ती;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    यदि(pluginToLoad->awaitingLoad) {
      यदि(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } अन्यथा यदि(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } अन्यथा {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = असत्य;
    }
  }
    
#यदि UPLUG_TRACE
  DBG((मानक_त्रुटि,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
स्थिर वर्ण plugin_file[2048] = "";
#endif

U_CAPI अचर वर्ण* U_EXPORT2
uplug_getPluginFile() {
#यदि U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  फर्काऊ plugin_file;
#अन्यथा
  फर्काऊ nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI शून्य U_EXPORT2
uplug_init(UErrorCode *status) {
#यदि !U_ENABLE_DYLOAD
  (शून्य)status; /* unused */
#अथवायदि !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  अचर वर्ण *env = दो_पर्या("ICU_PLUGINS");

  यदि(U_FAILURE(*status)) फर्काऊ;
  यदि(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  यदि(U_FAILURE(*status)) फर्काऊ;

#यदि defined(DEFAULT_ICU_PLUGINS) 
  यदि(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#यदि UPLUG_TRACE
  DBG((मानक_त्रुटि, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  यदि(!plugin_dir.isEmpty()) {
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

#यदि UPLUG_TRACE
    DBG((मानक_त्रुटि, "status=%s\n", u_errorName(*status)));
#endif

    यदि(U_FAILURE(*status)) {
      फर्काऊ;
    }
    यदि((माप_प्रकार)pluginFile.length() > (परिमाण(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#यदि UPLUG_TRACE
      DBG((मानक_त्रुटि, "status=%s\n", u_errorName(*status)));
#endif
      फर्काऊ;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, परिमाण(plugin_file), *status);

#यदि UPLUG_TRACE
    DBG((मानक_त्रुटि, "pluginfile= %s len %d/%d\n", plugin_file, (पूर्णाङ्क)म_माप(plugin_file), (पूर्णाङ्क)परिमाण(plugin_file)));
#endif
        
#ifdef __MVS__
    यदि (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    अन्यथा
#endif
    {
        f = ख_खोलो(pluginFile.data(), "r");
    }

    यदि(f != nullptr) {
      वर्ण linebuf[1024];
      वर्ण *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t पङ्क्ति = 0;
            
            
      जबसम्म(ख_माला_लो(linebuf,1023,f)) {
        पङ्क्ति++;

        यदि(!*linebuf || *linebuf=='#') {
          जारी;
        } अन्यथा {
          p = linebuf;
          जबसम्म(*p&&है_खाली((पूर्णाङ्क)*p))
            p++;
          यदि(!*p || *p=='#') जारी;
          libName = p;
          जबसम्म(*p&&!है_खाली((पूर्णाङ्क)*p)) {
            p++;
          }
          यदि(!*p || *p=='#') जारी; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          जबसम्म(*p&&है_खाली((पूर्णाङ्क)*p)) {
            p++;
          }
          यदि(!*p||*p=='#') जारी; /* no symname after libname +tab */
          symName = p;
          जबसम्म(*p&&!है_खाली((पूर्णाङ्क)*p)) {
            p++;
          }
                    
          यदि(*p) { /* has config */
            *p=0;
            ++p;
            जबसम्म(*p&&है_खाली((पूर्णाङ्क)*p)) {
              p++;
            }
            यदि(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          यदि(config!=nullptr&&*config!=0) {
            p = config+म_माप(config);
            जबसम्म(p>config&&है_खाली((पूर्णाङ्क)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            यदि(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#यदि UPLUG_TRACE
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
#यदि UPLUG_TRACE
      DBG((मानक_त्रुटि, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_माथिल्लो;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


