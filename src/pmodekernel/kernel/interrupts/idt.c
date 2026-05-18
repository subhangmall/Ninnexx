#include <stdint.h>
#include "../logging.h"
#define NULL ((void*)0)


#pragma pack(push, 1) 

struct InterruptDescriptor {
    uint16_t offset;
    uint16_t segSelector;
    uint8_t reserved;
    uint8_t gateType : 4;
    uint8_t zero : 1;
    uint8_t dpl : 2;
    uint8_t present : 1; 
    uint16_t offset2;
};

struct IdtrStruct {
    uint16_t size;
    uint32_t idtVAddr;
};

#pragma pack(pop)

struct IdtrStruct idtr __attribute__((aligned(16)));
struct InterruptDescriptor idt[256] __attribute__((aligned(16)));
void (*interruptRequestHandlers[256])(uint32_t*);
extern void int0(void);
extern void int1(void);
extern void int2(void);
extern void int3(void);
extern void int4(void);
extern void int5(void);
extern void int6(void);
extern void int7(void);
extern void int8(void);
extern void int9(void);
extern void intA(void);
extern void intB(void);
extern void intC(void);
extern void intD(void);
extern void intE(void); // SKIP F
extern void int10(void);
extern void int11(void);
extern void int12(void);
extern void int13(void);
extern void int14(void);
extern void int15(void);
extern void int16(void);
extern void int17(void);
extern void int18(void);
extern void int19(void);
extern void int1A(void);
extern void int1B(void);
extern void int1C(void);
extern void int1D(void);
extern void int1E(void);
extern void int1F(void);
extern void int20(void);
extern void int21(void);
extern void int22(void);
extern void int23(void);
extern void int24(void);
extern void int25(void);
extern void int26(void);
extern void int27(void);
extern void int28(void);
extern void int29(void);
extern void int2A(void);
extern void int2B(void);
extern void int2C(void);
extern void int2D(void);
extern void int2E(void);
extern void int2F(void);
extern void int30(void);
extern void int31(void);
extern void int32(void);
extern void int33(void);
extern void int34(void);
extern void int35(void);
extern void int36(void);
extern void int37(void);
extern void int38(void);
extern void int39(void);
extern void int3A(void);
extern void int3B(void);
extern void int3C(void);
extern void int3D(void);
extern void int3E(void);
extern void int3F(void);
extern void int40(void);
extern void int41(void);
extern void int42(void);
extern void int43(void);
extern void int44(void);
extern void int45(void);
extern void int46(void);
extern void int47(void);
extern void int48(void);
extern void int49(void);
extern void int4A(void);
extern void int4B(void);
extern void int4C(void);
extern void int4D(void);
extern void int4E(void);
extern void int4F(void);
extern void int50(void);
extern void int51(void);
extern void int52(void);
extern void int53(void);
extern void int54(void);
extern void int55(void);
extern void int56(void);
extern void int57(void);
extern void int58(void);
extern void int59(void);
extern void int5A(void);
extern void int5B(void);
extern void int5C(void);
extern void int5D(void);
extern void int5E(void);
extern void int5F(void);
extern void int60(void);
extern void int61(void);
extern void int62(void);
extern void int63(void);
extern void int64(void);
extern void int65(void);
extern void int66(void);
extern void int67(void);
extern void int68(void);
extern void int69(void);
extern void int6A(void);
extern void int6B(void);
extern void int6C(void);
extern void int6D(void);
extern void int6E(void);
extern void int6F(void);
extern void int70(void);
extern void int71(void);
extern void int72(void);
extern void int73(void);
extern void int74(void);
extern void int75(void);
extern void int76(void);
extern void int77(void);
extern void int78(void);
extern void int79(void);
extern void int7A(void);
extern void int7B(void);
extern void int7C(void);
extern void int7D(void);
extern void int7E(void);
extern void int7F(void);
extern void int80(void);
extern void int81(void);
extern void int82(void);
extern void int83(void);
extern void int84(void);
extern void int85(void);
extern void int86(void);
extern void int87(void);
extern void int88(void);
extern void int89(void);
extern void int8A(void);
extern void int8B(void);
extern void int8C(void);
extern void int8D(void);
extern void int8E(void);
extern void int8F(void);
extern void int90(void);
extern void int91(void);
extern void int92(void);
extern void int93(void);
extern void int94(void);
extern void int95(void);
extern void int96(void);
extern void int97(void);
extern void int98(void);
extern void int99(void);
extern void int9A(void);
extern void int9B(void);
extern void int9C(void);
extern void int9D(void);
extern void int9E(void);
extern void int9F(void);
extern void intA0(void);
extern void intA1(void);
extern void intA2(void);
extern void intA3(void);
extern void intA4(void);
extern void intA5(void);
extern void intA6(void);
extern void intA7(void);
extern void intA8(void);
extern void intA9(void);
extern void intAA(void);
extern void intAB(void);
extern void intAC(void);
extern void intAD(void);
extern void intAE(void);
extern void intAF(void);
extern void intB0(void);
extern void intB1(void);
extern void intB2(void);
extern void intB3(void);
extern void intB4(void);
extern void intB5(void);
extern void intB6(void);
extern void intB7(void);
extern void intB8(void);
extern void intB9(void);
extern void intBA(void);
extern void intBB(void);
extern void intBC(void);
extern void intBD(void);
extern void intBE(void);
extern void intBF(void);
extern void intC0(void);
extern void intC1(void);
extern void intC2(void);
extern void intC3(void);
extern void intC4(void);
extern void intC5(void);
extern void intC6(void);
extern void intC7(void);
extern void intC8(void);
extern void intC9(void);
extern void intCA(void);
extern void intCB(void);
extern void intCC(void);
extern void intCD(void);
extern void intCE(void);
extern void intCF(void);
extern void intD0(void);
extern void intD1(void);
extern void intD2(void);
extern void intD3(void);
extern void intD4(void);
extern void intD5(void);
extern void intD6(void);
extern void intD7(void);
extern void intD8(void);
extern void intD9(void);
extern void intDA(void);
extern void intDB(void);
extern void intDC(void);
extern void intDD(void);
extern void intDE(void);
extern void intDF(void);
extern void intE0(void);
extern void intE1(void);
extern void intE2(void);
extern void intE3(void);
extern void intE4(void);
extern void intE5(void);
extern void intE6(void);
extern void intE7(void);
extern void intE8(void);
extern void intE9(void);
extern void intEA(void);
extern void intEB(void);
extern void intEC(void);
extern void intED(void);
extern void intEE(void);
extern void intEF(void);
extern void intF0(void);
extern void intF1(void);
extern void intF2(void);
extern void intF3(void);
extern void intF4(void);
extern void intF5(void);
extern void intF6(void);
extern void intF7(void);
extern void intF8(void);
extern void intF9(void);
extern void intFA(void);
extern void intFB(void);
extern void intFC(void);
extern void intFD(void);
extern void intFE(void);
extern void intFF(void);

void (*idtRequestDispatchers[256])(void) = {
    int0,int1,int2,int3,int4,int5,int6,int7,
    int8,int9,intA,intB,intC,intD,intE,NULL,   // 0x0–0xF, skip 0xF
    int10,int11,int12,int13,int14,int15,int16,int17,
    int18,int19,int1A,int1B,int1C,int1D,int1E,int1F,
    int20,int21,int22,int23,int24,int25,int26,int27,
    int28,int29,int2A,int2B,int2C,int2D,int2E,int2F,
    int30,int31,int32,int33,int34,int35,int36,int37,
    int38,int39,int3A,int3B,int3C,int3D,int3E,int3F,
    int40,int41,int42,int43,int44,int45,int46,int47,
    int48,int49,int4A,int4B,int4C,int4D,int4E,int4F,
    int50,int51,int52,int53,int54,int55,int56,int57,
    int58,int59,int5A,int5B,int5C,int5D,int5E,int5F,
    int60,int61,int62,int63,int64,int65,int66,int67,
    int68,int69,int6A,int6B,int6C,int6D,int6E,int6F,
    int70,int71,int72,int73,int74,int75,int76,int77,
    int78,int79,int7A,int7B,int7C,int7D,int7E,int7F,
    int80,int81,int82,int83,int84,int85,int86,int87,
    int88,int89,int8A,int8B,int8C,int8D,int8E,int8F,
    int90,int91,int92,int93,int94,int95,int96,int97,
    int98,int99,int9A,int9B,int9C,int9D,int9E,int9F,
    intA0,intA1,intA2,intA3,intA4,intA5,intA6,intA7,
    intA8,intA9,intAA,intAB,intAC,intAD,intAE,intAF,
    intB0,intB1,intB2,intB3,intB4,intB5,intB6,intB7,
    intB8,intB9,intBA,intBB,intBC,intBD,intBE,intBF,
    intC0,intC1,intC2,intC3,intC4,intC5,intC6,intC7,
    intC8,intC9,intCA,intCB,intCC,intCD,intCE,intCF,
    intD0,intD1,intD2,intD3,intD4,intD5,intD6,intD7,
    intD8,intD9,intDA,intDB,intDC,intDD,intDE,intDF,
    intE0,intE1,intE2,intE3,intE4,intE5,intE6,intE7,
    intE8,intE9,intEA,intEB,intEC,intED,intEE,intEF,
    intF0,intF1,intF2,intF3,intF4,intF5,intF6,intF7,
    intF8,intF9,intFA,intFB,intFC,intFD,intFE,intFF
};
void commonHandler(uint32_t* stack);

void defaultFun(uint32_t* stack) {
    kprint("Uninitialized interrupt ");
    kprint_hex(stack[12]);
    kprint(" with (potential) error code ");
    kprint_hex(stack[13]);
}

void setInterruptDispatcher(uint8_t intNum, uint32_t addr, uint16_t segSelector, uint8_t gateType, uint8_t dpl);

void setupInterruptStructures() {
    // dispatcherFunAddrs[0] = (uint32_t) int0;
    // dispatcherFunAddrs[1] = (uint32_t) int1;

    for (int i = 0; i < 256; i++) {
        if (i == 0x0F) continue; // 0x0F is reserved
        setInterruptDispatcher(i, (uint32_t)(idtRequestDispatchers[i]), 0x08, 0xE, 0x00);
        interruptRequestHandlers[i] = defaultFun;
    }

    idtr = (struct IdtrStruct) {.size = (256 * sizeof(struct InterruptDescriptor) - 1), .idtVAddr = (uint32_t)&idt};
}

void setInterruptDispatcher(uint8_t intNum, uint32_t addr, uint16_t segSelector, uint8_t gateType, uint8_t dpl) {
    struct InterruptDescriptor intDesc = {
        .offset = (uint16_t)(addr & 0xFFFF),
        .segSelector = segSelector,
        .reserved = 0x00,
        .gateType = gateType,
        .zero = 0b0,
        .dpl = dpl,
        .present = 1,
        .offset2 = (uint16_t)((addr & 0xFFFF0000) >> 16)
    };

    idt[intNum] = intDesc;
}

void commonHandler(uint32_t* stack) {
    interruptRequestHandlers[stack[12]](stack);
}

void enableInterrupts() {
    asm volatile (
        "lidt %0\n\t"
        "sti"
        :
        : "m" (idtr)
        : 
    );
}

void setIDTHandler(int isrNum, int handlerAddr) {
    interruptRequestHandlers[isrNum] = (void(*)(uint32_t*)) handlerAddr;
}