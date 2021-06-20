#pragma once
#include <cstdint>

#include "Structures.h"


class MpActor;

class SpSnippet
{
public:
  SpSnippet(const char* cl_, const char* func_, const char* args_,
            uint32_t selfId_ = 0);
  Viet::Promise<VarValue> Execute(MpActor* actor);

private:
  const char *const cl, *const func, *const args;
  const uint32_t selfId;
};