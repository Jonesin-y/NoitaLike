#pragma once
#include <cstdint>
#include <memory>
#define Ref(x) std::shared_ptr<x>
#define Scope(x) std::unique_ptr<x>
inline uint32_t rand_state = 0x12345678;
inline uint32_t FastRandRange(uint32_t& state, uint32_t range)
{
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return (uint32_t)(((uint64_t)state * (uint64_t)range) >> 32);
}
