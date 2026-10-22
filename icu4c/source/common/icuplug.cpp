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
#جے_تعریف_شدہ __MVS__  /* defined by z/OS compiler */
#تعریف_کرو _POSIX_SOURCE
#ضمن <cics.h> /* 12 Nov 2011 JAM iscics() function */
#جے_انت
#ضمن "charstr.h"

ورتوں نطاق icu;

#جے_بنا_تعریف UPLUG_TRACE
#تعریف_کرو UPLUG_TRACE 0
#جے_انت

#إذا UPLUG_TRACE
#ضمن <stdio.h>
#تعریف_کرو DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#جے_انت

/**
 * Internal structure of an ICU plugin. 
 */

بنية UPlug_انکڑے {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t struct_ناپ;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  فراغ *lib;              /**< plugin library, or nullptr */
  حرف libName[UPLUG_NAME_MAX];   /**< library name */
  حرف sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  حرف config[UPLUG_NAME_MAX];     /**< configuration data */
  فراغ *context;          /**< user context data */
  حرف name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaiting_لدو; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UError_کوڈ plugin_حالت; /**< status code of plugin */
};



#تعریف_کرو UPLUG_LIBRARY_INITIAL_گنتی 8
#تعریف_کرو UPLUG_PLUGIN_INITIAL_گنتی 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
ساكن int32_t uplug_remove_اندراج_At(فراغ *list, int32_t list_ناپ, int32_t member_ناپ, int32_t شے_منزل_Remove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  إذا(list_ناپ<1) {
    أرجع list_ناپ;
  }
    
  /* is there anything to move? */
  إذا(list_ناپ > شے_منزل_Remove+1) {
    memmove(bytePtr+(شے_منزل_Remove*member_ناپ), bytePtr+((شے_منزل_Remove+1)*member_ناپ), member_ناپ);
  }
    
  أرجع list_ناپ-1;
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

ساكن UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_گنتی];
ساكن UPlugLibrary * libraryList = staticLibraryList;
ساكن int32_t library_گنتی = 0;
ساكن int32_t libraryMax = UPLUG_LIBRARY_INITIAL_گنتی;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ساكن int32_t تلاش_For_Library_Name(ثابت حرف *libName) {
  int32_t i;
    
  لكل(i=0;i<library_گنتی;i++) {
    إذا(!uprv_strcmp(libName, libraryList[i].name)) {
      أرجع i;
    }
  }
  أرجع -1;
}

ساكن int32_t تلاش_For_Library(فراغ *lib) {
  int32_t i;
    
  لكل(i=0;i<library_گنتی;i++) {
    إذا(lib==libraryList[i].lib) {
      أرجع i;
    }
  }
  أرجع -1;
}

U_CAPI حرف * U_EXPORT2
uplug_لبھو_Library(فراغ *lib, UError_کوڈ *حالت) {
  int32_t libEnt;
  حرف *ret = خالی_سوچک;
  إذا(U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }
  libEnt = تلاش_For_Library(lib);
  إذا(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } وإلا {
    *حالت = U_MISSING_RESOURCE_ERROR;
  }
  أرجع ret;
}

U_CAPI فراغ * U_EXPORT2
uplug_openLibrary(ثابت حرف *libName, UError_کوڈ *حالت) {
  int32_t lib_اندراج = -1;
  فراغ *lib = خالی_سوچک;
    
  إذا(U_FAILURE(*حالت)) أرجع خالی_سوچک;

  lib_اندراج = تلاش_For_Library_Name(libName);
  إذا(lib_اندراج == -1) {
    lib_اندراج = library_گنتی++;
    إذا(library_گنتی >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *حالت = U_حافظہ_ونڈ_ERROR;
#إذا UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#جے_انت
      أرجع خالی_سوچک;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[lib_اندراج].lib = uprv_dl_open(libName, حالت);
#إذا UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*حالت), lib_اندراج, lib));
#جے_انت
        
    إذا(libraryList[lib_اندراج].lib == خالی_سوچک || U_FAILURE(*حالت)) {
      /* cleanup. */
      libraryList[lib_اندراج].lib = خالی_سوچک; /* failure with open */
      libraryList[lib_اندراج].name[0] = 0;
#إذا UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*حالت), lib_اندراج, lib));
#جے_انت
      /* no need to free - just won't increase the count. */
      library_گنتی--;
    } وإلا { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[lib_اندراج].name,libName,UPLUG_NAME_MAX);
      libraryList[lib_اندراج].ref=1;
      lib = libraryList[lib_اندراج].lib;
    }

  } وإلا {
    lib = libraryList[lib_اندراج].lib;
    libraryList[lib_اندراج].ref++;
  }
  أرجع lib;
}

U_CAPI فراغ U_EXPORT2
uplug_closeLibrary(فراغ *lib, UError_کوڈ *حالت) {
  int32_t i;
    
#إذا UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*حالت), (فراغ*)libraryList));
#جے_انت
  إذا(U_FAILURE(*حالت)) أرجع;
    
  لكل(i=0;i<library_گنتی;i++) {
    إذا(lib==libraryList[i].lib) {
      إذا(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, حالت);
        library_گنتی = uplug_remove_اندراج_At(libraryList, library_گنتی, حجم(*libraryList), i);
      }
      أرجع;
    }
  }
  *حالت = U_اندرونی_پروگرام_ERROR; /* could not find the entry! */
}

#جے_انت

ساكن UPlug_انکڑے pluginList[UPLUG_PLUGIN_INITIAL_گنتی];
ساكن int32_t plugin_گنتی = 0;



  
ساكن int32_t uplug_plugin_نمبر(UPlug_انکڑے* d) {
  UPlug_انکڑے *pastPlug = &pluginList[plugin_گنتی];
  إذا(d<=pluginList) {
    أرجع 0;
  } وإلا إذا(d>=pastPlug) {
    أرجع plugin_گنتی;
  } وإلا {
    أرجع (d-pluginList)/حجم(pluginList[0]);
  }
}


U_CAPI UPlug_انکڑے * U_EXPORT2
uplug_اگلا_Plug(UPlug_انکڑے *پہلوں) {
  إذا(پہلوں==خالی_سوچک) {
    أرجع pluginList;
  } وإلا {
    UPlug_انکڑے *اگلا_Plug = &پہلوں[1];
    UPlug_انکڑے *pastPlug = &pluginList[plugin_گنتی];
    
    إذا(اگلا_Plug>=pastPlug) {
      أرجع خالی_سوچک;
    } وإلا {
      أرجع اگلا_Plug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ساكن فراغ uplug_callPlug(UPlug_انکڑے *plug, UPlugReason reason, UError_کوڈ *حالت) {
  UPlugTokenReturn token;
  إذا(plug==خالی_سوچک||U_FAILURE(*حالت)) {
    أرجع;
  }
  token = (*(plug->entrypoint))(plug, reason, حالت);
  إذا(token!=UPLUG_TOKEN) {
    *حالت = U_اندرونی_پروگرام_ERROR;
  }
}


ساكن فراغ uplug_unloadPlug(UPlug_انکڑے *plug, UError_کوڈ *حالت) {
  إذا(plug->awaiting_لدو) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت = U_اندرونی_پروگرام_ERROR;
    أرجع; 
  }
  إذا(U_SUCCESS(plug->plugin_حالت)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, حالت);
  }
}

ساكن فراغ uplug_queryPlug(UPlug_انکڑے *plug, UError_کوڈ *حالت) {
  إذا(!plug->awaiting_لدو || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت = U_اندرونی_پروگرام_ERROR;
    أرجع; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, حالت);
  إذا(U_SUCCESS(*حالت)) { 
    إذا(plug->level == UPLUG_LEVEL_INVALID) {
      plug->plugin_حالت = U_PLUGIN_DIDNT_سیٹ_LEVEL;
      plug->awaiting_لدو = خطأ;
    }
  } وإلا {
    plug->plugin_حالت = U_اندرونی_پروگرام_ERROR;
    plug->awaiting_لدو = خطأ;
  }
}


ساكن فراغ uplug_لدو_Plug(UPlug_انکڑے *plug, UError_کوڈ *حالت) {
  إذا(U_FAILURE(*حالت)) {
    أرجع;
  }
  إذا(!plug->awaiting_لدو || (plug->level < UPLUG_LEVEL_کم) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت = U_اندرونی_پروگرام_ERROR;
    أرجع;
  }
  uplug_callPlug(plug, UPLUG_REASON_لدو, حالت);
  plug->awaiting_لدو = خطأ;
  إذا(!U_SUCCESS(*حالت)) {
    plug->plugin_حالت = U_اندرونی_پروگرام_ERROR;
  }
}

ساكن UPlug_انکڑے *uplug_allocate_خالی_تھاواں_Plug(UError_کوڈ *حالت)
{
  UPlug_انکڑے *plug = خالی_سوچک;

  إذا(U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }

  إذا(plugin_گنتی == UPLUG_PLUGIN_INITIAL_گنتی) {
    *حالت = U_حافظہ_ونڈ_ERROR;
    أرجع خالی_سوچک;
  }

  plug = &pluginList[plugin_گنتی++];

  plug->token = UPLUG_TOKEN;
  plug->struct_ناپ = حجم(UPlug_انکڑے);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaiting_لدو = سچ;
  plug->dontUnload = خطأ;
  plug->plugin_حالت = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=خالی_سوچک;
  plug->entrypoint=خالی_سوچک;


  أرجع plug;
}

ساكن UPlug_انکڑے *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *symName,
                                     UError_کوڈ *حالت) {
  UPlug_انکڑے *plug = uplug_allocate_خالی_تھاواں_Plug(حالت);
  إذا(U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }

  إذا(config!=خالی_سوچک) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } وإلا {
    plug->config[0] = 0;
  }
    
  إذا(symName!=خالی_سوچک) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } وإلا {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, حالت);
    
  أرجع plug;
}

ساكن فراغ uplug_deallocatePlug(UPlug_انکڑے *plug, UError_کوڈ *حالت) {
  UError_کوڈ sub_حالت = U_ZERO_ERROR;
  إذا(!plug->dontUnload) {
#إذا U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &sub_حالت);
#جے_انت
  }
  plug->lib = خالی_سوچک;
  إذا(U_SUCCESS(*حالت) && U_FAILURE(sub_حالت)) {
    *حالت = sub_حالت;
  }
  /* shift plugins up and decrement count. */
  إذا(U_SUCCESS(*حالت)) {
    /* all ok- remove. */
    plugin_گنتی = uplug_remove_اندراج_At(pluginList, plugin_گنتی, حجم(plug[0]), uplug_plugin_نمبر(plug));
  } وإلا {
    /* not ok- leave as a message. */
    plug->awaiting_لدو=خطأ;
    plug->entrypoint=0;
    plug->dontUnload=سچ;
  }
}

ساكن فراغ uplug_doUnloadPlug(UPlug_انکڑے *plug_منزل_Remove, UError_کوڈ *حالت) {
  إذا(plug_منزل_Remove != خالی_سوچک) {
    uplug_unloadPlug(plug_منزل_Remove, حالت);
    uplug_deallocatePlug(plug_منزل_Remove, حالت);
  }
}

U_CAPI فراغ U_EXPORT2
uplug_removePlug(UPlug_انکڑے *plug, UError_کوڈ *حالت)  {
  UPlug_انکڑے *cursor = خالی_سوچک;
  UPlug_انکڑے *plug_منزل_Remove = خالی_سوچک;
  إذا(U_FAILURE(*حالت)) أرجع;
    
  لكل(cursor=pluginList;cursor!=خالی_سوچک;) {
    إذا(cursor==plug) {
      plug_منزل_Remove = plug;
      cursor=خالی_سوچک;
    } وإلا {
      cursor = uplug_اگلا_Plug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plug_منزل_Remove, حالت);
}




U_CAPI فراغ U_EXPORT2 
uplug_سیٹ_Plug_No_Unload(UPlug_انکڑے *انکڑے, UBool dontUnload)
{
  انکڑے->dontUnload = dontUnload;
}


U_CAPI فراغ U_EXPORT2
uplug_سیٹ_Plug_Level(UPlug_انکڑے *انکڑے, UPlugLevel level) {
  انکڑے->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlug_انکڑے *انکڑے) {
  أرجع انکڑے->level;
}


U_CAPI فراغ U_EXPORT2
uplug_سیٹ_Plug_Name(UPlug_انکڑے *انکڑے, ثابت حرف *name) {
  uprv_strncpy(انکڑے->name, name, UPLUG_NAME_MAX);
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_getPlugName(UPlug_انکڑے* انکڑے U_LIFETIME_BOUND) {
  أرجع انکڑے->name;
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_get_علامت_Name(UPlug_انکڑے* انکڑے U_LIFETIME_BOUND) {
  أرجع انکڑے->sym;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getLibraryName(UPlug_انکڑے* انکڑے U_LIFETIME_BOUND, UError_کوڈ* حالت) {
  إذا(انکڑے->libName[0]) {
    أرجع انکڑے->libName;
  } وإلا {
#إذا U_ENABLE_DYLOAD
    أرجع uplug_لبھو_Library(انکڑے->lib, حالت);
#وإلا
    أرجع خالی_سوچک;
#جے_انت
  }
}

U_CAPI فراغ * U_EXPORT2
uplug_getLibrary(UPlug_انکڑے *انکڑے) {
  أرجع انکڑے->lib;
}

U_CAPI فراغ * U_EXPORT2
uplug_getContext(UPlug_انکڑے *انکڑے) {
  أرجع انکڑے->context;
}


U_CAPI فراغ U_EXPORT2
uplug_سیٹ_Context(UPlug_انکڑے *انکڑے, فراغ *context) {
  انکڑے->context = context;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getConfiguration(UPlug_انکڑے* انکڑے U_LIFETIME_BOUND) {
  أرجع انکڑے->config;
}

U_CAPI UPlug_انکڑے* U_EXPORT2
uplug_get_Plug_اندرونی(int32_t n) { 
  إذا(n <0 || n >= plugin_گنتی) {
    أرجع خالی_سوچک;
  } وإلا { 
    أرجع &(pluginList[n]);
  }
}


U_CAPI UError_کوڈ U_EXPORT2
uplug_get_Plug_لدو_حالت(UPlug_انکڑے *plug) {
  أرجع plug->plugin_حالت;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ساكن UPlug_انکڑے* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ثابت حرف *config, فراغ *lib, ثابت حرف *sym,
                                                         UError_کوڈ *حالت) {
  UPlug_انکڑے *plug = خالی_سوچک;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, حالت);

  إذا(U_SUCCESS(*حالت)) {
    أرجع plug;
  } وإلا {
    uplug_deallocatePlug(plug, حالت);
    أرجع خالی_سوچک;
  }
}

U_CAPI UPlug_انکڑے* U_EXPORT2
uplug_لدو_Plug_From_Entrypoint(UPlugEntrypoint *entrypoint, ثابت حرف *config, UError_کوڈ *حالت) {
  UPlug_انکڑے* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, خالی_سوچک, خالی_سوچک, حالت);
  uplug_لدو_Plug(plug, حالت);
  أرجع plug;
}

#إذا U_ENABLE_DYLOAD

ساكن UPlug_انکڑے* 
uplug_initErrorPlug(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, ثابت حرف *nameOrError, UError_کوڈ لدو_حالت, UError_کوڈ *حالت)
{
  UPlug_انکڑے *plug = uplug_allocate_خالی_تھاواں_Plug(حالت);
  إذا(U_FAILURE(*حالت)) أرجع خالی_سوچک;

  plug->plugin_حالت = لدو_حالت;
  plug->awaiting_لدو = خطأ; /* Won't load. */
  plug->dontUnload = سچ; /* cannot unload. */

  إذا(sym!=خالی_سوچک) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  إذا(libName!=خالی_سوچک) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  إذا(nameOrError!=خالی_سوچک) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  إذا(config!=خالی_سوچک) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  أرجع plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ساكن UPlug_انکڑے* 
uplug_initPlugFromLibrary(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UError_کوڈ *حالت) {
  فراغ *lib = خالی_سوچک;
  UPlug_انکڑے *plug = خالی_سوچک;
  إذا(U_FAILURE(*حالت)) { أرجع خالی_سوچک; }
  lib = uplug_openLibrary(libName, حالت);
  إذا(lib!=خالی_سوچک && U_SUCCESS(*حالت)) {
    UPlugEntrypoint *entrypoint = خالی_سوچک;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, حالت);

    إذا(entrypoint!=خالی_سوچک&&U_SUCCESS(*حالت)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, حالت);
      إذا(plug!=خالی_سوچک&&U_SUCCESS(*حالت)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = خالی_سوچک; /* library is now owned by plugin. */
      }
    } وإلا {
      UError_کوڈ sub_حالت = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==خالی_سوچک)?U_MISSING_RESOURCE_ERROR:*حالت,&sub_حالت);
    }
    إذا(lib!=خالی_سوچک) { /* still need to close the lib */
      UError_کوڈ sub_حالت = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &sub_حالت); /* don't care here */
    }
  } وإلا {
    UError_کوڈ sub_حالت = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==خالی_سوچک)?U_MISSING_RESOURCE_ERROR:*حالت,&sub_حالت);
  }
  أرجع plug;
}

U_CAPI UPlug_انکڑے* U_EXPORT2
uplug_لدو_Plug_From_Library(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UError_کوڈ *حالت) { 
  UPlug_انکڑے *plug = خالی_سوچک;
  إذا(U_FAILURE(*حالت)) { أرجع خالی_سوچک; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, حالت);
  uplug_لدو_Plug(plug, حالت);

  أرجع plug;
}

#جے_انت

ساكن UPlugLevel gCurrentLevel = UPLUG_LEVEL_کم;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  أرجع gCurrentLevel;
}

ساكن UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlug_انکڑے *plugin_منزل_Remove;
  /* cleanup plugs */
  لكل(i=0;i<plugin_گنتی;i++) {
    UError_کوڈ sub_حالت = U_ZERO_ERROR;
    plugin_منزل_Remove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(plugin_منزل_Remove, &sub_حالت);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_کم;
  أرجع سچ;
}

#إذا U_ENABLE_DYLOAD

ساكن فراغ uplug_لدو_اڈیک_Plugs(UError_کوڈ *حالت) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  إذا(U_FAILURE(*حالت)) {
    أرجع;
  }
#إذا UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#جے_انت
  /* pass #1: low level plugs */
  لكل(i=0;i<plugin_گنتی;i++) {
    UError_کوڈ sub_حالت = U_ZERO_ERROR;
    UPlug_انکڑے *plugin_منزل_لدو = &pluginList[i];
    إذا(plugin_منزل_لدو->awaiting_لدو) {
      إذا(plugin_منزل_لدو->level == UPLUG_LEVEL_کم) {
        إذا(currentLevel > UPLUG_LEVEL_کم) {
          plugin_منزل_لدو->plugin_حالت = U_PLUGIN_TOO_بلند;
        } وإلا {
          UPlugLevel newLevel;
          uplug_لدو_Plug(plugin_منزل_لدو, &sub_حالت);
          newLevel = uplug_getCurrentLevel();
          إذا(newLevel > currentLevel) {
            plugin_منزل_لدو->plugin_حالت = U_PLUGIN_بدلیا_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        plugin_منزل_لدو->awaiting_لدو = خطأ;
      } 
    }
  }    
  لكل(i=0;i<plugin_گنتی;i++) {
    UError_کوڈ sub_حالت = U_ZERO_ERROR;
    UPlug_انکڑے *plugin_منزل_لدو = &pluginList[i];
        
    إذا(plugin_منزل_لدو->awaiting_لدو) {
      إذا(plugin_منزل_لدو->level == UPLUG_LEVEL_INVALID) { 
        plugin_منزل_لدو->plugin_حالت = U_PLUGIN_DIDNT_سیٹ_LEVEL;
      } وإلا إذا(plugin_منزل_لدو->level == UPLUG_LEVEL_UNKNOWN) {
        plugin_منزل_لدو->plugin_حالت = U_اندرونی_پروگرام_ERROR;
      } وإلا {
        uplug_لدو_Plug(plugin_منزل_لدو, &sub_حالت);
      }
      plugin_منزل_لدو->awaiting_لدو = خطأ;
    }
  }
    
#إذا UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#جے_انت
}

/* Name of the plugin config file */
ساكن حرف plugin_file[2048] = "";
#جے_انت

U_CAPI ثابت حرف* U_EXPORT2
uplug_getPluginFile() {
#إذا U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  أرجع plugin_file;
#وإلا
  أرجع خالی_سوچک;
#جے_انت
}


//  uplug_init()  is called first thing from u_init().

U_CAPI فراغ U_EXPORT2
uplug_init(UError_کوڈ *حالت) {
#إذا !U_ENABLE_DYLOAD
  (فراغ)حالت; /*بن_ورتیا*/
#وإلاإذا !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ثابت حرف *env = getenv("ICU_PLUGINS");

  إذا(U_FAILURE(*حالت)) أرجع;
  إذا(env != خالی_سوچک) {
    plugin_dir.append(env, -1, *حالت);
  }
  إذا(U_FAILURE(*حالت)) أرجع;

#إذا defined(DEFAULT_ICU_PLUGINS) 
  إذا(plugin_dir.is_خالی_تھاواں()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *حالت);
  }
#جے_انت

#إذا UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.انکڑے()));
#جے_انت

  إذا(!plugin_dir.is_خالی_تھاواں()) {
    FILE *f;
        
    CharString pluginFile;
#جے_تعریف_شدہ ICU_PLUGINS_DD
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
    pluginFile.append(U_ICU_نسخہ_SHORT, -1, *حالت);
    pluginFile.append(".txt", -1, *حالت);
#جے_انت

#إذا UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*حالت)));
#جے_انت

    إذا(U_FAILURE(*حالت)) {
      أرجع;
    }
    إذا((size_t)pluginFile.length() > (حجم(plugin_file)-1)) {
      *حالت = U_بفر_حد_توں_ودھ_ERROR;
#إذا UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*حالت)));
#جے_انت
      أرجع;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, حجم(plugin_file), *حالت);

#إذا UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (صحيح)لڑی_لمبائی(plugin_file), (صحيح)حجم(plugin_file)));
#جے_انت
        
#جے_تعریف_شدہ __MVS__
    إذا (iscics()) /* 12 Nov 2011 JAM */
    {
        f = خالی_سوچک;
    }
    وإلا
#جے_انت
    {
        f = fopen(pluginFile.انکڑے(), "r");
    }

    إذا(f != خالی_سوچک) {
      حرف linebuf[1024];
      حرف *p, *libName=خالی_سوچک, *symName=خالی_سوچک, *config=خالی_سوچک;
      int32_t سطر = 0;
            
            
      طالما(fgets(linebuf,1023,f)) {
        سطر++;

        إذا(!*linebuf || *linebuf=='#') {
          استمر;
        } وإلا {
          p = linebuf;
          طالما(*p&&خلا_اے((صحيح)*p))
            p++;
          إذا(!*p || *p=='#') استمر;
          libName = p;
          طالما(*p&&!خلا_اے((صحيح)*p)) {
            p++;
          }
          إذا(!*p || *p=='#') استمر; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          طالما(*p&&خلا_اے((صحيح)*p)) {
            p++;
          }
          إذا(!*p||*p=='#') استمر; /* no symname after libname +tab */
          symName = p;
          طالما(*p&&!خلا_اے((صحيح)*p)) {
            p++;
          }
                    
          إذا(*p) { /* has config */
            *p=0;
            ++p;
            طالما(*p&&خلا_اے((صحيح)*p)) {
              p++;
            }
            إذا(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          إذا(config!=خالی_سوچک&&*config!=0) {
            p = config+لڑی_لمبائی(config);
            طالما(p>config&&خلا_اے((صحيح)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UError_کوڈ sub_حالت = U_ZERO_ERROR;
            UPlug_انکڑے *plug = uplug_initPlugFromLibrary(libName, symName, config, &sub_حالت);
            إذا(U_FAILURE(sub_حالت) && U_SUCCESS(*حالت)) {
              *حالت = sub_حالت;
            }
#إذا UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (فراغ*)plug, u_errorName(sub_حالت)));
#وإلا
            (فراغ)plug; /*بن_ورتیا*/
#جے_انت
          }
        }
      }
      fclose(f);
    } وإلا {
#إذا UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#جے_انت
    }
  }
  uplug_لدو_اڈیک_Plugs(حالت);
#جے_انت /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_بلند;
  ucln_رجسٹر_Cleanup(UCLN_UPLUG, uplug_cleanup);
}

#جے_انت


