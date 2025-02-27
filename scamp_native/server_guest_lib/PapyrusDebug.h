#pragma once
#include "IPapyrusClass.h"
#include "SpSnippetFunctionGen.h"

class PapyrusDebug : public IPapyrusClass<PapyrusDebug>
{
public:
  const char* GetName() { return "debug"; }

  DEFINE_STATIC_SPSNIPPET(Notification);

  VarValue SendAnimationEvent(VarValue self,
                              const std::vector<VarValue>& arguments);

  void Register(VirtualMachine& vm,
                std::shared_ptr<IPapyrusCompatibilityPolicy> policy) override
  {
    compatibilityPolicy = policy;

    AddStatic(vm, "Notification", &PapyrusDebug::Notification);
    AddStatic(vm, "SendAnimationEvent", &PapyrusDebug::SendAnimationEvent);
  }

  std::shared_ptr<IPapyrusCompatibilityPolicy> compatibilityPolicy;
};