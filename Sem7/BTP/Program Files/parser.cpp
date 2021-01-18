#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/Support/SourceMgr.h>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
using namespace std;
using namespace llvm;
typedef long long int ll;

namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        Code code;
    public:
        explicit Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}
using namespace Color;
Modifier red(FG_RED);
Modifier green (FG_GREEN);
Modifier blue (FG_BLUE);
Modifier def(FG_DEFAULT);

ofstream output, debug, simpleC;

void generateSimpleC(unique_ptr<Module>& module){

}

ll labelCount = 0, varCount = 0;
unordered_map<string,ll> labelMap;
unordered_map<string, string> boolCondMap;
bool isFirstInstr, instrHandled, instrPrints;

static string getSimpleNodeLabel(const BasicBlock* node) {
    if (!node->getName().empty()) return node->getName().str();
    string Str;
    raw_string_ostream OS(Str);
    node->printAsOperand(OS, false);
    string name;
    if(name.size() > 0 && name[0] != '%') return name;
    if(labelMap.count(OS.str()) == 0) labelMap[OS.str()] = labelCount++;
//    return OS.str();
    return "q" + to_string(labelMap[OS.str()]);
}
static string getSimpleOperandName(const Value* node) {
    if (!node->getName().empty()) return node->getName().str();
    string Str, name;
    raw_string_ostream OS(Str);
    node->printAsOperand(OS, false);
    name = OS.str();

    if(name.size() > 0 && name[0] != '%') return name;
    if(labelMap.count(OS.str()) == 0) {
        labelMap[OS.str()] = labelCount++;
    }
//    return OS.str();
    return "q" + to_string(labelMap[OS.str()]);
}

string getPredicate(CmpInst::Predicate pred){
    string ans;
    switch(pred){
        case CmpInst::FCMP_FALSE:
            break;
        case CmpInst::FCMP_OEQ:
            break;
        case CmpInst::FCMP_OGT:
            break;
        case CmpInst::FCMP_OGE:
            break;
        case CmpInst::FCMP_OLT:
            break;
        case CmpInst::FCMP_OLE:
            break;
        case CmpInst::FCMP_ONE:
            break;
        case CmpInst::FCMP_ORD:
            break;
        case CmpInst::FCMP_UNO:
            break;
        case CmpInst::FCMP_UEQ:
            break;
        case CmpInst::FCMP_UGT:
            break;
        case CmpInst::FCMP_UGE:
            break;
        case CmpInst::FCMP_ULT:
            break;
        case CmpInst::FCMP_ULE:
            break;
        case CmpInst::FCMP_UNE:
            break;
        case CmpInst::FCMP_TRUE:
            break;
        case CmpInst::BAD_FCMP_PREDICATE:
            break;
        case CmpInst::ICMP_EQ: ans = "==";
            break;
        case CmpInst::ICMP_NE: ans = "!=";
            break;
        case CmpInst::ICMP_UGT:
        case CmpInst::ICMP_SGT: ans = ">";
            break;
        case CmpInst::ICMP_UGE:
        case CmpInst::ICMP_SGE: ans = ">=";
            break;
        case CmpInst::ICMP_ULT:
        case CmpInst::ICMP_SLT: ans = "<";
            break;
        case CmpInst::ICMP_ULE:
        case CmpInst::ICMP_SLE: ans = ">=";
            break;
        case CmpInst::BAD_ICMP_PREDICATE: ans = "??";
            break;
    }
    return ans;
}
string getLHS(Instruction* instIter){
    string resultName;
    if (!instIter->getName().empty()) resultName.assign(instIter->getName().str());
    else{
        string Str;
        raw_string_ostream OS(Str);
        instIter->printAsOperand(OS, false);
        resultName.assign(OS.str());
    }
    if(labelMap.count(resultName) == 0) labelMap[resultName] = labelCount++;
    return "q" + to_string(labelMap[resultName]);
}

int isFinalState(BasicBlock* bbIter){
    if(bbIter == nullptr) return true;
    BasicBlock &basicBlock = *bbIter;
    for (auto & instIter : basicBlock) {
        if(instIter.getOpcode() == Instruction::Br) return false;
    }
    return true;
}


//special cases
void handleBr(Instruction *instIter){
    if(instIter == nullptr) return;
    instrHandled = true;
    if(instIter->getNumOperands() == 1) {
        output << " " << getSimpleOperandName(instIter->getOperand(0));
        return;
    }
    else{
        output << " q" << labelCount << ";\n";
        output << "q" << labelCount++ << " 2 ";
        string lhs = getSimpleOperandName(instIter->getOperand(0));
        if(boolCondMap.count(lhs) == 0) return;
        output  << boolCondMap[lhs] << " | - " << getSimpleOperandName(instIter->getOperand(1)) << "\n";
        int intStateLength = labelCount > 1 ? (ll)log10(labelCount-1) + 1 : 1;
        for(int i = 0 ; i < intStateLength; i++) output << " ";
        output << "    !" << boolCondMap[lhs]  << " | - " << getSimpleOperandName(instIter->getOperand(2));
    }
}
void handleStore(Instruction *instIter){
    if(!instIter) return;
    if(!isFirstInstr) output << ", ";
    else isFirstInstr = false;
    output << getSimpleOperandName(instIter->getOperand(1)) << " = " << getSimpleOperandName(instIter->getOperand(0));
    instrHandled = true;
    instrPrints = true;
}
void handleLoad(Instruction *instIter){
    if(!instIter) return;
    string operandName = getSimpleOperandName(instIter->getOperand(0));
    if(!isFirstInstr) output << ", ";
    else isFirstInstr = false;
    output << getLHS(instIter) << " = " << operandName;
    instrHandled = true;
    instrPrints = true;
}
void handleCmp(Instruction* instIter){
    if(instIter == nullptr) return;
    string resultName = getLHS(instIter);
    auto pred = CmpInst::Predicate::BAD_ICMP_PREDICATE;
    if (auto *cmpInst = dyn_cast<CmpInst>(instIter)) pred = cmpInst->getPredicate();
    string rhs = "(" + getSimpleOperandName(instIter->getOperand(0)) + " " + getPredicate(pred) + " " + getSimpleOperandName(instIter->getOperand(1)) + ")";
    boolCondMap[resultName] = rhs;
    instrHandled = true;
}

//print instruction based on type of opCode
void handleBinaryOps(Instruction* instIter, int opCode){
    string oper = "??";
    switch(opCode){
        case Instruction::BinaryOpsBegin:
        case Instruction::FAdd: oper = "+";
            break;
        case Instruction::Sub:
        case Instruction::FSub: oper = "-";
            break;
        case Instruction::Mul:
        case Instruction::FMul: oper = "*";
            break;
        case Instruction::UDiv:
        case Instruction::SDiv:
        case Instruction::FDiv: oper = "/";
            break;
        case Instruction::URem:
        case Instruction::SRem:
        case Instruction::FRem: oper = "%";
            break;
        case Instruction::Shl: oper = "<<";
            break;
        case Instruction::LShr:
        case Instruction::AShr: oper = ">>";
            break;
        case Instruction::And: oper = "&";
            break;
        case Instruction::Or: oper = "|";
            break;
        case Instruction::Xor: oper = "^";
            break;
        case Instruction::BinaryOpsEnd:
        default:
            oper = "??";
            break;
    }
    if(oper != "??"){
        if(!isFirstInstr) output << ", ";
        else isFirstInstr = false;
        if(instIter) output << getLHS(instIter) << " = (" << getSimpleOperandName(instIter->getOperand(0)) << " " << oper << " " << getSimpleOperandName(instIter->getOperand(1)) << ")";
        instrHandled = true;
        instrPrints = true;
    }
}
void handleUnaryOps(Instruction* instIter, int opCode){
    switch(opCode){
        case Instruction::UnaryOpsBegin:
        case Instruction::UnaryOpsEnd:
        default:
            break;
    }
}
void handleTermOps(Instruction* instIter, int opCode){
    switch(opCode){
        case Instruction::TermOpsBegin:
            break;
        case Instruction::Br:
            handleBr(instIter);
            break;
        case Instruction::Switch:
        case Instruction::IndirectBr:
        case Instruction::Invoke:
        case Instruction::Resume:
        case Instruction::Unreachable:
        case Instruction::CleanupRet:
        case Instruction::CatchRet:
        case Instruction::CatchSwitch:
        case Instruction::Call:
        case Instruction::TermOpsEnd:
        default:
            break;
    }
}
void handleMemoryOps(Instruction* instIter, int opCode){
    switch(opCode){
        case Instruction::MemoryOpsBegin:
            break;
        case Instruction::Load:
            handleLoad(instIter);
            break;
        case Instruction::Store:
            handleStore(instIter);
            break;
        case Instruction::GetElementPtr:
        case Instruction::Fence:
        case Instruction::AtomicCmpXchg:
        case Instruction::AtomicRMW:
        case Instruction::MemoryOpsEnd:
        default:
            break;
    }
}
void handleCastOps(Instruction* instIter, int opCode){
    switch (opCode){
        case Instruction::CastOpsBegin:
        case Instruction::ZExt:
        case Instruction::SExt:
        case Instruction::FPToUI:
        case Instruction::FPToSI:
        case Instruction::UIToFP:
        case Instruction::SIToFP:
        case Instruction::FPTrunc:
        case Instruction::FPExt:
        case Instruction::PtrToInt:
        case Instruction::IntToPtr:
        case Instruction::BitCast:
        case Instruction::AddrSpaceCast:
        case Instruction::CastOpsEnd:
        default:
            break;
    }
}
void handleFuncletPadOps(Instruction* instIter, int opCode){
    switch(opCode){
        case Instruction::FuncletPadOpsBegin:
        case Instruction::CatchPad:
        case Instruction::FuncletPadOpsEnd:
        default:
            break;
    }
}
void handleOtherOps(Instruction* instIter, int opCode){
    switch(opCode){
        case Instruction::ICmp:
        case Instruction::FCmp:
            handleCmp(instIter);
            break;
        case Instruction::PHI:
        case Instruction::Call:
        case Instruction::Select:
        case Instruction::UserOp1:
        case Instruction::UserOp2:
        case Instruction::VAArg:
        case Instruction::ExtractElement:
        case Instruction::InsertElement:
        case Instruction::ShuffleVector:
        case Instruction::ExtractValue:
        case Instruction::InsertValue:
        case Instruction::LandingPad:
        case Instruction::OtherOpsEnd:
        default:
            break;
    }
}

static inline bool isMemoryOp(unsigned OpCode) {
    return OpCode >= Instruction::MemoryOpsBegin && OpCode < Instruction::MemoryOpsEnd;
}
static inline bool isTermOp(unsigned OpCode) {
    return OpCode >= Instruction::TermOpsBegin && OpCode < Instruction::TermOpsEnd;
}
static inline bool isOtherOp(unsigned OpCode) {
    return OpCode >= Instruction::OtherOpsBegin && OpCode < Instruction::OtherOpsEnd;
}

void handleInstruction(Instruction* instIter){
    if(instIter == nullptr) {
        debug << "\t\tinstIter is nullptr" << endl;
        cout << red << "\t\tinstIter is nullptr" << def << endl;
        return;
    }
    auto opCode = instIter->getOpcode();

    if(Instruction::isBinaryOp(opCode)) handleBinaryOps(instIter, opCode);
    else if(Instruction::isUnaryOp(opCode)) handleUnaryOps(instIter, opCode);
    else if(Instruction::isCast(opCode)) handleCastOps(instIter, opCode);
    else if(Instruction::isFuncletPad(opCode)) handleFuncletPadOps(instIter, opCode);
    else if(isTermOp(opCode)) handleTermOps(instIter, opCode);
    else if(isMemoryOp(opCode)) handleMemoryOps(instIter, opCode);
    else if(isOtherOp(opCode)) handleOtherOps(instIter, opCode);

    debug << "\t\tInstr: " << instIter->getOpcodeName() << endl;
    if(instrHandled) cout << "\t\tInstr: " << green << instIter->getOpcodeName() << def << endl;
    else cout << "\t\tInstr: " << red << instIter->getOpcodeName() << def << endl;
}
void handleBasicBlock(BasicBlock* bbIter){
    if(bbIter == nullptr) {
        debug << "\tbbIter is nullptr" << endl;
        cout << red << "\tbbIter is nullptr" << def << endl;
        return;
    }
    string bbName = getSimpleNodeLabel(bbIter);
    debug << "\tEntering Basic Block: " << bbName << endl;
    cout << "\tEntering Basic Block: " << blue << bbName << def << endl;

    bool isFinal = isFinalState(bbIter);
    int branches = isFinal ? 0 : 1;

    debug << "\t\tNo of branches : " << branches << endl;
    cout << "\t\tNo of branches : " << branches << endl;

    output << getSimpleNodeLabel(bbIter) << " " << branches;
    if(!isFinal) output << " - | ";

    isFirstInstr = true;
    bool noInstr = true;
    for(auto &inst : *bbIter){
        instrHandled = false;
        instrPrints = false;
        if(noInstr && inst.getOpcode() == Instruction::Br) output << "-";
        handleInstruction(&inst);
        if(instrPrints) noInstr = false;
    }
    output << " ;" << endl;

    debug << "\tExiting Basic Block: " << bbName << endl << endl;
    cout << "\tExiting Basic Block: " << blue << bbName << def << endl << endl;
}
void generateFSMD(unique_ptr<Module>& module){
    output << "\"FSMD.txt\"\n";
    if(!module) return;
    for(auto &function : module->getFunctionList()){
        debug << "Entering Function: " << function.getName().str() << endl;
        cout << "Entering Function: " << blue << function.getName().str() << def << endl;
        for(auto &basicBlock : function.getBasicBlockList()){
            handleBasicBlock(&basicBlock);
        }
        debug << "Exiting Function: " << function.getName().str() << endl << endl;
        cout << "Exiting Function: " << blue << function.getName().str() << def << endl << endl;
    }
}

void printPattern(unique_ptr<Module>& module){

}

int main(int argc, char **argv){

    if(argc < 2){
        cout << "Usage: " << red << "./a.out <IR file>\n";
        return 1;
    }
    output.open("FSMD.txt");
    debug.open("debug.txt");
    simpleC.open("simpleC.txt");
    SMDiagnostic err;
    LLVMContext context;
    unique_ptr<Module> module(parseIRFile(argv[1], err, context));
    printPattern(module);
    generateFSMD(module);
    output.close();
    debug.close();
    simpleC.close();
    return 0;
}