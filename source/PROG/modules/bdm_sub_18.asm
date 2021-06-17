;---------------------------------------------------------------------
; generated file for frequency = 18 MHz
; Send:
; SIZE-0 = 4 clocks
; SIZE-D = 10 clocks
; SIZE-1 = 5 clocks
; Receive:
; SIZE-0 = 4 clocks
; SIZE-S = 7 clocks
; SIZE-W = 8 clocks
;---------------------------------------------------------------------
.org (pc + 255) & 0xff00
bdms18_send8:		ldi	r20,8
			mov	XH,XL
			rjmp	bdms18_send16_1

.org (pc & 0xff00) + BDM_SUB_SWORD
bdms18_send16:		ldi	r20,16
bdms18_send16_1:	sbi	CTRLDDR,SIG2
			out	CTRLPORT,r0
			nop
			nop
			sbrc	XH,7
			out	CTRLPORT,r1
			ldi	r21,3
bdms18_send16_3:	dec	r21
			brne	bdms18_send16_3
			out	CTRLPORT,r1
			cbi	CTRLDDR,SIG2
			nop
			lsl	XL
			rol	XH
			dec	r20
			brne	bdms18_send16_1
			ret

.org (pc & 0xff00) + BDM_SUB_RBYTE
bdms18_recv8:		ldi	r20,8
			rjmp	bdms18_recv16_1

.org (pc & 0xff00) + BDM_SUB_RWORD
bdms18_recv16:		ldi	r20,16
bdms18_recv16_1:	sbi	CTRLDDR,SIG2
			out	CTRLPORT,r0
			ldi	r21,1
bdms18_recv16_2:	dec	r21
			brne	bdms18_recv16_2
			cbi	CTRLDDR,SIG2
			out	CTRLPORT,r1
			nop
			nop
			lsl	XL
			rol	XH
			sbic	CTRLPIN,SIG2
			inc	XL
			ldi	r21,1
bdms18_recv16_4:	dec	r21
			brne	bdms18_recv16_4
			nop
			nop
			dec	r20
			brne	bdms18_recv16_1
			ret

.org (pc & 0xff00) + BDM_SUB_WAIT16
bdms18_wait16:		
			ldi	r21,3
bdms18_wait16_1:	dec	r21
			brne	bdms18_wait16_1
			nop
			ret

.org (pc & 0xff00) + BDM_SUB_WAIT160
bdms18_wait160:		
			ldi	r21,60
bdms18_wait160_1:	dec	r21
			brne	bdms18_wait160_1
			ret

.org (pc & 0xff00) + BDM_SUB_BREAD8
bdms18_bread8:		ldi	XL,0xe0
			rcall	bdms18_send8
			movw	XL,r24
			rcall	bdms18_send16
			call	bdm_wait2_ack
			rcall	bdms18_wait16
			rcall	bdms18_recv8
			call	api_buf_bwrite
			adiw	r24,1
			ret

.org (pc & 0xff00) + BDM_SUB_BWRITE8
bdms18_bwrite8:		ldi	XL,0xc0
			rcall	bdms18_send8
			movw	XL,r24
			rcall	bdms18_send16
			call	api_buf_bread
			rcall	bdms18_send8
			call	bdm_wait2_ack
			rcall	bdms18_wait16
			adiw	r24,1
			ret

.org (pc & 0xff00) + BDM_SUB_BREADF8
bdms18_breadf8:		ldi	XL,0xe0
			rcall	bdms18_send8
			movw	XL,r22
			rcall	bdms18_send16
			call	bdm_wait2_ack
			rcall	bdms18_wait16
			rjmp	bdms18_recv8

.org (pc & 0xff00) + BDM_SUB_BWRITEF8
			push	XL
bdms18_bwritef8:	ldi	XL,0xc0
			rcall	bdms18_send8
			movw	XL,r22
			rcall	bdms18_send16
			pop	XL
			rcall	bdms18_send8
			call	bdm_wait2_ack
			rjmp	bdms18_wait16

.org (pc & 0xff00) + BDM_SUB_BREADF16
bdms18_breadf16:	ldi	XL,0xe0
bdms18_breadf16a:	
			rcall	bdms18_send8
			movw	XL,r22
			rcall	bdms18_send16
			rcall	bdms18_wait160
			rcall	bdms18_recv16
			sbrs	r22,0
			mov	XL,XH
			ret

.org (pc & 0xff00) + BDM_SUB_BWRITEF16
			push	XL
bdms18_bwritef16:	ldi	XL,0xc0
bdms18_bwritef16a:	
			rcall	bdms18_send8
			movw	XL,r22
			rcall	bdms18_send16
			pop	XL
			mov	XH,XL
			rcall	bdms18_send16
			rcall	bdms18_wait160
			ret

.org (pc & 0xff00) + BDM_SUB_WREAD
bdms18_wread:		ldi	XL,0xe8
			rcall	bdms18_send8
			movw	XL,r24
			rcall	bdms18_send16
			rcall	bdms18_wait160
			rcall	bdms18_recv16
			call	api_buf_mwrite
			adiw	r24,2
			ret

.org (pc & 0xff00) + BDM_SUB_WWRITE
bdms18_wwrite:		ldi	XL,0xc8
			rcall	bdms18_send8
			movw	XL,r24
			rcall	bdms18_send16
			call	api_buf_mread
			rcall	bdms18_send16
			rcall	bdms18_wait160
			adiw	r24,2
			ret

.org (pc & 0xff00) + BDM_SUB_WREADF
bdms18_wreadf:		ldi	XL,0xe8
			rcall	bdms18_send8
			movw	XL,r22
			rcall	bdms18_send16
			rcall	bdms18_wait160
			rcall	bdms18_recv16
			ret

.org (pc & 0xff00) + BDM_SUB_WWRITEF
bdms18_wwritef:		movw	r18,XL
			ldi	XL,0xc8
			rcall	bdms18_send8
			movw	XL,r22
			rcall	bdms18_send16
			movw	XL,r18
			rcall	bdms18_send16
			rjmp	bdms18_wait160

.org (pc & 0xff00) + BDM_SUB_RSTAT
bdms18_bstat16:		ldi	XL,0xe4
			ldi	r22,0x01
			ldi	r23,0xff
			rjmp	bdms18_breadf16a

.org (pc & 0xff00) + BDM_SUB_WSTAT
			push	XL
bdms18_wstat16:		ldi	XL,0xc4
			ldi	r22,0x01
			ldi	r23,0xff
			rjmp	bdms18_bwritef16a


