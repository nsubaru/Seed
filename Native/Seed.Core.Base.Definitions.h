#pragma once
/// <summary>
/// Не рекомендована функція
/// </summary>
#define DeprecatedCall(Why) __declspec(deprecated(Why))
///<summary>Внутрішній апі, використовувати напряму заборонено</summary>
#define InternalApiCall __declspec(deprecated("Internal API"))

/// <summary>
/// Якщо визначено SeedKernel то всі виклики
/// позначенні SeedKernelCall будуть експортуватись з виконавчого файлу
/// інакше будуть імпортуватись
/// </summary>
#ifdef SeedKernel
///<summary>Системний виклик який експортується з ядра Рем</summary>
#define SeedKernelCall __declspec(dllexport)

///<summary>Сигнатура статичного конструктор змінної</summary>
// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace {
    typedef void (__cdecl* PF)();
}

///<summary>Сигнатура деструктора статичних змінних</summary>
InternalApiCall extern int __cdecl RegisterStaticDtors(PF pf);

#pragma warning(push)
#pragma warning(disable: 4075)
#pragma init_seg(".INIT$m", RegisterStaticDtors)
#pragma comment(linker, "/merge:INIT=.rdata")
#define __INIT__
#pragma warning(pop)

/// <summary>
/// Експортує але в межах ядра замість виклику підставляє тіло функції
/// </summary>
/// <param name = "FuncSig"> Сигнатура функції </param>
/// <param name = "Body"> Тіло функції </param>
#define OptimizeExport(FunctSig, Body) SeedKernelCall __forceinline FunctSig Body

#else
/// <summary>
/// Системний виклик який імпортується з ядра Рем 
/// </summary>
#define SeedKernelCall __declspec(dllimport)
/// <summary>
/// Імпортує як явний виклик
/// </summary>
/// <param name = "FuncSig"> Сигнатура функції </param>
/// <param name = "Body"> Тіло функції </param>
#define OptimizeExport(FunctSig, Body) SeedKernelCall FunctSig;
#endif

#define EXPAND_VA_ARGS(x) x

///<summary>Властивість яка можна лиш читати</summary>
#define __Property1(Getter) __declspec( property( get=Getter) )
///<summary>Властивість яку можна і читати і записувати</summary>
#define __Property2(Getter, Setter) __declspec( property( get=Getter, put=Setter ) )

#define __Property_GET_MACRO(_1,_2,NAME,...) NAME

/// <summary>
/// Властивість яку можна читати або читати якщо дано лиш гетер і записувати якщо є гетер і сетер
/// </summary>
/// <param name = "__VA_ARGS__">Гетер або гетер і сетер</param>
#define Property(...) EXPAND_VA_ARGS(__Property_GET_MACRO(__VA_ARGS__, __Property2, __Property1)(__VA_ARGS__))

/// <summary>
/// Інформує що результат фнкції\методу може бути проігнорована
/// </summary>
#define UnusedData [[maybe_unused]]

/// <summary>
/// Інформує що результат фнкції\методу не можна ігнорувати
/// </summary>
#define NoDiscard [[nodiscard]]

/// <summary>
/// Відступ відносно початку структури до поля
/// </summary>
/// <param name = "s">Тип структури</param>
/// <param name = "memb">Ім'я поля</param>
/// <returns>SizeT число яке вказує який відступ поля у байтах відносно початку структури</returns>
#define StructOffsetOfMember(s,memb) \
    ((Seed::SizeT)((char *)&((s *)0)->memb - (char *)0))

/// <summary>
/// Примусовий вставка коду функцій
/// </summary>
#define forceinline __forceinline

/// <summary>
/// Гарантія компілятору унікальності адресу змінної
/// </summary>
#define restrict __restrict
