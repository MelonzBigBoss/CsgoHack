#pragma once
#include <windows.h>
#include <SDKDDKVer.h>
#include <string.h>
#include <vadefs.h>
#include <stdio.h>
#include <xstring>
#include <Psapi.h>
#include <thread>
#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <string>
#include <vector>
#include <time.h>
#include <winuser.h>
#include <random>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <set>
#include <map>
#include <unordered_map>
#include <memory>
#include <thread>
#include <array>
#include <assert.h>
#include <deque>
#include <intrin.h>
#include <atomic>
#include <mutex>
#include <stdint.h>
#include <stdlib.h>
#include <process.h>
#include <Wincon.h>
#include <cstdint>
#include <chrono>
#include <Shlobj.h>
#include <future>
#include <Lmcons.h>
#include <tchar.h>
#include "../../../source-sdk/classes/recv_props.hpp"
#include "../../../source-sdk/classes/client_class.hpp"
#include "../../utilities/fnv.hpp"
#include "../../../source-sdk/structs/datamap.hpp"
namespace netvar_manager {
    uintptr_t get_net_var( uint32_t table, uint32_t prop );
    unsigned int find_in_datamap(datamap_t* pMap, const char* uFieldHash);
}

#define NETVAR( table, prop, func_name, type ) type& func_name() { \
    static uintptr_t offset = 0; \
    if( !offset ) { \
        offset = netvar_manager::get_net_var( fnv::hash( table ), fnv::hash( prop ) ); \
    } \
    return *reinterpret_cast< type* >( uintptr_t( this ) + offset ); \
}
#define NETVAR_OFFSET( table, prop, func_name, type, additional_offset ) type& func_name() { \
    static uintptr_t offset = 0; \
    if( !offset ) { \
        offset = netvar_manager::get_net_var( fnv::hash( table ), fnv::hash( prop ) ); \
    } \
    return *reinterpret_cast< type* >( uintptr_t( this ) + offset + additional_offset ); \
}
#define NETVAR_PTR( table, prop, func_name, type ) type* func_name() { \
    static uintptr_t offset = 0; \
    if( !offset ) { \
        offset = netvar_manager::get_net_var( fnv::hash( table ), fnv::hash( prop ) ); \
    } \
    return reinterpret_cast< type* >( uintptr_t( this ) + offset ); \
}

#define OFFSET( type, var, offset ) type& var() { \
    return *( type* )( uintptr_t( this ) + offset ); \
}
