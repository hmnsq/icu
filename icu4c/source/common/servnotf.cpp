// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "servnotf.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NOTIFIER_DEBUG
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ಸ್ಥಿರ UMutex notify_ಬೀಗ;

ICUNotifier::ICUNotifier() 
: listeners(ಶೂನ್ಯಸೂಚಕ) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notify_ಬೀಗ);
        ಅಳಿಸು listeners;
        listeners = ಶೂನ್ಯಸೂಚಕ;
    }
}


ಶೂನ್ಯ 
ICUNotifier::ಕೂಡಿಸು_Listener(ಸ್ಥಿರಾಂಕ EventListener* l, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) 
{
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (l == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }

        ಆದರೆ (ಸ್ವೀಕರಿಸು_Listener(*l)) {
            Mutex lmx(&notify_ಬೀಗ);
            ಆದರೆ (listeners == ಶೂನ್ಯಸೂಚಕ) {
                Local_ಸೂಚಕ<UVector> lpListeners(ಹೊಸ UVector(5, ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
                ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                    ಹಿಂತಿರುಗಿ;
                }
                listeners = lpListeners.orphan();
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0, e = listeners->size(); i < e; ++i) {
                    ಸ್ಥಿರಾಂಕ EventListener* el = static_cast<ಸ್ಥಿರಾಂಕ EventListener*>(listeners->elementAt(i));
                    ಆದರೆ (l == el) {
                        ಹಿಂತಿರುಗಿ;
                    }
                }
            }

            listeners->ಕೂಡಿಸು_Element((ಶೂನ್ಯ*)l, ಸ್ಥಿತಿ_2); // cast away const
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NOTIFIER_DEBUG
        ಇಲ್ಲದಿದ್ದರೆ {
            fprintf(stderr, "Listener invalid for this notifier.");
            ನಿರ್ಗಮಿಸು(1);
        }
#ಆದರೆ_ಅಂತ್ಯ
    }
}

ಶೂನ್ಯ 
ICUNotifier::removeListener(ಸ್ಥಿರಾಂಕ EventListener *l, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) 
{
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (l == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }

        {
            Mutex lmx(&notify_ಬೀಗ);
            ಆದರೆ (listeners != ಶೂನ್ಯಸೂಚಕ) {
                // identity equality check
                ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0, e = listeners->size(); i < e; ++i) {
                    ಸ್ಥಿರಾಂಕ EventListener* el = static_cast<ಸ್ಥಿರಾಂಕ EventListener*>(listeners->elementAt(i));
                    ಆದರೆ (l == el) {
                        listeners->removeElementAt(i);
                        ಆದರೆ (listeners->size() == 0) {
                            ಅಳಿಸು listeners;
                            listeners = ಶೂನ್ಯಸೂಚಕ;
                        }
                        ಹಿಂತಿರುಗಿ;
                    }
                }
            }
        }
    }
}

ಶೂನ್ಯ 
ICUNotifier::notify_ಬದಲಾಯಿತು() 
{
    Mutex lmx(&notify_ಬೀಗ);
    ಆದರೆ (listeners != ಶೂನ್ಯಸೂಚಕ) {
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_ಕೊನೆ

/* UCONFIG_NO_SERVICE */
#ಆದರೆ_ಅಂತ್ಯ

