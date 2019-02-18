#pragma once
#include "Seed.Core.Base.h"

#include "Seed.Core.Security.h"

#include "Seed.Core.Containers.BitMap.h"
#include "Seed.Core.Containers.Array.h"

#include "Seed.Core.String.h"

namespace Seed {
	inline namespace Core {
		namespace Security {
			inline namespace Internal {
				interface IInternalGroup : public IGroup
				{
					SystemRightsMask GetMaxTokenRight();
					SystemRightsMask GetInitialTokenRight();
				};

				interface IInternalUser : public IUser
				{
					SystemRightsMask GetMaxTokenRight();
					SystemRightsMask GetInitialTokenRight();
				};

				class Token : IDisposable {
				private:
					struct data {
						Boolean IsRestriced;
						RefCountT RefCount;
						IInternalUser & user;
						SystemRightsMask CurrentRights, MaxRight, Restriction;

						data(
							IInternalUser & user,
							SystemRightsMask & CurrentRights,
							SystemRightsMask & MaxRight
						) :
							user(user),
							CurrentRights(CurrentRights),
							MaxRight(MaxRight),
							Restriction(),
							IsRestriced(false),
							RefCount(1)
						{};

						data(
							IInternalUser & user,
							SystemRightsMask & CurrentRights,
							SystemRightsMask & MaxRight,
							SystemRightsMask & Restriction
						) :
							user(user),
							CurrentRights(CurrentRights),
							MaxRight(MaxRight),
							Restriction(Restriction),
							IsRestriced(true),
							RefCount(1)
						{};
					};

					data * This;
				public:
					Token(IInternalUser & user) :
						This(new data(user, user.GetInitialTokenRight(), user.GetMaxTokenRight()))
					{}

					Token(IInternalUser & user, SystemRightsMask & Restriction) :
						This(new data(user, user.GetInitialTokenRight(), user.GetMaxTokenRight(), Restriction))
					{}

					RefCountClass(Token, This);

					virtual void Dispose() final;

					SystemRightsMask GetCurrentRights()
					{
						return this->This->CurrentRights;
					}
					Boolean GetStateOfRestriction()
					{
						return this->This->IsRestriced;
					}

					SystemRightsMask Property(GetCurrentRights) CurrentRight;
					Boolean Property(GetStateOfRestriction) IsRestriced;

					Boolean Elevate(SystemRights Right);
					Boolean Disable(SystemRights Right);

					Boolean RightCheck(SystemRights Right);
				};

				interface ISecurityAuthority
				{
					Token Login(const SecuredString & UserName, const Array<Byte> & PswdHash);
					Token RestricedLogin(const SecuredString & UserName, const Array<Byte> & PswdHash, SystemRightsMask Restriction);

					void Logoff(Token && token);

					IInternalUser & GetUserByID(const UserIDType & userID);
					IInternalUser & GetUserByName(const SecuredString & userName);

					IInternalGroup & GetGroupByID(const UserIDType & userID);
					IInternalGroup & GetGroupByName(const SecuredString & userName);

					Boolean AddUserToGroup(UserIDType user, GroupIDType & group);
					Boolean RemoveUserFromGroup(UserIDType user, GroupIDType & group);

					Boolean AddGroupToGroup(GroupIDType & group, GroupIDType & memberGroup);
					Boolean RemoveGroupFromGroup(GroupIDType & group, GroupIDType & memberGroup);

					Boolean CheckAccess(ACL & aceessControlList, IUser & user);

					Boolean IsUserInGroup(IUser & user, IGroup & group);

					Boolean CreateUser(SecuredString & name, SecuredString & fullName, Array<GroupIDType> groups, Array<Byte> HashPswd);
					Boolean CreateGroup(SecuredString & name, SecuredString & fullName, Array<GroupIDType> groups, Array<UserIDType> members);

					Boolean DeleteUser(UserIDType id);
					Boolean DeleteGroup(GroupIDType id);
				};
			}
		}
	}
}