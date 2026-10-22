// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1997-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*
* File URES.H (formerly CRESBUND.H)
*
* Modification History:
*
*   Date        Name        Description
*   04/01/97    aliu        Creation.
*   02/22/99    damiba      overhaul.
*   04/04/99    helena      Fixed internal header inclusion.
*   04/15/99    Madhu       Updated Javadoc
*   06/14/99    stephen     Removed functions taking a filename suffix.
*   07/20/99    stephen     Language-independent typedef to void*
*   11/09/99    weiv        Added ures_getLocale()
*   06/24/02    weiv        Added support for resource sharing
******************************************************************************
*/

#यदि_अपरिभाषितम् URES_H
#परिभाषय URES_H

#अन्तर्भाव "unicode/char16ptr.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uloc.h"

#यदि U_SHOW_CPLUSPLUS_API
#अन्तर्भाव "unicode/localpointer.h"
#यदि_अन्तः   // U_SHOW_CPLUSPLUS_API

/**
 * \file
 * \brief C API: Resource Bundle
 *
 * <h2>C API: Resource Bundle</h2>
 *
 * C API representing a collection of resource information pertaining to a given
 * locale. A resource bundle provides a way of accessing locale- specific information in
 * a data file. You create a resource bundle that manages the resources for a given
 * locale and then ask it for individual resources.
 * <P>
 * Resource bundles in ICU4C are currently defined using text files which conform to the following
 * <a href="https://github.com/unicode-org/icu-docs/blob/main/design/bnf_rb.txt">BNF definition</a>.
 * More on resource bundle concepts and syntax can be found in the
 * <a href="https://unicode-org.github.io/icu/userguide/locale/resources">Users Guide</a>.
 * <P>
 */

/**
 * UResourceBundle is an opaque type for handles for resource bundles in C APIs.
 * @stable ICU 2.0
 */
संरचना UResourceBundle;

/**
 * @stable ICU 2.0
 */
प्रकारनिर्देश संरचना UResourceBundle UResourceBundle;

/**
 * Numeric constants for types of resource items.
 * @see ures_getType
 * @stable ICU 2.0
 */
प्रकारनिर्देश गणना {
    /** Resource type constant for "no resource". @stable ICU 2.6 */
    URES_NONE=-1,

    /** Resource type constant for 16-bit Unicode strings. @stable ICU 2.6 */
    URES_STRING=0,

    /** Resource type constant for binary data. @stable ICU 2.6 */
    URES_द्विमान=1,

    /** Resource type constant for tables of key-value pairs. @stable ICU 2.6 */
    URES_सारणी=2,

    /**
     * Resource type constant for aliases;
     * internally stores a string which identifies the actual resource
     * storing the data (can be in a different resource bundle).
     * Resolved internally before delivering the actual resource through the API.
     * @stable ICU 2.6
     */
    URES_ALIAS=3,

    /**
     * Resource type constant for a single 28-bit integer, interpreted as
     * signed or unsigned by the ures_getInt() or ures_getUInt() function.
     * @see ures_getInt
     * @see ures_getUInt
     * @stable ICU 2.6
     */
    URES_INT=7,

    /** Resource type constant for arrays of resources. @stable ICU 2.6 */
    URES_ARRAY=8,

    /**
     * Resource type constant for vectors of 32-bit integers.
     * @see ures_getIntVector
     * @stable ICU 2.6
     */
    URES_INT_सदिशम् = 14,
#यदि_अपरिभाषितम् U_HIDE_DEPRECATED_API
    /** @deprecated ICU 2.6 Use the URES_ constant instead. */
    RES_NONE=URES_NONE,
    /** @deprecated ICU 2.6 Use the URES_ constant instead. */
    RES_STRING=URES_STRING,
    /** @deprecated ICU 2.6 Use the URES_ constant instead. */
    RES_द्विमान=URES_द्विमान,
    /** @deprecated ICU 2.6 Use the URES_ constant instead. */
    RES_सारणी=URES_सारणी,
    /** @deprecated ICU 2.6 Use the URES_ constant instead. */
    RES_ALIAS=URES_ALIAS,
    /** @deprecated ICU 2.6 Use the URES_ constant instead. */
    RES_INT=URES_INT,
    /** @deprecated ICU 2.6 Use the URES_ constant instead. */
    RES_ARRAY=URES_ARRAY,
    /** @deprecated ICU 2.6 Use the URES_ constant instead. */
    RES_INT_सदिशम्=URES_INT_सदिशम्,
    /** @deprecated ICU 2.6 Not used. */
    RES_RESERVED=15,

    /**
     * One more than the highest normal UResType value.
     * @deprecated ICU 58 The numeric value may change over time, see ICU ticket #12420.
     */
    URES_सीमा = 16
#यदि_अन्तः  // U_HIDE_DEPRECATED_API
} UResType;

/*
 * Functions to create and destroy resource bundles.
 */

/**
 * Opens a UResourceBundle, from which users can extract strings by using
 * their corresponding keys.
 * Note that the caller is responsible of calling <TT>ures_close</TT> on each successfully
 * opened resource bundle.
 * @param packageName   The packageName and locale together point to an ICU udata object,
 *                      as defined by <code> udata_open( packageName, "res", locale, err) </code>
 *                      or equivalent.  Typically, packageName will refer to a (.dat) file, or to
 *                      a package registered with udata_setAppData(). Using a full file or directory
 *                      pathname for packageName is deprecated. If NULL, ICU data will be used.
 * @param locale  specifies the locale for which we want to open the resource
 *                if NULL, the default locale will be used. If strlen(locale) == 0
 *                root locale will be used.
 *
 * @param status  fills in the outgoing error code.
 * The UErrorCode err parameter is used to return status information to the user. To
 * check whether the construction succeeded or not, you should check the value of
 * U_SUCCESS(err). If you wish more detailed information, you can check for
 * informational status results which still indicate success. U_USING_FALLBACK_WARNING
 * indicates that a fall back locale was used. For example, 'de_CH' was requested,
 * but nothing was found there, so 'de' was used. U_USING_DEFAULT_WARNING indicates that
 * the default locale data or root locale data was used; neither the requested locale
 * nor any of its fall back locales could be found. Please see the users guide for more
 * information on this topic.
 * @return      a newly allocated resource bundle.
 * @see ures_close
 * @stable ICU 2.0
 */
U_CAPI UResourceBundle*  U_EXPORT2
ures_open(ध्रुव वर्ण*    packageName,
          ध्रुव वर्ण*  locale,
          UError_संहिता*     स्थितिः);


/** This function does not care what kind of localeID is passed in. It simply opens a bundle with
 *  that name. Fallback mechanism is disabled for the new bundle. If the requested bundle contains
 *  an %%ALIAS directive, the results are undefined.
 * @param packageName   The packageName and locale together point to an ICU udata object,
 *                      as defined by <code> udata_open( packageName, "res", locale, err) </code>
 *                      or equivalent.  Typically, packageName will refer to a (.dat) file, or to
 *                      a package registered with udata_setAppData(). Using a full file or directory
 *                      pathname for packageName is deprecated. If NULL, ICU data will be used.
 * @param locale  specifies the locale for which we want to open the resource
 *                if NULL, the default locale will be used. If strlen(locale) == 0
 *                root locale will be used.
 *
 * @param status fills in the outgoing error code. Either U_ZERO_ERROR or U_MISSING_RESOURCE_ERROR
 * @return      a newly allocated resource bundle or NULL if it doesn't exist.
 * @see ures_close
 * @stable ICU 2.0
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_open_प्रत्यक्षम्(ध्रुव वर्ण* packageName,
                ध्रुव वर्ण* locale,
                UError_संहिता* स्थितिः);

/**
 * Same as ures_open() but takes a const UChar *path.
 * This path will be converted to char * using the default converter,
 * then ures_open() is called.
 *
 * @param packageName   The packageName and locale together point to an ICU udata object,
 *                      as defined by <code> udata_open( packageName, "res", locale, err) </code>
 *                      or equivalent.  Typically, packageName will refer to a (.dat) file, or to
 *                      a package registered with udata_setAppData(). Using a full file or directory
 *                      pathname for packageName is deprecated. If NULL, ICU data will be used.
 * @param locale  specifies the locale for which we want to open the resource
 *                if NULL, the default locale will be used. If strlen(locale) == 0
 *                root locale will be used.
 * @param status  fills in the outgoing error code.
 * @return      a newly allocated resource bundle.
 * @see ures_open
 * @stable ICU 2.0
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_openU(ध्रुव UChar* packageName,
           ध्रुव वर्ण* locale,
           UError_संहिता* स्थितिः);

#यदि_अपरिभाषितम् U_HIDE_DEPRECATED_API
/**
 * Returns the number of strings/arrays in resource bundles.
 * Better to use ures_getSize, as this function will be deprecated.
 *
 *@param resourceBundle resource bundle containing the desired strings
 *@param resourceKey key tagging the resource
 *@param err fills in the outgoing error code
 *                could be <TT>U_MISSING_RESOURCE_ERROR</TT> if the key is not found
 *                could be a non-failing error
 *                e.g.: <TT>U_USING_FALLBACK_WARNING</TT>,<TT>U_USING_FALLBACK_WARNING </TT>
 *@return: for    <STRONG>Arrays</STRONG>: returns the number of resources in the array
 *                <STRONG>Tables</STRONG>: returns the number of resources in the table
 *                <STRONG>single string</STRONG>: returns 1
 *@see ures_getSize
 * @deprecated ICU 2.8 User ures_getSize instead
 */
U_DEPRECATED int32_t U_EXPORT2
ures_गणनम्_Array_वस्तूनि(ध्रुव UResourceBundle* resourceBundle,
                     ध्रुव वर्ण* resource_कुञ्चिका,
                     UError_संहिता* err);
#यदि_अन्तः  /* U_HIDE_DEPRECATED_API */

/**
 * Close a resource bundle, all pointers returned from the various ures_getXXX calls
 * on this particular bundle should be considered invalid henceforth.
 *
 * @param resourceBundle a pointer to a resourceBundle struct. Can be NULL.
 * @see ures_open
 * @stable ICU 2.0
 */
U_CAPI शून्य U_EXPORT2
ures_close(UResourceBundle* resourceBundle);

#यदि U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * \class LocalUResourceBundlePointer
 * "Smart pointer" class, closes a UResourceBundle via ures_close().
 * For most methods see the LocalPointerBase base class.
 *
 * @see LocalPointerBase
 * @see LocalPointer
 * @stable ICU 4.4
 */
U_DEFINE_LOCAL_OPEN_POINTER(LocalUResourceBundlePointer, UResourceBundle, ures_close);

U_NAMESPACE_अन्तः

#यदि_अन्तः

#यदि_अपरिभाषितम् U_HIDE_DEPRECATED_API
/**
 * Return the version number associated with this ResourceBundle as a string. Please
 * use ures_getVersion as this function is going to be deprecated.
 *
 * @param resourceBundle The resource bundle for which the version is checked.
 * @return  A version number string as specified in the resource bundle or its parent.
 *          The caller does not own this string.
 * @see ures_getVersion
 * @deprecated ICU 2.8 Use ures_getVersion instead.
 */
U_DEPRECATED ध्रुव वर्ण* U_EXPORT2
ures_get_संस्करणम्_सङ्ख्या(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND);
#यदि_अन्तः  /* U_HIDE_DEPRECATED_API */

/**
 * Return the version number associated with this ResourceBundle as an
 * UVersionInfo array.
 *
 * @param resB The resource bundle for which the version is checked.
 * @param versionInfo A UVersionInfo array that is filled with the version number
 *                    as specified in the resource bundle or its parent.
 * @stable ICU 2.0
 */
U_CAPI शून्य U_EXPORT2
ures_get_संस्करणम्(ध्रुव UResourceBundle* resB,
                UVersionInfo संस्करणम्_Info);

#यदि_अपरिभाषितम् U_HIDE_DEPRECATED_API
/**
 * Return the name of the Locale associated with this ResourceBundle. This API allows
 * you to query for the real locale of the resource. For example, if you requested
 * "en_US_CALIFORNIA" and only "en_US" bundle exists, "en_US" will be returned.
 * For subresources, the locale where this resource comes from will be returned.
 * If fallback has occurred, getLocale will reflect this.
 *
 * @param resourceBundle resource bundle in question
 * @param status just for catching illegal arguments
 * @return  A Locale name
 * @deprecated ICU 2.8 Use ures_getLocaleByType instead.
 */
U_DEPRECATED ध्रुव वर्ण* U_EXPORT2
ures_getLocale(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_संहिता* स्थितिः);
#यदि_अन्तः  /* U_HIDE_DEPRECATED_API */

/**
 * Return the name of the Locale associated with this ResourceBundle.
 * You can choose between requested, valid and real locale.
 *
 * @param resourceBundle resource bundle in question
 * @param type You can choose between requested, valid and actual
 *             locale. For description see the definition of
 *             ULocDataLocaleType in uloc.h
 * @param status just for catching illegal arguments
 * @return  A Locale name
 * @stable ICU 2.8
 */
U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_getLocaleByType(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                     ULoc_दत्त_Locale_Type type,
                     UError_संहिता* स्थितिः);


#यदि_अपरिभाषितम् U_HIDE_आन्तरिकम्_API
/**
 * Same as ures_open() but uses the fill-in parameter instead of allocating a new bundle.
 *
 * TODO need to revisit usefulness of this function
 *      and usage model for fillIn parameters without knowing sizeof(UResourceBundle)
 * @param r The existing UResourceBundle to fill in. If NULL then status will be
 *               set to U_ILLEGAL_ARGUMENT_ERROR.
 * @param packageName   The packageName and locale together point to an ICU udata object,
 *                      as defined by <code> udata_open( packageName, "res", locale, err) </code>
 *                      or equivalent.  Typically, packageName will refer to a (.dat) file, or to
 *                      a package registered with udata_setAppData(). Using a full file or directory
 *                      pathname for packageName is deprecated. If NULL, ICU data will be used.
 * @param localeID specifies the locale for which we want to open the resource
 * @param status The error code.
 * @internal
 */
U_CAPI शून्य U_EXPORT2
ures_open_Fill_अन्तर्(UResourceBundle *r,
                ध्रुव वर्ण* packageName,
                ध्रुव वर्ण* localeID,
                UError_संहिता* स्थितिः);
#यदि_अन्तः  /* U_HIDE_INTERNAL_API */

/**
 * Returns a string from a string resource type
 *
 * @param resourceBundle a string resource
 * @param len    fills in the length of resulting string
 * @param status fills in the outgoing error code
 *                could be <TT>U_MISSING_RESOURCE_ERROR</TT> if the key is not found
 *                Always check the value of status. Don't count on returning NULL.
 *                could be a non-failing error
 *                e.g.: <TT>U_USING_FALLBACK_WARNING</TT>,<TT>U_USING_DEFAULT_WARNING </TT>
 * @return a pointer to a zero-terminated UChar array which lives in a memory mapped/DLL file.
 * @see ures_getBinary
 * @see ures_getIntVector
 * @see ures_getInt
 * @see ures_getUInt
 * @stable ICU 2.0
 */
U_CAPI ध्रुव UChar* U_EXPORT2
ures_getString(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND,
               int32_t* len,
               UError_संहिता* स्थितिः);

/**
 * Returns a UTF-8 string from a string resource.
 * The UTF-8 string may be returnable directly as a pointer, or
 * it may need to be copied, or transformed from UTF-16 using u_strToUTF8()
 * or equivalent.
 *
 * If forceCopy==true, then the string is always written to the dest buffer
 * and dest is returned.
 *
 * If forceCopy==false, then the string is returned as a pointer if possible,
 * without needing a dest buffer (it can be NULL). If the string needs to be
 * copied or transformed, then it may be placed into dest at an arbitrary offset.
 *
 * If the string is to be written to dest, then U_BUFFER_OVERFLOW_ERROR and
 * U_STRING_NOT_TERMINATED_WARNING are set if appropriate, as usual.
 *
 * If the string is transformed from UTF-16, then a conversion error may occur
 * if an unpaired surrogate is encountered. If the function is successful, then
 * the output UTF-8 string is always well-formed.
 *
 * @param resB Resource bundle.
 * @param dest Destination buffer. Can be NULL only if capacity=*length==0.
 * @param length Input: Capacity of destination buffer.
 *               Output: Actual length of the UTF-8 string, not counting the
 *               terminating NUL, even in case of U_BUFFER_OVERFLOW_ERROR.
 *               Can be NULL, meaning capacity=0 and the string length is not
 *               returned to the caller.
 * @param forceCopy If true, then the output string will always be written to
 *                  dest, with U_BUFFER_OVERFLOW_ERROR and
 *                  U_STRING_NOT_TERMINATED_WARNING set if appropriate.
 *                  If false, then the dest buffer may or may not contain a
 *                  copy of the string. dest may or may not be modified.
 *                  If a copy needs to be written, then the UErrorCode parameter
 *                  indicates overflow etc. as usual.
 * @param status Pointer to a standard ICU error code. Its input value must
 *               pass the U_SUCCESS() test, or else the function returns
 *               immediately. Check for U_FAILURE() on output or use with
 *               function chaining. (See User Guide for details.)
 * @return The pointer to the UTF-8 string. It may be dest, or at some offset
 *         from dest (only if !forceCopy), or in unrelated memory.
 *         Always NUL-terminated unless the string was written to dest and
 *         length==capacity (in which case U_STRING_NOT_TERMINATED_WARNING is set).
 *
 * @see ures_getString
 * @see u_strToUTF8
 * @stable ICU 3.6
 */
U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_getUTF8String(ध्रुव UResourceBundle* resB,
                   वर्ण* dest U_LIFETIME_BOUND,
                   int32_t* दीर्घता,
                   UBool forceCopy,
                   UError_संहिता* स्थितिः);

/**
 * Returns a binary data from a binary resource.
 *
 * @param resourceBundle a string resource
 * @param len    fills in the length of resulting byte chunk
 * @param status fills in the outgoing error code
 *                could be <TT>U_MISSING_RESOURCE_ERROR</TT> if the key is not found
 *                Always check the value of status. Don't count on returning NULL.
 *                could be a non-failing error
 *                e.g.: <TT>U_USING_FALLBACK_WARNING</TT>,<TT>U_USING_DEFAULT_WARNING </TT>
 * @return a pointer to a chunk of unsigned bytes which live in a memory mapped/DLL file.
 * @see ures_getString
 * @see ures_getIntVector
 * @see ures_getInt
 * @see ures_getUInt
 * @stable ICU 2.0
 */
U_CAPI ध्रुव uint8_t* U_EXPORT2
ures_get_द्विमान(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND,
               int32_t* len,
               UError_संहिता* स्थितिः);

/**
 * Returns a 32 bit integer array from a resource.
 *
 * @param resourceBundle an int vector resource
 * @param len    fills in the length of resulting byte chunk
 * @param status fills in the outgoing error code
 *                could be <TT>U_MISSING_RESOURCE_ERROR</TT> if the key is not found
 *                Always check the value of status. Don't count on returning NULL.
 *                could be a non-failing error
 *                e.g.: <TT>U_USING_FALLBACK_WARNING</TT>,<TT>U_USING_DEFAULT_WARNING </TT>
 * @return a pointer to a chunk of integers which live in a memory mapped/DLL file.
 * @see ures_getBinary
 * @see ures_getString
 * @see ures_getInt
 * @see ures_getUInt
 * @stable ICU 2.0
 */
U_CAPI ध्रुव int32_t* U_EXPORT2
ures_get_Int_सदिशम्(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                  int32_t* len,
                  UError_संहिता* स्थितिः);

/**
 * Returns an unsigned integer from a resource.
 * This integer is originally 28 bits.
 *
 * @param resourceBundle a string resource
 * @param status fills in the outgoing error code
 *                could be <TT>U_MISSING_RESOURCE_ERROR</TT> if the key is not found
 *                could be a non-failing error
 *                e.g.: <TT>U_USING_FALLBACK_WARNING</TT>,<TT>U_USING_DEFAULT_WARNING </TT>
 * @return an integer value
 * @see ures_getInt
 * @see ures_getIntVector
 * @see ures_getBinary
 * @see ures_getString
 * @stable ICU 2.0
 */
U_CAPI uint32_t U_EXPORT2
ures_getUInt(ध्रुव UResourceBundle* resourceBundle,
             UError_संहिता *स्थितिः);

/**
 * Returns a signed integer from a resource.
 * This integer is originally 28 bit and the sign gets propagated.
 *
 * @param resourceBundle a string resource
 * @param status  fills in the outgoing error code
 *                could be <TT>U_MISSING_RESOURCE_ERROR</TT> if the key is not found
 *                could be a non-failing error
 *                e.g.: <TT>U_USING_FALLBACK_WARNING</TT>,<TT>U_USING_DEFAULT_WARNING </TT>
 * @return an integer value
 * @see ures_getUInt
 * @see ures_getIntVector
 * @see ures_getBinary
 * @see ures_getString
 * @stable ICU 2.0
 */
U_CAPI int32_t U_EXPORT2
ures_getInt(ध्रुव UResourceBundle* resourceBundle,
            UError_संहिता *स्थितिः);

/**
 * Returns the size of a resource. Size for scalar types is always 1,
 * and for vector/table types is the number of child resources.
 * @warning Integer array is treated as a scalar type. There are no
 *          APIs to access individual members of an integer array. It
 *          is always returned as a whole.
 * @param resourceBundle a resource
 * @return number of resources in a given resource.
 * @stable ICU 2.0
 */
U_CAPI int32_t U_EXPORT2
ures_get_आकार(ध्रुव UResourceBundle *resourceBundle);

/**
 * Returns the type of a resource. Available types are defined in enum UResType
 *
 * @param resourceBundle a resource
 * @return type of the given resource.
 * @see UResType
 * @stable ICU 2.0
 */
U_CAPI UResType U_EXPORT2
ures_getType(ध्रुव UResourceBundle *resourceBundle);

/**
 * Returns the key associated with a given resource. Not all the resources have a key - only
 * those that are members of a table.
 *
 * @param resourceBundle a resource
 * @return a key associated to this resource, or NULL if it doesn't have a key
 * @stable ICU 2.0
 */
U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_कुञ्चिका(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND);

/* ITERATION API
    This API provides means for iterating through a resource
*/

/**
 * Resets the internal context of a resource so that iteration starts from the first element.
 *
 * @param resourceBundle a resource
 * @stable ICU 2.0
 */
U_CAPI शून्य U_EXPORT2
ures_resetIterator(UResourceBundle *resourceBundle);

/**
 * Checks whether the given resource has another element to iterate over.
 *
 * @param resourceBundle a resource
 * @return true if there are more elements, false if there is no more elements
 * @stable ICU 2.0
 */
U_CAPI UBool U_EXPORT2
ures_has_अग्र(ध्रुव UResourceBundle *resourceBundle);

/**
 * Returns the next resource in a given resource or NULL if there are no more resources
 * to iterate over. Features a fill-in parameter.
 *
 * @param resourceBundle    a resource
 * @param fillIn            if NULL a new UResourceBundle struct is allocated and must be closed by the caller.
 *                          Alternatively, you can supply a struct to be filled by this function.
 * @param status            fills in the outgoing error code. You may still get a non NULL result even if an
 *                          error occurred. Check status instead.
 * @return                  a pointer to a UResourceBundle struct. If fill in param was NULL, caller must close it
 * @stable ICU 2.0
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_get_अग्र_Resource(UResourceBundle* resourceBundle,
                     UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND,
                     UError_संहिता* स्थितिः);

/**
 * Returns the next string in a given resource or NULL if there are no more resources
 * to iterate over.
 *
 * @param resourceBundle    a resource
 * @param len               fill in length of the string
 * @param key               fill in for key associated with this string. NULL if no key
 * @param status            fills in the outgoing error code. If an error occurred, we may return NULL, but don't
 *                          count on it. Check status instead!
 * @return a pointer to a zero-terminated UChar array which lives in a memory mapped/DLL file.
 * @stable ICU 2.0
 */
U_CAPI ध्रुव UChar* U_EXPORT2
ures_get_अग्र_String(UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                   int32_t* len,
                   ध्रुव वर्ण** कुञ्चिका,
                   UError_संहिता* स्थितिः);

/**
 * Returns the resource in a given resource at the specified index. Features a fill-in parameter.
 *
 * @param resourceBundle    the resource bundle from which to get a sub-resource
 * @param indexR            an index to the wanted resource.
 * @param fillIn            if NULL a new UResourceBundle struct is allocated and must be closed by the caller.
 *                          Alternatively, you can supply a struct to be filled by this function.
 * @param status            fills in the outgoing error code. Don't count on NULL being returned if an error has
 *                          occurred. Check status instead.
 * @return                  a pointer to a UResourceBundle struct. If fill in param was NULL, caller must close it
 * @stable ICU 2.0
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_सूचकांक(ध्रुव UResourceBundle* resourceBundle,
                int32_t सूचकांक_R,
                UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND,
                UError_संहिता* स्थितिः);

/**
 * Returns the string in a given resource at the specified index.
 *
 * @param resourceBundle    a resource
 * @param indexS            an index to the wanted string.
 * @param len               fill in length of the string
 * @param status            fills in the outgoing error code. If an error occurred, we may return NULL, but don't
 *                          count on it. Check status instead!
 * @return                  a pointer to a zero-terminated UChar array which lives in a memory mapped/DLL file.
 * @stable ICU 2.0
 */
U_CAPI ध्रुव UChar* U_EXPORT2
ures_get_String_By_सूचकांक(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND,
                      int32_t सूचकांक_S,
                      int32_t* len,
                      UError_संहिता* स्थितिः);

/**
 * Returns a UTF-8 string from a resource at the specified index.
 * The UTF-8 string may be returnable directly as a pointer, or
 * it may need to be copied, or transformed from UTF-16 using u_strToUTF8()
 * or equivalent.
 *
 * If forceCopy==true, then the string is always written to the dest buffer
 * and dest is returned.
 *
 * If forceCopy==false, then the string is returned as a pointer if possible,
 * without needing a dest buffer (it can be NULL). If the string needs to be
 * copied or transformed, then it may be placed into dest at an arbitrary offset.
 *
 * If the string is to be written to dest, then U_BUFFER_OVERFLOW_ERROR and
 * U_STRING_NOT_TERMINATED_WARNING are set if appropriate, as usual.
 *
 * If the string is transformed from UTF-16, then a conversion error may occur
 * if an unpaired surrogate is encountered. If the function is successful, then
 * the output UTF-8 string is always well-formed.
 *
 * @param resB Resource bundle.
 * @param stringIndex An index to the wanted string.
 * @param dest Destination buffer. Can be NULL only if capacity=*length==0.
 * @param pLength Input: Capacity of destination buffer.
 *               Output: Actual length of the UTF-8 string, not counting the
 *               terminating NUL, even in case of U_BUFFER_OVERFLOW_ERROR.
 *               Can be NULL, meaning capacity=0 and the string length is not
 *               returned to the caller.
 * @param forceCopy If true, then the output string will always be written to
 *                  dest, with U_BUFFER_OVERFLOW_ERROR and
 *                  U_STRING_NOT_TERMINATED_WARNING set if appropriate.
 *                  If false, then the dest buffer may or may not contain a
 *                  copy of the string. dest may or may not be modified.
 *                  If a copy needs to be written, then the UErrorCode parameter
 *                  indicates overflow etc. as usual.
 * @param status Pointer to a standard ICU error code. Its input value must
 *               pass the U_SUCCESS() test, or else the function returns
 *               immediately. Check for U_FAILURE() on output or use with
 *               function chaining. (See User Guide for details.)
 * @return The pointer to the UTF-8 string. It may be dest, or at some offset
 *         from dest (only if !forceCopy), or in unrelated memory.
 *         Always NUL-terminated unless the string was written to dest and
 *         length==capacity (in which case U_STRING_NOT_TERMINATED_WARNING is set).
 *
 * @see ures_getStringByIndex
 * @see u_strToUTF8
 * @stable ICU 3.6
 */
U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_UTF8_String_By_सूचकांक(ध्रुव UResourceBundle* resB,
                          int32_t string_सूचकांक,
                          वर्ण* dest U_LIFETIME_BOUND,
                          int32_t* p_दीर्घता,
                          UBool forceCopy,
                          UError_संहिता* स्थितिः);

/**
 * Returns a resource in a given resource that has a given key. This procedure works only with table
 * resources. Features a fill-in parameter.
 *
 * @param resourceBundle    a resource
 * @param key               a key associated with the wanted resource
 * @param fillIn            if NULL a new UResourceBundle struct is allocated and must be closed by the caller.
 *                          Alternatively, you can supply a struct to be filled by this function.
 * @param status            fills in the outgoing error code.
 * @return                  a pointer to a UResourceBundle struct. If fill in param was NULL, caller must close it
 * @stable ICU 2.0
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_कुञ्चिका(ध्रुव UResourceBundle* resourceBundle,
              ध्रुव वर्ण* कुञ्चिका,
              UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND,
              UError_संहिता* स्थितिः);

/**
 * Returns a string in a given resource that has a given key. This procedure works only with table
 * resources.
 *
 * @param resB              a resource
 * @param key               a key associated with the wanted string
 * @param len               fill in length of the string
 * @param status            fills in the outgoing error code. If an error occurred, we may return NULL, but don't
 *                          count on it. Check status instead!
 * @return                  a pointer to a zero-terminated UChar array which lives in a memory mapped/DLL file.
 * @stable ICU 2.0
 */
U_CAPI ध्रुव UChar* U_EXPORT2
ures_get_String_By_कुञ्चिका(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                    ध्रुव वर्ण* कुञ्चिका,
                    int32_t* len,
                    UError_संहिता* स्थितिः);

/**
 * Returns a UTF-8 string from a resource and a key.
 * This function works only with table resources.
 *
 * The UTF-8 string may be returnable directly as a pointer, or
 * it may need to be copied, or transformed from UTF-16 using u_strToUTF8()
 * or equivalent.
 *
 * If forceCopy==true, then the string is always written to the dest buffer
 * and dest is returned.
 *
 * If forceCopy==false, then the string is returned as a pointer if possible,
 * without needing a dest buffer (it can be NULL). If the string needs to be
 * copied or transformed, then it may be placed into dest at an arbitrary offset.
 *
 * If the string is to be written to dest, then U_BUFFER_OVERFLOW_ERROR and
 * U_STRING_NOT_TERMINATED_WARNING are set if appropriate, as usual.
 *
 * If the string is transformed from UTF-16, then a conversion error may occur
 * if an unpaired surrogate is encountered. If the function is successful, then
 * the output UTF-8 string is always well-formed.
 *
 * @param resB Resource bundle.
 * @param key  A key associated with the wanted resource
 * @param dest Destination buffer. Can be NULL only if capacity=*length==0.
 * @param pLength Input: Capacity of destination buffer.
 *               Output: Actual length of the UTF-8 string, not counting the
 *               terminating NUL, even in case of U_BUFFER_OVERFLOW_ERROR.
 *               Can be NULL, meaning capacity=0 and the string length is not
 *               returned to the caller.
 * @param forceCopy If true, then the output string will always be written to
 *                  dest, with U_BUFFER_OVERFLOW_ERROR and
 *                  U_STRING_NOT_TERMINATED_WARNING set if appropriate.
 *                  If false, then the dest buffer may or may not contain a
 *                  copy of the string. dest may or may not be modified.
 *                  If a copy needs to be written, then the UErrorCode parameter
 *                  indicates overflow etc. as usual.
 * @param status Pointer to a standard ICU error code. Its input value must
 *               pass the U_SUCCESS() test, or else the function returns
 *               immediately. Check for U_FAILURE() on output or use with
 *               function chaining. (See User Guide for details.)
 * @return The pointer to the UTF-8 string. It may be dest, or at some offset
 *         from dest (only if !forceCopy), or in unrelated memory.
 *         Always NUL-terminated unless the string was written to dest and
 *         length==capacity (in which case U_STRING_NOT_TERMINATED_WARNING is set).
 *
 * @see ures_getStringByKey
 * @see u_strToUTF8
 * @stable ICU 3.6
 */
U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_UTF8_String_By_कुञ्चिका(ध्रुव UResourceBundle* resB,
                        ध्रुव वर्ण* कुञ्चिका,
                        वर्ण* dest U_LIFETIME_BOUND,
                        int32_t* p_दीर्घता,
                        UBool forceCopy,
                        UError_संहिता* स्थितिः);

#यदि U_SHOW_CPLUSPLUS_API
#अन्तर्भाव "unicode/unistr.h"

U_NAMESPACE_BEGIN
/**
 * Returns the string value from a string resource bundle.
 *
 * @param resB    a resource, should have type URES_STRING
 * @param status: fills in the outgoing error code
 *                could be <TT>U_MISSING_RESOURCE_ERROR</TT> if the key is not found
 *                could be a non-failing error
 *                e.g.: <TT>U_USING_FALLBACK_WARNING</TT>,<TT>U_USING_DEFAULT_WARNING </TT>
 * @return The string value, or a bogus string if there is a failure UErrorCode.
 * @stable ICU 2.0
 */
अन्तःस्थ UnicodeString
ures_getUnicodeString(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND, UError_संहिता* स्थितिः) {
    UnicodeString परिणाम;
    int32_t len = 0;
    ध्रुव char16_t *r = ConstChar16Ptr(ures_getString(resB, &len, स्थितिः));
    यदि(U_SUCCESS(*स्थितिः)) {
        परिणाम.समुच्चयः_गन्तव्यम्(true, r, len);
    } अन्यथा {
        परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    }
    प्रत्यागम परिणाम;
}

/**
 * Returns the next string in a resource, or an empty string if there are no more resources
 * to iterate over.
 * Use ures_getNextString() instead to distinguish between
 * the end of the iteration and a real empty string value.
 *
 * @param resB              a resource
 * @param key               fill in for key associated with this string
 * @param status            fills in the outgoing error code
 * @return The string value, or a bogus string if there is a failure UErrorCode.
 * @stable ICU 2.0
 */
अन्तःस्थ UnicodeString
ures_get_अग्र_Unicode_String(UResourceBundle* resB U_LIFETIME_BOUND,
                          ध्रुव वर्ण** कुञ्चिका,
                          UError_संहिता* स्थितिः) {
    UnicodeString परिणाम;
    int32_t len = 0;
    ध्रुव char16_t* r = ConstChar16Ptr(ures_get_अग्र_String(resB, &len, कुञ्चिका, स्थितिः));
    यदि(U_SUCCESS(*स्थितिः)) {
        परिणाम.समुच्चयः_गन्तव्यम्(true, r, len);
    } अन्यथा {
        परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    }
    प्रत्यागम परिणाम;
}

/**
 * Returns the string in a given resource array or table at the specified index.
 *
 * @param resB              a resource
 * @param indexS            an index to the wanted string.
 * @param status            fills in the outgoing error code
 * @return The string value, or a bogus string if there is a failure UErrorCode.
 * @stable ICU 2.0
 */
अन्तःस्थ UnicodeString
ures_get_Unicode_String_By_सूचकांक(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                             int32_t सूचकांक_S,
                             UError_संहिता* स्थितिः) {
    UnicodeString परिणाम;
    int32_t len = 0;
    ध्रुव char16_t* r = ConstChar16Ptr(ures_get_String_By_सूचकांक(resB, सूचकांक_S, &len, स्थितिः));
    यदि(U_SUCCESS(*स्थितिः)) {
        परिणाम.समुच्चयः_गन्तव्यम्(true, r, len);
    } अन्यथा {
        परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    }
    प्रत्यागम परिणाम;
}

/**
 * Returns a string in a resource that has a given key.
 * This procedure works only with table resources.
 *
 * @param resB              a resource
 * @param key               a key associated with the wanted string
 * @param status            fills in the outgoing error code
 * @return The string value, or a bogus string if there is a failure UErrorCode.
 * @stable ICU 2.0
 */
अन्तःस्थ UnicodeString
ures_get_Unicode_String_By_कुञ्चिका(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                           ध्रुव वर्ण* कुञ्चिका,
                           UError_संहिता* स्थितिः) {
    UnicodeString परिणाम;
    int32_t len = 0;
    ध्रुव char16_t* r = ConstChar16Ptr(ures_get_String_By_कुञ्चिका(resB, कुञ्चिका, &len, स्थितिः));
    यदि(U_SUCCESS(*स्थितिः)) {
        परिणाम.समुच्चयः_गन्तव्यम्(true, r, len);
    } अन्यथा {
        परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    }
    प्रत्यागम परिणाम;
}

U_NAMESPACE_अन्तः

#यदि_अन्तः

/**
 * Create a string enumerator, owned by the caller, of all locales located within
 * the specified resource tree.
 * @param packageName name of the tree, such as (NULL) or U_ICUDATA_ALIAS or  or "ICUDATA-coll"
 * This call is similar to uloc_getAvailable().
 * @param status error code
 * @stable ICU 3.2
 */
U_CAPI UEnumeration* U_EXPORT2
ures_open_उपलब्धम्_Locales(ध्रुव वर्ण *packageName, UError_संहिता *स्थितिः);


#यदि_अन्तः /*_URES*/
/*eof*/
