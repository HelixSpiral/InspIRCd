/*       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  InspIRCd: (C) 2002-2008 InspIRCd Development Team
 * See: http://www.inspircd.org/wiki/index.php/Credits
 *
 * This program is free but copyrighted software; see
 *            the file COPYING for details.
 *
 * ---------------------------------------------------
 */

#include "inspircd.h"

/* $ModDesc: Provides support for unreal-style channel mode +Q */

class NoKicks : public SimpleChannelModeHandler
{
 public:
	NoKicks(InspIRCd* Instance) : SimpleChannelModeHandler(Instance, 'Q') { }
};

class ModuleNoKicks : public Module
{

	NoKicks* nk;

 public:

	ModuleNoKicks(InspIRCd* Me)
		: Module(Me)
	{

		nk = new NoKicks(ServerInstance);
		if (!ServerInstance->Modes->AddMode(nk))
			throw ModuleException("Could not add new modes!");
		Implementation eventlist[] = { I_OnAccessCheck };
		ServerInstance->Modules->Attach(eventlist, this, 1);
	}


	virtual int OnAccessCheck(User* source,User* dest,Channel* channel,int access_type)
	{
		if (access_type == AC_KICK)
		{
			if (channel->IsModeSet('Q'))
			{
				if ((ServerInstance->ULine(source->nick.c_str())) || (ServerInstance->ULine(source->server)) || (!*source->server))
				{
					// ulines can still kick with +Q in place
					return ACR_ALLOW;
				}
				else
				{
					// nobody else can (not even opers with override, and founders)
					source->WriteNumeric(ERR_CHANOPRIVSNEEDED, "%s %s :Can't kick user %s from channel (+Q set)",source->nick.c_str(), channel->name.c_str(), dest->nick.c_str());
					return ACR_DENY;
				}
			}
		}
		return ACR_DEFAULT;
	}

	virtual ~ModuleNoKicks()
	{
		ServerInstance->Modes->DelMode(nk);
		delete nk;
	}

	virtual Version GetVersion()
	{
		return Version(1,2,0,0,VF_COMMON|VF_VENDOR,API_VERSION);
	}
};


MODULE_INIT(ModuleNoKicks)
