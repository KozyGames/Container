# Container

***********************************************************************************************************************************************
* SUMMARY
***********************************************************************************************************************************************
* This is a lightweight container that dynamically allocates arrays of dynamic sizes, these grow when the user defined capacity is reached or the user preemptively decides to increase them. 
* It was compiled and tested in the C++14 Standard and upwards compatible.
* The usage of this container is comparable to a wrapper of various vectors, albeit with some differences. 
* Internally, elements are UNSORTED. 
* 
* Eventhough this container was made for a specific use case, I tried to make it as usable as possible for general purposes.
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
* MOSTLY, IT DOES NOT CHECK FOR THE INPUT TO BE VALID. THE USER IS RESPONSIBLE FOR VALID INPUT. ESPECIALLY WHEN THE MACRO _MULTI_DYNAMIC_NO_CHECK_ IS DEFINED
* IF THE USER IS UNSURE, THEY CAN USE FUNCTIONS LIKE: bool contains(T&) TO ENSURE THAT AN ELEMENT IS OR IS NOT ALREADY IN THE CONTAINER;
* AND getElementPosition(T*) TO GET THE CURRENT POSITION INSIDE THE ARRAYGROUP. 
* DELETING AN ELEMENT MAY INVALIDATE POSITIONS OF ELEMENTS IN THE SAME ARRAYGROUP. 
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
* - The type of an element determines to which ArrayGroup they will be assigned.
* 
***********************************************************************************************************************************************
