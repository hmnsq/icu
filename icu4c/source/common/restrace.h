// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#اگر_غیر_تعریف_شدہ __RESTRACE_H__
#تعریف_کرو __RESTRACE_H__

#شامل "unicode/utypes.h"

#اگر U_ENABLE_TRACING

ساخت UResourceBundle;

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
عوامی:
    ResourceTracer() :
        fResB(nullptr),
        f_والد(nullptr),
        f_کلید(nullptr),
        f_اشاریہ(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB) :
        fResB(resB),
        f_والد(nullptr),
        f_کلید(nullptr),
        f_اشاریہ(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB, ثابت حرف* کلید) :
        fResB(resB),
        f_والد(nullptr),
        f_کلید(کلید),
        f_اشاریہ(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB, int32_t اشاریہ) :
        fResB(resB),
        f_والد(nullptr),
        f_کلید(nullptr),
        f_اشاریہ(اشاریہ) {}

    ResourceTracer(ثابت ResourceTracer& والد, ثابت حرف* کلید) :
        fResB(nullptr),
        f_والد(&والد),
        f_کلید(کلید),
        f_اشاریہ(-1) {}

    ResourceTracer(ثابت ResourceTracer& والد, int32_t اشاریہ) :
        fResB(nullptr),
        f_والد(&والد),
        f_کلید(nullptr),
        f_اشاریہ(اشاریہ) {}

    ~ResourceTracer();

    خالی trace(ثابت حرف* type) ثابت;
    خالی traceOpen() ثابت;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    خالی maybeTrace(ثابت حرف* type) ثابت {
        اگر (fResB || f_والد) {
            trace(type);
        }
    }

نجی:
    ثابت UResourceBundle* fResB;
    ثابت ResourceTracer* f_والد;
    ثابت حرف* f_کلید;
    int32_t f_اشاریہ;

    CharString& get_File_راستہ(CharString& output, UError_رمز& حالت_2) ثابت;

    CharString& get_Res_راستہ(CharString& output, UError_رمز& حالت_2) ثابت;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
عوامی:
    ساکن خالی traceOpen(ثابت حرف* راستہ, ثابت حرف* type, ثابت حرف* name);

نجی:
    ساکن خالی trace_Open_ڈیٹا_File(ثابت حرف* راستہ, ثابت حرف* type, ثابت حرف* name);
    ساکن خالی traceOpenResFile(ثابت حرف* راستہ, ثابت حرف* name);
};

U_NAMESPACE_انتہا

#ورنہ // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
عوامی:
    ResourceTracer() {}

    ResourceTracer(ثابت خالی*) {}

    ResourceTracer(ثابت خالی*, ثابت حرف*) {}

    ResourceTracer(ثابت خالی*, int32_t) {}

    ResourceTracer(ثابت ResourceTracer&, ثابت حرف*) {}

    ResourceTracer(ثابت ResourceTracer&, int32_t) {}

    خالی trace(ثابت حرف*) ثابت {}

    خالی traceOpen() ثابت {}

    خالی maybeTrace(ثابت حرف*) ثابت {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
عوامی:
    ساکن خالی traceOpen(ثابت حرف*, ثابت حرف*, ثابت حرف*) {}
};

U_NAMESPACE_انتہا

#اگر_ختم // U_ENABLE_TRACING

#اگر_ختم //__RESTRACE_H__
