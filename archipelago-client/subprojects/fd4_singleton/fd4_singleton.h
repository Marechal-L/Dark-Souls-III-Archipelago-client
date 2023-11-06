#pragma once

// By tremwil, extracted from TGA's CheatEngine table.
// See https://gist.github.com/tremwil/4f028937cd33fb6aa2306b3a0619e5ca
// Originally https://github.com/The-Grand-Archives/Dark-Souls-III-CT-TGA/blob/main/resources/include/tga/fd4_singleton.h

#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <format>
#include <string>

/// Wrapper around a character array that allows passing a string literal as a template parameter
template<size_t N>
struct FixedString
{
	char buf[N + 1]{};
	constexpr FixedString(char const* s)
	{
		for (unsigned i = 0; i != N; ++i) buf[i] = s[i];
	}

	constexpr FixedString(const std::string& s)
	{
		for (unsigned i = 0; i != N && i < s.length(); ++i) buf[i] = s[i];
	}

	constexpr FixedString(std::string_view s)
	{
		for (unsigned i = 0; i != N && i < s.length(); ++i) buf[i] = s[i];
	}

	constexpr operator char const* () const { return buf; }
	constexpr operator std::string_view() const { return { buf, N }; }

	constexpr bool operator==(const FixedString& lhs)
	{
		for (unsigned i = 0; i != N; ++i)
			if (buf[i] != lhs.buf[i]) return false;
		return true;
	}
};
template<size_t N> FixedString(char const (&)[N]) -> FixedString<N - 1>;

class FD4SingletonFinder
{
public:
	// Gets the static address which stores the address of the singleton with the given name, 
	// or nullptr if it could not be found 
	void** address_of(const std::string_view class_name) const;

	// Gets the instance pointer to the given singleton, or nullptr if not found/instantiated.
	void* instance_of(const std::string_view class_name) const;

	FD4SingletonFinder(const FD4SingletonFinder&) = delete;
	FD4SingletonFinder(FD4SingletonFinder&&) = delete;

	FD4SingletonFinder& operator=(const FD4SingletonFinder&) = delete;
	FD4SingletonFinder& operator=(FD4SingletonFinder&&) = delete;

	static FD4SingletonFinder& instance() {
		static FD4SingletonFinder ins;
		return ins;
	}

private:
	FD4SingletonFinder();
	std::unordered_map<std::string_view, void**> singleton_addresses;
};

// Template base class to auto-generate convenience method for accessing game singleton (FD4Singleton) classes.
template<class T, FixedString name>
struct FD4Singleton
{
	static constexpr const char* class_name = name;

	// Prevent instantiating anything derived from a FD4Singleton

	FD4Singleton() = delete;
	FD4Singleton(FD4Singleton&) = delete;
	FD4Singleton(FD4Singleton&&) = delete;
	FD4Singleton operator=(FD4Singleton&) = delete;

	// Tries to obtain the static address of this FD4Singleton, if it exists. Returns a null pointer otherwise.
	static T** static_address() {
		static T** addr = (T**)FD4SingletonFinder::instance().address_of(name);
		return addr;
	}

	// Get a pointer to the FD4Singleton instance. Panics if the singleton static address could not be found.
	static T* instance()
	{
		static T** static_address = [] {
			T** addr = FD4Singleton::static_address();
			if (addr == nullptr) {
				// Replace by whatever panic system you'd like
				throw std::runtime_error("static address of FD4Singleton not found");
			}
			return addr;
		}();
		return *static_address;
	}
};