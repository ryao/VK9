//========= Copyright Valve Corporation, All rights reserved. ============//
//                       TOGL CODE LICENSE
//
//  Copyright 2011-2014 Valve Corporation
//  All Rights Reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
// dxabstract.h
//
//==================================================================================================

/*
 * Code from ToGL has been modified to fit the design.
 */
 
#include "COpenGLQuery9.h"

COpenGLQuery9::COpenGLQuery9()
{
	
}

COpenGLQuery9::~COpenGLQuery9()
{
	
}

HRESULT COpenGLQuery9::GetData(void* pData, DWORD dwSize, DWORD dwGetDataFlags)
{
	GL_BATCH_PERF_CALL_TIMER;
	Assert( m_device->m_nValidMarker == D3D_DEVICE_VALID_MARKER );
	HRESULT	result = S_FALSE ;
	DWORD nCurThreadId = ThreadGetCurrentId();

	// Make sure calling thread owns the GL context.
	Assert( m_ctx->m_nCurOwnerThreadId == nCurThreadId );
	if ( pData )
	{
		*(uint*)pData = 0;
	}

	if ( !m_query->IsStarted() || !m_query->IsStopped() ) 
	{
		Assert(!"Can't GetData before issue/start/stop");
		printf("\n** IDirect3DQuery9::GetData: can't GetData before issue/start/stop");
		return S_FALSE;
	}
					
	// GetData is not always called with the flush bit.
		
	// if an answer is not yet available - return S_FALSE.
	// if an answer is available - return S_OK and write the answer into *pData.
	bool done = false;
	bool flush = (dwGetDataFlags & D3DGETDATA_FLUSH) != 0;	// aka spin until done

	// hmmm both of these paths are the same, maybe we could fold them up
		if ( m_type == D3DQUERYTYPE_OCCLUSION )
		{
			// Detect cases that are actually just not supported with the way we're using GL queries. (For example, beginning a query, then creating/deleting any query, the ending the same query is not supported.)
			// Also extra paranoid to detect/work around various NV/AMD driver issues.
			if ( ( ( m_nIssueStartThreadID != nCurThreadId ) || ( m_nIssueEndThreadID != nCurThreadId ) ) ||
				 ( m_nIssueStartDrawCallIndex == m_nIssueEndDrawCallIndex ) || ( m_nIssueStartFrameIndex != m_nIssueEndFrameIndex ) ||
				 ( m_nIssueStartQueryCreationCounter != m_nIssueEndQueryCreationCounter ) )
			{
				// The thread Issue() was called on differs from GetData() - NV's driver doesn't like this, not sure about AMD. Just fake the results if a flush is requested.
				// There are various ways to properly handle this scenario, but in practice it only seems to occur in non-critical times (during shutdown or when mat_queue_mode is changed in L4D2).
				if ( flush )
				{
					gGL->glFlush();
				}

#if 0
				if ( ( m_nIssueStartThreadID != nCurThreadId ) || ( m_nIssueEndThreadID != nCurThreadId ) )
				{
					GLMDebugPrintf( "IDirect3DQuery9::GetData: GetData() called from different thread verses the issueing thread()!\n" );
				}
#endif
				if ( m_nIssueStartQueryCreationCounter != m_nIssueEndQueryCreationCounter )
				{
					GLMDebugPrintf( "IDirect3DQuery9::GetData: One or more queries have been created or released while this query was still issued! This scenario is not supported in GL.\n");
				}

				// Return with a non-standard error code, so the caller has a chance to do something halfway intelligent.
				return D3DERR_NOTAVAILABLE;
			}
		}
		
		switch( m_type )
		{
			case	D3DQUERYTYPE_OCCLUSION:
			{
				
			if ( flush )
				{
				uint oqValue = 0;
					CFastTimer tm;
					tm.Start();
										

						// Is this flush actually necessary? According to the extension it's not.
						// It doesn't seem to matter if this is a glFlush() or glFinish() with NVidia's driver (tested in MT mode - not sure if it matters), it still can take several calls to IsDone() before we can stop waiting for the query results.
						// On AMD, this flush logic fails during shutdown (the query results never become available) - tried a bunch of experiments and checks with no luck.
																						
				m_query->Complete(&oqValue);

											
				double flTotalTime = tm.GetDurationInProgress().GetSeconds() * 1000.0f;
				if ( flTotalTime > .5f )
						{
							// Give up - something silly has obviously gone wrong in the driver, lying is better than stalling potentially forever.
							// This occurs on AMD (single threaded driver) during shutdown, not sure why yet. It has nothing to do with threading. It may have to do with releasing queries or other objects.
							// We must return a result otherwise the app itself could hang, waiting infinitely.
							//Assert( 0 );
					Warning( "IDirect3DQuery9::GetData(): Occlusion query flush took %3.3fms!\n", flTotalTime );
						}
				if (pData)
				{
					*(uint*)pData = oqValue;
					}
				result = S_OK;
				}
			else
			{
				done = m_query->IsDone();
				if (done)
				{
					uint oqValue = 0;				// or we could just pass pData directly to Complete...
					m_query->Complete(&oqValue);
					if (pData)
					{
						*(uint*)pData = oqValue;
					}					
					result = S_OK;
				}
				else
				{
					result = S_FALSE;
					Assert( !flush );
				}
				}
			}
			break;

			case	D3DQUERYTYPE_EVENT:
			{
				done = m_query->IsDone();
				if ( ( done ) || ( flush ) )
				{
					m_query->Complete(NULL);	// this will block on pre-SLGU
					
					result = S_OK;
				}
				else
				{
					result = S_FALSE;
					Assert( !flush );
				}
			}
			break;
	}

	return result;	
}

/*
DWORD COpenGLQuery9::GetDataSize()
{
	
}

HRESULT COpenGLQuery9::GetDevice(IDirect3DDevice9** pDevice)
{
	
}

D3DQUERYTYPE COpenGLQuery9::GetType()
{
	
}
*/

HRESULT COpenGLQuery9::Issue(DWORD dwIssueFlags)
{
	GL_BATCH_PERF_CALL_TIMER;
	Assert( m_device->m_nValidMarker == D3D_DEVICE_VALID_MARKER );
	// Flags field for Issue
	//	#define D3DISSUE_END (1 << 0) // Tells the runtime to issue the end of a query, changing it's state to "non-signaled".
	//	#define D3DISSUE_BEGIN (1 << 1) // Tells the runtime to issue the beginng of a query.

	// Make sure calling thread owns the GL context.
	Assert( m_ctx->m_nCurOwnerThreadId == ThreadGetCurrentId() );
		
	if (dwIssueFlags & D3DISSUE_BEGIN)
	{
		m_nIssueStartThreadID = ThreadGetCurrentId();
		m_nIssueStartDrawCallIndex = g_nTotalDrawsOrClears;
		m_nIssueStartFrameIndex = m_ctx->m_nCurFrame;
		m_nIssueStartQueryCreationCounter = CGLMQuery::s_nTotalOcclusionQueryCreatesOrDeletes;
						
		switch( m_type )
		{
			case	D3DQUERYTYPE_OCCLUSION:
				m_query->Start();	// drop "start counter" call into stream
			break;

			default:
				Assert(!"Can't use D3DISSUE_BEGIN on this query");
			break;
		}
	}
	
	if (dwIssueFlags & D3DISSUE_END)
	{
		m_nIssueEndThreadID = ThreadGetCurrentId();
		m_nIssueEndDrawCallIndex = g_nTotalDrawsOrClears;
		m_nIssueEndFrameIndex = m_ctx->m_nCurFrame;
		m_nIssueEndQueryCreationCounter = CGLMQuery::s_nTotalOcclusionQueryCreatesOrDeletes;
		
		switch( m_type )
		{
			case	D3DQUERYTYPE_OCCLUSION:
				m_query->Stop();	// drop "end counter" call into stream
			break;
			
			case	D3DQUERYTYPE_EVENT:
				m_nIssueStartThreadID = m_nIssueEndThreadID;
				m_nIssueStartDrawCallIndex = m_nIssueEndDrawCallIndex;
				m_nIssueStartFrameIndex = m_nIssueEndFrameIndex;
				m_nIssueStartQueryCreationCounter = m_nIssueEndQueryCreationCounter;
				
				// End is very weird with respect to Events (fences).
				// DX9 docs say to use End to put the fence in the stream.  So we map End to GLM's Start.
				// http://msdn.microsoft.com/en-us/library/ee422167(VS.85).aspx
				m_query->Start();	// drop "set fence" into stream
			break;
		}
	}
	
	return S_OK;	
}
