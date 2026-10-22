// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_SERVICE

#ਅਨ੍ਤਰ੍ਭਾਵ "servnotf.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NOTIFIER_DEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਜੇ_ਅੰਤ

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ਸਥਿਰ UMutex notify_ਤਾਲਾ;

ICUNotifier::ICUNotifier() 
: listeners(ਖਾਲੀ_ਸੂਚਕ) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notify_ਤਾਲਾ);
        ਮਿਟਾਓ listeners;
        listeners = ਖਾਲੀ_ਸੂਚਕ;
    }
}


ਖਾਲੀ 
ICUNotifier::ਜੋੜੋ_Listener(ਅਚਲ EventListener* l, UError_ਕੋਡ& ਸਥਿਤੀ) 
{
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਜੇ (l == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }

        ਜੇ (ਮੰਨਦਾ_Listener(*l)) {
            Mutex lmx(&notify_ਤਾਲਾ);
            ਜੇ (listeners == ਖਾਲੀ_ਸੂਚਕ) {
                Local_ਸੂਚਕ<UVector> lpListeners(ਨਵਾਂ UVector(5, ਸਥਿਤੀ), ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                    ਵਾਪਸ;
                }
                listeners = lpListeners.orphan();
            } ਨਹੀਂਤਾਂ {
                ਲਈ (ਪੂਰਨ i = 0, e = listeners->size(); i < e; ++i) {
                    ਅਚਲ EventListener* el = static_cast<ਅਚਲ EventListener*>(listeners->elementAt(i));
                    ਜੇ (l == el) {
                        ਵਾਪਸ;
                    }
                }
            }

            listeners->ਜੋੜੋ_Element((ਖਾਲੀ*)l, ਸਥਿਤੀ); // cast away const
        }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NOTIFIER_DEBUG
        ਨਹੀਂਤਾਂ {
            fprintf(stderr, "Listener invalid for this notifier.");
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
#ਜੇ_ਅੰਤ
    }
}

ਖਾਲੀ 
ICUNotifier::removeListener(ਅਚਲ EventListener *l, UError_ਕੋਡ& ਸਥਿਤੀ) 
{
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਜੇ (l == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }

        {
            Mutex lmx(&notify_ਤਾਲਾ);
            ਜੇ (listeners != ਖਾਲੀ_ਸੂਚਕ) {
                // identity equality check
                ਲਈ (ਪੂਰਨ i = 0, e = listeners->size(); i < e; ++i) {
                    ਅਚਲ EventListener* el = static_cast<ਅਚਲ EventListener*>(listeners->elementAt(i));
                    ਜੇ (l == el) {
                        listeners->removeElementAt(i);
                        ਜੇ (listeners->size() == 0) {
                            ਮਿਟਾਓ listeners;
                            listeners = ਖਾਲੀ_ਸੂਚਕ;
                        }
                        ਵਾਪਸ;
                    }
                }
            }
        }
    }
}

ਖਾਲੀ 
ICUNotifier::notify_ਬਦਲਿਆ() 
{
    Mutex lmx(&notify_ਤਾਲਾ);
    ਜੇ (listeners != ਖਾਲੀ_ਸੂਚਕ) {
        ਲਈ (ਪੂਰਨ i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_ਸਿਰਾ

/* UCONFIG_NO_SERVICE */
#ਜੇ_ਅੰਤ

