// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2000-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ifndef URESIMP_H
#define URESIMP_H

#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "unicode/utypes.h"

#அந்தர்பாவ "uresdata.h"

#define k_வேர்_Locale_Name         "root"
#define kPoolBundleName         "pool"

/*
 The default minor version and the version separator must be exactly one
 character long.
*/

#define kDefaultMinorVersion    "0"
#define kVersionSeparator       "."
#define kVersionTag             "Version"

#define MAGIC1 19700503
#define MAGIC2 19641227

#define URES_MAX_ALIAS_LEVEL 256

#define EMPTY_SET 0x2205

கட்டமைப்பு UResourceDataEntry;
வகைவரை கட்டமைப்பு UResourceDataEntry UResourceDataEntry;

/*
 * Note: If we wanted to make this structure smaller, then we could try
 * to use one UResourceDataEntry pointer for fAlias and fPool, with a separate
 * flag to distinguish whether this struct is for a real bundle with a pool,
 * or for an alias entry for which we won't use the pool after loading.
 */
கட்டமைப்பு UResourceDataEntry {
    எழுத்து *fName; /* name of the locale for bundle - still to decide whether it is original or fallback */
    எழுத்து *fPath; /* path to bundle - used for distinguishing between resources with the same name */
    UResourceDataEntry *fParent; /*next resource in fallback chain*/
    UResourceDataEntry *fAlias;
    UResourceDataEntry *fPool;
    ResourceData fData; /* data for low level access */
    எழுத்து fNameBuffer[3]; /* A small buffer of free space for fName. The free space is due to struct padding. */
    uint32_t f_மொத்தம்_Existing; /* how much is this resource used */
    UErrorCode fBogus;
    /* int32_t fHashKey;*/ /* for faster access in the hashtable */
};

#define RES_BUFSIZE 64
#define RES_PATH_SEPARATOR   '/'
#define RES_PATH_SEPARATOR_S   "/"

U_CAPI வெறுமை U_EXPORT2 ures_init_அடுக்கு_Object(UResourceBundle* resB);

#ifdef __cplusplus

கட்டமைப்பு UResourceBundle {
    மாறிலி எழுத்து *f_சாவி; /*tag*/
    /**
     * The dataEntry for the actual locale in which this item lives.
     * Used for accessing the item's data.
     * Non-const pointer for reference counting via entryIncrease().
     */
    UResourceDataEntry *fData; /*for low-level access*/
    எழுத்து *fVersion;
    /**
     * The dataEntry for the valid locale.
     * Used for /LOCALE/path alias resolution that starts back from the valid locale,
     * rather than from the actual locale of this item which might live in
     * an ancestor bundle.
     */
    UResourceDataEntry *fValidLocaleDataEntry;
    எழுத்து *fResPath; /* full path to the resource: "zh_TW/CollationElements/Sequence" */
    எழுத்து fResBuf[RES_BUFSIZE];
    int32_t fResPathLen;
    Resource fRes;
    UBool fHasFallback;
    UBool f_Is_உச்சி_Level;
    uint32_t fMagic1;   /* For determining if it's a stack object */
    uint32_t fMagic2;   /* For determining if it's a stack object */
    int32_t fIndex;
    int32_t fSize;

    உள்வரி மாறிலி ResourceData &getResData() மாறிலி { திரும்பு fData->fData; }
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
class U_COMMON_API அடுக்கு_UResource_Bundle {
பொது:
    // No heap allocation. Use only on the stack.
    நிலையான வெறுமை* U_EXPORT2 operator new(size_t) noexcept = delete;
    நிலையான வெறுமை* U_EXPORT2 operator new[](size_t) noexcept = delete;
    நிலையான வெறுமை* U_EXPORT2 operator new(size_t, வெறுமை*) noexcept = delete;

    அடுக்கு_UResource_Bundle();
    ~அடுக்கு_UResource_Bundle();

    UResourceBundle* getAlias() U_LIFETIME_BOUND { திரும்பு &bundle; }

    UResourceBundle& ref() U_LIFETIME_BOUND { திரும்பு bundle; }
    மாறிலி UResourceBundle& ref() மாறிலி U_LIFETIME_BOUND { திரும்பு bundle; }

    அடுக்கு_UResource_Bundle(மாறிலி அடுக்கு_UResource_Bundle&) = delete;
    அடுக்கு_UResource_Bundle& operator=(மாறிலி அடுக்கு_UResource_Bundle&) = delete;

    அடுக்கு_UResource_Bundle(அடுக்கு_UResource_Bundle&&) = delete;
    அடுக்கு_UResource_Bundle& operator=(அடுக்கு_UResource_Bundle&&) = delete;

தனிப்பட்ட:
    UResourceBundle bundle;
};

U_NAMESPACE_END

#endif  /* __cplusplus */

/**
 * Opens a resource bundle for the locale;
 * if there is not even a base language bundle, then loads the root bundle;
 * never falls back to the default locale.
 *
 * This is used for algorithms that have good pan-Unicode default behavior,
 * such as case mappings, collation, and segmentation (BreakIterator).
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(மாறிலி எழுத்து* path, மாறிலி எழுத்து* localeID, UErrorCode* status);

/* Some getters used by the copy constructor */
U_CFUNC மாறிலி எழுத்து* ures_getName(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND);
#ifdef URES_DEBUG
U_CFUNC மாறிலி எழுத்து* ures_getPath(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND);
/**
 * If anything was in the RB cache, dump it to the screen.
 * @return true if there was anything into the cache
 */
U_CAPI UBool U_EXPORT2 ures_dumpCacheContents(வெறுமை);
#endif

/* Candidates for export */
U_CFUNC UResourceBundle*
ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
              மாறிலி UResourceBundle* original,
              UErrorCode* status);

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
ures_findResource(மாறிலி எழுத்து* pathToResource,
                  UResourceBundle* fillIn U_LIFETIME_BOUND,
                  UErrorCode* status);

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
ures_findSubResource(மாறிலி UResourceBundle* resB,
                     எழுத்து* pathToResource,
                     UResourceBundle* fillIn U_LIFETIME_BOUND,
                     UErrorCode* status);

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
ures_getFunctionalEquivalent(எழுத்து *result, int32_t resultCapacity, 
                             மாறிலி எழுத்து *path, மாறிலி எழுத்து *resName, மாறிலி எழுத்து *keyword, மாறிலி எழுத்து *locid,
                             UBool *isAvailable, UBool omitDefault, UErrorCode *status);

/**
 * Given a tree path and keyword, return a string enumeration of all possible values for that keyword.
 * @param path path to the tree, or NULL for ICU data
 * @param keyword a particular keyword to consider, must match a top level resource name 
 * within the tree.
 * @param status error code
 */
U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_மதிப்புகள்(மாறிலி எழுத்து *path, மாறிலி எழுத்து *keyword, UErrorCode *status);


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
ures_get_By_சாவி_With_Fallback(மாறிலி UResourceBundle* resB,
                          மாறிலி எழுத்து* in_சாவி,
                          UResourceBundle* fillIn U_LIFETIME_BOUND,
                          UErrorCode* status);

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
U_CAPI மாறிலி UChar* U_EXPORT2
ures_get_String_By_சாவி_With_Fallback(மாறிலி UResourceBundle* resB U_LIFETIME_BOUND,
                                மாறிலி எழுத்து* in_சாவி,
                                int32_t* len,
                                UErrorCode* status);

#ifdef __cplusplus

U_CAPI வெறுமை U_EXPORT2
ures_get_மதிப்பு_With_Fallback(மாறிலி UResourceBundle *bundle, மாறிலி எழுத்து *path,
                          UResourceBundle *tempFillIn,
                          icu::Resource_Data_மதிப்பு &மதிப்பு_2, UErrorCode &errorCode);

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
U_CAPI வெறுமை U_EXPORT2
ures_get_All_பொருட்கள்_With_Fallback(மாறிலி UResourceBundle *bundle, மாறிலி எழுத்து *path,
                             icu::ResourceSink &sink, UErrorCode &errorCode);

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
U_CAPI வெறுமை U_EXPORT2
ures_getAllChildrenWithFallback(மாறிலி UResourceBundle *bundle, மாறிலி எழுத்து *path,
                                icu::ResourceSink &sink, UErrorCode &errorCode);

#endif  /* __cplusplus */

/**
 * Get a version number by key
 * @param resB bundle containing version number
 * @param key the key for the version number
 * @param ver fillin for the version number
 * @param status error code
 */
U_CAPI வெறுமை U_EXPORT2
ures_get_Version_By_சாவி(மாறிலி UResourceBundle *resB,
                     மாறிலி எழுத்து *சாவி,
                     UVersionInfo ver,
                     UErrorCode *status);


/**
 * Internal function.
 * Return the version number associated with this ResourceBundle as a string.
 *
 * @param resourceBundle The resource bundle for which the version is checked.
 * @return  A version number string as specified in the resource bundle or its parent.
 *          The caller does not own this string.
 * @see ures_getVersion
 */
U_CAPI மாறிலி எழுத்து* U_EXPORT2
ures_get_Version_எண்_Internal(மாறிலி UResourceBundle* resourceBundle U_LIFETIME_BOUND);

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
U_CAPI மாறிலி எழுத்து* U_EXPORT2
ures_getLocaleInternal(மாறிலி UResourceBundle* resourceBundle U_LIFETIME_BOUND, UErrorCode* status);

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
U_CAPI வெறுமை U_EXPORT2
ures_openDirectFillIn(UResourceBundle *r,
                      மாறிலி எழுத்து *packageName,
                      மாறிலி எழுத்து *locale,
                      UErrorCode *status);

#endif /*URESIMP_H*/
