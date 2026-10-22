// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/localpointer.h"

#જો !UCONFIG_NO_SERVICE

#અન્તર્ભાવ "serv.h"
#અન્તર્ભાવ "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

અચલ char16_t ICUService_ચાવી::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_ચાવી::ICUService_ચાવી(અચલ UnicodeString& id) 
: _id(id) {
}

ICUService_ચાવી::~ICUService_ચાવી() 
{
}

અચલ UnicodeString& 
ICUService_ચાવી::getID() અચલ 
{
    પાછા _id;
}

UnicodeString& 
ICUService_ચાવી::canonicalID(UnicodeString& result) અચલ 
{
    પાછા result.append(_id);
}

UnicodeString& 
ICUService_ચાવી::currentID(UnicodeString& result) અચલ 
{
    પાછા canonicalID(result);
}

UnicodeString& 
ICUService_ચાવી::currentDescriptor(UnicodeString& result) અચલ 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    પાછા currentID(result);
}

UBool 
ICUService_ચાવી::fallback() 
{
    પાછા ખોટું;
}

UBool 
ICUService_ચાવી::isFallbackOf(અચલ UnicodeString& id) અચલ 
{
    પાછા id == _id;
}

UnicodeString& 
ICUService_ચાવી::prefix(UnicodeString& result) અચલ 
{
    પાછા result;
}

UnicodeString& 
ICUService_ચાવી::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    જો (n < 0) {
        n = 0;
    }
    result.remove(n);
    પાછા result;
}

UnicodeString& 
ICUService_ચાવી::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    જો (n >= 0) {
        result.remove(0, n+1);
    }
    પાછા result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_ચાવી::debug(UnicodeString& result) અચલ 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    પાછા result;
}

UnicodeString& 
ICUService_ચાવી::debugClass(UnicodeString& result) અચલ 
{
    પાછા result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_ચાવી)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, અચલ UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(અચલ ICUService_ચાવી& ચાવી, અચલ ICUService* service, UErrorCode& status) અચલ 
{
    જો (U_SUCCESS(status)) {
        UnicodeString temp;
        જો (_id == ચાવી.currentID(temp)) {
            પાછા service->cloneInstance(_instance); 
        }
    }
    પાછા nullptr;
}

શૂન્ય 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) અચલ 
{
    જો (_visible) {
        result.put(_id, (શૂન્ય*)આ, status); // cast away const
    } નહીંતર {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(અચલ UnicodeString& id, અચલ Locale& /* locale */, UnicodeString& result) અચલ 
{
    જો (_visible && _id == id) {
        result = _id;
    } નહીંતર {
        result.setToBogus();
    }
    પાછા result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) અચલ 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    પાછા toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) અચલ 
{
    પાછા toAppendTo.append((UnicodeString)"SimpleFactory");
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
વર્ગ CacheEntry : જાહેર UMemory {
ખાનગી:
    int32_t refcount;

જાહેર:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(અચલ UnicodeString& _actualDescriptor, UObject* _service) 
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
        પાછા આ;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        જો ((--refcount) == 0) {
            delete આ;
            પાછા nullptr;
        }
        પાછા આ;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() અચલ {
        પાછા refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
સ્થિર શૂન્ય U_CALLCONV
cacheDeleter(શૂન્ય* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

વર્ગ DNCache : જાહેર UMemory {
જાહેર:
    Hashtable cache;
    અચલ Locale locale;

    DNCache(અચલ Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(અચલ UnicodeString& displayName, 
                   અચલ UnicodeString& id,
                   UErrorCode& status)
{
    જો (U_SUCCESS(status)) {
        StringPair* sp = નવું StringPair(displayName, id);
        જો (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            પાછા nullptr;
        }
        પાછા sp;
    }
    પાછા nullptr;
}

UBool 
StringPair::isBogus() અચલ {
    પાછા displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(અચલ UnicodeString& _displayName, 
                       અચલ UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
સ્થિર શૂન્ય U_CALLCONV
userv_deleteStringPair(શૂન્ય *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

સ્થિર UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(અચલ UnicodeString& newName) 
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
ICUService::get(અચલ UnicodeString& descriptor, UErrorCode& status) અચલ 
{
    પાછા get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(અચલ UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) અચલ 
{
    UObject* result = nullptr;
    ICUService_ચાવી* ચાવી = create_ચાવી(&descriptor, status);
    જો (ચાવી) {
        result = get_ચાવી(*ચાવી, actualReturn, status);
        delete ચાવી;
    }
    પાછા result;
}

UObject* 
ICUService::get_ચાવી(ICUService_ચાવી& ચાવી, UErrorCode& status) અચલ 
{
    પાછા get_ચાવી(ચાવી, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_ચાવી(ICUService_ચાવી& ચાવી, UnicodeString* actualReturn, UErrorCode& status) અચલ 
{
    પાછા get_ચાવી(ચાવી, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
વર્ગ XMutex : જાહેર UMemory {
જાહેર:
    અંતઃરેખ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        જો (fActive) umtx_lock(fMutex);
    }
    અંતઃરેખ ~XMutex() {
        જો (fActive) umtx_unlock(fMutex);
    }

ખાનગી:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_ચાવી(ICUService_ચાવી& ચાવી, UnicodeString* actualReturn, અચલ ICUServiceFactory* factory, UErrorCode& status) અચલ 
{
    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }

    જો (isDefault()) {
        પાછા handleDefault(ચાવી, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(આ); // cast away semantic const

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

        જો (serviceCache == nullptr) {
            ncthis->serviceCache = નવું Hashtable(status);
            જો (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                પાછા nullptr;
            }
            જો (U_FAILURE(status)) {
                delete serviceCache;
                પાછા nullptr;
            }
            serviceCache->set_કિંમત_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = ખોટું;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = સાચું;

        જો (factory != nullptr) {
            માટે (int32_t i = 0; i < limit; ++i) {
                જો (factory == static_cast<અચલ ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    તોડો;
                }
            }
            જો (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                પાછા nullptr;
            }
            cacheResult = ખોટું;
        }

        કરો {
            currentDescriptor.remove();
            ચાવી.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            જો (result != nullptr) {
                તોડો;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = સાચું;

            int32_t index = startIndex;
            જ્યારે (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(ચાવી, આ, status));
                જો (U_FAILURE(status)) {
                    પાછા nullptr;
                }
                જો (service.isValid()) {
                    result = નવું CacheEntry(currentDescriptor, service.getAlias());
                    જો (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        પાછા nullptr;
                    }
                    service.orphan(); // result now owns service.

                    જાઓ outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            જો (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(નવું UVector(uprv_deleteUObject, nullptr, 5, status), status);
                જો (U_FAILURE(status)) {
                    પાછા nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(નવું UnicodeString(currentDescriptor), status);
            જો (U_FAILURE(status)) {
                પાછા nullptr;
            }
            જો (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                પાછા nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            જો (U_FAILURE(status)) {
                પાછા nullptr;
            }
        } જ્યારે (ચાવી.fallback());
outerEnd:

        જો (result != nullptr) {
            જો (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                જો (U_FAILURE(status)) {
                    પાછા nullptr;
                }

                જો (cacheDescriptorList.isValid()) {
                    માટે (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        જો (U_FAILURE(status)) {
                            પાછા nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            જો (actualReturn != nullptr) {
                // strip null prefix
                જો (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } નહીંતર {
                    *actualReturn = result->actualDescriptor;
                }

                જો (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    પાછા nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            જો (putInCache && !cacheResult) {
                delete result;
            }
            પાછા service;
        }
    }

    પાછા handleDefault(ચાવી, actualReturn, status);
}

UObject* 
ICUService::handleDefault(અચલ ICUService_ચાવી& /*ચાવી*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) અચલ 
{
    પાછા nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) અચલ {
    પાછા getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, અચલ UnicodeString* matchID, UErrorCode& status) અચલ 
{
    result.removeAllElements();

    જો (U_FAILURE(status)) {
        પાછા result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        અચલ Hashtable* map = getVisibleIDMap(status);
        જો (map != nullptr) {
            ICUService_ચાવી* fallback_ચાવી = create_ચાવી(matchID, status);

            માટે (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                અચલ UHashElement* e = map->nextElement(pos);
                જો (e == nullptr) {
                    તોડો;
                }

                અચલ UnicodeString* id = static_cast<અચલ UnicodeString*>(e->ચાવી.pointer);
                જો (fallback_ચાવી != nullptr) {
                    જો (!fallback_ચાવી->isFallbackOf(*id)) {
                        ચાલુ;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_ચાવી;
        }
    }
    જો (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    પાછા result;
}

અચલ Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) અચલ {
    જો (U_FAILURE(status)) પાછા nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(આ); // cast away semantic const
    જો (idCache == nullptr) {
        ncthis->idCache = નવું Hashtable(status);
        જો (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } નહીંતર જો (factories != nullptr) {
            માટે (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            જો (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    પાછા idCache;
}


UnicodeString& 
ICUService::getDisplayName(અચલ UnicodeString& id, UnicodeString& result) અચલ 
{
    પાછા getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(અચલ UnicodeString& id, UnicodeString& result, અચલ Locale& locale) અચલ 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        અચલ Hashtable* map = getVisibleIDMap(status);
        જો (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            જો (f != nullptr) {
                f->getDisplayName(id, locale, result);
                પાછા result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_ચાવી* fallback_ચાવી = create_ચાવી(&id, status);
            જ્યારે (fallback_ચાવી != nullptr && fallback_ચાવી->fallback()) {
                UnicodeString us;
                fallback_ચાવી->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                જો (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_ચાવી;
                    પાછા result;
                }
            }
            delete fallback_ચાવી;
        }
    }
    result.setToBogus();
    પાછા result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) અચલ 
{
    પાછા getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, અચલ Locale& locale, UErrorCode& status) અચલ 
{
    પાછા getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            અચલ Locale& locale, 
                            અચલ UnicodeString* matchID, 
                            UErrorCode& status) અચલ 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    જો (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(આ); // cast away semantic const
        Mutex mutex(&lock);

        જો (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        જો (dnCache == nullptr) {
            અચલ Hashtable* m = getVisibleIDMap(status);
            જો (U_FAILURE(status)) {
                પાછા result;
            }
            ncthis->dnCache = નવું DNCache(locale); 
            જો (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                પાછા result;
            }

            int32_t pos = UHASH_FIRST;
            અચલ UHashElement* entry = nullptr;
            જ્યારે ((entry = m->nextElement(pos)) != nullptr) {
                અચલ UnicodeString* id = static_cast<અચલ UnicodeString*>(entry->ચાવી.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                જો (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } નહીંતર {
                    dnCache->cache.put(dname, (શૂન્ય*)id, status); // share pointer with visibleIDMap
                    જો (U_SUCCESS(status)) {
                        ચાલુ;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                પાછા result;
            }
        }
    }

    ICUService_ચાવી* match_ચાવી = create_ચાવી(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    અચલ UHashElement *entry = nullptr;
    જ્યારે ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        અચલ UnicodeString* id = static_cast<અચલ UnicodeString*>(entry->value.pointer);
        જો (match_ચાવી != nullptr && !match_ચાવી->isFallbackOf(*id)) {
            ચાલુ;
        }
        અચલ UnicodeString* dn = static_cast<અચલ UnicodeString*>(entry->ચાવી.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        જો (U_FAILURE(status)) {
            result.removeAllElements();
            તોડો;
        }
    }
    delete match_ચાવી;

    પાછા result;
}

URegistry_ચાવી
ICUService::registerInstance(UObject* objToAdopt, અચલ UnicodeString& id, UErrorCode& status) 
{
    પાછા registerInstance(objToAdopt, id, સાચું, status);
}

URegistry_ચાવી
ICUService::registerInstance(UObject* objToAdopt, અચલ UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_ચાવી* ચાવી = create_ચાવી(&id, status);
    જો (ચાવી != nullptr) {
        UnicodeString canonicalID;
        ચાવી->canonicalID(canonicalID);
        delete ચાવી;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        જો (f != nullptr) {
            પાછા registerFactory(f, status);
        }
    }
    delete objToAdopt;
    પાછા nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, અચલ UnicodeString& id, UBool visible, UErrorCode& status)
{
    જો (U_SUCCESS(status)) {
        જો ((objToAdopt != nullptr) && (!id.isBogus())) {
            પાછા નવું SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    પાછા nullptr;
}

URegistry_ચાવી
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    જો (U_FAILURE(status) || factoryToAdopt == nullptr) {
        પાછા nullptr;
    }
    {
        Mutex mutex(&lock);

        જો (factories == nullptr) {
            LocalPointer<UVector> lpFactories(નવું UVector(uprv_deleteUObject, nullptr, status), status);
            જો (U_FAILURE(status)) {
                પાછા nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->ઉમેરો_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        જો (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    જો (U_SUCCESS(status)) {
        notifyChanged();
        પાછા (URegistry_ચાવી)factoryToAdopt;
    } નહીંતર {
        પાછા nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_ચાવી rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = ખોટું;
    જો (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        જો (factories->removeElement(factory)) {
            clearCaches();
            result = સાચું;
        } નહીંતર {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    જો (result) {
        notifyChanged();
    }
    પાછા result;
}

શૂન્ય 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

શૂન્ય 
ICUService::reInitializeFactories() 
{
    જો (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() અચલ 
{
    પાછા ગણતરી_Factories() == 0;
}

ICUService_ચાવી* 
ICUService::create_ચાવી(અચલ UnicodeString* id, UErrorCode& status) અચલ 
{
    પાછા (U_FAILURE(status) || id == nullptr) ? nullptr : નવું ICUService_ચાવી(*id);
}

શૂન્ય 
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

શૂન્ય 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(અચલ EventListener& l) અચલ 
{
    પાછા dynamic_cast<અચલ ServiceListener*>(&l) != nullptr;
}

શૂન્ય 
ICUService::notifyListener(EventListener& l) અચલ 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*આ);
}

UnicodeString&
ICUService::getName(UnicodeString& result) અચલ 
{
    પાછા result.append(name);
}

int32_t 
ICUService::ગણતરી_Factories() અચલ 
{
    પાછા factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() અચલ
{
    પાછા timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
