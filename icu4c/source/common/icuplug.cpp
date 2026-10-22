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

#ضمن "unicode/icuplug.h"


#إذا UCONFIG_ENABLE_PLUGINS


#ضمن "icuplugimp.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "putilimp.h"
#ضمن "ucln.h"
#ضمن <stdio.h>
#جيڪڏهن_وصف_ٿيل __MVS__  /* defined by z/OS compiler */
#وصف_ڪريو _POSIX_SOURCE
#ضمن <cics.h> /* 12 Nov 2011 JAM iscics() function */
#جيڪڏهن_پورو
#ضمن "charstr.h"

استعمال نطاق icu;

#جيڪڏهن_وصف_نه_ٿيل UPLUG_TRACE
#وصف_ڪريو UPLUG_TRACE 0
#جيڪڏهن_پورو

#إذا UPLUG_TRACE
#ضمن <stdio.h>
#وصف_ڪريو DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#جيڪڏهن_پورو

/**
 * Internal structure of an ICU plugin. 
 */

بنية UPlug_معلومات {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t struct_سائيز;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  فراغ *lib;              /**< plugin library, or nullptr */
  حرف libName[UPLUG_NAME_MAX];   /**< library name */
  حرف sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  حرف config[UPLUG_NAME_MAX];     /**< configuration data */
  فراغ *context;          /**< user context data */
  حرف name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaiting_ڀريو; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UError_ڪوڊ plugin_حالت; /**< status code of plugin */
};



#وصف_ڪريو UPLUG_LIBRARY_INITIAL_ڳڻپ 8
#وصف_ڪريو UPLUG_PLUGIN_INITIAL_ڳڻپ 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
ساكن int32_t uplug_remove_داخلا_At(فراغ *list, int32_t list_سائيز, int32_t member_سائيز, int32_t شي_منزل_Remove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  إذا(list_سائيز<1) {
    أرجع list_سائيز;
  }
    
  /* is there anything to move? */
  إذا(list_سائيز > شي_منزل_Remove+1) {
    memmove(bytePtr+(شي_منزل_Remove*member_سائيز), bytePtr+((شي_منزل_Remove+1)*member_سائيز), member_سائيز);
  }
    
  أرجع list_سائيز-1;
}




#إذا U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
بنية UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
تعريفنوع بنية UPlugLibrary {
  فراغ *lib;                           /**< library ptr */
  حرف name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

ساكن UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_ڳڻپ];
ساكن UPlugLibrary * libraryList = staticLibraryList;
ساكن int32_t library_ڳڻپ = 0;
ساكن int32_t libraryMax = UPLUG_LIBRARY_INITIAL_ڳڻپ;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ساكن int32_t ڳولا_For_Library_Name(ثابت حرف *libName) {
  int32_t i;
    
  لكل(i=0;i<library_ڳڻپ;i++) {
    إذا(!uprv_strcmp(libName, libraryList[i].name)) {
      أرجع i;
    }
  }
  أرجع -1;
}

ساكن int32_t ڳولا_For_Library(فراغ *lib) {
  int32_t i;
    
  لكل(i=0;i<library_ڳڻپ;i++) {
    إذا(lib==libraryList[i].lib) {
      أرجع i;
    }
  }
  أرجع -1;
}

U_CAPI حرف * U_EXPORT2
uplug_ڳوليو_Library(فراغ *lib, UError_ڪوڊ *حالت) {
  int32_t libEnt;
  حرف *ret = خالي_اشارو;
  إذا(U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }
  libEnt = ڳولا_For_Library(lib);
  إذا(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } وإلا {
    *حالت = U_MISSING_RESOURCE_ERROR;
  }
  أرجع ret;
}

U_CAPI فراغ * U_EXPORT2
uplug_openLibrary(ثابت حرف *libName, UError_ڪوڊ *حالت) {
  int32_t lib_داخلا = -1;
  فراغ *lib = خالي_اشارو;
    
  إذا(U_FAILURE(*حالت)) أرجع خالي_اشارو;

  lib_داخلا = ڳولا_For_Library_Name(libName);
  إذا(lib_داخلا == -1) {
    lib_داخلا = library_ڳڻپ++;
    إذا(library_ڳڻپ >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *حالت = U_يادگيري_ورهاست_ERROR;
#إذا UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#جيڪڏهن_پورو
      أرجع خالي_اشارو;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[lib_داخلا].lib = uprv_dl_open(libName, حالت);
#إذا UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*حالت), lib_داخلا, lib));
#جيڪڏهن_پورو
        
    إذا(libraryList[lib_داخلا].lib == خالي_اشارو || U_FAILURE(*حالت)) {
      /* cleanup. */
      libraryList[lib_داخلا].lib = خالي_اشارو; /* failure with open */
      libraryList[lib_داخلا].name[0] = 0;
#إذا UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*حالت), lib_داخلا, lib));
#جيڪڏهن_پورو
      /* no need to free - just won't increase the count. */
      library_ڳڻپ--;
    } وإلا { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[lib_داخلا].name,libName,UPLUG_NAME_MAX);
      libraryList[lib_داخلا].ref=1;
      lib = libraryList[lib_داخلا].lib;
    }

  } وإلا {
    lib = libraryList[lib_داخلا].lib;
    libraryList[lib_داخلا].ref++;
  }
  أرجع lib;
}

U_CAPI فراغ U_EXPORT2
uplug_closeLibrary(فراغ *lib, UError_ڪوڊ *حالت) {
  int32_t i;
    
#إذا UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*حالت), (فراغ*)libraryList));
#جيڪڏهن_پورو
  إذا(U_FAILURE(*حالت)) أرجع;
    
  لكل(i=0;i<library_ڳڻپ;i++) {
    إذا(lib==libraryList[i].lib) {
      إذا(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, حالت);
        library_ڳڻپ = uplug_remove_داخلا_At(libraryList, library_ڳڻپ, حجم(*libraryList), i);
      }
      أرجع;
    }
  }
  *حالت = U_اندروني_پروگرام_ERROR; /* could not find the entry! */
}

#جيڪڏهن_پورو

ساكن UPlug_معلومات pluginList[UPLUG_PLUGIN_INITIAL_ڳڻپ];
ساكن int32_t plugin_ڳڻپ = 0;



  
ساكن int32_t uplug_plugin_عدد(UPlug_معلومات* d) {
  UPlug_معلومات *pastPlug = &pluginList[plugin_ڳڻپ];
  إذا(d<=pluginList) {
    أرجع 0;
  } وإلا إذا(d>=pastPlug) {
    أرجع plugin_ڳڻپ;
  } وإلا {
    أرجع (d-pluginList)/حجم(pluginList[0]);
  }
}


U_CAPI UPlug_معلومات * U_EXPORT2
uplug_ايندڙ_Plug(UPlug_معلومات *اڳواٽ) {
  إذا(اڳواٽ==خالي_اشارو) {
    أرجع pluginList;
  } وإلا {
    UPlug_معلومات *ايندڙ_Plug = &اڳواٽ[1];
    UPlug_معلومات *pastPlug = &pluginList[plugin_ڳڻپ];
    
    إذا(ايندڙ_Plug>=pastPlug) {
      أرجع خالي_اشارو;
    } وإلا {
      أرجع ايندڙ_Plug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ساكن فراغ uplug_callPlug(UPlug_معلومات *plug, UPlugReason reason, UError_ڪوڊ *حالت) {
  UPlugTokenReturn token;
  إذا(plug==خالي_اشارو||U_FAILURE(*حالت)) {
    أرجع;
  }
  token = (*(plug->entrypoint))(plug, reason, حالت);
  إذا(token!=UPLUG_TOKEN) {
    *حالت = U_اندروني_پروگرام_ERROR;
  }
}


ساكن فراغ uplug_unloadPlug(UPlug_معلومات *plug, UError_ڪوڊ *حالت) {
  إذا(plug->awaiting_ڀريو) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت = U_اندروني_پروگرام_ERROR;
    أرجع; 
  }
  إذا(U_SUCCESS(plug->plugin_حالت)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, حالت);
  }
}

ساكن فراغ uplug_queryPlug(UPlug_معلومات *plug, UError_ڪوڊ *حالت) {
  إذا(!plug->awaiting_ڀريو || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت = U_اندروني_پروگرام_ERROR;
    أرجع; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, حالت);
  إذا(U_SUCCESS(*حالت)) { 
    إذا(plug->level == UPLUG_LEVEL_INVALID) {
      plug->plugin_حالت = U_PLUGIN_DIDNT_سيٽ_LEVEL;
      plug->awaiting_ڀريو = خطأ;
    }
  } وإلا {
    plug->plugin_حالت = U_اندروني_پروگرام_ERROR;
    plug->awaiting_ڀريو = خطأ;
  }
}


ساكن فراغ uplug_ڀريو_Plug(UPlug_معلومات *plug, UError_ڪوڊ *حالت) {
  إذا(U_FAILURE(*حالت)) {
    أرجع;
  }
  إذا(!plug->awaiting_ڀريو || (plug->level < UPLUG_LEVEL_هيٺيون) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت = U_اندروني_پروگرام_ERROR;
    أرجع;
  }
  uplug_callPlug(plug, UPLUG_REASON_ڀريو, حالت);
  plug->awaiting_ڀريو = خطأ;
  إذا(!U_SUCCESS(*حالت)) {
    plug->plugin_حالت = U_اندروني_پروگرام_ERROR;
  }
}

ساكن UPlug_معلومات *uplug_allocate_خالي_جايون_Plug(UError_ڪوڊ *حالت)
{
  UPlug_معلومات *plug = خالي_اشارو;

  إذا(U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }

  إذا(plugin_ڳڻپ == UPLUG_PLUGIN_INITIAL_ڳڻپ) {
    *حالت = U_يادگيري_ورهاست_ERROR;
    أرجع خالي_اشارو;
  }

  plug = &pluginList[plugin_ڳڻپ++];

  plug->token = UPLUG_TOKEN;
  plug->struct_سائيز = حجم(UPlug_معلومات);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaiting_ڀريو = سچ;
  plug->dontUnload = خطأ;
  plug->plugin_حالت = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=خالي_اشارو;
  plug->entrypoint=خالي_اشارو;


  أرجع plug;
}

ساكن UPlug_معلومات *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *symName,
                                     UError_ڪوڊ *حالت) {
  UPlug_معلومات *plug = uplug_allocate_خالي_جايون_Plug(حالت);
  إذا(U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }

  إذا(config!=خالي_اشارو) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } وإلا {
    plug->config[0] = 0;
  }
    
  إذا(symName!=خالي_اشارو) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } وإلا {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, حالت);
    
  أرجع plug;
}

ساكن فراغ uplug_deallocatePlug(UPlug_معلومات *plug, UError_ڪوڊ *حالت) {
  UError_ڪوڊ sub_حالت = U_ZERO_ERROR;
  إذا(!plug->dontUnload) {
#إذا U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &sub_حالت);
#جيڪڏهن_پورو
  }
  plug->lib = خالي_اشارو;
  إذا(U_SUCCESS(*حالت) && U_FAILURE(sub_حالت)) {
    *حالت = sub_حالت;
  }
  /* shift plugins up and decrement count. */
  إذا(U_SUCCESS(*حالت)) {
    /* all ok- remove. */
    plugin_ڳڻپ = uplug_remove_داخلا_At(pluginList, plugin_ڳڻپ, حجم(plug[0]), uplug_plugin_عدد(plug));
  } وإلا {
    /* not ok- leave as a message. */
    plug->awaiting_ڀريو=خطأ;
    plug->entrypoint=0;
    plug->dontUnload=سچ;
  }
}

ساكن فراغ uplug_doUnloadPlug(UPlug_معلومات *plug_منزل_Remove, UError_ڪوڊ *حالت) {
  إذا(plug_منزل_Remove != خالي_اشارو) {
    uplug_unloadPlug(plug_منزل_Remove, حالت);
    uplug_deallocatePlug(plug_منزل_Remove, حالت);
  }
}

U_CAPI فراغ U_EXPORT2
uplug_removePlug(UPlug_معلومات *plug, UError_ڪوڊ *حالت)  {
  UPlug_معلومات *cursor = خالي_اشارو;
  UPlug_معلومات *plug_منزل_Remove = خالي_اشارو;
  إذا(U_FAILURE(*حالت)) أرجع;
    
  لكل(cursor=pluginList;cursor!=خالي_اشارو;) {
    إذا(cursor==plug) {
      plug_منزل_Remove = plug;
      cursor=خالي_اشارو;
    } وإلا {
      cursor = uplug_ايندڙ_Plug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plug_منزل_Remove, حالت);
}




U_CAPI فراغ U_EXPORT2 
uplug_سيٽ_Plug_No_Unload(UPlug_معلومات *معلومات, UBool dontUnload)
{
  معلومات->dontUnload = dontUnload;
}


U_CAPI فراغ U_EXPORT2
uplug_سيٽ_Plug_Level(UPlug_معلومات *معلومات, UPlugLevel level) {
  معلومات->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlug_معلومات *معلومات) {
  أرجع معلومات->level;
}


U_CAPI فراغ U_EXPORT2
uplug_سيٽ_Plug_Name(UPlug_معلومات *معلومات, ثابت حرف *name) {
  uprv_strncpy(معلومات->name, name, UPLUG_NAME_MAX);
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_getPlugName(UPlug_معلومات* معلومات U_LIFETIME_BOUND) {
  أرجع معلومات->name;
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_get_نشان_Name(UPlug_معلومات* معلومات U_LIFETIME_BOUND) {
  أرجع معلومات->sym;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getLibraryName(UPlug_معلومات* معلومات U_LIFETIME_BOUND, UError_ڪوڊ* حالت) {
  إذا(معلومات->libName[0]) {
    أرجع معلومات->libName;
  } وإلا {
#إذا U_ENABLE_DYLOAD
    أرجع uplug_ڳوليو_Library(معلومات->lib, حالت);
#وإلا
    أرجع خالي_اشارو;
#جيڪڏهن_پورو
  }
}

U_CAPI فراغ * U_EXPORT2
uplug_getLibrary(UPlug_معلومات *معلومات) {
  أرجع معلومات->lib;
}

U_CAPI فراغ * U_EXPORT2
uplug_getContext(UPlug_معلومات *معلومات) {
  أرجع معلومات->context;
}


U_CAPI فراغ U_EXPORT2
uplug_سيٽ_Context(UPlug_معلومات *معلومات, فراغ *context) {
  معلومات->context = context;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getConfiguration(UPlug_معلومات* معلومات U_LIFETIME_BOUND) {
  أرجع معلومات->config;
}

U_CAPI UPlug_معلومات* U_EXPORT2
uplug_get_Plug_اندروني(int32_t n) { 
  إذا(n <0 || n >= plugin_ڳڻپ) {
    أرجع خالي_اشارو;
  } وإلا { 
    أرجع &(pluginList[n]);
  }
}


U_CAPI UError_ڪوڊ U_EXPORT2
uplug_get_Plug_ڀريو_حالت(UPlug_معلومات *plug) {
  أرجع plug->plugin_حالت;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ساكن UPlug_معلومات* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *sym,
                                                         UError_ڪوڊ *حالت) {
  UPlug_معلومات *plug = خالي_اشارو;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, حالت);

  إذا(U_SUCCESS(*حالت)) {
    أرجع plug;
  } وإلا {
    uplug_deallocatePlug(plug, حالت);
    أرجع خالي_اشارو;
  }
}

U_CAPI UPlug_معلومات* U_EXPORT2
uplug_ڀريو_Plug_From_Entrypoint(UPlugEntrypoint *entrypoint, ثابت حرف *config, UError_ڪوڊ *حالت) {
  UPlug_معلومات* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, خالي_اشارو, خالي_اشارو, حالت);
  uplug_ڀريو_Plug(plug, حالت);
  أرجع plug;
}

#إذا U_ENABLE_DYLOAD

ساكن UPlug_معلومات* 
uplug_initErrorPlug(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, ثابت حرف *nameOrError, UError_ڪوڊ ڀريو_حالت, UError_ڪوڊ *حالت)
{
  UPlug_معلومات *plug = uplug_allocate_خالي_جايون_Plug(حالت);
  إذا(U_FAILURE(*حالت)) أرجع خالي_اشارو;

  plug->plugin_حالت = ڀريو_حالت;
  plug->awaiting_ڀريو = خطأ; /* Won't load. */
  plug->dontUnload = سچ; /* cannot unload. */

  إذا(sym!=خالي_اشارو) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  إذا(libName!=خالي_اشارو) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  إذا(nameOrError!=خالي_اشارو) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  إذا(config!=خالي_اشارو) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  أرجع plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ساكن UPlug_معلومات* 
uplug_initPlugFromLibrary(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UError_ڪوڊ *حالت) {
  فراغ *lib = خالي_اشارو;
  UPlug_معلومات *plug = خالي_اشارو;
  إذا(U_FAILURE(*حالت)) { أرجع خالي_اشارو; }
  lib = uplug_openLibrary(libName, حالت);
  إذا(lib!=خالي_اشارو && U_SUCCESS(*حالت)) {
    UPlugEntrypoint *entrypoint = خالي_اشارو;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, حالت);

    إذا(entrypoint!=خالي_اشارو&&U_SUCCESS(*حالت)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, حالت);
      إذا(plug!=خالي_اشارو&&U_SUCCESS(*حالت)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = خالي_اشارو; /* library is now owned by plugin. */
      }
    } وإلا {
      UError_ڪوڊ sub_حالت = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==خالي_اشارو)?U_MISSING_RESOURCE_ERROR:*حالت,&sub_حالت);
    }
    إذا(lib!=خالي_اشارو) { /* still need to close the lib */
      UError_ڪوڊ sub_حالت = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &sub_حالت); /* don't care here */
    }
  } وإلا {
    UError_ڪوڊ sub_حالت = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==خالي_اشارو)?U_MISSING_RESOURCE_ERROR:*حالت,&sub_حالت);
  }
  أرجع plug;
}

U_CAPI UPlug_معلومات* U_EXPORT2
uplug_ڀريو_Plug_From_Library(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UError_ڪوڊ *حالت) { 
  UPlug_معلومات *plug = خالي_اشارو;
  إذا(U_FAILURE(*حالت)) { أرجع خالي_اشارو; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, حالت);
  uplug_ڀريو_Plug(plug, حالت);

  أرجع plug;
}

#جيڪڏهن_پورو

ساكن UPlugLevel gCurrentLevel = UPLUG_LEVEL_هيٺيون;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  أرجع gCurrentLevel;
}

ساكن UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlug_معلومات *plugin_منزل_Remove;
  /* cleanup plugs */
  لكل(i=0;i<plugin_ڳڻپ;i++) {
    UError_ڪوڊ sub_حالت = U_ZERO_ERROR;
    plugin_منزل_Remove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(plugin_منزل_Remove, &sub_حالت);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_هيٺيون;
  أرجع سچ;
}

#إذا U_ENABLE_DYLOAD

ساكن فراغ uplug_ڀريو_انتظار_Plugs(UError_ڪوڊ *حالت) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  إذا(U_FAILURE(*حالت)) {
    أرجع;
  }
#إذا UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#جيڪڏهن_پورو
  /* pass #1: low level plugs */
  لكل(i=0;i<plugin_ڳڻپ;i++) {
    UError_ڪوڊ sub_حالت = U_ZERO_ERROR;
    UPlug_معلومات *plugin_منزل_ڀريو = &pluginList[i];
    إذا(plugin_منزل_ڀريو->awaiting_ڀريو) {
      إذا(plugin_منزل_ڀريو->level == UPLUG_LEVEL_هيٺيون) {
        إذا(currentLevel > UPLUG_LEVEL_هيٺيون) {
          plugin_منزل_ڀريو->plugin_حالت = U_PLUGIN_TOO_مٿاهون;
        } وإلا {
          UPlugLevel newLevel;
          uplug_ڀريو_Plug(plugin_منزل_ڀريو, &sub_حالت);
          newLevel = uplug_getCurrentLevel();
          إذا(newLevel > currentLevel) {
            plugin_منزل_ڀريو->plugin_حالت = U_PLUGIN_بدليو_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        plugin_منزل_ڀريو->awaiting_ڀريو = خطأ;
      } 
    }
  }    
  لكل(i=0;i<plugin_ڳڻپ;i++) {
    UError_ڪوڊ sub_حالت = U_ZERO_ERROR;
    UPlug_معلومات *plugin_منزل_ڀريو = &pluginList[i];
        
    إذا(plugin_منزل_ڀريو->awaiting_ڀريو) {
      إذا(plugin_منزل_ڀريو->level == UPLUG_LEVEL_INVALID) { 
        plugin_منزل_ڀريو->plugin_حالت = U_PLUGIN_DIDNT_سيٽ_LEVEL;
      } وإلا إذا(plugin_منزل_ڀريو->level == UPLUG_LEVEL_UNKNOWN) {
        plugin_منزل_ڀريو->plugin_حالت = U_اندروني_پروگرام_ERROR;
      } وإلا {
        uplug_ڀريو_Plug(plugin_منزل_ڀريو, &sub_حالت);
      }
      plugin_منزل_ڀريو->awaiting_ڀريو = خطأ;
    }
  }
    
#إذا UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#جيڪڏهن_پورو
}

/* Name of the plugin config file */
ساكن حرف plugin_file[2048] = "";
#جيڪڏهن_پورو

U_CAPI ثابت حرف* U_EXPORT2
uplug_getPluginFile() {
#إذا U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  أرجع plugin_file;
#وإلا
  أرجع خالي_اشارو;
#جيڪڏهن_پورو
}


//  uplug_init()  is called first thing from u_init().

U_CAPI فراغ U_EXPORT2
uplug_init(UError_ڪوڊ *حالت) {
#إذا !U_ENABLE_DYLOAD
  (فراغ)حالت; /*اڻ_استعمال_ٿيل*/
#وإلاإذا !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ثابت حرف *env = getenv("ICU_PLUGINS");

  إذا(U_FAILURE(*حالت)) أرجع;
  إذا(env != خالي_اشارو) {
    plugin_dir.append(env, -1, *حالت);
  }
  إذا(U_FAILURE(*حالت)) أرجع;

#إذا defined(DEFAULT_ICU_PLUGINS) 
  إذا(plugin_dir.is_خالي_جايون()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *حالت);
  }
#جيڪڏهن_پورو

#إذا UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.معلومات()));
#جيڪڏهن_پورو

  إذا(!plugin_dir.is_خالي_جايون()) {
    FILE *f;
        
    CharString pluginFile;
#جيڪڏهن_وصف_ٿيل ICU_PLUGINS_DD
/* There are potentially a lot of ways to implement a plugin directory on OS390/zOS  */
/* Keeping in mind that unauthorized file access is logged, monitored, and enforced  */
/* I've chosen to open a DDNAME if BATCH and leave it alone for (presumably) UNIX    */
/* System Services.  Alternative techniques might be allocating a member in          */
/* SYS1.PARMLIB or setting an environment variable "ICU_PLUGIN_PATH" (?).  The       */
/* DDNAME can be connected to a file in the HFS if need be.                          */

    pluginFile.append("//DD:ICUPLUG", -1, *حالت);        /* JAM 20 Oct 2011 */
#وإلا
    pluginFile.append(plugin_dir, *حالت);
    pluginFile.append(U_FILE_SEP_STRING, -1, *حالت);
    pluginFile.append("icuplugins", -1, *حالت);
    pluginFile.append(U_ICU_نسخو_SHORT, -1, *حالت);
    pluginFile.append(".txt", -1, *حالت);
#جيڪڏهن_پورو

#إذا UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*حالت)));
#جيڪڏهن_پورو

    إذا(U_FAILURE(*حالت)) {
      أرجع;
    }
    إذا((size_t)pluginFile.length() > (حجم(plugin_file)-1)) {
      *حالت = U_بفر_حد_کان_مٿي_ERROR;
#إذا UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*حالت)));
#جيڪڏهن_پورو
      أرجع;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, حجم(plugin_file), *حالت);

#إذا UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (صحيح)لڙي_ڊيگهه(plugin_file), (صحيح)حجم(plugin_file)));
#جيڪڏهن_پورو
        
#جيڪڏهن_وصف_ٿيل __MVS__
    إذا (iscics()) /* 12 Nov 2011 JAM */
    {
        f = خالي_اشارو;
    }
    وإلا
#جيڪڏهن_پورو
    {
        f = fopen(pluginFile.معلومات(), "r");
    }

    إذا(f != خالي_اشارو) {
      حرف linebuf[1024];
      حرف *p, *libName=خالي_اشارو, *symName=خالي_اشارو, *config=خالي_اشارو;
      int32_t سٽ = 0;
            
            
      طالما(fgets(linebuf,1023,f)) {
        سٽ++;

        إذا(!*linebuf || *linebuf=='#') {
          استمر;
        } وإلا {
          p = linebuf;
          طالما(*p&&خال_آهي((صحيح)*p))
            p++;
          إذا(!*p || *p=='#') استمر;
          libName = p;
          طالما(*p&&!خال_آهي((صحيح)*p)) {
            p++;
          }
          إذا(!*p || *p=='#') استمر; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          طالما(*p&&خال_آهي((صحيح)*p)) {
            p++;
          }
          إذا(!*p||*p=='#') استمر; /* no symname after libname +tab */
          symName = p;
          طالما(*p&&!خال_آهي((صحيح)*p)) {
            p++;
          }
                    
          إذا(*p) { /* has config */
            *p=0;
            ++p;
            طالما(*p&&خال_آهي((صحيح)*p)) {
              p++;
            }
            إذا(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          إذا(config!=خالي_اشارو&&*config!=0) {
            p = config+لڙي_ڊيگهه(config);
            طالما(p>config&&خال_آهي((صحيح)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UError_ڪوڊ sub_حالت = U_ZERO_ERROR;
            UPlug_معلومات *plug = uplug_initPlugFromLibrary(libName, symName, config, &sub_حالت);
            إذا(U_FAILURE(sub_حالت) && U_SUCCESS(*حالت)) {
              *حالت = sub_حالت;
            }
#إذا UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (فراغ*)plug, u_errorName(sub_حالت)));
#وإلا
            (فراغ)plug; /*اڻ_استعمال_ٿيل*/
#جيڪڏهن_پورو
          }
        }
      }
      fclose(f);
    } وإلا {
#إذا UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#جيڪڏهن_پورو
    }
  }
  uplug_ڀريو_انتظار_Plugs(حالت);
#جيڪڏهن_پورو /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_مٿاهون;
  ucln_رجسٽر_Cleanup(UCLN_UPLUG, uplug_cleanup);
}

#جيڪڏهن_پورو


