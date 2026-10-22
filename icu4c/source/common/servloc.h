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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_END

#അല്ലെങ്കിൽ

#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/strenum.h"

#അന്തര്ഭാവ "hash.h"
#അന്തര്ഭാവ "uvector.h"

#അന്തര്ഭാവ "serv.h"
#അന്തര്ഭാവ "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_താക്കോൽ;
class Locale_താക്കോൽ_Factory;
class Simple_Locale_താക്കോൽ_Factory;
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

class U_COMMON_API Locale_താക്കോൽ : പൊതു ICUService_താക്കോൽ {
  സ്വകാര്യ: 
    int32_t _kind;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  പൊതു:
    എണ്ണൽ {
        KIND_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    സ്ഥിതം Locale_താക്കോൽ* createWithCanonicalFallback(സ്ഥിരം UnicodeString* primaryID, 
                                                  സ്ഥിരം UnicodeString* canonicalFallbackID,
                                                  UErrorCode& status);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    സ്ഥിതം Locale_താക്കോൽ* createWithCanonicalFallback(സ്ഥിരം UnicodeString* primaryID, 
                                                  സ്ഥിരം UnicodeString* canonicalFallbackID, 
                                                  int32_t kind,
                                                  UErrorCode& status);

  സംരക്ഷിത:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_താക്കോൽ(സ്ഥിരം UnicodeString& primaryID, 
              സ്ഥിരം UnicodeString& canonicalPrimaryID, 
              സ്ഥിരം UnicodeString* canonicalFallbackID, 
              int32_t kind);

 പൊതു:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    മായാ UnicodeString& prefix(UnicodeString& result) സ്ഥിരം override;

    /**
     * Return the kind code associated with this key.
     */
    മായാ int32_t kind() സ്ഥിരം;

    /**
     * Return the canonicalID.
     */
    മായാ UnicodeString& canonicalID(UnicodeString& result) സ്ഥിരം override;

    /**
     * Return the currentID.
     */
    മായാ UnicodeString& currentID(UnicodeString& result) സ്ഥിരം override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    മായാ UnicodeString& currentDescriptor(UnicodeString& result) സ്ഥിരം override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    മായാ Locale& canonicalLocale(Locale& result) സ്ഥിരം;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    മായാ Locale& currentLocale(Locale& result) സ്ഥിരം;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    മായാ UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    മായാ UBool isFallbackOf(സ്ഥിരം UnicodeString& id) സ്ഥിരം override;
    
 പൊതു:
    /**
     * UObject boilerplate.
     */
    സ്ഥിതം UClassID U_EXPORT2 getStaticClassID();

    മായാ UClassID getDynamicClassID() സ്ഥിരം override;

    /**
     * Destructor.
     */
    മായാ ~Locale_താക്കോൽ();

#ifdef SERVICE_DEBUG
 പൊതു:
    മായാ UnicodeString& debug(UnicodeString& result) സ്ഥിരം;
    മായാ UnicodeString& debugClass(UnicodeString& result) സ്ഥിരം;
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
class U_COMMON_API Locale_താക്കോൽ_Factory : പൊതു ICUServiceFactory {
സംരക്ഷിത:
    സ്ഥിരം UnicodeString _name;
    സ്ഥിരം int32_t _coverage;

പൊതു:
    എണ്ണൽ {
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
    മായാ ~Locale_താക്കോൽ_Factory();

സംരക്ഷിത:
    /**
     * Constructor used by subclasses.
     */
    Locale_താക്കോൽ_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_താക്കോൽ_Factory(int32_t coverage, സ്ഥിരം UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
പൊതു:
    മായാ UObject* create(സ്ഥിരം ICUService_താക്കോൽ& താക്കോൽ, സ്ഥിരം ICUService* service, UErrorCode& status) സ്ഥിരം override;

സംരക്ഷിത:
    മായാ UBool handles_താക്കോൽ(സ്ഥിരം ICUService_താക്കോൽ& താക്കോൽ, UErrorCode& status) സ്ഥിരം;

പൊതു:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    മായാ ശൂന്യം updateVisibleIDs(Hashtable& result, UErrorCode& status) സ്ഥിരം override;

    /**
     * Return a localized name for the locale represented by id.
     */
    മായാ UnicodeString& getDisplayName(സ്ഥിരം UnicodeString& id, സ്ഥിരം Locale& locale, UnicodeString& result) സ്ഥിരം override;

സംരക്ഷിത:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    മായാ UObject* handleCreate(സ്ഥിരം Locale& loc, int32_t kind, സ്ഥിരം ICUService* service, UErrorCode& status) സ്ഥിരം;

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
    മായാ സ്ഥിരം Hashtable* getSupportedIDs(UErrorCode& status) സ്ഥിരം;

പൊതു:
    /**
     * UObject boilerplate.
     */
    സ്ഥിതം UClassID U_EXPORT2 getStaticClassID();

    മായാ UClassID getDynamicClassID() സ്ഥിരം override;

#ifdef SERVICE_DEBUG
 പൊതു:
    മായാ UnicodeString& debug(UnicodeString& result) സ്ഥിരം;
    മായാ UnicodeString& debugClass(UnicodeString& result) സ്ഥിരം;
#endif

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_താക്കോൽ_Factory : പൊതു Locale_താക്കോൽ_Factory {
 സ്വകാര്യ:
    UObject* _obj;
    UnicodeString _id;
    സ്ഥിരം int32_t _kind;

 പൊതു:
    Simple_Locale_താക്കോൽ_Factory(UObject* objToAdopt, 
                           സ്ഥിരം UnicodeString& locale, 
                           int32_t kind, 
                           int32_t coverage);

    Simple_Locale_താക്കോൽ_Factory(UObject* objToAdopt, 
                           സ്ഥിരം Locale& locale, 
                           int32_t kind, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    മായാ ~Simple_Locale_താക്കോൽ_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    മായാ UObject* create(സ്ഥിരം ICUService_താക്കോൽ& താക്കോൽ, സ്ഥിരം ICUService* service, UErrorCode& status) സ്ഥിരം override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    മായാ ശൂന്യം updateVisibleIDs(Hashtable& result, UErrorCode& status) സ്ഥിരം override;

 സംരക്ഷിത:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


പൊതു:
    /**
     * UObject boilerplate.
     */
    സ്ഥിതം UClassID U_EXPORT2 getStaticClassID();

    മായാ UClassID getDynamicClassID() സ്ഥിരം override;

#ifdef SERVICE_DEBUG
 പൊതു:
    മായാ UnicodeString& debug(UnicodeString& result) സ്ഥിരം;
    മായാ UnicodeString& debugClass(UnicodeString& result) സ്ഥിരം;
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
class U_COMMON_API ICUResourceBundleFactory : പൊതു Locale_താക്കോൽ_Factory 
{
 സംരക്ഷിത:
    UnicodeString _bundleName;

 പൊതു:
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
    ICUResourceBundleFactory(സ്ഥിരം UnicodeString& bundleName);

    /**
     * Destructor
     */
    മായാ ~ICUResourceBundleFactory();

സംരക്ഷിത:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    മായാ സ്ഥിരം Hashtable* getSupportedIDs(UErrorCode& status) സ്ഥിരം override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    മായാ UObject* handleCreate(സ്ഥിരം Locale& loc, int32_t kind, സ്ഥിരം ICUService* service, UErrorCode& status) സ്ഥിരം override;

പൊതു:
    /**
     * UObject boilerplate.
     */
    സ്ഥിതം UClassID U_EXPORT2 getStaticClassID();
    മായാ UClassID getDynamicClassID() സ്ഥിരം override;


#ifdef SERVICE_DEBUG
 പൊതു:
    മായാ UnicodeString& debug(UnicodeString& result) സ്ഥിരം;
    മായാ UnicodeString& debugClass(UnicodeString& result) സ്ഥിരം;
#endif

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : പൊതു ICUService
{
 സ്വകാര്യ:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 പൊതു:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(സ്ഥിരം UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API മായാ ~ICULocaleService();

#എങ്കിൽ 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(സ്ഥിരം UnicodeString& descriptor, UErrorCode& status) സ്ഥിരം {
    തിരികെ ICUService::get(descriptor, status);
  }

  UObject* get(സ്ഥിരം UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) സ്ഥിരം {
    തിരികെ ICUService::get(descriptor, actualReturn, status);
  }
#endif

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(സ്ഥിരം Locale& locale, UErrorCode& status) സ്ഥിരം;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(സ്ഥിരം Locale& locale, int32_t kind, UErrorCode& status) സ്ഥിരം;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(സ്ഥിരം Locale& locale, Locale* actualReturn, UErrorCode& status) സ്ഥിരം;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(സ്ഥിരം Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) സ്ഥിരം;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API മായാ URegistry_താക്കോൽ registerInstance(UObject* objToAdopt, സ്ഥിരം Locale& locale, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API മായാ URegistry_താക്കോൽ registerInstance(UObject* objToAdopt, സ്ഥിരം Locale& locale, int32_t kind, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API മായാ URegistry_താക്കോൽ registerInstance(UObject* objToAdopt, സ്ഥിരം Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API മായാ URegistry_താക്കോൽ registerInstance(UObject* objToAdopt, സ്ഥിരം UnicodeString& locale, UBool visible, UErrorCode& status) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API മായാ StringEnumeration* getAvailableLocales() സ്ഥിരം U_LIFETIME_BOUND;

 സംരക്ഷിത:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API സ്ഥിരം UnicodeString& validateFallbackLocale() സ്ഥിരം;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API മായാ ICUService_താക്കോൽ* create_താക്കോൽ(സ്ഥിരം UnicodeString* id, UErrorCode& status) സ്ഥിരം override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API മായാ ICUService_താക്കോൽ* create_താക്കോൽ(സ്ഥിരം UnicodeString* id, int32_t kind, UErrorCode& status) സ്ഥിരം;

  friend class ServiceEnumeration;
};

U_NAMESPACE_END

    /* UCONFIG_NO_SERVICE */
#endif

    /* ICULSERV_H */
#endif

