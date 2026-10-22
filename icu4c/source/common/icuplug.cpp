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

#અન્તર્ભાવ "unicode/icuplug.h"


#જો UCONFIG_ENABLE_PLUGINS


#અન્તર્ભાવ "icuplugimp.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "putilimp.h"
#અન્તર્ભાવ "ucln.h"
#અન્તર્ભાવ <stdio.h>
#ifdef __MVS__  /* defined by z/OS compiler */
#define _POSIX_SOURCE
#અન્તર્ભાવ <cics.h> /* 12 Nov 2011 JAM iscics() function */
#endif
#અન્તર્ભાવ "charstr.h"

using નામાકાશ icu;

#ifndef UPLUG_TRACE
#define UPLUG_TRACE 0
#endif

#જો UPLUG_TRACE
#અન્તર્ભાવ <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

સંરચના UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  શૂન્ય *lib;              /**< plugin library, or nullptr */
  અક્ષર libName[UPLUG_NAME_MAX];   /**< library name */
  અક્ષર sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  અક્ષર config[UPLUG_NAME_MAX];     /**< configuration data */
  શૂન્ય *context;          /**< user context data */
  અક્ષર name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaitingLoad; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UErrorCode pluginStatus; /**< status code of plugin */
};



#define UPLUG_LIBRARY_INITIAL_ગણતરી 8
#define UPLUG_PLUGIN_INITIAL_ગણતરી 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
સ્થિર int32_t uplug_removeEntryAt(શૂન્ય *list, int32_t listSize, int32_t memberSize, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  જો(listSize<1) {
    પાછા listSize;
  }
    
  /* is there anything to move? */
  જો(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  પાછા listSize-1;
}




#જો U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
સંરચના UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
પ્રકારવ્યાખ્યા સંરચના UPlugLibrary {
  શૂન્ય *lib;                           /**< library ptr */
  અક્ષર name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

સ્થિર UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_ગણતરી];
સ્થિર UPlugLibrary * libraryList = staticLibraryList;
સ્થિર int32_t library_ગણતરી = 0;
સ્થિર int32_t libraryMax = UPLUG_LIBRARY_INITIAL_ગણતરી;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
સ્થિર int32_t શોધ_For_Library_Name(અચલ અક્ષર *libName) {
  int32_t i;
    
  માટે(i=0;i<library_ગણતરી;i++) {
    જો(!uprv_strcmp(libName, libraryList[i].name)) {
      પાછા i;
    }
  }
  પાછા -1;
}

સ્થિર int32_t શોધ_For_Library(શૂન્ય *lib) {
  int32_t i;
    
  માટે(i=0;i<library_ગણતરી;i++) {
    જો(lib==libraryList[i].lib) {
      પાછા i;
    }
  }
  પાછા -1;
}

U_CAPI અક્ષર * U_EXPORT2
uplug_findLibrary(શૂન્ય *lib, UErrorCode *status) {
  int32_t libEnt;
  અક્ષર *ret = nullptr;
  જો(U_FAILURE(*status)) {
    પાછા nullptr;
  }
  libEnt = શોધ_For_Library(lib);
  જો(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } નહીંતર {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  પાછા ret;
}

U_CAPI શૂન્ય * U_EXPORT2
uplug_openLibrary(અચલ અક્ષર *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  શૂન્ય *lib = nullptr;
    
  જો(U_FAILURE(*status)) પાછા nullptr;

  libEntry = શોધ_For_Library_Name(libName);
  જો(libEntry == -1) {
    libEntry = library_ગણતરી++;
    જો(library_ગણતરી >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#જો UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      પાછા nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#જો UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    જો(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#જો UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      library_ગણતરી--;
    } નહીંતર { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } નહીંતર {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  પાછા lib;
}

U_CAPI શૂન્ય U_EXPORT2
uplug_closeLibrary(શૂન્ય *lib, UErrorCode *status) {
  int32_t i;
    
#જો UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (શૂન્ય*)libraryList));
#endif
  જો(U_FAILURE(*status)) પાછા;
    
  માટે(i=0;i<library_ગણતરી;i++) {
    જો(lib==libraryList[i].lib) {
      જો(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_ગણતરી = uplug_removeEntryAt(libraryList, library_ગણતરી, કદ(*libraryList), i);
      }
      પાછા;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

સ્થિર UPlugData pluginList[UPLUG_PLUGIN_INITIAL_ગણતરી];
સ્થિર int32_t plugin_ગણતરી = 0;



  
સ્થિર int32_t uplug_plugin_સંખ્યા(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[plugin_ગણતરી];
  જો(d<=pluginList) {
    પાછા 0;
  } નહીંતર જો(d>=pastPlug) {
    પાછા plugin_ગણતરી;
  } નહીંતર {
    પાછા (d-pluginList)/કદ(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  જો(prior==nullptr) {
    પાછા pluginList;
  } નહીંતર {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[plugin_ગણતરી];
    
    જો(nextPlug>=pastPlug) {
      પાછા nullptr;
    } નહીંતર {
      પાછા nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
સ્થિર શૂન્ય uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  જો(plug==nullptr||U_FAILURE(*status)) {
    પાછા;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  જો(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


સ્થિર શૂન્ય uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  જો(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    પાછા; 
  }
  જો(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

સ્થિર શૂન્ય uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  જો(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    પાછા; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  જો(U_SUCCESS(*status)) { 
    જો(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = ખોટું;
    }
  } નહીંતર {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = ખોટું;
  }
}


સ્થિર શૂન્ય uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  જો(U_FAILURE(*status)) {
    પાછા;
  }
  જો(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_નીચલું) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    પાછા;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = ખોટું;
  જો(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

સ્થિર UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  જો(U_FAILURE(*status)) {
    પાછા nullptr;
  }

  જો(plugin_ગણતરી == UPLUG_PLUGIN_INITIAL_ગણતરી) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    પાછા nullptr;
  }

  plug = &pluginList[plugin_ગણતરી++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = કદ(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = સાચું;
  plug->dontUnload = ખોટું;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  પાછા plug;
}

સ્થિર UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, અચલ અક્ષર *config, શૂન્ય *lib, અચલ અક્ષર *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  જો(U_FAILURE(*status)) {
    પાછા nullptr;
  }

  જો(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } નહીંતર {
    plug->config[0] = 0;
  }
    
  જો(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } નહીંતર {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  પાછા plug;
}

સ્થિર શૂન્ય uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  જો(!plug->dontUnload) {
#જો U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  જો(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  જો(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_ગણતરી = uplug_removeEntryAt(pluginList, plugin_ગણતરી, કદ(plug[0]), uplug_plugin_સંખ્યા(plug));
  } નહીંતર {
    /* not ok- leave as a message. */
    plug->awaitingLoad=ખોટું;
    plug->entrypoint=0;
    plug->dontUnload=સાચું;
  }
}

સ્થિર શૂન્ય uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  જો(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI શૂન્ય U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  જો(U_FAILURE(*status)) પાછા;
    
  માટે(cursor=pluginList;cursor!=nullptr;) {
    જો(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } નહીંતર {
      cursor = uplug_nextPlug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI શૂન્ય U_EXPORT2 
uplug_setPlugNoUnload(UPlugData *data, UBool dontUnload)
{
  data->dontUnload = dontUnload;
}


U_CAPI શૂન્ય U_EXPORT2
uplug_setPlugLevel(UPlugData *data, UPlugLevel level) {
  data->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlugData *data) {
  પાછા data->level;
}


U_CAPI શૂન્ય U_EXPORT2
uplug_setPlugName(UPlugData *data, અચલ અક્ષર *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI અચલ અક્ષર* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  પાછા data->name;
}


U_CAPI અચલ અક્ષર* U_EXPORT2
uplug_get_પ્રતીક_Name(UPlugData* data U_LIFETIME_BOUND) {
  પાછા data->sym;
}

U_CAPI અચલ અક્ષર* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  જો(data->libName[0]) {
    પાછા data->libName;
  } નહીંતર {
#જો U_ENABLE_DYLOAD
    પાછા uplug_findLibrary(data->lib, status);
#નહીંતર
    પાછા nullptr;
#endif
  }
}

U_CAPI શૂન્ય * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  પાછા data->lib;
}

U_CAPI શૂન્ય * U_EXPORT2
uplug_getContext(UPlugData *data) {
  પાછા data->context;
}


U_CAPI શૂન્ય U_EXPORT2
uplug_setContext(UPlugData *data, શૂન્ય *context) {
  data->context = context;
}

U_CAPI અચલ અક્ષર* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  પાછા data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  જો(n <0 || n >= plugin_ગણતરી) {
    પાછા nullptr;
  } નહીંતર { 
    પાછા &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  પાછા plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
સ્થિર UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, અચલ અક્ષર *config, શૂન્ય *lib, અચલ અક્ષર *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  જો(U_SUCCESS(*status)) {
    પાછા plug;
  } નહીંતર {
    uplug_deallocatePlug(plug, status);
    પાછા nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, અચલ અક્ષર *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  પાછા plug;
}

#જો U_ENABLE_DYLOAD

સ્થિર UPlugData* 
uplug_initErrorPlug(અચલ અક્ષર *libName, અચલ અક્ષર *sym, અચલ અક્ષર *config, અચલ અક્ષર *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  જો(U_FAILURE(*status)) પાછા nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = ખોટું; /* Won't load. */
  plug->dontUnload = સાચું; /* cannot unload. */

  જો(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  જો(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  જો(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  જો(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  પાછા plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
સ્થિર UPlugData* 
uplug_initPlugFromLibrary(અચલ અક્ષર *libName, અચલ અક્ષર *sym, અચલ અક્ષર *config, UErrorCode *status) {
  શૂન્ય *lib = nullptr;
  UPlugData *plug = nullptr;
  જો(U_FAILURE(*status)) { પાછા nullptr; }
  lib = uplug_openLibrary(libName, status);
  જો(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    જો(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      જો(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } નહીંતર {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    જો(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } નહીંતર {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  પાછા plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(અચલ અક્ષર *libName, અચલ અક્ષર *sym, અચલ અક્ષર *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  જો(U_FAILURE(*status)) { પાછા nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  પાછા plug;
}

#endif

સ્થિર UPlugLevel gCurrentLevel = UPLUG_LEVEL_નીચલું;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  પાછા gCurrentLevel;
}

સ્થિર UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  માટે(i=0;i<plugin_ગણતરી;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_નીચલું;
  પાછા સાચું;
}

#જો U_ENABLE_DYLOAD

સ્થિર શૂન્ય uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  જો(U_FAILURE(*status)) {
    પાછા;
  }
#જો UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  માટે(i=0;i<plugin_ગણતરી;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    જો(pluginToLoad->awaitingLoad) {
      જો(pluginToLoad->level == UPLUG_LEVEL_નીચલું) {
        જો(currentLevel > UPLUG_LEVEL_નીચલું) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_ઉપલું;
        } નહીંતર {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          જો(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = ખોટું;
      } 
    }
  }    
  માટે(i=0;i<plugin_ગણતરી;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    જો(pluginToLoad->awaitingLoad) {
      જો(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } નહીંતર જો(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } નહીંતર {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = ખોટું;
    }
  }
    
#જો UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
સ્થિર અક્ષર plugin_file[2048] = "";
#endif

U_CAPI અચલ અક્ષર* U_EXPORT2
uplug_getPluginFile() {
#જો U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  પાછા plugin_file;
#નહીંતર
  પાછા nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI શૂન્ય U_EXPORT2
uplug_init(UErrorCode *status) {
#જો !U_ENABLE_DYLOAD
  (શૂન્ય)status; /* unused */
#અથવાજો !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  અચલ અક્ષર *env = getenv("ICU_PLUGINS");

  જો(U_FAILURE(*status)) પાછા;
  જો(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  જો(U_FAILURE(*status)) પાછા;

#જો defined(DEFAULT_ICU_PLUGINS) 
  જો(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#જો UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  જો(!plugin_dir.isEmpty()) {
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
#નહીંતર
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#જો UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    જો(U_FAILURE(*status)) {
      પાછા;
    }
    જો((size_t)pluginFile.length() > (કદ(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#જો UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      પાછા;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, કદ(plugin_file), *status);

#જો UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (પૂર્ણાંક)strlen(plugin_file), (પૂર્ણાંક)કદ(plugin_file)));
#endif
        
#ifdef __MVS__
    જો (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    નહીંતર
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    જો(f != nullptr) {
      અક્ષર linebuf[1024];
      અક્ષર *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t લીટી = 0;
            
            
      જ્યારે(fgets(linebuf,1023,f)) {
        લીટી++;

        જો(!*linebuf || *linebuf=='#') {
          ચાલુ;
        } નહીંતર {
          p = linebuf;
          જ્યારે(*p&&isspace((પૂર્ણાંક)*p))
            p++;
          જો(!*p || *p=='#') ચાલુ;
          libName = p;
          જ્યારે(*p&&!isspace((પૂર્ણાંક)*p)) {
            p++;
          }
          જો(!*p || *p=='#') ચાલુ; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          જ્યારે(*p&&isspace((પૂર્ણાંક)*p)) {
            p++;
          }
          જો(!*p||*p=='#') ચાલુ; /* no symname after libname +tab */
          symName = p;
          જ્યારે(*p&&!isspace((પૂર્ણાંક)*p)) {
            p++;
          }
                    
          જો(*p) { /* has config */
            *p=0;
            ++p;
            જ્યારે(*p&&isspace((પૂર્ણાંક)*p)) {
              p++;
            }
            જો(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          જો(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            જ્યારે(p>config&&isspace((પૂર્ણાંક)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            જો(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#જો UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (શૂન્ય*)plug, u_errorName(subStatus)));
#નહીંતર
            (શૂન્ય)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } નહીંતર {
#જો UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_ઉપલું;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


