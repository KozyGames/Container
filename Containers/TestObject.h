#pragma once
#include <string>
namespace Kozy
{
	namespace Containers
	{
		class TestObject
		{
		public:
			TestObject(int randomInteger);
			~TestObject();
			bool operator==(const TestObject& obj) const;
			bool operator!=(const TestObject& obj) const;

			bool isSameGroup(const TestObject& obj) const;

			unsigned int getID() const;

			std::string toStr() const;
		private:
			unsigned int ID;
			int mockField;

		};

		void testMulti_DynamicArray();
	}
}