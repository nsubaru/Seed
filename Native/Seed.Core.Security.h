#pragma once
//#include "Seed.Core.Base.h"
//#include "Seed.Core.Containers.BitMap.h"
//#include "Seed.Core.Containers.Array.h"
//#include "Seed.Core.Containers.List.h"
//#include "Seed.Core.String.h"
//
//namespace Seed {
//	inline namespace Core {
//		namespace Security {
//			///<summary>Системні права</summary>
//			enum class SystemRights {
//				Login,
//				Logoff,
//
//				TokenCreate,
//				TokenUpdate,
//				TokenRead,
//
//				ReadUserData,
//
//				UserCreate,
//				UserUpdate,
//				UserRead,
//				UserDelete,
//
//				GroupCreate,
//				GroupUpdate,
//				GroupRead,
//				GroupDelete,
//
//				MaxRight
//			};
//			///<summary>Маска системних прав</summary>
//			namespace { using SystemRightsMask = BitMapT<static_cast<SizeT>(SystemRights::MaxRight)>; }
//
//			///<summary>Тип ідентифікатора користувача</summary>
//			namespace { using UserIDType = UUID; }
//			///<summary>Тип ідентифікатора групи</summary>
//			namespace { using GroupIDType = UUID; }
//
//			///<summary>Можливі права <code>ACL</code></summary>
//			enum class ACLAcessEnumaration {
//				ReadACL, ModifyACL, CreateACL, DeleteACL, SetOwner, ReadOwner, MaxRight
//			};
//
//			///<summary>Список прав користувача\або групи<\summary>
//			template<typename RightsEnumaration>
//			class ACE {
//			private:
//				using ACLAcessMask = BitMapT<static_cast<SizeT>(ACLAcessEnumaration::MaxRight)>;
//				using ACLMask = BitMapT<static_cast<SizeT>(RightsEnumaration::MaxRight)>;
//			private:
//				struct IDType {
//					union {
//						UserIDType UserID;
//						GroupIDType GroupID;
//					};
//					Boolean IsUserID;
//				};
//				IDType ID;
//				ACLAcessMask aclAcess;
//				ACLMask acess;
//			public:
//				Boolean GetStateOfIsUserID() { return ID.IsUserID; }
//				UserIDType GetUserID() {
//					if (ID.IsUserID)
//						Throw(InvalidTypeException(__FILE__, __FUNCSIG__, __LINE__, typeid(UserIDType), typeid(GroupIDType)));
//					return ID.UserID;
//				}
//				GroupIDType GetGroupID() {
//					if (!ID.IsUserID)
//						Throw(InvalidTypeException(__FILE__, __FUNCSIG__, __LINE__, typeid(GroupIDType), typeid(UserIDType)));
//					return ID.GroupID;
//				}
//				ACLMask GetAcess() { return acess; }
//				ACLAcessMask GetACLAccess() { return aclAcess; }
//
//				void SetUserID(UserIDType newUser) {
//					if (ID.IsUserID)
//						Throw(InvalidTypeException(__FILE__, __FUNCSIG__, __LINE__, typeid(UserIDType), typeid(GroupIDType)));
//					ID.UserID = newUser;
//				}
//				void SetGroupID(GroupIDType newGroup) {
//					if (!ID.IsUserID)
//						Throw(InvalidTypeException(__FILE__, __FUNCSIG__, __LINE__, typeid(GroupIDType), typeid(UserIDType)));
//					ID.GroupID = newGroup;
//				}
//				void SetAcess(ACLMask newAcess) { acess = newAcess; }
//				void SetACLAcess(ACLAcessMask newACLAcess) { aclAcess = newACLAcess; }
//
//				Boolean Property(GetStateOfIsUserID) IsUserID;
//				UserIDType Property(GetUserID, SetUserID) UserID;
//				GroupIDType Property(GetGroupID, SetGroupID) UserID;
//				ACLMask Property(GetAcess, SetAcess) Acess;
//				ACLAcessMask Property(GetACLAccess, SetACLAcess) AclAcess;
//
//				bool operator==(ACE other) {
//					if (this->ID.IsUserID != other.ID.IsUserID)
//						return false;
//					if (this->ID.IsUserID)
//						if (this->ID.UserID != other.ID.UserID)
//							return false;
//						else;
//					else
//						if (this->ID.GroupID != other.ID.GroupID)
//							return false;
//						else;
//					if (this->aclAcess != other.aclAcess)
//						return false;
//					if (this->acess != other.acess)
//						return false;
//					return true;
//				}
//
//				bool operator!=(ACE other) {
//					if (this->ID.IsUserID != other.ID.IsUserID)
//						return true;
//					if (this->ID.IsUserID)
//						if (this->ID.UserID != other.ID.UserID)
//							return true;
//						else;
//					else
//						if (this->ID.GroupID != other.ID.GroupID)
//							return true;
//						else;
//					if (this->aclAcess != other.aclAcess)
//						return true;
//					if (this->acess != other.acess)
//						return true;
//					return false;
//				}
//			};
//
//			///<summary>Елеемент списку прав доступу</summary>
//			template<typename RightsEnumaration>
//			class ACL {
//			public:
//				UserIDType Owner;
//				GroupIDType GroupOwner;
//
//				List<ACE<RightsEnumaration>> ACLs;
//				List<ACE<RightsEnumaration>> AuditACLs;
//
//				ACL(UserIDType & Owner, GroupIDType & GroupOwner, Array<ACE<RightsEnumaration>> & ACLs, Array<ACE<RightsEnumaration>> & AuditACLs) :
//					Owner(Owner), GroupOwner(GroupOwner), ACLs(ACLs), AuditACLs(AuditACLs)
//				{}
//			};
//
//			interface IUser
//			{
//				SecuredString GetName();
//				SecuredString GetFullName();
//				Array<GroupIDType> GetMemberGroups();
//				Boolean GetState();
//
//				SecuredString Property(GetName) Name;
//				SecuredString Property(GetFullName) FullName;
//				Array<GroupIDType> Property(GetMemberGroups) MemberGroups;
//				Boolean Property(GetState) IsDisabled;
//			};
//
//			interface IGroup
//			{
//				SecuredString GetName();
//				SecuredString GetFullName();
//				Array<GroupIDType> GetMemberGroups();
//				Array<UserIDType> GetMemberUsers();
//				Boolean GetState();
//
//				SecuredString Property(GetName) Name;
//				SecuredString Property(GetFullName) FullName;
//				Array<GroupIDType> Property(GetMemberGroups) MemberGroups;
//				Array<UserIDType> Property(GetMemberGroups) MemberUsers;
//				Boolean Property(GetState) IsDisabled;
//			};
//
//			template<typename RightsEnumaration>
//			interface ISecurityDescriptor
//			{
//				void VerifyRight(
//					ACLAcessEnumaration requiredRight,
//					ConstACharStr file,
//					ConstACharStr functionSignature,
//					SizeT line
//				);
//
//				void VerifyRight(
//					RightsEnumaration requiredRight,
//					ConstACharStr file,
//					ConstACharStr functionSignature,
//					SizeT line
//				);
//
//				void AddACE(ACE<RightsEnumaration> newACE);
//				void SetOwner(UserIDType newOwner);
//				void SetGroupOwner(GroupIDType newGroupOwner);
//				void RemoveACE(ACE<RightsEnumaration> ace);
//				void ModifyACE(ACE<RightsEnumaration> oldAce, ACE<RightsEnumaration> newAce);
//			};
//
//			template<typename RightsEnumaration>
//			class ISecurableImplementation
//			{
//			protected:
//				virtual ISecurityDescriptor<RightsEnumaration> & impGetSecurityDescriptor() = 0;
//			};
//
//			template<typename RightsEnumaration>
//			class ISecurable : protected ISecurableImplementation<RightsEnumaration>
//			{
//			public:
//				virtual ISecurityDescriptor<RightsEnumaration> & GetSecurityDescriptor() final {
//					auto descriptor = impGetSecurityDescriptor();
//					descriptor.VerifyRight(ACLAcessEnumaration::ReadACE, __FILE__, __FUNCSIG__, __LINE__);
//					return descriptor;
//				}
//				ISecurityDescriptor<RightsEnumaration> Property(GetSecurityDescriptor) SecurityDescriptor;
//			};
//
//			template<
//				typename RightsEnumaration,
//				BitMapT<static_cast<SizeT>(RightsEnumaration::MaxRight)>(GenericReadMaskF)(),
//				BitMapT<static_cast<SizeT>(RightsEnumaration::MaxRight)>(GenericWriteMaskF)(),
//				BitMapT<static_cast<SizeT>(RightsEnumaration::MaxRight)>(GenericDeleteMaskF)(),
//				BitMapT<static_cast<SizeT>(RightsEnumaration::MaxRight)>(GenericExecuteMaskF)()
//			>
//				class GenericSecurityDescriptor : public ISecurityDescriptor<RightsEnumaration> {
//				//requires = IsEnum<RightsEnumaration>;
//				public:
//					using RightMap = BitMapT<static_cast<SizeT>(RightsEnumaration::MaxRight)>;
//				public:
//
//					static constexpr RightMap GetGenericReadMask() {
//						return GenericReadMaskF();
//					}
//					static constexpr RightMap GetGenericWriteMask() {
//						return GenericWriteMaskF();
//					}
//					static constexpr RightMap GetGenericDeleteMask() {
//						return GenericDeleteMaskF();
//					}
//					static constexpr RightMap GetGenericExecuteMask() {
//						return GenericExecuteMaskF();
//					}
//				private:
//					static_assert(!IS_CONSTEXPR(GetGenericReadMask()), "GenericReadMask funct non constexpr");
//					static_assert(!IS_CONSTEXPR(GetGenericWriteMask()), "GenericWriteMask funct non constexpr");
//					static_assert(!IS_CONSTEXPR(GetGenericDeleteMask()), "GenericDeleteMask funct non constexpr");
//					static_assert(!IS_CONSTEXPR(GetGenericExecuteMask()), "GenericExecuteMask funct non constexpr");
//				private:
//					ACL<RightsEnumaration> CurrentACL;
//				protected:
//					GenericSecurityDescriptor(const NullptrT) : CurrentACL() {};
//					GenericSecurityDescriptor(const ACL<RightsEnumaration> & CurrentACL) : CurrentACL(CurrentACL) {};
//
//					void VerifyRight(
//						RightsEnumaration requiredRight,
//						ConstACharStr file,
//						ConstACharStr functionSignature,
//						SizeT line
//					);
//
//					ACL<RightsEnumaration> GetACL() {
//						VerifyRight()
//							return CurrentACL;
//					}
//
//					void SetACL(ACL<RightsEnumaration> NewACL) {
//						CurrentACL = NewACl;
//					}
//
//					ACL<RightsEnumaration> Property(GetACL, SetACL) ACL;
//				public:
//					~GenericSecurityDescriptor() {};
//			};
//
//			constexpr auto SystemRightGenericReadMask = [] ()->SystemRightsMask {
//				SystemRightsMask mask;
//				mask.SetBit (static_cast<SizeT>(SystemRights::GroupRead));
//				mask.SetBit (static_cast<SizeT>(SystemRights::ReadUserData));
//				mask.SetBit (static_cast<SizeT>(SystemRights::UserRead));
//				return mask;
//			};
//
//			constexpr auto SystemRightGenericWriteMask = [] ()->SystemRightsMask {
//				SystemRightsMask mask;
//				mask.SetBit (static_cast<SizeT>(SystemRights::GroupUpdate));
//				mask.SetBit (static_cast<SizeT>(SystemRights::UserUpdate));
//				return mask;
//			};
//
//			constexpr auto SystemRightGenericExecuteMask = [] ()->SystemRightsMask {
//				SystemRightsMask mask;
//				mask.SetBit (static_cast<SizeT>(SystemRights::GroupDelete));
//				mask.SetBit (static_cast<SizeT>(SystemRights::UserDelete));
//				return mask;
//			};
//
//			constexpr auto SystemRightGenericDeleteMask = [] ()->SystemRightsMask {
//				SystemRightsMask mask;
//				return mask;
//			};
//
//			using SystemRightSecurityDescriptor = GenericSecurityDescriptor<SystemRights,
//				SystemRightGenericWriteMask, SystemRightGenericWriteMask, SystemRightGenericExecuteMask, SystemRightGenericDeleteMask>;
//
//			Boolean RightCheck(SystemRights requiredRight);
//
//			Boolean Login(SecuredString & userName, Array<Byte> pswdHash);
//			Boolean Logoff();
//
//			UserIDType GetActiveUser();
//
//			Boolean ElevateRight(SystemRights requiredRight);
//			Boolean DisableRight(SystemRights right);
//
//			class AccessDeniedExeption : public Exception {
//			private:
//				UserIDType userID;
//
//				struct RequiredRightT {
//					union
//					{
//						SystemRights systemRight;
//						UInt64 accessRight;
//					};
//					Boolean isAccessRight;
//				};
//
//				RequiredRightT requireRight;
//			public:
//				///<summary>Конструктор помилки</summary>
//				///<param name = 'File'>Файл в якому згенерована помилка</param>
//				///<param name = 'FunctName'>Ім'я функції</param>
//				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
//				AccessDeniedExeption(const AChar * File, const AChar * FunctName, SizeT Line,
//					UserIDType userID, UInt64 accessRight)
//					:Exception(File, FunctName, Line, L"В доступі відмовлено")
//				{
//					this->userID = userID;
//					this->requireRight.isAccessRight = true;
//					this->requireRight.accessRight = accessRight;
//				}
//
//				///<summary>Конструктор помилки</summary>
//				///<param name = 'File'>Файл в якому згенерована помилка</param>
//				///<param name = 'FunctName'>Ім'я функції</param>
//				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
//				AccessDeniedExeption(const AChar * File, const AChar * FunctName, SizeT Line,
//					UserIDType user, SystemRights systemRight)
//					:Exception(File, FunctName, Line, L"В доступі відмовлено")
//				{
//					this->userID = userID;
//					this->requireRight.isAccessRight = false;
//					this->requireRight.systemRight = systemRight;
//				}
//
//				UInt64 GetRequiredAccessRight() {
//					if (!this->requireRight.isAccessRight)
//						return -1;
//					else
//						return this->requireRight.accessRight;
//				}
//
//				SystemRights GetRequiredSystemRight() {
//					if (this->requireRight.isAccessRight)
//						return SystemRights::MaxRight;
//					else
//						return this->requireRight.systemRight;
//				}
//
//				Boolean GetStateOfIsAcessRight() {
//					return this->requireRight.isAccessRight;
//				}
//
//				Boolean Property(GetStateOfIsAcessRight) IsAcessRight;
//				UInt64 Property(GetRequiredAccessRight) RequiredAccessRight;
//				SystemRights Property(GetRequiredSystemRight) RequiredSystemRight;
//			};
//		}
//	}
//}