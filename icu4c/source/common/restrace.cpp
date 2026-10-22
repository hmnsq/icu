// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_ENABLE_TRACING

#అన్తర్భావ "restrace.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "utracimp.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = అప్రమేయ;

శూన్య ResourceTracer::trace(స్థిరరాశి అక్షరం* resType) స్థిరరాశి {
    U_ASSERT(fResB || f_మాతృ);
    UTRACE_నమోదు(UTRACE_UDATA_RESOURCE);
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;

    CharString file_మార్గం;
    get_File_మార్గం(file_మార్గం, స్థితి_2);

    CharString res_మార్గం;
    get_Res_మార్గం(res_మార్గం, స్థితి_2);

    // The longest type ("intvector") is 9 chars
    స్థిరరాశి అక్షరం kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, పరిమాణం(kSpaces) - 1 - uprv_strlen(resType), స్థితి_2);
    format.append("(%s) %s @ %s", స్థితి_2);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.దత్తాంశం(),
        resType,
        file_మార్గం.దత్తాంశం(),
        res_మార్గం.దత్తాంశం());
    UTRACE_EXIT_స్థితి(స్థితి_2);
}

శూన్య ResourceTracer::traceOpen() స్థిరరాశి {
    U_ASSERT(fResB);
    UTRACE_నమోదు(UTRACE_UDATA_BUNDLE);
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;

    CharString file_మార్గం;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", get_File_మార్గం(file_మార్గం, స్థితి_2).దత్తాంశం());
    UTRACE_EXIT_స్థితి(స్థితి_2);
}

CharString& ResourceTracer::get_File_మార్గం(CharString& output, UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->f_దత్తాంశం->f_మార్గం, స్థితి_2);
        output.append('/', స్థితి_2);
        output.append(fResB->f_దత్తాంశం->fName, స్థితి_2);
        output.append(".res", స్థితి_2);
    } లేకపోతే {
        f_మాతృ->get_File_మార్గం(output, స్థితి_2);
    }
    తిరిగి output;
}

CharString& ResourceTracer::get_Res_మార్గం(CharString& output, UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (fResB) {
        output.append('/', స్థితి_2);
        output.append(fResB->f_Res_మార్గం, స్థితి_2);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } లేకపోతే {
        f_మాతృ->get_Res_మార్గం(output, స్థితి_2);
    }
    ఒకవేళ (f_కీలకం) {
        output.append('/', స్థితి_2);
        output.append(f_కీలకం, స్థితి_2);
    }
    ఒకవేళ (f_సూచీ != -1) {
        output.append('[', స్థితి_2);
        UnicodeString సూచీ_String;
        ICU_Utility::append_సంఖ్య(సూచీ_String, f_సూచీ);
        output.appendInvariantChars(సూచీ_String, స్థితి_2);
        output.append(']', స్థితి_2);
    }
    తిరిగి output;
}

శూన్య FileTracer::traceOpen(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* type, స్థిరరాశి అక్షరం* name) {
    ఒకవేళ (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(మార్గం, name);
    } లేకపోతే {
        trace_Open_దత్తాంశం_File(మార్గం, type, name);
    }
}

శూన్య FileTracer::trace_Open_దత్తాంశం_File(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* type, స్థిరరాశి అక్షరం* name) {
    UTRACE_నమోదు(UTRACE_UDATA_దత్తాంశం_FILE);
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;

    CharString file_మార్గం;
    file_మార్గం.append(మార్గం, స్థితి_2);
    file_మార్గం.append('/', స్థితి_2);
    file_మార్గం.append(name, స్థితి_2);
    file_మార్గం.append('.', స్థితి_2);
    file_మార్గం.append(type, స్థితి_2);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_మార్గం.దత్తాంశం());
    UTRACE_EXIT_స్థితి(స్థితి_2);
}

శూన్య FileTracer::traceOpenResFile(స్థిరరాశి అక్షరం* మార్గం, స్థిరరాశి అక్షరం* name) {
    UTRACE_నమోదు(UTRACE_UDATA_RES_FILE);
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;

    CharString file_మార్గం;
    file_మార్గం.append(మార్గం, స్థితి_2);
    file_మార్గం.append('/', స్థితి_2);
    file_మార్గం.append(name, స్థితి_2);
    file_మార్గం.append(".res", స్థితి_2);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_మార్గం.దత్తాంశం());
    UTRACE_EXIT_స్థితి(స్థితి_2);
}

U_NAMESPACE_చివర

#అయితే_ముగింపు // U_ENABLE_TRACING
