#include "Seed.Core.Testing.Tests.Serializator.Update.ObjectProperties.h"

#include "Seed.Core.IO.h"
#include "Seed.Core.Services.h"

using namespace Seed::Core::Testing::Test::Serializator::Update;

bool ObjectPropertiesTest::Execute() {
    class TestData final : public IDisposable {
    public:
        CttiClassRecord(
            TestData,
            NoCttiInterfaces(),
            NoCttiConstructors(TestData),
            CttiMethodGroup(),
            CttiProperties(
                CttiPropertyRecord(Value1, CttiMethodGroup(CttiMethodRecord(TestData, GetValue1)), CttiMethodGroup()),
                CttiPropertyRecord(Value2, CttiMethodGroup(CttiMethodRecord(TestData, GetValue2)), CttiMethodGroup()),
                CttiPropertyRecord(Value3, CttiMethodGroup(CttiMethodRecord(TestData, GetValue3)), CttiMethodGroup()),
                CttiPropertyRecord(Value4, CttiMethodGroup(CttiMethodRecord(TestData, GetValue4)), CttiMethodGroup()),
                CttiPropertyRecord(Value5, CttiMethodGroup(CttiMethodRecord(TestData, GetValue5)), CttiMethodGroup())
            )
        );

    private:
        struct Data {
            Int32 Value1;
            Boolean Value2;
            String Value3;
            Containers::List<String> Value4;
            Containers::Map<Int32, String> Value5;

            RefCountT RefCount;

            Data(
                Int32 Value1,
                Boolean Value2,
                String Value3,
                Containers::List<String>&& Value4,
                Containers::Map<Int32, String>&& Value5
            ) :
                Value1(Value1),
                Value2(Value2),
                Value3(Value3),
                Value4(Value4),
                Value5(Value5),
                RefCount(1) 
            {}
        };

    private:
        Data* This;

    protected:
        virtual void Dispose() {}

    public:
        TestData(
            Int32 Value1,
            Boolean Value2,
            String&& Value3,
            Containers::List<String>&& Value4,
            Containers::Map<Int32, String>&& Value5
        ) : This(new Data(Value1, Value2, MoveRValue(Value3), MoveRValue(Value4), MoveRValue(Value5))) {}

    public:
        Int32 GetValue1() const { return This->Value1; }
        Boolean GetValue2() const { return This->Value2; }
        String GetValue3() const { return This->Value3; }
        Containers::List<String> GetValue4() const { return This->Value4; }
        Containers::Map<Int32, String> GetValue5() const { return This->Value5; }

    public:
        Int32 Property(GetValue1) Value1;
        Boolean Property(GetValue2) Value2;
        String Property(GetValue3) Value3;
        Containers::List<String> Property(GetValue4) Value4;
        Containers::Map<Int32, String> Property(GetValue5) Value5;

    public:
        Containers::Map<String, RTTI::Dynamic> GetValues() {
            return {
                { nameof(Value1), Value1 },
                { nameof(Value2), Value2 },
                { nameof(Value3), Value3 },
                { nameof(Value4), Value4 },
                { nameof(Value5), Value5 },
            };
        }

    public:
        RefCountClass(TestData);
    };

    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();
    auto object = TestData(10, true, L"test1", { L"test1", L"test2", L"test3" }, { {1, "1"}, {2, "2"}, {3, "3"} });

    auto data = RTTI::ObjectProperties<TestData>(object);
    auto stream = service.Serialize(data);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 0, SourcePosition);

    return true;
}