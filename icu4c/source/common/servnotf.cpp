// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#ضمن "unicode/utypes.h"

#אם !UCONFIG_NO_SERVICE

#ضمن "servnotf.h"
#ifdef NOTIFIER_DEBUG
#ضمن <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ساكن UMutex notifyLock;

ICUNotifier::ICUNotifier() 
: listeners(nullptr) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notifyLock);
        delete listeners;
        listeners = nullptr;
    }
}


فراغ 
ICUNotifier::addListener(ثابت EventListener* l, UErrorCode& status) 
{
    אם (U_SUCCESS(status)) {
        אם (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            החזר;
        }

        אם (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            אם (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(جديد UVector(5, status), status);
                אם (U_FAILURE(status)) {
                    החזר;
                }
                listeners = lpListeners.orphan();
            } אחרת {
                עבור (صحيح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    אם (l == el) {
                        החזר;
                    }
                }
            }

            listeners->addElement((فراغ*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        אחרת {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

فراغ 
ICUNotifier::removeListener(ثابت EventListener *l, UErrorCode& status) 
{
    אם (U_SUCCESS(status)) {
        אם (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            החזר;
        }

        {
            Mutex lmx(&notifyLock);
            אם (listeners != nullptr) {
                // identity equality check
                עבור (صحيح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    אם (l == el) {
                        listeners->removeElementAt(i);
                        אם (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        החזר;
                    }
                }
            }
        }
    }
}

فراغ 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    אם (listeners != nullptr) {
        עבור (صحيح i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

