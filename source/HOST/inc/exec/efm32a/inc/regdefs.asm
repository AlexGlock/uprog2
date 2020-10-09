	//CMU registers
	.equ	CMU_CLKEN0	, 0x40008064
	.equ	CMU_CLKEN1	, 0x40008068
	

	//GPIO registers
	.equ	GPIOA_CTRL		, 0x4003C000
	.equ	GPIOA_MODEL		, 0x4003C004
	.equ	GPIOA_MODEH		, 0x4003C00C
	.equ	GPIOA_DOUT		, 0x4003C010
	.equ	GPIOA_DIN		, 0x4003C014

	.equ	GPIOB_CTRL		, 0x4003C030
	.equ	GPIOB_MODEL		, 0x4003C034
	.equ	GPIOB_MODEH		, 0x4003C03C
	.equ	GPIOB_DOUT		, 0x4003C040
	.equ	GPIOB_DIN		, 0x4003C044

	.equ	GPIOC_CTRL		, 0x4003C060
	.equ	GPIOC_MODEL		, 0x4003C064
	.equ	GPIOC_MODEH		, 0x4003C06C
	.equ	GPIOC_DOUT		, 0x4003C070
	.equ	GPIOC_DIN		, 0x4003C074

	.equ	GPIOD_CTRL		, 0x4003C090
	.equ	GPIOD_MODEL		, 0x4003C094
	.equ	GPIOD_MODEH		, 0x4003C09C
	.equ	GPIOD_DOUT		, 0x4003C0A0
	.equ	GPIOD_DIN		, 0x4003C0A4
	
		
	//MSC registers

	.equ	MSC_BASE		, 0x40030000
	.equ	MSC_WRITECTRL		, 0x0C
	.equ	MSC_WRITECMD		, 0x10
	.equ	MSC_ADDRB		, 0x14
	.equ	MSC_WDATA		, 0x18
	.equ	MSC_STATUS		, 0x1C
	

	