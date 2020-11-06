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

#ifndef CCL_PRODUCT_FULL
#error "Do not include this file directly. Please include 'ccl.hpp'"
#endif

namespace ccl {
namespace details {

/**
 * Traits for context attributes specializations
 */
template <>
struct ccl_api_type_attr_traits<context_attr_id, context_attr_id::version> {
    using type = library_version;
    using return_type = type;
};

template <>
struct ccl_api_type_attr_traits<context_attr_id, context_attr_id::cl_backend> {
    using type = cl_backend_type;
    using return_type = type;
};

template <>
struct ccl_api_type_attr_traits<context_attr_id, context_attr_id::native_handle> {
    using type = typename unified_device_context_type::ccl_native_t;
    using return_type = type;
};
}
}
