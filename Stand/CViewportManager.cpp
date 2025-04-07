#include "CViewportManager.hpp"

#include "fwRenderThreadInterface.hpp"
#include "pointers.hpp"

const rage::grcViewport* CViewportManager::GetCurrentGameGrcViewport() const
{
	return &m_cachedGameGrcViewport[Stand::pointers::render_thread_interface->GetCurrentBuffer()];
}
