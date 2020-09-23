#pragma once
#include "Seed.Core.Testing.TestPackResult.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Testing
		{
			class TestSuiteResults final : IDisposable{ //-V1052
			private:
				struct Node {
					Containers::Map<String, Node> Namespaces;
					Containers::Map<String, Testing::TestResult> Tests;
					Time::TimeSpan durration;
					TestResult::EStatus State;
					SizeT TestCount;

					Node() : 
						durration(), 
						Namespaces(), 
						Tests(), 
						State(TestResult::EStatus::Success), 
						TestCount(1) {}

					Node(Time::TimeSpan durration) :
						durration(durration), 
						Namespaces(), 
						Tests(), 
						State(TestResult::EStatus::Success), 
						TestCount(1) {}
					
					Node(const Node& node) : 
						Namespaces(node.Namespaces), 
						Tests(node.Tests), 
						durration(node.durration), 
						State(node.State), 
						TestCount(node.TestCount) {}

					Node(const Node&& node) : 
						Namespaces(MoveRValue(node.Namespaces)), 
						Tests(MoveRValue(node.Tests)), 
						durration(node.durration), 
						State(node.State), 
						TestCount(node.TestCount) {}

					Node& operator=(const Node& node) {
						this->~Node();
						this->Node::Node(node);
						return *this;
					}

					Node& operator=(Node&& node) {
						this->~Node();
						this->Node::Node(MoveRValue(node));
						return *this;
					}
				};
			private:
				struct Data {
					Node Root;
					RefCountT RefCount;
					TestPackResult results;

					Data(const TestPackResult& results) : Root(), results(results), RefCount(1) {};
				};
			private:
				Data* This;
			private:
				virtual void Dispose() override {}
			public:
				TestSuiteResults(const TestPackResult& results);
			public:
				String toNUnitXMlReport();
			private:
				void AddTest(Containers::List<String>&& namespaces, const TestResult& testResult);
			private:
				void genNUnitXmlHeader(String& buffer) const;
				void genNUnitXmlData(String& buffer) const;
				void genNUnitXmlTestSuiteData(String& buffer, const String& name, const Node& node) const;
				void genNUnitXmlTestData(String& buffer, const String& name, const TestResult& testResult) const;
				void gemNUnitXmlFooter(String& buffer) const;
			public:
				RefCountClass(TestSuiteResults, This)
			};
		}
	}
}