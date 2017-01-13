//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of the OpenCraft server.
//
// The OpenCraft server is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// The OpenCraft server is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//      OpenCraft client connection class - sends and receives packets
//
//-----------------------------------------------------------------------------

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

#include <vector>

using boost::asio::ip::tcp;

class opencraft_client_connection {
   public:
     typedef std::vector<unsigned char> net_buff_t;

     net_buff_t pending_recv;
     net_buff_t pending_send;

     void start();
     tcp::socket& socket() { return _socket; }
     void do_read();
     void do_packet_read();
     opencraft_client_connection(boost::asio::io_service& io_service) : _socket(io_service) {}
   private:
     boost::mutex mtx_;
     tcp::socket _socket;
     char _data[1024];
     proto_mode_t cur_proto_mode;
};