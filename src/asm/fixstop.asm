include 'include/ez80.inc'
include 'include/tiformat.inc'
include 'include/ti84pceg.inc'

;include 'include/hook_equates.inc'
;include 'include/capnhook.inc'

;thanks Cesium
;https://github.com/mateoconlechuga/cesium/blob/bfbddaab369c8bc6b0f39dc82ac1afa8bddc22e4/src/hooks.asm#L29

hook_token_stop := $d9 - $ce
hook_parser:
	db	$83			; hook signifier
	cp	a,2
	jr	z,.maybe_stop
	xor	a,a
	ret
.maybe_stop:
	; clear the hook so it doesn't break stuff
	; call ti.ClrParserHook
	ld	a,hook_token_stop	; check if stop token
	cp	a,b
	ld	a,ti.E_Label
	jp	z,ti.JError
	xor	a,a
	ret
hook_parser_size := $-hook_parser

public _InstallFixStopHook
_InstallFixStopHook:
;indentations are now fixed
;why is it jp and not call?
;NOTE: call/ret is the same thing as jp (to remember for later)
	ld	hl,hook_parser
	ld	de,ti.pixelShadow2
	ld	bc,hook_parser_size
	ldir
	ld	hl,ti.pixelShadow2
	jq	ti.SetParserHook


public _ClearFixStopHook
_ClearFixStopHook:
	bit	ti.parserHookActive,(iy + ti.hookflags4)
	jq	ti.ClrParserHook
	ret