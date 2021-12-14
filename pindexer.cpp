#include <nan.h>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <iostream>

#include "partialname.h"
#include "problem.h"

using namespace std;

ProblemPool *pool;
int maxReturnSize;

void Match(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  auto *isolate = args.GetIsolate();
  auto context = isolate->GetCurrentContext();
  if (args.Length() < 1)
  {
    Nan::ThrowError("Wrong number of arguments");
    return;
  }

  Nan::MaybeLocal<v8::String> maybeStr = Nan::To<v8::String>(args[0]);
  v8::Local<v8::String> str;
  if (!maybeStr.ToLocal(&str))
  {
    Nan::ThrowTypeError("Error converting first argument to string");
    return;
  }

  set<string> insertedProblemsSet;
  v8::Local<v8::Array> insertedElementsArray = v8::Local<v8::Array>::Cast(args[1]);
  for (unsigned int i = 0; i < insertedElementsArray->Length(); i++)
  {
    v8::Local<v8::String> elem =
        v8::Local<v8::String>::Cast(insertedElementsArray->Get(context, i).ToLocalChecked());
    insertedProblemsSet.insert(*v8::String::Utf8Value(isolate, elem));
  }

  PartialName *pn = new PartialName(*v8::String::Utf8Value(isolate, str));
  args.GetReturnValue().Set(pool->FindMatches(context, pn, insertedProblemsSet));
}

void SetReturnSize(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  auto *isolate = args.GetIsolate();
  auto context = isolate->GetCurrentContext();
  if (!args[0]->IsNumber())
  {
    Nan::ThrowTypeError("Argument is not a number");
    return;
  }
  pool->SetMaxReturnSize(args[0]->NumberValue(context).FromMaybe(0));
}

void AddProblem(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
  if (args.Length() != 1)
  {
    Nan::ThrowError("Wrong number of arguments");
    return;
  }

  Nan::MaybeLocal<v8::Object> maybeObj = Nan::To<v8::Object>(args[0]);

  v8::Local<v8::Object> obj;
  if (!maybeObj.ToLocal(&obj))
  {
    Nan::ThrowError("Error converting parameter to local object");
    return;
  }

  pool->AddProblem(obj);
}

void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module)
{
  pool = new ProblemPool();
  v8::Local<v8::Context> context = exports->CreationContext();
  exports->Set(context,
               Nan::New("match").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Match)
                   ->GetFunction(context)
                   .ToLocalChecked());
  exports->Set(context,
               Nan::New("setReturnSize").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(SetReturnSize)
                   ->GetFunction(context)
                   .ToLocalChecked());
  exports->Set(context,
               Nan::New("addProblem").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(AddProblem)
                   ->GetFunction(context)
                   .ToLocalChecked());
}

NODE_MODULE(myaddon, Init);