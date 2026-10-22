// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2011, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 *
 *******************************************************************************
 */
#ifndef ICULSERV_H
#define ICULSERV_H

#அந்தர்பாவ "unicode/utypes.h"

#எனில் UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_END

#இல்லையெனில்

#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/strenum.h"

#அந்தர்பாவ "hash.h"
#அந்தர்பாவ "uvector.h"

#அந்தர்பாவ "serv.h"
#அந்தர்பாவ "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_சாவி;
class Locale_சாவி_Factory;
class Simple_Locale_சாவி_Factory;
class ServiceListener;

/*
 ******************************************************************
 */

/**
 * A subclass of Key that implements a locale fallback mechanism.
 * The first locale to search for is the locale provided by the
 * client, and the fallback locale to search for is the current
 * default locale.  If a prefix is present, the currentDescriptor
 * includes it before the locale proper, separated by "/".  This
 * is the default key instantiated by ICULocaleService.</p>
 *
 * <p>Canonicalization adjusts the locale string so that the
 * section before the first understore is in lower case, and the rest
 * is in upper case, with no trailing underscores.</p> 
 */

class U_COMMON_API Locale_சாவி : பொது ICUService_சாவி {
  தனிப்பட்ட: 
    int32_t _kind;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  பொது:
    எண்ணிக்கை {
        KIND_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    நிலையான Locale_சாவி* createWithCanonicalFallback(மாறிலி UnicodeString* primaryID, 
                                                  மாறிலி UnicodeString* canonicalFallbackID,
                                                  UErrorCode& status);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    நிலையான Locale_சாவி* createWithCanonicalFallback(மாறிலி UnicodeString* primaryID, 
                                                  மாறிலி UnicodeString* canonicalFallbackID, 
                                                  int32_t kind,
                                                  UErrorCode& status);

  பாதுகாக்கப்பட்ட:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_சாவி(மாறிலி UnicodeString& primaryID, 
              மாறிலி UnicodeString& canonicalPrimaryID, 
              மாறிலி UnicodeString* canonicalFallbackID, 
              int32_t kind);

 பொது:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    மெய்நிகர் UnicodeString& prefix(UnicodeString& result) மாறிலி override;

    /**
     * Return the kind code associated with this key.
     */
    மெய்நிகர் int32_t kind() மாறிலி;

    /**
     * Return the canonicalID.
     */
    மெய்நிகர் UnicodeString& canonicalID(UnicodeString& result) மாறிலி override;

    /**
     * Return the currentID.
     */
    மெய்நிகர் UnicodeString& currentID(UnicodeString& result) மாறிலி override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    மெய்நிகர் UnicodeString& currentDescriptor(UnicodeString& result) மாறிலி override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    மெய்நிகர் Locale& canonicalLocale(Locale& result) மாறிலி;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    மெய்நிகர் Locale& currentLocale(Locale& result) மாறிலி;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    மெய்நிகர் UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    மெய்நிகர் UBool isFallbackOf(மாறிலி UnicodeString& id) மாறிலி override;
    
 பொது:
    /**
     * UObject boilerplate.
     */
    நிலையான UClassID U_EXPORT2 getStaticClassID();

    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override;

    /**
     * Destructor.
     */
    மெய்நிகர் ~Locale_சாவி();

#ifdef SERVICE_DEBUG
 பொது:
    மெய்நிகர் UnicodeString& debug(UnicodeString& result) மாறிலி;
    மெய்நிகர் UnicodeString& debugClass(UnicodeString& result) மாறிலி;
#endif

};

/*
 ******************************************************************
 */

/**
 * A subclass of ICUServiceFactory that uses LocaleKeys, and is able to
 * 'cover' more specific locales with more general locales that it
 * supports.  
 *
 * <p>Coverage may be either of the values VISIBLE or INVISIBLE.
 *
 * <p>'Visible' indicates that the specific locale(s) supported by
 * the factory are registered in getSupportedIDs, 'Invisible'
 * indicates that they are not.
 *
 * <p>Localization of visible ids is handled
 * by the handling factory, regardless of kind.
 */
class U_COMMON_API Locale_சாவி_Factory : பொது ICUServiceFactory {
பாதுகாக்கப்பட்ட:
    மாறிலி UnicodeString _name;
    மாறிலி int32_t _coverage;

பொது:
    எண்ணிக்கை {
        /**
         * Coverage value indicating that the factory makes
         * its locales visible, and does not cover more specific 
         * locales.
         */
        VISIBLE = 0,

        /**
         * Coverage value indicating that the factory does not make
         * its locales visible, and does not cover more specific
         * locales.
         */
        INVISIBLE = 1
    };

    /**
     * Destructor.
     */
    மெய்நிகர் ~Locale_சாவி_Factory();

பாதுகாக்கப்பட்ட:
    /**
     * Constructor used by subclasses.
     */
    Locale_சாவி_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_சாவி_Factory(int32_t coverage, மாறிலி UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
பொது:
    மெய்நிகர் UObject* create(மாறிலி ICUService_சாவி& சாவி, மாறிலி ICUService* service, UErrorCode& status) மாறிலி override;

பாதுகாக்கப்பட்ட:
    மெய்நிகர் UBool handles_சாவி(மாறிலி ICUService_சாவி& சாவி, UErrorCode& status) மாறிலி;

பொது:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    மெய்நிகர் வெறுமை updateVisibleIDs(Hashtable& result, UErrorCode& status) மாறிலி override;

    /**
     * Return a localized name for the locale represented by id.
     */
    மெய்நிகர் UnicodeString& getDisplayName(மாறிலி UnicodeString& id, மாறிலி Locale& locale, UnicodeString& result) மாறிலி override;

பாதுகாக்கப்பட்ட:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    மெய்நிகர் UObject* handleCreate(மாறிலி Locale& loc, int32_t kind, மாறிலி ICUService* service, UErrorCode& status) மாறிலி;

   /**
     * Return true if this id is one the factory supports (visible or 
     * otherwise).
     */
 //   virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;

   /**
     * Return the set of ids that this factory supports (visible or 
     * otherwise).  This can be called often and might need to be
     * cached if it is expensive to create.
     */
    மெய்நிகர் மாறிலி Hashtable* getSupportedIDs(UErrorCode& status) மாறிலி;

பொது:
    /**
     * UObject boilerplate.
     */
    நிலையான UClassID U_EXPORT2 getStaticClassID();

    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override;

#ifdef SERVICE_DEBUG
 பொது:
    மெய்நிகர் UnicodeString& debug(UnicodeString& result) மாறிலி;
    மெய்நிகர் UnicodeString& debugClass(UnicodeString& result) மாறிலி;
#endif

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_சாவி_Factory : பொது Locale_சாவி_Factory {
 தனிப்பட்ட:
    UObject* _obj;
    UnicodeString _id;
    மாறிலி int32_t _kind;

 பொது:
    Simple_Locale_சாவி_Factory(UObject* objToAdopt, 
                           மாறிலி UnicodeString& locale, 
                           int32_t kind, 
                           int32_t coverage);

    Simple_Locale_சாவி_Factory(UObject* objToAdopt, 
                           மாறிலி Locale& locale, 
                           int32_t kind, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    மெய்நிகர் ~Simple_Locale_சாவி_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    மெய்நிகர் UObject* create(மாறிலி ICUService_சாவி& சாவி, மாறிலி ICUService* service, UErrorCode& status) மாறிலி override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    மெய்நிகர் வெறுமை updateVisibleIDs(Hashtable& result, UErrorCode& status) மாறிலி override;

 பாதுகாக்கப்பட்ட:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


பொது:
    /**
     * UObject boilerplate.
     */
    நிலையான UClassID U_EXPORT2 getStaticClassID();

    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override;

#ifdef SERVICE_DEBUG
 பொது:
    மெய்நிகர் UnicodeString& debug(UnicodeString& result) மாறிலி;
    மெய்நிகர் UnicodeString& debugClass(UnicodeString& result) மாறிலி;
#endif

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that creates a service based on the ICU locale data.
 * This is a base class for most ICU factories.  Subclasses instantiate it
 * with a constructor that takes a bundle name, which determines the supported
 * IDs.  Subclasses then override handleCreate to create the actual service
 * object.  The default implementation returns a resource bundle.
 */
class U_COMMON_API ICUResourceBundleFactory : பொது Locale_சாவி_Factory 
{
 பாதுகாக்கப்பட்ட:
    UnicodeString _bundleName;

 பொது:
    /**
     * Convenience constructor that uses the main ICU bundle name.
     */
    ICUResourceBundleFactory();

    /**
     * A service factory based on ICU resource data in resources with
     * the given name.  This should be a 'path' that can be passed to
     * ures_openAvailableLocales, such as U_ICUDATA or U_ICUDATA_COLL.
     * The empty string is equivalent to U_ICUDATA.
     */
    ICUResourceBundleFactory(மாறிலி UnicodeString& bundleName);

    /**
     * Destructor
     */
    மெய்நிகர் ~ICUResourceBundleFactory();

பாதுகாக்கப்பட்ட:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    மெய்நிகர் மாறிலி Hashtable* getSupportedIDs(UErrorCode& status) மாறிலி override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    மெய்நிகர் UObject* handleCreate(மாறிலி Locale& loc, int32_t kind, மாறிலி ICUService* service, UErrorCode& status) மாறிலி override;

பொது:
    /**
     * UObject boilerplate.
     */
    நிலையான UClassID U_EXPORT2 getStaticClassID();
    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override;


#ifdef SERVICE_DEBUG
 பொது:
    மெய்நிகர் UnicodeString& debug(UnicodeString& result) மாறிலி;
    மெய்நிகர் UnicodeString& debugClass(UnicodeString& result) மாறிலி;
#endif

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : பொது ICUService
{
 தனிப்பட்ட:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 பொது:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(மாறிலி UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API மெய்நிகர் ~ICULocaleService();

#எனில் 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(மாறிலி UnicodeString& descriptor, UErrorCode& status) மாறிலி {
    திரும்பு ICUService::get(descriptor, status);
  }

  UObject* get(மாறிலி UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) மாறிலி {
    திரும்பு ICUService::get(descriptor, actualReturn, status);
  }
#endif

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(மாறிலி Locale& locale, UErrorCode& status) மாறிலி;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(மாறிலி Locale& locale, int32_t kind, UErrorCode& status) மாறிலி;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(மாறிலி Locale& locale, Locale* actualReturn, UErrorCode& status) மாறிலி;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(மாறிலி Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) மாறிலி;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API மெய்நிகர் URegistry_சாவி registerInstance(UObject* objToAdopt, மாறிலி Locale& locale, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API மெய்நிகர் URegistry_சாவி registerInstance(UObject* objToAdopt, மாறிலி Locale& locale, int32_t kind, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API மெய்நிகர் URegistry_சாவி registerInstance(UObject* objToAdopt, மாறிலி Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API மெய்நிகர் URegistry_சாவி registerInstance(UObject* objToAdopt, மாறிலி UnicodeString& locale, UBool visible, UErrorCode& status) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API மெய்நிகர் StringEnumeration* getAvailableLocales() மாறிலி U_LIFETIME_BOUND;

 பாதுகாக்கப்பட்ட:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API மாறிலி UnicodeString& validateFallbackLocale() மாறிலி;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API மெய்நிகர் ICUService_சாவி* create_சாவி(மாறிலி UnicodeString* id, UErrorCode& status) மாறிலி override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API மெய்நிகர் ICUService_சாவி* create_சாவி(மாறிலி UnicodeString* id, int32_t kind, UErrorCode& status) மாறிலி;

  friend class ServiceEnumeration;
};

U_NAMESPACE_END

    /* UCONFIG_NO_SERVICE */
#endif

    /* ICULSERV_H */
#endif

