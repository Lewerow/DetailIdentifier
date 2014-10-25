#ifndef UNIQUE_PTR_VERIFIERS_mfioremgf4jgruieojfoiewjwejo3w4jgf45igjreofjodoiewjiowejfoef
#define UNIQUE_PTR_VERIFIERS_mfioremgf4jgruieojfoiewjwejo3w4jgf45igjreofjodoiewjiowejfoef

#include <memory>

namespace helpers
{
	template <typename T>
	class exact_unique_ptr
	{
	public:
		exact_unique_ptr(T* p) : ptr(p)
		{}

		bool operator()(const std::unique_ptr<T>& p) const
		{
			return p.get() == ptr;
		}

	private:
		T* ptr;
	};

	template <typename T>
	exact_unique_ptr<T> exact_unique(T* ptr)
	{
		return exact_unique_ptr<T>(ptr);
	}
}

#endif