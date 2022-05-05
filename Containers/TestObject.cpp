#include "Multi_DynamicArray.h"
#include "TestObject.h"

#include <iostream>


using std::endl; using std::cin; using std::cout;
using Kozy::Containers::Multi_DynamicArray;
using Kozy::Containers::TestObject;
using std::string;

TestObject::TestObject(int randomInteger) : mockField(randomInteger)
{

	static unsigned cnt{ 0 };
	ID = ++cnt;
	cout << ID << " is Created" << endl;
}

TestObject::~TestObject()
{
	cout << ID << " is Deleted" << endl;
}

bool TestObject::operator==(const TestObject& obj) const
{

	return (ID == obj.ID);

}

bool TestObject::operator!=(const TestObject& obj) const
{
	return (ID != obj.ID);

}

bool TestObject::isSameGroup(const TestObject& obj) const
{
	return (mockField != obj.mockField) ? false : true;
}

unsigned int TestObject::getID() const
{
	return ID;
}

std::string Kozy::Containers::TestObject::toStr() const
{
	return { "ID is " + std::to_string(ID) + '\n' + '\0' };
}



void Kozy::Containers::testMulti_DynamicArray()
{
	//Initializing a Multi_DynamicArray
	Multi_DynamicArray<TestObject> dynArr(3, 2, 10, 2);


	//Creating Elements of various types
	TestObject* t = new TestObject(2);

	TestObject* t1 = new TestObject(0);
	TestObject* t2 = new TestObject(0);
	TestObject* t3 = new TestObject(0);
	TestObject* t4 = new TestObject(0);

	TestObject* t5 = new TestObject(1);
	TestObject* t6 = new TestObject(1);

	TestObject* t7 = new TestObject(4);

	TestObject* t8 = new TestObject(1);
	TestObject* t9 = new TestObject(2);
	TestObject* t10 = new TestObject(2);

	TestObject* t11 = new TestObject(5);

	TestObject* t12 = new TestObject(3);

	TestObject* t13 = new TestObject(0);
	TestObject* t14 = new TestObject(0);
	TestObject* t15 = new TestObject(0);

	TestObject* t16 = new TestObject(0);
	TestObject* t17 = new TestObject(0);
	TestObject* t18 = new TestObject(0);
	TestObject* t19 = new TestObject(0);
	TestObject& t20 = *(new TestObject(2));


	

	//Test Adding single elements
	dynArr.addElement(t);

	dynArr.addElement(t1);
	dynArr.addElement(t3);
	dynArr.addElement(t2);
	dynArr.addElement(t4);

	dynArr.addElement(t5);
	dynArr.addElement(t6);

	dynArr.addElement(t7);
	dynArr.addElement(t8);
	dynArr.addElement(t9);
	dynArr.addElement(t10);
	dynArr.addElement(t11);
	dynArr.addElement(t12);
	dynArr.addElement(t13);
	dynArr.addElement(t14);
	dynArr.addElement(t15);
	dynArr.addElement(t16);
	dynArr.addElement(t17);
	dynArr.addElement(t18);
	dynArr.addElement(t19);
	dynArr.addElement(t20);

	//Test adding R-Value to an existing arrayGroup
	unsigned int CNT_ELEMENTS{ 14 };
	unsigned int TYPE_ELEMENT{ 0 };

	for (unsigned int index = 0; index != CNT_ELEMENTS; ++index)
	{
		dynArr.addElement(new TestObject(TYPE_ELEMENT));
	}
	//Test adding R-Value of a new arrayGroup
	CNT_ELEMENTS= 24;
	TYPE_ELEMENT= 6 ;
	for (unsigned int index = 0; index != CNT_ELEMENTS; ++index)
	{
		dynArr.addElement(new TestObject(TYPE_ELEMENT));
	}
	dynArr.printContent(cout);

	//Test iterating through each arrayGroup and their elements and also accessing these elements
	cout << "BEGIN TEST ARRAY CONTENT AND ELEMENT POSITION" << endl;
	cout << endl << "TEST 1" << endl;

	for (unsigned int index{ 0 }; index != dynArr.getTypesSize(); ++index)
	{
		cout << "Group " << index << " contains " << dynArr.getCurrentArraySize(index) << " elements" << endl;

		for (unsigned int index2 = 0; index2 != dynArr.getCurrentArraySize(index); ++index2)
		{
			//You can also access an element like that
			//TestObject& obj = dynArr.getElement(index,index2); 

			const TestObject& obj = *dynArr[index][index2];

			cout
				<< "Group " << index << " Element " << index2 << " ID "
				<< obj.getID() << " Position " << dynArr.getElementPositionOrAdd(obj)
				<< endl;

			cout << "GROUP CHECK. GROUP " << dynArr.getArrayGroupOrAdd(obj) << endl;
		}
		cout << endl;
	}
	cout << "END TEST ARRAY CONTENT AND ELEMENT POSITION" << endl << endl;
	
	//Test adding an array of elements of the same new arrayGroup
	cout << "BEGIN TEST ADDING ARRAY OF ELEMENTS" << endl;
	cout << endl << endl << "TEST 1" << endl;

	
	TYPE_ELEMENT= 42 ;
	TestObject* testArr[4]{};

	for (unsigned int index = 0; index != 4; ++index)
	{
		testArr[index] = new TestObject(TYPE_ELEMENT);
	}
	dynArr.addElementsOfSameGroup(testArr, 4);
	dynArr.printContent(cout);
	
	//Test adding an array of elements of the same existing arrayGroup
	cout << endl << endl << "TEST 2" << endl;

	
	TYPE_ELEMENT= 5 ;
	TestObject* testArr2[9]{};

	for (unsigned int index = 0; index != 9; ++index)
	{
		testArr2[index] = new TestObject(TYPE_ELEMENT);
	}
	dynArr.addElementsOfSameGroup(testArr2, 9);
	dynArr.printContent(cout);

	//Test adding an array of elements of various arrayGroups
	cout << endl << endl << "TEST 3" << endl;

	

	TestObject* testArr3[8]{
	new TestObject(10),
	new TestObject(42),
	new TestObject(10),
	new TestObject(10),
	new TestObject(10),
	new TestObject(3),
	new TestObject(10),
	new TestObject(10)
	};
	dynArr.addElementsOfVariousGroups(testArr3, 8);
	dynArr.printContent(cout);

	cout << "END TEST ADDING ARRAY OF ELEMENTS\n" << endl;
	
	cout << "BEGIN TEST SHRINKING" << endl;

	dynArr.shrinkToSize();
	dynArr.changeArraySize(testArr[0], 30);


	CNT_ELEMENTS= 14;
	TYPE_ELEMENT= 10 ;

	for (unsigned int index = 0; index != CNT_ELEMENTS; ++index)
	{
		dynArr.addElement(new TestObject(TYPE_ELEMENT));
	}

	CNT_ELEMENTS= 4;
	TYPE_ELEMENT=21 ;

	for (unsigned int index = 0; index != CNT_ELEMENTS; ++index)
	{
		dynArr.addElement(new TestObject(TYPE_ELEMENT));
	}
	TYPE_ELEMENT= 22;

	for (unsigned int index = 0; index != CNT_ELEMENTS; ++index)
	{
		dynArr.addElement(new TestObject(TYPE_ELEMENT));
	}

	TYPE_ELEMENT=23 ;

	for (unsigned int index = 0; index != CNT_ELEMENTS; ++index)
	{
		dynArr.addElement(new TestObject(TYPE_ELEMENT));
	}

	dynArr.printContent(cout);
	cout << "END TEST SHRINKING" << endl;

	cout << "\nBEGIN TEST DELETE SINGLE ELEMENTS" << endl;
	dynArr.printContent(cout, dynArr.getArrayGroupOrAdd(testArr3[0]));

	dynArr.deleteElement(testArr3[7]);
	dynArr.deleteElement(testArr3[4]);
	dynArr.deleteElement(dynArr.getArrayGroupOrAdd(testArr3[0]), dynArr.getCurrentArraySize(testArr3[0])-1);

	const TestObject& testSingleElement = dynArr.addElement(new TestObject(44));
	dynArr.printContent(cout, testSingleElement);

	dynArr.deleteElement(testSingleElement);


	cout << "\nEND TEST DELETE SINGLE ELEMENTS" << endl;


	cout << endl << "BEGIN FREEING RESOURCES\n" << endl;

	dynArr.deleteAllElements();
	cout << endl << "END FREEING RESOURCES" << endl;

	cout << endl << "PROGRAM FINISHED. ENTER F TO FINISH" << endl;
	char c{};

	while (cin >> c && c != 'f' && c != 'F')
		;
	


}


