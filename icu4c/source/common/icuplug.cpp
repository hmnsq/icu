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

#شامل "unicode/icuplug.h"


#اگر UCONFIG_ENABLE_PLUGINS


#شامل "icuplugimp.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "putilimp.h"
#شامل "ucln.h"
#شامل <stdio.h>
#اگر_تعریف_شدہ __MVS__  /* defined by z/OS compiler */
#تعریف_کرو _POSIX_SOURCE
#شامل <cics.h> /* 12 Nov 2011 JAM iscics() function */
#اگر_ختم
#شامل "charstr.h"

استعمال نامخلا icu;

#اگر_غیر_تعریف_شدہ UPLUG_TRACE
#تعریف_کرو UPLUG_TRACE 0
#اگر_ختم

#اگر UPLUG_TRACE
#شامل <stdio.h>
#تعریف_کرو DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#اگر_ختم

/**
 * Internal structure of an ICU plugin. 
 */

ساخت UPlug_ڈیٹا {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t struct_سائز;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  خالی *lib;              /**< plugin library, or nullptr */
  حرف libName[UPLUG_NAME_MAX];   /**< library name */
  حرف sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  حرف config[UPLUG_NAME_MAX];     /**< configuration data */
  خالی *context;          /**< user context data */
  حرف name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaiting_بھرو; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UError_رمز plugin_حالت; /**< status code of plugin */
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
ساکن int32_t uplug_remove_اندراج_At(خالی *list, int32_t list_سائز, int32_t member_سائز, int32_t شے_مقصد_Remove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  اگر(list_سائز<1) {
    واپس list_سائز;
  }
    
  /* is there anything to move? */
  اگر(list_سائز > شے_مقصد_Remove+1) {
    memmove(bytePtr+(شے_مقصد_Remove*member_سائز), bytePtr+((شے_مقصد_Remove+1)*member_سائز), member_سائز);
  }
    
  واپس list_سائز-1;
}




#اگر U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
ساخت UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
قسمتعریف ساخت UPlugLibrary {
  خالی *lib;                           /**< library ptr */
  حرف name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

ساکن UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_گنتی];
ساکن UPlugLibrary * libraryList = staticLibraryList;
ساکن int32_t library_گنتی = 0;
ساکن int32_t libraryMax = UPLUG_LIBRARY_INITIAL_گنتی;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ساکن int32_t تلاش_For_Library_Name(ثابت حرف *libName) {
  int32_t i;
    
  برائے(i=0;i<library_گنتی;i++) {
    اگر(!uprv_strcmp(libName, libraryList[i].name)) {
      واپس i;
    }
  }
  واپس -1;
}

ساکن int32_t تلاش_For_Library(خالی *lib) {
  int32_t i;
    
  برائے(i=0;i<library_گنتی;i++) {
    اگر(lib==libraryList[i].lib) {
      واپس i;
    }
  }
  واپس -1;
}

U_CAPI حرف * U_EXPORT2
uplug_ڈھونڈو_Library(خالی *lib, UError_رمز *حالت_2) {
  int32_t libEnt;
  حرف *ret = خالی_اشاریہ;
  اگر(U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }
  libEnt = تلاش_For_Library(lib);
  اگر(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } ورنہ {
    *حالت_2 = U_MISSING_RESOURCE_ERROR;
  }
  واپس ret;
}

U_CAPI خالی * U_EXPORT2
uplug_openLibrary(ثابت حرف *libName, UError_رمز *حالت_2) {
  int32_t lib_اندراج = -1;
  خالی *lib = خالی_اشاریہ;
    
  اگر(U_FAILURE(*حالت_2)) واپس خالی_اشاریہ;

  lib_اندراج = تلاش_For_Library_Name(libName);
  اگر(lib_اندراج == -1) {
    lib_اندراج = library_گنتی++;
    اگر(library_گنتی >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *حالت_2 = U_حافظہ_تخصیص_ERROR;
#اگر UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#اگر_ختم
      واپس خالی_اشاریہ;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[lib_اندراج].lib = uprv_dl_open(libName, حالت_2);
#اگر UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*حالت_2), lib_اندراج, lib));
#اگر_ختم
        
    اگر(libraryList[lib_اندراج].lib == خالی_اشاریہ || U_FAILURE(*حالت_2)) {
      /* cleanup. */
      libraryList[lib_اندراج].lib = خالی_اشاریہ; /* failure with open */
      libraryList[lib_اندراج].name[0] = 0;
#اگر UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*حالت_2), lib_اندراج, lib));
#اگر_ختم
      /* no need to free - just won't increase the count. */
      library_گنتی--;
    } ورنہ { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[lib_اندراج].name,libName,UPLUG_NAME_MAX);
      libraryList[lib_اندراج].ref=1;
      lib = libraryList[lib_اندراج].lib;
    }

  } ورنہ {
    lib = libraryList[lib_اندراج].lib;
    libraryList[lib_اندراج].ref++;
  }
  واپس lib;
}

U_CAPI خالی U_EXPORT2
uplug_closeLibrary(خالی *lib, UError_رمز *حالت_2) {
  int32_t i;
    
#اگر UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*حالت_2), (خالی*)libraryList));
#اگر_ختم
  اگر(U_FAILURE(*حالت_2)) واپس;
    
  برائے(i=0;i<library_گنتی;i++) {
    اگر(lib==libraryList[i].lib) {
      اگر(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, حالت_2);
        library_گنتی = uplug_remove_اندراج_At(libraryList, library_گنتی, حجم(*libraryList), i);
      }
      واپس;
    }
  }
  *حالت_2 = U_اندرونی_پروگرام_ERROR; /* could not find the entry! */
}

#اگر_ختم

ساکن UPlug_ڈیٹا pluginList[UPLUG_PLUGIN_INITIAL_گنتی];
ساکن int32_t plugin_گنتی = 0;



  
ساکن int32_t uplug_plugin_نمبر(UPlug_ڈیٹا* d) {
  UPlug_ڈیٹا *pastPlug = &pluginList[plugin_گنتی];
  اگر(d<=pluginList) {
    واپس 0;
  } ورنہ اگر(d>=pastPlug) {
    واپس plugin_گنتی;
  } ورنہ {
    واپس (d-pluginList)/حجم(pluginList[0]);
  }
}


U_CAPI UPlug_ڈیٹا * U_EXPORT2
uplug_اگلا_Plug(UPlug_ڈیٹا *پیشگی) {
  اگر(پیشگی==خالی_اشاریہ) {
    واپس pluginList;
  } ورنہ {
    UPlug_ڈیٹا *اگلا_Plug = &پیشگی[1];
    UPlug_ڈیٹا *pastPlug = &pluginList[plugin_گنتی];
    
    اگر(اگلا_Plug>=pastPlug) {
      واپس خالی_اشاریہ;
    } ورنہ {
      واپس اگلا_Plug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ساکن خالی uplug_callPlug(UPlug_ڈیٹا *plug, UPlugReason reason, UError_رمز *حالت_2) {
  UPlugTokenReturn token;
  اگر(plug==خالی_اشاریہ||U_FAILURE(*حالت_2)) {
    واپس;
  }
  token = (*(plug->entrypoint))(plug, reason, حالت_2);
  اگر(token!=UPLUG_TOKEN) {
    *حالت_2 = U_اندرونی_پروگرام_ERROR;
  }
}


ساکن خالی uplug_unloadPlug(UPlug_ڈیٹا *plug, UError_رمز *حالت_2) {
  اگر(plug->awaiting_بھرو) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت_2 = U_اندرونی_پروگرام_ERROR;
    واپس; 
  }
  اگر(U_SUCCESS(plug->plugin_حالت)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, حالت_2);
  }
}

ساکن خالی uplug_queryPlug(UPlug_ڈیٹا *plug, UError_رمز *حالت_2) {
  اگر(!plug->awaiting_بھرو || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت_2 = U_اندرونی_پروگرام_ERROR;
    واپس; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, حالت_2);
  اگر(U_SUCCESS(*حالت_2)) { 
    اگر(plug->level == UPLUG_LEVEL_INVALID) {
      plug->plugin_حالت = U_PLUGIN_DIDNT_سیٹ_LEVEL;
      plug->awaiting_بھرو = جھوٹ;
    }
  } ورنہ {
    plug->plugin_حالت = U_اندرونی_پروگرام_ERROR;
    plug->awaiting_بھرو = جھوٹ;
  }
}


ساکن خالی uplug_بھرو_Plug(UPlug_ڈیٹا *plug, UError_رمز *حالت_2) {
  اگر(U_FAILURE(*حالت_2)) {
    واپس;
  }
  اگر(!plug->awaiting_بھرو || (plug->level < UPLUG_LEVEL_کم) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *حالت_2 = U_اندرونی_پروگرام_ERROR;
    واپس;
  }
  uplug_callPlug(plug, UPLUG_REASON_بھرو, حالت_2);
  plug->awaiting_بھرو = جھوٹ;
  اگر(!U_SUCCESS(*حالت_2)) {
    plug->plugin_حالت = U_اندرونی_پروگرام_ERROR;
  }
}

ساکن UPlug_ڈیٹا *uplug_allocate_خالی_خانے_Plug(UError_رمز *حالت_2)
{
  UPlug_ڈیٹا *plug = خالی_اشاریہ;

  اگر(U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }

  اگر(plugin_گنتی == UPLUG_PLUGIN_INITIAL_گنتی) {
    *حالت_2 = U_حافظہ_تخصیص_ERROR;
    واپس خالی_اشاریہ;
  }

  plug = &pluginList[plugin_گنتی++];

  plug->token = UPLUG_TOKEN;
  plug->struct_سائز = حجم(UPlug_ڈیٹا);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaiting_بھرو = سچ;
  plug->dontUnload = جھوٹ;
  plug->plugin_حالت = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=خالی_اشاریہ;
  plug->entrypoint=خالی_اشاریہ;


  واپس plug;
}

ساکن UPlug_ڈیٹا *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ثابت حرف *config, خالی *lib, ثابت حرف *symName,
                                     UError_رمز *حالت_2) {
  UPlug_ڈیٹا *plug = uplug_allocate_خالی_خانے_Plug(حالت_2);
  اگر(U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }

  اگر(config!=خالی_اشاریہ) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } ورنہ {
    plug->config[0] = 0;
  }
    
  اگر(symName!=خالی_اشاریہ) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } ورنہ {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, حالت_2);
    
  واپس plug;
}

ساکن خالی uplug_deallocatePlug(UPlug_ڈیٹا *plug, UError_رمز *حالت_2) {
  UError_رمز sub_حالت = U_ZERO_ERROR;
  اگر(!plug->dontUnload) {
#اگر U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &sub_حالت);
#اگر_ختم
  }
  plug->lib = خالی_اشاریہ;
  اگر(U_SUCCESS(*حالت_2) && U_FAILURE(sub_حالت)) {
    *حالت_2 = sub_حالت;
  }
  /* shift plugins up and decrement count. */
  اگر(U_SUCCESS(*حالت_2)) {
    /* all ok- remove. */
    plugin_گنتی = uplug_remove_اندراج_At(pluginList, plugin_گنتی, حجم(plug[0]), uplug_plugin_نمبر(plug));
  } ورنہ {
    /* not ok- leave as a message. */
    plug->awaiting_بھرو=جھوٹ;
    plug->entrypoint=0;
    plug->dontUnload=سچ;
  }
}

ساکن خالی uplug_doUnloadPlug(UPlug_ڈیٹا *plug_مقصد_Remove, UError_رمز *حالت_2) {
  اگر(plug_مقصد_Remove != خالی_اشاریہ) {
    uplug_unloadPlug(plug_مقصد_Remove, حالت_2);
    uplug_deallocatePlug(plug_مقصد_Remove, حالت_2);
  }
}

U_CAPI خالی U_EXPORT2
uplug_removePlug(UPlug_ڈیٹا *plug, UError_رمز *حالت_2)  {
  UPlug_ڈیٹا *cursor = خالی_اشاریہ;
  UPlug_ڈیٹا *plug_مقصد_Remove = خالی_اشاریہ;
  اگر(U_FAILURE(*حالت_2)) واپس;
    
  برائے(cursor=pluginList;cursor!=خالی_اشاریہ;) {
    اگر(cursor==plug) {
      plug_مقصد_Remove = plug;
      cursor=خالی_اشاریہ;
    } ورنہ {
      cursor = uplug_اگلا_Plug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plug_مقصد_Remove, حالت_2);
}




U_CAPI خالی U_EXPORT2 
uplug_سیٹ_Plug_No_Unload(UPlug_ڈیٹا *ڈیٹا, UBool dontUnload)
{
  ڈیٹا->dontUnload = dontUnload;
}


U_CAPI خالی U_EXPORT2
uplug_سیٹ_Plug_Level(UPlug_ڈیٹا *ڈیٹا, UPlugLevel level) {
  ڈیٹا->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlug_ڈیٹا *ڈیٹا) {
  واپس ڈیٹا->level;
}


U_CAPI خالی U_EXPORT2
uplug_سیٹ_Plug_Name(UPlug_ڈیٹا *ڈیٹا, ثابت حرف *name) {
  uprv_strncpy(ڈیٹا->name, name, UPLUG_NAME_MAX);
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_getPlugName(UPlug_ڈیٹا* ڈیٹا U_LIFETIME_BOUND) {
  واپس ڈیٹا->name;
}


U_CAPI ثابت حرف* U_EXPORT2
uplug_get_علامت_Name(UPlug_ڈیٹا* ڈیٹا U_LIFETIME_BOUND) {
  واپس ڈیٹا->sym;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getLibraryName(UPlug_ڈیٹا* ڈیٹا U_LIFETIME_BOUND, UError_رمز* حالت_2) {
  اگر(ڈیٹا->libName[0]) {
    واپس ڈیٹا->libName;
  } ورنہ {
#اگر U_ENABLE_DYLOAD
    واپس uplug_ڈھونڈو_Library(ڈیٹا->lib, حالت_2);
#ورنہ
    واپس خالی_اشاریہ;
#اگر_ختم
  }
}

U_CAPI خالی * U_EXPORT2
uplug_getLibrary(UPlug_ڈیٹا *ڈیٹا) {
  واپس ڈیٹا->lib;
}

U_CAPI خالی * U_EXPORT2
uplug_getContext(UPlug_ڈیٹا *ڈیٹا) {
  واپس ڈیٹا->context;
}


U_CAPI خالی U_EXPORT2
uplug_سیٹ_Context(UPlug_ڈیٹا *ڈیٹا, خالی *context) {
  ڈیٹا->context = context;
}

U_CAPI ثابت حرف* U_EXPORT2
uplug_getConfiguration(UPlug_ڈیٹا* ڈیٹا U_LIFETIME_BOUND) {
  واپس ڈیٹا->config;
}

U_CAPI UPlug_ڈیٹا* U_EXPORT2
uplug_get_Plug_اندرونی(int32_t n) { 
  اگر(n <0 || n >= plugin_گنتی) {
    واپس خالی_اشاریہ;
  } ورنہ { 
    واپس &(pluginList[n]);
  }
}


U_CAPI UError_رمز U_EXPORT2
uplug_get_Plug_بھرو_حالت(UPlug_ڈیٹا *plug) {
  واپس plug->plugin_حالت;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ساکن UPlug_ڈیٹا* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ثابت حرف *config, خالی *lib, ثابت حرف *sym,
                                                         UError_رمز *حالت_2) {
  UPlug_ڈیٹا *plug = خالی_اشاریہ;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, حالت_2);

  اگر(U_SUCCESS(*حالت_2)) {
    واپس plug;
  } ورنہ {
    uplug_deallocatePlug(plug, حالت_2);
    واپس خالی_اشاریہ;
  }
}

U_CAPI UPlug_ڈیٹا* U_EXPORT2
uplug_بھرو_Plug_From_Entrypoint(UPlugEntrypoint *entrypoint, ثابت حرف *config, UError_رمز *حالت_2) {
  UPlug_ڈیٹا* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, خالی_اشاریہ, خالی_اشاریہ, حالت_2);
  uplug_بھرو_Plug(plug, حالت_2);
  واپس plug;
}

#اگر U_ENABLE_DYLOAD

ساکن UPlug_ڈیٹا* 
uplug_initErrorPlug(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, ثابت حرف *nameOrError, UError_رمز بھرو_حالت, UError_رمز *حالت_2)
{
  UPlug_ڈیٹا *plug = uplug_allocate_خالی_خانے_Plug(حالت_2);
  اگر(U_FAILURE(*حالت_2)) واپس خالی_اشاریہ;

  plug->plugin_حالت = بھرو_حالت;
  plug->awaiting_بھرو = جھوٹ; /* Won't load. */
  plug->dontUnload = سچ; /* cannot unload. */

  اگر(sym!=خالی_اشاریہ) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  اگر(libName!=خالی_اشاریہ) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  اگر(nameOrError!=خالی_اشاریہ) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  اگر(config!=خالی_اشاریہ) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  واپس plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ساکن UPlug_ڈیٹا* 
uplug_initPlugFromLibrary(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UError_رمز *حالت_2) {
  خالی *lib = خالی_اشاریہ;
  UPlug_ڈیٹا *plug = خالی_اشاریہ;
  اگر(U_FAILURE(*حالت_2)) { واپس خالی_اشاریہ; }
  lib = uplug_openLibrary(libName, حالت_2);
  اگر(lib!=خالی_اشاریہ && U_SUCCESS(*حالت_2)) {
    UPlugEntrypoint *entrypoint = خالی_اشاریہ;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, حالت_2);

    اگر(entrypoint!=خالی_اشاریہ&&U_SUCCESS(*حالت_2)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, حالت_2);
      اگر(plug!=خالی_اشاریہ&&U_SUCCESS(*حالت_2)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = خالی_اشاریہ; /* library is now owned by plugin. */
      }
    } ورنہ {
      UError_رمز sub_حالت = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==خالی_اشاریہ)?U_MISSING_RESOURCE_ERROR:*حالت_2,&sub_حالت);
    }
    اگر(lib!=خالی_اشاریہ) { /* still need to close the lib */
      UError_رمز sub_حالت = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &sub_حالت); /* don't care here */
    }
  } ورنہ {
    UError_رمز sub_حالت = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==خالی_اشاریہ)?U_MISSING_RESOURCE_ERROR:*حالت_2,&sub_حالت);
  }
  واپس plug;
}

U_CAPI UPlug_ڈیٹا* U_EXPORT2
uplug_بھرو_Plug_From_Library(ثابت حرف *libName, ثابت حرف *sym, ثابت حرف *config, UError_رمز *حالت_2) { 
  UPlug_ڈیٹا *plug = خالی_اشاریہ;
  اگر(U_FAILURE(*حالت_2)) { واپس خالی_اشاریہ; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, حالت_2);
  uplug_بھرو_Plug(plug, حالت_2);

  واپس plug;
}

#اگر_ختم

ساکن UPlugLevel gCurrentLevel = UPLUG_LEVEL_کم;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  واپس gCurrentLevel;
}

ساکن UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlug_ڈیٹا *plugin_مقصد_Remove;
  /* cleanup plugs */
  برائے(i=0;i<plugin_گنتی;i++) {
    UError_رمز sub_حالت = U_ZERO_ERROR;
    plugin_مقصد_Remove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(plugin_مقصد_Remove, &sub_حالت);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_کم;
  واپس سچ;
}

#اگر U_ENABLE_DYLOAD

ساکن خالی uplug_بھرو_انتظار_Plugs(UError_رمز *حالت_2) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  اگر(U_FAILURE(*حالت_2)) {
    واپس;
  }
#اگر UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#اگر_ختم
  /* pass #1: low level plugs */
  برائے(i=0;i<plugin_گنتی;i++) {
    UError_رمز sub_حالت = U_ZERO_ERROR;
    UPlug_ڈیٹا *plugin_مقصد_بھرو = &pluginList[i];
    اگر(plugin_مقصد_بھرو->awaiting_بھرو) {
      اگر(plugin_مقصد_بھرو->level == UPLUG_LEVEL_کم) {
        اگر(currentLevel > UPLUG_LEVEL_کم) {
          plugin_مقصد_بھرو->plugin_حالت = U_PLUGIN_TOO_بلند;
        } ورنہ {
          UPlugLevel newLevel;
          uplug_بھرو_Plug(plugin_مقصد_بھرو, &sub_حالت);
          newLevel = uplug_getCurrentLevel();
          اگر(newLevel > currentLevel) {
            plugin_مقصد_بھرو->plugin_حالت = U_PLUGIN_بدلا_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        plugin_مقصد_بھرو->awaiting_بھرو = جھوٹ;
      } 
    }
  }    
  برائے(i=0;i<plugin_گنتی;i++) {
    UError_رمز sub_حالت = U_ZERO_ERROR;
    UPlug_ڈیٹا *plugin_مقصد_بھرو = &pluginList[i];
        
    اگر(plugin_مقصد_بھرو->awaiting_بھرو) {
      اگر(plugin_مقصد_بھرو->level == UPLUG_LEVEL_INVALID) { 
        plugin_مقصد_بھرو->plugin_حالت = U_PLUGIN_DIDNT_سیٹ_LEVEL;
      } ورنہ اگر(plugin_مقصد_بھرو->level == UPLUG_LEVEL_UNKNOWN) {
        plugin_مقصد_بھرو->plugin_حالت = U_اندرونی_پروگرام_ERROR;
      } ورنہ {
        uplug_بھرو_Plug(plugin_مقصد_بھرو, &sub_حالت);
      }
      plugin_مقصد_بھرو->awaiting_بھرو = جھوٹ;
    }
  }
    
#اگر UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#اگر_ختم
}

/* Name of the plugin config file */
ساکن حرف plugin_file[2048] = "";
#اگر_ختم

U_CAPI ثابت حرف* U_EXPORT2
uplug_getPluginFile() {
#اگر U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  واپس plugin_file;
#ورنہ
  واپس خالی_اشاریہ;
#اگر_ختم
}


//  uplug_init()  is called first thing from u_init().

U_CAPI خالی U_EXPORT2
uplug_init(UError_رمز *حالت_2) {
#اگر !U_ENABLE_DYLOAD
  (خالی)حالت_2; /*غیر_مستعمل*/
#وگرنہ_اگر !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ثابت حرف *env = getenv("ICU_PLUGINS");

  اگر(U_FAILURE(*حالت_2)) واپس;
  اگر(env != خالی_اشاریہ) {
    plugin_dir.append(env, -1, *حالت_2);
  }
  اگر(U_FAILURE(*حالت_2)) واپس;

#اگر defined(DEFAULT_ICU_PLUGINS) 
  اگر(plugin_dir.is_خالی_خانے()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *حالت_2);
  }
#اگر_ختم

#اگر UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.ڈیٹا()));
#اگر_ختم

  اگر(!plugin_dir.is_خالی_خانے()) {
    FILE *f;
        
    CharString pluginFile;
#اگر_تعریف_شدہ ICU_PLUGINS_DD
/* There are potentially a lot of ways to implement a plugin directory on OS390/zOS  */
/* Keeping in mind that unauthorized file access is logged, monitored, and enforced  */
/* I've chosen to open a DDNAME if BATCH and leave it alone for (presumably) UNIX    */
/* System Services.  Alternative techniques might be allocating a member in          */
/* SYS1.PARMLIB or setting an environment variable "ICU_PLUGIN_PATH" (?).  The       */
/* DDNAME can be connected to a file in the HFS if need be.                          */

    pluginFile.append("//DD:ICUPLUG", -1, *حالت_2);        /* JAM 20 Oct 2011 */
#ورنہ
    pluginFile.append(plugin_dir, *حالت_2);
    pluginFile.append(U_FILE_SEP_STRING, -1, *حالت_2);
    pluginFile.append("icuplugins", -1, *حالت_2);
    pluginFile.append(U_ICU_نسخہ_SHORT, -1, *حالت_2);
    pluginFile.append(".txt", -1, *حالت_2);
#اگر_ختم

#اگر UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*حالت_2)));
#اگر_ختم

    اگر(U_FAILURE(*حالت_2)) {
      واپس;
    }
    اگر((size_t)pluginFile.length() > (حجم(plugin_file)-1)) {
      *حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
#اگر UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*حالت_2)));
#اگر_ختم
      واپس;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, حجم(plugin_file), *حالت_2);

#اگر UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (صحیح)لڑی_لمبائی(plugin_file), (صحیح)حجم(plugin_file)));
#اگر_ختم
        
#اگر_تعریف_شدہ __MVS__
    اگر (iscics()) /* 12 Nov 2011 JAM */
    {
        f = خالی_اشاریہ;
    }
    ورنہ
#اگر_ختم
    {
        f = fopen(pluginFile.ڈیٹا(), "r");
    }

    اگر(f != خالی_اشاریہ) {
      حرف linebuf[1024];
      حرف *p, *libName=خالی_اشاریہ, *symName=خالی_اشاریہ, *config=خالی_اشاریہ;
      int32_t سطر = 0;
            
            
      جب(fgets(linebuf,1023,f)) {
        سطر++;

        اگر(!*linebuf || *linebuf=='#') {
          جاری;
        } ورنہ {
          p = linebuf;
          جب(*p&&خلا_ہے((صحیح)*p))
            p++;
          اگر(!*p || *p=='#') جاری;
          libName = p;
          جب(*p&&!خلا_ہے((صحیح)*p)) {
            p++;
          }
          اگر(!*p || *p=='#') جاری; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          جب(*p&&خلا_ہے((صحیح)*p)) {
            p++;
          }
          اگر(!*p||*p=='#') جاری; /* no symname after libname +tab */
          symName = p;
          جب(*p&&!خلا_ہے((صحیح)*p)) {
            p++;
          }
                    
          اگر(*p) { /* has config */
            *p=0;
            ++p;
            جب(*p&&خلا_ہے((صحیح)*p)) {
              p++;
            }
            اگر(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          اگر(config!=خالی_اشاریہ&&*config!=0) {
            p = config+لڑی_لمبائی(config);
            جب(p>config&&خلا_ہے((صحیح)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UError_رمز sub_حالت = U_ZERO_ERROR;
            UPlug_ڈیٹا *plug = uplug_initPlugFromLibrary(libName, symName, config, &sub_حالت);
            اگر(U_FAILURE(sub_حالت) && U_SUCCESS(*حالت_2)) {
              *حالت_2 = sub_حالت;
            }
#اگر UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (خالی*)plug, u_errorName(sub_حالت)));
#ورنہ
            (خالی)plug; /*غیر_مستعمل*/
#اگر_ختم
          }
        }
      }
      fclose(f);
    } ورنہ {
#اگر UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#اگر_ختم
    }
  }
  uplug_بھرو_انتظار_Plugs(حالت_2);
#اگر_ختم /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_بلند;
  ucln_رجسٹر_Cleanup(UCLN_UPLUG, uplug_cleanup);
}

#اگر_ختم


