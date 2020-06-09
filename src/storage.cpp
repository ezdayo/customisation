/**
 *
 * @file      src/storage.cpp
 *
 * @brief     This is the implementation of Customisation parameter storage
 *            classes
 *
 *            This file is part of the Customisation framework (see link).
 *
 * @author    Olivier Stoltz-Douchet <ezdayo@gmail.com>
 *
 * @copyright (c) 2019-2020 Olivier Stoltz-Douchet
 * @license   http://opensource.org/licenses/MIT MIT
 * @link      https://github.com/ezdayo/customisation
 *
 **/

#include "storage.tpl.hpp"

/* All storages apply to everything, so let's be exhaustive and quick! */
#define IMPLEMENT_STORAGES_FOR(T) \
template class Customisation::Storage::Immediate<T>; \
template class Customisation::Storage::Immediate<std::set<T>>; \
template class Customisation::Storage::Immediate<std::vector<T>>; \
template class Customisation::Storage::Callable<T>; \
template class Customisation::Storage::Callable<std::set<T>>; \
template class Customisation::Storage::Callable<std::vector<T>>; \
template class Customisation::Storage::Deferred<T>; \
template class Customisation::Storage::Deferred<std::set<T>>; \
template class Customisation::Storage::Deferred<std::vector<T>>; \
template class Customisation::Storage::ReadOnly<T>; \
template class Customisation::Storage::ReadOnly<std::set<T>>; \
template class Customisation::Storage::ReadOnly<std::vector<T>>;

#define IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1,T2) \
template class Customisation::Storage::Immediate<std::pair<T1, T2>>; \
template class Customisation::Storage::Immediate<std::map<T1, T2>>; \
template class Customisation::Storage::Callable<std::pair<T1, T2>>; \
template class Customisation::Storage::Callable<std::map<T1, T2>>; \
template class Customisation::Storage::Deferred<std::pair<T1, T2>>; \
template class Customisation::Storage::Deferred<std::map<T1, T2>>; \
template class Customisation::Storage::ReadOnly<std::pair<T1, T2>>; \
template class Customisation::Storage::ReadOnly<std::map<T1, T2>>;

#define IMPLEMENT_STORAGES_PAIR_MAP_KEY(T1) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, bool) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, int8_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, int16_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, int32_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, int64_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, uint8_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, uint16_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, uint32_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, uint64_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, float) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, double) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY_VAL(T1, std::string)

#define IMPLEMENT_STORAGES \
IMPLEMENT_STORAGES_FOR(std::string) \
IMPLEMENT_STORAGES_FOR(bool) \
IMPLEMENT_STORAGES_FOR(int8_t) \
IMPLEMENT_STORAGES_FOR(int16_t) \
IMPLEMENT_STORAGES_FOR(int32_t) \
IMPLEMENT_STORAGES_FOR(int64_t) \
IMPLEMENT_STORAGES_FOR(uint8_t) \
IMPLEMENT_STORAGES_FOR(uint16_t) \
IMPLEMENT_STORAGES_FOR(uint32_t) \
IMPLEMENT_STORAGES_FOR(uint64_t) \
IMPLEMENT_STORAGES_FOR(float) \
IMPLEMENT_STORAGES_FOR(double) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY(int8_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY(int16_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY(int32_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY(int64_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY(uint8_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY(uint16_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY(uint32_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY(uint64_t) \
IMPLEMENT_STORAGES_PAIR_MAP_KEY(std::string) \
IMPLEMENT_STORAGES_FOR(Customisation::Entity*) \
IMPLEMENT_STORAGES_FOR(Customisation::Component*) \
IMPLEMENT_STORAGES_FOR(void*)

/*
 * Implement'em all!!!
 */
IMPLEMENT_STORAGES

