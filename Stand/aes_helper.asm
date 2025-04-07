.DATA
	rage_AES_TransformITDecrypt		dq	0
	aes_initfile_cont				dq	0

	rage_AES_Decrypt				dq	0
	aes_decrypt_callsite_cont		dq	0

.CODE
	aes_initfile_init PROC
		mov rage_AES_TransformITDecrypt, rcx
		mov aes_initfile_cont, rdx
		ret
	aes_initfile_init ENDP

	aes_decrypt_init PROC
		mov rage_AES_Decrypt, rcx
		mov aes_decrypt_callsite_cont, rdx
		ret
	aes_decrypt_init ENDP

	aes_initfile_detour PROC
		cmp ecx, 4E45504Fh ; OpenIV-unencrypted key id; 'OPEN' in little-endian
		je _initfile_cont
		mov rdx, [rbx+20h]
		mov r8d, edi
		mov ecx, r14d
		mov rax, rage_AES_TransformITDecrypt
		call rax
	_initfile_cont:
		mov rax, aes_initfile_cont
		jmp rax
	aes_initfile_detour ENDP

	aes_decrypt_callsite_detour PROC
		cmp ecx, 4E45504Fh ; OpenIV-unencrypted key id; 'OPEN' in little-endian
		je _decrypt_cont
		mov rax, rage_AES_Decrypt
		call rax
	_decrypt_cont:
		mov rax, aes_decrypt_callsite_cont
		jmp rax
	aes_decrypt_callsite_detour ENDP
END
