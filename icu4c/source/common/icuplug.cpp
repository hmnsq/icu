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

#অন্তর্ভাব "unicode/icuplug.h"


#যদি UCONFIG_ENABLE_PLUGINS


#অন্তর্ভাব "icuplugimp.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "ucln.h"
#অন্তর্ভাব <stdio.h>
#ifdef __MVS__  /* defined by z/OS compiler */
#define _POSIX_SOURCE
#অন্তর্ভাব <cics.h> /* 12 Nov 2011 JAM iscics() function */
#endif
#অন্তর্ভাব "charstr.h"

using নামাকাশ icu;

#ifndef UPLUG_TRACE
#define UPLUG_TRACE 0
#endif

#যদি UPLUG_TRACE
#অন্তর্ভাব <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

কাঠামো UPlug_তথ্য {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t struct_আকার;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  শূন্য *lib;              /**< plugin library, or nullptr */
  অক্ষর libName[UPLUG_NAME_MAX];   /**< library name */
  অক্ষর sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  অক্ষর config[UPLUG_NAME_MAX];     /**< configuration data */
  শূন্য *context;          /**< user context data */
  অক্ষর name[UPLUG_NAME_MAX];   /**< name of plugin */
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
স্থির int32_t uplug_removeEntryAt(শূন্য *list, int32_t list_আকার, int32_t member_আকার, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  যদি(list_আকার<1) {
    ফেরত list_আকার;
  }
    
  /* is there anything to move? */
  যদি(list_আকার > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*member_আকার), bytePtr+((itemToRemove+1)*member_আকার), member_আকার);
  }
    
  ফেরত list_আকার-1;
}




#যদি U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
কাঠামো UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
প্রকারলেখ কাঠামো UPlugLibrary {
  শূন্য *lib;                           /**< library ptr */
  অক্ষর name[UPLUG_NAME_MAX]; /**< library name */
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
স্থির int32_t অনুসন্ধান_For_Library_Name(ধ্রুবক অক্ষর *libName) {
  int32_t i;
    
  জন্য(i=0;i<library_গুনতি;i++) {
    যদি(!uprv_strcmp(libName, libraryList[i].name)) {
      ফেরত i;
    }
  }
  ফেরত -1;
}

স্থির int32_t অনুসন্ধান_For_Library(শূন্য *lib) {
  int32_t i;
    
  জন্য(i=0;i<library_গুনতি;i++) {
    যদি(lib==libraryList[i].lib) {
      ফেরত i;
    }
  }
  ফেরত -1;
}

U_CAPI অক্ষর * U_EXPORT2
uplug_findLibrary(শূন্য *lib, UErrorCode *status) {
  int32_t libEnt;
  অক্ষর *ret = nullptr;
  যদি(U_FAILURE(*status)) {
    ফেরত nullptr;
  }
  libEnt = অনুসন্ধান_For_Library(lib);
  যদি(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } অন্যথা {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  ফেরত ret;
}

U_CAPI শূন্য * U_EXPORT2
uplug_openLibrary(ধ্রুবক অক্ষর *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  শূন্য *lib = nullptr;
    
  যদি(U_FAILURE(*status)) ফেরত nullptr;

  libEntry = অনুসন্ধান_For_Library_Name(libName);
  যদি(libEntry == -1) {
    libEntry = library_গুনতি++;
    যদি(library_গুনতি >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#যদি UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      ফেরত nullptr;
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
  ফেরত lib;
}

U_CAPI শূন্য U_EXPORT2
uplug_closeLibrary(শূন্য *lib, UErrorCode *status) {
  int32_t i;
    
#যদি UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (শূন্য*)libraryList));
#endif
  যদি(U_FAILURE(*status)) ফেরত;
    
  জন্য(i=0;i<library_গুনতি;i++) {
    যদি(lib==libraryList[i].lib) {
      যদি(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_গুনতি = uplug_removeEntryAt(libraryList, library_গুনতি, আকার(*libraryList), i);
      }
      ফেরত;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

স্থির UPlug_তথ্য pluginList[UPLUG_PLUGIN_INITIAL_গুনতি];
স্থির int32_t plugin_গুনতি = 0;



  
স্থির int32_t uplug_plugin_সংখ্যা(UPlug_তথ্য* d) {
  UPlug_তথ্য *pastPlug = &pluginList[plugin_গুনতি];
  যদি(d<=pluginList) {
    ফেরত 0;
  } অন্যথা যদি(d>=pastPlug) {
    ফেরত plugin_গুনতি;
  } অন্যথা {
    ফেরত (d-pluginList)/আকার(pluginList[0]);
  }
}


U_CAPI UPlug_তথ্য * U_EXPORT2
uplug_পরবর্তী_Plug(UPlug_তথ্য *prior) {
  যদি(prior==nullptr) {
    ফেরত pluginList;
  } অন্যথা {
    UPlug_তথ্য *পরবর্তী_Plug = &prior[1];
    UPlug_তথ্য *pastPlug = &pluginList[plugin_গুনতি];
    
    যদি(পরবর্তী_Plug>=pastPlug) {
      ফেরত nullptr;
    } অন্যথা {
      ফেরত পরবর্তী_Plug;
    }
  }
}



/**
 * Call the plugin with some params
 */
স্থির শূন্য uplug_callPlug(UPlug_তথ্য *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  যদি(plug==nullptr||U_FAILURE(*status)) {
    ফেরত;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  যদি(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


স্থির শূন্য uplug_unloadPlug(UPlug_তথ্য *plug, UErrorCode *status) {
  যদি(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ফেরত; 
  }
  যদি(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

স্থির শূন্য uplug_queryPlug(UPlug_তথ্য *plug, UErrorCode *status) {
  যদি(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ফেরত; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  যদি(U_SUCCESS(*status)) { 
    যদি(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = মিথ্যা;
    }
  } অন্যথা {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = মিথ্যা;
  }
}


স্থির শূন্য uplug_loadPlug(UPlug_তথ্য *plug, UErrorCode *status) {
  যদি(U_FAILURE(*status)) {
    ফেরত;
  }
  যদি(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_নিচের) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ফেরত;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = মিথ্যা;
  যদি(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

স্থির UPlug_তথ্য *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlug_তথ্য *plug = nullptr;

  যদি(U_FAILURE(*status)) {
    ফেরত nullptr;
  }

  যদি(plugin_গুনতি == UPLUG_PLUGIN_INITIAL_গুনতি) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ফেরত nullptr;
  }

  plug = &pluginList[plugin_গুনতি++];

  plug->token = UPLUG_TOKEN;
  plug->struct_আকার = আকার(UPlug_তথ্য);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = সত্য;
  plug->dontUnload = মিথ্যা;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  ফেরত plug;
}

স্থির UPlug_তথ্য *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ধ্রুবক অক্ষর *config, শূন্য *lib, ধ্রুবক অক্ষর *symName,
                                     UErrorCode *status) {
  UPlug_তথ্য *plug = uplug_allocateEmptyPlug(status);
  যদি(U_FAILURE(*status)) {
    ফেরত nullptr;
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
    
  ফেরত plug;
}

স্থির শূন্য uplug_deallocatePlug(UPlug_তথ্য *plug, UErrorCode *status) {
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
    plugin_গুনতি = uplug_removeEntryAt(pluginList, plugin_গুনতি, আকার(plug[0]), uplug_plugin_সংখ্যা(plug));
  } অন্যথা {
    /* not ok- leave as a message. */
    plug->awaitingLoad=মিথ্যা;
    plug->entrypoint=0;
    plug->dontUnload=সত্য;
  }
}

স্থির শূন্য uplug_doUnloadPlug(UPlug_তথ্য *plugToRemove, UErrorCode *status) {
  যদি(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI শূন্য U_EXPORT2
uplug_removePlug(UPlug_তথ্য *plug, UErrorCode *status)  {
  UPlug_তথ্য *cursor = nullptr;
  UPlug_তথ্য *plugToRemove = nullptr;
  যদি(U_FAILURE(*status)) ফেরত;
    
  জন্য(cursor=pluginList;cursor!=nullptr;) {
    যদি(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } অন্যথা {
      cursor = uplug_পরবর্তী_Plug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI শূন্য U_EXPORT2 
uplug_setPlugNoUnload(UPlug_তথ্য *তথ্য, UBool dontUnload)
{
  তথ্য->dontUnload = dontUnload;
}


U_CAPI শূন্য U_EXPORT2
uplug_setPlugLevel(UPlug_তথ্য *তথ্য, UPlugLevel level) {
  তথ্য->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlug_তথ্য *তথ্য) {
  ফেরত তথ্য->level;
}


U_CAPI শূন্য U_EXPORT2
uplug_setPlugName(UPlug_তথ্য *তথ্য, ধ্রুবক অক্ষর *name) {
  uprv_strncpy(তথ্য->name, name, UPLUG_NAME_MAX);
}


U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
uplug_getPlugName(UPlug_তথ্য* তথ্য U_LIFETIME_BOUND) {
  ফেরত তথ্য->name;
}


U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
uplug_get_প্রতীক_Name(UPlug_তথ্য* তথ্য U_LIFETIME_BOUND) {
  ফেরত তথ্য->sym;
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
uplug_getLibraryName(UPlug_তথ্য* তথ্য U_LIFETIME_BOUND, UErrorCode* status) {
  যদি(তথ্য->libName[0]) {
    ফেরত তথ্য->libName;
  } অন্যথা {
#যদি U_ENABLE_DYLOAD
    ফেরত uplug_findLibrary(তথ্য->lib, status);
#অন্যথা
    ফেরত nullptr;
#endif
  }
}

U_CAPI শূন্য * U_EXPORT2
uplug_getLibrary(UPlug_তথ্য *তথ্য) {
  ফেরত তথ্য->lib;
}

U_CAPI শূন্য * U_EXPORT2
uplug_getContext(UPlug_তথ্য *তথ্য) {
  ফেরত তথ্য->context;
}


U_CAPI শূন্য U_EXPORT2
uplug_setContext(UPlug_তথ্য *তথ্য, শূন্য *context) {
  তথ্য->context = context;
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
uplug_getConfiguration(UPlug_তথ্য* তথ্য U_LIFETIME_BOUND) {
  ফেরত তথ্য->config;
}

U_CAPI UPlug_তথ্য* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  যদি(n <0 || n >= plugin_গুনতি) {
    ফেরত nullptr;
  } অন্যথা { 
    ফেরত &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlug_তথ্য *plug) {
  ফেরত plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
স্থির UPlug_তথ্য* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ধ্রুবক অক্ষর *config, শূন্য *lib, ধ্রুবক অক্ষর *sym,
                                                         UErrorCode *status) {
  UPlug_তথ্য *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  যদি(U_SUCCESS(*status)) {
    ফেরত plug;
  } অন্যথা {
    uplug_deallocatePlug(plug, status);
    ফেরত nullptr;
  }
}

U_CAPI UPlug_তথ্য* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, ধ্রুবক অক্ষর *config, UErrorCode *status) {
  UPlug_তথ্য* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  ফেরত plug;
}

#যদি U_ENABLE_DYLOAD

স্থির UPlug_তথ্য* 
uplug_initErrorPlug(ধ্রুবক অক্ষর *libName, ধ্রুবক অক্ষর *sym, ধ্রুবক অক্ষর *config, ধ্রুবক অক্ষর *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlug_তথ্য *plug = uplug_allocateEmptyPlug(status);
  যদি(U_FAILURE(*status)) ফেরত nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = মিথ্যা; /* Won't load. */
  plug->dontUnload = সত্য; /* cannot unload. */

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

  ফেরত plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
স্থির UPlug_তথ্য* 
uplug_initPlugFromLibrary(ধ্রুবক অক্ষর *libName, ধ্রুবক অক্ষর *sym, ধ্রুবক অক্ষর *config, UErrorCode *status) {
  শূন্য *lib = nullptr;
  UPlug_তথ্য *plug = nullptr;
  যদি(U_FAILURE(*status)) { ফেরত nullptr; }
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
  ফেরত plug;
}

U_CAPI UPlug_তথ্য* U_EXPORT2
uplug_loadPlugFromLibrary(ধ্রুবক অক্ষর *libName, ধ্রুবক অক্ষর *sym, ধ্রুবক অক্ষর *config, UErrorCode *status) { 
  UPlug_তথ্য *plug = nullptr;
  যদি(U_FAILURE(*status)) { ফেরত nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  ফেরত plug;
}

#endif

স্থির UPlugLevel gCurrentLevel = UPLUG_LEVEL_নিচের;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  ফেরত gCurrentLevel;
}

স্থির UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlug_তথ্য *pluginToRemove;
  /* cleanup plugs */
  জন্য(i=0;i<plugin_গুনতি;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_নিচের;
  ফেরত সত্য;
}

#যদি U_ENABLE_DYLOAD

স্থির শূন্য uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  যদি(U_FAILURE(*status)) {
    ফেরত;
  }
#যদি UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  জন্য(i=0;i<plugin_গুনতি;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlug_তথ্য *pluginToLoad = &pluginList[i];
    যদি(pluginToLoad->awaitingLoad) {
      যদি(pluginToLoad->level == UPLUG_LEVEL_নিচের) {
        যদি(currentLevel > UPLUG_LEVEL_নিচের) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_উপরের;
        } অন্যথা {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          যদি(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = মিথ্যা;
      } 
    }
  }    
  জন্য(i=0;i<plugin_গুনতি;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlug_তথ্য *pluginToLoad = &pluginList[i];
        
    যদি(pluginToLoad->awaitingLoad) {
      যদি(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } অন্যথা যদি(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } অন্যথা {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = মিথ্যা;
    }
  }
    
#যদি UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
স্থির অক্ষর plugin_file[2048] = "";
#endif

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
uplug_getPluginFile() {
#যদি U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  ফেরত plugin_file;
#অন্যথা
  ফেরত nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI শূন্য U_EXPORT2
uplug_init(UErrorCode *status) {
#যদি !U_ENABLE_DYLOAD
  (শূন্য)status; /* unused */
#নতুবা_যদি !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ধ্রুবক অক্ষর *env = getenv("ICU_PLUGINS");

  যদি(U_FAILURE(*status)) ফেরত;
  যদি(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  যদি(U_FAILURE(*status)) ফেরত;

#যদি defined(DEFAULT_ICU_PLUGINS) 
  যদি(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#যদি UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.তথ্য()));
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
      ফেরত;
    }
    যদি((size_t)pluginFile.length() > (আকার(plugin_file)-1)) {
      *status = U_বাফার_OVERFLOW_ERROR;
#যদি UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      ফেরত;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, আকার(plugin_file), *status);

#যদি UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (পূর্ণসংখ্যা)strlen(plugin_file), (পূর্ণসংখ্যা)আকার(plugin_file)));
#endif
        
#ifdef __MVS__
    যদি (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    অন্যথা
#endif
    {
        f = fopen(pluginFile.তথ্য(), "r");
    }

    যদি(f != nullptr) {
      অক্ষর linebuf[1024];
      অক্ষর *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t সারি = 0;
            
            
      যখন(fgets(linebuf,1023,f)) {
        সারি++;

        যদি(!*linebuf || *linebuf=='#') {
          চালু;
        } অন্যথা {
          p = linebuf;
          যখন(*p&&isspace((পূর্ণসংখ্যা)*p))
            p++;
          যদি(!*p || *p=='#') চালু;
          libName = p;
          যখন(*p&&!isspace((পূর্ণসংখ্যা)*p)) {
            p++;
          }
          যদি(!*p || *p=='#') চালু; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          যখন(*p&&isspace((পূর্ণসংখ্যা)*p)) {
            p++;
          }
          যদি(!*p||*p=='#') চালু; /* no symname after libname +tab */
          symName = p;
          যখন(*p&&!isspace((পূর্ণসংখ্যা)*p)) {
            p++;
          }
                    
          যদি(*p) { /* has config */
            *p=0;
            ++p;
            যখন(*p&&isspace((পূর্ণসংখ্যা)*p)) {
              p++;
            }
            যদি(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          যদি(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            যখন(p>config&&isspace((পূর্ণসংখ্যা)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlug_তথ্য *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
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
  gCurrentLevel = UPLUG_LEVEL_উপরের;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


