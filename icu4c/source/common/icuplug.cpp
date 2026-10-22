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

#समावेश "unicode/icuplug.h"


#अगर UCONFIG_ENABLE_PLUGINS


#समावेश "icuplugimp.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "putilimp.h"
#समावेश "ucln.h"
#समावेश <stdio.h>
#अगर_घोषित __MVS__  /* defined by z/OS compiler */
#घोषणा _POSIX_SOURCE
#समावेश <cics.h> /* 12 Nov 2011 JAM iscics() function */
#पूर्ण_अगर
#समावेश "charstr.h"

using namespace icu;

#अगर_अघोषित UPLUG_TRACE
#घोषणा UPLUG_TRACE 0
#पूर्ण_अगर

#अगर UPLUG_TRACE
#समावेश <stdio.h>
#घोषणा DBG(x) ख_लिखो(मानक_त्रुटि, "%s:%d: ",__खाता__,__LINE__); ख_लिखो x
#पूर्ण_अगर

/**
 * Internal structure of an ICU plugin. 
 */

काष्ठा UPlug_तथ्य {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t struct_आकार;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  व्योम *lib;              /**< plugin library, or nullptr */
  अक्षर libName[UPLUG_NAME_MAX];   /**< library name */
  अक्षर sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  अक्षर config[UPLUG_NAME_MAX];     /**< configuration data */
  व्योम *context;          /**< user context data */
  अक्षर name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaitingLoad; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UErrorCode pluginStatus; /**< status code of plugin */
};



#घोषणा UPLUG_LIBRARY_INITIAL_गिनती 8
#घोषणा UPLUG_PLUGIN_INITIAL_गिनती 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
अटल int32_t uplug_removeEntryAt(व्योम *list, int32_t list_आकार, int32_t member_आकार, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  अगर(list_आकार<1) {
    वापस list_आकार;
  }
    
  /* is there anything to move? */
  अगर(list_आकार > itemToRemove+1) {
    स_हटाओ(bytePtr+(itemToRemove*member_आकार), bytePtr+((itemToRemove+1)*member_आकार), member_आकार);
  }
    
  वापस list_आकार-1;
}




#अगर U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
काष्ठा UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
प्रकार काष्ठा UPlugLibrary {
  व्योम *lib;                           /**< library ptr */
  अक्षर name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

अटल UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_गिनती];
अटल UPlugLibrary * libraryList = staticLibraryList;
अटल int32_t library_गिनती = 0;
अटल int32_t libraryMax = UPLUG_LIBRARY_INITIAL_गिनती;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
अटल int32_t खोज_For_Library_Name(स्थिर अक्षर *libName) {
  int32_t i;
    
  क्रम(i=0;i<library_गिनती;i++) {
    अगर(!uprv_strcmp(libName, libraryList[i].name)) {
      वापस i;
    }
  }
  वापस -1;
}

अटल int32_t खोज_For_Library(व्योम *lib) {
  int32_t i;
    
  क्रम(i=0;i<library_गिनती;i++) {
    अगर(lib==libraryList[i].lib) {
      वापस i;
    }
  }
  वापस -1;
}

U_CAPI अक्षर * U_EXPORT2
uplug_findLibrary(व्योम *lib, UErrorCode *status) {
  int32_t libEnt;
  अक्षर *ret = nullptr;
  अगर(U_FAILURE(*status)) {
    वापस nullptr;
  }
  libEnt = खोज_For_Library(lib);
  अगर(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } अन्यथा {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  वापस ret;
}

U_CAPI व्योम * U_EXPORT2
uplug_openLibrary(स्थिर अक्षर *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  व्योम *lib = nullptr;
    
  अगर(U_FAILURE(*status)) वापस nullptr;

  libEntry = खोज_For_Library_Name(libName);
  अगर(libEntry == -1) {
    libEntry = library_गिनती++;
    अगर(library_गिनती >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#अगर UPLUG_TRACE
      DBG((मानक_त्रुटि, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#पूर्ण_अगर
      वापस nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#अगर UPLUG_TRACE
    DBG((मानक_त्रुटि, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#पूर्ण_अगर
        
    अगर(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#अगर UPLUG_TRACE
      DBG((मानक_त्रुटि, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#पूर्ण_अगर
      /* no need to free - just won't increase the count. */
      library_गिनती--;
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
  वापस lib;
}

U_CAPI व्योम U_EXPORT2
uplug_closeLibrary(व्योम *lib, UErrorCode *status) {
  int32_t i;
    
#अगर UPLUG_TRACE
  DBG((मानक_त्रुटि, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (व्योम*)libraryList));
#पूर्ण_अगर
  अगर(U_FAILURE(*status)) वापस;
    
  क्रम(i=0;i<library_गिनती;i++) {
    अगर(lib==libraryList[i].lib) {
      अगर(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_गिनती = uplug_removeEntryAt(libraryList, library_गिनती, माप(*libraryList), i);
      }
      वापस;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#पूर्ण_अगर

अटल UPlug_तथ्य pluginList[UPLUG_PLUGIN_INITIAL_गिनती];
अटल int32_t plugin_गिनती = 0;



  
अटल int32_t uplug_plugin_संख्या(UPlug_तथ्य* d) {
  UPlug_तथ्य *pastPlug = &pluginList[plugin_गिनती];
  अगर(d<=pluginList) {
    वापस 0;
  } अन्यथा अगर(d>=pastPlug) {
    वापस plugin_गिनती;
  } अन्यथा {
    वापस (d-pluginList)/माप(pluginList[0]);
  }
}


U_CAPI UPlug_तथ्य * U_EXPORT2
uplug_अग्र_Plug(UPlug_तथ्य *prior) {
  अगर(prior==nullptr) {
    वापस pluginList;
  } अन्यथा {
    UPlug_तथ्य *अग्र_Plug = &prior[1];
    UPlug_तथ्य *pastPlug = &pluginList[plugin_गिनती];
    
    अगर(अग्र_Plug>=pastPlug) {
      वापस nullptr;
    } अन्यथा {
      वापस अग्र_Plug;
    }
  }
}



/**
 * Call the plugin with some params
 */
अटल व्योम uplug_callPlug(UPlug_तथ्य *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  अगर(plug==nullptr||U_FAILURE(*status)) {
    वापस;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  अगर(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


अटल व्योम uplug_unloadPlug(UPlug_तथ्य *plug, UErrorCode *status) {
  अगर(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    वापस; 
  }
  अगर(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

अटल व्योम uplug_queryPlug(UPlug_तथ्य *plug, UErrorCode *status) {
  अगर(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    वापस; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  अगर(U_SUCCESS(*status)) { 
    अगर(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = false;
    }
  } अन्यथा {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = false;
  }
}


अटल व्योम uplug_loadPlug(UPlug_तथ्य *plug, UErrorCode *status) {
  अगर(U_FAILURE(*status)) {
    वापस;
  }
  अगर(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_निचला) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    वापस;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = false;
  अगर(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

अटल UPlug_तथ्य *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlug_तथ्य *plug = nullptr;

  अगर(U_FAILURE(*status)) {
    वापस nullptr;
  }

  अगर(plugin_गिनती == UPLUG_PLUGIN_INITIAL_गिनती) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    वापस nullptr;
  }

  plug = &pluginList[plugin_गिनती++];

  plug->token = UPLUG_TOKEN;
  plug->struct_आकार = माप(UPlug_तथ्य);
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


  वापस plug;
}

अटल UPlug_तथ्य *uplug_allocatePlug(UPlugEntrypoint *entrypoint, स्थिर अक्षर *config, व्योम *lib, स्थिर अक्षर *symName,
                                     UErrorCode *status) {
  UPlug_तथ्य *plug = uplug_allocateEmptyPlug(status);
  अगर(U_FAILURE(*status)) {
    वापस nullptr;
  }

  अगर(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } अन्यथा {
    plug->config[0] = 0;
  }
    
  अगर(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } अन्यथा {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  वापस plug;
}

अटल व्योम uplug_deallocatePlug(UPlug_तथ्य *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  अगर(!plug->dontUnload) {
#अगर U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#पूर्ण_अगर
  }
  plug->lib = nullptr;
  अगर(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  अगर(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_गिनती = uplug_removeEntryAt(pluginList, plugin_गिनती, माप(plug[0]), uplug_plugin_संख्या(plug));
  } अन्यथा {
    /* not ok- leave as a message. */
    plug->awaitingLoad=false;
    plug->entrypoint=0;
    plug->dontUnload=true;
  }
}

अटल व्योम uplug_doUnloadPlug(UPlug_तथ्य *plugToRemove, UErrorCode *status) {
  अगर(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI व्योम U_EXPORT2
uplug_removePlug(UPlug_तथ्य *plug, UErrorCode *status)  {
  UPlug_तथ्य *cursor = nullptr;
  UPlug_तथ्य *plugToRemove = nullptr;
  अगर(U_FAILURE(*status)) वापस;
    
  क्रम(cursor=pluginList;cursor!=nullptr;) {
    अगर(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } अन्यथा {
      cursor = uplug_अग्र_Plug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI व्योम U_EXPORT2 
uplug_setPlugNoUnload(UPlug_तथ्य *तथ्य, UBool dontUnload)
{
  तथ्य->dontUnload = dontUnload;
}


U_CAPI व्योम U_EXPORT2
uplug_setPlugLevel(UPlug_तथ्य *तथ्य, UPlugLevel level) {
  तथ्य->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlug_तथ्य *तथ्य) {
  वापस तथ्य->level;
}


U_CAPI व्योम U_EXPORT2
uplug_setPlugName(UPlug_तथ्य *तथ्य, स्थिर अक्षर *name) {
  uprv_strncpy(तथ्य->name, name, UPLUG_NAME_MAX);
}


U_CAPI स्थिर अक्षर* U_EXPORT2
uplug_getPlugName(UPlug_तथ्य* तथ्य U_LIFETIME_BOUND) {
  वापस तथ्य->name;
}


U_CAPI स्थिर अक्षर* U_EXPORT2
uplug_get_प्रतीक_Name(UPlug_तथ्य* तथ्य U_LIFETIME_BOUND) {
  वापस तथ्य->sym;
}

U_CAPI स्थिर अक्षर* U_EXPORT2
uplug_getLibraryName(UPlug_तथ्य* तथ्य U_LIFETIME_BOUND, UErrorCode* status) {
  अगर(तथ्य->libName[0]) {
    वापस तथ्य->libName;
  } अन्यथा {
#अगर U_ENABLE_DYLOAD
    वापस uplug_findLibrary(तथ्य->lib, status);
#अन्यथा
    वापस nullptr;
#पूर्ण_अगर
  }
}

U_CAPI व्योम * U_EXPORT2
uplug_getLibrary(UPlug_तथ्य *तथ्य) {
  वापस तथ्य->lib;
}

U_CAPI व्योम * U_EXPORT2
uplug_getContext(UPlug_तथ्य *तथ्य) {
  वापस तथ्य->context;
}


U_CAPI व्योम U_EXPORT2
uplug_setContext(UPlug_तथ्य *तथ्य, व्योम *context) {
  तथ्य->context = context;
}

U_CAPI स्थिर अक्षर* U_EXPORT2
uplug_getConfiguration(UPlug_तथ्य* तथ्य U_LIFETIME_BOUND) {
  वापस तथ्य->config;
}

U_CAPI UPlug_तथ्य* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  अगर(n <0 || n >= plugin_गिनती) {
    वापस nullptr;
  } अन्यथा { 
    वापस &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlug_तथ्य *plug) {
  वापस plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
अटल UPlug_तथ्य* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, स्थिर अक्षर *config, व्योम *lib, स्थिर अक्षर *sym,
                                                         UErrorCode *status) {
  UPlug_तथ्य *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  अगर(U_SUCCESS(*status)) {
    वापस plug;
  } अन्यथा {
    uplug_deallocatePlug(plug, status);
    वापस nullptr;
  }
}

U_CAPI UPlug_तथ्य* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, स्थिर अक्षर *config, UErrorCode *status) {
  UPlug_तथ्य* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  वापस plug;
}

#अगर U_ENABLE_DYLOAD

अटल UPlug_तथ्य* 
uplug_initErrorPlug(स्थिर अक्षर *libName, स्थिर अक्षर *sym, स्थिर अक्षर *config, स्थिर अक्षर *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlug_तथ्य *plug = uplug_allocateEmptyPlug(status);
  अगर(U_FAILURE(*status)) वापस nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = false; /* Won't load. */
  plug->dontUnload = true; /* cannot unload. */

  अगर(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  अगर(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  अगर(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  अगर(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  वापस plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
अटल UPlug_तथ्य* 
uplug_initPlugFromLibrary(स्थिर अक्षर *libName, स्थिर अक्षर *sym, स्थिर अक्षर *config, UErrorCode *status) {
  व्योम *lib = nullptr;
  UPlug_तथ्य *plug = nullptr;
  अगर(U_FAILURE(*status)) { वापस nullptr; }
  lib = uplug_openLibrary(libName, status);
  अगर(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    अगर(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      अगर(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } अन्यथा {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    अगर(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } अन्यथा {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  वापस plug;
}

U_CAPI UPlug_तथ्य* U_EXPORT2
uplug_loadPlugFromLibrary(स्थिर अक्षर *libName, स्थिर अक्षर *sym, स्थिर अक्षर *config, UErrorCode *status) { 
  UPlug_तथ्य *plug = nullptr;
  अगर(U_FAILURE(*status)) { वापस nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  वापस plug;
}

#पूर्ण_अगर

अटल UPlugLevel gCurrentLevel = UPLUG_LEVEL_निचला;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  वापस gCurrentLevel;
}

अटल UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlug_तथ्य *pluginToRemove;
  /* cleanup plugs */
  क्रम(i=0;i<plugin_गिनती;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_निचला;
  वापस true;
}

#अगर U_ENABLE_DYLOAD

अटल व्योम uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  अगर(U_FAILURE(*status)) {
    वापस;
  }
#अगर UPLUG_TRACE
  DBG((मानक_त्रुटि,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#पूर्ण_अगर
  /* pass #1: low level plugs */
  क्रम(i=0;i<plugin_गिनती;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlug_तथ्य *pluginToLoad = &pluginList[i];
    अगर(pluginToLoad->awaitingLoad) {
      अगर(pluginToLoad->level == UPLUG_LEVEL_निचला) {
        अगर(currentLevel > UPLUG_LEVEL_निचला) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_ऊपरी;
        } अन्यथा {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          अगर(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = false;
      } 
    }
  }    
  क्रम(i=0;i<plugin_गिनती;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlug_तथ्य *pluginToLoad = &pluginList[i];
        
    अगर(pluginToLoad->awaitingLoad) {
      अगर(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } अन्यथा अगर(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } अन्यथा {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = false;
    }
  }
    
#अगर UPLUG_TRACE
  DBG((मानक_त्रुटि,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#पूर्ण_अगर
}

/* Name of the plugin config file */
अटल अक्षर plugin_file[2048] = "";
#पूर्ण_अगर

U_CAPI स्थिर अक्षर* U_EXPORT2
uplug_getPluginFile() {
#अगर U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  वापस plugin_file;
#अन्यथा
  वापस nullptr;
#पूर्ण_अगर
}


//  uplug_init()  is called first thing from u_init().

U_CAPI व्योम U_EXPORT2
uplug_init(UErrorCode *status) {
#अगर !U_ENABLE_DYLOAD
  (व्योम)status; /* unused */
#या_अगर !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  स्थिर अक्षर *env = दो_पर्या("ICU_PLUGINS");

  अगर(U_FAILURE(*status)) वापस;
  अगर(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  अगर(U_FAILURE(*status)) वापस;

#अगर defined(DEFAULT_ICU_PLUGINS) 
  अगर(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#पूर्ण_अगर

#अगर UPLUG_TRACE
  DBG((मानक_त्रुटि, "ICU_PLUGINS=%s\n", plugin_dir.तथ्य()));
#पूर्ण_अगर

  अगर(!plugin_dir.isEmpty()) {
    खाता *f;
        
    CharString pluginFile;
#अगर_घोषित ICU_PLUGINS_DD
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
#पूर्ण_अगर

#अगर UPLUG_TRACE
    DBG((मानक_त्रुटि, "status=%s\n", u_errorName(*status)));
#पूर्ण_अगर

    अगर(U_FAILURE(*status)) {
      वापस;
    }
    अगर((माप_प्रकार)pluginFile.length() > (माप(plugin_file)-1)) {
      *status = U_बफर_OVERFLOW_ERROR;
#अगर UPLUG_TRACE
      DBG((मानक_त्रुटि, "status=%s\n", u_errorName(*status)));
#पूर्ण_अगर
      वापस;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, माप(plugin_file), *status);

#अगर UPLUG_TRACE
    DBG((मानक_त्रुटि, "pluginfile= %s len %d/%d\n", plugin_file, (पूर्णांक)म_माप(plugin_file), (पूर्णांक)माप(plugin_file)));
#पूर्ण_अगर
        
#अगर_घोषित __MVS__
    अगर (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    अन्यथा
#पूर्ण_अगर
    {
        f = ख_खोलो(pluginFile.तथ्य(), "r");
    }

    अगर(f != nullptr) {
      अक्षर linebuf[1024];
      अक्षर *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t पंक्ति = 0;
            
            
      जबतक(ख_माला_लो(linebuf,1023,f)) {
        पंक्ति++;

        अगर(!*linebuf || *linebuf=='#') {
          जारी;
        } अन्यथा {
          p = linebuf;
          जबतक(*p&&है_खाली((पूर्णांक)*p))
            p++;
          अगर(!*p || *p=='#') जारी;
          libName = p;
          जबतक(*p&&!है_खाली((पूर्णांक)*p)) {
            p++;
          }
          अगर(!*p || *p=='#') जारी; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          जबतक(*p&&है_खाली((पूर्णांक)*p)) {
            p++;
          }
          अगर(!*p||*p=='#') जारी; /* no symname after libname +tab */
          symName = p;
          जबतक(*p&&!है_खाली((पूर्णांक)*p)) {
            p++;
          }
                    
          अगर(*p) { /* has config */
            *p=0;
            ++p;
            जबतक(*p&&है_खाली((पूर्णांक)*p)) {
              p++;
            }
            अगर(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          अगर(config!=nullptr&&*config!=0) {
            p = config+म_माप(config);
            जबतक(p>config&&है_खाली((पूर्णांक)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlug_तथ्य *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            अगर(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#अगर UPLUG_TRACE
            DBG((मानक_त्रुटि, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((मानक_त्रुटि, " -> %p, %s\n", (व्योम*)plug, u_errorName(subStatus)));
#अन्यथा
            (व्योम)plug; /* unused */
#पूर्ण_अगर
          }
        }
      }
      ख_बंद(f);
    } अन्यथा {
#अगर UPLUG_TRACE
      DBG((मानक_त्रुटि, "Can't open plugin file %s\n", plugin_file));
#पूर्ण_अगर
    }
  }
  uplug_loadWaitingPlugs(status);
#पूर्ण_अगर /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_ऊपरी;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#पूर्ण_अगर


