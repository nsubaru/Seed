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
/// Властивість яку можна читати або читати і записувати якщо дано лиш гетер і записувати якщо є гетер і сетер
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

/// <summary>
/// Стрілочна функція без аргументів
/// </summary>
#define __ArrowFunct1(ret) \
    [](){ return ret; }
/// <summary>
/// Стрілочна функція з одним аргументом
/// </summary>
#define __ArrowFunct2(ret, x1) \
    [](auto x1){ return ret; }
/// <summary>
/// Стрілочна функція з двома аргументами
/// </summary>
#define __ArrowFunct3(ret, x1, x2) \
    [](auto x1, auto x2){ return ret; }
/// <summary>
/// Стрілочна функція з трьома аргументами
/// </summary>
#define __ArrowFunct4(ret, x1, x2, x3) \
    [](auto x1, auto x2, auto x3){ return ret; }
/// <summary>
/// Стрілочна функція з чотирьма аргументами
/// </summary>
#define __ArrowFunct5(ret, x1, x2, x3, x4) \
    [](auto x1, auto x2, auto x3, auto x4){ return ret; }
/// <summary>
/// Стрілочна функція з п'ятьма аргументами
/// </summary>
#define __ArrowFunct6(ret, x1, x2, x3, x4, x5) \
    [](auto x1, auto x2, auto x3, auto x4, auto x5){ return ret; }
/// <summary>
/// Стрілочна функція з шістьма аргументами
/// </summary>
#define __ArrowFunct7(ret, x1, x2, x3, x4, x5, x6) \
    [](auto x1, auto x2, auto x3, auto x4, auto x5, auto x6){ return ret; }
/// <summary>
/// Стрілочна функція з сімньма аргументами
/// </summary>
#define __ArrowFunct8(ret, x1, x2, x3, x4, x5, x6, x7) \
    [](auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7){ return ret; }
/// <summary>
/// Стрілочна функція з вісьма аргументами
/// </summary>
#define __ArrowFunct9(ret, x1, x2, x3, x4, x5, x6, x7, x8) \
    [](auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7, auto x8){ return ret; }
/// <summary>
/// Стрілочна функція з девятьма аргументами
/// </summary>
#define __ArrowFunct10(ret, x1, x2, x3, x4, x5, x6, x7, x8, x9) \
    [](auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7, auto x8, auto x9){ return ret; }
/// <summary>
/// Стрілочна функція з десятьма аргументами
/// </summary>
#define __ArrowFunct11(ret, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) \
    [](auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7, auto x8, auto x9, auto x10){ return ret; }

/// <summary>
/// Вираз для визначення перегрузки макроса
/// </summary>
#define __ArrowFunct_GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11, NAME,...) NAME

/// <summary>
/// Стрілочна функція
/// </summary>
/// <param name = "__VA_ARGS__">Вираз і аргументи</param>
#define ArrowFunct(...) EXPAND_VA_ARGS( \
    __ArrowFunct_GET_MACRO(__VA_ARGS__, \
    __ArrowFunct11,                     \
    __ArrowFunct10,                     \
    __ArrowFunct9,                      \
    __ArrowFunct8,                      \
    __ArrowFunct7,                      \
    __ArrowFunct6,                      \
    __ArrowFunct5,                      \
    __ArrowFunct4,                      \
    __ArrowFunct3,                      \
    __ArrowFunct2,                      \
    __ArrowFunct1)                      \
    (__VA_ARGS__))


/// <summary>
/// Стрілочна функція без аргументів
/// </summary>
#define __ArrowFunctWithClosure1(ret) \
    [&](){ return ret; }
/// <summary>
/// Стрілочна функція з одним аргументом
/// </summary>
#define __ArrowFunctWithClosure2(ret, x1) \
    [&](auto x1){ return ret; }
/// <summary>
/// Стрілочна функція з двома аргументами
/// </summary>
#define __ArrowFunctWithClosure3(ret, x1, x2) \
    [&](auto x1, auto x2){ return ret; }
/// <summary>
/// Стрілочна функція з трьома аргументами
/// </summary>
#define __ArrowFunctWithClosure4(ret, x1, x2, x3) \
    [&](auto x1, auto x2, auto x3){ return ret; }
/// <summary>
/// Стрілочна функція з чотирьма аргументами
/// </summary>
#define __ArrowFunctWithClosure5(ret, x1, x2, x3, x4) \
    [&](auto x1, auto x2, auto x3, auto x4){ return ret; }
/// <summary>
/// Стрілочна функція з п'ятьма аргументами
/// </summary>
#define __ArrowFunctWithClosure6(ret, x1, x2, x3, x4, x5) \
    [&](auto x1, auto x2, auto x3, auto x4, auto x5){ return ret; }
/// <summary>
/// Стрілочна функція з шістьма аргументами
/// </summary>
#define __ArrowFunctWithClosure7(ret, x1, x2, x3, x4, x5, x6) \
    [&](auto x1, auto x2, auto x3, auto x4, auto x5, auto x6){ return ret; }
/// <summary>
/// Стрілочна функція з сімньма аргументами
/// </summary>
#define __ArrowFunctWithClosure8(ret, x1, x2, x3, x4, x5, x6, x7) \
    [&](auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7){ return ret; }
/// <summary>
/// Стрілочна функція з вісьма аргументами
/// </summary>
#define __ArrowFunctWithClosure9(ret, x1, x2, x3, x4, x5, x6, x7, x8) \
    [&](auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7, auto x8){ return ret; }
/// <summary>
/// Стрілочна функція з девятьма аргументами
/// </summary>
#define __ArrowFunctWithClosure10(ret, x1, x2, x3, x4, x5, x6, x7, x8, x9) \
    [&](auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7, auto x8, auto x9){ return ret; }
/// <summary>
/// Стрілочна функція з десятьма аргументами
/// </summary>
#define __ArrowFunctWithClosure11(ret, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) \
    [&](auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7, auto x8, auto x9, auto x10){ return ret; }

/// <summary>
/// Вираз для визначення перегрузки макроса
/// </summary>
#define __ArrowFunctWithClosure_GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11, NAME,...) NAME

/// <summary>
/// Стрілочна функція з захопленням зміних
/// </summary>
/// <param name = "__VA_ARGS__">Вираз і аргументи</param>
#define ArrowFunctWithClosure(...) EXPAND_VA_ARGS( \
    __ArrowFunctWithClosure_GET_MACRO(__VA_ARGS__, \
    __ArrowFunctWithClosure11,                     \
    __ArrowFunctWithClosure10,                     \
    __ArrowFunctWithClosure9,                      \
    __ArrowFunctWithClosure8,                      \
    __ArrowFunctWithClosure7,                      \
    __ArrowFunctWithClosure6,                      \
    __ArrowFunctWithClosure5,                      \
    __ArrowFunctWithClosure4,                      \
    __ArrowFunctWithClosure3,                      \
    __ArrowFunctWithClosure2,                      \
    __ArrowFunctWithClosure1)                      \
    (__VA_ARGS__))
