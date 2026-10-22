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
#جے_تعریف_شدہ NOTIFIER_DEBUG
#ضمن <stdio.h>
#جے_انت

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ساكن UMutex notify_جندرا;

ICUNotifier::ICUNotifier() 
: listeners(خالی_سوچک) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notify_جندرا);
        مٹاؤ listeners;
        listeners = خالی_سوچک;
    }
}


فراغ 
ICUNotifier::جوڑو_Listener(ثابت EventListener* l, UError_کوڈ& حالت) 
{
    إذا (U_SUCCESS(حالت)) {
        إذا (l == خالی_سوچک) {
            حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }

        إذا (مندا_Listener(*l)) {
            Mutex lmx(&notify_جندرا);
            إذا (listeners == خالی_سوچک) {
                Local_اشارہ_گر<UVector> lpListeners(جديد UVector(5, حالت), حالت);
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

            listeners->جوڑو_Element((فراغ*)l, حالت); // cast away const
        }
#جے_تعریف_شدہ NOTIFIER_DEBUG
        وإلا {
            fprintf(stderr, "Listener invalid for this notifier.");
            باہر_نکلو(1);
        }
#جے_انت
    }
}

فراغ 
ICUNotifier::removeListener(ثابت EventListener *l, UError_کوڈ& حالت) 
{
    إذا (U_SUCCESS(حالت)) {
        إذا (l == خالی_سوچک) {
            حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }

        {
            Mutex lmx(&notify_جندرا);
            إذا (listeners != خالی_سوچک) {
                // identity equality check
                لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    إذا (l == el) {
                        listeners->removeElementAt(i);
                        إذا (listeners->size() == 0) {
                            مٹاؤ listeners;
                            listeners = خالی_سوچک;
                        }
                        أرجع;
                    }
                }
            }
        }
    }
}

فراغ 
ICUNotifier::notify_بدلیا() 
{
    Mutex lmx(&notify_جندرا);
    إذا (listeners != خالی_سوچک) {
        لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_سرا

/* UCONFIG_NO_SERVICE */
#جے_انت

