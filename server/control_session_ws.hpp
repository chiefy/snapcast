/***
    This file is part of snapcast
    Copyright (C) 2014-2024  Johannes Pohl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***/

#pragma once


// local headers
#include "control_session.hpp"

// 3rd party headers
#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <boost/asio/strand.hpp>
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif
#include <boost/beast/core.hpp>
#pragma GCC diagnostic pop
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>

// standard headers
#include <deque>


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
using tcp_socket = boost::asio::ip::tcp::socket;
using ssl_socket = boost::asio::ssl::stream<tcp_socket>;


/// Endpoint for a connected control client.
/**
 * Endpoint for a connected control client.
 * Messages are sent to the client with the "send" method.
 * Received messages from the client are passed to the ControlMessageReceiver callback
 */
class ControlSessionWebsocket : public ControlSession
{
public:
    /// ctor. Received message from the client are passed to ControlMessageReceiver
    ControlSessionWebsocket(ControlMessageReceiver* receiver, websocket::stream<ssl_socket>&& wss);
    ~ControlSessionWebsocket() override;
    void start() override;
    void stop() override;

    /// Sends a message to the client (asynchronous)
    void sendAsync(const std::string& message) override;

protected:
    // Websocket methods
    void on_read_ws(beast::error_code ec, std::size_t bytes_transferred);
    void do_read_ws();
    void send_next();

    websocket::stream<ssl_socket> wss_;

protected:
    beast::flat_buffer buffer_;
    boost::asio::strand<boost::asio::any_io_executor> strand_;
    std::deque<std::string> messages_;
};
