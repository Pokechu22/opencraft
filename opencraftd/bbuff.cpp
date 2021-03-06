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
// along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//      Port of bound buffer from spockbot
//
//-----------------------------------------------------------------------------

#include <common.h>
#include <bbuff.h>
#include <string.h>
#include <utils.h>
#include <arpa/inet.h>

#include <vector>

bound_buffer::bound_buffer() {
    _data.resize(0);
    _cursor   = 0;
    _cur_size = 0;
}

bound_buffer::bound_buffer(unsigned char* data, uint32_t len) {
    _data.resize(0);
    _cursor = 0;
    write(data,len);
}

void bound_buffer::clear_backlog() {
     if(_cursor == _data.size()) {
       _data.clear();
       _data.resize(0);
       _cursor = 0;
     }
}

bound_buffer *bound_buffer::read_buf(uint32_t len) {
    bound_buffer *retval = new bound_buffer();
    retval->write(&(_data[_cursor]),len);
    _cursor += len;
    return retval;
}

uint16_t bound_buffer::read_ushort() {
    unsigned char* s = read(2);
    uint16_t *retval = (uint16_t*)s;
    uint16_t ho_retval = ntohs(*retval);
    return ho_retval;
}

std::string bound_buffer::read_string() {
    int32_t s_len    = read_varint(32);
    unsigned char* s = read(s_len);
    std::string retval = std::string((const char*)s);
    retval[s_len] = 0;
    return retval;
}

unsigned char* bound_buffer::peek(uint32_t len) {
    return &(_data[_cursor]);
}

unsigned char* bound_buffer::read(uint32_t len) {
    unsigned char* retval = peek(len);
    _cursor += len;
    return retval;
}

uint32_t bound_buffer::size() {
    return _data.size() - _cursor;
}

int32_t bound_buffer::read_varint(int max_bits) {
    unsigned char *intbuf = peek(max_bits / 8);
    int32_t        retval = parse_var_int(intbuf, max_bits/8);
    _cursor += varint_size(retval);
    return retval;
}

void bound_buffer::write_varint(int32_t i) {
     int len = varint_size(i);
     unsigned char buf[4];
     int offs = 0;
     while((offs & -128) != 0) {
        buf[offs++] = (i & 127) | 128;
        i >>= 7;
     }
     buf[offs++] = i;
     write(buf, len);
}

void bound_buffer::write_string(std::string s) {
     write_varint((int32_t)s.size());
     write((unsigned char*)s.c_str(),(int32_t)s.size());
}

void bound_buffer::write(unsigned char* data, uint32_t len) {
     int32_t new_len = _cur_size + len;

     for(int i=0; i<len; i++) {
       _data.push_back(data[i]);
     }
     _data.resize(new_len);
     _cur_size = new_len;
}


