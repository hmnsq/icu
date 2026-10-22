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
#اگر_تعریف_شده NOTIFIER_DEBUG
#ضمن <stdio.h>
#پایان_اگر

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ساكن UMutex notify_قفل;

ICUNotifier::ICUNotifier() 
: listeners(اشاره_تهی) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notify_قفل);
        حذف listeners;
        listeners = اشاره_تهی;
    }
}


فراغ 
ICUNotifier::جمع_کن_Listener(ثابت EventListener* l, UError_کد& وضعیت) 
{
    إذا (U_SUCCESS(وضعیت)) {
        إذا (l == اشاره_تهی) {
            وضعیت = U_ILLEGAL_آرگومان_ERROR;
            أرجع;
        }

        إذا (میپذیرد_Listener(*l)) {
            Mutex lmx(&notify_قفل);
            إذا (listeners == اشاره_تهی) {
                Local_اشاره_گر<UVector> lpListeners(جديد UVector(5, وضعیت), وضعیت);
                إذا (U_FAILURE(وضعیت)) {
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

            listeners->جمع_کن_Element((فراغ*)l, وضعیت); // cast away const
        }
#اگر_تعریف_شده NOTIFIER_DEBUG
        وإلا {
            fprintf(stderr, "Listener invalid for this notifier.");
            خروج(1);
        }
#پایان_اگر
    }
}

فراغ 
ICUNotifier::removeListener(ثابت EventListener *l, UError_کد& وضعیت) 
{
    إذا (U_SUCCESS(وضعیت)) {
        إذا (l == اشاره_تهی) {
            وضعیت = U_ILLEGAL_آرگومان_ERROR;
            أرجع;
        }

        {
            Mutex lmx(&notify_قفل);
            إذا (listeners != اشاره_تهی) {
                // identity equality check
                لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    إذا (l == el) {
                        listeners->removeElementAt(i);
                        إذا (listeners->size() == 0) {
                            حذف listeners;
                            listeners = اشاره_تهی;
                        }
                        أرجع;
                    }
                }
            }
        }
    }
}

فراغ 
ICUNotifier::notify_تغییریافته() 
{
    Mutex lmx(&notify_قفل);
    إذا (listeners != اشاره_تهی) {
        لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_انتها

/* UCONFIG_NO_SERVICE */
#پایان_اگر

