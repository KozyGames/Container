#pragma once

#include <string>

namespace Kozy{
namespace Containers{
class ExampleObject {
public:
	ExampleObject(unsigned int oType);
	ExampleObject() = delete;
	~ExampleObject();

	bool operator==(const ExampleObject& obj) const;
	/**/
	bool operator!=(const ExampleObject& obj) const;

	/* EXPECTED FUNCTIONS */
	bool isSameGroup(const ExampleObject& obj) const;
	std::basic_string<char> toStr() const;

	const unsigned int getID() const;

private:
	unsigned int ID;
	unsigned int objectType;

};

template<class> class Multi_DynamicArray;
/*
* Free function that uses most functions of the Multi_DynamicArray
- adding elements
- deleting elements
- removing elements
- for loop and for range loop
*/
void exampleFunction();

}
}