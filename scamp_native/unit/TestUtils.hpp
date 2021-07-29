#pragma once
#include "BinaryStructures.h"
#include "FormCallbacks.h"
#include "IPapyrusCompatibilityPolicy.h"
#include "MpActor.h"
#include "MsgType.h"
#include "PartOne.h"
#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>
#include <thread>

using namespace Catch;

// Utilities for testing
namespace {
std::string MakeMessage(const nlohmann::json& j)
{
  std::string s;
  s += (char)Networking::MinPacketId;
  s += j.dump();
  return s;
}

void DoMessage(PartOne& partOne, Networking::UserId id,
               const nlohmann::json& j)
{
  std::string s = MakeMessage(j);
  PartOne* ptr = &partOne;
  PartOne::HandlePacket(ptr, id, Networking::PacketType::Message,
                        reinterpret_cast<Networking::PacketData>(s.data()),
                        s.size());
}

void DoMessage(PartOne& partOne, Networking::UserId id, uint8_t messageType, uint8_t* data, size_t dataLength)
{
  auto newData = new uint8_t[dataLength + 1];
  newData[0] = messageType;
  memcpy(&newData[1], data, dataLength);

  PartOne* ptr = &partOne;
  PartOne::HandlePacket(ptr, id, Networking::PacketType::Message, newData,
                        dataLength + sizeof(uint8_t));
}

void DoConnect(PartOne& partOne, Networking::UserId id)
{
  PartOne* ptr = &partOne;
  PartOne::HandlePacket(ptr, id, Networking::PacketType::ServerSideUserConnect,
                        nullptr, 0);
}

void DoDisconnect(PartOne& partOne, Networking::UserId id)
{
  PartOne* ptr = &partOne;
  PartOne::HandlePacket(
    ptr, id, Networking::PacketType::ServerSideUserDisconnect, nullptr, 0);
}

static const auto jMovement =
  nlohmann::json{ { "t", MsgType::UpdateMovement },
                  { "idx", 0 },
                  { "data",
                    { { "worldOrCell", 0x3c },
                      { "pos", { 1, -1, 1 } },
                      { "rot", { 0, 0, 179 } },
                      { "runMode", "Standing" },
                      { "direction", 0 },
                      { "isInJumpState", false },
                      { "isSneaking", false },
                      { "isBlocking", false },
                      { "isWeapDrawn", false } } } };

//uint16_t packedAngleZ = static_cast<uint16_t>((angleZ / 360.f)*65535.f)
static const auto binaryMovement = Structures::Movement{
  0, 1, -1, 1, (int)(179.f / 360.f * 65535.f), 0, 0, 0x3c
};

static const auto jLook = nlohmann::json{
  { "t", MsgType::UpdateLook },
  { "idx", 0 },
  { "data",
    { { "isFemale", false },
      { "raceId", 0x00000001 },
      { "weight", 99.9f },
      { "skinColor", -1 },
      { "hairColor", -1 },
      { "headpartIds", nlohmann::json::array() },
      { "headTextureSetId", 0x00000000 },
      { "tints", nlohmann::json::array() },
      { "name", "Oberyn" },
      { "options",
        nlohmann::json::array({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0 }) },                  // size=19
      { "presets", nlohmann::json::array({ 0, 0, 0, 0 }) } } } // size=4
};

static const auto jEquipment = nlohmann::json{
  { "t", MsgType::UpdateEquipment },
  { "idx", 0 },
  { "data", { { "inv", { { "entries", nlohmann::json::array() } } } } }
};

void DoUpdateMovement(PartOne& partOne, uint32_t actorFormId,
                      Networking::UserId userId)
{
  auto jMyMovement = jMovement;
  jMyMovement["idx"] = dynamic_cast<MpActor*>(
                         partOne.worldState.LookupFormById(actorFormId).get())
                         ->GetIdx();
  DoMessage(partOne, userId, jMyMovement);
}

class FakeListener : public PartOne::Listener
{
public:
  static std::shared_ptr<FakeListener> New()
  {
    return std::shared_ptr<FakeListener>(new FakeListener);
  }

  void OnConnect(Networking::UserId userId) override
  {
    ss << "OnConnect(" << userId << ")" << std::endl;
  }

  void OnDisconnect(Networking::UserId userId) override
  {
    ss << "OnDisconnect(" << userId << ")" << std::endl;
  }

  void OnCustomPacket(Networking::UserId userId,
                      const simdjson::dom::element& content) override
  {
    ss << "OnCustomPacket(" << userId << ", " << simdjson::minify(content)
       << ")" << std::endl;
  }

  bool OnMpApiEvent(const char* eventName,
                    std::optional<simdjson::dom::element> args,
                    std::optional<uint32_t> formId) override
  {
    return true;
  }

  std::string str() { return ss.str(); }

  void clear() { ss = std::stringstream(); }

private:
  std::stringstream ss;
};

class PapyrusCompatibilityPolicy : public IPapyrusCompatibilityPolicy
{
public:
  PapyrusCompatibilityPolicy(MpActor* ac_)
    : ac(ac_)
  {
  }

  MpActor* GetDefaultActor(const char*, const char*, int32_t) const override
  {
    return ac;
  }

private:
  MpActor* const ac;
};

}