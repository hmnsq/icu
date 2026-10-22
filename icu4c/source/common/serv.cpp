// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localpointer.h"

#ਜੇ !UCONFIG_NO_SERVICE

#ਅਨ੍ਤਰ੍ਭਾਵ "serv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ਅਚਲ char16_t ICUService_ਕੁੰਜੀ::ਅਗੇਤਰ_DELIMITER = 0x002F;   /* '/' */

ICUService_ਕੁੰਜੀ::ICUService_ਕੁੰਜੀ(ਅਚਲ UnicodeString& id) 
: _id(id) {
}

ICUService_ਕੁੰਜੀ::~ICUService_ਕੁੰਜੀ() 
{
}

ਅਚਲ UnicodeString& 
ICUService_ਕੁੰਜੀ::getID() ਅਚਲ 
{
    ਵਾਪਸ _id;
}

UnicodeString& 
ICUService_ਕੁੰਜੀ::canonicalID(UnicodeString& ਨਤੀਜਾ) ਅਚਲ 
{
    ਵਾਪਸ ਨਤੀਜਾ.append(_id);
}

UnicodeString& 
ICUService_ਕੁੰਜੀ::currentID(UnicodeString& ਨਤੀਜਾ) ਅਚਲ 
{
    ਵਾਪਸ canonicalID(ਨਤੀਜਾ);
}

UnicodeString& 
ICUService_ਕੁੰਜੀ::currentDescriptor(UnicodeString& ਨਤੀਜਾ) ਅਚਲ 
{
    ਅਗੇਤਰ(ਨਤੀਜਾ);
    ਨਤੀਜਾ.append(ਅਗੇਤਰ_DELIMITER);
    ਵਾਪਸ currentID(ਨਤੀਜਾ);
}

UBool 
ICUService_ਕੁੰਜੀ::fallback() 
{
    ਵਾਪਸ ਝੂਠ;
}

UBool 
ICUService_ਕੁੰਜੀ::is_Fallback_ਦਾ(ਅਚਲ UnicodeString& id) ਅਚਲ 
{
    ਵਾਪਸ id == _id;
}

UnicodeString& 
ICUService_ਕੁੰਜੀ::ਅਗੇਤਰ(UnicodeString& ਨਤੀਜਾ) ਅਚਲ 
{
    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString& 
ICUService_ਕੁੰਜੀ::parse_ਅਗੇਤਰ(UnicodeString& ਨਤੀਜਾ) 
{
    int32_t n = ਨਤੀਜਾ.ਸੂਚਕਾਂਕ_ਦਾ(ਅਗੇਤਰ_DELIMITER);
    ਜੇ (n < 0) {
        n = 0;
    }
    ਨਤੀਜਾ.remove(n);
    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString& 
ICUService_ਕੁੰਜੀ::parseSuffix(UnicodeString& ਨਤੀਜਾ) 
{
    int32_t n = ਨਤੀਜਾ.ਸੂਚਕਾਂਕ_ਦਾ(ਅਗੇਤਰ_DELIMITER);
    ਜੇ (n >= 0) {
        ਨਤੀਜਾ.remove(0, n+1);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SERVICE_DEBUG
UnicodeString& 
ICUService_ਕੁੰਜੀ::debug(UnicodeString& ਨਤੀਜਾ) ਅਚਲ 
{
    debugClass(ਨਤੀਜਾ);
    ਨਤੀਜਾ.append((UnicodeString)" id: ");
    ਨਤੀਜਾ.append(_id);
    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString& 
ICUService_ਕੁੰਜੀ::debugClass(UnicodeString& ਨਤੀਜਾ) ਅਚਲ 
{
    ਵਾਪਸ ਨਤੀਜਾ.append((UnicodeString)"ICUServiceKey");
}
#ਜੇ_ਅੰਤ

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_ਕੁੰਜੀ)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instance_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ UnicodeString& id, UBool visible) 
: _instance(instance_ਮੰਜ਼ਿਲ_Adopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    ਮਿਟਾਓ _instance;
}

UObject* 
SimpleFactory::create(ਅਚਲ ICUService_ਕੁੰਜੀ& ਕੁੰਜੀ, ਅਚਲ ICUService* service, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        UnicodeString ਆਰਜ਼ੀ;
        ਜੇ (_id == ਕੁੰਜੀ.currentID(ਆਰਜ਼ੀ)) {
            ਵਾਪਸ service->cloneInstance(_instance); 
        }
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਖਾਲੀ 
SimpleFactory::updateVisibleIDs(Hashtable& ਨਤੀਜਾ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    ਜੇ (_visible) {
        ਨਤੀਜਾ.put(_id, (ਖਾਲੀ*)ਇਹ, ਸਥਿਤੀ); // cast away const
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ਅਚਲ UnicodeString& id, ਅਚਲ Locale& /* locale */, UnicodeString& ਨਤੀਜਾ) ਅਚਲ 
{
    ਜੇ (_visible && _id == id) {
        ਨਤੀਜਾ = _id;
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& ਮੰਜ਼ਿਲ_Append_ਮੰਜ਼ਿਲ) ਅਚਲ 
{
    debugClass(ਮੰਜ਼ਿਲ_Append_ਮੰਜ਼ਿਲ);
    ਮੰਜ਼ਿਲ_Append_ਮੰਜ਼ਿਲ.append((UnicodeString)" id: ");
    ਮੰਜ਼ਿਲ_Append_ਮੰਜ਼ਿਲ.append(_id);
    ਮੰਜ਼ਿਲ_Append_ਮੰਜ਼ਿਲ.append((UnicodeString)", visible: ");
    ਮੰਜ਼ਿਲ_Append_ਮੰਜ਼ਿਲ.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    ਵਾਪਸ ਮੰਜ਼ਿਲ_Append_ਮੰਜ਼ਿਲ;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& ਮੰਜ਼ਿਲ_Append_ਮੰਜ਼ਿਲ) ਅਚਲ 
{
    ਵਾਪਸ ਮੰਜ਼ਿਲ_Append_ਮੰਜ਼ਿਲ.append((UnicodeString)"SimpleFactory");
}
#ਜੇ_ਅੰਤ

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
ਵਰਗ Cache_ਇੰਦਰਾਜ : ਜਨਤਕ UMemory {
ਨਿਜੀ:
    int32_t refcount;

ਜਨਤਕ:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~Cache_ਇੰਦਰਾਜ() {
        ਮਿਟਾਓ service;
    }

    Cache_ਇੰਦਰਾਜ(ਅਚਲ UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    Cache_ਇੰਦਰਾਜ* ref() {
        ++refcount;
        ਵਾਪਸ ਇਹ;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    Cache_ਇੰਦਰਾਜ* unref() {
        ਜੇ ((--refcount) == 0) {
            ਮਿਟਾਓ ਇਹ;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਵਾਪਸ ਇਹ;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ਅਚਲ {
        ਵਾਪਸ refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
cacheDeleter(ਖਾਲੀ* obj) {
    U_NAMESPACE_USE ((Cache_ਇੰਦਰਾਜ*)obj)->unref();
}

U_CDECL_ਸਿਰਾ

/*
******************************************************************
*/

ਵਰਗ DNCache : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    Hashtable cache;
    ਅਚਲ Locale locale;

    DNCache(ਅਚਲ Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ਅਚਲ UnicodeString& displayName, 
                   ਅਚਲ UnicodeString& id,
                   UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        StringPair* sp = ਨਵਾਂ StringPair(displayName, id);
        ਜੇ (sp == ਖਾਲੀ_ਸੂਚਕ || sp->isBogus()) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਮਿਟਾਓ sp;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਵਾਪਸ sp;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

UBool 
StringPair::isBogus() ਅਚਲ {
    ਵਾਪਸ displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ਅਚਲ UnicodeString& _displayName, 
                       ਅਚਲ UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
userv_deleteStringPair(ਖਾਲੀ *obj) {
    U_NAMESPACE_USE ਮਿਟਾਓ (StringPair*) obj;
}
U_CDECL_ਸਿਰਾ

/*
******************************************************************
*/

ਸਥਿਰ UMutex ਤਾਲਾ;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(ਖਾਲੀ_ਸੂਚਕ)
, serviceCache(ਖਾਲੀ_ਸੂਚਕ)
, idCache(ਖਾਲੀ_ਸੂਚਕ)
, dnCache(ਖਾਲੀ_ਸੂਚਕ)
{
}

ICUService::ICUService(ਅਚਲ UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(ਖਾਲੀ_ਸੂਚਕ)
, serviceCache(ਖਾਲੀ_ਸੂਚਕ)
, idCache(ਖਾਲੀ_ਸੂਚਕ)
, dnCache(ਖਾਲੀ_ਸੂਚਕ)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&ਤਾਲਾ);
        clearCaches();
        ਮਿਟਾਓ factories;
        factories = ਖਾਲੀ_ਸੂਚਕ;
    }
}

UObject* 
ICUService::get(ਅਚਲ UnicodeString& descriptor, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    ਵਾਪਸ get(descriptor, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
}

UObject* 
ICUService::get(ਅਚਲ UnicodeString& descriptor, UnicodeString* actualReturn, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    UObject* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    ICUService_ਕੁੰਜੀ* ਕੁੰਜੀ = create_ਕੁੰਜੀ(&descriptor, ਸਥਿਤੀ);
    ਜੇ (ਕੁੰਜੀ) {
        ਨਤੀਜਾ = get_ਕੁੰਜੀ(*ਕੁੰਜੀ, actualReturn, ਸਥਿਤੀ);
        ਮਿਟਾਓ ਕੁੰਜੀ;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

UObject* 
ICUService::get_ਕੁੰਜੀ(ICUService_ਕੁੰਜੀ& ਕੁੰਜੀ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    ਵਾਪਸ get_ਕੁੰਜੀ(ਕੁੰਜੀ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_ਕੁੰਜੀ(ICUService_ਕੁੰਜੀ& ਕੁੰਜੀ, UnicodeString* actualReturn, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    ਵਾਪਸ get_ਕੁੰਜੀ(ਕੁੰਜੀ, actualReturn, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
ਵਰਗ XMutex : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    ਸਤਰਅੰਦਰ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , f_ਸਰਗਰਮ(!reentering) 
    {
        ਜੇ (f_ਸਰਗਰਮ) umtx_ਤਾਲਾ(fMutex);
    }
    ਸਤਰਅੰਦਰ ~XMutex() {
        ਜੇ (f_ਸਰਗਰਮ) umtx_unlock(fMutex);
    }

ਨਿਜੀ:
    UMutex  *fMutex;
    UBool f_ਸਰਗਰਮ;
};

// called only by factories, treat as private
UObject* 
ICUService::get_ਕੁੰਜੀ(ICUService_ਕੁੰਜੀ& ਕੁੰਜੀ, UnicodeString* actualReturn, ਅਚਲ ICUServiceFactory* factory, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (isDefault()) {
        ਵਾਪਸ handleDefault(ਕੁੰਜੀ, actualReturn, ਸਥਿਤੀ);
    }

    ICUService* ncthis = const_cast<ICUService*>(ਇਹ); // cast away semantic const

    Cache_ਇੰਦਰਾਜ* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&ਤਾਲਾ, factory != ਖਾਲੀ_ਸੂਚਕ);

        ਜੇ (serviceCache == ਖਾਲੀ_ਸੂਚਕ) {
            ncthis->serviceCache = ਨਵਾਂ Hashtable(ਸਥਿਤੀ);
            ਜੇ (ncthis->serviceCache == ਖਾਲੀ_ਸੂਚਕ) {
                ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਮਿਟਾਓ serviceCache;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            serviceCache->ਸੈੱਟ_ਮੁੱਲ_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        Local_ਸੂਚਕ<UVector> cacheDescriptorList;
        UBool put_ਵਿੱਚ_Cache = ਝੂਠ;

        int32_t ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0;
        int32_t ਹੱਦਬੰਦੀ = factories->size();
        UBool cache_ਨਤੀਜਾ = ਸੱਚ;

        ਜੇ (factory != ਖਾਲੀ_ਸੂਚਕ) {
            ਲਈ (int32_t i = 0; i < ਹੱਦਬੰਦੀ; ++i) {
                ਜੇ (factory == static_cast<ਅਚਲ ICUServiceFactory*>(factories->elementAt(i))) {
                    ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = i + 1;
                    ਰੋਕੋ;
                }
            }
            ਜੇ (ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            cache_ਨਤੀਜਾ = ਝੂਠ;
        }

        ਕਰੋ {
            currentDescriptor.remove();
            ਕੁੰਜੀ.currentDescriptor(currentDescriptor);
            ਨਤੀਜਾ = static_cast<Cache_ਇੰਦਰਾਜ*>(serviceCache->get(currentDescriptor));
            ਜੇ (ਨਤੀਜਾ != ਖਾਲੀ_ਸੂਚਕ) {
                ਰੋਕੋ;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            put_ਵਿੱਚ_Cache = ਸੱਚ;

            int32_t ਸੂਚਕਾਂਕ = ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ;
            ਜਦੋਂ (ਸੂਚਕਾਂਕ < ਹੱਦਬੰਦੀ) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(ਸੂਚਕਾਂਕ++));
                Local_ਸੂਚਕ<UObject> service(f->create(ਕੁੰਜੀ, ਇਹ, ਸਥਿਤੀ));
                ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }
                ਜੇ (service.isValid()) {
                    ਨਤੀਜਾ = ਨਵਾਂ Cache_ਇੰਦਰਾਜ(currentDescriptor, service.getAlias());
                    ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
                        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                    }
                    service.orphan(); // result now owns service.

                    ਜਾਓ outer_ਸਿਰਾ;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            ਜੇ (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adopt_Instead_And_Check_Error_ਕੋਡ(ਨਵਾਂ UVector(uprv_deleteUObject, ਖਾਲੀ_ਸੂਚਕ, 5, ਸਥਿਤੀ), ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }
            }

            Local_ਸੂਚਕ<UnicodeString> id_ਮੰਜ਼ਿਲ_Cache(ਨਵਾਂ UnicodeString(currentDescriptor), ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            ਜੇ (id_ਮੰਜ਼ਿਲ_Cache->isBogus()) {
                ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            cacheDescriptorList->adoptElement(id_ਮੰਜ਼ਿਲ_Cache.orphan(), ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
        } ਜਦੋਂ (ਕੁੰਜੀ.fallback());
outer_ਸਿਰਾ:

        ਜੇ (ਨਤੀਜਾ != ਖਾਲੀ_ਸੂਚਕ) {
            ਜੇ (put_ਵਿੱਚ_Cache && cache_ਨਤੀਜਾ) {
                serviceCache->put(ਨਤੀਜਾ->actualDescriptor, ਨਤੀਜਾ, ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }

                ਜੇ (cacheDescriptorList.isValid()) {
                    ਲਈ (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, ਨਤੀਜਾ, ਸਥਿਤੀ);
                        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                        }

                        ਨਤੀਜਾ->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            ਜੇ (actualReturn != ਖਾਲੀ_ਸੂਚਕ) {
                // strip null prefix
                ਜੇ (ਨਤੀਜਾ->actualDescriptor.ਸੂਚਕਾਂਕ_ਦਾ(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(ਨਤੀਜਾ->actualDescriptor, 
                        1, 
                        ਨਤੀਜਾ->actualDescriptor.length() - 1);
                } ਨਹੀਂਤਾਂ {
                    *actualReturn = ਨਤੀਜਾ->actualDescriptor;
                }

                ਜੇ (actualReturn->isBogus()) {
                    ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਮਿਟਾਓ ਨਤੀਜਾ;
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }
            }

            UObject* service = cloneInstance(ਨਤੀਜਾ->service);
            ਜੇ (put_ਵਿੱਚ_Cache && !cache_ਨਤੀਜਾ) {
                ਮਿਟਾਓ ਨਤੀਜਾ;
            }
            ਵਾਪਸ service;
        }
    }

    ਵਾਪਸ handleDefault(ਕੁੰਜੀ, actualReturn, ਸਥਿਤੀ);
}

UObject* 
ICUService::handleDefault(ਅਚਲ ICUService_ਕੁੰਜੀ& /*ਕੁੰਜੀ*/, UnicodeString* /* actualIDReturn */, UError_ਕੋਡ& /* status */) ਅਚਲ 
{
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

UVector& 
ICUService::getVisibleIDs(UVector& ਨਤੀਜਾ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਵਾਪਸ getVisibleIDs(ਨਤੀਜਾ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
}

UVector& 
ICUService::getVisibleIDs(UVector& ਨਤੀਜਾ, ਅਚਲ UnicodeString* ਮੇਲ_ID, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    ਨਤੀਜਾ.removeAllElements();

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    UObjectDeleter *savedDeleter = ਨਤੀਜਾ.ਸੈੱਟ_Deleter(uprv_deleteUObject);

    {
        Mutex mutex(&ਤਾਲਾ);
        ਅਚਲ Hashtable* map = getVisibleIDMap(ਸਥਿਤੀ);
        ਜੇ (map != ਖਾਲੀ_ਸੂਚਕ) {
            ICUService_ਕੁੰਜੀ* fallback_ਕੁੰਜੀ = create_ਕੁੰਜੀ(ਮੇਲ_ID, ਸਥਿਤੀ);

            ਲਈ (int32_t pos = UHASH_ਪਹਿਲਾ; U_SUCCESS(ਸਥਿਤੀ); ) {
                ਅਚਲ UHashElement* e = map->ਅਗਲਾ_Element(pos);
                ਜੇ (e == ਖਾਲੀ_ਸੂਚਕ) {
                    ਰੋਕੋ;
                }

                ਅਚਲ UnicodeString* id = static_cast<ਅਚਲ UnicodeString*>(e->ਕੁੰਜੀ.ਸੂਚਕ);
                ਜੇ (fallback_ਕੁੰਜੀ != ਖਾਲੀ_ਸੂਚਕ) {
                    ਜੇ (!fallback_ਕੁੰਜੀ->is_Fallback_ਦਾ(*id)) {
                        ਜਾਰੀ;
                    }
                }

                Local_ਸੂਚਕ<UnicodeString> idClone(id->clone(), ਸਥਿਤੀ);
                ਨਤੀਜਾ.adoptElement(idClone.orphan(), ਸਥਿਤੀ);
            }
            ਮਿਟਾਓ fallback_ਕੁੰਜੀ;
        }
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਨਤੀਜਾ.removeAllElements();
    }
    ਨਤੀਜਾ.ਸੈੱਟ_Deleter(savedDeleter);
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਅਚਲ Hashtable* 
ICUService::getVisibleIDMap(UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(ਇਹ); // cast away semantic const
    ਜੇ (idCache == ਖਾਲੀ_ਸੂਚਕ) {
        ncthis->idCache = ਨਵਾਂ Hashtable(ਸਥਿਤੀ);
        ਜੇ (idCache == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ ਜੇ (factories != ਖਾਲੀ_ਸੂਚਕ) {
            ਲਈ (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, ਸਥਿਤੀ);
            }
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਮਿਟਾਓ idCache;
                ncthis->idCache = ਖਾਲੀ_ਸੂਚਕ;
            }
        }
    }

    ਵਾਪਸ idCache;
}


UnicodeString& 
ICUService::getDisplayName(ਅਚਲ UnicodeString& id, UnicodeString& ਨਤੀਜਾ) ਅਚਲ 
{
    ਵਾਪਸ getDisplayName(id, ਨਤੀਜਾ, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ਅਚਲ UnicodeString& id, UnicodeString& ਨਤੀਜਾ, ਅਚਲ Locale& locale) ਅਚਲ 
{
    {
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        Mutex mutex(&ਤਾਲਾ);
        ਅਚਲ Hashtable* map = getVisibleIDMap(ਸਥਿਤੀ);
        ਜੇ (map != ਖਾਲੀ_ਸੂਚਕ) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            ਜੇ (f != ਖਾਲੀ_ਸੂਚਕ) {
                f->getDisplayName(id, locale, ਨਤੀਜਾ);
                ਵਾਪਸ ਨਤੀਜਾ;
            }

            // fallback
            ਸਥਿਤੀ = U_ZERO_ERROR;
            ICUService_ਕੁੰਜੀ* fallback_ਕੁੰਜੀ = create_ਕੁੰਜੀ(&id, ਸਥਿਤੀ);
            ਜਦੋਂ (fallback_ਕੁੰਜੀ != ਖਾਲੀ_ਸੂਚਕ && fallback_ਕੁੰਜੀ->fallback()) {
                UnicodeString us;
                fallback_ਕੁੰਜੀ->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                ਜੇ (f != ਖਾਲੀ_ਸੂਚਕ) {
                    f->getDisplayName(id, locale, ਨਤੀਜਾ);
                    ਮਿਟਾਓ fallback_ਕੁੰਜੀ;
                    ਵਾਪਸ ਨਤੀਜਾ;
                }
            }
            ਮਿਟਾਓ fallback_ਕੁੰਜੀ;
        }
    }
    ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ ਨਤੀਜਾ;
}

UVector& 
ICUService::get_Display_ਨਾਂ(UVector& ਨਤੀਜਾ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    ਵਾਪਸ get_Display_ਨਾਂ(ਨਤੀਜਾ, Locale::getDefault(), ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
}


UVector& 
ICUService::get_Display_ਨਾਂ(UVector& ਨਤੀਜਾ, ਅਚਲ Locale& locale, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    ਵਾਪਸ get_Display_ਨਾਂ(ਨਤੀਜਾ, locale, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
}

UVector& 
ICUService::get_Display_ਨਾਂ(UVector& ਨਤੀਜਾ, 
                            ਅਚਲ Locale& locale, 
                            ਅਚਲ UnicodeString* ਮੇਲ_ID, 
                            UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    ਨਤੀਜਾ.removeAllElements();
    ਨਤੀਜਾ.ਸੈੱਟ_Deleter(userv_deleteStringPair);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ICUService* ncthis = const_cast<ICUService*>(ਇਹ); // cast away semantic const
        Mutex mutex(&ਤਾਲਾ);

        ਜੇ (dnCache != ਖਾਲੀ_ਸੂਚਕ && dnCache->locale != locale) {
            ਮਿਟਾਓ dnCache;
            ncthis->dnCache = ਖਾਲੀ_ਸੂਚਕ;
        }

        ਜੇ (dnCache == ਖਾਲੀ_ਸੂਚਕ) {
            ਅਚਲ Hashtable* m = getVisibleIDMap(ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਵਾਪਸ ਨਤੀਜਾ;
            }
            ncthis->dnCache = ਨਵਾਂ DNCache(locale); 
            ਜੇ (dnCache == ਖਾਲੀ_ਸੂਚਕ) {
                ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ ਨਤੀਜਾ;
            }

            int32_t pos = UHASH_ਪਹਿਲਾ;
            ਅਚਲ UHashElement* ਇੰਦਰਾਜ = ਖਾਲੀ_ਸੂਚਕ;
            ਜਦੋਂ ((ਇੰਦਰਾਜ = m->ਅਗਲਾ_Element(pos)) != ਖਾਲੀ_ਸੂਚਕ) {
                ਅਚਲ UnicodeString* id = static_cast<ਅਚਲ UnicodeString*>(ਇੰਦਰਾਜ->ਕੁੰਜੀ.ਸੂਚਕ);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(ਇੰਦਰਾਜ->value.ਸੂਚਕ);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                ਜੇ (dname.isBogus()) {
                    ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                } ਨਹੀਂਤਾਂ {
                    dnCache->cache.put(dname, (ਖਾਲੀ*)id, ਸਥਿਤੀ); // share pointer with visibleIDMap
                    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
                        ਜਾਰੀ;
                    }
                }
                ਮਿਟਾਓ dnCache;
                ncthis->dnCache = ਖਾਲੀ_ਸੂਚਕ;
                ਵਾਪਸ ਨਤੀਜਾ;
            }
        }
    }

    ICUService_ਕੁੰਜੀ* ਮੇਲ_ਕੁੰਜੀ = create_ਕੁੰਜੀ(ਮੇਲ_ID, ਸਥਿਤੀ);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_ਪਹਿਲਾ; 
    ਅਚਲ UHashElement *ਇੰਦਰਾਜ = ਖਾਲੀ_ਸੂਚਕ;
    ਜਦੋਂ ((ਇੰਦਰਾਜ = dnCache->cache.ਅਗਲਾ_Element(pos)) != ਖਾਲੀ_ਸੂਚਕ) {
        ਅਚਲ UnicodeString* id = static_cast<ਅਚਲ UnicodeString*>(ਇੰਦਰਾਜ->value.ਸੂਚਕ);
        ਜੇ (ਮੇਲ_ਕੁੰਜੀ != ਖਾਲੀ_ਸੂਚਕ && !ਮੇਲ_ਕੁੰਜੀ->is_Fallback_ਦਾ(*id)) {
            ਜਾਰੀ;
        }
        ਅਚਲ UnicodeString* dn = static_cast<ਅਚਲ UnicodeString*>(ਇੰਦਰਾਜ->ਕੁੰਜੀ.ਸੂਚਕ);
        StringPair* sp = StringPair::create(*id, *dn, ਸਥਿਤੀ);
        ਨਤੀਜਾ.adoptElement(sp, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਨਤੀਜਾ.removeAllElements();
            ਰੋਕੋ;
        }
    }
    ਮਿਟਾਓ ਮੇਲ_ਕੁੰਜੀ;

    ਵਾਪਸ ਨਤੀਜਾ;
}

URegistry_ਕੁੰਜੀ
ICUService::ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ UnicodeString& id, UError_ਕੋਡ& ਸਥਿਤੀ) 
{
    ਵਾਪਸ ਰਜਿਸਟਰ_Instance(obj_ਮੰਜ਼ਿਲ_Adopt, id, ਸੱਚ, ਸਥਿਤੀ);
}

URegistry_ਕੁੰਜੀ
ICUService::ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ UnicodeString& id, UBool visible, UError_ਕੋਡ& ਸਥਿਤੀ) 
{
    ICUService_ਕੁੰਜੀ* ਕੁੰਜੀ = create_ਕੁੰਜੀ(&id, ਸਥਿਤੀ);
    ਜੇ (ਕੁੰਜੀ != ਖਾਲੀ_ਸੂਚਕ) {
        UnicodeString canonicalID;
        ਕੁੰਜੀ->canonicalID(canonicalID);
        ਮਿਟਾਓ ਕੁੰਜੀ;

        ICUServiceFactory* f = createSimpleFactory(obj_ਮੰਜ਼ਿਲ_Adopt, canonicalID, visible, ਸਥਿਤੀ);
        ਜੇ (f != ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ ਰਜਿਸਟਰ_Factory(f, ਸਥਿਤੀ);
        }
    }
    ਮਿਟਾਓ obj_ਮੰਜ਼ਿਲ_Adopt;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ UnicodeString& id, UBool visible, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਜੇ ((obj_ਮੰਜ਼ਿਲ_Adopt != ਖਾਲੀ_ਸੂਚਕ) && (!id.isBogus())) {
            ਵਾਪਸ ਨਵਾਂ SimpleFactory(obj_ਮੰਜ਼ਿਲ_Adopt, id, visible);
        }
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

URegistry_ਕੁੰਜੀ
ICUService::ਰਜਿਸਟਰ_Factory(ICUServiceFactory* factory_ਮੰਜ਼ਿਲ_Adopt, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    Local_ਸੂਚਕ<ICUServiceFactory>lp_Factory_ਮੰਜ਼ਿਲ_Adopt(factory_ਮੰਜ਼ਿਲ_Adopt);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ) || factory_ਮੰਜ਼ਿਲ_Adopt == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    {
        Mutex mutex(&ਤਾਲਾ);

        ਜੇ (factories == ਖਾਲੀ_ਸੂਚਕ) {
            Local_ਸੂਚਕ<UVector> lpFactories(ਨਵਾਂ UVector(uprv_deleteUObject, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ), ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            factories = lpFactories.orphan();
        }
        factories->ਪਾਓ_Element_At(lp_Factory_ਮੰਜ਼ਿਲ_Adopt.orphan(), 0, ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        notify_ਬਦਲਿਆ();
        ਵਾਪਸ (URegistry_ਕੁੰਜੀ)factory_ਮੰਜ਼ਿਲ_Adopt;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

UBool 
ICUService::unregister(URegistry_ਕੁੰਜੀ rkey, UError_ਕੋਡ& ਸਥਿਤੀ) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool ਨਤੀਜਾ = ਝੂਠ;
    ਜੇ (factory != ਖਾਲੀ_ਸੂਚਕ && factories != ਖਾਲੀ_ਸੂਚਕ) {
        Mutex mutex(&ਤਾਲਾ);

        ਜੇ (factories->removeElement(factory)) {
            clearCaches();
            ਨਤੀਜਾ = ਸੱਚ;
        } ਨਹੀਂਤਾਂ {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਮਿਟਾਓ factory;
        }
    }
    ਜੇ (ਨਤੀਜਾ) {
        notify_ਬਦਲਿਆ();
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ 
ICUService::reset() 
{
    {
        Mutex mutex(&ਤਾਲਾ);
        reInitializeFactories();
        clearCaches();
    }
    notify_ਬਦਲਿਆ();
}

ਖਾਲੀ 
ICUService::reInitializeFactories() 
{
    ਜੇ (factories != ਖਾਲੀ_ਸੂਚਕ) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ਅਚਲ 
{
    ਵਾਪਸ ਗਿਣਤੀ_Factories() == 0;
}

ICUService_ਕੁੰਜੀ* 
ICUService::create_ਕੁੰਜੀ(ਅਚਲ UnicodeString* id, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ 
{
    ਵਾਪਸ (U_FAILURE(ਸਥਿਤੀ) || id == ਖਾਲੀ_ਸੂਚਕ) ? ਖਾਲੀ_ਸੂਚਕ : ਨਵਾਂ ICUService_ਕੁੰਜੀ(*id);
}

ਖਾਲੀ 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    ਮਿਟਾਓ dnCache;
    dnCache = ਖਾਲੀ_ਸੂਚਕ;
    ਮਿਟਾਓ idCache;
    idCache = ਖਾਲੀ_ਸੂਚਕ;
    ਮਿਟਾਓ serviceCache; serviceCache = ਖਾਲੀ_ਸੂਚਕ;
}

ਖਾਲੀ 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    ਮਿਟਾਓ serviceCache; serviceCache = ਖਾਲੀ_ਸੂਚਕ;
}

UBool 
ICUService::ਮੰਨਦਾ_Listener(ਅਚਲ EventListener& l) ਅਚਲ 
{
    ਵਾਪਸ dynamic_cast<ਅਚਲ ServiceListener*>(&l) != ਖਾਲੀ_ਸੂਚਕ;
}

ਖਾਲੀ 
ICUService::notifyListener(EventListener& l) ਅਚਲ 
{
    (static_cast<ServiceListener&>(l)).service_ਬਦਲਿਆ(*ਇਹ);
}

UnicodeString&
ICUService::getName(UnicodeString& ਨਤੀਜਾ) ਅਚਲ 
{
    ਵਾਪਸ ਨਤੀਜਾ.append(name);
}

int32_t 
ICUService::ਗਿਣਤੀ_Factories() ਅਚਲ 
{
    ਵਾਪਸ factories == ਖਾਲੀ_ਸੂਚਕ ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ਅਚਲ
{
    ਵਾਪਸ timestamp;
}

U_NAMESPACE_ਸਿਰਾ

/* UCONFIG_NO_SERVICE */
#ਜੇ_ਅੰਤ
