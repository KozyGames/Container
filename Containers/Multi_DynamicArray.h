#pragma once
#include <iostream>

/*
***********************************************************************************************************************************************
* LEGEND
***********************************************************************************************************************************************
* Element
* - An Element is the r-value object that the user want to be stored inside the container. It is used interchangeably with a pointer to that value.
* 
* Container
* - A variable of this type is a Container. For example std::vector and std::list are Containers of the standard C++ library.
* - This Container consists of ArrayGroups, which are tightly packed pointers to Elements. Be aware that only pointers inside an ArrayGroup are tightly packed.
* 
* ArrayGroup
* - it is a pointer to an array of tightly packed pointers to elements. All elements in an ArrayGroup return true when any element's
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
* This is a lightweight container that dynamically allocates arrays of dynamic sizes, these grow when the user defined capacity is reached or the user preemptively decides to increase them. 
* It was compiled and tested in the C++14 Standard and upwards compatible.
* The usage of this container is comparable to a wrapper of various vectors, albeit with some differences. 
* Internally, elements are UNSORTED. 
* 
* Eventhough this container was made for a specific use case, I tried to make it usable for general purposes.
* 
* My use case:
* I need a container that stores objects, which inherit from the same main mother-object and have a unique ID. These objects can be drastically different in all regards, except for the pre-determined group they belong to.
* Further, I need fast direct access to one object of each group, no matter which of them.
* 
***********************************************************************************************************************************************
* SPECIFICS
***********************************************************************************************************************************************
* IF NEEDED, THE CONTAINER CAN TAKE OWNERSHIP OF THE ELEMENTS, AS IN, IT CAN BE THE ONE TO FREE UP MEMORY. IT CALLS THE DELETE FUNCTION.
* THE DESTRUCTOR DOES NOT DELETE ELEMENTS ON ITS OWN. IF YOU WANT THE CONTAINER TO FREE MEMORY, CALL deleteElements() BEFORE ITS LIFETIME ENDS.
* 
* MOSTLY, IT DOES NOT CHECK FOR THE INPUT TO BE VALID. THE USER IS RESPONSIBLE FOR VALID INPUT. 
* IF THE USER IS UNSURE, THEY CAN USE FUNCTIONS LIKE: bool contains(T&) TO ENSURE THAT AN ELEMENT IS OR IS NOT ALREADY IN THE CONTAINER;
* AND getElementPosition(T*) TO GET THE CURRENT POSITION INSIDE THE ARRAYGROUP. 
* DELETING AN ELEMENT MAY INVALIDATE ANY POSITIONS OF ELEMENTS IN THE SAME ARRAYGROUP.
* 
***********************************************************************************************************************************************
* ASSUMPTIONS OF ELEMENTS
***********************************************************************************************************************************************
* * ELEMENTS HAVE THE bool OPERATOR==(T&) METHOD
* * * THE OPERATOR== METHOD RETURNS TRUE IF THE CALLER IS THE SAME ELEMENT OR AN ELEMENT OF THE SAME VALUE AS THE PASSED ELEMENT
* * ELEMENTS HAVE THE METHOD bool isSameGroup(T& element)
* * * RETURNS TRUE IF THE CALLER IS OF THE SAME GROUP AS THE PASSED ELEMENT 
* * ELEMENTS MAY OPTIONALLY HAVE A toStr() METHOD THAT RETURNS FOR EXAMPLE A C-STYLE STRING OR STANDARD LIBRARY STRING
* 
***********************************************************************************************************************************************
* MACROS
***********************************************************************************************************************************************
* _MULTI_DYNAMIC_NO_CHECK_
* * IF DEFINED, NO SAFETY CHECKS WILL BE DONE
* 
*/


namespace Kozy{
	namespace Containers
	{
		template <typename T>
		class Multi_DynamicArray{
		public:
			friend class GroupsIterator;
			friend class ArrayIterator;

			/** CONSTRUCTOR **/
			Multi_DynamicArray(unsigned typesCap, unsigned typesDynRate, unsigned arrayCap, unsigned arrayDynRate) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (!typesCap)
					throw std::invalid_argument("The Capacity of Types has to be at least 1");
				if (!typesDynRate)
					throw std::invalid_argument("The Growing Rate of Types has to be at least 1.");
				if (!arrayDynRate)
					throw std::invalid_argument("The Growing Rate of ArrayGroups has to be at least 1.");
				if (arrayCap<4)
					throw std::invalid_argument("The Capacity of ArrayGroups has to be at least 4.");
#endif
				//initialize typesCapacity
				typesCapacity = typesCap;
				//initialize typesDynamicIncRate
				typesDynamicIncRate = typesDynRate;
				//initialize arrayDynamicIncRate
				arrayDynamicIncRate = arrayDynRate;
				//initialize arrayCapacity Part I
				arrayCapacity = new unsigned int[typesCapacity];
				initialArrayCapacity = arrayCap;

				//initialize pointerToArrayGroup and currentArraySize
				pointerToArrayGroup = new T * *[typesCapacity];
				currentArraySize = new unsigned int[typesCapacity];
				for (unsigned int index = 0; index != typesCapacity; ++index) {
					pointerToArrayGroup[index] = new T * [initialArrayCapacity];

					currentArraySize[index] = 0;
					arrayCapacity[index] = initialArrayCapacity; //initialize arrayCapacity Part II
				}
			}
			Multi_DynamicArray() {
				arrayCapacity = new unsigned int[typesCapacity];

				pointerToArrayGroup = new T * *[typesCapacity];
				currentArraySize = new unsigned int[typesCapacity];
				for (unsigned int index = 0; index != typesCapacity; ++index) {
					pointerToArrayGroup[index] = new T * [initialArrayCapacity];

					currentArraySize[index] = 0;
					arrayCapacity[index] = initialArrayCapacity; //initialize arrayCapacity Part II
				}
			}


			/** OPERATOR OVERLOADING **/
			/* creates a copy */
			Multi_DynamicArray& operator=(Multi_DynamicArray& arr) {
				pointerToArrayGroup=arr.pointerToArrayGroup;

				typesCapacity== arr.typesCapacity;
				typesDynamicIncRate = arr.typesDynamicIncRate;
				currentGroupTypesSize = arr.currentGroupTypesSize;

				arrayCapacity = arr.arrayCapacity;
				arrayDynamicIncRate = arr.arrayDynamicIncRate;
				currentArraySize = arr.currentArraySize;
				initialArrayCapacity = arr.initialArrayCapacity;
			};

			Multi_DynamicArray& operator>>(Multi_DynamicArray& arr) {
				for (unsigned int index{ 0 }; index != currentGroupTypesSize; ++index) {
					arr.addElementsOfSameGroup(pointerToArrayGroup[index], currentArraySize[index]);
					currentArraySize[index] = 0;
				}
				currentGroupTypesSize = 0;
				return *this;
			}
			Multi_DynamicArray& operator<<(Multi_DynamicArray& arr) {
				arr >> *this;
				return *this;
			}
			Multi_DynamicArray& operator<<(T& element) {
				addElement(element);
				return *this;
			}
			T** operator[](unsigned int arrGroup) {
				return pointerToArrayGroup[arrGroup];
			}

			/** ADDING ELEMENTS FUNCTIONS **/
			T& addElement(T& element) {
				for (unsigned int index = 0; index != currentGroupTypesSize; ++index)
					if (element.isSameGroup(*pointerToArrayGroup[index][0])) {
						if (currentArraySize[index] == arrayCapacity[index])
							growArrayGroup(index);

						pointerToArrayGroup[index][currentArraySize[index]++] = &element;
						return element;
			}
				if (currentGroupTypesSize == typesCapacity)
					growTypesCapacity();

				pointerToArrayGroup[currentGroupTypesSize][0] = &element;
				currentArraySize[currentGroupTypesSize++] = 1;

				return element;
			}
			/* checks for count not being zero */
			T** addElementsOfSameGroup(T** arrayPtr, unsigned int count) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (!arrayPtr) 
					throw std::runtime_error("The pointer to an array of your Elements is NULL");
#endif
				if (!count) return arrayPtr;
				else {
					const unsigned int group = getArrayGroup(addElement(*arrayPtr[0]));
					const unsigned int neededSize = currentArraySize[group] + count - 1;

					if ((arrayCapacity[group] - currentArraySize[group]) < neededSize)
						changeArrayGroupCapacity(group, neededSize);

					for (unsigned int index = currentArraySize[group]; count != 1; ++index)
						pointerToArrayGroup[group][index] = arrayPtr[--count];

					currentArraySize[group] = neededSize;
					return arrayPtr;
				}
			}
			void addElementsOfVariousGroups(T** arrayPtr, unsigned count) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (!arrayPtr)
					throw std::runtime_error("The pointer to an array of your Elements is NULL");
#endif
				if (!count) return;
				else {
					unsigned int positionOfFirst{ 0 }, arraySize{ 1 };
					for (unsigned int nextIndex{ 1 }; nextIndex != count; ++nextIndex) //iterate through each element and look for ranges of elements of the same group
						if (!arrayPtr[nextIndex]->isSameGroup(*arrayPtr[positionOfFirst])) {//when an element of a different type is found, add the range to the container
							addElementsOfSameGroup(&arrayPtr[positionOfFirst], arraySize);
							positionOfFirst = nextIndex;
							arraySize = 1;
						}
						else
							++arraySize;
					
					addElementsOfSameGroup(&arrayPtr[positionOfFirst], arraySize);
				}
			}

			/** BASIC CHANGE FUNCTIONS **/
			void changeArrayIncreaseRate(unsigned int value) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (!value) //makes sure value is at least 1
					arrayDynamicIncRate = 0;
				else
#endif
					arrayDynamicIncRate = value;
			}
			void changeTypeIncreaseRate(unsigned int value) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (!value) //makes sure value is at least 1
					typesDynamicIncRate = 0;
				else
#endif
					typesDynamicIncRate = value;
			}
			void changeInitialArrayCapacity(unsigned value) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if(value) 
#endif
					initialArrayCapacity = value;
			}
			void changeTypeCapacity(unsigned int value) {
				if (value >= currentGroupTypesSize) 
					growTypesCapacity(value);
			}

			/** INFORMATION GETTER **/
			T& getElement(unsigned arrayGroup, unsigned position) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (arrayGroup < currentGroupTypesSize)
					throw std::out_of_range("Invalid arrayGroup");
				if (currentArraySize[arrayGroup] > position)
					throw std::out_of_range("Invalid position");
#endif
				return *pointerToArrayGroup[arrayGroup][position];
			}
			/* BE AWARE THAT IF THE ARRAYGROUP IS FOUND, THEN THE ELEMENT DOES NOT GET ADDED! */
			unsigned int getArrayGroupOrAdd(T& element){
				for (unsigned int index = 0; index != currentGroupTypesSize; ++index){
					if (element.isSameGroup(*pointerToArrayGroup[index][0]))
						return index;
				}

				addElement(element);
				return currentGroupTypesSize - 1;
			}
			/* returns currentGroupTypesSize if it does not contain element */
			unsigned int getArrayGroup(T& element) {
				for (unsigned int index = 0; index != currentGroupTypesSize; ++index) 
					if (element.isSameGroup(*pointerToArrayGroup[index][0]))
						return index;
				
				return currentGroupTypesSize;
			}
			unsigned int getElementPositionOrAdd(T& element) {
				for (unsigned int index = 0; index != typesCapacity; ++index){
					for (unsigned int index2 = 0; index2 != currentArraySize[index]; ++index2)
					{
						if (element == *pointerToArrayGroup[index][index2])
							return index2;
					}
				}

				addElement(element);
				return getElementPositionOrAdd(element);
			}
			unsigned int getElementPositionInSpecificArrayGroupOrAdd(T& element, unsigned int arrayGroup) {
				for (unsigned int index = 0; index != currentArraySize[arrayGroup]; ++index) {
					if (element == *pointerToArrayGroup[arrayGroup][index])
						return index;
				}
				return getElementPositionOrAdd(element);
			}
			unsigned int getTypesSize() {
				return currentGroupTypesSize;
			}
			unsigned int getCurrentArraySizeOrAdd(T& element) {
				return currentArraySize[getArrayGroupOrAdd(element)];
			}
			unsigned int getCurrentArraySize(unsigned int arrayGroup) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (arrayGroup <= currentGroupTypesSize)
					throw std::out_of_range("Invalid arrayGroup");
#endif
				return currentArraySize[arrayGroup];
			}
			unsigned int getCurrentArrayCapacity(unsigned int arrayGroup) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (arrayGroup <= currentGroupTypesSize)
					throw std::out_of_range("Invalid arrayGroup");
#endif
				return arrayCapacity[arrayGroup];
			}
			unsigned int getCntAllElements() {
				unsigned int result{ 0 };
				for (unsigned int index{ 0 }; index != currentGroupTypesSize; ++index)
					result += currentArraySize[index];
				return result;
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

			/** ITERATOR **/
			class GroupsIterator {
			public:
				friend class ArrayIterator;
				class ArrayIterator {
				public:
					friend class GroupsIterator;

					ArrayIterator(GroupsIterator& ref, unsigned int pos = 0) :index { pos }, fatherRef{ ref } {}
					T& operator*() {
						return fatherRef.multiRef.*pointerToArrayGroup[fatherRef.index][index];
					}

					ArrayIterator& operator++() {
						++index;
						return *this;
					}
					ArrayIterator& operator++(int) {
						ArrayIterator copyOfIter = *this;
						++index;
						return *this;
					}
					T* operator->() {		
						return fatherRef.multiRef.pointerToArrayGroup[fatherRef.index][index];
					}
					bool operator!=(const ArrayIterator& iter) {
						return index != iter.index;
					}
					bool operator==(const ArrayIterator& iter) {
						return index == iter.index;
					}
				private:
					unsigned int index;
					GroupsIterator& fatherRef;

				};
				GroupsIterator(Multi_DynamicArray& ref, unsigned int pos=0) : index{ pos }, multiRef{ ref }, arrayIter{ *this } {}
				
				GroupsIterator& operator++() {
					++index;
					arrayIter.index = 0;
					return *this;
				}

				GroupsIterator operator++(int) {
					GroupsIterator copyOldIter = *this;
					++index;
					arrayIter.index = 0;		
					return copyOldIter;
				}
				bool operator!=(const GroupsIterator& iter) {
					return index != iter.index;
				}
				bool operator==(const GroupsIterator& iter) {
					return index == iter.index;
				}
				T& operator*() {
					return multiRef.*pointerToArrayGroup[index][0];
				}
				T* operator->() {
					return multiRef.pointerToArrayGroup[index][0];
				}
				ArrayIterator& begin() {
					return arrayIter;
				}
				ArrayIterator end() {
					return ArrayIterator(*this, multiRef.getCurrentArraySize(index));
				}
			private:
				unsigned int index;
				ArrayIterator arrayIter;
				Multi_DynamicArray& multiRef;
				
			};

			GroupsIterator begin() {
				return GroupsIterator(*this);
			}
			GroupsIterator end() {
				return GroupsIterator(*this,currentGroupTypesSize);
			}

			/** CHECK FUNCTIONS **/
			bool containsElement(T& element) {
				for (unsigned int index = 0; index != typesCapacity; ++index)
					for (unsigned int index2 = 0; index2 != currentArraySize[index]; ++index2)
						if (element == *pointerToArrayGroup[index][index2])
							return true;
					
				return false;
			}
			bool containsType(T& element) {
				for (unsigned int index = 0; index != typesCapacity; ++index)
						if (element.isSameGroup(*pointerToArrayGroup[index][0]))
							return true;

				return false;
			}
			bool isEmpty() {
				 return (!currentArraySize[0]);
			}

			/** GROWTH **/
			void growTypesCapacity() {
				unsigned int newTypesCapacity = typesCapacity + typesDynamicIncRate;

				T*** newBiggerTypes = new T * *[newTypesCapacity];
				unsigned int* newBiggerArrayCapacity = new unsigned int[newTypesCapacity];
				unsigned int* newBiggerArraySize = new unsigned int[newTypesCapacity];

				for (unsigned int index{ 0 }; index != typesCapacity; ++index) {						//Copy old elements and info
					newBiggerArrayCapacity[index] = arrayCapacity[index];
					newBiggerArraySize[index] = currentArraySize[index];

					newBiggerTypes[index] = pointerToArrayGroup[index];
				}

				for (unsigned int index{ typesCapacity }; index != newTypesCapacity; ++index) {		//initialize new capacity range
					newBiggerArrayCapacity[index] = initialArrayCapacity;
					newBiggerArraySize[index] = 0;

					newBiggerTypes[index] = new T * [initialArrayCapacity];
				}

				/* delete old arrays */
				delete[] arrayCapacity;
				arrayCapacity = newBiggerArrayCapacity;

				delete[] currentArraySize;
				currentArraySize = newBiggerArraySize;

				delete[] pointerToArrayGroup;
				pointerToArrayGroup = newBiggerTypes;

				typesCapacity = newTypesCapacity;
			}
			void growArrayGroup(unsigned int arrayGroup) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (arrayGroup <= currentGroupTypesSize)
					throw std::out_of_range("Invalid arrayGroup");
#endif
				T** newBiggerArray = new T * [arrayCapacity[arrayGroup] + arrayDynamicIncRate];
				for (unsigned int index = 0; index != currentArraySize[arrayGroup]; ++index) {
					newBiggerArray[index] = pointerToArrayGroup[arrayGroup][index];
				}

				delete[] pointerToArrayGroup[arrayGroup];
				pointerToArrayGroup[arrayGroup] = newBiggerArray;
				arrayCapacity[arrayGroup] += arrayDynamicIncRate;
			}
			void growArrayOrAdd(T& element) {
				growArrayGroup(getElementPositionOrAdd(element));
			}
			void changeArraySize(unsigned int arrayGroup, unsigned int newSize) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (arrayGroup <= currentGroupTypesSize)
					throw std::out_of_range("Invalid arrayGroup");
#endif
				if (newSize > currentArraySize[arrayGroup])
					changeArrayGroupCapacity(arrayGroup, newSize);
			}
			void shrinkToSize() {
				changeTypeCapacity(currentGroupTypesSize);

				for (unsigned int index{ 0 }; index != currentGroupTypesSize; ++index)
					changeArrayGroupCapacity(index, currentArraySize[index]);
			}

			/** DEBUG INFORMATION **/
			/*
			* ASSUMES
			* - elements have a toStr() function that returns a value that can be printed by the ostream object
			*/
			void printContent(std::ostream& output) {
				output << "\nBEGIN READING ELEMENTS\n";
				for (unsigned int index = 0; index != currentGroupTypesSize; ++index)
				{
					output << "Group " << index << " contains " << currentArraySize[index] << " elements\n";
					for (unsigned index2 = 0; index2 != currentArraySize[index]; ++index2)
					{
						output << "Group " << index << " Position " << index2 << '\n'
						 << pointerToArrayGroup[index][index2]->toStr()<<std::endl;
					}
					output<<'\n';
				}
				output << "END READING ELEMENTS\n" << std::endl;
			}
			void printContent(std::ostream& output, unsigned int arrayGroup) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (arrayGroup <= currentGroupTypesSize)
					throw std::out_of_range("Invalid arrayGroup");
#endif
				output << "\nBEGIN READING ELEMENTS OF ARRAYGROUP " << arrayGroup << '\n';

				output << "Group " << arrayGroup << " contains " << currentArraySize[arrayGroup] << " elements\n";
				for (unsigned index = 0; index != currentArraySize[arrayGroup]; ++index)
				{
					output << "Group " << arrayGroup << " Position " << index << '\n';
					output << pointerToArrayGroup[arrayGroup][index]->toStr() << '\n';
				}

				output << "\nEND READING ELEMENTS\n" << std::endl;
			}
			void printContent(std::ostream& output, T& element) {
				printContent(output, getArrayGroupOrAdd(element));
			}

			/** DELETE AND REMOVE **/
			/* Does not delete Element
			* if it is not inside the multi_array, nothing happens
			*/
			T& removeElement(T& element) {
				for (unsigned int index{ 0 }; index != currentGroupTypesSize;)
					if (!element.isSameGroup(*pointerToArrayGroup[index][0]))
						++index;
					else
						for (unsigned int index2{ 0 }; index2 != currentArraySize[index]; ++index2)
							if (element == *pointerToArrayGroup[index][index2])
								if (--currentArraySize[index]) {			//there are still elements inside the arrayGroup
									pointerToArrayGroup[index][index2] = pointerToArrayGroup[index][currentArraySize[index]];
									return element;
								}
								else {										//the arrayGroup is now empty
									if (!(--currentGroupTypesSize)) {		//the multi_array is now empty
										return element;
									}
									else {									//there are still arrayGroups left
										T** temp{ pointerToArrayGroup[index] };
										unsigned int tempCapacity{ arrayCapacity[index] };

										pointerToArrayGroup[index] = pointerToArrayGroup[currentGroupTypesSize];
										currentArraySize[index] = currentArraySize[currentGroupTypesSize];
										arrayCapacity[index] = arrayCapacity[currentGroupTypesSize];

										pointerToArrayGroup[currentGroupTypesSize] = temp;
										currentArraySize[currentGroupTypesSize] = 0;
										arrayCapacity[currentGroupTypesSize] = tempCapacity;
										return element;
									}
								}

				return element;
			}
			~Multi_DynamicArray() {
				for (unsigned index{ 0 }; index != typesCapacity; ++index)
					delete[] pointerToArrayGroup[index];
				delete[] pointerToArrayGroup;
				delete[] currentArraySize;
				delete[] arrayCapacity;
			}
			void deleteAllElements() {
				for (unsigned int index{ 0 }; index != currentGroupTypesSize; ++index) {
					for (unsigned int index2 = 0; index2 != currentArraySize[index]; ++index2) //deletes elements!
						delete pointerToArrayGroup[index][index2];
					currentArraySize[index] = 0;
				}
				currentGroupTypesSize = 0;
			}
			/* deletes element regardless if it is in the array or not */
			void deleteElement(T& element) {
				for (unsigned int index{ 0 }; index != currentGroupTypesSize;) {						//search for group
					if (!element.isSameGroup(*pointerToArrayGroup[index][0]))
						++index;
					else 
						for (unsigned int index2{ 0 }; index2 != currentArraySize[index]; ++index2)		//search for Element
							if (element == *pointerToArrayGroup[index][index2]) {
								deleteElementAt(index, index2);
								return;
							}
				}
				delete& element;
			}
			void deleteElementAt(unsigned int arrayGroup, unsigned int position) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (arrayGroup <= currentGroupTypesSize)
					throw std::out_of_range("Invalid arrayGroup");
				if (position <= currentArraySize[arrayGroup])
					throw std::out_of_range("Invalid position");
#endif
				if (currentArraySize[arrayGroup]!=1) {
					T* tempElement = pointerToArrayGroup[arrayGroup][position];
					pointerToArrayGroup[arrayGroup][position] = pointerToArrayGroup[arrayGroup][--currentArraySize[arrayGroup]];
					delete tempElement;
				}
				else
					deleteArrayGroup(arrayGroup);
			}
			/*
			* deletes the group AND its elements!
			* Does nothing if arrayGroup has no elements or there are no elements at all. Can not be higher than capacity!
			*/
			void deleteArrayGroup(unsigned int arrayGroup) {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (arrayGroup <= currentGroupTypesSize)
					throw std::out_of_range("Invalid arrayGroup");
#endif
				unsigned int elementsToDelete = currentArraySize[arrayGroup];
				while (elementsToDelete != 0)
					delete pointerToArrayGroup[arrayGroup][--elementsToDelete];

				if (!(--currentGroupTypesSize)) {
					currentArraySize[0] = 0;
				}
				else {
					T** temp{ pointerToArrayGroup[arrayGroup] };
					unsigned int tempCap{ arrayCapacity[arrayGroup] };

					pointerToArrayGroup[arrayGroup] = pointerToArrayGroup[currentGroupTypesSize];
					pointerToArrayGroup[currentGroupTypesSize] = temp;
					currentArraySize[arrayGroup] = currentArraySize[currentGroupTypesSize];
					currentArraySize[currentGroupTypesSize] = 0;
					arrayCapacity[arrayGroup] = arrayCapacity[currentGroupTypesSize];
					arrayCapacity[currentGroupTypesSize] = tempCap;
				}
			}
			/*
			* deletes the group AND its elements!
			* Does not delete the passed element
			*/
			void deleteArrayGroupOfThisType(T& element) {
				deleteArrayGroup(getArrayGroup(element));
			}

			/*
			* This function is being used for a Garbage-Collector
			*	The Garbage-Collector deletes an Element whenever workload is low
			* POSITIONS OF ELEMENTS CAN CHANGE. NOT GUARRANTED THAT NEWEST ELEMENT IS DELETED
			*/
			void deleteRandomElement() {
#ifndef _MULTI_DYNAMIC_NO_CHECK_
				if (isEmpty()) {
					throw std::exception("Multi_DynamicArray::deleteLastElement Assumes That There Is At Least One Element.\nPlease check if it is empty or use deleteRandomElementOrDoNothing!");
					return;
				}
#endif
				delete pointerToArrayGroup[currentGroupTypesSize-1][--currentArraySize[currentGroupTypesSize-1]];
				if (!currentArraySize[currentGroupTypesSize-1])
					--currentGroupTypesSize;
			}
			/*
			* This function is being used for a Garbage-Collector
			*	The Garbage-Collector deletes an Element whenever workload is low
			* POSITIONS OF ELEMENTS CAN CHANGE. NOT GUARRANTED THAT NEWEST ELEMENT IS DELETED
			*/
			void deleteRandomElementOrDoNothing() {
				if (!isEmpty()) {
					delete pointerToArrayGroup[currentGroupTypesSize-1][--currentArraySize[currentGroupTypesSize-1]];
					if (!currentArraySize[currentGroupTypesSize-1])
						--currentGroupTypesSize;
				}
			}
			

			T*** getArray() {
				return pointerToArrayGroup;
			}

		private:
			T*** pointerToArrayGroup;

			unsigned int typesCapacity{ 1 };
			unsigned int typesDynamicIncRate{ 1 };
			unsigned int currentGroupTypesSize{ 0 };

			unsigned int* arrayCapacity;
			unsigned int arrayDynamicIncRate{ 1 };
			unsigned int* currentArraySize;
			unsigned int initialArrayCapacity{ 4 };

			/*
			* assumes newSize is at least as big as current size of array
			*/
			void changeArrayGroupCapacity(unsigned int arrayGroup, unsigned int newSize) {
				T** newBiggerArray = new T * [newSize];
				for (unsigned int index = 0; index != currentArraySize[arrayGroup]; ++index)
					newBiggerArray[index] = pointerToArrayGroup[arrayGroup][index];
				
				delete pointerToArrayGroup[arrayGroup];
				pointerToArrayGroup[arrayGroup] = newBiggerArray;
				arrayCapacity[arrayGroup] = newSize;
			}
			void growTypesCapacity(unsigned int newTypesCapacity) {
				T*** newBiggerTypes = new T * *[newTypesCapacity];
				unsigned int* newBiggerArrayCapacity = new unsigned int[newTypesCapacity];
				unsigned int* newBiggerArraySize = new unsigned int[newTypesCapacity];

				for (unsigned int index = 0; index != currentGroupTypesSize; ++index){
					newBiggerArrayCapacity[index] = arrayCapacity[index];
					newBiggerArraySize[index] = currentArraySize[index];

					newBiggerTypes[index] = pointerToArrayGroup[index];
				}

				for (unsigned int index = currentGroupTypesSize; index != newTypesCapacity; ++index){
					newBiggerArrayCapacity[index] = initialArrayCapacity;
					newBiggerArraySize[index] = 0;

					newBiggerTypes[index] = new T * [initialArrayCapacity];
				}

				delete[] arrayCapacity;
				arrayCapacity = newBiggerArrayCapacity;

				delete[] currentArraySize;
				currentArraySize = newBiggerArraySize;

				delete[] pointerToArrayGroup;
				pointerToArrayGroup = newBiggerTypes;

				typesCapacity = newTypesCapacity;
			}
		
		};
	}
}