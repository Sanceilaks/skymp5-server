#pragma once
#include "Networking.h"
#include <cstdint>

namespace MpClientPlugin {
typedef void (*OnPacket)(int32_t type, const char* jsonContent,
                         const char* error, void* state_);

/// Current client state
struct State
{
  /// Client ptr
  std::shared_ptr<Networking::IClient> cl;
};

/// Create new client state
/// \param st Target client state
/// \param targetHostname Server ip or host name
/// \param targetPort Server port
void CreateClient(State& st, const char* targetHostname, uint16_t targetPort);

/// Delete client state
/// \param st State to remove
void DestroyClient(State& st);

/// Is state has connection with server
/// \param st Target client state
/// \return Connection state
bool IsConnected(State& st);

/// Do client tick
/// \param st Target client state
/// \param onPacket OnPacket callback
/// \param state_
void Tick(State& st, OnPacket onPacket, void* state_);

/// Sends data on server
/// \param state Current client state
/// \param jsonContent Content to send
/// \param reliable Is lost packets will resend
void Send(State& state, const char* jsonContent, bool reliable);

//void Send(State& state, std::vector<uint8_t>& data, bool reliable);

/// Sends data on server
/// \param state Current client state
/// \param data Content to send
/// \param dataSize Size of content to send
/// \param reliable Is lost packets will resend
void Send(State& state, uint8_t* data, size_t dataSize, bool reliable);
};