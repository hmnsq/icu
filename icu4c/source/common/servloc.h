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
#నిర్వచితం_కాకపోతే ICULSERV_H
#నిర్వచించు ICULSERV_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_చివర

#లేకపోతే

#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/strenum.h"

#అన్తర్భావ "hash.h"
#అన్తర్భావ "uvector.h"

#అన్తర్భావ "serv.h"
#అన్తర్భావ "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_కీలకం;
class Locale_కీలకం_Factory;
class Simple_Locale_కీలకం_Factory;
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

class U_COMMON_API Locale_కీలకం : బహిరంగ ICUService_కీలకం {
  అంతరంగ: 
    int32_t రకం_2;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  బహిరంగ:
    గణన {
        రకం_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    స్థిర Locale_కీలకం* createWithCanonicalFallback(స్థిరరాశి UnicodeString* primaryID, 
                                                  స్థిరరాశి UnicodeString* canonicalFallbackID,
                                                  UError_సంకేతం& స్థితి_2);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    స్థిర Locale_కీలకం* createWithCanonicalFallback(స్థిరరాశి UnicodeString* primaryID, 
                                                  స్థిరరాశి UnicodeString* canonicalFallbackID, 
                                                  int32_t రకం,
                                                  UError_సంకేతం& స్థితి_2);

  రక్షిత:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_కీలకం(స్థిరరాశి UnicodeString& primaryID, 
              స్థిరరాశి UnicodeString& canonicalPrimaryID, 
              స్థిరరాశి UnicodeString* canonicalFallbackID, 
              int32_t రకం);

 బహిరంగ:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    మాయా UnicodeString& ఉపసర్గ(UnicodeString& ఫలితం) స్థిరరాశి override;

    /**
     * Return the kind code associated with this key.
     */
    మాయా int32_t రకం() స్థిరరాశి;

    /**
     * Return the canonicalID.
     */
    మాయా UnicodeString& canonicalID(UnicodeString& ఫలితం) స్థిరరాశి override;

    /**
     * Return the currentID.
     */
    మాయా UnicodeString& currentID(UnicodeString& ఫలితం) స్థిరరాశి override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    మాయా UnicodeString& currentDescriptor(UnicodeString& ఫలితం) స్థిరరాశి override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    మాయా Locale& canonicalLocale(Locale& ఫలితం) స్థిరరాశి;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    మాయా Locale& currentLocale(Locale& ఫలితం) స్థిరరాశి;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    మాయా UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    మాయా UBool is_Fallback_యొక్క(స్థిరరాశి UnicodeString& id) స్థిరరాశి override;
    
 బహిరంగ:
    /**
     * UObject boilerplate.
     */
    స్థిర UClassID U_EXPORT2 getStaticClassID();

    మాయా UClassID getDynamicClassID() స్థిరరాశి override;

    /**
     * Destructor.
     */
    మాయా ~Locale_కీలకం();

#నిర్వచితమైతే SERVICE_DEBUG
 బహిరంగ:
    మాయా UnicodeString& debug(UnicodeString& ఫలితం) స్థిరరాశి;
    మాయా UnicodeString& debugClass(UnicodeString& ఫలితం) స్థిరరాశి;
#అయితే_ముగింపు

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
class U_COMMON_API Locale_కీలకం_Factory : బహిరంగ ICUServiceFactory {
రక్షిత:
    స్థిరరాశి UnicodeString _name;
    స్థిరరాశి int32_t _coverage;

బహిరంగ:
    గణన {
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
    మాయా ~Locale_కీలకం_Factory();

రక్షిత:
    /**
     * Constructor used by subclasses.
     */
    Locale_కీలకం_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_కీలకం_Factory(int32_t coverage, స్థిరరాశి UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
బహిరంగ:
    మాయా UObject* create(స్థిరరాశి ICUService_కీలకం& కీలకం, స్థిరరాశి ICUService* service, UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

రక్షిత:
    మాయా UBool handles_కీలకం(స్థిరరాశి ICUService_కీలకం& కీలకం, UError_సంకేతం& స్థితి_2) స్థిరరాశి;

బహిరంగ:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    మాయా శూన్య updateVisibleIDs(Hashtable& ఫలితం, UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

    /**
     * Return a localized name for the locale represented by id.
     */
    మాయా UnicodeString& getDisplayName(స్థిరరాశి UnicodeString& id, స్థిరరాశి Locale& locale, UnicodeString& ఫలితం) స్థిరరాశి override;

రక్షిత:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    మాయా UObject* handleCreate(స్థిరరాశి Locale& loc, int32_t రకం, స్థిరరాశి ICUService* service, UError_సంకేతం& స్థితి_2) స్థిరరాశి;

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
    మాయా స్థిరరాశి Hashtable* getSupportedIDs(UError_సంకేతం& స్థితి_2) స్థిరరాశి;

బహిరంగ:
    /**
     * UObject boilerplate.
     */
    స్థిర UClassID U_EXPORT2 getStaticClassID();

    మాయా UClassID getDynamicClassID() స్థిరరాశి override;

#నిర్వచితమైతే SERVICE_DEBUG
 బహిరంగ:
    మాయా UnicodeString& debug(UnicodeString& ఫలితం) స్థిరరాశి;
    మాయా UnicodeString& debugClass(UnicodeString& ఫలితం) స్థిరరాశి;
#అయితే_ముగింపు

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_కీలకం_Factory : బహిరంగ Locale_కీలకం_Factory {
 అంతరంగ:
    UObject* _obj;
    UnicodeString _id;
    స్థిరరాశి int32_t రకం_2;

 బహిరంగ:
    Simple_Locale_కీలకం_Factory(UObject* obj_గమ్యం_Adopt, 
                           స్థిరరాశి UnicodeString& locale, 
                           int32_t రకం, 
                           int32_t coverage);

    Simple_Locale_కీలకం_Factory(UObject* obj_గమ్యం_Adopt, 
                           స్థిరరాశి Locale& locale, 
                           int32_t రకం, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    మాయా ~Simple_Locale_కీలకం_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    మాయా UObject* create(స్థిరరాశి ICUService_కీలకం& కీలకం, స్థిరరాశి ICUService* service, UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    మాయా శూన్య updateVisibleIDs(Hashtable& ఫలితం, UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

 రక్షిత:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


బహిరంగ:
    /**
     * UObject boilerplate.
     */
    స్థిర UClassID U_EXPORT2 getStaticClassID();

    మాయా UClassID getDynamicClassID() స్థిరరాశి override;

#నిర్వచితమైతే SERVICE_DEBUG
 బహిరంగ:
    మాయా UnicodeString& debug(UnicodeString& ఫలితం) స్థిరరాశి;
    మాయా UnicodeString& debugClass(UnicodeString& ఫలితం) స్థిరరాశి;
#అయితే_ముగింపు

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
class U_COMMON_API ICUResourceBundleFactory : బహిరంగ Locale_కీలకం_Factory 
{
 రక్షిత:
    UnicodeString _bundleName;

 బహిరంగ:
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
    ICUResourceBundleFactory(స్థిరరాశి UnicodeString& bundleName);

    /**
     * Destructor
     */
    మాయా ~ICUResourceBundleFactory();

రక్షిత:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    మాయా స్థిరరాశి Hashtable* getSupportedIDs(UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    మాయా UObject* handleCreate(స్థిరరాశి Locale& loc, int32_t రకం, స్థిరరాశి ICUService* service, UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

బహిరంగ:
    /**
     * UObject boilerplate.
     */
    స్థిర UClassID U_EXPORT2 getStaticClassID();
    మాయా UClassID getDynamicClassID() స్థిరరాశి override;


#నిర్వచితమైతే SERVICE_DEBUG
 బహిరంగ:
    మాయా UnicodeString& debug(UnicodeString& ఫలితం) స్థిరరాశి;
    మాయా UnicodeString& debugClass(UnicodeString& ఫలితం) స్థిరరాశి;
#అయితే_ముగింపు

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : బహిరంగ ICUService
{
 అంతరంగ:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 బహిరంగ:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(స్థిరరాశి UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API మాయా ~ICULocaleService();

#ఒకవేళ 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(స్థిరరాశి UnicodeString& descriptor, UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    తిరిగి ICUService::get(descriptor, స్థితి_2);
  }

  UObject* get(స్థిరరాశి UnicodeString& descriptor, UnicodeString* actualReturn, UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    తిరిగి ICUService::get(descriptor, actualReturn, స్థితి_2);
  }
#అయితే_ముగింపు

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(స్థిరరాశి Locale& locale, UError_సంకేతం& స్థితి_2) స్థిరరాశి;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(స్థిరరాశి Locale& locale, int32_t రకం, UError_సంకేతం& స్థితి_2) స్థిరరాశి;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(స్థిరరాశి Locale& locale, Locale* actualReturn, UError_సంకేతం& స్థితి_2) స్థిరరాశి;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(స్థిరరాశి Locale& locale, int32_t రకం, Locale* actualReturn, UError_సంకేతం& స్థితి_2) స్థిరరాశి;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API మాయా URegistry_కీలకం రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి Locale& locale, UError_సంకేతం& స్థితి_2);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API మాయా URegistry_కీలకం రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి Locale& locale, int32_t రకం, UError_సంకేతం& స్థితి_2);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API మాయా URegistry_కీలకం రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి Locale& locale, int32_t రకం, int32_t coverage, UError_సంకేతం& స్థితి_2);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API మాయా URegistry_కీలకం రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి UnicodeString& locale, UBool visible, UError_సంకేతం& స్థితి_2) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API మాయా StringEnumeration* get_అందుబాటు_Locales() స్థిరరాశి U_LIFETIME_BOUND;

 రక్షిత:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API స్థిరరాశి UnicodeString& validateFallbackLocale() స్థిరరాశి;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API మాయా ICUService_కీలకం* create_కీలకం(స్థిరరాశి UnicodeString* id, UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API మాయా ICUService_కీలకం* create_కీలకం(స్థిరరాశి UnicodeString* id, int32_t రకం, UError_సంకేతం& స్థితి_2) స్థిరరాశి;

  మిత్ర class ServiceEnumeration;
};

U_NAMESPACE_చివర

    /* UCONFIG_NO_SERVICE */
#అయితే_ముగింపు

    /* ICULSERV_H */
#అయితే_ముగింపు

