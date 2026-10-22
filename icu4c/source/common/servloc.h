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
#अगर_अघोषित ICULSERV_H
#घोषणा ICULSERV_H

#समावेश "unicode/utypes.h"

#अगर UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_END

#अन्यथा

#समावेश "unicode/unistr.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/strenum.h"

#समावेश "hash.h"
#समावेश "uvector.h"

#समावेश "serv.h"
#समावेश "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_कुंजी;
class Locale_कुंजी_Factory;
class Simple_Locale_कुंजी_Factory;
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

class U_COMMON_API Locale_कुंजी : खुला ICUService_कुंजी {
  निजी: 
    int32_t _kind;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  खुला:
    क्रमागत {
        KIND_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    अटल Locale_कुंजी* createWithCanonicalFallback(स्थिर UnicodeString* primaryID, 
                                                  स्थिर UnicodeString* canonicalFallbackID,
                                                  UErrorCode& status);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    अटल Locale_कुंजी* createWithCanonicalFallback(स्थिर UnicodeString* primaryID, 
                                                  स्थिर UnicodeString* canonicalFallbackID, 
                                                  int32_t kind,
                                                  UErrorCode& status);

  रक्षित:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_कुंजी(स्थिर UnicodeString& primaryID, 
              स्थिर UnicodeString& canonicalPrimaryID, 
              स्थिर UnicodeString* canonicalFallbackID, 
              int32_t kind);

 खुला:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    भव UnicodeString& prefix(UnicodeString& परिणाम) स्थिर override;

    /**
     * Return the kind code associated with this key.
     */
    भव int32_t kind() स्थिर;

    /**
     * Return the canonicalID.
     */
    भव UnicodeString& canonicalID(UnicodeString& परिणाम) स्थिर override;

    /**
     * Return the currentID.
     */
    भव UnicodeString& currentID(UnicodeString& परिणाम) स्थिर override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    भव UnicodeString& currentDescriptor(UnicodeString& परिणाम) स्थिर override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    भव Locale& canonicalLocale(Locale& परिणाम) स्थिर;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    भव Locale& currentLocale(Locale& परिणाम) स्थिर;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    भव UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    भव UBool isFallbackOf(स्थिर UnicodeString& id) स्थिर override;
    
 खुला:
    /**
     * UObject boilerplate.
     */
    अटल UClassID U_EXPORT2 getStaticClassID();

    भव UClassID getDynamicClassID() स्थिर override;

    /**
     * Destructor.
     */
    भव ~Locale_कुंजी();

#अगर_घोषित SERVICE_DEBUG
 खुला:
    भव UnicodeString& debug(UnicodeString& परिणाम) स्थिर;
    भव UnicodeString& debugClass(UnicodeString& परिणाम) स्थिर;
#पूर्ण_अगर

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
class U_COMMON_API Locale_कुंजी_Factory : खुला ICUServiceFactory {
रक्षित:
    स्थिर UnicodeString _name;
    स्थिर int32_t _coverage;

खुला:
    क्रमागत {
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
    भव ~Locale_कुंजी_Factory();

रक्षित:
    /**
     * Constructor used by subclasses.
     */
    Locale_कुंजी_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_कुंजी_Factory(int32_t coverage, स्थिर UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
खुला:
    भव UObject* create(स्थिर ICUService_कुंजी& कुंजी, स्थिर ICUService* service, UErrorCode& status) स्थिर override;

रक्षित:
    भव UBool handles_कुंजी(स्थिर ICUService_कुंजी& कुंजी, UErrorCode& status) स्थिर;

खुला:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    भव व्योम updateVisibleIDs(Hashtable& परिणाम, UErrorCode& status) स्थिर override;

    /**
     * Return a localized name for the locale represented by id.
     */
    भव UnicodeString& getDisplayName(स्थिर UnicodeString& id, स्थिर Locale& locale, UnicodeString& परिणाम) स्थिर override;

रक्षित:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    भव UObject* handleCreate(स्थिर Locale& loc, int32_t kind, स्थिर ICUService* service, UErrorCode& status) स्थिर;

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
    भव स्थिर Hashtable* getSupportedIDs(UErrorCode& status) स्थिर;

खुला:
    /**
     * UObject boilerplate.
     */
    अटल UClassID U_EXPORT2 getStaticClassID();

    भव UClassID getDynamicClassID() स्थिर override;

#अगर_घोषित SERVICE_DEBUG
 खुला:
    भव UnicodeString& debug(UnicodeString& परिणाम) स्थिर;
    भव UnicodeString& debugClass(UnicodeString& परिणाम) स्थिर;
#पूर्ण_अगर

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_कुंजी_Factory : खुला Locale_कुंजी_Factory {
 निजी:
    UObject* _obj;
    UnicodeString _id;
    स्थिर int32_t _kind;

 खुला:
    Simple_Locale_कुंजी_Factory(UObject* objToAdopt, 
                           स्थिर UnicodeString& locale, 
                           int32_t kind, 
                           int32_t coverage);

    Simple_Locale_कुंजी_Factory(UObject* objToAdopt, 
                           स्थिर Locale& locale, 
                           int32_t kind, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    भव ~Simple_Locale_कुंजी_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    भव UObject* create(स्थिर ICUService_कुंजी& कुंजी, स्थिर ICUService* service, UErrorCode& status) स्थिर override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    भव व्योम updateVisibleIDs(Hashtable& परिणाम, UErrorCode& status) स्थिर override;

 रक्षित:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


खुला:
    /**
     * UObject boilerplate.
     */
    अटल UClassID U_EXPORT2 getStaticClassID();

    भव UClassID getDynamicClassID() स्थिर override;

#अगर_घोषित SERVICE_DEBUG
 खुला:
    भव UnicodeString& debug(UnicodeString& परिणाम) स्थिर;
    भव UnicodeString& debugClass(UnicodeString& परिणाम) स्थिर;
#पूर्ण_अगर

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
class U_COMMON_API ICUResourceBundleFactory : खुला Locale_कुंजी_Factory 
{
 रक्षित:
    UnicodeString _bundleName;

 खुला:
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
    ICUResourceBundleFactory(स्थिर UnicodeString& bundleName);

    /**
     * Destructor
     */
    भव ~ICUResourceBundleFactory();

रक्षित:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    भव स्थिर Hashtable* getSupportedIDs(UErrorCode& status) स्थिर override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    भव UObject* handleCreate(स्थिर Locale& loc, int32_t kind, स्थिर ICUService* service, UErrorCode& status) स्थिर override;

खुला:
    /**
     * UObject boilerplate.
     */
    अटल UClassID U_EXPORT2 getStaticClassID();
    भव UClassID getDynamicClassID() स्थिर override;


#अगर_घोषित SERVICE_DEBUG
 खुला:
    भव UnicodeString& debug(UnicodeString& परिणाम) स्थिर;
    भव UnicodeString& debugClass(UnicodeString& परिणाम) स्थिर;
#पूर्ण_अगर

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : खुला ICUService
{
 निजी:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 खुला:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(स्थिर UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API भव ~ICULocaleService();

#अगर 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(स्थिर UnicodeString& descriptor, UErrorCode& status) स्थिर {
    वापस ICUService::get(descriptor, status);
  }

  UObject* get(स्थिर UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) स्थिर {
    वापस ICUService::get(descriptor, actualReturn, status);
  }
#पूर्ण_अगर

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(स्थिर Locale& locale, UErrorCode& status) स्थिर;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(स्थिर Locale& locale, int32_t kind, UErrorCode& status) स्थिर;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(स्थिर Locale& locale, Locale* actualReturn, UErrorCode& status) स्थिर;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(स्थिर Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) स्थिर;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API भव URegistry_कुंजी registerInstance(UObject* objToAdopt, स्थिर Locale& locale, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API भव URegistry_कुंजी registerInstance(UObject* objToAdopt, स्थिर Locale& locale, int32_t kind, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API भव URegistry_कुंजी registerInstance(UObject* objToAdopt, स्थिर Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API भव URegistry_कुंजी registerInstance(UObject* objToAdopt, स्थिर UnicodeString& locale, UBool visible, UErrorCode& status) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API भव StringEnumeration* getAvailableLocales() स्थिर U_LIFETIME_BOUND;

 रक्षित:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API स्थिर UnicodeString& validateFallbackLocale() स्थिर;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API भव ICUService_कुंजी* create_कुंजी(स्थिर UnicodeString* id, UErrorCode& status) स्थिर override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API भव ICUService_कुंजी* create_कुंजी(स्थिर UnicodeString* id, int32_t kind, UErrorCode& status) स्थिर;

  मित्र class ServiceEnumeration;
};

U_NAMESPACE_END

    /* UCONFIG_NO_SERVICE */
#पूर्ण_अगर

    /* ICULSERV_H */
#पूर्ण_अगर

