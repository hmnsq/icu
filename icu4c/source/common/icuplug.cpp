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

#ଅନ୍ତର୍ଭାଵ "unicode/icuplug.h"


#ଯଦି UCONFIG_ENABLE_PLUGINS


#ଅନ୍ତର୍ଭାଵ "icuplugimp.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "ucln.h"
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ifdef __MVS__  /* defined by z/OS compiler */
#define _POSIX_SOURCE
#ଅନ୍ତର୍ଭାଵ <cics.h> /* 12 Nov 2011 JAM iscics() function */
#endif
#ଅନ୍ତର୍ଭାଵ "charstr.h"

using ନାମାକାଶ icu;

#ifndef UPLUG_TRACE
#define UPLUG_TRACE 0
#endif

#ଯଦି UPLUG_TRACE
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#define DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#endif

/**
 * Internal structure of an ICU plugin. 
 */

ସଂରଚନା UPlugData {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t structSize;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  ଶୂନ୍ୟ *lib;              /**< plugin library, or nullptr */
  ଅକ୍ଷର libName[UPLUG_NAME_MAX];   /**< library name */
  ଅକ୍ଷର sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  ଅକ୍ଷର config[UPLUG_NAME_MAX];     /**< configuration data */
  ଶୂନ୍ୟ *context;          /**< user context data */
  ଅକ୍ଷର name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaitingLoad; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UErrorCode pluginStatus; /**< status code of plugin */
};



#define UPLUG_LIBRARY_INITIAL_ଗଣନା 8
#define UPLUG_PLUGIN_INITIAL_ଗଣନା 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
ସ୍ଥିର int32_t uplug_removeEntryAt(ଶୂନ୍ୟ *list, int32_t listSize, int32_t memberSize, int32_t itemToRemove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  ଯଦି(listSize<1) {
    ଫେରନ୍ତୁ listSize;
  }
    
  /* is there anything to move? */
  ଯଦି(listSize > itemToRemove+1) {
    memmove(bytePtr+(itemToRemove*memberSize), bytePtr+((itemToRemove+1)*memberSize), memberSize);
  }
    
  ଫେରନ୍ତୁ listSize-1;
}




#ଯଦି U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
ସଂରଚନା UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା UPlugLibrary {
  ଶୂନ୍ୟ *lib;                           /**< library ptr */
  ଅକ୍ଷର name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

ସ୍ଥିର UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_ଗଣନା];
ସ୍ଥିର UPlugLibrary * libraryList = staticLibraryList;
ସ୍ଥିର int32_t library_ଗଣନା = 0;
ସ୍ଥିର int32_t libraryMax = UPLUG_LIBRARY_INITIAL_ଗଣନା;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ସ୍ଥିର int32_t ସନ୍ଧାନ_For_Library_Name(ଅଚଳ ଅକ୍ଷର *libName) {
  int32_t i;
    
  ପାଇଁ(i=0;i<library_ଗଣନା;i++) {
    ଯଦି(!uprv_strcmp(libName, libraryList[i].name)) {
      ଫେରନ୍ତୁ i;
    }
  }
  ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର int32_t ସନ୍ଧାନ_For_Library(ଶୂନ୍ୟ *lib) {
  int32_t i;
    
  ପାଇଁ(i=0;i<library_ଗଣନା;i++) {
    ଯଦି(lib==libraryList[i].lib) {
      ଫେରନ୍ତୁ i;
    }
  }
  ଫେରନ୍ତୁ -1;
}

U_CAPI ଅକ୍ଷର * U_EXPORT2
uplug_findLibrary(ଶୂନ୍ୟ *lib, UErrorCode *status) {
  int32_t libEnt;
  ଅକ୍ଷର *ret = nullptr;
  ଯଦି(U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }
  libEnt = ସନ୍ଧାନ_For_Library(lib);
  ଯଦି(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } ଅନ୍ୟଥା {
    *status = U_MISSING_RESOURCE_ERROR;
  }
  ଫେରନ୍ତୁ ret;
}

U_CAPI ଶୂନ୍ୟ * U_EXPORT2
uplug_openLibrary(ଅଚଳ ଅକ୍ଷର *libName, UErrorCode *status) {
  int32_t libEntry = -1;
  ଶୂନ୍ୟ *lib = nullptr;
    
  ଯଦି(U_FAILURE(*status)) ଫେରନ୍ତୁ nullptr;

  libEntry = ସନ୍ଧାନ_For_Library_Name(libName);
  ଯଦି(libEntry == -1) {
    libEntry = library_ଗଣନା++;
    ଯଦି(library_ଗଣନା >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *status = U_MEMORY_ALLOCATION_ERROR;
#ଯଦି UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#endif
      ଫେରନ୍ତୁ nullptr;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[libEntry].lib = uprv_dl_open(libName, status);
#ଯଦି UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
        
    ଯଦି(libraryList[libEntry].lib == nullptr || U_FAILURE(*status)) {
      /* cleanup. */
      libraryList[libEntry].lib = nullptr; /* failure with open */
      libraryList[libEntry].name[0] = 0;
#ଯଦି UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*status), libEntry, lib));
#endif
      /* no need to free - just won't increase the count. */
      library_ଗଣନା--;
    } ଅନ୍ୟଥା { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[libEntry].name,libName,UPLUG_NAME_MAX);
      libraryList[libEntry].ref=1;
      lib = libraryList[libEntry].lib;
    }

  } ଅନ୍ୟଥା {
    lib = libraryList[libEntry].lib;
    libraryList[libEntry].ref++;
  }
  ଫେରନ୍ତୁ lib;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uplug_closeLibrary(ଶୂନ୍ୟ *lib, UErrorCode *status) {
  int32_t i;
    
#ଯଦି UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*status), (ଶୂନ୍ୟ*)libraryList));
#endif
  ଯଦି(U_FAILURE(*status)) ଫେରନ୍ତୁ;
    
  ପାଇଁ(i=0;i<library_ଗଣନା;i++) {
    ଯଦି(lib==libraryList[i].lib) {
      ଯଦି(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, status);
        library_ଗଣନା = uplug_removeEntryAt(libraryList, library_ଗଣନା, ଆକାର(*libraryList), i);
      }
      ଫେରନ୍ତୁ;
    }
  }
  *status = U_INTERNAL_PROGRAM_ERROR; /* could not find the entry! */
}

#endif

ସ୍ଥିର UPlugData pluginList[UPLUG_PLUGIN_INITIAL_ଗଣନା];
ସ୍ଥିର int32_t plugin_ଗଣନା = 0;



  
ସ୍ଥିର int32_t uplug_plugin_ସଂଖ୍ୟା(UPlugData* d) {
  UPlugData *pastPlug = &pluginList[plugin_ଗଣନା];
  ଯଦି(d<=pluginList) {
    ଫେରନ୍ତୁ 0;
  } ଅନ୍ୟଥା ଯଦି(d>=pastPlug) {
    ଫେରନ୍ତୁ plugin_ଗଣନା;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ (d-pluginList)/ଆକାର(pluginList[0]);
  }
}


U_CAPI UPlugData * U_EXPORT2
uplug_nextPlug(UPlugData *prior) {
  ଯଦି(prior==nullptr) {
    ଫେରନ୍ତୁ pluginList;
  } ଅନ୍ୟଥା {
    UPlugData *nextPlug = &prior[1];
    UPlugData *pastPlug = &pluginList[plugin_ଗଣନା];
    
    ଯଦି(nextPlug>=pastPlug) {
      ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା {
      ଫେରନ୍ତୁ nextPlug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ସ୍ଥିର ଶୂନ୍ୟ uplug_callPlug(UPlugData *plug, UPlugReason reason, UErrorCode *status) {
  UPlugTokenReturn token;
  ଯଦି(plug==nullptr||U_FAILURE(*status)) {
    ଫେରନ୍ତୁ;
  }
  token = (*(plug->entrypoint))(plug, reason, status);
  ଯଦି(token!=UPLUG_TOKEN) {
    *status = U_INTERNAL_PROGRAM_ERROR;
  }
}


ସ୍ଥିର ଶୂନ୍ୟ uplug_unloadPlug(UPlugData *plug, UErrorCode *status) {
  ଯଦି(plug->awaitingLoad) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ଫେରନ୍ତୁ; 
  }
  ଯଦି(U_SUCCESS(plug->pluginStatus)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, status);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ uplug_queryPlug(UPlugData *plug, UErrorCode *status) {
  ଯଦି(!plug->awaitingLoad || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ଫେରନ୍ତୁ; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, status);
  ଯଦି(U_SUCCESS(*status)) { 
    ଯଦି(plug->level == UPLUG_LEVEL_INVALID) {
      plug->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      plug->awaitingLoad = ମିଥ୍ୟା;
    }
  } ଅନ୍ୟଥା {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
    plug->awaitingLoad = ମିଥ୍ୟା;
  }
}


ସ୍ଥିର ଶୂନ୍ୟ uplug_loadPlug(UPlugData *plug, UErrorCode *status) {
  ଯଦି(U_FAILURE(*status)) {
    ଫେରନ୍ତୁ;
  }
  ଯଦି(!plug->awaitingLoad || (plug->level < UPLUG_LEVEL_ତଳ) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *status = U_INTERNAL_PROGRAM_ERROR;
    ଫେରନ୍ତୁ;
  }
  uplug_callPlug(plug, UPLUG_REASON_LOAD, status);
  plug->awaitingLoad = ମିଥ୍ୟା;
  ଯଦି(!U_SUCCESS(*status)) {
    plug->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
  }
}

ସ୍ଥିର UPlugData *uplug_allocateEmptyPlug(UErrorCode *status)
{
  UPlugData *plug = nullptr;

  ଯଦି(U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }

  ଯଦି(plugin_ଗଣନା == UPLUG_PLUGIN_INITIAL_ଗଣନା) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ nullptr;
  }

  plug = &pluginList[plugin_ଗଣନା++];

  plug->token = UPLUG_TOKEN;
  plug->structSize = ଆକାର(UPlugData);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaitingLoad = ସତ୍ୟ;
  plug->dontUnload = ମିଥ୍ୟା;
  plug->pluginStatus = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=nullptr;
  plug->entrypoint=nullptr;


  ଫେରନ୍ତୁ plug;
}

ସ୍ଥିର UPlugData *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ଅଚଳ ଅକ୍ଷର *config, ଶୂନ୍ୟ *lib, ଅଚଳ ଅକ୍ଷର *symName,
                                     UErrorCode *status) {
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  ଯଦି(U_FAILURE(*status)) {
    ଫେରନ୍ତୁ nullptr;
  }

  ଯଦି(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } ଅନ୍ୟଥା {
    plug->config[0] = 0;
  }
    
  ଯଦି(symName!=nullptr) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } ଅନ୍ୟଥା {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, status);
    
  ଫେରନ୍ତୁ plug;
}

ସ୍ଥିର ଶୂନ୍ୟ uplug_deallocatePlug(UPlugData *plug, UErrorCode *status) {
  UErrorCode subStatus = U_ZERO_ERROR;
  ଯଦି(!plug->dontUnload) {
#ଯଦି U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &subStatus);
#endif
  }
  plug->lib = nullptr;
  ଯଦି(U_SUCCESS(*status) && U_FAILURE(subStatus)) {
    *status = subStatus;
  }
  /* shift plugins up and decrement count. */
  ଯଦି(U_SUCCESS(*status)) {
    /* all ok- remove. */
    plugin_ଗଣନା = uplug_removeEntryAt(pluginList, plugin_ଗଣନା, ଆକାର(plug[0]), uplug_plugin_ସଂଖ୍ୟା(plug));
  } ଅନ୍ୟଥା {
    /* not ok- leave as a message. */
    plug->awaitingLoad=ମିଥ୍ୟା;
    plug->entrypoint=0;
    plug->dontUnload=ସତ୍ୟ;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ uplug_doUnloadPlug(UPlugData *plugToRemove, UErrorCode *status) {
  ଯଦି(plugToRemove != nullptr) {
    uplug_unloadPlug(plugToRemove, status);
    uplug_deallocatePlug(plugToRemove, status);
  }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uplug_removePlug(UPlugData *plug, UErrorCode *status)  {
  UPlugData *cursor = nullptr;
  UPlugData *plugToRemove = nullptr;
  ଯଦି(U_FAILURE(*status)) ଫେରନ୍ତୁ;
    
  ପାଇଁ(cursor=pluginList;cursor!=nullptr;) {
    ଯଦି(cursor==plug) {
      plugToRemove = plug;
      cursor=nullptr;
    } ଅନ୍ୟଥା {
      cursor = uplug_nextPlug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plugToRemove, status);
}




U_CAPI ଶୂନ୍ୟ U_EXPORT2 
uplug_setPlugNoUnload(UPlugData *data, UBool dontUnload)
{
  data->dontUnload = dontUnload;
}


U_CAPI ଶୂନ୍ୟ U_EXPORT2
uplug_setPlugLevel(UPlugData *data, UPlugLevel level) {
  data->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlugData *data) {
  ଫେରନ୍ତୁ data->level;
}


U_CAPI ଶୂନ୍ୟ U_EXPORT2
uplug_setPlugName(UPlugData *data, ଅଚଳ ଅକ୍ଷର *name) {
  uprv_strncpy(data->name, name, UPLUG_NAME_MAX);
}


U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uplug_getPlugName(UPlugData* data U_LIFETIME_BOUND) {
  ଫେରନ୍ତୁ data->name;
}


U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uplug_get_ପ୍ରତୀକ_Name(UPlugData* data U_LIFETIME_BOUND) {
  ଫେରନ୍ତୁ data->sym;
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uplug_getLibraryName(UPlugData* data U_LIFETIME_BOUND, UErrorCode* status) {
  ଯଦି(data->libName[0]) {
    ଫେରନ୍ତୁ data->libName;
  } ଅନ୍ୟଥା {
#ଯଦି U_ENABLE_DYLOAD
    ଫେରନ୍ତୁ uplug_findLibrary(data->lib, status);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ nullptr;
#endif
  }
}

U_CAPI ଶୂନ୍ୟ * U_EXPORT2
uplug_getLibrary(UPlugData *data) {
  ଫେରନ୍ତୁ data->lib;
}

U_CAPI ଶୂନ୍ୟ * U_EXPORT2
uplug_getContext(UPlugData *data) {
  ଫେରନ୍ତୁ data->context;
}


U_CAPI ଶୂନ୍ୟ U_EXPORT2
uplug_setContext(UPlugData *data, ଶୂନ୍ୟ *context) {
  data->context = context;
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uplug_getConfiguration(UPlugData* data U_LIFETIME_BOUND) {
  ଫେରନ୍ତୁ data->config;
}

U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n) { 
  ଯଦି(n <0 || n >= plugin_ଗଣନା) {
    ଫେରନ୍ତୁ nullptr;
  } ଅନ୍ୟଥା { 
    ଫେରନ୍ତୁ &(pluginList[n]);
  }
}


U_CAPI UErrorCode U_EXPORT2
uplug_getPlugLoadStatus(UPlugData *plug) {
  ଫେରନ୍ତୁ plug->pluginStatus;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ସ୍ଥିର UPlugData* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ଅଚଳ ଅକ୍ଷର *config, ଶୂନ୍ୟ *lib, ଅଚଳ ଅକ୍ଷର *sym,
                                                         UErrorCode *status) {
  UPlugData *plug = nullptr;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, status);

  ଯଦି(U_SUCCESS(*status)) {
    ଫେରନ୍ତୁ plug;
  } ଅନ୍ୟଥା {
    uplug_deallocatePlug(plug, status);
    ଫେରନ୍ତୁ nullptr;
  }
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromEntrypoint(UPlugEntrypoint *entrypoint, ଅଚଳ ଅକ୍ଷର *config, UErrorCode *status) {
  UPlugData* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, nullptr, nullptr, status);
  uplug_loadPlug(plug, status);
  ଫେରନ୍ତୁ plug;
}

#ଯଦି U_ENABLE_DYLOAD

ସ୍ଥିର UPlugData* 
uplug_initErrorPlug(ଅଚଳ ଅକ୍ଷର *libName, ଅଚଳ ଅକ୍ଷର *sym, ଅଚଳ ଅକ୍ଷର *config, ଅଚଳ ଅକ୍ଷର *nameOrError, UErrorCode loadStatus, UErrorCode *status)
{
  UPlugData *plug = uplug_allocateEmptyPlug(status);
  ଯଦି(U_FAILURE(*status)) ଫେରନ୍ତୁ nullptr;

  plug->pluginStatus = loadStatus;
  plug->awaitingLoad = ମିଥ୍ୟା; /* Won't load. */
  plug->dontUnload = ସତ୍ୟ; /* cannot unload. */

  ଯଦି(sym!=nullptr) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  ଯଦି(libName!=nullptr) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  ଯଦି(nameOrError!=nullptr) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  ଯଦି(config!=nullptr) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  ଫେରନ୍ତୁ plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ସ୍ଥିର UPlugData* 
uplug_initPlugFromLibrary(ଅଚଳ ଅକ୍ଷର *libName, ଅଚଳ ଅକ୍ଷର *sym, ଅଚଳ ଅକ୍ଷର *config, UErrorCode *status) {
  ଶୂନ୍ୟ *lib = nullptr;
  UPlugData *plug = nullptr;
  ଯଦି(U_FAILURE(*status)) { ଫେରନ୍ତୁ nullptr; }
  lib = uplug_openLibrary(libName, status);
  ଯଦି(lib!=nullptr && U_SUCCESS(*status)) {
    UPlugEntrypoint *entrypoint = nullptr;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, status);

    ଯଦି(entrypoint!=nullptr&&U_SUCCESS(*status)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, status);
      ଯଦି(plug!=nullptr&&U_SUCCESS(*status)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = nullptr; /* library is now owned by plugin. */
      }
    } ଅନ୍ୟଥା {
      UErrorCode subStatus = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
    }
    ଯଦି(lib!=nullptr) { /* still need to close the lib */
      UErrorCode subStatus = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &subStatus); /* don't care here */
    }
  } ଅନ୍ୟଥା {
    UErrorCode subStatus = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==nullptr)?U_MISSING_RESOURCE_ERROR:*status,&subStatus);
  }
  ଫେରନ୍ତୁ plug;
}

U_CAPI UPlugData* U_EXPORT2
uplug_loadPlugFromLibrary(ଅଚଳ ଅକ୍ଷର *libName, ଅଚଳ ଅକ୍ଷର *sym, ଅଚଳ ଅକ୍ଷର *config, UErrorCode *status) { 
  UPlugData *plug = nullptr;
  ଯଦି(U_FAILURE(*status)) { ଫେରନ୍ତୁ nullptr; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, status);
  uplug_loadPlug(plug, status);

  ଫେରନ୍ତୁ plug;
}

#endif

ସ୍ଥିର UPlugLevel gCurrentLevel = UPLUG_LEVEL_ତଳ;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  ଫେରନ୍ତୁ gCurrentLevel;
}

ସ୍ଥିର UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlugData *pluginToRemove;
  /* cleanup plugs */
  ପାଇଁ(i=0;i<plugin_ଗଣନା;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    pluginToRemove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(pluginToRemove, &subStatus);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_ତଳ;
  ଫେରନ୍ତୁ ସତ୍ୟ;
}

#ଯଦି U_ENABLE_DYLOAD

ସ୍ଥିର ଶୂନ୍ୟ uplug_loadWaitingPlugs(UErrorCode *status) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  ଯଦି(U_FAILURE(*status)) {
    ଫେରନ୍ତୁ;
  }
#ଯଦି UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#endif
  /* pass #1: low level plugs */
  ପାଇଁ(i=0;i<plugin_ଗଣନା;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
    ଯଦି(pluginToLoad->awaitingLoad) {
      ଯଦି(pluginToLoad->level == UPLUG_LEVEL_ତଳ) {
        ଯଦି(currentLevel > UPLUG_LEVEL_ତଳ) {
          pluginToLoad->pluginStatus = U_PLUGIN_TOO_ଉପର;
        } ଅନ୍ୟଥା {
          UPlugLevel newLevel;
          uplug_loadPlug(pluginToLoad, &subStatus);
          newLevel = uplug_getCurrentLevel();
          ଯଦି(newLevel > currentLevel) {
            pluginToLoad->pluginStatus = U_PLUGIN_CHANGED_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        pluginToLoad->awaitingLoad = ମିଥ୍ୟା;
      } 
    }
  }    
  ପାଇଁ(i=0;i<plugin_ଗଣନା;i++) {
    UErrorCode subStatus = U_ZERO_ERROR;
    UPlugData *pluginToLoad = &pluginList[i];
        
    ଯଦି(pluginToLoad->awaitingLoad) {
      ଯଦି(pluginToLoad->level == UPLUG_LEVEL_INVALID) { 
        pluginToLoad->pluginStatus = U_PLUGIN_DIDNT_SET_LEVEL;
      } ଅନ୍ୟଥା ଯଦି(pluginToLoad->level == UPLUG_LEVEL_UNKNOWN) {
        pluginToLoad->pluginStatus = U_INTERNAL_PROGRAM_ERROR;
      } ଅନ୍ୟଥା {
        uplug_loadPlug(pluginToLoad, &subStatus);
      }
      pluginToLoad->awaitingLoad = ମିଥ୍ୟା;
    }
  }
    
#ଯଦି UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#endif
}

/* Name of the plugin config file */
ସ୍ଥିର ଅକ୍ଷର plugin_file[2048] = "";
#endif

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uplug_getPluginFile() {
#ଯଦି U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  ଫେରନ୍ତୁ plugin_file;
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ nullptr;
#endif
}


//  uplug_init()  is called first thing from u_init().

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uplug_init(UErrorCode *status) {
#ଯଦି !U_ENABLE_DYLOAD
  (ଶୂନ୍ୟ)status; /* unused */
#ନଚେତଯଦି !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ଅଚଳ ଅକ୍ଷର *env = getenv("ICU_PLUGINS");

  ଯଦି(U_FAILURE(*status)) ଫେରନ୍ତୁ;
  ଯଦି(env != nullptr) {
    plugin_dir.append(env, -1, *status);
  }
  ଯଦି(U_FAILURE(*status)) ଫେରନ୍ତୁ;

#ଯଦି defined(DEFAULT_ICU_PLUGINS) 
  ଯଦି(plugin_dir.isEmpty()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *status);
  }
#endif

#ଯଦି UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.data()));
#endif

  ଯଦି(!plugin_dir.isEmpty()) {
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
#ଅନ୍ୟଥା
    pluginFile.append(plugin_dir, *status);
    pluginFile.append(U_FILE_SEP_STRING, -1, *status);
    pluginFile.append("icuplugins", -1, *status);
    pluginFile.append(U_ICU_VERSION_SHORT, -1, *status);
    pluginFile.append(".txt", -1, *status);
#endif

#ଯଦି UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif

    ଯଦି(U_FAILURE(*status)) {
      ଫେରନ୍ତୁ;
    }
    ଯଦି((size_t)pluginFile.length() > (ଆକାର(plugin_file)-1)) {
      *status = U_BUFFER_OVERFLOW_ERROR;
#ଯଦି UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*status)));
#endif
      ଫେରନ୍ତୁ;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, ଆକାର(plugin_file), *status);

#ଯଦି UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(plugin_file), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(plugin_file)));
#endif
        
#ifdef __MVS__
    ଯଦି (iscics()) /* 12 Nov 2011 JAM */
    {
        f = nullptr;
    }
    ଅନ୍ୟଥା
#endif
    {
        f = fopen(pluginFile.data(), "r");
    }

    ଯଦି(f != nullptr) {
      ଅକ୍ଷର linebuf[1024];
      ଅକ୍ଷର *p, *libName=nullptr, *symName=nullptr, *config=nullptr;
      int32_t ଧାଡ଼ି = 0;
            
            
      ଯେତେବେଳେ(fgets(linebuf,1023,f)) {
        ଧାଡ଼ି++;

        ଯଦି(!*linebuf || *linebuf=='#') {
          ଜାରି;
        } ଅନ୍ୟଥା {
          p = linebuf;
          ଯେତେବେଳେ(*p&&isspace((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*p))
            p++;
          ଯଦି(!*p || *p=='#') ଜାରି;
          libName = p;
          ଯେତେବେଳେ(*p&&!isspace((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*p)) {
            p++;
          }
          ଯଦି(!*p || *p=='#') ଜାରି; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          ଯେତେବେଳେ(*p&&isspace((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*p)) {
            p++;
          }
          ଯଦି(!*p||*p=='#') ଜାରି; /* no symname after libname +tab */
          symName = p;
          ଯେତେବେଳେ(*p&&!isspace((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*p)) {
            p++;
          }
                    
          ଯଦି(*p) { /* has config */
            *p=0;
            ++p;
            ଯେତେବେଳେ(*p&&isspace((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*p)) {
              p++;
            }
            ଯଦି(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          ଯଦି(config!=nullptr&&*config!=0) {
            p = config+strlen(config);
            ଯେତେବେଳେ(p>config&&isspace((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UErrorCode subStatus = U_ZERO_ERROR;
            UPlugData *plug = uplug_initPlugFromLibrary(libName, symName, config, &subStatus);
            ଯଦି(U_FAILURE(subStatus) && U_SUCCESS(*status)) {
              *status = subStatus;
            }
#ଯଦି UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (ଶୂନ୍ୟ*)plug, u_errorName(subStatus)));
#ଅନ୍ୟଥା
            (ଶୂନ୍ୟ)plug; /* unused */
#endif
          }
        }
      }
      fclose(f);
    } ଅନ୍ୟଥା {
#ଯଦି UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#endif
    }
  }
  uplug_loadWaitingPlugs(status);
#endif /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_ଉପର;
  ucln_registerCleanup(UCLN_UPLUG, uplug_cleanup);
}

#endif


