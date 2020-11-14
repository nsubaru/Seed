#pragma once
#include "Seed.Core.IO.FS.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace IO 
		{
			namespace FS
			{
				/// <summary>
				/// Інформація про точку обробки
				/// </summary>
				struct RepasePointInformation {
					/// <summary>
					/// Типи точок обробки
					/// </summary>
					enum class ReparsePointType : UInt8 {
						/// <summary>
						/// Посилання
						/// </summary>
						Symlink,
						/// <summary>
						/// Точка зв'язки
						/// </summary>
						Junction,
						/// <summary>
						/// Точка монтування
						/// </summary>
						MountPoint,
						/// <summary>
						/// Дедупліковані дані
						/// </summary>
						Dedup,
						/// <summary>
						/// Невідомий тип
						/// </summary>
						Uknown
					};

					/// <summary>
					/// Тип точок обробки
					/// </summary>
					ReparsePointType Type;

					/// <summary>
					/// Сирі дані точки повторної обробки
					/// </summary>
					Byte ReparsePointData[(16Kb).Bytes];
				};

				/// <summary>
				/// Інформація про файловий об'єкт
				/// </summary>
				struct FSObjectInformation {
					/// <summary>
					/// Тип файлового об'єкту
					/// </summary>
					enum class Types : UInt8 {
						/// <summary>
						/// Файл
						/// </summary>
						File,
						/// <summary>
						/// Директорія
						/// </summary>
						Directory,
						/// <summary>
						/// Не відомий тип
						/// </summary>
						Uknown
					};

					/// <summary>
					/// Тип об'єкта
					/// </summary>
					Types Type;

					/// <summary>
					/// Чи є об'єки точкою повторної обробки
					/// </summary>
					bool IsReparsePoint;

					union {
						struct {
							/// <summary>
							/// Чи архівований
							/// </summary>
							Boolean IsArchived : 1;
							/// <summary>
							/// Чи системний
							/// </summary>
							Boolean IsSystem : 1;
							/// <summary>
							/// Чи схований
							/// </summary>
							Boolean IsHiden : 1;
							/// <summary>
							/// Чи стиснений
							/// </summary>
							Boolean IsCompresed : 1;
							/// <summary>
							/// Чи тільки читання
							/// </summary>
							Boolean IsReadOnly : 1;
							/// <summary>
							/// Чи тимчасовий
							/// </summary>
							Boolean IsTemporary : 1;
						};
						/// <summary>
						/// Флаги об'єкта
						/// </summary>
						UInt8 Flags;
					};
					/// <summary>
					/// Розмір об'єкта, в папок 0
					/// </summary>
					Memory::DataSizeT Size;
					/// <summary>
					/// Ім'я файлового об'єкта
					/// </summary>
					String Name;

					/// <summary>
					/// Конструктор файлового об'єкта
					/// </summary>
					/// <param name="Type">Тип</param>
					/// <param name="IsArchived">Флаг архівованості</param>
					/// <param name="IsSystem">Флаг системності</param>
					/// <param name="IsHiden">Флаг схованості</param>
					/// <param name="IsCompresed">Флан стиснення</param>
					/// <param name="IsReadOnly">Флаг тільки читання</param>
					/// <param name="IsTemporary">Флаг тимчасовості</param>
					/// <param name="Size">Розмір</param>
					/// <param name="Name">Ім'я об'єкта</param>
					FSObjectInformation(
						Types Type,
						Boolean IsReparsePoint,
						Boolean IsArchived,
						Boolean IsSystem,
						Boolean IsHiden,
						Boolean IsCompresed,
						Boolean IsReadOnly,
						Boolean IsTemporary,
						Memory::DataSizeT Size,
						const String& Name
					) :
						Type(Type),
						IsReparsePoint(IsReparsePoint),
						IsArchived(IsArchived),
						IsSystem(IsSystem),
						IsHiden(IsHiden),
						IsCompresed(IsCompresed),
						IsReadOnly(IsReadOnly),
						IsTemporary(IsTemporary),
						Size(Size),
						Name(Name)
					{}

					/// <summary>
					/// Конструктор файлового об'єкта
					/// </summary>
					/// <param name="Type">Тип</param>
					/// <param name="IsArchived">Флаг архівованості</param>
					/// <param name="IsSystem">Флаг системності</param>
					/// <param name="IsHiden">Флаг схованості</param>
					/// <param name="IsCompresed">Флан стиснення</param>
					/// <param name="IsReadOnly">Флаг тільки читання</param>
					/// <param name="IsTemporary">Флаг тимчасовості</param>
					/// <param name="Size">Розмір</param>
					/// <param name="Name">Ім'я об'єкта</param>
					FSObjectInformation(
						Types Type,
						Boolean IsReparsePoint,
						Boolean IsArchived,
						Boolean IsSystem,
						Boolean IsHiden,
						Boolean IsCompresed,
						Boolean IsReadOnly,
						Boolean IsTemporary,
						Memory::DataSizeT Size,
						String&& Name
					) :
						Type(Type),
						IsReparsePoint(IsReparsePoint),
						IsArchived(IsArchived),
						IsSystem(IsSystem),
						IsHiden(IsHiden),
						IsCompresed(IsCompresed),
						IsReadOnly(IsReadOnly),
						IsTemporary(IsTemporary),
						Size(Size),
						Name(MoveRValue(Name))
					{}

					FSObjectInformation() :
						Type(Types::Uknown),
						IsReparsePoint(false),
						IsArchived(false),
						IsSystem(false),
						IsHiden(false),
						IsCompresed(false),
						IsReadOnly(false),
						IsTemporary(false),
						Size(0),
						Name()
					{}

					StructCttiDef(FSObjectInformation,
						DefStructFieldData(FSObjectInformation, Type),
						DefStructFieldData(FSObjectInformation, IsReparsePoint),
						DefStructFieldData(FSObjectInformation, Flags),
						DefStructFieldData(FSObjectInformation, Size),
						DefStructFieldData(FSObjectInformation, Name)
					);
				};


				/// <summary>
				/// Вертає список файлів і папок в заданій директорії
				/// Директорія можу бути задана відносно
				/// </summary>
				/// <param name="Directory">Шлях до директорії</param>
				/// <returns>Список об'єктів в директорії</returns>
				Containers::List<FSObjectInformation> QueryFilesInDirectory(const String& Directory);

				/// <summary>
				/// Вертає список файлів і папок в заданій директорії, фільтруючи їх по масці WilCardPatern
				/// Директорія можу бути задана відносно
				/// </summary>
				/// <param name="Directory">Шлях до директорії</param>
				/// <param name="WilCardPatern">Маска фльтрування</param>
				/// <returns>Список об'єктів в директорії</returns>
				Containers::List<FSObjectInformation> QueryFilesInDirectory(const String& Directory, const String& WilCardPatern);

				/// <summary>
				/// Конвертує стандартне ім'я і адресс файла\папки в Nt вигляд
				/// </summary>
				/// <param name="Win32Name">Win32 ім'я вигляду C:\... або відносний шлях</param>
				/// <returns>NT шлях</returns>
				String GetNtFileName(const String& Win32Name);

				/// <summary>
				/// Об'єкт файла
				/// </summary>
				class File : IDisposable {
				public:
					/// <summary>
					/// Режими відкритя файла
					/// </summary>
					struct OpeningMode {
						union {
							UInt8 Mode;
							struct {
								/// <summary>
								/// Флаг читання
								/// </summary>
								Boolean Read : 1;
								/// <summary>
								/// Флаг запису
								/// </summary>
								Boolean Write : 1;
								/// <summary>
								/// Флаг не буферизованого запису
								/// </summary>
								Boolean WriteThrought : 1;
								/// <summary>
								/// Флаг видалення
								/// </summary>
								Boolean Delete : 1;
							};
						};

						/// <summary>
						/// Оператор об'єднання масок режимів відкритя файла
						/// </summary>
						/// <param name="other"></param>
						/// <returns></returns>
						constexpr OpeningMode operator|(const OpeningMode& other) const {
							return OpeningMode{ static_cast<UInt8>(Mode | other.Mode) };
						}
					};
					/// <summary>
					/// Константа для режиму читання
					/// </summary>
					const static inline OpeningMode ReadMode{ 0B0001ui8 };

					/// <summary>
					/// Константа для режиму запису
					/// </summary>
					const static inline OpeningMode WriteMode{ 0B0010ui8 };

					/// <summary>
					/// Константа для режиму запису без буферизації
					/// </summary>
					const static inline OpeningMode WriteThroughtMode{ 0B0100ui8 };

					/// <summary>
					/// Константа для режиму запису без буферизації
					/// </summary>
					const static inline OpeningMode DeleteMode{ 0B1000ui8 };

					/// <summary>
					/// Політика відкритя\створення файла
					/// </summary>
					enum class OpenDisposition : UInt8 {
						Superseed,
						Create,
						Open,
						OpenIf,
						Overwrite,
						OverwriteIf
					};
				private:
					struct Data {
						Handle handle;
						SizeT readPosition;
						SizeT writePosition;
						String name;
						OpeningMode mode;
						OpenDisposition openPolicy;
						Threading::SRWLock syncLock;

						RefCountT RefCount;

						Data(Handle handle, String&& name, OpeningMode mode, OpenDisposition openPolicy) :
							handle(handle),
							name(MoveRValue(name)),
							mode(mode),
							openPolicy(openPolicy),
							readPosition(0),
							writePosition(0),
							syncLock(),
							RefCount(1)
						{}
					};
				private:
					Data* This;
				protected:
					virtual void Dispose() override;
				public:
					/// <summary>
					/// Створює об'єкт файла і відриває або створює файл в системі
					/// </summary>
					/// <param name="path">Шлях до файла може бути в Nt або Win32 форматі</param>
					/// <param name="mode">Режим файла</param>
					/// <param name="policy">Політика відкритя файла</param>
					/// <param name="ShareWithOs">Флаг шарингу файла з рештою системи</param>
					File(String&& path, OpeningMode mode, OpenDisposition policy, bool ShareWithOs);
					/// <summary>
					/// Створює об'єкт файла і відриває або створює файл в системі
					/// </summary>
					/// <param name="path">Шлях до файла може бути в Nt або Win32 форматі</param>
					/// <param name="mode">Режим файла</param>
					/// <param name="policy">Політика відкритя файла</param>
					/// <param name="ShareWithOs">Флаг шарингу файла з рештою системи</param>
					File(const String& path, OpeningMode mode, OpenDisposition policy, bool ShareWithOs);
				public:
					/// <summary>
					/// Зчитує бінарні дані з файла
					/// </summary>
					/// <param name="Position">Позиція в файлі</param>
					/// <param name="buffer">Масив даних</param>
					void Read(SizeT Position, Containers::Blob& buffer);

					/// <summary>
					/// Запис в файл бінарних даних по позиції
					/// </summary>
					/// <param name="Position">Позиція в файлі</param>
					/// <param name="buffer">Буфер в який запишуться дані</param>
					void Write(SizeT Position, const Containers::Blob& buffer);

					/// <summary>
					/// Зчитує бінарні дані з файла по позиції яку менеджить сам об'єкт
					/// </summary>
					/// <param name="buffer">Буфер в який запишуться дані</param>
					void Read(Containers::Blob& buffer);

					/// <summary>
					/// Запис в файл бінарних даних по позиції яку менеджить сам об'єкт
					/// </summary>
					/// <param name="buffer">Буфер в який запишуться дані</param>
					void Write(const Containers::Blob& buffer);
				public:
					/// <summary>
					/// Скидання даних з кешу даних
					/// </summary>
					void Flush();
				public:
					/// <summary>
					/// Вертає кількість даних у файлі
					/// </summary>
					/// <returns></returns>
					SizeT GetSize() const;
				public:
					/// <summary>
					/// Вертає позицію зчитування
					/// </summary>
					/// <returns></returns>
					SizeT GetReadPosition() const {
						return This->readPosition;
					}

					/// <summary>
					/// Вертає позицію запису
					/// </summary>
					/// <returns></returns>
					SizeT GetWritePosition() const {
						return This->writePosition;
					}
				public:
					/// <summary>
					/// Вертає шлях в NT форматі
					/// </summary>
					/// <returns>Шлях в NT форматі</returns>
					const String& GetPath() const {
						return This->name;
					}
				public:
					/// <summary>
					/// Позиція зчитування
					/// </summary>
					SizeT Property(GetReadPosition) ReadPosition;

					/// <summary>
					/// Позиція запису
					/// </summary>
					SizeT Property(GetWritePosition) WritePosition;

					/// <summary>
					/// Шлях в NT форматі
					/// </summary>
					String Property(GetPath) Path;
				public:
					/// <summary>
					/// Видалення файла
					/// </summary>
					void Delete()&&;
				public:
					RefCountClass(File, This);
				};

				/// <summary>
				/// Вертає true якщо шлях в Nt форматі
				/// </summary>
				/// <param name="FileAdress">Шлях</param>
				/// <returns>true якщо шлях в Nt форматі</returns>
				bool IsNtAdressFormat(const String& FileAdress);

				/// <summary>
				/// Відкриває існуючий файл
				/// </summary>
				/// <param name="path">Шлях в Win32 або Nt форматі</param>
				/// <param name="mode">Режим відкритя</param>
				/// <returns>Об'єкт файла</returns>
				ForceInline static File RawOpenFile(String&& path, File::OpeningMode mode) {
					return File(
						MoveRValue(path),
						mode,
						File::OpenDisposition::Open,
						true);
				}

				/// <summary>
				/// Створює новий файл
				/// </summary>
				/// <param name="path">Шлях в Win32 або Nt форматі</param>
				/// <param name="mode">Режим відкритя</param>
				/// <param name="OverrideIfExist">якщо true то </param>
				/// <returns>Об'єкт файла</returns>
				ForceInline static File RawCreateFile(String&& path, File::OpeningMode mode, bool OverrideIfExist) {
					return File(
						MoveRValue(path),
						mode,
						(OverrideIfExist) ? File::OpenDisposition::OverwriteIf : File::OpenDisposition::Create,
						true);
				}

				/// <summary>
				/// Створює або відкриває файл
				/// </summary>
				/// <param name="path">Шлях в Win32 або Nt форматі</param>
				/// <param name="mode">Режим відкритя</param>
				/// <param name="policy">Політика відритя файла(поведінка при наявності або відсутності файла</param>
				/// <returns>Об'єкт файла</returns>
				ForceInline static File RawCreateFile(String&& path, File::OpeningMode mode, File::OpenDisposition policy) {
					return File(
						MoveRValue(path),
						mode,
						policy,
						true);
				}
			};
		}
	}
}