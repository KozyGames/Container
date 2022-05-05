#pragma once

#include <iostream>
using std::cout;
using std::cin;
using std::endl;

/*
***********************************************************************************************************************************************
* LEGEND
***********************************************************************************************************************************************
* Element
* - An Element is the r-value object that the user want to be stored inside the container. It is sometimes used interchangeably with a pointer to that value.
* 
* Container
* - A variable of this type is a Container. For example std::vector and std::list are Containers of the standard C++ library.
* - This Container consists of ArrayGroups, which are tightly packed pointers to Elements. Be aware that only pointers inside an ArrayGroup are tightly packed.
* 
* ArrayGroup
* - it is a pointer to an array of tightly packed pointers to not neccessarily tightly packed elements. All elements in an ArrayGroup return true when any element's
* - isSameGroup method is used with any element of the same ArrayGroup as an argument.
* 
* Capacity
* - how many Elements the Container expects to receive
* - the rate of increase is determined by the field arrayDynamicIncRate, which can be changed through changeArrayIncreaseRate by the user.
* 
* (Element-)Type / Group
* - The grouptype of an element determines to which ArrayGroup they will be assigned.
* 
***********************************************************************************************************************************************
*/

/*
***********************************************************************************************************************************************
* SUMMARY
***********************************************************************************************************************************************
* This is a lightweight container that dynamically allocates arrays of dynamic sizes, these grow when the user defined capacity is reached or the user preemptively decides to. 
* The usage of this container is comparable to a wrapper of various vectors, albeit with some differences. 
* Internally, elements are unsorted. 
* 
* Eventhough this container was made for a specific use case, I tried to make it usable for general purposes.
* 
* My use case:
* I need a container that stores objects, which these inherit of the same main mother-object and have a unique ID. These can be different in all regards, except for the group they belong to.
* Further, I need access to one object of each group, no matter which of them.
* Also, iterating and adding elements shall be as multithread friendly as possible. Deleting elements is neither expected nor neccassary to be multithreadable.
* 
***********************************************************************************************************************************************
* SPECIFICS
***********************************************************************************************************************************************
* IF NEEDED, THE CONTAINER CAN TAKE OWNERSHIP OF THE ELEMENTS, AS IN, IT CAN BE THE ONE TO FREE UP MEMORY.
* THE DESTRUCTOR DOES NOT DELETE ELEMENTS ON ITS OWN. IF YOU WANT THE CONTAINER TO FREE MEMORY, CALL deleteElements() BEFORE ITS LIFETIME ENDS.
* 
* MOSTLY, IT DOES NOT CHECK FOR THE INPUT TO BE VALID. THE USER IS RESPONSIBLE FOR VALID INPUT. 
* IF THE USER IS UNSURE, THEY CAN USE FUNCTIONS LIKE: bool contains(T*) TO ENSURE THAT AN ELEMENT IS OR IS NOT ALREADY IN THE CONTAINER;
* AND unsigned getElementPosition(T*) TO GET THE CURRENT POSITION INSIDE THE ARRAYGROUP. 
* DELETING AN ELEMENT MAY INVALIDATE ANY POSITIONS OF ELEMENTS IN THE SAME ARRAYGROUP:
* 
***********************************************************************************************************************************************
* ASSUMPTIONS OF ELEMENTS
***********************************************************************************************************************************************
* - ELEMENTS HAVE THE bool OPERATOR==(const T&) METHOD
* -- THE OPERATOR== METHOD RETURNS TRUE IF THE CALLER IS THE SAME ELEMENT OR AN ELEMENT OF THE SAME VALUE AS THE PASSED ELEMENT
* - ELEMENTS HAVE THE METHOD bool isSameGroup(const T& element)
* -- RETURNS TRUE IF THE CALLER IS OF THE SAME GROUP AS THE PASSED ELEMENT 
* - ELEMENTS MAY OPTIONALLY HAVE A toStr() METHOD THAT RETURNS A C-STYLE STRING OR STANDARD LIBRARY STRING
*
*/

namespace Kozy
{
	namespace Containers
	{
		template <typename T>
		class Multi_DynamicArray
		{
		public:
			Multi_DynamicArray(unsigned typesCap, unsigned typesDynRate, unsigned arrayCap, unsigned arrayDynRate) {

				//initialize typesCapacity
				if (typesCap > 1) typesCapacity = typesCap;

				//initialize typesDynamicIncRate
				if (typesDynRate > 1) typesDynamicIncRate = typesDynRate;

				//initialize arrayDynamicIncRate
				if (arrayDynRate != 0)
					arrayDynamicIncRate = arrayDynRate;

				//initialize arrayCapacity Part I
				arrayCapacity = new unsigned int[typesCapacity];
				if (arrayCap > 8)
					initialArrayCapacity = arrayCap;

				//initialize pointerToArrayGroup and currentArraySize
				pointerToArrayGroup = new const T * *[typesCapacity];
				currentArraySize = new unsigned int[typesCapacity];
				for (unsigned int index = 0; index != typesCapacity; ++index) {
					pointerToArrayGroup[index] = new const T * [initialArrayCapacity];

					currentArraySize[index] = 0;
					arrayCapacity[index] = initialArrayCapacity; //initialize arrayCapacity Part II
				}
			}
			Multi_DynamicArray() {
				arrayCapacity = new unsigned int[typesCapacity];

				pointerToArrayGroup = new const T * *[typesCapacity];
				currentArraySize = new unsigned int[typesCapacity];
				for (unsigned int index = 0; index != typesCapacity; ++index) {
					pointerToArrayGroup[index] = new const T * [initialArrayCapacity];

					currentArraySize[index] = 0;
					arrayCapacity[index] = initialArrayCapacity; //initialize arrayCapacity Part II
				}
			}


			~Multi_DynamicArray() {
				delete[] pointerToArrayGroup;
				delete[] currentArraySize;
				delete[] arrayCapacity;
			}
			void deleteAllElements() {
				unsigned int oldTypesCapacity = typesCapacity;
				typesCapacity = 0;
				for (unsigned int index = 0; index != oldTypesCapacity; ++index)
				{
					for (unsigned int index2 = 0; index2 != currentArraySize[index]; ++index2) //deletes elements!
						delete pointerToArrayGroup[index][index2];
				}
			}

			const T& addElement(const T& element) {
				for (unsigned int index = 0; index != currentGroupTypesSize; ++index)
				{
					if (element.isSameGroup(*pointerToArrayGroup[index][0]))
					{
						if (currentArraySize[index] == arrayCapacity[index])
							growArrayGroup(index);

						pointerToArrayGroup[index][currentArraySize[index]++] = &element;
						return element;
					}
				}

				if (currentGroupTypesSize == typesCapacity)
					growTypesCapacity();

				pointerToArrayGroup[currentGroupTypesSize][0] = &element;
				currentArraySize[currentGroupTypesSize++] = 1;

				return element;
			}
			const T& addElement(T* element) {
				for (unsigned int index = 0; index != currentGroupTypesSize; ++index)
				{
					if (element->isSameGroup(*pointerToArrayGroup[index][0]))
					{
						if (currentArraySize[index] == arrayCapacity[index])
							growArrayGroup(index);

						pointerToArrayGroup[index][currentArraySize[index]++] = element;
						return *element;
					}
				}

				if (currentGroupTypesSize == typesCapacity)
					growTypesCapacity();

				pointerToArrayGroup[currentGroupTypesSize][0] = element;
				currentArraySize[currentGroupTypesSize++] = 1;

				return *element;
			}

			Multi_DynamicArray& operator=(const Multi_DynamicArray& arr) = delete;
			Multi_DynamicArray& operator+=(const Multi_DynamicArray& arr) {
				for (unsigned int index = 0; index != arr.currentGroupTypesSize; ++index)
					addElementsOfSameGroup(arr.pointerToArrayGroup[index], arr.currentArraySize[index]);
				return this;
			}
			/*
			* checks for count not being zero
			*/
			T** addElementsOfSameGroup(T** arrayPtr, unsigned int count) {
				if (!count)
					return arrayPtr;

				unsigned int group = getArrayGroupOrAdd(arrayPtr[0]);
				if (!contains(arrayPtr[0]))
					addElement(arrayPtr[0]);

				unsigned int neededSize = currentArraySize[group] + count - 1;

				if ((arrayCapacity[group] - currentArraySize[group]) < neededSize)
					changeArrayGroupCapacity(group, neededSize);


				for (unsigned int index = currentArraySize[group]; count != 1; ++index)
					pointerToArrayGroup[group][index] = arrayPtr[--count];

				currentArraySize[group] = neededSize;
				return arrayPtr;
			}
			T** addElementsOfVariousGroups(T** arrayPtr, unsigned count) {
				if (!count)
					return arrayPtr;
				else {
					unsigned int positionOfFirst{ 0 }, arraySize{ 1 };
					for (unsigned int nextIndex{ 1 }; nextIndex != count; ++nextIndex) //iterate through each element and look for ranges of elements of the same group
					{
						if (!arrayPtr[nextIndex]->isSameGroup(*arrayPtr[positionOfFirst])) //when a different element is found, add the range to the container
						{
							addElementsOfSameGroup(&arrayPtr[positionOfFirst], arraySize); 
							positionOfFirst += arraySize;
							arraySize = 1;
						}
						else
							++arraySize;
					}

					addElementsOfSameGroup(&arrayPtr[positionOfFirst], arraySize);

					return arrayPtr;
				}
			}
			void changeArrayIncreaseRate(unsigned int value) {
				if (value) //makes sure value is at least 1
					arrayDynamicIncRate = value;
				else
					arrayDynamicIncRate = 0;
			}
			void changeTypeIncreaseRate(unsigned int value) {
				if (value) //makes sure value is at least 1
					typesDynamicIncRate = value;
				else
					typesDynamicIncRate = 0;
			}
			void shrinkToSize() {
				changeTypeCapacity(currentGroupTypesSize);

				for (unsigned int index{ 0 }; index != currentGroupTypesSize; ++index)
					changeArrayGroupCapacity(index, currentArraySize[index]);
			}
			void changeTypeCapacity(unsigned int value) {
				if (value >= currentGroupTypesSize) 
					growTypesCapacity(value);
			}
			
			const T** operator[](unsigned int arrGroup) {
				return pointerToArrayGroup[arrGroup];
			}
			const T*** getArray() const {

				return pointerToArrayGroup;
			}
			const T& getElement(unsigned arrayGroup, unsigned position) {

				return *pointerToArrayGroup[arrayGroup][position];
			}
			/*
			* IF ELEMENT DOES NOT EXIST IN THE ARRAY, THEN IT WILL BE ADDED
			*/
			unsigned int getArrayGroupOrAdd(const T& element)
			{
				for (unsigned int index = 0; index != currentGroupTypesSize; ++index)
				{
					if (element.isSameGroup(*pointerToArrayGroup[index][0]))
						return index;
				}

				addElement(element);
				return currentGroupTypesSize - 1;

			}
			unsigned int getArrayGroupOrAdd(T* element)
			{
				for (unsigned int index = 0; index != currentGroupTypesSize; ++index)
				{
					if (element->isSameGroup(*pointerToArrayGroup[index][0]))
						return index;
				}

				addElement(element);
				return currentGroupTypesSize - 1;

			}

			unsigned int getElementPositionOrAdd(const T& element)
			{
				for (unsigned int index = 0; index != typesCapacity; ++index)
				{
					for (unsigned int index2 = 0; index2 != currentArraySize[index]; ++index2)
					{
						if (element == *pointerToArrayGroup[index][index2])
							return index2;
					}
				}

				addElement(element);
				return getElementPositionOrAdd(element);
			}
			unsigned int getElementPositionOrAdd(T* element) {
				for (unsigned int index = 0; index != typesCapacity; ++index)
				{
					for (unsigned int index2 = 0; index2 != currentArraySize[index]; ++index2)
					{
						if (*element == *pointerToArrayGroup[index][index2])
							return index2;
					}
				}

				addElement(element);
				return getElementPositionOrAdd(element);
			}

			unsigned int getTypesSize() {
				return currentGroupTypesSize;
			}
			unsigned int getCurrentArraySize(const T& element) {
				return currentArraySize[getArrayGroupOrAdd(element)];
			}
			unsigned int getCurrentArraySize(T* element) {
				return currentArraySize[getArrayGroupOrAdd(element)];
			}
			unsigned int getCurrentArraySize(unsigned int groupArray) {
				return currentArraySize[groupArray];
			}
			unsigned int getCurrentArrayCapacity(const T& element) {
				return arrayCapacity[getArrayGroupOrAdd(element)];
			}
			unsigned int getCurrentArrayCapacity(T* element) {
				return arrayCapacity[getArrayGroupOrAdd(element)];
			}
			unsigned int getCurrentArrayCapacity(unsigned int groupArray) {
				return arrayCapacity[groupArray];
			}

			unsigned int getTypesCapacity() {
				return typesCapacity;
			}
			unsigned int getTypesCapacityDynamicIncreaseRate() {
				return typesDynamicIncRate;
			}
			unsigned int getArrayGroupCapacityDynamicIncreaseRate() {
				return arrayDynamicIncRate;
			}
			unsigned int getInitialArrayCapacityValue() {
				return initialArrayCapacity;
			}

			const T** beginGroupIterator() {
				return pointerToArrayGroup[0];
			}
			const T** endGroupIterator() {
				return pointerToArrayGroup[0] + currentGroupTypesSize;
			}

			bool contains(const T& element) {
				for (unsigned int index = 0; index != typesCapacity; ++index)
				{
					for (unsigned int index2 = 0; index2 != currentArraySize[index]; ++index2)
					{
						if (element == *pointerToArrayGroup[index][index2])
							return true;
					}
				}
				return false;
			}
			bool contains(T* element) {
				for (unsigned int index = 0; index != typesCapacity; ++index)
				{
					for (unsigned int index2 = 0; index2 != currentArraySize[index]; ++index2)
					{
						if (*element == *pointerToArrayGroup[index][index2])
							return true;
					}
				}
				return false;
			}

			void growTypesCapacity() {
				unsigned int newTypesCapacity = typesCapacity + typesDynamicIncRate;

				const T*** newBiggerTypes = new const T * *[newTypesCapacity];
				unsigned int* newBiggerArrayCapacity = new unsigned int[newTypesCapacity];
				unsigned int* newBiggerArraySize = new unsigned int[newTypesCapacity];

				for (unsigned int index = 0; index != typesCapacity; ++index) {
					newBiggerArrayCapacity[index] = arrayCapacity[index];
					newBiggerArraySize[index] = currentArraySize[index];

					newBiggerTypes[index] = pointerToArrayGroup[index];
					//ASK Leonard why I sometimes get a buffer overflow warning here
				}

				for (unsigned int index = typesCapacity; index != newTypesCapacity; ++index)
				{
					newBiggerArrayCapacity[index] = initialArrayCapacity;
					newBiggerArraySize[index] = 0;

					newBiggerTypes[index] = new const T * [initialArrayCapacity];
				}

				unsigned int* old = arrayCapacity;
				arrayCapacity = newBiggerArrayCapacity;
				delete[] old;

				old = currentArraySize;
				currentArraySize = newBiggerArraySize;
				delete[] old;

				const T*** oldTypes = pointerToArrayGroup;
				pointerToArrayGroup = newBiggerTypes;
				delete[] oldTypes;

				typesCapacity = newTypesCapacity;
			}

			void growArray(unsigned int arrayGroup) {
				growArrayGroup(arrayGroup);
			}
			void growArray(T* element) {
				growArrayGroup(getElementPositionOrAdd(element));
			}
			void growArray(const T& element) {
				growArrayGroup(getElementPositionOrAdd(element));
			}
			void changeArraySize(unsigned int arrayGroup, unsigned int newSize) {
				if (newSize > currentArraySize[arrayGroup])
					changeArrayGroupCapacity(arrayGroup, newSize);
			}
			void changeArraySize(T* element, unsigned int newSize) {
				changeArraySize(getElementPositionOrAdd(element), newSize);
			}
			void changeArraySize(const T& element, unsigned int newSize) {
				changeArraySize(getElementPositionOrAdd(element), newSize);
			}
			/*
			* ASSUMES
			* - elements have a toStr() function that returns a value that can be printed by the ostream object
			*/
			void printContent(std::ostream& output) {
				output << endl << "BEGIN READING ELEMENTS" << endl;

				for (unsigned int index = 0; index != currentGroupTypesSize; ++index)
				{
					output << "Group " << index << " contains " << currentArraySize[index] << " elements" << endl;
					for (unsigned index2 = 0; index2 != currentArraySize[index]; ++index2)
					{
						output << "Group " << index << " Position " << index2 << '\n';

						const T* a = pointerToArrayGroup[index][index2];
						output << a->toStr();

					}
				}
				output << "END READING ELEMENTS\n" << endl;
			}
			void printContent(std::ostream& output, unsigned int arrayGroup) {
				output << endl << "BEGIN READING ELEMENTS OF ARRAYGROUP " << arrayGroup << endl;

				output << "Group " << arrayGroup << " contains " << currentArraySize[arrayGroup] << " elements" << endl;
				for (unsigned index = 0; index != currentArraySize[arrayGroup]; ++index)
				{
					output << "Group " << arrayGroup << " Position " << index << '\n';

					const T* a = pointerToArrayGroup[arrayGroup][index];
					output << a->toStr() << '\n';
				}

				output << "END READING ELEMENTS\n" << endl;
			}
			void printContent(std::ostream& output, const T& element) {
				printContent(output, getArrayGroupOrAdd(element));
			}
			void printContent(std::ostream& output, T* element) {
				printContent(output, getArrayGroupOrAdd(element));
			}


			void deleteElement(T* element) {
				unsigned int arrayGroup = getArrayGroupOrAdd(element);
				deleteElement(arrayGroup, getElementPositionOrAdd(element, arrayGroup));
			}
			void deleteElement(const T& element) {
				unsigned int arrayGroup = getArrayGroupOrAdd(element);
				deleteElement(arrayGroup, getElementPositionOrAdd(element, arrayGroup));
			}
			void deleteElement(unsigned int arrayGroup, unsigned int position) {
				if (currentArraySize[arrayGroup] != 1) {
					const T* tempElement = pointerToArrayGroup[arrayGroup][position];
					pointerToArrayGroup[arrayGroup][position] = pointerToArrayGroup[arrayGroup][--currentArraySize[arrayGroup]];
					delete tempElement;
				}
				else
					deleteArrayGroup(arrayGroup);

			}
			/*
			* deletes the group AND its elements!
			*/
			void deleteArrayGroup(unsigned int arrayGroup) {
				unsigned int elementsToDelete = currentArraySize[arrayGroup];
				currentArraySize[arrayGroup] = 0;
				while (elementsToDelete != 0)
					delete pointerToArrayGroup[arrayGroup][--elementsToDelete];

				pointerToArrayGroup[arrayGroup] = pointerToArrayGroup[--currentGroupTypesSize];
				currentArraySize[arrayGroup] = currentArraySize[currentGroupTypesSize];
				arrayCapacity[arrayGroup] = arrayCapacity[currentGroupTypesSize];
			}
			/*
			* deletes the group AND its elements!
			*/
			void deleteArrayGroup(T* element) {
				deleteArrayGroup(getArrayGroupOrAdd(element));
			}
			/*
			* deletes the group AND its elements!
			*/
			void deleteArrayGroup(const T& element) {
				deleteArrayGroup(getArrayGroupOrAdd(element));
			}

		private:
			const T*** pointerToArrayGroup;

			unsigned int typesCapacity{ 1 };
			unsigned int typesDynamicIncRate{ 1 };
			unsigned int currentGroupTypesSize{ 0 };

			unsigned int* arrayCapacity;
			unsigned int arrayDynamicIncRate{ 1 };
			unsigned int* currentArraySize;
			unsigned int initialArrayCapacity{ 8 };

			void growArrayGroup(unsigned int arrayGroup) {
				const T** newBiggerArray = new const T * [arrayCapacity[arrayGroup] + arrayDynamicIncRate];
				for (unsigned int index = 0; index != currentArraySize[arrayGroup]; ++index)
				{
					newBiggerArray[index] = pointerToArrayGroup[arrayGroup][index];
				}

				const T** old = pointerToArrayGroup[arrayGroup];
				pointerToArrayGroup[arrayGroup] = newBiggerArray;
				arrayCapacity[arrayGroup] += arrayDynamicIncRate;
				delete[] old;
			}
			/*
			* assumes newSize is at least as big as current size of array
			*/
			void changeArrayGroupCapacity(unsigned int arrayGroup, unsigned int newSize) {
				const T** newBiggerArray = new const T * [newSize];
				for (unsigned int index = 0; index != currentArraySize[arrayGroup]; ++index)
				{
					newBiggerArray[index] = pointerToArrayGroup[arrayGroup][index];
				}

				const T** old = pointerToArrayGroup[arrayGroup];
				pointerToArrayGroup[arrayGroup] = newBiggerArray;
				arrayCapacity[arrayGroup] = newSize;
				delete[] old;
			}

			void growTypesCapacity(unsigned int newTypesCapacity) {
				const T*** newBiggerTypes = new const T * *[newTypesCapacity];
				unsigned int* newBiggerArrayCapacity = new unsigned int[newTypesCapacity];
				unsigned int* newBiggerArraySize = new unsigned int[newTypesCapacity];

				for (unsigned int index = 0; index != currentGroupTypesSize; ++index)
				{
					newBiggerArrayCapacity[index] = arrayCapacity[index];
					newBiggerArraySize[index] = currentArraySize[index];

					newBiggerTypes[index] = pointerToArrayGroup[index];
				}

				for (unsigned int index = currentGroupTypesSize; index != newTypesCapacity; ++index)
				{
					newBiggerArrayCapacity[index] = initialArrayCapacity;
					newBiggerArraySize[index] = 0;

					newBiggerTypes[index] = new const T * [initialArrayCapacity];
				}

				unsigned int* old = arrayCapacity;
				arrayCapacity = newBiggerArrayCapacity;
				delete[] old;

				old = currentArraySize;
				currentArraySize = newBiggerArraySize;
				delete[] old;

				const T*** oldTypes = pointerToArrayGroup;
				pointerToArrayGroup = newBiggerTypes;
				delete[] oldTypes;

				typesCapacity = newTypesCapacity;
			}

			unsigned int getElementPositionOrAdd(T* element, unsigned int arrayGroup)
			{
				for (unsigned int index = 0; index != currentArraySize[arrayGroup]; ++index)
				{
					if (*element == *pointerToArrayGroup[arrayGroup][index])
						return index;
				}

				return getElementPositionOrAdd(element);
			}
			unsigned int getElementPositionOrAdd(const T& element, unsigned int arrayGroup)
			{
				for (unsigned int index = 0; index != currentArraySize[arrayGroup]; ++index) {
					if (element == *pointerToArrayGroup[arrayGroup][index])
						return index;
				}
				return getElementPositionOrAdd(element);
			}
		};
	}
}


/*
* BUGS:
* - memory leaks
* -- Auxilliary memory is not correctly made free. Elements are correctly deleted.
*
* TO DO:
* - write comments
* - fix memory leaks
* - add a swap method
*/

/*
* Guide
* Get a specific element of the Multiarray
* - TestObject& obj = dynArr.getElement(index,index2);
* - TestObject* obj = dynArr[index][index2];
*/