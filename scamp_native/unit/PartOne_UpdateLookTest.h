#include "TestUtils.hpp"

TEST_CASE("SetRaceMenuOpen failures", "[PartOne]")
{
  
  PartOne partOne;

  partOne.CreateActor(0xff000000, { 1.f, 2.f, 3.f }, 180.f, 0x3c);

  REQUIRE_THROWS_WITH(
    partOne.SetRaceMenuOpen(0xff000000, true),
    Contains("Actor with id ff000000 is not attached to any of users"));

  REQUIRE_THROWS_WITH(partOne.SetRaceMenuOpen(0xffffffff, true),
                      Contains("Form with id ffffffff doesn't exist"));

  partOne.worldState.AddForm(std::make_unique<MpForm>(), 0xffffffff);

  REQUIRE_THROWS_WITH(partOne.SetRaceMenuOpen(0xffffffff, true),
                      Contains("Form with id ffffffff is not Actor"));
}

TEST_CASE("SetRaceMenuOpen", "[PartOne]")
{
  
  PartOne partOne;

  DoConnect(partOne, 1);
  partOne.CreateActor(0xff000000, { 1.f, 2.f, 3.f }, 180.f, 0x3c);
  auto actor = std::dynamic_pointer_cast<MpActor>(
    partOne.worldState.LookupFormById(0xff000000));
  partOne.SetUserActor(1, 0xff000000);
  partOne.Messages().clear();

  REQUIRE(actor->IsRaceMenuOpen() == false);

  partOne.SetRaceMenuOpen(0xff000000, true);

  REQUIRE(actor->IsRaceMenuOpen() == true);
  REQUIRE(partOne.Messages().size() == 1);
  REQUIRE(partOne.Messages()[0].j ==
          nlohmann::json{ { "type", "setRaceMenuOpen" }, { "open", true } });
  REQUIRE(partOne.Messages()[0].userId == 1);
  REQUIRE(partOne.Messages()[0].reliable);

  for (int i = 0; i < 3; ++i)
    partOne.SetRaceMenuOpen(0xff000000, true);
  REQUIRE(partOne.Messages().size() == 1);

  partOne.SetRaceMenuOpen(0xff000000, false);
  REQUIRE(partOne.Messages().size() == 2);
  REQUIRE(partOne.Messages()[1].j ==
          nlohmann::json{ { "type", "setRaceMenuOpen" }, { "open", false } });
  REQUIRE(partOne.Messages()[1].userId == 1);
  REQUIRE(partOne.Messages()[1].reliable);

  for (int i = 0; i < 3; ++i)
    partOne.SetRaceMenuOpen(0xff000000, false);
  REQUIRE(partOne.Messages().size() == 2);
}

TEST_CASE("Look <=> JSON casts", "[PartOne]")
{
  simdjson::dom::parser p;
  auto jLookSimd = p.parse(jLook["data"].dump());
  auto look = Look::FromJson(jLookSimd.value());

  REQUIRE(nlohmann::json::parse(look.ToJson()) == jLook["data"]);
}

TEST_CASE("UpdateLook1", "[PartOne]")
{
  
  PartOne partOne;

  DoConnect(partOne, 0);
  partOne.CreateActor(0xff000ABC, { 1.f, 2.f, 3.f }, 180.f, 0x3c);
  partOne.SetUserActor(0, 0xff000ABC);
  partOne.SetRaceMenuOpen(0xff000ABC, true);

  DoConnect(partOne, 1);
  partOne.CreateActor(0xffABCABC, { 11.f, 22.f, 33.f }, 180.f, 0x3c);
  partOne.SetUserActor(1, 0xffABCABC);

  partOne.Messages().clear();
  auto doLook = [&] { DoMessage(partOne, 0, jLook); };
  doLook();

  REQUIRE(partOne.Messages().size() == 2);
  REQUIRE(std::find_if(partOne.Messages().begin(), partOne.Messages().end(),
                       [&](auto m) {
                         return m.j["t"] == MsgType::UpdateLook &&
                           m.j["idx"] == 0 && m.reliable && m.userId == 1 &&
                           m.j["data"] == jLook["data"];
                       }) != partOne.Messages().end());

  auto& ac = partOne.worldState.GetFormAt<MpActor>(0xff000ABC);
  REQUIRE(ac.GetLook() != nullptr);
  REQUIRE(nlohmann::json::parse(ac.GetLookAsJson()) == jLook["data"]);
  REQUIRE(ac.IsRaceMenuOpen() == false);
}

TEST_CASE("UpdateLook2", "[PartOne]")
{
  
  PartOne partOne;

  DoConnect(partOne, 0);
  partOne.CreateActor(0xff000ABC, { 1.f, 2.f, 3.f }, 180.f, 0x3c);
  partOne.SetUserActor(0, 0xff000ABC);
  partOne.SetRaceMenuOpen(0xff000ABC, true);

  DoConnect(partOne, 1);
  partOne.CreateActor(0xffABCABC, { 11.f, 22.f, 33.f }, 180.f, 0x3c);
  partOne.SetUserActor(1, 0xffABCABC);

  partOne.Messages().clear();
  auto doLook = [&] { DoMessage(partOne, 0, jLook); };
  doLook();

  REQUIRE(partOne.Messages().size() == 2);
  REQUIRE(std::find_if(partOne.Messages().begin(), partOne.Messages().end(),
                       [&](auto m) {
                         return m.j["t"] == MsgType::UpdateLook &&
                           m.j["idx"] == 0 && m.reliable && m.userId == 1 &&
                           m.j["data"] == jLook["data"];
                       }) != partOne.Messages().end());

  REQUIRE(partOne.worldState.GetFormAt<MpActor>(0xff000ABC).GetLook() !=
          nullptr);
  REQUIRE(
    nlohmann::json::parse(
      partOne.worldState.GetFormAt<MpActor>(0xff000ABC).GetLookAsJson()) ==
    jLook["data"]);
}