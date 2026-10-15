using System;
using System.Collections.Generic;
using System.Linq;

namespace Seed.XMLDocGen;

internal partial class XdcParser
{
    public static readonly Dictionary<string, string> nameReplaceTable = new()
    {
        {
            "op_Subscript",
            "operator[]"
        },
        {
            "#ctor",
            "ctor"
        },
        {
            "<unknown type>",
            "implementation specific"
        },
        {
            "op_Implicit~",
            "operator "
        },
        {
            "=FUNC:",
            "=FunctionPtr!"
        },
        {
            "op_Assign",
            "operator="
        },
        {
            "op_Addition",
            "operator+"
        },
        {
            "op_Subtraction",
            "operator-"
        },
        {
            "op_AdditionAssignment",
            "operator+="
        },
        {
            "op_SubtractionAssignment",
            "operator-="
        },
        {
            "op_Decrement",
            "operator--"
        },
        {
            "op_Increment",
            "operator++"
        },
        {
            "op_Equality",
            "operator=="
        },
        {
            "op_GreaterThan",
            "operator>"
        },
        {
            "op_GreaterThanOrEqual",
            "operator>="
        },
        {
            "op_Inequality",
            "operator!="
        },
        {
            "op_LessThan",
            "operator<"
        },
        {
            "op_LessThanOrEqual",
            "operator<="
        },
        {
            "op_FunctionCall",
            "operator()"
        },
        {
            "<=>",
            "operator<=>"
        },
        {
            "System.Void",
            "void"
        },
        {
            "System.SByte",
            "signed char"
        },
        {
            "System.Byte",
            "char"
        },
    };
    public static readonly Dictionary<string, string> typeReplaceTable = new()
    {
        {
            "{",
            "<"
        },
        {
            "}",
            ">"
        },
    };
    private static readonly Dictionary<string, string> signToUnSignTable = new()
    {
        {
            "System.SByte",
            "System.Byte"
        },
        {
            "signed char",
            "char"
        }
    };

    /// <summary>
    /// Parse item name depending on name prefix
    /// </summary>
    /// <param name="name">Name with prefix</param>
    /// <returns>Parsed name</returns>
    public static string ParseName(string name)
    {
        switch (name[0])
        {
            case 'M':
                return ParseMethodName(name[2..]);
            case 'P':
                return ParsePropertyName(name[2..]);
            case 'T':
                return ParseTypeName(name[2..]);
            case 'F':
                return ParseFieldName(name[2..]);
            default:
                Console.WriteLine($"Invalid type of member: {name}");
                return name;
        }
    }

    /// <summary>
    /// Parse method name without prefix
    /// </summary>
    /// <param name="name">Method name without prefix</param>
    /// <returns>Parsed name</returns>
    public static string ParseMethodName(string name)
    {
        foreach (var replaceItem in nameReplaceTable)
        {
            name = name.Replace(replaceItem.Key, replaceItem.Value);
        }

        if (name.IndexOf('(') == -1)
        {
            name += "()";

            return name;
        }

        var methodArgs       = name.Substring(name.IndexOf('(') + 1, name.IndexOf(')') - name.IndexOf('(') - 1);
        var methodArg        = methodArgs.Split(',');
        var parsedMethodName = name.Substring(0, name.IndexOf('(') + 1);

        foreach (var arg in methodArg)
        {
            var argPart = new List<string>(arg.Split('!'));
            switch (argPart.Count)
            {
                case 1:
                {
                    parsedMethodName += ($"{argPart[0]}, ");
                    continue;
                }

                case 2:
                {
                    if (argPart[0] == "=FunctionPtr")
                    {
                        parsedMethodName += $"{argPart[1]}(*)(), ";
                    }

                    if (argPart[1].Contains("System.Runtime.CompilerServices.IsImplicitlyDereferenced"))
                    {
                        if (argPart[0].Last() == '*')
                        {
                            parsedMethodName += string.Concat(argPart[0].AsSpan(0, argPart[0].Length - 1), "&, ");
                        }
                        else
                        {
                            parsedMethodName += ($"{argPart[0]}&, ");
                        }

                        continue;
                    }

                    if (argPart[1].Contains("System.Runtime.CompilerServices.IsConst*"))
                    {
                        parsedMethodName += ($"const {argPart[0]} *, ");
                        continue;
                    }

                    if (argPart[1].Contains("System.Runtime.CompilerServices.IsVolatile*"))
                    {
                        parsedMethodName += ($"volatile {argPart[0]} *, ");
                        continue;
                    }

                    if (argPart[1].Contains("System.Runtime.CompilerServices.IsConst"))
                    {
                        parsedMethodName += ($"const {argPart[0]}");
                        continue;
                    }

                    if (argPart[1].Contains("System.Runtime.CompilerServices.IsVolatile"))
                    {
                        parsedMethodName += ($"volatile {argPart[0]}");
                        continue;
                    }

                    if (argPart[1].Contains("System.Runtime.CompilerServices.IsSignUnspecifiedByte"))
                    {
                        parsedMethodName += ($"{signToUnSignTable[argPart[0]]}*, ");
                    }

                    continue;
                }

                case 3:
                {
                    if (argPart[1].Contains("System.Runtime.CompilerServices.IsSignUnspecifiedByte"))
                    {
                        if (argPart[2].Contains("System.Runtime.CompilerServices.IsConst*"))
                        {
                            parsedMethodName += ($"const {signToUnSignTable[argPart[0]]}*, ");
                        }
                        else if (argPart[2].Contains("System.Runtime.CompilerServices.IsVolatile*"))
                        {
                            parsedMethodName += ($"volatile {signToUnSignTable[argPart[0]]}*, ");
                        }
                        else
                        {
                            parsedMethodName += ($"{signToUnSignTable[argPart[0]]}*, ");
                        }

                        continue;
                    }

                    if (argPart[1].Contains("System.Runtime.CompilerServices.IsConst*"))
                    {
                        if (argPart[2].Contains("System.Runtime.CompilerServices.IsImplicitlyDereferenced"))
                        {
                            parsedMethodName += ($"const {argPart[0]} &, ");
                        }

                        continue;
                    }

                    if (argPart[1].Contains("System.Runtime.CompilerServices.IsVolatile*"))
                    {
                        if (argPart[2].Contains("System.Runtime.CompilerServices.IsImplicitlyDereferenced"))
                        {
                            parsedMethodName += ($"volatile {argPart[0]} &, ");
                        }

                        continue;
                    }

                    continue;
                }

                default:
                {
                    parsedMethodName += ($"{argPart[0]}, ");
                    continue;
                }
            }
        }

        if (parsedMethodName.EndsWith("("))
        {
            return parsedMethodName.Substring(parsedMethodName.Length - 1);
        }
        else
        {
            return string.Concat(parsedMethodName.AsSpan(0, parsedMethodName.Length - 2), ")");
        }
    }

    /// <summary>
    /// Parse property name
    /// </summary>
    /// <param name="name">Property name without prefix</param>
    /// <returns>Parsed name which is same as name param</returns>
    public static string ParsePropertyName(string name) => name;

    /// <summary>
    /// Parse type name
    /// </summary>
    /// <param name="name">Type name without prefix</param>
    /// <returns>Parsed name</returns>
    public static string ParseTypeName(string name)
    {
        foreach (var replaceItem in typeReplaceTable)
        {
            name = name.Replace(replaceItem.Key, replaceItem.Value);
        }

        return name;
    }

    /// <summary>
    /// Parse field name
    /// </summary>
    /// <param name="name">Field name without prefix</param>
    /// <returns>Parsed name which is same as name param</returns>
    public static string ParseFieldName(string name) => name;
}
