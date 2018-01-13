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

#include <thread>
#include <boost/lockfree/queue.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include "Perf_RenderManager.h"

#ifndef COMMANDSTREAMMANAGER_H
#define COMMANDSTREAMMANAGER_H

enum WorkItemType
{
	CreateInstance
};

struct WorkItem
{
	WorkItemType WorkItemType;
	size_t Id;
	void* Argument1;
	void* Argument2;
	void* Argument3;
};

struct CommandStreamManager;

void ProcessQueue(CommandStreamManager* commandStreamManager);

struct CommandStreamManager
{ 	
	std::thread mWorkerThread;
	RenderManager mRenderManager;
	boost::lockfree::queue<WorkItem> mWorkItems;
	bool IsRunning = 1;

	CommandStreamManager();
	~CommandStreamManager();

	size_t RequestWork(const WorkItem& workItem);
};

#endif // COMMANDSTREAMMANAGER_H