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
#که_تعریف_شوی __MVS__  /* defined by z/OS compiler */
#تعریف_کړه _POSIX_SOURCE
#ضمن <cics.h> /* 12 Nov 2011 JAM iscics() function */
#د_که_پای
#ضمن "charstr.h"

کارول نطاق icu;

#که_نه_تعریف_شوی UPLUG_TRACE
#تعریف_کړه UPLUG_TRACE 0
#د_که_پای

#إذا UPLUG_TRACE
#ضمن <stdio.h>
#تعریف_کړه DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#د_که_پای

/**
 * Internal structure of an ICU plugin. 
 */

بنية UPlug_معلومات {
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
  UBool   awaiting_بارول; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UError_کوډ plugin_حالت; /**< status code of plugin */
};



#تعریف_کړه UPLUG_LIBRARY_INITIAL_شمېر 8
#تعریف_کړه UPLUG_PLUGIN_INITIAL_شمېر 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
ساكن int32_t uplug_remove_ثبت_At(فراغ *list, int32_t list_اندازه, int32_t member_اندازه, int32_t توکی_منزل_Remove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  إذا(list_اندازه<1) {
    أرجع list_اندازه;
  }
    
  /* is there anything to move? */
  إذا(list_اندازه > توکی_منزل_Remove+1) {
    memmove(bytePtr+(توکی_منزل_Remove*member_اندازه), bytePtr+((توکی_منزل_Remove+1)*member_اندازه), member_اندازه);
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

ساكن UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_شمېر];
ساكن UPlugLibrary * libraryList = staticLibraryList;
ساكن int32_t library_شمېر = 0;
ساكن int32_t libraryMax = UPLUG_LIBRARY_INITIAL_شمېر;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ساكن int32_t پلټنه_For_Library_Name(ثابت حرف *libName) {
  int32_t i;
    
  لكل(i=0;i<library_شمېر;i++) {
    إذا(!uprv_strcmp(libName, libraryList[i].name)) {
      أرجع i;
    }
  }
  أرجع -1;
}

ساكن int32_t پلټنه_For_Library(فراغ *lib) {
  int32_t i;
    
  لكل(i=0;i<library_شمېر;i++) {
    إذا(lib==libraryList[i].lib) {
      أرجع i;
    }
  }
  أرجع -1;
}

U_CAPI حرف * U_EXPORT2
uplug_پیدا_کړه_Library(فراغ *lib, UError_کوډ *حالت) {
  int32_t libEnt;
  حرف *ret = تش_اشاره;
  إذا(U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }
  libEnt = پلټنه_For_Library(lib);
  إذا(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } وإلا {
    *حالت = U_MISSING_RESOURCE_ERROR;
  }
  أرجع ret;
}

U_CAPI فراغ * U_EXPORT2
uplug_openLibrary(ثابت حرف *libName, UError_کوډ *حالت) {
  int32_t lib_ثبت = -1;
  فراغ *lib = تش_اشاره;
    
  إذا(U_FAILURE(*حالت)) أرجع تش_اشاره;

  lib_ثبت = پلټنه_For_Library_Name(libName);
  إذا(lib_ثبت == -1) {
    lib_ثبت = library_شمېر++;
    إذا(library_شمېر >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *حالت = U_حافظه_ویش_ERROR;
#إذا UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#د_که_پای
      أرجع تش_اشاره;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[lib_ثبت].lib = uprv_dl_open(libName, حالت);
#إذا UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*حالت), lib_ثبت, lib));
#د_که_پای
        
    إذا(libraryList[lib_ثبت].lib == تش_اشاره || U_FAILURE(*حالت)) {
      /* cleanup. */
      libraryList[lib_ثبت].lib = تش_اشاره; /* failure with open */
      libraryList[lib_ثبت].name[0] = 0;
#إذا UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*حالت), lib_ثبت, lib));
#د_که_پای
      /* no need to free - just won't increase the count. */
      library_شمېر--;
    } وإلا { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[lib_ثبت].name,libName,UPLUG_NAME_MAX);
      libraryList[lib_ثبت].ref=1;
      lib = libraryList[lib_ثبت].lib;
    }

  } وإلا {
    lib = libraryList[lib_ثبت].lib;
    libraryList[lib_ثبت].ref++;
  }
  أرجع lib;
}

U_CAPI فراغ U_EXPORT2
uplug_closeLibrary(فراغ *lib, UError_کوډ *حالت) {
  int32_t i;
    
#إذا UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*حالت), (فراغ*)libraryList));
#د_که_پای
  إذا(U_FAILURE(*حالت)) أرجع;
    
  لكل(i=0;i<library_شمېر;i++) {
    إذا(lib==libraryList[i].lib) {
      إذا(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, حالت);
        library_شمېر = uplug_remove_ثبت_At(libraryList, library_شمېر, حجم(*libraryList), i);
      }
      أرجع;
    }
  }
  *حالت = U_دننی_پروګرام_ERROR; /* could not find the entry! */
}

#د_که_پای

ساكن UPlug_معلومات pluginList[UPLUG_PLUGIN_INITIAL_شمېر];
ساكن int32_t plugin_شمېر = 0;



  
ساكن int32_t uplug_plugin_شمېره(UPlug_معلومات* d) {
  UPlug_معلومات *pastPlug = &pluginList[plugin_شمېر];
  إذا(d<=pluginList) {
    أرجع 0;
  } وإلا إذا(d>=pastPlug) {
    أرجع plugin_شمېر;
  } وإلا {
    أرجع (d-pluginList)/حجم(pluginList[0]);
  }
}


U_CAPI UPlug_معلومات * U_EXPORT2
uplug_بل_Plug(UPlug_معلومات *مخکینی) {
  إذا(مخکینی==تش_اشاره) {
    أرجع pluginList;
  } وإلا {
    UPlug_معلومات *بل_Plug = &مخکینی[1];
    UPlug_معلومات *pastPlug = &pluginList[plugin_شمېر];
    
    إذا(بل_Plug>=pastPlug) {
      أرجع تش_اشاره;
    } وإلا {
      أرجع بل_Plug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ساكن فراغ uplug_callPlug(UPlug_معلومات *plug, UPlugReason reason, UError_کوډ *حالت) {
  UPlugTokenReturn token;
  إذا(plug==تش_اشاره||U_FAILURE(*حالت)) {
    أرجع;
  }
  token = (*(plug->entrypoint))(plug, reason, حالت);
  إذا(token!=UPLUG_TOKEN) {
    *حالت = U_دننی_پروګرام_ERROR;
  }
}


ساكن فراغ uplug_unloadPlug(UPlug_معلومات *plug, UError_کوډ *حالت) {
  إذا(plug->awaiting_بارول) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت = U_دننی_پروګرام_ERROR;
    أرجع; 
  }
  إذا(U_SUCCESS(plug->plugin_حالت)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, حالت);
  }
}

ساكن فراغ uplug_queryPlug(UPlug_معلومات *plug, UError_کوډ *حالت) {
  إذا(!plug->awaiting_بارول || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت = U_دننی_پروګرام_ERROR;
    أرجع; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, حالت);
  إذا(U_SUCCESS(*حالت)) { 
    إذا(plug->level == UPLUG_LEVEL_INVALID) {
      plug->plugin_حالت = U_PLUGIN_DIDNT_سېټ_LEVEL;
      plug->awaiting_بارول = خطأ;
    }
  } وإلا {
    plug->plugin_حالت = U_دننی_پروګرام_ERROR;
    plug->awaiting_بارول = خطأ;
  }
}


ساكن فراغ uplug_بارول_Plug(UPlug_معلومات *plug, UError_کوډ *حالت) {
  إذا(U_FAILURE(*حالت)) {
    أرجع;
  }
  إذا(!plug->awaiting_بارول || (plug->level < UPLUG_LEVEL_ټیټ) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت = U_دننی_پروګرام_ERROR;
    أرجع;
  }
  uplug_callPlug(plug, UPLUG_REASON_بارول, حالت);
  plug->awaiting_بارول = خطأ;
  إذا(!U_SUCCESS(*حالت)) {
    plug->plugin_حالت = U_دننی_پروګرام_ERROR;
  }
}

ساكن UPlug_معلومات *uplug_allocate_تشه_Plug(UError_کوډ *حالت)
{
  UPlug_معلومات *plug = تش_اشاره;

  إذا(U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }

  إذا(plugin_شمېر == UPLUG_PLUGIN_INITIAL_شمېر) {
    *حالت = U_حافظه_ویش_ERROR;
    أرجع تش_اشاره;
  }

  plug = &pluginList[plugin_شمېر++];

  plug->token = UPLUG_TOKEN;
  plug->struct_اندازه = حجم(UPlug_معلومات);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaiting_بارول = ریښتیا;
  plug->dontUnload = خطأ;
  plug->plugin_حالت = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=تش_اشاره;
  plug->entrypoint=تش_اشاره;


  أرجع plug;
}

ساكن UPlug_معلومات *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *symName,
                                     UError_کوډ *حالت) {
  UPlug_معلومات *plug = uplug_allocate_تشه_Plug(حالت);
  إذا(U_FAILURE(*حالت)) {
    أرجع تش_اشاره;
  }

  إذا(config!=تش_اشاره) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } وإلا {
    plug->config[0] = 0;
  }
    
  إذا(symName!=تش_اشاره) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } وإلا {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, حالت);
    
  أرجع plug;
}

ساكن فراغ uplug_deallocatePlug(UPlug_معلومات *plug, UError_کوډ *حالت) {
  UError_کوډ sub_حالت = U_ZERO_ERROR;
  إذا(!plug->dontUnload) {
#إذا U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &sub_حالت);
#د_که_پای
  }
  plug->lib = تش_اشاره;
  إذا(U_SUCCESS(*حالت) && U_FAILURE(sub_حالت)) {
    *حالت = sub_حالت;
  }
  /* shift plugins up and decrement count. */
  إذا(U_SUCCESS(*حالت)) {
    /* all ok- remove. */
    plugin_شمېر = uplug_remove_ثبت_At(pluginList, plugin_شمېر, حجم(plug[0]), uplug_plugin_شمېره(plug));
  } وإلا {
    /* not ok- leave as a message. */
    plug->awaiting_بارول=خطأ;
    plug->entrypoint=0;
    plug->dontUnload=ریښتیا;
  }
}

ساكن فراغ uplug_doUnloadPlug(UPlug_معلومات *plug_منزل_Remove, UError_کوډ *حالت) {
  إذا(plug_منزل_Remove != تش_اشاره) {
    uplug_unloadPlug(plug_منزل_Remove, حالت);
    uplug_deallocatePlug(plug_منزل_Remove, حالت);
  }
}

U_CAPI فراغ U_EXPORT2
uplug_removePlug(UPlug_معلومات *plug, UError_کوډ *حالت)  {
  UPlug_معلومات *cursor = تش_اشاره;
  UPlug_معلومات *plug_منزل_Remove = تش_اشاره;
  إذا(U_FAILURE(*حالت)) أرجع;
    
  لكل(cursor=pluginList;cursor!=تش_اشاره;) {
    إذا(cursor==plug) {
      plug_منزل_Remove = plug;
      cursor=تش_اشاره;
    } وإلا {
      cursor = uplug_بل_Plug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plug_منزل_Remove, حالت);
}




U_CAPI فراغ U_EXPORT2 
uplug_سېټ_Plug_No_Unload(UPlug_معلومات *معلومات, UBool dontUnload)
{
  معلومات->dontUnload = dontUnload;
}


U_CAPI فراغ U_EXPORT2
uplug_سېټ_Plug_Level(UPlug_معلومات *معلومات, UPlugLevel level) {
  معلومات->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlug_معلومات *معلومات) {
  أرجع معلومات->level;
}


U_CAPI فراغ U_EXPORT2
uplug_سېټ_Plug_Name(UPlug_معلومات *معلومات, ثابت حرف *name) {
  uprv_strncpy(معلومات->name, name, UPLUG_NAME_MAX);
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_getPlugName(UPlug_معلومات* معلومات U_LIFETIME_BOUND) {
  أرجع معلومات->name;
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_get_نښه_Name(UPlug_معلومات* معلومات U_LIFETIME_BOUND) {
  أرجع معلومات->sym;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getLibraryName(UPlug_معلومات* معلومات U_LIFETIME_BOUND, UError_کوډ* حالت) {
  إذا(معلومات->libName[0]) {
    أرجع معلومات->libName;
  } وإلا {
#إذا U_ENABLE_DYLOAD
    أرجع uplug_پیدا_کړه_Library(معلومات->lib, حالت);
#وإلا
    أرجع تش_اشاره;
#د_که_پای
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
uplug_سېټ_Context(UPlug_معلومات *معلومات, فراغ *context) {
  معلومات->context = context;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getConfiguration(UPlug_معلومات* معلومات U_LIFETIME_BOUND) {
  أرجع معلومات->config;
}

U_CAPI UPlug_معلومات* U_EXPORT2
uplug_get_Plug_دننی(int32_t n) { 
  إذا(n <0 || n >= plugin_شمېر) {
    أرجع تش_اشاره;
  } وإلا { 
    أرجع &(pluginList[n]);
  }
}


U_CAPI UError_کوډ U_EXPORT2
uplug_get_Plug_بارول_حالت(UPlug_معلومات *plug) {
  أرجع plug->plugin_حالت;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ساكن UPlug_معلومات* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *sym,
                                                         UError_کوډ *حالت) {
  UPlug_معلومات *plug = تش_اشاره;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, حالت);

  إذا(U_SUCCESS(*حالت)) {
    أرجع plug;
  } وإلا {
    uplug_deallocatePlug(plug, حالت);
    أرجع تش_اشاره;
  }
}

U_CAPI UPlug_معلومات* U_EXPORT2
uplug_بارول_Plug_From_Entrypoint(UPlugEntrypoint *entrypoint, ثابت حرف *config, UError_کوډ *حالت) {
  UPlug_معلومات* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, تش_اشاره, تش_اشاره, حالت);
  uplug_بارول_Plug(plug, حالت);
  أرجع plug;
}

#إذا U_ENABLE_DYLOAD

ساكن UPlug_معلومات* 
uplug_initErrorPlug(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, ثابت حرف *nameOrError, UError_کوډ بارول_حالت, UError_کوډ *حالت)
{
  UPlug_معلومات *plug = uplug_allocate_تشه_Plug(حالت);
  إذا(U_FAILURE(*حالت)) أرجع تش_اشاره;

  plug->plugin_حالت = بارول_حالت;
  plug->awaiting_بارول = خطأ; /* Won't load. */
  plug->dontUnload = ریښتیا; /* cannot unload. */

  إذا(sym!=تش_اشاره) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  إذا(libName!=تش_اشاره) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  إذا(nameOrError!=تش_اشاره) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  إذا(config!=تش_اشاره) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  أرجع plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ساكن UPlug_معلومات* 
uplug_initPlugFromLibrary(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UError_کوډ *حالت) {
  فراغ *lib = تش_اشاره;
  UPlug_معلومات *plug = تش_اشاره;
  إذا(U_FAILURE(*حالت)) { أرجع تش_اشاره; }
  lib = uplug_openLibrary(libName, حالت);
  إذا(lib!=تش_اشاره && U_SUCCESS(*حالت)) {
    UPlugEntrypoint *entrypoint = تش_اشاره;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, حالت);

    إذا(entrypoint!=تش_اشاره&&U_SUCCESS(*حالت)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, حالت);
      إذا(plug!=تش_اشاره&&U_SUCCESS(*حالت)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = تش_اشاره; /* library is now owned by plugin. */
      }
    } وإلا {
      UError_کوډ sub_حالت = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==تش_اشاره)?U_MISSING_RESOURCE_ERROR:*حالت,&sub_حالت);
    }
    إذا(lib!=تش_اشاره) { /* still need to close the lib */
      UError_کوډ sub_حالت = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &sub_حالت); /* don't care here */
    }
  } وإلا {
    UError_کوډ sub_حالت = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==تش_اشاره)?U_MISSING_RESOURCE_ERROR:*حالت,&sub_حالت);
  }
  أرجع plug;
}

U_CAPI UPlug_معلومات* U_EXPORT2
uplug_بارول_Plug_From_Library(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UError_کوډ *حالت) { 
  UPlug_معلومات *plug = تش_اشاره;
  إذا(U_FAILURE(*حالت)) { أرجع تش_اشاره; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, حالت);
  uplug_بارول_Plug(plug, حالت);

  أرجع plug;
}

#د_که_پای

ساكن UPlugLevel gCurrentLevel = UPLUG_LEVEL_ټیټ;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  أرجع gCurrentLevel;
}

ساكن UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlug_معلومات *plugin_منزل_Remove;
  /* cleanup plugs */
  لكل(i=0;i<plugin_شمېر;i++) {
    UError_کوډ sub_حالت = U_ZERO_ERROR;
    plugin_منزل_Remove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(plugin_منزل_Remove, &sub_حالت);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_ټیټ;
  أرجع ریښتیا;
}

#إذا U_ENABLE_DYLOAD

ساكن فراغ uplug_بارول_انتظار_Plugs(UError_کوډ *حالت) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  إذا(U_FAILURE(*حالت)) {
    أرجع;
  }
#إذا UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#د_که_پای
  /* pass #1: low level plugs */
  لكل(i=0;i<plugin_شمېر;i++) {
    UError_کوډ sub_حالت = U_ZERO_ERROR;
    UPlug_معلومات *plugin_منزل_بارول = &pluginList[i];
    إذا(plugin_منزل_بارول->awaiting_بارول) {
      إذا(plugin_منزل_بارول->level == UPLUG_LEVEL_ټیټ) {
        إذا(currentLevel > UPLUG_LEVEL_ټیټ) {
          plugin_منزل_بارول->plugin_حالت = U_PLUGIN_TOO_لوړ;
        } وإلا {
          UPlugLevel newLevel;
          uplug_بارول_Plug(plugin_منزل_بارول, &sub_حالت);
          newLevel = uplug_getCurrentLevel();
          إذا(newLevel > currentLevel) {
            plugin_منزل_بارول->plugin_حالت = U_PLUGIN_بدل_شوی_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        plugin_منزل_بارول->awaiting_بارول = خطأ;
      } 
    }
  }    
  لكل(i=0;i<plugin_شمېر;i++) {
    UError_کوډ sub_حالت = U_ZERO_ERROR;
    UPlug_معلومات *plugin_منزل_بارول = &pluginList[i];
        
    إذا(plugin_منزل_بارول->awaiting_بارول) {
      إذا(plugin_منزل_بارول->level == UPLUG_LEVEL_INVALID) { 
        plugin_منزل_بارول->plugin_حالت = U_PLUGIN_DIDNT_سېټ_LEVEL;
      } وإلا إذا(plugin_منزل_بارول->level == UPLUG_LEVEL_UNKNOWN) {
        plugin_منزل_بارول->plugin_حالت = U_دننی_پروګرام_ERROR;
      } وإلا {
        uplug_بارول_Plug(plugin_منزل_بارول, &sub_حالت);
      }
      plugin_منزل_بارول->awaiting_بارول = خطأ;
    }
  }
    
#إذا UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#د_که_پای
}

/* Name of the plugin config file */
ساكن حرف plugin_file[2048] = "";
#د_که_پای

U_CAPI ثابت حرف* U_EXPORT2
uplug_getPluginFile() {
#إذا U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  أرجع plugin_file;
#وإلا
  أرجع تش_اشاره;
#د_که_پای
}


//  uplug_init()  is called first thing from u_init().

U_CAPI فراغ U_EXPORT2
uplug_init(UError_کوډ *حالت) {
#إذا !U_ENABLE_DYLOAD
  (فراغ)حالت; /*ناکارول_شوی*/
#وإلاإذا !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ثابت حرف *env = getenv("ICU_PLUGINS");

  إذا(U_FAILURE(*حالت)) أرجع;
  إذا(env != تش_اشاره) {
    plugin_dir.append(env, -1, *حالت);
  }
  إذا(U_FAILURE(*حالت)) أرجع;

#إذا defined(DEFAULT_ICU_PLUGINS) 
  إذا(plugin_dir.is_تشه()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *حالت);
  }
#د_که_پای

#إذا UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.معلومات()));
#د_که_پای

  إذا(!plugin_dir.is_تشه()) {
    FILE *f;
        
    CharString pluginFile;
#که_تعریف_شوی ICU_PLUGINS_DD
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
    pluginFile.append(U_ICU_نسخه_SHORT, -1, *حالت);
    pluginFile.append(".txt", -1, *حالت);
#د_که_پای

#إذا UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*حالت)));
#د_که_پای

    إذا(U_FAILURE(*حالت)) {
      أرجع;
    }
    إذا((size_t)pluginFile.length() > (حجم(plugin_file)-1)) {
      *حالت = U_بفر_ډکېدنه_ERROR;
#إذا UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*حالت)));
#د_که_پای
      أرجع;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, حجم(plugin_file), *حالت);

#إذا UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (صحيح)مزي_اوږدوالی(plugin_file), (صحيح)حجم(plugin_file)));
#د_که_پای
        
#که_تعریف_شوی __MVS__
    إذا (iscics()) /* 12 Nov 2011 JAM */
    {
        f = تش_اشاره;
    }
    وإلا
#د_که_پای
    {
        f = fopen(pluginFile.معلومات(), "r");
    }

    إذا(f != تش_اشاره) {
      حرف linebuf[1024];
      حرف *p, *libName=تش_اشاره, *symName=تش_اشاره, *config=تش_اشاره;
      int32_t کرښه = 0;
            
            
      طالما(fgets(linebuf,1023,f)) {
        کرښه++;

        إذا(!*linebuf || *linebuf=='#') {
          استمر;
        } وإلا {
          p = linebuf;
          طالما(*p&&که_تشه((صحيح)*p))
            p++;
          إذا(!*p || *p=='#') استمر;
          libName = p;
          طالما(*p&&!که_تشه((صحيح)*p)) {
            p++;
          }
          إذا(!*p || *p=='#') استمر; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          طالما(*p&&که_تشه((صحيح)*p)) {
            p++;
          }
          إذا(!*p||*p=='#') استمر; /* no symname after libname +tab */
          symName = p;
          طالما(*p&&!که_تشه((صحيح)*p)) {
            p++;
          }
                    
          إذا(*p) { /* has config */
            *p=0;
            ++p;
            طالما(*p&&که_تشه((صحيح)*p)) {
              p++;
            }
            إذا(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          إذا(config!=تش_اشاره&&*config!=0) {
            p = config+مزي_اوږدوالی(config);
            طالما(p>config&&که_تشه((صحيح)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UError_کوډ sub_حالت = U_ZERO_ERROR;
            UPlug_معلومات *plug = uplug_initPlugFromLibrary(libName, symName, config, &sub_حالت);
            إذا(U_FAILURE(sub_حالت) && U_SUCCESS(*حالت)) {
              *حالت = sub_حالت;
            }
#إذا UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (فراغ*)plug, u_errorName(sub_حالت)));
#وإلا
            (فراغ)plug; /*ناکارول_شوی*/
#د_که_پای
          }
        }
      }
      fclose(f);
    } وإلا {
#إذا UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#د_که_پای
    }
  }
  uplug_بارول_انتظار_Plugs(حالت);
#د_که_پای /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_لوړ;
  ucln_ثبتوونکی_Cleanup(UCLN_UPLUG, uplug_cleanup);
}

#د_که_پای


