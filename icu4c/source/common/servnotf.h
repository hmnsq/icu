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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_END

#ಅನ್ಯಥಾ

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"

#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : ಸಾರ್ವಜನಿಕ UObject {
ಸಾರ್ವಜನಿಕ: 
    ಆಭಾಸ ~EventListener();

ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    ಆಭಾಸ UClassID getDynamicClassID() ಧ್ರುವ override;

ಸಾರ್ವಜನಿಕ:
#ifdef SERVICE_DEBUG
    ಆಭಾಸ UnicodeString& debug(UnicodeString& result) ಧ್ರುವ {
      ಪ್ರತ್ಯಾಗಮ debugClass(result);
    }

    ಆಭಾಸ UnicodeString& debugClass(UnicodeString& result) ಧ್ರುವ {
      ಪ್ರತ್ಯಾಗಮ result.append((UnicodeString)"Key");
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

class U_COMMON_API ICUNotifier : ಸಾರ್ವಜನಿಕ UMemory  {
ಗುಪ್ತ: UVector* listeners;
         
ಸಾರ್ವಜನಿಕ: 
    ICUNotifier();
    
    ಆಭಾಸ ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    ಆಭಾಸ ಶೂನ್ಯ addListener(ಧ್ರುವ EventListener* l, UErrorCode& status);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    ಆಭಾಸ ಶೂನ್ಯ removeListener(ಧ್ರುವ EventListener* l, UErrorCode& status);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    ಆಭಾಸ ಶೂನ್ಯ notifyChanged();
    
ರಕ್ಷಿತ: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    ಆಭಾಸ UBool acceptsListener(ಧ್ರುವ EventListener& l) ಧ್ರುವ = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    ಆಭಾಸ ಶೂನ್ಯ notifyListener(EventListener& l) ಧ್ರುವ = 0;
};

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

/* ICUNOTIF_H */
#endif
