// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __RESTRACE_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __RESTRACE_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_ENABLE_TRACING

ರಚನೆ UResourceBundle;

U_NAMESPACE_BEGIN

class CharString;

/**
 * Instances of this class store information used to trace reads from resource
 * bundles when ICU is built with --enable-tracing.
 *
 * All arguments of type const UResourceBundle*, const char*, and
 * const ResourceTracer& are stored as pointers. The caller must retain
 * ownership for the lifetime of this ResourceTracer.
 *
 * Exported as U_COMMON_API for Windows because it is a value field
 * in other exported types.
 */
class U_COMMON_API ResourceTracer {
ಸಾರ್ವಜನಿಕ:
    ResourceTracer() :
        fResB(nullptr),
        f_ಪೋಷಕ(nullptr),
        f_ಕೀಲಿ(nullptr),
        f_ಸೂಚ್ಯಂಕ(-1) {}

    ResourceTracer(ಸ್ಥಿರಾಂಕ UResourceBundle* resB) :
        fResB(resB),
        f_ಪೋಷಕ(nullptr),
        f_ಕೀಲಿ(nullptr),
        f_ಸೂಚ್ಯಂಕ(-1) {}

    ResourceTracer(ಸ್ಥಿರಾಂಕ UResourceBundle* resB, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ) :
        fResB(resB),
        f_ಪೋಷಕ(nullptr),
        f_ಕೀಲಿ(ಕೀಲಿ),
        f_ಸೂಚ್ಯಂಕ(-1) {}

    ResourceTracer(ಸ್ಥಿರಾಂಕ UResourceBundle* resB, int32_t ಸೂಚ್ಯಂಕ) :
        fResB(resB),
        f_ಪೋಷಕ(nullptr),
        f_ಕೀಲಿ(nullptr),
        f_ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ) {}

    ResourceTracer(ಸ್ಥಿರಾಂಕ ResourceTracer& ಪೋಷಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ) :
        fResB(nullptr),
        f_ಪೋಷಕ(&ಪೋಷಕ),
        f_ಕೀಲಿ(ಕೀಲಿ),
        f_ಸೂಚ್ಯಂಕ(-1) {}

    ResourceTracer(ಸ್ಥಿರಾಂಕ ResourceTracer& ಪೋಷಕ, int32_t ಸೂಚ್ಯಂಕ) :
        fResB(nullptr),
        f_ಪೋಷಕ(&ಪೋಷಕ),
        f_ಕೀಲಿ(nullptr),
        f_ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ) {}

    ~ResourceTracer();

    ಶೂನ್ಯ trace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type) ಸ್ಥಿರಾಂಕ;
    ಶೂನ್ಯ traceOpen() ಸ್ಥಿರಾಂಕ;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    ಶೂನ್ಯ maybeTrace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type) ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (fResB || f_ಪೋಷಕ) {
            trace(type);
        }
    }

ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ UResourceBundle* fResB;
    ಸ್ಥಿರಾಂಕ ResourceTracer* f_ಪೋಷಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* f_ಕೀಲಿ;
    int32_t f_ಸೂಚ್ಯಂಕ;

    CharString& get_File_ಮಾರ್ಗ(CharString& output, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

    CharString& get_Res_ಮಾರ್ಗ(CharString& output, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ಶೂನ್ಯ traceOpen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name);

ಖಾಸಗಿ:
    ಸ್ಥಿರ ಶೂನ್ಯ trace_Open_ದತ್ತಾಂಶ_File(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name);
    ಸ್ಥಿರ ಶೂನ್ಯ traceOpenResFile(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name);
};

U_NAMESPACE_ಕೊನೆ

#ಇಲ್ಲದಿದ್ದರೆ // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
ಸಾರ್ವಜನಿಕ:
    ResourceTracer() {}

    ResourceTracer(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*) {}

    ResourceTracer(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) {}

    ResourceTracer(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*, int32_t) {}

    ResourceTracer(ಸ್ಥಿರಾಂಕ ResourceTracer&, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) {}

    ResourceTracer(ಸ್ಥಿರಾಂಕ ResourceTracer&, int32_t) {}

    ಶೂನ್ಯ trace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) ಸ್ಥಿರಾಂಕ {}

    ಶೂನ್ಯ traceOpen() ಸ್ಥಿರಾಂಕ {}

    ಶೂನ್ಯ maybeTrace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) ಸ್ಥಿರಾಂಕ {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ಶೂನ್ಯ traceOpen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) {}
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ // U_ENABLE_TRACING

#ಆದರೆ_ಅಂತ್ಯ //__RESTRACE_H__
