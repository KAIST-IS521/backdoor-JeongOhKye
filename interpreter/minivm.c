//-----------------------------------------------------------------------------
// minivm.c -- The basic definitions for the vm.
// Copyright (C) Philip Conrad 4/30/2013 @ 3:56 PM -- MIT License
//
//-----------------------------------------------------------------------------

#include "minivm.h"
#include <stdbool.h>
#include <stdio.h>

extern bool is_running;

//---------------------------------------------------------
// FUNCTION IMPLEMENTATIONS:
void halt(struct VMContext* ctx, const uint32_t instr){
    is_running = false;
}

void load(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr); 
    uint8_t reg2 = EXTRACT_B2(instr);
    uint32_t reg2value = ctx->r[reg2].value;

    ctx->r[reg1].value = (uint32_t)ctx->Memory[reg2value];
}

void store(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr); 
    uint8_t reg2 = EXTRACT_B2(instr);
    uint32_t reg1value = ctx->r[reg1].value;
    
    ctx->Memory[reg1value] = (uint8_t)ctx->r[reg2].value;
}

void move(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr); 
    uint8_t reg2 = EXTRACT_B2(instr);
    
    ctx->r[reg1].value = ctx->r[reg2].value;
}

void puti(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr); 
    uint8_t imm1 = EXTRACT_B2(instr);
    
    ctx->r[reg1].value = (uint32_t)imm1;
}

void add(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr); 
    uint8_t reg2 = EXTRACT_B2(instr);
    uint8_t reg3 = EXTRACT_B3(instr);
    
    ctx->r[reg1].value = ctx->r[reg2].value + ctx->r[reg3].value;
}

void sub(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr); 
    uint8_t reg2 = EXTRACT_B2(instr);
    uint8_t reg3 = EXTRACT_B3(instr);
    
    ctx->r[reg1].value = ctx->r[reg2].value - ctx->r[reg3].value;
}

void gt(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr); 
    uint8_t reg2 = EXTRACT_B2(instr);
    uint8_t reg3 = EXTRACT_B3(instr);
    
    if(ctx->r[reg2].value > ctx->r[reg3].value)
        ctx->r[reg1].value = 1; 
    else
        ctx->r[reg1].value = 0;
}

void ge(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr); 
    uint8_t reg2 = EXTRACT_B2(instr);
    uint8_t reg3 = EXTRACT_B3(instr);
    
    if(ctx->r[reg2].value >= ctx->r[reg3].value)
        ctx->r[reg1].value = 1; 
    else
        ctx->r[reg1].value = 0;
}

void eq(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr); 
    uint8_t reg2 = EXTRACT_B2(instr);
    uint8_t reg3 = EXTRACT_B3(instr);
    
    if(ctx->r[reg2].value == ctx->r[reg3].value)
        ctx->r[reg1].value = 1; 
    else
        ctx->r[reg1].value = 0;
}

void ite(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr); 
    uint8_t addr1 = EXTRACT_B2(instr);
    uint8_t addr2 = EXTRACT_B3(instr);
    
    if(ctx->r[reg1].value > 0)
        ctx->pc = addr1;
    else
        ctx->pc = addr2;
}

void jump(struct VMContext* ctx, const uint32_t instr){
    uint8_t addr1= EXTRACT_B1(instr); 
    ctx->pc = addr1;
}

void _puts(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr);
    uint32_t reg1value = ctx->r[reg1].value;
    char c;
    while(true){
        if(reg1value > DEFAULT_HEAP_SIZE){
            perror("Memory Access Out of Bound\n");
            is_running = false;
            return;
        }
        c = ctx->Memory[reg1value];
        if(c == 0)
            break;
        putchar(c);
        reg1value +=1;
    }
}

void _gets(struct VMContext* ctx, const uint32_t instr){
    uint8_t reg1 = EXTRACT_B1(instr);
    uint32_t reg1value = ctx->r[reg1].value;
    char c;
    while(true){
        if(reg1value > DEFAULT_HEAP_SIZE){
            perror("Memory Access Out of Bound\n");
            is_running = false;
            return;
        }
        c = getchar(); 
        if(c == 10)
            break;
        ctx->Memory[reg1value] = c;
        reg1value += 1;
    }
    //Set zero to final
    ctx->Memory[reg1value] = 0;
}


// Defers decoding of register args to the called function.
// dispatch :: VMContext -> uint32_t -> Effect()
void dispatch(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t i = EXTRACT_B0(instr);
    (*ctx->funtable[i])(ctx, instr);
}


// Initializes a VMContext in-place.
// initVMContext :: VMContext -> uint32_t -> uint32_t -> [Reg] -> [FunPtr] -> Effect()
void initVMContext(struct VMContext* ctx, const uint32_t numRegs, const uint32_t numFuns, Reg* registers, FunPtr* funtable, uint32_t* code, uint32_t codesize) {
    ctx->numRegs    = numRegs;
    ctx->numFuns    = numFuns;
    ctx->r          = registers;
    ctx->funtable   = funtable;
    ctx->code       = code;
    ctx->codesize   = codesize;
    //Initial Program Counter is 0
    ctx->pc         = 0;
}


// Reads an instruction, executes it, then steps to the next instruction.
// stepVMContext :: VMContext -> uint32_t** -> Effect()
void stepVMContext(struct VMContext* ctx) {
    // Read a 32-bit bytecode instruction.
    uint32_t instr;
    instr = ctx->code[ctx->pc]; 

    // Increment to next instruction.
    ctx->pc++;

    // Dispatch to an opcode-handler.
    dispatch(ctx, instr);
}

