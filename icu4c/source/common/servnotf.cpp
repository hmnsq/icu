// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_SERVICE

#شامل "servnotf.h"
#اگر_تعریف_شدہ NOTIFIER_DEBUG
#شامل <stdio.h>
#اگر_ختم

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ساکن UMutex notify_تالا;

ICUNotifier::ICUNotifier() 
: listeners(خالی_اشاریہ) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notify_تالا);
        حذف listeners;
        listeners = خالی_اشاریہ;
    }
}


خالی 
ICUNotifier::جوڑو_Listener(ثابت EventListener* l, UError_رمز& حالت_2) 
{
    اگر (U_SUCCESS(حالت_2)) {
        اگر (l == خالی_اشاریہ) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR;
            واپس;
        }

        اگر (قبول_کرتا_Listener(*l)) {
            Mutex lmx(&notify_تالا);
            اگر (listeners == خالی_اشاریہ) {
                Local_اشارہ_گر<UVector> lpListeners(نیا UVector(5, حالت_2), حالت_2);
                اگر (U_FAILURE(حالت_2)) {
                    واپس;
                }
                listeners = lpListeners.orphan();
            } ورنہ {
                برائے (صحیح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    اگر (l == el) {
                        واپس;
                    }
                }
            }

            listeners->جوڑو_Element((خالی*)l, حالت_2); // cast away const
        }
#اگر_تعریف_شدہ NOTIFIER_DEBUG
        ورنہ {
            fprintf(stderr, "Listener invalid for this notifier.");
            باہر_نکلو(1);
        }
#اگر_ختم
    }
}

خالی 
ICUNotifier::removeListener(ثابت EventListener *l, UError_رمز& حالت_2) 
{
    اگر (U_SUCCESS(حالت_2)) {
        اگر (l == خالی_اشاریہ) {
            حالت_2 = U_ILLEGAL_دلیل_ERROR;
            واپس;
        }

        {
            Mutex lmx(&notify_تالا);
            اگر (listeners != خالی_اشاریہ) {
                // identity equality check
                برائے (صحیح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    اگر (l == el) {
                        listeners->removeElementAt(i);
                        اگر (listeners->size() == 0) {
                            حذف listeners;
                            listeners = خالی_اشاریہ;
                        }
                        واپس;
                    }
                }
            }
        }
    }
}

خالی 
ICUNotifier::notify_بدلا() 
{
    Mutex lmx(&notify_تالا);
    اگر (listeners != خالی_اشاریہ) {
        برائے (صحیح i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_انتہا

/* UCONFIG_NO_SERVICE */
#اگر_ختم

