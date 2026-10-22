// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2000-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#جيڪڏهن_وصف_نه_ٿيل URESIMP_H
#وصف_ڪريو URESIMP_H

#ضمن "unicode/ures.h"
#ضمن "unicode/utypes.h"

#ضمن "uresdata.h"

#وصف_ڪريو k_پاڙ_Locale_Name         "root"
#وصف_ڪريو kPoolBundleName         "pool"

/*
 The default minor version and the version separator must be exactly one
 character long.
*/

#وصف_ڪريو k_Default_Minor_نسخو    "0"
#وصف_ڪريو k_نسخو_Separator       "."
#وصف_ڪريو k_نسخو_ٽيگ             "Version"

#وصف_ڪريو MAGIC1 19700503
#وصف_ڪريو MAGIC2 19641227

#وصف_ڪريو URES_MAX_ALIAS_LEVEL 256

#وصف_ڪريو خالي_جايون_سيٽ 0x2205

بنية UResource_معلومات_داخلا;
تعريفنوع بنية UResource_معلومات_داخلا UResource_معلومات_داخلا;

/*
 * Note: If we wanted to make this structure smaller, then we could try
 * to use one UResourceDataEntry pointer for fAlias and fPool, with a separate
 * flag to distinguish whether this struct is for a real bundle with a pool,
 * or for an alias entry for which we won't use the pool after loading.
 */
بنية UResource_معلومات_داخلا {
    حرف *fName; /* name of the locale for bundle - still to decide whether it is original or fallback */
    حرف *f_رستو; /* path to bundle - used for distinguishing between resources with the same name */
    UResource_معلومات_داخلا *f_والد; /*next resource in fallback chain*/
    UResource_معلومات_داخلا *fAlias;
    UResource_معلومات_داخلا *fPool;
    Resource_معلومات f_معلومات; /* data for low level access */
    حرف f_Name_بفر[3]; /* A small buffer of free space for fName. The free space is due to struct padding. */
    uint32_t f_ڳڻپ_Existing; /* how much is this resource used */
    UError_ڪوڊ fBogus;
    /* int32_t fHashKey;*/ /* for faster access in the hashtable */
};

#وصف_ڪريو RES_BUFSIZE 64
#وصف_ڪريو RES_رستو_SEPARATOR   '/'
#وصف_ڪريو RES_رستو_SEPARATOR_S   "/"

U_CAPI فراغ U_EXPORT2 ures_init_ڍير_Object(UResourceBundle* resB);

#جيڪڏهن_وصف_ٿيل __cplusplus

بنية UResourceBundle {
    ثابت حرف *f_ڪنجي; /*ٽيگ*/
    /**
     * The dataEntry for the actual locale in which this item lives.
     * Used for accessing the item's data.
     * Non-const pointer for reference counting via entryIncrease().
     */
    UResource_معلومات_داخلا *f_معلومات; /*for low-level access*/
    حرف *f_نسخو;
    /**
     * The dataEntry for the valid locale.
     * Used for /LOCALE/path alias resolution that starts back from the valid locale,
     * rather than from the actual locale of this item which might live in
     * an ancestor bundle.
     */
    UResource_معلومات_داخلا *f_Valid_Locale_معلومات_داخلا;
    حرف *f_Res_رستو; /* full path to the resource: "zh_TW/CollationElements/Sequence" */
    حرف fResBuf[RES_BUFSIZE];
    int32_t f_Res_رستو_Len;
    Resource fRes;
    UBool fHasFallback;
    UBool f_Is_چوٽي_Level;
    uint32_t fMagic1;   /* For determining if it's a stack object */
    uint32_t fMagic2;   /* For determining if it's a stack object */
    int32_t f_اشاريو;
    int32_t f_سائيز;

    مضمن ثابت Resource_معلومات &get_Res_معلومات() ثابت { أرجع f_معلومات->f_معلومات; }
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
class U_COMMON_API ڍير_UResource_Bundle {
عام:
    // No heap allocation. Use only on the stack.
    ساكن فراغ* U_EXPORT2 عامل new(size_t) noexcept = delete;
    ساكن فراغ* U_EXPORT2 عامل new[](size_t) noexcept = delete;
    ساكن فراغ* U_EXPORT2 عامل new(size_t, فراغ*) noexcept = delete;

    ڍير_UResource_Bundle();
    ~ڍير_UResource_Bundle();

    UResourceBundle* getAlias() U_LIFETIME_BOUND { أرجع &bundle; }

    UResourceBundle& ref() U_LIFETIME_BOUND { أرجع bundle; }
    ثابت UResourceBundle& ref() ثابت U_LIFETIME_BOUND { أرجع bundle; }

    ڍير_UResource_Bundle(ثابت ڍير_UResource_Bundle&) = delete;
    ڍير_UResource_Bundle& عامل=(ثابت ڍير_UResource_Bundle&) = delete;

    ڍير_UResource_Bundle(ڍير_UResource_Bundle&&) = delete;
    ڍير_UResource_Bundle& عامل=(ڍير_UResource_Bundle&&) = delete;

خاص:
    UResourceBundle bundle;
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو  /* __cplusplus */

/**
 * Opens a resource bundle for the locale;
 * if there is not even a base language bundle, then loads the root bundle;
 * never falls back to the default locale.
 *
 * This is used for algorithms that have good pan-Unicode default behavior,
 * such as case mappings, collation, and segmentation (BreakIterator).
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ثابت حرف* رستو, ثابت حرف* localeID, UError_ڪوڊ* حالت);

/* Some getters used by the copy constructor */
U_CFUNC ثابت حرف* ures_getName(ثابت UResourceBundle* resB U_LIFETIME_BOUND);
#جيڪڏهن_وصف_ٿيل URES_DEBUG
U_CFUNC ثابت حرف* ures_get_رستو(ثابت UResourceBundle* resB U_LIFETIME_BOUND);
/**
 * If anything was in the RB cache, dump it to the screen.
 * @return true if there was anything into the cache
 */
U_CAPI UBool U_EXPORT2 ures_dumpCacheContents(فراغ);
#جيڪڏهن_پورو

/* Candidates for export */
U_CFUNC UResourceBundle*
ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
              ثابت UResourceBundle* original,
              UError_ڪوڊ* حالت);

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
ures_ڳوليو_Resource(ثابت حرف* رستو_منزل_Resource,
                  UResourceBundle* fill_اندريون U_LIFETIME_BOUND,
                  UError_ڪوڊ* حالت);

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
ures_ڳوليو_Sub_Resource(ثابت UResourceBundle* resB,
                     حرف* رستو_منزل_Resource,
                     UResourceBundle* fill_اندريون U_LIFETIME_BOUND,
                     UError_ڪوڊ* حالت);

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
ures_getFunctionalEquivalent(حرف *نتيجو, int32_t نتيجو_صلاحيت, 
                             ثابت حرف *رستو, ثابت حرف *resName, ثابت حرف *keyword, ثابت حرف *locid,
                             UBool *is_موجود, UBool omitDefault, UError_ڪوڊ *حالت);

/**
 * Given a tree path and keyword, return a string enumeration of all possible values for that keyword.
 * @param path path to the tree, or NULL for ICU data
 * @param keyword a particular keyword to consider, must match a top level resource name 
 * within the tree.
 * @param status error code
 */
U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_قدرون(ثابت حرف *رستو, ثابت حرف *keyword, UError_ڪوڊ *حالت);


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
ures_get_By_ڪنجي_With_Fallback(ثابت UResourceBundle* resB,
                          ثابت حرف* اندريون_ڪنجي,
                          UResourceBundle* fill_اندريون U_LIFETIME_BOUND,
                          UError_ڪوڊ* حالت);

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
U_CAPI ثابت UChar* U_EXPORT2
ures_get_String_By_ڪنجي_With_Fallback(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                ثابت حرف* اندريون_ڪنجي,
                                int32_t* len,
                                UError_ڪوڊ* حالت);

#جيڪڏهن_وصف_ٿيل __cplusplus

U_CAPI فراغ U_EXPORT2
ures_get_قدر_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *رستو,
                          UResourceBundle *عارضي_Fill_اندريون,
                          icu::Resource_معلومات_قدر &قدر_2, UError_ڪوڊ &error_ڪوڊ);

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
U_CAPI فراغ U_EXPORT2
ures_get_All_شيون_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *رستو,
                             icu::ResourceSink &sink, UError_ڪوڊ &error_ڪوڊ);

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
U_CAPI فراغ U_EXPORT2
ures_getAllChildrenWithFallback(ثابت UResourceBundle *bundle, ثابت حرف *رستو,
                                icu::ResourceSink &sink, UError_ڪوڊ &error_ڪوڊ);

#جيڪڏهن_پورو  /* __cplusplus */

/**
 * Get a version number by key
 * @param resB bundle containing version number
 * @param key the key for the version number
 * @param ver fillin for the version number
 * @param status error code
 */
U_CAPI فراغ U_EXPORT2
ures_get_نسخو_By_ڪنجي(ثابت UResourceBundle *resB,
                     ثابت حرف *ڪنجي,
                     UVersionInfo ver,
                     UError_ڪوڊ *حالت);


/**
 * Internal function.
 * Return the version number associated with this ResourceBundle as a string.
 *
 * @param resourceBundle The resource bundle for which the version is checked.
 * @return  A version number string as specified in the resource bundle or its parent.
 *          The caller does not own this string.
 * @see ures_getVersion
 */
U_CAPI ثابت حرف* U_EXPORT2
ures_get_نسخو_عدد_اندروني(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND);

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
U_CAPI ثابت حرف* U_EXPORT2
ures_get_Locale_اندروني(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_ڪوڊ* حالت);

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
U_CAPI فراغ U_EXPORT2
ures_open_سڌو_Fill_اندريون(UResourceBundle *r,
                      ثابت حرف *packageName,
                      ثابت حرف *locale,
                      UError_ڪوڊ *حالت);

#جيڪڏهن_پورو /*URESIMP_H*/
