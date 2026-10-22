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
#यदि_परिभाषितम् __MVS__  /* defined by z/OS compiler */
#परिभाषय _POSIX_SOURCE
#अन्तर्भाव <cics.h> /* 12 Nov 2011 JAM iscics() function */
#यदि_अन्तः
#अन्तर्भाव "charstr.h"

उपयुज्य नामाकाश icu;

#यदि_अपरिभाषितम् UPLUG_TRACE
#परिभाषय UPLUG_TRACE 0
#यदि_अन्तः

#यदि UPLUG_TRACE
#अन्तर्भाव <stdio.h>
#परिभाषय DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#यदि_अन्तः

/**
 * Internal structure of an ICU plugin. 
 */

संरचना UPlug_दत्त {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t struct_आकार;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  शून्य *lib;              /**< plugin library, or nullptr */
  वर्ण libName[UPLUG_NAME_MAX];   /**< library name */
  वर्ण sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  वर्ण config[UPLUG_NAME_MAX];     /**< configuration data */
  शून्य *context;          /**< user context data */
  वर्ण name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaiting_आरोपय; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UError_संहिता plugin_स्थितिः; /**< status code of plugin */
};



#परिभाषय UPLUG_LIBRARY_INITIAL_गणनम् 8
#परिभाषय UPLUG_PLUGIN_INITIAL_गणनम् 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
स्थिर int32_t uplug_remove_प्रविष्टिः_At(शून्य *list, int32_t list_आकार, int32_t member_आकार, int32_t item_गन्तव्यम्_Remove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  यदि(list_आकार<1) {
    प्रत्यागम list_आकार;
  }
    
  /* is there anything to move? */
  यदि(list_आकार > item_गन्तव्यम्_Remove+1) {
    memmove(bytePtr+(item_गन्तव्यम्_Remove*member_आकार), bytePtr+((item_गन्तव्यम्_Remove+1)*member_आकार), member_आकार);
  }
    
  प्रत्यागम list_आकार-1;
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
    यदि(!uprv_strcmp(libName, libraryList[i].name)) {
      प्रत्यागम i;
    }
  }
  प्रत्यागम -1;
}

स्थिर int32_t अन्वेषणम्_For_Library(शून्य *lib) {
  int32_t i;
    
  प्रति(i=0;i<library_गणनम्;i++) {
    यदि(lib==libraryList[i].lib) {
      प्रत्यागम i;
    }
  }
  प्रत्यागम -1;
}

U_CAPI वर्ण * U_EXPORT2
uplug_findLibrary(शून्य *lib, UError_संहिता *स्थितिः) {
  int32_t libEnt;
  वर्ण *ret = शून्यसूचकः;
  यदि(U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }
  libEnt = अन्वेषणम्_For_Library(lib);
  यदि(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } अन्यथा {
    *स्थितिः = U_MISSING_RESOURCE_ERROR;
  }
  प्रत्यागम ret;
}

U_CAPI शून्य * U_EXPORT2
uplug_openLibrary(ध्रुव वर्ण *libName, UError_संहिता *स्थितिः) {
  int32_t lib_प्रविष्टिः = -1;
  शून्य *lib = शून्यसूचकः;
    
  यदि(U_FAILURE(*स्थितिः)) प्रत्यागम शून्यसूचकः;

  lib_प्रविष्टिः = अन्वेषणम्_For_Library_Name(libName);
  यदि(lib_प्रविष्टिः == -1) {
    lib_प्रविष्टिः = library_गणनम्++;
    यदि(library_गणनम् >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
#यदि UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#यदि_अन्तः
      प्रत्यागम शून्यसूचकः;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[lib_प्रविष्टिः].lib = uprv_dl_open(libName, स्थितिः);
#यदि UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*स्थितिः), lib_प्रविष्टिः, lib));
#यदि_अन्तः
        
    यदि(libraryList[lib_प्रविष्टिः].lib == शून्यसूचकः || U_FAILURE(*स्थितिः)) {
      /* cleanup. */
      libraryList[lib_प्रविष्टिः].lib = शून्यसूचकः; /* failure with open */
      libraryList[lib_प्रविष्टिः].name[0] = 0;
#यदि UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*स्थितिः), lib_प्रविष्टिः, lib));
#यदि_अन्तः
      /* no need to free - just won't increase the count. */
      library_गणनम्--;
    } अन्यथा { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[lib_प्रविष्टिः].name,libName,UPLUG_NAME_MAX);
      libraryList[lib_प्रविष्टिः].ref=1;
      lib = libraryList[lib_प्रविष्टिः].lib;
    }

  } अन्यथा {
    lib = libraryList[lib_प्रविष्टिः].lib;
    libraryList[lib_प्रविष्टिः].ref++;
  }
  प्रत्यागम lib;
}

U_CAPI शून्य U_EXPORT2
uplug_closeLibrary(शून्य *lib, UError_संहिता *स्थितिः) {
  int32_t i;
    
#यदि UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*स्थितिः), (शून्य*)libraryList));
#यदि_अन्तः
  यदि(U_FAILURE(*स्थितिः)) प्रत्यागम;
    
  प्रति(i=0;i<library_गणनम्;i++) {
    यदि(lib==libraryList[i].lib) {
      यदि(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, स्थितिः);
        library_गणनम् = uplug_remove_प्रविष्टिः_At(libraryList, library_गणनम्, परिमाण(*libraryList), i);
      }
      प्रत्यागम;
    }
  }
  *स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR; /* could not find the entry! */
}

#यदि_अन्तः

स्थिर UPlug_दत्त pluginList[UPLUG_PLUGIN_INITIAL_गणनम्];
स्थिर int32_t plugin_गणनम् = 0;



  
स्थिर int32_t uplug_plugin_सङ्ख्या(UPlug_दत्त* d) {
  UPlug_दत्त *pastPlug = &pluginList[plugin_गणनम्];
  यदि(d<=pluginList) {
    प्रत्यागम 0;
  } अन्यथा यदि(d>=pastPlug) {
    प्रत्यागम plugin_गणनम्;
  } अन्यथा {
    प्रत्यागम (d-pluginList)/परिमाण(pluginList[0]);
  }
}


U_CAPI UPlug_दत्त * U_EXPORT2
uplug_अग्र_Plug(UPlug_दत्त *प्राक्) {
  यदि(प्राक्==शून्यसूचकः) {
    प्रत्यागम pluginList;
  } अन्यथा {
    UPlug_दत्त *अग्र_Plug = &प्राक्[1];
    UPlug_दत्त *pastPlug = &pluginList[plugin_गणनम्];
    
    यदि(अग्र_Plug>=pastPlug) {
      प्रत्यागम शून्यसूचकः;
    } अन्यथा {
      प्रत्यागम अग्र_Plug;
    }
  }
}



/**
 * Call the plugin with some params
 */
स्थिर शून्य uplug_callPlug(UPlug_दत्त *plug, UPlugReason reason, UError_संहिता *स्थितिः) {
  UPlugTokenReturn token;
  यदि(plug==शून्यसूचकः||U_FAILURE(*स्थितिः)) {
    प्रत्यागम;
  }
  token = (*(plug->entrypoint))(plug, reason, स्थितिः);
  यदि(token!=UPLUG_TOKEN) {
    *स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR;
  }
}


स्थिर शून्य uplug_unloadPlug(UPlug_दत्त *plug, UError_संहिता *स्थितिः) {
  यदि(plug->awaiting_आरोपय) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR;
    प्रत्यागम; 
  }
  यदि(U_SUCCESS(plug->plugin_स्थितिः)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, स्थितिः);
  }
}

स्थिर शून्य uplug_queryPlug(UPlug_दत्त *plug, UError_संहिता *स्थितिः) {
  यदि(!plug->awaiting_आरोपय || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR;
    प्रत्यागम; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, स्थितिः);
  यदि(U_SUCCESS(*स्थितिः)) { 
    यदि(plug->level == UPLUG_LEVEL_INVALID) {
      plug->plugin_स्थितिः = U_PLUGIN_DIDNT_समुच्चयः_LEVEL;
      plug->awaiting_आरोपय = असत्य;
    }
  } अन्यथा {
    plug->plugin_स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR;
    plug->awaiting_आरोपय = असत्य;
  }
}


स्थिर शून्य uplug_आरोपय_Plug(UPlug_दत्त *plug, UError_संहिता *स्थितिः) {
  यदि(U_FAILURE(*स्थितिः)) {
    प्रत्यागम;
  }
  यदि(!plug->awaiting_आरोपय || (plug->level < UPLUG_LEVEL_नीचः) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR;
    प्रत्यागम;
  }
  uplug_callPlug(plug, UPLUG_REASON_आरोपय, स्थितिः);
  plug->awaiting_आरोपय = असत्य;
  यदि(!U_SUCCESS(*स्थितिः)) {
    plug->plugin_स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR;
  }
}

स्थिर UPlug_दत्त *uplug_allocate_रिक्तानि_Plug(UError_संहिता *स्थितिः)
{
  UPlug_दत्त *plug = शून्यसूचकः;

  यदि(U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }

  यदि(plugin_गणनम् == UPLUG_PLUGIN_INITIAL_गणनम्) {
    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम शून्यसूचकः;
  }

  plug = &pluginList[plugin_गणनम्++];

  plug->token = UPLUG_TOKEN;
  plug->struct_आकार = परिमाण(UPlug_दत्त);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaiting_आरोपय = सत्य;
  plug->dontUnload = असत्य;
  plug->plugin_स्थितिः = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=शून्यसूचकः;
  plug->entrypoint=शून्यसूचकः;


  प्रत्यागम plug;
}

स्थिर UPlug_दत्त *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ध्रुव वर्ण *config, शून्य *lib, ध्रुव वर्ण *symName,
                                     UError_संहिता *स्थितिः) {
  UPlug_दत्त *plug = uplug_allocate_रिक्तानि_Plug(स्थितिः);
  यदि(U_FAILURE(*स्थितिः)) {
    प्रत्यागम शून्यसूचकः;
  }

  यदि(config!=शून्यसूचकः) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } अन्यथा {
    plug->config[0] = 0;
  }
    
  यदि(symName!=शून्यसूचकः) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } अन्यथा {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, स्थितिः);
    
  प्रत्यागम plug;
}

स्थिर शून्य uplug_deallocatePlug(UPlug_दत्त *plug, UError_संहिता *स्थितिः) {
  UError_संहिता sub_स्थितिः = U_ZERO_ERROR;
  यदि(!plug->dontUnload) {
#यदि U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &sub_स्थितिः);
#यदि_अन्तः
  }
  plug->lib = शून्यसूचकः;
  यदि(U_SUCCESS(*स्थितिः) && U_FAILURE(sub_स्थितिः)) {
    *स्थितिः = sub_स्थितिः;
  }
  /* shift plugins up and decrement count. */
  यदि(U_SUCCESS(*स्थितिः)) {
    /* all ok- remove. */
    plugin_गणनम् = uplug_remove_प्रविष्टिः_At(pluginList, plugin_गणनम्, परिमाण(plug[0]), uplug_plugin_सङ्ख्या(plug));
  } अन्यथा {
    /* not ok- leave as a message. */
    plug->awaiting_आरोपय=असत्य;
    plug->entrypoint=0;
    plug->dontUnload=सत्य;
  }
}

स्थिर शून्य uplug_doUnloadPlug(UPlug_दत्त *plug_गन्तव्यम्_Remove, UError_संहिता *स्थितिः) {
  यदि(plug_गन्तव्यम्_Remove != शून्यसूचकः) {
    uplug_unloadPlug(plug_गन्तव्यम्_Remove, स्थितिः);
    uplug_deallocatePlug(plug_गन्तव्यम्_Remove, स्थितिः);
  }
}

U_CAPI शून्य U_EXPORT2
uplug_removePlug(UPlug_दत्त *plug, UError_संहिता *स्थितिः)  {
  UPlug_दत्त *cursor = शून्यसूचकः;
  UPlug_दत्त *plug_गन्तव्यम्_Remove = शून्यसूचकः;
  यदि(U_FAILURE(*स्थितिः)) प्रत्यागम;
    
  प्रति(cursor=pluginList;cursor!=शून्यसूचकः;) {
    यदि(cursor==plug) {
      plug_गन्तव्यम्_Remove = plug;
      cursor=शून्यसूचकः;
    } अन्यथा {
      cursor = uplug_अग्र_Plug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plug_गन्तव्यम्_Remove, स्थितिः);
}




U_CAPI शून्य U_EXPORT2 
uplug_समुच्चयः_Plug_No_Unload(UPlug_दत्त *दत्त, UBool dontUnload)
{
  दत्त->dontUnload = dontUnload;
}


U_CAPI शून्य U_EXPORT2
uplug_समुच्चयः_Plug_Level(UPlug_दत्त *दत्त, UPlugLevel level) {
  दत्त->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlug_दत्त *दत्त) {
  प्रत्यागम दत्त->level;
}


U_CAPI शून्य U_EXPORT2
uplug_समुच्चयः_Plug_Name(UPlug_दत्त *दत्त, ध्रुव वर्ण *name) {
  uprv_strncpy(दत्त->name, name, UPLUG_NAME_MAX);
}


U_CAPI ध्रुव वर्ण* U_EXPORT2
uplug_getPlugName(UPlug_दत्त* दत्त U_LIFETIME_BOUND) {
  प्रत्यागम दत्त->name;
}


U_CAPI ध्रुव वर्ण* U_EXPORT2
uplug_get_चिह्नम्_Name(UPlug_दत्त* दत्त U_LIFETIME_BOUND) {
  प्रत्यागम दत्त->sym;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uplug_getLibraryName(UPlug_दत्त* दत्त U_LIFETIME_BOUND, UError_संहिता* स्थितिः) {
  यदि(दत्त->libName[0]) {
    प्रत्यागम दत्त->libName;
  } अन्यथा {
#यदि U_ENABLE_DYLOAD
    प्रत्यागम uplug_findLibrary(दत्त->lib, स्थितिः);
#अन्यथा
    प्रत्यागम शून्यसूचकः;
#यदि_अन्तः
  }
}

U_CAPI शून्य * U_EXPORT2
uplug_getLibrary(UPlug_दत्त *दत्त) {
  प्रत्यागम दत्त->lib;
}

U_CAPI शून्य * U_EXPORT2
uplug_getContext(UPlug_दत्त *दत्त) {
  प्रत्यागम दत्त->context;
}


U_CAPI शून्य U_EXPORT2
uplug_समुच्चयः_Context(UPlug_दत्त *दत्त, शून्य *context) {
  दत्त->context = context;
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uplug_getConfiguration(UPlug_दत्त* दत्त U_LIFETIME_BOUND) {
  प्रत्यागम दत्त->config;
}

U_CAPI UPlug_दत्त* U_EXPORT2
uplug_get_Plug_आन्तरिकम्(int32_t n) { 
  यदि(n <0 || n >= plugin_गणनम्) {
    प्रत्यागम शून्यसूचकः;
  } अन्यथा { 
    प्रत्यागम &(pluginList[n]);
  }
}


U_CAPI UError_संहिता U_EXPORT2
uplug_get_Plug_आरोपय_स्थितिः(UPlug_दत्त *plug) {
  प्रत्यागम plug->plugin_स्थितिः;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
स्थिर UPlug_दत्त* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ध्रुव वर्ण *config, शून्य *lib, ध्रुव वर्ण *sym,
                                                         UError_संहिता *स्थितिः) {
  UPlug_दत्त *plug = शून्यसूचकः;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, स्थितिः);

  यदि(U_SUCCESS(*स्थितिः)) {
    प्रत्यागम plug;
  } अन्यथा {
    uplug_deallocatePlug(plug, स्थितिः);
    प्रत्यागम शून्यसूचकः;
  }
}

U_CAPI UPlug_दत्त* U_EXPORT2
uplug_आरोपय_Plug_From_Entrypoint(UPlugEntrypoint *entrypoint, ध्रुव वर्ण *config, UError_संहिता *स्थितिः) {
  UPlug_दत्त* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, शून्यसूचकः, शून्यसूचकः, स्थितिः);
  uplug_आरोपय_Plug(plug, स्थितिः);
  प्रत्यागम plug;
}

#यदि U_ENABLE_DYLOAD

स्थिर UPlug_दत्त* 
uplug_initErrorPlug(ध्रुव वर्ण *libName, ध्रुव वर्ण *sym, ध्रुव वर्ण *config, ध्रुव वर्ण *nameOrError, UError_संहिता आरोपय_स्थितिः, UError_संहिता *स्थितिः)
{
  UPlug_दत्त *plug = uplug_allocate_रिक्तानि_Plug(स्थितिः);
  यदि(U_FAILURE(*स्थितिः)) प्रत्यागम शून्यसूचकः;

  plug->plugin_स्थितिः = आरोपय_स्थितिः;
  plug->awaiting_आरोपय = असत्य; /* Won't load. */
  plug->dontUnload = सत्य; /* cannot unload. */

  यदि(sym!=शून्यसूचकः) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  यदि(libName!=शून्यसूचकः) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  यदि(nameOrError!=शून्यसूचकः) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  यदि(config!=शून्यसूचकः) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  प्रत्यागम plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
स्थिर UPlug_दत्त* 
uplug_initPlugFromLibrary(ध्रुव वर्ण *libName, ध्रुव वर्ण *sym, ध्रुव वर्ण *config, UError_संहिता *स्थितिः) {
  शून्य *lib = शून्यसूचकः;
  UPlug_दत्त *plug = शून्यसूचकः;
  यदि(U_FAILURE(*स्थितिः)) { प्रत्यागम शून्यसूचकः; }
  lib = uplug_openLibrary(libName, स्थितिः);
  यदि(lib!=शून्यसूचकः && U_SUCCESS(*स्थितिः)) {
    UPlugEntrypoint *entrypoint = शून्यसूचकः;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, स्थितिः);

    यदि(entrypoint!=शून्यसूचकः&&U_SUCCESS(*स्थितिः)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, स्थितिः);
      यदि(plug!=शून्यसूचकः&&U_SUCCESS(*स्थितिः)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = शून्यसूचकः; /* library is now owned by plugin. */
      }
    } अन्यथा {
      UError_संहिता sub_स्थितिः = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==शून्यसूचकः)?U_MISSING_RESOURCE_ERROR:*स्थितिः,&sub_स्थितिः);
    }
    यदि(lib!=शून्यसूचकः) { /* still need to close the lib */
      UError_संहिता sub_स्थितिः = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &sub_स्थितिः); /* don't care here */
    }
  } अन्यथा {
    UError_संहिता sub_स्थितिः = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==शून्यसूचकः)?U_MISSING_RESOURCE_ERROR:*स्थितिः,&sub_स्थितिः);
  }
  प्रत्यागम plug;
}

U_CAPI UPlug_दत्त* U_EXPORT2
uplug_आरोपय_Plug_From_Library(ध्रुव वर्ण *libName, ध्रुव वर्ण *sym, ध्रुव वर्ण *config, UError_संहिता *स्थितिः) { 
  UPlug_दत्त *plug = शून्यसूचकः;
  यदि(U_FAILURE(*स्थितिः)) { प्रत्यागम शून्यसूचकः; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, स्थितिः);
  uplug_आरोपय_Plug(plug, स्थितिः);

  प्रत्यागम plug;
}

#यदि_अन्तः

स्थिर UPlugLevel gCurrentLevel = UPLUG_LEVEL_नीचः;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  प्रत्यागम gCurrentLevel;
}

स्थिर UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlug_दत्त *plugin_गन्तव्यम्_Remove;
  /* cleanup plugs */
  प्रति(i=0;i<plugin_गणनम्;i++) {
    UError_संहिता sub_स्थितिः = U_ZERO_ERROR;
    plugin_गन्तव्यम्_Remove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(plugin_गन्तव्यम्_Remove, &sub_स्थितिः);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_नीचः;
  प्रत्यागम सत्य;
}

#यदि U_ENABLE_DYLOAD

स्थिर शून्य uplug_आरोपय_प्रतीक्षा_Plugs(UError_संहिता *स्थितिः) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  यदि(U_FAILURE(*स्थितिः)) {
    प्रत्यागम;
  }
#यदि UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#यदि_अन्तः
  /* pass #1: low level plugs */
  प्रति(i=0;i<plugin_गणनम्;i++) {
    UError_संहिता sub_स्थितिः = U_ZERO_ERROR;
    UPlug_दत्त *plugin_गन्तव्यम्_आरोपय = &pluginList[i];
    यदि(plugin_गन्तव्यम्_आरोपय->awaiting_आरोपय) {
      यदि(plugin_गन्तव्यम्_आरोपय->level == UPLUG_LEVEL_नीचः) {
        यदि(currentLevel > UPLUG_LEVEL_नीचः) {
          plugin_गन्तव्यम्_आरोपय->plugin_स्थितिः = U_PLUGIN_TOO_उच्चः;
        } अन्यथा {
          UPlugLevel newLevel;
          uplug_आरोपय_Plug(plugin_गन्तव्यम्_आरोपय, &sub_स्थितिः);
          newLevel = uplug_getCurrentLevel();
          यदि(newLevel > currentLevel) {
            plugin_गन्तव्यम्_आरोपय->plugin_स्थितिः = U_PLUGIN_परिवर्तितम्_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        plugin_गन्तव्यम्_आरोपय->awaiting_आरोपय = असत्य;
      } 
    }
  }    
  प्रति(i=0;i<plugin_गणनम्;i++) {
    UError_संहिता sub_स्थितिः = U_ZERO_ERROR;
    UPlug_दत्त *plugin_गन्तव्यम्_आरोपय = &pluginList[i];
        
    यदि(plugin_गन्तव्यम्_आरोपय->awaiting_आरोपय) {
      यदि(plugin_गन्तव्यम्_आरोपय->level == UPLUG_LEVEL_INVALID) { 
        plugin_गन्तव्यम्_आरोपय->plugin_स्थितिः = U_PLUGIN_DIDNT_समुच्चयः_LEVEL;
      } अन्यथा यदि(plugin_गन्तव्यम्_आरोपय->level == UPLUG_LEVEL_UNKNOWN) {
        plugin_गन्तव्यम्_आरोपय->plugin_स्थितिः = U_आन्तरिकम्_क्रमादेशः_ERROR;
      } अन्यथा {
        uplug_आरोपय_Plug(plugin_गन्तव्यम्_आरोपय, &sub_स्थितिः);
      }
      plugin_गन्तव्यम्_आरोपय->awaiting_आरोपय = असत्य;
    }
  }
    
#यदि UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#यदि_अन्तः
}

/* Name of the plugin config file */
स्थिर वर्ण plugin_file[2048] = "";
#यदि_अन्तः

U_CAPI ध्रुव वर्ण* U_EXPORT2
uplug_getPluginFile() {
#यदि U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  प्रत्यागम plugin_file;
#अन्यथा
  प्रत्यागम शून्यसूचकः;
#यदि_अन्तः
}


//  uplug_init()  is called first thing from u_init().

U_CAPI शून्य U_EXPORT2
uplug_init(UError_संहिता *स्थितिः) {
#यदि !U_ENABLE_DYLOAD
  (शून्य)स्थितिः; /*अप्रयुक्तम्*/
#नोचेद्यदि !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ध्रुव वर्ण *env = getenv("ICU_PLUGINS");

  यदि(U_FAILURE(*स्थितिः)) प्रत्यागम;
  यदि(env != शून्यसूचकः) {
    plugin_dir.append(env, -1, *स्थितिः);
  }
  यदि(U_FAILURE(*स्थितिः)) प्रत्यागम;

#यदि defined(DEFAULT_ICU_PLUGINS) 
  यदि(plugin_dir.is_रिक्तानि()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *स्थितिः);
  }
#यदि_अन्तः

#यदि UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.दत्त()));
#यदि_अन्तः

  यदि(!plugin_dir.is_रिक्तानि()) {
    FILE *f;
        
    CharString pluginFile;
#यदि_परिभाषितम् ICU_PLUGINS_DD
/* There are potentially a lot of ways to implement a plugin directory on OS390/zOS  */
/* Keeping in mind that unauthorized file access is logged, monitored, and enforced  */
/* I've chosen to open a DDNAME if BATCH and leave it alone for (presumably) UNIX    */
/* System Services.  Alternative techniques might be allocating a member in          */
/* SYS1.PARMLIB or setting an environment variable "ICU_PLUGIN_PATH" (?).  The       */
/* DDNAME can be connected to a file in the HFS if need be.                          */

    pluginFile.append("//DD:ICUPLUG", -1, *स्थितिः);        /* JAM 20 Oct 2011 */
#अन्यथा
    pluginFile.append(plugin_dir, *स्थितिः);
    pluginFile.append(U_FILE_SEP_STRING, -1, *स्थितिः);
    pluginFile.append("icuplugins", -1, *स्थितिः);
    pluginFile.append(U_ICU_संस्करणम्_SHORT, -1, *स्थितिः);
    pluginFile.append(".txt", -1, *स्थितिः);
#यदि_अन्तः

#यदि UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*स्थितिः)));
#यदि_अन्तः

    यदि(U_FAILURE(*स्थितिः)) {
      प्रत्यागम;
    }
    यदि((size_t)pluginFile.length() > (परिमाण(plugin_file)-1)) {
      *स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
#यदि UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*स्थितिः)));
#यदि_अन्तः
      प्रत्यागम;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, परिमाण(plugin_file), *स्थितिः);

#यदि UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (पूर्णाङ्क)वाक्यदैर्घ्यम्(plugin_file), (पूर्णाङ्क)परिमाण(plugin_file)));
#यदि_अन्तः
        
#यदि_परिभाषितम् __MVS__
    यदि (iscics()) /* 12 Nov 2011 JAM */
    {
        f = शून्यसूचकः;
    }
    अन्यथा
#यदि_अन्तः
    {
        f = fopen(pluginFile.दत्त(), "r");
    }

    यदि(f != शून्यसूचकः) {
      वर्ण linebuf[1024];
      वर्ण *p, *libName=शून्यसूचकः, *symName=शून्यसूचकः, *config=शून्यसूचकः;
      int32_t पङ्क्तिः = 0;
            
            
      यावत्(fgets(linebuf,1023,f)) {
        पङ्क्तिः++;

        यदि(!*linebuf || *linebuf=='#') {
          अनुवर्त;
        } अन्यथा {
          p = linebuf;
          यावत्(*p&&रिक्तम्_वा((पूर्णाङ्क)*p))
            p++;
          यदि(!*p || *p=='#') अनुवर्त;
          libName = p;
          यावत्(*p&&!रिक्तम्_वा((पूर्णाङ्क)*p)) {
            p++;
          }
          यदि(!*p || *p=='#') अनुवर्त; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          यावत्(*p&&रिक्तम्_वा((पूर्णाङ्क)*p)) {
            p++;
          }
          यदि(!*p||*p=='#') अनुवर्त; /* no symname after libname +tab */
          symName = p;
          यावत्(*p&&!रिक्तम्_वा((पूर्णाङ्क)*p)) {
            p++;
          }
                    
          यदि(*p) { /* has config */
            *p=0;
            ++p;
            यावत्(*p&&रिक्तम्_वा((पूर्णाङ्क)*p)) {
              p++;
            }
            यदि(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          यदि(config!=शून्यसूचकः&&*config!=0) {
            p = config+वाक्यदैर्घ्यम्(config);
            यावत्(p>config&&रिक्तम्_वा((पूर्णाङ्क)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UError_संहिता sub_स्थितिः = U_ZERO_ERROR;
            UPlug_दत्त *plug = uplug_initPlugFromLibrary(libName, symName, config, &sub_स्थितिः);
            यदि(U_FAILURE(sub_स्थितिः) && U_SUCCESS(*स्थितिः)) {
              *स्थितिः = sub_स्थितिः;
            }
#यदि UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (शून्य*)plug, u_errorName(sub_स्थितिः)));
#अन्यथा
            (शून्य)plug; /*अप्रयुक्तम्*/
#यदि_अन्तः
          }
        }
      }
      fclose(f);
    } अन्यथा {
#यदि UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#यदि_अन्तः
    }
  }
  uplug_आरोपय_प्रतीक्षा_Plugs(स्थितिः);
#यदि_अन्तः /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_उच्चः;
  ucln_पञ्जिकाकोशः_Cleanup(UCLN_UPLUG, uplug_cleanup);
}

#यदि_अन्तः


