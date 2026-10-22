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
#که_تعریف_شوی NOTIFIER_DEBUG
#ضمن <stdio.h>
#د_که_پای

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ساكن UMutex notify_قلف;

ICUNotifier::ICUNotifier() 
: listeners(تش_اشاره) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notify_قلف);
        ړنګول listeners;
        listeners = تش_اشاره;
    }
}


فراغ 
ICUNotifier::جمع_کړه_Listener(ثابت EventListener* l, UError_کوډ& حالت) 
{
    إذا (U_SUCCESS(حالت)) {
        إذا (l == تش_اشاره) {
            حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }

        إذا (مني_Listener(*l)) {
            Mutex lmx(&notify_قلف);
            إذا (listeners == تش_اشاره) {
                Local_اشاره_ګر<UVector> lpListeners(جديد UVector(5, حالت), حالت);
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

            listeners->جمع_کړه_Element((فراغ*)l, حالت); // cast away const
        }
#که_تعریف_شوی NOTIFIER_DEBUG
        وإلا {
            fprintf(stderr, "Listener invalid for this notifier.");
            وتل(1);
        }
#د_که_پای
    }
}

فراغ 
ICUNotifier::removeListener(ثابت EventListener *l, UError_کوډ& حالت) 
{
    إذا (U_SUCCESS(حالت)) {
        إذا (l == تش_اشاره) {
            حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }

        {
            Mutex lmx(&notify_قلف);
            إذا (listeners != تش_اشاره) {
                // identity equality check
                لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    إذا (l == el) {
                        listeners->removeElementAt(i);
                        إذا (listeners->size() == 0) {
                            ړنګول listeners;
                            listeners = تش_اشاره;
                        }
                        أرجع;
                    }
                }
            }
        }
    }
}

فراغ 
ICUNotifier::notify_بدل_شوی() 
{
    Mutex lmx(&notify_قلف);
    إذا (listeners != تش_اشاره) {
        لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_پای_ټکی

/* UCONFIG_NO_SERVICE */
#د_که_پای

