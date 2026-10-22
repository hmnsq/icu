// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_SERVICE

#అన్తర్భావ "servnotf.h"
#నిర్వచితమైతే NOTIFIER_DEBUG
#అన్తర్భావ <stdio.h>
#అయితే_ముగింపు

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

స్థిర UMutex notify_తాళం;

ICUNotifier::ICUNotifier() 
: listeners(శూన్యసూచిక) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notify_తాళం);
        తొలగించు listeners;
        listeners = శూన్యసూచిక;
    }
}


శూన్య 
ICUNotifier::కూడు_Listener(స్థిరరాశి EventListener* l, UError_సంకేతం& స్థితి_2) 
{
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        ఒకవేళ (l == శూన్యసూచిక) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }

        ఒకవేళ (స్వీకరించు_Listener(*l)) {
            Mutex lmx(&notify_తాళం);
            ఒకవేళ (listeners == శూన్యసూచిక) {
                Local_సూచిక<UVector> lpListeners(కొత్త UVector(5, స్థితి_2), స్థితి_2);
                ఒకవేళ (U_FAILURE(స్థితి_2)) {
                    తిరిగి;
                }
                listeners = lpListeners.orphan();
            } లేకపోతే {
                కొరకు (పూర్ణాంకం i = 0, e = listeners->size(); i < e; ++i) {
                    స్థిరరాశి EventListener* el = static_cast<స్థిరరాశి EventListener*>(listeners->elementAt(i));
                    ఒకవేళ (l == el) {
                        తిరిగి;
                    }
                }
            }

            listeners->కూడు_Element((శూన్య*)l, స్థితి_2); // cast away const
        }
#నిర్వచితమైతే NOTIFIER_DEBUG
        లేకపోతే {
            fprintf(stderr, "Listener invalid for this notifier.");
            నిష్క్రమించు(1);
        }
#అయితే_ముగింపు
    }
}

శూన్య 
ICUNotifier::removeListener(స్థిరరాశి EventListener *l, UError_సంకేతం& స్థితి_2) 
{
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        ఒకవేళ (l == శూన్యసూచిక) {
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }

        {
            Mutex lmx(&notify_తాళం);
            ఒకవేళ (listeners != శూన్యసూచిక) {
                // identity equality check
                కొరకు (పూర్ణాంకం i = 0, e = listeners->size(); i < e; ++i) {
                    స్థిరరాశి EventListener* el = static_cast<స్థిరరాశి EventListener*>(listeners->elementAt(i));
                    ఒకవేళ (l == el) {
                        listeners->removeElementAt(i);
                        ఒకవేళ (listeners->size() == 0) {
                            తొలగించు listeners;
                            listeners = శూన్యసూచిక;
                        }
                        తిరిగి;
                    }
                }
            }
        }
    }
}

శూన్య 
ICUNotifier::notify_మారింది() 
{
    Mutex lmx(&notify_తాళం);
    ఒకవేళ (listeners != శూన్యసూచిక) {
        కొరకు (పూర్ణాంకం i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_చివర

/* UCONFIG_NO_SERVICE */
#అయితే_ముగింపు

