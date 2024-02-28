// Bad template type resolution
#if 0
#include <iostream>

template <typename T>
void print(const T& val) 
{
	std::cout << "simple print: " << val << "\n";
}

template <>
void print(const double& val)
{
	std::cout << "some special processing for FP: " << val << "\n";
}

int main() 
{
	print(5);
	print(3.14);
	print(3.14f);
}
#endif

// SFINAE
#if 0
#include <iostream>

template <typename T,
		  typename = std::enable_if_t<!std::is_floating_point_v<T>>,
		  bool = true>
void print(const T& val) 
{
	std::cout << "simple: " << val << "\n";
}

template <typename T,
		  typename = std::enable_if_t<std::is_floating_point_v<T>>>
void print(const T& val) 
{
	std::cout << "some special processing for FP: " << val << "\n";
}

int main() 
{
	print(5);
	print(3.14);
	print(3.14f);
}

#endif

// Simple solution with concepts
#if 0
#include <iostream>
#include <concepts>

template <typename T>
void print(const T& val) 
{
	std::cout << "simple: " << val << "\n";
}

template <std::floating_point T>
void print(const T& val)
{
	std::cout << "some special processing for FP: " << val << "\n";
}

int main() 
{
	print(5);
	print(3.14);
	print(3.14f);
}
#endif

//Errors with wrong usage of standard lib
#if 0
#include <vector>
#include <set>
#include <atomic>

//concept
#if 0
template<typename Collection, typename Type>
concept SupportsPushBack = requires(Collection c, Type v)
{
	c.push_back(v);
};

template<typename Collection, typename Type>
	requires SupportsPushBack<Collection, Type> &&
			 std::convertible_to<Type, typename Collection::value_type>
void add(Collection& coll, const Type val)
{
	coll.push_back(val);
}

#else

template<typename Collection, typename Type>
void add(Collection& coll, const Type& val)
{
	coll.push_back(val);
}
#endif

int main()
{
	std::vector<int> vec;
	add(vec, 42); // OK
	add(vec, "hello"); // ERROR

	std::set<int> coll;
	add(coll, 42); // ERROR

	std::vector<std::atomic<int>> aiVec;
	std::atomic<int> ai{ 42 };
	add(aiVec, ai); // ERROR

}
#endif

//First constraint example
#if 0
#include <iostream>
template<typename T>
void print(const T& a)
{
	std::cout << a << "\n";
}

template<typename T>
	requires std::is_pointer_v<T>
void print(const T& a)
{
	print(*a);
}

int main()
{
	int x = 42;
	int* p = &x;

	print(x);
	print(&x);
	print(p);
	print(&p);
}
#endif

//First concept example
#if 0
#include <iostream>

template <typename T>
concept IsPointer = std::is_pointer_v<T>;

template<typename T>
//	requires (!IsPointer<T>)
void print(const T& a) 
{
	std::cout << a << "\n";
}

template<typename T>
	requires IsPointer<T>
void print(const T& a) 
{
	print(*a);
}

template<IsPointer T>
void print1(const T& a) 
{
	print(*a);
}

void print2(const IsPointer auto& a) 
{
	print(*a);
}


int main()
{
	int x = 42;
	int* p = &x;

	print(x);
	print(&x);
	print(p);
	print(&p);
}
#endif

// Second concept example
#if 0
#include <iostream>

auto Max(const auto& a, const auto& b)
{
	return b < a ? a : b;
}

template <typename T>
concept IsPointer = std::is_pointer_v<T>;

template <IsPointer T>
auto Max(const T& a, const T& b)
{
	return Max(*a, *b);
}

auto Max2(const IsPointer auto& a, const IsPointer auto& b)
{
	return Max(*a, *b);
}

int main()
{
	int x = 3, y = 5;
	std::cout << Max(x, y) << "\n";
	std::cout << Max(&x, &y) << "\n";

	float z = 7.3f;
	std::cout << Max2(&x, &z) << "\n";
}
#endif

// Different constrained elements
#if 0
#include <concepts>
#include <ranges>
#include <iostream>

//class
template<typename T>
	requires std::integral<T>
class MyType {
	//...
};

// type alias
template<std::ranges::range T>
using ValueType = std::ranges::range_value_t<T>;

// template variable
template<std::ranges::range T>
constexpr bool IsIntegralValRange = std::integral<std::ranges::range_value_t<T>>;


// member functions
template<typename T>
class ValueOrCollection {
	T value;
public:
	ValueOrCollection(const T& val)
		: value{ val } 
	{
	}
	
	ValueOrCollection(T&& val)
		: value{ std::move(val) } 
	{
	}
	
	void print() const
	{
		std::cout << value << '\n';
	}
	
	void print() const
		requires std::ranges::range<T> 
	{
		for (const auto& elem : value) {
			std::cout << elem << ' ';
		}
		std::cout << '\n';
	}
};

//ERROR!
//template <std::ranges::range T>
//concept IntRange = std::integral<std::ranges::range_value_t<T>>;

//ERROR!
//template <typename T>
//	requires std::ranges::range<T>
//concept IntRange = std::integral<std::ranges::range_value_t<T>>;

// Correct
template <typename T>
concept IntRange =
	std::ranges::range<T> && 
	std::integral<std::ranges::range_value_t<T>>;

#endif

// Value parameters
#if 0
#include <concepts>

template<int Val>
concept LessThan10 = Val < 10;

template<typename T, int Size>
	requires LessThan10<Size>
class MyType 
{
	//...	
};

template <std::integral auto Max>
class MyClass 
{
	//...
};

//template <LessThan10 Max> // ERROR
class MyClass2 
{
	//...
};

#endif

// if constexpr
#if 0
#include <concepts>
#include <set>
#include <vector>

template<typename Coll>
concept SupportsPushBack = requires(Coll coll) 
{
	coll.push_back(std::declval<typename Coll::value_type&>());
};

template <typename Collection, typename Val>
void add(Collection& c, const Val& v)
{
	//if constexpr (requires {c.push_back(v); }) {
	if constexpr (SupportsPushBack<Collection>) {
		c.push_back(v);
	}
	else {
		c.insert(v);
	}
}

int main()
{
	std::set<int> s;
	std::vector<int> v;
	add(s, 10);
	add(v, 10);
}
#endif

// Subsume
#if 0 
#include <concepts>
#include <iostream>

struct Color {};

template<typename T>
concept CadObject = requires(T obj) {
	{ obj.width() } -> std::integral;
	{ obj.height() } -> std::integral;
	obj.bbox();
};

template<typename T>
concept ColoredCadObject =
	CadObject<T> &&		// subsumes concept GeoObject
	requires(T obj) {	// additional constraints
		obj.setColor(Color{});
		{ obj.getColor() } -> std::convertible_to<Color>;
	}
;
#endif

// Not working if constraint is no concept
#if 0
#include <concepts>

template<typename T>
	requires std::is_convertible_v<T, int>
void print(T) 
{
	std::cout << "Version 1\n";
}
template<typename T>
	requires (std::is_convertible_v<T, int> && sizeof(int) >= 4)
void print(T) 
{
	std::cout << "Version 2\n";
}

int main() 
{
	print(42); // Error if no concepts. Replacing value "is_convertible_v" with concept "convertible_to" will solve the problem
}

#endif

// No implicit subsume - standartization discussion
#if 0
// declared in a header file:
template<typename T>
concept GeoObject = requires(T obj) {
	obj.draw();
};

// declared in another header file:
template<typename T>
concept Cowboy = requires(T obj) {
	obj.draw();
	obj = obj;
};

// assume that we overload a function template for both GeoObject and Cowboy:
template<GeoObject T>
void print(T) 
{
	//...
}

template<Cowboy T>
void print(T) 
{
	//...
}
#endif

// Concept subsumtion and commutative constraints
#if 0
#include <concepts>

template<typename T, typename U>
concept SameAs = std::is_same_v<T, U>;


//template<typename T, typename U>
//concept SameAsHelper = std::is_same_v<T, U>;
//
//template<typename T, typename U>
//concept SameAs = SameAsHelper<T, U> && SameAsHelper<U, T>; // make commutative

template<typename T, typename U>
	requires SameAs<T, U>
void foo(T, U)
{
	std::cout << "foo() for parameters of same type" << '\n';
}

template<typename T, typename U>
	requires SameAs<U, T> && std::integral<T> // use SameAs with other order
void foo(T, U)
{
	std::cout << "foo() for integral parameters of same type" << '\n';
}

int main() 
{
	foo(1, 2);
}

#endif
