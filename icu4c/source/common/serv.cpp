// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#include "unicode/utypes.h"
#include "unicode/localpointer.h"

#නම් !UCONFIG_NO_SERVICE

#include "serv.h"
#include "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

const char16_t ICUServiceKey::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUServiceKey::ICUServiceKey(const UnicodeString& id) 
: _id(id) {
}

ICUServiceKey::~ICUServiceKey() 
{
}

const UnicodeString& 
ICUServiceKey::getID() const 
{
    ආපසු _id;
}

UnicodeString& 
ICUServiceKey::canonicalID(UnicodeString& result) const 
{
    ආපසු result.append(_id);
}

UnicodeString& 
ICUServiceKey::currentID(UnicodeString& result) const 
{
    ආපසු canonicalID(result);
}

UnicodeString& 
ICUServiceKey::currentDescriptor(UnicodeString& result) const 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    ආපසු currentID(result);
}

UBool 
ICUServiceKey::fallback() 
{
    ආපසු false;
}

UBool 
ICUServiceKey::isFallbackOf(const UnicodeString& id) const 
{
    ආපසු id == _id;
}

UnicodeString& 
ICUServiceKey::prefix(UnicodeString& result) const 
{
    ආපසු result;
}

UnicodeString& 
ICUServiceKey::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    නම් (n < 0) {
        n = 0;
    }
    result.remove(n);
    ආපසු result;
}

UnicodeString& 
ICUServiceKey::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    නම් (n >= 0) {
        result.remove(0, n+1);
    }
    ආපසු result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUServiceKey::debug(UnicodeString& result) const 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    ආපසු result;
}

UnicodeString& 
ICUServiceKey::debugClass(UnicodeString& result) const 
{
    ආපසු result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUServiceKey)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, const UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(const ICUServiceKey& key, const ICUService* service, UErrorCode& status) const 
{
    නම් (U_SUCCESS(status)) {
        UnicodeString temp;
        නම් (_id == key.currentID(temp)) {
            ආපසු service->cloneInstance(_instance); 
        }
    }
    ආපසු nullptr;
}

void 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) const 
{
    නම් (_visible) {
        result.put(_id, (void*)this, status); // cast away const
    } නැතහොත් {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(const UnicodeString& id, const Locale& /* locale */, UnicodeString& result) const 
{
    නම් (_visible && _id == id) {
        result = _id;
    } නැතහොත් {
        result.setToBogus();
    }
    ආපසු result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) const 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    ආපසු toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) const 
{
    ආපසු toAppendTo.append((UnicodeString)"SimpleFactory");
}
#endif

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
class CacheEntry : public UMemory {
private:
    int32_t refcount;

public:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(const UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    CacheEntry* ref() {
        ++refcount;
        ආපසු this;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        නම් ((--refcount) == 0) {
            delete this;
            ආපසු nullptr;
        }
        ආපසු this;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() const {
        ආපසු refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
static void U_CALLCONV
cacheDeleter(void* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

class DNCache : public UMemory {
public:
    Hashtable cache;
    const Locale locale;

    DNCache(const Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(const UnicodeString& displayName, 
                   const UnicodeString& id,
                   UErrorCode& status)
{
    නම් (U_SUCCESS(status)) {
        StringPair* sp = new StringPair(displayName, id);
        නම් (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            ආපසු nullptr;
        }
        ආපසු sp;
    }
    ආපසු nullptr;
}

UBool 
StringPair::isBogus() const {
    ආපසු displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(const UnicodeString& _displayName, 
                       const UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
static void U_CALLCONV
userv_deleteStringPair(void *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

static UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(const UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&lock);
        clearCaches();
        delete factories;
        factories = nullptr;
    }
}

UObject* 
ICUService::get(const UnicodeString& descriptor, UErrorCode& status) const 
{
    ආපසු get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(const UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) const 
{
    UObject* result = nullptr;
    ICUServiceKey* key = createKey(&descriptor, status);
    නම් (key) {
        result = getKey(*key, actualReturn, status);
        delete key;
    }
    ආපසු result;
}

UObject* 
ICUService::getKey(ICUServiceKey& key, UErrorCode& status) const 
{
    ආපසු getKey(key, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::getKey(ICUServiceKey& key, UnicodeString* actualReturn, UErrorCode& status) const 
{
    ආපසු getKey(key, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
class XMutex : public UMemory {
public:
    anta_hstha XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        නම් (fActive) umtx_lock(fMutex);
    }
    anta_hstha ~XMutex() {
        නම් (fActive) umtx_unlock(fMutex);
    }

private:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::getKey(ICUServiceKey& key, UnicodeString* actualReturn, const ICUServiceFactory* factory, UErrorCode& status) const 
{
    නම් (U_FAILURE(status)) {
        ආපසු nullptr;
    }

    නම් (isDefault()) {
        ආපසු handleDefault(key, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(this); // cast away semantic const

    CacheEntry* result = nullptr;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&lock, factory != nullptr);

        නම් (serviceCache == nullptr) {
            ncthis->serviceCache = new Hashtable(status);
            නම් (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ආපසු nullptr;
            }
            නම් (U_FAILURE(status)) {
                delete serviceCache;
                ආපසු nullptr;
            }
            serviceCache->setValueDeleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = false;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = true;

        නම් (factory != nullptr) {
            සඳහා (int32_t i = 0; i < limit; ++i) {
                නම් (factory == static_cast<const ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    bha_nga;
                }
            }
            නම් (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ආපසු nullptr;
            }
            cacheResult = false;
        }

        do {
            currentDescriptor.remove();
            key.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            නම් (result != nullptr) {
                bha_nga;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = true;

            int32_t index = startIndex;
            අතරතුර (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(key, this, status));
                නම් (U_FAILURE(status)) {
                    ආපසු nullptr;
                }
                නම් (service.isValid()) {
                    result = new CacheEntry(currentDescriptor, service.getAlias());
                    නම් (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        ආපසු nullptr;
                    }
                    service.orphan(); // result now owns service.

                    goto outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            නම් (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(new UVector(uprv_deleteUObject, nullptr, 5, status), status);
                නම් (U_FAILURE(status)) {
                    ආපසු nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(new UnicodeString(currentDescriptor), status);
            නම් (U_FAILURE(status)) {
                ආපසු nullptr;
            }
            නම් (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ආපසු nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            නම් (U_FAILURE(status)) {
                ආපසු nullptr;
            }
        } අතරතුර (key.fallback());
outerEnd:

        නම් (result != nullptr) {
            නම් (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                නම් (U_FAILURE(status)) {
                    ආපසු nullptr;
                }

                නම් (cacheDescriptorList.isValid()) {
                    සඳහා (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        නම් (U_FAILURE(status)) {
                            ආපසු nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            නම් (actualReturn != nullptr) {
                // strip null prefix
                නම් (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } නැතහොත් {
                    *actualReturn = result->actualDescriptor;
                }

                නම් (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    ආපසු nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            නම් (putInCache && !cacheResult) {
                delete result;
            }
            ආපසු service;
        }
    }

    ආපසු handleDefault(key, actualReturn, status);
}

UObject* 
ICUService::handleDefault(const ICUServiceKey& /* key */, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) const 
{
    ආපසු nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) const {
    ආපසු getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, const UnicodeString* matchID, UErrorCode& status) const 
{
    result.removeAllElements();

    නම් (U_FAILURE(status)) {
        ආපසු result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        const Hashtable* map = getVisibleIDMap(status);
        නම් (map != nullptr) {
            ICUServiceKey* fallbackKey = createKey(matchID, status);

            සඳහා (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                const UHashElement* e = map->nextElement(pos);
                නම් (e == nullptr) {
                    bha_nga;
                }

                const UnicodeString* id = static_cast<const UnicodeString*>(e->key.pointer);
                නම් (fallbackKey != nullptr) {
                    නම් (!fallbackKey->isFallbackOf(*id)) {
                        continue;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallbackKey;
        }
    }
    නම් (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    ආපසු result;
}

const Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) const {
    නම් (U_FAILURE(status)) ආපසු nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(this); // cast away semantic const
    නම් (idCache == nullptr) {
        ncthis->idCache = new Hashtable(status);
        නම් (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } නැතහොත් නම් (factories != nullptr) {
            සඳහා (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            නම් (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    ආපසු idCache;
}


UnicodeString& 
ICUService::getDisplayName(const UnicodeString& id, UnicodeString& result) const 
{
    ආපසු getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(const UnicodeString& id, UnicodeString& result, const Locale& locale) const 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        const Hashtable* map = getVisibleIDMap(status);
        නම් (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            නම් (f != nullptr) {
                f->getDisplayName(id, locale, result);
                ආපසු result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUServiceKey* fallbackKey = createKey(&id, status);
            අතරතුර (fallbackKey != nullptr && fallbackKey->fallback()) {
                UnicodeString us;
                fallbackKey->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                නම් (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallbackKey;
                    ආපසු result;
                }
            }
            delete fallbackKey;
        }
    }
    result.setToBogus();
    ආපසු result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) const 
{
    ආපසු getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, const Locale& locale, UErrorCode& status) const 
{
    ආපසු getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            const Locale& locale, 
                            const UnicodeString* matchID, 
                            UErrorCode& status) const 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    නම් (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(this); // cast away semantic const
        Mutex mutex(&lock);

        නම් (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        නම් (dnCache == nullptr) {
            const Hashtable* m = getVisibleIDMap(status);
            නම් (U_FAILURE(status)) {
                ආපසු result;
            }
            ncthis->dnCache = new DNCache(locale); 
            නම් (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ආපසු result;
            }

            int32_t pos = UHASH_FIRST;
            const UHashElement* entry = nullptr;
            අතරතුර ((entry = m->nextElement(pos)) != nullptr) {
                const UnicodeString* id = static_cast<const UnicodeString*>(entry->key.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                නම් (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } නැතහොත් {
                    dnCache->cache.put(dname, (void*)id, status); // share pointer with visibleIDMap
                    නම් (U_SUCCESS(status)) {
                        continue;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                ආපසු result;
            }
        }
    }

    ICUServiceKey* matchKey = createKey(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    const UHashElement *entry = nullptr;
    අතරතුර ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        const UnicodeString* id = static_cast<const UnicodeString*>(entry->value.pointer);
        නම් (matchKey != nullptr && !matchKey->isFallbackOf(*id)) {
            continue;
        }
        const UnicodeString* dn = static_cast<const UnicodeString*>(entry->key.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        නම් (U_FAILURE(status)) {
            result.removeAllElements();
            bha_nga;
        }
    }
    delete matchKey;

    ආපසු result;
}

URegistryKey
ICUService::registerInstance(UObject* objToAdopt, const UnicodeString& id, UErrorCode& status) 
{
    ආපසු registerInstance(objToAdopt, id, true, status);
}

URegistryKey
ICUService::registerInstance(UObject* objToAdopt, const UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUServiceKey* key = createKey(&id, status);
    නම් (key != nullptr) {
        UnicodeString canonicalID;
        key->canonicalID(canonicalID);
        delete key;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        නම් (f != nullptr) {
            ආපසු registerFactory(f, status);
        }
    }
    delete objToAdopt;
    ආපසු nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, const UnicodeString& id, UBool visible, UErrorCode& status)
{
    නම් (U_SUCCESS(status)) {
        නම් ((objToAdopt != nullptr) && (!id.isBogus())) {
            ආපසු new SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ආපසු nullptr;
}

URegistryKey
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    නම් (U_FAILURE(status) || factoryToAdopt == nullptr) {
        ආපසු nullptr;
    }
    {
        Mutex mutex(&lock);

        නම් (factories == nullptr) {
            LocalPointer<UVector> lpFactories(new UVector(uprv_deleteUObject, nullptr, status), status);
            නම් (U_FAILURE(status)) {
                ආපසු nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->insertElementAt(lpFactoryToAdopt.orphan(), 0, status);
        නම් (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    නම් (U_SUCCESS(status)) {
        notifyChanged();
        ආපසු (URegistryKey)factoryToAdopt;
    } නැතහොත් {
        ආපසු nullptr;
    }
}

UBool 
ICUService::unregister(URegistryKey rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = false;
    නම් (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        නම් (factories->removeElement(factory)) {
            clearCaches();
            result = true;
        } නැතහොත් {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    නම් (result) {
        notifyChanged();
    }
    ආපසු result;
}

void 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

void 
ICUService::reInitializeFactories() 
{
    නම් (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() const 
{
    ආපසු countFactories() == 0;
}

ICUServiceKey* 
ICUService::createKey(const UnicodeString* id, UErrorCode& status) const 
{
    ආපසු (U_FAILURE(status) || id == nullptr) ? nullptr : new ICUServiceKey(*id);
}

void 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    delete dnCache;
    dnCache = nullptr;
    delete idCache;
    idCache = nullptr;
    delete serviceCache; serviceCache = nullptr;
}

void 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(const EventListener& l) const 
{
    ආපසු dynamic_cast<const ServiceListener*>(&l) != nullptr;
}

void 
ICUService::notifyListener(EventListener& l) const 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*this);
}

UnicodeString&
ICUService::getName(UnicodeString& result) const 
{
    ආපසු result.append(name);
}

int32_t 
ICUService::countFactories() const 
{
    ආපසු factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() const
{
    ආපසු timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
