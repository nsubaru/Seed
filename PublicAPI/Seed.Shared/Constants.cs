namespace Seed.Serialization;

public static partial class Constants
{
    public static class Types
    {
        public const string Boolean = "Seed::Core::Boolean";

        public const string Int8 = "Seed::Core::Int8";
        public const string Int16 = "Seed::Core::Int16";
        public const string Int32 = "Seed::Core::Int32";
        public const string Int64 = "Seed::Core::Int64";

        public const string UInt8 = "Seed::Core::UInt8";
        public const string UInt16 = "Seed::Core::UInt16";
        public const string UInt32 = "Seed::Core::UInt32";
        public const string UInt64 = "Seed::Core::UInt64";

        public const string Float = "Seed::Core::Float";
        public const string Double = "Seed::Core::Double";

        public const string SizeT = "Seed::Core::SizeT";
        public const string DataSizeT = "Seed::Core::Memory::DataSizeT";
        public const string Id = "Seed::Core::ID";
        public const string RefCountT = "Seed::Core::RefCountT";

        public const string UChar = "Seed::Core::UChar";
        public const string AChar = "Seed::Core::AChar";

        public const string U8Char = "Seed::Core::U8Char";
        public const string U16Char = "Seed::Core::U16Char";
        public const string U32Char = "Seed::Core::U32Char";

        public const string Uuid = "Seed::Core::UUID";

        public const string EmptyAttributeValueT = "Seed::Core::CTTI::TypeTraits::EmptyAttributeValueT";

        public const string EnumFlag = @"Seed::Core::EnumFlag";
        public const string EnumBitFlag = @"Seed::Core::EnumBitFlags";

        public const string TypeMetaDataRecord = "Seed::Core::Serialization::TypeMetaDataRecord";
        public const string SensitiveTypeMetaDataRecord = "Seed::Core::Serialization::SensitiveTypeMetaDataRecord";
        public const string RTTISection = "Seed::Core::RTTI::RTTISection";

        public const string String = "Seed::Core::String";
        public const string SensitiveString = "Seed::Core::SensitiveString";
        public const string ExceptionString = "Seed::Core::ExceptionString";

        public const string UniString = "Seed::Core::UniString";
        public const string SensitiveUniString = "Seed::Core::SensitiveUniString";
        public const string ExceptionUniString = "Seed::Core::ExceptionUniString";

        public const string Blob = "Seed::Core::Containers::Blob";
        public const string BaseBlob = "Seed::Core::Containers::BaseBlob";
        public const string SensitiveBlob = "Seed::Core::Containers::SensitiveBlob";

        public const string BaseArray = "Seed::Core::Containers::BaseArray";
        public const string BaseList = "Seed::Core::Containers::BaseList";

        public const string List = "Seed::Core::Containers::List";
        public const string Array = "Seed::Core::Containers::Array";
        public const string DynamicStack = "Seed::Core::Containers::DynamicStack";
        public const string DynamicQueue = "Seed::Core::Containers::DynamicQueue";

        public const string SensitiveList = "Seed::Core::Containers::SensitiveList";
        public const string SensitiveArray = "Seed::Core::Containers::SensitiveArray";
        public const string SensitiveDynamicStack = "Seed::Core::Containers::SensitiveDynamicStack";
        public const string SensitiveDynamicQueue = "Seed::Core::Containers::SensitiveDynamicQueue";

        public const string ConcurrencyList = "Seed::Core::Containers::ConcurrencyList";
        public const string ConcurrencyArray = "Seed::Core::Containers::ConcurrencyArray";
        public const string ConcurrencyDynamicStack = "Seed::Core::Containers::ConcurrencyDynamicStack";
        public const string ConcurrencyDynamicQueue = "Seed::Core::Containers::ConcurrencyDynamicQueue";

        public const string ConcurrencySensitiveList = "Seed::Core::Containers::ConcurrencySensitiveList";
        public const string ConcurrencySensitiveArray = "Seed::Core::Containers::ConcurrencySensitiveArray";
        public const string ConcurrencySensitiveDynamicStack = "Seed::Core::Containers::ConcurrencySensitiveDynamicStack";
        public const string ConcurrencySensitiveDynamicQueue = "Seed::Core::Containers::ConcurrencySensitiveDynamicQueue";

        public const string FixedArray = "Seed::Core::Containers::FixedArray";
        public const string FixedQueue = "Seed::Core::Containers::FixedQueue";
        public const string FixedStack = "Seed::Core::Containers::FixedStack";

        public const string SensitiveFixedArray = "Seed::Core::Containers::SensitiveFixedArray";
        public const string SensitiveFixedQueue = "Seed::Core::Containers::SensitiveFixedQueue";
        public const string SensitiveFixedStack = "Seed::Core::Containers::SensitiveFixedStack";

        public const string ConcurrencyFixedArray = "Seed::Core::Containers::ConcurrencyFixedArray";
        public const string ConcurrencyFixedQueue = "Seed::Core::Containers::ConcurrencyFixedQueue";
        public const string ConcurrencyFixedStack = "Seed::Core::Containers::ConcurrencyFixedStack";

        public const string ConcurrencySensitiveFixedArray = "Seed::Core::Containers::ConcurrencySensitiveFixedArray";
        public const string ConcurrencySensitiveFixedQueue = "Seed::Core::Containers::ConcurrencySensitiveFixedQueue";
        public const string ConcurrencySensitiveFixedStack = "Seed::Core::Containers::ConcurrencySensitiveFixedStack";

        public const string Pair = "Seed::Core::Containers::Pair";
        public const string SensitivePair = "Seed::Core::Containers::SensitivePair";

        public const string Map = "Seed::Core::Containers::Map";
        public const string Dictionary = "Seed::Core::Containers::Dictionary";

        public const string SensitiveMap = "Seed::Core::Containers::SensitiveMap";
        public const string SensitiveDictionary = "Seed::Core::Containers::SensitiveDictionary";

        public const string ConcurrencyMap = "Seed::Core::Containers::ConcurrencyMap";
        public const string ConcurrencyDictionary = "Seed::Core::Containers::ConcurrencyDictionary";

        public const string ConcurrencySensitiveMap = "Seed::Core::Containers::ConcurrencySensitiveMap";
        public const string ConcurrencySensitiveDictionary = "Seed::Core::Containers::ConcurrencySensitiveDictionary";

        public const string Tuple = "Seed::Core::Containers::Tuple";

        public const string SmartPtr = "Seed::Core::Containers::SmartPtr";
        public const string SensitiveSmartPtr = "Seed::Core::Containers::SensitiveSmartPtr";

        public const string AtomicSmartPtr = "Seed::Core::Containers::AtomicSmartPtr";
        public const string AtomicSensitiveSmartPtr = "Seed::Core::Containers::AtomicSensitiveSmartPtr";

        public const string Optional = "Seed::Core::Functionality::Optional";
        public const string Result = "Seed::Core::Functionality::Result";
        public const string Union = "Seed::Core::Functionality::Union";
        public const string Either = "Seed::Core::Functionality::Either";

        public const string ObjectProperties = "Seed::Core::RTTI::ObjectProperties";

        public const string Date = "Seed::Core::Time::Date";
    }

    public static partial class DerCodeBlocks
    {
        public const string DeserializationMethodName = "DeserializeImpl";

        public const string Prolog = @"public {FriendlyTypeName} {DeserializationMethodName}({FriendlyTypeName} data, Stream stream) { logger?.LogTrace(""Deserialize type {FriendlyTypeName}"");";
        public const string Epilog = @"}";

        public const string Int8 = @"data = unchecked((sbyte) (byte) stream.ReadByte());";
        public const string Int16 = @"byte[] buffer = new byte[sizeof(Int16)]; stream.Read(buffer, 0, sizeof(Int16)); data = BitConverter.ToInt16(buffer, 0);";
        public const string Int32 = @"byte[] buffer = new byte[sizeof(Int32)]; stream.Read(buffer, 0, sizeof(Int32)); data = BitConverter.ToInt32(buffer, 0);";
        public const string Int64 = @"byte[] buffer = new byte[sizeof(Int64)]; stream.Read(buffer, 0, sizeof(Int64)); data = BitConverter.ToInt64(buffer, 0);";

        public const string UInt8 = @"data = (byte) stream.ReadByte();";
        public const string UInt16 = @"byte[] buffer = new byte[sizeof(UInt16)]; stream.Read(buffer, 0, sizeof(UInt16)); data = BitConverter.ToUInt16(buffer, 0);";
        public const string UInt32 = @"byte[] buffer = new byte[sizeof(UInt32)]; stream.Read(buffer, 0, sizeof(UInt32)); data = BitConverter.ToUInt32(buffer, 0);";
        public const string UInt64 = @"byte[] buffer = new byte[sizeof(UInt64)]; stream.Read(buffer, 0, sizeof(UInt64)); data = BitConverter.ToUInt64(buffer, 0);";

        public const string Float = @"byte[] buffer = new byte[sizeof(float)]; stream.Read(buffer, 0, sizeof(float)); data = BitConverter.ToSingle(buffer, 0);";
        public const string Double = @"byte[] buffer = new byte[sizeof(double)]; stream.Read(buffer, 0, sizeof(double)); data = BitConverter.ToDouble(buffer, 0);";

        public const string UUID = @"byte[] rawData = new byte[16]; stream.Read(rawData, 0, 16); data = new Guid(rawData);";

        public const string Boolean = @"byte[] buffer = new byte[sizeof(bool)]; stream.Read(buffer, 0, sizeof(bool)); data = BitConverter.ToBoolean(buffer, 0);";

        public const string UChar = @"byte[] buffer = new byte[sizeof(char)]; stream.Read(buffer, 0, sizeof(char)); data = BitConverter.ToChar(buffer, 0);";
        public const string AChar = @"byte[] buffer = new byte[sizeof(byte)]; stream.Read(buffer, 0, sizeof(byte)); data = buffer[0];";

        public const string U8Char = @"byte[] buffer = new byte[sizeof(byte)]; stream.Read(buffer, 0, sizeof(byte)); data = buffer[0];";
        public const string U16Char = @"byte[] buffer = new byte[sizeof(char)]; stream.Read(buffer, 0, sizeof(char)); data = BitConverter.ToChar(buffer, 0);";
        public const string U32Char = @"byte[] buffer = new byte[sizeof(UInt32)]; stream.Read(buffer, 0, sizeof(UInt32)); data = BitConverter.ToUInt32(buffer, 0);";

        public const string String = @"var strSize = (UInt64) 0; 
strSize = {DeserializationMethodName}(strSize, stream); 
if (strSize == 0) return CreateObject<{FriendlyTypeName}>();
strSize -= 2;
byte[] buffer = new byte[strSize];
stream.Read(buffer, 0, (int) strSize);
data = Encoding.Unicode.GetString(buffer);
stream.Seek(2, SeekOrigin.Current);";

        public const string SensitiveString = @"var strSize = (UInt64) 0; 
strSize = {DeserializationMethodName}(strSize, stream); 
if (strSize == 0) return CreateObject<{FriendlyTypeName}>();
strSize -= 2;
byte[] buffer = new byte[strSize];
stream.Read(buffer, 0, (int) strSize);
var plain = Encoding.Unicode.GetString(buffer);
data = plain.ToCharArray().Aggregate(new SecureString(), (s, c) => {s.AppendChar(c); return s; }, (s) => {s.MakeReadOnly(); return s; });
stream.Seek(2, SeekOrigin.Current);";

        public const string TypeMetaDataRecord = $@"var testMetaData = string.Empty; testMetaData = {DeserializationMethodName}(testMetaData, stream); data = new TypeMetaDataDecoder().DecodeTypeMetaData(testMetaData);";
        public const string RTTISection = $@"var bytesCount = (UInt64) 0; 
bytesCount = {DeserializationMethodName}(bytesCount, stream); 
byte[] bytes = new byte[bytesCount];
stream.Read(bytes, 0, (int) bytesCount);
data = new RTTI.RTTISection(bytes);";

        public const string Collections = $@"for (UInt64 i = 0; i < count; i++){{
var item = CreateObject<$FriendlyTypeName>();
item = {DeserializationMethodName}(item, stream);
data.Add(item);}}";

        public const string ListCollections = @"for (UInt64 i = 0; i < count; i++){
var item = CreateObject<{FriendlyTypeName}>();
item = {DeserializationMethodName}(item, stream);
data.AddLast(item);}";

        public const string AssociativeCollections = @"UInt64 count = 0;
count = {DeserializationMethodName}(count, stream);
data = CreateObject<{PairFriendlyTypeName}>();
for (UInt64 i = 0; i < count; i++){
var item = new KeyValuePair<{KeyFriendlyTypeName}, {ValueFriendlyTypeName}>();
item = {DeserializationMethodName}(item, stream);
data.Add(item.Key, item.Value);}";

        public const string FixedCollections = @"for (int i = 0; i < {FixedCollectionLenght}; i++) data[i] = {DeserializationMethodName}(data[i], stream);";
        public const string FixedArray = @"for (int i = 0; i < {FixedCollectionLenght}; i++) data[i] = {DeserializationMethodName}(data[i], stream);";

        public const string Pair = @"var key = data.Key; key = {DeserializationMethodName}(key, stream); var value = data.Value; value = {DeserializationMethodName}(value, stream); data = new {PairFriendlyTypeName}(key, value);";

        public const string Box = @"var value = data.Value; if (value != null) value = {DeserializationMethodName}(value!, stream);";
        public const string Either = @"UInt64 idx = 0ul;
idx = {DeserializationMethodName}(idx, stream);
if (idx == 0) {
var value = CreateObject<{LeftTypeFriendlyTypeName}>();
data = {FriendlyTypeName}.Left({DeserializationMethodName}(value, stream));
} else {
var value = CreateObject<{RightTypeFriendlyTypeName}>();
data = {FriendlyTypeName}.Right({DeserializationMethodName}(value, stream));
}";

        public const string StructItem = @"data.{field} = {DeserializationMethodName}(data.{field}, stream);";
        public const string TupleItem = @"data.{field} = {DeserializationMethodName}(data.{field}, stream);";
        public const string ObjectPropertiesItem = @"data.{field} = {DeserializationMethodName}(data.{field}, stream);";

        public const string EnumFlag = @"var value = data.AsInteger(); value = {DeserializationMethodName}(value, stream); data =  new {FriendlyTypeName} (value);";

        public const string EmptyAttributeValueT = @"byte[] buffer = new byte[sizeof(byte)]; stream.Read(buffer, 0, sizeof(byte));";
    }

    public static partial class SerCodeBlocks
    {
        public const string SerializationMethodName = "SerializeImpl";

        public const string Prolog = @"public void {SerializationMethodName}({FriendlyTypeName} data, Stream stream) { logger?.LogTrace(\""Serialize type {FriendlyTypeName}\"");""";
        public const string Epilog = @"}";

        public const string Int8 = @"stream.WriteByte(unchecked((byte) (sbyte) data));";
        public const string Int16 = @"var val = (Int16) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(Int16));";
        public const string Int32 = @"var val = (Int32) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(Int32));";
        public const string Int64 = @"var val = (Int64) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(Int64));";

        public const string UInt8 = @"stream.WriteByte((byte) data);";
        public const string UInt16 = @"var val = (UInt16) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));";
        public const string UInt32 = @"var val = (UInt32) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt32));";
        public const string UInt64 = @"var val = (UInt64) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt64));";

        public const string Float = @"var val = (float) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(float));";
        public const string Double = @"var val = (double) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(double));";

        public const string UUID = @"var val = (Guid)data; stream.Write(val.ToByteArray(), 0, 16);";

        public const string Boolean = @"bool val = (bool) data; stream.Write(BitConverter.GetBytes(val), 0, 1);";

        public const string UChar = @"var val = (char) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(char));";
        public const string AChar = @"var val = (byte) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(byte));";

        public const string U8Char = @"var val = (byte) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(byte));";
        public const string U16Char = @"var val = (char) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(char));";
        public const string U32Char = @"var val = (UInt32) data; stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt32));";

        public const string String = @"var bytes = Encoding.Unicode.GetBytes(data); 
{SerializationMethodName}(((ulong)data.Length + 1) * 2, stream); 
stream.Write(bytes, 0, bytes.Length); 
stream.Write(Encoding.Unicode.GetBytes(new char[] { '\0' }), 0, 1 * sizeof(char));";

        public const string SensitiveString = @"char[] chars;
var ptr = IntPtr.Zero;
try{
ptr = Marshal.SecureStringToBSTR(data);
chars = new char[data.Length];
Marshal.Copy(ptr, chars, 0, data.Length);}
finally{
if (ptr != IntPtr.Zero){
Marshal.ZeroFreeBSTR(ptr);}}
var bytes = Encoding.Unicode.GetBytes(chars);
{SerializationMethodName}(((ulong)data.Length + 1) * 2, stream);
stream.Write(bytes, 0, bytes.Length);
stream.Write(Encoding.Unicode.GetBytes(new char[] { '\0' }), 0, 1 * sizeof(char));";

        public const string TypeMetaDataRecord = $@"string serializationText = data.ToString(""S""); {SerializationMethodName}(serializationText, stream);";
        public const string RTTISection = $@"var bytes = data.Bytes; {SerializationMethodName}(bytes, stream);";

        public const string Collections = @"var sz = (ulong) data.LongCount(); {SerializationMethodName}(sz, stream); foreach(var item in data) {SerializationMethodName}(item, stream);";

        public const string AssociativeCollections = @"var sz = (ulong) data.LongCount(); {SerializationMethodName}(sz, stream); foreach (var item in data) {SerializationMethodName}(item, stream);";

        public const string FixedCollections = @"foreach (var item in data) {SerializationMethodName} (item, stream);";
        public const string FixedArray = @"foreach (var item in data) {SerializationMethodName} (item, stream);";

        public const string Pair = @"var key = data.Key; var value = data.Value; {SerializationMethodName}(key, stream); {SerializationMethodName}(value, stream);";

        public const string Box = @"{SerializationMethodName}(data.Value, stream);";
        public const string Either = @"UInt64 idx = data.IsLeft ? 0ul : 1ul; {SerializationMethodName}(idx, stream); data.Match(x => {SerializationMethodName}(x, stream), x => {SerializationMethodName}(x, stream));";

        public const string StructItem = @"{SerializationMethodName}(data.{field}, stream);";
        public const string TupleItem = @"{SerializationMethodName}(data.{field}, stream);";
        public const string ObjectPropertiesItem = @"{SerializationMethodName}(data.{field}, stream);";

        public const string EnumFlag = @"var value = data.AsInteger(); {SerializationMethodName}(value, stream);";

        public const string EmptyAttributeValueT = @"var val = (byte) 0; stream.WriteByte(val);";
    }

    public const string CreateObjectMethodBody = @"private T CreateObject<T>()
    {
        Type type = typeof(T);

        object? obj = null;

        if (type == typeof(String))
        {
            obj = string.Empty;
        }
        else
        { 
            obj = !typeof(T).GetConstructors().Any(x => x.GetParameters().Length == 0)
                ? (T)RuntimeHelpers.GetUninitializedObject(typeof(T))
                : Activator.CreateInstance(typeof(T)); 

            foreach(var field in obj.GetType().GetFields())
            {
                // Skip all field except public non static rw
                if (field.IsStatic || field.IsPrivate && field.IsInitOnly || field.IsLiteral)
                    continue;

                if (field.FieldType.GetConstructors().Any(x => x.GetParameters().Length == 0) ||
                    field.FieldType.GetConstructors().Length == 0)
                {
                    field.SetValue(obj, Activator.CreateInstance(field.FieldType));
                }
            }
        }

        return (T)obj;
    }

    public object CreateObjectByTypeRecord(TypeRecord typeRecord) {
        return GetType()
            .GetRuntimeMethods()
            .Where(x => x.Name == nameof(CreateObject))
            .First()
            .MakeGenericMethod(typeRecord.ClrType)
            .Invoke(this, new object[] {});
    }";
}
