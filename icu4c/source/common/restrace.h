// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#నిర్వచితం_కాకపోతే __RESTRACE_H__
#నిర్వచించు __RESTRACE_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_ENABLE_TRACING

నిర్మాణం UResourceBundle;

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
బహిరంగ:
    ResourceTracer() :
        fResB(nullptr),
        f_మాతృ(nullptr),
        f_కీలకం(nullptr),
        f_సూచీ(-1) {}

    ResourceTracer(స్థిరరాశి UResourceBundle* resB) :
        fResB(resB),
        f_మాతృ(nullptr),
        f_కీలకం(nullptr),
        f_సూచీ(-1) {}

    ResourceTracer(స్థిరరాశి UResourceBundle* resB, స్థిరరాశి అక్షరం* కీలకం) :
        fResB(resB),
        f_మాతృ(nullptr),
        f_కీలకం(కీలకం),
        f_సూచీ(-1) {}

    ResourceTracer(స్థిరరాశి UResourceBundle* resB, int32_t సూచీ) :
        fResB(resB),
        f_మాతృ(nullptr),
        f_కీలకం(nullptr),
        f_సూచీ(సూచీ) {}

    ResourceTracer(స్థిరరాశి ResourceTracer& మాతృ, స్థిరరాశి అక్షరం* కీలకం) :
        fResB(nullptr),
        f_మాతృ(&మాతృ),
        f_కీలకం(కీలకం),
        f_సూచీ(-1) {}

    ResourceTracer(స్థిరరాశి ResourceTracer& మాతృ, int32_t సూచీ) :
        fResB(nullptr),
        f_మాతృ(&మాతృ),
        f_కీలకం(nullptr),
        f_సూచీ(సూచీ) {}

    ~ResourceTracer();

    శూన్య trace(స్థిరరాశి అక్షరం* type) స్థిరరాశి;
    శూన్య traceOpen() స్థిరరాశి;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    శూన్య maybeTrace(స్థిరరాశి అక్షరం* type) స్థిరరాశి {
        ఒకవేళ (fResB || f_మాతృ) {
            trace(type);
        }
    }

అంతరంగ:
    స్థిరరాశి UResourceBundle* fResB;
    స్థిరరాశి ResourceTracer* f_మాతృ;
    స్థిరరాశి అక్షరం* f_కీలకం;
    int32_t f_సూచీ;

    CharString& get_File_మార్గం(CharString& output, UError_సంకేతం& స్థితి_2) స్థిరరాశి;

    CharString& get_Res_మార్గం(CharString& output, UError_సంకేతం& స్థితి_2) స్థిరరాశి;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
బహిరంగ:
    స్థిర శూన్య traceOpen(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* type, స్థిరరాశి అక్షరం* name);

అంతరంగ:
    స్థిర శూన్య trace_Open_దత్తాంశం_File(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* type, స్థిరరాశి అక్షరం* name);
    స్థిర శూన్య traceOpenResFile(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* name);
};

U_NAMESPACE_చివర

#లేకపోతే // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
బహిరంగ:
    ResourceTracer() {}

    ResourceTracer(స్థిరరాశి శూన్య*) {}

    ResourceTracer(స్థిరరాశి శూన్య*, స్థిరరాశి అక్షరం*) {}

    ResourceTracer(స్థిరరాశి శూన్య*, int32_t) {}

    ResourceTracer(స్థిరరాశి ResourceTracer&, స్థిరరాశి అక్షరం*) {}

    ResourceTracer(స్థిరరాశి ResourceTracer&, int32_t) {}

    శూన్య trace(స్థిరరాశి అక్షరం*) స్థిరరాశి {}

    శూన్య traceOpen() స్థిరరాశి {}

    శూన్య maybeTrace(స్థిరరాశి అక్షరం*) స్థిరరాశి {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
బహిరంగ:
    స్థిర శూన్య traceOpen(స్థిరరాశి అక్షరం*, స్థిరరాశి అక్షరం*, స్థిరరాశి అక్షరం*) {}
};

U_NAMESPACE_చివర

#అయితే_ముగింపు // U_ENABLE_TRACING

#అయితే_ముగింపు //__RESTRACE_H__
