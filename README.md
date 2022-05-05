# Container
PLEASE DO NOT USE THIS PROJECT YET. Although it works as intended, it lacks comments and in the end does not free all memory that it allocates.
I am self-taught and wrote this project in my freetime. 

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
