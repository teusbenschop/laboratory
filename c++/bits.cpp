/*
Copyright (©) 2021-2026 Teus Benschop.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "bits.h"

#include <bit>
#include <bitset>

namespace bits {
// https://en.cppreference.com/w/cpp/header/bit


// The system's endian-ness.
constexpr auto endian{std::endian::native};
static_assert(endian != std::endian::big);
static_assert(endian == std::endian::little);


// Demo of std::bit_cast.
// Reinterpret the object representation of one type as that of another.
constexpr uint8_t ui8{255};
constexpr int8_t i8 = std::bit_cast<int8_t>(ui8);
static_assert(ui8 == 255);
static_assert(i8 == -1);


static_assert(std::bitset<4>(2u) == 0b0010);

static_assert(std::has_single_bit(2u));

static_assert(std::bitset<4>(2u).to_string() == "0010");


// rotl - Rotate bits to the left.
// rotr - Rotate bits to the right.
constexpr uint8_t ui2 = 0b0010;
static_assert(std::rotl(ui2, 1) == 0b0100);
static_assert(std::rotr(ui2, 1) == 0b0001);


void demo()
{
    // bit_ceil
    // Finds the smallest integral power of two not less than the given value.
    // bit_floor
    // Finds the largest integral power of two not greater than the given value.
    // bit_width
    // Finds the smallest number of bits needed to represent the given value.
    // countl_zero
    // Counts the number of consecutive 0 bits, starting from the most significant bit.
    // countl_one
    // Counts the number of consecutive 1 bits, starting from the most significant bit.
    // countr_zero
    // Counts the number of consecutive 0 bits, starting from the least significant bit.
    // countr_one
    // Counts the number of consecutive 1 bits, starting from the least significant bit.
    // popcount - Counts the number of 1 bits in an unsigned integer.

    // The std::byteswap reverses the bytes in the given integer value n.
    {
        // const auto dump = [](auto value)
        // {
        //     std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(sizeof(value) * 2) << value <<
        //         " : ";
        //     for (std::size_t i{}; i != sizeof(value); i++, value >>= 8)
        //         std::cout << std::setw(2) << static_cast<unsigned>(decltype(value)(0xFF) & value) << ' ';
        //     std::cout << std::dec << std::endl;
        // };
        //
        // static_assert(std::byteswap('a') == 'a');
        //
        // std::cout << "byteswap for U16:" << std::endl;
        // constexpr auto x = std::uint16_t(0xCAFE);
        // dump(x);
        // dump(std::byteswap(x));
        //
        // std::cout << "\nbyteswap for U32:" << std::endl;
        // constexpr auto y = std::uint32_t(0xDEADBEEFu);
        // dump(y);
        // dump(std::byteswap(y));
        //
        // std::cout << "\nbyteswap for U64:" << std::endl;
        // constexpr auto z = std::uint64_t{0x0123456789ABCDEFull};
        // dump(z);
        // dump(std::byteswap(z));
    }
}

}
