using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Runtime.InteropServices.JavaScript.JSType;
using Windows.Storage.Streams;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    Type? TryToGetPredefinedType(RTTI.Record.RttiTypeRecord record)
    {
        if (record.TypeName == Constants.Types.Date)
        {
            return typeof(DateTime);
        }
        else
        {
            return null;
        }
    }

    private bool AddPredefinedStructSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        // Ser -> in stream
        if (typeRecord.TypeName == Constants.Types.Date)
        {
            // Year
            code.AppendLine("var val = (UInt16) data.Year;");
            code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));");

            // Month
            code.AppendLine("val = (UInt16) data.Month;");
            code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));");

            // WeekDay
            code.AppendLine("val = (UInt16) data.DayOfWeek;");
            code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));");

            // Day
            code.AppendLine("val = (UInt16) data.Day;");
            code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));");

            // Hour
            code.AppendLine("val = (UInt16) data.Hour;");
            code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));");

            // Minutes
            code.AppendLine("val = (UInt16) data.Minute;");
            code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));");

            // Second
            code.AppendLine("val = (UInt16) data.Second;");
            code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));");

            // MiliSecond
            code.AppendLine("val = (UInt16) data.Millisecond;");
            code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));");

            return true;
        }
        else
        {
            return false;
        }
    }

    private bool AddPredefinedStructDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        if (typeRecord.TypeName == Constants.Types.Date)
        {
            //Predefs
            code.AppendLine("byte[] buffer = new byte[sizeof(UInt16)];");
            code.AppendLine("UInt16 year = 0;");
            code.AppendLine("UInt16 mounth = 0;");
            code.AppendLine("UInt16 weekDay = 0;");
            code.AppendLine("UInt16 day = 0;");
            code.AppendLine("UInt16 hour = 0;");
            code.AppendLine("UInt16 minutes = 0;");
            code.AppendLine("UInt16 second = 0;");
            code.AppendLine("UInt16 miliSecond = 0;");

            //Year
            code.AppendLine("stream.Read(buffer, 0, sizeof(UInt16));");
            code.AppendLine("year = BitConverter.ToUInt16(buffer, 0);");

            //Mounth
            code.AppendLine("stream.Read(buffer, 0, sizeof(UInt16));");
            code.AppendLine("mounth = BitConverter.ToUInt16(buffer, 0);");

            //WeekDay
            code.AppendLine("stream.Read(buffer, 0, sizeof(UInt16));");
            code.AppendLine("weekDay = BitConverter.ToUInt16(buffer, 0);");

            //Day
            code.AppendLine("stream.Read(buffer, 0, sizeof(UInt16));");
            code.AppendLine("day = BitConverter.ToUInt16(buffer, 0);");

            //Hour
            code.AppendLine("stream.Read(buffer, 0, sizeof(UInt16));");
            code.AppendLine("hour = BitConverter.ToUInt16(buffer, 0);");

            //Minutes
            code.AppendLine("stream.Read(buffer, 0, sizeof(UInt16));");
            code.AppendLine("minutes = BitConverter.ToUInt16(buffer, 0);");

            //Second
            code.AppendLine("stream.Read(buffer, 0, sizeof(UInt16));");
            code.AppendLine("second = BitConverter.ToUInt16(buffer, 0);");

            //MiliSecond
            code.AppendLine("stream.Read(buffer, 0, sizeof(UInt16));");
            code.AppendLine("miliSecond = BitConverter.ToUInt16(buffer, 0);");

            //Create
            code.AppendLine("data = new DateTime(year, mounth, day, hour, minutes, second, miliSecond);");

            return true;
        }
        else
        {
            return false;
        }
    }
}
