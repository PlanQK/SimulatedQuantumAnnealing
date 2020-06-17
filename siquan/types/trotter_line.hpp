/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2016-2018, ETH Zurich, ITP, Mario S. Koenz                        *
 *                                                                             *
 * Licensed under the Apache License, Version 2.0 (the "License");             *
 * you may not use this file except in compliance with the License.            *
 * You may obtain a copy of the License at                                     *
 *                                                                             *
 *   http://www.apache.org/licenses/LICENSE-2.0                                *
 *                                                                             *
 * Unless required by applicable law or agreed to in writing, software         *
 * distributed under the License is distributed on an "AS IS" BASIS,           *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    *
 * See the License for the specific language governing permissions and         *
 * limitations under the License.                                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ** */

/** \file
 * \brief The trotter line types are defined here
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */
#ifndef SIQUAN_TYPES_TROTTER_LINE_HPP_GUARD
#define SIQUAN_TYPES_TROTTER_LINE_HPP_GUARD

#include <array>
#include <bitset>
#include <climits>
#include <vector>

namespace siquan {  // documented in base.hpp
namespace types {   // documented in interaction.hpp

    /// \brief A static trotter-line if N is known at compiletime
    /// \tparam NT the amount of trotter slices
    template <int NT>
    class StaticTrotterLine : public std::bitset<NT> {
    public:
        // structors
        /// \brief Constructor
        StaticTrotterLine() = default;
        /// \brief Constructor to be interface compatible with the runtime
        /// version \param nt has to be the same as \p NT, otherwise throws.
        StaticTrotterLine(uint32_t const &nt) : StaticTrotterLine() {
            if(NT != nt)  // sanity check
                throw std::runtime_error(
                    "StaticTrotterLine: NT not consistent!");
        }

        // modifying methods

        // const methods
        /// \cond IMPLEMENTATION_DETAIL_DOC
        class Biterator {
            using skip_type = uint32_t;
            static constexpr uint8_t skip_size = sizeof(skip_type) * CHAR_BIT;
            using array_ptr = std::array<skip_type, (NT - 1) / skip_size + 1> *;

        public:
            // structors
            Biterator(StaticTrotterLine const &data) noexcept
                : data_(data),
                  arr_(static_cast<array_ptr>(static_cast<void *>(&data_))),
                  ts_(0),
                  block_(-1),
                  temp_(0) {
                next_block();
                ++(*this);
            }
            Biterator(StaticTrotterLine const &data,
                      uint32_t const &ts) noexcept
                : data_(data), arr_(nullptr), ts_(ts), block_(), temp_() {}
            // modifying methods
            inline void next_block() noexcept {
                ++block_;
                temp_ = (*arr_)[block_];
                ts_ = block_ * skip_size;
                // maybe add later __builtin_ctz(32bit) __builtin_ctll(64bit)
            }
            inline Biterator &operator++() noexcept {
                while(block_ < arr_->size()) {
                    while(temp_.any()) {  // check if there are any bits left
                        if(temp_.test(0)) {
                            temp_.reset(0);  // delete last bit
                            return (*this);
                        }
                        ++ts_;
                        temp_ >>= 1;
                    }
                    next_block();
                }
                ts_ = NT;
                return (*this);
            }
            // const methods
            inline bool operator!=(Biterator const &rhs) const noexcept {
                return ts_ != rhs.ts_;
            }
            inline uint32_t const &operator*() const noexcept { return ts_; }

        private:
            StaticTrotterLine<NT> const &data_;
            array_ptr arr_;
            uint32_t ts_;
            uint32_t block_;
            std::bitset<skip_size> temp_;
        };
        /// \endcond

        /// \brief this Bit-iterator iterates from break-point (e.g. 01 or 10)
        /// to break-point
        inline Biterator begin() const noexcept { return Biterator(*this); }
        /// \brief see begin()
        inline Biterator end() const noexcept { return Biterator(*this, NT); }

    private:
    };

    /// \brief A dynamic trotter-line if N is only known at runtime
    class DynamicTrotterLine {
        static int constexpr base_size = 32;

    public:
        // structors
        /// \brief Constructor
        /// \param nt the amount of trotter slices
        DynamicTrotterLine(uint32_t const &nt)
            : NT(nt), data_((NT - 1.0) / base_size + 1) {}

        /// \brief bitset compatible interface
        DynamicTrotterLine &operator=(DynamicTrotterLine const &rhs) {
            data_ = rhs.data_;
            return (*this);
        }
        // modifying methods
        /// \brief bitset compatible interface
        auto operator[](size_t const &index) noexcept {
            return data_[index / base_size][index % base_size];
        }
        /// \brief bitset compatible interface
        /// \pre rhs.size() == size()
        DynamicTrotterLine &operator^=(DynamicTrotterLine const &rhs) {
            for(size_t i = 0; i < data_.size(); ++i) data_[i] ^= rhs.data_[i];
            return (*this);
        }
        /// \brief bitset compatible interface
        DynamicTrotterLine operator^(DynamicTrotterLine const &rhs) {
            DynamicTrotterLine res(NT);

            for(size_t i = 0; i < data_.size(); ++i)
                res.data_[i] = data_[i] ^ rhs.data_[i];
            return res;
        }
        /// \brief bitset compatible interface
        /// \pre rhs.size() == size()
        DynamicTrotterLine &operator&=(DynamicTrotterLine const &rhs) {
            for(size_t i = 0; i < data_.size(); ++i) data_[i] &= rhs.data_[i];
            return (*this);
        }
        /// \brief bitset compatible interface
        /// \pre rhs.size() == size()
        DynamicTrotterLine &operator|=(DynamicTrotterLine const &rhs) {
            for(size_t i = 0; i < data_.size(); ++i) data_[i] |= rhs.data_[i];
            return (*this);
        }
        /// \brief bitset compatible interface
        DynamicTrotterLine &operator<<=(uint32_t /*copy*/ shift) {
            if(shift >= base_size) {
                int32_t big_shift = shift / base_size;
                for(int32_t i = data_.size() - 1; i >= big_shift; --i)
                    data_[i] = data_[i - big_shift];

                for(int32_t i = big_shift - 1; i >= 0; --i) data_[i] = 0;

                shift %= base_size;
            }
            if(shift) {
                data_.back() <<= shift;
                for(int i = data_.size() - 2; i >= 0; --i) {
                    data_[i + 1] |= (data_[i] >> (base_size - shift));
                    data_[i] <<= shift;
                }
            }
            return (*this);
        }
        // const methods
        /// \brief bitset compatible interface
        auto operator[](size_t const &index) const noexcept {
            return data_[index / base_size][index % base_size];
        }
        /// \brief bitset compatible interface
        void set() {
            for(auto it = data_.begin(); it != data_.end(); ++it) it->set();
        }
        /// \brief bitset compatible interface
        void reset() {
            for(auto it = data_.begin(); it != data_.end(); ++it) it->reset();
        }
        /// \brief bitset compatible interface
        void flip(size_t const &index) {
            data_[index / base_size].flip(index % base_size);
        }
        /// \brief bitset compatible interface
        void flip() {
            for(auto it = data_.begin(); it != data_.end(); ++it) it->flip();
        }
        /// \endcond

        /// \brief used to print the content to a stream
        /// \tparam S type of stream
        /// \param os instance of stream \p S
        template <typename S>
        void print(S &os) const {
            // print the same as a bitset would (little endian)
            auto hide = NT % base_size;

            for(auto it = data_.rbegin(); it != data_.rend(); ++it) {
                if(hide) {
                    for(uint32_t i = 0; i < hide; ++i) {
                        os << (*it)[hide - 1 - i];
                    }
                    hide = 0;
                } else {
                    os << *it;
                }
            }
        }

        // const methods
        /// \cond IMPLEMENTATION_DETAIL_DOC
        class Biterator {
        public:
            // structors
            Biterator(std::vector<std::bitset<base_size>> const &data,
                      uint32_t const &_NT) noexcept
                : arr_(data), NT_(_NT), ts_(0), block_(-1), temp_(0) {
                next_block();
                ++(*this);
            }
            Biterator(std::vector<std::bitset<base_size>> const &data,
                      uint32_t const &_NT, uint32_t const &ts) noexcept
                : arr_(data), NT_(_NT), ts_(ts) {}
            // modifying methods
            inline void next_block() noexcept {
                ++block_;
                temp_ = arr_[block_];
                ts_ = block_ * base_size;
                // maybe add later __builtin_ctz(32bit) __builtin_ctll(64bit)
            }
            inline Biterator &operator++() noexcept {
                while(block_ < arr_.size()) {
                    while(temp_.any()) {  // check if there are any bits left
                        if(temp_.test(0)) {
                            temp_.reset(0);  // delete last bit
                            return (*this);
                        }
                        ++ts_;
                        temp_ >>= 1;
                    }
                    next_block();
                }
                ts_ = NT_;
                return (*this);
            }
            // const methods
            inline bool operator!=(Biterator const &rhs) const noexcept {
                return ts_ != rhs.ts_;
            }
            inline uint32_t const &operator*() const noexcept { return ts_; }

        private:
            std::vector<std::bitset<base_size>> const &arr_;
            size_t const NT_;
            uint32_t ts_;
            uint32_t block_;
            std::bitset<base_size> temp_;
        };
        /// \endcond

        /// \brief this Bit-iterator iterates from break-point (e.g. 01 or 10)
        /// to break-point
        inline Biterator begin() const noexcept { return Biterator(data_, NT); }
        /// \brief see begin()
        inline Biterator end() const noexcept {
            return Biterator(data_, NT, NT);
        }
        /// \brief returns the amount of trotter slices§
        size_t const &size() const noexcept { return NT; }

    private:
        size_t const NT;
        std::vector<std::bitset<base_size>> data_;
    };

    /// \brief makes the dynamic totter-line printable
    /// \param os
    /// \param arg is the trotter-line we want to print
    inline std::ostream &operator<<(std::ostream &os,
                                    DynamicTrotterLine const &arg) {
        arg.print(os);
        return os;
    }

}  // end namespace types
}  // end namespace siquan

#endif  // SIQUAN_TYPES_TROTTER_LINE_HPP_GUARD
