#include "ExampleElement.h"

#define _MULTI_DYNAMIC_NO_CHECK_
#include "Multi_DynamicArray.h"

#include <map>

using std::endl; using std::cin; using std::cout;
using std::string;
using std::map;
using std::to_string;
using namespace Kozy::Containers;

struct un2 {
	unsigned int group;
	bool isDeleted;
};
void set(unsigned int ID, unsigned gro, bool isDeleted, map<unsigned, un2>* ptr = nullptr) {
	static map<unsigned, un2>* info{ ptr };
	if (!ptr) {
		(*info)[ID].group = gro;
		(*info)[ID].isDeleted = isDeleted;
	}
}

void printMap(map<unsigned, un2>* ptr, bool onlyNotDeleted) {
	unsigned cntNotDeleted{ 0 };
	if (onlyNotDeleted) {
		cout << "Listing all elements that have not been properly deleted." << endl;
		for (auto& e : *ptr)
			if (!e.second.isDeleted) {
				cout << "Group: " + to_string(e.second.group) + " ID: " + to_string(e.first) + " isDeleted: " + to_string(e.second.isDeleted) + '\n';
				++cntNotDeleted;
			}
	}
	else {
		cout << "Listing all elements." << endl;
		for (auto& e : *ptr) {
			cout << "Group: " + to_string(e.second.group) + " ID: " + to_string(e.first) + " isDeleted: " + to_string(e.second.isDeleted) + '\n';
			if (!e.second.isDeleted) ++cntNotDeleted;
		}
	}
	cout << to_string(cntNotDeleted) + " elements have not been deleted!\n";
}

ExampleObject::ExampleObject(unsigned int oType) :objectType{ oType } {
	static unsigned cnt{ 0 };
	ID = ++cnt;
	cout << ID << " is Created" << endl;
	set(ID, objectType, false, nullptr);
}
ExampleObject::~ExampleObject() {
	cout << ID << " is Deleted" << endl;
	set(ID, objectType, true, nullptr);
}

bool ExampleObject::operator==(const ExampleObject& obj) const {
	return (ID == obj.ID);
}
bool ExampleObject::operator!=(const ExampleObject& obj) const {
	return (ID != obj.ID);
}

bool ExampleObject::isSameGroup(const ExampleObject& obj) const {
	return (objectType != obj.objectType) ? false : true;
}
unsigned int ExampleObject::getID() const {
	return ID;
}
std::basic_string<char> ExampleObject::toStr() const {
	return "ID is " + to_string(ID) + " GroupType " + to_string(objectType);
}

void Kozy::Containers::testMulti_DynamicArray() {
	/* CONTROL MAP */
	map<unsigned, un2> testmap;
	set(0, 0, false, &testmap);

	//Initializing a Multi_DynamicArray
	Multi_DynamicArray<ExampleObject> arr(3, 2, 10, 2);

	//creating Elements
	arr.addElement(*new ExampleObject(44));
	arr.addElement(*new ExampleObject(42));
	arr.addElement(*new ExampleObject(43));
	arr.addElement(*new ExampleObject(42));

	arr.addElement(*new ExampleObject(43));
	arr.addElement(*new ExampleObject(44));
	arr.addElement(*new ExampleObject(44));
	arr.addElement(*new ExampleObject(45));
	auto a = new ExampleObject(11);

	arr.printContent(cout);

	arr.removeElement(*a);			//does not delete element
	printMap(&testmap, true);
	arr.deleteAllElements();		//deletes elements!
	printMap(&testmap, true);
	arr << *a;						//same as addElement()
	arr.printContent(cout, arr.getArrayGroupOrAdd(*new ExampleObject(45)));	// type 45 now exists again and the function will add the element

	printMap(&testmap, true);
	arr.printContent(cout);

	arr.deleteAllElements();

	printMap(&testmap, true);
	testmap.clear();

	cout << endl << "PROGRAM FINISHED. ENTER F TO EXIT\n";

	char c{};
	while (cin >> c && c != 'f' && c != 'F')
		;
}



