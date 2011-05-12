// Copyright (c) 2011, Robert Escriva
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of this project nor the names of its contributors may
//       be used to endorse or promote products derived from this software
//       without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// C
#include <stdint.h>

// Google Test
#include <gtest/gtest.h>

// po6
#include <e/buffer.h>

#pragma GCC diagnostic ignored "-Wswitch-default"

namespace
{

TEST(BufferTest, CtorAndDtor)
{
    e::buffer a; // Create a buffer without any size.
    e::buffer b(3); // Create a buffer which can pack 3 bytes without resizing.
    e::buffer c("xyz", 3); // Create a buffer with the three bytes "XYZ".
    e::buffer d(reinterpret_cast<const void*>("xyz"), 3);
}

TEST(BufferTest, PackBuffer)
{
    uint64_t a = 0xdeadbeefcafebabe;
    uint32_t b = 0x8badf00d;
    uint16_t c = 0xface;
    uint8_t d = '!';
    e::buffer buf("the buffer", 10);

    e::buffer pack;
    e::buffer::packer packer(&pack);

    packer << a << b << c << d << e::buffer::padding(5) << buf;
    EXPECT_EQ(0,
        memcmp(pack.get(), "\xde\xad\xbe\xef\xca\xfe\xba\xbe"
                           "\x8b\xad\xf0\x0d"
                           "\xfa\xce"
                           "!"
                           "\x00\x00\x00\x00\x00"
                           "the buffer", 30));
}

TEST(BufferTest, UnpackBuffer)
{
    uint64_t a;
    uint32_t b;
    uint16_t c;
    uint8_t d;
    e::buffer buf;
    e::buffer pack("\xde\xad\xbe\xef\xca\xfe\xba\xbe"
                   "\x8b\xad\xf0\x0d"
                   "\xfa\xce"
                   "!"
                   "\x00\x00\x00\x00\x00"
                   "the buffer", 30);
    e::buffer::unpacker unpacker(pack);
    unpacker >> a >> b >> c >> d >> e::buffer::padding(5) >> buf;
    EXPECT_EQ(0xdeadbeefcafebabe, a);
    EXPECT_EQ(0x8badf00d, b);
    EXPECT_EQ(0xface, c);
    EXPECT_EQ('!', d);
    EXPECT_EQ(10, buf.size());
    EXPECT_TRUE(e::buffer("the buffer", 10) == buf);
}

TEST(BufferTest, UnpackErrors)
{
    bool caught = false;

    e::buffer buf("\x8b\xad\xf0\x0d" "\xfa\xce", 6);
    uint32_t a;
    e::buffer::unpacker u(buf);
    u >> a;
    EXPECT_EQ(0x8badf00d, a);

    try
    {
        u >> a;
    }
    catch (e::buffer::exception& e)
    {
        caught = true;
    }

    EXPECT_TRUE(caught);
    // "a" should not change
    EXPECT_EQ(0x8badf00d, a);
    // Getting the next value should succeed
    uint16_t b;
    u >> b;
    EXPECT_EQ(0xface, b);
}

} // namespace