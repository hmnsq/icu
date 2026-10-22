// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2000-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#నిర్వచితం_కాకపోతే URESIMP_H
#నిర్వచించు URESIMP_H

#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/utypes.h"

#అన్తర్భావ "uresdata.h"

#నిర్వచించు k_మూలం_Locale_Name         "root"
#నిర్వచించు kPoolBundleName         "pool"

/*
 The default minor version and the version separator must be exactly one
 character long.
*/

#నిర్వచించు k_Default_Minor_సంస్కరణ    "0"
#నిర్వచించు k_సంస్కరణ_Separator       "."
#నిర్వచించు k_సంస్కరణ_ట్యాగ్             "Version"

#నిర్వచించు MAGIC1 19700503
#నిర్వచించు MAGIC2 19641227

#నిర్వచించు URES_MAX_ALIAS_LEVEL 256

#నిర్వచించు ఖాళీలు_సమితి 0x2205

నిర్మాణం UResource_దత్తాంశం_నమోదు;
రకనిర్వచనం నిర్మాణం UResource_దత్తాంశం_నమోదు UResource_దత్తాంశం_నమోదు;

/*
 * Note: If we wanted to make this structure smaller, then we could try
 * to use one UResourceDataEntry pointer for fAlias and fPool, with a separate
 * flag to distinguish whether this struct is for a real bundle with a pool,
 * or for an alias entry for which we won't use the pool after loading.
 */
నిర్మాణం UResource_దత్తాంశం_నమోదు {
    అక్షరం *fName; /* name of the locale for bundle - still to decide whether it is original or fallback */
    అక్షరం *f_మార్గం; /* path to bundle - used for distinguishing between resources with the same name */
    UResource_దత్తాంశం_నమోదు *f_మాతృ; /*next resource in fallback chain*/
    UResource_దత్తాంశం_నమోదు *fAlias;
    UResource_దత్తాంశం_నమోదు *fPool;
    Resource_దత్తాంశం f_దత్తాంశం; /* data for low level access */
    అక్షరం f_Name_బఫరు[3]; /* A small buffer of free space for fName. The free space is due to struct padding. */
    uint32_t f_లెక్క_Existing; /* how much is this resource used */
    UError_సంకేతం fBogus;
    /* int32_t fHashKey;*/ /* for faster access in the hashtable */
};

#నిర్వచించు RES_BUFSIZE 64
#నిర్వచించు RES_మార్గం_SEPARATOR   '/'
#నిర్వచించు RES_మార్గం_SEPARATOR_S   "/"

U_CAPI శూన్య U_EXPORT2 ures_init_దొంతర_Object(UResourceBundle* resB);

#నిర్వచితమైతే __cplusplus

నిర్మాణం UResourceBundle {
    స్థిరరాశి అక్షరం *f_కీలకం; /*ట్యాగ్*/
    /**
     * The dataEntry for the actual locale in which this item lives.
     * Used for accessing the item's data.
     * Non-const pointer for reference counting via entryIncrease().
     */
    UResource_దత్తాంశం_నమోదు *f_దత్తాంశం; /*for low-level access*/
    అక్షరం *f_సంస్కరణ;
    /**
     * The dataEntry for the valid locale.
     * Used for /LOCALE/path alias resolution that starts back from the valid locale,
     * rather than from the actual locale of this item which might live in
     * an ancestor bundle.
     */
    UResource_దత్తాంశం_నమోదు *f_Valid_Locale_దత్తాంశం_నమోదు;
    అక్షరం *f_Res_మార్గం; /* full path to the resource: "zh_TW/CollationElements/Sequence" */
    అక్షరం fResBuf[RES_BUFSIZE];
    int32_t f_Res_మార్గం_Len;
    Resource fRes;
    UBool fHasFallback;
    UBool f_Is_శిఖరం_Level;
    uint32_t fMagic1;   /* For determining if it's a stack object */
    uint32_t fMagic2;   /* For determining if it's a stack object */
    int32_t f_సూచీ;
    int32_t f_కొలత;

    అంతఃవరుస స్థిరరాశి Resource_దత్తాంశం &get_Res_దత్తాంశం() స్థిరరాశి { తిరిగి f_దత్తాంశం->f_దత్తాంశం; }
};

U_NAMESPACE_BEGIN

/**
 * \class StackUResourceBundle
 * "Smart pointer" like class, closes a UResourceBundle via ures_close().
 *
 * This code:
 *
 *   StackUResourceBundle bundle;
 *   foo(bundle.getAlias());
 *
 * Is equivalent to this code:
 *
 *   UResourceBundle bundle;
 *   ures_initStackObject(&bundle);
 *   foo(&bundle);
 *   ures_close(&bundle);
 *
 * @see LocalUResourceBundlePointer
 * @internal
 */
class U_COMMON_API దొంతర_UResource_Bundle {
బహిరంగ:
    // No heap allocation. Use only on the stack.
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new(size_t) noexcept = delete;
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new[](size_t) noexcept = delete;
    స్థిర శూన్య* U_EXPORT2 క్రియాకారకం new(size_t, శూన్య*) noexcept = delete;

    దొంతర_UResource_Bundle();
    ~దొంతర_UResource_Bundle();

    UResourceBundle* getAlias() U_LIFETIME_BOUND { తిరిగి &bundle; }

    UResourceBundle& ref() U_LIFETIME_BOUND { తిరిగి bundle; }
    స్థిరరాశి UResourceBundle& ref() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి bundle; }

    దొంతర_UResource_Bundle(స్థిరరాశి దొంతర_UResource_Bundle&) = delete;
    దొంతర_UResource_Bundle& క్రియాకారకం=(స్థిరరాశి దొంతర_UResource_Bundle&) = delete;

    దొంతర_UResource_Bundle(దొంతర_UResource_Bundle&&) = delete;
    దొంతర_UResource_Bundle& క్రియాకారకం=(దొంతర_UResource_Bundle&&) = delete;

అంతరంగ:
    UResourceBundle bundle;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  /* __cplusplus */

/**
 * Opens a resource bundle for the locale;
 * if there is not even a base language bundle, then loads the root bundle;
 * never falls back to the default locale.
 *
 * This is used for algorithms that have good pan-Unicode default behavior,
 * such as case mappings, collation, and segmentation (BreakIterator).
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* localeID, UError_సంకేతం* స్థితి_2);

/* Some getters used by the copy constructor */
U_CFUNC స్థిరరాశి అక్షరం* ures_getName(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND);
#నిర్వచితమైతే URES_DEBUG
U_CFUNC స్థిరరాశి అక్షరం* ures_get_మార్గం(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND);
/**
 * If anything was in the RB cache, dump it to the screen.
 * @return true if there was anything into the cache
 */
U_CAPI UBool U_EXPORT2 ures_dumpCacheContents(శూన్య);
#అయితే_ముగింపు

/* Candidates for export */
U_CFUNC UResourceBundle*
ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
              స్థిరరాశి UResourceBundle* original,
              UError_సంకేతం* స్థితి_2);

/**
 * Returns a resource that can be located using the pathToResource argument. One needs optional package, locale
 * and path inside the locale, for example: "/myData/en/zoneStrings/3". Keys and indexes are supported. Keys
 * need to reference data in named structures, while indexes can reference both named and anonymous resources.
 * Features a fill-in parameter. 
 * 
 * Note, this function does NOT have a syntax for specifying items within a tree.  May want to consider a
 * syntax that delineates between package/tree and resource.  
 *
 * @param pathToResource    a path that will lead to the requested resource
 * @param fillIn            if NULL a new UResourceBundle struct is allocated and must be deleted by the caller.
 *                          Alternatively, you can supply a struct to be filled by this function.
 * @param status            fills in the outgoing error code.
 * @return                  a pointer to a UResourceBundle struct. If fill in param was NULL, caller must delete it
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_కనుగొను_Resource(స్థిరరాశి అక్షరం* మార్గం_గమ్యం_Resource,
                  UResourceBundle* fill_లోపల U_LIFETIME_BOUND,
                  UError_సంకేతం* స్థితి_2);

/**
 * Returns a sub resource that can be located using the pathToResource argument. One needs a path inside 
 * the supplied resource, for example, if you have "en_US" resource bundle opened, you might ask for
 * "zoneStrings/3". Keys and indexes are supported. Keys
 * need to reference data in named structures, while indexes can reference both 
 * named and anonymous resources.
 * Features a fill-in parameter. 
 *
 * @param resourceBundle    a resource
 * @param pathToResource    a path that will lead to the requested resource
 * @param fillIn            if NULL a new UResourceBundle struct is allocated and must be deleted by the caller.
 *                          Alternatively, you can supply a struct to be filled by this function.
 * @param status            fills in the outgoing error code.
 * @return                  a pointer to a UResourceBundle struct. If fill in param was NULL, caller must delete it
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_కనుగొను_Sub_Resource(స్థిరరాశి UResourceBundle* resB,
                     అక్షరం* మార్గం_గమ్యం_Resource,
                     UResourceBundle* fill_లోపల U_LIFETIME_BOUND,
                     UError_సంకేతం* స్థితి_2);

/**
 * Returns a functionally equivalent locale (considering keywords) for the specified keyword.
 * @param result fillin for the equivalent locale
 * @param resultCapacity capacity of the fillin buffer
 * @param path path to the tree, or NULL for ICU data
 * @param resName top level resource. Example: "collations"
 * @param keyword locale keyword. Example: "collation"
 * @param locid The requested locale
 * @param isAvailable If non-null, pointer to fillin parameter that indicates whether the 
 * requested locale was available. The locale is defined as 'available' if it physically 
 * exists within the specified tree.
 * @param omitDefault if true, omit keyword and value if default. 'de_DE\@collation=standard' -> 'de_DE'
 * @param status error code
 * @return  the actual buffer size needed for the full locale.  If it's greater 
 * than resultCapacity, the returned full name will be truncated and an error code will be returned.
 */
U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(అక్షరం *ఫలితం, int32_t ఫలితం_సామర్థ్యం, 
                             స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *resName, స్థిరరాశి అక్షరం *keyword, స్థిరరాశి అక్షరం *locid,
                             UBool *is_అందుబాటు, UBool omitDefault, UError_సంకేతం *స్థితి_2);

/**
 * Given a tree path and keyword, return a string enumeration of all possible values for that keyword.
 * @param path path to the tree, or NULL for ICU data
 * @param keyword a particular keyword to consider, must match a top level resource name 
 * within the tree.
 * @param status error code
 */
U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_విలువలు(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *keyword, UError_సంకేతం *స్థితి_2);


/**
 * Get a resource with multi-level fallback. Normally only the top level resources will
 * fallback to its parent. This performs fallback on subresources. For example, when a table
 * is defined in a resource bundle and a parent resource bundle, normally no fallback occurs
 * on the sub-resources because the table is defined in the current resource bundle, but this
 * function can perform fallback on the sub-resources of the table.
 * @param resB              a resource
 * @param inKey             a key associated with the requested resource
 * @param fillIn            if NULL a new UResourceBundle struct is allocated and must be deleted by the caller.
 *                          Alternatively, you can supply a struct to be filled by this function.
 * @param status: fills in the outgoing error code
 *                could be <TT>U_MISSING_RESOURCE_ERROR</TT> if the key is not found
 *                could be a non-failing error 
 *                e.g.: <TT>U_USING_FALLBACK_WARNING</TT>,<TT>U_USING_DEFAULT_WARNING </TT>
 * @return                  a pointer to a UResourceBundle struct. If fill in param was NULL, caller must delete it
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_కీలకం_With_Fallback(స్థిరరాశి UResourceBundle* resB,
                          స్థిరరాశి అక్షరం* లోపల_కీలకం,
                          UResourceBundle* fill_లోపల U_LIFETIME_BOUND,
                          UError_సంకేతం* స్థితి_2);

/**
 * Get a String with multi-level fallback. Normally only the top level resources will
 * fallback to its parent. This performs fallback on subresources. For example, when a table
 * is defined in a resource bundle and a parent resource bundle, normally no fallback occurs
 * on the sub-resources because the table is defined in the current resource bundle, but this
 * function can perform fallback on the sub-resources of the table.
 * @param resB              a resource
 * @param inKey             a key associated with the requested resource
 * @param len               if not NULL, used to return the length of the string
 * @param status: fills in the outgoing error code
 *                could be <TT>U_MISSING_RESOURCE_ERROR</TT> if the key is not found
 *                could be a non-failing error 
 *                e.g.: <TT>U_USING_FALLBACK_WARNING</TT>,<TT>U_USING_DEFAULT_WARNING </TT>
 * @return returns a pointer to a zero-terminated UChar array which lives in a
 *         memory mapped/DLL file.
 */
U_CAPI స్థిరరాశి UChar* U_EXPORT2
ures_get_String_By_కీలకం_With_Fallback(స్థిరరాశి UResourceBundle* resB U_LIFETIME_BOUND,
                                స్థిరరాశి అక్షరం* లోపల_కీలకం,
                                int32_t* len,
                                UError_సంకేతం* స్థితి_2);

#నిర్వచితమైతే __cplusplus

U_CAPI శూన్య U_EXPORT2
ures_get_విలువ_With_Fallback(స్థిరరాశి UResourceBundle *bundle, స్థిరరాశి అక్షరం *మార్గం,
                          UResourceBundle *తాత్కాలిక_Fill_లోపల,
                          icu::Resource_దత్తాంశం_విలువ &విలువ_2, UError_సంకేతం &error_సంకేతం);

/**
 * Locates the resource specified by `path` in the resource bundle specified by `bundle` (performing any
 * necessary fallback and following any aliases) and calls the specified `sink`'s `put()` method with that
 * resource.  Then walks the bundle's parent chain, calling `put()` on the sink for each item in the
 * parent chain.
 * @param bundle The bundle to search
 * @param path The path of the desired resource
 * @param sink A `ResourceSink` that gets called for each resource in the parent chain
 * @param errorCode The error code
 */
U_CAPI శూన్య U_EXPORT2
ures_get_All_వస్తువులు_With_Fallback(స్థిరరాశి UResourceBundle *bundle, స్థిరరాశి అక్షరం *మార్గం,
                             icu::ResourceSink &sink, UError_సంకేతం &error_సంకేతం);

/**
 * Locates the resource specified by `path` in the resource bundle specified by `bundle` (performing any
 * necessary fallback and following any aliases) and, if the resource is a table resource, iterates over its
 * immediate child resources (again, following any aliases to get the individual resource values), and calls the specified
 * `sink`'s `put()` method for each child resource (passing it that resource's key and either its actual value or,
 * if that value is an alias, the value you get by following the alias).  Then walks back over the bundle's
 * parent chain, similarly iterating over each parent table resource's child resources.
 * Does not descend beyond one level of table children.
 * @param bundle The bundle to search
 * @param path The path of the desired resource
 * @param sink A `ResourceSink` that gets called for each child resource of the specified resource (and each child
 * of the resources in its parent chain).
 * @param errorCode The error code.  This will be U_RESOURCE_TYPE_MISMATCH if the resource the caller
 * is asking for isn't a table resource.
 */
U_CAPI శూన్య U_EXPORT2
ures_getAllChildrenWithFallback(స్థిరరాశి UResourceBundle *bundle, స్థిరరాశి అక్షరం *మార్గం,
                                icu::ResourceSink &sink, UError_సంకేతం &error_సంకేతం);

#అయితే_ముగింపు  /* __cplusplus */

/**
 * Get a version number by key
 * @param resB bundle containing version number
 * @param key the key for the version number
 * @param ver fillin for the version number
 * @param status error code
 */
U_CAPI శూన్య U_EXPORT2
ures_get_సంస్కరణ_By_కీలకం(స్థిరరాశి UResourceBundle *resB,
                     స్థిరరాశి అక్షరం *కీలకం,
                     UVersionInfo ver,
                     UError_సంకేతం *స్థితి_2);


/**
 * Internal function.
 * Return the version number associated with this ResourceBundle as a string.
 *
 * @param resourceBundle The resource bundle for which the version is checked.
 * @return  A version number string as specified in the resource bundle or its parent.
 *          The caller does not own this string.
 * @see ures_getVersion
 */
U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ures_get_సంస్కరణ_సంఖ్య_అంతర్గత(స్థిరరాశి UResourceBundle* resourceBundle U_LIFETIME_BOUND);

/**
 * Return the name of the Locale associated with this ResourceBundle. This API allows
 * you to query for the real locale of the resource. For example, if you requested 
 * "en_US_CALIFORNIA" and only "en_US" bundle exists, "en_US" will be returned. 
 * For subresources, the locale where this resource comes from will be returned.
 * If fallback has occurred, getLocale will reflect this.
 *
 * This internal version avoids deprecated-warnings in ICU code.
 *
 * @param resourceBundle resource bundle in question
 * @param status just for catching illegal arguments
 * @return  A Locale name
 */
U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ures_get_Locale_అంతర్గత(స్థిరరాశి UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_సంకేతం* స్థితి_2);

/**
 * Same as ures_openDirect() but uses the fill-in parameter instead of allocating a new bundle.
 * 
 * @param r The existing UResourceBundle to fill in. If NULL then status will be
 *          set to U_ILLEGAL_ARGUMENT_ERROR.
 * @param packageName   The packageName and locale together point to an ICU udata object,
 *                      as defined by <code> udata_open( packageName, "res", locale, err) </code>
 *                      or equivalent.  Typically, packageName will refer to a (.dat) file, or to
 *                      a package registered with udata_setAppData(). Using a full file or directory
 *                      pathname for packageName is deprecated. If NULL, ICU data will be used.
 * @param locale  specifies the locale for which we want to open the resource
 *                if NULL, the default locale will be used. If strlen(locale) == 0
 *                root locale will be used.
 * @param status The error code.
 * @see ures_openDirect
 * @internal
 */
U_CAPI శూన్య U_EXPORT2
ures_open_ప్రత్యక్ష_Fill_లోపల(UResourceBundle *r,
                      స్థిరరాశి అక్షరం *packageName,
                      స్థిరరాశి అక్షరం *locale,
                      UError_సంకేతం *స్థితి_2);

#అయితే_ముగింపు /*URESIMP_H*/
