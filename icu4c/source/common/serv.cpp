// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/localpointer.h"

#ఒకవేళ !UCONFIG_NO_SERVICE

#అన్తర్భావ "serv.h"
#అన్తర్భావ "umutex.h"

#నిర్వచనం_రద్దు SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

స్థిరరాశి char16_t ICUService_కీలకం::ఉపసర్గ_DELIMITER = 0x002F;   /* '/' */

ICUService_కీలకం::ICUService_కీలకం(స్థిరరాశి UnicodeString& id) 
: _id(id) {
}

ICUService_కీలకం::~ICUService_కీలకం() 
{
}

స్థిరరాశి UnicodeString& 
ICUService_కీలకం::getID() స్థిరరాశి 
{
    తిరిగి _id;
}

UnicodeString& 
ICUService_కీలకం::canonicalID(UnicodeString& ఫలితం) స్థిరరాశి 
{
    తిరిగి ఫలితం.append(_id);
}

UnicodeString& 
ICUService_కీలకం::currentID(UnicodeString& ఫలితం) స్థిరరాశి 
{
    తిరిగి canonicalID(ఫలితం);
}

UnicodeString& 
ICUService_కీలకం::currentDescriptor(UnicodeString& ఫలితం) స్థిరరాశి 
{
    ఉపసర్గ(ఫలితం);
    ఫలితం.append(ఉపసర్గ_DELIMITER);
    తిరిగి currentID(ఫలితం);
}

UBool 
ICUService_కీలకం::fallback() 
{
    తిరిగి అబద్ధం;
}

UBool 
ICUService_కీలకం::is_Fallback_యొక్క(స్థిరరాశి UnicodeString& id) స్థిరరాశి 
{
    తిరిగి id == _id;
}

UnicodeString& 
ICUService_కీలకం::ఉపసర్గ(UnicodeString& ఫలితం) స్థిరరాశి 
{
    తిరిగి ఫలితం;
}

UnicodeString& 
ICUService_కీలకం::parse_ఉపసర్గ(UnicodeString& ఫలితం) 
{
    int32_t n = ఫలితం.సూచీ_యొక్క(ఉపసర్గ_DELIMITER);
    ఒకవేళ (n < 0) {
        n = 0;
    }
    ఫలితం.remove(n);
    తిరిగి ఫలితం;
}

UnicodeString& 
ICUService_కీలకం::parseSuffix(UnicodeString& ఫలితం) 
{
    int32_t n = ఫలితం.సూచీ_యొక్క(ఉపసర్గ_DELIMITER);
    ఒకవేళ (n >= 0) {
        ఫలితం.remove(0, n+1);
    }
    తిరిగి ఫలితం;
}

#నిర్వచితమైతే SERVICE_DEBUG
UnicodeString& 
ICUService_కీలకం::debug(UnicodeString& ఫలితం) స్థిరరాశి 
{
    debugClass(ఫలితం);
    ఫలితం.append((UnicodeString)" id: ");
    ఫలితం.append(_id);
    తిరిగి ఫలితం;
}

UnicodeString& 
ICUService_కీలకం::debugClass(UnicodeString& ఫలితం) స్థిరరాశి 
{
    తిరిగి ఫలితం.append((UnicodeString)"ICUServiceKey");
}
#అయితే_ముగింపు

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_కీలకం)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instance_గమ్యం_Adopt, స్థిరరాశి UnicodeString& id, UBool visible) 
: _instance(instance_గమ్యం_Adopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    తొలగించు _instance;
}

UObject* 
SimpleFactory::create(స్థిరరాశి ICUService_కీలకం& కీలకం, స్థిరరాశి ICUService* service, UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        UnicodeString తాత్కాలిక;
        ఒకవేళ (_id == కీలకం.currentID(తాత్కాలిక)) {
            తిరిగి service->cloneInstance(_instance); 
        }
    }
    తిరిగి శూన్యసూచిక;
}

శూన్య 
SimpleFactory::updateVisibleIDs(Hashtable& ఫలితం, UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    ఒకవేళ (_visible) {
        ఫలితం.put(_id, (శూన్య*)ఇది, స్థితి_2); // cast away const
    } లేకపోతే {
        ఫలితం.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(స్థిరరాశి UnicodeString& id, స్థిరరాశి Locale& /* locale */, UnicodeString& ఫలితం) స్థిరరాశి 
{
    ఒకవేళ (_visible && _id == id) {
        ఫలితం = _id;
    } లేకపోతే {
        ఫలితం.సమితి_గమ్యం_Bogus();
    }
    తిరిగి ఫలితం;
}

#నిర్వచితమైతే SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& గమ్యం_Append_గమ్యం) స్థిరరాశి 
{
    debugClass(గమ్యం_Append_గమ్యం);
    గమ్యం_Append_గమ్యం.append((UnicodeString)" id: ");
    గమ్యం_Append_గమ్యం.append(_id);
    గమ్యం_Append_గమ్యం.append((UnicodeString)", visible: ");
    గమ్యం_Append_గమ్యం.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    తిరిగి గమ్యం_Append_గమ్యం;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& గమ్యం_Append_గమ్యం) స్థిరరాశి 
{
    తిరిగి గమ్యం_Append_గమ్యం.append((UnicodeString)"SimpleFactory");
}
#అయితే_ముగింపు

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(SimpleFactory)

/*
******************************************************************
*/

ServiceListener::~ServiceListener() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceListener)

/*
******************************************************************
*/

// Record the actual id for this service in the cache, so we can return it
// even if we succeed later with a different id.
తరగతి Cache_నమోదు : బహిరంగ UMemory {
అంతరంగ:
    int32_t refcount;

బహిరంగ:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~Cache_నమోదు() {
        తొలగించు service;
    }

    Cache_నమోదు(స్థిరరాశి UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    Cache_నమోదు* ref() {
        ++refcount;
        తిరిగి ఇది;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    Cache_నమోదు* unref() {
        ఒకవేళ ((--refcount) == 0) {
            తొలగించు ఇది;
            తిరిగి శూన్యసూచిక;
        }
        తిరిగి ఇది;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() స్థిరరాశి {
        తిరిగి refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV
cacheDeleter(శూన్య* obj) {
    U_NAMESPACE_USE ((Cache_నమోదు*)obj)->unref();
}

U_CDECL_చివర

/*
******************************************************************
*/

తరగతి DNCache : బహిరంగ UMemory {
బహిరంగ:
    Hashtable cache;
    స్థిరరాశి Locale locale;

    DNCache(స్థిరరాశి Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(స్థిరరాశి UnicodeString& displayName, 
                   స్థిరరాశి UnicodeString& id,
                   UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        StringPair* sp = కొత్త StringPair(displayName, id);
        ఒకవేళ (sp == శూన్యసూచిక || sp->isBogus()) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తొలగించు sp;
            తిరిగి శూన్యసూచిక;
        }
        తిరిగి sp;
    }
    తిరిగి శూన్యసూచిక;
}

UBool 
StringPair::isBogus() స్థిరరాశి {
    తిరిగి displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(స్థిరరాశి UnicodeString& _displayName, 
                       స్థిరరాశి UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV
userv_deleteStringPair(శూన్య *obj) {
    U_NAMESPACE_USE తొలగించు (StringPair*) obj;
}
U_CDECL_చివర

/*
******************************************************************
*/

స్థిర UMutex తాళం;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(శూన్యసూచిక)
, serviceCache(శూన్యసూచిక)
, idCache(శూన్యసూచిక)
, dnCache(శూన్యసూచిక)
{
}

ICUService::ICUService(స్థిరరాశి UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(శూన్యసూచిక)
, serviceCache(శూన్యసూచిక)
, idCache(శూన్యసూచిక)
, dnCache(శూన్యసూచిక)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&తాళం);
        clearCaches();
        తొలగించు factories;
        factories = శూన్యసూచిక;
    }
}

UObject* 
ICUService::get(స్థిరరాశి UnicodeString& descriptor, UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    తిరిగి get(descriptor, శూన్యసూచిక, స్థితి_2);
}

UObject* 
ICUService::get(స్థిరరాశి UnicodeString& descriptor, UnicodeString* actualReturn, UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    UObject* ఫలితం = శూన్యసూచిక;
    ICUService_కీలకం* కీలకం = create_కీలకం(&descriptor, స్థితి_2);
    ఒకవేళ (కీలకం) {
        ఫలితం = get_కీలకం(*కీలకం, actualReturn, స్థితి_2);
        తొలగించు కీలకం;
    }
    తిరిగి ఫలితం;
}

UObject* 
ICUService::get_కీలకం(ICUService_కీలకం& కీలకం, UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    తిరిగి get_కీలకం(కీలకం, శూన్యసూచిక, స్థితి_2);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_కీలకం(ICUService_కీలకం& కీలకం, UnicodeString* actualReturn, UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    తిరిగి get_కీలకం(కీలకం, actualReturn, శూన్యసూచిక, స్థితి_2);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
తరగతి XMutex : బహిరంగ UMemory {
బహిరంగ:
    అంతఃవరుస XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , f_చురుకైన(!reentering) 
    {
        ఒకవేళ (f_చురుకైన) umtx_తాళం(fMutex);
    }
    అంతఃవరుస ~XMutex() {
        ఒకవేళ (f_చురుకైన) umtx_unlock(fMutex);
    }

అంతరంగ:
    UMutex  *fMutex;
    UBool f_చురుకైన;
};

// called only by factories, treat as private
UObject* 
ICUService::get_కీలకం(ICUService_కీలకం& కీలకం, UnicodeString* actualReturn, స్థిరరాశి ICUServiceFactory* factory, UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (isDefault()) {
        తిరిగి handleDefault(కీలకం, actualReturn, స్థితి_2);
    }

    ICUService* ncthis = const_cast<ICUService*>(ఇది); // cast away semantic const

    Cache_నమోదు* ఫలితం = శూన్యసూచిక;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&తాళం, factory != శూన్యసూచిక);

        ఒకవేళ (serviceCache == శూన్యసూచిక) {
            ncthis->serviceCache = కొత్త Hashtable(స్థితి_2);
            ఒకవేళ (ncthis->serviceCache == శూన్యసూచిక) {
                స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి శూన్యసూచిక;
            }
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తొలగించు serviceCache;
                తిరిగి శూన్యసూచిక;
            }
            serviceCache->సమితి_విలువ_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        Local_సూచిక<UVector> cacheDescriptorList;
        UBool put_లోపల_Cache = అబద్ధం;

        int32_t ప్రారంభం_సూచీ = 0;
        int32_t హద్దు = factories->size();
        UBool cache_ఫలితం = నిజం;

        ఒకవేళ (factory != శూన్యసూచిక) {
            కొరకు (int32_t i = 0; i < హద్దు; ++i) {
                ఒకవేళ (factory == static_cast<స్థిరరాశి ICUServiceFactory*>(factories->elementAt(i))) {
                    ప్రారంభం_సూచీ = i + 1;
                    ఆపు;
                }
            }
            ఒకవేళ (ప్రారంభం_సూచీ == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
                తిరిగి శూన్యసూచిక;
            }
            cache_ఫలితం = అబద్ధం;
        }

        చేయి {
            currentDescriptor.remove();
            కీలకం.currentDescriptor(currentDescriptor);
            ఫలితం = static_cast<Cache_నమోదు*>(serviceCache->get(currentDescriptor));
            ఒకవేళ (ఫలితం != శూన్యసూచిక) {
                ఆపు;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            put_లోపల_Cache = నిజం;

            int32_t సూచీ = ప్రారంభం_సూచీ;
            అయితే (సూచీ < హద్దు) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(సూచీ++));
                Local_సూచిక<UObject> service(f->create(కీలకం, ఇది, స్థితి_2));
                ఒకవేళ (U_FAILURE(స్థితి_2)) {
                    తిరిగి శూన్యసూచిక;
                }
                ఒకవేళ (service.isValid()) {
                    ఫలితం = కొత్త Cache_నమోదు(currentDescriptor, service.getAlias());
                    ఒకవేళ (ఫలితం == శూన్యసూచిక) {
                        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                        తిరిగి శూన్యసూచిక;
                    }
                    service.orphan(); // result now owns service.

                    వెళ్ళు outer_చివర;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            ఒకవేళ (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adopt_Instead_And_Check_Error_సంకేతం(కొత్త UVector(uprv_deleteUObject, శూన్యసూచిక, 5, స్థితి_2), స్థితి_2);
                ఒకవేళ (U_FAILURE(స్థితి_2)) {
                    తిరిగి శూన్యసూచిక;
                }
            }

            Local_సూచిక<UnicodeString> id_గమ్యం_Cache(కొత్త UnicodeString(currentDescriptor), స్థితి_2);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తిరిగి శూన్యసూచిక;
            }
            ఒకవేళ (id_గమ్యం_Cache->isBogus()) {
                స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి శూన్యసూచిక;
            }
            cacheDescriptorList->adoptElement(id_గమ్యం_Cache.orphan(), స్థితి_2);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తిరిగి శూన్యసూచిక;
            }
        } అయితే (కీలకం.fallback());
outer_చివర:

        ఒకవేళ (ఫలితం != శూన్యసూచిక) {
            ఒకవేళ (put_లోపల_Cache && cache_ఫలితం) {
                serviceCache->put(ఫలితం->actualDescriptor, ఫలితం, స్థితి_2);
                ఒకవేళ (U_FAILURE(స్థితి_2)) {
                    తిరిగి శూన్యసూచిక;
                }

                ఒకవేళ (cacheDescriptorList.isValid()) {
                    కొరకు (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, ఫలితం, స్థితి_2);
                        ఒకవేళ (U_FAILURE(స్థితి_2)) {
                            తిరిగి శూన్యసూచిక;
                        }

                        ఫలితం->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            ఒకవేళ (actualReturn != శూన్యసూచిక) {
                // strip null prefix
                ఒకవేళ (ఫలితం->actualDescriptor.సూచీ_యొక్క(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(ఫలితం->actualDescriptor, 
                        1, 
                        ఫలితం->actualDescriptor.length() - 1);
                } లేకపోతే {
                    *actualReturn = ఫలితం->actualDescriptor;
                }

                ఒకవేళ (actualReturn->isBogus()) {
                    స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    తొలగించు ఫలితం;
                    తిరిగి శూన్యసూచిక;
                }
            }

            UObject* service = cloneInstance(ఫలితం->service);
            ఒకవేళ (put_లోపల_Cache && !cache_ఫలితం) {
                తొలగించు ఫలితం;
            }
            తిరిగి service;
        }
    }

    తిరిగి handleDefault(కీలకం, actualReturn, స్థితి_2);
}

UObject* 
ICUService::handleDefault(స్థిరరాశి ICUService_కీలకం& /*కీలకం*/, UnicodeString* /* actualIDReturn */, UError_సంకేతం& /* status */) స్థిరరాశి 
{
    తిరిగి శూన్యసూచిక;
}

UVector& 
ICUService::getVisibleIDs(UVector& ఫలితం, UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    తిరిగి getVisibleIDs(ఫలితం, శూన్యసూచిక, స్థితి_2);
}

UVector& 
ICUService::getVisibleIDs(UVector& ఫలితం, స్థిరరాశి UnicodeString* సరిపోలిక_ID, UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    ఫలితం.removeAllElements();

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి ఫలితం;
    }
    UObjectDeleter *savedDeleter = ఫలితం.సమితి_Deleter(uprv_deleteUObject);

    {
        Mutex mutex(&తాళం);
        స్థిరరాశి Hashtable* map = getVisibleIDMap(స్థితి_2);
        ఒకవేళ (map != శూన్యసూచిక) {
            ICUService_కీలకం* fallback_కీలకం = create_కీలకం(సరిపోలిక_ID, స్థితి_2);

            కొరకు (int32_t pos = UHASH_మొదటి; U_SUCCESS(స్థితి_2); ) {
                స్థిరరాశి UHashElement* e = map->తదుపరి_Element(pos);
                ఒకవేళ (e == శూన్యసూచిక) {
                    ఆపు;
                }

                స్థిరరాశి UnicodeString* id = static_cast<స్థిరరాశి UnicodeString*>(e->కీలకం.సూచిక);
                ఒకవేళ (fallback_కీలకం != శూన్యసూచిక) {
                    ఒకవేళ (!fallback_కీలకం->is_Fallback_యొక్క(*id)) {
                        కొనసాగు;
                    }
                }

                Local_సూచిక<UnicodeString> idClone(id->clone(), స్థితి_2);
                ఫలితం.adoptElement(idClone.orphan(), స్థితి_2);
            }
            తొలగించు fallback_కీలకం;
        }
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        ఫలితం.removeAllElements();
    }
    ఫలితం.సమితి_Deleter(savedDeleter);
    తిరిగి ఫలితం;
}

స్థిరరాశి Hashtable* 
ICUService::getVisibleIDMap(UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి శూన్యసూచిక;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(ఇది); // cast away semantic const
    ఒకవేళ (idCache == శూన్యసూచిక) {
        ncthis->idCache = కొత్త Hashtable(స్థితి_2);
        ఒకవేళ (idCache == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే ఒకవేళ (factories != శూన్యసూచిక) {
            కొరకు (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, స్థితి_2);
            }
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తొలగించు idCache;
                ncthis->idCache = శూన్యసూచిక;
            }
        }
    }

    తిరిగి idCache;
}


UnicodeString& 
ICUService::getDisplayName(స్థిరరాశి UnicodeString& id, UnicodeString& ఫలితం) స్థిరరాశి 
{
    తిరిగి getDisplayName(id, ఫలితం, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(స్థిరరాశి UnicodeString& id, UnicodeString& ఫలితం, స్థిరరాశి Locale& locale) స్థిరరాశి 
{
    {
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        Mutex mutex(&తాళం);
        స్థిరరాశి Hashtable* map = getVisibleIDMap(స్థితి_2);
        ఒకవేళ (map != శూన్యసూచిక) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            ఒకవేళ (f != శూన్యసూచిక) {
                f->getDisplayName(id, locale, ఫలితం);
                తిరిగి ఫలితం;
            }

            // fallback
            స్థితి_2 = U_ZERO_ERROR;
            ICUService_కీలకం* fallback_కీలకం = create_కీలకం(&id, స్థితి_2);
            అయితే (fallback_కీలకం != శూన్యసూచిక && fallback_కీలకం->fallback()) {
                UnicodeString us;
                fallback_కీలకం->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                ఒకవేళ (f != శూన్యసూచిక) {
                    f->getDisplayName(id, locale, ఫలితం);
                    తొలగించు fallback_కీలకం;
                    తిరిగి ఫలితం;
                }
            }
            తొలగించు fallback_కీలకం;
        }
    }
    ఫలితం.సమితి_గమ్యం_Bogus();
    తిరిగి ఫలితం;
}

UVector& 
ICUService::get_Display_పేర్లు(UVector& ఫలితం, UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    తిరిగి get_Display_పేర్లు(ఫలితం, Locale::getDefault(), శూన్యసూచిక, స్థితి_2);
}


UVector& 
ICUService::get_Display_పేర్లు(UVector& ఫలితం, స్థిరరాశి Locale& locale, UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    తిరిగి get_Display_పేర్లు(ఫలితం, locale, శూన్యసూచిక, స్థితి_2);
}

UVector& 
ICUService::get_Display_పేర్లు(UVector& ఫలితం, 
                            స్థిరరాశి Locale& locale, 
                            స్థిరరాశి UnicodeString* సరిపోలిక_ID, 
                            UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    ఫలితం.removeAllElements();
    ఫలితం.సమితి_Deleter(userv_deleteStringPair);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        ICUService* ncthis = const_cast<ICUService*>(ఇది); // cast away semantic const
        Mutex mutex(&తాళం);

        ఒకవేళ (dnCache != శూన్యసూచిక && dnCache->locale != locale) {
            తొలగించు dnCache;
            ncthis->dnCache = శూన్యసూచిక;
        }

        ఒకవేళ (dnCache == శూన్యసూచిక) {
            స్థిరరాశి Hashtable* m = getVisibleIDMap(స్థితి_2);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తిరిగి ఫలితం;
            }
            ncthis->dnCache = కొత్త DNCache(locale); 
            ఒకవేళ (dnCache == శూన్యసూచిక) {
                స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి ఫలితం;
            }

            int32_t pos = UHASH_మొదటి;
            స్థిరరాశి UHashElement* నమోదు_2 = శూన్యసూచిక;
            అయితే ((నమోదు_2 = m->తదుపరి_Element(pos)) != శూన్యసూచిక) {
                స్థిరరాశి UnicodeString* id = static_cast<స్థిరరాశి UnicodeString*>(నమోదు_2->కీలకం.సూచిక);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(నమోదు_2->value.సూచిక);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                ఒకవేళ (dname.isBogus()) {
                    స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                } లేకపోతే {
                    dnCache->cache.put(dname, (శూన్య*)id, స్థితి_2); // share pointer with visibleIDMap
                    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
                        కొనసాగు;
                    }
                }
                తొలగించు dnCache;
                ncthis->dnCache = శూన్యసూచిక;
                తిరిగి ఫలితం;
            }
        }
    }

    ICUService_కీలకం* సరిపోలిక_కీలకం = create_కీలకం(సరిపోలిక_ID, స్థితి_2);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_మొదటి; 
    స్థిరరాశి UHashElement *నమోదు_2 = శూన్యసూచిక;
    అయితే ((నమోదు_2 = dnCache->cache.తదుపరి_Element(pos)) != శూన్యసూచిక) {
        స్థిరరాశి UnicodeString* id = static_cast<స్థిరరాశి UnicodeString*>(నమోదు_2->value.సూచిక);
        ఒకవేళ (సరిపోలిక_కీలకం != శూన్యసూచిక && !సరిపోలిక_కీలకం->is_Fallback_యొక్క(*id)) {
            కొనసాగు;
        }
        స్థిరరాశి UnicodeString* dn = static_cast<స్థిరరాశి UnicodeString*>(నమోదు_2->కీలకం.సూచిక);
        StringPair* sp = StringPair::create(*id, *dn, స్థితి_2);
        ఫలితం.adoptElement(sp, స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            ఫలితం.removeAllElements();
            ఆపు;
        }
    }
    తొలగించు సరిపోలిక_కీలకం;

    తిరిగి ఫలితం;
}

URegistry_కీలకం
ICUService::రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి UnicodeString& id, UError_సంకేతం& స్థితి_2) 
{
    తిరిగి రిజిస్టరు_Instance(obj_గమ్యం_Adopt, id, నిజం, స్థితి_2);
}

URegistry_కీలకం
ICUService::రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి UnicodeString& id, UBool visible, UError_సంకేతం& స్థితి_2) 
{
    ICUService_కీలకం* కీలకం = create_కీలకం(&id, స్థితి_2);
    ఒకవేళ (కీలకం != శూన్యసూచిక) {
        UnicodeString canonicalID;
        కీలకం->canonicalID(canonicalID);
        తొలగించు కీలకం;

        ICUServiceFactory* f = createSimpleFactory(obj_గమ్యం_Adopt, canonicalID, visible, స్థితి_2);
        ఒకవేళ (f != శూన్యసూచిక) {
            తిరిగి రిజిస్టరు_Factory(f, స్థితి_2);
        }
    }
    తొలగించు obj_గమ్యం_Adopt;
    తిరిగి శూన్యసూచిక;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* obj_గమ్యం_Adopt, స్థిరరాశి UnicodeString& id, UBool visible, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        ఒకవేళ ((obj_గమ్యం_Adopt != శూన్యసూచిక) && (!id.isBogus())) {
            తిరిగి కొత్త SimpleFactory(obj_గమ్యం_Adopt, id, visible);
        }
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    }
    తిరిగి శూన్యసూచిక;
}

URegistry_కీలకం
ICUService::రిజిస్టరు_Factory(ICUServiceFactory* factory_గమ్యం_Adopt, UError_సంకేతం& స్థితి_2)
{
    Local_సూచిక<ICUServiceFactory>lp_Factory_గమ్యం_Adopt(factory_గమ్యం_Adopt);
    ఒకవేళ (U_FAILURE(స్థితి_2) || factory_గమ్యం_Adopt == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }
    {
        Mutex mutex(&తాళం);

        ఒకవేళ (factories == శూన్యసూచిక) {
            Local_సూచిక<UVector> lpFactories(కొత్త UVector(uprv_deleteUObject, శూన్యసూచిక, స్థితి_2), స్థితి_2);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తిరిగి శూన్యసూచిక;
            }
            factories = lpFactories.orphan();
        }
        factories->చొప్పించు_Element_At(lp_Factory_గమ్యం_Adopt.orphan(), 0, స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        notify_మారింది();
        తిరిగి (URegistry_కీలకం)factory_గమ్యం_Adopt;
    } లేకపోతే {
        తిరిగి శూన్యసూచిక;
    }
}

UBool 
ICUService::unregister(URegistry_కీలకం rkey, UError_సంకేతం& స్థితి_2) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool ఫలితం = అబద్ధం;
    ఒకవేళ (factory != శూన్యసూచిక && factories != శూన్యసూచిక) {
        Mutex mutex(&తాళం);

        ఒకవేళ (factories->removeElement(factory)) {
            clearCaches();
            ఫలితం = నిజం;
        } లేకపోతే {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తొలగించు factory;
        }
    }
    ఒకవేళ (ఫలితం) {
        notify_మారింది();
    }
    తిరిగి ఫలితం;
}

శూన్య 
ICUService::reset() 
{
    {
        Mutex mutex(&తాళం);
        reInitializeFactories();
        clearCaches();
    }
    notify_మారింది();
}

శూన్య 
ICUService::reInitializeFactories() 
{
    ఒకవేళ (factories != శూన్యసూచిక) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() స్థిరరాశి 
{
    తిరిగి లెక్క_Factories() == 0;
}

ICUService_కీలకం* 
ICUService::create_కీలకం(స్థిరరాశి UnicodeString* id, UError_సంకేతం& స్థితి_2) స్థిరరాశి 
{
    తిరిగి (U_FAILURE(స్థితి_2) || id == శూన్యసూచిక) ? శూన్యసూచిక : కొత్త ICUService_కీలకం(*id);
}

శూన్య 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    తొలగించు dnCache;
    dnCache = శూన్యసూచిక;
    తొలగించు idCache;
    idCache = శూన్యసూచిక;
    తొలగించు serviceCache; serviceCache = శూన్యసూచిక;
}

శూన్య 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    తొలగించు serviceCache; serviceCache = శూన్యసూచిక;
}

UBool 
ICUService::స్వీకరించు_Listener(స్థిరరాశి EventListener& l) స్థిరరాశి 
{
    తిరిగి dynamic_cast<స్థిరరాశి ServiceListener*>(&l) != శూన్యసూచిక;
}

శూన్య 
ICUService::notifyListener(EventListener& l) స్థిరరాశి 
{
    (static_cast<ServiceListener&>(l)).service_మారింది(*ఇది);
}

UnicodeString&
ICUService::getName(UnicodeString& ఫలితం) స్థిరరాశి 
{
    తిరిగి ఫలితం.append(name);
}

int32_t 
ICUService::లెక్క_Factories() స్థిరరాశి 
{
    తిరిగి factories == శూన్యసూచిక ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() స్థిరరాశి
{
    తిరిగి timestamp;
}

U_NAMESPACE_చివర

/* UCONFIG_NO_SERVICE */
#అయితే_ముగింపు
