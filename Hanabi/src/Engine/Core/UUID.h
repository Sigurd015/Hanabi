#pragma once
#include <random>
#include <unordered_map>

namespace Hanabi
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	class UUID
	{
	public:
		UUID() : m_UUID(s_UniformDistribution(s_Engine)) {}
		UUID(uint64_t uuid) : m_UUID(uuid) {}
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}
namespace std
{
	template<>
	struct hash<Hanabi::UUID>
	{
		std::size_t operator()(const Hanabi::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};

}