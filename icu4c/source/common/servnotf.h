// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ICUNOTIF_H
#ವ್ಯಾಖ್ಯಾನಿಸು ICUNOTIF_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_ಕೊನೆ

#ಇಲ್ಲದಿದ್ದರೆ

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"

#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : ಸಾರ್ವಜನಿಕ UObject {
ಸಾರ್ವಜನಿಕ: 
    ವಾಸ್ತವಿಕ ~EventListener();

ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;

ಸಾರ್ವಜನಿಕ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SERVICE_DEBUG
    ವಾಸ್ತವಿಕ UnicodeString& debug(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ {
      ಹಿಂತಿರುಗಿ debugClass(ಫಲಿತಾಂಶ);
    }

    ವಾಸ್ತವಿಕ UnicodeString& debugClass(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ {
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append((UnicodeString)"Key");
    }
#ಆದರೆ_ಅಂತ್ಯ
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
ಖಾಸಗಿ: UVector* listeners;
         
ಸಾರ್ವಜನಿಕ: 
    ICUNotifier();
    
    ವಾಸ್ತವಿಕ ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಕೂಡಿಸು_Listener(ಸ್ಥಿರಾಂಕ EventListener* l, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ removeListener(ಸ್ಥಿರಾಂಕ EventListener* l, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ notify_ಬದಲಾಯಿತು();
    
ಸಂರಕ್ಷಿತ: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    ವಾಸ್ತವಿಕ UBool ಸ್ವೀಕರಿಸು_Listener(ಸ್ಥಿರಾಂಕ EventListener& l) ಸ್ಥಿರಾಂಕ = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ notifyListener(EventListener& l) ಸ್ಥಿರಾಂಕ = 0;
};

U_NAMESPACE_ಕೊನೆ

/* UCONFIG_NO_SERVICE */
#ಆದರೆ_ಅಂತ್ಯ

/* ICUNOTIF_H */
#ಆದರೆ_ಅಂತ್ಯ
