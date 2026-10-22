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
#اگر_تعریف_شده __MVS__  /* defined by z/OS compiler */
#تعریف_کن _POSIX_SOURCE
#ضمن <cics.h> /* 12 Nov 2011 JAM iscics() function */
#پایان_اگر
#ضمن "charstr.h"

استفاده نطاق icu;

#اگر_تعریف_نشده UPLUG_TRACE
#تعریف_کن UPLUG_TRACE 0
#پایان_اگر

#إذا UPLUG_TRACE
#ضمن <stdio.h>
#تعریف_کن DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#پایان_اگر

/**
 * Internal structure of an ICU plugin. 
 */

بنية UPlug_داده {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t struct_اندازه;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  فراغ *lib;              /**< plugin library, or nullptr */
  حرف libName[UPLUG_NAME_MAX];   /**< library name */
  حرف sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  حرف config[UPLUG_NAME_MAX];     /**< configuration data */
  فراغ *context;          /**< user context data */
  حرف name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaiting_بارگیری; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UError_کد plugin_وضعیت; /**< status code of plugin */
};



#تعریف_کن UPLUG_LIBRARY_INITIAL_شمار 8
#تعریف_کن UPLUG_PLUGIN_INITIAL_شمار 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
ساكن int32_t uplug_remove_مدخل_At(فراغ *list, int32_t list_اندازه, int32_t member_اندازه, int32_t قلم_مقصد_Remove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  إذا(list_اندازه<1) {
    أرجع list_اندازه;
  }
    
  /* is there anything to move? */
  إذا(list_اندازه > قلم_مقصد_Remove+1) {
    memmove(bytePtr+(قلم_مقصد_Remove*member_اندازه), bytePtr+((قلم_مقصد_Remove+1)*member_اندازه), member_اندازه);
  }
    
  أرجع list_اندازه-1;
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

ساكن UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_شمار];
ساكن UPlugLibrary * libraryList = staticLibraryList;
ساكن int32_t library_شمار = 0;
ساكن int32_t libraryMax = UPLUG_LIBRARY_INITIAL_شمار;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ساكن int32_t جستجو_For_Library_Name(ثابت حرف *libName) {
  int32_t i;
    
  لكل(i=0;i<library_شمار;i++) {
    إذا(!uprv_strcmp(libName, libraryList[i].name)) {
      أرجع i;
    }
  }
  أرجع -1;
}

ساكن int32_t جستجو_For_Library(فراغ *lib) {
  int32_t i;
    
  لكل(i=0;i<library_شمار;i++) {
    إذا(lib==libraryList[i].lib) {
      أرجع i;
    }
  }
  أرجع -1;
}

U_CAPI حرف * U_EXPORT2
uplug_پیدا_کن_Library(فراغ *lib, UError_کد *وضعیت) {
  int32_t libEnt;
  حرف *ret = اشاره_تهی;
  إذا(U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }
  libEnt = جستجو_For_Library(lib);
  إذا(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } وإلا {
    *وضعیت = U_MISSING_RESOURCE_ERROR;
  }
  أرجع ret;
}

U_CAPI فراغ * U_EXPORT2
uplug_openLibrary(ثابت حرف *libName, UError_کد *وضعیت) {
  int32_t lib_مدخل = -1;
  فراغ *lib = اشاره_تهی;
    
  إذا(U_FAILURE(*وضعیت)) أرجع اشاره_تهی;

  lib_مدخل = جستجو_For_Library_Name(libName);
  إذا(lib_مدخل == -1) {
    lib_مدخل = library_شمار++;
    إذا(library_شمار >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *وضعیت = U_حافظه_تخصیص_ERROR;
#إذا UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#پایان_اگر
      أرجع اشاره_تهی;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[lib_مدخل].lib = uprv_dl_open(libName, وضعیت);
#إذا UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*وضعیت), lib_مدخل, lib));
#پایان_اگر
        
    إذا(libraryList[lib_مدخل].lib == اشاره_تهی || U_FAILURE(*وضعیت)) {
      /* cleanup. */
      libraryList[lib_مدخل].lib = اشاره_تهی; /* failure with open */
      libraryList[lib_مدخل].name[0] = 0;
#إذا UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*وضعیت), lib_مدخل, lib));
#پایان_اگر
      /* no need to free - just won't increase the count. */
      library_شمار--;
    } وإلا { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[lib_مدخل].name,libName,UPLUG_NAME_MAX);
      libraryList[lib_مدخل].ref=1;
      lib = libraryList[lib_مدخل].lib;
    }

  } وإلا {
    lib = libraryList[lib_مدخل].lib;
    libraryList[lib_مدخل].ref++;
  }
  أرجع lib;
}

U_CAPI فراغ U_EXPORT2
uplug_closeLibrary(فراغ *lib, UError_کد *وضعیت) {
  int32_t i;
    
#إذا UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*وضعیت), (فراغ*)libraryList));
#پایان_اگر
  إذا(U_FAILURE(*وضعیت)) أرجع;
    
  لكل(i=0;i<library_شمار;i++) {
    إذا(lib==libraryList[i].lib) {
      إذا(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, وضعیت);
        library_شمار = uplug_remove_مدخل_At(libraryList, library_شمار, حجم(*libraryList), i);
      }
      أرجع;
    }
  }
  *وضعیت = U_داخلی_برنامه_ERROR; /* could not find the entry! */
}

#پایان_اگر

ساكن UPlug_داده pluginList[UPLUG_PLUGIN_INITIAL_شمار];
ساكن int32_t plugin_شمار = 0;



  
ساكن int32_t uplug_plugin_عدد(UPlug_داده* d) {
  UPlug_داده *pastPlug = &pluginList[plugin_شمار];
  إذا(d<=pluginList) {
    أرجع 0;
  } وإلا إذا(d>=pastPlug) {
    أرجع plugin_شمار;
  } وإلا {
    أرجع (d-pluginList)/حجم(pluginList[0]);
  }
}


U_CAPI UPlug_داده * U_EXPORT2
uplug_بعدی_Plug(UPlug_داده *پیشین) {
  إذا(پیشین==اشاره_تهی) {
    أرجع pluginList;
  } وإلا {
    UPlug_داده *بعدی_Plug = &پیشین[1];
    UPlug_داده *pastPlug = &pluginList[plugin_شمار];
    
    إذا(بعدی_Plug>=pastPlug) {
      أرجع اشاره_تهی;
    } وإلا {
      أرجع بعدی_Plug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ساكن فراغ uplug_callPlug(UPlug_داده *plug, UPlugReason reason, UError_کد *وضعیت) {
  UPlugTokenReturn token;
  إذا(plug==اشاره_تهی||U_FAILURE(*وضعیت)) {
    أرجع;
  }
  token = (*(plug->entrypoint))(plug, reason, وضعیت);
  إذا(token!=UPLUG_TOKEN) {
    *وضعیت = U_داخلی_برنامه_ERROR;
  }
}


ساكن فراغ uplug_unloadPlug(UPlug_داده *plug, UError_کد *وضعیت) {
  إذا(plug->awaiting_بارگیری) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *وضعیت = U_داخلی_برنامه_ERROR;
    أرجع; 
  }
  إذا(U_SUCCESS(plug->plugin_وضعیت)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, وضعیت);
  }
}

ساكن فراغ uplug_queryPlug(UPlug_داده *plug, UError_کد *وضعیت) {
  إذا(!plug->awaiting_بارگیری || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *وضعیت = U_داخلی_برنامه_ERROR;
    أرجع; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, وضعیت);
  إذا(U_SUCCESS(*وضعیت)) { 
    إذا(plug->level == UPLUG_LEVEL_INVALID) {
      plug->plugin_وضعیت = U_PLUGIN_DIDNT_مجموعه_LEVEL;
      plug->awaiting_بارگیری = خطأ;
    }
  } وإلا {
    plug->plugin_وضعیت = U_داخلی_برنامه_ERROR;
    plug->awaiting_بارگیری = خطأ;
  }
}


ساكن فراغ uplug_بارگیری_Plug(UPlug_داده *plug, UError_کد *وضعیت) {
  إذا(U_FAILURE(*وضعیت)) {
    أرجع;
  }
  إذا(!plug->awaiting_بارگیری || (plug->level < UPLUG_LEVEL_کم) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *وضعیت = U_داخلی_برنامه_ERROR;
    أرجع;
  }
  uplug_callPlug(plug, UPLUG_REASON_بارگیری, وضعیت);
  plug->awaiting_بارگیری = خطأ;
  إذا(!U_SUCCESS(*وضعیت)) {
    plug->plugin_وضعیت = U_داخلی_برنامه_ERROR;
  }
}

ساكن UPlug_داده *uplug_allocate_خالی_Plug(UError_کد *وضعیت)
{
  UPlug_داده *plug = اشاره_تهی;

  إذا(U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }

  إذا(plugin_شمار == UPLUG_PLUGIN_INITIAL_شمار) {
    *وضعیت = U_حافظه_تخصیص_ERROR;
    أرجع اشاره_تهی;
  }

  plug = &pluginList[plugin_شمار++];

  plug->token = UPLUG_TOKEN;
  plug->struct_اندازه = حجم(UPlug_داده);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaiting_بارگیری = درست;
  plug->dontUnload = خطأ;
  plug->plugin_وضعیت = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=اشاره_تهی;
  plug->entrypoint=اشاره_تهی;


  أرجع plug;
}

ساكن UPlug_داده *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *symName,
                                     UError_کد *وضعیت) {
  UPlug_داده *plug = uplug_allocate_خالی_Plug(وضعیت);
  إذا(U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }

  إذا(config!=اشاره_تهی) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } وإلا {
    plug->config[0] = 0;
  }
    
  إذا(symName!=اشاره_تهی) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } وإلا {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, وضعیت);
    
  أرجع plug;
}

ساكن فراغ uplug_deallocatePlug(UPlug_داده *plug, UError_کد *وضعیت) {
  UError_کد sub_وضعیت = U_ZERO_ERROR;
  إذا(!plug->dontUnload) {
#إذا U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &sub_وضعیت);
#پایان_اگر
  }
  plug->lib = اشاره_تهی;
  إذا(U_SUCCESS(*وضعیت) && U_FAILURE(sub_وضعیت)) {
    *وضعیت = sub_وضعیت;
  }
  /* shift plugins up and decrement count. */
  إذا(U_SUCCESS(*وضعیت)) {
    /* all ok- remove. */
    plugin_شمار = uplug_remove_مدخل_At(pluginList, plugin_شمار, حجم(plug[0]), uplug_plugin_عدد(plug));
  } وإلا {
    /* not ok- leave as a message. */
    plug->awaiting_بارگیری=خطأ;
    plug->entrypoint=0;
    plug->dontUnload=درست;
  }
}

ساكن فراغ uplug_doUnloadPlug(UPlug_داده *plug_مقصد_Remove, UError_کد *وضعیت) {
  إذا(plug_مقصد_Remove != اشاره_تهی) {
    uplug_unloadPlug(plug_مقصد_Remove, وضعیت);
    uplug_deallocatePlug(plug_مقصد_Remove, وضعیت);
  }
}

U_CAPI فراغ U_EXPORT2
uplug_removePlug(UPlug_داده *plug, UError_کد *وضعیت)  {
  UPlug_داده *cursor = اشاره_تهی;
  UPlug_داده *plug_مقصد_Remove = اشاره_تهی;
  إذا(U_FAILURE(*وضعیت)) أرجع;
    
  لكل(cursor=pluginList;cursor!=اشاره_تهی;) {
    إذا(cursor==plug) {
      plug_مقصد_Remove = plug;
      cursor=اشاره_تهی;
    } وإلا {
      cursor = uplug_بعدی_Plug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plug_مقصد_Remove, وضعیت);
}




U_CAPI فراغ U_EXPORT2 
uplug_مجموعه_Plug_No_Unload(UPlug_داده *داده, UBool dontUnload)
{
  داده->dontUnload = dontUnload;
}


U_CAPI فراغ U_EXPORT2
uplug_مجموعه_Plug_Level(UPlug_داده *داده, UPlugLevel level) {
  داده->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlug_داده *داده) {
  أرجع داده->level;
}


U_CAPI فراغ U_EXPORT2
uplug_مجموعه_Plug_Name(UPlug_داده *داده, ثابت حرف *name) {
  uprv_strncpy(داده->name, name, UPLUG_NAME_MAX);
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_getPlugName(UPlug_داده* داده U_LIFETIME_BOUND) {
  أرجع داده->name;
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_get_نماد_Name(UPlug_داده* داده U_LIFETIME_BOUND) {
  أرجع داده->sym;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getLibraryName(UPlug_داده* داده U_LIFETIME_BOUND, UError_کد* وضعیت) {
  إذا(داده->libName[0]) {
    أرجع داده->libName;
  } وإلا {
#إذا U_ENABLE_DYLOAD
    أرجع uplug_پیدا_کن_Library(داده->lib, وضعیت);
#وإلا
    أرجع اشاره_تهی;
#پایان_اگر
  }
}

U_CAPI فراغ * U_EXPORT2
uplug_getLibrary(UPlug_داده *داده) {
  أرجع داده->lib;
}

U_CAPI فراغ * U_EXPORT2
uplug_getContext(UPlug_داده *داده) {
  أرجع داده->context;
}


U_CAPI فراغ U_EXPORT2
uplug_مجموعه_Context(UPlug_داده *داده, فراغ *context) {
  داده->context = context;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getConfiguration(UPlug_داده* داده U_LIFETIME_BOUND) {
  أرجع داده->config;
}

U_CAPI UPlug_داده* U_EXPORT2
uplug_get_Plug_داخلی(int32_t n) { 
  إذا(n <0 || n >= plugin_شمار) {
    أرجع اشاره_تهی;
  } وإلا { 
    أرجع &(pluginList[n]);
  }
}


U_CAPI UError_کد U_EXPORT2
uplug_get_Plug_بارگیری_وضعیت(UPlug_داده *plug) {
  أرجع plug->plugin_وضعیت;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ساكن UPlug_داده* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *sym,
                                                         UError_کد *وضعیت) {
  UPlug_داده *plug = اشاره_تهی;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, وضعیت);

  إذا(U_SUCCESS(*وضعیت)) {
    أرجع plug;
  } وإلا {
    uplug_deallocatePlug(plug, وضعیت);
    أرجع اشاره_تهی;
  }
}

U_CAPI UPlug_داده* U_EXPORT2
uplug_بارگیری_Plug_From_Entrypoint(UPlugEntrypoint *entrypoint, ثابت حرف *config, UError_کد *وضعیت) {
  UPlug_داده* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, اشاره_تهی, اشاره_تهی, وضعیت);
  uplug_بارگیری_Plug(plug, وضعیت);
  أرجع plug;
}

#إذا U_ENABLE_DYLOAD

ساكن UPlug_داده* 
uplug_initErrorPlug(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, ثابت حرف *nameOrError, UError_کد بارگیری_وضعیت, UError_کد *وضعیت)
{
  UPlug_داده *plug = uplug_allocate_خالی_Plug(وضعیت);
  إذا(U_FAILURE(*وضعیت)) أرجع اشاره_تهی;

  plug->plugin_وضعیت = بارگیری_وضعیت;
  plug->awaiting_بارگیری = خطأ; /* Won't load. */
  plug->dontUnload = درست; /* cannot unload. */

  إذا(sym!=اشاره_تهی) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  إذا(libName!=اشاره_تهی) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  إذا(nameOrError!=اشاره_تهی) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  إذا(config!=اشاره_تهی) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  أرجع plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ساكن UPlug_داده* 
uplug_initPlugFromLibrary(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UError_کد *وضعیت) {
  فراغ *lib = اشاره_تهی;
  UPlug_داده *plug = اشاره_تهی;
  إذا(U_FAILURE(*وضعیت)) { أرجع اشاره_تهی; }
  lib = uplug_openLibrary(libName, وضعیت);
  إذا(lib!=اشاره_تهی && U_SUCCESS(*وضعیت)) {
    UPlugEntrypoint *entrypoint = اشاره_تهی;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, وضعیت);

    إذا(entrypoint!=اشاره_تهی&&U_SUCCESS(*وضعیت)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, وضعیت);
      إذا(plug!=اشاره_تهی&&U_SUCCESS(*وضعیت)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = اشاره_تهی; /* library is now owned by plugin. */
      }
    } وإلا {
      UError_کد sub_وضعیت = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==اشاره_تهی)?U_MISSING_RESOURCE_ERROR:*وضعیت,&sub_وضعیت);
    }
    إذا(lib!=اشاره_تهی) { /* still need to close the lib */
      UError_کد sub_وضعیت = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &sub_وضعیت); /* don't care here */
    }
  } وإلا {
    UError_کد sub_وضعیت = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==اشاره_تهی)?U_MISSING_RESOURCE_ERROR:*وضعیت,&sub_وضعیت);
  }
  أرجع plug;
}

U_CAPI UPlug_داده* U_EXPORT2
uplug_بارگیری_Plug_From_Library(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UError_کد *وضعیت) { 
  UPlug_داده *plug = اشاره_تهی;
  إذا(U_FAILURE(*وضعیت)) { أرجع اشاره_تهی; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, وضعیت);
  uplug_بارگیری_Plug(plug, وضعیت);

  أرجع plug;
}

#پایان_اگر

ساكن UPlugLevel gCurrentLevel = UPLUG_LEVEL_کم;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  أرجع gCurrentLevel;
}

ساكن UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlug_داده *plugin_مقصد_Remove;
  /* cleanup plugs */
  لكل(i=0;i<plugin_شمار;i++) {
    UError_کد sub_وضعیت = U_ZERO_ERROR;
    plugin_مقصد_Remove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(plugin_مقصد_Remove, &sub_وضعیت);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_کم;
  أرجع درست;
}

#إذا U_ENABLE_DYLOAD

ساكن فراغ uplug_بارگیری_انتظار_Plugs(UError_کد *وضعیت) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  إذا(U_FAILURE(*وضعیت)) {
    أرجع;
  }
#إذا UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#پایان_اگر
  /* pass #1: low level plugs */
  لكل(i=0;i<plugin_شمار;i++) {
    UError_کد sub_وضعیت = U_ZERO_ERROR;
    UPlug_داده *plugin_مقصد_بارگیری = &pluginList[i];
    إذا(plugin_مقصد_بارگیری->awaiting_بارگیری) {
      إذا(plugin_مقصد_بارگیری->level == UPLUG_LEVEL_کم) {
        إذا(currentLevel > UPLUG_LEVEL_کم) {
          plugin_مقصد_بارگیری->plugin_وضعیت = U_PLUGIN_TOO_بالا;
        } وإلا {
          UPlugLevel newLevel;
          uplug_بارگیری_Plug(plugin_مقصد_بارگیری, &sub_وضعیت);
          newLevel = uplug_getCurrentLevel();
          إذا(newLevel > currentLevel) {
            plugin_مقصد_بارگیری->plugin_وضعیت = U_PLUGIN_تغییریافته_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        plugin_مقصد_بارگیری->awaiting_بارگیری = خطأ;
      } 
    }
  }    
  لكل(i=0;i<plugin_شمار;i++) {
    UError_کد sub_وضعیت = U_ZERO_ERROR;
    UPlug_داده *plugin_مقصد_بارگیری = &pluginList[i];
        
    إذا(plugin_مقصد_بارگیری->awaiting_بارگیری) {
      إذا(plugin_مقصد_بارگیری->level == UPLUG_LEVEL_INVALID) { 
        plugin_مقصد_بارگیری->plugin_وضعیت = U_PLUGIN_DIDNT_مجموعه_LEVEL;
      } وإلا إذا(plugin_مقصد_بارگیری->level == UPLUG_LEVEL_UNKNOWN) {
        plugin_مقصد_بارگیری->plugin_وضعیت = U_داخلی_برنامه_ERROR;
      } وإلا {
        uplug_بارگیری_Plug(plugin_مقصد_بارگیری, &sub_وضعیت);
      }
      plugin_مقصد_بارگیری->awaiting_بارگیری = خطأ;
    }
  }
    
#إذا UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#پایان_اگر
}

/* Name of the plugin config file */
ساكن حرف plugin_file[2048] = "";
#پایان_اگر

U_CAPI ثابت حرف* U_EXPORT2
uplug_getPluginFile() {
#إذا U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  أرجع plugin_file;
#وإلا
  أرجع اشاره_تهی;
#پایان_اگر
}


//  uplug_init()  is called first thing from u_init().

U_CAPI فراغ U_EXPORT2
uplug_init(UError_کد *وضعیت) {
#إذا !U_ENABLE_DYLOAD
  (فراغ)وضعیت; /*استفادهنشده*/
#وإلاإذا !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ثابت حرف *env = getenv("ICU_PLUGINS");

  إذا(U_FAILURE(*وضعیت)) أرجع;
  إذا(env != اشاره_تهی) {
    plugin_dir.append(env, -1, *وضعیت);
  }
  إذا(U_FAILURE(*وضعیت)) أرجع;

#إذا defined(DEFAULT_ICU_PLUGINS) 
  إذا(plugin_dir.is_خالی()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *وضعیت);
  }
#پایان_اگر

#إذا UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.داده()));
#پایان_اگر

  إذا(!plugin_dir.is_خالی()) {
    FILE *f;
        
    CharString pluginFile;
#اگر_تعریف_شده ICU_PLUGINS_DD
/* There are potentially a lot of ways to implement a plugin directory on OS390/zOS  */
/* Keeping in mind that unauthorized file access is logged, monitored, and enforced  */
/* I've chosen to open a DDNAME if BATCH and leave it alone for (presumably) UNIX    */
/* System Services.  Alternative techniques might be allocating a member in          */
/* SYS1.PARMLIB or setting an environment variable "ICU_PLUGIN_PATH" (?).  The       */
/* DDNAME can be connected to a file in the HFS if need be.                          */

    pluginFile.append("//DD:ICUPLUG", -1, *وضعیت);        /* JAM 20 Oct 2011 */
#وإلا
    pluginFile.append(plugin_dir, *وضعیت);
    pluginFile.append(U_FILE_SEP_STRING, -1, *وضعیت);
    pluginFile.append("icuplugins", -1, *وضعیت);
    pluginFile.append(U_ICU_نسخه_SHORT, -1, *وضعیت);
    pluginFile.append(".txt", -1, *وضعیت);
#پایان_اگر

#إذا UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*وضعیت)));
#پایان_اگر

    إذا(U_FAILURE(*وضعیت)) {
      أرجع;
    }
    إذا((size_t)pluginFile.length() > (حجم(plugin_file)-1)) {
      *وضعیت = U_میانگیر_سرریز_ERROR;
#إذا UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*وضعیت)));
#پایان_اگر
      أرجع;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, حجم(plugin_file), *وضعیت);

#إذا UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (صحيح)طول_رشته(plugin_file), (صحيح)حجم(plugin_file)));
#پایان_اگر
        
#اگر_تعریف_شده __MVS__
    إذا (iscics()) /* 12 Nov 2011 JAM */
    {
        f = اشاره_تهی;
    }
    وإلا
#پایان_اگر
    {
        f = fopen(pluginFile.داده(), "r");
    }

    إذا(f != اشاره_تهی) {
      حرف linebuf[1024];
      حرف *p, *libName=اشاره_تهی, *symName=اشاره_تهی, *config=اشاره_تهی;
      int32_t سطر = 0;
            
            
      طالما(fgets(linebuf,1023,f)) {
        سطر++;

        إذا(!*linebuf || *linebuf=='#') {
          استمر;
        } وإلا {
          p = linebuf;
          طالما(*p&&آیا_فاصله((صحيح)*p))
            p++;
          إذا(!*p || *p=='#') استمر;
          libName = p;
          طالما(*p&&!آیا_فاصله((صحيح)*p)) {
            p++;
          }
          إذا(!*p || *p=='#') استمر; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          طالما(*p&&آیا_فاصله((صحيح)*p)) {
            p++;
          }
          إذا(!*p||*p=='#') استمر; /* no symname after libname +tab */
          symName = p;
          طالما(*p&&!آیا_فاصله((صحيح)*p)) {
            p++;
          }
                    
          إذا(*p) { /* has config */
            *p=0;
            ++p;
            طالما(*p&&آیا_فاصله((صحيح)*p)) {
              p++;
            }
            إذا(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          إذا(config!=اشاره_تهی&&*config!=0) {
            p = config+طول_رشته(config);
            طالما(p>config&&آیا_فاصله((صحيح)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UError_کد sub_وضعیت = U_ZERO_ERROR;
            UPlug_داده *plug = uplug_initPlugFromLibrary(libName, symName, config, &sub_وضعیت);
            إذا(U_FAILURE(sub_وضعیت) && U_SUCCESS(*وضعیت)) {
              *وضعیت = sub_وضعیت;
            }
#إذا UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (فراغ*)plug, u_errorName(sub_وضعیت)));
#وإلا
            (فراغ)plug; /*استفادهنشده*/
#پایان_اگر
          }
        }
      }
      fclose(f);
    } وإلا {
#إذا UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#پایان_اگر
    }
  }
  uplug_بارگیری_انتظار_Plugs(وضعیت);
#پایان_اگر /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_بالا;
  ucln_ثبات_Cleanup(UCLN_UPLUG, uplug_cleanup);
}

#پایان_اگر


