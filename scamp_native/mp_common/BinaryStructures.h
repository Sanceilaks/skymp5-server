#pragma once

#include <array>

namespace Structures {
  enum class RunMode
  {
    Standing,
    Walking,
    Running,
    Sprinting
  };

  enum class MovementFlags
  {
    RunMode0 = 0,
    RunMode1 = 1 << 0,
    IsInJumpState = 1 << 1,
    IsSneaking = 1 << 2,
    IsBlocking = 1 << 3,
    IsWeapDrawn = 1 << 4
  };

  #pragma pack(push, 1)
  struct Movement
  {
    int id;                      // 4 bytes
    float x, y, z;               // 4 bytes at var
    short int angleZ;            // 2 bytes
    int direction;               // 4 bytes
    int movementFlags;           // 4 bytes
    int worldOrCell;             // 4 bytes
  };                             // total 30 bytes
  #pragma pack(pop)
  constexpr auto MovementSize = 30;

}

namespace StructuresTools {
  template<size_t size> std::array<uint8_t, size> ReadStructureToArray(uint8_t* struc)
  {
    auto data = std::array<uint8_t, size>();

    for (auto i = 0; i < size; ++i) {
      data[i] = struc[i];
    }

    return data;
  }

}
