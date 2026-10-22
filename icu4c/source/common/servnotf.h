// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
#که_نه_تعریف_شوی ICUNOTIF_H
#تعریف_کړه ICUNOTIF_H

#ضمن "unicode/utypes.h"

#إذا UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_پای_ټکی

#وإلا

#ضمن "unicode/uobject.h"
#ضمن "unicode/unistr.h"

#ضمن "mutex.h"
#ضمن "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : عام UObject {
عام: 
    مجازي ~EventListener();

عام:
    ساكن UClassID U_EXPORT2 getStaticClassID();

    مجازي UClassID getDynamicClassID() ثابت override;

عام:
#که_تعریف_شوی SERVICE_DEBUG
    مجازي UnicodeString& debug(UnicodeString& پایله) ثابت {
      أرجع debugClass(پایله);
    }

    مجازي UnicodeString& debugClass(UnicodeString& پایله) ثابت {
      أرجع پایله.append((UnicodeString)"Key");
    }
#د_که_پای
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

class U_COMMON_API ICUNotifier : عام UMemory  {
خاص: UVector* listeners;
         
عام: 
    ICUNotifier();
    
    مجازي ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    مجازي فراغ جمع_کړه_Listener(ثابت EventListener* l, UError_کوډ& حالت);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    مجازي فراغ removeListener(ثابت EventListener* l, UError_کوډ& حالت);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    مجازي فراغ notify_بدل_شوی();
    
محمي: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    مجازي UBool مني_Listener(ثابت EventListener& l) ثابت = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    مجازي فراغ notifyListener(EventListener& l) ثابت = 0;
};

U_NAMESPACE_پای_ټکی

/* UCONFIG_NO_SERVICE */
#د_که_پای

/* ICUNOTIF_H */
#د_که_پای
