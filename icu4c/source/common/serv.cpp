// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#समावेश "unicode/utypes.h"
#समावेश "unicode/localpointer.h"

#अगर !UCONFIG_NO_SERVICE

#समावेश "serv.h"
#समावेश "umutex.h"

#अघोषित SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

स्थिर char16_t ICUService_कुंजी::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_कुंजी::ICUService_कुंजी(स्थिर UnicodeString& id) 
: _id(id) {
}

ICUService_कुंजी::~ICUService_कुंजी() 
{
}

स्थिर UnicodeString& 
ICUService_कुंजी::getID() स्थिर 
{
    वापस _id;
}

UnicodeString& 
ICUService_कुंजी::canonicalID(UnicodeString& परिणाम) स्थिर 
{
    वापस परिणाम.append(_id);
}

UnicodeString& 
ICUService_कुंजी::currentID(UnicodeString& परिणाम) स्थिर 
{
    वापस canonicalID(परिणाम);
}

UnicodeString& 
ICUService_कुंजी::currentDescriptor(UnicodeString& परिणाम) स्थिर 
{
    prefix(परिणाम);
    परिणाम.append(PREFIX_DELIMITER);
    वापस currentID(परिणाम);
}

UBool 
ICUService_कुंजी::fallback() 
{
    वापस false;
}

UBool 
ICUService_कुंजी::isFallbackOf(स्थिर UnicodeString& id) स्थिर 
{
    वापस id == _id;
}

UnicodeString& 
ICUService_कुंजी::prefix(UnicodeString& परिणाम) स्थिर 
{
    वापस परिणाम;
}

UnicodeString& 
ICUService_कुंजी::parsePrefix(UnicodeString& परिणाम) 
{
    int32_t n = परिणाम.सूचकांक_Of(PREFIX_DELIMITER);
    अगर (n < 0) {
        n = 0;
    }
    परिणाम.हटाओ(n);
    वापस परिणाम;
}

UnicodeString& 
ICUService_कुंजी::parseSuffix(UnicodeString& परिणाम) 
{
    int32_t n = परिणाम.सूचकांक_Of(PREFIX_DELIMITER);
    अगर (n >= 0) {
        परिणाम.हटाओ(0, n+1);
    }
    वापस परिणाम;
}

#अगर_घोषित SERVICE_DEBUG
UnicodeString& 
ICUService_कुंजी::debug(UnicodeString& परिणाम) स्थिर 
{
    debugClass(परिणाम);
    परिणाम.append((UnicodeString)" id: ");
    परिणाम.append(_id);
    वापस परिणाम;
}

UnicodeString& 
ICUService_कुंजी::debugClass(UnicodeString& परिणाम) स्थिर 
{
    वापस परिणाम.append((UnicodeString)"ICUServiceKey");
}
#पूर्ण_अगर

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_कुंजी)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, स्थिर UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    मिटाओ _instance;
}

UObject* 
SimpleFactory::create(स्थिर ICUService_कुंजी& कुंजी, स्थिर ICUService* service, UErrorCode& status) स्थिर 
{
    अगर (U_SUCCESS(status)) {
        UnicodeString अस्थायी;
        अगर (_id == कुंजी.currentID(अस्थायी)) {
            वापस service->cloneInstance(_instance); 
        }
    }
    वापस nullptr;
}

व्योम 
SimpleFactory::updateVisibleIDs(Hashtable& परिणाम, UErrorCode& status) स्थिर 
{
    अगर (_visible) {
        परिणाम.दो(_id, (व्योम*)यह, status); // cast away const
    } अन्यथा {
        परिणाम.हटाओ(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(स्थिर UnicodeString& id, स्थिर Locale& /* locale */, UnicodeString& परिणाम) स्थिर 
{
    अगर (_visible && _id == id) {
        परिणाम = _id;
    } अन्यथा {
        परिणाम.setToBogus();
    }
    वापस परिणाम;
}

#अगर_घोषित SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) स्थिर 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    वापस toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) स्थिर 
{
    वापस toAppendTo.append((UnicodeString)"SimpleFactory");
}
#पूर्ण_अगर

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
श्रेणी CacheEntry : खुला UMemory {
निजी:
    int32_t refcount;

खुला:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        मिटाओ service;
    }

    CacheEntry(स्थिर UnicodeString& _actualDescriptor, UObject* _service) 
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
        वापस यह;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        अगर ((--refcount) == 0) {
            मिटाओ यह;
            वापस nullptr;
        }
        वापस यह;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() स्थिर {
        वापस refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
अटल व्योम U_CALLCONV
cacheDeleter(व्योम* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

श्रेणी DNCache : खुला UMemory {
खुला:
    Hashtable cache;
    स्थिर Locale locale;

    DNCache(स्थिर Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(स्थिर UnicodeString& displayName, 
                   स्थिर UnicodeString& id,
                   UErrorCode& status)
{
    अगर (U_SUCCESS(status)) {
        StringPair* sp = नया StringPair(displayName, id);
        अगर (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            मिटाओ sp;
            वापस nullptr;
        }
        वापस sp;
    }
    वापस nullptr;
}

UBool 
StringPair::isBogus() स्थिर {
    वापस displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(स्थिर UnicodeString& _displayName, 
                       स्थिर UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
अटल व्योम U_CALLCONV
userv_deleteStringPair(व्योम *obj) {
    U_NAMESPACE_USE मिटाओ (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

अटल UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(स्थिर UnicodeString& newName) 
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
        मिटाओ factories;
        factories = nullptr;
    }
}

UObject* 
ICUService::लो(स्थिर UnicodeString& descriptor, UErrorCode& status) स्थिर 
{
    वापस लो(descriptor, nullptr, status);
}

UObject* 
ICUService::लो(स्थिर UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) स्थिर 
{
    UObject* परिणाम = nullptr;
    ICUService_कुंजी* कुंजी = create_कुंजी(&descriptor, status);
    अगर (कुंजी) {
        परिणाम = get_कुंजी(*कुंजी, actualReturn, status);
        मिटाओ कुंजी;
    }
    वापस परिणाम;
}

UObject* 
ICUService::get_कुंजी(ICUService_कुंजी& कुंजी, UErrorCode& status) स्थिर 
{
    वापस get_कुंजी(कुंजी, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_कुंजी(ICUService_कुंजी& कुंजी, UnicodeString* actualReturn, UErrorCode& status) स्थिर 
{
    वापस get_कुंजी(कुंजी, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
श्रेणी XMutex : खुला UMemory {
खुला:
    अंतरभूत XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        अगर (fActive) umtx_lock(fMutex);
    }
    अंतरभूत ~XMutex() {
        अगर (fActive) umtx_unlock(fMutex);
    }

निजी:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_कुंजी(ICUService_कुंजी& कुंजी, UnicodeString* actualReturn, स्थिर ICUServiceFactory* factory, UErrorCode& status) स्थिर 
{
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    अगर (isDefault()) {
        वापस handleDefault(कुंजी, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(यह); // cast away semantic const

    CacheEntry* परिणाम = nullptr;
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

        अगर (serviceCache == nullptr) {
            ncthis->serviceCache = नया Hashtable(status);
            अगर (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                वापस nullptr;
            }
            अगर (U_FAILURE(status)) {
                मिटाओ serviceCache;
                वापस nullptr;
            }
            serviceCache->set_मूल्य_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = false;

        int32_t start_सूचकांक = 0;
        int32_t limit = factories->size();
        UBool cache_परिणाम = true;

        अगर (factory != nullptr) {
            क्रम (int32_t i = 0; i < limit; ++i) {
                अगर (factory == static_cast<स्थिर ICUServiceFactory*>(factories->elementAt(i))) {
                    start_सूचकांक = i + 1;
                    अवरोध;
                }
            }
            अगर (start_सूचकांक == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                वापस nullptr;
            }
            cache_परिणाम = false;
        }

        करो {
            currentDescriptor.हटाओ();
            कुंजी.currentDescriptor(currentDescriptor);
            परिणाम = static_cast<CacheEntry*>(serviceCache->लो(currentDescriptor));
            अगर (परिणाम != nullptr) {
                अवरोध;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = true;

            int32_t सूचकांक = start_सूचकांक;
            जबतक (सूचकांक < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(सूचकांक++));
                LocalPointer<UObject> service(f->create(कुंजी, यह, status));
                अगर (U_FAILURE(status)) {
                    वापस nullptr;
                }
                अगर (service.isValid()) {
                    परिणाम = नया CacheEntry(currentDescriptor, service.getAlias());
                    अगर (परिणाम == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        वापस nullptr;
                    }
                    service.orphan(); // result now owns service.

                    जाओ outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            अगर (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(नया UVector(uprv_deleteUObject, nullptr, 5, status), status);
                अगर (U_FAILURE(status)) {
                    वापस nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(नया UnicodeString(currentDescriptor), status);
            अगर (U_FAILURE(status)) {
                वापस nullptr;
            }
            अगर (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                वापस nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            अगर (U_FAILURE(status)) {
                वापस nullptr;
            }
        } जबतक (कुंजी.fallback());
outerEnd:

        अगर (परिणाम != nullptr) {
            अगर (putInCache && cache_परिणाम) {
                serviceCache->दो(परिणाम->actualDescriptor, परिणाम, status);
                अगर (U_FAILURE(status)) {
                    वापस nullptr;
                }

                अगर (cacheDescriptorList.isValid()) {
                    क्रम (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->दो(*desc, परिणाम, status);
                        अगर (U_FAILURE(status)) {
                            वापस nullptr;
                        }

                        परिणाम->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            अगर (actualReturn != nullptr) {
                // strip null prefix
                अगर (परिणाम->actualDescriptor.सूचकांक_Of(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->हटाओ();
                    actualReturn->append(परिणाम->actualDescriptor, 
                        1, 
                        परिणाम->actualDescriptor.length() - 1);
                } अन्यथा {
                    *actualReturn = परिणाम->actualDescriptor;
                }

                अगर (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    मिटाओ परिणाम;
                    वापस nullptr;
                }
            }

            UObject* service = cloneInstance(परिणाम->service);
            अगर (putInCache && !cache_परिणाम) {
                मिटाओ परिणाम;
            }
            वापस service;
        }
    }

    वापस handleDefault(कुंजी, actualReturn, status);
}

UObject* 
ICUService::handleDefault(स्थिर ICUService_कुंजी& /*कुंजी*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) स्थिर 
{
    वापस nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& परिणाम, UErrorCode& status) स्थिर {
    वापस getVisibleIDs(परिणाम, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& परिणाम, स्थिर UnicodeString* matchID, UErrorCode& status) स्थिर 
{
    परिणाम.removeAllElements();

    अगर (U_FAILURE(status)) {
        वापस परिणाम;
    }
    UObjectDeleter *savedDeleter = परिणाम.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        स्थिर Hashtable* map = getVisibleIDMap(status);
        अगर (map != nullptr) {
            ICUService_कुंजी* fallback_कुंजी = create_कुंजी(matchID, status);

            क्रम (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                स्थिर UHashElement* e = map->अग्र_Element(pos);
                अगर (e == nullptr) {
                    अवरोध;
                }

                स्थिर UnicodeString* id = static_cast<स्थिर UnicodeString*>(e->कुंजी.pointer);
                अगर (fallback_कुंजी != nullptr) {
                    अगर (!fallback_कुंजी->isFallbackOf(*id)) {
                        जारी;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                परिणाम.adoptElement(idClone.orphan(), status);
            }
            मिटाओ fallback_कुंजी;
        }
    }
    अगर (U_FAILURE(status)) {
        परिणाम.removeAllElements();
    }
    परिणाम.setDeleter(savedDeleter);
    वापस परिणाम;
}

स्थिर Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) स्थिर {
    अगर (U_FAILURE(status)) वापस nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(यह); // cast away semantic const
    अगर (idCache == nullptr) {
        ncthis->idCache = नया Hashtable(status);
        अगर (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा अगर (factories != nullptr) {
            क्रम (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            अगर (U_FAILURE(status)) {
                मिटाओ idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    वापस idCache;
}


UnicodeString& 
ICUService::getDisplayName(स्थिर UnicodeString& id, UnicodeString& परिणाम) स्थिर 
{
    वापस getDisplayName(id, परिणाम, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(स्थिर UnicodeString& id, UnicodeString& परिणाम, स्थिर Locale& locale) स्थिर 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        स्थिर Hashtable* map = getVisibleIDMap(status);
        अगर (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->लो(id));
            अगर (f != nullptr) {
                f->getDisplayName(id, locale, परिणाम);
                वापस परिणाम;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_कुंजी* fallback_कुंजी = create_कुंजी(&id, status);
            जबतक (fallback_कुंजी != nullptr && fallback_कुंजी->fallback()) {
                UnicodeString us;
                fallback_कुंजी->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->लो(us));
                अगर (f != nullptr) {
                    f->getDisplayName(id, locale, परिणाम);
                    मिटाओ fallback_कुंजी;
                    वापस परिणाम;
                }
            }
            मिटाओ fallback_कुंजी;
        }
    }
    परिणाम.setToBogus();
    वापस परिणाम;
}

UVector& 
ICUService::getDisplayNames(UVector& परिणाम, UErrorCode& status) स्थिर 
{
    वापस getDisplayNames(परिणाम, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& परिणाम, स्थिर Locale& locale, UErrorCode& status) स्थिर 
{
    वापस getDisplayNames(परिणाम, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& परिणाम, 
                            स्थिर Locale& locale, 
                            स्थिर UnicodeString* matchID, 
                            UErrorCode& status) स्थिर 
{
    परिणाम.removeAllElements();
    परिणाम.setDeleter(userv_deleteStringPair);
    अगर (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(यह); // cast away semantic const
        Mutex mutex(&lock);

        अगर (dnCache != nullptr && dnCache->locale != locale) {
            मिटाओ dnCache;
            ncthis->dnCache = nullptr;
        }

        अगर (dnCache == nullptr) {
            स्थिर Hashtable* m = getVisibleIDMap(status);
            अगर (U_FAILURE(status)) {
                वापस परिणाम;
            }
            ncthis->dnCache = नया DNCache(locale); 
            अगर (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                वापस परिणाम;
            }

            int32_t pos = UHASH_FIRST;
            स्थिर UHashElement* entry = nullptr;
            जबतक ((entry = m->अग्र_Element(pos)) != nullptr) {
                स्थिर UnicodeString* id = static_cast<स्थिर UnicodeString*>(entry->कुंजी.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                अगर (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अन्यथा {
                    dnCache->cache.दो(dname, (व्योम*)id, status); // share pointer with visibleIDMap
                    अगर (U_SUCCESS(status)) {
                        जारी;
                    }
                }
                मिटाओ dnCache;
                ncthis->dnCache = nullptr;
                वापस परिणाम;
            }
        }
    }

    ICUService_कुंजी* match_कुंजी = create_कुंजी(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    स्थिर UHashElement *entry = nullptr;
    जबतक ((entry = dnCache->cache.अग्र_Element(pos)) != nullptr) {
        स्थिर UnicodeString* id = static_cast<स्थिर UnicodeString*>(entry->value.pointer);
        अगर (match_कुंजी != nullptr && !match_कुंजी->isFallbackOf(*id)) {
            जारी;
        }
        स्थिर UnicodeString* dn = static_cast<स्थिर UnicodeString*>(entry->कुंजी.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        परिणाम.adoptElement(sp, status);
        अगर (U_FAILURE(status)) {
            परिणाम.removeAllElements();
            अवरोध;
        }
    }
    मिटाओ match_कुंजी;

    वापस परिणाम;
}

URegistry_कुंजी
ICUService::registerInstance(UObject* objToAdopt, स्थिर UnicodeString& id, UErrorCode& status) 
{
    वापस registerInstance(objToAdopt, id, true, status);
}

URegistry_कुंजी
ICUService::registerInstance(UObject* objToAdopt, स्थिर UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_कुंजी* कुंजी = create_कुंजी(&id, status);
    अगर (कुंजी != nullptr) {
        UnicodeString canonicalID;
        कुंजी->canonicalID(canonicalID);
        मिटाओ कुंजी;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        अगर (f != nullptr) {
            वापस registerFactory(f, status);
        }
    }
    मिटाओ objToAdopt;
    वापस nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, स्थिर UnicodeString& id, UBool visible, UErrorCode& status)
{
    अगर (U_SUCCESS(status)) {
        अगर ((objToAdopt != nullptr) && (!id.isBogus())) {
            वापस नया SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    वापस nullptr;
}

URegistry_कुंजी
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    अगर (U_FAILURE(status) || factoryToAdopt == nullptr) {
        वापस nullptr;
    }
    {
        Mutex mutex(&lock);

        अगर (factories == nullptr) {
            LocalPointer<UVector> lpFactories(नया UVector(uprv_deleteUObject, nullptr, status), status);
            अगर (U_FAILURE(status)) {
                वापस nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->जोड़ो_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        अगर (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    अगर (U_SUCCESS(status)) {
        notifyChanged();
        वापस (URegistry_कुंजी)factoryToAdopt;
    } अन्यथा {
        वापस nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_कुंजी rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool परिणाम = false;
    अगर (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        अगर (factories->removeElement(factory)) {
            clearCaches();
            परिणाम = true;
        } अन्यथा {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            मिटाओ factory;
        }
    }
    अगर (परिणाम) {
        notifyChanged();
    }
    वापस परिणाम;
}

व्योम 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

व्योम 
ICUService::reInitializeFactories() 
{
    अगर (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() स्थिर 
{
    वापस गिनती_Factories() == 0;
}

ICUService_कुंजी* 
ICUService::create_कुंजी(स्थिर UnicodeString* id, UErrorCode& status) स्थिर 
{
    वापस (U_FAILURE(status) || id == nullptr) ? nullptr : नया ICUService_कुंजी(*id);
}

व्योम 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    मिटाओ dnCache;
    dnCache = nullptr;
    मिटाओ idCache;
    idCache = nullptr;
    मिटाओ serviceCache; serviceCache = nullptr;
}

व्योम 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    मिटाओ serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(स्थिर EventListener& l) स्थिर 
{
    वापस dynamic_cast<स्थिर ServiceListener*>(&l) != nullptr;
}

व्योम 
ICUService::notifyListener(EventListener& l) स्थिर 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*यह);
}

UnicodeString&
ICUService::getName(UnicodeString& परिणाम) स्थिर 
{
    वापस परिणाम.append(name);
}

int32_t 
ICUService::गिनती_Factories() स्थिर 
{
    वापस factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() स्थिर
{
    वापस timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#पूर्ण_अगर
