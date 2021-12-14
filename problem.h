#include <nan.h>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <map>

using namespace std;
using namespace v8;

string ProblemKeys[] = {"_id", "id", "name", "oj", "url", "originalUrl", "fullName",
                        "timelimit", "memorylimit", "author", "supportedLangs"};

class Problem
{
public:
  Problem(Local<Object> obj);
  bool match(PartialName *);

  PartialName *m_fullName;
  string m_id;
  map<string, string> m_obj;
};

string unwrap(Local<Object> &obj, string key)
{
  Local<String> str =
      Nan::To<String>(Nan::Get(obj, Nan::New(key).ToLocalChecked())
                          .ToLocalChecked())
          .ToLocalChecked();
  return *String::Utf8Value(obj->GetIsolate(), str);
}

Problem::Problem(Local<Object> obj)
{
  m_fullName = new PartialName(unwrap(obj, "fullName"));
  m_id = unwrap(obj, "_id");
  for (auto key : ProblemKeys)
  {
    m_obj[key] = unwrap(obj, key);
  }
}

set<string> inserted;
bool Problem::match(PartialName *pn)
{
  vector<uint16_t> &v = m_fullName->v;
  vector<uint16_t> &v2 = pn->v;
  int lim_j = v2.size();
  int lim_i = v.size() - lim_j + 1;
  bool can;
  for (int i = 0; i < lim_i; i++)
  {
    can = true;
    for (int j = 0; j < lim_j; j++)
    {
      if (v[i + j] != v2[j])
      {
        can = false;
        break;
      }
    }
    if (can)
    {
      return inserted.count(m_id) == 0;
    }
  }
  return false;
}

class ProblemPool
{
public:
  ProblemPool();
  void AddProblem(Local<Object>);
  void SetMaxReturnSize(int);
  Local<Array> FindMatches(Local<Context> context, PartialName *, set<string>);

private:
  vector<Problem *> pool;
  int m_maxReturnSize = 16;
};

ProblemPool::ProblemPool() {}

void ProblemPool::SetMaxReturnSize(int maxReturnSize)
{
  m_maxReturnSize = maxReturnSize;
}

void ProblemPool::AddProblem(Local<Object> obj)
{
  Problem *p = new Problem(obj);
  pool.push_back(p);
}

Local<Array> ProblemPool::FindMatches(Local<Context> context, PartialName *pn, set<string> inserted)
{
  Local<Array> ret = Nan::New<Array>();
  int sz = 0;
  for (auto p : pool)
  {
    if (p->match(pn) && !inserted.count(p->m_id))
    {
      Local<Object> obj = Nan::New<Object>();
      for (auto keyValue : p->m_obj)
      {
        obj->Set(context, Nan::New(keyValue.first).ToLocalChecked(), Nan::New(keyValue.second).ToLocalChecked());
      }
      ret->Set(context, sz++, obj);
      if (sz >= m_maxReturnSize)
        break;
    }
  }
  return ret;
}
