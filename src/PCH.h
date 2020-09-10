#pragma once

#pragma warning(push)
#pragma warning(disable : 4200 4324 5053)
#include "F4SE/F4SE.h"
#include "RE/Fallout.h"
#pragma warning(pop)

#include <algorithm>
#include <array>
#include <execution>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeinfo>

#pragma warning(push)
#pragma warning(disable : 4702)
#include <boost/algorithm/searching/knuth_morris_pratt.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container/vector.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <robin_hood.h>
#pragma warning(pop)

#ifdef NDEBUG
#include <spdlog/sinks/basic_file_sink.h>
#else
#include <spdlog/sinks/base_sink.h>

namespace WinAPI
{
	void OutputDebugStringA(const char* a_outputString);
}

namespace logger
{
	template <class Mutex>
	class msvc_sink :
		public spdlog::sinks::base_sink<Mutex>
	{
	private:
		using super = spdlog::sinks::base_sink<Mutex>;

	public:
		explicit msvc_sink() {}

	protected:
		void sink_it_(const spdlog::details::log_msg& a_msg) override
		{
			spdlog::memory_buf_t formatted;
			super::formatter_->format(a_msg, formatted);
			WinAPI::OutputDebugStringA(fmt::to_string(formatted).c_str());
		}

		void flush_() override {}
	};

	using msvc_sink_mt = msvc_sink<std::mutex>;
	using msvc_sink_st = msvc_sink<spdlog::details::null_mutex>;

	using windebug_sink_mt = msvc_sink_mt;
	using windebug_sink_st = msvc_sink_st;
}
#endif

#define DLLEXPORT __declspec(dllexport)

namespace logger
{
	using namespace F4SE::log;
}

namespace stl
{
	using F4SE::stl::report_and_fail;
	using F4SE::stl::span;
	using F4SE::stl::zstring;
	using F4SE::util::to_underlying;

	[[nodiscard]] constexpr char tolower(char a_ch) noexcept
	{
		constexpr auto table = []() noexcept {
			std::array<char, std::numeric_limits<unsigned char>::max() + 1> buf{ '\0' };
			for (std::size_t i = 0; i < buf.size(); ++i) {
				buf[i] = static_cast<char>(i);
			}

			for (unsigned char i = 'A'; i <= 'Z'; ++i) {
				buf[static_cast<std::size_t>(i)] = static_cast<char>(i + 32);
			}

			return buf;
		}();

		return table[static_cast<unsigned char>(a_ch)];
	}

	[[nodiscard]] constexpr char toupper(char a_ch) noexcept
	{
		constexpr auto table = []() noexcept {
			std::array<char, std::numeric_limits<unsigned char>::max() + 1> buf{ '\0' };
			for (std::size_t i = 0; i < buf.size(); ++i) {
				buf[i] = static_cast<char>(i);
			}

			for (unsigned char i = 'a'; i <= 'z'; ++i) {
				buf[static_cast<std::size_t>(i)] = static_cast<char>(i - 32);
			}

			return buf;
		}();

		return table[static_cast<unsigned char>(a_ch)];
	}

	[[nodiscard]] inline std::string_view safe_string(const char* a_str) noexcept { return a_str ? a_str : ""; }

	class cistring_iterator :
		public boost::iterator_facade<
			cistring_iterator,
			char,
			std::random_access_iterator_tag,
			char,
			std::ptrdiff_t>
	{
	private:
		using super =
			boost::iterator_facade<
				cistring_iterator,
				char,
				std::random_access_iterator_tag,
				char,
				std::ptrdiff_t>;

		using proxy_type = std::string_view::iterator;

	public:
		using iterator_category = typename super::iterator_category;
		using value_type = typename super::value_type;
		using difference_type = typename super::difference_type;
		using pointer = typename super::pointer;
		using reference = typename super::reference;

		cistring_iterator(proxy_type a_proxy) noexcept :
			_proxy(std::move(a_proxy))
		{}

		cistring_iterator() noexcept = default;
		cistring_iterator(const cistring_iterator&) noexcept = default;
		cistring_iterator(cistring_iterator&&) noexcept = default;

		cistring_iterator& operator=(const cistring_iterator&) noexcept = default;
		cistring_iterator& operator=(cistring_iterator&&) noexcept = default;

	protected:
		friend class boost::iterator_core_access;

		reference dereference() const noexcept { return tolower(*_proxy); }

		bool equal(const cistring_iterator& a_rhs) const noexcept { return _proxy == a_rhs._proxy; }

		void increment() noexcept { ++_proxy; }

		void decrement() noexcept { --_proxy; }

		void advance(difference_type a_off) noexcept { _proxy += a_off; }

		difference_type distance_to(const cistring_iterator& a_rhs) const noexcept { return a_rhs._proxy - _proxy; }

	private:
		proxy_type _proxy;
	};

	template <class... Args>
	auto make_array(Args&&... a_args)
		-> std::array<std::common_type_t<Args...>, sizeof...(Args)>
	{
		return { std::forward<Args>(a_args)... };
	}
}

using namespace std::literals;
