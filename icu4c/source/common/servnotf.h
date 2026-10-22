// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
#ifndef ICUNOTIF_H
#define ICUNOTIF_H

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_END

#ଅନ୍ୟଥା

#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"

#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : ସର୍ବସାଧାରଣ UObject {
ସର୍ବସାଧାରଣ: 
    ଆଭାସୀ ~EventListener();

ସର୍ବସାଧାରଣ:
    ସ୍ଥିର UClassID U_EXPORT2 getStaticClassID();

    ଆଭାସୀ UClassID getDynamicClassID() ଅଚଳ override;

ସର୍ବସାଧାରଣ:
#ifdef SERVICE_DEBUG
    ଆଭାସୀ UnicodeString& debug(UnicodeString& result) ଅଚଳ {
      ଫେରନ୍ତୁ debugClass(result);
    }

    ଆଭାସୀ UnicodeString& debugClass(UnicodeString& result) ଅଚଳ {
      ଫେରନ୍ତୁ result.append((UnicodeString)"Key");
    }
#endif
};

/**
 * <p>Abstract implementation of a notification facility.  Clients add
 * EventListeners with addListener and remove them with removeListener.
 * Notifiers call notifyChanged when they wish to notify listeners.
 * This queues the listener list on the notification thread, which
 * eventually dequeues the list and calls notifyListener on each
 * listener in the list.</p>
 *
 * <p>Subclasses override acceptsListener and notifyListener 
 * to add type-safe notification.  AcceptsListener should return
 * true if the listener is of the appropriate type; ICUNotifier
 * itself will ensure the listener is non-null and that the
 * identical listener is not already registered with the Notifier.
 * NotifyListener should cast the listener to the appropriate 
 * type and call the appropriate method on the listener.
 */

class U_COMMON_API ICUNotifier : ସର୍ବସାଧାରଣ UMemory  {
ବ୍ୟକ୍ତିଗତ: UVector* listeners;
         
ସର୍ବସାଧାରଣ: 
    ICUNotifier();
    
    ଆଭାସୀ ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    ଆଭାସୀ ଶୂନ୍ୟ addListener(ଅଚଳ EventListener* l, UErrorCode& status);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    ଆଭାସୀ ଶୂନ୍ୟ removeListener(ଅଚଳ EventListener* l, UErrorCode& status);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    ଆଭାସୀ ଶୂନ୍ୟ notifyChanged();
    
ସୁରକ୍ଷିତ: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    ଆଭାସୀ UBool acceptsListener(ଅଚଳ EventListener& l) ଅଚଳ = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    ଆଭାସୀ ଶୂନ୍ୟ notifyListener(EventListener& l) ଅଚଳ = 0;
};

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

/* ICUNOTIF_H */
#endif
