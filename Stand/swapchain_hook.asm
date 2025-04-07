.DATA
	present_detour			dq	0
	present_jumpout			dq	0
	resize_buffers_og_call	dq	0
	resize_buffers_detour	dq	0
	resize_buffers_jumpout	dq	0

.CODE
	swapchain_hook_init_present PROC
		mov present_detour, rcx
		mov present_jumpout, rdx
		ret
	swapchain_hook_init_present ENDP

	swapchain_hook_init_resize_buffers PROC
		mov resize_buffers_og_call, rcx
		mov resize_buffers_detour, rdx
		mov resize_buffers_jumpout, r8
		ret
	swapchain_hook_init_resize_buffers ENDP

	swapchain_hook_present PROC
		mov rax, present_detour
		call rax
		mov rcx, rax
		mov rax, present_jumpout
		jmp rax
	swapchain_hook_present ENDP

	swapchain_hook_resize_buffers PROC
		mov rax, resize_buffers_og_call
		call rax
		mov rcx, rax
		mov rax, resize_buffers_detour
		call rax
		mov r10, resize_buffers_jumpout
		jmp r10
	swapchain_hook_resize_buffers ENDP
END
