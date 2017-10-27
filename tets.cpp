//============================================================================
// Name        : 决策树.cpp
// Author      : ztftrue
// Version     :
// Copyright   : Your copyright notice
// Description : 本来想用node做服务器，没搞定
//============================================================================

#include <iostream>
#include <opencv2/opencv.hpp>
#include "ImageTools.h"
#include <node6/node.h>

using namespace std;
using namespace cv;
using namespace v8;

namespace opnode {

    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;

    void Method(const FunctionCallbackInfo<Value> &args) {//方法
        Isolate *isolate = args.GetIsolate();
        v8::String::Utf8Value param1(args[0]->ToString());
        std::string from = std::string(*param1);
        Mat src1 = imread(from, IMREAD_COLOR);
        ImageTools imageTools;
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, imageTools.recognition(src1).c_str()));
    }

    void init(Local<Object> exports) {
        NODE_SET_METHOD(exports, "name", Method);//调用Method()
    }

    NODE_MODULE(addon, init)
}
