// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_SERVICE

#ضمن "servnotf.h"
#جيڪڏهن_وصف_ٿيل NOTIFIER_DEBUG
#ضمن <stdio.h>
#جيڪڏهن_پورو

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ساكن UMutex notify_تالو;

ICUNotifier::ICUNotifier() 
: listeners(خالي_اشارو) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notify_تالو);
        ميٽيو listeners;
        listeners = خالي_اشارو;
    }
}


فراغ 
ICUNotifier::جوڙيو_Listener(ثابت EventListener* l, UError_ڪوڊ& حالت) 
{
    إذا (U_SUCCESS(حالت)) {
        إذا (l == خالي_اشارو) {
            حالت = U_ILLEGAL_دليل_ERROR;
            أرجع;
        }

        إذا (قبولي_Listener(*l)) {
            Mutex lmx(&notify_تالو);
            إذا (listeners == خالي_اشارو) {
                Local_اشارو<UVector> lpListeners(جديد UVector(5, حالت), حالت);
                إذا (U_FAILURE(حالت)) {
                    أرجع;
                }
                listeners = lpListeners.orphan();
            } وإلا {
                لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    إذا (l == el) {
                        أرجع;
                    }
                }
            }

            listeners->جوڙيو_Element((فراغ*)l, حالت); // cast away const
        }
#جيڪڏهن_وصف_ٿيل NOTIFIER_DEBUG
        وإلا {
            fprintf(stderr, "Listener invalid for this notifier.");
            ٻاهر_نڪرو(1);
        }
#جيڪڏهن_پورو
    }
}

فراغ 
ICUNotifier::removeListener(ثابت EventListener *l, UError_ڪوڊ& حالت) 
{
    إذا (U_SUCCESS(حالت)) {
        إذا (l == خالي_اشارو) {
            حالت = U_ILLEGAL_دليل_ERROR;
            أرجع;
        }

        {
            Mutex lmx(&notify_تالو);
            إذا (listeners != خالي_اشارو) {
                // identity equality check
                لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    إذا (l == el) {
                        listeners->removeElementAt(i);
                        إذا (listeners->size() == 0) {
                            ميٽيو listeners;
                            listeners = خالي_اشارو;
                        }
                        أرجع;
                    }
                }
            }
        }
    }
}

فراغ 
ICUNotifier::notify_بدليو() 
{
    Mutex lmx(&notify_تالو);
    إذا (listeners != خالي_اشارو) {
        لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_پڇاڙي

/* UCONFIG_NO_SERVICE */
#جيڪڏهن_پورو

