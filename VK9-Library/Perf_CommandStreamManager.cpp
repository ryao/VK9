/*
Copyright(c) 2018 Christopher Joseph Dean Schaefer

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "Perf_CommandStreamManager.h"

void ProcessQueue(CommandStreamManager* commandStreamManager)
{
	while (commandStreamManager->IsRunning)
	{
		WorkItem workItem;
		if (commandStreamManager->mWorkItems.pop(workItem))
		{
			switch (workItem.WorkItemType)
			{
			case WorkItemType::CreateInstance:
				commandStreamManager->mRenderManager.mStateManager.CreateInstance();
			break;
			default:
				BOOST_LOG_TRIVIAL(error) << "C9::C9 Started.";
				break;
			}
		}
	}
}

CommandStreamManager::CommandStreamManager()
	: mWorkerThread(ProcessQueue, this)
{

}

CommandStreamManager::~CommandStreamManager()
{
	IsRunning = 0;
}

size_t CommandStreamManager::RequestWork(const WorkItem& workItem)
{
	while (!mWorkItems.push(workItem)){}

	//fetching key should be atomic because it's an atomic size_t.
	switch (workItem.WorkItemType)
	{
	case WorkItemType::CreateInstance:
		return mRenderManager.mStateManager.mInstanceKey++;
	default:
		return 0;
	}	
}