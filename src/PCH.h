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

#include <boost/algorithm/searching/knuth_morris_pratt.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container/vector.hpp>
#include <boost/iterator/iterator_facade.hpp>

#pragma warning(push)
#pragma warning(disable : 4702)
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

	template <
		class CharT,
		class Traits = std::char_traits<CharT>>
	class basic_safe_string :
		public std::basic_string_view<CharT, Traits>
	{
	private:
		using super = std::basic_string_view<CharT, Traits>;

	public:
		using traits_type = typename super::traits_type;
		using value_type = typename super::value_type;
		using pointer = typename super::pointer;
		using const_pointer = typename super::const_pointer;
		using reference = typename super::reference;
		using const_reference = typename super::const_reference;
		using const_iterator = typename super::const_iterator;
		using iterator = typename super::iterator;
		using const_reverse_iterator = typename super::const_reverse_iterator;
		using reverse_iterator = typename super::reverse_iterator;
		using size_type = typename super::size_type;
		using difference_type = typename super::difference_type;

		constexpr basic_safe_string(const_pointer a_str) :
			super(a_str ? a_str : EMPTY)
		{}

		using super::super;
		using super::operator=;

	private:
		static constexpr value_type EMPTY[] = { static_cast<value_type>(0) };
	};

	using safe_string = basic_safe_string<char>;
	extern template class basic_safe_string<char>;

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
}

using namespace std::literals;
