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

#ಅನ್ತರ್ಭಾವ "unicode/icuplug.h"


#ಆದರೆ UCONFIG_ENABLE_PLUGINS


#ಅನ್ತರ್ಭಾವ "icuplugimp.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "ucln.h"
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MVS__  /* defined by z/OS compiler */
#ವ್ಯಾಖ್ಯಾನಿಸು _POSIX_SOURCE
#ಅನ್ತರ್ಭಾವ <cics.h> /* 12 Nov 2011 JAM iscics() function */
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ "charstr.h"

ಬಳಸಿ ನಾಮಾಕಾಶ icu;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UPLUG_TRACE
#ವ್ಯಾಖ್ಯಾನಿಸು UPLUG_TRACE 0
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ UPLUG_TRACE
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ವ್ಯಾಖ್ಯಾನಿಸು DBG(x) fprintf(stderr, "%s:%d: ",__FILE__,__LINE__); fprintf x
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Internal structure of an ICU plugin. 
 */

ರಚನೆ UPlug_ದತ್ತಾಂಶ {
  UPlugEntrypoint  *entrypoint; /**< plugin entrypoint */
  uint32_t struct_ಗಾತ್ರ;    /**< initialized to the size of this structure */
  uint32_t token;         /**< must be U_PLUG_TOKEN */
  ಶೂನ್ಯ *lib;              /**< plugin library, or nullptr */
  ಅಕ್ಷರ libName[UPLUG_NAME_MAX];   /**< library name */
  ಅಕ್ಷರ sym[UPLUG_NAME_MAX];        /**< plugin symbol, or nullptr */
  ಅಕ್ಷರ config[UPLUG_NAME_MAX];     /**< configuration data */
  ಶೂನ್ಯ *context;          /**< user context data */
  ಅಕ್ಷರ name[UPLUG_NAME_MAX];   /**< name of plugin */
  UPlugLevel  level; /**< level of plugin */
  UBool   awaiting_ತುಂಬು; /**< true if the plugin is awaiting a load call */
  UBool   dontUnload; /**< true if plugin must stay resident (leak plugin and lib) */
  UError_ಸಂಕೇತ plugin_ಸ್ಥಿತಿ; /**< status code of plugin */
};



#ವ್ಯಾಖ್ಯಾನಿಸು UPLUG_LIBRARY_INITIAL_ಎಣಿಕೆ 8
#ವ್ಯಾಖ್ಯಾನಿಸು UPLUG_PLUGIN_INITIAL_ಎಣಿಕೆ 12

/**
 * Remove an item
 * @param list the full list
 * @param listSize the number of entries in the list
 * @param memberSize the size of one member
 * @param itemToRemove the item number of the member
 * @return the new listsize 
 */
ಸ್ಥಿರ int32_t uplug_remove_ನಮೂದು_At(ಶೂನ್ಯ *list, int32_t list_ಗಾತ್ರ, int32_t member_ಗಾತ್ರ, int32_t ಅಂಶ_ಗಮ್ಯ_Remove) {
  uint8_t *bytePtr = (uint8_t *)list;
    
  /* get rid of some bad cases first */
  ಆದರೆ(list_ಗಾತ್ರ<1) {
    ಹಿಂತಿರುಗಿ list_ಗಾತ್ರ;
  }
    
  /* is there anything to move? */
  ಆದರೆ(list_ಗಾತ್ರ > ಅಂಶ_ಗಮ್ಯ_Remove+1) {
    memmove(bytePtr+(ಅಂಶ_ಗಮ್ಯ_Remove*member_ಗಾತ್ರ), bytePtr+((ಅಂಶ_ಗಮ್ಯ_Remove+1)*member_ಗಾತ್ರ), member_ಗಾತ್ರ);
  }
    
  ಹಿಂತಿರುಗಿ list_ಗಾತ್ರ-1;
}




#ಆದರೆ U_ENABLE_DYLOAD
/**
 * Library management. Internal. 
 * @internal
 */
ರಚನೆ UPlugLibrary;

/**
 * Library management. Internal. 
 * @internal
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UPlugLibrary {
  ಶೂನ್ಯ *lib;                           /**< library ptr */
  ಅಕ್ಷರ name[UPLUG_NAME_MAX]; /**< library name */
  uint32_t ref;                        /**< reference count */
} UPlugLibrary;

ಸ್ಥಿರ UPlugLibrary   staticLibraryList[UPLUG_LIBRARY_INITIAL_ಎಣಿಕೆ];
ಸ್ಥಿರ UPlugLibrary * libraryList = staticLibraryList;
ಸ್ಥಿರ int32_t library_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ int32_t libraryMax = UPLUG_LIBRARY_INITIAL_ಎಣಿಕೆ;

/**
 * Search for a library. Doesn't lock
 * @param libName libname to search for
 * @return the library's struct
 */
ಸ್ಥಿರ int32_t ಹುಡುಕು_For_Library_Name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libName) {
  int32_t i;
    
  ಗಾಗಿ(i=0;i<library_ಎಣಿಕೆ;i++) {
    ಆದರೆ(!uprv_strcmp(libName, libraryList[i].name)) {
      ಹಿಂತಿರುಗಿ i;
    }
  }
  ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ int32_t ಹುಡುಕು_For_Library(ಶೂನ್ಯ *lib) {
  int32_t i;
    
  ಗಾಗಿ(i=0;i<library_ಎಣಿಕೆ;i++) {
    ಆದರೆ(lib==libraryList[i].lib) {
      ಹಿಂತಿರುಗಿ i;
    }
  }
  ಹಿಂತಿರುಗಿ -1;
}

U_CAPI ಅಕ್ಷರ * U_EXPORT2
uplug_ಕಂಡುಹಿಡಿ_Library(ಶೂನ್ಯ *lib, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  int32_t libEnt;
  ಅಕ್ಷರ *ret = ಶೂನ್ಯಸೂಚಕ;
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  libEnt = ಹುಡುಕು_For_Library(lib);
  ಆದರೆ(libEnt!=-1) { 
    ret = libraryList[libEnt].name;
  } ಇಲ್ಲದಿದ್ದರೆ {
    *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
  }
  ಹಿಂತಿರುಗಿ ret;
}

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uplug_openLibrary(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libName, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  int32_t lib_ನಮೂದು = -1;
  ಶೂನ್ಯ *lib = ಶೂನ್ಯಸೂಚಕ;
    
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

  lib_ನಮೂದು = ಹುಡುಕು_For_Library_Name(libName);
  ಆದರೆ(lib_ನಮೂದು == -1) {
    lib_ನಮೂದು = library_ಎಣಿಕೆ++;
    ಆದರೆ(library_ಎಣಿಕೆ >= libraryMax) {
      /* Ran out of library slots. Statically allocated because we can't depend on allocating memory.. */
      *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
#ಆದರೆ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary() - out of library slots (max %d)\n", libraryMax));
#ಆದರೆ_ಅಂತ್ಯ
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    /* Some operating systems don't want 
       DL operations from multiple threads. */
    libraryList[lib_ನಮೂದು].lib = uprv_dl_open(libName, ಸ್ಥಿತಿ_2);
#ಆದರೆ UPLUG_TRACE
    DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*ಸ್ಥಿತಿ_2), lib_ನಮೂದು, lib));
#ಆದರೆ_ಅಂತ್ಯ
        
    ಆದರೆ(libraryList[lib_ನಮೂದು].lib == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
      /* cleanup. */
      libraryList[lib_ನಮೂದು].lib = ಶೂನ್ಯಸೂಚಕ; /* failure with open */
      libraryList[lib_ನಮೂದು].name[0] = 0;
#ಆದರೆ UPLUG_TRACE
      DBG((stderr, "uplug_openLibrary(%s,%s) libEntry %d, lib %p\n", libName, u_errorName(*ಸ್ಥಿತಿ_2), lib_ನಮೂದು, lib));
#ಆದರೆ_ಅಂತ್ಯ
      /* no need to free - just won't increase the count. */
      library_ಎಣಿಕೆ--;
    } ಇಲ್ಲದಿದ್ದರೆ { /* is it still there? */
      /* link it in */
      uprv_strncpy(libraryList[lib_ನಮೂದು].name,libName,UPLUG_NAME_MAX);
      libraryList[lib_ನಮೂದು].ref=1;
      lib = libraryList[lib_ನಮೂದು].lib;
    }

  } ಇಲ್ಲದಿದ್ದರೆ {
    lib = libraryList[lib_ನಮೂದು].lib;
    libraryList[lib_ನಮೂದು].ref++;
  }
  ಹಿಂತಿರುಗಿ lib;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_closeLibrary(ಶೂನ್ಯ *lib, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  int32_t i;
    
#ಆದರೆ UPLUG_TRACE
  DBG((stderr, "uplug_closeLibrary(%p,%s) list %p\n", lib, u_errorName(*ಸ್ಥಿತಿ_2), (ಶೂನ್ಯ*)libraryList));
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    
  ಗಾಗಿ(i=0;i<library_ಎಣಿಕೆ;i++) {
    ಆದರೆ(lib==libraryList[i].lib) {
      ಆದರೆ(--(libraryList[i].ref) == 0) {
        uprv_dl_close(libraryList[i].lib, ಸ್ಥಿತಿ_2);
        library_ಎಣಿಕೆ = uplug_remove_ನಮೂದು_At(libraryList, library_ಎಣಿಕೆ, ಗಾತ್ರ(*libraryList), i);
      }
      ಹಿಂತಿರುಗಿ;
    }
  }
  *ಸ್ಥಿತಿ_2 = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR; /* could not find the entry! */
}

#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ UPlug_ದತ್ತಾಂಶ pluginList[UPLUG_PLUGIN_INITIAL_ಎಣಿಕೆ];
ಸ್ಥಿರ int32_t plugin_ಎಣಿಕೆ = 0;



  
ಸ್ಥಿರ int32_t uplug_plugin_ಸಂಖ್ಯೆ(UPlug_ದತ್ತಾಂಶ* d) {
  UPlug_ದತ್ತಾಂಶ *pastPlug = &pluginList[plugin_ಎಣಿಕೆ];
  ಆದರೆ(d<=pluginList) {
    ಹಿಂತಿರುಗಿ 0;
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(d>=pastPlug) {
    ಹಿಂತಿರುಗಿ plugin_ಎಣಿಕೆ;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ (d-pluginList)/ಗಾತ್ರ(pluginList[0]);
  }
}


U_CAPI UPlug_ದತ್ತಾಂಶ * U_EXPORT2
uplug_ಮುಂದಿನ_Plug(UPlug_ದತ್ತಾಂಶ *ಪೂರ್ವ) {
  ಆದರೆ(ಪೂರ್ವ==ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ pluginList;
  } ಇಲ್ಲದಿದ್ದರೆ {
    UPlug_ದತ್ತಾಂಶ *ಮುಂದಿನ_Plug = &ಪೂರ್ವ[1];
    UPlug_ದತ್ತಾಂಶ *pastPlug = &pluginList[plugin_ಎಣಿಕೆ];
    
    ಆದರೆ(ಮುಂದಿನ_Plug>=pastPlug) {
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಹಿಂತಿರುಗಿ ಮುಂದಿನ_Plug;
    }
  }
}



/**
 * Call the plugin with some params
 */
ಸ್ಥಿರ ಶೂನ್ಯ uplug_callPlug(UPlug_ದತ್ತಾಂಶ *plug, UPlugReason reason, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  UPlugTokenReturn token;
  ಆದರೆ(plug==ಶೂನ್ಯಸೂಚಕ||U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ;
  }
  token = (*(plug->entrypoint))(plug, reason, ಸ್ಥಿತಿ_2);
  ಆದರೆ(token!=UPLUG_TOKEN) {
    *ಸ್ಥಿತಿ_2 = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
  }
}


ಸ್ಥಿರ ಶೂನ್ಯ uplug_unloadPlug(UPlug_ದತ್ತಾಂಶ *plug, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  ಆದರೆ(plug->awaiting_ತುಂಬು) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *ಸ್ಥಿತಿ_2 = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
    ಹಿಂತಿರುಗಿ; 
  }
  ಆದರೆ(U_SUCCESS(plug->plugin_ಸ್ಥಿತಿ)) {
    /* Don't unload a plug which has a failing load status - means it didn't actually load. */
    uplug_callPlug(plug, UPLUG_REASON_UNLOAD, ಸ್ಥಿತಿ_2);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ uplug_queryPlug(UPlug_ದತ್ತಾಂಶ *plug, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  ಆದರೆ(!plug->awaiting_ತುಂಬು || !(plug->level == UPLUG_LEVEL_UNKNOWN) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *ಸ್ಥಿತಿ_2 = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
    ಹಿಂತಿರುಗಿ; 
  }
  plug->level = UPLUG_LEVEL_INVALID;
  uplug_callPlug(plug, UPLUG_REASON_QUERY, ಸ್ಥಿತಿ_2);
  ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) { 
    ಆದರೆ(plug->level == UPLUG_LEVEL_INVALID) {
      plug->plugin_ಸ್ಥಿತಿ = U_PLUGIN_DIDNT_ಗಣ_LEVEL;
      plug->awaiting_ತುಂಬು = ಸುಳ್ಳು;
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    plug->plugin_ಸ್ಥಿತಿ = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
    plug->awaiting_ತುಂಬು = ಸುಳ್ಳು;
  }
}


ಸ್ಥಿರ ಶೂನ್ಯ uplug_ತುಂಬು_Plug(UPlug_ದತ್ತಾಂಶ *plug, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ;
  }
  ಆದರೆ(!plug->awaiting_ತುಂಬು || (plug->level < UPLUG_LEVEL_ಕೆಳಗಿನ) ) {  /* shouldn't happen. Plugin hasn't been loaded yet.*/
    *ಸ್ಥಿತಿ_2 = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
    ಹಿಂತಿರುಗಿ;
  }
  uplug_callPlug(plug, UPLUG_REASON_ತುಂಬು, ಸ್ಥಿತಿ_2);
  plug->awaiting_ತುಂಬು = ಸುಳ್ಳು;
  ಆದರೆ(!U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
    plug->plugin_ಸ್ಥಿತಿ = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
  }
}

ಸ್ಥಿರ UPlug_ದತ್ತಾಂಶ *uplug_allocate_ಖಾಲಿಗಳು_Plug(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
  UPlug_ದತ್ತಾಂಶ *plug = ಶೂನ್ಯಸೂಚಕ;

  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  ಆದರೆ(plugin_ಎಣಿಕೆ == UPLUG_PLUGIN_INITIAL_ಎಣಿಕೆ) {
    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  plug = &pluginList[plugin_ಎಣಿಕೆ++];

  plug->token = UPLUG_TOKEN;
  plug->struct_ಗಾತ್ರ = ಗಾತ್ರ(UPlug_ದತ್ತಾಂಶ);
  plug->name[0]=0;
  plug->level = UPLUG_LEVEL_UNKNOWN; /* initialize to null state */
  plug->awaiting_ತುಂಬು = ನಿಜ;
  plug->dontUnload = ಸುಳ್ಳು;
  plug->plugin_ಸ್ಥಿತಿ = U_ZERO_ERROR;
  plug->libName[0] = 0;
  plug->config[0]=0;
  plug->sym[0]=0;
  plug->lib=ಶೂನ್ಯಸೂಚಕ;
  plug->entrypoint=ಶೂನ್ಯಸೂಚಕ;


  ಹಿಂತಿರುಗಿ plug;
}

ಸ್ಥಿರ UPlug_ದತ್ತಾಂಶ *uplug_allocatePlug(UPlugEntrypoint *entrypoint, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config, ಶೂನ್ಯ *lib, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *symName,
                                     UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  UPlug_ದತ್ತಾಂಶ *plug = uplug_allocate_ಖಾಲಿಗಳು_Plug(ಸ್ಥಿತಿ_2);
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }

  ಆದರೆ(config!=ಶೂನ್ಯಸೂಚಕ) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  } ಇಲ್ಲದಿದ್ದರೆ {
    plug->config[0] = 0;
  }
    
  ಆದರೆ(symName!=ಶೂನ್ಯಸೂಚಕ) {
    uprv_strncpy(plug->sym, symName, UPLUG_NAME_MAX);
  } ಇಲ್ಲದಿದ್ದರೆ {
    plug->sym[0] = 0;
  }
    
  plug->entrypoint = entrypoint;
  plug->lib = lib;
  uplug_queryPlug(plug, ಸ್ಥಿತಿ_2);
    
  ಹಿಂತಿರುಗಿ plug;
}

ಸ್ಥಿರ ಶೂನ್ಯ uplug_deallocatePlug(UPlug_ದತ್ತಾಂಶ *plug, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
  ಆದರೆ(!plug->dontUnload) {
#ಆದರೆ U_ENABLE_DYLOAD
    uplug_closeLibrary(plug->lib, &sub_ಸ್ಥಿತಿ);
#ಆದರೆ_ಅಂತ್ಯ
  }
  plug->lib = ಶೂನ್ಯಸೂಚಕ;
  ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2) && U_FAILURE(sub_ಸ್ಥಿತಿ)) {
    *ಸ್ಥಿತಿ_2 = sub_ಸ್ಥಿತಿ;
  }
  /* shift plugins up and decrement count. */
  ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
    /* all ok- remove. */
    plugin_ಎಣಿಕೆ = uplug_remove_ನಮೂದು_At(pluginList, plugin_ಎಣಿಕೆ, ಗಾತ್ರ(plug[0]), uplug_plugin_ಸಂಖ್ಯೆ(plug));
  } ಇಲ್ಲದಿದ್ದರೆ {
    /* not ok- leave as a message. */
    plug->awaiting_ತುಂಬು=ಸುಳ್ಳು;
    plug->entrypoint=0;
    plug->dontUnload=ನಿಜ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ uplug_doUnloadPlug(UPlug_ದತ್ತಾಂಶ *plug_ಗಮ್ಯ_Remove, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  ಆದರೆ(plug_ಗಮ್ಯ_Remove != ಶೂನ್ಯಸೂಚಕ) {
    uplug_unloadPlug(plug_ಗಮ್ಯ_Remove, ಸ್ಥಿತಿ_2);
    uplug_deallocatePlug(plug_ಗಮ್ಯ_Remove, ಸ್ಥಿತಿ_2);
  }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_removePlug(UPlug_ದತ್ತಾಂಶ *plug, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)  {
  UPlug_ದತ್ತಾಂಶ *cursor = ಶೂನ್ಯಸೂಚಕ;
  UPlug_ದತ್ತಾಂಶ *plug_ಗಮ್ಯ_Remove = ಶೂನ್ಯಸೂಚಕ;
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    
  ಗಾಗಿ(cursor=pluginList;cursor!=ಶೂನ್ಯಸೂಚಕ;) {
    ಆದರೆ(cursor==plug) {
      plug_ಗಮ್ಯ_Remove = plug;
      cursor=ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
      cursor = uplug_ಮುಂದಿನ_Plug(cursor);
    }
  }
    
  uplug_doUnloadPlug(plug_ಗಮ್ಯ_Remove, ಸ್ಥಿತಿ_2);
}




U_CAPI ಶೂನ್ಯ U_EXPORT2 
uplug_ಗಣ_Plug_No_Unload(UPlug_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ, UBool dontUnload)
{
  ದತ್ತಾಂಶ->dontUnload = dontUnload;
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_ಗಣ_Plug_Level(UPlug_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ, UPlugLevel level) {
  ದತ್ತಾಂಶ->level = level;
}


U_CAPI UPlugLevel U_EXPORT2
uplug_getPlugLevel(UPlug_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ) {
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->level;
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_ಗಣ_Plug_Name(UPlug_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
  uprv_strncpy(ದತ್ತಾಂಶ->name, name, UPLUG_NAME_MAX);
}


U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uplug_getPlugName(UPlug_ದತ್ತಾಂಶ* ದತ್ತಾಂಶ U_LIFETIME_BOUND) {
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->name;
}


U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uplug_get_ಚಿಹ್ನೆ_Name(UPlug_ದತ್ತಾಂಶ* ದತ್ತಾಂಶ U_LIFETIME_BOUND) {
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->sym;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uplug_getLibraryName(UPlug_ದತ್ತಾಂಶ* ದತ್ತಾಂಶ U_LIFETIME_BOUND, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
  ಆದರೆ(ದತ್ತಾಂಶ->libName[0]) {
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->libName;
  } ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ U_ENABLE_DYLOAD
    ಹಿಂತಿರುಗಿ uplug_ಕಂಡುಹಿಡಿ_Library(ದತ್ತಾಂಶ->lib, ಸ್ಥಿತಿ_2);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
#ಆದರೆ_ಅಂತ್ಯ
  }
}

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uplug_getLibrary(UPlug_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ) {
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->lib;
}

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uplug_getContext(UPlug_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ) {
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->context;
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_ಗಣ_Context(UPlug_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ, ಶೂನ್ಯ *context) {
  ದತ್ತಾಂಶ->context = context;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uplug_getConfiguration(UPlug_ದತ್ತಾಂಶ* ದತ್ತಾಂಶ U_LIFETIME_BOUND) {
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->config;
}

U_CAPI UPlug_ದತ್ತಾಂಶ* U_EXPORT2
uplug_get_Plug_ಆಂತರಿಕ(int32_t n) { 
  ಆದರೆ(n <0 || n >= plugin_ಎಣಿಕೆ) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  } ಇಲ್ಲದಿದ್ದರೆ { 
    ಹಿಂತಿರುಗಿ &(pluginList[n]);
  }
}


U_CAPI UError_ಸಂಕೇತ U_EXPORT2
uplug_get_Plug_ತುಂಬು_ಸ್ಥಿತಿ(UPlug_ದತ್ತಾಂಶ *plug) {
  ಹಿಂತಿರುಗಿ plug->plugin_ಸ್ಥಿತಿ;
}




/**
 * Initialize a plugin from an entrypoint and library - but don't load it.
 */
ಸ್ಥಿರ UPlug_ದತ್ತಾಂಶ* uplug_initPlugFromEntrypointAndLibrary(UPlugEntrypoint *entrypoint, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config, ಶೂನ್ಯ *lib, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sym,
                                                         UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  UPlug_ದತ್ತಾಂಶ *plug = ಶೂನ್ಯಸೂಚಕ;

  plug = uplug_allocatePlug(entrypoint, config, lib, sym, ಸ್ಥಿತಿ_2);

  ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ plug;
  } ಇಲ್ಲದಿದ್ದರೆ {
    uplug_deallocatePlug(plug, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
}

U_CAPI UPlug_ದತ್ತಾಂಶ* U_EXPORT2
uplug_ತುಂಬು_Plug_From_Entrypoint(UPlugEntrypoint *entrypoint, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  UPlug_ದತ್ತಾಂಶ* plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
  uplug_ತುಂಬು_Plug(plug, ಸ್ಥಿತಿ_2);
  ಹಿಂತಿರುಗಿ plug;
}

#ಆದರೆ U_ENABLE_DYLOAD

ಸ್ಥಿರ UPlug_ದತ್ತಾಂಶ* 
uplug_initErrorPlug(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libName, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sym, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nameOrError, UError_ಸಂಕೇತ ತುಂಬು_ಸ್ಥಿತಿ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
  UPlug_ದತ್ತಾಂಶ *plug = uplug_allocate_ಖಾಲಿಗಳು_Plug(ಸ್ಥಿತಿ_2);
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

  plug->plugin_ಸ್ಥಿತಿ = ತುಂಬು_ಸ್ಥಿತಿ;
  plug->awaiting_ತುಂಬು = ಸುಳ್ಳು; /* Won't load. */
  plug->dontUnload = ನಿಜ; /* cannot unload. */

  ಆದರೆ(sym!=ಶೂನ್ಯಸೂಚಕ) {
    uprv_strncpy(plug->sym, sym, UPLUG_NAME_MAX);
  }

  ಆದರೆ(libName!=ಶೂನ್ಯಸೂಚಕ) {
    uprv_strncpy(plug->libName, libName, UPLUG_NAME_MAX);
  }

  ಆದರೆ(nameOrError!=ಶೂನ್ಯಸೂಚಕ) {
    uprv_strncpy(plug->name, nameOrError, UPLUG_NAME_MAX);
  }

  ಆದರೆ(config!=ಶೂನ್ಯಸೂಚಕ) {
    uprv_strncpy(plug->config, config, UPLUG_NAME_MAX);
  }

  ಹಿಂತಿರುಗಿ plug;
}

/**
 * Fetch a plugin from DLL, and then initialize it from a library- but don't load it.
 */
ಸ್ಥಿರ UPlug_ದತ್ತಾಂಶ* 
uplug_initPlugFromLibrary(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libName, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sym, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  ಶೂನ್ಯ *lib = ಶೂನ್ಯಸೂಚಕ;
  UPlug_ದತ್ತಾಂಶ *plug = ಶೂನ್ಯಸೂಚಕ;
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
  lib = uplug_openLibrary(libName, ಸ್ಥಿತಿ_2);
  ಆದರೆ(lib!=ಶೂನ್ಯಸೂಚಕ && U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
    UPlugEntrypoint *entrypoint = ಶೂನ್ಯಸೂಚಕ;
    entrypoint = (UPlugEntrypoint*)uprv_dlsym_func(lib, sym, ಸ್ಥಿತಿ_2);

    ಆದರೆ(entrypoint!=ಶೂನ್ಯಸೂಚಕ&&U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
      plug = uplug_initPlugFromEntrypointAndLibrary(entrypoint, config, lib, sym, ಸ್ಥಿತಿ_2);
      ಆದರೆ(plug!=ಶೂನ್ಯಸೂಚಕ&&U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        plug->lib = lib; /* plug takes ownership of library */
        lib = ಶೂನ್ಯಸೂಚಕ; /* library is now owned by plugin. */
      }
    } ಇಲ್ಲದಿದ್ದರೆ {
      UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
      plug = uplug_initErrorPlug(libName,sym,config,"ERROR: Could not load entrypoint",(lib==ಶೂನ್ಯಸೂಚಕ)?U_MISSING_RESOURCE_ERROR:*ಸ್ಥಿತಿ_2,&sub_ಸ್ಥಿತಿ);
    }
    ಆದರೆ(lib!=ಶೂನ್ಯಸೂಚಕ) { /* still need to close the lib */
      UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
      uplug_closeLibrary(lib, &sub_ಸ್ಥಿತಿ); /* don't care here */
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    plug = uplug_initErrorPlug(libName,sym,config,"ERROR: could not load library",(lib==ಶೂನ್ಯಸೂಚಕ)?U_MISSING_RESOURCE_ERROR:*ಸ್ಥಿತಿ_2,&sub_ಸ್ಥಿತಿ);
  }
  ಹಿಂತಿರುಗಿ plug;
}

U_CAPI UPlug_ದತ್ತಾಂಶ* U_EXPORT2
uplug_ತುಂಬು_Plug_From_Library(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libName, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sym, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) { 
  UPlug_ದತ್ತಾಂಶ *plug = ಶೂನ್ಯಸೂಚಕ;
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
  plug = uplug_initPlugFromLibrary(libName, sym, config, ಸ್ಥಿತಿ_2);
  uplug_ತುಂಬು_Plug(plug, ಸ್ಥಿತಿ_2);

  ಹಿಂತಿರುಗಿ plug;
}

#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ UPlugLevel gCurrentLevel = UPLUG_LEVEL_ಕೆಳಗಿನ;

U_CAPI UPlugLevel U_EXPORT2 uplug_getCurrentLevel() {
  ಹಿಂತಿರುಗಿ gCurrentLevel;
}

ಸ್ಥಿರ UBool U_CALLCONV uplug_cleanup()
{
  int32_t i;
    
  UPlug_ದತ್ತಾಂಶ *plugin_ಗಮ್ಯ_Remove;
  /* cleanup plugs */
  ಗಾಗಿ(i=0;i<plugin_ಎಣಿಕೆ;i++) {
    UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    plugin_ಗಮ್ಯ_Remove = &pluginList[i];
    /* unload and deallocate */
    uplug_doUnloadPlug(plugin_ಗಮ್ಯ_Remove, &sub_ಸ್ಥಿತಿ);
  }
  /* close other held libs? */
  gCurrentLevel = UPLUG_LEVEL_ಕೆಳಗಿನ;
  ಹಿಂತಿರುಗಿ ನಿಜ;
}

#ಆದರೆ U_ENABLE_DYLOAD

ಸ್ಥಿರ ಶೂನ್ಯ uplug_ತುಂಬು_ಕಾಯುವಿಕೆ_Plugs(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  int32_t i;
  UPlugLevel currentLevel = uplug_getCurrentLevel();
    
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
    ಹಿಂತಿರುಗಿ;
  }
#ಆದರೆ UPLUG_TRACE
  DBG((stderr,  "uplug_loadWaitingPlugs() Level: %d\n", currentLevel));
#ಆದರೆ_ಅಂತ್ಯ
  /* pass #1: low level plugs */
  ಗಾಗಿ(i=0;i<plugin_ಎಣಿಕೆ;i++) {
    UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    UPlug_ದತ್ತಾಂಶ *plugin_ಗಮ್ಯ_ತುಂಬು = &pluginList[i];
    ಆದರೆ(plugin_ಗಮ್ಯ_ತುಂಬು->awaiting_ತುಂಬು) {
      ಆದರೆ(plugin_ಗಮ್ಯ_ತುಂಬು->level == UPLUG_LEVEL_ಕೆಳಗಿನ) {
        ಆದರೆ(currentLevel > UPLUG_LEVEL_ಕೆಳಗಿನ) {
          plugin_ಗಮ್ಯ_ತುಂಬು->plugin_ಸ್ಥಿತಿ = U_PLUGIN_TOO_ಮೇಲಿನ;
        } ಇಲ್ಲದಿದ್ದರೆ {
          UPlugLevel newLevel;
          uplug_ತುಂಬು_Plug(plugin_ಗಮ್ಯ_ತುಂಬು, &sub_ಸ್ಥಿತಿ);
          newLevel = uplug_getCurrentLevel();
          ಆದರೆ(newLevel > currentLevel) {
            plugin_ಗಮ್ಯ_ತುಂಬು->plugin_ಸ್ಥಿತಿ = U_PLUGIN_ಬದಲಾಯಿತು_LEVEL_WARNING;
            currentLevel = newLevel;
          }
        }
        plugin_ಗಮ್ಯ_ತುಂಬು->awaiting_ತುಂಬು = ಸುಳ್ಳು;
      } 
    }
  }    
  ಗಾಗಿ(i=0;i<plugin_ಎಣಿಕೆ;i++) {
    UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    UPlug_ದತ್ತಾಂಶ *plugin_ಗಮ್ಯ_ತುಂಬು = &pluginList[i];
        
    ಆದರೆ(plugin_ಗಮ್ಯ_ತುಂಬು->awaiting_ತುಂಬು) {
      ಆದರೆ(plugin_ಗಮ್ಯ_ತುಂಬು->level == UPLUG_LEVEL_INVALID) { 
        plugin_ಗಮ್ಯ_ತುಂಬು->plugin_ಸ್ಥಿತಿ = U_PLUGIN_DIDNT_ಗಣ_LEVEL;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(plugin_ಗಮ್ಯ_ತುಂಬು->level == UPLUG_LEVEL_UNKNOWN) {
        plugin_ಗಮ್ಯ_ತುಂಬು->plugin_ಸ್ಥಿತಿ = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
      } ಇಲ್ಲದಿದ್ದರೆ {
        uplug_ತುಂಬು_Plug(plugin_ಗಮ್ಯ_ತುಂಬು, &sub_ಸ್ಥಿತಿ);
      }
      plugin_ಗಮ್ಯ_ತುಂಬು->awaiting_ತುಂಬು = ಸುಳ್ಳು;
    }
  }
    
#ಆದರೆ UPLUG_TRACE
  DBG((stderr,  " Done Loading Plugs. Level: %d\n", (int32_t)uplug_getCurrentLevel()));
#ಆದರೆ_ಅಂತ್ಯ
}

/* Name of the plugin config file */
ಸ್ಥಿರ ಅಕ್ಷರ plugin_file[2048] = "";
#ಆದರೆ_ಅಂತ್ಯ

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uplug_getPluginFile() {
#ಆದರೆ U_ENABLE_DYLOAD && !UCONFIG_NO_FILE_IO
  ಹಿಂತಿರುಗಿ plugin_file;
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
#ಆದರೆ_ಅಂತ್ಯ
}


//  uplug_init()  is called first thing from u_init().

U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_init(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
#ಆದರೆ !U_ENABLE_DYLOAD
  (ಶೂನ್ಯ)ಸ್ಥಿತಿ_2; /*ಬಳಸದ*/
#ಅಥವಾಆದರೆ !UCONFIG_NO_FILE_IO
  CharString plugin_dir;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *env = getenv("ICU_PLUGINS");

  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
  ಆದರೆ(env != ಶೂನ್ಯಸೂಚಕ) {
    plugin_dir.append(env, -1, *ಸ್ಥಿತಿ_2);
  }
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;

#ಆದರೆ defined(DEFAULT_ICU_PLUGINS) 
  ಆದರೆ(plugin_dir.is_ಖಾಲಿಗಳು()) {
    plugin_dir.append(DEFAULT_ICU_PLUGINS, -1, *ಸ್ಥಿತಿ_2);
  }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ UPLUG_TRACE
  DBG((stderr, "ICU_PLUGINS=%s\n", plugin_dir.ದತ್ತಾಂಶ()));
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!plugin_dir.is_ಖಾಲಿಗಳು()) {
    FILE *f;
        
    CharString pluginFile;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ICU_PLUGINS_DD
/* There are potentially a lot of ways to implement a plugin directory on OS390/zOS  */
/* Keeping in mind that unauthorized file access is logged, monitored, and enforced  */
/* I've chosen to open a DDNAME if BATCH and leave it alone for (presumably) UNIX    */
/* System Services.  Alternative techniques might be allocating a member in          */
/* SYS1.PARMLIB or setting an environment variable "ICU_PLUGIN_PATH" (?).  The       */
/* DDNAME can be connected to a file in the HFS if need be.                          */

    pluginFile.append("//DD:ICUPLUG", -1, *ಸ್ಥಿತಿ_2);        /* JAM 20 Oct 2011 */
#ಇಲ್ಲದಿದ್ದರೆ
    pluginFile.append(plugin_dir, *ಸ್ಥಿತಿ_2);
    pluginFile.append(U_FILE_SEP_STRING, -1, *ಸ್ಥಿತಿ_2);
    pluginFile.append("icuplugins", -1, *ಸ್ಥಿತಿ_2);
    pluginFile.append(U_ICU_ಆವೃತ್ತಿ_SHORT, -1, *ಸ್ಥಿತಿ_2);
    pluginFile.append(".txt", -1, *ಸ್ಥಿತಿ_2);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ UPLUG_TRACE
    DBG((stderr, "status=%s\n", u_errorName(*ಸ್ಥಿತಿ_2)));
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
      ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ((size_t)pluginFile.length() > (ಗಾತ್ರ(plugin_file)-1)) {
      *ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
#ಆದರೆ UPLUG_TRACE
      DBG((stderr, "status=%s\n", u_errorName(*ಸ್ಥಿತಿ_2)));
#ಆದರೆ_ಅಂತ್ಯ
      ಹಿಂತಿರುಗಿ;
    }
    
    /* plugin_file is not used for processing - it is only used 
       so that uplug_getPluginFile() works (i.e. icuinfo)
    */
    pluginFile.extract(plugin_file, ಗಾತ್ರ(plugin_file), *ಸ್ಥಿತಿ_2);

#ಆದರೆ UPLUG_TRACE
    DBG((stderr, "pluginfile= %s len %d/%d\n", plugin_file, (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(plugin_file), (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(plugin_file)));
#ಆದರೆ_ಅಂತ್ಯ
        
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MVS__
    ಆದರೆ (iscics()) /* 12 Nov 2011 JAM */
    {
        f = ಶೂನ್ಯಸೂಚಕ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
        f = fopen(pluginFile.ದತ್ತಾಂಶ(), "r");
    }

    ಆದರೆ(f != ಶೂನ್ಯಸೂಚಕ) {
      ಅಕ್ಷರ linebuf[1024];
      ಅಕ್ಷರ *p, *libName=ಶೂನ್ಯಸೂಚಕ, *symName=ಶೂನ್ಯಸೂಚಕ, *config=ಶೂನ್ಯಸೂಚಕ;
      int32_t ಸಾಲು = 0;
            
            
      ಆಗ(fgets(linebuf,1023,f)) {
        ಸಾಲು++;

        ಆದರೆ(!*linebuf || *linebuf=='#') {
          ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ {
          p = linebuf;
          ಆಗ(*p&&ಖಾಲಿಯೇ((ಪೂರ್ಣಾಂಕ)*p))
            p++;
          ಆದರೆ(!*p || *p=='#') ಮುಂದುವರಿ;
          libName = p;
          ಆಗ(*p&&!ಖಾಲಿಯೇ((ಪೂರ್ಣಾಂಕ)*p)) {
            p++;
          }
          ಆದರೆ(!*p || *p=='#') ಮುಂದುವರಿ; /* no tab after libname */
          *p=0; /* end of libname */
          p++;
          ಆಗ(*p&&ಖಾಲಿಯೇ((ಪೂರ್ಣಾಂಕ)*p)) {
            p++;
          }
          ಆದರೆ(!*p||*p=='#') ಮುಂದುವರಿ; /* no symname after libname +tab */
          symName = p;
          ಆಗ(*p&&!ಖಾಲಿಯೇ((ಪೂರ್ಣಾಂಕ)*p)) {
            p++;
          }
                    
          ಆದರೆ(*p) { /* has config */
            *p=0;
            ++p;
            ಆಗ(*p&&ಖಾಲಿಯೇ((ಪೂರ್ಣಾಂಕ)*p)) {
              p++;
            }
            ಆದರೆ(*p) {
              config = p;
            }
          }
                    
          /* chop whitespace at the end of the config */
          ಆದರೆ(config!=ಶೂನ್ಯಸೂಚಕ&&*config!=0) {
            p = config+ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(config);
            ಆಗ(p>config&&ಖಾಲಿಯೇ((ಪೂರ್ಣಾಂಕ)*(--p))) {
              *p=0;
            }
          }
                
          /* OK, we're good. */
          { 
            UError_ಸಂಕೇತ sub_ಸ್ಥಿತಿ = U_ZERO_ERROR;
            UPlug_ದತ್ತಾಂಶ *plug = uplug_initPlugFromLibrary(libName, symName, config, &sub_ಸ್ಥಿತಿ);
            ಆದರೆ(U_FAILURE(sub_ಸ್ಥಿತಿ) && U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
              *ಸ್ಥಿತಿ_2 = sub_ಸ್ಥಿತಿ;
            }
#ಆದರೆ UPLUG_TRACE
            DBG((stderr, "PLUGIN libName=[%s], sym=[%s], config=[%s]\n", libName, symName, config));
            DBG((stderr, " -> %p, %s\n", (ಶೂನ್ಯ*)plug, u_errorName(sub_ಸ್ಥಿತಿ)));
#ಇಲ್ಲದಿದ್ದರೆ
            (ಶೂನ್ಯ)plug; /*ಬಳಸದ*/
#ಆದರೆ_ಅಂತ್ಯ
          }
        }
      }
      fclose(f);
    } ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ UPLUG_TRACE
      DBG((stderr, "Can't open plugin file %s\n", plugin_file));
#ಆದರೆ_ಅಂತ್ಯ
    }
  }
  uplug_ತುಂಬು_ಕಾಯುವಿಕೆ_Plugs(ಸ್ಥಿತಿ_2);
#ಆದರೆ_ಅಂತ್ಯ /* U_ENABLE_DYLOAD */
  gCurrentLevel = UPLUG_LEVEL_ಮೇಲಿನ;
  ucln_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_UPLUG, uplug_cleanup);
}

#ಆದರೆ_ಅಂತ್ಯ


