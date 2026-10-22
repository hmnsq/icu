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

#अन्तर्भाव "unicode/utypes.h"

#यदि UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_END

#अन्यथा

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/strenum.h"

#अन्तर्भाव "hash.h"
#अन्तर्भाव "uvector.h"

#अन्तर्भाव "serv.h"
#अन्तर्भाव "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_साँचो;
class Locale_साँचो_Factory;
class Simple_Locale_साँचो_Factory;
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

class U_COMMON_API Locale_साँचो : सार्वजनिक ICUService_साँचो {
  गुप्त: 
    int32_t _kind;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  सार्वजनिक:
    गणना {
        KIND_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    स्थिर Locale_साँचो* createWithCanonicalFallback(अचर UnicodeString* primaryID, 
                                                  अचर UnicodeString* canonicalFallbackID,
                                                  UErrorCode& status);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    स्थिर Locale_साँचो* createWithCanonicalFallback(अचर UnicodeString* primaryID, 
                                                  अचर UnicodeString* canonicalFallbackID, 
                                                  int32_t kind,
                                                  UErrorCode& status);

  रक्षित:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_साँचो(अचर UnicodeString& primaryID, 
              अचर UnicodeString& canonicalPrimaryID, 
              अचर UnicodeString* canonicalFallbackID, 
              int32_t kind);

 सार्वजनिक:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    आभास UnicodeString& prefix(UnicodeString& result) अचर override;

    /**
     * Return the kind code associated with this key.
     */
    आभास int32_t kind() अचर;

    /**
     * Return the canonicalID.
     */
    आभास UnicodeString& canonicalID(UnicodeString& result) अचर override;

    /**
     * Return the currentID.
     */
    आभास UnicodeString& currentID(UnicodeString& result) अचर override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    आभास UnicodeString& currentDescriptor(UnicodeString& result) अचर override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    आभास Locale& canonicalLocale(Locale& result) अचर;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    आभास Locale& currentLocale(Locale& result) अचर;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    आभास UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    आभास UBool isFallbackOf(अचर UnicodeString& id) अचर override;
    
 सार्वजनिक:
    /**
     * UObject boilerplate.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    आभास UClassID getDynamicClassID() अचर override;

    /**
     * Destructor.
     */
    आभास ~Locale_साँचो();

#ifdef SERVICE_DEBUG
 सार्वजनिक:
    आभास UnicodeString& debug(UnicodeString& result) अचर;
    आभास UnicodeString& debugClass(UnicodeString& result) अचर;
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
class U_COMMON_API Locale_साँचो_Factory : सार्वजनिक ICUServiceFactory {
रक्षित:
    अचर UnicodeString _name;
    अचर int32_t _coverage;

सार्वजनिक:
    गणना {
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
    आभास ~Locale_साँचो_Factory();

रक्षित:
    /**
     * Constructor used by subclasses.
     */
    Locale_साँचो_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_साँचो_Factory(int32_t coverage, अचर UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
सार्वजनिक:
    आभास UObject* create(अचर ICUService_साँचो& साँचो, अचर ICUService* service, UErrorCode& status) अचर override;

रक्षित:
    आभास UBool handles_साँचो(अचर ICUService_साँचो& साँचो, UErrorCode& status) अचर;

सार्वजनिक:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    आभास शून्य updateVisibleIDs(Hashtable& result, UErrorCode& status) अचर override;

    /**
     * Return a localized name for the locale represented by id.
     */
    आभास UnicodeString& getDisplayName(अचर UnicodeString& id, अचर Locale& locale, UnicodeString& result) अचर override;

रक्षित:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    आभास UObject* handleCreate(अचर Locale& loc, int32_t kind, अचर ICUService* service, UErrorCode& status) अचर;

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
    आभास अचर Hashtable* getSupportedIDs(UErrorCode& status) अचर;

सार्वजनिक:
    /**
     * UObject boilerplate.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    आभास UClassID getDynamicClassID() अचर override;

#ifdef SERVICE_DEBUG
 सार्वजनिक:
    आभास UnicodeString& debug(UnicodeString& result) अचर;
    आभास UnicodeString& debugClass(UnicodeString& result) अचर;
#endif

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_साँचो_Factory : सार्वजनिक Locale_साँचो_Factory {
 गुप्त:
    UObject* _obj;
    UnicodeString _id;
    अचर int32_t _kind;

 सार्वजनिक:
    Simple_Locale_साँचो_Factory(UObject* objToAdopt, 
                           अचर UnicodeString& locale, 
                           int32_t kind, 
                           int32_t coverage);

    Simple_Locale_साँचो_Factory(UObject* objToAdopt, 
                           अचर Locale& locale, 
                           int32_t kind, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    आभास ~Simple_Locale_साँचो_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    आभास UObject* create(अचर ICUService_साँचो& साँचो, अचर ICUService* service, UErrorCode& status) अचर override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    आभास शून्य updateVisibleIDs(Hashtable& result, UErrorCode& status) अचर override;

 रक्षित:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


सार्वजनिक:
    /**
     * UObject boilerplate.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    आभास UClassID getDynamicClassID() अचर override;

#ifdef SERVICE_DEBUG
 सार्वजनिक:
    आभास UnicodeString& debug(UnicodeString& result) अचर;
    आभास UnicodeString& debugClass(UnicodeString& result) अचर;
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
class U_COMMON_API ICUResourceBundleFactory : सार्वजनिक Locale_साँचो_Factory 
{
 रक्षित:
    UnicodeString _bundleName;

 सार्वजनिक:
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
    ICUResourceBundleFactory(अचर UnicodeString& bundleName);

    /**
     * Destructor
     */
    आभास ~ICUResourceBundleFactory();

रक्षित:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    आभास अचर Hashtable* getSupportedIDs(UErrorCode& status) अचर override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    आभास UObject* handleCreate(अचर Locale& loc, int32_t kind, अचर ICUService* service, UErrorCode& status) अचर override;

सार्वजनिक:
    /**
     * UObject boilerplate.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();
    आभास UClassID getDynamicClassID() अचर override;


#ifdef SERVICE_DEBUG
 सार्वजनिक:
    आभास UnicodeString& debug(UnicodeString& result) अचर;
    आभास UnicodeString& debugClass(UnicodeString& result) अचर;
#endif

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : सार्वजनिक ICUService
{
 गुप्त:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 सार्वजनिक:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(अचर UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API आभास ~ICULocaleService();

#यदि 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(अचर UnicodeString& descriptor, UErrorCode& status) अचर {
    फर्काऊ ICUService::get(descriptor, status);
  }

  UObject* get(अचर UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) अचर {
    फर्काऊ ICUService::get(descriptor, actualReturn, status);
  }
#endif

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(अचर Locale& locale, UErrorCode& status) अचर;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(अचर Locale& locale, int32_t kind, UErrorCode& status) अचर;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(अचर Locale& locale, Locale* actualReturn, UErrorCode& status) अचर;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(अचर Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) अचर;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API आभास URegistry_साँचो registerInstance(UObject* objToAdopt, अचर Locale& locale, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API आभास URegistry_साँचो registerInstance(UObject* objToAdopt, अचर Locale& locale, int32_t kind, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API आभास URegistry_साँचो registerInstance(UObject* objToAdopt, अचर Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API आभास URegistry_साँचो registerInstance(UObject* objToAdopt, अचर UnicodeString& locale, UBool visible, UErrorCode& status) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API आभास StringEnumeration* getAvailableLocales() अचर U_LIFETIME_BOUND;

 रक्षित:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API अचर UnicodeString& validateFallbackLocale() अचर;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API आभास ICUService_साँचो* create_साँचो(अचर UnicodeString* id, UErrorCode& status) अचर override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API आभास ICUService_साँचो* create_साँचो(अचर UnicodeString* id, int32_t kind, UErrorCode& status) अचर;

  मित्र class ServiceEnumeration;
};

U_NAMESPACE_END

    /* UCONFIG_NO_SERVICE */
#endif

    /* ICULSERV_H */
#endif

