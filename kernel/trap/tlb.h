#define TF_REG0		0
#define TF_REG1		((TF_REG0) + 4)
#define TF_REG2		((TF_REG1) + 4)
#define TF_REG3		((TF_REG2) + 4)
#define TF_REG4		((TF_REG3) + 4)
#define TF_REG5		((TF_REG4) + 4)
#define TF_REG6		((TF_REG5) + 4)
#define TF_REG7		((TF_REG6) + 4)
#define TF_REG8		((TF_REG7) + 4)
#define TF_REG9		((TF_REG8) + 4)
#define TF_REG10	((TF_REG9) + 4)
#define TF_REG11	((TF_REG10) + 4)
#define TF_REG12	((TF_REG11) + 4)
#define TF_REG13	((TF_REG12) + 4)
#define TF_REG14	((TF_REG13) + 4)
#define TF_REG15	((TF_REG14) + 4)
#define TF_REG16	((TF_REG15) + 4)
#define TF_REG17	((TF_REG16) + 4)
#define TF_REG18	((TF_REG17) + 4)
#define TF_REG19	((TF_REG18) + 4)
#define TF_REG20	((TF_REG19) + 4)
#define TF_REG21	((TF_REG20) + 4)
#define TF_REG22	((TF_REG21) + 4)
#define TF_REG23	((TF_REG22) + 4)
#define TF_REG24	((TF_REG23) + 4)
#define TF_REG25	((TF_REG24) + 4)
/*
 * $26 (k0) and $27 (k1) not saved
 */
#define TF_REG26	((TF_REG25) + 4)
#define TF_REG27	((TF_REG26) + 4)
#define TF_REG28	((TF_REG27) + 4)
#define TF_REG29	((TF_REG28) + 4)
#define TF_REG30	((TF_REG29) + 4)
#define TF_REG31	((TF_REG30) + 4)

#define TF_HI		((TF_REG31) + 4)
#define TF_LO		((TF_HI) + 4)

/*
 * Size of stack frame, word/double word alignment
 */
#define TF_SIZE		((TF_LO)+4)


.set noat
.macro TLB_SAVE_TF
    move k0, sp
    subu	sp,sp, TF_SIZE
    sw	k0,TF_REG29(sp)                  
    sw	v0,TF_REG2(sp)                              
    mfhi	v0                               
    sw	v0,TF_HI(sp)                     
    mflo	v0                               
    sw	v0,TF_LO(sp)                     
    sw	$0,TF_REG0(sp)
    sw	$1,TF_REG1(sp)                    
    //sw	$2,TF_REG2(sp)                   
    sw	$3,TF_REG3(sp)                   
    sw	$4,TF_REG4(sp)                   
    sw	$5,TF_REG5(sp)                   
    sw	$6,TF_REG6(sp)                   
    sw	$7,TF_REG7(sp)                   
    sw	$8,TF_REG8(sp)                   
    sw	$9,TF_REG9(sp)                   
    sw	$10,TF_REG10(sp)                 
    sw	$11,TF_REG11(sp)                 
    sw	$12,TF_REG12(sp)                 
    sw	$13,TF_REG13(sp)                 
    sw	$14,TF_REG14(sp)                 
    sw	$15,TF_REG15(sp)                 
    sw	$16,TF_REG16(sp)                 
    sw	$17,TF_REG17(sp)                 
    sw	$18,TF_REG18(sp)                 
    sw	$19,TF_REG19(sp)                 
    sw	$20,TF_REG20(sp)                 
    sw	$21,TF_REG21(sp)                 
    sw	$22,TF_REG22(sp)                 
    sw	$23,TF_REG23(sp)                 
    sw	$24,TF_REG24(sp)                 
    sw	$25,TF_REG25(sp)                 
    sw	$26,TF_REG26(sp) 				 
    sw	$27,TF_REG27(sp) 				 
    sw	$28,TF_REG28(sp)                 
    sw	$30,TF_REG30(sp)                 
    sw	$31,TF_REG31(sp)
.endm

.macro TLB_RESTORE_TF                                  		 
    lw	v1,TF_LO(sp)                                       
    mtlo	v1                               
    lw	v0,TF_HI(sp)                                       
    mthi	v0                                                     
    lw	$31,TF_REG31(sp)                 
    lw	$30,TF_REG30(sp)                 
    lw	$28,TF_REG28(sp)                 
    lw	$25,TF_REG25(sp)                 
    lw	$24,TF_REG24(sp)                 
    lw	$23,TF_REG23(sp)                 
    lw	$22,TF_REG22(sp)                 
    lw	$21,TF_REG21(sp)                 
    lw	$20,TF_REG20(sp)                 
    lw	$19,TF_REG19(sp)                 
    lw	$18,TF_REG18(sp)                 
    lw	$17,TF_REG17(sp)                 
    lw	$16,TF_REG16(sp)                 
    lw	$15,TF_REG15(sp)                 
    lw	$14,TF_REG14(sp)                 
    lw	$13,TF_REG13(sp)                 
    lw	$12,TF_REG12(sp)                 
    lw	$11,TF_REG11(sp)                 
    lw	$10,TF_REG10(sp)                 
    lw	$9,TF_REG9(sp)                   
    lw	$8,TF_REG8(sp)                   
    lw	$7,TF_REG7(sp)                   
    lw	$6,TF_REG6(sp)                   
    lw	$5,TF_REG5(sp)                   
    lw	$4,TF_REG4(sp)                   
    lw	$3,TF_REG3(sp)                   
    lw	$2,TF_REG2(sp)                   
    lw	$1,TF_REG1(sp)           

    lw	sp,TF_REG29(sp) 
    // addiu	sp,sp, TF_SIZE        
.endm

.macro STI
	mfc0	k0,	CP0_STATUS
	ori	k0, k0, 0x1
	mtc0	k0, CP0_STATUS
.endm