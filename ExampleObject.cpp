#include "ExampleObject.h"
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
void setInfo(unsigned int ID, unsigned gro, bool isDeleted, map<unsigned, un2>* ptr = nullptr) {
	static map<unsigned, un2>* info{ ptr };
	if (!ptr) {			//map sends nullptr
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
	cout << cntNotDeleted << " elements have not been deleted!\n\n";
}

ExampleObject::ExampleObject(unsigned int oType) :objectType{ oType } {
	static unsigned cnt{ 0 };
	ID = ++cnt;
	cout << ID << " is Created" << endl;
	setInfo(ID, objectType, false, nullptr);
}
ExampleObject::~ExampleObject() {
	cout <<"Element with ID: "<< ID << " is Deleted" << endl;
	setInfo(ID, objectType, true, nullptr);
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
std::basic_string<char> ExampleObject::toStr() const {
	return "ID is " + to_string(ID) + " GroupType " + to_string(objectType);
}

const unsigned int ExampleObject::getID() const {
	return ID;
}

void Kozy::Containers::exampleFunction(){
	// CONTROL MAP
	map<unsigned, un2> testmap;
	setInfo(0, 0, false, &testmap);

	//Initialization of Multi_DynamicArray
	Multi_DynamicArray<ExampleObject> arr(3, 2, 10, 2);

	//Creating Elements
	arr.addElement(*new ExampleObject(44));	//ID: 1 GroupType: 44
	arr.addElement(*new ExampleObject(42)); //ID: 2 GroupType: 42
	arr.addElement(*new ExampleObject(43)); //ID: 3 GroupType: 43
	arr.addElement(*new ExampleObject(42)); //ID: 4 GroupType: 42
	arr.addElement(*new ExampleObject(44)); //ID: 5 GroupType: 44

	arr.addElement(*new ExampleObject(43)); //ID: 6 GroupType: 43
	arr.addElement(*new ExampleObject(44)); //ID: 7 GroupType: 44
	arr.addElement(*new ExampleObject(44)); //ID: 8 GroupType: 44
	arr.addElement(*new ExampleObject(45)); //ID: 9 GroupType: 45
	auto& lVal = *new ExampleObject(11);	//ID: 10 GroupType: 11
	arr << lVal;

	cout << endl<<"Range Test 1\n\n";
	//Standard for loop

	for (auto& iter = arr.begin(); iter != arr.end(); iter++)
		for (auto& iter2 = iter.begin(); iter2 != iter.end(); iter2++)
			cout << iter2->toStr() << endl;

	cout << "\nRange Test 2\n";
	//for range loop 
	//prints content of the first element of each group and then inclusive the first element the rest of that group

	for (auto& iter : arr) {
		cout <<'\n'<<
			"First of this group:\n"<< 
			iter->toStr() << '\n' << endl;
		for (auto& iter2 : iter) 
			cout << iter2.toStr() << endl;
	}

	cout << "\nRange Test 3\n\n";
	//prints all elements in order of groups

	for (auto& iter : arr)
		for (auto& iter2 : iter)
			cout << iter2.toStr() << endl;

	cout <<"\nRemove and Delete Test\n"<< endl;
	//remove single element
	arr.removeElement(lVal);			//does not delete element

	cout << '\n';
	printMap(&testmap, false);

	//remove array of elements, first group
	unsigned int cnt{arr.getCurrentArraySize(0)};
	ExampleObject** remArr = new ExampleObject* [cnt];
	arr.removeAndGetArrayGroup(0, remArr);

	cout << "\n";

	arr.deleteAllElements();		//deletes all elements inside the array!
	printMap(&testmap, true);

	arr << lVal;						//same as addElement()
	arr.addElementsOfSameGroup(remArr, 2);	//adds the first two elements back

	arr.deleteAllElements();

	printMap(&testmap, true);

	cout << "Deleting removed Elements.\n\n";
	for (unsigned index{ 2 }; index != cnt; ++index) 	//deletes rest of the removed array
		delete remArr[index];
	
	delete[] remArr;

	printMap(&testmap, true);
	testmap.clear();

	cout << endl << "PROGRAM FINISHED. ENTER F TO EXIT\n";

	char c{};
	while (cin >> c && c != 'f' && c != 'F')
		;
}







