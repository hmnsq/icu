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

#అన్తర్భావ "unicode/icuplug.h"


#ఒకవేళ UCONFIG_ENABLE_PLUGINS


#అన్తర్భావ "icuplugimp.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "ucln.h"
#అన్తర్భావ <stdio.h>
#నిర్వచితమైతే __MVS__  /* defined by z/OS compiler */
#నిర్వచించు _POSIX_SOURCE
#అన్తర్భావ <cics.h> /* 12 Nov 2011 JAM iscics() function */
#అయితే_ముగింపు
#అన్తర్భావ "charstr.h"

ఉపయోగించి నామాకాశ icu;

#నిర్వచితం_కాకపోతే UPLUG_TRACE
#నిర్వచించు UPLUG_TRACE 0
#అయితే_ముగింపు

#ఒకవేళ UPLUG_TRACE
#అన్తర్భావ <stdio.h>
#నిర్వచించు DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#అయితే_ముగింపు

/**
 * Internal structure of an ICU plugin. 
 */

నిర్మాణం UPlug_దత్తాంశం {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t struct_కొలత;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  శూన్య *lib;              /**< plugin library, or nullptr */
  అక్షరం libName[UPLUG_NAME_MAX];   /**< library name */
  అక్షరం sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  అక్షరం config[UPLUG_NAME_MAX];     /**< configuration data */
  శూన్య *context;          /**< user context data */
  అక్షరం name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaiting_ఎక్కించు; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UError_సంకేతం plugin_స్థితి; /**< status code of plugin */
};



#నిర్వచించు UPLUG_LIBRARY_INITIAL_లెక్క 8
#నిర్వచించు UPLUG_PLUGIN_INITIAL_లెక్క 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
స్థిర int32_t uplug_remove_నమోదు_At(శూన్య *list, int32_t list_కొలత, int32_t member_కొలత, int32_t అంశం_గమ్యం_Remove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  ఒకవేళ(list_కొలత<1) {
    తిరిగి list_కొలత;
  }
    
  /* is there anything to move? */
  ఒకవేళ(list_కొలత > అంశం_గమ్యం_Remove+1) {
    memmove(bytePtr+(అంశం_గమ్యం_Remove*member_కొలత), bytePtr+((అంశం_గమ్యం_Remove+1)*member_కొలత), member_కొలత);
  }
    
  తిరిగి list_కొలత-1;
}




#ఒకవేళ U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
నిర్మాణం UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
రకనిర్వచనం నిర్మాణం UPlugLibrary {
  శూన్య *lib;                           /**< library ptr */
  అక్షరం name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

స్థిర UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_లెక్క];
స్థిర UPlugLibrary * libraryList = staticLibraryList;
స్థిర int32_t library_లెక్క = 0;
స్థిర int32_t libraryMax = UPLUG_LIBRARY_INITIAL_లెక్క;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
స్థిర int32_t శోధన_For_Library_Name(స్థిరరాశి అక్షరం *libName) {
  int32_t i;
    
  కొరకు(i=0;i<library_లెక్క;i++) {
    ఒకవేళ(!uprv_strcmp(libName, libraryList[i].name)) {
      తిరిగి i;
    }
  }
  తిరిగి -1;
}

స్థిర int32_t శోధన_For_Library(శూన్య *lib) {
  int32_t i;
    
  కొరకు(i=0;i<library_లెక్క;i++) {
    ఒకవేళ(lib==libraryList[i].lib) {
      తిరిగి i;
    }
  }
  తిరిగి -1;
}

U_CAPI అక్షరం * U_EXPORT2
uplug_కనుగొను_Library(శూన్య *lib, UError_సంకేతం *స్థితి_2) {
  int32_t libEnt;
  అక్షరం *ret = శూన్యసూచిక;
  ఒకవేళ(U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }
  libEnt = శోధన_For_Library(lib);
  ఒకవేళ(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } లేకపోతే {
    *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
  }
  తిరిగి ret;
}

U_CAPI శూన్య * U_EXPORT2
uplug_openLibrary(స్థిరరాశి అక్షరం *libName, UError_సంకేతం *స్థితి_2) {
  int32_t lib_నమోదు = -1;
  శూన్య *lib = శూన్యసూచిక;
    
  ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి శూన్యసూచిక;

  lib_నమోదు = శోధన_For_Library_Name(libName);
  ఒకవేళ(lib_నమోదు == -1) {
    lib_నమోదు = library_లెక్క++;
    ఒకవేళ(library_లెక్క >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
#ఒకవేళ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#అయితే_ముగింపు
      తిరిగి శూన్యసూచిక;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[lib_నమోదు].lib = uprv_dl_open(libName, స్థితి_2);
#ఒకవేళ UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*స్థితి_2), lib_నమోదు, lib));
#అయితే_ముగింపు
        
    ఒకవేళ(libraryList[lib_నమోదు].lib == శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
      /* cleanup. */
      libraryList[lib_నమోదు].lib = శూన్యసూచిక; /* failure with open */
      libraryList[lib_నమోదు].name[0] = 0;
#ఒకవేళ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*స్థితి_2), lib_నమోదు, lib));
#అయితే_ముగింపు
      /* no need to free - just won't increase the count. */
      library_లెక్క--;
    } లేకపోతే { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[lib_నమోదు].name,libName,UPLUG_NAME_MAX);
      libraryList[lib_నమోదు].ref=1;
      lib = libraryList[lib_నమోదు].lib;
    }

  } లేకపోతే {
    lib = libraryList[lib_నమోదు].lib;
    libraryList[lib_నమోదు].ref++;
  }
  తిరిగి lib;
}

U_CAPI శూన్య U_EXPORT2
uplug_closeLibrary(శూన్య *lib, UError_సంకేతం *స్థితి_2) {
  int32_t i;
    
#ఒకవేళ UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*స్థితి_2), (శూన్య*)libraryList));
#అయితే_ముగింపు
  ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి;
    
  కొరకు(i=0;i<library_లెక్క;i++) {
    ఒకవేళ(lib==libraryList[i].lib) {
      ఒకవేళ(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, స్థితి_2);
        library_లెక్క = uplug_remove_నమోదు_At(libraryList, library_లెక్క, పరిమాణం(*libraryList), i);
      }
      తిరిగి;
    }
  }
  *స్థితి_2 = U_అంతర్గత_కార్యక్రమం_ERROR; /* could not find the entry! */
}

#అయితే_ముగింపు

స్థిర UPlug_దత్తాంశం pluginList[UPLUG_PLUGIN_INITIAL_లెక్క];
స్థిర int32_t plugin_లెక్క = 0;



  
స్థిర int32_t uplug_plugin_సంఖ్య(UPlug_దత్తాంశం* d) {
  UPlug_దత్తాంశం *pastPlug = &pluginList[plugin_లెక్క];
  ఒకవేళ(d<=pluginList) {
    తిరిగి 0;
  } లేకపోతే ఒకవేళ(d>=pastPlug) {
    తిరిగి plugin_లెక్క;
  } లేకపోతే {
    తిరిగి (d-pluginList)/పరిమాణం(pluginList[0]);
  }
}


U_CAPI UPlug_దత్తాంశం * U_EXPORT2
uplug_తదుపరి_Plug(UPlug_దత్తాంశం *పూర్వ) {
  ఒకవేళ(పూర్వ==శూన్యసూచిక) {
    తిరిగి pluginList;
  } లేకపోతే {
    UPlug_దత్తాంశం *తదుపరి_Plug = &పూర్వ[1];
    UPlug_దత్తాంశం *pastPlug = &pluginList[plugin_లెక్క];
    
    ఒకవేళ(తదుపరి_Plug>=pastPlug) {
      తిరిగి శూన్యసూచిక;
    } లేకపోతే {
      తిరిగి తదుపరి_Plug;
    }
  }
}



/**
 * Call the plugin with some params
 */
స్థిర శూన్య uplug_callPlug(UPlug_దత్తాంశం *plug, UPlugReason reason, UError_సంకేతం *స్థితి_2) {
  UPlugTokenReturn token;
  ఒకవేళ(plug==శూన్యసూచిక||U_FAILURE(*స్థితి_2)) {
    తిరిగి;
  }
  token = (*(plug->entrypoint))(plug, reason, స్థితి_2);
  ఒకవేళ(token!=UPLUG_TOKEN) {
    *స్థితి_2 = U_అంతర్గత_కార్యక్రమం_ERROR;
  }
}


స్థిర శూన్య uplug_unloadPlug(UPlug_దత్తాంశం *plug, UError_సంకేతం *స్థితి_2) {
  ఒకవేళ(plug->awaiting_ఎక్కించు) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *స్థితి_2 = U_అంతర్గత_కార్యక్రమం_ERROR;
    తిరిగి; 
  }
  ఒకవేళ(U_SUCCESS(plug->plugin_స్థితి)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, స్థితి_2);
  }
}

స్థిర శూన్య uplug_queryPlug(UPlug_దత్తాంశం *plug, UError_సంకేతం *స్థితి_2) {
  ఒకవేళ(!plug->awaiting_ఎక్కించు || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *స్థితి_2 = U_అంతర్గత_కార్యక్రమం_ERROR;
    తిరిగి; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, స్థితి_2);
  ఒకవేళ(U_SUCCESS(*స్థితి_2)) { 
    ఒకవేళ(plug->level == UPLUG_LEVEL_INVALID) {
      plug->plugin_స్థితి = U_PLUGIN_DIDNT_సమితి_LEVEL;
      plug->awaiting_ఎక్కించు = అబద్ధం;
    }
  } లేకపోతే {
    plug->plugin_స్థితి = U_అంతర్గత_కార్యక్రమం_ERROR;
    plug->awaiting_ఎక్కించు = అబద్ధం;
  }
}


స్థిర శూన్య uplug_ఎక్కించు_Plug(UPlug_దత్తాంశం *plug, UError_సంకేతం *స్థితి_2) {
  ఒకవేళ(U_FAILURE(*స్థితి_2)) {
    తిరిగి;
  }
  ఒకవేళ(!plug->awaiting_ఎక్కించు || (plug->level < UPLUG_LEVEL_దిగువ) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *స్థితి_2 = U_అంతర్గత_కార్యక్రమం_ERROR;
    తిరిగి;
  }
  uplug_callPlug(plug, UPLUG_REASON_ఎక్కించు, స్థితి_2);
  plug->awaiting_ఎక్కించు = అబద్ధం;
  ఒకవేళ(!U_SUCCESS(*స్థితి_2)) {
    plug->plugin_స్థితి = U_అంతర్గత_కార్యక్రమం_ERROR;
  }
}

స్థిర UPlug_దత్తాంశం *uplug_allocate_ఖాళీలు_Plug(UError_సంకేతం *స్థితి_2)
{
  UPlug_దత్తాంశం *plug = శూన్యసూచిక;

  ఒకవేళ(U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }

  ఒకవేళ(plugin_లెక్క == UPLUG_PLUGIN_INITIAL_లెక్క) {
    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి శూన్యసూచిక;
  }

  plug = &pluginList[plugin_లెక్క++];

  plug->token = UPLUG_TOKEN;
  plug->struct_కొలత = పరిమాణం(UPlug_దత్తాంశం);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaiting_ఎక్కించు = నిజం;
  plug->dontUnload = అబద్ధం;
  plug->plugin_స్థితి = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=శూన్యసూచిక;
  plug->entrypoint=శూన్యసూచిక;


  తిరిగి plug;
}

స్థిర UPlug_దత్తాంశం *uplug_allocatePlug(UPlugEntrypoint *entrypoint, స్థిరరాశి అక్షరం *config, శూన్య *lib, స్థిరరాశి అక్షరం *symName,
                                     UError_సంకేతం *స్థితి_2) {
  UPlug_దత్తాంశం *plug = uplug_allocate_ఖాళీలు_Plug(స్థితి_2);
  ఒకవేళ(U_FAILURE(*స్థితి_2)) {
    తిరిగి శూన్యసూచిక;
  }

  ఒకవేళ(config!=శూన్యసూచిక) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } లేకపోతే {
    plug->config[0] = 0;
  }
    
  ఒకవేళ(symName!=శూన్యసూచిక) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } లేకపోతే {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, స్థితి_2);
    
  తిరిగి plug;
}

స్థిర శూన్య uplug_deallocatePlug(UPlug_దత్తాంశం *plug, UError_సంకేతం *స్థితి_2) {
  UError_సంకేతం sub_స్థితి = U_ZERO_ERROR;
  ఒకవేళ(!plug->dontUnload) {
#ఒకవేళ U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &sub_స్థితి);
#అయితే_ముగింపు
  }
  plug->lib = శూన్యసూచిక;
  ఒకవేళ(U_SUCCESS(*స్థితి_2) && U_FAILURE(sub_స్థితి)) {
    *స్థితి_2 = sub_స్థితి;
  }
  /* shift plugins up and decrement count. */
  ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
    /* all ok- remove. */
    plugin_లెక్క = uplug_remove_నమోదు_At(pluginList, plugin_లెక్క, పరిమాణం(plug[0]), uplug_plugin_సంఖ్య(plug));
  } లేకపోతే {
    /* not ok- leave as a message. */
    plug->awaiting_ఎక్కించు=అబద్ధం;
    plug->entrypoint=0;
    plug->dontUnload=నిజం;
  }
}

స్థిర శూన్య uplug_doUnloadPlug(UPlug_దత్తాంశం *plug_గమ్యం_Remove, UError_సంకేతం *స్థితి_2) {
  ఒకవేళ(plug_గమ్యం_Remove != శూన్యసూచిక) {
    uplug_unloadPlug(plug_గమ్యం_Remove, స్థితి_2);
    uplug_deallocatePlug(plug_గమ్యం_Remove, స్థితి_2);
  }
}

U_CAPI శూన్య U_EXPORT2
uplug_removePlug(UPlug_దత్తాంశం *plug, UError_సంకేతం *స్థితి_2)  {
  UPlug_దత్తాంశం *cursor = శూన్యసూచిక;
  UPlug_దత్తాంశం *plug_గమ్యం_Remove = శూన్యసూచిక;
  ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి;
    
  కొరకు(cursor=pluginList;cursor!=శూన్యసూచిక;) {
    ఒకవేళ(cursor==plug) {
      plug_గమ్యం_Remove = plug;
      cursor=శూన్యసూచిక;
    } లేకపోతే {
      cursor = uplug_తదుపరి_Plug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plug_గమ్యం_Remove, స్థితి_2);
}




U_CAPI శూన్య U_EXPORT2 
uplug_సమితి_Plug_No_Unload(UPlug_దత్తాంశం *దత్తాంశం, UBool dontUnload)
{
  దత్తాంశం->dontUnload = dontUnload;
}


U_CAPI శూన్య U_EXPORT2
uplug_సమితి_Plug_Level(UPlug_దత్తాంశం *దత్తాంశం, UPlugLevel level) {
  దత్తాంశం->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlug_దత్తాంశం *దత్తాంశం) {
  తిరిగి దత్తాంశం->level;
}


U_CAPI శూన్య U_EXPORT2
uplug_సమితి_Plug_Name(UPlug_దత్తాంశం *దత్తాంశం, స్థిరరాశి అక్షరం *name) {
  uprv_strncpy(దత్తాంశం->name, name, UPLUG_NAME_MAX);
}


U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uplug_getPlugName(UPlug_దత్తాంశం* దత్తాంశం U_LIFETIME_BOUND) {
  తిరిగి దత్తాంశం->name;
}


U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uplug_get_చిహ్నం_Name(UPlug_దత్తాంశం* దత్తాంశం U_LIFETIME_BOUND) {
  తిరిగి దత్తాంశం->sym;
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uplug_getLibraryName(UPlug_దత్తాంశం* దత్తాంశం U_LIFETIME_BOUND, UError_సంకేతం* స్థితి_2) {
  ఒకవేళ(దత్తాంశం->libName[0]) {
    తిరిగి దత్తాంశం->libName;
  } లేకపోతే {
#ఒకవేళ U_ENABLE_DYLOAD
    తిరిగి uplug_కనుగొను_Library(దత్తాంశం->lib, స్థితి_2);
#లేకపోతే
    తిరిగి శూన్యసూచిక;
#అయితే_ముగింపు
  }
}

U_CAPI శూన్య * U_EXPORT2
uplug_getLibrary(UPlug_దత్తాంశం *దత్తాంశం) {
  తిరిగి దత్తాంశం->lib;
}

U_CAPI శూన్య * U_EXPORT2
uplug_getContext(UPlug_దత్తాంశం *దత్తాంశం) {
  తిరిగి దత్తాంశం->context;
}


U_CAPI శూన్య U_EXPORT2
uplug_సమితి_Context(UPlug_దత్తాంశం *దత్తాంశం, శూన్య *context) {
  దత్తాంశం->context = context;
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uplug_getConfiguration(UPlug_దత్తాంశం* దత్తాంశం U_LIFETIME_BOUND) {
  తిరిగి దత్తాంశం->config;
}

U_CAPI UPlug_దత్తాంశం* U_EXPORT2
uplug_get_Plug_అంతర్గత(int32_t n) { 
  ఒకవేళ(n <0 || n >= plugin_లెక్క) {
    తిరిగి శూన్యసూచిక;
  } లేకపోతే { 
    తిరిగి &(pluginList[n]);
  }
}


U_CAPI UError_సంకేతం U_EXPORT2
uplug_get_Plug_ఎక్కించు_స్థితి(UPlug_దత్తాంశం *plug) {
  తిరిగి plug->plugin_స్థితి;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
స్థిర UPlug_దత్తాంశం* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, స్థిరరాశి అక్షరం *config, శూన్య *lib, స్థిరరాశి అక్షరం *sym,
                                                         UError_సంకేతం *స్థితి_2) {
  UPlug_దత్తాంశం *plug = శూన్యసూచిక;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, స్థితి_2);

  ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
    తిరిగి plug;
  } లేకపోతే {
    uplug_deallocatePlug(plug, స్థితి_2);
    తిరిగి శూన్యసూచిక;
  }
}

U_CAPI UPlug_దత్తాంశం* U_EXPORT2
uplug_ఎక్కించు_Plug_From_Entrypoint(UPlugEntrypoint *entrypoint, స్థిరరాశి అక్షరం *config, UError_సంకేతం *స్థితి_2) {
  UPlug_దత్తాంశం* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, శూన్యసూచిక, శూన్యసూచిక, స్థితి_2);
  uplug_ఎక్కించు_Plug(plug, స్థితి_2);
  తిరిగి plug;
}

#ఒకవేళ U_ENABLE_DYLOAD

స్థిర UPlug_దత్తాంశం* 
uplug_initErrorPlug(స్థిరరాశి అక్షరం *libName, స్థిరరాశి అక్షరం *sym, స్థిరరాశి అక్షరం *config, స్థిరరాశి అక్షరం *nameOrError, UError_సంకేతం ఎక్కించు_స్థితి, UError_సంకేతం *స్థితి_2)
{
  UPlug_దత్తాంశం *plug = uplug_allocate_ఖాళీలు_Plug(స్థితి_2);
  ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి శూన్యసూచిక;

  plug->plugin_స్థితి = ఎక్కించు_స్థితి;
  plug->awaiting_ఎక్కించు = అబద్ధం; /* Won't load. */
  plug->dontUnload = నిజం; /* cannot unload. */

  ఒకవేళ(sym!=శూన్యసూచిక) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  ఒకవేళ(libName!=శూన్యసూచిక) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  ఒకవేళ(nameOrError!=శూన్యసూచిక) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  ఒకవేళ(config!=శూన్యసూచిక) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  తిరిగి plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
స్థిర UPlug_దత్తాంశం* 
uplug_initPlugFromLibrary(స్థిరరాశి అక్షరం *libName, స్థిరరాశి అక్షరం *sym, స్థిరరాశి అక్షరం *config, UError_సంకేతం *స్థితి_2) {
  శూన్య *lib = శూన్యసూచిక;
  UPlug_దత్తాంశం *plug = శూన్యసూచిక;
  ఒకవేళ(U_FAILURE(*స్థితి_2)) { తిరిగి శూన్యసూచిక; }
  lib = uplug_openLibrary(libName, స్థితి_2);
  ఒకవేళ(lib!=శూన్యసూచిక && U_SUCCESS(*స్థితి_2)) {
    UPlugEntrypoint *entrypoint = శూన్యసూచిక;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, స్థితి_2);

    ఒకవేళ(entrypoint!=శూన్యసూచిక&&U_SUCCESS(*స్థితి_2)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, స్థితి_2);
      ఒకవేళ(plug!=శూన్యసూచిక&&U_SUCCESS(*స్థితి_2)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = శూన్యసూచిక; /* library is now owned by plugin. */
      }
    } లేకపోతే {
      UError_సంకేతం sub_స్థితి = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==శూన్యసూచిక)?U_MISSING_RESOURCE_ERROR:*స్థితి_2,&sub_స్థితి);
    }
    ఒకవేళ(lib!=శూన్యసూచిక) { /* still need to close the lib */
      UError_సంకేతం sub_స్థితి = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &sub_స్థితి); /* don't care here */
    }
  } లేకపోతే {
    UError_సంకేతం sub_స్థితి = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==శూన్యసూచిక)?U_MISSING_RESOURCE_ERROR:*స్థితి_2,&sub_స్థితి);
  }
  తిరిగి plug;
}

U_CAPI UPlug_దత్తాంశం* U_EXPORT2
uplug_ఎక్కించు_Plug_From_Library(స్థిరరాశి అక్షరం *libName, స్థిరరాశి అక్షరం *sym, స్థిరరాశి అక్షరం *config, UError_సంకేతం *స్థితి_2) { 
  UPlug_దత్తాంశం *plug = శూన్యసూచిక;
  ఒకవేళ(U_FAILURE(*స్థితి_2)) { తిరిగి శూన్యసూచిక; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, స్థితి_2);
  uplug_ఎక్కించు_Plug(plug, స్థితి_2);

  తిరిగి plug;
}

#అయితే_ముగింపు

స్థిర UPlugLevel gCurrentLevel = UPLUG_LEVEL_దిగువ;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  తిరిగి gCurrentLevel;
}

స్థిర UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlug_దత్తాంశం *plugin_గమ్యం_Remove;
  /* cleanup plugs */
  కొరకు(i=0;i<plugin_లెక్క;i++) {
    UError_సంకేతం sub_స్థితి = U_ZERO_ERROR;
    plugin_గమ్యం_Remove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(plugin_గమ్యం_Remove, &sub_స్థితి);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_దిగువ;
  తిరిగి నిజం;
}

#ఒకవేళ U_ENABLE_DYLOAD

స్థిర శూన్య uplug_ఎక్కించు_నిరీక్షణ_Plugs(UError_సంకేతం *స్థితి_2) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  ఒకవేళ(U_FAILURE(*స్థితి_2)) {
    తిరిగి;
  }
#ఒకవేళ UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#అయితే_ముగింపు
  /* pass #1: low level plugs */
  కొరకు(i=0;i<plugin_లెక్క;i++) {
    UError_సంకేతం sub_స్థితి = U_ZERO_ERROR;
    UPlug_దత్తాంశం *plugin_గమ్యం_ఎక్కించు = &pluginList[i];
    ఒకవేళ(plugin_గమ్యం_ఎక్కించు->awaiting_ఎక్కించు) {
      ఒకవేళ(plugin_గమ్యం_ఎక్కించు->level == UPLUG_LEVEL_దిగువ) {
        ఒకవేళ(currentLevel > UPLUG_LEVEL_దిగువ) {
          plugin_గమ్యం_ఎక్కించు->plugin_స్థితి = U_PLUGIN_TOO_ఎగువ;
        } లేకపోతే {
          UPlugLevel newLevel;
          uplug_ఎక్కించు_Plug(plugin_గమ్యం_ఎక్కించు, &sub_స్థితి);
          newLevel = uplug_getCurrentLevel();
          ఒకవేళ(newLevel > currentLevel) {
            plugin_గమ్యం_ఎక్కించు->plugin_స్థితి = U_PLUGIN_మారింది_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        plugin_గమ్యం_ఎక్కించు->awaiting_ఎక్కించు = అబద్ధం;
      } 
    }
  }    
  కొరకు(i=0;i<plugin_లెక్క;i++) {
    UError_సంకేతం sub_స్థితి = U_ZERO_ERROR;
    UPlug_దత్తాంశం *plugin_గమ్యం_ఎక్కించు = &pluginList[i];
        
    ఒకవేళ(plugin_గమ్యం_ఎక్కించు->awaiting_ఎక్కించు) {
      ఒకవేళ(plugin_గమ్యం_ఎక్కించు->level == UPLUG_LEVEL_INVALID) { 
        plugin_గమ్యం_ఎక్కించు->plugin_స్థితి = U_PLUGIN_DIDNT_సమితి_LEVEL;
      } లేకపోతే ఒకవేళ(plugin_గమ్యం_ఎక్కించు->level == UPLUG_LEVEL_UNKNOWN) {
        plugin_గమ్యం_ఎక్కించు->plugin_స్థితి = U_అంతర్గత_కార్యక్రమం_ERROR;
      } లేకపోతే {
        uplug_ఎక్కించు_Plug(plugin_గమ్యం_ఎక్కించు, &sub_స్థితి);
      }
      plugin_గమ్యం_ఎక్కించు->awaiting_ఎక్కించు = అబద్ధం;
    }
  }
    
#ఒకవేళ UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#అయితే_ముగింపు
}

/* Name of the plugin config file */
స్థిర అక్షరం plugin_file[2048] = "";
#అయితే_ముగింపు

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uplug_getPluginFile() {
#ఒకవేళ U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  తిరిగి plugin_file;
#లేకపోతే
  తిరిగి శూన్యసూచిక;
#అయితే_ముగింపు
}


//  uplug_init()  is called first thing from u_init().

U_CAPI శూన్య U_EXPORT2
uplug_init(UError_సంకేతం *స్థితి_2) {
#ఒకవేళ !U_ENABLE_DYLOAD
  (శూన్య)స్థితి_2; /*వాడని*/
#లేదావేళ !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  స్థిరరాశి అక్షరం *env = getenv("ICU_PLUGINS");

  ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి;
  ఒకవేళ(env != శూన్యసూచిక) {
    plugin_dir.append(env, -1, *స్థితి_2);
  }
  ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి;

#ఒకవేళ defined(DEFAULT_ICU_PLUGINS) 
  ఒకవేళ(plugin_dir.is_ఖాళీలు()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *స్థితి_2);
  }
#అయితే_ముగింపు

#ఒకవేళ UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.దత్తాంశం()));
#అయితే_ముగింపు

  ఒకవేళ(!plugin_dir.is_ఖాళీలు()) {
    FILE *f;
        
    CharString pluginFile;
#నిర్వచితమైతే ICU_PLUGINS_DD
/* There are potentially a lot of ways to implement a plugin directory on OS390/zOS  */
/* Keeping in mind that unauthorized file access is logged, monitored, and enforced  */
/* I've chosen to open a DDNAME if BATCH and leave it alone for (presumably) UNIX    */
/* System Services.  Alternative techniques might be allocating a member in          */
/* SYS1.PARMLIB or setting an environment variable "ICU_PLUGIN_PATH" (?).  The       */
/* DDNAME can be connected to a file in the HFS if need be.                          */

    pluginFile.append("//DD:ICUPLUG", -1, *స్థితి_2);        /* JAM 20 Oct 2011 */
#లేకపోతే
    pluginFile.append(plugin_dir, *స్థితి_2);
    pluginFile.append(U_FILE_SEP_STRING, -1, *స్థితి_2);
    pluginFile.append("icuplugins", -1, *స్థితి_2);
    pluginFile.append(U_ICU_సంస్కరణ_SHORT, -1, *స్థితి_2);
    pluginFile.append(".txt", -1, *స్థితి_2);
#అయితే_ముగింపు

#ఒకవేళ UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*స్థితి_2)));
#అయితే_ముగింపు

    ఒకవేళ(U_FAILURE(*స్థితి_2)) {
      తిరిగి;
    }
    ఒకవేళ((size_t)pluginFile.length() > (పరిమాణం(plugin_file)-1)) {
      *స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
#ఒకవేళ UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*స్థితి_2)));
#అయితే_ముగింపు
      తిరిగి;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, పరిమాణం(plugin_file), *స్థితి_2);

#ఒకవేళ UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(plugin_file), (పూర్ణాంకం)పరిమాణం(plugin_file)));
#అయితే_ముగింపు
        
#నిర్వచితమైతే __MVS__
    ఒకవేళ (iscics()) /* 12 Nov 2011 JAM */
    {
        f = శూన్యసూచిక;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
        f = fopen(pluginFile.దత్తాంశం(), "r");
    }

    ఒకవేళ(f != శూన్యసూచిక) {
      అక్షరం linebuf[1024];
      అక్షరం *p, *libName=శూన్యసూచిక, *symName=శూన్యసూచిక, *config=శూన్యసూచిక;
      int32_t పంక్తి = 0;
            
            
      అయితే(fgets(linebuf,1023,f)) {
        పంక్తి++;

        ఒకవేళ(!*linebuf || *linebuf=='#') {
          కొనసాగు;
        } లేకపోతే {
          p = linebuf;
          అయితే(*p&&ఖాళీనా((పూర్ణాంకం)*p))
            p++;
          ఒకవేళ(!*p || *p=='#') కొనసాగు;
          libName = p;
          అయితే(*p&&!ఖాళీనా((పూర్ణాంకం)*p)) {
            p++;
          }
          ఒకవేళ(!*p || *p=='#') కొనసాగు; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          అయితే(*p&&ఖాళీనా((పూర్ణాంకం)*p)) {
            p++;
          }
          ఒకవేళ(!*p||*p=='#') కొనసాగు; /* no symname after libname +tab */
          symName = p;
          అయితే(*p&&!ఖాళీనా((పూర్ణాంకం)*p)) {
            p++;
          }
                    
          ఒకవేళ(*p) { /* has config */
            *p=0;
            ++p;
            అయితే(*p&&ఖాళీనా((పూర్ణాంకం)*p)) {
              p++;
            }
            ఒకవేళ(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          ఒకవేళ(config!=శూన్యసూచిక&&*config!=0) {
            p = config+అక్షరశ్రేణి_పొడవు(config);
            అయితే(p>config&&ఖాళీనా((పూర్ణాంకం)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UError_సంకేతం sub_స్థితి = U_ZERO_ERROR;
            UPlug_దత్తాంశం *plug = uplug_initPlugFromLibrary(libName, symName, config, &sub_స్థితి);
            ఒకవేళ(U_FAILURE(sub_స్థితి) && U_SUCCESS(*స్థితి_2)) {
              *స్థితి_2 = sub_స్థితి;
            }
#ఒకవేళ UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (శూన్య*)plug, u_errorName(sub_స్థితి)));
#లేకపోతే
            (శూన్య)plug; /*వాడని*/
#అయితే_ముగింపు
          }
        }
      }
      fclose(f);
    } లేకపోతే {
#ఒకవేళ UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#అయితే_ముగింపు
    }
  }
  uplug_ఎక్కించు_నిరీక్షణ_Plugs(స్థితి_2);
#అయితే_ముగింపు /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_ఎగువ;
  ucln_రిజిస్టరు_Cleanup(UCLN_UPLUG, uplug_cleanup);
}

#అయితే_ముగింపు


