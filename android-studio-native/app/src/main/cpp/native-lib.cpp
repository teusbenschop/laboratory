#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_org_bibleconsultants_lab_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */)
{
    std::string hello = "Hello from C++ within Bible Consultants Laboratory!";
    return env->NewStringUTF(hello.c_str());
}
