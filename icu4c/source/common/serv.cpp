// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/localpointer.h"

#जर !UCONFIG_NO_SERVICE

#अन्तर्भाव "serv.h"
#अन्तर्भाव "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

स्थिरांक char16_t ICUService_किल्ली::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_किल्ली::ICUService_किल्ली(स्थिरांक UnicodeString& id) 
: _id(id) {
}

ICUService_किल्ली::~ICUService_किल्ली() 
{
}

स्थिरांक UnicodeString& 
ICUService_किल्ली::getID() स्थिरांक 
{
    परत _id;
}

UnicodeString& 
ICUService_किल्ली::canonicalID(UnicodeString& result) स्थिरांक 
{
    परत result.append(_id);
}

UnicodeString& 
ICUService_किल्ली::currentID(UnicodeString& result) स्थिरांक 
{
    परत canonicalID(result);
}

UnicodeString& 
ICUService_किल्ली::currentDescriptor(UnicodeString& result) स्थिरांक 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    परत currentID(result);
}

UBool 
ICUService_किल्ली::fallback() 
{
    परत असत्य;
}

UBool 
ICUService_किल्ली::isFallbackOf(स्थिरांक UnicodeString& id) स्थिरांक 
{
    परत id == _id;
}

UnicodeString& 
ICUService_किल्ली::prefix(UnicodeString& result) स्थिरांक 
{
    परत result;
}

UnicodeString& 
ICUService_किल्ली::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    जर (n < 0) {
        n = 0;
    }
    result.हटाओ(n);
    परत result;
}

UnicodeString& 
ICUService_किल्ली::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    जर (n >= 0) {
        result.हटाओ(0, n+1);
    }
    परत result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_किल्ली::debug(UnicodeString& result) स्थिरांक 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    परत result;
}

UnicodeString& 
ICUService_किल्ली::debugClass(UnicodeString& result) स्थिरांक 
{
    परत result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_किल्ली)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, स्थिरांक UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(स्थिरांक ICUService_किल्ली& किल्ली, स्थिरांक ICUService* service, UErrorCode& status) स्थिरांक 
{
    जर (U_SUCCESS(status)) {
        UnicodeString temp;
        जर (_id == किल्ली.currentID(temp)) {
            परत service->cloneInstance(_instance); 
        }
    }
    परत nullptr;
}

शून्य 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) स्थिरांक 
{
    जर (_visible) {
        result.put(_id, (शून्य*)हे, status); // cast away const
    } अन्यथा {
        result.हटाओ(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(स्थिरांक UnicodeString& id, स्थिरांक Locale& /* locale */, UnicodeString& result) स्थिरांक 
{
    जर (_visible && _id == id) {
        result = _id;
    } अन्यथा {
        result.setToBogus();
    }
    परत result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) स्थिरांक 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    परत toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) स्थिरांक 
{
    परत toAppendTo.append((UnicodeString)"SimpleFactory");
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
वर्ग CacheEntry : सार्वजनिक UMemory {
खाजगी:
    int32_t refcount;

सार्वजनिक:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(स्थिरांक UnicodeString& _actualDescriptor, UObject* _service) 
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
        परत हे;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        जर ((--refcount) == 0) {
            delete हे;
            परत nullptr;
        }
        परत हे;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() स्थिरांक {
        परत refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
cacheDeleter(शून्य* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

वर्ग DNCache : सार्वजनिक UMemory {
सार्वजनिक:
    Hashtable cache;
    स्थिरांक Locale locale;

    DNCache(स्थिरांक Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(स्थिरांक UnicodeString& displayName, 
                   स्थिरांक UnicodeString& id,
                   UErrorCode& status)
{
    जर (U_SUCCESS(status)) {
        StringPair* sp = नवीन StringPair(displayName, id);
        जर (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            परत nullptr;
        }
        परत sp;
    }
    परत nullptr;
}

UBool 
StringPair::isBogus() स्थिरांक {
    परत displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(स्थिरांक UnicodeString& _displayName, 
                       स्थिरांक UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
userv_deleteStringPair(शून्य *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

स्थिर UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(स्थिरांक UnicodeString& newName) 
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
ICUService::get(स्थिरांक UnicodeString& descriptor, UErrorCode& status) स्थिरांक 
{
    परत get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(स्थिरांक UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) स्थिरांक 
{
    UObject* result = nullptr;
    ICUService_किल्ली* किल्ली = create_किल्ली(&descriptor, status);
    जर (किल्ली) {
        result = get_किल्ली(*किल्ली, actualReturn, status);
        delete किल्ली;
    }
    परत result;
}

UObject* 
ICUService::get_किल्ली(ICUService_किल्ली& किल्ली, UErrorCode& status) स्थिरांक 
{
    परत get_किल्ली(किल्ली, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_किल्ली(ICUService_किल्ली& किल्ली, UnicodeString* actualReturn, UErrorCode& status) स्थिरांक 
{
    परत get_किल्ली(किल्ली, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
वर्ग XMutex : सार्वजनिक UMemory {
सार्वजनिक:
    ओळीत XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        जर (fActive) umtx_lock(fMutex);
    }
    ओळीत ~XMutex() {
        जर (fActive) umtx_unlock(fMutex);
    }

खाजगी:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_किल्ली(ICUService_किल्ली& किल्ली, UnicodeString* actualReturn, स्थिरांक ICUServiceFactory* factory, UErrorCode& status) स्थिरांक 
{
    जर (U_FAILURE(status)) {
        परत nullptr;
    }

    जर (isDefault()) {
        परत handleDefault(किल्ली, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(हे); // cast away semantic const

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

        जर (serviceCache == nullptr) {
            ncthis->serviceCache = नवीन Hashtable(status);
            जर (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                परत nullptr;
            }
            जर (U_FAILURE(status)) {
                delete serviceCache;
                परत nullptr;
            }
            serviceCache->set_मूल्य_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = असत्य;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = सत्य;

        जर (factory != nullptr) {
            साठी (int32_t i = 0; i < limit; ++i) {
                जर (factory == static_cast<स्थिरांक ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    थांबा;
                }
            }
            जर (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                परत nullptr;
            }
            cacheResult = असत्य;
        }

        करा {
            currentDescriptor.हटाओ();
            किल्ली.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            जर (result != nullptr) {
                थांबा;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = सत्य;

            int32_t index = startIndex;
            पर्यंत (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(किल्ली, हे, status));
                जर (U_FAILURE(status)) {
                    परत nullptr;
                }
                जर (service.isValid()) {
                    result = नवीन CacheEntry(currentDescriptor, service.getAlias());
                    जर (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        परत nullptr;
                    }
                    service.orphan(); // result now owns service.

                    जा outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            जर (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(नवीन UVector(uprv_deleteUObject, nullptr, 5, status), status);
                जर (U_FAILURE(status)) {
                    परत nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(नवीन UnicodeString(currentDescriptor), status);
            जर (U_FAILURE(status)) {
                परत nullptr;
            }
            जर (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                परत nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            जर (U_FAILURE(status)) {
                परत nullptr;
            }
        } पर्यंत (किल्ली.fallback());
outerEnd:

        जर (result != nullptr) {
            जर (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                जर (U_FAILURE(status)) {
                    परत nullptr;
                }

                जर (cacheDescriptorList.isValid()) {
                    साठी (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        जर (U_FAILURE(status)) {
                            परत nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            जर (actualReturn != nullptr) {
                // strip null prefix
                जर (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->हटाओ();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } अन्यथा {
                    *actualReturn = result->actualDescriptor;
                }

                जर (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    परत nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            जर (putInCache && !cacheResult) {
                delete result;
            }
            परत service;
        }
    }

    परत handleDefault(किल्ली, actualReturn, status);
}

UObject* 
ICUService::handleDefault(स्थिरांक ICUService_किल्ली& /*किल्ली*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) स्थिरांक 
{
    परत nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) स्थिरांक {
    परत getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, स्थिरांक UnicodeString* matchID, UErrorCode& status) स्थिरांक 
{
    result.removeAllElements();

    जर (U_FAILURE(status)) {
        परत result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        स्थिरांक Hashtable* map = getVisibleIDMap(status);
        जर (map != nullptr) {
            ICUService_किल्ली* fallback_किल्ली = create_किल्ली(matchID, status);

            साठी (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                स्थिरांक UHashElement* e = map->nextElement(pos);
                जर (e == nullptr) {
                    थांबा;
                }

                स्थिरांक UnicodeString* id = static_cast<स्थिरांक UnicodeString*>(e->किल्ली.pointer);
                जर (fallback_किल्ली != nullptr) {
                    जर (!fallback_किल्ली->isFallbackOf(*id)) {
                        सुरूठेवा;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_किल्ली;
        }
    }
    जर (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    परत result;
}

स्थिरांक Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) स्थिरांक {
    जर (U_FAILURE(status)) परत nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(हे); // cast away semantic const
    जर (idCache == nullptr) {
        ncthis->idCache = नवीन Hashtable(status);
        जर (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा जर (factories != nullptr) {
            साठी (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            जर (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    परत idCache;
}


UnicodeString& 
ICUService::getDisplayName(स्थिरांक UnicodeString& id, UnicodeString& result) स्थिरांक 
{
    परत getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(स्थिरांक UnicodeString& id, UnicodeString& result, स्थिरांक Locale& locale) स्थिरांक 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        स्थिरांक Hashtable* map = getVisibleIDMap(status);
        जर (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            जर (f != nullptr) {
                f->getDisplayName(id, locale, result);
                परत result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_किल्ली* fallback_किल्ली = create_किल्ली(&id, status);
            पर्यंत (fallback_किल्ली != nullptr && fallback_किल्ली->fallback()) {
                UnicodeString us;
                fallback_किल्ली->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                जर (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_किल्ली;
                    परत result;
                }
            }
            delete fallback_किल्ली;
        }
    }
    result.setToBogus();
    परत result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) स्थिरांक 
{
    परत getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, स्थिरांक Locale& locale, UErrorCode& status) स्थिरांक 
{
    परत getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            स्थिरांक Locale& locale, 
                            स्थिरांक UnicodeString* matchID, 
                            UErrorCode& status) स्थिरांक 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    जर (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(हे); // cast away semantic const
        Mutex mutex(&lock);

        जर (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        जर (dnCache == nullptr) {
            स्थिरांक Hashtable* m = getVisibleIDMap(status);
            जर (U_FAILURE(status)) {
                परत result;
            }
            ncthis->dnCache = नवीन DNCache(locale); 
            जर (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                परत result;
            }

            int32_t pos = UHASH_FIRST;
            स्थिरांक UHashElement* entry = nullptr;
            पर्यंत ((entry = m->nextElement(pos)) != nullptr) {
                स्थिरांक UnicodeString* id = static_cast<स्थिरांक UnicodeString*>(entry->किल्ली.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                जर (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अन्यथा {
                    dnCache->cache.put(dname, (शून्य*)id, status); // share pointer with visibleIDMap
                    जर (U_SUCCESS(status)) {
                        सुरूठेवा;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                परत result;
            }
        }
    }

    ICUService_किल्ली* match_किल्ली = create_किल्ली(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    स्थिरांक UHashElement *entry = nullptr;
    पर्यंत ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        स्थिरांक UnicodeString* id = static_cast<स्थिरांक UnicodeString*>(entry->value.pointer);
        जर (match_किल्ली != nullptr && !match_किल्ली->isFallbackOf(*id)) {
            सुरूठेवा;
        }
        स्थिरांक UnicodeString* dn = static_cast<स्थिरांक UnicodeString*>(entry->किल्ली.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        जर (U_FAILURE(status)) {
            result.removeAllElements();
            थांबा;
        }
    }
    delete match_किल्ली;

    परत result;
}

URegistry_किल्ली
ICUService::registerInstance(UObject* objToAdopt, स्थिरांक UnicodeString& id, UErrorCode& status) 
{
    परत registerInstance(objToAdopt, id, सत्य, status);
}

URegistry_किल्ली
ICUService::registerInstance(UObject* objToAdopt, स्थिरांक UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_किल्ली* किल्ली = create_किल्ली(&id, status);
    जर (किल्ली != nullptr) {
        UnicodeString canonicalID;
        किल्ली->canonicalID(canonicalID);
        delete किल्ली;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        जर (f != nullptr) {
            परत registerFactory(f, status);
        }
    }
    delete objToAdopt;
    परत nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, स्थिरांक UnicodeString& id, UBool visible, UErrorCode& status)
{
    जर (U_SUCCESS(status)) {
        जर ((objToAdopt != nullptr) && (!id.isBogus())) {
            परत नवीन SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    परत nullptr;
}

URegistry_किल्ली
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    जर (U_FAILURE(status) || factoryToAdopt == nullptr) {
        परत nullptr;
    }
    {
        Mutex mutex(&lock);

        जर (factories == nullptr) {
            LocalPointer<UVector> lpFactories(नवीन UVector(uprv_deleteUObject, nullptr, status), status);
            जर (U_FAILURE(status)) {
                परत nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->घाला_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        जर (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    जर (U_SUCCESS(status)) {
        notifyChanged();
        परत (URegistry_किल्ली)factoryToAdopt;
    } अन्यथा {
        परत nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_किल्ली rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = असत्य;
    जर (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        जर (factories->removeElement(factory)) {
            clearCaches();
            result = सत्य;
        } अन्यथा {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    जर (result) {
        notifyChanged();
    }
    परत result;
}

शून्य 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

शून्य 
ICUService::reInitializeFactories() 
{
    जर (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() स्थिरांक 
{
    परत मोजणी_Factories() == 0;
}

ICUService_किल्ली* 
ICUService::create_किल्ली(स्थिरांक UnicodeString* id, UErrorCode& status) स्थिरांक 
{
    परत (U_FAILURE(status) || id == nullptr) ? nullptr : नवीन ICUService_किल्ली(*id);
}

शून्य 
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

शून्य 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(स्थिरांक EventListener& l) स्थिरांक 
{
    परत dynamic_cast<स्थिरांक ServiceListener*>(&l) != nullptr;
}

शून्य 
ICUService::notifyListener(EventListener& l) स्थिरांक 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*हे);
}

UnicodeString&
ICUService::getName(UnicodeString& result) स्थिरांक 
{
    परत result.append(name);
}

int32_t 
ICUService::मोजणी_Factories() स्थिरांक 
{
    परत factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() स्थिरांक
{
    परत timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
