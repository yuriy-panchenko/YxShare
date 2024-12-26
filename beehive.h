#pragma once
#include <functional>
#include <future>
#include <type_traits>

namespace bee
{
	template<typename _Fty>
	class hive final
	{
		using _FObty = std::function<_Fty>;
		using _Rty = typename _FObty::result_type;
		using _RtyPtr = typename std::conditional<std::is_void_v<_Rty>, _Rty, std::unique_ptr<_Rty>>::type;
		using iterator = typename std::vector<std::future<_Rty>>::iterator;
		using const_iterator = typename std::vector<std::future<_Rty>>::const_iterator;
		using _FUty = std::future<_Rty>;

	public:

		hive() = delete;

		hive(const hive&) = delete;

		hive(hive&&) = default;

		hive(_FObty&& fnc) :m_fnc{ fnc } {}

		template<typename FUNCTION>
		hive(FUNCTION f) : hive{ std::function<FUNCTION>{f} } {}

		~hive()
		{
			wait_all();
		}

		//	add task
		template<typename ...Args>
		_RtyPtr launch(Args ...args)
		{
			std::lock_guard _g{ m_mtx };	//	syncing

			if constexpr (ReturnsVoid())
			{
				if (m_bees.size() >= std::thread::hardware_concurrency())
				{
					wait_any();
					m_bees.push_back(std::async(std::launch::async, m_fnc, args...));
				}
				else
				{
					m_bees.push_back(std::async(std::launch::async, m_fnc, args...));
					extract_honey(check_any());
				}
			}
			else
			{
				if (m_bees.size() >= std::thread::hardware_concurrency())
				{
					auto ret{ wait_any() };
					m_bees.push_back(std::async(std::launch::async, m_fnc, args...));
					return ret;
				}
				else
				{
					m_bees.push_back(std::async(std::launch::async, m_fnc, args...));
					return extract_honey(check_any());
				}
			}
		}

		//	wait for any task to finish
		_RtyPtr wait_any()
		{
			if (m_bees.empty())
				if constexpr (!ReturnsVoid())
					return {};

			iterator iter;

			do iter = check_any();
			while (iter == m_bees.end());

			if constexpr (ReturnsVoid())
				extract_honey(iter);
			else return extract_honey(iter);
		}

		//	wait for all tasks to finish
		void wait_all()
		{
			for (auto& val : m_bees)
				val.wait();

			m_bees.clear();
		}

	protected:
		iterator check_any()
		{
			for (auto iter = m_bees.begin(); iter != m_bees.end(); ++iter)
				if (iter->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
					return iter;

			return m_bees.end();
		}

		template<typename ...Args>
		_Rty proc(Args...args)
		{
			return m_fnc(args...);
		}

		static constexpr bool ReturnsVoid()
		{
			return std::is_void_v<_Rty>;
		}

		_RtyPtr extract_honey(iterator iter)
		{
			if constexpr (ReturnsVoid())
			{
				if (iter != m_bees.end())
					m_bees.erase(iter);
			}
			else
			{
				if (iter != m_bees.end())
				{
					auto ret{ iter->get() };
					m_bees.erase(iter);
					return std::make_unique<_Rty>(ret);
				}

				return {};
			}
		}

	private:
		_FObty m_fnc;
		std::vector<_FUty> m_bees;
		std::mutex m_mtx;
	};
}

