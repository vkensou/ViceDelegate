#pragma once
#include <memory>
#include <cassert>
#include <type_traits>

namespace Vice
{
	namespace helper
	{
		template<bool flag, typename T, typename U>
		struct Select
		{
			typedef T Result;
		};
		template<typename T, typename U>
		struct Select<false, T, U>
		{
			typedef U Result;
		};

		template<class U> struct AddParameterType
		{
			typedef const U& Result;
		};

		template<class U> struct AddParameterType<U&>
		{
			typedef U& Result;
		};

		template<class T>
		struct ParameterType
		{
			typedef typename Select<std::is_arithmetic<T>::value || std::is_pointer<T>::value || std::is_member_function_pointer<T>::value, T, typename AddParameterType<T>::Result>::Result Type;
		};
	}
	namespace detail
	{
		template<class R, class...T>
		class Delegate_Impl
		{
		public:
			virtual ~Delegate_Impl(){}
			virtual R invoke(typename helper::ParameterType<T>::Type...) = 0;
			virtual Delegate_Impl* clone() = 0;
			virtual bool compare(Delegate_Impl* rhs) = 0;
		};

		template<class R, class...T>
		class Delegate_Function_Impl
			:public Delegate_Impl<R, T...>
		{
			typedef Delegate_Function_Impl<R, T...> My;
			typedef Delegate_Impl<R, T...> Base;
		public:
			typedef R(Function)(T...);
			Delegate_Function_Impl(Function* function) throw()
				:m_func(function)
			{
			}

			R invoke(typename helper::ParameterType<T>::Type...t) override
			{
				return m_func(t...);
			}

			Delegate_Impl* clone() override
			{
				return new Delegate_Function_Impl(m_func);
			}

			bool compare(Base* rhs) override
			{
				My* cast = dynamic_cast<My*>(rhs);
				if (cast)
				{
					return m_func == cast->m_func;
				}
				return false;
			}

		private:
			Function* m_func;
		};

		template<class Obj, class R, class...T>
		class Delegate_ObjMethod_Impl
			:public Delegate_Impl<R, T...>
		{
			typedef Delegate_ObjMethod_Impl<Obj, R, T...> My;
			typedef Delegate_Impl<R, T...> Base;

		public:
			typedef R(Obj::*Method)(T...);

			Delegate_ObjMethod_Impl(Obj* obj, Method method) throw()
				:m_obj(obj), m_method(method)
			{

			}

			R invoke(typename helper::ParameterType<T>::Type...t) override
			{
				return (m_obj->*m_method)(t...);
			}

			Delegate_Impl* clone() override
			{
				return new Delegate_ObjMethod_Impl(m_obj, m_method);
			}

			bool compare(Base* rhs) override
			{
				My* cast = dynamic_cast<My*>(rhs);
				if (cast)
				{
					return m_obj == cast->m_obj && m_method == cast->m_method;
				}
				return false;
			}

		private:
			Obj* m_obj;
			Method m_method;
		};

		template<class Callable, class R, class...T>
		class Delegate_Callable_Impl
			:public Delegate_Impl<R, T...>
		{
			typedef Delegate_Callable_Impl<Callable, R, T...> My;
			typedef Delegate_Impl<R, T...> Base;
		public:
			Delegate_Callable_Impl(Callable& function) throw()
				:m_func(function)
			{
			}

			R invoke(typename helper::ParameterType<T>::Type...t) override
			{
				return m_func(t...);
			}

			Delegate_Impl* clone() override
			{
				return new Delegate_Callable_Impl(m_func);
			}

			bool compare(Base* rhs) override
			{
				return false;
			}

		private:
			Callable m_func;
		};

	}

	template<class T>
	class Delegate;

	template<class R, class...T>
	class Delegate<R(T...)>
	{
	public:
		Delegate() throw()
		{
		}

		template<class Callable>
		Delegate(Callable function) throw()
			: m_dele(make_unique<detail::Delegate_Callable_Impl<Callable, R, T...>>(function))
		{
		}

		typedef R(Function)(T...);
		Delegate(Function* function) throw()
			: m_dele(make_unique<detail::Delegate_Function_Impl<R, T...>>(function))
		{
		}

		template<class Obj>
		Delegate(Obj* obj, R(Obj::*Method)(T...)) throw()
			: m_dele(make_unique<detail::Delegate_ObjMethod_Impl<Obj, R, T...>>(obj, Method))
		{
		}

		Delegate(const Delegate& rhs) throw()
			:m_dele(rhs.m_dele->clone())
		{
		}

		Delegate(Delegate&& rhs) throw()
		{
			swap(rhs);
		}

		Delegate& operator=(Delegate rhs)
		{
			swap(rhs);
			return *this;
		}

		~Delegate() throw()
		{
		}

		void swap(Delegate& other) throw()
		{
			m_dele.swap(other.m_dele);
		}

		R operator()(typename helper::ParameterType<T>::Type...t)
		{
			assert(m_dele);
			m_dele->invoke(t...);
		}

		operator bool() const
		{
			return (bool)m_dele;
		}

		bool operator==(const Delegate& rhs) const
		{
			return m_dele->compare(rhs.m_dele.get());
		}

		bool operator!=(const Delegate& rhs) const
		{
			return !(*this == rhs);
		}

	private:
		std::unique_ptr<detail::Delegate_Impl<R, T...>> m_dele;
	};

	template<class R, class...T>
	void swap(Delegate<R(T...)>& lhs, Delegate<R(T...)>& rhs)
	{
		lhs.swap(rhs);
	}
}