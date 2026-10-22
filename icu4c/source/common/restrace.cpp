// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن "unicode/utypes.h"

#إذا U_ENABLE_TRACING

#ضمن "restrace.h"
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "utracimp.h"
#ضمن "uresimp.h"
#ضمن "uassert.h"
#ضمن "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = افتراضي;

فراغ ResourceTracer::trace(ثابت حرف* resType) ثابت {
    U_ASSERT(fResB || f_والد);
    UTRACE_مدخل(UTRACE_UDATA_RESOURCE);
    UError_کد وضعیت = U_ZERO_ERROR;

    CharString file_مسیر;
    get_File_مسیر(file_مسیر, وضعیت);

    CharString res_مسیر;
    get_Res_مسیر(res_مسیر, وضعیت);

    // The longest type ("intvector") is 9 chars
    ثابت حرف kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, حجم(kSpaces) - 1 - uprv_strlen(resType), وضعیت);
    format.append("(%s) %s @ %s", وضعیت);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.داده(),
        resType,
        file_مسیر.داده(),
        res_مسیر.داده());
    UTRACE_EXIT_وضعیت(وضعیت);
}

فراغ ResourceTracer::traceOpen() ثابت {
    U_ASSERT(fResB);
    UTRACE_مدخل(UTRACE_UDATA_BUNDLE);
    UError_کد وضعیت = U_ZERO_ERROR;

    CharString file_مسیر;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", get_File_مسیر(file_مسیر, وضعیت).داده());
    UTRACE_EXIT_وضعیت(وضعیت);
}

CharString& ResourceTracer::get_File_مسیر(CharString& output, UError_کد& وضعیت) ثابت {
    إذا (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->f_داده->f_مسیر, وضعیت);
        output.append('/', وضعیت);
        output.append(fResB->f_داده->fName, وضعیت);
        output.append(".res", وضعیت);
    } وإلا {
        f_والد->get_File_مسیر(output, وضعیت);
    }
    أرجع output;
}

CharString& ResourceTracer::get_Res_مسیر(CharString& output, UError_کد& وضعیت) ثابت {
    إذا (fResB) {
        output.append('/', وضعیت);
        output.append(fResB->f_Res_مسیر, وضعیت);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } وإلا {
        f_والد->get_Res_مسیر(output, وضعیت);
    }
    إذا (f_کلید) {
        output.append('/', وضعیت);
        output.append(f_کلید, وضعیت);
    }
    إذا (f_شاخص != -1) {
        output.append('[', وضعیت);
        UnicodeString شاخص_String;
        ICU_Utility::append_عدد(شاخص_String, f_شاخص);
        output.appendInvariantChars(شاخص_String, وضعیت);
        output.append(']', وضعیت);
    }
    أرجع output;
}

فراغ FileTracer::traceOpen(ثابت حرف* مسیر, ثابت حرف* type, ثابت حرف* name) {
    إذا (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(مسیر, name);
    } وإلا {
        trace_Open_داده_File(مسیر, type, name);
    }
}

فراغ FileTracer::trace_Open_داده_File(ثابت حرف* مسیر, ثابت حرف* type, ثابت حرف* name) {
    UTRACE_مدخل(UTRACE_UDATA_داده_FILE);
    UError_کد وضعیت = U_ZERO_ERROR;

    CharString file_مسیر;
    file_مسیر.append(مسیر, وضعیت);
    file_مسیر.append('/', وضعیت);
    file_مسیر.append(name, وضعیت);
    file_مسیر.append('.', وضعیت);
    file_مسیر.append(type, وضعیت);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_مسیر.داده());
    UTRACE_EXIT_وضعیت(وضعیت);
}

فراغ FileTracer::traceOpenResFile(ثابت حرف* مسیر, ثابت حرف* name) {
    UTRACE_مدخل(UTRACE_UDATA_RES_FILE);
    UError_کد وضعیت = U_ZERO_ERROR;

    CharString file_مسیر;
    file_مسیر.append(مسیر, وضعیت);
    file_مسیر.append('/', وضعیت);
    file_مسیر.append(name, وضعیت);
    file_مسیر.append(".res", وضعیت);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_مسیر.داده());
    UTRACE_EXIT_وضعیت(وضعیت);
}

U_NAMESPACE_انتها

#پایان_اگر // U_ENABLE_TRACING
