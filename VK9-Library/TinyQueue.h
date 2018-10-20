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

#include <atomic>

#ifndef TINYQUEUE_H
#define TINYQUEUE_H

#define TINY_QUEUE_MAX_SIZE 14

template <class ElementType>
class TinyQueue
{
private:
	std::atomic_flag mLock = ATOMIC_FLAG_INIT;
	char mHeadIndex = 0;
	char mTailIndex = 0;
	char mCount = 0;
	char mIsFull = 0;
	ElementType* mData[TINY_QUEUE_MAX_SIZE] = {};

	bool IsEmpty()
	{
		return (!mIsFull && (mHeadIndex == mTailIndex));
	}

public:

	TinyQueue()
	{

	}

	~TinyQueue()
	{

	}

	bool Push(ElementType* item, size_t& count)
	{
		bool result = false;

		while (mLock.test_and_set(std::memory_order_acquire));

		if (!mIsFull)
		{
			result = true;
			mData[mHeadIndex] = item;

			mHeadIndex = (mHeadIndex + 1) % TINY_QUEUE_MAX_SIZE;

			mIsFull = mHeadIndex == mTailIndex;

			mCount++;
		}

		count = mCount;

		mLock.clear(std::memory_order_release);

		return result;
	}

	bool Pop(ElementType*& item, size_t& count)
	{
		bool result = false;

		while (mLock.test_and_set(std::memory_order_acquire));

		if (!IsEmpty())
		{
			result = true;
			item = mData[mTailIndex];
			mIsFull = false;
			mTailIndex = (mTailIndex + 1) % TINY_QUEUE_MAX_SIZE;

			mCount--;
		}

		count = mCount;

		mLock.clear(std::memory_order_release);

		return result;
	}

};

#endif // TINYQUEUE_H