/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <memory>
#include <string>
#include <vector>

enum SOCKET_STATUS
{
    SOCKET_STATUS_CLOSED,
    SOCKET_STATUS_RESOLVING,
    SOCKET_STATUS_CONNECTING,
    SOCKET_STATUS_CONNECTED,
    SOCKET_STATUS_LISTENING,
};

enum NETWORK_READPACKET
{
    NETWORK_READPACKET_SUCCESS,
    NETWORK_READPACKET_NO_DATA,
    NETWORK_READPACKET_MORE_DATA,
    NETWORK_READPACKET_DISCONNECTED
};

/**
 * Represents an address and port.
 */
struct INetworkEndpoint
{
    virtual ~INetworkEndpoint()
    {
    }

    virtual std::string GetHostname() const abstract;
};

/**
 * Represents a TCP socket / connection or listener.
 */
struct ITcpSocket
{
public:
    virtual ~ITcpSocket() = default;

    virtual SOCKET_STATUS GetStatus() const abstract;
    virtual const char* GetError() const abstract;
    virtual const char* GetHostName() const abstract;
    virtual std::string GetIpAddress() const abstract;

    virtual void Listen(uint16_t port) abstract;
    virtual void Listen(const std::string& address, uint16_t port) abstract;
    virtual std::unique_ptr<ITcpSocket> Accept() abstract;

    virtual void Connect(const std::string& address, uint16_t port) abstract;
    virtual void ConnectAsync(const std::string& address, uint16_t port) abstract;

    virtual size_t SendData(const void* buffer, size_t size) abstract;
    virtual NETWORK_READPACKET ReceiveData(void* buffer, size_t size, size_t* sizeReceived) abstract;

    virtual void Disconnect() abstract;
    virtual void Close() abstract;
};

/**
 * Represents a UDP socket / listener.
 */
struct IUdpSocket
{
public:
    virtual ~IUdpSocket() = default;

    virtual SOCKET_STATUS GetStatus() const abstract;
    virtual const char* GetError() const abstract;
    virtual const char* GetHostName() const abstract;

    virtual void Listen(uint16_t port) abstract;
    virtual void Listen(const std::string& address, uint16_t port) abstract;

    virtual size_t SendData(const std::string& address, uint16_t port, const void* buffer, size_t size) abstract;
    virtual size_t SendData(const INetworkEndpoint& destination, const void* buffer, size_t size) abstract;
    virtual NETWORK_READPACKET ReceiveData(
        void* buffer, size_t size, size_t* sizeReceived, std::unique_ptr<INetworkEndpoint>* sender) abstract;

    virtual void Close() abstract;
};

bool InitialiseWSA();
void DisposeWSA();
std::unique_ptr<ITcpSocket> CreateTcpSocket();
std::unique_ptr<IUdpSocket> CreateUdpSocket();
std::vector<std::unique_ptr<INetworkEndpoint>> GetBroadcastAddresses();

namespace Convert
{
    uint16_t HostToNetwork(uint16_t value);
    uint16_t NetworkToHost(uint16_t value);
} // namespace Convert
