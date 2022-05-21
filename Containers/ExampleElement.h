#pragma once

#include <string>


namespace Kozy
{
namespace Containers{
class ExampleObject {
public:
	ExampleObject(unsigned int oType);
	ExampleObject() = delete;
	~ExampleObject();
	bool operator==(const ExampleObject& obj) const;
	bool operator!=(const ExampleObject& obj) const;

	bool isSameGroup(const ExampleObject& obj) const;

	unsigned int getID() const;
	std::basic_string<char> toStr() const;

private:
	unsigned int ID;
	unsigned int objectType;

};
void testMulti_DynamicArray();

}
}