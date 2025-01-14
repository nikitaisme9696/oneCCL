/*
 Copyright 2016-2020 Intel Corporation
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
     http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/
#pragma once
#include "common/comm/l0/modules/kernel_argument_policies.hpp"

namespace native {

namespace options {

// Options for kernel arguments, here we should defile all the aspects that don't change parameter's
// behaviour, i.e. don't affect load/store functions such. Things that does that(i.e. thread-safety)
// should be defined as separate policy.
template <bool uncached = false>
struct generic {
    static constexpr bool is_uncached() {
        return uncached;
    }
};

using empty = generic<>;
using uncached = generic<true>;

} // namespace options

// base class for kernel argument
template <size_t pos, class policy_impl, class options = options::empty>
struct kernel_arg : public policy_impl, options {
    enum { index = pos };
    using policy = policy_impl;
    using arg_type = typename policy::arg_type;
    using return_t = typename policy::return_t;
    using options_t = options;
};

// thread-safe argument: used for concurrent read/write applications
template <size_t pos, class type, class options = options::empty>
using thread_safe_arg = kernel_arg<pos, arg_access_policy_atomic<pos, type, false>, options>;

// thread-safe destructive-copying argument (rechargeable): used for concurrent
// read/write applications, where reader take-away existing value
template <size_t pos, class type, class options = options::empty>
using thread_exchangable_arg =
    kernel_arg<pos, arg_access_policy_atomic_reset<pos, type, false>, options>;

// external argument using uncached flag and reset policy
template <size_t pos, class type, class options = options::uncached>
using external_arg = kernel_arg<pos, arg_access_policy_atomic_reset<pos, type, false>, options>;

// uncached permanent argument using uncached flag and no reset policy
template <size_t pos, class type, class options = options::uncached>
using permanent_arg = kernel_arg<pos, arg_access_policy_atomic<pos, type, false>, options>;

// default, single threaded access argument
template <size_t pos, class type, class options = options::empty>
using arg = kernel_arg<pos, arg_access_policy_default<pos, type>, options>;

// empty argument
template <size_t pos, class options = options::empty>
using stub_arg = kernel_arg<pos, arg_no_access_policy<pos>, options>;

// utilities
namespace detail {
struct args_printer {
    args_printer(std::stringstream& ss) : out(ss) {}

    template <typename Arg>
    void operator()(const Arg& arg) {
        out << "idx: " << Arg::index << "\t";
        dump_arg_value(arg, out);
        using opt = typename Arg::options_t;
        print_options(opt{}, out);
        out << std::endl;
    }

    // atomic argument pretty printing
    template <size_t pos, class type, class options>
    void operator()(const thread_safe_arg<pos, type, options>& arg) {
        out << "idx: " << pos << "\t";
        dump_arg_value(arg, out);
        print_options(options{}, out);
        out << "\tATOMIC" << std::endl;
    }

    template <size_t pos, class type, class options>
    void operator()(const thread_exchangable_arg<pos, type, options>& arg) {
        out << "idx: " << pos << "\t";
        arg.dump(out);
        print_options(options{}, out);
        out << "\tATOMIC_EXG" << std::endl;
    }

    // stub argument pretty printing
    template <size_t pos, class options>
    void operator()(const stub_arg<pos, options>& arg) {
        out << "idx: " << pos;
        print_options(options{}, out);
        out << "\tSTUB" << std::endl;
    }
    std::stringstream& out;

private:
    template <typename Arg>
    void dump_arg_value(const Arg& arg, std::stringstream& ss) {
        if (arg.test()) {
            auto ret = arg.load();
            ss << "{ " << ret.second << " }";
        }
        else {
            ss << "{ EMPTY }";
        }
    }

    template <class Options>
    void print_options(Options opt, std::stringstream& ss) {
        if (opt.is_uncached())
            ss << "\tUNCACHED ";
    }
};
} // namespace detail
} // namespace native
