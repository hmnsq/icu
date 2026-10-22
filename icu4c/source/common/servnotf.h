// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
#నిర్వచితం_కాకపోతే ICUNOTIF_H
#నిర్వచించు ICUNOTIF_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_చివర

#లేకపోతే

#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/unistr.h"

#అన్తర్భావ "mutex.h"
#అన్తర్భావ "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : బహిరంగ UObject {
బహిరంగ: 
    మాయా ~EventListener();

బహిరంగ:
    స్థిర UClassID U_EXPORT2 getStaticClassID();

    మాయా UClassID getDynamicClassID() స్థిరరాశి override;

బహిరంగ:
#నిర్వచితమైతే SERVICE_DEBUG
    మాయా UnicodeString& debug(UnicodeString& ఫలితం) స్థిరరాశి {
      తిరిగి debugClass(ఫలితం);
    }

    మాయా UnicodeString& debugClass(UnicodeString& ఫలితం) స్థిరరాశి {
      తిరిగి ఫలితం.append((UnicodeString)"Key");
    }
#అయితే_ముగింపు
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

class U_COMMON_API ICUNotifier : బహిరంగ UMemory  {
అంతరంగ: UVector* listeners;
         
బహిరంగ: 
    ICUNotifier();
    
    మాయా ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    మాయా శూన్య కూడు_Listener(స్థిరరాశి EventListener* l, UError_సంకేతం& స్థితి_2);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    మాయా శూన్య removeListener(స్థిరరాశి EventListener* l, UError_సంకేతం& స్థితి_2);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    మాయా శూన్య notify_మారింది();
    
రక్షిత: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    మాయా UBool స్వీకరించు_Listener(స్థిరరాశి EventListener& l) స్థిరరాశి = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    మాయా శూన్య notifyListener(EventListener& l) స్థిరరాశి = 0;
};

U_NAMESPACE_చివర

/* UCONFIG_NO_SERVICE */
#అయితే_ముగింపు

/* ICUNOTIF_H */
#అయితే_ముగింపు
