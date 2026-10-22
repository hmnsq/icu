// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ICUNOTIF_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICUNOTIF_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_ਸਿਰਾ

#ਨਹੀਂਤਾਂ

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : ਜਨਤਕ UObject {
ਜਨਤਕ: 
    ਆਭਾਸੀ ~EventListener();

ਜਨਤਕ:
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();

    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;

ਜਨਤਕ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SERVICE_DEBUG
    ਆਭਾਸੀ UnicodeString& debug(UnicodeString& ਨਤੀਜਾ) ਅਚਲ {
      ਵਾਪਸ debugClass(ਨਤੀਜਾ);
    }

    ਆਭਾਸੀ UnicodeString& debugClass(UnicodeString& ਨਤੀਜਾ) ਅਚਲ {
      ਵਾਪਸ ਨਤੀਜਾ.append((UnicodeString)"Key");
    }
#ਜੇ_ਅੰਤ
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

class U_COMMON_API ICUNotifier : ਜਨਤਕ UMemory  {
ਨਿਜੀ: UVector* listeners;
         
ਜਨਤਕ: 
    ICUNotifier();
    
    ਆਭਾਸੀ ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    ਆਭਾਸੀ ਖਾਲੀ ਜੋੜੋ_Listener(ਅਚਲ EventListener* l, UError_ਕੋਡ& ਸਥਿਤੀ);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    ਆਭਾਸੀ ਖਾਲੀ removeListener(ਅਚਲ EventListener* l, UError_ਕੋਡ& ਸਥਿਤੀ);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    ਆਭਾਸੀ ਖਾਲੀ notify_ਬਦਲਿਆ();
    
ਸੁਰੱਖਿਅਤ: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    ਆਭਾਸੀ UBool ਮੰਨਦਾ_Listener(ਅਚਲ EventListener& l) ਅਚਲ = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    ਆਭਾਸੀ ਖਾਲੀ notifyListener(EventListener& l) ਅਚਲ = 0;
};

U_NAMESPACE_ਸਿਰਾ

/* UCONFIG_NO_SERVICE */
#ਜੇ_ਅੰਤ

/* ICUNOTIF_H */
#ਜੇ_ਅੰਤ
